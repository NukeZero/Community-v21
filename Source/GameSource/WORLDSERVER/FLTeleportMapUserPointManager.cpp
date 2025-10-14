
#include "StdAfx.h"
#include "FLTeleportMapUserPointManager.h"

#include "FLItemUsing.h"
#include "../_Common/worldmng.h"

extern	CWorldMng		g_WorldMng;


FLTeleportMapUserPointManager*	FLTeleportMapUserPointManager::GetInstance()
{
	static FLTeleportMapUserPointManager kTeleportMapUserPointManager;
	return &kTeleportMapUserPointManager;
}

FLTeleportMapUserPointManager::FLTeleportMapUserPointManager()
{
}

FLTeleportMapUserPointManager::~FLTeleportMapUserPointManager()
{
	Clear();
}

bool	FLTeleportMapUserPointManager::Initialize()
{
	return true;
}

void	FLTeleportMapUserPointManager::Shutdown()
{
	Clear();
}

bool	FLTeleportMapUserPointManager::Add( OBJID dwUserID, const std::vector< T_TELEPORTMAP_POINT >* pUserPoints )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return false;
	}

	FLTeleportMapUserPoints* pTeleportMapUserPoints = Find( dwUserID );
	if( pTeleportMapUserPoints == NULL )
	{
		pTeleportMapUserPoints = new FLTeleportMapUserPoints;
		if( pTeleportMapUserPoints == NULL )
		{
			return false;
		}

		pTeleportMapUserPoints->Create( dwUserID, pUserPoints );
		if( m_mapUserPoints.insert( USER_POINTS_LIST::value_type( pTeleportMapUserPoints->GetUserID(), pTeleportMapUserPoints ) ).second == false )
		{
			SAFE_DELETE( pTeleportMapUserPoints );
			return false;
		}
	}
	else
	{
		pTeleportMapUserPoints->Create( dwUserID, pUserPoints );
	}

	return true;
}

void	FLTeleportMapUserPointManager::Del( OBJID dwUserID )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	USER_POINTS_ITER pos = m_mapUserPoints.find( dwUserID );
	if( pos != m_mapUserPoints.end() )
	{
		FLTeleportMapUserPoints* pDelete = pos->second;
		SAFE_DELETE( pDelete );
		m_mapUserPoints.erase( pos );
	}
}

FLTeleportMapUserPoints*	FLTeleportMapUserPointManager::Find( OBJID dwUserID )
{
	FLTeleportMapUserPoints* pFind = NULL;

	USER_POINTS_ITER pos = m_mapUserPoints.find( dwUserID );
	if( pos != m_mapUserPoints.end() )
	{
		pFind = pos->second;
	}

	return pFind;
}

void	FLTeleportMapUserPointManager::Clear()
{
	USER_POINTS_ITER pos = m_mapUserPoints.begin();
	USER_POINTS_ITER end = m_mapUserPoints.end();

	for( ; pos != end; ++pos )
	{
		FLTeleportMapUserPoints* pDelete = pos->second;
		SAFE_DELETE( pDelete );
	}

	m_mapUserPoints.clear();

	MEMPOOLER_DESTROY( FLTeleportMapUserPoints );
}

