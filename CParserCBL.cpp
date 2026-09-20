/*-------------------------------------------------------------------------------------

	CParserCBL.cpp

-------------------------------------------------------------------------------------*/
#include "CAnaCBL.h"

// ｺﾝｽﾄﾗｸﾀ
CParserCBL::CParserCBL()
 : m_dwTotalSize(0), m_unMaxLevel(0), m_unMinLevel(999), m_nWordCheck(0)
{
#ifdef _DEBUG_MODE_
	printf("CParserCBL コンストラクタ\n");
#endif	//_DEBUG_MODE_
	m_lpRec_cbl.clear();
}

// ﾃﾞｽﾄﾗｸﾀ
CParserCBL::~CParserCBL()
{
#ifdef _DEBUG_MODE_
	printf("CParserCBL デストラクタ\n");
#endif	//_DEBUG_MODE_
	ClearRecord();
}

// レコード格納配列のクリア
void CParserCBL::ClearRecord()
{
	for(std::vector<LPRECORD_CBL>::size_type i = 0; i < m_lpRec_cbl.size(); i++){
		if(m_lpRec_cbl[i]){
			if(GlobalFree(m_lpRec_cbl[i]) != NULL) AddTrace("ﾒﾓﾘ開放に失敗", TRACE_ALLOC);
		}
	}
	m_lpRec_cbl.clear();
}

// レコード配列の追加
void CParserCBL::PushRecord(LPRECORD_CBL lpRecCbl)
{
	if(lpRecCbl) m_lpRec_cbl.push_back(lpRecCbl);
}

