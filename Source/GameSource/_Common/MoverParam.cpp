#include "stdafx.h"
#include "defineText.h"
#include "resdata.h"
#include "Project.h"
#include "ProjectCmn.h"

#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef __WORLDSERVER
#include "DialogMsg.h"
#include "../WorldServer/user.h"
#include "spevent.h"
#include "../WorldServer/FLCC_Reward.h"
#include "../WorldServer/slord.h"
#include "PCBang.h"
#include "GuildHouse.h"
#include "../WorldServer/CampusHelper.h"
#include "../WorldServer/FLCC_Reward.h"
#include "../worldserver/FLMadrigalGift.h"
#include "../_Common/SeasonEffect.h"
#endif	// __WORLDSERVER

#ifdef __CLIENT			
#include "Dialogmsg.h"
#include "defineSound.h"
#endif



LPSKILL CMover::GetSkill( int /*nType*/, int nIdx )
{
	// nType == 0이면 직업 스킬
	// nType == 1이면 라이선스 스킬 
	if( nIdx >= MAX_SKILL_JOB )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s nId = %d 범위를 벗어남" ), m_szName, nIdx );
		nIdx = 0;
	}
	return &m_aJobSkill[ nIdx ];
}
LPSKILL CMover::GetSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return &m_aJobSkill[i];
	}
	return NULL;
}

int		CMover::GetSkillIdx( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return i;
	}
	return -1;
}


BOOL CMover::CheckSkill( DWORD dwSkill )
{
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	if( pSkillProp == NULL )
		return FALSE;
//sun: 10차 전승시스템	Neuz, World, Trans

	if( IsMaster() == FALSE && IsHero() == FALSE && IsLegendHero() == FALSE && GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
	{
		return FALSE;
	}

	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReqSkill1 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel1 )
			{
				return FALSE;
			}
		}
	}
	
	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReqSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel2 )
			{
				return FALSE;
			}				
		}
	}	
	
	return TRUE;
}

bool	CMover::isLearnSkill( const int job_skill_array_index ) const
{
	if( job_skill_array_index < 0 || job_skill_array_index >= MAX_SKILL_JOB ) {
		return false;
	}

	const SkillProp* pSkillProp	= prj.GetSkillProp( m_aJobSkill[job_skill_array_index].dwSkill );
	if( pSkillProp == NULL ) {
		return false;
	}

	return m_aJobSkill[job_skill_array_index].dwLevel > 0;
}


// 케릭터가 레벨업시 스킬중에 케릭터레벨땜시 활성화 안된것들 활성여부 체크하여 정보출력
#ifdef __CLIENT
void CMover::PutLvUpSkillName_1( DWORD dwLevel )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;
		
		SkillProp* pSkillProp = prj.GetSkillProp( m_aJobSkill[i].dwSkill );
		
		if( pSkillProp && dwLevel == pSkillProp->dwReqDisLV )
		{
			if( pSkillProp->dwReqSkill1 != NULL_ID )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReqSkill1);

				if( pSkillProp->dwReqSkillLevel1 > pSkill->dwLevel )
					continue;
			}
			if( pSkillProp->dwReqSkill2 != NULL_ID )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReqSkill2);
				
				if( pSkillProp->dwReqSkillLevel2 > pSkill->dwLevel )
					continue;
			}

			CString str;
			str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );
		}
	}
}		
#else
void CMover::PutLvUpSkillName_1( DWORD /*dwLevel*/ )
{
}
#endif //__CLIENT

BOOL IsSkillParent( CMover* pMover, DWORD dwSkill, DWORD dwChildSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReqSkill1 == dwChildSkill || pSkillProp->dwReqSkill2 == dwChildSkill )
		return TRUE;

	return FALSE;
}

BOOL IsActive( CMover* pMover, DWORD dwSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pSkill1;
		SkillProp* pSkillProp1;

		pSkill1		= pMover->GetSkill( pSkillProp->dwReqSkill1 );
		if( pSkill1 == NULL )
			return FALSE;

		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		if( pSkillProp1 == NULL )
			return FALSE;

//sun: 10차 전승시스템	Neuz, World, Trans

		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && pMover->IsMaster() == FALSE && pMover->IsHero() == FALSE && pMover->IsLegendHero() == FALSE )
		{
			return FALSE;
		}

		if( pSkillProp->dwReqSkillLevel1 != pSkill1->dwLevel )
			return FALSE;
	}

	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pSkill1;
		SkillProp* pSkillProp1;
		
		pSkill1		= pMover->GetSkill( pSkillProp->dwReqSkill2 );
		if( pSkill1 == NULL )
			return FALSE;
	
		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		if( pSkillProp1 == NULL )
			return FALSE;

//sun: 10차 전승시스템	Neuz, World, Trans
		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && pMover->IsMaster() == FALSE && pMover->IsHero() == FALSE && pMover->IsLegendHero() == FALSE )
		{
			return FALSE;
		}

		if( pSkillProp->dwReqSkillLevel2 != pSkill1->dwLevel )
			return FALSE;		
	}

//sun: 10차 전승시스템	Neuz, World, Trans
	if( (int)( pSkillProp->dwReqDisLV ) > pMover->GetLevel() && pMover->IsMaster() == FALSE && pMover->IsHero() == FALSE && pMover->IsLegendHero() == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}


// 스킬 레벨업시 케릭터 레벨땜시 안나왔던 스킬들 검색하여 정보 출력
#ifdef __CLIENT
void CMover::PutLvUpSkillName_2( DWORD dwSkill )
{
	LPSKILL pSkill = GetSkill( dwSkill );
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	//현재 활성화 되어있는냐?
	//나를 갖고있는 스킬을 검사하여 조건에 맞으면 그스킬을 활성화 한다.
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;

		if( IsSkillParent( this, m_aJobSkill[i].dwSkill, dwSkill ) )
		{
			if( IsActive( this, m_aJobSkill[i].dwSkill ) )
			{
				SkillProp* pSkillProp1;
				pSkillProp1 = prj.GetSkillProp( m_aJobSkill[i].dwSkill );

				CString str;
				str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp1->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );	
			}
		}
	}
}
#else
void CMover::PutLvUpSkillName_2( DWORD /*dwSkill*/ )
{

}
#endif // CLIENT

#ifdef __WORLDSERVER
/// 거래, 죽음, 개인상점중, 전투중 FALSE
int CMover::GetSummonState()
{
	// 거래
	if( m_vtInfo.GetOther() != NULL )
		return 1;
	  // 창고
	if( m_bBank )
		return 1;
	  // 길드창고
	if( m_bGuildBank )
		return 1;

	// 죽음
	if( IsDie() )
		return 2;

	// 개인상점중
	if( m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen() )
		return 3;

	// 전투중
	if( IsAttackMode() )
		return 4;
	
	// 비행중
	if( ( IsFly() ) )
		return 5;


//sun: 8, // 8차 듀얼 061226 ma
	if( m_nDuel )
		return 6;

	return 0;
}
#endif //


#ifdef __CLIENT
LPCTSTR CMover::GetJobString()
{
	if( m_nJob != -1 )
		return prj.m_aJob[ m_nJob ].szName;
	return _T( "" );
}
#endif	// __CLIENT

BOOL  CMover::IsJobType( DWORD dwJobType ) 
{ 
	if( IsExpert() )
	{
		if( dwJobType == JTYPE_EXPERT ) 
			return TRUE;
		return FALSE;
	}
	return JTYPE_BASE == dwJobType;
}
BOOL CMover::IsBaseJob()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_BASE;
}
BOOL CMover::IsExpert()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_EXPERT;
}
BOOL CMover::IsPro()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_PRO;
}

//sun: 10차 전승시스템	Neuz, World, Trans
BYTE	CMover::GetLegendChar()
{
	if(IsMaster())
		return LEGEND_CLASS_MASTER;
	else if(IsHero())
		return	LEGEND_CLASS_HERO;
	else if( IsLegendHero() == TRUE )
	{
		return LEGEND_CLASS_LEGEND_HERO;
	}
	else
		return LEGEND_CLASS_NORMAL;
}

BOOL	CMover::IsMaster()
{
	return prj.m_aJob[ m_nJob ].dwJobType == 5;//temp JTYPE_MASTER;
}

BOOL	CMover::IsHero()
{
	return prj.m_aJob[ m_nJob ].dwJobType == 6;//temp JTYPE_HERO;
}

BOOL	CMover::IsLegendClass()
{
	DWORD dwJobType = prj.m_aJob[ m_nJob ].dwJobType;
	if( dwJobType != NULL_ID && dwJobType >= JTYPE_HERO )
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CMover::IsLegendHero()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_LEGEND_HERO;
}


BOOL CMover::IsInteriorityJob( int nJob )
{
	if( nJob == JOB_VAGRANT || nJob == m_nJob )
		return TRUE;

	if( IsPro() && JOB_VAGRANT < nJob && nJob < MAX_EXPERT )
	{
		if( nJob * 2 + 4 == m_nJob || nJob * 2 + 5 == m_nJob )
		{
			return TRUE;
		}
	}
//sun: 10차 전승시스템	Neuz, World, Trans
	if( IsMaster()  )	// 레벨이 안되면 못참.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 14 == m_nJob || nJob * 2 + 15 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 10 == m_nJob )
				return TRUE;
		}
	}
	if( IsHero()  )	// 레벨이 안되면 못참.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 22 == m_nJob || nJob * 2 + 23 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 18 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_MASTER )	// Hero는 마스터 무기를 장착할 수 있다.
		{
			if( nJob + 8 == m_nJob )
				return TRUE;
		}
	}

	if( IsLegendHero() == TRUE )	// 레벨이 안되면 못참.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 30 == m_nJob || nJob * 2 + 31 == m_nJob )
			{
				return TRUE;
			}
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 26 == m_nJob )
			{
				return TRUE;
			}
		}
		else if( nJob < MAX_MASTER )	// NewHero는 마스터 무기를 장착할 수 있다.
		{
			if( nJob + 16 == m_nJob )
			{
				return TRUE;
			}
		}
		else if( nJob < MAX_HERO )	// NewHero는 히어로 무기를 장착할 수 있다.
		{
			if( nJob + 8 == m_nJob )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 스킬수 3(기본)14(익스퍼트)7(프로페셔널)
// 직업 10, 익스퍼트 40. 프로 30
BOOL  CMover::SetExpert( int nJob )
{
	if( m_nJob == nJob )
		return FALSE;
	m_nJob = nJob;

	int nNum = 3; // 3은 기본 스킬의 갯수다. 3 이상부터 익스퍼트 스킬이 추가된다는 의미 
	LPSKILL lpSkill;
	SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
	int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
	for( int i = 0; i < nJobNum; i++ )
	{
		SkillProp* pSkillProp = apSkillProp[ i ];
		lpSkill = &m_aJobSkill[ nNum++ ];
		lpSkill->dwSkill = pSkillProp->dwID;
	}
	return TRUE;
}

int   CMover::GetJob()
{
	return m_nJob; 
}

int   CMover::GetExpPercent()
{
	int nExpPercent = 0;

	nExpPercent = (int)( GetExp1() * 10000 / GetMaxExp1() );

	return nExpPercent; 
}


int   CMover::GetRemainGP()
{
	return m_Stat.GetRemainStatPoint();
}

// 명중률 하락 상태는 이큅이벤트와 상관없이 올수 있기때문에 GetParm()을 여기서 최종 계산해야한다.
int	CMover::GetAdjHitRate( void )
{
	return GetParam( DST_ADJ_HITRATE, m_nAdjHitRate );
}

// 회피율 
int CMover::GetParrying()
{
	if( IsPlayer() )
		return	( (int)( (GetDex() * 0.5f) + GetParam( DST_PARRY, m_nAdjParry ) ) );
	else
		return GetProp()->dwER;
}


void CMover::UpdateParam()
{
	if( IsPlayer() ) 
	{
		SumEquipDefenseAbility( &m_nDefenseMin, &m_nDefenseMax ); 
		SumEquipAdjValue( &m_nAdjHitRate, &m_nAdjParry );
	}
	else
	{
		m_nDefenseMin = m_nDefenseMax = GetNaturalArmor();
		m_nAdjHitRate = 0;
		m_nAdjParry = 0;
	}
}
// 아이템의 성능을 얻는다. 
int CMover::GetItemAbility( int nItem )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	if( nAbilityMax == nAbilityMin )
		return nAbilityMin;
	return nAbilityMin + ( xRand() % (nAbilityMax - nAbilityMin ) );
}
// 아이템의 최소 성능을 얻는다.
int CMover::GetItemAbilityMin( int nItem )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	return nAbilityMin;
}
// 아이템의 최대 성능을 얻는다.
int CMover::GetItemAbilityMax( int nItem )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItem );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	return nAbilityMax;
}

// 히트포인트 증감
void CMover::IncHitPoint(int nVal) 
{ 
	SetPointParam( DST_HP, m_nHitPoint + nVal );
}
// 마나포인트 증감
void CMover::IncManaPoint(int nVal) 
{ 
#ifdef __WORLDSERVER 			
	if( IsSMMode( SM_MAINTAIN_MP ) && nVal < 0 )
		return;
#endif // __WORLDSERVER
	SetPointParam( DST_MP, m_nManaPoint + nVal );
}

