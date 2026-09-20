/*----------------------------------------------------------------------------------------

	EditDlg.h

----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
	定義
----------------------------------------------------------------------------------------*/
#define REDIT_DLL				(LPCTSTR)"RICHED32.dll"
#define REDIT2_DLL				(LPCTSTR)"RICHED20.dll"

#define EDITDLG_CAPTION			(LPCTSTR)"Yamamoto - ｿｰｽｺｰﾄﾞの編集 -"

#define ID_EDITTOOLBAR			3101
#define ID_EDITSTBAR			3102
#define ID_EDITPROG				3103
#define ID_REDIT				3104

#define EDIT_LIMIT_TEXT			1024*64

#define EDIT_CANACBL_CLASS		(LPCTSTR)"Yamamoto_Edit_CAnaCBL_Class"
#define EDIT_CBMPTB_CLASS		(LPCTSTR)"Yamamoto_Edit_CBmpToolbar_Class"
#define EDIT_CPROGST_CLASS		(LPCTSTR)"Yamamoto_Edit_CProgStbar_Class"
	
// メニュー位置
enum {
	EDIT_MENU_POS_FILE,
	EDIT_MENU_POS_EDIT
};

// サブメニュー位置

// マクロ
#define PIXCEL2TWIP(nPixcel)	(int)(nPixcel * 15)

// ダイアログに渡すパラメータ
typedef struct _tagEDITDLGPARAM
{
	char *pszSrcCode;
	LPRECT lprc;
} EDITDLGPARAM, *LPEDITDLGPARAM;

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
int CreateEditDlg(HWND hParent, CAnaCBL *lpcAnaCBLconst, LPRECT lprcSize, const int nSrcNo = -1);
