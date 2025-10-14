// Funtion.cpp: implementation of the CFuntion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Funtion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuntion::CFuntion()
{

}

CFuntion::~CFuntion()
{

}		

void CFuntion::Bagic_Message()
{
	printf( "__________________________________________________________________\n"	);
	printf( " 패치 검출기 입니다 ㅋㅋ - seghope -\n"								);
	printf( "__________________________________________________________________\n"	);
/*	printf( " 첫번째 인자값 : 정식 : F, 테스트 : T\n"								);
	printf( " 두번째 인자값 : S: SERVER, C: CLIENT, E : EXE , A : ALL\n"			);
	printf( " 세번째 인자값 : DATE ( 06-11-04 : 2004/06/11 )\n"						);
	printf( " exit : 종료\n"														);
	printf( "__________________________________________________________________\n"	);
	printf( " ex ) F A 06-11-04\n"													);
	printf( " 두번째 인자와 세번째 인자를 넣지 않으면 기본 A 오늘날자가 적용됨\n"	);
	printf( "__________________________________________________________________\n"	);	
*/
}

BOOL CFuntion::IsDirectory( const char * pszFlag, int nYear, int nMonth, int nDay )
{
	char szCommand[256] = {0,};
	char szFlag[256] = {0,};
	CTime tTime = CTime::GetCurrentTime();
	strcpy( szFlag, pszFlag );
	if( strlen( szFlag ) == 0 )
	{
		sprintf( szFlag, "A" );
		nYear = tTime.GetYear() % 100; nMonth = tTime.GetMonth(); nDay = tTime.GetDay();
	}
	else
	if( nYear == 0 )
	{
		nYear = tTime.GetYear() % 100; nMonth = tTime.GetMonth(); nDay = tTime.GetDay();
	}

	char szDir[MAX_PATH] = {0,};
	DWORD dwResult;
	GetCurrentDirectory( MAX_PATH, szDir );

	sprintf( szCommand, "%s\\%d-%d-%02d", szDir, nMonth, nDay, nYear );
	dwResult = GetFileAttributes( szCommand );
	if( dwResult != 0xFFFFFFFF )
	{
		return FALSE;
	}
	return TRUE;
}

void CFuntion::PatchexeFun( const char * pszServer, const char * pszFlag, int nYear, int nMonth, int nDay )
{
	char szCommand[256] = {0,};
	char szFlag[256] = {0,};
	CTime tTime = CTime::GetCurrentTime();
	strcpy( szFlag, pszFlag );
	if( strlen( szFlag ) == 0 )
	{
		sprintf( szFlag, "A" );
		nYear = tTime.GetYear() % 100; nMonth = tTime.GetMonth(); nDay = tTime.GetDay();
	}
	else
	if( nYear == 0 )
	{
		nYear = tTime.GetYear() % 100; nMonth = tTime.GetMonth(); nDay = tTime.GetDay();
	}
	sprintf( szCommand, "Patch %s %d-%d-%02d %s", szFlag, nMonth, nDay, nYear, pszServer );//  A date T)
	system( szCommand );
	printf( "__________________________________\n" );
	printf( "  Patch 완료 : %s\n", szCommand );
	printf( "__________________________________\n" );
	if( _stricmp( "-", szFlag ) == 0 || _stricmp( "A", szFlag ) == 0 || _stricmp( "E", szFlag ) == 0 )
		CopyAndMoveFile( pszServer, nYear, nMonth, nDay );

/*	if( stricmp( "-", szFlag ) == 0 || stricmp( "A", szFlag ) == 0 || stricmp( "C", szFlag ) == 0 )
		GZip( pszServer, nYear, nMonth, nDay );
*/	
}

