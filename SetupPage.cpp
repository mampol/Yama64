/*----------------------------------------------------------------------------------------

	SetupPage.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "CTreePropPage.h"
//#include "CUICombo.h"
#include "SetupPage.h"
//#include "LicenceFunc.h"
//#include "HpAuth.h"
#include "resource.h"

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HWND ghMainWnd,
			ghPageAnalysys,
			ghPageAnalysysCore,
			ghPageExport,
			ghPageFolder,
			ghPageUI,
			ghPageOther;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupAnalysysPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK SetupAnalysysCorePageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK SetupExportPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK SetupFolderPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK SetupUIPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK SetupOtherPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

LRESULT OnInitAnalysysPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnApplyAnalysysPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

LRESULT OnInitAnalysysCorePage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
void SetErrorTypeText(HWND hPage, char *pszErrSign);
void GetErrorTypeText(HWND hPage, char *pszErrSign);
LRESULT OnApplyAnalysysCorePage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

LRESULT OnInitExportPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnCommandExportPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
BOOL AddExportItem(HWND Page);
BOOL DeleteExportItem(HWND Page);
BOOL MoveExportItem(HWND hPage, int nMove);
LRESULT OnApplyExportPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

LRESULT OnInitFolderPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnCommandFolderPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnHistoryDelete(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnApplyFolderPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

LRESULT OnInitUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnCommandUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnMeasureItemUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDrawItemUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
int SetDefaultUIColor(HWND hPage);
LRESULT OnApplyUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnDestroyUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

LRESULT OnInitOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
//LRESULT OnCommandOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnPaintOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnApplyOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);

HWND SetSpin(HWND hPage, HWND hBuddy, const int nUpper, const int nLower, const int nPos);

/*----------------------------------------------------------------------------------------
	設定画面表示
----------------------------------------------------------------------------------------*/
BOOL ShowSetupPage(HWND hParent, CYamaSetup *lpCSetup)
{
	if(!lpCSetup) return FALSE;
	ghMainWnd = hParent;
//	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE);
	CTreeProp CTreeProp;
	if(CTreeProp.Initialize(ICC_WIN95_CLASSES,
		hInst,
		hParent,
		SETUPPAGE_CLASS,
		SETUPPAGE_CAPTION,
		PSH_HASHELP,
		MAKEINTRESOURCE(IDI_ICON01),
		IDR_PNG_TREE,
		6,
		220,
		30)){

		HDC hDC = GetDC(CTreeProp.GetTreeHandle());
		HFONT ghTreeFont = MAKEFONT(hDC, 10, FW_NORMAL, FALSE, "Meiryo UI");
		if(ghTreeFont) CTreeProp.SetTreeFont(ghTreeFont);
		ReleaseDC(CTreeProp.GetTreeHandle(), hDC);

		//// インフォメーション
		//char szInfoMation[64];
		//wsprintf(szInfoMation, "%s %s", APP_NAME, APP_VERSION);
		//CTreeProp.SetInfoStr(szInfoMation);

		//// インデント
		//CTreeProp.SetTreeIndent(4);

		// ページ作成
		ghPageAnalysys = CTreeProp.AddPage(TREE_PROP_ROOT,
			hInst,
			MAKEINTRESOURCE(IDD_PAGE_ANALYSYS),
			(DLGPROC)SetupAnalysysPageProc,
			NULL, 0, 0,
			(LPVOID)lpCSetup);

		ghPageAnalysysCore = CTreeProp.AddPage(ghPageAnalysys,
			hInst,
			MAKEINTRESOURCE(IDD_PAGE_ANALYSYS_CORE),
			(DLGPROC)SetupAnalysysCorePageProc,
			NULL, 1, 1,
			(LPVOID)lpCSetup);

		ghPageExport = CTreeProp.AddPage(ghPageAnalysys,
			hInst,
			MAKEINTRESOURCE(IDD_PAGE_EXPORT),
			(DLGPROC)SetupExportPageProc,
			NULL, 4, 4,
			(LPVOID)lpCSetup);

		ghPageFolder = CTreeProp.AddPage(TREE_PROP_ROOT,
			hInst,
			MAKEINTRESOURCE(IDD_PAGE_FOLDER),
			(DLGPROC)SetupFolderPageProc,
			NULL, 2, 2,
			(LPVOID)lpCSetup);

		ghPageUI = CTreeProp.AddPage(TREE_PROP_ROOT,
			hInst,
			MAKEINTRESOURCE(IDD_PAGE_UI),
			(DLGPROC)SetupUIPageProc,
			NULL, 3, 3,
			(LPVOID)lpCSetup);

		ghPageOther = CTreeProp.AddPage(TREE_PROP_ROOT,
			hInst,
			MAKEINTRESOURCE(IDD_PAGE_OTHER),
			(DLGPROC)SetupOtherPageProc,
			NULL, 5, 5,
			(LPVOID)lpCSetup);

		// ページの表示と設定ファイルの更新
		BOOL bResult = CTreeProp.Show();

		// リソース破棄
		if (ghTreeFont) {
			DeleteObject(ghTreeFont);
			ghTreeFont = NULL;
		}

		return bResult;
	}
	return FALSE;
}

