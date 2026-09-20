/*------------------------------------------------------------------------------
	CYamaSetup.cpp
	設定ファイル操作
------------------------------------------------------------------------------*/
#include "CYamaSetup.h"

// コンストラクタ
CYamaSetup::CYamaSetup() :
m_nExportProp(0)
{
	// 解析設定
	m_AnalysysProp.m_nDefExpandLevel = 1;
	strcpy_s(m_AnalysysProp.m_szCblExt, sizeof(m_AnalysysProp.m_szCblExt), "cbl;cob;cobol;cpy;pco");
	m_AnalysysProp.m_bNoExpandRed = TRUE;
	m_AnalysysProp.m_bNoExpandOcc = TRUE;
	m_AnalysysProp.m_bBeginMsg = FALSE;
	m_AnalysysProp.m_bCmdLineSet = TRUE;
	m_AnalysysProp.m_bDropSet = TRUE;
	m_AnalysysProp.m_bDropInit = TRUE;
	m_AnalysysProp.m_bPasteSet = TRUE;
	m_AnalysysProp.m_bPasteInit = FALSE;
	// 詳細解析設定
	m_CoreAnalysysProp.m_nSeq = 6;
	m_CoreAnalysysProp.m_nMaxLine = 0;
	m_CoreAnalysysProp.m_bHostMode = FALSE;
	m_CoreAnalysysProp.m_szSign[0] = '#';
	m_CoreAnalysysProp.m_szSign[1] = ' ';
	m_CoreAnalysysProp.m_szSign[2] = ' ';
	m_CoreAnalysysProp.m_szSign[3] = ' ';
	m_CoreAnalysysProp.m_szSign[4] = ' ';
	m_CoreAnalysysProp.m_nCompPos = 0;
	m_CoreAnalysysProp.m_bUseCodeCheck = TRUE;
	// 出力設定
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nPhyLine = 0;			// 物理行
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nLogLine = -1;			// 論理行
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nLevel = 1;				// レベル番号
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nDescriptor = 2;			// 識別子
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypeDef = 3;			// 変数サイズ定義
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypeSize = 4;			// 変数のバッファサイズ
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypePos = 5;			// 変数のバッファ位置
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nStructLevel = -1;		// 構造体項目識別用ＩＤ
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nOccurs = 6;				// 繰り返し項目
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nOccursIndexed = -1;		// 繰り返し項目の指数定義
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nRedefines = -1;			// 再定義項目
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nRedefinesStruct = -1;	// 再定義項目構造体名
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nCompress = 7;			// 32bit変数定義
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nValue = -1;				// 初期値
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nValueEx = -1;			// データ拡張
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nComment = -1;			// コメント
	// パス設定
	m_PathProp.m_szDefOpenDir[0] = '\0';
	m_PathProp.m_szCobolOpen[0] = '\0';
	m_PathProp.m_szSmdOpen[0] = '\0';
	m_PathProp.m_szTmpDir[0] = '\0';
//	m_PathProp.m_bEnableTmpPath = FALSE;
	m_PathProp.m_nHistorySave = 4;
	// ＵＩ設定
	ZeroMemory(&(m_UIProp.m_rcMainWnd), sizeof(RECT));
	m_UIProp.m_bMainWndMaximize = FALSE;
	m_UIProp.m_bShowOutputWnd = TRUE;
	m_UIProp.m_nFontSize = 10;
//	strcpy(m_UIProp.m_szFontFace, "ＭＳ ゴシック");
	strcpy_s(m_UIProp.m_szFontFace, sizeof(m_UIProp.m_szFontFace), "ＭＳ ゴシック");
	// 検索設定
	m_SearchProp.m_dwCmdFlags = 0;
	m_SearchProp.m_dwCmdExFlags = 0x00000001;
	m_SearchProp.m_dwSrcFlags = 0x000000FF;
	// 編集ダイアログ
	m_EditDialogProp.nWidth = -1;
	m_EditDialogProp.nHeight = -1;
	m_EditDialogProp.nX = -1;
	m_EditDialogProp.nY = -1;
}

// デストラクタ
CYamaSetup::~CYamaSetup()
{
}

