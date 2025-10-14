
#ifndef	__FLEVENTARENASPEC_H__
#define	__FLEVENTARENASPEC_H__


#include "FLEventArenaSpecStruct.h"
#include "FLEventArenaStartPos.h"


class	FLEventArenaSpec
{
public:

	FLEventArenaSpec();
	~FLEventArenaSpec();

	void	Clear();

	bool	AddTonement( const T_EVENT_ARENA_TONEMENT_SPEC* pTonement );
	bool	AddGiveItem( const T_EVENT_ARENA_GIVE_ITEM_SPEC* pGiveItem );
	bool	SetTimeConfig( const T_EVENT_ARENA_TIME_CONFIG_SPEC* pTimeConfig );
	bool	AddStartPos( const T_EVENT_ARENA_START_POSITION_SPEC* pStartPos );
	void	SetDeathNPCID( DWORD dwID );
	void	SetWallCtrlID( DWORD dwID );
	void	SetTransformMoverID( DWORD dwID );

	const std::vector< DWORD >&		GetArenaWorldID()			const;
	const T_WORLD_POSITION*			GetStartPos()				const;
	const T_WORLD_POSITION*			GetStandbyPos( int nTeam )	const;
	const T_WORLD_POSITION*			GetFightPos( int nTeam )	const;
	const T_WORLD_POSITION*			GetWallPos()				const;
	T_WORLD_POSITION				GetDeathNPCPos()			const;

	const T_EVENT_ARENA_TONEMENT*	GetTonement( DWORD dwTonementID )	const;
	void							GetAllTonement( std::vector< T_EVENT_ARENA_TONEMENT >& vecTonements )	const;

	DWORD	GetCallWaitTime()		const;
	DWORD	GetBattleStandbyTime()	const;
	DWORD	GetBattleTime()			const;
	DWORD	GetWinnerDisplayTime()	const;

	const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >&	GetGiveItems()	const;

	DWORD	GetWallCtrlID()	const;
	DWORD	GetDeathNPCID()	const;
	DWORD	GetTransformMoverID()	const;

private:

	// 토너먼트
	typedef	std::vector< T_EVENT_ARENA_TONEMENT >		TONEMENT_LIST;
	typedef	TONEMENT_LIST::iterator						TONEMENT_ITER;

	typedef	std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >	GIVE_ITEM_LIST;
	typedef	GIVE_ITEM_LIST::iterator					GIVE_ITEM_ITER;


	TONEMENT_LIST					m_vecTonements;
	T_EVENT_ARENA_TIME_CONFIG_SPEC	m_tTimeConfig;
	GIVE_ITEM_LIST					m_vecGiveItems;
	FLEventArenaStartPos			m_kStartPos;

	DWORD							m_dwWallCtrlID;
	DWORD							m_dwDeathNPCID;
	DWORD							m_dwTransformMoverID;


};


#endif