/*-------------------------------------------------------------------------

	CWinTheme.h

-------------------------------------------------------------------------*/
#ifndef _CWINTHEME_
#define _CWINTHEME_

#ifndef _INC_WINDOWS
#include <windows.h>
#endif	//_INC_WINDOWS

#define XPTHEME_DLL				(LPCTSTR)"UxTheme.dll"
#define PROC_ISTHEMEACTIVE		(LPCTSTR)"IsThemeActive"
#define PROC_SETWINDOWTHEME		(LPCTSTR)"SetWindowTheme"
typedef BOOL (WINAPI *LPFUNCISTHEMEACTIVE)(VOID);
typedef HRESULT (WINAPI *LPFUNCSETWINDOWTHEME)(HWND, LPCWSTR, LPCWSTR);

class CWinTheme
{
private:
	HMODULE m_hUxTheme;
	LPFUNCISTHEMEACTIVE m_lpFuncIsThremeActive;
	LPFUNCSETWINDOWTHEME m_lpFuncSetWindowTheme;

protected:

public:

private:

protected:
	BOOL LoadUxTheme();
	BOOL UnloadUxTheme();

public:
	CWinTheme();
	virtual ~CWinTheme();

	BOOL IsThemeAct();
	BOOL SetTheme(HWND hWnd, LPCTSTR lpcSubAppName);
};

#endif	// _CWINTHEM_