// 設定ファイル読込
int CYamaSetup::ReadSetup()
{
	// 解析設定
	m_AnalysysProp.m_nDefExpandLevel = (unsigned int)GetInt(INI_SEC_ANALISYS, INI_KEY_DEFEXPLEVEL, 1);
	GetString(INI_SEC_ANALISYS, INI_KEY_CBLEXT, "cbl;cob;cobol;cpy;pco", m_AnalysysProp.m_szCblExt, (int)sizeof(m_AnalysysProp.m_szCblExt));
	m_AnalysysProp.m_bBeginMsg = (BOOL)GetInt(INI_SEC_ANALISYS, INI_KEY_BEGINMSG, (int)FALSE);
	m_AnalysysProp.m_bCmdLineSet = (BOOL)GetInt(INI_SEC_ANALISYS, INI_KEY_CMDLINESET, (int)TRUE);
	m_AnalysysProp.m_bDropSet = (BOOL)GetInt(INI_SEC_ANALISYS, INI_KEY_DROPSET, (int)TRUE);
	m_AnalysysProp.m_bDropInit = (BOOL)GetInt(INI_SEC_ANALISYS, INI_KEY_DROPINIT, (int)TRUE);
	m_AnalysysProp.m_bPasteSet = (BOOL)GetInt(INI_SEC_ANALISYS, INI_KEY_PASTESET, (int)TRUE);
	m_AnalysysProp.m_bPasteInit = (BOOL)GetInt(INI_SEC_ANALISYS, INI_KEY_PASTEINIT, (int)FALSE);
	if(m_AnalysysProp.m_nDefExpandLevel <= 0 || m_AnalysysProp.m_nDefExpandLevel > 999) m_AnalysysProp.m_nDefExpandLevel = 1;

	// 詳細解析設定
	m_CoreAnalysysProp.m_nSeq = GetInt(INI_SEC_COREANALISYS, INI_KEY_SEQ, 6);
	m_CoreAnalysysProp.m_nMaxLine = GetInt(INI_SEC_COREANALISYS, INI_KEY_MAXLINE, 0);
	m_CoreAnalysysProp.m_bHostMode = (BOOL)GetInt(INI_SEC_COREANALISYS, INI_KEY_HOSTMODE, (int)FALSE);
	m_CoreAnalysysProp.m_szSign[0] = (char)GetInt(INI_SEC_COREANALISYS, INI_KEY_SIGN1, (int)0x23);
	m_CoreAnalysysProp.m_szSign[1] = (char)GetInt(INI_SEC_COREANALISYS, INI_KEY_SIGN2, (int)0x20);
	m_CoreAnalysysProp.m_szSign[2] = (char)GetInt(INI_SEC_COREANALISYS, INI_KEY_SIGN3, (int)0x20);
	m_CoreAnalysysProp.m_szSign[3] = (char)GetInt(INI_SEC_COREANALISYS, INI_KEY_SIGN4, (int)0x20);
	m_CoreAnalysysProp.m_szSign[4] = (char)GetInt(INI_SEC_COREANALISYS, INI_KEY_SIGN5, (int)0x20);
	m_CoreAnalysysProp.m_nCompPos = GetInt(INI_SEC_COREANALISYS, INI_KET_COMPPOS, 0);
	GetString(INI_SEC_COREANALISYS, INI_KET_SMDEXT, "smd", m_CoreAnalysysProp.m_szSmdExt, (int)sizeof(m_CoreAnalysysProp.m_szSmdExt));
	m_CoreAnalysysProp.m_bUseCodeCheck = (BOOL)GetInt(INI_SEC_COREANALISYS, INI_KET_CODECHECK, (int)TRUE);
	if(m_CoreAnalysysProp.m_nSeq <= 0 || m_CoreAnalysysProp.m_nSeq > 99) m_CoreAnalysysProp.m_nSeq = 6;
	if(m_CoreAnalysysProp.m_nMaxLine <= 0 || m_CoreAnalysysProp.m_nMaxLine > 2000) m_CoreAnalysysProp.m_nMaxLine = 0;

	// 出力設定
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nPhyLine = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_PHYLINE, (int)0);				// 物理行
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nLogLine = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_LOGLINE, (int)-1);				// 論理行
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nLevel = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_LEVEL, (int)1);					// レベル番号
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nDescriptor = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_DESCRIPT, (int)2);			// 識別子
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypeDef = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_TYPEDEF, (int)3);				// 変数サイズ定義
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypeSize = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_TYPESIZE, (int)4);				// 変数のバッファサイズ
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypePos = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_TYPEPOS, (int)5);				// 変数のバッファ位置
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nStructLevel = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_STRUCTLV, (int)-1);			// 構造体項目識別用ＩＤ
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nOccurs = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_OCCURS, (int)6);					// 繰り返し項目
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nOccursIndexed = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_OCCINDEXED, (int)-1);		// 繰り返し項目の指数定義
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nRedefines = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_REDEFINES, (int)-1);			// 再定義項目
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nRedefinesStruct = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_REDSTRUCT, (int)-1);	// 再定義項目構造体名
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nCompress = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_COMPRESS, (int)7);				// 32bit変数定義
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nValue = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_VALUE, (int)-1);					// 初期値
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nValueEx = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_VALUEEX, (int)-1);				// データ拡張
	m_ExportProp.un_ExportProp.m_ExportPropState.m_nComment = GetInt(INI_SEC_EXPORT, INI_KET_EXPORT_COMMENT, (int)-1);				// コメント

	// パス設定
	GetString(INI_SEC_PATH, INI_KET_DEFOPENDIR, "", m_PathProp.m_szDefOpenDir, (int)sizeof(m_PathProp.m_szDefOpenDir));
	GetString(INI_SEC_PATH, INI_KET_COBOLOPEN, "", m_PathProp.m_szCobolOpen, (int)sizeof(m_PathProp.m_szCobolOpen));
	GetString(INI_SEC_PATH, INI_KET_SMDOPEN, "", m_PathProp.m_szSmdOpen, (int)sizeof(m_PathProp.m_szSmdOpen));
	GetString(INI_SEC_PATH, INI_KET_TMPDIR, "", m_PathProp.m_szTmpDir, (int)sizeof(m_PathProp.m_szTmpDir));
	wsprintf(m_PathProp.m_szHisFile, "%s\\%s", m_szWorkDir, HIS_FILE_TITLE);	// 履歴ﾌｧｲﾙ
	m_PathProp.m_nHistorySave = GetInt(INI_SEC_PATH, INI_KET_OPENHISCNT, 4);
	// 存在確認
	if(strlen(m_PathProp.m_szDefOpenDir) > 0){
//		if(!ExistFile(m_PathProp.m_szDefOpenDir)) strcpy(m_PathProp.m_szDefOpenDir, "");
		if(!ExistFile(m_PathProp.m_szDefOpenDir)) strcpy_s(m_PathProp.m_szDefOpenDir, sizeof(m_PathProp.m_szDefOpenDir), "");
	}
	if(strlen(m_PathProp.m_szCobolOpen) > 0){
//		if(!ExistFile(m_PathProp.m_szCobolOpen)) strcpy(m_PathProp.m_szCobolOpen, "");
		if(!ExistFile(m_PathProp.m_szCobolOpen)) strcpy_s(m_PathProp.m_szCobolOpen, sizeof(m_PathProp.m_szCobolOpen), "");
	}
	if(strlen(m_PathProp.m_szSmdOpen) > 0){
//		if(!ExistFile(m_PathProp.m_szSmdOpen)) strcpy(m_PathProp.m_szSmdOpen, "");
		if(!ExistFile(m_PathProp.m_szSmdOpen)) strcpy_s(m_PathProp.m_szSmdOpen, sizeof(m_PathProp.m_szSmdOpen), "");
	}
	if(strlen(m_PathProp.m_szTmpDir) > 0){
//		if(!ExistFile(m_PathProp.m_szTmpDir)) strcpy(m_PathProp.m_szTmpDir, "");
		if(!ExistFile(m_PathProp.m_szTmpDir)) strcpy_s(m_PathProp.m_szTmpDir, sizeof(m_PathProp.m_szTmpDir), "");
	}
	// 履歴読み込み
	ReadHistory();

	// ＵＩ設定
	m_UIProp.m_bRestorMainWnd = (BOOL)GetInt(INI_SEC_UI, INI_KET_WNDRESTOR, (int)TRUE);
	m_UIProp.m_bItemImage = (BOOL)GetInt(INI_SEC_UI, INI_KET_ITEMIMAGE, (int)TRUE);
	m_UIProp.m_rcMainWnd.left = GetInt(INI_SEC_UI, INI_KET_WNDL, 0);
	m_UIProp.m_rcMainWnd.top = GetInt(INI_SEC_UI, INI_KET_WNDT, 0);
	m_UIProp.m_rcMainWnd.right = GetInt(INI_SEC_UI, INI_KET_WNDR, 0);
	m_UIProp.m_rcMainWnd.bottom = GetInt(INI_SEC_UI, INI_KET_WNDB, 0);
	m_UIProp.m_bMainWndMaximize = (BOOL)GetInt(INI_SEC_UI, INI_KET_WNDMAX, (int)FALSE);
	m_UIProp.m_bShowOutputWnd = (BOOL)GetInt(INI_SEC_UI, INI_KET_SHOWOUTPUTWND, (int)TRUE);
	m_UIProp.m_nOutPutWndSize = GetInt(INI_SEC_UI, INI_KET_OUTPUTWNDSIZE, (int)170);
	if(m_UIProp.m_nOutPutWndSize < 1) m_UIProp.m_nOutPutWndSize = 170;
	m_UIProp.m_nFontSize = GetInt(INI_SEC_UI, INI_KET_FONTSIZE, 9);
