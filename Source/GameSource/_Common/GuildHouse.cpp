// GuildHouse.cpp: implementation of the CGuildHouse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GuildHouse.h"
#include "defineText.h"

#ifdef __CLIENT
#include "MsgHdr.h"
#include "../Neuz/DPClient.h"
#include "WndHousing.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __WORLDSERVER
#include "WorldMng.h"
extern CWorldMng g_WorldMng;
#include "guild.h"
extern CGuildMng g_GuildMng;
#include "../WorldServer/user.h"
#include "../WorldServer/npchecker.h"
#include "../worldserver/FLMapGrid.h"
#include "../worldserver/LinkMap.h"
#endif // __WORLDSERVER

#ifdef __DBSERVER
#include "../DatabaseServer/GuildHouseDBCtrl.h"

#ifdef __GUILD_HOUSE_MIDDLE
#include "guild.h"
extern CGuildMng g_GuildMng;
#include "post.h"
#include "../DatabaseServer/dptrans.h"
#include "../DatabaseServer/DPCoreSrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;
#endif // __GUILD_HOUSE_MIDDLE

#endif // __DBSERVER

//////////////////////////////////////////////////////////////////////
// CGuildHouseBase::GUILDHOUSE_FURNITURE_INFO
//////////////////////////////////////////////////////////////////////
CGuildHouseBase::GuildHouse_Furniture_Info::GuildHouse_Furniture_Info( DWORD dwII , BOOL bS, D3DXVECTOR3 v, float fA, time_t tK )
:dwItemId( dwII ), bSetup( bS ), vPos( v ), fAngle( fA ), objId( NULL_ID ), tKeepTime( time_null() + tK )
#ifdef __DBSERVER
, nSeqNum( 0 )
#endif // __DBSERVER
{
}

void CGuildHouseBase::GuildHouse_Furniture_Info::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << dwItemId << bSetup << vPos << fAngle << objId << nSeqNum;
		if( tKeepTime > 0 )
			ar << tKeepTime - time_null();
		else
			ar << static_cast<time_t>( 0 );
	}
	else	
	{
		ar >> dwItemId >> bSetup >> vPos >> fAngle >> objId >> nSeqNum;
		ar >> tKeepTime;
		if( tKeepTime > 0 )
			tKeepTime += time_null();
	}
}


//////////////////////////////////////////////////////////////////////
// CGuildHouseBase
//////////////////////////////////////////////////////////////////////
CGuildHouseBase::CGuildHouseBase( DWORD dwGuildId, DWORD dwWorldId )
:m_dwGuildId( dwGuildId ), m_dwWorldId( dwWorldId ), m_tUpkeepTime( 0 )
#ifdef __GUILD_HOUSE_MIDDLE
, m_dwGHIndex( 0 ), m_dwNpcIndex( 0 ), m_nAreaIndex( 0 ), m_nGHLevel( 0 )
#endif // __GUILD_HOUSE_MIDDLE
{
#ifdef __WORLDSERVER
	m_dwWaitDBAckPlayerId = NULL_ID;
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_dwSelectedObjID = NULL_ID;
	m_nExtraExp = 0;
	m_nWndDeployingIndex  = -1;
#endif
#ifdef __GUILD_HOUSE_MIDDLE
	memset( m_szGHComment, 0, sizeof( m_szGHComment ) );
#endif // __GUILD_HOUSE_MIDDLE
}

CGuildHouseBase::~CGuildHouseBase()
{
	Clear();
}

void CGuildHouseBase::Clear()
{
#ifdef __WORLDSERVER
	DestroyGuildHouseRoom();
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo.at( i ).bSetup )
			ResetDSTFunriture( g_xSpecManager->GetSpecItem( m_vecFntInfo.at( i ).dwItemId ) );
	}
#endif // __WORLDSERVER
	m_vecFntInfo.clear();
}

#ifdef __CLIENT
CGuildHouseBase* CGuildHouseBase::GetInstance()
{
	static CGuildHouseBase sGHB( NULL_ID, NULL_ID );
	return & sGHB;
}

void CGuildHouseBase::SendClientToWorld( int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN );
	ar << nPacketType << nIndex;
	gfi.Serialize( ar );
	SEND( ar, &g_DPlay, DPID_SERVERPLAYER );
}
/*
struct IsSameID : public binary_function< GH_Fntr_Info, OBJID, bool > 
{
	bool operator( ) ( const GH_Fntr_Info& kInfo, OBJID id_ ) const 
	{
		return ( kInfo.objId == id_ );
	}
};*/

GH_Fntr_Info*  CGuildHouseBase::Find( OBJID objID_ )
{
	VECFurnitureIter finder = std::find_if(m_vecFntInfo.begin(), m_vecFntInfo.end(), [objID_](const GH_Fntr_Info& kInfo)
		{
			return (kInfo.objId == objID_);
		});

	if (finder != m_vecFntInfo.end())
		return &(*finder);

	return nullptr;
}

int CGuildHouseBase::FindIndex( OBJID objID_ )
{
	for( int i = 0; i < GetFurnitureListSize( ); ++i )
	{
		GH_Fntr_Info* pInfo = GetFurnitureInfoPtr( i );
		FLASSERT( pInfo );

		if( objID_ == pInfo->objId )
		{
			return i;
		}
	}
	
	return -1;
}

void CGuildHouseBase::Setup( const HOUSING_ITEM& kInfo )
{
	GH_Fntr_Info kSendInfo;
	kSendInfo.dwItemId = kInfo.m_nIndex;
	kSendInfo.bSetup = kInfo.m_bDeploy;	
	kSendInfo.vPos = kInfo.m_vPos;
	kSendInfo.fAngle = kInfo.m_fAngle;
	kSendInfo.objId = kInfo.dwItemId;
	kSendInfo.tKeepTime = kInfo.m_dwTime;
	
	SendClientToWorld( GUILDHOUSE_PCKTTYPE_SETUP, kSendInfo, kInfo.m_nSlotIndex );
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
	//m_nExtraExp = 0;		//gmpbigsun: 버블타임 게이지가 초기화되는 현상-_-;생각없이 초기화를 한것 같군...
}

void CGuildHouseBase::Reset( )
{
	// 가구 편집모드를 불러주는 함수 
	GH_Fntr_Info* pInfo = Find( m_dwSelectedObjID );
	if( !pInfo )
		return;

	int slotIndex = FindIndex( m_dwSelectedObjID );
	if( slotIndex < 0 )
		return;

	HOUSING_ITEM kItem;
	kItem.m_bDeploy = pInfo->bSetup;
	kItem.dwItemId = pInfo->objId;
	kItem.m_nIndex = pInfo->dwItemId;
	kItem.m_vPos = pInfo->vPos;
	kItem.m_fAngle = pInfo->fAngle;
	kItem.m_nSlotIndex = slotIndex;

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pInfo->dwItemId );
	FLASSERT( pItemProp );

	GuildDeploy()->LoadToDeploy(pItemProp->dwLinkKind, kItem );

	m_iMode = GUILDHOUSE_PCKTTYPE_RESET;		//패킷을 실제로 보내기전에 모드를 세팅 ( ACQ 후에는 아무 의미없는 값이다 )
}

void CGuildHouseBase::Reset( const HOUSING_ITEM& kInfo )
{
	// 실제로 재설치 패킷을 전송하는 함수 
	GH_Fntr_Info kSendInfo;
	kSendInfo.dwItemId = kInfo.m_nIndex;
	kSendInfo.bSetup = kInfo.m_bDeploy;	
	kSendInfo.vPos = kInfo.m_vPos;
	kSendInfo.fAngle = kInfo.m_fAngle;
	kSendInfo.objId = kInfo.dwItemId;
	kSendInfo.tKeepTime = kInfo.m_dwTime;

	SendClientToWorld( GUILDHOUSE_PCKTTYPE_RESET, kSendInfo, kInfo.m_nSlotIndex );
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::Remove( )
{
	int slotIndex = FindIndex( m_dwSelectedObjID );
	
	if( slotIndex < 0 )
		return;
	
	GH_Fntr_Info* pInfo = Find( m_dwSelectedObjID );
	if( !pInfo )
		return;

	//gmpbigsun: testing
	SendClientToWorld( GUILDHOUSE_PCKTTYPE_REMOVE, *pInfo, slotIndex );
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::ApplyEFTexture( )
{
	CWorld* pWorld = g_WorldMng.Get();
	if( !pWorld )
		return;

	if( !pWorld->IsWorldGuildHouse() )
		return;

	// 벽지, 장판 해체시 복구flag
	BOOL bWallTex = FALSE;
	BOOL bTileTex = FALSE;

	int iMaxSize = GetFurnitureListSize( );
	for( int i = 0; i < iMaxSize; ++i )
	{
		GH_Fntr_Info* pInfo = GetFurnitureInfoPtr( i );
		if( !pInfo )
			continue;
		
		PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( pInfo->dwItemId );
		if( !pProp )
		{
			FLASSERT( 0 );
			continue;
		}

		if( IK2_GUILDHOUSE_FURNITURE != pProp->dwItemKind2 && IK2_GUILDHOUSE_PAPERING != pProp->dwItemKind2 )  
			continue;

		if( pInfo->bSetup )
		{
			if( IK3_WALLPAPER == pProp->dwItemKind3 )
			{
				GuildDeploy()->ChangeWallTex( pProp->szTextFileName );
				bWallTex = TRUE;
			}
					

			if( IK3_CARPET == pProp->dwItemKind3 )
			{
				GuildDeploy()->ChangeTileTex( pProp->szTextFileName );
				bTileTex = TRUE;
			}
		}
	}

		//장판, 벽지가 설치된게 없다면 default로 
	if( !bWallTex )
		GuildDeploy()->ChangeWallTex( );

	if( !bTileTex )
		GuildDeploy()->ChangeTileTex( );
}

void CGuildHouseBase::ResetValues( )
{
	m_dwGuildId = 0;
	m_dwWorldId = 0;
	m_vecFntInfo.clear();

	m_dwSelectedObjID = 0;
	m_iMode = 0;
	m_nExtraExp = 0;
	m_nWndDeployingIndex = 0;
	m_bSetFurnitureChannel = FALSE;
}

#endif // __CLIENT

#ifdef __WORLDSERVER
BOOL CGuildHouseBase::CreateGuildHouseRoom()
{
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( !pWorld ) return FALSE;
		
	if( pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
		return TRUE;

	if( pWorld->CreateLayer( static_cast<int>( m_dwGuildId ) ) )
	{
		for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
		{
			if( m_vecFntInfo.at( i ).bSetup )
			{
				if( SetupFurnitureCtrl( i, m_vecFntInfo.at( i ) ) == TRUE )
				{
					SendWorldToClient( GUILDHOUSE_PCKTTYPE_REFRESH, m_vecFntInfo.at( i ), i );	// OBJID와 남은 시간을 갱신시킨다.
				}
				else
				{
					m_vecFntInfo.at( i ).bSetup = FALSE;
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CGuildHouseBase::DestroyGuildHouseRoom()
{
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( !pWorld || !pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
		return FALSE;

	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo.at( i ).bSetup )
		{
			m_vecFntInfo.at( i ).objId = NULL_ID;
			SendWorldToClient( GUILDHOUSE_PCKTTYPE_REFRESH, m_vecFntInfo.at( i ), i );	// OBJID를 초기화 시키고 남은 시간을 갱신시킨다.
		}
	}

	pWorld->Invalidate( static_cast<int>( m_dwGuildId ) );
	return TRUE;
}

BOOL CGuildHouseBase::IsEnteranceAble( FLWSUser* pUser )
{
	if( m_tUpkeepTime == 0 )	// 유지 기간이 만료되었다.
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_EXPIRATION );
		return FALSE;
	}
	
	if( !IsValidObj( pUser ) || pUser->m_idGuild != m_dwGuildId )
		return FALSE;

	if( CreateGuildHouseRoom() )	// 입장자가 한명도 없어서 방이 생성되지 않은 경우 생성시킨다.
		return TRUE;

	return FALSE;
}

BOOL CGuildHouseBase::EnteranceGuildHouseRoom( FLWSUser* pUser )
{
	if( IsEnteranceAble( pUser ) )	
	{
		pUser->SetRestPointTick();
		return pUser->REPLACE( g_uIdofMulti, m_dwWorldId, GetEnterPos(), REPLACE_NORMAL, static_cast<int>( m_dwGuildId ) );
	}

	return FALSE;
}

void CGuildHouseBase::GoOutGuildHouseRoom( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) && pUser->GetWorld()->GetID() == m_dwWorldId && static_cast< DWORD >( pUser->GetLayer() ) == m_dwGuildId )
		pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer );
}

void CGuildHouseBase::CheckDestroyRoom( FLWSUser* pUser )
{
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
	/*
		int nWidth = pWorld->WORLD_WIDTH * pWorld->m_iMPU;
		int nHeight = pWorld->WORLD_HEIGHT * pWorld->m_iMPU;
		D3DXVECTOR3 vPosCenter( nWidth / 2, 0.0f, nHeight / 2 );
		int nRange = max( nWidth, nHeight ) / 2;
		CObj* pObj;
		FOR_LINKMAP( pWorld, vPosCenter, pObj, nRange, CObj::linkPlayer, static_cast<int>( m_dwGuildId ) )
		{
			if( pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() && pUser != pObj )	// 내가 아닌 다른 사람이 남아있는 경우다.
				return;
		}
		END_LINKMAP
	*/
		if( g_xWSUserManager->HasUserSameWorldnLayer( pUser ) )
			return;

		DestroyGuildHouseRoom();	// 한명도 남아있지 않은 경우 - 방만 파괴시킨다.
	}
}

BOOL CGuildHouseBase::IsAuthority( FLWSUser* pUser, int nPacketType )
{
	if( !IsValidObj( pUser ) || pUser->m_idGuild != m_dwGuildId )	return FALSE;
	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( !pGuild )	return FALSE;

	if( nPacketType == GUILDHOUSE_PCKTTYPE_UPKEEP ) // 유지비는 권한만 있으면 OK!
	{
		if( !pGuild->IsAuthority( pUser->m_idPlayer, PF_GUILDHOUSE_UPKEEP ) )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_TAX_LEVEL );
			return FALSE;
		}
	}
	else 
	{
		if( !pGuild->IsAuthority( pUser->m_idPlayer, PF_GUILDHOUSE_FURNITURE ) )	// 가구 제어 권한 검사(텔레포터 포함)
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_LEVEL );
			return FALSE;
		}
		else if( pUser->GetWorld()->GetID() != m_dwWorldId || static_cast< DWORD >( pUser->GetLayer() ) != m_dwGuildId )	// 현재 유저가 길드하우스 내에 있는가?
			return FALSE;
	}
	
	return TRUE;
}

