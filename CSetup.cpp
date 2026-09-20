/*------------------------------------------------------------------------------
	CSetup.cpp
------------------------------------------------------------------------------*/
#include "CSetup.h"

CSetup::CSetup()
{
	m_szWorkDir[0] = '\0';
	m_szIniFile[0] = '\0';
	//char *p, *r;
	//GetModuleFileName(NULL, m_szWorkDir, (DWORD)sizeof(m_szWorkDir));
	//for(r = p = m_szWorkDir; *p != '\0'; p++){
	//	if(IsDBCSLeadByte(*p) == TRUE){
	//		p++;
	//		continue;
	//	}
	//	if(*p == '\\') r = p;
	//}
	//*r = '\0';
	GetSpecialDir(m_szWorkDir, CSIDL_APPDATA);
	if(m_szWorkDir[strlen(m_szWorkDir) - 1] != '\\') strcat_s(m_szWorkDir, sizeof(m_szWorkDir), "\\");
}

int CSetup::SetIniFileTitle(const char *pszTitle)
{
	if(pszTitle[0] == '\\'){
		wsprintf(m_szIniFile, "%s%s", m_szWorkDir, pszTitle);
	}else{
		wsprintf(m_szIniFile, "%s\\%s", m_szWorkDir, pszTitle);
	}
	return ((int)strlen(m_szIniFile));
}

int CSetup::SetIniFile(const char *pszIniFile)
{
//	strcpy(m_szIniFile, pszIniFile);
	strcpy_s(m_szIniFile, sizeof(m_szIniFile), pszIniFile);
	return ((int)strlen(m_szIniFile));
}

////CSIDL_APPDATA
//int CSetup::SetIniFileTitle(const char *pszTitle, const int nFolder)
//{
//	char szSpecialDir[MAX_PATH];
//	GetSpecialDir(szSpecialDir, nFolder);
//	if(szSpecialDir[strlen(szSpecialDir) - 1] == '\\'){
//		wsprintf(m_szIniFile, "%s%s", szSpecialDir, pszTitle);
//	}else{
//		wsprintf(m_szIniFile, "%s\\%s", szSpecialDir, pszTitle);
//	}
//	return ((int)strlen(m_szIniFile));
//}

int CSetup::GetString(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize)
{
	if(strlen(m_szIniFile) < 1) return -1;
	return (GetPrivateProfileString(pszSec, pszKey, pszDef, pszData, nSize, m_szIniFile));
}

int CSetup::GetInt(const char *pszSec, const char *pszKey, const int nDef)
{
	if(strlen(m_szIniFile) < 1) return -1;
	return (GetPrivateProfileInt(pszSec, pszKey, nDef, m_szIniFile));
}

WORD CSetup::GetWord(const char *pszSec, const char *pszKey, const WORD wDef)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char szData[8];
	if(GetPrivateProfileString(pszSec, pszKey, "", szData, (DWORD)sizeof(szData), m_szIniFile) < 1) return wDef;
	return (Hex2Word(szData));
}

DWORD CSetup::GetDword(const char *pszSec, const char *pszKey, const DWORD dwDef)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char szData[12];
	if(GetPrivateProfileString(pszSec, pszKey, "", szData, (DWORD)sizeof(szData), m_szIniFile) < 1) return dwDef;
	return (Hex2Dword(szData));
}

int CSetup::GetB64(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char *pszTmp = (char *)GlobalAlloc(GPTR, (nSize * 2) + 1);
	if(!pszTmp) return -1;
	if(GetPrivateProfileString(pszSec, pszKey, "", pszTmp, (nSize * 2) + 1, m_szIniFile) < 1){
		GlobalFree(pszTmp);
//		strcpy(pszData, pszDef);
		strcpy_s(pszData, nSize, pszDef);
		return ((int)strlen(pszData));
	}
	int nLen = 0;
	char *pszVal = (char *)DecBase64((unsigned char *)pszTmp);
	if(pszVal){
		nLen = (int)strlen(pszVal);
//		strcpy(pszData, pszVal);
		strcpy_s(pszData, (nSize * 2), pszVal);
		GlobalFree(pszVal);
	}
	GlobalFree(pszTmp);
	return nLen;
}

int CSetup::GetJIS(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char *pszTmp = (char *)GlobalAlloc(GPTR, (nSize * 2) + 1);
	if(!pszTmp) return -1;
	if(GetPrivateProfileString(pszSec, pszKey, "", pszTmp, (nSize * 2) + 1, m_szIniFile) < 1){
		GlobalFree(pszTmp);
//		strcpy(pszData, pszDef);
		strcpy_s(pszData, (nSize * 2), pszDef);
		return ((int)strlen(pszData));
	}
	int nLen = 0;
	char *pszVal = (char *)Jis2SJis((unsigned char *)pszTmp);
	if(pszVal){
		nLen = (int)strlen(pszVal);
//		strcpy(pszData, pszVal);
		strcpy_s(pszData, nSize, pszVal);
		GlobalFree(pszVal);
	}
	GlobalFree(pszTmp);
	return nLen;
}

