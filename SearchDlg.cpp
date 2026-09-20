/*----------------------------------------------------------------------------------------

	OutputWnd.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "SearchDlg.h"
#include "OutputWnd.h"
#include "resource.h"

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HWND ghMainWnd = NULL;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SearchDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnInitSearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd);
LRESULT OnCommandSearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd);
LRESULT OnCloseSearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd);
LRESULT OnDestroySearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd);
int GetSearchData(HWND hDlg, LPSEARCHDATA lpsd);
BOOL GetSearchHistoryFile(char *pszSearchHistoryFile, int nLen);
int GetSearchHistroy(HWND hCombo);
int SetSearchHistroy(HWND hCombo);

/*----------------------------------------------------------------------------------------
	検索ダイアログ表示
----------------------------------------------------------------------------------------*/
int ShowSearchDlg(HWND hParent)
{
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hParent, CSETUP_CLASS);
	if(!lpCSetup) return -1;
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hParent, CANALST_CLASS);
	if(!lpCAnaLst) return -1;
	if(!lpCAnaLst->IsMakedList()){
		return -1;
	}
	ghMainWnd = hParent;
	int nSearch = -1;
	SEARCHDATA sd;
	ZeroMemory(&sd, sizeof(SEARCHDATA));
	sd.dwCmdFlags = lpCSetup->m_SearchProp.m_dwCmdFlags;
	sd.dwCmdExFlags = lpCSetup->m_SearchProp.m_dwCmdExFlags;
	sd.dwSrcFlags = lpCSetup->m_SearchProp.m_dwSrcFlags;
//	if(DialogBoxParam((HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
	if(DialogBoxParam((HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE),
										MAKEINTRESOURCE(IDD_SEARCHBOX),
										hParent,
										(DLGPROC)SearchDlgProc,
										(LPARAM)&sd) == IDOK){
		CProgStbar *lpCStatus = (CProgStbar *)GetProp(hParent, CPROGSTBAR_CLASS);
		if(lpCStatus) lpCStatus->SetSbText(0, SBT_NOBORDERS, "検索しています...");
		// 検索実行
		nSearch = lpCAnaLst->Search(sd.szSearchString, sd.dwCmdFlags, sd.dwCmdExFlags, sd.dwSrcFlags);
		if(lpCStatus) lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");
		// フラグ待避
		lpCSetup->m_SearchProp.m_dwCmdFlags = sd.dwCmdFlags;
		lpCSetup->m_SearchProp.m_dwCmdExFlags = sd.dwCmdExFlags;
		lpCSetup->m_SearchProp.m_dwSrcFlags = sd.dwSrcFlags;
	}
	ghMainWnd = NULL;
	return nSearch;
}

LRESULT CALLBACK SearchDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	static LPSEARCHDATA lpsd = NULL;
	switch(msg){
	case WM_INITDIALOG:
		lpsd = (LPSEARCHDATA)lp;
		return (OnInitSearchDialog(hDlg, msg, wp, lp, lpsd));
	case WM_COMMAND:
		return (OnCommandSearchDialog(hDlg, msg, wp, lp, lpsd));
	case WM_CLOSE:
		return (OnCloseSearchDialog(hDlg, msg, wp, lp, lpsd));
	case WM_DESTROY:
		return (OnDestroySearchDialog(hDlg, msg, wp, lp, lpsd));
	}
	return FALSE;
}

