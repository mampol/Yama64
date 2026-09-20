/*----------------------------------------------------------------------------------------

	Export.h

----------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
	定義
------------------------------------------------------------------------------*/
#define DEFAULT_EXPORT_FILE			(LPCTSTR)"解析結果.csv"
#define DEFAULT_EXPORT_FILE_HTML	(LPCTSTR)"解析結果.html"
#define DEFAULT_EXPORT_FILE_CBL		(LPCTSTR)"解析結果.cob"

#define HTML_HEADER					(LPCTSTR)"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"ja\" lang=\"ja\" dir=\"ltr\">\n"	\
									"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=Shift_JIS\" />\n"	\
									"<title>解析結果 - Yamamoto -</title>\n"	\
									"<meta http-equiv=\"content-style-type\" content=\"text/css\" />\n"	\
									"<meta http-equiv=\"content-script-type\" content=\"text/javascript\" />\n"	\
									"<meta http-equiv=\"content-language\" content=\"ja\" />\n"	\
									"<meta name=\"author\" content=\"author\" />\n"	\
									"<meta name=\"copyright\" content=\"Yama64\" />\n"	\
									"<style TYPE=\"text/css\">\n"	\
									"<!--\n"	\
									"BODY { font-size:80%%; }\n"	\
									"-->\n"	\
									"</style>\n"	\
									"<!-- Maked Yama64 //-->\n"	\
									"</head>\n"	\
									"<body>\n"
#define HTML_BODY_RED				(LPCTSTR)"<span style=\"margin:1px 0px 1px 0px;padding:4px;border-color:#990000;border-width:1px;border-style:solid;height:90px;width:%dpx;background:#FEC6C6;\">%s</span>\n"
#define HTML_BODY_GREEN				(LPCTSTR)"<span style=\"margin:1px 0px 1px 0px;padding:4px;border-color:#009900;border-width:1px;border-style:solid;height:90px;width:%dpx;background:#ADFEAD;\">%s</span>\n"
#define HTML_BODY_YELLOW			(LPCTSTR)"<span style=\"margin:1px 0px 1px 0px;padding:4px;border-color:#999933;border-width:1px;border-style:solid;height:90px;width:%dpx;background:#F9F9BA;\">%s</span>\n"
#define HTML_BODY_GLAY				(LPCTSTR)"<span style=\"margin:1px 0px 1px 0px;padding:4px;border-color:#333333;border-width:1px;border-style:solid;height:90px;width:%dpx;background:#CCCCCC;\">%s</span>\n"
#define HTML_BODY_WHITE				(LPCTSTR)"<span style=\"margin:1px 0px 1px 0px;padding:4px;border-color:#CCCCCC;border-width:1px;border-style:solid;height:90px;width:%dpx;background:#F7F7F7;\">%s</span>\n"
#define HTML_TAIL					(LPCTSTR)"</body>\n"	\
									"</html>\n"

#define CSV_HEADER					(LPCTSTR)"解析結果 - Yama64 -\r\n"

/*------------------------------------------------------------------------------
	マクロ
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
	関数プロトタイプ
------------------------------------------------------------------------------*/
int ExportAnalysysHTML(HWND hWndMain);
int ExportAnalysysCSV(HWND hWndMain);
int ExportAnalysysCBL(HWND hWndMain);
