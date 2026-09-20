/*-------------------------------------------------------------------------

	CAnaCBL.h

-------------------------------------------------------------------------*/
#ifndef _CANACBL_
#define _CANACBL_
//#define _DEBUG_MODE_	// デバッグスィッチ

#ifndef _INC_WINDOWS
#include <windows.h>
#endif	//_INC_WINDOWS
#ifndef _INC_MMSYSTEM
#include <mmsystem.h>
#endif	//_INC_MMSYSTEM
#ifndef _ALGORITHM_
#include <algorithm>
#endif	//_ALGORITHM_
#ifndef _VECTOR_
#include <vector>
#endif	//_VECTOR_

/*----------------------------------------------------------------------------------------
	リンク
----------------------------------------------------------------------------------------*/
#pragma comment(lib, "winmm.lib")

/*-------------------------------------------------------------------------
	マクロ
-------------------------------------------------------------------------*/
#define CBL_FILE_TITLE_LEN	8
#define MAX_FILE_SIZE		1024*1024*100
#define MAX_BUFFER			256
#define MAX_DESCRIPT_LEN	128
#define MAX_COMMENT_LEN		1024
#define MAX_SUBDATA_LEN		1024
#define MAX_SIGN			5
#define HOST_EOF			(char)0x1A	// ホストのファイル終端?
#define LEX_EOF				(char *)"[EOF]"
#define CBL_EOL				'.'

// ソースタイプ
#define SRC_TYPE_CBL		0
#define SRC_TYPE_SMD		1
#define SRC_TYPE_EDIT		2

// エラーコド
#define TRACE_INFO			0
#define TRACE_ALLOC			1
#define TRACE_FILE			2
#define TRACE_ANA			3
#define TRACE_SUPP			9

// 解析ステージ
#define STAGE_LEX			1
#define STAGE_PARSER		2
#define STAGE_FORM			3
#define STAGE_FORMSRC		4

// 旧ＳＤＫ過不足分
#ifndef CSTR_LESS_THAN
#define CSTR_LESS_THAN		1
#endif	// CSTR_LESS_THAN
#ifndef CSTR_EQUAL
#define CSTR_EQUAL			2
#endif	// CSTR_EQUAL
#ifndef CSTR_GREATER_THAN
#define CSTR_GREATER_THAN	3
#endif	// CSTR_GREATER_THAN

// ＣＯＢＯＬ型定義
#define CBL_TYPE_LEVEL		0x0001		// 変数のレベル定義
#define CBL_TYPE_DESCRIPT	0x0002		// COBOL変数名
#define CBL_TYPE_PIC		0x0003		// COBOL変数型
#define CBL_TYPE_PICSIZE	0x0004		// COBOL変数サイズ定義
#define CBL_TYPE_REDEFINES	0x0005		// 再定義項目
#define CBL_TYPE_OCCURS		0x0006		// 配列定義項目
#define CBL_TYPE_INDEX		0x0007		// 配列指数項目
#define CBL_TYPE_VALUE		0x0008		// 初期化定義項目
#define CBL_TYPE_BIT		0x0009		// BIT変数定義項目
#define CBL_TYPE_BINARY		0x0010		// 2進変数定義項目
#define CBL_TYPE_PACKED		0x0011		// 16進変数定義項目
#define CBL_TYPE_NUMCONST	0x0012		// 数値定数項目
#define CBL_TYPE_CHARCONST	0x0013		// 文字列定数項目
#define CBL_TYPE_BINCONST	0x0014		// バイナリ定数項目
#define CBL_TYPE_JPNCONST	0x0015		// 日本語定数項目
#define CBL_TYPE_SUB		0x0016		// 補助項目
#define CBL_TYPE_NULL		0x0017		// 特に解析には関係ない識別子
#define CBL_TYPE_EOL		0x0018		// 区切り文字(.)
#define CBL_TYPE_LSEPARATE	0x0019		// 演算符号保持
#define CBL_TYPE_TSEPARATE	0x0020		// 演算符号保持
#define CBL_TYPE_COMMENT	0x0021		// "*>"以降のコメント
#define CBL_TYPE_WEOL		0x0022		// 区切り文字が必要と思われる場所
#define CBL_TYPE_EOF		0x0080		// ファイル終端
#define CBL_TYPE_ERROR		0x0099		// 解析不能(失敗)

