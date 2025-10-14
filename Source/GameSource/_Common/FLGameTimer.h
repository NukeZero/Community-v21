#ifndef	__FLGAMETIMER_H__
#define	__FLGAMETIMER_H__


#include "../_CommonDefine/FLTm.h"
//#include "../_CommonDefine/FLSingleton.h"

//class FLGameTimer : public FLSingleton< FLGameTimer >
//{
//public:
//	FLGameTimer() : m_wMultiple( 1 )		{}
//
//	void					Initialize( const BYTE byMultiple )
//	{
////#undef max
//		m_oldDay				= -1;
//		m_wMultiple			= max( ( BYTE )1, byMultiple );
//	}
//
//	void					Update()
//	{
//		const __time64_t t		= ::_time64( NULL );
//		const struct tm * tm	= ::_localtime64( &t );
//		const long nRealSecond	= FLTimeToSecond( tm->tm_hour, tm->tm_min, tm->tm_sec );
//		const long nGameSecond	= nRealSecond * m_wMultiple;
//
//		const long day			= nGameSecond / 60 / 60 / 24;
//		if( m_oldDay != day )
//		{
//			++m_accumulated_day;
//			m_oldDay			= day;
//		}
//
//		//m_accumulated_day		= //nGameSecond / 60 / 60 / 24;
//		m_hour					= nGameSecond / 60 / 60 % 24;
//		m_minute				= nGameSecond / 60 % 60;
//		m_second				= nGameSecond % 60;
//	}
//
//	int						GetTotDay() const		{	return m_accumulated_day;		}
//	int						GetHour() const			{	return m_accumulated_day;		}
//	int						GetMiniute() const		{	return m_accumulated_day;		}
//	int						GetSecond() const		{	return m_accumulated_day;		}
//
//private:
//	WORD					m_wMultiple;
//	int						m_oldDay;
//
//	int						m_accumulated_day;	
//	int						m_hour;	
//	int						m_minute;
//	int						m_second;
//};
//
//inline static	FLGameTimer &		GAME_TIMER()		{		return FLGameTimer::GetInstance();	}


//neodeath comment :  template으로 하는게 나을려나
//class FLGameTimerCommon
//{
//public:
//	struct FLGameTime
//	{
//		FLGameTime() : wAccumulatedDay( 1 ), time( INVALID_DATETIME )		{}	//, hour( 0 ), minute( 0 ), second( 0 )		{}
//		~FLGameTime()	{}
//
//		bool operator == ( const FLGameTime & r	) const		{	return ( wAccumulatedDay == r.wAccumulatedDay && time < r.time );  }
//		bool operator != ( const FLGameTime & r	) const		{	return ( wAccumulatedDay != r.wAccumulatedDay || time != r.time );  }
//		bool operator > ( const FLGameTime & r ) const		{	return ( wAccumulatedDay > r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time > r.time );  }	
//		bool operator >= ( const FLGameTime & r ) const		{	return ( wAccumulatedDay > r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time >= r.time );  }	
//		bool operator < ( const FLGameTime & r ) const		{	return ( wAccumulatedDay < r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time < r.time );  }
//		bool operator <= ( const FLGameTime & r ) const 	{	return ( wAccumulatedDay < r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time <= r.time );  }
//		
//		WORD					wAccumulatedDay;	
//		FLTm					time;
//	};
//
//public:
//	FLGameTimerCommon();
//	virtual ~FLGameTimerCommon();
//
//	WORD					GetMultiple() const;
//
//	void					Initialize( const WORD wMultiple, const __time64_t tCurr );
//	FLGameTime				GetCurrentGameTime();
//
//
//	static void				GameSecondToGameDateTime( FLGameTime & o_GameTime, const __int64 nnGameSecond );
//	static long				ToGameSecond( const FLGameTime & GameTime );
//	static void				AddSecond( FLGameTime & o_GameTime, const __int64 nnSecond );
//
//protected: 
//	void					gameSecondToGameTime( FLGameTime & o_GameTime, const __int64 nnGameTick );
//
//protected:
//	WORD					m_wMultiple;
//	
//	DWORD					m_dwStartTick;		// real tick
//	FLGameTime				m_StartGameTime;	// game time	
//};
//
//
//class FLGameTimer_Server : public FLGameTimerCommon, public FLSingleton< FLGameTimer_Server >
//{
//public:
//	FLGameTimer_Server();
//	virtual ~FLGameTimer_Server();
//	void					Initialize( const WORD wMultiple );
//
//	FLTm					ToRealTime( const FLGameTime & gameTime );
//
//private:
//	__time64_t				m_tStart;					// ToRealTime 용		OnlyServer;
//};
//
//inline static	FLGameTimer_Server &		GAME_TIMER()		{		return FLGameTimer_Server::GetInstance();	}
//
//
//class FLGameTimer_Client : public FLGameTimerCommon
//{
//public:
//	FLGameTimer_Client();
//	virtual ~FLGameTimer_Client();
//	void					Initialize( const WORD wMultiple, FLGameTimerCommon::FLGameTime & CurrGameTime );
//};
//


//
////---------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


class FLGameTimerCommon //: public FLSingleton< FLGameTimerCommon >
{
public:
	enum					{ TIME_SPEED_MULTIPLE	= 6,		};				//6배속 시간

