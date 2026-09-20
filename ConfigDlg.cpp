/*----------------------------------------------------------------------------------------

	ConfigDlg.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "ConfigDlg.h"
#include "resource.h"

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HWND ghMainWnd = NULL;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK ConfigDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
int SetConfigHeader(HWND hListSetting);
int SetSourceListHeader(HWND hListSource);
int SetConfig(HWND hListSetting);
int SetSourceList(HWND hListSource);

/*----------------------------------------------------------------------------------------
	設定確認画面の表示
----------------------------------------------------------------------------------------*/
int ShowConfigBox(HWND hParent, BOOL bFlg/* = TRUE*/)
{
	ghMainWnd = hParent;
//	return (DialogBoxParam((HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE),
	return (DialogBoxParam((HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE),
			MAKEINTRESOURCE(IDD_CONFBOX),
			hParent,
			(DLGPROC)ConfigDlgProc,
			(LPARAM)bFlg));
}

/*----------------------------------------------------------------------------------------
	設定確認画面ダイアログプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK ConfigDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_INITDIALOG:
		if(!(BOOL)lp){
			ShowWindow(GetDlgItem(hDlg, IDOK), SW_HIDE);
			SetDlgItemText(hDlg, IDCANCEL, "閉じる");
		}
		SetConfigHeader(GetDlgItem(hDlg, IDC_LIST_SETTING));
		SetSourceListHeader(GetDlgItem(hDlg, IDC_LIST_SRCLIST));
		SetConfig(GetDlgItem(hDlg, IDC_LIST_SETTING));
		if(SetSourceList(GetDlgItem(hDlg, IDC_LIST_SRCLIST)) < 1) EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wp)){
		case IDOK:
			EndDialog(hDlg, IDOK);
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

/*----------------------------------------------------------------------------------------
	設定情報のリスト設定
----------------------------------------------------------------------------------------*/
int SetConfigHeader(HWND hListSetting)
{
	// 拡張スタイル
	ListView_SetExtendedListViewStyle(hListSetting,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	// イメージリスト
	HIMAGELIST hImgSetting = CreateImageList(ghMainWnd, IDB_BITMAP4, 1, RGB(255, 255, 255));
	ListView_SetImageList(hListSetting, hImgSetting, LVSIL_SMALL);

	RECT rc;
	GetClientRect(hListSetting, &rc);

	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.iSubItem = 0;
	lvcol.cx = 200;
	lvcol.pszText = "設定項目";
	ListView_InsertColumn(hListSetting, 0, &lvcol);
	lvcol.iSubItem = 1;
	lvcol.cx = rc.right - 200 - (GetSystemMetrics(SM_CXEDGE) * 2) - GetSystemMetrics(SM_CYHSCROLL);
	lvcol.pszText = "設定値";
	ListView_InsertColumn(hListSetting, 1, &lvcol);

	return 0;
}

/*----------------------------------------------------------------------------------------
	ソース一覧のリスト設定
----------------------------------------------------------------------------------------*/
int SetSourceListHeader(HWND hListSource)
{
	// 拡張スタイル
	ListView_SetExtendedListViewStyle(hListSource,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	// イメージリスト
	HIMAGELIST hImgSource = CreateImageList(ghMainWnd, IDB_BITMAP3, 3, RGB(255, 255, 255));
	ListView_SetImageList(hListSource, hImgSource, LVSIL_SMALL);

	RECT rc;
	GetClientRect(hListSource, &rc);

	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.iSubItem = 0;
	lvcol.cx = 50;
	lvcol.pszText = "解析順";
	ListView_InsertColumn(hListSource, 0, &lvcol);
	lvcol.iSubItem = 1;
	lvcol.cx = rc.right - 50 - (GetSystemMetrics(SM_CXEDGE) * 2) - GetSystemMetrics(SM_CYHSCROLL);
	lvcol.pszText = "ソース";
	ListView_InsertColumn(hListSource, 1, &lvcol);

	return 0;
}

/*----------------------------------------------------------------------------------------
	設定情報の取得
----------------------------------------------------------------------------------------*/
int SetConfig(HWND hListSetting)
{
	// 設定ファイル操作クラス
	CYamaSetup *lpCSetup = (CYamaSetup *)GetProp(ghMainWnd, CSETUP_CLASS);
	if(!lpCSetup) return -1;

	// リストに設定
	int n = 0;
	char szVal[256];
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	// 行番号の桁数
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "行番号の桁数";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	wsprintf(szVal, "%d 桁", lpCSetup->m_CoreAnalysysProp.m_nSeq);
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = szVal;
	ListView_SetItem(hListSetting, &lvi);
	n++;

	// ソースコードの有効桁数
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "ソースコードの有効桁数";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	if(lpCSetup->m_CoreAnalysysProp.m_nMaxLine == 0){
//		strcpy(szVal, "制限なし");
		strcpy_s(szVal, sizeof(szVal), "制限なし");
	}else{
		wsprintf(szVal, "%d 桁", lpCSetup->m_CoreAnalysysProp.m_nMaxLine);
	}
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = szVal;
	ListView_SetItem(hListSetting, &lvi);
	n++;

	// SMEDファイル識別拡張子
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "SMEDファイル識別拡張子";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = lpCSetup->m_CoreAnalysysProp.m_szSmdExt;
	ListView_SetItem(hListSetting, &lvi);
	n++;

	// 解析エラーとしない記号
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "解析エラーとしない記号";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	wsprintf(szVal, "%c  %c  %c  %c  %c",
		(lpCSetup->m_CoreAnalysysProp.m_szSign[0] ? lpCSetup->m_CoreAnalysysProp.m_szSign[0] : ' '),
		(lpCSetup->m_CoreAnalysysProp.m_szSign[1] ? lpCSetup->m_CoreAnalysysProp.m_szSign[1] : ' '),
		(lpCSetup->m_CoreAnalysysProp.m_szSign[2] ? lpCSetup->m_CoreAnalysysProp.m_szSign[2] : ' '),
		(lpCSetup->m_CoreAnalysysProp.m_szSign[3] ? lpCSetup->m_CoreAnalysysProp.m_szSign[3] : ' '),
		(lpCSetup->m_CoreAnalysysProp.m_szSign[4] ? lpCSetup->m_CoreAnalysysProp.m_szSign[4] : ' '));
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = szVal;
	ListView_SetItem(hListSetting, &lvi);
	n++;

	// PACKED-DECIMAL形式
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "圧縮型変数サイズ計算";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = (lpCSetup->m_CoreAnalysysProp.m_bHostMode ? "全てPACKED-DECIMAL形式" : "32Bit版COBOL形式");
	ListView_SetItem(hListSetting, &lvi);
	n++;

	// 変数位置算出は１Byteから計算する
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "変数位置算出";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	wsprintf(szVal, "先頭を%dByteとする", lpCSetup->m_CoreAnalysysProp.m_nCompPos);
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = szVal;
	ListView_SetItem(hListSetting, &lvi);
	n++;

	// コードチェックを使用する
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = n;
	lvi.iSubItem = 0;
	lvi.pszText = "簡易コードチェック";
	lvi.iImage = 0;
	ListView_InsertItem(hListSetting, &lvi);
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = (lpCSetup->m_CoreAnalysysProp.m_bUseCodeCheck ? "全て実行する" : "必要最低限のみ");
	ListView_SetItem(hListSetting, &lvi);
	n++;

	return n;
}

/*----------------------------------------------------------------------------------------
	ソース一覧の取得
----------------------------------------------------------------------------------------*/
int SetSourceList(HWND hListSource)
{
	// コボル解析クラス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(ghMainWnd, CANACBL_CLASS);
	if(!lpCAnaCBL) return (0L);

	// リストに設定
	int n = lpCAnaCBL->GetSourceCount();
	char szVal[14];
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	if(n > 0){
		for(int i = 0; i < n; i++){
			SOURCEINFO SrcInfo;
			lpCAnaCBL->GetSourceData(i, &SrcInfo);
			wsprintf(szVal, "%02d", SrcInfo.nNo + 1);
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = szVal;
			lvi.iImage = (int)SrcInfo.lpParam;
			ListView_InsertItem(hListSource, &lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			switch((int)SrcInfo.lpParam){
			case SRC_TYPE_CBL:
				lvi.pszText = SrcInfo.szSourcePath;
				break;
			case SRC_TYPE_SMD:
				lvi.pszText = SrcInfo.szParam;
				break;
			case SRC_TYPE_EDIT:
				lvi.pszText = "ユーザ編集コード";
				break;
			default:
				lvi.pszText = "<不明>";
				break;
			}
			ListView_SetItem(hListSource, &lvi);
		}
	}else{
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.pszText = "";
		lvi.iImage = 0;
		ListView_InsertItem(hListSource, &lvi);
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 1;
		lvi.pszText = "<未登録>";
		ListView_SetItem(hListSource, &lvi);
	}

	return n;
}