BOOL CGuildHouseBase::PreCheckPacket( FLWSUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	// 길드하우스 관련 권한 검사
	if( !IsAuthority( pUser, nPacketType ) )
		return FALSE;
					
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			{
				if( !IsListUpAble( gfi.dwItemId ) )
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_SPACE );
					return FALSE;
				}
				break;
			}

		case GUILDHOUSE_PCKTTYPE_SETUP:
			{
				if( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					return FALSE;

				if( nIndex < 0 || nIndex >= (int)( m_vecFntInfo.size() ) )	return FALSE;
				if( m_vecFntInfo.at( nIndex ).bSetup )	return FALSE;

				gfi.bSetup = TRUE;
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( gfi.dwItemId );
				if( !pItemProp ) return FALSE;
				if( m_vecFntInfo.at( nIndex ).dwItemId != gfi.dwItemId )
					return FALSE;
				if( pItemProp->dwItemKind3 == IK3_TELEPORTER )
				{
					gfi.vPos = GetTeleporterPos();
					gfi.fAngle = 0.0f;
				}

				gfi.tKeepTime = m_vecFntInfo.at( nIndex ).tKeepTime == 0 ? time_null() + ( pItemProp->dwAbilityMin * 60 ) : m_vecFntInfo.at( nIndex ).tKeepTime;
				
				return IsSetupAble( pUser, nIndex );
			}

		case GUILDHOUSE_PCKTTYPE_REMOVE:
				if( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					return FALSE;

				if( nIndex < 0 || nIndex >= (int)( m_vecFntInfo.size() ) )	return FALSE;
				if( !m_vecFntInfo.at( nIndex ).bSetup )	return FALSE;
				gfi.objId = NULL_ID;
				gfi.bSetup = FALSE;
				break;
		
		case GUILDHOUSE_PCKTTYPE_RESET:
				if( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					return FALSE;

				if( nIndex < 0 || nIndex >= (int)( m_vecFntInfo.size() ) )	return FALSE;
				if( !m_vecFntInfo.at( nIndex ).bSetup )	return FALSE;
				gfi.bSetup = TRUE;
				gfi.tKeepTime = m_vecFntInfo.at( nIndex ).tKeepTime;
				break;
	
		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			{
				gfi.tKeepTime = gfi.dwItemId * UPKEEP_DAY_TIME + ( GetUpkeepTime() ? GetUpkeepTime() : time_null() );	// 최종 설정될 유지기간(초) : 만료시간
				time_t nTotalDays = ( gfi.tKeepTime - time_null() ) / UPKEEP_DAY_TIME;	// 총 남은 유지기간(일) : 현재 + 추가
				if( nTotalDays >= MAX_UPKEEP_DAY || nTotalDays <= 0 )
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_TAX_MAX, "%d", MAX_UPKEEP_DAY );
					return FALSE;
				}

				
				//int nPayPenya = gfi.dwItemId * GetUpkeepPenya();
				//-->
				const __int64 n64PayPenya	= gfi.dwItemId * GetUpkeepPenya();
				if( n64PayPenya < 0 || n64PayPenya > INT_MAX )
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
					return FALSE;
				}
				const int nPayPenya		= static_cast< int >( n64PayPenya );

				//if( pUser->GetGold() >= nPayPenya )
				//-->
				if( pUser->CheckUserGold( nPayPenya, false ) == true )
				{
					pUser->AddGold( -nPayPenya );
					g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_UPKEEP", nPayPenya );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
					return FALSE;
				}
				break;
			}
	}

	return TRUE;
}

void CGuildHouseBase::AfterFailedGuildHousePacket( int nPacketType, GH_Fntr_Info& gfi, int /*nIndex*/ )
{
	FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( GetWaitDBAckPlayerId() ) );
	if( !IsValidObj( pUser ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid User! [Packet:%d], [User:%d], [ItemId:%d]" ), nPacketType, GetWaitDBAckPlayerId(), gfi.dwItemId );
		return;
	}

	pUser->AddDefinedText( TID_GAME_GUILDHOUSE_LATER );
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			{
				FLItemElem itemElem;
				itemElem.m_dwItemId = gfi.dwItemId;
				itemElem.m_nItemNum	= 1;
				if( pUser->CreateItem( &itemElem ) )
					g_DPSrvr.PutItemLog( pUser, "f", "GUILDHOUSE_LISTUP_REPAIR", &itemElem, 1 );
				else
					FLERROR_LOG( PROGRAM_NAME, _T( "CreateItem Failed! [User:%d], [ItemId:%d]" ), pUser->m_idPlayer, gfi.dwItemId );
				break;
			}

		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			{
				pUser->AddGold( gfi.dwItemId * GetUpkeepPenya() );
				g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_UPKEEP_REPAIR", (int)( gfi.dwItemId ) * GetUpkeepPenya() * ( -1 ) );		//sun : safecast

				break;
			}
	}
}

BOOL CGuildHouseBase::SendWorldToDatabase( FLWSUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	if( !IsValidObj( pUser ) )
		return FALSE;
	
	if( IsWaitDBAck() )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_LATER );
		return FALSE;
	}

	if( !PreCheckPacket( pUser, nPacketType, gfi, nIndex ) )
		return FALSE;
	
	SetWaitDBAckPlayerId( pUser->m_idPlayer );
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idGuild;
		ar << nPacketType << nIndex;
		gfi.Serialize( ar );
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}

	return TRUE;
}

void CGuildHouseBase::SendWorldToClient( int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( pGuild )
	{
		for( std::map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
				pUser->AddGuildHousePakcet( nPacketType, gfi, nIndex );
		}
	}
}

void CGuildHouseBase::SetApplyDST( FLWSUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return;

	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo.at( i ).bSetup )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( i ).dwItemId );
			if( pItemProp )
			{
				for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
				{
					g_xApplyItemEffect->SetDestParameter( pUser, Nth, pItemProp );
				}
// 				g_xApplyItemEffect->SetDestParameter( pUser, 0, pItemProp );
// 				g_xApplyItemEffect->SetDestParameter( pUser, 1, pItemProp );
// 				g_xApplyItemEffect->SetDestParameter( pUser, 2, pItemProp );
			}
		}
	}
}

void CGuildHouseBase::ResetApplyDST( FLWSUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return;

	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo.at( i ).bSetup )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( i ).dwItemId );
			if( pItemProp )
			{
				for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
				{
					g_xApplyItemEffect->ResetDestParameter( pUser, Nth, pItemProp );
				}
// 				g_xApplyItemEffect->ResetDestParameter( pUser, 0, pItemProp );
// 				g_xApplyItemEffect->ResetDestParameter( pUser, 1, pItemProp );
// 				g_xApplyItemEffect->ResetDestParameter( pUser, 2, pItemProp );
			}
		}
	}
}

void CGuildHouseBase::SetDSTFunriture( PT_ITEM_SPEC pItemProp )
{
	if( !pItemProp )
		return;
	
	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( pGuild )
	{
		for( std::map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
			{
				for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
				{
					g_xApplyItemEffect->SetDestParameter( pUser, Nth, pItemProp );
				}
// 				g_xApplyItemEffect->SetDestParameter( pUser, 0, pItemProp );
// 				g_xApplyItemEffect->SetDestParameter( pUser, 1, pItemProp );
// 				g_xApplyItemEffect->SetDestParameter( pUser, 2, pItemProp );
			}
		}
	}
}

void CGuildHouseBase::ResetDSTFunriture( PT_ITEM_SPEC pItemProp )
{
	if( !pItemProp )
		return;

	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( pGuild )
	{
		for( std::map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
			{
				for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
				{
					g_xApplyItemEffect->ResetDestParameter( pUser, Nth, pItemProp );
				}
// 				g_xApplyItemEffect->ResetDestParameter( pUser, 0, pItemProp );
// 				g_xApplyItemEffect->ResetDestParameter( pUser, 1, pItemProp );
// 				g_xApplyItemEffect->ResetDestParameter( pUser, 2, pItemProp );
			}
		}
	}
}

BOOL CGuildHouseBase::IsListUpAble( DWORD dwItemId )
{
	if( m_vecFntInfo.size() >= MAX_LISTUP_NUM )
		return FALSE;

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
	if( !pItemProp )
		return FALSE;

	int nMaxNum = GetMaxListUpNum( pItemProp );
	int nItemCount = 0;
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		PT_ITEM_SPEC pTargetProp = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( i ).dwItemId );
		if( pTargetProp && pItemProp->dwItemKind3 == pTargetProp->dwItemKind3 )
			nItemCount++;

		if( nItemCount >= nMaxNum )
			return FALSE;
	}

	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
int	CGuildHouseBase::GetFirstExpiredFurnitureIndex()
{
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo.at( i ).tKeepTime != 0 && m_vecFntInfo.at( i ).tKeepTime <= time_null() )
			return i;
	}

	return NULL_ID;
}
#endif // __DBSERVER

void CGuildHouseBase::SerializeAllInfo( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwGuildId << m_dwWorldId;
		if( m_tUpkeepTime > 0 )
			ar << m_tUpkeepTime - time_null();
		else
			ar << static_cast<time_t>( 0 );
#ifdef __GUILD_HOUSE_MIDDLE
		ar << m_dwGHIndex << m_dwNpcIndex << m_nAreaIndex << m_nGHLevel;
		ar.WriteString( m_szGHComment );
#endif // __GUILD_HOUSE_MIDDLE
		ar << m_vecFntInfo.size();
		for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
			m_vecFntInfo.at( i ).Serialize( ar );
	}
	else
	{
		ar >> m_dwGuildId >> m_dwWorldId >> m_tUpkeepTime;
		if( m_tUpkeepTime > 0 )
			m_tUpkeepTime += time_null();
#ifdef __GUILD_HOUSE_MIDDLE
		ar >> m_dwGHIndex >> m_dwNpcIndex >> m_nAreaIndex >> m_nGHLevel;
		ar.ReadString( m_szGHComment, _countof( m_szGHComment ) );
#endif // __GUILD_HOUSE_MIDDLE
		int nSize;
		ar >> nSize;
		m_vecFntInfo.clear();
		m_vecFntInfo.reserve( nSize );
		for( int i=0; i<nSize; i++ )
		{
			GuildHouse_Furniture_Info kFurnitureInfo;
			kFurnitureInfo.Serialize( ar );
			m_vecFntInfo.push_back( kFurnitureInfo );
		}
	}
}

BOOL CGuildHouseBase::OnGuildHousePacket( int nPacketType, GuildHouse_Furniture_Info & gfi, int nIndex )
{
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			return ListUpFurniture( gfi );

		case GUILDHOUSE_PCKTTYPE_LISTDROP:
			return ListDropFurniture( nIndex );

		case GUILDHOUSE_PCKTTYPE_SETUP:
#ifdef __CLIENT
			{
				PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( gfi.dwItemId );
				if( pProp )
				{
					CString str;
					str.Format( GETTEXT( TID_GAME_GUILDHOUSE_FUR_INSTALL_SUCCESS ), pProp->szName );		//설치성공 
					g_WndMng.PutString( str, NULL, prj.GetTextColor( TID_GAME_GUILDHOUSE_FUR_INSTALL_SUCCESS ) );
				}
			}

#endif // __CLIENT
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'S' );
#endif // __WORLDSERVER
			return SetupFurnitureCtrl( nIndex, gfi );

		case GUILDHOUSE_PCKTTYPE_REMOVE:
