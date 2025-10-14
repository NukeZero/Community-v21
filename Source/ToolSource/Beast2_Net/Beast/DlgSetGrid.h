#if !defined(AFX_DLGSETGRID_H__9697E618_7E97_405D_8D87_FB10BD7A6945__INCLUDED_)
#define AFX_DLGSETGRID_H__9697E618_7E97_405D_8D87_FB10BD7A6945__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetGrid.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetGrid dialog

class CDlgSetGrid : public CDialog
{
// Construction
public:
	CDlgSetGrid(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetGrid)
	enum { IDD = IDD_SET_GRID };
	float	m_fGridX;
	float	m_fGridY;
	float	m_fGridZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetGrid)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetGrid)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETGRID_H__9697E618_7E97_405D_8D87_FB10BD7A6945__INCLUDED_)
