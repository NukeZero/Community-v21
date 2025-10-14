#include "stdafx.h"

#	if defined(__WORLDSERVER) || defined(__CLIENT) || defined(__BEAST)
#include "mover.h"
#include "defineItem.h"
#	else	// __WORLDSERVER, __CLIENT
#include "..\_network\objects\obj.h"
#	endif	// __WORLDSERVER, __CLIENT

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK

void CObj::Serialize( CAr & ar )	// 21
{
	if( ar.IsStoring() )
	{
		ar << (u_char)m_dwType;
		ar << (u_short)m_dwIndex;
		ar << (u_short)( m_vScale.x * 100.0f );
		ar << (u_short)m_dwMotion;
		ar << m_vPos;
		ar << (short)( m_fAngle * 10.0f );
	}
	else
	{
		u_short u2;

		short i2;
		
		m_dwType	=
		m_dwIndex		=
		m_dwMotion	= 0;
		
		ar >> (u_char&)m_dwType;	// m_dwType
		ar >> u2;								// m_dwIndex
		m_dwIndex	= (DWORD)u2;
		ar >> u2;								// m_vScale.x * 100.0f
		m_vScale.x	=	m_vScale.y	=	m_vScale.z	= (float)u2 / 100.0f;
		ar >> u2;								// m_dwMotion
		ar >> m_vPos;							// m_vPos
		ar >> i2;									// m_fAngle * 10.0f
		m_fAngle	= (float)i2 / 10.0f;
#	if defined(__WORLDSERVER) || defined(__CLIENT)
		SetMotion( u2 );
		SetAngle( m_fAngle );
#	else	// __WORLDSERVER	// __CLIENT
		m_dwMotion	= (DWORD)u2;
#	endif	// __WORLDSERVER	// __CLIENT
#	ifdef __WORLDSERVER
		m_vLink		= m_vPos;
#	endif	// __WORLDSERVER
	}
}

// 장어구이, 신속의 두루마리 등의 예외처리
#	if defined(__WORLDSERVER) || defined(__CLIENT)
void CMover::OnApplySM()
{
	ItemProp* aItemprop;
	int nAdjParamVal;

	if( m_dwSMTime[SM_MAX_HP] > 0 )
	{
		aItemprop = prj.GetItemProp( II_CHR_FOO_COO_BULLHAMS );
		nAdjParamVal = aItemprop->nAdjParamVal2;
		SetDestParam( aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1 );	// fixed by jeff
	}

	if( m_dwSMTime[SM_MAX_HP50] > 0 )
	{
		aItemprop = prj.GetItemProp( II_CHR_FOO_COO_GRILLEDEEL );
		nAdjParamVal = m_nPlusMaxHitPoint;
		SetDestParam( aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1 );	// fixed by jeff
	}

	if( m_dwSMTime[SM_VELOCIJUMP] > 0 )
	{
		aItemprop = prj.GetItemProp( g_AddSMMode.dwSMItemID[SM_VELOCIJUMP] );
		if( aItemprop )
		{
			if( aItemprop->dwDestParam1 != -1 )
				SetDestParam( aItemprop->dwDestParam1, aItemprop->nAdjParamVal1, NULL_CHGPARAM );
			if( aItemprop->dwDestParam2 != -1 )
				SetDestParam( aItemprop->dwDestParam2, aItemprop->nAdjParamVal2, NULL_CHGPARAM );
		}
	}
}
#endif	//

