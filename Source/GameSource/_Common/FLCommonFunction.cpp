
#include "StdAfx.h"
#include "FLCommonFunction.h"


FLCommonFunction::FLCommonFunction()
{

}

FLCommonFunction::~FLCommonFunction()
{

}

int		FLCommonFunction::GetGeneralSkillPoint( const DWORD dwJob, const DWORD dwLevel )
{
	int nSkillPoint = 0;
	int nGeneralLevel = dwLevel;

	if( dwJob >= MAX_PROFESSIONAL )
	{
		nGeneralLevel = MAX_GENERAL_LEVEL;
	}

	for( int i = 2; i <= nGeneralLevel; ++i )
	{
		nSkillPoint += ( ( i - 1 ) / SKILLPOINT_DIVIDE ) + SKILLPOINT_BASE;
	}

	return nSkillPoint;
}

int		FLCommonFunction::GetTotalSkillPoint( const DWORD dwJob, const DWORD dwLevel )
{
	/*
	// 		현재 마스터, 히어로 일때는 스킬포인트를 지급 하지 않는다.
	// 		3차 전직을 했을 경우에는 스킬포인트를 지급 한다.
	//		그리하여 마스터, 히어로 구간의 스킬포인트를 계산에서 빼고 적용..
	*/

	//////////////////////////////////////////////////////////////////////////
	// BEGIN calc skill point

	int nSkillPoint = 0;

	//////////////////////////////////////////////////////////////////////////
	// BEGIN 일반 클래스 캐릭터 레벨 계산

	nSkillPoint = GetGeneralSkillPoint( dwJob, dwLevel );

	// END 일반 클래스 캐릭터 레벨 계산
	//////////////////////////////////////////////////////////////////////////

	switch( dwJob )
	{
		//////////////////////////////////////////////////////////////////////////
		// BEGIN 1차 전직 퀘스트 보상
	case JOB_MERCENARY:
		nSkillPoint += 40;
		break;

	case JOB_ACROBAT:
		nSkillPoint += 50;
		break;

	case JOB_ASSIST:
		nSkillPoint += 60;
		break;

	case JOB_MAGICIAN:
		nSkillPoint += 90;
		break;
		// END 1차 전직 퀘스트 보상
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// BEGIN 2차 전직 퀘스트 보상
	case JOB_KNIGHT:
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
		nSkillPoint += 40;
		nSkillPoint += 80;

		break;

	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		nSkillPoint += 50;
		nSkillPoint += 100;

		break;

	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		nSkillPoint += 60;
		nSkillPoint += 100;

		break;

	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		nSkillPoint += 60;
		nSkillPoint += 120;

		break;

	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		nSkillPoint += 90;
		nSkillPoint += 90;

		break;

	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		nSkillPoint += 90;
		nSkillPoint += 300;

		break;

	default:
		break;
		// END 2차 전직 퀘스트 보상
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// BEGIN 마스터 전승 시 퀘스트 보상 스킬 포인트
	if( dwJob >= MAX_PROFESSIONAL )
	{
		nSkillPoint += 400;
	}
	// END 마스터 전승 시 퀘스트 보상 스킬 포인트
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// BEGIN 3차 전직

	if( dwJob >= MAX_HERO )
	{
		int nRemainLevel = dwLevel - MAX_LEGEND_LEVEL;

		if( nRemainLevel > 0 )
		{
			if( dwJob == JOB_LORDTEMPLER_HERO || dwJob == JOB_FLORIST_HERO )
			{
				nSkillPoint += nRemainLevel * SKILLPOINT_LEGENDHERO_EXCEPTION;
			}
			else
			{
				nSkillPoint += nRemainLevel * SKILLPOINT_LEGENDHERO_BASE;
			}
		}

		switch( dwJob )
		{
		case JOB_LORDTEMPLER_HERO:
			nSkillPoint += 40;
			nSkillPoint += 80;
			nSkillPoint += 24;
			break;

		case JOB_STORMBLADE_HERO:
			nSkillPoint += 40;
			nSkillPoint += 80;
			nSkillPoint += 16;
			break;

		case JOB_WINDLURKER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			nSkillPoint += 50;
			nSkillPoint += 100;
			nSkillPoint += 16;
			break;

		case JOB_FLORIST_HERO:
			nSkillPoint += 60;
			nSkillPoint += 100;
			nSkillPoint += 24;
			break;

		case JOB_FORCEMASTER_HERO:
			nSkillPoint += 60;
			nSkillPoint += 120;
			nSkillPoint += 16;
			break;

		case JOB_MENTALIST_HERO:
			nSkillPoint += 90;
			nSkillPoint += 90;
			nSkillPoint += 20;
			break;

		case JOB_ELEMENTORLORD_HERO:
			nSkillPoint += 90;
			nSkillPoint += 300;
			nSkillPoint += 16;
			break;

		default:
			break;
		}
	}

	// END 3차 전직
	//////////////////////////////////////////////////////////////////////////

	return nSkillPoint;

	// END calc skill point
	//////////////////////////////////////////////////////////////////////////
}

DWORD	FLCommonFunction::GetSubLineJob( const DWORD dwJob )
{
	if( dwJob < MAX_JOBBASE || dwJob > MAX_JOB )
	{
		return NULL_ID;
	}

	else if( dwJob >= MAX_JOBBASE && dwJob < MAX_EXPERT )
	{
		return JOB_VAGRANT;
	}

	else if( dwJob >= MAX_EXPERT && dwJob < MAX_PROFESSIONAL )
	{
		return ( dwJob / 2 - 2 );
	}

	else if( dwJob >= MAX_PROFESSIONAL && dwJob < MAX_MASTER )
	{
		return ( dwJob - 10 );
	}

	else if( dwJob >= MAX_MASTER && dwJob < MAX_HERO )
	{
		return ( dwJob - 8 );
	}

	else if( dwJob >= MAX_HERO && dwJob < MAX_LEGEND_HERO )
	{
		return ( dwJob - 8 );
	}

	else
	{
		return NULL_ID;
	}
}

DWORD	FLCommonFunction::GetJobSkillCount( const DWORD dwJob )
{
	if( dwJob < JOB_VAGRANT || dwJob >= MAX_JOB )
	{
		return NULL_ID;
	}

	else if( dwJob >= JOB_VAGRANT && dwJob < MAX_JOBBASE )
	{
		return 0;
	}

	else if( dwJob >= MAX_JOBBASE && dwJob < MAX_EXPERT )
	{
		return MAX_JOB_SKILL;
	}

	else if( dwJob >= MAX_EXPERT && dwJob < MAX_PROFESSIONAL )
	{
		return MAX_JOB_SKILL + MAX_EXPERT_SKILL;
	}

	else if( dwJob >= MAX_PROFESSIONAL && dwJob < MAX_MASTER )
	{
		return MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL;
	}

	else if( dwJob >= MAX_MASTER && dwJob < MAX_HERO )
	{
		return MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL;
	}

	else if( dwJob >= MAX_HERO && dwJob < MAX_LEGEND_HERO )
	{
		return MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL;
	}

	else
	{
		return NULL_ID;
	}
}

bool	FLCommonFunction::IsMasterJob( const DWORD dwJob )
{
	if( dwJob >= MAX_PROFESSIONAL && dwJob < MAX_MASTER )
	{
		return true;
	}

	return false;
}

// int		FLCommonFunction::GetTotalStatPoint( const DWORD _Job, const DWORD _Level )
// {
// 	if( _Job < JOB_VAGRANT || _Job >= MAX_JOB || _Level < 0 || _Level >= MAX_EXPCHARACTER )
// 	{
// 		FLASSERT( 0 );
// 		return -1;
// 	}
// 
// 	int SumStatPoint	= 0;
// 
// 	// 기본.
// 	for( DWORD Nth = 2; Nth <= _Level; ++Nth )
// 	{
// 		SumStatPoint	+= prj.m_aExpCharacter[ Nth ].dwLPPoint;
// 	}
// 
// 	// 마스터 이상일때.
// 	if( _Job >= MAX_PROFESSIONAL )
// 	{
// 		for( DWORD Nth = 61; Nth <= _Level; ++Nth )
// 		{
// 			SumStatPoint	+= 1;
// 		}
// 	}
// 
// 	// 히어로 전승 퀘스트 보상.
// 	if( _Job >= MAX_MASTER )
// 	{
// 		SumStatPoint	+= 12;
// 	}
// 
// 	return SumStatPoint;
// }