#define DES_TYPE_DEC		0x0001		// 整数型
#define DES_TYPE_UNSIGN		0x0004		// 符号無し
#define DES_TYPE_SIGN		0x0008		// 符号付き
#define DES_TYPE_CHAR		0x0010		// 文字型
#define DES_TYPE_JAPANESE	0x0020		// ２バイト文字
#define DES_TYPE_DOUBLE		0x0040		// 小数点アリ
#define DES_TYPE_BIN		0x0080		// バイナリ
#define DES_TYPE_LSEPARATE	0x0100		// １ＢＹＴＥ使用する符号
#define DES_TYPE_TSEPARATE	0x0200		// １ＢＹＴＥ使用する符号
#define DES_TYPE_ERROR		0x8000		// 解析不能(失敗)

// ＦＯＲＭ型定義
#define SMD_TYPE_GL_X		0x21
#define SMD_TYPE_GL_9		0x22
#define SMD_TYPE_GL_N		0x23
#define SMD_TYPE_X			0x00
#define SMD_TYPE_9			0x01
#define SMD_TYPE_N			0x02
#define SMD_TYPE_GROUP		0x10

// ＣＯＢＯＬソース出力用
#define CHAR_X				(LPCTSTR)"PIC X(%02d)"
#define CHAR_9				(LPCTSTR)"PIC 9(%02d)"
#define CHAR_S				(LPCTSTR)"PIC S9(%02d)"
#define CHAR_F				(LPCTSTR)"PIC 9(%02d)V9(%02d)"
#define CHAR_SF				(LPCTSTR)"PIC S9(%02d)V9(%02d)"
#define CHAR_N				(LPCTSTR)"PIC N(%02d)"
#define CHAR_OCCURS			(LPCTSTR)"OCCURS %d TIMES"
#define CHAR_ERROR			(LPCTSTR)"%ERROR%"

// 圧縮項目詳細
#define COMP_BINARY			1
#define COMP_DECIMAL		2
#define COMP_BIT			3

// コード補完
#define SUP_PERIOD			1
#define SUP_STRUCT			2
#define SUP_FILLER			3
#define SUP_ERRCUT			4

// バイトオーダ入替え
#define HTONS(i)			(((WORD)i << 8) | ((WORD)i >> 8))
#define HTONL(i)			((((DWORD)i << 24) | ((DWORD)i >> 8 << 24 >> 8)) |	\
							(((DWORD)i << 8 >> 24 << 8) | ((DWORD)i >> 24)))

/*-------------------------------------------------------------------------
	構造体
-------------------------------------------------------------------------*/
// ＣＯＢＯＬソース情報
typedef struct tagSOURCEINFO{
	int nNo;								// 順番
	char szSourcePath[MAX_PATH];			// ソースファイルパス
	char *pszSourceCode;					// ソースコード
	DWORD dwSourceSize;						// ソースサイズ
	char szParam[MAX_PATH];					// 追加データ1
	LPVOID lpParam;							// 追加データ2
} SOURCEINFO, *LPSOURCEINFO;

// ＣＯＢＯＬ解析結果１トークン分
typedef struct tagTOKEN_CBL
{
	int nSrcNo;								// ソース情報の番号
	int nPhyLine;							// 物理行
	int nLogLine;							// 論理行
	BYTE btType;							// COBOL文法種別
	char szDescriptor[MAX_DESCRIPT_LEN];	// 識別子
	int nDescriptLen;						// 識別子の文字数
	DWORD dwDescriptorType;					// 変数の種別
	long lp1;								// 拡張用
	long lp2;								// 拡張用
} TOKEN_CBL, *LPTOKEN_CBL;

