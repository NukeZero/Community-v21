#if !defined(AFX_DLGGUIDESSETTING_H__08D7CD6D_0FF7_40BC_9B5F_8D18871CE08A__INCLUDED_)
#define AFX_DLGGUIDESSETTING_H__08D7CD6D_0FF7_40BC_9B5F_8D18871CE08A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGuidesSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgGuidesSetting dialog

class CDlgGuidesSetting : public CDialog
{
// Construction
public:
	CDlgGuidesSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgGuidesSetting)
	enum { IDD = IDD_GUIDE_SETTING };
	int		m_nWidth;
	int		m_nHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGuidesSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgGuidesSetting)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGUIDESSETTING_H__08D7CD6D_0FF7_40BC_9B5F_8D18871CE08A__INCLUDED_)
