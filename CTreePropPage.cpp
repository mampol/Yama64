/*------------------------------------------------------------------------------
	CTreePropPage.cpp
------------------------------------------------------------------------------*/
#include "CTreePropPage.h"

#if 0
void DebugDraw(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	HDC hDC = GetDC(hWnd);

	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
	HPEN hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));

	Rectangle(hDC, rc.left + 3, rc.top + 3, rc.right - 3, rc.bottom - 3);

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	HFONT hOldFont = (HFONT)SelectObject(hDC, GetStockObject(DEFAULT_GUI_FONT));
	INT_PTR nOldMode = SetBkMode(hDC, TRANSPARENT);

	TCHAR szHandle[20];
	wsprintf(szHandle, "0x%08x", hWnd);
	TextOut(hDC, 3, 3, szHandle, _tcslen(szHandle));

	SelectObject(hDC, hOldFont);
	SetBkMode(hDC, nOldMode);

	ReleaseDC(hWnd, hDC);
}
#endif

/*------------------------------------------------------------------------------
	コンストラクタ
------------------------------------------------------------------------------*/
CTreeProp::CTreeProp() :
 m_hMutex(NULL), bRegistWndClass(FALSE), m_hInst(NULL), m_hWndMain(NULL),
 m_lParam(NULL), m_hImgTreeArrow(NULL), m_dwFlags(0), m_dwTreeStyles(TREE_STYLES),
 m_hBtnOK(NULL), m_hBtnCan(NULL), m_nPageCount(0), m_nTreeItemHeight(TREE_ITEM_HEIGHT),
 m_bOK(FALSE)
{
}

