// EventLua.cpp: implementation of the CEventLua class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "EventLua.h"
#ifdef __WORLDSERVER
#include "../WorldServer/User.h"
#ifdef __EVENTLUA_SPAWN
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#endif // __EVENTLUA_SPAWN
#endif // __WORLDSERVER
#ifdef __DBSERVER
#include "lang.h"
#endif // __DBSERVER
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventLua::CEventLua()
{
	Clear();

	m_bRun = FALSE;
#ifdef __DBSERVER
	m_bTimeLimit = TRUE;
#endif // __DBSERVER
}

CEventLua::~CEventLua()
{
	Clear();
}

void	CEventLua::Clear()
{
#ifdef __WORLDSERVER
#ifdef __EVENTLUA_SPAWN
	m_mapSpawnList.clear();			// spawn 이벤트 진행시 생성될 아이템 및 몬스터 정보
	m_mapMonsterId.clear();			// spawn 이벤트 진행시 생성된 몬스터가 이벤트용 인지 검사하기 위해 필요
	m_vecSpawnRegion.clear();		// spawn 이벤트 진행시 아이템 및 몬스터의 생성위치(월드 및 좌표) 정보
	m_mapSpawnedMonster.clear();	// spawn 이벤트 진행시 실제로 생성된 몬스터들의 목록 저장(이벤트 종료시 삭제하기 위해 필요)
	m_vecEndSpawnEvent.clear();		// 종료된 이벤트 ID 목록( 남아있는 spawn 몬스터가 있으면 다 삭제한 후 ID를 제거한다
#endif // __EVENTLUA_SPAWN
#ifdef __EVENTLUA_KEEPCONNECT
	m_vecKeepConnectUser.clear();
	m_vecKeepConnectItem.clear();
#endif // __EVENTLUA_KEEPCONNECT
	m_vecTroupeExpUpData.clear();
#endif // __WORLDSERVER
}

#ifdef __DBSERVER
BOOL CEventLua::IsNoticeTime()
{
	if( !IsPossible() )
		return FALSE;

	BOOL bIsNoticeTime = FALSE;
	if( m_Lua.GetLuaFunction("IsNoticeTime") )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		bIsNoticeTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop(0);

	return bIsNoticeTime;
}

std::vector<std::string> CEventLua::GetNoticeMessage()
{
	std::vector<std::string> vecstrMessage;

	if( !IsPossible() )
		return vecstrMessage;

	if( m_Lua.GetLuaFunction( "GetNoticeMessage" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		m_Lua.PushNil();
		while( m_Lua.TableLoop( -2 ) )
		{
			std::string strMessage;
			strMessage = m_Lua.ToString( -1 );
			vecstrMessage.push_back( strMessage );

			m_Lua.Pop( 1 );
		}
	}
	m_Lua.Pop( 0 );

	return vecstrMessage;
}

BOOL CEventLua::CheckEventState()
{
	if( !IsPossible() )
		return FALSE;

	if( m_Lua.GetLuaFunction("GetEventState") )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return FALSE;
		}
		
		m_mapState.clear();
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			BYTE nId = (BYTE)m_Lua.GetFieldToNumber( -1, "nId" );
			BOOL bState = (BOOL)m_Lua.GetFieldToNumber( -1, "State" );
			m_mapState.insert(std::make_pair( nId, bState ) );
			m_Lua.Pop( 1 );
		}
		
		if( m_mapState.empty() )
		{
			m_Lua.Pop(0);
			return FALSE;
		}
	}
	m_Lua.Pop(0);
	
	return TRUE;
}
#endif // __DBSERVER

