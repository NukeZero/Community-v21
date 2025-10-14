#ifndef __FLPERIODREPETITIONALARM_H__
#define __FLPERIODREPETITIONALARM_H__


#include "FLAlarmClock.hpp"
#include "FLGameTimer.h"

//단점 : 상속 받은 객체가 m_startDate 등 맴버를 함부로 건드려서는 안된다.

//class IPeriodRepetitionAlarm : public FLDateTimeAlarmClock::IAlarm
//{
//public:
//	virtual BOOL			Initialize( const FLTm startDate, const FLTm endDate, const FLTm startTime, const FLTm endTime ) = 0;		
//
//	virtual BOOL			Start() = 0;
//
//	virtual BOOL			OnStart() = 0;			//return FALSE면 다음 End를 호출하지 않음
//	virtual BOOL			OnEnd() = 0;			//return FALSE면 다음 Start를 호출하지 않음
//	virtual void			OnExpired() = 0;
//	virtual void			OnError() = 0;
//
//protected:
//	FLTm					getStartDate() const;
//	FLTm					getStartTime() const;
//	FLTm					getEndDate() const;
//	FLTm					getEndTime() const;
//
//protected:
//	FLTm					m_startDate;					
//	FLTm					m_endDate;
//	FLTm					m_startTime;			
//	FLTm					m_endTime;
//};

class IPeriodRepetitionAlarm
{
public:
	virtual BOOL			Initialize( const FLTm startDate, const FLTm endDate, const FLTm startTime, const FLTm endTime ) = 0;		

	virtual BOOL			Start() = 0;

	virtual BOOL			OnStart() = 0;			//return FALSE면 다음 End를 호출하지 않음
	virtual BOOL			OnEnd() = 0;			//return FALSE면 다음 Start를 호출하지 않음
	virtual void			OnExpired() = 0;
	virtual void			OnError() = 0;

	virtual void			OnAlarm() = 0;

	void					Update( const FLTm kCurrTm );

protected:
	FLTm					getStartDate() const;
	FLTm					getStartTime() const;
	FLTm					getEndDate() const;
	FLTm					getEndTime() const;

protected:
	FLTm					m_startDate;					
	FLTm					m_endDate;
	FLTm					m_startTime;			
	FLTm					m_endTime;

	FLTm					m_checkTime;
};




class FLPeriodRepetitionAlarm_RealDateTime : public IPeriodRepetitionAlarm			//기간제 알람
{
public:
	FLPeriodRepetitionAlarm_RealDateTime();
	virtual ~FLPeriodRepetitionAlarm_RealDateTime();
	
	virtual BOOL			Initialize( const FLTm startDate, const FLTm endDate, const FLTm startTime, const FLTm endTime );
	virtual BOOL			Start();
	virtual void			OnAlarm();

private:
	FLTm					getStartAlarm_RealTime( const FLTm & tmCurr );		//시작될 datetime이나 시작된 datetime을 가져옴.;
	BOOL					isExpired( const FLTm & tmCurr );

private:
	BOOL					m_bStartProcess;
	//FLTm					m_checkTime;
	long					m_tTimeDistSecond;
};




class FLPeriodRepetitionAlarm_RealDateGameTime : public IPeriodRepetitionAlarm			//기간제 알람
{
public:
	FLPeriodRepetitionAlarm_RealDateGameTime();				//기존과 거의 같다.;
	virtual ~FLPeriodRepetitionAlarm_RealDateGameTime();

	virtual BOOL			Initialize( const FLTm startDate, const FLTm endDate, const FLTm startGameTime, const FLTm endGameTime );		//기존과 같다.;


	virtual BOOL			Start();
	virtual void			OnAlarm();

	virtual BOOL			OnStart() = 0;			//return FALSE면 다음 End를 호출하지 않음
	virtual BOOL			OnEnd() = 0;			//return FALSE면 다음 Start를 호출하지 않음
	virtual void			OnExpired() = 0;
	virtual void			OnError() = 0;

private:
	BOOL					isExpired( const FLTm & tmRealCurr );
	BOOL					getStartAlarm_GameDateTime( FLGameTimerCommon::FLGameTime & o_realStartGameTime, const FLGameTimerCommon::FLGameTime & tmGameCurr );	//시작될 game datetime이나 시작된 game datetime을 가져옴.;

private:
	BOOL					m_bStartProcess;
	FLGameTimerCommon::FLGameTime	m_AlarmStartGameTime;
	long					m_tTimeDistGameSecond;			//gametime
};

//---------------------------------------------------------------------------------------------------------------------------------------//

class FLPeriodRepetitionAlarm_Mng// : public FLSingleton< FLPeriodRepetitionAlarm_Mng >
{
public:
	FLPeriodRepetitionAlarm_Mng();
	virtual ~FLPeriodRepetitionAlarm_Mng();

	static FLPeriodRepetitionAlarm_Mng & GetInstance();

	void					LoadRandomRespawn();
	void					Start();
	void					Update();

private:
	typedef					std::vector< IPeriodRepetitionAlarm * >		VecIAlarm;
	VecIAlarm				m_vecIAlarm;
};

inline static FLPeriodRepetitionAlarm_Mng &		PERIOD_REPETITION_ALARM_MNG()	{	return FLPeriodRepetitionAlarm_Mng::GetInstance();	}



#endif //__FLPERIODREPETITIONALARM_H__