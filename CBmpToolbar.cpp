/*------------------------------------------------------------------------
	CBmpToolbar.h
------------------------------------------------------------------------*/
#include "CBmpToolbar.h"

CBmpToolbar::CBmpToolbar(HINSTANCE hCurInst,
						 HWND hWnd,
						 LPCTSTR lpcsCaption)
 : m_hInst(hCurInst), m_hParent(hWnd), 
   m_nBtn(0), m_uID(0)
{
	ZeroMemory(&m_rc, sizeof(RECT));
	ZeroMemory(&m_SizeBtn, sizeof(SIZE));
	lstrcpy(m_szCaption, lpcsCaption);
	m_hTooltips = m_hToolbar = NULL;
}

CBmpToolbar::~CBmpToolbar()
{
	if(m_hTooltips) DestroyWindow(m_hTooltips);
	HIMAGELIST hImgTmp;
	hImgTmp = (HIMAGELIST)SendMessage(m_hToolbar, TB_GETIMAGELIST, 0L, 0L);
	if(hImgTmp) ImageList_Destroy(hImgTmp);
	hImgTmp = (HIMAGELIST)SendMessage(m_hToolbar, TB_GETDISABLEDIMAGELIST, 0L, 0L);
	if(hImgTmp) ImageList_Destroy(hImgTmp);
	hImgTmp = (HIMAGELIST)SendMessage(m_hToolbar, TB_GETHOTIMAGELIST, 0L, 0L);
	if(hImgTmp) ImageList_Destroy(hImgTmp);
	if(m_hToolbar) DestroyWindow(m_hToolbar);
}

void CBmpToolbar::InitWin32Commctrl(DWORD dwICC/* = ICC_BAR_CLASSES*/)
{
	INITCOMMONCONTROLSEX icex;
	InitCommonControls();
	ZeroMemory(&icex, sizeof(INITCOMMONCONTROLSEX));
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = dwICC;
	InitCommonControlsEx(&icex);
}

void CBmpToolbar::MakeToolbar(UINT uiId, DWORD dwAddStyle)
{
	if(m_hToolbar) return;
	// ツールバー作成
	m_hToolbar = CreateWindowEx(0,
		TOOLBARCLASSNAME,
		m_szCaption,
		TOOLBAR_STYLE | dwAddStyle,
		0, 0,
		0, 0,
		m_hParent,
		(HMENU)uiId,
		m_hInst,
		NULL);
	if(!m_hToolbar) return;
	m_uID = uiId;
	SendMessage(m_hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0L);
}

BOOL CBmpToolbar::MakeTooltip(BOOL bUseBalloon)
{
//	// Windowsのバージョン情報取得
//	OSVERSIONINFO ovi;
//	ZeroMemory(&ovi, sizeof(OSVERSIONINFO));
//	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	GetVersionEx(&ovi);
//	if(!bUseBalloon) ovi.dwMajorVersion = 0;
	// ツールチップ作成
	m_hTooltips = CreateWindowEx(0,
		TOOLTIPS_CLASS,
		NULL,
//		TTS_ALWAYSTIP | (ovi.dwMajorVersion < 5 ? 0 : TTS_BALLOON),
		TTS_ALWAYSTIP | (bUseBalloon ? TTS_BALLOON : 0),
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_hParent,
		NULL,
		m_hInst,
		NULL);
	if(!m_hTooltips) return FALSE;
	return TRUE;
}

void CBmpToolbar::SetImageList(int nWidth, WORD wBmp, WORD wMask, const int nImgFlg/* = TB_IMG_ENABLE */)
{
	if(!m_hToolbar) return;

	HBITMAP hBmp, hMask;
	// ビットマップロード
	hBmp = (HBITMAP)LoadImage(m_hInst,
		MAKEINTRESOURCE(wBmp),
		IMAGE_BITMAP,
		0, 0, LR_DEFAULTCOLOR | LR_SHARED);
	hMask = (HBITMAP)LoadImage(m_hInst,
		MAKEINTRESOURCE(wMask),
		IMAGE_BITMAP,
		0, 0, LR_DEFAULTCOLOR | LR_SHARED);
	// イメージリスト作成
	HIMAGELIST hImgList = MakeImageList(nWidth, hBmp, hMask);
	// ツールバーに設定
	if(hImgList) SetToolbarImage(hImgList, nImgFlg);
	if(hBmp) DeleteObject(hBmp);
	if(hMask) DeleteObject(hMask);
}

