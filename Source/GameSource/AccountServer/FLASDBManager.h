#ifndef __FLASDBMANAGER_H__
#define	__FLASDBMANAGER_H__

#include <query.h>
#include <network/FLAr.h>
#include "account.h"


enum QUERYMODE
{	
	ACCOUNT_ON			// 유저로그인 
	, ACCOUNT_OFF		// 유저로그아웃  
	, LOG_SM_ITEM		// 유료화아이템 관련 
	, QUERY_BILLINGINFO	// 빌링정보를 쿼리 
	, RELOAD_PROJECT
};



typedef	struct _T_AS_DBOVERLAPPED
{
	OVERLAPPED		Overlapped;
	int				nQueryMode;
	char			szQuery[4096];
	char			szAccount[MAX_ACCOUNT];
	DWORD			dwKey;		// 요청, 로그아웃, 로그인, 응답의 케이스에 account를 구분하기 위해서 사용한다.
	int				nExtra;
	BYTE			lpBuf[2048];
	DWORD			dwBufSize;

}	T_AS_DBOVERLAPPED, *PT_AS_DBOVERLAPPED;


class	FLASDBManager
{
	public:
		static	FLASDBManager *	GetInstance();

	protected:
		enum	{	DEFAULT_DB_WORKER_THREAD_NUM	= 8	};
		HANDLE							m_hDbWorkerThreadTerminate[DEFAULT_DB_WORKER_THREAD_NUM];

	public:
		FLMemPooler<T_AS_DBOVERLAPPED>*	m_pDbIOData;
		HANDLE							m_hDbCompletionPort;
		char							m_szLoginPWD[256];
		char							m_szLogPWD[256];

	public:
	//	Constructions
		FLASDBManager();
		~FLASDBManager();

	//	Operations
		void	CloseDbWorkers( void );
		void	CreateDbWorkers( void );

		void	GetStrTime( CTime *time, char *strbuf );
		void	DBQryAccount( char* qryAccount, size_t cchAccount, char* Gu, char* szAccount = '\0' );
		void	UpdateTracking( BOOL bON, LPCTSTR lpszAccount );
		bool	AllOff();
		void	AccountOn( CQuery & qry, PT_AS_DBOVERLAPPED lpDbOverlappedPlus );
		void	AccountOff( CQuery & qry, PT_AS_DBOVERLAPPED lpDbOverlappedPlus );
		void	LogSMItem( CQuery & qryLog, PT_AS_DBOVERLAPPED lpDbOverlappedPlus );
		void	DBQryLogSMItem( char* szSql, size_t cchSql, T_LOGSTR_DATA & info);

};

#define g_xASDBManager	FLASDBManager::GetInstance()

u_int	__stdcall	DbWorkerThread( LPVOID lpDbManager );	// DbWorkerThread

#endif	// __FLASDBMANAGER_H__