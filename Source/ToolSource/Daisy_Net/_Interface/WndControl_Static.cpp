

#include "StdAfx.h"
#include "WndControl_Static.h"


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndStatic::CWndStatic()
#ifndef __V050322_CACHEQUEST
:	m_strTexture( _T("") )
#endif
{
	m_byWndType		= WTYPE_STATIC; 
#ifdef __V050322_CACHEQUEST
	m_strTexture	= DEF_CTRL_GROUP;
	m_bTile			= TRUE;
#endif
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndStatic::~CWndStatic()
{
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndStatic::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID ,int nR, int nG, int nB )	
{
	if( lpszCaption && strlen( lpszCaption ) ) m_strTitle = lpszCaption;
	if( !(dwStyle & WSS_GROUPBOX) )
		m_strTexture = _T("");

	SetFontColor( nR, nG, nB );
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME , rect,  pParentWnd, nID );
}


//---------------------------------------------------------------------------------------------
// 윈도우 영역 설정
// param	: rectWnd, bOnSize
// return	: void
//---------------------------------------------------------------------------------------------
void CWndStatic::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: p2DRender
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndStatic::OnDraw( C2DRender* p2DRender )
{
	CPoint point( 4, 4 );

#ifdef __FREE_TYPE_FONT
	//@@ --> 추가 KYT : 이 무슨 4, 4 하드코딩이야..
	//  static 컨트롤의 Rect를 이용해 계산 한다.
	CRect rect = GetWindowRect();

	UINT nHalfHeightSize =  (UINT)( ( rect.bottom - rect.top ) * 0.5f );
	UINT dwMaxHeight = (UINT)( p2DRender->GetFont()->GetMaxHeight() * 0.5f );

	point.y  = nHalfHeightSize - dwMaxHeight;

	if( point.y < 0  )
		point.y = 0;

	//<-- 
#endif 
	if( IsWndStyle( WSS_GROUPBOX ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
#ifdef __DAISY_UPDATE_4
		if( IsWndStyle( WSS_ALIGNHRIGHT ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= rect.Width() - sizeTitle.cx;
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNHCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= (rect.Width() - sizeTitle.cx) / 2;
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNVBOTTOM ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= rect.Height() - sizeTitle.cy;
			if( point.y < 2 )
				point.y = 2;
		}
		if( IsWndStyle( WSS_ALIGNVCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= (rect.Height() - sizeTitle.cy + 4) / 2;
			if( point.y < 2 )
				point.y = 2;
		}
#endif 

		p2DRender->TextOut( 0 + point.x, point.y, m_strTitle, m_dwColor );//m_dwColor );

#ifndef __FREE_TYPE_FONT
		p2DRender->TextOut( 1 + point.x, point.y, m_strTitle, m_dwColor );
#endif 
		p2DRender->SetFont( pOldFont );
	}
	else
	{
#ifdef __V050322_CACHEQUEST
		CPoint point( 2, 2 );
#endif
//#ifdef __NEWINTERFACE
//#	ifdef __CLIENT
		if( IsWndStyle( WSS_MONEY ) )
		{
#ifndef __DAISY
			CString str;
			str = GetNumberFormatEx(m_strTitle);
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
			p2DRender->TextOut( 0 + point.x, point.y, str, m_dwColor );
			p2DRender->TextOut( 1 + point.x, point.y, str, m_dwColor );
			p2DRender->SetFont( pOldFont );
#endif
		}
		else
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
#ifdef __DAISY_UPDATE_2
			if( IsWndStyle( WSS_ALIGNRIGHT ) )
			{
				CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
				CRect rect	= GetWindowRect();
				point.x	= rect.Width() - sizeTitle.cx - 4;
				if( point.x < 2 )
					point.x = 2;
			}
#endif //__DAISY_UPDATE_2
#ifdef __DAISY_UPDATE_4
			if( IsWndStyle( WSS_ALIGNHRIGHT ) )
			{
				CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
				CRect rect	= GetWindowRect();
				point.x	= rect.Width() - sizeTitle.cx;
				if( point.x < 2 )
					point.x = 2;
			}
			if( IsWndStyle( WSS_ALIGNHCENTER ) )
			{
				CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
				CRect rect	= GetWindowRect();
				point.x	= (rect.Width() - sizeTitle.cx) / 2;
				if( point.x < 2 )
					point.x = 2;
			}
			if( IsWndStyle( WSS_ALIGNVBOTTOM ) )
			{
				CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
				CRect rect	= GetWindowRect();
				point.y	= rect.Height() - sizeTitle.cy;
				if( point.y < 2 )
					point.y = 2;
			}
			if( IsWndStyle( WSS_ALIGNVCENTER ) )
			{
				CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
				CRect rect	= GetWindowRect();
				point.y	= (rect.Height() - sizeTitle.cy + 4) / 2;
				if( point.y < 2 )
					point.y = 2;
			}
#endif 
			p2DRender->TextOut( 0 + point.x, point.y,  m_strTitle, m_dwColor );

#ifndef __FREE_TYPE_FONT
			//@@ 수정 KYT : 이건 왜 한번더 그리지..일단 주석.
   			p2DRender->TextOut( 1 + point.x, point.y,  m_strTitle, m_dwColor );
#endif 
			p2DRender->SetFont( pOldFont );
		}
	}
}


//---------------------------------------------------------------------------------------------
// 
// param	: point
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndStatic::IsPickupSpace( CPoint point )
{
	if( IsWndStyle( WSS_GROUPBOX ) || IsWndStyle( WSS_PICTURE ) )
	{
#ifdef __DAISY
		CRect rectOut = GetWindowRect();
		CRect rectIn  = GetWindowRect();
		rectIn.DeflateRect( 6, 12, 6,  6 );
		if( rectOut.PtInRect( point ) && !rectIn.PtInRect( point ) )
		{
			return TRUE;
		}
#endif
		return FALSE;
	}
	CRect rect = GetWindowRect();
	if( rect.PtInRect( point ) )
	{
		if( m_bLButtonDown )
			int a = 0;

		return TRUE;
	}
	return FALSE;
}


//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 눌렀다 땟을 때
// param	: nFlags, point
// return	: void
//---------------------------------------------------------------------------------------------
void CWndStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if(m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}


//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 눌렀을 때
// param	: nFlags, point
// return	: void
//---------------------------------------------------------------------------------------------
void CWndStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if(m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: nFlags, point
// return	: void
//---------------------------------------------------------------------------------------------
void CWndStatic::PaintFrame( C2DRender* p2DRender )
{
#ifdef __TOON
	if( m_bTile == FALSE )
	{
		if( m_strTexture.IsEmpty() == FALSE && IsWndStyle( WSS_PICTURE ) )
		{
			SetTexture(  p2DRender->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(), m_strTexture)  );

			CRect rcWindow = GetWindowRect();
			m_pTexture->Render( p2DRender, CPoint(0, 0), CPoint(rcWindow.right, rcWindow.bottom ) );
		}
	}

	if( m_strTexture.IsEmpty() == FALSE ||IsWndStyle( WSS_PICTURE ) == FALSE  )
		return;

	CRect rect = GetWindowRect();
	p2DRender->RenderRect( rect, 0xff000000 );
#endif

#ifndef __TOON
	if( m_strTexture.IsEmpty() == FALSE ||IsWndStyle( WSS_PICTURE ) == FALSE  )
		return;

	CRect rect = GetWindowRect();
	p2DRender->RenderRect( rect, 0xff000000 );
#endif 
}

#ifdef __TOON

void CWndStatic::AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface,  D3DFORMAT d3dFormat )
{
	if( IsWndStyle( WSS_PICTURE ) && !m_bTile )
		return ;

	CWndBase::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );

}
void CWndStatic::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat /*= D3DFMT_A4R4G4B4 */)
{
	if( IsWndStyle( WSS_PICTURE ) && !m_bTile )
		return ;

	CWndBase::AfterSkinTexture( pDest, size, d3dFormat);
}

