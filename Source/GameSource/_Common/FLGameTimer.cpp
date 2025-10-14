#include "stdafx.h"
#include "FLGameTimer.h"

#undef max
//
//FLGameTimerCommon::FLGameTimerCommon() 
//: m_wMultiple( 1 )
//, m_dwStartTick( 0 )
//{
//}
//
//FLGameTimerCommon::~FLGameTimerCommon()
//{
//}
//
//WORD FLGameTimerCommon::GetMultiple() const
//{
//	return m_wMultiple;
//}
//
//FLGameTimerCommon::FLGameTime FLGameTimerCommon::GetCurrentGameTime()			
//{
//	const __int64 nnDistGameTick	= ( ::GetTickCount() - m_dwStartTick ) * ( __int64 )m_wMultiple;
//
//	////#ifdef _DEBUG
//	////	const __time64_t t		= ::_time64( NULL );
//	////	const struct tm * tm	= ::_localtime64( &t );
//	////#endif //_DEBUG
//
//	FLGameTime ret			= m_StartGameTime;
//	AddSecond( ret, nnDistGameTick / 1000 );
//	return ret;
//}
//
//void FLGameTimerCommon::GameSecondToGameDateTime( FLGameTime & o_GameTime, const __int64 nnGameSecond )
//{
//	o_GameTime.wAccumulatedDay	= ( WORD )( nnGameSecond / 60 / 60 / 24 );
//	o_GameTime.time.value		= 0;
//	o_GameTime.time.hour		= ( WORD )( nnGameSecond / 60 / 60 % 24 );
//	o_GameTime.time.minute		= ( WORD )( nnGameSecond / 60 % 60 );
//	o_GameTime.time.second		= ( WORD )( nnGameSecond % 60 );
//}
//
//long FLGameTimerCommon::ToGameSecond( const FLGameTime & GameTime )
//{
//	return ( long )( GameTime.wAccumulatedDay * FLTm::ONE_DAY_SECOND 
//		+ GameTime.time.hour * FLTm::ONE_HOUR_SECOND
//		+ GameTime.time.minute * FLTm::ONE_MINUTE_SECOND
//		+ GameTime.time.second );
//}
//
//void FLGameTimerCommon::AddSecond( FLGameTime & o_GameTime, const __int64 nnSecond )
//{
//	const __int64 nTotGameSecond	= ToGameSecond( o_GameTime ) + nnSecond;
//	GameSecondToGameDateTime( o_GameTime, nTotGameSecond );
//}
//
////------------------------------------------------------------------------------------------------------------------------------------------//
//
//
//FLGameTimer_Server::FLGameTimer_Server()
//: m_tStart( 0 )
//{
//
//}
//
//FLGameTimer_Server::~FLGameTimer_Server()
//{
//
//}
//
//void		FLGameTimer_Server::Initialize( const WORD wMultiple )
//{
//	m_wMultiple				= wMultiple;
//	m_tStart				= ::_time64( NULL );
//	m_dwStartTick			= ::GetTickCount();
//	const struct tm * tm	= ::_localtime64( &m_tStart );
//	const long nGameSecond	= FLTimeToSecond( tm->tm_hour, tm->tm_min, tm->tm_sec ) * m_wMultiple;			//오차 ( 999 millisecond * 6 )
//	GameSecondToGameDateTime( m_StartGameTime, nGameSecond );
//	//m_nnStartGameTick		= nGameSecond * 
//}
//
//FLTm		FLGameTimer_Server::ToRealTime( const FLGameTime & gameTime )	//배수만큼의 오차 ㄷㄷ
//{
//	const long nGameSecond		= ToGameSecond( gameTime );
//	const long nStartGameSecond = ToGameSecond( m_StartGameTime );
//
//	const long nDiffRealSecond	= ( nGameSecond - nStartGameSecond ) / m_wMultiple;
//
//	const __time64_t tReal		= m_tStart + nDiffRealSecond;
//	FLTm tm;
//	if( FLTimeTToDateTime( tm, tReal ) == FALSE )
//		return INVALID_DATETIME;
//	return tm;
//}
//
////------------------------------------------------------------------------------------------------------------------------------------------//
//FLGameTimer_Client::FLGameTimer_Client()
//{
//}
//
//FLGameTimer_Client::~FLGameTimer_Client()
//{
//}
//
//void FLGameTimer_Client::Initialize( const WORD wMultiple, FLGameTimerCommon::FLGameTime & CurrGameTime )
//{
//	m_wMultiple				= wMultiple;
//	m_dwStartTick			= ::GetTickCount();
//	m_StartGameTime			= CurrGameTime;
//}
//
////------------------------------------------------------------------------------------------------------------------------------------------//


FLGameTimerCommon::FLGameTimerCommon() 
//: m_wMultiple( 1 )
: m_tmStart( INVALID_DATETIME )
{
	m_tmStart				= DATE_TIMER().GetCurrentTime();
	const long nGameSecond	= FLTimeToSecond( m_tmStart.hour, m_tmStart.minute, m_tmStart.second ) * TIME_SPEED_MULTIPLE;			//오차 ( 999 millisecond * 6 )
	GameSecondToGameDateTime( m_StartGameTime, nGameSecond );
	AddSecond( m_StartGameTime, FLTm::ONE_DAY_SECOND );			// 게임 시간은 1일 부터 시작

	m_dwStartTick			= ::GetTickCount();
}

FLGameTimerCommon::~FLGameTimerCommon()
{
}

//WORD FLGameTimerCommon::GetMultiple() const
//{
//	return m_wMultiple;
//}