/*----------------------------------------------------------------------------------------
	解析の設定
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupAnalysysPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		return (OnInitAnalysysPage(hPage, msg, wp, lp));
	case UM_TPM_SETACTIVE:
		break;
	case UM_TPM_KILLACTIVE:
		break;
	case UM_TPM_APPLY:
		return (OnApplyAnalysysPage(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析の設定初期化
----------------------------------------------------------------------------------------*/
LRESULT OnInitAnalysysPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	// コントロール設定
	SETTEXTLIMIT(hPage, IDC_EDIT_DEFLV, 3);
	SETTEXTLIMIT(hPage, IDC_EDIT_CBLEXT, 255);
	// 設定値の復元
	SetDlgItemInt(hPage, IDC_EDIT_DEFLV, lpCSetup->m_AnalysysProp.m_nDefExpandLevel, FALSE);
	SetDlgItemText(hPage, IDC_EDIT_CBLEXT, lpCSetup->m_AnalysysProp.m_szCblExt);
	if(lpCSetup->m_AnalysysProp.m_bBeginMsg) CheckDlgButton(hPage, IDC_CHECK_CONFDLG, BST_CHECKED);
	if(lpCSetup->m_AnalysysProp.m_bCmdLineSet) CheckDlgButton(hPage, IDC_CHECK_CMDLINE, BST_CHECKED);
	if(lpCSetup->m_AnalysysProp.m_bDropSet) CheckDlgButton(hPage, IDC_CHECK_DROP, BST_CHECKED);
	if(lpCSetup->m_AnalysysProp.m_bDropInit) CheckDlgButton(hPage, IDC_CHECK_DROP2, BST_CHECKED);
	if(lpCSetup->m_AnalysysProp.m_bPasteSet) CheckDlgButton(hPage, IDC_CHECK_PASTE, BST_CHECKED);
	if(lpCSetup->m_AnalysysProp.m_bPasteInit) CheckDlgButton(hPage, IDC_CHECK_PASTE2, BST_CHECKED);
	// スピン
	SetSpin(hPage, GetDlgItem(hPage, IDC_EDIT_DEFLV), 999, 1, lpCSetup->m_AnalysysProp.m_nDefExpandLevel);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析設定の保存
