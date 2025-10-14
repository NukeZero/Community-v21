#if !defined(AFX_DLGNAVIGATOR_H__42B3C963_A9E0_4B5E_B7D2_B95356C4AA43__INCLUDED_)
#define AFX_DLGNAVIGATOR_H__42B3C963_A9E0_4B5E_B7D2_B95356C4AA43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNavigator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNavigator dialog

class CDlgNavigator : public CDialog
{
// Construction
public:
	D3DXVECTOR3 m_vPos;
	CWorld* m_pWorld;
	CWorldView* m_pWorldView;
	CDlgNavigator(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNavigator)
	enum { IDD = IDD_NAVIGATOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void UpdatePos();
	void Update( CWorld* pWorld, CWorldView* pWorldView );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNavigator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNavigator)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNAVIGATOR_H__42B3C963_A9E0_4B5E_B7D2_B95356C4AA43__INCLUDED_)