//	GetString(INI_SEC_UI, INI_KET_FONTFACE, "ＭＳ ゴシック", m_UIProp.m_szFontFace, (int)sizeof(m_UIProp.m_szFontFace));
	GetString(INI_SEC_UI, INI_KET_FONTFACE, "Meiryo UI", m_UIProp.m_szFontFace, (int)sizeof(m_UIProp.m_szFontFace));
	m_UIProp.m_colFore = (COLORREF)GetDword(INI_SEC_UI, INI_KET_FORCOL, UI_COL_FOR);
	m_UIProp.m_colBack = (COLORREF)GetDword(INI_SEC_UI, INI_KET_BAKCOL, UI_COL_BAK);
	m_UIProp.m_colStruct = (COLORREF)GetDword(INI_SEC_UI, INI_KET_STRUCTCOL, UI_COL_STRUCT);
	m_UIProp.m_colOccurs = (COLORREF)GetDword(INI_SEC_UI, INI_KET_OCCURSCOL, UI_COL_OCCURS);
	m_UIProp.m_colRedefines = (COLORREF)GetDword(INI_SEC_UI, INI_KET_REDEFINESCOL, UI_COL_REDEFINES);
	m_UIProp.m_colBinary = (COLORREF)GetDword(INI_SEC_UI, INI_KET_BINARYCOL, UI_COL_BINARY);
	m_UIProp.m_colSupplement = (COLORREF)GetDword(INI_SEC_UI, INI_KET_SUPPCOL, UI_COL_SUPP);
	m_UIProp.m_colDatCol = (COLORREF)GetDword(INI_SEC_UI, INI_KET_DATCOL, UI_COL_DATCOL);
	if(m_UIProp.m_nFontSize <= 0 || m_UIProp.m_nFontSize > 99) m_UIProp.m_nFontSize = 10;
