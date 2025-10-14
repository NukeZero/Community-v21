#include "stdafx.h"
#include "User.h"
#include <WorldMng.h>
#include "..\_Common\Ship.h"

#include "..\_aiinterface\AIPet.h"

#include <playerdata.h>

#include <collecting.h>

#include <SecretRoom.h>

#include <Tax.h>

#include "slord.h"
#include "lordskillexecutable.h"

#include "ItemUpgrade.h"


#include <guild.h>
extern	CGuildMng	g_GuildMng;
#include <guildwar.h>
extern	CGuildWarMng	g_GuildWarMng;

#include <Chatting.h>
extern	CChattingMng	g_ChattingMng;

#include <Party.h>
extern	CPartyMng			g_PartyMng;
extern CGuildCombat g_GuildCombatMng;

#include "UserMacro.h"
#include <post.h>

#include "couplehelper.h"

#include <spevent.h>
#include "../_Common/FLGameTimer.h"

#include "FLCooperativeContributions.h"
#include "FLCC_Condition.h"
#include "FLCC_Reward.h"

#include "../_Common/InstanceDungeonParty.h"

#include "FLItemUsing.h"
#include "FLTeleportMapUserPointManager.h"
#include "FLChargeZoneTicket.h"

#include "FLEventArenaGlobal.h"
#include "FLBarunaUpgradeNpc.h"

#include "../worldserver/LinkMap.h"

#include "FLMadrigalGift.h"
#include "FLFlyffPieceDailyPayment.h"
#include "../_CommonDefine/Packet/FLPacketItemOption.h"

#include "../_Common/SeasonEffect.h"
#include <FLSkillSystem.h>

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
extern	CWorldMng	g_WorldMng;
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifdef __INTERNALSERVER
	MEMPOOLER_IMPLEMENT( FLWSUser, 5 );
#else	// __INTERNALSERVER
	MEMPOOLER_IMPLEMENT( FLWSUser, 512 );
#endif	// __INTERNALSERVER


FLWSUser::FLWSUser()
{
#ifdef __AGGRO16
	m_bDisplayAggro		= FALSE;
#endif // __AGGRO16
	Init();
}

FLWSUser::FLWSUser( DPID dpidCache, DPID dpidUser )
{
	Init( dpidCache, dpidUser );
}

FLWSUser::~FLWSUser()
{
	//RemoveItFromView2();	// npc
}

BOOL FLWSUser::IsValid()
{
	if( IsDelete() )
		return FALSE;
	else
		return m_bValid;
}

void FLWSUser::Init( DPID dpidCache, DPID dpidUser )
{
#ifdef __AGGRO16
	m_bDisplayAggro		= FALSE;
#endif // __AGGRO16


	CMover::m_bPlayer	= TRUE;

	
	//////////////////////////////////////////////////////////////////////////
	m_bCheckTransMailBox = FALSE;
	m_nCountFromClient = 0;
	m_dwTickFromClient = 0;
	//////////////////////////////////////////////////////////////////////////

//	::memset( (PT_HACK_CHECK_DATA)&m_tHackCheckData, 0, sizeof(m_tHackCheckData) );


	m_dwSerial = 0;	
	m_bValid = FALSE;
	m_Snapshot.dpidCache = dpidCache;
	m_Snapshot.dpidUser  = dpidUser;

	m_dwAuthKey		= 0;
	m_idSetTarget	= NULL_ID;
	m_nOverHeal		= 0;
	m_szPartyName[0]	= '\0';

	memset( m_szBankPass, 0, sizeof(m_szBankPass) );
	memset( &m_playAccount, 0, sizeof(m_playAccount) );


//	m_idTargetCollect = NULL_ID;	
//	m_nCollect = 0;					
//	m_tmCollect = 0;
	m_idChatting = 0;
	memset( m_pWall, 0, sizeof(m_pWall) );
	m_dwLeavePenatyTime = 0;
	m_dwDestroyTime = 0;
	m_tmEscape = 0;
	m_idSnoop		= 0;

	m_dwReturnWorldID = 0;
	m_vReturnPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_lpszVillage = NULL;

	m_bInstantBank	= FALSE;

	DWORD dwTick = ::timeGetTime();
	m_dwTickSFS = dwTick + 1000;
	m_dwTickNotify = dwTick + 200;
	m_dwTimeout4Save = SEC_SAVEPLAYER;

	m_nEventFlag = 0;
	m_dwEventTime	= 0;
	m_dwEventElapsed	= 0;

	m_tmDuelRequest		= 0;

	m_bAllAction = TRUE;

	m_tickScript	= GetTickCount();
	
	m_nKawibawiboWin	= 0;
	m_nKawibawiboState  = 3;
	for ( int i=0; i<6; ++i )
		m_nBetFiveSystem[i] = 0;

	m_nExpLog = 0;
	m_nAngelExpLog = 0;

	m_nCollecting	= 0;

	m_nCoupon = 0;
	m_dwTickCoupon = GetTickCount();

	m_idElection	= NULL_ID;
	m_bQuerying		= FALSE;

	m_szInput[0]	= '\0';

	m_idProposer	= 0;
	m_cbProcessCouple	= 0;

#ifdef __EVENTLUA_KEEPCONNECT
	m_dwTickKeepConnect = GetTickCount();
#endif // __EVENTLUA_KEEPCONNECT

	m_nRestPoint = 0;
	m_nPrevRestPoint = 0;
	m_tRestPointTick = time_null();

#ifdef __PERIN_BUY_BUG
	m_dwLastTryBuyItem = NULL_ID;
	m_dwLastBuyItemTick = GetTickCount();
#endif // __PERIN_BUY_BUG

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100723 give coupon item event

	m_dwLastInputCouponNumberTick = GetTickCount();

	// mirchang_100723 give coupon item event
	//////////////////////////////////////////////////////////////////////////
}

BOOL FLWSUser::TryRemoveNotUsingItem( const DWORD dwItemID, const int nItemCount )
{
	int nCount				= 0;
	for( DWORD i = 0; i < m_Inventory.GetMax(); ++i )
	{
		FLItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem == NULL )
		{
			continue;
		}
		if( IsUsing( pItemElem ) == TRUE || m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
			continue;

		if( pItemElem->m_dwItemId == dwItemID )
			nCount				+= pItemElem->m_nItemNum;
	}
	
	if( nCount < nItemCount )
		return FALSE;

	int nRemain		= nItemCount;
	for( DWORD i = 0; i < m_Inventory.GetMax(); ++i )
	{
		FLItemElem* pItemElem = m_Inventory.GetAt( i );
		if( pItemElem == NULL )
		{
			continue;
		}

		if( IsUsing( pItemElem ) == TRUE || m_Inventory.IsEquip( pItemElem->m_dwObjId ) == TRUE )
			continue;

		if( pItemElem->m_dwItemId == dwItemID )
		{
			if( nRemain > pItemElem->m_nItemNum )
			{
				nRemain			-= pItemElem->m_nItemNum;
				UpdateItem( pItemElem->m_dwObjId, UI_NUM, 0 );	// remove
			}
			else
			{
				UpdateItem( pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum - nRemain );
				nRemain				= 0;
				//nCount			= 0;
				break;
			}
		}
	}

	FLASSERT( nRemain == 0 );
	
	return TRUE;
}

void FLWSUser::LevelUpSetting( void )
{
	g_xWSUserManager->AddSetLevel( this, (short)GetLevel() );
	AddSetGrowthLearningPoint( m_Stat.GetRemainStatPoint() );
	g_dpDBClient.SendLogLevelUp( this, 1 );	// 레벨업 로그
	g_dpDBClient.SendUpdatePlayerData( this );
}

void FLWSUser::ExpUpSetting( void )
{
	// 레벨 5이상 로그_레벨업 테이블에 로그를 남긴다
	// 20% 단위로 로그를 남김
	if( GetLevel() > 5 ) // 레벨 5이상
	{
		int nNextExpLog = (int)(m_nExpLog/20 + 1) * 20;	
		int nExpPercent = (int)( GetExp1() * 100 / GetMaxExp1() );
		if( nExpPercent >= nNextExpLog )
		{
			m_nExpLog = nExpPercent;
			g_dpDBClient.SendLogLevelUp( this, 5 );
		}
	}
}

//void FLWSUser::RemoveItFromView2( BOOL bRemoveall )
//{
//	CCtrl* pCtrl;
//
//	std::map<DWORD, CCtrl*>::iterator it = m_2npc.begin();
//	for( ; it != m_2npc.end(); ++it )
//	{
//		pCtrl = it->second;
//		pCtrl->PCRemoveKey( GetId() );
//	}
//	if( bRemoveall )
//		m_2npc.clear();
//}



void FLWSUser::Open( DWORD dwWorldId )
{
	AddItToGlobalId();
	m_Snapshot.SetSnapshot( GetId(),  PACKETTYPE_JOIN );
	AddSeasonInfo();
	AddSeasonEffect();
	AddWorldReadInfo( dwWorldId, GetPos() );
	AddAddObj( (CCtrl*)this );		//@@@@ 정보를 주는 개념
	AddGameTimer( g_GameTimer.GetCurrentTime() );
	AddTaskBar();
	AddPlayerData();
	AddFriendGameJoin();
	AddPartyName();
	ADDGameJoin();
	AddAllGuilds();
	AddMyGuild();
	AddMyGuildWar();
	AddFlyffEvent();
	AddLord();
	AddCouple();
}

// 유저의 주기적 처리 
// 200ms   Notify
// 10000ms SubSMMode
// 3Min    CheckFiniteItem, SavePlayer
void FLWSUser::Process()
{
	if( IsValid() == FALSE )
		return;

	DWORD dwTick	= g_tmCurrent;

	if( IsMode( MODE_OUTOF_PARTYQUESTRGN ) )
	{
		SetNotMode( MODE_OUTOF_PARTYQUESTRGN );
		D3DXVECTOR3 vPos	= D3DXVECTOR3( 6968.0f, 0, 3328.8f );
		REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, vPos, REPLACE_NORMAL, nDefaultLayer );
		return;
	}

	if( m_dwDestroyTime && dwTick > m_dwDestroyTime )
	{
		g_xWSUserManager->DestroyPlayer( this );
		return;
	}

	if( IsStateMode( STATE_BASEMOTION_MODE ) )
	{
		if( (int)( dwTick ) >= m_nReadyTime )
		{	
			m_nReadyTime	= 0;
			if( m_bItemFind )
			{
				FLItemElem* pItemElem = m_Inventory.GetAtId( m_dwUseItemId );
				if( pItemElem )
				{
					g_pItemUsing->DoUseItem( this, MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId ), pItemElem->m_dwObjId );
				}
				else
				{
					AddDefinedText(TID_PK_BLINK_LIMIT, "" );	// 시전중에는 사용할수 없습니다
					m_nReadyTime = 0;
					m_dwUseItemId = 0;
					m_bItemFind = FALSE;
					SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );	// 셑팅 빼기
				}
			}
			else
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwUseItemId );
				if( pItemProp )
				{
					DoUseItemVirtual( m_dwUseItemId, FALSE );
				}
			}
		}
	}

	if( ( m_nCount & 15 ) == 0 )
	{
		if( GetWorld() != NULL && g_xSpecManager->IsChargeZoneWorld( GetWorld()->GetID() ) != false )
		{
			if( g_pChargeZoneTicket->CanStayChargeZoneWorld( this ) == false )
			{
				REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984F, 100.0F, 3336.884F ), REPLACE_FORCE, nDefaultLayer );
			}
		}

		ProcessCouple();
		ProcessCampus();
		CCampusHelper::GetInstance()->RecoveryCampusPoint( this, dwTick );
	}

	CheckTickCheer();

	CMover::Process();

	if( IsLive() )
	{
		if( m_dwFlag & MVRF_MEDITATION_SIT )
		{
			if( m_pActMover->IsStateFlag( OBJSTAF_SIT ) ) 
			{
				m_dwFlag &= (~MVRF_MEDITATION_SIT);
			} 
			else
			{
				if( SendActMsg( OBJMSG_SITDOWN ) == 1 )
				{
					ClearDest();
					g_xWSUserManager->AddMotion( this, OBJMSG_SITDOWN );
					m_dwFlag &= (~MVRF_MEDITATION_SIT);
				}
			}
		}


		// 비행연료 자동 소모
		if( m_nFuel > 0 )
		{
			if( (m_nCount & 15) == 0 )
			{
				if( m_pActMover->IsFly() && m_pActMover->IsStateFlag( OBJSTAF_ACC ) )
				{
					PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwRideItemIdx );
					if( pItemProp )
						AddSetFuel( m_nFuel, m_tmAccFuel );	// 클라에 변화된 양을 전송
				}
			}
		}

		if( m_nOverHeal > 0)
			--m_nOverHeal;
	}

	if( dwTick > m_dwTickNotify )				// 200ms마다 
	{
		m_dwTickNotify = dwTick + 200;
		Notify();
	}

	if( dwTick > m_dwTickSFS )					// 1초마다 
	{
//		m_dwTickSFS = dwTick + 1000;
		m_dwTickSFS	+= 1000;	// 초당 오차가 평균 33 ms	- 康
		CheckFiniteItem();

		prj.m_EventLua.SetCoupon( this, dwTick );

#ifdef __EVENTLUA_KEEPCONNECT
		prj.m_EventLua.SetKeepConnectEvent( this, dwTick );
#endif // __EVENTLUA_KEEPCONNECT

		g_pMadrigalGift->OnKeepConnection( this, dwTick );

		using namespace nsFlyffPieceDailyPayment;
		g_pFlyffPieceDailyPayment->OnDailyKeepConnection( this, dwTick );

		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			SubSMMode();
			if( --m_dwTimeout4Save == 0 )		// 180초마다 
			{
				CheckHonorTime();

#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( this, pWorld->GetID(), GetPos(), GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( this, pWorld->GetID(), GetPos() );
#endif	// __LAYER_1015
				AddEventLuaDesc();
			}

			if( GuildHouseMng->IsGuildHouse( pWorld->GetID() ) && static_cast< u_long >( GetLayer() ) == m_idGuild )
			{
				if( m_tRestPointTick <= time_null() )	
				{
					SetIncRestPoint( REST_POINT_INC );
					SetRestPointTick();
				}
			}
			
			if( m_nRestPoint != m_nPrevRestPoint )
			{
				AddRestPoint();
				m_nPrevRestPoint = m_nRestPoint;
			}
		}
		if( m_nAccountPlayTime > -1 )
			m_nAccountPlayTime += SEC( 1 );

		m_playTaskBar.Process( this );
	}


	if( IsPKPink() && GetTickCount() > GetPKPink() )
	{
		SetPKPink( 0 );
		g_xWSUserManager->AddPKPink( this, 0 );
	}

	if( HasActivatedEatPet() )
	{
		CMover* pEatPet = prj.GetMover( GetEatPetId() );
		if( IsValidObj( pEatPet ) )
		{
			if( !IsValidArea( pEatPet, 32 ) )
			{
				CAIPet* pAIPet	= static_cast<CAIPet*>( pEatPet->m_pAIInterface );
				if( pAIPet )
				{
					FLItemElem* pItemElem = (FLItemElem*)GetItemId( pAIPet->GetPetItemId() );
					InactivateEatPet();
					ActivateEatPet( pItemElem );
// 					if( IsUsableItem( pItemElem ) )
// 					{
// 						g_pItemUsing->DoUseItem( this, MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId ), pItemElem->m_dwObjId );
// 					}
				}
				else
					InactivateEatPet();
			}
		}
	}

#ifdef __ENCHANT_BARUNA16						//User에 대한 컨텐츠를 SetDestPos에 코드를 넣으면 안된다.
	const OBJID	BarunaNPCOBJIDSaved		= GetBarunaNPCSummoner().GetNPCOBJID();
	if( GetBarunaNPCSummoner().TryInactivateNPCByRange( *this, 150.0f ) == FLNPCSummoner::E_OPERATION::INACTIVE )
		AddReleaseBarunaNPC( BarunaNPCOBJIDSaved );
#endif //__ENCHANT_BARUNA16

}

//void FLWSUser::Notify( void )
int	FLWSUser::Notify( void )
{
	if( IsValid() == FALSE )
		return	0;

	if( m_dwDestroyTime )		// 종료예약 유저는 Notify하지 않는다.
		return 0;

	static u_int uOffset	= sizeof(DPID)+sizeof(DWORD)+sizeof(OBJID)+sizeof(short);
	LPBYTE lpBuf;
	u_long nBufSize;

	if( m_Snapshot.cb > 0 )
	{
		lpBuf	= m_Snapshot.ar.GetBuffer( &nBufSize );
		*(UNALIGNED WORD*)( lpBuf + sizeof(DPID) + sizeof(DWORD) + sizeof(OBJID) )	= m_Snapshot.cb;
		g_DPSrvr.Send( (LPVOID)lpBuf, nBufSize, m_Snapshot.dpidCache );
		*(UNALIGNED DWORD*)( lpBuf + sizeof(DPID) )		= PACKETTYPE_SNAPSHOT;
		m_Snapshot.cb	= 0;
		m_Snapshot.ar.ReelIn( uOffset );
		return nBufSize;
	}
	return 0;
}

//int FLWSUser::FlushSnapshot()
//{
//	static u_int uOffset	= sizeof(DPID)+sizeof(DWORD)+sizeof(OBJID)+sizeof(short);
//	LPBYTE lpBuf;
//	u_long nBufSize;
//
//	if( m_Snapshot.cb > 0 )
//	{
//		lpBuf	= m_Snapshot.ar.GetBuffer( &nBufSize );
//		*(UNALIGNED WORD*)( lpBuf + sizeof(DPID) + sizeof(DWORD) + sizeof(OBJID) )	= m_Snapshot.cb;
//		g_DPSrvr.Send( (LPVOID)lpBuf, nBufSize, m_Snapshot.dpidCache );
//		*(UNALIGNED DWORD*)( lpBuf + sizeof(DPID) )		= PACKETTYPE_SNAPSHOT;
//		m_Snapshot.cb	= 0;
//		m_Snapshot.ar.ReelIn( uOffset );
//		return nBufSize;
//	}
//	return 0;
//}

void FLWSUser::SetAllAction()
{
	// 캐릭 이름이 첫글자가 숫자로 사용하면 못움직이게 하고 이름을 바꾸게 하자
	CString strName = GetName();
	LPCTSTR lpszString = strName;
	char c = strName[ 0 ];
	BOOL bCharacter = FALSE;
	m_bAllAction = TRUE;
	if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszString ) ) // "명칭에 첫글자를 숫자로 사용할 수 없습니다."
	{
		m_bAllAction = FALSE;
		bCharacter = TRUE;
	}
	// 길드 이름이 첫글자가 숫자로 사용하면 못움직이게 하고 이름을 바꾸게 하자
	else if( m_idGuild != 0 )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
		if( pGuild && pGuild->IsMaster( m_idPlayer ) )
		{
			CString strGuildName = pGuild->m_szGuild;
			LPCTSTR lpszGuildString = strGuildName;
#ifdef _DEBUG
			if( !strGuildName.IsEmpty() )
#endif // _DEBUG
			{
				c = strGuildName[ 0 ];
				if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszGuildString ) ) // "명칭에 첫글자를 숫자로 사용할 수 없습니다."
					m_bAllAction = FALSE;
			}
		}
	}
	AddAllAction( bCharacter );
}

void FLWSUser::AddBlock( LPBYTE lpBlock, u_long uBlockSize )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar.Write( lpBlock, uBlockSize );
}

void FLWSUser::AddHdr( OBJID objid, int nPacketType )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << nPacketType;
}

//this: i, pCtrl: other
void FLWSUser::AddAddObj( CCtrl * pCtrl )
{
	if( IsDelete() )	return;

	//	mulcom	BEGIN100430
	if( pCtrl == NULL )
	{
		return;
	}
	//	mulcom	END100430

	m_Snapshot.cb++;
	m_Snapshot.ar << pCtrl->GetId() << SNAPSHOTTYPE_ADD_OBJ << (BYTE)pCtrl->GetType() << pCtrl->GetIndex();
	pCtrl->Serialize( m_Snapshot.ar, pCtrl == this ? METHOD_NONE : METHOD_EXCLUDE_ITEM );

	//////////////////////////////////////////////////////////////////////////
	if( pCtrl->GetType() == OT_MOVER )
	{
		CMover* pMover = static_cast< CMover* >( pCtrl );
		if( IsValidObj( pMover ) )
		{
			m_Snapshot.ar << pMover->GetDestPos();
			if( pMover->IsDisguise() )
			{
				m_Snapshot.cb++;
				m_Snapshot.ar << pMover->GetId() << SNAPSHOTTYPE_DISGUISE << pMover->m_dwDisguiseID;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

void FLWSUser::AddText( LPCSTR lpsz )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TEXT;
	m_Snapshot.ar << TEXT_GENERAL;
	m_Snapshot.ar.WriteString( lpsz );
}


void FLWSUser::AddDiagText( LPCSTR lpsz )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TEXT;
	m_Snapshot.ar << TEXT_DIAG;
	m_Snapshot.ar.WriteString( lpsz );
}

void FLWSUser::AddAllAction( BOOL bCharacter )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ALLACTION;
	m_Snapshot.ar << m_bAllAction;
	m_Snapshot.ar << bCharacter;
}


void FLWSUser::AddReplace( DWORD dwWorldID, D3DXVECTOR3 & vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_REPLACE;
	m_Snapshot.ar << dwWorldID;
	m_Snapshot.ar << vPos;
}

void FLWSUser::AddCreateItem( FLItemBase* pItemBase, DWORD dwItemObjID, int nQuantity )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CREATEITEM;
	pItemBase->Serialize( m_Snapshot.ar );
	m_Snapshot.ar << dwItemObjID;
	m_Snapshot.ar << nQuantity;
}

void FLWSUser::AddMoveItem( DWORD dwSrcObjIndex, DWORD dwDestObjIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOVEITEM;
	m_Snapshot.ar << dwSrcObjIndex << dwDestObjIndex;
}
#ifdef INVENTORY_ITEM_ALIGN
void FLWSUser::AddAlignItem( DWORD dwIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ALIGNITEM;
	m_Snapshot.ar << dwIndex;
}
#endif	// INVENTORY_ITEM_ALIGN
void FLWSUser::AddTrade( FLWSUser* pTrader, u_long uidPlayer )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( pTrader );
	m_Snapshot.ar << SNAPSHOTTYPE_TRADE;
	m_Snapshot.ar << uidPlayer;

	pTrader->m_Inventory.Serialize( m_Snapshot.ar );
}

void FLWSUser::AddComfirmTrade( OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMTRADE;
}

void FLWSUser::AddComfirmTradeCancel( OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMTRADECANCEL;
}

void FLWSUser::AddExpBoxCoolTime( OBJID objid, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXPBOXCOLLTIME;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << dwTime;
}


void FLWSUser::AddCommonSkill( DWORD dwSkill, DWORD dwLevel )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COMMONSKILL;
	m_Snapshot.ar << dwSkill << dwLevel;
}

void FLWSUser::AddTradePut( OBJID objid, BYTE byNth, BYTE byType, DWORD dwItemObjID, int nItemNum )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUT;
	m_Snapshot.ar << byNth << byType << dwItemObjID << nItemNum;
}

void FLWSUser::AddTradePutError( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUTERROR;
}

void FLWSUser::AddTradePull( OBJID objid, BYTE byNth )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPULL;
	m_Snapshot.ar << byNth;
}

void FLWSUser::AddTradePutGold( OBJID objid, DWORD dwGold )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUTGOLD;
	m_Snapshot.ar << dwGold;
}
//raiders.2006.11.28
/*
void FLWSUser::AddTradeClearGold( OBJID objid )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADECLEARGOLD;
}
*/
void FLWSUser::AddTradeCancel( OBJID objid, u_long uidPlayer, int nMode )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADECANCEL;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar << nMode;
}

void	FLWSUser::AddTradeConsent( CAr& ar )
{
	if( IsDelete() )	return;

	u_long nSize = 0;
	LPBYTE pBuffer = ar.GetBuffer( &nSize );
	AddBlock( pBuffer, nSize );
}


void FLWSUser::AddOpenShopWnd( CMover* pVendor )
{
	int i;
	m_Snapshot.cb++;
	m_Snapshot.ar << pVendor->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_OPENSHOPWND;
	for( i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		pVendor->m_ShopInventory[i]->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddUnregisterPVendorItem( BYTE byNth )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UNREGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << byNth;
}

void FLWSUser::AddRegisterPVendorItem( BYTE byNth, BYTE byType, DWORD dwItemObjID, int nNum, int nCost )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << byNth << byType << dwItemObjID << nNum << nCost;
}

void FLWSUser::AddPVendorClose( OBJID objidVendor )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << objidVendor;
	m_Snapshot.ar << SNAPSHOTTYPE_PVENDOR_CLOSE;
	m_Snapshot.ar << (BYTE)0;
}

void FLWSUser::AddPVendorItem( FLWSUser* pUser, BOOL bState )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << pUser->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PVENDOR_ITEM;

	BYTE byVendorItem	= 0;
	ptrdiff_t nVendorItemTag = m_Snapshot.ar.ReserveSpace( sizeof( byVendorItem ) );

	for( BYTE byNth = 0; byNth < MAX_VENDITEM; ++byNth )
	{
		FLItemElem* pItemElem = (FLItemElem *)pUser->m_vtInfo.GetItem( byNth );
		if( pItemElem  == NULL )
			continue;

		m_Snapshot.ar << byNth;
		pItemElem->Serialize( m_Snapshot.ar );
		m_Snapshot.ar << pItemElem->GetExtra();
		m_Snapshot.ar << pItemElem->m_nCost;
		++byVendorItem;
	}
	m_Snapshot.ar << bState;

	m_Snapshot.ar.WriteReservedSpace( nVendorItemTag, &byVendorItem, sizeof( byVendorItem ) );
}

void FLWSUser::AddPutItemGuildBank( FLItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)1; // 아이템을 받는 헤더
	pItemElem->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddGetItemGuildBank( FLItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)1; // 아이템을 보내는 헤더
	pItemElem->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddGetGoldGuildBank( DWORD p_Gold, BYTE p_Mode, u_long playerID, BYTE cbCloak )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)p_Mode; // Gold를 보내는 헤더(0, 2가 들어온다.)
	m_Snapshot.ar << p_Gold;
	m_Snapshot.ar << playerID;	   // 길드돈을 빼내간 길원
	m_Snapshot.ar << cbCloak;	   // 망토를 산경우 (= 1)
}


void FLWSUser::AddPutItemBank( BYTE nSlot, FLItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTITEMBANK;
	m_Snapshot.ar << nSlot;
	pItemElem->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddGetItemBank( FLItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMBANK;
	pItemElem->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddPutGoldBank( BYTE nSlot, DWORD dwGold, DWORD dwGoldBank )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTGOLDBANK;
	m_Snapshot.ar << nSlot << dwGold << dwGoldBank;
}

void FLWSUser::AddMoveBankItem( BYTE nSrcIndex, BYTE nDestIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOVEBANKITEM;
	m_Snapshot.ar << nSrcIndex << nDestIndex;
}

void FLWSUser::AddBankIsFull( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BANKISFULL;
}

void FLWSUser::AddRemoveSkillInfluence( WORD wType, WORD wID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	m_Snapshot.ar << wType;
	m_Snapshot.ar << wID;
	
}

void FLWSUser::AddBankWindow( int nMode, DWORD dwId, DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BANKWINDOW;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;

}

void FLWSUser::AddGuildBankWindow( int nMode )
{
	if( IsDelete() )	return;
	

	CGuild* pGuild = GetGuild();
	if (pGuild)
	{
		//////////////////////////////////////////////////////////////////////////
		if( pGuild->m_GuildBank.GetAllocedSerialNumber() == FALSE )
		{
			pGuild->m_GuildBank.ReallocSerialNumber();
			pGuild->m_GuildBank.InsertAllItem();
			g_DPSrvr.UpdateGuildBank( pGuild, GUILD_PUT_ITEM );	// GUILD_PUT_ITEM 임시
		}
		//////////////////////////////////////////////////////////////////////////

		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_GUILD_BANK_WND;
		m_Snapshot.ar << nMode;
		
		m_Snapshot.ar << pGuild->m_nGoldGuild;
		pGuild->m_GuildBank.Serialize(m_Snapshot.ar);
	}
}

void FLWSUser::AddChangeBankPass( int nMode, DWORD dwId, DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHANGEBANKPASS;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;
}

void FLWSUser::AddconfirmBankPass( int nMode, DWORD dwId, DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMBANKPASS;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;
}

void FLWSUser::AddUpdateItemEx( unsigned char id, char cParam, __int64 iValue )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_ITEM_EX;
	m_Snapshot.ar << id << cParam << iValue;
}

