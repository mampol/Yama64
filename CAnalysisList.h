/*----------------------------------------------------------------------------------------

	CAnalysisList.h

----------------------------------------------------------------------------------------*/
#ifndef _CANALYSISLIST_H
#define _CANALYSISLIST_H

//#define _DEBUG_MODE

#ifndef _INC_WINDOWS
	#include <windows.h>
#endif	//_INC_WINDOWS
#ifndef _INC_COMMCTRL
	#include <commctrl.h>
#endif	//_INC_COMMCTRL
/*
#ifndef _CANACBL_
	#include "CAnaCBL.h"
#endif	//_CANACBL_
#ifndef _INC_CPROGSTBAR
	#include "CProgStbar.h"
#endif	//_INC_CPROGSTBAR
#ifndef _INC_CWNDSPLITTER_
	#include "CWndSplitter.h"
#endif	//_INC_CWNDSPLITTER_
*/
#ifndef _YAMAMOTOCOM_
#include "YamamotoCom.h"
#endif	//_YAMAMOTOCOM_

#ifndef _CWINTHEME_
#include "CWinTheme.h"
#endif	//_CWINTHEME_

#include "CDataFile.h"

#define ID_TREELIKELIST				2001

#define MAX_STRBUF					260
#define ITEM_TEXT_MARGIN			3

#define MAX_LVCOLUMN				6
enum { COL_LV = 0, COL_ID, COL_TYPE, COL_SIZE, COL_POS, COL_DATA };
#define MAX_IIMAGE					7
enum { IIMG_FCLOSE = 0, IIMG_FOPEN, IIMG_TYP9, IIMG_TYPX, IIMG_TYPS, IIMG_TYPC, IIMG_TYPN };

#define FLG_NONE					0
#define FLG_EXPAND					1
#define FLG_COLLAPSE				2

#define SEARCH_FLG_DESCRIPT			0x00000001
#define SEARCH_FLG_TYPEDEF			0x00000002
#define SEARCH_FLG_COMPRESS			0x00000004
#define SEARCH_FLG_VALUE			0x00000008
#define SEARCH_FLG_COMMENT			0x00000010
#define SEARCH_FLG_ALL				0x000000FF

#define SEARCH_FLGEX_OCCURSFIRST	0x00000001
#define SEARCH_FLGEX_ALL			0x000000FF

#define ANALST_HITTEST_ICON			0
#define ANALST_HITTEST_ITEM			1

class CDataFile;

typedef struct _tagTREEINFO
{
	LPRECORD_CBL lpRec;
	int nIndent;
	int nNextPos;
	int nChild;
	int nAllChild;
	BOOL bExpand;
} TREEINFO, *LPTREEINFO;

class CAnalysisList
{
private:
	CProgStbar *m_lpcProgStbar;
	HWND m_hWndParent, m_hWndList, m_hWndListHeader;
	UINT m_uiListId;
	HIMAGELIST m_hImgList, m_hImgListItem;
	std::vector<LPTREEINFO> m_lpTreeInfo;
	int m_nIconSizeX, m_nIconSizeY, m_nItemImgWidth;
	unsigned int m_nMaxLevel, m_nMinLevel;
	unsigned int m_unLevelCount[100];
	COLORREF m_colFore, m_colBack,
		m_colStruct, m_colOccurs,
		m_colRedefines, m_colBinary,
		m_colSupplementation,
		m_colDatCol;
	BOOL m_bUseBackGround, m_bDrawItemImg;
	SYSTEMTIME m_stTime;
	BOOL m_bMakeList;
	BOOL m_bCancelExpand;

	CDataFile* m_lpDataFile;
	int m_nDataPosBase;

protected:

public:

private:
/*
	static int CALLBACK GetFontInfo(ENUMLOGFONTEX *lpelfe,
		NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam);	// コールバック
*/
	void MessageDispatch();
	int AddTrace(const char *pszTrace);
	void GetTime() { GetLocalTime(&m_stTime); };
	BOOL Search(const char *pszOrigin, const char *pszSearchString, const DWORD dwCmdFlags);

	BOOL DecodeDataValue(
		LPRECORD_CBL lpRec,
		const BYTE* pRecord,
		DWORD dwRecordSize,
		char* pszOut,
		int nOutSize);

	BOOL DecodeDisplayNumeric(
		LPRECORD_CBL lpRec,
		const BYTE* pData,
		DWORD dwSize,
		char* pszOut,
		int nOutSize);

	BOOL DecodePackedDecimal(
		LPRECORD_CBL lpRec,
		const BYTE* pData,
		DWORD dwSize,
		char* pszOut,
		int nOutSize);

	BOOL DecodeBinary(
		LPRECORD_CBL lpRec,
		const BYTE* pData,
		DWORD dwSize,
		char* pszOut,
		int nOutSize);

	BOOL InsertDecimalPoint(
		const char* pszNumber,
		int nLowLen,
		char* pszOut,
		int nOutSize);

	void TrimDataString(
		char* pszText,
		BOOL bJapanese);

protected:
	BOOL SetImageList();
	HIMAGELIST CreateItemImageList(UINT uiImgBmp, const int nImgWidth = 16, COLORREF colMask = RGB(0, 0, 0));
	void CreateColumn();
	int GetTreeIndent(const unsigned int unLevel);
	int GetItemChild(const int nItem);
	int GetItemAllChild(const int nTreeInfo);
	int GetTreeImageIndex(const int nItem);
	int ListIndex2TreeInfoIndex(const int nItem);
	int TreeInfoIndex2ListIndex(const int nTreeInfo);
	
