#if !defined(AFX_DLGWORLDPROP_H__CA000A20_545D_4D17_B800_5BA62133469A__INCLUDED_)
#define AFX_DLGWORLDPROP_H__CA000A20_545D_4D17_B800_5BA62133469A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWorldProp.h : header file
//

class CWorld;
class CWorldView;

/////////////////////////////////////////////////////////////////////////////
// CDlgWorldProp dialog

class CDlgWorldProp : public CDialog
{
// Construction
public:
	CDlgWorldProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWorldProp)
	enum { IDD = IDD_WORLD_PROPERIES };
	BOOL	m_bIndoor;
	float	m_fFogStart;
	float	m_fFogEnd;
	float	m_fFogDensity;
	BOOL	m_bFly;
	DWORD	m_dwDiffuseBlue;
	DWORD	m_dwDiffuseGreen;
	DWORD	m_dwDiffuseRed;
	DWORD	m_dwAmbientBlue;
	DWORD	m_dwAmbientRed;
	DWORD	m_dwAmbientGreen;
	float	m_fLightDirX;
	float	m_fLightDirY;
	float	m_fLightDirZ;
	CString	m_strSky1;
	CString	m_strSky2;
	CString	m_strSky3;
	CString	m_strSun;
	CString	m_strMoon;
	CString	m_strCloud1;
	CString	m_strCloud2;
	CString	m_strCloud3;
	BOOL	m_bUse24;
	//}}AFX_DATA
	CWorldView* m_pWorldView;
	CWorld* m_pWorld;
	int		m_nPKMode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWorldProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWorldProp)
	afx_msg void OnApply();
	afx_msg void OnPeace();
	afx_msg void OnPenaltyPK();
	afx_msg void OnFreePK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnUse24();
	afx_msg void OnBtnEdit24();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strEditSeaCloud;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWORLDPROP_H__CA000A20_545D_4D17_B800_5BA62133469A__INCLUDED_)
