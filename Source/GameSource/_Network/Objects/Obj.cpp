#include "stdafx.h"
#include "..\_Network\Objects\Obj.h"
#include "../_Common/FLCommonFunction.h"

extern	CProject	prj;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CObj::CObj()
#ifdef __LAYER_1015
:
m_nLayer( 0 )
#endif	// __LAYER_1015
{
	m_dwType		=	m_dwIndex	= m_dwMotion	= 0;
	m_vPos	= D3DXVECTOR3( 0, 0, 0 );
	m_vScale	= D3DXVECTOR3( 1, 1, 1 );
	m_fAngle	= 0;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CCtrl::CCtrl()
{
	m_dwType	= OT_CTRL;
	m_objid		= NULL_ID;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef __CORESERVER

CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
}

CItem::~CItem()
{
	SAFE_DELETE( m_pItemBase );
}

#endif // __CORESERVER

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
//#ifdef __DBSERVER
#if defined(__DBSERVER)
MEMPOOLER_IMPLEMENT( CMover, 1024 );
#endif	// __DBSERVER

CMover::CMover()
{
	Init();
}

CMover::~CMover()
{
	SAFE_DELETE( m_pActMover );

	SAFE_DELETE_ARRAY( m_aQuest ); 
	SAFE_DELETE_ARRAY( m_aCompleteQuest );
	SAFE_DELETE_ARRAY( m_aCheckedQuest );
}

void	CMover::Init()
{
	m_buffs.SetMover( this );

	memset( m_aJobSkill, 0, sizeof(m_aJobSkill) );
	m_dwBelligerence	= 0;
	m_dwMute	= 0;
	m_dwReturnWorldID	= 0;
	m_idGuildCloak	= 0;
	memset( m_szName, 0, sizeof(m_szName) );
	m_vMarkingPos	= D3DXVECTOR3( 0, 0, 0 );
	m_vReturnPos	= D3DXVECTOR3( 0, 0, 0 );
	m_dwType		= OT_MOVER;
	m_bPlayer	= FALSE;
	m_idPlayer	= (u_long)0xffffffff;
	//	memset( m_szName, 0, sizeof(m_szName) );
	memset( m_szCharacterKey, 0, sizeof(m_szCharacterKey ) );
	m_nHitPoint		= m_nManaPoint	= m_nFatiguePoint	= 0;
	m_pActMover		= new CActionMover( this );
	m_Inventory.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_INVENTORY, MAX_INVENTORY, MAX_HUMAN_PARTS );

	m_nDBDataSlot = 0;
	for( int i = 0 ; i < MAX_CHARACTER_SLOT ; ++i )
	{
		m_Bank[i].SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_USER_BANK, MAX_BANK ) ;
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
	m_nPlusMaxHitPoint = 0;
	m_nAttackResistLeft = (BYTE)0xff;
	m_nAttackResistRight = (BYTE)0xff;
	m_nDefenseResist = (BYTE)0xff;
	memset( m_dwSMTime, 0, sizeof( m_dwSMTime ) );

	ClearEquipInfo();
	m_dwSkinSet	= 0;//SKINSET_00;
	m_dwHairMesh	= 0;//HAIRMESH_00;
	m_dwHairColor	= 0xffffffff;
	m_dwHeadMesh	= 0;
	m_nTutorialState	= 0;
	m_nFxp			= 0;
	//	m_nTxp			= 0;
	m_bySex		= SEX_FEMALE;
	m_dwRideItemIdx		= 0;

	m_dwGold	= 0;

	m_nJob	= -1;

	m_Stat.InitializeOriginStat();
	m_nLevel	= 1;
	m_nExp1	= m_nDeathExp	= 0;
	m_nDeathLevel = 0;

	m_idMarkingWorld	= NULL_ID;

	m_dwAuthorization	= AUTH_GENERAL;
	m_dwMode	= 0;
	m_dwStateMode = 0;
	m_dwUseItemId = 0;

	m_aQuest = new QUEST[ MAX_QUEST ]; 
	m_aCompleteQuest = new WORD[ MAX_COMPLETE_QUEST ];
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
	memset( m_aQuest, 0, sizeof(QUEST) * MAX_QUEST );
	memset( m_aCompleteQuest, 0, sizeof(WORD) * MAX_COMPLETE_QUEST );

	m_aCheckedQuest = new WORD[ MAX_CHECKED_QUEST ];
	m_nCheckedQuestSize = 0;
	memset( m_aCheckedQuest, 0, sizeof(WORD) * MAX_CHECKED_QUEST );

	m_idparty = 0;
	m_idGuild	= 0;
	m_idWar	= 0;
	m_dwPKTime			= 0;			/// 핑크 상태 시간
	m_nPKValue			= 0;			/// PK 수치
	m_dwPKPropensity	= 0;			/// PK 성향
	m_dwPKExp			= 0;			/// PK 성향 습득 경험치

	m_nAngelExp			= 0;			/// 엔젤 경험치
	m_nAngelLevel		= 0;			/// 엔젤 Level

	m_nFame		= 0;
	m_idMurderer	= 0;
#ifdef __DBSERVER
	m_dwWorldID		= 1;
#endif // __DBSERVER
	memset( m_szBankPass, 0, sizeof( m_szBankPass ) );

	m_nDuel	= 0;
	m_idDuelParty	= 0;
	m_nFuel		= -1;
	m_bActiveAttack		= FALSE;
	m_nMovePattern = m_nMoveEvent = m_nMoveEventCnt = 0;
	m_tmAccFuel = 0;
	m_dwPeriodicTick	= 0;
	m_dwTickCheer	= 0;
	m_nCheerPoint	= 0;
	m_nGuildCombatState = 0;
	m_nSkillLevel = 0;
	m_nSkillPoint = 0;

#ifdef __DBSERVER
	m_nEventFlag = 0;
	m_dwEventTime	= 0;
	m_dwEventElapsed	= 0;
	m_bLogout	= FALSE;
	*m_szAccount	= '\0';
#endif	// __DBSERVER

#ifdef __SKILL_0205
	memset( m_abUpdateSkill, 0, sizeof(m_abUpdateSkill)  );		// 쿼리 실행해야하는가?
#endif	// __SKILL_0205

	m_dwPetId	= NULL_ID;
	m_fSpeedFactor	= 1.0F;

	m_nExpLog = 0;
	m_nAngelExpLog = 0;

	m_nCoupon = 0;

	m_dwMute	= 0;

	m_nHonor = -1;					// 달인선택 
	memset( m_aHonorTitle, 0, sizeof( m_aHonorTitle ) );// 달인수치

	m_dwPCBangClass = 0;

	m_objIdVisPet = NULL_ID;
	m_dwMoverSfxId = NULL_ID;

	m_idCampus = 0;
	m_nCampusPoint = 0;

	m_nRestPoint = 0;
	m_tLogOut = 0;
#ifdef __DBSERVER
	memset( m_adjParamAry, 0, sizeof( LONG ) * MAX_ADJPARAMARY );

	for( int Nth = 0; Nth < MAX_ADJPARAMARY; ++Nth )
	{
		m_chgParamAry[ Nth ] = 0x7FFFFFFF;
	}

#endif // __DBSERVER
}

