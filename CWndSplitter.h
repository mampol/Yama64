/*---------------------------------------------------------------------
	CWndSplitter.h
---------------------------------------------------------------------*/
#ifndef _INC_CWNDSPLITTER_
#define _INC_CWNDSPLITTER_

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#define CWNDSPLITTER_PAIN						(LPCTSTR)"%s_CWndSplitter_Pain"
#define STR_SPLITPROP							(LPCTSTR)"SPLIT_PROPERTY"

#define CWNDSPLITTER_PAIN_ID					10001

#define SSPLITTER_LEFT							0
#define SSPLITTER_TOP							1
#define SSPLITTER_RIGHT							2
#define SSPLITTER_BOTTOM						3

#define SIZE_SCREEN								0
#define SIZE_CLIENT								1

#define SIZE_LIMIT								4		// スプリット移動限界
#define EDGE_X									4		// ラバー幅
#define EDGE_Y									4		// ラバー高さ

#define WM_RESTORED_PAIN						(WM_APP+300)

class CWndSplitter
{
private:
	BOOL m_bInitialize;
	HINSTANCE m_hInst;
	HWND m_hParent, m_hWndDesktop;
	HWND m_hPain, m_hSplit;
	DWORD m_dwSplitID;
	HCURSOR m_hCurSize, m_hCurWnd;
	char m_szPainWndClass[256], m_szSplitWndClass[256];
	WNDPROC m_SplitWndProc;
	DWORD m_dwSplitterPos;
	BOOL m_bFlat;
	DWORD m_dwSplitStyle, m_dwExSplitStyle;
//	DWORD m_dwPlatformId;
	int m_nPainSize, m_nEdgeX, m_nEdgeY, m_nPainSizeEx;
	RECT m_rcIndent;
	RECT m_rcSizeLimit;
//	RECT m_rcFrom, m_rcTo;
	HBITMAP m_hRubber;
	LPVOID m_lpParam;
	int m_nShowWnd;
	bool m_bDragging = false;
	int  m_nDragPos = 0;

	void SetMetrics(void);
	BOOL RegistClass(LPCTSTR lpcWndClass, WNDPROC WndProc, HBRUSH hBrush, HCURSOR hCursor);
	BOOL CreatePain(void);
	static LRESULT CALLBACK SplitPainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	BOOL CreateSplit(HWND hPain, LPCTSTR lpcSplitWndClass);
	void SetLimit();
	void SetPainSize(int nSize);
	void SetPainSize();
	//void DrawSplitter(const LPRECT lprcDraw);
	void DrawRubberLineV(int x);
	void DrawRubberLineH(int y);

	HWND GetParentHandle() { return m_hParent; };
	HWND GetDesktopHandle() { return m_hWndDesktop; };

	void Screen2Client(LPRECT lprc);
	void Screen2Client(LPRECT lprcin, LPRECT lprcout);
	void Client2Screen(LPRECT lprc);

public:
	CWndSplitter();
	CWndSplitter(HINSTANCE hCurInst,
		HWND hWnd,
		DWORD dwID,
		DWORD dwPos,
		BOOL bFlatStyle = FALSE,
		DWORD dwStyle = 0,
		DWORD dwExStyle = 0);
	virtual ~CWndSplitter();

	void Initialize(HINSTANCE hCurInst,
		HWND hWnd,
		DWORD dwID,
		DWORD dwPos,
		BOOL bFlatStyle = FALSE,
		DWORD dwStyle = 0,
		DWORD dwExStyle = 0);
	HWND Create(LPCTSTR lpcSplitWndClass,
		WNDPROC WndProc,
		int nSize,
		const int nIndextLeft = 0, const int nIndextTop = 0,
		const int nIndextRight = 0, const int nIndextBottom = 0,
		LPVOID lpParam = NULL,
		HCURSOR hCursorWnd = NULL);
	HWND GetWindowHandle() { return m_hSplit; }
	void Show();
	void Hide();
	int GetShow() { return m_nShowWnd; };
	void SetRectSize(int nLeft, int nTop, int nRight, int nBottom, DWORD dwFlag);
	void SetRectSize(LPRECT lprc, DWORD dwFlag);
	void GetRectSize(LPRECT lprc, DWORD dwFlag);
	LRESULT Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	void Update(void);
	void DrawFrame(UINT uiEdge);
	void ChangePos(DWORD dwPos);
	DWORD GetPos() { return m_dwSplitterPos; };
	int GetPainSize() { return m_nPainSize; };
	int SetPainSizeEx(const int nSizeEx);
	void SendPainSizeRestor();
	void SetIndents(const int nLeft, const int nTop, const int nRight, const int nBottom);
	LRESULT SendMessageSplit(UINT msg, WPARAM wp, LPARAM lp);
};

#endif	//_INC_CWNDSPLITTER_
