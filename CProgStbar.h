/*------------------------------------------------------------------------
	CProgStbar.h
------------------------------------------------------------------------*/
#ifndef _INC_CPROGSTBAR
#define _INC_CPROGSTBAR

#ifndef _INC_WINDOWS
	#include <windows.h>
#endif
#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif

#define STATUSBAR_STYLE			(DWORD)WS_CHILD | WS_VISIBLE | CCS_BOTTOM
#define PROGRESSBAR_STYLE		(DWORD)WS_CHILD | PBS_SMOOTH

#define MAX_ST_CAP				256
// ステータスバーパーツの最大数
#define MAX_PB_PARTS			50

#define STR_PROGSTBARCLASS		(LPCTSTR)"PROP_PROGSTBARCLASS"

class CProgStbar{
	HINSTANCE m_hInst;
	char m_szCaption[MAX_ST_CAP];
	HIMAGELIST m_hImgSt;
	int m_nIconWidth;
	WNDPROC m_OrgWndProc;

	HICON GetStIcon(int nImg);
	static LRESULT CALLBACK ChainStatusbarProc(HWND hStbar, UINT msg, WPARAM wp, LPARAM lp);

protected:
	HWND m_hParent;
	UINT m_uiSt, m_uiProg;
	int m_nPartsSize[MAX_PB_PARTS],
		m_nPartsStyle[MAX_PB_PARTS],
		m_nParts,
		m_nProgressPos,
		m_nFixedPartsSize[MAX_PB_PARTS];

	int GetPointParts(int x, int y);
	LRESULT ChainDefault(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT LButtonDown(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT LButtonUp(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT LButtonDblClk(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT RButtonDown(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT RButtonUp(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT RButtonDblClk(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT MButtonDown(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT MButtonUp(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT MButtonDblClk(HWND hSb, UINT msg, WPARAM wp, LPARAM lp);

public:
	HWND m_hSb, m_hProg;
	RECT m_Rc;

	CProgStbar(HINSTANCE hCurInst,
		HWND hWnd,
		UINT uiStID,
		UINT uiProgID,
		LPCTSTR lpcsCaption,
		const int *pnParts, const int nSize,
		const int nProgPos);
	~CProgStbar();

	void InitWin32Commctrl(DWORD dwICC = ICC_BAR_CLASSES | ICC_PROGRESS_CLASS);
	BOOL CreateBar(BOOL bProcChain = FALSE);
	void SetFixedParts(const int nParts, const int nSize);
	int SetStatusImage(UINT uiBmpRes,
		const int nImgCount,
		const int cx = 16, const int cy = 16,
		COLORREF colMaskColor = 0);
	void Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);		// WM_SIZEに応答
	void ReDraw();							// 強制描画
	void SetSbText(int nPart,
		UINT uiType,
		LPCTSTR lpsSbText,
		const int nImg = -1);
	void Invalidate();
	void GetRect(LPRECT lprc) { CopyMemory(lprc, &m_Rc, sizeof(RECT)); };

	void ShowProg(BOOL bShow);
	void SetProg(int nMin, int nMax, int nStep);
	void StepProg(void);
};

#endif
