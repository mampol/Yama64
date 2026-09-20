/*-------------------------------------------------------------------------------------

	CLexCBL.cpp

-------------------------------------------------------------------------------------*/
#include "CAnaCBL.h"

// コンストラクタ
CLexCBL::CLexCBL()
 : m_nSeqLen(0),
   m_nLineMax(0),
   m_bHostSize(FALSE),
   m_pszSource(NULL),
   m_dwSourceSize(0)
{
#ifdef _DEBUG_MODE_
	printf("CLexCBL コンストラクタ\n");
#endif	//_DEBUG_MODE_
	m_lpSrcInfo.clear();
	m_lpTok_cbl.clear();
}

// デストラクタ
CLexCBL::~CLexCBL()
{
#ifdef _DEBUG_MODE_
	printf("CLexCBL デストラクタ\n");
#endif	//_DEBUG_MODE_
	ClearToken();
	for(std::vector<LPSOURCEINFO>::size_type j = 0; j < m_lpSrcInfo.size(); j++){
		if(m_lpSrcInfo[j]){
			if(m_lpSrcInfo[j]->pszSourceCode) GlobalFree(m_lpSrcInfo[j]->pszSourceCode);
			if(GlobalFree(m_lpSrcInfo[j]) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
		}
	}
	if(m_pszSource){
		if(GlobalFree(m_pszSource) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
	}
}

// トークン格納配列のクリア
void CLexCBL::ClearToken()
{
	for(std::vector<LPTOKEN_CBL>::size_type i = 0; i < m_lpTok_cbl.size(); i++){
		if(GlobalFree(m_lpTok_cbl[i]) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
	}
	m_lpTok_cbl.clear();
}

// ソース番号から配列インデクス取得
int CLexCBL::GetIndexFromNo(const int nNo)
{
	for(std::vector<LPSOURCEINFO>::size_type i = 0; i <  m_lpSrcInfo.size(); i++){
		if(m_lpSrcInfo[i]->nNo == nNo) return (int)i;
	}
	return -1;
}

// 配列インデクスからソース番号取得
int CLexCBL::GetNoFromIndex(const int nIndex)
{
	for(std::vector<LPSOURCEINFO>::size_type i = 0; i <  m_lpSrcInfo.size(); i++){
		if(nIndex == i) return (int)m_lpSrcInfo[i]->nNo;
	}
	return -1;
}

// コードから解析するソース設定
int CLexCBL::AddSourceCode(const char *pszSource, DWORD dwSize,
						   const char *pszParam/* = NULL*/,
						   const LPVOID lpParam/* = 0*/)
{
	if(dwSize < 1) return -1;
	LPSOURCEINFO lpSrcInfo = (LPSOURCEINFO)GlobalAlloc(GPTR, (DWORD)sizeof(SOURCEINFO));
	if(!lpSrcInfo){
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		return -1;
	}
	lpSrcInfo->dwSourceSize = dwSize;
	lpSrcInfo->pszSourceCode = (char *)GlobalAlloc(GPTR, (DWORD)(lpSrcInfo->dwSourceSize + 1));
	if(!lpSrcInfo->pszSourceCode){
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		GlobalFree(lpSrcInfo);
		return -1;
	}
//	strcpy(lpSrcInfo->pszSourceCode, pszSource);
	strcpy_s(lpSrcInfo->pszSourceCode, (size_t)(dwSize + 1), pszSource);
	lpSrcInfo->nNo = (int)m_lpSrcInfo.size();
//	if(pszParam) strcpy(lpSrcInfo->szParam, pszParam);
	if(pszParam) strcpy_s(lpSrcInfo->szParam, sizeof(lpSrcInfo->szParam), pszParam);
	if(lpParam) lpSrcInfo->lpParam = lpParam;
	// 追加する
	m_lpSrcInfo.push_back(lpSrcInfo);
	return lpSrcInfo->nNo;
}

// ファイルから解析するソース設定
int CLexCBL::AddSourceFile(const char *pszSourcePath,
						   const char *pszParam/* = NULL*/,
						   const LPVOID lpParam/* = 0*/)
{
	// メモリ確保
	LPSOURCEINFO lpSrcInfo = (LPSOURCEINFO)GlobalAlloc(GPTR, (DWORD)sizeof(SOURCEINFO));
	if(!lpSrcInfo){
		AddTrace("ソースファイルの読込み時にメモリ不足が発生しています", TRACE_FILE);
		return -1;
	}

	// ファイル設定
//	strcpy(lpSrcInfo->szSourcePath, pszSourcePath);
	strcpy_s(lpSrcInfo->szSourcePath, sizeof(lpSrcInfo->szSourcePath), pszSourcePath);
	if(ReadSource(lpSrcInfo) < 0){
		AddTrace("ソースファイルの読込みに失敗しました", TRACE_FILE);
		return -1;
	}
	lpSrcInfo->nNo = (int)m_lpSrcInfo.size();
//	if(pszParam) strcpy(lpSrcInfo->szParam, pszParam);
	if(pszParam) strcpy_s(lpSrcInfo->szParam, sizeof(lpSrcInfo->szParam), pszParam);
	if(lpParam) lpSrcInfo->lpParam = lpParam;
	// 追加する
	m_lpSrcInfo.push_back(lpSrcInfo);
	return lpSrcInfo->nNo;
}

// 解析するソース設定の再読込み
int CLexCBL::ReloadSourceFile(const int nNo)
{
	if(nNo >= GetSourceCount()) return -1;
	int nIndex;
	if((nIndex = GetIndexFromNo(nNo)) < 0) return -1;
	if(strlen(m_lpSrcInfo[nIndex]->szSourcePath) < 1) return 0;	// コード指定は再読込みの必要なし
	// 再度読込み
	char szTraceMsg[1024];
	if(ReplaceSourceFile(nNo, m_lpSrcInfo[nIndex]->szSourcePath,
		m_lpSrcInfo[nIndex]->szParam, m_lpSrcInfo[nIndex]->lpParam) < 0){
		wsprintf(szTraceMsg, "%sの読込みに失敗しました", m_lpSrcInfo[nIndex]->szSourcePath);
		AddTrace(szTraceMsg, TRACE_FILE);
		return -1;
	}
	return (int)(m_lpSrcInfo[nIndex]->dwSourceSize);
}

// 解析するソース設定の全再読込み
int CLexCBL::ReloadAllSourceFile()
{
	int nCount = 0, nErr = 0;
	if(GetSourceCount() < 1) return 0;
	int nInfo = (int)m_lpSrcInfo.size();
	for(int i = 0; i < nInfo; i++){
		int nNo = GetNoFromIndex(i);
		if(nNo < 0) continue;
		if(ReloadSourceFile(nNo) < 0){
			nErr++;
		}else{
			nCount++;
		}
	}
	// エラー
	if(nErr > 0){
		AddTrace("読込みに失敗しています ファイルの存在等確認してください リロードは失敗している可能性があります", TRACE_FILE);
		// リナンバ
		SourceInfoRenumber();
	}
	return nCount;
}

// 解析するソース設定の削除
int CLexCBL::RemoveSource(const int nNo)
{
	if(nNo >= GetSourceCount()) return -1;
	int nIndex;
	if((nIndex = GetIndexFromNo(nNo)) < 0) return -1;
	if(m_lpSrcInfo[nIndex]->pszSourceCode) GlobalFree(m_lpSrcInfo[nIndex]->pszSourceCode);
	if(GlobalFree(m_lpSrcInfo[nIndex]) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
	m_lpSrcInfo.erase((m_lpSrcInfo.begin() + nIndex));
	// リナンバ
	SourceInfoRenumber();
	return ((int)m_lpSrcInfo.size());
}

// 解析するソース設定の全削除
int CLexCBL::RemoveAllSources()
{
	int nCount = 0;
	if(GetSourceCount() < 1) return 0;
	for(std::vector<LPSOURCEINFO>::size_type i = 0; i < m_lpSrcInfo.size(); i++){
		if(m_lpSrcInfo[i]){
			if(m_lpSrcInfo[i]->pszSourceCode) GlobalFree(m_lpSrcInfo[i]->pszSourceCode);
			if(GlobalFree(m_lpSrcInfo[i]) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
			nCount++;
		}
	}
	m_lpSrcInfo.clear();
	return nCount;
}

// 解析するソース設定順の入替
int CLexCBL::SwapSourceNo(const int nNewNo, const int nOldNo)
{
	int nCount = GetSourceCount();
	if(nNewNo >= nCount) return -1;
	if(nOldNo >= nCount) return -1;
	int nNewIndex, nOldIndex;
	if((nNewIndex = GetIndexFromNo(nNewNo)) < 0) return -1;
	if((nOldIndex = GetIndexFromNo(nOldNo)) < 0) return -1;
	int nNoSave = m_lpSrcInfo[nOldIndex]->nNo;
	m_lpSrcInfo[nOldIndex]->nNo = m_lpSrcInfo[nNewIndex]->nNo;
	m_lpSrcInfo[nNewIndex]->nNo = nNoSave;
	// ソートする
	SortSourceInfo();
	return 0;
}

// 解析するソース設定の置き換え
int CLexCBL::ReplaceSourceFile(const int nNo,
							   const char *pszSourcePath,
							   const char *pszParam/* = NULL*/,
							   const LPVOID lpParam/* = 0*/)
{
	// とりあえず追加
	int nRetNo = AddSourceFile(pszSourcePath, pszParam, lpParam);
	if(nRetNo < 0) return -1;
	// 目的位置との番号を入れ替える
	SwapSourceNo(nNo, nRetNo);
	// 入れ替えた位置を消す
	if(RemoveSource(nRetNo) < 0) return -1;
	return nNo;
}

int CLexCBL::ReplaceSourceCode(const int nNo,
							   const char *pszSource, DWORD dwSize,
							   const char *pszParam/* = NULL*/,
							   const LPVOID lpParam/* = 0*/)
{
	// とりあえず追加
	int nRetNo = AddSourceCode(pszSource, dwSize, pszParam, lpParam);
	if(nRetNo < 0) return -1;
	// 目的位置との番号を入れ替える
	SwapSourceNo(nNo, nRetNo);
	// 入れ替えた位置を消す
	if(RemoveSource(nRetNo) < 0) return -1;
	return nNo;
}

// リナンバ
void CLexCBL::SourceInfoRenumber()
{
	// ソートする
	SortSourceInfo();
	// SEQ振り直し
	for(int i = 0; i < (int)m_lpSrcInfo.size(); i++) m_lpSrcInfo[i]->nNo = i;
}

// ソース読込み
int CLexCBL::ReadSource(LPSOURCEINFO lpSrcInfo)
{
	char szTraceMsg[1024];
	HANDLE hSource;
	try{
		// ファイルオープン
		hSource = CreateFile(lpSrcInfo->szSourcePath,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hSource == INVALID_HANDLE_VALUE){
			wsprintf(szTraceMsg,
				"ソースファイルのオープンに失敗しました [%s]",
				lpSrcInfo->szSourcePath);
			throw szTraceMsg;
		}

		// ファイルサイズ取得
		DWORD dwFileSizeHi, dwFileSize;
		dwFileSize = GetFileSize(hSource, &dwFileSizeHi);
		if(dwFileSizeHi != 0){
			wsprintf(szTraceMsg,
				"ソースファイルのサイズが大きすぎます (2G OVER) [%s]",
				lpSrcInfo->szSourcePath);
			throw szTraceMsg;
		}
		if(dwFileSize > MAX_FILE_SIZE){
			wsprintf(szTraceMsg,
				"ソースファイルのサイズが大きすぎます (MAX %d Byte) [%s]",
				lpSrcInfo->szSourcePath, MAX_FILE_SIZE);
			throw szTraceMsg;
		}
		lpSrcInfo->dwSourceSize = dwFileSize;

		// メモリ確保
		if(lpSrcInfo->pszSourceCode){
			if(GlobalFree(lpSrcInfo->pszSourceCode)){
				wsprintf(szTraceMsg,
					"メモリ解放エラーが発生しています [%s]",
					lpSrcInfo->szSourcePath);
				throw szTraceMsg;
			}
		}
		lpSrcInfo->pszSourceCode = (char *)GlobalAlloc(GPTR, (DWORD)(lpSrcInfo->dwSourceSize + 1));
		if(!lpSrcInfo->pszSourceCode){
			wsprintf(szTraceMsg,
				"メモリ不足が発生しています [%s]",
				lpSrcInfo->szSourcePath);
			throw szTraceMsg;
		}

		// ファイル読込み
		DWORD dwReadSize;
		if(!ReadFile(hSource,
			(LPVOID)lpSrcInfo->pszSourceCode,
			lpSrcInfo->dwSourceSize,
			&dwReadSize, NULL)){
			wsprintf(szTraceMsg,
				"ソースファイルを読み込むことができません [%s]",
				lpSrcInfo->szSourcePath);
			throw szTraceMsg;
		}

		// ファイルクローズ
		CloseHandle(hSource);

		// 全部読み込めたか？
		if(lpSrcInfo->dwSourceSize != dwReadSize){
			wsprintf(szTraceMsg,
				"ソースファイルの内容全てを読み込むことができません [%s]",
				lpSrcInfo->szSourcePath);
			throw szTraceMsg;
		}
	}
	catch(const char *pszMsg)
	{
		AddTrace(pszMsg, TRACE_FILE);
		if(lpSrcInfo){
			if(lpSrcInfo->pszSourceCode) GlobalFree(lpSrcInfo->pszSourceCode);
			GlobalFree(lpSrcInfo);
		}
		if(hSource != INVALID_HANDLE_VALUE) CloseHandle(hSource);
		return -1;
	}
	return 0;
}

// 指定されたソースコードの加工
int CLexCBL::FactSource(const std::vector<LPSOURCEINFO>::size_type nNo)
{
	if(m_pszSource){
		GlobalFree(m_pszSource);
		m_pszSource = NULL;
	}

	// 必要なサイズを求める
	DWORD dwSize = m_lpSrcInfo[nNo]->dwSourceSize;

	// サイズが0BYTEのソースコードはエラー
	if(dwSize < 1){
		AddTrace("指定されたソースコードが不正です サイズが0Byteです", TRACE_ANA);
		return -1;
	}

	// SEQLENよりもソースコードが小さいのはエラー
	if((int)dwSize < m_nSeqLen){
		AddTrace("指定されたソースコードが不正です サイズが小さすぎます", TRACE_ANA);
		return -1;
	}

	// ソース末尾が'\n'じゃ無い物は＋１
	if(m_lpSrcInfo[nNo]->pszSourceCode[m_lpSrcInfo[nNo]->dwSourceSize - 1] != '\n') dwSize++;

	// ソースサイズ合計変数に加算
	m_dwSourceSize += dwSize;

	// バッファ確保
	m_pszSource = (char *)GlobalAlloc(GPTR, (DWORD)(dwSize + 1));
	if(m_pszSource == NULL){
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		return -2;
	}

	// ｿｰｽｺｰﾄﾞを解析用ﾊﾞｯﾌｧへｺﾋﾟｰ
//	strcpy(m_pszSource, m_lpSrcInfo[nNo]->pszSourceCode);
	strcpy_s(m_pszSource, (size_t)(dwSize + 1), m_lpSrcInfo[nNo]->pszSourceCode);

	// ソース末尾が'\n'じゃ無い物は'\n'付加
	if(m_lpSrcInfo[nNo]->pszSourceCode[m_lpSrcInfo[nNo]->dwSourceSize - 1] != '\n')
//		strcat(m_pszSource, "\n");
		strcat_s(m_pszSource, (size_t)(dwSize + 1), "\n");

	// 1行の最大文字数が設定されていなければここで終わり
	if(m_nLineMax < 1) return 0;

	// 1行の最大文字数が設定されている場合
	int nChar = 0;
	for(char *p = m_pszSource; *p != '\0'; p++){
		if(*p == '\n') nChar = 0;
		// m_nLineMax以降はすべて0x20で埋める
		if(nChar > m_nLineMax) *p = ' ';
		nChar++;
	}
	return 0;
}

// トークン格納
int CLexCBL::TokenCBL_Push(int nSrcNo, int nPhyLine, int nLogLine, BYTE btType, char *pszDescriptor, int nDescriptLen)
{
	LPTOKEN_CBL lpToken;
	int i;
//	static BOOL bOccurs = FALSE, bRedefines = FALSE;

	if(!(lpToken = (LPTOKEN_CBL)GlobalAlloc(GPTR, sizeof(TOKEN_CBL)))){
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		Cancel();
		return -1;
	}

	lpToken->nSrcNo = nSrcNo;
	lpToken->nPhyLine = nPhyLine;
	lpToken->nLogLine = nLogLine;
	lpToken->btType = btType;
	for(i = 0; i < nDescriptLen; i++){
		lpToken->szDescriptor[i] = *pszDescriptor;
		pszDescriptor++;
	}
	lpToken->szDescriptor[i] = '\0';
	lpToken->nDescriptLen = nDescriptLen;
//	// 大文字にする
//	CharUpper(lpToken->szDescriptor);

	// 格納する
	m_lpTok_cbl.push_back(lpToken);

	return 0;
}

// COBOLソースを解析
DWORD CLexCBL::Lex(const int nSeq/* = 6*/,
				   const int nMaxLine/* = 0*/,
				   const BOOL bHostMode/* = NULL*/,
				   char *pSign/* = NULL*/)
{
	// 初期化
	InitAnalysys(STAGE_LEX);

	DWORD dwStTime = timeGetTime();

	AddTrace("構文解析開始...");

	// ソース行番号の桁数
	m_nSeqLen = nSeq;
	// ソース行の最大桁数
	m_nLineMax = nMaxLine;
	// HOSTCOBOL変数サイズ計算
	m_bHostSize = bHostMode;
	// エラー除外記号の設定
	if(pSign) SetSign(pSign);
	// トークン格納配列のクリア
	ClearToken();
	// ソース合計サイズをクリア
	m_dwSourceSize = 0;

	// ソースをnNoメンバにて並び替える
	SortSourceInfo();

	std::vector<LPSOURCEINFO>::size_type nSrcSize = m_lpSrcInfo.size();
	if(nSrcSize < 1){
		AddTrace("解析するソースコードの指定がありません", TRACE_FILE);
		Cancel();
		return 0;
	}
	for(std::vector<LPSOURCEINFO>::size_type nSrcNo = 0; nSrcNo < nSrcSize; nSrcNo++){

		// ソースを解析用に加工
		if(FactSource(nSrcNo) != 0){
			AddTrace("指定されたソースコードが不正です ソースコードの解析が開始できません", TRACE_ANA);
			Cancel();
			break;
		}

		// トークンに分解
		Lex_sub(nSrcNo);

		// ﾒﾓﾘ解放
		if(m_pszSource){
			GlobalFree(m_pszSource);
			m_pszSource = NULL;
		}

		if(IsCancel()) break;
	}

	if(IsCancel()) return -1;

	// ｿｰｽ終端用ﾄｰｸﾝの格納と終了処理
	if(TokenCBL_Push(-1, m_nPhysLine, m_nLogiLine, CBL_TYPE_EOF, LEX_EOF, (int)strlen(LEX_EOF)) < 0) Cancel();
	AddTrace("構文解析完了");
	// 終了
	EndAnalysys();

	return (timeGetTime() - dwStTime);
}

DWORD CLexCBL::Lex_sub(const std::vector<LPSOURCEINFO>::size_type nSrcNo)
{
	char szTrace[512];
	char *pChar = NULL;
	int n = m_nSeqLen, // ソース先頭のコメントを除外する為の処置
		nPos = 0;
	BOOL bLineTop = TRUE, bPic = FALSE, bVal = FALSE;

	// 最初はレベル番号
	m_bLevel = TRUE;
	// 行末判定
	m_bEol = FALSE;

	// 行情報のクリア
	m_nPhysLine = m_nLogiLine = 0;

	// 改行以外が来るまでスキップ
	char *pFirst = m_pszSource;
	while(1){
		if(*pFirst == '\n'){
			m_nPhysLine++;
			pFirst++;
		}else{
			break;
		}
	}
	if(*pFirst == '\0'){
		AddTrace("指定されたソースコードはCOBOLコードでは無い可能性があります", TRACE_ANA);
		Cancel();
		return 0;
	}

	// SEQLEN+1が不正な文字はｴﾗｰ
	if((*(pFirst + m_nSeqLen) == ' ')
	|| (*(pFirst + m_nSeqLen) == '*')
	|| (*(pFirst + m_nSeqLen) == '/')
	|| (*(pFirst + m_nSeqLen) == 'D')
	|| (*(pFirst + m_nSeqLen) == 'd')
	|| (*(pFirst + m_nSeqLen) == '-')
	|| (*(pFirst + m_nSeqLen) == '\t')
	|| (*(pFirst + m_nSeqLen) == '\n')){
		if((int)m_lpSrcInfo[nSrcNo]->lpParam == SRC_TYPE_CBL){
			wsprintf(szTrace, "[%s]の構文解析を開始", m_lpSrcInfo[nSrcNo]->szSourcePath);
		}else if((int)m_lpSrcInfo[nSrcNo]->lpParam == SRC_TYPE_EDIT){
			wsprintf(szTrace, "[編集されたソースコード(No.%02d)]の構文解析を開始", (int)nSrcNo);
		}else{
			wsprintf(szTrace, "[%s]の構文解析を開始", m_lpSrcInfo[nSrcNo]->szParam);
		}
		AddTrace(szTrace, TRACE_ANA);
	}else{
		AddTrace("標識領域に不正な文字があります", TRACE_ANA);
		Cancel();
		return 0;
	}

	DWORD dwChkTime = 0;
	// 構文解析(トークンに区切る)
	for(pChar = pFirst + n; *pChar != '\0'; pChar += n){

		// コメント行のチェック
		if(bLineTop){
			bLineTop = FALSE;
			if(*pChar == '*'
			|| *pChar == '/'
			|| *pChar == 'D'
			|| *pChar == 'd'
			|| *pChar == '-'){
				// 改行までスキップ
				for(; *pChar != '\0'; pChar++){
					if(*pChar == '\n') break;
				}
				if(*pChar == '\0') break;
			}
		}

		// 改行の確認
		if(*pChar == '\n'){
			m_nPhysLine++;
			// 行先頭フラグオン
			bLineTop = TRUE;
			if(*(pChar + 1) == '\0'
			|| *(pChar + 1) == HOST_EOF){
				// 改行して終わっているファイルの場合(HOST_EOF(0x1a)もチェック)
				n = 1;
			}else
			// 改行してさらに改行して終わっているファイルの場合(HOST_EOF(0x1a)もチェック)
			if(*(pChar + 1) == '\r'
			&& *(pChar + 2) == '\n'){
				n = 1;
			}else{
				// 行番号を飛ばす
				n = m_nSeqLen + 1;
			}
			nPos = 0;
		}else

		// ソース中間でのコメントのチェック
		if((n = IsComment((int)nSrcNo, pChar)) > 0){
		}else

		// 変数サイズ定義判断
		if(bPic && (n = IsPicSize((int)nSrcNo, pChar)) > 0){
			bPic = FALSE;
		}else

		// 区切文字判断
		if((n = IsEOL((int)nSrcNo, pChar)) > 0){
			m_nLogiLine++;
			if((m_nLogiLine % 100) == 0){
				wsprintf(szTrace, "構文解析 %d行目を完了", m_nLogiLine);
				AddTrace(szTrace);
			}
		}else

		// 変数の拡張設定判断
		if((n = IsRedefines((int)nSrcNo, pChar)) > 0){
		}else
		if((n = IsOccurs((int)nSrcNo, pChar)) > 0){
		}else
		if((n = IsValue((int)nSrcNo, pChar)) > 0){
			bVal = TRUE;
		}else

		// ビット型変数判断
		if((n = IsBit((int)nSrcNo, pChar)) > 0){
		}else

		// ２進変数判断
		if((n = IsBinary((int)nSrcNo, pChar)) > 0){
		}else

		// 演算符号形式
		if((n = IsSeparate((int)nSrcNo, pChar)) > 0){
		}else

		// 変数型判断
		if((n = IsDescriptorType((int)nSrcNo, pChar)) > 0){
			bPic = TRUE;
		}else

		// レベル判断
		if((n = IsLevelNo((int)nSrcNo, pChar)) > 0){
		}else

		// 文字列定数判断
		if((n = IsCharConst((int)nSrcNo, pChar)) > 0){
			bVal = FALSE;
		}else

		// 数値定数判断
		if((n = IsNumericConst((int)nSrcNo, pChar)) > 0){
			bVal = FALSE;
		}else

		// 変数名判断
		if((n = IsIdentifier((int)nSrcNo, pChar)) > 0){
		}else

		// 解析上関係のない識別子
		if((n = IsNullConst((int)nSrcNo, pChar)) > 0){
		}else{

			// 空白の除去
			n = SkipBlank(pChar);
		}

		// nが進まない場合は緊急脱出
		if(n < 1){
			// COPY句の入れ子?
			if(n < 0) AddTrace("COPY句がネストされています 解析は不可能です", TRACE_ANA);
			wsprintf(szTrace, "%d行目%dバイト目に解析不能なエラーを発見 解析を中止しました", (m_nPhysLine + 1), nPos);
			AddTrace(szTrace, TRACE_ANA, (m_nLogiLine + 1));
			Cancel();
			break;
		}

		nPos += n;
	}

	return 0;
}

// コメント(*>形式)
int CLexCBL::IsComment(int nSrcNo, char *p)
{
	if(*p == '*'
	|| *(p + 1) == '>'){
		// 改行まで進める
		char *r, *j = p;
		for(r = p; *r != '\0'; r++){
			if((*r == '\r')
			&& (*(r + 1) == '\n')) break;
			if(*r == '\n') break;
		}
		// 文字位置まで進める
		for(p += 2; *p != '\0'; p++){
			if((*p == 0x81)
			&& (*(p + 1) == 0x40)){
				p++;
				continue;
			}
			if(*p == ' ') continue;
			break;
		}
		// コメント抜き取り
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_COMMENT, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		return (int)(r - j);
	}
	return 0;
}

// レベル判断
int CLexCBL::IsLevelNo(int nSrcNo, char *p)
{
	if(IsNumeric(*p)
	&& IsNumeric(*(p + 1))
	&& *(p + 2) == ' '){
		if(m_bLevel){
			m_bLevel = FALSE;
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_LEVEL, p, 2) < 0) return 0;
		}else
		if(m_bEol){
			m_bEol = FALSE;
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_WEOL, ".", 1) < 0) return 0;
			// トークン格納
			m_nLogiLine++;
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_LEVEL, p, 2) < 0) return 0;
			return 2;
		}
	}
	return 0;
}