#ifdef __CLIENT
			{
				PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( gfi.dwItemId );
				if( pProp )
				{
					CString str;
					str.Format( GETTEXT( TID_GAME_GUILDHOUSE_FUR_DISMANTLE_SUCCESS ), pProp->szName );	//해체 성공  
					g_WndMng.PutString( str, NULL, prj.GetTextColor( TID_GAME_GUILDHOUSE_FUR_DISMANTLE_SUCCESS ) );
				}
			}
#endif // __CLIENT
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'R' );
#endif // __WORLDSERVER
			return RemoveFurnitureCtrl( nIndex, gfi );

		case GUILDHOUSE_PCKTTYPE_RESET:
#ifdef __CLIENT
			{
				PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( gfi.dwItemId );
				if( pProp )
				{
					CString str;
					str.Format( GETTEXT( TID_GAME_GUILDHOUSE_FUR_REINSTALL_SUCCESS ), pProp->szName );	//재설치 성공 
					g_WndMng.PutString( str, NULL, prj.GetTextColor( TID_GAME_GUILDHOUSE_FUR_REINSTALL_SUCCESS ) );
				}
			}
#endif
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'C' );
#endif // __WORLDSERVER
			return ResetFurnitureCtrl( nIndex, gfi );
#ifdef __CLIENT
		case GUILDHOUSE_PCKTTYPE_REFRESH:
			return SetupFurnitureCtrl( nIndex, gfi );
#endif // __CLIENT
		
		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			SetUpkeeptime( gfi.tKeepTime );
#ifdef __CLIENT
			// 유지비 입금 완료 텍스트 ID.
			g_WndMng.PutString( prj.GetText( TID_MMI_GUILDHOUSERENTALSUCCESS ), NULL, prj.GetTextColor( TID_MMI_GUILDHOUSERENTALSUCCESS ) );
#endif // __CLIENT
			return TRUE;

		case GUILDHOUSE_PCKTTYPE_EXPIRED:
			return ExpiredGuildHouse();
	}

	return FALSE;
}

BOOL CGuildHouseBase::ListUpFurniture( GuildHouse_Furniture_Info & gfi )
{
	m_vecFntInfo.push_back( gfi );
#ifdef __WORLDSERVER
	g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'I' );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL CGuildHouseBase::ListDropFurniture( int nIndex )
{
	if( (int)( m_vecFntInfo.size() ) <= nIndex )
		return FALSE;
#ifdef __WORLDSERVER
	if( m_vecFntInfo.at( nIndex ).bSetup == TRUE )
		RemoveFurnitureCtrl( nIndex, m_vecFntInfo.at( nIndex ) );

	g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, m_vecFntInfo.at( nIndex ), 'D' );
#endif // __WORLDSERVER
	m_vecFntInfo.erase( m_vecFntInfo.begin() + nIndex );
	return TRUE;
}

BOOL CGuildHouseBase::SetupFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi )
{
	if( nIndex < 0 || (int)( m_vecFntInfo.size() ) <= nIndex || m_vecFntInfo.at( nIndex ).dwItemId != gfi.dwItemId )	return FALSE;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( gfi.dwItemId );
	if( !pItemProp )	return FALSE;

#ifdef __WORLDSERVER
	// 레이어가 존재하는지 확인하고 가구 컨트롤 생성...
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
		if( pWorld->VecInWorld( gfi.vPos ) == FALSE )
		{
			return FALSE;
		}

		CCtrl* pCtrl = NULL;
		if( pItemProp->dwItemKind2 == IK2_GUILDHOUSE_NPC )	// 텔레포터는 NPC이기 때문에 무버를 생성해야 한다.
		{
			pCtrl = static_cast<CCtrl*>( CreateObj( D3DDEVICE, OT_MOVER, pItemProp->dwLinkKind ) );
			CMover* pMover = static_cast<CMover*>( pCtrl );
			if( pMover )
			{
				FLStrcpy( pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ), pItemProp->szTextFileName );
				pMover->InitNPCProperty();
				pMover->InitCharacter( pMover->GetCharacter() );
				pMover->InitMotion( MTI_STAND );
				pMover->UpdateLocalMatrix();
			}
		}
		else if( pItemProp->dwItemKind2 == IK2_GUILDHOUSE_FURNITURE )
		{
			pCtrl = static_cast<CCtrl*>( CreateObj( D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind ) );
		}

		if( pCtrl )
		{
			pCtrl->SetPos( gfi.vPos );
			pCtrl->SetAngle( gfi.fAngle );
			pCtrl->SetLayer( static_cast<int>( m_dwGuildId ) );
			pCtrl->AddItToGlobalId();
			gfi.objId = pCtrl->GetId();
			if( pWorld->ADDOBJ( pCtrl, FALSE, static_cast<int>( m_dwGuildId ) ) == FALSE )
			{
				SAFE_DELETE( pCtrl );
				return FALSE;
			}
		}
	}

	if( !m_vecFntInfo.at( nIndex ).bSetup )	// 현재 해제되어 있던거만 능력치 설정
		SetDSTFunriture( pItemProp );	// 가구 능력치 설정
#endif // __WORLDSERVER

	m_vecFntInfo.at( nIndex ) = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::RemoveFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi )
{
	if( nIndex < 0 || (int)( m_vecFntInfo.size() ) <= nIndex || m_vecFntInfo.at( nIndex ).dwItemId != gfi.dwItemId )	return FALSE;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( nIndex ).dwItemId );
	if( !pItemProp )	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
		// 가구 컨트롤을 찾아서 삭제
		CCtrl* pCtrl = prj.GetCtrl( m_vecFntInfo.at( nIndex ).objId );
		if( pCtrl )
			pCtrl->Delete();
	}
	
	if( m_vecFntInfo.at( nIndex ).bSetup )	// 현재 설치되어 있던거만 능력치 해제
		ResetDSTFunriture( pItemProp );	// 가구 능력치 해제
#endif // __WORLDSERVER
	m_vecFntInfo.at( nIndex ) = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::ResetFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi )
{
	if( nIndex < 0 || (int)( m_vecFntInfo.size() ) <= nIndex || m_vecFntInfo.at( nIndex ).dwItemId != gfi.dwItemId )	return FALSE;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( gfi.dwItemId );
	if( !pItemProp )	return FALSE;
	if( pItemProp->dwItemKind3 == IK3_TELEPORTER )	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
		// 일단 먼저 재배치할 가구를 삭제한다.
		CCtrl* pCtrl	= prj.GetCtrl( m_vecFntInfo.at( nIndex ).objId );
		if( pCtrl )
		{
			pCtrl->Delete();	// 삭제
			// 그리고 다시 생성!!!
			pCtrl = (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind );
			if( pCtrl )
			{
				pCtrl->SetPos( gfi.vPos );
				pCtrl->SetAngle( gfi.fAngle );
				pCtrl->AddItToGlobalId();
				gfi.objId = pCtrl->GetId();
				if( pWorld->ADDOBJ( pCtrl, FALSE, static_cast<int>( m_dwGuildId ) ) == FALSE )
				{
					SAFE_DELETE( pCtrl );
				}
			}
		}
	}
#endif // __WORLDSERVER
	m_vecFntInfo.at( nIndex ) = gfi;
	return TRUE;	
}

BOOL CGuildHouseBase::ExpiredGuildHouse()
{
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo.at( i ).bSetup )
		{
			RemoveFurnitureCtrl( i, m_vecFntInfo.at( i ) );
			m_vecFntInfo.at( i ).bSetup = FALSE;
			m_vecFntInfo.at( i ).objId = NULL_ID;
		}
	}
	m_tUpkeepTime = 0;
#ifdef __WORLDSERVER
	DestroyGuildHouseRoom();
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __GUILD_HOUSE_MIDDLE
#ifndef __CLIENT
void CGuildHouseBase::SetGuildHouseData( DWORD dwGHIndex, DWORD dwNpcIndex, int nAreaIndex, int nGHLevel, time_t tUpkeepTime, const TCHAR* szGHComment )
{
	m_dwGHIndex = dwGHIndex;
	m_dwNpcIndex = dwNpcIndex;
	m_nAreaIndex = nAreaIndex;
	m_nGHLevel = nGHLevel;
	m_tUpkeepTime = tUpkeepTime;
	memset( m_szGHComment, 0, sizeof( m_szGHComment ) );
	FLStrcpy( m_szGHComment, _countof( m_szGHComment ), szGHComment );
}

void CGuildHouseBase::SetGuildHouseComment( const TCHAR* pszComment )
{
	memset( m_szGHComment, 0, sizeof( m_szGHComment ) );
	FLStrcpy( m_szGHComment, _countof( m_szGHComment ), pszComment );
}
#endif // __nCLIENT
#endif // __GUILD_HOUSE_MIDDLE

#ifndef __CLIENT
//////////////////////////////////////////////////////////////////////
// CGuildHouseSmall : CGuildBase
//////////////////////////////////////////////////////////////////////
CGuildHouseSmall::CGuildHouseSmall( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_SMALL )
{
}

CGuildHouseSmall::~CGuildHouseSmall()
{
}


#ifdef __WORLDSERVER
int	CGuildHouseSmall::GetMaxListUpNum( PT_ITEM_SPEC pItemProp )
{
	if( !pItemProp )
		return 0;

	return MAX_LISTUP_NUM;
}

// 소형 길드하우스는 종류별 개수 제한만 있다.
BOOL CGuildHouseSmall::IsSetupAble( FLWSUser* pUser, int nIndex )
{	
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( nIndex ).dwItemId );
	if( !pItemProp )
		return FALSE;

	if( pItemProp->dwAbilityMax > GH_FURNITURE_LEVEL_SMALL )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_FUR_INSTALL_LEVEL, "%d", pItemProp->dwAbilityMax );
		return FALSE;
	}

	int nMaxSetupNum = 0;
	switch( pItemProp->dwItemKind3 )
	{
		case IK3_CHAIR:	nMaxSetupNum = 4;	break;
		default:		nMaxSetupNum = 1;	break;
	}

	int nSetupNum = 0;
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( !m_vecFntInfo.at( i ).bSetup )
			continue;
		
		PT_ITEM_SPEC pSetup = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( i ).dwItemId );
		if( pItemProp->dwItemKind3 == pSetup->dwItemKind3 )
			nSetupNum++;

		if( nSetupNum >= nMaxSetupNum )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_KIND, "%d", nMaxSetupNum );	
			return FALSE;
		}
	}

	return TRUE;
}
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CGuildHouseMiddle : CGuildBase
//////////////////////////////////////////////////////////////////////
#ifdef __GUILD_HOUSE_MIDDLE

CGuildHouseMiddle::CGuildHouseMiddle( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_MIDDLE )
{
}

CGuildHouseMiddle::~CGuildHouseMiddle()
{
}

#ifdef __WORLDSERVER

BOOL CGuildHouseMiddle::IsSetupAble( FLWSUser* pUser, int nIndex )
{	
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( nIndex ).dwItemId );
	if( !pItemProp )
		return FALSE;

	if( pItemProp->dwAbilityMax > GH_FURNITURE_LEVEL_MIDDLE )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_FUR_INSTALL_LEVEL, "%d", pItemProp->dwAbilityMax );
		return FALSE;
	}

	if( pItemProp->dwItemKind3 == IK3_TELEPORTER && pItemProp->dwAbilityMax != GH_FURNITURE_LEVEL_MIDDLE )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_FUR_INSTALL_LEVEL, "%d", pItemProp->dwAbilityMax );
		return FALSE;
	}

	int nMaxSetupNum = 0;
	switch( pItemProp->dwItemKind3 )
	{
	case IK3_CHAIR:	nMaxSetupNum = 4;	break;
	default:		nMaxSetupNum = 1;	break;
	}

	int nSetupNum = 0;
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( !m_vecFntInfo.at( i ).bSetup )
			continue;

		PT_ITEM_SPEC pSetup = g_xSpecManager->GetSpecItem( m_vecFntInfo.at( i ).dwItemId );
		if( pItemProp->dwItemKind3 == pSetup->dwItemKind3 )
			nSetupNum++;

		if( nSetupNum >= nMaxSetupNum )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_KIND, "%d", nMaxSetupNum );	
			return FALSE;
		}
	}

	return TRUE;
}

#endif // __WORLDSERVER
#endif // __GUILD_HOUSE_MIDDLE


/*
//////////////////////////////////////////////////////////////////////
// CGuildHouseLarge : CGuildBase
//////////////////////////////////////////////////////////////////////
CGuildHouseLarge::CGuildHouseLarge( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_LARGE )
{
}

CGuildHouseLarge::~CGuildHouseLarge()
{
}
*/



//////////////////////////////////////////////////////////////////////
// CGuildHouseMng
//////////////////////////////////////////////////////////////////////
CGuildHouseMng::CGuildHouseMng()
#ifdef __DBSERVER
: m_nSeqNum( 0 )
#ifdef __GUILD_HOUSE_MIDDLE
, m_bGMTenderMode( false )
#endif // __GUILD_HOUSE_MIDDLE
#endif // __DBSERVER

{
}

CGuildHouseMng::~CGuildHouseMng()
{
	for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
	{
		SAFE_DELETE( it->second );
	}
	m_mapGuildHouse.clear();
#ifdef __GUILD_HOUSE_MIDDLE
	m_mapTenderData.clear();
	m_mapGuildHouseData.clear();
	for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
	{
		it->second.clear();
	}
	m_mapGuildHouseTender.clear();
#endif // __GUILD_HOUSE_MIDDLE
}

