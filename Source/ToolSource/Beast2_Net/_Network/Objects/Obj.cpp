#include "stdafx.h"
#include "..\_Network\Objects\Obj.h"
#ifdef __CHINA
 #include "..\DatabaseServerC\Project.h"
#else
 #include "..\DatabaseServer\Project.h"
#endif
extern	CProject	prj;

int		CObj::m_nMethod		= METHOD_NONE;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CObj::CObj()
{
	m_dwType		=	m_dwIndex	= m_dwMotion	= 0;
	m_vPos	= m_vAxis	= D3DXVECTOR3( 0, 0, 0 );
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
CItemBase::CItemBase()
{
	memset( m_szItemText, 0, sizeof(m_szItemText) );
	m_dwItemId = 0;
	m_dwObjId = NULL_ID;
	m_dwObjIndex	= NULL_ID;
	m_dwSerialNumber	= 0;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CItemElem::CItemElem()
{
	m_nItemNum	     = 1;
	m_idGuild        = 0;
	m_nAbilityOption = 0;
	m_nRepairNumber	 = 0;
	m_nHitPoint		 = 0;
	m_byFlag	     = 0;

	
    m_bItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;	
	m_nResistSMItemId = 0;

#ifdef __PIERCING_SM_TIME
	memset( &m_piercingInfo, 0x00, sizeof(m_piercingInfo) );
#endif	// __PIERCING_SM_TIME	

#ifdef __PIERCING_SM_TIME
	m_bCharged	= FALSE;
	m_dwKeepTime = 0;
#endif	// __PIERCING_SM_TIME

#ifdef __RANDOMOPTITEM0628
	m_nRandomOptItemId	= 0;
#endif	// __RANDOMOPTITEM0628
}


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
}

CItem::~CItem()
{
	SAFE_DELETE( m_pItemBase );
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
//#ifdef __DBSERVER
#if defined(__DBSERVER)
CMoverPool*	CMover::m_pPool	= new CMoverPool( 1024 );
#endif	// __DBSERVER

CMover::CMover()
{
	m_dwType		= OT_MOVER;
	m_bPlayer	= FALSE;
	m_idPlayer	= (u_long)0xffffffff;
//	memset( m_szName, 0, sizeof(m_szName) );
	memset( m_szCharacterKey, 0, sizeof(m_szCharacterKey ) );
	m_nHitPoint		= m_nManaPoint	= m_nFatiguePoint	= 0;
	m_pActMover		= new CActionMover( this );
	m_Inventory.SetItemContainer( ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS );

	m_nSlot = 0;
	for( int i = 0 ; i < 3 ; ++i )
	{
		m_Bank[i].SetItemContainer( ITYPE_ITEM, MAX_BANK ) ;
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
	m_nPlusMaxHitPoint = 0;
	m_nAttackResistLeft = (BYTE)0xff;
	m_nAttackResistRight = (BYTE)0xff;
	m_nDefenseResist = (BYTE)0xff;
	memset( m_dwSMTime, 0, sizeof( DWORD ) * SM_MAX );
	m_SkillState.Init();
	m_SkillState.SetMover( this );

#ifdef __FINITEITEM0705
	ClearEquipInfo();
#else	// __FINITEITEM0705
	memset( m_adwEquipment, 0xff, sizeof( m_adwEquipment ) );
#endif	// __FINITEITEM0705
	m_dwSkinSet	= 0;//SKINSET_00;
	m_dwHairMesh	= 0;//HAIRMESH_00;
	m_dwHairColor	= 0xffffffff;
	m_dwHeadMesh	= 0;
	m_nFlightLv		= 0;
	m_nFxp			= 0;
//	m_nTxp			= 0;
	m_bySex		= SEX_FEMALE;
	m_dwRideItemIdx		= 0;

  	m_dwGold	= 0;

	m_nJob	= -1;
	
	m_nStr	= m_nSta	= m_nDex	= m_nInt	= 15;
	m_nLevel	= 1;
	m_nExp1	= m_nExp2	= m_nDeathExp	= 0;
	m_nDeathLevel = 0;

	m_idMarkingWorld	= NULL_ID;

	m_dwAuthorization	= AUTH_GENERAL;
	m_dwMode	= 0;
#ifdef __S0504_FLY
	m_dwStateMode = 0;
	m_dwUseItemId = 0;
#endif // __S0504_FLY
	m_nRemainGP	= m_nRemainLP	= 0;
//#define __V050322_CACHEQUEST
#ifdef __V050322_CACHEQUEST
	m_aQuest = new QUEST[ MAX_QUEST ]; 
	m_aCompleteQuest = new WORD[ MAX_COMPLETE_QUEST ];
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
	memset( m_aQuest, 0, sizeof(QUEST) * MAX_QUEST );
	memset( m_aCompleteQuest, 0, sizeof(WORD) * MAX_QUEST );
#else // __V050322_CACHEQUEST
//{{AFX
	memset( m_aQuest, 0, sizeof(QUEST) * MAX_QUEST );
	m_nQuestSize	= 0;
//}}AFX
#endif //__V050322_CACHEQUEST
	m_idparty = 0;
	m_idGuild	= 0;
	m_idWar	= 0;
	m_nNumKill	= 0;
	m_nSlaughter	= 0;
	m_nFame		= 0;
	m_idMurderer	= 0;
#ifdef __DBSERVER
	m_dwWorldID		= 1;
#endif // __DBSERVER
	memset( m_szBankPass, 0, sizeof( m_szBankPass ) );


	m_nDuel	= 0;
//	m_idDuelOther	= 0;
	m_idDuelParty	= 0;

	m_nFuel		= -1;

	m_bActiveAttack		= FALSE;

#ifdef __XFLYAI0524
	m_nMovePattern = m_nMoveEvent = m_nMoveEventCnt = 0;
#endif
#ifdef __XACCFUEL0708
	m_tmAccFuel = 0;
#endif
	
#ifdef __XOPT0617
	m_nWeaponOption = 0;
#else	// __XOPT0617
	#ifndef __FINITEITEM0705
	memset( m_anEquipOption, 0, sizeof(m_anEquipOption) );
	#endif	// __FINITEITEM0705
#endif // // __XOPT0617

	m_dwSkillPoint = 0;

#ifdef __PERIODIC_0513
	m_dwPeriodicTick	= 0;
#endif	// __PERIODIC_0513

#ifdef __CHEER0615
	m_dwTickCheer	= 0;
	m_nCheerPoint	= 0;
#endif	// __CHEER0615

#ifdef __GUILDCOMBAT
	m_nGuildCombatState = 0;
#endif // __GUILDCOMBAT
}

CMover::~CMover()
{
	SAFE_DELETE( m_pActMover );

#ifdef __V050322_CACHEQUEST
	SAFE_DELETE_ARRAY( m_aQuest ); 
	SAFE_DELETE_ARRAY( m_aCompleteQuest );
#endif	// __V050322_CACHEQUEST
}

void CMover::InitProp( void )
{
#ifdef __DBSERVER
	MoverProp* pMvrProp	= GetProp();
	if( pMvrProp )
	{
		if( FALSE == m_bPlayer )
			_tcscpy( m_szName, pMvrProp->szName );

		for( int ii = 0 ; ii < MAX_JOB ; ii++ )
		{
			m_aJobLv[ii] = 1;
		}
		memset( m_aJobSkill, 0, sizeof(SKILL) * ( MAX_SKILL_JOB ) );

/*
		m_aJobLv[0]	= 15;
		m_aJobLv[1]	= 15;
		m_aJobLv[2]	= 15;
		m_aJobLv[3]	= 15;
		m_aJobLv[4]	= 15;
*/
		LPSKILL lpSkill;
		for( int i = 0; i < MAX_SKILL_JOB; i++ )
		{
			m_aJobSkill[ i ].dwSkill = NULL_ID;
			m_aJobSkill[ i ].dwLevel = 1;
		}
		if( m_nJob != -1 ) {
			ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
			int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
			for( i = 0; i < nJobNum; i++ )
			{
				ItemProp* pSkillProp = apSkillProp[ i ];
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

BOOL CMover::AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, DWORD dwSerialNumber, int nItemResist, int nResistAbilityOption, int nCharged, int nPiercedSize, DWORD adwItemId0, DWORD adwItemId1, DWORD adwItemId2, DWORD adwItemId3, DWORD m_dwKeepTime )
{
	if( dwSerialNumber == 0 )
		dwSerialNumber = xRand();
	
	return m_Inventory.Add( dwId, dwNum, nOption, dwSerialNumber, nItemResist, nResistAbilityOption, NULL, NULL, NULL, nCharged, nPiercedSize, adwItemId0, adwItemId1, adwItemId2, adwItemId3, m_dwKeepTime );
}

BOOL CMover::AddItemBank( int nSlot, DWORD dwId, DWORD dwNum, int nOption, DWORD dwSerialNumber )
{
	if( dwSerialNumber == 0 )
		dwSerialNumber = xRand();
	
	return m_Bank[nSlot].Add( dwId, dwNum, nOption, dwSerialNumber );
}

BOOL CMover::IsAbnormalPlayerData()
{
#ifndef __CORESERVER
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
	
	DWORD dwObjIndex;
	for( i = 0; i < nMax; i++ )
	{
		dwObjIndex	= m_Inventory.m_apItem[i].m_dwObjIndex;
		if( dwObjIndex != NULL_ID )
		{
			if( m_Inventory.m_apIndex[dwObjIndex] != i )
				return TRUE;
		}
		if( FALSE == m_Inventory.m_apItem[i].IsEmpty() )
		{
			ItemProp* pItemProp
				= prj.GetItemProp( m_Inventory.m_apItem[i].m_dwItemId );
			if( !pItemProp	/*It have not property*/
				|| pItemProp->dwItemKind3 == IK3_VIRTUAL/*It is monster's virtual weapon*/ )
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
#ifdef __FINITEITEM0705
		if( adwEquipment[i] != m_aEquipInfo[i].dwId )
#else	// __FINITEITEM0705
		if( adwEquipment[i] != m_adwEquipment[i] )
#endif	// __FINITEITEM0705
			return TRUE;
	}
#endif // __CORESERVER	
	return FALSE;
}

BOOL CMover::NormalizePlayerData()
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
	
	DWORD dwObjIndex;
	for( i = 0; i < nMax; i++ )
	{
		dwObjIndex	= m_Inventory.m_apItem[i].m_dwObjIndex;
		if( dwObjIndex != NULL_ID )
		{
			if( m_Inventory.m_apIndex[dwObjIndex] != i )
				m_Inventory.m_apItem[i].m_dwObjIndex	= NULL_ID;
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
#ifdef __FINITEITEM0705
		if( m_aEquipInfo[i].dwId != adwEquipment[i] )
			m_aEquipInfo[i].dwId	= adwEquipment[i];
#else	// __FINITEITEM0705
		if( m_adwEquipment[i] != adwEquipment[i] )
			m_adwEquipment[i]	= adwEquipment[i];
#endif	// __FINITEITEM0705
	}
	return TRUE;
}
#if !defined(__CORESERVER)
// IK3로 삭제하기 예) IK3_CLOAK
void CMover::RemoveItemIK3()
{
	int	nSize	= m_Inventory.m_dwItemMax;
	CItemElem* pItemElem;
	BOOL bRemove;
	for( int i = 0 ; i < nSize; i++ )
	{
		bRemove = FALSE;
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
		 	ItemProp* pItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
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
					if( m_Inventory.IsEquip( pItemElem->m_dwObjId ) )		
					{
						if( m_Inventory.UnEquip( pItemProp->dwParts ) ) // 장비 해제
						{
#ifdef __FINITEITEM0705
							m_aEquipInfo[pItemProp->dwParts].dwId	= NULL_ID;
#else	// __FINITEITEM0705
							m_adwEquipment[ pItemProp->dwParts ]	= NULL_ID;
#endif	// __FINITEITEM0705
							m_Inventory.RemoveAt( pItemElem->m_dwObjIndex );
						}
						else
						{
							WriteLog( "Not UnEquip RemoveItemIK3  : %s, %d, %d", m_szName, pItemElem->m_dwObjId, pItemProp->dwID );
						}
					}
					else
					{
						m_Inventory.RemoveAt( pItemElem->m_dwObjIndex );
					}
				}
			}
			else
			{
				WriteLog( "Not ItemProp RemoveItemIK3  : %s, %d ", m_szName, pItemElem->m_dwObjId );
			}
		}
	}
}
#endif // !__CORESERVER

#ifdef __CONV_SKILL_11_MONTH_JOB1
int CMover::InitSkillExp()
{
	int dwSkillPoint = 0;
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
					return 0;	// property not found
				}
				
				for( int j = 1 ; j < pSkill->dwLevel ; ++j )
				{
					AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, j );
					if( pAddSkillProp == NULL )
					{
						return 0;	// property not found
					}
					if( prj.m_aExpSkill[ j + 1 ] != 0 )
						dwSkillPoint += prj.m_aExpSkill[ j + 1 ] * pAddSkillProp->dwSkillExp;
					else
						dwSkillPoint += pAddSkillProp->dwSkillExp;
				}
				
				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );
				if( pAddSkillProp == NULL )
				{
					return 0;	// property not found
				}
				dwSkillPoint += pSkill->dwExp * pAddSkillProp->dwSkillExp;
				
				pSkill->dwLevel = 1;
				pSkill->dwExp = 0;
			}
		}
	}
	return dwSkillPoint;
}
#endif // __CONV_SKILL_11_MONTH_JOB1