int CSetup::GetEUC(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char *pszTmp = (char *)GlobalAlloc(GPTR, (nSize * 2) + 1);
	if(!pszTmp) return -1;
	if(GetPrivateProfileString(pszSec, pszKey, "", pszTmp, (nSize * 2) + 1, m_szIniFile) < 1){
		GlobalFree(pszTmp);
//		strcpy(pszData, pszDef);
		strcpy_s(pszData, (nSize * 2), pszDef);
		return ((int)strlen(pszData));
	}
	int nLen = 0;
	char *pszVal = (char *)Euc2SJis((unsigned char *)pszTmp);
	if(pszVal){
		nLen = (int)strlen(pszVal);
//		strcpy(pszData, pszVal);
		strcpy_s(pszData, nSize, pszVal);
		GlobalFree(pszVal);
	}
	GlobalFree(pszTmp);
	return nLen;
}

int CSetup::GetUnicode(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize)
{
	if(strlen(m_szIniFile) < 1) return -1;
	wchar_t *pszTmp = (wchar_t *)GlobalAlloc(GPTR, (nSize * sizeof(wchar_t)) + sizeof(wchar_t));
	if(!pszTmp) return -1;
	if(GetPrivateProfileString(pszSec, pszKey, "", (char *)pszTmp, (nSize * sizeof(wchar_t)) + sizeof(wchar_t), m_szIniFile) < 1){
		GlobalFree(pszTmp);
//		strcpy(pszData, pszDef);
		strcpy_s(pszData, nSize, pszDef);
		return ((int)strlen(pszData));
	}
	int nLen = 0;
	char *pszVal = (char *)UTF162SJis(pszTmp);
	if(pszVal){
		nLen = (int)strlen(pszVal);
//		strcpy(pszData, pszVal);
		strcpy_s(pszData, nSize, pszVal);
		GlobalFree(pszVal);
	}
	GlobalFree(pszTmp);
	return nLen;
}

int CSetup::GetUTF8(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char *pszTmp = (char *)GlobalAlloc(GPTR, (nSize * sizeof(char)) + sizeof(char));
	if(!pszTmp) return -1;
	if(GetPrivateProfileString(pszSec, pszKey, "", pszTmp, (nSize * sizeof(char)) + sizeof(char), m_szIniFile) < 1){
		GlobalFree(pszTmp);
//		strcpy(pszData, pszDef);
		strcpy_s(pszData, nSize, pszDef);
		return ((int)strlen(pszData));
	}
	int nLen = 0;
	char *pszVal = (char *)UTF82SJis(pszTmp);
	if(pszVal){
		nLen = (int)strlen(pszVal);
//		strcpy(pszData, pszVal);
		strcpy_s(pszData, nSize, pszVal);
		GlobalFree(pszVal);
	}
	GlobalFree(pszTmp);
	return nLen;
}

int CSetup::WriteString(const char *pszSec, const char *pszKey, char *pszData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	return (WritePrivateProfileString(pszSec, pszKey, pszData, m_szIniFile));
}

int CSetup::WriteInt(const char *pszSec, const char *pszKey, const int nData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char szData[15];
	wsprintf(szData, "%d", nData);
	return (WritePrivateProfileString(pszSec, pszKey, szData, m_szIniFile));
}

int CSetup::WriteWord(const char *pszSec, const char *pszKey, const WORD wData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char szData[8];
	wsprintf(szData, "0x%04x", wData);		// 0xを付ける
	return (WritePrivateProfileString(pszSec, pszKey, szData, m_szIniFile));
}

int CSetup::WriteDword(const char *pszSec, const char *pszKey, const DWORD dwData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	char szData[12];
	wsprintf(szData, "0x%08x", dwData);		// 0xを付ける
	return (WritePrivateProfileString(pszSec, pszKey, szData, m_szIniFile));
}

int CSetup::WriteB64(const char *pszSec, const char *pszKey, char *pszData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	int nLen = 0;
	char *pszVal = (char *)EncBase64((unsigned char *)pszData);
	if(pszVal){
		nLen = WritePrivateProfileString(pszSec, pszKey, pszVal, m_szIniFile);
		GlobalFree(pszVal);
	}else{
		return -1;
	}
	return nLen;
}

