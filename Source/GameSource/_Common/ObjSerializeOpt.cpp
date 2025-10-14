#include "stdafx.h"

#	if defined(__WORLDSERVER) || defined(__CLIENT)
#include "mover.h"
#	else	// __WORLDSERVER, __CLIENT
#include "..\_network\objects\obj.h"
#	endif	// __WORLDSERVER, __CLIENT

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
	u_long nBufSize;	\
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK

#include "honor.h"		//sun: 13, 달인

void CObj::Serialize( CAr & ar, int nMethod )	// 21
{
	UNUSED_ALWAYS( nMethod );
	if( ar.IsStoring() )
	{
		ar << (u_char)m_dwType;

		// mirchang_100513 type cast fix
		//ar << (u_short)m_dwIndex;
		ar << m_dwIndex;
		// mirchang_100513 type cast fix

		ar << (u_short)( m_vScale.x * 100.0f );
		ar << m_vPos;
		ar << (short)( m_fAngle * 10.0f );
	}
	else
	{
		u_short u2;
		short i2;
		m_dwType = m_dwIndex = 0; 
		
		ar >> (u_char&)m_dwType;				// m_dwType

		// mirchang_100513 type cast fix
		//ar >> u2;								// m_dwIndex
		//m_dwIndex	= (DWORD)u2;
		ar >> m_dwIndex;
		// mirchang_100513 type cast fix

		ar >> u2;								// m_vScale.x * 100.0f
		m_vScale.x	=	m_vScale.y	=	m_vScale.z	= (float)u2 / 100.0f;
		ar >> m_vPos;							// m_vPos
		ar >> i2;								// m_fAngle * 10.0f
		m_fAngle	= (float)i2 / 10.0f;
	#if defined(__WORLDSERVER) || defined(__CLIENT)
		SetAngle( m_fAngle );
	#endif

	#ifdef __WORLDSERVER
		m_vLink		= m_vPos;
	#endif	// __WORLDSERVER
	}
}

#if defined(__WORLDSERVER) || defined(__CLIENT)
// 장어구이, 신속의 두루마리 등의 예외처리
void CMover::OnApplySM()
{
#ifdef __WORLDSERVER

	if( m_dwSMTime[SM_MAX_HP] > 0 )
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10209, II_CHR_FOO_COO_BULLHAMS ) );
		if( pItemProp != NULL )
		{
			SetDestParam( pItemProp->dwDestParam[0], pItemProp->nAdjParamVal[1], NULL_CHGPARAM, 1 );	// fixed by jeff
		}
	}

	if( m_dwSMTime[SM_MAX_HP50] > 0 )
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10210, II_CHR_FOO_COO_GRILLEDEEL ) );
		if( pItemProp != NULL )
		{
			SetDestParam( pItemProp->dwDestParam[0], m_nPlusMaxHitPoint, NULL_CHGPARAM, 1 );	// fixed by jeff
		}
	}

	if( m_dwSMTime[SM_VELOCIJUMP] > 0 || m_dwSMTime[SM_JUMP] > 0)
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( g_AddSMMode.dwSMItemID[SM_VELOCIJUMP] );
		if( pItemProp != NULL )
		{
			for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
			{
				if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
				{
					SetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ], NULL_CHGPARAM );
				}
			}
// 			if( pItemProp->dwDestParam1 != NULL_ID )
// 				SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, NULL_CHGPARAM );
// 			if( pItemProp->dwDestParam2 != NULL_ID )
// 				SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, NULL_CHGPARAM );
// 			if( pItemProp->dwDestParam3 != NULL_ID )
// 				SetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3, NULL_CHGPARAM );
		}
	}

#endif // __WORLDSERVER
}
#endif // __WORLDSERVER __CLIENT

