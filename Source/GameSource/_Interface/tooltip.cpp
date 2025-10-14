
#include "StdAfx.h" 
#include "DialogMsg.h"

extern CDialogMsg g_DialogMsg;


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CToolTip::CToolTip()
:	m_dwToolTipId( -1 )	
,	m_nAlpha( 255 )
,	m_nPosition( NULL )
,	m_nSubToolTipFlag( CWndMgr::TOOL_TIP_SWITCH_MAIN )
,	m_nSubToolTipNumber( CWndMgr::TOOL_TIP_SWITCH_MAIN )
,	m_bReadyToopTip( FALSE )
,	m_bPutToolTip( FALSE )
,	m_bToolTip( FALSE )
,	m_bEnable( TRUE )	
,	m_strToolTip( _T("") )
,	m_ptToolTip( CPoint( 0, 0 ) )
,	m_ptOldToolTip( CPoint( 0, 0 ) )
,	m_rect( 0, 0, 0, 0 )
,	m_rectRender( 0, 0, 0, 0 )
,	m_nRevisedRect( 0, 0, 0, 0 )
,	m_nAdded( NULL )
,	m_nSlot( NULL )
,	m_pUltimateItemBase( NULL )
,	m_pUltimateTexture( NULL )
,	m_pJewelBgTexture( NULL )
#ifdef __ENCHANT_BARUNA16
,	m_pBarunaTexture( NULL )
#endif
#ifndef __IMPROVE_MAP_SYSTEM
,	m_nMonInfoCnt( NULL )
#endif
{
	ZeroMemory( m_nAddedJewel, sizeof(int) * 5 );

#ifndef __IMPROVE_MAP_SYSTEM
	ZeroMemory( m_pDwMonId, sizeof(DWORD) * 5 );	
#endif 

}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CToolTip::~CToolTip()
{
	Delete();
}


//---------------------------------------------------------------------------------------------
// Delete ( 삭제하기 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CToolTip::Delete()
{
	int nloadTexture( 0 );

	for( int i = 0 ; i < MAX_TT ; ++i )
	{
		for( int j = 0 ; j < 9 ; ++ j )
		{
			m_apTextureToolTip[j].DeleteDeviceObjects();
			++nloadTexture;
		}
	}
}



//---------------------------------------------------------------------------------------------
// Initialize Texture ( 텍스쳐 초기화 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CToolTip::InitTexture()
{
	CString		szTextName( _T("") );
	CString		szTextNamebuf( _T("WndTooltipTile") );

	int			nloadTexture( 0 );

	char		szBuf[32] = { NULL, };

	for( int i = 0 ; i < MAX_TT ; ++i )
	{
		for( int j = 0 ; j < 9 ; ++ j )
		{
			szTextName = szTextNamebuf;
			FLSPrintf( szBuf, _countof( szBuf ), "%02d", ( i * 9 ) + j );	// 툴팁 텍스쳐 9조각으로되어있음
			szTextName += szBuf;
			szTextName += ".tga";
			m_apTextureToolTip[nloadTexture].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), szTextName ), WNDCOLOR_DEFAULT_KEY, TRUE );
			++nloadTexture;
		}
	}

	//sun: 12, 무기 피어싱
	//Ultimate Icon & Bg Load
	CString strPath( _T("") );
	
	if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
		strPath = MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), "Icon_Ultimate.dds");
	else
		strPath = MakePath( DIR_ICON, "Icon_Ultimate.dds");

	m_pUltimateTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, WNDCOLOR_DEFAULT_KEY );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		m_pJewelBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "WndChgElemItem.bmp"), WNDCOLOR_DEFAULT_KEY );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		m_pJewelBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "back_Slotitem.tga"), WNDCOLOR_DEFAULT_KEY );

#ifdef __ENCHANT_BARUNA16
	m_pBarunaTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "Icon_Baruna01.dds" ), 0xffff00ff );
#endif 
}


//---------------------------------------------------------------------------------------------
// Cancel ToolTip ( 툴팁 취소 )
// param	: void
// return	: void 
// Desc		: 현재 열려진 툴팁을 취소( 사라지게 ) 한다.
//---------------------------------------------------------------------------------------------
void CToolTip::CancelToolTip()
{
	if( FALSE == m_bEnable )
		return;

	m_bReadyToopTip = FALSE;
	m_bToolTip		= FALSE;
	m_dwToolTipId	= -1;		
}


