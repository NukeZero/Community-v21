// Mover.cpp: implementation of the CMover class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineSkill.h"
#include "lang.h"
//#include "..\_DirectX\d3dfile.h"
#include "ModelObject.h"  
#include "authorization.h"
#include "CreateObj.h"
#ifdef __SCHOOL0701
#include "eveschool.h"
#ifdef __GUILDCOMBAT
#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;
#endif // __GUILDCOMBAT
#endif	// __SCHOOL0701

#ifdef __DEADLOCK__
#include "..\_UnhandledException\ExceptionHandler.h"
#endif	// __DEADLOCK__

#ifdef __CLIENT
 #include "Resdata.h"
 #include "DPClient.h"
 #include "DialogMsg.h"
 extern	CDPClient	g_DPlay;
 #include "GuildRender.h"
#else	// __CLIENT
 #ifdef __WORLDSERVER
  #include "User.h"
  #include "UserMacro.h"
  #include "WorldMng.h"
  #include "DPSrvr.h"
  #include "DPCoreClient.h"
  #include "dpdatabaseclient.h"
  extern	CUserMng	g_UserMng;
  extern	CWorldMng	g_WorldMng;
  extern	CDPSrvr		g_DPSrvr;
  extern	CDPDatabaseClient	g_dpDBClient;
  extern	CDPCoreClient	g_DPCoreClient;
 #endif	// __WORLDSERVER
#endif	// __CLIENT

#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "definequest.h"

#include "Ship.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __WORLDSERVER
CMoverPool*	CMover::m_pPool	= new CMoverPool( 512 );
#else	// __WORLDSERVER
#ifndef __BEAST
CMoverPool*	CMover::m_pPool	= new CMoverPool( 128 );
#endif // __BEAST
#endif	// __WORLDSERVER


const int	MAX_DIALOGFILENAME = 32;

#ifdef __V050322_CACHEQUEST
#ifdef __CLIENT
// 퀘스트 아이콘 확축 프로세스 
BOOL CMover::m_bQuestEmoticonAdd = TRUE;
FLOAT CMover::m_fQuestEmoticonScale = 1.0f;
#endif
#endif // __V050322_CACHEQUEST

// 비스트일 때는 컴팔 안되게..
#ifdef __BEAST
#define UpdateParam()        0
#define GetMaxFatiguePoint() 0
#define GetMaxHitPoint()     0 
#define GetMaxManaPoint()    0
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMover::CMover()
#ifdef __INFOCRITSECT_DEBUG__
: m_LockofidEnemies( "CMover::m_LockofidEnemies" )
#endif	// __INFOCRITSECT_DEBUG__
{
	Init();
}

CMover::~CMover()  
{
#ifdef __XLADOLF
	SAFE_DELETE( m_pLadolf );		// g_Object3DMng가 파괴되기전에 부를것
#endif

	SAFE_DELETE( m_pNpcProperty );

#ifndef __BEAST
	//raiders_fixed
	CMover* pOther = m_vtInfo.GetOther();
	if( IsValidObj( pOther ) )
	{
		if( pOther->m_vtInfo.GetOther() == this )
			pOther->m_vtInfo.TradeClear();
	}
#endif // __BEAST

#ifdef __CLIENT
	m_vtInfo.VendorClose();
	#ifdef __BUGFIX_DIALOGMSG1112
		g_DialogMsg.ClearMessage( (CObj*)this );
	#endif	// __BUGFIX_DIALOGMSG1112
#endif	// __CLIENT

	SAFE_DELETE( m_pActMover );

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		SAFE_DELETE( m_ShopInventory[ i ] );

	if( GetWorld() )
	{
	#if !defined(__BEAST) && !defined(__ONLINE)
		#ifdef __V050426_REMOVERESPAWN  
			GetWorld()->m_respawner.Increment( GetRespawn(), GetRespawnType(), m_bActiveAttack );
		#else
		//{{AFX
			GetWorld()->m_respawner.Increment( GetRespawn(), m_bActiveAttack );
		//}}AFX
		#endif
	#endif
	}

#ifdef __V050322_CACHEQUEST
	SAFE_DELETE_ARRAY( m_aQuest );
	SAFE_DELETE_ARRAY( m_aCompleteQuest );
#endif
}

void CMover::Init()
{
	m_nCount			= xRandom( 128 );		// 리스폰 되는 몹마다 시작카운터를 약간씩 틀리게 해서 좀더 다양한 반응이 나오게 했다.
	m_dwType			= OT_MOVER;
	m_bPlayer			= FALSE;
	m_dwTypeFlag		= 0;
	m_pRide				= NULL;  
	m_pAIInterface		= NULL;
	m_nHitPoint			= 100;
	m_nManaPoint		= 100;
	m_nFatiguePoint		= 100;
	m_vDestPos			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_idDest			= NULL_ID;
	m_fArrivalRange		= 0.0f;
	m_fTurnAngle		= 0.0f;
	m_bySex				= SEX_FEMALE;
	m_dwVirtItem		= II_WEA_HAN_HAND;
	m_dwVirtType		= VT_ITEM;
	m_nHitPoint			= 100;
	m_nManaPoint		= 100;
	m_nFatiguePoint		= 100;
	m_dwRideItemIdx		= II_RID_RID_STI_MAGIC;
//	m_nStyle			= 0;
	m_nRemainGP			= 0;
	m_nRemainLP			= 0;
	m_nDefenseMin		= 0;
	m_nDefenseMax		= 0;
	m_nJob				= -1;
	memset( m_szName, 0, sizeof(TCHAR) );
	m_nAdjHitRate		= 0;
	m_nAdjParry         = 0;
	m_nStr				= 15;
	m_nSta				= 15;
	m_nDex				= 15;
	m_nInt				= 15;
	m_nLevel			= 1;
	m_nExp1				= 0;
	m_nExp2				= 0;
	m_nDeathExp			= 0;
	m_nDeathLevel		= 0;
	m_dwFace			= 0;
	m_dwSkinSet			= 0;
	m_dwHairMesh		= 0;
	m_dwHairColor		= 0;
	m_dwHeadMesh		= 0;
	m_nFlightLv			= 0;
	m_nFxp				= 0;
	m_szCharacterKey[ 0 ] = 0;
	{
		int i;
		for( i = 0; i < MAX_ADJPARAMARY; i ++ )		m_adjParamAry[i] = 0;
		for( i = 0; i < MAX_ADJPARAMARY; i ++ )		m_chgParamAry[i] = 0x7FFFFFFF;	// 이유가 있다.
	}

#ifdef __FINITEITEM0705
	ClearEquipInfo();
#else	// __FINITEITEM0705
	memset( m_adwEquipment, 0xff, sizeof( m_adwEquipment ) );
#endif	// __FINITEITEM0705

	m_vtInfo.Init( this );

	m_idPlayer			= NULL_ID;
	m_dwAuthorization	= AUTH_GENERAL;
	m_idMarkingWorld	= NULL_ID;
	m_dwRegionAttr		= 0;
	m_dwOldRegionAttr	= 0;
	m_bPositiveX		= m_bPositiveZ	= false;
	m_fDestAngle		= -1.0f;
	m_bLeft				= false;
	m_bForward			= true;
	m_dwAuthorization	= AUTH_GENERAL;
	m_dwMode			= 0;
	memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	m_fWaitQueryGetPos	= FALSE;
	m_nCorr				= 0;
	m_uRemnantCorrFrm	= 0;
	m_fHairColorR		= 1.0f;
	m_fHairColorG		= 1.0f;
	m_fHairColorB		= 1.0f;
	m_dwHairColor		= D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f );
	m_nQuestSize		= 0;
#ifdef __V050322_CACHEQUEST
	m_aQuest	        = NULL;//new QUEST[ prj.m_aPropQuest.GetSize() ];
	m_aCompleteQuest    = NULL; 
	m_nCompleteQuestSize = 0; 
#ifdef __WORLDSERVER
	m_timerQuestLimitTime.Set( MIN( 1 ) );
	m_dwPKTargetLimit = 0;
#else // __WORLDSERVER
#ifndef __BEAST
	m_nQuestEmoticonIndex = -1; 
	m_bShowQuestEmoticon = TRUE;
#endif
#endif // __WORLDSERVER
#else // __V050322_CACHEQUEST
	m_pCacheQuest		= NULL;
	ZeroMemory( m_aQuest, sizeof( m_aQuest ) );
#endif // __V050322_CACHEQUEST
	m_idparty			= 0;
//	m_dwContribution	= 0;

#ifdef __XOPT0617
	m_nWeaponOption		= 0;
#else	// __XOPT0617
#ifndef __FINITEITEM0705
	memset(	m_anEquipOption, 0, sizeof(m_anEquipOption) );
#endif	// __FINITEITEM0705
#endif // NOT __XOPT0617

#ifdef	__BEAST
	m_dwExtraFlag		= 0;
#else	// !__BEAST
	m_oaCmd				= m_oaAct = OBJACT_NONE;
	memset( m_nCParam, 0, sizeof(m_nCParam) );
	memset( m_nAParam, 0, sizeof(m_nAParam) );
	m_dwFlag			= 0;
	m_nAtkCnt			= 0;
	m_wStunCnt			= 0;
	m_SkillState.Init();
	m_SkillState.SetMover( this );
	m_idAttacker		= NULL_ID;
	m_idTargeter		= NULL_ID;
	m_idTracking		= NULL_ID;
	m_fExpRatio			= 1.0f;
	m_nNumKill			= 0;				// 킬수

	m_tmCrime			= 0;				// 범죄자상태 유지시간
	m_idMurderer		= NULL_ID;
	m_tmDead			= 0;
	m_tmActionPoint		= timeGetTime();
	m_nDead				= 0;
	m_nSlaughter		= 0;				// 슬로터 포인트
#ifdef __ACROBAT_0504
	m_wDarkCover        = 0;
#endif //__ACROBAT_0504	
#ifdef __NPP_050308
	m_nRecoverChao_Slaughter = 0;		// 시련모드일때 슬로터 포인트
//	m_dwPKRelease	= 0;
#endif //__NPP_050308	
	m_idLastHitMover	= NULL_ID;			// this가 마지막으로 쳤던 무버아이디

	m_nFame				= 0;				// 명성
	m_nDuel				= 0;				// 듀얼중인가.
	m_idDuelOther		= NULL_ID;
	m_idDuelParty		= 0;
	m_nFuel				= -1;				// 초기값은 -1이다.
	m_tmAccFuel			= 0;				// 0이면 가속 못하는 상태.
	//	lenajang : 아래 두 변수의 초기화를 __BEAST디파인이 없을때 하는 것으로 바뀌었음 9.16
#ifndef __BEAST
	m_fAniSpeed = 1.0f;
#endif // not Beast
	m_idPoisonAttacker = NULL_ID;
	m_wPoisonCnt = m_tmPoisonUnit = 0;
	m_wPoisonDamage = 0;
	m_wDarkCnt = m_wDarkVal = 0;

	m_idBleedingAttacker = NULL_ID;
	m_wBleedingCnt = m_tmBleedingUnit = 0;
	m_wBleedingDamage = 0;
	
  #ifdef	__XFLYAI0524
	m_nMovePattern		= 0;
	m_nMoveEvent		= 0;
	m_nMoveEventCnt		= 0;
  #endif	//__XFLYAI0524
#endif	// !__BEAST

	m_idGuild			= 0;
	m_idGuildCloak		= 0;
	m_bGuildBank		= FALSE;
	m_idWar				= 0;

	m_nPlusMaxHitPoint = 0;

#ifndef __BEAST
	memset( m_dwSMTime, 0, sizeof( DWORD ) * SM_MAX );
#endif

	m_nAttackResistLeft = (BYTE)0xff;
	m_nAttackResistRight = (BYTE)0xff;
	m_nDefenseResist = (BYTE)0xff;

#ifdef __WORLDSERVER
	m_bLastPK		= FALSE;
	m_bLastDuelParty = FALSE;
  #ifdef __GUILDCOMBAT
	m_bGuildCombat = FALSE;
  #endif // __GUILDCOMBAT

	m_tGuildMember	= CTime::GetCurrentTime();

	m_nQuestKeeping	= 0;			// 길드퀘스트 클락워크 퀘스트와 관련 

	m_idCollecter	= NULL_ID;
	m_nResource		= 0;
	m_nCollectOwnCnt = 0;
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_nDmgCnt			= 0;
	m_uHead				= m_uTail	= 0;
	m_nWaterCircleCount = 0;
	m_idSfxHit		= 0;
	m_dwReqFlag		= 0;
	m_pCloakTexture = NULL;
	m_fDestScale	= 1.0f;
	m_fDestScaleSlerp = 0;
#else // CLIENT
	m_nReflexDmg = 0;
#endif	// not Client

	m_pActMover = new CActionMover( this );
	m_pActMover->SetMover( this );

#ifdef __BALANCE_050614
	m_timerRecovery.Set( SEC(3) );
	m_timerRecoveryStand.Set( SEC(6) );
#else
	m_timerRecovery.Set( SEC(6) );
	m_timerRecoveryStand.Set( SEC(8) );
#endif

	m_timerDuel.Set( SEC(1) );
	m_timerEndDuel.Set( MIN(3) );
	m_nDuelState = 0;

#ifdef __NPP_050308
	m_dwKarmaTick = 0;
	memset( &m_Resurrection_Data, 0, sizeof(m_Resurrection_Data) );
#endif

	m_bRegenItem		= TRUE;
	m_bActiveAttack		= FALSE;
	m_dwGold			= 0;		// 0으로 할것.  -xuzhu-

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		m_ShopInventory[ i ] = 0;
	
	m_lpfGetResist[SAI79::NO_PROP		]	= GetResistNULL;
	m_lpfGetResist[SAI79::FIRE			]	= GetResistFire;
	m_lpfGetResist[SAI79::WATER			]	= GetResistWater;
	m_lpfGetResist[SAI79::ELECTRICITY	]	= GetResistElectricity;
	m_lpfGetResist[SAI79::WIND			]	= GetResistWind;
	m_lpfGetResist[SAI79::EARTH			]	= GetResistEarth;

#ifndef __V050322_CACHEQUEST
//{{AFX
	m_dwCurQuest = 0;
//}}AFX
#endif
	m_pNpcProperty = NULL;
#if defined(__CLIENT) && !defined(__ONLINE)
	InitNPCProperty();
#endif
	m_dwSkillPoint = 0;
	
#ifdef	__XLADOLF
	m_dwLadolfFlag = 0;
	m_pLadolf = NULL;
#endif //__XLADOLF
	m_fCrrSpd	= 0.0f;
#ifdef __S0504_FLY
	m_dwStateMode = 0;
	m_nReadyTime = 0;
	m_dwUseItemId = 0;
	m_bItemFind = TRUE;
#endif // __S0504_FLY	

#ifdef __CHEER0615
	m_dwTickCheer= 0;
	m_nCheerPoint	= 0;
#endif	// __CHEER0615
	m_pIADestPos = NULL;

	m_idPet = NULL_ID;

#ifdef __INSTANT_BANK0831
	m_dwTickCreated	= 0;
#endif	// __INSTANT_BANK0831
	
#ifdef __GUILDCOMBAT
	m_nGuildCombatState = 0;
#endif // __GUILDCOMBAT
}

BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	if( nGold == 0 )
		return TRUE;

#ifdef __WORLDSERVER
	if( m_vtInfo.TradeGetGold() != 0 )
		return FALSE;
#endif // __WORLDSERVER

	int nTotal = GetGold() + nGold;

	if( nGold > 0 )
	{
		if( nTotal < 0 )		// overflow?
			nTotal = INT_MAX;
	}
	else
	{
		if( nTotal < 0 )		// underflow?
			return FALSE;
	}

	SetGold( nTotal );

	if( bSend )
	{
	#ifdef __WORLDSERVER
		extern CUserMng	g_UserMng;
		g_UserMng.AddSetPointParam( this, DST_GOLD, nTotal );
	#endif	// __WORLDSERVER
	}

	return TRUE;
}


// 새로운 머리색깔이 적용되기 전에 이함수를 사용해야함...
// 원본 머리색깔을 참고하기 때문...
int	CMover::GetHairCost( CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair )
{
	BYTE nOrignalR = pMover->m_fHairColorR * 255;
	BYTE nOrignalG = pMover->m_fHairColorG * 255;
	BYTE nOrignalB = pMover->m_fHairColorB * 255;

	int nHairCost = 0;
	int nHairColorCostR = 0;
	int nHairColorCostG = 0;
	int nHairColorCostB = 0;
	
	if( nR >= nOrignalR  )
		nHairColorCostR = (nR - nOrignalR)*13;
	else
		nHairColorCostR = (nOrignalR - nR)*7;
	
	if( nG >= nOrignalG  )
		nHairColorCostG = (nG - nOrignalG)*13;
	else
		nHairColorCostG = (nOrignalG - nG)*7;
	
	if( nB >= nOrignalB  )
		nHairColorCostB = (nB - nOrignalB)*13;
	else
		nHairColorCostB = (nOrignalB - nB)*7;
	
	if( pMover->m_dwHairMesh+1 != nHair+1 )
	{
		switch( nHair+1 )
		{
			case 1: nHairCost = 2500;	break;
			case 2:	nHairCost = 2500;	break;
			case 3:	nHairCost = 2500;	break;
			case 4:	nHairCost = 2500;	break;
			case 5:	nHairCost = 2500;	break;
			default: nHairCost = 4000;	break;
		}
	}
	else
	{
		nHairCost = 0;
	}

	return ( nHairColorCostR + nHairColorCostG + nHairColorCostB + nHairCost );
}

#ifdef __WORLDSERVER
void CMover::SubSMMode()
{
	for( int i = 0 ; i < SM_MAX ; ++i )
	{
		if( 0 < m_dwSMTime[i] )
		{
			if( g_AddSMMode.bSMModetime[i] )	// 절대시간
			{
				CTime time	= CTime::GetCurrentTime();
				if( m_dwSMTime[i] < time.GetTime() )
					m_dwSMTime[i] = 0;
			}
			else	// 카운트
			{
#ifdef __S050524_SM_ITEM
				if( i != SM_REVIVAL )
#endif // __S050524_SM_ITEM
					--m_dwSMTime[i];	
			}

			if( m_dwSMTime[i] == 0 )
			{
#ifdef __NPP_050308
				if( i == SM_STR_DOWN || i == SM_STA_DOWN || i == SM_INT_DOWN || i == SM_DEX_DOWN )
				{
					KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nRecoverChao_Slaughter);
					
					if( pKarmaProp )
					{
						// 함께 걸려있던 다른것들 모두 해제가 된거라면 시련모드 해제
						if( !IsSMMode(SM_STR_DOWN) && 
							!IsSMMode(SM_STA_DOWN) &&
							!IsSMMode(SM_INT_DOWN) &&
							!IsSMMode(SM_DEX_DOWN) )
						{
							// 시련모드 해제		
							((CUser*)this)->SetNotMode(RECOVERCHAO_MODE);
							((CUser*)this)->AddHdr( GETID( this ), SNAPSHOTTYPE_END_RECOVERMODE );

							// 시련모드가 끝났으니 카르마포인트를 회복시켜준다
							ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, m_nSlaughter + pKarmaProp->nKarmaRecoverNum );

							((CUser*)this)->AddDefinedText( TID_PK_PUNISH_END1, "" );	// 모든 시련을 견뎌내었습니다.
						}
					}						
				}				
#endif //__NPP_050308

				CItemElem* pItemElemParts = NULL;
				if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT )
				{
					int nAttackResist;
					if( i == SM_RESIST_ATTACK_LEFT )
						nAttackResist = m_nAttackResistLeft;
					else
						nAttackResist = m_nAttackResistRight;

					switch( nAttackResist )
					{
					case SAI79::FIRE:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_FIREASTONE;
						break;
					case SAI79::WATER:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_WATEILSTONE;
						break;
					case SAI79::WIND:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_WINDYOSTONE;
						break;
					case SAI79::ELECTRICITY:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_LIGHTINESTONE;
						break;
					case SAI79::EARTH:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_EARTHYSTONE;
						break;
					}
					if( i == SM_RESIST_ATTACK_LEFT )
					{
						m_nAttackResistLeft = (BYTE)0xff;
						pItemElemParts = m_Inventory.GetEquip( PARTS_LWEAPON );
					}
					else
					{
						m_nAttackResistRight = (BYTE)0xff;
						pItemElemParts = m_Inventory.GetEquip( PARTS_RWEAPON );
					}
					
				}
				else if( i == SM_RESIST_DEFENSE )
				{
					switch( m_nDefenseResist )
					{
					case SAI79::FIRE:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEFIREASTONE;
						break;
					case SAI79::WATER:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEWATEILSTONE;
						break;
					case SAI79::WIND:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEWINDYOSTONE;
						break;
					case SAI79::ELECTRICITY:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DELIGHTINESTONE;
						break;
					case SAI79::EARTH:
						g_AddSMMode.dwSMItemID[i] = II_CHR_SYS_SCR_DEEARTHYSTONE;
						break;
					}
					m_nDefenseResist = (BYTE)0xff;
					pItemElemParts = m_Inventory.GetEquip( PARTS_UPPER_BODY );
				}

				if( pItemElemParts && ( i == SM_RESIST_DEFENSE || i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT ) )
				{
					pItemElemParts->m_nResistSMItemId = 0;
					((CUser*)this)->AddCommercialElem( pItemElemParts->m_dwObjId, 0 );
				}

				ItemProp* aItemprop = prj.GetItemProp( g_AddSMMode.dwSMItemID[i] );
				if( aItemprop )
				{
					if( i == SM_MAX_HP )
					{
						ResetDestParam( aItemprop->dwDestParam[0], aItemprop->nAdjParamVal[1], 1 );
					}
					else if( i == SM_MAX_HP50 )
					{
						ResetDestParam( aItemprop->dwDestParam[0], m_nPlusMaxHitPoint, 1 );
					}
#ifdef __S050524_SM_ITEM
					else if( i == SM_VELOCIJUMP )
					{
						if( aItemprop->dwDestParam1 != -1 )
							ResetDestParam( aItemprop->dwDestParam1, aItemprop->nAdjParamVal1 );
						if( aItemprop->dwDestParam2 != -1 )
							ResetDestParam( aItemprop->dwDestParam2, aItemprop->nAdjParamVal2 );
					}
#endif // __S050524_SM_ITEM
					g_dpDBClient.SendLogSMItemUse( "2", (CUser*)this, NULL, aItemprop );
				}
				else
				{
					WriteLog( "%s, %d\r\n\tNo SMItem dwSMItemID[%d] : %d", __FILE__, __LINE__, i, g_AddSMMode.dwSMItemID[i] );
				}

				((CUser*)this)->AddSMMode( i, 0 );

			}
		}
	}
}

BOOL CMover::SetSMMode( int nType, DWORD dwTime )
{
	if( g_AddSMMode.bSMModetime[nType] )	// 절대시간
	{
		CTime tRealtime	= CTime::GetCurrentTime();
		CTimeSpan ctp( 0, 0, 0, dwTime );
		tRealtime += ctp;
		m_dwSMTime[nType] = tRealtime.GetTime();
	}
	else	// 카운트
	{
		m_dwSMTime[nType] = dwTime;
	}

	((CUser*)this)->AddSMMode( nType, m_dwSMTime[nType] );
	
	return TRUE;
}

#endif // __WORLDSERVER

LPCHARACTER CMover::GetCharacter()
{
	return prj.GetCharacter( m_szCharacterKey );
}
void CMover::SetMarkingPos()
{
	chASSERT( GetWorld() );
	m_vMarkingPos = GetPos();
	m_idMarkingWorld	= GetWorld()->GetID();
}

// NPC관련 프로퍼티를 초기화한다.
void CMover::InitNPCProperty()
{
	if( m_pNpcProperty == NULL )
		m_pNpcProperty = new CNpcProperty;
}

void CMover::InitCharacter( LPCHARACTER lpCharacter )
{
	if( lpCharacter )
	{
		strcpy( m_szName, lpCharacter->m_strName );
		for( int i = 0; i < lpCharacter->m_nEquipNum; i++ )
		{
			BYTE nId, nCount;
			short nNum;
			CItemElem itemElem;
			itemElem.m_dwItemId	= lpCharacter->m_adwEquip[ i ];
			itemElem.m_nItemNum	= 1;
			m_Inventory.Add( &itemElem, &nId, &nNum, &nCount );
			CItemElem* pAddItem = m_Inventory.GetAtId( nId );
			m_Inventory.DoEquip( pAddItem->m_dwObjIndex, itemElem.GetProp()->dwParts );
		}
		m_dwHairMesh = lpCharacter->m_dwHairMesh;
		m_dwHairColor = lpCharacter->m_dwHairColor;
		m_dwHeadMesh = lpCharacter->m_dwHeadMesh;

		AllocShopInventory( lpCharacter );
	}
#ifdef __BEAST
	m_nHitPoint = 0;
#else
	m_nHitPoint = GetMaxHitPoint();
#endif
	LoadDialog();
}

BOOL CMover::LoadDialog()
{
	LPCHARACTER lpCharacter = GetCharacter();
	if( m_pNpcProperty )
	{
		if( lpCharacter )
		{
#ifdef __OUTPUT_INFO_0803
				prj.OutputStore( lpCharacter->m_szKey, this );
#endif	// __OUTPUT_INFO_0803
		}
		return m_pNpcProperty->LoadDialog( lpCharacter );
	}
	else
	{
		return FALSE;
	}
}
#ifdef __V050322_CACHEQUEST
#ifdef __CLIENT
void CMover::ProcessQuestEmoticon()
{
	// 퀘스트 아이콘 확축 프로세스 
	if( m_bQuestEmoticonAdd )
	{
		m_fQuestEmoticonScale += 0.01f;
		if( m_fQuestEmoticonScale >= 1.2f )
			m_bQuestEmoticonAdd = FALSE;
	}
	else
	if( m_bQuestEmoticonAdd == FALSE )
	{
		m_fQuestEmoticonScale -= 0.01f;
		if( m_fQuestEmoticonScale <= 1.0f )
			m_bQuestEmoticonAdd = TRUE;
	}
}
#endif // __CLIENT
#endif // __V050322_CACHEQUEST

#ifdef __V050322_CACHEQUEST 
#ifdef __CLIENT
extern int __IsBeginQuestCondition( CMover* pMover, int nQuestId );
extern int __IsEndQuestCondition( CMover* pMover, int nQuestId );
#endif // __CLIENT
void CMover::ProcessQuest()
{
#ifdef __WORLDSERVER 
	if( IsPlayer() == FALSE ) 
		return;
	BOOL bTimer = FALSE;
	if( m_timerQuestLimitTime.IsTimeOut() )
	{
		m_timerQuestLimitTime.Reset();
		bTimer = TRUE;
	}
	for( int i = 0; i < m_nQuestSize; i++ )
	{
		LPQUEST lpQuest = (LPQUEST) &m_aQuest[ i ];
		if( lpQuest )
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
			if( pQuestProp )
			{
				D3DXVECTOR3 vPos = GetPos();
				// 퀘스트 리전안으로 들어가면 페트롤 임무 성공 플렉 세팅 
				if( pQuestProp->m_dwEndCondPatrolWorld == GetWorld()->m_dwWorldID && pQuestProp->m_rectEndCondPatrol.PtInRect( CPoint( (int) vPos.x, (int) vPos.z ) ) )
				{
					if( lpQuest->m_bPatrol == FALSE )
					{
						lpQuest->m_bPatrol = TRUE;
						// 여기서 클라에 매시지도 하나 보내자. 
						// 탐사를 완료했습니다.
						// 20 마리중 2마리를 퇴치했습니다.
						((CUser*)this)->AddSetQuest( lpQuest ); 
					}
				}
				// 시간 제한 퀘스트 시간 카운트 
				if( pQuestProp->m_nEndCondLimitTime && bTimer )
				{
#ifdef __V050721_BUGFIX_QUESTTIMER
					if( lpQuest->m_wTime && !(lpQuest->m_wTime & 0x8000) ) 
					{
						lpQuest->m_wTime--;
						((CUser*)this)->AddSetQuest( lpQuest ); 
					}
#else
//{{AFX
					if( lpQuest->m_wTime ) 
					{
						lpQuest->m_wTime--;
						((CUser*)this)->AddSetQuest( lpQuest ); 
					}
//}}AFX
#endif // __V050721_BUGFIX_QUESTTIMER
				}
			}
			else
			{
				//WriteError( "ProcessQuest : pQuestProp NULL이다." );
				//CString string;
				//string.Format( "CMover::ProcessQuest : %s의 Quest %d의 pQuestProp NULL이다.", m_szName, lpQuest->m_wId );
				//OutputDebugString( string );
			}
		}
		else
		{
			//WriteError( "ProcessQuest : lpQuest가 NULL이다." );
			//CString string;
			//string.Format( "CMover::ProcessQuest : %s의 lpQuest가 NULL이다.", m_szName );
			//OutputDebugString( string );
		}
	}
#else // __WORLDSERVER
#ifndef __BEAST
	if( IsPlayer()  ) 
		return;
	LPCHARACTER lpCharacter = GetCharacter();
	if( lpCharacter )
	{
		CMover* pMover = GetActiveMover();
		m_nQuestEmoticonIndex = -1;
		for( int i = 0; i < lpCharacter->m_awSrcQuest.GetSize() ; i++ )
		{
			int nQuest = lpCharacter->m_awSrcQuest.GetAt( i );
			int nItem = lpCharacter->m_anSrcQuestItem.GetAt( i );
			LPQUEST lpQuest = pMover->GetQuest( nQuest );
			if( lpQuest == NULL && pMover->IsCompleteQuest( nQuest ) == FALSE )
			{
				// 내가 퀘스트 시작 조건인가?
				if( __IsBeginQuestCondition( pMover, nQuest ) && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 1;
				// 내가 퀘스트 시작 조건이 아닌가?
				else
				if( m_nQuestEmoticonIndex != 1 && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 0;
			}
		}
		for(  i = 0; i < lpCharacter->m_awDstQuest.GetSize(); i++ )
		{
			int nQuest = lpCharacter->m_awDstQuest.GetAt( i );
			int nItem = lpCharacter->m_anDstQuestItem.GetAt( i );
			LPQUEST lpQuest = pMover->GetQuest( nQuest );
			// 퀘스트가 진행 중인 경우 
			if( lpQuest && pMover->IsCompleteQuest( nQuest ) == FALSE && ( lpQuest->m_nState == 0 || lpQuest->m_nState == 14 ) ) 
			{
				// 내가 퀘스트 종료 조건인가?
				if( __IsEndQuestCondition( pMover, nQuest ) && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 3;
				// 내가 퀘스트 종료 조건이 아닌가?
				else
				if( m_nQuestEmoticonIndex != 3 && ( nItem == 0 || pMover->GetItemNum( nItem ) ) )
					m_nQuestEmoticonIndex = 2;
			}

		}
	}
#endif
#endif // __WORLDSERVER
}
#endif // __V050322_CACHEQUEST

void CMover::ProcessScript()
{ 
#ifndef __ONLINE
	if( IsPlayer() ) 
		return;
#ifdef __V050322_CACHEQUEST
	if( m_szCharacterKey[ 0 ] && m_pNpcProperty->IsTimeOut() ) 
//	if( m_szCharacterKey[ 0 ] && m_pNpcProperty->IsTimeOut() && m_nQuestEmoticonIndex == -1 )
	{
		m_pNpcProperty->ResetTimer();
		m_pNpcProperty->RunDialog("#auto",NULL,0,GetId(),GetId(),0);
	}
#else // __V050322_CACHEQUEST
//{{AFX
	if( m_szCharacterKey[ 0 ] && m_pNpcProperty->m_timerScript.IsTimeOut() ) 
	{
		m_pNpcProperty->m_timerScript.Set( MIN( 1 ) + xRandom( SEC( 30 ) ) );
		m_pNpcProperty->RunDialog("#auto",NULL,0,GetId(),GetId());
	}
//}}AFX
#endif // __V050322_CACHEQUEST
#endif	// __ONLINE
}

CModel* CMover::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex )
{
	if( m_dwIndex == MI_FEMALE || m_dwIndex == MI_MALE )
		return prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex, TRUE );
	return prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex );
}
/*
LPCTSTR CMover::GetMoverKey()
{
	_tcslwr( m_szCharacterKey );
	if( m_szCharacterKey[ 0 ] == 0 )
		return m_szName;
	return m_szMoverKey;
}
*/
/*
struct SKILL
{
	DWORD dwSkillIdx;
};
DWORD g_dw

SI_MOOYEGA_BASIC1
SI_MOOYEGA_BASIC2
SI_MOOYEGA_BASIC3
SI_AXE_FALCONCLAW
SI_AXE_LIONCLAW
SI_AXE_WINGCLAW
SI_AXE_CHOPDOWN
SI_AXE_BOOSTER
SI_AXE_BEARCLAW
SI_AXE_DRAGONCLAW
SI_CASE_SKILL1
SI_CASE_SKILL2
SI_CASE_SPLASH
SI_CASE_SKILL3
SI_CASE_BOOSTER
SI_CASE_SKILL4
SI_CASE_SKILL5
-------------------------
SI_MOOYEGA_BASIC1
SI_MOOYEGA_BASIC2
SI_MOOYEGA_BASIC3
SI_AXE_FALCONCLAW
SI_AXE_LIONCLAW
SI_AXE_WINGCLAW
SI_AXE_CHOPDOWN
SI_AXE_BOOSTER
SI_AXE_BEARCLAW
SI_AXE_DRAGONCLAW
SI_CASE_SKILL1
SI_CASE_SKILL2
SI_CASE_SPLASH
SI_CASE_SKILL3
SI_CASE_BOOSTER
SI_CASE_SKILL4
SI_CASE_SKILL5
--------------------------
SI_MOOYEGA_BASIC1
SI_MOOYEGA_BASIC2
SI_MOOYEGA_BASIC3
SI_SWD_SKILL1
SI_SWD_SKILL2
SI_SWD_COUNTER
SI_SWD_SKILL3
SI_SWD_BOOSTER
SI_SWD_SKILL4
SI_SWD_SKILL5
SI_SHD_SHIELDING
SI_SHD_SHOVE
SI_SHD_ARROWSHIELD
SI_SHD_PROTECTIONOTHERNESS
SI_SHD_BULLETSHIELD
SI_SHD_PLASYARM
SI_SHD_DOUBLESHD

*/


// 이 함수를 실행하기 전에 인덱스와 타입이 먼저 세팅되어 있어야한다.
void CMover::InitProp()
{
	MoverProp* pProp = GetProp();
	if( pProp )
	{
		if( _tcslen( m_szName ) == 0 )
			_tcscpy( m_szName, pProp->szName );
#ifndef __ONLINE
		SetAIInterface( pProp->dwAI );
#endif	// __ONLINE

		m_dwBelligerence = pProp->dwBelligerence;

		m_Inventory.SetItemContainer( ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS );
		m_nSlot = 0;
		for( int k = 0 ; k < 3 ; ++k )
		{
			m_Bank[k].SetItemContainer( ITYPE_ITEM, MAX_BANK ) ;
			m_dwGoldBank[k] = 0;
			m_idPlayerBank[k] = 0;
		}
		m_bBank = FALSE;
		
		ZeroMemory( m_aJobLv, sizeof( m_aJobLv ) );
		ZeroMemory( m_aJobSkill, sizeof( m_aJobSkill ) );
#ifndef __BEAST
		ZeroMemory( m_tmReUseDelay, sizeof( m_tmReUseDelay ) );
		m_tmTimer = timeGetTime();
#endif

		ZeroMemory( m_ShopInventory, sizeof( m_ShopInventory ) );	// raiders_test
		if( IsNPC() )
		{
			m_nStr = GetProp()->dwStr; 
			m_nSta = GetProp()->dwSta;
			m_nDex = GetProp()->dwDex;
			m_nInt = GetProp()->dwInt;
		}
		LPSKILL lpSkill;
		// 잡스킬 초기화 
		for( int i = 0; i < MAX_SKILL_JOB; i++ )
		{
			m_aJobSkill[ i ].dwSkill = NULL_ID;
			m_aJobSkill[ i ].dwLevel = 1;
		}

		if( m_nJob != -1 ) 
		{
			ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
			int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
			for( i = 0; i < nJobNum; i++ )
			{
				ItemProp* pSkillProp = apSkillProp[ i ];
				lpSkill = &m_aJobSkill[ i ];
				lpSkill->dwSkill = pSkillProp->dwID;
			}
		}

		// 기본 능력치 초기화 
		m_nLevel        = pProp->dwLevel;
		if( m_nLevel < 1 ) 
			m_nLevel = 1;

		if( m_bPlayer )
		{
			if( m_nJob != -1 )
			{
				m_nHitPoint     = GetMaxHitPoint();
				m_nManaPoint    = GetMaxManaPoint();
				m_nFatiguePoint = GetMaxFatiguePoint();
			}
			else
			{
				m_nHitPoint     = 1;
				m_nManaPoint    = 1;
				m_nFatiguePoint = 1;
			}
			m_pActMover->m_fSpeed	= pProp->fSpeed;
		}
		else
		{
			m_nHitPoint     = GetMaxHitPoint();
#ifdef __XAI1013
			m_nManaPoint    = pProp->dwAddMp;
#else
			m_nManaPoint    = GetMaxManaPoint();
#endif
			m_nFatiguePoint = GetMaxFatiguePoint();
		}

		UpdateParam();
		m_dwVirtItem = pProp->dwAtk1;	// dwVirtItem은 앞으로 dwAtk1으로 쓴다.
		if( m_bPlayer == FALSE && pProp->dwFlying == 1 )	// 비행형 몬스터인가?
			m_dwTypeFlag |= OBJTYPE_FLYING;
 #ifdef __WORLDSERVER
		m_nResource = pProp->dwMaterialAmount;		// 자원량.
 #endif

#ifdef __V050322_CACHEQUEST
		if( m_bPlayer == TRUE )
		{
			m_aQuest = new QUEST[ MAX_QUEST ]; 
			m_aCompleteQuest = new WORD[ MAX_COMPLETE_QUEST ]; 
		}
#endif
	}      
	else
	{  
		TRACE( _T( "CMover::InitProp - MoverProp가 없음. \n" ) );
		ASSERT( 0 );
	}
}

