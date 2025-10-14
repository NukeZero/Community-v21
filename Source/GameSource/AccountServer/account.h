#ifndef __ACCOUNT_H__
#define	__ACCOUNT_H__

#include <memory/FLMemPooler.h>
#include <map>
#include <string>

#define	MAX_CERTIFIER			32
const int LEFTIME_NOTIFIED		= 1;

//빌링의 결과값 
enum BILLING_RETURN
{
	SUCCESS				= 0,
	BILLING_INFO_FAILED = 1,
	DATABASE_ERROR		= 2,
	OTHER_ERROR			= 3,
	TIME_OVER			= 11
};

//struct BILLING_INFO
//{
//	const char*		szAccount;
//	DWORD			dwKey;
//	long			lResult;				// 빌링의 결과값 
//	const CTime*	pTimeOverDays; 
//	char			cbLastOneLogon;			// 대만에서 사용하는 플래그 'Y'일 경우만 30분통지를 한다. 
//
//	BILLING_INFO() : 
//		szAccount( NULL ), 
//		dwKey( 0 ),
//		lResult( OTHER_ERROR ),
//		cbLastOneLogon( 'N' )		
//	{
//	}
//};

enum ACCOUNT_STATUS
{
	ACCOUNT_STATUS_INITIAL,			// 초기상태 
	ACCOUNT_STATUS_NOTIFIED,		// 남은 시간을 알려준 상태 
	ACCOUNT_STATUS_SECONDQUERY,		// 2번째 쿼리결과를 기다리는 상태   
};

class CAccount
{
public:
//	Constructions
	CAccount();
	CAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, BYTE b18, int fCheck, const CTime & tmBirthDate, const TCHAR* pAddress );
	virtual	~CAccount();

	BOOL	IsAdult();
	void	SetTimeOverDays(  const CTime* pOverDays );
	//void	SendBillingResult( BILLING_INFO* pResult );

//	Attributes
public:
	const CTime		m_tmBirthDate;

	DWORD			m_dpid1;
	DWORD			m_dpid2;
	DWORD			m_dwIdofServer;
	BYTE			m_cbAccountFlag;
	DWORD			m_dwBillingClass;
	CTime			m_TimeOverDays;
	TCHAR			m_lpszAccount[MAX_ACCOUNT];
	BOOL			m_fRoute;
	DWORD			m_dwPing;
	DWORD			m_dwAuthKey;
	int				m_cbRef;
	int				m_fCheck;
	ACCOUNT_STATUS	m_nStatus;				// ACCOUNT_STATUS_INITIAL, ...
	char			m_cbLastOneLogon;		// 대만에서 사용하는 플래그 'Y'일 경우만 30분통지를 한다. 
	u_long			m_uIdofMulti;
	BOOL			m_bCertify;

	TCHAR			m_address[MAX_ADDRESS_LEN];

public:

	MEMPOOLER_DECLARE( CAccount );

};

class CAccountMng
{
private:
	std::map<std::string, CAccount*>	m_stringToPAccount;
	std::map<DWORD, int>			m_dpidToIndex;
	std::map<DWORD, CAccount*>	m_adpidToPAccount[MAX_CERTIFIER];
	int		m_nSizeof;
	int		m_nOldHour;
public:
	int						m_nCount;
	std::map<DWORD, DWORD>		m_2IdofServer;

	int		m_nYear;
	int		m_nMonth;
	int		m_nDay;
	int		m_nHour;
	int		m_nMinute;
public:
//	Constructions
	CAccountMng();
	virtual	~CAccountMng();
public:
//	Attributes
	int			GetIndex( DWORD dpid1 );
	int			GetIdofServer( DWORD dpid );
	CAccount*	GetAccount( LPCTSTR lpszAccount );
	CAccount*	GetAccount( DWORD dpid1, DWORD dpid2 );
	CAccount*	GetAccount( LPCTSTR lpszAccount, DWORD dwSerial );

	std::map<std::string, CAccount*> GetMapAccount();

//	Operations
	void	Clear( void );
	void	AddConnection( DWORD dpid );
	void	RemoveConnection( DWORD dpid );
	void	AddIdofServer( DWORD dpid, DWORD dwIdofServer );
	void	RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection = TRUE );

	BYTE	AddAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, DWORD* pdwAuthKey, BYTE b18, int fCheck, const CTime & tmBirthDate, const TCHAR* pAddress );
	void	RemoveAccount( LPCTSTR lpszAccount );	
	void	RemoveAccount( DWORD dpid1, DWORD dpid2 );
	void	PreventExcess();
	BOOL	IsTimeCheckAddr();
	//void	SendBillingResult( BILLING_INFO* pResult );

public:
	CMclCritSec		m_AddRemoveLock;

	void	PushPCBangPlayer( DWORD dwAuthKey, DWORD dwClass )	{ m_mapPCBang.insert( std::map<DWORD, DWORD>::value_type( dwAuthKey, dwClass ) ); }
	DWORD	PopPCBangPlayer( DWORD dwAuthKey );

private:
	std::map<DWORD, DWORD>	m_mapPCBang;

};

#endif	// __ACCOUNT_H__