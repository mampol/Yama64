/*---------------------------------------------------------------------
	CWndSplitter.cpp
	２ペインのウィンドウを作成
---------------------------------------------------------------------*/
#include "CWndSplitter.h"

// コンストラクタ
CWndSplitter::CWndSplitter()
 : m_bInitialize(FALSE), m_nPainSizeEx(0)
{
}

CWndSplitter::CWndSplitter(HINSTANCE hCurInst,
						   HWND hWnd,
						   DWORD dwID,
						   DWORD dwPos,
						   BOOL bFlatStyle/* = FALSE*/,
						   DWORD dwStyle/* = 0*/,
						   DWORD dwExStyle/* = 0*/)
 : m_bInitialize(FALSE),
	m_nPainSizeEx(0)
{
	Initialize(hCurInst,
		hWnd,
		dwID,
		dwPos,
		bFlatStyle,
		dwStyle,
		dwExStyle);
}

// デストラクタ
CWndSplitter::~CWndSplitter()
{
	if(m_hRubber) DeleteObject(m_hRubber);
	if(m_hSplit) DestroyWindow(m_hSplit);
	if(m_hPain) DestroyWindow(m_hPain);
}

void CWndSplitter::Initialize(HINSTANCE hCurInst,
							  HWND hWnd,
							  DWORD dwID,
							  DWORD dwPos,
							  BOOL bFlatStyle/* = FALSE*/,
							  DWORD dwStyle/* = 0*/,
							  DWORD dwExStyle/* = 0*/)
{
	if(m_bInitialize) return;
	m_hInst = hCurInst;
	m_hParent = hWnd;
	m_hWndDesktop = GetDesktopWindow();
	m_dwSplitID = dwID;
	m_dwSplitterPos = dwPos;
	m_bFlat = bFlatStyle;
	m_dwSplitStyle = dwStyle;
	m_dwExSplitStyle = dwExStyle;
	SetRect(&m_rcIndent, 0, 0, 0, 0);
	SetRect(&m_rcSizeLimit, 0, 0, 0, 0);
	SetMetrics();
	m_nShowWnd = 0;
	m_bInitialize = TRUE;
}

// ウィンドウクラス登録
BOOL CWndSplitter::RegistClass(LPCTSTR lpcWndClass,
							   WNDPROC WndProc,
							   HBRUSH hBrush,
							   HCURSOR hCursor)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = m_hInst;
	wc.hIcon = (HICON)LoadImage(NULL,
		MAKEINTRESOURCE(IDI_APPLICATION),
		IMAGE_ICON,
		0, 0,
		LR_DEFAULTCOLOR | LR_SHARED);
	wc.hCursor = hCursor;
	wc.hbrBackground = hBrush;
	wc.lpszClassName = lpcWndClass;
	wc.lpszMenuName = NULL;
	if(!RegisterClassEx(&wc)) return FALSE;
	return TRUE;
}

// ペインウィンドウ作成
BOOL CWndSplitter::CreatePain(void)
{
	m_hPain = CreateWindowEx(0,
		m_szPainWndClass,
		"CWndSplitter_PainWnd",
		WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0,
		0, 0,
		m_hParent,
		(HMENU)m_dwSplitID + 1,
		m_hInst,
		(LPVOID)this);
	if(m_hPain == NULL) return FALSE;
//	DWORD dwParentStyle = GetWindowLong(m_hParent, GWL_STYLE);
	DWORD dwParentStyle = (DWORD)GetWindowLongPtr(m_hParent, GWL_STYLE);
	if(!(dwParentStyle & WS_CLIPSIBLINGS)){
		dwParentStyle |= WS_CLIPSIBLINGS;
		SetWindowLong(m_hParent, GWL_STYLE, (LONG)dwParentStyle);
	}
	m_nShowWnd = SW_HIDE;
	return TRUE;
}

