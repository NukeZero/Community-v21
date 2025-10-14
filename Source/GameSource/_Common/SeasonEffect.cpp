
#include "StdAfx.h"
#include "SeasonEffect.h"

#ifdef __CORESERVER
#include "../coreserver/DPCoreSrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#endif // __CORESERVER

static const int	maxMonth	= 12;

#ifndef __CORESERVER
namespace nsSeasonFile
{
	void	setVariableSeasonFile()
	{
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 20	/*"MaCoPrTr01"*/, "MaCoPrTr16" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 43	/*"MaCoPrTr03"*/, "MaCoPrTr16" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 197	/*"MaCoPrTr10"*/, "MaCoPrTr16" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 198	/*"MaCoPrTr11"*/, "MaCoPrTr16" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 201	/*"MaCoPrTr14"*/, "MaCoPrTr16" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 234	/*"MaCoPrTr20"*/, "MaCoPrTr16" ) );
																					
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 21	/*"MaCoPrTr02"*/, "MaCoPrTr17" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 75	/*"MaCoPrTr05"*/, "MaCoPrTr17" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 76	/*"MaCoPrTr04"*/, "MaCoPrTr17" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 199	/*"MaCoPrTr12"*/, "MaCoPrTr17" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 200	/*"MaCoPrTr13"*/, "MaCoPrTr17" ) );
		variableSeasonFileTable[SEASON_SPRING].insert( SeasonFileTable::value_type( 207	/*"MaCoPrTr15"*/, "MaCoPrTr17" ) );

		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 20	/*"MaCoPrTr01"*/, "MapleTree01" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 43	/*"MaCoPrTr03"*/, "MapleTree01" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 197	/*"MaCoPrTr10"*/, "MapleTree01" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 198	/*"MaCoPrTr11"*/, "MapleTree01" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 201	/*"MaCoPrTr14"*/, "MapleTree01" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 234	/*"MaCoPrTr20"*/, "MapleTree01" ) );

		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 21	/*"MaCoPrTr02"*/, "MapleTree02" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 75	/*"MaCoPrTr05"*/, "MapleTree02" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 76	/*"MaCoPrTr04"*/, "MapleTree02" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 199	/*"MaCoPrTr12"*/, "MapleTree02" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 200	/*"MaCoPrTr13"*/, "MapleTree02" ) );
		variableSeasonFileTable[SEASON_FALL].insert( SeasonFileTable::value_type( 207	/*"MaCoPrTr15"*/, "MapleTree02" ) );
	}

	bool	isVariableSeasonFile( const SeasonFileTable::value_type::first_type key )
	{
 		for( int season = 0; season < SEASON_MAX; ++season ) {
			if( variableSeasonFileTable[season].find( key ) != variableSeasonFileTable[season].end() ) {
				return true;
			}
		}

		return false;
	}

	bool	changeVariableSeasonFile( const int season, const SeasonFileTable::value_type::first_type key, std::string & fileName )
	{
		if( season < 0 || season >= SEASON_MAX ) {
			return false;
		}

		SeasonFileTable::const_iterator itr	= variableSeasonFileTable[season].find( key );
		if( itr == variableSeasonFileTable[season].end() ) {
			return false;
		}

		fileName	= itr->second;

		return true;
	}
}
#endif // __CORESERVER


CSeasonEffect::CSeasonEffect()
: m_season( 0 ), m_effect( false )
#ifdef __CORESERVER
, m_gmEffect( false ), m_hWorker( NULL ), m_hCloseWorker( NULL )
#endif // __CORESERVER
{
#ifdef __CORESERVER
	m_effectKeepTime.Set( MIN( 60 ), TRUE );
#endif // __CORESERVER
}

CSeasonEffect::~CSeasonEffect()
{
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER
}

CSeasonEffect*	CSeasonEffect::GetInstance()
{
	static CSeasonEffect SeasonEffect;

	return & SeasonEffect;
}

bool	CSeasonEffect::load()
{
#ifndef __CORESERVER
	nsSeasonFile::setVariableSeasonFile();
#endif // __CORESERVER

#ifdef __WORLDSERVER
	CScript scanner;

	const TCHAR szFileName[]	= _T( "EnvironmentEffect.txt" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( scanner.Load( szFullPath, FALSE ) == FALSE ) {
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	const CTime currTime	= CTime::GetCurrentTime();
	const int currMonth		= currTime.GetMonth();
	const int myLang		= g_xFlyffConfig->GetMainLanguage();

	int lang = scanner.GetNumber();

	while( scanner.tok != FINISHED ) {
		for( int month = 1; month <= maxMonth; ++month ) {
			const int season = scanner.GetNumber();
			if( myLang == lang && currMonth == month ) {
				m_season	= season;
				return true;
			}
		}

		lang = scanner.GetNumber();
	}
#endif // __WORLDSERVER
	return true;
}

int		CSeasonEffect::getSeason() const
{
	return m_season;
}

bool	CSeasonEffect::getEffect() const
{
	return m_effect;
}

void	CSeasonEffect::serializeSeason( CAr & ar ) const
{
	ar << m_season;
}

void	CSeasonEffect::deserializeSeason( CAr & ar )
{
	ar >> m_season;
}

void	CSeasonEffect::serializeEffect( CAr & ar ) const
{
	ar << m_effect;
}

void	CSeasonEffect::deserializeEffect( CAr & ar )
{
	ar >> m_effect;
}

#ifdef __CORESERVER
void	CSeasonEffect::setEffectByGM( const bool effect )
{
	m_gmEffect	= true;
	m_effect	= effect;
}

bool	CSeasonEffect::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= ::CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return false;
	return true;
}

void	CSeasonEffect::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT	CSeasonEffect::_Worker( LPVOID pParam )
{
	CSeasonEffect* pSeasonEffect	= (CSeasonEffect*)pParam;
	pSeasonEffect->Worker();
	return 0;
}

void	CSeasonEffect::Worker( void )
{
	HANDLE hHandle	= m_hCloseWorker;

	while( ::WaitForSingleObject( hHandle, 60000 ) == WAIT_TIMEOUT ) {
		if( m_gmEffect == true ) {
			m_gmEffect = false;
			m_effectKeepTime.Reset();
			g_dpCoreSrvr.SendSeasonEffect();
		}
		else {
			if( m_effectKeepTime.IsTimeOut() == TRUE ) {
				const bool prevEffect = m_effect;
				m_effect	= ( ::xRandom( 10000 ) < 3000 ) ? true : false;
				m_effectKeepTime.Reset();

				if( prevEffect != m_effect ) {
					g_dpCoreSrvr.SendSeasonEffect();
				}
			}
		}
	}
}
#endif // __CORESERVER
