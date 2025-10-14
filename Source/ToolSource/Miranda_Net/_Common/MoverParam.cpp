// MoverDecision.cpp: implementation of the CMover class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineQuest.h"
#include "resdata.h"
#include "defineObj.h"
#ifdef __WORLDSERVER
#include "DialogMsg.h"
#include "user.h"
#include "dpcoreclient.h"

extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER

#ifdef __CLIENT			
#include "Dialogmsg.h"
#endif

#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef __WORLDSERVER
#include "DPSrvr.h"
extern	CDPSrvr		g_DPSrvr;
#endif // __WORLDSERVER

#include "Project.h"
#include "ProjectCmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LPSKILL CMover::GetSkill( int nType, int nIdx )
{
	// nType == 0이면 직업 스킬
	// nType == 1이면 라이선스 스킬 
	if( nIdx >= MAX_SKILL_JOB )
	{
		Error( "CMover::GetSkill : %s nId = %d 범위를 벗어남", m_szName, nIdx );
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

BOOL CMover::CheckSkill( DWORD dwSkill )
{
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	if( pSkillProp == NULL || pSkillProp->nLog == 1 )
		return FALSE;
	
	if( GetLevel() < pSkillProp->dwReqDisLV )
		return FALSE;
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill1 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
	}	
	
	return TRUE;
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


// 케릭터가 레벨업시 스킬중에 케릭터레벨땜시 활성화 안된것들 활성여부 체크하여 정보출력
void CMover::PutLvUpSkillName_1( DWORD dwLevel )
{
#ifdef __CLIENT
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;
		
		ItemProp* pSkillProp = prj.GetSkillProp( m_aJobSkill[i].dwSkill );
		
		if( pSkillProp && dwLevel == pSkillProp->dwReqDisLV )
		{
			if( pSkillProp->dwReSkill1 != -1 )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill1);

				if( pSkillProp->dwReSkillLevel1 > pSkill->dwLevel )
					continue;
			}
			if( pSkillProp->dwReSkill2 != -1 )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill2);
				
				if( pSkillProp->dwReSkillLevel2 > pSkill->dwLevel )
					continue;
			}

			CString str;
			str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );
		}
	}
#endif //__CLIENT
}		

BOOL IsSkillParent( CMover* pMover, DWORD dwSkill, DWORD dwChildSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReSkill1 == dwChildSkill || pSkillProp->dwReSkill2 == dwChildSkill )
		return TRUE;

	return FALSE;
}

BOOL IsActive( CMover* pMover, DWORD dwSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReSkill1 != NULL_ID )
	{
		LPSKILL pSkill1;
		ItemProp* pSkillProp1;

		pSkill1		= pMover->GetSkill( pSkillProp->dwReSkill1 );
		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
	
		if( pSkillProp1 == NULL )
			return FALSE;
		
		if( pSkillProp1->dwReqDisLV > pMover->GetLevel() )
			return FALSE;
		
		if( pSkillProp->dwReSkillLevel1 != pSkill1->dwLevel )
			return FALSE;
	}

	if( pSkillProp->dwReSkill2 != NULL_ID )
	{
		LPSKILL pSkill1;
		ItemProp* pSkillProp1;
		
		pSkill1		= pMover->GetSkill( pSkillProp->dwReSkill2 );
		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		
		if( pSkillProp1 == NULL )
			return FALSE;
		
		if( pSkillProp1->dwReqDisLV > pMover->GetLevel() )
			return FALSE;
		
		if( pSkillProp->dwReSkillLevel2 != pSkill1->dwLevel )
			return FALSE;		
	}
	
	if( pSkillProp->dwReqDisLV > pMover->GetLevel() )
		return FALSE;

	return TRUE;
}


// 스킬 레벨업시 케릭터 레벨땜시 안나왔던 스킬들 검색하여 정보 출력
void CMover::PutLvUpSkillName_2( DWORD dwSkill )
{
#ifdef __CLIENT
	LPSKILL pSkill = GetSkill( dwSkill );
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
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
				ItemProp* pSkillProp1;
				pSkillProp1 = prj.GetSkillProp( m_aJobSkill[i].dwSkill );

				CString str;
				str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp1->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );	
			}
		}
	}
#endif // CLIENT
}


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
int CMover::GetJobBase()
{
	return prj.m_aJob[ m_nJob ].dwJobBase;
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
	return FALSE;
}

// 스킬수 3(기본)14(익스퍼트)7(프로페셔널)
// 직업 10, 익스퍼트 40. 프로 30
BOOL  CMover::SetExpert( int nJob )
{
	if( m_nJob == nJob )
		return FALSE;
	m_nJob = nJob;

	int nJobBase = GetJob();
	m_aJobLv[ nJobBase ] = 0;
	int nNum = 3; // 3은 기본 스킬의 갯수다. 3 이상부터 익스퍼트 스킬이 추가된다는 의미 
	LPSKILL lpSkill;
	ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
	int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
	for( int i = 0; i < nJobNum; i++ )
	{
		ItemProp* pSkillProp = apSkillProp[ i ];
		lpSkill = &m_aJobSkill[ nNum++ ];
		lpSkill->dwSkill = pSkillProp->dwID;
	}
	return TRUE;
}

int   CMover::GetJob()
{
	return m_nJob; 
}
int   CMover::GetRemainGP()
{
	return m_nRemainGP;
}
int   CMover::GetRemainLP()
{
	return m_nRemainLP;
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
		return (GetDex() / 2) + GetParam( DST_PARRY, m_nAdjParry );
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
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	if( nAbilityMax == nAbilityMin )
		return nAbilityMin;
	return nAbilityMin + ( xRand() % (nAbilityMax - nAbilityMin ) );
}
// 아이템의 최소 성능을 얻는다.
int CMover::GetItemAbilityMin( int nItem )
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	return nAbilityMin;
}
// 아이템의 최대 성능을 얻는다.
int CMover::GetItemAbilityMax( int nItem )
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
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
#ifdef __EVENT1206
	if( g_eLocal.GetState( EVE_EVENT1206 ) )
		return;
#endif	// __EVENT1206
#endif // __WORLDSERVER
	SetPointParam( DST_MP, m_nManaPoint + nVal );
}

void CMover::IncFatiguePoint(int nVal) 
{ 
#ifdef __WORLDSERVER
	if( IsSMMode( SM_MAINTAIN_FP ) && nVal < 0 )
		return;
#ifdef __EVENT1206
	if( g_eLocal.GetState( EVE_EVENT1206 ) )
		return;
#endif	// __EVENT1206
#endif // __WORLDSERVER
	SetPointParam( DST_FP, m_nFatiguePoint + nVal );
}

SLAUGHTER_GRADE CMover::GetSlaughterGrade()
{
#ifdef __NPP_050308
	return prj.GetSlaughterGrade( m_nSlaughter );		// project.h
#else
	if( m_nSlaughter == 0 )
		return SLAUGHTER_NORMAL;
	else
		return SLAUGHTER_CHAOTIC;
#endif
}

PK_TYPE GetPKCase( CMover *pAttacker, CMover *pDefender )
{
	SLAUGHTER_GRADE atkGrade = pAttacker->GetSlaughterGrade();			
	SLAUGHTER_GRADE defGrade = pDefender->GetSlaughterGrade();			

	if( pAttacker->IsMode( RECOVERCHAO_MODE ) )		// 카르마 회복 기간에 있는 상태에서 다시 다른 유저를 가격할 경우, 
		atkGrade = SLAUGHTER_SEMI_CHAOTIC;			// 준 카오 유저와 같은 룰이 적용이 된다.
	
	static PK_TYPE table[3][3] = {
		{ NORMAL_AND_NORMAL, NORMAL_AND_SEMI, NORMAL_AND_CHAO },
		{ SEMI_AND_NORMAL,   SEMI_AND_SEMI,   SEMI_AND_CHAO },
		{ CHAO_AND_NORMAL,   CHAO_AND_SEMI,   CHAO_AND_CHAO }
	};

	return table[ atkGrade ][ defGrade ];
}


// 시련모드 설정 
void CMover::SetRecoverMode()
{
#ifdef	__NPP_050308 
	if( IsChaotic() )
	{
		// 시련모드 일때의 슬로터포인트를 저장해둔다.(시련모드 끝날때 페널티 복구시 사용)
		m_nRecoverChao_Slaughter = m_nSlaughter;
		SetMode( RECOVERCHAO_MODE );
	}
#endif	// __NPP_050308 
}

// 듀얼후의 결과 처리.
// this가 승자가 된다.
// pLose는 패배자.
#ifndef	__BEAST

#ifdef __WORLDSERVER

// PK시도에 의한 m_nSlaughter(=카르마 수치) 변경  
// nDead : 죽은자, return : 증감량.
int CMover::IncSlaughterPoint2( CMover *pDead )
{
	ASSERT( pDead->IsPlayer() && IsPlayer() );

	int nOld = m_nSlaughter;
	PK_TYPE type = GetPKCase( this, pDead );

	if( IsRegionAttr( RA_PENALTY_PK ) == FALSE )
		return 0;

	switch( type )
	{
	case NORMAL_AND_NORMAL:		// 일반 유저 와 일반 유저 
		m_nSlaughter -= 50;
		break;
	case NORMAL_AND_SEMI:
		break;
	case NORMAL_AND_CHAO:
		break;

	case SEMI_AND_NORMAL:		// 준카오와 일반 유저 
		m_nSlaughter -= 30;
		break;
	case SEMI_AND_SEMI:
		break;
	case SEMI_AND_CHAO:
		break;

	case CHAO_AND_NORMAL:		// 카오와 일반 유저 
		break;
	case CHAO_AND_SEMI:
		break;
	case CHAO_AND_CHAO:
		break;
	}
	return ( m_nSlaughter - nOld );

}

