#ifndef __EVE_SCHOOL_H__
#define	__EVE_SCHOOL_H__

#include "guild.h"

#ifdef __SCHOOL0722
#define	MAX_SCHOOL	10
#else	// __SCHOOL0722
#define	MAX_SCHOOL	5
#endif	// __SCHOOL0722

enum
{
	SBS_END, SBS_READY, SBS_START, SBS_START2,
};

typedef	struct	_SCHOOL_ENTRY
{
	u_long	id;
	char	lpName[MAX_G_NAME];
	int		nSurvivor;
	int		nSize;
	int		nLevel;
#ifdef __SCHOOL0719
	int		nDead;
#endif	// __SCHOOL0719
	_SCHOOL_ENTRY()
		{	
			id	= 0;
			*lpName	= '\0';
			nSurvivor	= 0;
			nSize	= 0;
			nLevel = 0;
#ifdef __SCHOOL0719
			nDead	= 0;
#endif	// __SCHOOL0719
		};
}
SCHOOL_ENTRY, *PSCHOOL_ENTRY;

class CGuildMng;

class	CEveSchool
{
public:
	CGuildMng*	m_pSchoolMng;
	D3DXVECTOR3	m_avPos[MAX_SCHOOL];
	D3DXVECTOR3	m_vDefault;
#ifdef __SCHOOL0719
	DWORD	m_dwsbstart2;
#endif	// __SCHOOL0719
public:
	CEveSchool();
	~CEveSchool()	{}

	BOOL	Ready( void );
	BOOL	Start( void );
	BOOL	Start2( void );
	BOOL	End( void );
	BOOL	Report( void );

	BOOL	LoadPos( LPCSTR lpszFileName );
	D3DXVECTOR3		GetPos( u_long id );
	D3DXVECTOR3		GetPos( void )	{	return m_vDefault;	}

	static	CEveSchool* GetInstance();
};


typedef struct __AUTO_OPEN
{
	BOOL bUseing;
	BYTE nHour;	
	BYTE nMinute;		
} __AUTO_OPEN;

#ifdef __GUILDCOMBAT
class CGuildCombat
{
#ifdef __WORLDSERVER 	
	__AUTO_OPEN		__AutoOpen[7];
#endif // __WORLDSERVER
	
public:
	enum { OPEN_STATE, CLOSE_STATE, WAR_STATE, COMPLET_CLOSE_STATE, GM_COLSE_STATE };
	enum { NOTENTER_STATE = 100, NOTENTER_COUNT_STATE, ENTER_STATE, MAINTENANCE_STATE, WAR_WAR_STATE, WAR_CLOSE_STATE, WAR_CLOSE_WAIT_STATE, WAR_TELEPORT_STATE };
	enum { ALLMSG = 1000, GUILDMSG, JOINMSG, WORLDMSG, STATE, WARSTATE, WAIT };
	struct __GuildCombatMember
	{
		set<u_long> GuildCombatJoinMember;		// 길드전에 참가한멤버
		set<u_long> GuildCombatOutMember;		// 참가했는데 죽었던가 나간 멤버
		FLOAT fAvgLevel;		// 평균 레벨
		void Clear()
		{
			GuildCombatJoinMember.clear();
			GuildCombatOutMember.clear();
			fAvgLevel = 0.0f;
		};
	};
	struct __GuildCombatProcess
	{
		int		nState;
		DWORD	dwTime;
		DWORD	dwCommand;
		DWORD	dwParam;
	};

	int		m_nGuildCombatIndex;
	u_long	m_nWinGuildId;
	int		m_nWinGuildCount;
	map<u_long, int> m_GuildCombatRanking;
#ifdef __WORLDSERVER 
	map<u_long, __GuildCombatMember*> m_GuildCombatMem;
DWORD	m_dwTime;
	int		m_nProcessGo;
	int	 	m_nProcessCount[ 25 ];
	__GuildCombatProcess GuildCombatProcess[250];
	int		m_nStopWar;	// 1이면 중간에 종료, 2이면 운영자가 종료
#endif // __WORLDSERVER

	int		m_nState;		// 길드워 상태
	int		m_nGuildCombatState;	// 전투 중일 때의 상태
public:
	//	Constructions
	CGuildCombat();
	virtual ~CGuildCombat();
	void	Clear( BOOL bAll = TRUE );
	void	GameClear();

#ifdef __WORLDSERVER
	BOOL	LoadScript( LPCSTR lpszFileName );
	void	JoinGuildCombat( u_long idGuild );
	void	OutGuildCombat( u_long idGuild );
	void	SetMaintenance();
	void	SetGuildCombat();
	void	SetGuildCombatClose( BOOL bGM = FALSE );
	void	SetGuildCombatCloseWait( BOOL bGM = FALSE );
	void	GuildCombatCloseTeleport();
	
	void	JoinWar( CUser* pUser, BOOL bWar = FALSE );
	void	OutWar( CUser* pUser, BOOL bApplication = TRUE );
	void	JoinObserver( CUser* pUser );
	void	GuildCombatRequest( CUser* pUser );
	void	GuildCombatCancel( CUser* pUser );
	void	GuildCombatOpen( void );
	void	GuildCombatEnter( CUser* pUser );
	void	UserOutGuildCombatResult( CUser* pUser );
	void	GuildCombatResult( BOOL nResult = FALSE, u_long idGuildWin = 0 );
	void	Process();
	void	ProcessCommand();
	void	SendJoinMsg( LPCTSTR lpszString );
	void	SendGuildCombatEnterTime( void );
#endif // __WORLDSERVER
};
#endif // __GUILDCOMBAT

#endif	// __EVE_SCHOOL_H__