// ペインウィンドウプロシジャー
LRESULT CALLBACK CWndSplitter::SplitPainWndProc(HWND hWndPain, UINT msg, WPARAM wp, LPARAM lp)
{
	CWndSplitter *lpWndSplit;
	RECT rc;
	int nSize[2];
	HDC hDC;
	PAINTSTRUCT ps;
	POINT pt;

	switch(msg){
	case WM_CREATE:
		lpWndSplit = (CWndSplitter *)(((LPCREATESTRUCT)lp)->lpCreateParams);
		// スプリットウィンドウ作成
		if(!lpWndSplit->RegistClass(lpWndSplit->m_szSplitWndClass,
									lpWndSplit->m_SplitWndProc,
									(HBRUSH)(COLOR_WINDOW + 1),
									lpWndSplit->m_hCurWnd)) return -1;
		if(!lpWndSplit->CreateSplit(hWndPain, lpWndSplit->m_szSplitWndClass)) return -1;
		// ハンドルにインスタンス埋め込み
		SetProp(hWndPain, STR_SPLITPROP, (HANDLE)lpWndSplit);
		break;
	// サイズ変更
	case WM_SIZE:
		// インスタンス取得
		if(!(lpWndSplit = (CWndSplitter *)GetProp(hWndPain, STR_SPLITPROP))) return (DefWindowProc(hWndPain, msg, wp, lp));
		// RECT構造体取得
		GetClientRect(hWndPain, &rc);
		// スプリッタ(子供)ウィンドウのサイズ変更
		switch(lpWndSplit->m_dwSplitterPos){
		case SSPLITTER_LEFT:
//			lpWndSplit->m_nPainSize = (int)LOWORD(lp);
//			MoveWindow(lpWndSplit->m_hSplit,
//				0,
//				0,
//				LOWORD(lp) - (lpWndSplit->m_nEdgeX + 1),
//				HIWORD(lp),
//				TRUE);
			lpWndSplit->m_nPainSize = rc.right;
			MoveWindow(lpWndSplit->m_hSplit,
				0,
				0,
				rc.right - (lpWndSplit->m_nEdgeX + 1),
				rc.bottom,
				TRUE);
			break;
		case SSPLITTER_TOP:
//			lpWndSplit->m_nPainSize = (int)HIWORD(lp);
//			MoveWindow(lpWndSplit->m_hSplit,
//				0,
//				0,
//				LOWORD(lp),
//				HIWORD(lp) - (lpWndSplit->m_nEdgeY + 1),
//				TRUE);
			lpWndSplit->m_nPainSize = rc.bottom;
			MoveWindow(lpWndSplit->m_hSplit,
				0,
				0,
				rc.right,
				rc.bottom - (lpWndSplit->m_nEdgeY + 1),
				TRUE);
			break;
		case SSPLITTER_RIGHT:
//			lpWndSplit->m_nPainSize = (int)LOWORD(lp);
//			MoveWindow(lpWndSplit->m_hSplit,
//				lpWndSplit->m_nEdgeX + 1,
//				0,
//				LOWORD(lp) - (lpWndSplit->m_nEdgeX + 1),
//				HIWORD(lp),
//				TRUE);
			lpWndSplit->m_nPainSize = rc.right;
			MoveWindow(lpWndSplit->m_hSplit,
				lpWndSplit->m_nEdgeX + 1,
				0,
				rc.right - (lpWndSplit->m_nEdgeX + 1),
				rc.bottom,
				TRUE);
			break;
		case SSPLITTER_BOTTOM:
//			lpWndSplit->m_nPainSize = (int)HIWORD(lp);
//			MoveWindow(lpWndSplit->m_hSplit,
//				0,
//				lpWndSplit->m_nEdgeY + 1,
//				LOWORD(lp),
//				HIWORD(lp) - (lpWndSplit->m_nEdgeY + 1),
//				TRUE);
			lpWndSplit->m_nPainSize = rc.bottom;
			MoveWindow(lpWndSplit->m_hSplit,
				0,
				lpWndSplit->m_nEdgeY + 1,
				rc.right,
				rc.bottom - (lpWndSplit->m_nEdgeY + 1),
				TRUE);
			break;
		}
		// ペインウィンドウの親に通知
		SendMessage(lpWndSplit->m_hParent,
			WM_RESTORED_PAIN,
			(WPARAM)lpWndSplit->m_hSplit,
			MAKELPARAM(rc.right, rc.bottom));
		break;
	// ペイン境界線描画
	case WM_PAINT:
		// インスタンス取得
		if(!(lpWndSplit = (CWndSplitter *)GetProp(hWndPain, STR_SPLITPROP))) return (DefWindowProc(hWndPain, msg, wp, lp));
		// 描画
		hDC = BeginPaint(hWndPain, &ps);
		GetClientRect(hWndPain, &rc);
		switch(lpWndSplit->m_dwSplitterPos){
		case SSPLITTER_LEFT:
			DrawEdge(hDC, &rc,
				EDGE_ETCHED, BF_RIGHT | (lpWndSplit->m_bFlat ? BF_FLAT : 0));
			break;
		case SSPLITTER_TOP:
			DrawEdge(hDC, &rc,
				EDGE_ETCHED, BF_BOTTOM | (lpWndSplit->m_bFlat ? BF_FLAT : 0));
			break;
		case SSPLITTER_RIGHT:
			DrawEdge(hDC, &rc,
				EDGE_ETCHED, BF_LEFT | (lpWndSplit->m_bFlat ? BF_FLAT : 0));
			break;
		case SSPLITTER_BOTTOM:
			DrawEdge(hDC, &rc,
				EDGE_ETCHED, BF_TOP | (lpWndSplit->m_bFlat ? BF_FLAT : 0));
			break;
		}
		EndPaint(hWndPain, &ps);
		break;
	// サイズ変更開始
	case WM_LBUTTONDOWN:
		// インスタンス取得
		if(!(lpWndSplit = (CWndSplitter *)GetProp(hWndPain, STR_SPLITPROP))) return (DefWindowProc(hWndPain, msg, wp, lp));
		// マウスキャプチャー開始
		SetCapture(hWndPain);
		//// 位置を退避
		//GetWindowRect(hWndPain, &(lpWndSplit->m_rcFrom));
		//CopyMemory(&(lpWndSplit->m_rcTo), &(lpWndSplit->m_rcFrom), sizeof(RECT));
		lpWndSplit->m_bDragging = true;
		POINT pt;
		GetCursorPos(&pt);
		RECT rcWnd;
		GetWindowRect(lpWndSplit->m_hParent, &rcWnd);
		pt.x -= rcWnd.left;
		pt.y -= rcWnd.top;
		switch (lpWndSplit->m_dwSplitterPos) {
		case SSPLITTER_LEFT:
		case SSPLITTER_RIGHT:
			lpWndSplit->m_nDragPos = pt.x;
			lpWndSplit->DrawRubberLineV(lpWndSplit->m_nDragPos);
			break;
		case SSPLITTER_TOP:
		case SSPLITTER_BOTTOM:
			lpWndSplit->m_nDragPos = pt.y;
			lpWndSplit->DrawRubberLineH(lpWndSplit->m_nDragPos);
			break;
		}
		lpWndSplit->SetLimit();
		//// デスクトップの更新をロック
		//LockWindowUpdate(lpWndSplit->GetDesktopHandle());
		//// バンド描画
		//lpWndSplit->DrawSplitter(&(lpWndSplit->m_rcTo));
		break;
	// サイズ変更中
	case WM_MOUSEMOVE:
		if(GetCapture() != hWndPain) break;
		// インスタンス取得
		if(!(lpWndSplit = (CWndSplitter *)GetProp(hWndPain, STR_SPLITPROP))) return (DefWindowProc(hWndPain, msg, wp, lp));
		if (lpWndSplit->m_bDragging) {
			// 古いラバーを消す
			switch (lpWndSplit->m_dwSplitterPos) {
			case SSPLITTER_LEFT:
			case SSPLITTER_RIGHT:
				lpWndSplit->DrawRubberLineV(lpWndSplit->m_nDragPos);
				break;
			case SSPLITTER_TOP:
			case SSPLITTER_BOTTOM:
				lpWndSplit->DrawRubberLineH(lpWndSplit->m_nDragPos);
				break;
			}
			// 位置更新
			POINT pt;
			GetCursorPos(&pt);
			RECT rcWnd;
			GetWindowRect(lpWndSplit->m_hParent, &rcWnd);
			pt.x -= rcWnd.left;
			pt.y -= rcWnd.top;
			// 新しい位置に描画
			switch (lpWndSplit->m_dwSplitterPos) {
			case SSPLITTER_LEFT:
			case SSPLITTER_RIGHT:
				lpWndSplit->m_nDragPos = pt.x;
				if (lpWndSplit->m_nDragPos < lpWndSplit->m_rcSizeLimit.left)
					lpWndSplit->m_nDragPos = lpWndSplit->m_rcSizeLimit.left;
				if (lpWndSplit->m_nDragPos > lpWndSplit->m_rcSizeLimit.right)
					lpWndSplit->m_nDragPos = lpWndSplit->m_rcSizeLimit.right;
				lpWndSplit->DrawRubberLineV(lpWndSplit->m_nDragPos);
				break;
			case SSPLITTER_TOP:
			case SSPLITTER_BOTTOM:
				lpWndSplit->m_nDragPos = pt.y;
				if (lpWndSplit->m_nDragPos < lpWndSplit->m_rcSizeLimit.top)
					lpWndSplit->m_nDragPos = lpWndSplit->m_rcSizeLimit.top;
				if (lpWndSplit->m_nDragPos > lpWndSplit->m_rcSizeLimit.bottom)
					lpWndSplit->m_nDragPos = lpWndSplit->m_rcSizeLimit.bottom;
				lpWndSplit->DrawRubberLineH(lpWndSplit->m_nDragPos);
				break;
			}

		}
		//// サイズをチェックしながら新しいラバーを描画
		//GetWindowRect(lpWndSplit->m_hParent, &rc);
		//GetCursorPos(&pt);
		//CopyMemory((LPRECT)&(lpWndSplit->m_rcFrom),
		//	(LPRECT)&(lpWndSplit->m_rcTo),
		//	sizeof(RECT));
		/*
		switch(lpWndSplit->m_dwSplitterPos){
		case SSPLITTER_LEFT:
			lpWndSplit->m_rcTo.right = pt.x;
			nSize[0] = rc.left + (lpWndSplit->m_nEdgeX + lpWndSplit->m_rcSizeLimit.right);
			nSize[1] = rc.right - (lpWndSplit->m_nEdgeX + lpWndSplit->m_rcSizeLimit.left);
			if(nSize[0] > pt.x){	// 左限界
				lpWndSplit->m_rcTo.right = nSize[0];
			}else
			if(nSize[1] < pt.x){	// 右限界
				lpWndSplit->m_rcTo.right = nSize[1];
			}
			break;
		case SSPLITTER_TOP:
			lpWndSplit->m_rcTo.bottom = pt.y;
			nSize[0] = rc.top + (lpWndSplit->m_nEdgeY + lpWndSplit->m_rcSizeLimit.top);
			nSize[1] = rc.bottom - (lpWndSplit->m_nEdgeY + lpWndSplit->m_rcSizeLimit.bottom);
			if(nSize[0] > pt.y){	// 上限界
				lpWndSplit->m_rcTo.bottom = nSize[0];
			}else
			if(nSize[1] < pt.y){	// 下限界
				lpWndSplit->m_rcTo.bottom = nSize[1];
			}
			break;
		case SSPLITTER_RIGHT:
			lpWndSplit->m_rcTo.left = pt.x;
			nSize[0] = rc.left + (lpWndSplit->m_nEdgeX + lpWndSplit->m_rcSizeLimit.right);
			nSize[1] = rc.right - (lpWndSplit->m_nEdgeX + lpWndSplit->m_rcSizeLimit.left) - SIZE_LIMIT;
			if(nSize[0] > pt.x){	// 左限界
				lpWndSplit->m_rcTo.left = nSize[0];
			}else
			if(nSize[1] < pt.x){	// 右限界
				lpWndSplit->m_rcTo.left = nSize[1];
			}
			break;
		case SSPLITTER_BOTTOM:
			lpWndSplit->m_rcTo.top = pt.y;
			nSize[0] = rc.top + (lpWndSplit->m_nEdgeY + lpWndSplit->m_rcSizeLimit.top);
			nSize[1] = rc.bottom - (lpWndSplit->m_nEdgeY + lpWndSplit->m_rcSizeLimit.bottom) - SIZE_LIMIT;
			if(nSize[0] > pt.y){	// 上限界
				lpWndSplit->m_rcTo.top = nSize[0];
			}else
			if(nSize[1] < pt.y){	// 下限界
				lpWndSplit->m_rcTo.top = nSize[1];
			}
			break;
		}
		//// バンド消去
		//lpWndSplit->DrawSplitter(&(lpWndSplit->m_rcFrom));
		//// バンド描画
		//lpWndSplit->DrawSplitter(&(lpWndSplit->m_rcTo));
		// サイズ変更
		lpWndSplit->Screen2Client(&(lpWndSplit->m_rcTo), &rc);
		switch (lpWndSplit->m_dwSplitterPos) {
		case SSPLITTER_LEFT:
			MoveWindow(hWndPain,
				rc.left,
				rc.top,
				(rc.right - rc.left) + lpWndSplit->m_nEdgeX,
				rc.bottom - rc.top,
				TRUE);
			break;
		case SSPLITTER_TOP:
			MoveWindow(hWndPain,
				rc.left,
				rc.top,
				rc.right - rc.left,
				(rc.bottom - rc.top) + lpWndSplit->m_nEdgeY,
				TRUE);
			break;
		case SSPLITTER_RIGHT:
			MoveWindow(hWndPain,
				rc.left,
				rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				TRUE);
			break;
		case SSPLITTER_BOTTOM:
			MoveWindow(hWndPain,
				rc.left,
				rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				TRUE);
			break;
		}
		*/
		break;
	// サイズ変更終了
	case WM_LBUTTONUP:
	{
		if (GetCapture() != hWndPain) break;

		if (!(lpWndSplit = (CWndSplitter*)GetProp(hWndPain, STR_SPLITPROP)))
		{
			return DefWindowProc(hWndPain, msg, wp, lp);
		}

		if (lpWndSplit->m_bDragging)
		{
			// m_nDragPos(Window座標) → Client座標へ変換

			RECT rcWindow;
			GetWindowRect(lpWndSplit->m_hParent, &rcWindow);

			POINT ptClient = { 0, 0 };
			ClientToScreen(lpWndSplit->m_hParent, &ptClient);

			int clientOffsetX = ptClient.x - rcWindow.left;
			int clientOffsetY = ptClient.y - rcWindow.top;

			int splitX =
				lpWndSplit->m_nDragPos - clientOffsetX;

			int splitY =
				lpWndSplit->m_nDragPos - clientOffsetY;

			// 最後のラバーを消す
			switch (lpWndSplit->m_dwSplitterPos)
			{
			case SSPLITTER_LEFT:
			case SSPLITTER_RIGHT:
				lpWndSplit->DrawRubberLineV(
					lpWndSplit->m_nDragPos);
				break;

			case SSPLITTER_TOP:
			case SSPLITTER_BOTTOM:
				lpWndSplit->DrawRubberLineH(
					lpWndSplit->m_nDragPos);
				break;
			}

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(lpWndSplit->m_hParent, &pt);

			RECT rc;
			GetWindowRect(hWndPain, &rc);
			lpWndSplit->Screen2Client(&rc);

			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			switch (lpWndSplit->m_dwSplitterPos)
			{
			case SSPLITTER_LEFT:
			{
				int newWidth =
					splitX - rc.left;

				MoveWindow(
					hWndPain,
					rc.left,
					rc.top,
					newWidth,
					height,
					TRUE);

				break;
			}

			case SSPLITTER_RIGHT:
			{
				int newX = splitX;

				int newWidth =
					rc.right - newX;

				MoveWindow(
					hWndPain,
					newX,
					rc.top,
					newWidth,
					height,
					TRUE);

				break;
			}

			case SSPLITTER_TOP:
			{
				int newHeight =
					splitY - rc.top;

				MoveWindow(
					hWndPain,
					rc.left,
					rc.top,
					width,
					newHeight,
					TRUE);

				break;
			}

			case SSPLITTER_BOTTOM:
			{
				int newY = splitY;

				int newHeight =
					rc.bottom - newY;

				MoveWindow(
					hWndPain,
					rc.left,
					newY,
					width,
					newHeight,
					TRUE);

				break;
			}
			}
			ReleaseCapture();
			lpWndSplit->m_bDragging = false;
		}
		break;
	}
	case WM_DESTROY:
		if(GetProp(hWndPain, STR_SPLITPROP)) RemoveProp(hWndPain, STR_SPLITPROP);
		break;

	default:
		return (DefWindowProc(hWndPain, msg, wp, lp));
	}

	return (0L);
}

