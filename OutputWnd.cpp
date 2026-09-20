/*----------------------------------------------------------------------------------------

	OutputWnd.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "MainWnd.h"
#include "OutputWnd.h"
#include "resource.h"

#define DATAPANEL_PROP "YAMA_DATAPANEL"

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HWND ghMainWnd = NULL;
static WNDPROC gOrgTabWndProc = NULL;
static WNDPROC gOrgEditbocProc = NULL;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
HWND CreateChildCtrlWnd(HWND hParent, LPCTSTR lpcWndClass, LPCTSTR lpcWndTitle, UINT uiId, DWORD dwStylePlus, DWORD dwExStyle = 0);
BOOL CreateOutputSubWnd(HWND hParent, HWND hOutoutTab);
void SetChildCtrlWndFont(HWND hChildCtrl);
void InitSourceList(HWND hWnd, HWND hSourceList);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//void InitAnalysysList(HWND hWnd, HWND hAnalysysList);
void InitSearchList(HWND hWnd, HWND hSearchList);
HWND GetOutputSubWnd(HWND hOutputTab, const int nTabPos = -1);
char *GetOutputSubWndText(HWND hOutputTab, const int nTabPos = -1);
void ShowOutputSubWnd(HWND hOutputTab, const int nTabPos);
void SeFopcustOutputSubWnd(HWND hOutputTab, const int nTabPos);
LRESULT OnCreateOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnSizeOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnPaintOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnNotifyOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDestroyOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnAddOutput(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnUpdateSource(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//LRESULT OnUpdateAnalysys(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnAddSearch(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnNotifySourceList(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//LRESULT OnNotifyAnalysysList(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnNotifySearchList(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp);
void TrackOutputContextMenu(HWND hMainWnd, HWND hOutputTab);
void TrackOutputContextMenu(HWND hMainWnd, HWND hOutputTab, LPPOINT lppt);
LRESULT CALLBACK ChainOutputTabProc(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnMeasuerOutputChain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDrawOutputChain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnNotifyOutputChain(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnContextMenuOutputChain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK DataPanelDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnPaintDatePanel(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
void GetCurrentRecordRect(HWND hDlg, LPRECT lprc);
BOOL GetDataRecordName(HWND hMainWnd, char* pszName, int nSize);
LRESULT OnSizeDatePanel(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK ChainCurRecEditProc(HWND hEdit, UINT msg, WPARAM wp, LPARAM lp);
BOOL DrawDataMoveButton(LPDRAWITEMSTRUCT lpdis);

/*----------------------------------------------------------------------------------------
	アウトプットウィンドウプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK OutputSplitWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_CREATE:
		ghMainWnd = (HWND)(((LPCREATESTRUCT)lp)->lpCreateParams);
		return (OnCreateOutputWnd(ghMainWnd, hWnd, msg, wp, lp));
	case WM_SIZE:
		return (OnSizeOutputWnd(ghMainWnd, hWnd, msg, wp, lp));
	case WM_PAINT:
		return (OnPaintOutputWnd(ghMainWnd, hWnd, msg, wp, lp));
	case WM_NOTIFY:
		return (OnNotifyOutputWnd(ghMainWnd, hWnd, msg, wp, lp));
	case WM_DESTROY:
		return (OnDestroyOutputWnd(ghMainWnd, hWnd, msg, wp, lp));
	case UM_ADDOUTPUT:
		return (OnAddOutput(ghMainWnd, hWnd, msg, wp, lp));
	case UM_UPDSOURCE:
		return (OnUpdateSource(ghMainWnd, hWnd, msg, wp, lp));
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case UM_UPDANALYSYS:
//		return (OnUpdateAnalysys(ghMainWnd, hWnd, msg, wp, lp));
	case UM_ADDSEARCH:
		return (OnAddSearch(ghMainWnd, hWnd, msg, wp, lp));
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ウィンドウ作成完了通知
----------------------------------------------------------------------------------------*/
LRESULT OnCreateOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hOutputTab = CreateChildCtrlWnd(hWnd,
		WC_TABCONTROL,
		"Yamamoto Output TabControl",
		ID_OUTPUTTAB,
		WS_VISIBLE | WS_CLIPSIBLINGS | TCS_HOTTRACK | TCS_FOCUSNEVER | TCS_TABS,
		0);
	if(!hOutputTab) return -1;
	if(!CreateOutputSubWnd(hWnd, hOutputTab)) return -1;
	// タブをサブクラス化
//	gOrgTabWndProc = (WNDPROC)GetWindowLong(hOutputTab, GWL_WNDPROC);
//	SetWindowLong(hOutputTab, GWL_WNDPROC, (LONG)ChainOutputTabProc);
	gOrgTabWndProc = (WNDPROC)GetWindowLongPtr(hOutputTab, GWLP_WNDPROC);
	SetWindowLongPtr(hOutputTab, GWLP_WNDPROC, (LONG_PTR)ChainOutputTabProc);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	子供ウィンドウの作成
----------------------------------------------------------------------------------------*/
HWND CreateChildCtrlWnd(HWND hParent,
						LPCTSTR lpcWndClass,
						LPCTSTR lpcWndTitle,
						UINT uiId,
						DWORD dwStylePlus,
						DWORD dwExStyle/* = 0*/)
{
	HWND hChildCtrl = CreateWindowEx(dwExStyle,
				lpcWndClass,
				lpcWndTitle,
				WS_CHILD | dwStylePlus,
				0, 0,
				0, 0,
				hParent,
				(HMENU)uiId,
//				(HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
				(HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE),
				NULL);
	if(!hChildCtrl) return NULL;
//	SendMessage(hChildCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
	SetChildCtrlWndFont(hChildCtrl);
	return hChildCtrl;
}

/*----------------------------------------------------------------------------------------
	フォントハンドルの更新
----------------------------------------------------------------------------------------*/
void SetFontOutputWnd(
	HWND hOutputWnd)
{
	HWND hOutputTab =
		GetDlgItem(
			hOutputWnd,
			ID_OUTPUTTAB);

	if (!hOutputTab) {
		return;
	}

	SetChildCtrlWndFont(
		hOutputTab);

	int nCount =
		TabCtrl_GetItemCount(
			hOutputTab);

	for (int i = 0;
		i < nCount;
		i++)
	{
		HWND hChild =
			GetOutputSubWnd(
				hOutputTab,
				i);

		if (hChild) {
			SetChildCtrlWndFont(
				hChild);
		}
	}
}

