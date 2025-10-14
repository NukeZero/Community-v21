
#include "FLEnumProcess.h"


FLEnumProcess::FLEnumProcess()
{
	m_hSnap = INVALID_HANDLE_VALUE;
	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
}

FLEnumProcess::~FLEnumProcess()
{
	Close();
}

bool	FLEnumProcess::Open()
{
	Close();

	m_hSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( m_hSnap != INVALID_HANDLE_VALUE )
	{
		return true;
	}

	return false;
}

void	FLEnumProcess::Close()
{
	if( m_hSnap != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( m_hSnap );
		m_hSnap = INVALID_HANDLE_VALUE;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
}

const TCHAR*	FLEnumProcess::GetFirst()
{
	if( m_hSnap == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
	m_tEntry.dwSize = sizeof( m_tEntry );
	if( ::Process32First( m_hSnap, &m_tEntry ) == FALSE )
	{
		return NULL;
	}

	return m_tEntry.szExeFile;
}

const TCHAR*	FLEnumProcess::GetNext()
{
	if( m_hSnap == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	::memset( &m_tEntry, 0, sizeof( m_tEntry ) );
	m_tEntry.dwSize = sizeof( m_tEntry );
	if( ::Process32Next( m_hSnap, &m_tEntry ) == FALSE )
	{
		return NULL;
	}

	return m_tEntry.szExeFile;
}

DWORD	FLEnumProcess::GetProcessID()	const
{
	return m_tEntry.th32ProcessID;
}