/*------------------------------------------------------------------------------
	デストラクタ
------------------------------------------------------------------------------*/
CTreeProp::~CTreeProp()
{
	if(m_hImgTreeArrow) ImageList_Destroy(m_hImgTreeArrow);
	if(m_hGUIFont) DeleteObject(m_hGUIFont);
	if(m_hMutex){
		if(!CloseHandle(m_hMutex)){
			MessageBox(m_hWndParent, _T("ﾐｭｰﾃｸｽｵﾌﾞｼﾞｪｸﾄ破棄に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		}
	}
	if(bRegistWndClass){
		if(!UnregisterClass(m_szWndClass, m_hInst)){
			MessageBox(m_hWndParent, _T("ｳｨﾝﾄﾞｳｸﾗｽ解除に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		}
	}
}

/*------------------------------------------------------------------------------
	コモンコントロールの初期化
------------------------------------------------------------------------------*/
void CTreeProp::initComctl(DWORD dwICC)
{
	InitCommonControls();
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = dwICC;
	InitCommonControlsEx(&icc);
}

/*------------------------------------------------------------------------------
	プロパティシートメインウィンドウの作成
------------------------------------------------------------------------------*/
BOOL CTreeProp::CreateMain(HINSTANCE hInst,
						   HWND hParent,
						   LPCTSTR lpcWindowClass,
						   LPCTSTR lpcWindowCaption,
						   LPTSTR lpIconRes)
{
	m_nXEdge = GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXEDGE) + GetSystemMetrics(SM_CXFIXEDFRAME);
	m_nYEdge = GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYFIXEDFRAME) + 2;

	m_hMutex = CreateMutex(NULL, TRUE, lpcWindowClass);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		// 同じｸﾗｽ名で複数表示させない
		m_hMutex = NULL;
		MessageBox(hParent,
			_T("ﾌﾟﾛﾊﾟﾃｨｼｰﾄは既に表示されています"),
			lpcWindowCaption,
			MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
		SetForegroundWindow(FindWindow(lpcWindowClass, NULL));
		return FALSE;
	}

	if(!bRegistWndClass){
		// ウィンドウクラス登録
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX));
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = PropMainProp;
		wcex.hInstance = hInst;
		wcex.hIcon = (HICON)LoadImage(hInst,
			lpIconRes,
			IMAGE_ICON,
			32, 32,
			LR_DEFAULTCOLOR | LR_SHARED);
		wcex.hCursor = (HCURSOR)LoadImage(NULL,
			MAKEINTRESOURCE(IDC_ARROW),
			IMAGE_CURSOR,
			0, 0,
			LR_DEFAULTCOLOR | LR_SHARED);
		wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
		wcex.lpszClassName = lpcWindowClass;
		wcex.lpszMenuName = NULL;
		wcex.hIconSm = (HICON)LoadImage(hInst,
			lpIconRes,
			IMAGE_ICON,
			16, 16,
			LR_DEFAULTCOLOR);
		if(!RegisterClassEx(&wcex)){
			MessageBox(hParent, _T("ｳｨﾝﾄﾞｳｸﾗｽの登録に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
			return FALSE;
		}
		bRegistWndClass = TRUE;
	}

	// メンバセット
	_tcscpy_s(m_szWndClass, sizeof(m_szWndClass), lpcWindowClass);
	m_hInst = hInst;
	m_hWndParent = hParent;

	// ウィンドウ作成
	m_hWndMain = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT |
				(m_dwFlags & PSH_HASHELP ? WS_EX_CONTEXTHELP : 0),
				lpcWindowClass,
				lpcWindowCaption,
				WS_CAPTION | WS_CLIPSIBLINGS | WS_SYSMENU | WS_POPUP |
				WS_DLGFRAME | DS_MODALFRAME | DS_3DLOOK,
				0, 0,
				0, 0,
				hParent,
				NULL,
				hInst,
				(LPVOID)this);
	if(!m_hWndMain){
		MessageBox(hParent, _T("ｳｨﾝﾄﾞｳの作成に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// システムメニューの改造
	HMENU hSysMenu = GetSystemMenu(m_hWndMain, FALSE);
	if(hSysMenu){
		EnableMenuItem(hSysMenu, 0, MF_BYPOSITION | MF_GRAYED);	// 元のサイズに戻す
		EnableMenuItem(hSysMenu, 2, MF_BYPOSITION | MF_GRAYED);	// サイズ変更
		EnableMenuItem(hSysMenu, 3, MF_BYPOSITION | MF_GRAYED);	// 最小化
		EnableMenuItem(hSysMenu, 4, MF_BYPOSITION | MF_GRAYED);	// 最大化
	}

	ZeroMemory(&m_rcMain, sizeof(RECT));
	ZeroMemory(&m_rcTree, sizeof(RECT));
	ZeroMemory(&m_rcPage, sizeof(RECT));

	return TRUE;
}

/*------------------------------------------------------------------------------
	メッセージループ
------------------------------------------------------------------------------*/
void CTreeProp::MainMessageLoop()
{
	// アクセラレータテーブルの作成
	ACCEL accel[] = {
		{FVIRTKEY, VK_RETURN, IDOK},
		{FVIRTKEY, VK_ESCAPE, IDCANCEL},
		{FVIRTKEY | FCONTROL, VK_TAB, IDC_NEXTWNDSTOP},
		{FVIRTKEY | FCONTROL | FSHIFT, VK_TAB, IDC_PREVWNDSTOP}
	};
	HACCEL hAccel = CreateAcceleratorTable(accel, sizeof(accel) / sizeof(ACCEL));
	if(!hAccel) MessageBox(m_hWndMain, _T("ｱｸｾﾗﾚｰﾀﾃｰﾌﾞﾙの作成に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);

	ShowWindow(m_hWndMain, SW_SHOW);
	UpdateWindow(m_hWndMain);

	MSG msg;
	INT_PTR nRet;
	while((nRet = GetMessage(&msg, NULL, 0, 0))){
		if(nRet == -1){
			MessageBox(m_hWndMain, _T("ﾒｲﾝﾒｯｾｰｼﾞﾙｰﾌﾟでｴﾗｰが発生"), _T("Error"), MB_OK | MB_ICONSTOP);
			break;
		}
		// ダイアログメッセージの処理
		if(!TranslateAccelerator(m_hWndMain, hAccel, &msg)){
			if(!IsPageMessage(&msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	DestroyAcceleratorTable(hAccel);

	if(m_hMutex){
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}

HWND CTreeProp::IsPageMessage(MSG *lpmsg)
{
	HTREEITEM hItem = TreeView_GetSelection(m_hTreeView);
	if(hItem == NULL) return NULL;

	HWND hPage = GetHandle(hItem);
	if(hPage){
		if(IsDialogMessage(hPage, lpmsg)) return hPage;
	}
	return NULL;
}

/*------------------------------------------------------------------------------
	プロパティシートフレームプロシジャ
------------------------------------------------------------------------------*/
LRESULT CALLBACK CTreeProp::PropMainProp(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CTreeProp *lpCTreeProp;
	LPNMTREEVIEW lpNmTv;
	HDC hDC;
	PAINTSTRUCT ps;
	LPNODEPARAM lpNewNp, lpOldNp;

	switch(msg){
	case WM_CREATE:
		lpCTreeProp = (CTreeProp *)((LPCREATESTRUCT)lp)->lpCreateParams;
		SetProp(hWnd, CTREEPROP_ENTRY_CLASS, (HANDLE)lpCTreeProp);
		// 各コントロール作成
		if(!lpCTreeProp->MakeControl(hWnd)) return -1;
		// 親をロック
		if(lpCTreeProp->m_hWndParent) EnableWindow(lpCTreeProp->m_hWndParent, FALSE);
		break;
	case WM_NOTIFY:
		if(((LPNMHDR)lp)->idFrom == ID_TREEVIEW){
			lpNmTv = (LPNMTREEVIEW)lp;
			switch(lpNmTv->hdr.code){
			case TVN_SELCHANGED:
				lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
				if(!lpCTreeProp) break;
				if((lpOldNp = (LPNODEPARAM)lpNmTv->itemOld.lParam)){
					if(lpOldNp->hPage){
						ShowWindow(lpOldNp->hPage, SW_HIDE);
						SendMessage(lpOldNp->hPage, UM_TPM_KILLACTIVE, (WPARAM)lpOldNp->lpParam, (LPARAM)lpCTreeProp->m_lParam);
					}
				}
				if((lpNewNp = (LPNODEPARAM)lpNmTv->itemNew.lParam)){
					if(lpNewNp->hPage){
						ShowWindow(lpNewNp->hPage, SW_SHOW);
						SendMessage(lpNewNp->hPage, UM_TPM_SETACTIVE, (WPARAM)lpNewNp->lpParam, (LPARAM)lpCTreeProp->m_lParam);
					}
				}
				if(WS_DISABLED & GetWindowLongPtr(lpNewNp->hPage, GWL_STYLE)) EnableWindow(lpNewNp->hPage, TRUE);
				SetWindowText(lpCTreeProp->m_hSubTitle, DEFAULT_CAPTION);
				break;
			case TVN_DELETEITEM:
				lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
				if(!lpCTreeProp) break;
				// ノードに関連づけたメモリの開放と通知
				lpOldNp = (LPNODEPARAM)lpNmTv->itemOld.lParam;
				if(lpOldNp){
					SendMessage(lpOldNp->hPage, UM_TPM_DELETEPAGE, (WPARAM)lpOldNp->lpParam, (LPARAM)lpCTreeProp->m_lParam);
					GlobalFree(lpOldNp);
				}
				break;
			default:
				return (DefWindowProc(hWnd, msg, wp, lp));
			}
		}else{
			return (DefWindowProc(hWnd, msg, wp, lp));
		}
		break;
	case WM_DRAWITEM:
		lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
		if(!lpCTreeProp) break;
		if(((LPDRAWITEMSTRUCT)lp)->hwndItem == lpCTreeProp->m_hSubTitle){
			lpCTreeProp->DrawSubTitle((LPDRAWITEMSTRUCT)lp);
		}else{
			return (DefWindowProc(hWnd, msg, wp, lp));
		}
		break;
	case WM_PAINT:
		lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
		if(!lpCTreeProp) break;
		hDC = BeginPaint(hWnd, &ps);
		lpCTreeProp->Decoration(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		switch(LOWORD(wp)){
		case IDOK:
			lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
			if(lpCTreeProp){
				lpCTreeProp->SendUpdate();
				if(lpCTreeProp->m_bOK) SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			}else{
				SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			}
			break;
		case IDCANCEL:
			SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			break;
		case IDC_NEXTWNDSTOP:
		case IDC_PREVWNDSTOP:
			lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
			if(!lpCTreeProp) break;
			lpCTreeProp->SetNextWndFocus();
			break;
		default:
			return (DefWindowProc(hWnd, msg, wp, lp));
		}
		break;
	case WM_CLOSE:
		// 親をロック解除(破棄前に実行しないと親がＺオーダで無視される)
		lpCTreeProp = (CTreeProp *)GetProp(hWnd, CTREEPROP_ENTRY_CLASS);
		if(lpCTreeProp){
			if(lpCTreeProp->m_hWndParent) EnableWindow(lpCTreeProp->m_hWndParent, TRUE);
		}
		if(!RemoveProp(hWnd, CTREEPROP_ENTRY_CLASS)){
			MessageBox(hWnd, _T("ｸﾗｽｴﾝﾄﾘ削除失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		}
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}

	return (0L);
}

/*------------------------------------------------------------------------------
	プロパティシートの各コントロール作成
------------------------------------------------------------------------------*/
BOOL CTreeProp::MakeControl(HWND hParent)
{
	// ツリービュー作成
	m_hTreeView = CreateWindowEx(WS_EX_CLIENTEDGE,
							WC_TREEVIEW,
							_T("TreePropetyPage"),
							WS_CHILD | WS_VISIBLE | WS_TABSTOP | m_dwTreeStyles,
							0, 0,
							0, 0,
							hParent,
							(HMENU)ID_TREEVIEW,
							m_hInst,
							NULL);
	if(!m_hTreeView){
		MessageBox(hParent, _T("ﾂﾘｰﾋﾞｭｰの作成に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	TreeView_SetImageList(m_hTreeView, m_hImgTreeArrow, TVSIL_NORMAL);
	TreeView_SetItemHeight(m_hTreeView, m_nTreeItemHeight);
	// サブタイトル用スタティック
	m_hSubTitle = CreateWindowEx(0,
							_T("STATIC"),
							NULL,
							WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_OWNERDRAW,
							0, 0,
							0, 0,
							hParent,
							(HMENU)IDC_STATIC,
							m_hInst,
							NULL);
	if(!m_hSubTitle){
		MessageBox(hParent, _T("ｻﾌﾞﾀｲﾄﾙ用ｽﾀﾃｨｯｸの作成に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	// ボタン作成
	m_hBtnOK = CreateWindowEx(0,
							_T("BUTTON"),
							OK_BUTTON_CAPTION,
							WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
							0, 0,
							0, 0,
							hParent,
							(HMENU)IDOK,
							m_hInst,
							NULL);
	if(!m_hBtnOK){
		MessageBox(hParent, _T("OKﾎﾞﾀﾝの作成に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	m_hBtnCan = CreateWindowEx(0,
							_T("BUTTON"),
							CAN_BUTTON_CAPTION,
							WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
							0, 0,
							0, 0,
							hParent,
							(HMENU)IDCANCEL,
							m_hInst,
							NULL);
	if(!m_hBtnCan){
		MessageBox(hParent, _T("ｷｬﾝｾﾙﾎﾞﾀﾝの作成に失敗"), _T("Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	SendMessage(m_hTreeView, WM_SETFONT, (WPARAM)m_hGUIFont, MAKELPARAM(TRUE, 0));
	SendMessage(m_hSubTitle, WM_SETFONT, (WPARAM)m_hGUIFont, MAKELPARAM(TRUE, 0));
	SendMessage(m_hBtnOK, WM_SETFONT, (WPARAM)m_hGUIFont, MAKELPARAM(TRUE, 0));
	SendMessage(m_hBtnCan, WM_SETFONT, (WPARAM)m_hGUIFont, MAKELPARAM(TRUE, 0));

	return TRUE;
}

/*------------------------------------------------------------------------------
	プロパティシートのフォーカス移動
------------------------------------------------------------------------------*/
HWND CTreeProp::SetNextFocus(BOOL bPrevious)
{
	HWND hNow = GetFocus(), hPage = GetHandle(TreeView_GetSelection(m_hTreeView));
	if(hNow == m_hTreeView){
		SetFocus(hPage);
		if(bPrevious) SetFocus(GetNextDlgTabItem(hPage, GetFocus(), bPrevious));
	}else{
		SetFocus(GetNextDlgTabItem(hPage, hNow, bPrevious));
	}
	return (GetFocus());
}

/*------------------------------------------------------------------------------
	ツリーとページのフォーカス移動
------------------------------------------------------------------------------*/
HWND CTreeProp::SetNextWndFocus()
{
	HWND hNow = GetFocus();
	if(hNow != m_hTreeView){
		SetNextFocus(FALSE);
	}else{
		SetFocus(m_hTreeView);
	}
	return (GetFocus());
}

/*------------------------------------------------------------------------------
	ノードが選択されているか否か
------------------------------------------------------------------------------*/
BOOL CTreeProp::IsSelectedNode(HTREEITEM hItem)
{
	return (BOOL)(hItem == TreeView_GetSelection(m_hTreeView));
}

/*------------------------------------------------------------------------------
	更新（ＯＫボタン押下）を各ページに送信
------------------------------------------------------------------------------*/
void CTreeProp::SendUpdate()
{
	if(m_hTreeView == NULL) return;
	m_bOK = TRUE;
	HTREEITEM hRoot = TreeView_GetRoot(m_hTreeView);
	SendUpdate(hRoot);
}

void CTreeProp::SendUpdate(HTREEITEM hItem)
{
	if(hItem == NULL) return;

	HWND hPage = GetHandle(hItem);
	if(hPage){
		LPVOID lpParam = GetNodeParam(hPage);
		if(IsSelectedNode(hItem)){
			// 選択中のノードには「UM_TPM_APPLYNOW」送信後、「UM_TPM_APPLY」を送信
			//if(SendMessage(hPage, UM_TPM_APPLYNOW, (WPARAM)lpParam, m_lParam) != 0) m_bOK = FALSE;
			if(SendMessage(hPage, UM_TPM_APPLYNOW, (WPARAM)hItem, (LPARAM)lpParam) != 0) m_bOK = FALSE;
		}
		//if(SendMessage(hPage, UM_TPM_APPLY, (WPARAM)lpParam, m_lParam) != 0) m_bOK = FALSE;
		if(SendMessage(hPage, UM_TPM_APPLY, (WPARAM)hItem, (LPARAM)lpParam) != 0) m_bOK = FALSE;
	}

	HTREEITEM hItemNext;
	if((hItemNext = TreeView_GetChild(m_hTreeView, hItem))) SendUpdate(hItemNext);
	if((hItemNext = TreeView_GetNextSibling(m_hTreeView, hItem))) SendUpdate(hItemNext);
}

/*------------------------------------------------------------------------------
	ツリー選択ノードの文字列取得
------------------------------------------------------------------------------*/
INT_PTR CTreeProp::GetSelectionText(LPTSTR pszSelText, INT_PTR nSize)
{
	if(m_hTreeView == NULL) return -1;
	if(nSize < 1) return -1;

	HTREEITEM hItem = TreeView_GetSelection(m_hTreeView);
	if(!hItem) return -1;

	TVITEM tvItem;
	ZeroMemory(&tvItem, sizeof(TVITEM));
	tvItem.mask = TVIF_HANDLE | TVIF_TEXT;
	tvItem.hItem = hItem;
	tvItem.pszText = pszSelText;
	tvItem.cchTextMax = nSize;
	TreeView_GetItem(m_hTreeView, &tvItem);
	return ((INT_PTR)_tcslen(pszSelText));
}

/*------------------------------------------------------------------------------
	ツリーのノード検索
------------------------------------------------------------------------------*/
HTREEITEM CTreeProp::FindNode(HTREEITEM hItem, HWND hDlg)
{
	if((HWND)GetHandle(hItem) == hDlg) return hItem;

	HTREEITEM hItemNext, hRetItem = NULL;
	if((hItemNext = TreeView_GetChild(m_hTreeView, hItem))){
		hRetItem = FindNode(hItemNext, hDlg);
	}
	if(hRetItem == NULL){
		if((hItemNext = TreeView_GetNextSibling(m_hTreeView, hItem))){
			hRetItem = FindNode(hItemNext, hDlg);
		}
	}
	return hRetItem;
}

/*------------------------------------------------------------------------------
	ノードに関連付けられたページのウィンドウハンドル取得
------------------------------------------------------------------------------*/
HWND CTreeProp::GetHandle(HTREEITEM hItem)
{
	if(hItem == NULL) return NULL;

	TVITEM tvItem;
	ZeroMemory(&tvItem, sizeof(TVITEM));
	tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
	tvItem.hItem = hItem;
	TreeView_GetItem(m_hTreeView, &tvItem);
	LPNODEPARAM lpNp = (LPNODEPARAM)tvItem.lParam;
	if(lpNp) return (lpNp->hPage);
	return NULL;
}

/*------------------------------------------------------------------------------
	ページタイトル描画
------------------------------------------------------------------------------*/
void CTreeProp::DrawSubTitle(LPDRAWITEMSTRUCT lpdis)
{
	TCHAR szSubTitle[260];
	GetSelectionText(szSubTitle, sizeof(szSubTitle));

	FillRect(lpdis->hDC, &(lpdis->rcItem), GetSysColorBrush(COLOR_BTNFACE));
	HFONT hFont = CreateFont(-MulDiv((INT_PTR)9, GetDeviceCaps(lpdis->hDC, LOGPIXELSY), 72),
		0, 0, 0, FW_BOLD,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		CTP_GUI_FONT);
	HFONT hOldFont = (HFONT)SelectObject(lpdis->hDC, hFont);
	DRAWTEXTPARAMS dtp;
	RECT rc;
	CopyMemory(&rc, &(lpdis->rcItem), sizeof(RECT));
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iLeftMargin = 6;
	dtp.iRightMargin = 6;
	dtp.iTabLength = 4;
	DrawTextEx(lpdis->hDC,
			szSubTitle, -1,
			&rc,
			DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_CALCRECT,
			&dtp);
	DrawTextEx(lpdis->hDC,
			szSubTitle, -1,
			&rc,
			DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS,
			&dtp);
	SetRect(&rc,
		(rc.right), ((lpdis->rcItem.bottom - lpdis->rcItem.top) / 2) - 2,
		(lpdis->rcItem.right - GetSystemMetrics(SM_CYEDGE)), lpdis->rcItem.bottom);
	DrawEdge(lpdis->hDC, &rc, EDGE_ETCHED, BF_TOP);

	SelectObject(lpdis->hDC, hOldFont);
	DeleteObject(hFont);
}

/*------------------------------------------------------------------------------
	ページ描画
------------------------------------------------------------------------------*/
void CTreeProp::Decoration(HDC hDC)
{
	RECT rc;
	SetRect(&rc,
		m_rcTree.right + (m_nXEdge * 3),
		m_rcTree.bottom + m_nYEdge,
		m_rcMain.right - (m_nXEdge * 4),
		m_rcMain.bottom - m_nYEdge * 2);
	DrawEdge(hDC, &rc, EDGE_ETCHED, BF_TOP);

	SetRect(&rc,
		8,
		m_rcTree.bottom + m_nYEdge,
		m_rcMain.right - (BTN_WIDTH * 2) - 30,
		m_rcMain.bottom - m_nYEdge - 34);

	HFONT hFont = CreateFont(-MulDiv((INT_PTR)8, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		CTP_GUI_FONT);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	INT_PTR nOldMode = SetBkMode(hDC, TRANSPARENT);
	DrawText(hDC, m_szInfoString, -1, &rc, DT_SINGLELINE | DT_BOTTOM | DT_END_ELLIPSIS);
	SetBkMode(hDC, nOldMode);
	SelectObject(hDC, hOldFont);
}

/*------------------------------------------------------------------------------
	ツリー操作処理
------------------------------------------------------------------------------*/
BOOL CTreeProp::TreeViewExpand(HTREEITEM hNode, DWORD dwCode/* = TVE_EXPAND*/)
{
	return (TreeView_Expand(m_hTreeView, hNode, dwCode));
}

/*------------------------------------------------------------------------------
	初期化処理
	引数	:	dwICC				初期化するコモンコントロールのフラグ
				hInst				アプリケーションインスタンスハンドル
				hParent				親ウィンドウハンドル（なければNULL)
				lpcWindowClass		登録するウィンドウクラス名
				lpcWindowCaption	ウィンドウタイトルバーの文字
				lpIconRes			ウィンドウタイトルバーのアイコン
				lpTreeArowRes		ツリービューに表示するアイコン
									(16*16 で２つ 最初が非選択、２つめが選択)
				nTreeArow			イメージの数
				colImgMask			イメージのマスクカラー
				nTreeWidth			ツリービューの横幅
------------------------------------------------------------------------------*/
BOOL CTreeProp::Initialize(DWORD dwICC,
						   HINSTANCE hInst,
						   HWND hParent,
						   LPCTSTR lpcWindowClass,
						   LPCTSTR lpcWindowCaption,
						   const DWORD dwFlags,
						   LPTSTR lpIconRes,
						   LPTSTR lpTreeArowRes,
						   const INT_PTR nTreeArow,
						   COLORREF colImgMask/* = RGB(255, 0, 255)*/,
						   const INT_PTR nTreeWidth/* = TREE_WIDTH*/,
						   const INT_PTR nTreeItemHeight/* = TREE_ITEM_HEIGHT*/,
						   LPARAM lParam/* = NULL*/)
{
	if(m_hWndMain) return FALSE;
	if(_tcslen(lpcWindowClass) > sizeof(m_szWndClass)) return FALSE;

	m_szInfoString[0] = _T('\0');

	// コモンコントロール初期化
	if(dwICC == 0) dwICC = ICC_TREEVIEW_CLASSES;
	initComctl(dwICC);

	// フォント
	HDC hDC = GetDC(m_hWndMain);
	m_hGUIFont = CreateFont(-MulDiv((INT_PTR)9, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		CTP_GUI_FONT);
	ReleaseDC(m_hWndMain, hDC);

	// ツリービュー用イメージリスト
	HBITMAP hBmpTree = (HBITMAP)LoadImage(hInst,
										lpTreeArowRes,
										IMAGE_BITMAP,
										0, 0,
										LR_DEFAULTCOLOR | LR_SHARED);
	if(hBmpTree){
		BITMAP bmp;
		GetObject(hBmpTree, sizeof(BITMAP), &bmp);
		m_hImgTreeArrow = ImageList_Create((bmp.bmWidth / nTreeArow), bmp.bmHeight,
			ILC_COLOR16 | ILC_MASK, nTreeArow, 0);
		ImageList_AddMasked(m_hImgTreeArrow, hBmpTree, colImgMask);
		DeleteObject(hBmpTree);
	}

	// ツリービューの横幅
	m_nTreeWidth = nTreeWidth;
	if(m_nTreeWidth < 1) m_nTreeWidth = TREE_WIDTH;

	// ツリービューのアイテムの高さ
	m_nTreeItemHeight = nTreeItemHeight;
	if(m_nTreeItemHeight < 1) m_nTreeItemHeight = TREE_ITEM_HEIGHT;

	// フラグ
	m_dwFlags = dwFlags;

	// パラメータ退避
	m_lParam = lParam;

	// メインウィンドウ作成
	return (CreateMain(hInst,
				hParent,
				(lpcWindowClass ? lpcWindowClass : DEFAULT_CLASS),
				(lpcWindowCaption ? lpcWindowCaption : DEFAULT_CAPTION),
				lpIconRes));
}

/*------------------------------------------------------------------------------
	初期化処理(PNG版)
	引数	:	dwICC				初期化するコモンコントロールのフラグ
				hInst				アプリケーションインスタンスハンドル
				hParent				親ウィンドウハンドル（なければNULL)
				lpcWindowClass		登録するウィンドウクラス名
				lpcWindowCaption	ウィンドウタイトルバーの文字
				lpIconRes			ウィンドウタイトルバーのアイコン
				lpTreeArowRes		ツリービューに表示するアイコン
									(16*16 で２つ 最初が非選択、２つめが選択)
				nTreeArow			イメージの数
				colImgMask			イメージのマスクカラー
				nTreeWidth			ツリービューの横幅
------------------------------------------------------------------------------*/
BOOL CTreeProp::Initialize(DWORD dwICC,
	HINSTANCE hInst,
	HWND hParent,
	LPCTSTR lpcWindowClass,
	LPCTSTR lpcWindowCaption,
	const DWORD dwFlags,
	LPTSTR lpIconRes,
	UINT uiTreeArowRes,
	const INT_PTR nTreeArow,
	const INT_PTR nTreeWidth/* = TREE_WIDTH*/,
	const INT_PTR nTreeItemHeight/* = TREE_ITEM_HEIGHT*/,
	LPARAM lParam/* = NULL*/)
{
	if (m_hWndMain) return FALSE;
	if (_tcslen(lpcWindowClass) > sizeof(m_szWndClass)) return FALSE;

	m_szInfoString[0] = _T('\0');

	// コモンコントロール初期化
	if (dwICC == 0) dwICC = ICC_TREEVIEW_CLASSES;
	initComctl(dwICC);

	// フォント
	HDC hDC = GetDC(m_hWndMain);
	m_hGUIFont = CreateFont(-MulDiv((INT_PTR)9, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		CTP_GUI_FONT);
	ReleaseDC(m_hWndMain, hDC);

	// ツリービュー用イメージリスト
	CImgListPng ilPng;

	// PNG → 32bit HBITMAP
	HBITMAP hBmpTree =
		ilPng.LoadPngResource(
			hInst,
			uiTreeArowRes);
	if (!hBmpTree) {
		return FALSE;
	}
	BITMAP bmp;
	GetObject(hBmpTree, sizeof(BITMAP), &bmp);

	// PNG ImageList作成
	m_hImgTreeArrow =
		ilPng.MakeImageListPng(
			bmp.bmWidth / nTreeArow,
			hBmpTree);

	// ツリービューの横幅
	m_nTreeWidth = nTreeWidth;
	if (m_nTreeWidth < 1) m_nTreeWidth = TREE_WIDTH;

	// ツリービューのアイテムの高さ
	m_nTreeItemHeight = nTreeItemHeight;
	if (m_nTreeItemHeight < 1) m_nTreeItemHeight = TREE_ITEM_HEIGHT;

	// フラグ
	m_dwFlags = dwFlags;

	// パラメータ退避
	m_lParam = lParam;

	// メインウィンドウ作成
	return (CreateMain(hInst,
		hParent,
		(lpcWindowClass ? lpcWindowClass : DEFAULT_CLASS),
		(lpcWindowCaption ? lpcWindowCaption : DEFAULT_CAPTION),
		lpIconRes));
}

/*------------------------------------------------------------------------------
	プロパティページダイアログ追加処理
	引数	:	hParentDlg			追加するページの親になるダイアログハンドル
									(ルートに追加するならNULL)
				hInst				インスタンスハンドル
				lpcTmplate			ダイアログテンプレート
				lpDlgProc			ダイアログプロシジャ
				lpcAccel			アクセラレータリソース
				lp					追加するパラメータ
------------------------------------------------------------------------------*/
HWND CTreeProp::AddPage(HWND hParentDlg,
						HINSTANCE hInst,
						LPCTSTR lpcTmplate,
						DLGPROC lpDlgProc,
						LPTSTR pszTitle/*=NULL*/,
						const INT_PTR nImage/* = 0*/,
						const INT_PTR nSelectedImage/* = 1*/,
						LPVOID lpParam/*=NULL*/)
{
	if(!m_hTreeView) return NULL;

	if(MAX_PAGE < m_nPageCount) return NULL;

	// プロパティページ用ダイアログ作成
	HWND hDlg = NULL;
	if(lpParam){
		hDlg = CreateDialogParam(hInst, lpcTmplate, m_hWndMain, (DLGPROC)lpDlgProc, (LPARAM)lpParam);	// WM_INITDIALOGを利用してlpParam送信
	}else{
		hDlg = CreateDialog(hInst, lpcTmplate, m_hWndMain, (DLGPROC)lpDlgProc);
	}
	if(!hDlg) return NULL;

//	// パラメータ退避
//	m_lParam = lp;

	// ダイアログタイトル
	TCHAR szPageTitle[MAX_PATH+1];
	if(pszTitle) SetWindowText(hDlg, pszTitle);
	GetWindowText(hDlg, szPageTitle, sizeof(szPageTitle));

	// ページスタイルの変更
	DWORD dwStyle = (DWORD)GetWindowLongPtr(hDlg, GWL_STYLE);
	if(dwStyle & WS_CAPTION) dwStyle &= ~WS_CAPTION;
	if(dwStyle & WS_DLGFRAME) dwStyle &= ~WS_DLGFRAME;
	if(dwStyle & WS_BORDER) dwStyle &= ~WS_BORDER;
	if(dwStyle & WS_THICKFRAME) dwStyle &= ~WS_THICKFRAME;
	if(dwStyle & WS_POPUP) dwStyle &= ~WS_POPUP;
	if(dwStyle & WS_VISIBLE) dwStyle &= ~WS_VISIBLE;
	dwStyle |= WS_CHILDWINDOW | WS_TABSTOP;
	SetWindowLongPtr(hDlg, GWL_STYLE, (LONG)dwStyle);

	DWORD dwExStyle = (DWORD)GetWindowLongPtr(hDlg, GWL_EXSTYLE);
	if(dwExStyle & WS_EX_DLGMODALFRAME) dwExStyle &= ~WS_EX_DLGMODALFRAME;
	SetWindowLongPtr(hDlg, GWL_EXSTYLE, (LONG)dwExStyle);

	INT_PTR nCaption = GetSystemMetrics(SM_CYCAPTION);

	// タイトルバーの分サイズを小さく
	RECT rc;
	GetWindowRect(hDlg, &rc);
	INT_PTR nDlgW = rc.right - rc.left, nDlgH = rc.bottom - rc.top - nCaption;
	MoveWindow(hDlg, 0, 0, nDlgW, nDlgH, TRUE);

	// メインのサイズを決定
	if(m_rcPage.right < nDlgW) m_rcPage.right = nDlgW;
	if(m_rcPage.bottom < nDlgH) m_rcPage.bottom = nDlgH;
/*
	// TVINSERTSTRUCT構造体設定
	TVINSERTSTRUCT tvInst;
	ZeroMemory(&tvInst, sizeof(TVINSERTSTRUCT));
	tvInst.hInsertAfter = TVI_LAST;
	if(hParentDlg){
		// 親となるノードを検索
		HTREEITEM hRoot = TreeView_GetRoot(m_hTreeView);
		if(!hRoot) return NULL;
		if(!(tvInst.hParent = FindNode(hRoot, hParentDlg))) return NULL;
	}else{
		// ルートに追加
		tvInst.hParent = TVI_ROOT;
	}
	tvInst.item.mask = TVIF_TEXT | (m_hImgTreeArrow ? TVIF_IMAGE | TVIF_SELECTEDIMAGE : 0) | TVIF_PARAM;
	tvInst.item.pszText = szPageTitle;
	tvInst.item.iImage = (m_hImgTreeArrow ? nImage : 0);
	tvInst.item.iSelectedImage = (m_hImgTreeArrow ? nSelectedImage : 0);
	tvInst.item.lParam = (LPARAM)hDlg;
	TreeView_InsertItem(m_hTreeView, &tvInst);
*/
	// ツリーノード追加
	if(!AddTreeNode(hParentDlg, hDlg, szPageTitle, nImage, nSelectedImage, lpParam)) return NULL;

	// ページ位置
	SetWindowPos(hDlg,
			HWND_TOP,
			m_nTreeWidth + (m_nXEdge * 2),
			m_nYEdge + SUBTITLE_HEIGHT,
			0, 0,
			SWP_NOSIZE);

	// ペ－ジ数
	m_nPageCount++;

	// 親の再描画
	InvalidateRect(m_hWndMain, NULL, TRUE);

	return hDlg;
}

/*------------------------------------------------------------------------------
	プロパティページダイアログ追加処理(ダイアログ無し)
------------------------------------------------------------------------------*/
BOOL CTreeProp::AddPage(HWND hParentDlg,
						LPTSTR pszTitle,
						const INT_PTR nImage/* = 0*/,
						const INT_PTR nSelectedImage/* = 1*/,
						LPVOID lpParam/* = NULL*/)
{
	if(!m_hTreeView) return NULL;

	if(MAX_PAGE < m_nPageCount) return NULL;

	if(!AddTreeNode(hParentDlg, NULL, pszTitle, nImage, nSelectedImage, lpParam)) return FALSE;

	// ペ－ジ数
	m_nPageCount++;
	return TRUE;
}

/*------------------------------------------------------------------------------
	ツリービューノード追加
------------------------------------------------------------------------------*/
BOOL CTreeProp::AddTreeNode(HWND hParentDlg,
							HWND hDlg,
							LPSTR lpszPageTitle,
							const INT_PTR nImage/* = 0*/,
							const INT_PTR nSelectedImage/* = 1*/,
							LPVOID lpParam/* = NULL*/)
{
	// パラメータ(TVN_DELETEITEMにて開放)
	LPNODEPARAM lpNp = (LPNODEPARAM)GlobalAlloc(GPTR, sizeof(NODEPARAM));
	if(!lpNp) return FALSE;
	lpNp->hPage = hDlg;
	lpNp->lpParam = lpParam;
	// TVINSERTSTRUCT構造体設定
	TVINSERTSTRUCT tvInst;
	ZeroMemory(&tvInst, sizeof(TVINSERTSTRUCT));
	tvInst.hInsertAfter = TVI_LAST;
	if(hParentDlg){
		// 親となるノードを検索
		HTREEITEM hRoot = TreeView_GetRoot(m_hTreeView);
		if(!hRoot) return NULL;
		if(!(tvInst.hParent = FindNode(hRoot, hParentDlg))) return NULL;
	}else{
		// ルートに追加
		tvInst.hParent = TVI_ROOT;
	}
	tvInst.item.mask = TVIF_TEXT | (m_hImgTreeArrow ? TVIF_IMAGE | TVIF_SELECTEDIMAGE : 0) | TVIF_PARAM;
	tvInst.item.pszText = lpszPageTitle;
	tvInst.item.iImage = (m_hImgTreeArrow ? nImage : 0);
	tvInst.item.iSelectedImage = (m_hImgTreeArrow ? nSelectedImage : 0);
//	tvInst.item.lParam = (LPARAM)hDlg;
	tvInst.item.lParam = (LPARAM)lpNp;
	return (TreeView_InsertItem(m_hTreeView, &tvInst) ? TRUE : FALSE);
}

/*------------------------------------------------------------------------------
	プロパティページダイアログ削除処理
------------------------------------------------------------------------------*/
BOOL CTreeProp::DeletePage(HWND hPageDlg)
{
	if(hPageDlg == NULL) return FALSE;

	// ノードを検索
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) return FALSE;
	// ツリーノード破棄
	TreeView_DeleteItem(m_hTreeView, hNode);
	// ページダイアログ破棄
	DestroyWindow(hPageDlg);
	return TRUE;
}

/*------------------------------------------------------------------------------
	ツリー展開
------------------------------------------------------------------------------*/
BOOL CTreeProp::ExpandNode(HWND hPageDlg)
{
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) hNode = TreeView_GetRoot(m_hTreeView);
	return (TreeViewExpand(hNode));
}

/*------------------------------------------------------------------------------
	ツリー閉じる
------------------------------------------------------------------------------*/
BOOL CTreeProp::CollapseNode(HWND hPageDlg)
{
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) return FALSE;
	return (TreeViewExpand(hNode, TVE_COLLAPSE));
}

/*------------------------------------------------------------------------------
	ツリー反転
------------------------------------------------------------------------------*/
BOOL CTreeProp::ToggleNode(HWND hPageDlg)
{
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) return FALSE;
	return (TreeViewExpand(hNode, TVE_TOGGLE));
}

/*------------------------------------------------------------------------------
	ツリーノード選択
------------------------------------------------------------------------------*/
BOOL CTreeProp::SelectNode(HWND hPageDlg)
{
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) return FALSE;
	return (TreeView_Select(m_hTreeView, hNode, TVGN_CARET));
}

/*------------------------------------------------------------------------------
	プロパティシート表示処理
------------------------------------------------------------------------------*/
BOOL CTreeProp::Show(const HWND hStart/* = NULL*/)
{
	// ツリービューサイズ
	SetRect(&m_rcTree,
			0, 0,
			m_nTreeWidth,
			m_rcPage.bottom + SUBTITLE_HEIGHT);

	// メインウィンドウサイズ
	INT_PTR nFrameHeight = GetSystemMetrics(SM_CYCAPTION) + (GetSystemMetrics(SM_CYFIXEDFRAME) * 2);
	SetRect(&m_rcMain,
			0, 0,
			m_rcTree.right + m_rcPage.right + (m_nXEdge * 5),
			m_rcTree.bottom + (BTN_HEIGHT * 2) + nFrameHeight + (m_nYEdge * 2));

	// 位置、サイズ決定
	if(m_hWndParent == HWND_DESKTOP){
		MoveWindow(m_hWndMain,
				(GetSystemMetrics(SM_CXFULLSCREEN) - m_rcMain.right) / 2,
				(GetSystemMetrics(SM_CYFULLSCREEN) - m_rcMain.bottom) / 2,
				m_rcMain.right, m_rcMain.bottom,
				TRUE);
	}else{
		RECT rcParent;
		GetWindowRect(m_hWndParent, &rcParent);
		MoveWindow(m_hWndMain,
				rcParent.left + 26,
				rcParent.top + 26 + GetSystemMetrics(SM_CYCAPTION),
				m_rcMain.right, m_rcMain.bottom,
				TRUE);
	}
	MoveWindow(m_hTreeView,
			m_nXEdge,
			m_nYEdge,
			m_rcTree.right,
			m_rcTree.bottom,
			TRUE);
	MoveWindow(m_hSubTitle,
			m_rcTree.right + (m_nXEdge * 2),
			m_nYEdge,
			m_rcMain.right - m_rcTree.right - (m_nXEdge * 5),
			SUBTITLE_HEIGHT - m_nYEdge,
			TRUE);
	MoveWindow(m_hBtnOK,
			m_rcMain.right - ((BTN_WIDTH * 2) + BTN_FREE + (m_nXEdge * 2)),
			m_rcMain.bottom - ((BTN_HEIGHT / 2) + BTN_HEIGHT + nFrameHeight) - 4,
			BTN_WIDTH,
			BTN_HEIGHT,
			TRUE);
	MoveWindow(m_hBtnCan,
			m_rcMain.right - (BTN_WIDTH + BTN_FREE) - m_nXEdge,
			m_rcMain.bottom - ((BTN_HEIGHT / 2) + BTN_HEIGHT + nFrameHeight) - 4,
			BTN_WIDTH,
			BTN_HEIGHT,
			TRUE);

	if(hStart == NULL){
		TreeView_SelectItem(m_hTreeView, TreeView_GetRoot(m_hTreeView));
	}else{
		SelectNode(hStart);
	}
	SetFocus(m_hTreeView);	// ツリーを選択

	// メッセージループ
	MainMessageLoop();

	return m_bOK;
}

/*------------------------------------------------------------------------------
	ツリービューフォント設定
	(※ フォントハンドルは関数呼び元で責任を持って破棄すること)
------------------------------------------------------------------------------*/
void CTreeProp::SetTreeFont(const HFONT hFont)
{
	if(m_hTreeView == NULL) return;
	SendMessage(m_hTreeView, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

/*------------------------------------------------------------------------------
	インフォメーション文字列設定
------------------------------------------------------------------------------*/
void CTreeProp::SetInfoStr(LPCTSTR pszInfoText)
{
	if(_tcslen(pszInfoText) > MAX_INFO_STR) return;
	_tcscpy_s(m_szInfoString, sizeof(m_szInfoString), pszInfoText);
}

/*------------------------------------------------------------------------------
	ノード文字列変更
------------------------------------------------------------------------------*/
INT_PTR CTreeProp::SetNodeText(HWND hPageDlg, LPTSTR pszText)
{
	// ノードを検索
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) hNode = TreeView_GetRoot(m_hTreeView);

	TVITEM tvItem;
	ZeroMemory(&tvItem, sizeof(TVITEM));
	tvItem.mask = TVIF_HANDLE | TVIF_TEXT;
	tvItem.hItem = hNode;
	tvItem.pszText = pszText;
	return (INT_PTR)TreeView_SetItem(m_hTreeView, &tvItem);
}

/*------------------------------------------------------------------------------
	ＯＫボタン文字列設定
------------------------------------------------------------------------------*/
void CTreeProp::SetOkStr(LPCTSTR pszOkText)
{
	if(_tcslen(pszOkText) < 1) return;
	SetWindowText(m_hBtnOK, pszOkText);
}

/*------------------------------------------------------------------------------
	キャンセルボタン文字列設定
------------------------------------------------------------------------------*/
void CTreeProp::SetCancelStr(LPCTSTR pszCancelText)
{
	if(_tcslen(pszCancelText) < 1) return;
	SetWindowText(m_hBtnCan, pszCancelText);
}

/*------------------------------------------------------------------------------
	ノードに関連付けられたパラメータの取得
------------------------------------------------------------------------------*/
LPVOID CTreeProp::GetNodeParam(HWND hPageDlg)
{
	// ノードを検索
	HTREEITEM hNode = FindNode(TreeView_GetRoot(m_hTreeView), hPageDlg);
	if(hNode == NULL) return NULL;

	TVITEM tvItem;
	ZeroMemory(&tvItem, sizeof(TVITEM));
	tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
	tvItem.hItem = hNode;
	TreeView_GetItem(m_hTreeView, &tvItem);
	LPNODEPARAM lpNp = (LPNODEPARAM)tvItem.lParam;
	if(lpNp) return (LPVOID)(lpNp->lpParam);
	return NULL;
}

/*------------------------------------------------------------------------------
	ツリービューインデント設定
------------------------------------------------------------------------------*/
void CTreeProp::SetTreeIndent(const INT_PTR nIndent)
{
	if(m_hTreeView == NULL) return;
	if(nIndent < 0){
		TreeView_SetIndent(m_hTreeView, TREE_INDENT);
	}else{
		TreeView_SetIndent(m_hTreeView, nIndent);
	}
}

/*------------------------------------------------------------------------------
	ツリービュー文字色設定
------------------------------------------------------------------------------*/
void CTreeProp::SetTextColor(const COLORREF colText)
{
	if(m_hTreeView == NULL) return;
	TreeView_SetTextColor(m_hTreeView, colText);
}

/*------------------------------------------------------------------------------
	ツリービュー背景色設定
------------------------------------------------------------------------------*/
void CTreeProp::SetBkColor(const COLORREF colBk)
{
	if(m_hTreeView == NULL) return;
	TreeView_SetBkColor(m_hTreeView, colBk);
}

/*------------------------------------------------------------------------------
	ツリービュー文字・背景色設定
------------------------------------------------------------------------------*/
void CTreeProp::SetColor(const COLORREF colText, const COLORREF colBk)
{
	SetTextColor(colText);
	SetBkColor(colBk);
}
