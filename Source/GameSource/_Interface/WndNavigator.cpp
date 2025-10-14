
#include "StdAfx.h"
#include "WndNavigator.h"
#include "WndMapEx.h"

#include <party.h>

using namespace WndMapEx;

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndNavigator::CWndNavigator()
:	m_bObjFilterPlayer( TRUE )
,	m_bObjFilterParty( TRUE )
,	m_bObjFilterNPC( TRUE )
,	m_bObjFilterMonster( TRUE )
,	m_pDestinationPositionTexture( NULL )
,	m_pTexDungeonFog( NULL )
,	m_pTexArrow( NULL )
,	m_pTexAlpha( NULL )
,	m_pTexBlind( NULL )
,	m_iFrame( NULL )
,	m_iPastTime( NULL )
,	m_size( 0, 0 )
,	m_kSizeCircle( 166, 166 )
{
	ZeroMemory( m_szName, sizeof(TCHAR) * 64 );
	ZeroMemory( m_arrayObjVertex, sizeof( TEXTUREVERTEX ) * (MAX_DRAW_OBJECT_COUNT * VERTEXCOUNT_OF_ONE_RECTANGLE) );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_bNoCloseButton	= TRUE;
		m_bNoHelpButton		= TRUE;
	}
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndNavigator::~CWndNavigator()
{
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndNavigator::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[네비게이터] 부모 윈도우 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN( dwWndId,		FALSE, _T("[네비게이터] 생성 윈도우 아이디가 유효하지 않습니다.") );

	m_texNavPos.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"NavPosition.inc") );
	m_texNavObjs.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(),"Navigator.inc") );

	m_pTexArrow = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ImgNavArrow.bmp") ), WNDCOLOR_DEFAULT_KEY );

	ZeroMemory( &m_billboard, sizeof( m_billboard ) );
	m_billboard.rect.SetRect( 0, 0, m_pTexArrow->m_size.cx, m_pTexArrow->m_size.cy );
	m_billboard.ptCenter = CPoint( m_pTexArrow->m_size.cx / 2, m_pTexArrow->m_size.cy / 2 );
	m_billArrow.InitDeviceObjects( g_Neuz.m_pd3dDevice, &m_billboard, m_pTexArrow );
	m_billArrow.RestoreDeviceObjects();

	SetTitle( GETTEXT( TID_APP_NAVIGATOR ) );

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );	
}


//---------------------------------------------------------------------------------------------
// 메뉴 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::InitializeMenu()
{
	m_wndMenuPlace.CreateMenu( this );	

	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_PLAYER) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_PARTYTEXT) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 , prj.GetText(TID_GAME_NPC) );
	m_wndMenuPlace.AppendMenu( 0, 3 , prj.GetText(TID_GAME_MONSTER) ); 
	
	m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
	m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty );
	m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC );
	m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );

	m_wndMenuMover.CreateMenu( this );	
}


