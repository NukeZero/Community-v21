#ifndef __CMNHDR_H__
#define __CMNHDR_H__


#include <Winsock2.h>
#include <tlhelp32.h.>

#if defined (__WORLDSERVER) || defined(__CLIENT)
extern	time_t	g_tCurrent;
#endif // ( __WORLDSERVER || __CLIENT )


#include "../_CommonDefine\FlyffCommonDefine.h"
#include "../_CommonDefine/FLTypes.h"

#include "../_CommonDefine/FLCommonStruct.h"

#include <network/FLAr.h>


// ',', '/', '\0' 을 만날때까지 값을 리턴하고 커서는 다음...
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

//sun: 11, 각성, 축복
inline	__int64 GetInt64PaFromStr( char *pBuf, int *pLocation )
{
	char strTemp[50];
	int count	=0;
	while( pBuf[*pLocation] != ',' && pBuf[*pLocation] != 0 )
	{
		if( pBuf[*pLocation] != '/' )
		{
			strTemp[count]	=pBuf[*pLocation];
			count++;	( *pLocation )++;
		}
		else
		{
			if( count == 0 )
			{
				return 0;
			}
			else
			{
				strTemp[count]	=0;
				return _atoi64( strTemp );
			}
		}
	}
	strTemp[count]	=0;	( *pLocation )++;
	return _atoi64( strTemp );
}


// ',', '\0' 을 만날때까지 값을 리턴하고 커서는 다음...
// '/' 를 만나면 값을 리턴하고 커서는 현재('/')...
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

inline SERIALNUMBER	GetSerialNumberPaFromStr(char *pBuf, int *pLocation )
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
				return (SERIALNUMBER)0;
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
				strReturn[0] = '\0';
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