void CMover::IncFatiguePoint(int nVal) 
{ 
#ifdef __WORLDSERVER
	if( IsSMMode( SM_MAINTAIN_FP ) && nVal < 0 )
		return;
#endif // __WORLDSERVER
	SetPointParam( DST_FP, m_nFatiguePoint + nVal );
}

// 듀얼후의 결과 처리.
// this가 승자가 된다.
// pLose는 패배자.

#ifdef __WORLDSERVER

void CMover::ChangeFame( CMover *pLose )
{
	int nVal = 0;
	float fVal = 0;
	int nGap = 0;

	nGap = pLose->GetLevel() - GetLevel();		// 패자와 승자의 레벨차이

	if( nGap >= 80 )	    { nVal = 100; fVal = 15.0f; }	// 패자가 레벨이 훨 높았다.
	else if( nGap >= 30 )	{ nVal = 50;  fVal = 10.0f; }
	else if( nGap >= 20 )	{ nVal = 30;  fVal = 8.0f;  }
	else if( nGap >= 10 )	{ nVal = 10;  fVal = 6.0f;  }
	else if( nGap <= -80 )	{ nVal = 1;   fVal = 1.0f;  }	// 패자가 레벨이 훨 낮았다.
	else if( nGap <= -30 )	{ nVal = 2;   fVal = 2.0f;  }
	else if( nGap <= -20 )	{ nVal = 3;   fVal = 3.0f;  }
	else if( nGap <= -10 )	{ nVal = 4;   fVal = 4.0f;  }
	else                    { nVal = 5;   fVal = 5.0f;  }	// 9 ~ -9


	if( nVal )		// 승자에게 명성치 줌
	{	
		if( this->IsFly() && pLose->IsFly() )	// 공중 듀얼의 경우에는 패배시 상실하는 명성 포인트는 동일하지만, 승리 시 획득하는 명성포인트는 지상 전투에 2배로 한다
			nVal *= 2;
			
		m_nFame += nVal;	
		g_xWSUserManager->AddSetFame( this, m_nFame );
		((FLWSUser*)this)->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nVal );	// xxx 명성 포인트를 얻었다
//sun: 13, 달인
		((FLWSUser*)this)->CheckHonorStat();
		((FLWSUser*)this)->AddHonorListAck();
		g_xWSUserManager->AddHonorTitleChange( this, m_nHonor);
	}
	
	if( fVal )	// 패자의 명성치(퍼센트)를 깎음.
	{
		int		nDecVal;
		fVal = fVal / 100.0f;	// 비율로 환산.
		nDecVal = (int)(pLose->m_nFame * fVal);
		if( nDecVal <= 0 )	nDecVal = 1;
		pLose->m_nFame -= nDecVal;
		if( pLose->m_nFame < 0 )
			pLose->m_nFame = 0;

		g_xWSUserManager->AddSetFame( pLose, pLose->m_nFame );
		((FLWSUser*)pLose)->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx 명성 포인트가 감소.
		g_dpDBClient.SendLogPkPvp( this, pLose, nVal, 'D' );
//sun: 13, 달인
		((FLWSUser*)pLose)->CheckHonorStat();
		((FLWSUser*)pLose)->AddHonorListAck();
		g_xWSUserManager->AddHonorTitleChange( pLose, pLose->m_nHonor);
	}
}
#endif // __WORLDSERVER

int	CMover::DuelResult( CMover *pLose )
{
#ifdef __WORLDSERVER
	if( pLose->m_nDuel == 1 )
	{
		if( pLose->GetId() != m_idDuelOther )
			return 0;
	}

	// 승자는 레벨차에따라 경험치 상승
	// 패자는 레벨차에따라 경험치% 하락
	if( pLose->m_nDuel == 1 )	// 개인 PvP
	{
		ChangeFame( pLose );
		
		pLose->m_nDead = PROCESS_COUNT * 2;		// 종료 후 2초간은 무적
		pLose->RemoveAttackerBuffs( pLose->m_idDuelOther );
		m_nDead = PROCESS_COUNT * 2;		// 종료 후 2초간은 무적
		RemoveAttackerBuffs( m_idDuelOther );
	}
	else
	// 극단 듀얼.
	if( pLose->m_nDuel == 2 )		// 극단 듀얼중일땐 극단장끼리의 레벨을 비교.
	{
		int nVal = 0;
		float fVal = 0;
		int nGap = 0;

		CParty *pLoseParty = g_PartyMng.GetParty( pLose->m_idparty );
		if( pLoseParty == NULL || pLoseParty->IsLeader( pLose->m_idPlayer ) == FALSE )		// 파티가 없거나 죽은넘이 대장이 아니면 걍 리턴.
			return 0;
		
		CParty *pParty = g_PartyMng.GetParty( m_idparty );		// 이긴측 파티.
		if( pParty == NULL )
			return 0;

		CMover* pLeader	= pParty->GetLeader();
		if( IsValidObj( pLeader ) )
			nGap	= pLose->GetLevel() - pLeader->GetLevel();
		else
			nGap	= pLose->GetLevel() - GetLevel();
		
		if( nGap >= 80 )	{ nVal = 100; fVal = 15.0f; }		// 패자가 레벨이 훨 높았다.
		else if( nGap >= 50 )	{ nVal = 50; fVal = 10.0f; }
		else if( nGap >= 20 )	{ nVal = 30; fVal = 8.0f; }
		else if( nGap >= 10 )	{ nVal = 10; fVal = 6.0f; }
		else if( nGap <= -80 )	{ nVal = 1; fVal = 1.0f; }		// 패자가 레벨이 훨 낮았다.
		else if( nGap <= -50 )	{ nVal = 2; fVal = 2.0f; }
		else if( nGap <= -20 )	{ nVal = 3; fVal = 3.0f; }
		else if( nGap <= -10 )	{ nVal = 4; fVal = 4.0f; }
		else { nVal = 5; fVal = 5.0f; }	// 9 ~ -9

		g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pLoseParty->m_uPartyId, FALSE );

		pLose->SetDecreaseExperienceRevival( false );
		pLoseParty->DoDuelResult( pParty, 0, nVal, fVal );		// 파티원들에게 패배사실을 알리고 듀얼 중지시킴.
		pParty->DoDuelResult( pLoseParty, 1, nVal, fVal );		// 파티원들에게 승리를 알림.
	}
#endif	// __WORLDSERVER

	return 1;
}

// 히트포인트를 셋하기
void CMover::SetHitPoint(int nVal) 
{
	m_nHitPoint = nVal;
	if( m_nHitPoint < 0 )
		m_nHitPoint = 0;
	if( m_nHitPoint > GetMaxHitPoint() )
		m_nHitPoint = GetMaxHitPoint(); 
}
// 마나 포인트를 셋하기 
void CMover::SetManaPoint(int nVal) 
{
	m_nManaPoint = nVal;
	if( m_nManaPoint < 0 )
		m_nManaPoint = 0;
	if( m_nManaPoint > GetMaxManaPoint() )
		m_nManaPoint = GetMaxManaPoint(); 
}
// 피지칼 포인트를 셋하기 
void CMover::SetFatiguePoint(int nVal) 
{
	m_nFatiguePoint = nVal;
	if( m_nFatiguePoint < 0 )
		m_nFatiguePoint = 0;
	if( m_nFatiguePoint > GetMaxFatiguePoint() )
		m_nFatiguePoint = GetMaxFatiguePoint(); 
}

int CMover::GetPointParam(int nDestParameter )
{
	switch( nDestParameter )
	{
		case DST_HP:	return( GetHitPoint() );
		case DST_MP:	return( GetManaPoint() );
		case DST_FP:	return( GetFatiguePoint() );
		case DST_GOLD:	return( GetGold() );
	}
	return( 0 );
}

void CMover::SetPointParam( int nDstParameter, int nValue, BOOL bTrans )	// bTrans가 TRUE이면 강제전송
{
#ifdef __WORLDSERVER
	int nMax = nValue;
#endif // __WORLDSERVER
	if( nValue < 0 )	nValue	= 0;
	BOOL	bTransfer	= bTrans;

	switch( nDstParameter )
	{
		case DST_HP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxHitPoint() || nMax == 999999999 )	nValue	= GetMaxHitPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxHitPoint() )	nValue	= GetMaxHitPoint();
#endif // __WORLDSERVER
			if( m_nHitPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nHitPoint		= nValue;
			}
			break;
		case DST_MP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxManaPoint() || nMax == 999999999 )	nValue	= GetMaxManaPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxManaPoint() )	nValue	= GetMaxManaPoint();
#endif // __WORLDSERVER
			if( m_nManaPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nManaPoint	= nValue;
			}
			break;
		case DST_FP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() || nMax == 999999999 )	nValue	= GetMaxFatiguePoint();
#else // __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() )		nValue	= GetMaxFatiguePoint();
#endif // __WORLDSERVER
			if( m_nFatiguePoint != nValue )
			{
				bTransfer	= TRUE;
				m_nFatiguePoint		= nValue;
			}
			break;
		case DST_GOLD:
			//m_dwGold	= (DWORD)nValue;
//#ifdef __PERIN_BUY_BUG	// chipi_091219 추가
//#ifdef __WORLDSERVER
#if defined( __PERIN_BUY_BUG ) && defined( __WORLDSERVER )
			FLERROR_LOG( PROGRAM_NAME, _T( "SetPointParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]" ), static_cast<FLWSUser*>(this)->GetName(), static_cast<FLWSUser*>(this)->m_idPlayer, nValue );
			return;
//#endif // __WORLDSERVE
//#endif // __PERIN_BUY_BUG
#else

	#ifdef __CLIENT
			if( IsActiveMover() )
				PLAYSND(SND_INF_TRADE);
	#endif //__CLIENT
			bTransfer	= TRUE;
			SetGold( nValue );
			break;
#endif //defined( __PERIN_BUY_BUG ) && defined( __WORLDSERVER )
	}
#ifdef __WORLDSERVER
	if( bTransfer )
		g_xWSUserManager->AddSetPointParam( this, nDstParameter, nValue );
#endif	// __WORLDSERVER
}