bool	FLTeleportMapUserPointManager::CanUseTeleportMapItem( FLWSUser* pUser, DWORD /*dwItemObjID*/ )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지도 아이템을 사용 할 수 없는 ContentSwitch 설정입니다. User: %s(%d)" ), pUser->GetName(), pUser->m_idPlayer );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	if( pUser->IsDie() == TRUE )	// 죽었다면
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
		return false;
	}

	if( pUser->m_bAllAction == FALSE )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
		return false;
	}

	if( pUser->m_pActMover->IsActAttack() != FALSE )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
		return false;
	}

	if( pUser->m_pActMover->IsState( OBJSTA_STAND ) == FALSE )	// 서있는 상태가 아니면
	{
		pUser->AddDefinedText( TID_PK_STAND_LIMIT );
		return false;
	}

	if( pUser->m_vtInfo.IsVendorOpen() == TRUE )	// 개인 상점
	{
		pUser->AddDefinedText( TID_GAME_NOTTRADE );
		return false;
	}

	if( pUser->IsCollecting() == TRUE )	// 채집
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT_NOT_USE );
		return false;
	}

	if( pUser->IsFly() == TRUE )	// 비행 상태
	{
		pUser->AddDefinedText( TID_PK_STAND_LIMIT );
		return false;
	}

	if( !g_pItemUsing->canUseTeleportation( pUser ) ) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool	FLTeleportMapUserPointManager::CanTeleport( FLWSUser* /*pUser*/, DWORD dwWorldID, const D3DXVECTOR3& tPos )
{
	CWorld* pWorld = g_WorldMng.GetWorld( dwWorldID );
	if( pWorld == NULL )
	{
		return false;
	}

	int nAttr = pWorld->GetHeightAttribute( tPos.x, tPos.z );
	if( nAttr != HATTR_NONE )
	{
		return false;
	}

	// TODO 보강하자
	//pUser->IsRegionAttr(

	return true;
}

bool	FLTeleportMapUserPointManager::OnDoUseTeleportMapItem( FLWSUser* pUser, DWORD dwItemObjID, PT_ITEM_SPEC pItemProp )
{
	if( CanUseTeleportMapItem( pUser, dwItemObjID ) == false )
	{
		return false;
	}

	FLSnapshotTeleportMapOpenNoti kToClient;
	kToClient.dwItemObjID = dwItemObjID;

	if( pItemProp->dwItemKind3 == IK3_TELEPORTMAP_BASIC )	// 일반형 텔레포트 지도
	{
		kToClient.byType = TELEPORT_MAP_TYPE_SPEC;
	}
	else if( pItemProp->dwItemKind3 == IK3_TELEPORTMAP_USER )	// 유저 등록형 텔레포트 지도
	{
		kToClient.byType = TELEPORT_MAP_TYPE_USER;

		FLTeleportMapUserPoints* pUserPoint = Find( pUser->m_idPlayer );
		if( pUserPoint == NULL )
		{
			std::vector< T_TELEPORTMAP_POINT > vecEmptyPoints;
			if( Add( pUser->m_idPlayer, &vecEmptyPoints ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "유저 등록 지점 리스트를 생성할 수 없습니다. User: %s(%d)" ), pUser->GetName(), pUser->m_idPlayer );
				return false;
			}

			pUserPoint = Find( pUser->m_idPlayer );
		}

		if( pUserPoint != NULL )
		{
			pUserPoint->GetAllUserPoint( &kToClient.vecUserPoints );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "유저 등록 지점 리스트를 찾을 수 없습니다. User: %s(%d)" ), pUser->GetName(), pUser->m_idPlayer );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 텔레포트 지도 아이템입니다. Item: %s, IK3: %d" ), pItemProp->szName, pItemProp->dwItemKind3 );
		return false;
	}

	pUser->AddPacket( &kToClient );
	return false;
}

