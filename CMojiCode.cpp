/*
	CMojiCode.cpp
*/
#include "CMojiCode.h"

DWORD CMojiCode::Hex2Dword(const char *pszHex)
{
	DWORD dwNum = 0;
	int nIm;
	if(!(*pszHex == '0' && (*(pszHex + 1) == 'x' || *(pszHex + 1) == 'X'))){
		return 0;
	}
	for(pszHex += 2; *pszHex != '\0'; pszHex++){
		if(*pszHex >= '0' && *pszHex <= '9'){
			nIm = *pszHex - '0';
		}else if(*pszHex >= 'A' && *pszHex <= 'F'){
			nIm = *pszHex - 'A' + 10;
		}else if(*pszHex >= 'a' && *pszHex <= 'f'){
			nIm = *pszHex - 'a' + 10;
		}else{
			break;
		}
		dwNum = 16 * dwNum + nIm;
	}
	return ((DWORD)dwNum);
}

WORD CMojiCode::Hex2Word(const char *pszHex)
{
	return ((WORD)Hex2Dword(pszHex));
}

char CMojiCode::Hex2Char(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}

	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}

	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}

	return c;
}

unsigned char *CMojiCode::DecBase64(unsigned char *buf)
{
	unsigned char *ReturnBuf = (unsigned char *)GlobalAlloc(GPTR, (DWORD)strlen((const char *)buf) + 1);
	if(!ReturnBuf) return NULL;
	int j = 0;
	int l = (int)strlen((const char *)buf);
	// 4バイトずつ処理
	for(int i = 0; i < l; i += 4){
		ReturnBuf[j]   = (unsigned char)((valb64(i) << 2) | ((valb64(i+1) & 0x30) >> 4));
		ReturnBuf[j+1] = (unsigned char)(((valb64(i+1) & 0x0f) << 4) | ((valb64(i+2) & 0x3c) >> 2));
		ReturnBuf[j+2] = (unsigned char)(((valb64(i+2) & 0x03) << 6) | (valb64(i+3) & 0x3f));
		j += 3;
	}
	ReturnBuf[j] = '\0';
	return ReturnBuf;
}

unsigned char* CMojiCode::EncBase64(unsigned char* buf)
{
	if (!buf) return NULL;

	size_t nSrcLen = strlen((const char*)buf);

	unsigned char* ReturnBuf =
		(unsigned char*)GlobalAlloc(
			GPTR,
			(DWORD)((nSrcLen * 2) + 2));

	if (!ReturnBuf) return NULL;

	unsigned char* TmpBuf =
		(unsigned char*)GlobalAlloc(
			GPTR,
			(DWORD)((nSrcLen * 2) + 1));

	if (!TmpBuf) {
		GlobalFree(ReturnBuf);
		return NULL;
	}

	strcpy_s(
		(char*)TmpBuf,
		(nSrcLen * 2) + 1,
		(const char*)buf);

	int j = 0;
	int l = (int)nSrcLen;

	for (int i = 0; i < l; i += 3) {
		ReturnBuf[j] =
			(char)(0 == (TmpBuf[i] >> 2))
			? 'A'
			: B64[TmpBuf[i] >> 2];

		ReturnBuf[j + 1] =
			(char)(0 == ((TmpBuf[i] & 0x03) << 4 |
				(TmpBuf[i + 1] >> 4)))
			? 'A'
			: B64[(TmpBuf[i] & 0x03) << 4 |
			(TmpBuf[i + 1] >> 4)];

		ReturnBuf[j + 2] =
			(char)(0 == ((TmpBuf[i + 1] & 0x0f) << 2 |
				(TmpBuf[i + 2] >> 6)))
			? 'A'
			: B64[(TmpBuf[i + 1] & 0x0f) << 2 |
			(TmpBuf[i + 2] >> 6)];

		ReturnBuf[j + 3] =
			(char)(0 == (TmpBuf[i + 2] & 0x3f))
			? 'A'
			: B64[TmpBuf[i + 2] & 0x3f];

		j += 4;
	}

	ReturnBuf[j] = '\0';

	GlobalFree(TmpBuf);

	return ReturnBuf;
}