// 変数名(識別子)判断
int CLexCBL::IsIdentifier(int nSrcNo, char *p)
{
	char *r = NULL;
	// 英字、指定された符号、日本語のみおｋ
	if((IsAlphabet(*p))
	|| (IsSign(*p))
	|| (IsJapan(*p))){
		if((*p == 'C' || *p == 'c')
		&& (*(p + 1) == 'O' || *(p + 1) == 'o')
		&& (*(p + 2) == 'P' || *(p + 2) == 'p')
		&& (*(p + 3) == 'Y' || *(p + 3) == 'y')
		&& (*(p + 4) == ' ' || *(p + 4) == '\r' || *(p + 4) == '\n' || *(p + 4) == '\t')) return -1;
		for(r = p; *r != '\0'; r++){
			if(*r == ' ') break;
			if(*r == CBL_EOL) break;
			if(*r == '\r') break;
			if(*r == '\t') break;
			if(*r == '\n') break;
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_DESCRIPT, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)(r - p);
	}
	return 0;
}

// 変数型判断(予約語)
int CLexCBL::IsDescriptorType(int nSrcNo, char *p)
{
	// PIC
	if((*p == 'P' || *p == 'p')
	&& (*(p + 1) == 'I' || *(p + 1) == 'i')
	&& (*(p + 2) == 'C' || *(p + 2) == 'c')
	&& (*(p + 3) == ' ' || *(p + 3) == '\r' || *(p + 3) == '\n' || *(p + 3) == '\t')){
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_PIC, p, 3) < 0) return 0;
		m_bEol = FALSE;
		return 3;
	}

	// PICTUER
	if((*p == 'P' || *p == 'p')
	&& (*(p + 1) == 'I' || *(p + 1) == 'i')
	&& (*(p + 2) == 'C' || *(p + 2) == 'c')
	&& (*(p + 3) == 'T' || *(p + 3) == 't')
	&& (*(p + 4) == 'U' || *(p + 4) == 'u')
	&& (*(p + 5) == 'E' || *(p + 5) == 'e')
	&& (*(p + 6) == 'R' || *(p + 6) == 'r')
	&& (*(p + 7) == ' '  || *(p + 7) == '\r' || *(p + 7) == '\n' || *(p + 7) == '\t')){
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_PIC, p, 7) < 0) return 0;
		m_bEol = FALSE;
		return 7;
	}
	return 0;
}

