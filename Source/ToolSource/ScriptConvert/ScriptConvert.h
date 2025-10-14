// ScriptConvert.h : main header file for the SCRIPTCONVERT application
//

#if !defined(AFX_SCRIPTCONVERT_H__FF6B5447_4607_40A7_A7C2_A53CEBBA4AA1__INCLUDED_)
#define AFX_SCRIPTCONVERT_H__FF6B5447_4607_40A7_A7C2_A53CEBBA4AA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScriptConvertApp:
// See ScriptConvert.cpp for the implementation of this class
//

class CScriptConvertApp : public CWinApp
{
public:
	CScriptConvertApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptConvertApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CScriptConvertApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTCONVERT_H__FF6B5447_4607_40A7_A7C2_A53CEBBA4AA1__INCLUDED_)
