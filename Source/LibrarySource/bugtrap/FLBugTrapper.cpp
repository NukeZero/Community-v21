
#include "FLBugTrapper.h"

#if defined(_UNICODE) || defined(UNICODE)
#pragma comment( lib, "BugTrapU.lib" )
#else
#pragma comment( lib, "BugTrap.lib" )
#endif

#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib" )


const __int64 ONE_DAY	= 60 * 60 * 24 * 10000000I64;


bool	FLBugTrapper::s_bInstall	= false;


void	FLBugTrapper::Install( const TCHAR* pszAppName, const bool bInternal )
{
	// bInternal 이 true 이면 내부개발서버이다. 내부개발서버에서는 덤프전송UI를 띄워준다.
	s_bInstall = true;

	const TCHAR* szIP					= bInternal == true ? _T( "192.168.106.66" ) : _T( "119.207.77.114" );
	const BUGTRAP_ACTIVITY ActivityType	= bInternal == true ? BTA_SHOWUI : BTA_SENDREPORT;

	BT_SetAppName( pszAppName );
	BT_SetFlags( BTF_DETAILEDMODE | BTF_SCREENCAPTURE );
 	BT_SetActivityType( ActivityType );
	BT_SetReportFormat( BTRF_TEXT );
	BT_SetNotificationEMail( _T( "Flyff_Studio@gala.kr" ) );
	BT_SetSupportServer( szIP, 14141 );
}

void	FLBugTrapper::AddFile( const TCHAR* pszFullPathFileName )
{
	if( s_bInstall == false )
	{
		return;
	}

	BT_AddLogFile( pszFullPathFileName );
}

void	FLBugTrapper::AddFile( const TCHAR* pszFullPathFolderName, DWORD dwDays )
{
	if( s_bInstall == false )
	{
		return;
	}

	TCHAR pszFullPathFileName[MAX_PATH + _MAX_FNAME] = { 0, };
	::PathCombine( pszFullPathFileName, pszFullPathFolderName, _T( "*" ) );
	
	WIN32_FIND_DATA kFindData;

	HANDLE hFindFile = ::FindFirstFile( pszFullPathFileName, &kFindData );
	if( hFindFile == INVALID_HANDLE_VALUE )
	{
		return;
	}

	SYSTEMTIME kSystemTime;
	::GetLocalTime( &kSystemTime );

	BOOL bMore = TRUE;
	while( bMore )
	{
		if( ( kFindData.dwFileAttributes & ( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY ) ) == 0 )
		{
			FILETIME kFileTime;
			::SystemTimeToFileTime( &kSystemTime, &kFileTime );

			__int64 n64FindFileTime		= MAKEINT64( kFindData.ftLastWriteTime.dwLowDateTime, kFindData.ftLastWriteTime.dwHighDateTime );
			__int64 n64LocalTime		= MAKEINT64( kFileTime.dwLowDateTime, kFileTime.dwHighDateTime );

			__int64 nFileTimeGap		= n64LocalTime - n64FindFileTime;

			if( ::_abs64( nFileTimeGap ) <= ONE_DAY * static_cast<__int64>( dwDays ) )
			{
				::PathCombine( pszFullPathFileName, pszFullPathFolderName, kFindData.cFileName );
				BT_AddLogFile( pszFullPathFileName );
			}
		}

		bMore = ::FindNextFile( hFindFile, &kFindData );
	}

	::FindClose( hFindFile );
}

void	FLBugTrapper::SendToBugTrapServer( const DWORD dwDumpType )
{
	if( s_bInstall == false )
	{
		return;
	}

	BT_SetDumpType( dwDumpType );
	BT_CallSehFilter();
}
