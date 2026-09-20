/*------------------------------------------------------------------------
	CBmpToolbar.h
------------------------------------------------------------------------*/
#ifndef _CBMPTOOLBAR_H
#define _CBMPTOOLBAR_H

#ifndef _INC_WINDOWS
	#include <windows.h>
#endif
#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif
#include "CImgListPng.h"

#define TOOLBAR_STYLE (DWORD)WS_CHILD
//TBSTYLE_FLAT | CCS_NORESIZE | CCS_NODIVIDER

#ifndef BTNS_WHOLEDROPDOWN
#define BTNS_WHOLEDROPDOWN				0x0080
#endif	//BTNS_WHOLEDROPDOWN

#define MAX_TB_CAP						256

#ifndef TTS_BALLOON						// バルーンチップ実装
	#define TTS_BALLOON					0x40
#endif
// -- ie5.0以降 ----
#ifndef TBSTYLE_EX_MIXEDBUTTONS
	#define TBSTYLE_EX_MIXEDBUTTONS		0x00000008
#endif
#ifndef BTNS_SHOWTEXT
	#define BTNS_SHOWTEXT				0x0040
#endif
// -----------------

#define MAX_TIPSTRING					256
#define MAX_TB_STRING					256

#define TB_IMG_ENABLE					0
#define TB_IMG_DISABLE					1
#define TB_IMG_HOT						2

class CBmpToolbar
{
	HINSTANCE m_hInst;
	HWND m_hParent, m_hTooltips;
	int m_nBtn;
	SIZE m_SizeBtn;
	LPTBBUTTON m_lpTbBtn;
	char m_szCaption[MAX_TB_CAP];
	SIZE m_SizeBmp;

	HIMAGELIST MakeImageList(int nWidth, HBITMAP hBmp, HBITMAP hMask);
	BOOL SetToolbarImage(HIMAGELIST hImgLst, const int nImgFlg);
	BOOL MakeTooltip(BOOL bUseBalloon);

public:
	HWND m_hToolbar;
	UINT m_uID;
	RECT m_rc;

	CBmpToolbar(HINSTANCE hCurInst, HWND hWnd, LPCTSTR lpcsCaption);
	~CBmpToolbar();

	void InitWin32Commctrl(DWORD dwICC = ICC_BAR_CLASSES);
	void MakeToolbar(UINT uiId, DWORD dwAddStyle);
	void SetImageList(int nWidth, WORD wBmp, WORD wMask, const int nImgFlg = TB_IMG_ENABLE);
	void SetImageList(int nWidth, HINSTANCE hResInst, WORD wBmp, WORD wMask, const int nImgFlg = TB_IMG_ENABLE);
	void SetImageList(int nWidth, LPCTSTR lpcBmp, LPCTSTR lpcMask, const int nImgFlg = TB_IMG_ENABLE);
	int AddBtn(LPTBBUTTON lpbtninfo, const int nBtn = 0);
	void InsSep(int nIdx);

	void SetIndent(int nIndent);
	int AddString(UINT uiStringRes);
	int AddString(LPCTSTR lpcString);
	void SetString(int nId, UINT uiStringRes);
	void SetString(int nId, char *pszString);
	void SetBtnWidth(int nId, int nWidth);
	void EnableButton(UINT uiId, BOOL bEnable);
	void CheckedButton(UINT uiId, UINT uiChecked);
	DWORD IsCheck(UINT uiId);
	void PressedButton(UINT uiId, BOOL bPressed);
	DWORD IsPressed(UINT uiId);

	void SetTooltips(const char *pszTipText[], BOOL bUseBalloon = FALSE);
	void SetTooltips(UINT uiID, const char *pszTipText, BOOL bUseBalloon = FALSE);

	LRESULT Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);		// WM_SIZEに応答
	void GetRect(LPRECT lprc) { CopyMemory(lprc, &m_rc, sizeof(RECT)); };
	void GetBtnSize(LPSIZE lpSize){ CopyMemory(lpSize, &m_SizeBtn, sizeof(SIZE)); };
	int GetBtnCount() { return (int)SendMessage(m_hToolbar, TB_BUTTONCOUNT , 0L, 0L); };
	int GetBtnHeight(int nIdx);
	int GetBtnWidth(int nIdx);
	void GetWndSize(LPSIZE lpSize);
	UINT GetID() { return m_uID; };

	void ShowToolbar() { ShowWindow(m_hToolbar, SW_SHOW); };
	void HideToolbar() { ShowWindow(m_hToolbar, SW_HIDE); };

	HIMAGELIST MakeImageList32(int nWidth, HBITMAP hBmp32);

	void SetPngImageList(
		int nWidth,
		UINT uiPngRes,
		const int nImgFlg = TB_IMG_ENABLE);
};

#endif