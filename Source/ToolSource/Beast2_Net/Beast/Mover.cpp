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



const int	MAX_DIALOGFILENAME = 32;

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
{
	Init();
}

CMover::~CMover()  
{
	SAFE_DELETE( m_pNpcProperty );

	SAFE_DELETE( m_pActMover );

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		SAFE_DELETE( m_ShopInventory[ i ] );

	if( GetWorld() )
	{
	#if !defined(__BEAST) && !defined(__ONLINE)
		//{{AFX
			GetWorld()->m_respawner.Increment( GetRespawn(), m_bActiveAttack );
		//}}AFX
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

	memset( m_adwEquipment, 0xff, sizeof( m_adwEquipment ) );

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

	m_aQuest	        = NULL;//new QUEST[ prj.m_aPropQuest.GetSize() ];
	m_aCompleteQuest    = NULL; 
	m_nCompleteQuestSize = 0; 

	m_idparty			= 0;
//	m_dwContribution	= 0;

	memset(	m_anEquipOption, 0, sizeof(m_anEquipOption) );

	m_dwExtraFlag		= 0;

	m_idGuild			= 0;
	m_idGuildCloak		= 0;
	m_bGuildBank		= FALSE;
	m_idWar				= 0;

	m_nPlusMaxHitPoint = 0;


	m_nAttackResistLeft = (BYTE)0xff;
	m_nAttackResistRight = (BYTE)0xff;
	m_nDefenseResist = (BYTE)0xff;

	m_nReflexDmg = 0;

	m_pActMover = new CActionMover( this );
	m_pActMover->SetMover( this );

	m_timerRecovery.Set( SEC(6) );
	m_timerRecoveryStand.Set( SEC(8) );

	m_timerDuel.Set( SEC(1) );
	m_timerEndDuel.Set( MIN(3) );
	m_nDuelState = 0;

	m_bRegenItem		= TRUE;
	m_bActiveAttack		= FALSE;
	m_dwGold			= 0;		// 0으로 할것.  -xuzhu-

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		m_ShopInventory[ i ] = 0;
	
	m_lpfGetResist[SAI79::NO_PROP		]	= &CMover::GetResistNULL;
	m_lpfGetResist[SAI79::FIRE			]	= &CMover::GetResistFire;
	m_lpfGetResist[SAI79::WATER			]	= &CMover::GetResistWater;
	m_lpfGetResist[SAI79::ELECTRICITY	]	= &CMover::GetResistElectricity;
	m_lpfGetResist[SAI79::WIND			]	= &CMover::GetResistWind;
	m_lpfGetResist[SAI79::EARTH			]	= &CMover::GetResistEarth;

	m_pNpcProperty = NULL;
	m_dwSkillPoint = 0;
	
	m_fCrrSpd	= 0.0f;
	m_pIADestPos = NULL;

	m_idPet = NULL_ID;

}

BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	if( nGold == 0 )
		return TRUE;

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
	}

	return TRUE;
}


// 새로운 머리색깔이 적용되기 전에 이함수를 사용해야함...
// 원본 머리색깔을 참고하기 때문...
int	CMover::GetHairCost( CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair )
{
	BYTE nOrignalR = (BYTE)( pMover->m_fHairColorR * 255 );
	BYTE nOrignalG = (BYTE)( pMover->m_fHairColorG * 255 );
	BYTE nOrignalB = (BYTE)( pMover->m_fHairColorB * 255 );

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
			BYTE nId = 0, nCount = 0;
			short nNum = 0;
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
		}
		return m_pNpcProperty->LoadDialog( lpCharacter );
	}
	else
	{
		return FALSE;
	}
}

void CMover::ProcessQuest()
{

}

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
			for( int i = 0; i < nJobNum; i++ )
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

				for( int j = 1 ; j < (int)( pSkill->dwLevel ) ; ++j )
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
	for( int i = MAX_JOB_SKILL ; i < MAX_SKILL_JOB ; ++i )
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

	return TRUE;
}


void CMover::ReState()
{

}


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
					( (CItemElem*)pItemBase )->m_nItemNum	= (short)( dwValue );
				}
				break;
			}
		default:
			break;
		}
	}
}


CItem *CMover::_DropItemNPC( DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos )
{

	return NULL;
}

// 아이템을 떨굴 수 있는가? PK - PK를 당한 경우는 TRUE
BOOL CMover::IsDropableState( BOOL bPK )
{
	if( IsInvalidObj(this) )	
		return FALSE;

	return TRUE;
}	


// 인벤의 아이템을 바닥에 내려놓을때 사용한다.
// dwType, dwID의 아이템을 vPos에 떨어트린다.
// bPK == YES면 플레이어가 PK당해서 떨어트린 아이템이다.
CItem* CMover::DropItem( DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos, BOOL bPK )
{

	return NULL;
}

