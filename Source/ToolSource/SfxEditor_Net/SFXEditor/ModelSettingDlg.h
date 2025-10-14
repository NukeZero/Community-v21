#if !defined(AFX_MODELSETTINGDLG_H__82FD7C71_34A0_47E7_937F_CD1ED6EB3068__INCLUDED_)
#define AFX_MODELSETTINGDLG_H__82FD7C71_34A0_47E7_937F_CD1ED6EB3068__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelSettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelSettingDlg dialog

class CModelSettingDlg : public CDialog
{
// Construction
public:
	CString         m_strBoneFile;
	CString         m_strModelFile;
	CString         m_strAniFile;

	CSFXEditorView* m_pView;

	CModelSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelSettingDlg)
	enum { IDD = IDD_MODEL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelSettingDlg)
	afx_msg void OnBoneLoad();
	afx_msg void OnModelLoad();
	afx_msg void OnMotionLoad();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELSETTINGDLG_H__82FD7C71_34A0_47E7_937F_CD1ED6EB3068__INCLUDED_)
