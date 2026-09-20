/*----------------------------------------------------------------------------------------

	SetupPage.h

----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
	定義
----------------------------------------------------------------------------------------*/
#define SETUPPAGE_CLASS				(LPCTSTR)"YamamotoSetupPage_Class"
#define SETUPPAGE_CAPTION			(LPCTSTR)"Yama64 - 設定 -"

#define ID_SETUPPAGE				5500

#define SETTEXTLIMIT(hPage, ID, nLimit)	\
									SendDlgItemMessage((HWND)hPage, (int)ID, EM_LIMITTEXT, (WPARAM)nLimit, 0L)

#define ABOUT_MSG		"Yama64 は現在開発中です\n"	\
						"解析結果やデータ値については、実際の COBOL コンパイラおよび業務仕様と照合して確認してください\n"	\
						"現時点では、データファイルの閲覧を主目的としており、編集機能は未実装です\n\n"	\
						"本ソフトウェアはフリーウェアです、個人・法人を問わず無償で使用できます\n\n"	\
						"「COBOL の構造を、見やすく、扱いやすく」"


#ifdef _WIN64
#define CPU_PLATFORM	(LPCTSTR)"(x64)"	
#else
#define CPU_PLATFORM	(LPCTSTR)"(x86)"	
#endif

/*----------------------------------------------------------------------------------------
	関数プロトタイプ
----------------------------------------------------------------------------------------*/
BOOL ShowSetupPage(HWND hParent, CYamaSetup *lpCSetup);