void CBmpToolbar::SetImageList(int nWidth, HINSTANCE hResInst, WORD wBmp, WORD wMask, const int nImgFlg/* = TB_IMG_ENABLE */)
{
	if(!m_hToolbar) return;

	HBITMAP hBmp, hMask;
	// ビットマップロード
	hBmp = (HBITMAP)LoadImage(hResInst,
		MAKEINTRESOURCE(wBmp),
		IMAGE_BITMAP,
		0, 0, LR_DEFAULTCOLOR | LR_SHARED);
	hMask = (HBITMAP)LoadImage(hResInst,
		MAKEINTRESOURCE(wMask),
		IMAGE_BITMAP,
		0, 0, LR_DEFAULTCOLOR | LR_SHARED);
	// イメージリスト作成
	HIMAGELIST hImgList = MakeImageList(nWidth, hBmp, hMask);
	// ツールバーに設定
	if(hImgList) SetToolbarImage(hImgList, nImgFlg);
	if(hBmp) DeleteObject(hBmp);
	if(hMask) DeleteObject(hMask);
}

void CBmpToolbar::SetImageList(int nWidth, LPCTSTR lpcBmp, LPCTSTR lpcMask, const int nImgFlg/* = TB_IMG_ENABLE */)
{
	if(!m_hToolbar) return;

	HBITMAP hBmp, hMask;
	// ビットマップロード
	hBmp = (HBITMAP)LoadImage(0,
		lpcBmp,
		IMAGE_BITMAP,
		0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_SHARED);
	hMask = (HBITMAP)LoadImage(0,
		lpcMask,
		IMAGE_BITMAP,
		0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_SHARED);
	// イメージリスト作成
	HIMAGELIST hImgList = MakeImageList(nWidth, hBmp, hMask);
	// ツールバーに設定
	if(hImgList) SetToolbarImage(hImgList, nImgFlg);
	if(hBmp) DeleteObject(hBmp);
	if(hMask) DeleteObject(hMask);
}

HIMAGELIST CBmpToolbar::MakeImageList(int nWidth, HBITMAP hBmp, HBITMAP hMask)
{
	BITMAP bmp;
	GetObject((HBITMAP)hBmp, sizeof(BITMAP), &bmp);
	m_nBtn = bmp.bmWidth / nWidth;
	m_SizeBtn.cx = nWidth;
	m_SizeBtn.cy = bmp.bmHeight;
	// イメージリスト作成
	HIMAGELIST hImgList = ImageList_Create(m_SizeBtn.cx, m_SizeBtn.cy,
							ILC_COLOR16 | ILC_MASK,
							m_nBtn, 0);
	if(!hImgList) return NULL;
	ImageList_Add(hImgList, hBmp, hMask);
	return hImgList;
}

BOOL CBmpToolbar::SetToolbarImage(HIMAGELIST hImgLst, const int nImgFlg)
{
	HIMAGELIST hOldImgList = NULL;
	switch(nImgFlg){
	case TB_IMG_ENABLE:
		hOldImgList = (HIMAGELIST)SendMessage(m_hToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImgLst);
		break;
	case TB_IMG_DISABLE:
		hOldImgList = (HIMAGELIST)SendMessage(m_hToolbar, TB_SETDISABLEDIMAGELIST, (WPARAM)0, (LPARAM)hImgLst);
		break;
	case TB_IMG_HOT:
		hOldImgList = (HIMAGELIST)SendMessage(m_hToolbar, TB_SETHOTIMAGELIST, (WPARAM)0, (LPARAM)hImgLst);
		break;
	default:
		return FALSE;
	}
	if(hOldImgList) ImageList_Destroy(hOldImgList);
	return TRUE;
}

