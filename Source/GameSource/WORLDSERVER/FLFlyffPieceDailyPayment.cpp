
#include "StdAfx.h"
#include "FLFlyffPieceDailyPayment.h"
#include "post.h"


namespace nsFlyffPieceDailyPayment
{

FLFlyffPieceDailyPayment::FLFlyffPieceDailyPayment()
: m_bActivate( false ), m_nDay( DAY_NONE )
{
	Init();
}

FLFlyffPieceDailyPayment::~FLFlyffPieceDailyPayment()
{
	Init();
}

FLFlyffPieceDailyPayment*	FLFlyffPieceDailyPayment::GetInstance()
{
	static FLFlyffPieceDailyPayment sFlyffPieceDailyPayment;

	return & sFlyffPieceDailyPayment;
}

void	FLFlyffPieceDailyPayment::Init()
{
	m_nDay			= DAY_NONE;

	for( IndexMap::iterator pos = m_mapIndex.begin(); pos != m_mapIndex.end(); ++pos )
	{
		pos->second.clear();
	}
	m_mapIndex.clear();
	m_mapPeriodData.clear();
	m_mapConnectionData.clear();
	m_mapUserData.clear();
	m_mapItemData.clear();

	m_mapBurningModeData.clear();

	for( PlayerIndexMap::iterator pos = m_mapPlayerIndex.begin(); pos != m_mapPlayerIndex.end(); ++pos )
	{
		pos->second.clear();
	}
	m_mapPlayerIndex.clear();
	m_mapConnectionPlayer.clear();
}

void	FLFlyffPieceDailyPayment::Load()
{
	const bool bRet = LoadScript( _T( "Event\\FlyffPieceDailyPaymentEvent.csv" ) )
					&& CheckDailySettingData();

	if( bRet == true )
	{
		SetActivate( true );

		const int nDay = GetCurrentDay();

		SetDay( nDay );
	}
	else
	{
		Init();
		SetActivate( false );

		FLINFO_LOG( PROGRAM_NAME, _T( "[ CONTENT DEACTIVATED. 접속 시간에 따른 일일 지급 시스템이 비활성화 되었습니다. ]" ) );
	}
}

bool	FLFlyffPieceDailyPayment::IsActivate() const
{
	return m_bActivate;
}

bool	FLFlyffPieceDailyPayment::SetBurningMode( const T_BURNING_MODE & kMode )
{
	UNREFERENCED_PARAMETER( kMode );
	return false;

// 	if( IsActivate() == false )
// 	{
// 		return false;
// 	}
// 
// 	if( m_mapBurningModeData.empty() == false )
// 	{
// 		return false;
// 	}
// 
// 	if( kMode.dwBurningMode == 0 || kMode.dwBurningMode > E_MODE_ALL )
// 	{
// 		return false;
// 	}
// 
// 	const DWORD dwBurningModeMinute	= kMode.dwMinute > MAX_BURNING_MINUTE ? MAX_BURNING_MINUTE : kMode.dwMinute;
// 
// 	const CTime time		= CTime::GetCurrentTime() + CTimeSpan( 0, 0, dwBurningModeMinute, 0 );
// 	const time_t EndTime	= static_cast<time_t>( time.GetTime() );
// 
// 	if( kMode.dwBurningMode & E_ITEM_QUANTITY_MAX )
// 	{
// 		m_mapBurningModeData.insert( BurningModeDataMap::value_type( E_ITEM_QUANTITY_MAX, EndTime ) );
// 	}
// 
// 	if( kMode.dwBurningMode & E_CONNECTION_TICK_DECREASE )
// 	{
// 		m_mapBurningModeData.insert( BurningModeDataMap::value_type( E_CONNECTION_TICK_DECREASE, EndTime ) );
// 	}
// 
// 	if( m_mapBurningModeData.empty() == true )
// 	{
// 		return false;
// 	}
// 
// 	return true;
}

bool	FLFlyffPieceDailyPayment::InitializeBurningMode()
{
	if( m_mapBurningModeData.empty() == true )
	{
		return false;
	}

	m_mapBurningModeData.clear();

	return true;
}

bool	FLFlyffPieceDailyPayment::IsActivateBurningMode( const E_BURNING_MODE eBurningMode ) const
{
	BurningModeDataMap::const_iterator itr = m_mapBurningModeData.find( eBurningMode );
	if( itr == m_mapBurningModeData.end() )
	{
		return false;
	}

// 	const time_t CurrTime	= time( NULL );
// 	const time_t EndTime	= itr->second;
// 	
// 	if( CurrTime >= EndTime )
// 	{
// 		return false;
// 	}

	return true;
}

void	FLFlyffPieceDailyPayment::SetDay( const int nDay )
{
	m_nDay	= nDay;
}

int		FLFlyffPieceDailyPayment::GetDay() const
{
	return m_nDay;
}

int		FLFlyffPieceDailyPayment::GetCurrentDay() const
{
	const CTime tmCurr	= CTime::GetCurrentTime();
	const int nCurrDay	= tmCurr.GetDayOfWeek();

	return nCurrDay;
}

void	FLFlyffPieceDailyPayment::Process( const time_t time )
{
	if( IsActivate() == false )
	{
		return;
	}

	const int nDay		= GetCurrentDay();
	if( nDay <= DAY_NONE || nDay >= DAY_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED GET DAY INFO. CONTENT DEACTIVATED. nDay:(%d) ]" ), nDay );
		Init();
		SetActivate( false );
		return;
	}

