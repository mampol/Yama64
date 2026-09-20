/*------------------------------------------------------------------------------
	CSetup.h
	ê›íËÉtÉ@ÉCÉãëÄçÏäÓíÍ
------------------------------------------------------------------------------*/

#ifndef _INC_CSETUP
#define _INC_CSETUP

#ifndef _INC_WINDOWS
#include <windows.h>
#endif
#ifndef _SHLOBJ_H_
#include <shlobj.h>
#endif
#ifndef _INC_CMOJICODE
#include "CMojiCode.h"
#endif

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER			0xFFFFFFFF
#endif	//INVALID_SET_FILE_POINTER

class CSetup : public CMojiCode
{
private:

protected:
	char m_szWorkDir[MAX_PATH+1];
	char m_szIniFile[MAX_PATH+1];

private:

protected:

public:
	CSetup();
	virtual ~CSetup() {};

	int GetString(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize);
	int GetInt(const char *pszSec, const char *pszKey, const int nDef);
	WORD GetWord(const char *pszSec, const char *pszKey, const WORD wDef);
	DWORD GetDword(const char *pszSec, const char *pszKey, const DWORD dwDef);
	int GetB64(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize);
	int GetJIS(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize);
	int GetEUC(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize);
	int GetUnicode(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize);
	int GetUTF8(const char *pszSec, const char *pszKey, const char *pszDef, char *pszData, int nSize);
	int WriteString(const char *pszSec, const char *pszKey, char *pszData);
	int WriteInt(const char *pszSec, const char *pszKey, const int nData);
	int WriteWord(const char *pszSec, const char *pszKey, const WORD wData);
	int WriteDword(const char *pszSec, const char *pszKey, const DWORD dwData);
	int WriteB64(const char *pszSec, const char *pszKey, char *pszData);
	int WriteEUC(const char *pszSec, const char *pszKey, char *pszData);
	int WriteJIS(const char *pszSec, const char *pszKey, char *pszData);
	int WriteUnicode(const char *pszSec, const char *pszKey, char *pszData);
	int WriteUTF8(const char *pszSec, const char *pszKey, char *pszData);

	int SetIniFileTitle(const char *pszTitle);
	int SetIniFile(const char *pszIniFile);
	int SetIniFileTitle(const char * pszTitle, const int nFolder);
	int GetWorkDir(char *pszWorkDir, int nSize);
	BOOL GetSpecialDir(char *pszSpecialDir, int nFolder);
	int GetIniFile(char *pszIniFile, int nSize);
	BOOL ExistFile(const char *pszPath = NULL);
};

#endif	//_INC_CSETUP