/*----------------------------------------------------------------------------------------
	子供ウィンドウのフォント差し替え
----------------------------------------------------------------------------------------*/
void SetChildCtrlWndFont(HWND hChildCtrl)
{
	CYamaSetup* lpCSetup = (CYamaSetup*)GetProp(ghMainWnd, CSETUP_CLASS);
	if (!lpCSetup) return;
	HDC hDC = GetDC(hChildCtrl);
	HFONT hFont = MAKEFONT(hDC, 10, FW_NORMAL, FALSE, lpCSetup->m_UIProp.m_szFontFace);
	HFONT hOldFont = (HFONT)SendMessage(hChildCtrl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	if (hOldFont) DeleteObject(hOldFont);
	ReleaseDC(hChildCtrl, hDC);
}

/*----------------------------------------------------------------------------------------
	各サブウィンドウの作成
----------------------------------------------------------------------------------------*/
BOOL CreateOutputSubWnd(HWND hParent, HWND hOutputTab)
{
	// 出力
	HWND hOutputList = CreateChildCtrlWnd(hOutputTab,
				"LISTBOX",
				"Yamamoto Output ListBox",
				ID_OUTPUTLIST,
				WS_VSCROLL | LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
				WS_EX_CLIENTEDGE);
	if(!hOutputList) return FALSE;
	// ソース一覧
	HWND hSourceList = CreateChildCtrlWnd(hOutputTab,
				WC_LISTVIEW,
				"Yamamoto Source ListView",
				ID_SOURCELIST,
				LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL,
				WS_EX_CLIENTEDGE);
	if(!hSourceList) return FALSE;
	InitSourceList(hParent, hSourceList);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	// 解析結果
//	HWND hAnalysysList = CreateChildCtrlWnd(hOutputTab,
//				WC_LISTVIEW,
//				"Yamamoto Analysys ListView",
//				ID_ANALYSYSLIST,
//				LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | LVS_SINGLESEL,
//				WS_EX_CLIENTEDGE);
//	if(!hAnalysysList) return FALSE;
//	InitAnalysysList(hParent, hAnalysysList);
	// 検索結果
	HWND hSearchList = CreateChildCtrlWnd(hOutputTab,
				WC_LISTVIEW,
				"Yamamoto Search ListView",
				ID_SEARCHLIST,
				LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL,
				WS_EX_CLIENTEDGE);
	if(!hSearchList) return FALSE;
	InitSearchList(hParent, hSearchList);

	// タブに追加
	char *pszTabText[] = {
		"トレース",
		"ソース一覧",
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//		"解析結果詳細",
		"検索結果"
	};
	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = pszTabText[0];
	tci.lParam = (LPARAM)hOutputList;
	TabCtrl_InsertItem(hOutputTab, TabCtrl_GetItemCount(hOutputTab), &tci);
	tci.pszText = pszTabText[1];
	tci.lParam = (LPARAM)hSourceList;
	TabCtrl_InsertItem(hOutputTab, TabCtrl_GetItemCount(hOutputTab), &tci);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	tci.pszText = pszTabText[2];
//	tci.lParam = (LPARAM)hAnalysysList;
//	TabCtrl_InsertItem(hOutputTab, TabCtrl_GetItemCount(hOutputTab), &tci);
//	tci.pszText = pszTabText[3];
	tci.pszText = pszTabText[2];
	tci.lParam = (LPARAM)hSearchList;
	TabCtrl_InsertItem(hOutputTab, TabCtrl_GetItemCount(hOutputTab), &tci);

	// hOutputListのみ表示
	ShowWindow(hOutputList, SW_SHOW);
	UpdateWindow(hOutputList);

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ソース一覧の初期化
----------------------------------------------------------------------------------------*/
void InitSourceList(HWND hWnd, HWND hSourceList)
{
	// 拡張スタイル
	ListView_SetExtendedListViewStyle(hSourceList,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// イメージリスト
	HIMAGELIST hImgSource = CreateImageList(hWnd, IDB_BITMAP3, 3, RGB(255, 255, 255));
	ListView_SetImageList(hSourceList, hImgSource, LVSIL_SMALL);
	// リストヘッダ作成
	int pnCol[] = {
		60, 550, 110, 150
	};
	WORD pwCol[] = {
		LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_RIGHT
	};
	char *pszCol[] = {
		"No.", "ソースファイル", "ファイルサイズ", "最終更新日時"
	};
	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int i = 0; i < sizeof(pnCol) / sizeof(int); i++){
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.iSubItem = i;
		lvcol.fmt = pwCol[i];
		lvcol.cx = pnCol[i];
		lvcol.pszText = pszCol[i];
		ListView_InsertColumn(hSourceList, i, &lvcol);
	}
}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果一覧の初期化
//----------------------------------------------------------------------------------------*/
//void InitAnalysysList(HWND hWnd, HWND hAnalysysList)
//{
//	// 拡張スタイル
//	ListView_SetExtendedListViewStyle(hAnalysysList,
//		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
//	// イメージリスト
//	HIMAGELIST hImgAnalysys = CreateImageList(hWnd, IDB_BITMAP4, 1, RGB(255, 255, 255));
//	ListView_SetImageList(hAnalysysList, hImgAnalysys, LVSIL_SMALL);
//	// リストヘッダ作成
//	int pnCol[] = {
//		150, 220
//	};
//	WORD pwCol[] = {
//		LVCFMT_LEFT, LVCFMT_RIGHT
//	};
//	char *pszCol[] = {
//		"項目名", "内容"
//	};
//	LVCOLUMN lvcol;
//	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
//	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
//	for(int i = 0; i < sizeof(pnCol) / sizeof(int); i++){
//		lvcol.fmt = LVCFMT_LEFT;
//		lvcol.iSubItem = i;
//		lvcol.fmt = pwCol[i];
//		lvcol.cx = pnCol[i];
//		lvcol.pszText = pszCol[i];
//		ListView_InsertColumn(hAnalysysList, i, &lvcol);
//	}
//}

/*----------------------------------------------------------------------------------------
	検索結果一覧の初期化
----------------------------------------------------------------------------------------*/
void InitSearchList(HWND hWnd, HWND hSearchList)
{
	// 拡張スタイル
	ListView_SetExtendedListViewStyle(hSearchList,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	// イメージリスト
	HIMAGELIST hImgSearch = CreateImageList(hWnd, IDB_BITMAP5, 1, RGB(255, 255, 255));
	ListView_SetImageList(hSearchList, hImgSearch, LVSIL_SMALL);
	// リストヘッダ作成
	int pnCol[] = {
		120, 100, 90, 300
	};
	WORD pwCol[] = {
		LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT
	};
	char *pszCol[] = {
		"物理行位置", "論理行位置", "レベル", "項目名"
	};
	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int i = 0; i < sizeof(pnCol) / sizeof(int); i++){
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.iSubItem = i;
		lvcol.fmt = pwCol[i];
		lvcol.cx = pnCol[i];
		lvcol.pszText = pszCol[i];
		ListView_InsertColumn(hSearchList, i, &lvcol);
	}
}

/*----------------------------------------------------------------------------------------
	各サブウィンドウのハンドル
----------------------------------------------------------------------------------------*/
HWND GetOutputSubWnd(HWND hOutputTab, const int nTabPos/* = -1*/)
{
	if(nTabPos >= TabCtrl_GetItemCount(hOutputTab)) return NULL;
	int nSel;
	if(nTabPos < 0){
		nSel = TabCtrl_GetCurSel(hOutputTab);
	}else{
		nSel = nTabPos;
	}

	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_PARAM;
	if(!TabCtrl_GetItem(hOutputTab, nSel, &tci)) return NULL;
	return ((HWND)tci.lParam);
}

/*----------------------------------------------------------------------------------------
	各サブウィンドウの文字列
----------------------------------------------------------------------------------------*/
char *GetOutputSubWndText(HWND hOutputTab, const int nTabPos/* = -1*/)
{
	static char szTabText[128];
	if(nTabPos > TabCtrl_GetItemCount(hOutputTab)) return NULL;
	int nSel;
	if(nTabPos < 0){
		nSel = TabCtrl_GetCurSel(hOutputTab);
	}else{
		nSel = nTabPos;
	}

	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_TEXT;
	tci.pszText = szTabText;
	tci.cchTextMax = sizeof(szTabText);
	if(!TabCtrl_GetItem(hOutputTab, nSel, &tci)) return NULL;
	return szTabText;
}

/*----------------------------------------------------------------------------------------
	各サブウィンドウの表示切り替え
----------------------------------------------------------------------------------------*/
void ShowOutputSubWnd(HWND hOutputTab, const int nTabPos)
{
	int nCount = TabCtrl_GetItemCount(hOutputTab);
//	if(nTabPos > nCount) return;
	if (nTabPos < 0 || nTabPos >= nCount)
	{
		return;
	}

	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_PARAM;
	for(int i = 0; i < nCount; i++){
		if(!TabCtrl_GetItem(hOutputTab, i, &tci)) continue;
		if(i == nTabPos){
			ShowWindow((HWND)tci.lParam, SW_SHOW);
			SetFocus((HWND)tci.lParam);
		}else{
			ShowWindow((HWND)tci.lParam, SW_HIDE);
		}
	}

	// メニューの変更
	if(ghMainWnd){
		HMENU hViewMenu = GetMenuHandle(ghMainWnd, MENU_POS_VIEW);
		if (hViewMenu)
		if (nCount > 3) {
			CheckMenuRadioItem(hViewMenu,
				IDM_TRACE_VIEW, IDM_DATA_VIEW,
				(UINT)GetTabMenuId(nTabPos), MF_BYCOMMAND);
		}
		else {
			EnableMenuItem(hViewMenu, IDM_DATA_VIEW, FALSE);
		}
	}

	// タイトル再描画
	HWND hParent = GetParent(hOutputTab);
	RECT rc;
	GetClientRect(hParent, &rc);
	rc.bottom = OUTPUT_CAPTION;
	InvalidateRect(hParent, &rc, TRUE);
}

/*----------------------------------------------------------------------------------------
	各サブウィンドウのフォーカス切り替え
----------------------------------------------------------------------------------------*/
void SeFopcustOutputSubWnd(HWND hOutputTab, const int nTabPos)
{
	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_PARAM;
	TabCtrl_GetItem(hOutputTab, nTabPos, &tci);
	if(tci.lParam) SetFocus((HWND)tci.lParam);
}

/*----------------------------------------------------------------------------------------
	サイズ変更通知
----------------------------------------------------------------------------------------*/
LRESULT OnSizeOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hOutputTab = GetDlgItem(hWnd, ID_OUTPUTTAB);
	if(!hOutputTab) return (DefWindowProc(hWnd, msg, wp, lp));

	RECT rc;
	// 親の大きさ
	int nHeight = OUTPUT_CAPTION;
	GetClientRect(hWnd, &rc);
	MoveWindow(hOutputTab,
		rc.left, rc.top + nHeight,
		rc.right, rc.bottom - nHeight,
		TRUE);

	// 子供の大きさ
	GetClientRect(hOutputTab, &rc);
	TabCtrl_AdjustRect(hOutputTab, FALSE, &rc);
	int nCount = TabCtrl_GetItemCount(hOutputTab);
	for(int i = 0; i < nCount; i++){
		MoveWindow(GetOutputSubWnd(hOutputTab, i),
			rc.left,
			rc.top + 2,
			rc.right - rc.left, 
			(rc.bottom - rc.top) - 2,
			TRUE);
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	描画イベント処理
----------------------------------------------------------------------------------------*/
LRESULT OnPaintOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);
	RECT rc;
	GetClientRect(hWnd, &rc);
	rc.bottom = OUTPUT_CAPTION;
	// 背景を塗りつぶす
	FillRect(hDC, &rc, GetSysColorBrush(COLOR_3DFACE));
	// タイトルバーの色で描画
	HPEN hPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_ACTIVECAPTION));
	HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	SetRect(&rc,
		rc.left + 2, rc.top, rc.right - 2, rc.bottom - 3);
	RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 3, 3);
	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	// タイトル描画
	CYamaSetup* lpCSetup = (CYamaSetup*)GetProp(ghMainWnd, CSETUP_CLASS);
	HFONT hFont = CreateFont((rc.bottom - rc.top) - 2,
		0, 0, 0,
		FW_BOLD,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		(lpCSetup ? lpCSetup->m_UIProp.m_szFontFace : "Yu Gothic UI"));
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	SetTextColor(hDC, GetSysColor(COLOR_CAPTIONTEXT));
	SetBkColor(hDC, GetSysColor(COLOR_ACTIVECAPTION));
	rc.left += 8;	// 微調整
	rc.top += 1;	// 同上
	DrawText(hDC,
		//GetOutputSubWndText(GetDlgItem(hWnd, ID_OUTPUTTAB)),
		"アウトプット",
		-1,
		&rc,
		DT_SINGLELINE | DT_VCENTER);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	EndPaint(hWnd, &ps);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	コントロール通知メッセージ
