/*------------------------------------------------------------------------------
	CTreePropPage.h
------------------------------------------------------------------------------*/
#pragma comment(lib, "comctl32.lib ")

#ifndef _INC_TREEPROP
#define _INC_TREEPROP

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "CImgListPng.h"

// 更新（ＯＫボタン）が押されたとき各ページに送られるメッセージ
#define UM_TPM_APPLYNOW				(WM_USER+683)	// 選択中のノードのみ
#define UM_TPM_APPLY				(WM_USER+684)	// 全ノード
// ページダイアログがアクティブになったとき各ページに送られるメッセージ
#define UM_TPM_SETACTIVE			(WM_USER+685)
// ページダイアログがアクティブでなくなったとき各ページに送られるメッセージ
#define UM_TPM_KILLACTIVE			(WM_USER+686)
// UM_TPM_SETACTIVEとUM_TPM_KILLACTIVEならUM_TPM_KILLACTIVEが先に来る
// ページダイアログが削除されたとき各ページに送られるメッセージ
#define UM_TPM_DELETEPAGE			(WM_USER+687)

// InitializeのdwFlagsはとりあえずPSH_HASHELPのみ
// タイトルバーに「？」を追加します

// コントロールＩＤ
// (resource.hで同じＩＤがあると誤動作するかもしれないので、
//  なるべくＩＤはかぶらないようにする)
#define IDC_NEXTTABSTOP						1901
#define IDC_PREVTABSTOP						1902
#define IDC_BUTTON_HELP						1903
#define IDC_NEXTWNDSTOP						1904
#define IDC_PREVWNDSTOP						1905
//#define ID_TREEPROP_MAIN					2901
#define ID_TREEVIEW							2902
#ifndef IDC_STATIC
	#define IDC_STATIC						-1
#endif

// 新しいAPI
#define _USE_NEWFUNC

// 設定値
#define DEFAULT_CLASS						(LPCTSTR)_T("TreeProp_WndClass")
#define DEFAULT_CAPTION						(LPCTSTR)_T("TreeProp")
#define CTREEPROP_ENTRY_CLASS				(LPCTSTR)_T("TreeProp_Entry")
#define CTREEPROP_ENTRY_LPARAM				(LPCTSTR)_T("TreeProp_Entry_LPARAM")

#define MAX_PAGE							1024
#define TREE_WIDTH							100
#define TREE_PROP_ROOT						(HWND)NULL
#define TREE_STYLES							TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT
#define TREE_ITEM_HEIGHT					22
#define BTN_HEIGHT							26
#define BTN_WIDTH							80
#define BTN_FREE							15
#define SUBTITLE_HEIGHT						22
#define MAX_INFO_STR						256
#define TREE_INDENT							2

#define OK_BUTTON_CAPTION					(LPCTSTR)_T("OK")
#define CAN_BUTTON_CAPTION					(LPCTSTR)_T("キャンセル")

//#define SUB_TITLE_FONT						(LPCTSTR)"Tahoma"
#define CTP_GUI_FONT						(LPCTSTR)_T("Meiryo UI")

// ノード毎のパラメータ
typedef struct _tagNODEPARAM
{
	HWND hPage;
	LPVOID lpParam;
} NODEPARAM, *LPNODEPARAM;

