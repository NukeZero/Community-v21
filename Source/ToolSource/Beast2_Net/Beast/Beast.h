// Beast.h : main header file for the BEAST application
//

#if !defined(AFX_BEAST_H__C6EFE72D_98BE_45B9_830C_28E7A7FA3458__INCLUDED_)
#define AFX_BEAST_H__C6EFE72D_98BE_45B9_830C_28E7A7FA3458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define WM_CLOSE_OBJ_PROP_SHEET    (WM_USER + 15)

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CBeastApp:
// See Beast.cpp for the implementation of this class
//

class CBeastApp : public CWinApp
{
public:
	CBeastApp();
	virtual ~CBeastApp( );
	BOOL ProcessShellCommand(CCommandLineInfo& rCmdInfo);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeastApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CBeastApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpenProject(); 
	afx_msg void OnFileNewProject();
	//afx_msg void OnFileSaveProject();
	afx_msg void OnFileCloseProject();
	afx_msg void OnUpdateFileCloseProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNewProject(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateFileSaveProject(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenWorld();
	afx_msg void OnUpdateFileOpenWorld(CCmdUI* pCmdUI);
	afx_msg void OnFileNewWorld();
	afx_msg void OnUpdateFileNewWorld(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEAST_H__C6EFE72D_98BE_45B9_830C_28E7A7FA3458__INCLUDED_)