unsigned char *CMojiCode::SJis2Jis(unsigned char *buf)
{
	unsigned char *ReturnBuf = (unsigned char *)GlobalAlloc(GPTR, ((strlen((const char *)buf) * 2) + 1));
	if(!ReturnBuf) return NULL;
	int i = 0;
	int hc, lc;
	BOOL kanji = FALSE;
	// SJISからJISに
	for(unsigned char *p = buf; *p != '\0'; p++){
		hc = *p;
		if(iskanji(hc)){
			lc = *(++p);
			if(iskanji2(lc)) {
				if(hc <= 0x9F){
					if(lc < 0x9F){
						hc = (hc << 1) - 0xE1;
					}else{
						hc = (hc << 1) - 0xE0;
					}
				}else{
					if(lc < 0x9F){
						hc = (hc << 1) - 0x161;
					}else{
						hc = (hc << 1) - 0x160;
					}
				}
				if(lc < 0x7F){
					lc -= 0x1F;
				}else
				if(lc < 0x9F){
					lc -= 0x20;
				}else{
					lc -= 0x7E;
				}
				if(!kanji){
					// JIS開始
					*(ReturnBuf + (i++)) = ESC_CHAR;
					*(ReturnBuf + (i++)) = '$';
					*(ReturnBuf + (i++)) = 'B';
					kanji = TRUE;
				}
				*(ReturnBuf + (i++)) = hc;
				*(ReturnBuf + (i++)) = lc;
			}else{
				if(kanji){
					// JIS終わり(ｶﾅ開始)
					*(ReturnBuf + (i++)) = ESC_CHAR;
					*(ReturnBuf + (i++)) = '(';
					*(ReturnBuf + (i++)) = 'B';
					kanji = FALSE;
				}
				*(ReturnBuf + (i++)) = hc;
				*(ReturnBuf + (i++)) = lc;
			}
		}else{
			if(kanji){
				// JIS終わり(ｶﾅ開始)
				*(ReturnBuf + (i++)) = ESC_CHAR;
				*(ReturnBuf + (i++)) = '(';
				*(ReturnBuf + (i++)) = 'B';
				kanji = FALSE;
			}
			*(ReturnBuf + (i++)) = hc;
		}
	}
	if(kanji){
		// JIS終わり(ｶﾅ開始)
		*(ReturnBuf + (i++)) = ESC_CHAR;
		*(ReturnBuf + (i++)) = '(';
		*(ReturnBuf + (i++)) = 'B';
	}
	*(ReturnBuf + i) = '\0';
	return ReturnBuf;
}

unsigned char *CMojiCode::SJis2Euc(unsigned char *buf)
{
	unsigned char *ReturnBuf = (unsigned char *)GlobalAlloc(GPTR, (DWORD)((strlen((const char *)buf) * 2) + 1));
	if(!ReturnBuf) return NULL;
	int i = 0;
	int hc, lc;
	// SJISからEUCに
	for(unsigned char *p = buf; *p != '\0'; p++){
		hc = *p;
		if(iskanji(hc)){
			lc = *(++p);
			if(iskanji2(lc)) {
				if(hc <= 0x9F){
					if(lc < 0x9F){
						hc = (hc << 1) - 0xE1;
					}else{
						hc = (hc << 1) - 0xE0;
					}
				}else{
					if(lc < 0x9F){
						hc = (hc << 1) - 0x161;
					}else{
						hc = (hc << 1) - 0x160;
					}
				}
				if(lc < 0x7F){
					lc -= 0x1F;
				}else
				if(lc < 0x9F){
					lc -= 0x20;
				}else{
					lc -= 0x7E;
				}
				*(ReturnBuf + (i++)) = hc | 0x80;
				*(ReturnBuf + (i++)) = lc | 0x80;
			}else{
				*(ReturnBuf + (i++)) = hc;
				*(ReturnBuf + (i++)) = lc;
			}
		}else{
			*(ReturnBuf + (i++)) = hc;
		}
	}
	*(ReturnBuf + i) = '\0';
	return ReturnBuf;
}

