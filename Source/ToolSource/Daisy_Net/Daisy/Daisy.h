// Daisy.h : main header file for the DAISY application
//

#if !defined(AFX_DAISY_H__3455336E_4789_45C8_92BD_CC45A98A15B5__INCLUDED_)
#define AFX_DAISY_H__3455336E_4789_45C8_92BD_CC45A98A15B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "DaisyVer.h"

/////////////////////////////////////////////////////////////////////////////
// CDaisyApp:
// See Daisy.cpp for the implementation of this class
//

class CDaisyApp : public CWinApp
{
public:
	CString m_strResourcePath;

	CDaisyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDaisyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDaisyApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CDaisyApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAISY_H__3455336E_4789_45C8_92BD_CC45A98A15B5__INCLUDED_)
