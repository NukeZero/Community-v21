#include "StdAfx.h"
#include "FLGoldIconSet.h"


FLGoldIconSet &	FLGoldIconSet::GetInstance()
{
	static FLGoldIconSet inst;
	return inst;
}

void FLGoldIconSet::Load()
{
	const TCHAR szFileName[] = _T( "DropItem/propDropPenyaIconSet.csv" );

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

		std::string strGoldRange	= kScript.Token;
		std::transform( strGoldRange.begin(), strGoldRange.end(), strGoldRange.begin(), toupper );

#undef max
		const int nGoldMax		= ( strGoldRange == "MAX" ) ? std::numeric_limits< int >::max() : atoi( kScript.Token );
		const DWORD dwItemID 	= kScript.GetNumber( TRUE );

		//		if( line == 1 )					// 제목 스킵
		//			continue;

		//2. insert
		if( nGoldMax == 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID GOLD RANGE(%d) line(%d) ]"), nGoldMax, line );
			break;
		}

		m_vecIconSet.push_back( FLIconSet( nGoldMax, dwItemID ) );
	}

	//검증
	int nOldGoldMax				= 0;
	for( IconSetVec::const_iterator it = m_vecIconSet.begin(); it != m_vecIconSet.end(); ++it )
	{
		const FLIconSet & data	= *it;
		if( nOldGoldMax >= data.nGoldMax )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ BAD ORDER : OLD(%d) CURR(%d) ]" ), nOldGoldMax, data.nGoldMax );
			break;
		}

		nOldGoldMax				= data.nGoldMax;
	}

}

DWORD FLGoldIconSet::GetIconItemID( const int nGold )
{
	IconSetVec::const_iterator it = m_vecIconSet.begin();
	for( ; it != m_vecIconSet.end(); ++it )
	{
		const FLIconSet kIconSet	= *it;
		if( nGold <= kIconSet.nGoldMax )
			return kIconSet.dwItemID;
	}

	return m_vecIconSet[ m_vecIconSet.size() - 1 ].dwItemID;
}

//-----------------------------------------------------------------------------------------------------//

FLGoldIconSet::FLIconSet::FLIconSet( const int goldMax, const DWORD itemID ) : nGoldMax( goldMax ), dwItemID( itemID )
{

}