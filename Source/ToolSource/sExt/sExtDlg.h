// sExtDlg.h : header file
//

#if !defined(AFX_SEXTDLG_H__65689418_5771_479F_98C2_B984C7B47AC7__INCLUDED_)
#define AFX_SEXTDLG_H__65689418_5771_479F_98C2_B984C7B47AC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSExtDlg dialog

class CSExtDlg : public CDialog
{
// Construction
public:
	CSExtDlg(CWnd* pParent = NULL);	// standard constructor

//	op
	BOOL	FromtextClient( LPCTSTR sFile );
	BOOL	FrompropItemEtc( LPCTSTR sFile );
	BOOL	FrompropQuest( LPCTSTR sFile );
	BOOL	FrompropItem( LPCTSTR sFile, int nType=0 );

// Dialog Data
	//{{AFX_DATA(CSExtDlg)
	enum { IDD = IDD_SEXT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSExtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSExtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEXTDLG_H__65689418_5771_479F_98C2_B984C7B47AC7__INCLUDED_)