----------------------------------------------------------------------------------------*/
LRESULT OnApplyAnalysysPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;
	// 設定値取得
	lpCSetup->m_AnalysysProp.m_nDefExpandLevel = GetDlgItemInt(hPage, IDC_EDIT_DEFLV, NULL, FALSE);
	GetDlgItemText(hPage, IDC_EDIT_CBLEXT, lpCSetup->m_AnalysysProp.m_szCblExt, sizeof(lpCSetup->m_AnalysysProp.m_szCblExt));
	if(IsDlgButtonChecked(hPage, IDC_CHECK_CONFDLG) == BST_CHECKED){
		lpCSetup->m_AnalysysProp.m_bBeginMsg = TRUE;
	}else{
		lpCSetup->m_AnalysysProp.m_bBeginMsg = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_CMDLINE) == BST_CHECKED){
		lpCSetup->m_AnalysysProp.m_bCmdLineSet = TRUE;
	}else{
		lpCSetup->m_AnalysysProp.m_bCmdLineSet = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_DROP) == BST_CHECKED){
		lpCSetup->m_AnalysysProp.m_bDropSet = TRUE;
	}else{
		lpCSetup->m_AnalysysProp.m_bDropSet = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_DROP2) == BST_CHECKED){
		lpCSetup->m_AnalysysProp.m_bDropInit = TRUE;
	}else{
		lpCSetup->m_AnalysysProp.m_bDropInit = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_PASTE) == BST_CHECKED){
		lpCSetup->m_AnalysysProp.m_bPasteSet = TRUE;
	}else{
		lpCSetup->m_AnalysysProp.m_bPasteSet = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_PASTE2) == BST_CHECKED){
		lpCSetup->m_AnalysysProp.m_bPasteInit = TRUE;
	}else{
		lpCSetup->m_AnalysysProp.m_bPasteInit = FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析詳細の設定
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupAnalysysCorePageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		return (OnInitAnalysysCorePage(hPage, msg, wp, lp));
	case WM_COMMAND:
		break;
	case UM_TPM_SETACTIVE:
		break;
	case UM_TPM_KILLACTIVE:
		break;
	case UM_TPM_APPLY:
		return (OnApplyAnalysysCorePage(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析詳細の設定初期化
----------------------------------------------------------------------------------------*/
LRESULT OnInitAnalysysCorePage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;
	// コントロール設定
	SETTEXTLIMIT(hPage, IDC_EDIT_SEQLEN, 2);
	SETTEXTLIMIT(hPage, IDC_EDIT_MAXSRCLINE, 4);
	SETTEXTLIMIT(hPage, IDC_EDIT_SMEDEXT, (int)sizeof(lpCSetup->m_CoreAnalysysProp.m_szSmdExt));
	SETTEXTLIMIT(hPage, IDC_EDIT_ERRTYPE1, 1);
	SETTEXTLIMIT(hPage, IDC_EDIT_ERRTYPE2, 1);
	SETTEXTLIMIT(hPage, IDC_EDIT_ERRTYPE3, 1);
	SETTEXTLIMIT(hPage, IDC_EDIT_ERRTYPE4, 1);
	SETTEXTLIMIT(hPage, IDC_EDIT_ERRTYPE5, 1);
	// 設定値の復元
	SetDlgItemInt(hPage, IDC_EDIT_SEQLEN, lpCSetup->m_CoreAnalysysProp.m_nSeq, FALSE);
	SetDlgItemInt(hPage, IDC_EDIT_MAXSRCLINE, lpCSetup->m_CoreAnalysysProp.m_nMaxLine, FALSE);
	SetDlgItemText(hPage, IDC_EDIT_SMEDEXT, lpCSetup->m_CoreAnalysysProp.m_szSmdExt);
	SetErrorTypeText(hPage, lpCSetup->m_CoreAnalysysProp.m_szSign);
	if(lpCSetup->m_CoreAnalysysProp.m_bHostMode) CheckDlgButton(hPage, IDC_CHECK_HOSTCOMP, BST_CHECKED);
	if(lpCSetup->m_CoreAnalysysProp.m_nCompPos > 0) CheckDlgButton(hPage, IDC_CHECK_COMPPOS, BST_CHECKED);
	if(lpCSetup->m_CoreAnalysysProp.m_bUseCodeCheck) CheckDlgButton(hPage, IDC_CHECK_CODECHECK, BST_CHECKED);
	// スピン
	SetSpin(hPage, GetDlgItem(hPage, IDC_EDIT_SEQLEN), 99, 1, lpCSetup->m_CoreAnalysysProp.m_nSeq);
	SetSpin(hPage, GetDlgItem(hPage, IDC_EDIT_MAXSRCLINE), 2000, 0, lpCSetup->m_CoreAnalysysProp.m_nMaxLine);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	エラーとしない記号
----------------------------------------------------------------------------------------*/
void SetErrorTypeText(HWND hPage, char *pszErrSign)
{
	// １Byteずつ処理
	char szVal[2];
	int nLen = (int)strlen(pszErrSign);
	const int nEditId[5] = { IDC_EDIT_ERRTYPE1,
							 IDC_EDIT_ERRTYPE2, 
							 IDC_EDIT_ERRTYPE3, 
							 IDC_EDIT_ERRTYPE4, 
							 IDC_EDIT_ERRTYPE5 };
	for(int i = 0; i < nLen; i++){
		if(*(pszErrSign + i) == ' ') continue;
		szVal[0] = *(pszErrSign + i);
		szVal[1] = '\0';
		SetDlgItemText(hPage, nEditId[i], szVal);
	}
}

void GetErrorTypeText(HWND hPage, char *pszErrSign)
{
	// １Byteずつ処理
	char szVal[2];
	int nLen = (int)strlen(pszErrSign), i;
	const int nEditId[5] = { IDC_EDIT_ERRTYPE1,
							 IDC_EDIT_ERRTYPE2, 
							 IDC_EDIT_ERRTYPE3, 
							 IDC_EDIT_ERRTYPE4, 
							 IDC_EDIT_ERRTYPE5 };
	for(i = 0; i < nLen; i++){
		if(GetDlgItemText(hPage, nEditId[i], szVal, (int)sizeof(szVal)) > 0){
			*(pszErrSign + i) = szVal[0];
		}else{
			*(pszErrSign + i) = ' ';
		}
	}
	*(pszErrSign + i) = '\0';
}

/*----------------------------------------------------------------------------------------
	解析詳細設定の保存
----------------------------------------------------------------------------------------*/
LRESULT OnApplyAnalysysCorePage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;
	// 設定値の取得
	lpCSetup->m_CoreAnalysysProp.m_nSeq = GetDlgItemInt(hPage, IDC_EDIT_SEQLEN, NULL, FALSE);
	lpCSetup->m_CoreAnalysysProp.m_nMaxLine = GetDlgItemInt(hPage, IDC_EDIT_MAXSRCLINE, NULL, FALSE);
	GetDlgItemText(hPage, IDC_EDIT_SMEDEXT, lpCSetup->m_CoreAnalysysProp.m_szSmdExt, (int)sizeof(lpCSetup->m_CoreAnalysysProp.m_szSmdExt));
	GetErrorTypeText(hPage, lpCSetup->m_CoreAnalysysProp.m_szSign);
	if(IsDlgButtonChecked(hPage, IDC_CHECK_HOSTCOMP) == BST_CHECKED){
		lpCSetup->m_CoreAnalysysProp.m_bHostMode = TRUE;
	}else{
		lpCSetup->m_CoreAnalysysProp.m_bHostMode = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_COMPPOS) == BST_CHECKED){
		lpCSetup->m_CoreAnalysysProp.m_nCompPos = 1;
	}else{
		lpCSetup->m_CoreAnalysysProp.m_nCompPos = 0;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHECK_CODECHECK) == BST_CHECKED){
		lpCSetup->m_CoreAnalysysProp.m_bUseCodeCheck = TRUE;
	}else{
		lpCSetup->m_CoreAnalysysProp.m_bUseCodeCheck = FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力の設定
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupExportPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		return (OnInitExportPage(hPage, msg, wp, lp));
	case WM_COMMAND:
		return (OnCommandExportPage(hPage, msg, wp, lp));
		break;
	case UM_TPM_SETACTIVE:
		break;
	case UM_TPM_KILLACTIVE:
		break;
	case UM_TPM_APPLY:
		return (OnApplyExportPage(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力設定の初期化
----------------------------------------------------------------------------------------*/
LRESULT OnInitExportPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;

	// 設定復元
	for(int nIndex = 0; nIndex < MAX_EXPORTPROP; nIndex++){
		char szExportStr[256], szListString[256];
		szExportStr[0] = '\0';
		int nItem = 0, nOutNo = lpCSetup->GetExportStr(nIndex, szExportStr, sizeof(szExportStr));
		wsprintf(szListString, " (%02d) %s", nIndex + 1, szExportStr);
		if(nOutNo < 0){
			nItem = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_ADDSTRING, 0L, (LPARAM)szListString);
			SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_SETITEMDATA, (WPARAM)nItem, (LPARAM)nIndex);
		}else{
			// 該当位置に復元する
			int nCount = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETCOUNT, 0L, 0L);
			if(nCount > nOutNo){
				nItem = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_INSERTSTRING, (WPARAM)nOutNo, (LPARAM)szListString);
			}else{
				nItem = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_ADDSTRING, 0L, (LPARAM)szListString);
			}
			SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_SETITEMDATA, (WPARAM)nItem, (LPARAM)nIndex);
		}