//	if(strlen(m_UIProp.m_szFontFace) < 1) strcpy(m_UIProp.m_szFontFace, "ＭＳ ゴシック");
	if(strlen(m_UIProp.m_szFontFace) < 1) strcpy_s(m_UIProp.m_szFontFace, sizeof(m_UIProp.m_szFontFace), "Meiryo UI");
	m_UIProp.m_bUseBackColor = (BOOL)GetInt(INI_SEC_UI, INI_KET_USEBACKCOLOR, (int)FALSE);
	m_UIProp.m_bShowRecSelWnd = (BOOL)GetInt(INI_SEC_UI, INI_KET_SHOWRECSELWND, (int)FALSE);
	m_UIProp.m_nRecSelWndSize = GetInt(INI_SEC_UI, INI_KET_RECSELWNDSIZE, (int)300);
	if(m_UIProp.m_nRecSelWndSize < 1) m_UIProp.m_nRecSelWndSize = 300;

	// 検索設定
	m_SearchProp.m_dwCmdFlags = GetDword(INI_SEC_SEARCH, INI_KET_CMDFLG, 0);
	m_SearchProp.m_dwCmdExFlags = GetDword(INI_SEC_SEARCH, INI_KET_CMDFLGEX, 0x00000001);
	m_SearchProp.m_dwSrcFlags = GetDword(INI_SEC_SEARCH, INI_KET_SRCFLG, 0x000000FF);

	// 編集ダイアログ設定
	m_EditDialogProp.nWidth = GetInt(INI_SEC_EDITDLG, INI_KET_ED_WIDTH, -1);
	m_EditDialogProp.nHeight = GetInt(INI_SEC_EDITDLG, INI_KET_ED_HEIGHT, -1);
	m_EditDialogProp.nX = GetInt(INI_SEC_EDITDLG, INI_KET_ED_XPOS, -1);
	m_EditDialogProp.nY = GetInt(INI_SEC_EDITDLG, INI_KET_ED_YPOS, -1);

	return 0;
}