//---------------------------------------------------------------------------------------------
// 윈도우 위치를 계산하여 설정한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::CalculationWindowPosition()
{
	CRect	rectWindow( GetWindowRect() );
	CRect	rectRoot( m_pWndRoot->GetLayoutRect() );

	CPoint	point( rectRoot.right - rectWindow.Width(), rectRoot.top );

	Move( point );
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();

	CRect rectClient = GetClientRect();
	
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		rectClient.right = 13;
		m_wndPlace.Create  ( "P", 0, CRect( rectClient.left,   1, rectClient.left + 16,   1 + 16 ), this, 100000 );
		m_wndZoomIn.Create ( "+", 0, CRect( rectClient.left,  54, rectClient.left + 16,  54 + 16 ), this, 100005 );
		m_wndZoomOut.Create( "-", 0, CRect( rectClient.left,  70, rectClient.left + 16,  70 + 16 ), this, 100006 );

		m_wndPlace.SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtNavLeft.tga" ), TRUE );
		m_wndPlace.FitTextureSize();
		m_wndZoomIn.SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtNavZoomIn.tga" ), TRUE );
		m_wndZoomIn.FitTextureSize();
		m_wndZoomOut.SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtNavZoomOut.tga" ), TRUE );
		m_wndZoomOut.FitTextureSize();
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		int nSizeXY = 20;
		m_wndPlace.Create  ( "", 0, CRect( rectClient.left,   1, rectClient.left + nSizeXY,   1 + nSizeXY ), this, 100000 );
		m_wndPlace.SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtMap04.tga" ), TRUE );
		m_wndPlace.FitTextureSize();
		
		m_wndZoomIn.Create ( "", 0, CRect( rectClient.left,  54, rectClient.left + nSizeXY,  54 + nSizeXY ), this, 100005 );
		m_wndZoomIn.SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtMap01.tga" ), TRUE );
		m_wndZoomIn.FitTextureSize();
		
		m_wndZoomOut.Create( "", 0, CRect( rectClient.left,  70, rectClient.left + nSizeXY,  70 + nSizeXY ), this, 100006 );
		m_wndZoomOut.SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtMap02.tga" ), TRUE );
		m_wndZoomOut.FitTextureSize();

		m_wndNewClose.Create( "", 0, CRect( rectClient.left,  10, rectClient.left + nSizeXY, 10 + nSizeXY ), this, 100007 );
		m_wndNewClose.SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtMap05.tga" ), TRUE );
		m_wndNewClose.FitTextureSize();

		m_wndNewHelp.Create( "", 0, CRect( rectClient.left,  10, rectClient.left + nSizeXY, 10 + nSizeXY ), this, 100008 );
		m_wndNewHelp.SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtMap06.tga" ), TRUE );
		m_wndNewHelp.FitTextureSize();
	}

	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;
	FLERROR_ASSERT_LOG_RETURN_VOID( pd3dDevice, _T("[네비게이터] DirectX 장치 포인터가 유효하지 않습니다.") );
		
	m_pTexDungeonFog				= m_textureMng.AddTexture( pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("NavDunFog.tga") ), NULL, TRUE );
	m_pDestinationPositionTexture	= m_textureMng.AddTexture( pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ButtDestination.bmp") ), WNDCOLOR_DEFAULT_KEY );

	m_GuildCombatTextureMask.LoadTexture( pd3dDevice, MakePath( DIR_WORLD_GUILDCOMBAT, "WdGuildWar_Mask.dds" ), 0  );

	m_size		= CSize( 256, 256 ); //MINIMAP_SIZE, MINIMAP_SIZE );
	m_nSizeCnt	= 0;

	CalculationWindowPosition();
	InitializeMenu();
	ResizeMiniMap();
	
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CRect rectWnd( GetWndRect() );

		m_kSizeCircle.cx = 166;
		m_kSizeCircle.cy = 166;

		m_pTexAlpha = m_textureMng.AddTexture( pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("alphacircle.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_pTexBlind = m_textureMng.AddTexture( pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("map3_1.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );

		// 최적의 사이즈 160 * 148
		// D3DPOOL_DEFAULT이므로 Restore에서!
	//	m_kTexRenderTarget.CreateTexture( pd3dDevice, m_kSizeCircle.cx, m_kSizeCircle.cy, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT );
	}
}  


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 삭제 
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
HRESULT CWndNavigator::DeleteDeviceObjects()
{
	m_texNavObjs.DeleteDeviceObjects();
	m_texNavPos.DeleteDeviceObjects();
	m_billArrow.DeleteDeviceObjects();

	m_GuildCombatTextureMask.DeleteDeviceObjects();
	m_kTexRenderTarget.DeleteDeviceObjects( );
	
	return CWndNeuz::DeleteDeviceObjects();
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 무효화
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
HRESULT CWndNavigator::InvalidateDeviceObjects()
{
	m_texNavPos.InvalidateDeviceObjects();
	m_texNavObjs.InvalidateDeviceObjects();	
	m_billArrow.InvalidateDeviceObjects();

	m_GuildCombatTextureMask.Invalidate();

	m_kTexRenderTarget.DeleteDeviceObjects();

	return CWndNeuz::InvalidateDeviceObjects();
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 복구
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
HRESULT CWndNavigator::RestoreDeviceObjects()
{
	m_texNavPos.RestoreDeviceObjects( m_pApp->m_pd3dDevice );
	m_texNavObjs.RestoreDeviceObjects( m_pApp->m_pd3dDevice );	
	m_billArrow.RestoreDeviceObjects();

	m_GuildCombatTextureMask.SetInvalidate(m_pApp->m_pd3dDevice);
	
	m_kTexRenderTarget.CreateTexture( m_pApp->m_pd3dDevice, m_kSizeCircle.cx, m_kSizeCircle.cy, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT );

	ResizeMiniMap();

	return CWndNeuz::RestoreDeviceObjects();
}


//---------------------------------------------------------------------------------------------
// 지역이름 설정
// param	: 지역 이름
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::SetRegionName( TCHAR *tszName )
{
	CString strTitle = tszName;

	if( strTitle.IsEmpty() )
		strTitle = prj.GetText( TID_APP_NAVIGATOR );

	SetTitle( strTitle );
}


//---------------------------------------------------------------------------------------------
// 레인보우 레이스 시간 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::RenderRainbowRaceTime( C2DRender* p2DRender )
{
	// 레인보우 레이스 시간 출력
	DWORD dwRainbowRaceTime = CRainbowRace::GetInstance()->m_dwRemainTime;
	if(dwRainbowRaceTime > 0)
	{
		char szMsg[256] = { NULL, };
		CTimeSpan ct( (dwRainbowRaceTime -  GetTickCount()) / 1000 );
		FLSPrintf( szMsg, _countof( szMsg ), _T("%.2d:%.2d:%.2d"), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
		CRect rectWindow = GetClientRect();
		p2DRender->TextOut(rectWindow.right - 50, rectWindow.bottom - 16, szMsg, 0xffffff00 );
	}
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::OnDraw( C2DRender* p2DRender )
{
	if( NULL == p2DRender )
		return ;

	RenderRainbowRaceTime( p2DRender );
	RenderPlayerArrow( p2DRender );
}


//---------------------------------------------------------------------------------------------
// 그리는 프레임
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::PaintFrame( C2DRender* p2DRender )
{
	// 지명 출력
	if( CS_VER2 == _GetContentState( CT_NEWUI_19  ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );

		CSize kSize = CWndBase::m_Theme.m_pFontWndTitle->GetTextExtent( m_strTitle.GetString() );
		int nX = (int)( (float)m_rectWindow.Width() * 0.5f - (float)kSize.cx * 0.5f );

		p2DRender->TextOut( nX, 8, m_strTitle, m_dwColor );
		p2DRender->SetFont( pOldFont );
		return;
	}

	CWndNeuz::PaintFrame( p2DRender );
}



//---------------------------------------------------------------------------------------------
// 지형 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndNavigator::RenderLandScape( C2DRender* p2DRender )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return false;

	CWorld* pWorld( g_WorldMng() );
	if( NULL == pWorld )
		return false;

	CLandscape* pLand( pWorld->GetLandscape( pWorld->m_pCamera->m_vPos ) );
	if( pLand == NULL )
		return false;
	
	CRect	rect( GetClientRect() );
	BOOL	bOpen( prj.m_MapInformationManager.CanOpenMap( pWorld->GetID(), GetWndId(), TRUE ) );

	// 18차
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if( FALSE == bOpen )
		{
			m_pTexDungeonFog->m_size = CSize( rect.Width(), rect.Height() );
			p2DRender->RenderTexture( CPoint( 0, 0 ), m_pTexDungeonFog, 255 );
			return false;
		}
		else
		{
			BOOL			bDrawGuildWarNum( FALSE );

			DWORD			dwWorldID( pWorld->GetID() );

			CRect			rectCur( 0, 0, 0 ,0 );
			CRectClip		clipRect( 0, 0, p2DRender->m_clipRect.Width(), p2DRender->m_clipRect.Height() );

			CPoint			point( 0, 0 );
		
			int				nImageBlock( pLand->m_texMiniMap.m_size.cx );

			float			fx( (float)m_size.cx / ( MAP_SIZE * MPU ) );
			float			fy( (float)m_size.cy / ( MAP_SIZE * MPU ) );

			D3DXVECTOR3		vCenter( ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) ) );
			vCenter.x *= fx;
			vCenter.z *= fy;

			int				xCenter = (int)( vCenter.x );
			int				yCenter = (int)( -vCenter.z );

			if( dwWorldID == WI_WORLD_GUILDWAR )
				bDrawGuildWarNum = TRUE;

			for( int z = 0; z < pWorld->m_nLandHeight; z++ )
			{
				for( int x = 0; x < pWorld->m_nLandWidth; x++ )
				{
					point = CPoint ( x * nImageBlock, z * nImageBlock );
				
					point.y -= ( nImageBlock * pWorld->m_nLandHeight );
					point.x += rect.Width() / 2;
					point.y += rect.Height() / 2;
					point.x -= xCenter;
					point.y -= yCenter;
					rectCur.SetRect( point.x, point.y, point.x + nImageBlock, point.y + nImageBlock );
					CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
					if( pLand )
					{
						pLand->m_texMiniMap.m_size = m_size;
						if( clipRect.RectLapRect( rectCur ) )
						{
							if( pLand->m_texMiniMap.m_pTexture )
							{
								pLand->m_texMiniMap.Render( p2DRender, point, 255);//CWndBase::m_nAlpha );

								if( bDrawGuildWarNum )
								{
									m_GuildCombatTextureMask.m_size = pLand->m_texMiniMap.m_size;
									m_GuildCombatTextureMask.Render( p2DRender, point, 200 );
								}
							}
						} 
					}
				}
			}

			return true;
		}
	}

	// 19차
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		//_SUN_CIRCLE_MINIMAP
		//m_kTexRenderTarget 는 미리 Process타임에 만들어져있다.
		if( bOpen )
		{
			p2DRender->RenderTextureForMinimap( m_rectWindow, &m_kTexRenderTarget, m_pTexAlpha, m_kSizeCircle );
			return true;
		}
		else 
		{
			p2DRender->RenderTextureForMinimap( m_rectWindow, m_pTexBlind, m_pTexAlpha, m_kSizeCircle );
			return false;
		}
	}

	return false;
}