void FLWSUser::AddUpdateItem( CHAR cType, DWORD dwId, CHAR cParam, DWORD dwValue, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_ITEM;
	m_Snapshot.ar << cType << dwId << cParam << dwValue;
	m_Snapshot.ar << dwTime;
}

void FLWSUser::AddUpdateBankItem( BYTE nSlot, DWORD dwItemObjID, CHAR cParam, DWORD dwValue )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_BANKITEM;
	m_Snapshot.ar << nSlot << dwItemObjID << cParam << dwValue;
}

void FLWSUser::AddSetExperience( EXPINTEGER nExp1, WORD wLevel, int nSkillPoint, int nSkillLevel, EXPINTEGER nDeathExp, WORD wDeathLevel )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETEXPERIENCE;
	m_Snapshot.ar << nExp1 << wLevel;
	m_Snapshot.ar  << nSkillLevel << nSkillPoint;
	m_Snapshot.ar << nDeathExp << wDeathLevel;
}

void FLWSUser::AddSetFxp( int nFxp, int nFlightLv )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETFXP;
	m_Snapshot.ar << (WORD)nFxp << (WORD)nFlightLv;
}

void FLWSUser::AddSetGrowthLearningPoint( long nRemainGP )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_GROWTH_LEARNING_POINT;
	m_Snapshot.ar << nRemainGP;
}

void FLWSUser::AddSetStatLevel( CHAR chID, long nValue, long nRemainGP )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_STAT_LEVEL;
	m_Snapshot.ar << chID << nValue << nRemainGP;
}

void FLWSUser::AddSetChangeJob( int nJob )
{
	DWORD dwJobLv[MAX_JOB] = {0, };

	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_JOB_SKILL;
	m_Snapshot.ar << nJob;

	m_Snapshot.ar.Write( (void*)&m_aJobSkill[0], sizeof(SKILL) *  ( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL ) );
	m_Snapshot.ar.Write( (void*)dwJobLv, sizeof(DWORD) * MAX_JOB );
}

void FLWSUser::AddReturnSay( int ReturnFlag, const CHAR* lpszPlayer )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RETURNSAY;
	m_Snapshot.ar << ReturnFlag;
	m_Snapshot.ar.WriteString( lpszPlayer );
	
}

void FLWSUser::AddGameTimer( double dCurrentTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GAMETIMER;
	m_Snapshot.ar << dCurrentTime;
//#ifdef _DEBUG
	m_Snapshot.ar << DATE_TIMER().GetMultiple();			// 서버 시간 배속 세팅
//#endif //_DEBUG
}

void FLWSUser::AddDoEquip( DWORD dwItemObjID, DWORD dwItemId, BYTE fEquip )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DOEQUIP;
	m_Snapshot.ar << dwItemObjID << dwItemId << fEquip;
	
}

void FLWSUser::AddTaskBar()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TASKBAR;
	m_playTaskBar.Serialize( m_Snapshot.ar );
	
}

void FLWSUser::AddSendErrorParty( DWORD dw, DWORD dwSkill )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ERRORPARTY;
	m_Snapshot.ar << dw;
	if( dw == ERROR_NOTTARGET )
	{
		m_Snapshot.ar << dwSkill;
	}
	
}

void FLWSUser::AddSetPartyMemberParam( u_long idPlayer, BYTE nParam, int nVal )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_PARTY_MEMBER_PARAM;
	m_Snapshot.ar << idPlayer << nParam << nVal;
	
}

void FLWSUser::AddPartyMember( CParty *pParty, u_long idPlayer, const char* pszLeader, const char* pszMember )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMEMBER;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString( pszLeader );
	m_Snapshot.ar.WriteString( pszMember );

	if( pParty )
	{
		m_Snapshot.ar << pParty->m_nSizeofMember;
		pParty->Serialize( m_Snapshot.ar );
	}
	else
	{
		m_Snapshot.ar << (int)0;
	}
	
}

void FLWSUser::AddPartyExpLevel( int Exp, int Level, int nPoint )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYEXP;
	m_Snapshot.ar << Exp;
	m_Snapshot.ar << Level;
	m_Snapshot.ar << nPoint;
	
}

void FLWSUser::AddSetPartyMode( int nMode, BOOL bOnOff, LONG nPoint ,DWORD dwSkillTime )			
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETPARTYMODE;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwSkillTime;
	m_Snapshot.ar << bOnOff;
	if( bOnOff == TRUE )
	{
		m_Snapshot.ar << nPoint;
	}
	
}

void FLWSUser::AddPartyChangeItemMode( int nItemMode )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGEITEMMODE;
	m_Snapshot.ar << nItemMode;
	
}

void FLWSUser::AddPartyChangeExpMode( int nExpMode )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGEEXPMODE;
	m_Snapshot.ar << nExpMode;
	
}


void FLWSUser::AddPartyChangeName( const char * szPartyName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGENAME;
	m_Snapshot.ar.WriteString( szPartyName );
	
}

void FLWSUser::AddPartyChangeTroup( const char * szPartyName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGETROUP;
	m_Snapshot.ar.WriteString( szPartyName );
	
}

void FLWSUser::AddPartyRequest( FLWSUser * pLeader, FLWSUser * pMember, BOOL bTroup )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYREQEST;
	m_Snapshot.ar << pLeader->m_idPlayer << pLeader->m_nLevel << pLeader->m_nJob << pLeader->GetSex();
	m_Snapshot.ar << pMember->m_idPlayer << pMember->m_nLevel << pMember->m_nJob << pMember->GetSex();
	m_Snapshot.ar.WriteString( pLeader->m_szName );
	m_Snapshot.ar << bTroup;
	
}

void FLWSUser::AddPartyRequestCancel( u_long uLeaderid, u_long uMemberid, int nMode )
{
	// nMode
	// 0 : 실제로 캔슬
	// 1 : 이미 극단에 포함되어 있음
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYREQESTCANCEL;
	m_Snapshot.ar << uLeaderid << uMemberid;
	m_Snapshot.ar << nMode;
	
}

void FLWSUser::AddPartyName()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYNAME;
	m_Snapshot.ar.WriteString( m_szPartyName );
	
}

// nSkill : 파티스킬 ID
// vLeader : 리더의 좌표.
void FLWSUser::AddPartySkillCall( const D3DXVECTOR3 &vLeader )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYSKILL_CALL;
	m_Snapshot.ar << vLeader;
	
}

void	FLWSUser::AddPartySkillBlitz( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYSKILL_BLITZ;
	m_Snapshot.ar << idTarget;
	
}

void FLWSUser::AddFriendGameJoin()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDGAMEJOIN;
	m_RTMessenger.Serialize( m_Snapshot.ar );
}

void FLWSUser::AddPartyChangeLeader( u_long uidChangeLeader )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYCHANGELEADER;
	m_Snapshot.ar << uidChangeLeader;
	
}

void FLWSUser::AddCancelQuest( DWORD dwQuestCancelID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)-1 << dwQuestCancelID;
	
}

void FLWSUser::AddRemoveQuest( DWORD dwQuestCancelID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)0 << dwQuestCancelID;
	
}

void FLWSUser::AddRemoveAllQuest()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)1 << (DWORD)0;
	
}
void FLWSUser::AddRemoveCompleteQuest()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)2 << (DWORD)0;
	
}

void FLWSUser::ADDGameJoin( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDGAMEJOIN;
	m_Snapshot.ar << m_dwSavePlayerTime;
	
}

void FLWSUser::AddFriendReqest( u_long uLeader, LONG nJob, BYTE nSex, const char * szName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDREQEST;
	m_Snapshot.ar << uLeader;
	m_Snapshot.ar << nSex;
	m_Snapshot.ar << nJob;
	m_Snapshot.ar.WriteString( szName );
	
}

void FLWSUser::AddFriendCancel()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDCANCEL;
	
}

void FLWSUser::AddFriendError( BYTE nError, const char * szName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDERROR;
	m_Snapshot.ar << nError;
	m_Snapshot.ar.WriteString( szName );
	
}

void FLWSUser::AddSeasonInfo()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SEASON_INFO;

	g_pSeasonEffect->serializeSeason( m_Snapshot.ar );
}

void FLWSUser::AddSeasonEffect()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SEASON_EFFECT;

	g_pSeasonEffect->serializeEffect( m_Snapshot.ar );

	m_Snapshot.ar.WriteString( prj.m_EventLua.GetWeatherEventTitle().c_str() );
}

void FLWSUser::AddPartyChat( const CHAR* lpName, const CHAR* lpString, OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHAT;
	m_Snapshot.ar << objid;

	m_Snapshot.ar.WriteString( lpName );
	m_Snapshot.ar.WriteString( lpString );
	
}

void FLWSUser::AddAddFriend( u_long idPlayer, const char* lpszPlayer )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIEND;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString( lpszPlayer );
	
}

void FLWSUser::AddRemoveFriend( u_long uidSender )
{
	if( IsDelete() ) return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEFRIEND;
	m_Snapshot.ar << uidSender;
	
}

void FLWSUser::AddDuelRequest( u_long uidSrc, u_long uidDst )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELREQUEST;
	m_Snapshot.ar << uidSrc << uidDst;
	
}

void FLWSUser::AddDuelStart( u_long uidTarget, int bStart )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELSTART;
	m_Snapshot.ar << uidTarget;
	m_Snapshot.ar << bStart;
	
}

void FLWSUser::AddDuelCount( int nCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELCOUNT;
	m_Snapshot.ar << nCount;
	
}

void FLWSUser::AddDuelNo( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELNO;
	m_Snapshot.ar << idTarget;
	
}

void FLWSUser::AddDuelCancel( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELCANCEL;
	m_Snapshot.ar << idTarget;
	
}
// 파티듀얼--------------------------------------------------------------------
void FLWSUser::AddDuelPartyRequest( u_long uidSrc, u_long uidDst )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYREQUEST;
	m_Snapshot.ar << uidSrc << uidDst;
	
}

void FLWSUser::AddDuelPartyStart( LPCTSTR szPartyName, int nMax, OBJID *pMembers, u_long idParty, int bFlag )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYSTART;
	m_Snapshot.ar << nMax;
	m_Snapshot.ar << idParty;
	m_Snapshot.ar.WriteString( szPartyName );
	for( int  i = 0; i < nMax; i ++ )
		m_Snapshot.ar << pMembers[i];
	m_Snapshot.ar << bFlag;
	
}

void FLWSUser::AddDuelPartyNo( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYNO;
	m_Snapshot.ar << idTarget;
	
}

void FLWSUser::AddDuelPartyCancel( CParty *pDuelOther )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYCANCEL;

	if( pDuelOther )
	{
		int		i, nSize = pDuelOther->GetSizeofMember();	// 상대 파티멤버수
		m_Snapshot.ar << nSize;
		for( i = 0; i < nSize; i ++ )	
		{
			m_Snapshot.ar << pDuelOther->m_aMember[i].m_uPlayerId;		// 상대 파티원 유저아이디.
		}
	}
	else
	{
		m_Snapshot.ar << 0;
	}
	
}

void FLWSUser::AddDuelPartyResult( CParty *pDuelOther, BOOL bWin )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYRESULT;
	m_Snapshot.ar << bWin;

	int		i, nSize = pDuelOther->GetSizeofMember();	// 상대 파티멤버수
	m_Snapshot.ar << nSize;
	for( i = 0; i < nSize; i ++ )	
	{
		m_Snapshot.ar << pDuelOther->m_aMember[i].m_uPlayerId;		// 상대 파티원 유저아이디.
	}
	
	
}

void FLWSUser::AddQueryPlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUERY_PLAYER_DATA;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString( pPlayerData->szPlayer );
	m_Snapshot.ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
}

void FLWSUser::AddPlayerData( void )
{
	std::vector<u_long>	aPlayer;
	for( std::map<u_long, Friend>::iterator i1 = m_RTMessenger.begin(); i1 != m_RTMessenger.end(); ++i1 )
		aPlayer.push_back( i1->first );

	CGuild* pGuild	= GetGuild();
	if( pGuild )
	{
		for( std::map<u_long, CGuildMember*>::iterator i2 = pGuild->m_mapPMember.begin(); i2 != pGuild->m_mapPMember.end(); ++i2 )
			aPlayer.push_back( i2->second->m_idPlayer );
	}
	for( size_t i = 0; i < aPlayer.size(); i++ )
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( aPlayer.at(i) );
		if( pPlayerData )
			AddQueryPlayerData( aPlayer.at(i), pPlayerData );
	}
}

void FLWSUser::AddGuildInvite( u_long idGuild, u_long idMaster )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_INVITE;
	m_Snapshot.ar << idGuild;
	m_Snapshot.ar << idMaster;
	
}

void FLWSUser::AddAllGuilds( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_ALL_GUILDS;
	g_GuildMng.Serialize( m_Snapshot.ar, TRUE );
	
}

void FLWSUser::AddMyGuild( void )
{
	if( IsDelete() )	return;

	CGuild* pGuild	= GetGuild();
	if( !pGuild )
		return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD;
	m_Snapshot.ar << pGuild->m_idGuild;
	pGuild->Serialize( m_Snapshot.ar, FALSE );
}

void FLWSUser::AddMyGuildWar( void )
{
	if( IsDelete() )	return;

	CGuildWar* pWar	= GetWar();
	if( !pWar )
		return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_WAR;
	m_Snapshot.ar << pWar->m_idWar;
	pWar->Serialize( m_Snapshot.ar );
	
}

void FLWSUser::AddContribution( CONTRIBUTION_CHANGED_INFO& info )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_CONTRIBUTION;
	m_Snapshot.ar << info;
	
}

void FLWSUser::AddInsertedVote( VOTE_INSERTED_INFO& info )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_ADDVOTE;
	m_Snapshot.ar << info;
	
}

void FLWSUser::AddModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_MODIFYVOTE;
	m_Snapshot.ar << idVote;
	m_Snapshot.ar << cbOperation;
	m_Snapshot.ar << cbExtra;
	
}

void FLWSUser::AddSetNotice( u_long idGuild, const char* szNotice )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_NOTICE;
	m_Snapshot.ar << idGuild;
	m_Snapshot.ar.WriteString( szNotice );
	
}

void FLWSUser::AddSetGuildAuthority( DWORD dwAuthority[] )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_AUTHORITY;
	m_Snapshot.ar.Write( dwAuthority, sizeof(DWORD) * MAX_GM_LEVEL );
	
}

void FLWSUser::AddSetGuildPenya( DWORD dwType, DWORD dwPenya )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_PENYA;
	m_Snapshot.ar << dwType << dwPenya;	
	
}

void FLWSUser::AddGuildRealPenya( int nGoldGuild, int nType )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_REAL_PENYA;
	m_Snapshot.ar << nGoldGuild;	
	m_Snapshot.ar << nType;	
	
}

void FLWSUser::SendGuildRank()
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_REQUEST_GUILDRANK;
	CGuildRank::Instance()->Serialize( m_Snapshot.ar );
	
}

void FLWSUser::AddSMMode( int nType, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SM_MODE;

	DWORD BufSMTime = 0;
	if( dwTime != 0 )
	{
		if( g_AddSMMode.bSMModetime[nType] )
		{
			CTime tSMtime( m_dwSMTime[nType] );
			CTimeSpan ct = tSMtime - CTime::GetCurrentTime();
			BufSMTime = (DWORD)( ct.GetTotalSeconds() );
		}
		else
		{
			BufSMTime = m_dwSMTime[nType];
		}
	}
	else
	{
		BufSMTime = 0;
	}

	m_Snapshot.ar << nType;
	m_Snapshot.ar << BufSMTime;
	
	if( nType < 0 || SM_MAX <= nType )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "SMMODE OVERFLOW nType : %d" ), nType );
	}
	
}

void FLWSUser::AddSMModeAll()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SM_MODE_ALL;

	short nReal = 0;
	ptrdiff_t nRealTag = m_Snapshot.ar.ReserveSpace( sizeof( nReal ) );

	for( int i = 0 ; i < SM_MAX ; ++i )
	{
		DWORD BufSMTime = 0;
		if( 0 < m_dwSMTime[i] )
		{
			if( g_AddSMMode.bSMModetime[i] )
			{
				CTime tSMtime( m_dwSMTime[i] );
				CTimeSpan ct = tSMtime - CTime::GetCurrentTime();
				BufSMTime = (DWORD)( ct.GetTotalSeconds() );
			}
			else
			{
				BufSMTime = m_dwSMTime[i];
			}
			m_Snapshot.ar << i;
			m_Snapshot.ar << BufSMTime;
			++nReal;
		}
	}
	
	m_Snapshot.ar.WriteReservedSpace( nRealTag, &nReal, sizeof( nReal ) );
}

void FLWSUser::AddResistSMMode( BYTE nAttackResistLeft, BYTE nAttackResistRight, BYTE nDefenceResist )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_RESISTSMMODE;
	m_Snapshot.ar << nAttackResistLeft;
	m_Snapshot.ar << nAttackResistRight;
	m_Snapshot.ar << nDefenceResist;
	
}

void FLWSUser::AddCommercialElem( DWORD dwItemId, int nResistSMItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_COMMERCIALELEM;
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << nResistSMItemId;
	
}

void FLWSUser::AddFlyffEvent( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_FLYFF_EVENT;
	g_eLocal.Serialize( m_Snapshot.ar );
	
}

void FLWSUser::AddEventLuaDesc( int nState, string strDesc )
{
	if( strDesc.length() == 0 ) return;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EVENTLUA_DESC;
	m_Snapshot.ar << nState;
	m_Snapshot.ar.WriteString( strDesc.c_str() );	
}

void FLWSUser::AddGoldText( int nPlus )
{
	char szPlus[64] = { 0, };
	char szGold[64] = { 0, };

	FLSPrintf( szPlus, _countof( szPlus ), "%d", nPlus );
	FLSPrintf( szGold, _countof( szGold ), "%d", GetGold() );

	CString strPlus = GetNumberFormatEx( szPlus );
	CString strGold = GetNumberFormatEx( szGold );

	AddDefinedText( TID_GAME_REAPMONEY, "%s %s", strPlus, strGold );
}

void FLWSUser::AddRemoveGuildBankItem( u_long idGuild, DWORD dwId, DWORD dwItemNum )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVE_GUILD_BANK_ITEM;
	m_Snapshot.ar << idGuild << dwId << dwItemNum;
	
}

void FLWSUser::AddDefinedText( int dwText, LPCSTR lpszFormat, ... )
{
	if( IsDelete() )	return;

	TCHAR szBuffer[1024];

	// TODO 유저에게 잘라서 보내도 되는지 좀더 생각해보자
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = FLVSPrintf( szBuffer, _countof(szBuffer), lpszFormat, args );
	va_end( args );

	if( nBuf < 0 )
		return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDTEXT;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString( szBuffer );
	
}

void FLWSUser::AddChatText( int dwText, LPCSTR lpszFormat, ... )
{
	if( IsDelete() )	return;
	
	TCHAR szBuffer[1024];

	// TODO 유저에게 잘라서 보내도 되는지 좀더 생각해보자
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = FLVSPrintf( szBuffer, _countof(szBuffer), lpszFormat, args );
	va_end( args );
	
	if( nBuf < 0 )
		return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTEXT;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString( szBuffer );
	
}

void FLWSUser::AddDefinedText( int dwText )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDTEXT1;
	m_Snapshot.ar << dwText;
	
}

void FLWSUser::AddExpBoxInfo( OBJID objid, DWORD dwSet, DWORD dwTime, u_long idPlayer )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXPBOXINFO;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << dwSet;
	m_Snapshot.ar << dwTime;
	m_Snapshot.ar << idPlayer;
	
}

void FLWSUser::AddDefinedCaption( BOOL bSmall, int dwText, LPCSTR lpszFormat, ... )
{
	if( IsDelete() )	return;
	
	TCHAR szBuffer[1024];

	// TODO 유저에게 잘라서 보내도 되는지 좀더 생각해보자
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = FLVSPrintf( szBuffer, _countof(szBuffer), lpszFormat, args );
	va_end( args );
	
	if( nBuf < 0 )
		return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDCAPTION;
	m_Snapshot.ar << bSmall;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString( szBuffer );
	
}

void FLWSUser::AddSetGuildQuest( int nQuestId, int nState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETGUILDQUEST;
	m_Snapshot.ar << nQuestId << nState;
	
}

void FLWSUser::AddRemoveGuildQuest( int nQuestId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEGUILDQUEST;
	m_Snapshot.ar << nQuestId;
	
}

void FLWSUser::AddSetQuest( LPQUEST lpQuest )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETQUEST;
	m_Snapshot.ar.Write( lpQuest, sizeof( QUEST ) );//ar << nQuestIdx << nState;
	
}

void FLWSUser::AddQueryGetDestObj( OBJID idFrom )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYGETDESTOBJ;
	m_Snapshot.ar << idFrom;
	
}

void FLWSUser::AddGetDestObj( OBJID objid, OBJID objidDest, FLOAT fRange )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_GETDESTOBJ;
	m_Snapshot.ar << objidDest;
	m_Snapshot.ar << fRange;

	
}

//  void FLWSUser::AddQueryGetPos( OBJID idFrom )
//  {
//  	if( IsDelete() )	return;
//  	
//  	m_Snapshot.cb++;
//  	m_Snapshot.ar << GetId();
//  	m_Snapshot.ar << SNAPSHOTTYPE_QUERYGETPOS;
//  	m_Snapshot.ar << idFrom;
//  	
//  }

void FLWSUser::AddGetPos( OBJID objid, const D3DXVECTOR3 & vPos, float fAngle )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_GETPOS;
	m_Snapshot.ar << vPos << fAngle;
	
}

void FLWSUser::AddRevivalBySkillMessage()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REVIVAL_BY_SKILL_MESSAGE;
	
}

// 디버깅용으로 쓰인다.  선택된 무버의 좌표등의 값들을 클라로 보낸다.
void FLWSUser::AddCorrReq( CMover *pMover )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << pMover->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CORRREQ;
	m_Snapshot.ar << pMover->GetPos();
	m_Snapshot.ar << pMover->m_idAttacker;
	
}

void FLWSUser::AddSetFuel( int nFuel, DWORD tmAccFuel )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETFUEL;
	m_Snapshot.ar << nFuel << tmAccFuel;
	
}

void FLWSUser::AddSetSkill( DWORD dwSkill, DWORD dwSkillLevel )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CMDSETSKILLLEVEL;
	m_Snapshot.ar << dwSkill << dwSkillLevel;
	
}

void FLWSUser::AddMotionError( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOTION;
	m_Snapshot.ar << (DWORD)OBJMSG_NONE;
	
}




// 클라이언트에게 쪽지를 보낸다.
void FLWSUser::AddTag( short nTagCount, const TAG_ENTRY* tags )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAG;

	short nReal = 0;
	ptrdiff_t nRealTag = m_Snapshot.ar.ReserveSpace( sizeof( nReal ) );

	for( short i=0; i<nTagCount; ++i )
	{
		LPCTSTR lpszName = CPlayerDataCenter::GetInstance()->GetPlayerString( tags[i].idFrom );
		if( lpszName )
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "TAG:%s %d %s" ), lpszName, tags[i].dwDate, tags[i].szString);

			m_Snapshot.ar.WriteString( lpszName );				// 누구에게서 
			m_Snapshot.ar << tags[i].dwDate;					// 언제 
			m_Snapshot.ar.WriteString( tags[i].szString );		// 쪽지 내용 

			nReal++;
		}
	}
	m_Snapshot.ar.WriteReservedSpace( nRealTag, &nReal, sizeof( nReal ) );
}


// 버프스킬 적용상태를 클라에 전송
void FLWSUser::AddSetSkillState( CMover *pMover, DWORD dwSkill, DWORD dwLevel, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << pMover->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETSKILLSTATE;
	m_Snapshot.ar << dwSkill;
	m_Snapshot.ar << dwLevel;
	m_Snapshot.ar << dwTime;
}

void FLWSUser::AddPlayMusic( u_long idMusic )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYMUSIC;
	m_Snapshot.ar << idMusic;
	
}

void FLWSUser::AddPlaySound( u_long idSound )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYSOUND;
	m_Snapshot.ar << (BYTE)0;
	m_Snapshot.ar << idSound;
	
}

void FLWSUser::AddPlaySound2( const char* szSound )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYSOUND;
	m_Snapshot.ar << (BYTE)1;
	m_Snapshot.ar.WriteString( szSound );
	
}

void FLWSUser::AddMoverFocus( CMover *pMover )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MOVERFOCUS;
	m_Snapshot.ar << pMover->m_idPlayer;
	m_Snapshot.ar << pMover->GetGold();
	m_Snapshot.ar << pMover->GetExp1();
	
}

void FLWSUser::AddPartyMapInfo( int nIndex, D3DXVECTOR3 vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMAPINFO;
	m_Snapshot.ar << nIndex;
	m_Snapshot.ar << vPos;
	
}

void FLWSUser::AddSetNaviPoint( const NaviPoint & nv, OBJID objid, const char* Name )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_SETNAVIPOINT;
	m_Snapshot.ar << nv.Pos;// << nv.On;
	m_Snapshot.ar.WriteString( Name );
	
}

void FLWSUser::AddSetDuel( CMover* pMover )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETDUEL;
	m_Snapshot.ar << pMover->GetId() << pMover->m_nDuel << pMover->m_nDuelState << pMover->m_idDuelOther << pMover->m_idDuelParty;
	
}

void FLWSUser::AddPKValue()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PK_RELATION;
	m_Snapshot.ar << PK_PKVALUE;
	m_Snapshot.ar << GetPKValue();
}


void FLWSUser::ScheduleDestory( DWORD dwDestoryTime  )
{
	m_dwDestroyTime = dwDestoryTime;
}

void FLWSUser::AddSnoop( const char* lpString )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SNOOP;
	m_Snapshot.ar.WriteString( lpString );
	
}

void FLWSUser::AddSetCheerParam( int nCheerPoint, DWORD dwRest, BOOL bAdd )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SETCHEERPARAM;
	m_Snapshot.ar << nCheerPoint << dwRest;
	m_Snapshot.ar << bAdd; // chipi_080412
	
}

void FLWSUser::AddQueryEquip( FLWSUser* pUser )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( pUser );
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYEQUIP;

	int cbEquip		= 0;
	ptrdiff_t nEquipTag = m_Snapshot.ar.ReserveSpace( sizeof( cbEquip ) );
	
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		FLItemElem* pItemElem	= pUser->GetEquipItem( i );
		if( pItemElem )
		{
			m_Snapshot.ar << i;

			pItemElem->Serialize( m_Snapshot.ar );

// 			EQUIP_INFO_ADD kEquip;
// 			kEquip.CopyItemOption( *pItemElem );
// 
// 			kEquip.Serialize( m_Snapshot.ar );

// 			pItemElem->SerializePiercingOption( m_Snapshot.ar );
// 			pItemElem->SerializeRandomOptionExtension( m_Snapshot.ar );
// 			m_Snapshot.ar << pItemElem->m_byItemResist;
// 			m_Snapshot.ar << pItemElem->m_nResistAbilityOption;
// 			m_Snapshot.ar << pItemElem->GetRandomOptionOriginID();
// 			m_Snapshot.ar << pItemElem->GetLevelDown();

			cbEquip++;
		}
	}

	m_Snapshot.ar.WriteReservedSpace( nEquipTag, &cbEquip, sizeof( cbEquip ) );
}

void FLWSUser::AddSummonFriendUse( FLItemElem* pItemElem )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_FRIEND;
	m_Snapshot.ar << MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId );
	m_Snapshot.ar << pItemElem->m_dwObjId;
	
}
void FLWSUser::AddSummonFriendConfirm( OBJID objid, DWORD dwData, const char* szName, const char* szWorldName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_FRIEND_CONFIRM;
	m_Snapshot.ar << objid << dwData;
	m_Snapshot.ar.WriteString( szName );
	m_Snapshot.ar.WriteString( szWorldName );
	
}
void FLWSUser::AddSummonPartyConfirm( OBJID objid, DWORD dwData, const char * szWorldName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_PARTY_CONFIRM;
	m_Snapshot.ar << objid << dwData;
	m_Snapshot.ar.WriteString( szWorldName );
	
}

void FLWSUser::SetReturnPos( const D3DXVECTOR3& vPos )
{
	FLASSERT( GetWorld() );

	m_dwReturnWorldID = GetWorld()->GetID();
	m_vReturnPos = vPos;
	FLTRACE_LOG( PROGRAM_NAME, _T( "%f %f %f" ), m_vReturnPos.x, m_vReturnPos.y, m_vReturnPos.z );
}

D3DXVECTOR3& FLWSUser::GetReturnPos( DWORD* pdwWorldID )
{
	*pdwWorldID = m_dwReturnWorldID;
	return m_vReturnPos;
}


