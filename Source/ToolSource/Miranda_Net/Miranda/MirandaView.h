// MirandaView.h : interface of the CMirandaView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIRANDAVIEW_H__40B15E60_8B3D_4876_9D9D_720565487679__INCLUDED_)
#define AFX_MIRANDAVIEW_H__40B15E60_8B3D_4876_9D9D_720565487679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\_Common\Camera.h"
//#include "CtrlPanel.h"

typedef std::vector< BsSfxUnit* >		BsSfxUnitContainer;
typedef BsSfxUnitContainer::iterator	BsSfxUnitIter;

class CMirandaView : public CView, public CD3DApplication
{
protected: // create from serialization only
	CMirandaView();
	DECLARE_DYNCREATE(CMirandaView)

// Attributes
public:
	CMirandaDoc* GetDocument();
	//CCtrlPanel*	 GetPanel();

#ifdef __AXYZ
    class CUSTOM_VERTEX
    {
    public:
        D3DXVECTOR3				    m_v3Pos;
        D3DCOLOR				    m_dwColor;
		
        enum 
		{ 
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
		float						m_u;
		float						m_v;

        enum { 
            FVF = ( D3DFVF_XYZ | D3DFVF_DIFFUSE |D3DFVF_TEX1 )
        };
    };
	
	CUSTOM_VERTEX2			m_Flat[4];	

// Operations
public:

	HWND				m_hwndRenderWindow;
	HWND				m_hwndRenderFullScreen;

	CSfxModel			m_SfxObj;
	// 카메라 클래스 
	Camera				m_Camera;

	D3DXMATRIX			m_matProj;
	// 모드 관련 변수
	bool				m_bPlayMode;
	bool				m_bIsFocused;

	// 조명 관련 변수들 
	DWORD				m_dwAmbient;
	CMapStringToPtr		m_mapLight;
	DWORD				m_dwLightIndex;
	int					m_nBloom;
	D3DLIGHT9			m_lightFogSky;
	D3DLIGHT9			m_light;
	D3DMATERIAL9		m_baseMaterial;

	// fog 관련 
	int					m_nVisibilityLand;
	FLOAT				m_fFogStartValue;
	FLOAT				m_fFogEndValue;
	FLOAT				m_fFogDensity;

	// 마우스 관련
	POINT				m_ptLast;
	bool				m_bIsLBClick, m_bIsRBClick, m_bIsMBClick;

	//sun!
	CModelObject*	m_pAutoModel;							//Drag용 model
	CModelObject*	m_pOverloadBox;
	CString			m_strLoadedName;
	BOOL			m_bRenderBone;
	int				m_nParts;
	CString			m_strLoadedO3DName;;
	CString			m_strBoneName;
	CString			m_strFollowedBone; // 현재 sfx가 링크된 본이름 

	CD3DFont* m_pFont;
	CD3DFontAPI* m_pFontApi;

	//sfx
	BsSfxUnit*		m_pSfxUnit;

	BsSfxUnitContainer m_cSfxUnits;
		
	void Load_O3D( const TCHAR* szFullName );		
	void Load_SFX( const TCHAR* szFullName );

	void Load_SFX_Multi( const TCHAR* szFullName, const TCHAR* szBone );
	void ClearSfxUnits( );

//protected:
	BOOL LoadModel_Auto( const TCHAR* szFileName );		//Drag로 o3d를 로드 할경우 자동으로 본을 로드하고 Ani List를 만들어준다 
	void LoadBone_Auto( const TCHAR* szFileName );
	void RenderText( );

	void Load_MTE_Texture1( const TCHAR* szModelName );	// Multitexture effect 제 1번 texture 
	void Load_MTE_Texture2( const TCHAR* szModelName );	// Multitexture effect 제 2번 texture 

	//end of sun

	//gmpbigsun(20110427) : 모델올리기
	float m_fMoveUP;

public:

	BOOL CreateDevice();
	HRESULT Init();
	HRESULT Update();
	HRESULT Render();
	HRESULT Uninit();

	HRESULT InvalidateDeviceObjects();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();

	virtual HRESULT AdjustWindowForChange();
	
	VOID RenderScene( BOOL bFrameSkipping = TRUE )
	{ 
		SetFrameSkip( bFrameSkipping );
		Render3DEnvironment(); 
	}

	void	SetLight( BOOL bLight );
	void	AddLight( CLight* pLight );
	CLight*	GetLight( LPCTSTR lpszKey );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirandaView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMirandaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMirandaView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnDestroy();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MirandaView.cpp
inline CMirandaDoc* CMirandaView::GetDocument()
   { return (CMirandaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIRANDAVIEW_H__40B15E60_8B3D_4876_9D9D_720565487679__INCLUDED_)
