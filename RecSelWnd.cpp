/*----------------------------------------------------------------------------------------

	RecSelWnd.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "MainWnd.h"
#include "RecSelWnd.h"
#include "resource.h"

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HWND ghMainWnd = NULL;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
static LRESULT OnCreateRecSelWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT OnSizeRecSelWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

/*----------------------------------------------------------------------------------------
	アウトプットウィンドウプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK RecSelSplitWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_CREATE:
		ghMainWnd = (HWND)(((LPCREATESTRUCT)lp)->lpCreateParams);
		return (OnCreateRecSelWnd(ghMainWnd, hWnd, msg, wp, lp));
	case WM_SIZE:
		return (OnSizeRecSelWnd(ghMainWnd, hWnd, msg, wp, lp));
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ウィンドウ作成完了通知
----------------------------------------------------------------------------------------*/
LRESULT OnCreateRecSelWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return (DefWindowProc(hWnd, msg, wp, lp));
}

/*----------------------------------------------------------------------------------------
	サイズ変更通知
----------------------------------------------------------------------------------------*/
LRESULT OnSizeRecSelWnd(HWND hMainWnd, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return (DefWindowProc(hWnd, msg, wp, lp));
}
