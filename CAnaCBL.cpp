/*-------------------------------------------------------------------------------------

	CAnaCBL.cpp

-------------------------------------------------------------------------------------*/
#include "CAnaCBL.h"

// コンストラクタ
CAnaCBL::CAnaCBL()
 : m_lpCLexCBL(NULL),
   m_lpCParserCBL(NULL),
   m_lpCAnaForm(NULL),
   m_bAnalysys(FALSE),
   m_dwLex(0),
   m_dwParser(0),
   m_dwLexForm(0),
   m_dwCreateSource(0),
   m_nCodeCheck(0),
   m_nTokenIndex(0),
   m_nRecordIndex(0),
   m_nFormRecordIndex(0)
{
	ZeroMemory(&m_stAnalysys, sizeof(SYSTEMTIME));
#ifdef _DEBUG_MODE_
	printf("CAnaCBL コンストラクタ\n");
#endif	//_DEBUG_MODE_
}

// デストラクタ
CAnaCBL::~CAnaCBL()
{
#ifdef _DEBUG_MODE_
	printf("CAnaCBL デストラクタ\n");
#endif	//_DEBUG_MODE_
	Clean();
}

// 初期化
BOOL CAnaCBL::Initialize(HWND hWndMain)
{
	// クリーンアップ
	Clean();

	// 構文解析クラス
	m_lpCLexCBL = (CLexCBL *)new CLexCBL();
	if(!m_lpCLexCBL) return FALSE;
#ifdef _DEBUG_MODE_
	printf("CLexCBL インスタンス\n");
#endif	//_DEBUG_MODE_

	// 意味解析クラス
	m_lpCParserCBL = (CParserCBL *)new CParserCBL();
	if(!m_lpCParserCBL) return FALSE;
#ifdef _DEBUG_MODE_
	printf("CParserCBL インスタンス\n");
#endif	//_DEBUG_MODE_

	// ＦＯＲＭ解析クラス
	m_lpCAnaForm = (CAnaForm *)new CAnaForm();
	if(!m_lpCAnaForm) return FALSE;
#ifdef _DEBUG_MODE_
	printf("CAnaForm インスタンス\n");
#endif	//_DEBUG_MODE_

	SetWindowHandle(hWndMain);

	return TRUE;
}

// クリーンアップ
void CAnaCBL::Clean()
{
	// タイマ初期化
	m_dwLex = 0;
	m_dwParser = 0;
	m_dwLexForm = 0;
	m_dwCreateSource = 0;
	// インデックス初期化
	m_nTokenIndex = 0;
	m_nRecordIndex = 0;
	m_nFormRecordIndex = 0;
	if(m_lpCLexCBL){
		delete m_lpCLexCBL;
		m_lpCLexCBL = NULL;
#ifdef _DEBUG_MODE_
		printf("CLexCBL インスタンス破棄\n");
#endif	//_DEBUG_MODE_
	}
	if(m_lpCParserCBL){
		delete m_lpCParserCBL;
		m_lpCParserCBL = NULL;
#ifdef _DEBUG_MODE_
		printf("CParserCBL インスタンス破棄\n");
#endif	//_DEBUG_MODE_
	}
	if(m_lpCAnaForm){
		delete m_lpCAnaForm;
		m_lpCAnaForm = NULL;
#ifdef _DEBUG_MODE_
		printf("CAnaForm インスタンス破棄\n");
#endif	//_DEBUG_MODE_
	}
}

// ＣＯＢＯＬソース設定
int CAnaCBL::AddSource(const char *pszSourcePath)
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->AddSourceFile(pszSourcePath,
		NULL, (const LPVOID)SRC_TYPE_CBL));
}

int CAnaCBL::AddSourceCode(const char *pszSourceCode)
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->AddSourceCode(pszSourceCode, (DWORD)strlen(pszSourceCode),
		"編集されたソースコード", (const LPVOID)SRC_TYPE_EDIT));
}

// ＳＭＥＤソース設定
int CAnaCBL::AddForm(const char *pszFormPath,
					 const char *pszCobolSource,
					 const int nSeq/* = 6*/)
{
	if(!m_lpCAnaForm) return -1;
	if(!m_lpCLexCBL) return -1;

	// 富士通ＦＯＲＭの解析
	m_dwLexForm = m_lpCAnaForm->LexForm(pszFormPath);
	if(!m_lpCAnaForm->ResultLexForm()) return -1;

	// ＣＯＢＯＬソースの出力
	m_dwCreateSource = m_lpCAnaForm->CreateSource(pszCobolSource, nSeq);
	if(!m_lpCAnaForm->ResultLexForm()) return -1;

	// ＣＯＢＯＬソースの設定
	return (m_lpCLexCBL->AddSourceFile(pszCobolSource, pszFormPath, (const LPVOID)SRC_TYPE_SMD));
}

// 設定したソースの除外
int CAnaCBL::RemSource(const int nNo)
{
	if(!m_lpCLexCBL) return -1;
	if(nNo < 0){
		// nNo < 0 は全削除
		return (m_lpCLexCBL->RemoveAllSources());
	}else{
		return (m_lpCLexCBL->RemoveSource(nNo));
	}
}