// 変数サイズ定義判断
int CLexCBL::IsPicSize(int nSrcNo, char *p)
{
	char *r;
	// SPACE, '\r', '.' のいずれか迄切り出す
	for(r = p; *r != '\0'; r++){
		if(*r == ' '
		|| *r == '\r'
		|| *r == '\n'
		|| *r == '\t'
		|| *r == CBL_EOL){
			// ただし.の後に文字が続く場合は続行
			if((*r == '.')
			&&((*(r + 1) == '9')
			|| (*(r + 1) == '-')
			|| (*(r + 1) == '\\')
			|| (*(r + 1) == 'B' || *(r + 1) == 'b')
			|| (*(r + 1) == 'Z' || *(r + 1) == 'z'))) continue;
			if(r == p){
				break;
			}else{
				// 頭にＳＰＡＣＥがある場合は除去
				char *j = p + SkipBlank(p);
				// トークン格納
				if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_PICSIZE, j, (int)min(MAX_DESCRIPT_LEN, (r - j))) < 0) return 0;
				m_bEol = TRUE;
				return (int)(r - p);	// 飛ばしたSPACEを考慮して*pを使用する
			}
		}
	}
	return 0;
}

// 変数の拡張設定判断(予約語)
int CLexCBL::IsRedefines(int nSrcNo, char *p)
{
	// REDEFINES
	if((*p == 'R' || *p == 'r')
	&& (*(p + 1) == 'E' || *(p + 1) == 'e')
	&& (*(p + 2) == 'D' || *(p + 2) == 'd')
	&& (*(p + 3) == 'E' || *(p + 3) == 'e')
	&& (*(p + 4) == 'F' || *(p + 4) == 'f')
	&& (*(p + 5) == 'I' || *(p + 5) == 'i')
	&& (*(p + 6) == 'N' || *(p + 6) == 'n')
	&& (*(p + 7) == 'E' || *(p + 7) == 'e')
	&& (*(p + 8) == 'S' || *(p + 8) == 's')){
		if(*(p + 9) == ' ' || *(p + 9) == '\r' || *(p + 9) == '\n' || *(p + 9) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_REDEFINES, p, 9) < 0) return 0;
			m_bEol = FALSE;
			return 9;
		}
	}
	return 0;
}