//---------------------------------------------------------------------------------------------
// 위치가 미니맵 원 안에 존재하는가?
// param	: 위치
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndNavigator::IsPointInNavigationCircle( const CPoint ptDestPos )
{
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		D3DXVECTOR2		vNaviCenter( (float)GetClientCenter().x, (float)GetClientCenter().y );

		float			fLength( 0.0f );
		float			fNaviRadius( (float)GetClientRect().Width() * 0.3f );
		
		//원형맵, 거리안쪽일경우만 표현한다.
		D3DXVECTOR2 vDestPos( D3DXVECTOR2( (float)ptDestPos.x, (float)ptDestPos.y ) );
		D3DXVECTOR2 vDir( vNaviCenter - vDestPos );
		fLength = D3DXVec2Length( &vDir );
		if( fLength > fNaviRadius )
			return false;
		else
			return true;

	}
	
	return true;
}



//---------------------------------------------------------------------------------------------
// 일반 오브젝트 정점 만들기
// param	: 렌더러, 가온데 위치, 퀘스트 개수 ...
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::MakeVertexNormalObject( C2DRender* p2DRender, const CPoint ptCenter, int& nQuestNum, CMover** ppQuest, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return ;

	CWorld* pWorld( g_WorldMng() );
	if( NULL == pWorld )
		return ;
	
	CLandscape*	pLand( pWorld->GetLandscape( pWorld->m_pCamera->m_vPos ) );
	if( NULL == pLand )
		return ;

	CObj*			pObj( NULL );
	CPoint			point( 0, 0 );
	CRect			rtWindow( GetClientRect() );

	int				nIndex( NULL );
	int				nPartyMapCount( NULL );
	int				nPartyMap[ MAX_PTMEMBER_SIZE ] = { NULL, };

	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			if( pObj != g_pPlayer )
			{
				CMover* pMover = (CMover*)pObj;		
				if( NULL == pMover )
					continue;

				if( g_pPlayer->m_idMurderer != pMover->m_idPlayer )	// 찍을 타겟이 날 죽인넘이면 조건없이 무조건 찍는다.
				{
					// 플레이어 필터가 아닌데, 플레이어라면 
					if( m_bObjFilterPlayer == FALSE && pMover->IsPlayer() == TRUE )
					{
						// 게다가 파티필터도 아니라면 스킵 
						if( m_bObjFilterParty == FALSE )
							continue;
						if( g_Party.IsMember( pMover->m_idPlayer ) == FALSE ) 
							continue;
					}
					if( m_bObjFilterMonster == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == FALSE ) )
						continue;
					
					if( m_bObjFilterNPC == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == TRUE ) )
					{
						// 상점이 아니라면 컨티뉴.
						LPCHARACTER lpCharacter = pMover->GetCharacter();
						if( lpCharacter == NULL || lpCharacter->m_nStructure == -1 ) 
							continue;
					}

					if( pMover->IsMode( TRANSPARENT_MODE ) ||
						pMover->IsAuthHigher( AUTH_GAMEMASTER ) )	// 대상이 투명모드이거나 GM일경우 타겟 안됨.
						continue;
				}
				
				CPoint			ptMouse( GetMousePoint() );
				D3DXVECTOR3		vPos( pObj->GetPos() );

				vPos.x			*= (float)m_size.cx / ( MAP_SIZE * MPU );
				vPos.z			*= (float)m_size.cy / ( MAP_SIZE * MPU );
				point.x			= (LONG)( ( rtWindow.Width() / 2 ) + vPos.x );
				point.y			= (LONG)( ( rtWindow.Height() / 2 ) - vPos.z );
				point.x			-= ptCenter.x;
				point.y			-= ptCenter.y;

				if( false == IsPointInNavigationCircle( point ) )
					continue;

				CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);

				if( rectHittest.PtInRect( ptMouse ) )
				{
					ClientToScreen( &ptMouse );
					ClientToScreen( &rectHittest );
					g_toolTip.PutToolTip( 10000 + ((CMover*)pObj)->GetId(), ((CMover*)pObj)->GetName( TRUE ), rectHittest, ptMouse, 0 );
				}

				int nQuestIndex( pMover->m_nQuestEmoticonIndex >> 8 );

				// 퀘스트 이모티콘 
				if( pMover->IsNPC() && pMover->m_nQuestEmoticonIndex != -1 )
					ppQuest[nQuestNum++] = pMover;
				else if( g_pPlayer->m_idMurderer && g_pPlayer->m_idMurderer == pMover->m_idPlayer )		// 찍을 타겟이 날 죽인넘이면 빨갛게 표시
					nIndex = 5;
				else if( pMover->IsNPC() && pMover->IsPeaceful() )
					nIndex = 1;
				else if( g_pPlayer->m_idGuild && g_pPlayer->m_idGuild == pMover->m_idGuild )
					nIndex = 3;
				else if( pMover->IsNPC() && pMover->IsPeaceful() == FALSE )
					nIndex = 2;
				else if( pMover->IsPlayer() )
				{
					nPartyMap[nPartyMapCount] = g_Party.FindMember( pMover->m_idPlayer );
					if( nPartyMap[nPartyMapCount] != -1 )
					{
						g_Party.m_aMember[ nPartyMap[nPartyMapCount] ].m_vPos = pMover->GetPos();
						++nPartyMapCount;
						nIndex = 4;
					}
					else
						nIndex = 0;
				}
				else
				{
					FLASSERT( NULL );
				}

				if( wCurrentObjCount >= MAX_DRAW_OBJECT_COUNT )
				{
					FLASSERT( NULL );
					continue;
				}				
					
				LPCHARACTER lpCharacter = pMover->GetCharacter();

				if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpCharacter->m_nStructure, &pVertices );
				else
				{
					// 퀘스트가 없는 것만 출력
					if( pMover->m_nQuestEmoticonIndex == -1 )
						m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), nIndex, &pVertices );
				}

				wCurrentObjCount++;
			}
		}
		END_OBJ
	}
	END_LAND
}