// TODO_OPTIMIZE: 좌표가 변경될 때 호출되게한다. ( rect를 트리구조로 갖고 찾게 하는 것도 좋겠다.)
REGIONELEM* CMover::UpdateRegionAttr()
{
	REGIONELEM* pPortkey = NULL;

	return pPortkey;
}


void CMover::ProcessRegion()
{
}
void CMover::Execute()
{
	CObj::Execute();
}

BOOL CMover::Read( CFileIO* pFile )
{
	CObj::Read( pFile );

	char szDialogFile[MAX_DIALOGFILENAME] = {0, };;

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
		CItemElem* pItemElem = (CItemElem*)m_Inventory.GetEquip( nParts ); 
		if( pItemElem )
			return pItemElem->GetProp();
		return prj.GetItemProp( II_WEA_HAN_HAND );
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

ItemProp* CMover::GetEquipItemProp( CItemContainer<CItemElem>* pInventory, LPDWORD padwEquipment, int nParts )
{
	ItemProp *pItemProp = NULL;
#if 1	
	if( pInventory == NULL  )
	{
		if( padwEquipment[nParts] != NULL_ID )
			pItemProp = prj.GetItemProp( padwEquipment[nParts] );
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
	if( IsPlayer() )
	{
		CItemElem *pItemElemR = m_Inventory.GetEquip( PARTS_RWEAPON );
		CItemElem *pItemElemL = m_Inventory.GetEquip( PARTS_LWEAPON );
		if( pItemElemR && pItemElemL )
			return TRUE;
		else
			return FALSE;
	} else
	{
		if( m_adwEquipment[ PARTS_RWEAPON ] != NULL_ID && m_adwEquipment[ PARTS_LWEAPON ] != NULL_ID )
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

#define PROFILE( stopWatch, value ) ((void)0);
#define PROCESS_LOG( n, value )		((void)0);

void CMover::Process(FLOAT fElapsedTime)
{	

}

void CMover::GenerateVendorItem( ItemProp** apItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor )
 //int nItemKind, int nItemKind2, int nNumMax, int nUniqueMin, int nUniqueMax, int nTotalNum, CAnim* pAnimParent, int nMaterialCount )
{

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
						for( int j = 0; j < cbSize - 1; j++ )
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
						for( int j = 0; j < cbSize; j++ )
						{
							CItemElem itemElem;
							itemElem.m_dwItemId	= apItemProp[j]->dwID;
							itemElem.m_nItemNum		= (short)( apItemProp[j]->dwPackMax );
//							itemElem.m_nHitPoint	= -1;
							itemElem.m_nHitPoint	= apItemProp[j]->dwEndurance;
							if( m_ShopInventory[i]->Add( &itemElem ) == FALSE )
								break;
						}
					}
				}
			}

		}
	}
}


void CMover::EnforcedGhostCorr( void )
{

}

void CMover::ApproachGhostAngle( void )
{

}

void CMover::OnArriveAtPos( void )
{  

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

}

void CMover::RemoveItemBankId( int nSlot, DWORD dwId )
{
	m_Bank[nSlot].RemoveAtId( dwId );
}



// bSWDForce대신에 Option을 써서 비트로 사용하자.
// 0x01 : 검광
// 0x02 : 고정모션(+100안하는..)
// 0x04 : Motion Transition 사용하지 않음.
BOOL CMover::SetMotion( DWORD dwMotion, int nLoop, DWORD dwOption )
{

	return TRUE;
}
BOOL CMover::InitMotion(DWORD dwMotion)
{
	prj.m_modelMng.LoadMotion( m_pModel, m_dwType, m_dwIndex, dwMotion );
	m_dwMotion = dwMotion;
	return TRUE;
}


CGuild* CMover::GetGuild( void )
{


	return NULL;
}

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
	return FALSE;
}

void CMover::ClearDuel( void )
{
	m_nDuel		= 0;
	m_idDuelOther	= NULL_ID;
}

void CMover::ClearDuelParty( void )
{
	m_nDuel		= 0;
	m_idDuelParty	= 0;

}

void CMover::SetPKTargetLimit( int nSec )
{
}


BOOL CMover::IsAfterDeath()	
{	
	return ( m_nDeathLevel > m_nLevel || ( m_nDeathLevel == m_nLevel && m_nDeathExp > m_nExp1 ) )? TRUE: FALSE;	
}


#ifdef  __V050322_CACHEQUEST
BOOL CMover::NoDisguise( LPDIRECT3DDEVICE9 pd3dDevice )     
{ 
	return TRUE;
}

BOOL CMover::Disguise( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex )
{
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