// PK에 의한 경우, slaughter값을 변경시킨다.
// pDead : 죽은자, return : 증감량.
int CMover::IncSlaughterPoint( CMover *pDead )
{
#ifdef __NPP_050308
	ASSERT( pDead->IsPlayer() && IsPlayer() );

	int nOld = m_nSlaughter;

	PK_TYPE type = GetPKCase( this, pDead );

	switch( type )
	{
	case NORMAL_AND_NORMAL:		// 일반 유저 와 일반 유저 
		if( IsRegionAttr( RA_PENALTY_PK ) )
			m_nSlaughter = -2000;
		else if( IsRegionAttr( RA_PK ) )
			m_nSlaughter -= 50;
		break;
	case NORMAL_AND_SEMI:
		break;
	case NORMAL_AND_CHAO:
		{
			// 상대가 버서커 모드에서는 그 유저의 카르마 포인트를 가져올 수 없다
			if( pDead->IsAfterDeath() == FALSE )
			{
				// 절대값의 차가 큰 경우만 카르마 포인트를 얻는다.
				int nGradeA = prj.GetKarmaProp( m_nSlaughter )->nGrade;
				int nGradeB = prj.GetKarmaProp( pDead->m_nSlaughter )->nGrade;
				int nDelta = abs(nGradeB) - abs(nGradeA);
				if( nDelta > 0 )
					m_nSlaughter += nDelta;
			}
		}	
		break;

	case SEMI_AND_NORMAL:		// 준카오와 일반 유저 
		if( IsRegionAttr( RA_PENALTY_PK ) )
			m_nSlaughter -= 2000;
		else if( IsRegionAttr( RA_PK ) )
			m_nSlaughter -= 50;
		break;
	case SEMI_AND_SEMI:
		break;

	case SEMI_AND_CHAO:
		break;

	case CHAO_AND_NORMAL:		// 카오와 일반 유저 
		if( IsRegionAttr( RA_PENALTY_PK ) )
		{
			int nGrade = prj.GetKarmaProp( m_nSlaughter )->nGrade;
			switch( nGrade )
			{
			case -4: m_nSlaughter -= 5; break;
			case -5: m_nSlaughter -= 3; break;
			case -6: m_nSlaughter -= 1; break;
			}
		}
		break;
	case CHAO_AND_SEMI:
		break;
	case CHAO_AND_CHAO:
		break;
	}

	return ( m_nSlaughter - nOld );

#else // !__NPP_050308
	// 슬로터포인트 증가량을 공식으로 만들었으면 함.
	int nVal = 1000;
	int nGap = pDead->GetLevel() - GetLevel();		// 어태커와 죽은자의 레벨차이.

	if( pDead->IsPlayer() )		// 죽은자가 플레이어 일경우 적용
	{
		if( m_nDuel && GetId() == pDead->m_idDuelOther )	return 0;	// 나랑 듀얼(개인/파티)중인 넘이면 슬로터포인트 계산 안함.
		if( pDead->IsChaotic() == FALSE && pDead->m_tmCrime == 0 )	// 죽은자가 착한넘일경우. 포인트 증가
		{
			if( nGap >= 50 )		nVal = 100;		// 나보다 강한놈을 죽였음
			else if( nGap >= 40 )	nVal = 200;
			else if( nGap >= 30 )	nVal = 300;
			else if( nGap >= 20 )	nVal = 400;
			else if( nGap >= 15 )	nVal = 500;
			else if( nGap >= 10 )	nVal = 600;
			else if( nGap >= 7 )	nVal = 700;
			else if( nGap >= 4 )	nVal = 800;
			else if( nGap >= 1 )	nVal = 900;
			else if( nGap == 0 )	nVal = 1000;
			else if( nGap <= -50 )	nVal = 23000;	// 나보다 약한놈을 죽였음
			else if( nGap <= -40 )	nVal = 17000;
			else if( nGap <= -30 )	nVal = 12000;
			else if( nGap <= -20 )	nVal = 8000;
			else if( nGap <= -15 )	nVal = 5000;
			else if( nGap <= -10 )	nVal = 2950;
			else if( nGap <= -7 )	nVal = 1950;
			else if( nGap <= -4 )	nVal = 1550;
			else if( nGap <= -1 )	nVal = 1250;
		} else
		{	// 죽은자가 슬로터였을 경우. 포인트 감소
			if( nGap >= 9 )			nVal = -100;	// 나보다 강한놈을 죽였음
			else if( nGap >= 6 )	nVal = -80;
			else if( nGap >= 3 )	nVal = -60;
			else if( nGap >= 0 )	nVal = -40;
			else if( nGap <= -9 )	nVal = -5;		// 나보다 약한놈을 죽였음
			else if( nGap <= -6 )	nVal = -10;		
			else if( nGap <= -3 )	nVal = -20;
			else if( nGap <= 0 )	nVal = -40;
		}
	} else
	// 죽은자가 몹일 경우 적용
	{
		// 레벨차에 따라 포인트 감소.
		if( nGap >= 6 )			nVal = -10;		// 나보다 강한놈을 죽였음
		else if( nGap >= 3 )	nVal = -7;
		else if( nGap >= 0 )	nVal = -5;
		else if( nGap <= 6 )	nVal = -1;		// 나보다 약한놈을 죽였음
		else if( nGap <= 3 )	nVal = -3;
	}

	if( m_nSlaughter == 0 && nVal <= 0 )	// 더이상 깍일게 없으면 0을 리턴하자.
		return 0;
	m_nSlaughter += nVal;
	if( m_nSlaughter < 0 )	m_nSlaughter = 0;

	return nVal;
#endif	// !__NPP_050308
}

void CMover::ChangeFame( CMover *pLose )
{
	int nVal = 0;
	float fVal = 0;
	int nGap = 0;

#ifdef __NPP_050308
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
#else // !__NPP_050308
	nGap = pLose->m_nFame - m_nFame;		
	
	if( nGap >= 500 )		{ nVal = 50; fVal = 10.0f; }	// 패자가 명성이 훨 높았다.
	else if( nGap >= 100 )	{ nVal = 30; fVal = 9.0f; }
	else if( nGap >= 10 )	{ nVal = 10; fVal = 8.0f; }
	else if( nGap <= -500 )	{ nVal = 0;  fVal = 2.0f; }		// 패자가 명성이 훨 낮았다.
	else if( nGap <= -200 )	{ nVal = 1;  fVal = 3.0f; }
	else if( nGap <= -100 )	{ nVal = 2;  fVal = 4.0f; }
	else if( nGap <= -50 )	{ nVal = 3;  fVal = 5.0f; }
	else if( nGap <= -10 )	{ nVal = 4;  fVal = 6.0f; }
	else					{ nVal = 5;  fVal = 7.0f; }	// 9 ~ -9
#endif 

	if( nVal )		// 승자에게 명성치 줌
	{	
#ifdef __NPP_050308		
		if( this->IsFly() && pLose->IsFly() )	// 공중 듀얼의 경우에는 패배시 상실하는 명성 포인트는 동일하지만, 승리 시 획득하는 명성포인트는 지상 전투에 2배로 한다
			nVal *= 2;
#endif // __NPP_050308				
		m_nFame += nVal;	
		g_UserMng.AddSetFame( this, m_nFame );
		((CUser*)this)->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nVal );	// xxx 명성 포인트를 얻었다
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

		g_UserMng.AddSetFame( pLose, pLose->m_nFame );
		((CUser*)pLose)->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx 명성 포인트가 감소.
		g_dpDBClient.SendLogPkPvp( this, pLose, nVal, 'D' );
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

/*
	if( pLose->m_nDuel == 2 )
	{
#ifdef __NPP_050308
		if( pLose->m_idDuelParty != m_idparty || m_nDuelState == 1 )
#else // __NPP_050308
		if( pLose->m_idDuelParty != m_idparty )
#endif // __NPP_050308
			return 0;
	}
*/		
	// 승자는 레벨차에따라 경험치 상승
	// 패자는 레벨차에따라 경험치% 하락


	if( pLose->m_nDuel == 1 )	// 개인 PvP
	{
		ChangeFame( pLose );
	}
	else
	// 극단 듀얼.
	if( pLose->m_nDuel == 2 )		// 극단 듀얼중일땐 극단장끼리의 레벨을 비교.
	{
		int nVal = 0;
		float fVal = 0;
		int nGap = 0;

		g_PartyMng.Lock( theLineFile );
		CParty *pLoseParty = g_PartyMng.GetParty( pLose->m_idparty );
		if( pLoseParty == NULL || pLoseParty->IsLeader( pLose->m_idPlayer ) == FALSE )		// 파티가 없거나 죽은넘이 대장이 아니면 걍 리턴.
		{
			g_PartyMng.Unlock( theLineFile );
			return 0;
		}
		
		CParty *pParty = g_PartyMng.GetParty( m_idparty );		// 이긴측 파티.
		if( pParty == NULL )
		{
			g_PartyMng.Unlock( theLineFile );
			return 0;
		}

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

#ifdef __NPP_050308
		g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pLoseParty->m_uPartyId, FALSE );
#endif	// __NPP_050308

		pLose->m_bLastDuelParty = TRUE;
		pLoseParty->DoDuelResult( pParty, 0, nVal, fVal );		// 파티원들에게 패배사실을 알리고 듀얼 중지시킴.
		pParty->DoDuelResult( pLoseParty, 1, nVal, fVal );		// 파티원들에게 승리를 알림.
		
		g_PartyMng.Unlock( theLineFile );
	}
#endif	// __WORLDSERVER

	return 1;
}
#endif	//	__BEAST

// 히트포인트를 셋하기
void CMover::SetHitPoint(int nVal) 
{
	m_nHitPoint += nVal;
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

void CMover::SetPointParam( int nDstParameter, int nValue )
{
#ifdef __WORLDSERVER
	int nMax = nValue;
#endif // __WORLDSERVER
	if( nValue < 0 )	nValue	= 0;

	switch( nDstParameter )
	{
		case DST_HP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxHitPoint() || nMax == 999999999 )	nValue	= GetMaxHitPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxHitPoint() )	nValue	= GetMaxHitPoint();
#endif // __WORLDSERVER
			m_nHitPoint		= nValue;
			
			break;
		case DST_MP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxManaPoint() || nMax == 999999999 )	nValue	= GetMaxManaPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxManaPoint() )	nValue	= GetMaxManaPoint();
#endif // __WORLDSERVER
			m_nManaPoint	= nValue;
			break;
		case DST_FP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() || nMax == 999999999 )	nValue	= GetMaxFatiguePoint();
#else // __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() )		nValue	= GetMaxFatiguePoint();
#endif // __WORLDSERVER
			m_nFatiguePoint		= nValue;
			break;
		case DST_GOLD:
			//m_dwGold	= (DWORD)nValue;
	#ifdef __CLIENT
			if( IsActiveMover() )
				PLAYSND(SND_INF_TRADE);
	#endif //__CLIENT
			SetGold( nValue );
			break;
		case DST_PXP:
			m_nExp2		= static_cast<EXPINTEGER>( nValue );
			break;
	}
#ifdef __WORLDSERVER
	g_UserMng.AddSetPointParam( this, nDstParameter, nValue );
#endif	// __WORLDSERVER
}
BOOL CMover::IncJobLevel( int nJobBase )
{
	DWORD dwRequireExpLPPoint;
//	if( IsExpert() && GetJobBase() == nJobBase )
	if( IsExpert() && GetJob() == nJobBase )
	{
		//if( m_dwExpertLv >= MAX_EXP_LEVEL ) 
		if( m_aJobLv[ nJobBase ] >= MAX_EXP_LEVEL ) 
			return FALSE;
		dwRequireExpLPPoint = prj.m_aExpLPPoint[ m_aJobLv[ nJobBase ] ];
		if( (int)m_nRemainLP >= (int)dwRequireExpLPPoint )
		{
			m_aJobLv[ nJobBase ]++;
			m_nRemainLP -= dwRequireExpLPPoint;
			return TRUE;
		}
	}
	else
	{
		if( m_aJobLv[ nJobBase ] >= MAX_JOB_LEVEL )
			return FALSE;
		dwRequireExpLPPoint = prj.m_aExpLPPoint[ m_aJobLv[ nJobBase ] ];
		if( m_nRemainLP >= dwRequireExpLPPoint )
		{
			m_aJobLv[ nJobBase ]++;
			m_nRemainLP -= dwRequireExpLPPoint;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CMover::IncStrLevel()
{
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwStr )		// 이부분 (int)로 캐스트 해주지 않으면 컴파일러 버그 난다.
	{
		m_nStr += pJobAbillity->dwStrAdd;
		m_nRemainGP -= pJobAbillity->dwStr;
		return TRUE;
	}
	return FALSE;
}
BOOL CMover::IncStaLevel()
{
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwSta )
	{
		m_nSta += pJobAbillity->dwStaAdd;
		m_nRemainGP -= pJobAbillity->dwSta;
		return TRUE;
	}
	return FALSE;
}
BOOL CMover::IncDexLevel()
{
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwDex )
	{
		m_nDex += pJobAbillity->dwDexAdd;
		m_nRemainGP -= pJobAbillity->dwDex;
		return TRUE;
	}
	return FALSE;
}
BOOL CMover::IncIntLevel()
{
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwInt )
	{
		m_nInt += pJobAbillity->dwIntAdd;
		m_nRemainGP -= pJobAbillity->dwInt;
		return TRUE;
	}
	return FALSE;
}

