/*----------------------------------------------------------------------------------------

	Common.h

----------------------------------------------------------------------------------------*/
#ifndef _INC_COMMON_H_
#define _INC_COMMON_H_

#ifndef _WIN32_WINNT
#define _WIN32_WINNT			0x501
#endif

#pragma comment(lib, "comctl32.lib")

//#define _WIN32_WINNT						0x501

#ifndef _INC_WINDOWS
	#include <windows.h>
#endif
#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif
#ifndef _SHLOBJ_H_
	#include <shlobj.h>
#endif	// _SHLOBJ_H_

/*----------------------------------------------------------------------------------------
	定義
----------------------------------------------------------------------------------------*/
#ifndef CSTR_LESS_THAN
	#define CSTR_LESS_THAN					1
#endif
#ifndef CSTR_EQUAL
	#define CSTR_EQUAL						2
#endif
#ifndef CSTR_GREATER_THAN
	#define CSTR_GREATER_THAN				3
#endif
#ifndef WM_THEMECHANGED
	#define WM_THEMECHANGED					0x031A
#endif

// 旧SDKは使用出来ない
#ifndef BIF_NEWDIALOGSTYLE
	#define BIF_NEWDIALOGSTYLE				0
#endif
#ifndef BIF_NONEWFOLDERBUTTON
	#define BIF_NONEWFOLDERBUTTON			0
#endif
#ifndef LVS_EX_DOUBLEBUFFER
	#define LVS_EX_DOUBLEBUFFER				0x00010000
#endif

#define CTRLSTYLE_SPIN						(DWORD)(WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_WRAP | UDS_NOTHOUSANDS)

typedef struct _tagCOMEDITBOXSHOWBALLOON
{
	HWND hEdit;
	char szTitle[64];
	char szText[256];
	int nIcon;
} COMEDITBOXSHOWBALLOON, *LPCOMEDITBOXSHOWBALLOON;

#if (_WIN32_WINNT < 0x501)
	#ifndef TTI_INFO
		#define TTI_INFO					MB_ICONINFORMATION
	#endif
	#ifndef TTI_WARNING
		#define TTI_WARNING					MB_ICONEXCLAMATION
	#endif
	#ifndef TTI_ERROR
		#define TTI_ERROR					MB_ICONSTOP
	#endif
#endif
/*------------------------------------------------------------------------------
	マクロ
------------------------------------------------------------------------------*/
#ifndef APP_NAME
	#define APP_NAME						(LPCTSTR)"CommonMessage"
#endif
#define ERRMSG(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL)
#define EXCMSG(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_OK | MB_ICONEXCLAMATION)
#define INFMSG(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_OK | MB_ICONINFORMATION)
#define INFMSGTOPMOST(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL)
#define YESNOMSG(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL)
#define YESNOMSG2(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL | MB_DEFBUTTON2)
#define YESNOEXC(hWnd, pszMsg)	\
	MessageBox((HWND)hWnd, (LPCTSTR)pszMsg, APP_NAME, MB_YESNO | MB_ICONEXCLAMATION)
#ifndef MAKEFONT
	#define MAKEFONT(hdc, point, weight, itaric, face)	\
			CreateFont(-MulDiv((int)point, GetDeviceCaps(hdc, LOGPIXELSY), 72),	\
				0, 0, 0, (WORD)weight,	\
				(BOOL)itaric, FALSE, FALSE,	\
				SHIFTJIS_CHARSET,	\
				OUT_DEFAULT_PRECIS,	\
				CLIP_DEFAULT_PRECIS,	\
				PROOF_QUALITY,	\
				DEFAULT_PITCH | FF_MODERN,	\
				(LPCTSTR)face)
#endif
/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
BOOL InitCom();
BOOL UninitCom();
void InitWin32ComCtrl(DWORD dwICC = ICC_WIN95_CLASSES);
//WORD GetVersionNumber();
HIMAGELIST CreateImageList(HWND hWnd, UINT uiImgBmp, const int nImg, COLORREF colMask);
BOOL SetImageList2Listview(HWND hListView, UINT uiBmpRes,const int nImg, const int nIconStyle = LVSIL_SMALL, COLORREF colMask = RGB(255, 255, 255));
HWND CreateControl(HINSTANCE hInst, HWND hParent, WORD wID, LPCTSTR lpcClass, LPCTSTR lpcTitle, DWORD dwStyle, DWORD dwExStyle = WS_EX_CLIENTEDGE, int nX = 0, int nY = 0, int nWidth = 0, int nHeight = 0);
HWND CreateSpinCtrl(HINSTANCE hInst, HWND hParent, HWND hBody, int nMax, int nMin, WORD wID, DWORD dwStyle = CTRLSTYLE_SPIN);
BOOL GetOpenFileDlg(HWND hWnd, char *pszFilePath, const int nLen, const char *pszFilter, const char *pszTitle, char *pszFileTitle = NULL, const char *pszDefExt = NULL, DWORD dwFlg = OFN_EXPLORER | OFN_HIDEREADONLY /*| OFN_NOREADONLYRETURN*/ | OFN_FILEMUSTEXIST);
BOOL GetSaveFileDlg(HWND hWnd, char *pszFilePath, const int nLen, const char *pszFilter,  const char *pszTitle, char *pszFileTitle = NULL, const char *pszDefExt = NULL, DWORD dwFlg = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
int GetSHFolderDlg(HWND hParent, char *pszFolderPath, const char *pszTitle, BOOL bUseNewStyle, BOOL bUseNewFolderButton, UINT uiFlg = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
BOOL GetColorDlg(HWND hParent, DWORD dwDefColor, DWORD *lpdwResultColor);
HICON ShellGetIcon(char *pszPath, const int nFlg = SHGFI_LARGEICON);
HCURSOR GetFingerCursor();
COLORREF AddColorValue(COLORREF colBase, const int nAdd, const int nMax = 240, const int nMin = 15);
int IsSameDirectory(const char *pszPath1, const char *pszPath2);
BOOL StripDirectory(const char *pszFullPath, char *pszDirPath, const int nSize);
BOOL FullPathDirectory(const char *pszDirPath, char *pszFullPath, const int nSize);
BOOL CreateShellLink(const char *szLinkFile, const char *szTargetFile, const char *szArguments, const char *szWorkDir, int nShowCmd = SW_SHOWNORMAL);
int GetPermissionStr(const char *pszAppName, char *pszPermission);
BOOL ExistFile(const char *pszPath);
void EditBoxShowBalloon(LPCOMEDITBOXSHOWBALLOON lpcesb);
void SetPrivileges(LPCTSTR lpName);

#endif	//_INC_COMMON_H_