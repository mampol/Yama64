/*------------------------------------------------------------------------------
	CUICombo.cpp
------------------------------------------------------------------------------*/
#include "CUICombo.h"

const COLORREF crDefColor[] = {
	RGB(0, 0, 0),
	RGB(80, 80, 80),
	RGB(192, 192, 192),
	RGB(255, 255, 255),
	RGB(80, 0, 0),
	RGB(255, 0, 0),
	RGB(80, 0, 80),
	RGB(255, 0, 255),
	RGB(0, 80, 0),
	RGB(0, 255, 0),
	RGB(80, 80, 0),
	RGB(255, 255, 0),
	RGB(0, 0, 80),
	RGB(0, 0, 255),
	RGB(0, 80, 80),
	RGB(0, 255, 255)
};

const char *pszDefColorStr[] = {
	"黒色",
	"灰色",
	"銀色",
	"白色",
	"茶色",
	"赤色",
	"紫色",
	"フクシア",
	"緑色",
	"ライム",
	"オリーブ",
	"黄色",
	"ネイビー",
	"青色",
	"コガモ",
	"水色",
	"カスタム"			// カスタム色
};

void CUICombo::DeleteAllString(HWND hCombo)
{
	int n = ComboBox_GetCount(hCombo);
	for(int i = 0; i < n; i++) ComboBox_DeleteString(hCombo, 0);
}

int CALLBACK CUICombo::EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	CUICombo *lpcUiCombo = (CUICombo *)lParam;
	if(strncmp("@", (const char *)lpelfe->elfFullName, 1) != 0) ComboBox_AddString(lpcUiCombo->m_hTmp, lpelfe->elfFullName);
	return 1;
}

COLORREF CUICombo::GetColor(const char *pszColor)
{
	COLORREF crColor = RGB(255, 255, 255);
	int nIm;
	for(pszColor += 2; *pszColor != '\0'; pszColor++){
		if(*pszColor >= '0' && *pszColor <= '9'){
			nIm = *pszColor - '0';
		}else if(*pszColor >= 'A' && *pszColor <= 'F'){
			nIm = *pszColor - 'A' + 10;
		}else if(*pszColor >= 'a' && *pszColor <= 'f'){
			nIm = *pszColor - 'a' + 10;
		}else{
			break;
		}
		crColor = 16 * crColor + nIm;
	}
	return crColor;
}

void CUICombo::CreateFontFaceCombo(HWND hCombo, BYTE btCharSet, const char *pszFaceName)
{
	if(hCombo == NULL) return;
	m_hTmp = hCombo;

	DeleteAllString(hCombo);

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfCharSet = btCharSet;
//	strcpy(lf.lfFaceName, pszFaceName);
	strcpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), pszFaceName);

	HDC hDC = GetDC(hCombo);
	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);
	ReleaseDC(hCombo, hDC);
}

void CUICombo::CreateFontSizeCombo(HWND hCombo, UINT uiBegin, UINT uiEnd)
{
	if(hCombo == NULL) return;

	DeleteAllString(hCombo);

	for(UINT i = uiBegin; i < uiEnd; i++){
		char szComboStr[12];
		wsprintf(szComboStr, "%d", (i + 1));
		ComboBox_AddString(hCombo, szComboStr);
	}
}

void CUICombo::CreateColorCombo(HWND hCombo, COLORREF colUser/* = RGB(0, 0, 0)*/)
{
	if(hCombo == NULL) return;

//	DWORD dwStyles = (DWORD)GetWindowLong(hCombo, GWL_STYLE);
	DWORD dwStyles = (DWORD)GetWindowLongPtr(hCombo, GWL_STYLE);
	if(dwStyles & CBS_OWNERDRAWFIXED || dwStyles & CBS_OWNERDRAWVARIABLE){
		if(dwStyles & CBS_HASSTRINGS) return;

		DeleteAllString(hCombo);

		COLORREF crCustom = colUser;
		int n = sizeof(crDefColor) / sizeof(COLORREF);
		for(int i = 0; i < n; i++){
			ComboBox_SetItemData(hCombo, (int)ComboBox_AddString(hCombo, ""), crDefColor[i]);
			if(colUser == crDefColor[i]) crCustom = crDefColor[0];
		}
		// カスタム色
		ComboBox_SetItemData(hCombo, (int)ComboBox_AddString(hCombo, ""), crCustom);
	}
}

// WM_MEASUREITEM
LRESULT CUICombo::OnMeasureItemColorCombo(HWND hCombo, HWND hParent, UINT msg, WPARAM wp, LPARAM lp)
{
	if(hCombo == NULL) return FALSE;

	// 高さ
	TEXTMETRIC tm;
	HDC hDC = GetDC(hParent);
	GetTextMetrics(hDC, &tm);
	ReleaseDC(hParent, hDC);
	// 幅
	RECT rc;
	GetClientRect(hCombo, &rc);

	((LPMEASUREITEMSTRUCT)lp)->itemHeight = tm.tmHeight;
	((LPMEASUREITEMSTRUCT)lp)->itemWidth  = rc.right;

	return TRUE;
}

