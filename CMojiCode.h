/*
	CMojiCode.h
*/
#ifndef _INC_CMOJICODE
#define _INC_CMOJICODE

#ifndef _INC_WINDOWS
	#include <windows.h>
#endif
#include <locale.h>

const char B64[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
#define valb64(i) (buf[i]=='=' ? 0 : strchr(B64, buf[i])-B64)
#define iskanji(c) ((c)>=0x81 && (c)<=0x9F || (c)>=0xE0 && (c)<=0xFC)
#define iskanji2(c) ((c)>=0x40 && (c)<=0xFC && (c)!=0x7F)
#define ESC_CHAR 0x1B
#define TABLESIZE 0x100
#define DIGEST_LEN 16

class CMojiCode
{
private:
	wchar_t *ToWideChar(char *buf, UINT CodePage = CP_ACP);
	char *ToMultiByte(wchar_t *buf, UINT CodePage = CP_ACP);

public:
	CMojiCode() {};
	virtual ~CMojiCode() {};

	DWORD Hex2Dword(const char *pszHex);
	WORD Hex2Word(const char *pszHex);
	char Hex2Char(char c);
	unsigned char *DecBase64(unsigned char *buf);
	unsigned char *EncBase64(unsigned char *buf);
	unsigned char *SJis2Jis(unsigned char *buf);
	unsigned char *SJis2Euc(unsigned char *buf);
	unsigned char *Jis2SJis(unsigned char *buf);
	unsigned char *Euc2SJis(unsigned char *buf);
	char *URLDecode(char *buf);
	char *URLEncode(const char *buf);
	wchar_t *SJis2UTF16(char *buf);
	char *UTF162SJis(wchar_t *buf);
	char *SJis2UTF8(char *buf);
	char *UTF82SJis(char *buf);
};

#endif	//_INC_CMOJICODE
