/*----------------------------------------------------------------------------------------

	Edit.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "EditDlg.h"
#include "resource.h"

#ifndef IMF_DUALFONT
#define IMF_DUALFONT		0x0080
#endif	// IMF_DUALFONT

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HINSTANCE ghInst = NULL;
static HMODULE ghRichEd = NULL;
static BOOL gbChanged = FALSE;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
BOOL CreateToolbar(HWND hParent, const BOOL bRepSrc);
BOOL CreateStatusbar(HWND hParent);
HWND CreateREditCtrl(HWND hParent);
void SetupREditCtrl(HWND hREdit);
LRESULT CALLBACK EditDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnInitDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPEDITDLGPARAM lpedp);
LRESULT OnActivateDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDestroyDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPEDITDLGPARAM lpedp);
LRESULT OnSizeDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnCommandDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPEDITDLGPARAM lpedp);
void OnEditCommand(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
void IsCanselDo(HWND hDlg);
LRESULT OnContextMenuDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnNotifyDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnCloseDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
void SetTabSpace(HWND hREdit, const int nTabChar);
BOOL OpenEditFile(HWND hParent, HWND hREdit);
BOOL SaveEditFile(HWND hParent, HWND hREdit);

/*----------------------------------------------------------------------------------------
	編集用ダイアログの作成
----------------------------------------------------------------------------------------*/
int CreateEditDlg(HWND hParent, CAnaCBL *lpCAnaCBL, LPRECT lprcSize, const int nSrcNo/* = -1*/)
{
//	ghInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	ghInst = (HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE);
	char *pszSrcCode = (char *)GlobalAlloc(GPTR, EDIT_LIMIT_TEXT + 1);
	if(!pszSrcCode) return -1;
	// 番号指定があればそれを使う
	if(nSrcNo >= 0){
		SOURCEINFO SrcInfo;
		lpCAnaCBL->GetSourceData(nSrcNo, &SrcInfo);
		if(SrcInfo.dwSourceSize > EDIT_LIMIT_TEXT){
			ERRMSG(hParent, "ソースコードが大きすぎます");
			return -1;
		}
//		strcpy(pszSrcCode, SrcInfo.pszSourceCode);
		strcpy_s(pszSrcCode, EDIT_LIMIT_TEXT, SrcInfo.pszSourceCode);
	}
	int nRetNo = -1;
	EDITDLGPARAM edp;
	edp.pszSrcCode = pszSrcCode;
	edp.lprc = lprcSize;
	if(DialogBoxParam(ghInst,
		MAKEINTRESOURCE(IDD_EDITBOX),
		hParent,
		(DLGPROC)EditDlgProc,
		(LPARAM)&edp) == IDOK){
		if(nSrcNo >= 0){
			nRetNo = lpCAnaCBL->RepSourceCode(nSrcNo, pszSrcCode);
		}else{
			nRetNo = lpCAnaCBL->AddSourceCode(pszSrcCode);
		}
	}
	GlobalFree(pszSrcCode);
	return nRetNo;
}