// スプリッターウィンドウ作成
BOOL CWndSplitter::CreateSplit(HWND hPain, LPCTSTR lpcSplitWndClass)
{
	m_hSplit = CreateWindowEx(m_dwExSplitStyle,
		lpcSplitWndClass,
		"CWndSplitter_SplitWnd",
		WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | m_dwSplitStyle,
		0, 0,
		0, 0,
		hPain,
		(HMENU)m_dwSplitID,
		m_hInst,
		m_lpParam);
	if(m_hSplit == NULL) return FALSE;
	ShowWindow(m_hSplit, SW_SHOW);
	UpdateWindow(m_hSplit);
	return TRUE;
}

//// ラバーの描画
//void CWndSplitter::DrawSplitter(const LPRECT lprcDraw)
//{
//	// インデント考慮
//	RECT rcDraw;
//	CopyMemory(&rcDraw, lprcDraw, sizeof(RECT));
//
//	// デスクトップのＤＣの取得
//	HDC hScreenDc, hBackBuffer;
//	int i;
//	RECT rc;
//	GetClientRect(m_hParent, &rc);
//	HRGN hRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
//	if(GetWindowRgn(m_hParent, hRgn) == ERROR){
//		DeleteObject(hRgn);
//		return;
//	}
////	if(m_dwPlatformId == VER_PLATFORM_WIN32_NT){
////		hScreenDc = GetDCEx(m_hWndDesktop, hRgn, DCX_LOCKWINDOWUPDATE);
////	}else{
////		hScreenDc = GetDCEx(m_hWndDesktop, hRgn, DCX_PARENTCLIP | DCX_LOCKWINDOWUPDATE);
////	}
//	hScreenDc = GetDCEx(m_hWndDesktop, hRgn, DCX_LOCKWINDOWUPDATE);
//	DeleteObject(hRgn);
//	int nx = GetSystemMetrics(SM_CXSCREEN), xy = GetSystemMetrics(SM_CYSCREEN);
//	hBackBuffer = CreateCompatibleDC(hScreenDc);
//	HBITMAP hOldBmp = (HBITMAP)SelectObject(hBackBuffer, m_hRubber);
//
//	// ラバーバンドの描画
//	switch(m_dwSplitterPos){
//	case SSPLITTER_LEFT:
//		for(i = rcDraw.top; i < rcDraw.bottom; i += m_nEdgeY){
//			BitBlt(hScreenDc,
//				rcDraw.right, (i < rcDraw.bottom ? i : rcDraw.bottom),
//				m_nEdgeX, m_nEdgeY,
//				hBackBuffer,
//				0, 0,
//				SRCINVERT);
//		}
//		break;
//	case SSPLITTER_TOP:
//		for(i = rcDraw.left; i < rcDraw.right; i += m_nEdgeX){
//			BitBlt(hScreenDc,
//				(i < rcDraw.right ? i : rcDraw.right), rcDraw.bottom,
//				m_nEdgeX, m_nEdgeY,
//				hBackBuffer,
//				0, 0,
//				SRCINVERT);
//		}
//		break;
//	case SSPLITTER_RIGHT:
//		for(i = rcDraw.top; i < rcDraw.bottom; i += m_nEdgeY){
//			BitBlt(hScreenDc,
//				rcDraw.left, (i < rcDraw.bottom ? i : rcDraw.bottom),
//				m_nEdgeX, m_nEdgeY,
//				hBackBuffer,
//				0, 0,
//				SRCINVERT);
//		}
//		break;
//	case SSPLITTER_BOTTOM:
//		for(i = rcDraw.left; i < rcDraw.right; i += m_nEdgeX){
//			BitBlt(hScreenDc,
//				(i < rcDraw.right ? i : rcDraw.right), rcDraw.top,
//				m_nEdgeX, m_nEdgeY,
//				hBackBuffer,
//				0, 0,
//				SRCINVERT);
//		}
//		break;
//	}
//
//	SelectObject(hBackBuffer, hOldBmp);
//	DeleteDC(hBackBuffer);
//	ReleaseDC(m_hWndDesktop, hScreenDc);
//}
void CWndSplitter::DrawRubberLineV(int x)
{
	HDC hdc = GetWindowDC(m_hParent);

	RECT rc;
	GetClientRect(m_hParent, &rc);

	PatBlt(
		hdc,
		x,
		rc.top + 10,
		EDGE_X,
		rc.bottom - rc.top - 6,
		DSTINVERT
	);

	ReleaseDC(m_hParent, hdc);
}