	// 요일이 바뀌면 기존 접속시간 데이터를 초기화 한다.
	if( nDay != GetDay() )
	{
		InitializeAllPlayerTick();
	}

	SetDay( nDay );

	// 버닝모드가 종료되면 시스템 메시지로 알려준다. TODO : 해당 채널에만 메시지 날려주는 패킷
	const DWORD dwEndMode = CheckBurningMode( time );

	if( dwEndMode != E_BURNING_MODE_NONE && g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
	{
// 		FLPacketBroadCastMessageNoti toCoreSrvr;
// 		toCoreSrvr.byBroadCastType	= BROADCAST_TYPE_PUTSTRING;
// 		FLStrcpy( toCoreSrvr.szMessage, _countof( toCoreSrvr.szMessage ), prj.GetText( TID_MMI_FLYFFPIECE_TEXT07 ) );
// 
// 		g_DPCoreClient.SendPacket( &toCoreSrvr );

		g_DPCoreClient.SendSystem( prj.GetText( TID_MMI_FLYFFPIECE_TEXT07 ) );
	}
}

void	FLFlyffPieceDailyPayment::OnDailyKeepConnection( FLWSUser* pUser, const DWORD dwCurrTick )
{
	if( IsValidObj( pUser ) == false || IsActivate() == false )
	{
		return;
	}

	// 해당 요일에 해당되는 이벤트 인덱스를 얻어 온다.
	const int nDay		= GetDay();

	IndexVec vecIndex;
	if( GetIndexByDay( nDay, vecIndex ) == false )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ NOT EXIST EVENT ON TODAY. nDay:(%d) ]" ), nDay );
		return;
	}

	// 체크 후 지급.
	for( IndexVec::const_iterator pos = vecIndex.begin(); pos != vecIndex.end(); ++pos )
	{
		const DWORD dwIndex	= *pos;

		T_PLAYER_INDEX_KEY kKey( pUser->m_idPlayer, dwIndex );
		if( CanDailyPayment( pUser, dwIndex, dwCurrTick ) == true && UpdatePlayerTick( kKey, dwCurrTick ) == true )
		{
			DoDailyPayment( pUser, dwIndex );
		}
	}
}

void	FLFlyffPieceDailyPayment::OnLogOut( const DWORD dwPlayerID )
{
	if( IsActivate() == false )
	{
		return;
	}

	// 해당 플레이어가 진행중이었던 이벤트 인덱스를 얻어온다.
	IndexVec vecIndex;
	if( GetIndexByPlayerID( dwPlayerID, vecIndex ) == false )
	{
		return;
	}

	// 플레이어의 해당 이벤트 초기화.
	for( IndexVec::const_iterator pos = vecIndex.begin(); pos != vecIndex.end(); ++pos )
	{
		const DWORD dwIndex = *pos;
		const T_PLAYER_INDEX_KEY kKey( dwPlayerID, dwIndex );

		ConnectionPlayerMap::iterator itr = m_mapConnectionPlayer.find( kKey );
		if( itr == m_mapConnectionPlayer.end() )
		{
			continue;
		}

		m_mapConnectionPlayer.erase( itr );
	}

	PlayerIndexMap::iterator itr = m_mapPlayerIndex.find( dwPlayerID );
	if( itr == m_mapPlayerIndex.end() )
	{
		return;
	}

	itr->second.clear();
	m_mapPlayerIndex.erase( itr );
}