#if defined(__WORLDSERVER) || defined(__DBSERVER)
std::vector<BYTE> CEventLua::GetEventList( BOOL bProxy )
{
	std::vector<BYTE> vecList;

	if( !IsPossible() )
		return vecList;

#ifdef __WORLDSERVER
		if( bProxy )
			return m_proxy.GetEventList();
#else
	UNREFERENCED_PARAMETER( bProxy );
#endif // __WORLDSERVER

	if( m_Lua.GetLuaFunction( "GetEventList" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( lua_isnil( m_Lua.GetLuaState(), -1 ) )
		{
			m_Lua.Pop(0);
			return vecList;
		}
		
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			vecList.push_back( (BYTE)m_Lua.ToNumber(-1) );
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
	
	return vecList;
}

void CEventLua::LoadScript()
{
	m_bRun = FALSE;

	const TCHAR szFileName[]	= _T( "Event.lua" );

	if( m_Lua.RunScript( szFileName ) == 0 )
	{
		m_bRun = TRUE;
#ifdef __WORLDSERVER
		PrepareProxy();

#ifdef __EVENTLUA_SPAWN
		for( std::map<BYTE, VECSPAWNINFO>::iterator it=m_mapSpawnList.begin(); it!=m_mapSpawnList.end(); it++ )
			m_vecEndSpawnEvent.push_back( it->first );
		m_mapSpawnList.clear();
#endif // __EVENTLUA_SPAWN
#endif // __WORLDSERVER

#ifdef __DBSERVER
		if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
			m_bTimeLimit = m_Lua.GetGlobalBool( "bTimeLimit" );
#endif // __DBSERVER
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFileName );
	}
}
#endif // __WORLDSERVER, __DBSERVER

#ifdef __WORLDSERVER
void CEventLua::SetState( BYTE nId, BOOL bState )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "SetState" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.PushNumber( bState ? 1 : 0 );
		m_Lua.CallLuaFunction( 2, 1 );
		m_bRun = m_Lua.ToBool( -1 );
	}

	m_Lua.Pop(0);
#ifdef __EVENTLUA_SPAWN
	SetSpawnEvent( nId, bState );
#endif // __EVENTLUA_SPAWN
}

string CEventLua::GetDesc( BYTE nId )
{
	string strDesc;

	if( !IsPossible() )
		return strDesc;

	if( m_Lua.GetLuaFunction( "GetDesc" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.CallLuaFunction( 1, 1 );
		
		strDesc = m_Lua.ToString( -1 );
	}
	m_Lua.Pop( 0 );

	return strDesc;
}

std::map<DWORD, int> CEventLua::GetItem( DWORD dwLevel )
{
	std::map<DWORD, int> mapItem;

// 	if( _GetContentState( CT_DROP_ITEM_REFORM ) == CS_VER1 )
// 		return mapItem;

	if( !IsPossible() )
		return mapItem;

	DWORD dwTickCount	= GetTickCount();
	if( m_Lua.GetLuaFunction( "GetItem" ) )
	{
		m_Lua.PushNumber( dwTickCount );
		m_Lua.PushNumber( dwLevel );
		m_Lua.CallLuaFunction( 2, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return mapItem;
		}

		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			DWORD dwItem = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "ItemId" ) );
			int	nNum = (int)m_Lua.GetFieldToNumber( -1, "ItemNum" );
			mapItem.insert( make_pair( dwItem, nNum ) );
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);

	return mapItem;
}

