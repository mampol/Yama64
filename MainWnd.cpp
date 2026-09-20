//#pragma comment(lib, "HpAuth.lib")
/*----------------------------------------------------------------------------------------

	MainWnd.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commoncontrols.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
ULONG_PTR g_GdiplusToken = 0;
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "CWinTheme.h"
#include "MainWnd.h"
#include "OutputWnd.h"
//#include "RecSelWnd.h"
#include "SearchDlg.h"
#include "SetupPage.h"
#include "EditDlg.h"
#include "Export.h"
#include "Property.h"
#include "ConfigDlg.h"
#include "CDataFile.h"
//#include "LicenceFunc.h"
//#include "HpAuth.h"
#include "resource.h"

#pragma comment(linker, \
    "\"/manifestdependency:type='win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

enum DROPFILE_TYPE
{
	DROPFILE_UNKNOWN = 0,
	DROPFILE_COBOL,
	DROPFILE_DATA
};
#define DROP_DATA_SIZE_THRESHOLD	(10ULL * 1024 * 1024)

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static char gszTmpDir[MAX_PATH];
// 解析スレッドハンドルとか
static HANDLE ghAnaCBLThread = NULL;
static DWORD gdwAnaCBLThreadID = 0;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT PreCreateWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
BOOL InitGdiplus();
void UninitGdiplus();
BOOL MakeTempDir(HWND hWnd, const char *pszTempDir);
int RemoveTempDir(const char *pszTempDir);
BOOL CALLBACK EnumYamamotoProc(HWND hWnd, LPARAM lp);
int GetCommandLineFile(HWND hWnd);
int CreateHistroyMenu(HWND hWnd);
void ShowBootMessage(HWND hWndMain);
int SetControlState(HWND hWnd, const int nState);
int OnCreateWindow(HWND hWnd, LPSTR lpsCmdLine, const int nCmdShow);
LRESULT OnActivateWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnSize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnSizePain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
//void ResizeAnaLst(HWND hWnd, CDlgbar *lpCDlgbar, CProgStbar *lpCStatus, CWndSplitter *lpCWndSplit, CWndSplitter *lpCWndSplit2, CAnalysisList *lpCAnaLst);
void ResizeAnaLst(HWND hWnd, CDlgbar *lpCDlgbar, CProgStbar *lpCStatus, CWndSplitter *lpCWndSplit, CAnalysisList *lpCAnaLst);
LRESULT OnDrawItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnCommand(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT NewAppication(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT NewSource(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OpenSource(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CloseSource(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CloseAllSources(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int ReloadSourceFromNo(HWND hWnd, const int nNo);
LRESULT ReloadAllSources(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDropFiles(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
DROPFILE_TYPE GetDropFileType(CYamaSetup* lpCSetup, const char* pszFilePath);
BOOL IsCobolExtension(const char* pszExtensions, const char* pszExt);
DROPFILE_TYPE JudgeFileContent(const char* pszFilePath);
const char* StrStrIEx(const char* pszText, const char* pszFind);
int AddSourceFile(HWND hWndMain, const char *pszSource, BOOL bUpdate = TRUE);
int AddCobolSourceFile(HWND hWnd, const char *pszSource);
int AddSmdSourceFile(HWND hWnd, const char *pszSource);
BOOL DelAddSmdSourceFile(HWND hWndMain, const int nSrcNo);
void MoveSource(HWND hWnd, const BOOL bUp);
BOOL OnOpenListSrc(HWND hWnd);
int OnEditDlg(HWND hWndMain, const int nSrcNo = -1);
int OnPaste(HWND hWndMain);
int OnCopy(HWND hWndMain);
int OpenSourcePath(HWND hWndMain, const int nSrcNo);
BOOL StartProcess(HWND hWndMain, const char *pszApplication, const char *pszParam, LPDWORD lpdwExitCode);
void SearchExpandSel(HWND hWnd);
void SearchClearAll(HWND hWnd);
LRESULT OnNotify(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnMainToolbarNotify(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnListViewNotify(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnClickListItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDblClickListItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnContextMenu(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnContextBtn(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int TrackListContextMenu(HWND hMainWnd, LPPOINT lppt);
LRESULT OnCancelJob(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnOutputTabView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
//LRESULT OnRecSelView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnThemeChanged(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnAnaCBL(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
DWORD WINAPI ThreadAnaCBL(LPVOID lpParameter);
LRESULT OnAnalising(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
BOOL OnExportAnalysys(HWND hWndMain, WORD wpCmd);
LRESULT OnDeleteHistoryAnalysys(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
BOOL OnHistoryAnalysys(HWND hWndMain, UINT uiID);
LRESULT OnShowSetupPage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnShowConfigBox(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnVersionInfo(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
//LRESULT OnOpenReadMe(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnClose(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
//static int  CheckLicense();
//static void AboutLicense(HWND hWnd, UINT uiCmd);
//static void ShowHelp(HWND hWnd);
LRESULT OpenDataFileDlg(HWND hWnd);
LRESULT OpenDataFile(HWND hWnd, LPCTSTR lpszFilePath);
LRESULT CloseDataFile(HWND hWnd);
LRESULT CALLBACK JudgeDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
HICON GetFileIcon48(const char* pszFilePath);
LRESULT OnPaintJugeDlg(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDrawJugeBtn(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

/*----------------------------------------------------------------------------------------
	WinMain
----------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	if(hPrevInst != NULL){
		ERRMSG(HWND_DESKTOP, "このｱﾌﾟﾘｹｰｼｮﾝは32bit環境でのみ動作します");
		return 0;
	}

	InitGdiplus();

	// ウィンドウクラス登録
	WNDCLASSEX wcex;
	HWND hWnd;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hInstance = hCurInst;
	wcex.hIcon = (HICON)LoadImage(hCurInst,
		MAKEINTRESOURCE(IDI_ICON01),
		IMAGE_ICON,
		32, 32,
		LR_DEFAULTCOLOR | LR_SHARED);
	wcex.hCursor = (HCURSOR)LoadImage(NULL,
		MAKEINTRESOURCE(IDC_ARROW),
		IMAGE_CURSOR,
		0, 0,
		LR_DEFAULTCOLOR | LR_SHARED);
	wcex.hbrBackground = (HBRUSH)(/*COLOR_WINDOW*/COLOR_BTNFACE+1);
	wcex.lpszClassName = APP_CLASS;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	wcex.hIconSm = (HICON)LoadImage(hCurInst,
		MAKEINTRESOURCE(IDI_ICON01),
		IMAGE_ICON,
		16, 16,
		LR_DEFAULTCOLOR | LR_SHARED);
	if(!RegisterClassEx(&wcex)){
		ERRMSG(HWND_DESKTOP, "ｳｨﾝﾄﾞｳｸﾗｽの登録に失敗");
		return 0;
	}

	// メインウィンドウ作成
	hWnd = CreateWindowEx(WS_EX_CONTROLPARENT | WS_EX_ACCEPTFILES,
				APP_CLASS,
				APP_NAME,
				WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				NULL,
				NULL,
				hCurInst,
				NULL);
	if(!hWnd){
		ERRMSG(HWND_DESKTOP, "ｳｨﾝﾄﾞｳ作成に失敗");
		return 0;
	}

	// COM初期化
	if(!InitCom()) ERRMSG(HWND_DESKTOP, "COMの初期化に失敗");

	// ｱｸｾﾗﾚｰﾀﾛｰﾄﾞ
	HACCEL hAccel = LoadAccelerators(hCurInst, MAKEINTRESOURCE(IDR_ACCEL_MAIN));
	if (!hAccel) {
		ERRMSG(HWND_DESKTOP, "ｱｸｾﾗﾚｰﾀﾃｰﾌﾞﾙのﾛｰﾄﾞに失敗");
		return 0;
	}

	// ﾒｲﾝ作成後処理
	if(OnCreateWindow(hWnd, lpsCmdLine, nCmdShow) < 0) SendMessage(hWnd, WM_CLOSE, 0L, 0L);

	// メインメッセージループ
	MSG msg;
	int nRet;
	while((nRet = GetMessage(&msg, NULL, 0, 0))){
		if(nRet == -1){
			ERRMSG(HWND_DESKTOP, "ﾒｲﾝﾒｯｾｰｼﾞﾙｰﾌﾟでｴﾗｰが発生");
			break;
		}
		if(!TranslateAccelerator(hWnd, hAccel, &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DestroyAcceleratorTable(hAccel);

	UninitGdiplus();

	// COM解放
	UninitCom();

	return (int)(msg.wParam);
}

/*----------------------------------------------------------------------------------------
	メインウィンドウプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_CREATE:
		return (PreCreateWindow(hWnd, msg, wp, lp));
	case WM_ACTIVATE:
		return (OnActivateWindow(hWnd, msg, wp, lp));
	case WM_SIZE:
		return (OnSize(hWnd, msg, wp, lp));
	case WM_RESTORED_PAIN:
		return (OnSizePain(hWnd, msg, wp, lp));
	case WM_DRAWITEM:
		return (OnDrawItem(hWnd, msg, wp, lp));
	case WM_COMMAND:
		return (OnCommand(hWnd, msg, wp, lp));
	case WM_NOTIFY:
		return (OnNotify(hWnd, msg, wp, lp));
	case WM_CONTEXTMENU:
		return (OnContextMenu(hWnd, msg, wp, lp));
	case WM_DROPFILES:
		return (OnDropFiles(hWnd, msg, wp, lp));
	case UM_BEGINANACBL:
	case UM_FINISHANACBL:
		return (OnAnalising(hWnd, msg, wp, lp));
	case WM_THEMECHANGED:
		return (OnThemeChanged(hWnd, msg, wp, lp));
	case WM_CLOSE:
		return (OnClose(hWnd, msg, wp, lp));
	case WM_DESTROY:
		return (OnDestroy(hWnd, msg, wp, lp));
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ウィンドウ作成前プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT PreCreateWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	InitWin32ComCtrl(ICC_WIN95_CLASSES | ICC_COOL_CLASSES);

	HINSTANCE hCurInst = ((LPCREATESTRUCT)lp)->hInstance;
	CWinTheme *lpCWinTheme = NULL;
	CYamaSetup *lpCSetup = NULL;
	CDlgbar *lpCDlgbar = NULL;
	CProgStbar *lpCStatus = NULL;
	CAnalysisList *lpCAnaLst = NULL;
	CWndSplitter *lpCWndSplit = NULL;//, *lpCWndSplit2;
	CAnaCBL *lpCAnaCBL = NULL;
	try{
//		// ライセンス確認
//		CheckLicense();
		// テーマ確認クラス
		lpCWinTheme = (CWinTheme *)new CWinTheme();
		SetProp(hWnd, CWINTHEME_CLASS, (HANDLE)lpCWinTheme);
		// 設定ファイル操作クラスインスタンス作成
		lpCSetup = (CYamaSetup *)new CYamaSetup();
		if(!lpCSetup) throw "設定ファイル操作クラスインスタンス作成失敗";
		lpCSetup->SetIniFileTitle(INI_FILE_TITLE);
		lpCSetup->ReadSetup();
		SetProp(hWnd, CSETUP_CLASS, (HANDLE)lpCSetup);
		// ダイアログバー制御クラスインスタンス作成
		RECT rcDlgbar;
		TBBUTTON tbbtn_Ana[] = {
			{0, IDM_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
			{1, IDM_OPENSRC, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN, 0, 0},
			{2, IDM_ANACBL, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN, 0, 0},
			{3, IDM_OPENDAT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
			{4, IDM_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0}
		};
		const char *pszTipText_Ana[] = {
			"解析結果を消去し初期状態にします",
			"COBOLソースファイルを開きます",
			"ソース一覧に設定されているCOBOLソースを連結して解析します",
			"データファイルを開きます",
			"解析結果から変数名を検索します"
		};
		int nSeps_Ana[] = { 1, 3, 5, 7 };
		lpCDlgbar = (CDlgbar *)new CDlgbar(hCurInst, hWnd);
		if(!lpCDlgbar) return -1;
		lpCDlgbar->MakeRebar(ID_REBAR);
		// 解析用ツールバー
		HWND hToolbar_Ana = lpCDlgbar->AddToolbar("",
			FALSE, FALSE,
			ID_TOOLBAR_ANA,
			TBSTYLE_FLAT | CCS_NORESIZE | CCS_NODIVIDER);
//		lpCDlgbar->AddTbImageList(hToolbar_Ana, 32, IDB_BITMAP1, IDB_BITMAP2);
//		lpCDlgbar->AddTbDisableImageList(hToolbar_Ana, 32, IDB_BITMAP1D, IDB_BITMAP2);
		lpCDlgbar->AddTbPngImageList(hToolbar_Ana, 48, IDR_PNG_TOOLBAR);
		lpCDlgbar->AddTbPngDisableImageList(hToolbar_Ana, 48, IDR_PNG_TOOLBAR_D);
		lpCDlgbar->AddTbButton(hToolbar_Ana, tbbtn_Ana, (const int)(sizeof(tbbtn_Ana) / sizeof(TBBUTTON)));
		lpCDlgbar->InsTbToolTips(hToolbar_Ana, pszTipText_Ana);
		lpCDlgbar->InsTbSeps(hToolbar_Ana, nSeps_Ana, (const int)(sizeof(nSeps_Ana) / sizeof(int)));
		lpCDlgbar->SetTbIndent(hToolbar_Ana, 4);
		lpCDlgbar->ReflashBandSize(0, 14);
		SetProp(hWnd, CDLGBAR_CLASS, (HANDLE)lpCDlgbar);
		// ステータスバー制御クラスインスタンス作成
		RECT rcStatus;
		int nParts[] = { 40, 20, 20, 20 };
		lpCStatus = (CProgStbar *)new CProgStbar(hCurInst,
			hWnd,
			ID_STATUSBAR, ID_PROGBAR,
			"",
			nParts, 4,
			3);
		if(!lpCStatus) throw "ステータスバー制御クラスインスタンス作成失敗";
		lpCStatus->CreateBar();
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");
		lpCStatus->GetRect(&rcStatus);
		SetProp(hWnd, CPROGSTBAR_CLASS, (HANDLE)lpCStatus);
		// 可変ウィンドウ制御クラスインスタンス作成
		lpCDlgbar->GetRect(&rcDlgbar);
		lpCWndSplit = (CWndSplitter *)new CWndSplitter();
		if(!lpCWndSplit) throw "可変ウィンドウ制御クラスインスタンス作成失敗";
		lpCWndSplit->Initialize(hCurInst,
			hWnd,
			ID_OUTPUTWND,
			SSPLITTER_BOTTOM);
		if(lpCWndSplit->Create(CSPLITWND_CLASS,
			OutputSplitWndProc,
			lpCSetup->m_UIProp.m_nOutPutWndSize,
			0, rcDlgbar.bottom,
			0, rcStatus.bottom + GetSystemMetrics(SM_CYEDGE),
			(LPVOID)hWnd)) lpCWndSplit->Show();
		SetProp(hWnd, CSPLITWND_CLASS, (HANDLE)lpCWndSplit);
//		// 第二ペイン
//		lpCWndSplit2 = (CWndSplitter *)new CWndSplitter();
//		if(!lpCWndSplit2) throw "可変ウィンドウ制御クラス２インスタンス作成失敗";
//		lpCWndSplit2->Initialize(hCurInst,
//			hWnd,
//			ID_RECSELWND,
//			SSPLITTER_RIGHT);
//		if(lpCWndSplit2->Create(CSPLITWND_CLASS2,
//			RecSelSplitWndProc,
//			lpCSetup->m_UIProp.m_nRecSelWndSize,
//			0, rcDlgbar.bottom,
//			0, rcStatus.bottom + GetSystemMetrics(SM_CYEDGE),
//			(LPVOID)hWnd)) lpCWndSplit2->Hide();
//		SetProp(hWnd, CSPLITWND_CLASS2, (HANDLE)lpCWndSplit2);
		// 履歴メニューの作成
		CreateHistroyMenu(hWnd);
		// メニューをチェック
		HMENU hViewMenu = GetMenuHandle(hWnd, MENU_POS_VIEW);
		if(hViewMenu){
			MENUITEMINFO mif;
			ZeroMemory(&mif, sizeof(MENUITEMINFO));
			mif.cbSize = sizeof(MENUITEMINFO);
			mif.fMask = MIIM_STATE;
			mif.fState = MFS_CHECKED;
			SetMenuItemInfo(hViewMenu, IDM_OUTPUT_VIEW, FALSE, &mif);
			CheckMenuRadioItem(hViewMenu,
				IDM_TRACE_VIEW, IDM_SEARCH_VIEW,
				(UINT)GetTabMenuId(0), MF_BYCOMMAND);
		}
		// ＣＯＢＯＬ解析クラスインスタンス作成
		lpCAnaCBL = (CAnaCBL *)new CAnaCBL();
		if(!lpCAnaCBL) throw "ＣＯＢＯＬ解析クラスインスタンス作成失敗";
		lpCAnaCBL->Initialize(hWnd);
		SetProp(hWnd, CANACBL_CLASS, (HANDLE)lpCAnaCBL);
		// データ読み込みクラスインスタンス
		CDataFile* lpCDataFile = NULL;
		lpCDataFile = new CDataFile();
		if (!lpCDataFile) {
			throw "データファイルクラスインスタンス作成失敗";
		}
		SetProp(hWnd, CDATAFILE_CLASS, (HANDLE)lpCDataFile);
		// 解析結果リスト制御クラスインスタンス作成
		lpCAnaLst = (CAnalysisList *)new CAnalysisList();
		if(!lpCAnaLst) throw "解析結果リスト制御クラスインスタンス作成失敗";
		lpCAnaLst->SetDataFileClass(lpCDataFile);
		if(lpCAnaLst->Create(hWnd, IDB_BITMAP11)){
			lpCAnaLst->SetColorFore(lpCSetup->m_UIProp.m_colFore);
			lpCAnaLst->SetColorBack(lpCSetup->m_UIProp.m_colBack);
			lpCAnaLst->SetColorStruct(lpCSetup->m_UIProp.m_colStruct);
			lpCAnaLst->SetColorOccurs(lpCSetup->m_UIProp.m_colOccurs);
			lpCAnaLst->SetColorRedefines(lpCSetup->m_UIProp.m_colRedefines);
			lpCAnaLst->SetColorBinary(lpCSetup->m_UIProp.m_colBinary);
			lpCAnaLst->SetColorSupplementation(lpCSetup->m_UIProp.m_colSupplement);
			lpCAnaLst->SetColorDataColumn(lpCSetup->m_UIProp.m_colDatCol);
			lpCAnaLst->SetColorUse(lpCSetup->m_UIProp.m_bUseBackColor);
			lpCAnaLst->SetItemImageUse(lpCSetup->m_UIProp.m_bItemImage);
			lpCAnaLst->Show();
			lpCAnaLst->Update();
			lpCAnaLst->SetAnalstFont(lpCSetup->m_UIProp.m_nFontSize,
									 lpCSetup->m_UIProp.m_szFontFace);
		}
		// プログレスクラス設定
		lpCAnaLst->SetProgStbarClass(lpCStatus);
		SetProp(hWnd, CANALST_CLASS, (HANDLE)lpCAnaLst);
		// テンポラリディレクトリの作成
		if(!MakeTempDir(hWnd, lpCSetup->m_PathProp.m_szTmpDir)) throw "テンポラリディレクトリの作成に失敗";
		// コントロールの状態
		SetControlState(hWnd, STATE_NEW);
		// ファイルのドロップ受付開始
		DragAcceptFiles(hWnd, TRUE);
		// 起動時メッセージ
		ShowBootMessage(hWnd);
	}
	catch(const char *pszMsg){
		ERRMSG(hWnd, pszMsg);
		if(lpCStatus) delete lpCStatus;
		if(lpCWndSplit) delete lpCWndSplit;
//		if(lpCWndSplit2) delete lpCWndSplit2;
		if(lpCAnaCBL) delete lpCAnaCBL;
		return -1;
	}
	return (0L);
}

BOOL InitGdiplus()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	Gdiplus::Status status =
		Gdiplus::GdiplusStartup(
			&g_GdiplusToken,
			&gdiplusStartupInput,
			NULL);

	return (status == Gdiplus::Ok);
}

void UninitGdiplus()
{
	if (g_GdiplusToken != 0) {

		Gdiplus::GdiplusShutdown(
			g_GdiplusToken);

		g_GdiplusToken = 0;
	}
}

/*----------------------------------------------------------------------------------------
	テンポラリディレクトリの作成
----------------------------------------------------------------------------------------*/
BOOL MakeTempDir(HWND hWnd, const char *pszTempDir)
{
	// パス
	char szTempDir[MAX_PATH + 1];
//	strcpy(szTempDir, pszTempDir);
	strcpy_s(szTempDir, sizeof(szTempDir), pszTempDir);
	if(strlen(szTempDir) < 1){
//		if(!lpCSetup->GetSpecialDir(szTempDir, CSIDL_APPDATA)) return FALSE;
		GetTempPath((DWORD)sizeof(szTempDir), szTempDir);
	}
	// 作成
	int nYamamoto = 0;
	int *pnYamamoto = &nYamamoto;
	EnumWindows((WNDENUMPROC)EnumYamamotoProc, (LPARAM)&nYamamoto);
	if(szTempDir[strlen(szTempDir) - 1] == '\\'){
		wsprintf(gszTmpDir, "%s%s.%d", szTempDir, TMP_DIR_TITLE, nYamamoto);
	}else{
		wsprintf(gszTmpDir, "%s\\%s.%d", szTempDir, TMP_DIR_TITLE, nYamamoto);
	}
	// 作成の正否は問わず...
	if(!ExistFile(gszTmpDir)) CreateDirectory(gszTmpDir, NULL);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	テンポラリディレクトリの削除
----------------------------------------------------------------------------------------*/
int RemoveTempDir(const char *pszTempDir)
{
	int nCount = 0;
	char szTempDir[MAX_PATH], szFindString[MAX_PATH];
//	strcpy(szTempDir, pszTempDir);
	strcpy_s(szTempDir, sizeof(szTempDir), pszTempDir);
	if(szTempDir[strlen(szTempDir) - 1] == '\\') szTempDir[strlen(szTempDir) - 1] = '\0';
	wsprintf(szFindString, "%s\\*", szTempDir);

	WIN32_FIND_DATA w32fd;
	ZeroMemory(&w32fd, sizeof(WIN32_FIND_DATA));
	HANDLE hFind = FindFirstFile(szFindString, &w32fd);
	if(hFind == INVALID_HANDLE_VALUE) return nCount;
	do {
		if(strcmp(w32fd.cFileName, ".") == 0) continue;
		if(strcmp(w32fd.cFileName, "..") == 0) continue;

		char szFile[MAX_PATH];
		wsprintf(szFile, "%s\\%s", szTempDir, w32fd.cFileName);
		if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			nCount += RemoveTempDir(szFile);
		}else{
			if(!DeleteFile(szFile)) ERRMSG(HWND_DESKTOP, "テンポラリファイルの削除に失敗しました");
			nCount++;
		}
	} while(FindNextFile(hFind, &w32fd));
	FindClose(hFind);

	if(!RemoveDirectory(szTempDir)) ERRMSG(HWND_DESKTOP, "テンポラリディレクトリの削除に失敗しました");

	return nCount;
}

/*----------------------------------------------------------------------------------------
	起動済みの自分を数える
----------------------------------------------------------------------------------------*/
BOOL CALLBACK EnumYamamotoProc(HWND hWnd, LPARAM lp)
{
	int *pnYamamoto = (int *)lp;
	char szClassName[MAX_PATH];
	if(GetClassName(hWnd, szClassName, sizeof(szClassName)) > 0){
		if(strcmp(szClassName, APP_CLASS) == 0) (*pnYamamoto)++;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	コマンドラインの取得
----------------------------------------------------------------------------------------*/
int GetCommandLineFile(HWND hWnd)
{
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return -1;
	// コマンドライン
	char *pszCmdLine = GetCommandLine();
	if(!pszCmdLine) return 0;
	char szFilePath[MAX_PATH + 1];
	int nCount = 0, i, nLen = (sizeof(szFilePath) - 1);
	for(char *p = pszCmdLine; (nCount - 1) < MAX_OPENFILE && *p != '\0'; p++){
		szFilePath[0] = '\0';
		if(*p == '\"') {
			for(i = 0, p++; i < nLen && *p != '\0';p++){
				if(*p == '\"') break;
				szFilePath[i] = *p;
				i++;
			}
		}else{
			for(i = 0; i < nLen && *p != '\0'; p++){
				if(*p == ' ') break;
				szFilePath[i] = *p;
				i++;
			}
		}
		szFilePath[i] = '\0';
		if(strlen(szFilePath) < 1) break;
		// 最初は自分のパス
		if(nCount > 0){
			// 存在確認
			if(lpCSetup->ExistFile(szFilePath)){
				// ファイル追加
				if(AddSourceFile(hWnd, szFilePath) < 0) break;
			}else{
				char szTraceMsg[MAX_PATH + 64];
				wsprintf(szTraceMsg, "[%s] は見つかりませんでした...", szFilePath);
				AddOutputMessage(hWnd, szTraceMsg, TRACE_ANA);
			}
		}
		nCount++;
		// 終了確認
		if(*p == '\0') break;
		// ｺﾏﾝﾄﾞﾗｲﾝの区切りの0x20を飛ばす
		p++;
	}
	return (nCount - 1);
}

/*----------------------------------------------------------------------------------------
	解析履歴サブメニューの作成
----------------------------------------------------------------------------------------*/
int CreateHistroyMenu(HWND hWnd)
{
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return NULL;

	HMENU hMenu = GetMenu(hWnd);
	HMENU hFileMenu = GetSubMenu(hMenu, MENU_POS_FILE);
	HMENU hHisMenu = GetSubMenu(hFileMenu, MENU_FILE_POS_HIS);
	if(!hHisMenu) return NULL;

	// 一度全部削除
	int nCount = GetMenuItemCount(hHisMenu);
	for(int i = 0; i < nCount; i++) DeleteMenu(hHisMenu, 0, MF_BYPOSITION);

	// 再構築
	MENUITEMINFO mi;
	ZeroMemory(&mi, sizeof(MENUITEMINFO));
	mi.cbSize = sizeof (MENUITEMINFO);
	mi.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
	mi.fType = MFT_STRING;
	int nMenuItems = 0;
	for(nMenuItems = 0; nMenuItems < lpCSetup->m_PathProp.m_nHistorySave; nMenuItems++){
		LPHISDATA lpHisData = lpCSetup->GetHistory(nMenuItems);
		if(lpHisData == NULL) break;
		char szMenuString[MAX_PATH+16];
		if(lpHisData->nSource > 1){
			wsprintf(szMenuString, "&%d %s ...", nMenuItems + 1, lpHisData->szSource[0]);
		}else{
			wsprintf(szMenuString, "&%d %s", nMenuItems + 1, lpHisData->szSource[0]);
		}
		mi.wID = IDM_HISTORY + nMenuItems;
		mi.fState = MFS_ENABLED;
		mi.dwTypeData = TEXT(szMenuString);
		mi.dwItemData = nMenuItems;
		InsertMenuItem(hHisMenu, nMenuItems, TRUE, &mi);
	}
	// 履歴無し
	if(nMenuItems == 0){
		mi.wID = IDM_HISTORY;
		mi.dwTypeData = TEXT("履歴はありません");
		mi.fState = MFS_DISABLED;
		mi.dwItemData = 0;
		InsertMenuItem(hHisMenu, nMenuItems, TRUE, &mi);
	}

	return nMenuItems;
}

/*----------------------------------------------------------------------------------------
	おまけ
----------------------------------------------------------------------------------------*/
void ShowBootMessage(HWND hWndMain)
{
	AddOutputMessage(hWndMain, "COBOLソース(COPY句)を解析します、解析したいCOBOLコピー句をドロップしてください");
}

/*----------------------------------------------------------------------------------------
	コントロールのステータスを変更する
----------------------------------------------------------------------------------------*/
int SetControlState(HWND hWnd, const int nState)
{
	CDlgbar *lpCDlgbar = (CDlgbar *)GetProp(hWnd, CDLGBAR_CLASS);
	if(!lpCDlgbar) return -1;
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return -1;

	HMENU hMenu, hMenuFile, hMenuEdit, hMenuView, hMenuAna, hMenuAbout;
	hMenu = GetMenu(hWnd);
	hMenuFile = GetSubMenu(hMenu, MENU_POS_FILE);
	hMenuEdit = GetSubMenu(hMenu, MENU_POS_EDIT);
	hMenuView = GetSubMenu(hMenu, MENU_POS_VIEW);
	hMenuAna = GetSubMenu(hMenu, MENU_POS_ANA);
	hMenuAbout = GetSubMenu(hMenu, MENU_POS_ABOUT);
	HWND hToolbar_Ana = lpCDlgbar->GetToolbarHandle(ID_TOOLBAR_ANA);

	switch(nState){
	// 新規
	case STATE_NEW:
		EnableMenuItem(hMenu, MENU_POS_FILE, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_EDIT, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_VIEW, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ANA, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ABOUT, MF_ENABLED | MF_BYPOSITION);
//		EnableMenuItem(hMenuFile, IDM_SAVE, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuFile, MENU_FILE_POS_OPENDAT, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenuFile, MENU_FILE_POS_SAVE, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenuEdit, IDM_COPY, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuEdit, IDM_FIND, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuView, IDM_DATA_VIEW, MF_GRAYED | MF_BYCOMMAND);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW_APP, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENSRC, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_ANACBL, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENDAT, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_FIND, FALSE);
		break;
	// 解析中
	case STATE_ANALYSING:
		EnableMenuItem(hMenu, MENU_POS_FILE, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_EDIT, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_VIEW, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ANA, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ABOUT, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenuEdit, IDM_COPY, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuEdit, IDM_FIND, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuView, IDM_DATA_VIEW, MF_GRAYED | MF_BYCOMMAND);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW_APP, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENSRC, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_ANACBL, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENDAT, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_FIND, FALSE);
		break;
	// 解析完了
	case STATE_ANALYSYS:
		EnableMenuItem(hMenu, MENU_POS_FILE, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_EDIT, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_VIEW, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ANA, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ABOUT, MF_ENABLED | MF_BYPOSITION);
//		EnableMenuItem(hMenuFile, IDM_SAVE, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuFile, MENU_FILE_POS_OPENDAT, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenuFile, MENU_FILE_POS_SAVE, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenuEdit, IDM_COPY, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuEdit, IDM_FIND, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuView, IDM_DATA_VIEW, MF_GRAYED | MF_BYCOMMAND);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW_APP, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENSRC, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_ANACBL, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENDAT, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_FIND, TRUE);
		break;
	// 検索中
	case STATE_SEARCHING:
		EnableMenuItem(hMenu, MENU_POS_FILE, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_EDIT, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ANA, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(hMenuEdit, IDM_FIND, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuEdit, IDM_COPY, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(hMenuView, IDM_DATA_VIEW, MF_GRAYED | MF_BYCOMMAND);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW_APP, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENSRC, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_ANACBL, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENDAT, FALSE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_FIND, FALSE);
		break;
	// 検索完了
	case STATE_SEARCH:
		EnableMenuItem(hMenu, MENU_POS_FILE, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_EDIT, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ANA, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenuEdit, IDM_FIND, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuEdit, IDM_COPY, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuView, IDM_DATA_VIEW, MF_GRAYED | MF_BYCOMMAND);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW_APP, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENSRC, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_ANACBL, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENDAT, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_FIND, TRUE);
		break;
	//データオープン
	case STATE_DATOPEN:
		EnableMenuItem(hMenu, MENU_POS_FILE, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_EDIT, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenu, MENU_POS_ANA, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(hMenuEdit, IDM_FIND, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuEdit, IDM_COPY, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(hMenuView, IDM_DATA_VIEW, MF_ENABLED | MF_BYCOMMAND);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW_APP, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_NEW, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENSRC, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_ANACBL, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_OPENDAT, TRUE);
		lpCDlgbar->EnableTbButton(hToolbar_Ana, IDM_FIND, TRUE);
		break;
	}
	DrawMenuBar(hWnd);
	return 0;
}

/*----------------------------------------------------------------------------------------
	ウィンドウ作成後プロシジャ
----------------------------------------------------------------------------------------*/
int OnCreateWindow(HWND hWnd, LPSTR lpsCmdLine, const int nCmdShow)
{
	// ウィンドウサイズの復元
	int nWnShow = nCmdShow;
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(lpCSetup){
		if((lpCSetup->m_UIProp.m_bRestorMainWnd)
		&& (lpCSetup->m_UIProp.m_rcMainWnd.left != 0
		||  lpCSetup->m_UIProp.m_rcMainWnd.top != 0
		||  lpCSetup->m_UIProp.m_rcMainWnd.right != 0
		||  lpCSetup->m_UIProp.m_rcMainWnd.bottom != 0)){
			MoveWindow(hWnd,
				lpCSetup->m_UIProp.m_rcMainWnd.left,
				lpCSetup->m_UIProp.m_rcMainWnd.top,
				lpCSetup->m_UIProp.m_rcMainWnd.right - lpCSetup->m_UIProp.m_rcMainWnd.left,
				lpCSetup->m_UIProp.m_rcMainWnd.bottom - lpCSetup->m_UIProp.m_rcMainWnd.top,
				TRUE);
			if(lpCSetup->m_UIProp.m_bMainWndMaximize){
				ShowWindow(hWnd, nCmdShow);
				nWnShow = SW_SHOWMAXIMIZED;
			}
		}else{
			GetWindowRect(hWnd, &(lpCSetup->m_UIProp.m_rcMainWnd));
		}
		// アウトプット可視状態の復元
		if(!lpCSetup->m_UIProp.m_bShowOutputWnd) SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_OUTPUT_VIEW, 0L);
//		// レコードセレクタ可視状態の復元
//		if(lpCSetup->m_UIProp.m_bShowRecSelWnd) SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_RECSEL_VIEW, 0L);
	}
	// ウィンドウ表示
	ShowWindow(hWnd, nWnShow);
	UpdateWindow(hWnd);
	// コマンドライン取得
	if(GetCommandLineFile(hWnd) > 0){
		// 解析開始
		if (lpCSetup) {
			if (lpCSetup->m_AnalysysProp.m_bCmdLineSet)
				SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ANACBL, 0L);
		}
	}

	return 0;
}

/*----------------------------------------------------------------------------------------
	オーナードロー処理
----------------------------------------------------------------------------------------*/
LRESULT OnDrawItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(((LPDRAWITEMSTRUCT)lp)->CtlID != ID_TREELIKELIST) return (DefWindowProc(hWnd, msg, wp, lp));
	
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (DefWindowProc(hWnd, msg, wp, lp));

	// リストドロー
	return (lpCAnaLst->DrawItem(hWnd, msg, wp, lp));
}

/*----------------------------------------------------------------------------------------
	コマンドプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnCommand(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(LOWORD(wp)){
	case IDM_NEW_APP:
		return (NewAppication(hWnd, msg, wp, lp));
	case IDM_NEW:
		return (NewSource(hWnd, msg, wp, lp));
	case IDM_OPENSRC:
		return (OpenSource(hWnd, msg, wp, lp));
	case IDM_CLOSESRC:
		return (CloseSource(hWnd, msg, wp, lp));
	case IDM_CLOSEALLSRC:
		return (CloseAllSources(hWnd, msg, wp, lp));
	case IDM_REFLESHSRC:
		return (ReloadAllSources(hWnd, msg, wp, lp));
	case IDM_SRC_UP:
		MoveSource(hWnd, TRUE);
		break;
	case IDM_SRC_DOWN:
		MoveSource(hWnd, FALSE);
		break;
	case IDM_SHOPENSRC:
		OnOpenListSrc(hWnd);
		break;
	case IDM_OPENDAT:
		return OpenDataFileDlg(hWnd);
	case IDM_PREVDAT:
		return MoveDataRecord(hWnd, -1);
	case IDM_NEXTDAT:
		return MoveDataRecord(hWnd, 1);
	case IDM_SAVE_HTML:
	case IDM_SAVE_CSV:
	case IDM_SAVE_CBL:
		OnExportAnalysys(hWnd, LOWORD(wp));
		break;
	case IDM_DELETEHISTORY:
		return (OnDeleteHistoryAnalysys(hWnd, msg, wp, lp));
	case IDM_END:
		SendMessage(hWnd, WM_CLOSE, 0L, 0L);
		break;
	case IDM_EDITDLG:
		OnEditDlg(hWnd);
		break;
	// キャンセル処理
	case IDM_CANCELJOB:
		return (OnCancelJob(hWnd, msg, wp, lp));
	case IDM_PASTE:
	case IDM_PASTE2:
		OnPaste(hWnd);
		break;
	case IDM_COPY:
		OnCopy(hWnd);
		break;
	case IDM_FIND:
		if(ShowSearchDlg(hWnd) > 0) OutputTabSetCursel(TAB_SEARCH);
		break;
	case IDM_SEARCH_SEL:
		SearchExpandSel(hWnd);
		break;
	case IDM_SEARCH_CLEAR:
		SearchClearAll(hWnd);
		break;
	case IDM_OUTPUT_VIEW:
		return (OnOutputView(hWnd, msg, wp, lp));
	case IDM_TRACE_VIEW:
	case IDM_SOURCE_VIEW:
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case IDM_ANALYSYS_VIEW:
	case IDM_SEARCH_VIEW:
	case IDM_DATA_VIEW:
		return (OnOutputTabView(hWnd, msg, wp, lp));
//	case IDM_RECSEL_VIEW:
//		return (OnRecSelView(hWnd, msg, wp, lp));
	case IDM_AUTOEXPAND:
	case IDM_EXPAND:
	case IDM_ALLEXPAND:
	case IDM_COLLAPSE:
	case IDM_ITEMPROPERTY:
		return (OnAnaListView(hWnd, msg, wp, lp));
	case IDM_SETUP:
		return (OnShowSetupPage(hWnd, msg, wp, lp));
	case IDM_ANACBL:
		return (OnAnaCBL(hWnd, msg, wp, lp));
	case IDM_VIEWANACBL:
		return (OnShowConfigBox(hWnd, msg, wp, lp));
	case IDM_VERSIONINFO:
		return (OnVersionInfo(hWnd, msg, wp, lp));
//	case IDM_OPENHELP:
//		AddOutputMessage(hWnd, "ヘルプファイルは、まだ作ってないんよ...");
//		ShowHelp(hWnd);
//		break;
//	case IDM_OPENREADME:
//		return (OnOpenReadMe(hWnd, msg, wp, lp));
//	case IDM_LICENCE:
//	case IDM_REGIST:
//		AboutLicense(hWnd, LOWORD(wp));
//		return (0L);
	default:
		if(OnHistoryAnalysys(hWnd, (UINT)LOWORD(wp))) return (0L);
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	コントロール拡張メッセージ処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnNotify(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMHDR lpNmhdr = (LPNMHDR)lp;
	switch(lpNmhdr->idFrom){
	case ID_TOOLBAR_ANA:
		return (OnMainToolbarNotify(hWnd, msg, wp, lp));
	case ID_TREELIKELIST:
		return (OnListViewNotify(hWnd, msg, wp, lp));
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (DefWindowProc(hWnd, msg, wp, lp));
}

/*----------------------------------------------------------------------------------------
	ツールバー拡張メッセージ処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnMainToolbarNotify(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMHDR lpNmhdr = (LPNMHDR)lp;
	RECT rc;
	TPMPARAMS tpm;
	HMENU hSubMenu, hPopup;
	switch(lpNmhdr->code){
	case TBN_DROPDOWN:
		SendMessage(((LPNMTOOLBAR)lp)->hdr.hwndFrom,
			TB_GETRECT,
			(WPARAM)((LPNMTOOLBAR)lp)->iItem,
			(LPARAM)&rc);
		MapWindowPoints(((LPNMTOOLBAR)lp)->hdr.hwndFrom,
			HWND_DESKTOP,
			(LPPOINT)&rc,
			(sizeof(RECT) / sizeof(POINT)));
		tpm.cbSize = sizeof(TPMPARAMS);
		tpm.rcExclude.top = rc.top;
		tpm.rcExclude.left = rc.left;
		tpm.rcExclude.bottom = rc.bottom;
		tpm.rcExclude.right = rc.right;
		// メニューポップアップ
		hSubMenu = GetSubMenuHandle(hWnd);
		if(!hSubMenu) break;
		hPopup = NULL;
		switch(((LPNMTOOLBAR)lp)->iItem){
		case IDM_OPENSRC:
			hPopup = GetSubMenu(hSubMenu, SUB_MENU_OPENSERC);
			break;
		case IDM_ANACBL:
			hPopup = GetSubMenu(hSubMenu, SUB_MENU_ANACBL);
			break;
		}
		if(hPopup) TrackPopupMenuEx(hPopup,
							TPM_LEFTALIGN | TPM_VERTICAL | TPM_RIGHTBUTTON,
							rc.left, rc.bottom, hWnd, &tpm);
		DestroyMenu(hSubMenu);
		break;
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	リストビュー拡張メッセージ処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnListViewNotify(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);
	LPNMLISTVIEW lpNmLv = (LPNMLISTVIEW)lp;
	switch(lpNmLv->hdr.code){
	case NM_CLICK:
		if(lpNmLv->iItem < 0) break;
		return (OnClickListItem(hWnd, msg, wp, lp));
	case NM_DBLCLK:
		if(lpNmLv->iItem < 0) break;
		return (OnDblClickListItem(hWnd, msg, wp, lp));
//		if(lpCAnaLst->IsExpand(lpNmLv->iItem) == FLG_NONE){
//			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ITEMPROPERTY, 0L);
//		}else{
//			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_AUTOEXPAND, 0L);
//		}
		break;
	case LVN_KEYDOWN:
		if(lpNmLv->iItem < 0) break;
		switch(((LPNMLVKEYDOWN)lp)->wVKey){
		case VK_RETURN:
			if(lpCAnaLst->IsExpand(lpNmLv->iItem) == FLG_NONE){
				SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ITEMPROPERTY, 0L);
			}else{
				SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_AUTOEXPAND, 0L);
			}
			break;
		case VK_RIGHT:
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_EXPAND, 0L);
			break;
		case VK_LEFT:
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_COLLAPSE, 0L);
			break;
		case VK_APPS:
			return (OnContextBtn(hWnd, msg, wp, lp));
		default:
			return (DefWindowProc(hWnd, msg, wp, lp));
		}
		break;
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case LVN_ITEMCHANGED:
//		UpdateAnalysysMessage(hWnd, FALSE, ((LPNMLISTVIEW)lp)->iItem);
//		break;
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	左クリック処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnClickListItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);
	int nItem;
	if((nItem = lpCAnaLst->HitTest(ANALST_HITTEST_ICON)) < 0) return (DefWindowProc(hWnd, msg, wp, lp));
	if(lpCAnaLst->IsStruct(nItem)) lpCAnaLst->AutoExpandItem(nItem);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	左ダブルクリック処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnDblClickListItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);
	int nItem;
	if((nItem = lpCAnaLst->HitTest(ANALST_HITTEST_ITEM)) < 0) return (DefWindowProc(hWnd, msg, wp, lp));
	if(lpCAnaLst->IsExpand(nItem) == FLG_NONE){
		SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ITEMPROPERTY, 0L);
	}else{
		SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_AUTOEXPAND, 0L);
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	右クリック処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnContextMenu(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	POINT pt;
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	TrackListContextMenu(hWnd, &pt);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	リストコンテキストボタン処理プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnContextBtn(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);

	POINT pt;
	lpCAnaLst->GetSelPos(&pt);
	// 少～しズラす
	pt.x += 36;
	pt.y += 6;
	TrackListContextMenu(hWnd, &pt);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	右クリックポップアップ
----------------------------------------------------------------------------------------*/
int TrackListContextMenu(HWND hMainWnd, LPPOINT lppt)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hMainWnd, CANALST_CLASS);
	if(!lpCAnaLst) return -1;

	int nSelItem = lpCAnaLst->GetSel();
	if(nSelItem < 0) return -1;

	HMENU hMenu, hPopup = NULL;