// 変数の拡張設定判断(予約語)
int CLexCBL::IsOccurs(int nSrcNo, char *p)
{
	// OCCURS
	if((*p == 'O' || *p == 'o')
	&& (*(p + 1) == 'C' || *(p + 1) == 'c')
	&& (*(p + 2) == 'C' || *(p + 2) == 'c')
	&& (*(p + 3) == 'U' || *(p + 3) == 'u')
	&& (*(p + 4) == 'R' || *(p + 4) == 'r')
	&& (*(p + 5) == 'S' || *(p + 5) == 's')){
		if(*(p + 6) == ' ' || *(p + 6) == '\r' || *(p + 6) == '\n' || *(p + 6) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_OCCURS, p, 6) < 0) return 0;
			m_bEol = FALSE;
			return 6;
		}
	}

	// TIMES
	if((*p == 'T' || *p == 't')
	&& (*(p + 1) == 'I' || *(p + 1) == 'i')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'E' || *(p + 3) == 'e')
	&& (*(p + 4) == 'S' || *(p + 4) == 's')){
		if(*(p + 5) == ' ' || *(p + 5) == CBL_EOL || *(p + 5) == '\r' || *(p + 5) == '\n' || *(p + 5) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_SUB, p, 5) < 0) return 0;
			m_bEol = FALSE;
			return 5;
		}
	}

	// INDEXED
	if((*p == 'I' || *p == 'i')
	&& (*(p + 1) == 'N' || *(p + 1) == 'n')
	&& (*(p + 2) == 'D' || *(p + 2) == 'd')
	&& (*(p + 3) == 'E' || *(p + 3) == 'e')
	&& (*(p + 4) == 'X' || *(p + 4) == 'x')
	&& (*(p + 5) == 'E' || *(p + 5) == 'e')
	&& (*(p + 6) == 'D' || *(p + 6) == 'd')){
		if(*(p + 7) == ' ' || *(p + 7) == '\r' || *(p + 7) == '\n' || *(p + 7) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_INDEX, p, 7) < 0) return 0;
			m_bEol = FALSE;
			return 7;
		}
	}

	// BY
	if((*p == 'B' || *p == 'b')
	&& (*(p + 1) == 'Y' || *(p + 1) == 'y')){
		if(*(p + 2) == ' ' || *(p + 2) == '\r' || *(p + 2) == '\n' || *(p + 2) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_SUB, p, 2) < 0) return 0;
			m_bEol = FALSE;
			return 2;
		}
	}
	return 0;
}

