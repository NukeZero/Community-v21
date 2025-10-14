// Miranda.h : main header file for the MIRANDA application
//

#if !defined(AFX_MIRANDA_H__0A09BFA7_F4C5_4B83_8C4B_7C2F5D7057FF__INCLUDED_)
#define AFX_MIRANDA_H__0A09BFA7_F4C5_4B83_8C4B_7C2F5D7057FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMirandaApp:
// See Miranda.cpp for the implementation of this class
//

class CMirandaApp : public CWinApp
{
public:
	CMirandaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirandaApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMirandaApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIRANDA_H__0A09BFA7_F4C5_4B83_8C4B_7C2F5D7057FF__INCLUDED_)