void CWndSplitter::DrawRubberLineH(int y)
{
	HDC hdc = GetWindowDC(m_hParent);

	RECT rc;
	GetClientRect(m_hParent, &rc);

	PatBlt(
		hdc,
		rc.left + 10,
		y,
		rc.right - rc.left - 6,
		EDGE_Y,
		DSTINVERT
	);

	ReleaseDC(m_hParent, hdc);
}

// スクリーンRECTをクライアントRECTへ
void CWndSplitter::Screen2Client(LPRECT lprc)
{
	POINT pt;

	pt.x = lprc->left;
	pt.y = lprc->top;
	ScreenToClient(m_hParent, &pt);
	lprc->left = pt.x;
	lprc->top = pt.y;

	pt.x = lprc->right;
	pt.y = lprc->bottom;
	ScreenToClient(m_hParent, &pt);
	lprc->right = pt.x;
	lprc->bottom = pt.y;
}

void CWndSplitter::Screen2Client(LPRECT lprcin, LPRECT lprcout)
{
	POINT pt;

	pt.x = lprcin->left;
	pt.y = lprcin->top;
	ScreenToClient(m_hParent, &pt);
	lprcout->left = pt.x;
	lprcout->top = pt.y;

	pt.x = lprcin->right;
	pt.y = lprcin->bottom;
	ScreenToClient(m_hParent, &pt);
	lprcout->right = pt.x;
	lprcout->bottom = pt.y;
}

