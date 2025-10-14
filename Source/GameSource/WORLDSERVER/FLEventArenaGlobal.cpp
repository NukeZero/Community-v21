
#include "stdafx.h"
#include "FLEventArenaGlobal.h"
#include "FLEventArenaSpecLoader.h"


FLEventArenaGlobal*	FLEventArenaGlobal::GetInstance()
{
	static FLEventArenaGlobal kEventArenaGlobal;
	return &kEventArenaGlobal;
}

FLEventArenaGlobal::FLEventArenaGlobal()
{
	m_pEventArena = NULL;
	::memset( m_szConfigFile, 0, sizeof( m_szConfigFile ) );

	Clear();
}

FLEventArenaGlobal::~FLEventArenaGlobal()
{
	Clear();
}

void	FLEventArenaGlobal::Clear()
{
	const TCHAR* szDefaultEventArenaConfigFile = _T( "EventArena.txt" );

	m_bIsArenaChannel = false;
	FLStrcpy( m_szConfigFile, _countof( m_szConfigFile ), szDefaultEventArenaConfigFile );

	m_kSpec.Clear();
	m_kTeamManager.Clear();
	SAFE_DELETE( m_pEventArena );
}

void	FLEventArenaGlobal::SetArenaChannel()
{
	if( _GetContentState( CT_EVENT_ARENA ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	m_bIsArenaChannel = true;
}

void	FLEventArenaGlobal::SetConfigFile( const TCHAR* pszConfigFile )
{
	if( pszConfigFile == NULL )
	{
		return;
	}

	FLStrcpy( m_szConfigFile, _countof( m_szConfigFile ), pszConfigFile );
}

bool	FLEventArenaGlobal::Build()
{
	if( IsArenaChannel() == false )
	{
		return true;
	}

	SAFE_DELETE( m_pEventArena );
	m_kSpec.Clear();
	m_kTeamManager.Clear();

	FLEventArenaSpecLoader kSpecLoader;
	if( kSpecLoader.Load( m_szConfigFile, &m_kSpec, &m_kTeamManager ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나 설정 파일 로드 실패 File: %s" ), m_szConfigFile );
		return false;
	}

	m_pEventArena = new FLEventArena( &m_kSpec, &m_kTeamManager );
	if( m_pEventArena == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "이벤트 아레나 생성 실패" ) );
		return false;
	}

	return true;
}

bool	FLEventArenaGlobal::IsArenaChannel()	const
{
	return m_bIsArenaChannel;
}

FLEventArena*	FLEventArenaGlobal::GetEventArena()	const
{
	return m_pEventArena;
}