//		if(nItem == LB_ERR){
//			ERRMSG(hPage, "解析結果出力設定の初期化に失敗");
//			break;
//		}
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力のコマンドプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnCommandExportPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(LOWORD(wp)){
	case IDC_BTN_ADD:
		AddExportItem(hPage);
		break;
	case IDC_BTN_DEL:
		DeleteExportItem(hPage);
		break;
	case IDC_BTN_ITEMUP:
		MoveExportItem(hPage, -1);
		break;
	case IDC_BTN_ITEMDOWN:
		MoveExportItem(hPage, 1);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力項目追加
----------------------------------------------------------------------------------------*/
BOOL AddExportItem(HWND hPage)
{
	int nCount = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_GETCOUNT, 0L, 0L);
	if(nCount < 1) return TRUE;

	int nSel = 0;
	if((nSel = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_GETCURSEL, 0L, 0L)) == LB_ERR){
		ERRMSG(hPage, "項目が選択されていません");
		return FALSE;
	}

	int nIndex = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_GETITEMDATA, (WPARAM)nSel, 0L);
	if(nIndex < 0) return FALSE;

	char szListString[256];
	SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_GETTEXT, (WPARAM)nSel, (LPARAM)szListString);

	SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_DELETESTRING, (WPARAM)nSel, 0L);
	int nItem = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_ADDSTRING, 0L, (LPARAM)szListString);
	SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_SETITEMDATA, (WPARAM)nItem, (LPARAM)nIndex);

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力項目削除
----------------------------------------------------------------------------------------*/
BOOL DeleteExportItem(HWND hPage)
{
	int nCount = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETCOUNT, 0L, 0L);
	if(nCount < 2){
		ERRMSG(hPage, "出力項目は最低でも１つは残しといてください...");
		return TRUE;
	}

	int nSel = 0;
	if((nSel = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETCURSEL, 0L, 0L)) == LB_ERR){
		ERRMSG(hPage, "項目が選択されていません");
		return FALSE;
	}

	int nIndex = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETITEMDATA, (WPARAM)nSel, 0L);
	if(nIndex < 0) return FALSE;

	char szListString[256];
	SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETTEXT, (WPARAM)nSel, (LPARAM)szListString);

	SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_DELETESTRING, (WPARAM)nSel, 0L);
	int nItem = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_ADDSTRING, 0L, (LPARAM)szListString);
	SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_SETITEMDATA, (WPARAM)nItem, (LPARAM)nIndex);

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力項目移動
----------------------------------------------------------------------------------------*/
BOOL MoveExportItem(HWND hPage, int nMove)
{
	if(nMove == 0) return TRUE;

	int nCount = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETCOUNT, 0L, 0L);
	if(nCount < 2) return TRUE;

	int nSel = 0;
	if((nSel = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETCURSEL, 0L, 0L)) == LB_ERR){
		ERRMSG(hPage, "項目が選択されていません");
		return FALSE;
	}
	// リスト項目情報取得
	char szListString[256];
	if(SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETTEXT, (WPARAM)nSel, (LPARAM)szListString) == LB_ERR) return FALSE;
	int nIndex = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETITEMDATA, (WPARAM)nSel, 0L);
	if(nIndex < 0) return FALSE;

	int nMovePos = nSel + nMove;
	// 下移動は0始りのインデックスなので+1
	if(nMove > 0) nMovePos++;
	if((nMovePos < 0) || (nMovePos > nCount)) return TRUE;

	// とりあえず、追加
	int nInsert = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_INSERTSTRING, (WPARAM)nMovePos, (LPARAM)szListString);
	SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_SETITEMDATA, (WPARAM)nInsert, (LPARAM)nIndex);
	// 移動が↓ならば、素直に元項目削除、↑の場合は挿入項目を加味して削除
	if(nMove > 0){
		SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_DELETESTRING, (WPARAM)nSel, 0L);
		SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_SETCURSEL, (WPARAM)nInsert - 1, 0L);
	}else{
		SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_DELETESTRING, (WPARAM)(nSel + 1), 0L);
		SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_SETCURSEL, (WPARAM)nInsert, 0L);
	}

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果出力設定の保存
----------------------------------------------------------------------------------------*/
LRESULT OnApplyExportPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;

	// 設定保存
	int nOutNo = 0, nItem = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETCOUNT, 0L, 0L);
	for(nOutNo = 0; nOutNo < nItem && nOutNo < MAX_EXPORTPROP; nOutNo++){
		int nIndex = SendDlgItemMessage(hPage, IDC_LIST_HEADER, LB_GETITEMDATA, (WPARAM)nOutNo, 0L);
		if(nIndex == LB_ERR) continue;
		lpCSetup->m_ExportProp.un_ExportProp.m_nExportProp[nIndex] = nOutNo;
	}
	nItem = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_GETCOUNT, 0L, 0L);
	for(nOutNo = 0; nOutNo < nItem && nOutNo < MAX_EXPORTPROP; nOutNo++){
		int nIndex = SendDlgItemMessage(hPage, IDC_LIST_DISHEADER, LB_GETITEMDATA, (WPARAM)nOutNo, 0L);
		if(nIndex == LB_ERR) continue;
		lpCSetup->m_ExportProp.un_ExportProp.m_nExportProp[nIndex] = -1;	// 除外とする
	}

	return TRUE;
}