#define PHY_UNSIGNDEC		0				// 符号なし数字
#define PHY_SIGNDEC			1				// 符号あり数字
#define PHY_CHAR			2				// キャラクタ文字
#define PHY_JAPAN			3				// ２バイト文字
#define PHY_BIN				4				// バイナリ文字

typedef struct tagPHYDATA
{
	int nType;								// 型
	int nHiLen;								// 整数部の大きさ
	int nLowLen;							// 少数点以下の大きさ
	int nSign;								// 符号でｎバイト
} PHYDATA, *LPPHYDATA;

// ＣＯＢＯＬ解析結果１レコード分
typedef struct tagRECORD_CBL
{
	int nSrcNo;								// ソース情報の番号
	int nPhyLine;							// 物理行
	int nLogLine;							// 論理行
	unsigned int unLevel;					// レベル番号
	BOOL bStruct;							// 構造体項目
	unsigned int unStructGroupId;			// 構造体項目識別用ＩＤ
	int nStructGroupIndex;					// 親構造体識別ＩＤ(先頭は－１)
	unsigned int unStructGroupCount;		// 同レベル構造体数
	char szDescriptor[MAX_DESCRIPT_LEN];	// 識別子
	char szTypeDef[MAX_DESCRIPT_LEN];		// 変数サイズ定義
	DWORD dwTypeSize;						// 変数のバッファサイズ
	DWORD dwTypePos;						// 変数のバッファ位置
	DWORD dwType;							// 変数の種別
	BOOL bOccurs;							// 繰り返し項目
	char szOccursIndexed[MAX_DESCRIPT_LEN];	// 繰り返し項目の指数定義
	int nOccurs;							// 繰り返し回数
	int nOccursOrder;						// 繰り返しの何番目(※０始り)
	unsigned int unOccursGroupId;			// 繰り返し項目識別用ＩＤ
	int nOccursGroupIndex;					// 繰り返し項目識別用ＩＤ内枝番
	BOOL bRedefines;						// 再定義項目
	char szRedefines[MAX_DESCRIPT_LEN];		// 再定義項目
	int nRedefinesLine;						// 再定義項目元の論理行位置
	int nRedefinesRecIndex;					// 再定義項目元のレコード位置
	unsigned int unRedefinesGroupId;		// 再定義項目識別用ＩＤ
	int nRedefinesGroupIndex;				// 再定義項目識別用ＩＤ内枝番
	BYTE btCompress;						// バイナリ（圧縮？）項目
	char szCompress[MAX_DESCRIPT_LEN];		// 32bit変数定義
	char szValue[MAX_DESCRIPT_LEN];			// データ
	char szValueEx[MAX_DESCRIPT_LEN];		// データ拡張
	PHYDATA phyd;							// 実(?)サイズ
	char szComment[MAX_COMMENT_LEN];		// コメント
	int nSupplementation;					// 補完コード
	long lp1;								// 拡張用
	long lp2;								// 拡張用
} RECORD_CBL, *LPRECORD_CBL;

// ＦＯＲＭ解析結果１レコード分
typedef struct _tagRECORD_FORM {
	WORD wItenNo;
 	BYTE btLevel;
	int nLevelLen;
 	BYTE btName[MAX_DESCRIPT_LEN];
	WORD wItemLen;
	WORD wItemType;
	BYTE btFloatLen;
	WORD wOccurs;
	BOOL bSigned;
} RECORD_FORM, *LPRECORD_FORM;

// FUJITSU FORM(smed)ファイル形式
#pragma pack(1)
// ファイルヘッダ
typedef struct _tagSMD_HEAD {
	BYTE btSmedName[8];
	BYTE btFiller1[12];
	WORD wSizeY;
	WORD wSizeX;
	BYTE btFiller2[50];
	WORD wOutItemCount;
	BYTE btFiller3[2];
	WORD wFixedItemCount;
	BYTE btFiller4[80];
	BYTE btFiller5[2];
	WORD wGroupAdr;
	BYTE btFiller6[2];
	WORD wItemAdr;
	BYTE btFiller7[6];
	WORD wItemDataAdr;
	BYTE btFiller8[2];
	WORD wItemDataAdr2;
	BYTE btFiller9[2];
	WORD wGlDataAdr;
	BYTE btFiller10[6];
	WORD wExDataAdr;
	BYTE btFiller11[6];
	WORD wOvdNameAdr;
	BYTE btFiller12[6];
	WORD wRecNameAdr;
	BYTE btFiller13[2];
	WORD wRecDataAdr;
} SMD_HEAD, *LPSMD_HEAD;

