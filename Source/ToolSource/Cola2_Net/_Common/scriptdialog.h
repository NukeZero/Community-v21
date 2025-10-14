#ifndef __SCRIPTDIALOG_H
#define __SCRIPTDIALOG_H

#include "script.h"
#include "timer.h"

class CFieldMessage;
class CObj;

class CWndDialog;

typedef struct tagScript
{
	//CString m_strKey;
	TCHAR  m_szWord[ 64 ];
	TCHAR  m_szKey[ 64 ];
	TCHAR* m_pScript;
	BOOL   m_bStatus;
	BOOL   m_bHidden;
} 
SCRIPT,* LPSCRIPT;

class CScriptDialog : public CObject
{

	CScript m_smc;
	TCHAR m_szFileName[ 64 ];
public:
#ifdef __V050322_CACHEQUEST
//	CScript m_script;
	//int		m_globalArray[MAX_GLOBAL_ARRAY];
	CTimer	m_aTimer     [MAX_SCRIPT_TIMER];
	int		m_anGlobalVal[MAX_GLOBAL_ARRAY];
#endif
#ifdef __CLIENT
	CPtrArray m_scriptMarkAry;
#endif
	CPtrArray m_scriptAry;

	CScriptDialog();
	CScriptDialog(LPCTSTR lpszFileName); 
	~CScriptDialog();
#ifdef __CLIENT
	void SetMark();
	void GoMark();
#endif
	void Free();
	BOOL IsEmpty();
	void AddScriptButton(CWndDialog* pWndDialog,CObj* pObj);
	BOOL InterpretScript(CScript& smc);
	BOOL LoadScript(LPCTSTR lpszFileName); 
	//void SetAllScript(BOOL bStatus);
	LPSCRIPT Lookup(LPCTSTR pKey);
	LPSCRIPT Lookup(int nId);

#ifdef __WORLDSERVER
	static	char	s_pszName[64];
	static	char	s_pszKey[260];
	static	char	s_pszBeforeRun[260];
	static	char	s_pszAfterRun[260];
	static	void	SetLatestDialog( const char* pszName, const char* pszKey )
		{	lstrcpy( s_pszName, pszName );	lstrcpy( s_pszKey, pszKey );	}
	static	void	SetBeforeRun( const char* pszBeforeRun )
		{	lstrcpy( s_pszBeforeRun, pszBeforeRun );	}
	static	void	SetAfterRun( const char* pszAfterRun )
		{	lstrcpy( s_pszAfterRun, pszAfterRun );	}

#endif	// __WORLDSERVER

	// 처음 시작 
#ifdef __V050322_CACHEQUEST
	BOOL Run( LPCTSTR pKey, int* nResult, int nValue = 0, int nDstId = 0, int nSrcId = 0, int nQuestId = 0, int nCaller = WES_DIALOG );
	BOOL Run( int nId, int* nResult, int nValue = 0, int nDstId = 0, int nSrcId = 0, int nQuestId = 0, int nCaller = WES_DIALOG );
#else // __V050322_CACHEQUEST
//{{AFX
	BOOL Run(LPCTSTR pKey,int* nResult,int nValue = 0,int nGlobal2 = 0,int nGlobal3 = 0,int nQuestId = 0 );
	BOOL Run(int nId,int* nResult,int nValue = 0,int nGlobal2 = 0,int nGlobal3 = 0,int nQuestId = 0 );
//}}AFX
#endif // __V050322_CACHEQUEST

/*
#ifdef __V050322_CACHEQUEST
	// 펑션 콜 
	BOOL Call(LPCTSTR pKey,int* nResult,int nGlobal1 = 0,int nGlobal2 = 0,int nGlobal3 = 0);
	BOOL Call(LPCTSTR pKey,int* nResult,CUIntArray& globalArray);
	BOOL Call(int nId,int* nResult,int nGlobal1 = 0,int nGlobal2 = 0,int nGlobal3 = 0);
	BOOL Call(int nId,int* nResult,CUIntArray& globalArray);
#endif
	*/
};
#endif 