/*----------------------------------------------------------------------------------------
	パスの設定
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupFolderPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		return (OnInitFolderPage(hPage, msg, wp, lp));
	case WM_COMMAND:
		return (OnCommandFolderPage(hPage, msg, wp, lp));
	case UM_TPM_SETACTIVE:
		break;
	case UM_TPM_KILLACTIVE:
		break;
	case UM_TPM_APPLY:
		return (OnApplyFolderPage(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	パスの設定初期化
----------------------------------------------------------------------------------------*/
LRESULT OnInitFolderPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	// コントロール設定
	SETTEXTLIMIT(hPage, IDC_EDIT_DEFOPENDIR, (int)sizeof(lpCSetup->m_PathProp.m_szDefOpenDir));
	SETTEXTLIMIT(hPage, IDC_EDIT_COBOLOPEN, (int)sizeof(lpCSetup->m_PathProp.m_szCobolOpen));
	SETTEXTLIMIT(hPage, IDC_EDIT_SMDOPEN, (int)sizeof(lpCSetup->m_PathProp.m_szSmdOpen));
	SETTEXTLIMIT(hPage, IDC_EDIT_TMPDIR, (int)sizeof(lpCSetup->m_PathProp.m_szTmpDir));
	SETTEXTLIMIT(hPage, IDC_EDIT_OPENHISCOUNT, 2);