BOOL CMover::AddExperience( EXPINTEGER nExp, BOOL bFirstCall, BOOL bMultiPly, BOOL bMonster )
{
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( g_xFlyffConfig->GetMainLanguage() == LANG_VTN )
	{
		if( IsPlayer() && m_nAccountPlayTime != -1 )
		{
			if( m_nAccountPlayTime < 0 || m_nAccountPlayTime > MIN( 300 ) )
			{
				nExp	= 0;
			}
			else if( m_nAccountPlayTime >= MIN( 180 ) && m_nAccountPlayTime <= MIN( 300 ) )
			{
				nExp	= (EXPINTEGER)( nExp * 0.5f );
			}
		}
	}
	//	mulcom	END100315	베트남 시간 제한

	if( nExp <= 0 )		// nExp가 마이너스이거나 0이면 처리할필요 없음.
		return FALSE;

	if( m_nHitPoint <= 0 )
		return FALSE;

#ifdef __INTERNALSERVER
	FLTRACE_LOG( PROGRAM_NAME, _T( "EXP = %I64d" ), nExp );
#endif	// __INTERNALSERVER

	if( IsMaster() || IsHero() )
		nExp /= 2;
	if( bFirstCall && bMultiPly )
	{
#ifdef __WORLDSERVER
		EXPINTEGER nAddExp = static_cast<FLWSUser*>( this )->GetAddExpAfterApplyRestPoint( nExp );
		nExp	= (EXPINTEGER)( nExp * GetExpFactor() );
		nExp += nAddExp;
#endif // __WORLDSERVER
	}

	//sun: 8차 엔젤 소환 Neuz, World, Trans
	if( bFirstCall && HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
		IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
		if( wId > 0 )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( wId );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal[0] );	// TODO : 효과 적용 수치로 빼자
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;
		// 康: 전직 전 엔젤 버프 활성화 후 전직하는 경우, 필요 수치가 너무 크다.
		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[m_nAngelLevel].nExp1 / 100 * nAngel;
		if( m_nAngelExp < nMaxAngelExp )
		{
			//////////////////////////////////////////////////////////////////////////
// 			if( HasBuffByIk3( IK3_PKPENALTYSCROLL_C ) == TRUE )
// 			{
// 				m_nAngelExp += ( nExp / 2 );
// 			}
// 			else
// 			{
// 				nExp /= 2;
// 				m_nAngelExp += nExp;
// 			}
 			nExp /= 2;
 			m_nAngelExp += nExp;
			//////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
			// 엔젤 관련 로그
			int nAngelExpPercent = (int)( m_nAngelExp * 100 / nMaxAngelExp );
			int nNextAngelExpLog = (int)(((FLWSUser*)this)->m_nAngelExpLog / 20 + 1) * 20;
			
			if( nAngelExpPercent >= nNextAngelExpLog )
			{
				((FLWSUser*)this)->m_nAngelExpLog = nAngelExpPercent;
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( wId );
				
				if( pItemProp )
				{
					LogItemInfo aLogItem;
					//aLogItem.Action		= "&";
					//aLogItem.SendName	= ((FLWSUser*)this)->GetName();
					//aLogItem.RecvName	= "ANGEL_EXP_LOG";
					FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "&" );
					FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), ((FLWSUser*)this)->GetName() );
					FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ANGEL_EXP_LOG" );

					aLogItem.WorldId	= ((FLWSUser*)this)->GetWorld()->GetID();
					aLogItem.Gold		= aLogItem.Gold2 = ((FLWSUser*)this)->GetGold();
					//aLogItem.ItemName	= pItemProp->szName;
					FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", pItemProp->dwID );
					aLogItem.Gold_1		= (DWORD)( m_nAngelExp );
					g_DPSrvr.OnLogItem( aLogItem );
				}
			}
			
			BOOL bAngelComplete = FALSE;
			if( m_nAngelExp > nMaxAngelExp )
			{
				m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			((FLWSUser*)this)->AddAngelInfo( bAngelComplete );
#endif // __WORLDSERVER
		}
	}

	if( IsBaseJob() == TRUE )
	{
		if( m_nLevel >= MAX_JOB_LEVEL )
		{
			m_nLevel	= MAX_JOB_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
	}
	else if( IsExpert() == TRUE )
	{
		if( m_nLevel >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
		{
			m_nLevel	= MAX_JOB_LEVEL + MAX_EXP_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
	}
	else if( IsPro() == TRUE )
	{
		if( m_nLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel	= MAX_GENERAL_LEVEL;
			m_nExp1		= ( prj.m_aExpCharacter[m_nLevel + 1].nExp1 - 1 );
			return FALSE;
		}		
	}
//sun: 10차 전승시스템	Neuz, World, Trans
	else if( IsMaster() == TRUE )
	{
		if( m_nLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel	= MAX_GENERAL_LEVEL;
			m_nExp1		= ( prj.m_aExpCharacter[m_nLevel + 1].nExp1 - 1 );
			return FALSE;
		}
	}
	else if( IsHero() == TRUE )
	{
		if( m_nLevel >= MAX_LEGEND_LEVEL )
		{
			m_nLevel	= MAX_LEGEND_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
	}
	else if( IsLegendHero() == TRUE )
	{
//#if __VER >= 17
		if( m_nLevel >= MAX_3RD_LEGEND_LEVEL )
		{
			m_nLevel	= MAX_3RD_LEGEND_LEVEL;
			m_nExp1		= 0;
			return FALSE;
		}
// #else // __VER >= 17
// 		if( m_nLevel > MAX_3RD_LEGEND_LEVEL )
// 		{
// 			m_nLevel	= MAX_3RD_LEGEND_LEVEL;
// 			m_nExp1		= ( prj.m_aExpCharacter[m_nLevel + 1].nExp1 - 1 );
// 			return FALSE;
// 		}
// #endif // __VER >= 17
	}
	
//	int nLevelbuf = m_nLevel;
	int nNextLevel = m_nLevel + 1;

	m_nExp1 += nExp;									// pxp와 관계없이 exp가 증가. 만쉐이!

// #ifdef __WORLDSERVER
// 	const bool bChaoPlayer	= ( IsChaotic() == TRUE && static_cast<FLWSUser*>( this )->HasBuffByIk3( IK3_PKPENALTYSCROLL_C ) == FALSE ) ?
// 							true : false;
// #endif // __WORLDSERVER
	
	if( bFirstCall )
	{
//sun: 8, // __S8_PK
#ifdef __WORLDSERVER
		if( bMonster && IsChaotic() )
//		if( bMonster && bChaoPlayer )
		{
			m_dwPKExp	= (DWORD)( m_dwPKExp + nExp );
			DWORD dwPropensity = GetPKPropensity(); // undorflow
			int nLevelPKExp = prj.GetLevelExp( GetLevel() ); // Level별 감소 경험치
			if( nLevelPKExp != 0 )
			{
				int nSubExp = m_dwPKExp / nLevelPKExp;
				if( nSubExp )
				{
					SetPKPropensity( GetPKPropensity() - ( m_dwPKExp / nLevelPKExp ) );
					if( dwPropensity <= GetPKPropensity() )
						SetPKPropensity( 0 );
					m_dwPKExp %= nLevelPKExp;
					
					g_xWSUserManager->AddPKPropensity( this );
					g_dpDBClient.SendLogPkPvp( this, NULL, 0, 'P' );
				}
			}
		}
#endif // __WORLDSERVER

	}

	if( m_nExp1 >= prj.m_aExpCharacter[nNextLevel].nExp1 )	// 레벨업
	{
//sun: 10차 전승시스템	Neuz, World, Trans
		if( IsLegendHero() == TRUE && ( nNextLevel > MAX_3RD_LEGEND_LEVEL ) )
		{
			m_nLevel = MAX_3RD_LEGEND_LEVEL;
#if __VER >= 17
			m_nExp1  = 0;
#else // __VER >= 17
			m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
#endif // __VER >= 17
			return FALSE;
		}
		else if( IsHero() && ( nNextLevel > MAX_LEGEND_LEVEL ) )
		{
			m_nLevel = MAX_LEGEND_LEVEL;
			m_nExp1  = 0;
			return FALSE;
		}
		else if( IsLegendHero() == FALSE && IsHero() == FALSE && nNextLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;
			m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
			return FALSE;
		}

		EXPINTEGER nExptmp;
//		BOOL f	= FALSE;

		{
			m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() + prj.m_aExpCharacter[ nNextLevel ].dwLPPoint );

//sun: 10차 전승시스템	Neuz, World, Trans
			if( IsMaster() == TRUE || IsHero() == TRUE || IsLegendHero() == TRUE )
			{
				m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() + 1 );
			}

			nExptmp		= m_nExp1 - prj.m_aExpCharacter[nNextLevel].nExp1;
//sun: 8,
			m_nExp1		= 0;
//!			m_nExp1		= m_nDeathExp		= 0;

			m_nLevel	= nNextLevel;

//			if( m_nLevel > m_nDeathLevel )
//				f	= TRUE;

			BOOL bLevelUp = TRUE;
			if( IsBaseJob() && m_nLevel > MAX_JOB_LEVEL ) 
			{
				m_nLevel = MAX_JOB_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsExpert() && m_nLevel > MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			{
				m_nLevel = MAX_JOB_LEVEL + MAX_EXP_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsPro() && m_nLevel > MAX_GENERAL_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;

				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
				nExptmp  = 0;
				return FALSE;
			}
//sun: 10차 전승시스템	Neuz, World, Trans
			else if( IsMaster() && m_nLevel > MAX_GENERAL_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
				nExptmp  = 0;
				return FALSE;
			}
			else if( IsHero() && (m_nLevel > MAX_LEGEND_LEVEL ) )
			{
				m_nLevel = MAX_LEGEND_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
				nExptmp  = 0;
				return FALSE;
			}
			else if( IsLegendHero() == TRUE && ( m_nLevel > MAX_3RD_LEGEND_LEVEL ) )
			{
				m_nLevel = MAX_3RD_LEGEND_LEVEL;
				bLevelUp = FALSE;
#if __VER >= 17
				m_nExp1  = 0;
#else // __VER >= 17
				m_nExp1  = ( prj.m_aExpCharacter[nNextLevel].nExp1 - 1 );
#endif // __VER >= 17
				nExptmp  = 0;
				return FALSE;
			}

			if( bLevelUp )
			{
				m_nHitPoint = GetMaxHitPoint();
				m_nManaPoint = GetMaxManaPoint();
				m_nFatiguePoint = GetMaxFatiguePoint();
				SetJobLevel( m_nLevel, m_nJob );
//sun: 8,
				if( m_nDeathLevel >= m_nLevel )
				{
					m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - prj.m_aExpCharacter[ nNextLevel ].dwLPPoint );
//sun: 10차 전승시스템	Neuz, World, Trans

					if( IsMaster() == TRUE || IsHero() == TRUE || IsLegendHero() == TRUE )
					{
						m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - 1 );
					}
				}


#ifdef __WORLDSERVER
				if( m_nDeathLevel < m_nLevel )
				{
					int nGetPoint = 0;

					if( IsLegendHero() == TRUE )
					{
						if( GetJob() == JOB_LORDTEMPLER_HERO || GetJob() == JOB_FLORIST_HERO )
						{
							nGetPoint = SKILLPOINT_LEGENDHERO_EXCEPTION;
						}
						else
						{
							nGetPoint = SKILLPOINT_LEGENDHERO_BASE;
						}
					}
					else
					{
						nGetPoint = ( ( GetLevel() - 1 ) / SKILLPOINT_DIVIDE ) + SKILLPOINT_BASE;
					}

//sun: 10차 전승시스템	Neuz, World, Trans
					if( IsMaster() || IsHero() )
						SetMasterSkillPointUp();
					else
					{
						AddSkillPoint( nGetPoint );
						g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_GET_HUNT, nGetPoint, this, NULL );
					}
//sun: 10, __S_RECOMMEND_EVE
				    if( g_eLocal.GetState( EVE_RECOMMEND ) && IsPlayer() )
					{
						g_dpDBClient.SendRecommend( (FLWSUser*)this );
					}					

					((FLWSUser*)this)->m_nExpLog = 0;
#ifdef __EVENTLUA_GIFT
					prj.m_EventLua.SetLevelUpGift( (FLWSUser*)this, m_nLevel );
#endif // __EVENTLUA_GIFT
					CCampusHelper::GetInstance()->SetLevelUpReward( (FLWSUser*)this );
				}

				g_pMadrigalGift->OnLevel( static_cast<FLWSUser*>( this ) );
#endif // __WORLDSERVER
			}
			else
			{
				m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - prj.m_aExpCharacter[ nNextLevel ].dwLPPoint );

//sun: 10차 전승시스템	Neuz, World, Trans
				if( IsMaster() == TRUE || IsHero() == TRUE || IsLegendHero() == TRUE )
				{
					m_Stat.SetRemainStatPoint( m_Stat.GetRemainStatPoint() - 1 );
				}

				nExptmp	= m_nExp1	= 0;
			}

			// 레벨 20이 되는순간 비행레벨은 1이된다.
			if( m_nLevel == 20 )
				SetFlightLv( 1 );
			
#ifdef __CLIENT
			if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 )) )
				SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
			CreateSfx(g_Neuz.m_pd3dDevice, XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
			PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
		}

		if( nExptmp > 0 )
			AddExperience( nExptmp, FALSE, bMultiPly );
		return TRUE;
	}

	return FALSE;
}

// 경험치를 nExp만큼 깎는다.  렙다운도 된다.
// bExp2Clear : pxp를 0으로 할건지 말건지.
// bLvDown : 레벨다운을 할건지 말건지.
// BOOL CMover::DecExperience( EXPINTEGER nExp, BOOL /*bExp2Clear*/, BOOL bLvDown )
// {
// 	if( nExp < 0 )	// 값은 항상 양수로만 줘야 한다.
// 		return FALSE;
// 	
// 	int nPrevLevel	= m_nLevel - 1;
// 	if( nPrevLevel < 1 )	// 레벨 1이하로는 더이상 내려가지 않음.
// 		return FALSE;
// 
// 	m_nExp1		-= nExp;	// nExp만큼 경험치를 깎음. Exp2는 0%로.
// 
// 	if( m_nExp1 < 0 )
// 	{
// 		if( bLvDown )	// 렙다운이 되는 상황.
// 		{
// //sun: 8,  
// 			--m_nLevel;
// 			m_nExp1 = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 + m_nExp1 );
// 
// 			return TRUE;	// 렙다운 됐으면 TRUE리턴
// 		}
// 		else
// 		{
// 			m_nExp1		= 0;
// #ifdef __WORLDSERVER
// 			g_dpDBClient.SendLogLevelUp( this, 7 );
// #endif // __WORLDSERVER
// 		}
// 	}
// 	return FALSE;
// }

// 현재 레벨의 최대경험치의 %로 경험치 하락
// ex) fPercent = 0.2f;		// 20%
// BOOL CMover::DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown )
// {
// 	EXPINTEGER	nExpOld		= m_nExp1;		// 깎이기전 exp
// 	EXPINTEGER	nMaxExp		= prj.m_aExpCharacter[m_nLevel+1].nExp1;	// 깎이기전 경험치치 맥스량.
// 	int	nOldLv	= m_nLevel;		// 깎이기전 레벨.
// 
// 	if( fPercent == 0.0f )	
// 		return FALSE;	// 0%를 깎는 상황이면 계산할 필요 없다.
// 
// 	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 * fPercent );	// 현재레벨의 최대경험치 * 퍼센트
// 	bLvDown		= DecExperience( nDecExp, bExp2Clear, bLvDown );
// 
// 	if( bLvDown )	// 실제로 레벨이 다운되었을때.
// 	{
// 		if( m_nLevel - nOldLv < -1 )	// 레벨이 두단계 이상 다운되는 일은 없어야 한다.
// 			FLERROR_LOG( PROGRAM_NAME, _T( "%s 레벨다운 이상. %d -> %d  %I64d(%f)" )
// 			, m_szName, nOldLv, m_nLevel, nDecExp, fPercent );
// 
// 		EXPINTEGER	nDecExp2	= ( prj.m_aExpCharacter[m_nLevel+1].nExp1 - m_nExp1 ) + nExpOld;	// 실제 깎인 포인트.
// 		if( nDecExp2 > nDecExp )	// 실제 깎인양이 원래 깎으려고 했던 양보다 많으면 경고.
// 			FLERROR_LOG( PROGRAM_NAME, _T( "%s(Lv%d) 경험치가 이상하게 깎임. %I64d %I64d(%f) %I64d %I64d" )
// 			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, m_nExp1 );
// 	}
// 	else
// 	{
// 		EXPFLOAT fOldPercent = static_cast<EXPFLOAT>( nExpOld ) / static_cast<EXPFLOAT>( nMaxExp );		// 깎이기전 퍼센트량.
// 		EXPFLOAT fCurPercent = static_cast<EXPFLOAT>( m_nExp1 ) / static_cast<EXPFLOAT>( nMaxExp );		// 깍은후 퍼센트량.
// 
// 		if( fOldPercent - fCurPercent > 0.060f )
// 			FLERROR_LOG( PROGRAM_NAME, _T( "%s(Lv%d) 경험치가 이상하게 깎임. %I64d %I64d(%f) %I64d(%f) %I64d(%f)" )
// 			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, fOldPercent, m_nExp1, fCurPercent );
// 	}
// 
// 	return bLvDown;
// }

