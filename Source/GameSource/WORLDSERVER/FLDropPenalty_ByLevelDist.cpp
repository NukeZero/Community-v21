#include "StdAfx.h"

#include "FLDropPenalty_ByLevelDist.h"


//---------------------------------------------------------------------------------------------------------//

FLDropPenalty_ByLevelDist::FLDropPenalty_ByLevelDist()
{

}

FLDropPenalty_ByLevelDist::~FLDropPenalty_ByLevelDist()
{

}

FLDropPenalty_ByLevelDist & FLDropPenalty_ByLevelDist::GetInstance()
{
	static FLDropPenalty_ByLevelDist inst;
	return inst;
}

void FLDropPenalty_ByLevelDist::Load()
{
	//아이템이 로드된 후
	const TCHAR szFileName[] = _T( "DropItem/propDropLevelPenalty.csv" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	CScript kScript;
	if( kScript.Load( szFullPath ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return;
	}


	for( int line = 0; kScript.tok != FINISHED; ++line )
	{
		//1. preread
		kScript.GetToken( TRUE );
		if( kScript.Token == "" ) 
			continue;

		std::string strLevel	= kScript.Token;
		std::transform( strLevel.begin(), strLevel.end(), strLevel.begin(), toupper );

		const int nLevelMax		= ( strLevel == "MAX" ) ? MAX_3RD_LEGEND_LEVEL : atoi( kScript.Token );

		const float fItemRate	= kScript.GetFloat( TRUE );
		const float fGoldRate	= kScript.GetFloat( TRUE );

		//if( line == 1 )					// 제목 스킵
		//	continue;

		//2. insert
		if( nLevelMax == 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID LEVEL(%d) line(%d) ]"), nLevelMax, line );
			break;
		}

		m_vecPenaltyRate.push_back( FLPenaltyRate_Data( nLevelMax, fItemRate, fGoldRate ) );
	}

	//검증
	int nOldLevel				= 0;
	for( FLPenaltyRateVec::const_iterator it = m_vecPenaltyRate.begin(); it != m_vecPenaltyRate.end(); ++it )
	{
		const FLPenaltyRate_Data & data	= *it;
		if( nOldLevel >= data.nLevelDist )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ BAD ORDER : OLD(%d) CURR(%d) ]" ), nOldLevel, data.nLevelDist );
			break;
		}

		nOldLevel				= data.nLevelDist;
	}
}



float FLDropPenalty_ByLevelDist::GetDeclineProb_Item( const int nAttackerLevel, const int nDroperLevel )
{
	const int nLevelDist		= nAttackerLevel - nDroperLevel;

	for( FLPenaltyRateVec::const_iterator it = m_vecPenaltyRate.begin(); it != m_vecPenaltyRate.end(); ++it )
	{
		const FLPenaltyRate_Data & data = *it;
		if( nLevelDist <= data.nLevelDist )
			return data.fItemRate;
	}

	return 1.0f;
}

float FLDropPenalty_ByLevelDist::GetDeclineProb_Gold( const int nAttackerLevel, const int nDroperLevel )
{
	const int nLevelDist		= nAttackerLevel - nDroperLevel;

	for( FLPenaltyRateVec::const_iterator it = m_vecPenaltyRate.begin(); it != m_vecPenaltyRate.end(); ++it )
	{
		const FLPenaltyRate_Data & data = *it;
		if( data.nLevelDist <= nLevelDist )
			return data.fGoldRate;
	}

	return 1.0f;
}