bool	FLFlyffPieceDailyPayment::LoadScript( const TCHAR* pszFileName )
{
	CScript kScanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kScanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	DWORD dwLine		= 0;
	const int nMyLang	= g_xFlyffConfig->GetMainLanguage();

	DWORD dwIndex		= kScanner.GetNumber( TRUE );																		// 인덱스

	while( kScanner.tok != FINISHED )
	{
		++dwLine;

		kScanner.GetToken( TRUE );
		const std::string strStartDate		= kScanner.Token;
		const FLTm tmStartDate				= FLStringToFLTm( strStartDate.c_str(), FLTm::E_FORMAT::YYYYMMDD_HHMM );		// 시작 일자

		kScanner.GetToken( TRUE );
		const std::string strEndDate		= kScanner.Token;
		const FLTm tmEndDate				= FLStringToFLTm( strEndDate.c_str(), FLTm::E_FORMAT::YYYYMMDD_HHMM );			// 종료 일자

		const int	nLang					= kScanner.GetNumber( TRUE );													// 국가 설정

		kScanner.GetToken( TRUE );
		std::string strDay					= kScanner.Token;
		const int	nDay					= DayStringToDefine( strDay );													// 요일

		const DWORD	dwMinute				= static_cast<DWORD>( kScanner.GetNumber( TRUE ) );								// 접속시간(분단위)

		const DWORD dwItemID				= static_cast<DWORD>( kScanner.GetNumber( TRUE ) );								// 아이템 ID

		const int	nMinQuantity			= kScanner.GetNumber( TRUE );													// 최소 지급 개수

		const int	nMaxQuantity			= kScanner.GetNumber( TRUE );													// 최대 지급 개수

		const bool	bBind					= kScanner.GetNumber( TRUE ) != 0 ? true : false;								// 귀속 여부

		const int	nMinLevel				= kScanner.GetNumber( TRUE );													// 최소 지급 레벨

		const int	nMaxLevel				= kScanner.GetNumber( TRUE );													// 최대 지급 레벨

		kScanner.GetToken( TRUE );
		std::string strMinPlayerGrade		= kScanner.Token;
		const DWORD dwMinGrade				= PlayerGradeStringToDefine( strMinPlayerGrade );								// 최소 지급 클래스

		kScanner.GetToken( TRUE );
		std::string strMaxPlayerGrade		= kScanner.Token;
		const DWORD dwMaxGrade				= PlayerGradeStringToDefine( strMaxPlayerGrade );								// 최대 지급 클래스

		// 해당 국가 설정만 인서트
		if( nMyLang == nLang )
		{
			if( InsertDailyIndex( nDay, dwIndex ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DAY or INDEX. INDEX(%u), DAY(%d) ]" ), dwIndex, nDay );
				FLASSERT( 0 );
				return false;
			}

			T_PERIOD_DATA kPeriodData( tmStartDate, tmEndDate );
			if( InsertDailyPeriodData( dwIndex, kPeriodData ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PERIOD DATA. INDEX(%u), START(%I64u), END(%I64u) ]" )
					, dwIndex, kPeriodData.tmStartDate.value, kPeriodData.tmEndDate.value );
				FLASSERT( 0 );
				return false;
			}

			T_CONNECTION_DATA kConnectionData( nDay, MIN( dwMinute ) );
			if( InsertDailyConnectionData( dwIndex, kConnectionData ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID CONNECTION DATA. INDEX(%u), DAY(%d), TICK(%u) ]" )
					, dwIndex, kConnectionData.nDay, kConnectionData.dwConnectionTick );
				FLASSERT( 0 );
				return false;
			}

			T_USER_DATA kUserData( nMinLevel, nMaxLevel, dwMinGrade, dwMaxGrade );
			if( InsertDailyUserData( dwIndex, kUserData ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID USER DATA. INDEX(%u), MIN_LEVEL(%d), MAX_LEVEL(%d), MIN_GRADE(%u), MAX_GRADE(%u) ]" )
					, dwIndex, kUserData.nMinLevel, kUserData.nMaxLevel, kUserData.dwMinClass, kUserData.dwMaxClass );
				FLASSERT( 0 );
				return false;
			}

			T_ITEM_DATA kItemData( dwItemID, nMinQuantity, nMaxQuantity, bBind );
			if( InsertDailyItemData( dwIndex, kItemData ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ITEM DATA. INDEX(%u), ITEM_ID(%u), MIN_QUANTITY(%d), MAX_QUANTITY(%d), BIND(%d) ]" )
					, dwIndex, kItemData.dwItemID, kItemData.nMinQuantity, kItemData.nMaxQuantity, kItemData.bBind );
				FLASSERT( 0 );
				return false;
			}
		}

		dwIndex = kScanner.GetNumber( TRUE );			// 인덱스
	}

	return dwLine > 0 ? true : false;
}

