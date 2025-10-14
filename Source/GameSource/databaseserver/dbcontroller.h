#ifndef __DBCONTROLLER_H__
#define	__DBCONTROLLER_H__


////////////////////////////////////////////////////////////////////////////////
class CDbControllerTimer;
class CDbController
{
public:
	enum	{	eTimer	= -1,	};
public:
	CDbController();
	virtual	~CDbController();

	BOOL	CreateDbHandler( DWORD dwMilliseconds = INFINITE );
	void	CloseDbHandler( void );
	BOOL	PostRequest( int nQuery, BYTE* lpBuf, u_long nBufSize, ULONG_PTR dwCompletionKey );
	void	SetQueryObject( CQuery* pQuery )	{	m_pQuery	= pQuery;	}
	CQuery*	CreateQuery( void );
	CQuery*	GetQueryObject( void )	{	return m_pQuery;	}

	static	u_long	_DbHandler( LPVOID* pParam );
	void	DbHandler( void );

	virtual	void	Handler( LPDB_OVERLAPPED_PLUS pov, ULONG_PTR dwCompletionKey )		= 0;
	virtual void	OnTimer()	{}
	DWORD	GetTimer()	{	return m_dwTimer;	}

private:
	void	SetTimer( DWORD dwMilliseconds );

private:
	HANDLE	m_hIocp;
	HANDLE	m_hDbHndlr;
	CQuery*		m_pQuery;
	DWORD	m_dwTimer;
};


////////////////////////////////////////////////////////////////////////////////
typedef std::pair<CDbController*, DWORD>	PDD;
typedef std::vector<PDD>	VPDD;

class CDbControllerTimer
{
public:
	virtual	~CDbControllerTimer();
	void	Register( CDbController* dbCtrl );
	void	Create( void );
	void	Destroy( void );
	static CDbControllerTimer*	GetInstance( void );
private:
	CDbControllerTimer();
	static	u_int	_Tick( LPVOID pParam );
	void	Tick( void );

private:
	VPDD	m_vDbController;
	CMclCritSec	m_csLock;
	HANDLE	m_hTickThread;
	HANDLE	m_hCloseTick;
};


////////////////////////////////////////////////////////////////////////////////
// namespace	post	{
// 	void	PostItem( CQuery* pQuery, const u_long idReceiver, const DWORD dwItemId, const int nItemNum, const char * szTitle, const char * szText );
// 	void	Post( u_long idReceiver, CMail* pMail, CQuery* pQuery );
// 
// 	//neodeath : Mail率篮 楼刘唱 东户 内靛
// 	void	QueryCooperativeContributions_RewarItemByMail( const u_long idReceiver, const DWORD dwContributionID, const FLTm tExpire, CMail* pMail, CQuery* pQuery );
// 	void	ExecCooperativeContributions_RewarItemByMail( CQuery & Query, const u_long idReceiver, const DWORD dwContributionID, const FLTm tExpire
// 					, const DWORD dwItemId, const int nItemNum, const char * szTitle, const char * szText );
// };


#endif	// __DBCONTROLLER_H__