#if !defined(AFX_DLGRESPAWNDEFAULT_H__E2B4B4EB_F42A_47B9_B2DA_412CED5DBDCA__INCLUDED_)
#define AFX_DLGRESPAWNDEFAULT_H__E2B4B4EB_F42A_47B9_B2DA_412CED5DBDCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRespawnDefault.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRespawnDefault dialog

class CDlgRespawnDefault : public CDialog
{
// Construction
public:
	CDlgRespawnDefault(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRespawnDefault)
	enum { IDD = IDD_RESPAWN_DEFAULT };
	int		m_nNum;
	int		m_nSec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRespawnDefault)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRespawnDefault)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESPAWNDEFAULT_H__E2B4B4EB_F42A_47B9_B2DA_412CED5DBDCA__INCLUDED_)
