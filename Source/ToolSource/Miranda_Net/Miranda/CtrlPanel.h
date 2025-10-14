#if !defined(AFX_CTRLPANEL_H__263FFDDB_8C4B_4530_862A_70D848B659DC__INCLUDED_)
#define AFX_CTRLPANEL_H__263FFDDB_8C4B_4530_862A_70D848B659DC__INCLUDED_

#include "MirandaDoc.h"
#include "SetSfxDlg.h"
//#include "MirandaView.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtrlPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCtrlPanel dialog

class CCtrlPanel : public CDialogBar
{
// Construction
public:
	CCtrlPanel(/*CWnd* pParent = NULL*/);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCtrlPanel)
	enum { IDD = IDD_CTRL_PANEL };
	CSliderCtrl	m_sLighting;
	CComboBox	m_cbSize;
	CTreeCtrl	m_ObjTree;
	CString	m_cbvSize;
	//}}AFX_DATA
	
	CMirandaDoc*		m_pDoc;
	CMirandaView*		m_pView;

	CImageList			m_ImgTree;
	TV_INSERTSTRUCT		m_Instruct;
	HTREEITEM			m_ObjRoot, m_SfxRoot;
	
	CMenu				m_PopupMenu;

	CSetSfxDlg			m_SetDlg;

	// 내부 변수들 
	int			m_nObjValue;
	int			m_nSfxValue;
	int			m_nSelectedID;
	// 함수들 
	CMirandaDoc*  GetDoc(); 
	CMirandaView* GetView();
	bool	AddObj(int nCount = 1);
	bool	AddSfx(int nCount = 1);
	void	DelAll();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCtrlPanel)
	afx_msg void OnRclickObjTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMiAddobj();
	afx_msg void OnMiAddsfx();
	afx_msg void OnMiDelobj();
	afx_msg void OnMiLoadanim();
	afx_msg void OnMiLoadbone();
	afx_msg void OnMiLoadmodel();
	afx_msg void OnMiSetlod();
	afx_msg void OnMiDelsfx();
	afx_msg void OnMiLoadsfx();
	afx_msg void OnMiSet();
	afx_msg void OnCamMode();
	afx_msg void OnTexture();
	afx_msg void OnChangePlay();
	afx_msg void OnSelchangeMapSize();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClickObjTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBegindragObjTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT, LONG);
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLPANEL_H__263FFDDB_8C4B_4530_862A_70D848B659DC__INCLUDED_)