//---------------------------------------------------------------------------------------------
// Reset Map Monster Info ( 맵 몬스터 정보를 초기화 한다. )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CToolTip::ResetMapMonsterInfo()
{

#ifndef __IMPROVE_MAP_SYSTEM

	m_nMonInfoCnt = 0;
	ZeroMemory( m_pDwMonId, sizeof(DWORD) * 5 );	

#else

	m_vecMapMonsterID.clear();

#endif 

}


//---------------------------------------------------------------------------------------------
// Put ToolTip ( 툴팁을 입력한다. )
// param	: ...
// return	: void 
// Desc		: 0.5초가 지나면 알아서 출력한다.
//---------------------------------------------------------------------------------------------
void CToolTip::PutToolTip( DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos )
{
	if( -1 == nToolTipPos )
		return;

	PutToolTip( dwToolTipId, (LPCTSTR)string, rect, pt, nToolTipPos );
}


//---------------------------------------------------------------------------------------------
// Put ToolTip ( 툴팁을 입력한다. )
// param	: ...
// return	: void 
// Desc		: 0.5초가 지나면 알아서 출력한다.
//---------------------------------------------------------------------------------------------
void CToolTip::PutToolTip( DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos )
{
	if( FALSE == m_bEnable ||
		FALSE == rect.PtInRect(pt) )
		return ;

	if( dwToolTipId != m_dwToolTipId ||
		m_rect != rect )
	{
		m_nAlpha		= 0;
		m_rect			= rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId	= dwToolTipId;

		m_timerToopTip.Set(0);

		if(m_bToolTip)
			m_bToolTip = FALSE;

		m_nSubToolTipNumber = CWndMgr::TOOL_TIP_SWITCH_MAIN;
	}

	if( NULL != lpszString )
	{
		m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
		m_strToolTip = lpszString;

		PFONTCOLOR_WNDTOOLTIP pWndToolTip = g_WndFontColorManager->GetWndToolTip();
		
		if( pWndToolTip )
			m_strToolTip.SetColor( pWndToolTip->m_stToolTip.GetFontColor() );
	}

	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );

	m_rectRender	= CRect( 0, 0, size.cx + 6, size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) );
	m_rect			= rect;
	m_nPosition		= nToolTipPos;
	m_bPutToolTip	= TRUE;
	m_nAdded		= 0;
	m_nSlot			= 0;

	ResetMapMonsterInfo();
}


//---------------------------------------------------------------------------------------------
// Put ToolTip ( 툴팁을 입력한다. )
// param	: ...
// return	: void 
// Desc		: 0.5초가 지나면 알아서 출력한다.
//---------------------------------------------------------------------------------------------
void CToolTip::PutToolTip( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos )
{
	if( FALSE == m_bEnable )
		return;

	if( -1 == nToolTipPos )
		nToolTipPos = 0;
	else if(!rect.PtInRect(pt))
		return;

	if( dwToolTipId != m_dwToolTipId )
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		m_timerToopTip.Set(0);

		if(m_bToolTip)
			m_bToolTip = FALSE;

		m_nSubToolTipNumber = CWndMgr::TOOL_TIP_SWITCH_MAIN;
	}

	m_strToolTip = string;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );

	CSize size		= CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender	= CRect( 0, 0, size.cx + 6, size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) );
	m_rect			= rect;
	m_nPosition		= nToolTipPos;
	m_bPutToolTip	= TRUE;
	m_nAdded		= 0;
	m_nSlot			= 0;

	ResetMapMonsterInfo();
}