//	hMenu = LoadMenu((HINSTANCE)GetWindowLong(hMainWnd, GWL_HINSTANCE),
	hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hMainWnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(IDR_MENU_LVPOPUP));

	switch(lpCAnaLst->IsExpand(nSelItem)){
	case FLG_NONE:
		hPopup = GetSubMenu(hMenu, POPUP_NORMALITEM);
		break;
	case FLG_EXPAND:
		hPopup = GetSubMenu(hMenu, POPUP_COLLAPSE);
		break;
	case FLG_COLLAPSE:
		hPopup = GetSubMenu(hMenu, POPUP_EXPAND);
		break;
	default:
		hPopup = NULL;
		break;
	}

	if(hPopup){
		MENUITEMINFO mif;
		ZeroMemory(&mif, sizeof(MENUITEMINFO));
		mif.cbSize = sizeof(MENUITEMINFO);
		mif.fMask = MIIM_STATE;
		mif.fState = MFS_DEFAULT;
		SetMenuItemInfo(hPopup, 0, TRUE, &mif);
		TrackPopupMenuEx(hPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, (int)lppt->x, (int)lppt->y, hMainWnd, NULL);
	}

	DestroyMenu(hMenu);

	return 0;
}

/*----------------------------------------------------------------------------------------
	ヴィジュアルスタイル変更の通知
----------------------------------------------------------------------------------------*/
LRESULT OnThemeChanged(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// WM_SIZEをトレース
	RECT rc;
	GetClientRect(hWnd, &rc);
	SendMessage(hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
	return (0L);
}

/*----------------------------------------------------------------------------------------
	もう一つ起動
----------------------------------------------------------------------------------------*/
LRESULT NewAppication(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	char szModuleName[MAX_PATH+1];
	if(GetModuleFileName(NULL, szModuleName, sizeof(szModuleName)) < 1) return (0L);
	AddOutputMessage(hWnd, "Yama64.exe をもう１つ起動します...");
	ShellExecute(HWND_DESKTOP, "open", szModuleName, NULL, NULL, SW_SHOW);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソースを閉じて新規作成
----------------------------------------------------------------------------------------*/
LRESULT NewSource(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return (0L);
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);

	if(lpCAnaLst->IsMakedList()){
		if(YESNOMSG(hWnd, "解析結果を消去します") == IDNO) return (0L);
	}

	// データクローズ
	CloseDataFile(hWnd);

	// 再初期化
	lpCAnaCBL->Initialize(hWnd);
	lpCAnaLst->ClearList();

	// リスト更新
	UpdateSourceMessage(hWnd, FALSE);
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	UpdateAnalysysMessage(hWnd, FALSE);
	AddSearchListMessage(hWnd, FALSE, -1);

	// コントロールの状態変更
	SetControlState(hWnd, STATE_NEW);

	// メッセージ
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(lpCStatus){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");
		lpCStatus->SetSbText(1, 0, "");
		lpCStatus->SetSbText(2, 0, "");
	}

	// タイトルバーの文字
	SetWindowText(hWnd, APP_NAME);

	AddOutputMessage(hWnd, "ファイル一覧をクリアしました");

	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソースを開く
----------------------------------------------------------------------------------------*/
LRESULT OpenSource(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return (0L);
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return (0L);
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return (0L);
	// 現在の設定ソース数
	int nSourceCount = lpCAnaCBL->GetSourceCount();
	// ファイルを開くダイアログ
	char szSrcFile[MAX_OPENFILE * MAX_PATH];
	if(nSourceCount > 0){
		// 既に設定済みならそれを継承する
		lpCAnaCBL->GetSourcePath(0, szSrcFile, sizeof(szSrcFile));
	}else{
		if(strlen(lpCSetup->m_PathProp.m_szDefOpenDir) > 0){
			if(lpCSetup->m_PathProp.m_szDefOpenDir[strlen(lpCSetup->m_PathProp.m_szDefOpenDir) - 1] == '\\'){
				wsprintf(szSrcFile, "%s*.cob", lpCSetup->m_PathProp.m_szDefOpenDir);
			}else{
				wsprintf(szSrcFile, "%s\\*.cob", lpCSetup->m_PathProp.m_szDefOpenDir);
			}
		}else{
			szSrcFile[0] = '\0';
		}
	}
	lpCStatus->SetSbText(0, SBT_NOBORDERS, "解析するソースを開きます");
	if(!GetOpenFileDlg(hWnd,
		szSrcFile, (const int)sizeof(szSrcFile),
		"COBOLｿｰｽﾌｧｲﾙ (*.cob;*.cbl;*.cobol)\0*.cob;*.cbl;*.cobol\0"
		"FORMｿｰｽﾌｧｲﾙ (*.smd)\0*.smd\0"
		"全てのﾌｧｲﾙ (*.*)\0*.*\0",
		"ｿｰｽﾌｧｲﾙを開く",
		"cob",
		"",
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT)){
		if(CommDlgExtendedError() == FNERR_BUFFERTOOSMALL)
			EXCMSG(hWnd, "バッファが不足した為、ファイルを開けませんでした\n"
						 "解析対象ファイルを少なくしてもう一度実行してください。");
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");
		return (0L);
	}
	// ソースを追加
	AddSourceFile(hWnd, szSrcFile);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソースを閉じる
----------------------------------------------------------------------------------------*/
LRESULT CloseSource(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return (0L);
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return (0L);

	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return (0L);

	int nList = 0;
	if((nList = SourcesListCount(hWndSplit)) < 1) return (0L);

	// 選択項目のlParam取得
	int nNo = SourcesListGetSelParam(hWndSplit);
	if(nNo < 0){
		EXCMSG(hWnd, "削除する項目が選択されていません");
	}else{
		// ソース一覧から削除
		lpCAnaCBL->RemSource(nNo);
		AddOutputMessage(hWnd, "一覧からソースを削除しました");
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "一覧からソースを削除しました");
	}

	// ソース一覧の更新
	UpdateSourceMessage(hWnd, FALSE);

	// 選択
	SourcesListSetSel(hWndSplit, nNo);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	すべてのソースを閉じる
----------------------------------------------------------------------------------------*/
LRESULT CloseAllSources(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return (0L);
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return (0L);

	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return (0L);
	if(SourcesListCount(hWndSplit) < 1){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ソースは未登録です");
		return (0L);
	}

	// ソース一覧全削除
	if(YESNOMSG(hWnd, "一覧の内容を全て削除します") == IDYES){
		lpCAnaCBL->RemSource(-1);
		AddOutputMessage(hWnd, "一覧のソースを全て削除しました");
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "一覧のソースを全て削除しました");
	}

	// ソース一覧の更新
	UpdateSourceMessage(hWnd, FALSE);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソースのリロード
----------------------------------------------------------------------------------------*/
int ReloadSourceFromNo(HWND hWndMain, const int nSrcNo)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return 0;
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWndMain, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return 0;
	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return 0;
	if(SourcesListCount(hWndSplit) < 0) return 0;
	// ＳＭＤは解析し直し
	SOURCEINFO SrcInfo;
	lpCAnaCBL->GetSourceData(nSrcNo, &SrcInfo);
	if((int)SrcInfo.lpParam == SRC_TYPE_SMD){
		if(!DelAddSmdSourceFile(hWndMain, nSrcNo)){
			AddOutputMessage(hWndMain, "FORMソースの更新失敗");
			lpCStatus->SetSbText(0, SBT_NOBORDERS, "FORMソース更新に失敗しました");
			return 0;
		}
	}
	// ソースリロード
	return (lpCAnaCBL->ReloadSource(nSrcNo));
}

/*----------------------------------------------------------------------------------------
	全ソースのリロード
----------------------------------------------------------------------------------------*/
LRESULT ReloadAllSources(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return (0L);
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return 0;
	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return 0;
	// 全て再読込み
	int nList = 0;
	if((nList = SourcesListCount(hWndSplit)) < 1){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ソースは未登録です");
		return (0L);
	}
	int nRet = 0;
	for(int i = 0; i < nList; i++){
		int nNo = SourcesListGetParam(hWndSplit, i);
		if((nRet = ReloadSourceFromNo(hWnd, nNo)) < 0) break;
	}
	if(nRet < 0){
		AddOutputMessage(hWnd, "ソース一覧の更新失敗");
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ソース一覧更新に失敗しました");
	}else{
		AddOutputMessage(hWnd, "ソース一覧の更新完了");
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ソース一覧更新しました");
	}
	// ソース一覧の更新
	UpdateSourceMessage(hWnd, FALSE);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソースのドロップ
----------------------------------------------------------------------------------------*/
/*
LRESULT OnDropFiles(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(ghAnaCBLThread){
		AddOutputMessage(hWnd, "解析中なのでファイルのドロップは受付けられません");
		return (0L);
	}

	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return (0L);

	HDROP hDrop = (HDROP)wp;
	if(!hDrop) return (0L);
	// 解析初期化
	if(lpCSetup->m_AnalysysProp.m_bDropInit) SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_NEW, 0L); 
	char szDropFile[MAX_PATH + 1];
	int nDrop = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	for(int i = 0; i < nDrop; i++){
		if(DragQueryFile(hDrop, i, szDropFile, sizeof(szDropFile)) < 1) continue;
		if(GetFileAttributes(szDropFile) & FILE_ATTRIBUTE_DIRECTORY){
			AddOutputMessage(hWnd, "フォルダーのドロップは無効です");
			continue;
		}
		AddSourceFile(hWnd, szDropFile);
	}
	DragFinish(hDrop);
	UpdateSourceMessage(hWnd, TRUE);
	// 解析開始
	if(lpCSetup->m_AnalysysProp.m_bDropSet) SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ANACBL, 0L);
	return (0L);
}
*/
LRESULT OnDropFiles(
	HWND hWnd,
	UINT msg,
	WPARAM wp,
	LPARAM lp)
{
	HDROP hDrop = (HDROP)wp;
	if (!hDrop) return 0L;

	if (ghAnaCBLThread) {
		AddOutputMessage(
			hWnd,
			"解析中なのでファイルのドロップは受付けられません");
		DragFinish(hDrop);
		return 0L;
	}

	CYamaSetup* lpCSetup = (CYamaSetup*)GetProp(hWnd, CSETUP_CLASS);

	if (!lpCSetup) {
		DragFinish(hDrop);
		return 0L;
	}

	int nDrop = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	BOOL bSourceAdded = FALSE;

	for (int i = 0; i < nDrop; i++) {

		char szDropFile[MAX_PATH + 1];

		if (DragQueryFile(
			hDrop,
			i,
			szDropFile,
			sizeof(szDropFile)) < 1)
		{
			continue;
		}

		DWORD dwAttr =
			GetFileAttributes(
				szDropFile);

		if (dwAttr == INVALID_FILE_ATTRIBUTES) {
			continue;
		}

		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {

			AddOutputMessage(
				hWnd,
				"フォルダのドロップは無効です");

			continue;
		}

		DROPFILE_TYPE nType = GetDropFileType(lpCSetup, szDropFile);

		switch (nType)
		{
		case DROPFILE_COBOL:

			AddOutputMessage(hWnd, "COBOLソースファイルのドロップ");

			// 最初のCOBOLソース追加時だけ初期化
			if (!bSourceAdded &&
				lpCSetup->m_AnalysysProp.m_bDropInit)
			{
				SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_NEW, 0L);
			}

			AddSourceFile(hWnd, szDropFile);

			bSourceAdded = TRUE;

			break;

		case DROPFILE_DATA:

			AddOutputMessage(hWnd, "データファイルのドロップ");

			// ここは既存のデータファイルOPEN処理へ
			OpenDataFile(hWnd, szDropFile);

			break;

		default:

			AddOutputMessage(hWnd, "判別不能ファイルのドロップ");
			INT_PTR nResult = DialogBoxParam((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				MAKEINTRESOURCE(IDD_JUDGEBOX),
				hWnd,
				JudgeDlgProc,
				(LPARAM)szDropFile);
			switch (nResult) {
			case IDC_BTN_SRC:
				if (!bSourceAdded && lpCSetup->m_AnalysysProp.m_bDropInit)
				{
					SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_NEW, 0L);
				}
				AddSourceFile(hWnd, szDropFile);
				bSourceAdded = TRUE;
				break;
			case IDC_BTN_DAT:
				OpenDataFile(hWnd, szDropFile);
				break;
			}

			break;
		}
	}

	DragFinish(hDrop);

	if (bSourceAdded) {

		UpdateSourceMessage(
			hWnd,
			TRUE);

		if (lpCSetup->m_AnalysysProp.m_bDropSet) {

			SendMessage(
				hWnd,
				WM_COMMAND,
				(WPARAM)IDM_ANACBL,
				0L);
		}
	}

	return 0L;
}

DROPFILE_TYPE GetDropFileType(
	CYamaSetup* lpCSetup,
	const char* pszFilePath)
{
	if (!lpCSetup ||
		!pszFilePath ||
		!*pszFilePath)
	{
		return DROPFILE_UNKNOWN;
	}

	const char* pszExt = strrchr(pszFilePath, '.');
	if (pszExt && *(pszExt + 1) != '\0')
	{
		pszExt++;
		char szExt[32];
		strcpy_s(szExt,
			sizeof(szExt),
			pszExt);
		_strlwr_s(szExt,
			sizeof(szExt));

		if (IsCobolExtension(lpCSetup->m_AnalysysProp.m_szCblExt, szExt))
		{
			return DROPFILE_COBOL;
		}
	}

	return JudgeFileContent(pszFilePath);
}

BOOL IsCobolExtension(
	const char* pszExtensions,
	const char* pszExt)
{
	if (!pszExtensions ||
		!pszExt)
	{
		return FALSE;
	}

	char szBuf[256];

	strcpy_s(
		szBuf,
		sizeof(szBuf),
		pszExtensions);

	char* pContext = NULL;

	char* p =
		strtok_s(
			szBuf,
			";",
			&pContext);

	while (p) {

		while (*p == ' ' ||
			*p == '\t')
		{
			p++;
		}

		if (*p == '.') {
			p++;
		}

		if (_stricmp(
			p,
			pszExt) == 0)
		{
			return TRUE;
		}

		p = strtok_s(NULL, ";", &pContext);
	}

	return FALSE;
}

DROPFILE_TYPE JudgeFileContent(const char* pszFilePath)
{
	FILE* fp = NULL;

	if (fopen_s(&fp, pszFilePath, "rb") != 0 || !fp)
	{
		return DROPFILE_UNKNOWN;
	}

	const int BUF_SIZE = 4096;
	unsigned char szBuf[BUF_SIZE + 1];
	size_t nRead = fread(szBuf, 1, BUF_SIZE, fp);
	fclose(fp);
	if (nRead == 0) {
		return DROPFILE_UNKNOWN;
	}
	szBuf[nRead] = '\0';

	// --------------------------------------------------
	// バイナリらしさ
	// --------------------------------------------------
	int nNull = 0;
	int nCtrl = 0;

	for (size_t i = 0;
		i < nRead;
		i++)
	{
		unsigned char c =
			szBuf[i];

		if (c == 0) {
			nNull++;
			continue;
		}

		if (c < 0x20 &&
			c != '\r' &&
			c != '\n' &&
			c != '\t' &&
			c != '\f')
		{
			nCtrl++;
		}
	}

	// NULを含むならかなりデータ寄り
	if (nNull > 0) {
		return DROPFILE_DATA;
	}

	// 制御文字が多い
	if (nCtrl >
		(int)(nRead / 20))
	{
		return DROPFILE_DATA;
	}

	// ファイルサイズが10MB超
	WIN32_FILE_ATTRIBUTE_DATA fad = {};
	if (GetFileAttributesEx(
		pszFilePath,
		GetFileExInfoStandard,
		&fad))
	{
		ULARGE_INTEGER uli;
		uli.HighPart =
			fad.nFileSizeHigh;
		uli.LowPart =
			fad.nFileSizeLow;
		if (uli.QuadPart > DROP_DATA_SIZE_THRESHOLD) {
			return DROPFILE_DATA;
		}
	}

	// --------------------------------------------------
	// COBOLっぽさ
	// --------------------------------------------------
	char* pszText =
		(char*)szBuf;

	int nCobolScore = 0;

	if (StrStrIEx(pszText, " PIC "))
	{
		nCobolScore += 1;
	}

	if (nCobolScore >= 3) {
		return DROPFILE_COBOL;
	}

	// テキストっぽいがCOBOLか不明
	return DROPFILE_UNKNOWN;
}

const char* StrStrIEx(const char* pszText, const char* pszFind)
{
	if (!pszText ||
		!pszFind ||
		!*pszFind)
	{
		return NULL;
	}

	size_t nFindLen =
		strlen(pszFind);

	for (const char* p = pszText;
		*p;
		p++)
	{
		if (_strnicmp(
			p,
			pszFind,
			nFindLen) == 0)
		{
			return p;
		}
	}

	return NULL;
}

/*----------------------------------------------------------------------------------------
	ソースを追加
----------------------------------------------------------------------------------------*/
int AddSourceFile(HWND hWndMain, const char *pszSource, BOOL bUpdate)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWndMain, CSPLITWND_CLASS);
	if(!lpCWndSplit) return 0;
/*
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return 0;
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return 0;
*/
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return 0;

	// ファイル追加
	int nCount = 0;
	char szDir[MAX_PATH];
	szDir[0] = '\0';
	for(const char *p = pszSource; *p != '\0'; p++){
		const char *pszFile = p;
//		for(; *p != '\0'; p++){}
		if(GetFileAttributes(pszFile) & FILE_ATTRIBUTE_DIRECTORY){
//			strcpy(szDir, pszFile);
			strcpy_s(szDir, sizeof(szDir), pszFile);
//			if(szDir[strlen(szDir) - 1] != '\\') strcat(szDir, "\\");
			if(szDir[strlen(szDir) - 1] != '\\') strcat_s(szDir, sizeof(szDir), "\\");
		}else{
			int nNo = 0;
			char szSrcPath[MAX_PATH];
			if(strlen(szDir) > 0){
				wsprintf(szSrcPath, "%s%s", szDir, pszFile);
			}else{
//				strcpy(szSrcPath, pszFile);
				strcpy_s(szSrcPath, sizeof(szSrcPath), pszFile);
			}
			// 拡張子によってソースの種類判別
			char *p, *pExt;
			for(pExt = p = (char *)szSrcPath; *p != '\0'; p++){
				if(IsDBCSLeadByte(*p)){
					p++;
					continue;
				}
				if(*p == '.') pExt = p;
			}
			pExt++;
			if(strcmp(CharLower(pExt),
				CharLower(lpCSetup->m_CoreAnalysysProp.m_szSmdExt)) == 0){	// 小文字になっちゃうけど知らん
/*
				char szTmpFile[MAX_PATH];
				GetTempFileName(gszTmpDir, "YA", 0, szTmpFile);
				if(lpCAnaCBL->AddForm(szSrcPath, szTmpFile) < 0){
					EXCMSG(hWndMain, "ＦＯＲＭソースファイルの設定に失敗しました");
				}else{
					wsprintf(szTraceMsg, "FORMソース[%s]を追加しました", szSrcPath);
					AddOutputMessage(hWndMain, szTraceMsg);
					lpCStatus->SetSbText(0, SBT_NOBORDERS, szTraceMsg);
					nCount++;
				}
*/
				if((nNo = AddSmdSourceFile(hWndMain, szSrcPath)) >= 0) nCount++;
			}else{
/*
				if(lpCAnaCBL->AddSource(szSrcPath) < 0){
					EXCMSG(hWndMain, "ＣＯＢＯＬソースファイルの設定に失敗しました");
				}else{
					wsprintf(szTraceMsg, "COBOLソース[%s]を追加しました", szSrcPath);
					AddOutputMessage(hWndMain, szTraceMsg);
					lpCStatus->SetSbText(0, SBT_NOBORDERS, szTraceMsg);
					nCount++;
				}
*/
				if((nNo = AddCobolSourceFile(hWndMain, szSrcPath)) >= 0) nCount++;
			}
		}
		p += strlen(p) - 1;
	}
	// ソース一覧の更新
	if(nCount > 0 && bUpdate) UpdateSourceMessage(hWndMain, TRUE);
	return nCount;
}

