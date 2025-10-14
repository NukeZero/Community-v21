
#include "FLEnumThread.h"


FLEnumThread::FLEnumThread()
{
	m_hSnap = INVALID_HANDLE_VALUE;
	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
}

FLEnumThread::~FLEnumThread()
{
	Close();
}

bool	FLEnumThread::Open()
{
	Close();

	m_hSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
	if( m_hSnap != INVALID_HANDLE_VALUE )
	{
		return true;
	}

	return false;
}

void	FLEnumThread::Close()
{
	if( m_hSnap != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( m_hSnap );
		m_hSnap = INVALID_HANDLE_VALUE;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
}

DWORD	FLEnumThread::GetFirst( DWORD dwProcessID )
{
	if( m_hSnap == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
	m_tEntry.dwSize = sizeof( m_tEntry );
	if( ::Thread32First( m_hSnap, &m_tEntry ) == FALSE )
	{
		return 0;
	}

	while( m_tEntry.th32OwnerProcessID != dwProcessID )
	{
		::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
		m_tEntry.dwSize = sizeof( m_tEntry );
		if( ::Thread32Next( m_hSnap, &m_tEntry ) == FALSE )
		{
			return 0;
		}
	}

	return m_tEntry.th32ThreadID;
}

DWORD	FLEnumThread::GetNext( DWORD dwProcessID )
{
	if( m_hSnap == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	do
	{
		::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
		m_tEntry.dwSize = sizeof( m_tEntry );
		if( ::Thread32Next( m_hSnap, &m_tEntry ) == FALSE )
		{
			return 0;
		}

	} while( m_tEntry.th32OwnerProcessID != dwProcessID );

	return m_tEntry.th32ThreadID;
}