// 意味解析
DWORD CParserCBL::Parser(std::vector<LPTOKEN_CBL> &m_lpTok_cbl,
						 const int nCompPos/* = 0*/,
						 const BOOL bWordCheck/* = TRUE*/)
{
	// 初期化
	InitAnalysys(STAGE_PARSER);

	// レコード格納配列のクリア
	ClearRecord();

	// 最大、最小レベル番号
	m_unMaxLevel = 0;
	m_unMinLevel = 999;

	DWORD dwStTime = timeGetTime();

	AddTrace("意味解析開始...");

	m_nComp = max(nCompPos, 0);
	LPRECORD_CBL lpRecCbl;

	if(!(lpRecCbl = (LPRECORD_CBL)GlobalAlloc(GPTR, sizeof(RECORD_CBL)))){
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		Cancel();
		return 0;
	}

	std::vector<LPTOKEN_CBL>::size_type i = 0;
	int nRecCount = 0;

	AddTrace("ソースコード展開");

	for(i = 0; i < m_lpTok_cbl.size(); i++){

		if(IsCancel()) return 0;

		// ファイル終端
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_EOF) break;

		// 項目の追加
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_LEVEL){
			lpRecCbl->nPhyLine = m_lpTok_cbl[i]->nPhyLine;
			lpRecCbl->nLogLine = m_lpTok_cbl[i]->nLogLine;
			lpRecCbl->unLevel = (unsigned int)atol(m_lpTok_cbl[i]->szDescriptor);
			if(m_unMaxLevel < lpRecCbl->unLevel) m_unMaxLevel = lpRecCbl->unLevel;
			if(m_unMinLevel > lpRecCbl->unLevel) m_unMinLevel = lpRecCbl->unLevel;
		}else

		// 変数名
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_DESCRIPT){
			if(strlen(lpRecCbl->szDescriptor) < 1){
//				strcpy(lpRecCbl->szDescriptor, m_lpTok_cbl[i]->szDescriptor);
				strcpy_s(lpRecCbl->szDescriptor, sizeof(lpRecCbl->szDescriptor), m_lpTok_cbl[i]->szDescriptor);
			}else{
				if(!lpRecCbl->bOccurs) lpRecCbl->nSupplementation = SUP_ERRCUT;
			}
		}else

		// 変数サイズ計算
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_PICSIZE){
//			strcpy(lpRecCbl->szTypeDef, m_lpTok_cbl[i]->szDescriptor);
			strcpy_s(lpRecCbl->szTypeDef, sizeof(lpRecCbl->szTypeDef), m_lpTok_cbl[i]->szDescriptor);
			// 変数型
			DWORD dwPicType = GetTypeOf(m_lpTok_cbl[i]);
			lpRecCbl->dwType |= dwPicType;
			// 変数サイズ
			lpRecCbl->dwTypeSize = GetSizeOf(m_lpTok_cbl[i], dwPicType, &(lpRecCbl->phyd));
			// 符号が１Ｂｙｔｅ使う変数
			if((lpRecCbl->dwType & DES_TYPE_LSEPARATE)
			|| (lpRecCbl->dwType & DES_TYPE_TSEPARATE)) lpRecCbl->dwTypeSize++;
		}else

		// 符号が1Byte使用する変数
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_LSEPARATE
		|| m_lpTok_cbl[i]->btType == CBL_TYPE_TSEPARATE){
			// 型情報の追加
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_LSEPARATE){
				lpRecCbl->dwType |= DES_TYPE_LSEPARATE;
//				strcpy(lpRecCbl->szValueEx, "LEADING SEPARATE");
				strcpy_s(lpRecCbl->szValueEx, sizeof(lpRecCbl->szValueEx), "LEADING SEPARATE");
			}else{
				lpRecCbl->dwType |= DES_TYPE_TSEPARATE;
//				strcpy(lpRecCbl->szValueEx, "TRAILING SEPARATE");
				strcpy_s(lpRecCbl->szValueEx, sizeof(lpRecCbl->szValueEx), "TRAILING SEPARATE");
			}
			// この時点で変数サイズが求めてあれば変数サイズを+１
			if(lpRecCbl->dwType & DES_TYPE_SIGN) lpRecCbl->dwTypeSize++;
		}else

		// 再定義項目
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_REDEFINES){
			i++;
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_DESCRIPT){
//				strcpy(lpRecCbl->szRedefines, m_lpTok_cbl[i]->szDescriptor);
				strcpy_s(lpRecCbl->szRedefines, sizeof(lpRecCbl->szRedefines), m_lpTok_cbl[i]->szDescriptor);
				lpRecCbl->bRedefines = TRUE;
			}
		}else

		// 繰り返し項目
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_OCCURS){
			i++;
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_NUMCONST){
				lpRecCbl->nOccurs = atoi(m_lpTok_cbl[i]->szDescriptor);
				lpRecCbl->bOccurs = TRUE;
			}
		}else

		// 繰り返し項目の指数
		if((m_lpTok_cbl[i]->btType == CBL_TYPE_INDEX)
		&& (m_lpTok_cbl[i + 1]->btType == CBL_TYPE_SUB)){
			i += 2;
//			if(m_lpTok_cbl[i]->btType == CBL_TYPE_DESCRIPT) strcpy(lpRecCbl->szOccursIndexed, m_lpTok_cbl[i]->szDescriptor);
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_DESCRIPT) strcpy_s(lpRecCbl->szOccursIndexed, sizeof(lpRecCbl->szOccursIndexed), m_lpTok_cbl[i]->szDescriptor);
		}else

		// ユーザ定義項目
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_VALUE){
			i++;
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_CHARCONST){
//				strcpy(lpRecCbl->szValue, m_lpTok_cbl[i]->szDescriptor);
				strcpy_s(lpRecCbl->szValue, sizeof(lpRecCbl->szValue), m_lpTok_cbl[i]->szDescriptor);
			}else
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_JPNCONST){
				wsprintf(lpRecCbl->szValue, "%s (NC)", m_lpTok_cbl[i]->szDescriptor);
			}else
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_BINCONST){
				wsprintf(lpRecCbl->szValue, "%s (%d)",
					m_lpTok_cbl[i]->szDescriptor, Hex2Dwrod(m_lpTok_cbl[i]->szDescriptor));
			}else
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_NUMCONST){
//				strcpy(lpRecCbl->szValue, m_lpTok_cbl[i]->szDescriptor);
				strcpy_s(lpRecCbl->szValue, sizeof(lpRecCbl->szValue), m_lpTok_cbl[i]->szDescriptor);
			}
		}else

		// ビットサイズ取得
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_BIT){
			lpRecCbl->btCompress = COMP_BIT;
//			strcpy(lpRecCbl->szCompress, m_lpTok_cbl[i]->szDescriptor);
			strcpy_s(lpRecCbl->szCompress, sizeof(lpRecCbl->szCompress), m_lpTok_cbl[i]->szDescriptor);
			lpRecCbl->dwTypeSize = GetSizeOfBit(lpRecCbl);
		}else

		// ２進変数
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_BINARY){
			lpRecCbl->btCompress = COMP_BINARY;
//			strcpy(lpRecCbl->szCompress, m_lpTok_cbl[i]->szDescriptor);
			strcpy_s(lpRecCbl->szCompress, sizeof(lpRecCbl->szCompress), m_lpTok_cbl[i]->szDescriptor);
			lpRecCbl->dwType |= DES_TYPE_BIN;
			lpRecCbl->dwTypeSize = GetSizeOfBinary(lpRecCbl);
		}else

		// １６進変数
		if(m_lpTok_cbl[i]->btType == CBL_TYPE_PACKED){
			lpRecCbl->btCompress = COMP_DECIMAL;
//			strcpy(lpRecCbl->szCompress, m_lpTok_cbl[i]->szDescriptor);
			strcpy_s(lpRecCbl->szCompress, sizeof(lpRecCbl->szCompress), m_lpTok_cbl[i]->szDescriptor);
			lpRecCbl->dwType |= DES_TYPE_BIN;
			lpRecCbl->dwTypeSize = GetSizeOfHex(lpRecCbl);
		}else

		// 追加する
		if((m_lpTok_cbl[i]->btType == CBL_TYPE_EOL)
		|| (m_lpTok_cbl[i]->btType == CBL_TYPE_WEOL)){
			// ソース番号
			lpRecCbl->nSrcNo = m_lpTok_cbl[i]->nSrcNo;
			// グループ番号
			lpRecCbl->nStructGroupIndex = -1;	// 先頭は－１
			// ピリオド保管
			if(m_lpTok_cbl[i]->btType == CBL_TYPE_WEOL) lpRecCbl->nSupplementation = SUP_PERIOD;
			// 変数名の定義なし
			if(strlen(lpRecCbl->szDescriptor) < 1){
				strcpy_s(lpRecCbl->szDescriptor, sizeof(lpRecCbl->szDescriptor), "[No Definition]");
				AddTrace("変数名の定義がされていない変数があります[No Definition]として続行します", TRACE_INFO);
			}
			// コメントはEOL後に来るのでムリに押し込む
			for(std::vector<LPTOKEN_CBL>::size_type c = i; c < m_lpTok_cbl.size(); c++){
				if(m_lpTok_cbl[c]->btType == CBL_TYPE_LEVEL) break;
				if(m_lpTok_cbl[c]->btType == CBL_TYPE_COMMENT){
//					strcpy(lpRecCbl->szComment, m_lpTok_cbl[c]->szDescriptor);
					strcpy_s(lpRecCbl->szComment, sizeof(lpRecCbl->szComment), m_lpTok_cbl[c]->szDescriptor);
					break;
				}
			}
			// 構造体
			if(lpRecCbl->dwTypeSize == 0){
				lpRecCbl->bStruct = TRUE;
			}else{
				lpRecCbl->bStruct = FALSE;
			}
			// 追加
//			m_lpRec_cbl.push_back(lpRecCbl);
			PushRecord(lpRecCbl);
			// 新しい構造体メモリ確保
			if(!(lpRecCbl = (LPRECORD_CBL)GlobalAlloc(GPTR, sizeof(RECORD_CBL)))){
				AddTrace("ﾒﾓﾘ不足が発生しています", TRACE_ALLOC);
				Cancel();
				return 0;
			}
			nRecCount++;
		}
	}

	// 配列定義項目の展開
	m_nOccursCount = DevOccurs();
	if(IsCancel()) return 0;

	// 再定義項目の展開
	m_nRedefinesCount = DevRedefines();
	if(IsCancel()) return 0;

	// 集団項目の展開
	m_nStructCount = DevStruct();
	if(IsCancel()) return 0;

	// 変数バイト位置計算(と合計サイズ計算)
	m_dwTotalSize = GetPosOf();
	if(IsCancel()) return 0;

	// 構造体サイズ取得
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		if(m_lpRec_cbl[i]->bStruct) m_lpRec_cbl[i]->dwTypeSize = GetStructSizeOf(i);
	}
	if(IsCancel()) return 0;

	// ＣＯＢＯＬ文法的な問題チェック
	if(bWordCheck){
		if(CobolWordCheck() < 0) Cancel();
		if(IsCancel()) return 0;
	}

	AddTrace("意味解析完了");

	// 終了
	EndAnalysys();

	return (timeGetTime() - dwStTime);
}