// クラス定義
class CTreeProp
{
private:
	INT_PTR m_nXEdge, m_nYEdge;
	HANDLE m_hMutex;
	HINSTANCE m_hInst;
	BOOL bRegistWndClass;
	TCHAR m_szWndClass[256];
	HWND m_hWndParent, m_hWndMain,
		 m_hTreeView, m_hBtnOK, m_hBtnCan, m_hSubTitle;
	LPARAM m_lParam;
	INT_PTR m_nPageCount;
	HIMAGELIST m_hImgTreeArrow;
	RECT m_rcMain, m_rcTree, m_rcPage;
	INT_PTR m_nTreeWidth, m_nTreeItemHeight;
	DWORD m_dwFlags, m_dwTreeStyles;
	HFONT m_hGUIFont;
	TCHAR m_szInfoString[MAX_INFO_STR+1];

public:
	BOOL m_bOK;

private:
	void initComctl(DWORD dwICC);
	BOOL CreateMain(HINSTANCE hInst,
					HWND hParent,
					LPCTSTR lpcWindowClass,
					LPCTSTR lpcWindowCaption,
					LPTSTR lpIconRes);
	void MainMessageLoop();
	HWND IsPageMessage(MSG *lpmsg);
	static LRESULT CALLBACK PropMainProp(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	BOOL MakeControl(HWND hParent);
	HWND SetNextFocus(BOOL bPrevious);
	HWND SetNextWndFocus();
	BOOL IsSelectedNode(HTREEITEM hItem);
	void SendUpdate();
	void SendUpdate(HTREEITEM hItem);
	INT_PTR GetSelectionText(LPTSTR pszSelText, INT_PTR nSize);
	HTREEITEM FindNode(HTREEITEM hItem, HWND hDlg);
	HWND GetHandle(HTREEITEM hItem);
	void DrawSubTitle(LPDRAWITEMSTRUCT lpdis);
	void Decoration(HDC hDC);

	BOOL TreeViewExpand(HTREEITEM hNode, DWORD dwCode = TVE_EXPAND);
	BOOL AddTreeNode(HWND hParentDlg,
					 HWND hDlg,
					 LPSTR lpcszPageTitle,
					 const INT_PTR nImage = 0,
					 const INT_PTR nSelectedImage = 1,
					 LPVOID lParam = NULL);

public:
	CTreeProp();
	virtual ~CTreeProp();

	HWND GetTreeHandle() { return m_hTreeView; };
	void SetTreeStyles(DWORD dwTreeStyles) { m_dwTreeStyles = dwTreeStyles; };	// Initializeより先
	void AddTreeStyles(DWORD dwTreeStyles) { m_dwTreeStyles |= dwTreeStyles; };  // Initializeより先
	BOOL Initialize(DWORD dwICC,
					HINSTANCE hInst,
					HWND hParent,
					LPCTSTR lpcWindowClass,
					LPCTSTR lpcWindowCaption,
					const DWORD dwFlags,
					LPTSTR lpIconRes,
					LPTSTR lpTreeArowRes,
					const INT_PTR nTreeArow,
					COLORREF colImgMask = RGB(255, 0, 255),
					const INT_PTR nTreeWidth = TREE_WIDTH,
					const INT_PTR nTreeItemHeight = TREE_ITEM_HEIGHT,
					LPARAM lParam = NULL);
	BOOL Initialize(DWORD dwICC,
		HINSTANCE hInst,
		HWND hParent,
		LPCTSTR lpcWindowClass,
		LPCTSTR lpcWindowCaption,
		const DWORD dwFlags,
		LPTSTR lpIconRes,
		UINT uiTreeArowRes,
		const INT_PTR nTreeArow,
		const INT_PTR nTreeWidth = TREE_WIDTH,
		const INT_PTR nTreeItemHeight = TREE_ITEM_HEIGHT,
		LPARAM lParam = NULL);
	HWND AddPage(HWND hParentDlg,
				 HINSTANCE hInst,
				 LPCTSTR lpcTmplate,
				 DLGPROC lpDlgProc,
				 LPTSTR pszTitle = NULL,
 				 const INT_PTR nImage = 0,
				 const INT_PTR nSelectedImage = 1,
				 LPVOID lParam = NULL);
	BOOL AddPage(HWND hParentDlg,
				 LPTSTR pszTitle,
				 const INT_PTR nImage = 0,
				 const INT_PTR nSelectedImage = 1,
				 LPVOID lParam = NULL);
	BOOL ExpandNode(HWND hPageDlg);
	BOOL CollapseNode(HWND hPageDlg);
	BOOL ToggleNode(HWND hPageDlg);
	BOOL SelectNode(HWND hPageDlg);
	BOOL DeletePage(HWND hPageDlg);
	BOOL Show(const HWND hStart = NULL);
	void SetTreeFont(const HFONT hFont);
	void SetInfoStr(LPCTSTR pszInfoText);
	INT_PTR SetNodeText(HWND hPageDlg, LPTSTR pszText);
	void SetOkStr(LPCTSTR pszOkText);
	void SetCancelStr(LPCTSTR pszCancelText);
	INT_PTR GetAllPageCount() { return m_nPageCount; };
	LPVOID GetNodeParam(HWND hPageDlg);

	void SetTreeIndent(const INT_PTR nIndent);
	void SetTextColor(const COLORREF colText);
	void SetBkColor(const COLORREF colBk);
	void SetColor(const COLORREF colText, const COLORREF colBk);
};

#endif //_INC_TREEPROP
