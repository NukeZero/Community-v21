#if !defined(AFX_DLGEDITUNIT_H__CE62158C_DFAD_4E84_91FF_40159929F066__INCLUDED_)
#define AFX_DLGEDITUNIT_H__CE62158C_DFAD_4E84_91FF_40159929F066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditUnit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEditUnit dialog

class CDlgEditUnit : public CDialog
{
// Construction
public:
	CDlgEditUnit( CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditUnit)
	enum { IDD = IDD_DIALOG_EDITUNIT };
	CComboBox	m_cmbCase;
	CString	m_stcBoneName;
	CString	m_stcSfxName;
	int		m_iSelectedCase;
	BOOL	m_bLoop;
	//}}AFX_DATA
	
	//Domodal()로 불러 쓸때는 Domodal이 끝난후 핸들이 NULL이 되서 해당 콘트롤을 쓸수가 없다
	//해서 선택했을때 그 정보를 담아놓는용 
	CString m_strSelectedComboList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetSelectedComboText( CString& strOut );

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditUnit)
	afx_msg void OnSelchangeComboCase();
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT, LONG);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITUNIT_H__CE62158C_DFAD_4E84_91FF_40159929F066__INCLUDED_)