//---------------------------------------------------------------------------------------------
// Put ToolTip Ex ( 툴팁을 입력한다.( 확장 ) )
// param	: ...
// return	: void 
// Desc		: 0.5초가 지나면 알아서 출력한다.
//---------------------------------------------------------------------------------------------
void CToolTip::PutToolTipEx( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos, int nSubToolTipFlag )
{
	if(m_bEnable == FALSE)
		return;
	if(!rect.PtInRect(pt))
		return;

	if( m_rect == rect )
	{
		if( nSubToolTipFlag == 0 )
		{
			if( m_bPutToolTip == FALSE )
			{
				m_nAlpha = 0;
				m_rect = rect;
				m_bReadyToopTip = TRUE;
				m_dwToolTipId = dwToolTipId;
				m_timerToopTip.Set(0);
				if(m_bToolTip)
					m_bToolTip = FALSE;
			}
		}
		else if( nSubToolTipFlag == 1 || nSubToolTipFlag == 2 )
		{
			if( g_toolTip.GetReadyToolTipSwitch() == TRUE )
				m_bReadyToopTip = TRUE;
		}
	}
	else
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		m_timerToopTip.Set(0);
		if(m_bToolTip)
			m_bToolTip = FALSE;
	}

	m_strToolTip = string;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );

	CSize size			= CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender		= CRect( 0, 0, size.cx + 6, size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) );
	m_rect				= rect;
	m_nPosition			= nToolTipPos;
	m_bPutToolTip		= TRUE;
	m_nAdded			= 0;
	m_nSlot				= 0;
	m_nSubToolTipFlag	= nSubToolTipFlag;

	ResetMapMonsterInfo();

}


//---------------------------------------------------------------------------------------------
// Process ( 처리 )
// param	: ...
// return	: void 
//---------------------------------------------------------------------------------------------
void CToolTip::Process(CPoint pt,C2DRender* p2DRender)
{
	CD3DFont* pFont = p2DRender->m_pFont;

	if(m_bEnable == FALSE)
		return;

	if(m_bPutToolTip == FALSE)
	{
		CancelToolTip();
		return;
	}

	if( m_bPutToolTip == TRUE && m_bReadyToopTip == TRUE && m_timerToopTip.Over() )
	{
		if(m_bToolTip == NULL)
			m_ptToolTip = pt;
		else
			m_bToolTip = FALSE;

		if(0) // ??? 뭐지?
		{
			m_bReadyToopTip = FALSE;
		}
		else
		{
			m_bToolTip = TRUE;
		}
	}

	if(m_rect.PtInRect(pt) == FALSE)
		m_bPutToolTip = FALSE;

	m_nAlpha += 15;

	if( m_nAlpha > 255 )
		m_nAlpha = 255;
}


//---------------------------------------------------------------------------------------------
// 툴팁의 시작 위치 얻기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
CPoint CToolTip::GetStartPoint()
{		
	CPoint ptStartPoint( m_ptToolTip );
	
	switch( m_nPosition )
	{
		case 0:
			{
				ptStartPoint = m_rect.TopLeft();
				ptStartPoint.y -= m_rectRender.Height() + 10;
			}
			break;
		case 1:
			{
				ptStartPoint = CPoint(m_rect.left, m_rect.bottom + 20 );		
			}
			break;
		case 2:
			{
				ptStartPoint = m_rect.TopLeft();
				ptStartPoint.x -= m_rectRender.Width() + 2;
			}
			break;
		case 3:
			{
				ptStartPoint = CPoint(m_rect.right,m_rect.top);
			}
			break;
	}

	switch( m_nSubToolTipFlag )
	{
	case CWndMgr::TOOL_TIP_SWITCH_SUB1:
		{
			const CRect rectMain = g_toolTip.GetRevisedRect();
			const CRect rectRender = g_toolTip.GetRenderRect();
			ptStartPoint.x = rectMain.left + rectRender.Width() + 16;
			break;
		}
	case CWndMgr::TOOL_TIP_SWITCH_SUB2:
		{
			const CRect rectSub1 = g_toolTipSub1.GetRevisedRect();
			const CRect rectRender = g_toolTipSub1.GetRenderRect();
			ptStartPoint.x = rectSub1.left + rectRender.Width() + 16;
			break;
		}
	}

	return ptStartPoint;
}


