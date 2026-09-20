/*----------------------------------------------------------------------------------------

	Property.cpp

----------------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_
#include "Common.h"
#include "Property.h"
#include "resource.h"

/*----------------------------------------------------------------------------------------
	独自メッセージ
----------------------------------------------------------------------------------------*/
#define UM_SETORGWNDPROC				(WM_USER+990)

/*----------------------------------------------------------------------------------------
	グローバル変数
----------------------------------------------------------------------------------------*/
static HWND ghPage[ITEM_PROP_PAGE];

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
void SetStaticLine(HWND hStatic);
LRESULT CALLBACK ItemPropPage1Proc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
LRESULT OnNotifyPropertyList(HWND hPage, UINT msg, WPARAM wp, LPARAM lp);
void InitPropertyList(HWND hWnd, HWND hList);
int SetItemProperty(HWND hWndMain, HWND hPage);
int SetItemPropertyList(HWND hWndMain, HWND hPage);

/*----------------------------------------------------------------------------------------
	プロパティページの表示
----------------------------------------------------------------------------------------*/
int ShowItemProperty(HWND hWnd)
{
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWnd, CANALST_CLASS);
	if(!lpCAnaLst) return -1;
	// 選択項目の識別子を取得
	int nSelItem = lpCAnaLst->GetSel();
	if(nSelItem < 0) return -1; 
	char szItemId[MAX_DESCRIPT_LEN], szPropTitle[260];
//	lpCAnaLst->GetItemId(nSelItem, szItemId);
	lpCAnaLst->GetItemId(nSelItem, szItemId, sizeof(szItemId));
	wsprintf(szPropTitle, "%s のﾌﾟﾛﾊﾟﾃｨ", szItemId);

//	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	PROPSHEETPAGE psp;
	PROPSHEETHEADER psh;
	HPROPSHEETPAGE hpsp[ITEM_PROP_PAGE];

	ZeroMemory(&psp, sizeof(PROPSHEETPAGE));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hInst;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_ITEM_PROPPAGE_1);
	psp.pfnDlgProc = (DLGPROC)ItemPropPage1Proc;
	psp.lParam = (LPARAM)hWnd;
	hpsp[0] = CreatePropertySheetPage(&psp);
	if (!hpsp[0]) {
		DWORD err = GetLastError();
		// エラーコードをログに出力するか確認
	}
	ZeroMemory(&psh, sizeof(PROPSHEETHEADER));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEICONID;
	psh.hInstance = hInst;
	psh.hwndParent = hWnd;
	psh.nPages = ITEM_PROP_PAGE;
	psh.phpage = hpsp;
	psh.pszIcon = MAKEINTRESOURCE(IDI_ICON01);
	psh.pszCaption = szPropTitle;

	return (int)(PropertySheet(&psh));
}

/*----------------------------------------------------------------------------------------
	スタティックテキスト加工
----------------------------------------------------------------------------------------*/
void SetStaticLine(HWND hStatic)
{
	// SS_ETCHEDHORZにする
	LONG lgStyle = GetWindowLong(hStatic, GWL_STYLE);
	SetWindowLong(hStatic, GWL_STYLE, lgStyle | SS_ETCHEDHORZ);
	// 縦ｻｲｽﾞをｼｽﾃﾑのｴｯｼﾞｻｲｽﾞに
	RECT rc;
	GetClientRect(hStatic, &rc);
	SetWindowPos(hStatic, NULL, 0, 0,
		rc.right, GetSystemMetrics(SM_CYEDGE),
		SWP_NOMOVE);
}