/*----------------------------------------------------------------------------------------
	ＣＯＢＯＬソースを追加
----------------------------------------------------------------------------------------*/
int AddCobolSourceFile(HWND hWnd, const char *pszSource)
{
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return -1;
	char szTraceMsg[MAX_PATH * 2];
	int nNo = 0;
	// COBOLソース追加
	if((nNo = lpCAnaCBL->AddSource(pszSource)) < 0){
		EXCMSG(hWnd, "ＣＯＢＯＬソースファイルの設定に失敗しました");
		return -1;
	}else{
		wsprintf(szTraceMsg, "COBOLソース[%s]を追加しました", pszSource);
		AddOutputMessage(hWnd, szTraceMsg);
		lpCStatus->SetSbText(0, SBT_NOBORDERS, szTraceMsg);
	}
	return nNo;
}

/*----------------------------------------------------------------------------------------
	ＳＭＤソースを追加
----------------------------------------------------------------------------------------*/
int AddSmdSourceFile(HWND hWnd, const char *pszSource)
{
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return -1;
	char szTraceMsg[MAX_PATH * 2];
	int nNo = 0;
	// SMDファイル解析して追加
	char szTmpFile[MAX_PATH];
	GetTempFileName(gszTmpDir, "YA", 0, szTmpFile);
	if((nNo = lpCAnaCBL->AddForm(pszSource, szTmpFile)) < 0){
		EXCMSG(hWnd, "ＦＯＲＭソースファイルの設定に失敗しました");
		return -1;
	}else{
		wsprintf(szTraceMsg, "FORMソース[%s]を追加しました", pszSource);
		AddOutputMessage(hWnd, szTraceMsg);
		lpCStatus->SetSbText(0, SBT_NOBORDERS, szTraceMsg);
	}
	return nNo;
}

