
#ifndef	__FLDBOVERLAPPEDPLUS_H__
#define	__FLDBOVERLAPPEDPLUS_H__




typedef	struct	tagACCOUNT_INFO
{
	DWORD	dwPCBangClass;
	int		nSiteValue;	//	일본 인증 관련 변수

	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_ENCRYPT_PWD];
	char	szSessionPwd[MAX_SESSION_PWD];
	char	szBak[MAX_ACCOUNT];

	tagACCOUNT_INFO() : dwPCBangClass( 0 ), nSiteValue( 0 )
	{
		memset( szAccount, 0, sizeof( char ) * MAX_ACCOUNT );
		memset( szPassword, 0, sizeof( char ) * MAX_ENCRYPT_PWD );
		memset( szSessionPwd, 0, sizeof( char ) * MAX_SESSION_PWD );
		memset( szBak, 0, sizeof( char ) * MAX_ACCOUNT );
	}

}	ACCOUNT_INFO, *LPACCOUNT_INFO;


typedef	struct	tagAUTH_INFO
{
	ACCOUNT_INFO	AccountInfo;
	DWORD			dpId;
	DWORD			dwIP;		//connected ip

}	AUTH_INFO, *LPAUTH_INFO;

typedef	struct tagDB_OVERLAPPED_PLUS
{
	int				nQueryMode;
	AUTH_INFO		AuthInfo;

}	DB_OVERLAPPED_PLUS, *LPDB_OVERLAPPED_PLUS;






#endif