CGuildHouseMng* CGuildHouseMng::GetInstance()
{
	static CGuildHouseMng sGHM;
	return & sGHM;
}

CGuildHouseBase* CGuildHouseMng::MakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId )
{
	CGuildHouseBase* pGuildHouse	= NULL;
	switch( dwWorldId )
	{
		case WI_GUILDHOUSE_SMALL:
			pGuildHouse	= new CGuildHouseSmall( dwGuildId );
			break;

		case WI_GUILDHOUSE_MIDDLE:
			pGuildHouse	= new CGuildHouseMiddle( dwGuildId );
			break;

		default:
			break;
	}

	return pGuildHouse;
}

BOOL CGuildHouseMng::RemoveGuildHouse( DWORD dwGuildId )
{
	MapGuildHouse::iterator it = m_mapGuildHouse.find( dwGuildId );
	if( it == m_mapGuildHouse.end() )
		return FALSE;
	
	SAFE_DELETE( it->second );
	m_mapGuildHouse.erase( it );
	return TRUE;
}

CGuildHouseBase* CGuildHouseMng::GetGuildHouse( DWORD dwGuildId )
{
	MapGuildHouse::iterator it = m_mapGuildHouse.find( dwGuildId );
	if( it == m_mapGuildHouse.end() )
		return NULL;

	return it->second;
}

void CGuildHouseMng::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_mapGuildHouse.size();
		for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
		{
			ar << it->first << it->second->GetType();
			it->second->SerializeAllInfo( ar );
		}
#ifdef __GUILD_HOUSE_MIDDLE
		ar << m_mapGuildHouseTender.size();
		for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
		{
			ar << it->first << it->second.size();
			
			for( int i = 0; i < (int)it->second.size(); ++i )
			{
				T_GUILDHOUSE_TENDER tGHTender;// = { 0 };
				tGHTender = (it->second).at( i );
				tGHTender.Serialize( ar );
			}
		}
#endif // __GUILD_HOUSE_MIDDLE
	}
	else
	{
		for( MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); ++it )
		{
			SAFE_DELETE( it->second );
		}
		m_mapGuildHouse.clear();

		int nSize;
		DWORD dwGuildId, dwType;
		ar >> nSize;
		for( int i=0; i<nSize; i++ )
		{
			ar >> dwGuildId >> dwType;
			CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, dwType );
			
			if( pGuildHouse && AddGuildHouse( dwGuildId, pGuildHouse ) )
			{
				pGuildHouse->SerializeAllInfo( ar );
			}
			else
			{
				SAFE_DELETE( pGuildHouse );
				CGuildHouseSmall temp( NULL_ID );
				temp.SerializeAllInfo( ar );
			}
		}
#ifdef __GUILD_HOUSE_MIDDLE
		m_mapGuildHouseTender.clear();
		int nTenderIndexSize;
		int nTenderGuildSize;
		DWORD dwTenderIndex;
		ar >> nTenderIndexSize;
		for( int i = 0; i < nTenderIndexSize; ++i )
		{
			
			ar >> dwTenderIndex >> nTenderGuildSize;

			for( int j = 0; j < nTenderGuildSize; ++j )
			{
				T_GUILDHOUSE_TENDER tGHTender;// = { 0 };
				tGHTender.Serialize( ar );

				AddGuildHouseTender( &tGHTender );
			}
		}
#endif // __GUILD_HOUSE_MIDDLE
	}
}

#ifdef __GUILD_HOUSE_MIDDLE

CGuildHouseBase* CGuildHouseMng::GetGuildHouseByIndex( DWORD dwIndex )
{
	for( MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); ++it )
	{
		if( it->second->GetGuildHouseId() == dwIndex || it->second->GetGuildHouseNpcId() == dwIndex )
		{
			return it->second;
		}
	}
	return NULL;
}

#endif // __GUILD_HOUSE_MIDDLE

#ifdef __WORLDSERVER
BOOL CGuildHouseMng::IsBuyAble( FLWSUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return FALSE;

	CGuild* pGuild = pUser->GetGuild();
	if( pGuild )
	{
		if( !pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_MASTER );
			return FALSE;
		}
		
		if( pGuild->m_nLevel < BUY_MIN_LEVEL )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_LEVEL, "%d", BUY_MIN_LEVEL );
			return FALSE;
		}

		if( pGuild->GetSize() < BUY_MIN_MEMBERSIZE )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_POPULATION, "%d", BUY_MIN_MEMBERSIZE );
			return FALSE;
		}

		if( pUser->CheckUserGold( BUY_PENYA, false ) == false )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
			return FALSE;
		}

		if( m_mapGuildHouse.find( pGuild->GetGuildId() ) != m_mapGuildHouse.end() )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_HAVE );
			return FALSE;
		}
	}
	else
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_MASTER );
		return FALSE;
	}

	return TRUE;
}

void CGuildHouseMng::ReqBuyGuildHouse( FLWSUser* pUser )
{
	if( IsBuyAble( pUser ) )
	{
		pUser->AddGold( -BUY_PENYA );
		g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_BUY", BUY_PENYA ); // 페냐 로그

		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_BUY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer << pUser->m_idGuild;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}

void CGuildHouseMng::OnBuyGuildHouse( CAr & ar )
{
	BOOL bResult;
	DWORD dwPlayerId, dwGuildId;
	
	ar >> dwPlayerId >> dwGuildId >> bResult;
	if( bResult )
	{
		CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, WI_GUILDHOUSE_SMALL );
		if( pGuildHouse && AddGuildHouse( dwGuildId, pGuildHouse ) )
		{
			pGuildHouse->SerializeAllInfo( ar );
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
			//if( IsValidObj( pUser ) )
			//	pUser->AddDefinedText( TID_GAME_GUILDHOUSE_PURCHASE );

			CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
			if( pGuild )
			{
				for( std::map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
				{
					pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
					if( IsValidObj( pUser ) )
					{
						pUser->AddGuildHouseAllInfo( pGuildHouse );
						pUser->AddDefinedText( TID_GAME_GUILDHOUSE_PURCHASE );
						pGuildHouse->SetApplyDST( pUser );
					}
				}
			}
		}
		else
		{
			SAFE_DELETE( pGuildHouse );
			FLERROR_LOG( PROGRAM_NAME, _T( "CreateGuildHouse Failed!!! [User:%07d, Guild:%06d]" ), dwPlayerId, dwGuildId ); 
			CGuildHouseSmall temp( NULL_ID );
			temp.SerializeAllInfo( ar );
		}
	}
	else
	{
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
		{
			pUser->AddGold( BUY_PENYA );
			g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_BUY_FAIL", BUY_PENYA * ( -1 ) );
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_HAVE );
		}
	}
}

void CGuildHouseMng::OnRemoveGuildHouse( CAr & ar )
{
	DWORD dwGuildId;
	ar >> dwGuildId;

	RemoveGuildHouse( dwGuildId );
	
	CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
	if( pGuild )
	{
		for( std::map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
			{
				pUser->SetRestPoint( 0 );
				pUser->AddGuildHouseRemvoe();
			}
		}
	}
}

BOOL CGuildHouseMng::SendWorldToDatabase( FLWSUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	if( !IsValidObj( pUser ) )
		return FALSE;

	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		return pGuildHouse->SendWorldToDatabase( pUser, nPacketType, gfi, nIndex );

	return FALSE;
}

BOOL CGuildHouseMng::IsGuildHouse( DWORD dwWorldId )
{
	switch( dwWorldId )
	{
		case WI_GUILDHOUSE_SMALL:
		case WI_GUILDHOUSE_MIDDLE:
		case WI_GUILDHOUSE_LARGE:
			return TRUE;
	}

	return FALSE;
}

BOOL CGuildHouseMng::IsLoginAble( FLWSUser* pUser, DWORD dwWorldId, int nLayer )
{
	if( !IsGuildHouse( dwWorldId ) )	// 접속할때 World가 길드하우스가 아니면 패스!!!
		return TRUE;

	if( pUser->m_idGuild != static_cast< DWORD >( nLayer ) )	// 일단 길드하우스라면 최종 접속 레이어가 캐릭터의 길드ID와 일치해야 한다.
		return FALSE;
	
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )	// 입장 가능한 캐릭터이면 휴식 포인트를 요청한다.
		return pGuildHouse->IsEnteranceAble( pUser );
	
	return FALSE;
}

BOOL CGuildHouseMng::EnteranceGuildHouse( FLWSUser* pUser, DWORD dwComebackItemId, DWORD dwGHIndex )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( !pGuildHouse )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_ENTER_HAVE );
		return FALSE;
	}

	switch( dwComebackItemId )
	{
		case NULL_ID :	// 길드하우스 귀환 주문서를 사용하지 않은 경우(NPC 입장 메뉴 선택)
			{
#ifdef __GUILD_HOUSE_MIDDLE

				if( dwGHIndex != NULL_ID )	// 마을에 위치한 길드하우스 문을 통해 입장하는 경우
				{
					if( pGuildHouse->GetGuildHouseId() != dwGHIndex )
					{
						pUser->AddDefinedText( TID_MMI_GUILD_NOTGUILDWON );
						return FALSE;
					}
					if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDHOUSE_ENTER_DOOR, pUser->GetWorld(), pUser->GetPos() ) == TRUE )
					{
						pUser->SetMarkingPos();
					}
					else
					{
						return FALSE;
					}
				}
				else
				{
					if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDHOUSE_ENTER, pUser->GetWorld(), pUser->GetPos() ) == TRUE )
					{
						pUser->SetMarkingPos();
					}
					else
					{
						return FALSE;
					}
				}

#else // __GUILD_HOUSE_MIDDLE

				if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDHOUSE_ENTER, pUser->GetWorld(), pUser->GetPos() ) )
				{
					pUser->SetMarkingPos();
				}
				else
				{
					return FALSE;
				}
				break;

#endif // __GUILD_HOUSE_MIDDLE
			}
			break;

		default:
			{
				if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MADRIGAL )
				{
					pUser->m_idMarkingWorld = WI_WORLD_MADRIGAL;
					pUser->m_vMarkingPos = D3DXVECTOR3( 6968.0f, 0.0f, 3328.8f );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_COMEBACK_ERROR01 );
					return FALSE;
				}
				break;
			}
	}

	return pGuildHouse->EnteranceGuildHouseRoom( pUser );
}

void CGuildHouseMng::GoOutGuildHouse( FLWSUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->GoOutGuildHouseRoom( pUser );
}

void CGuildHouseMng::CheckDestroyGuildHouse( FLWSUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->CheckDestroyRoom( pUser );
}

void CGuildHouseMng::SetApplyDST( FLWSUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->SetApplyDST( pUser );
}

void CGuildHouseMng::ResetApplyDST( FLWSUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->ResetApplyDST( pUser );
}

float CGuildHouseMng::GetRestPointFactor( FLWSUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		return pGuildHouse->GetRestPointFactor();
	
	return 0.0f;
}

void CGuildHouseMng::AddRestExpFactorTable( int nMinLevel, int nMaxLevel, float fFactor )
{
	for( int i=nMinLevel; i<=nMaxLevel; i++ )
	{
		if( !m_mapRestExpFactorTable.insert( make_pair( i, fFactor ) ).second )
			FLERROR_LOG( PROGRAM_NAME, _T( "Insert Failed! - [Level:%d]" ), i );
	}
}

