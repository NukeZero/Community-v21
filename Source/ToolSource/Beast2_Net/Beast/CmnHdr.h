#ifndef __CMNHDR_H__
#define __CMNHDR_H__

//#define FD_SETSIZE	1024

#include <Winsock2.h>
#include <tlhelp32.h.>
#include "CMcl.h"
  
#pragma warning( disable:4001 )	// nonstandard extension 'single line comment' was used
#pragma warning( disable:4100 )	// unreferenced formal parameter
#pragma warning( disable:4699 )	// Note: Creating precompiled header 
#pragma warning( disable:4710 )	// function not inlined
#pragma warning( disable:4514 )	// unreferenced inline function has been removed
#pragma warning( disable:4512 )	// assignment operator could not be generated
#pragma warning( disable:4310 )	// cast truncates constant value
#pragma warning( disable:4786 )	// identifier was truncated to '255' characters in the browser information

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
	pvParam, fdwCreate, pdwThreadId)                 \
	((HANDLE)_beginthreadex(                      \
	(void *)        (psa),                     \
	(unsigned)      (cbStack),                 \
	(PTHREAD_START) (pfnStartAddr),            \
	(void *)        (pvParam),                 \
	(unsigned)      (fdwCreate),               \
	(unsigned *)    (pdwThreadId)))

#define chSTR2(x)   #x
#define chSTR(x)    chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)
#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))
#define chDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))
#define chSIZEOFSTRING(psz)   ((lstrlen(psz) + 1) * sizeof(TCHAR))

//#define IDHash(id,HashSize)	(((id) + (id>>8) + (id>>16) + (id>>24)) & (HashSize-1));
inline DWORD IDHash( DWORD id, size_t HashSize )
{
	return (((id) + (id>>8) + (id>>16) + (id>>24)) & (HashSize-1));
}


#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif

inline void chMB( PCSTR s )
{
	char szTMP[256];
	GetModuleFileNameA( NULL, szTMP, chDIMOF( szTMP ) );
	HWND hwnd	= GetActiveWindow();
}

inline void chMBANDDEBUG( PSTR szMsg )
{
	chMB( szMsg );
	DebugBreak();
}
/*
inline void chASSERTFAIL( LPCSTR file, int line, PCSTR expr )
{
	char sz[256];
	wsprintfA( sz, "File %s, line %d : %s", file, line, expr );
	chMBANDDEBUG( sz );
}
*/
inline BOOL IsPrime( u_long n )
{
	u_long i = 2, j = n;
	while( i < j ) {
		if( n % i == 0 )
			return FALSE;
		j = n / i++;
	}
	return TRUE;
}

inline int StringFind( const char* string, int c )
{
	const char* pdest;
	return ( pdest = strchr( string, c ) ) ? pdest - string : -1;
}

inline int StringReverseFind( const char* string, int c )
{
	const char* pdest;
	return ( pdest = strrchr( string, c ) ) ? pdest - string : -1;
}

inline void StringTrimLeft( char* & string )
{
	for(; *string != '\0'; string++ )
	{
		if( *string != ' ' )
			return;
	}
}

inline void StringTrimRight( char* string )
{
	int len	= strlen( string );
	for( int i = len-1; i >= 0; i-- )
	{
		if( string[i] != ' ' ) {
			string[i+1]		= '\0';
			return;
		}
	}
}

inline int GetIntFromStr(char *pBuf, int *pLocation)
{
	char strTemp[50];
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!='/' && pBuf[*pLocation]!=0) {
		strTemp[count]=pBuf[*pLocation];
		count++; (*pLocation)++;
	}
	strTemp[count]=0;(*pLocation)++;
	return atoi(strTemp);
}

inline void GetStrFromStr(char *pBuf, char *strReturn, int *pLocation)
{
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!='/' && pBuf[*pLocation]!=0) {
		strReturn[count]=pBuf[*pLocation];
		count++; (*pLocation)++;
	}
	strReturn[count]=0;(*pLocation)++;
}

inline int GetIntPaFromStr(char *pBuf, int *pLocation )
{
	char strTemp[50];
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!=0)
	{
		if( pBuf[*pLocation]!='/' )
		{
			strTemp[count]=pBuf[*pLocation];
			count++; (*pLocation)++;
		}
		else
		{
			if( count == 0 )
			{
				return 0;
			}
			else
			{
				strTemp[count]=0;
				return atoi(strTemp);
			}
		}
	}
	strTemp[count]=0;(*pLocation)++;
	return atoi(strTemp);
}

inline void GetStrPaFromStr(char *pBuf, char *strReturn, int *pLocation)
{
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!=0)
	{
		if( pBuf[*pLocation]!='/' )
		{
			strReturn[count]=pBuf[*pLocation];
			count++; (*pLocation)++;
		}
		else
		{
			if( count == 0 )
			{
				strReturn[0] = '0';
			}
			else
			{
				strReturn[count]=pBuf[*pLocation];
			}
			return;
		}
	}
	strReturn[count]=0;(*pLocation)++;
}

