// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDACTIVEDESKTOP_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDACTIVEDESKTOP_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////////////
// 매트릭스 태스크탑 
//
class CWndDesktopMatrix : public CWndBase
{
	CWndButton m_wndButton;
	CTexturePack m_texturePack;
	int m_vertical[ 32 ];
	int m_type[ 32 ];
	int m_speed[ 32 ];

public:
	CWndDesktopMatrix(); 
	virtual ~CWndDesktopMatrix();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL Process();

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT DeleteDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
};
//////////////////////////////////////////////////////////////////////////////////////
// 클락웍스 태스크탑 
//
class CWndDesktopClockworks : public CWndBase
{
	CWndButton m_wndButton;
	CModelObject m_mesh;

public:
	CWndDesktopClockworks(); 
	virtual ~CWndDesktopClockworks();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL Process();

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual HRESULT DeleteDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
};
#endif // !defined(AFX_WNDACTIVEDESKTOP_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