// 아이템을 사용할 수 있는 상태입니까?
BOOL FLWSUser::IsUsableState( DWORD dwId )
{
	FLItemElem* pItemElem = m_Inventory.GetAtId( dwId );
	if( pItemElem == NULL )
	{
		return FALSE;
	}
	if( pItemElem->IsEmpty() == TRUE )
	{
		return FALSE;
	}

	PT_ITEM_SPEC pItemSpec = pItemElem->GetProp();
	if( pItemSpec == NULL )
	{
		return FALSE;
	}

	if( m_Inventory.IsEquip( dwId ) == FALSE )
	{
		// 장비가 장착되어 있지 않은 상태에서는 거래상태를 체크한다.
		// 장비가 장착되어 있는 상태에서는 거래중에도 벗을 수 있어야 하기 때문에 검사하지 않는다. 
		if( m_vtInfo.GetOtherID() != NULL_ID )
		{
			AddDefinedText( TID_GAME_TRADELIMITUSING, "" );
			return FALSE;
		}
		if( m_vtInfo.VendorIsVendor() )
		{
			return FALSE;
		}
	}

	if( m_bAllAction == FALSE )
		return FALSE;

	return TRUE;
}

void FLWSUser::AddPetState( DWORD dwPetId, WORD wLife, WORD wEnergy, DWORD dwExp )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET_STATE;
	m_Snapshot.ar << dwPetId << wLife << wEnergy << dwExp;
}

void	FLWSUser::AddPetSetExp( DWORD dwExp )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET_SET_EXP;
	m_Snapshot.ar << dwExp;
}

void FLWSUser::AddPet( CPet* pPet, BYTE nPetLevelup )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET;
	pPet->Serialize( m_Snapshot.ar );
	m_Snapshot.ar << nPetLevelup;
}

void FLWSUser::AddPetFoodMill(int nResult, int nCount)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET_MILL;
	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nCount;
}

void	FLWSUser::ProcessCollecting( void )
{
	FLItemElem* pCol		= GetCollector();
	if( pCol == NULL )	
	{
		// 1. 통상 상태
		if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
			StopCollecting();
	}
	else if( IsDisguise() )	
	{
		if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
			StopCollecting();
	}
	else if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
	{
		// 3. 채집 상태
		// 3.1. 배터리 소모
		if( !HasBuff( BUFF_ITEM, ITEM_INDEX( 26454, II_GEN_TOO_COL_SILVERBATTERY ) )
			&& !HasBuff( BUFF_ITEM, ITEM_INDEX( 26455, II_GEN_TOO_COL_GOLDBATTERY ) ) 
			&& !HasBuff( BUFF_ITEM, ITEM_INDEX( 26560, II_GEN_TOO_COL_BATTERY001 ) ) 
#ifdef BATTERY_PREMIUM
			&& !HasBuff( BUFF_ITEM, ITEM_INDEX( 25455, II_GEN_TOO_COL_SILVERBATTERY_P ) )
			&& !HasBuff( BUFF_ITEM, ITEM_INDEX( 25456, II_GEN_TOO_COL_GOLDBATTERY_P ) ) 
			&& !HasBuff( BUFF_ITEM, ITEM_INDEX( 25457, II_GEN_TOO_COL_BATTERY001_P ) )  
#endif			
			)
		{
			if( --pCol->m_nHitPoint < 0 )
				pCol->m_nHitPoint	= 0;

			UpdateItem( pCol->m_dwObjId, UI_HP, pCol->m_nHitPoint );

			if( pCol->m_nHitPoint == 0 )
			{
				StopCollecting();
				return;
			}
		}

		// 3.2. 채집
		CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
		if( ++m_nCollecting >= pProperty->GetCool( pCol->GetAbilityOption() ) )
		{
			m_nCollecting		= 0;

			DWORD dwItemId	= pProperty->GetItem();

#ifdef BATTERY_PREMIUM
			// 프리미엄 배터리
			if( HasBuff( BUFF_ITEM, ITEM_INDEX( 25455, II_GEN_TOO_COL_SILVERBATTERY_P ) )
			|| HasBuff( BUFF_ITEM, ITEM_INDEX( 25456, II_GEN_TOO_COL_GOLDBATTERY_P ) ) 
			|| HasBuff( BUFF_ITEM, ITEM_INDEX( 25457, II_GEN_TOO_COL_BATTERY001_P ) ) )
				dwItemId = pProperty->GetPremiumItem();
#endif
			if( dwItemId == 0 )	
				return;
			FLItemElem itemElem;
			itemElem.m_dwItemId	= dwItemId;
			itemElem.m_nItemNum		= 1;
			itemElem.SetSerialNumber();
			if( CreateItem( &itemElem ) == TRUE )
			{
//				PT_ITEM_SPEC pItemProp		= itemElem.GetProp();
//				if( pItemProp )
//					AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
				AddRestartCollecting( dwItemId );
				// log
				LogItemInfo	log;
				//log.Action	= "c";
				//log.SendName	= GetName();
				//log.RecvName	= "CreateItem";
				FLStrcpy( log.Action, _countof( log.Action ), "c" );
				FLStrcpy( log.SendName, _countof( log.SendName ), GetName() );
				FLStrcpy( log.RecvName, _countof( log.RecvName ), "CreateItem" );
				log.WorldId		= GetWorld()->GetID();
				log.Gold	= GetGold();
				log.Gold2	= GetGold();
				g_DPSrvr.OnLogItem( log, &itemElem, itemElem.m_nItemNum );
			}
		}
		//
	}
	else
	{
		// 2. 채집 대기 상태
	}
}

void FLWSUser::AddRestartCollecting( DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_RESTART_COLLECTING;
	m_Snapshot.ar << dwItemId;
}

void FLWSUser::StartCollecting( void )
{
	if( IsNoActionState() == true )
	{
		return;
	}

	if( !IsRegionAttr( RA_COLLECTING ) )
	{
		AddDefinedText( TID_GAME_COLLECTING_REGION );		
		return;
	}

	// state
	if( GetCollector() == NULL )
		return;
	CMover::StartCollecting();
	g_xWSUserManager->AddStartCollecting( this );
	// log
	LogItemInfo	log;
	//log.Action	= "c";
	//log.SendName	= GetName();
	//log.RecvName	= "StartCollecting";
	FLStrcpy( log.Action, _countof( log.Action ), "c" );
	FLStrcpy( log.SendName, _countof( log.SendName ), GetName() );
	FLStrcpy( log.RecvName, _countof( log.RecvName ), "StartCollecting" );
	log.WorldId		= GetWorld()->GetID();
	log.Gold	= GetGold();
	log.Gold2	= GetGold();
	log.Gold_1	= GetCollector()->m_nHitPoint;
	g_DPSrvr.OnLogItem( log );
	m_dwHonorCheckTime = GetTickCount();
}

void FLWSUser::StopCollecting( void )
{
	if( IsNoActionState() == true )
	{
		return;
	}

	// state
	CMover::StopCollecting();
	m_nCollecting	= 0;
	g_xWSUserManager->AddStopCollecting( this );
	// log
	LogItemInfo	log;
	//log.Action	= "c";
	//log.SendName	= GetName();
	//log.RecvName	= "StopCollecting";
	FLStrcpy( log.Action, _countof( log.Action ), "c" );
	FLStrcpy( log.SendName, _countof( log.SendName ), GetName() );
	FLStrcpy( log.RecvName, _countof( log.RecvName ), "StopCollecting" );


	log.WorldId		= GetWorld()->GetID();
	g_DPSrvr.OnLogItem( log );
}

BOOL FLWSUser::DoUseItemBattery( void )
{
	FLItemElem* pCol		= GetCollector();
	if( pCol == NULL || m_pActMover->GetActionState() == OBJSTA_COLLECT )
		return FALSE;
	UpdateItem( pCol->m_dwObjId, UI_HP, CCollectingProperty::GetInstance()->GetMaxBattery() );

	return TRUE;
}

void	FLWSUser::SummonPlayer( FLWSUser* pDestUser )
{
	// this가 소환하는 주체
	if( IsDie() == TRUE )
	{
		return;
	}

	if( IsValidObj( pDestUser ) == FALSE )
	{
		return;
	}

	CWorld* pWorld = GetWorld();
	if( pWorld == NULL )
	{
		return;
	}

	const int nLayer = GetLayer();
	if( pWorld->m_linkMap.GetLinkMap( nLayer ) == NULL ) 
	{
		return;
	}

	const REPLACE_TYPE nReplaceType = IsAuthHigher( AUTH_GAMEMASTER ) ? REPLACE_FORCE : REPLACE_NORMAL;

	pDestUser->REPLACE( g_uIdofMulti, pWorld->GetID(), GetPos(), nReplaceType, nLayer );
}

void FLWSUser::AddGC1to1TenderOpenWnd( int nPenya )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_TENDEROPENWND;
	m_Snapshot.ar << nPenya;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nJoinPenya;
}

void FLWSUser::AddGC1to1TenderGuildView( int nPenya, int nRanking, time_t t, std::vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecTenderGuild )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_TENDERVIEW;
	m_Snapshot.ar << nPenya;
	m_Snapshot.ar << nRanking;
	m_Snapshot.ar << t;
	m_Snapshot.ar << vecTenderGuild.size();
	for( int i=0; i<(int)( vecTenderGuild.size() ); i++ )
		m_Snapshot.ar << vecTenderGuild.at( i ).ulGuildId;
}

void FLWSUser::AddGC1to1NowState( int nState, int nTime, int nProgCount )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_NOWSTATE;
	m_Snapshot.ar << nState << nTime;

	if( nState == g_GuildCombat1to1Mng.GC1TO1_ENTRANCE )
	{
		// 구성된 멤버 ID목록
		int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( this );
		int nSize = g_GuildCombat1to1Mng.m_vecTenderGuild.at( nIndex ).vecMemberId.size();
		m_Snapshot.ar << nSize;
		for( int i=0; i<nSize; i++ )
			m_Snapshot.ar << g_GuildCombat1to1Mng.m_vecTenderGuild.at( nIndex ).vecMemberId.at( i );
		// 상대편 길드 ID
		nIndex = g_GuildCombat1to1Mng.GetTargetTenderGuildIndexByUser( this );
		m_Snapshot.ar << g_GuildCombat1to1Mng.m_vecTenderGuild.at( nIndex ).ulGuildId;
	}

	if( nState == CGuildCombat1to1::GC1TO1WAR_WAIT )
		m_Snapshot.ar << nProgCount;
}

void FLWSUser::AddGC1to1MemberLineUpOpenWnd( std::vector<u_long>& vecMemberId )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_MEMBERLINEUPOPENWND;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nMinJoinPlayerLevel;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nMinJoinPlayer;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nMaxJoinPlayer;
	m_Snapshot.ar << vecMemberId.size();
	for( int i=0; i<(int)( vecMemberId.size() ); i++ )
		m_Snapshot.ar << vecMemberId.at( i );
}

void FLWSUser::AddGC1to1WarResult( int m_nState, u_long uIdPlayer, int nWinCount0, int nWinCount1 )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_WARRESULT;
	m_Snapshot.ar << m_nState << uIdPlayer << nWinCount0 << nWinCount1;
}

void FLWSUser::RemoveAngel( void )
{
	if( HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		PT_ITEM_SPEC pItemProp	= NULL;
		if( pBuff )
			pItemProp	= pBuff->GetSpecItem();
		if( pItemProp )
		{
			LogItemInfo aLogItem;
			//aLogItem.Action = "&";
			//aLogItem.SendName	= GetName();
			//aLogItem.RecvName	= "ANGEL_DIE";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "&" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ANGEL_DIE" );
			aLogItem.WorldId	= GetWorld()->GetID();
			aLogItem.Gold		= aLogItem.Gold2	= GetGold();
			aLogItem.Gold_1		= (DWORD)( m_nAngelExp );
			//aLogItem.ItemName	= pItemProp->szName;
			FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", pItemProp->dwID );
			g_DPSrvr.OnLogItem( aLogItem );
		}
		RemoveIk3Buffs( IK3_ANGEL_BUFF );
		m_nAngelLevel	= 0;
		m_nAngelExp		= 0;
	}

	AddAngelInfo();
}

void	FLWSUser::AddPocketAttribute( int nAttribute, int nPocket, int nData )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_POCKET_ATTRIBUTE;
	m_Snapshot.ar << nAttribute << nPocket << nData;
}

void	FLWSUser::AddPocketView( void )
{
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		char sPocket[100]	= { 0,};
		FLSPrintf( sPocket, _countof( sPocket ), "p# : %d, b : %d, t : %d", nPocket, m_Pocket.IsAvailable( nPocket ), m_Pocket.GetAvailable( nPocket ) );
		AddText( sPocket );

		if( m_Pocket.IsAvailable( nPocket, FALSE ) == TRUE )
		{
			for( DWORD dwObjId = 0; dwObjId < m_Pocket.m_kPocket[nPocket].GetMax(); ++dwObjId )
			{
				FLItemElem* pItem	= m_Pocket.m_kPocket[nPocket].GetAtId( dwObjId );
				if( pItem )
				{
					char sItem[100]	= { 0,};
					FLSPrintf( sItem, _countof( sItem ), "nItem : %d, szName: %s, nNum : %d", dwObjId, pItem->GetProp()->szName, pItem->m_nItemNum );
					AddText( sItem );
				}
			}
		}
	}
}

void	FLWSUser::AddPocketAddItem( int nPocket, FLItemElem* pItem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_POCKET_ADD_ITEM;
	m_Snapshot.ar << nPocket;
	pItem->Serialize( m_Snapshot.ar );
}

void	FLWSUser::AddPocketRemoveItem( int nPocket, DWORD dwItemObjID, int nNum, BOOL bExpiration )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_POCKET_REMOVE_ITEM;
	m_Snapshot.ar << nPocket << dwItemObjID << nNum << bExpiration;
}

void	FLWSUser::AddQuePetResurrectionResult( BOOL bResult )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUE_PETRESURRECTION_RESULT;
	m_Snapshot.ar << bResult;
}

void FLWSUser::ValidateItem( void )
{
	DWORD dwSize	= m_Inventory.GetMax();
	PT_ITEM_SPEC pItemProp;
	for( DWORD i = 0; i < dwSize; i++ )
	{
		FLItemElem* pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			pItemElem->SetExtra( 0 );
			if( pItemElem->GetSerialNumber() == 0 )
				pItemElem->SetSerialNumber();

			pItemProp	= pItemElem->GetProp();
			if( !pItemProp || pItemProp->dwItemKind3 == IK3_VIRTUAL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "ITEMPROP//0//%s//%d" ), GetName(), pItemElem->m_dwItemId );
				m_Inventory.RemoveAtId( i );
				continue;
			}
			if( pItemElem->m_nItemNum <= 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "ITEMNUM//0//%s//%d" ), GetName(), pItemElem->m_nItemNum );
				m_Inventory.RemoveAtId( i );
			}
		}
	}
}

void FLWSUser::AdjustGuildQuest( DWORD dwWorldId )
{
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
	int nId = -1;
	if( dwWorldId == WI_WORLD_MADRIGAL )
		nId = pProcessor->PtInQuestRect( GetPos() );

	if( nId > -1 )
	{
		PGUILDQUESTELEM pElem	= pProcessor->GetGuildQuest( nId );
		if( !pElem || pElem->idGuild != m_idGuild )
		{
			CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
			if( pWorld )
			{
				PRegionElem pRgnElem	= NULL;
				if( IsChaotic() )
				{
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );
				}
				else
				{
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), GetPos() );
				}
				if( pRgnElem )
					SetPos( pRgnElem->m_vPos );
			}
		}
	}
}

void FLWSUser::AdjustMailboxState( void )
{
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( m_idPlayer );
	if( pMailBox && pMailBox->IsStampedMailExists() )
		SetMode( MODE_MAILBOX );
	else
		SetNotMode( MODE_MAILBOX );
}

void FLWSUser::AddEventLuaDesc( void )
{
	std::vector<BYTE> vecList	= prj.m_EventLua.GetEventList();
	for( int it = 0; it < (int)( vecList.size() ); it++ )
		AddEventLuaDesc( 2, prj.m_EventLua.GetDesc( vecList.at( it ) ) );
}

void FLWSUser::AdjustPartyQuest( DWORD dwWorldId )
{
	CPartyQuestProcessor* pProc		= CPartyQuestProcessor::GetInstance();
	int nId		= pProc->PtInQuestRect( dwWorldId, GetPos() );
	if( nId > -1 )
	{
		PPARTYQUESTELEM pElem	= pProc->GetPartyQuest( nId );
		if( pElem && pElem->idParty == m_idparty )
		{
			DWORD dwTime	= pElem->dwEndTime - GetTickCount();
			AddQuestTextTime( TRUE, pElem->nProcess , dwTime );
		}
		else if( !pElem || pElem->idParty != m_idparty )
		{
			SetMode( MODE_OUTOF_PARTYQUESTRGN );
		}
	}
}

// void	FLWSUser::AdjustAuthority()
// {
// 	if( g_xFlyffConfig->GetMainLanguage() == LANG_USA )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "64.71.27.34" ) < 0
// 			&& strIp.Find( "172.16.100." ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "222.252.24.47" ) < 0 && strIp.Find( "222.255.38.14" ) < 0
// 			&& strIp.Find( "172.16.100." ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	//	mulcom	BEGIN100302	러시아 GM 권한 특정 IP로 제한 설정
// 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_RUS )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "195.218.191.95" ) < 0
// 			&& strIp.Find( "93.157.148.25" ) < 0 && strIp.Find( "213.129.119.90" ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	//	mulcom	END100302	러시아 GM 권한 특정 IP로 제한 설정
// 	//	mulcom	BEGIN100304	대만 GM 권한 특정 IP로 제한 설정
// 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "60.251.100.51" ) < 0 && strIp.Find( "60.251.100.52" ) < 0
// 			&& strIp.Find( "172.16.100." ) < 0 && strIp.Find( "175.98.112." ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	//	mulcom	END100304	대만 GM 권한 특정 IP로 제한 설정
// 	//	mulcom	BEGIN100305	일본 GM 권한 특정 IP로 제한 설정
// 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "219.111.2.70" ) < 0
// 			&& strIp.Find( "180.235.103." ) < 0 && strIp.Find( "192.168.160.212" ) < 0
// 			&& strIp.Find( "192.168.160.175" ) < 0 && strIp.Find( "192.168.160.249" ) < 0
// 			&& strIp.Find( "10.73.1.134" ) < 0 && strIp.Find( "10.73.1.68" ) < 0 
// 			&& strIp.Find( "122.211.251.254" ) < 0 && strIp.Find( "210.148.99.30" ) < 0
// 			&& strIp.Find( "210.148.99.131" ) < 0 && strIp.Find( "122.211.251.100" ) < 0 
// 			&& strIp.Find( "122.211.251.101" ) < 0
// 			&& strIp.Find( "172.16.100." ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	//	mulcom	END100305	일본 GM 권한 특정 IP로 제한 설정
// 	//	mulcom	BEGIN100309	유럽(독일/프랑스) GM 권한 특정 IP로 제한 설정
// 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_GER || g_xFlyffConfig->GetMainLanguage() == LANG_FRE )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "87.198.181.130" ) < 0
// 			&& strIp.Find( "172.16.100." ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	//	mulcom	END100309	유럽(독일/프랑스) GM 권한 특정 IP로 제한 설정
// 	//	mulcom	BEGIN100309	태국 GM 권한 특정 IP로 제한 설정
// 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
// 	{
// 		CString strIp	= m_playAccount.lpAddr;
// 		if( strIp.Find( "59.10.106.2" ) < 0 && strIp.Find( "203.107.158.41" ) < 0
// 			&& strIp.Find( "203.107.158.42" ) < 0 && strIp.Find( "203.107.158.43" ) < 0
// 			&& strIp.Find( "203.107.158.44" ) < 0 && strIp.Find( "203.107.158.45" ) < 0
// 			&& strIp.Find( "203.107.158.46" ) < 0 && strIp.Find( "58.64.24.253" ) < 0
// 			&& strIp.Find( "172.16.100." ) < 0 && strIp.Find( "58.10.124.160" ) < 0
// 			&& strIp.Find( "61.91.25.66" ) < 0 )
// 		{
// 			m_dwAuthorization	= AUTH_GENERAL;
// 		}
// 	}
// 	//	mulcom	END100309	태국 GM 권한 특정 IP로 제한 설정
// 
// #ifdef __INTERNALSERVER
// 	m_dwAuthorization	= AUTH_ADMINISTRATOR;
// #endif // __INTERNALSERVER
// }

void FLWSUser::AddCouple()
{
	if( IsDelete() )	return;
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( m_idPlayer );
	if( pCouple )
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_COUPLE;
		pCouple->Serialize( m_Snapshot.ar );
	}
}

void FLWSUser::AddProposeResult( u_long idProposer, const char* pszProposer )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PROPOSE_RESULT;
	m_Snapshot.ar << idProposer;
	m_Snapshot.ar.WriteString( pszProposer );
}

void FLWSUser::AddCoupleResult( u_long idPartner, const char* pszPartner )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COUPLE_RESULT;
	m_Snapshot.ar << idPartner;
	m_Snapshot.ar.WriteString( pszPartner );
}

void FLWSUser::AddDecoupleResult()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DECOUPLE_RESULT;
}

// 1회/1초 
void FLWSUser::ProcessCouple()
{
	BOOL bTick	= FALSE;
	if( ++m_cbProcessCouple > 60 )
	{
		bTick	= TRUE;
		m_cbProcessCouple	= 0;
	}
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( m_idPlayer );
	if( pCouple )
	{
		FLWSUser* pPartner	= static_cast<FLWSUser*>( prj.GetUserByID( pCouple->GetPartner( m_idPlayer ) ) );
		if( IsValidObj( pPartner ) )
		{
			if( bTick && m_idPlayer > pPartner->m_idPlayer && pCouple->GetLevel() < CCouple::eMaxLevel )	// 1 커플 1회 요청
				g_dpDBClient.SendQueryAddCoupleExperience( m_idPlayer, 1 );

			if( HasBuffByIk3( IK3_COUPLE_BUFF ) == FALSE )
			{
				ActiveCoupleBuff( pCouple->GetLevel() );
			}
		}
		else
		{
			RemoveIk3Buffs( IK3_COUPLE_BUFF );
		}
	}
	else
	{
		RemoveIk3Buffs( IK3_COUPLE_BUFF );
	}
}

void FLWSUser::ActiveCoupleBuff( int nLevel )
{
	VS& vSkills	= CCoupleProperty::Instance()->GetSkill( nLevel );
	for( int i = 0; i < (int)( vSkills.size() ); i++ )
	{
		PT_ITEM_SPEC pProp	= g_xSpecManager->GetSpecItem( vSkills.at( i ) );
		if( pProp != NULL )
		{
			g_xApplyItemEffect->DoApplyEffect( this, this, pProp );
		}
	}
}

void FLWSUser::AddAddCoupleExperience( int nExperience )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADD_COUPLE_EXPERIENCE;
	m_Snapshot.ar << nExperience;
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FLWSUserManager* FLWSUserManager::GetInstance()
{
	static FLWSUserManager	xUserManager;

	return	&xUserManager;
}

FLWSUserManager::FLWSUserManager()
{
	m_lCount = 0;
}

FLWSUserManager::~FLWSUserManager()
{
}


// 캐쉬서버와 연결이 끊길 경우, 모든 유저를 삭제한다.
// 월드서버에서 소켓과 접속이 끊길 경우 호출된다.
void FLWSUserManager::RemoveAllUsers()
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		DestroyPlayer( it->second );
	}

	m_users.clear();
	m_lCount = 0;
}


FLWSUser* FLWSUserManager::AddUser( DPID dpidCache, DPID dpidUser, DPID dpidSocket )
{
	FLWSUser *pUser = GetUser( dpidCache, dpidUser );
	if( pUser == NULL )
	{
		++m_lCount;
		
		pUser = new FLWSUser( dpidCache, dpidSocket );
		pUser->m_dwSerial = dpidUser;
		m_users.insert( make_pair( dpidUser, pUser ) );
	}
	else
	{
		FLASSERT( FALSE );
		FLERROR_LOG( PROGRAM_NAME, _T( "이미 존재하는 유저입니다. Cache: %u, User: %u, Socket: %u" ), dpidCache, dpidUser, dpidSocket );
		pUser = NULL;
	}

	return pUser;
}

extern CCommonCtrl* CreateExpBox( FLWSUser* pUser );

void FLWSUserManager::RemoveUser( DWORD dwSerial )
{
	std::map<DWORD, FLWSUser*>::iterator it = m_users.find( dwSerial );
	if( it == m_users.end() )
		return;

	FLWSUser* pUser = it->second;

	if( IsValidObj( pUser ) ) 
	{
		pUser->OnTradeRemoveUser();

		m_users.erase( it );
		--m_lCount;
		RemoveUserFromCacheMsg( pUser );
	}
}


// 캐쉬서버로 부터 PACKETTYPE_LEAVE를 받은 경우에만  
void FLWSUserManager::RemoveUserFromCacheMsg( FLWSUser *pUser )
{
	// 안전지역이면 즉시 종료 
	if( pUser->m_pWorld == NULL || pUser->IsRegionAttr(RA_SAFETY) || pUser->IsAuthHigher( AUTH_GAMEMASTER ) )   // 안전지역?		
	{
		DestroyPlayer( pUser );
		return;
	}

	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
		DestroyPlayer( pUser );
		return;
	}

	// 이벤트 아레나 채널은 즉시 종료
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		DestroyPlayer( pUser );
		return;
	}

	FLASSERT( pUser->m_dwDestroyTime == 0 );

	// 반대의 경우는 종료예약 
	if( pUser->m_dwLeavePenatyTime == 0 )						// 페널티 시간이 설정 안되었으면 
	{
		pUser->ScheduleDestory( ::timeGetTime() + TIMEWAIT_CLOSE * 1000 );	//  종료예약 
		g_dpDBClient.SendPreventLogin( pUser->m_playAccount.lpszAccount, TIMEWAIT_CLOSE + 5 );
	}
	else if( ::timeGetTime() > pUser->m_dwLeavePenatyTime )		// 페널티 시간을 견디었으면 
	{
		DestroyPlayer( pUser );												// 종료 
	}
	else														// 페널티 시간을 견디지 못했으면
	{
		pUser->ScheduleDestory( pUser->m_dwLeavePenatyTime );				//  종료예약 
		g_dpDBClient.SendPreventLogin( pUser->m_playAccount.lpszAccount, TIMEWAIT_CLOSE );
	}
}

FLWSUser* FLWSUserManager::GetUser( DPID dpidCache, DPID dpidUser )
{
	UNUSED_ALWAYS(dpidCache);

	std::map<DWORD, FLWSUser*>::iterator it = m_users.find( dpidUser );
	if( it != m_users.end() )
		return it->second;
	else
		return NULL;
}

FLWSUser* FLWSUserManager::GetUserByPlayerID( u_long idPlayer )
{
	return (FLWSUser*)prj.GetUserByID( idPlayer );
}

#ifdef __LAYER_1015
BOOL FLWSUserManager::AddPlayer( FLWSUser *pUser, DWORD dwWorldID, int nLayer )
#else	// __LAYER_1015
BOOL FLWSUserManager::AddPlayer( FLWSUser *pUser, DWORD dwWorldID )
#endif	// __LAYER_1015
{
	pUser->Open( dwWorldID );
#ifdef __LAYER_1015
	BOOL bResult = g_WorldMng.AddObj( pUser, dwWorldID, FALSE, nLayer ); 
#else	//__LAYER_1015
	BOOL bResult = g_WorldMng.AddObj( pUser, dwWorldID, FALSE ); 
#endif	// __LAYER_1015
	pUser->SetValid( bResult );

// 	if( pUser->ReallocSerialNumber() == TRUE )
// 	{
// #ifdef __LAYER_1015
// 		g_dpDBClient.SavePlayer( pUser, dwWorldID, pUser->GetPos(), pUser->GetLayer() );
// #else	// __LAYER_1015
// 		g_dpDBClient.SavePlayer( pUser, dwWorldID, pUser->GetPos() );
// #endif	// __LAYER_1015
// 	}
//
//	pUser->RegisterItemSerialNumber();

	//////////////////////////////////////////////////////////////////////////
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		g_pEventArena->OnLogin( pUser );
	}
	//////////////////////////////////////////////////////////////////////////
	return bResult;
}