// 項目(変数)の名前
typedef struct _tagSMD_ITEMNAME {
	BYTE btName[6];
	WORD wPos;
} SMD_ITEMNAME, *LPSMD_ITEMNAME;

// 項目の詳細な属性
typedef struct _tagSMD_ITEMDATA {
	WORD wGlPos;
	WORD wExPos;
	WORD wLine;
	WORD wColumn;
	WORD wItemLen;
	WORD wRecLen;
	WORD wRecPos;
	WORD wItemNo;
} SMD_ITEMDATA, *LPSMD_ITEMDATA;

// 大域的な項目の属性
typedef struct _tagSMD_GLITEMDATA {
	BYTE btTblLen;
	BYTE btReserved1;
	BYTE btItemType;
	BYTE btItemAttr;
	WORD wAttribute;
	BYTE btAttrEx;
	BYTE btReserved2;
	WORD wEdit;
	BYTE btEditChar;
	BYTE btFloatLen;
} SMD_GLITEMDATA, *LPSMD_GLITEMDATA;

// レコード定義の属性
typedef struct _tagSMD_RECDATA {
	BYTE btDiscript;
	BYTE btLevelNo;
	WORD wItrmLen;
	BYTE btItrmType;
	BYTE btFiller1;
	WORD wItrmOccurs;
	WORD wItemFiller1;
	WORD wItemFiller2;
	WORD wItemFiller3;
	WORD wItemFiller4;
	BYTE btItemName[8];
} SMD_RECDATA, *LPSMD_RECDATA;

// フォームオーバレイの定義
typedef struct _tagSMD_OVDNAMES {
	BYTE btOvdName[10][9];
} SMD_OVDNAMES, *LPSMD_OVDNAMES;
#pragma pack()

/*-------------------------------------------------------------------------
	クラス
-------------------------------------------------------------------------*/
class CLexCBL;
class CParserCBL;
class CAnaForm;

// 解析全般統合クラス
class CAnaCBL
{
private:
	SYSTEMTIME m_stAnalysys;
	CLexCBL *m_lpCLexCBL;
	CParserCBL *m_lpCParserCBL;
	CAnaForm *m_lpCAnaForm;
	DWORD m_dwLex, m_dwParser,
		m_dwLexForm, m_dwCreateSource;
	int m_nCodeCheck;
	BOOL m_bAnalysys;
	std::vector<LPTOKEN_CBL>::size_type m_nTokenIndex;
	std::vector<LPRECORD_CBL>::size_type m_nRecordIndex;
	std::vector<LPRECORD_FORM>::size_type m_nFormRecordIndex;

protected:

public:

private:
	void SetWindowHandle(HWND hWndMain);

protected:

public:
	CAnaCBL();
	virtual ~CAnaCBL();

	// 解析中？
	BOOL IsAnalysys() { return m_bAnalysys; };
	// 初期化
	BOOL Initialize(HWND hWndMain);
	// クリーンアップ
	void Clean();
	// ＣＯＢＯＬソース設定
	int AddSource(const char *pszSourcePath);
	int AddSourceCode(const char *pszSourceCode);
	// ＳＭＥＤソース設定
	int AddForm(const char *pszFormPath,
		const char *pszCobolSource,
		const int nSeq = 6);
	// 設定したソースの除外
	int RemSource(const int nNo);
	// 設定したソースの再読込み
	int ReloadSource(const int nNo);
	// 解析するソース設定の置き換え
	int RepSource(const int nNo, const char *pszSourcePath);
	int RepSourceCode(const int nNo, const char *pszSourceCode);
	// ＣＯＢＯＬソース解析
	BOOL Analysys(const int nSeq = 6,
		const int nMaxLine = 0,
		const BOOL bHostMode = FALSE,
		char *pSign = NULL,
		const int nCompPos = 0,
		const BOOL bCodeCheck = TRUE);
	// ＣＯＢＯＬソース解析中止
	void CancelAnalysys();
	// ＣＯＢＯＬレコード情報を追加する(※危険)
	BOOL AddRecordData(LPRECORD_CBL lpRecCbl);

