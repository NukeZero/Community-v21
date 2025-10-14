#ifndef __SMC_H
#define __SMC_H

#include "scanner.h"
#include "timer.h"
 
#define NUM_FUNC 	  	  10 //
#define NUM_GLOBAL_VARS 10 // 
#define NUM_LOCAL_VARS  10 //
#define NUM_BLOCK		    10 //
#define ID_LEN          31
#define FUNC_CALLS      31
#define NUM_PARAMS      31
#define LOOP_NEST       31


class CMemDC;
class CFieldMsg;

#ifdef __V050322_CACHEQUEST
#define SMC_VAL    0
#define SMC_DST_ID 1
#define SMC_SRC_ID 2
#define SMC_CALLER 3
#define SMC_FUNC   4
#define SMC_QUEST  5
#else // __V050322_CACHEQUEST
//{{AFX
#define SMC_VAL    0
#define SMC_SRC_ID 1
#define SMC_DST_ID 2
#define SMC_CALLER 3
#define SMC_FUNC   4
#define SMC_QUEST  5
//}}AFX
#endif // __V050322_CACHEQUEST

/////////////
/*
 *  These are the constants used to call sntx_err()
 *  when a syntax error occurs. Add more if you like.
 *  NOTE: SYNTAX is a generic error message used when nothing else
 *  seems appropriate.
 */
enum ErrorMsg
{
	SYNTAX,UNBAL_PARENS,NO_EXP,EQUALS_EXPECTED,NOT_VAR,PARAM_ERR,
	SEMI_EXPECTED,UNBAL_BRACES,FUNC_UNDEF,TYPE_EXPECTED,NEST_FUNC,
	RET_NOCALL,PAREN_EXPECTED,WHILE_EXPECTED,QUOTE_EXPECTED,
	NOT_TEMP,TOO_MANY_LVARS
};
/*
 *  An array of these structures will hold
 *  the info associated with global variable.
 */
struct VarType 
{
	char varName[64];
	int  varType;
	int  value;
};
/*
 *  this is the function call stack.
 */
struct FuncType 
{
	char name[64];
	char *loc; /* location of function entry point in file */
};
/*
 * key word table
 */
struct Command 
{
	char command[64];
	Toke tok;
};
/*
 * define table
 */
struct Define 
{
	char name[64];
	char *data;
};

int CopyChar( CHAR** ppSrc, CHAR** ppDst );


//class CScanner;

////////////////////////////////////////////////////////////////////////////////
// 
// Parsor : 계산기

//
class CParser : public CScanner
{
protected:

	void EvalExp1(int *value);
	void EvalExp2(int *value);
	void EvalExp3(int *value);
	void EvalExp4(int *value);
	void EvalExp5(int *value);
	void EvalExp6(int *value);

	// override

virtual void EvalExp0(int *value);
virtual void atom(int *value);
virtual	void SntxErr( LPCTSTR lpszHeader, int error);

public:

	CParser() { }
	~CParser() { }

	void Compute(LPTSTR initProg,int* value);
	void Compute(int* value);
	void operator () (LPTSTR initProg,int* value);
	void operator () (int* value);
};
struct LblType 
{
	char name[64];
	char *loc;
	int  block;
};
struct FuncInfo 
{
	int var;
	int label;
	int answer;
	int block;
};
struct AnswerInfo 
{
	char text [256];
	char label[64];
	BYTE attrib   : 1;
	BYTE selected : 1;
};

class CMapDefineToPtr : public CMapStringToPtr 
{
public:
	CMapDefineToPtr() {	InitHashTable(3907); }
	//{	InitHashTable(11); }
	~CMapDefineToPtr();
	void Free();
/*
	int GetCount() const;
	BOOL IsEmpty() const;
	BOOL Lookup(LPCTSTR key, void*& rValue) const;
	BOOL LookupKey(LPCTSTR key, LPCTSTR& rKey) const;
	//void*& operator[](LPCTSTR key);
	void SetAt(LPCTSTR key, void* newValue);
	BOOL RemoveKey(LPCTSTR key);
	void RemoveAll();
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, CString& rKey, void*& rValue) const;
	void AddDefineMap(LPCTSTR lpDefineFileName,BOOL bGlobalDef);
	*/
};
#define MAX_DEFINE_MAP   10
#define MAX_GLOBAL_ARRAY 16
#define MAX_SCRIPT_TIMER 6 
////////////////////////////////////////////////////////////////////////////////
//
// CScript : 인터프리터
//
class CScript : public CParser
{

	BOOL  process        ; // 0 : 인터프리트, 1 : 컴파일 
	BOOL  m_bOldWideChar;
	BOOL  m_bMarkInDefine;
	CHAR* m_pOldProg; 
	char* m_pDefineBuff;
	char  cmd;

