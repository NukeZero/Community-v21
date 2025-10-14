
#include "StdAfx.h"
#include "WndControl_ScrollBar.h"

CWndScrollBar::CWndScrollBar()
{
	m_nScrollPos = 0;
	m_nScrollMinPos = 0;
	m_nScrollMaxPos = 0;
	m_nScrollPage = 0;
	//m_dwScrollStyle = 0;
	m_byWndType = WTYPE_SCROLLBAR;
}
CWndScrollBar::~CWndScrollBar()
{
}
BOOL CWndScrollBar::Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	CRect rc = rect;
	rc.left = rc.right - 15;//VSCROLL_WIDTH;
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_DOCKING, rc, pParentWnd, nID );
}
int CWndScrollBar::GetScrollPos() const // Retrieves the current position of a scroll box. 
{
	return m_nScrollPos;
}
int CWndScrollBar::SetScrollPos(int nPos, BOOL bRedraw) // Sets the current position of a scroll box. 
{
	m_nScrollPos = nPos;
	return 1;
}
void CWndScrollBar::SetMinScrollPos() 
{
	m_nScrollPos = m_nScrollMinPos;
}
void CWndScrollBar::SetMaxScrollPos() 
{
	m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	if( m_nScrollPos < 0 ) m_nScrollPos = 0;
}
int CWndScrollBar::GetMinScrollPos() 
{
	return m_nScrollMinPos;
}
int CWndScrollBar::GetMaxScrollPos() 
{
	return m_nScrollMaxPos;
}
int CWndScrollBar::GetScrollRange() const
{
	return m_nScrollMaxPos - m_nScrollMinPos;
}
int CWndScrollBar::GetScrollAbiliableRange() 
{
	CRect rect = GetClientRect();
	rect.top += m_wndArrow1.GetClientRect().Height(); 
	rect.bottom -= m_wndArrow2.GetClientRect().Height();
	int nHeight = rect.Height();
	int nPadHeight = ( m_nScrollPage * nHeight / GetScrollRange() );
	if( nPadHeight < 6 ) nPadHeight = 6;
	rect.bottom = rect.top + nPadHeight;
	// 역산해서 길이를 조정하자.
	return nHeight * m_nScrollPage / nPadHeight;
}
int CWndScrollBar::GetScrollAbiliablePos() 
{
	// Range : pos = AbiliableRange : AbiliablePos
	return m_nScrollPos * GetScrollAbiliableRange() / GetScrollRange();
}
int CWndScrollBar::GetScrollAbiliablePage() 
{
	// Range : page = AbiliableRange : AbiliablePage
	return m_nScrollPage * GetScrollAbiliableRange() / GetScrollRange();
}
void CWndScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
{
	*lpMinPos = m_nScrollMinPos;
	*lpMinPos = m_nScrollMaxPos;
}
void CWndScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw) // Sets minimum and maximum position values for the given scroll bar. 
{
	m_nScrollMinPos = nMinPos;
	m_nScrollMaxPos = nMaxPos;
	/*
	// auto hide
	if( GetScrollRange() - m_nScrollPage <= 0 )
		SetVisible( FALSE );
	else
		SetVisible( TRUE );
	*/
}
void CWndScrollBar::ShowScrollBar(BOOL bShow) // Shows or hides a scroll bar. 
{
}
BOOL CWndScrollBar::EnableScrollBar(UINT nArrowFlags) // Enables or disables one or both arrows of a scroll bar. 
{
	return TRUE;
}
BOOL CWndScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw) // Sets information about the scroll bar. 
{
	return TRUE;
}
BOOL CWndScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask) // Retrieves information about the scroll bar. 
{
	return TRUE;
}
int CWndScrollBar::GetScrollLimit() // Retrieves the limit of the scroll bar
{
	return 1;
}
int CWndScrollBar::GetScrollPage() const
{
	return m_nScrollPage;
}	
void CWndScrollBar::SetScrollPage(int nPage)
{
	m_nScrollPage = nPage;
}	

void CWndScrollBar::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;

	if( bOnSize )
		OnSize(0,m_rectClient.Width(),m_rectClient.Height());
}