// 変数型取得
DWORD CParserCBL::GetTypeOf(LPTOKEN_CBL lptok)
{
	DWORD dwType = 0;

	// 変数型を判定
	if(lptok->szDescriptor[0] == '9'){
		dwType = DES_TYPE_DEC | DES_TYPE_UNSIGN;
	}else
	if(lptok->szDescriptor[0] == '1'){
		dwType = DES_TYPE_DEC | DES_TYPE_UNSIGN;
	}else
	if(lptok->szDescriptor[0] == 'S' || lptok->szDescriptor[0] == 's'){
		dwType = DES_TYPE_DEC | DES_TYPE_SIGN;
	}else
	if(lptok->szDescriptor[0] == 'Z' || lptok->szDescriptor[0] == 'z'){
		dwType = DES_TYPE_DEC | DES_TYPE_UNSIGN;
	}else
	if(lptok->szDescriptor[0] == 'X' || lptok->szDescriptor[0] == 'x'){
		dwType = DES_TYPE_CHAR;
	}else
	if(lptok->szDescriptor[0] == 'B' || lptok->szDescriptor[0] == 'b'){
		dwType = DES_TYPE_CHAR;
	}else
	if(lptok->szDescriptor[0] == 'N' || lptok->szDescriptor[0] == 'n'){
		dwType = DES_TYPE_CHAR | DES_TYPE_JAPANESE;
	}else
	if(lptok->szDescriptor[0] == 'J' || lptok->szDescriptor[0] == 'j'){
		dwType = DES_TYPE_CHAR | DES_TYPE_JAPANESE;
	}else
	if(lptok->szDescriptor[0] == 'G' || lptok->szDescriptor[0] == 'g'){
		dwType = DES_TYPE_CHAR | DES_TYPE_JAPANESE;
	}else
	if(lptok->szDescriptor[0] == '-'){
		dwType = DES_TYPE_DEC | DES_TYPE_SIGN;
	}else{
		dwType = DES_TYPE_ERROR;
	}

	for(int i = 1; i < lptok->nDescriptLen; i++){
		if(lptok->szDescriptor[i] == 'V' || lptok->szDescriptor[i] == 'v') dwType |= DES_TYPE_DOUBLE;
		if(lptok->szDescriptor[i] == '.') dwType |= DES_TYPE_DOUBLE;
	}

	return dwType;
}