void FLWSUserManager::DestroyPlayer( FLWSUser* pUser )
{
	//////////////////////////////////////////////////////////////////////////
	g_pTeleportMapUserPointMng->Del( pUser->m_idPlayer );

	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		g_pEventArena->OnLogout( pUser );
	}
	//////////////////////////////////////////////////////////////////////////

	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL ) 	
	{
		SAFE_DELETE( pUser );
		return;
	}

	if( pUser->IsCollecting() )
		pUser->StopCollecting();

	prj.m_MiniGame.DestroyWnd_FiveSystem( pUser );

	if( pUser->HasActivatedEatPet() )
		pUser->InactivateEatPet();

	D3DXVECTOR3 vPos;
	DWORD		dwWorldId;
#ifdef __LAYER_1015
	int nLayer	= nDefaultLayer;
#endif	// __LAYER_1015

	g_xApplyItemEffect->RemoveAllEquipActiveSkill( pUser );

	//////////////////////////////////////////////////////////////////////////
	using namespace nsFlyffPieceDailyPayment;
	g_pFlyffPieceDailyPayment->OnLogOut( pUser->m_idPlayer );
	//////////////////////////////////////////////////////////////////////////

	// 로그 아웃시 길드대전 맵에 있으면 모두 flaris로 이동
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
		PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( WI_WORLD_MADRIGAL, "flaris" );
		if( pRgnElem )
		{
			dwWorldId	= pRgnElem->m_dwWorldId;
			vPos	= pRgnElem->m_vPos;
#ifdef __LAYER_1015
			nLayer	= nRevivalLayer;
#endif	// __LAYER_1015
		}
		else
		{
			dwWorldId	= pUser->GetWorld()->GetID();
			vPos	= pUser->GetPos();
#ifdef __LAYER_1015
			nLayer	= pUser->GetLayer();
#endif	// __LAYER_1015
		}
		if( !pUser->IsDie() )
			pUser->RemoveAllEnemies();
	}
	else
	{
		if( pUser->IsDie() )
		{
			CWorld* pWorld = pUser->GetWorld();
			
			CCommonCtrl* pCtrl	= CreateExpBox( pUser );
			if( pCtrl && pWorld )
			{
				pCtrl->AddItToGlobalId();
				pWorld->ADDOBJ( pCtrl, FALSE, pUser->GetLayer() );
				g_dpDBClient.SendLogExpBox( pUser->m_idPlayer, pCtrl->GetId(), pCtrl->m_nExpBox );
			}
			pUser->Revival( false, false );
			

			PRegionElem pRgnElem	= NULL;
			if( pUser->IsChaotic() )
			{
				if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
					pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
				
				if( NULL == pRgnElem )
					pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
			}
			else
			{
				if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
					pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
				
				if( NULL == pRgnElem )
					pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
			}

			if( pRgnElem )
			{
				dwWorldId	= pRgnElem->m_dwWorldId;
				vPos	= pRgnElem->m_vPos;
#ifdef __LAYER_1015
				nLayer	= nRevivalLayer;
#endif	// __LAYER_1015
			}
			else
			{
				dwWorldId	= pWorld->GetID();
				vPos	= pUser->GetPos();
#ifdef __LAYER_1015
				nLayer	= pUser->GetLayer();
#endif	// __LAYER_1015
			}
		}
		else 
		{
			dwWorldId	= pWorld->GetID();
			vPos	= pUser->GetPos();
#ifdef __LAYER_1015
			nLayer	= pUser->GetLayer();
#endif	// __LAYER_1015
			if( pUser->IsFly() )	// 비행중이었으면 스탑시킴.
				pUser->SendActMsg( OBJMSG_STOP );

			pUser->RemoveAllEnemies();
		}
	}

	CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
	if( pChatting )
	{
		FLWSUser * pUserBuf;
		for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
		{
			pUserBuf = (FLWSUser*)prj.GetUserByID( pChatting->m_idMember[i] );
			if( ::IsValidObj( pUserBuf ) )
			{
				// 채팅에서 나감
				pUserBuf->AddRemoveChatting( pUser->m_idPlayer );
			}
		}
		pChatting->RemoveChattingMember( pUser->m_idPlayer );
		pUser->m_idChatting = 0;
	}	

	// 비밀의 방 - 길마이면 패배 처리하고 아니면 밖으로 쫓아냄
	if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
	{
		//if( CSecretRoomMng::GetInstance()->IsGuildMaster( pUser ) )
		//	CSecretRoomMng::GetInstance()->SetFailGuild( pUser );
		
		dwWorldId = WI_WORLD_MADRIGAL;
		vPos = CSecretRoomMng::GetInstance()->GetRevivalPos( pUser );
#ifdef __LAYER_1015
		nLayer	= nRevivalLayer;
#endif	// __LAYER_1015
	}
	
	// 사냥터용 비밀의 방안에 있으면...
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_DUNGEON_SECRET_0 )
	{
#ifdef __LAYER_1015
		nLayer	= nDefaultLayer;
#endif	// __LAYER_1015
		if( pUser->m_idMarkingWorld == WI_WORLD_MADRIGAL )
		{
			dwWorldId = pUser->m_idMarkingWorld;
			vPos = pUser->m_vMarkingPos;
		}
		else
		{
			dwWorldId = WI_WORLD_MADRIGAL;
			vPos = D3DXVECTOR3( 6968, 100, 3328 );
		}
	}

	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
	{
		dwWorldId = WI_WORLD_MADRIGAL;
		vPos = D3DXVECTOR3( 6968, 100, 3328 );
#ifdef __LAYER_1015
		nLayer	= nDefaultLayer;
#endif	// __LAYER_1015
	}
	CHousingMng::GetInstance()->DestroyHousing( pUser->m_idPlayer );

	// 접속종료시 탈락 처리로 변경 chipi_090317
	CRainbowRaceMng::GetInstance()->SetDropOut( pUser->m_idPlayer );

	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pUser->GetWorld()->GetID() ) )
	{
		//////////////////////////////////////////////////////////////////////////
		// BEGIN110502 콜로세움에서 나갔다면 진보스확률 버프 삭제
		if( CInstanceDungeonHelper::GetInstance()->GetDungeonKind( pUser ) == IDKIND_COLOSSEUM )
		{
			pUser->RemoveIk3Buffs( IK3_COLOSSEUM_BOSS_RATE );
		}
		// END110502
		//////////////////////////////////////////////////////////////////////////

		//CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		CInstanceDungeonHelper::GetInstance()->LeaveDungeon( pUser, pUser->GetWorld()->GetID() );	//@@@@@@@@@@@@@@
		
		//CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		if( pUser->m_idMarkingWorld == WI_WORLD_MADRIGAL )
		{
			dwWorldId = pUser->m_idMarkingWorld;
			vPos = pUser->m_vMarkingPos;
		}
		else
		{
			dwWorldId = WI_WORLD_MADRIGAL;
			vPos = D3DXVECTOR3( 6968, 100, 3328 );
		}
		nLayer = nDefaultLayer;
	}

	if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10469, II_SYS_SYS_SCR_RETURN ) ) == FALSE )
	{
		pUser->m_dwReturnWorldID	= WI_WORLD_MADRIGAL;
		pUser->m_vReturnPos			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

#ifdef __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, vPos, nLayer, TRUE );
#else	// __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, vPos, TRUE );
#endif	// __LAYER_1015

	pUser->UnregisterItemSerialNumber();

	g_dpDBClient.SendLogConnect( pUser );
	CPCBang::GetInstance()->DestroyPCBangPlayer( pUser->m_idPlayer );
//	GuildHouseMng->GoOutGuildHouse( pUser );
	GuildHouseMng->CheckDestroyGuildHouse( pUser );

	g_dpDBClient.CalluspXXXMultiServer( 0, pUser );

	
	//////////////////////////////////////////////////////////////////////////
	pUser->ResetCheckClientReq();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	const CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty != NULL )
	{
		g_PartyMng.CheckRemoveMemberPartyEffect( pUser->m_idPlayer, pParty->m_uPartyId );
	}
	//////////////////////////////////////////////////////////////////////////


	//if( pWorld->DoNotAdd( pUser ) )		// pUser가 m_apAddObjs중에 있으면 LINKMAP에는 없다. 
	if( pUser->m_nAddObjsArrayIndex >= 0 )
	{
		pWorld->removeObjectAtProcessArray( pUser );
		SAFE_DELETE( pUser );			// 메모리에서 삭제 
	}
	else
	{
		pUser->Delete();				// 링크맵에서도 삭제하게 한다.
	}
	if( pUser )
	{
		if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		{
			if( g_GuildCombatMng.m_nGCState == CGuildCombat::MAINTENANCE_STATE || g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_WAR_STATE )
				g_GuildCombatMng.OutWar( pUser, NULL, TRUE );
		}

		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		{
			if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) && pUser->m_nGuildCombatState == 1 )
			{
				int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
				if( nIndex != NULL_ID )
				{
					int nStageId = g_GuildCombat1to1Mng.m_vecTenderGuild.at( nIndex ).nStageId;
					if( nStageId != NULL_ID )
						g_GuildCombat1to1Mng.m_vecGuilCombat1to1.at( nStageId ).GuildCombat1to1WarResultCheck( TRUE, pUser );
				}
			}
		}
	}
}
/*
void FLWSUserManager::Notify( void )
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		pUser->Notify();
	}
}
*/

void FLWSUserManager::ModifyMode( DWORD dwMode )
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->m_dwAuthorization < AUTH_GAMEMASTER )
		{
			pUser->SetMode( dwMode );
			AddModifyMode( pUser );
		}
	}
}

// void FLWSUserManager::AddEventMessage( CCtrl* pCtrl, const TCHAR* szChat, int nFlag, DWORD dwItemId )
// {
// 	CAr ar;
// 	
// 	ar << GETID( pCtrl ) << SNAPSHOTTYPE_EVENTMESSAGE;
// 	ar.WriteString( szChat );
// 	ar << nFlag;
// 	ar << dwItemId;
// 	
// 	GETBLOCK( ar, lpBuf, nBufSize );
// 	
// 	FOR_VISIBILITYRANGE( pCtrl )
// 	USERPTR->AddBlock( lpBuf, nBufSize );
// 	NEXT_VISIBILITYRANGE( pCtrl )
// }

void	FLWSUserManager::AddDlgEmoticon( CCtrl* pCtrl, int nIdx )
{
	CHAR szString[32] = "!";
	CHAR szNum[32] = { 0, };
	FLIntToString( nIdx, szNum, _countof( szNum ), 10 );
	FLStrcat( szString, _countof( szString ), szNum );
	AddChat( pCtrl, szString );
}

void FLWSUserManager::AddStartCollecting( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;

	ar << GETID( pUser ) << SNAPSHOTTYPE_START_COLLECTING;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	{
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddStopCollecting( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;

	ar << GETID( pUser ) << SNAPSHOTTYPE_STOP_COLLECTING;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	{
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddChat( CCtrl* pCtrl, const TCHAR* szChat )
{
	CAr ar;

	ar << GETID( pCtrl ) << SNAPSHOTTYPE_CHAT;
	ar.WriteString( szChat );
	
	GETBLOCK( ar, lpBuf, nBufSize );

	BOOL bCheck = FALSE;
	D3DXVECTOR3 vPos = D3DXVECTOR3( 0, 0, 0 );
	if( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() 
		&& ( (FLWSUser*)pCtrl )->GetWorld() && ( (FLWSUser*)pCtrl )->GetWorld()->GetID() == WI_WORLD_QUIZ )
	{
		bCheck = TRUE;
		vPos = ( (FLWSUser*)pCtrl )->GetPos();
	}

	FOR_VISIBILITYRANGE( pCtrl )
	{
		if( bCheck )
		{
			D3DXVECTOR3 vDist = vPos - USERPTR->GetPos();
			float fDistSq = D3DXVec3LengthSq( &vDist );
			if( fDistSq <= 10.0f )
				USERPTR->AddBlock( lpBuf, nBufSize );
		}
		else
			USERPTR->AddBlock( lpBuf, nBufSize );
		
		if( USERPTR->m_idSnoop > 0 && pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() )
			g_DPCoreClient.SendChat( ( (CMover*)pCtrl )->m_idPlayer, USERPTR->m_idPlayer, szChat );
	}
	NEXT_VISIBILITYRANGE( pCtrl )
}

void FLWSUserManager::AddDefinedText( CMover* pMover, int dwText, LPCSTR lpszFormat, ... )
{
	if( pMover == NULL )
	{
		return;
	}

	TCHAR szBuffer[1024];

	// TODO 유저에게 잘라서 보내도 되는지 좀더 생각해보자
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = FLVSPrintf( szBuffer, _countof(szBuffer), lpszFormat, args );
	va_end( args );
	
	if( nBuf <= 0 )
		return;

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_DEFINEDTEXT;
	ar << dwText;
	ar.WriteString( szBuffer );
	
	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}
	
	FOR_VISIBILITYRANGE( pMover )
	{
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	}
	NEXT_VISIBILITYRANGE( pMover )		
}
void FLWSUserManager::AddShipActMsg( CMover* pMover, CShip *pShip, DWORD dwMsg, int nParam1, int nParam2 )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SHIP_ACTMSG;
	ar << dwMsg << nParam1 << nParam2 << pShip->GetId();
	
	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMotion( CMover* pMover, DWORD dwMsg )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOTION;
	ar << dwMsg;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddRemoveAllSkillInfluence( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_REMOVEALLSKILLINFULENCE;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}
	
	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddWorldShout( const TCHAR* szName, const TCHAR* szShout, D3DXVECTOR3 vPos, CWorld* pWorld )
{
	if( pWorld )
	{
		CAr arBlock;
		arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
		arBlock << NULL_ID;
		arBlock.WriteString( szName );
		arBlock.WriteString( szShout );
		arBlock << (DWORD)0xffff99cc;
		GETBLOCK( arBlock, lpBlock, uBlockSize );
		
		AddBlock( pWorld, vPos, 0xff, lpBlock, uBlockSize );
	}		
}

void FLWSUserManager::AddDamage( CMover* pMover, OBJID objidAttacker, DWORD dwHit, DWORD dwAtkFlags, DWORD dwFakeDamage )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_DAMAGE;
	ar << objidAttacker << dwHit;
	ar << dwAtkFlags;
	ar << dwFakeDamage;

	if( dwAtkFlags & AF_FLYING )
	{
		ar << pMover->GetPos();
		ar << pMover->GetAngle();
	}

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddDisguise( CMover* pMover, DWORD dwMoverIdx )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_DISGUISE << dwMoverIdx;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}
void FLWSUserManager::AddNoDisguise( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_NODISGUISE;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetState( CMover* pMover, BYTE statDice )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSTATE;
	pMover->m_Stat.Serialize( ar );
	
	GETBLOCK( ar, lpBuf, nBufSize );
	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
	
	if( pMover->IsPlayer() == TRUE )
	{
		ar << statDice;
		GETBLOCK( ar, lpBuf, nBufSize );
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}
}

void FLWSUserManager::AddSetStateLevel( CMover* pMover, CHAR chID, long nValue )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_STAT_LEVEL;
	ar << chID << nValue << pMover->m_Stat.GetRemainStatPoint();
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMoverDeath( CMover* pMover, CMover* pAttacker, DWORD dwMsg )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERDEATH;
	ar << GETID( pAttacker ) << dwMsg;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddUseSkill( CMover* pMover, DWORD dwSkill, DWORD dwLevel, OBJID objid, int nUseType, int nCastingTime )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_USESKILL;
	ar << dwSkill << dwLevel;
	ar << objid << nUseType << nCastingTime;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddDoEquip( CMover* pMover, int nPart, DWORD dwItemObjID, const EQUIP_INFO & rEquipInfo, BYTE fEquip )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	DWORD	idGuild = 0;

	ar << GETID( pMover ) << SNAPSHOTTYPE_DOEQUIP;

	ar  << dwItemObjID << idGuild << fEquip;
	ar.Write( (void*)&rEquipInfo, sizeof(EQUIP_INFO) );

	ar << nPart;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddDoEquip( CMover* pMover, int nPart, FLItemElem *pItemElem, BYTE fEquip )
{
	EQUIP_INFO equipInfo;
	equipInfo.dwItemID				= pItemElem->m_dwItemId;
	equipInfo.nOption				= pItemElem->GetAttrOption();
	equipInfo.byFlag				= pItemElem->m_byFlag;

	equipInfo.dwLooksChangeItemID	= pItemElem->GetLooksChangeItemID();
	equipInfo.bIsCombined			= pItemElem->IsSetCombinedOption();

	AddDoEquip( pMover, nPart, pItemElem->m_dwObjId, equipInfo, fEquip );
}


void FLWSUserManager::AddVendor( CMover* pVendor )
{
	if( pVendor == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pVendor ) << SNAPSHOTTYPE_VENDOR;

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		pVendor->m_ShopInventory[i]->Serialize( ar );

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pVendor )
		if( USERPTR->m_vtInfo.GetOther() == pVendor )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pVendor )
}

void FLWSUserManager::AddUpdateVendor( CMover* pVendor, CHAR cTab, DWORD dwItemObjID, int nNum )
{
	if( pVendor == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pVendor ) << SNAPSHOTTYPE_UPDATE_VENDOR;

	ar << cTab << dwItemObjID << nNum;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pVendor )
		if( USERPTR->m_vtInfo.GetOther() == pVendor )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pVendor )
}

void FLWSUserManager::AddSetDestParam( CMover* pMover, int nDstParameter, int nAdjParameterValue, int nChgParameterValue )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_SETDESTPARAM;

	ar << nDstParameter << nAdjParameterValue << nChgParameterValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddResetDestParam( CMover* pMover, int nDstParameter, int nAdjParameterValue )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_RESETDESTPARAM;

	ar << nDstParameter << nAdjParameterValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddResetDestParamSync( CMover* pMover, int nDstParameter,int nAdjParameterValue, int nParameterValue )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_RESETDESTPARAM_SYNC;

	ar << nDstParameter << nAdjParameterValue << nParameterValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetPointParam( CMover* pMover, int nDstParameter, int nValue )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_SETPOINTPARAM;

	ar << nDstParameter << nValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetPos( CMover* pMover, const D3DXVECTOR3 & vPos )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_SETPOS;
	ar << vPos;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}


void FLWSUserManager::AddSetLevel( CMover* pMover, WORD wLevel )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_SETLEVEL;
	ar << wLevel;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetFlightLevel( CMover* pMover, int nFlightLv )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETFLIGHTLEVEL;
	ar << (WORD)nFlightLv;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}


void FLWSUserManager::AddSetSkillLevel( CMover* pMover, DWORD dwSkill, DWORD dwLevel )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSKILLLEVEL;
	ar << dwSkill << dwLevel;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}


#ifdef __IAOBJ0622
void FLWSUserManager::AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward, OBJID objidIAObj )
#else	// __IAOBJ0622
void FLWSUserManager::AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward )
#endif	// __IAOBJ0622
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_DESTPOS;
	ar << vPos << fForward;

#ifdef __IAOBJ0622
	ar << objidIAObj;
#endif	// __IAOBJ0622

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetMovePattern( CMover* pMover, int nPattern, const D3DXVECTOR3 &vPos, FLOAT fAngle, FLOAT fAngleX )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETMOVEPATTERN;
	ar << nPattern << vPos << fAngle << fAngleX;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMoverSetDestObj( CMover* pMover, OBJID objid, float fRange, BOOL fTransferToMe )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERSETDESTOBJ;
	ar << objid << fRange;

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE && fTransferToMe == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMeleeAttack( CMover* pMover, OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3 )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MELEE_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMeleeAttack2( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MELEE_ATTACK2;
	ar << dwAtkMsg << objid << nParam2 << nParam3;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
		NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMagicAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int nMagicPower, int idSfxHit )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MAGIC_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3 << nMagicPower << idSfxHit;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddRangeAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int idSfxHit )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_RANGE_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3 << idSfxHit;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddAttackSP( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SP_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3 ;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
		NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddMoverBehavior( CMover* pMover, BOOL bTransferToMe )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERBEHAVIOR;
	pMover->AutoSynchronizer()->Serialize( ar );

	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE && bTransferToMe == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

// x, y, z ; 절대좌표에 오브젝트 생성.
void FLWSUserManager::AddCreateSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x, float y, float z, BOOL bFlag )
{
	if( pCtrl == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_CREATESFXOBJ;

	ar << dwSfxObj << x << y << z;
	ar << bFlag;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

void FLWSUserManager::AddRemoveSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x, float y, float z, BOOL bFlag )
{
	if( pCtrl == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_REMOVESFXOBJ;

	ar << dwSfxObj << x << y << z;
	ar << bFlag;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}


void FLWSUserManager::AddMotionArrive( CMover* pMover, OBJMSG objmsg )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOTION_ARRIVE;
	
	ar << objmsg;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddPKPink( CMover* pMover, BYTE byPink )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PK_RELATION;
	ar << PK_PINK;
	ar << byPink;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}
void FLWSUserManager::AddPKPropensity( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PK_RELATION;
	ar << PK_PROPENSITY;
	ar << pMover->GetPKPropensity();

	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddWorldCreateSfxObj( DWORD dwSfxObj, float x, float y, float z, BOOL bFlag, DWORD dwWorldId )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_CREATESFXOBJ << dwSfxObj << x << y << z << bFlag ;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
	AddBlockNoLock( lpBlock, uBlockSize, pWorld );
}

void	FLWSUserManager::AddCreateSfxAllow( CMover *pMover, DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_CREATESFXALLOW;
	ar << dwSfxObjArrow << dwSfxObjHit << vPosDest.x << vPosDest.y << vPosDest.z << idTarget;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddNearSetChangeJob( CMover* pMover, int nJob, LPSKILL /*lpSkill*/ )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_NEAR_JOB_SKILL;
	ar << nJob;

	if( pMover->IsLegendHero() == TRUE )
	{
		ar.Write( (void*)&pMover->m_aJobSkill[ MAX_EXPERT_SKILL ], sizeof(SKILL) *  ( MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL ) );
	}
	else if( pMover->IsHero() )
	{
		ar.Write( (void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL ], sizeof(SKILL) *  ( MAX_PRO_SKILL + MAX_MASTER_SKILL ) );
	}
	else if( pMover->IsMaster() )
	{
		ar.Write( (void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL ], sizeof(SKILL) *  ( MAX_PRO_SKILL + MAX_MASTER_SKILL ) );
	}
	else
	{
		ar.Write( (void*)&pMover->m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) *  ( MAX_EXPERT_SKILL ) );
	}

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddModifyMode( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_MODIFYMODE;
	ar << pUser->m_dwMode;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddStateMode( FLWSUser* pUser, BYTE nFlag )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_STATEMODE;
	ar << pUser->m_dwStateMode;
	ar << nFlag;
	if( nFlag == STATEMODE_BASEMOTION_ON )
	{
		PT_ITEM_SPEC pItemProp = NULL;
		if( pUser->m_bItemFind )
		{
			FLItemElem* pItemElem = ((CMover*)pUser)->m_Inventory.GetAtId( pUser->m_dwUseItemId );
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();
			}
		}
		else
		{
			pItemProp = g_xSpecManager->GetSpecItem( pUser->m_dwUseItemId );
		}

		if( pItemProp )
		{
			ar << pItemProp->dwID;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "나오면 안됨.. 시전" ) );
			return;
		}
	}
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddSetFame( CMover* pMover, int nFame )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETFAME;
	ar << nFame;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetLocalEvent( short id, BYTE nState )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_SET_LOCAL_EVENT;
	ar << id << nState;
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddGameRate( FLOAT fRate, BYTE nFlag )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GAMERATE;
	ar << fRate;
	ar << nFlag;
	GETBLOCK( ar, lpBuf, uBufSize );
	
	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddChangeFace( u_long uidPlayer, DWORD dwFace )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_CHANGEFACE;
	ar << uidPlayer << dwFace;
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddSchoolReport( PSCHOOL_ENTRY pSchool, short nElapse )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_SCHOOL_REPORT;
	ar.Write( pSchool, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
	ar << nElapse;

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddSexChange( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SEX_CHANGE;
	ar << (u_short)pMover->GetIndex();
	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddCreateGuild( u_long idPlayer, const char* lpszPlayer, u_long idGuild, const char* szGuild )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_CREATE_GUILD;
	ar << idPlayer << idGuild;
	ar.WriteString( lpszPlayer? lpszPlayer: "" );
	ar.WriteString( szGuild );
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddDestroyGuild( const char* lpszPlayer, u_long idGuild )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_DESTROY_GUILD;
	ar.WriteString( lpszPlayer? lpszPlayer: "" );
	ar << idGuild;
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddSetGuild( FLWSUser* pUser, u_long idGuild )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_GUILD;
	ar << idGuild;
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddSetWar( FLWSUser* pUser, u_long idWar )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_WAR;
	ar << idWar;
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

//모든 유저에게 알리는 버젼 
void FLWSUserManager::AddSetLogo( u_long idGuild, DWORD dwLogo )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILD_LOGO;
	ar << idGuild << dwLogo;

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddGetItemElem( FLWSUser* pUser, FLItemElem* pItemElem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;

	ar << GETID( pUser ) << SNAPSHOTTYPE_GETITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize( ar );
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	if ( USERPTR != pUser && USERPTR->m_bGuildBank == TRUE && USERPTR->m_idGuild == pUser->m_idGuild)
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddPutItemElem( FLWSUser* pUser, FLItemElem* pItemElem )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pUser ) << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize( ar );
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		if (  USERPTR != pUser && USERPTR->m_bGuildBank == TRUE && USERPTR->m_idGuild == pUser->m_idGuild)
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddPutItemElem( u_long uidGuild, FLItemElem* pItemElem )
{
	CAr ar;
	
	ar << NULL_ID << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize( ar );
	GETBLOCK( ar, lpBuf, nBufSize );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUsertmp = it->second;
		if( pUsertmp->IsValid() == FALSE )
			continue;
		
		if( pUsertmp->m_idGuild == uidGuild )
			pUsertmp->AddBlock( lpBuf, nBufSize );
	}
}

void FLWSUserManager::AddPVendorOpen( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_PVENDOR_OPEN;
	ar.WriteString( pUser->m_vtInfo.GetTitle() );
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddPVendorClose( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_PVENDOR_CLOSE;
	ar << (BYTE)1;
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddPVendorItemNum( FLWSUser* pUser, BYTE byNth, int nVend, const char* sBuyer )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_PVENDOR_ITEM_NUM;
	ar << byNth << nVend;
	ar.WriteString( sBuyer );
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( USERPTR == pUser || USERPTR->m_vtInfo.GetOther() == pUser )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddSetHair( FLWSUser* pUser, BYTE nHair, BYTE r, BYTE g, BYTE b )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_HAIR;
	ar << nHair << r << g << b;
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddHdr( CCtrl* pCtrl, int nPacketType )
{
	if( pCtrl == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pCtrl ) << nPacketType;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

void FLWSUserManager::AddBlock( LPBYTE lpBlock, u_long uBlockSize )
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() )
			pUser->AddBlock( lpBlock, uBlockSize );
	}

}

void FLWSUserManager::AddBlock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld )
{
	if( !pWorld )
		return;

	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() == pWorld )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUserManager::AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize )
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUserManager::AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld )
{
	if( !pWorld )
		return;

	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() == pWorld )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUserManager::AddBlock( CWorld* pWorld, const D3DXVECTOR3 & vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize )
{
	if( !pWorld )
		return;

	float d	= (float)( nRange * nRange );
	D3DXVECTOR3 vtmp;

	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		vtmp = vPos - pUser->GetPos();
		if( pUser->GetWorld() == pWorld && D3DXVec3LengthSq( &vtmp ) < d )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUserManager::AddGameSetting( void )
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		pUser->AddGameSetting();
	}
}

void FLWSUserManager::AddShout( FLWSUser* pUserSrc, int nRange, LPBYTE lpBlock, u_long uBlockSize )
{
	float fRange = (float)( nRange * nRange );
	D3DXVECTOR3 v;
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		if( pUser->GetWorld() == NULL )
			continue;

		if( nRange > 0  )
		{
			v = pUserSrc->GetPos() - pUser->GetPos();
			if(	pUser->GetWorld() != pUserSrc->GetWorld() || pUser->GetLayer() != pUserSrc->GetLayer()
				|| D3DXVec3LengthSq( &v ) > fRange
				)
				continue;
		}

		pUser->AddBlock( lpBlock, uBlockSize );
	}
}


void FLWSUserManager::AddWorldMsg( const CRect* pRect, LPCTSTR lpszString )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_WORLDMSG;
	arBlock.WriteString( lpszString );
	GETBLOCK( arBlock, lpBlock, uBlockSize );

	D3DXVECTOR3 vtmp;
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		vtmp = pUser->GetPos();
		if( pUser->GetWorld() )
		{
			if( pRect == NULL )
				pUser->AddBlock( lpBlock, uBlockSize );
			else if( pRect->PtInRect( CPoint( (int) vtmp.x, (int) vtmp.y ) ) )
				pUser->AddBlock( lpBlock, uBlockSize );
		}
	}
}