void CMover::InitProp( void )
{
#ifdef __DBSERVER
	MoverProp* pMvrProp	= GetProp();
	if( pMvrProp )
	{
		if( FALSE == m_bPlayer )
			FLStrcpy( m_szName, _countof( m_szName ), pMvrProp->szName );

		memset( m_aJobSkill, 0, sizeof(m_aJobSkill) );

		LPSKILL lpSkill;
		for( int i = 0; i < MAX_SKILL_JOB; i++ )
		{
			m_aJobSkill[ i ].dwSkill = NULL_ID;
		}
		
		if( m_nJob != -1 ) 
		{
			SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
			int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
			for( int i = 0; i < nJobNum; i++ )
			{
				SkillProp* pSkillProp = apSkillProp[ i ];
				lpSkill = &m_aJobSkill[ i ];
				lpSkill->dwSkill = pSkillProp->dwID;
			}
		}
		m_nHitPoint		= 77; //GetMaxHitPoint();
		m_nManaPoint	= 77; //GetMaxManaPoint();
		m_nFatiguePoint	= 77; //GetMaxFatiguePoint();
		memset( m_szBankPass, 0, sizeof( m_szBankPass ) );
		m_dwBelligerence	= pMvrProp->dwBelligerence;
	}
#endif	// __DBSERVER
}