// クライアントRECTをスクリーンRECTへ
void CWndSplitter::Client2Screen(LPRECT lprc)
{
	POINT pt;

	pt.x = lprc->left;
	pt.y = lprc->top;
	ClientToScreen(m_hParent, &pt);
	lprc->left = pt.x;
	lprc->top = pt.y;

	pt.x = lprc->right;
	pt.y = lprc->bottom;
	ClientToScreen(m_hParent, &pt);
	lprc->right = pt.x;
	lprc->bottom = pt.y;
}

// マトリクス設定
void CWndSplitter::SetMetrics(void)
{
//	// OSのバージョン
//	OSVERSIONINFO ovi;
//	ZeroMemory(&ovi, sizeof(OSVERSIONINFO));
//	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	GetVersionEx(&ovi);
//	m_dwPlatformId = ovi.dwPlatformId;

	// ラバーサイズ
	m_nEdgeX = EDGE_X;
	m_nEdgeY = EDGE_Y;

	// ラバーバンドに元になるビットマップ作成
	BYTE lpBits[] = {
		0xaa, 0xaa,
		0x55, 0x55,
		0xaa, 0xaa,
		0x55, 0x55
	};
	m_hRubber = CreateBitmap(m_nEdgeX,
						m_nEdgeY,
						1, 1, lpBits);
}