//---------------------------------------------------------------------------------------------
// 퀘스트 이모티콘 정점 만들기
// param	: 렌더러, 가온데 위치, 퀘스트 개수 ...
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::MakeVertexQuestEmoticon( C2DRender* p2DRender, const CPoint ptCenter, const int nQuestNum, CMover** ppQuest, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return ;

	CRect rtWindow( GetClientRect() );

	if( _GetContentState( CT_RENDER_QUESTEMOTICON ) == CS_VER2 )
	{
		for(int i = 0; i < nQuestNum; ++i )
		{
			CMover* pMover( ppQuest[i] );
			if( NULL == pMover )
				continue;

			D3DXVECTOR3	vPos( pMover->GetPos() );
			vPos.x	*= (float)m_size.cx / ( MAP_SIZE * MPU );
			vPos.z	*= (float)m_size.cy / ( MAP_SIZE * MPU );

			CPoint		point( (long)(( rtWindow.Width() / 2 ) + vPos.x), (long)( ( rtWindow.Height() / 2 ) - vPos.z ) );
			point.x -= ptCenter.x;
			point.y -= ptCenter.y;

			if( false == IsPointInNavigationCircle( point ) )
				continue;
			
			int nQuestIndex( pMover->m_nQuestEmoticonIndex >> 8 );

			QuestProp* pProp = prj.m_aPropQuest.GetAt( nQuestIndex );
			if( NULL == pProp )
				continue;

			int nQuestEmoState( pMover->m_nQuestEmoticonIndex & 0xFF );
			if( nQuestEmoState >= eState_Max )
			{
//				FLASSERT( NULL );
				continue;
			}

			CString strObjName( pMover->m_szCharacterKey );

			if( nQuestEmoState == eState_PossibleStart )
			{
				CString strQuestNPCName( pProp->m_szCharacterKey );

				if( strObjName != strQuestNPCName )
					continue;
				else
				{
					int nIndex( nQuestEmoState + 23 );	 
					int nHead( _GetHeadQuest( nQuestIndex ) );
					int nOffset( _CalcQuestEmotionOffset( nHead, eState_Complete ) );	
					nIndex += nOffset;
					
					if( wCurrentObjCount < MAX_DRAW_OBJECT_COUNT )
					{
						m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );		
						wCurrentObjCount++;
					}
					else
					{
						FLASSERT( NULL );
						return ;
					}
				}
			}
			else
			{
				CString strQuestNPCName( pProp->m_szEndCondCharacter );

				if( strObjName != strQuestNPCName )
					continue;
				else
				{
					int nIndex( nQuestEmoState + 23 );	 
					int nHead( _GetHeadQuest( nQuestIndex ) );
					int nOffset( _CalcQuestEmotionOffset( nHead, eState_Complete ) );	
					nIndex += nOffset;
					
					if( wCurrentObjCount < MAX_DRAW_OBJECT_COUNT )
					{
						m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );		
						wCurrentObjCount++;
					}
					else
					{
						FLASSERT( NULL );
						return ;
					}
				}
			}
		}
	}
}


//---------------------------------------------------------------------------------------------
// 범위 오브젝트 정점 만들기 
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::MakeVertexRegionObject( C2DRender* p2DRender, const CPoint ptCenter, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return ;

	CWorld* pWorld( g_WorldMng() );
	if( NULL == pWorld )
		return ;

	int			nSize( pWorld->m_aStructure.GetSize() );
	CRect		rtWindow( GetClientRect() );
	CPoint		point( 0, 0 );

	for( int i = 0; i < nSize; i++ )
	{
		LPREGIONELEM lpRegionElem = pWorld->m_aStructure.GetAt( i );
		if( NULL == lpRegionElem )
			continue;

		D3DXVECTOR3 vPos( lpRegionElem->m_vPos );
		vPos.x	*= (float)m_size.cx / ( MAP_SIZE * MPU );;
		vPos.z	*= (float)m_size.cy / ( MAP_SIZE * MPU );;
		point.x = (LONG)( ( rtWindow.Width() / 2 ) + vPos.x );
		point.y = (LONG)( ( rtWindow.Height() / 2 ) - vPos.z );
	
		point.x -= ptCenter.x;
		point.y -= ptCenter.y;

		if( IsPointInNavigationCircle( point ) )
		{
			if( wCurrentObjCount < MAX_DRAW_OBJECT_COUNT )
			{
				m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpRegionElem->m_dwStructure, &pVertices );
				wCurrentObjCount++;
			}
			else
			{
				FLASSERT( NULL );
				return ;
			}
		}
	}
}