// 変数の拡張設定判断(予約語)
int CLexCBL::IsBit(int nSrcNo, char *p)
{
	// BIT
	if((*p == 'B' || *p == 'b')
	&& (*(p + 1) == 'I' || *(p + 1) == 'i')
	&& (*(p + 2) == 'T' || *(p + 2) == 't')){
		if(*(p + 3) == ' ' || *(p + 3) == CBL_EOL || *(p + 3) == '\r' || *(p + 3) == '\n' || *(p + 3) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_BIT, p, 3) < 0) return 0;
			m_bEol = TRUE;
			return 3;
		}
	}
	return 0;
}

// 変数の拡張設定判断(予約語)
int CLexCBL::IsBinary(int nSrcNo, char *p)
{
	// COMP-1 (float)
	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& *(p + 4) == '-'
	&& *(p + 5) == '1'){
		if(*(p + 6) == ' ' || *(p + 6) == CBL_EOL || *(p + 6) == '\r' || *(p + 6) == '\n' || *(p + 6) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 6) < 0) return 0;
			m_bEol = TRUE;
			return 6;
		}
	}

	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& (*(p + 4) == 'U' || *(p + 4) == 'u')
	&& (*(p + 5) == 'T' || *(p + 5) == 't')
	&& (*(p + 6) == 'A' || *(p + 6) == 'a')
	&& (*(p + 7) == 'T' || *(p + 7) == 't')
	&& (*(p + 8) == 'I' || *(p + 8) == 'i')
	&& (*(p + 9) == 'O' || *(p + 9) == 'o')
	&& (*(p + 10) == 'N' || *(p + 10) == 'n')
	&& (*(p + 11) == 'A' || *(p + 11) == 'a')
	&& (*(p + 12) == 'L' || *(p + 12) == 'l')
	&& *(p + 13) == '-'
	&& *(p + 14) == '1'){
		if(*(p + 15) == ' ' || *(p + 15) == CBL_EOL || *(p + 15) == '\n' || *(p + 15) == '\r' || *(p + 15) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 15) < 0) return 0;
			m_bEol = TRUE;
			return 15;
		}
	}

	// COMP-2 (double)
	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& *(p + 4) == '-'
	&& *(p + 5) == '2'){
		if(*(p + 6) == ' ' || *(p + 6) == CBL_EOL || *(p + 6) == '\n' || *(p + 6) == '\r' || *(p + 6) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 6) < 0) return 0;
			m_bEol = TRUE;
			return 6;
		}
	}

	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& (*(p + 4) == 'U' || *(p + 4) == 'u')
	&& (*(p + 5) == 'T' || *(p + 5) == 't')
	&& (*(p + 6) == 'A' || *(p + 6) == 'a')
	&& (*(p + 7) == 'T' || *(p + 7) == 't')
	&& (*(p + 8) == 'I' || *(p + 8) == 'i')
	&& (*(p + 9) == 'O' || *(p + 9) == 'o')
	&& (*(p + 10) == 'N' || *(p + 10) == 'n')
	&& (*(p + 11) == 'A' || *(p + 11) == 'a')
	&& (*(p + 12) == 'L' || *(p + 12) == 'l')
	&& *(p + 13) == '-'
	&& *(p + 14) == '2'){
		if(*(p + 15) == ' ' || *(p + 15) == CBL_EOL || *(p + 15) == '\r' || *(p + 15) == '\n' || *(p + 15) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 15) < 0) return 0;
			m_bEol = TRUE;
			return 15;
		}
	}

	// COMP-3 (M-HOST互換(16進?))
	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& *(p + 4) == '-'
	&& *(p + 5) == '3'){
		if(*(p + 6) == ' ' || *(p + 6) == CBL_EOL || *(p + 6) == '\r' || *(p + 6) == '\n' || *(p + 6) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_PACKED, p, 6) < 0) return 0;
			m_bEol = TRUE;
			return 6;
		}
	}

	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& (*(p + 4) == 'U' || *(p + 4) == 'u')
	&& (*(p + 5) == 'T' || *(p + 5) == 't')
	&& (*(p + 6) == 'A' || *(p + 6) == 'a')
	&& (*(p + 7) == 'T' || *(p + 7) == 't')
	&& (*(p + 8) == 'I' || *(p + 8) == 'i')
	&& (*(p + 9) == 'O' || *(p + 9) == 'o')
	&& (*(p + 10) == 'N' || *(p + 10) == 'n')
	&& (*(p + 11) == 'A' || *(p + 11) == 'a')
	&& (*(p + 12) == 'L' || *(p + 12) == 'l')
	&& *(p + 13) == '-'
	&& *(p + 14) == '3'){
		if(*(p + 15) == ' ' || *(p + 15) == CBL_EOL || *(p + 15) == '\r' || *(p + 15) == '\n' || *(p + 15) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_PACKED, p, 15) < 0) return 0;
			m_bEol = TRUE;
			return 15;
		}
	}

	// COMP-5 (2進)
	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& *(p + 4) == '-'
	&& *(p + 5) == '5'){
		if(*(p + 6) == ' ' || *(p + 6) == CBL_EOL || *(p + 6) == '\r' || *(p + 6) == '\n' || *(p + 6) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 6) < 0) return 0;
			m_bEol = TRUE;
			return 6;
		}
	}

	if((*p == 'C' || *p == 'c')
	&& (*(p + 1) == 'O' || *(p + 1) == 'o')
	&& (*(p + 2) == 'M' || *(p + 2) == 'm')
	&& (*(p + 3) == 'P' || *(p + 3) == 'p')
	&& (*(p + 4) == 'U' || *(p + 4) == 'u')
	&& (*(p + 5) == 'T' || *(p + 5) == 't')
	&& (*(p + 6) == 'A' || *(p + 6) == 'a')
	&& (*(p + 7) == 'T' || *(p + 7) == 't')
	&& (*(p + 8) == 'I' || *(p + 8) == 'i')
	&& (*(p + 9) == 'O' || *(p + 9) == 'o')
	&& (*(p + 10) == 'N' || *(p + 10) == 'n')
	&& (*(p + 11) == 'A' || *(p + 11) == 'a')
	&& (*(p + 12) == 'L' || *(p + 12) == 'l')
	&& *(p + 13) == '-'
	&& *(p + 14) == '5'){
		if(*(p + 15) == ' ' || *(p + 15) == CBL_EOL || *(p + 15) == '\r' || *(p + 15) == '\n' || *(p + 15) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 15) < 0) return 0;
			m_bEol = TRUE;
			return 15;
		}
	}

	// BINARY (2進)
	if((*p == 'B' || *p == 'b')
	&& (*(p + 1) == 'I' || *(p + 1) == 'i')
	&& (*(p + 2) == 'N' || *(p + 2) == 'n')
	&& (*(p + 3) == 'A' || *(p + 3) == 'a')
	&& (*(p + 4) == 'R' || *(p + 4) == 'r')
	&& (*(p + 5) == 'Y' || *(p + 5) == 'y')){
		if(*(p + 6) == ' ' || *(p + 6) == CBL_EOL || *(p + 6) == '\r' || *(p + 6) == '\n' || *(p + 6) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, (m_bHostSize ? CBL_TYPE_PACKED : CBL_TYPE_BINARY), p, 6) < 0) return 0;
			m_bEol = TRUE;
			return 6;
		}
	}

	// PACKED-DECIMAL
	if((*p == 'P' || *p == 'p')
	&& (*(p + 1) == 'A' || *(p + 1) == 'a')
	&& (*(p + 2) == 'C' || *(p + 2) == 'c')
	&& (*(p + 3) == 'K' || *(p + 3) == 'k')
	&& (*(p + 4) == 'E' || *(p + 4) == 'e')
	&& (*(p + 5) == 'D' || *(p + 5) == 'd')
	&& *(p + 6) == '-'
	&& (*(p + 7) == 'D' || *(p + 7) == 'd')
	&& (*(p + 8) == 'E' || *(p + 8) == 'e')
	&& (*(p + 9) == 'C' || *(p + 9) == 'c')
	&& (*(p + 10) == 'I' || *(p + 10) == 'i')
	&& (*(p + 11) == 'M' || *(p + 11) == 'm')
	&& (*(p + 12) == 'A' || *(p + 12) == 'a')
	&& (*(p + 13) == 'L' || *(p + 13) == 'l')){
		if(*(p + 14) == ' ' || *(p + 14) == CBL_EOL || *(p + 14) == '\r' || *(p + 14) == '\n' || *(p + 14) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_PACKED, p, 14) < 0) return 0;
			m_bEol = TRUE;
			return 14;
		}
	}
	return 0;
}