void CMover::InitLevel( int nJob, LONG nLevel, BOOL bGamma )
{
#ifdef __WORLDSERVER
	Lock( theLineFile );
	// 운영자 명령으로 레벨업 하는곳임
	MoverProp* pProp = GetProp();
	if( pProp )
	{
		// 방랑자 스킬 값 가지고 있기.
		int nJobSkillBuf[MAX_JOB_SKILL];
		int nJobSkillLevelBuf[MAX_JOB_SKILL];
		for( int i = 0 ; i < MAX_JOB_SKILL ; ++i )
		{
			nJobSkillBuf[ i ] = m_aJobSkill[ i ].dwSkill;
			nJobSkillLevelBuf[ i ] = m_aJobSkill[ i ].dwLevel;
		}
		ZeroMemory( m_aJobLv, sizeof( m_aJobLv ) );
		ZeroMemory( m_aJobSkill, sizeof( m_aJobSkill ) );

		LPSKILL lpSkill;
		// 잡스킬 초기화 
		for( i = 0; i < MAX_SKILL_JOB; i++ )
		{
			m_aJobSkill[ i ].dwSkill = NULL_ID;
			m_aJobSkill[ i ].dwLevel = 1;
		}
	
		// 1. 방랑자 스킬은 그냥 지급
		m_nJob	= JOB_VAGRANT;
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		// 2. 기본 능력치 초기화 
		m_nLevel	= 1;
		m_nStr	= m_nSta	= m_nDex	= m_nInt	= 15;
		m_nRemainGP = 0;
		m_nRemainLP = 0;

//		m_aJobLv[ nJob ] = nLevel;

/*		if( nLevel > MAX_EXPJOBLEVEL )
			m_aJobLv[ nJob ] = prj.m_aExpJobLevel[ MAX_EXPJOBLEVEL ];
		else
			m_aJobLv[ nJob ] = prj.m_aExpJobLevel[ nLevel ];

		( (CUser*)this )->AddSetChangeJob( nJob );
		g_UserMng.AddNearSetChangeJob( this, nJob, &((CUser*)this)->m_aJobSkill[MAX_JOB_SKILL] );
		g_DPCoreClient.SendPartyMemberJob( (CUser*)this );
		g_DPCoreClient.SendFriendChangeJob( (CUser*)this );
*/
		// 3. LP, GP, 직업

		for( i = 1 ; i < nLevel ; i++ )
		{
			m_nLevel	= i + 1;
			m_nRemainGP += prj.m_aExpCharacter[ m_nLevel ].dwLPPoint;
			m_nRemainLP += 0;

			if( ( i + 1 ) == MAX_JOB_LEVEL )
			{
				if( nJob < MAX_EXPERT )
				{
					AddChangeJob( nJob );
				}
				else
				{
					if( nJob % 2 != 0 )
					{
						AddChangeJob( ( nJob - 5 ) / 2 );
					}
					else
					{
						AddChangeJob( ( nJob - 4 ) / 2 );
					}
				}
			}
			else
			if( ( i + 1 ) == MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			{
				AddChangeJob( nJob );
			}
		}

		if( IsExpert() )
		{
			m_aJobLv[0] = 15;
			if( nLevel > MAX_EXPJOBLEVEL )
				m_aJobLv[ nJob ] = prj.m_aExpJobLevel[ MAX_EXPJOBLEVEL ];
			else
				m_aJobLv[ nJob ] = prj.m_aExpJobLevel[ nLevel ];
		}
		else
		{
			m_aJobLv[ nJob ] = nLevel;
		}

		if( bGamma )
		{
			m_nExp1 = 0;
			m_nExp2 = 0;
		}
		
		if( m_nLevel >= 20 )
			m_nFlightLv = 1;
		else
			m_nFlightLv = 0;
		
		( (CUser*)this )->AddSetChangeJob( nJob );
		g_UserMng.AddNearSetChangeJob( this, nJob, &((CUser*)this)->m_aJobSkill[MAX_JOB_SKILL] );
		g_DPCoreClient.SendPartyMemberJob( (CUser*)this );
		g_DPCoreClient.SendFriendChangeJob( (CUser*)this );
		if( m_idGuild != 0 )
			g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)this );

		SetHitPoint( GetMaxHitPoint() );
		SetManaPoint( GetMaxManaPoint() );
		SetFatiguePoint( GetMaxFatiguePoint() );
	
		if( bGamma == FALSE )
		{
			for( int i = 0 ; i < MAX_JOB_SKILL ; ++i )
			{
				( (CUser*)this )->AddSetSkill( nJobSkillBuf[ i ], nJobSkillLevelBuf[ i ] );
			}
		}
			
		g_UserMng.AddSetLevel( this, (WORD)m_nLevel );
//		g_DPCoreClient.SendSetGrowthLearningPoint( this, m_nRemainGP, m_nRemainLP );
		( (CUser*)this )->AddSetGrowthLearningPoint( m_nRemainGP, m_nRemainLP );

		( (CUser*)this )->AddSetExperience( GetExp1(), GetExp2(), (WORD)m_nLevel );
//		g_DPCoreClient.SendSetExperience( this, (DWORD)m_nExp1, (DWORD)m_nExp2, (WORD)m_nLevel );

#ifdef __S050524_SM_ITEM
		( (CUser*)this )->m_playTaskBar.InitTaskBarShorcutKind( SHORTCUT_SKILL );
#else // __S050524_SM_ITEM
		( (CUser*)this )->m_playTaskBar.InitTaskBar();
#endif // __S050524_SM_ITEM
		( (CUser*)this )->AddTaskBar();
		((CUser*)this)->AddSetState( m_nStr, m_nSta, m_nDex, m_nInt, m_nRemainLP, m_nRemainGP );
	}
	Unlock( theLineFile );
#endif // __WORLDSERVER
}


BOOL CMover::InitSkillExp()
{
	DWORD	dwSkillPointBuf = 0;
	for( int i = MAX_JOB_SKILL ; i < MAX_SKILL_JOB ; ++i )
	{
		LPSKILL pSkill = &m_aJobSkill[ i ];
		if( pSkill != NULL )
		{
			if( pSkill->dwSkill != NULL_ID )
			{
				ItemProp* pSkillProp = prj.GetSkillProp( pSkill->dwSkill );
				if( pSkillProp == NULL )
				{
					return FALSE;	// property not found
				}

				for( int j = 1 ; j < pSkill->dwLevel ; ++j )
				{
					AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, j );
					if( pAddSkillProp == NULL )
					{
						return FALSE;	// property not found
					}
					if( prj.m_aExpSkill[ j + 1 ] != 0 )
						dwSkillPointBuf += prj.m_aExpSkill[ j + 1 ] * pAddSkillProp->dwSkillExp;
					else
						dwSkillPointBuf += pAddSkillProp->dwSkillExp;
				}

				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );
				if( pAddSkillProp == NULL )
				{
					return FALSE;	// property not found
				}
				dwSkillPointBuf += pSkill->dwExp * pAddSkillProp->dwSkillExp;
			}
		}
	}
	for( i = MAX_JOB_SKILL ; i < MAX_SKILL_JOB ; ++i )
	{
		LPSKILL pSkill = &m_aJobSkill[ i ];
		if( pSkill != NULL )
		{
			if( pSkill->dwSkill != NULL_ID )
			{
				pSkill->dwLevel = 1;
				pSkill->dwExp = 0;
			}
		}
	}
	m_dwSkillPoint += dwSkillPointBuf;

#ifdef __WORLDSERVER
	( (CUser*)this )->m_playTaskBar.InitTaskBarShorcutKind( SHORTCUT_SKILL );
	( (CUser*)this )->AddTaskBar();
#endif //__WORLDSERVER

	return TRUE;
}


void CMover::ReState()
{
#ifdef __WORLDSERVER
	Lock( theLineFile );
	m_nStr	= m_nSta	= m_nDex	= m_nInt	= 15;
	m_nRemainLP = 0;
	m_nRemainGP = 0;

	for( int i = 1 ; i < m_nLevel ; i++ )
	{
		m_nRemainGP += prj.m_aExpCharacter[ i + 1 ].dwLPPoint;
	}
	
	SetHitPoint( GetMaxHitPoint() );
	SetManaPoint( GetMaxManaPoint() );
	SetFatiguePoint( GetMaxFatiguePoint() );
	((CUser*)this)->AddSetState( m_nStr, m_nSta, m_nDex, m_nInt, m_nRemainLP, m_nRemainGP );

	Unlock( theLineFile );
#endif // __WORLDSERVER
}


#ifdef __WORLDSERVER

#ifdef __N_TRIGGER_AREASET_050203
BOOL CMover::ReplaceInspection( REGIONELEM* pPortkey )
{
	BOOL bResult = TRUE;
	if( bResult != FALSE && pPortkey->m_uItemId != 0xffffffff )
	{
		int nCount = GetItemNum( pPortkey->m_uItemId );
		if( nCount <= pPortkey->m_uiItemCount )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Item %d Count %d/%d Inspection %d \n", pPortkey->m_uItemId, nCount, pPortkey->m_uiItemCount, bResult );
	}
	if( bResult != FALSE && pPortkey->m_uiMinLevel != 0xffffffff )
	{
		if( pPortkey->m_uiMinLevel <= m_nLevel && m_nLevel <= pPortkey->m_uiMaxLevel)		
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Level (%d~%d) %d Inspection %d \n", pPortkey->m_uiMinLevel, pPortkey->m_uiMaxLevel, m_nLevel, bResult );
	}
	if( bResult != FALSE && pPortkey->m_iQuest != 0xffffffff )
	{
		LPQUEST pCurQuest = GetQuest( pPortkey->m_iQuest );
		if( pCurQuest && ( pPortkey->m_iQuestFlag == 0 || pPortkey->m_iQuestFlag == pCurQuest->m_nState ) )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Quest %d Flag %d/%d Inspection %d \n", pPortkey->m_iQuest, pPortkey->m_iQuestFlag, pCurQuest->m_nState, bResult );
	}
	if( bResult != FALSE && pPortkey->m_iJob != 0xffffffff )
	{
		if( pPortkey->m_iJob == m_nJob )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Job %d/%d Inspection %d \n", pPortkey->m_iJob, m_nJob, bResult );
	}
	if( bResult != FALSE && pPortkey->m_iGender != 0xffffffff )
	{
		if( pPortkey->m_iGender == m_bySex )
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE( "ReplaceInspection Gender %d/%d Inspection %d \n", pPortkey->m_iGender, m_bySex, bResult );
	}
	if( bResult != FALSE && pPortkey->m_bCheckParty != FALSE )
	{
		g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
		CParty* pParty	= g_PartyMng.GetParty( m_idparty );
		if( pParty != NULL && pParty->IsMember( m_idPlayer ) )
			bResult = TRUE;
		else 
			bResult = FALSE;
		g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1		
		TRACE( "ReplaceInspection Party Inspection %d \n", bResult );
	}
	if( bResult != FALSE && pPortkey->m_bCheckGuild != FALSE )
	{
		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
		CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
		if( pGuild && pGuild->IsMember( m_idPlayer ) )
			bResult = TRUE;
		else
			bResult = FALSE;
		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1
		TRACE( "ReplaceInspection Guild Inspection %d \n", bResult );
	}
	if( IsFly() )
	{
#ifdef _DEBUG
		( (CUser*)this )->AddText( "디버그만 나옴 일반유저는 안남옴 : 비행중에는 들어갈수 없어여~~ 진자루.. ^___^" );
#endif // _DEBUG
		bResult = FALSE;
	}
	return bResult;
}
#endif // __N_TRIGGER_AREASET_050203


#ifdef __MSVR
BOOL CMover::Replace( u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3 & vPos, DWORD dpid )
#else	// __MSVR
BOOL CMover::Replace( DWORD dwWorldID, D3DXVECTOR3 & vPos, DWORD dpid )
#endif	// __MSVR
{
	CWorld* pWorld	= GetWorld();
	if( !pWorld )
		return FALSE;
	if( dwWorldID == 0 || dwWorldID == NULL_ID )
		return FALSE;

	// 이동을 멈춘다.
	ClearDest();
	SendActMsg( OBJMSG_ACC_STOP );
	SendActMsg( OBJMSG_STOP_TURN );
	SendActMsg( OBJMSG_STAND );

	LPREPLACEOBJ lpReplaceObj	= NULL;

	pWorld->m_csReplaceObj.Enter( theLineFile );	// lock1
	pWorld->m_csModifyLink.Enter( theLineFile );	// lock2

	for( int i = 0; i < pWorld->m_cbModifyLink; i++ )
	{
		if( pWorld->m_apModifyLink[i] == this )
		{
			pWorld->m_apModifyLink[i]	= NULL;
			m_vRemoval	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			break;
		}
	}

	for( i = 0; i < pWorld->m_cbReplaceObj; i++ )
	{
		if( pWorld->m_aReplaceObj[i].pObj == this )
		{
			lpReplaceObj = &pWorld->m_aReplaceObj[i];
			break;
		}
	}

	if( lpReplaceObj == NULL )
	{
		ASSERT( pWorld->m_cbReplaceObj < MAX_REPLACEOBJ );
		lpReplaceObj = &pWorld->m_aReplaceObj[pWorld->m_cbReplaceObj++];
		lpReplaceObj->pObj	= this;
	}

	lpReplaceObj->dwWorldID	 = dwWorldID;
	lpReplaceObj->vPos       = vPos;
	lpReplaceObj->dpid       = dpid;
#ifdef __MSVR
	lpReplaceObj->uIdofMulti = uIdofMulti;
#endif	// __MSVR

	pWorld->m_csModifyLink.Leave( theLineFile );
	pWorld->m_csReplaceObj.Leave( theLineFile );	
	return TRUE;
}

BOOL CMover::CreateItem( CItemBase* pItemBase, BYTE* pnId, short* pnNum, BYTE nCount )
{
	if( pItemBase->m_dwItemId == 0 )
		return FALSE;
	CItemElem* pItemElem = (CItemElem*)pItemBase;
	if( pItemElem->m_nHitPoint == -1 )
	{
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp )
			pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
		else
			pItemElem->m_nHitPoint	= 0;
	}

	if( pItemBase->m_dwSerialNumber == 0 )
		pItemBase->m_dwSerialNumber		= xRand();
	BOOL fSuccess;

	if( nCount == 0 )
	{
		BYTE anId[MAX_INVENTORY];
		short anNum[MAX_INVENTORY];
		
		fSuccess	= m_Inventory.Add( (CItemElem*)pItemBase, anId, anNum, &nCount );

		if( fSuccess && IsPlayer() )
		{
			( (CUser*)this )->AddCreateItem( pItemBase, anId, anNum, nCount );
		}
		if( pnId != NULL )
			memcpy( pnId, anId, sizeof(BYTE) * nCount );
		if( pnNum != NULL )
			memcpy( pnNum, anNum, sizeof(short) * nCount );
	}
	else
	{
		CItemElem* pItemElem	= (CItemElem*)pItemBase;
		CItemElem itemElem	= *pItemElem;
		for( int i = 0; i < nCount; i++ )
		{
			itemElem.m_nItemNum		= pnNum[i];
			m_Inventory.SetAtId( pnId[i], &itemElem );
		}
		if( IsPlayer() )
			( (CUser*)this )->AddCreateItem( pItemBase, pnId, pnNum, nCount );
		fSuccess	= TRUE;
	}
	return fSuccess;
}

void CMover::RemoveItem( BYTE nId, short nNum )
{
	CItemBase* pItemBase	= GetItemId( nId );
	if( pItemBase )
		UpdateItem( nId, UI_NUM, ( (CItemElem*)pItemBase )->m_nItemNum - nNum );
}


// IK3로 삭제하기 예) IK3_CLOAK
void CMover::RemoveItemIK3( DWORD dwItemKind3 )
{
	int	nSize = m_Inventory.GetMax();
	for( int i = 0 ; i < nSize; i++ )
	{
		CItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( IsUsableItem( pItemElem ) && pItemElem->GetProp()->dwItemKind3 == dwItemKind3 )
		{
			if( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild == 0 )
				continue;

			if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		
			{
				if( DoEquip( pItemElem, FALSE ) )
				{
					g_UserMng.AddDoEquip( this, -1, pItemElem, FALSE );
					UpdateItem( i, UI_NUM, 0 );		// remove
				}
				else
				{
					WriteError( "RemoveItemIK3->DoEquip : %s %d, User = %s, Item = %d %d %d", __FILE__, __LINE__, GetName(), pItemElem->m_dwObjId, pItemElem->GetProp()->dwItemKind3, dwItemKind3 );
				}
			}
			else
			{
				UpdateItem( i, UI_NUM, 0 );		// remove
			}
		}
	}
}

void CMover::RemoveVendorItem( CHAR chTab, BYTE nId, short nNum )
{
	short num;
	CItemElem* pItemElem = m_ShopInventory[chTab]->GetAtId( nId );

	if( pItemElem ) 
	{
		if( ( num = pItemElem->m_nItemNum - nNum ) > 0 )
			pItemElem->m_nItemNum	= num;
		else
			m_ShopInventory[chTab]->RemoveAtId( nId );
		g_UserMng.AddUpdateVendor( this, chTab, nId, num );
	}
}
#endif	// __WORLDSERVER

void CMover::UpdateItemBank( int nSlot, BYTE nId, CHAR cParam, DWORD dwValue )
{
	CItemBase* pItemBase = GetItemBankId( nSlot, nId );
	
	if( pItemBase )
	{
		switch( cParam )
		{
		case UI_NUM:
			{
				if( dwValue == 0 ) 
				{
					RemoveItemBankId( nSlot, nId );
				}
				else
				{
					( (CItemElem*)pItemBase )->m_nItemNum	= dwValue;
				}
				break;
			}
		default:
			break;
		}
	}
#ifdef __WORLDSERVER
	if( IsPlayer() )
		( (CUser*)this )->AddUpdateBankItem( nSlot, nId, cParam, dwValue );
#endif
}

#ifdef __WORLDSERVER
// 인벤토리내에서 1개 아이템을 떨어트린다.
int	CMover::DoDropItemRandom( BOOL bExcludeEquip, CMover* pAttacker )
{
	int i, nMax, nRealMax, nVal, nDropCnt;
	CItemElem* pItemElem;
	CItemElem *pElemBuff[ MAX_INVENTORY + MAX_EQUIPMENT ] = { NULL, };

	nRealMax = nDropCnt = 0;

	nMax = m_Inventory.GetMax();
	for( i = 0; i < nMax; ++i ) 		// 인벤 갯수만큼 돌면서 아이템이 있는칸만 포인터를 뽑아놓는다.
	{
		pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem == NULL )
			continue;
			
		if( pItemElem->IsQuest() )					// 퀘스트 아이템 제외 
			continue;

		if( bExcludeEquip && m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		// 장착된 아이템 제외  
			continue;

		if( pItemElem->IsBinds() )
			continue;
				
		if( pItemElem->IsCharged() )	// 유료 상품아이템 제외
			continue;

		pElemBuff[ nRealMax++ ] = pItemElem;
	}

	while( nRealMax )	// 검색대상 아이템이 더이상 없을경우 끝.
	{
		int nIdx = xRandom( nRealMax );
		pItemElem = pElemBuff[ nIdx ];		// 아이템 리스트에서 랜덤으로 하나를 뽑아옴

		if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )	// 장착하고 있으면 벗김 			
		{
			if( DoEquip( pItemElem, FALSE ) )
				g_UserMng.AddDoEquip( this, -1, pItemElem, FALSE );
		}

		CItem* pItem = DropItem( pItemElem->m_dwObjId, 0, GetPos(), TRUE );
		if( pItem )	
		{
#ifdef __NPP_050308
//			pItem->SetOwner( pAttacker->GetId() );
			UNUSED_ALWAYS( pAttacker );
#else
			UNUSED_ALWAYS( pAttacker );
#endif
			return 1;
		}

		// drop에 실패한 아이템이라도 리스트에서 빼주자.
		for( int j = nIdx; j < nRealMax-1; j ++ )		// 앞으로 한칸씩 땡김.
			pElemBuff[j] = pElemBuff[j+1];
		nRealMax --;	// 하나를 떨어트렸으니 버퍼의 크기를 하나 줄임
	}		

	return 0;
}
#endif // __WORLDSERVER

CItem *CMover::_DropItemNPC( DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos )
{
#ifdef __WORLDSERVER
	if( IsInvalidObj(this) )	
		return NULL;

	CWorld *pWorld = GetWorld();
	if( pWorld == NULL )	
		return NULL;	
	CItemBase* pItemBase = GetItemId( dwID );
	if( NULL == pItemBase )		
		return NULL;
	
	short nNum	= 1;
	nNum = ((CItemElem*)pItemBase)->m_nItemNum;
	if( m_Inventory.IsEquip( pItemBase->m_dwObjId ) ) 
		return NULL;

	if( nDropNum == 0 )
		nDropNum = nNum;
	else if( nNum - (int)nDropNum < 0 )
		return NULL;
	
	CItem* pItem	                             = new CItem;
	pItem->m_pItemBase	                         = new CItemElem;
	*((CItemElem*)pItem->m_pItemBase)            = *((CItemElem*)pItemBase);
	((CItemElem*)pItem->m_pItemBase)->m_nItemNum = nDropNum;
		
	if( pItemBase->m_dwItemId == 0 ) 
		Error( "_DropItemNPC SetIndex: %s \n", GetName() ); 

	pItem->SetIndex( D3DDEVICE, pItemBase->m_dwItemId );
	if( pItem->m_pModel )
	{
		D3DXVECTOR3 v = vPos;
		v.y += ( pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y ) / 2.0f;
		pItem->SetPos( v );
		pItem->SetAngle( xRandom( 360 ) );
		pItem->m_dwDropTime = timeGetTime();	// 떨어트렸을때의 시간.
		pWorld->AddObj( (CObj*)pItem, TRUE );
		RemoveItem( dwID, nDropNum );
		return pItem;
	}
	else
	{
		SAFE_DELETE( pItem );
	}
#endif // WORLDSERVER
	return NULL;
}

// 아이템을 떨굴 수 있는가? PK - PK를 당한 경우는 TRUE
BOOL CMover::IsDropableState( BOOL bPK )
{
	if( IsInvalidObj(this) )	
		return FALSE;

#ifdef __WORLDSERVER
	CWorld *pWorld = GetWorld();
	if( pWorld == NULL )	
		return FALSE;

	if( m_pActMover->m_bGround == 0 || m_pActMover->IsFly() )	// 날고있는 중이거나 공중에 떠있는상태에선 일단 못떨어트리게 하자.
	{
	#ifdef __NPP_050308
		if( bPK == FALSE ) //  PK당한 경우가 아닐경우만 떨군다.
		{
			((CUser*)this)->AddDefinedText( TID_GAME_FLIGHTDROP, "" );
			return FALSE;
		}
	#else
		UNUSED_ALWAYS( bPK );
		((CUser*)this)->AddDefinedText( TID_GAME_FLIGHTDROP, "" );
		return FALSE;
	#endif // !__NPP_050308
	}

	if( m_vtInfo.GetOtherID() != NULL_ID )		// 트레이드중일땐 일단 못떨어트리게 하자
	{
		((CUser*)this)->AddDefinedText( TID_GAME_TRADELIMITITEM, "" );
		return FALSE;
	}

	if( m_vtInfo.VendorIsVendor() )
		return FALSE;

#ifdef __SCHOOL0701
	if( IsMode( ITEM_MODE ) )
		return FALSE;
#endif 

#endif // __WORLDSERVER
	return TRUE;
}	


// 인벤의 아이템을 바닥에 내려놓을때 사용한다.
// dwType, dwID의 아이템을 vPos에 떨어트린다.
// bPK == YES면 플레이어가 PK당해서 떨어트린 아이템이다.
CItem* CMover::DropItem( DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos, BOOL bPK )
{
#ifdef __WORLDSERVER
	if( IsPlayer() == FALSE )	
		return _DropItemNPC( 0, dwID, nDropNum, vPos );	// 몹이 떨어뜨릴땐 이걸사용.
	
	if( IsDropableState( bPK ) == FALSE )
		return NULL;

	CItemBase* pItemBase = GetItemId( dwID );
	if( IsUsableItem( pItemBase ) == FALSE )
		return NULL;
	
	if( ( (CItemElem*)pItemBase )->IsBinds() )
		return NULL;
	
	if( pItemBase->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemBase->m_dwItemId == II_RID_RID_BOR_LADOLF )
		return NULL;
	
	if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild )
		return NULL;

	if( ((CItemElem*)pItemBase)->IsCharged() )
		return NULL;

	if( m_Inventory.IsEquip( pItemBase->m_dwObjId ) ) 
		return NULL;
	
	short nNum	= ((CItemElem*)pItemBase)->m_nItemNum;

	if( nDropNum == 0 )
		nDropNum = nNum;
	else if( nDropNum > nNum )
		return NULL;
	
	CItem* pItem	= new CItem;
	pItem->m_pItemBase	= new CItemElem;
	*( (CItemElem*)pItem->m_pItemBase )		= *( (CItemElem*)pItemBase );
	( (CItemElem*)pItem->m_pItemBase )->m_nItemNum		= nDropNum;
		
	if( pItemBase->IsQuest() )
		pItem->m_idHolder	= m_idPlayer;
	
	if( pItemBase->GetProp()->nLog != -1 )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "D";
		aLogItem.SendName = GetName();
		aLogItem.RecvName = "GROUND";
		aLogItem.WorldId = GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = GetGold();
		g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), nDropNum );
	}
	
	pItem->SetIndex( D3DDEVICE, pItemBase->m_dwItemId );

	if( pItem->m_pModel )
	{
		D3DXVECTOR3 v = vPos;
		v.y += ( pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y ) / 2.0f;
		pItem->SetPos( v );
		pItem->SetAngle( xRandom( 360 ) );
		pItem->m_dwDropTime = timeGetTime();	// 떨어트렸을때의 시간.
#if !defined(__NPP_050308)
		if( bPK )
			pItem->m_idFrom = GetId();		// 누구로부터 떨어졌는지 아이디 기록 
#else
		UNUSED_ALWAYS( bPK );
#endif
		GetWorld()->AddObj( (CObj*)pItem, TRUE );

		RemoveItem( dwID, nDropNum );
		return pItem;
	}
	else
	{
		SAFE_DELETE( pItem );
	}

#endif // WORLDSERVER
	return NULL;
}

// TODO_OPTIMIZE: 좌표가 변경될 때 호출되게한다. ( rect를 트리구조로 갖고 찾게 하는 것도 좋겠다.)
REGIONELEM* CMover::UpdateRegionAttr()
{
	REGIONELEM* pPortkey = NULL;

#ifndef __BEAST
	D3DXVECTOR3 vPos = GetPos();
	POINT pt = { vPos.x, vPos.z };
	LPREGIONELEM lpRegionElem;
	int nSize = GetWorld()->m_aRegion.GetSize();

	BOOL bRegion  = FALSE;
	for( int i = 0; i < nSize; i++ )
	{
		lpRegionElem = GetWorld()->m_aRegion.GetAt( i );
		if( lpRegionElem->m_rect.PtInRect( pt ) )
		{
#ifdef __NPP_050308
			m_dwOldRegionAttr = m_dwRegionAttr;
			m_dwRegionAttr = 0;
#endif // __NPP_050308
			m_dwRegionAttr |= lpRegionElem->m_dwAttribute;
			if( lpRegionElem->m_dwIdTeleWorld != WI_WORLD_NONE )
				pPortkey = lpRegionElem;

#ifdef __NPP_050308
#ifdef __JAPAN_PVP
			if( ( m_dwRegionAttr & RA_SAFETY ) == RA_SAFETY || ( m_dwRegionAttr & RA_PENALTY_PK ) == RA_PENALTY_PK || ( m_dwRegionAttr & RA_PK ) == RA_PK || ( m_dwRegionAttr & RA_FIGHT ) == RA_FIGHT )
#else // __JAPAN_PVP
			if( ( m_dwRegionAttr & RA_SAFETY ) == RA_SAFETY || ( m_dwRegionAttr & RA_PENALTY_PK ) == RA_PENALTY_PK || ( m_dwRegionAttr & RA_PK ) == RA_PK )
#endif // __JAPAN_PVP
			{	// 기본적으로 세이프티 지역, 패널티, 프리 지역중 한곳에는 있어야 한다
				bRegion = TRUE;
			}
#endif // __NPP_050308
		}
	}
#ifdef __NPP_050308
	if( bRegion == FALSE )
	{
		m_dwOldRegionAttr = m_dwRegionAttr;
		m_dwRegionAttr = 0;
		m_dwRegionAttr |= GetWorld()->m_nPKMode;
	}
#endif // __NPP_050308
#endif

	return pPortkey;
}

#ifdef __NPP_050308
//다른 지역으로 이동했는지 검사 ( 평화 지역, 페널티 피케이 지역, 프리 피케이 지역 )
BOOL CMover::IsRegionMove( DWORD dwOlgRegionAttr, DWORD dwRegionAttr )
{
	if( ( dwOlgRegionAttr & RA_SAFETY ) == RA_SAFETY ) 
	{
		if( ( dwRegionAttr & RA_SAFETY ) != RA_SAFETY )
		{
			return TRUE;
		}
	}
	else if( ( dwOlgRegionAttr & RA_PENALTY_PK ) == RA_PENALTY_PK ) 
	{
		if( ( dwRegionAttr & RA_PENALTY_PK ) != RA_PENALTY_PK )
		{
			return TRUE;
		}
	}
	else if( ( dwOlgRegionAttr & RA_PK ) == RA_PK ) 
	{
		if( ( dwRegionAttr & RA_PK ) != RA_PK )
		{
			return TRUE;
		}
	}
#ifdef __JAPAN_PVP
	else if( ( dwOlgRegionAttr & RA_FIGHT ) == RA_FIGHT )
	{
		if( ( dwRegionAttr & RA_FIGHT ) != RA_FIGHT )
		{
			return TRUE;
		}
	}
#endif // __JAPAN_PVP
	
	return FALSE;
}

DWORD	CMover::GetPKPVPRegionAttr( void )
{
#ifdef __JAPAN_PVP
	if( IsRegionAttr( RA_FIGHT ) )
		return RA_FIGHT;
	else
#endif // __JAPAN_PVP
	if( IsRegionAttr( RA_PENALTY_PK ) )
		return RA_PENALTY_PK;
	else if( IsRegionAttr( RA_PK ) )
		return RA_PK;
	else
		return RA_SAFETY;
}
#endif // __NPP_050308

#ifdef __S0504_FLY
void CMover::SetStateMode( DWORD dwMode, BYTE nFlag )
{
	m_dwStateMode |= dwMode;
#ifdef __WORLDSERVER
	g_UserMng.AddStateMode( ((CUser*)this), nFlag );
#endif // __WORLDSERVER
}

void CMover::SetStateNotMode( DWORD dwMode, BYTE nFlag )
{
	m_dwStateMode &= (~dwMode);
#ifdef __WORLDSERVER
	g_UserMng.AddStateMode( ((CUser*)this), nFlag );
#endif // __WORLDSERVER
} 