//---------------------------------------------------------------------------------------------
// 선택 된 오브젝트 정점 만들기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::MakeVertexFocusObject( C2DRender* p2DRender, const CPoint ptCenter, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return ;

	CWorld* pWorld( g_WorldMng() );
	if( NULL == pWorld )
		return ;

	CObj* pObjFocus = pWorld->GetObjFocus();
	if( NULL == pObjFocus )
		return ;

	CRect			rtWindow( GetClientRect() );
	CPoint			point( 0, 0 );

	D3DXVECTOR3 vPos( pObjFocus->GetPos() );
	vPos.x *= (float)m_size.cx / ( MAP_SIZE * MPU );;
	vPos.z *= (float)m_size.cy / ( MAP_SIZE * MPU );;

	point.x = (LONG)( ( rtWindow.Width() / 2 ) + vPos.x );
	point.y = (LONG)( ( rtWindow.Height() / 2 ) - vPos.z );
	point.x -= ptCenter.x;
	point.y -= ptCenter.y;

	if( IsPointInNavigationCircle( point ) )
	{
		point.x -= 3;
		point.y -= 3;

		if( wCurrentObjCount < MAX_DRAW_OBJECT_COUNT )
		{
			m_texNavObjs.MakeVertex( p2DRender, point, 5, &pVertices );
			wCurrentObjCount++;
		}
		else
		{
			FLASSERT( NULL );
			return ;
		}
	}
}


//---------------------------------------------------------------------------------------------
// 목적지 화살표 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::RenderDestinationArrow( C2DRender* p2DRender, const CPoint ptCenter )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return ;

	D3DXVECTOR3&	rDestinationArrow( g_WndMng.m_pWndWorld->m_vDestinationArrow );

	CRect			rect( GetClientRect() );
	CPoint			point( 0, 0 );

	if( rDestinationArrow != D3DXVECTOR3( -1.0F, 0.0F, -1.0F ) )
	{
		D3DXVECTOR3 vPos( rDestinationArrow );
		vPos.x *= (float)m_size.cx / ( MAP_SIZE * MPU );
		vPos.z *= (float)m_size.cy / ( MAP_SIZE * MPU );
		point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
		point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
		point.x -= ptCenter.x;
		point.y -= ptCenter.y;

		if( IsPointInNavigationCircle( point ) )
		{
			point.x -= 3;
			point.y -= 3;

			m_pDestinationPositionTexture->RenderScal( p2DRender, point, 255, 0.4F, 0.4F );
		}
	}
}


//---------------------------------------------------------------------------------------------
// 플레이어 화살표 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndNavigator::RenderPlayerArrow( C2DRender* p2DRender )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if( NULL == pd3dDevice )
		return ;

	// 화면 비율 때문에 임의로 정사각형 뷰포트를 지정해 놓는다. 안그러면 화살표 모양이 찌그러짐.

	D3DVIEWPORT9	viewport;

	viewport.X      = p2DRender->m_clipRect.left + ( p2DRender->m_clipRect.Width()  / 2 );
	viewport.Y      = p2DRender->m_clipRect.top  + ( p2DRender->m_clipRect.Height() / 2 );
	viewport.Width  = 32;
	viewport.Height = 32;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	viewport.X      -= 16;
	viewport.Y      -= 16;

	pd3dDevice->SetViewport( &viewport );

	D3DXMATRIX		matProj;
	D3DXMATRIX		matView;
	D3DXMATRIX		matScale;
	D3DXMATRIX		matWorld;

	D3DXMatrixIdentity( &matProj );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matScale );
	D3DXMatrixIdentity( &matWorld );

	D3DXMatrixScaling( &matScale, 1.2f, 1.2f, 1.2f );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );

	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	
	D3DXVECTOR3		vDir( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );
	D3DXVECTOR3		vDestNor( g_pPlayer->GetPos() - g_Neuz.m_camera.m_vPos );
	D3DXVECTOR3		vAxis( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	D3DXVECTOR3		vYPW( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );

	D3DXQUATERNION  qRot;
	FLOAT			fTheta( 0.0f );

	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	QuaternionRotationToYPW( qRot, vYPW );
	D3DXMatrixRotationZ( &matScale, -(vYPW.y) );
	
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

	DWORD dwOldAlphaTestEnable( NULL );
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwOldAlphaTestEnable );

	// 화살표 출력 
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	m_billArrow.Render( pd3dDevice );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwOldAlphaTestEnable );
}


//---------------------------------------------------------------------------------------------
// 배경 지울 때 
// param	: 렌더러
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndNavigator::OnEraseBkgnd( C2DRender* p2DRender )
{
	if( NULL == p2DRender )
		return FALSE;

	LPDIRECT3DDEVICE9	pd3dDevice( p2DRender->m_pd3dDevice );

	if( NULL == pd3dDevice	||
		NULL == g_pPlayer	)
		return FALSE;

	CWorld* pWorld( g_WorldMng() );
	if( NULL == pWorld )
		return FALSE;

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	if( false == RenderLandScape( p2DRender ) )
		return FALSE;

	AccuFrame();	//	간단한 프레임 계산을 해준다.

	D3DXVECTOR3		vCenter( g_pPlayer->GetPos() );

	vCenter.x		*= (float)m_size.cx / ( MAP_SIZE * MPU );
	vCenter.z		*= (float)m_size.cx / ( MAP_SIZE * MPU );

	int				xCenter( static_cast<int>( vCenter.x ) );
	int				yCenter( static_cast<int>( -vCenter.z ) );

	ZeroMemory( m_arrayObjVertex, sizeof( TEXTUREVERTEX ) * (MAX_DRAW_OBJECT_COUNT * VERTEXCOUNT_OF_ONE_RECTANGLE) );

	TEXTUREVERTEX*	pVertices = m_arrayObjVertex; 
	WORD			wObjectCount = 0;

	CMover*			apQuest[ 100 ] = { NULL, };
	int				nQuestNum( NULL );

	MakeVertexNormalObject( p2DRender, CPoint( xCenter, yCenter ), nQuestNum, &apQuest[0], wObjectCount, pVertices );
	MakeVertexRegionObject( p2DRender, CPoint( xCenter, yCenter ), wObjectCount, pVertices );
	MakeVertexFocusObject( p2DRender, CPoint( xCenter, yCenter ), wObjectCount, pVertices );
	MakeVertexQuestEmoticon( p2DRender, CPoint( xCenter, yCenter ), nQuestNum, &apQuest[0], wObjectCount, pVertices );

	// 등록 된 모든 오브젝트 그리기
	m_texNavObjs.Render( m_pApp->m_pd3dDevice, m_arrayObjVertex, wObjectCount * VERTEXCOUNT_OF_ONE_RECTANGLE );

	RenderDestinationArrow( p2DRender, CPoint( xCenter, yCenter ) );
	RenderMarkAll( p2DRender, g_pPlayer );

	m_wndPlace.EnableWindow( !pWorld->m_bIsIndoor );

	return TRUE;
}



