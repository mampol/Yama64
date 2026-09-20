/*----------------------------------------------------------------------------------------

	CAnalysisList.cpp

----------------------------------------------------------------------------------------*/
#include "CAnalysisList.h"
#include "Common.h"

CAnalysisList::CAnalysisList()
: m_hWndParent(NULL), m_hWndList(NULL), m_hWndListHeader(NULL),
  m_lpcProgStbar(NULL),
  m_lpDataFile(NULL), m_nDataPosBase(0),
  m_uiListId(0), m_hImgList(NULL), m_hImgListItem(NULL),
  m_nIconSizeX(0), m_nIconSizeY(0), m_nItemImgWidth(16),
  m_nMaxLevel(999), m_nMinLevel(0),
  m_colFore(RGB(0, 0, 0)),
  m_colBack(RGB(250, 250, 250)),
  m_colStruct(RGB(0, 0, 250)), m_colOccurs(RGB(0, 80, 0)),
  m_colRedefines(RGB(80, 0, 0)), m_colBinary(RGB(80, 0, 80)),
  m_colSupplementation(RGB(80, 80, 80)),
  m_colDatCol(RGB(248, 248, 255)),
  m_bUseBackGround(FALSE), m_bDrawItemImg(FALSE),
  m_bMakeList(FALSE), m_bCancelExpand(FALSE)
{
}

CAnalysisList::~CAnalysisList()
{
	if(m_hWndList){
		if(m_hImgList) ImageList_Destroy(m_hImgList);
		if(m_hImgListItem) ImageList_Destroy(m_hImgListItem);
		HFONT hFont = (HFONT)SendMessage(m_hWndList, WM_GETFONT, 0L, 0L);
		if(hFont) DeleteObject(hFont);
	}
	ClearList();
}

void CAnalysisList::MessageDispatch()
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int CAnalysisList::AddTrace(const char *pszTrace)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(m_hWndParent, CSPLITWND_CLASS);
	if(!lpCWndSplit) return -1;
	lpCWndSplit->SendMessageSplit(UM_ADDOUTPUT, (WPARAM)TRACE_INFO, (LPARAM)pszTrace);
	return 0;
}

// リスト作成
BOOL CAnalysisList::Create(HWND hWndParent, UINT uiListImgRes/* = 0*/, UINT uiListId/* = ID_TREELIKELIST*/)
{
	CWinTheme CWt;
	if(m_hWndList) DestroyWindow(m_hWndList);
	m_lpcProgStbar = NULL;
	m_hWndList = CreateWindowEx(WS_EX_CLIENTEDGE,
		WC_LISTVIEW,
		"Analysys_ListView",
		WS_CHILD | WS_TABSTOP |
		LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED | LVS_SHAREIMAGELISTS,
		0, 0, 0, 0,
		hWndParent,
		(HMENU)uiListId,
//		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE),
		(HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
		NULL);
	if(!m_hWndList) return FALSE;
	m_uiListId = uiListId;
//	WORD wOsVer = GetVersionNumber();
//	if(LOWORD(wOsVer) >= 5){
//		// Vistaスタイルへ
//		ListView_SetExtendedListViewStyle(m_hWndList,
//			LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER);
//		if(CWt.IsThemeAct()) CWt.SetTheme(m_hWndList, "explorer");
//	}else{
//		ListView_SetExtendedListViewStyle(m_hWndList,
//			LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
//	}
	ListView_SetExtendedListViewStyle(m_hWndList,
		LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER);
	if(CWt.IsThemeAct()) CWt.SetTheme(m_hWndList, "explorer");
	m_hWndParent = hWndParent;
	// ヘッダハンドル
	m_hWndListHeader = ListView_GetHeader(m_hWndList);
	// リストカラム作成
	CreateColumn();
/*
	// イメージリスト作成
	if(!SetImageList()) return FALSE;
*/
	// リストアイテム用イメージリスト作成
	if(uiListImgRes > 0) CreateItemImageList(uiListImgRes);

	return TRUE;
}

// フォント設定
BOOL CAnalysisList::SetAnalstFont(const int nFontSize, const char *pszFontFace)
{
	if(!m_hWndList) return FALSE;
	// フォント作成
	HDC hDC = GetDC(m_hWndList);
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
//	strcpy(lf.lfFaceName, pszFontFace);
	strcpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), pszFontFace);
	HFONT hNewFont = CreateFontIndirect(&lf);
	ReleaseDC(m_hWndList, hDC);
	// 既に設定済みなら一旦削除
	HFONT hOldFont = (HFONT)SendMessage(m_hWndList, WM_GETFONT, 0L, 0L);
	if(hOldFont) DeleteObject(hOldFont);
	// フォント設定
	SendMessage(m_hWndList, WM_SETFONT, (WPARAM)hNewFont, MAKELPARAM(TRUE, 0));
	// イメージリスト設定
	SetImageList();
	// なんとなく、更新
	ListView_Arrange(m_hWndList, LVA_DEFAULT);
	return TRUE;
}
/*
int CALLBACK CAnalysisList::GetFontInfo(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	CAnalysisList *lpcAnaList = (CAnalysisList *)lParam;
	if(!lpcAnaList) return 0;
	if(FontType & TRUETYPE_FONTTYPE){
		lpcAnaList->m_nIconSizeX = max(lpntme->ntmTm.tmAveCharWidth, 18);
		lpcAnaList->m_nIconSizeY = max(lpntme->ntmTm.tmHeight, 18);
	}else{
		// TRUE TYPE以外はキャストしちゃう？？
		TEXTMETRIC *ptm = (TEXTMETRIC *)lpntme;
		lpcAnaList->m_nIconSizeX = max(ptm->tmAveCharWidth, 18);
		lpcAnaList->m_nIconSizeY = max(ptm->tmHeight, 18);
	}
	return 0;	// 先頭で終わり
}
*/
BOOL CAnalysisList::Move(LPRECT lprc)
{
	if(!m_hWndList) return FALSE;
	return (MoveWindow(m_hWndList,
		lprc->left, lprc->top,
		lprc->right - lprc->left, lprc->bottom - lprc->top,
		TRUE));
}

BOOL CAnalysisList::SetImageList()
{
	if(!m_hWndList) return FALSE;

	// 幅、高さ計算
	HDC hDC = GetDC(m_hWndList);
	if(!hDC) return FALSE;
/*	ListViewはGetTextMetricsがうまくいかない
	TEXTMETRIC tm;
	ZeroMemory(&tm ,sizeof(TEXTMETRIC));
	GetTextMetrics(hDC, &tm);
	m_nIconSizeX = max(tm.tmAveCharWidth, 17);
	m_nIconSizeY = max((tm.tmHeight + 2), 17);
*/
	RECT rcItem;
	GetClientRect(m_hWndListHeader, &rcItem);
	// ヘッダーの縦幅に合わせる
	m_nIconSizeX = m_nIconSizeY = max(rcItem.bottom, 18);
	if(!(m_nIconSizeX % 2)) m_nIconSizeX++;
	if(!(m_nIconSizeY % 2)) m_nIconSizeY++;

	// イメージリスト作成
	// (0 = ｜、1-3 = └ 、 4-6 = ├、 7-9 = ┬、 10-12 = ┌(線有ルート)、 13-15 = ┌(線無ルート))
	RECT rc;
	SetRect(&rc,
		0, 0, m_nIconSizeX * 16, m_nIconSizeY);
	if(m_hImgList) ImageList_Destroy(m_hImgList);
	m_hImgList = ImageList_Create(m_nIconSizeX, m_nIconSizeY, ILC_COLOR24 | ILC_MASK, 16, 0);
	if(!m_hImgList) return FALSE;

	// ビットマップ作成
	HDC hmDC = CreateCompatibleDC(hDC);
	HBITMAP hmBmp = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hmDC, hmBmp);

	// hDCはもういい
	ReleaseDC(m_hWndList, hDC);
	
	// ツリーイメージ作成
	HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW)),
		hOldBrush;
	HPEN hWndPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW)),
		hBtnPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW)),
		hTextPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNTEXT)),
		hOldPen;

	// 塗りつぶし
	FillRect(hmDC, &rc, hBrush);

	// ｜、└、├、┐、┌を書く(13-15は□のみ)
	int i, nx, ny, n;
	hOldBrush = (HBRUSH)SelectObject(hmDC, hBrush);
	hOldPen = (HPEN)SelectObject(hmDC, hBtnPen);
	for(i = 0; i < 13; i++){
		// 縦
		nx = (m_nIconSizeX / 2) + (i * m_nIconSizeX);
		if(i < 7){
			ny = 0;
		}else{
			ny = m_nIconSizeY / 2;
		}
		if(i < 4){
			// 0は「｜」のみ
			if(i == 0){
				n = m_nIconSizeY;
			}else{
				n = m_nIconSizeY / 2;
			}
		}else{
			n = m_nIconSizeY;
		}
		// 見栄えの問題で+1
		n++;
		MoveToEx(hmDC, nx, ny, NULL);
		for(; ny < n; ny++) LineTo(hmDC, nx, ny);
		// 横
		if(i == 0) continue;
		if(i < 7 || i > 9){
			nx = (m_nIconSizeX / 2) + (i * m_nIconSizeX);
		}else{
			nx = i * m_nIconSizeX;
		}
		ny = m_nIconSizeY / 2;
		if(i < 7 || i > 9){
			n = m_nIconSizeX * (i + 1);
		}else{
			n = (m_nIconSizeX * (i + 1)) - (m_nIconSizeX / 2);
		}
		MoveToEx(hmDC, nx, ny, NULL);
		for(; nx < n; nx++) LineTo(hmDC, nx, ny);
	}
	// □を書く
	SelectObject(hmDC, hBtnPen);
	int nOffsetX = max((m_nIconSizeX / 3), 4), nOffsetY = max((m_nIconSizeY / 3), 4);
	if(m_nIconSizeY > m_nIconSizeX){
		nOffsetY += (m_nIconSizeY - m_nIconSizeX) / 2;
	}else{
		nOffsetX += (m_nIconSizeX - m_nIconSizeY) / 2;
	}
	for(i = 0; i < 16; i++){
		if(i == 0) continue;
		if(i == 1 || i == 4 || i == 7 || i == 10) continue;
		Rectangle(hmDC,
		(i * m_nIconSizeX) + nOffsetX, nOffsetY,
		((i + 1) * m_nIconSizeX) - nOffsetX, (m_nIconSizeY - nOffsetY));
	}
	nOffsetX += 2;
	nOffsetY += 2;
	// ＋又は－を書く
	SelectObject(hmDC, hTextPen);
	for(i = 0; i < 16; i++){
		if(i == 0) continue;
		if(i == 1 || i == 4 || i == 7 || i == 10 || i == 13) continue;
		MoveToEx(hmDC, ((i * m_nIconSizeX) + nOffsetX), (m_nIconSizeY / 2), NULL);
		LineTo(hmDC, ((i + 1) * m_nIconSizeX) - nOffsetX, (m_nIconSizeY / 2));
		if(i == 2 || i == 5 || i == 8 || i == 11 || i == 14) continue;
		MoveToEx(hmDC, (i * m_nIconSizeX) + (m_nIconSizeX / 2), nOffsetY, NULL);
		LineTo(hmDC, (i * m_nIconSizeX) + (m_nIconSizeX / 2), (m_nIconSizeY - nOffsetY));
	}
	SelectObject(hmDC, hOldBmp);	// ここでデバイスコンテキストから外しておかないとイメージリストがヴっこわれる

	// イメージリストに追加
	if(ImageList_AddMasked(m_hImgList, hmBmp, (COLORREF)GetSysColor(COLOR_WINDOW)) < 0) MessageBox(m_hWndParent, "ImageList作成失敗", "ｴﾗｰ", MB_OK | MB_ICONSTOP);
	int nImg = ImageList_GetImageCount(m_hImgList);