int CBmpToolbar::AddBtn(LPTBBUTTON lpBtnInfo, const int nBtn/* = 0*/)
{
	if(!lpBtnInfo) return -1;
	m_nBtn = max(nBtn, m_nBtn);
	if(m_nBtn < 1) return -1;
	m_lpTbBtn = (LPTBBUTTON)GlobalAlloc(GPTR, (DWORD)(sizeof(TBBUTTON) * m_nBtn));
	CopyMemory(m_lpTbBtn, lpBtnInfo, (DWORD)(sizeof(TBBUTTON) * m_nBtn));
	// 拡張スタイル
	DWORD dwExStyles = 0;
	for(int i = 0; i < m_nBtn; i++){
		if((lpBtnInfo + i)->fsStyle & TBSTYLE_DROPDOWN) dwExStyles |= TBSTYLE_EX_DRAWDDARROWS;
		if((lpBtnInfo + i)->fsStyle & BTNS_SHOWTEXT) dwExStyles |= TBSTYLE_EX_MIXEDBUTTONS;
	}
	SendMessage(m_hToolbar, TB_SETEXTENDEDSTYLE, 0L, (LPARAM)dwExStyles);
	// ボタン追加
	SendMessage(m_hToolbar, TB_ADDBUTTONS, (WPARAM)m_nBtn, (LPARAM)m_lpTbBtn);
	// TB_AUTOSIZE送信
	SendMessage(m_hToolbar, TB_AUTOSIZE, 0L, 0L);
	GetClientRect(m_hToolbar, &m_rc);
	return 0;	
}

void CBmpToolbar::InsSep(int nIdx)
{
	TBBUTTON tbSep =
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0};
	SendMessage(m_hToolbar,
		TB_INSERTBUTTON,
		(WPARAM)nIdx,
		(LPARAM)&tbSep);
}

void CBmpToolbar::SetIndent(int nIndent)
{
	SendMessage(m_hToolbar,
		TB_SETINDENT,
		(WPARAM)nIndent,
		0L);
}

int CBmpToolbar::AddString(UINT uiStringRes)
{
	if(!m_hToolbar) return -1;
	char szString[MAX_TB_STRING];
	LoadString(m_hInst, uiStringRes, szString, sizeof(szString));
	return (int)SendMessage(m_hToolbar, TB_ADDSTRING, 0L, (LPARAM)szString);
}

int CBmpToolbar::AddString(LPCTSTR lpcString)
{
	if(!m_hToolbar) return -1;
	return (int)SendMessage(m_hToolbar, TB_ADDSTRING, 0L, (LPARAM)lpcString);
}

void CBmpToolbar::SetString(int nId, UINT uiStringRes)
{
	if(!m_hToolbar) return;
	char szString[MAX_TB_STRING];
	LoadString(m_hInst, uiStringRes, szString, sizeof(szString));
	TBBUTTONINFO tbi;
	ZeroMemory(&tbi, sizeof(TBBUTTONINFO));
	tbi.cbSize = sizeof(TBBUTTONINFO);
	tbi.dwMask = TBIF_TEXT | TBIF_STYLE;
	tbi.fsStyle = BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE;
	tbi.pszText = szString;
	tbi.cchText = strlen(szString);
	SendMessage(m_hToolbar, TB_SETBUTTONINFO, (WPARAM)nId, (LPARAM)&tbi);
	// TB_AUTOSIZE送信
	SendMessage(m_hToolbar, TB_AUTOSIZE, 0L, 0L);
}

void CBmpToolbar::SetString(
	int nId,
	char* pszString)
{
	if (!m_hToolbar)
		return;

	TBBUTTONINFO tbi;
	ZeroMemory(
		&tbi,
		sizeof(tbi));

	tbi.cbSize =
		sizeof(TBBUTTONINFO);

	// まず現在のスタイルを取得
	tbi.dwMask =
		TBIF_STYLE;

	if (!SendMessage(
		m_hToolbar,
		TB_GETBUTTONINFO,
		(WPARAM)nId,
		(LPARAM)&tbi))
	{
		return;
	}

	tbi.dwMask =
		TBIF_TEXT |
		TBIF_STYLE;

	// 現在のスタイルを残したまま追加
	tbi.fsStyle |=
		BTNS_SHOWTEXT |
		TBSTYLE_AUTOSIZE;

	tbi.pszText =
		pszString;

	tbi.cchText =
		(int)strlen(
			pszString);

	SendMessage(
		m_hToolbar,
		TB_SETBUTTONINFO,
		(WPARAM)nId,
		(LPARAM)&tbi);

	SendMessage(
		m_hToolbar,
		TB_AUTOSIZE,
		0,
		0);
}