bool	FLFlyffPieceDailyPayment::InsertDailyIndex( const int nDay, const DWORD dwIndex )
{
	if( nDay <= DAY_NONE || nDay >= DAY_MAX )
	{
		FLASSERT( 0 );
		return false;
	}

	if( dwIndex <= 0 || dwIndex == NULL_ID )
	{
		FLASSERT( 0 );
		return false;
	}

	IndexMap::iterator Itr = m_mapIndex.find( nDay );
	if( Itr != m_mapIndex.end() )
	{
		Itr->second.push_back( dwIndex );
		const bool bRet = Itr->second.empty() == false ? true : false;

		return bRet;
	}
	else
	{
		IndexVec vecIndex;
		vecIndex.push_back( dwIndex );
		const bool bRet = m_mapIndex.insert( IndexMap::value_type( nDay, vecIndex ) ).second;

		return bRet;
	}

	// 	IndexMap::const_iterator itr = m_mapIndex.find( nDay );
	// 	if( itr != m_mapIndex.end() )
	// 	{
	// 		FLASSERT( 0 );
	// 		return false;
	// 	}
	//
	//	const bool bRet = m_mapIndex.insert( IndexMap::value_type( nDay, dwIndex ) ).second;
	// 
	// 	return bRet;
}

bool	FLFlyffPieceDailyPayment::InsertDailyPeriodData( const DWORD dwIndex, const T_PERIOD_DATA & kData )
{
	if( dwIndex <= 0 )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.tmStartDate == INVALID_DATETIME || kData.tmEndDate == INVALID_DATETIME )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.tmStartDate >= kData.tmEndDate )
	{
		FLASSERT( 0 );
		return false;
	}

	const bool bRet = m_mapPeriodData.insert( PeriodDataMap::value_type( dwIndex, kData ) ).second;

	return bRet;
}

bool	FLFlyffPieceDailyPayment::InsertDailyConnectionData( const DWORD dwIndex, const T_CONNECTION_DATA & kData )
{
	if( dwIndex <= 0 )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.nDay <= DAY_NONE || kData.nDay >= DAY_MAX )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.dwConnectionTick < MIN_KEEP_CONNECTION_TICK || kData.dwConnectionTick >= MAX_KEEP_CONNECTION_TICK )
	{
		FLASSERT( 0 );
		return false;
	}

	const bool bRet = m_mapConnectionData.insert( ConnectionDataMap::value_type( dwIndex, kData ) ).second;

	return bRet;
}

bool	FLFlyffPieceDailyPayment::InsertDailyUserData( const DWORD dwIndex, const T_USER_DATA & kData )
{
	if( dwIndex <= 0 )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.nMinLevel <= 0 || kData.nMaxLevel <= 0 || kData.nMinLevel > kData.nMaxLevel )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.dwMinClass < LEGEND_CLASS_NORMAL || kData.dwMaxClass > LEGEND_CLASS_LEGEND_HERO || kData.dwMinClass > kData.dwMaxClass )
	{
		FLASSERT( 0 );
		return false;
	}

	const bool bRet = m_mapUserData.insert( UserDataMap::value_type( dwIndex, kData ) ).second;

	return bRet;
}

