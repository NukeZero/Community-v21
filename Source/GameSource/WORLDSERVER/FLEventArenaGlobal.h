
#ifndef	__FLEVENTARENAGLOBAL_H__
#define	__FLEVENTARENAGLOBAL_H__


#include "FLEventArena.h"
#include "FLEventArenaSpec.h"


class	FLEventArenaGlobal
{
public:

	static FLEventArenaGlobal*	GetInstance();

private:

	FLEventArenaGlobal();
	~FLEventArenaGlobal();

public:

	bool			Build();
	void			Clear();

	void			SetArenaChannel();
	void			SetConfigFile( const TCHAR* pszConfigFile );

	bool			IsArenaChannel()	const;
	FLEventArena*	GetEventArena()		const;


private:

	bool					m_bIsArenaChannel;			// 아레나 채널인가?
	TCHAR					m_szConfigFile[MAX_PATH];	// 아레나 설정 파일

	FLEventArenaSpec		m_kSpec;					// 아레나 스펙
	FLEventArenaTeamManager	m_kTeamManager;				// 팀 리스트
	FLEventArena*			m_pEventArena;				// 아레나

};


#define	g_pEventArenaGlobal		FLEventArenaGlobal::GetInstance()
#define	g_pEventArena			g_pEventArenaGlobal->GetEventArena()





#endif