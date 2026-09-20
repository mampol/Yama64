/*-------------------------------------------------------------------------------------

	YamamotoCom.h

-------------------------------------------------------------------------------------*/
#ifndef _YAMAMOTOCOM_

#ifndef _INC_CYAMASETUP_
	#include "CYamaSetup.h"
#endif	//_INC_CYAMASETUP_
#ifndef _CDLGBAR_H
	#include "CDlgbar.h"
#endif	//_CDLGBAR_H
#ifndef _INC_CPROGSTBAR
	#include "CProgStbar.h"
#endif	//_INC_CPROGSTBAR
#ifndef _INC_CWNDSPLITTER_
	#include "CWndSplitter.h"
#endif	//_INC_CWNDSPLITTER_
#ifndef _CANACBL_
	#include "CAnaCBL.h"
#endif	//_CANACBL_
#ifndef _CANALYSISLIST_H
	#include "CAnalysisList.h"
#endif	//_CANALYSISLIST_H
#ifndef _INC_CUICOMBO
	#include "CUICombo.h"
#endif

#define APP_HOPE_VER				(LPCTSTR)"COBOLer"
#define APP_CLASS					(LPCTSTR)"Yama_WindowClass"
#define APP_NAME					(LPCTSTR)"Yama64 Free"
#define APP_VERSION					(LPCTSTR)"0.4.0.0"
#define APP_VER_MEJOR				1
#define APP_VER_MINOR				1
#define APP_VER_BUILD				2
#define APP_VER_REVISION			0
#define APP_TITLE					(LPCTSTR)"Yama64 Free - %s -"
#define APP_COPYRIGHT				(LPCTSTR)"copyright Pol."
//#define APP_COPY					(LPCTSTR)"ÅR(`ÑDÅL)…"
#define APP_COPY					(LPCTSTR)"Pol."
#define APP_CAPTION					(LPCTSTR)"Yama64.exe"

#define TMP_DIR_TITLE				(LPCTSTR)"YamaTmp"

#define MAX_OPENFILE				12
#define MAX_FILENAME				65

#define SMED_EXT					".smd"

#define CWINTHEME_CLASS				(LPCTSTR)"CWinThemeClass_Inst"
#define CSETUP_CLASS				(LPCTSTR)"CSetupClass_Inst"
#define CDLGBAR_CLASS				(LPCTSTR)"CDlgbarClass_Inst"
#define CPROGSTBAR_CLASS			(LPCTSTR)"CProgstbarClass_Inst"
#define CSPLITWND_CLASS				(LPCTSTR)"CSplitWndClass_Inst"
//#define CSPLITWND_CLASS2			(LPCTSTR)"CSplitWndClass2_Inst"
#define CANACBL_CLASS				(LPCTSTR)"CAnacblClass_Inst"
#define CANALST_CLASS				(LPCTSTR)"CAnalstClass_Inst"
#define CUICOMBO_CLASS				(LPCTSTR)"CUIComboClass_Inst"
#define CDATAFILE_CLASS				(LPCTSTR)"CDataFileClass_Inst"

#define UM_ADDOUTPUT				(WM_USER+900)
#define UM_ADDSEARCH				(WM_USER+901)
#define UM_BEGINANACBL				(WM_USER+902)
#define UM_FINISHANACBL				(WM_USER+903)
#define UM_UPDSOURCE				(WM_USER+904)

#define HELP_FILE					"\\Readme.txt"

#endif	//_YAMAMOTOCOM_