LRESULT OnInitSearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd)
{
	SendDlgItemMessage(hDlg, IDC_CMB_SEARCH, CB_LIMITTEXT, (WPARAM)MAX_SEARCH_STRING, 0L);
	GetSearchHistroy(GetDlgItem(hDlg, IDC_CMB_SEARCH));
	CheckDlgButton(hDlg, IDC_CHECK_RECASE, (lpsd->dwCmdFlags & NORM_IGNORECASE ? BST_UNCHECKED : BST_CHECKED));
	CheckDlgButton(hDlg, IDC_CHECK_OCCURS, (lpsd->dwCmdExFlags & SEARCH_FLGEX_OCCURSFIRST ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hDlg, IDC_CHECK_DESCRIPT, (lpsd->dwSrcFlags & SEARCH_FLG_DESCRIPT ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hDlg, IDC_CHECK_TYPEDEF, (lpsd->dwSrcFlags & SEARCH_FLG_TYPEDEF ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hDlg, IDC_CHECK_COMPRESS, (lpsd->dwSrcFlags & SEARCH_FLG_COMPRESS ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hDlg, IDC_CHECK_VALUE, (lpsd->dwSrcFlags & SEARCH_FLG_VALUE ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hDlg, IDC_CHECK_COMMENT, (lpsd->dwSrcFlags & SEARCH_FLG_COMMENT ? BST_CHECKED : BST_UNCHECKED));
	return TRUE;
}

LRESULT OnCommandSearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd)
{
	switch(LOWORD(wp)){
	case IDOK:
		if(GetSearchData(hDlg, lpsd) > 0) EndDialog(hDlg, IDOK);
		break;
	case IDCANCEL:
		SendMessage(hDlg, WM_CLOSE, 0L, 0L);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT OnCloseSearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd)
{
	EndDialog(hDlg, IDCANCEL);
	return TRUE;
}

LRESULT OnDestroySearchDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp, LPSEARCHDATA lpsd)
{
	SetSearchHistroy(GetDlgItem(hDlg, IDC_CMB_SEARCH));
	return TRUE;
}

int GetSearchData(HWND hDlg, LPSEARCHDATA lpsd)
{
	int nLen;
	HWND hCombo = GetDlgItem(hDlg, IDC_CMB_SEARCH);
	// 検索文字列
	if((nLen = GetWindowText(hCombo, lpsd->szSearchString, MAX_SEARCH_STRING)) < 1){
		INFMSG(hDlg, "検索文字列が未入力です");
		SetFocus(hCombo);
		return -1;
	}
	// 検索フラグ
	lpsd->dwCmdFlags = ((SendDlgItemMessage(hDlg, IDC_CHECK_RECASE, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? 0 : NORM_IGNORECASE);
	// 繰返し項目検索
	lpsd->dwCmdExFlags = 0;
	lpsd->dwCmdExFlags |= ((SendDlgItemMessage(hDlg, IDC_CHECK_OCCURS, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? SEARCH_FLGEX_OCCURSFIRST : 0);
	// 検索項目
	lpsd->dwSrcFlags = 0;
	lpsd->dwSrcFlags |= ((SendDlgItemMessage(hDlg, IDC_CHECK_DESCRIPT, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? SEARCH_FLG_DESCRIPT : 0);
	lpsd->dwSrcFlags |= ((SendDlgItemMessage(hDlg, IDC_CHECK_TYPEDEF, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? SEARCH_FLG_TYPEDEF : 0);
	lpsd->dwSrcFlags |= ((SendDlgItemMessage(hDlg, IDC_CHECK_COMPRESS, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? SEARCH_FLG_COMPRESS : 0);
	lpsd->dwSrcFlags |= ((SendDlgItemMessage(hDlg, IDC_CHECK_VALUE, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? SEARCH_FLG_VALUE : 0);
	lpsd->dwSrcFlags |= ((SendDlgItemMessage(hDlg, IDC_CHECK_COMMENT, BM_GETSTATE, 0L, 0L) == BST_CHECKED) ? SEARCH_FLG_COMMENT : 0);
	if(lpsd->dwSrcFlags == 0){
		INFMSG(hDlg, "検索項目は最低１つは選択してください");
		return -1;
	}
	// コンボにない文字列ならばコンボに追加
	int nCount = SendMessage(hCombo, CB_GETCOUNT, 0L, 0L);
	BOOL bAddFlg = TRUE;
	if(nCount > 0){
		for(int i = 0; i < nCount; i++){
			char szBuf[MAX_SEARCH_STRING];
			if(SendMessage(hCombo, CB_GETLBTEXT, (WPARAM)i, (LPARAM)szBuf) == CB_ERR) continue;
			if(CompareString(LOCALE_USER_DEFAULT,
				lpsd->dwCmdFlags,
				lpsd->szSearchString,
				-1,
				szBuf,
				-1) == CSTR_EQUAL){
				bAddFlg = FALSE;
				break;
			}
		}
	}
	if(bAddFlg) SendMessage(hCombo, CB_INSERTSTRING, (WPARAM)0, (LPARAM)lpsd->szSearchString);
	return nLen;
}

BOOL GetSearchHistoryFile(char *pszSearchHistoryFile, int nLen)
{
	CSetup *lpCSetup = (CSetup *)GetProp(ghMainWnd, CSETUP_CLASS);
	if(!lpCSetup) return FALSE;
	if(lpCSetup->GetWorkDir(pszSearchHistoryFile, nLen) < 1) return FALSE;
//	strcat(pszSearchHistoryFile, SEARCHHISTORY_FILE);
	strcat_s(pszSearchHistoryFile, nLen, SEARCHHISTORY_FILE);
	return TRUE;
}

int GetSearchHistroy(HWND hCombo)
{
	int nHistory = 0;
	HANDLE hHistroy = NULL;
	char *pszBuf = NULL;

	__try{
		char szSearchHistoryFile[MAX_PATH + 1];
		if(!GetSearchHistoryFile(szSearchHistoryFile, (int)sizeof(szSearchHistoryFile))) __leave;
		hHistroy = CreateFile(szSearchHistoryFile,
									GENERIC_READ,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									OPEN_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
		if(hHistroy == INVALID_HANDLE_VALUE) __leave; 

		DWORD dwSize = GetFileSize(hHistroy, NULL);
		if(dwSize < 1) __leave;
		pszBuf = (char *)GlobalAlloc(GPTR, dwSize + 1);
		if(!pszBuf) __leave;

		DWORD dwRead;
		if(!ReadFile(hHistroy, pszBuf, dwSize, &dwRead, NULL)) __leave;

		// \nで１データ
		char *p, *pszSearchString;
		for(pszSearchString = p = pszBuf; *p != '\0'; p++){
			if(IsDBCSLeadByte(*p)){
				p++;
				continue;
			}
			if(*p == '\n'){
				*p = '\0';
				SendMessage(hCombo, CB_ADDSTRING, 0L, (LPARAM)pszSearchString);
				if(++nHistory > MAX_HISTORY) break;
				pszSearchString = ++p;
			}
		}
	}
	__finally{
		if(pszBuf) GlobalFree(pszBuf);
		if(hHistroy != INVALID_HANDLE_VALUE) CloseHandle(hHistroy);
	}
	return nHistory;
}

int SetSearchHistroy(HWND hCombo)
{
	int nHistory = 0;
	HANDLE hHistroy = NULL;

	__try{
		char szSearchHistoryFile[MAX_PATH + 1];
		if(!GetSearchHistoryFile(szSearchHistoryFile, (int)sizeof(szSearchHistoryFile))) __leave;
		hHistroy = CreateFile(szSearchHistoryFile,
									GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									TRUNCATE_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
		if(hHistroy == INVALID_HANDLE_VALUE) __leave;

		int nCount = SendMessage(hCombo, CB_GETCOUNT, 0L, 0L);
		if(nCount < 1) __leave;

		// \nで１データ
		char szSearchString[MAX_SEARCH_STRING + 1];
		for(int i = 0; i < nCount; i++){
			int nLen;
			if((nLen = (int)SendMessage(hCombo, CB_GETLBTEXTLEN, (WPARAM)i, 0L)) == CB_ERR) break;
			if(nLen > MAX_SEARCH_STRING) continue;
			if(SendMessage(hCombo, CB_GETLBTEXT, (WPARAM)i, (LPARAM)szSearchString) == CB_ERR) break;
			DWORD dwWrite;
			if(!WriteFile(hHistroy, szSearchString, (DWORD)nLen, &dwWrite, NULL)) __leave;
			if(!WriteFile(hHistroy, "\n", (DWORD)1, &dwWrite, NULL)) __leave;
			if(++nHistory > MAX_HISTORY) break;
		}
	}
	__finally{
		if(hHistroy != INVALID_HANDLE_VALUE) CloseHandle(hHistroy);
	}
	return nHistory;
}