CItemElem& CItemElem::operator =( CItemElem & ie )
{
	CItemBase::operator =( ie );
	m_nItemNum	     = ie.m_nItemNum;
	m_nRepairNumber	 = ie.m_nRepairNumber;
	m_nHitPoint	     = ie.m_nHitPoint;
	m_byFlag	     = ie.m_byFlag;
	m_dwSerialNumber = ie.m_dwSerialNumber;
	m_nAbilityOption = ie.m_nAbilityOption;
	m_idGuild		 = ie.m_idGuild;
	m_bItemResist = ie.m_bItemResist;
	m_nResistAbilityOption = ie.m_nResistAbilityOption;
	m_nResistSMItemId = ie.m_nResistSMItemId;

#ifdef __PIERCING_SM_TIME		
	m_bCharged = ie.m_nResistSMItemId;
	m_dwKeepTime = ie.m_dwKeepTime;
#endif // __PIERCING_SM_TIME
	
#ifdef __PIERCING_SM_TIME
	memcpy( &m_piercingInfo, &ie.m_piercingInfo, sizeof(PIERCINGINFO) );
	m_bCharged	= ie.m_bCharged;
#endif	// __PIERCING_SM_TIME

	return *this;
}

CItemBase& CItemBase::operator = ( CItemBase& ib )
{
	_tcscpy( m_szItemText, ib.m_szItemText );
	m_dwItemId       = ib.m_dwItemId  ;
	m_dwSerialNumber = ib.m_dwSerialNumber;
	return *this;
}