BOOL CMover::IsUseItemReadyTime( ItemProp* pItemProp, OBJID dwObjItemId )
{
	if( m_Inventory.IsEquip( dwObjItemId ) )
	{		
		return TRUE;
	}

	if( pItemProp->dwParts == PARTS_RIDE )
	{
		
		int nLimitLv = pItemProp->dwFlightLimit;
		if( nLimitLv == NULL_ID )
			nLimitLv = 1;
		
		CWorld* pWorld = GetWorld();
		if( pWorld == NULL )
			return FALSE;
		int nAttr	= pWorld->GetHeightAttribute( GetPos().x, GetPos().z );
		
		if( GetFlightLevel() < nLimitLv ) // 비행레벨이 안되면 못탄다.
		{
			PrintString( this, TID_GAME_USEAIRCRAFT );	// 서있어야 사용할수 있습니다
			return FALSE;
		}
		else if( !pWorld->m_bFly || nAttr == HATTR_NOFLY ) // 비행금지구역입니다.
		{
			PrintString( this, TID_ERROR_NOFLY );	// 서있어야 사용할수 있습니다
			return FALSE;
		}
	}
	
	if( pItemProp->dwSkillReadyType != 0 && 
		pItemProp->dwSkillReadyType != 0xffffffff )
	{
		if( m_vtInfo.IsVendorOpen() )	// 개인상점 중에는 못탄다
		{
			return FALSE;
		}
		else if( IsStateMode( STATE_BASEMOTION_MODE ) )	// 이미 시전중이면 메세지 처리
		{
			PrintString( this, TID_PK_BLINK_LIMIT );	// 시전중에는 사용할수 없습니다
			return FALSE;		
		}
		else if( m_pActMover->IsState( OBJSTA_STAND ) == FALSE )	// 서있는 상태가 아니면 메세지 처리
		{
			PrintString( this, TID_PK_STAND_LIMIT );	// 서있어야 사용할수 있습니다
			return FALSE;		
		}
		else if( IsFly() )
		{
#ifdef __CLIENT
			g_WndMng.PutString( prj.GetText( TID_PK_FLIGHT_NOUSE ), NULL, prj.GetTextColor( TID_PK_FLIGHT_NOUSE ) );
#endif // __CLINET
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __S0504_FLY

void CMover::ProcessRegion()
{
#ifndef __BEAST
	if( FALSE == IsPlayer() )
		return;

#ifdef __OPT_REGIONATTR041230
	REGIONELEM* pPortkey = NULL;
	if( IsPosChanged() )
	{
		pPortkey = UpdateRegionAttr();
		SetPosChanged( FALSE );
	}
#else	
	REGIONELEM* pPortkey = UpdateRegionAttr();
#endif

#ifdef __NPP_050308
#ifdef __WORLDSERVER
	if( IsRegionMove( m_dwOldRegionAttr, m_dwRegionAttr ) )
	{
		if( m_nDuel == 1 )
		{
			CMover *pDuelOther = prj.GetMover( m_idDuelOther );
			if( pDuelOther )
			{
				pDuelOther->DuelResult( this );
				pDuelOther->ClearDuel();
				( (CUser*)pDuelOther )->AddSetDuel( pDuelOther );
				ClearDuel();
				( (CUser*)this )->AddSetDuel( this );
			}
		}
		else if( m_nDuel == 2 )
		{
			g_PartyMng.Lock( theLineFile );
			CParty* pParty	= g_PartyMng.GetParty( m_idparty );
			if( pParty && pParty->IsLeader( m_idPlayer ) )
			{
				CParty* pParty2		= g_PartyMng.GetParty( m_idDuelParty );
				if( pParty2 )
				{
					CUser* pWinner	= (CUser*)pParty2->GetLeader();
					if( pWinner )
						pWinner->DuelResult( this );
				}
			}
			g_PartyMng.Unlock( theLineFile );
		}
	}
#endif // __WORLDSERVER
#else // __NPP_050308
	#ifdef __WORLDSERVER
		if( m_nDuel == 1 )	// 개인듀얼중일때
		{
			if( IsRegionAttr(RA_FIGHT) == FALSE )	// 듀얼존을 벗어나면 취소됨.
			{
				CMover *pDuelOther = prj.GetMover( m_idDuelOther );
				if( pDuelOther )
				{
					( (CUser*)pDuelOther )->AddDuelCancel( pDuelOther->m_idDuelOther );
					pDuelOther->ClearDuel();
				}
				( (CUser*)this )->AddDuelCancel( m_idDuelOther );
				ClearDuel();
			}
		}
	#endif // __WORLDSERVER
#endif // __NPP_050308
	#ifdef __WORLDSERVER
		if( pPortkey )
		{
#ifdef __N_TRIGGER_AREASET_050203
			if( ReplaceInspection( pPortkey ) == FALSE )
			{
				return;
			}
#ifdef __MSVR
			Replace( g_uIdofMulti, pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld );
#else	// __MSVR
			Replace( pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld );
#endif	// __MSVR
#else // __N_TRIGGER_AREASET_050203
		#ifdef __MSVR
			Replace( g_uIdofMulti, pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld );
		#else	// __MSVR
			Replace( pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld );
		#endif	// __MSVR
#endif // __N_TRIGGER_AREASET_050203
		}
	#endif	// __WORLDSERVER
#endif // __BEAST
}
void CMover::Execute()
{
	CObj::Execute();
}

BOOL CMover::Read( CFileIO* pFile )
{
	CObj::Read( pFile );

	char szDialogFile[MAX_DIALOGFILENAME];

	pFile->Read( m_szName, sizeof( m_szName ) );
	pFile->Read( szDialogFile, sizeof( szDialogFile ) );
	pFile->Read( m_szCharacterKey, sizeof( m_szCharacterKey ) );
	pFile->Read( &m_dwBelligerence, sizeof( m_dwBelligerence ) );
#ifdef __BEAST
	pFile->Read( &m_dwExtraFlag, sizeof( m_dwExtraFlag ) );
#else
	DWORD dwExtraFlag = 0;
	pFile->Read( &dwExtraFlag, sizeof( dwExtraFlag ) );
#endif

	UpdateParam();

#if !defined(__ONLINE)	
	InitCharacter( GetCharacter() );	// 싱글일 때는 리소스 스크립트를 읽어들인다.
	if( m_pNpcProperty )
		m_pNpcProperty->SetDlgName( szDialogFile );
#endif
	
	return TRUE;
}

BOOL CMover::Write( CFileIO* pFile )
{
	char szDialogFile[MAX_DIALOGFILENAME] = {0, };
	if( m_pNpcProperty )
		strcpy( szDialogFile, m_pNpcProperty->GetDlgName() );

	CObj::Write( pFile );
	pFile->Write( m_szName, sizeof( m_szName ) );
	pFile->Write( szDialogFile, sizeof( szDialogFile ) );
	pFile->Write( m_szCharacterKey, sizeof( m_szCharacterKey ) );
	pFile->Write( &m_dwBelligerence, sizeof( m_dwBelligerence ) );
#ifdef __BEAST
	pFile->Write( &m_dwExtraFlag, sizeof( m_dwExtraFlag ) );
#else
	DWORD dwExtraFlag = 0;
	pFile->Write( &dwExtraFlag, sizeof( dwExtraFlag ) );
#endif

	return TRUE;
}

void CMover::MessageFromModel( DWORD dwMessage )
{
}

BOOL CMover::IsAction()
{
	if( m_dwMotion == 10 || m_dwMotion == 5  || m_dwMotion == 6  || m_dwMotion == 7  || m_dwMotion == 8 )
		return TRUE;
	return FALSE;
}

CItemElem* CMover::GetLWeaponItem()
{
//	if( IsPlayer() )
//		return (CItemElem*)m_Inventory.GetEquip( PARTS_LWEAPON );
//	return NULL;
	return GetWeaponItem( PARTS_LWEAPON );
}

CItemElem* CMover::GetWeaponItem( int nParts )
{
	if( IsPlayer() )
		return (CItemElem*)m_Inventory.GetEquip( nParts );
	return NULL;
}

ItemProp* CMover::GetActiveHandItemProp( int nParts )
{
	if( IsPlayer() )
	{
#ifdef __ONLINE
		if( IsActiveMover() )
		{
			CItemElem* pItemElem = (CItemElem*)m_Inventory.GetEquip( nParts );
			if( pItemElem )
				return pItemElem->GetProp();
			return prj.GetItemProp( II_WEA_HAN_HAND );
		}
		else	// 클라이언트에서는 타 캐릭터는 m_adwEquipment에 dwObjId가 아닌 dwItemId를 가지고 있다.
		{
		#ifdef __FINITEITEM0705
			DWORD dwWeaponId	= m_aEquipInfo[nParts].dwId;
		#else	// __FINITEITEM0705
			DWORD dwWeaponId	= m_adwEquipment[ nParts ];
		#endif	// __FINITEITEM0705
			if( dwWeaponId != NULL_ID )
				return prj.GetItemProp( dwWeaponId );
			return prj.GetItemProp( II_WEA_HAN_HAND );
		}
#else	// __ONLINE
		CItemElem* pItemElem = (CItemElem*)m_Inventory.GetEquip( nParts ); 
		if( pItemElem )
			return pItemElem->GetProp();
		return prj.GetItemProp( II_WEA_HAN_HAND );
#endif	// __ONLINE
	}

	// virt는 NPC들이 사용하는 가상 아이템 또는 플레이어가 무기를 장착하지 않았을 때 사용할 아이템이다. 
	// 이것을 설정하여 몹의 사용할 아이템을 결정한다.
	// 그 아이템은 주로 무기나, 스킬을 의미할 것이며, 주로 공격용으로 사용될 것이다.
	if( m_dwVirtItem != NULL_ID )
	{
		if( m_dwVirtType == VT_ITEM )
			return prj.GetItemProp( m_dwVirtItem );
		if( m_dwVirtType == VT_SKILL )
			return prj.GetSkillProp( m_dwVirtItem );
	}
	return NULL;
}
CItemElem* CMover::GetEquipItem( int nParts )
{
	return (CItemElem*)m_Inventory.GetEquip( nParts );
}

#ifdef __FINITEITEM0705
ItemProp* CMover::GetEquipItemProp( CItemContainer<CItemElem>* pInventory, PEQUIP_INFO pEquipInfo, int nParts )
#else	// __FINITEITEM0705
ItemProp* CMover::GetEquipItemProp( CItemContainer<CItemElem>* pInventory, LPDWORD padwEquipment, int nParts )
#endif	// __FINITEITEM0705
{
	ItemProp *pItemProp = NULL;
#if 1	
	if( pInventory == NULL  )
	{
#ifdef __FINITEITEM0705
		if( pEquipInfo[nParts].dwId != NULL_ID )
			pItemProp = prj.GetItemProp( pEquipInfo[nParts].dwId );
#else	// __FINITEITEM0705
		if( padwEquipment[nParts] != NULL_ID )
			pItemProp = prj.GetItemProp( padwEquipment[nParts] );
#endif	// __FINITEITEM0705
	} else
	{
		CItemElem *pItemElem = pInventory->GetEquip( nParts );		
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}
	
#endif
	return pItemProp;
}

// 쌍칼모드 인가.
// 양손에 쌍칼을 들고 있나.
BOOL CMover::IsDualWeapon( void )
{
#ifdef __CLIENT
	if( IsActiveMover() )
#else
	if( IsPlayer() )
#endif
	{
		CItemElem *pItemElemR = m_Inventory.GetEquip( PARTS_RWEAPON );
		CItemElem *pItemElemL = m_Inventory.GetEquip( PARTS_LWEAPON );
		if( pItemElemR && pItemElemL )
			return TRUE;
		else
			return FALSE;
	} else
	{
#ifdef __FINITEITEM0705
		if( m_aEquipInfo[PARTS_RWEAPON].dwId != NULL_ID && m_aEquipInfo[PARTS_LWEAPON].dwId != NULL_ID )
#else	// __FINITEITEM0705
		if( m_adwEquipment[ PARTS_RWEAPON ] != NULL_ID && m_adwEquipment[ PARTS_LWEAPON ] != NULL_ID )
#endif	// __FINITEITEM0705
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

//
//
//

extern int g_nCounter;

#ifdef __WORLDSERVER
	#include "GlobalTime.h"
	extern float	r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
	extern BOOL     g_bProfiling;

	class CProfileSample
	{
	public:
		CStopwatch&		m_stopWatch;
		float&			m_fValue;

		CProfileSample( CStopwatch& stopWatch, float& value ) : m_stopWatch( stopWatch ), m_fValue( value )
		{
			if( g_bProfiling )
				stopWatch.PlayEx();
		}

		~CProfileSample() 
		{
			if( g_bProfiling )
				m_fValue += m_stopWatch.StopEx();
		}
	};

	#ifdef __DEADLOCK__
		#define DEADLOCK_LOG( value )	_AddErrorSeq( nIdx, value );	
	#else
		#define DEADLOCK_LOG( value )	((void)0);		
	#endif

	#ifdef __EH
		#define SNAPSHOT_EH( n, value )  n = ( value );
	#else	
		#define SNAPSHOT_EH( n, value )  ((void)0);	
	#endif

	#define PROFILE( stopWatch, value ) CProfileSample __sample( stopWatch, value )
	#define PROCESS_LOG( n, value )		SNAPSHOT_EH(n, value);  DEADLOCK_LOG( value );
#else
	#define PROFILE( stopWatch, value ) ((void)0);
	#define PROCESS_LOG( n, value )		((void)0);
#endif

void CMover::Process(FLOAT fElapsedTime)
{
#ifndef __BEAST

	CModelObject*	pModel = (CModelObject*)m_pModel;
	D3DXVECTOR3		vPos, vPosTemp;
	int				i;
	BOOL			fCollision;	
	int				nIdx = (int)fElapsedTime;
	
#ifdef __WORLDSERVER
	CStopwatch		sw;
#endif

#ifdef __EH
	int				n = 0;
#endif

#ifdef __EH
	try
	{
#endif	// __EH

	if( IsDelete() )
		return;
	
#ifdef _DEBUG
	#ifdef __CLIENT
		if( g_pPlayer == this )
		{
			int a = 0;
		} else
		{
			int a = 0;
		}
	#else
		if( IsPlayer() )
		{
#ifdef __XUZHU
//			Sleep( 25 + rand() % 100 );		// 일부러 느려지게 하기.
#endif
			int a = 0;
		} else
		{
			int a = 0;
		}
	#endif	// __CLIENT
#endif	// __DEBUG

	PROCESS_LOG( n, 0 );

#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
	#ifdef __S0504_FLY
		if( IsStateMode( STATE_BASEMOTION_MODE ) )
		{
			if( m_nReadyTime <= timeGetTime() )
			{	// 세팅
				m_nReadyTime	= 0;
				if( m_bItemFind )
				{
					CItemElem* pItemElem = m_Inventory.GetAtId( m_dwUseItemId );
					if( pItemElem )
					{
						DoUseItem( MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId ), pItemElem->m_dwObjId );
					}
					else
					{
						( (CUser*)this )->AddDefinedText(TID_PK_BLINK_LIMIT, "" );	// 시전중에는 사용할수 없습니다
						SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );	// 셑팅 빼기

						m_dwUseItemId = 0;
					}
				}
				else
				{
					ItemProp* pItemProp = prj.GetItemProp( m_dwUseItemId );
					if( pItemProp )
					{
						DoUseItemVirtual( m_dwUseItemId, FALSE );
					}
				}
			}
		}
	#endif // __S0504_FLY			

	#ifdef __NPP_050308
		CUser* pUser = (CUser* )this;
		if( pUser->m_dwDestroyTime && ::timeGetTime() > pUser->m_dwDestroyTime )
		{
			g_UserMng.DestroyPlayer( pUser );
			return;
		}
	#endif// __NPP_050308

#ifdef __CHEER0615
		CheckTickCheer();
#endif	// __CHEER0615
	} 

	if( m_2pc.GetCount() == 0 )
		return;
#endif	// __WORLDSERVER

#ifdef __XFLYAI0524
	#ifdef __CLIENT
		if( IsNPC() && IsFlyingNPC() )	// 비행몹은 파티클 생성
		{
			void CreateFlyParticle( CMover *pMover, float fAngX, int nType );
			CreateFlyParticle( this, GetAngleX(), 0 );
		}
	#endif
#endif

	{
		PROFILE( sw, r1 );

		PROCESS_LOG( n, 1 );

		m_SkillState.Process();			// 걸려있는 스킬들의 상태에 대한 처리.
	}


#ifdef __CLIENT
	if( m_fDestScaleSlerp > 0 )		// 줄어 들어야 하는 상태다.
	{
		MoverProp *pMoverProp = GetProp();
		if( pMoverProp )
		{
			D3DXVECTOR3 vScale = GetScale();
			if( pMoverProp->dwClass != RANK_MATERIAL )
			{
				LPCTSTR szErr = Message( "CMover::Process : 자원몹이 아닌데 들어왔다.%s %f, %f, %f %f %f", GetName(), m_fDestScaleSlerp, m_fDestScale, vScale.x, vScale.y, vScale.z );
				ADDERRORMSG( szErr );
				m_fDestScaleSlerp = 0;
			} else
			{
				D3DXVECTOR3 vDestScale = D3DXVECTOR3( m_fDestScale, m_fDestScale, m_fDestScale );
				D3DXVec3Lerp( &vScale, &vDestScale, &vScale, m_fDestScaleSlerp );
				SetScale( vScale );
				m_fDestScaleSlerp -= 0.001f;
			}
		}
	}
#endif	// __CLIENT

	{
		PROFILE( sw, r2 );

		PROCESS_LOG( n, 2 );

		ProcessMove();	// 4767-2935
	}

	{
		PROFILE( sw, r3 );

		PROCESS_LOG( n, 3 );

		// ProcessAI 또는 이 밑 어딘가에서 AI에 매시지를 던진다. 특히 이동 매시지에 주의할 것.
		// 이동 매시지가 날아가면 반드시 1회 이동을 하고 ProcessAI가 실행되어야한다. 
		// 따라서 ProcessMove 다음에 ProcessAI가 있어야한다.
		ProcessAI( nIdx );
	}

	{
		PROFILE( sw, r4 );

		PROCESS_LOG( n, 4 );

		vPosTemp = vPos	= GetPos();

		// 현재 행동 상태에 따라 적절한	 모션을 처리함
		m_pActMover->ProcessPreAction();

		PROCESS_LOG( n, 5 );

			// 공속 애니메이션 속도 조절.
			if( IsPlayer() )
			{
				switch( m_pActMover->GetState() & OBJSTA_ATK_ALL )
				{
				case OBJSTA_ATK1:
				case OBJSTA_ATK2:
				case OBJSTA_ATK3:
				case OBJSTA_ATK4:
				case OBJSTA_ATK_MAGIC1:
			#ifdef __ACROBAT_0504
				case OBJSTA_ATK_RANGE1:
			#endif	// __ACROBAT_0504
					m_fAniSpeed = GetAttackSpeed();			
					break;
#ifdef __NEWWPN0901
				case OBJSTA_ATK_CASTING1:
				case OBJSTA_ATK_CASTING2:
				case OBJSTA_ATK_MAGICSKILL:
					m_fAniSpeed		= GetCastingAniSpeed();
					break;
#endif	// __NEWWPN0901
				default:
					{
						int nSpeed = GetAdjParam( DST_SPEED );

						if( nSpeed > 0 && (m_pActMover->GetState() == OBJSTA_FMOVE) )
							m_fAniSpeed = 1.5f;		// 퀵스탭이고 전진중이면 에니속도 증가
						else
						if( nSpeed < 0 && (m_pActMover->GetState() == OBJSTA_FMOVE) )
							m_fAniSpeed = 0.7f;		// 
						else
							m_fAniSpeed = 1.0f;		// 보통은 기본 속도.			
					}
				}
			} 
			else
			{	
				if( GetAdjParam( DST_CHRSTATE ) & CHS_SLOW )
					m_fAniSpeed = 0.5f;
				else
					m_fAniSpeed = 1.0f;
			}
		#ifdef __XPET
			switch( GetIndex() )
			{
			case MI_PET_LAWOLF:
			case MI_PET_AIBATT:
			case MI_PET_LEYENA:
			case MI_PET_LUIA:
				switch( m_pActMover->GetState() )
				{
				case OBJSTA_FMOVE:
					m_fAniSpeed = 3.0f;
					break;
				default:
					m_fAniSpeed = 1.0f;
				}
				break;
			}
		#endif
	}
	
	{
		PROFILE( sw, r5 );


		#ifdef __X15
		for( i = 0; i < 4; i ++ )
		#endif
		{
			PROCESS_LOG( n, 60 );

			#ifdef __XFLYAI0524
				ProcessMovePattern();
			#endif // xflyAI0524

			PROCESS_LOG( n, 70 );

			m_pActMover->ProcessAction( &vPos, nIdx );		// 4767-83 최적화 필요, 원인 분석

			PROCESS_LOG( n, 80 );

#ifdef __YWALK_SOUND
			// 플레이어들이 뛸때 사운드소리를 낸다...콜라에서 사운드지정으로 플레이하는것이 아님.
			// 임시적으로 히트포인트를 사운드 플레이 시점으로 사용함...
			if( IsPlayer() && !( IsMode( TRANSPARENT_MODE ) ) )
			{
				if( m_pActMover->GetMoveState() & OBJSTA_FMOVE )
				{
					if( pModel->IsAttrHit() )
					{
						PLAYSND( SND_WALK_LANDSOFT, &GetPos() );
					}
				}
			}
#endif //__YDEBUG			
			
			
			

			// 애니메이션 프레임 증가
			if( IsAnimate() )
			{
				#ifdef XSKILL0806
				if( (m_dwFlag & MVRF_FREEZE) == 0 )		// 프리즈 상태에선 애니메이션 안됨.
				#endif
				{					
					pModel->FrameMove( &vPos, m_fAniSpeed );		// 애니메이션 프레임 증가
				}
				
				#ifdef __CLIENT			
				if( ! ( IsMode( TRANSPARENT_MODE ) ) )		// 투명상태가 아닐때만 렌더.
				{
					MOTION_ATTR* pAttr = pModel->IsAttrSound();
 					if( IsPlayer() && pAttr && pAttr->m_nSndID > 0 ) //&& pModel->m_nPause == 0 )		// 효과음 속성이 있다면 플레이, pause상태면 사운드 출력 안함
					{
						if( m_pActMover->IsActJump() )
							CreateSfx( g_Neuz.m_pd3dDevice, XI_NAT_DUST_JUMP, GetPos() );
						else
						if( m_pActMover->IsRun() && !m_pActMover->IsActAttack() )
							CreateSfx( g_Neuz.m_pd3dDevice, XI_NAT_DUST_RUN, GetPos() );
					}
				}
				#endif // CLIENT
			}

			PROCESS_LOG( n, 9 );

			if( m_nAtkCnt > 0 )		// 공격행위를 하고난 이후는 카운트가 계속 증가 한다.
				m_nAtkCnt ++;

		}	// for( 4 )
	}

	{
		PROFILE( sw, r6 );

		PROCESS_LOG( n, 10 );

		// 이동성분 더하기 & 충돌처리.
		fCollision = m_pActMover->ProcessCollision( &vPos );
	}

	{
		PROFILE( sw, r7 );

		PROCESS_LOG( n, 11 );

		if( m_pRide )
			m_pRide->FrameMove();

		#ifdef __XLADOLF
			if( m_pLadolf )
				m_pLadolf->FrameMove();
		#endif

		#ifdef __CLIENT
			if( m_nDmgCnt > 0 )		
				m_nDmgCnt --;
		#endif // CLIENT

		PROCESS_LOG( n, 12 );

		if( GetIAObjLink() )
		{
			D3DXMATRIX mLocal, mTrans, mRot, mWorld, *pmWorldIA;
			D3DXMatrixTranslation( &mTrans, vPos.x, vPos.y, vPos.z );
			D3DXMatrixRotationY( &mRot, D3DXToRadian( -GetAngle() ) );
			mWorld = m_matScale;
			
		#ifdef __WORLDSERVER
			// ※CMover에서 CShip에 락을 걸었으므로, CShip의 락 상태에서 CMover에 락을 걸면 교착상태에 빠질 수 있다.
			GetIAObjLink()->Lock( theLineFile );
		#endif	// __WORLDSERVER

			pmWorldIA = GetIAObjLink()->GetMatrixWorldPtr();
//			D3DXMatrixMultiply( &mWorld, &mWorld, &m_matRotation );
			D3DXMatrixMultiply( &mWorld, &mWorld, &mRot );
			D3DXMatrixMultiply( &mWorld, &mWorld, &mTrans );
			D3DXMatrixMultiply( &mLocal, &mWorld, GetIAObjLink()->GetInvTM() );	// IA오브젝트로부터의 로컬 행렬.
			D3DXMatrixMultiply( &mWorld, &mLocal, pmWorldIA );

		#ifdef __WORLDSERVER
			GetIAObjLink()->Unlock( theLineFile );
		#endif	// __WORLDSERVER

			vPos.x = mWorld._41;
			vPos.y = mWorld._42;
			vPos.z = mWorld._43;
			
			void QuaternionRotationToYPW( const D3DXQUATERNION qRot, D3DXVECTOR3& vYPW );
			D3DXQUATERNION qRot;
			D3DXVECTOR3 vYPW;
			D3DXQuaternionRotationMatrix( &qRot, &mWorld );
			QuaternionRotationToYPW( qRot, vYPW );
			FLOAT fAngNew = -D3DXToDegree(vYPW.y);
			SetAngle( fAngNew );
				
		}
	}
	
	#ifdef __CLIENT
		// 수면 효과 - 투명이 아닐때만 출력
		if( !IsMode( TRANSPARENT_MODE ) )	
		{
			LPWATERHEIGHT pWaterHeight = GetWorld()->GetWaterHeight( GetPos() );
			if( pWaterHeight && pWaterHeight->byWaterTexture == WTYPE_WATER )
			{
				FLOAT fHeight = (FLOAT)pWaterHeight->byWaterHeight;
				if( vPosTemp.y > fHeight && vPos.y <= fHeight && m_pActMover->IsActJump() ) 
				{
					CreateSfx( g_Neuz.m_pd3dDevice, XI_GEN_WATERCROWN01, D3DXVECTOR3( vPos.x, fHeight, vPos.z ) );
				}
				if( vPos != vPosTemp && vPos.y < fHeight && vPos.y > ( fHeight - 1.5f )  && m_nWaterCircleCount > 7 ) 
				{
					CreateSfx( g_Neuz.m_pd3dDevice, XI_GEN_WATERCIRCLE01, D3DXVECTOR3( vPos.x, fHeight + .01f, vPos.z) );
					m_nWaterCircleCount = 0;
				}
			}
			m_nWaterCircleCount++;
		}
	#endif /// __CLIENT

	#ifdef __WORLDSERVER
	{
		{
			PROFILE( sw, r8 );

			PROCESS_LOG( n, 13 );

			SetPos( vPos );
		}

		{
			PROFILE( sw, r9 );

			PROCESS_LOG( n, 14 );

			if( fCollision )
				OnActCollision();
		}
	}
	#else	// __WORLDSERVER
	{
		if( m_nCorr > 0 )
			m_nCorr--;
		// 클라이언트일 경우 이동할 좌표의 랜드가 있을 경우에만 이동을 허가 
		if(  GetWorld()->GetLandscape( vPos ) )
		{
			SetPos( vPos );
			if( fCollision )
				OnActCollision();
		}
	}
	#endif	// !__WORLDSERVER

	{
		PROFILE( sw, r10 );

		PROCESS_LOG( n, 15 );

		ProcessRegion();
	}
#ifdef __V050322_CACHEQUEST  
	ProcessQuest();
#endif
#ifdef __V050426_REMOVERESPAWN
#ifdef __WORLDSERVER
	ProcessDeleteRespawn();
#endif // __WORLDSERVER
#endif // __V050426_REMOVERESPAWN

	// 클라이언트 에서 온라인 상태면 수행안함
	#if !( ( defined( __CLIENT ) && defined( __ONLINE ) ) || defined( __BEAST ) )
	{
		PROFILE( sw, r11 );

		PROCESS_LOG( n, 16 );

		ProcessRegenItem();

		PROCESS_LOG( n, 17 );

		ProcessScript();

	}
	#endif	// !( ( defined( __CLIENT ) && defined( __ONLINE ) ) || defined( __BEAST ) )
	
	{
		PROFILE( sw, r11 );

		PROCESS_LOG( n, 18 );

		#ifdef __ONLINE
			Interpolate();
		#endif	// __ONLINE

		#ifdef __WORLDSERVER			
			ProcessRecovery();	// 히트포인트, 마나포인트, 피로포인트 회복 하기  일단은 플레이어만 자동으로 찬다.
		#endif  // WORLDSERVER

		PROCESS_LOG( n, 19 );

		#ifdef __WORLDSERVER
			if( m_wStunCnt > 0 )
			{
				if( --m_wStunCnt <= 0 )
					SetStun( FALSE );	// 스턴 상태 해제.
			}
			#ifdef __ACROBAT_0504
			if( m_wDarkCover > 0 )
			{
				if( --m_wDarkCover <= 0 )
					SetDarkCover( FALSE );	// 스턴 상태 해제.
			}
			#endif //__ACROBAT_0504			
	
			if( GetAdjParam( DST_CHRSTATE ) & CHS_POISON )
			{
				if( IsLive() )
				{
					int nUnitCnt = (m_tmPoisonUnit / 1000.0f) * PROCESS_COUNT;
					if( nUnitCnt <= 0 )		
					{
						nUnitCnt = 1;
						SetPoison( FALSE );
					} else
					{
						if( (m_nCount % nUnitCnt) == 0 )		// xx초마다 한번씩 데미지.
						{
							CCtrl *pAttacker = prj.GetCtrl( m_idPoisonAttacker );
							if( IsValidObj( pAttacker ) && pAttacker->GetType() == OT_MOVER )
							{
								m_pActMover->SendDamage( AF_FORCE, 
													     pAttacker->GetId(), 
														 m_wPoisonDamage );
							}
						}

						if( m_wPoisonCnt > 0 )	// 독 지속 시간동안.. SkillInfluence로 돌때는 이게 0이다
							if( --m_wPoisonCnt <= 0 )	// 독 시간 다되면

								SetPoison( FALSE );		// 독상태 헤제.

					}
				}
			}
			
			if( m_wDarkCnt > 0 )	// 암흑 지속시간동안...
			{
				if( --m_wDarkCnt <= 0 )
					SetDark( FALSE );
			}

			if( GetAdjParam( DST_CHRSTATE ) & CHS_BLEEDING )
			{
				if( IsLive() )
				{
					int nUnitCnt = (m_tmBleedingUnit / 1000.0f) * PROCESS_COUNT;
					if( nUnitCnt <= 0 )		
					{
						nUnitCnt = 1;
						SetBleeding( FALSE );
					} else
					{
						if( (m_nCount % nUnitCnt) == 0 )		// xx초마다 한번씩 데미지.
						{
							CCtrl *pAttacker = prj.GetCtrl( m_idBleedingAttacker );
							if( IsValidObj( pAttacker ) && pAttacker->GetType() == OT_MOVER )
							{
								m_pActMover->SendDamage( AF_FORCE, 
													     pAttacker->GetId(), 
														 m_wBleedingDamage );
							}
						}

						if( m_wBleedingCnt > 0 )	// 출혈 지속 시간동안..
							if( --m_wBleedingCnt <= 0 )	// 독 시간 다되면

								SetBleeding( FALSE );		// 독상태 해제.
					}
				}
			}
			if( m_nDead > 0 )		
				m_nDead --;
		#endif // __WORLDSERVER

		PROCESS_LOG( n, 20 );

		if( IsPlayer() )
		{
			if( m_tmCrime )
			{
				int tmTimer = (int)(g_tmCurrent - m_tmCrime);
				if( tmTimer > SEC(CRIME_TIME) )		// 범죄 저지른지 30초가 지나면 범죄해제
					m_tmCrime = 0;
			}
		} 
		else	
		{
			// 몬스터의 경우 정기적으로 idAttacker를 검사해서 없어진놈이면 삭제함.
			#ifdef __WORLDSERVER
				if( (m_nCount & 127) == 0 )		// 약 8초만에 한번씩 검사해서 날 타겟으로 잡았던놈이 없어졌으면 클리어 해줌.
				{
					if( m_idAttacker != NULL_ID || m_idTargeter != NULL_ID )		// 0819
					{
						CMover *pAttacker = GETMOVER( m_idAttacker );
						if( IsValidObj( pAttacker ) )
						{
							if( IsValidArea( pAttacker, 32.0f ) == FALSE )
							{
								m_idAttacker = NULL_ID;
							}
						} 
						else
						{
							m_idAttacker = NULL_ID;		// 어태커가 아예 없어졌으면 
						}

						CMover *pTargeter = GETMOVER( m_idTargeter );
						if( IsValidObj( pTargeter ) )
						{
							if( IsValidArea( pTargeter, 32.0f ) == FALSE )
							{
								m_idTargeter = NULL_ID;
							}
						} 
						else
						{
							m_idTargeter = NULL_ID;		// 타게터가 아예 없어졌으면 초기화.
						}
					}
#ifdef __INSTANT_BANK0831
					ProcInstantBanker();
#endif	// __INSTANT_BANK0831
				}
				if( m_nCollectOwnCnt > 0 )
					m_nCollectOwnCnt --;
			#endif	// __WORLDSERVER
		}

		{
			DWORD tmCurrent = timeGetTime();
			for( i = 0; i < MAX_SKILL_JOB; i ++ )
			{
				if( m_tmReUseDelay[i] )
				{
					{
						m_tmReUseDelay[i] -= (tmCurrent - m_tmTimer);		// 경과한시간만큼 빼줌.
						if( (int)m_tmReUseDelay[i] < 0 )
							m_tmReUseDelay[i] = 0;
					}
				}
			}
		}

		PROCESS_LOG( n, 21 );

		// 피케이 당한지 10분이 지나면 클리어.
		if( m_tmDead && ((int)(g_tmCurrent - m_tmDead) > (1000 * 60 * 10)) )		
			m_tmDead = 0;

		if( IsPlayer() && IsLive() )
		{
			// 액션포인트 자동 상승.
			if( (int)(g_tmCurrent - m_tmActionPoint) > (1000 * 3) )		
			{
				FLOAT fTime = (g_tmCurrent - m_tmActionPoint) / 1000.0f;	// 지나간 시간을 소수점 단위로 환산.
				fTime /= 3.0f;		// 3초마다 갱신이므로.
				if( fTime < 0 )	fTime = 0;

			#ifdef __WORLDSERVER
				((CUser*)this)->m_playTaskBar.m_nActionPoint += (int)(fTime * 2.0f);
				if( ((CUser*)this)->m_playTaskBar.m_nActionPoint > 100 )
					((CUser*)this)->m_playTaskBar.m_nActionPoint = 100;

				if( m_nDuel == 1 )		// 플레이어가 / 듀얼중일때.
				{
					CMover *pDstDuel = prj.GetMover( m_idDuelOther );
					if( IsInvalidObj(pDstDuel) )		// 듀얼상대가 없어졌으면.
					{
						( (CUser*)this )->AddDuelCancel( NULL_ID );
						ClearDuel();
					}
				}
			#endif // __WORLDSERVER

			#ifdef __CLIENT
				if( IsActiveMover() )
				{
					g_WndMng.m_pWndTaskBar->m_nActionPoint += (int)(fTime * 2.0f);
					if( g_WndMng.m_pWndTaskBar->m_nActionPoint > 100 )
						g_WndMng.m_pWndTaskBar->m_nActionPoint = 100;

				#ifdef __NPP_050308
					g_Neuz.m_NeuzEnemy.CheckInvalid();
				#endif // __NPP_050308
				}
			#endif // CLIENT
				
				m_tmActionPoint = g_tmCurrent;
			} 

		#ifdef __WORLDSERVER
			if( IsPlayer() )
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
							ClearDestAngle();
							memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
							
							g_UserMng.AddMotion( this, OBJMSG_SITDOWN );
							m_dwFlag &= (~MVRF_MEDITATION_SIT);
						}
					}
				}
			}
		#endif // __WORLDSERVER

		// 비행연료 자동 소모

		#ifdef __WORLDSERVER
			if( m_nFuel > 0 )
			{
				if( (m_nCount & 15) == 0 )
				{
					if( m_pActMover->IsFly() && m_pActMover->IsStateFlag( OBJSTAF_ACC ) )
					{
						ItemProp *pItemProp = prj.GetItemProp( m_dwRideItemIdx );
						if( pItemProp )
						{
						#ifdef __XFUEL
							m_nFuel -= (int)pItemProp->dwFFuelSp;	// 초당 소모량.
							if( m_nFuel < 0 )	
							{
								m_nFuel = 0;
								SendActMsg( OBJMSG_ACC_STOP );		// 가속 중지.
							}
						#endif // XFUEL

						#if defined(__XFUEL) || defined(__XTURBO)
							((CUser *)this)->AddSetFuel( m_nFuel, m_tmAccFuel );	// 클라에 변화된 양을 전송
						#endif
						}
					}
				}
			}

			if( ((CUser *)this)->m_nOverHeal > 0)		// cast를 해야하는 이 아픔.
			{
				((CUser *)this)->m_nOverHeal --;
			}
		#endif // WORLDSERVER
		} // IsPlayer() && IsLive()

		PROCESS_LOG( n, 22 );

		m_nCount ++;	// 무버가 범용으로 쓰는 순차적 카운터. 생성자 외엔 0으로 초기화 하지 말것.
	}	// PROFILE BLOCK

#ifdef __EH
	}
	catch( ... )
	{
		WriteError( "%s %d, i = %d", __FILE__, __LINE__, n );
	}
#endif	// __EH

#endif	// __BEAST

#ifndef __BEAST
	m_tmTimer = timeGetTime();
#endif
	
// 세트 아이템 효과ㅏ 이펙트
#if defined(__CLIENT) && defined(__J0608)
	int nAbilityOption	= GetSetItemClient();
	if( nAbilityOption >= 3 && !(m_dwFlag & MVRF_SETITEMSFX))
    {
		m_dwFlag |= MVRF_SETITEMSFX;

		CreateAbilityOption_SetItemSFX( nAbilityOption );

	}
#endif //defined(__CLIENT) && defined(__J0608)

	
#if defined(__CLIENT) && defined(__YSPARK)
	if( pModel->m_SparkInfo.m_bUsed )
	{
		if( pModel->m_SparkInfo.m_nCnt != 0 )
		{
			pModel->m_SparkInfo.m_fLerp -= 0.05f;

			if( pModel->m_SparkInfo.m_fLerp < 0.0f )
			{
				pModel->m_SparkInfo.m_bUsed = FALSE;
			}
		}
		
		pModel->m_SparkInfo.m_nCnt++;
	}
#endif //defined(__CLIENT) && defined(__YSPARK)
	
#ifdef __YADV_ENCHANT_SFX
#ifdef __CLIENT
//	UpdateLocalMatrix();		// 프로세스에서 매트릭스 갱신 하도록 바꿈.
#endif
#endif //__YADV_ENCHANT_SFX	
//	TRACE( "%5.3f\r\n", GetPos().y );

#ifdef __CLIENT
#ifdef __YDARKVADER_CHAT
	if( (m_nCount & 131071) == 0 ) // 30분마다
	{
		if( xRandom(100) > 50 )
		{
			ItemProp* pItemProp = NULL;

			if( IsActiveMover() )
			{
				CItemElem* pItemElem	= GetEquipItem( PARTS_HAT );

				if( pItemElem )
					pItemProp = pItemElem->GetProp();
			}
			else
			{
		#ifdef __FINITEITEM0705
				DWORD dwId	= m_aEquipInfo[PARTS_HAT].dwId;
		#else	// __FINITEITEM0705
				DWORD dwId	= m_adwEquipment[PARTS_HAT];
		#endif	// __FINITEITEM0705
				O3D_ELEMENT*	pElement = NULL;
				
				if( dwId != NULL_ID )
					pItemProp	= prj.GetItemProp( dwId );
			}


			if( pItemProp )
			{
				if( GetSex() == SEX_MALE )
				{
					if( pItemProp->dwID == II_ARM_M_CHR_DARKVADER01HAT )
					{
						char szFormat[128] = {0};
						sprintf( szFormat, "/s %s", prj.GetText(TID_GAME_FATHER) );
						g_DPlay.SendChat( szFormat );
					}
				}
				else
				{
					if( pItemProp->dwID == II_ARM_F_CHR_DARKVADER01HAT )
					{
						char szFormat[128] = {0};
						sprintf( szFormat, "/s %s", prj.GetText(TID_GAME_MOTHER) );
						g_DPlay.SendChat( szFormat );
					}
				}
			}		
		}
	}
#endif //__YDARKVADER_CHAT
#endif //__CLIENT
}

