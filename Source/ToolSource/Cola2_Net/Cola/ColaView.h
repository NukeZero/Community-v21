// ColaView.h : interface of the CColaView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLAVIEW_H__33121A82_928D_4362_9079_B88E8D8BE768__INCLUDED_)
#define AFX_COLAVIEW_H__33121A82_928D_4362_9079_B88E8D8BE768__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "d3d9.h"
//#include "d3dutil.h"

#include "ASE.h"

class CColaView : public CView, public CD3DApplication
{
private:
//    DWORD        m_dwVertexShader;    // Vertex shader
    CD3DFont*  m_pFont;              // Font for drawing text
    HWND       m_hwndRenderWindow;
    HWND       m_hwndRenderFullScreen;
    CArcBall         m_ArcBall;            // Mouse rotation utility
	D3DXMATRIX	m_mWorld, m_mView, m_mProj;		// View화면의 월드, 뷰, 투영 매트릭스
	int		m_nWidth;
	int		m_nHeight;
	int		m_nNumParts;		// o3d를 로딩하면서 LoadElement할때 몇번 파츠에 로딩할꺼냐 하는..

	D3DXVECTOR3	m_vRayOrig, m_vRayDir;


//	CAse	*m_pASE;
	D3DXMATRIX	m_mModel;		// 모델의 매트릭스

	D3DXVECTOR3	m_vCPos;	// 아크볼 
	D3DXVECTOR3 m_vCRot;

	D3DXVECTOR3	m_vLightPos, m_vLightDir;

	bool		m_bUseCLOD;
	int			m_nLastLodLv;

protected: // create from serialization only
	CColaView();
	DECLARE_DYNCREATE(CColaView)

// Attributes
public:
	CModel	*m_pModel;			// ase나 modelobject가 올수 있다.
	CModelObject *m_pPlane;		// 그림자 테스트용 플랜

	int		m_nCount;
	int		m_nLightRotateCnt;

	CColaDoc* GetDocument();

	void	LoadASE( LPCTSTR szFileName );
	void	LoadO3D( LPCTSTR szFileName );
	void	LoadModel( LPCTSTR szFileName );

	void	SetLight( void );
	void	RenderGrid( void );
	void	RenderParticle( void );
		
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColaView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
//    HRESULT ConfirmDevice( D3DCAPS9*,DWORD,D3DFORMAT );
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT FrameMove();
    HRESULT Render();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT FinalCleanup();

//    BOOL	IsReady() { return m_bReady; }
    BOOL	IsReady() { return TRUE; }
	HRESULT CheckForLostFullscreen();
    VOID	RenderScene() { Render3DEnvironment(); }
	HRESULT AdjustWindowForChange();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CColaView)
	afx_msg void OnAseLoad();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBonSaveAs();
	afx_msg void OnAniSave();
	afx_msg void OnMesSaveFilename();
	afx_msg void OnUpdateBonSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAniSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMesSaveFilename(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMesLoad();
	afx_msg void OnArcrotate();
	afx_msg void OnSelect();
	afx_msg void OnUpdateArcrotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSwdforce1();
	afx_msg void OnSwdforce2();
	afx_msg void OnUpdateSwdforce1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwdforce2(CCmdUI* pCmdUI);
	afx_msg void OnSwdforce3();
	afx_msg void OnSwdforce4();
	afx_msg void OnUpdateSwdforce3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwdforce4(CCmdUI* pCmdUI);
	afx_msg void OnMesSave();
	afx_msg void OnMesSaveas();
	afx_msg void OnSpeedup();
	afx_msg void OnSpeeddown();
	afx_msg void OnSpeedclr();
	afx_msg void OnOption();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdateViewFnormal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewVnormal(CCmdUI* pCmdUI);
	afx_msg void OnViewFnormal();
	afx_msg void OnViewVnormal();
	afx_msg void OnNormalClear();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFrameskip();
	afx_msg void OnUpdateFrameskip(CCmdUI* pCmdUI);
	afx_msg void OnConvert();
	afx_msg void OnTexture();
	afx_msg void OnUpdateTexture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMesSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMesSaveas(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLighting(CCmdUI* pCmdUI);
	afx_msg void OnViewLighting();
	afx_msg void OnEnvmap();
	afx_msg void OnUpdateEnvmap(CCmdUI* pCmdUI);
	afx_msg void OnPlay();
	afx_msg void OnUpdatePlay(CCmdUI* pCmdUI);
	afx_msg void OnPrev();
	afx_msg void OnNext();
	afx_msg void OnUpdatePrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNext(CCmdUI* pCmdUI);
	afx_msg void OnAniLoad();
	afx_msg void OnAseSaveauto();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateAseSaveAuto(CCmdUI* pCmdUI);
	afx_msg void OnAttrhit();
	afx_msg void OnUpdateAttrhit(CCmdUI* pCmdUI);
	afx_msg void OnAttrsnd();
	afx_msg void OnUpdateAttrsnd(CCmdUI* pCmdUI);
	afx_msg void OnSolid();
	afx_msg void OnUpdateSolid(CCmdUI* pCmdUI);
	afx_msg void OnWireframe();
	afx_msg void OnUpdateWireframe(CCmdUI* pCmdUI);
	afx_msg void OnPoint();
	afx_msg void OnUpdatePoint(CCmdUI* pCmdUI);
	afx_msg void OnViewbball();
	afx_msg void OnUpdateViewbball(CCmdUI* pCmdUI);
	afx_msg void OnViewbbone();
	afx_msg void OnUpdateViewbbone(CCmdUI* pCmdUI);
	afx_msg void OnViewbbnot();
	afx_msg void OnUpdateViewbbnot(CCmdUI* pCmdUI);
	afx_msg void OnAutorotate();
	afx_msg void OnUpdateAutoRotate(CCmdUI* pCmdUI);
	afx_msg void OnEdgedFace();
	afx_msg void OnUpdateEdgedFace(CCmdUI* pCmdUI);
	afx_msg void OnRotateLight();
	afx_msg void OnUpdateRotateLight(CCmdUI* pCmdUI);
	afx_msg void OnLod1();
	afx_msg void OnUpdateLod1(CCmdUI* pCmdUI);
	afx_msg void OnLod2();
	afx_msg void OnUpdateLod2(CCmdUI* pCmdUI);
	afx_msg void OnLod3();
	afx_msg void OnUpdateLod3(CCmdUI* pCmdUI);
	afx_msg void OnGrid();
	afx_msg void OnUpdateGrid(CCmdUI* pCmdUI);
	afx_msg void OnReloadReflection();
	afx_msg void OnNextOp();
	afx_msg void OnLightning();
	afx_msg void OnUpdateLightning(CCmdUI* pCmdUI);
	afx_msg void OnCollobject();
	afx_msg void OnUpdateCollobject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCamLod(CCmdUI* pCmdUI);
	afx_msg void OnCamLod();
	afx_msg void OnViewTextureEffect();
	afx_msg void OnUpdateViewTextureEffect( CCmdUI* pCmdUI );
	afx_msg void OnViewTextureEffectBase();
	afx_msg void OnUpdateViewTextureEffectBase( CCmdUI* pCmdUI );

	afx_msg void OnTopView();
	afx_msg void OnUpdateTopView( CCmdUI* pCmdUI );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ColaView.cpp
inline CColaDoc* CColaView::GetDocument()
   { return (CColaDoc*)m_pDocument; }
#endif

extern CColaView	*g_pView;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLAVIEW_H__33121A82_928D_4362_9079_B88E8D8BE768__INCLUDED_)