// 비행 경험치 획득
BOOL CMover::AddFxp( int nFxp )
{
	if( nFxp <= 0 )		// nExp가 마이너스이거나 0이면 처리할필요 없음.
		return FALSE;

	if( m_nHitPoint <= 0 )		// 죽고나선 경험치 획득 못함.
		return FALSE;

	if( GetFlightLv() == 0 )
		return FALSE;

	int nNextLevel = GetFlightLv() + 1;

	m_nFxp += nFxp;		// 일단 경험치를 더하고.
	if( (DWORD)( m_nFxp ) >= prj.m_aFxpCharacter[nNextLevel].dwFxp ) // 레벨업이 되었는지 확인.
	{
		int		FxpBuf;
		FxpBuf = m_nFxp - prj.m_aFxpCharacter[ nNextLevel ].dwFxp;		// 레벨업하고도 얼마나 더 오버됐는지?
		m_nFxp = 0;
		SetFlightLv( nNextLevel );	// 레벨 증가.
		
#ifdef __CLIENT
		// 레벨업시 이펙트
		CreateSfx(g_Neuz.m_pd3dDevice,XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
	
		// 오바된 경험치는 한번더 돌려서 더해줌.
		if( FxpBuf > 0)
		{
			AddFxp( FxpBuf );
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CMover::AddChangeJob( int nJob )
{
	BOOL	bResult = FALSE;
	LPSKILL lpSkill;

	if( nJob == JOB_VAGRANT )
	{
		m_nJob = nJob;

		SkillProp** apSkillProp = prj.m_aJobSkill[ nJob ];
		int nJobNum = prj.m_aJobSkillNum[ nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
	}

	if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT ) // 1차 전직 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL ) // 2차 전직 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
//sun: 10차 전승시스템	Neuz, World, Trans
	if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER ) // 전승 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
			lpSkill->dwLevel = 1;	//master스킬은 1부터 준다.
		}
		bResult = TRUE;
	}
	
	if( MAX_MASTER <= nJob && nJob < MAX_HERO ) // 영웅 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	else if( MAX_HERO <= nJob && nJob < MAX_LEGEND_HERO )
	{
		m_nJob = nJob;

		SkillProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			SkillProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}


#ifdef __WORLDSERVER
//sun: 13, 달인
	((FLWSUser*)this)->CheckHonorStat();
	((FLWSUser*)this)->AddHonorListAck();
	g_xWSUserManager->AddHonorTitleChange( this, m_nHonor);

	if( bResult == TRUE )
	{
		g_pMadrigalGift->OnLevel( static_cast<FLWSUser*>( this ) );
	}

#endif // __WORLDSERVER
	return bResult;
}

BOOL CMover::SetFxp( int nFxp, int nFlightLv )
{
	m_nFxp = nFxp;

	// 레벨이 올랐으면.
	if( nFlightLv > GetFlightLv() )
	{
		SetFlightLv( nFlightLv );
#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice,XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
#endif // CLIENT
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::SetExperience( EXPINTEGER nExp1, int nLevel )
{
	m_nExp1		= nExp1;

	if( IsInvalidObj(this) )
		return 0;

	if( nLevel > m_nLevel )
	{
#ifdef __CLIENT
 		// 15렙 되면 더이상 초보자가 아니므로 자동으로 초보자도움말 끄자.
		if( nLevel == 15 )		// 1차전직레벨 15에 대한 define 있으면 그걸로 바꿔주. -xuzhu-
			g_Option.m_nInstantHelp = 0;
 		
		PutLvUpSkillName_1(nLevel);

		if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 ) ) )
			SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
		CreateSfx(g_Neuz.m_pd3dDevice,XI_INDEX( 42, XI_GEN_LEVEL_UP01 ),GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		if( m_nDeathLevel < nLevel )
			g_WndMng.PutString( prj.GetText( TID_GAME_LEVELUP ), NULL, prj.GetTextColor( TID_GAME_LEVELUP ) );
		
		if( g_WndMng.m_pWndWorld )
		{
			if( g_xFlyffConfig->GetMainLanguage() != LANG_JAP )
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), g_WndMng.m_pWndWorld->m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), NULL );// CWndBase::m_Theme.m_pFontCaption );
		}
		
		// 2, 8, 12
		static	int nPatLv[4]	= { 2, 8, 12, 0	};
		for( int i = 0; i < 3; i++ ) {
			if( m_nLevel < nPatLv[i] && nLevel >= nPatLv[i] ) {
				g_WndMng.PutString( prj.GetText( TID_GAME_PATTERNUP ), NULL, prj.GetTextColor( TID_GAME_PATTERNUP ) );
				break;
			}
			else if( m_nLevel >= nPatLv[2] )
				break;
		}

		if( TRUE == IsJobType( JTYPE_BASE ) )
		{
			if( nLevel == 15 )
				g_WndMng.PutString( prj.GetText( TID_EVE_LEVEL15 ), NULL, prj.GetTextColor( TID_EVE_LEVEL15 ) );

			static	int nNum	= prj.m_aJobSkillNum[JOB_VAGRANT];
		}
		if( m_nLevel < 20 && nLevel >= 20 )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
			SetFlightLv( 1 );
		}
		else if( nLevel < 20 )
			SetFlightLv( 0 );
#endif	// __CLIENT
		m_nLevel	= nLevel;

		SetJobLevel( m_nLevel, m_nJob );

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = GetMaxManaPoint();
		m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
//sun: 13, 캐릭터정보창 리뉴얼
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER3);

				switch(GetLevel())
				{
			//sun: 12, 튜토리얼 개선
				case 15:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_CHANGEJOB);
					break;						
				case 20:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif //__CLIENT		

		return TRUE;
	}
	else if( nLevel < m_nLevel )	// 레벨 다운이 되었을때.
	{
		m_nLevel	= nLevel;
		if( nLevel < 20 )
			SetFlightLv( 0 );

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
				switch(GetLevel())
				{
				case 2:
//sun: 13, 캐릭터정보창 리뉴얼
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER3);
					break;
				case 3:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_BERSERKERMODE);
					break;
				case 4:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_SKILL_BEFOREJOB);
					break;
				case 8:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_ESSENSE);
					break;
				case 15:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_CHANGEJOB);
					break;					
				case 16:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT);
					break;
				case 20:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif	// __CLIENT
	}


	return FALSE;
}


void CMover::SetEquipDstParam()
{
	if( IsPlayer() ) 
	{
		FLItemElem* pItemElem;
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			if( pItemElem )
				SetDestParam( pItemElem->m_dwItemId, FALSE );	//don't send
		}
	}
}


/////////////////////////////////////////////////////////////////////////////// aaaaaaaaa
int CMover::SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax )
{
	if( IsPlayer() )
	{
		int	nMin = 0, nMax = 0;
		FLItemElem* pItemElem;
		PT_ITEM_SPEC pItemProp;

		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			//if(pItem && !pItem->IsBreakableItem() && (pItem->m_nHit_Point == 1 || pItem->m_nHit_Point == 0))
			//	continue; IK2_ARMOR
			// IK_ARMOR를 이제 체크하지 않으므로 머리나 팔찌등에 디펜스값이 들어가지 않도록 주의한다.
			// 무기의 어빌리티는 디펜스에 포함되지 않는다
			//if( pItemElem && pItemElem->GetProp()->dwItemKind2 < IK2_WEAPON_DIRECT && pItemElem->GetProp()->dwItemKind2 > IK2_WEAPON_GUN )  //&& ( pItemElem->m_nHit_Point > 1 || pItemElem->m_nHit_Point == -1) )
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();

				if( pItemProp == NULL )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Parts Not Property = %d" ), i );
				}

				if( pItemProp )
				{
					if( pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC )
					{
						int nOption = pItemElem->GetAbilityOption();
						int nOptionVal = 0;
						if( nOption > 0 ) 
							nOptionVal = (int)( pow( (float)( nOption ), 1.5f ) );
							
						float f		= GetItemMultiplier( pItemElem );

						nMin	+= (int)( pItemProp->dwAbilityMin * f ) + nOptionVal;
						nMax	+= (int)( pItemProp->dwAbilityMax * f ) + nOptionVal;
					}
#ifdef ADD_CHARACTER_INFO_DISPLAY
					else if( pItemProp->IsCostumeParts() )
					{
						int nOption = pItemElem->GetAbilityOption();
						float f		= GetItemMultiplier( pItemElem );

						nMin	+= (int)( pItemProp->dwAbilityMin * f );
						nMax	+= (int)( pItemProp->dwAbilityMax * f );

					}
#endif
				}
			}
		}
		// 모든 합산이 끝났다면 각 요소에 수정치를 적용한다.
		nMin = GetParam( DST_ABILITY_MIN, nMin );
		nMax = GetParam( DST_ABILITY_MAX, nMax );
		if( pnMin ) *pnMin = nMin;
		if( pnMax ) *pnMax = nMax;
		return ( ( nMin + nMax ) / 2 ); // 리턴 값은 평균 값이다. 판정시에는 사용하지 않는다. 출력이나 기타 용도로 사용한다.
	}
	else
	{
	}
	
	return 0;
}

void CMover::SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray )
{
	*pnAdjHitRate = 0;
	*pnAdjParray = 0;

	FLItemElem* pItemElem;
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		pItemElem = GetEquipItem( i );
		if( pItemElem )  
		{
			if( pItemElem->GetProp() )
			{
				if( (int)pItemElem->GetProp()->nAdjHitRate > 0 )
					*pnAdjHitRate += pItemElem->GetProp()->nAdjHitRate;
				if( (int)pItemElem->GetProp()->dwParry > 0 )
					*pnAdjParray += pItemElem->GetProp()->dwParry;
			}
		}
	}
}

// 안보이는 상태
BOOL CMover::SetDarkCover( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_INVISIBILITY )	// 투명화에 면역이라....그런것도 있나?
			return FALSE;	
		SetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, NULL_CHGPARAM );
		m_wDarkCover =	(short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_INVISIBILITY )
		{
			ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );		// 암흑상태해제
		}
		m_wDarkCover = 0;
	}

	return TRUE;
}

// 스턴상태로 만들기
BOOL CMover::SetStun( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_STUN ) 
			return FALSE;		// 독에 면역상태다.

		SetDestParam( DST_CHRSTATE, CHS_STUN, NULL_CHGPARAM, FALSE );
		SendActMsg( OBJMSG_STUN );		// 스턴 상태 시작.
		m_wStunCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );		// 얼마동안이나 스턴상태가 되느냐.
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )	// 이거 검사안하면 매번 클라로 ResetDestParam을 날린다.
			ResetDestParam( DST_CHRSTATE, CHS_STUN, FALSE);	// 스턴 상태 해제.
		m_wStunCnt = 0;
		m_dwFlag &= (~MVRF_NOACTION);				// 액션 금지 상태로 전환.
	}

#ifdef __WORLDSERVER
	g_xWSUserManager->AddSetStun( this, tmMaxTime );
#endif // Worldserver

	return TRUE;
}

// this를 독에 걸리게 한다.
// idAttacker : 공격자(컨트롤일수도 있슴)
// tmMaxTime : 1/1000초 단위.
// tmUnit : 몇초 마다 독데미지가 깎일꺼냐.
// nDamage : 데미지량
BOOL CMover::SetPoison( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_POISON ) 
			return FALSE;		// 독에 면역상태다.
		SetDestParam( DST_CHRSTATE, CHS_POISON, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 독효과의 지속시간이 지정되지 않음" ), GetName() );
		if( tmUnit == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 독효과의 tick 시간이 지정되지 않음" ), GetName() );
		if( wDamage == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 독효과의 틱당 데미지가 지정되지 않음" ), GetName() );
		
		m_wPoisonCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wPoisonDamage = wDamage;
		m_tmPoisonUnit = tmUnit;
		m_idPoisonAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_POISON )
			ResetDestParam( DST_CHRSTATE, CHS_POISON, TRUE );	// 독상태 해제
		m_wPoisonCnt = 0;
		m_wPoisonDamage = 0;
		m_tmPoisonUnit = 0;
	}

	return TRUE;
}

// this를 암흑상태에 빠지게 한다
// tmMaxTime : 암흑 지속시간(1/1000)
// nAdjHitRate : 지속시간동안 수정될 명중률
// bApply : 적용/해제
BOOL CMover::SetDark( BOOL bApply, DWORD tmMaxTime, int nAdjHitRate )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_DARK ) 
			return FALSE;		// 암흑에 면역상태다.
		SetDestParam( DST_CHRSTATE, CHS_DARK, NULL_CHGPARAM );
		SetDestParam( DST_ADJ_HITRATE, nAdjHitRate, NULL_CHGPARAM );
		m_wDarkCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
		if( nAdjHitRate == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 어둠효과의 명중률 수정치가 지정되지 않음" ), GetName() );
		m_wDarkVal = static_cast< short >( nAdjHitRate );
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_DARK )
		{
			ResetDestParam( DST_CHRSTATE, CHS_DARK, TRUE );		// 암흑상태해제
			ResetDestParam( DST_ADJ_HITRATE, m_wDarkVal, TRUE );	// 암흑동안 수정됐던 명중률 해제
//			if( m_wDarkCnt >= 0 )
//				FLERROR_LOG( PROGRAM_NAME, _T( "SetDark : %d" ), m_wDarkVal );
		}
		m_wDarkCnt = 0;
		m_wDarkVal = 0;
	}
	return TRUE;
}