void CMover::DecPXP( DWORD dwExp, int nSkillIdx )
{
	if( nSkillIdx < 0 || nSkillIdx > MAX_SKILL_JOB )
	{
		Error( "DecPXP : nSkillIdx의 값이 범위초과.%d", nSkillIdx );
		return;
	}

	LPSKILL pSkill = &m_aJobSkill[ nSkillIdx ];
	if( pSkill->bStop == FALSE )
		return;
	
	if( dwExp <= m_nExp2 )
		m_nExp2		-= static_cast<EXPINTEGER>( dwExp );
}

//
//
#ifndef __BEAST
void CMover::IncSkillExp( int nType, int nId )
{
	if( nId < 0 || nId > MAX_SKILL_JOB )
	{
		Error( "IncSkillExp : nId의 값이 범위초과.%d", nId );
		return;
	}

	LPSKILL pSkill = &m_aJobSkill[ nId ];

	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	BOOL bRet = GetSkillProp( &pSkillProp, &pAddSkillProp, pSkill->dwSkill, pSkill->dwLevel, "CMover::IncSkillExp" );
	if( bRet == FALSE )
		return;

	if( pSkill->bStop == FALSE )
		return;

//	m_nExp2 -= pAddSkillProp->dwExp;
	
	// 나의 렙에 맞쳐 최대로 올라갈수 있는 레벨이 정해진다. 방랑자스킬은 5가 최고입니다.
	// 전직 하고 나오는 스킬 렙 10이 최고
	if( m_nExp2 < static_cast<EXPINTEGER>( pAddSkillProp->dwExp ) )
		return;

	if( pSkill->dwLevel == pSkillProp->dwExpertMax )
		return;
	
	if( pSkill->dwLevel > pSkillProp->dwExpertMax ) // 렙업을 최대로 하여 렙업을 할수 없음.
	{
		pSkill->dwLevel = pSkillProp->dwExpertMax;
		pSkill->dwExp = 0;
#ifdef __WORLDSERVER
		if( IsPlayer() )
			( (CUser*)this )->AddSetSkillExp( nType, nId, m_nExp2, pSkill->dwExp );
#endif	// __WORLDSERVER
		return;
	}
		

	// 스킬의 dwReqDisLV를 해야 하는것이 아님

#ifndef __XPXP0303
	m_nExp2		-= static_cast<EXPINTEGER>( pAddSkillProp->dwExp );		// pxp깎는시점이 마법발사직전으로 바뀜.
#endif // not xPXP0303

#ifdef __SDEBUG
	pSkill->dwExp += prj.m_aExpSkill[ pSkill->dwLevel + 1 ] / 2;
#else // __SDEBUG

	DWORD dwSkillExp	= pAddSkillProp->dwSkillExp;
#ifdef __SM_ITEM_2ND
	if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_SKLINSTRUCT ) )
		dwSkillExp	*= 2;
#endif	// __SM_ITEM_2ND

	pSkill->dwExp += ( dwSkillExp * prj.m_fSkillExpRate );

#endif // __SDEBUG
	
#ifdef __WORLDSERVER
	if( IsPlayer() )
		( (CUser*)this )->AddSetSkillExp( nType, nId, m_nExp2, pSkill->dwExp );
#endif	// __WORLDSERVER

	// 최대치면 렙업 
	if( pSkill->dwExp >= prj.m_aExpSkill[ pSkill->dwLevel + 1 ] )
	{
		TRACE( "IncSkillExp : Skill LevelUp!\n" );
		pSkill->dwExp = 0;
		pSkill->dwLevel++;
#ifdef __WORLDSERVER
//		g_UserMng.AddSetSkillLevel( this, pSkill->dwSkill, pSkill->dwLevel );
#endif	// __WORLDSERVER
	}
}
#endif // __BEAST

BOOL CMover::AddExperience( EXPINTEGER nExp, BOOL bFirstCall )
{
	if( nExp <= 0 )		// nExp가 마이너스이거나 0이면 처리할필요 없음.
		return FALSE;

	if( m_nHitPoint <= 0 )
		return FALSE;

	if( bFirstCall )
	{
		nExp	*= GetExpFactor();
	}

#ifdef __INTERNALSERVER
	TRACE( "EXP = %I64d\n", nExp );
#endif	// __INTERNALSERVER

	if( IsBaseJob() )
	{
		if( m_nLevel >= MAX_JOB_LEVEL )
		{
			m_nExp1		= 0;
			return TRUE;
		}
	}
	else if( IsExpert() )
	{
		if( m_nLevel >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
		{
			return TRUE;
		}
	}
	else if( IsPro() )
	{
		if( m_nLevel >= MAX_LEVEL )
		{
			m_nExp1		= 0;
			return TRUE;
		}
	}
	

	int nLevelbuf = m_nLevel;
	int nNextLevel = m_nLevel + 1;
	
	EXPINTEGER nBufExp = m_nExp1;
	m_nExp2		+= nExp;
	if( m_nExp2 >= prj.m_aExpCharacter[m_nLevel].nExp2 )
	{
		m_nExp1		+= m_nExp2 - prj.m_aExpCharacter[m_nLevel].nExp2;
		m_nExp2		= prj.m_aExpCharacter[m_nLevel].nExp2;
	}

	if( m_nExp1 >= prj.m_aExpCharacter[nNextLevel].nExp1 )	// 레벨업
	{
		EXPINTEGER nExptmp;
		BOOL f	= FALSE;

		{
			m_nRemainGP += prj.m_aExpCharacter[ nNextLevel ].dwLPPoint;
			m_nRemainLP  = 0;

			nExptmp		= m_nExp1 - prj.m_aExpCharacter[nNextLevel].nExp1;
			m_nExp1		= m_nDeathExp		= 0;
			m_nLevel	= nNextLevel;

			if( m_nLevel > m_nDeathLevel )
				f	= TRUE;

			m_nHitPoint = GetMaxHitPoint();
			m_nManaPoint = GetMaxManaPoint();
			m_nFatiguePoint = GetMaxFatiguePoint();
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
			else if( IsPro() && m_nLevel > MAX_LEVEL )
			{
				m_nLevel = MAX_LEVEL;
				bLevelUp = FALSE;
			}

			if( bLevelUp )
			{
				if( IsBaseJob() )
				{
					m_aJobLv[ m_nJob ] = m_nLevel;
				}
				else
				{
					if( m_nLevel > MAX_EXPJOBLEVEL )
						m_aJobLv[ m_nJob ] = prj.m_aExpJobLevel[ MAX_EXPJOBLEVEL ];
					else
						m_aJobLv[ m_nJob ] = prj.m_aExpJobLevel[ m_nLevel ];
				}
			}
			else
			{
				m_nRemainGP -= prj.m_aExpCharacter[ nNextLevel ].dwLPPoint;
				m_nRemainLP  = 0;
				nExptmp	= m_nExp1	= 0;
			}
			

			// 레벨 20이 되는순간 비행레벨은 1이된다.
			if( m_nLevel == 20 )
			{
				m_nFlightLv = 1;
			}
			
#ifdef __CLIENT
			#ifdef __Y0216
				if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 )) )
					SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
			#endif // __Y0216
			CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
#ifdef __V041122_MUSIC
			PlayMusic( BGM_IN_LEVELUP );
#else
			PlayMusic( BGM_INSTANT03 );
#endif			

#endif	// __CLIENT
		}

#ifdef __WORLDSERVER
		AddItemEvent( f, nLevelbuf );
#endif // __WORLDSERVER
		if( nExptmp > 0 )
			AddExperience( nExptmp, FALSE );
		return TRUE;
	}

	return FALSE;
}

// 경험치를 nExp만큼 깎는다.  렙다운도 된다.
// bExp2Clear : pxp를 0으로 할건지 말건지.
// bLvDown : 레벨다운을 할건지 말건지.
BOOL CMover::DecExperience( EXPINTEGER nExp, BOOL bExp2Clear, BOOL bLvDown )
{
	if( nExp < 0 )	// 값은 항상 양수로만 줘야 한다.
		return FALSE;
	
	int nPrevLevel	= m_nLevel - 1;
	if( nPrevLevel < 1 )	// 레벨 1이하로는 더이상 내려가지 않음.
		return FALSE;

	m_nExp1		-= nExp;	// nExp만큼 경험치를 깎음. Exp2는 0%로.
	if( bExp2Clear ) 
		m_nExp2		= 0;	// pxp 클리어.
	if( m_nExp1 < 0 )
	{
		if( bLvDown )	// 렙다운이 되는 상황.
		{
			m_nExp1		= prj.m_aExpCharacter[nPrevLevel+1].nExp1 + m_nExp1;	// 앞레벨의 최대경험치 - 깍고 남은만큼 더깎음.
			m_nRemainGP -= 2;
			m_nRemainLP -= prj.m_aExpCharacter[ m_nLevel ].dwLPPoint;
			m_nLevel = nPrevLevel;	// 레벨도 다운.
			if( IsExpert() == FALSE )	// 아직 전직 안한상태.
			{
				m_aJobLv[ m_nJob ] = m_nLevel;	// 직업레벨(방랑자)은 곧 자기 레벨과 같다.
			}
			
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogLevelUp( this, 6 );
#endif // __WORLDSERVER
			return TRUE;	// 렙다운 됐으면 TRUE리턴
		}
		else
		{
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogLevelUp( this, 7 );
#endif // __WORLDSERVER
			m_nExp1		= 0;
		}
	}
	
	return FALSE;
}

