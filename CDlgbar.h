/*----------------------------------------------------------------------------------------

	CDlgbar.h

----------------------------------------------------------------------------------------*/
#ifndef _CDLGBAR_H
#define _CDLGBAR_H

#ifndef _INC_WINDOWS
	#include <windows.h>
#endif
#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif

#ifndef _VECTOR_
	#include <vector>
#endif

#ifndef _CBMPTOOLBAR_H
	#include "CBmpToolbar.h"
#endif

#define WM_DLGBAR_RELEASE			(WM_USER+0x800)
#define DEF_BAR_HIGHT				22

class CDlgbar{
	HINSTANCE m_hInst;
	HWND m_hParent, m_hRebar;
	HBITMAP m_hBmpBack;
	std::vector<CBmpToolbar *> m_vecBmptb;
	std::vector<HWND> m_vecDlgbar;

	BOOL PutRebar(HWND hBar,
		const char *pszcBandStr,
		BOOL bBreak, BOOL bUseGrop,
		UINT wID,
		const int cx = 0, const int cy = 0,
		const int nMin = 0);
	int FindToolbar(HWND hToolbar);
	int FindToolbar(UINT uiToolbar);
	int AdjustBandSize(const int i);

public:
	CDlgbar(HINSTANCE hCurInst, HWND hWnd);
	CDlgbar(HINSTANCE hCurInst, HWND hWnd, DWORD dwICC);
	~CDlgbar();

	void InitWin32Commctrl(DWORD dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES);
	int SetBmpBack(UINT unBmpRes);

	BOOL MakeRebar(UINT uiID, BOOL bLeftBar = FALSE);
	void ReflashBandSize(const int nCorrect);
	void ReflashBandSize(const int nBand, const int nCorrect);
	HWND AddToolbar(LPCTSTR lpcsCaption,
		BOOL bBreak, BOOL bUseGrop,
		UINT uiID,
		DWORD dwStyle,
		const int cx = 0, const int cy = 0,
		const int nMin = 0);
	int AddTbImageList(HWND hToolbar, const int nBtnSize, UINT uiBtnRes, UINT uiMaskRes);
	int AddTbPngImageList(HWND hToolbar, const int nBtnSize, UINT uiPngRes);
	int AddTbDisableImageList(HWND hToolbar, const int nBtnSize, UINT uiBtnRes, UINT uiMaskRes);
	int AddTbPngDisableImageList(HWND hToolbar, const int nBtnSize, UINT uiPngRes);
	int AddTbHotImageList(HWND hToolbar, const int nBtnSize, UINT uiBtnRes, UINT uiMaskRes);
	int AddTbPngHotImageList(HWND hToolbar, const int nBtnSize, UINT uiPngRes);
	int AddTbString(HWND hToolbar, UINT uiStringRes);
	int AddTbString(HWND hToolbar, LPCTSTR lpcString);
	int AddTbButton(HWND hToolbar, TBBUTTON *lpTbBtn, const int nBtn = 0);
	int AdjustBandSize(HWND hToolbar);
	void InsTbSeps(HWND hToolbar, const int nSeps[], const int nCount);
	void InsTbToolTips(HWND hToolbar, const char *pszTipText[], BOOL bUseBalloon = FALSE);
	void SetTbIndent(HWND hToolbar, int nIndent);
	void SetTbString(HWND hToolbar, int nId, UINT uiStringRes);
	void SetTbString(HWND hToolbar, int nId, char *pszString);
	void SetTbBtnWidth(HWND hToolbar, int nId, int nWidth);
	void EnableTbButton(HWND hToolbar, UINT uiId, BOOL bEnable);
	void CheckedTbButton(HWND hToolbar, UINT uiId, UINT uiChecked);
	DWORD IsTbCheck(HWND hToolbar, UINT uiId);
	void PressedTbButton(HWND hToolbar, UINT uiId, BOOL bPressed);
	DWORD IsTbPressed(HWND hToolbar, UINT uiId);

	HWND AddDlgbar(WORD wRes,
		DLGPROC dlgProc,
		const char *pszcBandStr,
		BOOL bBreak, BOOL bUseGrop,
		const int cx = 0, const int cy = 0,
		const int nMin = 0);
	HWND AddDlgbarParam(WORD wRes,
		DLGPROC dlgProc,
		LPARAM lp,
		const char *pszcBandStr,
		BOOL bBreak, BOOL bUseGrop,
		const int cx = 0, const int cy = 0,
		const int nMin = 0);

	HWND GetRebarHandle(void);
	HWND GetToolbarHandle(UINT uID);
	HWND GetDlgbarHandle(UINT wID);
	LRESULT Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);		// WM_SIZE‚É‰ž“š
	void HeightChange(void);				// WM_NOTIFY‚ÌRBN_HEIGHTCHANGE‚É‰ž“š
	void GetRect(LPRECT lprc);
	HWND GetDlgbarRect(unsigned int nDlgbar, LPRECT lprc);
	HWND GetDlgbarRect(HWND hDlgbar, LPRECT lprc);
	int GetId2Index(UINT uBandID);
	int ShowDlgbar(HWND hDlgbar, BOOL bShow);
	HWND ShowDlgbar(unsigned int nDlgbar, BOOL bShow);
};

#endif	//_CDLGBAR_H