#ifdef __CLIENT
void CMover::CreateAbilityOption_SetItemSFX( int nAbilityOption )
{
	DWORD dwSfx = 0;

//	nAbilityOption = 4;
	
	switch( nAbilityOption )
	{
	case 3: dwSfx = XI_GEN_ITEM_SETITEM03; break;
	case 4: dwSfx = XI_GEN_ITEM_SETITEM04; break;
	case 5: dwSfx = XI_GEN_ITEM_SETITEM05; break;
	case 6: dwSfx = XI_GEN_ITEM_SETITEM06; break;
	case 7: dwSfx = XI_GEN_ITEM_SETITEM07; break;
	case 8: dwSfx = XI_GEN_ITEM_SETITEM08; break;
	case 9: dwSfx = XI_GEN_ITEM_SETITEM09; break;
	case 10: dwSfx = XI_GEN_ITEM_SETITEM10; break;
	}
	
#ifdef __YADV_ENCHANT_SFX
	CSfxPartsLinkShoulder* pSfx = NULL;
	D3DXVECTOR3 v3Scal = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	
	if( nAbilityOption == 3 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	/*
	else
	if( nAbilityOption == 4 )
	{
		// 몸통

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3( 2.2f, 2.2f, 2.2f ));
	}
	*/
	else
	if( nAbilityOption == 4 )
	{
		// 몸통

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3( 3.0f, 3.0f, 3.0f ));
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 5 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 어깨, 팔목 중간
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 6 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 어깨, 팔목 중간
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 팔목

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 7 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 어깨, 팔목 중간
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 팔목

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 손	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 8 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 어깨, 팔목 중간
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 팔목

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 손	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 9 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 어깨, 팔목 중간
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 팔목

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 손	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;		
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 발
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 26;	
		pSfx->SetScale(D3DXVECTOR3( 1.5f, 1.5f, 1.5f ));		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 29;
		pSfx->SetScale(D3DXVECTOR3( 1.5f, 1.5f, 1.5f ));
		pSfx->m_nOldAbilityOption = nAbilityOption;
		
		
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else
	if( nAbilityOption == 10 )
	{
		// 어깨

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 어깨, 팔목 중간
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 4;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 5;	
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 몸통

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3( 3.0f, 4.5f, 3.0f ));
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 7;		
		pSfx->SetScale(D3DXVECTOR3( 3.0f, 3.0f, 3.0f ));		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 손	
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;	
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 팔목

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 2;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 3;		
		pSfx->m_nOldAbilityOption = nAbilityOption;
		// 발
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 26;			
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 29;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 8;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx( D3DDEVICE, XI_SETIEM_EFFECTHAND, GetPos(), GetId(), GetPos(), GetId(), -1 );
		pSfx->m_nPartsLink = 9;			
		pSfx->m_v3Offset.y = 0.2f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
#else //__YADV_ENCHANT_SFX
	CreateSfx( D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1 );
#endif //__YADV_ENCHANT_SFX	
}

#endif //__CLIENT

void CMover::GenerateVendorItem( ItemProp** apItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor )
 //int nItemKind, int nItemKind2, int nNumMax, int nUniqueMin, int nUniqueMax, int nTotalNum, CAnim* pAnimParent, int nMaterialCount )
{
#ifndef __BEAST
	CPtrArray* pItemKindAry		= prj.GetItemKindAry( pVendor->m_nItemkind3 );
	ItemProp* pItemProp		= NULL;
	int cbSizeOld	= *pcbSize;

	chASSERT( pVendor->m_nUniqueMin >= 0 );
	chASSERT( pVendor->m_nUniqueMax >= 0 );
	
	if( *pcbSize >= nMax )
		return;

	int nMinIdx	= -1, nMaxIdx	= -1;

	for( int j = pVendor->m_nUniqueMin; j <= pVendor->m_nUniqueMax; j++ )
	{
		nMinIdx		= prj.GetMinIdx( pVendor->m_nItemkind3, j );
		if( nMinIdx != -1 )
			break;
	}
	for( j = pVendor->m_nUniqueMax; j >= pVendor->m_nUniqueMin; j-- )
	{
		nMaxIdx		= prj.GetMaxIdx( pVendor->m_nItemkind3, j );
		if( nMaxIdx != -1 )
			break;
	}
#ifdef __MAINSERVER
	chASSERT( nMinIdx >= 0 );
#else	// __TESTSERVER
	if( nMinIdx < 0 )
	{
		WriteError( "VENDORITEM//%s//%d-%d//%d", GetName(), pVendor->m_nUniqueMin, pVendor->m_nUniqueMax, pVendor->m_nItemkind3 );
		return;
	}
#endif	// __TESTSERVER

	chASSERT( nMaxIdx >= 0 );

	for( int k = nMinIdx; k <= nMaxIdx; k++ )
	{
		pItemProp	= (ItemProp*)pItemKindAry->GetAt( k );

		if( ( NULL == pItemProp ) ||
			( pItemProp->dwShopAble == (DWORD)-1 ) ||
			( pVendor->m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor->m_nItemJob ) )
			continue;
		
		if( *pcbSize >= nMax )
			break;

		apItemProp[*pcbSize]	= pItemProp;
		(*pcbSize)++;
	}
#endif	// __BEAST
}

void CMover::ProcessRegenItem()
{
	if( IsNPC() )
	{
		if( m_bRegenItem )
		{

			m_bRegenItem = FALSE;
			//
			// vender item 발생 
			//
			//#define VENDER_DELAY (60 * 5) // 5분 
			LPCHARACTER pCharacter = GetCharacter();
			BOOL fShop	= FALSE;
			if( !pCharacter )
				return;

			LPVENDOR_ITEM pVendor;
			for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i ++ )
			{
				if( pCharacter->m_venderItemAry[i].GetSize() )
				{
					fShop	= TRUE;
					{
						m_ShopInventory[i]->Clear();		// m_pack을 다 없앤다.

						ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
						int cbSize	= 0;
						// generate
						for( int j = 0; j < pCharacter->m_venderItemAry[i].GetSize(); j++ )
						{
							pVendor		= (LPVENDOR_ITEM)pCharacter->m_venderItemAry[i].GetAt(j);
							GenerateVendorItem( apItemProp, &cbSize, MAX_VENDOR_INVENTORY, pVendor );
						}
						// sort
						for( j = 0; j < cbSize - 1; j++ )
						{
							for( int k = j + 1; k < cbSize; k++ )
							{
								if( ( apItemProp[k]->dwItemKind1 < apItemProp[j]->dwItemKind1 ) ||
									( apItemProp[k]->dwItemKind1 == apItemProp[j]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[j]->dwItemRare ) )
								{
									ItemProp* ptmp	= apItemProp[j];
									apItemProp[j]	= apItemProp[k];
									apItemProp[k]	= ptmp;
								}
							}
						}
						// add
						for( j = 0; j < cbSize; j++ )
						{
							CItemElem itemElem;
							itemElem.m_dwItemId	= apItemProp[j]->dwID;
							itemElem.m_nItemNum		= apItemProp[j]->dwPackMax;
//							itemElem.m_nHitPoint	= -1;
							itemElem.m_nHitPoint	= apItemProp[j]->dwEndurance;
							if( m_ShopInventory[i]->Add( &itemElem ) == FALSE )
								break;
						}
					}
				}
			}

#ifdef __WORLDSERVER
			if( fShop )
				g_UserMng.AddVendor( this );
#endif	// __WORLDSERVER
		}
	}
}


void CMover::EnforcedGhostCorr( void )
{
#ifndef __BEAST
	if( m_CorrAction.fValid == TRUE && ( m_uRemnantCorrFrm - 1 ) == 0 ) {
		if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
			ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.fAngleX,
						m_CorrAction.fAccPower, m_CorrAction.fTurnAngle, 
						m_CorrAction.dwState, m_CorrAction.dwStateFlag,
						m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
		else {
			ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.dwState,
						m_CorrAction.dwStateFlag, m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
//		m_uRemnantCorrFrm	= 0;
		memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	}
#endif
}

void CMover::ApproachGhostAngle( void )
{
#ifndef __BEAST
	if( m_fDestAngle > -0.001f )
	{
		float f	= m_pActMover->IsFly()? 0.601f: 4.001f;
		if( fabs(  GetAngle() - m_fDestAngle ) < f ) {
			if( m_CorrAction.fValid == FALSE ) {
				SetAngle( m_fDestAngle );
				ClearDestAngle();
				SendActMsg( OBJMSG_STOP_TURN );
			}
			else {
				ClearDestAngle();
				if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
//					ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.fAngleX,
//						m_CorrAction.fAccPower, m_CorrAction.fTurnAngle, 
//						m_CorrAction.dwState, m_CorrAction.dwStateFlag,
//						m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
//						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
				}
				else {
					ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.dwState,
						m_CorrAction.dwStateFlag, m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
						m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
				}
				memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
			}
		}
		else {
			f	= m_pActMover->IsFly() ? 0.6f * 100.0f : 0.0f;
			if( m_bLeft )
				SendActMsg( OBJMSG_LTURN, (int)f );
			else
				SendActMsg( OBJMSG_RTURN, (int)f );
		}
	}
#endif	// __BEAST
}

void CMover::OnArriveAtPos( void )
{  
#ifndef __BEAST
	if( m_CorrAction.fValid == FALSE )
	{
		SendActMsg( OBJMSG_STAND );
#ifdef __POSTAIMSG1008
		PostAIMsg( AIMSG_ARRIVAL, NULL_ID, 1 );
#else	// __POSTAIMSG1008
		SendAIMsg( AIMSG_ARRIVAL, NULL_ID, 1 );
#endif	// __POSTAIMSG1008
	}
	else {
#ifdef __J0518
		if( m_CorrAction.fHalf )
		{
			HalfForceSet( m_CorrAction.v, m_CorrAction.vd,
								m_CorrAction.f, m_CorrAction.fAngleX,
								m_CorrAction.fAccPower, m_CorrAction.fTurnAngle );
		}
		else
		{
#endif	// __J0518
			if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
				ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.fAngleX,
					m_CorrAction.fAccPower, m_CorrAction.fTurnAngle, 
					m_CorrAction.dwState, m_CorrAction.dwStateFlag,
					m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
					m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
			}
			else {
				ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f, m_CorrAction.dwState,
					m_CorrAction.dwStateFlag, m_CorrAction.dwMotion, m_CorrAction.nMotionEx,
					m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
			}
#ifdef __J0518
		}
#endif	// __J0518
		memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	}
	ClearDestPos();
#endif
}

BOOL CMover::AddItem( CItemBase* pItemBase )
{
	return m_Inventory.Add( (CItemElem*)pItemBase );
}

BOOL CMover::AddItemBank( int nSlot, CItemElem* pItemElem )
{
	return m_Bank[nSlot].Add( pItemElem );
}

CItemBase* CMover::GetItemId( DWORD dwId )
{
	return m_Inventory.GetAtId( dwId );
}

CItemBase* CMover::GetItemBankId( int nSlot, DWORD dwId )
{
	CItemBase* pItemBase = NULL;
	pItemBase = m_Bank[nSlot].GetAtId( dwId );
	return pItemBase;
}

void CMover::RemoveItemId( DWORD dwId )
{
#ifndef __BEAST
	if( m_Inventory.IsEquip( dwId ) )
		DoEquip( (CItemElem*)m_Inventory.GetAtId( dwId ), FALSE );		// 실패할 경우, 장착 해제 효과가 나타나지 않으므로 고려가 필요함.
	m_Inventory.RemoveAtId( dwId );
#endif	// __BEAST
}

void CMover::RemoveItemBankId( int nSlot, DWORD dwId )
{
	m_Bank[nSlot].RemoveAtId( dwId );
}


#ifdef __CLIENT
void CMover::DialogOut( LPCTSTR lpszText )
{
	g_DialogMsg.AddMessage( this, lpszText, 0xffffffff );
}
#endif

// bSWDForce대신에 Option을 써서 비트로 사용하자.
// 0x01 : 검광
// 0x02 : 고정모션(+100안하는..)
// 0x04 : Motion Transition 사용하지 않음.
BOOL CMover::SetMotion( DWORD dwMotion, int nLoop, DWORD dwOption )
{
#ifndef __BEAST
	CModelObject* pModel = (CModelObject*)m_pModel;
	DWORD	dwOrigMotion = dwMotion;
	ItemProp* pItemProp = GetActiveHandItemProp();
	if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) && (dwOption & MOP_FIXED) == 0 )
	{
#if defined(__CLIENT) 
		if( pItemProp == NULL )
		{
			if( IsPlayer() )
			{
#ifdef __FINITEITEM0705
				LPCTSTR szErr = Error( "CMover::SetMotion : Player %s %d %d", GetName(), IsActiveMover(), m_aEquipInfo[PARTS_RWEAPON].dwId );
#else	/// __FINITEITEM0705
				LPCTSTR szErr = Error( "CMover::SetMotion : Player %s %d %d", GetName(), IsActiveMover(), m_adwEquipment[ PARTS_RWEAPON ] );
#endif	// __FINITEITEM0705
				ADDERRORMSG( szErr );
			}
			else
			{
				LPCTSTR szErr = Error( "CMover::SetMotion : NPC %s %d", GetName(), m_dwVirtItem );
				ADDERRORMSG( szErr );
			}
		}
#endif // __CLIENT

		// 몹일 경우 dwItemKind3는 virtual 아이템이다.
		// 플레이어는 무기 종류에 따라서 모션을 결정하지만, 몹은 virtual 아이템에서 모션을 결정한다.
		int nIndex = GetIndex();
		if( !IsPlayer() || ( IsPlayer() && 
			( nIndex == MI_MALE || 
			  nIndex == MI_FEMALE || 
			  nIndex == MI_CROWNIBLIS || 
			  nIndex == MI_CROWNSHADE || 
			  nIndex == MI_CROWNBUBBLE || 
			  nIndex == MI_DWARPETMAS ) ) )  
		{
			BOOL bMasterPlayer = FALSE;

			if( nIndex == MI_CROWNIBLIS || nIndex == MI_CROWNSHADE || nIndex == MI_CROWNBUBBLE || nIndex == MI_DWARPETMAS ) 
				bMasterPlayer = TRUE;

			if( IsDualWeapon() && !bMasterPlayer )
			{
				dwMotion += 900;		// 이도류는 무슨 무기를 들었든 간에 이도류 동작이 된다.
			} 
			else
			{
				BOOL bHanded = ( pItemProp->dwHanded == HD_TWO ) ? TRUE : FALSE;	// 투핸드냐 원핸드냐?
				switch( pItemProp->dwItemKind3 )
				{
#ifdef __ACROBAT_0504
				case IK3_YOYO: 
					if( !bMasterPlayer ) dwMotion += MTI_STAND_14;
					else dwMotion += 200;
					break;
				case IK3_BOW: 
					if( !bMasterPlayer ) dwMotion += MTI_STAND_13;
					else dwMotion += 200;
					break;               
#endif //__ACROBAT_0504					
				case IK3_SWD: 
					if( bHanded )	dwMotion += 800;	
					else			dwMotion += 200; 
					break;
				case IK3_AXE: 
					if( bHanded )	dwMotion += 800;
					else			dwMotion += 500; 
					break;               
				case IK3_WAND:
					if( GetJob() == JOB_PSYCHIKEEPER )	dwMotion += MTI_STAND_11;
					else								dwMotion += MTI_STAND_02; 
					break; 
				case IK3_CHEERSTICK: dwMotion += 400; break; 
				case IK3_STAFF: 
					dwMotion += MTI_STAND_05; 
					break; 
				case IK3_KNUCKLEHAMMER: dwMotion += 700; break; 
				default: 
					if( IsPlayer() ) dwMotion += 200; 
					break;
				}
			}
		}
	}
	if( m_dwMotion == dwOrigMotion && m_dwMotionOption == dwOption )	// 같은 모션을 하라고 했는데...
	{
		if( nLoop == ANILOOP_LOOP )		return FALSE;	// 루핑모드 이면 걍 리턴
		if( pModel->m_bEndFrame == FALSE )		// 아직 애니메이션중일때는 
			return FALSE;						// 취소.
		if( pModel->m_bEndFrame && nLoop == ANILOOP_CONT )	// 애니메이션이 끝난상태고 지속모드면 마지막 프레임으로 지속
			return FALSE;
	}
#ifdef _DEBUG
	if( dwOrigMotion == MTI_WALK )
	{
		int a = 0;
	}
#endif
	prj.m_modelMng.LoadMotion( m_pModel, m_dwType, m_dwIndex, dwMotion );
	m_dwMotionOld = m_dwMotion;		// 옛날모션 백업

	m_dwMotion = dwOrigMotion;		// +100 하지 않은 값을 저장하자.
	m_dwMotionOption = dwOption;

	pModel->m_bEndFrame = FALSE;
	pModel->SetLoop( nLoop );
	if( dwOption & MOP_NO_TRANS )		pModel->SetMotionBlending( FALSE );
	else								pModel->SetMotionBlending( TRUE );
	
	if( dwOption & MOP_SWDFORCE )
	{
		if( IsPlayer() )
		{
			int nOption = 0;
			DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			if( IsActiveMover() )
			{
				CItemElem *pItemElem = GetWeaponItem();
				if( pItemElem )
					nOption = pItemElem->GetAbilityOption();
			} else
			{	// 타 플레이어는 페이크아이템이므로 여기서에서 값을 빼온다.
#ifdef __XOPT0617
 #ifdef __CLIENT
				nOption = m_nWeaponOption & 0xFF;	// 하위값만 쓴다.
 #endif		// __CLIENT
#else	// __XOPT0617
	#ifdef __FINITEITEM0705
				nOption		= m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF;
	#else	// __FINITEITEM0705
				nOption = m_anEquipOption[ PARTS_RWEAPON ] & 0xFF;
	#endif	// __FINITEITEM0705
#endif	// __XOPT0617
			}
			if( nOption == 10 )
				dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
			else if( nOption == 9 )
				dwColor = D3DCOLOR_ARGB( 255, 255, 150, 0 );
			else if( nOption >= 7 )
				dwColor = D3DCOLOR_ARGB( 255, 198, 0, 255 );
			else if( nOption >= 5 )
				dwColor = D3DCOLOR_ARGB( 255, 123, 82, 255 );
			else if( nOption >= 3)
				dwColor = D3DCOLOR_ARGB( 255, 129, 221, 251 );
			else if( nOption >= 1 )
				dwColor = D3DCOLOR_ARGB( 255, 201, 251, 190 );
			else
				dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );

#ifndef __BEAST
			pModel->MakeSWDForce( PARTS_RWEAPON, pItemProp->dwItemKind3, (dwOption & MOP_HITSLOW) ? TRUE : FALSE, dwColor, m_fAniSpeed );
			pModel->MakeSWDForce( PARTS_LWEAPON, pItemProp->dwItemKind3, (dwOption & MOP_HITSLOW) ? TRUE : FALSE, dwColor, m_fAniSpeed );
#endif // not beast
		}
	}
	else
	{
		if( pModel->m_pForce )
			pModel->m_pForce->Clear();
		if( pModel->m_pForce2 )
			pModel->m_pForce2->Clear();
	}
#endif // __BEAST
	return TRUE;
}
BOOL CMover::InitMotion(DWORD dwMotion)
{
	prj.m_modelMng.LoadMotion( m_pModel, m_dwType, m_dwIndex, dwMotion );
	m_dwMotion = dwMotion;
	return TRUE;
}

#ifndef __BEAST

//적의 숫자를 구한다.
int CMover::GetEnemyCount()
{
	int nCount = 0;
	m_LockofidEnemies.Enter( theLineFile );	// lock1
	nCount = m_idEnemies.size();
	m_LockofidEnemies.Leave( theLineFile );	// unlock1

	return nCount;
}

OBJID CMover::GetMaxEnemyHitID()
{
	OBJID objId = NULL_ID;
	int	nHitPoint = 0;

	m_LockofidEnemies.Enter( theLineFile );
	for( SET_OBJID::iterator i = m_idEnemies.begin(); i != m_idEnemies.end(); ++i )
	{
		HIT_INFO info = (*i).second;
		if( nHitPoint < info.nHit )
		{
			objId = (*i).first;
			nHitPoint = info.nHit;
		}
	}
	m_LockofidEnemies.Leave( theLineFile );
	return objId;
}

int CMover::GetEnemyHit( OBJID objid, DWORD* pdwTick )
{
	int nHit = 0;
	DWORD dwTick = 0;

	m_LockofidEnemies.Enter( theLineFile );	// lock1
	SET_OBJID::iterator i	= m_idEnemies.find( objid );
	if( i != m_idEnemies.end() )
	{
		HIT_INFO info = (*i).second;
		nHit = info.nHit;
		dwTick = info.dwTick;
	}

	m_LockofidEnemies.Leave( theLineFile );	// unlock1

	if( pdwTick )
		*pdwTick = dwTick;

	return nHit;
}

// 마지막으로 때린 시각을 리턴한다.
DWORD CMover::AddEnemy( OBJID objid, int nHit )
{
	DWORD dwLast = 0;
	m_LockofidEnemies.Enter( theLineFile );	// lock1

	SET_OBJID::iterator i	= m_idEnemies.find( objid );
	if( i == m_idEnemies.end() )
	{
		HIT_INFO info;
		info.nHit = nHit;
		info.dwTick = GetTickCount();
		m_idEnemies.insert( make_pair(objid, info) );
	}
	else
	{
		HIT_INFO& info = (*i).second;
		dwLast = info.dwTick;
		info.dwTick = GetTickCount();
		info.nHit += nHit;
	}
	
	m_LockofidEnemies.Leave( theLineFile );	// unlock1

	return dwLast;
}

void CMover::RemoveEnemy( OBJID objid )
{
//	CMclAutoLock	Lock( m_LockofidEnemies );
	m_LockofidEnemies.Enter( theLineFile );	// lock1

	SET_OBJID::iterator i	= m_idEnemies.find( objid );
	if( i != m_idEnemies.end() )
	{
		if( m_pActMover->GetObjHit() == objid )
			m_pActMover->SetObjHit( NULL_ID );

	#ifdef __POSTAIMSG1008
		PostAIMsg( AIMSG_DSTDIE, objid );
	#else	// __POSTAIMSG1008
		SendAIMsg( AIMSG_DSTDIE, objid );
	#endif	// __POSTAIMSG1008

		if( GetDestId() == objid )
		{
			ClearDestObj();
			SendActMsg( OBJMSG_STAND );
		}

	#ifdef __WORLDSERVER
		// PK세션을 종료시키기 위해서 플레이어간의 적대관계 종료를 보낸다.
		if( IsPlayer() )	
		{
			CUser* pEnemy = prj.GetUser( objid );
			if( IsValidObj( pEnemy ) && (GetTickCount() - i->second.dwTick) <= MIN(1) )
			{
				((CUser*)this)->UpdatePlayerEnemy( DEL_PLAYER_ENEMY, objid );
				pEnemy->UpdatePlayerEnemy( DEL_PLAYER_ENEMY, GetId() );
			}
		}	
	#endif // __WORLDSERVER

		m_idEnemies.erase( i );
	}

	m_LockofidEnemies.Leave( theLineFile );	// unlock1
}

void CMover::RemoveAllEnemies( void )
{
	int nSizeofEnemies	= 0;
	OBJID	idEnemies[1024];

	m_LockofidEnemies.Enter( theLineFile );
	for( SET_OBJID::iterator i = m_idEnemies.begin(); i != m_idEnemies.end(); ++i )
	{
		idEnemies[nSizeofEnemies++]	= (*i).first;
		if( nSizeofEnemies >= 1024 )
			break;
	}

	m_idEnemies.clear();
	m_LockofidEnemies.Leave( theLineFile );

	CMover* pEnemy;
	for( int j = 0; j < nSizeofEnemies; j++ )
	{
		pEnemy	= prj.GetMover( idEnemies[j] );
		if( IsValidObj( (CObj*)pEnemy ) )
			pEnemy->RemoveEnemy( GetId() );
	}
}

//
// this를 죽게 만드는 명령. - 외부에서 직접 호출해서 죽이기만 할때도 쓸수있다.
// Attacker는 NULL일 수도 있다.  어느지역가면 걍 뒤지는 경우.
int CMover::DoDie( CCtrl *pAttackCtrl, DWORD dwMsg )
{
#ifdef __WORLDSERVER
	if( IsPlayer() && m_nDead )
	{
		Message( "CMover::DoDie : %s 죽은 후 5초이내 또죽었다. 두번죽은듯?", GetName() );
		return 0;
	}

	// 클라이언트는 이쪽으로 오지 않음.
	BOOL	bBehavior = FALSE;
	CMover *pAttacker = NULL;

	if( pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER )		// 어태커가 무버라면 무버 포인터 만들어 둔다.
		pAttacker = (CMover *)pAttackCtrl;

	m_bLastPK = FALSE;
	if( pAttacker && pAttacker != this && pAttacker->IsPlayer() )		// 자살한게 아니고, 플레이어에게 죽었냐 아니냐.
	{
#ifdef __NPP_050308
		PVP_MODE mode = GetPVPCase( pAttacker ); 
		if( mode == PVP_MODE_PK )	// PK경우

		{
			if( g_eLocal.GetState( EVE_PKCOST ) )
				m_bLastPK = FALSE;	// 경험치 다운 
			else
				m_bLastPK = TRUE;	// 경험치 다운 없음 						
		}
		else
			m_bLastPK = TRUE;	// 경험치 다운 없음 
		
#ifdef __GUILDCOMBAT
		m_bGuildCombat = FALSE;
		if( GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
			m_bGuildCombat = TRUE;
#endif // __GUILDCOMBAT
#else
		m_bLastPK = TRUE;		// 경험치 다운 없음 							
#endif
	}

	m_nAtkCnt = 0;		// 죽고 난 후에는 어택카운트를 클리어 해서 빗자루를 탈 수 있게하자.
	
	if( IsPlayer() )
	{
#ifdef __S0504_FLY
		if( IsStateMode( STATE_BASEMOTION_MODE ) )
		{
			SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF );
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
		}
#endif // __S0504_FLY

		SetStun( FALSE );		// 플레이어만 부활후에 스턴 안걸려 있도록 풀어줌.
		SetPoison( FALSE );
		SetDark( FALSE );
		SetBleeding( FALSE );
	}
	
	BOOL fValid	= IsValidObj( this );
	
	if( IsPlayer() && m_pActMover->IsFly() )	// 날고있는 중에 죽었으면

		UnequipRide();
	
	SendActMsg( OBJMSG_STOP );
	SendActMsg( OBJMSG_DIE, dwMsg, (int)pAttacker );
	
	// 플레이어인 내가 죽었으면 내가 타겟으로 잡고있던넘을 풀어줌. 로그아웃 같은거 할때도 마찬가지 처리를 해야함.
	if( IsPlayer() )
	{
		CMover *pLastHit = prj.GetMover( m_idLastHitMover );
		if( IsValidObj( pLastHit ) )
		{
			if( pLastHit->m_idAttacker == GetId() )	// pLastHit의 공격자가 나로 설정되어 있을때만 풀어줌,.
				pLastHit->m_idAttacker = NULL_ID;	
			if( pLastHit->m_idTargeter == GetId() )
				pLastHit->m_idTargeter = NULL_ID;
		}

		// 유저가 죽었을때만 로그 남김

		if( fValid )
		{
			g_dpDBClient.SendLogPlayDeath( this, pAttacker );
		}
		// 듀얼중 플레이어의 경우 
		// 리더인지 판단해서 듀얼을 취소시키고 
		// 양 파티원들에게 결과를 통보함.
	}
	
	if( bBehavior ) {
		g_UserMng.AddMoverBehavior2
			( this, GetPos(), m_pActMover->m_vDelta, GetAngle(), GetAngleX(), m_pActMover->m_fAccPower, m_fTurnAngle, 
			m_pActMover->GetState(), m_pActMover->GetStateFlag(), m_dwMotion, m_pActMover->m_nMotionEx, m_pModel->m_nLoop, m_dwMotionOption, 
			g_TickCount.GetTickCount(), TRUE );
	}
	if( fValid )
	{
//		g_DPCoreClient.SendMoverDeath( this, pAttacker, dwMsg );
		g_UserMng.AddMoverDeath( this, pAttacker, dwMsg );
		
		RemoveAllEnemies();		// pAttacker->SendAIMsg( AIMSG_DSTDIE, GetId() );
	}	// fValid
	
	if( pAttacker )
		pAttacker->m_pActMover->SetObjHit( NULL_ID );
	
	// 타겟을 죽인후 AI에서 타겟을 클리어함.
	if( IsValidObj(pAttacker) && pAttacker->IsNPC() )
	{
		pAttacker->SendAIMsg( AIMSG_INIT_TARGETCLEAR );		// 공격자는 타겟을 클리어하고 대기모드로 돌아감.
	}
#ifdef __V050322_CACHEQUEST 
	// 몬스터를 죽이면 몬스터가 퀘스트가 요구하는 것인지 판단해서 킬 카운트 증가 
	if( IsPlayer() == FALSE && pAttacker && pAttacker->IsPlayer() )
	{
		for( int i = 0; i < pAttacker->m_nQuestSize; i++ )
		{
			LPQUEST lpQuest = (LPQUEST) &pAttacker->m_aQuest[ i ]; 
			if( lpQuest )
			{
				QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( pQuestProp )
				{
					for( int i = 0; i < 2; i++ )
					{
						// 진행 중인 퀘스트의 종료 조건과 같은 NPC인가?
						if( pQuestProp->m_nEndCondKillNPCIdx[ i ] == GetIndex() )
						{
							if( lpQuest->m_nKillNPCNum[ i ]  < pQuestProp->m_nEndCondKillNPCNum[ i ] )
							{
								lpQuest->m_nKillNPCNum[ i ]++; // 그렇다면 킬 넘버 
								((CUser*)pAttacker)->AddSetQuest( lpQuest ); 
							}
						}
					}
				}
				else
				{
//					Error( "DoDie : 죽인놈 퀘스트 카운트에서 pQuestProp NULL이다." );
				//	CString string;
				//	string.Format( "CMover::DoDie : 죽인놈 퀘스트 카운트에서 %s의 Quest %d의 pQuestProp NULL이다.", m_szName, lpQuest->m_wId );
				//	OutputDebugString( string );
				}
			}
			else
			{
//				Error( "DoDie : 죽인놈 퀘스트 카운트에서 lpQuest가 NULL이다." );
				//CString string;
				//string.Format( "CMover::DoDie : 죽인놈 퀘스트 카운트에서 %s의 pQuest가 NULL이다.", m_szName );
				//OutputDebugString( string );
			}
		}
	}
#endif // __V050322_CACHEQUEST

#ifdef __GUILDCOMBAT
	if( m_nGuildCombatState )	// 길드전쟁중이면

	{
		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
		g_GuildCombatMng.OutWar( (CUser*)this );
		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	}
#endif // __GUILDCOMBAT
	
#else // WORLDSERVER
 #ifndef __ONLINE
	CMover *pAttacker = NULL;
	if( pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER )		// 어태커가 무버라면 무버 포인터 만들어 둔다.
		pAttacker = (CMover *)pAttackCtrl;
	SendActMsg( OBJMSG_DIE, dwMsg, (int)pAttacker );
 #endif
#endif // not WORLDSERVER


#if defined(__WORLDSERVER)
	if( GetType() == OT_MOVER )
	{
		m_SkillState.RemoveAllSkillInfluence();
		g_UserMng.AddRemoveAllSkillInfluence( this );
	}
#endif //__WORLDSERVER	

	return 1;
}


#endif	//	__BEAST
/*
BOOL CMover::IsAbnormalInventory( void )
{
	DWORD dwId;
	int nMax	= m_Inventory.m_dwItemMax;
	int nNumberofIndex	= m_Inventory.m_dwIndexNum;

	for( int i = 0; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
		{
			if( m_Inventory.m_apItem[dwId].m_dwObjIndex != i )
				return TRUE;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset( adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS );
	for( i = nNumberofIndex; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
			adwEquipment[i - nNumberofIndex]	= dwId;
	}
	for( i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( adwEquipment[i] != m_adwEquipment[i] )
			return TRUE;
	}

	return FALSE;
}

BOOL CMover::NormalizeInventory( void )
{
	int nMax	= m_Inventory.m_dwItemMax;
	int nNumberofIndex	= m_Inventory.m_dwIndexNum;
	DWORD dwId;
	for( int i = 0; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
		{
			if( m_Inventory.m_apItem[dwId].m_dwObjIndex != i )
			{

				DWORD dwIndex	= NULL_ID;
				for( int j = 0; j < nMax; j++ )
				{
					if( m_Inventory.m_apItem[j].m_dwObjIndex == i )
					{
						dwIndex	= j;
						break;
					}
				}
				if( dwIndex != NULL_ID )
				{
					m_Inventory.m_apIndex[i]	= dwIndex;
				}
				else
				{
					if( i >= nNumberofIndex )
					{
						m_Inventory.m_apIndex[i]	= NULL_ID;
					}
					else
					{
						for( j = 0; j < nMax; j++ )
						{
							if( m_Inventory.m_apItem[j].m_dwObjIndex == NULL_ID )
							{
								m_Inventory.m_apItem[j].m_dwObjIndex	= i;
								m_Inventory.m_apIndex[i]	= j;
								break;
							}
						}
						if( j == nMax ) {
							ASSERT( 0 );
							return FALSE;
						}
					}
				}

			}
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset( adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS );
	for( i = nNumberofIndex; i < nMax; i++ )
	{
		dwId	= m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
			adwEquipment[i - nNumberofIndex]	= dwId;
	}
	for( i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( m_adwEquipment[i] != adwEquipment[i] )
			m_adwEquipment[i]	= adwEquipment[i];			
	}
	return TRUE;
}
*/


#ifdef __WORLDSERVER


int CMover::ChangeSlaughter( CHANGE_SLAUGHTER_TYPE type, CMover* pDefender, int nSetCarmaPoint )
{
	int nVal = 0;
	switch( type )
	{
	case CHANGE_SLAUGHTER_KILL:		// PK에 의한 Slaughter 변경 
		nVal = IncSlaughterPoint( pDefender );	
		break;
	case CHANGE_SLAUGHTER_ATTACK:	// PK시도에 의한 Slaughter 변경 
		nVal = IncSlaughterPoint2( pDefender );	
		break;
	case CHANGE_SLAUGHTER_RECOVERY:	
		if( m_nSlaughter < 0 )
		{
			m_nSlaughter += 1;
			nVal = 1;
		}
		break;
	case CHANGE_SLAUGHTER_SET:
		{
			nVal = nSetCarmaPoint - m_nSlaughter;
			m_nSlaughter = nSetCarmaPoint;
		}
		break;
	}

	if( nVal > 0 )
		((CUser*)this)->AddDefinedText( TID_GAME_GETPKPOINT, "%d", nVal );	// xxx 슬로터 포인트를 얻었다
	else if( nVal < 0 )
		((CUser*)this)->AddDefinedText( TID_GAME_DECPKPOINT, "%d", -nVal );	// xxx 슬로터 포인트가 감소했다.

	if( nVal )
	{
		// 슬로터 포인트의 변화가 있었으면 갱신시킴.
		g_UserMng.AddSetSlaughterPoint( this, m_nSlaughter, m_nNumKill );	// 어태커 주위 클라에 슬로터포인트 증가를 알림.
		if( pDefender )  // 전투에 의함 
			g_dpDBClient.SendLogPkPvp( this, pDefender, nVal, 'P' );
	}
	
	return nVal;
}

// 카르마 등급을 한단계 올린다.
void CMover::UpgradeKarma()
{
#ifdef __NPP_050308 
	KarmaProp* pProp = prj.GetKarmaProp( m_nSlaughter );
	int nGrade = pProp->nGrade + 1;
	pProp = prj.GetKarmaPropByGrade( nGrade ); 
	ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, pProp->nKarmaPoint );
#endif // __NPP_050308 
}