void	FLTeleportMapUserPointManager::HandleTeleportMapToPoint( FLWSUser* pUser, const FLPacketTeleportMapToPointReq* pMsg )
{
	if( g_pItemUsing->CheckUseItem( pUser, pMsg->dwItemObjID ) == FALSE )
	{
		return;
	}

	if( CanUseTeleportMapItem( pUser, pMsg->dwItemObjID ) == false )
	{
		return;
	}

	FLItemBase* pItemBase = pUser->GetItemId( pMsg->dwItemObjID ); 
	if( IsUsableItem( pItemBase ) == FALSE )
	{
		return;
	}

//	FLItemElem* pItemElem = (FLItemElem*)pItemBase;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemBase->m_dwItemId );
	if( pItemProp == NULL )
	{
		return;
	}

	switch( pItemProp->dwItemKind2 )
	{
	case IK2_TELEPORTMAP:
		{
			T_WORLD_POSITION tPosition;
			//::mem_set( &tPosition, 0, sizeof( tPosition ) );

			switch( pItemProp->dwItemKind3 )
			{
			case IK3_TELEPORTMAP_BASIC:
				{
					if( pMsg->byType == TELEPORT_MAP_TYPE_SPEC )
					{
						PT_TELEPORTMAP_POINT pSpec = g_xSpecManager->GetSpecTeleportMapPoint( pMsg->dwTeleportID );
						if( pSpec == NULL )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지점 스펙을 찾을 수 없습니다. TeleportID: %d" ), pMsg->dwTeleportID );
							return;
						}

						tPosition = pSpec->tPosition;
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "일반 텔레포트 아이템 패킷의 타입 정보가 잘못되었습니다. User: %s, Type: %d" ), pUser->GetName(), pMsg->byType );
						return;
					}
				}
				break;
			case IK3_TELEPORTMAP_USER:
				{
					if( pMsg->byType == TELEPORT_MAP_TYPE_SPEC )
					{
						PT_TELEPORTMAP_POINT pSpec = g_xSpecManager->GetSpecTeleportMapPoint( pMsg->dwTeleportID );
						if( pSpec == NULL )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지점 스펙을 찾을 수 없습니다. TeleportID: %d" ), pMsg->dwTeleportID );
							return;
						}

						tPosition = pSpec->tPosition;
					}
					else if( pMsg->byType == TELEPORT_MAP_TYPE_USER )
					{
						FLTeleportMapUserPoints* pUserPoint = Find( pUser->m_idPlayer );
						if( pUserPoint == NULL )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s" ), pUser->GetName() );
							return;
						}

						const T_TELEPORTMAP_POINT* pPoint = pUserPoint->Find( pMsg->dwTeleportID );
						if( pPoint == NULL )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s, TeleportID: %d" ), pUser->GetName(), pMsg->dwTeleportID );
							return;
						}

						tPosition = pPoint->tPosition;
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "유저 텔레포트 아이템 패킷의 타입 정보가 잘못되었습니다. User: %s, Type: %d" ), pUser->GetName(), pMsg->byType );
						return;
					}
				}
				break;

			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 유형의 텔레포트 지도입니다. Item: %s, IK3: %d" ), pItemProp->szName, pItemProp->dwItemKind3 );
					return;
				}
				break;
			}

			if( tPosition.dwWorldID != 0 )
			{
				if( CanTeleport( pUser, tPosition.dwWorldID, tPosition.tPos ) == false )
				{
					pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT04 );
					FLERROR_LOG( PROGRAM_NAME, _T( "이동할 수 없는 지점입니다. User: %s, WorldID: %d, (%f, %f, %f)" ), pUser->GetName(), tPosition.dwWorldID, tPosition.tPos.x, tPosition.tPos.y, tPosition.tPos.z );
					return;
				}

				if( pUser->REPLACE( g_uIdofMulti, tPosition.dwWorldID, tPosition.tPos, REPLACE_NORMAL, nDefaultLayer ) == FALSE )
				{
					pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT04 );
					FLERROR_LOG( PROGRAM_NAME, _T( "Replace 실패 User: %s, WorldID: %d, Layer: %d, (%f, %f, %f)" ), pUser->GetName(), tPosition.dwWorldID, nDefaultLayer, tPosition.tPos.x, tPosition.tPos.y, tPosition.tPos.z );
					return;
				}

				FLSnapshotTeleportMapToPointAck kToClient;
				pUser->AddPacket( &kToClient );
			}
		}
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지도가 아닙니다. Item: %s, IK2: %d" ), pItemProp->szName, pItemProp->dwItemKind2 );
			return;
		}
		break;
	}

	g_pItemUsing->CompleteUseItem( pUser, pItemBase->m_dwObjId );
}