#ifdef _DEBUG_MODE
	HDC hDeskDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	for(i = 0; i < 16; i++){
		if(!ImageList_Draw(m_hImgList, i, hDeskDC, m_nIconSizeX * i, m_nIconSizeY, ILD_TRANSPARENT)){
			DWORD dwErr = GetLastError();
		}
	}
	DeleteDC(hDeskDC);
#endif

	// GDIオブジェクト破棄
	SelectObject(hmDC, hOldBrush);
	SelectObject(hmDC, hOldPen);
	DeleteObject(hBrush);
	DeleteObject(hWndPen);
	DeleteObject(hBtnPen);
	DeleteObject(hTextPen);

	// クリーンアップ
	DeleteObject(hmBmp);
	DeleteDC(hmDC);

	// リストに設定
	ListView_SetImageList(m_hWndList, m_hImgList, LVSIL_SMALL);

	return TRUE;
}

HIMAGELIST CAnalysisList::CreateItemImageList(UINT uiImgBmp, const int nImgWidth/* = 16*/, COLORREF colMask/* = RGB(0, 0, 0)*/)
{
//	HBITMAP hBmp = (HBITMAP)LoadImage((HINSTANCE)GetWindowLong(m_hWndParent, GWL_HINSTANCE),
	HBITMAP hBmp = (HBITMAP)LoadImage((HINSTANCE)GetWindowLongPtr(m_hWndParent, GWLP_HINSTANCE),
										MAKEINTRESOURCE(uiImgBmp),
										IMAGE_BITMAP,
										0, 0,
										LR_SHARED);
	if(!hBmp) return NULL;
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	int nIImg = bmp.bmWidth / nImgWidth;
	if(nIImg < MAX_IIMAGE){
		DeleteObject(hBmp);
		MessageBox(m_hWndParent, "イメージリソースのロードエラー", "CAnalisysList", MB_OK);
		return NULL;
	}
	if(m_hImgListItem) ImageList_Destroy(m_hImgListItem);
	m_hImgListItem = ImageList_Create(nImgWidth, bmp.bmHeight,
										ILC_COLOR16 | ILC_MASK, nIImg, 0);
	if(m_hImgListItem) ImageList_AddMasked(m_hImgListItem, hBmp, colMask);
	DeleteObject(hBmp);
	m_nItemImgWidth = nImgWidth;
	return m_hImgListItem;
}

void CAnalysisList::CreateColumn()
{
	if (!m_hWndList) return;

	WORD wFmt[MAX_LVCOLUMN] = {
		LVCFMT_LEFT,
		LVCFMT_LEFT,
		LVCFMT_LEFT,
		LVCFMT_RIGHT,
		LVCFMT_RIGHT,
		LVCFMT_LEFT
	};

	int nWidth[MAX_LVCOLUMN] = {
		150,
		240,
		80,
		90,
		110,
		240
	};

	char szColText[MAX_LVCOLUMN][13] = {
		"レベル",
		"項目名",
		"型定義",
		"サイズ",
		"オフセット",
		"データ"
	};

	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));

	for (int i = 0; i < MAX_LVCOLUMN; i++) {
		lvcol.mask =
			LVCF_FMT |
			LVCF_SUBITEM |
			LVCF_WIDTH |
			LVCF_TEXT |
			LVCF_ORDER;

		lvcol.fmt = wFmt[i];
		lvcol.iSubItem = i;
		lvcol.cx = nWidth[i];
		lvcol.pszText = szColText[i];
		lvcol.iOrder = i;

		ListView_InsertColumn(
			m_hWndList,
			i,
			&lvcol);
	}
}

void CAnalysisList::SetColText(const int nCol, char *pszText)
{
	if(!m_hWndList) return;

	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_TEXT;
	lvcol.pszText = pszText;
	ListView_SetColumn(m_hWndList, nCol, &lvcol);
}

void CAnalysisList::SetColFmt(const int nCol, WORD wFmt)
{
	if(!m_hWndList) return;

	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT;
	lvcol.fmt = wFmt;
	ListView_SetColumn(m_hWndList, nCol, &lvcol);
}

void CAnalysisList::SetColWidth(const int nCol, int nWidth)
{
	if(!m_hWndList) return;

	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_WIDTH;
	lvcol.cx = nWidth;
	ListView_SetColumn(m_hWndList, nCol, &lvcol);
}