// pMover를 fScalePercent로 줄인다.
// ex)
// pMover가 클라에서 2.0f 2.0f 2.0f 스케일 상태였는데 fScalePercent가 0.5가 넘어갔다면
// 2.0 * 0.5 = 1.0f     1.0, 1.0, 1.0으로 스케일 조정된다
void FLWSUserManager::AddSetScale( CMover* pMover, float fScalePercent )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_SETSCALE;	
	ar << fScalePercent;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

// pCenter : 이놈을 중심으로 한 유저들에게 보낸다
// pTarget : SkillState를 적용할 실제 대상무버.
void FLWSUserManager::AddSetSkillState( CMover* pCenter, CMover *pTarget, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime  )
{
	if( pCenter == NULL || pTarget == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pCenter );
	ar << SNAPSHOTTYPE_SETSKILLSTATE;	
	ar << pTarget->GetId() << wType << wID;

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( wID );
	if( wType == BUFF_ITEM && pItemProp != NULL && pItemProp->IsAbsoluteTime() == TRUE )
	{
		time_t t	= (time_t)dwLevel - time_null();
		ar << t;
	}
	else
	{
		ar << dwLevel;
	}

	ar << dwTime;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pCenter->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pCenter )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pCenter )
		if( pCenter->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pCenter )
}

#ifdef __S1108_BACK_END_SYSTEM
void FLWSUserManager::AddMonsterProp()
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_MONSTERPROP;

	ar << prj.m_nAddMonsterPropSize;
	for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
	{
		ar.WriteString( prj.m_aAddProp[ i ].szMonsterName );
		ar << prj.m_aAddProp[ i ].nHitPoint;
		ar << prj.m_aAddProp[ i ].nAttackPower;
		ar << prj.m_aAddProp[ i ].nDefence;
		ar << prj.m_aAddProp[ i ].nExp;
		ar << prj.m_aAddProp[ i ].nItemDrop;
		ar << prj.m_aAddProp[ i ].nPenya;
	}
	
	ar << prj.m_nRemoveMonsterPropSize;
	for( int i = 0 ; i < prj.m_nRemoveMonsterPropSize ; ++i )
	{
		ar.WriteString( prj.m_aRemoveProp[i] );
	}

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddGMChat( int nSize )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GMCHAT;
	ar << nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		ar.WriteString( prj.m_chGMChat[i] );
	}

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}
#endif // __S1108_BACK_END_SYSTEM

void FLWSUserManager::AddGuildCombatUserState( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_GUILDCOMBAT;
	ar << GC_USERSTATE;
	ar << pMover->GetId() << pMover->m_nGuildCombatState;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddGCWinGuild( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT << GC_WINGUILD;
	ar << g_GuildCombatMng.m_nGuildCombatIndex << g_GuildCombatMng.m_uWinGuildId << g_GuildCombatMng.m_nWinGuildCount;
	
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlockNoLock( lpBuf, uBufSize );	// all
}
void FLWSUserManager::AddGCBestPlayer( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT << GC_BESTPLAYER;
	ar << g_GuildCombatMng.m_uBestPlayer;
	
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlockNoLock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddGuildCombatState( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	ar << GC_GCSTATE;
	ar << g_GuildCombatMng.m_nState << g_GuildCombatMng.m_nGCState;
	
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlockNoLock( lpBuf, uBufSize );	// all
}

void FLWSUserManager::AddGuildCombatNextTimeWorld( DWORD dwTime, DWORD dwState )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_NEXTTIMESTATE;
	arBlock << dwTime << dwState;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		pUser->AddBlock( lpBlock, uBlockSize );
	}
}
void FLWSUserManager::AddGuildCombatEnterTime( DWORD dwTime )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_ENTERTIME;
	arBlock << dwTime ;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUserManager::AddGCWarPlayerlist( u_long uidGuild, FLWSUser* pSendUser )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_WARPLAYERLIST;
	g_GuildCombatMng.SerializeGCWarPlayerList( arBlock );
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	if( pSendUser == NULL )
	{
		std::map<DWORD, FLWSUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			FLWSUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( uidGuild )
			{
				if( uidGuild == pUser->m_idGuild )
				{
					pUser->AddBlock( lpBlock, uBlockSize );
				}			
			}
			else if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			{
				pUser->AddBlock( lpBlock, uBlockSize );
			}
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}
void FLWSUserManager::AddGCLogWorld( void )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_LOG;
	arBlock << (u_long)g_GuildCombatMng.m_vecGCGetPoint.size();
	for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_vecGCGetPoint.size() ) ; ++veci )
	{
		CGuildCombat::__GCGETPOINT GCGetPoint = g_GuildCombatMng.m_vecGCGetPoint.at( veci );
		arBlock << GCGetPoint.uidGuildAttack;
		arBlock << GCGetPoint.uidGuildDefence;
		arBlock << GCGetPoint.uidPlayerAttack;
		arBlock << GCGetPoint.uidPlayerDefence;
		arBlock << GCGetPoint.nPoint;
		arBlock << GCGetPoint.bKillDiffernceGuild;
		arBlock << GCGetPoint.bMaster;
		arBlock << GCGetPoint.bDefender;
		arBlock << GCGetPoint.bLastLife;
	}
	GETBLOCK( arBlock, lpBlock, uBlockSize );

	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}
void FLWSUserManager::AddGCLogRealTimeWorld( CGuildCombat::__GCGETPOINT GCGetPoint )
{
	LPCSTR szAttacker, szDefender;
	szAttacker	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCGetPoint.uidPlayerAttack );
	szDefender	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCGetPoint.uidPlayerDefence );
	if( szAttacker == NULL || szDefender == NULL )
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_LOG_REALTIME;
	arBlock << GCGetPoint.uidGuildAttack;
	arBlock << GCGetPoint.uidGuildDefence;
	arBlock.WriteString( szAttacker );
	arBlock.WriteString( szDefender );
	arBlock << GCGetPoint.uidPlayerAttack;
	arBlock << GCGetPoint.uidPlayerDefence;
	arBlock << GCGetPoint.nPoint;
	arBlock << GCGetPoint.bKillDiffernceGuild;
	arBlock << GCGetPoint.bMaster;
	arBlock << GCGetPoint.bDefender;
	arBlock << GCGetPoint.bLastLife;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

#ifdef __LAYER_1015
void FLWSUserManager::ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, int nLayer )
#else	// __LAYER_1015
void FLWSUserManager::ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ )
#endif	// __LAYER_1015
{
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
	if( pWorld )
	{
		std::map<DWORD, FLWSUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			FLWSUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( pUser->GetWorld() == pWorld )
			{
				D3DXVECTOR3 v3Pos = pUser->GetPos();
				AddCreateSfxObj( (CMover*)pUser, XI_INDEX( 1748, XI_GEN_WEARF ), v3Pos.x, v3Pos.y, v3Pos.z );
				( (CMover*)pUser)->REPLACE( g_uIdofMulti, dwReplaceWorldId, D3DXVECTOR3( fReplaceX, 0.0f, fReplaceZ ), REPLACE_NORMAL, nLayer );
				pUser->m_vtInfo.SetOther( NULL );
				AddCreateSfxObj( (CMover*)pUser, XI_INDEX( 1748, XI_GEN_WEARF ), fReplaceX, v3Pos.y, fReplaceZ );				
			}
		}
	}
}

#ifdef __LAYER_1015
void FLWSUserManager::ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag, int nLayer )
#else	// __LAYER_1015
void FLWSUserManager::ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag )
#endif	// __LAYER_1015
{
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
	if( pWorld )
	{
		for( int i = 0; i < (int)( pWorld->m_dwObjNum ); i++ )
		{
			CObj* pObj	= pWorld->m_apObject[i];
			if( IsValidObj( pObj ) && pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() && ( (CMover*)pObj )->m_idparty == idParty )
			{
				FLWSUser* pUser	= (FLWSUser*)pObj;
				if( bDieFlag )
				{
					if( pUser->IsDie() )
						continue;
				}

				D3DXVECTOR3 vPos	= pUser->GetPos();
				AddCreateSfxObj( pUser, XI_INDEX( 1748, XI_GEN_WEARF ), vPos.x, vPos.y, vPos.z );
				float fNewArea	= fArea * 2.0f;
				fReplaceX += (-fArea) + xRandomF( fNewArea );
				fReplaceZ += (-fArea) + xRandomF( fNewArea );
				pUser->REPLACE( g_uIdofMulti, dwReplaceWorldId, D3DXVECTOR3( fReplaceX, 0.0f, fReplaceZ ), REPLACE_NORMAL, nLayer );
				AddCreateSfxObj( (CMover*)pUser, XI_INDEX( 1748, XI_GEN_WEARF ), fReplaceX, vPos.y, fReplaceZ );
			}
		}
	}
}

void FLWSUserManager::AddWorldMsg( DWORD dwWorldId, LPCTSTR lpszString )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_WORLDMSG;
	arBlock.WriteString( lpszString );
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		if( pUser->GetWorld()->GetID() == dwWorldId )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUserManager::AddGuildMsg( u_long idGuild, LPCSTR lpsz )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
		AddGuildMsg( pGuild, lpsz );
}

void FLWSUserManager::AddGuildMsg( CGuild* pGuild, LPCSTR lpsz )
{
	CGuildMember*	pMember;
	FLWSUser*			pUsertmp;
	
	for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
	i != pGuild->m_mapPMember.end(); ++i )
	{
		pMember		= i->second;
		pUsertmp	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUsertmp ) )
			pUsertmp->AddText( lpsz );
	}
}

void FLWSUserManager::AddGCIsRequest( u_long uidGuild, BOOL bRequest )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_ISREQUEST;
	arBlock << bRequest;

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUsertmp = it->second;
		if( pUsertmp->IsValid() == FALSE )
			continue;
		
		if( pUsertmp->m_idGuild == uidGuild )
			pUsertmp->AddBlock( lpBlock, uBlockSize );
	}
}
void FLWSUserManager::AddGCGuildStatus( u_long uidGuild, FLWSUser* pSendUser )
{
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_GUILDSTATUS;
	
	CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
	{
		// 순서에 맞게 vecPlayerList에 넣기 : 선수 -> 대기자(들어갈수 있음) -> 대기자(못들어감)
		list<CGuildCombat::__JOINPLAYER*> lspPlyaerList;
		lspPlyaerList.clear();

		CGuildCombat::__JOINPLAYER* pJoinPlayer;
		// 대기자(들어갈수 잇음) 넣음
		for( list<CGuildCombat::__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
		{
			pJoinPlayer = *i1;
			lspPlyaerList.push_back( pJoinPlayer );
		}

		// 선수 및 대기자(못들어감) 넣음
		for( int veci = pGCMember->vecGCSelectMember.size()-1 ; veci >= 0 ; --veci )
		{
			pJoinPlayer = pGCMember->vecGCSelectMember.at( veci );
			if( 0 < pJoinPlayer->nlife )	// 선수
			{
				BOOL bFind = FALSE;
				for( list<CGuildCombat::__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
					{
						bFind	= TRUE;
						break;
					}
				}				
				if( bFind == FALSE )
				{
					lspPlyaerList.push_front( pJoinPlayer );
				}
			}
			else // 대기자(못들어감)
			{
				lspPlyaerList.push_back( pJoinPlayer );
			}
		}

		// 보내기
		if( 0 < pGCMember->lspFifo.size() )
			arBlock << g_GuildCombatMng.m_nMaxWarPlayer;
		else
			arBlock << (int)0;
		
		arBlock << (int)lspPlyaerList.size();
		for( list<CGuildCombat::__JOINPLAYER*>::iterator i1 = lspPlyaerList.begin(); i1 != lspPlyaerList.end(); ++i1 )
		{
			pJoinPlayer = *i1;
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nlife;
		}	
	}
	else
	{
		arBlock << (int)0;
		arBlock << (int)0;
	}

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	if( pSendUser == NULL )
	{
		std::map<DWORD, FLWSUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			FLWSUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( pUser->m_idGuild == uidGuild )
			{
				pUser->AddBlock( lpBlock, uBlockSize );
			}
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}
void FLWSUserManager::AddGCGuildPrecedence( FLWSUser* pSendUser )
{
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_GUILDPRECEDENCE;

	arBlock << (int)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for( int gcmi = 0 ; gcmi < (int)( g_GuildCombatMng.m_vecGuildCombatMem.size() ) ; ++gcmi )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem.at( gcmi );
		if( 0 < pGCMember->vecGCSelectMember.size() )
		{			
			arBlock << (BOOL)TRUE; // bSend;
			CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
			if( pGuild )
				arBlock.WriteString( pGuild->m_szGuild );
			else
				arBlock.WriteString( "Not Guild" );
			arBlock << pGCMember->nGuildPoint;
		}
		else
		{
			arBlock << (BOOL)FALSE; // bSend;
		}
	}

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	if( pSendUser == NULL )
	{
		std::map<DWORD, FLWSUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			FLWSUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
				pUser->AddBlock( lpBlock, uBlockSize );
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}
void FLWSUserManager::AddGCPlayerPrecedence( FLWSUser* pSendUser )
{
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		return;
	
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_PLAYERPRECEDENCE;

	arBlock << (int)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for( int gcmi = 0 ; gcmi < (int)( g_GuildCombatMng.m_vecGuildCombatMem.size() ) ; ++gcmi )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem.at( gcmi );
		arBlock << (int)pGCMember->vecGCSelectMember.size();
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember.at( veci );
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nPoint;
		}
	}

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	if( pSendUser == NULL )
	{
		std::map<DWORD, FLWSUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			FLWSUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			{
				pUser->AddBlock( lpBlock, uBlockSize );
			}
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}

void FLWSUser::AddRunScriptFunc( const RunScriptFunc & runScriptFunc )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RUNSCRIPTFUNC;
	m_Snapshot.ar << runScriptFunc.wFuncType;
	switch( runScriptFunc.wFuncType )
	{
		case FUNCTYPE_ADDKEY:
		case FUNCTYPE_ADDANSWER:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal2 );
				m_Snapshot.ar << runScriptFunc.dwVal1;
				m_Snapshot.ar << runScriptFunc.dwVal2;
				break;
			}
		case FUNCTYPE_REMOVEKEY:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				break;
			}
		case FUNCTYPE_SAY:
		case FUNCTYPE_SAYQUEST:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				m_Snapshot.ar << runScriptFunc.dwVal2;
				break;
			}
		case FUNCTYPE_INITSTAT:
		case FUNCTYPE_INITSTR:
		case FUNCTYPE_INITSTA:			
		case FUNCTYPE_INITDEX:
		case FUNCTYPE_INITINT:
			{
				m_Snapshot.ar << runScriptFunc.dwVal1;
				break;
			}
		case FUNCTYPE_SETNAVIGATOR:
			{
				m_Snapshot.ar << runScriptFunc.dwVal1;
				m_Snapshot.ar << runScriptFunc.vPos;
				break;
			}
		case FUNCTYPE_NEWQUEST:
		case FUNCTYPE_CURRQUEST:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal2 );
				m_Snapshot.ar << runScriptFunc.dwVal1;
				m_Snapshot.ar << runScriptFunc.dwVal2;
				break;
			}
		default:
			break;
	}
	
}

void FLWSUser::AddEnterChatting( FLWSUser* pUser )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_ENTERMEMBER;
	m_Snapshot.ar << pUser->m_idPlayer;
	m_Snapshot.ar.WriteString( pUser->GetName() );
	
}

void FLWSUser::AddNewChatting( CChatting* pChatting )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_NEWCHATTING;
	m_Snapshot.ar << pChatting->GetChattingMember();
	for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
	{
		m_Snapshot.ar << pChatting->m_idMember[i];
		if( NULL == CPlayerDataCenter::GetInstance()->GetPlayerString( pChatting->m_idMember[i] ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "MAX_ChattingMember = %d, GetMember = %d" ), pChatting->GetChattingMember(), pChatting->m_idMember[i] );
			m_Snapshot.ar.WriteString( "" );
		}
		else
		{
			m_Snapshot.ar.WriteString( CPlayerDataCenter::GetInstance()->GetPlayerString( pChatting->m_idMember[i] ) );
		}
	}
}

/*
void FLWSUser::AddCreateAngel(BOOL isSuccess, char* createAngel)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ANGEL;
	m_Snapshot.ar << ANGEL_WNDCREATE;
	m_Snapshot.ar << isSuccess;
	m_Snapshot.ar.WriteString( createAngel );
}
*/

void FLWSUser::AddAngelInfo( BOOL bComplete )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ANGEL;
	m_Snapshot.ar << ANGEL_INFO;
	m_Snapshot.ar << m_nAngelExp;
	m_Snapshot.ar << m_nAngelLevel;
	m_Snapshot.ar << bComplete;
}

void FLWSUser::AddRemoveChatting( u_long uidPlayer )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_REMOVEMEMBER;
	m_Snapshot.ar << uidPlayer;
	
}

void FLWSUser::AddDeleteChatting()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_DELETECHATTING;
	
}

void FLWSUser::AddChatting( u_long uidPlayer, char* pszChat )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_CHATTING;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar.WriteString( pszChat );
	
}

void FLWSUser::AddChttingRoomState( BOOL bState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_CHATTINGROOMSTATE;
	m_Snapshot.ar << bState;
	
}

void FLWSUser::AddGameRate( FLOAT fRate, BYTE nFlag )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GAMERATE;
	m_Snapshot.ar << fRate;
	m_Snapshot.ar << nFlag;
	
}

void FLWSUser::AddGameSetting()
{
	if( IsDelete() )	return;

	AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
	AddGameRate( (float)( prj.m_dwVagSP ), GAME_SKILL_VAGSP );
	AddGameRate( (float)( prj.m_dwExpertSP ), GAME_SKILL_EXPERTSP );
	AddGameRate( (float)( prj.m_dwProSP ), GAME_SKILL_PROSP );
	AddGameRate( prj.m_EventLua.GetShopBuyFactor(), GAME_RATE_SHOP_BUY );
	AddGameRate( prj.m_EventLua.GetShopSellFactor(), GAME_RATE_SHOP_SELL );
	if( ((CMover*)this)->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		AddGameRate( prj.m_fItemDropRate, GAME_RATE_ITEMDROP );
		AddGameRate( prj.m_fGoldDropRate, GAME_RATE_GOLDDROP );
		AddGameRate( prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP );
		AddGameRate( prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT );
		
#ifdef __S1108_BACK_END_SYSTEM
		AddGameRate( prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH );
		AddGameRate( prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT );
		AddGameRate( prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE );
#endif // __S1108_BACK_END_SYSTEM
	}	
}

#ifdef __S1108_BACK_END_SYSTEM
void FLWSUser::AddMonsterProp()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MONSTERPROP;

	m_Snapshot.ar << prj.m_nMonsterPropSize;
	for( int i = 0 ; i < prj.m_nMonsterPropSize ; ++i )
	{
		m_Snapshot.ar.WriteString( prj.m_aMonsterProp[ i ].szMonsterName );
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nHitPoint;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nAttackPower;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nDefence;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nExp;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nItemDrop;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nPenya;
	}
	
	m_Snapshot.ar << prj.m_nRemoveMonsterPropSize;
	for( int i = 0 ; i < prj.m_nRemoveMonsterPropSize ; ++i )
	{
		m_Snapshot.ar.WriteString( prj.m_aRemoveProp[i] );
	}

	
}
#endif // __S1108_BACK_END_SYSTEM

void FLWSUser::AddInitSkill()
{

	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_INITSKILLPOINT;
	m_Snapshot.ar << m_nSkillPoint;
}


void FLWSUser::AddDoUseSkillPoint( SKILL aJobSkill[], int nSkillPoint )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DOUSESKILLPOINT;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &aJobSkill[ i ];
		
		m_Snapshot.ar << lpSkill->dwSkill << lpSkill->dwLevel;
	}
	m_Snapshot.ar << nSkillPoint;
}

void FLWSUser::AddWantedInfo( const D3DXVECTOR3& vPos, BYTE byOnline, DWORD dwWorldID, LPCTSTR lpszWorld )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_WANTED_INFO;
	m_Snapshot.ar << vPos;
	m_Snapshot.ar << byOnline;
	m_Snapshot.ar << dwWorldID;		
	m_Snapshot.ar.WriteString( lpszWorld );
	
}

void FLWSUserManager::AddCommonPlace( CCtrl* pCtrl, BYTE nType )
{
	if( pCtrl == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_COMMONPlACE;
	ar << nType;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

void FLWSUser::AddReturnScroll()
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RETURNSCORLL;
	
}

void FLWSUserManager::OutputStatistics( void )
{
	static const TCHAR	LOG_STATISTICS[]	= _T( "statistices" );
	
	
	int cb	= 0;
	int nTotal	= 0;

	//mem_set( acbUser, 0, sizeof(acbUser) );
	int	acbUser[MAX_LEGEND_LEVEL] = { 0 };

	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetLevel() >= 1 && pUser->GetLevel() <= MAX_LEGEND_LEVEL )
		{
			acbUser[pUser->GetLevel() - 1]++;
			cb++;
			nTotal	+= pUser->GetLevel();
		}
	}

	static char lpOutputString[4096];
	*lpOutputString	= '\0';
	for( int i = 0; i < MAX_LEGEND_LEVEL; i++ )
	{
		if( acbUser[i] == 0 )
			continue;
		char lpString[32]	= { 0, };
		FLSPrintf( lpString, _countof( lpString ), "%d\t%d\n", i+1, acbUser[i] );
		FLStrcat( lpOutputString, _countof( lpOutputString ), lpString );
	}

	if( cb > 0 )
	{
		char lpOverview[32]	= { 0, };
		FLSPrintf( lpOverview, _countof( lpOverview ), "U=%d, A=%d", cb, nTotal / cb );
		FLStrcat( lpOutputString, _countof( lpOutputString ), lpOverview );
	}

	CTime time	= CTime::GetCurrentTime();
	FLINFO_LOG( LOG_STATISTICS, lpOutputString );
}

void FLWSUser::AddRemoveMail( u_long nMail, int nType )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEMAIL;
	m_Snapshot.ar << nMail << nType;
	
}

void FLWSUser::AddMailBox( CMailBox* pMailBox )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYMAILBOX;
	pMailBox->Serialize( m_Snapshot.ar );
	
}

void FLWSUser::SendCheckMailBoxReq( BOOL bCheckTransMailBox )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYMAILBOX_REQ;
	m_Snapshot.ar << bCheckTransMailBox;
}


void FLWSUser::CheckTransMailBox( BOOL bCheckTransMailBox )
{
	m_bCheckTransMailBox = bCheckTransMailBox;
}

BOOL FLWSUser::GetCheckTransMailBox()
{
	return m_bCheckTransMailBox;
}

bool FLWSUser::CheckClientReq()
{
	DWORD dwTick = GetTickCount();
	if( dwTick >= m_dwTickFromClient + CHECK_TICK_FROM_CLIENT )
	{
		m_dwTickFromClient = dwTick;
		++m_nCountFromClient;
		return true;
	}
	return false;
}

void FLWSUser::ResetCheckClientReq()
{
	m_dwTickFromClient = 0;
	m_nCountFromClient = 0;
}

int FLWSUser::GetCountClientReq()
{
	return m_nCountFromClient;
}

// 신청 윈도우 띄움
void FLWSUser::AddGCWindow( __int64 nPrizePenya, DWORD dwRequstPenya, DWORD dwMinRequestPenya )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_IN_WINDOW;
	m_Snapshot.ar << nPrizePenya << dwRequstPenya << dwMinRequestPenya;
	
}

void FLWSUser::AddGCRequestStatus( __int64 nPrizePenya, std::vector<CGuildCombat::__REQUESTGUILD> vecRequestStatus )
{
	DWORD dwRequstPenya = 0;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_REQUEST_STATUS;
	CTime tNextTime = g_GuildCombatMng.GetNextGuildCobmatTime();
	time_t d = (time_t)( tNextTime.GetTime() - time_null() );
	m_Snapshot.ar << d;
	m_Snapshot.ar << nPrizePenya;
	int nSize = vecRequestStatus.size();
	m_Snapshot.ar << nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		CGuildCombat::__REQUESTGUILD RequestGuild = vecRequestStatus.at( i );
		CGuild* pGuild = g_GuildMng.GetGuild( RequestGuild.uidGuild );

		if( pGuild )
		{
			m_Snapshot.ar.WriteString( pGuild->m_szGuild );
//			길드전 참가 인원 구성 시 참가인원 표시
			CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember( RequestGuild.uidGuild );
			if( pGCMember != NULL )
			{
				m_Snapshot.ar << pGCMember->vecGCSelectMember.size();
			}
			else	
//
				m_Snapshot.ar << 0;
			
		}
		else
		{
			m_Snapshot.ar.WriteString( "NotGuild" );
			m_Snapshot.ar << 0;
		}
		if( m_idGuild == RequestGuild.uidGuild )
			dwRequstPenya = RequestGuild.dwPenya;
	}	
	m_Snapshot.ar << dwRequstPenya;
	
}
// 선택 캐릭터 윈도우 띄움
void FLWSUser::AddGCSelectPlayerWindow( std::vector<CGuildCombat::__JOINPLAYER> &vecSelectPlayer, u_long uidDefender, BOOL bWindow, BOOL bRequestWar )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_SELECTPLAYER;
	m_Snapshot.ar << g_GuildCombatMng.m_nMaxJoinMember;
	m_Snapshot.ar << g_GuildCombatMng.m_nMaxWarPlayer;

	m_Snapshot.ar << bRequestWar;
	if( bRequestWar )
	{
		m_Snapshot.ar << bWindow;
		if( bWindow )
		{
			m_Snapshot.ar << uidDefender;
			int nSize = vecSelectPlayer.size();
			m_Snapshot.ar << nSize;
			for( int i = 0 ; i < nSize ; ++i )
			{
				CGuildCombat::__JOINPLAYER JoinPlayer = vecSelectPlayer.at( i );
				m_Snapshot.ar << JoinPlayer.uidPlayer;
			}	
		}
	}
	
}
// 전투위치 윈도우 띄움
void FLWSUser::AddGCJoinWarWindow( int nMap, int nTelTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_SELECTWARPOS;
	m_Snapshot.ar << nMap;
	m_Snapshot.ar << nTelTime;
	
}
void FLWSUser::AddGCWinGuild( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_WINGUILD;
	m_Snapshot.ar << g_GuildCombatMng.m_nGuildCombatIndex << g_GuildCombatMng.m_uWinGuildId << g_GuildCombatMng.m_nWinGuildCount;
	
}
void FLWSUser::AddGCBestPlayer( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_BESTPLAYER;
	m_Snapshot.ar << g_GuildCombatMng.m_uBestPlayer;
	
}
void FLWSUser::AddGCIsRequest( BOOL bRequest )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_ISREQUEST;
	m_Snapshot.ar << bRequest;
	
}
void FLWSUser::AddGCGetPenyaGuild( int nGetResult, __int64 nGetPenya )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GETPENYAGUILD;
	m_Snapshot.ar << nGetResult;
	m_Snapshot.ar << nGetPenya;
	
}
void FLWSUser::AddGCGetPenyaPlayer( int nGetResult, __int64 nGetPenya )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GETPENYAPLAYER;
	m_Snapshot.ar << nGetResult;
	m_Snapshot.ar << nGetPenya;
	
}
void FLWSUser::AddGCDiagMessage( const char* str )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_DIAGMESSAGE;
	m_Snapshot.ar.WriteString( str );
	
}
void FLWSUser::AddGCTele( const char* str )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_TELE;
	m_Snapshot.ar.WriteString( str );
	
}
void FLWSUser::AddGuildCombatNextTime( DWORD dwTime, DWORD dwState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_NEXTTIMESTATE;
	m_Snapshot.ar << dwTime << dwState;
	
}
void FLWSUser::AddGuildCombatEnterTime( DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_ENTERTIME;
	m_Snapshot.ar << dwTime ;
	
}

void FLWSUser::AddGCWarPlayerlist()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_WARPLAYERLIST;
	g_GuildCombatMng.SerializeGCWarPlayerList( m_Snapshot.ar );
	
}

void FLWSUser::AddCtrlCoolTimeCancel()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXPBOXCOLLTIMECANCEL;
	
}

