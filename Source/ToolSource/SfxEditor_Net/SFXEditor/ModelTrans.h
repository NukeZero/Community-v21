#if !defined(AFX_MODELTRANS_H__1BC19264_0EF4_4D27_949A_DFF25DA4337C__INCLUDED_)
#define AFX_MODELTRANS_H__1BC19264_0EF4_4D27_949A_DFF25DA4337C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelTrans.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelTrans dialog

class CModelTrans : public CDialog
{
// Construction
public:
	CModelTrans(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelTrans)
	enum { IDD = IDD_MODELDIALOGBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelTrans)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelTrans)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELTRANS_H__1BC19264_0EF4_4D27_949A_DFF25DA4337C__INCLUDED_)
