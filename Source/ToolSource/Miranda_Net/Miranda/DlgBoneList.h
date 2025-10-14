#if !defined(AFX_DLGBONELIST_H__31CDADD7_3768_48F1_8FF7_2087E00DFB0D__INCLUDED_)
#define AFX_DLGBONELIST_H__31CDADD7_3768_48F1_8FF7_2087E00DFB0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBoneList.h : header file
//

#include "DlgMTE.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBoneList dialog

class CDlgBoneList : public CDialogBar
{
// Construction
public:
	CDlgBoneList( ); //CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBoneList)
	enum { IDD = IDD_DIALOG_BONELIST };
	CButton	m_btnBoneSave;
	CButton	m_btnLinkEffect;
	CButton m_btnMTE;
	CListCtrl	m_ctrlBoneList;
	//}}AFX_DATA

	CString m_strLoadedSfx;

	CDlgMTE m_kMTE;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBoneList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void Make_BoneList( CModelObject* pModel );
	void SelectBone( const char* szBoneName );
	void ResetAllWnd( );

	void SelectBone_Multi( const char* szBoneName );

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBoneList)
	afx_msg void OnItemchangedListBone(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonLinkeffect();
	afx_msg void OnButtonBonesave();
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT, LONG);
	afx_msg void OnUpdateButton1( CCmdUI* pCmdUI );
	afx_msg void OnUpdateButton2( CCmdUI* pCmdUI );
	afx_msg void OnUpdateButton3( CCmdUI* pCmdUI );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMteff();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBONELIST_H__31CDADD7_3768_48F1_8FF7_2087E00DFB0D__INCLUDED_)