	struct FLGameTime
	{
		FLGameTime() : wAccumulatedDay( 1 ), time( INVALID_DATETIME )		{}	//, hour( 0 ), minute( 0 ), second( 0 )		{}
		~FLGameTime()	{}

		bool operator == ( const FLGameTime & r	) const		{	return ( wAccumulatedDay == r.wAccumulatedDay && time < r.time );  }
		bool operator != ( const FLGameTime & r	) const		{	return ( wAccumulatedDay != r.wAccumulatedDay || time != r.time );  }
		bool operator > ( const FLGameTime & r ) const		{	return ( wAccumulatedDay > r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time > r.time );  }	
		bool operator >= ( const FLGameTime & r ) const		{	return ( wAccumulatedDay > r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time >= r.time );  }	
		bool operator < ( const FLGameTime & r ) const		{	return ( wAccumulatedDay < r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time < r.time );  }
		bool operator <= ( const FLGameTime & r ) const 	{	return ( wAccumulatedDay < r.wAccumulatedDay ) || ( wAccumulatedDay == r.wAccumulatedDay && time <= r.time );  }

		WORD					wAccumulatedDay;	
		FLTm					time;
	};

public:
	FLGameTimerCommon();
	virtual ~FLGameTimerCommon();

	static FLGameTimerCommon &		GetInstance()	
	{
		static FLGameTimerCommon inst;
		return inst;
	}


//	WORD					GetMultiple() const;

//	void					Initialize( const WORD wMultiple, const __time64_t tCurr );
	void					Initialize( /*const WORD wMultiple, */FLGameTimerCommon::FLGameTime & CurrGameTime );
	FLGameTime				GetCurrentGameTime();


	static void				GameSecondToGameDateTime( FLGameTime & o_GameTime, const __int64 nnGameSecond );
	static long				ToGameSecond( const FLGameTime & GameTime );
	static void				AddSecond( FLGameTime & o_GameTime, const __int64 nnSecond );

	FLTm					ToRealTime( const FLGameTime & gameTime );

protected: 
	void					gameSecondToGameTime( FLGameTime & o_GameTime, const __int64 nnGameTick );

protected:
	//WORD					m_wMultiple;

	FLTm					m_tmStart;					// ToRealTime 용	// 리얼 시간 진행 계산용.
	FLGameTime				m_StartGameTime;			// ToRealTime 용	// 게임 시간 진행 계산용.
	DWORD					m_dwStartTick;
};

inline static	FLGameTimerCommon &		GAME_TIMER()		{		return FLGameTimerCommon::GetInstance(); }

//
//
//class FLGameTimer_Server : public FLGameTimerCommon, public FLSingleton< FLGameTimer_Server >
//{
//public:
//	FLGameTimer_Server();
//	virtual ~FLGameTimer_Server();
//	//void					Initialize( const WORD wMultiple );
//};
//
//
//class FLGameTimer_Client : public FLGameTimerCommon, public FLSingleton< FLGameTimer_Client >
//{
//public:
//	FLGameTimer_Client();
//	virtual ~FLGameTimer_Client();
//
//	
//};
//
//#ifdef __CLIENT
//	inline static	FLGameTimer_Client &		GAME_TIMER()		{		return FLGameTimer_Client::GetInstance(); }
//#else 
//	inline static	FLGameTimer_Server &		GAME_TIMER()		{		return FLGameTimer_Server::GetInstance();	}
//#endif 


//#include "../_CommonDefine/FLTm.h"
//
//class FLDateTimeWithGameTime
//{
//public:
//	FLDateTimeWithGameTime() : m_tm( INVALID_DATETIME )	{}
//	virtual ~FLDateTimeWithGameTime()					{}
//
//	bool operator == ( const FLDateTimeWithGameTime & r	) const		{	return ( m_tm == r.m_tm && m_GameTime < r.m_GameTime );  }
//	bool operator != ( const FLDateTimeWithGameTime & r	) const		{	return ( m_tm != r.m_tm || m_GameTime != r.m_GameTime );  }
//	bool operator > ( const FLDateTimeWithGameTime & r ) const		{	return ( m_tm > r.m_tm ) || ( m_tm == r.m_tm && m_GameTime > r.m_GameTime );  }	
//	bool operator >= ( const FLDateTimeWithGameTime & r ) const		{	return ( m_tm > r.m_tm ) || ( m_tm == r.m_tm && m_GameTime >= r.m_GameTime );  }	
//	bool operator < ( const FLDateTimeWithGameTime & r ) const		{	return ( m_tm < r.m_tm ) || ( m_tm == r.m_tm && m_GameTime < r.m_GameTime );  }
//	bool operator <= ( const FLDateTimeWithGameTime & r ) const 	{	return ( m_tm < r.m_tm ) || ( m_tm == r.m_tm && m_GameTime <= r.m_GameTime );  }
//
//private:
//	FLTm					m_tm;
//	FLGameTimer::FLGameTime	m_GameTime;
//};
//
////---------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//
//#include "../_CommonDefine/FLSingleton.h"
//
//class FLGameTimeAlarmClock : public FLAlarmClock< FLDateTimeWithGameTime >, public FLSingleton< FLGameTimeAlarmClock >
//{};
//
//inline static		FLGameTimeAlarmClock & GAMETIME_ALARMCLOCK()		{ return FLGameTimeAlarmClock::GetInstance();	}
//
////---------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//



#endif			//__FLGAMETIMER_H__