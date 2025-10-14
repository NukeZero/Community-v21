#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapInformationManager.h"
#include "defineMapComboBoxData.h"
#include "Project.h"
#include "ContinentDef.h"
#include "defineText.h"
#include "Continent.h"
#include "ResData.h"
//-----------------------------------------------------------------------------
CMapInformationManager::CMapInformationManager( void ) : 
m_pPCArrowTexture( NULL ), 
m_pPartyPCArrowTexture( NULL ), 
m_pTeleportationPositionTexture( NULL ), 
m_pDestinationPositionTexture( NULL ), 
m_pNPCPositionTexture( NULL ), 
m_pUserMarkPositionTexture( NULL ), 
m_pHyperlinkMarkPositionTexture( NULL ), 
m_pRainbowNPCInformationPack( NULL ),
m_pTexTeleport_General( NULL ),
m_pTexTeleport_User( NULL )
{
}
//-----------------------------------------------------------------------------
CMapInformationManager::~CMapInformationManager( void )
{
	DeleteAllMapInformation();
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadMapInformationData( void )
{
	LoadMapIconTexture();
	LoadPropMapComboBoxData();
	LoadMapMonsterInformationPack();
	LoadRainbowNPCInformationPack();

	return TRUE;
}
//-----------------------------------------------------------------------------
void CMapInformationManager::DeleteAllMapInformation( void )
{
	DeleteAllUserMarkPositionInfo();

	SAFE_DELETE( m_pRainbowNPCInformationPack );

	for( std::map< BYTE, CRect* >::iterator Iterator = m_RealPositionRectMap.begin(); Iterator != m_RealPositionRectMap.end(); ++Iterator )
	{
		SAFE_DELETE( Iterator->second );
	}
	m_RealPositionRectMap.clear();

	for( MapMonsterInformationPackMap::iterator Iterator = m_MapMonsterInformationPackMap.begin(); Iterator != m_MapMonsterInformationPackMap.end(); ++Iterator )
	{
		SAFE_DELETE( Iterator->second );
	}
	m_MapMonsterInformationPackMap.clear();

	for( MapComboBoxDataVector::iterator Iterator = m_MapCategoryVector.begin(); Iterator != m_MapCategoryVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_MapCategoryVector.clear();

	for( MapComboBoxDataVector::iterator Iterator = m_MapNameVector.begin(); Iterator != m_MapNameVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_MapNameVector.clear();

	for( MapComboBoxDataVector::iterator Iterator = m_NPCNameVector.begin(); Iterator != m_NPCNameVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_NPCNameVector.clear();
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetPCArrowTexture( void )
{
	return m_pPCArrowTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetPartyPCArrowTexture( void )
{
	return m_pPartyPCArrowTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetTeleportationPositionTexture( void )
{
	return m_pTeleportationPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetDestinationPositionTexture( void )
{
	return m_pDestinationPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetNPCPositionTexture( void )
{
	return m_pNPCPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetUserMarkPositionTexture( void )
{
	return m_pUserMarkPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetHyperlinkMarkPositionTexture( void )
{
	return m_pHyperlinkMarkPositionTexture;
}
//-----------------------------------------------------------------------------
MapComboBoxDataVector* CMapInformationManager::GetMapCategoryVector( void )
{
	return &m_MapCategoryVector;
}
//-----------------------------------------------------------------------------
MapComboBoxDataVector* CMapInformationManager::GetMapNameVector( void )
{
	return &m_MapNameVector;
}
//-----------------------------------------------------------------------------
MapComboBoxDataVector* CMapInformationManager::GetNPCNameVector( void )
{
	return &m_NPCNameVector;
}
CTexture* const CMapInformationManager::GetTeleportTexture_General( void )
{
	return m_pTexTeleport_General;
}
CTexture* const CMapInformationManager::GetTeleportTexture_User( void )
{
	return m_pTexTeleport_User;
}
//-----------------------------------------------------------------------------
CMapMonsterInformationPack* CMapInformationManager::FindMapMonsterInformationPack( DWORD dwFirstKey )
{
	MapMonsterInformationPackMap::const_iterator Iterator = m_MapMonsterInformationPackMap.find( dwFirstKey );
	if( Iterator != m_MapMonsterInformationPackMap.end() )
	{
		return Iterator->second;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
CRect& CMapInformationManager::FindRealPosition( BYTE byFirstKey )
{
	static CRect rectNull( -1, -1, -1, -1 );
	std::map< BYTE, CRect* >::const_iterator Iterator = m_RealPositionRectMap.find( byFirstKey );
	if( Iterator != m_RealPositionRectMap.end() )
	{
		return *( Iterator->second );
	}
	return rectNull;
}
//-----------------------------------------------------------------------------
CRainbowNPCInformationPack* CMapInformationManager::GetRainbowNPCInformationPack( void )
{
	return m_pRainbowNPCInformationPack;
}
//-----------------------------------------------------------------------------
DWORD CMapInformationManager::InsertUserMarkPositionInfo( const CString& strName, FLOAT fPositionX, FLOAT fPositionY )
{
	if( static_cast< int >( m_UserMarkPositionInfoList.size() ) >= USER_MARK_MAX_SIZE )
	{
		g_WndMng.PutString( 
			prj.GetText( TID_ERROR_MAP_EX_MARK_MAX_NUMBER ), 
			NULL, 
			prj.GetTextColor( TID_ERROR_MAP_EX_MARK_MAX_NUMBER ) );

		return FALSE;
	}

	static DWORD dwID = 1;
	CUserMarkPositionInfo* pUserMarkPositionInfo = new CUserMarkPositionInfo;
	pUserMarkPositionInfo->SetID( dwID );
	pUserMarkPositionInfo->SetName( strName );
	pUserMarkPositionInfo->SetPositionX( fPositionX );
	pUserMarkPositionInfo->SetPositionY( fPositionY );
	m_UserMarkPositionInfoList.push_back( pUserMarkPositionInfo );
	++dwID;

	return ( dwID - 1 ); //TRUE;
}
//-----------------------------------------------------------------------------
void CMapInformationManager::DeleteUserMarkPositionInfo( DWORD dwID )
{
	for( UserMarkPositionInfoList::iterator Iterator = m_UserMarkPositionInfoList.begin(); Iterator != m_UserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		if( dwID == pUserMarkPositionInfo->GetID() )
		{
			SAFE_DELETE( pUserMarkPositionInfo );
			m_UserMarkPositionInfoList.erase( Iterator );

			break;
		}
	}
}
//-----------------------------------------------------------------------------
CUserMarkPositionInfo* const CMapInformationManager::FindUserMarkPositionInfo( DWORD dwID )
{
	for( UserMarkPositionInfoList::iterator Iterator = m_UserMarkPositionInfoList.begin(); Iterator != m_UserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* const pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		if( dwID == pUserMarkPositionInfo->GetID() )
		{
			return pUserMarkPositionInfo;
		}
	}
	return NULL;
}
//-----------------------------------------------------------------------------
void CMapInformationManager::DeleteAllUserMarkPositionInfo( void )
{
	for( UserMarkPositionInfoList::iterator Iterator = m_UserMarkPositionInfoList.begin(); Iterator != m_UserMarkPositionInfoList.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_UserMarkPositionInfoList.clear();
}
//-----------------------------------------------------------------------------
UserMarkPositionInfoList& CMapInformationManager::GetUserMarkPositionInfoList( void )
{
	return m_UserMarkPositionInfoList;
}

CMapComboBoxData* CMapInformationManager::FindMapComboBoxData( const DWORD worldID )
{
	for( MapComboBoxDataVector::iterator iter = m_MapNameVector.begin(); iter != m_MapNameVector.end(); ++iter )
	{
		CMapComboBoxData* pData = *iter;
		if( pData->GetID() == worldID )
			return pData;
	}

	return NULL;
}

BOOL CMapInformationManager::CanOpenMap( const DWORD worldID, const DWORD wndID, BOOL bNeedConverting )
{
	if( g_pPlayer == NULL )
		return FALSE;

	DWORD wndWorldID = worldID;
	//찾을수 없는경우는 맵네임 속한것이 아닐뿐 카테고리거나 다은종류 일수있기때문에 열수있음으로 판단.
	if( bNeedConverting )
		 wndWorldID = CContinent::GetInstance()->GetMapComboID_byWorldID( worldID, g_pPlayer->GetPos() );

	CMapComboBoxData* pData = FindMapComboBoxData( wndWorldID );
	if( !pData )
		return TRUE;

	//현재 던젼과 유료맵의 경우 버프가 유효하다면 열어준다.
	if( MCD_DUNGEON == pData->GetParentID() )
	{
		return ( TRUE == g_pPlayer->HasBuffByIk3( IK3_MAPOPEN_DUNGEON ) );
	}
	else if( MCD_CASHAREA == pData->GetParentID() )
	{
		if( APP_NAVIGATOR == wndID )
			return TRUE; //유료맵인경우 navigator는 무조건 오픈

		return ( TRUE == g_pPlayer->HasBuffByIk3( IK3_MAPOPEN_DUNGEON ) );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadMapIconTexture( void )
{
	CTexture* pPCArrowTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ImgMapArrow.bmp" ) ), 0xffff00ff );
	if( pPCArrowTexture )
	{
		m_pPCArrowTexture = pPCArrowTexture;
	}

	CTexture* pPartyPCArrowTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ImgMapArrowParty.bmp" ) ), 0xffff00ff );
	if( pPartyPCArrowTexture )
	{
		m_pPartyPCArrowTexture = pPartyPCArrowTexture;
	}

	CTexture* pTeleportationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtTeleport.bmp"), 0xffff00ff );
	if( pTeleportationPositionTexture )
	{
		m_pTeleportationPositionTexture = pTeleportationPositionTexture;
	}

	CTexture* pDestinationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ButtDestination.bmp"), 0xffff00ff );
	if( pDestinationPositionTexture )
	{
		m_pDestinationPositionTexture = pDestinationPositionTexture;
	}

	CTexture* pNPCPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"MapNPCPositionMark.bmp"), 0xffff00ff );
	if( pNPCPositionTexture )
	{
		m_pNPCPositionTexture = pNPCPositionTexture;
	}

	CTexture* pUserMarkPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"UserMark.bmp"), 0xffff00ff );
	if( pUserMarkPositionTexture )
	{
		m_pUserMarkPositionTexture = pUserMarkPositionTexture;
	}

	CTexture* pHyperlinkMarkPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "HyperlinkUserMark.bmp"), 0xffff00ff );
	if( pHyperlinkMarkPositionTexture )
	{
		m_pHyperlinkMarkPositionTexture = pHyperlinkMarkPositionTexture;
	}

	CTexture* pTexTeleport_General = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "TeleportMark01.tga"), 0xffff00ff );
	if( pTexTeleport_General )
	{
		m_pTexTeleport_General = pTexTeleport_General;
	}

	CTexture* pTexTeleport_User = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "TeleportMark02.tga"), 0xffff00ff );
	if( pTexTeleport_User )
	{
		m_pTexTeleport_User = pTexTeleport_User;
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadPropMapComboBoxData( void )
{
	//gmpbigsun: debug에서는 이함수가 한번이상 불리므로, 초기화 시켜줘야한다.
	SeqDeleter< MapComboBoxDataVector > ( ) ( m_MapCategoryVector );
	SeqDeleter< MapComboBoxDataVector > ( ) ( m_MapNameVector );
	SeqDeleter< MapComboBoxDataVector > ( ) ( m_NPCNameVector );

	CScript script;
	if( script.Load( _T( "propMapComboBoxData.inc" ) ) == FALSE )
	{
		return FALSE;
	}

	DWORD dwMapComboBoxDataID = script.GetNumber(); // ID;
	while( script.tok != FINISHED )
	{
		script.GetToken(); // {
		CMapComboBoxData::Category eMapComboBoxCategory = static_cast< CMapComboBoxData::Category >( MCC_MAP_CATEGORY );
		CString strMapComboBoxTitle = _T( "" );
		CString strMapPictureFileName = _T( "" );
		CRect rectRealPosition( 0, 0, 0, 0 );
		CPoint pointNPCPosition( -1, -1 );
		CString strMapMonsterInformationFileName = _T( "" );
		BYTE byLocationID = CONT_NODATA;
		DWORD dwMapComboBoxDataParentID = MCD_NONE;
		int nBlock = 1;
		while( nBlock > 0 && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == _T( "{" ) )
			{
				++nBlock;
			}
			else if( script.Token == _T( "}" ) )
			{
				--nBlock;
			}
			else if( script.Token == _T( "SetCategory" ) )
			{
				script.GetToken(); // (
				eMapComboBoxCategory = static_cast< CMapComboBoxData::Category >( script.GetNumber() );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetTitle" ) )
			{
				script.GetToken(); // (
				strMapComboBoxTitle = prj.GetLangScript( script );
			}
			else if( script.Token == _T( "SetPictureFile" ) )
			{
				script.GetToken(); // (
				strMapPictureFileName = prj.GetLangScript( script );
			}
			else if( script.Token == _T( "SetRealPositionRect" ) )
			{
				script.GetToken(); // (
				rectRealPosition.left = script.GetNumber();
				script.GetToken(); // ,
				rectRealPosition.top = script.GetNumber();
				script.GetToken(); // ,
				rectRealPosition.right = script.GetNumber();
				script.GetToken(); // ,
				rectRealPosition.bottom = script.GetNumber();
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetMonsterInformationFile" ) )
			{
				script.GetToken(); // (
				strMapMonsterInformationFileName = prj.GetLangScript( script );
			}
			else if( script.Token == _T( "SetLocationID" ) )
			{
				script.GetToken(); // (
				byLocationID = static_cast< BYTE >( script.GetNumber() );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetNPCPosition" ) )
			{
				script.GetToken(); // (
				pointNPCPosition.x = script.GetNumber();
				script.GetToken(); // ,
				pointNPCPosition.y = script.GetNumber();
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetParentID" ) )
			{
				script.GetToken(); // (
				dwMapComboBoxDataParentID = script.GetNumber();
				script.GetToken(); // )
				script.GetToken(); // ;
			}
		}

		CMapComboBoxData* pMapComboBoxData = new CMapComboBoxData( dwMapComboBoxDataID, eMapComboBoxCategory, strMapComboBoxTitle );
		pMapComboBoxData->SetPictureFileName( strMapPictureFileName );
		
		if( eMapComboBoxCategory == MCC_MAP_NAME )
		{
			//gmpbigsun( 20110712 ) : fixed leak
			std::map<BYTE, CRect*>::iterator iter = m_RealPositionRectMap.find( byLocationID );
			if( iter != m_RealPositionRectMap.end() )
			{
				SAFE_DELETE( iter->second );
			}

			CRect* pRealPositionRect = new CRect( rectRealPosition );
			m_RealPositionRectMap[ byLocationID ] = pRealPositionRect;
		}
		pMapComboBoxData->SetMapMonsterInformationFileName( strMapMonsterInformationFileName );
		pMapComboBoxData->SetLocationID( byLocationID );
		pMapComboBoxData->SetNPCPosition( pointNPCPosition );
		pMapComboBoxData->SetParentID( dwMapComboBoxDataParentID );

		switch( eMapComboBoxCategory )
		{
		case MCC_MAP_CATEGORY:
			{
				m_MapCategoryVector.push_back( pMapComboBoxData );
				break;
			}
		case MCC_MAP_NAME:
			{
				m_MapNameVector.push_back( pMapComboBoxData );
				break;
			}
		case MCC_NPC_NAME:
			{
				m_NPCNameVector.push_back( pMapComboBoxData );
				break;
			}
		}
		dwMapComboBoxDataID = script.GetNumber(); // ID
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadMapMonsterInformationPack( void )
{
	for( MapComboBoxDataVector::iterator Iterator = m_MapNameVector.begin(); Iterator != m_MapNameVector.end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( pMapComboBoxData->GetMapMonsterInformationFileName() == _T( "" ) )
		{
			continue;
		}

		CMapMonsterInformationPack* pMapMonsterInformationPack = new CMapMonsterInformationPack;
		if( pMapMonsterInformationPack->LoadScript( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),pMapComboBoxData->GetMapMonsterInformationFileName() ) ) == FALSE )
		{
			SAFE_DELETE( pMapMonsterInformationPack );
			continue;
		}

		m_MapMonsterInformationPackMap[ pMapComboBoxData->GetID() ] = pMapMonsterInformationPack;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadRainbowNPCInformationPack( void )
{
	m_pRainbowNPCInformationPack = new CRainbowNPCInformationPack;
	m_pRainbowNPCInformationPack->LoadScript( MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "texMapRainbow_NPC.inc" ) ) );

	return TRUE;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM