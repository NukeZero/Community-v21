#ifndef __FLRANDOMRESPAWNNPC_H__
#define __FLRANDOMRESPAWNNPC_H__

#include "../_Common/FLAlarmClock.hpp"
#include "../_Common/FLGameTimer.h"

#include "../_Common/FLPeriodRepetitionAlarm.h"


struct FLRandomRespawnNPC_Data			//template class 쓰기 위한 어쩔수 없는 분리
{
public:
	struct Pos
	{
		DWORD				 	worldID;
		D3DXVECTOR3	 			pos;
	};

	typedef					std::vector< Pos >	VecRandomPos;

	DWORD					m_dwMoverIndex;
	std::string				m_strCharacterKey;
	BOOL					m_bActiveAttack;

	VecRandomPos			m_vecRandomPos;
};



template< class _BaseClass >
class FLRandomRespawnNPCAlarm : public _BaseClass
{
public:
	FLRandomRespawnNPCAlarm();
	virtual ~FLRandomRespawnNPCAlarm();

	BOOL					SetData( const std::string & szAlarmName, const FLTm & startDate, const FLTm & endDate, const FLTm & startTime, const FLTm & endTime
		, const DWORD dwMoverIndex, const std::string & strCharacterKey, const BOOL bActiveAttack, FLRandomRespawnNPC_Data::VecRandomPos & vecPos );

	virtual	BOOL			OnStart();
	virtual BOOL			OnEnd();
	virtual void			OnExpired();
	virtual void			OnError();

private:
	std::string				m_strName;

	OBJID					m_objid;
	FLRandomRespawnNPC_Data	m_data;
};




#endif //__FLRANDOMRESPAWNNPC_H__