/*----------------------------------------------------------------------------------------
	プロパティページ１ダイアログプロシジャ
----------------------------------------------------------------------------------------*/
LRESULT CALLBACK ItemPropPage1Proc(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	static HWND hWndMain;
	switch(msg){
	case WM_INITDIALOG:
		ghPage[0] = hPage;
		hWndMain = (HWND)((LPPROPSHEETPAGE)lp)->lParam;
		SetStaticLine(GetDlgItem(hPage, IDC_STATIC_LINE1));
		SetStaticLine(GetDlgItem(hPage, IDC_STATIC_LINE2));
		SetStaticLine(GetDlgItem(hPage, IDC_STATIC_LINE3));
		InitPropertyList(hPage, GetDlgItem(hPage, IDC_LIST_STATE));
		// 項目属性の設定
		SetItemProperty(hWndMain, hPage);
		break;
    case WM_NOTIFY:
		switch(((LPNMHDR)lp)->idFrom){
		case IDC_LIST_STATE:
			return (OnNotifyPropertyList(hPage, msg, wp, lp));
		default:
			switch(((LPNMHDR)lp)->code){
			case PSN_APPLY:
				break;
			default:
				return FALSE;
			}
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果一覧通知メッセージ処理
----------------------------------------------------------------------------------------*/
LRESULT OnNotifyPropertyList(HWND hPage, UINT msg, WPARAM wp, LPARAM lp)
{
	LPNMLISTVIEW lpNmLv = (LPNMLISTVIEW)lp;
	switch(lpNmLv->hdr.code){
	case LVN_GETINFOTIP:
		break;
	case NM_CUSTOMDRAW:
		switch(((LPNMLVCUSTOMDRAW)lp)->nmcd.dwDrawStage){
		case CDDS_PREPAINT:
//			SetWindowLong(hPage, DWL_MSGRESULT, (LONG)CDRF_NOTIFYITEMDRAW);
			SetWindowLongPtr(hPage, DWLP_MSGRESULT, (LONG_PTR)CDRF_NOTIFYITEMDRAW);
			return TRUE;
		case CDDS_ITEMPREPAINT:
			((LPNMLVCUSTOMDRAW)lp)->clrText = (COLORREF)((LPNMLVCUSTOMDRAW)lp)->nmcd.lItemlParam;
//			SetWindowLong(hPage, DWL_MSGRESULT, (LONG)CDRF_NEWFONT);
			SetWindowLongPtr(hPage, DWLP_MSGRESULT, (LONG_PTR)CDRF_NEWFONT);
			return TRUE;
		default:
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------------------------------
	解析結果一覧の初期化
----------------------------------------------------------------------------------------*/
void InitPropertyList(HWND hWnd, HWND hPropertList)
{
	// 拡張スタイル
	ListView_SetExtendedListViewStyle(hPropertList,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	// イメージリスト
	HIMAGELIST hImgAnalysys = CreateImageList(hWnd, IDB_BITMAP4, 1, RGB(255, 255, 255));
	ListView_SetImageList(hPropertList, hImgAnalysys, LVSIL_SMALL);
	// リストヘッダ作成
	RECT rc;
	GetClientRect(hPropertList, &rc);
	int nX1 = (int)(rc.right * 0.4);
	int nX2 = rc.right - nX1 - 2;
	int pnCol[] = {
		nX1, nX2
	};
	WORD pwCol[] = {
		LVCFMT_LEFT, LVCFMT_LEFT
	};
	char *pszCol[] = {
		"項目名", "内容"
	};
	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int i = 0; i < sizeof(pnCol) / sizeof(int); i++){
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.iSubItem = i;
		lvcol.fmt = pwCol[i];
		lvcol.cx = pnCol[i];
		lvcol.pszText = pszCol[i];
		ListView_InsertColumn(hPropertList, i, &lvcol);
	}
}

/*----------------------------------------------------------------------------------------
	項目属性の設定
----------------------------------------------------------------------------------------*/
int SetItemProperty(HWND hWndMain, HWND hPage)
{
	// COBOL解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWndMain, CANALST_CLASS);
	if(!lpCAnaLst) return -1;

	char szVal[260];
	// 選択項目の情報を取得
	int nSelItem = lpCAnaLst->GetSel();
	if(nSelItem < 0) return -1; 
	RECORD_CBL stRecCBL;
	if(lpCAnaLst->GetItemData(nSelItem, &stRecCBL) < 0) return -1;
	// 識別子
	SetDlgItemText(hPage, IDC_EDIT_ID, stRecCBL.szDescriptor);
	// ｿｰｽ情報の取得
	SOURCEINFO stSrcInf;
	lpCAnaCBL->GetSourceData(stRecCBL.nSrcNo, &stSrcInf);
	// ｿｰｽﾀｲﾄﾙ
	char szSrcTitle[MAX_PATH+1];
	switch((int)stSrcInf.lpParam){
	// COBOLｿｰｽ
	case SRC_TYPE_CBL:
//		strcpy(szVal, stSrcInf.szSourcePath);
		strcpy_s(szVal, sizeof(szVal), stSrcInf.szSourcePath);
		GetFileTitle(szVal, szSrcTitle, sizeof(szSrcTitle));
		SetDlgItemText(hPage, IDC_EDIT_SRCTITLE, szSrcTitle);
		// ｿｰｽの場所
		szVal[strlen(szVal) - strlen(szSrcTitle)] = '\0';
		SetDlgItemText(hPage, IDC_EDIT_SRCPATH, szVal);
		break;
	// FORMｿｰｽ
	case SRC_TYPE_SMD:
//		strcpy(szVal, stSrcInf.szParam);
		strcpy_s(szVal, sizeof(szVal), stSrcInf.szParam);
		GetFileTitle(szVal, szSrcTitle, sizeof(szSrcTitle));
		SetDlgItemText(hPage, IDC_EDIT_SRCTITLE, szSrcTitle);
		// ｿｰｽの場所
		szVal[strlen(szVal) - strlen(szSrcTitle)] = '\0';
		SetDlgItemText(hPage, IDC_EDIT_SRCPATH, szVal);
		break;
	// ﾕｰｻﾞ編集ｺｰﾄﾞ
	case SRC_TYPE_EDIT:
		SetDlgItemText(hPage, IDC_EDIT_SRCTITLE, "(編集されたｿｰｽｺｰﾄﾞ)");
		SetDlgItemText(hPage, IDC_EDIT_SRCPATH, "");
		break;
	}
	// ﾃﾞｨｽｸ上のｻｲｽﾞ
	if(stSrcInf.dwSourceSize > 1024){
		double f1;
		int i1, i2;
		f1 = (double)(stSrcInf.dwSourceSize / 1024);
		i1 = (int)f1;
		i2 = (int)(f1 * 10000.0f) % 10000;
		wsprintf(szVal, "%d.%d KB (%d ﾊﾞｲﾄ)",
			i1, 12, stSrcInf.dwSourceSize);
	}else{
		wsprintf(szVal, "%d ﾊﾞｲﾄ", stSrcInf.dwSourceSize);
	}
	SetDlgItemText(hPage, IDC_ST_SRCSIZE, szVal);
	// ﾚﾍﾞﾙ定義
	wsprintf(szVal, "%02d", stRecCBL.unLevel);
	SetDlgItemText(hPage, IDC_ST_LEVEL, szVal);
	// 変数の型定義
	if(stRecCBL.bStruct){
//		strcpy(szVal, "構造体(集団項目)");
		strcpy_s(szVal, sizeof(szVal), "構造体(集団項目)");
	}else{
		char szTypeChar[100];
		szTypeChar[0] = '\0';
//		if(stRecCBL.dwType & DES_TYPE_UNSIGN) strcat(szTypeChar, "符号無し");
//		if(stRecCBL.dwType & DES_TYPE_SIGN) strcat(szTypeChar, "符号付き");
//		if(stRecCBL.dwType & DES_TYPE_DEC) strcat(szTypeChar, "整数型");
//		if(stRecCBL.dwType & DES_TYPE_DOUBLE) strcat(szTypeChar, " (小数点有)");
//		if(stRecCBL.dwType & DES_TYPE_BIN) strcat(szTypeChar, " (圧縮)");
//		if(stRecCBL.dwType & DES_TYPE_CHAR) strcat(szTypeChar, "文字型");
//		if(stRecCBL.dwType & DES_TYPE_JAPANESE) strcat(szTypeChar, " (2ﾊﾞｲﾄ文字専用)");
		if(stRecCBL.dwType & DES_TYPE_UNSIGN) strcat_s(szTypeChar, "符号無し");
		if(stRecCBL.dwType & DES_TYPE_SIGN) strcat_s(szTypeChar, sizeof(szTypeChar), "符号付き");
		if(stRecCBL.dwType & DES_TYPE_DEC) strcat_s(szTypeChar, sizeof(szTypeChar), "整数型");
		if(stRecCBL.dwType & DES_TYPE_DOUBLE) strcat_s(szTypeChar, sizeof(szTypeChar), " (小数点有)");
		if(stRecCBL.dwType & DES_TYPE_BIN) strcat_s(szTypeChar, sizeof(szTypeChar), " (圧縮)");
		if(stRecCBL.dwType & DES_TYPE_CHAR) strcat_s(szTypeChar, sizeof(szTypeChar), "文字型");
		if(stRecCBL.dwType & DES_TYPE_JAPANESE) strcat_s(szTypeChar, sizeof(szTypeChar), " (2ﾊﾞｲﾄ文字専用)");
		wsprintf(szVal, "%s, %s", stRecCBL.szTypeDef, szTypeChar);
	}
	SetDlgItemText(hPage, IDC_ST_TYPE, szVal);
	// ﾒﾓﾘ上のｻｲｽﾞ
	wsprintf(szVal, "%d ﾊﾞｲﾄ", stRecCBL.dwTypeSize);
	SetDlgItemText(hPage, IDC_ST_SIZE, szVal);
	// ﾚｺｰﾄﾞ先頭からのｵﾌｾｯﾄ
	wsprintf(szVal, "%d ﾊﾞｲﾄ", stRecCBL.dwTypePos);
	SetDlgItemText(hPage, IDC_ST_POS, szVal);

	// 詳細な項目設定
	SetItemPropertyList(hWndMain, hPage);

	return 0;
}

/*----------------------------------------------------------------------------------------
	詳細な項目属性の設定
----------------------------------------------------------------------------------------*/
int SetItemPropertyList(HWND hWndMain, HWND hPage)
{
	HWND hPropertList = GetDlgItem(hPage, IDC_LIST_STATE);
	if(!hPropertList) return -1;

	// COBOL解析クラスインスタンス
	CAnaCBL *lpCAnaCBL = (CAnaCBL *)GetProp(hWndMain, CANACBL_CLASS);
	if(!lpCAnaCBL) return -1;
	// 解析結果リスト制御クラスインスタンス
	CAnalysisList *lpCAnaLst = (CAnalysisList *)GetProp(hWndMain, CANALST_CLASS);
	if(!lpCAnaLst) return -1;

	SendMessage(hPropertList, WM_SETREDRAW, (WPARAM)FALSE, 0L);

	if(ListView_GetItemCount(hPropertList) > 0) ListView_DeleteAllItems(hPropertList);

	int nSelItem = lpCAnaLst->GetSel();
	if(nSelItem >= 0){
		COLORREF colFore = lpCAnaLst->GetColorFore(),
				 colBack = lpCAnaLst->GetColorBack(),	// 未使用
				 colStruct = lpCAnaLst->GetColorStruct(),
				 colOccurs = lpCAnaLst->GetColorOccurs(),
				 colRedefines = lpCAnaLst->GetColorRedefines(),
				 colBinary = lpCAnaLst->GetColorBinary(),
				 colSupplementation = lpCAnaLst->GetColorSupplementation();
		RECORD_CBL RecCbl;
		if(lpCAnaLst->GetItemData(nSelItem, &RecCbl) >= 0){

			// 項目の追加
			char szVal[64];
			int nItem = 0;
			LVITEM lvi;
			ZeroMemory(&lvi, sizeof(LVITEM));
/*
			// レベル
			wsprintf(szVal, "%02d", RecCbl.unLevel);
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.pszText = "レベル";
			lvi.iImage = 0;
			lvi.lParam = (LPARAM)colFore;
			ListView_InsertItem(hPropertList, &lvi);
			wsprintf(szVal, "%02d", RecCbl.unLevel);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = szVal;
			ListView_SetItem(hPropertList, &lvi);
			nItem++;

			// 項目名
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.pszText = "項目名";
			lvi.iImage = 0;
			lvi.lParam = (LPARAM)colFore;
			ListView_InsertItem(hPropertList, &lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = RecCbl.szDescriptor;
			ListView_SetItem(hPropertList, &lvi);
			nItem++;

			// 型定義
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.pszText = "型定義";
			lvi.iImage = 0;
			lvi.lParam = (LPARAM)(RecCbl.bStruct ? colStruct : colFore);
			ListView_InsertItem(hPropertList, &lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = (RecCbl.bStruct ? "<集団項目>" : RecCbl.szTypeDef);
			ListView_SetItem(hPropertList, &lvi);
			nItem++;

			// 大きさ
			wsprintf(szVal, "%d Byte", RecCbl.dwTypeSize);
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.pszText = "大きさ";
			lvi.iImage = 0;
			lvi.lParam = (LPARAM)colFore;
			ListView_InsertItem(hPropertList, &lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = szVal;
			ListView_SetItem(hPropertList, &lvi);
			nItem++;

			// 位置
			wsprintf(szVal, "%d Byte", RecCbl.dwTypePos);
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.pszText = "位置";
			lvi.iImage = 0;
			lvi.lParam = (LPARAM)colFore;
			ListView_InsertItem(hPropertList, &lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = szVal;
			ListView_SetItem(hPropertList, &lvi);
			nItem++;
*/
			// 繰返し
			if(RecCbl.unOccursGroupId > 0){
				if(RecCbl.bOccurs){
					wsprintf(szVal, "%d 回", RecCbl.nOccurs);
					lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
					lvi.iItem = nItem;
					lvi.iSubItem = 0;
					lvi.pszText = "繰返し回数";
					lvi.iImage = 0;
					lvi.lParam = (LPARAM)colOccurs;
					ListView_InsertItem(hPropertList, &lvi);
					lvi.mask = LVIF_TEXT;
					lvi.iSubItem = 1;
					lvi.pszText = szVal;
					ListView_SetItem(hPropertList, &lvi);
					nItem++;
				}

				wsprintf(szVal, "%d 番目", (RecCbl.nOccursOrder + 1));
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "繰返し指数";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colOccurs;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = szVal;
				ListView_SetItem(hPropertList, &lvi);
				nItem++;

				if(strlen(RecCbl.szOccursIndexed) > 0){
					lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
					lvi.iItem = nItem;
					lvi.iSubItem = 0;
					lvi.pszText = "繰返し指数用変数定義";
					lvi.iImage = 0;
					lvi.lParam = (LPARAM)colOccurs;
					ListView_InsertItem(hPropertList, &lvi);
					lvi.mask = LVIF_TEXT;
					lvi.iSubItem = 1;
					lvi.pszText = RecCbl.szOccursIndexed;
					ListView_SetItem(hPropertList, &lvi);
					nItem++;
				}
			}

			// 再定義
			if(RecCbl.unRedefinesGroupId > 0){
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "再定義項目";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colRedefines;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = RecCbl.szRedefines;
				ListView_SetItem(hPropertList, &lvi);
				nItem++;
			}

			// 圧縮
			if(RecCbl.btCompress){
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "圧縮形式";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colBinary;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = RecCbl.szCompress;
				ListView_SetItem(hPropertList, &lvi);
				nItem++;
			}

			// 初期値
			if(strlen(RecCbl.szValue) > 0){
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "初期値";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colFore;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = RecCbl.szValue;
				ListView_SetItem(hPropertList, &lvi);
				nItem++;
			}

			// 追加形式
			if(strlen(RecCbl.szValueEx) > 0){
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "追加形式";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colFore;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = RecCbl.szValueEx;
				ListView_SetItem(hPropertList, &lvi);
				nItem++;
			}

			// コメント
			if(strlen(RecCbl.szComment) > 0){
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "コメント";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colFore;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = RecCbl.szComment;
				ListView_SetItem(hPropertList, &lvi);
				nItem++;
			}

			// 補完項目
			if(RecCbl.nSupplementation > 0){
				char *szSupplementation[] = {
					"ピリオド補完",
					"集団項目補完",
					"FILLER補完",
					"削除項目"
				};
				lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem = nItem;
				lvi.iSubItem = 0;
				lvi.pszText = "補完項目";
				lvi.iImage = 0;
				lvi.lParam = (LPARAM)colSupplementation;
				ListView_InsertItem(hPropertList, &lvi);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				lvi.pszText = szSupplementation[RecCbl.nSupplementation - 1];
				ListView_SetItem(hPropertList, &lvi);
				nItem++;
			}
		}
	}

	SendMessage(hPropertList, WM_SETREDRAW, (WPARAM)TRUE, 0L);

	return 0;
}
