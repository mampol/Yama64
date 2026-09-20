/*----------------------------------------------------------------------------------------

	Export.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "Export.h"

class CHandleGuard
{
public:
	explicit CHandleGuard(HANDLE h = INVALID_HANDLE_VALUE)
		: m_hHandle(h)
	{
	}

	~CHandleGuard()
	{
		if (m_hHandle != INVALID_HANDLE_VALUE &&
			m_hHandle != NULL)
		{
			CloseHandle(m_hHandle);
		}
	}

private:
	HANDLE m_hHandle;

	CHandleGuard(const CHandleGuard&) = delete;
	CHandleGuard& operator=(const CHandleGuard&) = delete;
};

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
int ExportHTML(HWND hWndMain, const char *pszExportFile);
int ExportCSV(HWND hWndMain, const char *pszExportFile);
int ExportCBL(HWND hWndMain, const char *pszExportFile);

/*----------------------------------------------------------------------------------------
	解析結果をファイルに出力
----------------------------------------------------------------------------------------*/
int ExportAnalysysHTML(HWND hWndMain)
{
	static char szExportFile[MAX_PATH + 1];
//	if(strlen(szExportFile) == 0) strcpy(szExportFile, DEFAULT_EXPORT_FILE_HTML);
	if(strlen(szExportFile) == 0) strcpy_s(szExportFile, sizeof(szExportFile), DEFAULT_EXPORT_FILE_HTML);
	if(!GetSaveFileDlg(hWndMain,
		szExportFile,
		(const int)sizeof(szExportFile),
		"HTMLﾌｧｲﾙ (*.html,*htm)\0*.html,*.htm\0"
		"全てのﾌｧｲﾙ (*.*)\0*.*\0",
		"解析結果をﾌｧｲﾙに出力")) return 0;

	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return 0;
	lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙへの出力を開始します");

	lpCStatus->SetSbText(0, SBT_NOBORDERS, "HTML形式で出力中…");
	int nReturnValue = ExportHTML(hWndMain, szExportFile);
	if(nReturnValue < 0){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙ出力でｴﾗｰです");
	}else{
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙへの出力を完了");
	}
	return nReturnValue;
}

