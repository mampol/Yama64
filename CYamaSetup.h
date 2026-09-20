/*------------------------------------------------------------------------------
	CYamaSetup.h
	設定ファイル操作
------------------------------------------------------------------------------*/

#ifndef _INC_CYAMASETUP_
#define _INC_CYAMASETUP_

#include "CSetup.h"

#ifndef _VECTOR_
#include <vector>
#endif

#define INI_FILE_TITLE				(LPCTSTR)"Yama64.ini"

#define INI_SEC_ANALISYS			(LPCTSTR)"Analisys"
#define INI_KEY_DEFEXPLEVEL			(LPCTSTR)"DefaultExpandLevel"
#define INI_KEY_CBLEXT				(LPCTSTR)"CobolExtension"
#define INI_KEY_BEGINMSG			(LPCTSTR)"BeginMessage"
#define INI_KEY_CMDLINESET			(LPCTSTR)"CommandLineBoot"
#define INI_KEY_DROPSET				(LPCTSTR)"FileDropBoot"
#define INI_KEY_DROPINIT			(LPCTSTR)"FileDropInit"
#define INI_KEY_PASTESET			(LPCTSTR)"PasteBoot"
#define INI_KEY_PASTEINIT			(LPCTSTR)"PasteInit"

#define INI_SEC_COREANALISYS		(LPCTSTR)"AnalisysCore"
#define INI_KEY_SEQ					(LPCTSTR)"SeqLength"
#define INI_KEY_MAXLINE				(LPCTSTR)"LineMaxLength"
#define INI_KEY_HOSTMODE			(LPCTSTR)"MHostSize"
#define INI_KEY_SIGN1				(LPCTSTR)"ErrorSkipSign1"
#define INI_KEY_SIGN2				(LPCTSTR)"ErrorSkipSign2"
#define INI_KEY_SIGN3				(LPCTSTR)"ErrorSkipSign3"
#define INI_KEY_SIGN4				(LPCTSTR)"ErrorSkipSign4"
#define INI_KEY_SIGN5				(LPCTSTR)"ErrorSkipSign5"
#define INI_KET_COMPPOS				(LPCTSTR)"Comp1Byte"
#define INI_KET_SMDEXT				(LPCTSTR)"SmedFileExt"
#define INI_KET_CODECHECK			(LPCTSTR)"UseCodeChecker"

#define MAX_EXPORTPROP				16
#define INI_SEC_EXPORT				(LPCTSTR)"Export"
#define INI_KET_EXPORT_PHYLINE		(LPCTSTR)"PhyLine"			// 物理行
#define INI_KET_EXPORT_LOGLINE		(LPCTSTR)"LogLine"			// 論理行
#define INI_KET_EXPORT_LEVEL		(LPCTSTR)"Level"			// レベル番号
#define INI_KET_EXPORT_DESCRIPT		(LPCTSTR)"Descriptor"		// 識別子
#define INI_KET_EXPORT_TYPEDEF		(LPCTSTR)"TypeDef"			// 変数サイズ定義
#define INI_KET_EXPORT_TYPESIZE		(LPCTSTR)"TypeSize"			// 変数のバッファサイズ
#define INI_KET_EXPORT_TYPEPOS		(LPCTSTR)"TypePos"			// 変数のバッファ位置
#define INI_KET_EXPORT_STRUCTLV		(LPCTSTR)"StructLevel"		// 構造体項目識別用ＩＤ
#define INI_KET_EXPORT_OCCURS		(LPCTSTR)"Occurs"			// 繰り返し項目
#define INI_KET_EXPORT_OCCINDEXED	(LPCTSTR)"OccursIndexed"	// 繰り返し項目の指数定義
#define INI_KET_EXPORT_REDEFINES	(LPCTSTR)"Redefines"		// 再定義項目
#define INI_KET_EXPORT_REDSTRUCT	(LPCTSTR)"RedefinesStruct"	// 再定義項目
#define INI_KET_EXPORT_COMPRESS		(LPCTSTR)"Compress"			// 32bit変数定義
#define INI_KET_EXPORT_VALUE		(LPCTSTR)"Value"			// データ
#define INI_KET_EXPORT_VALUEEX		(LPCTSTR)"ValueEx"			// データ拡張
#define INI_KET_EXPORT_COMMENT		(LPCTSTR)"Comment"			// コメント

#define INI_SEC_PATH				(LPCTSTR)"Path"
#define INI_KET_DEFOPENDIR			(LPCTSTR)"DefaultOpenDirectory"
#define INI_KET_COBOLOPEN			(LPCTSTR)"CobolOpenApplication"
#define INI_KET_SMDOPEN				(LPCTSTR)"FormOpenApplication"
#define INI_KET_TMPDIR				(LPCTSTR)"TempDirectory"
#define INI_KET_OPENHISCNT			(LPCTSTR)"HistorySave"
#define HIS_FILE_TITLE				(LPCTSTR)"Yama64.his"