void CWndScrollBar::OnInitialUpdate()
{
	m_pTexButtVScrBar   = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtVScrBar.bmp"   ), WNDCOLOR_DEFAULT_KEY );
	m_pTexButtVScrPUp   = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtVScrPUp.bmp"   ), WNDCOLOR_DEFAULT_KEY );
	m_pTexButtVScrPDown = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtVScrPDown.bmp" ), WNDCOLOR_DEFAULT_KEY );
	m_pTexButtVScrPBar  = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtVScrPBar.bmp"  ), WNDCOLOR_DEFAULT_KEY );

	CRect rect = GetClientRect();
	rect.bottom = rect.bottom / 4 * 4;
	m_wndArrow1.Create( "",0,CRect( rect.left, rect.top, rect.right, rect.top + rect.Width()),this,1000);//,m_pSprPack,13+0);
	m_wndArrow1.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtVScrUp.tga" ), TRUE );
	m_wndArrow1.FitTextureSize();

	rect.top = 0;
	m_wndArrow2.Create( "",0,CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom),this,1001);//,m_pSprPack,13+2);
	m_wndArrow2.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtVScrDown.tga" ), TRUE );
	m_wndArrow2.FitTextureSize();

	m_wndArrow1.SetPushTime(500);
	m_wndArrow2.SetPushTime(500);

	m_wndArrow1.DelWndStyle( WBS_NODRAWFRAME );
	m_wndArrow2.DelWndStyle( WBS_NODRAWFRAME );
	m_bPushPad = FALSE;
}


//---------------------------------------------------------------------------------------------
// Paint Frame ( 그리는 프레임 )
// param	: p2DRender
// return	: void
//---------------------------------------------------------------------------------------------
void CWndScrollBar::PaintFrame(C2DRender* p2DRender)
{
	static int TEXTURE_V_SCROLL_ARROW_SIZE_Y ( NULL );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		TEXTURE_V_SCROLL_ARROW_SIZE_Y = 16;
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		TEXTURE_V_SCROLL_ARROW_SIZE_Y = 14;

	static const int TEXTURE_V_SCROLL_BAR_SIZE_Y	= m_pTexButtVScrBar->m_size.cy;

	CRect rect( GetClientRect() );
	CPoint point( rect.TopLeft() );

	FLOAT fHeightSize = static_cast<FLOAT>( ( rect.Height() - ( TEXTURE_V_SCROLL_ARROW_SIZE_Y * 2 ) ) / TEXTURE_V_SCROLL_BAR_SIZE_Y );
	p2DRender->RenderTexture( CPoint( point.x, point.y + TEXTURE_V_SCROLL_ARROW_SIZE_Y ), m_pTexButtVScrBar, CWndBase::m_nAlpha, 1.0F, fHeightSize );
}


void CWndScrollBar::OnDraw(C2DRender* p2DRender)
{
	if( GetScrollRange() && GetScrollRange() - m_nScrollPage > 0 )
	{
		CRect rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height();
		rect.bottom -= m_wndArrow2.GetClientRect().Height();

		int nHeight = rect.Height();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();

		int nPadHeight = ( nPage * nHeight / nRange );
		if( nPadHeight < 6 )
		{
			nHeight -= 6 - nPadHeight;
			nPadHeight = 6;
		}
		rect.bottom = rect.top + nPadHeight;
		int nPadPos =  ( nPos * nHeight / nRange );

		rect.OffsetRect( 0, nPadPos );

		static const int TEXTURE_SCROLL_P_UP_SIZE_Y = m_pTexButtVScrPUp->m_size.cy;
		static const int TEXTURE_SCROLL_P_BAR_SIZE_Y = m_pTexButtVScrPBar->m_size.cy;
		static const int TEXTURE_SCROLL_P_DOWN_SIZE_Y = m_pTexButtVScrPDown->m_size.cy;

		CPoint point = rect.TopLeft();
		FLOAT fHeightSize = static_cast<FLOAT>( ( rect.Height() / TEXTURE_SCROLL_P_BAR_SIZE_Y ) - 1 );

		if( nHeight > 0 )
		{
			p2DRender->RenderTexture( CPoint( point.x, point.y ), m_pTexButtVScrPUp );
			p2DRender->RenderTexture( CPoint( point.x, point.y + TEXTURE_SCROLL_P_UP_SIZE_Y ), m_pTexButtVScrPBar, CWndBase::m_nAlpha, 1.0F, fHeightSize );
			p2DRender->RenderTexture( CPoint( point.x, point.y + ( rect.Height() - TEXTURE_SCROLL_P_DOWN_SIZE_Y ) ), m_pTexButtVScrPDown );
		}
	}
}
//CTexturePack CWndScrollBar::m_texturePack;


void CWndScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	OnLButtonDown( nFlags, point );
}
void CWndScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( GetScrollPage() >= GetMaxScrollPos() )
		return;

	SetCapture();
	CRect rect = GetClientRect();
	rect.top += m_wndArrow1.GetClientRect().Height();
	rect.bottom -= m_wndArrow2.GetClientRect().Height();
	if(rect.PtInRect(point) && GetScrollRange())
	{
		rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height(); 
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();
		int nPadHeight = ( nPage * nHeight / nRange );
		if( nPadHeight < 6 )
		{
			nHeight -= 6 - nPadHeight;
			nPadHeight = 6;
		}
		rect.bottom = rect.top + nPadHeight;
		int nPadPos =  ( nPos * nHeight / nRange );
		rect.OffsetRect( 0, nPadPos );

		if( rect.PtInRect( point ) )
		{
			SetCapture();
			m_nPos = point.y - rect.top;
			m_bPushPad = TRUE;
		}
		else
		if( point.y < rect.top )
		{
			m_nScrollPos -= m_nScrollPage;
			if(m_nScrollPos < m_nScrollMinPos)
				m_nScrollPos = m_nScrollMinPos;
		}
		else
		if( point.y > rect.bottom )
		{
			m_nScrollPos += m_nScrollPage;
			if(m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
				m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
		}
	}
}
void CWndScrollBar::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bPushPad)
	{
		point.y -= ( m_wndArrow1.GetClientRect().Height() + m_nPos );
  		CRect rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height(); 
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		// rect.Height() : pt.y = GetScrollRange() : x
		int nPadHeight = ( m_nScrollPage * nHeight / GetScrollRange() );
		if( nPadHeight < 6 ) 		
			nHeight -= 6 - nPadHeight;
		m_nScrollPos = point.y * GetScrollRange() / nHeight;
		if( m_nScrollPos < m_nScrollMinPos ) m_nScrollPos = m_nScrollMinPos;
		if( m_nScrollPos > m_nScrollMaxPos - m_nScrollPage ) m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	}
}
BOOL CWndScrollBar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 1000: 
			if(--m_nScrollPos < m_nScrollMinPos)
				m_nScrollPos = m_nScrollMinPos;
			//FLTRACE_LOG( PROGRAM_NAME, _T( "%d" ), m_nScrollPos);
			break;
		case 1001:
			if(++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
				m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
			//FLTRACE_LOG( PROGRAM_NAME, _T( "%d" ), m_nScrollPos);
			break;
	}
	return TRUE;
}
void CWndScrollBar::SetScrollDown()
{
	if(++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
		m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
}
void CWndScrollBar::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();
	rect.bottom = rect.bottom / 4 * 4;

	m_wndArrow1.SetWndRect( CRect( rect.left, rect.top, rect.right, rect.top + rect.Width() ) );
	m_wndArrow2.SetWndRect( CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom) );

//	m_string.Reset( m_pFont, &GetClientRect() );
	CWndBase::OnSize( nType, cx, cy);
}


SIZE CWndScrollBar::GetArrowUpButtomTextureSize()
{
	SIZE TextureSize;

	TextureSize.cx = 0;
	TextureSize.cy = 0;
	
	CTexture* pTexture = m_wndArrow1.GetTexture();
	FLERROR_ASSERT_LOG_RETURN( pTexture, TextureSize, _T("[윈도우 스크롤 바] 올리기 스크롤 버튼 포인터가 유효하지 않습니다.") );

	TextureSize = pTexture->m_size;

	return TextureSize;
}

SIZE CWndScrollBar::GetArrowDownButtomTextureSize()
{
	SIZE TextureSize;

	TextureSize.cx = 0;
	TextureSize.cy = 0;
	
	CTexture* pTexture = m_wndArrow2.GetTexture();
	FLERROR_ASSERT_LOG_RETURN( pTexture, TextureSize, _T("[윈도우 스크롤 바] 내리기 스크롤 버튼 포인터가 유효하지 않습니다.") );

	TextureSize = pTexture->m_size;

	return TextureSize;
}