// BetaPatchClient.h : main header file for the BETAPATCHCLIENT application
//

#if !defined(AFX_BETAPATCHCLIENT_H__CB6FDBED_0695_4D26_8042_AFE31500A8C5__INCLUDED_)
#define AFX_BETAPATCHCLIENT_H__CB6FDBED_0695_4D26_8042_AFE31500A8C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

const TCHAR KEY[32] = "DeleteResourceFile";

/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientApp:
// See BetaPatchClient.cpp for the implementation of this class
//

enum CHECK_TYPE
{
	CHECK_TRUE,
	CHECK_FALSE,
	CHECK_SKIP,
};

class CBetaPatchClientApp : public CWinApp
{

public:
	CBetaPatchClientApp();

	void		RunClient();

	void		DeleteResourceFiles( const TCHAR * pszFileName );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBetaPatchClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void		InitLanguage();
	CHECK_TYPE	CheckSingleInstance();
	BOOL		InitPath();
	BOOL		CheckDirectXVersion();

	//{{AFX_MSG(CBetaPatchClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BETAPATCHCLIENT_H__CB6FDBED_0695_4D26_8042_AFE31500A8C5__INCLUDED_)
