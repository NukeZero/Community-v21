
#pragma once

#include "WndBase.h"

namespace WndTabCtrl
{
	static const UINT		nTabTextureCount( 6 );
	static const UINT		nInventoryTabTextureCount( 2 );
	static const UINT		nReserveTabControlItem( 10 );
	static const DWORD		dwTextureKeyColor( 0xFFFF00FF );
}

using namespace WndTabCtrl;

//---------------------------------------------------------------------------------------------
// 윈도우 탭 컨트롤 아이템 구조체
//---------------------------------------------------------------------------------------------
typedef struct tagWTCITEM
{
    UINT		mask;
    DWORD		dwState;
    DWORD		dwStateMask;
	LPCTSTR		pszText;
    int			cchTextMax;
    int			iImage;
    LPARAM		lParam;
	CWndBase*	pWndBase;

	tagWTCITEM()
	:	mask( NULL )
	,	dwState( NULL )
	,	dwStateMask( NULL )
	,	pszText( _T("") )
	,	cchTextMax( NULL )
	,	iImage( NULL )
	,	lParam( NULL )
	,	pWndBase( NULL )
	{
	}
} WTCITEM, FAR *LPWTCITEM;


//---------------------------------------------------------------------------------------------
// 탭 컨트롤 클레스
//---------------------------------------------------------------------------------------------
class CWndTabCtrl: public CWndBase
{

public:


	enum TabTitleAlign
	{ 
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};


public:

							CWndTabCtrl();
	virtual					~CWndTabCtrl();

	BOOL					Create( DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );

	BOOL					InsertItem( int nItem, WTCITEM* pTabCtrlItem );

	BOOL					GetItem( int nItem, WTCITEM* pTabCtrlItem ) const;
	LPWTCITEM				GetTabItem( int nItemNumber ) const;
	LPWTCITEM				GetSelectedTab( void ) const;
	const TabTitleAlign		GetTabTitleAlign( void ) const;
	int						GetCurSel( void ) const;
	int						GetSize( void )					{	return m_aTab.size();				}

	void					SetButtonLength( int nLength )	{	m_nTabButtonLength = nLength;		}
	void					SetTabTitleAlign( const TabTitleAlign eTabTitleAlign );
	int						SetCurSel( int nItem );

	virtual	void 			OnInitialUpdate( void );
	virtual void 			AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual BOOL 			OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void 			OnLButtonDown(UINT nFlags, CPoint point);
	virtual void			PaintFrame(C2DRender* p2DRender);
	virtual void 			OnDraw( C2DRender* p2DRender );
	virtual void 			OnSize(UINT nType, int cx, int cy);


protected:


	CRect					GetWndTabCtrlWindowRect( void );

	void					LoadTabTexture( void );
	void					LoadInventoryTabTexture( void );

	void					AdditionalSkinTexture_Background( CRect rtRect, LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat );
	void					AdditionalSkinTexture_TabImage( CRect rtRect, LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat );

	void					DrawTab( C2DRender* p2DRender );
	void					DrawTabSelect( C2DRender* p2DRender );
	

protected:


	int						m_nCurSelect;
	int						m_nTabButtonLength;

	TabTitleAlign			m_eTabTitleAlign;

	CTexture				m_TextureTab[ nTabTextureCount ];
	CTexture				m_TextureTabSelect[ nTabTextureCount ];
	CTexture				m_aInventoryTexture[ nInventoryTabTextureCount ];

	vector< LPWTCITEM >		m_aTab;

};