// 현재 레벨의 최대경험치의 %로 경험치 하락
// ex) fPercent = 0.2f;		// 20%
BOOL CMover::DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown )
{
	EXPINTEGER	nExpOld		= m_nExp1;		// 깎이기전 exp
	EXPINTEGER	nMaxExp		= prj.m_aExpCharacter[m_nLevel+1].nExp1;	// 깎이기전 경험치치 맥스량.
	int	nOldLv	= m_nLevel;		// 깎이기전 레벨.

	if( fPercent == 0.0f )	
		return FALSE;	// 0%를 깎는 상황이면 계산할 필요 없다.

	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 * fPercent );	// 현재레벨의 최대경험치 * 퍼센트
	bLvDown		= DecExperience( nDecExp, bExp2Clear, bLvDown );

	if( bLvDown )	// 실제로 레벨이 다운되었을때.
	{
		if( m_nLevel - nOldLv < -1 )	// 레벨이 두단계 이상 다운되는 일은 없어야 한다.
			Message( "CMover::DecExperiencePercent : %s 레벨다운 이상. %d -> %d  %I64d(%f)"
			, m_szName, nOldLv, m_nLevel, nDecExp, fPercent );

		EXPINTEGER	nDecExp2	= ( prj.m_aExpCharacter[m_nLevel+1].nExp1 - m_nExp1 ) + nExpOld;	// 실제 깎인 포인트.
		if( nDecExp2 > nDecExp )	// 실제 깎인양이 원래 깎으려고 했던 양보다 많으면 경고.
			Message( "1 CMover::DecExperiencePercent : %s(Lv%d) 경험치가 이상하게 깎임. %I64d %I64d(%f) %I64d %I64d"
			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, m_nExp1 );
	}
	else
	{
		EXPFLOAT fOldPercent = static_cast<EXPFLOAT>( nExpOld ) / static_cast<EXPFLOAT>( nMaxExp );		// 깎이기전 퍼센트량.
		EXPFLOAT fCurPercent = static_cast<EXPFLOAT>( m_nExp1 ) / static_cast<EXPFLOAT>( nMaxExp );		// 깍은후 퍼센트량.
	#ifdef __NPP_050308
		const float fDelta = 0.050f;
	#else
		const float fDelta = 0.035f;
	#endif
		if( fOldPercent - fCurPercent > fDelta )	// 오차방지를 위해 0.5% 더 + 시켰다
			Message( "0 CMover::DecExperiencePercent : %s(Lv%d) 경험치가 이상하게 깎임. %I64d %I64d(%f) %I64d(%f) %I64d(%f)"
			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, fOldPercent, m_nExp1, fCurPercent );
	}

	return bLvDown;
}

// 비행 경험치 획득
BOOL CMover::AddFxp( int nFxp )
{
#ifdef __XFXP0521
	if( nFxp <= 0 )		// nExp가 마이너스이거나 0이면 처리할필요 없음.
		return FALSE;

	if( m_nHitPoint <= 0 )		// 죽고나선 경험치 획득 못함.
		return FALSE;

	if( m_nFlightLv == 0 )
		return FALSE;

	int nNextLevel = m_nFlightLv + 1;

	m_nFxp += nFxp;		// 일단 경험치를 더하고.
	if( m_nFxp >= prj.m_aFxpCharacter[nNextLevel].dwFxp ) // 레벨업이 되었는지 확인.
	{
		int		FxpBuf;
		FxpBuf = m_nFxp - prj.m_aFxpCharacter[ nNextLevel ].dwFxp;		// 레벨업하고도 얼마나 더 오버됐는지?
		m_nFxp = 0;
		m_nFlightLv = nNextLevel;		// 레벨 증가.
		
#ifdef __CLIENT
		// 레벨업시 이펙트
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
#ifdef __V041122_MUSIC
		PlayMusic( BGM_IN_LEVELUP );
#else
		PlayMusic( BGM_INSTANT03 );
#endif
#endif	// __CLIENT
	
		// 오바된 경험치는 한번더 돌려서 더해줌.
		if( FxpBuf > 0)
		{
			AddFxp( FxpBuf );
		}

		return TRUE;
	}
#endif // XFXP0521	
	return FALSE;
}

#ifndef __BEAST
BOOL CMover::AddChangeJob( int nJob )
{
	LPSKILL lpSkill;
	if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT ) // 1차 전직 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		m_aJobLv[ m_nJob ] = prj.m_aExpJobLevel[ m_nLevel ];
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + 3 ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		return TRUE;
	}
	if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL ) // 2차 전직 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		m_aJobLv[ m_nJob ] = prj.m_aExpJobLevel[ m_nLevel ];
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		return TRUE;
	}
	return FALSE;
}
#endif
BOOL CMover::SetFxp( int nFxp, int nFlightLv )
{
	m_nFxp = nFxp;

	// 레벨이 올랐으면.
	if( nFlightLv > m_nFlightLv )
	{
		m_nFlightLv = nFlightLv;
#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
#ifdef __V041122_MUSIC
		PlayMusic( BGM_IN_LEVELUP );
#else
		PlayMusic( BGM_INSTANT03 );
#endif
		g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
#endif // CLIENT
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::SetExperience( EXPINTEGER nExp1, EXPINTEGER nExp2, int nLevel )
{
	m_nExp1		= nExp1;
	m_nExp2		= nExp2;

	if( IsInvalidObj(this) )
		return 0;

	if( nLevel > m_nLevel )
	{
#ifdef __CLIENT

		PutLvUpSkillName_1(nLevel);

#ifdef __Y0216
		if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 ) ) )
			SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
#endif // __Y0216
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
#ifdef __V041122_MUSIC
		PlayMusic( BGM_IN_LEVELUP );
#else
		PlayMusic( BGM_INSTANT03 );
#endif
		g_WndMng.PutString( prj.GetText( TID_GAME_LEVELUP ), NULL, prj.GetTextColor( TID_GAME_LEVELUP ) );
		
		
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
			m_nFlightLv = 1;
		}
		else if( nLevel < 20 )
			m_nFlightLv = 0;	

#endif	// __CLIENT
		m_nLevel	= nLevel;
		if( m_nLevel < 16)
		{
			m_aJobLv[ JOB_VAGRANT ] = (int)m_nLevel;
		}
		else
		{
			if( m_nLevel > MAX_EXPJOBLEVEL )
				m_aJobLv[ m_nJob ] = prj.m_aExpJobLevel[ MAX_EXPJOBLEVEL ];
			else
				m_aJobLv[ m_nJob ] = prj.m_aExpJobLevel[ m_nLevel ];
		}

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = GetMaxManaPoint();
		m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __CLIENT
#ifdef __YGUIDESYSTEM0923
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
				if( GetLevel() != 1 )
					pWndWorld->m_pWndGuideSystem->GuideStart(FALSE);

				switch(GetLevel())
				{
				case 2:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER);
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
				case 49:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif //__YGUIDESYSTEM0923
		
#endif //__CLIENT		
		return TRUE;
	}
	else if( nLevel < m_nLevel )	// 레벨 다운이 되었을때.
	{
		m_nLevel	= nLevel;
		if( nLevel < 20 )
			m_nFlightLv = 0;

#ifdef __CLIENT
#ifdef __YGUIDESYSTEM0923
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
				if( GetLevel() != 1 )
					pWndWorld->m_pWndGuideSystem->GuideStart(FALSE);
				
				switch(GetLevel())
				{
				case 2:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER);
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
				case 49:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif //__YGUIDESYSTEM0923
#endif	// __CLIENT
	}


	return FALSE;
}


void CMover::SetEquipDstParam()
{
	if( IsPlayer() ) 
	{
		CItemElem* pItemElem;
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
		CItemElem* pItemElem;
		ItemProp *pItemProp;

		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			//if(pItem && !pItem->IsBreakableItem() && (pItem->m_nHitPoint == 1 || pItem->m_nHitPoint == 0))
			//	continue; IK2_ARMOR
			// IK_ARMOR를 이제 체크하지 않으므로 머리나 팔찌등에 디펜스값이 들어가지 않도록 주의한다.
			// 무기의 어빌리티는 디펜스에 포함되지 않는다
			//if( pItemElem && pItemElem->GetProp()->dwItemKind2 < IK2_WEAPON_DIRECT && pItemElem->GetProp()->dwItemKind2 > IK2_WEAPON_GUN )  //&& ( pItemElem->m_nHitPoint > 1 || pItemElem->m_nHitPoint == -1) )
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();

				if( pItemProp == NULL )
				{
					ERROR( "Parts Not Property = %d", i );
				}

				if( pItemProp )
				{
					if( pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC )
					{
						int nOption = pItemElem->GetAbilityOption();
					#ifdef __BALANCE_050614	
						int nOptionVal = 0;
						if( nOption > 0 ) 
							nOptionVal = pow( nOption, 1.5 );
					#else
						int nOptionVal = prj.GetExpUpItem( pItemProp->dwItemKind3, nOption );
					#endif
						if( pItemElem->m_nRepairNumber >= 100 )
							nOptionVal	+= prj.GetExpUpItem( pItemProp->dwItemKind3, 1 );

						float f		= GetItemMultiplier( pItemElem );
						nMin	+= (int)( pItemProp->dwAbilityMin * f ) + nOptionVal;
						nMax	+= (int)( pItemProp->dwAbilityMax * f ) + nOptionVal;
					}
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

	CItemElem* pItemElem;
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
#ifndef __BEAST

#ifdef __ACROBAT_0504
// 안보이는 상태
BOOL CMover::SetDarkCover( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_INVISIBILITY )	// 투명화에 면역이라....그런것도 있나?
			return FALSE;	
		SetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, NULL_CHGPARAM );
		m_wDarkCover = (tmMaxTime / 1000.0f) * PROCESS_COUNT;	
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
#endif //__ACROBAT_0504

// 스턴상태로 만들기
BOOL CMover::SetStun( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetProp()->dwClass == RANK_SUPER )
			return FALSE;			// 슈퍼몹은 스턴에 안걸린다. 이렇게 하지말고 스턴에 이뮨이 되게 설정하는게 좋다.

		if( GetAdjParam( DST_IMMUNITY ) & CHS_STUN ) 
			return FALSE;		// 독에 면역상태다.

		SetDestParam( DST_CHRSTATE, CHS_STUN, NULL_CHGPARAM, FALSE );
		SendActMsg( OBJMSG_STUN );		// 스턴 상태 시작.
		m_wStunCnt = (tmMaxTime / 1000.0f) * PROCESS_COUNT;		// 얼마동안이나 스턴상태가 되느냐.
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )	// 이거 검사안하면 매번 클라로 ResetDestParam을 날린다.
			ResetDestParam( DST_CHRSTATE, CHS_STUN, FALSE);	// 스턴 상태 해제.
		m_wStunCnt = 0;
		m_dwFlag &= (~MVRF_NOACTION);				// 액션 금지 상태로 전환.
	}

#ifdef __WORLDSERVER
	g_UserMng.AddSetStun( this, tmMaxTime );
 #ifdef __XAI1013
	if( bApply )
		g_UserMng.AddDlgEmoticon( this, DLGEMOT_STUN );
 #endif
