/*----------------------------------------------------------------------------------------

	CDlgbar.cpp

----------------------------------------------------------------------------------------*/
#include "CDlgbar.h"

CDlgbar::CDlgbar(HINSTANCE hCurInst, HWND hWnd)
 : m_hParent(hWnd), m_hInst(hCurInst), m_hBmpBack(NULL), m_hRebar(NULL)
{
}

CDlgbar::CDlgbar(HINSTANCE hCurInst, HWND hWnd, DWORD dwICC)
 : m_hParent(hWnd), m_hInst(hCurInst), m_hBmpBack(NULL), m_hRebar(NULL)
{
	if(!(dwICC & ICC_COOL_CLASSES)) dwICC |= ICC_COOL_CLASSES;
	InitWin32Commctrl(dwICC);
}

CDlgbar::~CDlgbar()
{
	int i;
	for(i = 0; i < (int)m_vecBmptb.size(); i++) delete m_vecBmptb[i];
	for(i = 0; i < (int)m_vecDlgbar.size(); i++) DestroyWindow(m_vecDlgbar[i]);
	if(m_hRebar) DestroyWindow(m_hRebar);
	if(m_hBmpBack) DeleteObject(m_hBmpBack);
}

void CDlgbar::InitWin32Commctrl(DWORD dwICC/* = ICC_COOL_CLASSES | ICC_BAR_CLASSES*/)
{
	INITCOMMONCONTROLSEX icex;
	InitCommonControls();
	ZeroMemory(&icex, sizeof(INITCOMMONCONTROLSEX));
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = dwICC;
	InitCommonControlsEx(&icex);
}

BOOL CDlgbar::MakeRebar(UINT uiID, BOOL bLeftBar/* = FALSE*/)
{
	REBARINFO rbi;
	// ﾚﾊﾞｰ作成
	m_hRebar = CreateWindowEx(WS_EX_TOOLWINDOW,
		REBARCLASSNAME,
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		RBS_BANDBORDERS | RBS_VARHEIGHT |
		CCS_TOP | CCS_NOPARENTALIGN | CCS_NODIVIDER | (bLeftBar ? CCS_LEFT : 0),
		0, 0,
		0, 0,
		m_hParent,
		(HMENU)uiID,
		m_hInst,
		NULL);
	if(!m_hRebar) return FALSE;

	// 初期化
	ZeroMemory(&rbi, sizeof(REBARINFO));
	rbi.cbSize = sizeof(REBARINFO);
	SendMessage(m_hRebar, RB_SETBARINFO, 0, (LPARAM)&rbi);

	// 親ウィンドウを明示的に設定
	SetParent(m_hRebar, m_hParent);

	return TRUE;
}

int CDlgbar::SetBmpBack(UINT unBmpRes)
{
	if(m_hBmpBack) return -1;
	m_hBmpBack = (HBITMAP)LoadImage(m_hInst,
								MAKEINTRESOURCE(unBmpRes),
								IMAGE_BITMAP,
								0, 0,
								LR_DEFAULTCOLOR | LR_SHARED);
	if(!m_hBmpBack) return -1;
	return 0;
}