	// 解析時間取得
	DWORD GetLexTime() const { return m_dwLex; };
	DWORD GetParserTime() const { return m_dwParser; };
	DWORD GetLexFormTime() const { return m_dwLexForm; };
	DWORD GetCreateSourceTime() const { return m_dwCreateSource; };

	// 設定したソースの数
	int GetSourceCount();

	// ソース情報
	int GetSourceData(const int nIndex, LPSOURCEINFO lpSrcInfo);
	int GetSourcePath(const int nIndex, char *pszSourcePath, const int nSize);

	// ソース解析（連結）順の入替
	int SwapSourceNo(const int nNewNo, const int nOldNo);

	// 解析時間
	BOOL GetTime(LPSYSTEMTIME lpst) { CopyMemory(lpst, &m_stAnalysys, (DWORD)sizeof(SYSTEMTIME)); };

	// 解析した合計サイズ取得
	DWORD GetTotal();

	// 文法エラー検出件数
	int GetCodeCheckCount() { return m_nCodeCheck; };

	// ＣＯＢＯＬトークン取得
	int GetCount_Token();
	LPTOKEN_CBL GetFirst_Token();
	LPTOKEN_CBL GetNext_Token();
	LPTOKEN_CBL Get_Token(const int nIndex);
	std::vector<LPTOKEN_CBL> &Get_AllTokens();
	// ＣＯＢＯＬレコード取得
	int GetCount_Record();
	LPRECORD_CBL GetFirst_Record();
	LPRECORD_CBL GetNext_Record();
	LPRECORD_CBL Get_Record(const int nIndex);
	std::vector<LPRECORD_CBL> &Get_AllRecords();
	// ＦＯＲＭレコード取得
	int GetCount_FormRecord();
	LPRECORD_FORM GetFirst_FormRecord();
	LPRECORD_FORM GetNext_FormRecord();
	LPRECORD_FORM Get_FormRecord(const int nIndex);
	// 最大、最小レベル番号
	unsigned int GetMaxLevelNo();
	unsigned int GetMinLevelNo();
};

// 共通基底クラス
class CBaseCBL
{
private:

protected:
	// マルチメディアタイマ起動
	UINT m_uMMResult;
	// 処理中断フラグ
	BOOL m_bCancel;
	// 処理ステージ
	int m_nStage;

public:

private:

protected:
	virtual int InitAnalysys(int nStage, int nParam = 0) = 0;
	virtual int EndAnalysys(int nParam = 0) = 0;

public:
	CBaseCBL();
	virtual ~CBaseCBL();

public:
	virtual int AddTrace(const char *pszTrace, const int nFlag = TRACE_INFO, const int nLine = -1) = 0;

public:
	// キャンセル
	void Cancel();
	// キャンセル？
	BOOL IsCancel() const { return m_bCancel; };
};

// 共通基底クラス実体
// ------------------- このクラスを修正すればいいんじゃね？ -----------------------
class CComCBL : public CBaseCBL
{
private:
	HWND m_hWndMain;

protected:

public:

private:

protected:
	// オーバーライド
	virtual int InitAnalysys(int nStage, int nParam = 0);
	virtual int EndAnalysys(int nParam = 0);

public:
	CComCBL();
	virtual ~CComCBL();

public:
	// オーバーライド
	virtual int AddTrace(const char *pszTrace, const int nFlag = TRACE_INFO, const int nLine = -1);

public:
	void SetWindowHandle(HWND hWndMain);
	BOOL IsAnalysys() { return (m_nStage >= 0); };
};
// ----------------------------- 修正ここまで ------------------------------------