#ifdef __CLIENT
inline BOOL IsAcValid( const TCHAR* lpszAccount, int nLang )
{
/*
	int len	= _tcslen( lpszAccount );
	if( len < 3 || len > 16 )
		return FALSE;

	if( _istalpha( *lpszAccount ) == 0 )
		return FALSE;

	for( int i = 0; i < len; i++ ) {
		if( _istalnum( lpszAccount[i] ) == 0 )
			return FALSE;
	}
*/
	CString strAccount	= lpszAccount;
	if( strAccount.IsEmpty() == FALSE )
	{
		CHAR c	= strAccount.GetAt( 0 );
		int nMaxLength = 20;
		//if ( nLang == 13 )	// 필리핀일때 계정 최대 길이 변경을 합니다.
		{
			nMaxLength = 40;
		}
		if( strAccount.GetLength() < 3 || strAccount.GetLength() > nMaxLength/*16*/ )
		{
			return FALSE;
		}
		else
		{
			for( int i = 0; i < strAccount.GetLength(); i++ )
			{
				c	= strAccount.GetAt( i );
				if( IsDBCSLeadByte( c ) == TRUE )
				{
					CHAR c2	= strAccount.GetAt( ++i );
					WORD word	= ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
					{
						return FALSE;
					}
					else if( isalnum( c ) == FALSE || iscntrl( c ) )
					{
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}

inline BOOL IsPwdValid( const TCHAR* lpszPassword )
{
	CString strPassword	= lpszPassword;
	if( strPassword.IsEmpty() == FALSE )
	{
		CHAR c	= strPassword.GetAt( 0 );
		if( strPassword.GetLength() < 3 || strPassword.GetLength() > 20/*16*/ )
		{
			return FALSE;
		}
		else
		{
			for( int i = 0; i < strPassword.GetLength(); i++ )
			{
				c	= strPassword.GetAt( i );
				if( IsDBCSLeadByte( c ) == TRUE )
				{
					CHAR c2	= strPassword.GetAt( ++i );
					WORD word	= ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
					if( IsHangul( word ) == FALSE ) 
					{
						return FALSE;
					}
					else if( isalnum( c ) == FALSE || iscntrl( c ) )
					{
						return FALSE;
					}
				}
			}
		}
	}
	return TRUE;
}
#endif	// __CLIENT

inline void	NotifyProcessStart( DWORD /*dwId*/ )
{
	DWORD dwData = 0x00000001;
	HANDLE	hWriteHandle	= GetStdHandle( STD_OUTPUT_HANDLE );
	if( hWriteHandle == NULL || hWriteHandle == INVALID_HANDLE_VALUE )
		return;

	DWORD dwBytesWritten;
	WriteFile( hWriteHandle, &dwData, sizeof(dwData), &dwBytesWritten, NULL );	
}


typedef	struct	tagPLAY_ACCOUNT
{
	TCHAR	lpszAccount[36];
	SYSTEMTIME	m_stLogin;
	TCHAR	lpAddr[16];	
}
PLAY_ACCOUNT;


class CMover;
extern int ParsingCommand( LPCTSTR lpszString, CMover* pMover, BOOL bItem = FALSE );
extern void ParsingEffect( TCHAR* pChar, size_t cchChar, int nLen );
extern	void RemoveCRLF( char* szString, size_t cchString );

#define RIJNDAEL_DEFAULT_BLOCK_SIZE		16

#define	MAX_ACCOUNT						42
#define	MAX_PASSWORD					42
#define	MAX_PLAYER						42
#define	MAX_SESSION_PWD					64

#define MAX_ENCRYPT_PWD					MAX_PASSWORD * RIJNDAEL_DEFAULT_BLOCK_SIZE

const UINT	MAX_ADDRESS_LEN			=	16;


#if defined (__WORLDSERVER) || defined(__CLIENT) || defined(__CORESERVER) || defined(__DBSERVER)
// SM약자 :: ㅋㅋ 쑈 미더 머니~ ㅋㄷㅋㄷ
// 적용할 시간을 정해야함, 추가되면 _ADDSMMODE 도 추가되야함
enum 
{	
	SM_BANK15
	, SM_BANK30
	, SM_PARTYSKILL15
	, SM_PARTYSKILL30
	, SM_SHOUT15
	, SM_SHOUT30
	, SM_MAINTAIN_MP
	, SM_MAINTAIN_FP
	, SM_MAX_HP
	, SM_MAX_HP50
	, SM_ACTPOINT
	, SM_STR_DOWN
	, SM_STA_DOWN
	, SM_DEX_DOWN
	, SM_INT_DOWN
	, SM_ATTACK_UP
	, SM_RESIST_ATTACK_LEFT
	, SM_RESIST_ATTACK_RIGHT
	, SM_RESIST_DEFENSE
	, SM_ESCAPE
	, SM_REVIVAL
	, SM_VELOCIJUMP
	, SM_PARTYSKILL1
	, SM_ATTACK_UP1
	, SM_SHOUT001
	, SM_BANK001
	, SM_ACTPOINT_30M
	, SM_REVIVAL2
	, SM_JUMP
	, SM_MAX
};


#if defined (__WORLDSERVER) || defined(__CLIENT)
// 절대시간 : TRUE, 카운트 : FALSE
typedef struct _ADDSMMODE
{
	BOOL bSMModetime[SM_MAX];
	DWORD dwSMItemID[SM_MAX];
	_ADDSMMODE()
	{
		memset( bSMModetime, 0, sizeof( bSMModetime ) );
		memset( dwSMItemID, 0, sizeof( dwSMItemID ) );
		
		bSMModetime[SM_BANK15]				= TRUE;
		bSMModetime[SM_BANK30]				= TRUE;
		bSMModetime[SM_BANK001]				= TRUE;
		bSMModetime[SM_PARTYSKILL1]			= TRUE;
		bSMModetime[SM_PARTYSKILL15]		= TRUE;
		bSMModetime[SM_PARTYSKILL30]		= TRUE;
		bSMModetime[SM_SHOUT15]				= TRUE;
		bSMModetime[SM_SHOUT30]				= TRUE;
		bSMModetime[SM_SHOUT001]			= TRUE;
		bSMModetime[SM_ESCAPE]				= TRUE;
		dwSMItemID[SM_BANK15]				= ITEM_INDEX( 10216, II_CHR_SYS_SCR_COMMBANK15 );
		dwSMItemID[SM_BANK30]				= ITEM_INDEX( 10217, II_CHR_SYS_SCR_COMMBANK30 );
		dwSMItemID[SM_BANK001]				= ITEM_INDEX( 26559, II_CHR_SYS_SCR_COMMBANK001 );
		dwSMItemID[SM_PARTYSKILL1]			= ITEM_INDEX( 30057, II_CHR_SYS_SCR_PSKILLFULL1 );
		dwSMItemID[SM_PARTYSKILL15]			= ITEM_INDEX( 10214, II_CHR_SYS_SCR_PSKILLFULL15 );
		dwSMItemID[SM_PARTYSKILL30]			= ITEM_INDEX( 10215, II_CHR_SYS_SCR_PSKILLFULL30 );
		dwSMItemID[SM_SHOUT15]				= ITEM_INDEX( 10212, II_CHR_SYS_SCR_SHOUTFULL15 );
		dwSMItemID[SM_SHOUT30]				= ITEM_INDEX( 10213, II_CHR_SYS_SCR_SHOUTFULL30 );
		dwSMItemID[SM_SHOUT001]				= ITEM_INDEX( 26557, II_CHR_SYS_SCR_SHOUTFULL001 );
		dwSMItemID[SM_MAINTAIN_MP]			= ITEM_INDEX( 10207, II_CHR_REF_REF_HOLD );
		dwSMItemID[SM_MAINTAIN_FP]			= ITEM_INDEX( 10208, II_CHR_POT_DRI_VITALX );
		dwSMItemID[SM_MAX_HP]				= ITEM_INDEX( 10209, II_CHR_FOO_COO_BULLHAMS );
		dwSMItemID[SM_MAX_HP50]				= ITEM_INDEX( 10210, II_CHR_FOO_COO_GRILLEDEEL );
		dwSMItemID[SM_ACTPOINT]				= ITEM_INDEX( 10218, II_CHR_SYS_SCR_ACTIVITION );
		dwSMItemID[SM_STR_DOWN]				= ITEM_INDEX( 10366, II_SYS_SYS_VIR_PENALTYSTR );
		dwSMItemID[SM_STA_DOWN]				= ITEM_INDEX( 10367, II_SYS_SYS_VIR_PENALTYSTA );
		dwSMItemID[SM_DEX_DOWN]				= ITEM_INDEX( 10369, II_SYS_SYS_VIR_PENALTYDEX );
		dwSMItemID[SM_INT_DOWN]				= ITEM_INDEX( 10368, II_SYS_SYS_VIR_PENALTYINT );
		dwSMItemID[SM_ATTACK_UP1]			= ITEM_INDEX( 30058, II_CHR_SYS_SCR_UPCUTSTONE01 );
		dwSMItemID[SM_ATTACK_UP]			= ITEM_INDEX( 10270, II_CHR_SYS_SCR_UPCUTSTONE );
		dwSMItemID[SM_RESIST_ATTACK_LEFT]	= 0;
		dwSMItemID[SM_RESIST_ATTACK_RIGHT]	= 0;
		dwSMItemID[SM_RESIST_DEFENSE]		= 0;
		dwSMItemID[SM_REVIVAL]				= ITEM_INDEX( 10430, II_SYS_SYS_SCR_BLESSING );
		dwSMItemID[SM_VELOCIJUMP]			= ITEM_INDEX( 10432, II_SYS_SYS_SCR_VELOCIJUMP );			
		dwSMItemID[SM_ESCAPE]				= ITEM_INDEX( 10435, II_CHR_SYS_SCR_ESCAPEBLINKWING );
		dwSMItemID[SM_ACTPOINT_30M]			= ITEM_INDEX( 20515, II_CHR_SYS_SCR_NEWACTIVITION );
		dwSMItemID[SM_REVIVAL2]				= ITEM_INDEX( 25197, II_SYS_SYS_SCR_BLESSING_02 );
		dwSMItemID[SM_JUMP]					= ITEM_INDEX( 25564, II_SYS_SYS_SCR_JUMPUP );
	}
}
ADDSMMODE, *PADDSMMODE;
#endif // defined (__WORLDSERVER) || defined(__CLIENT)

#endif // defined (__WORLDSERVER) || defined(__CLIENT) || defined(__CORESERVER) || defined(__DBSERVER)

const size_t	MAX_PIERCING_SUIT		=	4;
const size_t	MAX_PIERCING_WEAPON		=	10;
const size_t	MAX_PIERCING_ULTIMATE	=	5;
const size_t	MAX_PIERCING			=	10;
const size_t	MAX_VIS					=	9;

const BYTE		FAILED_BOTH_NEEDVIS		=	0;
const BYTE		FAILED_1ST_NEEDVIS		=	1;
const BYTE		FAILED_2ND_NEEDVIS		=	2;
const BYTE		SUCCSESS_NEEDVIS		=	3;
const BYTE		UNDEFINED_NEEDVIS		=	4;


inline time_t	time_null( void )
{
#if defined(__WORLDSERVER) || defined(__CLIENT)
	return	g_tCurrent;
#else	//
	return time( NULL );
#endif	//
}

#endif	// __CMNHDR_H__