void FLGameTimerCommon::Initialize( FLGameTimerCommon::FLGameTime & CurrGameTime )
{
	//m_wMultiple				= wMultiple;

	m_tmStart				= DATE_TIMER().GetCurrentTime();
	m_StartGameTime			= CurrGameTime;

	m_dwStartTick			= ::GetTickCount();
}


FLGameTimerCommon::FLGameTime FLGameTimerCommon::GetCurrentGameTime()			
{
#if !defined( __CLIENT )
	const FLTm tmCurr		= DATE_TIMER().GetCurrentTime();

	// 진행된 초시간 구하기.
	__time64_t tStartSecond;
	BOOL bRet				= FLDateTimeToTimeT( tStartSecond, m_tmStart );
	RELEASE_ASSERT( bRet == TRUE );		//@@@@@@@@@@@@@@@@
	__time64_t tCurrSecond;	
	bRet					= FLDateTimeToTimeT( tCurrSecond, tmCurr );
	RELEASE_ASSERT( bRet == TRUE );		//@@@@@@@@@@@@@@@@

	// Game 시간 계산
	FLGameTime ret			= m_StartGameTime;
	AddSecond( ret, ( tCurrSecond - tStartSecond ) * TIME_SPEED_MULTIPLE );
#else
	const DWORD dwDistTick	= ::GetTickCount() - m_dwStartTick;			

	// Game 시간 계산
	FLGameTime ret			= m_StartGameTime;
	AddSecond( ret, ( ( __int64 )dwDistTick * TIME_SPEED_MULTIPLE * DATE_TIMER().GetMultiple() ) / 1000 );
#endif //__CLIENT

	return ret;
}

void FLGameTimerCommon::GameSecondToGameDateTime( FLGameTime & o_GameTime, const __int64 nnGameSecond )
{
	o_GameTime.wAccumulatedDay	= ( WORD )( nnGameSecond / 60 / 60 / 24 );
	o_GameTime.time.value		= 0;
	o_GameTime.time.hour		= ( WORD )( nnGameSecond / 60 / 60 % 24 );
	o_GameTime.time.minute		= ( WORD )( nnGameSecond / 60 % 60 );
	o_GameTime.time.second		= ( WORD )( nnGameSecond % 60 );
}

long FLGameTimerCommon::ToGameSecond( const FLGameTime & GameTime )
{
	return ( long )( GameTime.wAccumulatedDay * FLTm::ONE_DAY_SECOND 
		+ GameTime.time.hour * FLTm::ONE_HOUR_SECOND
		+ GameTime.time.minute * FLTm::ONE_MINUTE_SECOND
		+ GameTime.time.second );
}

void FLGameTimerCommon::AddSecond( FLGameTime & o_GameTime, const __int64 nnSecond )
{
	const __int64 nTotGameSecond	= ToGameSecond( o_GameTime ) + nnSecond;
	GameSecondToGameDateTime( o_GameTime, nTotGameSecond );
}


FLTm		FLGameTimerCommon::ToRealTime( const FLGameTime & gameTime )	//변경한것을 또 변경하고하면  나누기 배수만큼의 오차
{
	// 시작된 time_t 구하기
	__time64_t tStartSecond;
	const BOOL bRet				= FLDateTimeToTimeT( tStartSecond, m_tmStart );
	RELEASE_ASSERT( bRet == TRUE );		//@@@@@@@@@@@@@@@@

	// 진행된 시간 계산
	const long nGameSecond		= ToGameSecond( gameTime );
	const long nStartGameSecond = ToGameSecond( m_StartGameTime );

	const long nDiffRealSecond	= ( nGameSecond - nStartGameSecond ) / TIME_SPEED_MULTIPLE;

	// 실제 시간 계산.
	const __time64_t tReal		= tStartSecond + nDiffRealSecond;
	FLTm tm;
	if( FLTimeTToDateTime( tm, tReal ) == FALSE )
		return INVALID_DATETIME;
	return tm;
}

//------------------------------------------------------------------------------------------------------------------------------------------//

//
//FLGameTimer_Server::FLGameTimer_Server()
////: m_tmStart( INVALID_DATETIME )
//{
//	//m_tmStart				= DATE_TIMER().GetCurrentTime();
//	//const long nGameSecond	= FLTimeToSecond( m_tmStart.hour, m_tmStart.minute, m_tmStart.second ) * TIME_SPEED_MULTIPLE;			//오차 ( 999 millisecond * 6 )
//	//GameSecondToGameDateTime( m_StartGameTime, nGameSecond );
//	//AddSecond( m_StartGameTime, FLTm::ONE_DAY_SECOND );			// 게임 시간은 1일 부터 시작
//}
//
//FLGameTimer_Server::~FLGameTimer_Server()
//{
//
//}
//
////void		FLGameTimer_Server::Initialize( const WORD wMultiple )
////{
////	//m_wMultiple				= wMultiple;
////	//m_tStart				= ::_time64( NULL );
////	//m_dwStartTick			= ::GetTickCount();
////	//const struct tm * tm	= ::_localtime64( &m_tStart );
////	//const long nGameSecond	= FLTimeToSecond( tm->tm_hour, tm->tm_min, tm->tm_sec ) * m_wMultiple;			//오차 ( 999 millisecond * 6 )
////	//GameSecondToGameDateTime( m_StartGameTime, nGameSecond );
////
////
////	//m_wMultiple				= wMultiple;
////
////}
//
//
////------------------------------------------------------------------------------------------------------------------------------------------//
//FLGameTimer_Client::FLGameTimer_Client()
//{
//}
//
//FLGameTimer_Client::~FLGameTimer_Client()
//{
//}


//------------------------------------------------------------------------------------------------------------------------------------------//