// 構文解析クラス
class CLexCBL : public CComCBL
{
private:
	// ソース情報
	std::vector<LPSOURCEINFO> m_lpSrcInfo;
	// 解析対象ソースコード
	char *m_pszSource;
	DWORD m_dwSourceSize;
	// 行番号の桁数
	int m_nSeqLen;
	// 行最大桁数
	int m_nLineMax;
	// M-HOST変数サイズ計算モード
	BOOL m_bHostSize;
	// 数字定数とレベル番号を判別する為のフラグ
	BOOL m_bLevel;
	// ピリオド無しを判定するためのフラグ
	BOOL m_bEol;
	// エラーとして扱わない記号
	char m_cSign[MAX_SIGN];
    // 物理・論理行位置
	int m_nPhysLine ,m_nLogiLine;

protected:
	// トークン格納領域
	std::vector<LPTOKEN_CBL> m_lpTok_cbl;

public:

private:
	// トークン格納配列のクリア
	void ClearToken();
	// ソース番号からインデクス取得
	int GetIndexFromNo(const int nNo);
	// 配列インデクスからソース番号取得
	int GetNoFromIndex(const int nIndex);
	// リナンバ
	void SourceInfoRenumber();
	// ソース配列のソート用ファンクタ
	static bool Pred_SrcNo(LPSOURCEINFO lpSrc1, LPSOURCEINFO lpSrc2){ return (lpSrc1->nNo < lpSrc2->nNo); };

protected:
	// トークン格納
	int TokenCBL_Push(int nSrcNo, int nPhyLine, int nLogLine, BYTE btType, char *pszDescriptor, int nDescriptLent);
	// ソース読込み
	int ReadSource(LPSOURCEINFO lpSrcInfo);
	// 指定されたソースコードの加工
	int FactSource(const std::vector<LPSOURCEINFO>::size_type nSrcNo);
	// 除外記号設定
	void SetSign(char *p);
	// ﾄｰｸﾝ分解
	DWORD Lex_sub(const std::vector<LPSOURCEINFO>::size_type nSrcNo);
	// 除外記号判断
	BOOL IsSign(char c);
	// コメント判断
	int IsComment(int nSrcNo, char *p);
	// レベル判断
	int IsLevelNo(int nSrcNo, char *p);
	// 変数名判断
	int IsIdentifier(int nSrcNo, char *p);
	// 変数型判断(PIC)
	int IsDescriptorType(int nSrcNo, char *p);
	// 変数サイズ定義判断
	int IsPicSize(int nSrcNo, char *p);
	// 再定義項目判断(REDEFINES)
	int IsRedefines(int nSrcNo, char *p);
	// 再定義項目判断(OCCURES)
	int IsOccurs(int nSrcNo, char *p);
	// 変数の拡張設定判断(BIT)
	int IsBit(int nSrcNo, char *p);
	// 変数の拡張設定判断(COMP-n)
	int IsBinary(int nSrcNo, char *p);
	// 算術演算記号の扱い
	int IsSeparate(int nSrcNo, char *p);
	// 変数の拡張設定判断(VALUE)
	int IsValue(int nSrcNo, char *p);
	// 数値定数判断
	int IsNumericConst(int nSrcNo, char *p);
	// 文字列定数判断
	int IsCharConst(int nSrcNo, char *p);
	// 予約語だが特に解析に関係ないもの
	int IsNullConst(int nSrcNo, char *p);
	// 区切文字判断
	int IsEOL(int nSrcNo, char *p);
	// 空白読み飛ばし
	int SkipBlank(char *p);
	// 行末まで読み飛ばし
	int SkipEOL(char *p);
	// 数字判断
	BOOL IsNumeric(char c);
	BOOL IsNumericPlus(char c);
	// 英字判断
	BOOL IsAlphabet(char c);
	// 日本語判断
	BOOL IsJapan(char c);

public:
	// コンストラクタ
	CLexCBL();
	// デストラクタ
	~CLexCBL();