inline BOOL chWindows9x( void )
{
	OSVERSIONINFO vi	= { sizeof(vi) };
	GetVersionEx( &vi );
	return( vi.dwPlatformId != VER_PLATFORM_WIN32_NT );
}

inline HANDLE CreateNewCompletionPort( DWORD dwNumberOfConcurrentThreads )
{
	return( CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads ) );
}

class CSystemInfo : public SYSTEM_INFO
{
public:
	CSystemInfo()	{	GetSystemInfo( this );	}
};

#define	CLOSE_HANDLE(h)		\
	{	\
		if( h )	\
		{	\
			CloseHandle( h );	\
			( h )	= (HANDLE)NULL;	\
		}	\
	}

#define	CLOSE_WSAEVENT(h)	\
	{	\
		if( h != WSA_INVALID_EVENT )	\
		{	\
			WSACloseEvent( h );	\
			( h )	= WSA_INVALID_EVENT;	\
		}	\
	}

#define	CLOSE_THREAD( hThread, hClose )	\
	{	\
		if( hThread )	\
		{	\
			SetEvent( hClose );	\
			WaitForSingleObject( hThread, INFINITE );	\
			CloseHandle( hThread );	\
			CloseHandle( hClose );	\
			hThread		= hClose	= (HANDLE)NULL;	\
		}	\
	}

#define CLOSE_SOCKET( s )	\
	{	\
		if( s != INVALID_SOCKET )	\
		{	\
			shutdown( s, SD_BOTH );	\
			closesocket( s );	\
			( s )	= INVALID_SOCKET;	\
		}	\
	}

inline void	GetAddr( CHAR* lpAddr )
{
	WORD wVersionRequested	= MAKEWORD( 2, 2 );
	WSADATA wsaData;
	PHOSTENT hostinfo;
 
	if( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
//		if( LOBYTE( wsaData.wVersion ) == 2 &&
//			HIBYTE( wsaData.wVersion ) == 2 )
//		{
			CHAR name[260];
			if( gethostname( name, sizeof(name) ) == 0 )
			{
				if( ( hostinfo = gethostbyname( name ) ) )
					strcpy( lpAddr, inet_ntoa( *(struct in_addr*)*hostinfo->h_addr_list ) );
			}
//		}
	}
	WSACleanup( );
}


inline void	NotifyProcessStart( DWORD dwId )
{
	DWORD dwData = 0x00000001;
	HANDLE	hWriteHandle	= GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD dwBytesWritten;
	WriteFile( hWriteHandle, &dwData, sizeof(dwData), &dwBytesWritten, NULL );	

}


// do not use
BOOL	KillProcess( CString sExeName )
{
	sExeName.MakeUpper();
	HANDLE hSnapshot	= CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( (int)hSnapshot != -1 )
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize		= sizeof( PROCESSENTRY32 );
		BOOL fContinue;
		CString strProcessName;
		if( Process32First( hSnapshot, &pe32 ) )
		{
			do
			{
				strProcessName	= pe32.szExeFile;
				strProcessName.MakeUpper();
				if( (strProcessName.Find( sExeName, 0 ) != -1 ) )
				{
					HANDLE hProcess		= OpenProcess( PROCESS_ALL_ACCESS, 0, pe32.th32ParentProcessID );
					if( hProcess )
					{
						DWORD dwExitCode;
						GetExitCodeProcess( hProcess, &dwExitCode );
						TerminateProcess( hProcess, dwExitCode );
						CloseHandle( hProcess );
						CloseHandle( hSnapshot );
						return TRUE;
					}
					return FALSE;
				}
				fContinue	= Process32Next( hSnapshot, &pe32 );
			} while( fContinue );
		}
		CloseHandle( hSnapshot );
	}
	return FALSE;
}

typedef	struct	tagPLAY_ACCOUNT
{
	TCHAR	lpszAccount[36];
	SYSTEMTIME	m_stLogin;
	TCHAR	lpAddr[16];	
}
PLAY_ACCOUNT;


class CMover;
extern int ParsingCommand( LPCTSTR lpszString, CMover* pMover );

#define	MAX_ACCOUNT		42
#define	MAX_PASSWORD	42
#define	MAX_PLAYER	42

#define	CRIT_SEC	CMclCritSec
#define	theLineFile

#if defined (__WORLDSERVER) || defined(__CLIENT) || defined(__CORESERVER) || defined(__DBSERVER)
// SM약자 :: ㅋㅋ 쑈 미더 머니~ ㅋㄷㅋㄷ
// 적용할 시간을 정해야함, 추가되면 _ADDSMMODE 도 추가되야함
enum {	SM_BANK15, SM_BANK30, SM_PARTYSKILL15, SM_PARTYSKILL30, SM_SHOUT15, SM_SHOUT30, SM_MAINTAIN_MP, SM_MAINTAIN_FP, SM_MAX_HP, SM_MAX_HP50, SM_ACTPOINT
	, SM_STR_DOWN, SM_STA_DOWN, SM_DEX_DOWN, SM_INT_DOWN
	, SM_ATTACK_UP, SM_RESIST_ATTACK_LEFT, SM_RESIST_ATTACK_RIGHT, SM_RESIST_DEFENSE
	, SM_ESCAPE
	, SM_REVIVAL, SM_VELOCIJUMP
	, SM_MAX  };