BOOL CDlgbar::PutRebar(HWND hBar, const char *pszcBandStr,
					   BOOL bBreak, BOOL bUseGrop,
					   UINT wID,
					   const int cx/*=0*/, const int cy/*=0*/,
					   const int nMin/*=0*/)
{
	if(!m_hRebar) return FALSE;

	REBARBANDINFO rbBand;
	RECT rc;
	// 構造体セット
	GetClientRect(hBar, &rc);
	if(cx > 0) rc.right = cx;
	if(cy > 0) rc.bottom = cy;
	ZeroMemory(&rbBand, sizeof(REBARBANDINFO));
	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask  = RBBIM_STYLE | RBBIM_CHILD |
		RBBIM_CHILDSIZE | RBBIM_SIZE |
		(m_hBmpBack ? RBBIM_BACKGROUND : 0) |
		(wID > 0 ? RBBIM_ID : 0) |
		(cx > 0 ? RBBIM_IDEALSIZE : 0) |
		(pszcBandStr ? RBBIM_TEXT : 0);
	rbBand.fStyle = RBBS_CHILDEDGE |
		(bBreak ? RBBS_BREAK : 0) |
		(bUseGrop ? RBBS_GRIPPERALWAYS : RBBS_NOGRIPPER);
	rbBand.cxMinChild = (bUseGrop ? nMin : rc.right);
	rbBand.cyMinChild = rc.bottom;
	rbBand.hwndChild = hBar;
	rbBand.hbmBack = m_hBmpBack;
	rbBand.wID = wID;
	rbBand.cx = rc.right;
	rbBand.cxIdeal = rc.right;
	if(pszcBandStr) rbBand.lpText = (LPTSTR)pszcBandStr;

	// バンド追加
	if(!SendMessage(m_hRebar, RB_INSERTBAND,
		(WPARAM)m_vecDlgbar.size(), (LPARAM)&rbBand)) return FALSE;

	// コンテナ追加
	m_vecDlgbar.push_back(hBar);

	// メッセージ送信
	SendMessage(hBar,
		WM_DLGBAR_RELEASE,
		(WPARAM)(m_vecDlgbar.size() - 1),
		(LPARAM)this);

	return TRUE;
}

void CDlgbar::ReflashBandSize(const int nCorrect)
{
	if(!m_hRebar) return;

	int nCount = (int)SendMessage(m_hRebar, RB_GETBANDCOUNT, 0, 0L);
	for(int i = 0; i < nCount; i++) ReflashBandSize(i, 0);
}

void CDlgbar::ReflashBandSize(const int nBand, const int nCorrect)
{
	if(!m_hRebar) return;

	REBARBANDINFO rbBand;
	ZeroMemory(&rbBand, sizeof(REBARBANDINFO));
	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
	if(SendMessage(m_hRebar, RB_GETBANDINFO, nBand, (LPARAM)&rbBand)){
		RECT rcChild;
		GetClientRect(rbBand.hwndChild, &rcChild);
		rbBand.cxMinChild = (rbBand.fStyle & RBBS_GRIPPERALWAYS ? rbBand.cxMinChild : rcChild.right + nCorrect);
		rbBand.cx = rcChild.right + nCorrect;
		rbBand.cxIdeal = rcChild.right + nCorrect;
		// 再設定
		rbBand.fMask = RBBIM_CHILDSIZE | RBBIM_SIZE;
		SendMessage(m_hRebar, RB_SETBANDINFO, nBand, (LPARAM)&rbBand);
	}
}

int CDlgbar::FindToolbar(HWND hToolbar)
{
	for(int i = 0; i < (int)m_vecBmptb.size(); i++){
		if(m_vecBmptb[i]->m_hToolbar == hToolbar) return i;
	}
	
	return -1;
}

int CDlgbar::FindToolbar(UINT uiToolbar)
{
	for(int i = 0; i < (int)m_vecBmptb.size(); i++){
		if(m_vecBmptb[i]->m_uID == uiToolbar) return i;
	}
	
	return -1;
}

HWND CDlgbar::GetRebarHandle(void)
{
	return m_hRebar;
}

HWND CDlgbar::GetToolbarHandle(UINT uID)
{
	for(int i = 0; i < (int)m_vecBmptb.size(); i++){
//		if((UINT)GetWindowLong(m_vecBmptb[i]->m_hToolbar, GWL_ID) == uID) return m_vecBmptb[i]->m_hToolbar;
		if((UINT)GetWindowLongPtr(m_vecBmptb[i]->m_hToolbar, GWL_ID) == uID) return m_vecBmptb[i]->m_hToolbar;
	}
	return NULL;
}

HWND CDlgbar::GetDlgbarHandle(UINT wID)
{
	int n = (int)SendMessage(m_hRebar, RB_GETBANDCOUNT, (WPARAM)0L, (LPARAM)0L);
	for(int i = 0; i < n; i++){
		REBARBANDINFO rbBand;
		ZeroMemory(&rbBand, sizeof(REBARBANDINFO));
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask  = RBBIM_ID | RBBIM_CHILD;
		SendMessage(m_hRebar, RB_GETBANDINFO, (WPARAM)i, (LPARAM)&rbBand);
		if(wID == rbBand.wID) return (rbBand.hwndChild);
	}
	return NULL;
}