// this를 출혈을 일으킨다. 독과 비슷.
// tmMaxTime : 1/1000초 단위.
// tmUnit : 몇초 마다 데미지가 깎일꺼냐.
// nDamage : 데미지량
BOOL	CMover::SetBleeding( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_BLEEDING ) return FALSE;		// 출혈에 면역상태다.
		
		SetDestParam( DST_CHRSTATE, CHS_BLEEDING, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 출혈효과의 지속시간이 지정되지 않음" ), GetName() );
		if( tmUnit == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 출혈효과의 tick 시간이 지정되지 않음" ), GetName() );
		if( wDamage == -1 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 출혈효과의 틱당 데미지가 지정되지 않음" ), GetName() );
		
		m_wBleedingCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wBleedingDamage = wDamage;
		m_tmBleedingUnit = tmUnit;
		m_idBleedingAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_BLEEDING )
			ResetDestParam( DST_CHRSTATE, CHS_BLEEDING, TRUE );	// 독상태 해제
		m_wBleedingCnt = 0;
		m_wBleedingDamage = 0;
		m_tmBleedingUnit = 0;
	}
	return TRUE;
}

// dwState를 해제함
// CHS_ALL을 하면 모든 디버프.
void CMover::RemoveDebuff( DWORD dwState )
{
	// 스킬로 걸린건 이걸로 해제해야하고
	RemoveChrStateBuffs( dwState );
	// 스킬이 아닌 아이템같은걸로 상태만 걸린건 이걸로 풀어야 한다.
	if( dwState & CHS_DARK )
		SetDark( FALSE );
	if( dwState & CHS_POISON )
		SetPoison( FALSE );
	if( dwState & CHS_STUN )
		SetStun( FALSE );
	if( dwState & CHS_SLEEPING )
	{
		SetSleeping( false );
	}
//sun: 10, 
	if( dwState & CHS_BLEEDING )
		SetBleeding( FALSE );
	
	// TODO : 사용하지 않음
// 	if( dwState & CHS_DEBUFFALL )
// 	{
// 		RemoveDstParamBuffs( DST_SPEED );
// 		RemoveDstParamBuffs( DST_CHR_CHANCEPOISON );
// 		RemoveDstParamBuffs( DST_CHR_CHANCESTUN );
// 		RemoveDstParamBuffs( DST_CHR_CHANCEBLEEDING );
// 	}

	// 이거 안에서 Reset을 호출한다.
	SetDestParam( DST_CURECHR, dwState, NULL_CHGPARAM );
}

// this에게 걸린 버프를 1개 없앤다.
void CMover::RemoveBuffOne( DWORD /*dwSkill*/ )
{
	m_buffs.RemoveBuffs( RBF_ONCE | RBF_GOODSKILL, 0 );
}	

void CMover::RemoveBuffAll( void )
{
	m_buffs.RemoveBuffs( RBF_GOODSKILL, 0 );
}




///////////////////////////////////////////////////////////////////////////////
// 
// 독, 암흑등 추가능력치의 세팅은 이걸로 하자.
void CMover::SetDestParam( int nIdx, SkillProp *pProp, BOOL bSend )
{
	// TODO : range check nIdx
	SetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], pProp->dwChgParamVal[nIdx], bSend );
}

void CMover::ResetDestParam( int nIdx, SkillProp *pProp, BOOL bSend )
{
	// TODO : range check nIdx
	ResetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], bSend );
}

// 목표 파라미터를 셋한다.
void CMover::SetDestParam( int nItemIdx, BOOL bSend )
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nItemIdx );

	if( pItemProp == NULL )
	{
		return;
	}

	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
		{
			SetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ], pItemProp->dwChgParamVal[ Nth ], bSend );
		}
	}

// 	if( pItemProp->dwDestParam1 != -1 )
// 		SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, bSend );
// 	if( pItemProp->dwDestParam2 != -1 )
// 		SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, bSend );
// //sun: 10, __PROP_0827
// 	if( pItemProp->dwDestParam3 != -1 )
// 		SetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3, pItemProp->dwChgParamVal3, bSend );
}

//--------------------------------------------------------------------------
//
// 목표 파라메타를 셋한다.
void CMover::SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL bSend )
{
	BOOL fSuccess	= FALSE;

//sun: 10차 전승시스템	Neuz, World, Trans
	if( (nDstParameter < 0 ) && ( nDstParameter != DST_ADJDEF_RATE && nDstParameter != DST_ADJDEF ) )
		return;
	
	// HP, MP등은 SetPointParam으로 처리한다. 근디 SetDestParam 하나로 통일시켰으면 낫지 않을까?
	switch( nDstParameter )
	{
#ifdef __WORLDSERVER
	case DST_HP:
		{
			if( nAdjParameterValue == -1 )
			{
				SetPointParam( nDstParameter, GetMaxHitPoint() );
			}
			else
			{
				SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );
			}

			if( GetAdjParam( DST_IGNORE_DMG_PVP ) > 0 )
			{
				ResetDestParam( DST_IGNORE_DMG_PVP, 0, TRUE );
				RemoveDstParamBuffs( DST_IGNORE_DMG_PVP );
			}
		}
		return;
	case DST_MP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxManaPoint() );
			else
				SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );
			
		}
		return;
	case DST_FP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxFatiguePoint() );
			else
				SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );
		}
		return;
#else // __WORLDSERVER
	case DST_HP:	SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );	return;
	case DST_MP:	SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );	return;
	case DST_FP:	SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );	return;
#endif // __WORLDSERVER
	case DST_GOLD:	
#ifdef __PERIN_BUY_BUG	// chipi_091219 추가
		FLERROR_LOG( PROGRAM_NAME, _T( "SetDestParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]" ), static_cast<FLWSUser*>(this)->GetName(), static_cast<FLWSUser*>(this)->m_idPlayer, nAdjParameterValue );
		return;
#else
		if( AddGold( nAdjParameterValue ) == FALSE )	
			SetGold( 0 );		// underflow이면 0로 만든다. 이전코드와 호환되게 한다. 뭔가 찜찜 
		return;
#endif // __PERIN_BUY_BUG

	case DST_RESIST_ALL:
		SetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_FIRE,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WIND,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WATER,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_EARTH,			nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_STAT_ALLUP:
		SetDestParam( DST_STR,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_DEX,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_INT,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_STA,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		SetDestParam( DST_HP_MAX, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_CHR_DMG, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		SetDestParam( DST_ADJDEF, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_LOCOMOTION:
		SetDestParam( DST_SPEED,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), nChgParameterValue, bSend );
		return;

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			SetDestParam( DST_HP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_MP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_FP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			SetDestParam( DST_KILL_HP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	} // switch

	// 이 검사를 왜 위에다 안넣고 여기다 넣었냐면
	// DST_GOLD, PXP, RESIST_ALL같은경우는 MAX_ADJPARAMARY보다 더 큰값을 사용한다. 
	// 그래서 그런것들은 위에서 처리하고 바로 리턴하고
	// 일반적인것들만 이쪽으로 온다.
	if( nDstParameter >= MAX_ADJPARAMARY)
		return;
	
	if( nAdjParameterValue != 0 )	// 0이 들어온것도 잘못된거니까 경고를 내야 하지 않을까 싶다.
	{
		fSuccess	= TRUE;

		switch( nDstParameter )
		{
		case DST_CHRSTATE:		// 상태변경의 경우는
		case DST_IMMUNITY:
			if( nAdjParameterValue != NULL_ID )
			{
				if( nAdjParameterValue == 0xffffffff )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "SetDestParam : Adj == -1, %s" ), GetName() );
					return;
				}
				m_adjParamAry[nDstParameter] |= nAdjParameterValue;		// 비트로 온오프 된다.
			} else
				fSuccess = FALSE;
			break;
		case DST_CURECHR:		// 상태해제
			ResetDestParam( DST_CHRSTATE, nAdjParameterValue, bSend );
			return;
		case DST_REFLECT_DAMAGE:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 데미지의 몇%를 돌려주는가?
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// 몇%의 확률로 발동되는가?
			break;
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCESTUN:
		case DST_AUTOHP:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 찬스 확률.	// %미만으로 회복되게 할꺼냐.
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// 찬스가 걸렸을때 지속시간		// %HP가 회복될꺼냐.
			break;
//sun: 9, 9-10차 펫
		case DST_HEAL:
			m_nHealCnt	= (short)( PROCESS_COUNT * 2.0f );		// 쿨타임 2초
			m_adjParamAry[nDstParameter]	= nAdjParameterValue;	
			break;
		default:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 누적
			break;
		}
	}

	else if( nChgParameterValue != 0x7FFFFFFF ) 
	{
		fSuccess	= TRUE;

		// 尹
		if( nDstParameter == DST_SPEED )
		{
			if( m_chgParamAry[nDstParameter] == 0x7FFFFFFF )
				m_chgParamAry[nDstParameter]	= 0;
			m_chgParamAry[nDstParameter]++;
		}
		else
		{
			m_chgParamAry[nDstParameter] = nChgParameterValue;	// 고정된 값.
		}
	}
#ifdef __WORLDSERVER
	if( TRUE == ( fSuccess & bSend ) ) {
		g_xWSUserManager->AddSetDestParam( this, nDstParameter, nAdjParameterValue, nChgParameterValue );
	}
#endif	// __WORLDSERVER
}

//
// 목표 파라메타를 리셋한다.
//
void CMover::ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend )
{
	switch( nDstParameter )
	{
	case DST_RESIST_ALL:
		ResetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_FIRE,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WIND,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WATER,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_EARTH,		nAdjParameterValue, TRUE );
		return;
	case DST_STAT_ALLUP:
		ResetDestParam( DST_STR,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_DEX,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_INT,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_STA,	nAdjParameterValue, TRUE );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		ResetDestParam( DST_HP_MAX, nAdjParameterValue, TRUE );
		ResetDestParam( DST_CHR_DMG, nAdjParameterValue, TRUE );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		ResetDestParam( DST_ADJDEF, nAdjParameterValue, TRUE );
		ResetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, TRUE );
		return;
	case DST_LOCOMOTION:
		ResetDestParam( DST_SPEED,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), TRUE );
		return;

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			ResetDestParam( DST_HP_RECOVERY, nAdjParameterValue, TRUE );
			// MP
			ResetDestParam( DST_MP_RECOVERY, nAdjParameterValue, TRUE );
			// FP
			ResetDestParam( DST_FP_RECOVERY, nAdjParameterValue, TRUE );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			ResetDestParam( DST_KILL_HP, nAdjParameterValue, TRUE );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, TRUE );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, TRUE );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, TRUE );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, TRUE );
		}
	}


	// 이 검사를 왜 위에다 안넣고 여기다 넣었냐면
	// DST_GOLD, PXP, RESIST_ALL같은경우는 MAX_ADJPARAMARY보다 더 큰값을 사용한다. 
	// 그래서 그런것들은 위에서 처리하고 바로 리턴하고
	// 일반적인것들만 이쪽으로 온다.
	
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "ResetDestParam %s : 잘못된 값 nDstParam : %d" ), m_szName, nDstParameter );
		return;
	}
	if( nAdjParameterValue != 0 ) 
	{
		if( nDstParameter == DST_CHRSTATE )
		{
			m_adjParamAry[nDstParameter] &= (~nAdjParameterValue);		// 해당비트 오프.
		}
		else
		{
			if( m_adjParamAry[nDstParameter] != 0 )
			{
				m_adjParamAry[nDstParameter] += (-nAdjParameterValue);
			}
		}
	}

	// 尹
	if( nAdjParameterValue == 0 )	//
	{
		if( nDstParameter == DST_SPEED && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
		{
			if( --m_chgParamAry[nDstParameter] == 0 )
				m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
		else
		{
			m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
	}
#ifdef __WORLDSERVER
	if( fSend == TRUE ) 
	{
		if( nDstParameter == DST_SPEED )
			g_xWSUserManager->AddResetDestParamSync( this, nDstParameter, nAdjParameterValue, (int)m_adjParamAry[nDstParameter] );
		else
			g_xWSUserManager->AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	}
#endif	// __WORLDSERVER
}

#ifdef __CLIENT
void CMover::ResetDestParamSync( int nDstParameter,int nAdjParameterValue, int nParameterValue, BOOL /*fSend*/ )
{
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "ResetDestParam %s : 잘못된 값 nDstParam : %d" ), m_szName, nDstParameter );
		return;
	}

	switch( nDstParameter )
	{
		case DST_SPEED:
			{
				m_adjParamAry[nDstParameter] = nParameterValue;
				if( nAdjParameterValue == 0 && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
				{
					if( --m_chgParamAry[nDstParameter] == 0 )
						m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
				}
			}
			break;
	}
}
#else
void CMover::ResetDestParamSync( int /*nDstParameter*/,int /*nAdjParameterValue*/, int /*nParameterValue*/, BOOL /*fSend*/ )
{

}

