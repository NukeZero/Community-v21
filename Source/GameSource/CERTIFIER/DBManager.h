#ifndef __DBMANAGER_H__
#define	__DBMANAGER_H__

#include <query.h>
#include <network/FLAr.h>
#include <map>
#include <string>
#include <set>

#include <memory/FLMemPooler.h>
#include "FLDBOverlappedPlus.h"

#include "FLIDNAuthClient.h"
#include "FLAuthWait.h"

#define	VALID_CRLF( n, r, s )	if( ( n ) < 0 )	{	r.nResult	= -2;	*r.szResult	= '\0';	FLERROR_LOG( PROGRAM_NAME, _T( "[ ***** %s ***** ]" ), (LPCTSTR)s );	return;		}
#define	VALID_STR( len, max, r, s )		if( ( len ) > ( max ) )	{	r.nResult	= -3;	*r.szResult	= '\0';	FLERROR_LOG( PROGRAM_NAME, _T( "[ ***** %s ***** ]" ), (LPCTSTR)s );	return;		}



const DWORD			DEFAULT_DB_WORKER_THREAD_NUM = 8;

enum QUERYMODE
{	
	CERTIFY	= 1,
	CLOSE_EXISTING_CONNECTION,
	CERTIFY_ACK,				// 인증 결과

	ACCOUNT_LOGOUT,
};

class CAccountMgr;

typedef	struct	_GPAUTH_RESULT
{
	int		nResult;
	char	szResult[256];
	char	szGPotatoID[20];
	int		nGPotatoNo;
	char	szNickName[20];
	BOOL	bGM;
	char	szCheck[255];
	_GPAUTH_RESULT()
	{
		nResult	= -1;
		*szResult	= '\0';
		*szGPotatoID	= '\0';
		nGPotatoNo	= 0;
		*szNickName		= '\0';
		bGM	= 0;
		*szCheck	= '\0';
	}
}	GPAUTH_RESULT;


	#define JAPAN_AUTH_RESULT_AGREE				0
//	#define JAPAN_AUTH_RESULT_CONVERTED_USER	0
	#define JAPAN_AUTH_RESULT_NEED_AGREEMENT	1
	#define JAPAN_AUTH_RESULT_ID_ERROR			2
	#define JAPAN_AUTH_RESULT_PASSWORD_ERROR	3
	#define JAPAN_AUTH_RESULT_BLOCK				4
	#define JAPAN_AUTH_RESULT_PARAMETER_ERROR	6
	#define JAPAN_AUTH_RESULT_REJECT			7
	#define JAPAN_AUTH_RESULT_HANGAME_ERROR		8
	#define JAPAN_AUTH_RESULT_FAIL				9


typedef	struct	_JAPAN_AUTH_RESULT
{
	int		nResult;
	char	szResultMsg[256];
	_JAPAN_AUTH_RESULT()
	{
		nResult = JAPAN_AUTH_RESULT_FAIL;
		memset( szResultMsg, 0, sizeof( szResultMsg ) );
	}
}	JAPAN_AUTH_RESULT;


typedef struct	_USA_AUTH_RESULT
{
	int		nResult;
	char	szResult[256];
	char	szGPotatoID[21];
	int		nGPotatoNo;
	char	szNickName[21];
	BOOL	bGM;
	char	szCheck[255];

// 	_USA_AUTH_RESULT()
// 	{
// 		nResult			= -1;
// 		*szResult		= '\0';
// 		*szGPotatoID	= '\0';
// 		nGPotatoNo		= 0;
// 		*szNickName		= '\0';
// 		bGM				= 0;
// 		*szCheck		= '\0';
// 	}
} T_USA_AUTH_RESLUT, *PT_USA_AUTH_RESLUT;

typedef struct	_PHILIPPINES_AUTH_RESULT
{
	int		nResult;
	char	szResult[256];
	char	szMaster_id[21];
	int		nAkey_flag;
	char	szRef[21];

} T_PHILIPPINES_AUTH_RESLUT;

typedef struct	_PHILIPPINES_AUTH_IPBONUS_CHECKIN_RESULT
{
	int		nBonus_Level;
	char	szMaster_id[50];
	char	szRef[32];

} T_PHILIPPINES_AUTH_IPBONUS_CHECKIN_RESULT;

typedef struct	_PHILIPPINES_AUTH_IPBONUS_CHECKOUT_RESULT
{
	int		nError_Code;
	char	szMaster_id[50];
	char	szRef[32];

} T_PHILIPPINES_AUTH_IPBONUS_CHECKOUT_RESULT;

