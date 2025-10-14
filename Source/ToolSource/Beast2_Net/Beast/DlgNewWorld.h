#if !defined(AFX_DLGNEWWORLD_H__49708CE3_BD84_4784_A704_AF03DB23B760__INCLUDED_)
#define AFX_DLGNEWWORLD_H__49708CE3_BD84_4784_A704_AF03DB23B760__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewWorld.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewWorld dialog

class CDlgNewWorld : public CDialog, public CD3DApplication
{
	HWND       m_hwndRenderWindow;
	C2DRender  m_2DRender;
	CTexture   m_texture;

// Construction
public:
	CDlgNewWorld(CWnd* pParent = NULL);   // standard constructor
	void EnableButton() ;

// Dialog Data
	//{{AFX_DATA(CDlgNewWorld)
	enum { IDD = IDD_NEW_WORLD };
	CString	m_strTexture;
	BOOL	m_bIndoor;
	int		m_nUnitHeight;
	int		m_nUnitWidth;
	CString	m_strWorldHeight;
	CString	m_strWorldWidth;
	float	m_fLandHeight;
	CString	m_strHeightBitmap;
	float	m_fMutipleHeight;
	int		m_iMPU;
	//}}AFX_DATA
	int m_nSelectTexture;

	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT Render();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewWorld)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewWorld)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeComboTexture();
	afx_msg void OnBrowser();
	afx_msg void OnChangeMultipleHeight();
	afx_msg void OnChangeHeightBitmap();
	afx_msg void OnIndoor();
	afx_msg void OnActivate(UINT state, CWnd* other, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWWORLD_H__49708CE3_BD84_4784_A704_AF03DB23B760__INCLUDED_)
