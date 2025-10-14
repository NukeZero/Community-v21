#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "WndMapEx.h"
#include "MapComboBoxData.h"
#include "party.h"
#include "playerdata.h"
#include "Continent.h"
#include "defineMapComboBoxData.h"
extern CDPClient g_DPlay;
//-----------------------------------------------------------------------------
const FLOAT CWndMapEx::SOURCE_MAP_SIZE_X( 1280.0F );
const FLOAT CWndMapEx::SOURCE_MAP_SIZE_Y( 960.0F );
const FLOAT CWndMapEx::ANIMATION_SPEED( 800.0F );
const FLOAT CWndMapEx::PC_ARROW_POSITION_TEXTURE_SIZE_RATIO( 1.0F );
const FLOAT CWndMapEx::TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO( 1.4F );
const FLOAT CWndMapEx::EXTEND_RATIO( 0.4F );
const FLOAT CWndMapEx::EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO( TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO + EXTEND_RATIO );
const FLOAT CWndMapEx::DESTINATION_POSITION_TEXTURE_SIZE_RATIO( 0.6F );
const FLOAT CWndMapEx::NPC_POSITION_TEXTURE_SIZE_RATIO( 1.6F );
const FLOAT CWndMapEx::USER_MARK_POSITION_TEXTURE_SIZE_RATIO( 1.0F );
const FLOAT CWndMapEx::HYPERLINK_MARK_POSITION_TEXTURE_SIZE_RATIO( 1.0F );
const int CWndMapEx::WINDOW_TILE_TEXTURE_SIZE_XY( 16 );
const int CWndMapEx::MINIMUM_WINDOW_TILE_NUMBER_X( 34 );
const int CWndMapEx::MINIMUM_WINDOW_TILE_NUMBER_Y( 27 );
//-----------------------------------------------------------------------------
CWndMapEx::CWndMapEx( void ) : 
m_eConstructionMode( NORMAL ), 
m_pPCArrowTexture( NULL ), 
m_pPartyPCArrowTexture( NULL ), 
m_pMapTexture( NULL ), 
m_pTeleportationPositionTexture( NULL ), 
m_pDestinationPositionTexture( NULL ), 
m_pNPCPositionTexture( NULL ), 
m_pUserMarkPositionTexture( NULL ), 
m_pHyperlinkMarkPositionTexture( NULL ), 
m_fRevisedMapSizeRatio( 1.0F ), 
m_rectRevisedMapPosition( 0, 0, 0, 0 ), 
m_dwSelectedMapID( MCD_NONE ), 
m_bySelectedMapLocationID( CONT_NODATA ), 
m_bShowMonsterInformation( FALSE ), 
m_bMonsterInformationToolTip( FALSE ), 
m_nSelectedMonsterIconIndex( -1 ), 
m_dwSelectedUserMarkID( 0 ), 
m_nIconTextureAlpha( MINIMUM_ICON_TEXTURE_ALPHA ), 
m_tmOld( g_tmCurrent ), 
m_bAlphaSwitch( FALSE ), 
m_idTeleporter( NULL_ID ), 
m_fNPCPositionX( -1.0F ), 
m_fNPCPositionY( -1.0F ), 
m_strHyperlinkMarkTitle( _T( "" ) ), 
m_fHyperlinkMarkPositionX( -1.0F ), 
m_fHyperlinkMarkPositionY( -1.0F ), 
m_bMapComboBoxInitialization( FALSE ), 
m_byTransparentStateAlpha( NORMAL_STATE_ALPHA ), 
m_pWndUserMarkNameChanger( NULL ), 
m_nMinimumWindowTileWidth( WINDOW_TILE_TEXTURE_SIZE_XY * MINIMUM_WINDOW_TILE_NUMBER_X ), 
m_nMinimumWindowTileHeight( WINDOW_TILE_TEXTURE_SIZE_XY * MINIMUM_WINDOW_TILE_NUMBER_Y ), 
m_nMaximumWindowTileWidth( WINDOW_TILE_TEXTURE_SIZE_XY * ( MINIMUM_WINDOW_TILE_NUMBER_X + 1 ) ), 
m_nMaximumWindowTileHeight( WINDOW_TILE_TEXTURE_SIZE_XY * ( MINIMUM_WINDOW_TILE_NUMBER_Y + 1 ) ), 
m_nWndTaskBarHeight( 0 )
{
	m_bNoCloseButton = TRUE;
}
//-----------------------------------------------------------------------------
CWndMapEx::~CWndMapEx( void )
{
	SAFE_DELETE( m_pWndUserMarkNameChanger );

	m_WndMenuUserMark.DeleteAllMenu();

	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_vecTeleportationPositionRect.clear();
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MAP_EX, 0, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	m_pPCArrowTexture = prj.m_MapInformationManager.GetPCArrowTexture();
	m_pPartyPCArrowTexture = prj.m_MapInformationManager.GetPartyPCArrowTexture();
	m_pTeleportationPositionTexture = prj.m_MapInformationManager.GetTeleportationPositionTexture();
	m_pDestinationPositionTexture = prj.m_MapInformationManager.GetDestinationPositionTexture();
	m_pNPCPositionTexture = prj.m_MapInformationManager.GetNPCPositionTexture();
	m_pUserMarkPositionTexture = prj.m_MapInformationManager.GetUserMarkPositionTexture();
	m_pHyperlinkMarkPositionTexture = prj.m_MapInformationManager.GetHyperlinkMarkPositionTexture();

	CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
	if( pWndComboBoxMapCategory )
	{
		pWndComboBoxMapCategory->AddWndStyle( EBS_READONLY );

		int nIndex = 0;
		MapComboBoxDataVector* pMapCategoryVector = prj.m_MapInformationManager.GetMapCategoryVector();
		if( pMapCategoryVector )
		{
			for( MapComboBoxDataVector::iterator Iterator = pMapCategoryVector->begin(); Iterator != pMapCategoryVector->end(); ++Iterator )
			{
				CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
				if( pMapComboBoxData == NULL )
				{
					continue;
				}
				pWndComboBoxMapCategory->AddString( pMapComboBoxData->GetTitle() );
				pWndComboBoxMapCategory->SetItemData( nIndex, pMapComboBoxData->GetID() );
				++nIndex;
			}
		}
	}

	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
	if( pWndComboBoxMapName )
	{
		pWndComboBoxMapName->AddWndStyle( EBS_READONLY );
	}

	CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	if( pWndComboBoxNpcName )
	{
		pWndComboBoxNpcName->AddWndStyle( EBS_READONLY );
	}

	m_WndMenuUserMark.CreateMenu( this );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_DELETE , prj.GetText( TID_GAME_MAP_EX_MARK_DELETE ) );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_DELETE_ALL , prj.GetText( TID_GAME_MAP_EX_MARK_DELETE_ALL ) );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW , prj.GetText( TID_GAME_MAP_EX_MARK_INSERT_CHATTING_WINDOW ) );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_CHANGE_NAME , prj.GetText( TID_GAME_MAP_EX_MARK_CHANGE_NAME ) );

	CalculateMaximumWindowTileLength();

	//scan quest data
	ScanQuest( );

	//ready to render quest mark
	m_kTexPackQuest.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(),"Navigator.inc") );
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::Process( void )
{
	ProcessMapComboBoxInitialization();
	ProcessMapSizeInformation();
	ProcessMonsterInformationToolTip();
	ProcessUserMarkToolTip();
	ProcessHyperlinkMarkToolTip();
	ProcessIconTextureAlpha();
	ProcessVisualSetting();

	return CWndBase::Process();
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnDraw( C2DRender* p2DRender )
{
	if( _GetContentState( CT_BLIND_WNDMAP ) == CS_VER1 )
	{
		BOOL bOpen = prj.m_MapInformationManager.CanOpenMap( m_dwSelectedMapID, GetWndId() );

		if( FALSE == bOpen )
		{
			m_pMapTexture =	CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "WndMapBlind.dds" ) ), 0xffff00ff );
			
			RenderBackground( p2DRender );
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWorld );

			LPCTSTR szTemp = prj.GetText( TID_MMI_CASHMAP_MAPCLOSE01); //"암흑의 기운으로 인해 해당지역의 정보를 볼수없습니다";
			CRect rect = GetWndRect();
			CSize sizeText = p2DRender->GetFont()->GetTextExtent( szTemp );
			p2DRender->TextOut( (rect.Width() - sizeText.cx)/2, rect.Height()/2, szTemp, 0xffff1111 );
			p2DRender->SetFont( pOldFont );

			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
				if( pWndComboBoxNpcName )
					pWndComboBoxNpcName->DrawBackground( p2DRender );

				CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
				if( pWndComboBoxMapCategory )	
					pWndComboBoxMapCategory->DrawBackground( p2DRender );

				CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
				if( pWndComboBoxMapName )
					pWndComboBoxMapName->DrawBackground( p2DRender );

			}
			return;
		}
	}
	
	RenderBackground( p2DRender );
	RenderTeleportationPosition( p2DRender );
	RenderDestinationPosition( p2DRender );
	RenderNPCPosition( p2DRender );
	RenderUserMarkPosition( p2DRender );
	RenderHyperlinkMarkPosition( p2DRender );
	RenderPlayerAndPartyPlayerPosition( p2DRender );
	RenderMapMonsterInformation( p2DRender );
	RenderRainbowNPCInformation( p2DRender );
	RenderQuestEmoticon( p2DRender );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
		if( pWndComboBoxNpcName )
			pWndComboBoxNpcName->DrawBackground( p2DRender );

		CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
		if( pWndComboBoxMapCategory )	
			pWndComboBoxMapCategory->DrawBackground( p2DRender );

		CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
		if( pWndComboBoxMapName )
			pWndComboBoxMapName->DrawBackground( p2DRender );

	}
}