	char LookUp(char *s);
	int  InternalFunc(char *s);
	
	char* FindFunc(char *name);
	void DeclGlobal();
	void DeclLocal();
	void FindLabel();
	void Call();
	void GetArgs();
	void GetParams();
	void FuncRet();
	void LocalPush(VarType i);
	FuncInfo FuncPop();
	void FuncPush(FuncInfo fi);
	void AssignVar(char *varName, int value);
	int* GetVarPtr(char *varName);
	int  FindVar(char *s);
	int  IsVar(char *s);
	void ExecDefine( BOOL bLocalDef = FALSE );
	void ExecIf();
	void ExecWhile();
	void ExecDo();
	void ExecFor();
	void ExecSwitch();
	void ExecAnswer();
	char*ExecSelect();
	char*ExecYesNo();
	
	int  IsFunc(char *str);

	// override

	virtual	void EvalExp0(int *value);
	virtual	void atom(int *value);

	int SelectLabel(CMemDC* pMemDC,CFieldMsg* pFieldMsg,CRect* pLabelRect,CString* pLabelStr,int nNum);
	CFieldMsg* SetLabelFontStatus( CMemDC* pMemDC,int nLine);

#ifndef __V050322_CACHEQUEST
	static int  m_nRunScript;
	static BOOL m_bQuitDialog;
	static BOOL m_bPressDialog;
	static BOOL m_bEnableDialog;
#endif

public:
	enum  { INTERPRITER, COMPILER };

	VarType     globalVars   [NUM_GLOBAL_VARS];
	VarType     localVarStack[NUM_LOCAL_VARS ];
	FuncType    funcTable    [NUM_FUNC	     ];
	LblType     labelTable   [45			 ];
	FuncInfo    callStack    [NUM_FUNC	     ];
	CString     m_strDefine     ;

	int  functos      ; // index to top of function call stack
	int  answerCnt    ; // answer 카운터
	int  funcIndex ; // index to into function table
	int  gVarIndex ; // index into global variable table
	int  lvartos   ; // index into local variable stack
	int  labeltos  ; // index into local label table
	int  retValue  ; // function return value
	int  retSign   ; // return sing
	int  blocktos  ; //
	int  labelIndex;

	static	BOOL	m_bKeepGlobalDef;

#ifdef __V050322_CACHEQUEST
	CTimer*		m_paTimer;
	int*		m_panGlobalVal;
	int			m_anInternalVal[MAX_GLOBAL_ARRAY];
#else
static BOOL		m_bWaitServerResponse;
	int         m_globalArray[MAX_GLOBAL_ARRAY];
#endif

#ifdef __USE_STR_TBL0407
	CString		m_strKey;
#endif	// __USE_STR_TBL0407

	CScript( BOOL bComma = FALSE ); 
	~CScript();
	void Free();
	virtual void SntxErr( LPCTSTR lpszHeader, int error);

	// Define
	CString GetDefine() { return m_strDefine; }
static BOOL LookupDefine( LPCTSTR lpszString, void*& rValue );
static BOOL  RemoveDefine( LPCTSTR lpszDefine );
static BOOL  AddDefine( LPCTSTR lpszDefine, int nData );
static BOOL  AddDefine( LPCTSTR lpszDefine, LPCTSTR lpszData );
static void  GetFindDefDataToArray( LPCTSTR lpStrDef, CPtrArray* pPtrArray );
static void  GetFindIdToArray ( LPCTSTR strDef, CStringArray* pStrArray );
static void  GetFindDefToArray( LPCTSTR strDef, CStringArray* pStrArray );
static LPSTR GetDefineStr( LPCTSTR lpStr );
static int   GetDefineNum( LPCTSTR lpStr );
static CString GetFindIdStr( LPCTSTR lpStr, int nNum );
static void  FreeGlobalDefine();

static CMapDefineToPtr m_globalDef;
static CMapDefineToPtr m_localDef;

#ifdef __USE_STR_TBL0407
static map<string, string>	m_mapString;
#endif	// __USE_STR_TBL0407

	// access for smclib

	void FindEob();
	int  InterpBlock(int aaa = 0);
	void Compute(int *value);

	// operator

	BOOL Load( LPCTSTR lpszFileName, BOOL bMultiByte = TRUE, int nProcess = INTERPRITER );
	BOOL operator () ();
	BOOL Run();
	void PreScan( BOOL bLocalDef = FALSE );
	void Write(LPSTR* cplProg,char* str);
	void Compile();
	void SetCompilerMode() { process = COMPILER; }
	void SetInterpriterMode() { process = INTERPRITER; }
	BOOL IsCompilerMode() { return process == COMPILER; }
	BOOL IsInterpriterMode() { return process == INTERPRITER; }
	int  GetProcessMode() { return process; }