// 変数サイズ取得
DWORD CParserCBL::GetSizeOf(LPTOKEN_CBL lptok, DWORD dwPicType, LPPHYDATA lpphyd)
{
	DWORD dwSize = 0;
	char szBuf[MAX_BUFFER], *p;

	memset(lpphyd, 0, sizeof(PHYDATA));

	if(lptok->dwDescriptorType & DES_TYPE_ERROR) return 0;
	if(lptok->nDescriptLen > MAX_BUFFER) return 0;

//	strcpy(szBuf, lptok->szDescriptor);
	strcpy_s(szBuf, sizeof(szBuf), lptok->szDescriptor);

	dwSize = 0;
	if(dwPicType & DES_TYPE_DOUBLE){
		// サイズ定義を分解
		if(!(p = strstr(szBuf, "V"))){
			if(!(p = strstr(szBuf, "v"))){
				if(!(p = strstr(szBuf, "."))){
					return 0;
				}else{
					// '.'はそれだけでサイズ+1
					dwSize += 1;
				}
			}
		}
		dwSize += CompSize(p+1);
		// 実サイズ定義
		lpphyd->nLowLen = dwSize;
		*p = '\0';
	}

	dwSize += CompSize(szBuf);

	// 日本語（？）項目
	if(dwPicType & DES_TYPE_JAPANESE) dwSize *= 2;

	// 実サイズ定義
	lpphyd->nHiLen = dwSize - lpphyd->nLowLen;
	if(dwPicType & DES_TYPE_DEC){
		if(dwPicType & DES_TYPE_SIGN){
			lpphyd->nType = PHY_SIGNDEC;
		}else{
			lpphyd->nType = PHY_UNSIGNDEC;
		}
		lpphyd->nSign = 0;
	}else
	if(dwPicType & DES_TYPE_CHAR){
		if(dwPicType & DES_TYPE_JAPANESE){
			lpphyd->nType = PHY_JAPAN;
		}else{
			lpphyd->nType = PHY_CHAR;
		}
	}

	return dwSize;
}

// サイズ計算サブルーチン
DWORD CParserCBL::CompSize(char *pszDec)
{
	DWORD dwSize = 0;
	char *p;
	if(p = strstr(pszDec, "(")){
		// ()内の変数サイズを計算
		char *r = NULL;
		char szVal[41];
		for(p = pszDec; *p != '\0'; p++){
			if(*p == '(') r = p + 1;
			if(*p == ')'){
				if(r != NULL){
					memcpy(szVal, r, (p - r));
					szVal[(p - r)] = '\0';
					dwSize += atol(szVal);
					r = NULL;
				}
			}
		}

		// 以下のパターンはサイズ＋１
		DWORD dwExtSize = 0;
		for(p = pszDec; *p != '\0'; p++){
			if(*p == '(') break;
			if(*p == 'Z' || *p == 'z') dwExtSize++;
			if(*p == '9') dwExtSize++;
			if(*p == '-') dwExtSize++;
			if(*p == '\\') dwExtSize++;
			if(*p == 'B' || *p == 'b') dwExtSize++;
		}
		dwSize += (dwExtSize > 0 ? dwExtSize - 1 : 0);

	}else{
		for(p = pszDec; *p != '\0'; p++){
			if((*p == '.')
			|| (*p == 'V' || *p == 'v')){
				break;
			}
			if((*p == '9')
			|| (*p == 'X' || *p == 'x')
			|| (*p == 'Z' || *p == 'z')
			|| (*p == 'N' || *p == 'n')
			|| (*p == 'B' || *p == 'b')
			|| (*p == '\\')
			|| (*p == '-')){
				dwSize++;
			}
		}
	}

	return (dwSize + CompComma(pszDec));
}

// サイズ計算サブルーチン２
DWORD CParserCBL::CompComma(char *pszDec)
{
	DWORD dwSize = 0;
	// カンマの分だけ＋１
	for(char *p = pszDec; *p != '\0'; p++){
		if(*p == ',') dwSize++;
	}
	return dwSize;
}

// 変数サイズ取得(BIT)
DWORD CParserCBL::GetSizeOfBit(LPRECORD_CBL lptok)
{
	DWORD dwSize = 0;

	dwSize = lptok->dwTypeSize / 8;

	if((lptok->dwTypeSize % 8) > 0){
		dwSize++;
	}

	return dwSize;
}