////////////////////////////////////////////////////////////////////////////////
void CMover::Copy( CMover * pMover, BOOL bAll )
{
	m_nDBDataSlot		= pMover->m_nDBDataSlot;
	m_dwIndex			= pMover->m_dwIndex;
	m_idPlayer			= pMover->m_idPlayer;
	m_dwAuthorization	= pMover->m_dwAuthorization;
	m_idGuild			= pMover->m_idGuild;
	m_idWar				= pMover->m_idWar;
	m_dwMode			= pMover->m_dwMode;
	FLStrcpy( m_szBankPass, _countof( m_szBankPass ), pMover->m_szBankPass );
	m_dwPeriodicTick	= pMover->m_dwPeriodicTick;
	m_RTMessenger		= pMover->m_RTMessenger;

	m_buffs				= pMover->m_buffs;

	m_dwPCBangClass		= pMover->m_dwPCBangClass;
	m_idCampus			= pMover->m_idCampus;
	m_nCampusPoint		= pMover->m_nCampusPoint;

	m_bLogout			= pMover->m_bLogout;

	FLStrcpy( m_szAccount, _countof( m_szAccount ), pMover->m_szAccount );

	if( bAll )
	{
		m_Inventory.Copy( pMover->m_Inventory );
		m_Pocket.Copy( pMover->m_Pocket );
		memcpy( m_aEquipInfo, pMover->m_aEquipInfo, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
		memcpy( m_UserTaskBar.m_aSlotApplet, pMover->m_UserTaskBar.m_aSlotApplet, sizeof(m_UserTaskBar.m_aSlotApplet) );
		memcpy( m_UserTaskBar.m_aSlotItem, pMover->m_UserTaskBar.m_aSlotItem, sizeof(m_UserTaskBar.m_aSlotItem) );
		memcpy( m_UserTaskBar.m_aSlotQueue, pMover->m_UserTaskBar.m_aSlotQueue, sizeof(m_UserTaskBar.m_aSlotQueue) );

		if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
		{
			m_UserTaskBar.m_nActionPoint	= pMover->m_UserTaskBar.m_nActionPoint;
		}

		FLStrcpy( m_szName, _countof( m_szName ), pMover->m_szName );
		m_vScale						= pMover->m_vScale;
		m_dwMotion						= pMover->m_dwMotion;
#ifdef __DBSERVER
		m_dwWorldID						= pMover->m_dwWorldID;
#endif	// __DBSERVER
#ifdef __LAYER_1015
		SetLayer( pMover->GetLayer() );
#endif	// __LAYER_1015
		m_vPos							= pMover->m_vPos;
		m_fAngle						= pMover->m_fAngle;
		FLStrcpy( m_szCharacterKey, _countof( m_szCharacterKey ), pMover->m_szCharacterKey );
		m_nHitPoint						= pMover->m_nHitPoint;
		m_nManaPoint					= pMover->m_nManaPoint;
		m_nFatiguePoint					= pMover->m_nFatiguePoint;
		m_dwSkinSet						= pMover->m_dwSkinSet;
		m_dwHairMesh					= pMover->m_dwHairMesh;
		m_dwHairColor					= pMover->m_dwHairColor;
		m_dwHeadMesh					= pMover->m_dwHeadMesh;
		SetSex( pMover->GetSex() );
		m_dwRideItemIdx					= pMover->m_dwRideItemIdx;
		SetGold( pMover->GetGold() );
		m_nJob							= pMover->m_nJob;
		m_idparty						= pMover->m_idparty;

		m_dwPKTime						= pMover->m_dwPKTime;
		m_nPKValue						= pMover->m_nPKValue;
		m_dwPKPropensity				= pMover->m_dwPKPropensity;
		m_dwPKExp						= pMover->m_dwPKExp;

		m_nAngelExp						= pMover->m_nAngelExp;
		m_nAngelLevel					= pMover->m_nAngelLevel;

		m_idMurderer					= pMover->m_idMurderer;
		m_nFame							= pMover->m_nFame;
		m_pActMover->m_dwState			= pMover->m_pActMover->GetState();
		m_pActMover->m_dwStateFlag		= pMover->m_pActMover->GetStateFlag();
		m_Stat							= pMover->m_Stat;
		m_nLevel						= pMover->m_nLevel;
		m_nExp1							= pMover->m_nExp1;
		m_nFuel							= pMover->m_nFuel;
		m_tmAccFuel						= pMover->m_tmAccFuel;
		m_nSkillLevel					= pMover->m_nSkillLevel;
		m_nSkillPoint					= pMover->m_nSkillPoint;
		
		m_idMarkingWorld				= pMover->m_idMarkingWorld;
		m_vMarkingPos					= pMover->m_vMarkingPos;

		m_nTutorialState				= pMover->m_nTutorialState;

		m_nFxp							= pMover->m_nFxp;
		m_bPlayer						= TRUE;
		m_nDeathExp						= pMover->m_nDeathExp;
		m_nDeathLevel					= pMover->m_nDeathLevel;
		memcpy( m_aJobSkill, pMover->m_aJobSkill, sizeof(m_aJobSkill) );
#ifdef __SKILL_0205
		memcpy( m_abUpdateSkill, pMover->m_abUpdateSkill, sizeof(m_abUpdateSkill) );
#endif	// __SKILL_0205
		memcpy( m_aQuest, pMover->m_aQuest, sizeof(QUEST) * MAX_QUEST );
		m_nQuestSize					= pMover->m_nQuestSize;
		memcpy( m_aCompleteQuest, pMover->m_aCompleteQuest, sizeof(WORD) * MAX_COMPLETE_QUEST );
		m_nCompleteQuestSize			= pMover->m_nCompleteQuestSize;

		memcpy( m_aCheckedQuest, pMover->m_aCheckedQuest, sizeof(WORD) * MAX_CHECKED_QUEST );
		m_nCheckedQuestSize				= pMover->m_nCheckedQuestSize;

		memcpy( m_dwSMTime, pMover->m_dwSMTime, sizeof(m_dwSMTime) );
		m_nPlusMaxHitPoint				= pMover->m_nPlusMaxHitPoint;
		m_nAttackResistLeft				= pMover->m_nAttackResistLeft;
		m_nAttackResistRight			= pMover->m_nAttackResistRight;
		m_nDefenseResist				= pMover->m_nDefenseResist;

		m_dwReturnWorldID				= pMover->m_dwReturnWorldID;
		m_vReturnPos					= pMover->m_vReturnPos;
		
		for( int i = 0; i < MAX_CHARACTER_SLOT; i++ )
		{
			m_Bank[i].Copy( pMover->m_Bank[i] );
		}
		memcpy( m_idPlayerBank, pMover->m_idPlayerBank, sizeof(m_idPlayerBank) );
		memcpy( m_dwGoldBank, pMover->m_dwGoldBank, sizeof( m_dwGoldBank) );

#ifdef __DBSERVER
		m_nEventFlag					= pMover->m_nEventFlag;
		m_dwEventTime					= pMover->m_dwEventTime;
		m_dwEventElapsed				= pMover->m_dwEventElapsed;
#endif	// __DBSERVER

		SetPetId( pMover->GetPetId() );
		m_fSpeedFactor					= pMover->m_fSpeedFactor;

		m_nExpLog						= pMover->m_nExpLog;
		m_nAngelExpLog					= pMover->m_nAngelExpLog;

		m_nCoupon						= pMover->m_nCoupon;

		m_dwMute						= pMover->m_dwMute;

		m_nHonor						= pMover->m_nHonor;
		memcpy( m_aHonorTitle, pMover->m_aHonorTitle, sizeof(int) * MAX_HONOR_TITLE );

		m_nRestPoint					= pMover->m_nRestPoint;
		m_tLogOut						= pMover->m_tLogOut;

		m_kMadrigalGiftPoint.nPoint	= pMover->m_kMadrigalGiftPoint.nPoint;		
	}
}

void CMover::PeriodTick( void )
{
	char ch	= m_dwPeriodicTick == 0 ? 'O': 'P';
	BOOL bPut	= ( ch != 'P' );
	DWORD dwTick	= GetTickCount();
	if( !bPut )
	{
		if( m_dwPeriodicTick < dwTick )
			bPut	= TRUE;
	}
	if( bPut )
		m_dwPeriodicTick	= dwTick + MIN( 120 );
}


BOOL CMover::RemoveQuest( int nQuestId )
{
	BOOL	bRemove	= FALSE;
	for( size_t i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			for( ; i < m_nQuestSize - 1; i++ )
				m_aQuest[ i ] = m_aQuest[ i + 1 ];
			m_nQuestSize--;
			bRemove	= TRUE;
			break;
		}
	}
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCompleteQuestSize - 1; i++ )
				m_aCompleteQuest[ i ] = m_aCompleteQuest[ i + 1 ];
			m_nCompleteQuestSize--;
			bRemove	= TRUE;
			break;
		}
	}

	for( size_t i = 0; i < m_nCheckedQuestSize; ++i )
	{
		if( m_aCheckedQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCheckedQuestSize - 1; ++i )
				m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
			m_aCheckedQuest[ --m_nCheckedQuestSize ] = 0;
			bRemove	= TRUE;
			break;
		}
	}

	return bRemove;
}

