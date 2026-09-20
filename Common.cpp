/*----------------------------------------------------------------------------------------

	Common.cpp

----------------------------------------------------------------------------------------*/
#include "Common.h"

/*----------------------------------------------------------------------------------------
	定義
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static BOOL gbInitCOM = FALSE;
static OSVERSIONINFO gOvi;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
int CALLBACK SHOpenFolderProc(HWND hWnd, UINT msg, LPARAM lp1, LPARAM lp2);

/*----------------------------------------------------------------------------------------
	COM初期化
----------------------------------------------------------------------------------------*/
BOOL InitCom()
{
	if(gbInitCOM) return TRUE;
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr)) return FALSE;
	gbInitCOM = TRUE;
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	COM解放
----------------------------------------------------------------------------------------*/
BOOL UninitCom()
{
	CoUninitialize();
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	コモンコントロールの初期化
----------------------------------------------------------------------------------------*/
void InitWin32ComCtrl(DWORD dwICC/* = ICC_WIN95_CLASSES*/)
{
//	InitCommonControls();
	INITCOMMONCONTROLSEX icex;
	ZeroMemory(&icex, sizeof(INITCOMMONCONTROLSEX));
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = dwICC;
	InitCommonControlsEx(&icex);
}

/*----------------------------------------------------------------------------------------
	WindowsOSのバージョン取得
----------------------------------------------------------------------------------------*/
//WORD GetVersionNumber()
//{
//	if(gOvi.dwOSVersionInfoSize == 0){
//		ZeroMemory(&gOvi, sizeof(OSVERSIONINFO));
//		gOvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//		GetVersionEx(&gOvi);
//	}
//	return (MAKEWORD(gOvi.dwMajorVersion, gOvi.dwMinorVersion));
//}

/*----------------------------------------------------------------------------------------
	イメージリスト作成
----------------------------------------------------------------------------------------*/
HIMAGELIST CreateImageList(HWND hWnd, UINT uiImgBmp, const int nImg, COLORREF colMask)
{
//	HBITMAP hBmp = (HBITMAP)LoadImage((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	HBITMAP hBmp = (HBITMAP)LoadImage((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
										MAKEINTRESOURCE(uiImgBmp),
										IMAGE_BITMAP,
										0, 0,
										LR_SHARED);
	if(!hBmp) return NULL;
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	HIMAGELIST hImgLst = ImageList_Create(bmp.bmWidth / nImg, bmp.bmHeight,
										ILC_COLOR16 | ILC_MASK, nImg, 0);
	ImageList_AddMasked(hImgLst, hBmp, colMask);
	DeleteObject(hBmp);
	return hImgLst;
}

/*------------------------------------------------------------------------------
	リストビューにイメージリスト設定
------------------------------------------------------------------------------*/
BOOL SetImageList2Listview(HWND hListView, UINT uiBmpRes,const int nImg,
						   const int nIconStyle/* = LVSIL_SMALL*/,
						   COLORREF colMask/* = RGB(255, 255, 255)*/)
{
	if(!hListView) return FALSE;
	// リストビューのイメージリストは自動破棄スタイルでないとリークします
//	if(GetWindowLong(hListView, GWL_STYLE) & LVS_SHAREIMAGELISTS) return FALSE;
	if(GetWindowLongPtr(hListView, GWL_STYLE) & LVS_SHAREIMAGELISTS) return FALSE;
	// イメージリスト作成
	HIMAGELIST hImgList = NULL;
	if(!(hImgList = CreateImageList(hListView, uiBmpRes, nImg, colMask))) return FALSE;
	// リストビューにセット
	ListView_SetImageList(hListView, hImgList, nIconStyle);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	コントロール作成
----------------------------------------------------------------------------------------*/
HWND CreateControl(HINSTANCE hInst, HWND hParent, WORD wID,
				   LPCTSTR lpcClass, LPCTSTR lpcTitle, DWORD dwStyle, DWORD dwExStyle/* = WS_EX_CLIENTEDGE*/,
				   int nX/* = 0*/, int nY/* = 0*/, int nWidth/* = 0*/, int nHeight/* = 0*/)
{
	HWND hWndCtrl = CreateWindowEx(dwExStyle,
				lpcClass,
				lpcTitle,
				WS_CHILD | WS_VISIBLE | dwStyle,
				nX, nY,
				nWidth, nHeight,
				hParent,
				(HMENU)wID,
				hInst,
				NULL);
	if(!hWndCtrl) return NULL;
	SendMessage(hWndCtrl, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)TRUE);
	return hWndCtrl;
}