void FLWSUser::AddGuildCombatState( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GCSTATE;
	m_Snapshot.ar << g_GuildCombatMng.m_nState << g_GuildCombatMng.m_nGCState;
	
}
void FLWSUser::AddGCLog( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_LOG;
	m_Snapshot.ar << (u_long)g_GuildCombatMng.m_vecGCGetPoint.size();
	for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_vecGCGetPoint.size() ) ; ++veci )
	{
		CGuildCombat::__GCGETPOINT GCGetPoint = g_GuildCombatMng.m_vecGCGetPoint.at( veci );
		m_Snapshot.ar << GCGetPoint.uidGuildAttack;
		m_Snapshot.ar << GCGetPoint.uidGuildDefence;
		m_Snapshot.ar << GCGetPoint.uidPlayerAttack;
		m_Snapshot.ar << GCGetPoint.uidPlayerDefence;
		m_Snapshot.ar << GCGetPoint.nPoint;
		m_Snapshot.ar << GCGetPoint.bKillDiffernceGuild;
		m_Snapshot.ar << GCGetPoint.bMaster;
		m_Snapshot.ar << GCGetPoint.bDefender;
		m_Snapshot.ar << GCGetPoint.bLastLife;
	}
	
}
void FLWSUser::AddGCPlayerPoint( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_PLAYERPOINT;
	m_Snapshot.ar << (u_long)g_GuildCombatMng.m_vecGCPlayerPoint.size();
	for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_vecGCPlayerPoint.size() ) ; ++veci )
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint = g_GuildCombatMng.m_vecGCPlayerPoint.at( veci );
		m_Snapshot.ar << GCPlayerPoint.uidPlayer;
		m_Snapshot.ar << GCPlayerPoint.nJob;
		m_Snapshot.ar << GCPlayerPoint.nPoint;
	}	
}
void FLWSUser::AddQuestTextTime( BOOL bFlag, int nState, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUEST_TEXT_TIME;
	m_Snapshot.ar << bFlag;
	m_Snapshot.ar << nState;
	m_Snapshot.ar << dwTime;
	
}

void FLWSUser::AddFocusObj(OBJID objid)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_FOCUSOBJ;
	m_Snapshot.ar << objid;
	
}

void FLWSUser::OnMsgArrival( DWORD dwParam )
{
	CCtrl* pObj = prj.GetCtrl( dwParam );
	if( NULL == pObj )
		return;

	if( pObj->GetType() != OT_MOVER )
		ClearDestObj();
	
	if( pObj->GetType() == OT_ITEM )
	{
		if( IsFly() )
			return;

		if( IsMode( ITEM_MODE ) )
			return;

		CItem* pItem = (CItem*)pObj;

		//pItem이 집을수 있는 아이템인가 검사.
		if( IsLoot( pItem ) == FALSE )	// 집을 수 없는 경우
		{
			AddDefinedText( TID_GAME_PRIORITYITEMPER, "\"%s\"", pItem->GetProp()->szName );	// %는 다른사람 아이템임다.
			return;
		}
		//pItem을 줏음.
		if( DoLoot( pItem ) == FALSE )
		{
			return;
		}
		g_xWSUserManager->AddMotion( this, OBJMSG_PICKUP );		// 줍는 모션
	}
	else
	if( pObj->GetType() == OT_MOVER )
	{
	}
	else if( pObj->GetType() == OT_CTRL )
	{
		if( IsValidObj( (CObj*)pObj ) )
		{
			CCommonCtrl*  pCommonCtrl = (CCommonCtrl*)pObj;
			if( !pCommonCtrl )
				return;

			FLItemElem* pKey = NULL;

			// 오브젝트 동작 조건 검사
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_PLAYER_ID )
			{
				if( m_idPlayer != pCommonCtrl->m_idExpPlayer )
				{
					AddDefinedText( TID_GAME_NOT_EXPBOX );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}

				if( pCommonCtrl->m_bAction == TRUE )
				{
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}		
				
				if( pCommonCtrl->m_dwCtrlReadyTime != 0xffffffff )
				{
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
				
				pCommonCtrl->m_dwCtrlReadyTime = timeGetTime() + OPEN_COOLTIME;
				m_dwCtrlReadyTime        = timeGetTime() + OPEN_COOLTIME;
				m_dwCtrlReadyId  = pCommonCtrl->GetId();						
				
				AddExpBoxCoolTime( pCommonCtrl->GetId(), pCommonCtrl->m_dwCtrlReadyTime - timeGetTime() );
			}

			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM )
			{
				// 아이템과 아이템 개수와 내구력이 둘중 있어야 열수 있음
				BOOL bResult = FALSE;
				pKey = m_Inventory.GetAtByItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
				if( IsUsableItem( pKey ) )
				{
					int nItemCount = m_Inventory.GetItemNumByItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );	// 아이템 갯수 꺼내기						
					if( nItemCount < (int)( pCommonCtrl->m_CtrlElem.m_dwSetItemCount ) && pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )	
						bResult = TRUE;
				}
				else
				{
					if( pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )
						bResult = TRUE;
				}
				
				if( bResult )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE1, "\"%s\"", g_xSpecManager->GetSpecItem(pCommonCtrl->m_CtrlElem.m_dwSetItem)->szName );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}							
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_LEVEL )
			{
				if( GetLevel() < (int)( pCommonCtrl->m_CtrlElem.m_dwSetLevel ) )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE2 );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST )
			{
				LPQUEST pQuest = GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum );

				if( pQuest )
				{
					if( pQuest->m_nState != pCommonCtrl->m_CtrlElem.m_dwSetFlagNum )
					{
						QuestProp * pQuestPorp = prj.m_aPropQuest.GetAt( pQuest->m_wId );
						if( pQuestPorp )
							AddDefinedText( TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle );

						AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
						return;
					}
				}
				else
				{
						QuestProp * pQuestPorp = prj.m_aPropQuest.GetAt( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum );
						if( pQuestPorp )
							AddDefinedText( TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle );

						AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
						return;
				}
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_CLASS )
			{
				if( GetJob() != pCommonCtrl->m_CtrlElem.m_bSetJob[GetJob()] )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE5 );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_GENDER )
			{
				if( GetSex() != pCommonCtrl->m_CtrlElem.m_dwSetGender )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE3 );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
			}

			if( this )	// ??
				pCommonCtrl->m_nMoverID = GetId();
		
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_TELEPORT )
			{
				if( IsFly() == FALSE )
				{
					D3DXVECTOR3 vPos( (float)( pCommonCtrl->m_CtrlElem.m_dwTeleX ), (float)( pCommonCtrl->m_CtrlElem.m_dwTeleY ), (float)( pCommonCtrl->m_CtrlElem.m_dwTeleZ ) );
					REPLACE( g_uIdofMulti, pCommonCtrl->m_CtrlElem.m_dwTeleWorldId, vPos, REPLACE_NORMAL, nTempLayer );
				}
				else
				{
					AddDefinedText( TID_PK_NO_FLIGHTAREA );
				}
			}
			// 퀘스트
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST_END )
			{
				if( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 != 0 )
				{
					LPQUEST pQuest = GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 );
					if( pQuest )
					{
						QUEST quest;
						if( SetQuest( pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum1, &quest ) )
						{
							AddSetQuest( &quest );
						}
					}
				}
				if( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 != 0 )
				{
					LPQUEST pQuest = GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 );
					if( pQuest )
					{
						QUEST quest;
						if( SetQuest( pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum2, &quest ) )
						{
							AddSetQuest( &quest );
						}
					}
				}
			}

			// 아이템 삭제
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM )
			{
				int nItemCount = pCommonCtrl->m_CtrlElem.m_dwSetItemCount;
				for( int i = 0; i < 10 && nItemCount > 0; ++i )
				{
					pKey = m_Inventory.GetAtByItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
					if( IsUsableItem( pKey ) )
					{
						if( nItemCount <= pKey->m_nItemNum )
						{
							RemoveItem( pKey->m_dwObjId, nItemCount );
							nItemCount = 0;
						}
						else
						{
							RemoveItem( pKey->m_dwObjId, pKey->m_nItemNum );
							nItemCount -= pKey->m_nItemNum;
						}							
					}
				}
			}
			pCommonCtrl->SetActionPlay();
			AddClearTarget();
		}
	}
}


void FLWSUser::CheckFiniteItem()
{
	for( DWORD i = 0; i < m_Inventory.GetMax(); i++ )
	{
		FLItemElem* pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->IsExpiring() && !pItemElem->IsFlag( FLItemElem::expired ) )
		{
			if( pItemElem->IsEatPet() )
			{
				if( IsUsingEatPet( pItemElem ) )
					InactivateEatPet();
				UpdateItem( pItemElem->m_dwObjId, UI_NUM, 0 );
			}
			else
			{
				PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
				if( pItemElem->m_dwObjIndex < MAX_HUMAN_PARTS )	// equiped item
					ResetDestParamEquip( pItemProp, pItemElem );

				pItemElem->SetFlag( FLItemElem::expired );
				pItemElem->ResetFlag( FLItemElem::is_using );
				UpdateItem( pItemElem->m_dwObjId, UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
				// 먹이 주머니 기간 만료이고, 활성화 되어있는 경우에는 비 활성화 처리한다.
				if( pItemElem->m_dwItemId == ITEM_INDEX( 21035, II_SYS_SYS_SCR_PET_FEED_POCKET ) && pItemElem->m_dwKeepTime > 0 && HasBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) ) )
					RemoveBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) );
			}
		}
	}
}


void FLWSUser::DoSMItemEquip( FLItemElem* pItemElem, DWORD dwParts )
{
	BYTE nResistAttackLeft = (BYTE)0xff;
	BYTE nResistAttackRight = (BYTE)0xff;
	BYTE nResistDefense = (BYTE)0xff;
	BYTE nResist = 0;
	switch( pItemElem->m_nResistSMItemId )
	{
	case ITEM_INDEX( 10277, II_CHR_SYS_SCR_FIREASTONE ):
		nResist = SAI79::FIRE;
		break;
	case ITEM_INDEX( 10278, II_CHR_SYS_SCR_WATEILSTONE ):
		nResist = SAI79::WATER;
		break;
	case ITEM_INDEX( 10279, II_CHR_SYS_SCR_WINDYOSTONE ):
		nResist = SAI79::WIND;
		break;
	case ITEM_INDEX( 10280, II_CHR_SYS_SCR_LIGHTINESTONE ):
		nResist = SAI79::ELECTRICITY;
		break;
	case ITEM_INDEX( 10281, II_CHR_SYS_SCR_EARTHYSTONE ):
		nResist = SAI79::EARTH;
		break;
	case ITEM_INDEX( 10282, II_CHR_SYS_SCR_DEFIREASTONE ):
		nResist = SAI79::FIRE;
		break;
	case ITEM_INDEX( 10283, II_CHR_SYS_SCR_DEWATEILSTONE ):
		nResist = SAI79::WATER;
		break;
	case ITEM_INDEX( 10284, II_CHR_SYS_SCR_DEWINDYOSTONE ):
		nResist = SAI79::WIND;
		break;
	case ITEM_INDEX( 10285, II_CHR_SYS_SCR_DELIGHTINESTONE ):
		nResist = SAI79::ELECTRICITY;
		break;
	case ITEM_INDEX( 10286, II_CHR_SYS_SCR_DEEARTHYSTONE ):
		nResist = SAI79::EARTH;
		break;
	}

	if( dwParts == PARTS_LWEAPON )
	{
		nResistAttackLeft = nResist;
	}
	else if( dwParts == PARTS_RWEAPON )
	{
		nResistAttackRight = nResist;
	}
	else
	{
		nResistDefense = nResist;
	}

	PT_ITEM_SPEC pItemProp = NULL;
	pItemProp = g_xSpecManager->GetSpecItem( pItemElem->m_nResistSMItemId );
	if( pItemProp )
	{
		if( nResistAttackLeft != (BYTE)0xff )
		{
			m_nAttackResistLeft = nResistAttackLeft;
			((FLWSUser*)this)->AddResistSMMode( m_nAttackResistLeft, m_nAttackResistRight, m_nDefenseResist );
			SetSMMode( SM_RESIST_ATTACK_LEFT, pItemProp->dwCircleTime );
			g_dpDBClient.SendLogSMItemUse( "1", (FLWSUser*)this, pItemElem, pItemElem->GetProp() );
			g_dpDBClient.SendLogSMItemUse( "", (FLWSUser*)this, NULL, pItemProp );
		}
		else
		if( nResistAttackRight != (BYTE)0xff )
		{
			m_nAttackResistRight = nResistAttackRight;
			((FLWSUser*)this)->AddResistSMMode( m_nAttackResistLeft, m_nAttackResistRight, m_nDefenseResist );
			SetSMMode( SM_RESIST_ATTACK_RIGHT, pItemProp->dwCircleTime );
			g_dpDBClient.SendLogSMItemUse( "1", (FLWSUser*)this, pItemElem, pItemElem->GetProp() );
			g_dpDBClient.SendLogSMItemUse( "1", (FLWSUser*)this, NULL, pItemProp );
		}
		else
		if( nResistDefense != (BYTE)0xff )
		{
			m_nDefenseResist = nResistDefense;
			((FLWSUser*)this)->AddResistSMMode( m_nAttackResistLeft, m_nAttackResistRight, m_nDefenseResist );
			SetSMMode( SM_RESIST_DEFENSE, pItemProp->dwCircleTime );
			g_dpDBClient.SendLogSMItemUse( "1", (FLWSUser*)this, pItemElem, pItemElem->GetProp() );
			g_dpDBClient.SendLogSMItemUse( "1", (FLWSUser*)this, NULL, pItemProp );
		}
	}
}

void FLWSUser::DoSMItemUnEquip( FLItemElem* pItemElem, DWORD dwParts )
{
	if( pItemElem->m_nResistSMItemId == ITEM_INDEX( 10277, II_CHR_SYS_SCR_FIREASTONE ) || pItemElem->m_nResistSMItemId == ITEM_INDEX( 10278, II_CHR_SYS_SCR_WATEILSTONE ) ||
		pItemElem->m_nResistSMItemId == ITEM_INDEX( 10279, II_CHR_SYS_SCR_WINDYOSTONE ) || pItemElem->m_nResistSMItemId == ITEM_INDEX( 10280, II_CHR_SYS_SCR_LIGHTINESTONE ) ||
		pItemElem->m_nResistSMItemId == ITEM_INDEX( 10281, II_CHR_SYS_SCR_EARTHYSTONE )
		)
	{
		if( dwParts == PARTS_LWEAPON )
		{
			SetSMMode( SM_RESIST_ATTACK_LEFT, 1 );
			g_dpDBClient.SendLogSMItemUse( "2", (FLWSUser*)this, pItemElem, pItemElem->GetProp() );
		}
		else
		{
			SetSMMode( SM_RESIST_ATTACK_RIGHT, 1 );
		}
		pItemElem->m_nResistSMItemId = 0;
		((FLWSUser*)this)->AddCommercialElem( pItemElem->m_dwObjId, 0 );
		g_dpDBClient.SendLogSMItemUse( "2", (FLWSUser*)this, pItemElem, pItemElem->GetProp() );
	}
	else
	if( pItemElem->m_nResistSMItemId == ITEM_INDEX( 10282, II_CHR_SYS_SCR_DEFIREASTONE ) || pItemElem->m_nResistSMItemId == ITEM_INDEX( 10283, II_CHR_SYS_SCR_DEWATEILSTONE ) ||
		pItemElem->m_nResistSMItemId == ITEM_INDEX( 10284, II_CHR_SYS_SCR_DEWINDYOSTONE ) || pItemElem->m_nResistSMItemId == ITEM_INDEX( 10285, II_CHR_SYS_SCR_DELIGHTINESTONE ) ||
		pItemElem->m_nResistSMItemId == ITEM_INDEX( 10286, II_CHR_SYS_SCR_DEEARTHYSTONE )
		)
	{
		SetSMMode( SM_RESIST_DEFENSE, 1 );
		pItemElem->m_nResistSMItemId = 0;
		((FLWSUser*)this)->AddCommercialElem( pItemElem->m_dwObjId, 0 );
		g_dpDBClient.SendLogSMItemUse( "2", (FLWSUser*)this, pItemElem, pItemElem->GetProp() );
	}
}

void FLWSUserManager::AddAddRegion( DWORD dwWorldId, REGIONELEM & re )
{
	CAr ar;
	
	ar << NULL_ID << SNAPSHOTTYPE_ADDREGION;
	ar << dwWorldId;
	ar.Write( &re, sizeof(re) );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	list<FLWSUser*>	lspUser;
	list<FLWSUser*>::iterator i;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		CWorld* pWorld	= pUser->GetWorld();
		POINT point	= {	(LONG)( pUser->GetPos().x ), (LONG)( pUser->GetPos().z ) };
		if( pWorld && pWorld->GetID() == dwWorldId && re.m_rect.PtInRect( point ) )
		{
			pUser->SetPosChanged( TRUE );
			lspUser.push_back( pUser );
		}
	}

	ar << (int)lspUser.size();
	for( i	= lspUser.begin(); i != lspUser.end(); ++i )
		ar << (*i)->GetId();

	GETBLOCK( ar, lpBuf, nBufSize );

	// transfer
	for( i	= lspUser.begin(); i != lspUser.end(); ++i )
		(*i)->AddBlock( lpBuf, nBufSize );

	lspUser.clear();
}

__int64 FLWSUser::GetEventFlagBit( int nBit )
{
	return m_nEventFlag & ( (__int64)0x0000000000000001 << nBit );
}

int FLWSUser::SetEventFlagBit( int nBit )
{
	int nCount	= 0;
	m_nEventFlag	|= ( (__int64)0x0000000000000001 << nBit );
	for( int i = 0; i < 63; i++ )
	{
		if( GetEventFlagBit( i ) )
			nCount++;
	}
	return nCount;
}

void FLWSUser::AddCallTheRoll( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CALLTHEROLL;
	m_Snapshot.ar << m_nEventFlag;
}

typedef struct __ITEMINFO
{
	DWORD dwItemId;
	int nItemNum;
	int nBins;
	__ITEMINFO( DWORD dwII, int nIN, int nB )
	:dwItemId( dwII ), nItemNum( nIN ), nBins( nB ) {}
} ITEMINFO;

void FLWSUserManager::CallTheRoll( int nBit )
{
	std::map<DWORD, FLWSUser*>::iterator i;
	for( i = m_users.begin(); i != m_users.end(); ++i )
	{
		FLWSUser* pUser = i->second;
		if( pUser->IsValid() == FALSE )
			continue;
		if( pUser->GetEventFlagBit( nBit ) )	// 0 or 1
			continue;

		int nLimitLevel = CEventGeneric::GetInstance()->GetEventLimitLevel( 531 );
		if( pUser->GetLevel() < nLimitLevel )
		{
			continue;
		}

		if( (int)( pUser->m_dwEventElapsed ) < nBit )
		{
			pUser->m_dwEventElapsed	= nBit;
			pUser->m_dwEventTime	= 1;	// restart
		}
		else
		{
#ifdef __MAINSERVER
			if( ++pUser->m_dwEventTime >= 60 )
#else	// __MAINSERVER
			if( ++pUser->m_dwEventTime >= 1 )
#endif	// __MAINSERVER
			{
				int nCount	= pUser->SetEventFlagBit( nBit );
				pUser->AddCallTheRoll();	// print

				std::vector<CEventGeneric::T_CALLTHEROLL_ITEM> vecCallTheRollItem;

				bool bItem = CEventGeneric::GetInstance()->GetCallTheRollItem( 0, vecCallTheRollItem );		// 매일 지급
				if( bItem == true )
				{
					
					for( DWORD i = 0; i < vecCallTheRollItem.size(); ++i )
					{
						FLItemElem itemElem;
						itemElem.m_dwItemId	= vecCallTheRollItem.at( i ).dwItemId;
						itemElem.m_nItemNum	= vecCallTheRollItem.at( i ).nItemNum;
						itemElem.SetFlag( static_cast< BYTE >( vecCallTheRollItem.at( i ).nBinds ) );
						itemElem.SetSerialNumber();

						//mem_set( szText, 0, sizeof( szText ) );
						TCHAR szText[512] = {0, };
						FLSPrintf( szText, _countof( szText ), GETTEXT( TID_GAME_EVENT_MAIL_CALLTHEROLL ), nCount, itemElem.GetProp()->szName );
						g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, (char*)GETTEXT( TID_GAME_EVENT_MAIL_CALLTHEROLL_TITLE ), szText );
					}
				}

				vecCallTheRollItem.clear();
				bItem = CEventGeneric::GetInstance()->GetCallTheRollItem( nCount, vecCallTheRollItem );		// 해당 일수 지급
				if( bItem == true )
				{
					for( DWORD i = 0; i < vecCallTheRollItem.size(); ++i )
					{
						FLItemElem itemElem;
						itemElem.m_dwItemId	= vecCallTheRollItem.at( i ).dwItemId;
						itemElem.m_nItemNum	= vecCallTheRollItem.at( i ).nItemNum;
						itemElem.SetFlag( static_cast< BYTE >( vecCallTheRollItem.at( i ).nBinds ) );
						itemElem.SetSerialNumber();

						//mem_set( szText, 0, sizeof( szText ) );
						TCHAR szText[512] = {0, };
						FLSPrintf( szText, _countof( szText ), GETTEXT( TID_GAME_EVENT_MAIL_CALLTHEROLL ), nCount, itemElem.GetProp()->szName );
						g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, (char*)GETTEXT( TID_GAME_EVENT_MAIL_CALLTHEROLL_TITLE ), szText );
					}
				}
			}
		}
	}
}

void FLWSUser::AddKawibawiboResult( int nResult, int nWinCount, DWORD dwItemId, int nItemCount, DWORD dwNextItemId, int nNextItemCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_KAWIBAWIBO_RESUTLT;

	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nWinCount;
	if( nResult == CMiniGame::KAWIBAWIBO_WIN )
	{
		m_Snapshot.ar << dwItemId << dwNextItemId;
		m_Snapshot.ar << nItemCount << nNextItemCount;
		m_Snapshot.ar << prj.m_MiniGame.nKawiBawiBo_MaxWin;
	}
	else if( nResult == CMiniGame::KAWIBAWIBO_GETITEM )
	{
		m_Snapshot.ar << dwItemId;
		m_Snapshot.ar << nItemCount;
	}
}

void FLWSUser::AddReassembleOpenWnd( std::vector<DWORD> vecItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_REASSEMBLE_OPENWND;
	m_Snapshot.ar << vecItemId.size();
	for( int i=0; i<(int)( vecItemId.size() ); ++i )
		m_Snapshot.ar << vecItemId.at( i );
}
	

void FLWSUser::AddReassembleResult( DWORD dwItemId, int nItemCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_REASSEMBLE_RESULT;
	
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << nItemCount;
}

void FLWSUser::AddAlphabetOpenWnd( int nQuestionID, int nPublicAlphabet, char chPublicChar, CString strQuestion )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_ALPHABET_OPENWND;

	m_Snapshot.ar << nQuestionID;
	m_Snapshot.ar << nPublicAlphabet;
	m_Snapshot.ar << chPublicChar;
	m_Snapshot.ar.WriteString( strQuestion );
}

void FLWSUser::AddAlphabetResult( int nResult, DWORD dwItemId, int nItemCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_ALPHABET_RESULT;
	
	m_Snapshot.ar << nResult;
	if( nResult != CMiniGame::ALPHABET_FAILED && nResult != CMiniGame::ALPHABET_NOTENOUGH_MONEY )
	{
		m_Snapshot.ar << dwItemId;
		m_Snapshot.ar << nItemCount;
	}
}

void FLWSUser::AddFiveSystemOpenWnd( int nBetMinPenya, int nBetMaxPenya, int nMultiple )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_FIVESYSTEM_OPENWND;
	
	m_Snapshot.ar << nBetMinPenya;
	m_Snapshot.ar << nBetMaxPenya;
	m_Snapshot.ar << nMultiple;
}

void FLWSUser::AddFiveSystemResult( int nResult, int nGetPenya, int nBetNum )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_FIVESYSTEM_RESULT;
	
	m_Snapshot.ar << nResult;
	if( ( nResult != CMiniGame::FIVESYSTEM_NOTENOUGH ) &&
		( nResult != CMiniGame::FIVESYSTEM_OVERMAX ) &&
		( nResult != CMiniGame::FIVESYSTEM_FAILED ) )
		m_Snapshot.ar << nGetPenya;
	
	if( nResult == CMiniGame::FIVESYSTEM_BETOK )
		m_Snapshot.ar << nBetNum;
	
}

void FLWSUser::AddUltimateMakeItem( int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << ULTIMATE_MAKEITEM;
	m_Snapshot.ar << nResult;
}

void FLWSUser::AddUltimateMakeGem( int nResult, int nNum)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << ULTIMATE_MAKEGEM;
	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nNum;
}

void FLWSUser::AddUltimateWeapon( BYTE state, int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << state;
	m_Snapshot.ar << nResult;
}

void FLWSUser::AddExchangeResult( BYTE state, int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXCHANGE;
	m_Snapshot.ar << state;
	m_Snapshot.ar << nResult;
}

void FLWSUser::AddRemoveAttribute( BOOL bSuccess )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVE_ATTRIBUTE;
	m_Snapshot.ar << bSuccess;
}

void FLWSUserManager::AddSetPetName( FLWSUser* pUser, const char* szPetName )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_PET_NAME;
	ar.WriteString( szPetName );

	GETBLOCK( ar, lpBuf, nBufSize );
	
	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddPetCall( CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel, const char* szPetName )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_CALL;
	ar << dwPetId << dwIndex << nPetLevel;
	ar.WriteString( szPetName );
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddPetRelease( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_RELEASE;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}




void FLWSUserManager::AddPetLevelup( CMover* pMover, DWORD dwPetId )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_LEVELUP;
	ar << dwPetId;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddPetFeed( CMover* pMover, WORD wEnergy )
{
	if( pMover == NULL )
	{
		return;
	}

	// 兩
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_FEED;
	ar << wEnergy;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddHonorTitleChange( CMover* pMover, int nChange )
{
	if( pMover == NULL )
	{
		return;
	}

	// 兩
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_HONOR_CHANGE_ACK;
	ar << nChange;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUserManager::AddSetSpeedFactor( CMover* pMover, FLOAT fSpeedFactor )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_SPEED_FACTOR;
	ar << fSpeedFactor;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}







void FLWSUser::AddLegendSkillResult( int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_LEGENDSKILLUP_RESULT;
//	m_Snapshot.ar << LEGENDSKILL_RESULT;	
	m_Snapshot.ar << nResult;	
}


void FLWSUserManager::AddEventLua( BYTE nId, BOOL bState )
{
	string strDesc = prj.m_EventLua.GetDesc( nId );
	
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		pUser->AddEventLuaDesc( (int)bState, strDesc );
	}
}


void FLWSUser::AddGuildBankLogView( BYTE byListType, short nLogCount, const __GUILDBANKLOG_ENTRY* logs )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDLOG_VIEW;
	m_Snapshot.ar << byListType;
	m_Snapshot.ar << nLogCount;
	
	for( short i=0; i<nLogCount; ++i )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "%s %d %d %d %d" ), logs[i].szPlayer, logs[i].nDate,logs[i].nItemID,logs[i].nItemAbilityOption,logs[i].nItemCount);
		m_Snapshot.ar.WriteString( logs[i].szPlayer );				// 누구 
		m_Snapshot.ar << logs[i].nDate;					// 언제 
		m_Snapshot.ar << logs[i].nItemID;					//  nItemAbilityOption
		if( byListType == GI_LOG_VIEW_ITEM_ADD || byListType == GI_LOG_VIEW_ITEM_REMOVE )
		{
			m_Snapshot.ar << logs[i].nItemAbilityOption;
			m_Snapshot.ar << logs[i].nItemCount;					//  
		}
	}
}

void FLWSUser::AddHonorListAck()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HONOR_LIST_ACK;
	
	for( int i=0; i<MAX_HONOR_TITLE; ++i )
	{
		m_Snapshot.ar << m_aHonorTitle[i];
	}
}

void FLWSUser::AddSealChar( short nLogCount, const __SEALCHAR_ENTRY* seals )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SEALCHAR_REQ;
	m_Snapshot.ar << nLogCount;
	
	for( short i=0; i<nLogCount; ++i )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "AddSealChar:%s %d %d" ), seals[i].szPlayer, seals[i].nPlayerSlot,seals[i].idPlayer);
		m_Snapshot.ar << seals[i].nPlayerSlot;					// 
		m_Snapshot.ar << seals[i].idPlayer;					//  
		m_Snapshot.ar.WriteString( seals[i].szPlayer );				// 누구 
	}
}

void FLWSUser::AddSealCharSet()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SEALCHARGET_REQ;
}

void FLWSUser::AddCoupon( int nCoupon )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EVENT_COUPON;
	m_Snapshot.ar << nCoupon;
}

void FLWSUser::AddSecretRoomMngState( int nState, DWORD dwRemainTime )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_MNG_STATE;
	m_Snapshot.ar << nState;
	m_Snapshot.ar << dwRemainTime;
}