void CAnalysisList::SetColPos(const int nColFrom, const int nColTo)
{
	if(!m_hWndList) return;
	if(!m_hWndListHeader) m_hWndListHeader = ListView_GetHeader(m_hWndList);
	int nTo = min((Header_GetItemCount(m_hWndListHeader) - 1), nColTo);
	LVCOLUMN lvcol;
	ZeroMemory(&lvcol, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_ORDER;
	lvcol.iOrder = nTo;
	ListView_SetColumn(m_hWndList, nColFrom, &lvcol);
}

WORD CAnalysisList::GetColFmt(const int nCol)
{
	if(!m_hWndListHeader) m_hWndListHeader = ListView_GetHeader(m_hWndList);

	HDITEM hdi;
	ZeroMemory(&hdi, sizeof(HDITEM));
	hdi.mask = HDI_FORMAT;
	Header_GetItem(m_hWndListHeader, nCol, &hdi);
	return (hdi.fmt);
}

int CAnalysisList::GetColWidth(const int nCol)
{
	if(!m_hWndListHeader) m_hWndListHeader = ListView_GetHeader(m_hWndList);

	HDITEM hdi;
	ZeroMemory(&hdi, sizeof(HDITEM));
	hdi.mask = HDI_WIDTH;
	Header_GetItem(m_hWndListHeader, nCol, &hdi);
	return (hdi.cxy);
}

BOOL CAnalysisList::GetColRect(const int nCol, LPRECT lprc)
{
	if(!m_hWndList) return FALSE;

	if(!m_hWndListHeader) m_hWndListHeader = ListView_GetHeader(m_hWndList);
	return (Header_GetItemRect(m_hWndListHeader, nCol, lprc));
}

/*
void CAnalysisList::ClearList()
{
	for(std::vector<LPTREEINFO>::size_type i = 0; i < m_lpTreeInfo.size(); i++){
		if(m_lpTreeInfo[i]) GlobalFree(m_lpTreeInfo[i]);
	}
	m_lpTreeInfo.clear();
	if(m_hWndList) ListView_DeleteAllItems(m_hWndList);
	m_bMakeList = FALSE;
}
*/
void CAnalysisList::ClearList()
{
	//--------------------------------------------------
	// ListView更新停止
	//--------------------------------------------------
	if (m_hWndList) {

		SendMessage(
			m_hWndList,
			WM_SETREDRAW,
			FALSE,
			0);

		//--------------------------------------------------
		// 先にListViewを空にする
		//--------------------------------------------------
		ListView_DeleteAllItems(
			m_hWndList);
	}

	//--------------------------------------------------
	// その後でTreeInfoを解放
	//--------------------------------------------------
	for (std::vector<LPTREEINFO>::size_type i = 0;
		i < m_lpTreeInfo.size();
		i++)
	{
		if (m_lpTreeInfo[i]) {

			GlobalFree(
				m_lpTreeInfo[i]);
		}
	}

	m_lpTreeInfo.clear();

	m_bMakeList = FALSE;

	//--------------------------------------------------
	// 再描画再開
	//--------------------------------------------------
	if (m_hWndList) {

		SendMessage(
			m_hWndList,
			WM_SETREDRAW,
			TRUE,
			0);

		InvalidateRect(
			m_hWndList,
			NULL,
			TRUE);
	}
}

void CAnalysisList::SetRedraw(const BOOL bFlg)
{
	if(!m_hWndList) return;
	SendMessage(m_hWndList, WM_SETREDRAW, (WPARAM)bFlg, 0L);
}

int CAnalysisList::InitListItem(std::vector<LPRECORD_CBL> &lpRecCbl,
								const unsigned int unMaxLevel/* = 999*/,
								const unsigned int unMinLevel/* = 0*/)
{
	int nCount = 0;
	// 初期化
	ClearList();
	//--------------------------------------------------
	// 解析結果なし
	//--------------------------------------------------
	if (lpRecCbl.empty()) {
		return 0;
	}
	if (!lpRecCbl[0]) {
		return 0;
	}
	// 最大、最小レベル番号
	m_nMaxLevel = unMaxLevel;
	m_nMinLevel = unMinLevel;
	if(lpRecCbl[0]->unLevel > m_nMinLevel){
		m_nMinLevel = lpRecCbl[0]->unLevel;
		AddTrace("先頭項目よりも小さいﾚﾍﾞﾙ定義の項目があります..."
			"ｿｰｽを直さないと展開表示がおかしくなります...");
	}
	// レベル別カウント
	for(int l = 0; l < (sizeof(m_unLevelCount) / sizeof(unsigned int)); l++) m_unLevelCount[l] = 0;
	// 配列設定
	for(std::vector<LPRECORD_CBL>::size_type i = 0; i < lpRecCbl.size(); i++){
		LPTREEINFO lpTreeInfo = (LPTREEINFO)GlobalAlloc(GPTR, sizeof(TREEINFO));
		if(!lpTreeInfo) break;
		lpTreeInfo->lpRec = lpRecCbl[i];
		lpTreeInfo->nIndent = 0;
		lpTreeInfo->nChild = 0;
		lpTreeInfo->nAllChild = 0;
		lpTreeInfo->bExpand = FALSE;
		m_lpTreeInfo.push_back(lpTreeInfo);
		nCount++;
	}
	if(nCount > 0){
		unsigned int unLevel = m_lpTreeInfo[0]->lpRec->unLevel;
		// 子供ノードの数、各レベルの数を数える
		std::vector<LPTREEINFO>::size_type i, nSize = m_lpTreeInfo.size();
		for(i = 0; i < nSize; i++){
			m_lpTreeInfo[i]->nChild = GetItemChild((int)i);
			m_unLevelCount[m_lpTreeInfo[i]->lpRec->unLevel]++;
		}
		// 子供ノードの数を集計
		for(i = 0; i < nSize; i++){
			if(m_lpTreeInfo[i]->lpRec->unLevel > unLevel) continue;
			GetItemAllChild((int)i);
		}
		// インデント設定
		for(i = 0; i < nSize; i++){
			m_lpTreeInfo[i]->nIndent = GetTreeIndent(m_lpTreeInfo[i]->lpRec->unLevel);
		}
		// 先頭レベルまで表示
		for(i = 0; i < nSize; i++){
			if(m_lpTreeInfo[i]->lpRec->unLevel > m_nMinLevel) continue;
			AddListItem((int)i);
		}
		m_bMakeList = TRUE;
	}
	return nCount;
}

int CAnalysisList::GetTreeIndent(const unsigned int unLevel)
{
	int nIndent = 0;
	for(unsigned int i = 0; i < unLevel; i++){
		if(m_unLevelCount[i] > 0) nIndent++;
	}
/*
	int nIndent = 10;
	switch(unLevel){
	case 0:
	case 1:
	case 2:
		nIndent = 0;
		break;
	case 3:
	case 4:
		nIndent = 1;
		break;
	case 5:
	case 6:
		nIndent = 2;
		break;
	case 7:
	case 8:
		nIndent = 3;
		break;
	case 9:
	case 10:
		nIndent = 4;
		break;
	case 11:
	case 12:
		nIndent = 5;
		break;
	case 13:
	case 14:
		nIndent = 6;
		break;
	case 15:
	case 16:
		nIndent = 7;
		break;
	case 17:
	case 18:
		nIndent = 8;
		break;
	case 19:
	case 20:
		nIndent = 9;
		break;
	}
*/
	return nIndent;
}

int CAnalysisList::GetItemChild(const int nTreeInfo)
{
	// 集団項目のみ
	if(!m_lpTreeInfo[nTreeInfo]->lpRec->bStruct) return 0;

	int nFind = nTreeInfo + 1, nChild = 0, nSize = m_lpTreeInfo.size();
	if(nSize <= nFind) return 0;
	// レベル
	unsigned int unLevel = m_lpTreeInfo[nFind]->lpRec->unLevel;
	for(int i = nFind; i < nSize; i++){
		unsigned int unNextLevel = m_lpTreeInfo[i]->lpRec->unLevel;
		// 上位レベルがきたらオワリ
		if(unLevel > unNextLevel) break;
		// 下位レベルがきたらスキップ
		if(unLevel < unNextLevel) continue;
		nChild++;
	}
	return nChild;
}

int CAnalysisList::GetItemAllChild(const int nTreeInfo)
{
	// 集団項目のみ
	if(!m_lpTreeInfo[nTreeInfo]->lpRec->bStruct) return 0;
	m_lpTreeInfo[nTreeInfo]->nAllChild = m_lpTreeInfo[nTreeInfo]->nChild;
	// レベル
	unsigned int unLevel = m_lpTreeInfo[nTreeInfo]->lpRec->unLevel;
	int nSize = (int)m_lpTreeInfo.size();
	for(int i = (nTreeInfo + 1); i < nSize; i++){
		if(unLevel >= m_lpTreeInfo[i]->lpRec->unLevel) break;
		m_lpTreeInfo[nTreeInfo]->nAllChild += GetItemAllChild(i);
	}
	return (m_lpTreeInfo[nTreeInfo]->nChild);
}

int CAnalysisList::GetTreeImageIndex(const int nTreeInfo)
{
	int nSize = (int)m_lpTreeInfo.size();
	// ノードが１つだけ
	if(nSize == 1) return 13;
	// 最後
	if((nTreeInfo + 1) >= nSize) return 1;

	// (0 = ｜、1-3 = └ 、 4-6 = ├、 7-9 = ┬、 10-12 = ┌(線有ルート)、 13-15 = ┌(線無ルート))
	int nImgImdex = 1;
	// 今と次のレベル
	unsigned int unLevel = m_lpTreeInfo[nTreeInfo]->lpRec->unLevel,
				 unNexLevel = m_lpTreeInfo[nTreeInfo + 1]->lpRec->unLevel;
	if(nTreeInfo == 0){
		// ルート
		nImgImdex = 13;
		for(int i = nTreeInfo + 1; i < nSize; i++){
			// 以降に同位レベルがある場合はまだレコードは続く
			if(unLevel == m_lpTreeInfo[i]->lpRec->unLevel){
				nImgImdex = 10;
				break;
			}
		}
	}else if(unLevel == unNexLevel){
		// 次アイテムが同位レベル
		nImgImdex = 4;
	}else if(unLevel > unNexLevel){
		// 次アイテムが上位レベル
		nImgImdex = 1;
	}else{
		// 次アイテムが下位レベル
		nImgImdex = 1;
		for(int i = nTreeInfo + 1; i < nSize; i++){
			unsigned int unNextLevel = m_lpTreeInfo[i]->lpRec->unLevel;
			// 以降に上位レベルがある場合はまだレコードは続くケド、関連はない
			if(unLevel > unNextLevel) break;
			// 以降に同位レベルがある場合はまだレコードは続く
			if(unLevel == unNextLevel){
				nImgImdex = 4;
				break;
			}
		}
	}
	// 集団項目
	if(m_lpTreeInfo[nTreeInfo]->lpRec->bStruct) nImgImdex += 2;
	// 展開済み
	if(m_lpTreeInfo[nTreeInfo]->bExpand) nImgImdex--;
	return nImgImdex;
}

int CAnalysisList::ListIndex2TreeInfoIndex(const int nItem)
{
	if(!m_hWndList) return -1;
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nItem;
	ListView_GetItem(m_hWndList, &lvi);
	return ((int)lvi.lParam);
}

int CAnalysisList::TreeInfoIndex2ListIndex(const int nTreeInfo)
{
	if(!m_hWndList) return -1;
	int nCount = GetCount();
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;
	for(int nItem = 0; nItem < nCount; nItem++){
		lvi.iItem = nItem;
		ListView_GetItem(m_hWndList, &lvi);
		if(nTreeInfo == (int)lvi.lParam) return nItem;
	}
	return -1;
}

int CAnalysisList::ExpandItem(const int nItem, BOOL bSetSel/* = TRUE*/)
{
	if(!m_hWndList) return -1;
	if(nItem < 0) return -1;

	int nTreeInfo = ListIndex2TreeInfoIndex(nItem), nExpand = 0;
	if(nTreeInfo < 0) return -1;

	// 集団項目のみ
	if(!m_lpTreeInfo[nTreeInfo]->lpRec->bStruct){
		if(bSetSel){
			if(nItem < (ListView_GetItemCount(m_hWndList) - 1)) SetSel(nItem + 1);
		}
		return 0;
	}
	// 展開済み
	if(m_lpTreeInfo[nTreeInfo]->bExpand){
		if(bSetSel){
			if(nItem < (ListView_GetItemCount(m_hWndList) - 1)) SetSel(nItem + 1);
		}
		return 0;
	}
	// フラグオフ
	m_bCancelExpand = FALSE;
	// 範囲外
	if((int)(m_lpTreeInfo.size() - 1) > nTreeInfo){
		// 更新停止
		SetRedraw(FALSE);
/*
		// トレース
		char szTraceMsg[512];
		wsprintf(szTraceMsg, 
			"[%02d %s]の展開中...",
			m_lpTreeInfo[nTreeInfo]->lpRec->unLevel,
			m_lpTreeInfo[nTreeInfo]->lpRec->szDescriptor);
		AddTrace(szTraceMsg);
*/
		// 展開
		int nChild = m_lpTreeInfo[nTreeInfo]->nChild,
			nAllChild = m_lpTreeInfo[nTreeInfo]->nAllChild,
			nInstItem = nItem + 1,
			nExpandTreeInfo = nTreeInfo + 1;
		if(m_lpTreeInfo[nExpandTreeInfo]->lpRec){
			unsigned int unLevel = m_lpTreeInfo[nExpandTreeInfo]->lpRec->unLevel;
			int i = 0;
			if(nAllChild > 1000){
				char szTraceMsg[512];
				wsprintf(szTraceMsg,
					"[%s] ﾉｰﾄﾞｺﾞｳｹｲ 1000 Over!! Σ(ﾟДﾟ;)",
					m_lpTreeInfo[nTreeInfo]->lpRec->szDescriptor);
				AddTrace(szTraceMsg);
			}
			while(i < nChild){
				// キャンセル？
	//			MessageDispatch();
				if(m_bCancelExpand){
					AddTrace("Σ(ﾟДﾟ;) ﾁｭｳｼ!?");
					break;
				}
				if((int)m_lpTreeInfo.size() <= nExpandTreeInfo) break;
				if(unLevel == m_lpTreeInfo[nExpandTreeInfo]->lpRec->unLevel){
					// 同じレベルならリストアイテム追加
					nInstItem = InsertListItem(nInstItem, nExpandTreeInfo);
					nInstItem++;
					i++;
					if(m_lpcProgStbar) m_lpcProgStbar->StepProg();
				}
				nExpandTreeInfo++;
				nExpand++;
			}
		}
		// 展開フラグ
		if(nExpand > 0){
			m_lpTreeInfo[nTreeInfo]->bExpand = TRUE;
			UpdateListItem(nItem);
		}
		// 更新再開
		SetRedraw(TRUE);
	}
	return nExpand;
}

int CAnalysisList::ExpandAllItems(const int nItem, BOOL bSetSel/* = FALSE*/)
{
	if(!m_hWndList) return -1;

	int nExpandItem = nItem;
	int nTreeInfo = ListIndex2TreeInfoIndex(nExpandItem);
	if(nTreeInfo < 0) return -1;
	unsigned int unLevel = m_lpTreeInfo[nTreeInfo]->lpRec->unLevel;
	if(m_lpTreeInfo[nTreeInfo]->nAllChild > 1000){
		if(MessageBox(m_hWndParent,
			"展開するにはかなり時間がかかりそうです\n続行しますか？", "警告",
			MB_YESNO | MB_ICONEXCLAMATION) == IDNO) return 0;
		AddTrace("ι(´Д｀υ) ｲｿｶﾞｼｲ...");
	}

	while(1){
		// キャンセル？(メッセージディスパッチはExpandItem関数内で)
		if(m_bCancelExpand){
			AddTrace("Σ(ﾟДﾟ;) ﾁｭｳｼ!?");
			break;
		}
		ExpandItem(nExpandItem, bSetSel);
		nExpandItem++;
		if(nExpandItem > GetCount()) break;
		nTreeInfo = ListIndex2TreeInfoIndex(nExpandItem);
		if(nTreeInfo < 0) return -1;
		if(unLevel >= m_lpTreeInfo[nTreeInfo]->lpRec->unLevel) break;
	}
	return nExpandItem;
}

int CAnalysisList::CollapseItem(const int nItem, BOOL bSetSel/* = TRUE*/)
{
	if(!m_hWndList) return -1;
	if(nItem < 0) return -1;

	int nTreeInfo = ListIndex2TreeInfoIndex(nItem), nCollapse = 0;
	if(nTreeInfo < 0) return -1;

	// 集団項目のみ
	if(!m_lpTreeInfo[nTreeInfo]->lpRec->bStruct){
		if(bSetSel){
			if(nItem > 0) SetSel(nItem - 1);
		}
		return 0;
	}
	// 未展開
	if(!m_lpTreeInfo[nTreeInfo]->bExpand){
		if(bSetSel){
			if(nItem > 0) SetSel(nItem - 1);
		}
		return 0;
	}
	// フラグオフ
	m_bCancelExpand = FALSE;
	// 範囲外
	if((int)m_lpTreeInfo.size() > nTreeInfo){
		// 更新停止
		SetRedraw(FALSE);
/*
		// トレース
		char szTraceMsg[512];
		wsprintf(szTraceMsg, 
			"[%02d %s]を閉じてます...",
			m_lpTreeInfo[nTreeInfo]->lpRec->unLevel,
			m_lpTreeInfo[nTreeInfo]->lpRec->szDescriptor);
		AddTrace(szTraceMsg);
*/
		// 閉じる
		int nChild = m_lpTreeInfo[nTreeInfo]->nChild,
			nAllChild = m_lpTreeInfo[nTreeInfo]->nAllChild,
			nDeleteItem = nItem + 1,
			nCollapseTreeInfo = nTreeInfo + 1;
		unsigned int unLevel = m_lpTreeInfo[nCollapseTreeInfo]->lpRec->unLevel;
		int i = 0;
//		if(nAllChild > 1000) AddTrace("Σ(ﾟДﾟ;) ﾉｰﾄﾞｺﾞｳｹｲ 1000 Over!!");
		while(i < nChild){
			// キャンセル？
//			MessageDispatch();
			if(m_bCancelExpand){
				AddTrace("キャンセルします");
				break;
			}
			// 展開ノードがあればそれも閉じる
			if(m_lpTreeInfo[nCollapseTreeInfo]->bExpand) CollapseItem(nItem + 1);
			if(unLevel == m_lpTreeInfo[nCollapseTreeInfo]->lpRec->unLevel){
				// 同じレベルならリストアイテム削除
				DeleteListItem(nDeleteItem);
				i++;
				if(m_lpcProgStbar) m_lpcProgStbar->StepProg();
			}
			nCollapseTreeInfo++;
			nCollapse++;
		}
		// 展開フラグ
		if(nCollapse > 0){
			m_lpTreeInfo[nTreeInfo]->bExpand = FALSE;
			UpdateListItem(nItem);
		}
		// 更新再開
		SetRedraw(TRUE);
	}
	return nCollapse;
}

int CAnalysisList::ExpandLevelItem(const unsigned unLevel,
								   BOOL bNoRedefines,
								   BOOL bNoOccurs)
{
	if(!m_hWndList) return 0;
	int nItems = 0;
	AddTrace("レコード展開開始");
	std::vector<LPTREEINFO>::size_type i, nSize = m_lpTreeInfo.size();
	// ﾌﾟﾛｸﾞﾚｽ
	if(m_lpcProgStbar) m_lpcProgStbar->ShowProg(TRUE);
	for(i = 0; i < nSize; i++){
		// キャンセル？
		if(m_bCancelExpand){
			AddTrace("中断しました");
			break;
		}
		// 指定されたレベル以上のノードを処理
		if(m_lpTreeInfo[i]->lpRec->unLevel >= unLevel) continue;
		if(m_lpTreeInfo[i]->nChild == 0) continue;
		if(m_lpTreeInfo[i]->bExpand) continue;
//DEBUG		LPRECORD_CBL lpRec = m_lpTreeInfo[i]->lpRec;
		int nItem = TreeInfoIndex2ListIndex(i);	// ちと、時間がかかる
		if(nItem < 0) continue;
		// ﾌﾟﾛｸﾞﾚｽ
		if(m_lpcProgStbar){
			int nProgressMax = GetChildCount(i);
			m_lpcProgStbar->SetProg(0, nProgressMax, 1);
			m_lpcProgStbar->ShowProg(TRUE);
		}
		// トレース
		char szTraceMsg[512];
		wsprintf(szTraceMsg, 
			"[%02d %s]の展開中...",
			m_lpTreeInfo[i]->lpRec->unLevel,
			m_lpTreeInfo[i]->lpRec->szDescriptor);
		AddTrace(szTraceMsg);
		nItems += ExpandItem((const int)nItem);
	}
	// ﾌﾟﾛｸﾞﾚｽ
	if(m_lpcProgStbar) m_lpcProgStbar->ShowProg(FALSE);
	if(nItems > 0){
		AddTrace("レコード展開");
	}
	return nItems;
}

void CAnalysisList::UpdateListItem(const int nItem)
{
	if(!m_hWndList) return;

	RECT rcUpdate;
	ListView_GetItemRect(m_hWndList, nItem, &rcUpdate, LVIR_BOUNDS);
	// 更新
	InvalidateRect(m_hWndList, &rcUpdate, TRUE);
}

int CAnalysisList::AddListItem(const int nTreeInfo)
{
	return (InsertListItem(GetCount(), nTreeInfo));
}

int CAnalysisList::InsertListItem(const int nItem, const int nTreeInfo)
{
	if(!m_hWndList) return -1;
	if(nTreeInfo < 0 || nTreeInfo >= (int)m_lpTreeInfo.size()) return -1;

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	char szVal[260];
	LPRECORD_CBL lpTmp = m_lpTreeInfo[nTreeInfo]->lpRec;

	// Lv
	wsprintf(szVal, "%02d", lpTmp->unLevel);
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_INDENT;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szVal;
	lvi.lParam = (LPARAM)nTreeInfo;
	lvi.iIndent = m_lpTreeInfo[nTreeInfo]->nIndent;
	int nInstItem = ListView_InsertItem(m_hWndList, &lvi);

	if(nInstItem < 0) return -1;

	// 項目名
	lvi.mask = LVIF_TEXT;
	lvi.iItem = nItem;
	lvi.iSubItem = 1;
	lvi.pszText = lpTmp->szDescriptor;
	ListView_SetItem(m_hWndList, &lvi);

	// 型定義
	lvi.mask = LVIF_TEXT;
	lvi.iItem = nItem;
	lvi.iSubItem = 2;
	lvi.pszText = lpTmp->szTypeDef;
	ListView_SetItem(m_hWndList, &lvi);

	// 大きさ
	wsprintf(szVal, "%d Byte", lpTmp->dwTypeSize);
//TEST	wsprintf(szVal, "%d Child", m_lpTreeInfo[nTreeInfo]->nChild);
	lvi.mask = LVIF_TEXT;
	lvi.iItem = nItem;
	lvi.iSubItem = 3;
	lvi.pszText = szVal;
	ListView_SetItem(m_hWndList, &lvi);

	// 位置
	wsprintf(szVal, "%d Byte", lpTmp->dwTypePos);
//TEST	wsprintf(szVal, "%d Child", m_lpTreeInfo[nTreeInfo]->nAllChild);
	lvi.mask = LVIF_TEXT;
	lvi.iItem = nItem;
	lvi.iSubItem = 4;
	lvi.pszText = szVal;
	ListView_SetItem(m_hWndList, &lvi);

	return nInstItem;
}

BOOL CAnalysisList::DeleteListItem(const int nItem)
{
	if(!m_hWndList) return -1;
	return (ListView_DeleteItem(m_hWndList, nItem));
}

LRESULT CAnalysisList::DrawItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lp;
	if(lpdis->CtlType != ODT_LISTVIEW) return (DefWindowProc(hWnd, msg, wp, lp));
	if (lpdis->CtlID != m_uiListId) return (DefWindowProc(hWnd, msg, wp, lp));
	//	if(lpdis->itemAction != ODA_DRAWENTIRE) return (DefWindowProc(hWnd, msg, wp, lp));
	if (!m_hImgList) return (DefWindowProc(hWnd, msg, wp, lp));

	int nItem = lpdis->itemID;
	int nTreeInfo = lpdis->itemData;
	int nIndent = m_lpTreeInfo[nTreeInfo]->nIndent;
	LPRECORD_CBL lpRec = m_lpTreeInfo[nTreeInfo]->lpRec;

	// 基本の設定
	RECT rcItem;
	ListView_GetItemRect(m_hWndList, nItem, &rcItem, LVIR_BOUNDS);
	rcItem.right -= rcItem.left;
	rcItem.bottom -= rcItem.top;
	rcItem.left = 0;
	rcItem.top = 0;
	COLORREF colBg, colFg;
	if (lpdis->itemState & ODS_SELECTED) {
		/*
				if(bFocus){
					colBg = GetSysColor(COLOR_HIGHLIGHT);
					colFg = GetSysColor(COLOR_HIGHLIGHTTEXT);
				}else{
					colBg = GetSysColor(COLOR_INACTIVECAPTION);
					colFg = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
				}
		*/
		colBg = GetSysColor(COLOR_HIGHLIGHT);
		colFg = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else {
		colFg = m_colFore;
		colBg = m_colBack;
		COLORREF colBlend = 0;
		if (lpRec->bStruct) colBlend |= m_colStruct;
		if (lpRec->unOccursGroupId > 0) colBlend |= m_colOccurs;
		if (lpRec->unRedefinesGroupId > 0) colBlend |= m_colRedefines;
		if (lpRec->btCompress > 0) colBlend |= m_colBinary;
		if (lpRec->nSupplementation > 0) colBlend = m_colSupplementation;
		if (colBlend > 0) {
			if (m_bUseBackGround) {
				colBg = colBlend;
			}
			else {
				colFg = colBlend;
			}
		}
	}
	int nOffset = 0;

	// 描画用ビットマップ
	HDC hmDC = CreateCompatibleDC(lpdis->hDC);
	HBITMAP hmBmp = CreateCompatibleBitmap(lpdis->hDC, rcItem.right, rcItem.bottom);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hmDC, hmBmp);

	// 背景塗りつぶし
	FillRect(hmDC, &rcItem, GetSysColorBrush(COLOR_WINDOW));

	// ツリーっぽく描画
	RECT rcIcon;
	GetColRect(0, &rcIcon);
	if (nIndent > 0) DrawTreeItemParentLine(hmDC, &rcIcon, nTreeInfo);
	nOffset = nIndent * m_nIconSizeX;
	ImageList_DrawEx(m_hImgList,
		GetTreeImageIndex(nTreeInfo),
		hmDC,
		nOffset, 0,
		m_nIconSizeX, m_nIconSizeY,
		CLR_DEFAULT, CLR_DEFAULT, ILD_TRANSPARENT);

	// 背景を指定色に
	nOffset += m_nIconSizeX;
	HBRUSH hBrushBg = CreateSolidBrush(colBg);
	RECT rcFocus;
	SetRect(&rcFocus,
		nOffset, 0,
		rcItem.right, (rcItem.bottom - 1));
	FillRect(hmDC, &rcFocus, hBrushBg);
	// フォーカス描画
	if (lpdis->itemState & ODS_SELECTED) DrawFocusRect(hmDC, &rcFocus);
	DeleteObject(hBrushBg);

	// 文字列描画
	HFONT hFont = (HFONT)SendMessage(m_hWndList, WM_GETFONT, 0L, 0L);
	HFONT hOldFont = (HFONT)SelectObject(hmDC, hFont);
	COLORREF colOldBg = SetBkColor(hmDC, colBg);
	COLORREF colOldFg = SetTextColor(hmDC, colFg);

	RECT rcText;
	// 先頭項目はアイコンの幅(nOffset)を考慮
	int nCol = Header_GetItemCount(m_hWndListHeader), nx = nOffset;
	for (int i = 0; i < nCol; i++) {
		if (i > 0) nx = rcText.right;
		GetColRect(i, &rcText);
		SetRect(&rcText,
			nx, 0,
			(rcText.right - 1), rcItem.bottom);
		// --------------------------------------------
		// データ列だけ背景色変更
		// --------------------------------------------
		if (i == COL_DATA) {

			if(!(lpdis->itemState & ODS_SELECTED)){

				HBRUSH hDataBrush =
					CreateSolidBrush(m_colDatCol);

				FillRect(
					hmDC,
					&rcText,
					hDataBrush);

				DeleteObject(
					hDataBrush);

				colOldBg = SetBkColor(hmDC, m_colDatCol);
			}

			rcText.left += 4;
			rcText.right -= 4;
		}

		DrawItemText(nItem, i, hmDC, &rcText);
	}
/*
	char szDebug[256];
	wsprintf(szDebug,
		"Struct:%d Occurs:%d Redefines:%d Compress:%d Supplement:%d 0x%08x",
		lpRec->bStruct,
		lpRec->unOccursGroupId,
		lpRec->unRedefinesGroupId,
		lpRec->btCompress,
		lpRec->nSupplementation,
		colFg, colBg);
	TextOut(hmDC, nOffset, 1, szDebug, strlen(szDebug));
*/

	// 転送する
	BitBlt(lpdis->hDC,
		lpdis->rcItem.left, lpdis->rcItem.top,
		(lpdis->rcItem.right - lpdis->rcItem.left), (lpdis->rcItem.bottom - lpdis->rcItem.top),
		hmDC, 0, 0,
		SRCCOPY);

	SelectObject(hmDC, hOldFont);
	SetBkColor(hmDC, colOldBg);
	SetTextColor(hmDC, colOldFg);

	SelectObject(hmDC, hOldBmp);
	DeleteObject(hmBmp);
	DeleteDC(hmDC);

	return (0L);
}

