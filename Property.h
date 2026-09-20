/*----------------------------------------------------------------------------------------

	Property.h

----------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
	定義
------------------------------------------------------------------------------*/
#define ITEM_PROP_PAGE					1

// エディットボックスへバルーン表示
typedef struct _tagMYEDITBALLOONTIP
{
    DWORD   cbStruct;
    LPCWSTR pszTitle;
    LPCWSTR pszText;
    INT     ttiIcon; // From TTI_*
} MYEDITBALLOONTIP, *LPMYEDITBALLOONTIP;
#ifndef ECM_FIRST
	#define ECM_FIRST					0x1500      // Edit control messages
#endif
#ifndef EM_SHOWBALLOONTIP
	#define	EM_SHOWBALLOONTIP			(ECM_FIRST + 3)
#endif
#ifndef EM_HIDEBALLOONTIP
	#define EM_HIDEBALLOONTIP			(ECM_FIRST + 4)
#endif

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
int ShowItemProperty(HWND hWnd);