float CEventLua::GetExpFactor( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetExpFactor();

	if( m_Lua.GetLuaFunction( "GetExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

float CEventLua::GetItemDropRate( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetItemDropRate();

	if( m_Lua.GetLuaFunction( "GetItemDropRate" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	
	return fFactor;
}

float CEventLua::GetPieceItemDropRate( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetPieceItemDropRate();

	if( m_Lua.GetLuaFunction( "GetPieceItemDropRate" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	
	return fFactor;
}

float CEventLua::GetGoldDropFactor( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetGoldDropFactor();

	if( m_Lua.GetLuaFunction( "GetGoldDropFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	
	return fFactor;
}

void CEventLua::GetAllEventList( FLWSUser* pUser )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "GetAllEventList" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}
		
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			int nId = (int)m_Lua.GetFieldToNumber( -1, "nId" );
			string strTitle = m_Lua.GetFieldToString( -1, "strTitle" );
			int nState = (int)m_Lua.GetFieldToNumber( -1, "nState" );
			
			CString strTemp;
			strTemp.Format("ID : %d,  Title : %s,  State : %d", nId, strTitle.c_str(), nState );
			pUser->AddText( strTemp );
			
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}
void CEventLua::GetEventInfo( FLWSUser* pUser, int nId )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "GetEventInfo" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.CallLuaFunction( 1, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}

		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			CString strTemp = m_Lua.ToString(-1);
			pUser->AddText( strTemp );

			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}

int CEventLua::GetAttackPower( BOOL bProxy )
{
	int nAttackPower = 0;
	if( !IsPossible() )
		return nAttackPower;

	if( bProxy )
		return m_proxy.GetAttackPower();

	if( m_Lua.GetLuaFunction( "GetAttackPower" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		nAttackPower = (int)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return nAttackPower;
}

int CEventLua::GetDefensePower( BOOL bProxy )
{
	int nDefensePower = 0;
	
	if( !IsPossible() )
		return nDefensePower;

	if( bProxy )
		return m_proxy.GetDefensePower();

	if( m_Lua.GetLuaFunction( "GetDefensePower" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		nDefensePower = (int)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return nDefensePower;
}

DWORD CEventLua::GetCouponEvent( BOOL bProxy )
{
	if( !IsPossible() )
		return 0;

	if( bProxy )
		return m_proxy.GetCouponEvent();

	DWORD dwCouponEvent	= 0;
	if( m_Lua.GetLuaFunction( "GetCouponEvent" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		dwCouponEvent	= (DWORD)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	return dwCouponEvent;
}

void CEventLua::SetCoupon( FLWSUser* pUser, DWORD dwTick )
{
	if( !IsPossible() )
		return;

	if( !IsValidObj( pUser ) )
		return;

	DWORD dwCouponEvent	= GetCouponEvent();
	
	if( dwCouponEvent )
	{
		if( dwTick > pUser->m_dwTickCoupon + dwCouponEvent )
		{
			pUser->m_nCoupon++;
			pUser->m_dwTickCoupon = dwTick;
			pUser->AddCoupon( pUser->m_nCoupon );
		}
	}
	else
		pUser->m_dwTickCoupon	= dwTick;
}

#ifdef __EVENTLUA_GIFT
void CEventLua::SetLevelUpGift( FLWSUser* pUser, int nLevel )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "GetLevelUpGift" ) )
	{
		m_Lua.PushNumber( nLevel );
		m_Lua.PushString( pUser->m_playAccount.lpszAccount );
		m_Lua.PushNumber( pUser->GetLegendChar() );
		m_Lua.PushNumber( pUser->GetJob() );
		m_Lua.CallLuaFunction( 4, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}

		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			DWORD dwItemId = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) );
			int	nItemNum = (int)m_Lua.GetFieldToNumber( -1, "nItemNum" );

			if( !g_xSpecManager->GetSpecItem( dwItemId ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "ItemProp is NULL. ItemId = \"%s\"" ), m_Lua.GetFieldToString( -1, "strItemId" ) );
				m_Lua.Pop( 1 );
				continue;
			}

			FLItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum	= nItemNum;
			BYTE byFlag = static_cast<BYTE>( m_Lua.GetFieldToNumber( -1, "byFlag" ) );
			if( byFlag != FLItemElem::normal )
			{
				byFlag = FLItemElem::binds;
			}
			itemElem.SetFlag( byFlag );
			itemElem.SetSerialNumber();
			
			LogItemInfo aLogItem;
			//aLogItem.Action = "E";
			//aLogItem.SendName = pUser->GetName();
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "E" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

			
			if( pUser->CreateItem( &itemElem ) )
			{	// 로그 남김
				//aLogItem.RecvName = "EVENT_LEVELUP_GIFT";
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "EVENT_LEVELUP_GIFT" );
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
			}
			else
			{	// 우편 지급
				g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, "", "" );
				//aLogItem.RecvName = "EVENT_LEVELUP_GIFT_POST";
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "EVENT_LEVELUP_GIFT_POST" );
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
			}

			pUser->AddDefinedText( TID_GAME_EVENT_LEVELUP_GIFT, "\"%s\"", itemElem.GetName() );
			
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}
#endif // __EVENTLUA_GIFT

#ifdef __EVENTLUA_CHEEREXP
float CEventLua::GetCheerExpFactor( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetCheerExpFactor();

	if( m_Lua.GetLuaFunction( "GetCheerExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}
#endif // __EVENTLUA_CHEEREXP

#ifdef __EVENTLUA_SPAWN
void CEventLua::SetSpawnEvent( BYTE nId, BOOL bState )
{
	if( m_Lua.GetLuaFunction( "GetSpawn" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.CallLuaFunction( 1, 2 );
		int nSize = static_cast<int>( m_Lua.ToNumber( -1 ) );	m_Lua.Pop( 1 );

		if( bState && nSize > 0 )
		{
			VECSPAWNINFO vecTemp;
			m_Lua.PushNil();
			while( m_Lua.TableLoop( -2 ) )
			{
				int nType = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nType" ) );
				DWORD dwId = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strId" ) );
				DWORD dwInterval = static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nDayNum" ) );

				//////////////////////////////////////////////////////////////////////////
				// mirchang_101012 타입에 따른 셋팅 추가
				BYTE byFlag = static_cast<BYTE>( m_Lua.GetFieldToNumber( -1, "byFlag" ) );
				if( nType == OT_ITEM )
				{
					if( byFlag != FLItemElem::normal )
					{
						byFlag = FLItemElem::binds;
					}
				}
				else if( nType == OT_MOVER )
				{
					if( byFlag != 0 )
					{
						byFlag = 1;
					}
				}
				//////////////////////////////////////////////////////////////////////////

				dwInterval = 86400000 / dwInterval;

				//////////////////////////////////////////////////////////////////////////
				// mirchang_101012 타입에 따른 셋팅 추가
				vecTemp.push_back( __SPAWNINFO( nType, dwId, dwInterval, byFlag ) );
				//////////////////////////////////////////////////////////////////////////

				if( nType == OT_MOVER )
					m_mapMonsterId.insert( make_pair( dwId, nId ) );
				
				m_Lua.Pop( 1 );
			}

			std::map<BYTE, VECSPAWNINFO>::iterator it = m_mapSpawnList.find( nId );
			if( it == m_mapSpawnList.end() )
				m_mapSpawnList.insert( make_pair( nId, vecTemp ) );
			else
				it->second.assign( vecTemp.begin(), vecTemp.end() );

			// 완료 리스트에서 재적용된 이벤트를 제거한다.
			for( std::vector<BYTE>::iterator itVec=m_vecEndSpawnEvent.begin(); itVec!=m_vecEndSpawnEvent.end(); )
			{
				if( (*itVec) == nId )
					itVec = m_vecEndSpawnEvent.erase( itVec );
				else
					itVec++;
			}
		}
		else
		{
			std::map<BYTE, VECSPAWNINFO>::iterator it = m_mapSpawnList.find( nId );
			if( it != m_mapSpawnList.end() )
			{
				m_vecEndSpawnEvent.push_back( it->first );	// 완료 리스트에 해당 이벤트 ID를 추가한다. 스폰된 몬스터들을 제거하기 위해 필요.
				m_mapSpawnList.erase( it );
			}
		}
	}
	
	m_Lua.Pop(0);
}

void CEventLua::EventSpawnProcess()
{
	if( m_vecSpawnRegion.empty() == true )
	{
		return;
	}
	//if( m_mapSpawnList.size() <= 0 )
	//	return;

	for( std::map<BYTE, VECSPAWNINFO>::iterator itMap=m_mapSpawnList.begin(); itMap!=m_mapSpawnList.end(); itMap++ )
	{
		VECSPAWNINFO* pVecSP = &itMap->second;
		for( VECSPAWNINFO::iterator itVec=pVecSP->begin(); itVec!=pVecSP->end(); itVec++ )
		{
			__SPAWNREGION sp = GetSpawnRegion();
				CWorld* pWorld	= g_WorldMng.GetWorld( sp.dwWorldId );
			if( !pWorld )
				continue;
			
			__SPAWNINFO* pSI = &(*itVec);
			if( !pSI->IsTimeOut() )
				continue;

			switch( pSI->nType )
			{
				case OT_ITEM:
				{
					PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( pSI->dwId );
					if( pProp )
					{
						D3DXVECTOR3 v = sp.GetPos();
						v.y = pWorld->GetLandHeight( v.x, v.z ) + 1.0f;
						CObj* pObj = CreateObj( D3DDEVICE, OT_ITEM, pProp->dwID );
						if( pObj )
						{
							FLItemElem* pItemElem	= new FLItemElem;
							pItemElem->m_dwItemId	= pProp->dwID;
							pItemElem->m_nItemNum	= 1;
							pItemElem->m_nHitPoint	= ( pProp->dwEndurance == -1 ) ? 0 : pProp->dwEndurance;
							pItemElem->SetSerialNumber();
							( (CItem*)pObj )->m_dwDropTime	= g_tmCurrent + MIN( 1440 );	// 24시간 유지
							( (CItem*)pObj )->m_pItemBase	= pItemElem;
							pObj->SetPos( v );

							//////////////////////////////////////////////////////////////////////////
							// mirchang_101012 타입에 따른 셋팅 추가
							pItemElem->SetFlag( pSI->byFlag );
							//////////////////////////////////////////////////////////////////////////

							if( pWorld->ADDOBJ( pObj, TRUE, nDefaultLayer ) == TRUE )
							{
								FLTRACE_LOG( PROGRAM_NAME, _T( "Lua Spawn: type = ITEM, index = %d, world = %d, x = %f, z = %f" )
									, pItemElem->m_dwItemId, pWorld->GetID(), pObj->GetPos().x, pObj->GetPos().z );
							}
							else
							{
								SAFE_DELETE( pObj );
							}
						}
					}
					break;
				}
				case OT_MOVER:
				{
					MoverProp* pMoverProp = prj.GetMoverPropEx( pSI->dwId );
					if( pMoverProp )
					{
						CMover* pMover	= static_cast<CMover*>( CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID ) );
						if( pMover )	
						{
							D3DXVECTOR3 v = sp.GetPos();
							v.y = pWorld->GetLandHeight( v.x, v.z ) + 1.0f;
							pMover->SetPos( v );
							pMover->InitMotion( MTI_STAND );
							pMover->UpdateLocalMatrix();
							pMover->AddItToGlobalId();

							//////////////////////////////////////////////////////////////////////////
							// mirchang_101012 타입에 따른 셋팅 추가
							pMover->m_bActiveAttack = pSI->byFlag == 0 ? FALSE : TRUE;
							//////////////////////////////////////////////////////////////////////////

							if( pWorld->ADDOBJ( pMover, FALSE, nDefaultLayer ) == TRUE )
							{
								m_mapSpawnedMonster.insert( make_pair( pMover->GetId(), itMap->first ) );
								FLTRACE_LOG( PROGRAM_NAME, _T( "Lua Spawn: type = MONSTER, index = %d, world = %d, x = %f, z = %f" )
									, pSI->dwId, pWorld->GetID(), pMover->GetPos().x, pMover->GetPos().z );
							}
							else
							{
								SAFE_DELETE( pMover );
							}
						}
					}
					break;
				}
			}	// switch
		}	// for vector
	}	// for map
	
	DeleteEndEventProcess();
}

void CEventLua::DeleteEndEventProcess()
{
	if( m_vecEndSpawnEvent.empty() == true )
	{
		return;
	}
	// 이벤트 완료 리스트에 있는 스폰된 몬스터들을 제거한다.(프로세스당 100개씩)
	for( std::vector<BYTE>::iterator itVec=m_vecEndSpawnEvent.begin(); itVec!=m_vecEndSpawnEvent.end(); )
	{
		int nDelCount = 0;
		for( std::map<OBJID, BYTE>::iterator itMSM=m_mapSpawnedMonster.begin(); itMSM!=m_mapSpawnedMonster.end(); )
		{
			if( itMSM->second == (*itVec) )
			{
				CMover* pMover = prj.GetMover( itMSM->first );
				if( IsValidObj( pMover ) )
				{
					pMover->Delete();
					FLTRACE_LOG( PROGRAM_NAME, _T( "ID:%d, %s(%d):%d, Remain:%d" ), (*itVec), pMover->GetName(), pMover->GetIndex(), pMover->GetId(), m_mapSpawnedMonster.size() - 1 );
				}
				m_mapSpawnedMonster.erase( itMSM++ );

				nDelCount++;
				if( nDelCount == static_cast< int >(100/m_vecEndSpawnEvent.size()) )	// 100개가 삭제되었으면 다음 프로세스에 삭제한다.
					break;
			}
			else
				itMSM++;
		}
		
		if( nDelCount == 0 )	// 더이상 삭제될것이 없으면 이벤트 완료 목록에서 해당 ID를 제거한다.
		{
			for( std::map<DWORD, BYTE>::iterator itMId=m_mapMonsterId.begin(); itMId!=m_mapMonsterId.end(); )
			{
				if( itMId->second == (*itVec) )
					m_mapMonsterId.erase( itMId++ );
				else
					itMId++;
			}
			FLTRACE_LOG( PROGRAM_NAME, _T( "m_vecEndSpawnEvent : %d" ), (*itVec) );
			itVec = m_vecEndSpawnEvent.erase( itVec );
		}
		else
			itVec++;
	}
}
#endif // __EVENTLUA_SPAWN

#ifdef __EVENTLUA_KEEPCONNECT
void CEventLua::SetKeepConnectEvent( FLWSUser* pUser, DWORD dwTick )
{
	if( !IsPossible() )
		return;

	if( !IsValidObj( pUser ) )
		return;

	DWORD dwKeepConnect	= GetKeepConnectTime();
	
	if( dwKeepConnect )
	{
		if( dwTick > pUser->m_dwTickKeepConnect + dwKeepConnect )
		{
			pUser->m_dwTickKeepConnect = dwTick;
			m_vecKeepConnectUser.push_back( pUser->m_idPlayer );
		}
	}
	else
		pUser->m_dwTickKeepConnect	= dwTick;
}

DWORD CEventLua::GetKeepConnectTime( BOOL bProxy )
{
	if( !IsPossible() )
		return 0;

	if( bProxy )
		return m_proxy.GetKeepConnectTime();

	DWORD dwKeepConnect	= 0;
	if( m_Lua.GetLuaFunction( "GetKeepConnectTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		dwKeepConnect	= (DWORD)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return dwKeepConnect;
}

void CEventLua::GetKeepConnectItem()
{
	if( !IsPossible() )
		return;
	
	if( m_Lua.GetLuaFunction( "GetKeepConnectItem" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}
		m_vecKeepConnectItem.clear();
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			T_KEEP_CONNECT_ITEM tKeepConnectItem = { 0 };

			tKeepConnectItem.dwItemId	= CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) );
			tKeepConnectItem.nItemNum	= (int)m_Lua.GetFieldToNumber( -1, "nItemNum" );
			tKeepConnectItem.byFlag		= (BYTE)m_Lua.GetFieldToNumber( -1, "byFlag" );
			if( tKeepConnectItem.byFlag != FLItemElem::normal )
			{
				tKeepConnectItem.byFlag = FLItemElem::binds;
			}
			m_vecKeepConnectItem.push_back( tKeepConnectItem );
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}

void CEventLua::KeepConnectEventProcess()
{
	if( !IsPossible() )
		return;

	int nVecSize = m_vecKeepConnectUser.size();
	if( !nVecSize )
		return;
	
	u_long idPlayer;
	int nCount = 0;

	for( int i = 0; i < nVecSize; ++i )
	{
		if( nCount >= 10 )
			break;
		
		idPlayer = m_vecKeepConnectUser.back();
		m_vecKeepConnectUser.pop_back();
		
		for( std::vector<T_KEEP_CONNECT_ITEM>::iterator it = m_vecKeepConnectItem.begin(); it != m_vecKeepConnectItem.end(); ++it )
		{
			T_KEEP_CONNECT_ITEM tKeepConnectItem = { 0 };
			tKeepConnectItem = *it;
			FLItemElem itemElem;
			itemElem.m_dwItemId = tKeepConnectItem.dwItemId;
			itemElem.m_nItemNum	= tKeepConnectItem.nItemNum;
			itemElem.SetFlag( tKeepConnectItem.byFlag );
			itemElem.SetSerialNumber();
			
			g_dpDBClient.SendQueryPostMail( idPlayer, 0, itemElem, 0, "", "" );
		}
		++nCount;
	}
}
#endif // __EVENTLUA_KEEPCONNECT

float CEventLua::GetWeatherEventExpFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetWeatherEventExpFactor();

	if( m_Lua.GetLuaFunction( "GetWeatherEventExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

string CEventLua::GetWeatherEventTitle()
{
	string strTitle("");

	if( !IsPossible() )
		return strTitle;

	if( m_Lua.GetLuaFunction( "GetWeatherEventTitle" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		if( m_Lua.IsNil( -1 ) )
			return strTitle;
		strTitle = m_Lua.ToString( -1 );
	}
	m_Lua.Pop( 0 );

	return strTitle;
}

float CEventLua::GetShopBuyFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetShopBuyFactor();

	if( m_Lua.GetLuaFunction( "GetShopBuyFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

float CEventLua::GetShopSellFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

	if( bProxy )
		return m_proxy.GetShopSellFactor();

	if( m_Lua.GetLuaFunction( "GetShopSellFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

void CEventLua::GetTroupeExpUpEvent()
{
	if( IsPossible() == FALSE )
	{
		return;
	}

	if( m_Lua.GetLuaFunction( "GetTroupeExpUpEvent" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}
		
		m_vecTroupeExpUpData.clear();
		
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			T_TROUPE_EXPUP_DATA kTroupeExpUpData;

			kTroupeExpUpData.dwTroupeMemberCount	= static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nTroupeMemberCount" ) );
			if( kTroupeExpUpData.dwTroupeMemberCount > MAX_PTMEMBER_SIZE )
			{
				kTroupeExpUpData.dwTroupeMemberCount = MAX_PTMEMBER_SIZE;
			}
			
			kTroupeExpUpData.fTroupeExpUpFactor		= static_cast<float>( m_Lua.GetFieldToNumber( -1, "fTroupeExpUpFactor" ) );
			if( kTroupeExpUpData.fTroupeExpUpFactor < 1.0f )
			{
				kTroupeExpUpData.fTroupeExpUpFactor = 1.0f;
			}
			
			m_vecTroupeExpUpData.push_back( kTroupeExpUpData );
			
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}

float CEventLua::GetTroupeExpUpFactor( DWORD dwTroupeMemberCount )
{
	float fFactor = 1.0f;

	if( IsPossible() == FALSE )
	{
		return fFactor;
	}

	if( m_vecTroupeExpUpData.empty() == true )
	{
		return fFactor;
	}

	for( std::vector<T_TROUPE_EXPUP_DATA>::const_iterator it = m_vecTroupeExpUpData.begin(); it != m_vecTroupeExpUpData.end(); ++it )
	{
		if( dwTroupeMemberCount >= it->dwTroupeMemberCount )
		{
			fFactor		= it->fTroupeExpUpFactor;
		}
	}

	if( fFactor < 1.0f )
	{
		fFactor = 1.0f;
	}

	return fFactor;
}

//////////////////////////////////////////////////////////////////////////
// void	CEventLua::GetDailyKeepConnectEvent()
// {
// 	if( IsPossible() == FALSE )
// 	{
// 		return;
// 	}
// 
// 	m_kFlyffPiece.Init();
// 
// 	if( m_Lua.GetLuaFunction( "GetDailyKeepConnectEvent" ) )
// 	{
// 		m_Lua.CallLuaFunction( 0, 1 );
// 		if( m_Lua.IsNil( -1 ) )
// 		{
// 			m_Lua.Pop(0);
// 			return;
// 		}
// 
// 		m_Lua.PushNil();
// 		while( m_Lua.TableLoop(-2) )
// 		{
// 			const int nDay			= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nDay" ) );
// 			const DWORD dwKeepTime	= static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "dwKeepTime" ) );
// 			const DWORD dwItemID	= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemID" ) ) );
// 			const int nMinQuantity	= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nMinQuantity" ) );
// 			const int nMaxQuantity	= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nMaxQuantity" ) );
// 			const BYTE byFlag		= static_cast<BYTE>( m_Lua.GetFieldToNumber( -1, "byFlag" ) );
// 
// 			T_DAILY_CONNECTION_DATA kConnectionData( nDay, dwKeepTime );
// 			T_DAILY_ITEM_DATA kItemData( dwItemID, nMinQuantity, nMaxQuantity, byFlag );
// 
// 			m_kFlyffPiece.InsertDailyConnectionData( kConnectionData );
// 			m_kFlyffPiece.InsertDailyItemData( nDay, kItemData );
// 
// 			m_Lua.Pop(1);
// 		}
// 	}
// 	m_Lua.Pop(0);
// 
// 	const bool bActivate = m_kFlyffPiece.CheckDailySettingData();
// 
// 	m_kFlyffPiece.SetActivate( bActivate );
// }
// 
// void	CEventLua::OnDailyKeepConnection( FLWSUser* pUser, const DWORD dwTick ) const
// {
// 	m_kFlyffPiece.OnDailyKeepConnection( pUser, dwTick );
// }
// 
// bool	CEventLua::StartBurningMode( const T_BURNING_MODE & kMode )
// {
// 	return m_kFlyffPiece.SetBurningMode( kMode );
// }
// 
// bool	CEventLua::EndBurningMode()
// {
// 	return m_kFlyffPiece.InitializeBurningMode();
// }
// 
// void	CEventLua::ProcessCheckBurningMode( const time_t time )
// {
// 	const DWORD dwEndMode = m_kFlyffPiece.ProcessCheckBurningMode( time );
// 
// 	if( dwEndMode != E_BURNING_MODE_NONE && g_uIdofMulti == 1 )
// 	{
// // 		FLPacketBroadCastMessageNoti toCoreSrvr;
// // 		toCoreSrvr.byBroadCastType	= BROADCAST_TYPE_PUTSTRING;
// // 		FLStrcpy( toCoreSrvr.szMessage, _countof( toCoreSrvr.szMessage ), prj.GetText( TID_MMI_FLYFFPIECE_TEXT07 ) );
// // 
// // 		g_DPCoreClient.SendPacket( &toCoreSrvr );
// 
// 		g_DPCoreClient.SendSystem( prj.GetText( TID_MMI_FLYFFPIECE_TEXT07 ) );
// 	}
// }
//////////////////////////////////////////////////////////////////////////

#endif // __WORLDSERVER

#ifdef __WORLDSERVER
CEventLuaProxy::CEventLuaProxy()
:
m_fExpFactor( 1.0f ),
m_fItemDropRate( 1.0f ),
m_fPieceItemDropRate( 1.0f ),
m_fGoldDropFactor( 1.0f ),
m_nAttackPower( 0 ),
m_fDefensePower( 0 ),
m_dwCouponEvent( 0 )
#ifdef __EVENTLUA_CHEEREXP
,m_fCheerExpFactor( 1.0f )
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
, m_dwKeepConnect( 0 )
#endif // __EVENTLUA_KEEPCONNECT
, m_fWeatherEventExpFactor( 1.0f )
, m_fShopBuyFactor( 1.0f )
, m_fShopSellFactor( 1.0f )
{
}

CEventLuaProxy::~CEventLuaProxy()
{
}

void CEventLuaProxy::Initialize( CEventLua* pEventLua )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "CEventLuaProxy::Initialize" ) );

	SetEventList( pEventLua->GetEventList( FALSE ) );
	SetExpFactor( pEventLua->GetExpFactor( FALSE ) );
	SetItemDropRate( pEventLua->GetItemDropRate( FALSE ) );
	SetPieceItemDropRate( pEventLua->GetPieceItemDropRate( FALSE ) );
	SetGoldDropFactor( pEventLua->GetGoldDropFactor( FALSE ) );
	SetAttackPower( pEventLua->GetAttackPower( FALSE ) );
	SetDefensePower( pEventLua->GetDefensePower( FALSE ) );
	SetCouponEvent( pEventLua->GetCouponEvent( FALSE ) );
#ifdef __EVENTLUA_CHEEREXP
	SetCheerExpFactor( pEventLua->GetCheerExpFactor( FALSE ) );
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
	SetKeepConnectTime( pEventLua->GetKeepConnectTime( FALSE ) );
	pEventLua->GetKeepConnectItem();
#endif // __EVENTLUA_KEEPCONNECT

	SetWeatherEventExpFactor( pEventLua->GetWeatherEventExpFactor( FALSE ) );

	SetShopBuyFactor( pEventLua->GetShopBuyFactor( FALSE ) );
	SetShopSellFactor( pEventLua->GetShopSellFactor( FALSE ) );

	pEventLua->GetTroupeExpUpEvent();

	//////////////////////////////////////////////////////////////////////////
//	pEventLua->GetDailyKeepConnectEvent();
	//////////////////////////////////////////////////////////////////////////
}
#endif // __WORLDSERVER