/* ツリーの｜を描画 */
void CAnalysisList::DrawTreeItemParentLine(HDC hDC, LPRECT lprc, const int nTreeInfo)
{
	if(nTreeInfo < 1) return;

	// 親の位置
	int nParent = m_lpTreeInfo[nTreeInfo]->lpRec->nStructGroupIndex;
	if(nParent < 0) return;
/*
	// 上位に｜線が必要か？
	int nDrawIndent = -1;
	unsigned int unLevel = m_lpTreeInfo[nParent]->lpRec->unLevel;
	// 最小レベル以下は終わり
	if(unLevel < m_nMinLevel) return;
	for(int i = nParent + 1; i < (int)m_lpTreeInfo.size(); i++){
		unsigned int unNextLevel = m_lpTreeInfo[i]->lpRec->unLevel;
		// 以降に上位レベルがある場合はまだレコードは続くケド、関連はない
		if(unLevel > unNextLevel) break;
		// 以降に下位レベルがある場合はまだレコードは続く
		if(unLevel < unNextLevel) continue;
		// 親と同位のレベルが以降にある場合は「｜」が必要
		if(unLevel == unNextLevel){
			if(i > nTreeInfo){
				nDrawIndent = m_lpTreeInfo[nParent]->nIndent;
				break;
			}
		}
	}
*/
	// 上位に｜線が必要か？
	int nDrawIndent = -1;
	unsigned int unLevel = m_lpTreeInfo[nParent]->lpRec->unLevel;
	int nSize = (int)m_lpTreeInfo.size();
	for(int i = nTreeInfo; i < nSize; i++){
		// 親より上位があったらオワリ
		if(m_lpTreeInfo[i]->lpRec->unLevel < unLevel) break;
		// 親と同位のレベルが以降にある場合は「｜」が必要
		if(m_lpTreeInfo[i]->lpRec->unLevel == unLevel){
			nDrawIndent = m_lpTreeInfo[nParent]->nIndent;
			break;
		}
	}
	// 再起して更に上の描画
	DrawTreeItemParentLine(hDC, lprc, nParent);
	// ｜←描画
	if(nDrawIndent < 0) return;
	ImageList_DrawEx(m_hImgList,
			0,
			hDC,
			lprc->left + (nDrawIndent * m_nIconSizeX),
			lprc->top,
			m_nIconSizeX,
			m_nIconSizeY,
			CLR_DEFAULT,
			CLR_DEFAULT,
			ILD_TRANSPARENT);
}