float CGuildHouseMng::GetRestExpFactor( int nLevel )
{
	MapRestExpFactor::iterator it = m_mapRestExpFactorTable.find( nLevel );
	if( it == m_mapRestExpFactorTable.end() )
		return 0.0f;

	return it->second;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
extern	APP_INFO	g_appInfo;
void CGuildHouseMng::ProcessExpired()
{
	for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
	{
		int nIndex = it->second->GetFirstExpiredFurnitureIndex();	// 기간이 만료된 가구 한개를 가져온다.
		if( nIndex != NULL_ID )	// 기간이 만료된 가구가 있는 경우 리스트에서 제거한다.
		{
			CAr ar;
			ar << it->first << GUILDHOUSE_PCKTTYPE_LISTDROP << nIndex;
			it->second->GetFurnitureInfo( nIndex ).Serialize( ar );
			u_long nBufSize;
			LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
			GuildHouseDBMng->PostRequest( GUILDHOUSE_DEFAULT_PACKET, lpBuf, nBufSize );
		}

		if( it->second->GetUpkeepTime() <= time_null() )	// 길드하우스 유지 기간이 만료된 경우
		{
			if( it->second->GetUpkeepTime() > 0 )	// 길드하우스가 활성화 되어있는 상태이면 비활성화 시킨다.
			{
#ifdef __GUILD_HOUSE_MIDDLE
				CAr ar;
				ar << it->first;
				u_long nBufSize;
				LPBYTE lpBuf = ar.GetBuffer( &nBufSize );
				GuildHouseDBMng->PostRequest( GUILDHOUSE_EXPIRED, lpBuf, nBufSize );
#else // __GUILD_HOUSE_MIDDLE
				GH_Fntr_Info gfiTemp;
				CAr ar;
				ar << it->first << GUILDHOUSE_PCKTTYPE_EXPIRED << NULL_ID;
				gfiTemp.Serialize( ar );
				u_long nBufSize;
				LPBYTE lpBuf = ar.GetBuffer( &nBufSize );
				GuildHouseDBMng->PostRequest( GUILDHOUSE_DEFAULT_PACKET, lpBuf, nBufSize );
#endif // __GUILD_HOUSE_MIDDLE
			}
		//	기능 제거(나중에 필요할지 몰라서... 남겨둠...
		//	else if( it->second->GetFurnitureListSize() == 0 )	// 길드하우스가 비활성회 상태이고 가구 목록이 비어있는 경우는 길드하우스를 파괴한다.
		//		GuildHouseDBMng->PostRequest( GUILDHOUSE_REMOVE, NULL, 0, it->first );
		}
	}
}

BOOL CGuildHouseMng::DBLoadGuildHouse( CQuery* pQuery )
{
	if( pQuery->Execute( "usp_GuildHouse_MaxSEQ '%02d'", g_appInfo.dwSys ) )
	{
		if( pQuery->Fetch() )
			m_nSeqNum = pQuery->GetInt( "SEQ" );
	}
	else
		return FALSE;
	
	if( pQuery->Execute( "usp_GuildHouse_Load '%02d'", g_appInfo.dwSys ) )
	{
		while( pQuery->Fetch() )
		{
			DWORD dwGuildId = pQuery->GetInt( "m_idGuild" );
			DWORD dwWorldId = pQuery->GetInt( "dwWorldID" );
			time_t tKeepTime = pQuery->GetInt( "tKeepTime" );
#ifdef __GUILD_HOUSE_MIDDLE
			DWORD dwGHIndex = pQuery->GetInt( "dwGuildHouseId" );
			int nAreaIndex = pQuery->GetInt( "nAreaIndex" );
			int nGHLevel = pQuery->GetInt( "nGuildHouseLevel" );
			TCHAR szGHComment[MAX_GH_COMMENT] = {0, };
			pQuery->GetStr( "szGuildHouseName", szGHComment, _countof( szGHComment ) );
#endif // __GUILD_HOUSE_MIDDLE
			
			CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, dwWorldId );
			if( pGuildHouse && AddGuildHouse( dwGuildId, pGuildHouse ) )
			{
				pGuildHouse->SetUpkeeptime( tKeepTime );
#ifdef __GUILD_HOUSE_MIDDLE
				if( pGuildHouse->GetType() >= WI_GUILDHOUSE_MIDDLE && pGuildHouse->GetType() <= WI_GUILDHOUSE_LARGE )
				{
					PT_GUILDHOUSE_DATA ptGHData = GetGuildHouseData( dwGHIndex );
					if( ptGHData != NULL )
					{
						pGuildHouse->SetGuildHouseData( dwGHIndex, ptGHData->dwNpcIndex, nAreaIndex, nGHLevel, tKeepTime, szGHComment );
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "GetGuildHouseData Failed. [GuildId:%06d], [GHIndex:%d]" ), dwGuildId, dwGHIndex );
					}
				}
#endif // __GUILD_HOUSE_MIDDLE
			}
			else
			{
				SAFE_DELETE( pGuildHouse );
				FLERROR_LOG( PROGRAM_NAME, _T( "AddGuildHouse() Failed!!! [GuildId:%06d]" ), dwGuildId );
			}
		}
	}
	else
		return FALSE;

	for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
	{
		if( pQuery->Execute( "usp_GuildFurniture_Load  '%02d', '%06d'", g_appInfo.dwSys, it->first ) )
		{
			while( pQuery->Fetch() )
			{
				GH_Fntr_Info gfiData;
				gfiData.nSeqNum		= pQuery->GetInt( "SEQ" );
				gfiData.dwItemId	= static_cast<DWORD>( pQuery->GetInt( "ItemIndex" ) );
				gfiData.bSetup		= static_cast<BOOL>( pQuery->GetInt( "bSetup" ) );
				gfiData.vPos.x		= static_cast<float>( pQuery->GetFloat( "x_Pos" ) );
				gfiData.vPos.y		= static_cast<float>( pQuery->GetFloat( "y_Pos" ) );
				gfiData.vPos.z		= static_cast<float>( pQuery->GetFloat( "z_Pos" ) );
				gfiData.fAngle		= static_cast<float>( pQuery->GetFloat( "fAngle" ) );
				gfiData.tKeepTime	= pQuery->GetInt( "tKeepTime" );
				it->second->OnGuildHousePacket( GUILDHOUSE_PCKTTYPE_LISTUP, gfiData, NULL_ID );
			}
		}
		else
			return FALSE;
	}

#ifdef __GUILD_HOUSE_MIDDLE
	PT_TENDER_DATA ptTenderData = GetTenderData( WI_GUILDHOUSE_MIDDLE );
	if( ptTenderData != NULL )
	{
		if( pQuery->Execute( "usp_GuildHouse_MaxTender_Load '%02d'", g_appInfo.dwSys ) == TRUE )
		{
			if( pQuery->Fetch() == TRUE )
			{
				ptTenderData->nTenderId = pQuery->GetInt( "nTenderId" );
				ptTenderData->nTenderArea = pQuery->GetInt( "nTenderArea" );
			}
		}
		else
		{
			//return FALSE;
		}

		if( pQuery->Execute( "usp_GuildHouse_Tender_Load %d, '%02d', '%c'", ptTenderData->nTenderId, g_appInfo.dwSys, 'T' ) )
		{
			while( pQuery->Fetch() )
			{
				T_GUILDHOUSE_TENDER tGHTender;
				tGHTender.dwGHType = WI_GUILDHOUSE_MIDDLE;
				tGHTender.dwGuildId = static_cast<DWORD>( pQuery->GetInt( "m_idGuild" ) );
				tGHTender.nAreaIndex = static_cast<int>( pQuery->GetInt( "nTenderArea" ) );
				tGHTender.dwGHIndex = static_cast<OBJID>( pQuery->GetInt( "dwGuildHouseId" ) );
				tGHTender.nTenderPerin = static_cast<int>( pQuery->GetInt( "nTenderPerin" ) );
				tGHTender.nTenderPenya = static_cast<int>( pQuery->GetInt( "nTenderPenya" ) );
				tGHTender.tTenderTime = static_cast<time_t>( pQuery->GetInt( "tTenderTime" ) );
				tGHTender.chState = static_cast<char>( pQuery->GetChar( "chState" ) );

				if( AddGuildHouseTender( &tGHTender ) == false )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "AddGuildHouseTender Failed. GuildId:[%6d]" ), tGHTender.dwGuildId );
				}
			}
		}
		else
		{
			//return FALSE;
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GuildHouse Tender Script Data Not Found. DB Tender List Load Failed." ) );
		return FALSE;
	}
#endif // __GUILD_HOUSE_MIDDLE

	return TRUE;
}
#endif // \__DBSERVER

#ifdef __GUILD_HOUSE_MIDDLE
int CGuildHouseMng::GetTenderState( DWORD dwGHType )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		return ptTenderData->nTenderState;
	}
	FLERROR_LOG( PROGRAM_NAME, _T( "TenderData Not Found. GHType : %d" ), dwGHType );
	return GH_TENDER_MAX;
}

void CGuildHouseMng::SetTenderState( DWORD dwGHType, int nState )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		ptTenderData->nTenderState = nState;
		ptTenderData->dwNoticeTick = 0;
#ifdef __DBSERVER
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << dwGHType << nState;
		SEND( ar, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
#endif // __DBSERVER
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TenderData Not Found. GHType : %d, State : %d" ), dwGHType, nState );
	}
}

void CGuildHouseMng::CloseTender( DWORD dwGHType )
{
	SetTenderState( dwGHType, GH_TENDER_CLOSE );

	for( MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.begin(); it != m_mapGuildHouseData.end(); ++it )
	{
		if( it->second.dwGHType == dwGHType )
		{
			it->second.nState = GH_STATE_NONE;
		}
	}
	for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); )
	{
		PT_GUILDHOUSE_DATA ptGHData = GetGuildHouseData( it->first );
		if( ptGHData != NULL )
		{
			if( ptGHData->dwGHType == dwGHType )
			{
				it->second.clear();
				m_mapGuildHouseTender.erase( it++ );
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

#ifdef __DBSERVER
BOOL CGuildHouseMng::IsNoticeTime( DWORD /*dwGHType*/ )
{
	BOOL bIsNoticeTime = FALSE;

	if( m_Lua.GetLuaFunction( "IsNoticeTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		bIsNoticeTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop( 0 );

	return bIsNoticeTime;
}

BOOL CGuildHouseMng::IsTenderTime( DWORD /*dwGHType*/ )
{
	BOOL bIsTenderTime = FALSE;

	if( m_Lua.GetLuaFunction( "IsTenderTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		bIsTenderTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop( 0 );

	return bIsTenderTime;
}

void CGuildHouseMng::UpdateGuildHouseTender( DWORD dwGHType, CQuery* pQuery )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
		{
			for( std::vector<T_GUILDHOUSE_TENDER>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
			{
				if( it2->dwGHType == dwGHType )
				{
					pQuery->Execute( "usp_GuildHouse_Tender_Update %d, '%02d', '%06d', '%c'",
						ptTenderData->nTenderId, g_appInfo.dwSys, it2->dwGuildId, it2->chState );
				}
			}
		}
	}
}

void CGuildHouseMng::ReturnPenyaTenderGuild( DWORD dwGHType, CQuery* pQuery )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "ptTenderData is NULL" ) );
		return;
	}

	CGuild* pGuild = NULL;

	TCHAR szMailTitle[MAX_MAILTITLE] = { 0, };
	TCHAR szMailText[MAX_MAILTEXT] = { 0, };

	FLItemElem kItemElem;

	for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
	{
		for( std::vector<T_GUILDHOUSE_TENDER>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
		{
			if( it2->dwGHType == dwGHType )
			{
				if( it2->chState == 'W' )
				{
					if( it2->dwGuildId > 0 )
					{
						// send post
						FLSPrintf( szMailTitle, _countof( szMailTitle ), prj.GetText( TID_MMI_GHOUSE_BIDENDMAILTITLE ) );
						FLSPrintf( szMailText, _countof( szMailText ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSESUCCESSFUL ) );

						kItemElem.Empty();

						pGuild = g_GuildMng.GetGuild( it2->dwGuildId );
						if( pGuild != NULL )
						{
							CDbManager::PostMail( *pQuery, pGuild->m_idMaster, 0, kItemElem, 0, szMailTitle, szMailText );
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "Guild is Not Found. Guild:[%06d], Player:[%07d]" ), it2->dwGuildId, it2->dwPlayerId );
							CDbManager::PostMail( *pQuery, it2->dwPlayerId, 0, kItemElem, 0, szMailTitle, szMailText );
						}
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "Tender Success Guild ID is zero" ) );
					}
				}
				else if( it2->chState == 'L' )
				{
					if( it2->dwGuildId > 0 )
					{
						// send post
						FLSPrintf( szMailTitle, _countof( szMailTitle ), prj.GetText( TID_MMI_GHOUSE_BIDENDMAILTITLE ) );
						FLSPrintf( szMailText, _countof( szMailText ), prj.GetText( TID_MMI_GHOUSE_BIDENDMAILTITLE01 ) );

						kItemElem.Empty();

						const __int64 n64TenderPenya	= static_cast<__int64>( ( static_cast<__int64>( it2->nTenderPerin ) * PERIN_VALUE ) + it2->nTenderPenya );
						const __int64 n64ReturnPenya	= n64TenderPenya - ( ( n64TenderPenya * static_cast<__int64>( ptTenderData->nChargeRate ) ) / 100 );
						const int nPerinNum				= static_cast<int>( n64ReturnPenya / PERIN_VALUE );

						const int nPenya = max( static_cast<int>( n64ReturnPenya - static_cast<__int64>( static_cast<__int64>( nPerinNum ) * PERIN_VALUE ) ), 0 );

						if( nPerinNum > 0 )
						{
							kItemElem.m_dwItemId		= ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN );
							kItemElem.m_nItemNum		= nPerinNum;
							kItemElem.SetSerialNumber();
						}

						pGuild = g_GuildMng.GetGuild( it2->dwGuildId );
						if( pGuild != NULL )
						{
							CDbManager::PostMail( *pQuery, pGuild->m_idMaster, 0, kItemElem, nPenya, szMailTitle, szMailText );
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "Guild is Not Found. Guild:[%06d], Player:[%07d]" ), it2->dwGuildId, it2->dwPlayerId );
							CDbManager::PostMail( *pQuery, it2->dwPlayerId, 0, kItemElem, nPenya, szMailTitle, szMailText );
						}
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "Tender Failed Guild ID is zero" ) );
					}
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "chState Invalid : %s" ), it2->chState );
				}
			}
		}
	}
}