/*----------------------------------------------------------------------------------------
	解析結果をＨＴＭＬで出力
----------------------------------------------------------------------------------------*/
int ExportHTML(HWND hWndMain, const char *pszExportFile)
{
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	int nSrc = lpCAnaCBL->GetSourceCount(),
		nRec = lpCAnaCBL->GetCount_Record();
	if(nSrc < 1 && nRec < 1){
		EXCMSG(hWndMain, "解析結果はありません");
		return -1;
	}
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return -1;
	// ファイルを開く
	HANDLE hFile = CreateFile(
		pszExportFile,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		EXCMSG(hWndMain, "指定されたファイルが開けません");
		return -1;
	}
	CHandleGuard hFileGuard(hFile);

	// ファイル出力
	char szValue[MAX_BUFFER], szRecord[1024*64];
	DWORD dwWrite;
	ZeroMemory(szRecord, sizeof(szRecord));

	wsprintf(szRecord, HTML_HEADER, lpCAnaCBL->GetTotal());
	if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}

	int nSrcCount = lpCAnaCBL->GetSourceCount();
	char szSrcPath[MAX_PATH];
	for(int i = 0; i < nSrcCount; i++){
		lpCAnaCBL->GetSourcePath(i, szSrcPath, sizeof(szSrcPath));
		wsprintf(szValue, "No.%02d", i + 1);
		wsprintf(szRecord, HTML_BODY_GLAY, 110, szValue);
		if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
		wsprintf(szRecord, HTML_BODY_WHITE, 600, szSrcPath);
		if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
		if(!WriteFile(hFile, (LPCVOID)"<BR><BR>\n", 9, &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
	}
	wsprintf(szRecord, HTML_BODY_GLAY, 140, "合計サイズ");
	if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}
	DWORD dwTotalLength = lpCAnaCBL->GetTotal();
	wsprintf(szValue, "%d Byte", dwTotalLength);
	wsprintf(szRecord, HTML_BODY_WHITE, 100, szValue);
	if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}
	if(!WriteFile(hFile, (LPCVOID)"<BR><BR>\n", 9, &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}

	const int nWidth[] = {
		70, 70, 70, 300, 90, 130, 130, 90, 90, 120, 120, 120, 120, 120, 120, 160
	};

	int nExportProp = lpCSetup->GetFirstExportProp();
	if(nExportProp < 0){
		EXCMSG(hWndMain, "エクスポートの設定に誤りがあります");
		return -1;
	}
	while(nExportProp >= 0){
		if(lpCSetup->GetExportPropString(nExportProp, szValue, sizeof(szValue)) < 0) break;
		wsprintf(szRecord, HTML_BODY_GLAY, nWidth[nExportProp], szValue);
		if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
		nExportProp = lpCSetup->GetNextExportProp();
	}
	if(!WriteFile(hFile, (LPCVOID)"<BR><BR>\n", 9, &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}

	LPRECORD_CBL lpRecCbl = lpCAnaCBL->GetFirst_Record(), lpRecCblTmp = NULL;
	do{
		nExportProp = lpCSetup->GetFirstExportProp();
		if(nExportProp < 0){
			EXCMSG(hWndMain, "エクスポートでエラーが発生しました");
			return -1;
		}
		while(nExportProp >= 0){
			ZeroMemory(szRecord, sizeof(szRecord));
			ZeroMemory(szValue, sizeof(szValue));
			char szFormat[1024];
			if(lpRecCbl->bOccurs){
//				strcpy(szFormat, HTML_BODY_YELLOW);
				strcpy_s(szFormat, sizeof(szFormat), HTML_BODY_YELLOW);
			}else{
				if(lpRecCbl->bRedefines){
//					strcpy(szFormat, HTML_BODY_RED);
					strcpy_s(szFormat, sizeof(szFormat), HTML_BODY_RED);
				}else{
					if(lpRecCbl->bStruct){
//						strcpy(szFormat, HTML_BODY_GREEN);
						strcpy_s(szFormat, sizeof(szFormat), HTML_BODY_GREEN);
					}else{
//						strcpy(szFormat, HTML_BODY_WHITE);
						strcpy_s(szFormat, sizeof(szFormat), HTML_BODY_WHITE);
					}
				}
			}
			switch(nExportProp){
			case EXPORT_PHYLINE:		// 物理行
				wsprintf(szValue, "%d", lpRecCbl->nPhyLine + 1);
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_LOGLINE:		// 論理行
				wsprintf(szValue, "%d", lpRecCbl->nLogLine + 1);
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_LEVEL:			// レベル番号
				wsprintf(szValue, "%02d", lpRecCbl->unLevel);
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_DESC:			// 識別子
				wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szDescriptor);
				break;
			case EXPORT_TYPE:			// 変数サイズ定義
				if(lpRecCbl->bStruct){
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&lt;struct&gt;");
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szTypeDef);
				}
				break;
			case EXPORT_SIZE:			// 変数のバッファサイズ
				if(lpRecCbl->bStruct){
					wsprintf(szValue, "(%dByte)", lpRecCbl->dwTypeSize);
				}else{
					wsprintf(szValue, "%dByte", lpRecCbl->dwTypeSize);
				}
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_POS:			// 変数のバッファ位置
				wsprintf(szValue, "%dByte", lpRecCbl->dwTypePos);
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_STRUCTLV:		// 構造体項目識別用ＩＤ
				wsprintf(szValue, "%d", lpRecCbl->nStructGroupIndex);
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_OCCURS:			// 繰り返し項目
				if(lpRecCbl->unOccursGroupId > 0){
					wsprintf(szValue, "%d", lpRecCbl->nOccursOrder + 1);
				}else{
//					strcpy(szValue, "&nbsp;");
					strcpy_s(szValue, sizeof(szValue), "&nbsp;");
				}
				wsprintf(szRecord, szFormat, nWidth[nExportProp], szValue);
				break;
			case EXPORT_OCCINDEXED:		// 繰り返し項目の指数定義
				if(lpRecCbl->unOccursGroupId > 0){
					if(strlen(lpRecCbl->szOccursIndexed) > 0){
						wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szOccursIndexed);
					}else{
						wsprintf(szRecord, szFormat, "&nbsp;");
					}
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			case EXPORT_REDEFINES:		// 再定義項目
				if(lpRecCbl->bRedefines){
					wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szRedefines);
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			case EXPORT_REDSTRUCT:		// 再定義項目
				if(lpRecCbl->bRedefines){
					lpRecCblTmp = lpCAnaCBL->Get_Record(lpRecCbl->nRedefinesRecIndex);
					if(lpRecCblTmp){
						wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCblTmp->szDescriptor);
					}else{
						wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
					}
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			case EXPORT_COMPRESS:		// 32bit変数定義
				if(strlen(lpRecCbl->szCompress) > 0){
					wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szCompress);
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			case EXPORT_VALUE:			// データ
				if(strlen(lpRecCbl->szValue) > 0){
					wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szValue);
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			case EXPORT_VALUEEX:		// データ拡張
				if(strlen(lpRecCbl->szValueEx) > 0){
					wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szValueEx);
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			case EXPORT_COMMENT:		// コメント
				if(strlen(lpRecCbl->szComment) > 0){
					wsprintf(szRecord, szFormat, nWidth[nExportProp], lpRecCbl->szComment);
				}else{
					wsprintf(szRecord, szFormat, nWidth[nExportProp], "&nbsp;");
				}
				break;
			}
			if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
				EXCMSG(hWndMain, "ファイルが出力できません");
				return -1;
			}
			nExportProp = lpCSetup->GetNextExportProp();
		}
		if(!WriteFile(hFile, (LPCVOID)"<BR><BR>\n", 9, &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
	} while((lpRecCbl = lpCAnaCBL->GetNext_Record()));

	if(!WriteFile(hFile, (LPCVOID)HTML_TAIL, (DWORD)strlen(HTML_TAIL), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}
	SetEndOfFile(hFile);
	return 0;
}

/*----------------------------------------------------------------------------------------
	解析結果をファイルに出力
----------------------------------------------------------------------------------------*/
int ExportAnalysysCSV(HWND hWndMain)
{
	static char szExportFile[MAX_PATH + 1];
//	if(strlen(szExportFile) == 0) strcpy(szExportFile, DEFAULT_EXPORT_FILE);
	if(strlen(szExportFile) == 0) strcpy_s(szExportFile, sizeof(szExportFile), DEFAULT_EXPORT_FILE);
	if(!GetSaveFileDlg(hWndMain,
		szExportFile,
		(const int)sizeof(szExportFile),
		"CSVﾌｧｲﾙ (*.csv)\0*.csv\0"
		"全てのﾌｧｲﾙ (*.*)\0*.*\0",
		"解析結果をﾌｧｲﾙに出力")) return 0;

	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return 0;
	lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙへの出力を開始します");

	lpCStatus->SetSbText(0, SBT_NOBORDERS, "CSV形式で出力中…");
	int nReturnValue = ExportCSV(hWndMain, szExportFile);
	if(nReturnValue < 0){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙ出力でｴﾗｰです…");
	}else{
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙへの出力を完了したよ");
	}
	return nReturnValue;
}

/*----------------------------------------------------------------------------------------
	解析結果をＣＳＶで出力
----------------------------------------------------------------------------------------*/
int ExportCSV(HWND hWndMain, const char *pszExportFile)
{
	int nCols = 0, nCount = 0;
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	int nSrc = lpCAnaCBL->GetSourceCount(),
		nRec = lpCAnaCBL->GetCount_Record();
	if(nSrc < 1 && nRec < 1){
		EXCMSG(hWndMain, "解析結果はありません");
		return -1;
	}
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return -1;
	// ファイルを開く
	HANDLE hFile = CreateFile(
		pszExportFile,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		EXCMSG(hWndMain, "指定されたファイルが開けません");
		return -1;
	}
	CHandleGuard hFileGuard(hFile);

	// ファイル出力
	char szValue[MAX_BUFFER], szRecord[1024*64];
	DWORD dwWrite;
	ZeroMemory(szRecord, sizeof(szRecord));

	wsprintf(szRecord, CSV_HEADER);
	if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}

	int nSrcCount = lpCAnaCBL->GetSourceCount();
	char szSrcPath[MAX_PATH];
	for(int i = 0; i < nSrcCount; i++){
		lpCAnaCBL->GetSourcePath(i, szSrcPath, sizeof(szSrcPath));
		wsprintf(szRecord, "No.%02d,%s\r\n", i + 1, szSrcPath);
		if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
	}
	DWORD dwTotalLength = lpCAnaCBL->GetTotal();
	wsprintf(szRecord, "合計サイズ,%d Byte\r\n", dwTotalLength);
	if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}

	ZeroMemory(szRecord, sizeof(szRecord));
	int nExportProp = lpCSetup->GetFirstExportProp();
	if(nExportProp < 0){
		EXCMSG(hWndMain, "エクスポートの設定に誤りがあります");
		return -1;
	}
	while(nExportProp >= 0){
		if(lpCSetup->GetExportPropString(nExportProp, szValue, sizeof(szValue)) < 0) break;
//		strcat(szRecord, szValue);
//		strcat(szRecord, ",");
		strcat_s(szRecord, sizeof(szRecord), szValue);
		strcat_s(szRecord, sizeof(szRecord), ",");
		nExportProp = lpCSetup->GetNextExportProp();
	}
	szRecord[strlen(szRecord) - 1] = '\0';