CRect CToolTip::AdjustRenderRect( C2DRender* p2DRender, CPoint& ptTooltip )
{
	// 보석 슬롯이 있을 경우 강제로 크기를 늘린다.
	static const int TOOL_TIP_WIDTH_FOR_SLOTS = 194; 

	if( m_nSlot > 0 && m_rectRender.right < TOOL_TIP_WIDTH_FOR_SLOTS )
		m_rectRender.right = TOOL_TIP_WIDTH_FOR_SLOTS;

	CRect rtToolTip( ptTooltip.x, ptTooltip.y, ptTooltip.x + m_rectRender.Width(), ptTooltip.y + m_rectRender.Height() );

	if( NULL == p2DRender )
		return rtToolTip;

	// 사각형이 만들어졌지만, 화면 가장자리를 벗어날 수 있다. 벗어나면 벗어날 수 없도록 수정 
	switch( m_nSubToolTipFlag )
	{
	case CWndMgr::TOOL_TIP_SWITCH_MAIN:
		{
			static const int TOOL_TIP_MARGIN = 16;

			switch( m_nSubToolTipNumber )
			{
			case CWndMgr::TOOL_TIP_SWITCH_MAIN:
				{
					if( rtToolTip.right + TOOL_TIP_MARGIN > p2DRender->m_clipRect.right )
						ptTooltip.x = p2DRender->m_clipRect.Width() - rtToolTip.Width() - 8;
				}
				break;

			case CWndMgr::TOOL_TIP_SWITCH_SUB1:
				{
					const CRect rectToolTipSub1 = g_toolTipSub1.GetRevisedRect();
					if( rtToolTip.right + rectToolTipSub1.Width() + ( TOOL_TIP_MARGIN * 2 ) > p2DRender->m_clipRect.right )
						ptTooltip.x = p2DRender->m_clipRect.Width() - rtToolTip.Width() - 224;
				}
				break;

			case CWndMgr::TOOL_TIP_SWITCH_SUB2:
				{
					const CRect rectToolTipSub1 = g_toolTipSub1.GetRevisedRect();
					const CRect rectToolTipSub2 = g_toolTipSub2.GetRevisedRect();
					if( rtToolTip.right + rectToolTipSub1.Width() + rectToolTipSub2.Width() + ( TOOL_TIP_MARGIN * 3 ) > p2DRender->m_clipRect.right )
						ptTooltip.x = p2DRender->m_clipRect.Width() - rtToolTip.Width() - 470;
				}
				break;
			}

			if( rtToolTip.top < p2DRender->m_clipRect.top )
				ptTooltip.y = p2DRender->m_clipRect.top + 8;

			if( rtToolTip.bottom + TOOL_TIP_MARGIN > p2DRender->m_clipRect.bottom )
				ptTooltip.y = p2DRender->m_clipRect.Height() - rtToolTip.Height() - 8;

			m_nRevisedRect = CRect( ptTooltip.x, ptTooltip.y, ptTooltip.x + m_rectRender.Width(), ptTooltip.y + m_rectRender.Height() );
		}
		break;

	case CWndMgr::TOOL_TIP_SWITCH_SUB1:
	case CWndMgr::TOOL_TIP_SWITCH_SUB2:
		{
			if( rtToolTip.top < p2DRender->m_clipRect.top )
				ptTooltip.y = p2DRender->m_clipRect.top + 8;
			
			m_nRevisedRect = CRect( ptTooltip.x, ptTooltip.y, ptTooltip.x + m_rectRender.Width(), ptTooltip.y + m_rectRender.Height() );
		}
		break;
	}

	if(m_nSlot > 0)
	{
		rtToolTip.SetRect( ptTooltip.x, ptTooltip.y - (36), ptTooltip.x + m_rectRender.Width(), ptTooltip.y + m_rectRender.Height() );

		if(rtToolTip.top < p2DRender->m_clipRect.top)
		{
			ptTooltip.y = p2DRender->m_clipRect.top + 8;
			rtToolTip.SetRect( ptTooltip.x, ptTooltip.y, ptTooltip.x + m_rectRender.Width(), ptTooltip.y + m_rectRender.Height() + (36) );
		}
	}
	else
		rtToolTip.SetRect( ptTooltip.x, ptTooltip.y, ptTooltip.x + m_rectRender.Width(), ptTooltip.y + m_rectRender.Height() );


	return rtToolTip;
}