int	CMover::SubPK( CMover *pAttacker )
{
	if( !g_eLocal.GetState( EVE_PK ) )
		return 1;

	BOOL bChaotic = IsChaotic();

	m_idMurderer = pAttacker->m_idPlayer;	// 날 마지막으로 죽인놈의 플레이어아이디를 저장해둠.
	m_tmDead = timeGetTime();				// PK당하면 죽은 시간기록.
	pAttacker->m_nNumKill++;				// 킬수 증가
	((CUser*)this)->AddDefinedText( TID_GAME_PKDEAD, "\"%s\"", pAttacker->m_szName );	// ??에게 죽음을 당하였습니다.

	#ifdef __NPP_050308 
		int nGap = abs( this->GetLevel() - pAttacker->GetLevel() );
		if( nGap >= 80 )
			ChangeFame( this );	// 80 이상일 차이 날 경우, 예외적으로 명성치 증감  

		// 보상금 떨구기를 요청한다.
		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			//"%s님이 현상범 %s를 잡아 현상금 %s페냐를 얻었습니다."
			char szFormat[256];
			strcpy( szFormat, pAttacker->GetName() );
			strcat( szFormat, prj.GetText( TID_PK_REWARDNOTICE ) );
			g_DPCoreClient.SendWCWantedReward( m_idPlayer, pAttacker->m_idPlayer, szFormat, pWorld->GetID(), GetPos() );
		}

		if( g_eLocal.GetState( EVE_PKCOST ) )
		{
			if( bChaotic )		
			{
			#if !defined(__INTERNALSERVER)
				if( m_dwAuthorization < AUTH_HELPER )
			#endif
				{
					// 페냐드롭 
					KarmaProp* pProp = prj.GetKarmaProp( m_nSlaughter );
					float fRate = pProp->nDropGoldPercent / 100.0f;
					int nGold = GetGold() * fRate;
				
					if( nGold > 0 )
						DropGold( nGold , GetPos(), TRUE );
						
					// 아이템 드롭 
					for( int i=0; i<pProp->nDropItem; ++i )
					{
						BOOL bExcludeEquip = ( i == 0 );	// 1개 이하의 경우, 장착 아이템은 떨구지 않는다.

						if( xRandom( 100 ) < pProp->nDropPercent )	
						{
							if( DoDropItemRandom( bExcludeEquip, pAttacker ) == 0 )	
								break;
						}
					}
				}
			}				
		}

		//pAttacker의 slaughter값을 변경시킨다.
		pAttacker->ChangeSlaughter( CHANGE_SLAUGHTER_KILL, this );
		// 준카오 유저는 PK로 사망 당할 경우, 카르마 등급이 한 단계 올라간다.
		if( GetSlaughterGrade() == SLAUGHTER_SEMI_CHAOTIC )
			UpgradeKarma();

	#else // !__NPP_050308 
		if( bChaotic )		// 빨갱이인 내가 죽었을때.
		{		
			if( pAttacker->IsChaotic() == FALSE )
				pAttacker->m_nFame += 50;				// 죽인사람의 명성치 상승.

			int nVal = (int)(m_nSlaughter * 0.03f);		// 빨갱이가 사람에게 죽으면 슬로터포인트 깎임.
			m_nSlaughter -= nVal;
			((CUser*)this)->AddDefinedText( TID_GAME_DECPKPOINT, "%d", nVal );	// xxx 슬로터 포인트가 감소했다.
			g_UserMng.AddSetSlaughterPoint( this, m_nSlaughter, m_nNumKill );  // this 주위 클라에 슬로터포인트 변화를 알림.
		} 
		else				// 착한 내가 죽었을때.
		{		
			pAttacker->m_nFame -= 50;
			if( pAttacker->m_nFame < 0 )	
				pAttacker->m_nFame = 0;
		}
			
		if( g_eLocal.GetState( EVE_PKCOST ) )
		{
			if( bChaotic )		
				DoDropItemRandom( TRUE, pAttacker );		// TRUE - 장착아이템은 떨구지 않는다.
		}
		
	#endif // !__NPP_050308 
	return 1;
}

int CMover::SubDuel( CMover *pAttacker )
{
	if( m_nDuel )
		pAttacker->DuelResult( this );	// 듀얼후 결과 처리.

	return 1;
}

int CMover::SubWar( CMover *pAttacker )
{
#ifdef __WORLDSERVER
	// 길드전 가능한 서버인가.
	if( g_eLocal.GetState( EVE_GUILDWAR ) == 0 )	
		return 0;

	// 나랑 다른길드인가(동맹인경우는 이걸로 안된다.)
	if( m_idGuild == pAttacker->m_idGuild )
		return 0;

	// 적대길드원에게 죽었다.
	g_DPCoreClient.SendWarDead( m_idPlayer );
#endif	// __WORLDSERVER
	return 1;
}

PVP_MODE CMover::GetPVPCase( CMover *pAttacker )
{
	BOOL bPKEnable = FALSE;
#ifndef __JAPAN_PVP
	if( g_eLocal.GetState( EVE_18 ) == 1 )			// 18? ???? PK
#endif // __JAPAN_PVP	
		bPKEnable = TRUE;

	if( bPKEnable == FALSE || 
		pAttacker == NULL  || 
		pAttacker->IsNPC() || // 죽인놈이 NPC면 걍 리턴.
		IsNPC() )			  // 죽은놈이 NPC면 걍리턴
		return PVP_MODE_NONE;

	// 이건 사람대 사람의 싸움이다!
	if( m_idWar && pAttacker->m_idWar == m_idWar )	// 나랑 같은 전쟁에 참가중인 놈인가.
	{
		return PVP_MODE_GUILDWAR;
	}
				
	if( m_nDuel == 0 || m_nDuelState == 300 )	// PK
	{
		if( GetTickCount() < m_dwPKTargetLimit )		// PK선공불가시간동안에는 선공이 안됨  
		{
			return PVP_MODE_NONE;
		}
		return PVP_MODE_PK;
	} 
	else				//듀얼 상황.	
	{
		return PVP_MODE_DUEL; 
	}
}

// this가 죽고 난후 PVP(PK, 듀얼, 길드전)처리
void CMover::SubPVP( CMover *pAttacker )
{
	if( m_nDuel == 1 )
	{
		if( GetId() != pAttacker->m_idDuelOther )
		{
#ifdef __NPP_050308
			CUser* pOther	= prj.GetUser( m_idDuelOther );
			if( IsValidObj( (CObj*)pOther ) )
			{
				pOther->AddDuelCancel( pOther->m_idDuelOther );
				pOther->ClearDuel();
			}
			( (CUser*)this )->AddDuelCancel( m_idDuelOther );
			ClearDuel();
#endif	// __NPP_050308
			return;
		}
	}
	else if( m_nDuel == 2 )
	{
#ifdef __NPP_050308
		if( m_idDuelParty != pAttacker->m_idparty || pAttacker->m_nDuelState == 300 )
		{
			g_PartyMng.Lock( theLineFile );
			CParty *pLoseParty = g_PartyMng.GetParty( m_idparty );

			if( pLoseParty && pLoseParty->IsLeader( m_idPlayer ) )
			{
				g_DPCoreClient.SendSetPartyDuel( m_idDuelParty, pLoseParty->m_uPartyId, FALSE );

				CParty* pParty2	= g_PartyMng.GetParty( m_idDuelParty );
				if( pParty2 )
					pParty2->DoDuelPartyCancel( pLoseParty );
				pLoseParty->DoDuelPartyCancel( pParty2 );
			}
			g_PartyMng.Unlock( theLineFile );
		}
#else // __NPP_050308
		if( m_idDuelParty != pAttacker->m_idparty )
			return;
#endif	// __NPP_050308
	}

	PVP_MODE mode = GetPVPCase( pAttacker );
	switch( mode )
	{
	case PVP_MODE_GUILDWAR:
		SubWar( pAttacker );
		break;
	case PVP_MODE_PK:
		SubPK( pAttacker );		// PK의 서브루틴.
		break;
	case PVP_MODE_DUEL:
		SubDuel( pAttacker );
		break;
	}
}

void CMover::AddPartyMemberExperience( CUser * pUser, EXPINTEGER nExp, int nFxp )
{
	if( nFxp )	// 비행경험치가 0인경우는 아무 처리도 안해도 된다.
	{
		if( pUser->AddFxp( nFxp ) )
		{
			// 비행레벨업!
			g_UserMng.AddSetFlightLevel( pUser, pUser->m_nFlightLv );
		} else
		{
			// 레벨업하지 않고 경험치만 올라감.
			// 로그 남길것.
		}
		pUser->AddSetFxp( pUser->m_nFxp, pUser->m_nFlightLv );		// 당사자에게 비행경험치/레벨을 보냄.
	}

	if( nExp > prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp )
		nExp	= prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp;

	if( pUser->AddExperience( nExp ) )
	{
		g_UserMng.AddSetLevel( pUser, (WORD)pUser->m_nLevel );

		pUser->AddSetGrowthLearningPoint( pUser->m_nRemainGP, pUser->m_nRemainLP );
		g_dpDBClient.SendLogLevelUp( pUser, 1 );	// 레벨업 로그
		g_DPCoreClient.SendPartyMemberLevel( pUser );
		if( pUser->m_idGuild != 0 )
			g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)pUser );
		
		
	}
	else
	{
		// 레벨 5이상 로그_레벨업 테이블에 로그를 남긴다
		// 20% 단위로 로그를 남김

		if( pUser->GetLevel() > 5 ) // 레벨 5이상

		{
			int iLogExp = pUser->GetExp1() * 100 / pUser->GetMaxExp1();
			iLogExp /= 20;

			if( ( 20 * ( iLogExp + 1 ) ) <= ( pUser->GetExp1() * 100 / pUser->GetMaxExp1() ) )
				g_dpDBClient.SendLogLevelUp( pUser, 5 );
		}
	}
	pUser->AddSetExperience( pUser->GetExp1(), pUser->GetExp2(), (WORD)pUser->m_nLevel );
//	g_DPCoreClient.SendSetExperience( (CMover*)pUser, (DWORD)pUser->m_nExp1, (DWORD)pUser->m_nExp2, (WORD)pUser->m_nLevel );
}

// this가 죽은무버다.
// this를 중심으로 반경 fRange 이내에 있는 사람들에게 경험치를 분배한다.
void CMover::SubAroundExp( CMover *pAttacker, float fRange )
{
#if 1
#define MAX_AROUND_USER		512

	int		nMax;		// 주위에 있는 사람 수.
	D3DXVECTOR3	vPos = GetPos();
	D3DXVECTOR3 vDist;
	FLOAT		fDistSq;
	CUser *pUser;
	CUser *pList[ MAX_AROUND_USER ], **ptr;
	int		nMaxList = 0;
	int		i;

	TRACE( "보스몹 죽임 %s\n", pAttacker->GetName() );
	fRange *= fRange;		// Sq 버전으로 바꿈.

	memset( pList, 0, sizeof(pList) );
	ptr = pList;

	// 반경내에 있는 유저를 추려냄.
	FOR_VISIBILITYRANGE( this )
	{
		pUser = USERPTR;
		vDist = vPos - pUser->GetPos();
		fDistSq = D3DXVec3LengthSq( &vDist );		// 죽은넘과 유저사이의 거리Sq를 구함 
		if( fDistSq <= fRange )				// 반경 fRange 미터 이내에 있는 사람은
		{
			*ptr++ = pUser;
			nMaxList ++;
		}
	}	
	NEXT_VISIBILITYRANGE( this )

	if( nMaxList == 0 )
	{
		Message( "CMover::SubAroundExp %s주위에 유저가 없다. ", GetName() );
		return;
	}


	EXPINTEGER nExp		= GetProp()->nExpValue / (EXPINTEGER)nMaxList;	// 1인당 얼마씩 배분되야 하는가.
	// 추려낸 유저들에게 경험치를 줌.
	ptr = pList;
	for( i = 0; i < nMaxList; i ++ )
	{
		pUser = *ptr++;

		if( nExp > prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp )
			nExp	= prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp;

		if( pUser->AddExperience( nExp ) )
		{
			// 레벨업 됐다.
			g_UserMng.AddSetLevel( pUser, (WORD)pUser->m_nLevel );		// pUser의 주위사람에게 pUser가 레벨이 올랐다는걸 보냄.
//			g_DPCoreClient.SendSetGrowthLearningPoint( pUser, pUser->m_nRemainGP, pUser->m_nRemainLP );
			((CUser*)pUser)->AddSetGrowthLearningPoint( pUser->m_nRemainGP, pUser->m_nRemainLP );		// pUser에게 GP, LP 변동된것을 보냄.
			g_dpDBClient.SendLogLevelUp( pUser, 1 );	// 레벨업 로그
		}
		else
		{
			// 레벨업 안되고 겸치만 올랐다.
			// 레벨 5이상일때는 경험치 업을 로그_레벨업 테이블에 로그를 남긴다
			// 경험치 20% 단위로 로그를 남김

			int iLogExp = GetExp1() * 100 / GetMaxExp1();
			iLogExp /= 20;
			if( pUser->GetLevel() > 5 ) // 레벨 5이상

			{
				if( ( 20 * ( iLogExp + 1 ) ) <= ( pUser->GetExp1() * 100 / pUser->GetMaxExp1() ) )
					g_dpDBClient.SendLogLevelUp( pUser, 5 );
			}
		}
		// pUser에게 경험치 바뀐걸 보냄
		((CUser*)pUser)->AddSetExperience( pUser->GetExp1(), pUser->GetExp2(), (WORD)pUser->m_nLevel );
//		g_DPCoreClient.SendSetExperience( pUser, (DWORD)pUser->m_nExp1, (DWORD)pUser->m_nExp2, (WORD)pUser->m_nLevel );
	}
			
		
#endif // 0
}

#ifdef __S_DST_ADD
void CMover::AddKillRecovery( void )
{
	if( IsPlayer() == FALSE )	return; // this(Attacker)가 몬스터면 처리안함.
	
	int nHPPoint = GetParam( DST_KILL_HP, 0 );
	int nMPPoint = GetParam( DST_KILL_MP, 0 );
	int nFPPoint = GetParam( DST_KILL_FP, 0 );
	
	BOOL bSfx = FALSE;
	if( nHPPoint )
	{
		int nHP	   = GetPointParam( DST_HP );
		int nHPMax = GetMaxPoint( DST_HP );
		if( nHP + nHPPoint >= nHPMax )	// 포인트 더했을때 오바되는걸 처리.
			nHP = nHPMax;
		else
			nHP = nHP + nHPPoint;
		SetPointParam( DST_HP, nHP );
		bSfx = TRUE;
	}
	
	if( nMPPoint )
	{
		int nMP	   = GetPointParam( DST_MP );
		int nMPMax = GetMaxPoint( DST_MP );
		if( nMP + nMPPoint >= nMPMax )	// 포인트 더했을때 오바되는걸 처리.
			nMP = nMPMax;
		else
			nMP = nMP + nMPPoint;
		SetPointParam( DST_MP, nMP );
		bSfx = TRUE;
	}
	
	if( nFPPoint )
	{
		int nFP	   = GetPointParam( DST_FP );
		int nFPMax = GetMaxPoint( DST_FP );
		if( nFP + nFPPoint >= nFPMax )	// 포인트 더했을때 오바되는걸 처리.
			nFP = nFPMax;
		else
			nFP = nFP + nFPPoint;
		SetPointParam( DST_FP, nFP );
		bSfx = TRUE;
	}
	
	if( bSfx )
		g_UserMng.AddCreateSfxObj( this, XI_KILL_RECOVERY );
}
#endif // __S_DST_ADD

// pDead를 죽인후 경험치 처리
int CMover::SubExperience( CMover *pDead )
{
	if( IsPlayer() == FALSE )	return 0;	// this(Attacker)가 몬스터면 처리안함.
	if( pDead->IsPlayer() )	return 0;			// 죽은넘이 플레이어면 경험치 처리할일이 없음.
	
	MoverProp* pMoverProp	= pDead->GetProp();
	ASSERT( pMoverProp );

#ifdef __WORLDSERVER
	EXPFLOAT fExpValue = 0;
	float	fFxpValue = 0.0f;
	#ifdef __S1108_BACK_END_SYSTEM
			fExpValue	= pMoverProp->nExpValue * static_cast<EXPFLOAT>( prj.m_fMonsterExpRate ) * static_cast<EXPFLOAT>( pMoverProp->m_fExp_Rate );
			fFxpValue	= pMoverProp->nFxpValue * prj.m_fMonsterExpRate * pMoverProp->m_fExp_Rate;
	#else // __S1108_BACK_END_SYSTEM
			fExpValue	= pMoverProp->nExpValue * static_cast<EXPFLOAT>( prj.m_fMonsterExpRate );
 			fFxpValue	= pMoverProp->nFxpValue * prj.m_fMonsterExpRate;
	#endif // __S1108_BACK_END_SYSTEM
#else	// __WORLDSERVER
	EXPFLOAT fExpValue	= static_cast<EXPFLOAT>( pMoverProp->nExpValue );
	float	fFxpValue	= pMoverProp->nFxpValue;
#endif	// __WORLDSERVER
	

#ifndef __NPP_050308
	fExpValue	*= static_cast<EXPFLOAT>( pDead->m_fExpRatio );		// HP가 50%남은놈은 경험치 50%만 줘야 한다. 그것에 대한 처리.
	fFxpValue  *= pDead->m_fExpRatio;	// 비행경험치도 똑같이.
#endif // __NPP_050308

#ifdef __NPP_050308
	AddExperienceKillMember( pDead, fExpValue, pMoverProp, fFxpValue );
#else // __NPP_050308
	g_PartyMng.Lock( theLineFile );
	CParty* pParty = g_PartyMng.GetParty( m_idparty );
	if( pParty && pParty->IsMember( m_idPlayer ) )
	{	// 극단 경험치 분배

		int nTotalLevel		= 0;
		int nMaxLevel10		= 0;
		int nMaxLevel		= 0;
		int nMemberSize		= 0;
		CUser* apMember[MAX_PTMEMBER_SIZE];
		memset( apMember, 0, sizeof(apMember) );

		// 1. 주변 멤버 검사

		if( GetPartyMemberFind( pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize ) == FALSE )
		{
			g_PartyMng.Unlock( theLineFile );
			return 1;
		}

		if( 1 < nMemberSize )
		{	// 파티원가 같이 있음..
			// 2. 파티원들 경험치 주기

			AddExperienceParty( pDead, fExpValue, pMoverProp, fFxpValue, pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize );
		}
		else
		{	// 파티원들에게 왕따... (혼자 있음)
			// 혼자서 싸운것으로 처리....
			AddExperienceSolo( fExpValue, pMoverProp, fFxpValue );
		}
		g_PartyMng.Unlock( theLineFile );	// 더 이상 파티를 참조할 필요가 없다.
	}
	else
	// no party
	{
		m_idparty	= 0;
		g_PartyMng.Unlock( theLineFile );
		AddExperienceSolo( fExpValue, pMoverProp, fFxpValue );
	} // if not party
#endif // __NPP_050308
	return 1;
}

BOOL CMover::IsValidArea( CMover* pMover, float fLength )
{
	float fDist;
	D3DXVECTOR3	vDist;
	if( IsValidObj( (CObj*)this) && IsValidObj( (CObj*)pMover ) )
	{
		vDist = pMover->GetPos() - GetPos();
		fDist = D3DXVec3LengthSq( &vDist );
		if( fDist < fLength * fLength )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::GetPartyMemberFind( CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize )
{
	CUser* pUsertmp = NULL;
	float fDist;
	D3DXVECTOR3	vDist;

	for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )	
	{
		pUsertmp	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
		if( IsValidArea( (CMover*)pUsertmp, 64.0f ) )
		{
			apMember[(*nMemberSize)++]	= pUsertmp;
			(*nTotalLevel)		+= pUsertmp->GetLevel();
			if( (*nMaxLevel10) < pUsertmp->GetLevel() )
			{
				(*nMaxLevel) = (*nMaxLevel10) = pUsertmp->GetLevel();
			}			
		}
	}
	
	if( 0 < (*nMaxLevel10) - 20 )
	{
		(*nMaxLevel10) -= 20;
	}
	else
	{
		(*nMaxLevel10) = 0;
	}

	if( (*nMemberSize) == 0 || (*nTotalLevel) == 0 )
	{
		return FALSE;
	}

	return TRUE;
}

void CMover::AddExperienceKillMember( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue )
{
#ifdef __NPP_050308
/*
	int nSizeofEnemies	= 0;
	OBJID	oidEnemies[1024];
	int		nHitPoint[1024];
	DWORD	dwMaxEnemieHit = 0;
	
	// 죽인 몬스터의 경험치를 나눠먹을 사람 따로 추려냄
	pDead->m_LockofidEnemies.Enter( theLineFile );
	for( SET_OBJID::iterator i = pDead->m_idEnemies.begin(); i != pDead->m_idEnemies.end(); ++i )
	{
		oidEnemies[nSizeofEnemies]	= (*i).first;
//		nHitPoint[nSizeofEnemies]	= (*i).second;
		nHitPoint[nSizeofEnemies]	= (*i).second.nHit;
		dwMaxEnemieHit += nHitPoint[nSizeofEnemies];
		++nSizeofEnemies;
	}
	pDead->m_LockofidEnemies.Leave( theLineFile );

	g_PartyMng.Lock( theLineFile );
	OBJID	oidDeleteEnemies[1024];
	int nSizeofDeleteEnemies = 0;
	CMover* pEnemy;
	for( int j = 0 ; j < nSizeofEnemies; j++ )
	{
		BOOL bDeleteFind = FALSE;
		// 삭제되었는지 확인
		for( int k = 0 ; k < nSizeofDeleteEnemies ; k++ )
		{
			if( oidEnemies[j] == oidDeleteEnemies[k] )
			{
				bDeleteFind = TRUE;
				break;
			}
		}

		// 삭제가 안된것만 들어감
		if( bDeleteFind == FALSE )
		{
			pEnemy	= prj.GetMover( oidEnemies[j] );
			if( IsValidObj( pEnemy ) && pDead->IsValidArea( pEnemy, 64.0f ) && pEnemy->IsPlayer() )	// 플레이어, 범위 검사

			{
				CMover* pEnemyBuf;
				DWORD dwHitPointParty = 0;
				CParty* pParty	= g_PartyMng.GetParty( pEnemy->m_idparty );
				if( pParty && pParty->IsMember( pEnemy->m_idPlayer ) )
				{
					dwHitPointParty = nHitPoint[j];
					for( int l = j + 1 ; l < nSizeofEnemies ; l++ )
					{
						pEnemyBuf	= prj.GetMover( oidEnemies[l] );
						if( IsValidObj( pEnemyBuf ) && pDead->IsValidArea( pEnemyBuf, 64.0f ) && pEnemyBuf->IsPlayer() )	// 플레이어, 범위 검사

						{
							if( pEnemy->m_idparty == pEnemyBuf->m_idparty && pParty->IsMember( pEnemyBuf->m_idPlayer ) )
							{
								// 같은 파티 삭제 대상

								dwHitPointParty += nHitPoint[l];
								oidDeleteEnemies[ nSizeofDeleteEnemies++ ] = oidEnemies[ l ];
							}
						}
						else
						{
							// 삭제 대상

							oidDeleteEnemies[ nSizeofDeleteEnemies++ ] = oidEnemies[ l ];
						}
					}
				}
				if( dwHitPointParty != 0 )
					nHitPoint[j] = dwHitPointParty;
				fExpValue *= ( float( nHitPoint[j] ) / float( dwMaxEnemieHit ) );
				if( dwHitPointParty )
				{	// 극단 경험치 분배

					int nTotalLevel		= 0;
					int nMaxLevel10		= 0;
					int nMaxLevel		= 0;
					int nMemberSize		= 0;
					CUser* apMember[MAX_PTMEMBER_SIZE];
					memset( apMember, 0, sizeof(apMember) );
					
					// 1. 주변 멤버 검사

					if( pEnemy->GetPartyMemberFind( pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize ) == FALSE )
					{
						break;
					}
					
					if( 1 < nMemberSize )
					{	// 파티원가 같이 있음..
						// 2. 파티원들 경험치 주기

						pEnemy->AddExperienceParty( pDead, fExpValue, pMoverProp, fFxpValue, pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize );
					}
					else
					{	// 파티원들에게 왕따... (혼자 있음) // 혼자서 싸운것으로 처리....
						pEnemy->AddExperienceSolo( fExpValue, pMoverProp, fFxpValue );
					}
				}
				else	// no party
				{
					pEnemy->AddExperienceSolo( fExpValue, pMoverProp, fFxpValue );
				} // if not party
			}
		}
	}
	g_PartyMng.Unlock( theLineFile );
*/
	vector<OBJID>	adwEnemy;
	vector<int>		anHitPoint;
	DWORD	dwMaxEnemyHit	= 0;
	pDead->m_LockofidEnemies.Enter( theLineFile );
	for( SET_OBJID::iterator it = pDead->m_idEnemies.begin(); it != pDead->m_idEnemies.end(); ++it )
	{
		adwEnemy.push_back( (*it).first );
		anHitPoint.push_back( (*it).second.nHit );
		dwMaxEnemyHit	+= (*it).second.nHit;
	}
	pDead->m_LockofidEnemies.Leave( theLineFile );

	if( dwMaxEnemyHit == 0 )		//
		return;

	g_PartyMng.Lock( theLineFile );
	for( int j = 0; j < adwEnemy.size(); j++ )
	{
		if( adwEnemy[j] == 0 )		// 무시
			continue;
		CMover* pEnemy	= prj.GetMover( adwEnemy[j] );
		if( IsValidObj( pEnemy ) && pDead->IsValidArea( pEnemy, 64.0f ) && pEnemy->IsPlayer() )		// 플레이어, 범위 검사
		{
			DWORD dwHitPointParty	= 0;
			CParty* pParty	= g_PartyMng.GetParty( pEnemy->m_idparty );
			if( pParty && pParty->IsMember( pEnemy->m_idPlayer ) )
			{
				dwHitPointParty		= anHitPoint[j];
				for( int k = j + 1 ; k < adwEnemy.size(); k++ )
				{
					if( adwEnemy[k] == 0 )
						continue;	// 중복 처리 스킵
					CMover* pEnemy2	= prj.GetMover( adwEnemy[k] );
					if( IsValidObj( pEnemy2 ) && pDead->IsValidArea( pEnemy2, 64.0f ) && pEnemy2->IsPlayer() )	// 플레이어, 범위 검사
					{
						if( pEnemy->m_idparty == pEnemy2->m_idparty && pParty->IsMember( pEnemy2->m_idPlayer ) )	// 같은 파티 삭제 대상
						{
							dwHitPointParty		+= anHitPoint[k];
							adwEnemy[k]	= 0;	// 중복 처리 방지
						}
					}
					else
					{
						adwEnemy[k]	= 0;		// 삭제 대상
					}
				}
			}
			if( dwHitPointParty > 0 )
				anHitPoint[j]	= dwHitPointParty;
			fExpValue *= ( float( anHitPoint[j] ) / float( dwMaxEnemyHit ) );
			if( dwHitPointParty )		// 극단 경험치 분배
			{	
				int nTotalLevel		= 0;
				int nMaxLevel10	= 0;
				int nMaxLevel	= 0;
				int nMemberSize		= 0;
				CUser* apMember[MAX_PTMEMBER_SIZE];
				memset( apMember, 0, sizeof(apMember) );
				// 1. 주변 멤버 검사
				if( pEnemy->GetPartyMemberFind( pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize ) == FALSE )
					break;
				if( 1 < nMemberSize )	// 파티원가 같이 있음	// 파티원들 경험치 주기
					pEnemy->AddExperienceParty( pDead, fExpValue, pMoverProp, fFxpValue, pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize );
				else	// 혼자서 싸운것으로 처리.
					pEnemy->AddExperienceSolo( fExpValue, pMoverProp, fFxpValue );
			}
			else
			{
				pEnemy->AddExperienceSolo( fExpValue, pMoverProp, fFxpValue );
			}
		}
	}
	g_PartyMng.Unlock( theLineFile );
#endif // __NPP_050308
}

void CMover::AddExperienceSolo( EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue )
{
		// 죽음 이후 상태라면 경험치를 150%준다.
		if( IsAfterDeath() )
			fExpValue	*= static_cast<EXPFLOAT>( 1.5f );
//#ifdef __VCRITICAL
		// 크리티칼률 조정. 내 HP가 CRITICAL_BERSERK_HP%보다 적으면 적용

		int nHitPercent = GetHitPointPercent( 100 );
		if( nHitPercent < CRITICAL_BERSERK_HP )
		{
			// CRITICAL_BERSERK_HP : ( fExpValue * 0.2 ) = nHitPercent : x
			// 30 : ( 1000 * 0.2 ) = 0 : x
			if( nHitPercent <= 10 )
				fExpValue	*= 1.2f;
			else
				fExpValue	+= ( fExpValue * .2f ) - ( ( fExpValue * .2f ) * nHitPercent / CRITICAL_BERSERK_HP );
		}
//#endif
		// 레벨에 따라 경험치를 준다. 렙이 나보다 낮을경우 70  %, 나보다 높을경우 130 %
		int dw_Level = GetLevel() - (int)pMoverProp->dwLevel;
		if( dw_Level > 0 )
		{
			if( 1 == dw_Level || dw_Level == 2 )		// 1~2 차이
			{
				fExpValue	*= static_cast<EXPFLOAT>( 0.7f );
				fFxpValue	*= 0.7f;
			}
			else if( 3 == dw_Level || dw_Level == 4 )	// 3~4 차이
			{
				fExpValue	*= static_cast<EXPFLOAT>( 0.4f );
				fFxpValue	*= 0.4f;
			}
			else										// 5이상 차이
			{
				fExpValue	*= static_cast<EXPFLOAT>( 0.1f );
				fFxpValue	*= 0.1f;
			}
		}

		EXPINTEGER	iLogExp		= GetExp1() * 100 / GetMaxExp1();
		iLogExp	/= 20;

		if( fExpValue > static_cast<EXPFLOAT>( prj.m_aExpCharacter[m_nLevel].nLimitExp ) )
			fExpValue	= static_cast<EXPFLOAT>( prj.m_aExpCharacter[m_nLevel].nLimitExp );

		// 올릴 비행경험치가 없다면 실행안해도 된다.
		if( fFxpValue )
		{
			if( AddFxp( (int)fFxpValue ) )
			{
#			ifdef __WORLDSERVER
				g_UserMng.AddSetFlightLevel( this, m_nFlightLv );
//				g_dpDBClient.SendLogFlightLevelUp( this, 1 );	// 비행 레벨업 로그
#			endif	// __WORLDSERVER
			} else
			{
				// 비행경험치 획득 로그 넣을것!
			}
#		ifdef __WORLDSERVER
			( (CUser*)this )->AddSetFxp( m_nFxp, m_nFlightLv );
//			g_DPCoreClient.SendSetFxp( this, m_nFxp, m_nFlightLv );
#		endif	// __WORLDSERVER
		}
		
		TRACE("Name : %s 얻어지는 : %I64d ", this->GetName(), static_cast<EXPINTEGER>( fExpValue ) );
		TRACE(" 레벨업에 필요한:%I64d\n", prj.m_aExpCharacter[m_nLevel+1].nExp1 - m_nExp1 );

		if( AddExperience( static_cast<EXPINTEGER>( fExpValue ) ) )	// lv up
		{
#		ifdef __WORLDSERVER
			g_UserMng.AddSetLevel( this, (WORD)m_nLevel );
//			g_DPCoreClient.SendSetGrowthLearningPoint( this, m_nRemainGP, m_nRemainLP );
			( (CUser*)this )->AddSetGrowthLearningPoint( m_nRemainGP, m_nRemainLP );
			g_dpDBClient.SendLogLevelUp( this, 1 );	// 레벨업 로그
			if( m_idGuild != 0 )
				g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)this );
#		endif	// __WORLDSERVER
		}
		else
		{
			// 레벨 5이상일때는 경험치 업을 로그_레벨업 테이블에 로그를 남긴다
			// 경험치 20% 단위로 로그를 남김

			if( GetLevel() > 5 ) // 레벨 5이상

			{
#		ifdef __WORLDSERVER
				if( ( 20 * ( iLogExp + 1 ) ) <= ( GetExp1() * 100 / GetMaxExp1() ) )
					g_dpDBClient.SendLogLevelUp( this, 5 );
#		endif	// __WORLDSERVER
			}
		}
#		ifdef __WORLDSERVER
		( (CUser*)this )->AddSetExperience( GetExp1(), GetExp2(), (WORD)m_nLevel );
//		g_DPCoreClient.SendSetExperience( this, (DWORD)m_nExp1, (DWORD)m_nExp2, (WORD)m_nLevel );
#		endif	// __WORLDSERVER	
}

void CMover::AddExperienceParty( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize )
{
	// 죽이넘 주변의 파티원들중 최고 레벨 파티원으로 경험치를 감소 시킴
	AddExperienceLevelReduce( &fExpValue, pMoverProp, &fFxpValue, (*nMaxLevel) );

	// 몬스터 레벨과 극단총레벨이 5이상 몬스터레벨이 적으면 경험치와 포인트가 안올라감
	pParty->GetPoint( (*nTotalLevel), (*nMemberSize), pDead->GetLevel() );

	if( pParty->m_nKindTroup ) // 순회극단
	{
		switch( pParty->m_nTroupsShareExp ) // 경험치 분배 방식
		{
		case 1 :	// 기여분배

			{
				AddExperiencePartyContribution( pDead, apMember, pParty, fExpValue, fFxpValue, (*nMemberSize), (*nMaxLevel10) );
			}
			break;
		case 2 :	// 동일분배

			{
				// 아직 기획이 미정 적용안됨
			}
			break;
		default:	// 기본 분배(레벨분배)
			{
				AddExperiencePartyLevel( apMember, pParty, fExpValue, fFxpValue, (*nMemberSize), (*nMaxLevel10) );
			}
			break;
		}
	}
	else // 단막극단
	{
		// 단막극단은 순회극단의 레벨 분배와 같다
		AddExperiencePartyLevel( apMember, pParty, fExpValue, fFxpValue, (*nMemberSize), (*nMaxLevel10) );
	}
}

// 극단시 기여분배

void CMover::AddExperiencePartyContribution( CMover *pDead, CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 )
{
	// 획득한 경험치 = 자신의 타격에 의한 경험치 + 보너스 경험치 + 옵션 (몬스터경험치 * 파티원중 때린놈들 / 100 )
	// 보너스 경험치 = ( 몬스터 경험치 * 0.15) *( 극단 인원 -1 ) * ((자신의레벨의 제곱)/( 파티원 각각의 레벨의 제곱의 합))
	EXPINTEGER	nMemberExp;
	int nAttackMember = 0;
	float fMaxMemberLevel = 0.0f;
	for( int i = 0 ; i < nMemberSize ; i++ )
	{
		float fContribution = 0;
		int nHit = pDead->GetEnemyHit( apMember[i]->GetId() );
		if( nHit )
		{
			++nAttackMember;
		}
		fMaxMemberLevel += ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
	}
	float fAddExp = 0.0f;		// 보너스 경험치
	fAddExp = ( fExpValue * 0.2f ) * ( pParty->m_nSizeofMember - 1 );
	float fFullParty = 0.0f;	// 풀파티 보너스 경험치
	if( nMemberSize == MAX_PTMEMBER_SIZE )
		fFullParty = ( fExpValue * 0.1f );
	float fOptionExp = 0.0f;	// 옵션 경험치
	if( 1 < nAttackMember )
		fOptionExp = (fExpValue * (float)nAttackMember / 100.0f );

	TRACE("GetExp(Contribution) : %s -> %f\n ", pParty->m_sParty, fExpValue );
	
	for( i = 0 ; i < nMemberSize ; i++ )
	{
		float fContribution = 0;
		int nHit = pDead->GetEnemyHit( apMember[i]->GetId() );
		if( nHit )
		{
			fContribution = (float)nHit * 100 / (float)pDead->GetMaxHitPoint();
			if( 100 < fContribution )
			{
				fContribution = 100.0f;
			}
			if( nMaxLevel10 < apMember[i]->GetLevel() )
			{
				nMemberExp	= static_cast<EXPINTEGER>( ( fExpValue * ( fContribution / 100.0f ) ) + ( fAddExp * ( ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel()) / fMaxMemberLevel ) ) + fOptionExp + fFullParty );
				// 죽은후 경험치 더주기

				if( apMember[i]->IsAfterDeath() )
					nMemberExp	*= static_cast<EXPFLOAT>( 1.5f );
				
				// HP가 적을때 경험치 더주기

				int nHitPercent = apMember[i]->GetHitPointPercent( 100 );
				if( nHitPercent < CRITICAL_BERSERK_HP )
				{
					if( nHitPercent <= 10 )
						nMemberExp	*= 1.2f;
					else
						nMemberExp	+= ( nMemberExp * .2f ) - ( ( nMemberExp * .2f ) * nHitPercent / CRITICAL_BERSERK_HP );
				}
				TRACE("GetExp(Contribution) : %s -> %I64d\n ", apMember[i]->GetName(), static_cast<EXPINTEGER>( nMemberExp ) );
				AddPartyMemberExperience( apMember[i], nMemberExp, 0 );
			}
		}
		else	// 한대도 안때렸으면 보너스 경치와 옵션 경치와 풀파티보너스경치 만 준다
		{
			if( nMaxLevel10 < apMember[i]->GetLevel() )
			{
				nMemberExp	= static_cast<EXPINTEGER>( ( fAddExp * ( ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel()) / fMaxMemberLevel ) ) + fOptionExp + fFullParty );
				// 죽은후 경험치 더주기

				if( apMember[i]->IsAfterDeath() )
					nMemberExp	*= static_cast<EXPFLOAT>( 1.5f );
				
				// HP가 적을때 경험치 더주기

				int nHitPercent = apMember[i]->GetHitPointPercent( 100 );
				if( nHitPercent < CRITICAL_BERSERK_HP )
				{
					if( nHitPercent <= 10 )
						nMemberExp	*= 1.2f;
					else
						nMemberExp	+= ( nMemberExp * .2f ) - ( ( nMemberExp * .2f ) * nHitPercent / CRITICAL_BERSERK_HP );
				}				
				TRACE("GetExp(Contribution) : %s -> %I64d\n ", apMember[i]->GetName(), static_cast<EXPINTEGER>( nMemberExp ) );
				AddPartyMemberExperience( apMember[i], nMemberExp, 0 );
			}
		}
	}
}

