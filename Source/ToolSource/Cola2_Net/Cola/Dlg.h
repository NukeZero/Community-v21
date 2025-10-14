#if !defined(AFX_DLG_H__B40898F0_1946_4271_9ECA_38A92154D838__INCLUDED_)
#define AFX_DLG_H__B40898F0_1946_4271_9ECA_38A92154D838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg.h : header file
//
#include "CoolDialogBar.h"

/////////////////////////////////////////////////////////////////////////////
// CMyDialog dialog

class CMyDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CMyDialog)

public:
	CMyDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CMyDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgOption dialog

class CDlgOption : public CDialog
{
// Construction
public:
	CDlgOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOption)
	enum { IDD = IDD_OPTION };
	float	m_fScale;
	BYTE	m_nR;
	BYTE	m_nG;
	BYTE	m_nB;
	CString	m_strResource;
	CString	m_strTexture;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOption)
	virtual BOOL OnInitDialog();
	afx_msg void OnColordlg();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDlgConvert dialog

class CDlgConvert : public CDialog
{
// Construction
public:
	CDlgConvert(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConvert)
	enum { IDD = IDD_CONVERT };
	CListCtrl	m_List;
	CString	m_strState;
	//}}AFX_DATA

	CStringArray	m_straryList;
	void	SayMessage( LPCTSTR str );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConvert)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	void OnDropFiles( HDROP hDropInfo );
	void AddList( LPCTSTR szFileName );

	// Generated message map functions
	//{{AFX_MSG(CDlgConvert)
	afx_msg void OnDropFiles( HDROP hDropInfo ); 
	afx_msg void OnConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDlgTexture dialog

class CDlgTexture : public CDialog
{
// Construction
public:
	CDlgTexture(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTexture)
	enum { IDD = IDD_TEXTURE };
	float	m_fScrlU;
	float	m_fScrlV;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTexture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTexture)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDlgInfo dialog

class CDlgInfo : public CDialog
{
	CModel	*m_pModel;
	int		m_nMaxList;
	int		m_nMaxList2;
// Construction
public:
	CDlgInfo(CWnd* pParent = NULL);   // standard constructor

	void	SetModel( CModel *pModel ) { m_pModel = pModel; }
	void	AddList( LPCTSTR szName, LPCTSTR szValue );
	void	AddList2( LPCTSTR szName, LPCTSTR szValue );
	void	UpdateInfo( void );
	void	UpdateInfo2( char *pBuff );
	// Dialog Data
	//{{AFX_DATA(CDlgInfo)
	enum { IDD = IDD_INFO };
	CListCtrl	m_ctrlList2;
	CTabCtrl	m_ctrlTab;
	CListCtrl	m_ctrlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDlgTrack dialog

class CDlgTrack : public CDialog
{
	CModel	*m_pModel;
//	float	m_fFrame;		// 현재 프레임 외부에서 갱신시켜준다.

	CBitmap	m_hBitmap;
	COLORREF m_clrBk;
	CBitmap m_hAttrHit, m_hAttrSnd;

	int		m_nModeDivision;		// 눈금 모드
// Construction
public:
//	int		m_nMaxFrame;	// 로딩된 최대 프레임

	CDlgTrack(CWnd* pParent = NULL);   // standard constructor

	void SetModel( CModel *pModel ) { m_pModel = pModel; }
	void UpdateWavName( void );


// Dialog Data
	//{{AFX_DATA(CDlgTrack)
	enum { IDD = IDD_TRACK };
	CString	m_strID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgTrack)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnOK();
	afx_msg void OnKillfocusFilenameWav();
	afx_msg void OnViewFrame();
	afx_msg void OnUpdateViewFrame(CCmdUI* pCmdUI);
	afx_msg void OnViewSec();
	afx_msg void OnUpdateViewSec(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDlgAnimation dialog

class CDlgAnimation : public CDialog
{
// Construction
public:
	CDlgAnimation(CWnd* pParent = NULL);   // standard constructor

	//gmpbigsun : 파일이름을 받아서 관련 애니파일을 리스트에 등록시킨다.
	int ListUpAllAnimation( const char* szFilename );

// Dialog Data
	//{{AFX_DATA(CDlgAnimation)
	enum { IDD = IDD_DIALOG_ANIMATIONLIST };
	CListCtrl	m_kListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAnimation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAnimation)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__B40898F0_1946_4271_9ECA_38A92154D838__INCLUDED_)