// スプリッター作成
HWND CWndSplitter::Create(LPCTSTR lpcSplitWndClass,
						  WNDPROC WndProc,
						  int nSize,
						  const int nIndextLeft/* = 0*/, const int nIndextTop/* = 0*/,
						  const int nIndextRight/* = 0*/, const int nIndextBottom/* = 0*/,
						  LPVOID lpParam/* = NULL*/,
						  HCURSOR hCursorWnd/* = NULL*/)
{
	// パラメータ
	wsprintf(m_szPainWndClass, CWNDSPLITTER_PAIN, lpcSplitWndClass);
//	strcpy(m_szSplitWndClass, lpcSplitWndClass);
	strcpy_s(m_szSplitWndClass, sizeof(m_szSplitWndClass), lpcSplitWndClass);
	m_SplitWndProc = WndProc;
	SetRect(&m_rcIndent,
		nIndextLeft, nIndextTop,
		nIndextRight, nIndextBottom);
	m_lpParam = lpParam;
	if(m_dwSplitID < 1) m_dwSplitID = CWNDSPLITTER_PAIN_ID;
	// ペインウィンドウ作成
	switch(m_dwSplitterPos){
	case SSPLITTER_LEFT:
	case SSPLITTER_RIGHT:
		m_hCurSize = (HCURSOR)LoadImage(NULL,
			MAKEINTRESOURCE(IDC_SIZEWE),
			IMAGE_CURSOR,
			0, 0,
			LR_DEFAULTCOLOR | LR_SHARED);
		break;
	case SSPLITTER_TOP:
	case SSPLITTER_BOTTOM:
		m_hCurSize = (HCURSOR)LoadImage(NULL,
			MAKEINTRESOURCE(IDC_SIZENS),
			IMAGE_CURSOR,
			0, 0,
			LR_DEFAULTCOLOR | LR_SHARED);
		break;
	}
	if(hCursorWnd == NULL){
		m_hCurWnd = (HCURSOR)LoadImage(NULL,
			MAKEINTRESOURCE(IDC_ARROW),
			IMAGE_CURSOR,
			0, 0,
			LR_DEFAULTCOLOR | LR_SHARED);
	}else{
		m_hCurWnd = hCursorWnd;
	}
	if(!RegistClass(m_szPainWndClass,
				SplitPainWndProc,
				(HBRUSH)(COLOR_BTNFACE+1),
				m_hCurSize)) return NULL;
	if(!CreatePain()) return NULL;
	// スプリッタ限界値設定
	SetLimit();
	// サイズ決定
	SetPainSize(nSize);
	return m_hSplit;
}