void CMover::Serialize( CAr & ar, int nMethod )
{
	CCtrl::Serialize( ar, nMethod );	//	25
	DWORD dwGold;

	if( ar.IsStoring() )	// STORING
	{
		ar << (u_short)m_dwMotion;
		ar << (u_char)m_bPlayer;
		ar << m_nHitPoint;
		ar << m_pActMover->GetState();
		ar << m_pActMover->GetStateFlag();
		ar << (u_char)m_dwBelligerence;
		ar << m_dwMoverSfxId;
		if( m_bPlayer )	// PLAYER
		{
			ar.WriteString( m_szName );
			ar << GetSex();
			ar << (u_char)m_dwSkinSet;
			ar << (u_char)m_dwHairMesh;
			ar << m_dwHairColor;
			ar << (u_char)m_dwHeadMesh;
			ar << m_idPlayer;
			ar << (u_char)m_nJob;
			m_Stat.Serialize( ar );
			ar << (u_short)m_nLevel;
			ar << m_nFuel;
			ar << m_tmAccFuel;
			if( m_idGuild > 0 )
			{
				ar << (u_char)1;
				ar << m_idGuild;
				ar << m_idWar;
			}
			else
			{
				ar << (u_char)0;
			}
			ar << m_idGuildCloak;

			if( m_idparty > 0 )
			{
				ar << (u_char)1;
				ar << m_idparty;
				ar << m_idDuelParty;
			}
			else
			{
				ar << (u_char)0;
			}

			ar << (char)m_dwAuthorization;
			ar << m_dwMode;
			ar << m_dwStateMode;
 #ifdef __WORLDSERVER
			FLItemElem* pItemElem = m_Inventory.GetAtId( m_dwUseItemId );
			if( pItemElem != NULL )
				ar << pItemElem->m_dwItemId;
			else
				ar << (OBJID)0;
 #else // __WORLDSERVER
			ar << m_dwUseItemId;
 #endif // __WORLDSERVER

			if( m_dwPKTime > 0 )
	#ifdef __WORLDSERVER
				ar << ( m_dwPKTime - GetTickCount() );
	#else // __WORLDSERVER
				ar << m_dwPKTime;
	#endif // __WORLDSERVER
			else
				ar << m_dwPKTime;
			ar << m_nPKValue;
			ar << m_dwPKPropensity;
			ar << m_dwPKExp;
			ar << m_nFame;
			ar << (u_char)m_nDuel;
			ar << m_nHonor;		//sun: 13, 달인 
			{
				for( DWORD dwObjIndex = 0; dwObjIndex < MAX_HUMAN_PARTS; ++dwObjIndex )
				{
//					ar << m_aEquipInfo[i].nOption;
					FLItemElem* pItemElem	= m_Inventory.GetAt( dwObjIndex );
					if( pItemElem != NULL )
					{
						m_aEquipInfo[ dwObjIndex ].Copy( *pItemElem );
					}
					else
					{
						m_aEquipInfo[ dwObjIndex ].Init();
					}
				
					ar << m_aEquipInfo[ dwObjIndex ];
				}
			}
			ar << m_nGuildCombatState;
			
			for( int j = 0 ; j < SM_MAX ; ++j )
				ar << m_dwSMTime[j];

			if( nMethod == METHOD_NONE )
			{
				ar << (u_short)m_nManaPoint;
				ar << (u_short)m_nFatiguePoint;

				ar << m_nTutorialState;			//sun: 12, 튜토리얼 개선

				ar << m_nFxp;
				
				dwGold = GetGold();
				ar << dwGold;
				ar << m_nExp1;
				ar << m_nSkillLevel;
				ar << m_nSkillPoint;
				ar << m_nDeathExp;
				ar << m_nDeathLevel;
				DWORD dwJobLv[MAX_JOB] = {0, };
				ar.Write( (void*)dwJobLv, sizeof(DWORD) * MAX_JOB );
				ar << m_idMarkingWorld;
				ar << m_vMarkingPos;
				ar << m_nQuestSize;
				ar.Write( m_aQuest, sizeof(QUEST) * m_nQuestSize );
				ar << m_nCompleteQuestSize;
				ar.Write( m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize ); 

				ar << m_nCheckedQuestSize;
				ar.Write( m_aCheckedQuest, sizeof(WORD) * m_nCheckedQuestSize ); 

				ar << m_idMurderer;
// 				{
// 					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 						ar << m_aEquipInfo[i].dwId;
// 				}
				ar.Write( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
				
				ar << (BYTE)m_nCheerPoint << m_dwTickCheer - GetTickCount();

				ar << m_nDBDataSlot;
				for( int k = 0 ; k < MAX_CHARACTER_SLOT ; ++k )
				{
					ar << m_dwGoldBank[k];
				}
				for( int k = 0 ; k < MAX_CHARACTER_SLOT ; ++k )
				{
					ar << m_idPlayerBank[k];
				}
				ar << m_nPlusMaxHitPoint;
				ar << m_nAttackResistLeft;
				ar << m_nAttackResistRight;
				ar << m_nDefenseResist;
				ar << m_nAngelExp;			//sun: 8차 엔젤 소환 Neuz, World, Trans
				ar << m_nAngelLevel;		//sun: 8차 엔젤 소환 Neuz, World, Trans
				m_Inventory.Serialize( ar );
				for( int k = 0 ; k < MAX_CHARACTER_SLOT ; ++k )
				{
					m_Bank[k].Serialize( ar );
				}
				ar << GetPetId();			//sun: 9, 9-10차 펫 // 소환 중인 펫 인벤토리 위치
				m_Pocket.Serialize( ar );	//sun: 11, 주머니
				ar << m_dwMute;
//sun: 13, 달인
				for( int i = 0 ; i < MAX_HONOR_TITLE ; ++i )
				{
					ar << m_aHonorTitle[i];

				}

				ar << m_idCampus;
				ar << m_nCampusPoint;
			}
			else if( nMethod == METHOD_EXCLUDE_ITEM )
			{
#	if defined (__WORLDSERVER) || defined(__CLIENT)
				ar.WriteString( m_vtInfo.GetTitle() );
#	endif
// 				u_char uSize	= 0;
// 				ptrdiff_t nSizeTag = ar.ReserveSpace( sizeof( uSize ) );
// 				for( u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++ )
// 				{
// 					FLItemElem* pItemElem	= m_Inventory.GetEquip( uParts );
// 					if( pItemElem )
// 					{
// 						uSize++;
// 						ar << uParts;
// 						ar << (u_short)pItemElem->m_dwItemId;
// 						ar << pItemElem->m_byFlag;
// 					}
// 				}
// 				ar.WriteReservedSpace( nSizeTag, &uSize, sizeof( uSize ) );
//sun: 9, 9-10차 펫
				DWORD dwPetId	= NULL_ID;

				char* pszPetName	= "";		//sun: ?, __PET_1024

				FLItemElem* pItemElem	= GetPetItem();
				if( pItemElem )
				{
					dwPetId		= MAKELONG( (WORD)pItemElem->m_pPet->GetIndex(), (WORD)pItemElem->m_pPet->GetLevel() );
					pszPetName	= const_cast<char*>( pItemElem->m_pPet->GetName() );	//sun: ?, __PET_1024
				}
				ar << dwPetId;
				ar.WriteString( pszPetName );	//sun: ?, __PET_1024
			}

			ar << m_kMadrigalGiftPoint.nPoint;

// 			ar.Write( m_adjParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
// 			ar.Write( m_chgParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
		}
		else	// NPC
		{
			ar << (u_char)m_dwHairMesh << m_dwHairColor << (u_char)m_dwHeadMesh;
			ar.WriteString( m_szCharacterKey );

			u_char uSize	= 0;
			ptrdiff_t nSizeTag = ar.ReserveSpace( sizeof( uSize ) );
			if( IsEquipableNPC() )
			{
				FLItemElem* pItemElem;
				for( u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++ )
				{
					pItemElem	= m_Inventory.GetEquip( uParts );
					if( pItemElem )
					{
						uSize++;
						ar << uParts;
						ar << pItemElem->m_dwItemId;
					}
				}
			}
			ar.WriteReservedSpace( nSizeTag, &uSize, sizeof( uSize ) );


			ar << (u_char)m_bActiveAttack;
			ar << (u_char)m_nMovePattern;
			ar << (u_char)m_nMoveEvent;
			ar << m_nMoveEventCnt;
//sun: 9,	//__AI_0509
			ar << m_fSpeedFactor;
		}

		m_buffs.Serialize( ar );

		//////////////////////////////////////////////////////////////////////////
		ar.Write( m_adjParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
		ar.Write( m_chgParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
		//////////////////////////////////////////////////////////////////////////
	}
	else	// LOADING
	{
		m_dwMotion = 0;

		u_short nMotion;
		ar >> nMotion;							// m_dwMotion

		#if defined(__WORLDSERVER) || defined(__CLIENT)
				m_dwMotion = (DWORD)nMotion;	// 성별도 정해지지 않는 상태에서는 SetMotion호출하지 않는다.
		#else	// defined(__WORLDSERVER) || defined(__CLIENT)
			m_dwMotion = (DWORD)nMotion;
		#endif

		m_bPlayer	=
		m_dwBelligerence	=
		m_idGuild	=
		m_idWar	=
		m_idparty	=
		m_idDuelParty	=
		m_nDuel		= 
		m_dwRideItemIdx		= 0;

		ClearEquipInfo();
		
		ar >> (u_char&)m_bPlayer;

#ifdef __SKILL_0205
		SKILL	aJobSkill[MAX_SKILL_JOB];
		memcpy( aJobSkill, m_aJobSkill, sizeof(aJobSkill) );
#endif	// __SKILL_0205

		InitProp();

		m_nHitPoint	= 0;

		ar >> m_nHitPoint;
		DWORD dw1, dw2;
		ar >> dw1;
		ar >> dw2;
		m_pActMover->ClearStateFlag();
		m_pActMover->AddStateFlag( dw2 );
		m_pActMover->__ForceSetState( dw1 );
		ar >> (u_char&)m_dwBelligerence;
		ar >> m_dwMoverSfxId;

		if( m_bPlayer )	// PLAYER
		{
			m_dwSkinSet		=
			m_dwHairMesh	=
			m_dwHeadMesh	=
			m_idPlayer	=
			m_nJob	=
			m_nLevel	=
			m_dwAuthorization	= 0;

			BYTE bySex;
			ar.ReadString( m_szName, _countof( m_szName ) );
			ar >> bySex;
			SetSex( bySex );
			ar >> (u_char&)m_dwSkinSet;
			ar >> (u_char&)m_dwHairMesh;
			ar >> m_dwHairColor;
			ar >> (u_char&)m_dwHeadMesh;
#	if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor( m_dwHairColor );
#	endif	// __WORLDSERVER	//__CLIENT
			ar >> m_idPlayer;
			ar >> (u_char&)m_nJob;
			m_Stat.Serialize( ar );
			ar >> (u_short&)m_nLevel;
			ar >> m_nFuel;
			ar >> m_tmAccFuel;

			u_char u1;
			ar >> u1;
			if( u1 == 1 )
			{
				ar >> m_idGuild;
				ar >> m_idWar;
			}
			ar >> m_idGuildCloak;

			ar >> u1;
			if( u1 == 1 )
			{
				ar >> m_idparty;
				ar >> m_idDuelParty;
			}

			ar >> (char&)m_dwAuthorization;
			ar >> m_dwMode;
			ar >> m_dwStateMode;
#ifdef __WORLDSERVER
			OBJID dwItemTem;
			ar >> dwItemTem;
#else // __WORLDSERVER
			ar >> m_dwUseItemId;
#endif // __WORLDSERVER
//sun: 8, // __S8_PK
			ar >> m_dwPKTime;
	#ifdef __WORLDSERVER
			if( m_dwPKTime > 0 )
				m_dwPKTime = GetTickCount() + m_dwPKTime;
	#endif // __WORLDSERVER
			ar >> m_nPKValue;
			ar >> m_dwPKPropensity;
			ar >> m_dwPKExp;
			ar >> m_nFame;
			ar >> (u_char&)m_nDuel;
//sun: 13, 달인
			int nTemp = -1;
			ar >> nTemp;
#ifdef __CLIENT
			if(m_nHonor != nTemp)// 달인선택 
			{
				m_nHonor = nTemp;
				SetTitle(CTitleManager::Instance()->GetTitle(m_nHonor));
			}
#else	// __CLIENT
			m_nHonor = nTemp;
#endif	// __CLIENT
			{
				for( int i = 0; i < MAX_HUMAN_PARTS; i ++ )
				{
//					ar >> m_aEquipInfo[i].nOption;
					ar >> m_aEquipInfo[i];
				}
			}
			ar >> m_nGuildCombatState;
			
			for( int j = 0 ; j < SM_MAX ; ++j )
				ar >> m_dwSMTime[j];
			// 장어구이, 신속의 두루마리 등의 예외처리는 m_nPlusMaxHitPoint등이 세트되고 OnApplySM() 실행한다.

			if( nMethod == METHOD_NONE )
			{
				m_nManaPoint	= 0;
				m_nFatiguePoint	= 0;

				ar >> (u_short&)m_nManaPoint;
				ar >> (u_short&)m_nFatiguePoint;
//sun: 12, 튜토리얼 개선
				ar >> m_nTutorialState;
#ifdef __CLIENT
				g_Option.m_nTutorialLv	= m_nTutorialState;
#endif	// __CLIENT

				ar >> m_nFxp;

				ar >> dwGold;
				SetGold( dwGold );

				ar >> m_nExp1;
				ar >> m_nSkillLevel;
				ar >> m_nSkillPoint;
				ar >> m_nDeathExp;
				ar >> m_nDeathLevel; 
				DWORD dwJobLv[MAX_JOB];		// 사용하지 않음 
				ar.Read( (void*)dwJobLv, sizeof(DWORD) * MAX_JOB );
				ar >> m_idMarkingWorld;
				ar >> m_vMarkingPos;
				ar >> m_nQuestSize;
				ar.Read( m_aQuest, sizeof(QUEST) * m_nQuestSize ); 
				ar >> m_nCompleteQuestSize;
				ar.Read( m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize ); 

				ar >> m_nCheckedQuestSize;
				ar.Read( m_aCheckedQuest, sizeof(WORD) * m_nCheckedQuestSize );

				ar >> m_idMurderer;
// 				{
// 					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 						ar >> m_aEquipInfo[i].dwId;
// 				}
#ifdef __SKILL_0205
				ar.Read( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );

				/************************************************************************/
				/* 
				변경된 유저의 스킬데이터만 DB 저장하기 위해 사용된 방법. 현재는 모두 다시 저장하므로 사용하지 않음.
				*/
				/************************************************************************/

// 				for( int i = 0 ; i < MAX_SKILL_JOB; i++)
// 				{
// 					m_abUpdateSkill[i]	= (BYTE)( memcmp( &m_aJobSkill[i], &aJobSkill[i], sizeof(SKILL) ) != 0 );
// 				}
#else	// __SKILL_0205
				ar.Read( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
#endif	// __SKILL_0205

				m_nCheerPoint	= 0;
				ar >> (BYTE&)m_nCheerPoint >> m_dwTickCheer;
				m_dwTickCheer	+= GetTickCount();
				ar >> m_nDBDataSlot;
				for( int k = 0 ; k < MAX_CHARACTER_SLOT ; ++k )
				{
					ar >> m_dwGoldBank[k];
				}
				for( int k = 0 ; k < MAX_CHARACTER_SLOT ; ++k )
				{
					ar >> m_idPlayerBank[k];
				}

				ar >> m_nPlusMaxHitPoint;
				ar >> m_nAttackResistLeft;
				ar >> m_nAttackResistRight;
				ar >> m_nDefenseResist;

				ar >> m_nAngelExp;		//sun: 8차 엔젤 소환 Neuz, World, Trans
				ar >> m_nAngelLevel;	//sun: 8차 엔젤 소환 Neuz, World, Trans

				m_Inventory.Deserialize( ar );
				for( int k = 0 ; k < MAX_CHARACTER_SLOT ; ++k )
				{
					m_Bank[k].Deserialize( ar );
				}

				FLItemElem* pItemElem	= m_Inventory.GetEquip( PARTS_RIDE );
				if( pItemElem )
					m_dwRideItemIdx		= pItemElem->m_dwItemId;

//sun: 9, 9-10차 펫
				DWORD dwPetId;
				ar >> dwPetId;
				SetPetId( dwPetId );
	//sun: ?, __PET_1024
	#ifdef __CLIENT
				CPet* pPet	= GetPet();
				if( pPet )
					m_pet.SetName( const_cast<char*>( pPet->GetName() ) );
	#endif	// __CLIENT

				m_Pocket.Serialize( ar );		//sun: 11, 주머니

				ar >> m_dwMute;
//sun: 13, 달인
#ifdef __CLIENT
				CTitleManager::Instance()->InitEarned();
#endif	// __CLIENT
				for( int l = 0 ; l < MAX_HONOR_TITLE ; ++l )
				{
					ar >> m_aHonorTitle[l];
#ifdef __CLIENT
					int nNeed = CTitleManager::Instance()->GetNeedCount(l, -1);
					if(m_aHonorTitle[l] >= nNeed && nNeed > 0)
					{
						CTitleManager::Instance()->AddEarned(l);	
					}
					else
					{
						// 획득된 타이틀이지만 요구사항을 충족못하게 될때
						if(CTitleManager::Instance()->IsEarned(l))
						{
							CTitleManager::Instance()->RemoveEarned(l);
						}
					}
#endif	// __CLIENT
				}

				ar >> m_idCampus;
				ar >> m_nCampusPoint;
			}
			else if( nMethod == METHOD_EXCLUDE_ITEM )
			{
			#if defined (__WORLDSERVER) || defined(__CLIENT)
				char szPVendor[MAX_VENDORNAME] = {0, };
				ar.ReadString( szPVendor, _countof( szPVendor ) );
				m_vtInfo.SetTitle( szPVendor );
			#endif
// 				for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 					m_aEquipInfo[i].dwId	= NULL_ID;

// 				u_char uSize;
// 				ar >> uSize;
// 				u_char uParts;
// 				u_short	u2;
// 				for( u_char uCount = 0; uCount < uSize; uCount++ )
// 				{
// 					ar >> uParts;
// 					ar >> u2;
// 					m_aEquipInfo[uParts].dwId	= (DWORD)u2;
// 					ar >> m_aEquipInfo[uParts].byFlag;
// 				}

				if( m_aEquipInfo[PARTS_RIDE].dwItemID != 0 )
					m_dwRideItemIdx		= m_aEquipInfo[PARTS_RIDE].dwItemID;
//sun: 9, 9-10차 펫
				DWORD dwPetId;
				ar >> dwPetId;
				SetPetId( dwPetId );
	//sun: ?, __PET_1024
				char szPetName[MAX_PET_NAME]	= { 0,};
				ar.ReadString( szPetName, _countof( szPetName ) );
	#ifdef __CLIENT
				m_pet.SetName( szPetName );
	#endif	// __CLIENT
	
			}

		#if defined(__WORLDSERVER) || defined(__CLIENT)
			if( m_dwRideItemIdx )
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwRideItemIdx );
				if( pItemProp != NULL && m_nFuel == NULL_ID )
					m_nFuel		= (int)( pItemProp->dwFFuelReMax * 0.2f );
			}
			OnApplySM();
		#endif	// __WORLDSERVER	// __CLIENT

			ar >> m_kMadrigalGiftPoint.nPoint;

// 			ar.Read( m_adjParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
// 			ar.Read( m_chgParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
		}
		else	// NPC
		{
			ar >> (u_char&)m_dwHairMesh >> m_dwHairColor >> (u_char&)m_dwHeadMesh;
			ar.ReadString( m_szCharacterKey, _countof( m_szCharacterKey ) );
		#if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor( m_dwHairColor );
		#endif	// __WORLDSERVER	//__CLIENT
// 			{
// 				for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 					m_aEquipInfo[i].dwId	= NULL_ID;
// 			}
			u_char uSize;
			ar >> uSize;
			if( IsEquipableNPC() )
			{
				for( u_char uCount = 0; uCount < uSize; uCount++ )
				{
					u_char uParts	= 0;
					DWORD dwItemID	= 0;

					ar >> uParts;
					ar >> dwItemID;

					m_aEquipInfo[uParts].dwItemID	= dwItemID;
				}
			}

			m_bActiveAttack	=
			m_nMovePattern	=
			m_nMoveEvent	=
									0;

			ar >> (u_char&)m_bActiveAttack;
			ar >> (u_char&)m_nMovePattern;
			ar >> (u_char&)m_nMoveEvent;
			ar >> m_nMoveEventCnt;
		#ifdef __CLIENT
			if( *m_szCharacterKey )
			{
				LPCHARACTER pCharacter	= prj.GetCharacter( m_szCharacterKey );
				if( pCharacter )
				{
					FLStrcpy( m_szName, _countof( m_szName ), pCharacter->m_strName );
					AllocShopInventory( pCharacter );
				}
			}
		#endif	// __CLIENT

			ar >> m_fSpeedFactor;	//sun: 9,	//__AI_0509
		}

		m_buffs.Serialize( ar );

		//////////////////////////////////////////////////////////////////////////
		LONG tmp_m_adjParamAry[ MAX_ADJPARAMARY ] = { 0 };
		LONG tmp_m_chgParamAry[ MAX_ADJPARAMARY ] = { 0 };

		ar.Read( tmp_m_adjParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
		ar.Read( tmp_m_chgParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );

#ifdef __CLIENT
		memcpy( m_adjParamAry, tmp_m_adjParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
		memcpy( m_chgParamAry, tmp_m_chgParamAry, sizeof( LONG ) * MAX_ADJPARAMARY );
#endif // __CLIENT
		//////////////////////////////////////////////////////////////////////////

	#ifdef __CLIENT
		AutoSynchronizer()->Initialize();
	#endif	// __CLIENT

	} // LOADING

#ifdef __CLIENT	
	m_fDestScale = GetScale().x;
	if( m_fDestScale > 10.0f )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "SerializeOpt:%f %s" ), m_fDestScale, GetName() );
	}
#endif
}