int CAnalysisList::DrawItemText(const int nItem, const int nSubItem,
								HDC hDC, LPRECT lprc)
{
	if(!m_hWndList) return -1;

	DWORD dwFmt = 0;
	LVCOLUMN lvc;
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT;
	ListView_GetColumn(m_hWndList, nSubItem, &lvc);
	if(lvc.fmt & LVCFMT_LEFT){
		dwFmt = DT_LEFT;
	}else if(lvc.fmt & LVCFMT_RIGHT){
		dwFmt = DT_RIGHT;
	}else if(lvc.fmt & LVCFMT_CENTER){
		dwFmt = DT_CENTER;
	}

	char szVal[260];
	szVal[0] = '\0';

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.pszText = szVal;
	lvi.cchTextMax = sizeof(szVal);

	ListView_GetItem(m_hWndList, &lvi);

	if (nSubItem == COL_DATA) {

		szVal[0] = '\0';

		if (m_lpDataFile &&
			m_lpDataFile->IsOpen())
		{
			int nTreeInfo =
				ListIndex2TreeInfoIndex(nItem);

			if (nTreeInfo >= 0) {

				LPRECORD_CBL lpRec =
					m_lpTreeInfo[
						nTreeInfo]->lpRec;

				if (!lpRec->bStruct) {

					// 数値系なら右寄せ
					if (lpRec->dwType & DES_TYPE_DEC) {
						dwFmt = DT_RIGHT;
					}
					else {
						dwFmt = DT_LEFT;
					}
				}

				DecodeDataValue(
					lpRec,
					m_lpDataFile->GetRecordData(),
					m_lpDataFile->GetRecordSize(),
					szVal,
					sizeof(szVal));
			}
		}
	}

	DRAWTEXTPARAMS dtp;
	ZeroMemory(&dtp, sizeof(DRAWTEXTPARAMS));
	dtp.cbSize = sizeof(DRAWTEXTPARAMS);
	dtp.iLeftMargin = ITEM_TEXT_MARGIN;
	dtp.iRightMargin = 1;

	// 識別子描画＆アイテム用イメージリストが作成済み＆設定フラグＯＮの時のみ、イメージリスト描画
	if(nSubItem == COL_ID && m_hImgListItem && m_bDrawItemImg){
		int nTreeInfo = (int)lvi.lParam, nImage = 0;
		LPRECORD_CBL lpTmp = m_lpTreeInfo[nTreeInfo]->lpRec;
		if(lpTmp->bStruct){
			nImage = (m_lpTreeInfo[nTreeInfo]->bExpand ? IIMG_FOPEN : IIMG_FCLOSE);
		}else{
			if(lpTmp->dwType & DES_TYPE_DEC){
				nImage = IIMG_TYP9;
				if(lpTmp->dwType & DES_TYPE_UNSIGN) nImage = IIMG_TYP9;
				if(lpTmp->dwType & DES_TYPE_SIGN) nImage = IIMG_TYPS;
				if(lpTmp->dwType & DES_TYPE_BIN) nImage = IIMG_TYPC;
			}else if(lpTmp->dwType & DES_TYPE_CHAR){
				nImage = IIMG_TYPX;
				if(lpTmp->dwType & DES_TYPE_JAPANESE) nImage = IIMG_TYPN;
			}
		}
		ImageList_Draw(m_hImgListItem, nImage, hDC,
			lprc->left + ITEM_TEXT_MARGIN, lprc->top + ((lprc->bottom - lprc->top - m_nItemImgWidth) / 2),
			ILD_TRANSPARENT);
		dtp.iLeftMargin += m_nItemImgWidth + (ITEM_TEXT_MARGIN * 2);
	}

	DrawTextEx(hDC,
		szVal, -1,
		lprc,
		dwFmt | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING | DT_END_ELLIPSIS,
		&dtp);

	return 0;
}

int CAnalysisList::HitTest(const int nFlg/* = ANALST_HITTEST_ICON*/)
{
	if(GetAllCount() < 1) return -1;
	POINT pt;
	LV_HITTESTINFO lvhti;
	GetCursorPos((LPPOINT)&pt);
	lvhti.pt = pt;
	ScreenToClient(m_hWndList, &lvhti.pt);
	ListView_HitTest(m_hWndList, &lvhti.pt);
	if(lvhti.flags & LVHT_ONITEM){
		int nTreeInfo = ListIndex2TreeInfoIndex(lvhti.iItem);
		int nOffset = m_lpTreeInfo[nTreeInfo]->nIndent * m_nIconSizeX;
		RECT rcItem;
		ListView_GetItemRect(m_hWndList, lvhti.iItem, &rcItem, LVIR_BOUNDS);
		if(lvhti.pt.x >= nOffset){
			// 「+」印のクリックかどうか判定する
			if(nFlg == ANALST_HITTEST_ICON){
				if(lvhti.pt.x < (nOffset + m_nIconSizeX)) return (lvhti.iItem);
			}else
			// リストアイテムのクリックかどうか判定する
			if(nFlg == ANALST_HITTEST_ITEM){
				if(lvhti.pt.x < rcItem.right) return (lvhti.iItem);
			}
		}
	}
	return -1;
}

