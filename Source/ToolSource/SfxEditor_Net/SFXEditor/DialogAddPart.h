#if !defined(AFX_DIALOGADDPART_H__38AF962D_BB0F_4704_B0D4_E6A0C5EF42AC__INCLUDED_)
#define AFX_DIALOGADDPART_H__38AF962D_BB0F_4704_B0D4_E6A0C5EF42AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogAddPart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogAddPart dialog

class CDialogAddPart : public CDialog
{
// Construction
public:
	CDialogAddPart(CWnd* pParent = NULL);   // standard constructor

	SFXPARTTYPE m_nType;
	void UpdateRadio(void);
// Dialog Data
	//{{AFX_DATA(CDialogAddPart)
	enum { IDD = IDD_ADDPART };
	CString	m_StrName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAddPart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAddPart)
	afx_msg void OnRadioDapBill();
	afx_msg void OnRadioDapCustommesh();
	afx_msg void OnRadioDapMesh();
	afx_msg void OnRadioDapParticle();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGADDPART_H__38AF962D_BB0F_4704_B0D4_E6A0C5EF42AC__INCLUDED_)