----------------------------------------------------------------------------------------*/
LRESULT OnNotifyOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMHDR lpnmhdr = (LPNMHDR)lp;
	if(lpnmhdr->idFrom != ID_OUTPUTTAB) return (DefWindowProc(hWnd, msg ,wp, lp));

	switch(lpnmhdr->code){
	case TCN_SELCHANGE:
		// 表示ウィンドウの変更
		ShowOutputSubWnd(lpnmhdr->hwndFrom, TabCtrl_GetCurSel(lpnmhdr->hwndFrom));
		break;
	default:
		return (DefWindowProc(hWnd, msg ,wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	アウトプットメッセージ追加イベント
----------------------------------------------------------------------------------------*/
LRESULT OnAddOutput(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(lp == NULL) return (0L);
	int nFlg = (int)wp;

	HWND hOutputTab = GetDlgItem(hWnd, ID_OUTPUTTAB);
	if(!hOutputTab) return (0L);

	HWND hOutputList = GetOutputSubWnd(hOutputTab, TAB_OUTPUT);
	if(!hOutputList) return (0L);

	int nCount = (int)SendMessage(hOutputList, LB_GETCOUNT, 0L, 0L);
	if(nCount > MAX_OUTPUTLISTITEM) SendMessage(hOutputList, LB_DELETESTRING, (WPARAM)0, 0L);
	int nSel = SendMessage(hOutputList, LB_ADDSTRING, 0L, (LPARAM)lp);
	SendMessage(hOutputList, LB_SETCURSEL, (WPARAM)nSel, 0L);

	// 表示の切り替え
/*
	if(nFlg != TRACE_INFO){
		if(TabCtrl_GetCurSel(hOutputTab) != TAB_OUTPUT){
			TabCtrl_SetCurSel(hOutputTab, TAB_OUTPUT);
			ShowOutputSubWnd(hOutputTab, TAB_OUTPUT);
		}
	}
*/
	if(nFlg != TRACE_INFO) OutputTabSetCursel(hOutputTab, TAB_OUTPUT);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソース一覧更新イベント
----------------------------------------------------------------------------------------*/
LRESULT OnUpdateSource(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hMainWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return (0L);

	HWND hOutputTab = GetDlgItem(hWnd, ID_OUTPUTTAB);
	if(!hOutputTab) return (0L);

	HWND hSourceList = GetOutputSubWnd(hOutputTab, TAB_SOURCE);
	if(!hSourceList) return (0L);

	// 一度全削除
	ListView_DeleteAllItems(hSourceList);

	// 更新
	int nCount = lpCAnaCBL->GetSourceCount();
	for(int i = 0; i < nCount; i++){
		SOURCEINFO si;
		if(lpCAnaCBL->GetSourceData(i, &si) < 0) break;

		WIN32_FIND_DATA w32fd;
		ZeroMemory(&w32fd, sizeof(WIN32_FIND_DATA));
		if((int)si.lpParam == SRC_TYPE_EDIT){
			// 編集データの場合は、使用するメンバのみ設定して続行
			w32fd.nFileSizeLow = si.dwSourceSize;
		}else{
			HANDLE hFind = FindFirstFile(si.szSourcePath, &w32fd);
			if(hFind == INVALID_HANDLE_VALUE){
				// 読込みエラー!?
				ZeroMemory(&w32fd, sizeof(WIN32_FIND_DATA));
			}else{
				FindClose(hFind);
			}
		}

		// リストに追加
		int nCount = ListView_GetItemCount(hSourceList);
		char szVal[64];
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(LVITEM));
		wsprintf(szVal, "%02d", (int)(si.nNo + 1));
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iItem = nCount;
		lvi.iSubItem = 0;
		lvi.pszText = szVal;
		lvi.iImage = (int)si.lpParam;
		lvi.lParam = (LPARAM)si.nNo;
		ListView_InsertItem(hSourceList, &lvi);
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nCount;
		lvi.iSubItem= 1;
		lvi.pszText = (LPSTR)(si.lpParam == SRC_TYPE_CBL ? si.szSourcePath : si.szParam);
		ListView_SetItem(hSourceList, &lvi);
		if(w32fd.nFileSizeLow > 0){
			wsprintf(szVal, "%d Byte", w32fd.nFileSizeLow);
		}else{
//			strcpy(szVal, "? Byte");
			strcpy_s(szVal, sizeof(szVal), "-");
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nCount;
		lvi.iSubItem= 2;
		lvi.pszText = szVal;
		ListView_SetItem(hSourceList, &lvi);
		FILETIME ft;
		SYSTEMTIME st;
		if(w32fd.ftLastWriteTime.dwHighDateTime == 0 && w32fd.ftLastWriteTime.dwLowDateTime == 0){
//			strcpy(szVal, "ﾀｲﾑｽﾀﾝﾌﾟ不明");
			strcpy_s(szVal, sizeof(szVal), "-");
		}else{
			FileTimeToLocalFileTime(&(w32fd.ftLastWriteTime), &ft);
			FileTimeToSystemTime(&ft, &st);
			if((int)si.lpParam == SRC_TYPE_EDIT){
//				strcpy(szVal, "(´ー｀)ﾁﾗﾈｰﾖ");
				strcpy_s(szVal, sizeof(szVal), "-");
			}else{
				wsprintf(szVal, "%04d/%02d/%02d %02d:%02d",
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
			}
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nCount;
		lvi.iSubItem= 3;
		lvi.pszText = szVal;
		ListView_SetItem(hSourceList, &lvi);
	}

	// 表示の切り替え
	if((int)wp > 0) OutputTabSetCursel(hOutputTab, TAB_SOURCE);

	return (0L);
}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果更新イベント
//----------------------------------------------------------------------------------------*/
//LRESULT OnUpdateAnalysys(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
//{
//	// 解析結果リスト制御クラスインスタンス
//	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hMainWnd, CANALST_CLASS);
//	if(!lpCAnaLst) return (0L);
//	HWND hOutputTab = GetDlgItem(hWnd, ID_OUTPUTTAB);
//	if(!hOutputTab) return (0L);
//	HWND hAnalysysList = GetOutputSubWnd(hOutputTab, TAB_ANALYSYS);
//	if(!hAnalysysList) return (0L);
//
////	lpCAnaLst->SetRedraw(FALSE);
//	SendMessage(hAnalysysList, WM_SETREDRAW, (WPARAM)FALSE, 0L);
//
//	if(ListView_GetItemCount(hAnalysysList) > 0) ListView_DeleteAllItems(hAnalysysList);
//
//	int nSelItem = (int)lp;
//	if(nSelItem >= 0){
//		COLORREF colFore = lpCAnaLst->GetColorFore(),
//				 colBack = lpCAnaLst->GetColorBack(),	// 未使用
//				 colStruct = lpCAnaLst->GetColorStruct(),
//				 colOccurs = lpCAnaLst->GetColorOccurs(),
//				 colRedefines = lpCAnaLst->GetColorRedefines(),
//				 colBinary = lpCAnaLst->GetColorBinary(),
//				 colSupplementation = lpCAnaLst->GetColorSupplementation();
//		RECORD_CBL RecCbl;
//		if(lpCAnaLst->GetItemData(nSelItem, &RecCbl) >= 0){
//
//			// 項目の追加
//			char szVal[64];
//			int nItem = 0;
//			LVITEM lvi;
//			ZeroMemory(&lvi, sizeof(LVITEM));
//
//			// レベル
//			wsprintf(szVal, "%02d", RecCbl.unLevel);
//			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//			lvi.iItem = nItem;
//			lvi.iSubItem = 0;
//			lvi.pszText = "レベル";
//			lvi.iImage = 0;
//			lvi.lParam = (LPARAM)colFore;
//			ListView_InsertItem(hAnalysysList, &lvi);
//			wsprintf(szVal, "%02d", RecCbl.unLevel);
//			lvi.mask = LVIF_TEXT;
//			lvi.iSubItem = 1;
//			lvi.pszText = szVal;
//			ListView_SetItem(hAnalysysList, &lvi);
//			nItem++;
//
//			// 項目名
//			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//			lvi.iItem = nItem;
//			lvi.iSubItem = 0;
//			lvi.pszText = "項目名";
//			lvi.iImage = 0;
//			lvi.lParam = (LPARAM)colFore;
//			ListView_InsertItem(hAnalysysList, &lvi);
//			lvi.mask = LVIF_TEXT;
//			lvi.iSubItem = 1;
//			lvi.pszText = RecCbl.szDescriptor;
//			ListView_SetItem(hAnalysysList, &lvi);
//			nItem++;
//
//			// 型定義
//			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//			lvi.iItem = nItem;
//			lvi.iSubItem = 0;
//			lvi.pszText = "型定義";
//			lvi.iImage = 0;
//			lvi.lParam = (LPARAM)(RecCbl.bStruct ? colStruct : colFore);
//			ListView_InsertItem(hAnalysysList, &lvi);
//			lvi.mask = LVIF_TEXT;
//			lvi.iSubItem = 1;
//			lvi.pszText = (RecCbl.bStruct ? "<集団項目>" : RecCbl.szTypeDef);
//			ListView_SetItem(hAnalysysList, &lvi);
//			nItem++;
//
//			// 大きさ
//			wsprintf(szVal, "%d Byte", RecCbl.dwTypeSize);
//			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//			lvi.iItem = nItem;
//			lvi.iSubItem = 0;
//			lvi.pszText = "大きさ";
//			lvi.iImage = 0;
//			lvi.lParam = (LPARAM)colFore;
//			ListView_InsertItem(hAnalysysList, &lvi);
//			lvi.mask = LVIF_TEXT;
//			lvi.iSubItem = 1;
//			lvi.pszText = szVal;
//			ListView_SetItem(hAnalysysList, &lvi);
//			nItem++;
//
//			// 位置
//			wsprintf(szVal, "%d Byte", RecCbl.dwTypePos);
//			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//			lvi.iItem = nItem;
//			lvi.iSubItem = 0;
//			lvi.pszText = "位置";
//			lvi.iImage = 0;
//			lvi.lParam = (LPARAM)colFore;
//			ListView_InsertItem(hAnalysysList, &lvi);
//			lvi.mask = LVIF_TEXT;
//			lvi.iSubItem = 1;
//			lvi.pszText = szVal;
//			ListView_SetItem(hAnalysysList, &lvi);
//			nItem++;
//
//			// 繰返し
//			if(RecCbl.unOccursGroupId > 0){
//				if(RecCbl.bOccurs){
//					wsprintf(szVal, "%d 回", RecCbl.nOccurs);
//					lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//					lvi.iItem = nItem;
//					lvi.iSubItem = 0;
//					lvi.pszText = "繰返し回数";
//					lvi.iImage = 0;
//					lvi.lParam = (LPARAM)colOccurs;
//					ListView_InsertItem(hAnalysysList, &lvi);
//					lvi.mask = LVIF_TEXT;
//					lvi.iSubItem = 1;
//					lvi.pszText = szVal;
//					ListView_SetItem(hAnalysysList, &lvi);
//					nItem++;
//				}
//
//				wsprintf(szVal, "%d 番目", (RecCbl.nOccursOrder + 1));
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "繰返し指数";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colOccurs;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = szVal;
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//
//				if(strlen(RecCbl.szOccursIndexed) > 0){
//					lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//					lvi.iItem = nItem;
//					lvi.iSubItem = 0;
//					lvi.pszText = "繰返し指数用変数定義";
//					lvi.iImage = 0;
//					lvi.lParam = (LPARAM)colOccurs;
//					ListView_InsertItem(hAnalysysList, &lvi);
//					lvi.mask = LVIF_TEXT;
//					lvi.iSubItem = 1;
//					lvi.pszText = RecCbl.szOccursIndexed;
//					ListView_SetItem(hAnalysysList, &lvi);
//					nItem++;
//				}
//			}
//
//			// 再定義
//			if(RecCbl.unRedefinesGroupId > 0){
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "再定義項目";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colRedefines;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = RecCbl.szRedefines;
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//			}
//
//			// 圧縮
//			if(RecCbl.btCompress){
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "圧縮形式";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colBinary;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = RecCbl.szCompress;
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//			}
//
//			// 初期値
//			if(strlen(RecCbl.szValue) > 0){
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "初期値";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colFore;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = RecCbl.szValue;
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//			}
//
//			// 追加形式
//			if(strlen(RecCbl.szValueEx) > 0){
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "追加形式";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colFore;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = RecCbl.szValueEx;
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//			}
//
//			// コメント
//			if(strlen(RecCbl.szComment) > 0){
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "コメント";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colFore;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = RecCbl.szComment;
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//			}
//
//			// 補完項目
//			if(RecCbl.nSupplementation > 0){
//				char *szSupplementation[] = {
//					"ピリオド補完",
//					"集団項目補完",
//					"FILLER補完",
//					"削除項目"
//				};
//				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
//				lvi.iItem = nItem;
//				lvi.iSubItem = 0;
//				lvi.pszText = "補完項目";
//				lvi.iImage = 0;
//				lvi.lParam = (LPARAM)colSupplementation;
//				ListView_InsertItem(hAnalysysList, &lvi);
//				lvi.mask = LVIF_TEXT;
//				lvi.iSubItem = 1;
//				lvi.pszText = szSupplementation[RecCbl.nSupplementation - 1];
//				ListView_SetItem(hAnalysysList, &lvi);
//				nItem++;
//			}
//		}
//	}
//
////	lpCAnaLst->SetRedraw(TRUE);
//	SendMessage(hAnalysysList, WM_SETREDRAW, (WPARAM)TRUE, 0L);
//
//	// 表示の切り替え
//	if((int)wp > 0) OutputTabSetCursel(hOutputTab, TAB_ANALYSYS);
//
//	return (0L);
//}

/*----------------------------------------------------------------------------------------
	検索結果追加イベント
----------------------------------------------------------------------------------------*/
LRESULT OnAddSearch(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hMainWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);

	HWND hOutputTab = GetDlgItem(hWnd, ID_OUTPUTTAB);
	if(!hOutputTab) return (0L);

	HWND hSearchList = GetOutputSubWnd(hOutputTab, TAB_SEARCH);
	if(!hSearchList) return (0L);

	SendMessage(hSearchList, WM_SETREDRAW, (WPARAM)FALSE, 0L);

	int nTreeInfo = (int)lp;
	if(nTreeInfo < 0){
		ListView_DeleteAllItems(hSearchList);
	}else{
		RECORD_CBL RecCbl;
		if(lpCAnaLst->GetItemDataEx(nTreeInfo, &RecCbl) >= 0){

			// 項目の追加
			char szVal[24];
			int nItem = ListView_GetItemCount(hSearchList);
			LVITEM lvi;
			ZeroMemory(&lvi, sizeof(LVITEM));

			// 物理行位置
			wsprintf(szVal, "%4d 行目", (RecCbl.nPhyLine + 1));
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.pszText = szVal;
			lvi.iImage = 0;
			lvi.lParam = (LPARAM)nTreeInfo;
			ListView_InsertItem(hSearchList, &lvi);

			// 論理行位置
			wsprintf(szVal, "%4d 行目", (RecCbl.nLogLine + 1));
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = szVal;
			ListView_SetItem(hSearchList, &lvi);

			// レベル
			wsprintf(szVal, "%02d", RecCbl.unLevel);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 2;
			lvi.pszText = szVal;
			ListView_SetItem(hSearchList, &lvi);

			// 項目名
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 3;
			lvi.pszText = RecCbl.szDescriptor;
			ListView_SetItem(hSearchList, &lvi);
		}
	}

	SendMessage(hSearchList, WM_SETREDRAW, (WPARAM)TRUE, 0L);

	// 表示の切り替え
	if((int)wp > 0) OutputTabSetCursel(hOutputTab, TAB_SEARCH);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	ウィンドウ削除
----------------------------------------------------------------------------------------*/
LRESULT OnDestroyOutputWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hOutputTab = GetDlgItem(hWnd, ID_OUTPUTTAB);
	if(hOutputTab){
		// タブをサブクラス化を解除
//		SetWindowLong(hOutputTab, GWL_WNDPROC, (LONG)gOrgTabWndProc);
		SetWindowLongPtr(hOutputTab, GWLP_WNDPROC, (LONG_PTR)gOrgTabWndProc);
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	タブコントロール・サブクラスプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK ChainOutputTabProc(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_MEASUREITEM:
		return (OnMeasuerOutputChain(hOutputTab, msg, wp, lp));
	case WM_DRAWITEM:
		return (OnDrawOutputChain(hOutputTab, msg, wp, lp));
	case WM_NOTIFY:
		return (OnNotifyOutputChain(hOutputTab, msg, wp, lp));
	case WM_CONTEXTMENU:
		return (OnContextMenuOutputChain(hOutputTab, msg, wp, lp));
	}
	return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
}

/*----------------------------------------------------------------------------------------
	リストサイズ決定
----------------------------------------------------------------------------------------*/
LRESULT OnMeasuerOutputChain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lp;
	if(lpmis->CtlID != ID_OUTPUTLIST) return (0L);

	HWND hOutputList = GetDlgItem(hWnd, ID_OUTPUTLIST);
	HFONT hFont, hOldFont;
	hFont = (HFONT)SendMessage(hOutputList, WM_GETFONT, 0L, 0L);

	RECT rc;
	GetClientRect(hOutputList, &rc);

	TEXTMETRIC tm;
	HDC hDC = GetDC(hOutputList);
	hOldFont = (HFONT)SelectObject(hDC, hFont);
	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	ReleaseDC(hOutputList, hDC);

	lpmis->itemWidth = rc.right;
	lpmis->itemHeight = max((tm.tmHeight + 6), 16);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	リストボックスオーナードロー
----------------------------------------------------------------------------------------*/
LRESULT OnDrawOutputChain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lp;
	if(lpdis->CtlID != ID_OUTPUTLIST) return (0L);

	// 描画準備
	HFONT hFont, hOldFont;
	hFont = (HFONT)SendMessage(lpdis->hwndItem, WM_GETFONT, 0L, 0L);
	hOldFont = (HFONT)SelectObject(lpdis->hDC, hFont);
	HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	COLORREF colOldText = SetTextColor(lpdis->hDC, GetSysColor(COLOR_WINDOWTEXT));
	COLORREF colOldBk = SetBkColor(lpdis->hDC, GetSysColor(COLOR_WINDOW));

	// 描画
	RECT rcString;
	SetRect(&rcString,
		lpdis->rcItem.left + 4,
		lpdis->rcItem.top,
		lpdis->rcItem.right,
		lpdis->rcItem.bottom);

	DWORD dwLen = (DWORD)SendMessage(lpdis->hwndItem, LB_GETTEXTLEN, (WPARAM)lpdis->itemID, 0L);
	LPSTR pszOutputMsg = (LPSTR)GlobalAlloc(GPTR, dwLen + 1);
	if(pszOutputMsg){
		if(SendMessage(lpdis->hwndItem, LB_GETTEXT, (WPARAM)lpdis->itemID, (LPARAM)pszOutputMsg) != LB_ERR){
			DRAWTEXTPARAMS dtp;
			ZeroMemory(&dtp, sizeof(DRAWTEXTPARAMS));
			dtp.cbSize = sizeof(DRAWTEXTPARAMS);
			dtp.iTabLength = 4;
			DrawTextEx(lpdis->hDC,
				pszOutputMsg, -1,
				&rcString,
				DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_EXPANDTABS | DT_TABSTOP,
				&dtp);
		}
		GlobalFree(pszOutputMsg);
	}

	// 描画後始末
	SetTextColor(lpdis->hDC, colOldText);
	SetBkColor(lpdis->hDC, colOldBk);
	SelectObject(lpdis->hDC, hOldFont);
	DeleteObject(hBrush);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	アウトプット拡張通知メッセージ処理
----------------------------------------------------------------------------------------*/
LRESULT OnNotifyOutputChain(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(((LPNMHDR)lp)->idFrom){
	case ID_SOURCELIST:
		return (OnNotifySourceList(hOutputTab, msg, wp, lp));
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case ID_ANALYSYSLIST:
//		return (OnNotifyAnalysysList(hOutputTab, msg, wp, lp));
	case ID_SEARCHLIST:
		return (OnNotifySearchList(hOutputTab, msg, wp, lp));
	default:
		return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソース一覧通知メッセージ処理
----------------------------------------------------------------------------------------*/
LRESULT OnNotifySourceList(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMLISTVIEW lpNmLv = (LPNMLISTVIEW)lp;
	switch(lpNmLv->hdr.code){
	case NM_DBLCLK:
		SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_SHOPENSRC, 0L);
		break;
	case LVN_GETINFOTIP:
		break;
	case LVN_KEYDOWN:
		switch(((LPNMLVKEYDOWN)lp)->wVKey){
		case VK_RETURN:
			SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_SHOPENSRC, 0L);
			break;
		case VK_DELETE:
			SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_CLOSESRC, 0L);
			break;
		case VK_UP:
			if(GetAsyncKeyState(VK_MENU) < 0){
				SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_SRC_UP, 0L);
			}else{
				return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
			}
			break;
		case VK_DOWN:
			if(GetAsyncKeyState(VK_MENU) < 0){
				SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_SRC_DOWN, 0L);
			}else{
				return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
			}
			break;
		case VK_APPS:
			TrackOutputContextMenu(ghMainWnd, hOutputTab);
			break;
		default:
			return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
		}
		break;
	default:
		return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
	}
	return (0L);
}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果一覧通知メッセージ処理
//----------------------------------------------------------------------------------------*/
//LRESULT OnNotifyAnalysysList(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp)
//{
//	LPNMLISTVIEW lpNmLv = (LPNMLISTVIEW)lp;
//	switch(lpNmLv->hdr.code){
//	case LVN_GETINFOTIP:
//		break;
//	case NM_CUSTOMDRAW:
//		switch(((LPNMLVCUSTOMDRAW)lp)->nmcd.dwDrawStage){
//		case CDDS_PREPAINT:
//			return CDRF_NOTIFYITEMDRAW;
//		case CDDS_ITEMPREPAINT:
//			((LPNMLVCUSTOMDRAW)lp)->clrText = (COLORREF)((LPNMLVCUSTOMDRAW)lp)->nmcd.lItemlParam;
//			return CDRF_NEWFONT;
//		default:
//			return (0L);
//		}
//		break;
//	default:
//		return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
//	}
//	return (0L);
//}

/*----------------------------------------------------------------------------------------
	検索結果一覧通知メッセージ処理
----------------------------------------------------------------------------------------*/
LRESULT OnNotifySearchList(HWND hOutputTab, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMLISTVIEW lpNmLv = (LPNMLISTVIEW)lp;
	switch(lpNmLv->hdr.code){
	case LVN_GETINFOTIP:
		break;
	case NM_DBLCLK:
		SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_SEARCH_SEL, 0L);
		break;
	case LVN_KEYDOWN:
		switch(((LPNMLVKEYDOWN)lp)->wVKey){
		case VK_RETURN:
			SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_SEARCH_SEL, 0L);
			break;
		case VK_APPS:
			TrackOutputContextMenu(ghMainWnd, hOutputTab);
			break;
		default:
			return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
		}
		break;
	default:
		return (CallWindowProc(gOrgTabWndProc, hOutputTab, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	アウトプットコンテキストメニューメッセージ処理
----------------------------------------------------------------------------------------*/
LRESULT OnContextMenuOutputChain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	POINT pt;
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	TrackOutputContextMenu(ghMainWnd, hWnd, &pt);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ポップアップメニューの表示
----------------------------------------------------------------------------------------*/
void TrackOutputContextMenu(HWND hMainWnd, HWND hOutputTab)
{
	RECT rc;
	POINT pt;
	GetWindowRect(hOutputTab, &rc);
	pt.x = rc.left;
	pt.y = rc.top;
	TrackOutputContextMenu(hMainWnd, hOutputTab, &pt);
}

/*----------------------------------------------------------------------------------------
	ポップアップメニューの表示
----------------------------------------------------------------------------------------*/
void TrackOutputContextMenu(HWND hMainWnd, HWND hOutputTab, LPPOINT lppt)
{
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hMainWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return;

	HMENU hMenu, hPopup = NULL;
//	hMenu = LoadMenu((HINSTANCE)GetWindowLong(hMainWnd, GWL_HINSTANCE),
	hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hMainWnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(IDR_MENU_OUTPOPUP));

	HWND hOutputWnd = GetParent(hOutputTab);
	if(!hOutputWnd) return;
	// 範囲チェック
	int nTab = TabCtrl_GetCurSel(hOutputTab);
	HWND hOutputSubWnd = GetOutputSubWnd(hOutputTab, nTab);
	if(!hOutputSubWnd) return;
	RECT rcOutput;
	GetWindowRect(hOutputSubWnd, &rcOutput);
	POINT pt = { lppt->x, lppt->y };
	if(rcOutput.left > lppt->x) pt.x = rcOutput.left;
	if(rcOutput.top > lppt->y) pt.y = rcOutput.top;
	if(rcOutput.right < lppt->x) pt.x = rcOutput.right;
	if(rcOutput.bottom < lppt->y) pt.y = rcOutput.bottom;

	// アクティブなタブでメニューを変える
	int nSel = 0, nNo = 0;
	UINT unDefaultID = 0;
	switch(nTab){
	case TAB_OUTPUT:
		break;
	case TAB_SOURCE:
		if(SourcesListCount(hOutputWnd) > 0){
			if((nSel = SourcesListGetSel(hOutputWnd)) < 0){
				hPopup = GetSubMenu(hMenu, POPUP_SOURCE_NOSEL);
			}else{
				hPopup = GetSubMenu(hMenu, POPUP_SOURCE);
				unDefaultID = IDM_SHOPENSRC;
			}
		}
		break;
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case TAB_ANALYSYS:
//		if(AnalysysListCount(hOutputWnd) > 0) hPopup = GetSubMenu(hMenu, POPUP_ANALYSYS);
//		break;
	case TAB_SEARCH:
		if(SearchListCount(hOutputWnd) > 0){
			hPopup = GetSubMenu(hMenu, POPUP_SEARCH);
			unDefaultID = IDM_SEARCH_SEL;
		}
		break;
	case TAB_DATA:
		hPopup = GetSubMenu(hMenu, POPUP_DATA);
		break;
	default:
		break;
	}
	if(hPopup){
		if(unDefaultID > 0){
			MENUITEMINFO mif;
			ZeroMemory(&mif, sizeof(MENUITEMINFO));
			mif.cbSize = sizeof(MENUITEMINFO);
			mif.fMask = MIIM_STATE;
			mif.fState = MFS_DEFAULT;
			SetMenuItemInfo(hPopup, unDefaultID, FALSE, &mif);
		}
		TrackPopupMenuEx(hPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					(int)pt.x, (int)pt.y, hMainWnd, NULL);
	}
	DestroyMenu(hMenu);
}

/*----------------------------------------------------------------------------------------
	ソース一覧項目数の取得
----------------------------------------------------------------------------------------*/
int SourcesListCount(HWND hOutputWnd)
{
	HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
	if(!hOutputTab) return -1;

	HWND hSourceList = GetOutputSubWnd(hOutputTab, TAB_SOURCE);
	if(!hSourceList) return -1;

	return ((int)ListView_GetItemCount(hSourceList));
}

/*----------------------------------------------------------------------------------------
	ソース一覧選択項目の取得
----------------------------------------------------------------------------------------*/
int SourcesListGetSel(HWND hOutputWnd, const int nFlg/* = 0*/)
{
	HWND hSourceList = NULL;
	if(nFlg == 0){
		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
		if(!hOutputTab) return -1;
		hSourceList = GetOutputSubWnd(hOutputTab, TAB_SOURCE);
	}else{
		hSourceList = GetOutputSubWnd(hOutputWnd, TAB_SOURCE);
	}
	if(!hSourceList) return -1;
	return (ListView_GetNextItem(hSourceList, -1, LVNI_ALL | LVNI_SELECTED));
}

/*----------------------------------------------------------------------------------------
	ソース一覧選択項目の取得
----------------------------------------------------------------------------------------*/
int SourcesListGetSelParam(HWND hOutputWnd, const int nFlg/* = 0*/)
{
	HWND hSourceList = NULL;
	if(nFlg == 0){
		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
		if(!hOutputTab) return -1;
		hSourceList = GetOutputSubWnd(hOutputTab, TAB_SOURCE);
	}else{
		hSourceList = GetOutputSubWnd(hOutputWnd, TAB_SOURCE);
	}
	if(!hSourceList) return -1;

	int nSel = 	ListView_GetNextItem(hSourceList, -1, LVNI_ALL | LVNI_SELECTED);
	if(nSel < 0) return -1;

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nSel;
	if(ListView_GetItem(hSourceList, &lvi)) return ((int)lvi.lParam);
	return -1;
}

/*----------------------------------------------------------------------------------------
	ソース一覧選択項目の取得
----------------------------------------------------------------------------------------*/
int SourcesListGetParam(HWND hOutputWnd, const int nItem, const int nFlg/* = 0*/)
{
	HWND hSourceList = NULL;
	if(nFlg == 0){
		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
		if(!hOutputTab) return -1;
		hSourceList = GetOutputSubWnd(hOutputTab, TAB_SOURCE);
	}else{
		hSourceList = GetOutputSubWnd(hOutputWnd, TAB_SOURCE);
	}
	if(!hSourceList) return -1;

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nItem;
	if(ListView_GetItem(hSourceList, &lvi)) return ((int)lvi.lParam);
	return -1;
}

/*----------------------------------------------------------------------------------------
	ソース一覧選択項目の選択
----------------------------------------------------------------------------------------*/
int SourcesListSetSel(HWND hOutputWnd, const int nNo, const int nFlg/* = 0*/)
{
	HWND hSourceList = NULL;
	if(nFlg == 0){
		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
		if(!hOutputTab) return -1;
		hSourceList = GetOutputSubWnd(hOutputTab, TAB_SOURCE);
	}else{
		hSourceList = GetOutputSubWnd(hOutputWnd, TAB_SOURCE);
	}
	if(!hSourceList) return -1;

	int nSel, nCount = ListView_GetItemCount(hSourceList);
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	for(nSel = 0; nSel < nCount; nSel++){
		lvi.iItem = nSel;
		if(ListView_GetItem(hSourceList, &lvi)){
			if((int)lvi.lParam == nNo){
				ListView_SetItemState(hSourceList,
					nSel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				SourcesListScroll(hSourceList);
				break;
			}
		}
	}
	return nSel;
}

/*----------------------------------------------------------------------------------------
	ソース一覧スクロール
----------------------------------------------------------------------------------------*/
void SourcesListScroll(HWND hSourceList)
{
	int nSel = ListView_GetNextItem(hSourceList, -1, LVNI_ALL | LVNI_SELECTED);
	RECT rcClient, rcItem;
	GetClientRect(hSourceList, &rcClient);
	ListView_GetItemRect(hSourceList, nSel, &rcItem, LVIR_BOUNDS);
	if(rcItem.top < 0) ListView_Scroll(hSourceList, rcItem.left, rcItem.top);
	if(rcItem.bottom > rcClient.bottom) ListView_Scroll(hSourceList, rcItem.left, rcItem.top - (rcItem.bottom - rcItem.top));
}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果一覧項目数の取得
//----------------------------------------------------------------------------------------*/
//int AnalysysListCount(HWND hOutputWnd)
//{
//	HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
//	if(!hOutputTab) return -1;
//
//	HWND hAnalysysList = GetOutputSubWnd(hOutputTab, TAB_ANALYSYS);
//	if(!hAnalysysList) return -1;
//
//	return ((int)ListView_GetItemCount(hAnalysysList));
//}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果一覧選択項目の取得
//----------------------------------------------------------------------------------------*/
//int AnalysysListGetSel(HWND hOutputWnd, const int nFlg/* = 0*/)
//{
//	HWND hAnalysysList = NULL;
//	if(nFlg == 0){
//		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
//		if(!hOutputTab) return -1;
//		hAnalysysList = GetOutputSubWnd(hOutputTab, TAB_ANALYSYS);
//	}else{
//		hAnalysysList = GetOutputSubWnd(hOutputWnd, TAB_ANALYSYS);
//	}
//	if(!hAnalysysList) return -1;
//	return (ListView_GetNextItem(hAnalysysList, -1, LVNI_ALL | LVNI_SELECTED));
//}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果一覧選択項目の取得
//----------------------------------------------------------------------------------------*/
//int AnalysysListGetParam(HWND hOutputWnd, const int nFlg/* = 0*/)
//{
//	HWND hAnalysysList = NULL;
//	if(nFlg == 0){
//		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
//		if(!hOutputTab) return -1;
//		hAnalysysList = GetOutputSubWnd(hOutputTab, TAB_ANALYSYS);
//	}else{
//		hAnalysysList = GetOutputSubWnd(hOutputWnd, TAB_ANALYSYS);
//	}
//	if(!hAnalysysList) return -1;
//
//	int nSel = ListView_GetNextItem(hAnalysysList, -1, LVNI_ALL | LVNI_SELECTED);
//	if(nSel < 0) return -1;
//
//	LVITEM lvi;
//	ZeroMemory(&lvi, sizeof(LVITEM));
//	lvi.mask = LVIF_PARAM;
//	lvi.iItem = nSel;
//	lvi.iSubItem = 0;
//	if(ListView_GetItem(hAnalysysList, &lvi)) return ((int)lvi.lParam);
//	return -1;
//}

/*----------------------------------------------------------------------------------------
	検索結果一覧項目数の取得
----------------------------------------------------------------------------------------*/
int SearchListCount(HWND hOutputWnd)
{
	HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
	if(!hOutputTab) return -1;

	HWND hSearchList = GetOutputSubWnd(hOutputTab, TAB_SEARCH);
	if(!hSearchList) return -1;

	return ((int)ListView_GetItemCount(hSearchList));
}

/*----------------------------------------------------------------------------------------
	検索結果一覧選択項目の取得
----------------------------------------------------------------------------------------*/
int SearchListGetSel(HWND hOutputWnd, const int nFlg/* = 0*/)
{
	HWND hSearchList = NULL;
	if(nFlg == 0){
		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
		if(!hOutputTab) return -1;
		hSearchList = GetOutputSubWnd(hOutputTab, TAB_SEARCH);
	}else{
		hSearchList = GetOutputSubWnd(hOutputWnd, TAB_SEARCH);
	}
	if(!hSearchList) return -1;

	int nSel = ListView_GetNextItem(hSearchList, -1, LVNI_ALL | LVNI_SELECTED);
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nSel;
	lvi.iSubItem = 0;
	if(ListView_GetItem(hSearchList, &lvi)) return ((int)lvi.lParam);
	return -1;
}

/*----------------------------------------------------------------------------------------
	検索結果一覧選択項目の取得
----------------------------------------------------------------------------------------*/
int SearchListGetParam(HWND hOutputWnd, const int nFlg/* = 0*/)
{
	HWND hSearchList = NULL;
	if(nFlg == 0){
		HWND hOutputTab = GetDlgItem(hOutputWnd, ID_OUTPUTTAB);
		if(!hOutputTab) return -1;
		hSearchList = GetOutputSubWnd(hOutputTab, TAB_SEARCH);
	}else{
		hSearchList = GetOutputSubWnd(hOutputWnd, TAB_SEARCH);
	}
	if(!hSearchList) return -1;

	int nSel = ListView_GetNextItem(hSearchList, -1, LVNI_ALL | LVNI_SELECTED);
	if(nSel < 0) return -1;

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nSel;
	lvi.iSubItem = 0;
	if(ListView_GetItem(hSearchList, &lvi)) return ((int)lvi.lParam);
	return -1;
}

/*----------------------------------------------------------------------------------------
	ファイルからアウトプットメッセージ追加
----------------------------------------------------------------------------------------*/
BOOL AddOutputMessageFromFile(HWND hWndMain, LPCTSTR lpcOutputFile, const int nFlag/* = TRACE_INFO*/)
{
	HANDLE hFile = CreateFile(lpcOutputFile,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		AddOutputMessage(hWndMain, "Readmeファイルが見つかりません...", nFlag);
		return FALSE;
	}
	DWORD dwSize = GetFileSize(hFile, NULL);
	if(dwSize > 0){
		char *pszOutputMsg = (char *)GlobalAlloc(GPTR, (dwSize + 1));
		if(pszOutputMsg){
			DWORD dwRead;
			if(ReadFile(hFile, pszOutputMsg, dwSize, &dwRead, NULL)){
				AddOutputMessage(hWndMain, pszOutputMsg, nFlag);
			}else{
				AddOutputMessage(hWndMain, "ファイルが読めません", nFlag);
			}
			GlobalFree(pszOutputMsg);
		}else{
			AddOutputMessage(hWndMain, "メモリが足りません", nFlag);
		}
	}else{
		AddOutputMessage(hWndMain, "0Byteファイルです", nFlag);
	}
	CloseHandle(hFile);

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	アウトプットメッセージ追加
----------------------------------------------------------------------------------------*/
BOOL AddOutputMessage(HWND hWndMain, LPCTSTR lpcOutputMsg, const int nFlag/* = TRACE_INFO*/)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWndMain, CSPLITWND_CLASS);
	if(!lpCWndSplit) return FALSE;

	char szOutputMsg[1024];
	int i = 0;
	for(LPCTSTR p = lpcOutputMsg; *p != '\0'; p++){
		if(*p == '\r') continue;
		if((*p == '\n')
		|| (i > sizeof(szOutputMsg))){
			szOutputMsg[i] = '\0';
			lpCWndSplit->SendMessageSplit(UM_ADDOUTPUT, (WPARAM)nFlag, (LPARAM)szOutputMsg);
			i = 0;
		}else{
			szOutputMsg[i] = *p;
			i++;
		}
	}
	szOutputMsg[i] = '\0';
	if(i > 0) lpCWndSplit->SendMessageSplit(UM_ADDOUTPUT, (WPARAM)nFlag, (LPARAM)szOutputMsg);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ソース一覧更新
----------------------------------------------------------------------------------------*/
BOOL UpdateSourceMessage(HWND hWndMain, const BOOL bSelTab)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWndMain, CSPLITWND_CLASS);
	if(!lpCWndSplit) return FALSE;

	lpCWndSplit->SendMessageSplit(UM_UPDSOURCE, (bSelTab ? (WPARAM)1 : (WPARAM)0), 0L);
	return TRUE;
}

//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
///*----------------------------------------------------------------------------------------
//	解析結果一覧更新
//----------------------------------------------------------------------------------------*/
//BOOL UpdateAnalysysMessage(HWND hWndMain, const BOOL bSelTab, const int nSelItem/* = -1*/)
//{
//	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWndMain, CSPLITWND_CLASS);
//	if(!lpCWndSplit) return FALSE;
//
//	lpCWndSplit->SendMessageSplit(UM_UPDANALYSYS, (bSelTab ? (WPARAM)1 : (WPARAM)0), (LPARAM)nSelItem);
//	return TRUE;
//}

/*----------------------------------------------------------------------------------------
	検索結果一覧追加
----------------------------------------------------------------------------------------*/
BOOL AddSearchListMessage(HWND hWndMain, const BOOL bSelTab, const int nTreeIndex/* = -1*/)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWndMain, CSPLITWND_CLASS);
	if(!lpCWndSplit) return FALSE;

	lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, (bSelTab ? (WPARAM)1 : (WPARAM)0), (LPARAM)nTreeIndex);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	タブ選択の切り替え
----------------------------------------------------------------------------------------*/
void OutputTabSetCursel(const UINT uiSelTab)
{
	if(!ghMainWnd) return;

	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(ghMainWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return;
	HWND hSplit = lpCWndSplit->GetWindowHandle();

	HWND hOutputTab = GetDlgItem(hSplit, ID_OUTPUTTAB);
	if(!hOutputTab) return;

	OutputTabSetCursel(hOutputTab, uiSelTab);
}

void OutputTabSetCursel(HWND hOutputTab, const UINT uiSelTab)
{
	if(TabCtrl_GetCurSel(hOutputTab) != uiSelTab){
		TabCtrl_SetCurSel(hOutputTab, uiSelTab);
		ShowOutputSubWnd(hOutputTab, uiSelTab);
	}else{
		// フォーカスだけはセット
		SeFopcustOutputSubWnd(hOutputTab, uiSelTab);
	}
}

/*----------------------------------------------------------------------------------------
	解析リスト番号から解析結果ツリーの展開
----------------------------------------------------------------------------------------*/
int ExpandAnaLstEx(int nTreeInfo)
{
	if(nTreeInfo < 0) return -1;
	// 解析結果リスト制御クラスインスタンス
	if(!ghMainWnd) return -1;
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(ghMainWnd, CANALST_CLASS);
	if(!lpCAnaLst) return -1;
	return (lpCAnaLst->SetSelEx(nTreeInfo));
}

BOOL ShowDataTab(
	HWND hMainWnd,
	BOOL bShow)
{
	if (!hMainWnd) {
		return FALSE;
	}

	//--------------------------------------------------
	// CWndSplitter取得
	//--------------------------------------------------
	CWndSplitter* lpCWndSplit =
		(CWndSplitter*)GetProp(
			hMainWnd,
			CSPLITWND_CLASS);

	if (!lpCWndSplit) {
		return FALSE;
	}

	//--------------------------------------------------
	// OutputWnd本体
	//--------------------------------------------------
	HWND hOutputWnd =
		lpCWndSplit->GetWindowHandle();

	if (!hOutputWnd) {
		return FALSE;
	}

	//--------------------------------------------------
	// TabControl
	//--------------------------------------------------
	HWND hOutputTab =
		GetDlgItem(
			hOutputWnd,
			ID_OUTPUTTAB);

	if (!hOutputTab) {
		return FALSE;
	}

	//--------------------------------------------------
	// DATAタブ検索
	//--------------------------------------------------
	int nCount =
		TabCtrl_GetItemCount(
			hOutputTab);

	int nDataTab = -1;
	HWND hDataPanel = NULL;

	TCITEM tci;
	ZeroMemory(
		&tci,
		sizeof(tci));

	tci.mask = TCIF_PARAM;

	for (int i = 0;
		i < nCount;
		i++)
	{
		if (!TabCtrl_GetItem(
			hOutputTab,
			i,
			&tci))
		{
			continue;
		}

		HWND hChild =
			(HWND)tci.lParam;

		if (hChild &&
			GetDlgCtrlID(hChild) ==
			IDD_DATAPANEL)
		{
			nDataTab = i;
			hDataPanel = hChild;

			break;
		}
	}

	//--------------------------------------------------
	// 表示
	//--------------------------------------------------
	if (bShow) {

		if (nDataTab >= 0) {
			return TRUE;
		}

		//--------------------------------------------------
		// 現在選択タブを保存
		//--------------------------------------------------
		int nCurTab =
			TabCtrl_GetCurSel(
				hOutputTab);

		//--------------------------------------------------
		// DATAパネル作成
		//--------------------------------------------------
		hDataPanel = CreateDialog((HINSTANCE)GetWindowLongPtr(hMainWnd, GWLP_HINSTANCE),
			MAKEINTRESOURCE(IDD_DATAPANEL),
			hOutputTab,
			DataPanelDlgProc);

		if (!hDataPanel) {
			return FALSE;
		}

		// ダイアログにIDを付与
		SetWindowLongPtr(hDataPanel, GWLP_ID, IDD_DATAPANEL);
		SetProp(hMainWnd, DATAPANEL_PROP, hDataPanel);
		//--------------------------------------------------
		// Tab追加
		//--------------------------------------------------
		TCITEM tciData;
		ZeroMemory(
			&tciData,
			sizeof(tciData));

		tciData.mask =
			TCIF_TEXT |
			TCIF_PARAM;

		tciData.pszText =
			"データ";

		tciData.lParam =
			(LPARAM)hDataPanel;

		int nNewTab =
			TabCtrl_InsertItem(
				hOutputTab,
				TabCtrl_GetItemCount(
					hOutputTab),
				&tciData);

		if (nNewTab < 0) {
			RemoveProp(
				hMainWnd,
				DATAPANEL_PROP);

			DestroyWindow(
				hDataPanel);

			return FALSE;
		}

		//--------------------------------------------------
		// 全サブウィンドウのサイズを再計算
		//--------------------------------------------------
		SendMessage(
			hOutputWnd,
			WM_SIZE,
			0,
			0);

		//--------------------------------------------------
		// 追加前の選択状態を復元
		//--------------------------------------------------
		if (nCurTab >= 0) {

			TabCtrl_SetCurSel(
				hOutputTab,
				nCurTab);

			ShowOutputSubWnd(
				hOutputTab,
				nCurTab);

			HWND hCurWnd =
				GetOutputSubWnd(
					hOutputTab,
					nCurTab);

			if (hCurWnd) {
				UpdateWindow(hCurWnd);
			}

			UpdateWindow(hOutputTab);
		}

		return TRUE;
	}

	//--------------------------------------------------
	// 削除
	//--------------------------------------------------
	if (nDataTab < 0) {
		return TRUE;
	}

	int nCur =
		TabCtrl_GetCurSel(
			hOutputTab);

	if (nCur == nDataTab) {

		TabCtrl_SetCurSel(
			hOutputTab,
			TAB_OUTPUT);

		ShowOutputSubWnd(
			hOutputTab,
			TAB_OUTPUT);
	}

	TabCtrl_DeleteItem(
		hOutputTab,
		nDataTab);

	if (hDataPanel) {
		RemoveProp(
			hMainWnd,
			DATAPANEL_PROP);

		DestroyWindow(hDataPanel);
	}

	return TRUE;
}

LRESULT CALLBACK DataPanelDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hMain, hEdit;
	CDataFile* lpCDataFile;
	ULONGLONG ullRec;
	switch (msg) {
	case WM_INITDIALOG:
		if ((hEdit = GetDlgItem(hDlg, IDC_EDIT_CURDAT))) {
			gOrgEditbocProc = (WNDPROC)GetWindowLongPtr(hEdit, GWLP_WNDPROC);
			SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)ChainCurRecEditProc);
			ShowWindow(hEdit, SW_HIDE);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BTN_FIRSTDAT:

			hMain = GetAncestor(hDlg, GA_ROOT);

			lpCDataFile = (CDataFile*)GetProp(hMain, CDATAFILE_CLASS);
			if (!lpCDataFile) return 0;

			SetDataRecord(hMain, 0);

			break;

		case IDC_BTN_PREVDAT:

			hMain = GetAncestor(hDlg, GA_ROOT);

			lpCDataFile = (CDataFile*)GetProp(hMain, CDATAFILE_CLASS);
			if (!lpCDataFile) return 0;

			ullRec = lpCDataFile->GetCurrentRecord();
			if (ullRec < 1)
			{
				MessageBeep(MB_ICONWARNING);
				UpdateDataPanel(hMain);
				return 0;
			}
			SetDataRecord(hMain, ullRec - 1);

			break;
		case IDC_BTN_NEXTDAT:

			hMain = GetAncestor(hDlg, GA_ROOT);

			lpCDataFile = (CDataFile*)GetProp(hMain, CDATAFILE_CLASS);
			if (!lpCDataFile) return 0;

			ullRec = lpCDataFile->GetCurrentRecord();
			if (ullRec + 1 >= lpCDataFile->GetRecordCount())
			{
				MessageBeep(MB_ICONWARNING);
				UpdateDataPanel(hMain);
				return 0;
			}
			SetDataRecord(hMain, ullRec + 1);

			break;
		case IDC_BTN_LASTDAT:

			hMain = GetAncestor(hDlg, GA_ROOT);

			lpCDataFile = (CDataFile*)GetProp(hMain, CDATAFILE_CLASS);
			if (!lpCDataFile) return 0;

			SetDataRecord(hMain, lpCDataFile->GetRecordCount() - 1);

			break;
		}
		break;

	case WM_PAINT:
		return (OnPaintDatePanel(hDlg, msg, wp, lp));

	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpdis =
			(LPDRAWITEMSTRUCT)lp;

		switch (lpdis->CtlID)
		{
		case IDC_BTN_FIRSTDAT:
		case IDC_BTN_PREVDAT:
		case IDC_BTN_NEXTDAT:
		case IDC_BTN_LASTDAT:
			return DrawDataMoveButton(lpdis);
		}
		break;
	}
	case WM_SIZE:
		return (OnSizeDatePanel(hDlg, msg, wp, lp));

	case WM_LBUTTONDOWN:
	{
		POINT pt = {
			GET_X_LPARAM(lp),
			GET_Y_LPARAM(lp)
		};

		RECT rcCur;
		GetCurrentRecordRect(
			hDlg,
			&rcCur);

		if (PtInRect(
			&rcCur,
			pt))
		{
			HWND hEdit =
				GetDlgItem(
					hDlg,
					IDC_EDIT_CURDAT);

			if (hEdit)
			{
				char szBuf[32];

				CDataFile* lpData =
					(CDataFile*)GetProp(
						GetAncestor(
							hDlg,
							GA_ROOT),
						CDATAFILE_CLASS);

				if (lpData)
				{
					sprintf_s(
						szBuf,
						"%llu",
						lpData->GetCurrentRecord() + 1);

					SetWindowText(
						hEdit,
						szBuf);
				}

				ShowWindow(
					hEdit,
					SW_SHOW);

				SetFocus(
					hEdit);

				SendMessage(
					hEdit,
					EM_SETSEL,
					0,
					-1);
			}
		}

		break;
	}

	case WM_DESTROY:
		if ((hEdit = GetDlgItem(hDlg, IDC_EDIT_CURDAT))) {
			SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)gOrgEditbocProc);
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT OnPaintDatePanel(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hDlg, &ps);

	RECT rc;
	GetClientRect(hDlg, &rc);

	CYamaSetup* lpCSetup =
		(CYamaSetup*)GetProp(ghMainWnd, CSETUP_CLASS);

	if (!lpCSetup) {
		EndPaint(hDlg, &ps);
		return 0L;
	}

	CImgListPng ilPng;

	HBITMAP hBmp =
		ilPng.LoadPngResource(
			(HINSTANCE)GetWindowLongPtr(
				ghMainWnd,
				GWLP_HINSTANCE),
			IDB_PNG_DATICO);

	if (!hBmp) {
		EndPaint(hDlg, &ps);
		return 0L;
	}

	HIMAGELIST hImgList =
		ilPng.MakeImageListPng(
			24,
			hBmp);

	DeleteObject(hBmp);

	// --------------------------------------------------
	// 背景
	// --------------------------------------------------
	HBRUSH hBrBkg =
		CreateSolidBrush(
			GetSysColor(COLOR_WINDOW));

	HPEN hPenBkg =
		CreatePen(
			PS_SOLID,
			0,
			GetSysColor(COLOR_WINDOWFRAME));

	HBRUSH hBrOld =
		(HBRUSH)SelectObject(
			hDC,
			hBrBkg);

	HPEN hPenOld =
		(HPEN)SelectObject(
			hDC,
			hPenBkg);

	Rectangle(
		hDC,
		rc.left,
		rc.top,
		rc.right,
		rc.bottom);

	SetBkMode(
		hDC,
		TRANSPARENT);

	SetTextColor(
		hDC,
		GetSysColor(COLOR_WINDOWTEXT));

	// --------------------------------------------------
	// 通常フォント
	// --------------------------------------------------
	HFONT hFont =
		MAKEFONT(
			hDC,
			11,
			FW_NORMAL,
			FALSE,
			lpCSetup->m_UIProp.m_szFontFace);

	HFONT hOldFont = NULL;

	if (hFont) {
		hOldFont =
			(HFONT)SelectObject(
				hDC,
				hFont);
	}

	// --------------------------------------------------
	// DATA
	// --------------------------------------------------
	CDataFile* lpCDataFile =
		(CDataFile*)GetProp(
			GetAncestor(
				hDlg,
				GA_ROOT),
			CDATAFILE_CLASS);

	if (lpCDataFile &&
		lpCDataFile->IsOpen())
	{
		char szBuf[256];

		// ==================================================
		// レイアウト基準
		// ==================================================
		const int xSep1 =
			rc.right / 3;

		const int xSep2 =
			(rc.right * 2) / 3;

		RECT rcCur;
		GetCurrentRecordRect(
			hDlg,
			&rcCur);

		// ==================================================
		// ファイル名
		// ==================================================
		const char* pszFile =
			lpCDataFile->GetFilePath();

		const char* pszName =
			strrchr(
				pszFile,
				'\\');

		if (pszName) {
			pszName++;
		}
		else {
			pszName = pszFile;
		}

		const int FILE_ICON_X = 12;
		const int FILE_ICON_Y = 10;

		ImageList_Draw(
			hImgList,
			0,
			hDC,
			FILE_ICON_X,
			FILE_ICON_Y,
			ILD_TRANSPARENT);

		RECT rcFile = {
			FILE_ICON_X + 30,
			6,
			rc.right - 16,
			34
		};

		DrawText(
			hDC,
			pszName,
			-1,
			&rcFile,
			DT_LEFT |
			DT_VCENTER |
			DT_SINGLELINE |
			DT_END_ELLIPSIS);

		// -----------------------------------------
		// ファイルサイズ
		// -----------------------------------------
		WIN32_FILE_ATTRIBUTE_DATA fad = {};

		char szFileSize[64] = "-";

		if (GetFileAttributesEx(
			pszFile,
			GetFileExInfoStandard,
			&fad))
		{
			ULARGE_INTEGER uli;

			uli.HighPart =
				fad.nFileSizeHigh;

			uli.LowPart =
				fad.nFileSizeLow;

			sprintf_s(
				szFileSize,
				"%llu Byte",
				uli.QuadPart);
		}

		SIZE sizeFileName;

		GetTextExtentPoint32(
			hDC,
			pszName,
			(int)strlen(pszName),
			&sizeFileName);

		int xFileSize =
			FILE_ICON_X +
			30 +
			sizeFileName.cx +
			16;

		RECT rcFileSize = {
			xFileSize,
			6,
			rc.right - 16,
			34
		};

		DrawText(
			hDC,
			szFileSize,
			-1,
			&rcFileSize,
			DT_VCENTER |
			DT_SINGLELINE);

		// ==================================================
		// 区切り線
		// ==================================================
		int ySepTop =
			rcCur.top + 4;

		int ySepBottom =
			rcCur.bottom - 4;

		HPEN hSepPen =
			CreatePen(
				PS_SOLID,
				1,
				GetSysColor(COLOR_3DSHADOW));

		HPEN hOldSepPen =
			(HPEN)SelectObject(
				hDC,
				hSepPen);

		MoveToEx(
			hDC,
			xSep1,
			ySepTop,
			NULL);

		LineTo(
			hDC,
			xSep1,
			ySepBottom);

		MoveToEx(
			hDC,
			xSep2,
			ySepTop,
			NULL);

		LineTo(
			hDC,
			xSep2,
			ySepBottom);

		SelectObject(
			hDC,
			hOldSepPen);

		DeleteObject(
			hSepPen);

		// ==================================================
		// レコード名
		// ==================================================
		const int REC_ICON_X =
			32;

		const int REC_ICON_Y =
			(rcCur.top +
				rcCur.bottom -
				24) / 2;

		ImageList_Draw(
			hImgList,
			0,
			hDC,
			REC_ICON_X,
			REC_ICON_Y,
			ILD_TRANSPARENT);

		RECT rcRecName = {
			REC_ICON_X + 30,
			rcCur.top,
			xSep1 - 12,
			rcCur.bottom
		};

		//
		char szRecName[256];

		if (!GetDataRecordName(
			ghMainWnd,
			szRecName,
			sizeof(szRecName)))
		{
			strcpy_s(
				szRecName,
				sizeof(szRecName),
				"-");
		}

		DrawText(
			hDC,
			szRecName,
			-1,
			&rcRecName,
			DT_LEFT |
			DT_VCENTER |
			DT_SINGLELINE |
			DT_END_ELLIPSIS);

		if (!IsWindowVisible(GetDlgItem(hDlg, IDC_EDIT_CURDAT))) {
			// ==================================================
			// カレントレコード
			// ==================================================
			ULONGLONG ullRec =
				lpCDataFile->GetCurrentRecord() + 1;

			char szRec[32];

			sprintf_s(
				szRec,
				"%llu",
				ullRec);

			HFONT hBigFont =
				MAKEFONT(
					hDC,
					26,
					FW_EXTRABOLD,
					FALSE,
					lpCSetup->m_UIProp.m_szFontFace);

			HFONT hOldBigFont =
				(HFONT)SelectObject(
					hDC,
					hBigFont);

			COLORREF colOldText =
				SetTextColor(
					hDC,
					RGB(25, 25, 139));

			DrawText(
				hDC,
				szRec,
				-1,
				&rcCur,
				DT_CENTER |
				DT_VCENTER |
				DT_SINGLELINE);

			SetTextColor(
				hDC,
				colOldText);

			SelectObject(
				hDC,
				hOldBigFont);

			DeleteObject(
				hBigFont);
		}

		// ==================================================
		// 総レコード数
		// ==================================================
		sprintf_s(
			szBuf,
			"/ %llu",
			lpCDataFile->GetRecordCount());

		RECT rcTotal = {
			rcCur.left,
			rcCur.bottom - 2,
			rcCur.right,
			rcCur.bottom + 18
		};

		DrawText(
			hDC,
			szBuf,
			-1,
			&rcTotal,
			DT_CENTER |
			DT_VCENTER |
			DT_SINGLELINE);

		// ==================================================
		// レコードサイズ
		// ==================================================
		const int SIZE_ICON_X =
			xSep2 + 32;

		const int SIZE_ICON_Y =
			(rcCur.top +
				rcCur.bottom -
				24) / 2;

		ImageList_Draw(
			hImgList,
			1,
			hDC,
			SIZE_ICON_X,
			SIZE_ICON_Y,
			ILD_TRANSPARENT);

		sprintf_s(
			szBuf,
			"%lu Byte",
			lpCDataFile->GetRecordSize());

		RECT rcRecLen = {
			SIZE_ICON_X + 30,
			rcCur.top,
			rc.right - 16,
			rcCur.bottom
		};

		DrawText(
			hDC,
			szBuf,
			-1,
			&rcRecLen,
			DT_LEFT |
			DT_VCENTER |
			DT_SINGLELINE);
	}

	// --------------------------------------------------
	// 後始末
	// --------------------------------------------------
	if (hOldFont) {
		SelectObject(
			hDC,
			hOldFont);
	}

	if (hFont) {
		DeleteObject(
			hFont);
	}

	SelectObject(
		hDC,
		hBrOld);

	SelectObject(
		hDC,
		hPenOld);

	if (hBrBkg) {
		DeleteObject(
			hBrBkg);
	}

	if (hPenBkg) {
		DeleteObject(
			hPenBkg);
	}

	if (hImgList) {
		ImageList_Destroy(
			hImgList);
	}

	EndPaint(
		hDlg,
		&ps);

	return TRUE;
}

