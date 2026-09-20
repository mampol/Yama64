/*----------------------------------------------------------------------------------------

	OutputWnd.h

----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
	定義
----------------------------------------------------------------------------------------*/
#define ID_OUTPUTWND			2001
#define ID_OUTPUTTAB			2002
#define ID_OUTPUTLIST			2003
#define ID_SOURCELIST			2004
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//#define ID_ANALYSYSLIST			2005
#define ID_SEARCHLIST			2006
#define ID_DATAPANEL            2007

#ifdef _DEBUG
#define MAX_OUTPUTLISTITEM		100
#else
#define MAX_OUTPUTLISTITEM		1024*10
#endif
#define OUTPUT_CAPTION			20

// タブ位置
enum {
	TAB_OUTPUT,
	TAB_SOURCE,
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	TAB_ANALYSYS,
	TAB_SEARCH,
	TAB_DATA
};

// メニュー位置
enum {
	POPUP_SOURCE,
	POPUP_SOURCE_NOSEL,
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	POPUP_ANALYSYS,
	POPUP_SEARCH,
	POPUP_DATA
};

// サブメニュー位置

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK OutputSplitWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void SetFontOutputWnd(HWND hOutputWnd);
BOOL AddOutputMessageFromFile(HWND hWndMain, LPCTSTR lpcOutputFile, const int nFlag = TRACE_INFO);
BOOL AddOutputMessage(HWND hWndMain, LPCTSTR lpcOutputMsg, const int nFlag = TRACE_INFO);
BOOL UpdateSourceMessage(HWND hWndMain, const BOOL bSelTab);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//BOOL UpdateAnalysysMessage(HWND hWndMain, const BOOL bSelTab, const int nItem = -1);
BOOL AddSearchListMessage(HWND hWndMain, const BOOL bSelTab, const int nTreeIndex = -1);
int SourcesListCount(HWND hOutputWnd);
int SourcesListGetSel(HWND hOutputWnd, const int nFlg = 0);
int SourcesListGetSelParam(HWND hOutputWnd, const int nFlg = 0);
int SourcesListGetParam(HWND hOutputWnd, const int nItem, const int nFlg = 0);
int SourcesListSetSel(HWND hOutputWnd, const int nNo, const int nFlg = 0);
void SourcesListScroll(HWND hSourceList);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//int AnalysysListCount(HWND hOutputWnd);
//int AnalysysListGetSel(HWND hOutputWnd, const int nFlg = 0);
//int AnalysysListGetParam(HWND hOutputWnd, const int nFlg = 0);
int SearchListCount(HWND hOutputWnd);
int SearchListGetSel(HWND hOutputWnd, const int nFlg = 0);
int SearchListGetParam(HWND hOutputWnd, const int nFlg = 0);
void OutputTabSetCursel(const UINT uiSelTab);
void OutputTabSetCursel(HWND hOutputTab, const UINT uiSelTab);
int ExpandAnaLstEx(int nTreeInfo);
BOOL ShowDataTab(HWND hMainWnd,BOOL bShow);
LRESULT SetDataRecord(HWND hWnd, ULONGLONG ullRecordNo);
LRESULT MoveDataRecord(HWND hWnd, int nMove);
void UpdateDataPanel(HWND hOutputPanel);