//---------------------------------------------------------------------------------------------
// Paint ( 툴팁 그리기 )
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CToolTip::Paint(C2DRender* p2DRender)
{
	if(m_bEnable == FALSE)
		return;

	if( m_bToolTip )
	{
		CPoint	pt( GetStartPoint() );
		CRect	rect( AdjustRenderRect( p2DRender, pt ) );

		int nPlusLow = 8;
		int nPlusColumn = 8;
		int nSetting = g_Option.m_nToolTipTexture * 9;
#ifdef BARUNA_ULTIMATE_UPDATE
		if ( m_nAdded == 3 )	nSetting = 9;	// 바루나일떄 다른 툴팁 텍스쳐로
#endif	// BARUNA_ULTIMATE_UPDATE
		float fNa = float( rect.bottom - rect.top + nPlusLow * 2 ) / 16.0f;
		int nlowDraw = (int)fNa;
		if( nlowDraw == 0 && 0 < fNa )
			nlowDraw = 1;

		fNa = float( rect.right - rect.left + nPlusColumn * 2 ) / 16.0f;
		int nColumnDraw = (int)fNa;
		
		if( nColumnDraw == 0 && 0 < fNa )
			nColumnDraw = 1;

		CRect PlusRect = rect;
		PlusRect.left = rect.left - nPlusLow;
		PlusRect.top = rect.top - nPlusColumn;
		PlusRect.right = rect.right- 16 + nPlusLow;
		PlusRect.bottom = rect.bottom - 16 + nPlusColumn;

		for( int i = 0 ; i < nlowDraw ; ++i )
		{
			for( int j = 0 ; j < nColumnDraw ; ++j )
			{
				// 테두리 처리
				if( i == 0 || j == 0 )
				{
					if( j == 0 && i != 0 )
					{
						if( i + 1 == nlowDraw )
						{
							m_apTextureToolTip[ nSetting + 3 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top + ( i * 16 ) ), CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
							m_apTextureToolTip[ nSetting + 5 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top + ( i * 16 ) ), CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
						}
						else 
						{
							m_apTextureToolTip[ nSetting + 3 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top + ( i * 16 ) ) );
							m_apTextureToolTip[ nSetting + 5 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top + ( i * 16 ) ) );
						}
						
					}
					else if( i == 0 && j != 0 )
					{
						if( j + 1 == nColumnDraw )
						{
							m_apTextureToolTip[ nSetting + 1 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top ), CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
							m_apTextureToolTip[ nSetting + 7 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.bottom ), CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
						}
						else
						{
							m_apTextureToolTip[ nSetting + 1 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top ) );
							m_apTextureToolTip[ nSetting + 7 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.bottom ) );
						}
					}		
					continue;
				}
					
				// 가운데 처리
				if( i == nlowDraw - 1 || j == nColumnDraw - 1 )
				{
					// 끝에 맞지 않은 구조 처리 
					if( i == nlowDraw - 1 && j == nColumnDraw - 1 )
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
						
					}
					else if( i == nlowDraw - 1 )
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );

					}
					else
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
					}
				}
				else
				{
					m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ) );
				}
			}
		}

		m_apTextureToolTip[ nSetting + 0 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top ) );
		m_apTextureToolTip[ nSetting + 2 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top ) );
		m_apTextureToolTip[ nSetting + 6 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.bottom ) );
		m_apTextureToolTip[ nSetting + 8 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.bottom ) );

		p2DRender->TextOut_EditString( rect.TopLeft().x + 3, rect.TopLeft().y + 3, m_strToolTip, 0, 0, 2 );//, D3DCOLOR_ARGB( m_nAlpha * 255 / 255, 0, 0, 0 ) );

