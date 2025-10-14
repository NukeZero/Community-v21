// sExt.h : main header file for the SEXT application
//

#if !defined(AFX_SEXT_H__6E8929CF_2FD5_4DD0_8DFA_5554502F469C__INCLUDED_)
#define AFX_SEXT_H__6E8929CF_2FD5_4DD0_8DFA_5554502F469C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSExtApp:
// See sExt.cpp for the implementation of this class
//

class CSExtApp : public CWinApp
{
public:
	CSExtApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSExtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSExtApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEXT_H__6E8929CF_2FD5_4DD0_8DFA_5554502F469C__INCLUDED_)