void GetCurrentRecordRect(
	HWND hDlg,
	LPRECT lprc)
{
	RECT rc;
	GetClientRect(hDlg, &rc);

	SetRect(
		lprc,
		rc.right / 2 - 80,
		rc.bottom / 2 - 30,
		rc.right / 2 + 80,
		rc.bottom / 2 + 10);
}

BOOL GetDataRecordName(
	HWND hMainWnd,
	char* pszName,
	int nSize)
{
	if (!pszName || nSize <= 0) {
		return FALSE;
	}

	pszName[0] = '\0';

	CAnalysisList* lpCAnaLst =
		(CAnalysisList*)GetProp(
			hMainWnd,
			CANALST_CLASS);

	if (!lpCAnaLst) {
		return FALSE;
	}

	int nCount =
		lpCAnaLst->GetAllCount();

	for (int i = 0; i < nCount; i++) {

		RECORD_CBL rec;
		ZeroMemory(
			&rec,
			sizeof(rec));

		if (lpCAnaLst->GetItemDataEx(i, &rec) < 0) {
			continue;
		}

		// 01レベルをレコード名として採用
		if (rec.unLevel == 1) {

			strcpy_s(
				pszName,
				nSize,
				rec.szDescriptor);

			return TRUE;
		}
	}

	return FALSE;
}

