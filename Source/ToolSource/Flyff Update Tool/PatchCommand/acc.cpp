// acc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Funtion.h"

extern CFuntion g_Funtion;

int main(int argc, char* argv[])
{
	BOOL bExit = TRUE;
	DWORD dwResult;
	char szDir[MAX_PATH] = {0,};
	char szCommand[256] = {0,};
	GetCurrentDirectory( MAX_PATH, szDir );
	printf("Hello Flyff World!\n");

	sprintf( szCommand, "%s\\Patch.bat", szDir );
	dwResult = GetFileAttributes( szCommand );
	if( dwResult == 0xFFFFFFFF )
	{
		printf( "\n\nFile Not Found Patch.bat\n");
		printf( "Free Key\n");
		scanf( "%s", szCommand );
		bExit = FALSE;
	}
	sprintf( szCommand, "%s\\gzip.exe", szDir );
	dwResult = GetFileAttributes( szCommand );
	if( dwResult == 0xFFFFFFFF )
	{
		printf( "\n\nFile Not Found gzip.bat\n");
		printf( "Free Key\n");
		scanf( "%s", szCommand );
		bExit = FALSE;
	}

	g_Funtion.Bagic_Message();

	char szServer[256] = {0,};
	char szFlag[256] = {0,};
	char szBuf[256] = {0,};

	int	 nYear = 0;
	int	 nMonth = 0;
	int  nDay = 0;
	
	
	while( bExit )
	{
		ZeroMemory( szServer , sizeof( szServer ) );
		ZeroMemory( szFlag , sizeof( szFlag ) );
		nYear = 0;
		nMonth = 0;
		nDay = 0;

		BOOL bNotCommand = FALSE;
		printf( "\nex ) T(Test), F(정섭)\n Server : " );
		scanf( "%s", szServer );

		if( _stricmp( "exit", szServer ) == 0 )
		{
			bExit = FALSE;
			continue;
		}
		else if( _stricmp( "-h", szServer ) == 0 )
		{
			g_Funtion.Bagic_Message();
			continue;
		}
		else if( strcmp( "F", szServer ) == 0 || strcmp( "T", szServer ) == 0 )
		{
			printf( "ex ) A(All), E(exe), S(Server), C(Client), -(A로 적용, 뒤에 Date값은 오늘날짜로 적용)\n Flag : " );
			scanf( "%s", szBuf );
			
			if( _stricmp( "-", szBuf ) != 0 )
			{
				if( strcmp( "S", szBuf ) == 0 || strcmp( "C", szBuf ) == 0 || strcmp( "E", szBuf ) == 0 || strcmp( "A", szBuf ) == 0 )
				{
					strcpy( szFlag, szBuf );
					printf( "ex ) 041021(년월일), -(오늘날짜로 적용)\n Date : " );
					scanf( "%s", szBuf );
					if( strcmp( "-", szBuf ) != 0 )
					{
						int nDate = atoi( szBuf );
						if( 3 < ( nDate / 10000 ) && ( nDate / 100000 ) < 20  )
						{
							if( 1 <= ( ( nDate % 10000 ) / 100 ) && ( ( nDate % 10000 ) / 100 ) <= 12  )
							{
								if( 1 <= ( ( nDate % 10000 ) % 100 ) && ( ( nDate % 10000 ) % 100 ) <= 31  )
								{
									nYear = nDate / 10000;
									nMonth = ( nDate % 10000 ) / 100;
									nDay = ( nDate % 10000 ) % 100;
								}
							}
							else
							{
								bNotCommand = TRUE;
							}
						}
						else
						{
							bNotCommand = TRUE;
						}	
					}
				}
				else
				{
					bNotCommand = TRUE;
				}
			}
		}
		else
			bNotCommand = TRUE;

		if( bNotCommand )
		{
			printf( "\n\nNot Command ==> Retry Command\n");
		}
		else
		{
			if( FALSE == g_Funtion.IsDirectory( szFlag, nYear, nMonth, nDay ) )
			{
				printf( "\n\nAlready Directory ==> Delete Directory Retry Command\n");
				continue;
			}
			g_Funtion.PatchexeFun( szServer, szFlag, nYear, nMonth, nDay );
		}

		Sleep( 1 );
	}
	return 0;
}