#define INI_SEC_UI					(LPCTSTR)"UI"
#define INI_KET_WNDRESTOR			(LPCTSTR)"RestorWindowRect"
#define INI_KET_ITEMIMAGE			(LPCTSTR)"DrawListItemImage"
#define INI_KET_WNDL				(LPCTSTR)"WindowLeft"
#define INI_KET_WNDT				(LPCTSTR)"WindowTop"
#define INI_KET_WNDR				(LPCTSTR)"WindowRight"
#define INI_KET_WNDB				(LPCTSTR)"WindowBottom"
#define INI_KET_WNDMAX				(LPCTSTR)"WindowMaximize"
#define INI_KET_SHOWOUTPUTWND		(LPCTSTR)"ShowOutputWindow"
#define INI_KET_OUTPUTWNDSIZE		(LPCTSTR)"OutputWindowSize"
#define INI_KET_FONTSIZE			(LPCTSTR)"FontSize"
#define INI_KET_FONTFACE			(LPCTSTR)"FontFace"
#define INI_KET_FORCOL				(LPCTSTR)"ForeColor"
#define INI_KET_BAKCOL				(LPCTSTR)"BackColor"
#define INI_KET_STRUCTCOL			(LPCTSTR)"StructColor"
#define INI_KET_OCCURSCOL			(LPCTSTR)"OccursColor"
#define INI_KET_REDEFINESCOL		(LPCTSTR)"RedefinesColor"
#define INI_KET_BINARYCOL			(LPCTSTR)"BinaryColor"
#define INI_KET_SUPPCOL				(LPCTSTR)"SupplementationColor"
#define INI_KET_DATCOL				(LPCTSTR)"DatacolumnColor"
#define INI_KET_USEBACKCOLOR		(LPCTSTR)"UseBackGroundColor"
#define INI_KET_SHOWRECSELWND		(LPCTSTR)"ShowRecSelWindow"
#define INI_KET_RECSELWNDSIZE		(LPCTSTR)"RecSelWindowSize"

#define INI_SEC_SEARCH				(LPCTSTR)"Search"
#define INI_KET_CMDFLG				(LPCTSTR)"CommandFlag"
#define INI_KET_CMDFLGEX			(LPCTSTR)"CommandFlagEx"
#define INI_KET_SRCFLG				(LPCTSTR)"SourceFlag"

#define INI_SEC_EDITDLG				(LPCTSTR)"EditDialog"
#define INI_KET_ED_WIDTH			(LPCTSTR)"Width"
#define INI_KET_ED_HEIGHT			(LPCTSTR)"Height"
#define INI_KET_ED_XPOS				(LPCTSTR)"XPos"
#define INI_KET_ED_YPOS				(LPCTSTR)"YPos"

#define UI_COL_FOR					(DWORD)GetSysColor(COLOR_WINDOWTEXT)
#define UI_COL_BAK					(DWORD)GetSysColor(COLOR_WINDOW)
#define UI_COL_STRUCT				(DWORD)RGB(0, 0, 255)
#define UI_COL_OCCURS				(DWORD)RGB(0, 80, 0)
#define UI_COL_REDEFINES			(DWORD)RGB(80, 0, 0)
#define UI_COL_BINARY				(DWORD)RGB(80, 0, 80)
#define UI_COL_SUPP					(DWORD)RGB(80, 80, 80)
#define UI_COL_DATCOL				(DWORD)RGB(255, 245, 255)

#define MAX_HISCOUNT				100
typedef struct _HISDATA{
	int nSource;
	char szSource[MAX_HISCOUNT][MAX_PATH];
} HISDATA, *LPHISDATA;

enum {
	EXPORT_PHYLINE = 0,	// 物理行
	EXPORT_LOGLINE,		// 論理行
	EXPORT_LEVEL,		// レベル番号
	EXPORT_DESC,		// 識別子
	EXPORT_TYPE,		// 変数サイズ定義
	EXPORT_SIZE,		// 変数のバッファサイズ
	EXPORT_POS,			// 変数のバッファ位置
	EXPORT_STRUCTLV,	// 構造体項目識別用ＩＤ
	EXPORT_OCCURS,		// 繰り返し項目
	EXPORT_OCCINDEXED,	// 繰り返し項目の指数定義
	EXPORT_REDEFINES,	// 再定義項目
	EXPORT_REDSTRUCT,	// 再定義項目
	EXPORT_COMPRESS,	// 32bit変数定義
	EXPORT_VALUE,		// データ
	EXPORT_VALUEEX,		// データ拡張
	EXPORT_COMMENT		// コメント
};