LRESULT OnSizeDatePanel(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rc;
	GetClientRect(hDlg, &rc);

	SetWindowPos(GetDlgItem(hDlg, IDC_EDIT_CURDAT), hDlg, (rc.right - 80) / 2, (rc.bottom - 42) / 2, 80, 26, SWP_NOZORDER);

	const int BTN_W = 30;
	const int BTN_H = 22;
	const int GAP = 10;

	const int totalW =
		BTN_W * 4 +
		GAP * 3;

	int x = (rc.right - totalW) / 2;
	int y = rc.bottom - BTN_H - 8;

	SetWindowPos(
		GetDlgItem(hDlg, IDC_BTN_FIRSTDAT),
		NULL,
		x, y,
		BTN_W, BTN_H,
		SWP_NOZORDER);

	x += BTN_W + GAP;

	SetWindowPos(
		GetDlgItem(hDlg, IDC_BTN_PREVDAT),
		NULL,
		x, y,
		BTN_W, BTN_H,
		SWP_NOZORDER);

	x += BTN_W + GAP;

	SetWindowPos(
		GetDlgItem(hDlg, IDC_BTN_NEXTDAT),
		NULL,
		x, y,
		BTN_W, BTN_H,
		SWP_NOZORDER);

	x += BTN_W + GAP;

	SetWindowPos(
		GetDlgItem(hDlg, IDC_BTN_LASTDAT),
		NULL,
		x, y,
		BTN_W, BTN_H,
		SWP_NOZORDER);

	return TRUE;
}