#if !defined(__CORESERVER)
// IK3로 삭제하기 예) IK3_CLOAK
BOOL CMover::RemoveItemIK3()
{
	FLItemElem* pItemElem;
	BOOL bRemove;
	BOOL bResult	= FALSE;

	typedef std::vector< CItemContainer<FLItemElem>* > Containers;
	Containers containers;
	containers.push_back( &m_Inventory );
	for( int loop = 0; loop<MAX_CHARACTER_SLOT; ++loop )
		containers.push_back( &m_Bank[loop] );
	for( int loop = 0; loop<MAX_CHARACTER_POCKET; ++loop )
		containers.push_back( &m_Pocket.m_kPocket[loop] );
	
	for( Containers::iterator it = containers.begin(); it != containers.end(); ++it )
	{
		for( DWORD i = 0 ; i < (*it)->GetMax(); i++ )
		{
			bRemove = FALSE;
			pItemElem	= (*it)->GetAtId( i );
			if( pItemElem )
			{
				PT_ITEM_SPEC pItemProp		= g_xSpecManager->GetSpecItem( pItemElem->m_dwItemId );	
				if( pItemProp )
				{
					if( pItemProp->dwItemKind3 == IK3_CLOAK )
					{
						if( pItemElem->m_idGuild != 0 )
						{
							if( m_idGuild != pItemElem->m_idGuild )
							{
								bRemove = TRUE;
							}
						}
					}
					else
						if( pItemProp->dwItemKind3 == IK3_DELETE )
						{
							bRemove = TRUE;
						}

						if( bRemove )
						{
							if( (*it) == &m_Inventory && (*it)->IsEquip( pItemElem->m_dwObjId ) )		
							{
								if( (*it)->UnEquip( pItemProp->dwParts ) ) // 장비 해제
								{
									m_aEquipInfo[pItemProp->dwParts].dwItemID	= 0;
									(*it)->RemoveAt( pItemElem->m_dwObjIndex );
									bResult	= TRUE;
								}
								else
								{
									FLINFO_LOG( PROGRAM_NAME, _T( "Not UnEquip RemoveItemIK3  : %s, %d, %d" ), m_szName, pItemElem->m_dwObjId, pItemProp->dwID );
								}
							}
							else
							{
								(*it)->RemoveAt( pItemElem->m_dwObjIndex );
								bResult	= TRUE;
							}
						}
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Not ItemProp RemoveItemIK3  : %s, %d" ), m_szName, pItemElem->m_dwObjId );
					if( (*it) == &m_Inventory && (*it)->IsEquip( pItemElem->m_dwObjId ) )	{
						if( (*it)->UnEquip( pItemElem->m_dwObjIndex ) )	{ // 장비 해제
							m_aEquipInfo[pItemElem->m_dwObjIndex].dwItemID	= 0;
						}
						else	{
							FLINFO_LOG( PROGRAM_NAME, _T( "Not UnEquip RemoveItemIK3  : %s, %d, %d" ), m_szName, pItemElem->m_dwObjId, pItemElem->m_dwItemId );
						}
					}
					(*it)->RemoveAt( pItemElem->m_dwObjIndex );
					bResult	= TRUE;
				}
			}
		}
	}
	return bResult;
}
#endif // !__CORESERVER