	virtual void SetMark();
	virtual void GoMark();

#ifndef __V050322_CACHEQUEST
static void QuitDialog     () { m_bQuitDialog = TRUE  ; } // 대화를 강제 종료시킨다. 
static BOOL IsQuitDialog   () { return m_bQuitDialog  ; } // 대화를 종료할 수 있는지 검사한다.
static BOOL IsPressDialog  () { return m_bPressDialog ; } // 선택 대화에서 눌렀는지 검사한다.
static BOOL IsEnableDialog () { return m_bEnableDialog; } // 다이얼로그가 가능한가?
static BOOL IsRunScript    () { return m_nRunScript ? TRUE : FALSE; } // 스크립트가 실행중인가?
static void EnableDialog   (BOOL bEnable = TRUE) { m_bEnableDialog = bEnable; } // 다이얼로그를 가능하게 할지 결정한다.
#endif
	// attribute
#ifdef __V050322_CACHEQUEST
	
	//int  GetIValue () { return m_globalArray[0]; }
	//int  GetIValue1() { return m_globalArray[0]; }
	//int  GetIValue2() { return m_globalArray[1]; }
	//int  GetIValue3() { return m_globalArray[2]; }
	//int  GetIValue4() { return m_globalArray[3]; }

	void SetIValue(int nVal) { m_anInternalVal[SMC_VAL] = nVal; }
	void SetIValue(int nVal,...);

	void SetVal    (int nVal ) {m_anInternalVal[SMC_VAL] = nVal; }
	void SetDstId  (int nVal ) {m_anInternalVal[SMC_DST_ID] = nVal; }
	void SetSrcId  (int nVal ) {m_anInternalVal[SMC_SRC_ID] = nVal; }
	void SetCaller (int nVal ) {m_anInternalVal[SMC_CALLER] = nVal; }

	int GetReturn () { return m_anInternalVal[SMC_VAL   ]; }
	int GetVal    () { return m_anInternalVal[SMC_VAL   ]; }
	int GetDstId  () { return m_anInternalVal[SMC_DST_ID]; }
	int GetSrcId  () { return m_anInternalVal[SMC_SRC_ID]; }
	int GetNpcId  () { return m_anInternalVal[SMC_DST_ID]; }
	int GetPcId   () { return m_anInternalVal[SMC_SRC_ID]; }
	int GetCaller () { return m_anInternalVal[SMC_CALLER]; }
	void* GetFunc () { return (void*)m_anInternalVal[SMC_FUNC]; }
	int GetQuest  () { return m_anInternalVal[SMC_QUEST]; }

	void SetFunc   (void* pFunc) { m_anInternalVal[4] = (int)pFunc; }

#else // __V050322_CACHEQUEST
//{{AFX
	int  GetReturn () { return m_globalArray[0]; }
	int  GetGlobal () { return m_globalArray[0]; }
	int  GetGlobal1() { return m_globalArray[0]; }
	int  GetGlobal2() { return m_globalArray[1]; }
	int  GetGlobal3() { return m_globalArray[2]; }
	int  GetGlobal4() { return m_globalArray[3]; }

	void SetGlobal(int nVal) { m_globalArray[0] = nVal; }
	void SetGlobal(int nVal,...);

	void SetVal    (int nVal   ) {m_globalArray[0] = nVal; }
	void SetSrcId  (int nVal   ) {m_globalArray[1] = nVal; }
	void SetDstId  (int nVal   ) {m_globalArray[2] = nVal; }
	void SetCaller (int nVal   ) {m_globalArray[3] = nVal; }

	int GetNpcId  () {	return m_globalArray[1]; }
	int GetPcId   () {	return m_globalArray[2]; }

	void SetFunc   (void* pFunc) {m_globalArray[4] = (int)pFunc; }
//}}AFX
#endif // __V050322_CACHEQUEST
	

//static void SetReplace(CString strFieldName,CPtZ ptz);
static void ClearReplace();
static BOOL IsReplace() { return !m_strReplaceFieldName.IsEmpty(); }
static CString m_strReplaceFieldName;
//static CPtZ   m_ptzReplacePosition;

	int GetTkn( BOOL bComma = FALSE );
	int GetToken( BOOL bComma = FALSE );
	int GetToken_NoDef( BOOL bComma = FALSE );
	void GetLastFull();
	virtual int GetLineNum( LPTSTR lpProg = NULL );
	//int GetToken_Def( BOOL bComma = FALSE );
}; 
struct InterFuncType 
{
	char* name;
	int (*p)(CScript *SMC);
	char* prototype;
};
extern Command table[];
extern InterFuncType interFunc[]; 

#define DefineNum(x) CScript::GetDefineNum(x)
#define DefineStr(x) CScript::GetDefineStr(x)

#endif
 