// WM_DRAWITEM
LRESULT CUICombo::OnDrawItemColorCombo(HWND hCombo, HWND hParent, UINT msg, WPARAM wp, LPARAM lp)
{
	if(hCombo == NULL) return FALSE;
	LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lp;
	if(lpDis->hwndItem != hCombo) return FALSE;
//	DWORD dwStyles = (DWORD)GetWindowLong(hCombo, GWL_STYLE);
	DWORD dwStyles = (DWORD)GetWindowLongPtr(hCombo, GWL_STYLE);
	if(dwStyles & CBS_HASSTRINGS) return FALSE;

	// 塗りつぶす
	FillRect(lpDis->hDC, &(lpDis->rcItem), GetSysColorBrush(COLOR_WINDOW));
	// 選択枠
	if(lpDis->itemState & ODS_SELECTED) DrawFocusRect(lpDis->hDC, &(lpDis->rcItem));

	// 選択
	if(ComboBox_GetCurSel(hCombo) == CB_ERR) return TRUE;

	// 色
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	hPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWTEXT));
	hBrush = CreateSolidBrush(lpDis->itemData);
	hOldPen = (HPEN)SelectObject(lpDis->hDC, hPen);
	hOldBrush = (HBRUSH)SelectObject(lpDis->hDC, hBrush);
	Rectangle(lpDis->hDC,
		lpDis->rcItem.left + 2, lpDis->rcItem.top + 2,
		20, lpDis->rcItem.bottom - 2);
	SelectObject(lpDis->hDC, hOldPen);
	SelectObject(lpDis->hDC, hOldBrush);
	DeleteObject(hBrush);
	DeleteObject(hPen);

	// 文字
	RECT rc;
	HFONT hFont, hOldFont;
	SetRect(&rc,
		lpDis->rcItem.left + 24, lpDis->rcItem.top,
		lpDis->rcItem.right, lpDis->rcItem.bottom);
	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	hOldFont = (HFONT)SelectObject(lpDis->hDC, hFont);
	SetBkMode(lpDis->hDC, TRANSPARENT);
	SetTextColor(lpDis->hDC, GetSysColor(COLOR_WINDOWTEXT));
	DrawText(lpDis->hDC, pszDefColorStr[lpDis->itemID], -1, &rc, DT_SINGLELINE | DT_VCENTER);
	SelectObject(lpDis->hDC, hOldFont);

	return TRUE;
}

// ｶｽﾀﾑ選択時
BOOL CUICombo::OnCommandCustomColor(HWND hCombo, HWND hParent)
{
	if(hCombo == NULL) return FALSE;

	COLORREF crCustom[16];
	ZeroMemory(crCustom, sizeof(crCustom));

	// 現在のカスタム色
	int nCount = ComboBox_GetCount(hCombo);
	int nIdx = ComboBox_GetCurSel(hCombo);
	if(nIdx == CB_ERR) return FALSE;
	// カスタム選択？
	if((nCount - 1) != nIdx) return FALSE;
	crCustom[0] = (COLORREF)ComboBox_GetItemData(hCombo, nIdx);

	// 色選択ダイアログ
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hParent;
	cc.lpCustColors = crCustom;
	cc.Flags = CC_RGBINIT;
	if(!ChooseColor(&cc)) return FALSE;

	// カスタム色変更
	ComboBox_SetItemData(hCombo, nIdx, cc.rgbResult);

	return TRUE;
}

int CUICombo::SelectListItem(HWND hCombo, const char *pszSelectStr)
{
	int n = ComboBox_FindString(hCombo, 0, pszSelectStr);
	if(n == CB_ERR) return -1;
	ComboBox_SetCurSel(hCombo, n);
	return n;
}

int CUICombo::SelectListItem(HWND hCombo, const COLORREF crSelectColor)
{
	int nCount = ComboBox_GetCount(hCombo);
	if(nCount == CB_ERR) return -1;
	int nIdx;
	for(nIdx = 0; nIdx < nCount; nIdx++){
		if((COLORREF)ComboBox_GetItemData(hCombo, nIdx) == crSelectColor) break;
	}
	if(nIdx == nCount){
		nIdx--;
		ComboBox_SetItemData(hCombo, nIdx, crSelectColor);
		UpdateWindow(hCombo);
	}
	ComboBox_SetCurSel(hCombo, nIdx);
	return nIdx;
}

int CUICombo::SelectListItem(HWND hCombo, const int nSelectIdx)
{
	return ComboBox_SetCurSel(hCombo, nSelectIdx);
}

int CUICombo::GetSelText(HWND hCombo, char *pszSelText, const int nLen)
{
	if(hCombo == NULL) return -1;

	int nSelIdx, nSelLen;
	if((nSelIdx = ComboBox_GetCurSel(hCombo)) == CB_ERR) return -1;
	if((nSelLen = ComboBox_GetLBTextLen(hCombo, nSelIdx) > nLen)) return nSelLen;
	if(ComboBox_GetLBText(hCombo, nSelIdx, pszSelText) == CB_ERR) return -1;
	return 0;
}

int CUICombo::GetSelInt(HWND hCombo)
{
	if(hCombo == NULL) return 0;

	int nSelIdx, nSelLen, nValue = 0;
	if((nSelIdx = ComboBox_GetCurSel(hCombo)) == CB_ERR) return 0;
	nSelLen = ComboBox_GetLBTextLen(hCombo, nSelIdx);
	char *pszValue = (char *)GlobalAlloc(GPTR, (DWORD)(nSelLen + 1));
	if(pszValue == NULL) return 0;
	if(ComboBox_GetLBText(hCombo, nSelIdx, pszValue) != CB_ERR) nValue = atoi(pszValue);
	GlobalFree(pszValue);

	return nValue;
}

COLORREF CUICombo::GetSelColor(HWND hCombo)
{
	if(hCombo == NULL) return FALSE;

	int nIdx = ComboBox_GetCurSel(hCombo);
	if(nIdx == CB_ERR) return RGB(0, 0, 0);

	return ((COLORREF)ComboBox_GetItemData(hCombo, nIdx));
}