#endif	// __CLIENT

// 수정 파라미터를 얻는다.
int CMover::GetAdjParam( int nDestParameter )
{
	if( nDestParameter < MAX_ADJPARAMARY )
		return m_adjParamAry[ nDestParameter ];
	return 0;
}
// 교체 파라미터를 얻는다.
int CMover::GetChgParam( int nDestParameter )
{
	if( nDestParameter < MAX_ADJPARAMARY )
		return m_chgParamAry[ nDestParameter ];
	return 0x7FFFFFFF;
}

// 파라미터를 얻는다.
int CMover::GetParam( int nDest, int nParam )
{
	int nChgParam = GetChgParam( nDest );
	if( nChgParam != 0x7FFFFFFF ) 
		return nChgParam;

	int nAdjParam = GetAdjParam( nDest );
	if( nAdjParam )	
		return nParam + nAdjParam;
	else
		return nParam;
}

/////////////////////////////////////////////////////////////////////////////
// 여기부터 파라메타 얻기 함수들 
// GetParam을 사용하여 수정된 값을 내준다.
//
// 히트 포인트 얻기 
int CMover::GetHitPoint()
{
//	return GetParam( DST_HP, m_nHitPoint );
	return min( GetParam( DST_HP, m_nHitPoint ), GetMaxHitPoint() );
}
// 마나 포인트 얻기 
int CMover::GetManaPoint()
{
//	return GetParam( DST_MP, m_nManaPoint );
	return min( GetParam( DST_MP, m_nManaPoint ), GetMaxManaPoint() );
}
// 피지칼 포인트 얻기
int CMover::GetFatiguePoint()
{
//	return GetParam( DST_FP, m_nFatiguePoint );
	return min( GetParam( DST_FP, m_nFatiguePoint ), GetMaxFatiguePoint() );
}

// 최대 히트 포인트 얻기 
int CMover::GetMaxHitPoint()
{
	if( IsInvalidObj(this) )	
		return 0;

	float factor = 1.0f;
	__int64 nResult = GetParam( DST_HP_MAX, GetMaxOriginHitPoint( FALSE ) );
	int nFactor = GetParam( DST_HP_MAX_RATE, 0 );

	factor += (float)nFactor/(float)100;
	int nCF = (int)(factor * 10000);
	nResult	= (nResult * nCF) / 10000;

	if( nResult < 1 )
		nResult		= 1;

	return (int)(nResult); 
}

// 최대 마나 포인트 얻기 
int CMover::GetMaxManaPoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_MP_MAX, GetMaxOriginManaPoint( FALSE ) );
	int nFactor = GetParam( DST_MP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

	if( nResult < 1 )
		nResult		= 1;

	return nResult; 
}

// 최대 피로 포인트 얻기
int CMover::GetMaxFatiguePoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_FP_MAX, GetMaxOriginFatiguePoint( FALSE ) );
	int nFactor = GetParam( DST_FP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

	if( nResult < 1 )
		nResult		= 1;

	return nResult; 
}

int CMover::GetMaxPoint(int nDest)
{
	switch( nDest )
	{
		case DST_HP:
			return GetMaxHitPoint();
		case DST_MP:
			return GetMaxManaPoint();
		case DST_FP:
			return GetMaxFatiguePoint();
	}
	
	return 0;
}

int CMover::GetMaxOriginHitPoint( BOOL bOriginal )
{
	if( IsInvalidObj(this) )	
		return 0;

	if( IsPlayer() )
	{
		int nSta;
		if( bOriginal )
			nSta = m_Stat.GetSta();
		else
			nSta = GetSta();

		JobProp* pProperty = prj.GetJobProp( GetJob() ); 

		float a = (pProperty->fFactorMaxHP*m_nLevel)/2.0f;
		float b = a * ((m_nLevel+1.0f)/4.0f) * (1.0f + nSta/50.0f) + (nSta*10.0f) ;
		float maxHP = b + 80.f; 
		return (int)maxHP;
	}

	MoverProp *pMoverProp = GetProp();
#ifdef __S1108_BACK_END_SYSTEM
	//gmpbigsun( 20110106 ) : 
	float fRstRate = prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate;
	int nRate = 0;
	if( fRstRate > 0.0f )
	{
		nRate = (int)(fRstRate * 10000);
	}

	__int64 n64Val = ((__int64)pMoverProp->dwAddHp * nRate) / 10000;
	return (int)n64Val;

	//		return int( pMoverProp->dwAddHp * prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate ); 
#else // __S1108_BACK_END_SYSTEM
	return pMoverProp->dwAddHp; 
#endif // __S1108_BACK_END_SYSTEM
}

int CMover::GetMaxOriginManaPoint( BOOL bOriginal )
{
	int nInt = 0;
	if( bOriginal )
		nInt = m_Stat.GetInt();
	else
		nInt = GetInt();

	if( IsPlayer() )
	{
		// INT((((BaseLv*2) + (INT*8))*Job계수) + 22)+(INT*Job계수)) +  ① 
		// ① : 아이템에의한 추가 상승 (%단위) TDDO
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxMP;

// 		int nMaxMP = (int)( ((((m_nLevel*2.0f) + ( nInt*8.0f))*factor) + 22.0f)+( nInt*factor) );

		float fMaxMP = ( ( ( ( (float)m_nLevel * 2.0f ) + ( (float)nInt * 8.0f ) ) * factor ) + ( (float)nInt * factor ) ) + 22.0f;

		return (int)fMaxMP;
	}
	return 	( ( m_nLevel * 2 ) + ( nInt * 8 ) + 22 );
}

int CMover::GetMaxOriginFatiguePoint( BOOL bOriginal )
{
	int nSta, nStr, nDex;
	if( bOriginal )
	{
		nSta = m_Stat.GetSta();
		nStr = m_Stat.GetStr();
		nDex = m_Stat.GetDex();
	}
	else
	{
		nSta = GetSta();
		nStr = GetStr();
		nDex = GetDex();
	}

	if( IsPlayer() )
	{
		// (((BaseLv*2) + (STA*6))*Job계수) +(STA*Job계수) + ① + ②
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxFP;

		int nMaxFP = (int)( (((m_nLevel*2.0f) + (nSta*6.0f))*factor) +(nSta*factor) );
		return nMaxFP;
	}
	
	return ( ( m_nLevel * 2 ) + ( nStr * 7 ) + ( nSta *2 ) + ( nDex * 4 ) );
}

int CMover::GetHitPointPercent( int nPercent )
{
	int nMax = GetMaxHitPoint();

	if( nMax == 0 ) 
		return 0;

	int nCurrentHitPoint( GetHitPoint() );

	if( nCurrentHitPoint > nMax )
		nCurrentHitPoint = nMax;

	return MulDiv( nCurrentHitPoint, nPercent, nMax );
}

int CMover::GetManaPointPercent( int nPercent )
{
	int nMax = GetMaxManaPoint();

	if( nMax == 0 )
		return 0;

	int nCurrentManaPoint( GetManaPoint() );

	if( nCurrentManaPoint > nMax )
		nCurrentManaPoint = nMax;
	
	return MulDiv( nCurrentManaPoint, nPercent, nMax );
}

int CMover::GetFatiguePointPercent( int nPercent )
{
	int nMax = GetMaxFatiguePoint();

	if( nMax == 0 ) 
		return 0;

	int nCurrentFatiguePoint( GetFatiguePoint() );

	if( nCurrentFatiguePoint > nMax )
		nCurrentFatiguePoint = nMax;


	return MulDiv( nCurrentFatiguePoint,  nPercent, nMax );
}

// HP 회복 값 얻기
/*
	HP Recovery : 회복량	PC/NPC : int(((현재레벨 + STA)/3.4)+2) + AddHpRecovery
	MP Recovery : 회복량	PC/NPC : int(((현재레벨 + INT)/4)+2) + Addecovery
	FP Recovery : 회복량	PC/NPC : int(((현재레벨 + STA + DEX)/4.5)+2) + AddHpRecovery

	HP Recovery : 회복속도	PC/NPC : 휴식(정지)시 5초에 한번씩 회복하며 휴식하지 않는 상황에서는 12초에 한번씩 회복함 - AddHpRecoveryTime
	MP Recovery : 회복속도	PC/NPC : 휴식(정지)시 5초에 한번씩 회복하며 휴식하지 않는 상황에서는 12초에 한번씩 회복함 - AddMpRecoveryTime
	FP Recovery : 회복속도	PC/NPC : 휴식(정지)시 5초에 한번씩 회복하며 휴식하지 않는 상황에서는 12초에 한번씩 회복함 - AddFpRecoveryTime

  */
int CMover::GetHPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorHPRecovery;
	}

	//(레벨/3)+ (MaxHP/ (500* 레벨))+(STA*Job계수)
	int nValue = (int)( (GetLevel() / 3.0f) + (GetMaxHitPoint() / (500.f * GetLevel())) + (GetSta() * fFactor) );

	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // 회복량 10% 하향		// 9차 추가 (타격, 피격 10초 후 자동 회복)

	return GetParam( DST_HP_RECOVERY, nValue );
}
// MP 회복 값 얻기
int CMover::GetMPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorMPRecovery;
	}

	// ((레벨*1.5)+(MaxMP/(500*레벨))+(INT*Job계수))*0.2
	int nValue = (int)( ((GetLevel() * 1.5f) + (GetMaxManaPoint() / (500.f * GetLevel())) + (GetInt() * fFactor)) * 0.2f );

	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // 회복량 10% 하향		// 9차 추가 (타격, 피격 10초 후 자동 회복)

	return GetParam( DST_MP_RECOVERY, nValue );
}
// FP 회복 값 얻기 
int CMover::GetFPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorFPRecovery;
	}
	// ((레벨*2)+(MaxFP/(500*레벨))+(STA*Job계수))*0.2
	int nValue = (int)( ((GetLevel() * 2.0f) + (GetMaxFatiguePoint() / (500.f * GetLevel())) + (GetSta() * fFactor)) * 0.2f );

	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // 회복량 10% 하향	// 9차 추가 (타격, 피격 10초 후 자동 회복)

	return GetParam( DST_FP_RECOVERY, nValue );
}

#ifdef __CLIENT
// 명성 이름을 스트링으로 돌려준다.
LPCTSTR CMover::GetFameName( void )
{
	switch( GetJob() )
	{
	// 아크로뱃
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_ACR_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_ACR_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_ACR_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_ACR_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_ACR_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ACR_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ACR_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ACR_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ACR_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ACR_FAME01 );
		break;

	// 머서너리일때
	case JOB_MERCENARY:	
	case JOB_KNIGHT:	
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:	
	case JOB_BLADE_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_MER_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_MER_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_MER_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_MER_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_MER_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MER_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MER_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MER_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MER_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MER_FAME01 );
		break;
	// 매지션
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:	
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:	
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:	
	case JOB_ELEMENTOR_HERO:

		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_MAG_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_MAG_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_MAG_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_MAG_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_MAG_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MAG_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MAG_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MAG_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MAG_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MAG_FAME01 );
		break;

	// 어시스트
	case JOB_ASSIST:
	case JOB_BILLPOSTER:	
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER_MASTER:	
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_HERO:	
	case JOB_RINGMASTER_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_ASS_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_ASS_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_ASS_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_ASS_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_ASS_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ASS_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ASS_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ASS_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ASS_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ASS_FAME01 );		

		break;
	}
	return "";
}

#endif


// 마법 저향력 얻기 
int CMover::GetResistMagic()
{
	return GetParam( DST_RESIST_MAGIC, 0 );
}
int CMover::GetResistSpell( int nDestParam )
{
	MoverProp *pProp = GetProp();
	if( pProp == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "%d 프로퍼티 읽기 실패" ), GetName() );

	int		nResist = 0;

	switch( nDestParam )
	{
	case DST_RESIST_ELECTRICITY:	nResist = pProp->nResistElecricity;		break;
	case DST_RESIST_FIRE:			nResist = pProp->nResistFire;		break;
	case DST_RESIST_WATER:			nResist = pProp->nResistWater;		break;
	case DST_RESIST_EARTH:			nResist = pProp->nResistEarth;		break;
	case DST_RESIST_WIND:			nResist = pProp->nResistWind;		break;
	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "%s 파라메터 잘못됨 %d" ), GetName(), nDestParam );
		break;
	}
	return GetParam( nDestParam, nResist );
}

int CMover::GetNaturalArmor()
{
	return GetProp()->dwNaturalArmor;//GetParam(DST_NATURAL_ARMOR,GetProp()->m_dwNaturalArmor);
}