	int AddListItem(const int nTreeInfo);
	int InsertListItem(const int nItem, const int nTreeInfo);
	BOOL DeleteListItem(const int nItem);

	void DrawTreeItemParentLine(HDC hDC, LPRECT lprc, const int nTreeInfo);
	int DrawItemText(const int nItem, const int nSubItem, HDC hDC, LPRECT lprc);

public:
	CAnalysisList();
	virtual ~CAnalysisList();

	BOOL Create(HWND hWndParent, UINT uiListImgRes = 0, UINT uiListId = ID_TREELIKELIST);
	void SetProgStbarClass(CProgStbar *lpcProgStbar) { m_lpcProgStbar = lpcProgStbar; };
	BOOL SetAnalstFont(const int nFontSize, const char *pszFontFace);
	BOOL Move(LPRECT lprc);
	void ClearList();
	void SetRedraw(const BOOL bFlg);
	void SetFocusListView() { SetFocus(m_hWndList); };

	void SetColText(const int nCol, char *pszText);
	void SetColFmt(const int nCol, WORD wFmt);
	void SetColWidth(const int nCol, int nWidth);
	void SetColPos(const int nColFrom, const int nColTo);
	WORD GetColFmt(const int nCol);
	int GetColWidth(const int nCol);
	BOOL GetColRect(const int nCol, LPRECT lprc);
	void SetMaxLevel(const unsigned int unMaxLevel) { m_nMaxLevel = unMaxLevel; };
	void SetMinLevel(const unsigned int unMinLevel) { m_nMinLevel = unMinLevel; };

	int InitListItem(std::vector<LPRECORD_CBL> &lpRecCbl,
					const unsigned int unMaxLevel = 999,
					const unsigned int unMinLevel = 0);
	BOOL IsMakedList() { return m_bMakeList; };

	LRESULT DrawItem(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	int HitTest(const int nFlg = ANALST_HITTEST_ICON);
	int AutoExpandItem(const int nItem);
	int ExpandItem(const int nItem, BOOL bSetSel = TRUE);
	int ExpandAllItems(const int nItem, BOOL bSetSel = FALSE);
	int CollapseItem(const int nItem, BOOL bSetSel = TRUE);
	void UpdateListItem(const int nItem);
	int ExpandLevelItem(const unsigned unLevel, BOOL bNoRedefines, BOOL bNoOccurs);
	int GetItemData(const int nItem, LPRECORD_CBL lpRecCbl);
	int GetItemDataEx(const int nTreeInfo, LPRECORD_CBL lpRecCbl);
//	int GetItemId(const int nItem, char *pszItemId);
	int GetItemId(const int nItem, char *pszItemId, int nSize);
	BOOL IsStruct(const int nItem);

	int Search(const char *pszSearchString,
			const DWORD dwCmdFlags, const DWORD dwCmdExFlags,
			const DWORD dwSrcFlags = SEARCH_FLG_ALL);

	void ClipCopy(HWND hWndMain, int nIndents = 6);

	void Show() { if(m_hWndList) ShowWindow(m_hWndList, SW_SHOW); };
	void Hide() { if(m_hWndList) ShowWindow(m_hWndList, SW_HIDE); };
	void Update() { if(m_hWndList) UpdateWindow(m_hWndList); };

	int GetCount();
	int GetAllCount();
	int GetChildCount(const int nItem);
	int GetSel();
	int SetSel(const int nItem);
	int SetSelEx(const int nTreeInfo);
	void GetSelPos(LPPOINT lppt);
	void ScrollList(const int nItem);
	int IsExpand(const int nItem);
	void CancelExpand() { m_bCancelExpand = TRUE; };

	void SetColorFore(COLORREF colFore) { m_colFore = colFore; };
	void SetColorBack(COLORREF colBack) { m_colBack = colBack; };
	void SetColorStruct(COLORREF colStruct) { m_colStruct = colStruct; };
	void SetColorOccurs(COLORREF colOccurs) { m_colOccurs = colOccurs; };
	void SetColorRedefines(COLORREF colRedefines) { m_colRedefines = colRedefines; };
	void SetColorBinary(COLORREF colBinary) { m_colBinary = colBinary; };
	void SetColorSupplementation(COLORREF colSupplementation) { m_colSupplementation = colSupplementation; };
	void SetColorDataColumn(COLORREF colDatCol) { m_colDatCol = colDatCol; };
	void SetColorUse(BOOL bUseBackGround);
	void SetItemImageUse(BOOL bDrawItemImg);
	COLORREF GetColorFore() { return m_colFore; };
	COLORREF GetColorBack() { return m_colBack; };
	COLORREF GetColorStruct() { return m_colStruct; };
	COLORREF GetColorOccurs() { return m_colOccurs; };
	COLORREF GetColorRedefines() { return m_colRedefines; };
	COLORREF GetColorBinary() { return m_colBinary; };
	COLORREF GetColorSupplementation() { return m_colSupplementation; };
	COLORREF GetColorDataColumn() { return m_colDatCol; };

	void SetDataFileClass(CDataFile* lpDataFile);
	void RefreshDataValues();

	void SetDataPositionBase(int nPos)
	{
		m_nDataPosBase = nPos;
	}
};

#endif	//_CANALYSISLIST_H
