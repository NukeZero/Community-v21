#if !defined(AFX_DLGRANDOMDEGREESCALE_H__3994A583_A3FE_4CA0_885F_B6DB05920635__INCLUDED_)
#define AFX_DLGRANDOMDEGREESCALE_H__3994A583_A3FE_4CA0_885F_B6DB05920635__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRandomDegreeScale.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRandomDegreeScale dialog

class CDlgRandomDegreeScale : public CDialog
{
// Construction
public:
	CDlgRandomDegreeScale(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRandomDegreeScale)
	enum { IDD = IDD_RANDOM_DEGREE_SCALE };
	int		m_nDegree1;
	int		m_nDegree2;
	float	m_fScale2;
	float	m_fScale1;
	BOOL	m_bDegree;
	BOOL	m_bScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRandomDegreeScale)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRandomDegreeScale)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRANDOMDEGREESCALE_H__3994A583_A3FE_4CA0_885F_B6DB05920635__INCLUDED_)