int CAnalysisList::AutoExpandItem(const int nItem)
{
	int nTreeInfo = ListIndex2TreeInfoIndex(nItem), nRet;
	if(nTreeInfo < 0) return -1;
	if(!m_lpTreeInfo[nTreeInfo]->lpRec->bStruct) return 0;
	if(m_lpTreeInfo[nTreeInfo]->bExpand){
		nRet = CollapseItem(nItem);
	}else{
		nRet = ExpandItem(nItem);
	}
	return nRet;
}

int CAnalysisList::GetItemData(const int nItem, LPRECORD_CBL lpRecCbl)
{
	int nTreeInfo = 0;
	if((nTreeInfo = ListIndex2TreeInfoIndex(nItem)) < 0) return -1;
	if(nTreeInfo < 0) return -1;
	CopyMemory(lpRecCbl, m_lpTreeInfo[nTreeInfo]->lpRec, (DWORD)sizeof(RECORD_CBL));
	return nTreeInfo;
}

int CAnalysisList::GetItemDataEx(
	const int nTreeInfo,
	LPRECORD_CBL lpRecCbl)
{
	if (nTreeInfo < 0) {
		return -1;
	}

	if (nTreeInfo >=
		(int)m_lpTreeInfo.size())
	{
		return -1;
	}

	if (!m_lpTreeInfo[nTreeInfo] ||
		!m_lpTreeInfo[nTreeInfo]->lpRec)
	{
		return -1;
	}

	CopyMemory(
		lpRecCbl,
		m_lpTreeInfo[nTreeInfo]->lpRec,
		sizeof(RECORD_CBL));

	return nTreeInfo;
}

//int CAnalysisList::GetItemId(const int nItem, char *pszItemId)
int CAnalysisList::GetItemId(const int nItem, char *pszItemId, int nSize)
{
	int nTreeInfo = 0;
	if((nTreeInfo = ListIndex2TreeInfoIndex(nItem)) < 0) return -1;
	if(nTreeInfo < 0) return -1;
//	strcpy(pszItemId, m_lpTreeInfo[nTreeInfo]->lpRec->szDescriptor);
	strcpy_s(pszItemId, nSize, m_lpTreeInfo[nTreeInfo]->lpRec->szDescriptor);
	return ((int)strlen(pszItemId));
}

BOOL CAnalysisList::IsStruct(const int nItem)
{
	RECORD_CBL stRecCbl;
	ZeroMemory(&stRecCbl, sizeof(RECORD_CBL));
	if(GetItemData(nItem, &stRecCbl) < 0) return FALSE;
	return (stRecCbl.bStruct);
}

int CAnalysisList::GetCount()
{
	if(!m_hWndList) return -1;
	return (ListView_GetItemCount(m_hWndList));
}

int CAnalysisList::GetAllCount()
{
	return ((int)m_lpTreeInfo.size());
}

int CAnalysisList::GetChildCount(const int nItem)
{
	int nTreeInfo = ListIndex2TreeInfoIndex(nItem);
	if(nTreeInfo < 0) return -1;
	return (m_lpTreeInfo[nTreeInfo]->nAllChild);
}

int CAnalysisList::GetSel()
{
	if(!m_hWndList) return -1;
	if(GetCount() < 1) return -1;
	return (ListView_GetNextItem(m_hWndList, -1, LVNI_ALL | LVNI_SELECTED));
}

int CAnalysisList::SetSel(const int nItem)
{
	if(!m_hWndList) return -1;
	if(0 > nItem) return -1;
	if(GetCount() < nItem) return -1;
	SetFocus(m_hWndList);
	ListView_SetItemState(m_hWndList, nItem,
		LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	ScrollList(nItem);
	return 0;
}

int CAnalysisList::SetSelEx(const int nTreeInfo)
{
	if(!m_hWndList) return -1;
	if(GetCount() < 1) return -1;
	if(0 > nTreeInfo) return -1;
	if((int)m_lpTreeInfo.size() < nTreeInfo) return -1;
	// 順次展開
	int nExpandItem = -1, nParentItem = -1;
	while((nParentItem = m_lpTreeInfo[nTreeInfo]->lpRec->nStructGroupIndex) >= 0){
		// キャンセル？
		if(m_bCancelExpand){
			AddTrace("キャンセルします");
			break;
		}
		while(nParentItem >= 0){
			if(m_lpTreeInfo[nParentItem]->bExpand) break;
			nExpandItem = TreeInfoIndex2ListIndex(nParentItem);
			nParentItem = m_lpTreeInfo[nParentItem]->lpRec->nStructGroupIndex;
		}
		if(nExpandItem < 0) break;
		if(ExpandItem(nExpandItem) < 0) break;
		nExpandItem = -1;
	}
	return (SetSel(TreeInfoIndex2ListIndex(nTreeInfo)));
}

void CAnalysisList::GetSelPos(LPPOINT lppt)
{
	int nSel = GetSel();
	if(nSel < 0) return;
	ListView_GetItemPosition(m_hWndList, nSel, lppt);
	ClientToScreen(m_hWndList, lppt);
}

void CAnalysisList::ScrollList(const int nItem)
{
	if(!m_hWndList) return;
	if(GetCount() < 1) return;
	RECT rc;
	ListView_GetItemRect(m_hWndList, nItem, &rc, LVIR_ICON);
	ListView_Scroll(m_hWndList, rc.left, rc.top - (rc.bottom - rc.top));
}

int CAnalysisList::IsExpand(const int nItem)
{
	if(!m_hWndList) return -1;
	if(GetCount() < 1) return -1;
	int nTreeInfo = 0;
	if((nTreeInfo = ListIndex2TreeInfoIndex(nItem)) < 0) return -1;
	if(nTreeInfo < 0) return -1;
	if(!m_lpTreeInfo[nTreeInfo]->lpRec->bStruct) return FLG_NONE;
	if(m_lpTreeInfo[nTreeInfo]->nChild < 1) return -1;
	return ((m_lpTreeInfo[nTreeInfo]->bExpand ? FLG_EXPAND : FLG_COLLAPSE));
}

void CAnalysisList::SetColorUse(BOOL bUseBackGround)
{
	m_bUseBackGround = bUseBackGround;
	UpdateWindow(m_hWndList);
};

void CAnalysisList::SetItemImageUse(BOOL bDrawItemImg)
{
	m_bDrawItemImg = bDrawItemImg;
	UpdateWindow(m_hWndList);
	InvalidateRect(m_hWndList, NULL, TRUE);
}

BOOL CAnalysisList::Search(const char *pszOrigin, const char *pszSearchString, const DWORD dwCmdFlags)
{
	int nSearchStringLen = (int)strlen(pszSearchString);
	for(const char *p = pszOrigin; *p != '\0'; p++){
		if((int)strlen(p) < nSearchStringLen) return FALSE;
		if(CompareString(LOCALE_USER_DEFAULT,
			dwCmdFlags,
			p,
			nSearchStringLen,
			pszSearchString,
			nSearchStringLen) == CSTR_EQUAL) return TRUE;
	}
	return FALSE;
}

int CAnalysisList::Search(const char *pszSearchString, const DWORD dwCmdFlags, const DWORD dwCmdExFlags, const DWORD dwSrcFlags/* = SEARCH_FLG_ALL*/)
{
	CWndSplitter *lpCWndSplit = (CWndSplitter *)GetProp(m_hWndParent, CSPLITWND_CLASS);
	if(!lpCWndSplit) return -1;

	char szTraceMsg[260];
	wsprintf(szTraceMsg, "'%s'を検索中...", pszSearchString);
	AddTrace(szTraceMsg);

	// 結果全削除
	lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, 0L, (LPARAM)-1);
	// 検索開始
	int nSearch = 0, nSize = (int)m_lpTreeInfo.size();
	for(int i = 0; i < nSize; i++){
		if(dwCmdExFlags & SEARCH_FLGEX_OCCURSFIRST){
			if((m_lpTreeInfo[i]->lpRec->unOccursGroupId > 0)
			&& (m_lpTreeInfo[i]->lpRec->nOccursOrder != 0)) continue;
		}
		if(dwSrcFlags & SEARCH_FLG_DESCRIPT){
			if(Search(m_lpTreeInfo[i]->lpRec->szDescriptor, pszSearchString, dwCmdFlags)){
				lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, (WPARAM)0, (LPARAM)i);
				wsprintf(szTraceMsg,
					"%s(%d) : %s",
					m_lpTreeInfo[i]->lpRec->szDescriptor, 
					m_lpTreeInfo[i]->lpRec->nLogLine,
					m_lpTreeInfo[i]->lpRec->szDescriptor);
				AddTrace(szTraceMsg);
				nSearch++;
			}
		}
		if(dwSrcFlags & SEARCH_FLG_TYPEDEF){
			if(Search(m_lpTreeInfo[i]->lpRec->szTypeDef, pszSearchString, dwCmdFlags)){
				lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, (WPARAM)0, (LPARAM)i);
				wsprintf(szTraceMsg,
					"%s(%d) : %s",
					m_lpTreeInfo[i]->lpRec->szDescriptor, 
					m_lpTreeInfo[i]->lpRec->nLogLine,
					m_lpTreeInfo[i]->lpRec->szTypeDef);
				AddTrace(szTraceMsg);
				nSearch++;
			}
		}
		if(dwSrcFlags & SEARCH_FLG_COMPRESS){
			if(Search(m_lpTreeInfo[i]->lpRec->szCompress, pszSearchString, dwCmdFlags)){
				lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, (WPARAM)0, (LPARAM)i);
				wsprintf(szTraceMsg,
					"%s(%d) : %s",
					m_lpTreeInfo[i]->lpRec->szDescriptor, 
					m_lpTreeInfo[i]->lpRec->nLogLine,
					m_lpTreeInfo[i]->lpRec->szCompress);
				AddTrace(szTraceMsg);
				nSearch++;
			}
		}
		if(dwSrcFlags & SEARCH_FLG_VALUE){
			if(Search(m_lpTreeInfo[i]->lpRec->szValue, pszSearchString, dwCmdFlags)){
				lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, (WPARAM)0, (LPARAM)i);
				wsprintf(szTraceMsg,
					"%s(%d) : %s",
					m_lpTreeInfo[i]->lpRec->szDescriptor, 
					m_lpTreeInfo[i]->lpRec->nLogLine,
					m_lpTreeInfo[i]->lpRec->szValue);
				AddTrace(szTraceMsg);
				nSearch++;
			}
		}
		if(dwSrcFlags & SEARCH_FLG_COMMENT){
			if(Search(m_lpTreeInfo[i]->lpRec->szComment, pszSearchString, dwCmdFlags)){
				lpCWndSplit->SendMessageSplit(UM_ADDSEARCH, (WPARAM)0, (LPARAM)i);
				wsprintf(szTraceMsg,
					"%s(%d) : %s",
					m_lpTreeInfo[i]->lpRec->szDescriptor, 
					m_lpTreeInfo[i]->lpRec->nLogLine,
					m_lpTreeInfo[i]->lpRec->szComment);
				AddTrace(szTraceMsg);
				nSearch++;
			}
		}
	}

	wsprintf(szTraceMsg, "%d 個検索されました", nSearch);
	AddTrace(szTraceMsg);

	return nSearch;
}

