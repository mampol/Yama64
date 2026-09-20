/*-------------------------------------------------------------------------------------

	CBaseCBL.cpp

-------------------------------------------------------------------------------------*/
#include "CAnaCBL.h"

// ｺﾝｽﾄﾗｸﾀ
CBaseCBL::CBaseCBL()
 : m_uMMResult(1),
   m_bCancel(FALSE),
   m_nStage(-1)
{
#ifdef _DEBUG_MODE_
	printf("CBaseCBL コンストラクタ\n");
#endif	//_DEBUG_MODE_
}

// ﾃﾞｽﾄﾗｸﾀ
CBaseCBL::~CBaseCBL()
{
#ifdef _DEBUG_MODE_
	printf("CBaseCBL デストラクタ\n");
#endif	//_DEBUG_MODE_
	if(m_uMMResult == 0) timeEndPeriod(1);
}

// 共通初期処理
int CBaseCBL::InitAnalysys(int nStage, int nParam/*= 0*/)
{
	// タイマー精度を上げる
	m_uMMResult = timeBeginPeriod(1);
	// キャンセルフラグ
	m_bCancel = FALSE;
	// ステージ
	m_nStage = nStage;
	return 0;
}

// 共通終了処理
int CBaseCBL::EndAnalysys(int nParam/*= 0*/)
{
	// タイマー精度を元に戻す
	if(m_uMMResult == 0){
		timeEndPeriod(1);
		m_uMMResult = 1;
	}
	if(IsCancel()){
		char szTrace[64];
		wsprintf(szTrace, "解析中にエラーが発生しました STAGE%d", m_nStage);
		AddTrace(szTrace, TRACE_ANA);
	}
	// ステージ
	m_nStage = -1;
	return 0;
}

// キャンセル
void CBaseCBL::Cancel()
{
	// キャンセルフラグ
	m_bCancel = TRUE;
	// 終了処理の呼び出し
	EndAnalysys();
};

// トレース出力
int CBaseCBL::AddTrace(const char *pszTrace, const int nFlag/* = TRACE_INFO*/, const int nLine/* = -1*/)
{
	fprintf(stderr, "%s\n", pszTrace);
	return 0;
}