LRESULT CALLBACK ChainCurRecEditProc(HWND hEdit, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_KEYDOWN:
		if (wp == VK_RETURN) {

			char szBuf[32];
			GetWindowText(
				hEdit,
				szBuf,
				sizeof(szBuf));

			ULONGLONG ullRec =
				_strtoui64(
					szBuf,
					NULL,
					10);

			HWND hDlg =
				GetParent(hEdit);

			HWND hMain =
				GetAncestor(
					hDlg,
					GA_ROOT);

			CDataFile* lpCDataFile =
				(CDataFile*)GetProp(
					hMain,
					CDATAFILE_CLASS);

			if (lpCDataFile) {
				if (ullRec < 1 ||
					ullRec > lpCDataFile->GetRecordCount())
				{
					MessageBeep(MB_ICONWARNING);
					// 不正値なら現在レコード表示へ戻す
					UpdateDataPanel(hMain);
				}
				else {
					SetDataRecord(
						hMain,
						ullRec - 1);
				}
			}
			ShowWindow(hEdit, SW_HIDE);

			return 0;
		}
		break;

	case WM_KILLFOCUS:
		ShowWindow(hEdit, SW_HIDE);
		return 0;
	}

	return (CallWindowProc(gOrgEditbocProc, hEdit, msg, wp, lp));
}