void CAnalysisList::ClipCopy(HWND hWndMain, int nIndents/* = 6*/)
{
	int nSel = GetSel();
	if(nSel < 0) return;
	int nTreeInfo = ListIndex2TreeInfoIndex(nSel);
	if(nTreeInfo < 0) return;
	int nMaxTrrrInfo = (int)m_lpTreeInfo.size();
	// コピー開始のレベル番号退避領域
	int nLevel = -1;
	char szClip[1024 * 64];
	szClip[0] = '\0';
	for( ;nMaxTrrrInfo > nTreeInfo; nTreeInfo++){
		// レコード情報取得
		LPRECORD_CBL lpRec = m_lpTreeInfo[nTreeInfo]->lpRec;
		if(nLevel < 0){
			// １レコード目は退避
			nLevel = (int)lpRec->unLevel;
		}else{
			// １レコード目と同じかそれ以上(数字的には下)のレベル番号があったら終わり
			if(nLevel >= (int)lpRec->unLevel) break;
		}
		// 文字列作成
		int i;
		char szIndents[64];
		szIndents[0] = '\0';
//		for(i = 0; i < (nIndents + lpRec->unLevel) && i < sizeof(szIndents); i++) strcat(szIndents, " ");
		for(i = 0; i < (int)(nIndents + lpRec->unLevel) && i < (int)(sizeof(szIndents)); i++) strcat_s(szIndents, sizeof(szIndents), " ");
		char szBuf[1024];
		wsprintf(szBuf, "%s%02d %s", szIndents, lpRec->unLevel, lpRec->szDescriptor);
//		if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
		strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		if(!lpRec->bStruct){
			wsprintf(szBuf, " PIC %s", lpRec->szTypeDef);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		}
		if(lpRec->bRedefines && strlen(lpRec->szRedefines) > 0){
			wsprintf(szBuf, " REDEFINES %s", lpRec->szRedefines);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		}
		if(lpRec->bOccurs && (lpRec->nOccursOrder == 0)){
			wsprintf(szBuf, " OCCURS %d TIMES", lpRec->nOccurs);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
			if(strlen(lpRec->szOccursIndexed) > 0){
				wsprintf(szBuf, " INDEXED BY %s", lpRec->szOccursIndexed);
//				if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
				strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
			}
		}
		if(lpRec->btCompress && (strlen(lpRec->szCompress) > 0)){
			wsprintf(szBuf, " %s", lpRec->szCompress);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		}
		if(strlen(lpRec->szValue) > 0){
			wsprintf(szBuf, " VALUE \"%s\"", lpRec->szValue);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		}
		if(strlen(lpRec->szValueEx) > 0){
			wsprintf(szBuf, " %s", lpRec->szValueEx);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		}
		char szCblEOL[2] = { CBL_EOL, '\0' };
//		if(!strncat(szClip, szCblEOL, (sizeof(szClip) - strlen(szClip) - 1))) break;
		strncat_s(szClip, sizeof(szClip), szCblEOL, (sizeof(szClip) - strlen(szClip) - 1));
		if(strlen(lpRec->szComment) > 0){
			wsprintf(szBuf, " *> %s", lpRec->szComment);
//			if(!strncat(szClip, szBuf, (sizeof(szClip) - strlen(szClip) - 1))) break;
			strncat_s(szClip, sizeof(szClip), szBuf, (sizeof(szClip) - strlen(szClip) - 1));
		}
		// 次レコード
//		if(!strncat(szClip, "\r\n", (sizeof(szClip) - strlen(szClip) - 1))) break;
		strncat_s(szClip, sizeof(szClip), "\r\n", (sizeof(szClip) - strlen(szClip) - 1));
	}
	// クリップボードへ
	if(strlen(szClip) > 0){
		if(!OpenClipboard(hWndMain)){
			ERRMSG(hWndMain, "クリップボードがオープンできません");
			return;
		}
		EmptyClipboard();
		HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, (strlen(szClip) + 1));
		if(!hMem){
			ERRMSG(hWndMain, "メモリの確保に失敗しました");
			CloseClipboard();
			return;
		}
		LPTSTR lptMem = (LPTSTR)GlobalLock(hMem);
//		strcpy(lptMem, szClip);
		strcpy_s(lptMem, (strlen(szClip) + 1), szClip);
		GlobalUnlock(hMem);
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}
}

void CAnalysisList::SetDataFileClass(CDataFile* lpDataFile)
{
	m_lpDataFile = lpDataFile;
}

void CAnalysisList::RefreshDataValues()
{
	if (m_hWndList) {
		InvalidateRect(m_hWndList, NULL, FALSE);
	}
}

BOOL CAnalysisList::DecodeDataValue(
	LPRECORD_CBL lpRec,
	const BYTE* pRecord,
	DWORD dwRecordSize,
	char* pszOut,
	int nOutSize)
{
	if (!pszOut || nOutSize < 1) {
		return FALSE;
	}

	pszOut[0] = '\0';

	if (!lpRec || !pRecord) {
		return FALSE;
	}

	// 集団項目には値を表示しない
	if (lpRec->bStruct) {
		return TRUE;
	}

	DWORD dwDataPos = lpRec->dwTypePos;

	if (dwDataPos < (DWORD)m_nDataPosBase) {
		return FALSE;
	}

	dwDataPos -= m_nDataPosBase;

	// レコード外参照防止
	if (dwDataPos > dwRecordSize) {
		return FALSE;
	}

	if (lpRec->dwTypeSize >
		(dwRecordSize - dwDataPos))
	{
		return FALSE;
	}

	const BYTE* pData =
		pRecord + dwDataPos;

	DWORD dwSize =
		lpRec->dwTypeSize;

	// COMP-3 / PACKED-DECIMAL
	if (lpRec->btCompress == COMP_DECIMAL) {
		return DecodePackedDecimal(
			lpRec,
			pData,
			dwSize,
			pszOut,
			nOutSize);
	}

	// COMP / BINARY
	if (lpRec->btCompress == COMP_BINARY) {
		return DecodeBinary(
			lpRec,
			pData,
			dwSize,
			pszOut,
			nOutSize);
	}

	// その他
	if (lpRec->btCompress != 0) {
		strcpy_s(
			pszOut,
			nOutSize,
			"<COMPRESS>");

		return TRUE;
	}

	// 文字
	if (lpRec->dwType & DES_TYPE_CHAR) {

		DWORD dwCopy = dwSize;

		if (dwCopy >= (DWORD)nOutSize) {
			dwCopy = nOutSize - 1;
		}

		CopyMemory(
			pszOut,
			pData,
			dwCopy);

		pszOut[dwCopy] = '\0';

		TrimDataString(
			pszOut,
			(lpRec->dwType & DES_TYPE_JAPANESE)
			? TRUE
			: FALSE);

		return TRUE;
	}

	// DISPLAY数値
	if (lpRec->dwType & DES_TYPE_DEC) {
		return DecodeDisplayNumeric(
			lpRec,
			pData,
			dwSize,
			pszOut,
			nOutSize);
	}

	return TRUE;
}

BOOL CAnalysisList::DecodeDisplayNumeric(
	LPRECORD_CBL lpRec,
	const BYTE* pData,
	DWORD dwSize,
	char* pszOut,
	int nOutSize)
{
	if (!lpRec ||
		!pData ||
		!pszOut ||
		nOutSize < 2)
	{
		return FALSE;
	}

	pszOut[0] = '\0';

	if (dwSize == 0) {
		return TRUE;
	}

	char szWork[260];
	ZeroMemory(
		szWork,
		sizeof(szWork));

	DWORD dwCopy = dwSize;

	if (dwCopy >= sizeof(szWork)) {
		dwCopy = sizeof(szWork) - 1;
	}

	CopyMemory(
		szWork,
		pData,
		dwCopy);

	szWork[dwCopy] = '\0';

	BOOL bNegative = FALSE;

	char* pszNumber = szWork;
	int nNumberLen =
		static_cast<int>(dwCopy);

	//--------------------------------------------------
	// SIGN IS LEADING SEPARATE
	//--------------------------------------------------
	if (lpRec->dwType &
		DES_TYPE_LSEPARATE)
	{
		if (nNumberLen > 0) {

			if (szWork[0] == '-') {
				bNegative = TRUE;
			}

			pszNumber++;
			nNumberLen--;
		}
	}

	//--------------------------------------------------
	// SIGN IS TRAILING SEPARATE
	//--------------------------------------------------
	else
		if (lpRec->dwType &
			DES_TYPE_TSEPARATE)
		{
			if (nNumberLen > 0) {

				char cSign =
					szWork[nNumberLen - 1];

				if (cSign == '-') {
					bNegative = TRUE;
				}

				szWork[nNumberLen - 1] = '\0';
				nNumberLen--;
			}
		}

	//--------------------------------------------------
	// S9 の非SEPARATE符号
	//--------------------------------------------------
		else
			if ((lpRec->dwType & DES_TYPE_SIGN) &&
				nNumberLen > 0)
			{
				char& c =
					pszNumber[nNumberLen - 1];

				// ASCII系COBOLで使われる
				// overpunch を読む
				if (c == '{') {
					c = '0';
				}
				else
					if (c >= 'A' && c <= 'I') {
						c = '1' + (c - 'A');
					}
					else
						if (c == '}') {
							c = '0';
							bNegative = TRUE;
						}
						else
							if (c >= 'J' && c <= 'R') {
								c = '1' + (c - 'J');
								bNegative = TRUE;
							}
			}

	//--------------------------------------------------
	// 小数点
	//--------------------------------------------------

	char szNumber[260];
	ZeroMemory(
		szNumber,
		sizeof(szNumber));

	// V はファイル上に小数点を持たない
	BOOL bVirtualDecimal =
		(strchr(lpRec->szTypeDef, 'V') != NULL ||
			strchr(lpRec->szTypeDef, 'v') != NULL);

	if (bVirtualDecimal &&
		lpRec->phyd.nLowLen > 0)
	{
		int nLowLen =
			lpRec->phyd.nLowLen;

		if (nLowLen < nNumberLen) {

			int nHighLen =
				nNumberLen - nLowLen;

			CopyMemory(
				szNumber,
				pszNumber,
				nHighLen);

			szNumber[nHighLen] = '.';

			CopyMemory(
				szNumber + nHighLen + 1,
				pszNumber + nHighLen,
				nLowLen);

			szNumber[
				nHighLen +
					nLowLen +
					1] = '\0';
		}
		else {
			strcpy_s(
				szNumber,
				sizeof(szNumber),
				pszNumber);
		}
	}
	else {
		// PIC 9(3).99 等は
		// '.' 自体がデータ中に存在する
		strcpy_s(
			szNumber,
			sizeof(szNumber),
			pszNumber);
	}

	//--------------------------------------------------
	// 符号を表示
	//--------------------------------------------------

	if (bNegative) {
		strcpy_s(
			pszOut,
			nOutSize,
			"-");

		strcat_s(
			pszOut,
			nOutSize,
			szNumber);
	}
	else {
		strcpy_s(
			pszOut,
			nOutSize,
			szNumber);
	}

	return TRUE;
}