// 設定ファイル書き込み
int CYamaSetup::WriteSetup()
{
	// 解析設定
	WriteInt(INI_SEC_ANALISYS, INI_KEY_DEFEXPLEVEL, m_AnalysysProp.m_nDefExpandLevel);
	WriteString(INI_SEC_ANALISYS, INI_KEY_CBLEXT, m_AnalysysProp.m_szCblExt);
	WriteInt(INI_SEC_ANALISYS, INI_KEY_BEGINMSG, (int)m_AnalysysProp.m_bBeginMsg);
	WriteInt(INI_SEC_ANALISYS, INI_KEY_CMDLINESET, (int)m_AnalysysProp.m_bCmdLineSet);
	WriteInt(INI_SEC_ANALISYS, INI_KEY_DROPSET, (int)m_AnalysysProp.m_bDropSet);
	WriteInt(INI_SEC_ANALISYS, INI_KEY_DROPINIT, (int)m_AnalysysProp.m_bDropInit);
	WriteInt(INI_SEC_ANALISYS, INI_KEY_PASTESET, (int)m_AnalysysProp.m_bPasteSet);
	WriteInt(INI_SEC_ANALISYS, INI_KEY_PASTEINIT, (int)m_AnalysysProp.m_bPasteInit);

	// 詳細解析設定
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_SEQ, m_CoreAnalysysProp.m_nSeq);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_MAXLINE, m_CoreAnalysysProp.m_nMaxLine);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_HOSTMODE, (int)m_CoreAnalysysProp.m_bHostMode);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_SIGN1, (int)m_CoreAnalysysProp.m_szSign[0]);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_SIGN2, (int)m_CoreAnalysysProp.m_szSign[1]);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_SIGN3, (int)m_CoreAnalysysProp.m_szSign[2]);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_SIGN4, (int)m_CoreAnalysysProp.m_szSign[3]);
	WriteInt(INI_SEC_COREANALISYS, INI_KEY_SIGN5, (int)m_CoreAnalysysProp.m_szSign[4]);
	WriteInt(INI_SEC_COREANALISYS, INI_KET_COMPPOS, m_CoreAnalysysProp.m_nCompPos);
	WriteString(INI_SEC_COREANALISYS, INI_KET_SMDEXT, m_CoreAnalysysProp.m_szSmdExt);
	WriteInt(INI_SEC_COREANALISYS, INI_KET_CODECHECK, (int)m_CoreAnalysysProp.m_bUseCodeCheck);

	// 出力設定
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_PHYLINE, m_ExportProp.un_ExportProp.m_ExportPropState.m_nPhyLine);				// 物理行
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_LOGLINE, m_ExportProp.un_ExportProp.m_ExportPropState.m_nLogLine);				// 論理行
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_LEVEL, m_ExportProp.un_ExportProp.m_ExportPropState.m_nLevel);					// レベル番号
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_DESCRIPT, m_ExportProp.un_ExportProp.m_ExportPropState.m_nDescriptor);			// 識別子
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_TYPEDEF, m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypeDef);				// 変数サイズ定義
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_TYPESIZE, m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypeSize);			// 変数のバッファサイズ
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_TYPEPOS, m_ExportProp.un_ExportProp.m_ExportPropState.m_nTypePos);				// 変数のバッファ位置
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_STRUCTLV, m_ExportProp.un_ExportProp.m_ExportPropState.m_nStructLevel);			// 構造体項目識別用ＩＤ
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_OCCURS, m_ExportProp.un_ExportProp.m_ExportPropState.m_nOccurs);				// 繰り返し項目
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_OCCINDEXED, m_ExportProp.un_ExportProp.m_ExportPropState.m_nOccursIndexed);		// 繰り返し項目の指数定義
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_REDEFINES, m_ExportProp.un_ExportProp.m_ExportPropState.m_nRedefines);			// 再定義項目
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_REDSTRUCT, m_ExportProp.un_ExportProp.m_ExportPropState.m_nRedefinesStruct);	// 再定義項目構造体名
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_COMPRESS, m_ExportProp.un_ExportProp.m_ExportPropState.m_nCompress);			// 32bit変数定義
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_VALUE, m_ExportProp.un_ExportProp.m_ExportPropState.m_nValue);					// 初期値
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_VALUEEX, m_ExportProp.un_ExportProp.m_ExportPropState.m_nValueEx);				// データ拡張
	WriteInt(INI_SEC_EXPORT, INI_KET_EXPORT_COMMENT, m_ExportProp.un_ExportProp.m_ExportPropState.m_nComment);				// コメント

	// パス設定
	WriteString(INI_SEC_PATH, INI_KET_DEFOPENDIR, m_PathProp.m_szDefOpenDir);
	WriteString(INI_SEC_PATH, INI_KET_COBOLOPEN, m_PathProp.m_szCobolOpen);
	WriteString(INI_SEC_PATH, INI_KET_SMDOPEN, m_PathProp.m_szSmdOpen);
	WriteString(INI_SEC_PATH, INI_KET_TMPDIR, m_PathProp.m_szTmpDir);
	WriteInt(INI_SEC_PATH, INI_KET_OPENHISCNT, m_PathProp.m_nHistorySave);
	// 履歴書き出し
	WriteHistory();

	// ＵＩ設定
	WriteInt(INI_SEC_UI, INI_KET_WNDRESTOR, (int)m_UIProp.m_bRestorMainWnd);
	WriteInt(INI_SEC_UI, INI_KET_ITEMIMAGE, (int)m_UIProp.m_bItemImage);
	WriteInt(INI_SEC_UI, INI_KET_WNDL, m_UIProp.m_rcMainWnd.left);
	WriteInt(INI_SEC_UI, INI_KET_WNDT, m_UIProp.m_rcMainWnd.top);
	WriteInt(INI_SEC_UI, INI_KET_WNDR, m_UIProp.m_rcMainWnd.right);
	WriteInt(INI_SEC_UI, INI_KET_WNDB, m_UIProp.m_rcMainWnd.bottom);
	WriteInt(INI_SEC_UI, INI_KET_WNDMAX, (int)m_UIProp.m_bMainWndMaximize);
	WriteInt(INI_SEC_UI, INI_KET_SHOWOUTPUTWND, (int)m_UIProp.m_bShowOutputWnd);
	WriteInt(INI_SEC_UI, INI_KET_OUTPUTWNDSIZE, (int)m_UIProp.m_nOutPutWndSize);
	WriteInt(INI_SEC_UI, INI_KET_FONTSIZE, m_UIProp.m_nFontSize);
	WriteString(INI_SEC_UI, INI_KET_FONTFACE, m_UIProp.m_szFontFace);
	WriteDword(INI_SEC_UI, INI_KET_FORCOL, (DWORD)m_UIProp.m_colFore);
	WriteDword(INI_SEC_UI, INI_KET_BAKCOL, (DWORD)m_UIProp.m_colBack);
	WriteDword(INI_SEC_UI, INI_KET_STRUCTCOL, (DWORD)m_UIProp.m_colStruct);
	WriteDword(INI_SEC_UI, INI_KET_OCCURSCOL, (DWORD)m_UIProp.m_colOccurs);
	WriteDword(INI_SEC_UI, INI_KET_REDEFINESCOL, (DWORD)m_UIProp.m_colRedefines);
	WriteDword(INI_SEC_UI, INI_KET_BINARYCOL, (DWORD)m_UIProp.m_colBinary);
	WriteDword(INI_SEC_UI, INI_KET_SUPPCOL, (DWORD)m_UIProp.m_colSupplement);
	WriteDword(INI_SEC_UI, INI_KET_DATCOL, (DWORD)m_UIProp.m_colDatCol);
	WriteInt(INI_SEC_UI, INI_KET_USEBACKCOLOR, (int)m_UIProp.m_bUseBackColor);
	WriteInt(INI_SEC_UI, INI_KET_SHOWRECSELWND, (int)m_UIProp.m_bShowRecSelWnd);
	WriteInt(INI_SEC_UI, INI_KET_RECSELWNDSIZE, (int)m_UIProp.m_nRecSelWndSize);

	// 検索設定
	WriteDword(INI_SEC_SEARCH, INI_KET_CMDFLG, m_SearchProp.m_dwCmdFlags);
	WriteDword(INI_SEC_SEARCH, INI_KET_CMDFLGEX, m_SearchProp.m_dwCmdExFlags);
	WriteDword(INI_SEC_SEARCH, INI_KET_SRCFLG, m_SearchProp.m_dwSrcFlags);

	// 編集ダイアログ設定
	WriteInt(INI_SEC_EDITDLG, INI_KET_ED_WIDTH, m_EditDialogProp.nWidth);
	WriteInt(INI_SEC_EDITDLG, INI_KET_ED_HEIGHT, m_EditDialogProp.nHeight);
	WriteInt(INI_SEC_EDITDLG, INI_KET_ED_XPOS, m_EditDialogProp.nX);
	WriteInt(INI_SEC_EDITDLG, INI_KET_ED_YPOS, m_EditDialogProp.nY);

	return 0;
}