// スプリッタ移動限界
void CWndSplitter::SetLimit()
{
	// 親クライアント領域
	RECT rcClient;
	GetClientRect(m_hParent, &rcClient);

	// Client原点をScreen座標へ
	POINT ptLT = { rcClient.left, rcClient.top };
	POINT ptRB = { rcClient.right, rcClient.bottom };

	ClientToScreen(m_hParent, &ptLT);
	ClientToScreen(m_hParent, &ptRB);

	// 親WindowのScreen座標
	RECT rcWindow;
	GetWindowRect(m_hParent, &rcWindow);

	// Screen → Window座標
	ptLT.x -= rcWindow.left;
	ptLT.y -= rcWindow.top;

	ptRB.x -= rcWindow.left;
	ptRB.y -= rcWindow.top;

	// ラバーの移動範囲
	SetRect(
		&m_rcSizeLimit,

		ptLT.x + m_rcIndent.left + SIZE_LIMIT,

		ptLT.y + m_rcIndent.top + SIZE_LIMIT,

		ptRB.x - m_rcIndent.right
		- SIZE_LIMIT
		- EDGE_X,

		ptRB.y - m_rcIndent.bottom
		- SIZE_LIMIT
		- EDGE_Y
	);
}


// サイズ設定
void CWndSplitter::SetPainSize(int nSize)
{
	RECT rc;
	GetClientRect(m_hParent, &rc);
	switch(m_dwSplitterPos){
	case SSPLITTER_LEFT:
		SetRectSize(m_rcIndent.left,
			m_rcIndent.top,
			nSize,
			(m_nPainSizeEx > 0 ? m_nPainSizeEx : rc.bottom - (m_rcIndent.top + m_rcIndent.bottom)),
			SIZE_CLIENT);
		break;
	case SSPLITTER_TOP:
  		SetRectSize(m_rcIndent.left,
			m_rcIndent.top,
			(m_nPainSizeEx > 0 ? m_nPainSizeEx : rc.right - (m_rcIndent.left + m_rcIndent.right)),
			nSize,
			SIZE_CLIENT);
		break;
	case SSPLITTER_RIGHT:
		SetRectSize((rc.right - nSize) - m_rcIndent.right,
			m_rcIndent.top,
			nSize,
			(m_nPainSizeEx > 0 ? m_nPainSizeEx : rc.bottom - (m_rcIndent.top + m_rcIndent.bottom)),
			SIZE_CLIENT);
		break;
	case SSPLITTER_BOTTOM:
  		SetRectSize(m_rcIndent.left,
			(rc.bottom - nSize) - m_rcIndent.bottom,
			(m_nPainSizeEx > 0 ? m_nPainSizeEx : rc.right - (m_rcIndent.left + m_rcIndent.right)),
			nSize,
			SIZE_CLIENT);
		break;
	}
}

void CWndSplitter::SetPainSize()
{
	// ペインのサイズを更新
	int nSize = 0;
	RECT rc;
	GetClientRect(m_hPain, &rc);
	switch(m_dwSplitterPos){
	case SSPLITTER_LEFT:
	case SSPLITTER_RIGHT:
		nSize = rc.right;
		break;
	case SSPLITTER_TOP:
	case SSPLITTER_BOTTOM:
		nSize = rc.bottom;
		break;
	}
	SetPainSize(nSize);
}

// ウィンドウ表示
void CWndSplitter::Show()
{
	if(m_nShowWnd == SW_SHOW) return;
	ShowWindow(m_hPain, SW_SHOW);
	ShowWindow(m_hSplit, SW_SHOW);
	m_nShowWnd = SW_SHOW;
	RECT rc;
	GetClientRect(m_hPain, &rc);
	SendMessage(m_hParent,
		WM_RESTORED_PAIN,
		(WPARAM)m_hSplit,
		MAKELPARAM(rc.right, rc.bottom));
}

// ウィンドウ非表示
void CWndSplitter::Hide()
{
	if(m_nShowWnd == SW_HIDE) return;
	ShowWindow(m_hPain, SW_HIDE);
	ShowWindow(m_hSplit, SW_HIDE);
	m_nShowWnd = SW_HIDE;
	SendMessage(m_hParent,
		WM_RESTORED_PAIN,
		(WPARAM)m_hSplit,
		MAKELPARAM(0, 0));
}

// サイズ変更
void CWndSplitter::SetRectSize(int nLeft,
							   int nTop,
							   int nRight,
							   int nBottom,
							   DWORD dwFlag)
{
	RECT rc;
	SetRect(&rc, nLeft, nTop, nRight, nBottom);
	// サイズ変更
	SetRectSize(&rc, dwFlag);
}

// サイズ変更
void CWndSplitter::SetRectSize(LPRECT lprc, DWORD dwFlag)
{
	if(dwFlag == SIZE_SCREEN) Screen2Client(lprc);	// クライアント座標に
	// ペインサイズ変更
	MoveWindow(m_hPain,
		lprc->left, lprc->top, lprc->right, lprc->bottom, TRUE);
}

