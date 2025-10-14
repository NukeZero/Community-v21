#if !defined(AFX_DIALOGTOOLBAR_H__73577923_1DAC_4E3F_943B_191E228B82D2__INCLUDED_)
#define AFX_DIALOGTOOLBAR_H__73577923_1DAC_4E3F_943B_191E228B82D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogToolBar window

class CDialogToolBar : public CToolBar
{
// Construction
public:
	CDialogToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDialogToolBar();
	BOOL Create(CWnd* pParentWnd);
	// Generated message map functions
protected:
	//{{AFX_MSG(CDialogToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTOOLBAR_H__73577923_1DAC_4E3F_943B_191E228B82D2__INCLUDED_)
