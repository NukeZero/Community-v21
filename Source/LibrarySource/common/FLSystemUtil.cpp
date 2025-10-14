
#include "FLSystemUtil.h"
#include "FLStringFunction.h"



FLSystemInfo::FLSystemInfo()
{
	SYSTEM_INFO* pSysInfo = this;
	::memset( pSysInfo, 0, sizeof( *pSysInfo ) );

	if( FLIsWow64() == false )
	{
		::GetSystemInfo( pSysInfo );
	}
	else
	{
#if _WIN32_WINNT >= 0x0501
		::GetNativeSystemInfo( pSysInfo );
#else
		::GetSystemInfo( pSysInfo );
#endif
	}
}

FLOSInfo::FLOSInfo()
{
	OSVERSIONINFOEX* pOSInfo = this;
	::memset( pOSInfo, 0, sizeof( *pOSInfo ) );

	pOSInfo->dwOSVersionInfoSize	= sizeof( OSVERSIONINFOEX );
	if( ::GetVersionEx( reinterpret_cast< OSVERSIONINFO* >( pOSInfo ) ) == FALSE )
	{
		pOSInfo->dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		::GetVersionEx( reinterpret_cast< OSVERSIONINFO* >( pOSInfo ) );
	}
}

bool	FLIsWindows9x()
{
	FLOSInfo cOSInfo;
	return (cOSInfo.dwPlatformId != VER_PLATFORM_WIN32_NT);
}

bool	FLIsWow64()
{
	typedef BOOL (WINAPI *PIS_WOW64_PROCESS)( HANDLE, PBOOL );

	BOOL bIsWow64 = FALSE;
	PIS_WOW64_PROCESS pIsWow64Process = reinterpret_cast< PIS_WOW64_PROCESS >( ::GetProcAddress( ::GetModuleHandle( _T( "kernel32" ) ), "IsWow64Process" ) );
	if( pIsWow64Process != NULL )
	{
		if( pIsWow64Process( ::GetCurrentProcess(), &bIsWow64 ) == FALSE )
		{
			bIsWow64 = FALSE;
		}
	}

	return (bIsWow64 != FALSE);
}