//	EnableWindow(GetDlgItem(hPage, IDC_STATIC_TMP), lpCSetup->m_PathProp.m_bEnableTmpPath);
//	EnableWindow(GetDlgItem(hPage, IDC_EDIT_TMPDIR), lpCSetup->m_PathProp.m_bEnableTmpPath);
//	EnableWindow(GetDlgItem(hPage, IDC_BTN_TMPDIR), lpCSetup->m_PathProp.m_bEnableTmpPath);
	// 設定値の復元
	SetDlgItemText(hPage, IDC_EDIT_DEFOPENDIR, lpCSetup->m_PathProp.m_szDefOpenDir);
	SetDlgItemText(hPage, IDC_EDIT_COBOLOPEN, lpCSetup->m_PathProp.m_szCobolOpen);
	SetDlgItemText(hPage, IDC_EDIT_SMDOPEN, lpCSetup->m_PathProp.m_szSmdOpen);
	SetDlgItemText(hPage, IDC_EDIT_TMPDIR, lpCSetup->m_PathProp.m_szTmpDir);
	SetDlgItemInt(hPage, IDC_EDIT_OPENHISCOUNT, lpCSetup->m_PathProp.m_nHistorySave, FALSE);
	// スピン
	SetSpin(hPage, GetDlgItem(hPage, IDC_EDIT_OPENHISCOUNT), 20, 0, lpCSetup->m_PathProp.m_nHistorySave);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	パス設定のコマンドプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT OnCommandFolderPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	char szPath[MAX_PATH+1];
	switch(LOWORD(wp)){
	case IDC_BTN_DEFOPENDIR:
		GetDlgItemText(hPage, IDC_EDIT_DEFOPENDIR, szPath, (int)sizeof(szPath));
		if(!GetSHFolderDlg(hPage, szPath, "初期表示フォルダの選択", TRUE, TRUE)) break;
		SetDlgItemText(hPage, IDC_EDIT_DEFOPENDIR, szPath);
		break;
	case IDC_BTN_COBOLOPEN:
		GetDlgItemText(hPage, IDC_EDIT_COBOLOPEN, szPath, (int)sizeof(szPath));
		if(!GetOpenFileDlg(hPage,
			szPath, (const int)sizeof(szPath),
			"ｱﾌﾟﾘｹｰｼｮﾝ (*.exe)\0*.exe\0",
			"COBOLｿｰｽﾌｧｲﾙを開くｱﾌﾟﾘｹｰｼｮﾝの選択")) break;
		SetDlgItemText(hPage, IDC_EDIT_COBOLOPEN, szPath);
		break;
	case IDC_BTN_SMDOPEN:
		GetDlgItemText(hPage, IDC_EDIT_SMDOPEN, szPath, (int)sizeof(szPath));
		if(!GetOpenFileDlg(hPage,
			szPath, (const int)sizeof(szPath),
			"ｱﾌﾟﾘｹｰｼｮﾝ (*.exe)\0*.exe\0",
			"FORMｿｰｽﾌｧｲﾙを開くｱﾌﾟﾘｹｰｼｮﾝの選択")) break;
		SetDlgItemText(hPage, IDC_EDIT_SMDOPEN, szPath);
		break;
	case IDC_BTN_TMPDIR:
		GetDlgItemText(hPage, IDC_EDIT_TMPDIR, szPath, (int)sizeof(szPath));
		if(!GetSHFolderDlg(hPage, szPath, "作業用フォルダの選択", TRUE, TRUE)) break;
		SetDlgItemText(hPage, IDC_EDIT_TMPDIR, szPath);
		break;
	case IDC_BTN_OPENHISDEL:
		return (OnHistoryDelete(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析履歴消去
----------------------------------------------------------------------------------------*/
LRESULT OnHistoryDelete(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	if(YESNOMSG(hPage, "解析履歴を消去します") == IDNO) return TRUE;
	SendMessage(ghMainWnd, WM_COMMAND, (WPARAM)IDM_DELETEHISTORY, 0L);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	パス設定の保存
----------------------------------------------------------------------------------------*/
LRESULT OnApplyFolderPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;
	GetDlgItemText(hPage, IDC_EDIT_DEFOPENDIR, lpCSetup->m_PathProp.m_szDefOpenDir, (int)sizeof(lpCSetup->m_PathProp.m_szDefOpenDir));
	GetDlgItemText(hPage, IDC_EDIT_COBOLOPEN, lpCSetup->m_PathProp.m_szCobolOpen, (int)sizeof(lpCSetup->m_PathProp.m_szCobolOpen));
	GetDlgItemText(hPage, IDC_EDIT_SMDOPEN, lpCSetup->m_PathProp.m_szSmdOpen, (int)sizeof(lpCSetup->m_PathProp.m_szSmdOpen));
	GetDlgItemText(hPage, IDC_EDIT_TMPDIR, lpCSetup->m_PathProp.m_szTmpDir, (int)sizeof(lpCSetup->m_PathProp.m_szTmpDir));
	lpCSetup->m_PathProp.m_nHistorySave = GetDlgItemInt(hPage, IDC_EDIT_OPENHISCOUNT, NULL, FALSE);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ＵＩの設定
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupUIPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		return (OnInitUIPage(hPage, msg, wp, lp));
	case WM_COMMAND:
		return (OnCommandUIPage(hPage, msg, wp, lp));
	case WM_MEASUREITEM:
		return (OnMeasureItemUIPage(hPage, msg, wp, lp));
	case WM_DRAWITEM:
		return (OnDrawItemUIPage(hPage, msg, wp, lp));
	case UM_TPM_SETACTIVE:
		break;
	case UM_TPM_KILLACTIVE:
		break;
	case UM_TPM_APPLY:
		return (OnApplyUIPage(hPage, msg, wp, lp));
	case WM_DESTROY:
		return (OnDestroyUIPage(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ＵＩの設定初期化
----------------------------------------------------------------------------------------*/
LRESULT OnInitUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	// フォント設定ダイアログ
	CUICombo *lpCUIC = (CUICombo *)new CUICombo();
	if(lpCUIC){
		lpCUIC->CreateFontFaceCombo(GetDlgItem(hPage, IDC_CMB_FONTFACE), SHIFTJIS_CHARSET, "");
		lpCUIC->CreateFontSizeCombo(GetDlgItem(hPage, IDC_CMB_FONTSIZE), 6, 24);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_FORECOLOR), lpCSetup->m_UIProp.m_colFore);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_BACKCOLOR), lpCSetup->m_UIProp.m_colBack);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_STRUCTCOLOR), lpCSetup->m_UIProp.m_colStruct);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_OCCURSCOLOR), lpCSetup->m_UIProp.m_colOccurs);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_REDEFINESCOLOR), lpCSetup->m_UIProp.m_colRedefines);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_BINCOLOR), lpCSetup->m_UIProp.m_colBinary);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_SUPPCOLOR), lpCSetup->m_UIProp.m_colSupplement);
		lpCUIC->CreateColorCombo(GetDlgItem(hPage, IDC_CMB_DATCOLOR), lpCSetup->m_UIProp.m_colDatCol);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_FONTFACE), lpCSetup->m_UIProp.m_szFontFace);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_FONTSIZE), lpCSetup->m_UIProp.m_nFontSize - 7);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_FORECOLOR), (DWORD)lpCSetup->m_UIProp.m_colFore);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_BACKCOLOR), (DWORD)lpCSetup->m_UIProp.m_colBack);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_STRUCTCOLOR), (DWORD)lpCSetup->m_UIProp.m_colStruct);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_OCCURSCOLOR), (DWORD)lpCSetup->m_UIProp.m_colOccurs);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_REDEFINESCOLOR), (DWORD)lpCSetup->m_UIProp.m_colRedefines);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_BINCOLOR), (DWORD)lpCSetup->m_UIProp.m_colBinary);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_SUPPCOLOR), (DWORD)lpCSetup->m_UIProp.m_colSupplement);
		lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_DATCOLOR), (DWORD)lpCSetup->m_UIProp.m_colDatCol);
		SetProp(hPage, CUICOMBO_CLASS, (HANDLE)lpCUIC);
	}
	if(lpCSetup->m_UIProp.m_bUseBackColor) CheckDlgButton(hPage, IDC_CHK_COLBK, BST_CHECKED);
	if(lpCSetup->m_UIProp.m_bRestorMainWnd) CheckDlgButton(hPage, IDC_CHK_RESTORWND, BST_CHECKED);
	if(lpCSetup->m_UIProp.m_bItemImage) CheckDlgButton(hPage, IDC_CHK_ITEMIMAGE, BST_CHECKED);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ＵＩの設定ページコマンド