bool	FLFlyffPieceDailyPayment::InsertDailyItemData( const DWORD dwIndex, const T_ITEM_DATA & kData )
{
	if( dwIndex <= 0 )
	{
		FLASSERT( 0 );
		return false;
	}

	const T_ITEM_SPEC* pItemSpec = g_xSpecManager->GetSpecItem( kData.dwItemID );
	if( pItemSpec == NULL )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.nMinQuantity <= 0 || kData.nMaxQuantity <= 0 || kData.nMinQuantity > kData.nMaxQuantity )
	{
		FLASSERT( 0 );
		return false;
	}

	if( kData.nMinQuantity > LIMIT_MIN_QUANTITY || kData.nMaxQuantity > LIMIT_MAX_QUANTITY )
	{
		FLASSERT( 0 );
		return false;
	}

	const DWORD dwMinQuantity	= kData.nMaxQuantity > kData.nMinQuantity ? kData.nMaxQuantity : kData.nMinQuantity;
	if( pItemSpec->dwPackMax < dwMinQuantity )
	{
		FLASSERT( 0 );
		return false;
	}

	const bool bRet = m_mapItemData.insert( ItemDataMap::value_type( dwIndex, kData ) ).second;

	return bRet;
}

bool	FLFlyffPieceDailyPayment::CheckDailySettingData() const
{
	const bool bExistData = m_mapIndex.empty() == false
		&& m_mapPeriodData.empty() == false
		&& m_mapConnectionData.empty() == false
		&& m_mapUserData.empty() == false
		&& m_mapItemData.empty() == false;

	if( bExistData == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST DATA ]" ) );
		FLASSERT( 0 );
		return false;
	}

	for( IndexMap::const_iterator posMap = m_mapIndex.begin(); posMap != m_mapIndex.end(); ++posMap )
	{
		IndexVec vecIndex;
		vecIndex.clear();
		vecIndex.assign( posMap->second.begin(), posMap->second.end() );

		for( IndexVec::const_iterator posVec = vecIndex.begin(); posVec != vecIndex.end(); ++posVec )
		{
			const DWORD dwIndex = *posVec;

			if( m_mapPeriodData.find( dwIndex ) == m_mapPeriodData.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST INDEX PERIOD DATA. INDEX(%u) ]" ), dwIndex );
				FLASSERT( 0 );
				return false;
			}

			if( m_mapConnectionData.find( dwIndex ) == m_mapConnectionData.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST INDEX CONNECTION DATA. INDEX(%u) ]" ), dwIndex );
				FLASSERT( 0 );
				return false;
			}

			if( m_mapUserData.find( dwIndex ) == m_mapUserData.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST INDEX USER DATA. INDEX(%u) ]" ), dwIndex );
				FLASSERT( 0 );
				return false;
			}

			if( m_mapItemData.find( dwIndex ) == m_mapItemData.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST INDEX ITEM DATA. INDEX(%u) ]" ), dwIndex );
				FLASSERT( 0 );
				return false;
			}
		}
	}

	return true;
}

void	FLFlyffPieceDailyPayment::SetActivate( const bool bActivate )
{
	m_bActivate	= bActivate;
}