// 変数サイズ取得(２進)
DWORD CParserCBL::GetSizeOfBinary(LPRECORD_CBL lptok)
{
	DWORD dwSize = 0;
	// 変数定義の拡張を考慮
	if((strcmp(lptok->szCompress, "COMP-1") == 0 || strcmp(lptok->szCompress, "comp-1") == 0)
	|| (strcmp(lptok->szCompress, "COMPUTATIONAL-1") == 0 || strcmp(lptok->szCompress, "computational-1") == 0)){
		dwSize = sizeof(double);
	}else
	if((strcmp(lptok->szCompress, "COMP-2") == 0 || strcmp(lptok->szCompress, "comp-2") == 0)
	|| (strcmp(lptok->szCompress, "COMPUTATIONAL-2") == 0 || strcmp(lptok->szCompress, "computational-2") == 0)){
		dwSize = sizeof(float);
	}else
	if((strcmp(lptok->szCompress, "COMP-3") == 0 || strcmp(lptok->szCompress, "comp-3") == 0)
	|| (strcmp(lptok->szCompress, "COMP-5") == 0 || strcmp(lptok->szCompress, "comp-5") == 0)
	|| (strcmp(lptok->szCompress, "COMPUTATIONAL-3") == 0 || strcmp(lptok->szCompress, "computational-3") == 0)
	|| (strcmp(lptok->szCompress, "COMPUTATIONAL-5") == 0 || strcmp(lptok->szCompress, "computational-5") == 0)){
		if(lptok->dwTypeSize < 5){
			dwSize = sizeof(short int);
		}else{
			dwSize = sizeof(long int);
		}
	}else
	if(strcmp(lptok->szCompress, "BINARY") == 0 || strcmp(lptok->szCompress, "binary") == 0){
		if(lptok->dwTypeSize < 5){
			dwSize = sizeof(short int);
		}else{
			dwSize = sizeof(long int);
		}
	}
	if(strcmp(lptok->szCompress, "PACKED-DECIMAL") == 0 || strcmp(lptok->szCompress, "packed-decimal") == 0){
		if(lptok->dwTypeSize < 5){
			dwSize = sizeof(short int);
		}else{
			dwSize = sizeof(long int);
		}
	}

	return dwSize;
}

// 変数サイズ取得(１６進)
DWORD CParserCBL::GetSizeOfHex(LPRECORD_CBL lptok)
{
	// 変数定義の拡張を考慮
	return ((lptok->dwTypeSize / 2) + 1);
}

// ＣＯＢＯＬ文法的な問題チェック
int CParserCBL::CobolWordCheck()
{
	m_nWordCheck = 0;	
	char szTrace[256];
	LPRECORD_CBL lpRecTmp = NULL;
	std::vector<LPRECORD_CBL>::size_type nAll = m_lpRec_cbl.size();
	for(std::vector<LPRECORD_CBL>::size_type i = 0; i < nAll; i++){
		if(IsCancel()) return 0;
		lpRecTmp = m_lpRec_cbl[i];
		// レベルの正しさ
		if(i == 0){
			if(m_unMinLevel < lpRecTmp->unLevel){
				wsprintf(szTrace,
					"ソースコード確認 [%s] レベル階層におかしな所があります",
					lpRecTmp->szDescriptor);
				AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
				m_nWordCheck++;
			}
		}
		// ピリオドの補完
		if(lpRecTmp->nSupplementation == SUP_PERIOD){
			wsprintf(szTrace,
				"ソースコード確認 [%s] にピリオドがありません",
				lpRecTmp->szDescriptor);
			AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
			m_nWordCheck++;
		}
		// 無効（？）な変数名
		if(lpRecTmp->nSupplementation == SUP_ERRCUT){
			wsprintf(szTrace,
				"ソースコード確認 [%s] 以外に無効な変数名の定義あり",
				lpRecTmp->szDescriptor);
			AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
			m_nWordCheck++;
		}
		// 再定義項目が不明
		if(lpRecTmp->bRedefines && i < 1){
			wsprintf(szTrace,
				"ソースコード確認 [%s] 不明な再定義項目定義あり",
				lpRecTmp->szDescriptor);
			AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
			m_nWordCheck++;
		}
		// 再定義項目の大きさ
		if((lpRecTmp->bRedefines)
		&& (lpRecTmp->nRedefinesGroupIndex == 1)){
			DWORD dwSize_A, dwSize_B;
			int nRecIdx = lpRecTmp->nRedefinesRecIndex;
			if(m_lpRec_cbl[nRecIdx]->bStruct){
				dwSize_A = GetStructSizeOf(nRecIdx);
			}else{
				dwSize_A = m_lpRec_cbl[nRecIdx]->dwTypeSize;
			}
			if(lpRecTmp->bStruct){
				dwSize_B = GetStructSizeOf(i);
			}else{
				dwSize_B = lpRecTmp->dwTypeSize;
			}
			if(m_lpRec_cbl[nRecIdx]->bOccurs) dwSize_A *= m_lpRec_cbl[nRecIdx]->nOccurs;
			if(lpRecTmp->bOccurs) dwSize_B *= lpRecTmp->nOccurs;
			if(dwSize_B > dwSize_A){
				wsprintf(szTrace,
					"ソースコード確認 [%s] 再定義項目が再定義元項目より大きいサイズになっています",
					lpRecTmp->szDescriptor);
				AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
				m_nWordCheck++;
			}else
			if(dwSize_A > dwSize_B){
				wsprintf(szTrace,
					"ソースコード確認 [%s] 再定義項目が再定義元項目よりサイズが小さい",
					lpRecTmp->szDescriptor);
				AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
				m_nWordCheck++;
			}
		}
		// 上位レベルは集団項目でないとダメ
		if((i > 0)
		&& (m_lpRec_cbl[i - 1]->unLevel < lpRecTmp->unLevel)){
			if(!m_lpRec_cbl[i - 1]->bStruct){
				wsprintf(szTrace,
					"ソースコード確認 [%s] の上位レベル項目は集団項目でないといけません",
					lpRecTmp->szDescriptor);
				AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
				m_nWordCheck++;
			}
		}
		// 集団項目のサイズが０Σ(ﾟДﾟ)
		if(m_lpRec_cbl[i]->bStruct){
			if(m_lpRec_cbl[i]->dwTypeSize == 0){
				wsprintf(szTrace,
					"ソースコード確認 [%s] 集団項目定義で０バイト項目があります",
					lpRecTmp->szDescriptor);
				AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
				m_nWordCheck++;
			}
		}
		// 最後が集団項目は変でしょ
		if(i == (nAll - 1)){
			if(m_lpRec_cbl[i]->bStruct){
				wsprintf(szTrace,
					"ソースコード確認 [%s] 最終項目に集団項目定義があります",
					lpRecTmp->szDescriptor);
				AddTrace(szTrace, TRACE_SUPP, lpRecTmp->nPhyLine);
				m_nWordCheck++;
			}
		}
	}
	return m_nWordCheck;
}

