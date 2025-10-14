#ifndef __COMPANY_H__
#define	__COMPANY_H__

#include "mempooler.h"
#include <map>
#include <string>
#include "ar.h"
#ifdef __J0103
#include "projectcmn.h"
#endif	// __J0103

#ifdef __CORESERVER
class CPlayer;
#endif	// __CORESERVER

class CCompanyMember
{
public:
	u_long	m_idPlayer;
public:
//	Constructions
	CCompanyMember();
	CCompanyMember( u_long idPlayer );
	~CCompanyMember();
//	Operations
	void	Serialize( CAr & ar );
public:
	static	MemPooler<CCompanyMember>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CCompanyMember::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CCompanyMember::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CCompanyMember::m_pPool->Free( (CCompanyMember*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CCompanyMember::m_pPool->Free( (CCompanyMember*)lpMem );	}
};

#define	MAX_COMPANY_MEMBER	20
#define	MAX_COMPANY_NAME	260
#define	MAX_SQL_COMPANY_NAME	16
class CCompany
{
public:
	u_long	m_idCompany;
	char	m_szCompany[MAX_COMPANY_NAME];
	map<u_long, CCompanyMember*>	m_mapMemberPtr;
	u_long	m_idLeader;
#ifdef __J0103
	int		m_nLevel;
#endif	// __J0103
public:
//	Constructions
	CCompany();
	CCompany( u_long idCompany, const char* szCompany, u_long idLeader );
	~CCompany();
	void	Clear( void );
//	Operations
//	If an error occurs, return nonzero.
	void	AddMember( u_long idPlayer );
	BOOL	RemoveMember( u_long idPlayer );
	BOOL	IsMember( u_long idPlayer )		{	return ( GetMember( idPlayer ) != NULL );	}
	BOOL	IsLeader( u_long idPlayer )		{	return ( m_idLeader == idPlayer );	}
	void	Serialize( CAr & ar );
	void	SerializeEx( CAr & ar );
//	Attributs
	int		GetSize()	{	return m_mapMemberPtr.size();	}
	CCompanyMember*	GetMember( u_long idPlayer );
public:
	static	MemPooler<CCompany>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CCompany::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CCompany::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CCompany::m_pPool->Free( (CCompany*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CCompany::m_pPool->Free( (CCompany*)lpMem );	}
};

class CCompanyMng
{
private:
	map<u_long, CCompany*>	m_mapCompanyPtr;
	map<string, CCompany*>	m_mapCompanyPtr2;
#ifdef __J0103
	ExpCompany	m_aExp[MAX_COMPANYLEVEL];
	int		m_nMaxExp;
#endif	// __J0103
public:
	u_long	m_id;	// loaded
	CRIT_SEC	m_AddRemoveLock;
public:
//	Constructions
	CCompanyMng();
	~CCompanyMng();
	void	Clear( void );
//	Operations
	u_long	CreateCompany( u_long idPlayer, const char* szCompany, u_long idCompany = 0 );
	BOOL	DestroyCompany( u_long idCompany );
	CCompany*	GetCompany( u_long idCompany );
	CCompany*	GetCompany( const char* szCompany );

#ifdef __CORESERVER
	void	AddConnection( CPlayer* pPlayer );
	void	RemoveConnection( CPlayer* pPlayer );
#endif	// __CORESERVER
#ifdef __J0103
#ifdef __DBSERVER
	void	AddExpTable( int nExp, int nPenya, int cbMax );
#endif	// __DBSERVER
#endif	// __J0103
	void	Serialize( CAr & ar );
	void	SerializeEx( CAr & ar );	// information
//	Attributs
	int		GetSize( void )		{	return m_mapCompanyPtr.size();	}
};

#endif	// __COMPANY_H__