LRESULT MoveDataRecord(
	HWND hWnd,
	int nMove)
{
	CDataFile* lpCDataFile =
		(CDataFile*)GetProp(
			hWnd,
			CDATAFILE_CLASS);

	if (!lpCDataFile ||
		!lpCDataFile->IsOpen())
	{
		return -1;
	}

	ULONGLONG ullCur =
		lpCDataFile->GetCurrentRecord();

	ULONGLONG ullCount =
		lpCDataFile->GetRecordCount();

	if (nMove < 0) {
		if (ullCur == 0)
			return 0;

		return SetDataRecord(
			hWnd,
			ullCur - 1);
	}

	if (ullCur + 1 >= ullCount)
		return 0;

	return SetDataRecord(
		hWnd,
		ullCur + 1);
}

LRESULT SetDataRecord(
	HWND hWnd,
	ULONGLONG ullRecordNo)
{
	CDataFile* lpCDataFile =
		(CDataFile*)GetProp(
			hWnd,
			CDATAFILE_CLASS);

	if (!lpCDataFile ||
		!lpCDataFile->IsOpen())
	{
		return -1;
	}

	ULONGLONG ullCount =
		lpCDataFile->GetRecordCount();

	if (ullRecordNo >= ullCount) {
		return -1;
	}

	// 同じレコードなら何もしない
	if (ullRecordNo ==
		lpCDataFile->GetCurrentRecord())
	{
		return 0;
	}

	if (!lpCDataFile->ReadRecord(ullRecordNo)) {
		EXCMSG(
			hWnd,
			"レコードの読み込みに失敗しました");
		return -1;
	}

	CAnalysisList* lpCAnaLst =
		(CAnalysisList*)GetProp(
			hWnd,
			CANALST_CLASS);

	if (lpCAnaLst) {
		lpCAnaLst->RefreshDataValues();
	}

	UpdateDataPanel(hWnd);

	return 0;
}

