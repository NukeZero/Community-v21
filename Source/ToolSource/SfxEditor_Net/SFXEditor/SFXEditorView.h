// SFXEditorView.h : interface of the CSFXEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(SFX88990099)
#define SFX88990099

#include "sfxbase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef D3DFVF_3DVERTEX 
	#define D3DFVF_3DVERTEX (D3DFVF_XYZ)
#endif


/*
struct FVF_3DVERTEX
{
    D3DXVECTOR3 vPos;	// The 3D position for the vertex
};
*/



class CSFXEditorView : public CView, public CD3DApplication
{
private:
#ifdef __AXYZ
    class CUSTOM_VERTEX
    {
    public:
        D3DXVECTOR3				    m_v3Pos;
        D3DCOLOR				    m_dwColor;
		
        enum { 
            FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE)
        };
    };
	
    CUSTOM_VERTEX           m_Allow[6];
#endif // __AXYZ
    class CUSTOM_VERTEX2
    {
    public:
        D3DXVECTOR3				    m_v3Pos;
		D3DXVECTOR2				    m_v2Tx;
		
        enum { 
            FVF = (D3DFVF_XYZ|D3DFVF_TEX1)
        };
    };
	
	CUSTOM_VERTEX2			m_Flat[4];	
public:
#ifdef __ACENTERMODEL
	CModelObject*              m_pModel;
	BOOL					   m_bChrModel;
	BOOL					   m_bModelPlay;
#endif
#ifdef __AXYZ
	BOOL					   m_bAxis;
#endif
	BOOL					   m_bBG;
	BOOL					   m_bFlat;
	

protected: // create from serialization only
	CSFXEditorView();
	DECLARE_DYNCREATE(CSFXEditorView)

// Attributes
public:
	CSFXEditorDoc* GetDocument();

	C2DRender   m_2DRender;
	DWORD m_dwWndId;
	int m_testx,m_testy;

// Operations
public:
//	BOOL IsReady() { return m_bReady; }

	HRESULT ConfirmDevice( D3DCAPS9*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();
	BOOL IsReady() {return m_bActive;}
	virtual HRESULT AdjustWindowForChange();
	VOID RenderScene( BOOL bFrameSkipping = TRUE ) { 
		SetFrameSkip( bFrameSkipping );
		Render3DEnvironment(); 
	}
	HRESULT CheckForLostFullscreen();
	BOOL CreateDevice() ;

	HWND    m_hwndRenderWindow;
	HWND    m_hwndRenderFullScreen;

	float m_fCamerax,m_fCameray;
	D3DXVECTOR3 m_vCameraPos;
	POINT m_ptClicked;
	POINT m_ptClickedScrollState;
	POINT m_ptCurrent;

	POINT m_ptLClicked;
	POINT m_ptMClicked;
	POINT m_ptRClicked;

	FVF_3DVERTEX m_vGrids[100];
	DWORD m_nGridCount;

	BOOL m_bLBut;
	BOOL m_bMBut;
	BOOL m_bRBut;
	WORD m_nCurrentFrame;
	CSfxModel m_SfxObj;
	BOOL m_bPlay;

	D3DXVECTOR3 m_vObjPos;
	FLOAT m_fObjAngle;

	// 종료시 저장 구현
	bool	m_bIsModified;
	int		m_nFileType;
	void SetModified(bool bIsMod);
	void SetFileType(int nFileType);
	bool IsModified() const;
	int GetFileType() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSFXEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSFXEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSFXEditorView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnEditPlay();
	afx_msg void OnEditStop();
	afx_msg void OnViewchrmodel();
	afx_msg void OnUpdateViewchrmodel(CCmdUI* pCmdUI);
	afx_msg void OnViewaxis();
	afx_msg void OnUpdateViewaxis(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnModelSetting();
	afx_msg void OnViewbg();
	afx_msg void OnUpdateViewbg(CCmdUI* pCmdUI);
	afx_msg void OnFlat();
	afx_msg void OnUpdateFlat(CCmdUI* pCmdUI);
	afx_msg void OnInfo();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SFXEditorView.cpp
inline CSFXEditorDoc* CSFXEditorView::GetDocument()
   { return (CSFXEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SFX88990099)