bool	FLFlyffPieceDailyPayment::GetIndexByDay( const int nDay, IndexVec & vecIndex ) const
{
	if( nDay <= DAY_NONE || nDay >= DAY_MAX )
	{
		return false;
	}

	IndexMap::const_iterator itr = m_mapIndex.find( nDay );
	if( itr == m_mapIndex.end() )
	{
		return false;
	}

	vecIndex.clear();
	vecIndex.assign( itr->second.begin(), itr->second.end() );

	if( vecIndex.empty() == false )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool	FLFlyffPieceDailyPayment::GetPeriodDataByIndex( const DWORD dwIndex, T_PERIOD_DATA & kData ) const
{
	PeriodDataMap::const_iterator itr = m_mapPeriodData.find( dwIndex );
	if( itr == m_mapPeriodData.end() )
	{
		return false;
	}

	kData = itr->second;

	return true;
}

bool	FLFlyffPieceDailyPayment::GetConnectionDataByIndex( const DWORD dwIndex, T_CONNECTION_DATA & kData ) const
{
	ConnectionDataMap::const_iterator itr = m_mapConnectionData.find( dwIndex );
	if( itr == m_mapConnectionData.end() )
	{
		return false;
	}

	kData						= itr->second;
	const DWORD dwDecreaseTick	= IsActivateBurningMode( E_CONNECTION_TICK_DECREASE ) == true ? DECREASE_CONNECTION_TICK : 0;

	if( kData.dwConnectionTick > dwDecreaseTick )
	{
		kData.dwConnectionTick	-= dwDecreaseTick;
		return true;
	}
	else
	{
		return false;
	}
}

bool	FLFlyffPieceDailyPayment::GetUserDataByIndex( const DWORD dwIndex, T_USER_DATA & kData ) const
{
	UserDataMap::const_iterator itr = m_mapUserData.find( dwIndex );
	if( itr == m_mapUserData.end() )
	{
		return false;
	}

	kData = itr->second;

	return true;
}

bool	FLFlyffPieceDailyPayment::GetItemDataByIndex( const DWORD dwIndex, T_ITEM_DATA & kData ) const
{
	ItemDataMap::const_iterator itr = m_mapItemData.find( dwIndex );
	if( itr == m_mapItemData.end() )
	{
		return false;
	}

	kData = itr->second;

	return true;
}

bool	FLFlyffPieceDailyPayment::GetIndexByPlayerID( const DWORD dwPlayerID, IndexVec & vecIndex ) const
{
	PlayerIndexMap::const_iterator itr = m_mapPlayerIndex.find( dwPlayerID );
	if( itr == m_mapPlayerIndex.end() )
	{
		return false;
	}

	vecIndex.clear();
	vecIndex.assign( itr->second.begin(), itr->second.end() );

	if( vecIndex.empty() == false )
	{
		return true;
	}
	else
	{
		return false;
	}
}

DWORD	FLFlyffPieceDailyPayment::GetLastTick( const T_PLAYER_INDEX_KEY & kKey ) const
{
	ConnectionPlayerMap::const_iterator itr = m_mapConnectionPlayer.find( kKey );
	if( itr == m_mapConnectionPlayer.end() )
	{
		return 0;
	}

	const DWORD dwLastTick	= itr->second;

	return dwLastTick;
}

DWORD	FLFlyffPieceDailyPayment::DayStringToDefine( std::string & strDay ) const
{
	std::transform( strDay.begin(), strDay.end(), strDay.begin(), toupper );

	if( strDay == "SUN" )
	{
		return DAY_SUN;
	}
	else if( strDay == "MON" )
	{
		return DAY_MON;
	}
	else if( strDay == "TUE")
	{
		return DAY_TUE;
	}
	else if( strDay == "WED")
	{
		return DAY_WED;
	}
	else if( strDay == "THU")
	{
		return DAY_THU;
	}
	else if( strDay == "FRI")
	{
		return DAY_FRI;
	}
	else if( strDay == "SAT")
	{
		return DAY_SAT;
	}
	else
	{
		return DAY_NONE;
	}
}

DWORD	FLFlyffPieceDailyPayment::PlayerGradeStringToDefine( std::string & strPlayerGrade ) const
{
	std::transform( strPlayerGrade.begin(), strPlayerGrade.end(), strPlayerGrade.begin(), toupper );

	if( strPlayerGrade == "NORMAL" )
	{
		return LEGEND_CLASS_NORMAL;
	}
	else if( strPlayerGrade == "MASTER" )
	{
		return LEGEND_CLASS_MASTER;
	}
	else if( strPlayerGrade == "HERO")
	{
		return LEGEND_CLASS_HERO;
	}
	else if( strPlayerGrade == "LEGEND_HERO")
	{
		return LEGEND_CLASS_LEGEND_HERO;
	}
	else
	{
		return NULL_ID;
	}
}

bool	FLFlyffPieceDailyPayment::IsDailyPaymentPeriod( const T_PERIOD_DATA & kData ) const
{
	const FLTm tmCurr	= DATE_TIMER().GetCurrentTime();

	const bool bCan		= tmCurr >= kData.tmStartDate && tmCurr <= kData.tmEndDate ? true : false;

	return bCan;
}

bool	FLFlyffPieceDailyPayment::IsDailyPaymentUser( FLWSUser* pUser, const T_USER_DATA & kData ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	const DWORD dwUserClass		= pUser->GetLegendChar();
	if( dwUserClass < kData.dwMinClass || dwUserClass > kData.dwMaxClass )
	{
		return false;
	}

	const int nUserLevel		= pUser->GetLevel();
	if( nUserLevel < kData.nMinLevel || nUserLevel > kData.nMaxLevel )
	{
		return false;
	}

	return true;
}

bool	FLFlyffPieceDailyPayment::CanDailyPayment( FLWSUser* pUser, const DWORD dwIndex, const DWORD dwCurrTick )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( dwIndex <= 0 || dwCurrTick <= 0 )
	{
		return false;
	}

	T_PERIOD_DATA kPeriodData;
	if( GetPeriodDataByIndex( dwIndex, kPeriodData ) == false )
	{
		return false;
	}

	if( IsDailyPaymentPeriod( kPeriodData ) == false )
	{
		return false;
	}

	T_CONNECTION_DATA kConnectionData;
	if( GetConnectionDataByIndex( dwIndex, kConnectionData ) == false )
	{
		return false;
	}

	T_USER_DATA kUserData;
	if( GetUserDataByIndex( dwIndex, kUserData ) == false )
	{
		return false;
	}

	if( IsDailyPaymentUser( pUser, kUserData ) == false )
	{
		return false;
	}

	T_ITEM_DATA kItemData;
	if( GetItemDataByIndex( dwIndex, kItemData ) == false )
	{
		return false;
	}

	const T_PLAYER_INDEX_KEY kKey( pUser->m_idPlayer, dwIndex );
	if( IsExistPlayerTick( kKey ) == true )
	{
		const DWORD dwLastTick	= GetLastTick( kKey );
		if( dwLastTick == 0 || dwLastTick + kConnectionData.dwConnectionTick > dwCurrTick )
		{
			return false;
		}
	}
	else
	{
		InsertPlayerTick( kKey, dwCurrTick );
		return false;
	}

	return true;
}