void UpdateDataPanel(HWND hMainWnd)
{
	CDataFile* lpData =
		(CDataFile*)GetProp(hMainWnd, CDATAFILE_CLASS);

	if (!lpData || !lpData->IsOpen())
		return;

	HWND hDataPanel =
		(HWND)GetProp(
			hMainWnd,
			DATAPANEL_PROP);

	if (!hDataPanel ||
		!IsWindow(hDataPanel))
		return;

	InvalidateRect(hDataPanel, NULL, TRUE);
}

BOOL DrawDataMoveButton(
	LPDRAWITEMSTRUCT lpdis)
{
	HDC hDC = lpdis->hDC;
	RECT rc = lpdis->rcItem;

	BOOL bPressed =
		(lpdis->itemState & ODS_SELECTED) != 0;

	BOOL bDisabled =
		(lpdis->itemState & ODS_DISABLED) != 0;

	CImgListPng ilPng;

	HBITMAP hBmp =
		ilPng.LoadPngResource(
			(HINSTANCE)GetWindowLongPtr(
				ghMainWnd,
				GWLP_HINSTANCE),
			IDB_PNG_DATBTN);

	if (!hBmp) {
		return FALSE;
	}

	HIMAGELIST hImgList =
		ilPng.MakeImageListPng(
			24,
			hBmp);

	DeleteObject(hBmp);

	// --------------------------------------------------
	// 色
	// --------------------------------------------------
	COLORREF colBack;
	COLORREF colBorder;
	COLORREF colText;

	if (bDisabled) {

		colBack =
			GetSysColor(COLOR_BTNFACE);

		colBorder =
			GetSysColor(COLOR_3DSHADOW);

		colText =
			GetSysColor(COLOR_GRAYTEXT);
	}
	else if (bPressed) {

		colBack =
			RGB(225, 230, 240);

		colBorder =
			RGB(80, 100, 140);

		colText =
			GetSysColor(COLOR_WINDOWTEXT);
	}
	else {

		colBack =
			GetSysColor(COLOR_WINDOW);

		colBorder =
			RGB(160, 160, 160);

		colText =
			GetSysColor(COLOR_WINDOWTEXT);
	}

	// --------------------------------------------------
	// 背景＋枠
	// --------------------------------------------------
	HBRUSH hBrush =
		CreateSolidBrush(
			colBack);

	HPEN hPen =
		CreatePen(
			PS_SOLID,
			1,
			colBorder);

	HBRUSH hOldBrush =
		(HBRUSH)SelectObject(
			hDC,
			hBrush);

	HPEN hOldPen =
		(HPEN)SelectObject(
			hDC,
			hPen);

	RoundRect(
		hDC,
		rc.left,
		rc.top,
		rc.right,
		rc.bottom,
		5,
		5);

	SelectObject(
		hDC,
		hOldPen);

	SelectObject(
		hDC,
		hOldBrush);

	DeleteObject(
		hPen);

	DeleteObject(
		hBrush);

	// --------------------------------------------------
	// ボタン画像
	// --------------------------------------------------

	int nImgIdx = 0;
	switch (lpdis->CtlID)
	{
	case IDC_BTN_FIRSTDAT:
		nImgIdx = 0;
		break;

	case IDC_BTN_PREVDAT:
		nImgIdx = 1;
		break;

	case IDC_BTN_NEXTDAT:
		nImgIdx = 2;
		break;

	case IDC_BTN_LASTDAT:
		nImgIdx = 3;
		break;
	}

	ImageList_Draw(
		hImgList,
		nImgIdx,
		hDC,
		(rc.right - rc.left - ilPng.GetBtnWith()) / 2,
		(rc.bottom - rc.top - ilPng.GetBtnHeight()) / 2,
		ILD_TRANSPARENT);

	ImageList_Destroy(hImgList);

	return TRUE;
}