//////////////////////////////////////////////////////////////////////////
// mirchang_20100705
bool CMover::CheckUserGold( int nGold, bool bAdd )
{
	__int64 n64Gold = (__int64)nGold;

	if( n64Gold < 0 || n64Gold > INT_MAX )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "Invalid. UserID:[%07d], Gold:[%I64d]" ), m_idPlayer, n64Gold );
		return false;
	}

	__int64 n64UserGold = (__int64)m_dwGold;

	if( bAdd == true )
	{
		if( n64UserGold + n64Gold > INT_MAX )
		{
			return false;
		}
	}
	else
	{
		if( n64UserGold - n64Gold < 0 )
		{
			return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

FLItemElem* CMover::GetPetItem( void )
{
	return m_Inventory.GetAtId( m_dwPetId );
}

void CActionMover::__ForceSetState( DWORD dwState )
{
	m_dwState	= dwState;
}

int		CMover::GetUsableItemQuantityByItemID( const DWORD dwItemID ) const
{
	int nTotalQuantity	= 0;

	const PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( dwItemID );
	if( pItemProp == NULL )
	{
		return nTotalQuantity;
	}

	DWORD dwStart	= 0;
	DWORD dwEnd		= 0;
	if( m_Inventory.GetObjIndexRange( pItemProp, &dwStart, &dwEnd ) == false )
	{
		return nTotalQuantity;
	}

	for( DWORD Nth = dwStart; Nth < dwEnd; ++Nth )
	{
		FLItemElem* pItemElem = static_cast<FLItemElem*>( m_Inventory.GetAt( Nth ) );
		if( IsUsableItem( pItemElem ) == FALSE || pItemElem->m_dwItemId != dwItemID )
		{
			continue;
		}

		nTotalQuantity += pItemElem->m_nItemNum;
	}

	return nTotalQuantity;
}

#ifdef __DBSERVER

bool	CMover::InitializeJobSkill( const DWORD dwJob, const DWORD dwLevel )
{
	if( dwJob < JOB_VAGRANT || dwJob >= MAX_JOB )
	{
		return false;
	}

	const DWORD dwJobSkillCount			= FLCommonFunction::GetJobSkillCount( dwJob );
	if( dwJobSkillCount == NULL_ID )
	{
		return false;
	}

	const bool bMaster					= FLCommonFunction::IsMasterJob( dwJob );
	SkillProp** apSkillProp				= prj.m_aJobSkill[ dwJob ];
	const DWORD dwJobNum				= prj.m_aJobSkillNum[ dwJob ];

	if( dwJobNum + dwJobSkillCount > MAX_SKILL_JOB )
	{
		return false;
	}

	DWORD dwMasterSkillLevel			= 0;
	
	if( bMaster == true )
	{
		if( dwLevel <= 71 )
		{
			dwMasterSkillLevel			= 1;
		}
		else if( dwLevel > 71 && dwLevel <= 83 )
		{
			dwMasterSkillLevel			= 2;
		}
		else if( dwLevel > 83 && dwLevel <= 95 )
		{
			dwMasterSkillLevel			= 3;
		}
		else if( dwLevel > 95 && dwLevel <= 107 )
		{
			dwMasterSkillLevel			= 4;
		}
		else
		{
			dwMasterSkillLevel			= 5;
		}
	}

	for( DWORD Nth = 0; Nth < dwJobNum; ++Nth )
	{
		const SkillProp* pSkillProp		= apSkillProp[ Nth ];
		if( pSkillProp == NULL )
		{
			continue;
		}

		SKILL* const pSkill				= &m_aJobSkill[ Nth + dwJobSkillCount ];
		if( pSkill == NULL )
		{
			continue;
		}

		pSkill->dwSkill					= pSkillProp->dwID;

		if( bMaster == true && pSkillProp->dwSkillKind1 == JTYPE_MASTER )
		{
			pSkill->dwLevel				= dwMasterSkillLevel;
		}
	}

	return true;
}

bool	CMover::InitializePlayerSkill()
{
	for( DWORD Nth = 0; Nth < MAX_SKILL_JOB; ++Nth )
	{
		m_aJobSkill[ Nth ].dwSkill	= NULL_ID;
		m_aJobSkill[ Nth ].dwLevel	= 0;
	}

	DWORD dwSubLineJob	= m_nJob;
	const DWORD dwLevel	= ( m_nLevel >= m_nDeathLevel ) ? m_nLevel : m_nDeathLevel;

	do 
	{
		const bool bRet	= InitializeJobSkill( dwSubLineJob, dwLevel );
		if( bRet == false )
		{
			FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ FAILED InitializeJobSkill. PLAYER_ID(%07d), dwSubLineJob:(%u), dwLevel(%u) ]" )
				, m_idPlayer, dwSubLineJob, dwLevel );
		}

		dwSubLineJob	= FLCommonFunction::GetSubLineJob( dwSubLineJob );

	} while ( dwSubLineJob != NULL_ID );

	return true;
}