// 構造体項目の展開
int CParserCBL::DevStruct()
{
	AddTrace("集団項目の解析...");

	unsigned int unGroupId = 0;
	unsigned int unGroup = 0;
	std::vector<LPRECORD_CBL>::size_type i, j, aa = m_lpRec_cbl.size();
	LPRECORD_CBL lpRecTmp = NULL;
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		if(IsCancel()) return 0;
		lpRecTmp = m_lpRec_cbl[i];
		if(lpRecTmp->bStruct){
//			char szTrace[260];
//			wsprintf(szTrace, " [%s]の展開", lpRecTmp->szDescriptor);
//			AddTrace(szTrace);
			lpRecTmp->unStructGroupId = ++unGroupId;
			unsigned int unLevel = lpRecTmp->unLevel;
			LPRECORD_CBL lpRecTmp2 = NULL;
			for(j = i; j < m_lpRec_cbl.size(); j++){
				lpRecTmp2 = m_lpRec_cbl[j];
				if(unLevel > lpRecTmp2->unLevel) break;
//Rem			if(lpRecTmp2->unRedefinesGroupId > 0) continue;
				if(unLevel == lpRecTmp2->unLevel) continue;
				lpRecTmp2->unStructGroupId = unGroupId;
				lpRecTmp2->nStructGroupIndex = (int)i;	// 親の位置
				unGroup++;
				if ((unGroup % 500) == 0) {
					char szTrace[256];
					wsprintf(szTrace, "集団項目の展開 %d グループを完了", unGroup);
					AddTrace(szTrace);
				}
				if (IsCancel()) break;
			}
			lpRecTmp->unStructGroupCount = unGroup;
		}
	}
	return (int)unGroupId;
}

// 繰り返し項目の展開
int CParserCBL::DevOccurs()
{
	AddTrace("繰り返し項目の解析...");

	int nAddRec = 0;
	unsigned int unGroupId = 0;
	std::vector<LPRECORD_CBL>::size_type i, j;
	LPRECORD_CBL lpRecTmp = NULL;
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		lpRecTmp = m_lpRec_cbl[i];
		if(IsCancel()) return 0;
		if(!lpRecTmp->bOccurs) continue;
		// グループＩＤうｐ
		lpRecTmp->unOccursGroupId = ++unGroupId;
		// レベル番号待避
		unsigned int unLevel = lpRecTmp->unLevel;
		// レコード挿入位置のイテレータを見つける
		std::vector<LPRECORD_CBL>::iterator itrInsert;
		for(itrInsert = m_lpRec_cbl.begin() + (i + 1); itrInsert != m_lpRec_cbl.end(); itrInsert++){
			if((*itrInsert)->unLevel <= unLevel) break;
			// ついでにグループＩＤ設定
			(*itrInsert)->unOccursGroupId = unGroupId;
		}
		// 繰り返し分－１のレコードを挿入する
		std::vector<LPRECORD_CBL> lpRecInsert;
		for(int nOrder = 1; nOrder < lpRecTmp->nOccurs; nOrder++){
			if(IsCancel()) return 0;
			// 挿入用レコード作成
			for(std::vector<LPRECORD_CBL>::iterator itrOccurs = m_lpRec_cbl.begin() + i;
				itrOccurs != itrInsert && itrOccurs != m_lpRec_cbl.end(); itrOccurs++){
				LPRECORD_CBL lpRecCopy = (LPRECORD_CBL)GlobalAlloc(GPTR, sizeof(RECORD_CBL));
				if(!lpRecCopy){
					AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
					Cancel();
					return 0;
				}
				CopyMemory(lpRecCopy, *itrOccurs, sizeof(RECORD_CBL));
				// レコード編集
				if(strcmp(lpRecCopy->szDescriptor, lpRecTmp->szDescriptor) == 0){
					lpRecCopy->bOccurs = FALSE;		// TRUEは先頭のみ(無限ループしてしまう)
					lpRecCopy->nOccurs = 0;
				}
				lpRecCopy->nOccursOrder = nOrder;
				lpRecCopy->unOccursGroupId = unGroupId;
				// 追加
				lpRecInsert.push_back(lpRecCopy);
				// 繰り返し項目展開はかなり大きめになる場合があるので途中経過を通知する
				nAddRec++;
				if((nAddRec % 500) == 0){
					char szTrace[256];
					wsprintf(szTrace, "繰り返し項目の展開 %d 行目を完了", nAddRec);
					AddTrace(szTrace);
				}
				if (IsCancel()) break;
			}
		}
		m_lpRec_cbl.insert(itrInsert, lpRecInsert.begin(), lpRecInsert.end());
	}