// 設定したソースの再設定
int CAnaCBL::RepSource(const int nNo, const char *pszSourcePath)
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->ReplaceSourceFile(nNo,
		pszSourcePath,
		NULL, (const LPVOID)SRC_TYPE_CBL));
}

int CAnaCBL::RepSourceCode(const int nNo, const char *pszSourceCode)
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->ReplaceSourceCode(nNo,
		pszSourceCode, (DWORD)strlen(pszSourceCode),
		"編集されたソースコード", (const LPVOID)SRC_TYPE_EDIT));
}

// 設定したソースの再読込み
int CAnaCBL::ReloadSource(const int nNo)
{
	if(!m_lpCLexCBL) return -1;
	if(nNo < 0){
		// nNo < 0 は全部
		return (m_lpCLexCBL->ReloadAllSourceFile());
	}else{
		return (m_lpCLexCBL->ReloadSourceFile(nNo));
	}
}

// ＣＯＢＯＬソース解析
BOOL CAnaCBL::Analysys(const int nSeq/* = 6*/,
					   const int nMaxLine/* = 0*/,
					   const BOOL bHostMode/* = FALSE*/,
					   char *pSign/* = NULL*/,
					   const int nCompPos/* = 0*/,
					   const BOOL bCodeCheck/* = TRUE*/)
{
	ZeroMemory(&m_stAnalysys, sizeof(SYSTEMTIME));
	m_nCodeCheck = 0;

	m_bAnalysys = TRUE;	// 解析中フラグ

	BOOL bReturnValue = TRUE;
	try{
		// コボルソースをトークンに分解
		m_dwLex = m_lpCLexCBL->Lex(nSeq, nMaxLine, bHostMode, pSign);
		if(!m_lpCLexCBL->ResultLex()) throw "構文解析失敗です";

		// トークンからソースの解析
		m_dwParser = m_lpCParserCBL->Parser(m_lpCLexCBL->GetToken(), nCompPos, bCodeCheck);
		if(!m_lpCParserCBL->ResultParser()) throw "意味解析失敗です";

		// 文法チェック
		if(bCodeCheck){
			m_nCodeCheck = m_lpCParserCBL->GetWordCheckCount();
			if(m_nCodeCheck > 0){
				char szTraceMsg[256];
				wsprintf(szTraceMsg,
					"%2d 件の文法的誤りをｺｰﾄﾞから検出しました 確認して下さい",
					m_nCodeCheck);
				m_lpCParserCBL->AddTrace(szTraceMsg);
			}
		}
	}
	catch(const char *pszErrMsg){
		m_lpCParserCBL->AddTrace(pszErrMsg);
		bReturnValue = FALSE;
	}
	// 解析完了時刻
	GetLocalTime(&m_stAnalysys);

	m_bAnalysys = FALSE;	// 解析中フラグ

	return bReturnValue;
}

// ＣＯＢＯＬソース解析中止
void CAnaCBL::CancelAnalysys()
{
	if(m_lpCLexCBL){
		if(m_lpCLexCBL->IsAnalysys()){
			m_lpCLexCBL->Cancel();
			m_lpCLexCBL->AddTrace("構文解析中止...");
		}
	}
	if(m_lpCParserCBL){
		if(m_lpCParserCBL->IsAnalysys()){
			m_lpCParserCBL->Cancel();
			m_lpCLexCBL->AddTrace("意味解析中止...");
		}
	}
	if(m_lpCAnaForm){
		if(m_lpCAnaForm->IsAnalysys()){
			m_lpCAnaForm->Cancel();
			m_lpCLexCBL->AddTrace("フォーム解析中止...");
		}
	}
}

// ＣＯＢＯＬレコード情報を追加する
// (※危険 lpRecCblはGlobalAllocで確保しておかないといけない)
BOOL CAnaCBL::AddRecordData(LPRECORD_CBL lpRecCbl)
{
	if(!m_lpCParserCBL) return FALSE;
	if(GlobalSize(lpRecCbl) != (DWORD)sizeof(RECORD_CBL)) return FALSE;
	m_lpCParserCBL->PushRecord(lpRecCbl);
	return TRUE;
}

// 設定されたソースの数
int CAnaCBL::GetSourceCount()
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->GetSourceCount());
}

// 設定されたソース情報
int CAnaCBL::GetSourceData(const int nIndex, LPSOURCEINFO lpSrcInfo)
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->GetSourceInfo(nIndex, lpSrcInfo));
}

// 設定されたソースパス
int CAnaCBL::GetSourcePath(const int nIndex, char *pszSourcePath, const int nSize)
{
	if(!m_lpCLexCBL) return -1;
	SOURCEINFO si;
	if(m_lpCLexCBL->GetSourceInfo(nIndex, &si) < 0) return -1;
	if(si.lpParam == SRC_TYPE_CBL){
//		if((int)strlen(si.szSourcePath) < nSize) strcpy(pszSourcePath, si.szSourcePath);
		if((int)strlen(si.szSourcePath) < nSize) strcpy_s(pszSourcePath, nSize, si.szSourcePath);
		return ((int)strlen(si.szSourcePath));
	}else{
//		if((int)strlen(si.szParam) < nSize) strcpy(pszSourcePath, si.szParam);
		if((int)strlen(si.szParam) < nSize) strcpy_s(pszSourcePath, nSize, si.szParam);
		return ((int)strlen(si.szParam));
	}
}