/*----------------------------------------------------------------------------------------
	ＳＭＤソースを追加→削除で修正
----------------------------------------------------------------------------------------*/
BOOL DelAddSmdSourceFile(HWND hWndMain, const int nSrcNo)
{
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return FALSE;
	// ソース情報取得
	SOURCEINFO SrcInfo;
	lpCAnaCBL->GetSourceData(nSrcNo, &SrcInfo);
	if((int)SrcInfo.lpParam != SRC_TYPE_SMD) return TRUE;
	// とりあえず追加
	int nAddNo = 0;
	if((nAddNo = AddSmdSourceFile(hWndMain, SrcInfo.szParam)) < 0) return FALSE;
	// 既存と置換え
	if(lpCAnaCBL->SwapSourceNo(nSrcNo, nAddNo) < 0) return FALSE;
	// 既存を削除
	if(lpCAnaCBL->RemSource(nAddNo) < 0) return FALSE;
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ソース解析順の変更
----------------------------------------------------------------------------------------*/
void MoveSource(HWND hWnd, const BOOL bUp)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return;
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return;
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return;

	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return;

	// 登録が１つのみは意味なし
	if(SourcesListCount(hWndSplit) < 2) return;

	// 選択項目のlParam取得
	int nOldNo = SourcesListGetSelParam(hWndSplit), nNewNo = -1;
	if(nOldNo < 0){
		EXCMSG(hWnd, "移動する項目が選択されていません");
	}else{
		int nLastNo = lpCAnaCBL->GetSourceCount() - 1;
		nNewNo = nOldNo;
		if(bUp){
			if(nOldNo < 1){
				lpCStatus->SetSbText(0, SBT_NOBORDERS, "これ以上上には移動できません");
			}else{
				nNewNo = nOldNo - 1;
				lpCAnaCBL->SwapSourceNo(nNewNo, nOldNo);
				lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");
			}
		}else{
			if(nLastNo > nOldNo){
				nNewNo = nOldNo + 1;
				lpCAnaCBL->SwapSourceNo(nNewNo, nOldNo);
				lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");
			}else{
				lpCStatus->SetSbText(0, SBT_NOBORDERS, "これ以上下には移動できません");
			}
		}
	}

	// ソース一覧の更新
	UpdateSourceMessage(hWnd, TRUE);

	// 新項目がクライアント領域外ならばスクロール
	// 選択する
	SourcesListSetSel(hWndSplit, nNewNo);
}