//	AddTrace("繰り返し項目の枝番");
	// 同グループ内の枝番設定
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		if(IsCancel()) return 0;
		lpRecTmp = m_lpRec_cbl[i];
		unsigned int unGroupId_Save = 0;
		if(!lpRecTmp->bOccurs) continue;
		unGroupId_Save = lpRecTmp->unOccursGroupId;
		int nGroupIndex = 0;
		for(j = i; j < m_lpRec_cbl.size(); j++){
			// グループＩＤが小さくなったらオワリ
			if(unGroupId_Save > m_lpRec_cbl[j]->unOccursGroupId) break;
			// グループＩＤが大きいものは加算対象外
			if(unGroupId_Save < m_lpRec_cbl[j]->unOccursGroupId) continue;
			// インデックス加算
			m_lpRec_cbl[j]->nOccursGroupIndex = nGroupIndex++;

			if (IsCancel()) break;
		}
	}
	return (int)unGroupId;
}

// 再定義項目の展開
int CParserCBL::DevRedefines(void)
{
	AddTrace("再定義項目の解析...");

	// グループＩＤ設定
	unsigned int unGroupId = 0;
	std::vector<LPRECORD_CBL>::size_type i, j, k;
	LPRECORD_CBL lpRecTmp = NULL, lpRecTmp2 = NULL;
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		if(IsCancel()) return 0;
		lpRecTmp = m_lpRec_cbl[i];
		// 再定義項目
		if(lpRecTmp->bRedefines && i > 0){
			// グループＩＤうｐ
			unGroupId++;
			// レベル番号退避
			unsigned int unLevel = lpRecTmp->unLevel;
			// 変数名
			char szRedefines[MAX_DESCRIPT_LEN];
//			strcpy(szRedefines, lpRecTmp->szRedefines);
			strcpy_s(szRedefines, sizeof(szRedefines), lpRecTmp->szRedefines);
			// 繰り返し
			int nOccursOrder = lpRecTmp->nOccursOrder;
			// 再定義項目の元位置
			lpRecTmp2 = NULL;
			for(j = (i - 1); j > 0; j--){
				lpRecTmp2 = m_lpRec_cbl[j];
//2007.01.04				if((strcmp(szRedefines, lpRecTmp2->szDescriptor) == 0)
//2007.01.04				&& (nOccursOrder == lpRecTmp2->nOccursOrder)){
				if(strcmp(szRedefines, lpRecTmp2->szDescriptor) == 0){
					lpRecTmp->nRedefinesLine = lpRecTmp2->nLogLine;
					lpRecTmp->nRedefinesRecIndex = j;
					break;
				}
			}
			// 上位のレベル番号が出現するまで再定義項目
			lpRecTmp2 = NULL;
			lpRecTmp->unRedefinesGroupId = unGroupId;
			for(k = (i + 1); k < m_lpRec_cbl.size(); k++){
				lpRecTmp2 = m_lpRec_cbl[k];
				if(unLevel >= lpRecTmp2->unLevel) break;
				lpRecTmp2->unRedefinesGroupId = unGroupId;
//				// (－益－ )ゥ～ン・・・
//				lpRecTmp2->nRedefinesLine = lpRecTmp->nRedefinesLine;
			}
		}
	}
	// 同グループ内の枝番設定
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		if(IsCancel()) return 0;
		lpRecTmp = m_lpRec_cbl[i];
		unsigned int unGroupId_Save = 0;
		if(!lpRecTmp->bRedefines) continue;
		unGroupId_Save = lpRecTmp->unRedefinesGroupId;
		int nGroupIndex = 0;
		for(j = i; j < m_lpRec_cbl.size(); j++){
			lpRecTmp2 = m_lpRec_cbl[j];
			// グループＩＤが小さくなったらオワリ
			if(unGroupId_Save > lpRecTmp2->unRedefinesGroupId) break;
			// グループＩＤが大きいものは加算対象外
			if(unGroupId_Save < lpRecTmp2->unRedefinesGroupId) continue;
			// インデックス加算
			lpRecTmp2->nRedefinesGroupIndex = nGroupIndex++;
		}
	}
	return (int)unGroupId;
}