unsigned char *CMojiCode::Jis2SJis(unsigned char *buf)
{
	unsigned char *ReturnBuf = (unsigned char *)GlobalAlloc(GPTR, (DWORD)(strlen((const char *)buf) + 1));
	if(!ReturnBuf) return NULL;
	int i = 0;
	int hc, lc;
	BOOL kanji = FALSE;
	// JISからSJISに
	for(unsigned char *p = buf; *p != '\0'; p++){
		hc = *p;
		if(hc == ESC_CHAR){
			if(*(p + 1) == '$'){
				if(*(p + 2) == 'B' || *(p + 2) == '@'){
					// JIS開始
					kanji = TRUE;
					p += 2;
				}else{
					*(ReturnBuf + (i++)) = hc;
				}
			}else
			if(*(p + 1) == '('){
				if(*(p + 2) == 'B' || *(p + 2) == 'J'){
					// JIS終了
					kanji = FALSE;
					p += 2;
				}else{
					*(ReturnBuf + (i++)) = hc;
				}
			}else
			if(*(p + 1) == 'K'){
				// NECJIS開始
				kanji = TRUE;
				p++;
			}else
			if(*(p + 1) == 'H'){
				// NECJIS終了
				kanji = FALSE;
				p++;
			}else{
				*(ReturnBuf + (i++)) = hc;
			}
		}else
		if(kanji && hc >= 0x21 && hc <= 0x7E){
			lc = *(++p);
			if(lc >= 0x21 && lc <= 0x7E)
			if(hc & 1){
				if(lc < 0x60){
					lc += 0x1F;
				}else{
					lc += 0x20;
				}
			}else{
				lc += 0x7E;
			}
			if(hc < 0x5F){
				hc = (hc + 0xE1) >> 1;
			}else{
				hc = (hc + 0x161) >> 1;
			}
			*(ReturnBuf + (i++)) = hc;
			*(ReturnBuf + (i++)) = lc;
		}else{
			*(ReturnBuf + (i++)) = hc;
		}
	}
	*(ReturnBuf + i) = '\0';
	return ReturnBuf;
}

unsigned char *CMojiCode::Euc2SJis(unsigned char *buf)
{
	unsigned char *ReturnBuf = (unsigned char *)GlobalAlloc(GPTR, (DWORD)(strlen((const char *)buf) + 1));
	if(!ReturnBuf) return NULL;
	int i = 0;
	int hc, lc;
	// EUCからSJISに
	for(unsigned char *p = buf; *p != '\0'; p++){
		hc = *p;
		if(hc >= 0xA1 && hc <= 0xFE){
			lc = *(++p);
			if(lc >= 0xA1 && lc <= 0xFE) {
				hc &= 0x7F;
				lc &= 0x7F;
				if(hc & 1){
					if(lc < 0x60){
						lc += 0x1F;
					}else{
						lc += 0x20;
					}
				}else{
					lc += 0x7E;
				}
				if(hc < 0x5F){
					hc = (hc + 0xE1) >> 1;
				}else{
					hc = (hc + 0x161) >> 1;
				}
			}
			*(ReturnBuf + (i++)) = hc;
			*(ReturnBuf + (i++)) = lc;
		}else{
			*(ReturnBuf + (i++)) = hc;
		}
	}
	*(ReturnBuf + i) = '\0';
	return ReturnBuf;
}

char *CMojiCode::URLDecode(char *buf)
{
	char *ReturnBuf = (char *)GlobalAlloc(GPTR, (DWORD)(strlen((const char *)buf) + 1));
	if(!ReturnBuf) return NULL;

	char *p;
	int nLen, nHex;
	for(p = buf, nLen = nHex = 0; *p != '\0'; p++){
		if(*p == '%'){
			nHex = 1;
			continue;
		}
		if(nHex == 1){
			// 1Byte目
			ReturnBuf[nLen] = Hex2Char(*p);
			ReturnBuf[nLen] *= 16;
			p++;
			// 2Byte目
			ReturnBuf[nLen] += Hex2Char(*p);
			nHex = 0;
		}else{
			if(*p == '+'){
				ReturnBuf[nLen] = ' ';
			}else{
				ReturnBuf[nLen] = *p;
			}
		}
		nLen++;
	}
	ReturnBuf[nLen] = '\0';
	return ReturnBuf;
}