void CWndStatic::AdjustWndBase(D3DFORMAT d3dFormat /*= D3DFMT_A4R4G4B4*/)
{
	if( IsWndStyle( WSS_PICTURE )  && !m_bTile )
		return ;

	CWndBase::AdjustWndBase( d3dFormat );
}
#endif 


//---------------------------------------------------------------------------------------------
// 스킨 텍스쳐 추가
// param	: pDest, SizeSurface
// return	: void
//---------------------------------------------------------------------------------------------
#ifndef __V050322_CACHEQUEST
void CWndStatic::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface )
{
	if( m_strTexture.IsEmpty() || !IsVisible() )
		return;
	if( IsWndStyle( WSS_PICTURE ) )
	{
		LPIMAGE lpImage;
		CSize sizeBmp;

		if( m_strWndTileMap.Lookup( m_strTexture, (void*&)lpImage ) == FALSE )
		{
			lpImage = new IMAGE;
			LoadImage( MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),m_strTexture ), lpImage );//, &sizeBmp[i], &nImgBit );
			m_strWndTileMap.SetAt( m_strTexture, lpImage );
		}
		///////////////////////////////////////////////////////
		CRect rect = GetWindowRect( TRUE );
		rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );

		PaintTexture( pDest, lpImage, rect.TopLeft(), sizeSurface );
		return;
	}
	LPIMAGE lpImage[12];
	CSize sizeBmp[12];
	CString m_strTile = m_strTexture;//"WndEditTile06.tga";
	CString strFileName;

	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 9; i++ )
	{
		CString strTemp1 = m_strTile.Left( m_strTile.GetLength() - 6 );
		CString strTemp2 = m_strTile.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			LoadImage( MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),strFileName ), lpImage[i] );//, &sizeBmp[i], &nImgBit );
			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
		}
	}
	///////////////////////////////////////////////////////
	CRect rect = GetWindowRect( TRUE );
	rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	//ect = GetClientRect();
	if(	IsWndStyle( WBS_VSCROLL ) )
		rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int y = 0;
	CPoint point;

	int nHeight = rect.Height() - ( lpImage[ 2 ]->size.cy + lpImage[ 8 ]->size.cy );
	nHeight = ( nHeight / lpImage[ 5 ]->size.cy ) + 2;

	for( i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, y ) + rect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface );
				else
				if( j == nWidth - 1 )
				{
					PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface );
					y += lpImage[ 2 ]->size.cy;
				}
				else
					PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface );
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface );
				else
				if( j == nWidth - 1 )
				{
					PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface );
					y += lpImage[ 8 ]->size.cy;
				}
				else
					PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface );					
			}
			else
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface );
				else
				if( j == nWidth - 1 )
				{
					PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface );
					y += lpImage[ 5 ]->size.cy;
				}
				else
					PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface );					
			}
		}
	}
}

#endif 


