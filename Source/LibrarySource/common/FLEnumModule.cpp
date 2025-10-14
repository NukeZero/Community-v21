
#include "FLEnumModule.h"


FLEnumModule::FLEnumModule()
{
	m_hSnap = INVALID_HANDLE_VALUE;
	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
}

FLEnumModule::~FLEnumModule()
{
	Close();
}

bool	FLEnumModule::Open( DWORD dwProcessID )
{
	Close();

	m_hSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessID );
	if( m_hSnap != INVALID_HANDLE_VALUE )
	{
		return true;
	}

	return false;
}

void	FLEnumModule::Close()
{
	if( m_hSnap != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( m_hSnap );
		m_hSnap = INVALID_HANDLE_VALUE;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
}

const TCHAR*	FLEnumModule::GetFirst()
{
	if( m_hSnap == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
	m_tEntry.dwSize = sizeof( m_tEntry );
	if( ::Module32First( m_hSnap, &m_tEntry ) == FALSE )
	{
		return NULL;
	}

	return m_tEntry.szExePath;
}

const TCHAR*	FLEnumModule::GetNext()
{
	if( m_hSnap == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
	m_tEntry.dwSize = sizeof( m_tEntry );
	if( ::Module32Next( m_hSnap, &m_tEntry ) == FALSE )
	{
		return NULL;
	}

	return m_tEntry.szExePath;
}
