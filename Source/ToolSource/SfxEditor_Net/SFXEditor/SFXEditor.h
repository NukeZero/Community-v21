// SFXEditor.h : main header file for the SFXEDITOR application
//

#if !defined(AFX_SFXEDITOR_H__98C4A328_CDAA_47DA_8BB2_A77C30BA910A__INCLUDED_)
#define AFX_SFXEDITOR_H__98C4A328_CDAA_47DA_8BB2_A77C30BA910A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorApp:
// See SFXEditor.cpp for the implementation of this class
//
class CSFXEditorApp : public CWinApp
{
public:
	CSFXEditorApp();
	TCHAR m_szPath[ 256 ];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSFXEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSFXEditorApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CSFXEditorApp theApp;



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SFXEDITOR_H__98C4A328_CDAA_47DA_8BB2_A77C30BA910A__INCLUDED_)