// ソース解析（連結）順の入替
int CAnaCBL::SwapSourceNo(const int nNewNo, const int nOldNo)
{
	if(!m_lpCLexCBL) return -1;
	return (m_lpCLexCBL->SwapSourceNo(nNewNo, nOldNo));
}

// 合計サイズ取得
DWORD CAnaCBL::GetTotal()
{
	if(!m_lpCParserCBL) return 0;
	return m_lpCParserCBL->GetTotal();
}

// 解析結果のトークン数
int CAnaCBL::GetCount_Token()
{
	if(!m_lpCLexCBL) return -1;
	return m_lpCLexCBL->GetTokenCount();
}

// 解析結果のトークン取得
LPTOKEN_CBL CAnaCBL::GetFirst_Token()
{
	// インデックス初期化
	m_nTokenIndex = 0;
	return (GetNext_Token());
}

LPTOKEN_CBL CAnaCBL::GetNext_Token()
{
	if(!m_lpCLexCBL) return NULL;
	if(m_lpCLexCBL->GetTokenCount() > (int)m_nTokenIndex) return (m_lpCLexCBL->GetToken(m_nTokenIndex++));
	return NULL;
}

LPTOKEN_CBL CAnaCBL::Get_Token(const int nIndex)
{
	if(!m_lpCLexCBL) return NULL;
	if(m_lpCLexCBL->GetTokenCount() > nIndex) return (m_lpCLexCBL->GetToken(nIndex));
	return NULL;
}

std::vector<LPTOKEN_CBL> &CAnaCBL::Get_AllTokens()
{
	return (m_lpCLexCBL->GetToken());
}

// 解析結果の論理レコード数
int CAnaCBL::GetCount_Record()
{
	if(!m_lpCParserCBL) return -1;
	return m_lpCParserCBL->GetRecordCount();
}

// 解析結果のレコード取得
LPRECORD_CBL CAnaCBL::GetFirst_Record()
{
	// インデックス初期化
	m_nRecordIndex = 0;
	return (GetNext_Record());
}

LPRECORD_CBL CAnaCBL::GetNext_Record()
{
	if(!m_lpCParserCBL) return NULL;
	if(m_lpCParserCBL->GetRecordCount() > (int)m_nRecordIndex) return (m_lpCParserCBL->GetRecord(m_nRecordIndex++));
	return NULL;
}

LPRECORD_CBL CAnaCBL::Get_Record(const int nIndex)
{
	if(!m_lpCParserCBL) return NULL;
	if(m_lpCParserCBL->GetRecordCount() > nIndex) return (m_lpCParserCBL->GetRecord(nIndex));
	return NULL;
}

std::vector<LPRECORD_CBL> &CAnaCBL::Get_AllRecords()
{
	return (m_lpCParserCBL->GetRecord());
}

// ＦＯＲＭ解析結果の論理レコード数
int CAnaCBL::GetCount_FormRecord()
{
	if(!m_lpCAnaForm) return -1;
	return m_lpCAnaForm->GetRecordCount();
}

// ＦＯＲＭ解析結果のレコード取得
LPRECORD_FORM CAnaCBL::GetFirst_FormRecord()
{
	// インデックス初期化
	m_nFormRecordIndex = 0;
	return (GetNext_FormRecord());
}

LPRECORD_FORM CAnaCBL::GetNext_FormRecord()
{
	if(!m_lpCAnaForm) return NULL;
	if(m_lpCAnaForm->GetRecordCount() > (int)m_nFormRecordIndex) return (m_lpCAnaForm->GetRecord(m_nFormRecordIndex++));
	return NULL;
}

LPRECORD_FORM CAnaCBL::Get_FormRecord(const int nIndex)
{
	if(!m_lpCAnaForm) return NULL;
	if(m_lpCAnaForm->GetRecordCount() > nIndex) return (m_lpCAnaForm->GetRecord(nIndex));
	return NULL;
}

unsigned int CAnaCBL::GetMaxLevelNo()
{
	if(!m_lpCParserCBL) return NULL;
	return (m_lpCParserCBL->GetMaxLevel());
}

unsigned int CAnaCBL::GetMinLevelNo()
{
	if(!m_lpCParserCBL) return NULL;
	return (m_lpCParserCBL->GetMinLevel());
}

void CAnaCBL::SetWindowHandle(HWND hWndMain)
{
	if(m_lpCLexCBL) m_lpCLexCBL->SetWindowHandle(hWndMain);
	if(m_lpCParserCBL) m_lpCParserCBL->SetWindowHandle(hWndMain);
	if(m_lpCAnaForm) m_lpCAnaForm->SetWindowHandle(hWndMain);
}