void CMover::AddExperiencePartyLevel( CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 )
{
	float fRate;
	EXPINTEGER nMemberExp;
	int	  nMemberFxp;
	float fMaxMemberLevel = 0.0f;
	for( int i = 0 ; i < nMemberSize ; i++ )
	{
		fMaxMemberLevel += ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
	}
	float fAddExp = 0.0f;
	fAddExp = ( fExpValue * 0.2f ) * ( pParty->m_nSizeofMember - 1 );
	TRACE("GetExp(Level) : %s -> %f\n ", pParty->m_sParty, fExpValue );

	for( i = 0 ; i < nMemberSize ; i++ )
	{
		if( nMaxLevel10 < apMember[i]->GetLevel() )
		{
			//[ 몬스터의 경험치 + {(몬스터 경험치 * 0.15) * (극단 인원 - 1)} * (자신의 레벨의 제곱/모든 파티원 레벨 제곱의 합) ]
			nMemberExp = static_cast<EXPINTEGER>( ( ( fExpValue + fAddExp ) * ( ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel()) / fMaxMemberLevel ) ) ); //+ fFullAddExp );

			// 죽은후 경험치 더주기

			if( apMember[i]->IsAfterDeath() )
				nMemberExp	*= static_cast<EXPFLOAT>( 1.5f );

			// HP가 적을때 경험치 더주기

			int nHitPercent = apMember[i]->GetHitPointPercent( 100 );
			if( nHitPercent < CRITICAL_BERSERK_HP )
			{
				if( nHitPercent <= 10 )
					nMemberExp	*= 1.2f;
				else
					nMemberExp	+= ( nMemberExp * .2f ) - ( ( nMemberExp * .2f ) * nHitPercent / CRITICAL_BERSERK_HP );
			}

			AddPartyMemberExperience( apMember[i], nMemberExp, 0 );
			TRACE("GetExp(Level) : %s -> %I64d\n ", apMember[i]->GetName(), static_cast<EXPINTEGER>( nMemberExp ) );
		}
	}
}

void CMover::AddExperienceLevelReduce( EXPFLOAT* fExpValue, MoverProp* pMoverProp, float* fFxpValue, int nMaxLevel ) 
{
	// 죽이넘 주변의 파티원들중 최고 레벨 파티원으로 경험치를 감소 시킴
	int dw_Level = nMaxLevel - (int)pMoverProp->dwLevel;
	if( dw_Level > 0 )
	{
#ifdef __LEVELCLASS_EXP
		switch( dw_Level ) {
		case 1:
			(*fExpValue) *= static_cast<EXPFLOAT>( 0.8f );
			(*fFxpValue)  *= 0.8f;
			break;
		case 2:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.6f );
			(*fFxpValue)	*= 0.6f;
			break;
		case 3:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.35f );
			(*fFxpValue)	*= 0.35f;
			break;
		case 4:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.2f );
			(*fFxpValue)	*= 0.2f;
			break;
		case 5:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.12f );
			(*fFxpValue)	*= 0.12f;
			break;
		case 6:
			(*fExpValue) *= static_cast<EXPFLOAT>( 0.08f );
			(*fFxpValue)  *= 0.08f;
			break;
		case 7:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.04f );
			(*fFxpValue)	*= 0.04f;
			break;
		case 8:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.02f );
			(*fFxpValue)	*= 0.02f;
			break;
		default:
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.01f );
			(*fFxpValue)	*= 0.01f;
			break;
		}
#else // __LEVELCLASS_EXP
		if( 1 == dw_Level || dw_Level == 2 )		// 1~2 차이
		{
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.7f );
			(*fFxpValue)	*= 0.7f;
		}
		else if( 3 == dw_Level || dw_Level == 4 )	// 3~4 차이
		{
			(*fExpValue)	*= static_cast<EXPFLOAT>( 0.4f );
			(*fFxpValue)	*= 0.4f;
		}
		else										// 5이상 차이
			if( dw_Level >= 5 )
			{
				(*fExpValue)	*= static_cast<EXPFLOAT>( 0.1f );
				(*fFxpValue)	*= 0.1f;
			}
#endif //__LEVELCLASS_EXP
	}
}
#endif	// __WORLDSERVERnot client



#ifndef __BEAST

// 서버에서도 사용되는 신버전.
BOOL CMover::IsAttackAble( CObj *pObj )
{
	BOOL bAble	= FALSE;
	CWorld* pWorld = GetWorld();
	if( pObj )
	{
	#ifdef __S0504_FLY
		if( IsStateMode( STATE_BASEMOTION_MODE ) )	
		{
		#ifdef __CLIENT
			g_DPlay.SendStateModeCancel( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
		#else
			SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
		#endif 
			return FALSE;		
		}
	#endif	// __S0504_FLY
		
		if( pObj->GetType() == OT_MOVER )
		{
		#ifdef __CLIENT
			CMover *pMover = (CMover *)pObj;

			#ifdef __JAPAN_PVP
				if( pMover->IsPlayer() )
			#else // __JAPAN_PVP
				if( pMover->IsPlayer() && g_eLocal.GetState( EVE_18 ) == 1 )
			#endif // __JAPAN_PVP
				{
				#ifdef __GUILDCOMBAT
					if( pWorld->GetID() == WI_WORLD_GUILDWAR )
					{
						if( IsGuildCombatTarget( pMover ) ) // GuildCombat
							bAble = TRUE;
					}
					else
				#endif // __GUILDCOMBAT
					{
						if( IsPVPTarget( pMover ) )			// pvp
						{
							if( m_nDuelState == 1 && !IsFly() && !pMover->IsFly() )
								bAble	= TRUE;
						}
						else if( IsWarTarget( pMover ) )	// war
							bAble	= TRUE;
						else if( IsSchoolTarget( pMover ) )	// event
							bAble	= TRUE;
						else								// pk
						{
							bAble	= IsPKAttackAble( pMover );
							if( bAble )
								( (CWndWorld *)g_WndMng.m_pWndWorld )->SetLastTarget( pMover->GetId() );
						}
					}
				}
				else
				{
					bAble	= IsAttackAbleNPC( pMover );
				}
		#else // __CLIENT			
				if( GetHitType( (CMover *)pObj, TRUE ) == HITTYPE_FAIL )
					bAble = FALSE;
				else
					bAble = TRUE;
		#endif // __CLIENT
		}
		else
		{
			// no mover
			bAble	= TRUE;
		}
		
	#ifdef __CLIENT
		if( !bAble )
			( (CWndWorld *)g_WndMng.m_pWndWorld )->m_bAutoAttack	= FALSE;
	#endif
	}
	return bAble;
}

BOOL CMover::IsAttackAbleNPC( CMover* pNPC )
{
	BOOL bAble = TRUE;
	if( pNPC->IsDie() )	// 이미 죽은넘은 포커스 해제시키고 더이상 실행하지 않음.
	{
#ifdef __CLIENT
		g_WorldMng()->SetObjFocus( NULL );
#else
		if( IsPlayer() )
			((CUser *)this)->AddSetTarget( NULL_ID );
#endif
		bAble = FALSE;
	} else
	{
		MoverProp *pMoverProp = pNPC->GetProp();	

		if( pMoverProp->bKillable == 0 )	return FALSE;		// 공격이불가능한 NPC는 무조건 공격못함.
		// 평화적이면 못때림.
		if( pNPC->IsPeaceful() == TRUE )	// 평화적인넘은 공격 안됨
		{
			bAble = FALSE;
		}
		else
		{
			if( pMoverProp->dwClass == RANK_GUARD )	// 가드일때만 성향을 비교한다.  
			{
				if( pMoverProp->nChaotic < 0 && IsChaotic() == TRUE )		// 성향이 서로 같으면 공격못함.
					bAble = FALSE;
				else if( pMoverProp->nChaotic > 0 && IsChaotic() == FALSE )		// 성향이 서로 같으면 공격못함.
					bAble = FALSE;
			}

			if( m_pActMover->IsFly() && pMoverProp->dwFlying == 0 )	// 공격자가 비행중이고 타겟이 비행몹이 아니면

				bAble = FALSE;
			else if( m_pActMover->IsFly() == FALSE && pMoverProp->dwFlying == 1 )	// 공격자 지상 / 타겟 비행몹

				bAble = FALSE;
		}
	}
#ifdef __CLIENT
	if( bAble == TRUE )
		if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	// NPC에게 공격키를 누르고 있을때 클릭하면 자동공격.
			((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
#endif // Client
				
	return bAble;
}

#ifdef __CLIENT
BOOL CMover::IsPKAttackAble( CMover* pMover )
{
	BOOL bResult	= FALSE;

	if( !g_eLocal.GetState( EVE_PK ) )
		return bResult;

#ifdef __NPP_050308
	bResult		= IsPKInspection( pMover );
#else // __NPP_050308
#ifdef __CLIENT
	if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	// 공격키를 누르고 있을때
#else
	if( 1 )	// 서버에선 무조건 공격키 누르고 있는걸로 간주. 스킬큐땜시. 완벽한 방식은 아닌거 같군.
#endif
	{
		bResult = TRUE;

		if( pMover->IsRegionAttr( RA_SAFETY ) )		// 상대가 안전영역
		{
			if( pMover->IsChaotic() == FALSE )		// 상대가 착한넘이면 공격 못함.
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_PVP_SAFEZONENOATTACK ), NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK ) );							
				g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
				bResult = FALSE;
			}
			// 착한넘이 아니면 공격됨.
		} else
		if( IsRegionAttr( RA_SAFETY ) )	// 내가 안전영역
		{
			if( pMover->IsChaotic() == FALSE )		// 상대가 착한넘이면 공격 못함.
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_PVP_SAFEZONENOATTACK2 ), NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK2 ) );
				g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
				bResult = FALSE;
			}
			// 상대가 나쁜넘이면 공격됨.
		}
		if( g_pPlayer->GetLevel() < 15 )	// 내가 레벨 15가 안됬으면.
		{
			if( pMover->IsChaotic() == FALSE )
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_PVP_LOWLEVEL ), NULL, prj.GetTextColor( TID_GAME_PVP_LOWLEVEL ) );
				//g_WndMng.PutString( "레벨 15이상부터 공격이 가능합니다", NULL, 0xff99cc00 );
				g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
				bResult = FALSE;
			} else
				bResult = TRUE;	// 슬로터는 레벨이 낮아도 깔수 있다.
		}
		else if( pMover->GetLevel() < 15)	// 상대가 레벨15가 안됬으면.
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_PVP_LOWLEVEL2 ), NULL, prj.GetTextColor( TID_GAME_PVP_LOWLEVEL2 ) );
			//g_WndMng.PutString( "그 사람의 레벨이 낮습니다.", NULL, 0xff99cc00 );
			g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
			bResult = FALSE;
		}
		if( pMover->m_nDuel )	// 듀얼중인 상대는 PK할 수 없다.
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_ALREADYDUEL ), NULL, prj.GetTextColor( TID_GAME_ALREADYDUEL ) );
			//g_WndMng.PutString( "듀얼중인 상대는 공격할 수 없어요.", NULL, 0xff99cc00 );
			g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
			bResult = FALSE;
		}
		if( g_Party.FindMember( pMover->m_idPlayer ) != -1 )	// 같은 파티원은 공격안됨 - Ctrl자동공격키로 실수할 우려.
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_NOTATTACKPARTY ), NULL, prj.GetTextColor( TID_GAME_NOTATTACKPARTY ) );
			g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
			bResult = FALSE;
		}
	#ifndef __XUZHU
		if( pMover->m_idMurderer == m_idPlayer && pMover->m_tmDead )		// 내가 pMover를 또 죽이려 하고 아직 10분이 안된 상태면

		{
			g_WndMng.PutString( prj.GetText( TID_GAME_PKTIMELIMIT ), NULL, prj.GetTextColor( TID_GAME_PKTIMELIMIT ) );
			//g_WndMng.PutString( "10분 이내에 같은사람을 공격할 수는 없어요", NULL, 0xff99cc00 );
			g_WndMng.m_pWndWorld->m_bLButtonDown = 0;
			bResult = FALSE;
		}
	#endif // xuzhu
		if( IsFly() == FALSE && pMover->IsFly() )	// this는 비행중이 아닌데 플레이어가 비행중일때.
			bResult = FALSE;		// 공격안됨.
		else
		if( IsFly() && pMover->IsFly() == FALSE )	// this는 비행중/ 타겟은 지상

			bResult = FALSE;
	}
#endif // __NPP_050308
	return bResult;
}