bool	CMover::InitializeSkillLevel()
{
	for( DWORD Nth = 0; Nth < MAX_SKILL_JOB; ++Nth )
	{
		if( m_aJobSkill[ Nth ].dwSkill == NULL_ID )
		{
			m_aJobSkill[ Nth ].dwLevel	= 0;
			continue;
		}

		const SkillProp* pSkillProp		= prj.GetSkillProp( m_aJobSkill[ Nth ].dwSkill );
		if( pSkillProp == NULL )
		{
			m_aJobSkill[ Nth ].dwSkill	= NULL_ID;
			m_aJobSkill[ Nth ].dwLevel	= 0;
			continue;
		}

		if( pSkillProp->dwSkillKind1 == JTYPE_MASTER || pSkillProp->dwSkillKind1 == JTYPE_HERO )
		{
			continue;
		}

		m_aJobSkill[ Nth ].dwLevel	= 0;
	}

	return true;
}

bool	CMover::CheckPlayerSkill() const
{
	std::vector< SKILL >	vecSubLineSkill;
	vecSubLineSkill.reserve( MAX_SKILL_JOB );
	vecSubLineSkill.clear();

	// 습득한 스킬 중 선행 스킬이 필요한 스킬의 정보를 따로 분리
	for( DWORD Nth = 0; Nth < MAX_SKILL_JOB; ++Nth )
	{
		const SKILL kLearnSkill			= m_aJobSkill[ Nth ];
		if( kLearnSkill.dwSkill == NULL_ID )
		{
			continue;
		}

		if( kLearnSkill.dwLevel <= 0 )
		{
			continue;
		}

		const SkillProp* pSkillProp		= prj.GetSkillProp( kLearnSkill.dwSkill );
		if( pSkillProp == NULL )
		{
			continue;
		}

		if( pSkillProp->dwReqSkill1 != NULL_ID )
		{
			SKILL kReqSubLineSkill;
			kReqSubLineSkill.dwSkill	= pSkillProp->dwReqSkill1;
			kReqSubLineSkill.dwLevel	= pSkillProp->dwReqSkillLevel1;
			vecSubLineSkill.push_back( kReqSubLineSkill );
		}

		if( pSkillProp->dwReqSkill2 != NULL_ID )
		{
			SKILL kReqSubLineSkill;
			kReqSubLineSkill.dwSkill	= pSkillProp->dwReqSkill2;
			kReqSubLineSkill.dwLevel	= pSkillProp->dwReqSkillLevel2;
			vecSubLineSkill.push_back( kReqSubLineSkill );
		}
	}

	if( vecSubLineSkill.empty() == true )
	{
		return true;
	}

	// 선행 스킬을 필요로 하는 스킬들.. 습득한 스킬들에 대해 선행 스킬 충족 조건을 체크.
	for( std::vector< SKILL >::const_iterator pos = vecSubLineSkill.begin(); pos != vecSubLineSkill.end(); ++pos )
	{
		const SKILL kSubLineSkill		= *pos;

		volatile bool bCheck			= false;

		for( DWORD Nth = 0; Nth < MAX_SKILL_JOB; ++Nth )
		{
			const SKILL kLearnSkill		= m_aJobSkill[ Nth ];

			if( kLearnSkill.dwSkill == NULL_ID || kLearnSkill.dwLevel <= 0 )
			{
				continue;
			}

			if( kLearnSkill.dwSkill == kSubLineSkill.dwSkill && kLearnSkill.dwLevel >= kSubLineSkill.dwLevel )
			{
				bCheck = true;
				break;
			}
		}

		if( bCheck == false )
		{
			return false;
		}
	}

	return true;
}

