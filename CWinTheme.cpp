/*-------------------------------------------------------------------------

	CWinTheme.cpp

-------------------------------------------------------------------------*/
#include "CWinTheme.h"

CWinTheme::CWinTheme()
{
	LoadUxTheme();
}

CWinTheme::~CWinTheme()
{
	UnloadUxTheme();
}

BOOL CWinTheme::LoadUxTheme()
{
	m_hUxTheme = LoadLibrary(XPTHEME_DLL);
	if(!m_hUxTheme) return FALSE;
	m_lpFuncIsThremeActive = (LPFUNCISTHEMEACTIVE)GetProcAddress(m_hUxTheme, PROC_ISTHEMEACTIVE);
	m_lpFuncSetWindowTheme = (LPFUNCSETWINDOWTHEME)GetProcAddress(m_hUxTheme, PROC_SETWINDOWTHEME);
	return TRUE;
}

BOOL CWinTheme::UnloadUxTheme()
{
	if(m_hUxTheme) FreeLibrary(m_hUxTheme);
	return TRUE;
}

BOOL CWinTheme::IsThemeAct()
{
	if(!m_hUxTheme) return FALSE;
	if(!m_lpFuncIsThremeActive) return FALSE;
	return (m_lpFuncIsThremeActive());
}

BOOL CWinTheme::SetTheme(HWND hWnd, LPCTSTR lpcSubAppName)
{
    if (!IsThemeAct()) return FALSE;
    if (!m_lpFuncSetWindowTheme) return FALSE;

    WCHAR szwSubApp[1024];

    if (MultiByteToWideChar(
        CP_ACP,
        0,
        lpcSubAppName,
        -1,
        szwSubApp,
        _countof(szwSubApp)) == 0)
    {
        return FALSE;
    }

    return (BOOL)(SUCCEEDED(
        m_lpFuncSetWindowTheme(hWnd, szwSubApp, NULL)));
}