#ifdef __NPP_050308
BOOL CMover::IsPKInspection( CMover* pMover )
#else	// __NPP_050308
BOOL CMover::IsPKInspection( CMover* pMover, int nFlag )
#endif	// __NPP_050308
{
#ifdef __NPP_050308
#ifdef __CLIENT
	if( g_Party.IsMember( pMover->m_idPlayer ) )
	{
		return FALSE;
	}
#endif	// __CLIENT
	if( m_idGuild > 0 && m_idGuild == pMover->m_idGuild )
	{
		return FALSE;
	}

	int nError	= 0;
	DWORD dwRegionAttr	= GetPKPVPRegionAttr();

	BOOL bAble	= TRUE;
	if( ( nError = IsPKPVPInspectionBase( dwRegionAttr, FALSE ) ) )
	{
		//
		bAble	= FALSE;
	}
	if( ( nError = pMover->IsPKPVPInspectionBase( dwRegionAttr, FALSE ) ) )
	{
		//
		bAble	= FALSE;
	}
	if( bAble )
	{
		if( pMover->m_vtInfo.IsVendorOpen() )
		{
			if( IsForceAttack() )
			{
			}
			else if( !( g_Neuz.m_NeuzEnemy.IsPKing( pMover->GetId() ) ) )
				bAble	= FALSE;
		}
		else
		{
			if( dwRegionAttr == RA_PENALTY_PK )
			{
				if( IsForceAttack() )
				{
//					( (CWndWorld *)g_WndMng.m_pWndWorld )->m_bAutoAttack	= TRUE;
				}
				else if(
					!( IsMode( FREEPK_MODE ) ||
						( g_Neuz.m_NeuzEnemy.IsPKing( pMover->GetId() ) ) ||
//						( (CWndWorld *)g_WndMng.m_pWndWorld )->IsLastTarget( pMover->GetId() ) ||
						pMover->IsChaotic() )
				)
				{
					bAble	= FALSE;
				}
			}
			else
			{
//				if( IsForceAttack() )
				{
//					( (CWndWorld *)g_WndMng.m_pWndWorld )->m_bAutoAttack	= TRUE;
				}
			}
		}
	}
//	( (CWndWorld *)g_WndMng.m_pWndWorld )->SetLastTarget( pMover->GetId() );
	return bAble;
#else	// __NPP_050308
//{{AFX
	BOOL bResult = FALSE;

	// 카오스 유저는 무조건 공격
#ifdef __NPP_050308
	if( IsRegionAttr( RA_SAFETY ) == TRUE || pMover->IsRegionAttr( RA_SAFETY ) == TRUE )
	{
		if( pMover->m_vtInfo.IsVendorOpen() == FALSE )
		{
			PrintString( pMover, TID_GAME_PVP_SAFEZONENOATTACK );	//g_WndMng.PutString( prj.GetText( TID_GAME_PVP_SAFEZONENOATTACK ), NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK ) );
		}		
	}
	else
#ifdef __WORLDSERVER
	if( g_PartyMng.GetParty( m_idparty ) != NULL && g_PartyMng.GetParty( m_idparty )->IsMember( pMover->m_idPlayer ) )	// 극단 
	{
	}
#endif // __WORLDSERER
#ifdef __CLIENT
	if( g_Party.IsMember( pMover->m_idPlayer ) )	// 극단 
	{
		g_WndMng.PutString( "같은 극단은 PK를 할수 없습니다", NULL, prj.GetTextColor( TID_GAME_LACKSPACE ) ); 
	}
#endif // __CLIENT
	else
	if( GetGuild() != NULL && GetGuild()->IsMember( pMover->m_idPlayer ) )
	{
#ifdef __CLIENT
		g_WndMng.PutString( "같은 길드는 PK를 할수 없습니다", NULL, prj.GetTextColor( TID_GAME_LACKSPACE ) ); 
#endif // __CLIENT
	}
	else
	if( m_nDuelState == 215 || pMover->m_nDuelState == 215 ) //
	{
#ifdef __CLIENT
		g_WndMng.PutString( "교전후 15초가 지냐야 대전을 할수 있습니다", NULL, prj.GetTextColor( TID_GAME_LACKSPACE ) ); 
#endif // __CLIENT
	}
	else
	if( m_nDuelState != 0 || pMover->m_nDuelState != 0 ) // 듀얼중
	{
#ifdef __CLIENT
		g_WndMng.PutString( "듀얼중이나 피케이중이라 전투를 할수 없습니다", NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddText( "듀얼중이나 피케이중이라 전투를 할수 없습니다" );
#endif // __WORLDSERVER
	}
	else
	{
		if( pMover->IsChaotic() == TRUE && IsBaseJob() == FALSE )
			bResult = TRUE;
		else
			bResult = IsPKPVPInspectionBase( pMover, nFlag );
	}
#ifdef __CLIENT
	if( bResult && pMover->IsChaotic() == FALSE && pMover->IsDie() == FALSE )
	{
		if( this == g_pPlayer )
		{
			if( g_pPlayer->IsRegionAttr( RA_PENALTY_PK ) )
			{
				if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	// 공격키를 누르고 있을때
					bResult = TRUE;
				else
					bResult = FALSE;
			}
		}
	}
#endif // __CLIENT
#else // __NPP_050308
	bResult = IsPKPVPInspectionBase( pMover, nFlag );
#endif // __NPP_050308

	if( bResult )
	{
		// Fly 검사

		if( IsFly() == FALSE && pMover->IsFly() )	// this는 비행중이 아닌데 플레이어가 비행중일때.
			bResult = FALSE;		// 공격안됨.
		else if( IsFly() && pMover->IsFly() == FALSE )	// this는 비행중/ 타겟은 지상

			bResult = FALSE;
	}
	return bResult;
//}}AFX
#endif	// __NPP_050308
}
#endif // CLIENT

#ifdef __NPP_050308
#ifdef __WORLDSERVER
BOOL CMover::IsPVPInspection( CMover* pMover, int nFlag )
{
	int nError	= 0;
	DWORD dwAttr	= GetPKPVPRegionAttr();
	
	switch( nFlag )
	{
		case 1:		// 개인 PVP
			{
//				1
				if( abs( GetLevel() - pMover->GetLevel() ) >= 30 )	// 레벨 30이상 나는것
				{
#ifdef __WORLDSERVER
					((CUser*)this)->AddDefinedText( TID_PK_LEVEL_GAP, "" );	// 레벨이 30이상 차이가 나면 PVP를 할수 없습니다
#endif // __WORLDSERVER
					return FALSE;
				}
//				1	// 2	// 3	// 4	// 5
				if( ( nError = IsPKPVPInspectionBase( dwAttr ) ) == 0 )
				{
					nError = pMover->IsPKPVPInspectionBase( dwAttr );
				}
#ifdef __WORLDSERVER
				if( nError != 0 )
				{
					switch( nError )
					{
					case 1:
						((CUser*)this)->AddDefinedText( TID_PK_SAFETY_NO, "" );	// 안전영역에서는 PVP할수 없습니다
						break;
					case 2:
						((CUser*)this)->AddDefinedText( TID_PK_SAME_NO, "" );	// 같은영역에 있어야 PVP를 할수 있습니다
						break;
					case 3:
						((CUser*)this)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1차전직 이후에 PVP를 할수 있습니다
						break;
					}
					return FALSE;
				}
#endif // __WORLDSERVER
				return TRUE;
			}
		case 2:		// 극단 PVP
			{
				g_PartyMng.m_AddRemoveLock.Enter( theLineFile );	// lock
				CParty* pSrc	= g_PartyMng.GetParty( m_idparty );
				CParty* pDest	= g_PartyMng.GetParty( pMover->m_idparty );
				if( pSrc && pDest )
				{
					DWORD dwTick	= GetTickCount();
					u_long anPlayer[MAX_PTMEMBER_SIZE*2];
					int	nSize	= 0;
					for( int i = 0; i < pSrc->GetSizeofMember(); i++ )
						anPlayer[nSize++]	= pSrc->GetPlayerId( i );
					for( i = 0; i < pDest->GetSizeofMember(); i++ )
						anPlayer[nSize++]	= pDest->GetPlayerId( i );
					g_PartyMng.m_AddRemoveLock.Leave( theLineFile );	// unlock

					CUser* pLeaderSrc	= (CUser*)pSrc->GetLeader();
					CUser* pLeaderDest	= (CUser*)pDest->GetLeader();
					if( IsValidObj( (CObj*)pLeaderSrc ) && IsValidObj( (CObj*)pLeaderDest ) )
					{
						if( abs( pLeaderSrc->GetLevel() - pLeaderDest->GetLevel() ) >= 30 )
						{
#ifdef __WORLDSERVER
							((CUser*)this)->AddDefinedText( TID_PK_LEVEL_GAP, "" );	// 레벨이 30이상 차이가 나면 PVP를 할수 없습니다
#endif // __WORLDSERVER
							return FALSE;
						}
					}
					else
					{
						//
						return FALSE;
					}

					for( i = 0; i < nSize; i++ )
					{
						CUser* pPlayer	= g_UserMng.GetUserByPlayerID( anPlayer[i] );
						if( IsValidObj( (CObj*)pPlayer ) )
						{
							if( ( nError = pPlayer->IsPKPVPInspectionBase( dwAttr ) ) )
							{
#ifdef __WORLDSERVER
								if( nError != 0 )
								{
									switch( nError )
									{
									case 1:
										((CUser*)this)->AddDefinedText( TID_PK_SAFETY_NO, "" );	// 안전영역에서는 PVP할수 없습니다
										break;
									case 2:
										((CUser*)this)->AddDefinedText( TID_PK_SAME_NO, "" );	// 같은영역에 있어야 PVP를 할수 있습니다
										break;
									case 3:
										((CUser*)this)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1차전직 이후에 PVP를 할수 있습니다
										break;
									}
									return FALSE;
								}
#endif // __WORLDSERVER
							}
						}
					}
					return TRUE;
				}
				g_PartyMng.m_AddRemoveLock.Leave( theLineFile );	// unlock
				return FALSE;
			}
	}
	return TRUE;
}
#endif	// __WORLDSERVER
#endif	// __NPP_050308

#ifdef __NPP_050308
int CMover::IsPKPVPInspectionBase( DWORD dwRegionAttr, BOOL bPVP )
#else	// __NPP_050308
BOOL CMover::IsPKPVPInspectionBase( CMover* pMover, int nFlag )
#endif	// __NPP_050308
{
#ifdef __NPP_050308
//	0
	DWORD dwAttr	= GetPKPVPRegionAttr();
	if( dwRegionAttr == RA_SAFETY )		// 안전지역 검사

	{
		return 1;
	}
	if( dwRegionAttr != dwAttr )	// 같은지역에 있는지 검사

	{
		return 2;
	}

#ifdef __CLIENT
	if( GetAsyncKeyState( VK_MENU ) & 0x8000 )
		return 9;
#endif	// __CLIENT
	
//	1
#ifdef __JAPAN_PVP
	if( bPVP && dwAttr == RA_FIGHT || dwAttr != RA_PK )	// PVP나 Penalty 지역이면 검사

#else // __JAPAN_PVP
	if( bPVP || dwAttr != RA_PK )	// PVP나 Penalty 지역이면 검사

#endif // __JAPAN_PVP
	{
		if( IsBaseJob() )
		{
			return 3;
		}
	}
	if( bPVP )
	{
		if( m_nDuelState != 0 )
		{
			return 9;
		}
	}
//	2	// 4
/*
	if( bPVP )
	{
		DWORD dwTick	= GetTickCount();
		if( m_dwPKRelease > dwTick )
		{
			//
			return 1;
		}
	}
*/
//	3
	if( IsFly() )
	{
		//
		return 1;
	}
//	5
	if( m_idWar > 0 )
	{
		//
		return 1;
	}
	return 0;
#else	// __NPP_050308
//{{AFX
	DWORD dwPKAttr = 0xffffffff;
	if( nFlag == 2 )
	{
#ifdef __WORLDSERVER
		if( m_idparty == 0 || pMover->m_idparty == 0 )
		{
			PrintString( pMover, TID_GAME_HAVENOTPARTY );
			return FALSE;
		}
		if( m_idparty == pMover->m_idparty )
		{
			PrintString( pMover, TID_GAME_SAMEPARTY );
			return FALSE;
		}
		g_PartyMng.Lock( theLineFile );	// lock
		int nLevel, nLevel1;
		CMover* pMember;
		CParty* pParty	= g_PartyMng.GetParty( m_idparty );
		if( pParty && pParty->IsLeader( m_idPlayer ) )
		{
			int nPartyMemberSize	= pParty->GetSizeofMember();
			for( int j = 0; j < nPartyMemberSize; j++ )
			{
				PartyMember* pPartyMember	= &pParty->m_aMember[j];
				pMember		= prj.GetUserByID( pPartyMember->m_uPlayerId );
				if( IsValidObj( pMember ) )
				{
					if( pMember->SubPKPVPInspectionBase( this, pMover, dwPKAttr, nFlag ) == FALSE )
					{
						g_PartyMng.Unlock( theLineFile );	// unlock2
						return FALSE;
					}
					if( j == 0 )	// 극단장의 레벨을 구해놓음
					{
						nLevel = pMember->GetLevel();
					}
				}
			}
		}
		else
		{
			PrintString( pMover, TID_GAME_NOTPARTYLEADER );		// 파티가 없음
			g_PartyMng.Unlock( theLineFile );	// unlock2
			return FALSE;
		}
		CParty* pParty1	= g_PartyMng.GetParty( pMover->m_idparty );
		if( pParty1 && pParty1->IsLeader( pMover->m_idPlayer ) )
		{
			int nPartyMemberSize	= pParty1->GetSizeofMember();
			for( int j = 0; j < nPartyMemberSize; j++ )
			{
				PartyMember* pPartyMember	= &pParty1->m_aMember[j];
				pMember		= prj.GetUserByID( pPartyMember->m_uPlayerId );
				if( IsValidObj( pMember ) )
				{
					if( pMember->SubPKPVPInspectionBase( this, pMover, dwPKAttr, nFlag ) == FALSE )
					{
						g_PartyMng.Unlock( theLineFile );	// unlock2
						return FALSE;
					}
					if( j == 0 )	// 극단장의 레벨을 구해놓음
					{
						nLevel1 = pMember->GetLevel();
					}
				}
			}
		}
		else
		{
			PrintString( pMover, TID_GAME_NOTPARTYLEADER );
			g_PartyMng.Unlock( theLineFile );	// unlock2
			return FALSE;
		}		
		g_PartyMng.Unlock( theLineFile );	// unlock2

		if( abs( nLevel - nLevel1 ) >= 30 )
		{
			((CUser*)this)->AddText( "레벨이 30이상 차이가 나면 안됩니다" );
			if( IsValidObj( pMover ) )
				((CUser*)pMover)->AddText( "레벨이 30이상 차이가 나면 안됩니다" );
//			PrintString( pMover, TID_GAME_PVP_LOWLEVEL2 );
			return FALSE;
		}
#endif // __WORLDSERVER
	}
	else
	{
		// 레벨 검사

		if( SubPKPVPInspectionBase( this, pMover, dwPKAttr, nFlag ) == FALSE )
		{
			return FALSE;
		}

		if( pMover->SubPKPVPInspectionBase( this, pMover, dwPKAttr, nFlag ) == FALSE )
		{
			return FALSE;
		}
		
		if( abs( GetLevel() - pMover->GetLevel() ) >= 30 )
		{
#ifdef __CLIENT
			g_WndMng.PutString( "레벨이 30이상 차이가 나면 안됩니다", NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
			((CUser*)this)->AddText( "레벨이 30이상 차이가 나면 안됩니다" );
			if( IsValidObj( pMover ) )
				((CUser*)pMover)->AddText( "레벨이 30이상 차이가 나면 안됩니다" );
#endif // __WORLDSERVER
//			PrintString( pMover, TID_GAME_PVP_LOWLEVEL2 );
			return FALSE;
		}
	}
	return TRUE;
//}}AFX
#endif	// __NPP_050308
}

BOOL CMover::SubPKPVPInspectionBase( CMover* pMover, CMover* pMover1, DWORD dwPKAttr, int nFlag )
{
	if( IsBaseJob() ) // 전직을 안햇음
	{
#ifdef __CLIENT
		g_WndMng.PutString( prj.GetText( TID_PK_CHANGEJOB_NO ), NULL, prj.GetTextColor( TID_PK_CHANGEJOB_NO ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1차전직 이후에 PVP를 할수 있습니다
		if( IsValidObj( pMover ) )
			((CUser*)pMover)->AddDefinedText( TID_PK_CHANGEJOB_NO, "" );	// 1차전직 이후에 PVP를 할수 있습니다
#endif // __WORLDSERVER
//		pMover->PrintString( pMover1, TID_GAME_PVP_LOWLEVEL2 );
		return FALSE;
	}
	if( IsFly() )	// 날고 있음
	{
#ifdef __CLIENT
		g_WndMng.PutString( prj.GetText( TID_PK_NOFLIGHT_DURING ), NULL, prj.GetTextColor( TID_PK_NOFLIGHT_DURING ) ); // 비행중이면 듀얼을 할수 없습니다
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddDefinedText( TID_PK_NOFLIGHT_DURING, "" );	// 비행중이면 듀얼을 할수 없습니다
		if( IsValidObj( pMover ) )
			((CUser*)pMover)->AddDefinedText( TID_PK_NOFLIGHT_DURING, "" );	// 비행중이면 듀얼을 할수 없습니다
#endif // __WORLDSERVER		
//		pMover->PrintString( pMover1, TID_GAME_PVP_LOWLEVEL2 );
		return FALSE;
	}

#ifdef __WORLDSERVER
	if( m_idWar )	// 전쟁중엔 듀얼, PK 못함.
	{
		if( g_eLocal.GetState( EVE_GUILDWAR ) )
		{
			pMover->PrintString( pMover1, TID_GAME_GUILDWARERRORDUEL );	// 전쟁중엔 듀얼 못함다.
			return FALSE;
		}
	} 
	else
	{
#ifdef __NPP_050308
		if( m_nDuelState != 0 )
		{
			return FALSE;
		}
#else // __NPP_050308
		if( m_nDuel != 0 )
		{
			PrintString( pMover1, TID_GAME_ALREADYDUEL );	// 그넘은 이미 듀얼중
			return FALSE;
		}	
		
		if( pMover->m_nDuel != 0 )	// 듀얼중이 아닐때만 신청을 보냄.
		{
			PrintString( pMover1, TID_GAME_ALREADYDUEL );	// 그넘은 이미 듀얼중
			return FALSE;
		}
#endif // __NPP_050308
	}
#endif __WORLDSERVER

#ifdef __NPP_050308
	if( IsRegionAttr( dwPKAttr ) == FALSE )	// 같은 지역에 있는지 확인
	{
#ifdef __CLIENT
		g_WndMng.PutString( prj.GetText( TID_PK_SAME_NO ), NULL, prj.GetTextColor( TID_PK_SAME_NO ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
		((CUser*)this)->AddDefinedText( TID_PK_SAME_NO, "" );	// 같은영역에 있어야 PVP를 할수 있습니다
		if( IsValidObj( pMover ) )
			((CUser*)pMover)->AddDefinedText( TID_PK_SAME_NO, "" );	// 같은영역에 있어야 PVP를 할수 있습니다
#endif // __WORLDSERVER	
//		pMover->PrintString( pMover1, TID_GAME_PVP_SAFEZONENOATTACK );	//g_WndMng.PutString( prj.GetText( TID_GAME_PVP_SAFEZONENOATTACK ), NULL, prj.GetTextColor( TID_GAME_PVP_SAFEZONENOATTACK ) );
		return FALSE;
	}
#endif // __NPP_050308
	return TRUE;
}

void CMover::PrintString( CMover* pMover, DWORD dwId )
{
#ifdef __CLIENT
	g_WndMng.PutString( prj.GetText( dwId ), NULL, prj.GetTextColor( dwId ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
	((CUser*)this)->AddDefinedText( dwId, "" );
	if( IsValidObj( pMover ) )
		((CUser*)pMover)->AddDefinedText( dwId, "" );
#endif // __WORLDSERVER
}
// this는 공격자
// this가 pTarget을 스틸하려는지 검사.
// pTarget은 반드시 IsValid 체크를 하고 넘어와야 한다.
#ifdef __WORLDSERVER
int CMover::IsSteal( CMover *pTarget )
{
	if( IsNPC() || prj.m_nLanguage == LANG_JAP)	return 0;		// 공격자가 NPC면 스틸 체크대상 아님.
#ifdef __XHALLOW
	if( prj.m_nLanguage == LANG_THA || prj.m_nLanguage == LANG_CHI )	// 태국/대만의 경우

		if( pTarget->GetIndex() == MI_MRPUMPKIN5 || pTarget->GetIndex() == MI_ZOMBIGER5 )						// 괴수 미스터펌킨은
			return 0;													// 스틸이 허용됨.
#endif
#ifdef __EVENT_OLDBOY0621
		if( pTarget->GetIndex() == MI_DEMIAN5 || pTarget->GetIndex() == MI_KEAKOON5 || pTarget->GetIndex() == MI_MUFFRIN5 )
			return 0;;		// 얘들은 이벤트몹이므로 아무나 스틸 가능
#endif
		

	// 플레이어만 여기로 들어온다.
	if( pTarget->IsNPC() || (pTarget->IsPlayer() && pTarget->IsChaotic() == FALSE) )	// 공격자:플레이어, 맞는자:NPC 혹은 착한사람. (슬로터는 다구리당해도 된다).
	{
		BOOL bStealCheck = TRUE;
		if( pTarget->m_idTargeter == NULL_ID )	
		{
			return 0;	//타겟을를 누군가가 찜한 흔적이 없으면 스틸아님.
		}
		CMover *pOtherPlayer = prj.GetMover( pTarget->m_idTargeter );		// pTarget를 찜한 다른 유저.
		if( IsValidObj(pOtherPlayer) )
		{
			// 공격자가 파티가 있고 pTarget를 때렸던 사람이 같은 파티면 스틸이 아니다.
			if( m_idparty && (pOtherPlayer->m_idparty == m_idparty) )
				bStealCheck = FALSE;
			if( m_idWar && m_idGuild && (pOtherPlayer->m_idGuild == m_idGuild) ) // 전쟁중에 나랑같은 길드끼리 치는건 스틸이 아니다.
				bStealCheck = FALSE;
#ifdef __SCHOOL0701
			if( g_eLocal.GetState( EVE_SCHOOL ) )
				bStealCheck	= FALSE;
#endif	// __SCHOOL0701
			if( pTarget->GetProp()->dwClass == RANK_SUPER )		// 보스몹은 스틸없음.
				bStealCheck = FALSE;
		} else
			bStealCheck = FALSE;	// pTarget를 때렸던 유저가 유효하지 않으면 스틸이 아니다.

		if( bStealCheck && 
			pTarget->m_idTargeter != NULL_ID && pTarget->m_idTargeter != GetId() ) // pTarget를 찜한놈이 있고 그게 현재공격자가 아니라면 스틸.
		{
			((CUser*)this)->AddDefinedText( TID_GAME_PRIORITYMOB, "" );	// 몬스터가 다른캐릭터랑 전투중임다.
			return 1;		// 스틸.
		}
	}

	return 0;	// 스틸이 아님.
}


// 
// 죽었을때 경험치 깎는 부분.
//
float CMover::SubDieDecExp( BOOL bTransfer, DWORD dwDestParam )
{
	if( IsNPC() )	
		return 0.0f;

	float fRate = 0.1f, fDecExp = 0.0f;
	BOOL  bPxpClear = FALSE, bLvDown = FALSE;
	int nLevel	= GetLevel();

	if( nLevel == 1 )		{ fRate	= 0.8f; fDecExp = 0.0f ; bPxpClear = FALSE; bLvDown = FALSE; }
	else if( nLevel == 2 )	{ fRate	= 0.6f; fDecExp = 0.0f ; bPxpClear = FALSE; bLvDown = FALSE; }
	else if( nLevel <= 5 )	{ fRate	= 0.5f; fDecExp = 0.0f ; bPxpClear = FALSE; bLvDown = FALSE; }
	else if( nLevel <= 10 )	{ fRate	= 0.4f; fDecExp = 0.0f ; bPxpClear = TRUE ; bLvDown = FALSE; }
	else if( nLevel <= 15 )	{ fRate	= 0.3f; fDecExp = 0.02f; bPxpClear = TRUE ; bLvDown = FALSE; }
	else					{ fRate	= 0.3f; fDecExp = 0.03f; bPxpClear = TRUE ; bLvDown = TRUE;  }

	if( m_bLastPK )		// 무조건 경험치 안깍는다...
		return fRate;

#ifdef __GUILDCOMBAT
	if( m_bGuildCombat )
		return fRate;
#endif // __GUILDCOMBAT
	if( m_bLastDuelParty )
	{
		m_bLastDuelParty = FALSE;
		return fRate;
	}

#ifdef	__NPP_050308 
	bLvDown = FALSE;	// 사망 시 경험치 하락으로 인한 레벨 하락 막기

#endif

	if( fDecExp )
	{
		if( IsAfterDeath() == FALSE ) 
		{
			m_nDeathExp = m_nExp1;		// 현재 경험치를 죽었을 때 경험치에 기록 
			m_nDeathLevel = m_nLevel;	// 현재 레벨 기록 
		}
		
		// 로드스타/라이트로 부활한것은 경험치 하락.
		if( dwDestParam != 0 )
		{
			FLOAT fAddDec = (float)(100 - dwDestParam) / 100.0f;
			fDecExp = fDecExp - (fDecExp * fAddDec);
		}

#ifdef __NPP_050308 
		// 카르마 등급은 기존의 사망시 경험치 하락에도 영향을 미친다.
		KarmaProp* pProp = prj.GetKarmaProp( m_nSlaughter );
		if( pProp )
		{
			//fDecExp = ( fDecExp * (1.0f + ( 30.0f / 100.0f) ));
			fDecExp = ( fDecExp * (1.0f + ( (float)pProp->nSubtractExpRate / 100.0f) ));
		}
#endif // __NPP_050308 

#ifdef __S050524_SM_ITEM
		if( IsSMMode( SM_REVIVAL ) )
			((CUser*)this)->SetSMMode( SM_REVIVAL, 0 );
		else
#endif // __S050524_SM_ITEM
			bLvDown = DecExperiencePercent( fDecExp, bPxpClear, bLvDown );

		if( bTransfer )
		{
			if( bLvDown )	// 레벨 다운이 되었는가?
			{
				g_UserMng.AddSetLevel( this, (WORD)m_nLevel );		// this이외의 주변에 레벨변경정보를 보냄.
				((CUser *)this)->AddSetGrowthLearningPoint( m_nRemainGP, m_nRemainLP );		// 해당유저에게 GP,LP변경정보를 보냄.
			}

			( (CUser *)this )->AddSetExperience( GetExp1(), GetExp2(), (WORD)m_nLevel,  m_nDeathExp, (WORD)m_nDeathLevel );		// 해당유저에게 exp1,exp2변경된 정보를 보냄.
		}
	}
	
	return fRate;	// 부활할때 얼마의 HP를 회복할꺼냐.
}

#endif // WORLDSERVER



#ifdef __NPP_050308
BOOL CMover::DropItemByDied( CMover* pAttacker )
{
	BOOL bResult;
	OBJID objid = GetMaxEnemyHitID();
	if( objid != NULL_ID )
	{
		CMover* pMover	= prj.GetMover( objid );
		if( IsValidObj( pMover ) )
			bResult = DropItem( pMover );
		else
			objid = NULL_ID;
	}

	if( objid == NULL_ID )
		bResult = DropItem( pAttacker );

	return bResult;
}
#endif // __NPP_050308


// 몹이 죽어서 아이템을 드랍할때 사용

// 클라는 이쪽으로 안들어온다.
// 죽어서 물건을 떨어트리는넘이 this다.
BOOL CMover::DropItem( CMover* pAttacker )
{
//#ifndef __BEAST
#ifdef __WORLDSERVER
	if( pAttacker->IsPlayer() && IsNPC() )
	{
		MoverProp* lpMoverProp	= GetProp();
//		ASSERT( lpMoverProp );
		if( !lpMoverProp )
			return FALSE;

		if( m_nQuestKeeping > 0 )
		{
			CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
			GUILDQUESTELEM* pElem	= pProcessor->GetGuildQuest( m_nQuestKeeping );
			if( pElem && pElem->objidWormon == GetId() )
			{
				g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
				CGuild* pGuild	= pAttacker->GetGuild();
				if( pGuild )
				{
					pGuild->SetQuest( pElem->nId, pElem->ns );
					g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, pElem->nId, pElem->ns );
					pElem->nProcess	= GQP_GETITEM;
					pElem->dwEndTime	= GetTickCount() + MIN( 20 );
					pElem->ns	= pElem->nf	= 0;
					pElem->nState	= 0;
					pElem->objidWormon	= NULL_ID;
				}
				g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
			}
		}

#ifdef __EVE_0401A
		{
			static EVENTITEM eventItem[4]	=
			{
				II_SYS_SYS_EVE_0401A01HEART, 15000000,
				II_SYS_SYS_EVE_0401A02WILL, 60000000,
				II_SYS_SYS_EVE_0401A03PEACE, 300000000,
				II_SYS_SYS_EVE_0401A04HONOR, 600000000,
			};
			
			float	f	= 0.0f;
			int  d	= pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
			if( d <= 1 )	f	= 1.0f;
			else if( d <= 2 )	f	= 0.8f;
			else if( d <= 4 )	f	= 0.6f;
			else if( d <= 7 )	f	= 0.3f;
			else	f	= 0.1f;

			for( int i = 0;i < 4; i++ )
			{
				if( xRandom( 3000000000 ) <= (DWORD)( eventItem[i].dwPrabability * f ) )
				{
					CItemElem itemElem;
					itemElem.m_dwItemId		= eventItem[i].dwIndex;
					itemElem.m_nItemNum		= 1;
					itemElem.m_nHitPoint	= -1;
					if( pAttacker->CreateItem( &itemElem ) == FALSE )
					{
						CItemElem* pItemElem	= new CItemElem;
						pItemElem->m_dwItemId	= eventItem[i].dwIndex;
						pItemElem->m_nItemNum	= 1;
						ItemProp* pItemProp		= pItemElem->GetProp();
						if( pItemProp )
							pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
						pItemElem->m_dwSerialNumber		= xRand();
						CItem* pItem	= new CItem;
						pItem->m_pItemBase	= pItemElem;

						pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
						pItem->SetPos( pAttacker->GetPos() );
						pItem->SetAngle( xRandom( 360 ) );
						pItem->m_dwDropTime		= timeGetTime();
						pItem->m_bDropMob	= TRUE;
						pAttacker->GetWorld()->AddObj( pItem, TRUE );
						( (CUser*)pAttacker )->AddDefinedText( TID_GAME_DROPITEM, "\"%s\"", prj.GetItemProp( eventItem[i].dwIndex )->szName );
					}
					else
					{
						ItemProp* pItemProp	= prj.GetItemProp( eventItem[i].dwIndex );
						if( pItemProp )
							( (CUser*)pAttacker )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
					}
				}
			}

			if( g_eLocal.GetState( EVE_0401A ) == 1 && ( pAttacker->m_nLevel <= (int)lpMoverProp->dwLevel ) )
			{	//
				if( lpMoverProp->dwID == MI_PUKEPUKE5 || lpMoverProp->dwID == MI_BANG5 ||
					lpMoverProp->dwID == MI_ROCKMUSCLE5 )
				{
					if( pAttacker->m_Inventory.GetCount( II_SYS_SYS_EVE_0401F01COUPON ) == 0 )
					{
						CItemElem itemElem;
						itemElem.m_dwItemId		= II_SYS_SYS_EVE_0401F01COUPON;
						itemElem.m_nItemNum		= 1;
						itemElem.m_nHitPoint	= -1;
						if( pAttacker->CreateItem( &itemElem ) == TRUE )
						{
							ItemProp* pItemProp	= prj.GetItemProp( II_SYS_SYS_EVE_0401F01COUPON );
							if( pItemProp )
							{
								g_DPSrvr.OnLogItem( "M", m_szName, pAttacker->m_szName, pAttacker->GetWorld()->GetID(), pAttacker->GetGold(), pAttacker->GetGold(), (DWORD)0, (DWORD)0,  pItemProp->szName, 1, 0, GetGold() );
								( (CUser*)pAttacker )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
							}
						}
					}
				}
			}	// 
		}
#endif	// __EVE_0401A

#ifdef __EVE_XMAS
		if( xRandom( 3000000000 ) <= (DWORD)3000000 )
		{
			DWORD dwItemId	= xRandom( II_ARM_M_CLO_SANTA01GLOVES, II_ARM_F_CLO_RUDOLF01SUIT );
			CItemElem itemElem;
			itemElem.m_dwItemId		= dwItemId;
			itemElem.m_nItemNum		= 1;
			itemElem.m_nHitPoint	= -1;
			if( pAttacker->CreateItem( &itemElem ) == TRUE )
			{
				ItemProp* pItemProp	= prj.GetItemProp( dwItemId );
				if( pItemProp )
					( (CUser*)pAttacker )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
			}
		}	
#endif	// __EVE_XMAS

#ifdef __EVE_NEWYEAR
		CTime tmCur		= CTime::GetCurrentTime();
#ifdef __TAIWAN_TEST_SERVER__
		if( tmCur > CTime( 2004, 12, 23, 0, 0, 0 ) && tmCur < CTime( 2004, 12, 27, 0, 0, 0 ) )
#else	// __TAIWAN_TEST_SERVER__
		if( tmCur > CTime( 2004, 12, 30, 0, 0, 0 ) && tmCur < CTime( 2005, 1, 4, 0, 0, 0 ) )
#endif	// __TAIWAN_TEST_SERVER__
		{
			if( xRandom( 3000000000 ) <= (DWORD)150000000 )
			{
				CItemElem itemElem;
				itemElem.m_dwItemId		= II_GEN_MAG_TRI_NEWYEARBOMB;
				itemElem.m_nItemNum		= 1;
				itemElem.m_nHitPoint	= -1;
				if( pAttacker->CreateItem( &itemElem ) == TRUE )
				{
					ItemProp* pItemProp	= prj.GetItemProp( II_GEN_MAG_TRI_NEWYEARBOMB );
					if( pItemProp )
						( (CUser*)pAttacker )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
				}
			}
		}
#endif	// __EVE_NEWYEAR

		QUESTITEM* pQuestItem;
		int	nQuestCnt;
		short nNum;
		u_long uSizeOfQuestItem	= lpMoverProp->m_QuestItemGenerator.GetSize();
		for( u_long i = 0; i < uSizeOfQuestItem; i++ ) 
		{
			pQuestItem	= lpMoverProp->m_QuestItemGenerator.GetAt( i );
			LPQUEST lpQuest = pAttacker->GetQuest( pQuestItem->dwQuest );
			if( lpQuest && lpQuest->m_nState == pQuestItem->dwState ) 
			{
				DWORD dwProbability	= pQuestItem->dwProbability;

				if( pQuestItem->dwQuest == QUEST_VALENTINE || pQuestItem->dwQuest == QUEST_WHITEDAY )
				{
#ifdef __TAIWAN__
					if( ( pAttacker->m_nLevel - (int)lpMoverProp->dwLevel ) >= 5 )
						dwProbability	/= 2;
#else	// __TAIWAN__
					float f;
					int  d	= pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
					if( d <= 1 )	f	= 1.0f;
					else if( d <= 2 )	f	= 0.8f;
					else if( d <= 4 )	f	= 0.6f;
					else if( d <= 7 )	f	= 0.3f;
					else	f	= 0.1f;
					dwProbability	*= f;
#endif	// __TAIWAN__
				}

				if( xRandom( 3000000000 ) <= dwProbability ) 
				{
					if( pQuestItem->dwNumber == 0 )
						Error( "CMover::DropItem : %s의 quest item drop %d번째의 dwNumber가 0", GetName(), i );
					nNum	= xRandom( pQuestItem->dwNumber ) + 1;
					if( pQuestItem->dwIndex == 0 )
					{
						WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, lpMoverProp->szName );
						break;
					}

					int nMemberSize		= 0;
					CMover* pMember;
					CMover* apMember[MAX_PTMEMBER_SIZE];
					// 1
					g_PartyMng.Lock( theLineFile );
					CParty* pParty	= g_PartyMng.GetParty( pAttacker->m_idparty );
					if( pParty && pParty->IsMember( pAttacker->m_idPlayer ) )
					{
						int nPartyMemberSize	= pParty->GetSizeofMember();
						for( int j = 0; j < nPartyMemberSize; j++ )
						{
							PartyMember* pPartyMember	= &pParty->m_aMember[j];
							pMember		= prj.GetUserByID( pPartyMember->m_uPlayerId );
							if( IsValidObj( pMember ) && IsNearPC( pMember->GetId() ) )
							{
								LPQUEST pMemberQuest	= pMember->GetQuest( pQuestItem->dwQuest );
								if( pMemberQuest && pMemberQuest->m_nState == pQuestItem->dwState )
									apMember[nMemberSize++]	= pMember;
							}
						}
					}
					// 3
					g_PartyMng.Unlock( theLineFile );
					// 4
					CMover* pTakeMover;
					if( nMemberSize > 0 )
						pTakeMover	= apMember[xRandom( nMemberSize )];
					else
						pTakeMover	= pAttacker;

					BYTE nId;
					CItemElem itemElem;
					itemElem.m_dwItemId		= pQuestItem->dwIndex;
					itemElem.m_nItemNum		= nNum;
					itemElem.m_nHitPoint	= -1;
					if( pTakeMover->CreateItem( &itemElem, &nId ) == FALSE )
					{
						CItemElem* pItemElem	= new CItemElem;
						pItemElem->m_dwItemId	= pQuestItem->dwIndex;
						pItemElem->m_nItemNum	= nNum;
						ItemProp* pItemProp		= pItemElem->GetProp();
						if( pItemProp )
							pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
						pItemElem->m_dwSerialNumber		= xRand();
						CItem* pItem	= new CItem;
						pItem->m_pItemBase	= pItemElem;
						if( pItemElem->m_dwItemId == 0 ) Error("DropItem:1st %s\n", GetName() );
						pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
						pItem->SetPos( pTakeMover->GetPos() );
						pItem->SetAngle( xRandom( 360 ) );
						pItem->m_idHolder	= pTakeMover->m_idPlayer;
						pItem->m_dwDropTime		= timeGetTime();
						pTakeMover->GetWorld()->AddObj( pItem, TRUE );
						( (CUser*)pTakeMover )->AddDefinedText( TID_GAME_DROPITEM, "\"%s\"", prj.GetItemProp( pQuestItem->dwIndex )->szName );
					}
					else
					{
						( (CUser*)pTakeMover )->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", prj.GetItemProp( pQuestItem->dwIndex )->szName );
						// 퀘스트 아이템 로그
						CItemBase* pItemBase	= pTakeMover->GetItemId( nId );
						if( pItemBase )
						{
							CItemElem* pItemElem	= (CItemElem*)pItemBase;
							LogItemInfo aLogItem;
							aLogItem.Action = "Q";
							aLogItem.SendName = pTakeMover->m_szName;
							aLogItem.RecvName = "QUEST";
							aLogItem.WorldId = pTakeMover->GetWorld()->GetID();
							aLogItem.Gold = aLogItem.Gold2 = pTakeMover->GetGold();
							g_DPSrvr.OnLogItem( aLogItem, pItemElem, pItemElem->m_nItemNum );
						}
					}
				}
			}
		}

		int nloop = 1;
		BOOL bUnique = FALSE;
		if( pAttacker->m_idparty ) // 내가 파티에 참여 여부
		{
			g_PartyMng.Lock( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pAttacker->m_idparty );
			if( pParty )
			{
				// 순회극단이며 링크어택중이며 단장만 데미지를 높여준다
				if( pParty->m_nKindTroup == 1 )
				{
					if( pParty->m_nModeTime[PARTY_GIFTBOX_MODE] || pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
					{
						// 여기서 단장주위를 검색하여 주위이면 할수 잇게 함
						CUser* pLeader = g_UserMng.GetUserByPlayerID( pParty->m_aMember[0].m_uPlayerId );
						if( IsValidObj( (CObj*)pLeader )/* && pLeader->IsNearPC( (CUser*)pAttacker ) && IsOrigin()*/ )
						{
							if( pLeader->IsSMMode( SM_PARTYSKILL15 ) || pLeader->IsSMMode( SM_PARTYSKILL30 ) )	// 여기서 유료 아이템 사용중인지 확인
							{
								if( pParty->m_nModeTime[PARTY_GIFTBOX_MODE] )
								{
									nloop = 2;
								}
								if( pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
								{
									bUnique = TRUE;
								}
							}
							else
							{
								if( pLeader->IsValidArea( pAttacker, 255.0f ) )
								{
									if( pParty->m_nModeTime[PARTY_GIFTBOX_MODE] )
									{
										nloop = 2;
									}
									if( pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
									{
										bUnique = TRUE;
									}
								}
							}
						}
					}
				}
			}
			g_PartyMng.Unlock( theLineFile );	// 더 이상 파티를 참조할 필요가 없다.
		}

		D3DXVECTOR3 vPos;		// 드랍될 위치.
		
		for( int k = 0 ; k < nloop ; k++ )
		{
			// 아이템 드롭 확률		
			int nProbability	= 100, nPenyaRate	= 100;
			BOOL bAdjDropRate = TRUE;	// 일반적으론 레벨차에 의한 드랍률저하가 이뤄진다.

			if( GetIndex() == MI_CLOCKWORK1 
#ifdef __TAIWAN__
				|| GetIndex() == MI_MRPUMPKIN5 || GetIndex() == MI_ZOMBIGER5
#endif	// __TAIWAN__
			)	// 보스는 레벨차에의한 드랍률저하가 없다.
				bAdjDropRate = FALSE;

#ifdef __EVENT_OLDBOY0621
			if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
				bAdjDropRate = FALSE;		// 이벤트몹들도 레벨차에의한 드랍률 저하가 없다.
#endif
			
			if( bAdjDropRate )
			{
				int d	= pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
				if( d <= 1 )	{	nProbability	= 100;	nPenyaRate	= 100;	}
				else if( d <= 2 )	{	nProbability	= 80;	nPenyaRate	= 100;	}
				else if( d <= 4 )	{	nProbability	= 60;	nPenyaRate	= 80;	}
				else if( d <= 7 )	{	nProbability	= 30;	nPenyaRate	= 65;	}
				else	{	nProbability	= 10;	nPenyaRate	= 50;	}
			}

		#ifdef __S1108_BACK_END_SYSTEM
			if( xRandom( 100 ) < nProbability * prj.m_fItemDropRate * lpMoverProp->m_fItemDrop_Rate )	// 아이템을 드롭할지 말지 결정. 레벨차가 많이 나면 아예 떨어트리지 않는다.			
		#else // __S1108_BACK_END_SYSTEM
			if( xRandom( 100 ) < nProbability * prj.m_fItemDropRate )	// 아이템을 드롭할지 말지 결정. 레벨차가 많이 나면 아예 떨어트리지 않는다.			
		#endif // __S1108_BACK_END_SYSTEM
			{
				int nSize	= lpMoverProp->m_DropItemGenerator.GetSize();
				int nNumber	= 0;
				DROPITEM* lpDropItem;
		
				for( int i = 0; i < nSize; i++ )
				{
					if( ( lpDropItem = lpMoverProp->m_DropItemGenerator.GetAt( i, bUnique, true ) ) != NULL )
					{
						if( lpDropItem->dtType == DROPTYPE_NORMAL )
						{
							DWORD dwNum		= lpDropItem->dwNumber;
							if(  dwNum == (DWORD)-1 )
								dwNum	= 1;

							if( prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN )	// 대만은...
							{
								if( GetProp()->dwClass == RANK_BOSS || GetProp()->dwClass == RANK_SUPER	 )// 보스/거대

								{
									if( lpDropItem->dwLevel >= 5 )	// +5 아이템 드롭 금지.
										continue;
								} else
								{	// 일반 몹일경우

									if( lpDropItem->dwLevel >= 3 )	// +3 아이템 드롭 금지.
										continue;
								}
							}
							if( lpMoverProp->dwFlying )
							{
								CItemElem itemElem;
								itemElem.m_dwItemId	= lpDropItem->dwIndex;
								itemElem.m_nItemNum		= xRandom( dwNum ) + 1;
								itemElem.SetAbilityOption( lpDropItem->dwLevel );
								
								if( pAttacker->CreateItem( &itemElem ) == TRUE )
								{	// log
									ItemProp* pItemProp	= itemElem.GetProp();
									if( pItemProp )
									{
										( (CUser*)pAttacker )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
										if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
										{
											switch( pItemProp->nLog )
											{
												case 1:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), itemElem.GetAbilityOption() );	break;
												case 2:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 200 );	break;
												case 3:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 100 );	break;
											}
										}
									}
									if( lpDropItem->dwNumber != (DWORD)-1 )
										nNumber++;
									if(  nNumber >= lpMoverProp->m_DropItemGenerator.m_dwMax )
										break;
								}
								continue;
							}
							CItemElem* pItemElem	= new CItemElem;
							pItemElem->m_dwItemId	= lpDropItem->dwIndex;
							pItemElem->m_nItemNum	= xRandom( dwNum ) + 1;
							ItemProp* pItemProp		= pItemElem->GetProp();
							if( pItemProp )
							{
								pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
#ifdef __RANDOMOPTITEM0817
								pItemElem->m_nRandomOptItemId	= CRandomOptItemGen::GetInstance()->GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp );
#endif	// __RANDOMOPTITEM0817
							}
							pItemElem->SetAbilityOption( lpDropItem->dwLevel );		// 추가 능력치 +1, +2 같은거.
							pItemElem->m_dwSerialNumber = xRand();
							CItem* pItem	= new CItem;
							pItem->m_pItemBase	= pItemElem;
							BOOL bJJim = TRUE;		// 주인이 있다는걸 표시해야하는 템인가.
							if( lpMoverProp->dwClass == RANK_SUPER )		// 보스몹이 드롭한 아이템은 아무나 먹을수 있다.
								bJJim = FALSE;
#ifdef __EVENT_OLDBOY0621
							if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
								bJJim = FALSE;		// 얘들은 이벤트몹이므로 찜안해놔도 된다. 아무나 먹을수 있음
#endif
							if( bJJim )
							{
								pItem->m_idOwn = pAttacker->GetId();	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
								pItem->m_dwDropTime = timeGetTime();	// 드랍 했을당시의 시간을 기록함.
							}
							pItem->m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
	#ifdef __CLIENT
							pItem->m_pItemBase->SetTexture();
	#endif	// __CLIENT
							if( pItem->m_pItemBase->m_dwItemId == 0 ) Error("DropItem:2nd %s\n", GetName() );
							pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
#ifdef __WORLDSERVER
							vPos = GetPos();
							vPos.x += ( xRandomF(2.0f) - 1.0f );
							vPos.z += ( xRandomF(2.0f) - 1.0f );
							vPos.y = GetPos().y;
							pItem->SetPos( vPos );
#else
							pItem->SetPos( GetPos() );
#endif
							GetWorld()->AddObj( pItem, TRUE );

							if( lpDropItem->dwNumber != (DWORD)-1 )
								nNumber++;

	#ifdef __WORLDSERVER
							if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
							{
								switch( pItemProp->nLog )
								{
								case 1:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, pItemElem->GetAbilityOption() );	// 일반 아이템 +아이템
									break;
								case 2:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 200 );	// 이펙트 아이템
									break;
								case 3:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 100 );	// 유니크 아이템
									break;
								}
							}
	#endif // __WORLDSERVER
							if( nNumber == lpMoverProp->m_DropItemGenerator.m_dwMax )
								break;
						} else
						// 돈은 무조건떨어져야 한다.
						if( lpDropItem->dtType == DROPTYPE_SEED && k == 0 )
						{
							int	nSeedID = 0;
							int nNumGold = lpDropItem->dwNumber + xRandom( lpDropItem->dwNumber2 - lpDropItem->dwNumber );	// Number ~ Number2 사이의 랜덤값.
							nNumGold	= nNumGold * nPenyaRate / 100;
#						ifdef __EVENT_01
							nNumGold	*= 2;
#						endif	// __EVENT_01
	#ifdef __S1108_BACK_END_SYSTEM
							nNumGold	= nNumGold * prj.m_fGoldDropRate * lpMoverProp->m_fPenya_Rate; 
							if( nNumGold == 0 )
								continue;
	#else // __S1108_BACK_END_SYSTEM
							nNumGold	*= prj.m_fGoldDropRate;
	#endif // __S1108_BACK_END_SYSTEM


							if( lpMoverProp->dwFlying )
							{
								pAttacker->AddGold( nNumGold );
								( (CUser*)pAttacker )->AddGoldText( nNumGold );
							}
							else
							{
								// 돈액수에 따라 어떤모양의 시드를 사용할지 결정한다.
								if( nNumGold <= prj.GetItemProp( II_GOLD_SEED1 )->dwAbilityMax )
									nSeedID = II_GOLD_SEED1;
								else if( nNumGold <= prj.GetItemProp( II_GOLD_SEED2 )->dwAbilityMax )
									nSeedID = II_GOLD_SEED2;
								else if( nNumGold <= prj.GetItemProp( II_GOLD_SEED3 )->dwAbilityMax )
									nSeedID = II_GOLD_SEED3;
								else 
									nSeedID = II_GOLD_SEED4;

								CItemElem* pItemElem	= new CItemElem;
								pItemElem->m_dwItemId	= nSeedID;
								pItemElem->m_nItemNum	= nNumGold;	// 돈액수

								pItemElem->m_nHitPoint	= nNumGold;
								CItem* pItem	= new CItem;
								pItem->m_pItemBase	= pItemElem;
								BOOL bJJim = TRUE;
								if( lpMoverProp->dwClass == RANK_SUPER )		// 보스몹이 드롭한 아이템은 아무나 먹을수 있다.
									bJJim = FALSE;
#ifdef __EVENT_OLDBOY0621
								if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
									bJJim = FALSE;		// 얘들은 이벤트몹이므로 찜안해놔도 된다. 아무나 먹을수 있음
#endif
								if( bJJim )
								{
									pItem->m_idOwn = pAttacker->GetId();	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
									pItem->m_dwDropTime = timeGetTime();	// 드랍 했을당시의 시간을 기록함.
								}
								pItem->m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
	#						ifdef __CLIENT
								pItem->m_pItemBase->SetTexture();
	#						endif	// __CLIENT
								if( pItem->m_pItemBase->m_dwItemId == 0 ) Error("DropItem: 3rd %s\n", GetName() );
								pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
#ifdef __WORLDSERVER
								vPos = GetPos();
								vPos.x += ( xRandomF(2.0f) - 1.0f );
								vPos.z += ( xRandomF(2.0f) - 1.0f );
								pItem->SetPos( vPos );
#else
								pItem->SetPos( GetPos() );
#endif
								GetWorld()->AddObj( pItem, TRUE );
							}
						} // DROPTYPE_SEED
					} // if
					//////////////
					//  여기까지 for-loop안내려오고 continue하는 수도 있으니까 여기다 코드 넣지 말것.
					///////////
				} // for nSize

				nSize	= lpMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				int nAbilityOption, nDropLuck;
				BOOL bDrop = FALSE;
				for( i = 0; i < nSize; i++ )
				{
					bDrop = FALSE;
					pDropKind	= lpMoverProp->m_DropKindGenerator.GetAt( i );
					pItemKindAry	= prj.GetItemKindAry( pDropKind->dwIK3 );
					int nMinIdx	= -1,	nMaxIdx		= -1;
					for( int j = pDropKind->nMinUniq; j <= pDropKind->nMaxUniq; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pDropKind->dwIK3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pDropKind->dwIK3, j );
						if( nMaxIdx != -1 )
							break;
					}
					if( nMinIdx < 0 || nMaxIdx < 0 )
					{
#ifdef __DEV
						WriteLog( "dropkind, 1, dwIndex = %d, nIndex = %d", lpMoverProp->dwID, i );
#endif	// __DEV
						continue;
					}
					ItemProp* pItemProp		= (ItemProp*)pItemKindAry->GetAt( nMinIdx + xRandom( nMaxIdx - nMinIdx + 1 ) );
					if( NULL == pItemProp )
					{
#ifdef __DEV
						WriteLog( "dropkind, 2, dwIndex = %d, nIndex = %d", lpMoverProp->dwID, i );
#endif	// __DEV
						continue;
					}

					nAbilityOption	= xRandom( 11 );	// 0 ~ 10

					DWORD dwAdjRand;
					
					for( int k = nAbilityOption; k >= 0; k-- )
					{
#ifdef __EXPDROPLUCK0524
						DWORD dwPrabability		= (DWORD)( prj.m_adwExpDropLuck[( pItemProp->dwItemLV > 120? 119: pItemProp->dwItemLV-1 )][k]
#else	// __EXPDROPLUCK0524
						DWORD dwPrabability		= (DWORD)( prj.m_adwExpDropLuck[( pItemProp->dwItemLV > 100? 99: pItemProp->dwItemLV-1 )][k]
#endif	// __EXPDROPLUCK0524
						* ( (float)lpMoverProp->dwCorrectionValue / 100.0f ) );

						dwAdjRand	= xRandom( 3000000000 );
						if( bUnique && dwPrabability <= 10000000 )
							dwAdjRand	/= 2;

						if( prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN )	// 대만은...
						{
							if( GetProp()->dwClass == RANK_BOSS || GetProp()->dwClass == RANK_SUPER	)	// 거대몹일 경우

							{
								if( k >= 5 )	// +5 아이템 드롭 금지.
									dwPrabability = 0;
							} else
							{	// 일반 몹일경우

								if( k >= 3 )	// +3 아이템 드롭 금지.
									dwPrabability = 0;
							}
						}
//						if( prj.m_nLanguage == LANG_CHI && k >= 1 )	// 대만은 +아이템 드롭 금지.
//							if( GetProp()->dwClass != RANK_BOSS && GetProp()->dwClass != RANK_SUPER )	// 보스도 아니고 슈퍼도 아닐때만 금지적용.
//								dwPrabability = 0;	// 확률을 제로로 만들어버림
							
						if( dwAdjRand < dwPrabability )
						{
							if( lpMoverProp->dwFlying )
							{
								CItemElem itemElem;
								itemElem.m_dwItemId		= pItemProp->dwID;
								itemElem.m_nItemNum		= 1;
								itemElem.SetAbilityOption( k );
#ifdef __RANDOMOPTITEM0817
								itemElem.m_nRandomOptItemId	= CRandomOptItemGen::GetInstance()->GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp );
#endif	// __RANDOMOPTITEM0817
								if( pAttacker->CreateItem( &itemElem ) == TRUE )
								{	// log
									( (CUser*)pAttacker )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
									if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
									{
										switch( pItemProp->nLog )
										{
											case 1:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), itemElem.GetAbilityOption() );	break;
											case 2:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 200 );	break;
											case 3:		g_dpDBClient.SendLogUniqueItem2( pAttacker, itemElem.GetProp(), 100 );	break;
										}
									}
									continue;
								}
							}

							CItemElem* pItemElem	= new CItemElem;
							pItemElem->m_dwItemId	= pItemProp->dwID;
							pItemElem->m_nItemNum	= 1;
							pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
							pItemElem->SetAbilityOption( k );
#ifdef __RANDOMOPTITEM0817
							pItemElem->m_nRandomOptItemId	= CRandomOptItemGen::GetInstance()->GenRandomOptItem( lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp );
#endif	// __RANDOMOPTITEM0817
							pItemElem->m_dwSerialNumber		= xRand();
							CItem* pItem	= new CItem;
							pItem->m_pItemBase	= pItemElem;
							BOOL bJJim = TRUE;

							if( lpMoverProp->dwClass == RANK_SUPER )		// 보스몹이 드롭한 아이템은 아무나 먹을수 있다.
								bJJim = FALSE;
#ifdef __EVENT_OLDBOY0621
							if( GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5 )
								bJJim = FALSE;		// 얘들은 이벤트몹이므로 찜안해놔도 된다. 아무나 먹을수 있음
#endif
							if( bJJim )
							{
								pItem->m_idOwn	= pAttacker->GetId();
								pItem->m_dwDropTime		= timeGetTime();
							}
							pItem->m_bDropMob	= TRUE;
//							pItem->m_pItemBase->SetTexture();
							if( pItem->m_pItemBase->m_dwItemId == 0 ) Error("DropItem: 4th %s\n", GetName() );
							pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
							pItem->SetPos( GetPos() );
							GetWorld()->AddObj( pItem, TRUE );
							bDrop = TRUE;
	#ifdef __WORLDSERVER
							if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || ( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY ) )
							{
								switch( pItemProp->nLog )
								{
								case 1:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, pItemElem->GetAbilityOption() );	break;
								case 2:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 200 );	break;
								case 3:		g_dpDBClient.SendLogUniqueItem( pAttacker, pItem, 100 );	break;
								}
							}
	#endif // __WORLDSERVER
							break;
						}
					}

#ifdef __TAIWAN__
					if( GetProp()->dwClass == RANK_BOSS || GetProp()->dwClass == RANK_SUPER )
					{
						if( bDrop )		// KIND아이템을 하나 떨어트렸다면 
							break;		// 더이상 떨어트리지 않고 종료.
					}
#endif // Taiwan
				} // dropKindGene for-loop;
			}
		}
	}
	
#ifdef __XAI1013
	if( IsNPC() )	// 몹이 죽었을때..
	{
		MoverProp* pMoverProp	= GetProp();
		if( pMoverProp->m_nLoot == 2 )	// 2아 d옵션.  아이템 먹고 뱉기까지 하는 옵션.
		{
			int nSize = m_Inventory.GetSize();
			int i;
			CItemElem *pElem = NULL;
			for( i = 0; i < nSize; i ++ )
			{
				pElem = m_Inventory.GetAt( i );
				if( pElem )
				{
					D3DXVECTOR3 vPos = GetPos();
				#ifdef __WORLDSERVER
					vPos.x += ( xRandomF(2.0f) - 1.0f );
					vPos.z += ( xRandomF(2.0f) - 1.0f );
				#endif
					CItem *pItem = DropItem( i, pElem->m_nItemNum, vPos, FALSE );
					pItem->m_idOwn	= pAttacker->GetId();
					pItem->m_dwDropTime		= timeGetTime();
				}
			}
		}
	}
#endif // xAI1013

#endif	// __WORLDSERVER

	return TRUE;
}

// this오브젝트에다 찜한사람(JJimer -_-;;;) 의 아이디를 박아둬서 
// this가 찜당했다는걸 표시.
void CMover::SetJJim( CMover *pJJimer )
{
#ifdef __XHALLOW
	if( prj.m_nLanguage == LANG_THA || prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN )	// 태국/대만의 경우

		if( GetIndex() == MI_MRPUMPKIN5 )						// 괴수 미스터펌킨은
			return;													// 스틸이 허용됨.
#endif

	if( GetProp()->dwClass == RANK_SUPER )		// 슈퍼몹은 스틸 허용.
		return;
		
	// 찜한넘은 사람 / 찜당한넘은 NPC일때만.
	if( pJJimer->IsPlayer() && IsNPC() ) //this != pJJimer )		// 찜한사람이 사람일경우만 적용./ 자기자신을 찜할순 없다.
	{
#ifdef __WORLDSERVER
#ifndef __NPP_050308
		if( m_idTargeter != NULL_ID &&  m_idTargeter != pJJimer->GetId() )
		{
			CMover *pTargeter = prj.GetMover( m_idTargeter );
			if( IsValidObj(pTargeter) )
			{
				if( pTargeter->m_idparty != pJJimer->m_idparty )	// 파티도 다를때.
					Message( "찜머가 바뀜 %s %s %s", GetName(), pJJimer->GetName(), pTargeter->GetName() );
			}
		}
#endif // __NPP_050308
#endif
		m_idTargeter = pJJimer->GetId();
	}
}

#endif // not BEAST


CGuild* CMover::GetGuild( void )
{
#ifdef __WORLDSERVER
//	locked
	CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
	if( pGuild && pGuild->IsMember( m_idPlayer ) )
		return pGuild;
	return NULL;
#else	// __WORLDSERVER
#ifdef __CLIENT
	if( m_idGuild > 0 && CMover::GetActiveMover() &&
		 m_idGuild == CMover::GetActiveMover()->m_idGuild )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
		if( pGuild && pGuild->IsMember( m_idPlayer ) )
			return pGuild;
		return NULL;
	}
	else
	{
		return g_GuildMng.GetGuild( m_idGuild );
	}
#endif	// __CLIENT
#endif	// __WORLDSERVER

	return NULL;
}

#ifndef __BEAST
CGuildWar* CMover::GetWar( void )
{
	return g_GuildWarMng.GetWar(m_idWar);
}
#endif

