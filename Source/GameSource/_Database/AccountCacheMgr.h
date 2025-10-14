#ifndef _ACCOUNTCACHEMANAGER_H_
#define _ACCOUNTCACHEMANAGER_H_

struct ACCOUNT_CACHE;
class CAccountCacheMgr
{
private:
	std::map<std::string, ACCOUNT_CACHE*>	m_2Account;
	FLMemPooler<ACCOUNT_CACHE>*	m_pAccountMemPooler;
	std::list< ACCOUNT_CACHE* >		m_list;
	int							m_nAlloc;
public:
	CAccountCacheMgr();
	ACCOUNT_CACHE*	Find( LPCTSTR szAccount );
	void			Clear();
	void			AddMover( u_long idPlayer, LPCTSTR szAccount, int nSlot );
	void			RemoveMover( u_long idPlayer, LPCTSTR szAccount );
	void			AddAccount( u_long idPlayer, LPCTSTR szAccount, BOOL bCache, ACCOUNT_CACHE *pCache );
	ACCOUNT_CACHE*	GetAccount( LPCTSTR szAccount, BOOL *pbCacheHit );

	void	ChangeMultiServer( LPCTSTR szAccount, u_long uMultiServer );
	int		InitMultiServer( u_long uMultiServer );
};


#endif // _ACCOUNTCACHEMANAGER_H_