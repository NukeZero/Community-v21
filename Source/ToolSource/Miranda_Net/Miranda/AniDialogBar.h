#if !defined(AFX_ANIDIALOGBAR_H__329B4946_E3C9_4EC3_AF3E_A3420679A26E__INCLUDED_)
#define AFX_ANIDIALOGBAR_H__329B4946_E3C9_4EC3_AF3E_A3420679A26E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AniDialogBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAniDialogBar dialog

class CAniDialogBar : public CDialogBar
{
// Construction
public:
	CAniDialogBar(/*CWnd* pParent = NULL*/);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAniDialogBar)
	enum { IDD = IDD_DIALOG_ANIMATION };
	CListCtrl	m_ctrlAniList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAniDialogBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	int ListUpAllAnimation( const TCHAR* szFileName );
	void ResetAllWnd( );

protected:

	// Generated message map functions
	//{{AFX_MSG(CAniDialogBar)
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT, LONG);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIDIALOGBAR_H__329B4946_E3C9_4EC3_AF3E_A3420679A26E__INCLUDED_)
