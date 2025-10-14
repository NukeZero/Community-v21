// Cola.h : main header file for the COLA application
//

#if !defined(AFX_COLA_H__0751C781_C01E_4205_A0E3_8A8A0FCFC9A9__INCLUDED_)
#define AFX_COLA_H__0751C781_C01E_4205_A0E3_8A8A0FCFC9A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CColaApp:
// See Cola.cpp for the implementation of this class
//

class CColaApp : public CWinApp
{
	CString	m_strCmdFile;		// Cmdline에서 입력된 파일 

public:
	CColaApp();

	CString	GetCmdFile( void ) { return m_strCmdFile; }

	BOOL	Convert( void );
//	int		LoadINI( LPCTSTR szFileName );
//	int		SaveINI( LPCTSTR szFileName );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColaApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CColaApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CColaApp	*g_pApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLA_H__0751C781_C01E_4205_A0E3_8A8A0FCFC9A9__INCLUDED_)