	// コードから解析するソース設定
	int AddSourceCode(const char *pszSource, DWORD dwSize, const char *pszParam = NULL, const LPVOID lpParam = 0);
	// ファイルから解析するソース設定
	int AddSourceFile(const char *pszSourcePath, const char *pszParam = NULL, const LPVOID lpParam = 0);
	// 解析するソース設定の再読込み
	int ReloadSourceFile(const int nNo);
	// 解析するソース設定の全再読込み
	int ReloadAllSourceFile();
	// 解析するソース設定の削除
	int RemoveSource(const int nNo);
	// 解析するソース設定の全削除
	int RemoveAllSources();
	// 解析するソース設定順の入替
	int SwapSourceNo(const int nNewNo, const int nOldNo);
	// 解析するソース設定の置き換え
	int ReplaceSourceFile(const int nNo,
		const char *pszSourcePath,
		const char *pszParam = NULL,
		const LPVOID lpParam = 0);
	int ReplaceSourceCode(const int nNo,
		const char *pszSource, DWORD dwSize,
		const char *pszParam = NULL,
		const LPVOID lpParam = 0);
	// ソース構文解析
	DWORD Lex(const int nSeq = 6,
		const int nMaxLine = 0,
		const BOOL bHostMode = FALSE,
		char *pSign = NULL);
	// トークン全配列の取得
	std::vector<LPTOKEN_CBL> &GetToken() { return m_lpTok_cbl; };
	// トークンの取得
	LPTOKEN_CBL GetToken(const int nToken);
	// トークンの総数取得
	int GetTokenCount(){ return (int)(m_lpTok_cbl.size()); };
	// 完了確認
	BOOL ResultLex() const { return (IsCancel() ? FALSE : TRUE); };
	// ソース情報取得
	int GetSourceCount() { return (int)m_lpSrcInfo.size(); };
	int GetSourceInfo(const int nIndex, LPSOURCEINFO lpSrcInfo);
	void SortSourceInfo() { std::sort(m_lpSrcInfo.begin(), m_lpSrcInfo.end(), Pred_SrcNo); };

};

// 意味解析クラス
class CParserCBL : public CComCBL
{
private:
	// サイズ計算サブルーチン
	DWORD CompSize(char *pszDec);
	DWORD CompComma(char *pszDec);

	// 変数位置計算
	int m_nComp;
	// 最大、最小レベル番号
	unsigned int m_unMaxLevel, m_unMinLevel;
	// ワードチェック
	int m_nWordCheck;

protected:
	// レコード格納領域
	std::vector<LPRECORD_CBL> m_lpRec_cbl;
	// トータルサイズ
	DWORD m_dwTotalSize;
	// 構造体項目の数
	int m_nStructCount;
	// 繰り返し項目の数
	int m_nOccursCount;
	// 再定義項目の数
	int m_nRedefinesCount;

public:

private:
	// レコード格納配列のクリア
	void ClearRecord();

protected:
	// 変数の型判定
	DWORD GetTypeOf(LPTOKEN_CBL lptok);
	// 変数のサイズ計算
	DWORD GetSizeOf(LPTOKEN_CBL lptok, DWORD dwPicType, LPPHYDATA lpphyd);
	DWORD GetSizeOfBit(LPRECORD_CBL lptok);
	DWORD GetSizeOfBinary(LPRECORD_CBL lptok);
	DWORD GetSizeOfHex(LPRECORD_CBL lptok);
	// ＣＯＢＯＬ文法的な問題チェック
	virtual int CobolWordCheck();
	// 構造体項目の展開
	int DevStruct();
	// 繰り返し項目の展開
	int DevOccurs();
	// 再定義項目の展開
	int DevRedefines();
	// 構造体のサイズ計算
	DWORD GetStructSizeOf(int nIdx);
	// 変数のバイト位置計算
	DWORD GetPosOf();
	// バイナリ変換
	DWORD Hex2Dwrod(const char *pszHex);

public:
	// コンストラクタ
	CParserCBL();
	// デストラクタ
	virtual ~CParserCBL();

