#if !defined(AFX_PAGEPROPPATH_H__5831435A_C657_4841_9B66_61B46079A1EB__INCLUDED_)
#define AFX_PAGEPROPPATH_H__5831435A_C657_4841_9B66_61B46079A1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PagePropPath.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPagePropPath dialog

class CPagePropPath : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropPath)
	CObj* m_pObj;
// Construction
public:
	void SetItemData( DWORD dwItemData );
	void Init( CObj* pObj );
	CPagePropPath();
	~CPagePropPath();

// Dialog Data
	//{{AFX_DATA(CPagePropPath)
	enum { IDD = IDD_PROP_PATH };
	CComboBox	m_Motion;
	CString	m_Delay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropPath)
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROPPATH_H__5831435A_C657_4841_9B66_61B46079A1EB__INCLUDED_)