void CBmpToolbar::SetBtnWidth(int nId, int nWidth)
{
	if(!m_hToolbar) return;
	TBBUTTONINFO tbi;
	ZeroMemory(&tbi, sizeof(TBBUTTONINFO));
	tbi.cbSize = sizeof(TBBUTTONINFO);
	tbi.dwMask = TBIF_SIZE;
	tbi.cx = nWidth;
	SendMessage(m_hToolbar, TB_SETBUTTONINFO, (WPARAM)nId, (LPARAM)&tbi);
}

void CBmpToolbar::EnableButton(UINT uiId, BOOL bEnable)
{
	DWORD dwState = (DWORD)SendMessage(m_hToolbar, TB_GETSTATE, (WPARAM)uiId, 0L);
	if(bEnable){
		if(dwState & TBSTATE_INDETERMINATE) dwState &= ~TBSTATE_INDETERMINATE;
		dwState |= TBSTATE_ENABLED;
	}else{
		if(dwState & TBSTATE_ENABLED) dwState &= ~TBSTATE_ENABLED;
		dwState |= TBSTATE_INDETERMINATE;
	}
	SendMessage(m_hToolbar, TB_SETSTATE, (WPARAM)uiId, (LPARAM)dwState);
}

void CBmpToolbar::CheckedButton(UINT uiId, UINT uiChecked)
{
	DWORD dwState = (DWORD)SendMessage(m_hToolbar, TB_GETSTATE, (WPARAM)uiId, 0L);
	if(uiChecked == BST_CHECKED){
		dwState |= TBSTATE_CHECKED;
	}else{
		dwState &= ~TBSTATE_CHECKED;
	}
	SendMessage(m_hToolbar, TB_SETSTATE, (WPARAM)uiId, (LPARAM)dwState);
}

void CBmpToolbar::PressedButton(UINT uiId, BOOL bPressed)
{
	DWORD dwState = (DWORD)SendMessage(m_hToolbar, TB_GETSTATE, (WPARAM)uiId, 0L);
	if(bPressed){
		dwState |= TBSTATE_PRESSED;
	}else{
		dwState &= ~TBSTATE_PRESSED;
	}
	SendMessage(m_hToolbar, TB_SETSTATE, (WPARAM)uiId, (LPARAM)dwState);
}

DWORD CBmpToolbar::IsCheck(UINT uiId)
{
	DWORD dwState = (DWORD)SendMessage(m_hToolbar,
		TB_GETSTATE,
		(WPARAM)uiId, 0L);
	if(dwState & TBSTATE_CHECKED){
		return BST_CHECKED;
	}else{
		return BST_UNCHECKED;
	}
}

DWORD CBmpToolbar::IsPressed(UINT uiId)
{
	DWORD dwState = (DWORD)SendMessage(m_hToolbar,
		TB_GETSTATE,
		(WPARAM)uiId, 0L);
	if(dwState & TBSTATE_PRESSED){
		return BST_CHECKED;
	}else{
		return BST_UNCHECKED;
	}
}

void CBmpToolbar::SetTooltips(const char *pszTipText[], BOOL bUseBalloon/* = FALSE*/)
{
	if(!m_hToolbar) return;
	if(m_hTooltips) DestroyWindow(m_hTooltips);
	if(!MakeTooltip(bUseBalloon)) return;

	TOOLINFO ti;
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = m_hToolbar;
	ti.hinst = m_hInst;
	ti.uFlags = TTF_SUBCLASS;
	for(int i = 0; i < m_nBtn; i++){
		SendMessage(m_hToolbar, TB_GETITEMRECT, i, (LPARAM)&ti.rect);
		ti.uId = (m_lpTbBtn + i)->idCommand;
		ti.lpszText = (LPTSTR)pszTipText[i];
		SendMessage(m_hTooltips, TTM_ADDTOOL, 0L, (LPARAM)&ti);
	}
	SendMessage(m_hToolbar, TB_SETTOOLTIPS, (WPARAM)m_hTooltips, 0);
}

