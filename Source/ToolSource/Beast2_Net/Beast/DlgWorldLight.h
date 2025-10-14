#if !defined(AFX_DLGWORLDLIGHT_H__5795716C_072B_4096_A0D9_A01BBF0CCE0C__INCLUDED_)
#define AFX_DLGWORLDLIGHT_H__5795716C_072B_4096_A0D9_A01BBF0CCE0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWorldLight.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgWorldLight dialog

class CWorldView;
class CWorld;

class CDlgWorldLight : public CDialog
{
// Construction
public:
	CWorld* m_pWorld;
	CWorldView* m_pWorldView;
	
	CDlgWorldLight(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWorldLight)
	enum { IDD = IDD_WORLD_LIGHT };
	float	m_fAmbientB;
	float	m_fAmbientG;
	float	m_fAmbientR;
	float	m_fDiffuseB;
	float	m_fDiffuseG;
	float	m_fDiffuseR;
	int		m_nLightType;
	int		m_nHour;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWorldLight)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWorldLight)
	afx_msg void OnApply();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWORLDLIGHT_H__5795716C_072B_4096_A0D9_A01BBF0CCE0C__INCLUDED_)