void	CMover::CorrectErrorsSkillPoint()
{
	const DWORD dwCurrentLevel			= ( m_nDeathLevel > m_nLevel ) ? m_nDeathLevel : m_nLevel;

	const int nCurrentSkillLevel		= FLCommonFunction::GetTotalSkillPoint( m_nJob, dwCurrentLevel );
	const int nUseSkillPoint			= GetUseSkillPoint();

	/************************************************************************/
	/*
	2012.08.20 수정
	이전에는 캐릭터가 가지고 있는 총 스킬포인트와 원래 가지고 있어야 할 총 스킬포인트가 다를 시 재분배.
	기존 스킬 숙련도 시스템을 이용하여 원래 가질 수 있는 스킬포인트보다 더 많이 가지고 있는 유저들이 존재 한다.
	포인트가 모자라면 보정을 해주고 넘칠땐 보정을 하지 않는다. 기존 데이터 유지.
	*/
	/************************************************************************/
	
	// 스킬 레벨(총 스킬 포인트)이 모자랄 경우 보정.
	if( m_nSkillLevel < nCurrentSkillLevel )
	{
		m_nSkillLevel					= nCurrentSkillLevel;

//		const int nRewardPoint = nTotalSkillPoint - nUseSkillPoint - m_nSkillPoint;
//		g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_GET_REWARD, nRewardPoint, this, NULL );
	}

	// 남은 스킬 포인트는 스킬 레벨에서 사용한 스킬 포인트를 뺀 나머지.
	const int nRemainSkillPoint			= m_nSkillLevel - nUseSkillPoint;
	m_nSkillPoint						= ( nRemainSkillPoint > 0 ) ? nRemainSkillPoint : 0;
}