#endif // Worldserver

	return TRUE;
}
#endif
#ifndef __BEAST

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
			Message( "SetPoison : %s 독효과의 지속시간이 지정되지 않음", GetName() );
		if( tmUnit == -1 )
			Message( "SetPoison : %s 독효과의 tick 시간이 지정되지 않음", GetName() );
		if( wDamage == -1 )
			Message( "SetPoison : %s 독효과의 틱당 데미지가 지정되지 않음", GetName() );
		
		m_wPoisonCnt = (tmMaxTime / 1000.0f) * PROCESS_COUNT;			// 1/1000
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
		m_wDarkCnt = (tmMaxTime / 1000.0f) * PROCESS_COUNT;	
		if( nAdjHitRate == -1 )
			Message( "SetDark : %s 어둠효과의 명중률 수정치가 지정되지 않음", GetName() );
		m_wDarkVal = nAdjHitRate;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_DARK )
		{
			ResetDestParam( DST_CHRSTATE, CHS_DARK, TRUE );		// 암흑상태해제
			ResetDestParam( DST_ADJ_HITRATE, m_wDarkVal, TRUE );	// 암흑동안 수정됐던 명중률 해제
//			if( m_wDarkCnt >= 0 )
//				Message( "SetDark : %d", m_wDarkVal );
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
			Message( "SetBleeding : %s 출혈효과의 지속시간이 지정되지 않음", GetName() );
		if( tmUnit == -1 )
			Message( "SetBleeding : %s 출혈효과의 tick 시간이 지정되지 않음", GetName() );
		if( wDamage == -1 )
			Message( "SetBleeding : %s 출혈효과의 틱당 데미지가 지정되지 않음", GetName() );
		
		m_wBleedingCnt = (tmMaxTime / 1000.0f) * PROCESS_COUNT;			// 1/1000
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
#endif // not Beast

// dwState를 해제함
// CHS_ALL을 하면 모든 디버프.
void CMover::RemoveDebuff( DWORD dwState )
{
#ifndef __BEAST	
	// 스킬로 걸린건 이걸로 해제해야하고
	m_SkillState.RemoveSkillInfluenceState( dwState );	// 상태이상을 해제함.
	// 스킬이 아닌 아이템같은걸로 상태만 걸린건 이걸로 풀어야 한다.
	if( dwState & CHS_DARK )
		SetDark( FALSE );
	if( dwState & CHS_POISON )
		SetPoison( FALSE );
	if( dwState & CHS_STUN )
		SetStun( FALSE );
	// 이거 안에서 Reset을 호출한다.
	SetDestParam( DST_CURECHR, dwState, NULL_CHGPARAM );
#endif //__BEAST
}


///////////////////////////////////////////////////////////////////////////////
// 
// 독, 암흑등 추가능력치의 세팅은 이걸로 하자.
void CMover::SetDestParam( int nIdx, ItemProp *pProp, BOOL bSend )
{

	SetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], pProp->dwChgParamVal[nIdx], bSend );

#ifndef __BEAST
	// 위에서 다 처리못한걸 여기서 추가 처리.
 #ifndef __XSKILL0425		
	//{{AFX
	switch( pProp->dwDestParam[nIdx] )
	{
	case DST_CHR_CHANCEPOISON:
		m_tmPoisonUnit  = pProp->nDestData1[0];		// 몇초마다 데미지 먹냐?
		m_wPoisonDamage = pProp->nDestData1[1];		// 독데미지량
		break;
	case DST_CHR_CHANCEDARK:
		m_wDarkVal = pProp->nDestData1[0];			// 암흑동안 깎일 명중률 수정치.
		break;
	}
	//}}AFX
 #endif // not xSkill0425
#endif // not Beast
}

void CMover::ResetDestParam( int nIdx, ItemProp *pProp, BOOL bSend )
{
	//
	ResetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], bSend );
#ifndef __BEAST
 #ifndef __XSKILL0425
	// 위에서 다 처리못한걸 여기서 추가 처리.
	//{{AFX
	switch( pProp->dwDestParam[nIdx] )
	{
	case DST_CHR_CHANCEPOISON:
		m_tmPoisonUnit  = 0;		// 몇초마다 데미지 먹냐?
		m_wPoisonDamage = 0;		// 독데미지량
		break;
	case DST_CHR_CHANCEDARK:
		m_wDarkVal = 0;			// 암흑동안 깎일 명중률 수정치.
		break;
	#ifdef __ACROBAT_0504
	case DST_CHR_CHANCEBLEEDING:
		m_tmBleedingUnit  = 0;		// 몇초마다 데미지 먹냐?
		m_wBleedingDamage = 0;		// 독데미지량
		break;
	#endif //__ACROBAT_0504
	}
	//}}AFX
 #endif // not xSkill0425
#endif
}

// 목표 파라미터를 셋한다.
void CMover::SetDestParam( int nItemIdx, BOOL bSend )
{
	ItemProp* pItemProp = prj.GetItemProp( nItemIdx );
	if( pItemProp->dwDestParam1 != -1 )
		SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, bSend );
	if( pItemProp->dwDestParam2 != -1 )
		SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, bSend );
}

//--------------------------------------------------------------------------
//
// 목표 파라메타를 셋한다.
void CMover::SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL bSend )
{
#ifndef __BEAST
	BOOL fSuccess	= FALSE;
	if( nDstParameter < 0 )
		return;
	
	if( nDstParameter == DST_HP_MAX )
	{
		int a = 0;
	}

	// HP, MP등은 SetPointParam으로 처리한다. 근디 SetDestParam 하나로 통일시켰으면 낫지 않을까?
	switch( nDstParameter )
	{
#ifdef __WORLDSERVER
	case DST_HP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxHitPoint() );
			else
				SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );
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
		if( AddGold( nAdjParameterValue ) == FALSE )	
			SetGold( 0 );		// underflow이면 0로 만든다. 이전코드와 호환되게 한다. 뭔가 찜찜 
		return;

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
#ifdef __ITEMBUFF0929
	case DST_LOCOMOTION:
		SetDestParam( DST_SPEED,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), nChgParameterValue, bSend );
		return;
#endif	// __ITEMBUFF0929
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		SetDestParam( DST_MASTRY_EARTH,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_FIRE,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_WATER,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_WIND,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
#endif	// __NEWWPN1024
/*
	case DST_ADJDEF_RATE:		// 현재 defense의 %율을 DST_ADJDEF 로 조절.
		{
			ATTACK_INFO info;
			ZeroMemory( &info, sizeof(info) );		// info의 내용을 사용하지 않지만 포인터로 참조를 하니까 
			info.dwAtkFlags = AF_GENERIC;

			int nDefense = CalcDefense( &info, FALSE );		// 물리 방어력 구함.
			nDefense = (int)(nDefense * (nAdjParameterValue / 100.0f));		// Adj퍼센트를 구함
			SetDestParam( DST_ADJDEF, nDefense, NULL_CHGPARAM, bSend );
		}
		return;
*/
#ifdef __XITEMBUFF
	case DST_HP_MAX_RATE:
		{
			int nOriginMaxHP = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 HP
			int nAddHP = (nOriginMaxHP * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_HP_MAX, nAddHP, NULL_CHGPARAM, bSend );	// HPMAX치를 증가시킴.
		}
		return;
	case DST_MP_MAX_RATE:
		{
			int nOriginMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nAdd = (nOriginMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_MP_MAX, nAdd, NULL_CHGPARAM, bSend );	// MAX치를 증가시킴.
		}
		return;
	case DST_FP_MAX_RATE:
		{
			int nOriginMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nAdd = (nOriginMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_FP_MAX, nAdd, NULL_CHGPARAM, bSend );	// MAX치를 증가시킴.
		}
		return;
#endif // xItemBuff
#ifdef __S_DST_ADD
	case DST_HP_RECOVERY_RATE:
		{
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
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
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// MP
			nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// FP
			nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
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
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
			
			// MP
			nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
			
			// FP
			nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
#endif // __S_DST_ADD
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
					Message( "SetDestParam : Adj == -1, %s", GetName() );
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
 #ifdef __ACROBAT_0504
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
 #endif //__ACROBAT_0504
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCESTUN:
		case DST_AUTOHP:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 찬스 확률.	// %미만으로 회복되게 할꺼냐.
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// 찬스가 걸렸을때 지속시간		// %HP가 회복될꺼냐.
			break;
		default:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 누적
			break;
		}
	}

	else if( nChgParameterValue != 0x7FFFFFFF ) 
	{
		fSuccess	= TRUE;
		m_chgParamAry[nDstParameter] = nChgParameterValue;	// 고정된 값.
	}
#ifdef __WORLDSERVER
	if( TRUE == ( fSuccess & bSend ) ) {
		g_UserMng.AddSetDestParam( this, nDstParameter, nAdjParameterValue, nChgParameterValue );
	}
#endif	// __WORLDSERVER
#endif
}

//
// 목표 파라메타를 리셋한다.
//
void CMover::ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend )
{
#ifndef __BEAST
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
#ifdef __ITEMBUFF0929
	case DST_LOCOMOTION:
		ResetDestParam( DST_SPEED,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), TRUE );
		return;
#endif	// __ITEMBUFF0929
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		ResetDestParam( DST_MASTRY_EARTH,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_FIRE,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_WATER,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_WIND,	nAdjParameterValue, TRUE );
		return;
#endif	// __NEWWPN1024
/*
	case DST_ADJDEF_RATE:		// 현재 defense의 %율을 DST_ADJDEF 로 조절.
		{
			ATTACK_INFO info;
			ZeroMemory( &info, sizeof(info) );		// info의 내용을 사용하지 않지만 포인터로 참조를 하니까 
			info.dwAtkFlags = AF_GENERIC;

			int nDefense = CalcDefense( &info, FALSE );		// 물리 방어력 구함.
			nDefense = (int)(nDefense * (nAdjParameterValue / 100.0f));		// Adj퍼센트를 구함
			ResetDestParam( DST_ADJDEF, nDefense, TRUE );
		}
		return;
*/
#ifdef __XITEMBUFF
	case DST_HP_MAX_RATE:
		{
			int nOriginMaxHP = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 HP
			int nAddHP = (nOriginMaxHP * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_HP_MAX, nAddHP, TRUE );	// 올렸던 값을 리셋시킴.
		}
		return;
	case DST_MP_MAX_RATE:
		{
			int nOriginMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nAdd = (nOriginMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_MP_MAX, nAdd, TRUE );	// 
		}
		return;
	case DST_FP_MAX_RATE:
		{
			int nOriginMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nAdd = (nOriginMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_FP_MAX, nAdd, TRUE );	// MAX치를 증가시킴.
		}
		return;
#endif // xItemBuff
#ifdef __S_DST_ADD
	case DST_HP_RECOVERY_RATE:
		{
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
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
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
			
			// MP
			nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
			
			// FP
			nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
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
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax = GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
			// MP
			nMax = GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
			// FP
			nMax = GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv = (nMax * (nAdjParameterValue / 100.0f));
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, TRUE );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, TRUE );
		}
#endif // __S_DST_ADD
	}


	// 이 검사를 왜 위에다 안넣고 여기다 넣었냐면
	// DST_GOLD, PXP, RESIST_ALL같은경우는 MAX_ADJPARAMARY보다 더 큰값을 사용한다. 
	// 그래서 그런것들은 위에서 처리하고 바로 리턴하고
	// 일반적인것들만 이쪽으로 온다.
	
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		Error( "ResetDestParam %s : 잘못된 값 nDstParam : %d", m_szName, nDstParameter );
		return;
	}
	if( nAdjParameterValue != 0 ) 
	{
		if( nDstParameter == DST_CHRSTATE )
			m_adjParamAry[nDstParameter] &= (~nAdjParameterValue);		// 해당비트 오프.
		else
			m_adjParamAry[nDstParameter] += (-nAdjParameterValue);
	}
	m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
#ifdef __WORLDSERVER
	if( fSend == TRUE ) {
		g_UserMng.AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	}