/*----------------------------------------------------------------------------------------
	スピンコントロール作成
----------------------------------------------------------------------------------------*/
HWND CreateSpinCtrl(HINSTANCE hInst, HWND hParent, HWND hBody, int nMax, int nMin, WORD wID,
					DWORD dwStyle/* = CTRLSTYLE_SPIN*/)
{
	HWND hSpin = CreateUpDownControl(dwStyle,
		0, 0, 0, 0, 
		hParent,
		(int)wID,
		hInst,
		NULL,
		0, 0, 0);
	if(!hSpin) return NULL;
	// ボディは後付しないと、なんかおかしくなる
	SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)(HWND)hBody, 0L);
	SendMessage(hSpin, UDM_SETRANGE32, (WPARAM)nMin, (LPARAM)nMax);
	return hSpin;
}

/*----------------------------------------------------------------------------------------
	ファイルを開くダイアログ
----------------------------------------------------------------------------------------*/
BOOL GetOpenFileDlg(HWND hParent,
					char *pszFilePath,
					const int nLen,
					const char *pszFilter,
					const char *pszTitle,
					char *pszFileTitle/* = NULL*/,
					const char *pszDefExt/* = NULL*/,
					DWORD dwFlg/* = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_FILEMUSTEXIST*/)
{
	if(!pszFilePath) return FALSE;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hParent;
	ofn.lpstrFilter = pszFilter;
	ofn.lpstrFile = pszFilePath;
	ofn.nMaxFile = nLen;
	ofn.lpstrTitle = (strlen(pszTitle) > 0 ? pszTitle : NULL);
	ofn.Flags = dwFlg;
	ofn.lpstrDefExt = pszDefExt;
	return (GetOpenFileName(&ofn));
}

