/*------------------------------------------------------------------------
	CProgStbar.cpp
------------------------------------------------------------------------*/
#include "CProgStbar.h"

// コンストラクタ
CProgStbar::CProgStbar(HINSTANCE hCurInst,
					   HWND hWnd,
					   UINT uiStID,
					   UINT uiProgID,
					   LPCTSTR lpcsCaption,
					   const int *pnParts, const int nSize,
					   const int nProgPos)
 : m_hInst(hCurInst),
   m_hParent(hWnd),
   m_uiSt(uiStID),
   m_uiProg(uiProgID),
   m_nIconWidth(0),
   m_hSb(NULL),
   m_hProg(NULL),
   m_hImgSt(NULL),
   m_OrgWndProc(NULL)
{
	ZeroMemory(m_nPartsSize, sizeof(m_nPartsSize));
	m_nParts = nSize;
	for(int i = 0; i < m_nParts && i < MAX_PB_PARTS; i++){
		m_nPartsSize[i] = *(pnParts + i);
		m_nPartsStyle[i] = 0;
	}
	m_nProgressPos = nProgPos;
	ZeroMemory(m_nFixedPartsSize, sizeof(m_nFixedPartsSize));
	ZeroMemory(&m_Rc, sizeof(RECT));
	m_hSb = m_hProg = NULL;
	lstrcpy(m_szCaption, lpcsCaption);
#if 0
	MessageBox(m_hParent, "CProgStbar コンストラクタ", "DEBUG", MB_OK);
#endif
}

// デストラクタ
CProgStbar::~CProgStbar()
{
	for(int i = 0; i < m_nParts; i++){
		HICON hIcon = (HICON)SendMessage(m_hSb, SB_GETICON, (WPARAM)i, 0L);
		if(hIcon) DestroyIcon(hIcon);
	}

//	if(m_OrgWndProc) SetWindowLong(m_hSb, GWL_WNDPROC, (LONG)m_OrgWndProc);
	if(m_OrgWndProc) SetWindowLongPtr(m_hSb, GWLP_WNDPROC, (LONG_PTR)m_OrgWndProc);
	if(GetProp(m_hSb, STR_PROGSTBARCLASS)) RemoveProp(m_hSb, STR_PROGSTBARCLASS);

	if(m_hProg) DestroyWindow(m_hProg);
	if(m_hSb) DestroyWindow(m_hSb);
	if(m_hImgSt) ImageList_Destroy(m_hImgSt);

#if 0
	MessageBox(m_hParent, "CProgStbar デストラクタ", "DEBUG", MB_OK);
#endif
}

void CProgStbar::InitWin32Commctrl(DWORD dwICC/* = ICC_BAR_CLASSES | ICC_PROGRESS_CLASS*/)
{
	INITCOMMONCONTROLSEX icex;
	InitCommonControls();
	ZeroMemory(&icex, sizeof(INITCOMMONCONTROLSEX));
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = dwICC;
	InitCommonControlsEx(&icex);
}

BOOL CProgStbar::CreateBar(BOOL bProcChain/* = FALSE*/)
{
	DWORD dwParentStyle = GetWindowLong(m_hParent, GWL_STYLE);
	m_hSb = CreateWindowEx(0,
		STATUSCLASSNAME, 
		(LPCTSTR)m_szCaption,
		STATUSBAR_STYLE | (dwParentStyle & WS_THICKFRAME ? SBARS_SIZEGRIP : 0),
		0, 0,
		0, 0,
		m_hParent,
		(HMENU)m_uiSt,
		m_hInst,
		NULL);
	if(!m_hSb) return FALSE;

	// うまく言ったらパーツ設定
	ReDraw();

	if(m_nProgressPos > 0 && m_nProgressPos < m_nParts){
		m_hProg = CreateWindowEx(0,
			PROGRESS_CLASS,
			NULL,
			PROGRESSBAR_STYLE,
			0, 0,
			0, 0,
			m_hSb,			// 親はステータスバー
			(HMENU)m_uiProg,
			m_hInst,
			NULL);
		if(!m_hProg) return FALSE;
	}

	GetClientRect(m_hSb, &m_Rc);

	if(bProcChain){
		// ｻﾌﾞｸﾗｽ化
//		m_OrgWndProc = (WNDPROC)GetWindowLong(m_hSb, GWL_WNDPROC);
//		SetWindowLong(m_hSb, GWL_WNDPROC, (LONG)ChainStatusbarProc);
		m_OrgWndProc = (WNDPROC)GetWindowLongPtr(m_hSb, GWLP_WNDPROC);
		SetWindowLongPtr(m_hSb, GWLP_WNDPROC, (LONG_PTR)ChainStatusbarProc);
		SetProp(m_hSb, STR_PROGSTBARCLASS, (HANDLE)this);
	}

	return TRUE;
}