#if defined (__WORLDSERVER) || defined(__CLIENT)
// 절대시간 : TRUE, 카운트 : FALSE
typedef struct _ADDSMMODE
{
	BOOL bSMModetime[SM_MAX];
	DWORD dwSMItemID[SM_MAX];
//	int	 nValue[SM_MAX];
	_ADDSMMODE()
	{
		memset( bSMModetime, 0, sizeof( BOOL ) * SM_MAX );
		memset( dwSMItemID, 0, sizeof( DWORD ) * SM_MAX );
//		memset( nValue, 0, sizeof( int ) * SM_MAX );
		
		bSMModetime[SM_BANK15] = TRUE;
		bSMModetime[SM_BANK30] = TRUE;
		bSMModetime[SM_PARTYSKILL15] = TRUE;
		bSMModetime[SM_PARTYSKILL30] = TRUE;
		bSMModetime[SM_SHOUT15] = TRUE;
		bSMModetime[SM_SHOUT30] = TRUE;
		
		dwSMItemID[SM_BANK15] = II_CHR_SYS_SCR_COMMBANK15;
		dwSMItemID[SM_BANK30] = II_CHR_SYS_SCR_COMMBANK30;
		dwSMItemID[SM_PARTYSKILL15] = II_CHR_SYS_SCR_PSKILLFULL15;
		dwSMItemID[SM_PARTYSKILL30] = II_CHR_SYS_SCR_PSKILLFULL30;
		dwSMItemID[SM_SHOUT15] = II_CHR_SYS_SCR_SHOUTFULL15;
		dwSMItemID[SM_SHOUT30] = II_CHR_SYS_SCR_SHOUTFULL30;
		dwSMItemID[SM_MAINTAIN_MP] = II_CHR_REF_REF_HOLD;
		dwSMItemID[SM_MAINTAIN_FP] = II_CHR_POT_DRI_VITALX;
		dwSMItemID[SM_MAX_HP] = II_CHR_FOO_COO_BULLHAMS;
		dwSMItemID[SM_MAX_HP50] = II_CHR_FOO_COO_GRILLEDEEL;
		dwSMItemID[SM_ACTPOINT] = II_CHR_SYS_SCR_ACTIVITION;

		dwSMItemID[SM_ATTACK_UP]				= II_CHR_SYS_SCR_UPCUTSTONE;
		dwSMItemID[SM_RESIST_ATTACK_LEFT]		= 0;
		dwSMItemID[SM_RESIST_ATTACK_RIGHT]		= 0;
		dwSMItemID[SM_RESIST_DEFENSE]			= 0;
	}
}
ADDSMMODE, *PADDSMMODE;
#endif // defined (__WORLDSERVER) || defined(__CLIENT)

#endif // defined (__WORLDSERVER) || defined(__CLIENT) || defined(__CORESERVER) || defined(__DBSERVER)

typedef	struct	_LogItemInfo	// ItemLog쓰임
{
	LPCTSTR Action;
	LPCTSTR SendName;
	LPCTSTR RecvName;
	DWORD	WorldId;
	DWORD	Gold;
	DWORD	Gold2;
	DWORD	ItemNo;
	DWORD	Negudo;
	LPCTSTR ItemName;
	DWORD	itemNumber;
	int		nAbilityOption;
	DWORD	Gold_1;
	int		nSlot;
	int		nSlot1;
	int		nItemResist;
	int		nResistAbilityOption;
	BOOL	m_bCharged;
	DWORD	m_dwKeepTime;
	int		m_nRandomOptItemId;
	int 	nPiercedSize;
	int		adwItemId0;
	int		adwItemId1;
	int		adwItemId2;
	int		adwItemId3;

	_LogItemInfo()
	{
		Action = "";
		SendName = "";
		RecvName = "";
		WorldId = Gold = Gold2 = 0;
		ItemNo = Negudo = 0;
		ItemName = "";
		itemNumber = 0;
		nAbilityOption = 0;
		Gold_1 = 0;
		nSlot = 100;
		nSlot1 = 100;
		nItemResist = 0;
		nResistAbilityOption = 0;
		m_bCharged = FALSE;
		m_dwKeepTime = 0;
		m_nRandomOptItemId = 0;
		nPiercedSize = 0;
		adwItemId0 = 0;
		adwItemId1 = 0;
		adwItemId2 = 0;
		adwItemId3 = 0;
	}
}	LogItemInfo,	*PLogItemInfo;

#endif	// __CMNHDR_H__