char* CMojiCode::URLEncode(const char* buf)
{
	if (!buf) return NULL;

	size_t nSrcLen = strlen(buf);
	size_t nBufSize = (nSrcLen * 3) + 1;

	char* ReturnBuf =
		(char*)GlobalAlloc(GPTR, nBufSize);

	if (!ReturnBuf) return NULL;

	char szVal[5] = { 0 };

	for (const unsigned char* p =
		(const unsigned char*)buf;
		*p != '\0';
		p++)
	{
		szVal[0] = '\0';

		if (*p == ' ') {
			strcpy_s(szVal, sizeof(szVal), "+");
		}
		else
			if ((*p >= '0' && *p <= '9') ||
				(*p >= 'a' && *p <= 'z') ||
				(*p >= 'A' && *p <= 'Z'))
			{
				szVal[0] = (char)*p;
				szVal[1] = '\0';
			}
			else {
				wsprintf(
					szVal,
					"%%%02X",
					*p);
			}

		strcat_s(ReturnBuf, nBufSize, szVal);
	}

	return ReturnBuf;
}


wchar_t *CMojiCode::SJis2UTF16(char *buf)
{
	setlocale(LC_ALL, "Japanese");
	return ToWideChar(buf, CP_ACP);
}

char *CMojiCode::UTF162SJis(wchar_t *buf)
{
	setlocale(LC_ALL, "Japanese");
	return ToMultiByte(buf, CP_ACP);
}

char* CMojiCode::SJis2UTF8(char* buf)
{
	setlocale(LC_ALL, "Japanese");

	wchar_t* wBuf = ToWideChar(buf, CP_ACP);
	if (!wBuf) return NULL;

	char* ReturnBuf = ToMultiByte(wBuf, CP_UTF8);

	GlobalFree(wBuf);

	return ReturnBuf;
}

char* CMojiCode::UTF82SJis(char* buf)
{
	if (!buf) return NULL;

	char* pszUtf8 = buf;

	if ((unsigned char)buf[0] == 0xEF &&
		(unsigned char)buf[1] == 0xBB &&
		(unsigned char)buf[2] == 0xBF) {
		pszUtf8 = buf + 3;
	}

	wchar_t* wBuf = ToWideChar(pszUtf8, CP_UTF8);
	if (!wBuf) return NULL;

	char* ReturnBuf = ToMultiByte(wBuf, CP_ACP);

	GlobalFree(wBuf);

	return ReturnBuf;
}

wchar_t *CMojiCode::ToWideChar(char *buf, UINT CodePage/* = CP_ACP*/)
{
	int len = MultiByteToWideChar(CodePage, 0, (PCSTR)buf, -1, NULL, 0);
	if(len < 1) return NULL;
	LPWSTR ReturnBuf = (LPWSTR)GlobalAlloc(GPTR, (DWORD)(len * sizeof(wchar_t)) + sizeof(wchar_t));
	if(!ReturnBuf) return NULL;
	if(MultiByteToWideChar(CodePage, 0, (PCSTR)buf, -1, ReturnBuf, len) < 1){
		GlobalFree(ReturnBuf);
		return NULL;
	}
	*(ReturnBuf + len) = L'\0';
	return (wchar_t *)ReturnBuf;
}

char *CMojiCode::ToMultiByte(wchar_t *buf, UINT CodePage/* = CP_ACP*/)
{
	int len = WideCharToMultiByte(CodePage, 0, (PCWSTR)buf, -1, NULL, 0, NULL, NULL);
	if(len < 1) return NULL;
	char *ReturnBuf = (char *)GlobalAlloc(GPTR, (DWORD)(len * sizeof(char)) + sizeof(char));
	if(!ReturnBuf) return NULL;
	if(WideCharToMultiByte(CodePage, 0, (PCWSTR)buf, -1, ReturnBuf, len, NULL, NULL) < 1){
		GlobalFree(ReturnBuf);
		return NULL;
	}
	*(ReturnBuf + len) = '\0';

	return ReturnBuf;
}
