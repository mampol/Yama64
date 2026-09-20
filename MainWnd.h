/*----------------------------------------------------------------------------------------

	MainWnd.h

----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
	定義
----------------------------------------------------------------------------------------*/
#define ID_REBAR			1001
#define ID_TOOLBAR_ANA		1002
#define ID_TOOLBAR_DAT		1003
#define ID_STATUSBAR		1004
#define ID_PROGBAR			1005

// メニュー位置
enum {
	MENU_POS_FILE,
	MENU_POS_EDIT,
	MENU_POS_VIEW,
	MENU_POS_ANA,
	MENU_POS_ABOUT
};

enum {
//	MENU_FILE_POS_HIS = 11
//	MENU_FILE_POS_HIS = 8
	MENU_FILE_POS_OPENDAT = 4,
	MENU_FILE_POS_SAVE = 5,
	MENU_FILE_POS_HIS = 7
};

// リストポップアップメニュー位置
enum { POPUP_EXPAND, POPUP_COLLAPSE, POPUP_NORMALITEM, POPUP_DUMMY };

// サブメニュー位置
enum { SUB_MENU_OPENSERC, SUB_MENU_ANACBL };

// コントロールの状態
enum { STATE_NEW, STATE_ANALYSING, STATE_ANALYSYS, STATE_DATOPEN, STATE_SEARCHING, STATE_SEARCH };

// ノード展開時のプログレス表示ノード数
#define SHOW_PROG_NODE		100

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
LRESULT OnOutputView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnAnaListView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
HMENU GetMenuHandle(HWND hWnd, DWORD dwMenuPos);
HMENU GetSubMenuHandle(HWND hWnd);
UINT GetTabMenuId(const UINT uiTab);