void	FLGetWinVersion( TCHAR* szVersion, size_t cchVersion, TCHAR* szBuildNum, size_t cchBuildNum )
{
	// winbase.h
#ifndef	VER_PLATFORM_WIN32_CE
#define	VER_PLATFORM_WIN32_CE           3
#endif

	if( szVersion != NULL && cchVersion > 0 && szBuildNum != NULL && cchBuildNum > 0 )
	{
		static const TCHAR	szBuildUnknown[]	= _T( "0.0.0" );
		static const TCHAR	szWinUnknown[]		= _T( "Unknown Windows Version" );

		static const TCHAR szWin95[]			= _T( "Windows 95" );
		static const TCHAR szWin95SP1[]			= _T( "Windows 95 SP1" );
		static const TCHAR szWin95OSR2[]		= _T( "Windows 95 OSR2" );
		static const TCHAR szWin98[]			= _T( "Windows 98" );
		static const TCHAR szWin98SP1[]			= _T( "Windows 98 SP1" );
		static const TCHAR szWin98SE[]			= _T( "Windows 98 SE" );
		static const TCHAR szWinME[]			= _T( "Windows ME" );

		static const TCHAR szWinNT351[]			= _T( "Windows NT 3.51" );
		static const TCHAR szWinNT4[]			= _T( "Windows NT 4" );
		static const TCHAR szWin2000[]			= _T( "Windows 2000 Professional" );
		static const TCHAR szWin2000Server[]	= _T( "Windows 2000 Server" );

		static const TCHAR szWinXP[]			= _T( "Windows XP" );
		static const TCHAR szWinXP64[]			= _T( "Windows XP(x64)" );
		static const TCHAR szWin2003Server[]	= _T( "Windows Server 2003" );
		static const TCHAR szWin2003Server64[]	= _T( "Windows Server 2003(x64)" );

		static const TCHAR szWinVista[]			= _T( "Windows Vista" );
		static const TCHAR szWin2008Server[]	= _T( "Windows Server 2008" );
		static const TCHAR szWin7[]				= _T( "Windows 7" );
		static const TCHAR szWin2008ServerR2[]	= _T( "Windows Server 2008 R2" );

		static const TCHAR szWinCE[]			= _T( "Windows CE" );


		FLStrcpy( szVersion, cchVersion, szWinUnknown );
		FLStrcpy( szBuildNum, cchBuildNum, szBuildUnknown );


		FLSystemInfo cSysInfo;
		FLOSInfo cOSInfo;

		DWORD dwPlatformId = cOSInfo.dwPlatformId;
		DWORD dwMajorVersion = cOSInfo.dwMajorVersion;
		DWORD dwMinorVersion = cOSInfo.dwMinorVersion;
		DWORD dwBuildNumber = cOSInfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

		FLSPrintf( szBuildNum, cchBuildNum, _T( "%u.%u.%u" ), dwMajorVersion, dwMinorVersion, dwBuildNumber );

		if( (dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4) )
		{
			if( (dwMinorVersion < 10) && (dwBuildNumber == 950) )
			{
				FLStrcpy( szVersion, cchVersion, szWin95 );
			}
			else if( (dwMinorVersion < 10) && ((dwBuildNumber > 950) && (dwBuildNumber <= 1080)) )
			{
				FLStrcpy( szVersion, cchVersion, szWin95SP1 );
			}
			else if( (dwMinorVersion < 10) && (dwBuildNumber > 1080) )
			{
				FLStrcpy( szVersion, cchVersion, szWin95OSR2 );
			}
			else if( (dwMinorVersion == 10) && (dwBuildNumber == 1998) )
			{
				FLStrcpy( szVersion, cchVersion, szWin98 );
			}
			else if( (dwMinorVersion == 10) && ((dwBuildNumber > 1998) && (dwBuildNumber < 2183)) )
			{
				FLStrcpy( szVersion, cchVersion, szWin98SP1 );
			}
			else if( (dwMinorVersion == 10) && (dwBuildNumber >= 2183) )
			{
				FLStrcpy( szVersion, cchVersion, szWin98SE );
			}
			else if( dwMinorVersion == 90 )
			{
				FLStrcpy( szVersion, cchVersion, szWinME );
			}
		}
		else if( dwPlatformId == VER_PLATFORM_WIN32_NT )
		{
			if( (dwMajorVersion == 3) && (dwMinorVersion == 51) )
			{
				FLStrcpy( szVersion, cchVersion, szWinNT351 );
			}
			else if( (dwMajorVersion == 4) && (dwMinorVersion == 0) )
			{
				FLStrcpy( szVersion, cchVersion, szWinNT4 );
			}
			else if( (dwMajorVersion == 5) && (dwMinorVersion == 0) )
			{
				if( cOSInfo.wProductType == VER_NT_WORKSTATION )
				{
					FLStrcpy( szVersion, cchVersion, szWin2000 );
				}
				else
				{
					FLStrcpy( szVersion, cchVersion, szWin2000Server );
				}
			}
			else if( (dwMajorVersion == 5) && (dwMinorVersion == 1) )
			{
				FLStrcpy( szVersion, cchVersion, szWinXP );
			}
			else if( (dwMajorVersion == 5) && (dwMinorVersion == 2) )
			{
				if( cOSInfo.wProductType == VER_NT_WORKSTATION && cSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
				{
					FLStrcpy( szVersion, cchVersion, szWinXP64 );
				}
				else if( cOSInfo.wProductType != VER_NT_WORKSTATION && cSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
				{
					FLStrcpy( szVersion, cchVersion, szWin2003Server64 );
				}
				else
				{
					FLStrcpy( szVersion, cchVersion, szWin2003Server );
				}
			}
			else if( (dwMajorVersion == 6) && (dwMinorVersion == 0) )
			{
				if( cOSInfo.wProductType == VER_NT_WORKSTATION )
				{
					FLStrcpy( szVersion, cchVersion, szWinVista );
				}
				else
				{
					FLStrcpy( szVersion, cchVersion, szWin2008Server );
				}
			}
			else if( (dwMajorVersion == 6) && (dwMinorVersion == 1) )
			{
				if( cOSInfo.wProductType == VER_NT_WORKSTATION )
				{
					FLStrcpy( szVersion, cchVersion, szWin7 );
				}
				else
				{
					FLStrcpy( szVersion, cchVersion, szWin2008ServerR2 );
				}
			}

			if( dwMajorVersion > 4 )
			{
				if( _tcslen( cOSInfo.szCSDVersion ) > 0 )
				{
					FLStrcat( szVersion, cchVersion, _T( " " ) );
					FLStrcat( szVersion, cchVersion, cOSInfo.szCSDVersion );
				}
			}

			if( dwMajorVersion >= 6 )
			{
				if( cSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
				{
					FLStrcat( szVersion, cchVersion, _T( ", 64-bit" ) );
				}
				else if( cSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
				{
					FLStrcat( szVersion, cchVersion, _T( ", 32-bit" ) );
				}
			}
		}
		else if( dwPlatformId == VER_PLATFORM_WIN32_CE )
		{
			FLStrcpy( szVersion, cchVersion, szWinCE );
		}
	}
}