// 出力設定文字列取得
int CYamaSetup::GetExportStr(const int nIndex, char *pszExportStr, int nSize)
{
	if(nIndex >= MAX_EXPORTPROP) return -1;

	static char szExport[16][23] = {
		"物理行",
		"論理行",
		"レベル番号",
		"識別子",
		"変数サイズ定義",
		"変数のバッファサイズ",
		"変数のバッファ位置",
		"構造体項目識別用ＩＤ",
		"繰り返し項目",
		"繰り返し項目の指数定義",
		"再定義項目",
		"再定義項目構造体名",
		"32bit変数定義",
		"初期値",
		"データ拡張",
		"コメント"
	};

	if((int)strlen(szExport[nIndex]) > nSize) return -1;
//	strcpy(pszExportStr, szExport[nIndex]);
	strcpy_s(pszExportStr, nSize, szExport[nIndex]);

	return (m_ExportProp.un_ExportProp.m_nExportProp[nIndex]);
}

// 履歴ファイル読み込み
int CYamaSetup::ReadHistory()
{
	// 初期化
	EmptyHistory();
	// オープン
	HANDLE hHisFile = CreateFile(m_PathProp.m_szHisFile,
								 GENERIC_READ,
								 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
								 NULL,
								 OPEN_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL,
								 NULL);
	if(hHisFile == INVALID_HANDLE_VALUE) return 0;
	// 内容の読み込んで末尾に追加する
	int nHis = 0;
	while(1){
		LPHISDATA lpHisData = (LPHISDATA)GlobalAlloc(GPTR, (DWORD)sizeof(HISDATA));
		if(!lpHisData) break;
		DWORD dwRead = 0;
		if(!ReadFile(hHisFile, lpHisData, (DWORD)sizeof(HISDATA), &dwRead, NULL)) break;
		if(dwRead < 1) break;
		m_PathProp.m_vecHisData.push_back(lpHisData);
		nHis++;
	}
	CloseHandle(hHisFile);
	return nHis;
}

