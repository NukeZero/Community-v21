#ifndef __FLYFFEVENT_H__
#define	__FLYFFEVENT_H__

#define	MAX_EVENT	1024

enum
{
	EVE_0401A, EVE_18, ENABLE_GUILD_INVENTORY,
	EVE_GUILDWAR,
	EVE_SCHOOL, EVE_SCHOOL_BATTLE,
	EVE_NOFLYMONSTER, // 비행 몬스터 제거 
	EVE_NODARKON, // 다콘 제거 
	EVE_NOGUILD, // 길드 제거 
	EVE_WORMON,
	EVE_RESPAWN,
	EVE_PK,
	EVE_PKCOST,
	EVE_STEAL,
	EVE_EVENT0913,
	EVE_GUILDCOMBAT,
	EVE_EVENT1206,
	EVE_DROPITEMREMOVE,
};

class CFlyffEvent
{
private:
	BYTE	m_aEvent[MAX_EVENT];
public:
//	Constructions
	CFlyffEvent();
	~CFlyffEvent()	{}
//	Operations
	BOOL	SetState( int id, BYTE nState );
	BYTE	GetState( int id );
	BOOL	ClearEvent( int id )	{	return SetState( id, 0 );	}

	void	Serialize( CAr & ar );
};

#endif	// __FLYFFEVENT_H__