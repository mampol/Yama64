/*-------------------------------------------------------------------------------------

	CAnaForm.cpp

-------------------------------------------------------------------------------------*/
#include "CAnaCBL.h"

// コンストラクタ
CAnaForm::CAnaForm(void)
 : m_bComplete(FALSE),
   m_lpbtFormData(NULL),
   m_dwFormSize(0),
   m_dwCOBOLSize(0),
   m_nOvdNames(0)
{
#ifdef _DEBUG_MODE_
	printf("CAnaForm コンストラクタ\n");
#endif	//_DEBUG_MODE_
	m_szFromFile[0] = '\0';
	m_szCobolFile[0] = '\0';
}

// デストラクタ
CAnaForm::~CAnaForm()
{
#ifdef _DEBUG_MODE_
	printf("CAnaForm デストラクタ\n");
#endif	//_DEBUG_MODE_
	ClearBuffer();
}

// メモリ解放
void CAnaForm::ClearBuffer()
{
	for(std::vector<LPRECORD_FORM>::size_type i = 0; i < m_lpRecForm.size(); i++){
		if(GlobalFree(m_lpRecForm[i]) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
	}
	if(m_lpbtFormData){
		if(GlobalFree(m_lpbtFormData) != NULL) AddTrace("メモリ開放に失敗", TRACE_ALLOC);
	}
	m_lpRecForm.clear();
	m_lpbtFormData = NULL;
}

// ＦＯＲＭ解析
int CAnaForm::AnaForm(void)
{
	if(m_lpbtFormData == NULL){
		AddTrace("FORMデータ無し", TRACE_FILE);
		Cancel();
		return -1;
	}
	if(IsCancel()) return -1;

	LPSMD_HEAD lpHead = NULL;

	// ヘッダ部取得
	AddTrace("ﾍｯﾀﾞ部取得");
	if((lpHead = GetHeader()) == NULL){
		AddTrace("FORMﾍｯﾀﾞ部取得不可", TRACE_FILE);
		Cancel();
		return -2;
	}
	if(IsCancel()) return -1;

	// ヘッダチェック
	AddTrace("ﾍｯﾀﾞﾁｪｯｸ");
	if(!CheckHeader(lpHead)){
		AddTrace("FORMﾍｯﾀﾞ部情報異常", TRACE_FILE);
		Cancel();
		return -4;
	}
	if(IsCancel()) return -1;

	// オーバレイ定義名の取得
	GetOvdNames(lpHead);
	if(IsCancel()) return -1;

	// 解析
	if(lpHead->wRecDataAdr == 0){
		AddTrace("レコード定義情報無し");
		return (AnaForm_NoRecordSet(lpHead));
	}else{
		AddTrace("レコード定義情報解析");
		return (AnaForm_RecordSet(lpHead));
	}
}

// ＦＯＲＭ解析
int CAnaForm::AnaForm_RecordSet(LPSMD_HEAD lpHead)
{
	LPSMD_RECDATA lpRecData = (LPSMD_RECDATA)GetRecData(0, lpHead);
	for(int i = 1; lpRecData != NULL; i++){
		// メモリ確保
		LPRECORD_FORM lpRecForm = (LPRECORD_FORM)GlobalAlloc(GPTR, sizeof(RECORD_FORM));
		if(lpRecForm == NULL){
			AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
			return -1;
		}
		// 格納
		lpRecForm->wItenNo = i;
		lpRecForm->btLevel = lpRecData->btLevelNo;
		lpRecForm->nLevelLen = (int)lpRecData->btLevelNo;
		if(lpRecForm->btLevel > 1) lpRecForm->nLevelLen += 3;
		CopyMemory((LPBYTE)lpRecForm->btName, (LPBYTE)lpRecData->btItemName, sizeof(lpRecData->btItemName));
		for(LPBYTE p = lpRecForm->btName; *p != '\0'; p++){
			if(*p == '\0'){
				*p = '\0';
				break;
			}
		}
		lpRecForm->wItemLen = lpRecData->wItrmLen;
		lpRecForm->wItemType = lpRecData->btItrmType;
		lpRecForm->btFloatLen = 0;
		lpRecForm->wOccurs = lpRecData->wItrmOccurs;
		lpRecForm->bSigned = FALSE;
		m_lpRecForm.push_back(lpRecForm);
		// レコードデータ取得
		lpRecData = (LPSMD_RECDATA)GetRecData(i, lpHead);
	}
	return 0;
}

// ＦＯＲＭ解析(レコード定義無し)
int CAnaForm::AnaForm_NoRecordSet(LPSMD_HEAD lpHead)
{
	LPSMD_ITEMNAME lpItemName = NULL;
	LPSMD_ITEMDATA lpItemData = NULL;
	LPSMD_GLITEMDATA lpGlData = NULL;
	int nCount;

	// 項目数取得
	if((nCount = GetItemCount(lpHead)) < 1){
		AddTrace("項目データ無し", TRACE_ANA);
		return -2;
	}

	for(int i = 0; i < nCount; i++){
		// 項目名部取得
		if((lpItemName = GetItemName(i, lpHead)) == NULL){
			AddTrace("項目データ名称不明", TRACE_ANA);
			return -3;
		}
		// 繰返し項目の判定
		if(lpItemName->btName[0] == 0x5D) lpItemName->btName[0] = 'A';
		// 項目属性部取得
		if((lpItemData = GetItemData(lpItemName)) == NULL){
			AddTrace("項目データ属性不明", TRACE_ANA);
			return -4;
		}
		// 大域属性部取得
		if((lpGlData = GetGlData(lpItemData)) == NULL){
			AddTrace("項目データ大域属性不明", TRACE_ANA);
			return -5;
		}
		// メモリ確保
		LPRECORD_FORM lpRecForm = (LPRECORD_FORM)GlobalAlloc(GPTR, sizeof(RECORD_FORM));
		if(lpRecForm == NULL){
			AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
			return -1;
		}
		// 格納
		lpRecForm->wItenNo = lpItemData->wItemNo;
		lpRecForm->btLevel = 1;
		lpRecForm->nLevelLen = 1;
		if(lpRecForm->btLevel > 1) lpRecForm->nLevelLen += 3;
		CopyMemory((LPBYTE)lpRecForm->btName, (LPBYTE)lpItemName->btName, sizeof(lpItemName->btName));
		for(LPBYTE p = lpRecForm->btName; *p != '\0'; p++){
			if(*p == '\0'){
				*p = '\0';
				break;
			}
		}
		lpRecForm->wItemLen = lpItemData->wRecLen;
		lpRecForm->wItemType = lpGlData->btItemType;
		lpRecForm->btFloatLen = 0;
		lpRecForm->wOccurs = 0;
		m_lpRecForm.push_back(lpRecForm);
	}
	return 0;
}

// ヘッダ部取得
LPSMD_HEAD CAnaForm::GetHeader(void)
{
	LPSMD_HEAD lpHead = (LPSMD_HEAD)m_lpbtFormData;
	if(lpHead == NULL) return NULL;

	lpHead->wSizeY = HTONS(lpHead->wSizeY);
	lpHead->wSizeX = HTONS(lpHead->wSizeX);
	lpHead->wOutItemCount = HTONS(lpHead->wOutItemCount);
	lpHead->wFixedItemCount = HTONS(lpHead->wFixedItemCount);
	lpHead->wGroupAdr = HTONS(lpHead->wGroupAdr);
	lpHead->wItemAdr = HTONS(lpHead->wItemAdr);
	lpHead->wItemDataAdr = HTONS(lpHead->wItemDataAdr);
	lpHead->wItemDataAdr2 = HTONS(lpHead->wItemDataAdr2);
	lpHead->wGlDataAdr = HTONS(lpHead->wGlDataAdr);
	lpHead->wOvdNameAdr = HTONS(lpHead->wOvdNameAdr);
	lpHead->wExDataAdr = HTONS(lpHead->wExDataAdr);
	lpHead->wRecNameAdr = HTONS(lpHead->wRecNameAdr);
	lpHead->wRecDataAdr = HTONS(lpHead->wRecDataAdr);

	m_wSizeY = lpHead->wSizeY;
	m_wSizeX = lpHead->wSizeX;
	m_wOutItemCount = lpHead->wOutItemCount;
	m_wFixedItemCount = lpHead->wFixedItemCount;

	return lpHead;
}

// ヘッダ部チェック
BOOL CAnaForm::CheckHeader(const LPSMD_HEAD lpHead)
{
	for(int i = 0; i < sizeof(lpHead->btSmedName); i++){
		if(*(lpHead->btSmedName + i) > 0x7F) return FALSE;
	}
	if(lpHead->wSizeX < 0) return FALSE;
	if(lpHead->wSizeY < 0) return FALSE;
	if(lpHead->wFixedItemCount < 0) return FALSE;
	if(lpHead->wOutItemCount < 0) return FALSE;
	if(lpHead->wItemAdr > m_dwFormSize) return FALSE;
	if(lpHead->wGroupAdr > m_dwFormSize) return FALSE;
	if(lpHead->wExDataAdr > m_dwFormSize) return FALSE;
	if(lpHead->wItemDataAdr > m_dwFormSize) return FALSE;
	if(lpHead->wItemDataAdr2 > m_dwFormSize) return FALSE;
	if(lpHead->wRecDataAdr > m_dwFormSize) return FALSE;
	return TRUE;
}

// レコード定義部取得
LPSMD_RECDATA CAnaForm::GetRecData(const WORD wItemNo,
								   const LPSMD_HEAD lpHead)
{
	LPSMD_RECDATA lpRecData = (LPSMD_RECDATA)((m_lpbtFormData + lpHead->wRecDataAdr) + (sizeof(SMD_RECDATA) * wItemNo));
	if(lpRecData == NULL) return NULL;
	if(lpRecData->btDiscript != 0x18) return NULL;
	lpRecData->wItrmLen = HTONS(lpRecData->wItrmLen);
	lpRecData->wItrmOccurs = HTONS(lpRecData->wItrmOccurs);
	lpRecData->wItemFiller1 = HTONS(lpRecData->wItemFiller1);
	lpRecData->wItemFiller2 = HTONS(lpRecData->wItemFiller2);
	lpRecData->wItemFiller3 = HTONS(lpRecData->wItemFiller3);
	lpRecData->wItemFiller4 = HTONS(lpRecData->wItemFiller4);
	return lpRecData;
}

// 項目数取得
int CAnaForm::GetItemCount(const LPSMD_HEAD lpHead)
{
	int nCount = 0;
	LPSMD_ITEMNAME lpItemName = (LPSMD_ITEMNAME)(m_lpbtFormData + lpHead->wItemAdr);
	while(lpItemName){
		if((lpItemName->btName[0] == 0xFF)
		&& (lpItemName->btName[1] == 0xFF)) break;
		nCount++;
		lpItemName = (LPSMD_ITEMNAME)((m_lpbtFormData + lpHead->wItemAdr) + (sizeof(SMD_ITEMNAME) * nCount));
	}
	return nCount;
}

// 項目名部取得
LPSMD_ITEMNAME CAnaForm::GetItemName(const WORD wItemNo,
									 const LPSMD_HEAD lpHead)
{
	LPSMD_ITEMNAME lpItemName = (LPSMD_ITEMNAME)((m_lpbtFormData + lpHead->wItemAdr) + (sizeof(SMD_ITEMNAME) * wItemNo));
	if(lpItemName == NULL) return NULL;
	if(lpItemName->btName[0] == '\0') return NULL;
	// ' ' ～ '/'
	if((lpItemName->btName[0] > 0x19)
	&& (lpItemName->btName[0] < 0x30)) return NULL;
	// ':' ～ '@'
	if((lpItemName->btName[0] > 0x39)
	&& (lpItemName->btName[0] < 0x41)) return NULL;
	// '[' ～ '`'
	if((lpItemName->btName[0] > 0x5A)
	&& (lpItemName->btName[0] < 0x61)) return NULL;
	// '{' ～ '~'
	if((lpItemName->btName[0] > 0x7A)
	&& (lpItemName->btName[0] < 0x7F)) return NULL;
	if(lpItemName->wPos == '\0') return NULL;
	lpItemName->wPos = HTONS((lpItemName->wPos - 0x80));
	return lpItemName;
}

// 項目属性部取得
LPSMD_ITEMDATA CAnaForm::GetItemData(const LPSMD_ITEMNAME lpItemName)
{
	LPSMD_ITEMDATA lpItemData = (LPSMD_ITEMDATA)(m_lpbtFormData + lpItemName->wPos);
	if(lpItemData == NULL) return NULL;
	lpItemData->wGlPos = HTONS(lpItemData->wGlPos);
	lpItemData->wExPos = HTONS(lpItemData->wExPos);
	lpItemData->wLine = HTONS(lpItemData->wLine);
	lpItemData->wColumn = HTONS(lpItemData->wColumn);
	lpItemData->wItemLen = HTONS(lpItemData->wItemLen);
	lpItemData->wRecLen = HTONS(lpItemData->wRecLen);
	lpItemData->wRecPos = HTONS(lpItemData->wRecPos);
	lpItemData->wItemNo = HTONS(lpItemData->wItemNo);
	return lpItemData;
}

// 大域属性部取得
LPSMD_GLITEMDATA CAnaForm::GetGlData(const LPSMD_ITEMDATA lpItemData)
{
	LPSMD_GLITEMDATA lpGlData = (LPSMD_GLITEMDATA)(m_lpbtFormData + lpItemData->wGlPos);
	if(lpGlData == NULL) return NULL;

	lpGlData->wAttribute = HTONS(lpGlData->wAttribute);
	lpGlData->wEdit = HTONS(lpGlData->wEdit);
	switch(lpGlData->btItemType){
	case SMD_TYPE_GL_X:
		lpGlData->btItemType = SMD_TYPE_X;
		break;
	case SMD_TYPE_GL_9:
		lpGlData->btItemType = SMD_TYPE_9;
		break;
	case SMD_TYPE_GL_N:
		lpGlData->btItemType = SMD_TYPE_N;
		break;
	}
	return lpGlData;
}

// オーバレイ定義名の取得
int CAnaForm::GetOvdNames(LPSMD_HEAD lpHead)
{
	m_nOvdNames = 0;
	ZeroMemory(&m_stOvdNames, sizeof(SMD_OVDNAMES));
	if(lpHead->wOvdNameAdr < 1) return 0;
	if(lpHead->wOvdNameAdr > m_dwFormSize) return 0;
	BYTE btOvdNamesLen = *(m_lpbtFormData + lpHead->wOvdNameAdr);
	LPBYTE lpbtOvdNames = m_lpbtFormData + lpHead->wOvdNameAdr;
	// ４Ｂｙｔｅ切り捨て
	btOvdNamesLen -= 4;
	lpbtOvdNames += 4;
	// オーバレイ登録数
	m_nOvdNames = btOvdNamesLen / CBL_FILE_TITLE_LEN;
	// オーバレイ登録名取得
	for(int i = 0; i < m_nOvdNames; i++){
		CopyMemory(m_stOvdNames.btOvdName[i], lpbtOvdNames + (i * CBL_FILE_TITLE_LEN), CBL_FILE_TITLE_LEN);
		m_stOvdNames.btOvdName[i][CBL_FILE_TITLE_LEN] = '\0';
	}
	return m_nOvdNames;
}

BOOL CAnaForm::MakeRec(const LPRECORD_FORM lpRecForm,
					   char *pszRec,
					   int nMaxBuffer,
					   int nSeqLen)
{
	int i = 0, n, j;
	char szVal[MAX_BUFFER];

	// 行番号
	for(; i < nSeqLen; i++){
		*(pszRec + i) = '0';
		if(i > nMaxBuffer) return FALSE;
	}

	// レベル番号
	for(j = 0; j < lpRecForm->nLevelLen; i++, j++){
		*(pszRec + i) = ' ';
		if(i > nMaxBuffer) return FALSE;
	}
	wsprintf(szVal, "%02d", lpRecForm->btLevel);
	n = i + (int)strlen(szVal);
	for(j = 0; i < n; i++, j++){
		*(pszRec + i) = szVal[j];
		if(i > nMaxBuffer) return FALSE;
	}

	// SPACE
	n = i + (4 - (int)strlen(szVal));
	for(; i < n; i++){
		*(pszRec + i) = ' ';
		if(i > nMaxBuffer) return FALSE;
	}

	// 項目名
//	strcpy(szVal, (const char *)lpRecForm->btName);
	strcpy_s(szVal, sizeof(szVal), (const char *)lpRecForm->btName);
	n = i + (int)strlen(szVal);
	for(j = 0; i < n; i++, j++){
		if(szVal[j] == ' ') break;
		*(pszRec + i) = szVal[j];
		if(i > nMaxBuffer) return FALSE;
	}

	if((lpRecForm->wItemType == SMD_TYPE_GROUP)
	&& (lpRecForm->wOccurs == 0)){
		*(pszRec + i) = '\0';
	}else{
		// 40BYTEまでSPACEで埋める
		if(i > 39){
			n = i + 4;
		}else{
			n = 39;
		}
		for(; i < n; i++){
			*(pszRec + i) = ' ';
			if(i > nMaxBuffer) return FALSE;
		}

		// 変数型定義
		switch(lpRecForm->wItemType){
		case SMD_TYPE_X:
			wsprintf(szVal, CHAR_X, lpRecForm->wItemLen);
			break;
		case SMD_TYPE_9:
			if(lpRecForm->btFloatLen > 0){
				if(lpRecForm->bSigned){
					wsprintf(szVal, CHAR_F, lpRecForm->wItemLen - lpRecForm->btFloatLen, lpRecForm->btFloatLen);
				}else{
					wsprintf(szVal, CHAR_SF, lpRecForm->wItemLen - lpRecForm->btFloatLen, lpRecForm->btFloatLen);
				}
			}else{
				if(lpRecForm->bSigned){
					wsprintf(szVal, CHAR_S, lpRecForm->wItemLen);
				}else{
					wsprintf(szVal, CHAR_9, lpRecForm->wItemLen);
				}
			}
			break;
		case SMD_TYPE_N:
			wsprintf(szVal, CHAR_N, lpRecForm->wItemLen / 2);
			break;
		case SMD_TYPE_GROUP:
			szVal[0] = '\0';
			break;
		default:
//			strcpy(szVal, CHAR_ERROR);
			strcpy_s(szVal, sizeof(szVal), CHAR_ERROR);
			AddTrace("項目種別判別不能ｴﾗｰ", TRACE_ANA);
			break;
		}
		n = i + (int)strlen(szVal);
		for(j = 0; i < n; i++, j++){
			*(pszRec + i) = szVal[j];
			if(i > nMaxBuffer) return FALSE;
		}

		// 繰返し
		if(lpRecForm->wOccurs > 0){
			if(lpRecForm->wItemType != SMD_TYPE_GROUP){
				for(j = 0; j < 2; i++, j++){
					*(pszRec + i) = ' ';
					if(i > nMaxBuffer) return FALSE;
				}
			}
			wsprintf(szVal, CHAR_OCCURS, lpRecForm->wOccurs);
			n = i + (int)strlen(szVal);
			for(j = 0; i < n; i++, j++){
				*(pszRec + i) = szVal[j];
				if(i > nMaxBuffer) return FALSE;
			}
		}
	}

	// ピリオド
	*(pszRec + i) = '.';
	i++;

	// 改行
	*(pszRec + i) = '\r';
	i++;
	*(pszRec + i) = '\n';
	i++;

	// NULL
	*(pszRec + i) = '\0';

	return TRUE;
}

// ＦＯＲＭ解析
DWORD CAnaForm::LexForm(const char *pszFormFile)
{
	// 初期化
	InitAnalysys(STAGE_FORM);

	// バッファの初期化
	ClearBuffer();

	DWORD dwStTime = timeGetTime();

	AddTrace("FORM解析開始...");

	m_bComplete = FALSE;
//	strcpy(m_szFromFile, pszFormFile);
	strcpy_s(m_szFromFile, sizeof(m_szFromFile), pszFormFile);

	HANDLE hFile = CreateFile(m_szFromFile,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		AddTrace("指定されたﾌｧｲﾙが開けません", TRACE_FILE);
		Cancel();
		return 0;
	}

	m_dwFormSize = GetFileSize(hFile, NULL);
	if(m_dwFormSize < 1){
		CloseHandle(hFile);
		AddTrace("ﾌｧｲﾙｻｲｽﾞは0Byteです", TRACE_FILE);
		Cancel();
		return 0;
	}

	m_lpbtFormData = (LPBYTE)GlobalAlloc(GPTR, m_dwFormSize + 1);
	if(m_lpbtFormData == NULL){
		CloseHandle(hFile);
		AddTrace("メモリ不足が発生しています", TRACE_ALLOC);
		Cancel();
		return 0;
	}

	DWORD dwRead;
	if(!ReadFile(hFile, (LPBYTE)m_lpbtFormData, m_dwFormSize, &dwRead, NULL)){
		GlobalFree(m_lpbtFormData);
		CloseHandle(hFile);
		AddTrace("ﾌｧｲﾙ読込みｴﾗｰ", TRACE_FILE);
		Cancel();
		return 0;
	}

	CloseHandle(hFile);

	if(dwRead < 1){
		GlobalFree(m_lpbtFormData);
		AddTrace("ﾌｧｲﾙ読込みｴﾗｰ", TRACE_FILE);
		Cancel();
		return 0;
	}

	AddTrace("ﾌｧｲﾙ読込み完了");

	if(AnaForm() != 0){
		GlobalFree(m_lpbtFormData);
		Cancel();
		return 0;
	}

	GlobalFree(m_lpbtFormData);
	m_lpbtFormData = NULL;

	m_bComplete = TRUE;
	AddTrace("FORM解析完了");

	// 終了
	EndAnalysys();

	return (timeGetTime() - dwStTime);
}

// ＣＯＢＯＬソース出力
DWORD CAnaForm::CreateSource(const char *pszCBLFile, int nSeqLen)
{
	// 初期化
	InitAnalysys(STAGE_FORMSRC);

	DWORD dwStTime = timeGetTime();

	if(!m_bComplete){
		AddTrace("FORMの解析が完了していません", TRACE_ANA);
		Cancel();
		return 0;
	}

	AddTrace("FORMからCOBOLソースコード生成開始...");

	m_szCobolFile[0] = '\0';

	HANDLE hFile = CreateFile(pszCBLFile,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		AddTrace("出力ﾌｧｲﾙｵｰﾌﾟﾝｴﾗｰ", TRACE_FILE);
		Cancel();
		return 0;
	}
	char szRec[MAX_SUBDATA_LEN];
	DWORD dwWrite;
	m_dwCOBOLSize = 0;
	for(int i = 0; i < (int)m_lpRecForm.size(); i++){
		ZeroMemory(&szRec, MAX_SUBDATA_LEN);
		if(!MakeRec(m_lpRecForm[i], szRec, MAX_SUBDATA_LEN, nSeqLen)){
			AddTrace("出力ﾌｧｲﾙﾊﾞｯﾌｧｵｰﾊﾞｰﾌﾛｰ", TRACE_FILE);
			Cancel();
			return 0;
		}
		if(!WriteFile(hFile, (char *)szRec, (DWORD)strlen(szRec), &dwWrite, NULL)){
			AddTrace("ﾌｧｲﾙ出力ｴﾗｰ", TRACE_FILE);
			Cancel();
			return 0;
		}
		m_dwCOBOLSize += dwWrite;
	}
	CloseHandle(hFile);

//	strcpy(m_szCobolFile, pszCBLFile);
	strcpy_s(m_szCobolFile, sizeof(m_szCobolFile), pszCBLFile);

	AddTrace("FORMからCOBOLソースコード生成完了");

	// 終了
	EndAnalysys();

	return (timeGetTime() - dwStTime);
}

// ＣＢＬソース削除
BOOL CAnaForm::RemoveSource()
{
	if(strlen(m_szCobolFile) < 1){
		AddTrace("COBOLｿｰｽの削除はできません", TRACE_FILE);
		return FALSE;
	}
	WIN32_FIND_DATA w32fd;
	ZeroMemory(&w32fd, sizeof(WIN32_FIND_DATA));
	HANDLE hFind = FindFirstFile(m_szCobolFile, &w32fd);
	if(hFind == INVALID_HANDLE_VALUE){
		AddTrace("COBOLｿｰｽの削除はできません", TRACE_FILE);
		return FALSE;
	}
	FindClose(hFind);

	if(!DeleteFile(m_szCobolFile)) return FALSE;

	m_szCobolFile[0] = '\0';

	AddTrace("FORMから生成したCOBOLｿｰｽの削除");
	return TRUE;
}

// レコードの取得
LPRECORD_FORM CAnaForm::GetRecord(const int nRecord)
{
	if(GetRecordCount() < nRecord) return NULL;
	return m_lpRecForm[nRecord];
}
