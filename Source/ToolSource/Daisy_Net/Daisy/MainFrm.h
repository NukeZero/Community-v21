
#if !defined(AFX_MAINFRM_H__027BF7BA_3CD5_436E_9F0E_69C3078B0D6B__INCLUDED_)
#define AFX_MAINFRM_H__027BF7BA_3CD5_436E_9F0E_69C3078B0D6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SizingTabCtrlBar.h"
#include "DialogToolBar.h"

class CDaisyView;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)


public:

						CMainFrame();
	virtual				~CMainFrame();

	void				PushWndId( DWORD dwWndId );
	DWORD				PopWndId( void );

	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
	virtual void		RecalcLayout(BOOL bNotify = TRUE);

#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif


protected: 


	void 				SaveWndID_H( LPCTSTR lpszFileName );
	void 				SaveResource_Inc( LPCTSTR lpszFileName );
	void 				OnUpdateRect(CCmdUI* pCmdUI);
	void				InsertAppletToTreeView( void );



public:


	CDaisyView*			m_pDaisyView;

	CString				m_strSourcePath;
	DWORD				m_dwToolSelect;

	int					m_nIdApplet;
	int					m_nIdWndCtrl;

	CDialogToolBar		m_DialogToolBar;
	CSizingTabCtrlBar	m_wndSTCBar;


protected:


	CToolBar    		m_wndToolBar;
	CToolBar			m_wndLayoutBar;
	CDWordStack			m_wndIdStack;
	CStatusBar  		m_wndStatusBar;


protected:


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWndButton();
	afx_msg void OnUpdateWndButton(CCmdUI* pCmdUI);
	afx_msg void OnWndEdit();
	afx_msg void OnUpdateWndEdit(CCmdUI* pCmdUI);
	afx_msg void OnWndCombobox();
	afx_msg void OnUpdateWndCombobox(CCmdUI* pCmdUI);
	afx_msg void OnWndCheck();
	afx_msg void OnUpdateWndCheck(CCmdUI* pCmdUI);
	afx_msg void OnWndRadio();
	afx_msg void OnUpdateWndRadio(CCmdUI* pCmdUI);
	afx_msg void OnWndListbox();
	afx_msg void OnUpdateWndListbox(CCmdUI* pCmdUI);
	afx_msg void OnWndListctrl();
	afx_msg void OnUpdateWndListctrl(CCmdUI* pCmdUI);
	afx_msg void OnWndTabctrl();
	afx_msg void OnUpdateWndTabctrl(CCmdUI* pCmdUI);
	afx_msg void OnWndStatic();
	afx_msg void OnUpdateWndStatic(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveFrame();
	afx_msg void OnWndTreectrl();
	afx_msg void OnUpdateWndTreectrl(CCmdUI* pCmdUI);
	afx_msg void OnWndCustom();
	afx_msg void OnUpdateWndCustom(CCmdUI* pCmdUI);
	afx_msg void OnWndText();
	afx_msg void OnUpdateWndText(CCmdUI* pCmdUI);
	afx_msg void OnWndGroupbox();
	afx_msg void OnUpdateWndGroupbox(CCmdUI* pCmdUI);
	afx_msg void OnWndPicture();
	afx_msg void OnUpdateWndPicture(CCmdUI* pCmdUI);
	afx_msg void OnSettingLanguage();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);


#ifdef __DAISY_UPDATE_1
	afx_msg void OnAlignLeft();
	afx_msg void OnAlignRight();
	afx_msg void OnAlignTop();
	afx_msg void OnAlignBottom();
	afx_msg void OnMakeSameWidht();
	afx_msg void OnMakeSameHeight();
	afx_msg void OnMakeSameSize();

#endif 

	DECLARE_MESSAGE_MAP()
};

#endif 