----------------------------------------------------------------------------------------*/
LRESULT OnCommandUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	CUICombo *lpCUIC = (CUICombo *)GetProp(hPage, CUICOMBO_CLASS);
	switch(LOWORD(wp)){
	case IDC_CMB_FORECOLOR:
	case IDC_CMB_BACKCOLOR:
	case IDC_CMB_STRUCTCOLOR:
	case IDC_CMB_OCCURSCOLOR:
	case IDC_CMB_REDEFINESCOLOR:
	case IDC_CMB_BINCOLOR:
	case IDC_CMB_SUPPCOLOR:
	case IDC_CMB_DATCOLOR:
		if(HIWORD(wp) == CBN_SELCHANGE && lpCUIC){
			lpCUIC->OnCommandCustomColor(GetDlgItem(hPage, LOWORD(wp)), hPage);
		}
		break;
	case IDC_BTN_DEFCOL:
		SetDefaultUIColor(hPage);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	色指定コンボ初期設定
----------------------------------------------------------------------------------------*/
LRESULT OnMeasureItemUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	LPMEASUREITEMSTRUCT lpMis = (LPMEASUREITEMSTRUCT)lp;
	CUICombo *lpCUIC = (CUICombo *)GetProp(hPage, CUICOMBO_CLASS);
	if(lpCUIC) lpCUIC->OnMeasureItemColorCombo(GetDlgItem(hPage, lpMis->CtlID), hPage, msg, wp, lp);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	色指定コンボオーナードロー
----------------------------------------------------------------------------------------*/
LRESULT OnDrawItemUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lp;
	CUICombo *lpCUIC = (CUICombo *)GetProp(hPage, CUICOMBO_CLASS);
	if(lpCUIC) lpCUIC->OnDrawItemColorCombo(lpDis->hwndItem, hPage, msg, wp, lp);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	色指定コンボデフォルト設定
----------------------------------------------------------------------------------------*/
int SetDefaultUIColor(HWND hPage)
{
	CUICombo *lpCUIC = (CUICombo *)GetProp(hPage, CUICOMBO_CLASS);
	if(!lpCUIC) return 0;
	if(YESNOMSG(hPage, "標準の配色に設定します") == IDNO) return 0;
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_FORECOLOR), UI_COL_FOR);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_BACKCOLOR), UI_COL_BAK);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_STRUCTCOLOR), UI_COL_STRUCT);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_OCCURSCOLOR), UI_COL_OCCURS);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_REDEFINESCOLOR), UI_COL_REDEFINES);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_BINCOLOR), UI_COL_BINARY);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_SUPPCOLOR), UI_COL_SUPP);
	lpCUIC->SelectListItem(GetDlgItem(hPage, IDC_CMB_DATCOLOR), UI_COL_DATCOL);
	return 0;
}

