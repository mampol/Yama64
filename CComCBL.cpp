/*-------------------------------------------------------------------------------------

	CComCBL.cpp
	※ これ改造する

-------------------------------------------------------------------------------------*/
#include "CAnaCBL.h"
// インプリメンテーション(Yamamoto用)
#include "YamamotoCom.h"

// ｺﾝｽﾄﾗｸﾀ
CComCBL::CComCBL()
{
	m_hWndMain = NULL;
}

// ﾃﾞｽﾄﾗｸﾀ
CComCBL::~CComCBL()
{
}

// 共通初期処理
int CComCBL::InitAnalysys(int nStage, int nParam/*= 0*/)
{
	return (CBaseCBL::InitAnalysys(nStage, nParam));
}

// 共通終了処理
int CComCBL::EndAnalysys(int nParam/*= 0*/)
{
	return (CBaseCBL::EndAnalysys());
}

// ウィンドウハンドルセット
void CComCBL::SetWindowHandle(HWND hWndMain)
{
	m_hWndMain = hWndMain;
}

// トレース出力
int CComCBL::AddTrace(const char *pszTrace, const int nFlag/* = TRACE_INFO*/, const int nLine/* = -1*/)
{
	// インプリメンテーション(Yamamoto用)
//	return (CBaseCBL::AddTrace(pszTrace, nFlag, nLine));
	if(!m_hWndMain) return -1;
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(m_hWndMain, CSPLITWND_CLASS);
	if(!lpCWndSplit) return -1;
	lpCWndSplit->SendMessageSplit(UM_ADDOUTPUT, (WPARAM)nFlag, (LPARAM)pszTrace);
	return 0;
}