/*----------------------------------------------------------------------------------------
	ツールバーの作成
----------------------------------------------------------------------------------------*/
BOOL CreateToolbar(HWND hParent, const BOOL bRepSrc)
{
	TBBUTTON tbbtn[] = {
		{0, IDM_EDITADDSRC, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, 0, 0},
		{1, IDM_EDITOPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, 0, 0},
		{2, IDM_EDITSAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, 0, 0}
	};
	const char *pszTipText[] = {
		(bRepSrc ? "解析リストを置換え" : "解析リストに追加"),
		"ファイルを開く",
		"名前を付けてファイルに保存"
	};
	CBmpToolbar *lpCBmpTb = (CBmpToolbar *)new CBmpToolbar(ghInst, hParent, "EditToolbar");
	if(!lpCBmpTb) return FALSE;
	tbbtn[0].iString = lpCBmpTb->AddString("解析一覧に追加");
	lpCBmpTb->MakeToolbar(ID_EDITTOOLBAR, TBSTYLE_FLAT);
//	lpCBmpTb->SetImageList(16, IDB_BITMAP7, IDB_BITMAP8, TB_IMG_ENABLE);
//	lpCBmpTb->SetImageList(16, IDB_BITMAP7D, IDB_BITMAP8, TB_IMG_DISABLE);
	lpCBmpTb->SetPngImageList(16, IDR_PNG_EDIT);
	lpCBmpTb->AddBtn(tbbtn, (const int)(sizeof(tbbtn) / sizeof(TBBUTTON)));
	lpCBmpTb->SetTooltips(pszTipText);
	lpCBmpTb->SetIndent(2);
	lpCBmpTb->ShowToolbar();
	SetProp(hParent, EDIT_CBMPTB_CLASS, (HANDLE)lpCBmpTb);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ステータスバーの作成
----------------------------------------------------------------------------------------*/
BOOL CreateStatusbar(HWND hParent)
{
	int nParts[] = { 80, 10, 5, 5 };
	CProgStbar *lpCStatus = (CProgStbar *)new CProgStbar(ghInst,
		hParent,
		ID_EDITSTBAR, ID_EDITPROG,
		"",
		nParts, 4,
		1);
	if(!lpCStatus) throw "ステータスバー制御クラスインスタンス作成失敗";
	lpCStatus->CreateBar();
	lpCStatus->SetSbText(0, SBT_NOBORDERS, "ｺｰﾄﾞ編集");
	SetProp(hParent, EDIT_CPROGST_CLASS, (HANDLE)lpCStatus);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	リッチエディットコントロールの作成
----------------------------------------------------------------------------------------*/
HWND CreateREditCtrl(HWND hParent)
{
	if(!(ghRichEd = LoadLibrary(REDIT2_DLL))) return NULL;
	return (CreateWindowEx(WS_EX_CLIENTEDGE,
							RICHEDIT_CLASS,
							"",
							WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_TABSTOP |
							ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_SAVESEL,
							0, 0,
							0, 0,
							hParent,
							(HMENU)ID_REDIT,
							ghInst,
							NULL));
}

/*----------------------------------------------------------------------------------------
	リッチエディットコントロールの設定？
----------------------------------------------------------------------------------------*/
void SetupREditCtrl(HWND hREdit)
{
	// 勝手にフォントが変わる？
	int nLangOpt = (int)SendMessage(hREdit, EM_GETLANGOPTIONS, 0L, 0L);
	nLangOpt &= ~(IMF_DUALFONT);
	SendMessage(hREdit, EM_SETLANGOPTIONS, 0L, (LPARAM)nLangOpt);
	// デフォルト書式
	CHARFORMAT cfmt;
	ZeroMemory(&cfmt, sizeof(CHARFORMAT));
	cfmt.cbSize = sizeof(CHARFORMAT);
	cfmt.dwMask = CFM_CHARSET | CFM_SIZE | CFM_FACE | CFM_COLOR;
	cfmt.bCharSet = SHIFTJIS_CHARSET;
	cfmt.yHeight = 200;
	cfmt.dwEffects = CFE_AUTOCOLOR;
//	strcpy(cfmt.szFaceName, "ＭＳ ゴシック");
	strcpy_s(cfmt.szFaceName, sizeof(cfmt.szFaceName), "ＭＳ ゴシック");
	SendMessage(hREdit, EM_SETCHARFORMAT, (WPARAM)SCF_DEFAULT/*SCF_SELECTION | SCF_WORD*/, (LPARAM)&cfmt);
	// イベントマスク設定
	SendMessage(hREdit, EM_SETEVENTMASK, 0L, (LPARAM)ENM_CHANGE);
	// アンドゥ設定
	SendMessage(hREdit, EM_SETUNDOLIMIT, (WPARAM)100, 0L);
}

/*----------------------------------------------------------------------------------------
	編集用ダイアログプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK EditDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	static LPEDITDLGPARAM lpedp = NULL;
	switch(msg){
	case WM_INITDIALOG:
		lpedp = (LPEDITDLGPARAM)lp;
		if(!lpedp){
			SendMessage(hDlg, WM_CLOSE, 0L, 0L);
			break;
		}
		return (OnInitDialog(hDlg, msg, wp, lp, lpedp));
	case WM_ACTIVATE:
		return (OnActivateDialog(hDlg, msg, wp, lp));
	case WM_SIZE:
		return (OnSizeDialog(hDlg, msg, wp, lp));
	case WM_COMMAND:
		return (OnCommandDialog(hDlg, msg, wp, lp, lpedp));
	case WM_CONTEXTMENU:
		return (OnContextMenuDialog(hDlg, msg, wp, lp));
	case WM_NOTIFY:
		return (OnNotifyDialog(hDlg, msg, wp, lp));
	case WM_CLOSE:
		return (OnCloseDialog(hDlg, msg, wp, lp));
	case WM_DESTROY:
		return (OnDestroyDialog(hDlg, msg, wp, lp, lpedp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ダイアログ作成時
----------------------------------------------------------------------------------------*/
LRESULT OnInitDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPEDITDLGPARAM lpedp)
{
//	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
	HICON hIconEdit = (HICON)LoadImage(hInst,
									MAKEINTRESOURCE(IDI_ICON01),
									IMAGE_ICON,
									16, 16,
									LR_SHARED);
	SendMessage(hDlg, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconEdit);
	SetWindowText(hDlg, EDITDLG_CAPTION);
	BOOL bRepSrc = (strlen(lpedp->pszSrcCode) > 0 ? TRUE : FALSE);
	// メニュー文字列の設定
	if(bRepSrc){
		HMENU hMenu, hFileMenu;
		hMenu = GetMenu(hDlg);
		hFileMenu = GetSubMenu(hMenu, 0);
		char szMenuString[] = "ｿｰｽ一覧を置換え(&A)";
		MENUITEMINFO mif;
		ZeroMemory(&mif, sizeof(MENUITEMINFO));
		mif.cbSize = (UINT)sizeof(MENUITEMINFO);
		mif.fMask = MIIM_TYPE;
		mif.fType = MFT_STRING;
		mif.dwTypeData = szMenuString;
		mif.cch = (int)strlen(szMenuString);
		SetMenuItemInfo(hFileMenu, IDM_EDITADDSRC, FALSE, &mif);
	}
	if(!CreateToolbar(hDlg, bRepSrc)) ERRMSG(hDlg, "ツールバー作成に失敗");
	if(!CreateStatusbar(hDlg)) ERRMSG(hDlg, "ステータスバー作成に失敗");
	HWND hREdit = NULL;
	if(!(hREdit = CreateREditCtrl(hDlg))) ERRMSG(hDlg, "エディットボックス作成に失敗");
	SetupREditCtrl(hREdit);
	if(strlen(lpedp->pszSrcCode) > 0){
		SetWindowText(hREdit, LPCTSTR(lpedp->pszSrcCode));
		// カレット？キャレット？位置設定
		SendMessage(hREdit, EM_SETSEL, (WPARAM)0, (LPARAM)0);
		SendMessage(hREdit, EM_SCROLLCARET, 0L, 0L);
	}
	// ウィンドウ位置・サイズの復元
	if((lpedp->lprc->left > 0)
	&& (lpedp->lprc->top > 0)
	&& (lpedp->lprc->right > 0)
	&& (lpedp->lprc->bottom > 0)){
		if((lpedp->lprc->left > 0)
		&& (lpedp->lprc->top > 0)){
			SetWindowPos(hDlg,
				HWND_TOP,
				lpedp->lprc->left, lpedp->lprc->top,
				lpedp->lprc->right, lpedp->lprc->bottom,
				SWP_NOZORDER);
		}else{
			SetWindowPos(hDlg,
				HWND_TOP,
				0, 0,
				lpedp->lprc->right, lpedp->lprc->bottom,
				SWP_NOZORDER | SWP_NOMOVE);
		}
	}
	IsCanselDo(hDlg);
	gbChanged = FALSE;
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ダイアログアクティブ時
----------------------------------------------------------------------------------------*/
LRESULT OnActivateDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	if(LOWORD(wp) != WA_INACTIVE) SetFocus(GetDlgItem(hDlg, ID_REDIT));
	return FALSE;
}

/*----------------------------------------------------------------------------------------
	ダイアログ破棄時
----------------------------------------------------------------------------------------*/
LRESULT OnDestroyDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPEDITDLGPARAM lpedp)
{
	RECT rc;
	GetWindowRect(hDlg, &rc);
	SetRect(lpedp->lprc,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top);
	CBmpToolbar *lpCBmpTb = (CBmpToolbar *)GetProp(hDlg, EDIT_CBMPTB_CLASS);
	if(lpCBmpTb){
		RemoveProp(hDlg, EDIT_CBMPTB_CLASS);
		delete lpCBmpTb;
		lpCBmpTb = NULL;
	}
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hDlg, EDIT_CPROGST_CLASS);
	if(lpCStatus){
		RemoveProp(hDlg, EDIT_CPROGST_CLASS);
		delete lpCStatus;
		lpCStatus = NULL;
	}
	HWND hREdit = GetDlgItem(hDlg, ID_REDIT);
	if(hREdit) DestroyWindow(hREdit);
	if(ghRichEd){
		FreeLibrary(ghRichEd);
		ghRichEd = NULL;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ダイアログサイズ変更時
----------------------------------------------------------------------------------------*/
LRESULT OnSizeDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rcTb = { 0, 0, 0, 0 },
		 rcSt = { 0, 0, 0, 0 };
	CBmpToolbar *lpCBmpTb = (CBmpToolbar *)GetProp(hDlg, EDIT_CBMPTB_CLASS);
	if(lpCBmpTb){
		lpCBmpTb->Resize(hDlg, msg, wp, lp);
		lpCBmpTb->GetRect(&rcTb);
	}
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hDlg, EDIT_CPROGST_CLASS);
	if(lpCStatus){
		lpCStatus->Resize(hDlg, msg, wp, lp);
		lpCStatus->GetRect(&rcSt);
	}
	HWND hREdit = GetDlgItem(hDlg, ID_REDIT);
	if(hREdit){
		int nEdge = GetSystemMetrics(SM_CYEDGE);
		MoveWindow(hREdit,
			0, rcTb.bottom + nEdge,
			LOWORD(lp), HIWORD(lp) - (rcTb.bottom + rcSt.bottom + nEdge),
			TRUE);
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ダイアログコマンド時
----------------------------------------------------------------------------------------*/
LRESULT OnCommandDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPEDITDLGPARAM lpedp)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hDlg, EDIT_CPROGST_CLASS);
	switch(LOWORD(wp)){
	case ID_REDIT:
		switch(HIWORD(wp)){
		case EN_CHANGE:
			IsCanselDo(hDlg);
			if(!gbChanged) gbChanged = TRUE;
			break;
		default:
			return FALSE;
		}
		break;
	case IDM_EDITADDSRC:
		if(GetDlgItemText(hDlg, ID_REDIT, lpedp->pszSrcCode, EDIT_LIMIT_TEXT) > 0){
			EndDialog(hDlg, IDOK);
		}else{
			if(lpCStatus){
				lpCStatus->SetSbText(0, 0, "一覧へ追加すべきｺｰﾄﾞが記述されていません");
			}else{
				ERRMSG(hDlg, "ソースコードがありません");
			}
		}
		break;
	case IDM_EDITOPEN:
		if(lpCStatus){
			lpCStatus->SetSbText(0, 0, "指定されたﾌｧｲﾙを開きます...");
		}
		if(OpenEditFile(hDlg, GetDlgItem(hDlg, ID_REDIT))){
			lpCStatus->SetSbText(0, 0, "ﾚﾃﾞｨ");
		}else{
			lpCStatus->SetSbText(0, 0, "指定されたﾌｧｲﾙが開けません");
		}
		break;
	case IDM_EDITSAVE:
		if(lpCStatus){
			lpCStatus->SetSbText(0, 0, "編集したｺｰﾄﾞを保存します");
		}
		if(SaveEditFile(hDlg, GetDlgItem(hDlg, ID_REDIT))){
			lpCStatus->SetSbText(0, 0, "ﾚﾃﾞｨ");
		}else{
			lpCStatus->SetSbText(0, 0, "ﾌｧｲﾙに保存できません");
		}
		break;
	case IDM_EDITCLOSE:
		SendMessage(hDlg, WM_CLOSE, 0L, 0L);
		break;
	case IDM_EDITUNDO:
	case IDM_EDITREDO:
	case IDM_EDITCUT:
	case IDM_EDITCOPY:
	case IDM_EDITPASTE:
	case IDM_EDITDEL:
	case IDM_EDITALLSEL:
		OnEditCommand(hDlg, msg, wp, lp);
		break;
//	case IDM_EDITFONT:
//		EXCMSG(hDlg, "できねーｗｗｗｗ m9（＾Д＾）ﾌﾟｷﾞｬｰ!!");
//		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	編集コマンド
----------------------------------------------------------------------------------------*/
void OnEditCommand(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hREdit = GetDlgItem(hDlg, ID_REDIT);
	if(!hREdit) return;

	switch(LOWORD(wp)){
	case IDM_EDITUNDO:
		SendMessage(hREdit, EM_UNDO, 0L, 0L);
		break;
	case IDM_EDITREDO:
		SendMessage(hREdit, EM_REDO, 0L, 0L);
		break;
	case IDM_EDITCUT:
		SendMessage(hREdit, WM_CUT, 0L, 0L);
		break;
	case IDM_EDITCOPY:
		SendMessage(hREdit, WM_COPY, 0L, 0L);
		break;
	case IDM_EDITPASTE:
		SendMessage(hREdit, WM_PASTE, 0L, 0L);
		break;
	case IDM_EDITDEL:
		SendMessage(hREdit, WM_CLEAR, 0L, 0L);
		break;
	case IDM_EDITALLSEL:
		SendMessage(hREdit, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
		break;
	default:
		return;
	}
	IsCanselDo(hDlg);
}

/*----------------------------------------------------------------------------------------
	メニューの使用可不可のチェック
----------------------------------------------------------------------------------------*/
void IsCanselDo(HWND hDlg)
{
	HWND hREdit = GetDlgItem(hDlg, ID_REDIT);
	if(!hREdit) return;
	CBmpToolbar *lpCBmpTb = (CBmpToolbar *)GetProp(hDlg, EDIT_CBMPTB_CLASS);
	if(!lpCBmpTb) return;

	HMENU hMenu, hMenuEdit;
	hMenu = GetMenu(hDlg);
	hMenuEdit = GetSubMenu(hMenu, EDIT_MENU_POS_EDIT);

	if(SendMessage(hREdit, EM_CANUNDO, 0L, 0L)){
		EnableMenuItem(hMenuEdit, IDM_EDITUNDO, MF_ENABLED | MF_BYCOMMAND);
	}else{
		EnableMenuItem(hMenuEdit, IDM_EDITUNDO, MF_GRAYED | MF_BYCOMMAND);
	}
	if(SendMessage(hREdit, EM_CANREDO, 0L, 0L)){
		EnableMenuItem(hMenuEdit, IDM_EDITREDO, MF_ENABLED | MF_BYCOMMAND);
	}else{
		EnableMenuItem(hMenuEdit, IDM_EDITREDO, MF_GRAYED | MF_BYCOMMAND);
	}
	if(SendMessage(hREdit, EM_CANPASTE, 0L, 0L)){
		EnableMenuItem(hMenuEdit, IDM_EDITPASTE, MF_ENABLED | MF_BYCOMMAND);
	}else{
		EnableMenuItem(hMenuEdit, IDM_EDITPASTE, MF_GRAYED | MF_BYCOMMAND);
	}
}

/*----------------------------------------------------------------------------------------
	コンテキストメニュー処理
----------------------------------------------------------------------------------------*/
LRESULT OnContextMenuDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	HMENU hMenu, hMenuEdit;
	hMenu = GetMenu(hDlg);
	hMenuEdit = GetSubMenu(hMenu, EDIT_MENU_POS_EDIT);
	if(!hMenuEdit) return FALSE;
	TrackPopupMenuEx(hMenuEdit,
		TPM_LEFTALIGN | TPM_VERTICAL | TPM_RIGHTBUTTON,
		LOWORD(lp), HIWORD(lp),
		hDlg,
		NULL);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	通知メッセージ処理
----------------------------------------------------------------------------------------*/
LRESULT OnNotifyDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	return FALSE;
}

/*----------------------------------------------------------------------------------------
	ダイアログクローズ時
----------------------------------------------------------------------------------------*/
LRESULT OnCloseDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	if(gbChanged){
		if(YESNOMSG(hDlg, "編集内容を破棄して終了します") == IDNO) return FALSE;
	}
	EndDialog(hDlg, IDCANCEL);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	リッチエディットのタブ幅変更
----------------------------------------------------------------------------------------*/
void SetTabSpace(HWND hREdit, const int nTabChar)
{
/*
	DEVMODE dm;
	ZeroMemnory(&dm, sizeof(DEVMODE));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
*/
	TEXTMETRIC tm;
	HDC hDC = GetDC(hREdit);
	GetTextMetrics(hDC, &tm);
	ReleaseDC(hREdit, hDC);
	int nTabSpace = nTabChar * PIXCEL2TWIP(tm.tmAveCharWidth);
	PARAFORMAT pfmt;
	ZeroMemory(&pfmt, sizeof(PARAFORMAT));
	pfmt.cbSize = sizeof(PARAFORMAT);
	SendMessage(hREdit, EM_GETPARAFORMAT, 0, (LPARAM)&pfmt);
	pfmt.dwMask |= PFM_TABSTOPS;
	pfmt.cTabCount = MAX_TAB_STOPS;
	for(int i = 0; i < MAX_TAB_STOPS; i++) pfmt.rgxTabs[i] = nTabSpace * (i + 1);
	SendMessage(hREdit, EM_SETPARAFORMAT, 0, (LPARAM)&pfmt);
}

/*----------------------------------------------------------------------------------------
	ファイルを開く
----------------------------------------------------------------------------------------*/
BOOL OpenEditFile(HWND hParent, HWND hREdit)
{
	static char szFilePath[MAX_PATH];
	// ファイルを開くダイアログ
	if(!GetOpenFileDlg(hParent,
		szFilePath,
		(const int)sizeof(szFilePath),
		"COBOLｿｰｽﾌｧｲﾙ (*.cob;*.cbl;*.cobol)\0*.cob;*.cbl;*.cobol\0"
		"全てのﾌｧｲﾙ (*.*)\0*.*\0",
		"ｿｰｽﾌｧｲﾙを開く")) return TRUE;
	// ファイルを開く
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwSize, dwRead = 0;
	char *pszBuf = NULL;
	try{
		hFile = CreateFile(szFilePath,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE) throw "指定されたファイルが開けません";
		dwSize = GetFileSize(hFile, NULL);
		if(dwSize > EDIT_LIMIT_TEXT) throw "ファイルサイズが大きすぎます";
		if(dwSize == 0) throw "ファイルサイズが0Byteのﾌｧｲﾙです";
		pszBuf = (char *)GlobalAlloc(GPTR, dwSize + 1);
		if(!pszBuf) throw "メモリ確保失敗";
		if(!ReadFile(hFile, pszBuf, dwSize, &dwRead, NULL)) throw "ファイルの読込みエラー";
	}
	catch(const char *pszErrMsg){
		EXCMSG(hParent, pszErrMsg);
		if(pszBuf) GlobalFree(pszBuf);
		if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		return FALSE;
	}
	SetWindowText(hREdit, pszBuf);
	GlobalFree(pszBuf);
	CloseHandle(hFile);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	編集ソースをファイルに保存
----------------------------------------------------------------------------------------*/
BOOL SaveEditFile(HWND hParent, HWND hREdit)
{
	static char szFilePath[MAX_PATH];
	// ファイルを保存ダイアログ
	if(!GetSaveFileDlg(hParent,
		szFilePath,
		(const int)sizeof(szFilePath),
		"COBOLｿｰｽﾌｧｲﾙ (*.cob;*.cbl;*.cobol)\0*.cob;*.cbl;*.cobol\0"
		"全てのﾌｧｲﾙ (*.*)\0*.*\0",
		"名前を付けてｿｰｽﾌｧｲﾙを保存"), "cob") return TRUE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwSize, dwWrite = 0;
	char *pszBuf = NULL;
	try{
		pszBuf = (char *)GlobalAlloc(GPTR, EDIT_LIMIT_TEXT + 1);
		if(!pszBuf) throw "メモリ確保失敗";
		if((dwSize = (DWORD)GetWindowText(hREdit, pszBuf, EDIT_LIMIT_TEXT)) < 1){
			if(YESNOMSG(hParent,
				"0Byteのファイルを作成しようとしてますが、"
				"これは意図したことですか？") == IDNO) throw "ﾚﾃﾞｨ";
		}
		// ファイルを開く
		hFile = CreateFile(szFilePath,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE) throw "指定されたファイルが開けません";
		if(!WriteFile(hFile, pszBuf, dwSize, &dwWrite, NULL)) throw "ファイルの書込みエラー";
		if(!SetEndOfFile(hFile)) throw "ファイルの書込みエラー";
	}
	catch(const char *pszErrMsg){
		EXCMSG(hParent, pszErrMsg);
		if(pszBuf) GlobalFree(pszBuf);
		if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		return FALSE;
	}
	GlobalFree(pszBuf);
	CloseHandle(hFile);
	return TRUE;
}