// 算術演算記号の扱い
int CLexCBL::IsSeparate(int nSrcNo, char *p)
{
	char* pStart = p;

	// SIGN IS
	if ((*p == 'S' || *p == 's')
		&& (*(p + 1) == 'I' || *(p + 1) == 'i')
		&& (*(p + 2) == 'G' || *(p + 2) == 'g')
		&& (*(p + 3) == 'N' || *(p + 3) == 'n')) {
		if (*(p + 4) == ' ') {
			p += 4;
			p += SkipBlank(p);
			if ((*p == 'I' || *p == 'i')
				&& (*(p + 1) == 'S' || *(p + 1) == 's')) {
				if (*(p + 2) == ' ') {
					p += 2;
					p += SkipBlank(p);
				}
			}
		}
	}

	// LEADING
	if((*p == 'L' || *p == 'l')
	&& (*(p + 1) == 'E' || *(p + 1) == 'e')
	&& (*(p + 2) == 'A' || *(p + 2) == 'a')
	&& (*(p + 3) == 'D' || *(p + 3) == 'd')
	&& (*(p + 4) == 'I' || *(p + 4) == 'i')
	&& (*(p + 5) == 'N' || *(p + 5) == 'n')
	&& (*(p + 6) == 'G' || *(p + 6) == 'g')){
		if(*(p + 7) == ' '){
			int n = 7;
			n += SkipBlank((p + n));
			for(char *r = (p + n); *r != CBL_EOL && *r != ' ' && *r != '\0'; r++){
				if((*r == 'S' || *r == 's')
				&& (*(r + 1) == 'E' || *(r + 1) == 'e')
				&& (*(r + 2) == 'P' || *(r + 2) == 'p')
				&& (*(r + 3) == 'A' || *(r + 3) == 'a')
				&& (*(r + 4) == 'R' || *(r + 4) == 'r')
				&& (*(r + 5) == 'A' || *(r + 5) == 'a')
				&& (*(r + 6) == 'T' || *(r + 6) == 't')
				&& (*(r + 7) == 'E' || *(r + 7) == 'e')){
					if(*(r + 8) == ' ' || *(r + 8) == CBL_EOL || *(r + 8) == '\r' || *(r + 8) == '\n' || *(r + 8) == '\t'){
						int nLen = (int)((r + 8) - pStart);
						if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_LSEPARATE, pStart, nLen) < 0) return 0;
						m_bEol = TRUE;
						return nLen;
					}
				}
				n++;
			}
		}
	}

	// TRAILING
	if((*p == 'T' || *p == 't')
	&& (*(p + 1) == 'R' || *(p + 1) == 'r')
	&& (*(p + 2) == 'A' || *(p + 2) == 'a')
	&& (*(p + 3) == 'I' || *(p + 3) == 'i')
	&& (*(p + 4) == 'L' || *(p + 4) == 'l')
	&& (*(p + 5) == 'I' || *(p + 5) == 'i')
	&& (*(p + 6) == 'N' || *(p + 6) == 'n')
	&& (*(p + 7) == 'G' || *(p + 7) == 'g')){
		if(*(p + 8) == ' '){
			int n = 8;
			n += SkipBlank((p + n));
			for(char *r = (p + n); *r != CBL_EOL && *r != ' ' && *r != '\0'; r++){
				if((*r == 'S' || *r == 's')
				&& (*(r + 1) == 'E' || *(r + 1) == 'e')
				&& (*(r + 2) == 'P' || *(r + 2) == 'p')
				&& (*(r + 3) == 'A' || *(r + 3) == 'a')
				&& (*(r + 4) == 'R' || *(r + 4) == 'r')
				&& (*(r + 5) == 'A' || *(r + 5) == 'a')
				&& (*(r + 6) == 'T' || *(r + 6) == 't')
				&& (*(r + 7) == 'E' || *(r + 7) == 'e')){
					if(*(r + 8) == ' ' || *(r + 8) == CBL_EOL || *(r + 8) == '\r' || *(r + 8) == '\n' || *(r + 8) == '\t'){
						int nLen = (int)((r + 8) - pStart);
						if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_TSEPARATE, pStart, nLen) < 0) return 0;
						m_bEol = TRUE;
						return nLen;
					}
				}
				n++;
			}
		}
	}

	return 0;
}

