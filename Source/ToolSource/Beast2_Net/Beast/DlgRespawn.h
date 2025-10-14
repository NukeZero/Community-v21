#if !defined(AFX_DLGRESPAWN_H__7C2B3C74_C9B4_4A64_B906_C0E8A9850C5E__INCLUDED_)
#define AFX_DLGRESPAWN_H__7C2B3C74_C9B4_4A64_B906_C0E8A9850C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRespawn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRespawn dialog

class CDlgRespawn : public CDialog
{
// Construction
public:
	NM_LISTVIEW m_selectItem;
	CWorldView* m_pWorldView;
	CWorld* m_pWorld;
	CDlgRespawn(CWorld* pWorld,CWnd* pParent = NULL);   // standard constructor
	void Update(CWorldView* pWorldView,CWorld* pWorld);

// Dialog Data
	//{{AFX_DATA(CDlgRespawn)
	enum { IDD = IDD_RESPAWN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRespawn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRespawn)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListRespawn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnClickListRespawn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangingListRespawn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMove();
	afx_msg void OnSave();
	afx_msg void OnClose();
	afx_msg void OnColumnclickListRespawn(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESPAWN_H__7C2B3C74_C9B4_4A64_B906_C0E8A9850C5E__INCLUDED_)
