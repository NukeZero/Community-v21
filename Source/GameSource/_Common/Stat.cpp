#include "StdAfx.h"
#include "Stat.h"
#include <network/FLAr.h>

Stat::Stat()
:	m_OriginStr( 0 ),	m_OriginSta( 0 ),	m_OriginDex( 0 ),	m_OriginInt( 0 )
,	m_DiceStr( 0 ),		m_DiceSta( 0 ),		m_DiceDex( 0 ),		m_DiceInt( 0 )
,	m_RemainStatPoint( 0 )

{
	InitializeOriginStat();
	InitializeDiceStat();
}

Stat::~Stat()
{
}

Stat& Stat::operator=( const Stat& rhs )
{
	m_OriginStr 		= rhs.m_OriginStr;
	m_OriginSta 		= rhs.m_OriginSta;
	m_OriginDex 		= rhs.m_OriginDex;
	m_OriginInt 		= rhs.m_OriginInt;

	m_DiceStr 			= rhs.m_DiceStr;
	m_DiceSta 			= rhs.m_DiceSta;
	m_DiceDex 			= rhs.m_DiceDex;
	m_DiceInt 			= rhs.m_DiceInt;

	m_RemainStatPoint	= rhs.m_RemainStatPoint;

	return *this;
}

void Stat::InitializeOriginStat()
{
	m_OriginStr 		= DEFAULT_ORIGIN_STAT_VALUE;
	m_OriginSta 		= DEFAULT_ORIGIN_STAT_VALUE;
	m_OriginDex 		= DEFAULT_ORIGIN_STAT_VALUE;
	m_OriginInt 		= DEFAULT_ORIGIN_STAT_VALUE;

	m_RemainStatPoint	= 0;
}

void Stat::InitializeDiceStat()
{
	m_DiceStr = 0;
	m_DiceSta = 0;
	m_DiceDex = 0;
	m_DiceInt = 0;
}

void Stat::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_OriginStr << m_OriginSta << m_OriginDex << m_OriginInt;
		ar << m_DiceStr << m_DiceSta << m_DiceDex << m_DiceInt;
		ar << m_RemainStatPoint;
	}
	else
	{
		ar >> m_OriginStr >> m_OriginSta >> m_OriginDex >> m_OriginInt;
		ar >> m_DiceStr >> m_DiceSta >> m_DiceDex >> m_DiceInt;
		ar >> m_RemainStatPoint;
	}
}

int* Stat::GetOriginStatPtr( StatType st )
{
	switch( st )
	{
		case Str :	return &m_OriginStr;
		case Sta :	return &m_OriginSta;
		case Dex :	return &m_OriginDex;
		case Int :	return &m_OriginInt;
	}
	FLASSERT( false );
	return NULL;
}

int Stat::GetOriginStat( StatType st )
{
	int* pStat = GetOriginStatPtr( st );
	return *pStat;
}

void Stat::SetOriginStat( StatType st, int value )
{
	int* const pStat = GetOriginStatPtr( st );
	*pStat = value;
}

int* Stat::GetDiceStatPtr( StatType st )
{
	switch( st )
	{
		case Str :	return &m_DiceStr;
		case Sta :	return &m_DiceSta;
		case Dex :	return &m_DiceDex;
		case Int :	return &m_DiceInt;
	}
	FLASSERT( false );
	return NULL;
}

int Stat::GetDiceStat( StatType st )
{
	int* pStat = GetDiceStatPtr( st );
	return *pStat;
}

void Stat::SetDiceStat( StatType st, int value )
{
	int* const pStat = GetDiceStatPtr( st );
	*pStat = value;
}

bool	Stat::SetRemainStatPoint( const int _RemainStatPoint )
{
	if( _RemainStatPoint < 0 )
	{
		FLASSERT( 0 );
		return false;
	}

	m_RemainStatPoint	= _RemainStatPoint;

	return true;
}

#ifdef __WORLDSERVER
int	Stat::GetVirtualJob( int job )
{
	// 2차 직업 이상인 경우 2차 직업으로 맞춘다.
	int virtualJobBase = 0;
	if( job < MAX_PROFESSIONAL )
		virtualJobBase = MAX_PROFESSIONAL;
	else if( job < MAX_MASTER )							
		virtualJobBase = MAX_MASTER;
	else if( job < MAX_HERO )
		virtualJobBase = MAX_HERO;
	else if( job < MAX_LEGEND_HERO )
		virtualJobBase = MAX_LEGEND_HERO;
	
	return job - ( virtualJobBase - MAX_PROFESSIONAL );
}