HWND CDlgbar::AddToolbar(LPCTSTR lpcsCaption, BOOL bBreak, BOOL bUseGrop,
 					 	 UINT uiID, DWORD dwStyle,
						 const int cx/*=0*/, const int cy/*=0*/,
						 const int nMin/*=0*/)
{
//	DWORD dwRebarStyle = (DWORD)GetWindowLong(m_hRebar, GWL_STYLE);
	DWORD dwRebarStyle = (DWORD)GetWindowLongPtr(m_hRebar, GWL_STYLE);
	// ﾂｰﾙﾊﾞｰ作成
	CBmpToolbar *lpcBmptb = (CBmpToolbar *)new CBmpToolbar(m_hInst,
		m_hRebar,
		lpcsCaption);
	if(lpcBmptb == NULL) return NULL;
	lpcBmptb->MakeToolbar(uiID, dwStyle | (dwRebarStyle & CCS_LEFT ? TBSTYLE_WRAPABLE : 0));
	// ﾊﾞﾝﾄﾞにｾｯﾄ
	if(!PutRebar(lpcBmptb->m_hToolbar, lpcsCaption, bBreak, bUseGrop, uiID, cx, cy, nMin)){
		DestroyWindow(lpcBmptb->m_hToolbar);
		return NULL;
	}
	m_vecBmptb.push_back(lpcBmptb);
	return lpcBmptb->m_hToolbar;
}

int CDlgbar::AddTbImageList(HWND hToolbar, const int nBtnSize, UINT uiBtnRes, UINT uiMaskRes)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	// ﾂｰﾙﾊﾞｰ作成
	m_vecBmptb[i]->SetImageList(nBtnSize, uiBtnRes, uiMaskRes, TB_IMG_ENABLE);
	return 0;
}

int CDlgbar::AddTbPngImageList(
	HWND hToolbar,
	const int nBtnSize,
	UINT uiPngRes)
{
	int i =
		FindToolbar(
			hToolbar);

	if (i < 0) {
		return -1;
	}

	m_vecBmptb[i]->SetPngImageList(
		nBtnSize,
		uiPngRes,
		TB_IMG_ENABLE);

	return 0;
}

int CDlgbar::AddTbDisableImageList(HWND hToolbar, const int nBtnSize, UINT uiBtnRes, UINT uiMaskRes)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	// ﾂｰﾙﾊﾞｰ作成
	m_vecBmptb[i]->SetImageList(nBtnSize, uiBtnRes, uiMaskRes, TB_IMG_DISABLE);
	return 0;
}

int CDlgbar::AddTbPngDisableImageList(
	HWND hToolbar,
	const int nBtnSize,
	UINT uiPngRes)
{
	int i =
		FindToolbar(
			hToolbar);

	if (i < 0) {
		return -1;
	}

	m_vecBmptb[i]->SetPngImageList(
		nBtnSize,
		uiPngRes,
		TB_IMG_DISABLE);

	return 0;
}

int CDlgbar::AddTbHotImageList(HWND hToolbar, const int nBtnSize, UINT uiBtnRes, UINT uiMaskRes)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	// ﾂｰﾙﾊﾞｰ作成
	m_vecBmptb[i]->SetImageList(nBtnSize, uiBtnRes, uiMaskRes, TB_IMG_HOT);
	return 0;
}

int CDlgbar::AddTbPngHotImageList(
	HWND hToolbar,
	const int nBtnSize,
	UINT uiPngRes)
{
	int i =
		FindToolbar(
			hToolbar);

	if (i < 0) {
		return -1;
	}

	m_vecBmptb[i]->SetPngImageList(
		nBtnSize,
		uiPngRes,
		TB_IMG_HOT);

	return 0;
}

int CDlgbar::AddTbString(HWND hToolbar, UINT uiStringRes)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	// ﾂｰﾙﾊﾞｰ作成
	return m_vecBmptb[i]->AddString(uiStringRes);
}

int CDlgbar::AddTbString(HWND hToolbar, LPCTSTR lpcString)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	// ﾂｰﾙﾊﾞｰ作成
	return m_vecBmptb[i]->AddString(lpcString);
}

