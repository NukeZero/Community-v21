#ifndef __FLALARMCLOCK_H__
#define __FLALARMCLOCK_H__

#include "../_CommonDefine/FLPriorityQueue.hpp"

//단점 : //같은 객체를 한 컨테이너에 두번 등록 할수 없게 된다

template < class _Key >
class FLAlarmClock
{
public:
	class IAlarm : public FLPriorityQueue< _Key, IAlarm * >::FLNode			
	{
	public:
	//	IAlarm() {}
	//	virtual ~IAlarm() {}

		virtual void		OnAlarm() = 0;
	};

public:
	FLAlarmClock();
	virtual ~FLAlarmClock();

	BOOL					Regist( const _Key & key, IAlarm * pAlarm );
	BOOL					Unregist( IAlarm * pAlarm );

	void					Update( const _Key & currTime );

private:
	FLPriorityQueue< _Key, IAlarm * > m_PriorityQ;
};

//---------------------------------------------------------------------------------------------------------------------------//


#include "../_CommonDefine/FLTm.h"
#include "../_CommonDefine/FLSingleton.h"
class FLDateTimeAlarmClock : public FLAlarmClock< FLTm >//, public FLSingleton< FLDateTimeAlarmClock >
{
public:
	static FLDateTimeAlarmClock & GetInstance()
	{
		static FLDateTimeAlarmClock inst;
		return inst;
	}

};

inline static			FLDateTimeAlarmClock & DATETIME_ALARMCLOCK()		{	return FLDateTimeAlarmClock::GetInstance();	}

//---------------------------------------------------------------------------------------------------------------------------//


#endif //__FLALARMCLOCK_H__