BYTE Stat::gettable( int job )
{
	typedef std::map<int, BYTE> ProbabiltyTable;
	static ProbabiltyTable	probabilityTable;
	if( probabilityTable.empty() )	{										//	Str	Sta	Dex	Int
		// 1차 직업
		probabilityTable.insert( make_pair( JOB_MERCENARY,		0x0a ) );	//	1	0	1	0
		probabilityTable.insert( make_pair( JOB_ACROBAT,		0x0a ) );	//	1	0	1	0
		probabilityTable.insert( make_pair( JOB_ASSIST,			0x01 ) );	//	0	0	0	1
		probabilityTable.insert( make_pair( JOB_MAGICIAN,		0x01 ) );	//	0	0	0	1
		// 2차 직업
		probabilityTable.insert( make_pair( JOB_KNIGHT,			0x0c ) );	//	1	1	0	0
		probabilityTable.insert( make_pair( JOB_BLADE,			0x0a ) );	//	1	0	1	0
		probabilityTable.insert( make_pair( JOB_JESTER,			0x0a ) );	//	1	0	1	0
		probabilityTable.insert( make_pair( JOB_RANGER,			0x02 ) );	//	0	0	1	0
		probabilityTable.insert( make_pair( JOB_RINGMASTER,		0x01 ) );	//	0	0	0	1
		probabilityTable.insert( make_pair( JOB_BILLPOSTER,		0x0b ) );	//	1	0	1	0
		probabilityTable.insert( make_pair( JOB_PSYCHIKEEPER,	0x01 ) );	//	0	0	0	1
		probabilityTable.insert( make_pair( JOB_ELEMENTOR,		0x01 ) );	//	0	0	0	1
	}

	ProbabiltyTable::iterator it = probabilityTable.find( GetVirtualJob( job ) );
	if( it == probabilityTable.end() )
		return 0;

	return it->second;
}

bool Stat::IsStatUp( int job, StatType st, const BYTE table )
{
	const UINT	BaseProbability = 100;
	const UINT	VagrantStatProbability = 50;
	const UINT	AdvantageousStatProbability = 45;
	const UINT	DisadvantageousStatProbability = 55;

	if( job == JOB_VAGRANT )
		return xRandom( BaseProbability ) < VagrantStatProbability;

	const UINT probablity = ( table & ( 0x08 >> st ) ) != 0x00 ? AdvantageousStatProbability : DisadvantageousStatProbability;
	return xRandom( BaseProbability ) < probablity;
}

void Stat::ApplyRandomDiceStats( int job )
{
	const int UnitStatDelta = 1;
	const int MAX_COUNT = 2;
	int	statUpCount = 0, statDownCount = 0;

	// 초기화(이미 최소값인 경우 무조건 증가)
	int result[MaxStatType] = {0,};
	for( int i = LoopBaseStatType; i < MaxStatType; ++i )	{
		StatType stat = static_cast<StatType>( i );
		if( GetDiceStat( stat ) - UnitStatDelta <= -DEFAULT_ORIGIN_STAT_VALUE )	{
			++statUpCount;
			result[stat] = UnitStatDelta;
		}
	}

	// 결과 산출
	const BYTE table = gettable( job );
	for( int i = LoopBaseStatType; i < MaxStatType; ++i )	{
		StatType stat = static_cast<StatType>( i );
		if( result[stat] != 0 )
			continue;

		bool bStatUp = false;
		if( ( statUpCount < MAX_COUNT ) && ( statDownCount < MAX_COUNT ) )
			bStatUp = IsStatUp( job, stat, table );
		else
			bStatUp = ( statUpCount < statDownCount ) ? true : false;

		if( bStatUp )	{	// 스텟 증가
			++statUpCount;
			result[stat] = UnitStatDelta;
		}
		else	{			// 스텟 감소
			++statDownCount;
			result[stat] = -UnitStatDelta;
		}
	}
	// 결과 적용
	for( int i = LoopBaseStatType; i < MaxStatType; ++i )	{
		StatType stat = static_cast<StatType>( i );
		SetDiceStat( stat, GetDiceStat( stat ) + result[stat] );
	}
}

bool Stat::ResetDiceStat()
{
	if( m_DiceStr == 0 && m_DiceSta == 0 && m_DiceDex == 0 && m_DiceInt == 0 )
		return false;

	InitializeDiceStat();
	return true;
}
#endif // __WORLDSERVER