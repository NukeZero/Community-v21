#if !defined(AFX_DLGPATROL1_H__E00B64BE_A5F3_4BBE_9827_5D8B2ED0BE28__INCLUDED_)
#define AFX_DLGPATROL1_H__E00B64BE_A5F3_4BBE_9827_5D8B2ED0BE28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPatrol1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPatrol dialog

class CDlgPatrol : public CDialog
{
// Construction
public:
	CDlgPatrol(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPatrol)
	enum { IDD = IDD_PATROL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPatrol)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPatrol)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPATROL1_H__E00B64BE_A5F3_4BBE_9827_5D8B2ED0BE28__INCLUDED_)