void CWndNavigator::RenderMark( C2DRender* p2DRender, CMover* Player )
{
	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	if ( Player->m_nvPoint.Time != 0 )
	{	
		D3DXVECTOR3 Pos = g_pPlayer->GetPos() - Player->m_nvPoint.Pos;
		Pos.x *= fx ;
		Pos.z *= -fy;
		Player->m_nvPoint.Time--;

		CPoint point;
		CRect rect = GetClientRect();
		point.x = (LONG)( ( rect.Width() / 2 ) - ( Pos.x + 2 ) );
		point.y = (LONG)( ( rect.Height() / 2 ) - ( Pos.z + 2 ) );
		
		//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
		m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

		CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
		CPoint ptMouse = GetMousePoint();
		if( rectHit.PtInRect( ptMouse ) )
		{
//			CString toolTip("Mark : ");
			CString toolTip		= prj.GetText( TID_GAME_NAV_MARK );
			toolTip += Player->GetName( TRUE );
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHit );
			g_toolTip.PutToolTip( 10000 + Player->GetId(), toolTip, rectHit, ptMouse, 0 );
		}
	}
}

//	이걸 넣어 준다음에 정리를 위해서 NaviPoint형을 참조나 포인터로 받는 형태로 다시 만들어 주어야 할것이다.
void CWndNavigator::RenderMarkAll( C2DRender* p2DRender , CMover * Player )
{
	RenderMark( p2DRender, Player );

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	V_NaviPoint::iterator nvi = g_pPlayer->m_vOtherPoint.begin();
	for ( int i = 0 ; i < (int)( g_pPlayer->m_vOtherPoint.size() ) ; )
	{
		if ( g_pPlayer->m_vOtherPoint.at( i ).Time != 0 )
		{
			D3DXVECTOR3 Pos = g_pPlayer->GetPos() - g_pPlayer->m_vOtherPoint.at( i ).Pos;
			Pos.x *= fx ;
			Pos.z *= -fy;
			g_pPlayer->m_vOtherPoint.at( i ).Time--;

			CPoint point;
			CRect rect = GetClientRect();
			point.x = (LONG)( ( rect.Width() / 2 ) - ( Pos.x + 2 ) );
			point.y = (LONG)( ( rect.Height() / 2 ) - ( Pos.z + 2 ) );
			
			//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
			m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

			CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
			CPoint ptMouse = GetMousePoint();
			if( rectHit.PtInRect( ptMouse ) )
			{
//				CString toolTip("Mark : ");
				CString toolTip		= prj.GetText( TID_GAME_NAV_MARK );
				toolTip += g_pPlayer->m_vOtherPoint.at( i ).Name.c_str();
				ClientToScreen( &ptMouse );
				ClientToScreen( &rectHit );
				g_toolTip.PutToolTip( 10000 + g_pPlayer->m_vOtherPoint.at( i ).objid, toolTip, rectHit, ptMouse, 0 );
			}
			nvi++;
			i++;
		}
		else
		{
			g_pPlayer->m_vOtherPoint.erase(nvi);
		}
		
	}
}





void CWndNavigator::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
		}
		else
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
			ar >> m_size.cx >> m_size.cy;
		}
		m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
		m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
		m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
		m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );
	}
	else
	{
		dwVersion = 1;
		ar << m_bObjFilterPlayer << m_bObjFilterParty << m_bObjFilterNPC << m_bObjFilterMonster;
		ar << m_size.cx << m_size.cy;
	}
}