void CMover::Serialize( CAr & ar )
{
#ifndef __BEAST
	CCtrl::Serialize( ar );	//	25
	DWORD dwGold;

	if( ar.IsStoring() )	// STORING
	{
		ar << (u_char)m_bPlayer;
		ar << m_nHitPoint;
		ar << m_pActMover->GetState();
		ar << m_pActMover->GetStateFlag();
		ar << (u_char)m_dwBelligerence;

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
			ar << (u_short)m_nStr;
			ar << (u_short)m_nSta;
			ar << (u_short)m_nDex;
			ar << (u_short)m_nInt;
			ar << (u_short)m_nLevel;
#   ifndef __BEAST			
			ar << m_nFuel;
#   endif
#	ifdef __XACCFUEL0708
			ar << m_tmAccFuel;
#	endif	// __XACCFUEL0708
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
#ifdef __S0504_FLY	
			ar << m_dwStateMode;
 #ifdef __WORLDSERVER
			CItemElem* pItemElem = m_Inventory.GetAtId( m_dwUseItemId );
			if( pItemElem )
				ar << pItemElem->GetProp()->dwID;
			else
				ar << (OBJID)0;
 #else // __WORLDSERVER
			ar << m_dwUseItemId;
 #endif // __WORLDSERVER
#endif // __S0504_FLY
			ar << (u_short)m_nNumKill;
			ar << m_nSlaughter;
			ar << m_nFame;
			ar << (u_char)m_nDuel;
#ifdef __XOPT0617
			ar << (u_char)m_nWeaponOption;
 #ifdef __XEFFECT1006
			short wWeaponAttr = (m_nWeaponOption & 0xFFFF0000) >> 16;
			ar << wWeaponAttr;		//무기의 속성값도 더 보내자.
 #endif
#else	// xOpt0617
			{
				int i;
				for( i = 0; i < MAX_HUMAN_PARTS; i ++ )
				{
#ifdef __FINITEITEM0705
					ar << m_aEquipInfo[i].nOption;
#else	// __FINITEITEM0705
					ar << m_anEquipOption[i];
#endif	// __FINITEITEM0705
				}
			}
#endif	// not XOPT0617
			
#ifdef __GUILDCOMBAT
			ar << m_nGuildCombatState;
#endif // __GUILDCOMBAT
			
			
			for( int j = 0 ; j < SM_MAX ; ++j )
				ar << m_dwSMTime[j];

			if( CObj::GetMethod() == METHOD_NONE )
			{
				ar << (u_short)m_nManaPoint;
				ar << (u_short)m_nFatiguePoint;
				ar << (u_short)m_nFlightLv;
				ar << m_nFxp;
//				ar << m_nTxp;
				
				dwGold = GetGold();
				ar << dwGold;
				ar << m_nExp1;
				ar << m_nExp2;
				ar << m_nDeathExp;
				ar << m_nDeathLevel;
				ar.Write( (void*)m_aJobLv, sizeof(DWORD) * MAX_JOB );
				ar << m_idMarkingWorld;
				ar << m_vMarkingPos;
				ar << m_nQuestSize;
				ar.Write( m_aQuest, sizeof(QUEST) * m_nQuestSize );
#ifdef __V050322_CACHEQUEST
				ar << m_nCompleteQuestSize;
				ar.Write( m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize ); 
#endif // __V050322_CACHEQUEST
				ar << m_idMurderer;
				ar << (short)m_nRemainGP;
				ar << (short)m_nRemainLP;
#ifdef __FINITEITEM0705
				{
					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
						ar << m_aEquipInfo[i].dwId;
				}
#else	// __FINITEITEM0705
				ar.Write( m_adwEquipment, sizeof(DWORD) * MAX_HUMAN_PARTS );
#endif	// __FINITEITEM0705
				ar.Write( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
				
#ifdef __CHEER0615
				ar << (BYTE)m_nCheerPoint << m_dwTickCheer - GetTickCount();
#endif	// __CHEER0615

				ar << m_nSlot;
				for( int k = 0 ; k < 3 ; ++k )
					ar << m_dwGoldBank[k];
				for( k = 0 ; k < 3 ; ++k )
					ar << m_idPlayerBank[k];
				ar << m_nPlusMaxHitPoint;
				ar << m_nAttackResistLeft;
				ar << m_nAttackResistRight;
				ar << m_nDefenseResist;
				ar << m_dwSkillPoint;
				m_Inventory.Serialize( ar );
				for( k = 0 ; k < 3 ; ++k )
					m_Bank[k].Serialize( ar );
			}
			else if( CObj::GetMethod() == METHOD_EXCLUDE_ITEM )
			{
#	if defined (__WORLDSERVER) || defined(__CLIENT)
				ar.WriteString( m_vtInfo.GetTitle() );
#	endif
				u_char uSize	= 0;
				u_long uOffset	= ar.GetOffset();
				ar << uSize;
				CItemElem* pItemElem;
				for( u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++ )
				{
					pItemElem	= m_Inventory.GetEquip( uParts );
					if( pItemElem )
					{
						uSize++;
						ar << uParts;
						ar << (u_short)pItemElem->m_dwItemId;
					#ifdef __FINITEITEM0705
						ar << pItemElem->m_byFlag;
					#endif	// __FINITEITEM0705
					}
				}
				GETBLOCK( ar, pBlock, nBlockSize );
				*(UNALIGNED u_char*)( pBlock + uOffset )	= uSize;
			}
		}
		else	// NPC
		{
			ar << (u_char)m_dwHairMesh << m_dwHairColor << (u_char)m_dwHeadMesh;

			u_char uSize	= 0;
			u_long uOffset	= ar.GetOffset();
			ar << uSize;
			CItemElem* pItemElem;
			for( u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++ )
			{
				pItemElem	= m_Inventory.GetEquip( uParts );
				if( pItemElem )
				{
					uSize++;
					ar << uParts;
					ar << (u_short)pItemElem->m_dwItemId;
				}
			}
			GETBLOCK( ar, pBlock, nBlockSize );
			*(UNALIGNED u_char*)( pBlock + uOffset )	= uSize;


			ar << (u_char)m_bActiveAttack;
#ifdef __XFLYAI0524
			ar << (u_char)m_nMovePattern;
			ar << (u_char)m_nMoveEvent;
			ar << m_nMoveEventCnt;
#endif	// __XFLYAI0524
			ar.WriteString( m_szCharacterKey );
		}

		m_SkillState.Serialize( ar );
	}
	else	// LOADING
	{
		m_bPlayer	=
		m_dwBelligerence	=
		m_idGuild	=
		m_idWar	=
		m_idparty	=
		m_idDuelParty	=
		m_nDuel		= 
		m_dwRideItemIdx		= 0;

#ifdef __XOPT0617
		m_nWeaponOption		= 0 ;
#else // xOpt0617
	#ifdef __FINITEITEM0705
		ClearEquipInfo();
	#else	// __FINITEITEM0705
		memset(	m_anEquipOption, 0, sizeof(m_anEquipOption) );
	#endif	// __FINITEITEM0705
#endif	// not XOPT0617
		
		ar >> (u_char&)m_bPlayer;

		InitProp();
#ifdef __WORLDSERVER
		if( m_bPlayer )
			SetAIInterface( AII_HUMAN );
#endif	// __WORLDSERVER

		m_nHitPoint	=
								0;

		ar >> m_nHitPoint;
		DWORD dw1, dw2;
		ar >> dw1;
		ar >> dw2;
		m_pActMover->ClearStateFlag();
		m_pActMover->AddStateFlag( dw2 );
		m_pActMover->__ForceSetState( dw1 );
		ar >> (u_char&)m_dwBelligerence;

		if( m_bPlayer )	// PLAYER
		{
			m_dwSkinSet		=
			m_dwHairMesh	=
			m_dwHeadMesh	=
			m_idPlayer	=
			m_nJob	=
			m_nStr	=
			m_nSta	=
			m_nDex	=
			m_nInt	=
			m_nLevel	=
			m_dwAuthorization	=
			m_nNumKill	=
									0;

			BYTE bySex;
			ar.ReadString( m_szName );
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
			ar >> (u_short&)m_nStr;
			ar >> (u_short&)m_nSta;
			ar >> (u_short&)m_nDex;
			ar >> (u_short&)m_nInt;
			ar >> (u_short&)m_nLevel;
#   ifndef __BEAST
			ar >> m_nFuel;
#	endif 
#	ifdef __XACCFUEL0708
			ar >> m_tmAccFuel;
#	endif	// __XACCFUEL0708

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
#ifdef __S0504_FLY
			ar >> m_dwStateMode;
 #ifdef __WORLDSERVER
			OBJID dwItemTem;
			ar >> dwItemTem;
 #else // __WORLDSERVER
			ar >> m_dwUseItemId;
 #endif // __WORLDSERVER
#endif // __S0504_FLY
			ar >> (u_short&)m_nNumKill;
			ar >> m_nSlaughter;
			ar >> m_nFame;
			ar >> (u_char&)m_nDuel;
#ifdef __XOPT0617
			ar >> (u_char&)m_nWeaponOption;
 #ifdef __XEFFECT1006
			short wWeaponAttr;
			ar >> wWeaponAttr;		// 무기의 속성값
			m_nWeaponOption |= ((int)wWeaponAttr << 16);
 #endif
#else // xOpt0617
			{
				for( int i = 0; i < MAX_HUMAN_PARTS; i ++ )
				{
#ifdef __FINITEITEM0705
					ar >> m_aEquipInfo[i].nOption;
#else	// __FINITEITEM0705
					ar >> m_anEquipOption[i];
#endif	// __FINITEITEM0705
				}
			}
#endif	// not xOPT0617
			
#ifdef __GUILDCOMBAT
			ar >> m_nGuildCombatState;
#endif // __GUILDCOMBAT

			for( int j = 0 ; j < SM_MAX ; ++j )
				ar >> m_dwSMTime[j];
			// 장어구이, 신속의 두루마리 등의 예외처리는 m_nPlusMaxHitPoint등이 세트되고 OnApplySM() 실행한다.
			
			if( CObj::GetMethod() == METHOD_NONE )
			{
				m_nManaPoint	=
				m_nFatiguePoint		=
				m_nFlightLv		=
				m_nRemainGP		=
				m_nRemainLP		=
										0;

				ar >> (u_short&)m_nManaPoint;
				ar >> (u_short&)m_nFatiguePoint;
				ar >> (u_short&)m_nFlightLv;
				ar >> m_nFxp;

				ar >> dwGold;
				SetGold( dwGold );

				ar >> m_nExp1;
				ar >> m_nExp2;
				ar >> m_nDeathExp;
				ar >> m_nDeathLevel; 

				ar.Read( (void*)m_aJobLv, sizeof(DWORD) * MAX_JOB );
				ar >> m_idMarkingWorld;
				ar >> m_vMarkingPos;
				ar >> m_nQuestSize;
				ar.Read( m_aQuest, sizeof(QUEST) * m_nQuestSize ); 
#ifdef __V050322_CACHEQUEST
				ar >> m_nCompleteQuestSize;
				ar.Read( m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize ); 
#endif // __V050322_CACHEQUEST
				ar >> m_idMurderer;
				short n1, n2;
				ar >> n1 >> n2;
				m_nRemainGP		= n1;
				m_nRemainLP		= n2;
//				ar >> (short&)m_nRemainGP;
//				ar >> (short&)m_nRemainLP;
#ifdef __FINITEITEM0705
				{
					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
						ar >> m_aEquipInfo[i].dwId;
				}
#else	// __FINITEITEM0705
				ar.Read( m_adwEquipment, sizeof(DWORD) * MAX_HUMAN_PARTS );
#endif	// __FINITEITEM0705
				ar.Read( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );

#ifdef __CHEER0615
				m_nCheerPoint	= 0;
				ar >> (BYTE&)m_nCheerPoint >> m_dwTickCheer;
				m_dwTickCheer	+= GetTickCount();
#endif	// __CHEER0615
				ar >> m_nSlot;
				for( int k = 0 ; k < 3 ; ++k )
					ar >> m_dwGoldBank[k];
				for( k = 0 ; k < 3 ; ++k )
					ar >> m_idPlayerBank[k];

				ar >> m_nPlusMaxHitPoint;
				ar >> m_nAttackResistLeft;
				ar >> m_nAttackResistRight;
				ar >> m_nDefenseResist;
				ar >> m_dwSkillPoint;
				m_Inventory.Serialize( ar );
				for( k = 0 ; k < 3 ; ++k )
					m_Bank[k].Serialize( ar );

				CItemElem* pItemElem	= m_Inventory.GetEquip( PARTS_RIDE );
				if( pItemElem )
					m_dwRideItemIdx		= pItemElem->m_dwItemId;
			}
			else if( CObj::GetMethod() == METHOD_EXCLUDE_ITEM )
			{
#	if defined (__WORLDSERVER) || defined(__CLIENT)
				char szPVendor[MAX_VENDORNAME] = {0, };
				ar.ReadString( szPVendor );
				m_vtInfo.SetTitle( szPVendor );
#	endif
#ifdef __FINITEITEM0705
				{
					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
						m_aEquipInfo[i].dwId	= NULL_ID;
				}
#else	// FINITEITEM0705
				memset( (void*)m_adwEquipment, 0xff, sizeof( m_adwEquipment ) );
#endif	// __FINITEITEM0705
				u_char uSize;
				ar >> uSize;
				u_char uParts;
				u_short	u2;
				for( u_char uCount = 0; uCount < uSize; uCount++ )
				{
					ar >> uParts;
					ar >> u2;
				#ifdef __FINITEITEM0705
					m_aEquipInfo[uParts].dwId	= (DWORD)u2;
					ar >> m_aEquipInfo[uParts].byFlag;
				#else	// __FINITEITEM0705
					m_adwEquipment[uParts]	= (DWORD)u2;
				#endif	// __FINITEITEM0705
				}
			#ifdef __FINITEITEM0705
				if( m_aEquipInfo[PARTS_RIDE].dwId != NULL_ID )
					m_dwRideItemIdx		= m_aEquipInfo[PARTS_RIDE].dwId;
			#else	// __FINITEITEM0705
				if( m_adwEquipment[PARTS_RIDE] != NULL_ID )
					m_dwRideItemIdx		= m_adwEquipment[PARTS_RIDE];
			#endif	// __FINITEITEM0705
			}
#		if defined(__WORLDSERVER) || defined(__CLIENT)
			if( m_dwRideItemIdx )
			{
				ItemProp *pItemProp	= prj.GetItemProp( m_dwRideItemIdx );
				if( pItemProp && m_nFuel == -1 )
					m_nFuel		= (int)( pItemProp->dwFFuelReMax * 0.2f );
			}
			OnApplySM();
#		endif	// __WORLDSERVER	// __CLIENT
		}
		else	// NPC
		{
			ar >> (u_char&)m_dwHairMesh >> m_dwHairColor >> (u_char&)m_dwHeadMesh;
		#if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor( m_dwHairColor );
		#endif	// __WORLDSERVER	//__CLIENT
		#ifdef __FINITEITEM0705
			{
				for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
					m_aEquipInfo[i].dwId	= NULL_ID;
			}
		#else	// __FINITEITEM0705
			memset( (void*)m_adwEquipment, 0xff, sizeof( m_adwEquipment ) );
		#endif	// __FINITEITEM0705
			u_char uSize;
			ar >> uSize;
			u_char uParts;
			u_short	u2;
			for( u_char uCount = 0; uCount < uSize; uCount++ )
			{
				ar >> uParts;
				ar >> u2;
			#ifdef __FINITEITEM0705
				m_aEquipInfo[uParts].dwId	= (DWORD)u2;
			#else	// __FINITEITEM0705
				m_adwEquipment[uParts]	= (DWORD)u2;
			#endif	// __FINITEITEM0705
			}

			m_bActiveAttack	=
#ifdef __XFLYAI0524
			m_nMovePattern	=
			m_nMoveEvent	=
#endif	// __XFLYAI0524
									0;

			ar >> (u_char&)m_bActiveAttack;
#ifdef __XFLYAI0524
			ar >> (u_char&)m_nMovePattern;
			ar >> (u_char&)m_nMoveEvent;
			ar >> m_nMoveEventCnt;
#endif	// __XFLYAI0524
			ar.ReadString( m_szCharacterKey );
#ifdef __CLIENT
			if( *m_szCharacterKey )
			{
				LPCHARACTER pCharacter	= prj.GetCharacter( m_szCharacterKey );
				if( pCharacter )
				{
					_tcscpy( m_szName, pCharacter->m_strName );
					AllocShopInventory( pCharacter );
				}
			}
#endif	// __CLIENT
		}

		m_SkillState.Serialize( ar );
		
#ifdef __ONLINE
		InitInterpolation();
#endif	// __ONLINE
	}
#ifdef __CLIENT	
	m_fDestScale = GetScale().x;
	if( m_fDestScale > 10.0f )
	{
		LPCTSTR szErr = Message( "SerializeOpt:%f %s", m_fDestScale, GetName() );
		ADDERRORMSG( szErr );
	}
#endif

#endif //!__BEAST
}