//	strcat(szRecord, "\r\n");
	strcat_s(szRecord, sizeof(szRecord), "\r\n");
	if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
		EXCMSG(hWndMain, "ファイルが出力できません");
		return -1;
	}

	LPRECORD_CBL lpRecCbl = lpCAnaCBL->GetFirst_Record(), lpRecCblTmp = NULL;
	do{
		ZeroMemory(szRecord, sizeof(szRecord));
		nExportProp = lpCSetup->GetFirstExportProp();
		if(nExportProp < 0){
			EXCMSG(hWndMain, "エクスポートでエラーが発生しました");
			return -1;
		}
		while(nExportProp >= 0){
			ZeroMemory(szValue, sizeof(szValue));
			switch(nExportProp){
			case EXPORT_PHYLINE:		// 物理行
				wsprintf(szValue, "%d", lpRecCbl->nPhyLine + 1);
//				strcat(szRecord, szValue);
				strcat_s(szRecord, sizeof(szRecord), szValue);
				break;
			case EXPORT_LOGLINE:		// 論理行
				wsprintf(szValue, "%d", lpRecCbl->nLogLine + 1);
//				strcat(szRecord, szValue);
				strcat_s(szRecord, sizeof(szRecord), szValue);
				break;
			case EXPORT_LEVEL:			// レベル番号
				wsprintf(szValue, "%02d", lpRecCbl->unLevel);
//				strcat(szRecord, szValue);
				strcat_s(szRecord, sizeof(szRecord), szValue);
				break;
			case EXPORT_DESC:			// 識別子
//				strcat(szRecord, lpRecCbl->szDescriptor);
				strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szDescriptor);
				break;
			case EXPORT_TYPE:			// 変数サイズ定義
				if(lpRecCbl->bStruct){
//					strcat(szRecord, "<struct>");
					strcat_s(szRecord, sizeof(szRecord), "<struct>");
				}else{
//					strcat(szRecord, lpRecCbl->szTypeDef);
					strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szTypeDef);
				}
				break;
			case EXPORT_SIZE:			// 変数のバッファサイズ
				if(lpRecCbl->bStruct){
					wsprintf(szValue, "(%dByte)", lpRecCbl->dwTypeSize);
				}else{
					wsprintf(szValue, "%dByte", lpRecCbl->dwTypeSize);
				}
//				strcat(szRecord, szValue);
				strcat_s(szRecord, sizeof(szRecord), szValue);
				break;
			case EXPORT_POS:			// 変数のバッファ位置
				wsprintf(szValue, "%dByte", lpRecCbl->dwTypePos);
//				strcat(szRecord, szValue);
				strcat_s(szRecord, sizeof(szRecord), szValue);
				break;
			case EXPORT_STRUCTLV:		// 構造体項目識別用ＩＤ
				wsprintf(szValue, "%d", lpRecCbl->nStructGroupIndex);
//				strcat(szRecord, szValue);
				strcat_s(szRecord, sizeof(szRecord), szValue);
				break;
			case EXPORT_OCCURS:			// 繰り返し項目
				if(lpRecCbl->unOccursGroupId > 0){
					wsprintf(szValue, "%d", lpRecCbl->nOccursOrder + 1);
//					strcat(szRecord, szValue);
					strcat_s(szRecord, sizeof(szRecord), szValue);
				}
				break;
			case EXPORT_OCCINDEXED:		// 繰り返し項目の指数定義
				if(lpRecCbl->unOccursGroupId > 0){
//					if(strlen(lpRecCbl->szOccursIndexed) > 0) strcat(szRecord, lpRecCbl->szOccursIndexed);
					if(strlen(lpRecCbl->szOccursIndexed) > 0) strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szOccursIndexed);
				}
				break;
			case EXPORT_REDEFINES:		// 再定義項目
