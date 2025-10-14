#if !defined(AFX_SETSFXDLG_H__FFD0B378_93CF_4FCB_A007_923D94603B1B__INCLUDED_)
#define AFX_SETSFXDLG_H__FFD0B378_93CF_4FCB_A007_923D94603B1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetSfxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetSfxDlg dialog

class CSetSfxDlg : public CDialog
{
// Construction
public:
	CSetSfxDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetSfxDlg)
	enum { IDD = IDD_SET_SFX };
	CComboBox	m_CtrlObj;
	CComboBox	m_CtrlKey;
	CString	m_cbvKey;
	CString	m_cbvObj;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetSfxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	public:
		// 변수 
		bool	m_bIsLoop;
		int		m_nHotKey;
		int		m_nObjID;

		// 함수 
		void SetValue(bool bIsLoop, int nHotKey, int nObjID);
		bool GetIsLoop();
		int	GetHotKey();
		int GetObjNum();


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetSfxDlg)
	afx_msg void OnLoopf();
	afx_msg void OnLoopt();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFobj();
	afx_msg void OnSelchangeHotkey();
	afx_msg void OnSelendokHotkey();
	afx_msg void OnEditupdateHotkey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETSFXDLG_H__FFD0B378_93CF_4FCB_A007_923D94603B1B__INCLUDED_)