#endif	// __WORLDSERVER
#endif // __BEAST
}

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
	ERROR( "CMover::GetHitPoint()" );

	return GetParam( DST_HP, m_nHitPoint );
}
// 마나 포인트 얻기 
int CMover::GetManaPoint()
{
#ifdef __EVENT1206
	if( g_eLocal.GetState( EVE_EVENT1206 ) )
		return GetMaxManaPoint();
#endif	// __EVENT1206
	return GetParam( DST_MP, m_nManaPoint );
}
// 피지칼 포인트 얻기
int CMover::GetFatiguePoint()
{
#ifdef __EVENT1206
	if( g_eLocal.GetState( EVE_EVENT1206 ) )
		return GetMaxFatiguePoint();
#endif	// __EVENT1206
	return GetParam( DST_FP, m_nFatiguePoint );
}

// 최대 히트 포인트 얻기 
int CMover::GetMaxHitPoint()
{
	if( IsInvalidObj(this) )
	{
		return 0;
	}

	return GetParam( DST_HP_MAX, GetMaxOriginHitPoint( FALSE ) ); 
}

// 최대 마나 포인트 얻기 
int CMover::GetMaxManaPoint()
{
	return GetParam( DST_MP_MAX, GetMaxOriginManaPoint( FALSE ) );
}

// 최대 피로 포인트 얻기
int CMover::GetMaxFatiguePoint()
{
	return GetParam( DST_FP_MAX, GetMaxOriginFatiguePoint( FALSE ) );
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
	{
		return 0;
	}

	if( IsPlayer() )
	{
		int nSta, nStr;
		if( bOriginal )
		{
			nSta = m_nSta;
			nStr = m_nStr;
		}
		else
		{
			nSta = GetSta();
			nStr = GetStr();
		}

		JobProp* pProperty = prj.GetJobProp( GetJob() ); 

		float a = (pProperty->fFactorMaxHP*m_nLevel)/2.0f;
		float b = a * ((m_nLevel+1.0f)/4.0f) * (1.0f + nSta/50.0f) + (nSta*10.0f) ;
		float maxHP = b + 80.f; 

		return (int)maxHP;
	}
	else
	{
		MoverProp *pMoverProp = GetProp();
#ifdef __S1108_BACK_END_SYSTEM
		return int( pMoverProp->dwAddHp * prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate ); 
#else // __S1108_BACK_END_SYSTEM
		return pMoverProp->dwAddHp; 
#endif // __S1108_BACK_END_SYSTEM
	}

	return 0;
}

int CMover::GetMaxOriginManaPoint( BOOL bOriginal )
{
	int nInt = 0;
	if( bOriginal )
		nInt = m_nInt;
	else
		nInt = GetInt();

	if( IsPlayer() )
	{
		// INT((((BaseLv*2) + (INT*8))*Job계수) + 22)+(INT*Job계수)) +  ① 
		// ① : 아이템에의한 추가 상승 (%단위) TDDO
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxMP;
		
		int nMaxMP = ((((m_nLevel*2.0f) + ( nInt*8.0f))*factor) + 22.0f)+( nInt*factor);
		return nMaxMP;
	}
	return 	( ( m_nLevel * 2 ) + ( nInt * 8 ) + 22 );
}

int CMover::GetMaxOriginFatiguePoint( BOOL bOriginal )
{
	int nSta, nStr, nDex;
	if( bOriginal )
	{
		nSta = m_nSta;
		nStr = m_nStr;
		nDex = m_nDex;
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

		int nMaxFP = (((m_nLevel*2.0f) + (nSta*6.0f))*factor) +(nSta*factor);
		return nMaxFP;
	}
	
	return ( ( m_nLevel * 2 ) + ( nStr * 7 ) + ( nSta *2 ) + ( nDex * 4 ) );
}

int CMover::GetHitPointPercent( int nPercent )
{
	int nMax = GetMaxHitPoint();
	if( nMax == 0 ) return 0;
//	return GetHitPoint() * nPercent / nMax;
	return MulDiv( GetHitPoint(), nPercent, nMax );
}
int CMover::GetManaPointPercent( int nPercent )
{
	int nMax = GetMaxManaPoint();
	if( nMax == 0 ) return 0;
//	return GetManaPoint() * nPercent / GetMaxManaPoint();
	return MulDiv( GetManaPoint(), nPercent, nMax );
}
int CMover::GetFatiguePointPercent( int nPercent )
{
	int nMax = GetMaxFatiguePoint();
	if( nMax == 0 ) return 0;
//	return GetFatiguePoint() * nPercent / GetMaxFatiguePoint();
	return MulDiv( GetFatiguePoint(),  nPercent, nMax );
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
	int nValue = (GetLevel() / 3.0f) + (GetMaxHitPoint() / (500.f * GetLevel())) + (GetSta() * fFactor);
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
	int nValue = ((GetLevel() * 1.5f) + (GetMaxManaPoint() / (500.f * GetLevel())) + (GetInt() * fFactor)) * 0.2f;
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
	int nValue = ((GetLevel() * 2.0f) + (GetMaxFatiguePoint() / (500.f * GetLevel())) + (GetSta() * fFactor)) * 0.2f;
	return GetParam( DST_FP_RECOVERY, nValue );
}

#ifdef __CLIENT
// 명성 이름을 스트링으로 돌려준다.
LPCTSTR CMover::GetFameName( void )
{
	switch( GetJob() )
	{
#ifdef __ACROBAT_0504
	// 아크로뱃
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
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
#endif //__ACROBAT_0504

	// 머서너리일때
	case JOB_MERCENARY:	
	case JOB_KNIGHT:	
	case JOB_BLADE:

#ifdef __NPP_050308	
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
#else //__NPP_050308		
		if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MER_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MER_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MER_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MER_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MER_FAME01 );
#endif //__NPP_050308
		break;
	// 매지션
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:	
	case JOB_ELEMENTOR:

#ifdef __NPP_050308	
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
#else //__NPP_050308		
		if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MAG_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MAG_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MAG_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MAG_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MAG_FAME01 );
#endif //__NPP_050308
		break;
	// 어시스트
	case JOB_ASSIST:
	case JOB_BILLPOSTER:	
	case JOB_RINGMASTER:
		
#ifdef __NPP_050308	
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
#else //__NPP_050308		
		if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ASS_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ASS_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ASS_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ASS_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ASS_FAME01 );
#endif //__NPP_050308
		break;
	}
	return "";
}

// 슬로터 이름을 스트링으로 돌려준다.
LPCTSTR CMover::GetSlaughterName( void )
{
#ifdef __NPP_050308
	KarmaProp* pProp = prj.GetKarmaProp( m_nSlaughter );

	if( pProp )
	{
		if(strlen(pProp->szName) > 0 )
			return pProp->szName;
	}	
#else //__NPP_050308
	if( IsChaotic() == FALSE )		return "";

	// 슬로터 포인트가 있으면 킬수에 따라서 색깔 달리.
	if( m_nNumKill >= 1000 )	return "사신";
	else if( m_nNumKill >= 100 )	return "사안";
	else if( m_nNumKill >= 3 )	return "살인광";
	else if( m_nNumKill == 2 )	return "살인마";
	else if( m_nNumKill == 1 )	return "살인자";
#endif //__NPP_050308

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
		Error( "CMover::GetReistSpell : %d 프로퍼티 읽기 실패", GetName() );

	int		nResist = 0;

	switch( nDestParam )
	{
	case DST_RESIST_ELECTRICITY:	nResist = pProp->nResistElecricity;		break;
	case DST_RESIST_FIRE:			nResist = pProp->nResistFire;		break;
	case DST_RESIST_WATER:			nResist = pProp->nResistWater;		break;
	case DST_RESIST_EARTH:			nResist = pProp->nResistEarth;		break;
	case DST_RESIST_WIND:			nResist = pProp->nResistWind;		break;
	default:
		Error( "CMover::GetResistSpell : %s 파라메터 잘못됨 %d", GetName(), nDestParam );
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
	int nResult = m_nStr + GetParam( DST_STR, 0 );
#ifdef __NPP_050308
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_STR_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);

		if( pKarmaProp )
		{
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
		}
	}
#endif //__NPP_050308
	return nResult;
}
int CMover::GetDex()
{
	int nResult = m_nDex + GetParam( DST_DEX, 0 );
#ifdef __NPP_050308
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_DEX_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		
		if( pKarmaProp )
		{
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
		}
	}
#endif //__NPP_050308
	return nResult;
}
int CMover::GetInt()
{
	int nResult = m_nInt + GetParam( DST_INT, 0 );
#ifdef __NPP_050308
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_INT_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		
		if( pKarmaProp )
		{
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
		}
	}
#endif //__NPP_050308
	return nResult;
}
int CMover::GetSta()
{
	int nResult = m_nSta + GetParam( DST_STA, 0 );
#ifdef __NPP_050308
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_STA_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		
		if( pKarmaProp )
		{
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
		}
	}
#endif //__NPP_050308
	return nResult;
}

FLOAT CMover::GetSpeed(FLOAT fSrcSpeed)
{
	if( m_fCrrSpd != 0.0f )
		return m_fCrrSpd;
	int nChgValue	= GetChgParam( DST_SPEED );
	if( nChgValue != 0x7FFFFFFF )
		return fSrcSpeed * (nChgValue / 100.0f);		// ex) fSrcSpeed * 0.5f
	int nAdjValue	= GetAdjParam( DST_SPEED );
	if( nAdjValue != 0 )
		return fSrcSpeed + ( fSrcSpeed * (nAdjValue / 100.0f) );
	return fSrcSpeed;
}




//
// 현재 동작상태를 강제로 세팅
// 동기화 이외는 사용하지 말것.
//

// 행위 명령은 강제 동기화 하고 이동 명령은 무시한다.
void CMover::BehaviorActionForceSet( void )
{
#ifndef __BEAST
	if( m_CorrAction.fValid && m_CorrAction.fBehavior == TRUE ) {
		if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
			ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f,
								m_CorrAction.fAngleX, m_CorrAction.fAccPower, m_CorrAction.fTurnAngle,
								m_CorrAction.dwState, m_CorrAction.dwStateFlag,
								m_CorrAction.dwMotion, m_CorrAction.nMotionEx, m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
		else {
			ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f,
								m_CorrAction.dwState, m_CorrAction.dwStateFlag,
								m_CorrAction.dwMotion, m_CorrAction.nMotionEx, m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
	}
	memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
#endif
}

void CMover::ActionForceSet( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
							 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption )
{
#ifndef __BEAST
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	CModelObject* pModel = (CModelObject*)m_pModel;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	m_pActMover->ForcedSet( vDelta, dwState, dwStateFlag, nMotionEx );
	m_dwMotion = -1;
	SetMotion( nMotion, nLoop, dwMotionOption );
	pModel->SetMotionBlending( FALSE );
#endif
}

void CMover::ActionForceSet2( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
							 FLOAT fAngleX, FLOAT fAccPower, FLOAT fTurnAngle,
							 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption )
{
#ifndef __BEAST
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	CModelObject* pModel = (CModelObject*)m_pModel;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	SetAngleX( fAngleX );
	m_pActMover->ForcedSet2( vDelta, fAccPower, fTurnAngle, dwState, dwStateFlag, nMotionEx );
	m_dwMotion = -1;
#ifdef __WORLDSERVER	
	if( nMotion == 31 && (dwMotionOption & MOP_FIXED) == 0 )
		dwMotionOption |= MOP_FIXED;	
#endif

	SetMotion( nMotion, nLoop, dwMotionOption );
	pModel->SetMotionBlending( FALSE );
#endif
}

#ifdef __J0518
void CMover::HalfForceSet( D3DXVECTOR3 & vPos, D3DXVECTOR3 & vd, float fAngle, float fAngleX, float fAccPower, float fTurnAngle )
{
#ifndef __BEAST
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	SetAngleX( fAngleX );
	m_pActMover->HalfForcedSet( vd, fAccPower, fTurnAngle );
#endif	// __BEAST
}
#endif	// __J0518

#ifdef  __V050322_CACHEQUEST
BOOL CMover::IsCompleteQuest( int nQuestId )
{
	for( int i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			return TRUE;
		}
	}
	return FALSE;
}
void CMover::__MakeCompleteQuest( int nQuestId, LPQUEST lpQuest )
{
	ZeroMemory( lpQuest, sizeof( QUEST ) );
	lpQuest->m_wId = nQuestId;
	lpQuest->m_nState = QS_END;
}