#ifdef  __WORLDSERVER
// HP, MP, FP회복을 처리한다.
void CMover::ProcessRecovery()
{
//	if( IsLive() == FALSE )
//		return;
	if( IsDie() )
		return;

	if( IsPlayer() == FALSE )
		return;

#ifdef __NPP_050308
	// 일정시간마다 카르마 회복 
	KarmaProp* pProp = prj.GetKarmaProp( m_nSlaughter );
	if( pProp->dwKarmaRecoverPoint )
	{
		if( m_dwKarmaTick == 0 )
		{
			m_dwKarmaTick = GetTickCount() + pProp->dwKarmaRecoverPoint * 1000;
		}
		else if( GetTickCount() > m_dwKarmaTick )
		{
			m_dwKarmaTick = 0; 
			ChangeSlaughter( CHANGE_SLAUGHTER_RECOVERY, NULL );
		}
	}

	if( m_timerDuel.IsTimeOut() )	// 두얼 시작후 1초에 3, 2, 1, Fight
	{
		if( 100 <= m_nDuelState && m_nDuelState < 200 )		// 100 ~ 200 은 듀얼 시작후 시작 타임
		{
			--m_nDuelState;
			if( 100 == m_nDuelState )	// 마지막 Fight 메세지를 보냄
			{
				m_nDuelState = 1;		// 듀얼 중이라고 세팅
				if( m_nDuel == 1 )
				{
					CMover *pDuelOther = prj.GetMover( m_idDuelOther );
					if( pDuelOther )
					{
						((CUser*)this)->AddDuelStart( pDuelOther->m_idPlayer, 1 );	// Fight
						((CUser*)this)->AddDuelCount( 0 );
					}
				}
				else if( m_nDuel == 2 )
				{
					g_PartyMng.Lock( theLineFile );	// lock2
					CParty* pParty = g_PartyMng.GetParty( m_idDuelParty );		// 신청자의 파티꺼냄
					if( pParty )
					{
						LPCSTR pszPartyName;
						if( pParty->m_nKindTroup ) // 순회극단
							pszPartyName = pParty->m_sParty;
						else // 단막극단
							pszPartyName = prj.GetPlayerString( pParty->GetPlayerId( 0 ) );

						if( !pszPartyName )
						{
							Error( "Error Party PVP 2" );
						}
						else
						{
							int		i, j;
							OBJID	pDstMember[ MAX_PTMEMBER_SIZE ];
							memset( pDstMember, 0xff, sizeof(pDstMember) );
							CMover *pMember2;

							for( j = 0; j < pParty->m_nSizeofMember; j ++ )
							{
								pMember2 = (CMover *)g_UserMng.GetUserByPlayerID( pParty->m_aMember[j].m_uPlayerId );
								if( IsValidObj(pMember2) )
									pDstMember[j] = pMember2->GetId();
							}
							// 상대 파티에 대한 정보를 우리멤버들에게 보냄
							((CUser *)this)->AddDuelPartyStart( pszPartyName, pParty->m_nSizeofMember, pDstMember, pParty->m_uPartyId, 1 );		// 상대방 멤버의 ID를 다보낸다. 2값으로
							((CUser*)this)->AddDuelCount( 0 );
						}
					}
					g_PartyMng.Unlock( theLineFile );	// unlock2
				}
			}
			else
			{
				((CUser*)this)->AddDuelCount( m_nDuelState - 100 );
//				CString strMessage;
//				strMessage.Format( "%d", m_nDuelState - 100 );
//				((CUser*)this)->AddText( strMessage );	// 3, 2, 1
			}
		}
		m_timerDuel.Reset();		
	}

	if( m_nDuel != 0 && m_timerEndDuel.IsTimeOut() )
	{
		// 듀얼취소 3분동안 교전이 없었음
		if( m_nDuel == 1 )
		{
			CMover *pDuelOther = prj.GetMover( m_idDuelOther );
			if( pDuelOther )
			{
				pDuelOther->ClearDuel();
				( (CUser*)pDuelOther )->AddSetDuel( pDuelOther );
				ClearDuel();
				( (CUser*)this )->AddSetDuel( this );
			}
		}
		else if( m_nDuel == 2 )
		{
			g_PartyMng.Lock( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( m_idparty );
			if( pParty && pParty->IsLeader( m_idPlayer ) )
			{
				u_long uidDuelParty = m_idDuelParty;
				CMover *pMember;
				CParty* pParty2 = g_PartyMng.GetParty( uidDuelParty );
				for( int k = 0 ; k < pParty->m_nSizeofMember ; ++k )
				{
					pMember	= (CMover *)g_UserMng.GetUserByPlayerID( pParty->m_aMember[k].m_uPlayerId );					
					if( IsValidObj( pMember ) )
					{
						if( pParty2 )
						{
							pParty2->DoDuelPartyCancel( pParty );
						}
						else 
						{
							pParty->DoDuelPartyCancel( NULL );
							Message( "CMover::ProcessRecovery : 파티멤버 %s의 정보이상. %d %d", pMember->GetName(), pMember->m_idDuelParty, uidDuelParty );
						}
					}
				}
				if( pParty2 )
				{
					for( int k = 0 ; k < pParty2->m_nSizeofMember ; ++k )
					{
						pMember	= (CMover *)g_UserMng.GetUserByPlayerID( pParty2->m_aMember[k].m_uPlayerId );					
						if( IsValidObj( pMember ) )
						{	
							pParty->DoDuelPartyCancel( pParty2 );
						}
					}
				}
				g_DPCoreClient.SendSetPartyDuel( m_idparty, uidDuelParty, FALSE );
			}
			g_PartyMng.Unlock( theLineFile );
		}
	}
#endif // __NPP_050308
	
	int nRecovery = 100;
	if( m_pActMover->IsSit() )		// 앉아있을 때
	{
		if( m_timerRecovery.IsTimeOut() == FALSE )	// 일정시간마다.
			return;
		m_timerRecovery.Reset();
		
		// 파티 PARTY_STRETCHING_MODE일 때 
		g_PartyMng.Lock( theLineFile );
		CParty* pParty = g_PartyMng.GetParty( m_idparty );
		if( pParty && pParty->IsMember( m_idPlayer ) )
		{
			if( pParty->m_nKindTroup == 1 && pParty->m_nModeTime[PARTY_STRETCHING_MODE] )
			{
				CUser* pLeader = g_UserMng.GetUserByPlayerID( pParty->m_aMember[0].m_uPlayerId );
				if( IsValidObj( (CObj*)pLeader ) )
				{
					BOOL nMItem = FALSE;
					if( pLeader->IsSMMode( SM_PARTYSKILL15 ) || pLeader->IsSMMode( SM_PARTYSKILL30 ) )	// 여기서 유료 아이템 사용중인지 확인
					{
						nMItem = TRUE;
					}
					else
					{
						if( IsValidArea( pLeader, 255.0f ) )
						{
							nMItem = TRUE;
						}
					}				

					if( nMItem )
					{
						if( pLeader->GetJob() == JOB_ASSIST || pLeader->GetJob() == JOB_RINGMASTER || pLeader->GetJob() == JOB_BILLPOSTER )
						{
							nRecovery = 180;
						}
						else
						{
							nRecovery = 150;
						}
					}
				}
			}
		}
		else
		{
			m_idparty	= 0;
		}
		g_PartyMng.Unlock( theLineFile );	// 더 이상 파티를 참조할 필요가 없다.

		IncHitPoint( (GetHPRecovery() * nRecovery) / 100 );
		IncManaPoint( (GetMPRecovery() * nRecovery) / 100 );
		IncFatiguePoint( (GetFPRecovery() * nRecovery) / 100 );
	} 
	else	// 서있을 때
	{
		m_timerRecovery.Reset();		// 서있을땐 앉아있는 타임을 계속 리셋한다.
		if( m_pActMover->IsActAttack() == FALSE )	// 어택상태가 아닐때만 피찬다.
		{
			if( m_timerRecoveryStand.IsTimeOut() )	// 일정시간마다.
			{
				m_timerRecoveryStand.Reset();

				nRecovery = 60;  // 회복률의 60%의 효과 밖에 볼 수 없다.

				IncHitPoint( (GetHPRecovery() * nRecovery) / 100 );
				IncManaPoint( (GetMPRecovery() * nRecovery) / 100 );
				IncFatiguePoint( (GetFPRecovery() * nRecovery) / 100 );
			}
		} 
		else
		{
			m_timerRecoveryStand.Reset();	// 어택상태일때는 계속 시간을 리셋한다.
		}
	}
}
#endif // __WORLDSERVER

void CMover::AllocShopInventory( LPCHARACTER pCharacter )
{
	if( IsVendorNPC() )
	{
		m_ShopInventory[ 0 ] = new CItemContainer< CItemElem  >; m_ShopInventory[ 0 ]->SetItemContainer( ITYPE_ITEM, MAX_VENDOR_INVENTORY );
		m_ShopInventory[ 1 ] = new CItemContainer< CItemElem  >; m_ShopInventory[ 1 ]->SetItemContainer( ITYPE_ITEM, MAX_VENDOR_INVENTORY );
		m_ShopInventory[ 2 ] = new CItemContainer< CItemElem  >; m_ShopInventory[ 2 ]->SetItemContainer( ITYPE_ITEM, MAX_VENDOR_INVENTORY );
		m_ShopInventory[ 3 ] = new CItemContainer< CItemElem  >; m_ShopInventory[ 3 ]->SetItemContainer( ITYPE_ITEM, MAX_VENDOR_INVENTORY );
	}
}

BOOL CMover::IsVendorNPC()
{
	LPCHARACTER pCharacter	= GetCharacter();
	if( !pCharacter )
		return FALSE;
	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
	{
		if( pCharacter->m_venderItemAry[i].GetSize() )
			return TRUE;
	}
	return FALSE;
}


BOOL CMover::EndMotion()
{
#ifdef __CLIENT	
	if( !m_pActMover->IsActAttack() &&
		!m_pActMover->IsActDamage() &&
		!m_pActMover->IsActJump() && 
		!m_pActMover->IsFly() && 
		!m_pActMover->IsDie() && 
		!m_pActMover->IsSit() )
	{
		SetAngle( GetDegree(GetWorld()->GetCamera()->m_vPos, GetPos()) );		// 목표쪽으로 몸을 돌림.
		SetMotion( MTI_LOGOUT, ANILOOP_CONT, MOP_FIXED );
		return TRUE;
	}
#endif //__CLIENT
	return FALSE;
}

#ifndef __BEAST
BOOL CMover::IsBullet( ItemProp* pItemProp )
{
	// 필요총알종류가 지정되어 있다면

	if( pItemProp->dwLinkKindBullet != NULL_ID )
	{
		DWORD dwTip = TID_TIP_NEEDSKILLITEM;
#ifdef __NPP_050308
		if( pItemProp->dwItemKind3 == IK3_BOW || pItemProp->dwLinkKind == IK3_BOW )
		{
			dwTip = TID_TIP_NEEDSATTACKITEM;			
		}
#endif // __NPP_050308
		
		ItemProp *pProp = GetActiveHandItemProp( PARTS_BULLET );
		if( pProp )
		{
			if( pProp->dwItemKind2 != pItemProp->dwLinkKindBullet )		// 필요총알종류랑 다른걸 장착하고 있으면 에러
			{
#ifdef __CLIENT
				g_WndMng.PutString( prj.GetText( dwTip ), NULL, prj.GetTextColor( dwTip ) );
				( (CWndWorld *)g_WndMng.m_pWndWorld )->m_bAutoAttack	= FALSE;
#endif // __CLIENT
#ifdef __WORLDSERVER
				((CUser*)this)->AddDefinedText( dwTip, "" );
#endif // __WORLDSERVER
				return FALSE;
			}
		} else
		{	// 총알(?)이 없어도 에러
#ifdef __CLIENT
			g_WndMng.PutString( prj.GetText( dwTip ), NULL, prj.GetTextColor( dwTip ) );
#endif // __CLIENT
#ifdef __WORLDSERVER
			((CUser*)this)->AddDefinedText( dwTip, "" );
#endif // __WORLDSERVER
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __BEAST

#ifdef __ACROBAT_0504
#ifdef __WORLDSERVER
// 화살을 nCount대로 삭제
// 화살을 장착되어 있는것을 소모했을시 자동으로 같은 화살을 장착

// 같은 화살인지 체크
void CMover::ArrowDown( int nCount )
{
	CItemElem* pItemElem = m_Inventory.GetEquip( PARTS_BULLET );	// 장착되어 있는 화살 아이템 꺼냄
	if( pItemElem )
	{
		// 장착되어 있는것 모두 소진 되었음. 장착되어 있는 아이템 삭제후
		// 인벤토리에서 같은 화살이 있으면 그것을 사용하게 해줌
		DWORD dwItemId = pItemElem->m_dwItemId;
		BOOL bDoEquip = FALSE;
		for( int i = 0 ; i < 100 && 0 < nCount ; i++ )	// 혹 100번은 안나오겠지?
		{	// 아이템 사용이 4개인데 아이템 1개짜리가 4개일때 루프가 돌음
			BOOL bGetItem = FALSE;
			if( pItemElem->m_nItemNum <= nCount )
			{
				nCount -= pItemElem->m_nItemNum;
				pItemElem->m_nItemNum = 0;
				bGetItem = TRUE;
			}
			else
			{
				pItemElem->m_nItemNum -= nCount;
				nCount = 0;
			}
			UpdateItem( pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum );
			if( bGetItem )
			{
				bDoEquip = FALSE;
				pItemElem = m_Inventory.GetAtItemId( dwItemId );
				if( pItemElem == NULL ) 
				{
					break;
				}
				bDoEquip = TRUE;
			}
		}
		
		if( bDoEquip ) 
		{
			if( TRUE == DoEquip( pItemElem, TRUE, pItemElem->GetProp()->dwParts ) )
			{
				g_UserMng.AddDoEquip( this, pItemElem->GetProp()->dwParts, pItemElem, TRUE );
			}
		}
	}
}
#endif // __WORLDSERVER
#endif // __ACROBAT_0504


void CMover::ClearDuel( void )
{
	m_nDuel		= 0;
	m_idDuelOther	= NULL_ID;
#ifdef __NPP_050308
	m_nDuelState	= 0;
	m_timerEndDuel.Set( MIN(3) );
	SetPKTargetLimit( 10 );
#endif	// __NPP_050308
}

void CMover::ClearDuelParty( void )
{
	m_nDuel		= 0;
	m_idDuelParty	= 0;
#ifdef __NPP_050308
	m_nDuelState	= 0;
	m_timerEndDuel.Set( MIN(3) );
	SetPKTargetLimit( 10 );			// PK선공을 못하게 막게함
#endif	// __NPP_050308
}

void CMover::SetPKTargetLimit( int nSec )
{
#if defined( __WORLDSERVER ) && defined ( __NPP_050308 )
	m_dwPKTargetLimit = GetTickCount() + SEC( nSec );
#endif // defined( __WORLDSERVER ) && defined ( __NPP_050308 )
}

#if defined(__S0504_FLY) && defined(__WORLDSERVER)
BOOL CMover::IsItemRedyTime( ItemProp* pItemProp, OBJID dwObjid, BOOL bItemFind )
{
	if( pItemProp->dwSkillReadyType != 0 &&
		pItemProp->dwSkillReadyType != 0xffffffff )
	{
		// 라이드를 장착을 하려면 프로퍼티에 있는값을 참조하여 시전을 해야함
		if( IsStateMode( STATE_BASEMOTION_MODE ) ) 
		{
			if( m_nReadyTime != 0 )	// 시전중
			{
				( (CUser*)this )->AddDefinedText(TID_PK_BLINK_LIMIT, "" );	// 시전중에는 사용할수 없습니다
				return FALSE;
			}
			else	// 시전이 완료된 시점
			{
				SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF );
			}
		}
		else	// 시전 셑팅
		{
			m_nReadyTime = timeGetTime() + pItemProp->dwSkillReadyType;
			m_dwUseItemId = dwObjid;
			m_bItemFind = bItemFind;
			SetStateMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_ON );
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __S0504_FLY __WORLDSERVER

#ifndef __BEAST
#ifdef __ACROBAT_0504
CSfx* CMover::CreateSfxArrow( DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget  )
{
	CSfx* pSfx = NULL;

	// 화살은 왼손에 연결...
	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)m_pModel;
	pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );		// 주먹 월드좌표 구함	
	
	pSfx = CreateSfx( D3DDEVICE, dwSfxObjArrow, vPos, GetId(), vPosDest , idTarget );
	
	if( dwSfxObjHit != NULL_ID )
	{
		if( pSfx ) 
		{
			((CSfxShoot*)pSfx)->m_dwSfxHit = dwSfxObjHit;
		}
	}
	
	return pSfx;
	
}
#endif //__ACROBAT_0504
#endif //__BEAST

BOOL CMover::IsAfterDeath()	
{	
#ifdef __NPP_050308
	if( m_nDeathLevel > m_nLevel )
		return TRUE;

	if( m_nDeathLevel == m_nLevel )
	{
		if( m_nExp1 == 0 )
			return TRUE;
		if( m_nDeathExp > m_nExp1 )
			return TRUE;
	}
	return FALSE;
#else
	return ( m_nDeathLevel > m_nLevel || ( m_nDeathLevel == m_nLevel && m_nDeathExp > m_nExp1 ) )? TRUE: FALSE;	
#endif
}

#ifdef __J0608
int CMover::GetSetItem( CItemElem* pItemElem )
{
	static	DWORD adwParts[4]	= { PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CAP	};
	if( !pItemElem )
	{
		pItemElem	= GetEquipItem( PARTS_UPPER_BODY );
	}
	else
	{
		if( !IsSetItemPart( pItemElem->GetProp()->dwParts ) )
			return 0;
	}

	if( !pItemElem )
		return 0;

#ifdef __FINITEITEM0705
	if( pItemElem->IsFlag( CItemElem::expired ) )
		return 0;
#endif	// __FINITEITEM0705


	int nAbilityOption	= pItemElem->GetAbilityOption();
	DWORD dwParts	= pItemElem->GetProp()->dwParts;

	for( int i = 0; i < 4; i++ )
	{
		if( dwParts != adwParts[i] )
		{
			pItemElem	= GetEquipItem( adwParts[i] );
#ifdef __FINITEITEM0705
#ifdef __ENCHANT0916
			if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
			{
				if( nAbilityOption > pItemElem->GetAbilityOption() )
					nAbilityOption	= pItemElem->GetAbilityOption();
				continue;
			}
			return 0;
#else	// __ENCHANT0916
			if( pItemElem && pItemElem->GetAbilityOption() == nAbilityOption && !pItemElem->IsFlag( CItemElem::expired ) )
				continue;
			return 0;
#endif	// __ENCHANT0916
#else	// __FINITEITEM0705
			if( pItemElem && pItemElem->GetAbilityOption() == nAbilityOption )
				continue;
			return 0;
#endif	// __FINITEITEM0705
		}
	}
	return nAbilityOption;
}

int CMover::GetSetItemParts(DWORD dwParts)
{
	CItemElem* pItemElem = NULL;
	ItemProp * pItemProp = NULL;
	
	pItemElem = GetEquipItem( dwParts );
	
	int nAbilityOption = 0;
	
	if( pItemElem )
	{
		nAbilityOption = pItemElem->GetAbilityOption();
	} else
	{
		if( IsActiveMover() )
			return 0;
#ifdef __FINITEITEM0705
		nAbilityOption	= (m_aEquipInfo[dwParts].nOption & 0xFF );
#else	// __FINITEITEM0705
		nAbilityOption  = (m_anEquipOption[dwParts] & 0xFF);
#endif	// __FINITEITEM0705
	}

	return nAbilityOption;
}
		
int CMover::GetSetItemClient()
{
	static	DWORD adwParts[4]	= { PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CAP	};

#ifdef __ENCHANT0916
	int nAbilityOption	= 10;
#else	// __ENCHANT0916
	int nAbilityOption = GetSetItemParts(PARTS_UPPER_BODY);
#endif	// __ENCHANT0916
	
	for( int i = 0; i < 4; i++ )
	{
#ifdef __ENCHANT0916
		int nValue	= GetSetItemParts( adwParts[i] );
		if( nValue == 0 )
			return 0;
		if( nAbilityOption > nValue )
			nAbilityOption	= nValue;
#else	// __ENCHANT0916
		if( nAbilityOption != GetSetItemParts(adwParts[i]) )
		{
			return 0;
		}
#endif	// __EHCHANT0916
	}

	return nAbilityOption;
}

BOOL CMover::IsSetItemPart( DWORD dwParts )
{
	switch( dwParts )
	{
		case PARTS_UPPER_BODY:
		case PARTS_HAND:
		case PARTS_FOOT:
		case PARTS_CAP:
			return TRUE;
	}
	return FALSE;
}

void CMover::SetSetItemAvail( int nAbilityOption )
{
	PSETITEMAVAIL pSetItemAvail		= prj.GetSetItemAvail( nAbilityOption );
	if( pSetItemAvail )
	{
		OutputDebugString( "SetSetItemAvail" );
		if( pSetItemAvail->nHitRate != 0 )
			SetDestParam( DST_ADJ_HITRATE, pSetItemAvail->nHitRate, NULL_CHGPARAM );
		if( pSetItemAvail->nBlock != 0 )
		{
			SetDestParam( DST_BLOCK_RANGE, pSetItemAvail->nBlock, NULL_CHGPARAM );
			SetDestParam( DST_BLOCK_MELEE, pSetItemAvail->nBlock, NULL_CHGPARAM );
		}
		if( pSetItemAvail->nMaxHitPointRate != 0 )
			SetDestParam( DST_HP_MAX_RATE, pSetItemAvail->nMaxHitPointRate, NULL_CHGPARAM );
		if( pSetItemAvail->nAddMagic != 0 )
			SetDestParam( DST_ADDMAGIC, pSetItemAvail->nAddMagic, NULL_CHGPARAM );
		if( pSetItemAvail->nAdded != 0 )
		{
			SetDestParam( DST_STR, pSetItemAvail->nAdded, NULL_CHGPARAM );
			SetDestParam( DST_DEX, pSetItemAvail->nAdded, NULL_CHGPARAM );
			SetDestParam( DST_INT, pSetItemAvail->nAdded, NULL_CHGPARAM );
			SetDestParam( DST_STA, pSetItemAvail->nAdded, NULL_CHGPARAM );
		}
	}
}

void CMover::ResetSetItemAvail( int nAbilityOption )
{
	PSETITEMAVAIL pSetItemAvail		= prj.GetSetItemAvail( nAbilityOption );
	if( pSetItemAvail )
	{
		OutputDebugString( "ResetSetItemAvail" );
		if( pSetItemAvail->nHitRate != 0 )
			ResetDestParam( DST_ADJ_HITRATE, pSetItemAvail->nHitRate, TRUE );
		if( pSetItemAvail->nBlock != 0 )
		{
			ResetDestParam( DST_BLOCK_RANGE, pSetItemAvail->nBlock, TRUE );
			ResetDestParam( DST_BLOCK_MELEE, pSetItemAvail->nBlock, TRUE );
		}
		if( pSetItemAvail->nMaxHitPointRate != 0 )
			ResetDestParam( DST_HP_MAX_RATE, pSetItemAvail->nMaxHitPointRate, TRUE );
		if( pSetItemAvail->nAddMagic != 0 )
			ResetDestParam( DST_ADDMAGIC, pSetItemAvail->nAddMagic, TRUE );
		if( pSetItemAvail->nAdded != 0 )
		{
			ResetDestParam( DST_STR, pSetItemAvail->nAdded, TRUE );
			ResetDestParam( DST_DEX, pSetItemAvail->nAdded, TRUE );
			ResetDestParam( DST_INT, pSetItemAvail->nAdded, TRUE );
			ResetDestParam( DST_STA, pSetItemAvail->nAdded, TRUE );
		}
	}
}
#endif	// __J0608

#ifdef __PIERCING_SM_TIME
void CMover::SetDestParamPiercingAvail( CItemElem* pItemElem )
{
	PIERCINGAVAIL piercingAvail;
	memset( &piercingAvail, 0, sizeof(piercingAvail) );

	pItemElem->GetPiercingAvail( &piercingAvail );
	for( int i = 0; i < piercingAvail.nSize; i++ )
		SetDestParam( piercingAvail.anDstParam[i], piercingAvail.anAdjParam[i], NULL_CHGPARAM );
}

void CMover::ResetDestParamPiercingAvail( CItemElem* pItemElem )
{
	PIERCINGAVAIL piercingAvail;
	memset( &piercingAvail, 0, sizeof(piercingAvail) );

	pItemElem->GetPiercingAvail( &piercingAvail );
	for( int i = 0; i < piercingAvail.nSize; i++ )
		ResetDestParam( piercingAvail.anDstParam[i], piercingAvail.anAdjParam[i], TRUE );
}

BOOL CMover::Pierce( CItemElem *pSuit, DWORD dwItemId )
{
	for( int i = 0; i < pSuit->m_piercingInfo.nPiercedSize; i++ )
	{
		if( pSuit->m_piercingInfo.adwItemId[i] == 0 )
		{
			pSuit->m_piercingInfo.adwItemId[i]	= dwItemId;
			UpdateItem( pSuit->m_dwObjId, UI_PIERCING, MAKELONG( i, dwItemId ) );
			return TRUE;
		}

	}
	return FALSE;
}
#endif	// __PIERCING_SM_TIME

#ifdef __CHEER0615
#ifdef __WORLDSERVER
void CMover::CheckTickCheer( void )
{
	if( FALSE == IsPlayer() )
		return;

	if( m_nCheerPoint >= MAX_CHEERPOINT )
		return;

	DWORD dwTickCount	= GetTickCount();
	if( dwTickCount > m_dwTickCheer )
		SetCheerParam( m_nCheerPoint + 1, dwTickCount, TICK_CHEERPOINT );
}
#endif	// __WORLDSERVER

void CMover::SetCheerParam( int nCheerPoint, DWORD dwTickCount, DWORD dwRest )
{
	m_nCheerPoint	= nCheerPoint;
	m_dwTickCheer	= dwTickCount + dwRest;
#ifdef __WORLDSERVER
	( (CUser*)this )->AddSetCheerParam( nCheerPoint, dwRest );
#endif	// __WORLDSERVER
}
#endif	// __CHEER0615

#ifdef  __V050322_CACHEQUEST
BOOL CMover::NoDisguise( LPDIRECT3DDEVICE9 pd3dDevice )     
{ 
#ifdef __CLIENT
	// 이미 사람 모양이므로 해제 처리 할 필요 없음. 
	if( GetIndex() == MI_MALE || GetIndex() == MI_FEMALE )
		return FALSE;
	// 기존 모델 파괴 
	DWORD dwIndex = (GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	SetIndex( pd3dDevice, dwIndex );
#endif  //__CLIENT
	return TRUE;
}
BOOL CMover::Disguise( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex )
{
#ifdef __CLIENT
	return SetIndex( pd3dDevice, dwMoverIndex );
#endif // __CLIENT
	return TRUE;
}
BOOL CMover::SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex, BOOL bInitProp )
{ 
	MoverProp* pMoverProp = prj.GetMoverProp( dwMoverIndex );
	if( pMoverProp == NULL )
		return FALSE;
	// 기존 모델 파괴 
	if( m_pModel && m_pModel->IsAniable() )
		SAFE_DELETE( m_pModel );
	SetTypeIndex( pd3dDevice, OT_MOVER, dwMoverIndex, bInitProp );//
	m_dwMotion = m_dwMotionOld = -1;
	SetMotion( MTI_STAND );
	// 이런건 프로퍼티를 이용하는게 좋다.
	if( dwMoverIndex == MI_MALE || dwMoverIndex == MI_FEMALE )
	{
	#if defined( __CLIENT )// || defined( __BEAST )
		RedoEquip( !IsActiveMover() );
	#else
		RedoEquip( FALSE );
	#endif
		SetHairColor( m_dwHairColor );
	}
	ResetScale();
	UpdateLocalMatrix();
	return TRUE;
}
BOOL CMover::IsDisguise()
{
	if( IsPlayer() )
	{
		return ( GetIndex() != MI_MALE && GetIndex() != MI_FEMALE );
	}
	return FALSE;
}
#endif // __V050322_CACHEQUEST

LPCTSTR CMover::GetName( BOOL bNickname )
{
	if( !bNickname )
		return (LPCTSTR)m_szName;

	if( m_dwAuthorization >= AUTH_GAMEMASTER && ( GetIndex() == MI_CROWNIBLIS || GetIndex() == MI_CROWNSHADE || GetIndex() == MI_CROWNBUBBLE ) )
	{
		MoverProp* pMoverProp	= GetProp();
		if( pMoverProp )
			return (LPCTSTR)pMoverProp->szName;
	}
	return (LPCTSTR)m_szName;
}

#ifdef __SETITEM0624
void CMover::SetDestParamSetItem( CItemElem* pItemElem )
{
	ITEMAVAIL itemAvail;
	memset( &itemAvail, 0, sizeof(itemAvail) );

#ifdef __FINITEITEM0705
	if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
#else	// __FINITEITEM0705
	if( pItemElem )
#endif	// __FINITEITEM0705
	{
		CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemElem->m_dwItemId );
		if( pSetItem )
		{
			int nEquiped	= GetEquipedSetItemNumber( pSetItem );
			pSetItem->GetItemAvail( &itemAvail, nEquiped, FALSE );
		}
	}
	else
	{
		map<CSetItem*, int>	mapSetItem;
		map<CSetItem*, int>::iterator i;
		for( int nParts = 0; nParts < MAX_HUMAN_PARTS; nParts++ )
		{
			pItemElem	= GetEquipItem( nParts );
#ifdef __FINITEITEM0705
			if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
#else	// __FINITEITEM0705
			if( pItemElem )
#endif	// __FINITEITEM0705
			{
				CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemElem->m_dwItemId );
				if( pSetItem )
				{
					i	= mapSetItem.find( pSetItem );
					if( i != mapSetItem.end() )
						i->second++;
					else
						mapSetItem.insert( map<CSetItem*, int>::value_type( pSetItem, 1 ) );
				}
			}
		}
		for( i = mapSetItem.begin(); i != mapSetItem.end(); ++i )
		{
			CSetItem* pSetItem	= i->first;
			int nEquiped	= i->second;
			pSetItem->GetItemAvail( &itemAvail, nEquiped, TRUE );
		}
	}
		
	for( int i = 0; i < itemAvail.nSize; i++ )
		SetDestParam( itemAvail.anDstParam[i], itemAvail.anAdjParam[i], NULL_CHGPARAM );
}

void CMover::ResetDestParamSetItem( CItemElem* pItemElem )
{
#ifdef __FINITEITEM0705
	if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
#else	// __FINITEITEM0705
	if( pItemElem )
#endif	// __FINITEITEM0705
	{
		CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItemByItemId( pItemElem->m_dwItemId );
		if( pSetItem )
		{
			ITEMAVAIL itemAvail;
			memset( &itemAvail, 0, sizeof(itemAvail) );
			int nEquiped	= GetEquipedSetItemNumber( pSetItem );
			pSetItem->GetItemAvail( &itemAvail, nEquiped + 1, FALSE );
			for( int i = 0; i < itemAvail.nSize; i++ )
				ResetDestParam( itemAvail.anDstParam[i], itemAvail.anAdjParam[i], TRUE );				
		}
	}
}

int CMover::GetEquipedSetItemNumber( CSetItem* pSetItem )
{
	int nEquiped	= 0;
	for( int i = 0; i < pSetItem->m_nElemSize; i++ )
	{
		CItemElem* pItemElem	= GetEquipItem( pSetItem->m_anParts[i] );
#ifdef __FINITEITEM0705
		if( pItemElem && pItemElem->m_dwItemId == pSetItem->m_adwItemId[i] && !pItemElem->IsFlag( CItemElem::expired ) )
#else	// __FINITEITEM0705
		if( pItemElem && pItemElem->m_dwItemId == pSetItem->m_adwItemId[i] )
#endif	// __FINITEITEM0705
			nEquiped++;
	}
	return nEquiped;
}

void CMover::GetEquipedSetItem( int nSetItemId, BOOL* pbEquiped, int* pnEquip )
{
	*pnEquip	= 0;
//	memset( pbEquiped, 0, sizeof(BYTE) * MAX_HUMAN_PARTS );
	CSetItem* pSetItem	= CSetItemFinder::GetInstance()->GetSetItem( nSetItemId );
	if( pSetItem )
	{
		for( int i = 0; i < pSetItem->m_nElemSize; i++ )
		{
			CItemElem* pItemElem	= GetEquipItem( pSetItem->m_anParts[i] );
#ifdef __FINITEITEM0705
			if( pItemElem && pItemElem->m_dwItemId == pSetItem->m_adwItemId[i] && !pItemElem->IsFlag( CItemElem::expired ) )
#else	// __FINITEITEM0705
			if( pItemElem && pItemElem->m_dwItemId == pSetItem->m_adwItemId[i] )
#endif	// __FINITEITEM0705
			{
				pbEquiped[i]	= TRUE;
				(*pnEquip)++;
			}
		}
	}
}
#endif	// __SETITEM0624

#ifdef __RANDOMOPTITEM0628
void CMover::SetDestParamRandomOpt( CItemElem* pItemElem )
{
	if( pItemElem->m_nRandomOptItemId > 0 )
	{
		PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pItemElem->m_nRandomOptItemId );
		if( pRandomOptItem )
		{
			for( int i = 0; i < pRandomOptItem->ia.nSize; i++ )
				SetDestParam( pRandomOptItem->ia.anDstParam[i], pRandomOptItem->ia.anAdjParam[i], NULL_CHGPARAM );
		}
	}
}

void CMover::ResetDestParamRandomOpt( CItemElem* pItemElem )
{
	if( pItemElem->m_nRandomOptItemId > 0 )
	{
		PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( pItemElem->m_nRandomOptItemId );
		if( pRandomOptItem )
		{
			for( int i = 0; i < pRandomOptItem->ia.nSize; i++ )
				ResetDestParam( pRandomOptItem->ia.anDstParam[i], pRandomOptItem->ia.anAdjParam[i], TRUE );
		}
	}
}
#endif	// __RANDOMOPTITEM0628


#ifdef __CLIENT
// pModel를 인자로 받은 이유는....글로벌모델을 사용하는 넘들이 있어서...
void CMover::OverCoatItemRenderCheck( CModelObject* pModel )
{
#ifdef __YPARTS_EX
	if( !pModel )
		return;
	
	// 외투의상을 입었다면 주방어구 의상은 숨긴다...			
	int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
	int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
	
	O3D_ELEMENT *pElem = NULL;
	O3D_ELEMENT *pElem2 = NULL;
	
	for( int j=0; j<5; j++ )
	{
		pElem = pModel->GetParts( nArryEquip1[j] );
		
		if( !pElem )
			continue;

		pElem2 = pModel->GetParts( nArryEquip2[j] );
			
		if( pElem2 )
		{
			if( IsActiveMover() )
			{
				CItemElem* pItemElemOvercoat	= GetEquipItem( PARTS_HAT );

				if( pItemElemOvercoat )
				{
					if( pItemElemOvercoat->IsFlag( CItemElem::expired ) )
					{
						pElem->m_nEffect &= ~XE_HIDE;
						pElem2->m_nEffect |= XE_HIDE;
						continue;
					}
				}
			}
			else
			{
		#ifdef __FINITEITEM0705
				DWORD dwId	= m_aEquipInfo[PARTS_HAT].dwId;
		#else	// __FINITEITEM0705
				DWORD dwId	= m_adwEquipment[PARTS_HAT];
		#endif	// __FINITEITEM0705

				if( (m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
				{
					pElem->m_nEffect &= ~XE_HIDE;
					pElem2->m_nEffect |= XE_HIDE;					
					continue;
				}
			}

			pElem->m_nEffect |= XE_HIDE;
		}
	}
#endif //__YPARTS_EX		
}
#endif	// __CLIENT

#ifdef __WORLDSERVER
#ifdef __INSTANT_BANK0831
void CMover::ProcInstantBanker( void )
{
	if( m_dwTickCreated && m_dwIndex == MI_INFO_PENG )
	{
		LPCHARACTER lpCharacter	= GetCharacter();
		if( lpCharacter && lstrcmp( lpCharacter->m_szKey, "MaFl_InstantBank" ) == 0 )
		{
			if( GetTickCount() > m_dwTickCreated + TIMEWAIT_INSTANT_BANKER )
				Delete();
		}
	}
}
#endif	// __INSTANT_BANK0831
#endif	// __WORLDSERVER

CItemBase*	CMover::GetVendorItem()
{
	return NULL;
}

BOOL CMover::IsDropable( CItemElem* pItemElem, BOOL bPK )
{
	return ( IsDropableState( bPK )
		&& IsUsableItem( pItemElem )
		&& !pItemElem->IsBinds()
		&& !( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == II_RID_RID_BOR_LADOLF )
		&& !( pItemElem->GetProp()->dwItemKind3 == IK3_CLOAK && pItemElem->m_idGuild )
		&& !pItemElem->IsCharged()
		&& !m_Inventory.IsEquip( pItemElem->m_dwObjId ) );
}