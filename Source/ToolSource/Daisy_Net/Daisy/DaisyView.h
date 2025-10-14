
#if !defined(AFX_DAISYVIEW_H__7DD09CBA_7AC8_47D8_B859_FFD82CDAAF21__INCLUDED_)
#define AFX_DAISYVIEW_H__7DD09CBA_7AC8_47D8_B859_FFD82CDAAF21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "properties.h"
#include "../_Interface/WndControlFactory.h"


class CDaisyView : public CView, public CD3DApplication
{


protected: 


						CDaisyView();
						DECLARE_DYNCREATE(CDaisyView)


public:


	virtual				~CDaisyView();

	CDaisyDoc*			GetDocument( void );

	BOOL				CreateDevice( void );

	HRESULT 			InitDeviceObjects( void );
	HRESULT 			DeleteDeviceObjects( void );

	HRESULT 			RestoreDeviceObjects( void );
	HRESULT 			InvalidateDeviceObjects( void );

	HRESULT 			OneTimeSceneInit( void );
	HRESULT 			FrameMove( void );
	HRESULT 			Render( void );
	VOID				RenderScene( void )			{	Render3DEnvironment();		}

	HRESULT				CheckForLostFullscreen( void );
	virtual HRESULT		AdjustWindowForChange( void );


#ifdef _DEBUG
	virtual void		AssertValid( void ) const;
	virtual void		Dump(CDumpContext& dc) const;
#endif


public:


	virtual BOOL 		Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL 		PreCreateWindow(CREATESTRUCT& cs);
	virtual void 		OnInitialUpdate( void );
	virtual void 		OnDraw(CDC* pDC);  


protected:

	bool				OpenProperty( BOOL bIsControl, LPWNDAPPLET pWndApplet, CWndDialog* pWndDialog, CWndBase* pWndBase );

	virtual LRESULT		WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


public:


	DWORD				m_dwWndId;
	HWND				m_hwndRenderWindow;
	HWND				m_hwndRenderFullScreen;
	CProperties*		m_pProperties;

	C2DRender   		m_2DRender;
	CWndManager 		m_WndMng;

#ifdef __DAISY_UPDATE_1
	CWndBase			m_pTempWnd;
#endif 


private:


	CWndControlFactory	m_WndControlFactory;


protected:


	afx_msg void 		OnFileNewapp( void );
	afx_msg void 		OnWndPicture( void );
	afx_msg void 		OnLayoutCling( void );
	afx_msg void 		OnInsertNewClass( void );
	afx_msg void 		OnLayoutShowgrid( void );
	afx_msg void 		OnLayoutGuideSetting( void );
	afx_msg void 		OnSize(UINT nType, int cx, int cy);
	afx_msg void 		OnUpdateWndPicture(CCmdUI* pCmdUI);
	afx_msg void 		OnUpdateLayoutCling(CCmdUI* pCmdUI);
	afx_msg void 		OnUpdateInsertNewClass(CCmdUI* pCmdUI);
	afx_msg void 		OnUpdateLayoutShowgrid(CCmdUI* pCmdUI);
	afx_msg void 		OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void 		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void 		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void 		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL 		OnPopupCommand(UINT nID);

	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  
	inline CDaisyDoc* CDaisyView::GetDocument()
	   { return (CDaisyDoc*)m_pDocument; }
#endif


#endif 