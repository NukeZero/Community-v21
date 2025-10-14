// BetaPatchClientDlg.h : header file
//

#if !defined(AFX_BETAPATCHCLIENTDLG_H__179321FA_0D9A_43C5_A089_598C606DDDC5__INCLUDED_)
#define AFX_BETAPATCHCLIENTDLG_H__179321FA_0D9A_43C5_A089_598C606DDDC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientDlg dialog

#include "Label.h"
#include "resource.h"
#include "BtnST.h"
#include "HyperLink.h"
#include "Picture.h"

enum 
{
	STATE_COMPLETED,			// 패치작업 완료 
	STATE_WAIT_PATCHLISTFILE,	// 패치 리스트를 받으려 함
	STATE_SELECTION,			// 리스트에서 받을 파일선별과정 
	STATE_OLD_PATCH,			// 
	STATE_PATCH_END,			// 패치 완료
};

class CHttpDownload;
class CPatchManager;
class CTranslucentButton;

class CBetaPatchClientDlg : public CDialog
{
// Construction
public:
	CBetaPatchClientDlg(CWnd* pParent = NULL);	// standard constructor

private:
	CHttpDownload*	m_pHttpDownload;

public:
	CBrush m_brush;
	COLORREF m_clrTransparent;

protected:
	BOOL			m_bClose;
	int				m_nState;
	CPicture		m_pic;
	CWnd			m_wndBrowser;
	CWnd			m_wndBrowser2;
	
public:
	BOOL			InitControls();
	void			CreateWebControl( LPCTSTR szURL );
	void			CreateWebControl2( LPCTSTR szURL );
	void			GameStart();
	void			SetPatchVersionString( int nVersion );
	void			NewPatchClient();
	BOOL			IsClose() const	{ return m_bClose; }
	void			ShowDialog( );

// Dialog Data
	//{{AFX_DATA(CBetaPatchClientDlg)
	enum { IDD = IDD_BETAPATCHCLIENT_DIALOG };
	CButtonST		m_Button_Start;
	CButtonST		m_Button_Register;
	CProgressCtrl	m_File_Progress;
	CProgressCtrl	m_Total_Progress;
	CLabel			m_Static_FileNumber;
	CLabel			m_Static_DownSpeed;
	CLabel			m_Static_CurrentState;
	CLabel			m_Static_MasqueradeVersion;
	CHyperLink		m_Static_HomeLink;
	CHyperLink		m_Static_HomeLink2;
	CButtonST		m_Button_Option;
	CButtonST		m_Button_Exit;

	CButtonST		m_Button_Minimize;
	CButtonST		m_Button_Start_Disable;
	CButtonST		m_Button_PatchNotes;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBetaPatchClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBetaPatchClientDlg)
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnExit();
	afx_msg void OnOption();
	afx_msg void OnMinimize();
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
//	afx_msg LRESULT OnNcHitTest(CPoint point);
#else
	afx_msg UINT OnNcHitTest(CPoint point);
#endif
	afx_msg	LRESULT OnHttpDownloadThreadFinished( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT OnHttpDownloadFail( WPARAM wParam, LPARAM lParam );
	afx_msg void OnButtonStart();
	afx_msg void OnButtonRegister();
	afx_msg void OnButtonPatchNotes();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BETAPATCHCLIENTDLG_H__179321FA_0D9A_43C5_A089_598C606DDDC5__INCLUDED_)