int CMover::GetStr()
{
	int nResult = m_Stat.GetStr() + GetParam( DST_STR, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}
int CMover::GetDex()
{
	int nResult = m_Stat.GetDex() + GetParam( DST_DEX, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}
int CMover::GetInt()
{
	int nResult = m_Stat.GetInt() + GetParam( DST_INT, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}
int CMover::GetSta()
{
	int nResult = m_Stat.GetSta() + GetParam( DST_STA, 0 );

	if( nResult < 1 )
		nResult	= 1;

	return nResult;
}

bool CMover::HasZeroSpeedStateParam_hardcoding()
{
	const int nStateParam	= GetAdjParam( DST_CHRSTATE );
	return ( nStateParam & ( CHS_STUN | CHS_LOOT | CHS_SETSTONE | CHS_SLEEPING ) ) != 0;
}

FLOAT CMover::GetSpeed(FLOAT fSrcSpeed)
{
//sun: 9, 9-10차 펫
#ifdef __CLIENT
	if( m_dwAIInterface == AII_EGG && m_pAIInterface )
	{
		CAIEgg* pAI	= (CAIEgg*)m_pAIInterface;
		CMover* pOwner	= prj.GetMover( pAI->GetOwnerId() );
		if( IsValidObj( pOwner ) )
			return pOwner->GetSpeed( pOwner->m_pActMover->m_fSpeed );
	}
#endif	// __CLIENT

	if( HasZeroSpeedStateParam_hardcoding() == true )
		return 0;

	if( IsPlayer() == FALSE )
	{
		fSrcSpeed	*= GetSpeedFactor();		//sun: 9,	//__AI_0509
	}

	int nChgValue	= GetChgParam( DST_SPEED );
//	if( nChgValue != 0x7FFFFFFF )
//		return fSrcSpeed * (nChgValue / 100.0f);		// ex) fSrcSpeed * 0.5f

	// 尹
	if( nChgValue != 0x7FFFFFFF )
		return 0;

	int nAdjValue	= GetAdjParam( DST_SPEED );
	if( nAdjValue != 0 )
	{
		fSrcSpeed = fSrcSpeed + ( fSrcSpeed * (nAdjValue / 100.0f) );
		if( fSrcSpeed < 0.0F )
			fSrcSpeed	= 0.0F;

		return fSrcSpeed;
	}
	if( fSrcSpeed < 0.0F )
		fSrcSpeed	= 0.0F;

	return fSrcSpeed;
}

BOOL CMover::IsCompleteQuest( int nQuestId )
{
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::MakeCompleteQuest( int nQuestId, LPQUEST lpQuest )
{
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			ZeroMemory( lpQuest, sizeof( QUEST ) );
			lpQuest->m_wId = static_cast< WORD >( nQuestId );
			lpQuest->m_nState = QS_END;
			return TRUE;
		}
	}
	return FALSE;
}
void CMover::RemoveAllQuest()
{
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
	m_nCheckedQuestSize = 0;
}
void CMover::RemoveCompleteQuest()
{
	m_nCompleteQuestSize = 0;
}


LPQUEST CMover::FindQuest( int nQuestId )
{
	for( size_t i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			return &m_aQuest[ i ];
		}
	}

	return NULL;
}
BOOL CMover::RemoveQuest( int nQuestId )
{
//	QuestProp* pProp = prj.m_aPropQuest.GetAt( nQuestId );

	for( size_t i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			for( ; i < m_nQuestSize - 1; i++ )
			{
				m_aQuest[ i ] = m_aQuest[ i + 1 ];
			}
			m_nQuestSize--;
			break;
		}
	}
	for( size_t i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCompleteQuestSize - 1; i++ )
			{
				m_aCompleteQuest[ i ] = m_aCompleteQuest[ i + 1 ];
			}
			m_nCompleteQuestSize--;
			break;
		}
	}
	for( size_t i = 0; i < m_nCheckedQuestSize; ++i )
	{
		if( m_aCheckedQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCheckedQuestSize -1; ++i )
				m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
			m_aCheckedQuest[ --m_nCheckedQuestSize ] = 0;
			break;
		}
	}

#ifdef __CLIENT
	PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

BOOL CMover::SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest )
{
	QUEST newQuest;
	ZeroMemory( &newQuest, sizeof( QUEST ) );
	newQuest.m_wId = static_cast< WORD >( nQuestId );
	newQuest.m_nState = static_cast< BYTE >( nState );

	LPQUEST lpQuest = FindQuest( nQuestId );
	// 이미 완료된 케스트라면 세팅 실패 
	if( lpQuest == NULL && IsCompleteQuest( newQuest.m_wId ) )
		return FALSE;
	if( lpReturnQuest )
		memcpy( lpReturnQuest, &newQuest, sizeof( QUEST ) );
	return __SetQuest( lpQuest, &newQuest );
}
BOOL CMover::SetQuest( LPQUEST lpNewQuest )
{
	LPQUEST lpQuest = FindQuest( lpNewQuest->m_wId );
	// 이미 완료된 케스트라면 세팅 실패 
	if( lpQuest == NULL && IsCompleteQuest( lpNewQuest->m_wId ) )
		return FALSE;
	return __SetQuest( lpQuest, lpNewQuest );
}
BOOL CMover::__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest )
{
	// 퀘스트를 발견 못했으면 새로 추가한다.
	if( lpQuest == NULL )
	{
		if( m_nQuestSize >= MAX_DOING_QUEST )
		{
			return FALSE;
		}
#ifdef __CLIENT
		PlayMusic( BGM_EV_START );
#endif
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
			lpQuest = &m_aQuest[ m_nQuestSize++ ];
		else
			return FALSE;

		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
		if( pQuestProp && pQuestProp->m_nEndCondLimitTime  )
			lpNewQuest->m_wTime = static_cast< WORD >( pQuestProp->m_nEndCondLimitTime );
#ifdef __WORLDSERVER
		// 타이머 초기화 
		m_timerQuestLimitTime.Reset();
#endif
	}
	else
	// 퀘스트 완료다. 그렇다면 캐쉬에서 뒤져서 제거하자 
	if( lpNewQuest->m_nState == QS_END )
	{
		// 대신, 완료 궤스트 배열에 넣어 보관한다.
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
		{
			for( size_t i = 0; i < m_nQuestSize; i++ )
			{
				if( m_aQuest[ i ].m_wId == lpNewQuest->m_wId )
				{
					for( ; i < m_nQuestSize - 1; i++ )
					{
						m_aQuest[ i ] = m_aQuest[ i + 1 ];
					}
					m_nQuestSize--;
					break;
				}
			}

			for( size_t i = 0; i < m_nCheckedQuestSize; ++i )
			{
				if( m_aCheckedQuest[ i ] == lpNewQuest->m_wId )
				{
					for( ; i < m_nCheckedQuestSize - 1; ++i )
						m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
					m_aCheckedQuest[--m_nCheckedQuestSize] = 0;
					break;
				}
			}

			QuestProp* pProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
			// 퀘스트 타입이 의뢰소라면 완료 배열에 들어가지 않는다. 
			//if( pProp->m_nQuestType != QT_REQUEST && pProp->m_bRepeat == FALSE )
			if( pProp && pProp->m_bRepeat == FALSE )
				m_aCompleteQuest[ m_nCompleteQuestSize++ ] = lpNewQuest->m_wId; 
			lpQuest = NULL;
#if defined( __CLIENT )
			g_QuestTreeInfoManager.DeleteTreeInformation( lpNewQuest->m_wId );
			D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
			rDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );
#endif // defined( __CLIENT )
		}
		else
			return FALSE;
	}
	if( lpQuest )
		memcpy( lpQuest, lpNewQuest, sizeof( QUEST ) );

#ifdef __CLIENT
	if( lpNewQuest->m_nState == QS_END )
		PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

LPQUEST CMover::GetQuest( int nQuestId )
{
	return FindQuest( nQuestId );
}

int CMover::GetItemNum( DWORD dwItemId )
{
	int nNum	= 0;
	DWORD dwSize	= m_Inventory.GetMax();
	FLItemElem* pItemElem;
	for( DWORD i = 0 ; i < dwSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			//raiders.06.05.02 (prevent abuse quest ending condition : trade & quest )
			if( IsUsableItem( pItemElem ) == FALSE )
				return 0;
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}

#ifdef __CLIENT
int CMover::GetItemNumForClient( DWORD dwItemId )
{
	int nNum	= 0;
	DWORD dwSize	= m_Inventory.GetMax();
	FLItemElem* pItemElem;
	for( DWORD i = 0 ; i < dwSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}
#endif //__CLIENT
#ifdef __WORLDSERVER
// 스크립트에서 사용하는 함수
// 퀘스트에서 아이템을 제거할 때 사용한다. 퀘스트 아이템들은 거래나 개인상점에 올릴 수 없기 때문에
// IsUsableItem검사를 하지 않는다.
int CMover::RemoveItemA( DWORD dwItemId, int nNum )
{
	if( nNum == -1 )
	{
		return RemoveAllItem( dwItemId );
	}

	DWORD	dwSize	= m_Inventory.GetMax();
	FLItemElem* pItemElem;
	int nRemnant	= nNum;

	for( DWORD i = 0 ; i < dwSize && nRemnant > 0; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			if( nRemnant > pItemElem->m_nItemNum )
			{
				nRemnant	-= pItemElem->m_nItemNum;
				UpdateItem( i, UI_NUM, 0 );	// remove
			}
			else
			{
				UpdateItem( i, UI_NUM, pItemElem->m_nItemNum - nRemnant );
				nRemnant	= 0;
			}
		}
	}
	return ( nNum - nRemnant );
}
#endif	// __WORLDSERVER

// 스크립트에서 사용하는 함수 
int CMover::RemoveAllItem( DWORD dwItemId )
{
	int nNum = 0;
	DWORD	dwSize	= m_Inventory.GetMax();

	for( DWORD i = 0 ; i < dwSize; ++i )
	{
		FLItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			nNum += pItemElem->m_nItemNum;
			UpdateItem( i, UI_NUM, 0 );		// remove
		}
	}
	return nNum;
}

void CMover::OnTradeRemoveUser()
{
	if( IsPlayer() )
	{
		CMover* pOther = m_vtInfo.GetOther();
		if( IsValidObj( pOther ) && pOther->IsPlayer() )
		{	
			if( pOther->m_vtInfo.GetOther() == this )
			{
				pOther->m_vtInfo.TradeClear();
			}
		}
		m_vtInfo.TradeClear();
	}
}

#ifdef __WORLDSERVER

void CMover::Abrade( CMover* /*pAttacker*/, int /*nParts*/ )
{
}

DWORD CMover::GetRandomPartsAbraded( void )
{
//	실드 50%
	int nRand	= xRand();
	if( nRand % 2 )
		return PARTS_SHIELD;
	nRand	= nRand % 100;
	if( nRand < 20 )	// 20%
		return PARTS_CAP;
	else if( nRand < 60 )	// 40%
		return PARTS_UPPER_BODY;
	else if( nRand < 85 )	// 25%
		return PARTS_HAND;
	else	// 15%
		return PARTS_FOOT;
}
#endif	// __WORLDSERVER



float CMover::GetItemEnduranceInfluence( int nEndurance )
{
	if( nEndurance == 0 )
		return 0.0f;
/*	else if( nEndurance <= 50 )
		return 0.6f;
	else if( nEndurance <= 75 )
		return 0.8f;
*/	return 1.0f;
}

int CMover::GetItemEnduranceWeight( int /*nEndurance*/ )
{
	int nMinusMultyPly = 5;
/*	if( nEndurance <= 50 )
		return (int)3 * nMinusMultyPly;
	else if( nEndurance <= 75 )
		return (int)2 * nMinusMultyPly;
*/	return 1 * nMinusMultyPly;
}

// //sun: 11, 각성, 축복
// void CMover::UpdateItemEx( unsigned char id, char cParam, __int64 iValue )
// {
// 	FLItemElem* pItemElem	= (FLItemElem*)GetItemId( id );
// 
// 	if( pItemElem )
// 	{
// 		switch( cParam )
// 		{
// 			case UI_RANDOMOPTITEMID:
// 				{
// 					pItemElem->SetRandomOptItemId( iValue );
// 					break;
// 				}
// 			default:
// 				break;
// 		}
// #ifdef __CLIENT
// 		CWndInventory* pWnd		= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
// 		if( pWnd )
// 			pWnd->UpdateTooltip();
// #endif	// __CLIENT
// 	}
// #ifdef __WORLDSERVER
// 	if( IsPlayer() )
// 		( (FLWSUser*)this )->AddUpdateItemEx( id, cParam, iValue );
// #endif
// }

