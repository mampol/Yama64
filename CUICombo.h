/*------------------------------------------------------------------------------
	CUICombo.h
------------------------------------------------------------------------------*/
#ifndef _INC_CUICOMBO
#define _INC_CUICOMBO

#ifndef _INC_WINDOWS
#include <windows.h>
#endif
#ifndef _INC_WINDOWSX
#include <windowsx.h>
#endif

class CUICombo
{
private:
	HWND m_hTmp;

public:

private:
	void DeleteAllString(HWND hCombo);
	static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam);
	COLORREF GetColor(const char *pszColor);

public:
	CUICombo() : m_hTmp(NULL) {};
	~CUICombo() {};

	void CreateFontFaceCombo(HWND hCombo, BYTE btCharSet, const char *pszFaceName);
	void CreateFontSizeCombo(HWND hCombo, UINT uiBegin, UINT uiEnd);
	void CreateColorCombo(HWND hCombo, COLORREF colUser = RGB(0, 0, 0));
	LRESULT OnMeasureItemColorCombo(HWND hCombo, HWND hParent, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT OnDrawItemColorCombo(HWND hCombo, HWND hParent, UINT msg, WPARAM wp, LPARAM lp);
	BOOL OnCommandCustomColor(HWND hCombo, HWND hParent);
	int SelectListItem(HWND hCombo, const char *pszSelectStr);
	int SelectListItem(HWND hCombo, const COLORREF crSelectColor);
	int SelectListItem(HWND hCombo, const int nSelectIdx);
	int GetSelText(HWND hCombo, char *pszSelText, const int nLen);
	int GetSelInt(HWND hCombo);
	COLORREF GetSelColor(HWND hCombo);
};

#endif //_INC_CUICOMBO