void	FLTeleportMapUserPointManager::HandleTeleportMapAddMyPoint( FLWSUser* pUser, const FLPacketTeleportMapAddMyPointReq* pMsg )
{
	if( g_pItemUsing->CheckUseItem( pUser, pMsg->dwItemObjID ) == FALSE )
	{
		return;
	}

	if( CanUseTeleportMapItem( pUser, pMsg->dwItemObjID ) == false )
	{
		return;
	}

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	// 주의
	// 이 제한을 풀려면 World의 Layer에 대한 이해가 필요하다.
	// Layer로 인해 DB와 Teleport Spec에 Layer를 추가해야 할지도 모른다.
	if( pWorld->GetID() != WI_WORLD_MADRIGAL )
	{
		pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT01 );
		return;
	}

	FLItemBase* pItemBase = pUser->GetItemId( pMsg->dwItemObjID ); 
	if( IsUsableItem( pItemBase ) == FALSE )
	{
		return;
	}

//	FLItemElem* pItemElem = (FLItemElem*)pItemBase;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemBase->m_dwItemId );
	if( pItemProp == NULL )
	{
		return;
	}

	switch( pItemProp->dwItemKind2 )
	{
	case IK2_TELEPORTMAP:
		{
			switch( pItemProp->dwItemKind3 )
			{
			case IK3_TELEPORTMAP_USER:
				{
					FLTeleportMapUserPoints* pUserPoint = Find( pUser->m_idPlayer );
					if( pUserPoint == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s" ), pUser->GetName() );
						return;
					}

					CWorld* pWorld = pUser->GetWorld();
					if( pWorld != NULL )
					{
						T_TELEPORTMAP_POINT tUserPoint = { 0 };
						//::mem_set( &tUserPoint, 0, sizeof( tUserPoint ) );

						tUserPoint.dwIndex = pUserPoint->GetEmptyID();
						if( tUserPoint.dwIndex == 0 )
						{
							pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT02 );
							FLERROR_LOG( PROGRAM_NAME, _T( "더 이상 텔레포트 유저 등록 지점을 등록할 수 없습니다. User: %s" ), pUser->GetName() );
							return;
						}

						tUserPoint.byType = TELEPORT_MAP_TYPE_USER;
						tUserPoint.tPosition.dwWorldID = pWorld->GetID();
						tUserPoint.tPosition.tPos = pUser->GetPos();
						FLStrcpy( tUserPoint.szName, _countof( tUserPoint.szName ), prj.GetText( TID_MMI_TELEPORTUSERPOINT ) );

						if( CanTeleport( pUser, tUserPoint.tPosition.dwWorldID, tUserPoint.tPosition.tPos ) == false )
						{
							pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT01 );
							FLERROR_LOG( PROGRAM_NAME, _T( "이곳은 등록할 수 없습니다. User: %s World: %d, (%f, %f, %f)" ), pUser->GetName(), tUserPoint.tPosition.dwWorldID, tUserPoint.tPosition.tPos.x, tUserPoint.tPosition.tPos.y, tUserPoint.tPosition.tPos.z );
							return;
						}

						if( pUserPoint->IsNearPos( &tUserPoint.tPosition, NEAR_DISTANCE ) == true )
						{
							pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT05 );
							FLERROR_LOG( PROGRAM_NAME, _T( "이전 등록지점과 너무 가깝습니다. User: %s World: %d, (%f, %f, %f)" ), pUser->GetName(), tUserPoint.tPosition.dwWorldID, tUserPoint.tPosition.tPos.x, tUserPoint.tPosition.tPos.y, tUserPoint.tPosition.tPos.z );
							return;
						}

						if( pUserPoint->Add( &tUserPoint ) == false )
						{
							pUser->AddDefinedText( TID_MMI_TELEPORTERRORTEXT02 );
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점 등록 실패. User: %s" ), pUser->GetName() );
							return;
						}

						FLPacketTeleportMapAddPointNoti kToDB;
						kToDB.dwPlayerID = pUser->m_idPlayer;
						kToDB.tUserPoint = tUserPoint;
						g_dpDBClient.SendPacket( &kToDB );

						FLSnapshotTeleportMapAddMyPointAck kToClient;
						kToClient.tUserPoint = tUserPoint;
						pUser->AddPacket( &kToClient );
					}
				}
				break;

			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 유형의 텔레포트 지도입니다. Item: %s, IK3: %d" ), pItemProp->szName, pItemProp->dwItemKind3 );
					return;
				}
				break;
			}
		}
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지도가 아닙니다. Item: %s, IK2: %d" ), pItemProp->szName, pItemProp->dwItemKind2 );
			return;
		}
		break;
	}
}