#ifndef __IMPROVE_MAP_SYSTEM
//sun: 13, WorldMap 몬스터 표시
		if(m_nMonInfoCnt > 0)
		{
			int nMonElementYPos = 0;
			int nStrLine = 0;

			for(int i=0; i<m_nMonInfoCnt; i++)
			{
				if( (int)( m_strToolTip.GetLineCount() ) > i )
				{
					CString strTemp = m_strToolTip.GetLine(nStrLine);
					
					// Check Line.
					CString strLv, strEnd;
					strEnd = strTemp.GetAt( strTemp.GetLength() - 1 );
					if( strTemp.Find( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ) ) == 0 )
					{
						int nLine = 1;
						if(strEnd != "\n")
						{
							nLine = 2;
							nStrLine++;
							strTemp = m_strToolTip.GetLine(nStrLine);
						}

						strTemp.TrimRight();
						CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent(strTemp);
						MoverProp* pMoverProp = prj.GetMoverProp(m_pDwMonId[i]);
						
						if(pMoverProp)
						{
							if( g_WndMng.m_pWndWorld && pMoverProp->eElementType )
							{
								if(i==0)
									nMonElementYPos = PlusRect.top + 8 + (size.cy + 2) * (nLine - 1);
								else
									nMonElementYPos += (size.cy + 2) * nLine;

								g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint(PlusRect.left + size.cx + 20, nMonElementYPos), pMoverProp->eElementType-1, 255, 1.0f, 1.0f );
							}
						}

						nStrLine++;
					}
				}
			}
		}

#endif // __IMPROVE_MAP_SYSTEM

#ifdef __IMPROVE_MAP_SYSTEM
		if( static_cast< int >( m_vecMapMonsterID.size() ) > 0 )
		{
			int nMonElementYPos = 0;
			int nStringLine = 0;
			for( std::vector< DWORD >::iterator Iterator = m_vecMapMonsterID.begin(); Iterator != m_vecMapMonsterID.end(); ++Iterator )
			{
				CString strTemp = m_strToolTip.GetLine( nStringLine );
				CString strLevel = _T( "" );
				CString strEnd = strTemp.GetAt( strTemp.GetLength() - 1 );
				if( strTemp.Find( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ) ) == 0 )
				{
					int nLine = 1;
					if( strEnd != "\n" )
					{
						nLine = 2;
						++nStringLine;
						strTemp = m_strToolTip.GetLine( nStringLine );
					}

					strTemp.TrimRight();
					CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent( strTemp );
					MoverProp* pMoverProp = prj.GetMoverProp( *Iterator );
					if( pMoverProp )
					{
						if( g_WndMng.m_pWndWorld && pMoverProp->eElementType )
						{
							if( Iterator == m_vecMapMonsterID.begin() )
								nMonElementYPos = PlusRect.top + 8 + ( size.cy + 2 ) * ( nLine - 1 );
							else
								nMonElementYPos += ( size.cy + 2 ) * nLine;

							g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint( PlusRect.left + size.cx + 20, nMonElementYPos ), pMoverProp->eElementType - 1, 255, 1.0f, 1.0f );
						}
					}

					++nStringLine;
				}
			}
		}
#endif // __IMPROVE_MAP_SYSTEM

//sun: 9차 전승관련 Clienet
		if(m_nAdded == 1)
		{
//sun: 12, 무기 피어싱
			CTexture* pTexture;
			
			if(m_pUltimateTexture != NULL)
			{
				if( m_nSubToolTipFlag == CWndMgr::TOOL_TIP_SWITCH_MAIN )
					m_pUltimateTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 8) );
				else
					m_pUltimateTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 24) );
			}

			//Jewel Icon Added
			CPoint point;
			point.x = PlusRect.left + 14 + ((PlusRect.Width() - 194) / 2);
			point.y = PlusRect.bottom - 24;

			for(int i=0; i<m_nSlot; i++)
			{
				if(m_pJewelBgTexture != NULL)
					m_pJewelBgTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );

				//Jewel Render
				if(m_nAddedJewel[i] != 0)
				{
					PT_ITEM_SPEC pItemProp;
					pItemProp = g_xSpecManager->GetSpecItem( m_nAddedJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );
					}
				}

				point.x += 38;
			}
		}
#ifdef __ENCHANT_BARUNA16
		else if( 2 == m_nAdded )
		{
			if( m_pBarunaTexture )
			{
				if( m_nSubToolTipFlag == CWndMgr::TOOL_TIP_SWITCH_MAIN )
					m_pBarunaTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 8) );
				else
					m_pBarunaTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 24) );
	
			}
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
			CTexture* pTexture;
			//Jewel Icon Added
			CPoint point;
			point.x = PlusRect.left + 14 + ((PlusRect.Width() - 194) / 2);
			point.y = PlusRect.bottom - 24;

			for(int i=0; i<m_nSlot; i++)
			{
				if(m_pJewelBgTexture != NULL)
					m_pJewelBgTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );

				//Jewel Render
				if(m_nAddedJewel[i] != 0)
				{
					PT_ITEM_SPEC pItemProp;
					pItemProp = g_xSpecManager->GetSpecItem( m_nAddedJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );
					}
				}

				point.x += 38;
			}