BOOL CMover::MakeCompleteQuest( int nQuestId, LPQUEST lpQuest )
{
	for( int i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			__MakeCompleteQuest( nQuestId, lpQuest );
			return TRUE;
		}
	}
	return FALSE;
}
void CMover::RemoveAllQuest()
{
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
}
void CMover::RemoveCompleteQuest()
{
	m_nCompleteQuestSize = 0;
}

#endif // __V050322_CACHEQUEST
LPQUEST CMover::FindQuest( int nQuestId )
{
#ifdef  __V050322_CACHEQUEST
	/*
	for( int j = 0; j < m_apCacheQuest.GetSize(); j++ )
	{
		if( ((LPQUEST)m_apCacheQuest.GetAt( j ))->m_wId == nQuestId )
		{
			return (LPQUEST)m_apCacheQuest.GetAt( j );
		}
	}
	*/
	for( int i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			return &m_aQuest[ i ];
		}
	}
#else
	if( m_pCacheQuest != NULL && m_pCacheQuest->m_wId == nQuestId )
		return m_pCacheQuest;
	for( int i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			m_pCacheQuest = &m_aQuest[ i ];
			return m_pCacheQuest;
		}
	}
#endif
	return NULL;
}
BOOL CMover::RemoveQuest( int nQuestId )
{
#ifdef  __V050322_CACHEQUEST
	QuestProp* pProp = prj.m_aPropQuest.GetAt( nQuestId );
	//if( pProp && pProp->m_bNoRemove == TRUE )
	//	return FALSE;
	for( int i = 0; i < m_nQuestSize; i++ )
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
	for( i = 0; i < m_nCompleteQuestSize; i++ )
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
#else // __V050322_CACHEQUEST
		for( int i = 0; i < m_nQuestSize; i++ )
		{
			if( m_aQuest[ i ].m_wId == nQuestId )
			{
				for( ; i < m_nQuestSize - 1; i++ )
				{
					m_aQuest[ i ] = m_aQuest[ i + 1 ];
				}
				m_nQuestSize--;
				m_pCacheQuest = NULL;
				break;
			}
		}
#endif


#ifdef __CLIENT
#ifdef __V041122_MUSIC
	PlayMusic( BGM_EV_END );
#else
	PlayMusic( BGM_EVENT04 );
#endif
#endif
	return TRUE;
}
#ifdef __V050322_CACHEQUEST
BOOL CMover::SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest )
{
	QUEST newQuest;
	ZeroMemory( &newQuest, sizeof( QUEST ) );
	newQuest.m_wId = nQuestId;
	newQuest.m_nState = nState;

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
		if( m_nQuestSize >= MAX_QUEST )
		{
#ifndef __BEAST
			DEBUGOUT("SetQuestCnt : 퀘스트 초과");
#endif
			return NULL;
		}
#ifdef __CLIENT
#ifdef __V041122_MUSIC
		PlayMusic( BGM_EV_START );
#else
		PlayMusic( BGM_EVENT03 );
#endif
#endif
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
			lpQuest = &m_aQuest[ m_nQuestSize++ ];
		else
			return FALSE;

		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
		if( pQuestProp && pQuestProp->m_nEndCondLimitTime  )
			lpNewQuest->m_wTime = pQuestProp->m_nEndCondLimitTime;
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
			for( int i = 0; i < m_nQuestSize; i++ )
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
			QuestProp* pProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
			// 퀘스트 타입이 의뢰소라면 완료 배열에 들어가지 않는다. 
			//if( pProp->m_nQuestType != QT_REQUEST && pProp->m_bRepeat == FALSE )
			if( pProp && pProp->m_bRepeat == FALSE )
				m_aCompleteQuest[ m_nCompleteQuestSize++ ] = lpNewQuest->m_wId; 
			lpQuest = NULL;
		}
		else
			return FALSE;
	}
	if( lpQuest )
		memcpy( lpQuest, lpNewQuest, sizeof( QUEST ) );

#ifdef __CLIENT
	if( lpNewQuest->m_nState == QS_END )
#ifdef __V041122_MUSIC
		PlayMusic( BGM_EV_END );
#else
		PlayMusic( BGM_EVENT04 );
#endif
#endif
	return TRUE;
}
#else // __V050322_CACHEQUEST
//{{AFX
void CMover::SetQuest( int nQuestId, int nState, int nTime )
{
	LPQUEST lpQuest = FindQuest( nQuestId );
	// 퀘스트를 발견 못했으면 새로 추가한다.
	if( lpQuest == NULL )
	{
		if( m_nQuestSize >= MAX_QUEST )
		{
#ifndef __BEAST
			DEBUGOUT("SetQuestCnt : 퀘스트 초과");
#endif
			return;
		}
#ifdef __CLIENT
#ifdef __V041122_MUSIC
		PlayMusic( BGM_EV_START );
#else
		PlayMusic( BGM_EVENT03 );
#endif
#endif
		lpQuest = &m_aQuest[ m_nQuestSize ];
		m_nQuestSize++;
#ifdef  __V050322_CACHEQUEST
		ZeroMemory( lpQuest, sizeof( QUEST ) );
		m_apCacheQuest.Add( lpQuest );
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
		if( pQuestProp && pQuestProp->m_nEndCondLimitTime  )
		{
			lpQuest->m_wTime = pQuestProp->m_nEndCondLimitTime;
		}
#endif
	}
#ifdef  __V050322_CACHEQUEST
	else
	// 퀘스트 완료다. 그렇다면 캐쉬에서 뒤져서 제거하자 
	if( nState == QS_END )
	{
		for( int i = 0; i < m_apCacheQuest.GetSize(); i++ )
		{
			if( m_apCacheQuest.GetAt( i ) == lpQuest )
			{
				m_apCacheQuest.RemoveAt( i );
				break;
			}
		}
	}
	else
	// 현재 상태가 QS_END인데, nState가 QS_END가 아니면 이상 현상
	if( lpQuest->m_nState == QS_END )
	{
		
	}
#endif
	lpQuest->m_nState = nState;
	lpQuest->m_wId = nQuestId;
#ifdef __V050322_CACHEQUEST
#else
	lpQuest->m_wTime = nTime;
#endif

#ifdef __CLIENT
	if( nState == QS_END )
#ifdef __V041122_MUSIC
		PlayMusic( BGM_EV_END );
#else
		PlayMusic( BGM_EVENT04 );
#endif
#endif
}
//}}AFX
#endif // __V050322_CACHEQUEST
LPQUEST CMover::GetQuest( int nQuestId )
{
	return FindQuest( nQuestId );
}
int CMover::GetItemNum( DWORD dwItemId )
{
	int nNum	= 0;
	int nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	for( int i = 0 ; i < nSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
			nNum	+= pItemElem->m_nItemNum;
	}
	return nNum;
}