void	FLTeleportMapUserPointManager::HandleTeleportMapChgPointName( FLWSUser* pUser, const FLPacketTeleportMapChgPointNameReq* pMsg )
{
	if( g_pItemUsing->CheckUseItem( pUser, pMsg->dwItemObjID ) == FALSE )
	{
		return;
	}

	if( CanUseTeleportMapItem( pUser, pMsg->dwItemObjID ) == false )
	{
		return;
	}

	FLItemBase* pItemBase = pUser->GetItemId( pMsg->dwItemObjID ); 
	if( IsUsableItem( pItemBase ) == FALSE )
	{
		return;
	}

//	FLItemElem* pItemElem = (FLItemElem*)pItemBase;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemBase->m_dwItemId );
	if( pItemProp == NULL )
	{
		return;
	}

	switch( pItemProp->dwItemKind2 )
	{
	case IK2_TELEPORTMAP:
		{
			switch( pItemProp->dwItemKind3 )
			{
			case IK3_TELEPORTMAP_USER:
				{
					FLTeleportMapUserPoints* pUserPoint = Find( pUser->m_idPlayer );
					if( pUserPoint == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s" ), pUser->GetName() );
						return;
					}

					const T_TELEPORTMAP_POINT* pPoint = pUserPoint->Find( pMsg->dwUserPointID );
					if( pPoint != NULL )
					{
						if( pUserPoint->ChangeName( pMsg->dwUserPointID, pMsg->szPointName ) == true )
						{
							FLPacketTeleportMapChgPointNameNoti kToDB;
							kToDB.dwPlayerID = pUser->m_idPlayer;
							kToDB.tUserPoint = *pPoint;
							g_dpDBClient.SendPacket( &kToDB );

							FLSnapshotTeleportMapChgPointNameAck kToClient;
							kToClient.tUserPoint = *pPoint;
							pUser->AddPacket( &kToClient );
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점의 이름을 변경할 수 없습니다. User: %s, PointID: %d" ), pUser->GetName(), pMsg->dwUserPointID );
						}
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "유효한 텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s, PointID: %d" ), pUser->GetName(), pMsg->dwUserPointID );
					}
				}
				break;

			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 유형의 텔레포트 지도입니다. Item: %s, IK3: %d" ), pItemProp->szName, pItemProp->dwItemKind3 );
					return;
				}
				break;
			}
		}
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지도가 아닙니다. Item: %s, IK2: %d" ), pItemProp->szName, pItemProp->dwItemKind2 );
			return;
		}
		break;
	}
}

