#if !defined(AFX_DLGADDDELOBJECT_H__FEA79FB4_4A27_4C17_80BC_933B5BFBE20F__INCLUDED_)
#define AFX_DLGADDDELOBJECT_H__FEA79FB4_4A27_4C17_80BC_933B5BFBE20F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddDelObject.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddObject dialog

class CDlgAddObject : public CDialog
{
	CStrArray* m_pStrArray;
// Construction
public:
	CDlgAddObject( CStrArray* pStrArray, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddObject)
	enum { IDD = IDD_ADD_OBJECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddObject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddObject)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgDelObject dialog

class CDlgDelObject : public CDialog
{
	CStrArray* m_pStrArray;
// Construction
public:
	CDlgDelObject( CStrArray* pStrArray, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDelObject)
	enum { IDD = IDD_DELETE_OBJECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDelObject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDelObject)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDDELOBJECT_H__FEA79FB4_4A27_4C17_80BC_933B5BFBE20F__INCLUDED_)