void CBmpToolbar::SetTooltips(UINT uiID, const char *pszTipText, BOOL bUseBalloon/* = FALSE*/)
{
	if(!m_hToolbar) return;
	if(!m_hTooltips){
		if(!MakeTooltip(bUseBalloon)) return;
	}

	TOOLINFO ti;
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = m_hToolbar;
	ti.hinst = m_hInst;
	ti.uFlags = TTF_SUBCLASS;
	for(int i = 0; i < m_nBtn; i++){
		if((UINT)(m_lpTbBtn + i)->idCommand == uiID){
			SendMessage(m_hToolbar, TB_GETITEMRECT, i, (LPARAM)&ti.rect);
			ti.uId = (m_lpTbBtn + i)->idCommand;
			ti.lpszText = (LPTSTR)pszTipText;
			SendMessage(m_hTooltips, TTM_SETTOOLINFO, 0L, (LPARAM)&ti);
		}
	}
	SendMessage(m_hToolbar, TB_SETTOOLTIPS, (WPARAM)m_hTooltips, 0);
}

LRESULT CBmpToolbar::Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	SendMessage(m_hToolbar, WM_SIZE, wp, lp);
	GetClientRect(m_hToolbar, &m_rc);
	return (0L);
}

int CBmpToolbar::GetBtnHeight(int nIdx)
{
	RECT rc;
	SendMessage(m_hToolbar, TB_GETITEMRECT, (WPARAM)nIdx, (LPARAM)&rc);
	return (rc.bottom - rc.top);
}

int CBmpToolbar::GetBtnWidth(int nIdx)
{
	RECT rc;
	SendMessage(m_hToolbar, TB_GETITEMRECT, (WPARAM)nIdx, (LPARAM)&rc);
	return (rc.right - rc.left);
}

void CBmpToolbar::GetWndSize(LPSIZE lpSize)
{
	lpSize->cx = 0;
	lpSize->cy = 0;
	int n = GetBtnCount();
	for(int i = 0; i < n; i++) lpSize->cx += GetBtnWidth(i);
	lpSize->cy += GetBtnHeight(0);
}

HIMAGELIST CBmpToolbar::MakeImageList32(int nWidth, HBITMAP hBmp32)
{
    if (!hBmp32)
        return NULL;

    BITMAP bmp;
    ZeroMemory(&bmp, sizeof(bmp));

    GetObject(
        hBmp32,
        sizeof(BITMAP),
        &bmp);

    m_nBtn =
        bmp.bmWidth / nWidth;

    m_SizeBtn.cx =
        nWidth;

    m_SizeBtn.cy =
        bmp.bmHeight;

    HIMAGELIST hImgList =
        ImageList_Create(
            m_SizeBtn.cx,
            m_SizeBtn.cy,
            ILC_COLOR32,
            m_nBtn,
            0);

    if (!hImgList)
        return NULL;

    if (ImageList_Add(
        hImgList,
        hBmp32,
        NULL) < 0)
    {
        ImageList_Destroy(
            hImgList);

        return NULL;
    }

    return hImgList;
}

void CBmpToolbar::SetPngImageList(
	int nWidth,
	UINT uiPngRes,
	const int nImgFlg)
{
	if (!m_hToolbar) {
		return;
	}

	CImgListPng ilPng;

	//--------------------------------------------------
	// PNG → 32bit HBITMAP
	//--------------------------------------------------
	HBITMAP hBmp =
		ilPng.LoadPngResource(
			m_hInst,
			uiPngRes);

	if (!hBmp) {
		return;
	}

	//--------------------------------------------------
	// PNG ImageList作成
	//--------------------------------------------------
	HIMAGELIST hImgList =
		ilPng.MakeImageListPng(
			nWidth,
			hBmp);

	m_nBtn = ilPng.GetBtnCount();
	m_SizeBtn.cx = ilPng.GetBtnWith();
	m_SizeBtn.cy = ilPng.GetBtnHeight();

	//--------------------------------------------------
	// Toolbarへ設定
	//--------------------------------------------------
	if (hImgList) {

		SetToolbarImage(
			hImgList,
			nImgFlg);
	}

	DeleteObject(
		hBmp);
}