// 履歴ファイルの書き込み
int CYamaSetup::WriteHistory()
{
	// オープン
	HANDLE hHisFile = CreateFile(m_PathProp.m_szHisFile,
								 GENERIC_READ | GENERIC_WRITE,
								 0,
								 NULL,
								 OPEN_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL,
								 NULL);
	if(hHisFile == INVALID_HANDLE_VALUE) return 0;
	// リスト内容を先頭から書き出し
	int nHis = 0;
	for(std::vector<LPHISDATA>::size_type i = 0; i < m_PathProp.m_vecHisData.size(); i++){
		DWORD dwWrite = 0;
		if(!WriteFile(hHisFile, m_PathProp.m_vecHisData[i], (DWORD)sizeof(HISDATA), &dwWrite, NULL)) break;
		if(dwWrite < 1) break;
		nHis++;
	}
	SetEndOfFile(hHisFile);
	CloseHandle(hHisFile);
	// 初期化
	EmptyHistory();
	return nHis;
}

// 履歴ファイルのクリア
void CYamaSetup::EmptyHistory()
{
	for(std::vector<LPHISDATA>::size_type i = 0; i < m_PathProp.m_vecHisData.size(); i++){
		GlobalFree(m_PathProp.m_vecHisData[i]);
	}
	m_PathProp.m_vecHisData.clear();
}