void CProgStbar::SetFixedParts(const int nParts, const int nSize)
{
	if(nParts < 0 || nParts > MAX_PB_PARTS) return;
	if(nSize < 0) return;
	m_nFixedPartsSize[nParts] = nSize + (GetSystemMetrics(SM_CXEDGE) * 4);	// 少し調整
}

HICON CProgStbar::GetStIcon(int nImg)
{
	if(m_hImgSt == NULL) return NULL;
	return (ImageList_GetIcon(m_hImgSt, nImg, ILD_TRANSPARENT));
}

int CProgStbar::SetStatusImage(UINT uiBmpRes,
							   const int nImgCount,
							   const int cx/* = 16*/, const int cy/* = 16*/,
							   COLORREF colMaskColor/*=0*/)
{
	if(m_hImgSt) ImageList_Destroy(m_hImgSt);
	if(!(m_hImgSt = ImageList_Create(cx, cy, ILC_COLOR24 | ILC_MASK, nImgCount, 0))) return 0;
	HBITMAP hBmp = (HBITMAP)LoadImage(m_hInst,
			MAKEINTRESOURCE(uiBmpRes),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR | LR_SHARED);
	ImageList_AddMasked(m_hImgSt, hBmp, colMaskColor);
	DeleteObject(hBmp);
	m_nIconWidth = cx;
	return (ImageList_GetImageCount(m_hImgSt));
}

void CProgStbar::ReDraw()
{
	RECT rc;
	GetClientRect(m_hParent, &rc);
	Resize(m_hParent, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
	UpdateWindow(m_hSb);
}

void CProgStbar::Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	SendMessage(m_hSb, WM_SIZE, wp, lp);

	if(m_nParts < 1) return;

	GetClientRect(m_hSb, &m_Rc);
	int *pnStatusParts = (int *)GlobalAlloc(GPTR, (m_nParts * sizeof(int)));
	if(pnStatusParts == NULL) return;
	int nTotalSize = m_Rc.right, nPartsSize, nSize = 0, i;
	// サイズグリップがあるなら考慮する
	DWORD dwStyle = (DWORD)GetWindowLong(m_hSb, GWL_STYLE);
	// 垂直スクロールバーの幅+ウィンドウエッジくらい？
	if(dwStyle & SBARS_SIZEGRIP) nTotalSize -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXEDGE);
	// 幅から固定サイズ分減算
	nPartsSize = nTotalSize;
	for(i = 0; i < m_nParts; i++){
		if(m_nFixedPartsSize[i] > 0) nPartsSize -= m_nFixedPartsSize[i];
	}
	// パーツサイズ計算
	for(i = 0; i < (m_nParts - 1); i++){
		if(m_nFixedPartsSize[i] > 0){
			nSize += m_nFixedPartsSize[i];
		}else{
			nSize += (int)(nPartsSize * ((float)m_nPartsSize[i] / 100));
		}
		*(pnStatusParts + i) = nSize;
	}
	*(pnStatusParts + i) = nTotalSize;
	// サイズ設定
	SendMessage(m_hSb, SB_SETPARTS, (WPARAM)m_nParts, (LPARAM)pnStatusParts);
	GlobalFree(pnStatusParts);

	SendMessage(m_hSb, SB_GETRECT, (WPARAM)m_nProgressPos, (LPARAM)&m_Rc);
	if(m_hProg){
		HICON hIcon = (HICON)SendMessage(m_hSb, SB_GETICON, (WPARAM)m_nProgressPos, 0L);
		MoveWindow(m_hProg,
			m_Rc.left + 2 + (hIcon ? m_nIconWidth : 0),
			m_Rc.top + 2,
			((m_Rc.right - m_Rc.left) - 5) - (hIcon ? m_nIconWidth : 0),
			(m_Rc.bottom - m_Rc.top) - 5,
			TRUE);
	}
}