BOOL CWndNavigator::UpdateRenderTarget( )
{
	//CRect rectWnd = GetWndRect( );
	//rectWnd.right = rectWnd.left + 256;
	//rectWnd.bottom = rectWnd.top + 256;
	//SetWndRect( rectWnd, TRUE );

	//gmpbigsun: 미리설정한 RenderTarget Texture에 4가지 미니맵을 조합한 결과를 랜더링 해놓는다.
	//추후 Draw타임에 조합한결과를 알파맵과 조합해서 그려줌.

	CWorld* pWorld	= g_WorldMng();
	if( !pWorld )
		return FALSE;

	C2DRender* p2DRender = &g_Neuz.m_2DRender;
	if( !p2DRender )
		return FALSE;

//	SIZE size;
//	size.cx = 256;
//	size.cy = 256;

	CRect rect = GetClientRect();
	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;
	
	D3DVIEWPORT9 viewportOld;
	pd3dDevice->GetViewport(&viewportOld);

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = m_kSizeCircle.cx;//rect.Width();//.cx;
	viewport.Height = m_kSizeCircle.cy;//rect.Height();//size.cy;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	D3DDeviceCombo* pDeviceCombo = g_Neuz.m_d3dSettings.PDeviceCombo();
	HRESULT hr = g_Neuz.m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      pDeviceCombo->AdapterFormat,
                                      D3DUSAGE_RENDERTARGET,
                                      D3DRTYPE_TEXTURE,
                                      D3DFMT_A8R8G8B8 );
	if( hr != D3D_OK )
		return FALSE;

	IDirect3DSurface9* pOldSurface = NULL;
	IDirect3DSurface9* pDstSurface = NULL;

	//랜더타겟을 텍스쳐와 연결된 임의의 surface로 바꾼다.
	pd3dDevice->GetRenderTarget( 0, &pOldSurface );
	m_kTexRenderTarget.m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
	pd3dDevice->SetRenderTarget( 0, pDstSurface );//, pOldSurfaceZ );

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000/*CWorld::m_dwBgColor*/, 1.0f, 0);
	if( pd3dDevice->BeginScene() == S_OK )
	{
		//아래 4가지 이미지를 조합(각자 그리는) 하는건 기존코드를 차용함.
		CLandscape* pLand = pWorld->GetLandscape( pWorld->m_pCamera->m_vPos );
		if( pLand == NULL ) 
			return FALSE;

		int nImageBlock = pLand->m_texMiniMap.m_size.cx;
		
		CRectClip clipRect( 0, 0, m_rectWindow.Width(), m_rectWindow.Height() ); //p2DRender->m_clipRect.Width(), p2DRender->m_clipRect.Height() );
		CPoint point;
		CRect rectCur;

		FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
		FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );
		D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
		vCenter.x *= fx;
		vCenter.z *= fy;
		int xCenter = (int)( vCenter.x );
		int yCenter = (int)( -vCenter.z );

		DWORD dwWorldID = pWorld->GetID();

		for( int z = 0; z < pWorld->m_nLandHeight; z++ )
		{
			for( int x = 0; x < pWorld->m_nLandWidth; x++ )
			{
				point = CPoint ( x * nImageBlock, z * nImageBlock );
			
				point.y -= ( nImageBlock * pWorld->m_nLandHeight );
				point.x += rect.Width() / 2;
				point.y += rect.Height() / 2;
				point.x -= xCenter;
				point.y -= yCenter;
				rectCur.SetRect( point.x, point.y, point.x + nImageBlock, point.y + nImageBlock );
				CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
				if( NULL == pLand )
					continue;

				pLand->m_texMiniMap.m_size = m_size;
				if( clipRect.RectLapRect( rectCur ) )
				{
					if( pLand->m_texMiniMap.m_pTexture )
						pLand->m_texMiniMap.Render( p2DRender, point, 255);//CWndBase::m_nAlpha );
				} 

			}

		}
		
		pd3dDevice->EndScene();
	}  //end of if

	pd3dDevice->SetRenderTarget( 0, pOldSurface );//, pOldSurfaceZ );
	pOldSurface->Release();
	pDstSurface->Release();
	pd3dDevice->SetViewport(&viewportOld);

	return TRUE;
}


void CWndNavigator::ResizeMiniMap()
{
	CWorld* pWorld	= g_WorldMng();
	if( pWorld == NULL )
		return;
	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
				pLand->m_texMiniMap.m_size = m_size;
		}
	}
}
BOOL CWndNavigator::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CRect rect = GetWindowRect( TRUE );
	//int nImageBlock = pLand->m_texMiniMap.m_size.cx;

	CWndBase* pWndBase = (CWndBase*) pLResult;

	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
	//	g_WndMng.m_nObjectFilter = nID;
		//m_wndMenuPlace.CheckMenuRadioItem( 0, 2, nID, TRUE );
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		SetFocus();
		switch( nID )
		{
		case eObjectFilter::OBJFILTER_PLAYER:
			m_bObjFilterPlayer = nState;
			break;
		case eObjectFilter::OBJFILTER_PARTY:
			m_bObjFilterParty = nState;
			break;
		case eObjectFilter::OBJFILTER_NPC:
			m_bObjFilterNPC = nState;
			break;
		case eObjectFilter::OBJFILTER_MONSTER:
			m_bObjFilterMonster = nState;
			break;
		}
	}
	else
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case 100000: // 장소 찾기
				{
					CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
					int nMenuPlaceLeft = rect.left - m_wndMenuPlace.GetWindowRect().Width();
					if( nMenuPlaceLeft < rectRootLayout.left )
						m_wndMenuPlace.Move( CPoint( rect.right, rect.top ) );
					else
						m_wndMenuPlace.Move( CPoint( nMenuPlaceLeft, rect.top ) );
					m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
					m_wndMenuPlace.SetFocus();
				}
				break;
			case 100001: // 장소 찾기
				{
					m_wndMenuMover.DeleteAllMenu();
					CWorld* pWorld	= g_WorldMng();
					CLandscape* pLand;
					CObj* pObj;
					CWndButton* pWndButton;
					int i = 0;

					FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
					{
						FOR_OBJ( pLand, pObj, OT_MOVER )
						{
							pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
							pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_OBJECT;
						}
						END_OBJ
					}
					END_LAND
					m_wndMenuMover.Move( CPoint( rect.left - m_wndMenuMover.GetWindowRect().Width(), rect.top ) );
					m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
					m_wndMenuMover.SetFocus();
				}
				break;
			case 100005: // zoom in
				m_nSizeCnt = 1;
				m_size.cx += 32;
				m_size.cy += 32;
				if(m_size.cx > 352)
					m_size.cx = 352;
				if(m_size.cy > 352 )
					m_size.cy = 352;
				/*
				if(m_size.cx > 1024)
					m_size.cx = 1024;
				if(m_size.cy > 1024 )
					m_size.cy = 1024;
					*/
				ResizeMiniMap();
				break;
			case 100006: // zoom out 
				m_nSizeCnt = -1;
				m_size.cx -= 32;
				m_size.cy -= 32;
				if( m_size.cx < 128 )
					m_size.cx = 128;
				if( m_size.cy < 128 )
					m_size.cy = 128;
				ResizeMiniMap();
				break;

			case 100007:
				Destroy();
				break;

			case 100008:
			{
#ifdef __CLIENT					
					LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( GetWndId() );
					if(g_WndMng.m_pWndHelpInstant)
						SAFE_DELETE(g_WndMng.m_pWndHelpInstant);

					g_WndMng.m_pWndHelpInstant = new CWndHelpInstant;
					g_WndMng.m_pWndHelpInstant->m_strHelpKey = lpWndApplet->strToolTip;
					g_WndMng.m_pWndHelpInstant->Initialize();
#endif
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndNavigator::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL ) 
		return;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CRect rect = GetClientRect();
		CPoint pt = point;
		
		pt.x -= ( rect.Width() / 2 );
		pt.y -= ( rect.Height() / 2 );
		
		FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );//(FLOAT)m_size.cx / (FLOAT)MINIMAP_SIZE;// * 2;
		FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );//(FLOAT)m_size.cy / (FLOAT)MINIMAP_SIZE;// * 2;
		
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		vPos.x += ( pt.x / fx );
		vPos.z -= ( pt.y / fy );

		CString string;
		string.Format( "/teleport %d %f %f", g_WorldMng()->m_dwWorldID, vPos.x, vPos.z );
		g_DPlay.SendChat( string );
	}
}