/*----------------------------------------------------------------------------------------
	ソース解析一覧のソースを開く
----------------------------------------------------------------------------------------*/
BOOL OnOpenListSrc(HWND hWnd)
{
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return FALSE;
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return FALSE;
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return FALSE;

	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return FALSE;

	int nList = 0;
	if((nList = SourcesListCount(hWndSplit)) < 1){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "(ﾟДﾟ)ﾊｧ?");
		return FALSE;
	}

	// 選択項目のlParam取得
	int nNo = SourcesListGetSelParam(hWndSplit);
	if(nNo < 0){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "選択項目がわかりません...");
		return FALSE;
	}else{
		SOURCEINFO SrcInfo;
		lpCAnaCBL->GetSourceData(nNo, &SrcInfo);
		switch((int)SrcInfo.lpParam){
		case SRC_TYPE_CBL:
		case SRC_TYPE_SMD:
			OpenSourcePath(hWnd, nNo);
			break;
		case SRC_TYPE_EDIT:
			OnEditDlg(hWnd, nNo);
			break;
		default:
			break;
		}
	}

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ソースコード編集ダイアログを表示
----------------------------------------------------------------------------------------*/
int OnEditDlg(HWND hWndMain, const int nSrcNo/* = -1*/)
{
	// ステータスバー操作クラスインスタンス
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return -1;
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	// 設定ファイル操作クラスインスタンス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return 0;
	int nRetNo = 0;
	char szTraceMsg[256];
	RECT rcEditDlg = { 
		lpCSetup->m_EditDialogProp.nX,
		lpCSetup->m_EditDialogProp.nY,
		lpCSetup->m_EditDialogProp.nWidth,
		lpCSetup->m_EditDialogProp.nHeight
	};
	nRetNo = CreateEditDlg(hWndMain, lpCAnaCBL, &rcEditDlg, nSrcNo);
	// 終了位置・サイズ保持
	lpCSetup->m_EditDialogProp.nX = rcEditDlg.left;
	lpCSetup->m_EditDialogProp.nY = rcEditDlg.top;
	lpCSetup->m_EditDialogProp.nWidth = rcEditDlg.right;
	lpCSetup->m_EditDialogProp.nHeight = rcEditDlg.bottom;
	// キャンセル時は中止する
	if(nRetNo < 0) return -1;
	// 結果の反映
	if(nSrcNo < 0){
		wsprintf(szTraceMsg, "No.%dへCOBOLソースコードを追加しました", nRetNo + 1);
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "COBOLソースコードを追加");
	}else{
		wsprintf(szTraceMsg, "No.%dのCOBOLソースコードを編集しました", nRetNo + 1);
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "COBOLソースコードを編集");
	}
	AddOutputMessage(hWndMain, szTraceMsg);
	// ソース一覧の更新
	UpdateSourceMessage(hWndMain, TRUE);
	return 0;
}

/*----------------------------------------------------------------------------------------
	ソースコードをペタッ
----------------------------------------------------------------------------------------*/
int OnPaste(HWND hWndMain)
{
	// ステータスバー操作クラスインスタンス
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return -1;
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return -1;
	// クリップボード操作
	if(!OpenClipboard(NULL)){
		ERRMSG(hWndMain, "クリップボードが開けません･･･");
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ｸﾘｯﾌﾟﾎﾞｰﾄﾞが開けません");
		return -1;
	}
	int nRetNo = 0;
	char szTraceMsg[256];
	HANDLE hSrc = GetClipboardData(CF_TEXT);
	if(hSrc == NULL){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ｸﾘｯﾌﾟﾎﾞｰﾄﾞにﾃｷｽﾄﾃﾞｰﾀはありません");
	}else{
		char *pszSrcCode = (char *)GlobalLock(hSrc);
		if(pszSrcCode){
			// 解析初期化
			if(lpCSetup->m_AnalysysProp.m_bPasteInit) SendMessage(hWndMain, WM_COMMAND, (WPARAM)IDM_NEW, 0L);
			nRetNo = lpCAnaCBL->AddSourceCode(pszSrcCode);
			GlobalUnlock(hSrc);
			if(nRetNo < 0){
				wsprintf(szTraceMsg, "クリップボードにデータはありません");
				lpCStatus->SetSbText(0, SBT_NOBORDERS, "NoData");
			}else{
				wsprintf(szTraceMsg, "No.%dへCOBOLソースコードを貼り付けました", nRetNo + 1);
				lpCStatus->SetSbText(0, SBT_NOBORDERS, "Paste");
			}
			AddOutputMessage(hWndMain, szTraceMsg);
		}
	}
	CloseClipboard();
	UpdateSourceMessage(hWndMain, TRUE);
	// 解析開始
	if(lpCSetup->m_AnalysysProp.m_bPasteSet) SendMessage(hWndMain, WM_COMMAND, (WPARAM)IDM_ANACBL, 0L);
	return nRetNo;
}

/*----------------------------------------------------------------------------------------
	クリップボードにコピー
----------------------------------------------------------------------------------------*/
int OnCopy(HWND hWndMain)
{
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return 0;
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWndMain, CANALST_CLASS);
	if(!lpCAnaLst) return 0;
	lpCAnaLst->ClipCopy(hWndMain, lpCSetup->m_CoreAnalysysProp.m_nSeq);
	return 0;
}

/*----------------------------------------------------------------------------------------
	ソースを開く
----------------------------------------------------------------------------------------*/
int OpenSourcePath(HWND hWndMain, const int nSrcNo)
{
	if(nSrcNo < 0) return -1;
	// ステータスバー操作クラスインスタンス
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return -1;
	// ＣＯＢＯＬ解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	// 設定ファイル操作クラスインスタンス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return -1;
	// ソース情報の取得
	SOURCEINFO SrcInfo;
	lpCAnaCBL->GetSourceData(nSrcNo, &SrcInfo);
	// 種類によって開き方変更
	DWORD dwExitCode = 0;
	if((int)SrcInfo.lpParam == SRC_TYPE_SMD){
		if(strlen(lpCSetup->m_PathProp.m_szSmdOpen) > 0){
//			int nNo = 0;
			if(StartProcess(hWndMain,
				lpCSetup->m_PathProp.m_szSmdOpen,
				SrcInfo.szParam,
				&dwExitCode)){
				// うまく行ったらソースのリロード
				if(!DelAddSmdSourceFile(hWndMain, nSrcNo)) ERRMSG(hWndMain, "FORMソースのリロードが失敗");
			}
		}else{
			// 指定が無ければシェルで開く
			ShellExecute(hWndMain, "open", SrcInfo.szParam, NULL, NULL, SW_SHOWNORMAL);
		}
	}else{
		if(strlen(lpCSetup->m_PathProp.m_szCobolOpen) > 0){
			if(StartProcess(hWndMain,
				lpCSetup->m_PathProp.m_szCobolOpen,
				SrcInfo.szSourcePath,
				&dwExitCode)){
				// うまく行ったらソースのリロード
				if(ReloadSourceFromNo(hWndMain, nSrcNo) < 0){
					AddOutputMessage(hWndMain, "ソースの更新失敗");
					lpCStatus->SetSbText(0, SBT_NOBORDERS, "ソース更新に失敗しました");
				}
			}
		}else{
			// 指定が無ければシェルで開く
			ShellExecute(hWndMain, "open", SrcInfo.szSourcePath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	// ソース一覧の更新
	UpdateSourceMessage(hWndMain, TRUE);
	return 0;
}

/*----------------------------------------------------------------------------------------
	コマンドの実行
----------------------------------------------------------------------------------------*/
BOOL StartProcess(HWND hWndMain,
				  const char *pszApplication, const char *pszParam,
				  LPDWORD lpdwExitCode)
{
	char szTraceMsg[1024], szCmdLine[1024], szCurDir[MAX_PATH+1];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	// コマンドライン
	wsprintf(szCmdLine, "%s \"%s\"", pszApplication, pszParam);
	// カレントディレクトリ
	char *p, *r;
//	strcpy(szCurDir, pszApplication);
	strcpy_s(szCurDir, sizeof(szCurDir), pszApplication);
	for(p = r = szCurDir; *p != '\0'; p++){
		if(IsDBCSLeadByte(*p)){
			p++;
			continue;
		}
		if(*p == '\\') r = p;
	}
	*r = '\0';
	// トレースに出力
	wsprintf(szTraceMsg, "[%s]の実行開始...", pszApplication);
	AddOutputMessage(hWndMain, szTraceMsg);
	// プロセスの作成
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;
	if(!CreateProcess(NULL,
			szCmdLine,
			NULL,
			NULL,
			TRUE,
			NORMAL_PRIORITY_CLASS,
			NULL,
			szCurDir,
			&si,
			&pi)){
		wsprintf(szTraceMsg, "[%s]は実行出来ません Σ(ﾟдﾟlll)", pszApplication);
		AddOutputMessage(hWndMain, szTraceMsg);
		return FALSE;
	}
	// スレッドハンドルクローズ
	CloseHandle(pi.hThread);
	// プロセスの終了待ち
	AddOutputMessage(hWndMain, "ｱﾌﾟﾘｹｰｼｮﾝの終了を監視します...操作は一切受付けません");
	WaitForSingleObject(pi.hProcess, INFINITE);
	// プロセス終了コード取得
	GetExitCodeProcess(pi.hProcess, lpdwExitCode);
	// プロセスハンドルクローズ
	CloseHandle(pi.hProcess);
	// 実行の終了
	wsprintf(szTraceMsg, "[%s]は終了ｺｰﾄﾞ %d で終了しました", pszApplication, *lpdwExitCode);
	AddOutputMessage(hWndMain, szTraceMsg);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	検索結果一覧からのツリー天海
----------------------------------------------------------------------------------------*/
void SearchExpandSel(HWND hWnd)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return;
	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return;
	ExpandAnaLstEx(SearchListGetParam(hWndSplit));
}

/*----------------------------------------------------------------------------------------
	検索結果一覧の消去
----------------------------------------------------------------------------------------*/
void SearchClearAll(HWND hWnd)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return;
	lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, 0L, (LPARAM)-1);
}

/*----------------------------------------------------------------------------------------
	キャンセル処理
----------------------------------------------------------------------------------------*/
LRESULT OnCancelJob(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
//	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	// 今、何してる？
	if(lpCAnaCBL->IsAnalysys()){
		// 解析中
		if(YESNOMSG(hWnd, "解析を中止します") == IDYES) lpCAnaCBL->CancelAnalysys();
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	トレースウィンドウ表示変更
----------------------------------------------------------------------------------------*/
LRESULT OnOutputView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);
	HMENU hViewMenu;
	if(!(hViewMenu = GetMenuHandle(hWnd, MENU_POS_VIEW))) return (0L);
	MENUITEMINFO mif;
	ZeroMemory(&mif, sizeof(MENUITEMINFO));
	mif.cbSize = sizeof(MENUITEMINFO);
	mif.fMask = MIIM_STATE;
	if(SW_HIDE == lpCWndSplit->GetShow()){
		lpCWndSplit->Show();
		mif.fState = MFS_CHECKED;
	}else{
		lpCWndSplit->Hide();
		mif.fState = MFS_UNCHECKED;
	}
	SetMenuItemInfo(hViewMenu, IDM_OUTPUT_VIEW, FALSE, &mif);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	トレースウィンドウタブ変更
----------------------------------------------------------------------------------------*/
LRESULT OnOutputTabView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(LOWORD(wp)){
	case IDM_TRACE_VIEW:
		OutputTabSetCursel(TAB_OUTPUT);
		break;
	case IDM_SOURCE_VIEW:
		OutputTabSetCursel(TAB_SOURCE);
		break;
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case IDM_ANALYSYS_VIEW:
//		OutputTabSetCursel(TAB_ANALYSYS);
//		break;
	case IDM_SEARCH_VIEW:
		OutputTabSetCursel(TAB_SEARCH);
		break;
	case IDM_DATA_VIEW:
		OutputTabSetCursel(TAB_DATA);
		break;
	default:
		return FALSE;
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	タブインデックスからメニューＩＤ
----------------------------------------------------------------------------------------*/
UINT GetTabMenuId(const UINT uiTab)
{
	switch(uiTab){
	case TAB_OUTPUT:
		return IDM_TRACE_VIEW;
	case TAB_SOURCE:
		return IDM_SOURCE_VIEW;
//	ﾌﾟﾛﾊﾟﾃｨｳｨﾝﾄﾞｳに併合
//	case TAB_ANALYSYS:
//		return IDM_ANALYSYS_VIEW;
	case TAB_SEARCH:
		return IDM_SEARCH_VIEW;
	case TAB_DATA:
		return IDM_DATA_VIEW;
	}
	return 0;
}

///*----------------------------------------------------------------------------------------
///	レコードセレクタ窓更
//----------------------------------------------------------------------------------------*/
//LRESULT OnRecSelView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
//{
//	CWndSplitter *lpCWndSplit2 = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS2);
//	if(!lpCWndSplit2) return (0L);
//	HMENU hViewMenu;
//	if(!(hViewMenu = GetMenuHandle(hWnd, MENU_POS_VIEW))) return (0L);
//	MENUITEMINFO mif;
//	ZeroMemory(&mif, sizeof(MENUITEMINFO));
//	mif.cbSize = sizeof(MENUITEMINFO);
//	mif.fMask = MIIM_STATE;
//	if(SW_HIDE == lpCWndSplit2->GetShow()){
//		lpCWndSplit2->Show();
//		mif.fState = MFS_CHECKED;
//	}else{
//		lpCWndSplit2->Hide();
//		mif.fState = MFS_UNCHECKED;
//	}
//	SetMenuItemInfo(hViewMenu, IDM_RECSEL_VIEW, FALSE, &mif);
//	return (0L);
//}

/*----------------------------------------------------------------------------------------
	解析結果ツリー表示変更
----------------------------------------------------------------------------------------*/
LRESULT OnAnaListView(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);
	// ステータスバー制御クラスインスタンス
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(!lpCStatus) return (DefWindowProc(hWnd, msg, wp, lp));

	int nSelItem = lpCAnaLst->GetSel();
	if(nSelItem < 0) return (0L);

	// プログレス表示
	int nProgressMax = 0;
	if((LOWORD(wp) == IDM_AUTOEXPAND)
	|| (LOWORD(wp) == IDM_EXPAND)
	|| (LOWORD(wp) == IDM_COLLAPSE)){
		nProgressMax = lpCAnaLst->GetChildCount(nSelItem);
		if(nProgressMax > SHOW_PROG_NODE){
			lpCStatus->SetProg(0, nProgressMax, 1);
			lpCStatus->ShowProg(TRUE);
		}
	}

	int nRet = 0;
	switch(LOWORD(wp)){
	case IDM_AUTOEXPAND:
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "処理中...");
		lpCAnaLst->AutoExpandItem(nSelItem);
		break;
	case IDM_EXPAND:
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚｺｰﾄﾞ展開...");
		lpCAnaLst->ExpandItem(nSelItem);
		break;
	case IDM_ALLEXPAND:
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "全て展開します...");
		lpCAnaLst->ExpandAllItems(nSelItem);
		break;
	case IDM_COLLAPSE:
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "閉じてます...");
		lpCAnaLst->CollapseItem(nSelItem);
		break;
	case IDM_ITEMPROPERTY:
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾌﾟﾛﾊﾟﾃｨの表示");
		nRet = ShowItemProperty(hWnd);
		break;
	}

	// プログレス非表示
	if((LOWORD(wp) == IDM_AUTOEXPAND)
	|| (LOWORD(wp) == IDM_EXPAND)
	|| (LOWORD(wp) == IDM_COLLAPSE)){
		if(nProgressMax > SHOW_PROG_NODE) lpCStatus->ShowProg(FALSE);
	}

	lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚﾃﾞｨ");

	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソース解析