int CDlgbar::AddTbButton(HWND hToolbar, TBBUTTON *lpTbBtn, const int nBtn/* = 0*/)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	// ボタン作成
	m_vecBmptb[i]->AddBtn(lpTbBtn, nBtn);
	// バンドサイズ調整
	AdjustBandSize(i);
	// 表示
	m_vecBmptb[i]->ShowToolbar();
	return 0;
}

int CDlgbar::AdjustBandSize(const int i)
{
	if(i < 0) return -1;
	SIZE size = { 0, 0 };
	m_vecBmptb[i]->GetWndSize(&size);
	REBARBANDINFO rbi;
	ZeroMemory(&rbi, sizeof(REBARBANDINFO));
	rbi.cbSize = sizeof(REBARBANDINFO);
	rbi.fMask = RBBIM_CHILDSIZE;
	rbi.cxMinChild = size.cx + (GetSystemMetrics(SM_CXEDGE) * 3);
	rbi.cyMinChild = size.cy + (GetSystemMetrics(SM_CYEDGE) * 2);
	return (int)SendMessage(m_hRebar, RB_SETBANDINFO, (WPARAM)GetId2Index(m_vecBmptb[i]->GetID()), (LPARAM)&rbi);
}

int CDlgbar::AdjustBandSize(HWND hToolbar)
{
	int i = FindToolbar(hToolbar);
	return AdjustBandSize(i);
}

void CDlgbar::InsTbSeps(HWND hToolbar, const int nSeps[], const int nCount)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	for(int n = 0; n < nCount; n++) m_vecBmptb[i]->InsSep(nSeps[n]);
}

void CDlgbar::InsTbToolTips(HWND hToolbar, const char *pszTipText[], BOOL bUseBalloon/* = FALSE*/)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->SetTooltips(pszTipText, bUseBalloon);
}

void CDlgbar::SetTbIndent(HWND hToolbar, int nIndent)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->SetIndent(nIndent);
}

void CDlgbar::SetTbString(HWND hToolbar, int nId, UINT uiStringRes)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->SetString(nId, uiStringRes);
	AdjustBandSize(i);
}

void CDlgbar::SetTbString(HWND hToolbar, int nId, char *pszString)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->SetString(nId, pszString);
	AdjustBandSize(i);
}

void CDlgbar::SetTbBtnWidth(HWND hToolbar, int nId, int nWidth)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->SetBtnWidth(nId, nWidth);
}

void CDlgbar::EnableTbButton(HWND hToolbar, UINT uiId, BOOL bEnable)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->EnableButton(uiId, bEnable);
}

void CDlgbar::CheckedTbButton(HWND hToolbar, UINT uiId, UINT uiChecked)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->CheckedButton(uiId, uiChecked);
}

DWORD CDlgbar::IsTbCheck(HWND hToolbar, UINT uiId)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	return m_vecBmptb[i]->IsCheck(uiId);
}

void CDlgbar::PressedTbButton(HWND hToolbar, UINT uiId, BOOL bPressed)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return;
	m_vecBmptb[i]->PressedButton(uiId, bPressed);
}

DWORD CDlgbar::IsTbPressed(HWND hToolbar, UINT uiId)
{
	int i = FindToolbar(hToolbar);
	if(i < 0) return -1;
	return m_vecBmptb[i]->IsPressed(uiId);
}

HWND CDlgbar::AddDlgbar(WORD wRes,
						DLGPROC dlgProc,
						const char *pszcBandStr,
						BOOL bBreak, BOOL bUseGrop,
						const int cx/*=0*/, const int cy/*=0*/,
						const int nMin/*=0*/)
{
	HWND hDlgbar;
	// ﾀﾞｲｱﾛｸﾞﾊﾞｰ作成
	hDlgbar = CreateDialog(m_hInst,
		MAKEINTRESOURCE(wRes),
		m_hRebar,
		dlgProc);
	if(!hDlgbar) return NULL;
	// ﾊﾞﾝﾄﾞにｾｯﾄ
	if(!PutRebar(hDlgbar, pszcBandStr, bBreak, bUseGrop, (UINT)wRes, cx, cy, nMin)){
		DestroyWindow(hDlgbar);
		return NULL;
	}
	// 表示
	ShowWindow(hDlgbar, SW_SHOW);
	UpdateWindow(hDlgbar);
	return hDlgbar;
}