#ifdef __WORLDSERVER
// 스크립트에서 사용하는 함수
// 퀘스트에서 아이템을 제거할 때 사용한다. 퀘스트 아이템들은 거래나 개인상점에 올릴 수 없기 때문에
// IsUsableItem검사를 하지 않는다.
int CMover::RemoveItemA( DWORD dwItemId, short nNum )
{
	if( nNum == -1 )
	{
		return RemoveAllItem( dwItemId );
	}

	int	nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	short nRemnant	= nNum;

	for( int i = 0 ; i < nSize && nRemnant > 0; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			if( nRemnant > pItemElem->m_nItemNum )
			{
				UpdateItem( i, UI_NUM, 0 );	// remove
				nRemnant	-= pItemElem->m_nItemNum;
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
	int	nSize	= m_Inventory.GetMax();

	for( int i = 0 ; i < nSize; ++i )
	{
		CItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			nNum += pItemElem->m_nItemNum;
			UpdateItem( i, UI_NUM, 0 );		// remove
		}
	}
	return nNum;
}

//raiders_fixed
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
void CMover::Abrade( CMover* pAttacker, int nParts )
{
	int nValue;

	if( NULL == pAttacker )
		return;

	if( pAttacker->IsPlayer() )
	{
		CItemElem* pWeapon	= pAttacker->GetWeaponItem( nParts );
		if( pWeapon )
		{
			ItemProp* pWeaponProp	= pWeapon->GetProp();
			if( NULL == pWeaponProp )
				return;

			int nWpn	= ( pWeapon->m_nHitPoint * 100 ) / (int)pWeaponProp->dwEndurance;
			int nWpnVal		= CMover::GetItemEnduranceWeight( nWpn );
			pWeapon->m_nHitPoint	-= ( pWeaponProp->nAbrasion / 10 ) * nWpnVal;

			if( IsPlayer() )	// PVP
			{
				DWORD dwParts	= GetRandomPartsAbraded();
				CItemElem* pArmorElem	= m_Inventory.GetEquip( dwParts );
				if( pArmorElem )
				{
					ItemProp* pArmorProp		= pArmorElem->GetProp();
					if( pArmorProp && ( pArmorProp->dwItemKind2 == IK2_ARMOR || pArmorProp->dwItemKind2 == IK2_ARMORETC ))
					{
						nValue	= pArmorProp->nAbrasion - pWeaponProp->nHardness;
						if( nValue < 0 )	nValue	= 0;
						pWeapon->m_nHitPoint	-= nValue * nWpnVal;

						int nArm	= ( pArmorElem->m_nHitPoint * 100 ) / (int)pArmorProp->dwEndurance;
						int nArmVal		= CMover::GetItemEnduranceWeight( nArm );
						nValue	= pWeaponProp->nAbrasion - pArmorProp->nHardness;
						if( nValue < 0 )	nValue	= 0;
						pArmorElem->m_nHitPoint		-= nValue * nArmVal;
						if( pWeapon->m_nHitPoint < 0 )	pWeapon->m_nHitPoint	= 0;
						if( pArmorElem->m_nHitPoint < 0 )	pArmorElem->m_nHitPoint	= 0;

						if( nWpn != ( pWeapon->m_nHitPoint * 100 ) / (int)pWeaponProp->dwEndurance )
							pAttacker->UpdateItem( pWeapon->m_dwObjId, UI_HP, pWeapon->m_nHitPoint );
						if( nArm != ( pArmorElem->m_nHitPoint * 100 ) / (int)pArmorProp->dwEndurance )
							this->UpdateItem( pArmorElem->m_dwObjId, UI_HP, pArmorElem->m_nHitPoint );
					}
				}
			}
			else	// PVM
			{
				MoverProp* pMoverProp	= GetProp();
				nValue	= pMoverProp->nAbrasion - pWeaponProp->nHardness;
				if( nValue < 0 )	nValue	= 0;
				pWeapon->m_nHitPoint	-= nValue * nWpnVal;
				if( pWeapon->m_nHitPoint < 0 )	pWeapon->m_nHitPoint	= 0;

				if( nWpn != ( pWeapon->m_nHitPoint * 100 ) / (int)pWeaponProp->dwEndurance )
					pAttacker->UpdateItem( pWeapon->m_dwObjId, UI_HP, pWeapon->m_nHitPoint );
			}
		}
	}
	else
	{
		if( IsPlayer() )	// MVP
		{
			DWORD dwParts	= GetRandomPartsAbraded();
			CItemElem* pArmorElem	= m_Inventory.GetEquip( dwParts );
			if( pArmorElem )
			{
				ItemProp* pArmorProp		= pArmorElem->GetProp();
				if( pArmorProp && ( pArmorProp->dwItemKind2 == IK2_ARMOR || pArmorProp->dwItemKind2 == IK2_ARMORETC ) )
				{
					ItemProp* pWeaponProp	= pAttacker->GetActiveHandItemProp( nParts );
					if( pWeaponProp )
					{
						int nArm	= ( pArmorElem->m_nHitPoint * 100 ) / (int)pArmorProp->dwEndurance;
						int nArmVal		= CMover::GetItemEnduranceWeight( nArm );
						nValue	= pWeaponProp->nAbrasion - pArmorProp->nHardness;
						if( nValue < 0 )	nValue	= 0;
						pArmorElem->m_nHitPoint		-= nValue * nArmVal;
						if( pArmorElem->m_nHitPoint < 0 )	pArmorElem->m_nHitPoint	= 0;
						if( nArm != ( pArmorElem->m_nHitPoint * 100 ) / (int)pArmorProp->dwEndurance )
							this->UpdateItem( pArmorElem->m_dwObjId, UI_HP, pArmorElem->m_nHitPoint );
					}
				}
			}
		}
	}
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
	else if( nEndurance <= 50 )
		return 0.6f;
	else if( nEndurance <= 75 )
		return 0.8f;
	return 1.0f;
}

int CMover::GetItemEnduranceWeight( int nEndurance )
{
	if( nEndurance <= 50 )
		return (int)3;
	else if( nEndurance <= 75 )
		return (int)2;
	return 1;
}



#ifdef __WORLDSERVER
void CMover::AddItemCommercialEvent( BOOL f, int nLevelbuf )
{

}

void CMover::AddItemLevelUpEvent( BOOL f, int nLevelbuf )
{
#ifndef __SDEBUG
	if( f && ((CUser*)this)->m_nMaximumLevel < GetLevel() && GetLevel() < 250 && nLevelbuf < GetLevel() ) // 렙따 검사
#endif // __SDEBUG
	{
		CTime tCurr	= CTime::GetCurrentTime();
		// 이벤트 아이템 레벨어시마다 주기
		int nRand = xRandom( 9999 );
		DWORD dwTrum = 0;
		DWORD dwLevelFound  = 0;
		int nGetLevel = GetLevel();
		if( 82 < nGetLevel )
		{
			nGetLevel = 82;		// 60->82
		}

		if( prj.m_nReNewItemFound[ nGetLevel + 1 ] > prj.m_nReNewItemFound[ nGetLevel ] )
		{
			dwTrum = ( prj.m_nReNewItemFound[ nGetLevel + 1 ] - prj.m_nReNewItemFound[ nGetLevel ] ) / 2;
			dwLevelFound = prj.m_nReNewItemFound[ nGetLevel ] + dwTrum;
		}
		else
		{
			dwLevelFound = 0;
		}

		if( dwLevelFound > 0 )
		{
			int nruff = 0;
			while( 1 )
			{
				dwTrum /= 2;
				if( dwTrum == 0 )
					dwTrum = 1;
				
				if( prj.m_aReNewEvent[dwLevelFound].fPercent < nRand )
				{
					if( nRand < prj.m_aReNewEvent[ dwLevelFound + 1 ].fPercent )
					{
						dwLevelFound += 1;
						break;
					}
					dwLevelFound += dwTrum;
				}
				else
				{
					if( prj.m_aReNewEvent[ dwLevelFound - 1 ].fPercent < nRand )
					{
						break;
					}
					dwLevelFound -= dwTrum;
				}
				if( nruff == 3000 )
				{
					dwLevelFound = 0;
					break;
				}
				++nruff;
			}

			CItemElem itemElem;
			itemElem.m_dwItemId		= prj.m_aReNewEvent[ dwLevelFound ].dwItemId;
			itemElem.m_nItemNum		= 1;
			itemElem.m_nHitPoint	= -1;

//			if( prj.m_aReNewEvent[ dwLevelFound ].StartTime <= tCurr && tCurr <= prj.m_aReNewEvent[ dwLevelFound ].EndTime )
			if( prj.m_tReNewEventStartTime <= tCurr && tCurr < prj.m_tReNewEventEndTime )
			{
				if( CreateItem( &itemElem ) == TRUE )
				{
					ItemProp* pItemProp	= prj.GetItemProp( prj.m_aReNewEvent[ dwLevelFound ].dwItemId );
					if( pItemProp )
					{
						CString strItemMessage;
						strItemMessage.Format("%s : Event Item -> %s", GetName(), pItemProp->szName );
						//						( (CUser*)this )->AddText( strItemMessage );
						g_UserMng.AddEventMessage( this, strItemMessage, 2 );
						// 이벤트 로그 남기기
						LogItemInfo aLogItem;
						aLogItem.Action = "E";
						aLogItem.SendName = GetName();
						aLogItem.RecvName = "ReNewEvent";
						aLogItem.WorldId = ((CUser*)this)->GetWorld()->GetID();
						aLogItem.Gold = aLogItem.Gold2 = GetGold();
						aLogItem.Gold_1 = GetLevel();
						g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
					}
				}
				else
				{
					g_UserMng.AddEventMessage( this, " ", 3 );
				}
			}
		}
		((CUser*)this)->m_nMaximumLevel = GetLevel();	
	}
}
void CMover::AddItemEvent( BOOL f, int nLevelbuf )
{
	AddItemLevelUpEvent( f, nLevelbuf );
	AddItemCommercialEvent( f, nLevelbuf );
}
#endif // __WORLDSERVER

void CMover::UpdateItem( BYTE nId, CHAR cParam, DWORD dwValue )
{
	CItemBase* pItemBase = GetItemId( nId );

	if( pItemBase )
	{
		switch( cParam )
		{
			case UI_COOLTIME:
			case UI_NUM:
				{
					if( (int)dwValue <= 0 ) 
					{
						RemoveItemId( nId );
					}
					else 
					{
						( (CItemElem*)pItemBase )->m_nItemNum	= dwValue;
					}
					break;
				}
			case UI_HP:
				{
					( (CItemElem*)pItemBase )->m_nHitPoint	= dwValue;
					UpdateParam();
					break;
				}
			case UI_RN:
				{
					if( ( (CItemElem*)pItemBase )->m_nRepairNumber >= 100 )
						return;
					if( dwValue > 100 )
						dwValue		= 100;
					( (CItemElem*)pItemBase )->m_nRepairNumber	= dwValue;
					UpdateParam();
				}
				break;
				
			case UI_AO: // 아이템 + 올리기...
				{
					if( ( (CItemElem*)pItemBase )->GetAbilityOption() > 10 )
						return;
					
					if( dwValue > 10 )
						dwValue		= 10;
					( (CItemElem*)pItemBase )->SetAbilityOption( dwValue );
					UpdateParam();
				}
				break;
			case UI_RAO: // 아이템 속성 + 올리기...
				{
					if( ( (CItemElem*)pItemBase )->m_nResistAbilityOption > 10 )
						return;
					if( dwValue > 10 )
						dwValue		= 10;
					( (CItemElem*)pItemBase )->m_nResistAbilityOption	= dwValue;
					UpdateParam();
				}
				break;
			case UI_IR:  // 아이템에 무슨속성인지 넣기
				{
					( (CItemElem*)pItemBase )->m_bItemResist	= dwValue;
					UpdateParam();
				}
				break;

#ifdef __PIERCING_SM_TIME
			case UI_PIERCING_SIZE:
				( (CItemElem*)pItemBase )->m_piercingInfo.nPiercedSize = dwValue;
				break;
			case UI_PIERCING:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (CItemElem*)pItemBase )->m_piercingInfo.adwItemId[wIndex]	= wItemId;
				}
				break;
#endif	// __PIERCING_SM_TIME

#ifdef __FINITEITEM0705
			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= (DWORD)(short)LOWORD( dwValue );
					BYTE byFlag		= (BYTE)HIWORD( dwValue );
					int nParts	= dwObjIndex - m_Inventory.GetSize();
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
						m_aEquipInfo[nParts].byFlag	= byFlag;
					( (CItemElem*)pItemBase )->m_byFlag		= byFlag;
					UpdateParts( FALSE );
#endif	// __CLIENT
					UpdateParam();
					break;
				}
#endif	// __FINITEITEM0705
			default:
				break;
		}
	}
	else
	{
		switch( cParam )
		{
			case UI_FLAG:
				{
#ifdef __FINITEITEM0705
#ifdef __CLIENT
					DWORD dwObjIndex	= (DWORD)(short)LOWORD( dwValue );
					BYTE byFlag		= (BYTE)HIWORD( dwValue );
					int nParts	= dwObjIndex - m_Inventory.GetSize();
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
					{
						m_aEquipInfo[nParts].byFlag	= byFlag;
						UpdateParts( TRUE );
					}
#endif	// __CLIENT
#endif // __FINITEITEM0705
//					UpdateParam();
					break;
				}
			default:
				break;
		}
	}
#ifdef __WORLDSERVER
	if( IsPlayer() )
		( (CUser*)this )->AddUpdateItem( 0, nId, cParam, dwValue );
#endif
}

float CMover::GetExpFactor( void )
{
	float fFactor	= 1.0f;
#ifdef __EVENT_OLDBOY0621
	if( IsMode( EVENT_OLDBOY_MODE ) )
		fFactor	*= 1.1f;
#endif	// __EVENT_OLDBOY0621

	BOOL bGlvCheer	= FALSE;
#ifdef __SM_ITEM_2ND
	if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) )
		fFactor	*= 1.5f;
	if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ) )
		fFactor	*= 2.0f;

	if( m_SkillState.HasSkill( BUFF_ITEM, II_SYS_SYS_SCR_GLVCHEER ) )
	{
		bGlvCheer	= TRUE;
		fFactor	*= 1.10f;
	}
#endif	// __SM_ITEM_2ND

#ifdef __CHEER0615
	if( !bGlvCheer && m_SkillState.HasSkill( BUFF_ITEM, II_CHEERUP ) )
		fFactor	*= 1.05f;
#endif	// __CHEER0620

	return fFactor;
}