void CMover::UpdateItem( DWORD dwId, CHAR cParam, DWORD dwValue, DWORD dwTime )
{
	FLItemBase* pItemBase = GetItemId( dwId );
	if( pItemBase == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ pItemBase is NULL ]" ) );
		return;
	}

	const T_ITEM_SPEC* pItemSpec	= pItemBase->GetProp();
	if( pItemSpec == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ pItemSpec is NULL ]" ) );
		return;
	}

	if( pItemBase )
	{
#ifdef __CLIENT
		//gmpbigsun( 20100706 ) : m_bControlForbidden 이 상점에서 아이템을 팔경우 패킷받기전에 조작하면서 크래쉬되는 현상 수정때문에 들어간듯 하다.
        //패킷을 받아서 아이템이 업데이트 되는경우 Lock을 여기서 풀어주자.
		if( pItemBase->m_bControlForbidden )
			pItemBase->m_bControlForbidden = FALSE;
		//gmpbigsun end
#endif //__CLIENT

		switch( cParam )
		{
			case UI_COOLTIME:
			case UI_NUM:
				{
					if( (int)dwValue <= 0 ) 
					{
						RemoveItemId( dwId );
					}
					else
					{
						/////////////////////////////////////////////////////////////////////////////
						//	BEGIN100708
						if( dwValue > pItemSpec->dwPackMax )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "UI_NUM Error. m_idPlayer [%d], Serial[%d], dwId [%d], OBJID[%d], ItemID[%d]" ),
								m_idPlayer, pItemBase->GetSerialNumber(), dwId, pItemBase->m_dwObjId, pItemBase->m_dwItemId );

							RemoveItemId( dwId );
						}
						else
						{
							( (FLItemElem*)pItemBase )->m_nItemNum	= (int)( dwValue );
						}
						//	END100708
						/////////////////////////////////////////////////////////////////////////////
					}
					break;
				}
			case UI_HP:
				{
					( (FLItemElem*)pItemBase )->m_nHitPoint	= dwValue;
					UpdateParam();
					break;
				}
			case UI_REPAIR_NUM:
				{
					( (FLItemElem*)pItemBase )->m_nRepair	= dwValue;
					break;
				}
			case UI_RN:
				{
					if( ( (FLItemElem*)pItemBase )->m_nRepairNumber >= 100 )
						return;
					if( dwValue > 100 )
						dwValue		= 100;
					( (FLItemElem*)pItemBase )->m_nRepairNumber	= (BYTE)( dwValue );
					UpdateParam();
				}
				break;
				
			case UI_AO: // 아이템 + 올리기...
				{
					DWORD dwMax = 20;		//sun:13, 제련 확장(속성, 일반)

//sun: 11, 채집 시스템
					if( ( (FLItemElem*)pItemBase )->IsAccessory() == TRUE || pItemSpec->IsAccessory() == TRUE )
						dwMax	= 20;

//sun: 11, 각성, 축복
					if( ( (FLItemElem*)pItemBase )->IsCollector( TRUE ) )
						dwMax	= 5;

					if( ( (FLItemElem*)pItemBase )->GetAbilityOption() > (int)( dwMax ) )
						return;
					if( dwValue > dwMax )
						dwValue		= dwMax;

					( (FLItemElem*)pItemBase )->SetAbilityOption( dwValue );
					UpdateParam();
				}
				break;
			case UI_RAO: // 아이템 속성 + 올리기...
				{
					if( ( (FLItemElem*)pItemBase )->m_nResistAbilityOption > 20 )		//sun:13, 제련 확장(속성, 일반)
						return;
//sun:13, 제련 확장(속성, 일반)
					if( dwValue > 20 )
						dwValue		= 20;

					( (FLItemElem*)pItemBase )->m_nResistAbilityOption	= dwValue;

					if( ( (FLItemElem*)pItemBase )->m_nResistAbilityOption <= 0 )
					{
						( (FLItemElem*)pItemBase )->m_byItemResist	= SAI79::NO_PROP;
					}
					UpdateParam();
				}
				break;
			case UI_IR:  // 아이템에 무슨속성인지 넣기
				{
					( (FLItemElem*)pItemBase )->m_byItemResist	= (BYTE)( dwValue );
					UpdateParam();
				}
				break;

			case UI_PIERCING_SIZE:
				if( static_cast<FLItemElem*>( pItemBase )->IsEatPet() ) {
					return;
				}
				( (FLItemElem*)pItemBase )->SetGeneralPiercingSize( dwValue );
				break;
			case UI_PIERCING:
				{
					if( static_cast<FLItemElem*>( pItemBase )->IsEatPet() ) {
						return;
					}
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (FLItemElem*)pItemBase )->SetGeneralPiercingItemID( wIndex, wItemId );
				}
				break;

//sun: 12, 무기 피어싱
			case UI_ULTIMATE_PIERCING_SIZE:
				( (FLItemElem*)pItemBase )->SetUltimatePiercingSize( dwValue );
				break;
			case UI_ULTIMATE_PIERCING:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (FLItemElem*)pItemBase )->SetUltimatePiercingItemID( wIndex, wItemId );
				}
				break;

			case UI_PETVIS_SIZE:	
				{
					static_cast<FLItemElem*>(pItemBase)->SetGeneralPiercingSize( dwValue );
					static_cast<FLItemElem*>(pItemBase)->SetVisPetPiercingSize( dwValue );

#ifdef __CLIENT	//sun : 실체 비스 장착이( 서버로부터 패킷을 받아서 처리될때 )이루어질때 효과음 출력 
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;
			case UI_PETVIS_ITEM:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					static_cast<FLItemElem*>(pItemBase)->SetGeneralPiercingItemID( wIndex, wItemId );
					static_cast<FLItemElem*>(pItemBase)->SetVisPetPiercingItemKeepTime( wIndex, ( dwTime * 60 ) + time_null() ); // 분단위 시간을 초단위로 변경

#ifdef __CLIENT
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;

			case UI_PETVIS_ITEMSWAP:
				{
					WORD wPos1	= LOWORD( dwValue ), wPos2	= HIWORD( dwValue );
					static_cast<FLItemElem*>(pItemBase)->SwapVisItem( wPos1, wPos2 );

#ifdef __CLIENT
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;

			case UI_TRANSFORM_VISPET:
				{
					static_cast<FLItemElem*>(pItemBase)->m_bTranformVisPet = static_cast<BOOL>( dwValue );
					static_cast<FLItemElem*>(pItemBase)->m_bCharged = TRUE;
				}
				break;

			case UI_RANDOMOPTITEMID:
				{
					static_cast<FLItemElem*>( pItemBase )->SetRandomOptionOriginID( static_cast<WORD>( dwValue ) );
					break;
				}
			case UI_KEEPTIME:	// offset
				{
					CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, dwValue, 0 );
					static_cast<FLItemElem*>( pItemBase )->m_dwKeepTime	= static_cast<DWORD>( tm.GetTime() );
					break;
				}

			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= static_cast<DWORD>( LOWORD( dwValue ) );
					BYTE byFlag		= static_cast<BYTE>( HIWORD( dwValue ) );
					int nParts	= dwObjIndex;
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
						m_aEquipInfo[nParts].byFlag	= byFlag;
					static_cast<FLItemElem*>( pItemBase )->m_byFlag		= byFlag;
					UpdateParts( FALSE );
#endif	// __CLIENT
					UpdateParam();
					break;
				}

			case UI_EQUIP_LEVEL:
				{
					if( static_cast<int>( dwValue ) > 0 )
					{
						static_cast<FLItemElem*>( pItemBase )->SetLevelDown( dwValue );
					}
					else
					{
						static_cast<FLItemElem*>( pItemBase )->InitializeLevelDown();
					}
				}
				break;

			case UI_LOOKS_CHANGE:
				{
					dwValue > 0 ? static_cast<FLItemElem*>( pItemBase )->SetLooksChangeItemID( dwValue )
								: static_cast<FLItemElem*>( pItemBase )->InitializeLooksChangeItemID();
				}
				break;

			default:
				break;
		}
//sun: 9차 전승관련 Clienet
#ifdef __CLIENT
		CWndInventory* pWnd	= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
		if( pWnd )
			pWnd->OnMouseWndSurface( pWnd->GetMousePoint() );
		//	pWnd->UpdateTooltip();

#endif	// __CLIENT
	}
	else
	{
		switch( cParam )
		{
			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= (DWORD)(short)LOWORD( dwValue );
					BYTE byFlag		= (BYTE)HIWORD( dwValue );
					int nParts	= dwObjIndex;
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
					{
						m_aEquipInfo[nParts].byFlag	= byFlag;
						UpdateParts( TRUE );
					}
#endif	// __CLIENT
					break;
				}
			default:
				break;
		}
	}
#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		static_cast<FLWSUser*>( this )->AddUpdateItem( 0, dwId, cParam, dwValue, dwTime );
		if( pItemBase && cParam == UI_TRANSFORM_VISPET && HasActivatedEatPet() )
		{
			InactivateEatPet();
			ActivateEatPet( static_cast<FLItemElem*>( pItemBase ) );
		}
	}
#endif // __WORLDSERVER
}

float CMover::GetItemDropRateFactor( CMover* /*pAttacker*/ )
{
	float fFactor	= 1.0F;
#ifdef __WORLDSERVER
	fFactor		*= prj.m_fItemDropRate;
#ifdef __S1108_BACK_END_SYSTEM
	fFactor		*= GetProp()->m_fItemDrop_Rate;
#endif // __S1108_BACK_END_SYSTEM

	fFactor		*= CEventGeneric::GetInstance()->GetItemDropRateFactor();

	//fFactor	*= prj.m_EventLua.GetDropFactor() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue();	//sun: 9, 이벤트 (루아 스크립트 적용)
	const float fMulRate = ( prj.m_EventLua.GetItemDropRate() - 1.0f ) + ( ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_DROP_RATE().GetRewardValue() / ( float )REWARD_PRECISION ) - 1.0f ) + 1.0f;
	fFactor			*= fMulRate;

#endif	// __WORLDSERVER
	return fFactor;
}

float CMover::GetPieceItemDropRateFactor( CMover* pAttacker )
{
	float fFactor	= 1.0F;

#ifdef __WORLDSERVER
	fFactor		*= prj.m_EventLua.GetPieceItemDropRate();		//sun: 9, 이벤트 (루아 스크립트 적용)

//sun: 12, 군주
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetIFactor();
	if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) )
		fFactor		*=	1.1F;
	if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) )
		fFactor		*= 1.05F;

//sun: 13, 커플 보상
	if( pAttacker->HasBuff( BUFF_ITEM, ITEM_INDEX( 26740, II_COUPLE_BUFF_MIRACLE_01 ) ) )
		fFactor	*= 1.05F;

	if( pAttacker->IsPlayer() )
		fFactor *= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast<FLWSUser*>( pAttacker ) );
#endif // __WORLDSERVER
	return fFactor;
}

#ifdef __WORLDSERVER
float CMover::GetExpFactor( void )
{
	float fFactor	= 1.0f;
	if( IsMode( EVENT_OLDBOY_MODE ) )
		fFactor	*= 1.1f;

	BOOL bGlvCheer	= FALSE;

	int nValue = g_xApplyItemEffect->GetAdjValueDuplicationBuff( (FLWSUser*)this, IK3_EXP_RATE );

	if( nValue > 0 )
	{
		fFactor *= ( (float)( (float)nValue / 100.0f ) + 1.0f );
	}

	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 10467, II_SYS_SYS_SCR_GLVCHEER ) ) )
	{
		bGlvCheer	= TRUE;
		fFactor	*= 1.10f;
	}

	if( !bGlvCheer && HasBuff( BUFF_ITEM, ITEM_INDEX( 10445, II_CHEERUP ) ) )
	{
#ifdef __EVENTLUA_CHEEREXP
		if( prj.m_EventLua.GetCheerExpFactor() > 1.05f )
			fFactor *= prj.m_EventLua.GetCheerExpFactor();
		else
#endif // __EVENTLUA_CHEEREXP
			fFactor	*= 1.05f;
	}

#ifdef __WORLDSERVER

	fFactor	*= CEventGeneric::GetInstance()->GetExpFactor();
	//fFactor	*= prj.m_EventLua.GetExpFactor() + nsCooperativeContributions::GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue();	//sun: 9, 이벤트 (루아 스크립트 적용)
	//const float fMulRate = ( prj.m_EventLua.GetExpFactor() - 1.0f ) + ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue() - 1.0f ) + 1.0f;
	const float fMulRate	= ( prj.m_EventLua.GetExpFactor() - 1.0f ) + ( ( nsCooperativeContributions::GLOBAL_REWARD_ALARM_EXP_FACTOR().GetRewardValue() / ( float )REWARD_PRECISION ) - 1.0f ) + 1.0f;
	fFactor			*= fMulRate;

	fFactor	*= prj.m_fMonsterExpRate;

//sun: 9,10차 제련 __ULTIMATE
	int nDstExp = GetParam( DST_EXPERIENCE, 0 );
	if( 0 < nDstExp )
		fFactor *= ( 1.0f + ( nDstExp / 100.0f ) );

//sun: 12, 군주
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetEFactor();
	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 20019, II_SYS_SYS_LS_CHEERING ) ) )
		fFactor		*=	1.1F;
	if( HasBuff( BUFF_ITEM, ITEM_INDEX( 20036, II_SYS_SYS_VOTE_THANKS ) ) )
		fFactor		*=	1.05F;

	if( g_pSeasonEffect->getEffect() == true && g_pSeasonEffect->getSeason() != SEASON_NONE )
	{
		fFactor *= prj.m_EventLua.GetWeatherEventExpFactor();
	}

#endif	// __WORLDSERVER

	return fFactor;
}
#endif // __WORLDSERVER


bool	CMover::SetSleeping( bool bApply, DWORD dwDuration )
{
	if( bApply == true )
	{
		MoverProp* pMoverProp = GetProp();

		if( pMoverProp != NULL )
		{
			if( GetAdjParam( DST_IMMUNITY ) & CHS_SLEEPING ) 
			{
				return false;
			}

			SetDestParam( DST_CHRSTATE, CHS_SLEEPING, NULL_CHGPARAM, FALSE );
			SendActMsg( OBJMSG_SLEEPING );
			m_dwSleepingDuration = (DWORD)( ( dwDuration / 1000.0f ) * PROCESS_COUNT );
		}
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_SLEEPING )
		{
			ResetDestParam( DST_CHRSTATE, CHS_SLEEPING, FALSE );
		}
		m_dwSleepingDuration = 0;
		m_dwFlag &= (~MVRF_NOACTION);
	}

#ifdef __WORLDSERVER
	g_xWSUserManager->AddSetSleeping( this, dwDuration );
#endif // Worldserver

	return true;
}

bool	CMover::IsNoActionState()
{
	if( GetAdjParam( DST_CHRSTATE ) & CHS_SLEEPING )
	{
		return true;
	}

	if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )
	{
		return true;
	}

	return false;
}