void FLWSUser::AddSecretRoomContInfo( BYTE nContinent, BYTE nType, std::vector<__SECRETROOM_TENDER> & vecSecreetRoomTender, int nIndex )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_INFO;
	m_Snapshot.ar << nContinent;
	m_Snapshot.ar << nType;

	switch( nType )
	{
		case SECRETROOM_GUILDLIST :
			{
				m_Snapshot.ar << vecSecreetRoomTender.size();
	
				for( int i=0; i<(int)( vecSecreetRoomTender.size() ); i++ )
				{
					m_Snapshot.ar << vecSecreetRoomTender.at( i ).dwGuildId;
					m_Snapshot.ar << vecSecreetRoomTender.at( i ).nWarState;
					m_Snapshot.ar << vecSecreetRoomTender.at( i ).nKillCount;

					if( m_idGuild == vecSecreetRoomTender.at( i ).dwGuildId )
					{
						int nSize = vecSecreetRoomTender.at( i ).vecLineUpMember.size();
						m_Snapshot.ar << nSize;

						for(int j=0; j<nSize; j++)
						{
							m_Snapshot.ar << vecSecreetRoomTender.at( i ).vecLineUpMember.at( j );
						}
					}
				}
				
				std::map<int, int> mapMonsterNum = CSecretRoomMng::GetInstance()->m_mapMonsterNum;
				std::map<int, int>::iterator it = mapMonsterNum.begin();
				m_Snapshot.ar << mapMonsterNum.size();
				for( ; it!=mapMonsterNum.end(); ++it )
				{
					m_Snapshot.ar << it->first;
					m_Snapshot.ar << it->second;
				}
			}
			break;

		case SECRETROOM_KILLCOUNT :
			{
				m_Snapshot.ar << nIndex;
				m_Snapshot.ar << vecSecreetRoomTender.at( nIndex ).nKillCount;
			}
			break;

		case SECRETROOM_WARSTATE :
			{
				m_Snapshot.ar << nIndex;
				m_Snapshot.ar << vecSecreetRoomTender.at( nIndex ).nWarState;
			}
			break;
	}
}

void FLWSUser::AddSecretRoomTenderOpenWnd( int nTenderPenya )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_TENDEROPENWND;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMinPenya;
	m_Snapshot.ar << nTenderPenya;
}

void FLWSUser::AddSecretRoomLineUpOpenWnd( std::vector<DWORD>& vecLineUpMember )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_LINEUPOPENWND;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum;
	m_Snapshot.ar << vecLineUpMember.size();

	for( int i=0; i<(int)( vecLineUpMember.size() ); i++ )
		m_Snapshot.ar << vecLineUpMember.at( i );
}

void FLWSUser::AddSecretRoomTenderView( int nTenderPenya, int nRanking, time_t t, std::vector<__SECRETROOM_TENDER>& vecSRTender )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_TENDERVIEW;
	m_Snapshot.ar << nTenderPenya;
	m_Snapshot.ar << nRanking;
	m_Snapshot.ar << t;
	m_Snapshot.ar << vecSRTender.size();
	for( int i=0; i<(int)( vecSRTender.size() ); i++ )
		m_Snapshot.ar << vecSRTender.at( i ).dwGuildId;
}

void FLWSUser::AddLord( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LORD;
	CSLord::Instance()->Serialize( m_Snapshot.ar );
}

int FLWSUser::GetMuteText( void )
{
	if( m_dwMute > 0 )
		return TID_GAME_STATE_MUTE;
	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 20023, II_SYS_SYS_LS_RAGE ) ) )
		return TID_GAME_LORD_SKILL_RAGE_E001;
	return 0;
}

void FLWSUser::AddLordSkillTick( CLordSkill* pSkills )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LORD_SKILL_TICK;
	pSkills->SerializeTick( m_Snapshot.ar );	
}

// 
BOOL FLWSUser::IsTeleportable( void )
{
	DWORD dwWorld	= GetWorld()->GetID();
	if( dwWorld == WI_WORLD_GUILDWAR
		|| ( dwWorld >= WI_WORLD_GUILDWAR1TO1_0 &&  dwWorld <= WI_WORLD_GUILDWAR1TO1_L )
		|| dwWorld == WI_DUNGEON_MUSCLE
		|| dwWorld == WI_DUNGEON_KRRR
		|| dwWorld == WI_DUNGEON_BEAR
		|| dwWorld == WI_WORLD_KEBARAS
		|| ( dwWorld == WI_WORLD_MADRIGAL && prj.IsGuildQuestRegion( dwWorld, GetPos() ) )
		|| dwWorld == WI_DUNGEON_SECRET_0
		|| ( dwWorld >= WI_DUNGEON_SECRET_1 && dwWorld <= WI_DUNGEON_SECRET_L )
		|| dwWorld == WI_WORLD_CISLAND	// 2008.01.21 코랄 아일랜드 검사 추가
//		|| ( dwWorld >= WI_WORLD_HEAVEN01 && dwWorld <= WI_WORLD_HEAVEN05 )
		|| CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorld )
		|| dwWorld == WI_WORLD_QUIZ
	)
		return FALSE;
	return ( GetSummonState() == 0 );
}

void FLWSUser::AddSetTutorialState( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SET_TUTORIAL_STATE;
	m_Snapshot.ar << GetTutorialState();
}

void FLWSUserManager::AddElectionBeginCandidacy( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_BEGIN_CANDIDACY;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddElectionBeginVote( int nRequirement )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_BEGIN_VOTE;
	ar << nRequirement;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddElectionEndVote( u_long idPlayer )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_END_VOTE;
	ar << idPlayer;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_ADD_DEPOSIT;
	ar << idPlayer << iDeposit << tCreate;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddElectionSetPledge( u_long idPlayer, const char* szPledge )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_SET_PLEDGE;
	ar << idPlayer;
	ar.WriteString( szPledge );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddElectionIncVote( u_long idPlayer, u_long idElector )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_INC_VOTE;
	ar << idPlayer << idElector;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddLEventCreate( CLEComponent* pComponent )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_CREATE;
	pComponent->Serialize( ar );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddLEventInitialize( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_INITIALIZE;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddLEventTick( ILordEvent* pEvent )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_TICK;
	pEvent->SerializeTick( ar );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUserManager::AddLordSkillUse( FLWSUser* pUser, u_long idTarget, int nSkill )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_LORD_SKILL_USE << idTarget << nSkill;
	GETBLOCK( ar, lpBuf, nBufSize );

	pUser->AddBlock( lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( pUser->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pUser )
}

void FLWSUserManager::AddQueryPlayerData( u_long idPlayer )
{
	if( idPlayer == NULL_ID )
		return;
	PlayerData* pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( !pData )
		return;
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_QUERY_PLAYER_DATA;
	ar << idPlayer;
	ar.WriteString( pData->szPlayer );
	ar.Write( &pData->data, sizeof(sPlayerData) );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void FLWSUser::AddTaxInfo( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAX_ALLINFO;
	CTax::GetInstance()->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddTaxSetTaxRateOpenWnd( BYTE nCont )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAX_SETTAXRATE_OPENWND;
	m_Snapshot.ar << nCont;
	m_Snapshot.ar << CTax::GetInstance()->m_nMinTaxRate << CTax::GetInstance()->m_nMaxTaxRate;
}

void FLWSUserManager::AddTaxInfo( void )
{
	std::map<DWORD, FLWSUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		pUser->AddTaxInfo();
	}
}

void FLWSUser::AddRainbowRacePrevRankingOpenWnd()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_PREVRANKING_OPENWND;
	std::vector<DWORD> vecdwPrevRanking;
	vecdwPrevRanking = CRainbowRaceMng::GetInstance()->GetPrevRanking();
	m_Snapshot.ar << vecdwPrevRanking.size();
	for( int i=0; i<(int)( vecdwPrevRanking.size() ); i++ )
		m_Snapshot.ar << vecdwPrevRanking.at( i );
}

void FLWSUser::AddRainbowRaceApplicationOpenWnd()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_APPLICATION_OPENWND;
	m_Snapshot.ar << CRainbowRaceMng::GetInstance()->GetApplicantNum();
}

void FLWSUser::AddRainbowRaceState( CRainbowRace* pRainbowRace )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_NOWSTATE;
	pRainbowRace->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddMiniGameState( __MINIGAME_PACKET MiniGamePacket )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_MINIGAMESTATE;
	MiniGamePacket.Serialize( m_Snapshot.ar );
}

void FLWSUser::AddMiniGameExtState( __MINIGAME_EXT_PACKET MiniGameExtPacket )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_MINIGAMEEXTSTATE;
	MiniGameExtPacket.Serialize( m_Snapshot.ar );
}

void FLWSUser::AddHousingAllInfo( CHousing* pHousing )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_ALLINFO;
	pHousing->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddHousingSetFurnitureList( HOUSINGINFO& housingInfo, BOOL bAdd )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_FURNITURELIST;
	housingInfo.Serialize( m_Snapshot.ar );
	m_Snapshot.ar << bAdd;
}

void FLWSUser::AddHousingSetupFurniture( HOUSINGINFO* pHousingInfo )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_SETUPFURNITURE;
	pHousingInfo->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_PAPERINGINFO;
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << bSetup;
}

void FLWSUserManager::AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup, DWORD dwMasterId )
{
	for( std::map<DWORD, FLWSUser*>::iterator it=m_users.begin(); it!=m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() && pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM  )
		{
			if( pUser->GetLayer() == static_cast<int>( dwMasterId ) )
				pUser->AddHousingPaperingInfo( dwItemId, bSetup );
		}
	}
}

void FLWSUser::AddHousingSetVisitAllow( DWORD dwTargetId, BOOL bAllow )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_SETVISITALLOW;
	m_Snapshot.ar << dwTargetId << bAllow;
}

void FLWSUser::AddHousingVisitableList( std::vector<DWORD> & vecVisitable )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_VISITABLELIST;
	m_Snapshot.ar << vecVisitable.size();
	for( int i=0; i<(int)( vecVisitable.size() ); i++ )
		m_Snapshot.ar << vecVisitable.at( i );
}

void FLWSUser::AddNPCPos( const D3DXVECTOR3 & vPos )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUESTHELPER_NPCPOS;
	m_Snapshot.ar << vPos;
}

void FLWSUser::AddClearTarget()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CLEAR_TARGET;
}

BOOL FLWSUser::IsGuildMaster()
{
	if( GetGuild() && GetGuild()->IsMaster( m_idPlayer ) )
		return TRUE;

	return FALSE;
}

void FLWSUser::AddPCBangInfo( CPCBangInfo* pPI )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PCBANG_INFO;
	pPI->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddAccountPlayTime()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ACCOUNT_PLAYTIME;
	m_Snapshot.ar << m_nAccountPlayTime;
}

void FLWSUserManager::ResetAccountPlayTime()
{
	for( std::map<DWORD, FLWSUser*>::iterator it=m_users.begin(); it!=m_users.end(); ++it )
	{
		FLWSUser* pUser = it->second;
		if( pUser->IsValid() && pUser->m_nAccountPlayTime > -1 )
		{
			pUser->m_nAccountPlayTime = 0;
			pUser->AddAccountPlayTime();
		}
	}
}

void FLWSUser::AddSmeltSafety( BYTE nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SMELT_SAFETY;
	m_Snapshot.ar << nResult;
}

void FLWSUser::AddWorldReadInfo( DWORD dwWorldId, D3DXVECTOR3 vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_WORLD_READINFO;
	m_Snapshot.ar << dwWorldId << vPos;
}

void FLWSUser::AddQuizEventState( int nType, int nState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUIZ_STATE;
	m_Snapshot.ar << nType << nState;
}

void FLWSUser::AddQuizEventMessage( const char* lpszMessage, int nState )
{
	if( strlen( lpszMessage ) == 0 ) return;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUIZ_MESSAGE;
	m_Snapshot.ar.WriteString( lpszMessage );
	m_Snapshot.ar << nState;
}

void FLWSUser::AddQuizQuestion( int nType, const char* lpszQuestion, int nCount )
{
	if( strlen( lpszQuestion ) == 0 ) return;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUIZ_QUESTION;
	m_Snapshot.ar << nType;
	m_Snapshot.ar.WriteString( lpszQuestion );
	m_Snapshot.ar << nCount;
}

void FLWSUser::AddActivateVisPet( OBJID objIdVisPetItem, OBJID objIdVisPetId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_VISPET_ACTIVATE;
	m_Snapshot.ar << objIdVisPetItem << objIdVisPetId;
}
	
void FLWSUserManager::AddChangeMoverSfxId( CMover* pMover )
{
	if( pMover == NULL )
	{
		return;
	}

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVER_CHANGESFX;
	ar << pMover->m_dwMoverSfxId;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	if( pMover->IsPlayer() == TRUE )
	{
		static_cast<FLWSUser*>( pMover )->AddBlock( lpBuf, nBufSize );
	}

	FOR_VISIBILITYRANGE( pMover )
		if( pMover->m_idPlayer != USERPTR->m_idPlayer )
		{
			USERPTR->AddBlock( lpBuf, nBufSize );
		}
	NEXT_VISIBILITYRANGE( pMover )
}

void FLWSUser::AddGuildHousePakcet( int nPacketType, GH_Fntr_Info & gfi, int nIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_PACKET;
	m_Snapshot.ar << nPacketType << nIndex;
	gfi.Serialize( m_Snapshot.ar );
}

void FLWSUser::AddGuildHouseAllInfo( CGuildHouseBase* pGuildHouse )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_ALLINFO;
	m_Snapshot.ar << static_cast<BOOL>( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) );
	if( pGuildHouse )
	{
		m_Snapshot.ar << TRUE;
		pGuildHouse->SerializeAllInfo( m_Snapshot.ar );
	}
	else
		m_Snapshot.ar << FALSE;
}

void FLWSUser::AddGuildHouseRemvoe()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_REMOVE;
}

void FLWSUser::SetRestPoint( const int nRestPoint )
{
	m_nRestPoint = nRestPoint;

	//////////////////////////////////////////////////////////////////////////
	// 버블 포인트 버프화
	if( m_nRestPoint > 0 )
	{
		if( HasBuffByIk3( IK3_BUBBLEPOINT ) == FALSE )
		{
			AddBuff( BUFF_EQUIP, BUBBLEPOINT_BUFF_ID, 1, 999999999, 0, BT_TICK );
		}

		IBuff* pBuff = GetBuffByIk3( IK3_BUBBLEPOINT );
		if( pBuff != NULL )
		{
			pBuff->SetAdjValue( m_nRestPoint );
			AddBuffAdjValueRefresh( IK3_BUBBLEPOINT, pBuff->GetAdjValue() );
		}
	}
	else
	{
		IBuff* pBuff = GetBuffByIk3( IK3_BUBBLEPOINT );
		if( pBuff != NULL )
		{
			RemoveBuff( pBuff->GetType(), pBuff->GetId() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

EXPINTEGER FLWSUser::GetAddExpAfterApplyRestPoint( EXPINTEGER nExp )
{
	if( m_nRestPoint > 0 )
	{
		EXPINTEGER nAddExp = (EXPINTEGER)( nExp * GuildHouseMng->GetRestExpFactor( m_nLevel ) );	// 추가 경험치를 구한다.
		EXPFLOAT fTemp = static_cast<EXPFLOAT>( nExp * min( m_nLevel / 5, 1 ) ) / static_cast<EXPFLOAT>( prj.m_aExpCharacter[m_nLevel + 1].nExp1 );
		int nSubRestPoint = (int)( fTemp * static_cast<EXPFLOAT>( MAX_REST_POINT * GuildHouseMng->GetRestPointFactor(  this ) ) );	// 차감될 휴식포인트
		if( nSubRestPoint > 0 )	// 차감되는 포인트가 있어야 추가 경험치를 적용할 수 있다.
		{
			int nRemainPoint = SetDecRestPoint( nSubRestPoint );	// 휴식포인트를 차감하고 남은 포인트를 반환한다.
			if( nRemainPoint < 0 )	// 남은 포인트가 부족하면 남은 포인트만큼의 추가 경험치를 반환한다.
				return nAddExp * ( nSubRestPoint + nRemainPoint ) / nSubRestPoint;
			else	// 포인트가 부족하지 않으면 추가 경험치를 반환한다.
				return nAddExp;
		}
	}

	return 0;
}

int FLWSUser::SetAddRestPoint( int nRestPoint )
{
	if( nRestPoint == 0 ) return m_nRestPoint;
	
	m_nRestPoint	= (int)( m_nRestPoint + ( nRestPoint * GuildHouseMng->GetRestPointFactor( this ) ) );
	int nRemainPoint = m_nRestPoint;
	if( m_nRestPoint < 0 )	m_nRestPoint = 0;
	if( m_nRestPoint > MAX_REST_POINT ) m_nRestPoint = MAX_REST_POINT;

	SetRestPoint( m_nRestPoint );

	return nRemainPoint;
}

void FLWSUser::AddRestPoint()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_RESTPOINT;
	m_Snapshot.ar << m_nRestPoint;
}

BOOL FLWSUserManager::HasUserSameWorldnLayer( FLWSUser* pUserSrc )
{
	if( !IsValidObj( pUserSrc ) || !pUserSrc->GetWorld() )
		return FALSE;

	for( std::map<DWORD, FLWSUser*>::iterator it=m_users.begin(); it!=m_users.end(); ++it )
	{
		if( ( pUserSrc != it->second ) && ( pUserSrc->GetWorld() && it->second->GetWorld() )
			&& ( pUserSrc->GetWorld()->GetID() == it->second->GetWorld()->GetID() ) && ( pUserSrc->GetLayer() && it->second->GetLayer() ) )
			return TRUE;
	}

	return FALSE;
}

void FLWSUser::AddCheckedQuest()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUEST_CHECKED;
	m_Snapshot.ar << this->m_nCheckedQuestSize;
	if( this->m_nCheckedQuestSize )
		m_Snapshot.ar.Write( this->m_aCheckedQuest, sizeof(WORD) * this->m_nCheckedQuestSize ); 
}

void FLWSUser::AddInviteCampusMember( FLWSUser* pRequest )
{
	if( IsDelete() )	return;
	if( !IsValidObj( pRequest ) )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_INVITE;
	m_Snapshot.ar << pRequest->m_idPlayer;
	m_Snapshot.ar.WriteString( pRequest->GetName() );
}

void FLWSUser::AddUpdateCampus( CCampus* pCampus )
{
	if( IsDelete() )	return;
	if( !pCampus )		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_UPDATE;
	pCampus->Serialize( m_Snapshot.ar );
}

void FLWSUser::AddRemoveCampus( u_long idCampus )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_REMOVE;
	m_Snapshot.ar << idCampus;
}

void FLWSUser::AddUpdateCampusPoint( int nCampusPoint )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_UPDATE_POINT;
	m_Snapshot.ar << nCampusPoint;
}

void FLWSUser::ProcessCampus()
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( GetCampusId() );
	if( pCampus && pCampus->IsMember( m_idPlayer ) )
	{
		int nBuffLevel = pCampus->GetBuffLevel( m_idPlayer );
		if( nBuffLevel )
		{
			if( pCampus->IsMaster( m_idPlayer ) )
			{
				if( pCampus->IsChangeBuffLevel( m_idPlayer ) )
					RemoveIk3Buffs( IK3_TS_BUFF );
			}
			if( !HasBuffByIk3( IK3_TS_BUFF ) )
				ActiveCampusBuff( nBuffLevel );
		}
		else
			RemoveIk3Buffs( IK3_TS_BUFF );
	}
	else
		RemoveIk3Buffs( IK3_TS_BUFF );
}

void FLWSUser::ActiveCampusBuff( int nLevel )
{
	PT_ITEM_SPEC pProp	= g_xSpecManager->GetSpecItem( CCampusHelper::GetInstance()->GetCampusBuff( nLevel ) );
	if( pProp != NULL )
	{
		g_xApplyItemEffect->DoApplyEffect( this, this, pProp );
	}
}

int FLWSUser::GetPointByType( int nType )
{
	int nPoint = 0;

	switch( nType )
	{
	case POINTTYPE_CAMPUS:
		nPoint = GetCampusPoint();
		break;

	default:
		break;
	}
	
	return nPoint;
}

//	mulcom	BEGIN100405	각성 보호의 두루마리
void	FLWSUser::SelectAwakeningValue( DWORD dwItemObjID, SERIALNUMBER iSerialNumber, BYTE bySelectFlag )
{
	FLItemElem *	pItem = NULL;
	pItem	= (FLItemElem*)( GetItemId( dwItemObjID ) );

	if( pItem == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pItem is NULL in SelectAwakeningValue function. ItemObjID : [%d]" ), dwItemObjID );

		return;
	}

	SERIALNUMBER	iItemSerialNumber = 0;
	iItemSerialNumber	= pItem->GetSerialNumber();

	if( iItemSerialNumber != iSerialNumber )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Item SerialNumber is different. ItemSerialNumber : [%d] [%d]" ), iItemSerialNumber, iSerialNumber );

		return;
	}

	if( pItem->IsSetNewRandomOptionExtension() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "This item have not new random option." ) );

		return;
	}

	bool	bRetValue	= false;

	bRetValue	= pItem->SelectRandomOption( bySelectFlag );

	if( bRetValue == true )
	{
		FLSnapshotItemUpgradeRandomOptionAck toClient;
		toClient.dwItemObjID		= pItem->m_dwObjId;
		toClient.dwSerialNumber		= pItem->GetSerialNumber();
		pItem->GetRandomOption( toClient.kRandomOption );
		AddPacket( &toClient );

//		UpdateItemEx( (BYTE)( pItem->m_dwObjId ), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId() );

		//	mulcom	BEGIN100426	각성 보호의 두루마리 로그.
		//FLERROR_LOG( PROGRAM_NAME, _T( "UserID [%d] : ItemSerialNumber[%d] SelectAwakeningValue bySelectFlag[%d]" ), (int)( GetId() ), pItem->GetSerialNumber(), (int)( bySelectFlag ) );

		TCHAR	szSelectValue[128] = { 0 };
 		//mem_set( szSelectValue, 0, sizeof(szSelectValue) );

		if( bySelectFlag == 1 )			//	_AWAKE_OLD_VALUE
		{
 			FLSPrintf( szSelectValue, _countof( szSelectValue ), "SELECT _AWAKE_OLD_VALUE" );
		}
		else if( bySelectFlag == 2 )	//	_AWAKE_NEW_VALUE
		{
			FLSPrintf( szSelectValue, _countof( szSelectValue ), "SELECT _AWAKE_NEW_VALUE" );
		}
		else
		{
			FLSPrintf( szSelectValue, _countof( szSelectValue ), "SELECT ERROR" );
		}

 		g_DPSrvr.PutItemLog( this, "z", szSelectValue, pItem, 1 );
		//	mulcom	END100426	각성 보호의 두루마리 로그.

	}

	return;
}


#ifdef __GUILD_HOUSE_MIDDLE
void FLWSUser::AddGuildHouseTenderMainWnd( std::map<DWORD, DWORD> & mapGHTenderList )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_TENDER_MAINWND;
	m_Snapshot.ar << mapGHTenderList.size();
	for( std::map<DWORD, DWORD>::iterator it = mapGHTenderList.begin(); it != mapGHTenderList.end(); ++it )
	{
		m_Snapshot.ar << it->first << it->second;
	}
}

void FLWSUser::AddGuildHouseTenderInfoWnd( DWORD dwGHIndex, int nTenderMinPenya, __int64 n64TenderPenya, std::vector<DWORD>& vecGuildId )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_TENDER_INFOWND;
	m_Snapshot.ar << dwGHIndex << nTenderMinPenya << n64TenderPenya << vecGuildId.size();
	for( DWORD i = 0; i < vecGuildId.size(); ++i )
	{
		m_Snapshot.ar << vecGuildId.at( i );
	}
}

void FLWSUser::AddGuildHouseTenderResult( DWORD dwGHIndex, BOOL bResult )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_TENDER_RESULT;
	m_Snapshot.ar << dwGHIndex << bResult;
}

void FLWSUser::AddGuildHouseInfoWnd( const PT_PACKET_GUILDHOUSE_INFO ptGHInfo )
{
	if( IsDelete() == TRUE || ptGHInfo == NULL )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_INFOWND;
	m_Snapshot.ar.Write( (void*)ptGHInfo, sizeof( T_PACKET_GUILDHOUSE_INFO ) );
}
#endif // __GUILD_HOUSE_MIDDLE


#ifdef __HYPERLINK_ITEM16

void	FLWSUser::AddItemLinkInfo( const PT_PACKET_ITEM_LINK ptItemLink, FLItemElem* pItemElem )
{
	if( IsDelete() == TRUE || pItemElem == NULL )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ITEM_LINK_INFO;
	m_Snapshot.ar.Write( (void*)ptItemLink, sizeof( T_PACKET_ITEM_LINK ) );
	pItemElem->Serialize( m_Snapshot.ar );
}

#endif // __HYPERLINK_ITEM16


BOOL	FLWSUser::ReallocSerialNumber()
{
	BOOL bNeedUpdate = FALSE;

	// 시리얼 넘버 할당
	if( m_Inventory.GetAllocedSerialNumber() == FALSE )
	{
		m_Inventory.ReallocSerialNumber();
		bNeedUpdate = TRUE;
	}

	for( int i = 0; i < 3; ++i )
	{
		if( m_Bank[i].GetAllocedSerialNumber() == FALSE )
		{
			m_Bank[i].ReallocSerialNumber();
			bNeedUpdate = TRUE;
		}
	}
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( m_Pocket.IsAvailable( nPocket, FALSE ) == TRUE )
		{
			if( m_Pocket.m_kPocket[nPocket].GetAllocedSerialNumber() == FALSE )
			{
				m_Pocket.m_kPocket[nPocket].ReallocSerialNumber();
				bNeedUpdate = TRUE;
			}
		}
	}

	return bNeedUpdate;
}

void	FLWSUser::RegisterItemSerialNumber()
{
	m_Inventory.InsertAllItem();

	for( int i = 0; i < 3; ++i )
	{
		m_Bank[i].InsertAllItem();
	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( m_Pocket.IsAvailable( nPocket, FALSE ) == TRUE )
		{
			m_Pocket.m_kPocket[nPocket].InsertAllItem();
		}
	}
}

void	FLWSUser::UnregisterItemSerialNumber()
{
	m_Inventory.RemoveAllItem();

	for( int i = 0; i < 3; ++i )
	{
		m_Bank[i].RemoveAllItem();
	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( m_Pocket.IsAvailable( nPocket, FALSE ) == TRUE )
		{
			m_Pocket.m_kPocket[nPocket].RemoveAllItem();
		}
	}
}

// DWORD	FLWSUser::CheckValidMove( const D3DXVECTOR3 & vPos )
// {
// 	DWORD dwReturn	= MOVE_NONE;
// 
// 	const DWORD dwCurrTick		= GetTickCount();
// 
// 	const DWORD dwTickGap		= dwCurrTick - m_tMoveCheckData.dwLastMoveTick;
// 
// 	m_tMoveCheckData.dwLastMoveTick	= dwCurrTick;
// 
// 	const FLOAT fLimitDistance	= GetSpeed( m_pActMover->m_fSpeed ) * 100.0F;
// 
// 	D3DXVECTOR3 vDist	= GetPos() - vPos;
// 	const FLOAT fDist	= sqrtf( vDist.x * vDist.x + vDist.z * vDist.z );
// 
// 	if( fLimitDistance >= fDist )
// 	{
// 		if( dwTickGap >= VALID_PLAYER_MOVE_TICK )
// 		{
// 			dwReturn	= MOVE_VALID;
// 		}
// 	}
// 	else
// 	{
// 		dwReturn	= MOVE_INVALID;
// 	}
// 
// 	return dwReturn;
// }

#ifdef __ENCHANT_BARUNA16

void FLWSUser::AddSummonBarunaNPC()
{
	if( IsDelete() )	return;

	DWORD dwExp = 0;
	DWORD dwLevel = 0;			
	DWORD dwExpRatio = 0;	

	if( getBarunaNPCInfo( dwExp, dwLevel, dwExpRatio ) == TRUE )
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_CALL_BARUNA_NPC << m_BarunaNPCSummoner.GetNPCOBJID() << dwLevel << dwExpRatio;
	}
}


void FLWSUser::AddBarunaNPCInfo(/* const DWORD dwOldLevel */)
{
	if( IsDelete() )	return;

	DWORD dwExp = 0;
	DWORD dwLevel = 0;			
	DWORD dwExpRatio = 0;	

	if( getBarunaNPCInfo(/* dwOldLevel,*/ dwExp, dwLevel, dwExpRatio ) == TRUE )
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_BARUNA_NPC_INFO << m_BarunaNPCSummoner.GetNPCOBJID() << dwLevel << dwExpRatio;
	}
}