class CYamaSetup : public CSetup
{
private:
	int m_nExportProp;

private:
	int GetExportProp();

public:
	// 解析設定
	struct AnalysysProp
	{
		unsigned int m_nDefExpandLevel;		// 初期展開レベル
		char m_szCblExt[256];				// CBOBL拡張子
		BOOL m_bNoExpandRed;				// 再定義項目展開梨
		BOOL m_bNoExpandOcc;				// 繰返し項目展開梨
		BOOL m_bBeginMsg;					// 解析開始前にメッセージ
		BOOL m_bCmdLineSet;					// コマンドラインで即解析
		BOOL m_bDropSet;					// ドロップで即解析
		BOOL m_bDropInit;					// ドロップで初期化解析
		BOOL m_bPasteSet;					// 貼り付けで即解析
		BOOL m_bPasteInit;					// 貼り付けで初期化解析
	} m_AnalysysProp;
	// 詳細解析設定
	struct CoreAnalysysProp
	{
		int m_nSeq;							// ソース行番号桁数
		int m_nMaxLine;						// ソース１行最大文字数
		BOOL m_bHostMode;					// ホスト(COBOL85)計算
		char m_szSign[5];					// エラーとしない記号
		int m_nCompPos;						// 計算開始バイト数
		char m_szSmdExt[10];				// FORM拡張子
		BOOL m_bUseCodeCheck;				// コードチェック
	} m_CoreAnalysysProp;
	// 出力設定
	struct ExportProp
	{
		union Union_ExportProp
		{
			int m_nExportProp[MAX_EXPORTPROP];
			struct ExportPropState{
				int m_nPhyLine;					// 物理行
				int m_nLogLine;					// 論理行
				int m_nLevel;					// レベル番号
				int m_nDescriptor;				// 識別子
				int m_nTypeDef;					// 変数サイズ定義
				int m_nTypeSize;				// 変数のバッファサイズ
				int m_nTypePos;					// 変数のバッファ位置
				int m_nStructLevel;				// 構造体項目識別用ＩＤ
				int m_nOccurs;					// 繰り返し項目
				int m_nOccursIndexed;			// 繰り返し項目の指数定義
				int m_nRedefines;				// 再定義項目
				int m_nRedefinesStruct;			// 再定義項目構造体名
				int m_nCompress;				// 32bit変数定義
				int m_nValue;					// 初期値
				int m_nValueEx;					// データ拡張
				int m_nComment;					// コメント
			} m_ExportPropState;
		} un_ExportProp;
	} m_ExportProp;
	// パス設定
	struct PathProp
	{
		char m_szDefOpenDir[MAX_PATH+1];		// 初期表示ディレクトリ
		char m_szCobolOpen[MAX_PATH+1];			// COBOLソースを開くアプリ
		char m_szSmdOpen[MAX_PATH+1];			// FORMソースを開くアプリ
		char m_szTmpDir[MAX_PATH+1];			// テンポラリディレクトリ
//		BOOL m_bEnableTmpPath;					// テンポラリの再作成可否(記録はしない)
		char m_szHisFile[MAX_PATH+1];			// 解析履歴ファイル
		int m_nHistorySave;						// 解析履歴最大保持数
		std::vector<LPHISDATA> m_vecHisData;	// 履歴のリスト
	} m_PathProp;
	// ＵＩ設定
	struct UIProp
	{
		BOOL m_bRestorMainWnd;					// ウィンドウ復元の有無
		BOOL m_bItemImage;						// リストアイテムへのイメージ描画
		RECT m_rcMainWnd;						// ウィンドウ位置
		BOOL m_bMainWndMaximize;				// ウィンドウ最大化
		BOOL m_bShowOutputWnd;					// アウトプットウィンドウ可視の有無
		int m_nOutPutWndSize;					// アウトプットウィンドウサイズ
		int m_nFontSize;						// フォントサイズ
		char m_szFontFace[LF_FACESIZE+1];		// フォントフェイス
		COLORREF m_colFore, m_colBack;			// 色情報
		COLORREF m_colStruct, m_colOccurs,
				 m_colRedefines, m_colBinary,
				 m_colSupplement, m_colDatCol;
		BOOL m_bUseBackColor;					// 背景色として使う
		BOOL m_bShowRecSelWnd;					// レコード選択ウィンドウ可視の有無
		int m_nRecSelWndSize;					// レコード選択ウィンドウサイズ
	} m_UIProp;
	// 検索設定
	struct SearchProp
	{
		DWORD m_dwCmdFlags;
		DWORD m_dwCmdExFlags;
		DWORD m_dwSrcFlags;
	} m_SearchProp;
	// 編集ダイアログ設定
	struct EditDialogProp
	{
		int nWidth;
		int nHeight;
		int nX;
		int nY;
	} m_EditDialogProp;
	
public:
	CYamaSetup();
	virtual ~CYamaSetup();

	int ReadSetup();
	int WriteSetup();
	int GetExportStr(const int nIndex, char *pszExportStr, int nSize);

	int ReadHistory();
	int WriteHistory();
	void EmptyHistory();
	LPHISDATA GetHistory(const int nNo);
	int SetHistory(const LPHISDATA lpHisData);
	void RemoveHistory();

	int GetFirstExportProp();
	int GetNextExportProp();
	int GetExportPropString(const int nExportProp, char *pszExportPropString, int nSize);
};

#endif	// _INC_CYAMASETUP_