void CFuntion::GZip( const char * pszServer )
{
	DWORD dwResult;
	char szCommand[MAX_PATH] = {0,};
	sprintf( szCommand, "%s\\C", pszServer );
	dwResult = GetFileAttributes( szCommand );
	if( dwResult != 0xFFFFFFFF )
	{
		sprintf( szCommand, "gzip -r %s\\C", pszServer );
		system( szCommand );

		printf( "__________________________________\n" );
		printf( "  gzip 완료 : %s\n", szCommand );
		printf( "__________________________________\n" );
	}

	sprintf( szCommand, "%s\\A", pszServer );
	RemoveDirectory( szCommand );
	sprintf( szCommand, "%s\\C", pszServer );
	RemoveDirectory( szCommand );
	sprintf( szCommand, "%s\\E", pszServer );
	RemoveDirectory( szCommand );
	sprintf( szCommand, "%s\\S", pszServer );
	RemoveDirectory( szCommand );

	printf( "\n\n__________________________________\n" );
	printf( "  Complete Good~~~~  \n" );
	printf( "  즐거운 하루 되세여~ 패치 잘하세여~ \n" );
	printf( "__________________________________\n" );
}

void CFuntion::CopyAndMoveFile( const char * pszServer, int nYear, int nMonth, int nDay )
{
	char szCommand[MAX_PATH] = {0,};
	char szDir[MAX_PATH] = {0,};
	
	GetCurrentDirectory( MAX_PATH, szDir );

	sprintf( szCommand, "%s\\%d-%d-%02d", szDir, nMonth, nDay, nYear );

	OneCopyMove( szCommand, "AccountServer" );
	OneCopyMove( szCommand, "Certifier" );
	OneCopyMove( szCommand, "CoreServer" );
	OneCopyMove( szCommand, "CacheServer" );
	OneCopyMove( szCommand, "DatabaseServer", 1 );
	OneCopyMove( szCommand, "LoginServer" );
	OneCopyMove( szCommand, "WorldServer", 1 );
	OneCopyMove( szCommand, "Neuz", 2 );

	GZip( szCommand );
}

void CFuntion::RemoveReleFile( const char * pszPath )
{
	BOOL			bResult	= TRUE;
	HANDLE			hSrch	= NULL;
	WIN32_FIND_DATA wfd;
	CString			sDelet;
	CString			sBagic = pszPath;
	CString			sAll = pszPath;
	sAll += "\\*.*";
		
	hSrch	= FindFirstFile( sAll, &wfd );										// 패스의 처음 파일을 찾는다
	while( bResult )
	{
		if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )					// 디렉토리
		{
			//					sprintf( szServer, "D:\\abc\\%s", wfd.cFileName );
			//					RemoveDirectory( szServer );
		}
		else
		{
			sDelet = sBagic + "\\";
			sDelet += wfd.cFileName;
			DeleteFile( sDelet );
		}
		bResult = FindNextFile( hSrch, &wfd );
	}
	FindClose( hSrch );
}

void CFuntion::OneCopyMove( const char * pszPath, const char * pszDirectory, short i )
{
	DWORD dwResult;
	char szCommand[MAX_PATH] = {0,};
	sprintf( szCommand, "%s\\E\\%s\\Release", pszPath, pszDirectory );
	dwResult = GetFileAttributes( szCommand );
	if( dwResult != 0xFFFFFFFF )
	{
		if( i == 1 )
		{
			sprintf( szCommand, "%s\\S", pszPath );
			CreateDirectory( szCommand, NULL );
		}
		else
		if( i == 2 )
		{
			sprintf( szCommand, "%s\\C", pszPath );
			CreateDirectory( szCommand, NULL );
		}
	}
	else
	{
		return;
	}

	if( i == 0 )
		sprintf( szCommand, "xcopy %s\\E\\%s\\Release %s\\E\\%s /s", pszPath, pszDirectory, pszPath, pszDirectory );
	else
	if( i == 1 )
		sprintf( szCommand, "xcopy %s\\E\\%s\\Release %s\\S /s", pszPath, pszDirectory, pszPath );
	else 
		sprintf( szCommand, "xcopy %s\\E\\%s\\Release %s\\C /s", pszPath, pszDirectory, pszPath );

	system( szCommand );
	sprintf( szCommand, "%s\\E\\%s\\Release", pszPath, pszDirectory );

	RemoveReleFile( szCommand );
	RemoveDirectory( szCommand );

	printf( "__________________________________\n" );
	printf( " Copy & Move 완료 : %s\n", szCommand );
	printf( "__________________________________\n" );
	

	if( i != 0 )
	{
		sprintf( szCommand, "%s\\E\\%s", pszPath, pszDirectory );
		RemoveDirectory( szCommand );
	}
}
CFuntion g_Funtion;
