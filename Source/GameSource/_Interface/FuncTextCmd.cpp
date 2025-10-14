
#include "stdafx.h"
#include "FuncTextCmd.h"
#include "WorldMng.h"

#ifdef __CLIENT
#include "WndAdminCreateItem.h"
#include "WndIndirectTalk.h"
#include "WndChangeFace.h"
#include "../Neuz/timeLimit.h"
#endif // __CLIENT

#ifdef __WORLDSERVER
#include "../worldserver/LinkMap.h"
#include "../WorldServer/User.h"
#include "../WorldServer/UserMacro.h"
#include "eveschool.h"
#include "../WorldServer/WorldDialog.h"
#include "../WorldServer/ItemUpgrade.h"	//sun:13, 제련 확장(속성, 일반)

#include "../_AIInterface/FLFSM.h"
#include "../WorldServer/FLCC_Reward.h"
#include "../worldserver/FLMadrigalGift.h"
#include "../worldserver/FLTreasureChest.h"

#include "../worldserver/FLFlyffPieceEvent.h"
#include "../_CommonDefine/Packet/FLPacketFlyffPieceEvent.h"
#endif	// __WORLDSERVER

#include "../_CommonDefine/Packet/FLPacketItemOption.h"




#include "playerdata.h"		//sun: 11, 캐릭터 정보 통합
#include "SecretRoom.h"		//sun: 12, 비밀의 방

//sun: 12, 군주
#ifdef __CLIENT
#include "../Neuz/clord.h"
#endif	// __CLIENT

#include "Tax.h"	//sun: 12, 세금
#include "honor.h"	//sun: 13, 달인

//sun: 13, 레인보우 레이스
#ifdef __WORLDSERVER
#include "RainbowRace.h"
#endif // __WORLDSERVER

#include "guild.h"
#include "party.h"
#include "post.h"

//sun: 13, 커플 시스템
#ifdef	__CLIENT
#include "../Neuz/couplehelper.h"
#endif
#ifdef	__WORLDSERVER
#include "../WorldServer/couplehelper.h"
#endif
#include "couple.h"

#ifdef __WORLDSERVER
#include "Quiz.h"
#endif // __WORLDSERVER

#include "GuildHouse.h"

#ifdef __WORLDSERVER
#include "../WorldServer/CampusHelper.h"
#include "../WorldServer/FLEventArenaGlobal.h"
#endif // __WORLDSERVER

extern	CPartyMng			g_PartyMng;
extern	CGuildMng			g_GuildMng;

#ifdef __CLIENT
extern	CParty				g_Party;
extern  CDPClient			g_DPlay;
#endif // __CLITEM

#ifdef __WORLDSERVER
extern  CWorldMng			g_WorldMng;
extern	CGuildCombat		g_GuildCombatMng;
#endif

#define TCM_CLIENT 0
#define TCM_SERVER 1
#define TCM_BOTH   2


#define BEGINE_TEXTCMDFUNC_MAP TextCmdFunc m_textCmdFunc[] = {
#define END_TEXTCMDFUNC_MAP 0, 0, 0, 0, 0, 0, AUTH_GENERAL, 0 };
#define ON_TEXTCMDFUNC( a, b, c, d, e, f, g, h ) a, b, c, d, e, f, g, h,

BOOL TextCmd_InvenClear( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	
	if( IsValidObj( pUser ) )
	{
		const DWORD invenSize = pUser->m_Inventory.GetMax();
		
		for( DWORD Nth = 0 ; Nth < invenSize; ++Nth )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAt( Nth );
			if( pItemElem && pUser->IsUsing( pItemElem ) == FALSE )
			{
				pUser->RemoveItem( pItemElem->m_dwObjId, pItemElem->m_nItemNum );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommandList( CScanner& /*scanner*/ )  
{ 
	return TRUE;
}
#ifdef __CLIENT
BOOL TextCmd_Open( CScanner& scanner )  
{ 

	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	g_WndMng.CreateApplet( dwIdApplet );
	return TRUE;
}
#else
BOOL TextCmd_Open( CScanner& /*scanner*/ )  
{ 
	return TRUE;
}
#endif


#ifdef __CLIENT
BOOL TextCmd_Close( CScanner& scanner )  
{ 
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	CWndBase* pWndBase = g_WndMng.GetWndBase( dwIdApplet );
	if( pWndBase ) pWndBase->Destroy();

	return TRUE;
}
#else
BOOL TextCmd_Close( CScanner& /*scanner*/ )  
{ 
	return TRUE;
}
#endif



BOOL TextCmd_Time( CScanner& /*scanner*/ )  
{ 
#ifdef __CLIENT
	CString string;
	CTime time = CTime::GetCurrentTime();
	//time.Get
	string = time.Format( "Real Time - %H:%M:%S" );
	g_WndMng.PutString( string );
	string.Format( "Madrigal Time - %d:%d:%d\n", g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	g_WndMng.PutString( string );
	
#endif
	return TRUE;
}

BOOL TextCmd_ChangeShopCost( CScanner & scanner )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();
	int nAllServer = scanner.GetNumber();

	if( f > 2.0f )
		f = 1.0f;
	else if( f < 0.5f )
		f = 1.0f;

	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_SHOPCOST );
		return TRUE;
	}

	prj.m_fShopCost = f;	
	g_xWSUserManager->AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetMonsterRespawn( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );

	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 30 ) dwNum = 30;
		if( dwNum < 1 ) dwNum = 1;

		DWORD dwAttackNum	= scanner.GetNumber();
		if( dwAttackNum > dwNum ) dwAttackNum = dwNum;
		if( dwAttackNum < 1 ) dwAttackNum = 0;

		DWORD dwRect = scanner.GetNumber();
		if( dwRect > 255 ) dwRect = 255;
		if( dwRect < 1 ) dwRect = 1;

		DWORD dwTime = scanner.GetNumber();
		if( dwTime > 10800 ) dwTime = 10800;
		if( dwTime < 10 ) dwTime = 10;

		int nAllServer = scanner.GetNumber();
		if( nAllServer != 0 )
		{
			BOOL bFlying = FALSE;
			if( pMoverProp->dwFlying )
				bFlying = TRUE;
			g_DPCoreClient.SendSetMonsterRespawn( pUser->m_idPlayer, pMoverProp->dwID, dwNum, dwAttackNum, dwRect, dwTime, bFlying );
			return TRUE;
		}

		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = pMoverProp->dwID;
		ri.m_cb = dwNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( pMoverProp->dwFlying != 0 )
			ri.m_vPos = vPos;
		ri.m_rect.left		= (LONG)( vPos.x - dwRect );
		ri.m_rect.right		= (LONG)( vPos.x + dwRect );
		ri.m_rect.top		= (LONG)( vPos.z - dwRect );
		ri.m_rect.bottom	= (LONG)( vPos.z + dwRect );
		ri.m_uTime			= (u_short)( dwTime );
		ri.m_cbTime			= 0;

		char chMessage[512] = {0,};
#ifdef __S1108_BACK_END_SYSTEM
			pWorld->m_respawner.Add( ri, TRUE );
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri );
#endif // __S1108_BACK_END_SYSTEM

		FLSPrintf( chMessage, _countof( chMessage ), "Add Respwan Monster : %s(%d/%d) Rect(%d, %d, %d, %d) Time : %d", 
			pMoverProp->szName, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __S1108_BACK_END_SYSTEM

BOOL TextCmd_PropMonster( CScanner & /*scanner*/ )
{
#ifdef __CLIENT
	char chMessage[1024] = {0,};
	if( 0 < prj.m_nAddMonsterPropSize )
	{
		for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
		{
			FLSPrintf( chMessage, _countof( chMessage ), "Monster Prop(%s) AttackPower(%d), Defence(%d), Exp(%d), Hitpioint(%d), ItemDorp(%d), Penya(%d)", 
				prj.m_aAddProp[i].szMonsterName, prj.m_aAddProp[i].nAttackPower, prj.m_aAddProp[i].nDefence, prj.m_aAddProp[i].nExp,
				prj.m_aAddProp[i].nHitPoint, prj.m_aAddProp[i].nItemDrop, prj.m_aAddProp[i].nPenya	);
			g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
		}
	}
	else
	{
		FLSPrintf( chMessage, _countof( chMessage ), "Monster Prop Not Data" );
		g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
	}
#endif	// __CLIENT
	return TRUE;
}
#endif // __S1108_BACK_END_SYSTEM

BOOL TextCmd_GameSetting( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->AddGameSetting();	
#endif // __WORLDSERVER
	return TRUE;	
}

BOOL TextCmd_ChangeFace( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;

	DWORD dwFace = scanner.GetNumber();
	if( dwFace < 0 || 4 < dwFace )
		return TRUE;
	
	if( (pUser->m_dwMode & NOTFRESH_MODE) || (pUser->m_dwMode & NOTFRESH_MODE2) )
	{
		pUser->m_dwHeadMesh = dwFace;
		g_xWSUserManager->AddChangeFace( pUser->m_idPlayer, dwFace );
		if( pUser->m_dwMode & NOTFRESH_MODE )
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE;
			pUser->AddDefinedText( TID_CHANGEFACE_ONE, "" );
//			pUser->AddText( "얼굴변경을 한번 사용하였습니다" );
		}
		else
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE2;
			pUser->AddDefinedText( TID_CHANGEFACE_TWO, "" );
//			pUser->AddText( "얼굴변경을 2번 사용하였습니다" );
		}
	}
	else
	{
		pUser->AddDefinedText( TID_CHANGEFACE_THREE, "" );
//		pUser->AddText( "얼굴변경을 2번 모두 사용하여 사용할수 없습니다" );
	}
#else // __WORLDSERVER
	CWndChangeSex* pWndChangeSex	= (CWndChangeSex*)g_WndMng.GetWndBase( APP_CHANGESEX );
	if( NULL == pWndChangeSex )
	{
		pWndChangeSex	= new CWndChangeSex;
		pWndChangeSex->Initialize();
	}
	pWndChangeSex->SetData( NULL_ID, NULL_ID );
	return FALSE;
#endif
	return TRUE;
}

BOOL TextCmd_AroundKill( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	if( pUser->GetWeaponItem() == NULL )
		return TRUE;

	CWorld* pWorld	= pUser->GetWorld();
	if( pWorld )
		pUser->SendDamageAround( AF_MAGICSKILL, (CMover*)pUser, OBJTYPE_MONSTER, 1, 3, 0.0, 1.0f );
#endif // __WORLDSERVER
	return TRUE;
}

//sun: 9, 9-10차 펫
BOOL	TextCmd_PetLevel( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CPet* pPet	= pUser->GetPet();
	if( pPet && pPet->GetExpPercent() == 100 )
		pUser->PetLevelup();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_MakePetFeed( CScanner & /*s*/ )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndPetFoodMill == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndPetFoodMill );
		g_WndMng.m_pWndPetFoodMill = new CWndPetFoodMill;
		g_WndMng.m_pWndPetFoodMill->Initialize( &g_WndMng, APP_PET_FOOD );
		return FALSE;
	}
#endif	// __CLIENT
	return TRUE;
}