class CDbManager
{
protected:
	HANDLE	m_hDbWorkerThread[DEFAULT_DB_WORKER_THREAD_NUM];
	HANDLE	m_hIOCP[DEFAULT_DB_WORKER_THREAD_NUM];

public:
	FLMemPooler<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
	std::set<std::string>							m_eveSchoolAccount;

private:
	TCHAR							m_szLogin_DSN_Name[256];
	TCHAR							m_szLogin_DB_AdminId[256];
	TCHAR							m_szLogin_PWD[256];
	

public:
	CDbManager();
	~CDbManager();

	TCHAR*	GetDBConnectString( DB_CONNECT_TYPE nType );
	void	SetDBConnectString( DB_CONNECT_TYPE nType, const char* pString );

	BOOL	CreateDbWorkers();
	void	DestroyDbWorkers();

	void	Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, CAccountMgr& accountMgr );
	void	CloseExistingConnection( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	SQLAddAccount( CQuery & query, char* szAccount, char* szPassword, BOOL bGM );

	void	Certify_GPotato( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr );
	void	CloseExistingConnection_GPotato( CQuery & query, LPDB_OVERLAPPED_PLUS pov );

	void	Certify_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr );
	void	CloseExistingConnection_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov );
	void	AccountLogOut_Japan( LPDB_OVERLAPPED_PLUS pov );

	void	Certify_USA( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr );
	void	CloseExistingConnection_USA( CQuery & query, LPDB_OVERLAPPED_PLUS pov );

	void	Certify_Philippines( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr );
	void	CloseExistingConnection_Philippines( CQuery & query, LPDB_OVERLAPPED_PLUS pov );

	void	Certify_Indonesia( LPDB_OVERLAPPED_PLUS pov, FLIDNAuthClient& cIDNAuth, FLAuthWait& cAuthWait );
	void	CertifyAck_Indonesia( BYTE byResult, CQuery & query, LPDB_OVERLAPPED_PLUS pov, FLIDNAuthClient& cIDNAuth, FLAuthWait& cAuthWait );
	void	CloseExistingConnection_Indonesia( CQuery & query, LPDB_OVERLAPPED_PLUS pov );
	void	AccountLogout_Indonesia( LPDB_OVERLAPPED_PLUS pov, FLIDNAuthClient& cIDNAuth );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	bool	DBQryAccount( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pData );		
	bool	Query_Account( CQuery & query, const LPDB_OVERLAPPED_PLUS pData );

	// END110119
	//////////////////////////////////////////////////////////////////////////

	BOOL	LoadEveSchoolAccount( void );
	BOOL	IsEveSchoolAccount( const char* pszAccount );
	BYTE	GetAccountFlag( int f18, LPCTSTR szAccount );
	bool	OnCertifyQueryOK( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, const char* szCheck = "" );
	HANDLE	GetIOCPHandle( int n );
	void	PostQ( LPDB_OVERLAPPED_PLUS pData, DWORD dwNumberOfBytesTransferred = 1 );
};

u_int	__stdcall	DbWorkerThread( LPVOID lpDbManager );	// DbWorkerThread

u_int	__stdcall	GPotatoAuthWorker( LPVOID pParam );
void	GetGPAuthResult( const char* szUrl, int nMode, int nGameMode, const char* sAccount, const char* sPassword, const char* sAddr, GPAUTH_RESULT & result );

u_int __stdcall		JapanAuthWorker( LPVOID pParam );
JAPAN_AUTH_RESULT	GetJapanAuthResult( LPDB_OVERLAPPED_PLUS pov );
DWORD				NotiAccountLogOut_Japan( LPDB_OVERLAPPED_PLUS pov );


u_int	__stdcall	USAAuthWorker( LPVOID pParam );
void				GetUSAAuthResult( LPDB_OVERLAPPED_PLUS pov, T_USA_AUTH_RESLUT & result );

//u_int __stdcall PhilippinesAuthWorker( LPVOID pParam );
void				GetPhilippinesAuthResult( LPDB_OVERLAPPED_PLUS pov, T_PHILIPPINES_AUTH_RESLUT & result );
void				CheckInIPBonus( DWORD dpId, T_PHILIPPINES_AUTH_RESLUT result );
void				CheckOutIPBonus();

u_int __stdcall		IndonesiaAuthWorker( LPVOID pParam );

#endif	// __DBMANAGER_H__