// 履歴情報の取得
LPHISDATA CYamaSetup::GetHistory(const int nNo)
{
	// 古い順のリストなので逆順に取得する
	int nSize = (int)m_PathProp.m_vecHisData.size() - 1;
	if(nNo > nSize) return NULL;
	return m_PathProp.m_vecHisData[nSize - nNo];
}

// 履歴情報の追加
int CYamaSetup::SetHistory(LPHISDATA lpHisData)
{
	if(!lpHisData) return 0;
	// 同じものがあったら中止
	for(std::vector<LPHISDATA>::size_type i = 0; i < m_PathProp.m_vecHisData.size(); i++){
		if(memcmp(m_PathProp.m_vecHisData[i], lpHisData, sizeof(HISDATA)) == 0) return 0;
	}
	// 古い順のリストなので末尾に追加する
	LPHISDATA lpHisDataTmp = (LPHISDATA)GlobalAlloc(GPTR, (DWORD)sizeof(HISDATA));
	if(!lpHisDataTmp) return 0;
	CopyMemory(lpHisDataTmp, lpHisData, (DWORD)sizeof(HISDATA));
	m_PathProp.m_vecHisData.push_back(lpHisDataTmp);
	return (int)(m_PathProp.m_vecHisData.size());
}

// 履歴情報の削除
void CYamaSetup::RemoveHistory()
{
	if(!ExistFile(m_PathProp.m_szHisFile)) return;
	DeleteFile(m_PathProp.m_szHisFile);
	// ０件で読み込み（メモリクリアとファイル作成のため）
	ReadHistory();
}

// エクスポート設定の取得
int CYamaSetup::GetExportProp()
{
	if(MAX_EXPORTPROP > m_nExportProp){
		for(int i = 0; i < MAX_EXPORTPROP; i++){
			if(m_ExportProp.un_ExportProp.m_nExportProp[i] == m_nExportProp)
				return i;
		}
	}
	return -1;
}

int CYamaSetup::GetFirstExportProp()
{
	m_nExportProp = 0;
	return (GetExportProp());
}

int CYamaSetup::GetNextExportProp()
{
	m_nExportProp++;
	return (GetExportProp());
}

int CYamaSetup::GetExportPropString(const int nExportProp, char *pszExportPropString, int nSize)
{
	const char *szExportPropString[MAX_EXPORTPROP] = {
		"物理行",
		"論理行",
		"レベル番号",
		"識別子",
		"変数サイズ定義",
		"変数のバッファサイズ",
		"変数のバッファ位置",
		"構造体項目識別用ＩＤ",
		"繰り返し項目",
		"繰り返し項目の指数定義",
		"再定義項目",
		"再定義項目構造体名",
		"32bit変数定義",
		"データ",
		"データ拡張",
		"コメント"
	};
	if(MAX_EXPORTPROP > nExportProp){
		if((int)strlen(szExportPropString[nExportProp]) > nSize) return -1;
//		strcpy(pszExportPropString, szExportPropString[nExportProp]);
		strcpy_s(pszExportPropString, nSize, szExportPropString[nExportProp]);
		return (int)(strlen(pszExportPropString));
	}
	return -1;
}