void CGuildHouseMng::SendGuildHouseTenderInfo( DPID dpId )
{
	DWORD dwGHType = WI_GUILDHOUSE_MIDDLE;
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << dwGHType << ptTenderData->nTenderId << ptTenderData->nTenderArea << ptTenderData->nTenderState;

		std::map<DWORD, DWORD> mapGHTenderList;
		if( GetTenderAbleGuildHouse( dwGHType, mapGHTenderList ) == true )
		{
			ar << TRUE << mapGHTenderList.size();
			for( std::map<DWORD, DWORD>::iterator it = mapGHTenderList.begin(); it != mapGHTenderList.end(); ++it )
			{
				ar << it->first;
			}
		}
		else
		{
			ar << FALSE;
		}
		SEND( ar, CDPTrans::GetInstance(), dpId );
	}
}

bool CGuildHouseMng::IsExistTenderGuild( DWORD dwGHType )
{
	bool bRet = false;

	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		for( MAP_T_GUILDHOUSE_TENDER::iterator kItrMap = m_mapGuildHouseTender.begin(); kItrMap != m_mapGuildHouseTender.end(); ++kItrMap )
		{
			for( std::vector<T_GUILDHOUSE_TENDER>::iterator kItrVec = kItrMap->second.begin(); kItrVec != kItrMap->second.end(); ++kItrVec )
			{
				if( dwGHType == kItrVec->dwGHType && ptTenderData->nTenderArea == kItrVec->nAreaIndex )
				{
					MAP_T_GUILDHOUSE_DATA::const_iterator kCItr = m_mapGuildHouseData.find( kItrMap->first );
					if( kCItr != m_mapGuildHouseData.end() )
					{
						if( kCItr->second.nState != GH_STATE_USING )
						{
							bRet = true;
							continue;
						}
					}
				}

				kItrVec->chState = 'L';
			}
		}
	}

	return bRet;
}

bool CGuildHouseMng::CheckTenderOpenAble( DWORD dwGHType )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );

	if( ptTenderData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GetTenderData Fail. GuildHouseType:[%d]" ), dwGHType );
		return false;
	}

	for( MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); ++it )
	{
		if( it->second->GetType() == dwGHType )
		{
			MAP_T_GUILDHOUSE_DATA::iterator it2 = m_mapGuildHouseData.find( it->second->GetGuildHouseId() );
			if( it2 != m_mapGuildHouseData.end() )
			{
				it2->second.nState = GH_STATE_USING;
			}
		}
	}

	std::vector<DWORD> vecGHId;
	vecGHId.clear();
	int nCount = 0;

	if( IsExistTenderGuild( dwGHType ) == true )
	{
		for( MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.begin(); it != m_mapGuildHouseData.end(); ++it )
		{
			if( it->second.nTenderArea == ptTenderData->nTenderArea && it->second.nState == GH_STATE_NONE )
			{
				if( nCount < ptTenderData->nMaxHouse )
				{
					++nCount;
					vecGHId.push_back( it->first );
					it->second.nState = GH_STATE_TENDERABLE;
				}
			}
		}

		if( vecGHId.empty() == false )
		{
			SetTenderInfo( dwGHType, ptTenderData->nTenderId, ptTenderData->nTenderArea );
			SendGuildHouseTenderInfo( DPID_ALLPLAYERS );
			return true;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Exist Tender Data Invalid. Tender Index:[%d], Area:[%d]" ),
				ptTenderData->nTenderId, ptTenderData->nTenderArea );
		}
	}

	int nTenderId	= ptTenderData->nTenderId + 1;
	int nTenderArea = ptTenderData->nTenderArea;

	for( int i = 0; i < GH_AREA_MAX; ++i )
	{
		int nArea = ++nTenderArea % GH_AREA_MAX;
		if( nArea == GH_AREA_NONE )
		{
			continue;
		}

		for( MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.begin(); it != m_mapGuildHouseData.end(); ++it )
		{
			if( it->second.nTenderArea == nArea && it->second.nState == GH_STATE_NONE )
			{
				if( nCount < ptTenderData->nMaxHouse )
				{
					++nCount;
					vecGHId.push_back( it->first );
					it->second.nState = GH_STATE_TENDERABLE;
				}
			}
		}

		if( vecGHId.empty() == false )
		{
			SetTenderInfo( dwGHType, nTenderId, nArea );
			SendGuildHouseTenderInfo( DPID_ALLPLAYERS );
			return true;
		}
	}

	return false;
}

void CGuildHouseMng::ProcessTender( DWORD dwGHType )
{
	DWORD dwTick = GetTickCount();
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TenderData Not Found. GHType : %d" ), dwGHType );
		return;
	}

	switch( GetTenderState( dwGHType ) )
	{
	case GH_TENDER_CLOSE:
		{
			if( IsNoticeTime( dwGHType ) == TRUE || IsTenderTime( dwGHType ) == TRUE  )
			{
				SetTenderState( dwGHType, GH_TENDER_CHECK_OPEN );
			}
		}
		break;

	case GH_TENDER_CANCEL:
		{
			if( dwTick >= ptTenderData->dwNoticeTick + ptTenderData->dwPrevNoticeTick )
			{
				g_dpCoreSrvr.SendNoticeMessage( GetNoticeMessage( dwGHType, GH_TENDER_CANCEL ) );
				ptTenderData->dwNoticeTick = dwTick;
			}

			if( IsNoticeTime( dwGHType ) == FALSE && IsTenderTime( dwGHType ) == FALSE  )
			{
				SetGMTenderMode( false );
				CloseTender( dwGHType );
			}
		}
		break;

	case GH_TENDER_CHECK_OPEN:
		{
			if( CheckTenderOpenAble( dwGHType ) == true )
			{
				SetTenderState( dwGHType, GH_TENDER_OPEN );
			}
			else
			{
				SetTenderState( dwGHType, GH_TENDER_CANCEL );
			}
		}
		break;

	case GH_TENDER_OPEN:
		{
			if( IsTenderTime( dwGHType ) == TRUE && IsGMTenderMode() == false )
			{
				SetTenderState( dwGHType, GH_TENDER_TENDER );
			}
			else if( dwTick >= ptTenderData->dwNoticeTick + ptTenderData->dwPrevNoticeTick )
			{
				g_dpCoreSrvr.SendNoticeMessage( GetNoticeMessage( dwGHType, GH_TENDER_OPEN ) );
				ptTenderData->dwNoticeTick = dwTick;
			}
		}
		break;

	case GH_TENDER_TENDER:
		{
			if( IsTenderTime( dwGHType ) == FALSE && IsGMTenderMode() == false )
			{
				SetTenderState( dwGHType, GH_TENDER_TENDER_RESULT );
			}
			else if( dwTick >= ptTenderData->dwNoticeTick + ptTenderData->dwTenderNoticeTick )
			{
				g_dpCoreSrvr.SendNoticeMessage( GetNoticeMessage( dwGHType, GH_TENDER_TENDER ) );
				ptTenderData->dwNoticeTick = dwTick;
			}
		}
		break;

	case GH_TENDER_TENDER_RESULT:
		{
			SetGMTenderMode( false );
			g_dpCoreSrvr.SendNoticeMessage( GetNoticeMessage( dwGHType, GH_TENDER_TENDER_RESULT ) );
			GetHighestTenderGuild( dwGHType );
		}
		break;

	default:
		break;
	}
}

const TCHAR* CGuildHouseMng::GetNoticeMessage( DWORD dwGHType, int nState )
{
	memset( m_szNoticeMessage, 0, sizeof( m_szNoticeMessage ) );

	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		switch( nState )
		{
		case GH_TENDER_OPEN:		// 입찰준비
			{
				if( ptTenderData->nTenderArea == GH_AREA_FLARIS )
				{
					FLSPrintf( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSEBIDNOTICE ), prj.GetText( TID_GAME_CONT_FLARIS ) );
				}
				else if( ptTenderData->nTenderArea == GH_AREA_SAINTMORNING )
				{
					FLSPrintf( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSEBIDNOTICE ), prj.GetText( TID_GAME_CONT_SAINTMORNING ) );
				}
				else if( ptTenderData->nTenderArea == GH_AREA_ELIUN )
				{
					FLSPrintf( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSEBIDNOTICE ), prj.GetText( TID_GAME_CONT_ELIUN ) );
				}
			}
			break;

		case GH_TENDER_CANCEL:		// 입찰없음
			{
				FLStrcpy( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_NOTWEEKBIDGUILDHOUSE ) );
			}
			break;

		case GH_TENDER_TENDER:		// 입찰진행중
			{
				if( ptTenderData->nTenderArea == GH_AREA_FLARIS )
				{
					FLSPrintf( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSEENDNOTICE ), prj.GetText( TID_GAME_CONT_FLARIS ) );
				}
				else if( ptTenderData->nTenderArea == GH_AREA_SAINTMORNING )
				{
					FLSPrintf( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSEENDNOTICE ), prj.GetText( TID_GAME_CONT_SAINTMORNING ) );
				}
				else if( ptTenderData->nTenderArea == GH_AREA_ELIUN )
				{
					FLSPrintf( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDHOUSEENDNOTICE ), prj.GetText( TID_GAME_CONT_ELIUN ) );
				}
			}
			break;

		case GH_TENDER_TENDER_RESULT:	// 입찰종료
			{
				FLStrcpy( m_szNoticeMessage, _countof( m_szNoticeMessage ), prj.GetText( TID_MMI_GHOUSE_GUILDBIDENDMASSAGE ) );
			}
			break;

		default:
			break;
		}
	}

	return m_szNoticeMessage;
}

void CGuildHouseMng::GetHighestTenderGuild( DWORD dwGHType )
{
	std::map<DWORD, DWORD> mapHighestTenderGuild;
	DWORD	dwHighestGuildId = 0;
	__int64	n64HighestPenya = 0;
	time_t	tHighestTime = 0;

	{
		for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
		{
			for( std::vector<T_GUILDHOUSE_TENDER>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
			{
				if( it2->chState == 'T' )
				{
					__int64 n64TenderPenya = static_cast<__int64>( ( static_cast<__int64>( it2->nTenderPerin ) * PERIN_VALUE ) + it2->nTenderPenya );
					if( n64TenderPenya > n64HighestPenya )
					{
						n64HighestPenya = n64TenderPenya;
						dwHighestGuildId = it2->dwGuildId;
						tHighestTime = it2->tTenderTime;
					}
					else if( n64TenderPenya == n64HighestPenya )
					{
						if( it2->tTenderTime < tHighestTime )
						{
							n64HighestPenya = n64TenderPenya;
							dwHighestGuildId = it2->dwGuildId;
							tHighestTime = it2->tTenderTime;
						}
					}
				}
			}
			if( dwHighestGuildId > 0 )
			{
				mapHighestTenderGuild.insert( std::map<DWORD, DWORD>::value_type( it->first, dwHighestGuildId ) );
			}
			dwHighestGuildId = 0;
			n64HighestPenya = static_cast<__int64>( 0 );
			tHighestTime = 0;
		}
	}

	{
		for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
		{
			volatile DWORD dwTenderGuildId = 0;
			std::map<DWORD, DWORD>::iterator it3 = mapHighestTenderGuild.find( it->first );
			if( it3 != mapHighestTenderGuild.end() )
			{
				dwTenderGuildId = it3->second;
			}

			for( std::vector<T_GUILDHOUSE_TENDER>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
			{
				if( it2->dwGHType == dwGHType )
				{
					if( it2->dwGuildId == dwTenderGuildId )
					{
						it2->chState = 'W';
					}
					else
					{
						it2->chState = 'L';
					}
				}
			}
		}
	}

	// tender end
	CAr ar;
	ar << dwGHType << mapHighestTenderGuild.size();
	for( std::map<DWORD, DWORD>::iterator it = mapHighestTenderGuild.begin(); it != mapHighestTenderGuild.end(); ++it )
	{
		ar << it->first << it->second;
	}
	u_long nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	GuildHouseDBMng->PostRequest( GUILDHOUSE_TENDER_END, lpBuf, nBufSize );
}
#endif // __DBSERVER

#ifdef __WORLDSERVER
bool CGuildHouseMng::ReqTenderGuildHouseList( FLWSUser* pUser, DWORD dwGHType, DWORD dwNpcIndex )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		if( GetTenderState( dwGHType ) != GH_TENDER_TENDER )
		{
			pUser->AddDefinedText( TID_MMI_GHOUSE_NOTBIDTERM );
			return FALSE;
		}

		PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
		if( ptTenderData != NULL )
		{
			std::map<DWORD, int>::iterator it = ptTenderData->mapNpc.find( dwNpcIndex );
			if( it != ptTenderData->mapNpc.end() )
			{
				if( GetTenderArea( dwGHType ) == it->second )
				{
					std::map<DWORD, DWORD> mapGHTenderList;
					GetTenderAbleGuildHouse( dwGHType, mapGHTenderList );
					pUser->AddGuildHouseTenderMainWnd( mapGHTenderList );
					return TRUE;
				}
				else
				{
					pUser->AddDefinedText( TID_MMI_GHOUSE_NOTBIDTWON );
				}
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid NpcId : [%d]" ), dwNpcIndex );
			}
		}
	}
	return FALSE;
}