bool	FLFlyffPieceDailyPayment::DoDailyPayment( FLWSUser* pUser, const DWORD dwIndex ) const
{
	T_CONNECTION_DATA kConnectionData;
	if( GetConnectionDataByIndex( dwIndex, kConnectionData ) == false )
	{
		return false;
	}

	T_ITEM_DATA kItemData;
	if( GetItemDataByIndex( dwIndex, kItemData ) == false )
	{
		return false;
	}

	const T_ITEM_SPEC* pItemSpec	= g_xSpecManager->GetSpecItem( kItemData.dwItemID );
	if( pItemSpec == NULL )
	{
		return false;
	}

	FLItemElem kItemElem;
	kItemElem.m_dwItemId	= kItemData.dwItemID;
	kItemElem.m_nItemNum	= IsActivateBurningMode( E_ITEM_QUANTITY_MAX ) == true ? kItemData.nMaxQuantity : kItemData.nMinQuantity;
	kItemElem.m_byFlag		= static_cast<BYTE>( kItemData.bBind == true ? FLItemElem::binds : FLItemElem::normal );
	kItemElem.SetSerialNumber();

	//////////////////////////////////////////////////////////////////////////
	// send post
	const DWORD dwMinute	= kConnectionData.dwConnectionTick / ( 60 * 1000 );
	TCHAR szMailTitle[MAX_MAILTITLE]	= { 0, };
	FLSPrintf( szMailTitle, _countof( szMailTitle ), prj.GetText( TID_MMI_FLYFFPIECE_POST_TITLE ) );

	TCHAR szMailText[MAX_MAILTEXT]		= { 0, };
	FLSPrintf( szMailText, _countof( szMailText ), prj.GetText( TID_MMI_FLYFFPIECE_POST_TEXT01 ), dwMinute, pItemSpec->szName );

	g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, NULL_PLAYER_ID, kItemElem, 0, szMailTitle, szMailText );

	g_DPSrvr.PutItemLog( pUser, "E", "PAY_DAILY_EVENT", &kItemElem, kItemElem.m_nItemNum );
	//////////////////////////////////////////////////////////////////////////

	pUser->AddDefinedText( TID_MMI_FLYFFPIECE_TEXT04, "\"%s\"", pItemSpec->szName );

	return true;
}