// 変数バイト位置取得
DWORD CParserCBL::GetPosOf()
{
	//AddTrace("変数ﾊﾞｲﾄ位置算出");
	AddTrace("変数オフセット算出...");

	std::vector<LPRECORD_CBL>::size_type i;
	DWORD dwTotal = m_nComp;
	LPRECORD_CBL lpRecTmp = NULL;
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		lpRecTmp = m_lpRec_cbl[i];
		// 再定義項目はとりあえず無視
		if(lpRecTmp->unRedefinesGroupId > 0) continue;
		lpRecTmp->dwTypePos = dwTotal;
		if(!lpRecTmp->bStruct) dwTotal += lpRecTmp->dwTypeSize;
	}
	// 再定義項目の位置算出
	unsigned int unRedefinesGroupId = 0;
	// 全再定義項目数分のメモリを確保
	DWORD *pdwRedefinesTotal = (DWORD *)GlobalAlloc(GPTR, (DWORD)sizeof(DWORD) * m_nRedefinesCount);
	if(!pdwRedefinesTotal){
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		Cancel();
		return 0;
	}
	int nId = 0;
	for(i = 0; i < m_lpRec_cbl.size(); i++){
		lpRecTmp = m_lpRec_cbl[i];
		if(lpRecTmp->unRedefinesGroupId > 0){
			if(unRedefinesGroupId != lpRecTmp->unRedefinesGroupId){
				unRedefinesGroupId = lpRecTmp->unRedefinesGroupId;
				nId = unRedefinesGroupId - 1;		// unRedefinesGroupIdは1から始まるので指数は-1
				if(nId < 0){
					AddTrace("予期せぬエラー 指数が０以下です", TRACE_ALLOC);
					Cancel();
					break;
				}
				// 再定義元の位置を記憶する
				if(lpRecTmp->nRedefinesGroupIndex == 0)
					*(pdwRedefinesTotal + nId) = m_lpRec_cbl[lpRecTmp->nRedefinesRecIndex]->dwTypePos;
				// 繰り返していたらその分足し込む･･･(－益－ )ゥ～ン
				for(int j = 0; j < lpRecTmp->nOccursOrder; j++){
					if(lpRecTmp->bStruct){
						*(pdwRedefinesTotal + nId) += GetStructSizeOf(i);
					}else{
						*(pdwRedefinesTotal + nId) += lpRecTmp->dwTypeSize;
					}
				}
			}
			lpRecTmp->dwTypePos = *(pdwRedefinesTotal + nId);
			*(pdwRedefinesTotal + nId) += lpRecTmp->dwTypeSize;
		}
	}
	GlobalFree(pdwRedefinesTotal);
//	return dwTotal;
	return (dwTotal - m_nComp);
}

// 構造体サイズ取得
DWORD CParserCBL::GetStructSizeOf(int nIdx)
{
	if(!m_lpRec_cbl[nIdx]->bStruct) return 0;

	unsigned int unLevel = m_lpRec_cbl[nIdx]->unLevel;
	unsigned int unGroupId = m_lpRec_cbl[nIdx]->unStructGroupId;
	unsigned int unRedefinesGroupId = m_lpRec_cbl[nIdx]->unRedefinesGroupId;
	DWORD dwSize = 0;
	LPRECORD_CBL lpRecTmp = NULL;
	for(std::vector<LPRECORD_CBL>::size_type i = nIdx + 1; i < m_lpRec_cbl.size(); i++){
		lpRecTmp = m_lpRec_cbl[i];
		if(lpRecTmp->unLevel <= unLevel) break;
		if(lpRecTmp->unRedefinesGroupId != unRedefinesGroupId) continue;
		if(lpRecTmp->unStructGroupId >= unGroupId) dwSize += lpRecTmp->dwTypeSize;
	}
	return dwSize;
}

// バイナリ変換
DWORD CParserCBL::Hex2Dwrod(const char *pszHex)
{
	long lnum = 0;
	int im;
	for( ; *pszHex != '\0'; pszHex++){
		if(*pszHex >= '0' && *pszHex <= '9'){
			im = *pszHex - '0';
		}else if(*pszHex >= 'A' && *pszHex <= 'F'){
			im = *pszHex - 'A' + 10;
		}else if(*pszHex >= 'a' && *pszHex <= 'f'){
			im = *pszHex - 'a' + 10;
		}else{
			break;
		}
		lnum = 16 * lnum + im;
	}
	return (DWORD)lnum;
}

// レコード取得
LPRECORD_CBL CParserCBL::GetRecord(const int nRecord)
{
	if(GetRecordCount() < nRecord) return NULL;
	return m_lpRec_cbl[nRecord];
}