BOOL FLWSUser::getBarunaNPCInfo( /*const DWORD dwOldLevel, */DWORD & o_dwExp, DWORD & o_dwLevel, DWORD & o_dwExpRatio )
{
	const DWORD dwNPCItemOBJID		= GetBarunaNPCSummoner().GetNPCItemOBJID();
	if( dwNPCItemOBJID == NULL_ID )
		return FALSE;

	FLItemElem * pNPFLItemElem		= m_Inventory.GetAtId( dwNPCItemOBJID );
	if( pNPFLItemElem == NULL || pNPFLItemElem->m_pPet == NULL )
		return FALSE;

	o_dwExp				= pNPFLItemElem->m_pPet->GetExp();
	o_dwLevel				= g_xBarunaUpgradeNpc->ExpToLevel( o_dwExp );

	const DWORD dwCurrLevelExp	= o_dwExp - g_xBarunaUpgradeNpc->GetExpRequired( o_dwLevel - 1 );
	o_dwExpRatio				= ( DWORD )( ( float )dwCurrLevelExp / g_xBarunaUpgradeNpc->GetEachExp( o_dwLevel ) * 100 );
	//o_dwExpRatio		= ( DWORD )( o_dwExp / g_xBarunaUpgradeNpc->GetEachExp( o_dwLevel ) * 100 );
	return TRUE;
}

void FLWSUser::AddReleaseBarunaNPC( const OBJID NPCOBJID )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RELEASE_BARUNA_NPC << NPCOBJID;
}

void	FLWSUser::AddResultOperExtract( const PT_PACKET_OPER_EXTRACT ptOperExtract )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BARUNA16_OPER_EXTRACT;
	m_Snapshot.ar.Write( (void*)ptOperExtract, sizeof( T_PACKET_OPER_EXTRACT ) );
}

void	FLWSUser::AddResultOperCreate( const PT_PACKET_OPER_CREATE ptOperCreate )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BARUNA16_OPER_CREATE;
	m_Snapshot.ar.Write( (void*)ptOperCreate, sizeof( T_PACKET_OPER_CREATE ) );
}

void	FLWSUser::AddResultCidCreate( const PT_PACKET_CID_CREATE ptCidCreate )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BARUNA16_CID_CREATE;
	m_Snapshot.ar.Write( (void*)ptCidCreate, sizeof( T_PACKET_CID_CREATE ) );
}

void	FLWSUser::AddResultCidPieceUpgrade( const PT_PACKET_CIDPIECE_UPGRADE ptCidPieceUpgrade )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BARUNA16_CIDPIECE_UPGRADE;
	m_Snapshot.ar.Write( (void*)ptCidPieceUpgrade, sizeof( T_PACKET_CIDPIECE_UPGRADE ) );
}

void	FLWSUser::AddResultOperCidCreate( const PT_PACKET_OPERCID_CREATE ptOperCidCreate )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BARUNA16_OPERCID_CREATE;
	m_Snapshot.ar.Write( (void*)ptOperCidCreate, sizeof( T_PACKET_OPERCID_CREATE ) );
}

#endif // __ENCHANT_BARUNA16

//////////////////////////////////////////////////////////////////////////
// shopping basket
void	FLWSUser::AddResultBuyItem( const PT_PACKET_SHOPPING_BASKET ptBasket )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BUY_SHOPPING_BASKET;
	m_Snapshot.ar.Write( (void*)ptBasket, sizeof( T_PACKET_SHOPPING_BASKET ) );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// reset bind
void	FLWSUser::AddResultResetBind( const PT_PACKET_RESET_BIND ptResetBind )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RESET_BIND;
	m_Snapshot.ar.Write( (void*)ptResetBind, sizeof( T_PACKET_RESET_BIND ) );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// item merge
void	FLWSUser::AddResultItemMerge( const PT_PACKET_ITEM_MERGE ptItemMerge )
{
	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ITEM_MERGE_RESULT_ACK;
	m_Snapshot.ar.Write( (void*)ptItemMerge, sizeof( T_PACKET_ITEM_MERGE ) );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// teleport map
void	FLWSUser::AddPacket( const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	if( IsDelete() == TRUE )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();

	pPacket->Serialize( m_Snapshot.ar );
}
//////////////////////////////////////////////////////////////////////////


// 협동 기부
void FLWSUser::AddCooperativeContributions_Info( nsCooperativeContributions::FLContribution & contribution )
{
	using namespace nsCooperativeContributions;

	if( IsDelete() )	return;


	//CMover * pNPC	= prj.GetMover( NPCobjid );
	//if( IsValidObj( pNPC ) == FALSE )
	//	return;

	//if( IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
	//	return;

	//LPCHARACTER pCharacter	= prj.GetCharacter( pNPC->m_szCharacterKey );
	//if( pCharacter == NULL )
	//	return;

	//const DWORD dwContributionID	= pCharacter->m_dwContributionID;
	//FLContribution * pContribution	= COOPERATIVE_CONTRIBUTIONS().FindContribution( dwContributionID );
	//if( pContribution == NULL )
	//	return;

	//if( pContribution->IsEventPeriod() == FALSE )
	//{
	//	AddDefinedText( TID_MMI_COOPERATION_DONATION02 );
	//}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COOPERATIVE_CONTRIBUTIONS_INFO_ACK;

	T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_INFO_HEADER_ACK ack;
//	ack.dwContributionID	= dwContributionID;							//NPC가 다 가지고 있으므로 필요가 없다.
	ack.dwDescription_TextID = contribution.m_dwDescription_TextID;
	ack.dwEndCond_TextID	= contribution.m_dwEndCond_TextID;

	FLTm tmCurr				= DATE_TIMER().GetCurrentTime();
	if( tmCurr >= contribution.m_tmStartDate )		
	{
		__time64_t tEndSecond	= 0;	
		__time64_t tCurSecond	= 0;
		FLDateTimeToTimeT( tEndSecond, contribution.m_tmEndDate );
		FLDateTimeToTimeT( tCurSecond, DATE_TIMER().GetCurrentTime() );

		ack.tRemainSecond		= /*std::*/max( tEndSecond - tCurSecond, 0 );
	}
	else
	{
		ack.tRemainSecond		= 0;
	}

	ack.nEndConditionCount	= contribution.m_vecGlobalCondition.size();
	m_Snapshot.ar.Write( &ack, sizeof( ack ) );

	// 기부 정보
	for( FLContribution::GlobalConditionVec::iterator it = contribution.m_vecGlobalCondition.begin(); it != contribution.m_vecGlobalCondition.end(); ++it )
	{
		IGlobalEndCondition * p = *it;
		p->SerializeOut_Info( m_Snapshot.ar );
	}

	// 보상 정보
	const size_t nRewardCount	= contribution.m_vecReward_User.size() + contribution.m_vecReward_Global.size();
	m_Snapshot.ar << nRewardCount;

	for( FLContribution::UserRewardVec::const_iterator it = contribution.m_vecReward_User.begin(); it != contribution.m_vecReward_User.end(); ++it )
	{
		IUserReward * p		= *it;
		p->SerializeOut_Info( m_Snapshot.ar );
	}

	for( FLContribution::GlobalRewardVec::const_iterator it = contribution.m_vecReward_Global.begin(); it != contribution.m_vecReward_Global.end(); ++it )
	{
		IGlobalReward * p		= *it;
		p->SerializeOut_Info( m_Snapshot.ar );
	//	m_Snapshot.ar << p->GetRewardTextID();
	}
}

void	FLWSUser::AddCooperativeContributions_Contribute( const T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::E_RESULT eResult )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK;

	T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK ack;
	ack.eResult				= eResult;

	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
}


//void	FLWSUser::AddCooperativeContributions_ContributeItem( const T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::E_RESULT eResult, const DWORD dwContributionID, const DWORD dwItemID, const DWORD dwItemCount )
//{
//	m_Snapshot.cb++;
//	m_Snapshot.ar << GetId();
//	m_Snapshot.ar << SNAPSHOTTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ITEM_ACK;
//
//	T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK ack;
//	ack.eResult				= eResult;
//	ack.dwContributionID	= dwContributionID;
//	ack.dwItemID			= dwItemID;
//	ack.dwContribuedCount	= dwItemCount;
//
//	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
//}
//
// 
//void	FLWSUser::AddCooperativeContributions_ContributeGold( const T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK::E_RESULT eResult, const DWORD dwContributionID, const DWORD dwGoldCount )
//{
//	m_Snapshot.cb++;
//	m_Snapshot.ar << GetId();
//	m_Snapshot.ar << SNAPSHOTTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GOLD_ACK;
//
//	T_PACKET_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK ack;
//	ack.eResult				= eResult;
//	ack.dwContributionID	= dwContributionID;
//	ack.dwContribuedCount	= dwGoldCount;
//
//	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
//}


void	FLWSUser::AddCooperativeContributions_GetRanking( const T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK::E_RESULT eResult, const T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_ACK * pRanking )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_GET_RANKING_ACK;

	T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK ack = { 0 };
	ack.eResult				= eResult;

	if( pRanking == NULL )
	{
		m_Snapshot.ar.Write( &ack, sizeof( ack ) );
		return;
	}

	for( int i = 0; i < T_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK::RANKING_MAX; ++i )
	{
		FLStrcpy( ack.arrNameString[ i ], _countof( ack.arrNameString[ i ] ), pRanking->arrNameString[ i ] );
	}

	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
}

void	FLWSUser::AddCooperativeContributions_SetGlobalBuffReward( const E_REWARD_TYPE eRewardType )
{
	using namespace nsCooperativeContributions;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COOPERATIVE_CONTRIBUTIONS_REWARD_BUFF;
	m_Snapshot.ar << eRewardType;

	switch( eRewardType )
	{
	case REWARD_EXP_RATE:		
		{
			GLOBAL_REWARD_ALARM_EXP_FACTOR().SerializeOut_Rewarded( m_Snapshot.ar );
		}
		break;
	case REWARD_DROP_RATE:		
		{
			GLOBAL_REWARD_ALARM_DROP_RATE().SerializeOut_Rewarded( m_Snapshot.ar );
		}
		break;
	case REWARD_SHOP_BUY_FACTOR:		
		{
			GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().SerializeOut_Rewarded( m_Snapshot.ar );
		}
		break;
	case REWARD_SHOP_SELL_FACTOR:		
		{
			GLOBAL_REWARD_ALARM_SHOP_SELL_FACTOR().SerializeOut_Rewarded( m_Snapshot.ar );
		}
		break;
	case REWARD_DEFENCE_POWER:		
		{
			GLOBAL_REWARD_ALARM_DEFENCE_POWER().SerializeOut_Rewarded( m_Snapshot.ar );
		}
		break;
	case REWARD_ATTACK_POWER:		
		{
			GLOBAL_REWARD_ALARM_ATTACK_POWER().SerializeOut_Rewarded( m_Snapshot.ar );
		}
		break;
	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID reward buff type(%d) ]" ), eRewardType );
		break;
	}

}



//콜로세움
void	FLWSUser::AddColosseum_Enter( const T_PACKET_COLOSSEUM_ENTER_ACK::E_RESULT eResult )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_ENTER_ACK;

	T_PACKET_COLOSSEUM_ENTER_ACK ack;
	ack.eResult			= eResult;
	
	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
}

void	FLWSUser::AddColosseum_Retry( const T_PACKET_COLOSSEUM_RETRY_ACK::E_RESULT eResult )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_RETRY_ACK;

	T_PACKET_COLOSSEUM_RETRY_ACK ack;
	ack.eResult			= eResult;

	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
}

void	FLWSUser::AddColosseum_AutoInviteReq( const TCHAR * szInvitePlayerName, const DWORD dwWorldID )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_AUTO_INVITE_REQ;

	m_Snapshot.ar.WriteString( szInvitePlayerName );
	m_Snapshot.ar << dwWorldID;

}

void	FLWSUser::AddColosseum_AutoStartWaitingTick( const DWORD dwRemainTick )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_AUTO_START_WAITING_TICK;

	m_Snapshot.ar << dwRemainTick;

}
void	FLWSUser::AddColosseum_StageWaitingTick( const int nStage, const DWORD dwRemainTick )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_STAGE_WAITING_TICK;

	m_Snapshot.ar << ( nStage + 1 ) << dwRemainTick;
}

void	FLWSUser::AddColosseum_StatedStage( const int nStage, const DWORD dwTimeLimitRemainTick )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_STARTED_STAGE;

	T_PACKET_COLOSSEUM_STARTED_STAGE ack;
	ack.nStage				= nStage + 1;
	ack.dwTimeLimitRemainTick = dwTimeLimitRemainTick;

	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
}
void	FLWSUser::AddColosseum_ClearedStage( const int nStage )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_CLEARED_STAGE;

	T_PACKET_COLOSSEUM_CLEARED_STAGE ack;
	ack.nStage				= nStage + 1;

	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
}
void	FLWSUser::AddColosseum_StageTimeLimit( const int nStage, const DWORD dwRetryRemainTick )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_STAGE_TIME_LIMIT;

	m_Snapshot.ar << ( nStage + 1 ) << dwRetryRemainTick;

}
void	FLWSUser::AddColosseum_AllDie( const int nStage, const DWORD dwRetryRemainTick )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_ALL_DIE;

	m_Snapshot.ar << ( nStage + 1 ) << dwRetryRemainTick;
}

void	FLWSUser::AddColosseum_Retried( const int nStage, const DWORD dwRetryStageWaitingTick )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_RETRIED;

	m_Snapshot.ar << ( nStage + 1 ) << dwRetryStageWaitingTick;
}

// void	FLWSUser::AddColosseum_AllClear( const DWORD dwRemainTick, const DWORD dwCompleteTimeTick, CInstanceDungeonBase::DUNGEON_DATA::FLLevelProp::RewardVec & vecReward, const int nWeeklyRank )
// {
// 	m_Snapshot.cb++;
// 	m_Snapshot.ar << GetId();
// 	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_ALL_CLEAR;
// 
// 	T_PACKET_COLOSSEUM_ALL_CLEAR_HEADER ack;
// 	ack.dwRemainTick		= dwRemainTick;
// 	ack.dwCompleteTimeTick	= dwCompleteTimeTick;
// 	ack.nWeeklyRank			= nWeeklyRank;
// 
// 	ack.nRewardCount		= vecReward.size();
// 	m_Snapshot.ar.Write( &ack, sizeof( ack ) );
// 
// 	//가변 인자
// 	for( CInstanceDungeonBase::DUNGEON_DATA::FLLevelProp::RewardVec::iterator it = vecReward.begin(); it != vecReward.end(); ++it )
// 	{
// 		IDungeonReward * p	= *it;
// 		p->PushSnapshotTail( m_Snapshot.ar );
// 	}
// }

void	FLWSUser::AddColosseum_AllClear( const T_PACKET_COLOSSEUM_ALL_CLEAR_HEADER & toClient )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_ALL_CLEAR;

	m_Snapshot.ar.Write( &toClient, sizeof( toClient ) );
}

void	FLWSUser::AddColosseum_RegistAdditionalRealMonsterRateItemAck( const BOOL bSuccess )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_REGIST_ADDITIONAL_REAL_MONSTER_RATE_ITEM_ACK;

	m_Snapshot.ar << bSuccess;
}

void	FLWSUser::AddColosseum_UnRegistAdditionalRealMonsterRateItemAck( const BOOL bSuccess )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_UNREGIST_ADDITIONAL_REAL_MONSTER_RATE_ITEM_ACK;

	m_Snapshot.ar << bSuccess;
}

void	FLWSUser::AddColosseum_RegistAdditionalRealMonsterRateItemNoti( const DWORD dwItemID )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_REGISTED_ADDITIONAL_REAL_MONSTER_RATE_ITEM_NOTI;

	m_Snapshot.ar << dwItemID;
}


void FLWSUser::AddColosseum_ColosseumRanking( const T_PACKET_COLOSSEUM_GET_RANKING_INFO & ranking )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_RANKING_INFO;

	m_Snapshot.ar.Write( &ranking, sizeof( ranking ) );
}


void FLWSUser::TryRevival( const float fHPMPFPRate )
{
	if( IsLive() == TRUE )
		return;

	m_pActMover->ClearState();
	g_xWSUserManager->AddHdr( this, SNAPSHOTTYPE_REVIVAL_TO_CURRENT_POS );

	SetPointParam( DST_HP, ( int )( GetMaxHitPoint() * fHPMPFPRate ) );

	int nVal		= ( int )( GetMaxManaPoint() * fHPMPFPRate );			
	if( GetManaPoint() < nVal )
		SetPointParam( DST_MP, nVal );

	nVal			= ( int )( GetMaxFatiguePoint() * fHPMPFPRate );		
	if( GetFatiguePoint() < nVal )
		SetPointParam( DST_FP, nVal );

	g_xWSUserManager->AddCreateSfxObj( this, XI_INDEX( 108, XI_SYS_EXPAN01 ) );
}


void FLWSUser::OnRemoveGuildMember()
{
	//// 길드 인던 처리  OnRemoveGuildMember
	if( IsValidObj( this ) == FALSE )
		return;

	if( GetWorld() == NULL )
		return;

	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( GetWorld()->GetID() ) == FALSE )
		return;

	if( GetPartyId() == 0 )
		return;

	CParty* pParty		= g_PartyMng.GetParty( GetPartyId() );
	if( pParty == NULL )
		return;

	if( pParty->GetLeader() == NULL )
		return;

	INDUN_INFO * pInfo		= CInstanceDungeonParty::GetInstance()->GetDungeonInfo( pParty->m_uPartyId, GetWorld()->GetID() );
	if( pInfo == NULL )	
		return;

	if( pInfo->dwGuildID == 0 )			
		return; 

	CInstanceDungeonHelper::GetInstance()->GoOut( this );			//튕겨냄
	g_DPCoreClient.SendRemovePartyMember( pParty->GetLeader()->m_idPlayer, m_idPlayer );	//파티 탈퇴 요청
}

void	FLWSUser::AddCombinedOption( FLItemElem* pItemElem )
{
	if( pItemElem == NULL )
	{
		return;
	}

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ITEM_UPGRADE_COMBINED_OPTION_ACK;

	m_Snapshot.ar << pItemElem->m_dwObjId;
	m_Snapshot.ar << pItemElem->IsSetCombinedOption();
	m_Snapshot.ar << pItemElem->GetCombinedAddDamage();
	m_Snapshot.ar << pItemElem->GetCombinedRandomOptionOriginID();
	pItemElem->SerializeCombinedPiercingOption( m_Snapshot.ar );
	pItemElem->SerializeCombinedRandomOptionExtension( m_Snapshot.ar );
}

void	FLWSUser::Add_SellItemToNPC_Ack( const T_SELL_ITEM_TO_NPC_ACK & toClient )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SELL_ITEM_TO_NPC_ACK;

	m_Snapshot.ar.Write( &toClient, sizeof( toClient ) );
}
#ifdef PAT_LOOTOPTION
void	FLWSUser::AddPetLootOption()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PETLOOTOPTION;
}
#endif // PAT_LOOTOPTION

//void	FLWSUser::AddColosseum_Kick()
//{
//	m_Snapshot.cb++;
//	m_Snapshot.ar << GetId();
//	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_KICK;
//}

//-콜로세움

bool	FLWSUser::CanTeleportState( const T_WORLD_POSITION & kPos )
{
	if( IsDie() == TRUE || IsFly() == TRUE )
	{
		return false;
	}

	CWorld* pWorld	= g_WorldMng.GetWorld( kPos.dwWorldID );
	if( pWorld == NULL || pWorld->VecInWorld( kPos.tPos ) == FALSE )
	{
		return false;
	}

	CLinkMap* pLinkMap = pWorld->m_linkMap.GetLinkMap( nDefaultLayer );
	if( pLinkMap == NULL || pLinkMap->IsInvalid() == TRUE )
	{
		return false;
	}

	return true;
}

void	FLWSUser::TeleportToNPC( const DWORD dwNPC_ObjID, const int nMMI_ID )
{
	CMover* pNPC	= prj.GetMover( dwNPC_ObjID );
	if( pNPC == NULL || pNPC->IsPlayer() == TRUE )
	{
		return;
	}

	LPCHARACTER pCharacter	= pNPC->GetCharacter();
	if( pCharacter == NULL )
	{
		return;
	}

	if( IsRangeObj( pNPC->GetPos(), MAX_LEN_MOVER_MENU ) == FALSE )
	{
		return;
	}

	const T_WORLD_POSITION* pPos	= pCharacter->GetTeleportPos( nMMI_ID );
	if( pPos == NULL )
	{
		AddDefinedText( TID_GAME_NOMOVING );
		return;
	}

	if( CanTeleportState( *pPos ) == false )
	{
		AddDefinedText( TID_GAME_NOMOVING );
		return;
	}

	REPLACE( g_uIdofMulti, pPos->dwWorldID, pPos->tPos, REPLACE_NORMAL, nDefaultLayer );
}

void FLWSUser::AddRemoveObj( OBJID objid )
{
	AddHdr( objid, SNAPSHOTTYPE_DEL_OBJ );
}

//////////////////////////////////////////////////////////////////////////
// 20120516 - mirchang : 액션 슬롯 사용 시 쿨타임 체크 후 사용 못할 시 보내는 패킷..리팩토링 후 제거 예정.
void	FLWSUser::AddActionSlotSkillSkip_HardCording( const int nSkillUseType )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ACTION_SLOT_SKILL_SKIP;
	m_Snapshot.ar << nSkillUseType;
}
//////////////////////////////////////////////////////////////////////////

void	FLWSUser::AddBuffAdjValueRefresh( const DWORD dwItemKind3, const int nAdjValue )
{
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REFRESH_BUFF_ADJVALUE;
	m_Snapshot.ar << dwItemKind3 << nAdjValue;
}

/************************************************************************/
#ifdef __WORLDSERVER
float	FLWSUser::GetRateRecoveryRevival()
{
	float recoveryRate = 0.1f;

	CWorld* pWorld = GetWorld();
	if( pWorld == NULL ) {
		return recoveryRate;
	}

	if( pWorld->GetID() == WI_WORLD_GUILDWAR || g_GuildCombat1to1Mng.IsPossibleUser( this ) == TRUE ) {
		recoveryRate = 1.0f;
	}
	else {
		const int currentLevel	= GetLevel();

		for( std::vector< DIE_PENALTY >::const_iterator pos = prj.m_vecRevivalPenalty.begin(); pos != prj.m_vecRevivalPenalty.end(); ++pos ) {
			if( currentLevel <= pos->nLevel ) {
				recoveryRate = static_cast<float>( pos->nValue * decimalOneinTenThousand );
				break;
			}
		}
	}

	return recoveryRate;
}

bool	FLWSUser::GetDecreaseExperienceRevival( nsRevival::DECREASE_EXP_DATA& decreaseExpData ) const
{
	if( CanDecreaseExperienceRevival() == false ) {
		return false;
	}

	const int currentLevel	= GetLevel();

	for( std::vector< DIE_PENALTY >::const_iterator pos = prj.m_vecDecExpPenalty.begin(); pos != prj.m_vecDecExpPenalty.end(); ++pos ) {
		if( currentLevel <= pos->nLevel ) {
			decreaseExpData.decreaseExpRate = static_cast<float>( pos->nValue * decimalOneinTenThousand );
			break;
		}
	}

	for( std::vector< DIE_PENALTY >::const_iterator pos = prj.m_vecLevelDownPenalty.begin(); pos != prj.m_vecLevelDownPenalty.end(); ++pos ) {
		if( currentLevel <= pos->nLevel ) {
			decreaseExpData.downLevel = pos->nValue > 0 ? true : false;
			break;
		}
	}

	if( m_Resurrection_Data.bUseing == TRUE ) {
		if( m_Resurrection_Data.pAddSkillProp->dwDestParam[ 1 ] == DST_RECOVERY_EXP ) {
			float compensateExpRate = static_cast<float>( 100 - m_Resurrection_Data.pAddSkillProp->dwAdjParamVal[ 1 ] ) * decimalOneinHundred;
			decreaseExpData.decreaseExpRate = decreaseExpData.decreaseExpRate - ( decreaseExpData.decreaseExpRate * compensateExpRate );
		}
	}

	return ( decreaseExpData.decreaseExpRate > 0.0f ) ? true : false;
}

void	FLWSUser::DoRecoveryRevival( const float recoveryRate )
{
	if( m_Resurrection_Data.bUseing ) {
		FLWSUser* pSrc = g_xWSUserManager->GetUserByPlayerID( m_Resurrection_Data.dwPlayerID );
		if( IsValidObj( pSrc ) ) {
			g_cSkillSystem->ApplyParam( (CMover*)pSrc, this, m_Resurrection_Data.pSkillProp, m_Resurrection_Data.pAddSkillProp, TRUE , 0 );
		} else {
			this->SetDestParam( m_Resurrection_Data.pAddSkillProp->dwDestParam[ 0 ], m_Resurrection_Data.pAddSkillProp->dwAdjParamVal[ 0 ], static_cast<int>( m_Resurrection_Data.pAddSkillProp->dwChgParamVal[ 0 ] ), TRUE );
		}
	} else {
		SetPointParam( DST_HP, static_cast<int>( GetMaxHitPoint() * recoveryRate ) );
		const int recoveryMP	= static_cast<int>( GetMaxManaPoint() * recoveryRate );
		if( recoveryMP > GetManaPoint() )	{
			SetPointParam( DST_MP, recoveryMP );
		}
		const int recoveryFP	= static_cast<int>( GetMaxFatiguePoint() * recoveryRate );
		if( recoveryFP > GetFatiguePoint() )	{
			SetPointParam( DST_FP, recoveryFP );
		}
	}
}

bool	FLWSUser::DoDecreaseExperience( const nsRevival::DECREASE_EXP_DATA& decreaseExpData, bool& downLevel )
{
	const int originalLevel			= GetLevel();
	if( originalLevel <= 0 || originalLevel + 1 >= MAX_EXPCHARACTER ) {
		return false;
	}

	const EXPINTEGER originalMaxExp	= prj.m_aExpCharacter[ originalLevel + 1 ].nExp1;
	
	const EXPINTEGER decreaseExp	= static_cast<EXPINTEGER>( originalMaxExp * decreaseExpData.decreaseExpRate );
	if( decreaseExp <= 0 ) {
		return false;
	}

	if( IsAfterDeath() == FALSE ) {
		m_nDeathExp		= m_nExp1;			// 현재 경험치를 죽었을 때 경험치에 기록 
		m_nDeathLevel	= originalLevel;	// 현재 레벨 기록 
	}

	m_nExp1		-= decreaseExp;

	if( m_nExp1 < 0 ) {
		int logLevelUpAction = 0;

		if( decreaseExpData.downLevel == true && originalLevel >= 2 ) {
			m_nLevel			= originalLevel - 1;
			m_nExp1				= static_cast<EXPINTEGER>( prj.m_aExpCharacter[ originalLevel ].nExp1 + m_nExp1 );
			downLevel			= true;
			logLevelUpAction	= 6;	// 경험치 하락 후 레벨 다운.
		}
		else
		{
			m_nExp1				= 0;
			downLevel			= false;
			logLevelUpAction	= 7;	// 경험치 하락.
		}

		g_dpDBClient.SendLogLevelUp( this, logLevelUpAction );
	}

	return true;
}

bool	FLWSUser::GetRevivalPos( T_WORLD_POSITION& pos )
{
	CWorld* pWorld	= GetWorld();
	if( pWorld == NULL ) {
		return false;
	}

	if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( this ) ) {
		pos.dwWorldID		= WI_WORLD_MADRIGAL;
		pos.tPos			= CSecretRoomMng::GetInstance()->GetRevivalPos( this );
		pos.layer			= nDefaultLayer;
	}
	else if( pWorld->GetID() == WI_DUNGEON_MUSCLE || pWorld->GetID() == WI_DUNGEON_KRRR || pWorld->GetID() == WI_DUNGEON_BEAR ) {
		pos.dwWorldID		= WI_WORLD_MADRIGAL;
		pos.tPos			= D3DXVECTOR3( 6968.0f, 0.0f, 3328.8f );
		pos.layer			= nDefaultLayer;
	}
	else {
		PRegionElem	pRgnElem	= NULL;
		if( IsChaotic() == TRUE && pWorld->IsArena() == FALSE ) {
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );

			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );	
		}
		else {
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );

			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), GetPos() );	
		}

		if( pRgnElem != NULL ) {
			pos.dwWorldID		= pRgnElem->m_dwWorldId;
			pos.tPos			= pRgnElem->m_vPos;
			pos.layer			= nRevivalLayer;
		}
		else {
			pos.dwWorldID		= pWorld->GetID();
			pos.tPos			= GetPos();
			pos.layer			= GetLayer();
		}
	}

	return true;
}
#endif // __WORLDSERVER
/************************************************************************/
