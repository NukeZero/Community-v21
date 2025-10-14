#if !defined(AFX_PROPERTY_H__BC6E5EBA_D291_4BAB_8102_BA42A1279BC4__INCLUDED_)
#define AFX_PROPERTY_H__BC6E5EBA_D291_4BAB_8102_BA42A1279BC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Property.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPagePropControl dialog

class CPagePropControl : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropControl)

// Construction
public:
	CPagePropControl();
	~CPagePropControl();

// Dialog Data
	//{{AFX_DATA(CPagePropControl)
	enum { IDD = IDD_PROP_CTRL };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropControl)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTY_H__BC6E5EBA_D291_4BAB_8102_BA42A1279BC4__INCLUDED_)