void CProgStbar::SetSbText(int nPart, UINT uiType, LPCTSTR lpsSbText, const int nImg/*=-1*/)
{
	HICON hIcon = (HICON)SendMessage(m_hSb, SB_GETICON, (WPARAM)nPart, 0L);
	if(hIcon) DestroyIcon(hIcon);
	if(nImg >= 0){
		if((hIcon = GetStIcon(nImg))) SendMessage(m_hSb, SB_SETICON, (WPARAM)nPart, (LPARAM)hIcon);
	}else{
		SendMessage(m_hSb, SB_SETICON, (WPARAM)nPart, (LPARAM)NULL);
	}
	SendMessage(m_hSb, SB_SETTEXT, (WPARAM)(nPart | uiType), (LPARAM)lpsSbText);
	m_nPartsStyle[nPart] = uiType;
}

void CProgStbar::Invalidate()
{
	InvalidateRect(m_hSb, NULL, TRUE);
}

void CProgStbar::ShowProg(BOOL bShow)
{
	if(m_hProg == NULL) return;
	if(bShow){
		ShowWindow(m_hProg, SW_SHOW);
	}else{
		ShowWindow(m_hProg, SW_HIDE);
	}
}

void CProgStbar::SetProg(int nMin, int nMax, int nStep)
{
	if(m_hProg == NULL) return;
	SendMessage(m_hProg, PBM_SETRANGE32, (WPARAM)nMin, (LPARAM)nMax);
	SendMessage(m_hProg, PBM_SETSTEP, (WPARAM)nStep, 0L);
}

void CProgStbar::StepProg(void)
{
	if(m_hProg == NULL) return;
	SendMessage(m_hProg, PBM_STEPIT, 0L, 0L);
}

LRESULT CALLBACK CProgStbar::ChainStatusbarProc(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	CProgStbar *lpcThis = (CProgStbar *)GetProp(hSb, STR_PROGSTBARCLASS);
	if(!lpcThis) return 0L;
	
	switch(msg){
	case WM_LBUTTONDOWN:
		return (lpcThis->LButtonDown(hSb, msg, wp, lp));
	case WM_LBUTTONUP:
		return (lpcThis->LButtonUp(hSb, msg, wp, lp));
	case WM_LBUTTONDBLCLK:
		return (lpcThis->LButtonDblClk(hSb, msg, wp, lp));
	case WM_RBUTTONDOWN:
		return (lpcThis->RButtonDown(hSb, msg, wp, lp));
	case WM_RBUTTONUP:
		return (lpcThis->RButtonUp(hSb, msg, wp, lp));
	case WM_RBUTTONDBLCLK:
		return (lpcThis->RButtonDblClk(hSb, msg, wp, lp));
	case WM_MBUTTONDOWN:
		return (lpcThis->MButtonDown(hSb, msg, wp, lp));
	case WM_MBUTTONUP:
		return (lpcThis->MButtonUp(hSb, msg, wp, lp));
	case WM_MBUTTONDBLCLK:
		return (lpcThis->MButtonDblClk(hSb, msg, wp, lp));
	default:
		return (lpcThis->ChainDefault(hSb, msg, wp, lp));
	}
	return (0L);
}

LRESULT CProgStbar::ChainDefault(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

int CProgStbar::GetPointParts(int x, int y)
{
	POINT pt = {x, y};

	int *pnPartsList = (int *)GlobalAlloc(GPTR, m_nParts * sizeof(int));
	if(!pnPartsList) return -1;
	SendMessage(m_hSb, SB_GETPARTS, (WPARAM)m_nParts, (LPARAM)pnPartsList);
	int n;
	for(n = 0; n < m_nParts; n++){
		if(pt.x < *(pnPartsList + n)) break;
	}
	GlobalFree(pnPartsList);
	return n;
}

LRESULT CProgStbar::LButtonDown(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::LButtonUp(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::LButtonDblClk(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::RButtonDown(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::RButtonUp(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::RButtonDblClk(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::MButtonDown(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::MButtonUp(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}

LRESULT CProgStbar::MButtonDblClk(HWND hSb, UINT msg, WPARAM wp, LPARAM lp)
{
	return (CallWindowProc(m_OrgWndProc, hSb, msg, wp, lp));
}