BOOL CWndNavigator::Process ()
{
	if( CS_VER2 == _GetContentState( CT_NEWUI_19  ) )
		UpdateRenderTarget( );

	return TRUE;
}

BOOL CWndNavigator::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	if( pWndBase == &m_wndMenuMover )
	{
		if( nID >= 0 && nID < 100000 )
		{
			CMover* pMover = prj.GetMover( nID );
			g_WorldMng()->SetObjFocus( pMover );
			SetFocus();
			g_WndMng.m_pWndWorld->m_vDestinationArrow = pMover->m_vPos;
		}
	}
	/*
	CRect rect = GetWindowRect( TRUE );
	
	switch(nID)
	{
		case 100: // wnd1 
			m_wndMenu.Move( CPoint( rect.right, rect.top ) );
			m_wndMenu.SetVisible( !m_wndMenu.IsVisible() );
			m_wndMenu.SetFocus();
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}



void CWndNavigator::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		//Correct the size
		rectWnd.left = rectWnd.right - 176;
		rectWnd.bottom = rectWnd.top + 208;
	}

	AdjustMinRect( &rectWnd, 16 * 6, 16 * 7 );
	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 13 );
//	m_kSizeCircle.cx = rectWnd.Width();
//	m_kSizeCircle.cy = rectWnd.Height();
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndNavigator::OnSize(UINT nType, int cx, int cy)
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_wndPlace.Move( CPoint( 0, 0 ) );
		m_wndZoomIn.Move( CPoint(0, cy - 32 ) );
		m_wndZoomOut.Move( CPoint(0, cy - 16 ) );
	}
	else if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		DelWndStyle( WBS_THICKFRAME );

		m_wndPlace.Move( CPoint( 22, 15 ) );
		m_wndZoomIn.Move( CPoint( 2, 105 ) );
		m_wndZoomOut.Move( CPoint( 10, 120 ) );

		m_wndNewClose.Move( CPoint( 125, 15 ) );
		m_wndNewHelp.Move( CPoint( 140, 30 ) );
	}
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndNavigator::OnRButtonDown(UINT nFlags, CPoint point)
{				
	CWorld* pWorld = g_WorldMng();
	pWorld->SetObjFocus(  NULL );
	g_pPlayer->ClearDest();

	m_wndMenuMover.DeleteAllMenu();
	CLandscape* pLand;
	CObj* pObj;
	CWndButton* pWndButton;
	int i = 0;

	CMover* pMover = NULL;
	int nTarget = 0;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			pMover = ( CMover* )pObj;
			if( !pMover->IsPlayer( ) && pMover->GetCharacter( ) )		//NPC인경우만 
			{
				//gmpbigsun( 20100917, #0009821 ) : npc중 pet금지
				MoverProp* pProp = pMover->GetProp();
				if( pProp->dwUseable != 1 ) //펫 및 몬스터는 1
				{
					pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
					pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_OBJECT;
					++nTarget;
				}
			}
		}
		END_OBJ
	}
	END_LAND

	if( nTarget > 0 )
	{
		CRect rect = GetWindowRect( TRUE );
		CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
		int nMenuMoverLeft = rect.left - m_wndMenuMover.GetWindowRect().Width();
		if( nMenuMoverLeft < rectRootLayout.left )
			m_wndMenuMover.Move( CPoint( rect.right, rect.top ) );
		else
			m_wndMenuMover.Move( CPoint( nMenuMoverLeft, rect.top ) );
		m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
		m_wndMenuMover.SetFocus();
	}
}

void CWndNavigator::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetClientRect();

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		//사각형 -> 원 
		CPoint kCenter = rect.CenterPoint();
		D3DXVECTOR3 kPosA( (float)kCenter.x, 0.0f, (float)kCenter.y );
		D3DXVECTOR3 kPosB( (float)point.x, 0.0f, (float)point.y );
		D3DXVECTOR3 kDiff = kPosB - kPosA;
		float fLen = D3DXVec3Length( &kDiff );

		if( fLen > 65.0f )
			return;
	}
	
	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x
	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	CWorld* pWorld	= g_WorldMng();

	g_pPlayer->m_nvPoint.Pos.x = vCenter.x + ( (float)( point.x - (rect.right / 2) ) / fx );
	g_pPlayer->m_nvPoint.Pos.z = vCenter.z - ( (float)( point.y - (rect.bottom / 2) ) / fy );
	//	이 시간은 네비게이터에 남아 있는 시간으로 디파인이나 파일로 뺄수도 있을듯하지만 우선 하드 코딩이닷
	g_pPlayer->m_nvPoint.Time = 200;	
	if( pWorld )
	{
		CObj* pObj	= pWorld->GetObjFocus();
		if( IsValidObj( pObj ) && pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() )
		{
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, ( (CCtrl*)pObj )->GetId() );
		}
		else
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, NULL_ID );
		}
	}

	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = (int)( vCenter.x );
	int yCenter = (int)( - vCenter.z );
	
	CObj* pObj;
	CLandscape* pLand;
//	CWorld* pWorld	= g_WorldMng();
	CMover *pMover;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				pMover = (CMover *)pObj;
				if( pMover->IsMode( TRANSPARENT_MODE ) == FALSE )
				{
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					int x = (int)( ( rect.Width() / 2 ) + vPos.x );
					int y = (int)( ( rect.Height() / 2 ) - vPos.z );
					x -= xCenter + 2;
					y -= yCenter + 2;
					CRect rectHittest( x, y, x + 5, y + 5);
					if( rectHittest.PtInRect( point ) )
					{
						if( CMover::GetActiveMover() != pObj )
							pWorld->SetObjFocus( pObj );
						return;
					}
				}
			}
		}
		END_OBJ
	}
	END_LAND

}