int CSetup::WriteJIS(const char *pszSec, const char *pszKey, char *pszData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	int nLen = 0;
	char *pszVal = (char *)SJis2Jis((unsigned char *)pszData);
	if(pszVal){
		nLen = WritePrivateProfileString(pszSec, pszKey, pszVal, m_szIniFile);
		GlobalFree(pszVal);
	}else{
		return -1;
	}
	return nLen;
}

int CSetup::WriteEUC(const char *pszSec, const char *pszKey, char *pszData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	int nLen = 0;
	char *pszVal = (char *)SJis2Euc((unsigned char *)pszData);
	if(pszVal){
		nLen = WritePrivateProfileString(pszSec, pszKey, pszVal, m_szIniFile);
		GlobalFree(pszVal);
	}else{
		return -1;
	}
	return nLen;
}

int CSetup::WriteUnicode(const char *pszSec, const char *pszKey, char *pszData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	int nLen = 0;
	char *pszVal = (char *)SJis2UTF16(pszData);
	if(pszVal){
		nLen = WritePrivateProfileString(pszSec, pszKey, pszVal, m_szIniFile);
		GlobalFree(pszVal);
	}else{
		return -1;
	}
	return nLen;
}

int CSetup::WriteUTF8(const char *pszSec, const char *pszKey, char *pszData)
{
	if(strlen(m_szIniFile) < 1) return -1;
	int nLen = 0;
	char *pszVal = (char *)SJis2UTF8(pszData);
	if(pszVal){
		nLen = WritePrivateProfileString(pszSec, pszKey, pszVal, m_szIniFile);
		GlobalFree(pszVal);
	}else{
		return -1;
	}
	return nLen;
}

int CSetup::GetWorkDir(char *pszWorkDir, int nSize)
{
	if((int)strlen(m_szWorkDir) > nSize) return -1;
//	strcpy(pszWorkDir, m_szWorkDir);
	strcpy_s(pszWorkDir, nSize, m_szWorkDir);
	return ((int)strlen(pszWorkDir));
}

int CSetup::GetIniFile(char *pszIniFile, int nSize)
{
	if((int)strlen(m_szIniFile) > nSize) return -1;
//	strcpy(pszIniFile, m_szIniFile);
	strcpy_s(pszIniFile, nSize, m_szIniFile);
	return ((int)strlen(pszIniFile));
}

/*----------------------------------------------------------------------
	CSIDL_DESKTOP         デスクトップ(ネームスペースのルートにある仮想フォルダ)
	CSIDL_DESKTOPDIRECTOY デスクトップのファイルを物理的に格納するフォルダ
	CSIDL_STARTMENU       スタートメニュー
	CSIDL_PROGRAMS        プログラムメニュー
	CSIDL_PERSONAL        マイ ドキュメント
	CSIDL_PROGRAM_FILES   Program Files
	CSIDL_FAVORITES       お気に入り
	CSIDL_APPDATA         アプリケーションデータ
	CSIDL_COMMON_xxx      共通(All Users)のフォルダ
----------------------------------------------------------------------*/
BOOL CSetup::GetSpecialDir(char *pszSpecialDir, int nFolder)
{
/*
	BOOL bRet = FALSE; 

	OSVERSIONINFO ovi;
	ZeroMemory(&ovi, sizeof(OSVERSIONINFO));
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ovi);
	if(ovi.dwMajorVersion < 5){
		LPITEMIDLIST lpIdl;
		IMalloc *pMalloc;
		SHGetMalloc(&pMalloc);
		if(SUCCEEDED(SHGetSpecialFolderLocation(HWND_DESKTOP,
			nFolder,
			&lpIdl))){
			if((bRet = SHGetPathFromIDList(lpIdl, pszSpecialDir))) pMalloc->Free(lpIdl);
		}
		pMalloc->Release();
	}else{
		bRet = SHGetSpecialFolderPath(HWND_DESKTOP,
			pszSpecialDir,
			nFolder,
			FALSE);
	}
	return bRet;
*/
	return (BOOL)SHGetSpecialFolderPath(HWND_DESKTOP,
			pszSpecialDir,
			nFolder,
			FALSE);
}

BOOL CSetup::ExistFile(const char *pszPath/*= NULL*/)
{
	HANDLE hFind;
	WIN32_FIND_DATA w32fd;
	ZeroMemory(&w32fd, sizeof(WIN32_FIND_DATA));
	if(pszPath){
		hFind = FindFirstFile(pszPath, &w32fd);
	}else{
		hFind = FindFirstFile(m_szIniFile, &w32fd);
	}
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;
	FindClose(hFind);
	return TRUE;
}