// 変数の拡張設定判断(予約語)
int CLexCBL::IsValue(int nSrcNo, char *p)
{
	// VALUE
	if((*p == 'V' || *p == 'v')
	&& (*(p + 1) == 'A' || *(p + 1) == 'a')
	&& (*(p + 2) == 'L' || *(p + 2) == 'l')
	&& (*(p + 3) == 'U' || *(p + 3) == 'u')
	&& (*(p + 4) == 'E' || *(p + 4) == 'e')){
		if(*(p + 5) == ' ' || *(p + 5) == '\r' || *(p + 5) == '\n' || *(p + 5) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_VALUE, p, 5) < 0) return 0;
			m_bEol = FALSE;
			return 5;
		}
	}
	return 0;
}
 
// 数値定数判断
int CLexCBL::IsNumericConst(int nSrcNo, char *p)
{
	char *r;
	if(!m_bLevel){
		if(IsNumericPlus(*p)){
			for(r = p; *r != '\0'; r++){
				if(!IsNumericPlus(*r)) break;
			}
			if(p == r) return 0;
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_NUMCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
			m_bEol = TRUE;
			return (int)(r - p);
		}
	}
	return 0;
}

// 文字列定数判断
int CLexCBL::IsCharConst(int nSrcNo, char *p)
{
	char *r;
	if(*p == '\"'){
		p++;
		for(r = p; *r != '\0'; r++){
			if(*r == '\"'){
				if((*(r + 1) == CBL_EOL)
				|| (*(r + 1) == '\0')) break;
			}
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_CHARCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)((r - p) + 2);	// ""の分を+2
	}else
	if(*p == '\''){
		p++;
		for(r = p; *r != '\0'; r++){
			if(*r == '\''){
				if((*(r + 1) == CBL_EOL)
				|| (*(r + 1) == '\0')) break;
			}
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_CHARCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)((r - p) + 2);	// ''の分を+2
	}

	if(((*p == 'X' || *p == 'x')
	||  (*p == 'B' || *p == 'b'))
	&& (*(p + 1) == '\"')){
		p += 2;
		for(r = p; *r != '\0'; r++){
			if(*r == '\"'){
				if((*(r + 1) == CBL_EOL)
				|| (*(r + 1) == '\0')) break;
			}
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_BINCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)((r - p) + 3);	// X""の分を+3
	}else
	if(((*p == 'X' || *p == 'x')
	||  (*p == 'B' || *p == 'b'))
	&& (*(p + 1) == '\'')){
		p += 2;
		for(r = p; *r != '\0'; r++){
			if(*r == '\''){
				if((*(r + 1) == CBL_EOL)
				|| (*(r + 1) == '\0')) break;
			}
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_BINCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)((r - p) + 3);	// X''の分を+3
	}

	if((*p == 'N' || *p == 'n')
	&& ((*(p + 1) == 'C' || *(p + 1) == 'c')
	||  (*(p + 1) == 'A' || *(p + 1) == 'a'))
	&& (*(p + 2) == '\"')){
		p += 3;
		for(r = p; *r != '\0'; r++){
			if(*r == '\"'){
				if((*(r + 1) == CBL_EOL)
				|| (*(r + 1) == '\0')) break;
			}
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_JPNCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)((r - p) + 4);	// NC""の分を+4
	}else
	if((*p == 'N' || *p == 'n')
	&& ((*(p + 1) == 'C' || *(p + 1) == 'c')
	||  (*(p + 1) == 'A' || *(p + 1) == 'a'))
	&& (*(p + 2) == '\'')){
		p += 3;
		for(r = p; *r != '\0'; r++){
			if(*r == '\''){
				if((*(r + 1) == CBL_EOL)
				|| (*(r + 1) == '\0')) break;
			}
		}
		if(p == r) return 0;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_JPNCONST, p, (int)min(MAX_DESCRIPT_LEN, (r - p))) < 0) return 0;
		m_bEol = TRUE;
		return (int)((r - p) + 4);	// NC''の分を+4
	}
	return 0;
}