----------------------------------------------------------------------------------------*/
LRESULT OnAnaCBL(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(ghAnaCBLThread){
		DWORD dwExitCode = 0;
		GetExitCodeThread(ghAnaCBLThread, &dwExitCode);
		if(dwExitCode == STILL_ACTIVE){
			AddOutputMessage(hWnd, "処理中です...");
			return (0L);
		}
	}

	// 設定ファイル操作クラスインスタンス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return (0L);
	if(lpCSetup->m_AnalysysProp.m_bBeginMsg){
		if(ShowConfigBox(hWnd, TRUE) == IDCANCEL){
			AddOutputMessage(hWnd, "解析中止...");
			return (0L);
		}
	}

	// 検索結果削除
	SearchClearAll(hWnd);

	// データファイルを閉じる
	CloseDataFile(hWnd);

	ghAnaCBLThread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)ThreadAnaCBL, (LPVOID)hWnd,
		0, &gdwAnaCBLThreadID);
	if(!ghAnaCBLThread) EXCMSG(hWnd, "解析処理の起動に失敗しました");

	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソース解析スレッド
----------------------------------------------------------------------------------------*/
DWORD WINAPI ThreadAnaCBL(LPVOID lpParameter)
{
	HWND hWndMain = (HWND)lpParameter;
	if(!hWndMain) return 0;

	AddOutputMessage(hWndMain, "ソースの解析を開始しました...");

	CAnaCBL *lpCAnaCBL = NULL;
	CProgStbar *lpCStatus = NULL;
	CAnalysisList *lpCAnaLst = NULL;
	CYamaSetup *lpCSetup = NULL;

	try{
		// 開始メッセージ
		PostMessage(hWndMain, UM_BEGINANACBL, (WPARAM)STATE_ANALYSING, 0L);

		// ＣＯＢＯＬ解析クラスインスタンス
		lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
		if(!lpCAnaCBL) throw (const int)STATE_NEW;
		// ステータスバー制御クラスインスタンス
		lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
		if(!lpCStatus) throw (const int)STATE_NEW;
		// 解析結果リスト制御クラスインスタンス
		lpCAnaLst = (CAnalysisList *)GetProp(hWndMain, CANALST_CLASS);
		if(!lpCAnaLst) throw (const int)STATE_NEW;
		// 設定ファイル操作クラスインスタンス
		lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
		if(!lpCSetup) throw (const int)STATE_NEW;

		// 解析リストクリア
		lpCAnaLst->ClearList();
		// ソースの解析開始
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "解析してます...");
		OutputTabSetCursel(TAB_OUTPUT);
		if(lpCAnaCBL->GetSourceCount() < 1){
			EXCMSG(hWndMain, "解析するソースの指定がありません");
			AddOutputMessage(hWndMain, "ソースファイルの指定がありません\n");
			throw (const int)STATE_NEW;
		}
		lpCAnaLst->SetDataPositionBase(lpCSetup->m_CoreAnalysysProp.m_nCompPos);
		if(!lpCAnaCBL->Analysys(lpCSetup->m_CoreAnalysysProp.m_nSeq,
			lpCSetup->m_CoreAnalysysProp.m_nMaxLine,
			lpCSetup->m_CoreAnalysysProp.m_bHostMode,
			lpCSetup->m_CoreAnalysysProp.m_szSign,
			lpCSetup->m_CoreAnalysysProp.m_nCompPos)){
			EXCMSG(hWndMain, "ソースコードの解析に失敗しました");
			AddOutputMessage(hWndMain, "ソースコードの解析に失敗しました\n");
			throw (const int)STATE_NEW;
		}
		// 解析結果
		char szTraceMsg[1024];
		wsprintf(szTraceMsg,
			"構文解析 %dm/s 意味解析 %dm/s\n"
			"解析したﾚｺｰﾄﾞの合計ｻｲｽﾞは %d Byte、解析完了です\nデータファイルを読み込むことができます",
			lpCAnaCBL->GetLexTime(), lpCAnaCBL->GetParserTime(),
			lpCAnaCBL->GetTotal());
		AddOutputMessage(hWndMain, szTraceMsg);
		// ステータスバー
		char szStatus[260];
		wsprintf(szStatus,
			" ﾚｺｰﾄﾞｻｲｽﾞ %d Byte",
			lpCAnaCBL->GetTotal());
		lpCStatus->SetSbText(1, 0, szStatus);
		wsprintf(szStatus,
			" 解析時間 %d m/s",
			lpCAnaCBL->GetLexTime() + lpCAnaCBL->GetParserTime());
		lpCStatus->SetSbText(2, 0, szStatus);
		// リストに設定
		lpCAnaLst->InitListItem(lpCAnaCBL->Get_AllRecords(),
			lpCAnaCBL->GetMaxLevelNo(), lpCAnaCBL->GetMinLevelNo());
		// 指定レベルまで自動展開
		if(lpCSetup->m_AnalysysProp.m_nDefExpandLevel > 1){
			lpCStatus->SetSbText(0, SBT_NOBORDERS, "ﾚｺｰﾄﾞ展開中...");
			lpCAnaLst->ExpandLevelItem(lpCSetup->m_AnalysysProp.m_nDefExpandLevel,
				lpCSetup->m_AnalysysProp.m_bNoExpandRed, 
				lpCSetup->m_AnalysysProp.m_bNoExpandOcc);
		}
		// ステータスメッセージ
		if(lpCSetup->m_CoreAnalysysProp.m_bUseCodeCheck){
			int nCodeCheckCount = lpCAnaCBL->GetCodeCheckCount();
			wsprintf(szStatus,
				" 解析完了 (ソースコードエラー検出 %d件)", nCodeCheckCount);
			lpCStatus->SetSbText(0, SBT_NOBORDERS, szStatus);
		}else{
			lpCStatus->SetSbText(0, SBT_NOBORDERS, "解析完了");
		}
		// 終了メッセージ
		PostMessage(hWndMain, UM_FINISHANACBL, (WPARAM)STATE_ANALYSYS, 0L);
	}
	catch(const int nErrState){
		lpCStatus->SetSbText(0, SBT_NOBORDERS, "解析失敗です");
		// 終了メッセージ
		PostMessage(hWndMain, UM_FINISHANACBL, (WPARAM)nErrState, 0L);
	}

	return 0;
}