HWND CDlgbar::AddDlgbarParam(WORD wRes,
							 DLGPROC dlgProc,
							 LPARAM lp,
							 const char *pszcBandStr,
							 BOOL bBreak, BOOL bUseGrop,
							 const int cx/*=0*/, const int cy/*=0*/,
							 const int nMin/*=0*/)
{
	HWND hDlgbar;

	// ﾀﾞｲｱﾛｸﾞﾊﾞｰ作成
	hDlgbar = CreateDialogParam(m_hInst,
		MAKEINTRESOURCE(wRes),
		m_hRebar,
		dlgProc,
		lp);
	if(!hDlgbar) return NULL;

	// ﾊﾞﾝﾄﾞにｾｯﾄ
	if(!PutRebar(hDlgbar, pszcBandStr, bBreak, bUseGrop, (UINT)wRes, cx, cy, nMin)){
		DestroyWindow(hDlgbar);
		return NULL;
	}

	// 表示
	ShowWindow(hDlgbar, SW_SHOW);
	UpdateWindow(hDlgbar);

	return hDlgbar;
}

LRESULT CDlgbar::Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
//	SendMessage(m_hRebar, WM_SIZE, 0L, 0L);
	// Windowsに任せるとエッジが消える・・・orz
	RECT rc;
	GetWindowRect(m_hRebar, &rc);
	MoveWindow(m_hRebar, 0, 0, LOWORD(lp), (rc.bottom - rc.top), TRUE);
	return (0L);
}

void CDlgbar::HeightChange(void)
{
	RECT rc;
	for(int i = 0; i < (int)m_vecDlgbar.size(); i++){
		GetDlgbarRect(i, &rc);
		MoveWindow(m_vecDlgbar[i],
			0, 0,
			(rc.right - rc.left), (rc.bottom - rc.top), TRUE);
	}
}

void CDlgbar::GetRect(LPRECT lprc)
{
	GetClientRect(m_hRebar, lprc);
	// エッジ分のサイズ微調整
	lprc->bottom += GetSystemMetrics(SM_CYEDGE);
}

HWND CDlgbar::GetDlgbarRect(unsigned int nDlgbar, LPRECT lprc)
{
	for(int i = 0; i < (int)m_vecDlgbar.size(); i++){
		if((unsigned int)i == nDlgbar){
			SendMessage(m_hRebar, RB_GETRECT, (WPARAM)i, (LPARAM)lprc);
			return m_vecDlgbar[i];
		}
	}

	return NULL;
}

HWND CDlgbar::GetDlgbarRect(HWND hDlgbar, LPRECT lprc)
{
	for(int i = 0; i < (int)m_vecDlgbar.size(); i++){
		if(m_vecDlgbar[i] == hDlgbar){
			SendMessage(m_hRebar, RB_GETRECT, (WPARAM)i, (LPARAM)lprc);
			return m_vecDlgbar[i];
		}
	}

	return NULL;
}

int CDlgbar::GetId2Index(UINT uBandID)
{
	return (int)SendMessage(m_hRebar, RB_IDTOINDEX, (WPARAM)uBandID, 0L);
}

int CDlgbar::ShowDlgbar(HWND hDlgbar, BOOL bShow)
{
	for(int i = 0; i < (int)m_vecDlgbar.size(); i++){
		if(m_vecDlgbar[i] == hDlgbar){
			SendMessage(m_hRebar, RB_SHOWBAND, (WPARAM)i, (LPARAM)bShow);
			return i;
		}
	}

	return 0;
}

HWND CDlgbar::ShowDlgbar(unsigned int nDlgbar, BOOL bShow)
{
	for(int i = 0; i < (int)m_vecDlgbar.size(); i++){
		SendMessage(m_hRebar, RB_SHOWBAND, (WPARAM)i, (LPARAM)bShow);
		return m_vecDlgbar[i];
	}

	return NULL;
}