//-----------------------------------------------------------------------------
void CWndMapEx::PaintFrame( C2DRender* p2DRender )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		return;
	}

	if( m_pTexture == NULL )
	{
		return;
	}

	RenderWnd();

	if( IsWndStyle( WBS_CAPTION ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, 4, m_strTitle, ( ( m_dwColor & 0x00ffffff ) | ( m_nAlphaCount << 24 ) ) );
		p2DRender->SetFont( pOldFont );
	}
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( &m_WndMenuUserMark == pWndBase )
	{
		switch( nID )
		{
		case ID_USER_MARK_MENU_DELETE:
			{
				prj.m_MapInformationManager.DeleteUserMarkPositionInfo( m_dwSelectedUserMarkID );
				m_dwSelectedUserMarkID = 0;
				m_WndMenuUserMark.SetVisible( FALSE );

				break;
			}
		case ID_USER_MARK_MENU_DELETE_ALL:
			{
				prj.m_MapInformationManager.DeleteAllUserMarkPositionInfo();
				m_dwSelectedUserMarkID = 0;
				m_WndMenuUserMark.SetVisible( FALSE );

				break;
			}
		case ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW:
			{
				CWndChat* pWndChat = (CWndChat*)g_WndMng.CreateApplet( APP_COMMUNICATION_CHAT );
				if( pWndChat == NULL )
				{
					break;
				}

				CUserMarkPositionInfo* pUserMarkPositionInfo = prj.m_MapInformationManager.FindUserMarkPositionInfo( m_dwSelectedUserMarkID );
				if( pUserMarkPositionInfo == NULL )
				{
					break;
				}

				CString strUserMarkText = _T( "" );
				strUserMarkText.Format( 
					_T( "[%s:%d,%d]" ), 
					pUserMarkPositionInfo->GetName(), 
					static_cast< int >( pUserMarkPositionInfo->GetPositionX() ), 
					static_cast< int >( pUserMarkPositionInfo->GetPositionY() ) );
				pUserMarkPositionInfo->GetPositionX();

				pWndChat->m_wndEdit.AddString( strUserMarkText, 0xff0505ff );
				//pWndChat->m_wndEdit.SetString( strUserMarkText, 0xff0505ff );
				pWndChat->m_wndEdit.SetFocus();

				m_dwSelectedUserMarkID = 0;
				m_WndMenuUserMark.SetVisible( FALSE );

				return -1;
			}
		case ID_USER_MARK_MENU_CHANGE_NAME:
			{
				CUserMarkPositionInfo* pUserMarkPositionInfo = prj.m_MapInformationManager.FindUserMarkPositionInfo( m_dwSelectedUserMarkID );
				if( pUserMarkPositionInfo == NULL )
				{
					break;
				}
				SAFE_DELETE( m_pWndUserMarkNameChanger );
				m_pWndUserMarkNameChanger = new CWndUserMarkNameChanger;
				m_pWndUserMarkNameChanger->Initialize();
				m_pWndUserMarkNameChanger->SetInfo( pUserMarkPositionInfo->GetID(), pUserMarkPositionInfo->GetName() );

				m_dwSelectedUserMarkID = 0;
				m_WndMenuUserMark.SetVisible( FALSE );

				return -1;
			}
		}
	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( message )
	{
	case WNM_SELCHANGE:
		{
			switch( nID )
			{
			case WIDC_COMBOBOX_MAP_CATEGORY:
				{
					CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )pLResult;
					if( pWndComboBoxMapCategory == NULL )
					{
						break;
					}
					DWORD dwSelectedMapCategoryItemData = pWndComboBoxMapCategory->GetSelectedItemData();
					RearrangeComboBoxData( dwSelectedMapCategoryItemData, WIDC_COMBOBOX_MAP_NAME, prj.m_MapInformationManager.GetMapNameVector() );

					CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
					if( pWndComboBoxMapName == NULL )
					{
						break;
					}
					DWORD dwSelectedMapNameItemData = pWndComboBoxMapName->GetSelectedItemData();
					RearrangeComboBoxData( dwSelectedMapNameItemData, WIDC_COMBOBOX_NPC_NAME, prj.m_MapInformationManager.GetNPCNameVector() );

					break;
				}
			case WIDC_COMBOBOX_MAP_NAME:
				{
					CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )pLResult;
					if( pWndComboBoxMapName == NULL )
					{
						break;
					}
					DWORD dwSelectedMapNameItemData = pWndComboBoxMapName->GetSelectedItemData();
					RearrangeComboBoxData( dwSelectedMapNameItemData, WIDC_COMBOBOX_NPC_NAME, prj.m_MapInformationManager.GetNPCNameVector() );

					break;
				}
			}

			ResetMapInformation();
			ResetNPCPosition();

			break;
		}
	case WNM_CLICKED:
		{
			switch( nID )
			{
			case WIDC_BUTTON_MONSTER_INFO:
				{
					m_bShowMonsterInformation = !m_bShowMonsterInformation;

					//gmpbigsun( 20100823 ) : 몬스터 버튼 토글상태에따라 버튼상태 바꿈
					CWndButton* pBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
					if( pBtn )
					{
						if(m_bShowMonsterInformation) 
							pBtn->SetCheck( 2 );
						else 
							pBtn->SetCheck( 0 );
					}

					break;
				}
			}

			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndMapEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, m_nMinimumWindowTileWidth, m_nMinimumWindowTileHeight );
	AdjustMaxRect( &rectWnd, m_nMaximumWindowTileWidth, m_nMaximumWindowTileHeight );

	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->OnLButtonUp( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
	else
	{
		m_dwSelectedUserMarkID = 0;
		m_WndMenuUserMark.SetVisible( FALSE );

		//gmpbigsun: 우클릭으로 변경
		//if( g_WndMng.m_pWndWorld->m_bCtrlPushed == TRUE )
		//{
		//	D3DXVECTOR3 vReconvertedPosition( 0.0F, 0.0F, 0.0F );
		//	ReconvertPosition( 
		//		vReconvertedPosition, 
		//		D3DXVECTOR3( static_cast< FLOAT >( point.x ), 0.0F, static_cast< FLOAT >( point.y ) ), 
		//		m_bySelectedMapLocationID );
		//	if( IsPositionInSelectedMapArea( vReconvertedPosition ) == TRUE )
		//	{
		//		prj.m_MapInformationManager.InsertUserMarkPositionInfo( prj.GetText( TID_GAME_MAP_EX_MARK_INITIAL_TITLE_NAME ), vReconvertedPosition.x, vReconvertedPosition.z );
		//	}
		//	else
		//	{
		//		g_WndMng.PutString( prj.GetText( TID_ERROR_MAP_EX_MARK_INVALID_POSITION ), NULL, prj.GetTextColor( TID_ERROR_MAP_EX_MARK_INVALID_POSITION ) );
		//	}
		//	return;
		//}

		if( m_eConstructionMode == TELEPORTATION )
		{
			for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
			{
				TeleportationIconInfo* pTeleportationIconInfo = ( TeleportationIconInfo* )( *Iterator );
				if( pTeleportationIconInfo == NULL )
				{
					continue;
				}

				const D3DXVECTOR3 vTeleportationIconPosition( 
					static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().x ), 
					0.0F, 
					static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().y ) );
				if( IsPositionInSelectedMapArea( vTeleportationIconPosition ) == FALSE )
				{
					continue;
				}

				D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
				ConvertPosition( vConvertedPosition, vTeleportationIconPosition, m_bySelectedMapLocationID );

				CRect rectTeleportationPositionIcon( 0, 0, 0, 0 );
				CalculateMapIconRectFromPoint( 
					rectTeleportationPositionIcon, 
					vConvertedPosition.x, 
					vConvertedPosition.z, 
					m_pTeleportationPositionTexture, 
					TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO );
				if( rectTeleportationPositionIcon.PtInRect( point ) == TRUE )
				{
					CMover* pMover = prj.GetMover( m_idTeleporter );
					if( pMover == NULL )
					{
						return;
					}

					PLAYSND( SND_INF_CLICK );
					g_DPlay.SendTeleporterReq( pMover->m_szCharacterKey, pTeleportationIconInfo->GetIndex() );
					Destroy();
					return;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->OnRButtonUp( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnRButtonDown( UINT nFlags, CPoint point )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
	else
	{
		//if( g_WndMng.m_pWndWorld->m_bCtrlPushed == TRUE )
		if( GetAsyncKeyState( VK_CONTROL ) & 0x80000 )
		{
			D3DXVECTOR3 vReconvertedPosition( 0.0F, 0.0F, 0.0F );
			ReconvertPosition( 
				vReconvertedPosition, 
				D3DXVECTOR3( static_cast< FLOAT >( point.x ), 0.0F, static_cast< FLOAT >( point.y ) ), 
				m_bySelectedMapLocationID );
			
			if( IsPositionInSelectedMapArea( vReconvertedPosition ) == TRUE )
			{
				//gmpbigsun: 추가된 좌표의 아이디를 선택한 좌표로 인식하게 해서 곧바로 채팅창 링크될수 있게...
				m_dwSelectedUserMarkID = prj.m_MapInformationManager.InsertUserMarkPositionInfo( 
					prj.GetText( TID_GAME_MAP_EX_MARK_INITIAL_TITLE_NAME ), vReconvertedPosition.x, vReconvertedPosition.z );
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_ERROR_MAP_EX_MARK_INVALID_POSITION ), NULL, prj.GetTextColor( TID_ERROR_MAP_EX_MARK_INVALID_POSITION ) );
			}

			//곧바로 채팅창 링크
			OnCommand( ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW, 0, &m_WndMenuUserMark );

			return;
		}

		UserMarkPositionInfoList& rUserMarkPositionInfoList = prj.m_MapInformationManager.GetUserMarkPositionInfoList();
		for( UserMarkPositionInfoList::iterator Iterator = rUserMarkPositionInfoList.begin(); Iterator != rUserMarkPositionInfoList.end(); ++Iterator )
		{
			CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
			if( pUserMarkPositionInfo == NULL )
			{
				continue;
			}

			D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
			ConvertPosition( 
				vConvertedPosition, 
				D3DXVECTOR3( pUserMarkPositionInfo->GetPositionX(), 0.0F, pUserMarkPositionInfo->GetPositionY() ), 
				m_bySelectedMapLocationID );

			CRect rectUserMarkIcon( 0, 0, 0, 0 );
			CalculateMapIconRectFromPoint( 
				rectUserMarkIcon, 
				vConvertedPosition.x, 
				vConvertedPosition.z, 
				m_pUserMarkPositionTexture, 
				USER_MARK_POSITION_TEXTURE_SIZE_RATIO );
			if( rectUserMarkIcon.PtInRect( point ) == TRUE )
			{
				m_dwSelectedUserMarkID = pUserMarkPositionInfo->GetID();

				ClientToScreen( &point );
				m_WndMenuUserMark.Move( point );
				m_WndMenuUserMark.SetVisible( TRUE );
				m_WndMenuUserMark.SetFocus();

				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnMouseMove( UINT nFlags, CPoint point )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->OnMouseMove( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	CRect rectMapWindow = GetWindowRect( TRUE );
	if( zDelta < 0 )
	{
		rectMapWindow.right -= WINDOW_TILE_TEXTURE_SIZE_XY;
		rectMapWindow.bottom -= WINDOW_TILE_TEXTURE_SIZE_XY;
	}
	else
	{
		if( rectMapWindow.right + WINDOW_TILE_TEXTURE_SIZE_XY > g_Option.m_nResWidth )
		{
			rectMapWindow.left -= WINDOW_TILE_TEXTURE_SIZE_XY;
		}
		else
		{
			rectMapWindow.right += WINDOW_TILE_TEXTURE_SIZE_XY;
		}

		if( rectMapWindow.bottom + WINDOW_TILE_TEXTURE_SIZE_XY > g_Option.m_nResHeight - m_nWndTaskBarHeight )
		{
			rectMapWindow.top -= WINDOW_TILE_TEXTURE_SIZE_XY;
		}
		else
		{
			rectMapWindow.bottom += WINDOW_TILE_TEXTURE_SIZE_XY;
		}
	}

	SetWndRect( rectMapWindow, TRUE );

	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndMapEx::SetConstructionMode( CWndMapEx::ConstructionMode eConstructionMode )
{
	m_eConstructionMode = eConstructionMode;
}
//-----------------------------------------------------------------------------
void CWndMapEx::SetHyperlinkMarkPosition( const CString& strHyperlinkMarkTitle, 
										  FLOAT fHyperlinkMarkPositionX, 
										  FLOAT fHyperlinkMarkPositionY )
{
	SetConstructionMode( HYPERLINK );
	m_strHyperlinkMarkTitle = strHyperlinkMarkTitle;
	m_fHyperlinkMarkPositionX = fHyperlinkMarkPositionX;
	m_fHyperlinkMarkPositionY = fHyperlinkMarkPositionY;
}
//-----------------------------------------------------------------------------
void CWndMapEx::InitializeTeleportationInformation( CMover* const pFocusMover )
{
	switch( m_eConstructionMode )
	{
	case TELEPORTATION:
		{
			CHARACTER* pCharacter = pFocusMover->GetCharacter();
			if( pCharacter == NULL )
			{
				return;
			}

			if( pCharacter->m_vecTeleportPos.empty() == true )
			{
				return;
			}

			if( m_idTeleporter != pFocusMover->GetId() )
			{
				m_idTeleporter = pFocusMover->GetId();

				int nIndex = 0;
				for( std::vector< D3DXVECTOR3 >::iterator Iterator = pCharacter->m_vecTeleportPos.begin(); Iterator != pCharacter->m_vecTeleportPos.end(); ++Iterator )
				{
					TeleportationIconInfo* pTeleportationIconInfo = new TeleportationIconInfo;
					pTeleportationIconInfo->SetIndex( nIndex );
					++nIndex;
					pTeleportationIconInfo->SetIconPoint( CPoint( static_cast< LONG >( Iterator->x ), static_cast< LONG >( Iterator->z ) ) );
					m_vecTeleportationPositionRect.push_back( pTeleportationIconInfo );
				}
			}

			break;
		}
	}
}

//-----------------------------------------------------------------------------
void CWndMapEx::OnBlindBuff( )
{
	ResetMapInformation();
}
//-----------------------------------------------------------------------------
void CWndMapEx::ResetMapInformation( void )
{
	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
	if( pWndComboBoxMapName == NULL )
	{
		return;
	}
	DWORD dwSelectedMapNameItemData = pWndComboBoxMapName->GetSelectedItemData();
	if( dwSelectedMapNameItemData == MCD_NONE )
	{
		return;
	}

	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetMapNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( dwSelectedMapNameItemData == pMapComboBoxData->GetID() )
		{
			m_pMapTexture = pMapComboBoxData->GetMapTexture();
			m_dwSelectedMapID = pMapComboBoxData->GetID();
			m_bySelectedMapLocationID = pMapComboBoxData->GetLocationID();
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ResetNPCPosition( void )
{
	CWndComboBox* pWndComboBoxNPCName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	if( pWndComboBoxNPCName == NULL )
	{
		return;
	}

	DWORD dwSelectedNPCNameItemData = pWndComboBoxNPCName->GetSelectedItemData();
	if( dwSelectedNPCNameItemData == MCD_NONE )
	{
		m_fNPCPositionX = -1.0F;
		m_fNPCPositionY = -1.0F;
		return;
	}

	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetNPCNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		//@@ 버그 : 월드맵에서 NPC 위치를 제대로 표시하는 버그.
		//@@ [2012/10/22 ]수정 KYT  : ID와 ParentID까지 같이 검사함.

		// if( dwSelectedNPCNameItemData == pMapComboBoxData->GetID() )
		if( dwSelectedNPCNameItemData == pMapComboBoxData->GetID() &&
			m_dwSelectedMapID == pMapComboBoxData->GetParentID() )
		{
			CPoint pointNPCPosition = pMapComboBoxData->GetNPCPosition();
			m_fNPCPositionX = static_cast< FLOAT >( pointNPCPosition.x );
			m_fNPCPositionY = static_cast< FLOAT >( pointNPCPosition.y );
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RearrangeComboBoxData( DWORD dwParentSelectedListItemData, 
									   DWORD dwComboBoxWIDC, 
									   MapComboBoxDataVector* pMapComboBoxDataVector )
{
	CWndComboBox* pWndComboBox = ( CWndComboBox* )GetDlgItem( dwComboBoxWIDC );
	if( pWndComboBox == NULL )
	{
		return;
	}
	pWndComboBox->ResetContent();

	if( pMapComboBoxDataVector == NULL )
	{
		return;
	}

	if( dwParentSelectedListItemData == MCD_NONE )
	{
		pWndComboBox->EnableWindow( FALSE );
	}
	else
	{
		pWndComboBox->EnableWindow( TRUE );
	}

	int nIndex = 0;
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( pMapComboBoxData->GetParentID() == dwParentSelectedListItemData )
		{
			pWndComboBox->AddString( pMapComboBoxData->GetTitle() );
			pWndComboBox->SetItemData( nIndex, pMapComboBoxData->GetID() );
			++nIndex;
		}
	}

	if( static_cast< int >( pWndComboBox->GetListBoxSize() ) == 1 )
	{
		pWndComboBox->SetCurSel( 0 );
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessMapComboBoxInitialization( void )
{
	if( m_bMapComboBoxInitialization == TRUE )
	{
		return;
	}

	m_bMapComboBoxInitialization = TRUE;

	BYTE byLocationID = CONT_NODATA;
	switch( m_eConstructionMode )
	{
	case NORMAL:
		{
			if( g_pPlayer )
			{
				byLocationID = GetMapArea( g_pPlayer->GetPos() );
			}
			break;
		}
	case HYPERLINK:
		{
			if( m_strHyperlinkMarkTitle != _T( "" ) && m_fHyperlinkMarkPositionX != -1.0F && m_fHyperlinkMarkPositionY != -1.0F )
			{
				byLocationID = GetMapArea( D3DXVECTOR3( m_fHyperlinkMarkPositionX, 0.0F, m_fHyperlinkMarkPositionY ) );
			}
			break;
		}
	}

	CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );

	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetMapNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( byLocationID == pMapComboBoxData->GetLocationID() )
		{
			DWORD dwMapCategoryID = pMapComboBoxData->GetParentID();

			if( pWndComboBoxMapCategory )
			{
				pWndComboBoxMapCategory->SelectItem( dwMapCategoryID );
			}

			if( pWndComboBoxMapName )
			{
				pWndComboBoxMapName->SelectItem( pMapComboBoxData->GetID() );
			}

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessMapSizeInformation( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	CSize sizeClient = GetClientRect().Size();
	CSize sizeTexture = m_pMapTexture->m_size;
	float fWindowXYRatio = static_cast< float >( sizeClient.cx ) / static_cast< float >( sizeClient.cy );
	float fTextureXYRatio = static_cast< float >( sizeTexture.cx ) / static_cast< float >( sizeTexture.cy );
	float fTextureYXRatio = static_cast< float >( sizeTexture.cy ) / static_cast< float >( sizeTexture.cx );
	FLOAT fRevisedMapSizeRatio = 1.0F;

	CSize sizeRevisedTexture( sizeClient );
	int nStartPositionX = 0;
	int nStartPositionY = 0;
	if( fWindowXYRatio > fTextureXYRatio )
	{
		sizeRevisedTexture.cx = FtoInt( static_cast< float >( sizeClient.cy ) * fTextureXYRatio );
		sizeRevisedTexture.cy = sizeClient.cy;
		nStartPositionX = ( sizeClient.cx - sizeRevisedTexture.cx ) / 2;
		m_fRevisedMapSizeRatio = fRevisedMapSizeRatio * ( ( static_cast< FLOAT >( sizeRevisedTexture.cy ) / static_cast< FLOAT >( sizeTexture.cy ) ) );
	}
	else if( fWindowXYRatio < fTextureXYRatio )
	{
		sizeRevisedTexture.cx = sizeClient.cx;
		sizeRevisedTexture.cy = FtoInt( static_cast< float >( sizeClient.cx ) * fTextureYXRatio );
		nStartPositionY = ( sizeClient.cy - sizeRevisedTexture.cy ) / 2;
		m_fRevisedMapSizeRatio = fRevisedMapSizeRatio * ( ( static_cast< FLOAT >( sizeRevisedTexture.cx ) / static_cast< FLOAT >( sizeTexture.cx ) ) );
	}

	m_rectRevisedMapPosition.SetRect( nStartPositionX, nStartPositionY, nStartPositionX + sizeRevisedTexture.cx, nStartPositionY + sizeRevisedTexture.cy );
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessMonsterInformationToolTip( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
	if( pWndButton == NULL )
	{
		return;
	}

	if( m_bShowMonsterInformation == FALSE )
	{
		return;
	}

	CMapMonsterInformationPack* pMapMonsterInformationPack = prj.m_MapInformationManager.FindMapMonsterInformationPack( m_dwSelectedMapID );
	if( pMapMonsterInformationPack == NULL )
	{
		return;
	}

	CPoint pointMouse = GetMousePoint();

	for( int i = 0; i < static_cast< int >( pMapMonsterInformationPack->GetNumber() ); ++i )
	{
		CTexture* pMonsterIconTexture = pMapMonsterInformationPack->GetAt( i );
		if( pMonsterIconTexture == NULL )
		{
			continue;
		}

		const CMapMonsterInformation* pMapMonsterInformation = pMapMonsterInformationPack->GetMapMonsterInformation( i );
		if( pMapMonsterInformation == NULL )
		{
			continue;
		}

		CRect rectRevisedMonsterIconPosition( 0, 0, 0, 0 );
		ReviseScriptRectInformation( rectRevisedMonsterIconPosition, pMapMonsterInformation->GetIconPositionRect() );

		m_bMonsterInformationToolTip = FALSE;

		if( rectRevisedMonsterIconPosition.PtInRect( pointMouse ) == TRUE )
		{
			CEditString strMonsterInformationToolTip = _T( "" );
			CString strTemp = _T( "" );

			CPoint pointMouseScreen = pointMouse;
			CRect rectRevisedMonsterIconPositionScreen = rectRevisedMonsterIconPosition;
			ClientToScreen( &pointMouseScreen );
			ClientToScreen( &rectRevisedMonsterIconPositionScreen );

			for( int j = 0; j < pMapMonsterInformation->GetMonsterIDNumber(); ++j )
			{
				MoverProp* pMoverProp = prj.GetMoverProp( pMapMonsterInformation->GetMonsterID( j ) );
				if( pMoverProp == NULL )
				{
					continue;
				}

				strMonsterInformationToolTip.AddString( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ), D3DCOLOR_XRGB( 0, 0, 255 ) );
				strTemp.Format( _T( "%d " ), pMoverProp->dwLevel );
				strMonsterInformationToolTip.AddString( strTemp, D3DCOLOR_XRGB( 130, 130, 200 ) );
				strTemp.Format( _T( "%s" ), pMoverProp->szName );
				if( j + 1 == pMapMonsterInformation->GetMonsterIDNumber() )
				{
					strMonsterInformationToolTip.AddString( strTemp, D3DCOLOR_XRGB( 255, 0, 0 ) );
				}
				else
				{
					strMonsterInformationToolTip.AddString( strTemp );
				}
				strMonsterInformationToolTip.AddString( _T( "\n" ) );
			}

			strMonsterInformationToolTip.AddString( _T( "\n" ) );

			strTemp.Format( _T( "%s : " ), prj.GetText( TID_GAME_DROP_ITEM ) );
			strMonsterInformationToolTip.AddString( strTemp );
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pMapMonsterInformation->GetDropItemID() );
			if( pItemProp )
			{
				strTemp.Format( _T( "%s" ), pItemProp->szName );
				strMonsterInformationToolTip.AddString( strTemp, D3DCOLOR_XRGB( 46, 112, 169 ) );
			}
			else
			{
				strMonsterInformationToolTip.AddString( prj.GetText( TID_GAME_DROP_NONE ) );
			}

			g_toolTip.PutToolTip( 10000, strMonsterInformationToolTip, rectRevisedMonsterIconPositionScreen, pointMouseScreen, 0 );
			g_toolTip.ResizeMapMonsterToolTip();
			for( int k = 0; k < pMapMonsterInformation->GetMonsterIDNumber(); ++k )
			{
				g_toolTip.InsertMonsterID( pMapMonsterInformation->GetMonsterID( k ) );
			}

			m_nSelectedMonsterIconIndex = i;
			m_bMonsterInformationToolTip = TRUE;

			break;
		}
	}

	if( m_bMonsterInformationToolTip == FALSE )
	{
		m_nSelectedMonsterIconIndex = -1;
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessUserMarkToolTip( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	UserMarkPositionInfoList& rUserMarkPositionInfoList = prj.m_MapInformationManager.GetUserMarkPositionInfoList();
	for( UserMarkPositionInfoList::iterator Iterator = rUserMarkPositionInfoList.begin(); Iterator != rUserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		FLOAT fUserMarkPositionX = pUserMarkPositionInfo->GetPositionX();
		FLOAT fUserMarkPositionY = pUserMarkPositionInfo->GetPositionY();
		if( fUserMarkPositionX == -1.0F && fUserMarkPositionY == -1.0F )
		{
			continue;
		}

		const D3DXVECTOR3 vUserMarkPosition( fUserMarkPositionX, 0.0F, fUserMarkPositionY );
		if( IsPositionInSelectedMapArea( vUserMarkPosition ) == FALSE )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( vConvertedPosition, vUserMarkPosition, m_bySelectedMapLocationID );

		CRect rectUserMarkIcon( 0, 0, 0, 0 );
		CalculateMapIconRectFromPoint( 
			rectUserMarkIcon, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pUserMarkPositionTexture, 
			USER_MARK_POSITION_TEXTURE_SIZE_RATIO );
		CPoint pointMouse = GetMousePoint();
		if( rectUserMarkIcon.PtInRect( pointMouse ) == TRUE )
		{
			CEditString strUserMarkToolTip = pUserMarkPositionInfo->GetName();
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectUserMarkIcon );
			g_toolTip.PutToolTip( 10000, strUserMarkToolTip, rectUserMarkIcon, pointMouse, 0 );

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessHyperlinkMarkToolTip( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	if( m_strHyperlinkMarkTitle == _T( "" ) )
	{
		return;
	}

	if( m_fHyperlinkMarkPositionX == -1.0F && m_fHyperlinkMarkPositionY == -1.0F )
	{
		return;
	}

	const D3DXVECTOR3 vHyperlinkMarkPosition( m_fHyperlinkMarkPositionX, 0.0F, m_fHyperlinkMarkPositionY );
	if( IsPositionInSelectedMapArea( vHyperlinkMarkPosition ) == FALSE )
	{
		return;
	}

	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( 
		vConvertedPosition, 
		D3DXVECTOR3( m_fHyperlinkMarkPositionX, 0.0F, m_fHyperlinkMarkPositionY ), 
		m_bySelectedMapLocationID );

	CRect rectHyperlinkMarkIcon( 0, 0, 0, 0 );
	CalculateMapIconRectFromPoint( 
		rectHyperlinkMarkIcon, 
		vConvertedPosition.x, 
		vConvertedPosition.z, 
		m_pHyperlinkMarkPositionTexture, 
		HYPERLINK_MARK_POSITION_TEXTURE_SIZE_RATIO );
	CPoint pointMouse = GetMousePoint();
	if( rectHyperlinkMarkIcon.PtInRect( pointMouse ) == TRUE )
	{
		ClientToScreen( &pointMouse );
		ClientToScreen( &rectHyperlinkMarkIcon );
		g_toolTip.PutToolTip( 10000, m_strHyperlinkMarkTitle, rectHyperlinkMarkIcon, pointMouse, 0 );
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessIconTextureAlpha( void )
{
	if( m_eConstructionMode != DESTINATION )
	{
		return;
	}

	float fDelta = ( static_cast< float >( g_tmCurrent - m_tmOld ) / 1000.0f );
	m_tmOld = g_tmCurrent;

	if( m_bAlphaSwitch == FALSE )
	{
		m_nIconTextureAlpha += static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nIconTextureAlpha > MAXIMUM_ICON_TEXTURE_ALPHA )
		{
			m_nIconTextureAlpha = MAXIMUM_ICON_TEXTURE_ALPHA;
			m_bAlphaSwitch = TRUE;
		}
	}
	else
	{
		m_nIconTextureAlpha -= static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nIconTextureAlpha < MINIMUM_ICON_TEXTURE_ALPHA )
		{
			m_nIconTextureAlpha = MINIMUM_ICON_TEXTURE_ALPHA;
			m_bAlphaSwitch = FALSE;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessVisualSetting( void )
{
	//gmpbigsun( 20101028 ) : 보여줄것이 있을때 등장
	CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	if( 1 > pWndComboBoxNpcName->GetListBoxSize( ) )
		pWndComboBoxNpcName->SetVisible( FALSE );
	else pWndComboBoxNpcName->SetVisible( TRUE );

	//gmpbigsun: 아래는 쓰지 않는기능으로 주석처리함
	//if( prj.m_bMapTransparent == FALSE )
	//{
	//	m_byTransparentStateAlpha = NORMAL_STATE_ALPHA;

	//	CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
	//	if( pWndComboBoxMapCategory && pWndComboBoxMapCategory->IsVisible() == FALSE )
	//	{
	//		pWndComboBoxMapCategory->SetVisible( TRUE );
	//	}

	//	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
	//	if( pWndComboBoxMapName && pWndComboBoxMapName->IsVisible() == FALSE )
	//	{
	//		pWndComboBoxMapName->SetVisible( TRUE );
	//	}

	//	CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	//	if( pWndComboBoxNpcName && pWndComboBoxNpcName->IsVisible() == FALSE )
	//	{
	//		pWndComboBoxNpcName->SetVisible( TRUE );
	//	}

	//	CWndButton* pWndButtonMonsterInfo = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
	//	if( pWndButtonMonsterInfo && pWndButtonMonsterInfo->IsVisible() == FALSE )
	//	{
	//		pWndButtonMonsterInfo->SetVisible( TRUE );
	//	}
	//}
	//else
	//{
	//	m_byTransparentStateAlpha = TRANSPARENT_STATE_ALPHA;

	//	CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
	//	if( pWndComboBoxMapCategory && pWndComboBoxMapCategory->IsVisible() == TRUE )
	//	{
	//		pWndComboBoxMapCategory->SetVisible( FALSE );
	//	}

	//	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
	//	if( pWndComboBoxMapName && pWndComboBoxMapName->IsVisible() == TRUE )
	//	{
	//		pWndComboBoxMapName->SetVisible( FALSE );
	//	}

	//	//CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	//	//if( pWndComboBoxNpcName && pWndComboBoxNpcName->IsVisible() == TRUE )
	//	//{
	//	//	pWndComboBoxNpcName->SetVisible( FALSE );
	//	//}

	//	CWndButton* pWndButtonMonsterInfo = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
	//	if( pWndButtonMonsterInfo && pWndButtonMonsterInfo->IsVisible() == TRUE )
	//	{
	//		pWndButtonMonsterInfo->SetVisible( FALSE );
	//	}
	//}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderBackground( C2DRender* p2DRender )
{
	if( prj.m_bMapTransparent == FALSE )
	{
		p2DRender->RenderFillRect( GetLayoutRect(), D3DCOLOR_ARGB( m_byTransparentStateAlpha, 0, 0, 0 ) );
	}

	if( m_pMapTexture == NULL )
	{
		return;
	}

	p2DRender->RenderTexture( 
		CPoint( m_rectRevisedMapPosition.left, m_rectRevisedMapPosition.top ), 
		m_pMapTexture, 
		m_byTransparentStateAlpha, 
		m_fRevisedMapSizeRatio, 
		m_fRevisedMapSizeRatio );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderTeleportationPosition( C2DRender* p2DRender )
{
	if( m_eConstructionMode != TELEPORTATION )
		return;

	if( m_pTeleportationPositionTexture == NULL )
		return;

	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
	{
		TeleportationIconInfo* pTeleportationIconInfo = ( TeleportationIconInfo* )( *Iterator );
		if( pTeleportationIconInfo == NULL )
		{
			continue;
		}

		const D3DXVECTOR3 vTeleportationIconPosition( 
			static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().x ), 
			0.0F, 
			static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().y ) );
		if( IsPositionInSelectedMapArea( vTeleportationIconPosition ) == FALSE )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( vConvertedPosition, vTeleportationIconPosition, m_bySelectedMapLocationID );

		CRect rectTeleportationPositionIcon( 0, 0, 0, 0 );
		FLOAT fRevisedTeleportationPositionTextureSizeRatio = CalculateMapIconRectFromPoint( 
			rectTeleportationPositionIcon, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pTeleportationPositionTexture, 
			TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO );
		CPoint pointMouse = GetMousePoint();
		if( rectTeleportationPositionIcon.PtInRect( pointMouse ) == TRUE )
		{
			CPoint pointExtendTeleportationPosition( 0, 0 );
			FLOAT fExtendTeleportationPositionTextureSizeRatio = CalculateMapIconStartPosition( 
				pointExtendTeleportationPosition, 
				vConvertedPosition.x, 
				vConvertedPosition.z, 
				m_pTeleportationPositionTexture, 
				EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO );
			m_pTeleportationPositionTexture->RenderScal( 
				p2DRender, 
				pointExtendTeleportationPosition, 
				255, 
				fExtendTeleportationPositionTextureSizeRatio, 
				fExtendTeleportationPositionTextureSizeRatio );
		}
		else
		{
			m_pTeleportationPositionTexture->RenderScal( 
				p2DRender, 
				CPoint( rectTeleportationPositionIcon.left, rectTeleportationPositionIcon.top ), 
				255, 
				fRevisedTeleportationPositionTextureSizeRatio, 
				fRevisedTeleportationPositionTextureSizeRatio );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderDestinationPosition( C2DRender* p2DRender )
{
	if( m_eConstructionMode != DESTINATION )
	{
		return;
	}

	if( m_pDestinationPositionTexture == NULL )
	{
		return;
	}

	CWndWorld* pWndWorld = ( CWndWorld* )g_WndMng.GetApplet( APP_WORLD );
	if( pWndWorld == NULL )
	{
		return;
	}

	FLOAT fDestinationPositionX = pWndWorld->m_vDestinationArrow.x;
	FLOAT fDestinationPositionY = pWndWorld->m_vDestinationArrow.z;
	if( fDestinationPositionX == -1.0F && fDestinationPositionY == -1.0F )
	{
		return;
	}

	const D3DXVECTOR3 vDestinationPosition( fDestinationPositionX, 0.0F, fDestinationPositionY );
	if( IsPositionInSelectedMapArea( vDestinationPosition ) == FALSE )
	{
		return;
	}

	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( vConvertedPosition, vDestinationPosition, m_bySelectedMapLocationID );

	CPoint pointRevisedDestinationPosition( 0, 0 );
	FLOAT fRevisedDestinationPositionTextureSizeRatio = CalculateMapIconStartPosition( 
		pointRevisedDestinationPosition, 
		vConvertedPosition.x, 
		vConvertedPosition.z, 
		m_pDestinationPositionTexture, 
		DESTINATION_POSITION_TEXTURE_SIZE_RATIO );
	m_pDestinationPositionTexture->RenderScal( 
		p2DRender, 
		pointRevisedDestinationPosition, 
		m_nIconTextureAlpha, 
		fRevisedDestinationPositionTextureSizeRatio, 
		fRevisedDestinationPositionTextureSizeRatio );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderNPCPosition( C2DRender* p2DRender )
{
	if( m_pNPCPositionTexture == NULL )
	{
		return;
	}

	if( m_fNPCPositionX == -1.0F && m_fNPCPositionY == -1.0F )
	{
		return;
	}

	const D3DXVECTOR3 vNPCPosition( m_fNPCPositionX, 0.0F, m_fNPCPositionY );
	if( IsPositionInSelectedMapArea( vNPCPosition ) == FALSE )
	{
		return;
	}

	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( vConvertedPosition, vNPCPosition, m_bySelectedMapLocationID );

	CPoint pointRevisedNPCPosition( 0, 0 );
	FLOAT fRevisedNPCPositionTextureSizeRatio = CalculateMapIconStartPosition( 
		pointRevisedNPCPosition, 
		vConvertedPosition.x, 
		vConvertedPosition.z, 
		m_pNPCPositionTexture, 
		NPC_POSITION_TEXTURE_SIZE_RATIO );

	//gmpbigsun( 20111125 ) : 마크 깜빡거림, 사이즈 약간 키움.
	//frame 대비 일정한 수치로!
	static int nAlpha = 0; 
	static BOOL bToggle = TRUE;
	float fFrameUP = ( g_Neuz.m_fFPS / 60.0f  );
	if( fFrameUP > 1.0f )
		fFrameUP = 1.0f;
	else fFrameUP = 60.0f / g_Neuz.m_fFPS;


	if( bToggle )
	{
		nAlpha -= int( fFrameUP * 8.0f );
		if( nAlpha < 10 )
		{
			nAlpha = 10;
			bToggle = FALSE;
		}
	}
	else
	{
		nAlpha += int( fFrameUP * 8.0f );
		if( nAlpha > 255 )
		{
			nAlpha = 255;
			bToggle = TRUE;
		}
	}
	
	fRevisedNPCPositionTextureSizeRatio *= 1.4f;
	m_pNPCPositionTexture->RenderScal( 
		p2DRender, 
		pointRevisedNPCPosition, 
		(DWORD)nAlpha, 
		fRevisedNPCPositionTextureSizeRatio, 
		fRevisedNPCPositionTextureSizeRatio );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderUserMarkPosition( C2DRender* p2DRender )
{
	if( m_pUserMarkPositionTexture == NULL )
	{
		return;
	}

	//BEGIN --gmpbigsun: 포지션 마크 깜빡거리는 효과
	static float fAddedDelta = 0.0f;
	static BOOL bReverse = FALSE;
	
	if( fAddedDelta > 300.f )
		bReverse = TRUE;
	else if ( fAddedDelta <= 0.0f )
	{
		fAddedDelta = 0.0f;
		bReverse = FALSE;
	}
	
	bReverse ? fAddedDelta -= _GetDeltaTime( ) : fAddedDelta += _GetDeltaTime( );

	float fRate = fAddedDelta / 200.0f;
	fRate <= 0.0f ? fRate = 0.0f : 0;
	fRate >= 1.0f ? fRate = 1.0f : 0;

	DWORD dwAlpha = (DWORD)( 255.0f * fRate );
	//END --

	UserMarkPositionInfoList& rUserMarkPositionInfoList = prj.m_MapInformationManager.GetUserMarkPositionInfoList();
	for( UserMarkPositionInfoList::iterator Iterator = rUserMarkPositionInfoList.begin(); Iterator != rUserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		FLOAT fUserMarkPositionX = pUserMarkPositionInfo->GetPositionX();
		FLOAT fUserMarkPositionY = pUserMarkPositionInfo->GetPositionY();
		if( fUserMarkPositionX == -1.0F && fUserMarkPositionY == -1.0F )
		{
			continue;
		}

		const D3DXVECTOR3 vUserMarkPosition( fUserMarkPositionX, 0.0F, fUserMarkPositionY );
		if( IsPositionInSelectedMapArea( vUserMarkPosition ) == FALSE )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( vConvertedPosition, vUserMarkPosition, m_bySelectedMapLocationID );

		CPoint pointRevisedUserMarkPosition( 0, 0 );
		FLOAT fRevisedUserMarkPositionTextureSizeRatio = CalculateMapIconStartPosition( 
			pointRevisedUserMarkPosition, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pUserMarkPositionTexture, 
			USER_MARK_POSITION_TEXTURE_SIZE_RATIO );

		DWORD dwAlpha_Applied = 255;
		UserMarkPositionInfoList::iterator tempIter = Iterator;
		if( ++tempIter == rUserMarkPositionInfoList.end() )	//last data
			dwAlpha_Applied = dwAlpha;
		
		m_pUserMarkPositionTexture->RenderScal( 
			p2DRender, 
			pointRevisedUserMarkPosition, 
			dwAlpha_Applied, 
			fRevisedUserMarkPositionTextureSizeRatio, 
			fRevisedUserMarkPositionTextureSizeRatio );
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderHyperlinkMarkPosition( C2DRender* p2DRender )
{
	if( m_pHyperlinkMarkPositionTexture == NULL )
	{
		return;
	}

	if( m_fHyperlinkMarkPositionX == -1.0F && m_fHyperlinkMarkPositionY == -1.0F )
	{
		return;
	}

	const D3DXVECTOR3 vHyperlinkMarkPosition( m_fHyperlinkMarkPositionX, 0.0F, m_fHyperlinkMarkPositionY );
	if( IsPositionInSelectedMapArea( vHyperlinkMarkPosition ) == FALSE )
	{
		return;
	}

	//BEGIN --gmpbigsun: 포지션 마크 깜빡거리는 효과
	static float fAddedDelta = 0.0f;
	static BOOL bReverse = FALSE;
	
	if( fAddedDelta > 300.f )
		bReverse = TRUE;
	else if ( fAddedDelta <= 0.0f )
	{
		fAddedDelta = 0.0f;
		bReverse = FALSE;
	}
	
	bReverse ? fAddedDelta -= _GetDeltaTime( ) : fAddedDelta += _GetDeltaTime( );

	float fRate = fAddedDelta / 200.0f;
	fRate <= 0.0f ? fRate = 0.0f : 0;
	fRate >= 1.0f ? fRate = 1.0f : 0;

	DWORD dwAlpha = (DWORD)( 255.0f * fRate );
	//END --

	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( vConvertedPosition, vHyperlinkMarkPosition, m_bySelectedMapLocationID );

	CPoint pointRevisedHyperlinkMarkPosition( 0, 0 );
	FLOAT fRevisedHyperlinkMarkPositionTextureSizeRatio = CalculateMapIconStartPosition( 
		pointRevisedHyperlinkMarkPosition, 
		vConvertedPosition.x, 
		vConvertedPosition.z, 
		m_pHyperlinkMarkPositionTexture, 
		HYPERLINK_MARK_POSITION_TEXTURE_SIZE_RATIO );
	m_pHyperlinkMarkPositionTexture->RenderScal( 
		p2DRender, 
		pointRevisedHyperlinkMarkPosition, 
		dwAlpha, 
		fRevisedHyperlinkMarkPositionTextureSizeRatio, 
		fRevisedHyperlinkMarkPositionTextureSizeRatio );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderPlayerAndPartyPlayerPosition( C2DRender* p2DRender )
{
	for( int i = 0; i < g_Party.GetSizeofMember(); ++i )
	{
		if( g_pPlayer->m_idPlayer != g_Party.GetPlayerId( i ) )
		{
			if( g_Party.m_aMember[ i ].m_bRemove == FALSE )
			{
				if( m_pPartyPCArrowTexture )
				{
					RenderPlayerPosition( 
						p2DRender, 
						m_pPartyPCArrowTexture, 
						g_Party.GetPos( i ), 
						g_Party.GetPos( i ) + D3DXVECTOR3( 0.0F, 0.0F, -1.0F ), 
						CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ), 
						0xffffff7f );
				}
			}
		}
	}

	RenderPlayerPosition( 
		p2DRender, 
		m_pPCArrowTexture, 
		g_pPlayer->GetPos(), 
		g_Neuz.m_camera.m_vPos, 
		g_pPlayer->GetName(), 
		0xff7fff7f );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderPlayerPosition( C2DRender* p2DRender, 
									 CTexture* pArrowTexture, 
									 const D3DXVECTOR3& vPlayerPosition, 
									 const D3DXVECTOR3& vCameraPosition, 
									 const CString& strName, 
									 DWORD dwNameColor )
{
	if( pArrowTexture == NULL )
		return;

	if( IsPositionInSelectedMapArea( vPlayerPosition ) == FALSE )
		return;

	// 위치 출력
	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( vConvertedPosition, vPlayerPosition, m_bySelectedMapLocationID );
	CPoint pointConvertedPosition( static_cast< int >( vConvertedPosition.x ), static_cast< int >( vConvertedPosition.z ) );

	D3DXVECTOR3 vCamera = g_Neuz.m_camera.m_vPos;
	D3DXVECTOR3 vView = vPlayerPosition - vCameraPosition;
	vView.y = 0.0F;
	D3DXVec3Normalize( &vView, &vView );
	D3DXVECTOR3 vDatumLine = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	FLOAT fTheta = D3DXVec3Dot( &vDatumLine, &vView );
	D3DXVECTOR3 vAxis( 0.0F, 0.0F, 0.0F );
	D3DXVec3Cross( &vAxis, &vDatumLine, &vView );
	FLOAT fResultRadian = ( vAxis.y >= 0.0F ) ? acosf( fTheta ) : -acosf( fTheta );
	CPoint pointRevisedPCArrowPosition( 0, 0 );
	FLOAT fRevisedPCArrowTextureSizeRatio = CalculateMapIconStartPosition( 
		pointRevisedPCArrowPosition, 
		static_cast< FLOAT >( pointConvertedPosition.x ), 
		static_cast< FLOAT >( pointConvertedPosition.y ), 
		pArrowTexture, 
		PC_ARROW_POSITION_TEXTURE_SIZE_RATIO );
	pArrowTexture->RenderRotate( 
		p2DRender, 
		pointRevisedPCArrowPosition, 
		fResultRadian, 
		TRUE, 
		255, 
		fRevisedPCArrowTextureSizeRatio, 
		fRevisedPCArrowTextureSizeRatio );

	// 이름 출력
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( CWndBase::m_Theme.m_pFontWorld );
	static const int FONT_REVISING_VALUE = 2;

	CSize sizeNameText = p2DRender->GetFont()->GetTextExtent( strName );
	// 폰트가 변경되어 원래 크기보다 FONT_REVISING_VALUE 만큼 더 늘어남 (가로 세로 모두 )
	sizeNameText.cx += ( FONT_REVISING_VALUE * 2 );
	sizeNameText.cy += ( FONT_REVISING_VALUE * 2 );

	static const FLOAT ARROW_TEXTURE_AND_NAME_TEXT_INTERVAL = 22.0F;
	int nTextPositionX = pointConvertedPosition.x;
	int nTextPositionY = pointConvertedPosition.y;
	nTextPositionX -= sizeNameText.cx / 2;
	nTextPositionY -= FtoInt( ARROW_TEXTURE_AND_NAME_TEXT_INTERVAL * m_fRevisedMapSizeRatio ) + ( sizeNameText.cy / 2 );
	p2DRender->TextOut( nTextPositionX, nTextPositionY, strName, dwNameColor );

	p2DRender->SetFont( pOldFont );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderMapMonsterInformation( C2DRender* p2DRender )
{
	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
	if( pWndButton == NULL )
	{
		return;
	}

	if( m_bShowMonsterInformation == FALSE )
	{
		return;
	}

	if( m_dwSelectedMapID == MCD_NONE )
	{
		return;
	}
	CMapMonsterInformationPack* pMapMonsterInformationPack = prj.m_MapInformationManager.FindMapMonsterInformationPack( m_dwSelectedMapID );
	if( pMapMonsterInformationPack == NULL )
	{
		return;
	}

	for( int i = 0; i < static_cast< int >( pMapMonsterInformationPack->GetNumber() ); ++i )
	{
		CTexture* pMonsterIconTexture = pMapMonsterInformationPack->GetAt( i );
		if( pMonsterIconTexture == NULL )
		{
			continue;
		}

		const CMapMonsterInformation* pMapMonsterInformation = pMapMonsterInformationPack->GetMapMonsterInformation( i );
		if( pMapMonsterInformation == NULL )
		{
			continue;
		}

		CRect rectRevisedMonsterIconPosition( 0, 0, 0, 0 );
		ReviseScriptRectInformation( 
			rectRevisedMonsterIconPosition, 
			pMapMonsterInformation->GetIconPositionRect() );

		if( i == m_nSelectedMonsterIconIndex )
		{
			pMonsterIconTexture->RenderScal( 
				p2DRender, 
				CPoint( rectRevisedMonsterIconPosition.left, rectRevisedMonsterIconPosition.top ), 
				255, 
				m_fRevisedMapSizeRatio, 
				m_fRevisedMapSizeRatio );
		}
		else
		{
			pMonsterIconTexture->RenderScal( 
				p2DRender, 
				CPoint( rectRevisedMonsterIconPosition.left, rectRevisedMonsterIconPosition.top ), 
				150, 
				m_fRevisedMapSizeRatio, 
				m_fRevisedMapSizeRatio );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderRainbowNPCInformation( C2DRender* p2DRender )
{
	if( CRainbowRace::GetInstance()->m_dwRemainTime == 0 )
	{
		return;
	}

	CRainbowNPCInformationPack* pRainbowNPCInformationPack = prj.m_MapInformationManager.GetRainbowNPCInformationPack();
	if( pRainbowNPCInformationPack == NULL )
	{
		return;
	}

	for( int i = 0; i < static_cast< int >( pRainbowNPCInformationPack->GetNumber() ); ++i )
	{
		CTexture* pRainbowNPCTexture = pRainbowNPCInformationPack->GetAt( i );
		if( pRainbowNPCTexture == NULL )
		{
			continue;
		}

		const CRainbowNPCInformation* pRainbowNPCInformation = pRainbowNPCInformationPack->GetRainbowNPCInformation( i );
		if( pRainbowNPCInformation == NULL )
		{
			continue;
		}

		CRect rectRevisedRainbowNPCPosition( 0, 0, 0, 0 );
		if( m_dwSelectedMapID == MCD_MADRIGAL )
		{
			ReviseScriptRectInformation( 
				rectRevisedRainbowNPCPosition, 
				pRainbowNPCInformation->GetWorldPositionRect() );
			pRainbowNPCTexture->RenderScal( 
				p2DRender, 
				CPoint( rectRevisedRainbowNPCPosition.left, rectRevisedRainbowNPCPosition.top ), 
				255, 
				m_fRevisedMapSizeRatio, 
				m_fRevisedMapSizeRatio );
		}
		else if( m_dwSelectedMapID == pRainbowNPCInformation->GetMapID() )
		{
			ReviseScriptRectInformation( 
				rectRevisedRainbowNPCPosition, 
				pRainbowNPCInformation->GetRegionPositionRect() );
			pRainbowNPCTexture->RenderScal( 
				p2DRender, 
				CPoint( rectRevisedRainbowNPCPosition.left, rectRevisedRainbowNPCPosition.top ), 
				255, 
				m_fRevisedMapSizeRatio, 
				m_fRevisedMapSizeRatio );
		}
	}
}
//-----------------------------------------------------------------------------
FLOAT CWndMapEx::CalculateMapIconRectFromPoint( CRect& rectDestinationIcon, 
											    FLOAT fIconPositionX, 
												FLOAT fIconPositionY, 
												const CTexture* const pIconTexture, 
												FLOAT fIconSizeRatio )
{
	if( pIconTexture == NULL )
	{
		return 0.0F;
	}

	FLOAT fRevisedIconSizeRatio = fIconSizeRatio * m_fRevisedMapSizeRatio;
	FLOAT fRevisedIconSizeX = static_cast< FLOAT >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconSizeY = static_cast< FLOAT >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconStartPositionX = fIconPositionX - ( fRevisedIconSizeX * 0.5f );
	FLOAT fRevisedIconStartPositionY = fIconPositionY - ( fRevisedIconSizeY * 0.5f );
	rectDestinationIcon.SetRect( 
		FtoInt( fRevisedIconStartPositionX ), 
		FtoInt( fRevisedIconStartPositionY ), 
		FtoInt( fRevisedIconStartPositionX + fRevisedIconSizeX ), 
		FtoInt( fRevisedIconStartPositionY + fRevisedIconSizeY ) );
	return fRevisedIconSizeRatio;
}
//-----------------------------------------------------------------------------
FLOAT CWndMapEx::CalculateMapIconStartPosition( CPoint& pointDestinationPosition, 
											    FLOAT fIconPositionX, 
												FLOAT fIconPositionY, 
												const CTexture* const pIconTexture, 
												FLOAT fIconSizeRatio )
{
	FLOAT fRevisedIconSizeRatio = fIconSizeRatio * m_fRevisedMapSizeRatio;
	FLOAT fRevisedIconSizeX = static_cast< FLOAT >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconSizeY = static_cast< FLOAT >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio;

	pointDestinationPosition.SetPoint(	FtoInt( fIconPositionX - ( fRevisedIconSizeX * 0.5f ) ), 			//gmpbigsun( 20110417 ) : static_cast< int > ===> FtoInt 
										FtoInt( fIconPositionY - ( fRevisedIconSizeY * 0.5f ) ) ); 

	return fRevisedIconSizeRatio;
}


//-----------------------------------------------------------------------------
FLOAT CWndMapEx::CalculateQuestIconStartPosition(	CPoint&					ptDestinationPosition, 
													float					fIconPositionX, 
													float					fIconPositionY, 
													const CTexture* const	pIconTexture, 
													float					fIconSizeRatio )
{
	if( NULL == pIconTexture )
	{
		FLASSERT( NULL );
		return 0.0f;
	}

	FLOAT fRevisedIconSizeRatio( fIconSizeRatio * m_fRevisedMapSizeRatio );
	FLOAT fRevisedIconSizeX( static_cast< float >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio );	// 아이콘 가로 사이즈 * 맵 비율
	FLOAT fRevisedIconSizeY( static_cast< float >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio ); // 아이콘 세로 사이즈 * 맵 비율

	ptDestinationPosition.SetPoint(	FtoInt( fIconPositionX - ( fRevisedIconSizeX * 0.5f ) ),		
									FtoInt( fIconPositionY - ( fRevisedIconSizeY ) ) );					// 시작 위치가 아이콘의 맨 밑에 위치하게 한다... 기획이 부탁함

	return fRevisedIconSizeRatio;
}


//-----------------------------------------------------------------------------
const D3DXVECTOR3& CWndMapEx::ConvertPosition( D3DXVECTOR3& vDestination, 
											   const D3DXVECTOR3& vSource, 
											   BYTE byLocationID )
{
	CRect rectLocationRealPosition = prj.m_MapInformationManager.FindRealPosition( byLocationID );
	FLOAT fLocationRealPositionX1 = static_cast< FLOAT >( rectLocationRealPosition.left );	
	FLOAT fLocationRealPositionY1 = static_cast< FLOAT >( rectLocationRealPosition.top );
	FLOAT fLocationRealPositionX2 = static_cast< FLOAT >( rectLocationRealPosition.right );
	FLOAT fLocationRealPositionY2 = static_cast< FLOAT >( rectLocationRealPosition.bottom );
	FLOAT fRealMapWidth = fLocationRealPositionX2 - fLocationRealPositionX1;
	FLOAT fRealMapHeight = fLocationRealPositionY2 - fLocationRealPositionY1;
	FLOAT fRealPositionRatioX = ( vSource.x - fLocationRealPositionX1 ) / fRealMapWidth;
	FLOAT fRealPositionRatioY = ( fLocationRealPositionY2 - vSource.z ) / fRealMapHeight;
	vDestination.x = static_cast< FLOAT >( m_rectRevisedMapPosition.left ) + ( static_cast< FLOAT >( m_rectRevisedMapPosition.Width() ) * fRealPositionRatioX );
	vDestination.y = vSource.y;
	vDestination.z = static_cast< FLOAT >( m_rectRevisedMapPosition.top ) + ( static_cast< FLOAT >( m_rectRevisedMapPosition.Height() ) * fRealPositionRatioY );
	return vDestination;
}
//-----------------------------------------------------------------------------
const D3DXVECTOR3& CWndMapEx::ReconvertPosition( D3DXVECTOR3& vDestination, 
												 const D3DXVECTOR3& vSource, 
												 BYTE byLocationID )
{
	CRect rectLocationRealPosition = prj.m_MapInformationManager.FindRealPosition( byLocationID );
	FLOAT fLocationRealPositionX1 = static_cast< FLOAT >( rectLocationRealPosition.left );
	FLOAT fLocationRealPositionY1 = static_cast< FLOAT >( rectLocationRealPosition.top );
	FLOAT fLocationRealPositionX2 = static_cast< FLOAT >( rectLocationRealPosition.right );
	FLOAT fLocationRealPositionY2 = static_cast< FLOAT >( rectLocationRealPosition.bottom );
	FLOAT fRealMapWidth = fLocationRealPositionX2 - fLocationRealPositionX1;
	FLOAT fRealMapHeight = fLocationRealPositionY2 - fLocationRealPositionY1;
	vDestination.x = fLocationRealPositionX1 + ( ( fRealMapWidth * ( vSource.x - static_cast< FLOAT >( m_rectRevisedMapPosition.left ) ) ) / static_cast< FLOAT >( m_rectRevisedMapPosition.Width() ) );
	vDestination.y = vSource.y;
	vDestination.z = fLocationRealPositionY2 - ( ( fRealMapHeight * ( vSource.z - static_cast< FLOAT >( m_rectRevisedMapPosition.top ) ) ) / static_cast< FLOAT >( m_rectRevisedMapPosition.Height() ) );
	return vDestination;
}
//-----------------------------------------------------------------------------
const CRect& CWndMapEx::ReviseScriptRectInformation( CRect& rectDestination, 
													 const CRect& rectSource )
{
	// 스크립트의 원본 위치 데이터는 모두 1280 * 960 해상도를 기준으로 작성되어 있음
	// 새로운 맵 시스템에서는 맵 크기를 마음대로 변경할 수 있으므로, 그에 맞게끔 보정 계산을 해서 보정된 위치 데이터를 사용해야 함
	FLOAT fRevisedPositionXRatio = static_cast< FLOAT >( rectSource.left ) / SOURCE_MAP_SIZE_X * static_cast< FLOAT >( m_rectRevisedMapPosition.Width() );
	FLOAT fRevisedPositionYRatio = static_cast< FLOAT >( rectSource.top ) / SOURCE_MAP_SIZE_Y * static_cast< FLOAT >( m_rectRevisedMapPosition.Height() );
	FLOAT fRevisedWidthRatio = static_cast< FLOAT >( rectSource.Width() ) * m_fRevisedMapSizeRatio;
	FLOAT fRevisedHeightRatio = static_cast< FLOAT >( rectSource.Height() ) * m_fRevisedMapSizeRatio;
	rectDestination.left = FtoInt( static_cast< FLOAT >( m_rectRevisedMapPosition.left ) + fRevisedPositionXRatio );
	rectDestination.top = FtoInt( static_cast< FLOAT >( m_rectRevisedMapPosition.top ) + fRevisedPositionYRatio );
	rectDestination.right = rectDestination.left + FtoInt( fRevisedWidthRatio );
	rectDestination.bottom = rectDestination.top + FtoInt( fRevisedHeightRatio );
	return rectDestination;
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::IsPositionInSelectedMapArea( const D3DXVECTOR3& vPosition )
{
	// 타운과 지역이 겹칠수 있기 때문에 타운 검사후 지역검사까지 2번 검사.
	// 타운안에 있지만 선택한곳(m_bySelectedMapLocationID)이 타운을 포함하는 지역이라면 TRUE가 되어야 한다.
	BYTE byLocationID = GetMapArea( vPosition, FALSE, TRUE );
	if( m_bySelectedMapLocationID != CONT_NODATA && byLocationID != m_bySelectedMapLocationID )
	{
		byLocationID = GetMapArea( vPosition, FALSE, FALSE );
		if( m_bySelectedMapLocationID != CONT_NODATA && byLocationID != m_bySelectedMapLocationID )
			return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
BYTE CWndMapEx::GetMapArea( const D3DXVECTOR3& vPosition, BOOL bTownFirst, BOOL bTownOnly )
{
	//bTownFirst : TRUE 가장 작은단위 부터 검출함, TRUE일경우 bCity는 무시.
	//bTownOnly는 bTownFirst가 FALSE일경우만 이용.
	//bTownOnly : TRUE 타운만 검색, FALSE 지역검색

	//SUNOTE: WI_WORLD_MADRIGAL를 제외한 전채맵 추가될때마다 추가해줘야 함. 

	BYTE byLocationID( CONT_NODATA );

	if( !g_pPlayer )
		return 0;

	CWorld* pWorld = g_pPlayer->GetWorld();
	if( pWorld )
	{
		byLocationID = GetMapLocationIDFromWorldID( pWorld->GetID() );
		if( CONT_NODATA != byLocationID )
			return byLocationID;
	}

	if( bTownFirst )
	{
		//타운이 현재 가장 작은 검출단위로, 타운먼저 검색후 지역검색 ex)타운안에서 전채맵을 오픈하면 마을지도가 먼져 나오게됨
		byLocationID = CContinent::GetInstance()->GetTown( vPosition );
		if( byLocationID == CONT_NODATA )
			byLocationID = CContinent::GetInstance()->GetContinent( vPosition );
	}
	else
	{
		if( bTownOnly )
			byLocationID = CContinent::GetInstance()->GetTown( vPosition );
		else byLocationID = CContinent::GetInstance()->GetContinent( vPosition );
	}

	return byLocationID;
}
//-----------------------------------------------------------------------------
void CWndMapEx::CalculateMaximumWindowTileLength( void )
{
	FLASSERT( ( m_nMinimumWindowTileWidth < g_Option.m_nResWidth ) && ( m_nMinimumWindowTileHeight < g_Option.m_nResHeight ) );
	if( g_WndMng.m_pWndTaskBar )
	{
		m_nWndTaskBarHeight = g_WndMng.m_pWndTaskBar->GetWindowRect().Height();
	}

	int nMaximumWindowTileNumberX = MINIMUM_WINDOW_TILE_NUMBER_X;
	while( ( WINDOW_TILE_TEXTURE_SIZE_XY * ( nMaximumWindowTileNumberX + 1 ) ) < g_Option.m_nResWidth )
	{
		++nMaximumWindowTileNumberX;
	}
	m_nMaximumWindowTileWidth = WINDOW_TILE_TEXTURE_SIZE_XY * nMaximumWindowTileNumberX;

	int nMaximumWindowTileNumberY = MINIMUM_WINDOW_TILE_NUMBER_Y;
	while( ( WINDOW_TILE_TEXTURE_SIZE_XY * ( nMaximumWindowTileNumberY + 1 ) ) < ( g_Option.m_nResHeight - m_nWndTaskBarHeight ) )
	{
		++nMaximumWindowTileNumberY;
	}
	m_nMaximumWindowTileHeight = WINDOW_TILE_TEXTURE_SIZE_XY * nMaximumWindowTileNumberY;
}

//_SUN_RENDER_QUESTEMOTICON
//-----------------------------------------------------------------------------
BOOL CWndMapEx::ScanQuest( )
{
	if( _GetContentState( CT_RENDER_QUESTEMOTICON ) == CS_VER1 )
		return FALSE;

	//모든 퀘스트에 대하여, 시작-진행-완료대기 인 퀘스트를 수집
	//레벨이 변화하면 불려야 한다.
	
	QuestProp* pQuestProp = NULL;
	int nTotalSize = prj.m_aPropQuest.GetSize();
	for( int i = 0; i < nTotalSize; ++i )
	{
		pQuestProp = (QuestProp*)prj.m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
			//flyff에 존재하는 npc인지 체크, 퀘스트가 존재하지만 주체가 없을경우는 현재 동작안하는 퀘스트로 봐야함.
			LPCHARACTER pCharacter = prj.GetCharacter( pQuestProp->m_szCharacterKey );
			if( NULL == pCharacter )
				continue;

			//우선순위 : 완료 > 신규 > 진행
			int nQuest = pQuestProp->m_wId;
			QUEST* pQuest = g_pPlayer->FindQuest( nQuest);
			BOOL bComplete = g_pPlayer->IsCompleteQuest( nQuest );
			BOOL bProcessing = FALSE;

			if( pQuest && bComplete == FALSE && pQuest->m_nState != QS_END )	//진행중이나, 완료인지는 추가검사 필요
			{
				if( __IsEndQuestCondition( g_pPlayer, nQuest ) )
				{
					//완료됨.
					m_mapMyQuestForEmoticon.insert( std::map<int, int>::value_type( nQuest, eState_Complete ) );
					continue;
				}
				else 
				{
					//진행중
					bProcessing = TRUE;
				//	m_mapQuestEmoData.insert( std::map<int, int>::value_type( nQuest, 2 ) );
				//	continue;
				}
			}
			
			if( pQuest == NULL && bComplete == FALSE )
			{
				//시작조건에 부합하는가?
				if( __IsBeginQuestCondition( g_pPlayer, nQuest ) ) 
				{
					//시작가능
					m_mapMyQuestForEmoticon.insert( std::map<int,int>::value_type( nQuest, eState_PossibleStart ) );
					continue;
				}
			}
			
			if( bProcessing )
			{
				//진행중
				m_mapMyQuestForEmoticon.insert( std::map<int,int>::value_type( nQuest, eState_Progress ) );
				continue;
			}
		
		}
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 월드맵 아이디로 지역 맵 아이디를 얻어온다
// param	: 월드맵 아이디
// return	: 지역맵 아이디
//---------------------------------------------------------------------------------------------
BYTE CWndMapEx::GetMapLocationIDFromWorldID( const DWORD dwWorldID )
{
	switch( dwWorldID )
	{
		case WI_INSTANCE_OMINOUS :			return DUNGEON_OMINOUS01;
		case WI_INSTANCE_OMINOUS_1 :		return DUNGEON_OMINOUS02;
		case WI_INSTANCE_DREADFULCAVE :		return DUNGEON_DREADFULCAVE;
		case WI_INSTANCE_BEHAMAH :			return DUNGEON_BEHEMOTH;
		case WI_WORLD_KEBARAS:				return CASHAREA_ASRIA;
		case WI_WORLD_CISLAND:				return CASHAREA_CORALICELAND;
		case WI_WORLD_RARTESIA:				return CASHAREA_RARTESIA;
		case WI_INSTANCE_RUSTIA:			return DUNGEON_RUSTIA_1;
		case WI_INSTANCE_RUSTIA_1:			return DUNGEON_RUSTIA_2;
		case WI_INSTANCE_KALGAS:			return DUNGEON_KALGAS;
		case WI_DUNGEON_FL_MAS:				return DUNGEON_MASDUNGEON;		
		case WI_DUNGEON_DA_DK:				return DUNGEON_DEKANES;			
		case WI_DUNGEON_SA_TA:				return DUNGEON_IBLESS;

		case WI_INSTANCE_UPRESIA:			return DUNGEON_UPRESIA;		
		case WI_INSTANCE_UPRESIA_1:			return DUNGEON_UPRESIA_1;
		case WI_INSTANCE_HERNEOS:			return DUNGEON_HERNEOS;
		case WI_INSTANCE_HERNEOS_1:			return DUNGEON_HERNEOS_1;
		case WI_INSTANCE_SANPRES:			return DUNGEON_SANPRES;
		case WI_INSTANCE_SANPRES_1:			return DUNGEON_SANPRES_1;

		case WI_WORLD_HEAVEN01:				return DUNGEON_WdHeaven01;
		case WI_WORLD_HEAVEN02:				return DUNGEON_WdHeaven02;
		case WI_WORLD_HEAVEN03:				return DUNGEON_WdHeaven03;
		case WI_WORLD_HEAVEN04:				return DUNGEON_WdHeaven04;
		case WI_WORLD_HEAVEN05:				return DUNGEON_WdHeaven05;
		case WI_DUNGEON_VOLCANE:			return DUNGEON_VOLCANE;			// 볼케인
		case WI_WORLD_DARKRARTESIA:			return CASHAREA_DARKRARTESIA;	// 어둠의 라르테시아

		case WI_INSTANCE_CONTAMINTRAILS:	return DUNGEON_ContaminatedTrails;	// 오염된 리시스의 산책길

		default:
			break;
	}	

	return CONT_NODATA;
}



//---------------------------------------------------------------------------------------------
// 퀘스트의 목표 지점의 위치를 얻어낸다.
// param	: 위치기 담길 백터( OUT ), 퀘스트 상태, 퀘스트 속성
// return	: 위치 얻기 성공 / 실패
//---------------------------------------------------------------------------------------------
bool CWndMapEx::GetQuestEmoticonPosition( D3DXVECTOR3& vPos, DWORD& dwWorldID, const int nQuestState, const QuestProp* const pQuestProp )
{
	switch( nQuestState )
	{
	case eState_PossibleStart:		
		{
			vPos.x		= pQuestProp->m_kBeginNpcData.m_fGoalPositionX;
			vPos.z		= pQuestProp->m_kBeginNpcData.m_fGoalPositionZ;
			dwWorldID	= pQuestProp->m_kBeginNpcData.m_dwGoalWorldID;
		}
		break;
	
	case eState_Progress:		
	case eState_Complete:		
		{
			vPos.x		= pQuestProp->m_MeetCharacterGoalData.m_fGoalPositionX;
			vPos.z		= pQuestProp->m_MeetCharacterGoalData.m_fGoalPositionZ;
			dwWorldID	= pQuestProp->m_kBeginNpcData.m_dwGoalWorldID;
		}
		break;
	
	default:
		{
			FLASSERT( 0 );
			return false;
		}
		break;
	}


	if( false == ( 0.0f < vPos.x ) )
		return false;

	if( false == ( 0.0f < vPos.z ) )
		return false;

	if( vPos.x == 0.0f && vPos.z == 0.0f )
		return false;

	return true;
}
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 퀘스트 NPC가 현재 선택 된 맵에 존재하는가 ?
// 설명		: 이미 그려야 할 퀘스트는 ScanQuest에서 걸러져서 Container에 담겨져 있다...
//			: 별로 좋지 않게 담겨 있음.	
//---------------------------------------------------------------------------------------------
bool CWndMapEx::IsQuestNpcInSelectMapArea( const D3DXVECTOR3 vPos, const DWORD dwGoalWorldID )
{
	BYTE byLocationID = GetMapLocationIDFromWorldID( dwGoalWorldID );

	if( m_bySelectedMapLocationID == CONT_NODATA )
	{
		if( CONT_NODATA != byLocationID )
			return false;
		else
			return true;
	}
	else
	{
		if( CONT_NODATA != byLocationID )
		{
			if( m_bySelectedMapLocationID == byLocationID )
				return true;
			else
				return false;
		}

		byLocationID = CContinent::GetInstance()->GetTown( vPos );

		if( CONT_NODATA != byLocationID )
		{
			if( m_bySelectedMapLocationID == byLocationID )
				return true;
			else
			{
				byLocationID = CContinent::GetInstance()->GetContinent( vPos );

				if( CONT_NODATA != byLocationID )
				{
					if( m_bySelectedMapLocationID == byLocationID )
						return true;
					else
						return false;
				}
			}
		}
	}

	return false;
}
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 월드맵에 퀘스트 마크 그리기
// 설명		: 이미 그려야 할 퀘스트는 ScanQuest에서 걸러져서 Container에 담겨져 있다...
//			: 별로 좋지 않게 담겨 있음.	
//---------------------------------------------------------------------------------------------
void CWndMapEx::RenderQuestEmoticon( C2DRender* p2DRender )
{
	if( _GetContentState( CT_RENDER_QUESTEMOTICON ) == CS_VER1 )
		return;

	if( m_eConstructionMode != ConstructionMode::NORMAL )
		return;

	int nHead( NULL );
	int nOffset( NULL );
	int nTexIndex( NULL );

	for( std::map<int,int>::iterator iter = m_mapMyQuestForEmoticon.begin(); iter != m_mapMyQuestForEmoticon.end(); ++iter )
	{
		int nQuest( iter->first );

		QuestProp* pQuestProp = (QuestProp*)prj.m_aPropQuest.GetAt( nQuest );
		if( NULL == pQuestProp )
			continue;

		LPCHARACTER pCharacter = prj.GetCharacter( pQuestProp->m_szCharacterKey );
		if( NULL == pCharacter )
			continue;

		if( FALSE == pCharacter->bOutput )
			continue;

		int				nQuestState( iter->second );
		CPoint			pointRevisedNPCPosition( 0, 0 );

		DWORD			dwGoalWorldID( NULL );
		D3DXVECTOR3		vPosition( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3		vConvertedPosition( 0.0f, 0.0f, 0.0f );

		if( false == GetQuestEmoticonPosition( vPosition, dwGoalWorldID, nQuestState, pQuestProp ) )
			continue;

		if( false == IsQuestNpcInSelectMapArea( vPosition, dwGoalWorldID ) )
			continue;

		ConvertPosition( vConvertedPosition, vPosition, m_bySelectedMapLocationID );

		FLOAT fRevisedSizeRatio = CalculateQuestIconStartPosition(	pointRevisedNPCPosition, 
																	vConvertedPosition.x, 
																	vConvertedPosition.z, 
																	m_pNPCPositionTexture, 
																	NPC_POSITION_TEXTURE_SIZE_RATIO );

		nTexIndex			= nQuestState + 23;
		nHead				= _GetHeadQuest( nQuest );
		nOffset				= _CalcQuestEmotionOffset( nHead, 3 );
		nTexIndex			= nTexIndex + nOffset;
		fRevisedSizeRatio	= fRevisedSizeRatio * 1.6f;

		m_kTexPackQuest.Render( p2DRender, pointRevisedNPCPosition, nTexIndex, 255, fRevisedSizeRatio, fRevisedSizeRatio ); // * 1.6f, fRevisedSizeRatio * 1.6f );

#ifdef _DEBUG
		CString str;
		str.Format( "%d", nQuest );
		p2DRender->TextOut( pointRevisedNPCPosition.x, pointRevisedNPCPosition.y, str, WNDFONTCOLOR_WHITE, WNDFONTCOLOR_BLACK );
#endif
	}
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