#endif
		}
#endif //__ENCHANT_BARUNA16


#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		else if( 4 == m_nAdded )
		{
			CTexture* pTexture;
			//Jewel Icon Added
			CPoint point;
			point.x = PlusRect.left + 14 + ((PlusRect.Width() - 194) / 2);
			point.y = PlusRect.bottom - 24;

			for(int i=0; i<m_nSlot; i++)
			{
				if(m_pJewelBgTexture != NULL)
					m_pJewelBgTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );

				//Jewel Render
				if(m_nAddedJewel[i] != 0)
				{
					PT_ITEM_SPEC pItemProp;
					pItemProp = g_xSpecManager->GetSpecItem( m_nAddedJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );
					}
				}

				point.x += 38;
			}
		}
#endif
	}
}

//sun: 9차 전승관련 Clienet
void CToolTip::SetUltimateToolTip(FLItemBase* pItemBase)
{
	m_nAdded = 1;
//sun: 12, 무기 피어싱
	m_nSlot = ((FLItemElem*)pItemBase)->GetUltimatePiercingSize();
	for(int i=0; i<m_nSlot; i++)
		m_nAddedJewel[i] = ((FLItemElem*)pItemBase)->GetUltimatePiercingItemID(i);
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	if( ((FLItemElem*)pItemBase)->GetProp()->IsCostumeEnhanceParts() )
	{
		m_nAdded = 4;
		int nJewel = ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + ((FLItemElem*)pItemBase)->GetProp()->dwParts - PARTS_HAT;
		for(int i=0; i<m_nSlot; i++)
		{
			if( m_nAddedJewel[i] >= DST_STR && m_nAddedJewel[i] < MAX_ADJPARAMARY )
				m_nAddedJewel[i] = nJewel;
		}
	}
#endif

	m_pUltimateItemBase = pItemBase;
}

#ifndef __IMPROVE_MAP_SYSTEM
//sun: 13, WorldMap 몬스터 표시
void CToolTip::SetWorldMapMonsterInfo(int nMonCnt, DWORD* pDwMonId)
{
	m_nMonInfoCnt = nMonCnt;
	for(int i=0; i<nMonCnt; i++)
		m_pDwMonId[i] = pDwMonId[i];

	m_rectRender.right += 18;
}

#endif // __IMPROVE_MAP_SYSTEM

#ifdef __IMPROVE_MAP_SYSTEM
//-----------------------------------------------------------------------------
void CToolTip::ResizeMapMonsterToolTip( void )
{
	m_rectRender.right += 18;
}
//-----------------------------------------------------------------------------
void CToolTip::InsertMonsterID( DWORD dwMonsterID )
{
	m_vecMapMonsterID.push_back( dwMonsterID );
}
//-----------------------------------------------------------------------------
#endif // __IMPROVE_MAP_SYSTEM

//-----------------------------------------------------------------------------
const CPoint& CToolTip::GetPointToolTip( void ) const
{
	return m_ptToolTip;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRect( void ) const
{
	return m_rect;
}
//-----------------------------------------------------------------------------
void CToolTip::SetRenderRect( const CRect& rectRender )
{
	m_rectRender = rectRender;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRenderRect( void ) const
{
	return m_rectRender;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRevisedRect( void ) const
{
	return m_nRevisedRect;
}
//-----------------------------------------------------------------------------
void CToolTip::SetSubToolTipNumber( int nSubToolTipNumber )
{
	m_nSubToolTipNumber = nSubToolTipNumber;
}
//-----------------------------------------------------------------------------
int CToolTip::GetSubToolTipNumber( void ) const
{
	return m_nSubToolTipNumber;
}
//-----------------------------------------------------------------------------
BOOL CToolTip::GetReadyToolTipSwitch( void ) const
{
	return m_bReadyToopTip;
}
//-----------------------------------------------------------------------------