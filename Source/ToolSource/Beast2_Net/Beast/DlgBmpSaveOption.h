#if !defined(AFX_DLGBMPSAVEOPTION_H__5B0B72F8_F0C3_45AF_BD67_866D14A8E6A4__INCLUDED_)
#define AFX_DLGBMPSAVEOPTION_H__5B0B72F8_F0C3_45AF_BD67_866D14A8E6A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBmpSaveOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBmpSaveOption dialog

class CDlgBmpSaveOption : public CDialog
{
// Construction
public:
	CDlgBmpSaveOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBmpSaveOption)
	enum { IDD = IDD_BMP_SAVE_OPTION };
	BOOL	m_bViewRegion;
	BOOL	m_bViewName;
	BOOL	m_bViewSpawn;
	BOOL	m_bHeightAttrb;
	BOOL	m_bWater;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBmpSaveOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBmpSaveOption)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBMPSAVEOPTION_H__5B0B72F8_F0C3_45AF_BD67_866D14A8E6A4__INCLUDED_)
