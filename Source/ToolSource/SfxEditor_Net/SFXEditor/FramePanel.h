#if !defined(AFX_FRAMEPANEL_H__C3A7D22C_0482_44B3_BEE2_D6A0570822E2__INCLUDED_)
#define AFX_FRAMEPANEL_H__C3A7D22C_0482_44B3_BEE2_D6A0570822E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FramePanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFramePanel dialog

class CFramePanel : public CDialogBar
{
// Construction
public:
	CFramePanel(CWnd* pParent = NULL);   // standard constructor

	CToolTipCtrl   m_cToolTip; 
	CSFXEditorDoc* m_pDoc;
	CSFXEditorView* m_pView;

	CBitmap	m_hBitmap;

	BOOL m_bLBut;
	BOOL m_bRBut;
	BOOL m_bMBut;
	int m_nGapLevel;
	
#ifdef __AALLSELECT
	BOOL m_bLayerDragDrop;
	int  m_nSfxPartClipBoard;
	int  m_nSfxPartLayer;

	void ReplaceSfx( int nSrcIndex, int nDestIndex );
#endif // __AALLSELECT

	WORD m_nLeftFrame;
	WORD m_nTopObj;

	POINT m_ptClicked;
	POINT m_ptClickedScrollState;
	POINT m_ptCurrent;

	POINT m_ptLClicked;

	int m_nCurFrame,m_nCurPart;
	int m_nLCFrame,m_nLCPart;

	POINT m_ptSelectStart,m_ptSelectEnd;

	BOOL m_bKeyLocked;
	BOOL m_bSelecting;
	SfxKeyFrame* m_pClickedKey;
	CPtrArray m_SelectedKeys;
	CSfxPart* m_pSfxPart;

	void UpdatePanel(void);

// Dialog Data
	//{{AFX_DATA(CFramePanel)
	enum { IDD = IDD_FRAMEPANEL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFramePanel)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFramePanel)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPopupmenu();
	afx_msg void OnUpdatePopupmenu(CCmdUI* pCmdUI);
	afx_msg void OnValutchange();
	afx_msg void OnNamechange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEPANEL_H__C3A7D22C_0482_44B3_BEE2_D6A0570822E2__INCLUDED_)
