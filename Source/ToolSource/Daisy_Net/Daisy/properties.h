
#if !defined(AFX_PROPERTIES_H__778E8237_493A_4C12_9D82_B4A520A66AC9__INCLUDED_)
#define AFX_PROPERTIES_H__778E8237_493A_4C12_9D82_B4A520A66AC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 


/////////////////////////////////////////////////////////////////////////////
// CProperties
//
class CProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(CProperties)

// Construction
public:
	CProperties(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CProperties(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProperties)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProperties();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProperties)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#include "WndDialog.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropAppletGeneral dialog

class CPropAppletGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropAppletGeneral)

	CString m_strOldTexture;
	CString m_strOldDefine;
	//BOOL m_bError;
// Construction
public:
	CDaisyView* m_pDaisyView;
	CTexture* m_pTexture;
	LPWNDAPPLET m_pWndApplet_;
	CWndDialog* m_pWndDialog;
	CPropAppletGeneral();
	~CPropAppletGeneral();

// Dialog Data
	//{{AFX_DATA(CPropAppletGeneral)
	enum { IDD = IDD_PROP_APPLET_GENERAL };
	int		m_nWidth;
	int		m_nHeight;
	BOOL	m_bTile;
	CString	m_strToolTip;
	CString	m_strSurfaceFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropAppletGeneral)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropAppletGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnTextureBrowser();
	afx_msg void OnFitTexutreSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonDefaultImg();
};



/////////////////////////////////////////////////////////////////////////////
// CPropControlGeneral dialog
//
class CDaisyView;

class CPropControlGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropControlGeneral)

	CString m_strOldDefine;

// Construction
public:
	CDaisyView* m_pDaisyView;
	CTexture* m_pTexture;
	LPWNDAPPLET m_pWndApplet_;
	LPWNDCTRL m_pWndCtrl_;
	CWndBase* m_pWndBase;
	CWndDialog* m_pWndDialog;
	CString m_strOldTexture;
	CPropControlGeneral();
	~CPropControlGeneral();

// Dialog Data
	//{{AFX_DATA(CPropControlGeneral)
	enum { IDD = IDD_PROP_CONTROL_GENERAL };
	int		m_nWidth;
	int		m_nHeight;
	BOOL	m_bVisible;
	BOOL	m_bTabStop;
	BOOL	m_bGroup;
	BOOL	m_bDisabled;
	CString	m_strToolTip;
	BOOL	m_bTile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropControlGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropControlGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnTextureBrowser();
	afx_msg void OnFitTexutreSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//flyingjin
	//속성창에 x,y좌표 보이게 수정
	int m_Edit_X;
	int m_Edit_Y;
};


/////////////////////////////////////////////////////////////////////////////
// CPropControlBaseStyle dialog
//
class CPropControlBaseStyle : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropControlBaseStyle)


public:


	CPropControlBaseStyle();
	~CPropControlBaseStyle();

	enum { IDD = IDD_PROP_CONTROL_BASE_STYLE };

	afx_msg void	OnBnClickedFontColor( void );
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


protected:


	void			LimitFontColorValue( int& nColorValue, CString& strFontColorDDX  );		

	virtual BOOL	OnInitDialog( void );
	virtual void	DoDataExchange( CDataExchange* pDX );
	virtual void	OnOK( void );

	DECLARE_MESSAGE_MAP()


public:


	CDaisyView*		m_pDaisyView;
	LPWNDAPPLET		m_pWndApplet_;
	LPWNDCTRL		m_pWndCtrl_;
	CWndBase*		m_pWndBase;
	CWndDialog*		m_pWndDialog;

	BOOL			m_bNoDrawFrame;
	BOOL			m_bVScrollBar;

#ifdef __DAISY_UPDATE_2
	BOOL			m_bAlightRight;
	CButton			m_CheckAlign;
#endif 

#ifdef __DAISY_UPDATE_4

	int				m_nStaticHAlign;
	int				m_nStaticVAlign;
	CButton			m_CheckHAlign;
	CButton			m_CheckVAlign;

	enum StaticHAlign
	{
		ALIGN_H_LEFT, 
		ALIGN_H_RIGHT, 
		ALIGN_H_CENTER, 
	};
	enum StaticVAlign
	{
		ALIGN_V_TOP, 
		ALIGN_V_BOTTOM, 
		ALIGN_V_CENTER
	};

#endif 


protected:


	CString			m_strFontColorR;
	CString			m_strFontColorG;
	CString			m_strFontColorB;

	CEdit			m_EditFontColorR;
	CEdit			m_EditFontColorG;
	CEdit			m_EditFontColorB;
	CButton			m_ButtonChangeFontColor;
};



/////////////////////////////////////////////////////////////////////////////
// CPropAppletBaseStyle dialog

class CPropAppletBaseStyle : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropAppletBaseStyle)

// Construction
public:
	LPWNDAPPLET m_pWndApplet_;
	CWndDialog* m_pWndDialog;
	
	CPropAppletBaseStyle();
	~CPropAppletBaseStyle();

// Dialog Data
	//{{AFX_DATA(CPropAppletBaseStyle)
	enum { IDD = IDD_PROP_APPLET_BASE_STYLE1 };
	BOOL	m_bNoDrawFrame;
	BOOL	m_bThickFrame;
	BOOL	m_bCaption;
	BOOL	m_bNoFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropAppletBaseStyle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropAppletBaseStyle)
	virtual BOOL OnInitDialog();
	afx_msg void OnCaption();
	afx_msg void OnNoFrame();
	afx_msg void OnNoDrawFrame();
	afx_msg void OnThickframe();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCloseButton();
	BOOL m_bCloseBT;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIES_H__778E8237_493A_4C12_9D82_B4A520A66AC9__INCLUDED_)