/*----------------------------------------------------------------------------------------
	解析してます...
----------------------------------------------------------------------------------------*/
LRESULT OnAnalising(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CAnaCBL *lpCAnaCBL = NULL;
	CAnalysisList *lpCAnaLst = NULL;
	CYamaSetup *lpCSetup = NULL;

	switch(msg){
	case UM_BEGINANACBL:
	case UM_FINISHANACBL:
		// ＣＯＢＯＬ解析クラスインスタンス
		lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
		if(!lpCAnaCBL) return (0L);
		// 解析結果リスト制御クラスインスタンス
		lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
		if(!lpCAnaLst) return (0L);
		// 設定ファイル操作クラスインスタンス
		lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
		if(!lpCSetup) return (0L);

		// コントロールの状態変更
		SetControlState(hWnd, (const int)wp);

		// スレッド開始時の処理
		if(msg == UM_BEGINANACBL){
			// 履歴の更新とタイトルバーにソース情報設定
			int nHisCount = 0;
			HISDATA stHisData;
			ZeroMemory(&stHisData, sizeof(HISDATA));
			char szWndTitle[1024 + 256], szTmp[1024 + 256];
//			strcpy(szWndTitle, APP_NAME);
//			strcpy(szTmp, "(ﾟДﾟ)ｙ－~");
			strcpy_s(szWndTitle, sizeof(szWndTitle), APP_NAME);
			strcpy_s(szTmp, sizeof(szTmp), "(none))");
			SOURCEINFO SrcInfo;
			int nSrcCount = lpCAnaCBL->GetSourceCount();
			for(int i = 0; i < nSrcCount; i++){
				if(lpCAnaCBL->GetSourceData(i, &SrcInfo) < 0) break;
				if(i == 0){
					szTmp[0] = '\0';
				}else{
//					strcat(szTmp, ",");
					strcat_s(szTmp, sizeof(szTmp), ",");
				}
				char szVal[MAX_PATH];
				switch((int)SrcInfo.lpParam){
				case SRC_TYPE_CBL:
					GetFileTitle(SrcInfo.szSourcePath, szVal, sizeof(szVal));
					if(nHisCount < MAX_HISCOUNT){
//						strcpy(stHisData.szSource[nHisCount], SrcInfo.szSourcePath);
						strcpy_s(stHisData.szSource[nHisCount], sizeof(stHisData.szSource[nHisCount]), SrcInfo.szSourcePath);
						nHisCount++;
					}
					break;
				case SRC_TYPE_SMD:
					GetFileTitle(SrcInfo.szParam, szVal, sizeof(szVal));
					if(nHisCount < MAX_HISCOUNT){
//						strcpy(stHisData.szSource[nHisCount], SrcInfo.szParam);
						strcpy_s(stHisData.szSource[nHisCount], sizeof(stHisData.szSource[nHisCount]), SrcInfo.szParam);
						nHisCount++;
					}
					break;
				case SRC_TYPE_EDIT:
//					strcpy(szVal, "(edit code)");
					strcpy_s(szVal, sizeof(szVal), "(edit code)");
					break;
				default:
					szVal[0] = '\0';
					break;
				}
//				strcat(szTmp, szVal);
				strcat_s(szTmp, sizeof(szTmp), szVal);
				if(strlen(szTmp) > 1024){
//					strcat(szTmp, "...");
					strcat_s(szTmp, sizeof(szTmp), "...");
					break;
				}
			}
			wsprintf(szWndTitle, APP_TITLE, szTmp);
			SetWindowText(hWnd, szWndTitle);
			// 履歴更新
			if(nHisCount > 0){
				stHisData.nSource = nHisCount;
				lpCSetup->SetHistory(&stHisData);
				// 履歴メニューの再作成
				CreateHistroyMenu(hWnd);
			}
			break;
		}

		// スレッド停止時の処理
		if(ghAnaCBLThread){
			DWORD dwExitCode = 0;
			int nLoop = 0;
			do{
				if(nLoop > 1000){
					ERRMSG(hWnd, "解析スレッドが止まりません...強制停止します");
					TerminateThread(ghAnaCBLThread, 1);
					break;
				}
				nLoop++;
				Sleep(100);
				GetExitCodeThread(ghAnaCBLThread, &dwExitCode);
			} while(dwExitCode == STILL_ACTIVE);
			CloseHandle(ghAnaCBLThread);
			ghAnaCBLThread = NULL;
			gdwAnaCBLThreadID = 0;
		}
		// フォーカスセット
		lpCAnaLst->SetFocusListView();
		// 解析結果の先頭を選択状態に
        if(lpCAnaLst->GetAllCount() > 0) lpCAnaLst->SetSel(0);
		break;
	default:
		return (0L);
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソース解析結果の保存
----------------------------------------------------------------------------------------*/
BOOL OnExportAnalysys(HWND hWndMain, WORD wCmd)
{
	// ステータスバー制御クラスインスタンス
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWndMain, CPROGSTBAR_CLASS);
	if(!lpCStatus) return FALSE;
	switch(wCmd){
	case IDM_SAVE_HTML:
		return (BOOL)(ExportAnalysysHTML(hWndMain) >= 0);
	case IDM_SAVE_CSV:
		return (BOOL)(ExportAnalysysCSV(hWndMain) >= 0);
	case IDM_SAVE_CBL:
		return (BOOL)(ExportAnalysysCBL(hWndMain) >= 0);
	}
	return FALSE;
}

/*----------------------------------------------------------------------------------------
	ソース解析履歴を削除
----------------------------------------------------------------------------------------*/
LRESULT OnDeleteHistoryAnalysys(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return (0L);
	lpCSetup->RemoveHistory();
	CreateHistroyMenu(hWnd);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ソース解析履歴を開く
----------------------------------------------------------------------------------------*/
BOOL OnHistoryAnalysys(HWND hWndMain, UINT uiID)
{
	if(uiID < IDM_HISTORY) return FALSE;
	if(uiID > IDM_HISTORY + 20) return FALSE;

	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWndMain, CSETUP_CLASS);
	if(!lpCSetup) return FALSE;

	HMENU hMenu = GetMenu(hWndMain);
	HMENU hFileMenu = GetSubMenu(hMenu, MENU_POS_FILE);
	HMENU hHisMenu = GetSubMenu(hFileMenu, MENU_FILE_POS_HIS);
	if(!hHisMenu) return FALSE;

	MENUITEMINFO mi;
	ZeroMemory(&mi, sizeof(MENUITEMINFO));
	mi.cbSize = sizeof (MENUITEMINFO);
	mi.fMask = MIIM_DATA;
	GetMenuItemInfo(hHisMenu, uiID, FALSE, &mi);
	LPHISDATA lpHisData = lpCSetup->GetHistory((int)mi.dwItemData);
	if(lpHisData == NULL) return FALSE;

	// ソースを追加
	for (int i = 0; i < lpHisData->nSource; i++) {
		AddSourceFile(hWndMain, lpHisData->szSource[i], FALSE);
	}
	UpdateSourceMessage(hWndMain, TRUE);

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	設定画面の表示
----------------------------------------------------------------------------------------*/
LRESULT OnShowSetupPage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return (0L);
	// 設定ファイル操作クラスインスタンス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return (0L);
	// 分割ウィンドウ操作クラスインスタンス
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(!lpCWndSplit) return (0L);
	HWND hWndSplit = lpCWndSplit->GetWindowHandle();
	if(!hWndSplit) return (0L);
/*	// 作業フォルダ再作成の可否
	if(SourcesListCount(hWndSplit) > 0){
		lpCSetup->m_PathProp.m_bEnableTmpPath = FALSE;
	}else{
		lpCSetup->m_PathProp.m_bEnableTmpPath = TRUE;
	}*/
	// 設定画面の表示
	if(!ShowSetupPage(hWnd, lpCSetup)) return (0L);
	// リスト描画変更
	lpCAnaLst->SetColorFore(lpCSetup->m_UIProp.m_colFore);
	lpCAnaLst->SetColorBack(lpCSetup->m_UIProp.m_colBack);
	lpCAnaLst->SetColorStruct(lpCSetup->m_UIProp.m_colStruct);
	lpCAnaLst->SetColorOccurs(lpCSetup->m_UIProp.m_colOccurs);
	lpCAnaLst->SetColorRedefines(lpCSetup->m_UIProp.m_colRedefines);
	lpCAnaLst->SetColorBinary(lpCSetup->m_UIProp.m_colBinary);
	lpCAnaLst->SetColorSupplementation(lpCSetup->m_UIProp.m_colSupplement);
	lpCAnaLst->SetColorDataColumn(lpCSetup->m_UIProp.m_colDatCol);
	lpCAnaLst->SetAnalstFont(lpCSetup->m_UIProp.m_nFontSize, lpCSetup->m_UIProp.m_szFontFace);
	lpCAnaLst->SetColorUse(lpCSetup->m_UIProp.m_bUseBackColor);
	// リストアイテムイメージ
	lpCAnaLst->SetItemImageUse(lpCSetup->m_UIProp.m_bItemImage);
/*	// テンポラリディレクトリ再作成
	if(lpCSetup->m_PathProp.m_bEnableTmpPath){
		if(strlen(gszTmpDir) > 0) RemoveTempDir(gszTmpDir);
		if(!MakeTempDir(hWnd, lpCSetup->m_PathProp.m_szTmpDir))
			ERRMSG(hWnd, "テンポラリディレクトリの作成に失敗");
	}*/
	SetFontOutputWnd(hWndSplit);
	InvalidateRect(hWndSplit, NULL, TRUE);

	return (0L);
}

/*----------------------------------------------------------------------------------------
	設定確認画面の表示
----------------------------------------------------------------------------------------*/
LRESULT OnShowConfigBox(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	ShowConfigBox(hWnd, FALSE);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	メニューハンドル取得
----------------------------------------------------------------------------------------*/
HMENU GetMenuHandle(HWND hWnd, DWORD dwMenuPos)
{
	HMENU hMenu = GetMenu(hWnd);
	return (GetSubMenu(hMenu, dwMenuPos));
}

HMENU GetSubMenuHandle(HWND hWnd)
{
//	return (LoadMenu((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	return (LoadMenu((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(IDR_MENU_SUB)));
}

/*----------------------------------------------------------------------------------------
	バージョン情報
----------------------------------------------------------------------------------------*/
LRESULT OnVersionInfo(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	char szVersionInfo[1024];
	AddOutputMessage(hWnd, "===============================================\n");
	LoadString((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		IDS_VERSIONINFO,
		szVersionInfo, (int)sizeof(szVersionInfo));
	AddOutputMessage(hWnd, szVersionInfo);
	AddOutputMessage(hWnd,
		"===============================================",
		TRACE_ANA);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	README.txtの参照
----------------------------------------------------------------------------------------*/
/*
LRESULT OnOpenReadMe(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	if(!lpCSetup) return (0L);
	char szReadme[MAX_PATH+1];
	lpCSetup->GetWorkDir(szReadme, sizeof(szReadme));
//	strcat(szReadme, "\\Readme.txt");
	strcat_s(szReadme, sizeof(szReadme), "\\Readme.txt");
	AddOutputMessageFromFile(hWnd, szReadme);
	return (0L);
}
*/

/*----------------------------------------------------------------------------------------
	ウィンドウアクティブ状態変更プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnActivateWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HWND hWndFocus = NULL;
	if(wp == WA_INACTIVE){
		hWndFocus = GetFocus();
	}else{
		if(!hWndFocus){
			// 解析結果リストクラス
			CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
			if(!lpCAnaLst) return (0L);
			lpCAnaLst->SetFocusListView();
		}else{
			// フォーカス復元(オーナードローリストビューはフォーカスが戻らない？？)
			SetFocus(hWndFocus);
		}
	}
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ウィンドウサイズ変更プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnSize(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ダイアログバークラス
	CDlgbar *lpCDlgbar = (CDlgbar *)GetProp(hWnd, CDLGBAR_CLASS);
	if(lpCDlgbar) lpCDlgbar->Resize(hWnd, msg, wp, lp);
	// ステータスバークラス
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	if(lpCStatus) lpCStatus->Resize(hWnd, msg, wp, lp);
	// 可変ウィンドウクラス
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
	if(lpCWndSplit) lpCWndSplit->Resize(hWnd, msg, wp, lp);
//	CWndSplitter *lpCWndSplit2 = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS2);
//	if(lpCWndSplit2) lpCWndSplit2->Resize(hWnd, msg, wp, lp);
	// 解析結果リストクラス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	// リストビューサイズ変更
//	ResizeAnaLst(hWnd, lpCDlgbar, lpCStatus, lpCWndSplit, lpCWndSplit2, lpCAnaLst);
	ResizeAnaLst(hWnd, lpCDlgbar, lpCStatus, lpCWndSplit, lpCAnaLst);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ペインウィンドウサイズ変更プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnSizePain(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 解析結果リストのみサイズ変更
	CDlgbar *lpCDlgbar = (CDlgbar *)GetProp(hWnd, CDLGBAR_CLASS);
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
//	CWndSplitter *lpCWndSplit2 = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS2);
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
//	ResizeAnaLst(hWnd, lpCDlgbar, lpCStatus, lpCWndSplit, lpCWndSplit2, lpCAnaLst);
	ResizeAnaLst(hWnd, lpCDlgbar, lpCStatus, lpCWndSplit, lpCAnaLst);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	リストビューサイズ変更
----------------------------------------------------------------------------------------*/
void ResizeAnaLst(HWND hWnd,
				  CDlgbar *lpCDlgbar, 
				  CProgStbar *lpCStatus,
				  CWndSplitter *lpCWndSplit,
//				  CWndSplitter *lpCWndSplit2,
				  CAnalysisList *lpCAnaLst)
{
	if(!hWnd) return;
	if(!lpCDlgbar) return;
	if(!lpCStatus) return;
	if(!lpCWndSplit) return;
//	if(!lpCWndSplit2) return;
	if(!lpCAnaLst) return;
	CWinTheme *lpCWinTheme = (CWinTheme *)GetProp(hWnd, CWINTHEME_CLASS);
	RECT rc, rcDlgbar, rcStbar, rcSplit, /*rcSplit2,*/ rcAnaLst;
	GetClientRect(hWnd, &rc);
	// Themeが有効時には少しスマートにする
	if(lpCWinTheme){
		if(lpCWinTheme->IsThemeAct()){
			rc.left += GetSystemMetrics(SM_CXEDGE);
			rc.right -= GetSystemMetrics(SM_CXEDGE);
		}
	}
	lpCDlgbar->GetRect(&rcDlgbar);
	lpCStatus->GetRect(&rcStbar);
	lpCWndSplit->GetRectSize(&rcSplit, SIZE_CLIENT);
//	lpCWndSplit2->GetRectSize(&rcSplit2, SIZE_CLIENT);
	SetRect(&rcAnaLst,
		rc.left,
		(rcDlgbar.bottom - rcDlgbar.top),
//		rc.right - (rcSplit2.right - rcSplit2.left),
		rc.right,
		rc.bottom - ((rcStbar.bottom - rcStbar.top) +
		(rcSplit.bottom - rcSplit.top) + (GetSystemMetrics(SM_CYEDGE) * 3)));
	lpCAnaLst->Move(&rcAnaLst);
//	// 第二ペインもサイズ変更
//	lpCWndSplit2->SetPainSizeEx(rcAnaLst.bottom - rcAnaLst.top);
}

/*----------------------------------------------------------------------------------------
	ウィンドウ閉じるプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnClose(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(ghAnaCBLThread){
		DWORD dwExitCode = 0;
		GetExitCodeThread(ghAnaCBLThread, &dwExitCode);
		if(dwExitCode == STILL_ACTIVE){
			if(YESNOMSG(hWnd, "現在解析中です、強制的終了しますか？") == IDYES){
				// 解析ﾁｭｳｼ
				CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
				lpCAnaCBL->CancelAnalysys();
				// 展開ﾁｭｳｼ
				CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
				lpCAnaLst->CancelExpand();
				// スレッド停止待ち
				for(int i = 0 ; i < 10; i++){
					Sleep(300);
					GetExitCodeThread(ghAnaCBLThread, &dwExitCode);
					if(dwExitCode != STILL_ACTIVE) break;
				}
				if(dwExitCode == STILL_ACTIVE){
					AddOutputMessage(hWnd, "解析ｽﾚｯﾄﾞ強制停止");
					TerminateThread(ghAnaCBLThread, 0);
				}
				CloseHandle(ghAnaCBLThread);
				ghAnaCBLThread = NULL;
				gdwAnaCBLThreadID = 0;
				Sleep(2000);
			}else{
				return (0L);
			}
		}
	}
	DestroyWindow(hWnd);
	return (0L);
}

/*----------------------------------------------------------------------------------------
	ウィンドウ破棄プロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CDataFile* lpCDataFile = (CDataFile*)GetProp(hWnd, CDATAFILE_CLASS);
	CWinTheme *lpCWinTheme = (CWinTheme *)GetProp(hWnd, CWINTHEME_CLASS);
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(hWnd, CSETUP_CLASS);
	CDlgbar *lpCDlgbar = (CDlgbar *)GetProp(hWnd, CDLGBAR_CLASS);
	CProgStbar *lpCStatus = (CProgStbar *)GetProp(hWnd, CPROGSTBAR_CLASS);
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWnd, CANACBL_CLASS);
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS);
//	CWndSplitter *lpCWndSplit2 = (CWndSplitter *)GetProp(hWnd, CSPLITWND_CLASS2);
	// データクラスクラスインスタンス破棄
	if (lpCDataFile) {
		delete lpCDataFile;
		RemoveProp(hWnd, CDATAFILE_CLASS);
	}
	// テーマ確認クラスインスタンス破棄
	if(lpCWinTheme){
		delete lpCWinTheme;
		RemoveProp(hWnd, CWINTHEME_CLASS);
	}
	// 設定ファイル操作クラスインスタンス破棄
	if(lpCSetup){
		// ウィンドウ位置サイズを記録する
		if(!(lpCSetup->m_UIProp.m_bMainWndMaximize = IsZoomed(hWnd))){
			if(!IsIconic(hWnd)) GetWindowRect(hWnd, &(lpCSetup->m_UIProp.m_rcMainWnd));
		}
		// アウトプットウィンドウの可視状態
		if(SW_HIDE == lpCWndSplit->GetShow()){
			lpCSetup->m_UIProp.m_bShowOutputWnd = FALSE;
		}else{
			lpCSetup->m_UIProp.m_bShowOutputWnd = TRUE;
		}
		// アウトプットウィンドウの大きさ
		lpCSetup->m_UIProp.m_nOutPutWndSize = lpCWndSplit->GetPainSize();
//		// レコード選択ウィンドウの可視状態
//		if(SW_HIDE == lpCWndSplit2->GetShow()){
//			lpCSetup->m_UIProp.m_bShowRecSelWnd = FALSE;
//		}else{
//			lpCSetup->m_UIProp.m_bShowRecSelWnd = TRUE;
//		}
//		// レコード選択ウィンドウの大きさ
//		lpCSetup->m_UIProp.m_nRecSelWndSize = lpCWndSplit2->GetPainSize();
		// 記録
		lpCSetup->WriteSetup();
		delete lpCSetup;
		RemoveProp(hWnd, CSETUP_CLASS);
	}
	// ダイアログバー制御クラスインスタンス破棄
	if(lpCDlgbar){
		delete lpCDlgbar;
		RemoveProp(hWnd, CDLGBAR_CLASS);
	}
	// ステータスバー制御クラスインスタンス破棄
	if(lpCStatus){
		delete lpCStatus;
		RemoveProp(hWnd, CPROGSTBAR_CLASS);
	}
	// 解析結果リスト制御クラスインスタンス破棄
	if(lpCAnaLst){
		delete lpCAnaLst;
		RemoveProp(hWnd, CANALST_CLASS);
	}
	// 可変ウィンドウ制御クラスインスタンス破棄
	if(lpCWndSplit){
		delete lpCWndSplit;
		RemoveProp(hWnd, CSPLITWND_CLASS);
	}
//	if(lpCWndSplit2){
//		delete lpCWndSplit2;
//		RemoveProp(hWnd, CSPLITWND_CLASS2);
//	}
	// ＣＯＢＯＬ解析クラスインスタンス破棄
	if(lpCAnaCBL){
		delete lpCAnaCBL;
		RemoveProp(hWnd, CANACBL_CLASS);
	}
	// テンポラリディレクトリの削除
	RemoveTempDir(gszTmpDir);
	// メッセージループ終了
	PostQuitMessage(0);
	return (0L);
}

/*------------------------------------------------------------------------------
	ライセンス関連
------------------------------------------------------------------------------*/
/*
int CheckLicense()
{
	int nRet = 0;
	// ライセンス確認
	HPAUTHSTRUCT stHpAuth;
	ZeroMemory(&stHpAuth, sizeof(HPAUTHSTRUCT));
	strcpy(stHpAuth.szHopeVer, APP_HOPE_VER);
	strcpy(stHpAuth.szAppName, APP_NAME);
	stHpAuth.wVerMejor = APP_VER_MEJOR;
	stHpAuth.wVerMinor = APP_VER_MINOR;
	stHpAuth.wVerBuild = APP_VER_BUILD;
	stHpAuth.wVerRivision = APP_VER_REVISION;
	strcpy(stHpAuth.szProduct, APP_COPY);
	strcpy(stHpAuth.szFullProduct, APP_COPYRIGHT);
	strcpy(stHpAuth.szWndClass, APP_CLASS);
	// DLL初期化
	if((nRet = AuthInitialize(&stHpAuth, FALSE, "ユーザＩＤ", "ユーザ名")) != HPAUTH_INITOK){
		AuthMessageBox(HWND_DESKTOP, nRet);
		throw "認証機能の初期化に失敗";
	}
	// 認証確認
	if(AuthGetCheck(&stHpAuth) != HPAUTH_AUTHCHECKOK){
		// 認証ダイアログ
		if((nRet = AuthDialog(HWND_DESKTOP)) != HPAUTH_AUTHOK){
			AuthMessageBox(HWND_DESKTOP, nRet);
			MessageBox(HWND_DESKTOP,
				"アプリケーション認証が完了できてないみたいですが、\r\n"
				"実験的に付加した機能なので「OK」でこのまま起動できます\r\n"
				"どうしてもウザイって方は以下のコードで認証してください(´･ω･`)\r\n"
				"ユーザＩＤ ：9999999999\r\n"
				"ユーザ名   ：山本\r\n"
				"認証ﾊﾟｽﾜｰﾄﾞ：2B462C3960\r\n"
				"\r\n",
				"- お知らせ -",
				MB_ICONINFORMATION | MB_SYSTEMMODAL);
		}else{
			AuthMessageBox(HWND_DESKTOP, nRet);
		}
	}
	return 0;
}
*/
/*
void AboutLicense(HWND hWnd, UINT uiCmd)
{
	int nRet = 0;
	switch(uiCmd){
	case IDM_LICENCE:
		AuthAboutDlg(hWnd, "");
		break;
	case IDM_REGIST:
		nRet = AuthDialog(hWnd);
		AuthMessageBox(hWnd, nRet);
		break;
	}
}
/*
/*------------------------------------------------------------------------------
	ヘルプ
------------------------------------------------------------------------------*/
/*
void ShowHelp(HWND hWnd)
{
	char szHelpFile[MAX_PATH];
	char *p, *r;
	GetModuleFileName(NULL, szHelpFile, (DWORD)sizeof(szHelpFile));
	for(r = p = szHelpFile; *p != '\0'; p++){
		if(IsDBCSLeadByte(*p) == TRUE){
			p++;
			continue;
		}
		if(*p == '\\') r = p;
	}
	*r = '\0';
//	strcat(szHelpFile, "\\");
//	strcat(szHelpFile, HELP_FILE);
	strcat_s(szHelpFile, sizeof(szHelpFile), "\\");
	strcat_s(szHelpFile, sizeof(szHelpFile), HELP_FILE);
	ShellExecute(hWnd, "open", szHelpFile, NULL, NULL, SW_SHOWNORMAL);
}
*/
LRESULT OpenDataFileDlg(HWND hWnd)
{
	CAnaCBL* lpCAnaCBL =
		(CAnaCBL*)GetProp(
			hWnd,
			CANACBL_CLASS);

	if (!lpCAnaCBL) {
		return -1;
	}

	// 解析結果が無ければ開けない
	DWORD dwRecordSize =
		lpCAnaCBL->GetTotal();

	if (dwRecordSize == 0) {
		EXCMSG(
			hWnd,
			"先にCOBOLソースを解析してください");
		return -1;
	}

	static char szFilePath[MAX_PATH] = "";

	if (!GetOpenFileDlg(
		hWnd,
		szFilePath,
		sizeof(szFilePath),
		"ﾃﾞｰﾀﾌｧｲﾙ (*.*)\0*.*\0",
		"ﾃﾞｰﾀﾌｧｲﾙを開く"))
	{
		return 0;
	}

	return (OpenDataFile(hWnd, szFilePath));
}

LRESULT OpenDataFile(HWND hWnd, LPCTSTR lpszFilePath)
{
	CDataFile* lpCDataFile =
		(CDataFile*)GetProp(
			hWnd,
			CDATAFILE_CLASS);

	if (!lpCDataFile) {
		return -1;
	}

	CAnaCBL* lpCAnaCBL =
		(CAnaCBL*)GetProp(
			hWnd,
			CANACBL_CLASS);

	if (!lpCAnaCBL) {
		return -1;
	}

	// 解析結果が無ければ開けない
	DWORD dwRecordSize =
		lpCAnaCBL->GetTotal();

	if (dwRecordSize == 0) {
		EXCMSG(
			hWnd,
			"データファイルを開くには、COBOLソースを解析しておく必要があります");
		return -1;
	}

	if (!lpCDataFile->Open(
		lpszFilePath,
		dwRecordSize))
	{
		EXCMSG(hWnd, "データファイルを開けません");
		return -1;
	}

	if (lpCDataFile->GetRecordCount() < 1) {
		EXCMSG(
			hWnd,
			"データファイルに1レコード分のデータがありません");
		lpCDataFile->Close();
		return -1;
	}

	if (!lpCDataFile->ReadRecord(0)) {
		EXCMSG(hWnd, "先頭レコードを読み込めません");
		lpCDataFile->Close();
		return -1;
	}

	if (lpCDataFile->GetRemainSize() != 0) {
		char szWarn[256];
		wsprintf(
			szWarn,
			"ファイルサイズがレコード長で割り切れません (%I64u Byte余り)",
			lpCDataFile->GetRemainSize());
		AddOutputMessage(
			hWnd,
			szWarn);
	}

	CAnalysisList* lpCAnaLst =(CAnalysisList*)GetProp(hWnd, CANALST_CLASS);
	if (lpCAnaLst) {
		lpCAnaLst->RefreshDataValues();
	}

	char szMsg[512];
	wsprintf(
		szMsg,
		"データファイルを開きました [%I64u件 / 1レコード %uByte]",
		lpCDataFile->GetRecordCount(),
		dwRecordSize);

	AddOutputMessage(hWnd, szMsg);

	SetControlState(
		hWnd,
		STATE_DATOPEN);

	ShowDataTab(
		hWnd,
		TRUE);

	UpdateDataPanel(hWnd);

	return 0;
}

LRESULT CloseDataFile(
	HWND hWnd)
{
	CDataFile* lpCDataFile =
		(CDataFile*)GetProp(
			hWnd,
			CDATAFILE_CLASS);

	if (!lpCDataFile) {
		return -1;
	}

	lpCDataFile->Close();

	CAnalysisList* lpCAnaLst =
		(CAnalysisList*)GetProp(
			hWnd,
			CANALST_CLASS);

	if (lpCAnaLst) {
		lpCAnaLst->RefreshDataValues();
	}
	char szMsg[512];
	strcpy_s(szMsg, sizeof(szMsg), "データファイルを閉じました");

	AddOutputMessage(
		hWnd,
		szMsg);

	ShowDataTab(
		hWnd,
		FALSE);

	return 0;
}

LRESULT CALLBACK JudgeDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG:
		{
			const char* pszFilePath = (const char*)lp;
			SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pszFilePath);

			const char* pszName = strrchr(pszFilePath, '\\');
			SetDlgItemText(hDlg, IDC_ST_FILE, pszName ? pszName + 1 : pszFilePath);

			WIN32_FILE_ATTRIBUTE_DATA fad = {};
			char szFileSize[64] = "-";
			char szLastUpdate[32] = "-";
			if (GetFileAttributesEx(pszFilePath, GetFileExInfoStandard, &fad))
			{
				ULARGE_INTEGER uli;
				uli.HighPart = fad.nFileSizeHigh;
				uli.LowPart = fad.nFileSizeLow;
				sprintf_s(szFileSize, "%llu Byte", uli.QuadPart);
				SetDlgItemText(hDlg, IDC_ST_SIZE, szFileSize);

				SYSTEMTIME st;
				FileTimeToSystemTime(&(fad.ftLastWriteTime), &st);
				sprintf_s(szLastUpdate, "%d年%d月%d日 %d時%d分%d秒", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
				SetDlgItemText(hDlg, IDC_ST_UPDATE, szLastUpdate);
			}

			const char* pszExt = strrchr(pszFilePath, '.');
			if (pszExt) {
				SetDlgItemText(hDlg, IDC_ST_EXT, pszExt);
			}
			else {
				SetDlgItemText(hDlg, IDC_ST_EXT, "(none)");
			}

		}
		break;
	case WM_PAINT:
		return (OnPaintJugeDlg(hDlg, msg, wp, lp));
	case WM_DRAWITEM:
		return (OnDrawJugeBtn(hDlg, msg, wp, lp));
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BTN_SRC:
			EndDialog(hDlg, IDC_BTN_SRC);
			break;
		case IDC_BTN_DAT:
			EndDialog(hDlg, IDC_BTN_DAT);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		default:
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT OnPaintJugeDlg(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	CYamaSetup* lpCSetup = (CYamaSetup*)GetProp(GetParent(hDlg), CSETUP_CLASS);
	if (!lpCSetup) return FALSE;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hDlg, &ps);

	RECT rc;
	GetClientRect(hDlg, &rc);

	const char* pszFilePath = (const char*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	if (pszFilePath) {
		HICON hIcon = GetFileIcon48(pszFilePath);
		if (hIcon) {
			DrawIconEx(ps.hdc, 28, 104, hIcon, 48, 48, 0, NULL, DI_NORMAL);
			DestroyIcon(hIcon);
		}
	}

	CImgListPng ilPng;
	HBITMAP hBmp =ilPng.LoadPngResource((HINSTANCE)GetWindowLongPtr(GetParent(hDlg), GWLP_HINSTANCE), IDB_PNG_JUDGE);
	if (!hBmp) {
		EndPaint(hDlg, &ps);
		return FALSE;
	}
	HIMAGELIST hImgList =ilPng.MakeImageListPng(56, hBmp);
	DeleteObject(hBmp);

	ImageList_Draw(hImgList, 2, ps.hdc, 16, 12, ILD_TRANSPARENT);

	RECT rcText = { 86, 14, rc.right, rc.bottom };
	HFONT hFont = MAKEFONT(ps.hdc, 12, FW_NORMAL, FALSE, lpCSetup->m_UIProp.m_szFontFace);
	HFONT hOldFont = (HFONT)SelectObject(ps.hdc, hFont);
	TEXTMETRIC tm;
	GetTextMetrics(ps.hdc, &tm);
	SetTextColor(ps.hdc, RGB(1, 1, 139));
	SetBkMode(ps.hdc, TRANSPARENT);
	DrawText(ps.hdc, "ドロップされたファイルの種別を判定できませんでした", -1, &rcText, DT_SINGLELINE | DT_END_ELLIPSIS);
	SelectObject(ps.hdc, hOldFont);
	DeleteObject(hFont);
	hFont = MAKEFONT(ps.hdc, 10, FW_NORMAL, FALSE, lpCSetup->m_UIProp.m_szFontFace);
	SelectObject(ps.hdc, hFont);
	rcText.top += tm.tmHeight + 4;
	SetTextColor(ps.hdc, RGB(1, 1, 1));
	DrawText(ps.hdc, "このファイルをどの種類として開きますか？", -1, &rcText, DT_SINGLELINE | DT_END_ELLIPSIS);
	SelectObject(ps.hdc, hOldFont);
	DeleteObject(hFont);

	ImageList_Destroy(hImgList);

	EndPaint(hDlg, &ps);
	return TRUE;
}

HICON GetFileIcon48(const char* pszFilePath)
{
	SHFILEINFO sfi = {};

	// まずシステムイメージリスト内のインデックス取得
	if (!SHGetFileInfo(
		pszFilePath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(sfi),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES))
	{
		return NULL;
	}

	IImageList* pImageList = nullptr;

	HRESULT hr = SHGetImageList(
		SHIL_EXTRALARGE,   // だいたい48x48
		IID_IImageList,
		(void**)&pImageList);

	if (FAILED(hr) || !pImageList)
		return NULL;

	HICON hIcon = NULL;

	hr = pImageList->GetIcon(
		sfi.iIcon,
		ILD_TRANSPARENT,
		&hIcon);

	pImageList->Release();

	if (FAILED(hr))
		return NULL;

	return hIcon;
}

LRESULT OnDrawJugeBtn(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	CYamaSetup* lpCSetup = (CYamaSetup*)GetProp(GetParent(hDlg), CSETUP_CLASS);
	if (!lpCSetup) return FALSE;

	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lp;

	BOOL bPressed = (lpdis->itemState & ODS_SELECTED) != 0;

	char szCaption[64], szComment[128];

	int nImg;
	switch (lpdis->CtlID)
	{
	case IDC_BTN_SRC:
		nImg = 0;
		strcpy_s(szCaption, sizeof(szCaption), "COBOLソースとして開く(&C)");
		strcpy_s(szComment, sizeof(szComment), "COBOLソース / Copybook として解析対象に追加します\n"
			".cbl や .cob などのソースファイルの場合はこちらを選択してください");
		break;
	case IDC_BTN_DAT:
		nImg = 1;
		strcpy_s(szCaption, sizeof(szCaption), "データファイルとして開く(&D)");
		strcpy_s(szComment, sizeof(szComment), "現在の解析結果をレコードレイアウトとして固定長データを読み込みます\n"
				".dat などのデータファイルの場合はこちらを選択してください");
		break;
	default:
		return FALSE;
	}

	HDC hDC = lpdis->hDC;
	RECT rc = lpdis->rcItem;

	COLORREF colBack;
	COLORREF colBorder;
	COLORREF colText;

	if (bPressed) {

		colBack =
			RGB(225, 230, 240);

		colBorder =
			RGB(80, 100, 140);
	}
	else {

		colBack =
			GetSysColor(COLOR_WINDOW);

		colBorder =
			RGB(160, 160, 160);
	}

	HBRUSH hBrush = CreateSolidBrush(colBack);
	HPEN hPen = CreatePen(PS_SOLID, 1, colBorder);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 5, 5);
	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	CImgListPng ilPng;
	HBITMAP hBmp = ilPng.LoadPngResource((HINSTANCE)GetWindowLongPtr(GetParent(hDlg), GWLP_HINSTANCE), IDB_PNG_JUDGE);
	if (!hBmp) return FALSE;
	HIMAGELIST hImgList = ilPng.MakeImageListPng(56, hBmp);
	DeleteObject(hBmp);

	ImageList_Draw(hImgList, nImg, hDC, rc.left + 14, rc.top + 9, ILD_TRANSPARENT);

	RECT rcText = { rc.left + 88, rc.top + 7, rc.right, rc.bottom };
	HFONT hFont = MAKEFONT(hDC, 11, FW_NORMAL, FALSE, lpCSetup->m_UIProp.m_szFontFace);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	SetTextColor(hDC, RGB(1, 1, 139));
	SetBkMode(hDC, TRANSPARENT);
	DrawText(hDC, szCaption, -1, &rcText, DT_SINGLELINE | DT_END_ELLIPSIS);
	DeleteObject(hFont);
	hFont = MAKEFONT(hDC, 9, FW_NORMAL, FALSE, lpCSetup->m_UIProp.m_szFontFace);
	SelectObject(hDC, hFont);
	rcText.top += tm.tmHeight + 2;
	SetTextColor(hDC, RGB(1, 1, 1));
	DrawText(hDC, szComment, -1, &rcText, DT_WORDBREAK | DT_END_ELLIPSIS);
	DeleteObject(hFont);

	ImageList_Destroy(hImgList);

	return TRUE;
}