//				if(lpRecCbl->bRedefines) strcat(szRecord, lpRecCbl->szRedefines);
				if(lpRecCbl->bRedefines) strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szRedefines);
				break;
			case EXPORT_REDSTRUCT:		// 再定義項目
				if(lpRecCbl->bRedefines){
					lpRecCblTmp = lpCAnaCBL->Get_Record(lpRecCbl->nRedefinesRecIndex);
//					if(lpRecCblTmp) strcat(szRecord, lpRecCblTmp->szDescriptor);
					if(lpRecCblTmp) strcat_s(szRecord, sizeof(szRecord), lpRecCblTmp->szDescriptor);
				}
				break;
			case EXPORT_COMPRESS:		// 32bit変数定義
//				strcat(szRecord, lpRecCbl->szCompress);
				strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szCompress);
				break;
			case EXPORT_VALUE:			// データ
//				strcat(szRecord, lpRecCbl->szValue);
				strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szValue);
				break;
			case EXPORT_VALUEEX:		// データ拡張
//				strcat(szRecord, lpRecCbl->szValueEx);
				strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szValueEx);
				break;
			case EXPORT_COMMENT:		// コメント
//				strcat(szRecord, lpRecCbl->szComment);
				strcat_s(szRecord, sizeof(szRecord), lpRecCbl->szComment);
				break;
			}