bool	FLFlyffPieceDailyPayment::IsExistPlayerTick( const T_PLAYER_INDEX_KEY & kKey ) const
{
	ConnectionPlayerMap::const_iterator itr = m_mapConnectionPlayer.find( kKey );
	if( itr != m_mapConnectionPlayer.end() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool	FLFlyffPieceDailyPayment::InsertPlayerTick( const T_PLAYER_INDEX_KEY & kKey, const DWORD dwCurrTick )
{
	if( kKey.dwIndex <= 0 || dwCurrTick <= 0 )
	{
		return false;
	}

	if( m_mapConnectionPlayer.insert( ConnectionPlayerMap::value_type( kKey, dwCurrTick ) ).second == false )
	{
		return false;
	}

	PlayerIndexMap::iterator itr = m_mapPlayerIndex.find( kKey.dwPlayerID );
	if( itr == m_mapPlayerIndex.end() )
	{
		IndexVec vecIndex;
		vecIndex.push_back( kKey.dwIndex );
		if( m_mapPlayerIndex.insert( PlayerIndexMap::value_type( kKey.dwPlayerID, vecIndex ) ).second == false )
		{
			ConnectionPlayerMap::iterator itr = m_mapConnectionPlayer.find( kKey );
			if( itr != m_mapConnectionPlayer.end() )
			{
				m_mapConnectionPlayer.erase( itr );
			}
			return false;
		}
	}
	else
	{
		itr->second.push_back( kKey.dwIndex );
	}

	return true;
}

bool	FLFlyffPieceDailyPayment::UpdatePlayerTick( const T_PLAYER_INDEX_KEY & kKey, const DWORD dwCurrTick )
{
	if( kKey.dwIndex <= 0 || dwCurrTick <= 0 )
	{
		return false;
	}

	ConnectionPlayerMap::iterator itr = m_mapConnectionPlayer.find( kKey );
	if( itr == m_mapConnectionPlayer.end() )
	{
		return false;
	}

//	m_mapConnectionPlayer.erase( itr );
	itr->second		= dwCurrTick;

	return true;
}

void	FLFlyffPieceDailyPayment::InitializeAllPlayerTick()
{
	for( PlayerIndexMap::iterator pos = m_mapPlayerIndex.begin(); pos != m_mapPlayerIndex.end(); ++pos )
	{
		pos->second.clear();
	}
	m_mapPlayerIndex.clear();
	m_mapConnectionPlayer.clear();
}

DWORD	FLFlyffPieceDailyPayment::CheckBurningMode( const time_t time )
{
	UNREFERENCED_PARAMETER( time );

	return E_BURNING_MODE_NONE;

// 	DWORD dwEndMode	= E_BURNING_MODE_NONE;
// 
// 	if( m_mapBurningModeData.empty() == true )
// 	{
// 		return dwEndMode;
// 	}
// 
// 	if( IsActivate() == false )
// 	{
// 		m_mapBurningModeData.clear();
// 		return E_MODE_ALL;
// 	}
// 
// 	for( BurningModeDataMap::iterator pos = m_mapBurningModeData.begin(); pos != m_mapBurningModeData.end();  )
// 	{
// 		const time_t EndTime = pos->second;
// 		if( EndTime <= time )
// 		{
// 			dwEndMode	|=	pos->first;
// 			m_mapBurningModeData.erase( pos++ );
// 		}
// 		else
// 		{
// 			++pos;
// 		}
// 	}
// 
// 	return dwEndMode;
}

}	// namespace nsFlyffPieceDailyPayment
