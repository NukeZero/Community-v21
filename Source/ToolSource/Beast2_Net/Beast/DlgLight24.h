#if !defined(AFX_DLGLIGHT24_H__C93ADA16_92BC_4CB2_AD40_869774B94FAD__INCLUDED_)
#define AFX_DLGLIGHT24_H__C93ADA16_92BC_4CB2_AD40_869774B94FAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLight24.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLight24 dialog

class CDlgLight24 : public CDialog
{
// Construction
public:
	CDlgLight24(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLight24)
	enum { IDD = IDD_DIALOG_LIGHT24 };
	float   m_fAR0;
	float   m_fAR1;
	float   m_fAR2;
	float   m_fAR3;
	float   m_fAR4;
	float   m_fAR5;
	float   m_fAR6;
	float   m_fAR7;
	float   m_fAR8;
	float   m_fAR9;
	float   m_fAR10;
	float   m_fAR11;
	float   m_fAR12;
	float   m_fAR13;
	float   m_fAR14;
	float	m_fAR15;
	float   m_fAR16;
	float   m_fAR17;
	float   m_fAR18;
	float	m_fAR19;
	float   m_fAR20;
	float   m_fAR21;
	float   m_fAR22;
	float   m_fAR23;

	float   m_fAG0;
	float   m_fAG1;
	float   m_fAG2;
	float   m_fAG3;
	float   m_fAG4;
	float   m_fAG5;
	float   m_fAG6;
	float   m_fAG7;
	float   m_fAG8;
	float   m_fAG9;
	float   m_fAG10;
	float   m_fAG11;
	float   m_fAG12;
	float   m_fAG13;
	float   m_fAG14;
	float	m_fAG15;
	float   m_fAG16;
	float   m_fAG17;
	float   m_fAG18;
	float	m_fAG19;
	float   m_fAG20;
	float   m_fAG21;
	float   m_fAG22;
	float   m_fAG23;

	float   m_fAB0;
	float   m_fAB1;
	float   m_fAB2;
	float   m_fAB3;
	float   m_fAB4;
	float   m_fAB5;
	float   m_fAB6;
	float   m_fAB7;
	float   m_fAB8;
	float   m_fAB9;
	float   m_fAB10;
	float   m_fAB11;
	float   m_fAB12;
	float   m_fAB13;
	float   m_fAB14;
	float	m_fAB15;
	float   m_fAB16;
	float   m_fAB17;
	float   m_fAB18;
	float	m_fAB19;
	float   m_fAB20;
	float   m_fAB21;
	float   m_fAB22;
	float   m_fAB23;

	float   m_fDR0;
	float   m_fDR1;
	float   m_fDR2;
	float   m_fDR3;
	float   m_fDR4;
	float   m_fDR5;
	float   m_fDR6;
	float   m_fDR7;
	float   m_fDR8;
	float   m_fDR9;
	float   m_fDR10;
	float   m_fDR11;
	float   m_fDR12;
	float   m_fDR13;
	float   m_fDR14;
	float	m_fDR15;
	float   m_fDR16;
	float   m_fDR17;
	float   m_fDR18;
	float	m_fDR19;
	float   m_fDR20;
	float   m_fDR21;
	float   m_fDR22;
	float   m_fDR23;

	float   m_fDG0;
	float   m_fDG1;
	float   m_fDG2;
	float   m_fDG3;
	float   m_fDG4;
	float   m_fDG5;
	float   m_fDG6;
	float   m_fDG7;
	float   m_fDG8;
	float   m_fDG9;
	float   m_fDG10;
	float   m_fDG11;
	float   m_fDG12;
	float   m_fDG13;
	float   m_fDG14;
	float	m_fDG15;
	float   m_fDG16;
	float   m_fDG17;
	float   m_fDG18;
	float	m_fDG19;
	float   m_fDG20;
	float   m_fDG21;
	float   m_fDG22;
	float   m_fDG23;

	float   m_fDB0;
	float   m_fDB1;
	float   m_fDB2;
	float   m_fDB3;
	float   m_fDB4;
	float   m_fDB5;
	float   m_fDB6;
	float   m_fDB7;
	float   m_fDB8;
	float   m_fDB9;
	float   m_fDB10;
	float   m_fDB11;
	float   m_fDB12;
	float   m_fDB13;
	float   m_fDB14;
	float	m_fDB15;
	float   m_fDB16;
	float   m_fDB17;
	float   m_fDB18;
	float	m_fDB19;
	float   m_fDB20;
	float   m_fDB21;
	float   m_fDB22;
	float   m_fDB23;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLight24)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLight24)
	afx_msg void On24Apply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL Init( CWorld* pWorld,  const char* filename );
	BOOL SaveFile( const char* szFileName );

protected:
	BOOL ReadFile( const char* filename );
	BOOL Setup24Light( const char* filename );

protected:
	CWorld* _pWorld;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLIGHT24_H__C93ADA16_92BC_4CB2_AD40_869774B94FAD__INCLUDED_)