//			strcat(szRecord, ",");
			strcat_s(szRecord, sizeof(szRecord), ",");
			nExportProp = lpCSetup->GetNextExportProp();
		}
		szRecord[strlen(szRecord) - 1] = '\0';
//		strcat(szRecord, "\r\n");
		strcat_s(szRecord, sizeof(szRecord), "\r\n");
		if(!WriteFile(hFile, (LPCVOID)szRecord, (DWORD)strlen(szRecord), &dwWrite, NULL)){
			EXCMSG(hWndMain, "ファイルが出力できません");
			return -1;
		}
	} while((lpRecCbl = lpCAnaCBL->GetNext_Record()));

	SetEndOfFile(hFile);
	return 0;
}

/*----------------------------------------------------------------------------------------
	解析結果をファイルに出力
----------------------------------------------------------------------------------------*/
int ExportAnalysysCBL(HWND hWndMain)
{
	static char szExportFile[MAX_PATH + 1];
//	if(strlen(szExportFile) == 0) strcpy(szExportFile, DEFAULT_EXPORT_FILE_CBL);
	if(strlen(szExportFile) == 0) strcpy_s(szExportFile, sizeof(szExportFile), DEFAULT_EXPORT_FILE_CBL);
	if(!GetSaveFileDlg(hWndMain,
		szExportFile,
		(const int)sizeof(szExportFile),
		"COBOLｿｰｽﾌｧｲﾙ (*.cob;*.cbl;*.cobol)\0*.cob;*.cbl;*.cobol\0"
		"全てのﾌｧｲﾙ (*.*)\0*.*\0",
		"解析結果をﾌｧｲﾙに出力")) return 0;

	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return 0;
	lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙへの出力を開始します");

	lpCStatus->SetSbText(0, SBT_NOBORDERS, "COBOL形式で出力中…");
	int nReturnValue = ExportCBL(hWndMain, szExportFile);
	if(nReturnValue < 0){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙ出力でｴﾗｰです…");
	}else{
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌｧｲﾙへの出力を完了したよ");
	}
	return nReturnValue;
}

/*----------------------------------------------------------------------------------------
	解析結果をＣＯＢＯＬで出力
----------------------------------------------------------------------------------------*/
int ExportCBL(HWND hWndMain, const char *pszExportFile)
{
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	int nSrc = lpCAnaCBL->GetSourceCount(),
		nRec = lpCAnaCBL->GetCount_Record();
	if(nSrc < 1 && nRec < 1){
		EXCMSG(hWndMain, "解析結果はありません");
		return -1;
	}

	// ファイルを開く
	HANDLE hFile = CreateFile(
		pszExportFile,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		EXCMSG(hWndMain, "指定されたファイルが開けません");
		return -1;
	}
	CHandleGuard hFileGuard(hFile);

	// ソース連結出力
	int nCount = lpCAnaCBL->GetSourceCount();
	SOURCEINFO stSi;
	DWORD dwWrite = 0, dwTotalWrite = 0;
	for(int i = 0; i < nCount; i++){
		ZeroMemory(&stSi, sizeof(SOURCEINFO));
		if(lpCAnaCBL->GetSourceData(i, &stSi) == 0){
			if(!WriteFile(hFile, (LPCVOID)stSi.pszSourceCode, (DWORD)stSi.dwSourceSize, &dwWrite, NULL)){
				EXCMSG(hWndMain, "ファイルが出力できません");
				break;
			}
			dwTotalWrite += dwWrite;
		}
	}

	SetEndOfFile(hFile);

	return 0;
}