void CGuildHouseMng::ReqTenderGuildHouseInfo( FLWSUser* pUser, DWORD dwGHIndex )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		PT_GUILDHOUSE_DATA ptGHData =  GetGuildHouseData( dwGHIndex );
		if( ptGHData != NULL )
		{
			if( GetTenderState( ptGHData->dwGHType ) != GH_TENDER_TENDER )
			{
				pUser->AddDefinedText( TID_MMI_GHOUSE_NOTBIDTERM );
				return;
			}
			PT_TENDER_DATA ptTenderData = GetTenderData( ptGHData->dwGHType );
			if( ptTenderData != NULL )
			{
				__int64 n64TenderPenya = 0;
				CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
				if( pGuild != NULL && pGuild->IsMaster( pUser->m_idPlayer ) == TRUE )
				{
					PT_GUILDHOUSE_TENDER ptGHTender = GetGuildHouseTender( pGuild->GetGuildId() );
					if( ptGHTender != NULL )
					{
						n64TenderPenya = static_cast<__int64>( ( static_cast<__int64>( ptGHTender->nTenderPerin ) * PERIN_VALUE ) + ptGHTender->nTenderPenya );
					}
				}
				std::vector<DWORD> vecTenderGuild = GetTenderView( dwGHIndex );
				pUser->AddGuildHouseTenderInfoWnd( dwGHIndex, ptTenderData->nMinPenya, n64TenderPenya, vecTenderGuild );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid Tender Data" ) );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid GuildHouse Data, GHId : [%d]" ), dwGHIndex );
		}
	}
}

void CGuildHouseMng::ReqGuildHouseInfo( FLWSUser* pUser, PT_PACKET_GUILDHOUSE_INFO ptGHInfo )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		PT_GUILDHOUSE_DATA ptGHData = GetGuildHouseData( ptGHInfo->dwGuildHouseNpcIndex );
		if( ptGHData != NULL )
		{
			ptGHInfo->dwGuildHouseTitleIndex = ptGHData->dwGHTitleIndex;

			CGuildHouseBase* pGuildHouse = GetGuildHouseByIndex( ptGHInfo->dwGuildHouseNpcIndex );
			if( pGuildHouse != NULL )
			{
				ptGHInfo->dwGuildId = pGuildHouse->GetGuildId();
				pGuildHouse->GetGuildHouseComment( ptGHInfo->szGuildHouseComment, _countof( ptGHInfo->szGuildHouseComment ) );
			}
			else
			{
				ptGHInfo->dwGuildId = NULL_ID;
			}
			pUser->AddGuildHouseInfoWnd( ptGHInfo );
		}
	}
}

void CGuildHouseMng::ReqGuildHouseCommentChange( FLWSUser* pUser, const PT_PACKET_GUILDHOUSE_INFO ptGHInfo )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
		if( pGuild != NULL )
		{
			if( pGuild->IsMaster( pUser->m_idPlayer ) == TRUE )
			{
				CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
				if( pGuildHouse != NULL )
				{
					pGuildHouse->SetGuildHouseComment( ptGHInfo->szGuildHouseComment );
					pUser->AddDefinedText( TID_MMI_GHOUSE_GUILDCHANGECOMPLETE );
				}
			}
			else
			{
				pUser->AddDefinedText( TID_MMI_GHOUSE_GUILDCHANGEFAILED );
			}
		}
	}
}

bool CGuildHouseMng::IsTenderAbleGuildHouse( DWORD dwGHType, DWORD dwGHIndex )
{
	MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.find( dwGHIndex );
	if( it != m_mapGuildHouseData.end() )
	{
		if( it->second.dwGHType == dwGHType && it->second.nState == GH_STATE_TENDERABLE )
		{
			return true;
		}
	}
	return false;
}

bool CGuildHouseMng::IsTenderAble( FLWSUser* pUser, DWORD dwGHIndex, int nTenderPerin, int nTenderPenya )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}
	
	PT_GUILDHOUSE_DATA ptGHData = GetGuildHouseData( dwGHIndex );
	if( ptGHData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid Guildhouse Data. GHId : [%d]" ), dwGHIndex );
		return FALSE;
	}

	PT_TENDER_DATA ptTenderData = GetTenderData( ptGHData->dwGHType );
	if( ptTenderData == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid Tender Data. GHType : [%d]" ), ptGHData->dwGHType );
		return FALSE;
	}

	if( GetTenderState( ptGHData->dwGHType ) != GH_TENDER_TENDER )
	{
		pUser->AddDefinedText( TID_MMI_GHOUSE_NOTBIDTERM );
		return FALSE;
	}

	CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild == NULL || pGuild->IsMaster( pUser->m_idPlayer ) == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_GHOUSE_NOTGUILDNOTMASTER );
		return FALSE;
	}

	if( GetGuildHouseTender( pGuild->m_idGuild ) != NULL )
	{
		pUser->AddDefinedText( TID_MMI_GHOUSE_ALREADYBID );
		return FALSE;
	}

	CGuildHouseBase* pGuildHouse = GetGuildHouse( pGuild->m_idGuild );
	if( pGuildHouse != NULL )
	{
		if( pGuildHouse->GetType() >= ptGHData->dwGHType )
		{
			pUser->AddDefinedText( TID_MMI_GHOUSE_ALREADYGHOUSE );
			return FALSE;
		}
		if( pGuildHouse->GetUpkeepTime() <= 0 )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_EXPIRATION );
			return FALSE;
		}
	}
	else
	{
		pUser->AddDefinedText( TID_MMI_GHOUSE_GHOUSTNOTFOUND );
		return FALSE;
	}

	if( IsTenderAbleGuildHouse( ptGHData->dwGHType, dwGHIndex ) == FALSE )
	{
		pUser->AddDefinedText( TID_MMI_GHOUSE_NOTBIDGHOUSE );
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid GHId. User : [%07d], GHId : [%d]" ), pUser->m_idPlayer, dwGHIndex );
		return FALSE;
	}

	if( nTenderPerin > 0 )
	{
		const int nHavePerin	= pUser->GetUsableItemQuantityByItemID( ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) );

		if( nHavePerin < nTenderPerin )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
			return FALSE;
		}
	}

	if( pUser->CheckUserGold( nTenderPenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
		return FALSE;
	}

	__int64 n64TotalPenya = static_cast<__int64>( ( PERIN_VALUE * static_cast<__int64>( nTenderPerin ) ) + nTenderPenya );
	if( n64TotalPenya < static_cast<__int64>( ptTenderData->nMinPenya ) )
	{
		pUser->AddDefinedText( TID_MMI_GHOUSE_SMALLMONEY );
		return FALSE;
	}

	return TRUE;
}

void CGuildHouseMng::ReqGuildHouseTenderJoin( FLWSUser* pUser, DWORD dwGHIndex, int nTenderPerin, int nTenderPenya )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	if( IsTenderAble( pUser, dwGHIndex, nTenderPerin, nTenderPenya ) == FALSE )
	{
		return;
	}

	if( nTenderPerin > 0 )
	{
		const int nRemain = pUser->RemoveItemByItemID( ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ), nTenderPerin, "G", "GUILDHOUSE_TENDER_PERIN" );
		if( nRemain != 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR. INVALID PLAYER HAVE PERIN ITEM. PlayerID:(%07d) ]" ), pUser->m_idPlayer );
			return;
		}
	}

	pUser->AddGold( -nTenderPenya );
	g_DPSrvr.PutPenyaLog( pUser, "G", "GUILDHOUSE_TENDER_PENYA", nTenderPenya );

	BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_JOIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	T_GUILDHOUSE_TENDER GHT( dwGHIndex, pUser->m_idGuild, pUser->m_idPlayer, nTenderPerin, nTenderPenya );
	GHT.Serialize( ar );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
}

CGuildHouseMng::PT_GUILDHOUSE_TENDER CGuildHouseMng::GetGuildHouseTender( DWORD dwGuildId )
{
	for( MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.begin(); it != m_mapGuildHouseTender.end(); ++it )
	{
 		for( int i = 0; i < (int)it->second.size(); ++i )
 		{
 			if( dwGuildId == (it->second).at( i ).dwGuildId )
 			{
 				return &( (it->second).at( i ) );
 			}
 		}
	}
	return NULL;
}

std::vector<DWORD> CGuildHouseMng::GetTenderView( DWORD dwGHIndex )
{
	std::vector<DWORD> vecGuildId;

	MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.find( dwGHIndex );
	if( it != m_mapGuildHouseTender.end() )
	{
		for( std::vector<T_GUILDHOUSE_TENDER>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
		{
			vecGuildId.push_back( it2->dwGuildId );
		}
	}
	return vecGuildId;
}

void CGuildHouseMng::OnGuildHouseTenderInfo( CAr & ar )
{
	DWORD dwGHType;
	int nTenderId, nTenderArea, nTenderState;
	ar >> dwGHType >> nTenderId >> nTenderArea >> nTenderState;

	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		ptTenderData->nTenderId = nTenderId;
		ptTenderData->nTenderArea = nTenderArea;
		ptTenderData->nTenderState = nTenderState;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TenderData Not Found. GHType : %d" ), dwGHType );
	}

	
	BOOL bTender;
	ar >> bTender;
	if( bTender == TRUE )
	{
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; ++i )
		{
			DWORD dwGHIndex;
			ar >> dwGHIndex;
			MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.find( dwGHIndex );
			if( it != m_mapGuildHouseData.end() )
			{
				it->second.nState = GH_STATE_TENDERABLE;
			}
		}
	}
}

void CGuildHouseMng::OnGuildHouseTenderJoin( CAr & ar )
{
	DWORD dwPlayerId;
	DWORD dwGHIndex;
	BOOL bTender;

	ar >> dwPlayerId >> dwGHIndex >> bTender;
	if( bTender == TRUE )
	{
		T_GUILDHOUSE_TENDER tGHTender;
		tGHTender.Serialize( ar );

		if( AddGuildHouseTender( &tGHTender ) == true )
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
			if( IsValidObj( pUser ) == TRUE )
			{
				pUser->AddGuildHouseTenderResult( dwGHIndex, TRUE );
				pUser->AddDefinedText( TID_MMI_GHOUSE_BIDSUCCESS );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "AddGuildHouseTender Failed. [User : %07d, Guild : %06d, GHId : %d]" ), dwPlayerId, tGHTender.dwGuildId, dwGHIndex ); 
		}
	}
	else
	{
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) == TRUE )
		{
			pUser->AddGuildHouseTenderResult( dwGHIndex, FALSE );
			pUser->AddDefinedText( TID_MMI_GHOUSE_BIDFAILED );
		}
	}
}

void CGuildHouseMng::OnGuildHouseTenderState( CAr & ar )
{
	DWORD dwGHType;
    int nState;
	ar >> dwGHType >> nState;

	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		ptTenderData->nTenderState = nState;

		if( nState == GH_TENDER_CLOSE )
		{
			CloseTender( dwGHType );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TenderData Not Found. GHType : %d" ), dwGHType );
	}
}

void CGuildHouseMng::OnGuildHouseGradeUpdate( CAr & ar )
{
	DWORD dwGuildId, dwWorldId;
	ar >> dwGuildId >> dwWorldId;

	CGuildHouseBase* pGuildHouse = ReMakeGuildHouse( dwGuildId, dwWorldId );
	if( pGuildHouse != NULL )
	{
		pGuildHouse->SerializeAllInfo( ar );

		CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
		if( pGuild != NULL )
		{
			for( std::map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
			{
				FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
				if( IsValidObj( pUser ) == TRUE )
				{
					pUser->AddGuildHouseAllInfo( pGuildHouse );
				}
			}
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "ReMakeGuildHouse Failed. Guild : [%06d], World : [%d]" ), dwGuildId, dwWorldId ); 
		CGuildHouseSmall temp( NULL_ID );
		temp.SerializeAllInfo( ar );
	}
}

void CGuildHouseMng::OnGuildHouseLevelUpdate( CAr & ar )
{
	DWORD dwPlayerId;
	BOOL bLevelUp;
	ar >> dwPlayerId >> bLevelUp;

	if( bLevelUp == TRUE )
	{
		DWORD dwGuildId;
		int nLevel;
		ar >> dwGuildId >> nLevel;

		CGuildHouseBase* pGuildHouse = GetGuildHouse( dwGuildId );
		if( pGuildHouse != NULL )
		{
			pGuildHouse->SetGuildHouseLevel( nLevel );

			RemoveGuildHouseDoor( pGuildHouse );

			FLWSUser* pUser = NULL;
			CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
			if( pGuild != NULL )
			{
				for( std::map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); ++it )
				{
					pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
					if( IsValidObj( pUser ) == TRUE )
					{
						pUser->AddDefinedText( TID_MMI_GHOUSE_GHOUSELEVELUP );
						pUser->AddGuildHouseAllInfo( pGuildHouse );
					}
				}
			}
		}
	}
	else
	{
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) == TRUE )
		{
			pUser->AddDefinedText( TID_MMI_GHOUSE_GUILDLEVELUPFAILED );
		}
	}
}

