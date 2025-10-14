// ProfileView.h : main header file for the PROFILEVIEW application
//

#if !defined(AFX_PROFILEVIEW_H__305C8ED0_5804_4B79_82FD_2811D6BF224E__INCLUDED_)
#define AFX_PROFILEVIEW_H__305C8ED0_5804_4B79_82FD_2811D6BF224E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProfileViewApp:
// See ProfileView.cpp for the implementation of this class
//

class CProfileViewApp : public CWinApp
{
public:
	CProfileViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProfileViewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFILEVIEW_H__305C8ED0_5804_4B79_82FD_2811D6BF224E__INCLUDED_)