int		CMover::GetUseSkillPoint() const
{
	int nUseSkillPoint = 0;

	for( int i = 0 ; i < MAX_SKILL_JOB ; ++i )
	{
		const SKILL* pSkill = &m_aJobSkill[ i ];
		if( pSkill != NULL && pSkill->dwSkill != NULL_ID )
		{
			const SkillProp* pSkillProp = prj.GetSkillProp( pSkill->dwSkill );
			if( pSkillProp != NULL )
			{
				if( pSkill->dwLevel > 0 && pSkillProp->dwSkillKind1 != JTYPE_MASTER && pSkillProp->dwSkillKind1 != JTYPE_HERO )
				{
					nUseSkillPoint += ( pSkill->dwLevel * prj.GetSkillPoint( pSkillProp ) ); 
				}
			}
		}
	}

	return nUseSkillPoint;
}

bool	CMover::CheckPlayerOriginStat()
{
	// 게임마스터 이상 권한인 경우는 스탯체크를 하지 않는다.
	if( m_dwAuthorization >= AUTH_GAMEMASTER )
	{
		return true;
	}

	const int OriginStr		= m_Stat.GetOriginStr();
	const int OriginSta		= m_Stat.GetOriginSta();
	const int OriginDex		= m_Stat.GetOriginDex();
	const int OriginInt		= m_Stat.GetOriginInt();

// 	if( OriginStr < DEFAULT_ORIGIN_STAT_VALUE || OriginSta < DEFAULT_ORIGIN_STAT_VALUE || OriginDex < DEFAULT_ORIGIN_STAT_VALUE || OriginInt < DEFAULT_ORIGIN_STAT_VALUE )
// 	{
// 		return false;
// 	}
// 
// 	const DWORD CurrentLevel	= ( m_nDeathLevel > m_nLevel ) ? m_nDeathLevel : m_nLevel;
// 
// 	const int TotalStatPoint	= FLCommonFunction::GetTotalStatPoint( m_nJob, CurrentLevel );
// 	if( TotalStatPoint < 0 )
// 	{
// 		return false;
// 	}

	const int MaxStatPoint		= 1000;		//TotalStatPoint + ( DEFAULT_ORIGIN_STAT_VALUE * 4 );

	if( OriginStr > MaxStatPoint || OriginSta > MaxStatPoint || OriginDex > MaxStatPoint || OriginInt > MaxStatPoint )
	{
		return false;
	}

// 	if( ( OriginStr + OriginSta + OriginDex + OriginInt + m_Stat.GetRemainStatPoint() ) > MaxStatPoint )
// 	{
// 		return false;
// 	}

	return true;
}

#endif // __DBSERVER
