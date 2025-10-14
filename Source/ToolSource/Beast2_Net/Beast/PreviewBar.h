#if !defined(AFX_PREVIEWBAR_H__4CC6886A_46CE_447A_8E52_BF3D03AB982E__INCLUDED_)
#define AFX_PREVIEWBAR_H__4CC6886A_46CE_447A_8E52_BF3D03AB982E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPreviewer window

class CPreviewer : public CStatic, public CD3DApplication
{
// Construction
public:
	DWORD  m_dwPreview;

	HWND       m_hwndRenderWindow;
	C2DRender  m_2DRender;
	CTexture   m_texture;
 	CModelObject* m_pObject3D;
	//CObject3D* m_pObject3D;

	CPreviewer();
	BOOL Create( LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff );
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT Render();
	HRESULT CheckForLostFullscreen();
	HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT Format,D3DFORMAT backBufferFormat );
	void SetModel( DWORD dwType, DWORD dwIndex );
	void SetTexture( LPCTSTR lpszFileName );

	virtual HRESULT AdjustWindowForChange();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewer)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPreviewBar window

class CPreviewBar : public CSizingControlBar2
{
// Construction
public:
	CPreviewBar();
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	//CPreviwer 
	CPreviewer m_viwer;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWBAR_H__4CC6886A_46CE_447A_8E52_BF3D03AB982E__INCLUDED_)
