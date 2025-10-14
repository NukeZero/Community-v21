
#ifndef	__FLEVENTARENASTARTPOS_H__
#define	__FLEVENTARENASTARTPOS_H__


#include "FLEventArenaSpecStruct.h"


class	FLEventArenaStartPos
{
private:

	typedef	std::vector< T_EVENT_ARENA_START_POSITION_SPEC >	START_POS_LIST;
	typedef	START_POS_LIST::const_iterator						START_POS_CITER;

public:

	FLEventArenaStartPos();
	~FLEventArenaStartPos();

	void	Clear();

	bool	AddStartPos( const T_EVENT_ARENA_START_POSITION_SPEC* pStartPos );

	const std::vector< DWORD >&	GetWorldID()	const;
	const T_WORLD_POSITION*	GetStart()	const;
	const T_WORLD_POSITION*	GetStandby( int nTeam )	const;
	const T_WORLD_POSITION*	GetFight( int nTeam )	const;
	const T_WORLD_POSITION*	GetWall()	const;
	T_WORLD_POSITION		GetDeathNPC()	const;

private:

	void	AddWorldID( DWORD dwWorldID );

	static const T_EVENT_ARENA_START_POSITION_SPEC*	GetRandPos( const START_POS_LIST& vecPosList );

private:

	std::vector< DWORD >	m_vecWorldIDs;

	START_POS_LIST			m_vecStart;
	mutable int				m_nNextStartIdx;

	START_POS_LIST			m_vecRedStandby;
	START_POS_LIST			m_vecRedFigth;

	START_POS_LIST			m_vecBlueStandby;
	START_POS_LIST			m_vecBlueFigth;

	START_POS_LIST			m_vecWall;
	START_POS_LIST			m_vecDeathNPC;

};







#endif