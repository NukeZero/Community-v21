
#if _MSC_VER > 1000
#pragma once
#endif 

#include "WndBase.h"

class CWndStatic : public CWndBase
{


public:


					CWndStatic();
					~CWndStatic();

	BOOL			Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID ,int nR, int nG, int nB );

	virtual BOOL	IsPickupSpace(CPoint point); 
	virtual	void	PaintFrame( C2DRender* p2DRender );
	void			OnDraw( C2DRender* p2DRender );

#ifdef __TOON
	virtual void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void AdjustWndBase( D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
#endif 

	void			SetWndRect( CRect rectWnd, BOOL bOnSize );

	virtual void	OnLButtonUp(UINT nFlags, CPoint point);
	virtual void	OnLButtonDown(UINT nFlags, CPoint point);

#ifndef __V050322_CACHEQUEST

	virtual void	AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface );

#endif 

public:

#ifndef __V050322_CACHEQUEST
	CString			m_strTexture;
#endif 

};

