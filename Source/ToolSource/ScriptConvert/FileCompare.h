#if !defined(AFX_FILECOMPARE_H__552E7F1A_25D5_454F_AB5F_C1FC8804357D__INCLUDED_)
#define AFX_FILECOMPARE_H__552E7F1A_25D5_454F_AB5F_C1FC8804357D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileCompare.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileCompare dialog

class CFileCompare : public CDialog
{
// Construction
public:
	CFileCompare(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileCompare)
	enum { IDD = IDD_SCRIPTCONVERT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileCompare)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileCompare)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILECOMPARE_H__552E7F1A_25D5_454F_AB5F_C1FC8804357D__INCLUDED_)