BOOL CAnalysisList::DecodePackedDecimal(
	LPRECORD_CBL lpRec,
	const BYTE* pData,
	DWORD dwSize,
	char* pszOut,
	int nOutSize)
{
	if (!lpRec ||
		!pData ||
		!pszOut ||
		nOutSize < 2)
	{
		return FALSE;
	}

	pszOut[0] = '\0';

	if (dwSize == 0) {
		return TRUE;
	}

	char szDigits[260];
	ZeroMemory(
		szDigits,
		sizeof(szDigits));

	int nDigitPos = 0;

	//--------------------------------------------------
	// 最終byte以外
	//--------------------------------------------------
	for (DWORD i = 0;
		i < dwSize;
		i++)
	{
		BYTE b = pData[i];

		int nHigh =
			(b >> 4) & 0x0F;

		int nLow =
			b & 0x0F;

		// 最後のLow nibbleは符号
		BOOL bLast =
			(i == dwSize - 1);

		//--------------------------------------------------
		// High nibble
		//--------------------------------------------------
		if (nDigitPos <
			(int)sizeof(szDigits) - 1)
		{
			if (nHigh <= 9) {
				szDigits[nDigitPos++] =
					static_cast<char>(
						'0' + nHigh);
			}
			else {
				// 不正なPACKED値
				strcpy_s(
					pszOut,
					nOutSize,
					"<INVALID COMP-3>");

				return FALSE;
			}
		}

		//--------------------------------------------------
		// Low nibble
		//--------------------------------------------------
		if (!bLast) {
			if (nDigitPos <
				(int)sizeof(szDigits) - 1)
			{
				if (nLow <= 9) {
					szDigits[nDigitPos++] =
						static_cast<char>(
							'0' + nLow);
				}
				else {
					strcpy_s(
						pszOut,
						nOutSize,
						"<INVALID COMP-3>");

					return FALSE;
				}
			}
		}
	}

	szDigits[nDigitPos] = '\0';

	//--------------------------------------------------
	// 符号取得
	//--------------------------------------------------
	BYTE bSign =
		pData[dwSize - 1] & 0x0F;

	BOOL bNegative = FALSE;

	switch (bSign) {
	case 0x0B:
	case 0x0D:
		bNegative = TRUE;
		break;

	case 0x0A:
	case 0x0C:
	case 0x0E:
	case 0x0F:
		bNegative = FALSE;
		break;

	default:
		strcpy_s(
			pszOut,
			nOutSize,
			"<INVALID COMP-3>");

		return FALSE;
	}

	//--------------------------------------------------
	// PACKED領域の先頭に余分な0 nibbleがある場合
	//--------------------------------------------------
	int nDefinedDigits =
		lpRec->phyd.nHiLen +
		lpRec->phyd.nLowLen;

	char* pszDigits =
		szDigits;

	int nPackedDigits =
		static_cast<int>(
			strlen(szDigits));

	if (nDefinedDigits > 0 &&
		nPackedDigits > nDefinedDigits)
	{
		pszDigits +=
			nPackedDigits -
			nDefinedDigits;
	}

	//--------------------------------------------------
	// 暗黙小数点を挿入
	//--------------------------------------------------
	char szNumber[260];
	ZeroMemory(
		szNumber,
		sizeof(szNumber));

	int nLen =
		static_cast<int>(
			strlen(pszDigits));

	if (lpRec->phyd.nLowLen > 0) {

		int nLow =
			lpRec->phyd.nLowLen;

		if (nLow < nLen) {

			int nHigh =
				nLen - nLow;

			CopyMemory(
				szNumber,
				pszDigits,
				nHigh);

			szNumber[nHigh] = '.';

			CopyMemory(
				szNumber + nHigh + 1,
				pszDigits + nHigh,
				nLow);

			szNumber[
				nHigh +
					nLow +
					1] = '\0';
		}
		else {
			//
			// 例えば V99 で値が "12"
			//
			strcpy_s(
				szNumber,
				sizeof(szNumber),
				"0.");

			int nZero =
				nLow - nLen;

			while (nZero-- > 0) {
				strcat_s(
					szNumber,
					sizeof(szNumber),
					"0");
			}

			strcat_s(
				szNumber,
				sizeof(szNumber),
				pszDigits);
		}
	}
	else {
		strcpy_s(
			szNumber,
			sizeof(szNumber),
			pszDigits);
	}

	//--------------------------------------------------
	// 最終結果
	//--------------------------------------------------
	if (bNegative) {

		strcpy_s(
			pszOut,
			nOutSize,
			"-");

		strcat_s(
			pszOut,
			nOutSize,
			szNumber);
	}
	else {
		strcpy_s(
			pszOut,
			nOutSize,
			szNumber);
	}

	return TRUE;
}

BOOL CAnalysisList::DecodeBinary(
	LPRECORD_CBL lpRec,
	const BYTE* pData,
	DWORD dwSize,
	char* pszOut,
	int nOutSize)
{
	if (!lpRec ||
		!pData ||
		!pszOut ||
		nOutSize < 2)
	{
		return FALSE;
	}

	pszOut[0] = '\0';

	char szNumber[260];

	//--------------------------------------------------
	// 2Byte
	//--------------------------------------------------
	if (dwSize == 2) {

		if (lpRec->dwType & DES_TYPE_SIGN) {

			short nValue = 0;

			CopyMemory(
				&nValue,
				pData,
				sizeof(nValue));

			wsprintf(
				szNumber,
				"%d",
				(int)nValue);
		}
		else {

			unsigned short nValue = 0;

			CopyMemory(
				&nValue,
				pData,
				sizeof(nValue));

			wsprintf(
				szNumber,
				"%u",
				(unsigned int)nValue);
		}

		return InsertDecimalPoint(
			szNumber,
			lpRec->phyd.nLowLen,
			pszOut,
			nOutSize);
	}

	//--------------------------------------------------
	// 4Byte
	//--------------------------------------------------
	if (dwSize == 4) {

		if (lpRec->dwType & DES_TYPE_SIGN) {

			LONG nValue = 0;

			CopyMemory(
				&nValue,
				pData,
				sizeof(nValue));

			wsprintf(
				szNumber,
				"%ld",
				nValue);
		}
		else {

			DWORD nValue = 0;

			CopyMemory(
				&nValue,
				pData,
				sizeof(nValue));

			wsprintf(
				szNumber,
				"%lu",
				nValue);
		}

		return InsertDecimalPoint(
			szNumber,
			lpRec->phyd.nLowLen,
			pszOut,
			nOutSize);
	}

	//--------------------------------------------------
	// 未対応
	//--------------------------------------------------
	wsprintf(
		pszOut,
		"<BINARY:%uByte>",
		dwSize);

	return TRUE;
}

BOOL CAnalysisList::InsertDecimalPoint(
	const char* pszNumber,
	int nLowLen,
	char* pszOut,
	int nOutSize)
{
	if (!pszNumber ||
		!pszOut ||
		nOutSize < 2)
	{
		return FALSE;
	}

	pszOut[0] = '\0';

	if (nLowLen <= 0) {
		strcpy_s(
			pszOut,
			nOutSize,
			pszNumber);

		return TRUE;
	}

	BOOL bNegative =
		(pszNumber[0] == '-');

	const char* pszDigits =
		bNegative
		? pszNumber + 1
		: pszNumber;

	int nLen =
		(int)strlen(pszDigits);

	char szWork[260];
	ZeroMemory(
		szWork,
		sizeof(szWork));

	if (bNegative) {
		strcat_s(
			szWork,
			sizeof(szWork),
			"-");
	}

	if (nLen > nLowLen) {

		int nHigh =
			nLen - nLowLen;

		strncat_s(
			szWork,
			sizeof(szWork),
			pszDigits,
			nHigh);

		strcat_s(
			szWork,
			sizeof(szWork),
			".");

		strcat_s(
			szWork,
			sizeof(szWork),
			pszDigits + nHigh);
	}
	else {

		strcat_s(
			szWork,
			sizeof(szWork),
			"0.");

		for (int i = 0;
			i < nLowLen - nLen;
			i++)
		{
			strcat_s(
				szWork,
				sizeof(szWork),
				"0");
		}

		strcat_s(
			szWork,
			sizeof(szWork),
			pszDigits);
	}

	strcpy_s(
		pszOut,
		nOutSize,
		szWork);

	return TRUE;
}

void CAnalysisList::TrimDataString(
	char* pszText,
	BOOL bJapanese)
{
	if (!pszText) {
		return;
	}

	int nLen =
		(int)strlen(pszText);

	while (nLen > 0) {

		// 半角SPACE
		if (pszText[nLen - 1] == ' ') {

			pszText[nLen - 1] = '\0';
			nLen--;

			continue;
		}

		// Shift-JIS全角SPACE
		if (bJapanese &&
			nLen >= 2 &&
			(BYTE)pszText[nLen - 2] == 0x81 &&
			(BYTE)pszText[nLen - 1] == 0x40)
		{
			pszText[nLen - 2] = '\0';
			nLen -= 2;

			continue;
		}

		break;
	}
}