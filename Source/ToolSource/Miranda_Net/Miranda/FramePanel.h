#if !defined(AFX_FRAMEPANEL_H__2097D354_7804_4261_9249_16161BE655C8__INCLUDED_)
#define AFX_FRAMEPANEL_H__2097D354_7804_4261_9249_16161BE655C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FramePanel.h : header file
//
#include "MirandaDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CFramePanel dialog

class CFramePanel : public CDialogBar
{
// Construction
public:
	CFramePanel();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFramePanel)
	enum { IDD = IDD_FRAME_PANEL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFramePanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CMirandaDoc*		m_pDoc;
	CMirandaView*		m_pView;

	CBitmap				m_hBitmap;
	int					m_nSelectedCellNum;

	int			  ChangeSelection(int nIdx);
	int			  FindSelection(CPoint point);
	CMirandaDoc*  GetDoc(); 
	CMirandaView* GetView();
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFramePanel)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	afx_msg LONG OnInitDialog(UINT, LONG);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEPANEL_H__2097D354_7804_4261_9249_16161BE655C8__INCLUDED_)