// 予約語だが特に解析に関係ない(?)もの
int CLexCBL::IsNullConst(int nSrcNo, char *p)
{
	// SYNC
	if((*p == 'S' || *p == 's')
	&& (*(p + 1) == 'Y' || *(p + 1) == 'y')
	&& (*(p + 2) == 'N' || *(p + 2) == 'n')
	&& (*(p + 3) == 'C' || *(p + 3) == 'c')){
		if(*(p + 4) == ' ' || *(p + 4) == CBL_EOL || *(p + 4) == '\r' || *(p + 4) == '\n' || *(p + 4) == '\t'){
			// トークン格納
			if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_NULL, p, 4) < 0) return 0;
			m_bEol = TRUE;
			return 4;
		}
	}
	return 0;
}

// 区切文字判断
int CLexCBL::IsEOL(int nSrcNo, char *p)
{
	if(*p == CBL_EOL){
		// 区切り文字後の最初の数値定数はレベル番号
		m_bLevel = TRUE;
		// トークン格納
		if(TokenCBL_Push(nSrcNo, m_nPhysLine, m_nLogiLine, CBL_TYPE_EOL, p, 1) < 0) return 0;
		return 1;
	}

	return 0;
}

// 空白読み飛ばし
int CLexCBL::SkipBlank(char *p)
{
	int n = 0;
	while(1){
		if(*(p + n) == ' '
		|| *(p + n) == '\t'
		|| *(p + n) == '\r'
		|| *(p + n) == HOST_EOF){
			n++;
		}else{
			break;
		}
	}
	return n;
}

// 行末まで読み飛ばし
int CLexCBL::SkipEOL(char *p)
{
	int n = 0;
	while(1){
		if(*(p + n) == '\0'
		|| *(p + n) == '\n'
		|| *(p + n) == HOST_EOF) break;
	}
	return n;
}

// エラーとして扱わない記号
void CLexCBL::SetSign(char *c)
{
	int i;
	for(i = 0; i < MAX_SIGN; i++) m_cSign[i] = '\0';
	for(i = 0; i < MAX_SIGN; i++){
		if(*(c + i) == '\0') return;
		if(*(c + i) == ' ') return;
		m_cSign[i] = *(c + i);
	}
}

// 記号判定
BOOL CLexCBL::IsSign(char c)
{
	for(int i = 0; i < MAX_SIGN; i++){
		if(m_cSign[i] == '\0') return FALSE;
		if(m_cSign[i] == c) return TRUE;
	}
	return FALSE;
}

// 日本語文字判定
BOOL CLexCBL::IsJapan(char c)
{
	return (IsDBCSLeadByte(c));
}

// 数字か判定
BOOL CLexCBL::IsNumeric(char c)
{
	return ((c >= '0' && c <= '9'));
}

BOOL CLexCBL::IsNumericPlus(char c)
{
	// + - , もおｋ
	return ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == ',');
}

// 英字か判定
BOOL CLexCBL::IsAlphabet(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

// トークン取得
LPTOKEN_CBL CLexCBL::GetToken(const int nToken)
{
	if(GetTokenCount() < nToken) return NULL;
	return m_lpTok_cbl[nToken];
}

// ソース情報取得
int CLexCBL::GetSourceInfo(const int nIndex, LPSOURCEINFO lpSrcInfo)
{
//	if(GetSourceCount() < nIndex) return -1;
	if (nIndex < 0 || nIndex >= GetSourceCount()) return -1;
	CopyMemory(lpSrcInfo, m_lpSrcInfo[nIndex], sizeof(SOURCEINFO));
	return 0;
}