void CGuildHouseMng::OnGuildHouseExpired( CAr & ar )
{
	DWORD dwGuildId;
	ar >> dwGuildId;

	CGuildHouseBase* pGuildHouse = GetGuildHouse( dwGuildId );
	if( pGuildHouse != NULL )
	{
		if( pGuildHouse->GetType() == WI_GUILDHOUSE_SMALL )
		{
			pGuildHouse->ExpiredGuildHouse();
		}
		else if( pGuildHouse->GetType() >= WI_GUILDHOUSE_MIDDLE && pGuildHouse->GetType() <= WI_GUILDHOUSE_LARGE )
		{
			// remake
			pGuildHouse = ReMakeGuildHouse( dwGuildId, WI_GUILDHOUSE_SMALL );
			if( pGuildHouse != NULL )
			{
				pGuildHouse->SerializeAllInfo( ar );

				CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
				if( pGuild != NULL )
				{
					FLWSUser* pUser = NULL;
					for( std::map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); ++it )
					{
						pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
						if( IsValidObj( pUser ) == TRUE )
						{
							pUser->AddGuildHouseAllInfo( pGuildHouse );
							pGuildHouse->SetApplyDST( pUser );
						}
					}
				}
			}
		}
	}
}

void CGuildHouseMng::RemoveGuildHouseDoor( CObj* pObj, DWORD dwWorldId, int nLayer )
{
	if( pObj != NULL && IsGuildHouse( dwWorldId ) == TRUE && nLayer > 0 )
	{
		CGuildHouseBase* pGuildHouse = GetGuildHouse( nLayer );
		if( pGuildHouse != NULL && pGuildHouse->GetType() != WI_GUILDHOUSE_SMALL )
		{
			for( MAP_T_GUILDHOUSE_QUEST::iterator it = m_mapGHQuest.begin(); it != m_mapGHQuest.end(); ++it )
			{
				if( it->second.nLevel <= pGuildHouse->GetGuildHouseLevel() )
				{
					if( pObj->GetType() == OT_CTRL && pObj->GetIndex() == it->second.dwDoorIndex )
					{
						pObj->Delete();
					}
				}
			}
		}
	}
}

void CGuildHouseMng::RemoveGuildHouseDoor( CGuildHouseBase* pGuildHouse )
{
	if( pGuildHouse != NULL && pGuildHouse->GetType() != WI_GUILDHOUSE_SMALL )
	{
		CWorld* pWorld = g_WorldMng.GetWorld( pGuildHouse->GetType() );
		if( pWorld == NULL || pWorld->m_linkMap.GetLinkMap( static_cast<int>( pGuildHouse->GetGuildId() ) ) == NULL )
		{
			return;
		}

		for( MAP_T_GUILDHOUSE_QUEST::iterator it = m_mapGHQuest.begin(); it != m_mapGHQuest.end(); ++it )
		{
			if( it->second.nLevel <= pGuildHouse->GetGuildHouseLevel() )
			{
				int nWidth = pWorld->WORLD_WIDTH * pWorld->m_iMPU;
				int nHeight = pWorld->WORLD_HEIGHT * pWorld->m_iMPU;
				D3DXVECTOR3 vPosCenter( float( nWidth / 2 ), 0.0f, float( nHeight / 2 ) );
				int nRange = max( nWidth, nHeight ) / 2;
				CObj* pObj;
				FOR_LINKMAP( pWorld, vPosCenter, pObj, nRange, CObj::linkDynamic, static_cast<int>( pGuildHouse->GetGuildId() ) )
				{
					if( pObj->GetType() == OT_CTRL && pObj->GetIndex() == it->second.dwDoorIndex )
					{
						pObj->Delete();
					}
				}
				END_LINKMAP
			}
		}
	}
}

CGuildHouseBase* CGuildHouseMng::ReMakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId )
{
	// 해당 길드하우스 내에 있는 유저를 밖으로 보내고 보자..
	CGuildHouseBase* pGuildHouse = GetGuildHouse( dwGuildId );
	CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
	if( pGuildHouse != NULL && pGuild != NULL )
	{
		CGuildMember* pMember = NULL;
		FLWSUser* pUser = NULL;
		for( std::map<u_long, CGuildMember*>::iterator it = pGuild->m_mapPMember.begin(); it != pGuild->m_mapPMember.end(); ++it )
		{
			pMember = it->second;
			if( pMember != NULL )
			{
				pUser = g_xWSUserManager->GetUserByPlayerID( pMember->m_idPlayer );
                pGuildHouse->GoOutGuildHouseRoom( pUser );
			}

			pMember = NULL;
			pUser = NULL;
		}
	}

	if( RemoveGuildHouse( dwGuildId ) == TRUE )
	{
		CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, dwWorldId );
		if( pGuildHouse != NULL && AddGuildHouse( dwGuildId, pGuildHouse ) == TRUE )
		{
			return pGuildHouse;
		}
		else
		{
			SAFE_DELETE( pGuildHouse );
			FLERROR_LOG( PROGRAM_NAME, _T( "MakeGuildHouse Failed. Guild : [%06d], World : [%d]" ), dwGuildId, dwWorldId );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "RemoveGuildHouse Failed. Guild : [%06d], World : [%d]" ), dwGuildId, dwWorldId );
	}
	return NULL;
}

CGuildHouseMng::PT_GUILDHOUSE_QUEST CGuildHouseMng::GetGuildHouseQuestData( int nQuestId )
{
	MAP_T_GUILDHOUSE_QUEST::iterator it = m_mapGHQuest.find( nQuestId );
	if( it != m_mapGHQuest.end() )
	{
		return &(it->second);
	}
	return NULL;
}

void CGuildHouseMng::CheckGuildHouseQuest( FLWSUser* pUser, int nQuestId )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		if( pUser->IsCompleteQuest( nQuestId ) == FALSE )
		{
			return;
		}
		PT_GUILDHOUSE_QUEST ptGHQuest = GetGuildHouseQuestData( nQuestId );
		if( ptGHQuest != NULL )
		{
			CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild != NULL )
			{
				CGuildHouseBase* pGuildHouse = GetGuildHouse( pGuild->GetGuildId() );
				if( pGuildHouse != NULL )
				{
					if( pGuildHouse->GetGuildHouseLevel() < ptGHQuest->nLevel )
					{
						// send trans
						BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
						ar << pUser->m_idPlayer << pGuild->GetGuildId() << ptGHQuest->nLevel;
						SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
					}
					else
					{
					}
				}
			}
		}
	}
}

void CGuildHouseMng::CheckGuildHouseQuest( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
		if( pGuild != NULL && pGuild->IsMaster( pUser->m_idPlayer ) == TRUE )
		{
			CGuildHouseBase* pGuildHouse = GetGuildHouse( pGuild->GetGuildId() );
			if( pGuildHouse != NULL && pGuildHouse->GetType() != WI_GUILDHOUSE_SMALL )
			{
				for( MAP_T_GUILDHOUSE_QUEST::iterator it = m_mapGHQuest.begin(); it != m_mapGHQuest.end(); ++it )
				{
					PT_GUILDHOUSE_QUEST ptGHQuest = &(it->second);
					if( ptGHQuest != NULL )
					{
						if( pGuildHouse->GetGuildHouseLevel() < ptGHQuest->nLevel )
						{
							pUser->RemoveQuest( it->first );
						}
					}
				}
			}
		}
	}
}

#endif // __WORLDSERVER

void CGuildHouseMng::LoadScript()
{
	const TCHAR szFileName[]	= _T( "GuildHouseTender.lua" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( m_Lua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		exit(0);
	}

	m_Lua.GetGloabal( "tTender" );
	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		DWORD dwGHType				= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strGHType" ) ) );
		T_TENDER_DATA tTenderData;
		tTenderData.nMaxHouse		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nMaxHouse" ) );
		tTenderData.nMinPenya		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nMinPenya" ) );
		tTenderData.nChargeRate		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nChargeRate" ) );
		tTenderData.dwPrevNoticeTick = static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nPrevNoticeTick" ) );
		tTenderData.dwTenderNoticeTick = static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nTenderNoticeTick" ) );
		MAP_T_TENDER_DATA::iterator it = m_mapTenderData.insert( MAP_T_TENDER_DATA::value_type( dwGHType, tTenderData ) ).first;

		m_Lua.GetField( -1, "tNpc" );
		m_Lua.PushNil();
		while( m_Lua.TableLoop( -2 ) )
		{
			DWORD dwNpcIndex	= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strNpcId" ) ) );
			int nAreaIndex	= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nAreaIndex" ) );
			if( prj.GetMoverProp( dwNpcIndex ) == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid GuildHouse NpcId : %d" ), dwNpcIndex );
				FLLOG_FLUSH();
				ExitProcess( 0 );
			}
			it->second.mapNpc.insert( std::map<DWORD, int>::value_type( dwNpcIndex, nAreaIndex ) );

			m_Lua.Pop( 1 );
		}
		m_Lua.Pop( 1 );

		m_Lua.Pop( 1 );
	}

	m_Lua.GetGloabal( "tGHInfo" );
	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		T_GUILDHOUSE_DATA tGHData;
		tGHData.dwGHIndex		= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strGHId" ) ) );
		tGHData.dwNpcIndex		= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strNpcId" ) ) );
		tGHData.dwGHType		= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strGHType" ) ) );
		tGHData.nTenderArea		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nAreaIndex" ) );
		tGHData.dwGHTitleIndex	= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strTitle" ) ) );
		m_mapGuildHouseData.insert( MAP_T_GUILDHOUSE_DATA::value_type( tGHData.dwGHIndex, tGHData ) );
		m_Lua.Pop( 1 );
	}
#ifdef __WORLDSERVER
	m_Lua.GetGloabal( "tGHQuest" );
	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		int nQuestId			= static_cast<OBJID>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strQuestId" ) ) );
		T_GUILDHOUSE_QUEST tGHQuest;
		tGHQuest.dwGHType		= static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "dwGHType" ) );
		tGHQuest.nLevel			= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nLevel" ) );
		tGHQuest.dwDoorIndex	= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strDoorId" ) ) );
		m_mapGHQuest.insert( MAP_T_GUILDHOUSE_QUEST::value_type( nQuestId, tGHQuest ) );
		m_Lua.Pop( 1 );
	}
#endif // __WORLDSERVER

	m_Lua.Pop( 0 );
}

void CGuildHouseMng::SetTenderInfo( DWORD dwGHType, int nTenderId, int nTenderArea )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		ptTenderData->nTenderId		= nTenderId;
		ptTenderData->nTenderArea	= nTenderArea;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "TenderData Not Found. GHType : %d" ), dwGHType );
	}
}

int CGuildHouseMng::GetTenderArea( DWORD dwGHType )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		return ptTenderData->nTenderArea;
	}
	return 0;
}

int CGuildHouseMng::GetTenderId( DWORD dwGHType )
{
	PT_TENDER_DATA ptTenderData = GetTenderData( dwGHType );
	if( ptTenderData != NULL )
	{
		return ptTenderData->nTenderId;
	}
	return 0;
}

CGuildHouseMng::PT_TENDER_DATA CGuildHouseMng::GetTenderData( DWORD dwGHType )
{
	MAP_T_TENDER_DATA::iterator it = m_mapTenderData.find( dwGHType );
	if( it != m_mapTenderData.end() )
	{
		return &(it->second);
	}
	return NULL;
}

CGuildHouseMng::PT_GUILDHOUSE_DATA CGuildHouseMng::GetGuildHouseData( DWORD dwIndex )
{
	for( MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.begin(); it != m_mapGuildHouseData.end(); ++it )
	{
		if( it->second.dwGHIndex == dwIndex || it->second.dwNpcIndex == dwIndex )
		{
			return &(it->second);
		}
	}
	return NULL;
}

bool	CGuildHouseMng::GetTenderAbleGuildHouse( DWORD dwGHType, std::map<DWORD, DWORD> & mapGHTenderList )
{
	int nTenderArea = GetTenderArea( dwGHType );
	if( nTenderArea > 0 )
	{
		for( MAP_T_GUILDHOUSE_DATA::iterator it = m_mapGuildHouseData.begin(); it != m_mapGuildHouseData.end(); ++it )
		{
			if( it->second.nTenderArea == nTenderArea && it->second.nState == GH_STATE_TENDERABLE )
			{
				mapGHTenderList.insert( std::map<DWORD, DWORD>::value_type( it->second.dwGHIndex, it->second.dwGHTitleIndex ) );
			}
		}
	}
	if( mapGHTenderList.empty() == false )
	{
		return true;
	}
	return false;
}

bool	CGuildHouseMng::AddGuildHouseTender( const PT_GUILDHOUSE_TENDER ptGHTender )
{
	if( ptGHTender == NULL )
	{
		return false;
	}

	bool bResult = false;

	MAP_T_GUILDHOUSE_TENDER::iterator it = m_mapGuildHouseTender.find( ptGHTender->dwGHIndex );
	if( it != m_mapGuildHouseTender.end() )
	{
		it->second.push_back( (*ptGHTender) );
		bResult = true;
	}
	else
	{
		std::vector<T_GUILDHOUSE_TENDER> vecGHTender;
		vecGHTender.push_back( (*ptGHTender) );
		bResult = m_mapGuildHouseTender.insert( MAP_T_GUILDHOUSE_TENDER::value_type( ptGHTender->dwGHIndex, vecGHTender ) ).second;
	}

	return bResult;
}

#endif // __GUILD_HOUSE_MIDDLE

#endif // __CLIENT