void	FLTeleportMapUserPointManager::HandleTeleportMapDelPoint( FLWSUser* pUser, const FLPacketTeleportMapDelPointReq* pMsg )
{
	if( g_pItemUsing->CheckUseItem( pUser, pMsg->dwItemObjID ) == FALSE )
	{
		return;
	}

	if( CanUseTeleportMapItem( pUser, pMsg->dwItemObjID ) == false )
	{
		return;
	}

	FLItemBase* pItemBase = pUser->GetItemId( pMsg->dwItemObjID ); 
	if( IsUsableItem( pItemBase ) == FALSE )
	{
		return;
	}

//	FLItemElem* pItemElem = (FLItemElem*)pItemBase;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemBase->m_dwItemId );
	if( pItemProp == NULL )
	{
		return;
	}

	switch( pItemProp->dwItemKind2 )
	{
	case IK2_TELEPORTMAP:
		{
			switch( pItemProp->dwItemKind3 )
			{
			case IK3_TELEPORTMAP_USER:
				{
					FLTeleportMapUserPoints* pUserPoint = Find( pUser->m_idPlayer );
					if( pUserPoint == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s" ), pUser->GetName() );
						return;
					}

					const T_TELEPORTMAP_POINT* pDelPoint = pUserPoint->Find( pMsg->dwUserPointID );
					if( pDelPoint != NULL )
					{
						T_TELEPORTMAP_POINT tUserPoint;
						tUserPoint = *pDelPoint;

						if( pUserPoint->Del( pMsg->dwUserPointID ) == true )
						{
							FLPacketTeleportMapDelPointNoti kToDB;
							kToDB.dwPlayerID = pUser->m_idPlayer;
							kToDB.tUserPoint = tUserPoint;
							g_dpDBClient.SendPacket( &kToDB );

							FLSnapshotTeleportMapDelPointAck kToClient;
							kToClient.tUserPoint = tUserPoint;
							pUser->AddPacket( &kToClient );
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 삭제 할 수 없습니다. User: %s, PointID: %d" ), pUser->GetName(), pMsg->dwUserPointID );
						}
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "유효한 텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s, PointID: %d" ), pUser->GetName(), pMsg->dwUserPointID );
					}
				}
				break;

			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 유형의 텔레포트 지도입니다. Item: %s, IK3: %d" ), pItemProp->szName, pItemProp->dwItemKind3 );
					return;
				}
				break;
			}
		}
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지도가 아닙니다. Item: %s, IK2: %d" ), pItemProp->szName, pItemProp->dwItemKind2 );
			return;
		}
		break;
	}
}

void	FLTeleportMapUserPointManager::HandleTeleportMapDelAllPoint( FLWSUser* pUser, const FLPacketTeleportMapDelAllPointReq* pMsg )
{
	if( g_pItemUsing->CheckUseItem( pUser, pMsg->dwItemObjID ) == FALSE )
	{
		return;
	}

	if( CanUseTeleportMapItem( pUser, pMsg->dwItemObjID ) == false )
	{
		return;
	}

	FLItemBase* pItemBase = pUser->GetItemId( pMsg->dwItemObjID ); 
	if( IsUsableItem( pItemBase ) == FALSE )
	{
		return;
	}

//	FLItemElem* pItemElem = (FLItemElem*)pItemBase;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemBase->m_dwItemId );
	if( pItemProp == NULL )
	{
		return;
	}

	switch( pItemProp->dwItemKind2 )
	{
	case IK2_TELEPORTMAP:
		{
			switch( pItemProp->dwItemKind3 )
			{
			case IK3_TELEPORTMAP_USER:
				{
					FLTeleportMapUserPoints* pUserPoint = Find( pUser->m_idPlayer );
					if( pUserPoint == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 지점을 찾을 수 없습니다. User: %s" ), pUser->GetName() );
						return;
					}

					std::vector< T_TELEPORTMAP_POINT > vecDelPoints;
					pUserPoint->GetAllUserPoint( &vecDelPoints );

					if( pUserPoint->DelAll() == true )
					{
						std::vector< T_TELEPORTMAP_POINT >::const_iterator pos = vecDelPoints.begin();
						std::vector< T_TELEPORTMAP_POINT >::const_iterator end = vecDelPoints.end();

						for( ; pos != end; ++pos )
						{
							FLPacketTeleportMapDelPointNoti kToDB;
							kToDB.dwPlayerID = pUser->m_idPlayer;
							kToDB.tUserPoint = *pos;
							g_dpDBClient.SendPacket( &kToDB );
						}

						FLSnapShotTeleportMapDelAllPointAck kToClient;
						pUser->AddPacket( &kToClient );
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 유저 등록 모든 지점을 삭제 할 수 없습니다. User: %s" ), pUser->GetName() );
					}
				}
				break;

			default:
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "알 수 없는 유형의 텔레포트 지도입니다. Item: %s, IK3: %d" ), pItemProp->szName, pItemProp->dwItemKind3 );
					return;
				}
				break;
			}
		}
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "텔레포트 지도가 아닙니다. Item: %s, IK2: %d" ), pItemProp->szName, pItemProp->dwItemKind2 );
			return;
		}
		break;
	}
}