BOOL	TextCmd_PetExp( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CPet* pPet	= pUser->GetPet();
	if( pPet && pPet->GetLevel() != PL_S )
	{
		pPet->SetExp( MAX_PET_EXP );
		pUser->AddPetSetExp( pPet->GetExp() );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Pet( CScanner & s )
{
	// /pet 1 100
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
//sun: 11, 캐릭터 정보 통합
	DWORD idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( idPlayer == 0 )	//
		return TRUE;
	FLWSUser* pTarget	= (FLWSUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( pTarget ) == FALSE )
		return TRUE;
	CPet* pPet	= pTarget->GetPet();
	if( pPet == NULL )	//
		return TRUE;

	// kind
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nKind	= static_cast< BYTE >( atoi( s.token ) );
	if( nKind >= PK_MAX )
		return TRUE;

	// exp
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nExpRate	= static_cast< BYTE >( atoi( s.token ) );

	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nLevel		= static_cast< BYTE >( s.Token.GetLength() );

	if( nLevel > PL_S )
		return TRUE;

	BYTE anAvail[PL_MAX - 1]	= { 0,};
	char sAvail[2]	= { 0,};
	
	for( int i = 0; i < nLevel; i++ )
	{
		sAvail[0]	= s.Token.GetAt( i );
		sAvail[1]	= '\0';
		anAvail[i]	= static_cast< BYTE >( atoi( sAvail ) );
		if( anAvail[i] < 1 || anAvail[i] > 9 )
			return TRUE;
	}

	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nLife	= static_cast< BYTE >( atoi( s.token ) );
	if( nLife > 99 )
		nLife	= 99;

	FLItemElem* pItemElem	= pTarget->GetPetItem();
	pPet->SetKind( nKind );
	pPet->SetLevel( nLevel );
	if( nLevel == PL_EGG )
		pPet->SetKind( 0 );	// initialize
	pItemElem->m_dwItemId	= pPet->GetItemId();
	pPet->SetEnergy( pPet->GetMaxEnergy() );
	DWORD dwExp		= pPet->GetMaxExp() * nExpRate / 100;
	pPet->SetExp( dwExp );

	for( BYTE i = PL_D; i <= nLevel; i++ )
		pPet->SetAvailLevel( i, anAvail[i-1] );
	for( BYTE i = nLevel + 1; i <= PL_S; i++ )
		pPet->SetAvailLevel( i, 0 );

	pPet->SetLife( nLife );

	if( pTarget->HasPet() )
		pTarget->RemovePet();

	g_dpDBClient.CalluspPetLog( pTarget->m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_LEVELUP, pPet );

	pTarget->AddPet( pPet, PF_PET_GET_AVAIL );	// 自
	g_xWSUserManager->AddPetLevelup( pTarget, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
#endif	// __WORLDSERVER
	return TRUE;
}

//sun: 11, 주머니
BOOL	TextCmd_MoveItem_Pocket( CScanner & 
#ifdef __CLIENT
								s 
#endif 
								)
{
#ifdef __CLIENT
	int	nPocket1	= s.GetNumber();
	int nData	= s.GetNumber();
	int nNum	= s.GetNumber();
	int	nPocket2	= s.GetNumber();
	FLItemElem* pItem	= NULL;
	if( nPocket1 < 0 )
		pItem	= g_pPlayer->m_Inventory.GetAt( nData );
	else
	{
		pItem	= g_pPlayer->m_Pocket.GetAtId( nPocket1, nData );
	}
	if( pItem )
		g_DPlay.SendMoveItem_Pocket( nPocket1, pItem->m_dwObjId, nNum, nPocket2 );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_AvailPocket( CScanner & 
#ifdef __CLIENT
						 s 
#endif
						 )
{
#ifdef __CLIENT
	int nPocket		= s.GetNumber();
	FLItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAt( 0 );
	if( pItemElem )
		g_DPlay.SendAvailPocket( nPocket, pItemElem->m_dwObjId );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_PocketView( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	pUser->AddPocketView();
#endif	// __WORLDSERVER
	return TRUE;
}

//sun: 11, 채집 시스템
BOOL TextCmd_RefineCollector( CScanner & s )
{
// 0번째
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	for( int nType = INVEN_TYPE_GENERAL; nType < INVEN_TYPE_MAX; ++nType )
	{
		DWORD dwObjIndex = pUser->m_Inventory.GetFirstObjIndex( nType );
		if( dwObjIndex == NULL_ID )
		{
			break;
		}
		int nAbilityOption	= s.GetNumber();
		if( s.tok == FINISHED )
			nAbilityOption	= 0;
		if( nAbilityOption > 5 )
			nAbilityOption	= 5;
		FLItemElem* pTarget	= pUser->m_Inventory.GetAt( dwObjIndex );
		if( pTarget && pTarget->IsCollector( TRUE ) )
		{
			pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
			pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
			if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
				g_xWSUserManager->AddCreateSfxObj( pUser, XI_INDEX( 1714, XI_INDEX( 1714, XI_INT_SUCCESS ) ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
			pUser->UpdateItem( (BYTE)( pTarget->m_dwObjId ), UI_AO,  nAbilityOption );
			
			break;
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StartCollecting( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	pUser->StartCollecting();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StopCollecting( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	pUser->StopCollecting();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DoUseItemBattery( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	pUser->DoUseItemBattery();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RefineAccessory( CScanner & s )
{
	// 0번째
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	for( int nType = INVEN_TYPE_GENERAL; nType < INVEN_TYPE_MAX; ++nType )
	{
		DWORD dwObjIndex = pUser->m_Inventory.GetFirstObjIndex( nType );
		if( dwObjIndex == NULL_ID )
		{
			break;
		}
		int nAbilityOption	= s.GetNumber();
		if( s.tok == FINISHED )
			nAbilityOption	= 0;
		FLItemElem* pTarget	= pUser->m_Inventory.GetAt( dwObjIndex );
		if( pTarget == NULL )
		{
			return TRUE;
		}

		const T_ITEM_SPEC* pItemSpec = pTarget->GetProp();
		if( pItemSpec == NULL )
		{
			return TRUE;
		}

		if(pTarget->IsAccessory() == TRUE || pItemSpec->IsAccessory() == TRUE )
		{
			pUser->UpdateItem( pTarget->m_dwObjId, UI_AO,  nAbilityOption );
			
			break;
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

//sun: 11, 각성, 축복
BOOL TextCmd_SetRandomOption( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	for( int nType = INVEN_TYPE_GENERAL; nType < INVEN_TYPE_MAX; ++nType )
	{
		DWORD dwObjIndex = pUser->m_Inventory.GetFirstObjIndex( nType );
		if( dwObjIndex == NULL_ID )
		{
			break;
		}
		FLItemElem* pItemElem	= pUser->m_Inventory.GetAt( dwObjIndex );
		if( IsUsableItem( pItemElem ) == TRUE )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp == NULL )
			{
				return FALSE;
			}

			if( pItemProp->IsUpgradeAble( IUTYPE_RANDOM_OPTION_EXTENSION ) == FALSE )
			{
				return FALSE;
			}

			{
				pItemElem->InitializeRandomOptionExtension();
				WORD wDstID			= static_cast<WORD>( s.GetNumber() );
				
				DWORD dwSize	= 0;
				while( s.tok != FINISHED )
				{
					switch( wDstID )
					{
					case DST_STR:
					case DST_DEX:
					case DST_INT:
					case DST_STA:
					case DST_CHR_CHANCECRITICAL:
					case DST_SPEED:
					case DST_ATTACKSPEED:
					case DST_ADJDEF:
					case DST_HP_MAX:
					case DST_MP_MAX:
					case DST_FP_MAX:
					case DST_SPELL_RATE:
					case DST_CRITICAL_BONUS:
					case DST_ATKPOWER:
						break;
					
					default:
						return TRUE;
					}

					short shAdjValue	= static_cast<short>( s.GetNumber() );
					
					if( s.tok == FINISHED )
						break;

					T_RANDOMOPTION_EXT kRandomOption;
					kRandomOption.wDstID		= wDstID;
					kRandomOption.shAdjValue	= shAdjValue;
					pItemElem->SetRandomOptionExtension( kRandomOption );
					if( ++dwSize >= MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE )
						break;
					wDstID	= static_cast<WORD>( s.GetNumber() );
				}

				FLSnapshotItemUpgradeRandomOptionAck toClient;
				toClient.dwItemObjID		= pItemElem->m_dwObjId;
				pItemElem->GetRandomOption( toClient.kRandomOption );
				pUser->AddPacket( &toClient );

				break;
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PickupPetAwakeningCancel( CScanner & /*s*/ )
{
#ifdef __CLIENT
	SAFE_DELETE( g_WndMng.m_pWndPetAwakCancel );
	g_WndMng.m_pWndPetAwakCancel = new CWndPetAwakCancel;
	g_WndMng.m_pWndPetAwakCancel->Initialize(&g_WndMng);
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_InitializeRandomOption( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	for( int nType = INVEN_TYPE_GENERAL; nType < INVEN_TYPE_MAX; ++nType )
	{
		DWORD dwObjIndex = pUser->m_Inventory.GetFirstObjIndex( nType );
		if( dwObjIndex == NULL_ID )
		{
			break;
		}
		FLItemElem* pItemElem	= pUser->m_Inventory.GetAt( dwObjIndex );
		if( IsUsableItem( pItemElem ) == TRUE && pItemElem->IsSetRandomOptionExtension() == true )
		{
			pItemElem->InitializeRandomOptionExtension();

			FLSnapshotItemUpgradeRandomOptionAck toClient;
			toClient.dwItemObjID		= pItemElem->m_dwObjId;
			pItemElem->GetRandomOption( toClient.kRandomOption );
			pUser->AddPacket( &toClient );

			break;
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemLevel( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	int nMinusLevel	= s.GetNumber();
	if( s.tok == FINISHED )
	{
		nMinusLevel	= 0;
	}

	if( nMinusLevel <= 0 )
	{
		return FALSE;
	}

	for( int nType = INVEN_TYPE_GENERAL; nType < INVEN_TYPE_MAX; ++nType )
	{
		DWORD dwObjIndex = pUser->m_Inventory.GetFirstObjIndex( nType );
		if( dwObjIndex == NULL_ID )
		{
			break;
		}

		FLItemElem* pTarget	= pUser->m_Inventory.GetAt( dwObjIndex );
		if( pTarget )
		{
			PT_ITEM_SPEC pProp	= pTarget->GetProp();
			if( pProp->dwParts != NULL_ID )
			{
				pTarget->SetLevelDown( nMinusLevel );

				pUser->UpdateItem( pTarget->m_dwObjId, UI_EQUIP_LEVEL, nMinusLevel );

				break;
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Level( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	scanner.GetToken();
	CString strJob = scanner.Token;



	int nJob = JOB_VAGRANT;

	LONG nLevel = scanner.GetNumber();
	
	if( nLevel == 0 )
		nLevel = 1;

	//	BEGIN100806	레벨업 명령어 사용시 클래스 코드도 적용
	int	nJobCode	= 0;
	nJobCode	= _ttoi( strJob.GetString() );
	//	END100806	레벨업 명령어 사용시 클래스 코드도 적용

	if( nJobCode != 0 && nJobCode < MAX_JOB )
	{
		nJob	= nJobCode;
	}
	else
	{
		for( int i = 0 ; i < MAX_JOB ; i++ )
		{
			if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
			{
				nJob = i;
				break;
			}
		}
	}

	char chMessage[MAX_PATH] = {0,};
	if( MAX_JOB_LEVEL < nLevel && nJob == 0 )
	{
		FLSPrintf( chMessage, _countof( chMessage ), prj.GetText(TID_GAME_CHOICEJOB) );
		pUser->AddText( chMessage );		
		return TRUE;
	}

//sun: 10차 전승시스템	Neuz, World, Trans
	LONG	nLegend = scanner.GetNumber();

	if( ( nLegend > 0 ) && ( nLegend < 4 ) )
	{
		for( int i = nJob + 1 ; i < MAX_JOB ; i++ )
		{
			if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
			{
				nJob = i;
				if( nLegend == 3 )
					break;
				else if( nLegend == 1 )
					break;
				else
					nLegend--;
			}
		}

		pUser->InitLevel( nJob, nLevel );	// lock
		return	TRUE;
	}
	
	if( nLevel <= MAX_JOB_LEVEL )	
	{
		pUser->InitLevel( JOB_VAGRANT, nLevel );	// lock
	}
	else if( MAX_JOB_LEVEL < nLevel &&  nLevel <= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
	{
		if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT)
		{
			pUser->InitLevel( nJob, nLevel );	// lock
		}
		else
		{
			FLSPrintf( chMessage, _countof( chMessage ), "Not Expert Job" );
			pUser->AddText( chMessage );
			FLSPrintf( chMessage, _countof( chMessage ), "Expert Job : " );
			for( int i = MAX_JOBBASE ; i < MAX_EXPERT ; ++i )
			{
				if( strlen( prj.m_aJob[i].szName ) < 15 )
				{
					FLStrcat( chMessage, _countof( chMessage ), prj.m_aJob[i].szName );
					if( i + 1 != MAX_EXPERT )
					{
						FLStrcat( chMessage, _countof( chMessage ), ", ");
					}
				}
			}
			pUser->AddText( chMessage );
			FLSPrintf( chMessage, _countof( chMessage ), "Expert Level : %d ~ %d", MAX_JOB_LEVEL + 1, MAX_JOB_LEVEL + MAX_EXP_LEVEL );
			pUser->AddText( chMessage );
			return TRUE;
		}
	}
	else if( MAX_JOB_LEVEL + MAX_EXP_LEVEL < nLevel && nLevel <= MAX_GENERAL_LEVEL )
	{
		if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL )
		{
			pUser->InitLevel( nJob, nLevel );	// lock
		}
		else
		{
			FLSPrintf( chMessage, _countof( chMessage ), "Not Professional Job" );
			pUser->AddText( chMessage );
			FLSPrintf( chMessage, _countof( chMessage ), "Professional Job : " );
			for( int i = MAX_EXPERT ; i < MAX_PROFESSIONAL ; ++i )
			{
				if( strlen( prj.m_aJob[i].szName ) < 15 )
				{
					FLStrcat( chMessage, _countof( chMessage ), prj.m_aJob[i].szName );
					if( i + 1 != MAX_PROFESSIONAL )
					{
						FLStrcat( chMessage, _countof( chMessage ), ", ");
					}
				}
			}
			pUser->AddText( chMessage );
			FLSPrintf( chMessage, _countof( chMessage ), "Professional Level : %d ~~~ ", MAX_JOB_LEVEL + MAX_EXP_LEVEL + 1 );
			pUser->AddText( chMessage );
			return TRUE;
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __SFX_OPT
BOOL TextCmd_SfxLv( CScanner & scanner )
{
	int nLevel = scanner.GetNumber();
	if(nLevel > 5) nLevel = 5;
	if(nLevel < 0) nLevel = 0;
	g_Option.m_nSfxLevel = nLevel;

	return TRUE;
}
#endif

BOOL TextCmd_ChangeJob( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	scanner.GetToken();
	CString strJob = scanner.Token;
	
	int nJob = JOB_VAGRANT;
	
	for( int i = 0 ; i < MAX_JOB ; i++ )
	{
		if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
		{
			nJob = i;
			break;
		}
	}
	
	char chMessage[MAX_PATH] = {0,};
	if( nJob == 0 )
	{
		FLSPrintf( chMessage, _countof( chMessage ), "Error Job Name or Number" );
		pUser->AddText( chMessage );		
		return TRUE;
	}
	
	if( pUser->AddChangeJob( nJob ) )
	{
		( (FLWSUser*)pUser )->AddSetChangeJob( nJob );
		g_xWSUserManager->AddNearSetChangeJob( (CMover*)pUser, nJob, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
		g_dpDBClient.SendLogLevelUp( (FLWSUser*)pUser, 4 );
//sun: 11, 캐릭터 정보 통합
		g_dpDBClient.SendUpdatePlayerData( pUser );
		return TRUE;
	}
	else
	{
		FLSPrintf( chMessage, _countof( chMessage ), "Error 1ch -> 2ch" );
		pUser->AddText( chMessage );		
		return TRUE;
	}

#else
	return TRUE;
#endif // __WORLDSERVER
}

BOOL TextCmd_stat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	scanner.GetToken();
	CString strstat = scanner.Token;
	
	DWORD dwNum	= scanner.GetNumber();

	if( 2 <= strstat.GetLength() && strstat.GetLength() <= 7)
	{
		strstat.MakeLower();

		if( strcmp( strstat, "str" ) == 0 )
		{
			pUser->m_Stat.SetOriginStr( dwNum );
		}
		else
		if( strcmp( strstat, "sta" ) == 0 )
		{
			pUser->m_Stat.SetOriginSta( dwNum );
		}
		else
		if( strcmp( strstat, "dex" ) == 0 )
		{
			pUser->m_Stat.SetOriginDex( dwNum );
		}
		else
		if( strcmp( strstat, "int" ) == 0 )
		{
			pUser->m_Stat.SetOriginInt( dwNum );
		}
		else
		if( strcmp( strstat, "gp" ) == 0 )
		{
			pUser->m_Stat.SetRemainStatPoint( dwNum );
		}
		else
		if( strcmp( strstat, "restate" ) == 0 )
		{
			pUser->ReState();
			return FALSE;
		}
//sun: 8, //__CSC_VER8_6
		else if( strcmp( strstat, "all" ) == 0 )
		{
			pUser->m_Stat.SetOriginStr( dwNum );
			pUser->m_Stat.SetOriginSta( dwNum );
			pUser->m_Stat.SetOriginDex( dwNum );
			pUser->m_Stat.SetOriginInt( dwNum );
		}

		else
		{
			strstat += "unknown setting target";
			pUser->AddText( strstat );
			return FALSE;
		}
	}
	else
	{
		strstat += "unknown setting target";
		pUser->AddText( strstat );
		return FALSE;
	}

	g_xWSUserManager->AddSetState( pUser );
//sun: 13, 달인
	pUser->CheckHonorStat();
	pUser->AddHonorListAck();
	g_xWSUserManager->AddHonorTitleChange( pUser, pUser->m_nHonor);
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetSnoop( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;

	s.GetToken();
	if( s.tok != FINISHED )
	{
		if( lstrcmp( pUser->GetName(), s.Token ) )
		{
//sun: 11, 캐릭터 정보 통합
			u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

			if( idPlayer > 0 )
			{
				BOOL bRelease	= FALSE;
				s.GetToken();
				if( s.tok != FINISHED )
					bRelease	= (BOOL)atoi( s.Token );
				g_DPCoreClient.SendSetSnoop( idPlayer, pUser->m_idPlayer,  bRelease );
			}
			else
				pUser->AddReturnSay( 3, s.Token );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_SetSnoopGuild( CScanner & s )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;

	s.GetToken();
	if( s.tok != FINISHED )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( s.Token );
		if( pGuild )
		{
			BOOL bRelease	= FALSE;
			s.GetToken();
			if( s.tok != FINISHED )
				bRelease	= (BOOL)atoi( s.Token );
			g_DPCoreClient.SendSetSnoopGuild( pGuild->m_idGuild, bRelease );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_QuerySetPlayerName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	scanner.GetToken();
	CString strPlayer	= scanner.Token;
	strPlayer.TrimLeft();
	strPlayer.TrimRight();
	g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, strPlayer, MAKELONG( 0xffff, 0 ) );
	return TRUE;
#else
	return FALSE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_QuerySetGuildName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	scanner.GetToken();
	CString strGuild	= scanner.Token;
	strGuild.TrimLeft();
	strGuild.TrimRight();
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, (LPSTR)(LPCSTR)strGuild, 0xff );
	}
	else
	{
		// is not kingpin
	}
	return TRUE;
#else
	return FALSE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_CreateGuild( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	scanner.GetToken();
	GUILD_MEMBER_INFO	info;
	info.idPlayer	= pUser->m_idPlayer;

	g_DPCoreClient.SendCreateGuild( &info, 1, scanner.Token );
	return TRUE;
#else
	return FALSE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_DestroyGuild( CScanner & /*scanner*/ )
{
#ifdef __CLIENT
	g_DPlay.SendDestroyGuild( g_pPlayer->m_idPlayer );
#endif
	return TRUE; 
}

BOOL TextCmd_RemoveGuildMember( CScanner & scanner )
{
#ifdef __CLIENT
	scanner.GetToken();
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	FLStrcpy( lpszPlayer, _countof( lpszPlayer ), scanner.Token );

	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( lpszPlayer );		//sun: 11, 캐릭터 정보 통합

	if( idPlayer != 0 )
		g_DPlay.SendRemoveGuildMember( g_pPlayer->m_idPlayer, idPlayer );
	return TRUE;
#else
	UNREFERENCED_PARAMETER( scanner );
	return FALSE;
#endif	// __CLIENT
}

BOOL TextCmd_GuildChat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

//sun: 12, 군주
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}

	char sChat[260]		= { 0, };
	scanner.GetLastFull();
	if( strlen( scanner.token ) >= 260 )
		return TRUE;
	FLStrcpy( sChat, _countof( sChat ), scanner.token );

	StringTrimRight( sChat );
	if( !(pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ) )))
		ParsingEffect(sChat, _countof( sChat ), strlen(sChat) );
	
	RemoveCRLF( sChat, _countof( sChat ) );

	g_DPCoreClient.SendGuildChat( pUser, sChat );
	return TRUE;
#else	// __WORLDSERVER
#ifdef __CLIENT
	CString string	= scanner.m_pProg;
	g_WndMng.WordChange( string );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
		string = '"'+string+'"';

	CString strCommand;
	strCommand.Format( "/g %s", string );
	g_DPlay.SendChat( strCommand );
#endif	// __CLIENT
	return FALSE;
#endif	// __WORLDSERVER
}

#ifdef __CLIENT
BOOL TextCmd_DeclWar( CScanner & scanner )
{

	char szGuild[MAX_G_NAME]	= { 0, };
	scanner.GetLastFull();
	if( strlen( scanner.token ) >= MAX_G_NAME )
		return TRUE;
	FLStrcpy( szGuild, _countof( szGuild ), scanner.token );
	StringTrimRight( szGuild );
	g_DPlay.SendDeclWar( g_pPlayer->m_idPlayer, szGuild );

	return FALSE;	
}
#else
BOOL TextCmd_DeclWar( CScanner & /*scanner*/ )			{		return FALSE; }
#endif	// __CLIENT


// 길드 랭킹정보를 업데이트 시키는 명령어이다.
BOOL TextCmd_GuildRanking( CScanner & /*scanner*/ )
{
#ifdef __WORLDSERVER
	// TRANS 서버에게 길드 랭킹 정보가 업데이트 되야함을 알린다.
	g_dpDBClient.UpdateGuildRanking();
#endif

	return TRUE;
}

// 길드 랭킹정보 DB를 업데이트 시키는 명령어이다.
BOOL TextCmd_GuildRankingDBUpdate( CScanner & /*scanner*/ )
{
#ifdef __WORLDSERVER
	// TRANS 서버에게 길드 랭킹 정보가 업데이트 되야함을 알린다.
	g_dpDBClient.UpdateGuildRankingUpdate();
#endif
	
	return TRUE;
}

BOOL TextCmd_GuildStat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	scanner.GetToken();
	CString strstat = scanner.Token;
	
	strstat.MakeLower();

	if( strstat == "logo" )
	{
		DWORD dwLogo = scanner.GetNumber();
		FLTRACE_LOG( PROGRAM_NAME, _T( "guild Logo:%d" ), dwLogo);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_LOGO, dwLogo );
	}
	else
	if( strstat == "pxp" )
	{
		DWORD dwPxpCount = scanner.GetNumber();
		FLTRACE_LOG( PROGRAM_NAME, _T( "guild pxpCount:%d" ), dwPxpCount);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PXPCOUNT, dwPxpCount );
	}
	else
	if( strstat == "penya" )
	{
		DWORD dwPenya = scanner.GetNumber();
		FLTRACE_LOG( PROGRAM_NAME, _T( "guild dwPenya:%d" ), dwPenya);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PENYA, dwPenya);
	}
	else
	if( strstat == "notice" )
	{
		scanner.GetToken();
		FLTRACE_LOG( PROGRAM_NAME, _T( "guild notice:%s" ), scanner.Token);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_NOTICE, (DWORD)(LPCTSTR)scanner.Token);
	}
	else
	{
		strstat += "syntax error guild stat command.";
		pUser->AddText( strstat );
	}

#endif 
	return FALSE;
}

BOOL TextCmd_SetGuildQuest( CScanner & s )
{
#ifdef __WORLDSERVER

	s.GetToken();
	char sGuild[MAX_G_NAME] = { 0, };
	FLStrcpy( sGuild, _countof( sGuild ), s.Token );
	int nQuestId	= s.GetNumber();
	int nState		= s.GetNumber();
	GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( nQuestId );
	if( !pProp )
		return FALSE;

	CGuild* pGuild	= g_GuildMng.GetGuild( sGuild );
	if( pGuild )
	{
		if( nState < QS_BEGIN || nState > QS_END )
		{
		}
		else
		{
			pGuild->SetQuest( nQuestId, nState );
			g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, nQuestId, nState );
		}
	}
	return TRUE;
#else
	return FALSE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_PartyLevel( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	DWORD dwLevel = scanner.GetNumber();
	DWORD dwPoint = scanner.GetNumber();
	if( dwPoint == 0 )
		dwPoint = 100;

	FLTRACE_LOG( PROGRAM_NAME, _T( "plv LV:%d POINT:%d" ), dwLevel, dwPoint);
	
	CParty* pParty;
	pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty )
	{
		pParty->SetPartyLevel( pUser, dwLevel, dwPoint, pParty->GetExp() );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL  TextCmd_InitSkillExp( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	if( pUser->InitSkillExp() == TRUE )
		pUser->AddInitSkill();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SkillLevel( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	DWORD dwSkillKind	= scanner.GetNumber();
	DWORD dwSkillLevel = scanner.GetNumber();

	LPSKILL pSkill = pUser->GetSkill( dwSkillKind );
	if( pSkill )
	{
		pSkill->dwLevel = dwSkillLevel;
	}
	else
	{
		return FALSE;
	}
	
	pUser->AddSetSkill( pSkill->dwSkill, pSkill->dwLevel );
#endif // __WORLDSERVER
#ifdef __CLIENT

	DWORD dwSkillLevel = scanner.GetNumber();

#ifdef __SKILL_UI16
	CWndSkill_16* pSkill = ( CWndSkill_16* )g_WndMng.GetWndBase( APP_SKILL4 );
#else
	CWndSkillTreeEx* pSkill = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif
	if( pSkill )
	{
		int nIndex = pSkill->GetCurSelect();
		if( nIndex == -1 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}
		LPSKILL pSkillbuf = pSkill->GetSkill( nIndex );
		if( pSkillbuf == NULL ) 
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}

		SkillProp* pSkillProp = prj.GetSkillProp( pSkillbuf->dwSkill );

		if( pSkillProp->dwExpertMax < 1 || pSkillProp->dwExpertMax < dwSkillLevel )
		{
			char szMessage[MAX_PATH];
			FLSPrintf( szMessage, _countof( szMessage ), prj.GetText(TID_GAME_SKILLLEVELLIMIT), pSkillProp->szName, pSkillProp->dwExpertMax );
//			FLSPrintf( szMessage, _countof( szMessage ), "%s' 스킬은 1 ~ %d 로만 레벨을 올릴수 있습니다", pSkillProp->szName, pSkillProp->dwExpertMax );

			g_WndMng.PutString( szMessage, NULL, 0xffff0000 );
			return FALSE;
		}
		char szSkillLevel[MAX_PATH];
		FLSPrintf( szSkillLevel, _countof( szSkillLevel ), prj.GetText(TID_GAME_GAMETEXT001), pSkillbuf->dwSkill, dwSkillLevel );
		scanner.SetProg( szSkillLevel );		
		//sprintf( scanner.pBuf, "/스렙 %d %d", pSkillbuf->dwSkill, dwSkillLevel );
	}
	else
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
//		g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
		return FALSE;
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SkillLevelAll( CScanner & scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	LPSKILL pSkill = NULL;
	SkillProp* pSkillProp = NULL;

	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		pSkill = &(pUser->m_aJobSkill[i]);

		if( pSkill == NULL || pSkill->dwSkill == 0xffffffff )
			continue;

		pSkillProp = prj.GetSkillProp( pSkill->dwSkill );

		if( pSkillProp == NULL )
			continue;

		const DWORD level	= pUser->GetLevel() >= pUser->GetDeathLevel() ? pUser->GetLevel() : pUser->GetDeathLevel();
		if( level < pSkillProp->dwReqDisLV ) {
			continue;
		}

		pSkill->dwLevel = pSkillProp->dwExpertMax;
	}
	pUser->m_nSkillPoint = 0;
	pUser->AddDoUseSkillPoint( pUser->m_aJobSkill, pUser->m_nSkillPoint );
#endif // __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_whisper( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
	static	CHAR	lpString[260];

	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( pUser->IsMode( SAYTALK_MODE ) )
		return TRUE;

//sun: 12, 군주
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}

	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
		u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );	//sun: 11, 캐릭터 정보 통합

		if( idPlayer > 0 ) 
		{
			scanner.GetLastFull();
			if( strlen( scanner.token ) >= 260 )	
				return TRUE;
			FLStrcpy( lpString, _countof( lpString ), scanner.token );
			
			StringTrimRight( lpString );
			
			if( !(pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ))))
			{
				ParsingEffect(lpString, _countof( lpString ), strlen(lpString) );
			}
			RemoveCRLF( lpString, _countof( lpString ) );
			
			if( 0 < strlen(lpString) )
				g_DPCoreClient.SendWhisper( pUser->m_idPlayer, idPlayer, lpString );
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_say( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
	static	CHAR	lpString[1024];
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	if( pUser->IsMode( SAYTALK_MODE ) )
		return TRUE;

//sun: 12, 군주
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}

	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );		//sun: 11, 캐릭터 정보 통합

		if( idPlayer > 0 ) 
		{
			scanner.GetLastFull();
			if( strlen( scanner.token ) > 260 )	//
				return TRUE;

			FLStrcpy( lpString, _countof( lpString ), scanner.token );

			StringTrimRight( lpString );
			
			if( !(pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ))))
			{
				ParsingEffect(lpString, _countof( lpString ), strlen(lpString) );
			}
			RemoveCRLF( lpString, _countof( lpString ) );
			
			g_DPCoreClient.SendSay( pUser->m_idPlayer, idPlayer, lpString );
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// 자기 자신에게 명령했다.
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ResistItem( CScanner& scanner )
{

#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	DWORD dwObjId	= scanner.GetNumber();
	BYTE bItemResist = static_cast< BYTE >( scanner.GetNumber() );
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();

// 	if( bItemResist < 0 || 5 < bItemResist )
// 	{
// 		return FALSE;
// 	}
// //sun:13, 제련 확장(속성, 일반)
// 	if( nResistAbilityOption < 0 || CItemUpgrade::GetInstance()->GetMaxAttributeEnchantSize() < nResistAbilityOption 
// 		|| nAbilityOption < 0 || CItemUpgrade::GetInstance()->GetMaxGeneralEnchantSize() < nAbilityOption )
// 	{
// 		return FALSE;
// 	}
// 
// 	if( bItemResist == 0 )
// 	{
// 		nResistAbilityOption = 0;
// 	}

	if( bItemResist < SAI79::NO_PROP || bItemResist >= SAI79::END_PROP )
	{
		return FALSE;
	}

	if( nResistAbilityOption < 0 || nAbilityOption < 0 )
	{
		return FALSE;
	}

	if( bItemResist == SAI79::NO_PROP )
	{
		nResistAbilityOption = 0;
	}

	FLItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
	{
		return FALSE;
	}
	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return FALSE;
	}

	if( nResistAbilityOption > CItemUpgrade::GetInstance()->GetMaxAttributeEnchantSize()
		|| nAbilityOption > CItemUpgrade::GetInstance()->GetMaxEnchantSize( pItemElem0 ) )
	{
		return FALSE;
	}
	
	pUser->UpdateItem( (BYTE)( pItemElem0->m_dwObjId ), UI_IR,  bItemResist );
	pUser->UpdateItem( (BYTE)( pItemElem0->m_dwObjId ), UI_RAO,  nResistAbilityOption );
	pUser->UpdateItem( (BYTE)( pItemElem0->m_dwObjId ), UI_AO,  nAbilityOption );

	if( pItemElem0->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )		//sun: 9,10차 제련 __ULTIMATE
//sun: 12, 무기 피어싱
		pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, nAbilityOption - 5 );
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
	if( pItemElem0->GetProp()->IsBarunaWeapon() )
	{
		int nPiercingSize = ((nAbilityOption + 1) - 10) * 0.5f;
		pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, nPiercingSize );
	}
#endif	// BARUNA_UPGRADE_ENHANCEMENT_GEM
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	if( ( pItemElem0->GetProp()->IsCostumeEnhanceParts()))
		pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, nAbilityOption - 6 );
#endif
#else // __WORLDSERVER
#ifdef __CLIENT
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}

	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();

// 	if( bItemResist < 0 || 5 < bItemResist )
// 	{
// 		return FALSE;
// 	}
// //sun:13, 제련 확장(속성, 일반)
// 	if( nResistAbilityOption < 0 || 20 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
// 	{
// 		return FALSE;
// 	}
// 
// 	if( bItemResist == 0 )
// 	{
// 		nResistAbilityOption = 0;
// 	}

	if( bItemResist < SAI79::NO_PROP || bItemResist >= SAI79::END_PROP )
	{
		return FALSE;
	}

	if( nResistAbilityOption < 0 || nAbilityOption < 0 )
	{
		return FALSE;
	}

	if( bItemResist == SAI79::NO_PROP )
	{
		nResistAbilityOption = 0;
	}

	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_pItemElem[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_pItemElem[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			FLSPrintf( szSkillLevel, _countof( szSkillLevel ), "/ritem %d %d %d %d", dwObjId, bItemResist, nResistAbilityOption, nAbilityOption );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommercialElem( CScanner& /*scanner*/ )
{
#ifdef __CLIENT
	SAFE_DELETE( g_WndMng.m_pWndCommerialElem );
	g_WndMng.m_pWndCommerialElem = new CWndCommercialElem;
	g_WndMng.m_pWndCommerialElem->Initialize( &g_WndMng, APP_COMMERCIAL_ELEM );
#endif // __CLIENT
	return FALSE;
}

BOOL TextCmd_Piercing( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	DWORD dwObjId	= scanner.GetNumber();
	BYTE bPierNum = static_cast< BYTE >( scanner.GetNumber() );

	FLItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
		return FALSE;

	if( bPierNum < 0 || bPierNum > MAX_PIERCING )
		return FALSE;

	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return FALSE;
	}

	if( bPierNum < pItemElem0->GetGeneralPiercingSize() )
	{
		pUser->UpdateItem( pItemElem0->m_dwObjId, UI_PIERCING_SIZE, bPierNum );
		return TRUE;
	}

	for( int i=1; i<=bPierNum; i++ )
	{
//sun: 12, 무기 피어싱
		if( pItemElem0->IsPierceAble( NULL_ID, TRUE ) )
			pUser->UpdateItem( pItemElem0->m_dwObjId, UI_PIERCING_SIZE, i );
	}
#else // __WORLDSERVER
#ifdef __CLIENT
	BYTE bPierNum = scanner.GetNumber();

	if( bPierNum < 0 || bPierNum > MAX_PIERCING )
	{
		return FALSE;
	}	

	CWndPiercing* pWndPiercing = (CWndPiercing*)g_WndMng.GetWndBase(APP_PIERCING);
	if( pWndPiercing )
	{
		if( pWndPiercing->m_pItemElem[0] )
		{
			DWORD dwObjId = pWndPiercing->m_pItemElem[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			FLSPrintf( szSkillLevel, _countof( szSkillLevel ), "/pier %d %d", dwObjId, bPierNum );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL IsRight( CString &str )
{
	if( str.GetLength() == 1 )
	{
		if( str.Find( "#", 0 ) == 0 )
			return FALSE;
	}
	else
	if( str.GetLength() == 2 )
	{
		if( str.Find( "#u", 0 ) == 0 )
			return FALSE;
		
		if( str.Find( "#b", 0 ) == 0 )
			return FALSE;
	}
	
	int nFind = str.Find( "#c", 0 );	
	if( nFind != -1 )
	{
		if( (str.GetLength() - nFind) < 8 )
			return FALSE;
	}
		
		return TRUE;
}

BOOL TextCmd_shout( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( pUser->IsMode( SHOUTTALK_MODE ) )
		return FALSE;

	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		return FALSE;

//sun: 12, 군주
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return FALSE;
	}

	if( ( // 미국 & 유럽
		g_xFlyffConfig->GetMainLanguage() == LANG_USA
		|| g_xFlyffConfig->GetMainLanguage() == LANG_ID
		|| g_xFlyffConfig->GetMainLanguage() == LANG_GER
		|| g_xFlyffConfig->GetMainLanguage() == LANG_FRE
		|| g_xFlyffConfig->GetMainLanguage() == LANG_VTN
		|| g_xFlyffConfig->GetMainLanguage() == LANG_POR
		|| g_xFlyffConfig->GetMainLanguage() == LANG_RUS
		)
		&& pUser->GetLevel() < 20 )
	{
		pUser->AddDefinedText( TID_GAME_CANT_SHOUT_BELOW_20 );
		return FALSE;
	}

	if( ( g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_HK )  && pUser->GetLevel() < 15 )
	{
		pUser->AddDefinedText( TID_GAME_CANNOT_SHOUT_BELOW_15_LEVEL );
		return FALSE;
	}

	char szString[1024] = { 0, };

	scanner.GetLastFull();
	if( strlen( scanner.token ) > 260 )	//
		return TRUE;

	FLStrcpy( szString, _countof( szString ), scanner.token );
	StringTrimRight( szString );

	if( !(pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ))))
	{
		ParsingEffect( szString, _countof( szString ), strlen(szString) );
	}
	RemoveCRLF( szString, _countof( szString ) );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
	arBlock << GETID( pUser );
	arBlock.WriteString(pUser->GetName());
	arBlock.WriteString( szString );
//sun: 12, 군주
	DWORD dwColor	= 0xffff99cc;
	if( pUser->HasBuff(  BUFF_ITEM, ITEM_INDEX( 20022, II_SYS_SYS_LS_SHOUT ) ) )
		dwColor		= 0xff00ff00;
	arBlock << dwColor;

	GETBLOCK( arBlock, lpBlock, uBlockSize );

//sun: 13,
	int nRange = 0x000000ff;
	if( pUser->IsShoutFull() )
		nRange = 0;
	g_xWSUserManager->AddShout( pUser, nRange, lpBlock, uBlockSize );

#else	// __WORLDSERVER
#ifdef __CLIENT

	static CTimer timerDobe;
	static CString stringBack;
	static CTimeLimit timeLimit( g_xFlyffConfig->GetShoutLimitCount(), g_xFlyffConfig->GetShoutLimitSecond() );

	CString string = scanner.m_pProg; 

	CString strTrim = string;
	strTrim.TrimLeft();
		
	if( IsRight( strTrim ) == FALSE )
		return FALSE;

	if( g_WndMng.GetBanningTimer().IsTimeOut() == FALSE )
	{
		CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( pWndChat )
		{
			int nOriginalSecond = static_cast< int >( CWndMgr::BANNING_MILLISECOND - static_cast< int >( g_WndMng.GetBanningTimer().GetLeftTime() ) ) / 1000;
			int nMinute = static_cast< int >( nOriginalSecond / 60 );
			int nSecond = static_cast< int >( nOriginalSecond % 60 );
			CString strMessage = _T( "" );
			// 현재 채팅 금지 페널티를 받고 있습니다. (남은 시간: %d분 %d초)
			strMessage.Format( prj.GetText( TID_GAME_ERROR_CHATTING_3 ), nMinute, nSecond );
			pWndChat->PutString( strMessage, 0xffff0000 );
			return FALSE;
		}
	}
	else
	{
		if( stringBack != string || timerDobe.TimeOut() )
		{
			if( !g_pPlayer->IsShoutFull()
				|| g_xFlyffConfig->GetMainLanguage() == LANG_USA
				|| g_xFlyffConfig->GetMainLanguage() == LANG_ID
				|| g_xFlyffConfig->GetMainLanguage() == LANG_GER
				|| g_xFlyffConfig->GetMainLanguage() == LANG_FRE
				|| g_xFlyffConfig->GetMainLanguage() == LANG_JAP
				|| g_xFlyffConfig->GetMainLanguage() == LANG_VTN
				|| g_xFlyffConfig->GetMainLanguage() == LANG_POR
				|| g_xFlyffConfig->GetMainLanguage() == LANG_RUS
				)
			{
				if( g_xFlyffConfig->GetShoutLimitCount() > 0 && timeLimit.Check() == TRUE ) // 제한을 넘었다.
				{
					//외치기는 %n초안에 %d번만 허용됩니다.
					char szMsg[256];
					FLSPrintf( szMsg, _countof( szMsg ), prj.GetText( TID_GAME_LIMIT_SHOUT ), g_xFlyffConfig->GetShoutLimitSecond() / 1000, g_xFlyffConfig->GetShoutLimitCount() );
					g_WndMng.PutString( szMsg, NULL, prj.GetTextColor( TID_GAME_LIMIT_SHOUT ) );
					return FALSE;
				}
			}

			stringBack = string;
			timerDobe.Set( SEC( 3 ) );
			g_WndMng.WordChange( string );

			if( g_WndMng.IsShortcutCommand() == TRUE )
			{
				if( g_WndMng.GetShortcutWarningTimer().IsTimeOut() == FALSE )
				{
					g_WndMng.SetWarningCounter( g_WndMng.GetWarningCounter() + 1 );
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						if( g_WndMng.GetWarningCounter() >= CWndMgr::BANNING_POINT )
						{
							// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
							CString strChattingError1 = _T( "" );
							strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
							pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
							g_WndMng.SetWarningCounter( 0 );
							g_WndMng.GetBanningTimer().Reset();
						}
						else
						{
							// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
							pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
						}
					}
				}
				else
				{
					CString strCommand = _T( "" );
					strCommand.Format( "/s %s", string );
					g_DPlay.SendChat( strCommand );
				}
				g_WndMng.GetShortcutWarningTimer().Reset();
			}
			else
			{
				if( g_WndMng.GetWarningTimer().IsTimeOut() == FALSE )
				{
					g_WndMng.SetWarningCounter( g_WndMng.GetWarningCounter() + 1 );
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						if( g_WndMng.GetWarningCounter() >= CWndMgr::BANNING_POINT )
						{
							// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
							CString strChattingError1 = _T( "" );
							strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
							pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
							g_WndMng.SetWarningCounter( 0 );
							g_WndMng.GetBanningTimer().Reset();
						}
						else
						{
							// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
							pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
						}
					}
				}
				else
				{
					if( g_WndMng.GetWarning2Timer().IsTimeOut() == FALSE )
					{
						g_WndMng.SetWarning2Counter( g_WndMng.GetWarning2Counter() + 1 );
						CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
						if( pWndChat )
						{
							if( g_WndMng.GetWarning2Counter() >= CWndMgr::BANNING_2_POINT )
							{
								// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
								CString strChattingError1 = _T( "" );
								strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
								pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
								g_WndMng.SetWarning2Counter( 0 );
								g_WndMng.GetBanningTimer().Reset();
							}
							else
							{
								CString strCommand = _T( "" );
								strCommand.Format( "/s %s", string );
								g_DPlay.SendChat( strCommand );
							}
						}
					}
					else
					{
						CString strCommand = _T( "" );
						strCommand.Format( "/s %s", string );
						g_DPlay.SendChat( strCommand );
					}
				}
				g_WndMng.GetWarningTimer().Reset();
				g_WndMng.GetWarning2Timer().Reset();
			}
		}
		else
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
			if( pWndChat )
				g_WndMng.PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), NULL, prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
		}
	}

	return FALSE;
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PartyChat( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CHAR lpString[260] = { 0, };	
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		scanner.GetLastFull();
		scanner.Token.TrimLeft();
		scanner.Token.TrimRight();
		g_pEventArena->SendTeamChat( pUser, scanner.Token );
		return TRUE;
	}

//sun: 12, 군주
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
	
	scanner.GetLastFull();
	if( lstrlen( scanner.token ) >= 260 )
		return TRUE;

	FLStrcpy( lpString, _countof( lpString ), scanner.token );

	StringTrimRight( lpString );	

	if( !(pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ))))
	{
		ParsingEffect( lpString, _countof( lpString ), strlen(lpString) );
	}
	RemoveCRLF( lpString, _countof( lpString ) );
	
	CParty* pParty;
	
	pParty	= g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty && pParty->IsMember( pUser->m_idPlayer ))
	{
		// 파티가 있어서 파티원들에게 보냄
		g_DPCoreClient.SendPartyChat( pUser, lpString );
	}
	else
	{
		// 월드서버에서 파티가 없는경우
		pUser->AddSendErrorParty( ERROR_NOPARTY );
	}

	// 클라이언트에서 극단참여중이니즐 먼저 검색함
#else // __WORLDSERVER
#ifdef __CLIENT

	if( !g_pPlayer )
		return FALSE;

	BOOL bParty = g_Party.GetSizeofMember() >= 2;

	//Event arena 에서는 파티챗 사용가능
	if( FALSE == bParty )
	{
		CWorld* pWorld = g_pPlayer->GetWorld();
		bParty = ( pWorld ? pWorld->IsEventArena() : FALSE );
	}

	if( bParty )
	{
		CString string = scanner.m_pProg;
		g_WndMng.WordChange( string );

		if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
			string = '"'+string+'"';
		
		CString strCommand;
		strCommand.Format( "/p %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		// 극단원이 아니므로 
		// 극단에 포함되지 않아 극단채팅을 할수 없습니다.
		//g_WndMng.PutString( "극단에 포함되지 않아 극단채팅을 할수 없습니다", NULL, 0xff99cc00 );
		g_WndMng.PutString( prj.GetText( TID_GAME_PARTYNOTCHAT ), NULL, prj.GetTextColor( TID_GAME_PARTYNOTCHAT ) );
		
	}
	return FALSE;
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_Music( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	u_long idmusic	= scanner.GetNumber();
	g_DPCoreClient.SendPlayMusic( pUser->GetWorld()->GetID(), idmusic );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Sound( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	u_long idsound	= scanner.GetNumber();
	g_DPCoreClient.SendPlaySound( pUser->GetWorld()->GetID(), idsound );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Summon( CScanner& scanner )           
{
#ifdef __WORLDSERVER
	scanner.GetToken();
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	const u_long idSummonPlayer = CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );		//sun: 11, 캐릭터 정보 통합
	if( pUser->m_idPlayer == idSummonPlayer )
	{
		pUser->AddReturnSay( 2, " " );  		// 자기 자신에게 명령했다.
		return TRUE;
	}

	FLWSUser* pDestUser = g_xWSUserManager->GetUserByPlayerID( idSummonPlayer );
	if( IsValidObj( pDestUser ) == FALSE )
	{
		pUser->AddReturnSay( 3, scanner.Token );
		return TRUE;
	}

	pUser->SummonPlayer( pDestUser );
#endif	// __WORLDSERVER
	return TRUE;
}

//sun: ?, __PET_1024
BOOL TextCmd_ClearPetName( CScanner & /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendClearPetName();
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_SetPetName( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CPet* pPet	= pUser->GetPet();
	if( !pPet )
		return TRUE;
	s.GetToken();
	pPet->SetName( s.token );
	g_xWSUserManager->AddSetPetName( pUser, s.token );
#endif	// __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_CreateLayer( CScanner & s )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
//		pWorld->m_linkMap.CreateLinkMap( nLayer );
		pWorld->CreateLayer( nLayer );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DeleteLayer( CScanner & s )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
//		pWorld->m_linkMap.DeleteLinkMap( nLayer );
		pWorld->ReleaseLayer( nLayer );		// do not call ReleaseLayer directly
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Layer( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
		CLinkMap* pLinkMap	= pWorld->m_linkMap.GetLinkMap( nLayer );
		if( pLinkMap && pLinkMap->IsInvalid() == FALSE )
		{
			FLOAT x	= s.GetFloat();
			FLOAT z	= s.GetFloat();
			if( pWorld->VecInWorld( x, z ) && x > 0 && z > 0 )	
				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), D3DXVECTOR3( x, 0, z ), REPLACE_NORMAL, nLayer );
			else
				pUser->AddText( "OUT OF WORLD" );
		}
		else
		{
			pUser->AddText( "LAYER NO EXISTS" );
		}
	}
	else
	{
		pUser->AddText( "UNDEFINED WORLD" );
	}
#endif	// __WORLDSERVER
	return TRUE;
}


//sun: 13, 커플 시스템
//sun: 13, 커플 보상
BOOL TextCmd_NextCoupleLevel( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( pUser->m_idPlayer );
	if( pCouple )
	{
		if( pCouple->GetLevel() < CCouple::eMaxLevel )
		{
			int nExperience	= CCoupleProperty::Instance()->GetTotalExperience( pCouple->GetLevel() + 1 ) - pCouple->GetExperience();
			g_dpDBClient.SendQueryAddCoupleExperience( pUser->m_idPlayer, nExperience );
		}
		else
			pUser->AddText( "MAX COUPLE LEVEL" );
	}
	else
		pUser->AddText( "COUPLE NOT FOUND" );
#endif	// __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_Propose( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	s.GetToken();
	CCoupleHelper::Instance()->OnPropose( pUser, s.token );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Refuse( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CCoupleHelper::Instance()->OnRefuse( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Couple( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CCoupleHelper::Instance()->OnCouple( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Decouple( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CCoupleHelper::Instance()->OnDecouple( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ClearPropose( CScanner & /*s*/ )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	g_dpDBClient.SendClearPropose();
#endif	// __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_CoupleState( CScanner & s )
{
#ifdef __CLIENT
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple();
	if( pCouple )
	{
		char szText[200]	= { 0,};
		const char* pszPartner	= CPlayerDataCenter::GetInstance()->GetPlayerString( pCouple->GetPartner( g_pPlayer->m_idPlayer ) );
		if( !pszPartner )	pszPartner	= "";
		FLSPrintf( szText, _countof( szText ), "%s is partner.", pszPartner );
		g_WndMng.PutString( szText );
	}
	else
	{
		g_WndMng.PutString( "null couple." );
	}
#endif	// __CLIENT
	return TRUE;
}
*/

#ifdef	__INTERNALSERVER
BOOL	TextCmd_SetAdmin( CScanner& kScanner )		// 관리자 & 일반 모드 토글 기능
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = reinterpret_cast< FLWSUser* >( kScanner.dwValue );
	if( IsValidObj( pUser ) )
	{
//		pUser->m_dwAuthorization = AUTH_ADMINISTRATOR;

		pUser->m_dwAuthorization	= pUser->m_dwAuthorization == AUTH_ADMINISTRATOR ? AUTH_GENERAL : AUTH_ADMINISTRATOR;

		FLSnapshotSetAuthorizationNoti kPacket;
		kPacket.dwAuthorization = pUser->m_dwAuthorization;
		pUser->AddPacket( &kPacket );

		if( pUser->m_dwAuthorization == AUTH_ADMINISTRATOR )
		{
			pUser->AddText( "관리자 모드로 변경 되었습니다" );
		}
		else
		{
			pUser->AddText( "일반 모드로 변경 되었습니다" );
		}
	}
#endif
	return TRUE;
}

BOOL	TextCmd_SetGeneralUser( CScanner& kScanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = reinterpret_cast< FLWSUser* >( kScanner.dwValue );
	if( IsValidObj( pUser ) )
	{
		pUser->m_dwAuthorization = AUTH_GENERAL;

		FLSnapshotSetAuthorizationNoti kPacket;
		kPacket.dwAuthorization = pUser->m_dwAuthorization;
		pUser->AddPacket( &kPacket );

		pUser->AddText( "일반 유저로 변경 되었습니다." );
	}
#endif
	return TRUE;
}
#endif

BOOL TextCmd_Teleport( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
//	TCHAR *lpszPlayer = NULL;
	int x, z, y = 0;
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	// 플레이어에게 바로 텔레포트 
	int nTok = scanner.GetToken();
	if( nTok != NUMBER )
	{
		// player
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );		//sun: 11, 캐릭터 정보 통합

		FLWSUser* pUserTarget = static_cast<FLWSUser*>( prj.GetUserByID( idPlayer ) );
		if( IsValidObj( pUserTarget ) )
		{
			CWorld* pWorld = pUserTarget->GetWorld();
			if( pWorld )
			{
				if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
					if( pWorld != pUser->GetWorld() || pUser->GetLayer() != pUserTarget->GetLayer() )
						return TRUE;

				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pUserTarget->GetPos(), REPLACE_NORMAL, pUserTarget->GetLayer() );
			}
		}
		else 
		{
			// 플레이어를 못찾으면 NPC로 찾는다.
			CWorld* pWorld	= pUser->GetWorld();
			CMover* pMover = pWorld->FindMover( scanner.Token );
			if( pMover )
			{
				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pMover->GetPos(), REPLACE_NORMAL, pMover->GetLayer() );
				return TRUE;
			}
			pUser->AddReturnSay( 3, scanner.m_mszToken );
		}
	}
	// 첫번째 파라메타는 월드 번호.
	DWORD dwWorldId = atoi( scanner.token );

	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId ) )
	{
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() != dwWorldId )
			return TRUE;
	}

	if( g_WorldMng.GetWorldStruct( dwWorldId ) )
	{
		// 두번째 파라메타가 스트링이면 리젼 키
		if( scanner.GetToken() != NUMBER )
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( dwWorldId, scanner.token );
			if( NULL != pRgnElem )
				pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
		}
		// 스트링이 아니면 좌표 
		else
		{
			x = atoi( scanner.token );
			z = scanner.GetNumber();
			y = scanner.GetNumber();

			CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
			if( pWorld && pWorld->VecInWorld( (FLOAT)( x ), (FLOAT)( z ) ) && x > 0 && z > 0 )
			{
				int nLayer	= pWorld == pUser->GetWorld()? pUser->GetLayer(): nDefaultLayer;
				pUser->REPLACE( g_uIdofMulti, dwWorldId, D3DXVECTOR3( (FLOAT)x, (FLOAT)y, (FLOAT)z ), REPLACE_NORMAL, nLayer );
			}
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Out( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
//	TCHAR lpszPlayer[MAX_PLAYER];
	scanner.GetToken();

	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( strcmp( pUser->GetName(), scanner.Token) )
	{	
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );	//sun: 11, 캐릭터 정보 통합

		if( idPlayer > 0 ) {
			g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, idPlayer );
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RemoveNpc( CScanner& s )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	OBJID objid	= (OBJID)s.GetNumber();

	CMover* pMover	= prj.GetMover( objid );
	if( IsValidObj( (CObj*)pMover ) )
	{
		if( pMover->IsNPC() )
			pMover->Delete();
	}
#endif	// __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_CreateItem2( CScanner & s )
{
#ifdef __WORLDSERVER
	s.GetToken();
	PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( s.Token );
	if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL )
	{
		int nRandomOptItemId	= s.GetNumber();
		PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( nRandomOptItemId );
		if( pRandomOptItem )
		{
			FLItemElem itemElem;
			itemElem.m_dwItemId		= pItemProp->dwID;
			itemElem.m_nItemNum	= 1;
			//itemElem.m_nHitPoint	= -1;
			itemElem.SetRandomOptionOriginID( static_cast< WORD >( pRandomOptItem->nId ) );
			FLWSUser* pUser	= (FLWSUser*)s.dwValue;
			pUser->CreateItem( &itemElem );
		}
	}
	return TRUE;
#else	// __WORLDSERVER
	return TRUE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_CreateItem( CScanner& scanner )       
{
	scanner.GetToken();

#ifdef __CLIENT
	// 클라이언트에서
	if( scanner.tok == FINISHED )
	{
		if( g_WndMng.GetWndBase( APP_ADMIN_CREATEITEM ) == NULL )
		{
			CWndAdminCreateItem* pWndAdminCreateItem = new CWndAdminCreateItem;
			pWndAdminCreateItem->Initialize();
		}
		return FALSE;
	}
	return TRUE;
#else   // __CLIENT
	DWORD dwNum;
	DWORD dwCharged		= 0;
	PT_ITEM_SPEC pProp	= NULL;

	if( scanner.tokenType == NUMBER )
		pProp	= g_xSpecManager->GetSpecItem( _ttoi( scanner.Token ) );
	else {
		const int defineNumber = CScript::GetDefineNum( scanner.Token );
		if( defineNumber != -1 ) {
			pProp	= g_xSpecManager->GetSpecItem( defineNumber );
		}
		else {
			pProp	= g_xSpecManager->GetSpecItem( scanner.Token );
		}
	}

	if( pProp && pProp->dwItemKind3 != IK3_VIRTUAL )
	{
		if( pProp->dwItemKind3 == IK3_EGG && pProp->dwID != ITEM_INDEX( 21029, II_PET_EGG ) )	// 리어펫을 생성하려고 할 경우 "알"인 경우만 생성 가능하다.
			return TRUE;

		dwNum	= scanner.GetNumber();
		dwNum	= ( dwNum == 0? 1: dwNum );
		dwCharged	= scanner.GetNumber();
		dwCharged	= ( dwCharged == 0 ? 0 : 1 );
		
		FLItemElem itemElem;
		itemElem.m_dwItemId		= pProp->dwID;
		itemElem.m_nItemNum		= (int)( dwNum );
		//itemElem.m_nHitPoint	= -1;
		itemElem.m_bCharged		= dwCharged;

		if( pProp->dwItemKind3 == IK3_EGG && pProp->dwID == ITEM_INDEX( 21039, II_PET_WHITETIGER01 ) )
		{	// 여기는 안들어옵니다. 위에 주석처리해야함.
			SAFE_DELETE( itemElem.m_pPet );
			itemElem.m_pPet= new CPet;
			int nPetLevel = 3;
			itemElem.m_pPet->SetKind(1);
			itemElem.m_pPet->SetLevel(nPetLevel);
			itemElem.m_pPet->SetExp(0);
			itemElem.m_pPet->SetEnergy( itemElem.m_pPet->GetMaxEnergy() );
			BYTE anAvail[PL_MAX - 1]	= {1,3,5,7,9};
			for( BYTE i = PL_D; i <= nPetLevel; i++ )
				itemElem.m_pPet->SetAvailLevel( i, anAvail[i-1] );
			for( BYTE i = nPetLevel + 1; i <= PL_S; i++ )
				itemElem.m_pPet->SetAvailLevel( i, 0 );

			itemElem.m_pPet->SetLife( 1 );
		}

		FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
		pUser->CreateItem( &itemElem );

		FLINFO_LOG( PROGRAM_NAME, _T( "TextCmd_CreateItem UserName : %s / Item ID : %d / NUM : %d / Charge : %d" ), pUser->GetName(), itemElem.m_dwItemId, itemElem.m_nItemNum, itemElem.m_bCharged );
	}
#endif	// !__CLIENT 
	return TRUE;
}

BOOL TextCmd_LocalEvent( CScanner & s )
{
#ifdef __WORLDSERVER
	int id	= s.GetNumber();
	if( id != EVE_18 )	// 이 식별자는 18세 서버를 나타내는 식별자므로 운영자의 명령에 의한 설정 불가
	{
		BYTE nState		= static_cast< BYTE >( s.GetNumber() );
		if( g_eLocal.SetState( id, nState ) )		g_xWSUserManager->AddSetLocalEvent( static_cast< short >( id ), nState );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

// 무버이름 캐릭터키 갯수 선공 
BOOL TextCmd_CreateChar( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	D3DXVECTOR3 vPos = pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	scanner.GetToken();
	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pMover ) return FALSE; // FLASSERT( pObj );
			FLStrcpy( pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ), strName );
			pMover->InitNPCProperty();
			pMover->InitCharacter( pMover->GetCharacter() );
			pMover->SetPos( vPos );
			pMover->InitMotion( MTI_STAND );
			pMover->UpdateLocalMatrix();
			if( bActiveAttack )
				pMover->m_bActiveAttack = bActiveAttack;
			if( pWorld->ADDOBJ( pMover, TRUE, pUser->GetLayer() ) == FALSE )
			{
				SAFE_DELETE( pMover );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CreateCtrl( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	DWORD dwID	= s.GetNumber();
	
	if( dwID == 0 )
		return FALSE;

	CCtrl* pCtrl	= (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, dwID );
	if( !pCtrl )
		return FALSE;

	pCtrl->SetPos( vPos );
	if( pWorld->ADDOBJ( pCtrl, TRUE, pUser->GetLayer() ) == FALSE )
	{
		SAFE_DELETE( pCtrl );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __CLIENT
BOOL TextCmd_PostMail( CScanner & s )
{

	DWORD dwIndex	= s.GetNumber();
	int nItemNum	= s.GetNumber();
	char lpszReceiver[MAX_PLAYER]	= { 0, };
	s.GetToken();
	FLStrcpy( lpszReceiver, _countof( lpszReceiver ), s.Token );
	int nGold	= s.GetNumber();
	char lpszTitle[MAX_MAILTITLE]	= { 0, };
	s.GetToken();
	FLStrcpy( lpszTitle, _countof( lpszTitle ), s.Token );
	char lpszText[MAX_MAILTEXT]	= { 0, };
	s.GetToken();
	FLStrcpy( lpszText, _countof( lpszText ), s.Token );

	FLItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAt( dwIndex );
	if( pItemElem )
	{
		g_DPlay.SendQueryPostMail( pItemElem->m_dwObjId, nItemNum, lpszReceiver, nGold, lpszTitle, lpszText );
	}

	return TRUE;
}
#else
BOOL TextCmd_PostMail( CScanner & /*s*/ )	{	return TRUE; }
#endif	// __CLIENT

#ifdef __CLIENT
BOOL TextCmd_RemoveMail( CScanner & s )
{

	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryRemoveMail( nMail );

	return TRUE;
}
#else
BOOL TextCmd_RemoveMail( CScanner & /*s*/ )		{ return TRUE; }
#endif	// __CLIENT


#ifdef __CLIENT
BOOL TextCmd_GetMailItem( CScanner & s )
{

	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryGetMailItem( nMail );

	return TRUE;
}
#else
BOOL TextCmd_GetMailItem( CScanner & /*s*/ )			{ return TRUE; }
#endif	// __CLIENT

#ifdef __CLIENT
BOOL TextCmd_GetMailGold( CScanner & s )
{
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryGetMailGold( nMail );
	return TRUE;
}
#else
BOOL TextCmd_GetMailGold( CScanner & /*s*/ )		{	return TRUE; }
#endif	// __CLIENT

//sun: 9, 이벤트 (루아 스크립트 적용)
BOOL TextCmd_Lua( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	s.GetToken();
	s.Token.MakeLower();
		
	if( s.Token == "event" )
	{
		pUser->AddText( "Event.lua Reload..." );
		FLINFO_LOG( PROGRAM_NAME, _T( "Event.lua Reload... - %s" ), pUser->GetName() );
		g_dpDBClient.SendEventLuaChanged();
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LuaEventList( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	prj.m_EventLua.GetAllEventList( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LuaEventInfo( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	prj.m_EventLua.GetEventInfo( pUser, s.GetNumber() );
#endif // __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_Mute( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;

	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );		//sun: 11, 캐릭터 정보 통합

	if( idPlayer == 0 )
	{
		pUser->AddText( "player not found" );
		return TRUE;
	}
	FLWSUser* pTarget	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( IsValidObj( pTarget ) )
	{
		DWORD dwMute	= (DWORD)s.GetNumber();
		if( s.tok == FINISHED )
			return TRUE;
		pTarget->m_dwMute	= dwMute;
	}
#endif	// __WORLDSERVER
	return TRUE;
}

//sun: 8차 엔젤 소환 Neuz, World, Trans
BOOL TextCmd_AngelExp( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	int nAddExp	= s.GetNumber();

	if( pUser->HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
		IBuff* pBuff	= pUser->m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
		if( wId )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( wId );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal[0] );
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;

		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[pUser->m_nAngelLevel].nExp1 / 100 * nAngel;
		if( pUser->m_nAngelExp < nMaxAngelExp )
		{
			pUser->m_nAngelExp += nAddExp;
			BOOL bAngelComplete = FALSE;
			if( pUser->m_nAngelExp > nMaxAngelExp )
			{
				pUser->m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			pUser->AddAngelInfo( bAngelComplete );
		}
	}
#endif // __WORLDSERVER
	return TRUE;	
}


BOOL TextCmd_CallTheRoll( CScanner& s )  
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	int nCount	= s.GetNumber();

	pUser->m_nEventFlag = 0;		//sun 10, __EVENT_1101_2

	pUser->m_dwEventTime	= 0;
	pUser->m_dwEventElapsed	= 0;
	for(  int i = 0; i < nCount; i++ )
		pUser->SetEventFlagBit( 62 - i );	//sun 10, __EVENT_1101_2

#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CreatePc( CScanner& scanner )  
{ 
#ifdef __PERF_0226
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	int nNum = scanner.GetNumber();
	for( int i=0; i<nNum; i++ )
	{
		int nSex	= xRandom( 2 );
		DWORD dwIndex	= ( nSex == SEX_FEMALE? OBJECT_INDEX( 12, MI_FEMALE ): OBJECT_INDEX( 11, MI_MALE ) );

		CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, dwIndex );
		if( NULL == pMover )	
			return FALSE;
		pMover->SetPos( pUser->GetPos() );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		SAFE_DELETE( pMover->m_pAIInterface );
		SAFE_DELETE( pMover->m_pFSM );
		pMover->SetAIInterface( AII_MONSTER );
		pMover->m_Inventory.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_INVENTORY, MAX_INVENTORY, MAX_HUMAN_PARTS ); 

		static DWORD adwParts[5]	= {	PARTS_CAP, PARTS_HAND, PARTS_UPPER_BODY, PARTS_FOOT, PARTS_RWEAPON };
		for( int i = 0; i < 5; i++ )
		{
			FLItemElem itemElem;
			PT_ITEM_SPEC pProp	= CPartsItem::GetInstance()->GetItemProp( ( i == 4? SEX_SEXLESS: nSex ), adwParts[i] );
			if( pProp )
			{
				FLItemElem	itemElem;
				itemElem.m_dwItemId	= pProp->dwID;
				itemElem.m_nItemNum	= 1;
				itemElem.SetAbilityOption( xRandom( 10 ) );
				
				DWORD adwItemObjID[MAX_INVENTORY] = { NULL_ID, };
				int anNum[MAX_INVENTORY] = { 0, };
				DWORD dwCount = 0;
				
				itemElem.SetSerialNumber();
				pMover->m_Inventory.Add( &itemElem, adwItemObjID, anNum, &dwCount );

				for( DWORD dwNth = 0; dwNth < dwCount; ++dwNth )
				{
					FLItemElem* pAddItem	= pMover->m_Inventory.GetAtId( adwItemObjID[ dwNth ] );
					if( pAddItem != NULL )
					{
						pMover->m_Inventory.DoEquip( pAddItem->m_dwObjIndex, pProp->dwParts );
					}
				}
			}
		}
		if( pUser->GetWorld()->ADDOBJ( pMover, TRUE, pUser->GetLayer() ) == FALSE )
		{
			SAFE_DELETE( pMover );
		}
	}
#endif	// __WORLDSERVER
#endif	// __PERF_0226
	return TRUE;
}

BOOL TextCmd_CreateNPC( CScanner& scanner )  
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	CString strName = scanner.Token;

	//@@@@@@@@@@@@@@@@@@ MOVER 속성으로 구분해서 몬스터인것만 생성 해야할 듯.
	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		if( pMoverProp->dwAI != AII_MONSTER
			&& pMoverProp->dwAI != AII_CLOCKWORKS
			&& pMoverProp->dwAI != AII_BIGMUSCLE
			&& pMoverProp->dwAI != AII_KRRR
			&& pMoverProp->dwAI != AII_BEAR
			&& pMoverProp->dwAI != AII_METEONYKER
#ifdef __AGGRO16
			&& pMoverProp->dwAI != AII_AGGRO_NORMAL
#endif // __AGGRO16
			&& pMoverProp->dwAI != AII_PARTY_AGGRO_LEADER
			&& pMoverProp->dwAI != AII_PARTY_AGGRO_SUB
			&& pMoverProp->dwAI != AII_ARENA_REAPER
		)
			return TRUE;

		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	
				return FALSE;	
			pObj->SetPos( vPos );
			pObj->InitMotion( MTI_STAND );
			pObj->UpdateLocalMatrix();

			if( bActiveAttack )
				((CMover*)pObj)->m_bActiveAttack = bActiveAttack;
			
			((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정
			if( pWorld->ADDOBJ( pObj, TRUE, pUser->GetLayer() ) == FALSE )
			{
				SAFE_DELETE( pObj );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Invisible( CScanner& scanner )        
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode |= TRANSPARENT_MODE;
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoInvisible( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode &= (~TRANSPARENT_MODE);
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoUndying( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	g_xWSUserManager->AddModifyMode( pUser );
#endif
	return TRUE;
}

// exp상승 금지 명령. 토글방식으로 동작.
BOOL TextCmd_ExpUpStop( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	if( pUser->m_dwMode & MODE_EXPUP_STOP )
		pUser->m_dwMode &= (~MODE_EXPUP_STOP);
	else
		pUser->m_dwMode |= MODE_EXPUP_STOP;
	
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	
	return TRUE;
}
BOOL TextCmd_PartyInvite( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
//	MoverProp* pMoverProp = NULL;
	scanner.GetToken();

	u_long uidPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );	//sun: 11, 캐릭터 정보 통합

	if( 0 < uidPlayer )
	{
		FLWSUser* pUser2	= g_xWSUserManager->GetUserByPlayerID( uidPlayer );	
		if( IsValidObj( pUser2 ) )
		{
			CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
			if( pParty != NULL )	// 기존 극단이 있으면 극단장인지 체크..
			{
				if( pParty->IsLeader( pUser->m_idPlayer ) == TRUE )
				{
					g_DPSrvr.InviteParty( pUser->m_idPlayer, pUser2->m_idPlayer, FALSE );
				}
				else
				{
					pUser->AddDefinedText( TID_DIAG_0007 );
				}
			}
			else	// 최초 극단결성 시..
			{
				g_DPSrvr.InviteParty( pUser->m_idPlayer, pUser2->m_idPlayer, FALSE );
			}
		}
		else
		{
			pUser->AddDefinedText( TID_DIAG_0060, "\"%s\"", scanner.Token );
		}
	}
	else
	{
		pUser->AddDefinedText( TID_DIAG_0061, "\"%s\"", scanner.Token );
	}
#endif // __WORLDSERVER

	return TRUE;
}
BOOL TextCmd_GuildInvite( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
//	MoverProp* pMoverProp = NULL;
	scanner.GetToken();

	u_long uidPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );		//sun: 11, 캐릭터 정보 통합

	if( 0 < uidPlayer )
	{
		FLWSUser* pUser2	= g_xWSUserManager->GetUserByPlayerID( uidPlayer );	
		if( IsValidObj( pUser2 ) )
		{
			g_DPSrvr.InviteCompany( pUser, pUser2->GetId() );
		}
		else
			pUser->AddDefinedText( TID_DIAG_0060, "\"%s\"", scanner.Token );
	}
	else
	{
		pUser->AddDefinedText( TID_DIAG_0061, "\"%s\"", scanner.Token );
	}
#endif // __WORLDSERVER

	return TRUE;
}

BOOL bCTDFlag	= FALSE;

BOOL TextCmd_CTD( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	if( g_eLocal.GetState( EVE_WORMON ) == 0 )
	{
		CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
		CRect* pRect	= pProcessor->GetQuestRect( QUEST_INDEX( 1, QUEST_WARMON_LV1 ) );
		if( pRect )
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "recv /ctd" ) );
			REGIONELEM re = { 0 };
			//mem_set( &re, 0, sizeof(re) );
			re.m_uItemId	= 0xffffffff;
			re.m_uiItemCount	= 0xffffffff;
			re.m_uiMinLevel	= 0xffffffff;
			re.m_uiMaxLevel	= 0xffffffff;
			re.m_iQuest	= 0xffffffff;
			re.m_iQuestFlag	= 0xffffffff;
			re.m_iJob	= 0xffffffff;
			re.m_iGender	= 0xffffffff;
			re.m_dwAttribute	= RA_DANGER | RA_FIGHT;
			re.m_dwIdMusic	= 121;
			re.m_bDirectMusic	= TRUE;
			re.m_dwIdTeleWorld	= 0;
			re.m_rect.SetRect( pRect->TopLeft(), pRect->BottomRight() );
			FLStrcpy( re.m_szTitle, _countof( re.m_szTitle ), "Duel Zone" );

			CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
			if( pWorld )
			{
				LPREGIONELEM ptr	= pWorld->m_aRegion.GetAt( pWorld->m_aRegion.GetSize() - 1 );
				if( ptr->m_dwAttribute != ( RA_DANGER | RA_FIGHT ) )
					pWorld->m_aRegion.AddTail( &re );
				pUser->AddText( "recv /ctd" );
				g_xWSUserManager->AddAddRegion( WI_WORLD_MADRIGAL, re );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Undying( CScanner& scanner )          
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
	g_xWSUserManager->AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Undying2( CScanner& scanner )          
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
	g_xWSUserManager->AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
#endif
#endif
	return TRUE;
}


BOOL TextCmd_NoDisguise( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->NoDisguise( NULL );
	g_xWSUserManager->AddNoDisguise( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __WORLDSERVER
BOOL DoDisguise( FLWSUser* pUser, DWORD dwIndex )
{
	pUser->Disguise( NULL, dwIndex );
	g_xWSUserManager->AddDisguise( pUser, dwIndex );
	return TRUE;
}
#endif // __WORLDSERVER


BOOL TextCmd_Disguise( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp )
		DoDisguise( pUser, pMoverProp->dwID );
#endif
	return TRUE;
}
BOOL TextCmd_Freeze( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
//sun: 11, 캐릭터 정보 통합
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );

		if( idFrom > 0 && idTo > 0 ) 
		{
			// 1 : 추가 m_dwMode
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)1, idFrom, idTo );					
		}
		else 
		{
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoFreeze( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
//sun: 11, 캐릭터 정보 통합
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );

		if( idFrom > 0 && idTo > 0 ) 
		{
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)0, idFrom, idTo );	// 0 : 뺌 m_dwMode
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_Talk( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	scanner.GetToken();

	u_long idFrom, idTo;
//sun: 11, 캐릭터 정보 통합
	idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
	idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );

	if( idFrom > 0 && idTo > 0 ) 
	{
		g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)0, idFrom, idTo );	// 0 : 뺌 m_dwMode
	}
	else 
	{
		//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
		pUser->AddReturnSay( 3, scanner.Token );
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoTalk( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER

	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	
	scanner.GetToken();

	{
		u_long idFrom, idTo;
//sun: 11, 캐릭터 정보 통합
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );

		if( idFrom > 0 && idTo > 0 ) 
		{
			g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)1, idFrom, idTo );	// 1 : 추가
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_GetGold( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	int nGold = scanner.GetNumber();	
	pUser->AddGold( nGold );
#endif	// __WORLDSERVER	
	return TRUE;
}

// /간접 npcId "대사"
BOOL TextCmd_indirect( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
//	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	DWORD dwIdNPC = scanner.GetNumber();
	TCHAR szString[ 1024 ];

	scanner.GetLastFull();
	if( strlen( scanner.token ) > 260 )
		return TRUE;
	FLStrcpy( szString, _countof( szString ), scanner.token );
	StringTrimRight( szString );

	if( szString[ 0 ] )
	{
		CMover* pMover = prj.GetMover( dwIdNPC );
		if( pMover )
			g_xWSUserManager->AddChat( (CCtrl*)pMover, (LPCSTR)szString );
	}
#else // __WORLDSERVER
	scanner.GetToken();
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		if( scanner.tok == FINISHED )
		{
			if( g_WndMng.GetWndBase( APP_ADMIN_INDIRECT_TALK ) == NULL )
			{
				CWndIndirectTalk* pWndIndirectTalk = new CWndIndirectTalk;
				pWndIndirectTalk->Initialize();
			}
			return FALSE;
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetMode( ITEM_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetNotMode( ITEM_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_AttackMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetMode( NO_ATTACK_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_EscapeReset( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	CMover* pMover = prj.GetUserByID( scanner.GetNumber() );
	if( IsValidObj( pMover ) ) {
		pMover->SetSMMode( SM_ESCAPE, 0 );
	}
	else {
		pUser->SetSMMode( SM_ESCAPE, 0 );
	}
#else	// __WORLDSERVER
#ifdef __CLIENT
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/EscapeReset %d", pMover->m_idPlayer  );
		}

		g_DPlay.SendChat( (LPCSTR)strSend );

		return FALSE;
	}
#endif //__CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}



BOOL TextCmd_AttackNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetNotMode( NO_ATTACK_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetMode( COMMUNITY_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetNotMode( COMMUNITY_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetMode( OBSERVE_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->SetNotMode( OBSERVE_MODE );
	g_xWSUserManager->AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Onekill( CScanner& scanner )          
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
	g_xWSUserManager->AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Position( CScanner& /*scanner*/ )          
{ 
#ifdef __CLIENT
	CString string;
	D3DXVECTOR3 vPos = g_pPlayer->GetPos();
	//string.Format( "현재좌표 : x = %f, y = %f, z = %f", vPos.x, vPos.y, vPos.z );
	string.Format( prj.GetText(TID_GAME_NOWPOSITION), vPos.x, vPos.y, vPos.z );
	g_WndMng.PutString( string, NULL, prj.GetTextColor( TID_GAME_NOWPOSITION ) );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_NoOnekill( CScanner& scanner )          
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
	g_xWSUserManager->AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
#endif
#endif
	return TRUE;
}
BOOL TextCmd_ip( CScanner& scanner )               
{ 
#ifdef __WORLDSERVER
	scanner.GetToken();

	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;

	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );		//sun: 11, 캐릭터 정보 통합

	if( idPlayer > 0 )
		g_DPCoreClient.SendGetPlayerAddr( pUser->m_idPlayer, idPlayer );
	else 
		pUser->AddReturnSay( 3, scanner.Token );
#else	// __WORLDSERVER
	#ifdef __CLIENT
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/ip %s", pMover->GetName() );
		}
		else
		{
			strSend.Format( "%s", scanner.m_pBuf );
		}
		g_DPlay.SendChat( (LPCSTR)strSend );
		return FALSE;
	}
	#endif //__CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_userlist( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	g_DPCoreClient.SendGetCorePlayer( pUser->m_idPlayer );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_count( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	g_DPCoreClient.SendGetPlayerCount( pUser->m_idPlayer );

	char szCount[128]	= { 0, };
	FLSPrintf( szCount, _countof( szCount ), "%d", g_xWSUserManager->GetCount() );
	pUser->AddText( szCount );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_System( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CHAR szString[512] = "";

//	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	scanner.GetLastFull();
	if( strlen( scanner.token ) >= 512 )
		return TRUE;
	FLStrcpy( szString, _countof( szString ), scanner.token );
	StringTrimRight( szString );

	g_DPCoreClient.SendSystem( szString );
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __WORLDSERVER
BOOL TextCmd_LoadScript( CScanner & scanner )
{
	#if defined(__REMOVE_SCIRPT_060712)
		UNREFERENCED_PARAMETER( scanner );
		if( CWorldDialog::GetInstance().Reload() == FALSE )
			FLERROR_LOG( PROGRAM_NAME, _T( "WorldScript.dll reload error" ) );
	#else
		FLWSUser* pUser	 = (FLWSUser*)scanner.dwValue;
		pUser->GetWorld()->LoadAllMoverDialog();
	#endif
	return TRUE;
}
#else
BOOL TextCmd_LoadScript( CScanner & /*scanner*/ )		{ return TRUE; }
#endif	//__WORLDSERVER

BOOL TextCmd_FallEffect( CScanner & /*scanner*/ )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	 = (FLWSUser*)scanner.dwValue;
	g_DPCoreClient.SendSeasonEffectByGM( true );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StopEffect( CScanner & /*scanner*/ )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	 = (FLWSUser*)scanner.dwValue;
	g_DPCoreClient.SendSeasonEffectByGM( false );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_JobName( CScanner & /*scanner*/ )
{
#ifdef __CLIENT
	char chMessage[MAX_PATH] = {0,};
	FLSPrintf( chMessage, _countof( chMessage ), "Expert Job : " );
	for( int i = MAX_JOBBASE ; i < MAX_EXPERT ; ++i )
	{
		if( strlen( prj.m_aJob[i].szName ) < 15 )
		{
			FLStrcat( chMessage, _countof( chMessage ), prj.m_aJob[i].szName );
			if( i + 1 != MAX_EXPERT )
			{
				FLStrcat( chMessage, _countof( chMessage ), ", ");
			}
		}
	}
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
	FLSPrintf( chMessage, _countof( chMessage ), "Expert Level : %d ~ %d", MAX_JOB_LEVEL + 1, MAX_JOB_LEVEL + MAX_EXP_LEVEL );
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );

	FLSPrintf( chMessage, _countof( chMessage ), "Professional Job : " );
	for( int i = MAX_EXPERT ; i < MAX_PROFESSIONAL ; ++i )
	{
		if( strlen( prj.m_aJob[i].szName ) < 15 )
		{
			FLStrcat( chMessage, _countof( chMessage ), prj.m_aJob[i].szName );
			if( i + 1 != MAX_PROFESSIONAL )
			{
				FLStrcat( chMessage, _countof( chMessage ), ", ");
			}
		}
	}
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
	FLSPrintf( chMessage, _countof( chMessage ), "Professional Level : %d ~~~ ", MAX_JOB_LEVEL + MAX_EXP_LEVEL );
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
#endif // __CLIENT
	return TRUE;
}

#ifdef __CLIENT

BOOL TextCmd_tradeagree( CScanner & scanner )
{
	g_Option.m_bTrade = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEAGREE ), NULL, prj.GetTextColor( TID_GAME_TRADEAGREE ) );
	return TRUE;
}
BOOL TextCmd_traderefuse( CScanner & scanner )
{
	g_Option.m_bTrade = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEREFUSE ), NULL, prj.GetTextColor( TID_GAME_TRADEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_whisperagree( CScanner & scanner )
{
	g_Option.m_bSay = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERAGREE ), NULL, prj.GetTextColor( TID_GAME_WHISPERAGREE ) );
	return TRUE;
}
BOOL TextCmd_whisperrefuse( CScanner & scanner )
{
	g_Option.m_bSay = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERREFUSE ), NULL, prj.GetTextColor( TID_GAME_WHISPERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_messengeragree( CScanner & scanner )
{
	g_Option.m_bMessenger = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERAGREE ), NULL, prj.GetTextColor( TID_GAME_MSGERAGREE ) );
	return TRUE;
}
BOOL TextCmd_messengerrefuse( CScanner & scanner )
{
	g_Option.m_bMessenger = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERREFUSE ), NULL, prj.GetTextColor( TID_GAME_MSGERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_stageagree( CScanner & scanner )
{
	g_Option.m_bParty = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEAGREE ), NULL, prj.GetTextColor( TID_GAME_STAGEAGREE ) );
	return TRUE;
}
BOOL TextCmd_stagerefuse( CScanner & scanner )
{
	g_Option.m_bParty = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEREFUSE ), NULL, prj.GetTextColor( TID_GAME_STAGEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_connectagree( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNAGREE ), NULL, prj.GetTextColor( TID_GAME_CONNAGREE ) );
	return TRUE;
}
BOOL TextCmd_connectrefuse( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNREFUSE ), NULL, prj.GetTextColor( TID_GAME_CONNREFUSE ) );
	return TRUE;
}
BOOL TextCmd_shoutagree( CScanner & scanner )
{
	g_Option.m_bShout = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTAGREE ), NULL, prj.GetTextColor( TID_GAME_SHOUTAGREE ) );
	return TRUE;
}
BOOL TextCmd_shoutrefuse( CScanner & scanner )
{
	g_Option.m_bShout = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTREFUSE ), NULL, prj.GetTextColor( TID_GAME_SHOUTREFUSE ) );
	return TRUE;
}

BOOL TextCmd_BlockUser( CScanner & scanner )
{
	if( prj.m_setBlockedUserID.size() >= CProject::BLOCKING_NUMBER_MAX )
	{
		// 차단 가능한 유저 수를 초과하였습니다. 차단 목록을 정리한 후에 다시 사용해 주십시오.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_FULL_BLOCKED_USER_LIST ), NULL, prj.GetTextColor( TID_GAME_ERROR_FULL_BLOCKED_USER_LIST ) );
		return FALSE;
	}
	scanner.GetToken();
	CString strUserName = scanner.token;
	if( strUserName == _T( "" ) )
	{
		// 존재하지 않은 아이디입니다. 아이디를 확인해 주십시오.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_INVALID_USER_ID ), NULL, prj.GetTextColor( TID_GAME_ERROR_INVALID_USER_ID ) );
		return FALSE;
	}
	if( g_pPlayer && g_pPlayer->GetName( TRUE ) == strUserName )
	{
		// 자기 캐릭터는 차단할 수 없습니다. 아이디를 확인해 주십시오.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_MY_CHARACTER_CANT_BLOCKING ), NULL, prj.GetTextColor( TID_GAME_ERROR_MY_CHARACTER_CANT_BLOCKING ) );
		return FALSE;
	}
	std::set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find( strUserName );
	if( BlockedUserIterator != prj.m_setBlockedUserID.end() )
	{
		// 이미 채팅 차단되어 있는 대상입니다.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_ALREADY_BLOCKED ), NULL, prj.GetTextColor( TID_GAME_ERROR_ALREADY_BLOCKED ) );
	}
	else
	{
		prj.m_setBlockedUserID.insert( strUserName );

		if( g_WndMng.m_pWndChattingBlockingList )
		{
			g_WndMng.m_pWndChattingBlockingList->UpdateInformation();
		}

		CString strMessage = _T( "" );
		// %s 님의 채팅을 차단하였습니다.
		strMessage.Format( prj.GetText( TID_GAME_USER_CHATTING_BLOCKING ), strUserName );
		g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_USER_CHATTING_BLOCKING ) );
	}
	return TRUE;
}

BOOL TextCmd_CancelBlockedUser( CScanner & scanner )
{
	scanner.GetToken();
	CString strUserName = scanner.token;
	std::set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find( strUserName );
	if( BlockedUserIterator != prj.m_setBlockedUserID.end() )
	{
		prj.m_setBlockedUserID.erase( strUserName );

		if( g_WndMng.m_pWndChattingBlockingList )
		{
			g_WndMng.m_pWndChattingBlockingList->UpdateInformation();
		}

		CString strMessage = _T( "" );
		// %s 님의 채팅 차단을 해제하였습니다.
		strMessage.Format( prj.GetText( TID_GAME_USER_CHATTING_UNBLOCKING ), strUserName );
		g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_USER_CHATTING_UNBLOCKING ) );
	}
	else
	{
		// 채팅 차단 목록에 없는 대상입니다.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_THERE_IS_NO_BLOCKED_TARGET ), NULL, prj.GetTextColor( TID_GAME_ERROR_THERE_IS_NO_BLOCKED_TARGET ) );
	}
	return TRUE;
}

BOOL TextCmd_IgnoreList( CScanner & scanner )
{
	if( g_WndMng.m_pWndChattingBlockingList )
	{
		SAFE_DELETE( g_WndMng.m_pWndChattingBlockingList );
	}

	g_WndMng.m_pWndChattingBlockingList = new CWndChattingBlockingList;
	if( g_WndMng.m_pWndChattingBlockingList )
	{
		g_WndMng.m_pWndChattingBlockingList->Initialize();
	}

	return TRUE;
}

#endif // __CLIENT

BOOL TextCmd_QuestState( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pAdmin	= (FLWSUser*)s.dwValue;
	FLWSUser* pUser	= NULL;
	int nQuest	= s.GetNumber();
	int nState = s.GetNumber();
	s.GetToken();
	if( s.tok != FINISHED )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );		//sun: 11, 캐릭터 정보 통합

		if( idPlayer )
			pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
		if( pUser == NULL )
		{
			pAdmin->AddDefinedText( TID_DIAG_0061, "%s", s.Token );
			return TRUE;
		}
	}
	else
	{
		pUser	= pAdmin;
	}
	if( nState >= QS_BEGIN && nState < QS_END )
	{
		QUEST quest;
		if( pUser->SetQuest( nQuest, nState, &quest ) )
		{
			pUser->AddSetQuest( &quest );

			char pszComment[100]	= { 0, };
			FLSPrintf( pszComment, _countof( pszComment ), "%s %d", pAdmin->GetName(), nState );
			g_dpDBClient.CalluspLoggingQuest(  pUser->m_idPlayer, nQuest, 11, pszComment );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_BeginQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	int nQuest = s.GetNumber();
	QUEST quest;
	if( pUser->SetQuest( nQuest, 0, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
BOOL TextCmd_EndQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	int nQuest = s.GetNumber();
	QUEST quest;
	if( pUser->SetQuest( nQuest, QS_END, &quest ) )
		pUser->AddSetQuest( &quest );

#ifdef __GUILD_HOUSE_MIDDLE
	GuildHouseMng->CheckGuildHouseQuest( pUser, nQuest );
#endif // __GUILD_HOUSE_MIDDLE

#endif
	return TRUE;
}
// 지정한 것, 현재와 완료 다 뒤져서 삭제 
BOOL TextCmd_RemoveQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pAdmin	= (FLWSUser*)s.dwValue;
	FLWSUser* pUser	= NULL;
	int nQuest	= s.GetNumber();
	s.GetToken();
	if( s.tok != FINISHED )
	{
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );		//sun: 11, 캐릭터 정보 통합

		if( idPlayer )
			pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
		if( pUser == NULL )
		{
			pAdmin->AddDefinedText( TID_DIAG_0061, "%s", s.Token );
			return TRUE;
		}
	}
	else
	{
		pUser	= pAdmin;
	}

	LPQUEST pQuest	= pUser->GetQuest(nQuest );
	char pszComment[100]	= { 0, };
	FLSPrintf( pszComment, _countof( pszComment ), "%s %d", pAdmin->GetName(), ( pQuest? pQuest->m_nState: -1 ) );
	g_dpDBClient.CalluspLoggingQuest(  pUser->m_idPlayer, nQuest, 40, pszComment );

	pUser->RemoveQuest( nQuest );
	pUser->AddRemoveQuest( nQuest );
#endif	// __WORLDSERVER
	return TRUE;
}
// 현재, 완료 모두 삭제 
BOOL TextCmd_RemoveAllQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	int nQuest = s.GetNumber();
	UNREFERENCED_PARAMETER( nQuest );

	pUser->RemoveAllQuest();
	pUser->AddRemoveAllQuest();
#endif
	return TRUE;
}
// 완료만 삭제 
BOOL TextCmd_RemoveCompleteQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	int nQuest = s.GetNumber();
	UNREFERENCED_PARAMETER( nQuest );

	pUser->RemoveCompleteQuest();
	pUser->AddRemoveCompleteQuest();
#endif
	return TRUE;
}

BOOL TextCmd_PvpParam( CScanner& scanner )
{
#if defined(__WORLDSERVER)
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	int	nFame       = scanner.GetNumber();
	int nSlaughter  = scanner.GetNumber();
	UNREFERENCED_PARAMETER( nSlaughter );

	pUser->m_nFame  = nFame;
	g_xWSUserManager->AddSetFame( pUser, nFame );

#endif
	return TRUE;
}
//sun: 8, // __S8_PK
BOOL TextCmd_PKParam( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser*	pUser			= (FLWSUser*)scanner.dwValue;
	int		nPKValue		= scanner.GetNumber();
	int		nPKPropensity	= scanner.GetNumber();

	if( nPKValue >= 0 )
	{
		pUser->SetPKValue( nPKValue );
		pUser->AddPKValue();
//sun: 13, 달인
		pUser->CheckHonorStat();
		pUser->AddHonorListAck();
		g_xWSUserManager->AddHonorTitleChange( pUser, pUser->m_nHonor);
	}

	if( nPKPropensity >= 0 )
	{
		pUser->SetPKPropensity( nPKPropensity );
		g_xWSUserManager->AddPKPropensity( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef _DEBUG
BOOL TextCmd_TransyItemList( CScanner& scanner )
{
#ifdef __CLIENT
	CString szMsg;
	scanner.GetToken();
	if( scanner.tokenType == STRING )
	{
		szMsg.Format( "Wait : Write %s", scanner.Token );
		g_WndMng.PutString( szMsg );

		for( SpecItemIdItr it = g_xSpecManager->m_SpecItemIdMap.begin(); it != g_xSpecManager->m_SpecItemIdMap.end(); ++it )
		{
			PT_ITEM_SPEC pItemProp = &it->second;
			g_pPlayer->GetTransyItem( pItemProp, TRUE, scanner.Token );
		}
		szMsg.Format( "Finish : Finish %s", scanner.Token );
		g_WndMng.PutString( szMsg );
	}
	else
	{
		g_WndMng.PutString( "Error : Ex) /TransyItemList Transy.txt&" );
	}
#else
	UNREFERENCED_PARAMETER( scanner );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_LoadToolTipColor( CScanner& /*scanner*/ )
{
#ifdef __CLIENT
	return g_Option.LoadToolTip( "ToolTip.ini" );
#else
	return TRUE;
#endif // __CLIENT
}

BOOL TextCmd_TestNewEnchantEffect( CScanner& scanner )
{
#ifdef __CLIENT 

	int nKind = scanner.GetNumber( );
	scanner.GetToken( );
	if( scanner.Token == CString( "off" ) )
	{
		g_pPlayer->DeleteEnchantEffect_NEW( nKind );
	}
	else
	{
		g_pPlayer->CreateEnchantEffect_NEW( nKind, scanner.token );
	}
#else
	UNREFERENCED_PARAMETER( scanner );
#endif //__CLIENT
	return TRUE;
}

#endif //_DEBUG


BOOL TextCmd_ReloadConstant( CScanner& /*scanner*/ )
{
#ifdef __WORLDSERVER
	g_DPCoreClient.SendLoadConstant();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatRequest( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	DWORD dwPenya = scanner.GetNumber();	
	g_GuildCombatMng.GuildCombatRequest( pUser, dwPenya );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatCancel( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	g_GuildCombatMng.GuildCombatCancel( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatOpen( CScanner& scanner )
{
#ifdef __CLIENT
#endif // __CLINET
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		FLSPrintf( chMessage, _countof( chMessage ), "Not GuildCombat Open :: Not CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}

	g_GuildCombatMng.GuildCombatOpen();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatIn( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	g_GuildCombatMng.GuildCombatEnter( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatClose( CScanner& scanner )
{
#ifdef __WORLDSERVER
	int	nClose       = scanner.GetNumber();

	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		FLSPrintf( chMessage, _countof( chMessage ), "Not GuildCombat Close :: Is CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}

	if( nClose == 0 )
		g_GuildCombatMng.SetGuildCombatClose( TRUE );
	else
		g_GuildCombatMng.SetGuildCombatCloseWait( TRUE );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatNext( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		FLSPrintf( chMessage, _countof( chMessage ), "Not GuildCombat Close :: Is CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}
	
	g_GuildCombatMng.m_dwTime = GetTickCount();
#endif // __WORLDSERVER
	return TRUE;
}

//sun: 10, 속성제련 제거(10차로 변경)
BOOL TextCmd_RemoveAttribute( CScanner& /*scanner*/ )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}
	
	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_pItemElem[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_pItemElem[0]->m_dwObjId;
			g_DPlay.SendRemoveAttribute( dwObjId );
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
	return TRUE;	
}

//sun: 11, 일대일 길드 대전
BOOL	TextCmd_GC1to1Open( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( g_GuildCombat1to1Mng.m_nState != g_GuildCombat1to1Mng.GC1TO1_OPEN )
	{
		BOOL bSrvrDown = FALSE;
		if( scanner.GetNumber() == 1 )
			bSrvrDown = TRUE;
		g_GuildCombat1to1Mng.GuildCombat1to1Open( bSrvrDown );
		return TRUE;
	}

	pUser->AddText( "already OPEN State!!!" );
#endif //__WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_GC1to1Close( CScanner& /*scanner*/ )
{
#ifdef __WORLDSERVER
	g_GuildCombat1to1Mng.m_nState = g_GuildCombat1to1Mng.GC1TO1_WAR;
	for( DWORD i=0; i<g_GuildCombat1to1Mng.m_vecGuilCombat1to1.size(); i++ )
	{
		if( g_GuildCombat1to1Mng.m_vecGuilCombat1to1.at( i ).m_nState != CGuildCombat1to1::GC1TO1WAR_CLOSEWAIT )
			g_GuildCombat1to1Mng.m_vecGuilCombat1to1.at( i ).GuildCombat1to1CloseWait();
		g_GuildCombat1to1Mng.m_vecGuilCombat1to1.at( i ).m_nWaitTime = -1;
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_GC1to1Next( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	if( g_GuildCombat1to1Mng.m_nState == g_GuildCombat1to1Mng.GC1TO1_CLOSE ) // GC1TO1_CLOSE
	{
		pUser->AddText( "Is CLOSE State!!!" );
		return TRUE;
	}
	if( g_GuildCombat1to1Mng.m_nState == g_GuildCombat1to1Mng.GC1TO1_WAR )
	{
		for( DWORD i=0; i<g_GuildCombat1to1Mng.m_vecGuilCombat1to1.size(); i++ )
			g_GuildCombat1to1Mng.m_vecGuilCombat1to1.at( i ).m_nWaitTime = -1;
		return TRUE;
	}

	g_GuildCombat1to1Mng.m_nWaitTime = -1;
#endif // __WORLDERVER
	return TRUE;
}

BOOL TextCmd_Coupon( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	pUser->m_nCoupon = s.GetNumber();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RemoveAllBuff( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	pUser->RemoveAllBuff();
	pUser->ClearAllSMMode();
#endif // __WORLDSERVER
	return TRUE;
}

//sun: 12, 심연의 탑
BOOL TextCmd_HeavenTower( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndHeavenTower)
		SAFE_DELETE(g_WndMng.m_pWndHeavenTower);

	g_WndMng.m_pWndHeavenTower = new CWndHeavenTower;

	if(g_WndMng.m_pWndHeavenTower)
		g_WndMng.m_pWndHeavenTower->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
BOOL TextCmd_RemoveJewel( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRemoveJewel)
		SAFE_DELETE(g_WndMng.m_pWndRemoveJewel);

	g_WndMng.m_pWndRemoveJewel = new CWndRemoveJewel(CWndRemoveJewel::WND_ULTIMATE);

	if(g_WndMng.m_pWndRemoveJewel)
		g_WndMng.m_pWndRemoveJewel->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

//sun: 12, 펫 알 변환 기능 추가
BOOL TextCmd_TransEggs( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_WndMng.CreateApplet( APP_INVENTORY );

	if(g_WndMng.m_pWndPetTransEggs)
		SAFE_DELETE(g_WndMng.m_pWndPetTransEggs);
	
	g_WndMng.m_pWndPetTransEggs = new CWndPetTransEggs;

	if(g_WndMng.m_pWndPetTransEggs)
		g_WndMng.m_pWndPetTransEggs->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomOpen( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_CLOSE )
		CSecretRoomMng::GetInstance()->SecretRoomOpen();
	else
		pUser->AddText( "Is Not Close State!!!" );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomEntrance( CScanner& s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 


#ifdef __CLIENT
	if( s.GetNumber() == 1 )
	{
		g_DPlay.SendTeleportToSecretRoomDungeon();
		return TRUE;
	}

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if(pWndWorld)
	{
		for(int i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
		{
			pWndWorld->m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
			pWndWorld->m_stKillCountCiphers[i].szMyGuildKillCount = '0';
			pWndWorld->m_stKillCountCiphers[i].ptPos = CPoint(0,0);
			pWndWorld->m_stKillCountCiphers[i].fScaleX = 1.0f;
			pWndWorld->m_stKillCountCiphers[i].fScaleY = 1.0f;
			pWndWorld->m_stKillCountCiphers[i].nAlpha = 255;

//			pWndWorld->m_bDrawMyGuildKillCount[i] = TRUE;
//			pWndWorld->m_szMyGuildKillCount[i] = '0';
		}
	}
	
	if(g_WndMng.m_pWndSecretRoomMsg)
		SAFE_DELETE( g_WndMng.m_pWndSecretRoomMsg );

	g_DPlay.SendSecretRoomEntrance();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomNext( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
//	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CSecretRoomMng::GetInstance()->m_dwRemainTime = 0;
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomTender( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomLineUp( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomLineUpOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomClose( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	
	if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_WAR )
	{
		std::map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for( ; it!=CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++ )
		{
			CSecretRoomContinent* pSRCont = it->second;
			if( pSRCont && pSRCont->m_nState != SRCONT_CLOSE )
				pSRCont->m_dwRemainTime = 0;
				//pSRCont->SetContCloseWait();
		}
	}
	else	
		pUser->AddText( "Is Not War State!!!" );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomTenderView( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderView();

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomCheckTaxRate);
	g_WndMng.m_pWndSecretRoomCheckTaxRate = new CWndSecretRoomCheckTaxRate;

	if(g_WndMng.m_pWndSecretRoomCheckTaxRate)
	{
		g_WndMng.m_pWndSecretRoomCheckTaxRate->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomTenderCancelReturn( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderCancelReturn();
#endif // __CLIENT
	return TRUE;
}


//sun: 12, 군주
BOOL TextCmd_ElectionRequirement( CScanner& /*s*/ )
{
#ifdef __CLIENT
	IElection* pElection	= CCLord::Instance()->GetElection();
	char lpString[100]		= { 0,};
	FLSPrintf( lpString, _countof( lpString ), "election state : total(%d)/requirement(%d)", pElection->GetVote(), pElection->GetRequirement() );
	g_WndMng.PutString( lpString );
#endif	// __CLIENT
	return TRUE;
}

#ifdef __INTERNALSERVER
BOOL TextCmd_RemoveTotalGold( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	__int64 iGold	= static_cast<__int64>( s.GetInt64() );
	if( iGold > pUser->GetTotalGold() )
	{
		char szText[100]	= { 0,};
		FLSPrintf( szText, _countof( szText ), "TextCmd_RemoveTotalGold: %I64d", pUser->GetTotalGold() );
		pUser->AddText( szText );
	}
	else
		pUser->RemoveTotalGold( iGold );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionAddDeposit( CScanner& s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 

#ifdef __CLIENT
	__int64 iDeposit	= static_cast<__int64>( s.GetNumber() );
	g_DPlay.SendElectionAddDeposit( iDeposit );
#endif	// __CLIENT
	return TRUE;
}
BOOL TextCmd_ElectionSetPledge( CScanner& s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 

#ifdef __CLIENT
	char szPledge[CCandidate::nMaxPledgeLen]	= { 0,};
	
	s.GetLastFull();
	if( lstrlen( s.token ) >= CCandidate::nMaxPledgeLen )
		return TRUE;
	FLStrcpy( szPledge, _countof( szPledge ), s.token );
	StringTrimRight( szPledge );
//	RemoveCRLF( szPledge );
	g_DPlay.SendElectionSetPledge( szPledge );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionIncVote( CScanner& s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 

#ifdef __CLIENT
	s.GetToken();
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
	if( idPlayer > 0 )
		g_DPlay.SendElectionIncVote( idPlayer );
	else
		g_WndMng.PutString( prj.GetText( TID_GAME_ELECTION_INC_VOTE_E001 ) );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionProcess( CScanner& s )
{
#ifdef __WORLDSERVER
	BOOL bRun	= static_cast<BOOL>( s.GetNumber() );
	g_dpDBClient.SendElectionProcess( bRun );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionBeginCandidacy( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginCandidacy();
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_ElectionBeginVote( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginVote();
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_ElectionEndVote( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginEndVote();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionState( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	pElection->State();
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_LEventCreate( CScanner & s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 

#ifdef __CLIENT
	int iEEvent		= s.GetNumber();
	int iIEvent		= s.GetNumber();
	g_DPlay.SendLEventCreate( iEEvent, iIEvent );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_LEventInitialize( CScanner & /*s*/ )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendLEventInitialize();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LSkill( CScanner & s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 

#ifdef __CLIENT
	int nSkill	= s.GetNumber();
	s.GetToken();
	char szTarget[MAX_PLAYER]	= { 0,};
	FLStrncpy( szTarget, _countof( szTarget ), s.token, MAX_PLAYER );
	g_DPlay.SendLordSkillUse( nSkill, szTarget );
#endif	// __CLIENT
	return TRUE;
}
#endif	// __INTERNALSERVER

//sun: 12, 튜토리얼 개선
BOOL TextCmd_SetTutorialState( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	int nTutorialState	= s.GetNumber();
	pUser->SetTutorialState( nTutorialState );
	pUser->AddSetTutorialState();
#endif	// __WORLDSERVER
	return TRUE;
}

//sun: 12, 세금
BOOL TextCmd_TaxApplyNow( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	CTax::GetInstance()->SetApplyTaxRateNow();
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __INTERNALSERVER
BOOL TextCmd_TaxRateUpdate( CScanner& scanner )
{
#ifdef __WORLDSERVER
	const int nTaxRate = scanner.GetNumber();
	CTax::GetInstance()->UpdateTaxRate( nTaxRate );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ViewPartyEffect( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return TRUE;
	}

	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty == NULL )
	{
		return TRUE;
	}

	for( size_t Nth = PARTY_EFFECT_PARSKILLFULL; Nth < PARTY_EFFECT_MAX; ++Nth )
	{
		TCHAR szMessage[128] = { 0, };
		FLSPrintf( szMessage, _countof( szMessage ), "%d - %d", Nth, pParty->IsActivatedEffect( Nth ) );
		pUser->AddText( szMessage );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Mirchang_Test( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	if( IsValidObj( pUser ) == FALSE )
	{
		return TRUE;
	}

	FLPacketFlyffPieceExchangeListReq kPacket;
	kPacket.dwMainCategory	= scanner.GetNumber();
	kPacket.dwSubCategory	= scanner.GetNumber();

	g_pFlyffPieceEvent->OnViewList( pUser, &kPacket );
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __INTERNALSERVER

//sun: 13, 달인
BOOL TextCmd_HonorTitleSet( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	int nIdx = -1;
	s.GetToken();
	if( s.tokenType == NUMBER ) 
	{
		nIdx	= _ttoi( s.Token );
	}
	else
	{
		nIdx	= CTitleManager::Instance()->GetIdxByName( s.Token );
	}

	if( nIdx < 0 ||  nIdx >= MAX_HONOR_TITLE )
		return FALSE;

	DWORD dwNum	= s.GetNumber();
	if( dwNum > 100000000 ) dwNum = 100000000;
	if( dwNum < 0 ) dwNum = 0;

	pUser->SetHonorCount(nIdx,dwNum);
	pUser->AddHonorListAck();
	g_dpDBClient.SendLogGetHonorTime(pUser,nIdx);
#endif // __WORLDSERVER
	return TRUE;
}

//sun: 13, 달인

//sun: 13, 레인보우 레이스
BOOL TextCmd_RainbowRaceDay( CScanner & _scanner )
{
#ifdef __WORLDSERVER
#pragma warning( disable : 4482 )
	FLWSUser* pUser	= reinterpret_cast<FLWSUser*>( _scanner.dwValue );
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	if( CRainbowRaceMng::GetInstance()->GetState() != CRainbowRaceMng::RR_CLOSED )
	{
		pUser->AddText( "[ 지금은 요일 설정을 할 수 없습니다. 레인보우 레이스가 진행중 입니다.]" );
		return FALSE;
	}

	_scanner.GetToken();
	std::string	strDay	= _scanner.Token;

	const int nDay		= _tstoi( strDay.c_str() );

	const int SetDay	= ( nDay > CRainbowRaceMng::Day::DAY_NONE && nDay < CRainbowRaceMng::Day::DAY_MAX )
						? nDay : CRainbowRaceMng::GetInstance()->DayStringToDefine( strDay );

	if( CRainbowRaceMng::GetInstance()->SetCurrentDay( SetDay ) == false )
	{
		pUser->AddText( "[ 요일 설정이 잘못되었습니다. SUN ~ SAT 또는 1 ~ 7 사용. ]" );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceApp( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRaceApplicationOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRacePass( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CRainbowRace* pRainbowRace = CRainbowRaceMng::GetInstance()->GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
	{
		pRainbowRace->SetNowGameComplete( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceOpen( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	if( CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_CLOSED )
		CRainbowRaceMng::GetInstance()->SetState( CRainbowRaceMng::RR_OPEN );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceNext( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	CRainbowRaceMng::GetInstance()->SetNextTime( 0 );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceInfo( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRaceInfo)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRaceInfo);

	g_WndMng.m_pWndRainbowRaceInfo = new CWndRainbowRaceInfo;

	if(g_WndMng.m_pWndRainbowRaceInfo)
		g_WndMng.m_pWndRainbowRaceInfo->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceRule( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRaceRule)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRaceRule);

	g_WndMng.m_pWndRainbowRaceRule = new CWndRainbowRaceRule;

	if(g_WndMng.m_pWndRainbowRaceRule)
		g_WndMng.m_pWndRainbowRaceRule->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceRanking( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRacePrevRankingOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRacePrize( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRacePrize)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRacePrize);

	g_WndMng.m_pWndRainbowRacePrize = new CWndRainbowRacePrize;

	if(g_WndMng.m_pWndRainbowRacePrize)
		g_WndMng.m_pWndRainbowRacePrize->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceKawiBawiBo( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_GAWIBAWIBO, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceDice( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_DICEPLAY, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceArithmetic( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_ARITHMATIC, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceStopWatch( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_STOPWATCH, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceTyping( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_TYPING, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceCard( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_PAIRGAME, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceLadder( CScanner& /*s*/ )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_LADDER, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceReqFininsh( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRaceReqFinish();
#endif // __CLIENT
	return TRUE;
}


//sun:13, 제련 확장(속성, 일반)
BOOL TextCmd_ChangeAttribute( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndChangeAttribute)
		SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);

	g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;

	if(g_WndMng.m_pWndChangeAttribute)
		g_WndMng.m_pWndChangeAttribute->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

//sun: 13, 하우징 시스템
BOOL TextCmd_HousingVisitRoom( CScanner& s )
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER( s );
#endif 

#ifdef __CLIENT
	s.GetToken();
	if( s.Token == "" )	// 아무 값 없으면 내방으로...
		g_DPlay.SendHousingVisitRoom( g_pPlayer->m_idPlayer );
	else	// 캐릭터명이 있으면 해당 캐릭터의 방으로...
	{
		DWORD dwPlayerId = CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
		if( dwPlayerId )
			g_DPlay.SendHousingVisitRoom( dwPlayerId );
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_HousingGMRemoveAll( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	CHousingMng::GetInstance()->ReqGMFunrnitureListAll( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_SmeltSafetyNormal( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_NORMAL);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SmeltSafetyAccessary( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ACCESSARY);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SmeltSafetyPiercing( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_PIERCING);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}
*/

BOOL TextCmd_SmeltSafetyElement( CScanner& /*s*/ )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndSmeltSafety )
		SAFE_DELETE( g_WndMng.m_pWndSmeltSafety );

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety( CWndSmeltSafety::WND_ELEMENT );
	if( g_WndMng.m_pWndSmeltSafety )
		g_WndMng.m_pWndSmeltSafety->Initialize( NULL );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_QuizEventOpen( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	if( !CQuiz::GetInstance()->IsRun() )
		g_dpDBClient.SendQuizEventOpen( CQuiz::GetInstance()->GetType() );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_QuizEventEnterance( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendQuizEventEntrance();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_QuizStateNext( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( CQuiz::GetInstance()->IsRun() )
	{
		if( IsValidObj( pUser ) && pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			CQuiz::GetInstance()->SetNextTime( 0 );
			CQuiz::GetInstance()->Process();
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_QuizEventClose( CScanner& /*s*/ )
{
#ifdef __WORLDSERVER
	if( CQuiz::GetInstance()->IsRun() )
		CQuiz::GetInstance()->CloseQuizEvent();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_BuyGuildHouse( CScanner& /*s*/ )
{
#ifdef __CLIENT
	g_DPlay.SendBuyGuildHouse();
#endif // __CLIENT
	return TRUE;
}

#ifdef __CLIENT
#include "WndGuildHouse.h"
#endif // __CLIENT
BOOL TextCmd_GuildHouseUpkeep( CScanner & /*s*/ )
{
#ifdef __CLIENT
	if( !g_WndMng.m_pWndUpkeep )
	{
		g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
		g_WndMng.m_pWndUpkeep->Initialize( &g_WndMng, APP_CONFIRM_BUY_ );
	}
	else
	{
		g_WndMng.m_pWndUpkeep->Destroy( );
		g_WndMng.m_pWndUpkeep = NULL;
	}
#endif // __CLIENT
	return TRUE;
}

#ifdef __GUILD_HOUSE_MIDDLE
BOOL TextCmd_GuildHouseTenderOpen( CScanner & s )
{
#ifdef __WORLDSERVER

	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_GUILDHOUSE_TENDER_STATE tGHTenderState = { 0 };
		//::mem_set( &tGHTenderState, 0, sizeof( tGHTenderState ) );

		tGHTenderState.dwGHType = WI_GUILDHOUSE_MIDDLE;
		tGHTenderState.nState = GH_TENDER_CHECK_OPEN;
		g_dpDBClient.SendGuildHouseTenderState( &tGHTenderState );
	}

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildHouseTenderStart( CScanner & s )
{
#ifdef __WORLDSERVER

	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_GUILDHOUSE_TENDER_STATE tGHTenderState = { 0 };
		//::mem_set( &tGHTenderState, 0, sizeof( tGHTenderState ) );

		tGHTenderState.dwGHType = WI_GUILDHOUSE_MIDDLE;
		tGHTenderState.nState = GH_TENDER_TENDER;
		g_dpDBClient.SendGuildHouseTenderState( &tGHTenderState );
	}

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildHouseTenderClose( CScanner & s )
{
#ifdef __WORLDSERVER

	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == TRUE )
	{
		T_PACKET_GUILDHOUSE_TENDER_STATE tGHTenderState = { 0 };
		//::mem_set( &tGHTenderState, 0, sizeof( tGHTenderState ) );

		tGHTenderState.dwGHType = WI_GUILDHOUSE_MIDDLE;
		tGHTenderState.nState = GH_TENDER_TENDER_RESULT;
		g_dpDBClient.SendGuildHouseTenderState( &tGHTenderState );
	}

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildHouseExpired( CScanner & s )
{
#ifdef __WORLDSERVER

	FLWSUser* pUser	= (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) == TRUE && pUser->m_idGuild > 0 )
	{
		CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( pUser->m_idGuild );
		if( pGuildHouse != NULL )
		{
			g_dpDBClient.SendGuildHouseExpired( pUser->m_idGuild );
		}
	}

#endif // __WORLDSERVER
	return TRUE;
}

#endif // __GUILD_HOUSE_MIDDLE

BOOL TextCmd_CampusInvite( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pRequest = (FLWSUser*)s.dwValue;
	if( !IsValidObj( pRequest ) )
		return FALSE;

	s.GetToken();
	u_long idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( 0 < idTarget )
	{
		FLWSUser* pTarget	= g_xWSUserManager->GetUserByPlayerID( idTarget );	
		if( IsValidObj( pTarget ) )
		{
			CCampusHelper::GetInstance()->OnInviteCampusMember( pRequest, pTarget );
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idTarget );
			if( pPlayerData )
				pRequest->AddQueryPlayerData( idTarget, pPlayerData );
		}

		else
			pRequest->AddDefinedText( TID_DIAG_0061, "\"%s\"", s.Token );
	}
	else
		pRequest->AddDefinedText( TID_DIAG_0060, "\"%s\"", s.Token );
#endif // __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_RemoveCampusMember( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pRequest = (FLWSUser*)s.dwValue;
	if( !IsValidObj( pRequest ) )
		return FALSE;

	s.GetToken();
	u_long idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( idTarget > 0 )
		CCampusHelper::GetInstance()->OnRemoveCampusMember( pRequest, idTarget );
		
	else
		pRequest->AddDefinedText( TID_DIAG_0060, "\"%s\"", s.Token );

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_UpdateCampusPoint( CScanner& s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	if( !IsValidObj( pUser ) )
		return FALSE;

	int nCampusPoint = 0;
	nCampusPoint = s.GetNumber();

	if( IsValidObj( pUser ) )
		g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, nCampusPoint, TRUE, 'G' );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_InvenRemove( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	if( IsValidObj( pUser ) )
	{
		const DWORD invenSize = pUser->m_Inventory.GetMax();
		for( DWORD Nth = MAX_HUMAN_PARTS; Nth < invenSize; ++Nth )
		{
			FLItemElem* pItemElem = pUser->m_Inventory.GetAt( Nth );
			if( pItemElem != NULL && pUser->IsUsing( pItemElem ) == FALSE )
			{
				pUser->RemoveItem( pItemElem->m_dwObjId, pItemElem->m_nItemNum );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __AGGRO16
BOOL TextCmd_DisplayAggro( CScanner& scanner )    
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	if( IsValidObj( pUser ) )
	{
		pUser->m_bDisplayAggro	 = !pUser->m_bDisplayAggro;

		g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, FALSE );
		g_DPCoreClient.SendChat( pUser->m_idPlayer, pUser->m_idPlayer, pUser->m_bDisplayAggro ? "display aggro ON" : "display aggro OFF" );
		g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, TRUE );
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif // __AGGRO16

BOOL	TextCmd_EventArenaManageWindowOpen( CScanner& kScanner )
{
#ifdef __WORLDSERVER
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		FLWSUser* pUser = reinterpret_cast< FLWSUser* >( kScanner.dwValue );
		if( IsValidObj( pUser ) )
		{
			g_pEventArena->StartManage( pUser );
		}
	}
#endif

	return TRUE;
}

BOOL	TextCmd_EventArenaManageWindowClose( CScanner& kScanner )
{
#ifdef __WORLDSERVER
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		FLWSUser* pUser = reinterpret_cast< FLWSUser* >( kScanner.dwValue );
		if( IsValidObj( pUser ) )
		{
			g_pEventArena->StopManage( pUser, _T( "ChatCommand" ) );
		}
	}
#endif

	return TRUE;
}

BOOL	TextCmd_EventArenaManageGameForceEnd( CScanner& kScanner )
{
#ifdef	__WORLDSERVER
	if( g_pEventArenaGlobal->IsArenaChannel() )
	{
		FLWSUser* pUser = reinterpret_cast< FLWSUser* >( kScanner.dwValue );
		if( IsValidObj( pUser ) )
		{
			g_pEventArena->StopGame( pUser, _T( "ChatCommand" ) );
		}
	}
#endif

	return TRUE;
}

BOOL TextCmd_SetGiftPoint( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
	const int nGiftPoint = scanner.GetNumber();

	if( IsValidObj( pUser ) == TRUE )
	{
		pUser->m_kMadrigalGiftPoint.nPoint	= nGiftPoint;

		g_pMadrigalGift->OnLevel( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ViewGiftPoint( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	if( IsValidObj( pUser ) == TRUE )
	{
		TCHAR szMessage[32] = { 0, };
		FLSPrintf( szMessage, _countof( szMessage ), "%d", pUser->m_kMadrigalGiftPoint.nPoint );
		pUser->AddText( szMessage );
	}
#endif // __WORLDSERVER
	return TRUE;
}

// BOOL TextCmd_StartFlyffPieceBurningMode( CScanner& scanner )
// {
// #ifdef __WORLDSERVER
// 	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
// 
// 	if( IsValidObj( pUser ) == TRUE )
// 	{
// 		T_BURNING_MODE kMode;
// 		kMode.dwBurningMode	= scanner.GetNumber();
// 		kMode.dwMinute		= scanner.GetNumber();
// 
// 		BEFOREBROADCAST( ar, PACKETTYPE_START_FLYFF_PIECE_BURNING_MODE );
// 		ar << pUser->m_idPlayer;
// 		ar << kMode;
// 		SEND( ar, &g_DPCoreClient, DPID_SERVERPLAYER );
// 	}
// #endif // __WORLDSERVER
// 	return TRUE;
// }
// 
// BOOL TextCmd_EndFlyffPieceBurningMode( CScanner& scanner )
// {
// #ifdef __WORLDSERVER
// 	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;
// 
// 	if( IsValidObj( pUser ) == TRUE )
// 	{
// 		BEFOREBROADCAST( ar, PACKETTYPE_END_FLYFF_PIECE_BURNING_MODE );
// 		ar << pUser->m_idPlayer;
// 		SEND( ar, &g_DPCoreClient, DPID_SERVERPLAYER );
// 	}
// #endif // __WORLDSERVER
// 	return TRUE;
// }

BOOL TextCmd_OpenTreasureChest( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	if( IsValidObj( pUser ) == FALSE )
	{
		return TRUE;
	}

	FLItemElem* pChest	= pUser->m_Inventory.GetAt( MAX_HUMAN_PARTS );
	FLItemElem* pKey	= pUser->m_Inventory.GetAt( MAX_HUMAN_PARTS + 1 );
	if( IsUsableItem( pChest ) == FALSE || IsUsableItem( pKey ) == FALSE )
	{
		return TRUE;
	}

	FLPacketTreasureChestOpenByKeyReq kPacket;
	kPacket.dwChestItemObjID	= pChest->m_dwObjId;
	kPacket.dwKeyItemObjID		= pKey->m_dwObjId;

	const DWORD dwMax = pChest->m_nItemNum < pKey->m_nItemNum ? pChest->m_nItemNum : pKey->m_nItemNum;

	for( DWORD Nth = 0; Nth < dwMax; ++Nth )
	{
		const bool bRet = g_pTreasureChest->OnDoOpenTreasureChest( pUser, &kPacket );
		if( bRet == false )
		{
			break;
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DeleteAllMail( CScanner& scanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= (FLWSUser*)scanner.dwValue;

	if( IsValidObj( pUser ) == FALSE )
	{
		return TRUE;
	}

	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox( pUser->m_idPlayer );
	if( pMailBox == NULL )
	{
		return TRUE;
	}

	for( MailVectorItr pos = pMailBox->begin(); pos != pMailBox->end(); ++pos )
	{
		CMail* pMail	= *pos;
		if( pMail == NULL )
		{
			continue;
		}

		g_dpDBClient.SendQueryRemoveMail( pUser->m_idPlayer, pMail->m_nMail );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_DonationBuffInfo( CScanner& kScanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser			= (FLWSUser*)kScanner.dwValue;
	if( IsValidObj( pUser ) )
	{
		using namespace nsCooperativeContributions;
		
		COOPERATIVE_CONTRIBUTIONS().Send_ContributionInfo( *pUser );

		using namespace nsCooperativeContributions;
		char szMessage[ 1024 ] = { 0 };
		//FLSPrintf( szMessage, _countof( szMessage ), "exp(%f), drop(%f), buy(%f), sell(%f), def(%d), attack(%d)"
		FLSPrintf( szMessage, _countof( szMessage ), "exp(%u), drop(%u), buy(%u), sell(%u), def(%d), attack(%d)"
			, GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue() 
			, GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue()
			, GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR().GetRewardValue()
			, GLOBAL_REWARD_ALARM_SHOP_SELL_FACTOR().GetRewardValue()
			, GLOBAL_REWARD_ALARM_DEFENCE_POWER().GetRewardValue()
			, GLOBAL_REWARD_ALARM_ATTACK_POWER().GetRewardValue()
			);
		g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, FALSE );
		g_DPCoreClient.SendChat( pUser->m_idPlayer, pUser->m_idPlayer, szMessage );
		g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, TRUE );
	}
#endif
	return TRUE;
}

#ifdef __WORLDSERVER
#include "../worldserver/FLShutdownRule_Mng.h"
#endif
BOOL TextCmd_DisplayShutdownList( CScanner & s )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser = (FLWSUser*)s.dwValue;
	if( IsValidObj( pUser ) )
	{
		g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, FALSE );
		for( std::map< u_long /*playerID*/, CTime >::const_iterator it = FLShutdownRule_Mng::GetInstance().m_mapDoubtUser.begin(); it != FLShutdownRule_Mng::GetInstance().m_mapDoubtUser.end(); ++it )
		{
			const u_long uPlayerID	= it->first;
			const CTime tmBirthDate = it->second;

			FLWSUser *pUser			= g_xWSUserManager->GetUserByPlayerID( uPlayerID );	
			if( pUser == NULL )
				continue;

			char szOut[ 1024 ] = { 0 };
			FLSPrintf( szOut, _countof( szOut ), "[ (%s):(%d-%d-%d)]", pUser->GetName(), tmBirthDate.GetYear(), tmBirthDate.GetMonth(), tmBirthDate.GetDay() );

			g_DPCoreClient.SendChat( pUser->m_idPlayer, pUser->m_idPlayer, szOut );
		}
		g_DPCoreClient.SendSetSnoop( pUser->m_idPlayer, pUser->m_idPlayer, TRUE );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_ShowKeepTime( CScanner & kScanner )
{
#ifdef __WORLDSERVER
	FLWSUser* pUser	= reinterpret_cast<FLWSUser*>( kScanner.dwValue );
	if( IsValidObj( pUser ) == TRUE )
	{
		const DWORD dwKeepConnectionMin	= pUser->GetKeepConnectionMinute();

		TCHAR szText[128] = { 0, };
		FLSPrintf( szText, _countof( szText ), _T( "[ Keep Connection Time : %d Minute ]" ), dwKeepConnectionMin );
		pUser->AddText( szText );
	}
#endif // __WORLDSERVER

	return TRUE;
}
#define COMMAND_NPC_OUTPUT 1
#ifdef COMMAND_NPC_OUTPUT
BOOL	TextCmd_NpcOutPut( CScanner & scanner )
{
#ifdef __WORLDSERVER
	/*FLWSUser* pUser	= reinterpret_cast<FLWSUser*>( kScanner.dwValue );
	kScanner.GetToken();
	LPCHARACTER pCharacter = prj.GetCharacter (kScanner.Token);
	
	pCharacter->bOutput = TRUE;*/

	FLWSUser* pUser = (FLWSUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	CString strName = scanner.Token;

	//@@@@@@@@@@@@@@@@@@ MOVER 속성으로 구분해서 몬스터인것만 생성 해야할 듯.
	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		if( pMoverProp->dwAI != AII_MONSTER
			&& pMoverProp->dwAI != AII_CLOCKWORKS
			&& pMoverProp->dwAI != AII_BIGMUSCLE
			&& pMoverProp->dwAI != AII_KRRR
			&& pMoverProp->dwAI != AII_BEAR
			&& pMoverProp->dwAI != AII_METEONYKER
#ifdef __AGGRO16
			&& pMoverProp->dwAI != AII_AGGRO_NORMAL
#endif // __AGGRO16
			&& pMoverProp->dwAI != AII_PARTY_AGGRO_LEADER
			&& pMoverProp->dwAI != AII_PARTY_AGGRO_SUB
			&& pMoverProp->dwAI != AII_ARENA_REAPER
			&& pMoverProp->dwAI != AII_NONE
		)
			return TRUE;
		scanner.GetToken();
		LPCHARACTER pCharacter = prj.GetCharacter (scanner.Token);

		

		//DWORD dwNum	= scanner.GetNumber();
		//if( dwNum > 100 ) dwNum = 100;
		//if( dwNum == 0 ) dwNum = 1;

		//BOOL bActiveAttack = scanner.GetNumber();
		//for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	
				return FALSE;	
			pObj->SetPos( vPos );
			pObj->InitMotion( MTI_STAND );
			pObj->UpdateLocalMatrix();

			//if( bActiveAttack )
			//	((CMover*)pObj)->m_bActiveAttack = bActiveAttack;
			
			//((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정

			if( pWorld->ADDOBJ( pObj, TRUE, pUser->GetLayer() ) == FALSE )
			{
				SAFE_DELETE( pObj );
			}
			else
			{
				CMover* pMover = static_cast<CMover*>(pObj);
				FLStrcpy( pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ), scanner.Token );
				pMover->InitNPCProperty();
				LPCHARACTER lpChar = static_cast<CMover*>(pObj)->GetCharacter();
				lpChar = new CHARACTER;
				pCharacter->bOutput = TRUE;
				pCharacter->m_dwWorldId = pUser->GetWorld()->GetID();
				pCharacter->m_vPos = vPos;
				memcpy ( lpChar, pCharacter, sizeof(CHARACTER) );

			}
		}
	}
				int a;
				a = 1;
#else
	scanner.GetToken();
	scanner.GetToken();
	LPCHARACTER pCharacter = prj.GetCharacter (scanner.Token);
	pCharacter->bOutput = TRUE;
#endif
	return TRUE;
}
#endif

//BOOL	TextCmd_TimerSpeedUp( CScanner & scanner )
//{
//	DATE_TIMER().SetTimeSpeed_DebugMode( 333 );
//	return TRUE;
//}


BEGINE_TEXTCMDFUNC_MAP
////////////////////////////////////////////////// AUTH_GENERAL begin/////////////////////////////////////////////////////
	ON_TEXTCMDFUNC( TextCmd_DisplayShutdownList,	"ShutdownruleList", "sdr", "셧다운룰리스트", "셧룰리",  TCM_SERVER, AUTH_GAMEMASTER, "협동 기부 버프 정보" )
	ON_TEXTCMDFUNC( TextCmd_whisper,               "whisper",           "w",              "귓속말",         "귓",      TCM_SERVER, AUTH_GENERAL      , "귓속말 [/명령 아이디 내용]" )
	ON_TEXTCMDFUNC( TextCmd_say,                   "say",               "say",            "말",             "말",      TCM_SERVER, AUTH_GENERAL      , "속삭임 [/명령 아이디 내용]" )
	ON_TEXTCMDFUNC( TextCmd_Position,              "position",          "pos",            "좌표",           "좌표",    TCM_CLIENT, AUTH_GENERAL      , "현재 좌표를 출력해준다." )
	ON_TEXTCMDFUNC( TextCmd_shout,                 "shout",             "s",              "외치기",         "외",      TCM_BOTH  , AUTH_GENERAL      , "외치기 [/명령 아이디 내용]" )
	ON_TEXTCMDFUNC( TextCmd_PartyChat,             "partychat",         "p",              "극단말",         "극",      TCM_BOTH  , AUTH_GENERAL      , "파티 채팅 [/명령 내용]" )
	ON_TEXTCMDFUNC( TextCmd_Time,                  "Time",              "ti",             "시간",           "시",      TCM_CLIENT, AUTH_GENERAL      , "시간 보기 [/시간]" )
//	ON_TEXTCMDFUNC( TextCmd_ChangeFace,            "ChangeFace",        "cf",             "얼굴변경",       "얼변",    TCM_BOTH  , AUTH_GENERAL      , "얼굴 변경" )
	ON_TEXTCMDFUNC( TextCmd_GuildChat,             "GuildChat",         "g",              "길드말",         "길말",    TCM_BOTH, AUTH_GENERAL      , "길드말" )
	ON_TEXTCMDFUNC( TextCmd_PartyInvite,           "PartyInvite",       "partyinvite",    "극단초청",       "극초",    TCM_SERVER, AUTH_GENERAL      , "극단 초청" )
	ON_TEXTCMDFUNC( TextCmd_GuildInvite,           "GuildInvite",       "guildinvite",    "길드초청",       "길초",    TCM_SERVER, AUTH_GENERAL      , "길드 초청" )
	ON_TEXTCMDFUNC( TextCmd_CampusInvite,          "CampusInvite",		"campusinvite",   "사제초청",		"사초",    TCM_SERVER, AUTH_GENERAL      , "사제 초청" )
#ifdef __CLIENT
	ON_TEXTCMDFUNC( TextCmd_tradeagree,            "tradeagree",        "ta",             "거래승인",       "거승",    TCM_CLIENT, AUTH_GENERAL      , "거래 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_traderefuse,           "traderefuse",       "tr",             "거래거절",       "거절",    TCM_CLIENT, AUTH_GENERAL      , "거래 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_whisperagree,          "whisperagree",      "wa",             "귓속말승인",     "귓승",    TCM_CLIENT, AUTH_GENERAL      , "귓속말 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_whisperrefuse,         "whisperrefuse",     "wr",             "귓속말거절",     "귓절",    TCM_CLIENT, AUTH_GENERAL      , "귓속말 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_messengeragree,        "messengeragree",    "ma",             "메신저승인",     "메승",    TCM_CLIENT, AUTH_GENERAL      , "메신저 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_messengerrefuse,       "messengerrefuse",   "mr",             "메신저거절",     "메절",    TCM_CLIENT, AUTH_GENERAL      , "메신저 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_stageagree,            "stageagree",        "ga",             "극단승인",       "극승",    TCM_CLIENT, AUTH_GENERAL      , "극단 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_stagerefuse,           "stagerefuse",       "gr",             "극단거절",       "극절",    TCM_CLIENT, AUTH_GENERAL      , "극단 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_connectagree,          "connectagree",      "ca",             "접속알림",       "접알",    TCM_CLIENT, AUTH_GENERAL      , "접속알림 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_connectrefuse,         "connectrefuse",     "cr",             "접속알림해제",   "접해",    TCM_CLIENT, AUTH_GENERAL      , "접속알림 해제 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_shoutagree,            "shoutagree",        "ha",             "외치기승인",     "외승",    TCM_CLIENT, AUTH_GENERAL      , "외치기 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_shoutrefuse,           "shoutrefuse",       "hr",             "외치기해제",     "외해",    TCM_CLIENT, AUTH_GENERAL      , "외치기 거절 [/명령] " )

	ON_TEXTCMDFUNC( TextCmd_BlockUser,             "ignore",             "ig",             "채팅차단",       "채차",    TCM_CLIENT, AUTH_GENERAL      , "채팅차단 [/명령 아이디]" )
	ON_TEXTCMDFUNC( TextCmd_CancelBlockedUser,     "unignore",           "uig",            "채팅차단해제",   "채차해",  TCM_CLIENT, AUTH_GENERAL      , "채팅차단해제 [/명령 아이디]" )
	ON_TEXTCMDFUNC( TextCmd_IgnoreList,            "ignorelist",         "igl",            "채팅차단목록",   "채차목",  TCM_CLIENT, AUTH_GENERAL      , "채팅 차단 목록" )

#endif //__CLIENT
////////////////////////////////////////////////// AUTH_GENERAL end/////////////////////////////////////////////////////
#ifdef	__INTERNALSERVER
	ON_TEXTCMDFUNC( TextCmd_SetAdmin,				"setadmin",			"admin",		"관리자",			"어드민",		TCM_SERVER, AUTH_GENERAL,	"관리자 되기" )
	ON_TEXTCMDFUNC( TextCmd_SetGeneralUser,			"setuser",			"user",			"일반유저",		"유저",		TCM_SERVER, AUTH_GAMEMASTER,	"일반 유저 되기" )
#endif
	// GM_LEVEL_1
	ON_TEXTCMDFUNC( TextCmd_Teleport,              "teleport",          "te",             "텔레포트",       "텔레",    TCM_SERVER, AUTH_GAMEMASTER   , "텔레포트" )
	ON_TEXTCMDFUNC( TextCmd_Invisible,             "invisible",         "inv",            "투명",           "투명",    TCM_SERVER, AUTH_GAMEMASTER   , "투명화" )
	ON_TEXTCMDFUNC( TextCmd_NoInvisible,           "noinvisible",       "noinv",          "투명해제",       "투해",    TCM_SERVER, AUTH_GAMEMASTER   , "투명화 해제" )
	ON_TEXTCMDFUNC( TextCmd_Summon,                "summon",            "su",             "소환",           "소환",    TCM_SERVER, AUTH_GAMEMASTER   , "유저소환" )
	ON_TEXTCMDFUNC( TextCmd_count,                 "count",             "cnt",            "접속자수",       "접속자수",TCM_SERVER, AUTH_GAMEMASTER   , "접속자 카운트" )
	
	// GM_LEVEL_2
	ON_TEXTCMDFUNC( TextCmd_Out,                   "out",               "out",            "퇴출",           "퇴출",    TCM_SERVER, AUTH_GAMEMASTER2   , "퇴출" )
	ON_TEXTCMDFUNC( TextCmd_Talk,                  "talk",              "nota",           "말해제",         "말해",    TCM_SERVER, AUTH_GAMEMASTER2   , "말하지 못하게 하기 해제" )
	ON_TEXTCMDFUNC( TextCmd_NoTalk,                "notalk",            "ta",             "말정지",         "말정",    TCM_SERVER, AUTH_GAMEMASTER2   , "말하지 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_ip,                    "ip",                "ip",             "아이피",         "아이피",  TCM_BOTH  , AUTH_GAMEMASTER2     , "상대 IP알기" )

	ON_TEXTCMDFUNC( TextCmd_Mute,					"Mute",				"mute",          "조용히",     "조용히",    TCM_SERVER,  AUTH_GAMEMASTER2, "" )	

	ON_TEXTCMDFUNC( TextCmd_GuildRanking,          "GuildRanking",       "ranking",        "길랭",           "길랭",    TCM_SERVER, AUTH_GAMEMASTER2   , "" )

	ON_TEXTCMDFUNC( TextCmd_FallEffect,              "FallEffect",           "fe",             "내려라",         "내려",    TCM_SERVER, AUTH_GAMEMASTER2   , "내리기 토글" )
	ON_TEXTCMDFUNC( TextCmd_StopEffect,              "StopEffect",           "se",             "그만내려라",     "그만내려",    TCM_SERVER, AUTH_GAMEMASTER2   , "내리기 못하게 토글" )

	ON_TEXTCMDFUNC( TextCmd_System,                "system",             "sys",            "알림",           "알",      TCM_SERVER, AUTH_GAMEMASTER2   , "시스템 메시지" )

	// GM_LEVEL_3
	ON_TEXTCMDFUNC( TextCmd_PvpParam,              "PvpParam",           "p_Param",        "PVP설정",        "피설",    TCM_SERVER, AUTH_GAMEMASTER3, "PVP(카오)설정" )
//sun: 8, // __S8_PK
	ON_TEXTCMDFUNC( TextCmd_PKParam,			   "PKParam",			 "pkparam",		   "PK설정",		 "pk설정",  TCM_SERVER, AUTH_GAMEMASTER3, "카오설정" )

	ON_TEXTCMDFUNC( TextCmd_Undying,               "undying",            "ud",             "무적",           "무",      TCM_BOTH  , AUTH_GAMEMASTER3   , "무적" )
	ON_TEXTCMDFUNC( TextCmd_Undying2,              "undying2",           "ud2",            "반무적",         "반무",    TCM_BOTH  , AUTH_GAMEMASTER3   , "반무적" )
	ON_TEXTCMDFUNC( TextCmd_NoUndying,             "noundying",          "noud",           "무적해제",       "무해",    TCM_BOTH  , AUTH_GAMEMASTER3   , "무적 해제" )
	ON_TEXTCMDFUNC( TextCmd_Onekill,               "onekill",            "ok",             "초필",           "초필",    TCM_BOTH  , AUTH_GAMEMASTER3   , "적을 한방에 죽이기" )
	ON_TEXTCMDFUNC( TextCmd_NoOnekill,             "noonekill",          "nook",           "초필해제",       "초해",    TCM_BOTH  , AUTH_GAMEMASTER3   , "적을 한방에 죽이기 해제" )
	ON_TEXTCMDFUNC( TextCmd_AroundKill,            "aroundkill",         "ak",             "원샷",           "원",      TCM_SERVER, AUTH_GAMEMASTER3   , "어라운드에 있는 몬스터 죽이기" )
	ON_TEXTCMDFUNC( TextCmd_stat,                  "stat",               "stat",           "스탯",           "스탯",    TCM_SERVER, AUTH_GAMEMASTER3   , "스탯 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_Level,                 "level",              "lv",             "레벨",           "렙",      TCM_SERVER, AUTH_GAMEMASTER3   , "레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_InitSkillExp,          "InitSkillExp",       "InitSE",         "스킬초기화",     "스초",    TCM_SERVER, AUTH_GAMEMASTER3, "스킬초기화" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevel,            "skilllevel",         "slv",            "스킬레벨",       "스렙",    TCM_BOTH  , AUTH_GAMEMASTER3   , "스킬레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevelAll,         "skilllevelAll",      "slvAll",         "스킬레벨올",     "스렙올",  TCM_BOTH  , AUTH_GAMEMASTER3   , "스킬레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_BeginQuest,            "BeginQuest",         "bq",             "퀘스트시작",     "퀘시",    TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 시작 [ID]" )
	ON_TEXTCMDFUNC( TextCmd_EndQuest,              "EndQuest",           "eq",             "퀘스트종료",     "퀘종",    TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 종료 [ID]" )
	ON_TEXTCMDFUNC( TextCmd_RemoveQuest,           "RemoveQuest",        "rq",             "퀘스트제거",     "퀘제",    TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 제거 [ID]" )
	ON_TEXTCMDFUNC( TextCmd_RemoveAllQuest,        "RemoveAllQuest",     "raq",            "퀘스트전체제거", "퀘전제",  TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 전체 제거" )
	ON_TEXTCMDFUNC( TextCmd_RemoveCompleteQuest,   "RemoveCompleteQuest","rcq",            "퀘스트완료제거", "퀘완제",  TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 완료 제거" )
	ON_TEXTCMDFUNC( TextCmd_ChangeJob,             "changejob",			 "cjob",           "전직",           "전직",    TCM_SERVER, AUTH_GAMEMASTER3   , "전직 하기" )
	ON_TEXTCMDFUNC( TextCmd_Freeze,                "freeze",             "fr",             "정지",           "정지",    TCM_SERVER, AUTH_GAMEMASTER3   , "움직이지 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_NoFreeze,              "nofreeze",           "nofr",           "정지해제",       "정해",    TCM_SERVER, AUTH_GAMEMASTER3   , "움직이지 못하게 하기 해제" )
	ON_TEXTCMDFUNC( TextCmd_PartyLevel,            "PartyLevel",         "plv",            "극단레벨",       "극레",    TCM_SERVER, AUTH_GAMEMASTER3   , "극단레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_GuildStat,             "GuildStat",          "gstat",          "길드스탯",       "길스탯",  TCM_SERVER, AUTH_GAMEMASTER3   , "길드 스탯변경" )
	ON_TEXTCMDFUNC( TextCmd_CreateGuild,           "createguild",        "cg",             "길드생성",       "길생",    TCM_SERVER, AUTH_GAMEMASTER3   , "길드 생성" )
	ON_TEXTCMDFUNC( TextCmd_DestroyGuild,          "destroyguild",       "dg",             "길드해체",       "길해",    TCM_CLIENT, AUTH_GAMEMASTER3   , "길드 해체" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatIn,         "GCIn",               "gcin",           "길드워입장",     "길워입",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 입장" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatOpen,       "GCOpen",             "gcopen",         "길드워오픈",     "길워오",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 오픈" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatClose,      "GCClose",            "gcclose",        "길드워닫기",     "길워닫",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 닫기" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatNext,       "GCNext",             "gcNext",         "길드워다음",     "길워다",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 다음" )	
	ON_TEXTCMDFUNC( TextCmd_indirect,              "indirect",           "id",             "간접",           "간접",    TCM_BOTH  , AUTH_GAMEMASTER3   , "상대에게 간접으로 말하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_CreateNPC,             "createnpc",          "cn",             "엔피씨생성",     "엔생",    TCM_SERVER, AUTH_GAMEMASTER3   , "npc생성" )
//sun: 9, 이벤트 (루아 스크립트 적용)
	ON_TEXTCMDFUNC( TextCmd_LuaEventList,     "EVENTLIST",         "eventlist",          "이벤트목록",     "이벤트목록",    TCM_SERVER,  AUTH_GAMEMASTER3, "" )
	ON_TEXTCMDFUNC( TextCmd_LuaEventInfo,     "EVENTINFO",         "eventinfo",          "이벤트정보",     "이벤트정보",    TCM_SERVER,  AUTH_GAMEMASTER3, "" )	

	ON_TEXTCMDFUNC( TextCmd_GameSetting,           "gamesetting",        "gs",             "게임설정",       "게설",    TCM_SERVER, AUTH_GAMEMASTER3   , "게임 설정 보기" )
	ON_TEXTCMDFUNC( TextCmd_RemoveNpc,             "rmvnpc",             "rn",             "삭제",           "삭",      TCM_SERVER, AUTH_GAMEMASTER3, "NPC삭제" )

	// GM_LEVEL_4
	ON_TEXTCMDFUNC( TextCmd_Disguise,				"disguise",           "dis",            "변신",           "변",      TCM_SERVER, AUTH_ADMINISTRATOR   , "변신" )
	ON_TEXTCMDFUNC( TextCmd_NoDisguise,				"noDisguise",         "nodis",          "변신해제",       "변해",    TCM_SERVER, AUTH_ADMINISTRATOR   , "변신 해제" )
	ON_TEXTCMDFUNC( TextCmd_ResistItem,				"ResistItem",         "ritem",          "속성아이템",     "속아",    TCM_BOTH  , AUTH_ADMINISTRATOR, "속성아이템" )
	ON_TEXTCMDFUNC( TextCmd_JobName,				"jobname",            "jn",             "직업이름",       "직이",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "직업이름 보기" )
	ON_TEXTCMDFUNC( TextCmd_GetGold,				"getgold",            "gg",             "돈줘",           "돈",      TCM_SERVER, AUTH_ADMINISTRATOR, "돈 얻기" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem,				"createitem",         "ci",             "아이템생성",     "아생",    TCM_BOTH  , AUTH_ADMINISTRATOR, "아이템생성" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem2,			"createitem2",        "ci2",            "아이템생성2",    "아생2",   TCM_SERVER, AUTH_ADMINISTRATOR, "아이템생성2" )
	ON_TEXTCMDFUNC( TextCmd_QuestState,				"QuestState",         "qs",             "퀘스트상태",     "퀘상",    TCM_SERVER, AUTH_ADMINISTRATOR, "퀘스트 설정 [ID] [State]" )
	ON_TEXTCMDFUNC( TextCmd_LoadScript,				"loadscript",         "loscr",          "로드스크립트",   "로스",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "스크립트 다시 읽기" )
	ON_TEXTCMDFUNC( TextCmd_ReloadConstant,			"ReloadConstant",     "rec",            "리로드콘스탄트", "리콘",    TCM_SERVER, AUTH_ADMINISTRATOR, "리로드 콘스탄트파일" )
	ON_TEXTCMDFUNC( TextCmd_CTD,					"ctd",				 "ctd",            "이벤트듀얼존",   "이듀",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "이벤트 듀얼존 설정" )
	ON_TEXTCMDFUNC( TextCmd_Piercing,				"Piercing",           "pier",           "피어싱",         "피싱",    TCM_BOTH  , AUTH_ADMINISTRATOR, "피어싱(소켓)" )
//sun: 9, 9-10차 펫
	ON_TEXTCMDFUNC( TextCmd_PetLevel,				"petlevel",         "pl",          "펫레벨",     "펫레",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PetExp,					"petexp",         "pe",          "펫경험치",     "펫경",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_MakePetFeed,			"makepetfeed",         "mpf",          "먹이만들기",     "먹이",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_Pet,					"Pet",         "pet",          "펫",     "펫",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )

//sun: 9, 이벤트 (루아 스크립트 적용)
	ON_TEXTCMDFUNC( TextCmd_Lua,					"Lua",         "lua",          "루아",     "루아",    TCM_SERVER,  AUTH_ADMINISTRATOR, "" )

//sun: 11, 일대일 길드 대전
	ON_TEXTCMDFUNC( TextCmd_GC1to1Open,				"GC1TO1OPEN",		"gc1to1open",			"일대일대전오픈", "일오",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Close,			"GC1TO1CLOSE",		"gc1to1close",			"일대일대전닫기", "일닫",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Next,				"GC1TO1NEXT",		"gc1to1next",			"일대일대전다음", "일다",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RefineAccessory,		"RefineAccessory",	"ra",	"액세서리제련", "액제",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RefineCollector,		"RefineCollector",	"rc",	"채집기재련", "채제",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_InitializeRandomOption,	"InitializeRandomOption",	"iro",	"각성축복제거", "각축제거",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SetRandomOption,		"SetRandomOption",	"sro",	"각성축복지정", "각지",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

//sun: ?, __PET_1024
	ON_TEXTCMDFUNC( TextCmd_SetPetName,             "SetPetName",           "setpetname",             "펫작명",       "펫작",    TCM_SERVER, AUTH_ADMINISTRATOR , "펫작명" )
	ON_TEXTCMDFUNC( TextCmd_ClearPetName,           "ClearPetName",           "cpn",             "펫작명취소",       "펫작취",    TCM_CLIENT, AUTH_ADMINISTRATOR , "펫작명취소" )

//sun: 13, 커플 시스템
	ON_TEXTCMDFUNC( TextCmd_Propose,				"Propose",           "propose",             "프러포즈",       "프러포즈",    TCM_SERVER, AUTH_ADMINISTRATOR , "프러포즈" )
	ON_TEXTCMDFUNC( TextCmd_Refuse,					"Refuse",           "refuse",             "프러포즈거절",       "프거",    TCM_SERVER, AUTH_ADMINISTRATOR , "프러포즈거절" )
	ON_TEXTCMDFUNC( TextCmd_Couple,					"Couple",           "couple",             "커플",       "커플",    TCM_SERVER, AUTH_ADMINISTRATOR , "커플" )
	ON_TEXTCMDFUNC( TextCmd_Decouple,				"Decouple",           "decouple",             "커플해지",       "커해",    TCM_SERVER, AUTH_ADMINISTRATOR , "커플해지" )
	ON_TEXTCMDFUNC( TextCmd_ClearPropose,           "ClearPropose",           "clearpropose",             "프러포즈초기화",       "프초",    TCM_SERVER, AUTH_ADMINISTRATOR , "프러포즈초기화" )
//	ON_TEXTCMDFUNC( TextCmd_CoupleState,            "CoupleState",           "couplestate",             "커플상태",       "커상",    TCM_CLIENT, AUTH_ADMINISTRATOR , "커플상태" )
//sun: 13, 커플 보상
	ON_TEXTCMDFUNC( TextCmd_NextCoupleLevel,        "NextCoupleLevel",           "ncl",             "커플레벨업",       "커레",    TCM_SERVER, AUTH_ADMINISTRATOR , "커플레벨업" )

	ON_TEXTCMDFUNC( TextCmd_RemoveAllBuff,			"RemoveBuff",		"rb",			"버프해제", "버해",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	//sun: 13, 달인
	ON_TEXTCMDFUNC( TextCmd_HonorTitleSet,			"HonorTitleSet", "hts", "달인세팅", "달세", TCM_BOTH, AUTH_ADMINISTRATOR, "" )


	//	mulcom	BEGIN100819	QA팀 요청으로 GM 명령어 제한 해제
	ON_TEXTCMDFUNC( TextCmd_InvenClear,            "InvenClear",         "icr",            "인벤청소",       "인청",    TCM_SERVER, AUTH_ADMINISTRATOR, "인벤토리의 내용을 모두 삭제" )
	ON_TEXTCMDFUNC( TextCmd_InvenRemove,            "InvenRemove",         "irm",       "인벤삭제",       "인삭",    TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_HousingGMRemoveAll,		"HousingGMRemoveAll",	"hgmra",	"가구삭제", "가삭",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_CallTheRoll,			"CallTheRoll",        "ctr",            "출석설정",       "출석",  TCM_BOTH,	AUTH_ADMINISTRATOR, "출석 조작 명령어" )

	//sun: 12, 튜토리얼 개선
	ON_TEXTCMDFUNC( TextCmd_SetTutorialState,		"SetTutorialState", "sts", "튜토리얼레벨", "튜레", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_GuildHouseTenderOpen,	"GuildHouseTenderOpen",	"gto",	"길드하우스입찰오픈",	"길입오", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GuildHouseTenderStart,	"GuildHouseTenderStart",	"gts",	"길드하우스입찰시작",	"길입시", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GuildHouseTenderClose,	"GuildHouseTenderClose", "gtc",	"길드하우스입찰종료",	"길입종", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GuildHouseExpired,	"GuildHouseExpired", "ghe",	"길드하우스유지기간종료",	"길유종", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_EventArenaManageWindowOpen,	"arenaeventsetup",	"aesetup",	"아레나경기설정",	"경기설정", TCM_SERVER, AUTH_GAMEMASTER, "" )
#ifdef	__INTERNALSERVER
	ON_TEXTCMDFUNC( TextCmd_EventArenaManageWindowClose,	"arenaeventclose",	"aeclose",	"아레나경기설정닫기",	"경기설정닫기", TCM_SERVER, AUTH_GAMEMASTER, "" )
	ON_TEXTCMDFUNC( TextCmd_EventArenaManageGameForceEnd,	"arenaeventgameforceend",	"aeforceend",	"아레나게임강제종료",	"경기강제종료", TCM_SERVER, AUTH_GAMEMASTER, "" )
#endif

	ON_TEXTCMDFUNC( TextCmd_SetGiftPoint,			"SetGiftPoint", "setgp", "마드리갈선물포인트설정", "마포설정", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ViewGiftPoint,			"ViewGiftPoint", "viewgp", "마드리갈선물포인트보기", "마포보기", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

// 	ON_TEXTCMDFUNC( TextCmd_StartFlyffPieceBurningMode,			"StartFlyffPieceBurningMode", "bms", "프리프조각버닝시작", "버닝시작", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
// 	ON_TEXTCMDFUNC( TextCmd_EndFlyffPieceBurningMode,			"EndFlyffPieceBurningMode", "bme", "프리프조각버닝종료", "버닝종료", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	//sun: 13, 레인보우 레이스
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceDay,			"RRDay",	"rrday",	"레인보우요일", "레요",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceApp,			"RRApp",	"rrapp",	"레인보우신청", "레신",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceOpen,		"RROpen",	"rropen",	"레인보우오픈", "레오",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceNext,		"RRNext",	"rrnext",	"레인보우다음", "레다",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRacePass,		"RRPass",	"rrpass",	"레인보우패스", "레패",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )

//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceInfo,		"RRinfo",	"rrinfo",	"레인보우정보", "레정",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceRule,		"RRRule",	"rrrule",	"레인보우규칙", "레규",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceRanking,		"RRRanking","rrranking","레인보우랭킹", "레랭",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRacePrize,		"RRPrize",	"rrprize",	"레인보우상품", "레상",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_RainbowRaceKawiBawiBo,	"RRKawiBawiBo",	"rrkawibawibo",	"레인보우가위바위보", "레가",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceDice,		"RRDice",	"rrdice",	"레인보우주사위", "레주",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceArithmetic,	"RRArithmetic",	"rrarithmetic",	"레인보우수학", "레수",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )	
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceStopWatch,	"RRStopWatch",	"rrstopwatch",	"레인보우스톱워치", "레스",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceTyping,		"RRTyping",	"rrtyping",	"레인보우타자치기", "레타",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceCard,		"RRCard",	"rrcard",	"레인보우카드", "레카",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceLadder,		"RRLadder",	"rrladder",	"레인보우사다리", "레사",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceReqFininsh,	"RRFINISH",	"rrfinish",	"레인보우완주", "레완",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_OpenTreasureChest,		"OpenTreasureChest", "oc", "보물상자오픈", "보물상자오픈", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_DeleteAllMail,			"DeleteAllMail", "dm", "우편모두삭제", "우편모두삭제", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_DisplayAggro,		"DisplayAggro",			"aggro",		"어그로보기",	"어그로",	TCM_SERVER, AUTH_ADMINISTRATOR, "어그로보기" )

	//sun: 12, 비밀의 방
	ON_TEXTCMDFUNC( TextCmd_SecretRoomOpen,			"SROPEN",		"sropen",			"비밀의방오픈", "비오",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomNext,			"SRNEXT",		"srnext",			"비밀의방다음", "비다",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomEntrance,		"SRENTRANCE",		"srentrance",			"비밀의방입장", "비입장",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTender,		"SRTENDER",		"srtender",			"비밀의방입찰", "비입",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomLineUp,		"SRLINEUP",		"srlineup",			"비밀의방구성", "비구",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomClose,		"SRCLOSE",		"srclose",			"비밀의방닫기", "비닫",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTenderView,	"SRVIEW",		"srview",			"비밀의방입찰현황", "비현",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTenderCancelReturn, "SRCANCEL",		"srcancel",		"비밀의방입찰취소", "비취",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_ShowKeepTime,		"ShowKeepTime",			"st",		"접속시간보기",	"접속시간",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )


	//	mulcom	END100819	QA팀 요청으로 GM 명령어 제한 해제


// 여기부터 국내만 
	ON_TEXTCMDFUNC( TextCmd_Open,                  "open",               "open",           "열기",           "열기",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_Close,                 "close",              "close",          "닫기",           "닫기",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_Music,                 "music",              "mu",             "음악",           "음악",    TCM_SERVER, AUTH_ADMINISTRATOR   , "배경음악" )
	ON_TEXTCMDFUNC( TextCmd_Sound,                 "sound",              "so",             "소리",           "소리",    TCM_SERVER, AUTH_ADMINISTRATOR   , "사운드 효과" )
	ON_TEXTCMDFUNC( TextCmd_LocalEvent,            "localevent",         "le",             "지역이벤트",     "지이",    TCM_SERVER, AUTH_ADMINISTRATOR   , "지역이벤트" )
	ON_TEXTCMDFUNC( TextCmd_CommercialElem,        "CommercialElem",     "CommercialElem", "속성강화창",     "속강",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "속성강화창 띄우기" )
	ON_TEXTCMDFUNC( TextCmd_QuerySetPlayerName,    "SetPlayerName",      "spn",            "플레이어이름",   "플이",    TCM_SERVER, AUTH_ADMINISTRATOR   , "플레이어 이름 변경" )
	ON_TEXTCMDFUNC( TextCmd_QuerySetGuildName,     "SetGuildName",       "sgn",            "길드이름",       "길이",    TCM_SERVER, AUTH_ADMINISTRATOR   , "길드 이름 변경" )
	ON_TEXTCMDFUNC( TextCmd_DeclWar,               "DeclWar",            "declwar",        "길드전신청",     "길신",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "길드전 신청" )
	ON_TEXTCMDFUNC( TextCmd_RemoveGuildMember,     "rgm",                "rgm",            "길드추방",       "길추",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "길드 추방" )
	ON_TEXTCMDFUNC( TextCmd_GuildRankingDBUpdate,  "GuildRankingUpdate", "rankingupdate",  "길업",           "길업",    TCM_SERVER, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_ItemMode,              "gmitem",             "gmitem",         "아이템모드",     "아모",    TCM_SERVER, AUTH_ADMINISTRATOR   , "아이템 못집고 못떨어트리게 하기" )
	ON_TEXTCMDFUNC( TextCmd_ItemNotMode,           "gmnotitem",          "gmnotitem",      "아이템해제",     "아모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "아이템 모드 해제" )
	ON_TEXTCMDFUNC( TextCmd_AttackMode,            "gmattck",            "gmattck",        "공격모드",       "공모",    TCM_SERVER, AUTH_ADMINISTRATOR   , "공격 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_AttackNotMode,         "gmnotattck",         "gmnotattck",     "공격해제",       "공모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "공격 모드 해제" )
	ON_TEXTCMDFUNC( TextCmd_CommunityMode,         "gmcommunity",        "gmcommunity",    "커뮤니티모드",   "커모",    TCM_SERVER, AUTH_ADMINISTRATOR	, "길드, 파티, 친구, 거래, 상점 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_CommunityNotMode,      "gmnotcommunity",     "gmnotcommunity", "커뮤니티해제",   "커모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "커뮤니티 모드 해체" )
	ON_TEXTCMDFUNC( TextCmd_ObserveMode,           "gmobserve",          "gmobserve",      "관전모드",       "관모",    TCM_SERVER, AUTH_ADMINISTRATOR   , "아이템, 커뮤니티, 말못하게, 어텍 모드 합한것" )
	ON_TEXTCMDFUNC( TextCmd_ObserveNotMode,        "gmnotobserve",       "gmnotobserve",   "관전해제",       "관모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "관전 모드 해제" )
	ON_TEXTCMDFUNC( TextCmd_EscapeReset,           "EscapeReset",        "EscapeReset",    "탈출초기화",     "탈초",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "탈출(귀환석) 시간 초기화" )
//	ON_TEXTCMDFUNC( TextCmd_userlist,              "userlist",           "ul",             "사용자리스트",   "사용자리",TCM_SERVER, AUTH_ADMINISTRATOR, "사용자 리스트" )
//	ON_TEXTCMDFUNC( TextCmd_sbready,               "sbready",            "sbready",        "sbready",        "sbready", TCM_SERVER, AUTH_ADMINISTRATOR   , "sbready" )
//	ON_TEXTCMDFUNC( TextCmd_sbstart,               "sbstart",            "sbstart",        "sbstart",        "sbstart", TCM_SERVER, AUTH_ADMINISTRATOR   , "sbstart" )
//	ON_TEXTCMDFUNC( TextCmd_sbstart2,              "sbstart2",           "sbstart2",       "sbstart2",       "sbstart2",TCM_SERVER, AUTH_ADMINISTRATOR   , "sbstart2" )
//	ON_TEXTCMDFUNC( TextCmd_sbend,                 "sbend",              "sbend",          "sbend",          "sbend",   TCM_SERVER, AUTH_ADMINISTRATOR   , "sbend" )
//	ON_TEXTCMDFUNC( TextCmd_sbreport,              "sbreport",           "sbreport",       "sbreport",       "sbreport",TCM_SERVER, AUTH_ADMINISTRATOR   , "sbreport" )
//	ON_TEXTCMDFUNC( TextCmd_OpenBattleServer,      "bsopen",             "bsopen",         "bsopen",         "bsopen",  TCM_SERVER, AUTH_ADMINISTRATOR   , "bsopen" )
//	ON_TEXTCMDFUNC( TextCmd_CloseBattleServer,     "bsclose",            "bsclose",        "bsclose",        "bsclose", TCM_SERVER, AUTH_ADMINISTRATOR   , "bsclose" )
	ON_TEXTCMDFUNC( TextCmd_SetGuildQuest,         "SetGuildQuest",      "sgq",            "길드퀘스트",     "길퀘",    TCM_SERVER, AUTH_ADMINISTRATOR   , "길드 퀘스트 상태 변경" )
	ON_TEXTCMDFUNC( TextCmd_SetSnoop,              "Snoop",              "snoop",          "감청",           "감청",    TCM_SERVER, AUTH_ADMINISTRATOR   , "감청" )
	ON_TEXTCMDFUNC( TextCmd_SetSnoopGuild,         "SnoopGuild",         "sg",             "길드대화저장",   "길저",    TCM_SERVER, AUTH_ADMINISTRATOR   , "길드 대화 저장" )

	ON_TEXTCMDFUNC( TextCmd_GuildCombatRequest,    "GCRequest",          "gcrquest",       "길드워신청",     "길워신",  TCM_BOTH  , AUTH_ADMINISTRATOR, "길드대전 신청" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatCancel,     "GCCancel",           "gccancel",       "길드워탈퇴",     "길워탈",  TCM_BOTH  , AUTH_ADMINISTRATOR, "길드대전 탈퇴" )
//	ON_TEXTCMDFUNC( TextCmd_PostMail,              "PostMail",           "pm",             "편지발송",       "발송",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "편지 발송" )
//	ON_TEXTCMDFUNC( TextCmd_RemoveMail,            "RemoveMail",         "rm",             "편지삭제",       "편삭",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "편지 삭제" )
//	ON_TEXTCMDFUNC( TextCmd_GetMailItem,           "GetMailItem",        "gm",             "소포받기",       "소포",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "소포 받기" )
//	ON_TEXTCMDFUNC( TextCmd_GetMailGold,           "GetMailgOLD",        "gmg",            "수금",           "수금",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "수금" )
	ON_TEXTCMDFUNC( TextCmd_ExpUpStop,             "ExpUpStop",          "es",             "경험치금지",     "경금",    TCM_SERVER, AUTH_ADMINISTRATOR, "사냥으로 오르는 경험치 상승을 금지" )

#ifdef _DEBUG
	ON_TEXTCMDFUNC( TextCmd_CreateChar,            "createchar",         "cc",             "캐릭터생성",     "캐생",    TCM_SERVER, AUTH_ADMINISTRATOR   , "캐릭터생성" )
	ON_TEXTCMDFUNC( TextCmd_CreateCtrl,            "createctrl",         "ct",             "컨트롤생성",     "컨생",    TCM_SERVER, AUTH_ADMINISTRATOR   , "ctrl생성" )
	ON_TEXTCMDFUNC( TextCmd_SetMonsterRespawn,     "setmonsterrespawn",  "smr",            "리스폰영역설정", "리영설",  TCM_SERVER, AUTH_ADMINISTRATOR, "리스폰 영역 설정" )
	ON_TEXTCMDFUNC( TextCmd_TransyItemList,        "TransyItemList",     "til",            "트랜지리스트",   "트아리",  TCM_CLIENT, AUTH_ADMINISTRATOR, "트랜지아이템리스트" )
	ON_TEXTCMDFUNC( TextCmd_LoadToolTipColor,      "LoadToolTip",        "ltt",            "로드툴팁",       "로툴팁",  TCM_CLIENT, AUTH_ADMINISTRATOR, "로드 툴팁 컬러" )

	//added by gmpbigsun( 20101228 )
	ON_TEXTCMDFUNC( TextCmd_TestNewEnchantEffect,  "TestNewEnchant",     "tne",            "테스트인첸",     "테인",    TCM_CLIENT, AUTH_ADMINISTRATOR, "테스트 인첸효과" )
#endif

#ifdef __S1108_BACK_END_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_PropMonster,           "monstersetting",     "ms",             "몬스터설정",     "몬설",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "몬스터 설정 보기" )
#else
	ON_TEXTCMDFUNC( TextCmd_ChangeShopCost,	       "changeshopcost",	 "csc",	           "상점가격조정",   "상가조",  TCM_SERVER, AUTH_ADMINISTRATOR, "상점가격조정 Min(0.5) ~ Max(2.0)"  )
#endif // __S1108_BACK_END_SYSTEM

	
	//sun: 8차 엔젤 소환 Neuz, World, Trans
	ON_TEXTCMDFUNC( TextCmd_AngelExp,				"AExp",		"aexp",			"엔젤경험치", "엔경",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	
	//sun: 10, 속성제련 제거(10차로 변경)
	ON_TEXTCMDFUNC( TextCmd_RemoveAttribute,		"RemAttr",		"remattr",			"속성제거", "속제",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )

//sun: 11, 채집 시스템
	ON_TEXTCMDFUNC( TextCmd_StartCollecting,		"StartCollecting",	"col1",	"채집시작", "채시",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_StopCollecting,			"StopCollecting",	"col2",	"채집끝", "채끝",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_DoUseItemBattery,		"Battery",	"battery",	"채집기충전", "채충",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

//sun: 11, 주머니
	ON_TEXTCMDFUNC( TextCmd_AvailPocket,			"AvailPocket",	"ap",	"주머니사용", "주사",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PocketView,				"PocketView",	"pv",	"주머니보기", "주보",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_MoveItem_Pocket,		"MoveItemPocket",	"mip",	"아이템이동", "아이",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

//sun: 11, 각성, 축복
	ON_TEXTCMDFUNC( TextCmd_ItemLevel,				"ItemLevel",	"il",	"하락", "하락",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_Coupon,					"COUPON",		"coupon",			"쿠폰설정", "쿠폰",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )

//#ifdef __PERF_0226
	ON_TEXTCMDFUNC( TextCmd_CreatePc,				"CreatePc",		"cp",			"cp", "cp",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
//#endif	// __PERF_0226
#ifdef __SFX_OPT
	ON_TEXTCMDFUNC( TextCmd_SfxLv,					"SfxLevel",		"sl",			"sl", "sl",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif	

//sun: 12, 군주
	ON_TEXTCMDFUNC( TextCmd_ElectionRequirement,	"ElectionRequirement", "er", "군주투표현황", "군투현", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#ifdef __INTERNALSERVER
	ON_TEXTCMDFUNC( TextCmd_ElectionAddDeposit,		"ElectionAddDeposit", "ead", "군주입찰", "군입", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionSetPledge,		"ElectionSetPledge", "esp", "군주공약설정", "군공", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionIncVote,		"ElectionIncVote", "eiv", "군주투표", "군투", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionProcess,		"ElectionProcess", "ep", "군주프로세스", "군프", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionBeginCandidacy,	"ElectionBeginCandidacy", "ebc", "군주입후보시작", "군입시", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionBeginVote,		"ElectionBeginVote", "ebv", "군주투표시작", "군투시", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionEndVote,		"ElectionEndVote", "eev", "군주투표종료", "군투종", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionState,			"ElectionState", "estate", "군주투표상태", "군투상", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LEventCreate,			"LEventCreate", "lecreate", "군주이벤트시작", "군이시", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LEventInitialize,		"LEventInitialize", "leinitialize", "군주이벤트초기화", "군이초", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LSkill,					"LSkill", "lskill", "군주스킬", "군스", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RemoveTotalGold,		"RemoveTotalGold", "rtg", "돈삭제", "돈삭", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif	// __INTERNALSERVER


//sun: 12, 세금
	ON_TEXTCMDFUNC( TextCmd_TaxApplyNow,			"TaxApplyNow", "tan", "세율적용", "세적", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

#ifdef __INTERNALSERVER
	ON_TEXTCMDFUNC( TextCmd_TaxRateUpdate,			"TaxRateUpdate", "tru", "세율변경", "세변", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ViewPartyEffect,		"ViewPartyEffect", "viewpe", "파티효과보기", "파보", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_Mirchang_Test,			"mirchangtest", "mir", "미르창", "미르창", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif // __INTERNALSERVER

//sun: 12, 심연의 탑
	ON_TEXTCMDFUNC( TextCmd_HeavenTower,			"HeavenTower", "HTower", "심연의탑", "심탑", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
	ON_TEXTCMDFUNC( TextCmd_RemoveJewel,			"RemoveJewel", "RJewel", "보석제거", "보제", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

//sun: 12, 펫 알 변환 기능 추가
	ON_TEXTCMDFUNC( TextCmd_TransEggs,				"TransEggs", "TEggs", "알변환", "알변", TCM_BOTH, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_PickupPetAwakeningCancel,	"PickupPetAwakeningCancel",	"ppac",	"픽업펫각성취소", "픽소",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )


	ON_TEXTCMDFUNC( TextCmd_CreateLayer,            "CreateLayer",           "cl",             "레이어생성",       "레생",    TCM_SERVER, AUTH_ADMINISTRATOR , "레이어생성" )
	ON_TEXTCMDFUNC( TextCmd_DeleteLayer,            "DeleteLayer",           "dl",             "레이어파괴",       "레파",    TCM_SERVER, AUTH_ADMINISTRATOR , "레이어파괴" )
	ON_TEXTCMDFUNC( TextCmd_Layer,					"Layer",           "lay",             "레이어이동",       "레이",    TCM_SERVER, AUTH_ADMINISTRATOR , "레이어이동" )


//sun: 13, 하우징 시스템
	ON_TEXTCMDFUNC( TextCmd_HousingVisitRoom,		"HousingVisit",	"hv",	"방문", "방문",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
/*
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyNormal,		"SmeltSafetyNormal",	"ssn",	"안전제련일반", "안제일",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyAccessary,	"SmeltSafetyAccessary",	"ssa",	"안전제련악세", "안제악",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyPiercing,	"SmeltSafetyPiercing",	"ssp",	"안전제련피어싱", "안제피",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
*/

	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyElement,		"SmeltSafetyElement",	"sse",	"안전제련속성", "안제속",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_QuizEventOpen,			"QuizEventOpen",		"qeo",		"퀴즈오픈", "퀴오", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizEventEnterance,		"QuizEventEnterance",	"qee",		"퀴즈입장", "퀴입", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizStateNext,			"QuizStateNext",		"qsn",		"퀴즈다음", "퀴다", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizEventClose,			"QuizEventClose",		"qec",		"퀴즈종료", "퀴종", TCM_SERVER, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_BuyGuildHouse,			"BuyGuildHouse",		"bgh",		"길드하우스구입",	"길하구", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GuildHouseUpkeep,		"GuildHouseUpkeep",		"ghu",		"길드하우스유지비",	"길하유", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_RemoveCampusMember,		"RemoveCampusMember",	"rcm",		"사제해지",		"사해",		TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_UpdateCampusPoint,		"UpdateCampusPoint",	"ucp",		"사제포인트업",	"사포업",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )

//	ON_TEXTCMDFUNC( TextCmd_TimerSpeedUp,		"TimerSpeedUp",			"tsu",		"타이머스피드업",	"타이머스피드",	TCM_SERVER, AUTH_ADMINISTRATOR, "타이머스피드업" )

	ON_TEXTCMDFUNC( TextCmd_DonationBuffInfo, "DonationBuffInfo", "dbi", "협동기부버프정보", "협버정", TCM_SERVER, AUTH_GAMEMASTER, "협동 기부 버프 정보" )
#ifdef COMMAND_NPC_OUTPUT
	ON_TEXTCMDFUNC( TextCmd_NpcOutPut,		"NpcOutPut",	"nop",		"엔피씨출력",		"엔출",		TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif
END_TEXTCMDFUNC_MAP


int ParsingCommand( LPCTSTR lpszString, CMover* pMover, BOOL bItem )
{
	CScanner scanner;
	scanner.SetProg( (LPTSTR)lpszString );
	scanner.dwValue	= (DWORD_PTR)pMover;
	scanner.GetToken(); // skip /
	scanner.GetToken(); // get command

	int nCount = 0;
	while( m_textCmdFunc[ nCount ].m_pFunc )
	{
		TextCmdFunc* pTextCmdFunc = &m_textCmdFunc[nCount];			// 해외 명령어 제한 
		if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
		{
			if( memcmp( pTextCmdFunc->m_pCommand, "open", 4 ) == 0 )
				break;
		}

		if( scanner.Token == pTextCmdFunc->m_pCommand || scanner.Token == pTextCmdFunc->m_pAbbreviation ||
			scanner.Token == pTextCmdFunc->m_pKrCommand || scanner.Token == pTextCmdFunc->m_pKrAbbreviation )
		{
		#ifdef __CLIENT
			if( scanner.Token == "disguise" || scanner.Token == "dis" || scanner.Token == "변신" || scanner.Token == "변" ||
				scanner.Token == "noDisguise" || scanner.Token == "nodis" || scanner.Token == "변신해제" || scanner.Token == "변해" )
			{
				g_WndMng.PutString( "Not Command!" );
				return TRUE;
			}				
		#endif // __CLIENT

			if( bItem == FALSE && pTextCmdFunc->m_dwAuthorization > pMover->m_dwAuthorization )
				break;

		#ifdef __CLIENT
			if( pTextCmdFunc->m_nServer == TCM_CLIENT || pTextCmdFunc->m_nServer == TCM_BOTH )
			{
				if( ( *pTextCmdFunc->m_pFunc )( scanner ) )
					if( pTextCmdFunc->m_nServer == TCM_BOTH )
					{
						char szSendChat[MAX_PATH] = { 0,};
						FLSPrintf( szSendChat, _countof( szSendChat ), "%s", scanner.m_pBuf );
						g_DPlay.SendChat( (LPCSTR)szSendChat );
					}
			}
			else
				g_DPlay.SendChat( (LPCSTR)lpszString );
		#else	// __CLIENT
			if( pTextCmdFunc->m_nServer == TCM_SERVER ||  pTextCmdFunc->m_nServer == TCM_BOTH )
			{
				( *pTextCmdFunc->m_pFunc )( scanner );
			}
		#endif	// __CLIENT
			return TRUE;
		}
		nCount++;
	}

#ifdef __CLIENT
	BOOL bSkip = FALSE;
	CString strTemp = lpszString;
	
	if( strTemp.Find( "#", 0 ) >= 0 )
		bSkip = TRUE;

	int nstrlen = strlen(lpszString);

	if( !bSkip )
	{
		TCHAR	szText[MAX_EMOTICON_STR];

		if(nstrlen < MAX_EMOTICON_STR)
			FLStrcpy( szText, _countof( szText ), lpszString );
		else
		{
			FLStrncpy(szText, _countof( szText ), lpszString, MAX_EMOTICON_STR);
			szText[MAX_EMOTICON_STR-1] = NULL;
		}

		// 이모티콘 명령
		for( int j=0; j < MAX_EMOTICON_NUM; j++ )
		{
			if( _stricmp( &(szText[1]), g_DialogMsg.m_EmiticonCmd[ j ].m_szCommand ) == 0 )			
	//		if( _tcsicmp( szText, g_DialogMsg.m_EmiticonCmd[ j ].m_szCommand ) == 0 )
			{
				g_DPlay.SendChat( (LPCSTR)lpszString );
				return TRUE;
			}
		}
	}
#endif	//__CLIENT

	return FALSE;
}

void RemoveCRLF( char* szString, size_t cchString )
{
	CString str		= szString;
	str.Replace( "\\n", " " );
	FLStrcpy( szString, cchString, (LPCSTR)str );
}

void ParsingEffect( TCHAR* pChar, size_t cchChar, int nLen )
{
	CString strTemp;
	
	for( int i = 0; i < nLen; i++ )
	{
		if( pChar[ i ] == '#' ) // 인식 코드
		{
			if( ++i >= nLen )
				break;
			switch( pChar[ i ] )
			{
			case 'c':
				{
					if( ++i >= nLen )
						break;
					
					i += 7;
				}
				break;
			case 'u':
				break;
			case 'b':
				break;
			case 's':
				break;
				
			case 'l':
				{
					if(++i >= nLen)
						break;
					
					i += 3;
				}				
				break;
			case 'n':
				if( ++i >= nLen )
					break;
				
				{					
					switch( pChar[ i ] )
					{
					case 'c':
						break;
					case 'b':
						break;
					case 'u':
						break;
					case 's':
						break;
					case 'l':
						break;
					}
				}
				break;
			default: // 명령코드를 발견 못했을 경우 
				{
					// #코드를 넣어준다
					strTemp += pChar[ i - 1 ];
					strTemp += pChar[ i ];
				}
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
// 		else if( pChar[ i ] == '$' )	// 아이템 링크에 쓰이는 명령코드 : "$i%x%0.8d%0.4d<%s>$ni"
// 		{
// 			if( ++i >= nLen )
// 			{
// 				break;
// 			}
// 
// 			switch( pChar[ i ] )
// 			{
// 			case 'i':
// 				{
// 					i	+=	8 + 8 + 4;
// 				}
// 				break;
// 
// 			default:
// 				{
// 					strTemp += pChar[ i - 1 ];
// 					strTemp += pChar[ i ];
// 				}
// 				break;
// 			}
// 		}
		//////////////////////////////////////////////////////////////////////////
		else
		{
			if( pChar[ i ] == '\\' && pChar[ i+1 ] == 'n' )
			{
				strTemp += '\n';
				i+=1;
			}
			else
			{
				strTemp += pChar[ i ];
				int nlength = strTemp.GetLength();
				UNREFERENCED_PARAMETER( nlength );
			}
		}
	}
	
//	memcpy( pChar, strTemp, sizeof(TCHAR)*nLen );
	FLStrcpy( pChar, cchChar, strTemp );
}