/*----------------------------------------------------------------------------------------
	ファイルを保存ダイアログ
----------------------------------------------------------------------------------------*/
BOOL GetSaveFileDlg(HWND hParent,
					char *pszFilePath,
					const int nLen,
					const char *pszFilter,
					const char *pszTitle,
					char *pszFileTitle/* = NULL*/,
					const char *pszDefExt/* = NULL*/,
					DWORD dwFlg/* = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/)
{
	if(!pszFilePath) return FALSE;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hParent;
	ofn.lpstrFilter = pszFilter;
	ofn.lpstrFile = pszFilePath;
	ofn.lpstrFileTitle = pszFileTitle;
	ofn.nMaxFile = nLen;
	ofn.lpstrTitle = (strlen(pszTitle) > 0 ? pszTitle : NULL);
	ofn.Flags = dwFlg;
	ofn.lpstrDefExt = pszDefExt;
	return (GetSaveFileName(&ofn));
}

/*----------------------------------------------------------------------------------------
	フォルダ選択ダイアログ
----------------------------------------------------------------------------------------*/
int GetSHFolderDlg(HWND hParent,
				   char *pszFolderPath,
				   const char *pszTitle,
				   BOOL bUseNewStyle,
				   BOOL bUseNewFolderButton,
				   UINT uiFlg/* = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT*/)
{
//	OSVERSIONINFO ovi;
//	ZeroMemory(&ovi, sizeof(OSVERSIONINFO));
//	ovi.dwOSVersionInfoSize = (DWORD)sizeof(OSVERSIONINFO);
//	GetVersionEx(&ovi);
//	if(ovi.dwMajorVersion > 4){
//		if(bUseNewStyle) uiFlg |= BIF_NEWDIALOGSTYLE;
//		if(!bUseNewFolderButton) uiFlg |= BIF_NONEWFOLDERBUTTON;
//	}
	if(bUseNewStyle) uiFlg |= BIF_NEWDIALOGSTYLE;
	if(!bUseNewFolderButton) uiFlg |= BIF_NONEWFOLDERBUTTON;
	BROWSEINFO bi;
	if(pszFolderPath[strlen(pszFolderPath) - 1] == '\\')
		pszFolderPath[strlen(pszFolderPath) - 1] = '\0';
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = hParent;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pszFolderPath;
	bi.ulFlags = uiFlg;
	bi.lpszTitle = pszTitle;
	bi.lpfn = &SHOpenFolderProc;
	bi.lParam = (LPARAM)pszFolderPath;
	bi.iImage = (int)NULL;
	ITEMIDLIST *lpid = SHBrowseForFolder(&bi);
	if(lpid == NULL){
		return FALSE;
	}else{
		LPMALLOC pMalloc = NULL;
		HRESULT hr = SHGetMalloc(&pMalloc);
		if(FAILED(hr)) return FALSE;
		SHGetPathFromIDList(lpid, pszFolderPath);
		pMalloc->Free(lpid);
		pMalloc->Release();
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	フォルダ選択ダイアログのコールバック
----------------------------------------------------------------------------------------*/
int CALLBACK SHOpenFolderProc(HWND hDlg, UINT msg, LPARAM lp1, LPARAM lp2)
{
	switch(msg){
	case BFFM_INITIALIZED:
		SendMessage(hDlg, BFFM_SETSELECTION, (WPARAM)TRUE, lp2);
		break;
	}
	return 0;
}

/*----------------------------------------------------------------------------------------
	シェルを使ったアイコンハンドル取得
	nFlg : SHGFI_LARGEICON | SHGFI_SMALLICON | SHGFI_LINKOVERLAY
----------------------------------------------------------------------------------------*/
HICON ShellGetIcon(char *pszPath, const int nFlg/* = SHGFI_LARGEICON*/)
{
	SHFILEINFO shfi;
	ZeroMemory(&shfi, sizeof(SHFILEINFO));
	SHGetFileInfo(pszPath, FILE_ATTRIBUTE_ARCHIVE, &shfi, sizeof(SHFILEINFO), SHGFI_ICON | nFlg);
	return (shfi.hIcon);
}

/*---------------------------------------------------------------------------------------------------------------
	GetFingerCursor
---------------------------------------------------------------------------------------------------------------*/
HCURSOR GetFingerCursor()
{
//	HCURSOR hCursor;
//	OSVERSIONINFO ovi;
//	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	GetVersionEx(&ovi);
//	if(ovi.dwPlatformId == VER_PLATFORM_WIN32_NT && ovi.dwMajorVersion >= 5){
//		hCursor = (HCURSOR)LoadImage(NULL,
//			MAKEINTRESOURCE(32649),
//			IMAGE_CURSOR,
//			0, 0,
//			LR_SHARED);
//	}else{
//		char szPath[MAX_PATH];
//		GetWindowsDirectory(szPath, MAX_PATH);
//		strcat(szPath, "\\winhlp32.exe");
//		HINSTANCE hInst = LoadLibrary(szPath);
//		if(hInst != NULL){
//			hCursor = (HCURSOR)LoadImage(hInst,
//				MAKEINTRESOURCE(106),
//				IMAGE_CURSOR,
//				0, 0,
//				LR_SHARED);
//			FreeLibrary(hInst);
//		}else{
//			hCursor = (HCURSOR)LoadImage(NULL,
//				MAKEINTRESOURCE(IDC_ARROW),
//				IMAGE_CURSOR,
//				0, 0,
//				LR_SHARED);
//		}
//	}
//	return hCursor;	
	return (HCURSOR)LoadImage(NULL,
			MAKEINTRESOURCE(32649),
			IMAGE_CURSOR,
			0, 0,
			LR_SHARED);
}

/*----------------------------------------------------------------------------------------
	色選択ダイアログ
----------------------------------------------------------------------------------------*/
BOOL GetColorDlg(HWND hParent, DWORD dwDefColor, DWORD *lpdwResultColor)
{
    static DWORD dwCustColors[16];
	dwCustColors[0] = dwDefColor;
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hParent;
	cc.lpCustColors = dwCustColors;
	cc.rgbResult = dwDefColor;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	if(ChooseColor(&cc)){
		*lpdwResultColor = cc.rgbResult;
		return TRUE;
	}
	return FALSE;
}

/*----------------------------------------------------------------------------------------
	色属性増加
----------------------------------------------------------------------------------------*/
COLORREF AddColorValue(COLORREF colBase, const int nAdd, const int nMax/* = 240*/, const int nMin/* = 15*/)
{
	int nRValue = max(min(GetRValue(colBase) + nAdd, nMax), nMin),
		nGValue = max(min(GetGValue(colBase) + nAdd, nMax), nMin),
		nBValue = max(min(GetBValue(colBase) + nAdd, nMax), nMin);
	return (RGB(nRValue, nGValue, nBValue));
}

/*----------------------------------------------------------------------------------------
	パスの比較
----------------------------------------------------------------------------------------*/
int IsSameDirectory(const char *pszPath1, const char *pszPath2)
{
	char szDir1[MAX_PATH+1], szDir2[MAX_PATH+1];
	if(!StripDirectory(pszPath1, szDir1, sizeof(szDir1))) return 0;
	if(!StripDirectory(pszPath2, szDir2, sizeof(szDir1))) return 0;
	return (CompareString(LOCALE_SYSTEM_DEFAULT,
		NORM_IGNORECASE, szDir1, -1, szDir2, -1) - 2);
}

/*----------------------------------------------------------------------------------------
	ディレクトリパスへ変換
	(文字列末尾のバックスラッシュは除去)
----------------------------------------------------------------------------------------*/
BOOL StripDirectory(const char *pszFullPath, char *pszDirPath, const int nSize)
{
	if(!pszFullPath) return FALSE;
	if(!pszDirPath) return FALSE;
	char szBuf[MAX_PATH+1];
	if(!FullPathDirectory(pszFullPath, szBuf, nSize)) return FALSE;
	if(GetFileAttributes(szBuf) & FILE_ATTRIBUTE_DIRECTORY){
		if(szBuf[(strlen(szBuf) - 1)] == '\\') szBuf[(strlen(szBuf) - 1)] = '\0';
	}else{
		char *p, *r;
		for(p = r = szBuf; *p != '\0'; p++){
			if(IsDBCSLeadByte(*p)){
				p++;
				continue;
			}
			if(*p == '\\') r = p;
		}
		*r = '\0';
	}
//	strcpy(pszDirPath, szBuf);
	strcpy_s(pszDirPath, nSize, szBuf);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ディレクトリパスを相対から絶対へ変換
----------------------------------------------------------------------------------------*/
BOOL FullPathDirectory(const char *pszDirPath, char *pszFullPath, const int nSize)
{
	if(!pszFullPath) return FALSE;
	if(!pszDirPath) return FALSE;
	_fullpath(pszFullPath, pszDirPath, nSize);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ショートカットファイルを作成する
	※実行にはCOMの初期化が必須
----------------------------------------------------------------------------------------*/
BOOL CreateShellLink(const char *pszLinkFile, const char *pszTargetFile, const char *pszArguments, const char *pszWorkDir, int nShowCmd/* = SW_SHOWNORMAL*/)
{
	BOOL bReturn = FALSE;
	HRESULT hres = S_OK;
	IShellLink *psl = NULL; 
	IPersistFile *ppf = NULL;
	PWSTR pwszLinkFile = NULL;
	__try{
		// IShellLink インターフェースの取得
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
		if(!SUCCEEDED(hres)) __leave;
		// IPersistFile インターフェースの取得
		hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
		if(!SUCCEEDED(hres)) __leave;
		// リンク先をセット
		hres = psl->SetPath(pszTargetFile);
		if(!SUCCEEDED(hres)) __leave;
		// 引数をセット
		if(pszArguments){
			hres = psl->SetArguments(pszArguments);
			if(!SUCCEEDED(hres)) __leave;
		}
		// 作業フォルダをセット
		if(pszWorkDir){
			hres = psl->SetWorkingDirectory(pszWorkDir);
			if(!SUCCEEDED(hres)) __leave;
		}
		// ウィンドウ表示方法をセット
		hres = psl->SetShowCmd(nShowCmd);
		if(!SUCCEEDED(hres)) __leave;
		// 作成するショートカットファイル名をセット
		hres = psl->SetDescription(pszLinkFile);
		if(!SUCCEEDED(hres)) __leave;
		// UNICODEでパスを指定する
		int nNeeded = MultiByteToWideChar(CP_ACP, 0, pszLinkFile, -1, NULL, 0);
		pwszLinkFile = (PWSTR)GlobalAlloc(GPTR, (nNeeded + 1) * sizeof(wchar_t));
		if(!pwszLinkFile) __leave;
		if(!MultiByteToWideChar(CP_ACP, 0, pszLinkFile, -1, pwszLinkFile, nNeeded)) __leave;
		// 出力
		hres = ppf->Save(pwszLinkFile, TRUE);
		if(SUCCEEDED(hres)) bReturn = TRUE;
	}
	__finally{
		if(pwszLinkFile) GlobalFree(pwszLinkFile);
		if(ppf) ppf->Release();
		if(psl) psl->Release();
	}
	return bReturn;
}

/*----------------------------------------------------------------------------------------
	ソフトウェア使用許諾書の表示
----------------------------------------------------------------------------------------*/
int GetPermissionStr(const char *pszAppName, char *pszPermission)
{
	const char szDef[] = "【ソフトウェア使用許諾書】\n\n"
						"「%s」（以下本ソフトウェア）は、以下の条項全てに同意いただけた方のみご利用頂けます。\n"
						"本ソフトウェアは、本ソフトウェアの著者（以下著作者）の著作物であり、著作者によってのみライセンスされます。\n"
						"本ソフトウェアをインストールした時点（インストーラー形式でない場合は内容を読み取れる形式に変換した時点）で以下の条項に同意頂けたものとみなしますので、インストール前に各条項を十分にお読み下さい。同意できない場合は、たとえ試用目的であっても、本ソフトウェアを使用することはできません。使用を中止し、速やかに全てのインストールされたファイルを削除してください。\n\n"
						"１．「本ソフトウェア」について\n"
						" (1) 「本ソフトウェア」とは、「番号カウントクライアント」の原本及びその複製物（部分的複製物及び他のプログラムに結合された複製物を含みます。）を意味します。「本ソフトウェア」には、機械で読み取りうる形の命令、その構成物、データ、視聴覚コンテンツ（例えば、イメージ、テキスト、録音または写真機）及びその他の関連するライセンス資料が含まれます。\n"
						" (2) 本ソフトウェアには、スパイウェア等の有害なプログラムは、一切含まれていません。\n"
						" (3) 本ソフトウェアは、本ソフトウェアの動作不良が死亡、怪我又は重大な物理的又は環境的損害につながる恐れのある場所（例えば、原子力施設の運用、航空機の運行や航空管制等のシステム）において使用されることを想定したものではありません。\n\n"
						"２．使用権\n"
						" (1) 本ソフトウェアをインストールし、使用する者（以下使用者）に対して、自ら使用する権利を与えます。この権利は、独占的に有する権利ではないものとします。\n"
						" (2) 本使用許諾書は使用者が本ソフトウェアをインストールされた時点から発効します。\n"
						" (3) 使用者が、本使用許諾書により許諾される許諾プログラムの使用権を終了させる場合、記憶メディアからの削除をもって終了するものとします。この場合、複製物を含めた全ての本ソフトウェアに関するデータを破棄するものとします。\n"
						" (4) 本ソフトウェアの使用権は、本使用許諾書の使用条件の規定に基づき終了するまで有効に存続します。\n\n"
						"３．禁止事項\n"
						" (1) 使用者は、本ソフトウェアの全部または一部を、改変、リバースエンジニアリング、逆コンパイル又は逆アセンブルなどの解析作業や改変行為を一切行ってはいけません。\n"
						" (2) 使用者は、上記 (1) の方法又はそれ以外の方法で、本ソフトウェアのソースコードの抽出又は派生物の作成を試みてはいけません。\n"
						" (3) 使用者は、本ソフトウェア上に表示され、または本ソフトウェア中に含まれている、所有権、商標又は著作権の表示を、除去又は破棄してはいけません。\n\n"
						"４．その他\n"
						" (1) 本ソフトウェアの仕様及びマニュアル等の印刷物の内容は将来予告無く変更されることがあります。\n";
	if(!pszPermission) (int)(strlen(szDef) + strlen(pszAppName) - 2);
	wsprintf(pszPermission, szDef, pszAppName);
	return (int)(strlen(pszPermission));
}

/*----------------------------------------------------------------------------------------
	ファイル存在確認
----------------------------------------------------------------------------------------*/
BOOL ExistFile(const char *pszPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA w32fd;
	ZeroMemory(&w32fd, sizeof(WIN32_FIND_DATA));
	if(!pszPath) return FALSE;
	hFind = FindFirstFile(pszPath, &w32fd);
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;
	FindClose(hFind);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	プロセスに権限の付与
	void SetPrivileges(LPCTSTR lpName)
------------------------------------------------------------------------------------------
	SE_ASSIGNPRIMARYTOKEN_NAME
	TEXT("SeAssignPrimaryTokenPrivilege") Required to assign the primary token of a process. 

	User Right: Replace a process-level token.

	SE_AUDIT_NAME
	TEXT("SeAuditPrivilege") Required to generate audit-log entries. Give this privilege to secure servers. 

	User Right: Generate security audits.

	SE_BACKUP_NAME
	TEXT("SeBackupPrivilege") Required to perform backup operations. This privilege causes the system to grant all read access control to any file, regardless of the access control list (ACL) specified for the file. Any access request other than read is still evaluated with the ACL. This privilege is required by the RegSaveKey and RegSaveKeyExfunctions. The following access rights are granted if this privilege is held:

	READ_CONTROL
	ACCESS_SYSTEM_SECURITY
	FILE_GENERIC_READ
	FILE_TRAVERSE
	User Right: Back up files and directories.

	SE_CHANGE_NOTIFY_NAME
	TEXT("SeChangeNotifyPrivilege") Required to receive notifications of changes to files or directories. This privilege also causes the system to skip all traversal access checks. It is enabled by default for all users. 

	User Right: Bypass traverse checking.

	SE_CREATE_GLOBAL_NAME
	TEXT("SeCreateGlobalPrivilege") Required to create named file mapping objects in the global namespace during Terminal Services sessions. This privilege is enabled by default for administrators, services, and the local system account.

	User Right: Create global objects.

	Windows XP/2000:  This privilege is not supported. Note that this value is supported starting with Windows Server 2003, Windows XP with SP2, and Windows 2000 with SP4. 
	SE_CREATE_PAGEFILE_NAME
	TEXT("SeCreatePagefilePrivilege") Required to create a paging file. 

	User Right: Create a pagefile.

	SE_CREATE_PERMANENT_NAME
	TEXT("SeCreatePermanentPrivilege") Required to create a permanent object. 

	User Right: Create permanent shared objects.

	SE_CREATE_SYMBOLIC_LINK_NAME
	TEXT("SeCreateSymbolicLinkPrivilege") Required to create a symbolic link.

	User Right: Create symbolic links.

	SE_CREATE_TOKEN_NAME
	TEXT("SeCreateTokenPrivilege") Required to create a primary token. 

	User Right: Create a token object.

	SE_DEBUG_NAME
	TEXT("SeDebugPrivilege") Required to debug and adjust the memory of a process owned by another account. 

	User Right: Debug programs.

	SE_ENABLE_DELEGATION_NAME
	TEXT("SeEnableDelegationPrivilege") Required to mark user and computer accounts as trusted for delegation.

	User Right: Enable computer and user accounts to be trusted for delegation.

	SE_IMPERSONATE_NAME
	TEXT("SeImpersonatePrivilege") Required to impersonate.

	User Right: Impersonate a client after authentication.

	Windows XP/2000:  This privilege is not supported. Note that this value is supported starting with Windows Server 2003, Windows XP with SP2, and Windows 2000 with SP4. 
	SE_INC_BASE_PRIORITY_NAME
	TEXT("SeIncreaseBasePriorityPrivilege") Required to increase the base priority of a process. 

	User Right: Increase scheduling priority.

	SE_INCREASE_QUOTA_NAME
	TEXT("SeIncreaseQuotaPrivilege") Required to increase the quota assigned to a process. 

	User Right: Adjust memory quotas for a process.

	SE_INC_WORKING_SET_NAME
	TEXT("SeIncreaseWorkingSetPrivilege") Required to allocate more memory for applications that run in the context of users.

	User Right: Increase a process working set.

	SE_LOAD_DRIVER_NAME
	TEXT("SeLoadDriverPrivilege") Required to load or unload a device driver. 

	User Right: Load and unload device drivers.

	SE_LOCK_MEMORY_NAME
	TEXT("SeLockMemoryPrivilege") Required to lock physical pages in memory. 

	User Right: Lock pages in memory.

	SE_MACHINE_ACCOUNT_NAME
	TEXT("SeMachineAccountPrivilege") Required to create a computer account. 

	User Right: Add workstations to domain.

	SE_MANAGE_VOLUME_NAME
	TEXT("SeManageVolumePrivilege") Required to enable volume management privileges. 

	User Right: Manage the files on a volume.

	SE_PROF_SINGLE_PROCESS_NAME
	TEXT("SeProfileSingleProcessPrivilege") Required to gather profiling information for a single process. 

	User Right: Profile single process.

	SE_RELABEL_NAME
	TEXT("SeRelabelPrivilege") Required to modify the mandatory integrity level of an object.

	User Right: Modify an object label.

	SE_REMOTE_SHUTDOWN_NAME
	TEXT("SeRemoteShutdownPrivilege") Required to shut down a system using a network request. 

	User Right: Force shutdown from a remote system.

	SE_RESTORE_NAME
	TEXT("SeRestorePrivilege") Required to perform restore operations. This privilege causes the system to grant all write access control to any file, regardless of the ACL specified for the file. Any access request other than write is still evaluated with the ACL. Additionally, this privilege enables you to set any valid user or group SID as the owner of a file. This privilege is required by the RegLoadKey function. The following access rights are granted if this privilege is held:

	WRITE_DAC
	WRITE_OWNER
	ACCESS_SYSTEM_SECURITY
	FILE_GENERIC_WRITE
	FILE_ADD_FILE
	FILE_ADD_SUBDIRECTORY
	DELETE
	User Right: Restore files and directories.

	SE_SECURITY_NAME
	TEXT("SeSecurityPrivilege") Required to perform a number of security-related functions, such as controlling and viewing audit messages. This privilege identifies its holder as a security operator. 

	User Right: Manage auditing and security log.

	SE_SHUTDOWN_NAME
	TEXT("SeShutdownPrivilege") Required to shut down a local system. 

	User Right: Shut down the system.

	SE_SYNC_AGENT_NAME
	TEXT("SeSyncAgentPrivilege") Required for a domain controller to use the LDAP directory synchronization services. This privilege enables the holder to read all objects and properties in the directory, regardless of the protection on the objects and properties. By default, it is assigned to the Administrator and LocalSystem accounts on domain controllers. 

	User Right: Synchronize directory service data.

	SE_SYSTEM_ENVIRONMENT_NAME
	TEXT("SeSystemEnvironmentPrivilege") Required to modify the nonvolatile RAM of systems that use this type of memory to store configuration information. 

	User Right: Modify firmware environment values.

	SE_SYSTEM_PROFILE_NAME
	TEXT("SeSystemProfilePrivilege") Required to gather profiling information for the entire system. 

	User Right: Profile system performance.

	SE_SYSTEMTIME_NAME
	TEXT("SeSystemtimePrivilege") Required to modify the system time. 

	User Right: Change the system time.

	SE_TAKE_OWNERSHIP_NAME
	TEXT("SeTakeOwnershipPrivilege") Required to take ownership of an object without being granted discretionary access. This privilege allows the owner value to be set only to those values that the holder may legitimately assign as the owner of an object. 

	User Right: Take ownership of files or other objects.

	SE_TCB_NAME
	TEXT("SeTcbPrivilege") This privilege identifies its holder as part of the trusted computer base. Some trusted protected subsystems are granted this privilege. 

	User Right: Act as part of the operating system.

	SE_TIME_ZONE_NAME
	TEXT("SeTimeZonePrivilege") Required to adjust the time zone associated with the computer's internal clock.

	User Right: Change the time zone.

	SE_TRUSTED_CREDMAN_ACCESS_NAME
	TEXT("SeTrustedCredManAccessPrivilege") Required to access Credential Manager as a trusted caller.

	User Right: Access Credential Manager as a trusted caller.

	SE_UNDOCK_NAME
	TEXT("SeUndockPrivilege") Required to undock a laptop.

	User Right: Remove computer from docking station.

	SE_UNSOLICITED_INPUT_NAME
	TEXT("SeUnsolicitedInputPrivilege") Required to read unsolicited input from a terminal device.

	User Right: Not applicable.
----------------------------------------------------------------------------------------*/
void SetPrivileges(LPCTSTR lpName)
{
	DWORD ret = 0;
	HANDLE hToken = NULL;
	LUID Luid;

	TOKEN_PRIVILEGES tokenNew;
	TOKEN_PRIVILEGES tokenPre;

	ZeroMemory(&tokenNew, sizeof(tokenNew));
	ZeroMemory(&tokenPre, sizeof(tokenPre));

	HANDLE hProcess = GetCurrentProcess();

	if (!OpenProcessToken(
		hProcess,
		TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
		&hToken))
	{
		return;
	}

	if (!LookupPrivilegeValue(NULL, lpName, &Luid)) {
		CloseHandle(hToken);
		return;
	}

	tokenNew.PrivilegeCount = 1;
	tokenNew.Privileges[0].Luid = Luid;
	tokenNew.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tokenNew,
		sizeof(tokenPre),
		&tokenPre,
		&ret);

	CloseHandle(hToken);
}

/*----------------------------------------------------------------------------------------
	エディットボックスにバルーンチップ
----------------------------------------------------------------------------------------*/
void EditBoxShowBalloon(LPCOMEDITBOXSHOWBALLOON lpcesb)
{
	EDITBALLOONTIP esbt;
	ZeroMemory(&esbt, sizeof(EDITBALLOONTIP));

	wchar_t wszTitle[128];
	wchar_t wszText[512];

	if (MultiByteToWideChar(
		CP_ACP,
		0,
		lpcesb->szTitle,
		-1,
		wszTitle,
		_countof(wszTitle)) == 0)
	{
		return;
	}

	if (MultiByteToWideChar(
		CP_ACP,
		0,
		lpcesb->szText,
		-1,
		wszText,
		_countof(wszText)) == 0)
	{
		return;
	}

	esbt.cbStruct = sizeof(EDITBALLOONTIP);
	esbt.pszTitle = wszTitle;
	esbt.pszText = wszText;
	esbt.ttiIcon = lpcesb->nIcon;

	Edit_ShowBalloonTip(lpcesb->hEdit, &esbt);
}