// サイズ取得
void CWndSplitter::GetRectSize(LPRECT lprc, DWORD dwFlag)
{
	// 非表示なら無条件で０
	if(m_nShowWnd == SW_HIDE){
		SetRect(lprc, 0, 0, 0, 0);
	}else{
		// ペインサイズ取得
		GetWindowRect(m_hPain, lprc);
		if(dwFlag == SIZE_CLIENT) Screen2Client(lprc);	// クライアント座標に
	}
}

// WM_SIZEメッセージに応答
LRESULT CWndSplitter::Resize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(!m_hPain) return (DefWindowProc(hWnd, msg, wp, lp));

	RECT rc;
//	GetRectSize(&rc, SIZE_CLIENT);
	GetClientRect(m_hPain, &rc);
	switch(m_dwSplitterPos){
	case SSPLITTER_LEFT:
	case SSPLITTER_RIGHT:
		SetPainSize(rc.right - rc.left);
		break;
	case SSPLITTER_TOP:
	case SSPLITTER_BOTTOM:
		SetPainSize(rc.bottom - rc.top);
		break;
	}
	return (0L);
}

// ペイン強制再描画
void CWndSplitter::Update(void)
{
	InvalidateRect(m_hPain, NULL, TRUE);
}

// 拡張ペインサイズ設定
// ペイン位置により意味が異なる
int CWndSplitter::SetPainSizeEx(const int nSizeEx)
{
	int nOldSizeEx = m_nPainSizeEx;
	m_nPainSizeEx = nSizeEx;
	SetPainSize();
	SendPainSizeRestor();
	return nOldSizeEx;
}

// スプリッターウィンドウに３Ｄ効果
// (ラバーの描画が遅くなる？何で？)
void CWndSplitter::DrawFrame(UINT uiEdge)
{
	RECT rc;
	HDC hDC = GetDC(m_hSplit);
	GetClientRect(m_hSplit, &rc);
	DrawEdge(hDC, &rc, uiEdge | BF_ADJUST, BF_RECT | (m_bFlat ? BF_FLAT : 0));
	ReleaseDC(m_hSplit, hDC);
}

// ペイン位置変更
void CWndSplitter::ChangePos(DWORD dwPos)
{
	if(!m_hPain) return;
	if(m_dwSplitterPos == dwPos) return;

	RECT rc;
	int nSize;
//	GetRectSize(&rc, SIZE_CLIENT);
	GetClientRect(m_hPain, &rc);
	BOOL bSendMsg = FALSE;
	if((m_dwSplitterPos == SSPLITTER_TOP)
	|| (m_dwSplitterPos == SSPLITTER_BOTTOM)){
		// サイズを取得
		nSize = rc.bottom - rc.top;
		if((dwPos == SSPLITTER_LEFT)
		|| (dwPos == SSPLITTER_RIGHT)){
			// カーソル変更
			m_hCurSize = (HCURSOR)LoadImage(NULL,
				MAKEINTRESOURCE(IDC_SIZEWE),
				IMAGE_CURSOR,
				0, 0,
				LR_DEFAULTCOLOR | LR_SHARED);
//			SetClassLong(m_hPain, GCL_HCURSOR, (LONG)m_hCurSize);
			SetClassLongPtr(m_hPain, GCLP_HCURSOR, 
				reinterpret_cast<LONG_PTR>(m_hCurSize));
		}else{
			bSendMsg = TRUE;
		}
	}else{
		// サイズを取得
		nSize = rc.right - rc.left;
		if((dwPos == SSPLITTER_TOP)
		|| (dwPos == SSPLITTER_BOTTOM)){
			// カーソル変更
			m_hCurSize = (HCURSOR)LoadImage(NULL,
				MAKEINTRESOURCE(IDC_SIZENS),
				IMAGE_CURSOR,
				0, 0,
				LR_DEFAULTCOLOR | LR_SHARED);
//			SetClassLong(m_hPain, GCL_HCURSOR, (LONG)m_hCurSize);
			SetClassLongPtr(m_hPain, GCLP_HCURSOR, 
				reinterpret_cast<LONG_PTR>(m_hCurSize));
		}else{
			bSendMsg = TRUE;
		}
	}
	// 位置変更
	m_dwSplitterPos = dwPos;
	// 大きさ変更
	SetPainSize(nSize);
	// 上<->下移動はサイズが変わらないのでWM_SIZE強制送信
	if(bSendMsg) SendPainSizeRestor();
}

// ペイン子ウィンドウのリサイズ送信
void CWndSplitter::SendPainSizeRestor()
{
	// WM_SIZE強制送信
	if(m_hPain) SendMessage(m_hPain, WM_SIZE, (WPARAM)SIZE_RESTORED, 0L);
}

// インデント設定
void CWndSplitter::SetIndents(const int nLeft, const int nTop,
							  const int nRight, const int nBottom)
{
	SetRect(&m_rcIndent,
		nLeft, nTop,
		nRight, nBottom);
	SetLimit();
	SetPainSize();
}

// メッセージ送信
LRESULT CWndSplitter::SendMessageSplit(UINT msg, WPARAM wp, LPARAM lp)
{
	if(!m_hSplit) return (0L);
	return (SendMessage(m_hSplit, msg, wp, lp));
}