	// レコード情報の追加
	void PushRecord(LPRECORD_CBL lpRecCbl);
	// ソース意味解析
	DWORD Parser(std::vector<LPTOKEN_CBL> &m_lpTok_cbl,
		const int nCompPos = 0,
		const BOOL bWordCheck = TRUE);
	// レコード全配列の取得
	std::vector<LPRECORD_CBL> &GetRecord() { return m_lpRec_cbl; };
	// 合計サイズ
	DWORD GetTotal() const { return m_dwTotalSize; };
	// レコードの取得
	LPRECORD_CBL GetRecord(const int nRecord);
	// レコードの総数取得
	int GetRecordCount(){ return (int)(m_lpRec_cbl.size()); };
	// 最大、最小レベル番号
	unsigned int GetMaxLevel() { return m_unMaxLevel; };
	unsigned int GetMinLevel() { return m_unMinLevel; };
	// チェックカウント
	int GetWordCheckCount() { return m_nWordCheck; };
	// 完了確認
	BOOL ResultParser() const { return (IsCancel() ? FALSE : TRUE); };

};

// 富士通ＦＯＲＭ解析クラス
class CAnaForm : public CComCBL
{
private:
	char m_szFromFile[MAX_PATH];
	char m_szCobolFile[MAX_PATH];
	DWORD m_dwFormSize;
	DWORD m_dwCOBOLSize;
	LPBYTE m_lpbtFormData;

	WORD m_wSizeY;
	WORD m_wSizeX;
	WORD m_wOutItemCount;
	WORD m_wFixedItemCount;

protected:
	// レコード情報
	std::vector<LPRECORD_FORM> m_lpRecForm;
	// オーバレイ定義名
	int m_nOvdNames;
	SMD_OVDNAMES m_stOvdNames;
	// ＦＯＲＭ解析完了フラグ
	BOOL m_bComplete;

public:

private:
	void ClearBuffer();

protected:
	int AnaForm();									// ＦＯＲＭ解析
	int AnaForm_RecordSet(LPSMD_HEAD lpHead);
	int AnaForm_NoRecordSet(LPSMD_HEAD lpHead);
	LPSMD_HEAD GetHeader();							// ヘッダ部取得
	BOOL CheckHeader(const LPSMD_HEAD lpHead);		// ヘッダ部チェック
	LPSMD_RECDATA GetRecData(const WORD wItemNo,
			   const LPSMD_HEAD lpHead);			// レコード定義部取得
	int GetItemCount(const LPSMD_HEAD lpHead);		// 項目数取得
	LPSMD_ITEMNAME GetItemName(const WORD wItemNo,
				const LPSMD_HEAD lpHead);			// 項目名部取得
	LPSMD_ITEMDATA GetItemData(const LPSMD_ITEMNAME lpItemName);
													// 項目属性部取得
	LPSMD_GLITEMDATA GetGlData(const LPSMD_ITEMDATA lpItemData);
													// 大域属性部取得
	int GetOvdNames(LPSMD_HEAD lpHead);				// オーバレイ定義取得
	BOOL MakeRec(const LPRECORD_FORM lpRecForm,
				char *pszRec,
				int nMaxBuffer,
				int nSeqLen);						// ＣＢＬソースコード作成

public:
	CAnaForm();										// コンストラクタ
	virtual ~CAnaForm();							// デストラクタ

	DWORD LexForm(const char *pszFormFile);			// ＦＯＲＭ解析
	DWORD CreateSource(const char *pszCBLFile, 
				int nSeqLen);						// ＣＢＬソース出力
	BOOL RemoveSource();							// ＣＢＬソース削除
	// レコードの取得
	LPRECORD_FORM GetRecord(const int nRecord);
	// レコードの総数取得
	int GetRecordCount(){ return (int)(m_lpRecForm.size()); };
	// 完了確認
	BOOL ResultLexForm() const { return (IsCancel() ? FALSE : TRUE); };
	// ＦＯＲＭソース取得
	char *GetForm() { return m_szFromFile; };
	// ＣＯＢＯＬソース取得
	char *GetCobolSource() { return m_szCobolFile; };
};

#endif	// _CANACBL_