/*----------------------------------------------------------------------------------------
	ＵＩ設定の保存
----------------------------------------------------------------------------------------*/
LRESULT OnApplyUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;
	CUICombo *lpCUIC = (CUICombo *)GetProp(hPage, CUICOMBO_CLASS);
	if(!lpCSetup) return FALSE;
	lpCUIC->GetSelText(GetDlgItem(hPage, IDC_CMB_FONTFACE), lpCSetup->m_UIProp.m_szFontFace, sizeof(lpCSetup->m_UIProp.m_szFontFace));
	lpCSetup->m_UIProp.m_nFontSize = lpCUIC->GetSelInt(GetDlgItem(hPage, IDC_CMB_FONTSIZE));
	lpCSetup->m_UIProp.m_colFore = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_FORECOLOR));
	lpCSetup->m_UIProp.m_colBack = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_BACKCOLOR));
	lpCSetup->m_UIProp.m_colStruct = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_STRUCTCOLOR));
	lpCSetup->m_UIProp.m_colOccurs = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_OCCURSCOLOR));
	lpCSetup->m_UIProp.m_colRedefines = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_REDEFINESCOLOR));
	lpCSetup->m_UIProp.m_colBinary = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_BINCOLOR));
	lpCSetup->m_UIProp.m_colSupplement = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_SUPPCOLOR));
	lpCSetup->m_UIProp.m_colDatCol = lpCUIC->GetSelColor(GetDlgItem(hPage, IDC_CMB_DATCOLOR));
	if(IsDlgButtonChecked(hPage, IDC_CHK_COLBK) == BST_CHECKED){
		lpCSetup->m_UIProp.m_bUseBackColor = TRUE;
	}else{
		lpCSetup->m_UIProp.m_bUseBackColor = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHK_RESTORWND) == BST_CHECKED){
		lpCSetup->m_UIProp.m_bRestorMainWnd = TRUE;
	}else{
		lpCSetup->m_UIProp.m_bRestorMainWnd = FALSE;
	}
	if(IsDlgButtonChecked(hPage, IDC_CHK_ITEMIMAGE) == BST_CHECKED){
		lpCSetup->m_UIProp.m_bItemImage = TRUE;
	}else{
		lpCSetup->m_UIProp.m_bItemImage = FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	ＵＩの設定ページ破棄
----------------------------------------------------------------------------------------*/
LRESULT OnDestroyUIPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	CUICombo *lpCUIC = (CUICombo *)GetProp(hPage, CUICOMBO_CLASS);
	if(lpCUIC){
		RemoveProp(hPage, CUICOMBO_CLASS);
		delete lpCUIC;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	その他の設定
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK SetupOtherPageProc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		return (OnInitOtherPage(hPage, msg, wp, lp));
	case WM_COMMAND:
//		return (OnCommandOtherPage(hPage, msg, wp, lp));
		return FALSE;
	case WM_PAINT:
		return (OnPaintOtherPage(hPage, msg, wp, lp));
	case UM_TPM_SETACTIVE:
		break;
	case UM_TPM_KILLACTIVE:
		break;
	case UM_TPM_APPLY:
		return (OnApplyOtherPage(hPage, msg, wp, lp));
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	その他の設定初期化
----------------------------------------------------------------------------------------*/
LRESULT OnInitOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	その他の設定のコマンド
----------------------------------------------------------------------------------------*/
/*
LRESULT OnCommandOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(LOWORD(wp)){
	case IDC_BTN_LICENCE:
		AuthAboutDlg(hPage, "");
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
*/
/*----------------------------------------------------------------------------------------
	その他の設定の描画
----------------------------------------------------------------------------------------*/
LRESULT OnPaintOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	BeginPaint(hPage, &ps);

	SetBkMode(ps.hdc, TRANSPARENT);
	HFONT hFont, hOldFont;
	char szBuf[1024];

	// タイトル
	HICON hIcon = (HICON)LoadImage((HINSTANCE)GetWindowLongPtr(ghMainWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_ICON01), IMAGE_ICON, 128, 128, LR_DEFAULTCOLOR);
	DrawIconEx(ps.hdc, 28, 42, hIcon, 128, 128, 0, NULL, DI_NORMAL);
	DestroyIcon(hIcon);
	SetTextColor(ps.hdc, RGB(65, 105, 225));
	hFont = MAKEFONT(ps.hdc, 20, FW_BOLD, FALSE, "Noto Sans JP");
	hOldFont = (HFONT)SelectObject(ps.hdc, hFont);
	TextOut(ps.hdc, 176, 54, APP_NAME, strlen(APP_NAME));
	SelectObject(ps.hdc, hOldFont);
	DeleteObject(hFont);

	// バージョン、コピーライト
	SetTextColor(ps.hdc, GetSysColor(COLOR_3DDKSHADOW));
	hFont = MAKEFONT(ps.hdc, 10, FW_BOLD, FALSE, "Noto Sans JP");
	SelectObject(ps.hdc, hFont);
	wsprintf(szBuf, "VERSION %s %s", APP_VERSION, CPU_PLATFORM);
	TextOut(ps.hdc, 182, 98, szBuf, strlen(szBuf));
	wsprintf(szBuf, "COPYRIGHT (C) %s", APP_COPY);
	TextOut(ps.hdc, 182, 118, szBuf, strlen(szBuf));
	SelectObject(ps.hdc, hOldFont);
	DeleteObject(hFont);

	// 権利
	RECT rc = { 40, 174, 414, 500 };
	SetTextColor(ps.hdc, GetSysColor(COLOR_WINDOWTEXT));
	hFont = MAKEFONT(ps.hdc, 10, FW_NORMAL, FALSE, "Yu Gothic UI");
	SelectObject(ps.hdc, hFont);
	DrawText(ps.hdc, ABOUT_MSG, -1, &rc, DT_WORDBREAK);
	SelectObject(ps.hdc, hOldFont);
	DeleteObject(hFont);

	DeleteObject(hOldFont);

	EndPaint(hPage, &ps);
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	その他の設定の保存
----------------------------------------------------------------------------------------*/
LRESULT OnApplyOtherPage(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	// 設定情報
	CYamaSetup *lpCSetup = (CYamaSetup *)lp;
	if(!lpCSetup) return FALSE;
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	スピンボタン作成
----------------------------------------------------------------------------------------*/
HWND SetSpin(HWND hPage, HWND hBuddy, const int nUpper, const int nLower, const int nPos)
{
	return (CreateUpDownControl(WS_CHILD | WS_BORDER | WS_VISIBLE |
						UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_SETBUDDYINT | UDS_NOTHOUSANDS | UDS_WRAP,
						0, 0, 0, 0,
						hPage,
						-1,
//						(HINSTANCE)GetWindowLong(hPage, GWL_HINSTANCE),
						(HINSTANCE)GetWindowLongPtr(hPage, GWLP_HINSTANCE),
						hBuddy,
						nUpper, nLower, nPos));
}
