#ifndef __POCKET_H__
#define	__POCKET_H__

#include "FLItemElem.h"

#include <network/FLAr.h>


typedef struct _T_POCKET_PROPERTY
{
	BOOL	bExpired;
	time_t	tExpirationDate;

	_T_POCKET_PROPERTY()
	{
		bExpired		= TRUE;
		tExpirationDate	= 0;
	}

} T_POCKET_PROPERTY, *PT_POCKET_PROPERTY;

class CPocketController
{
public:
// Constructions
	CPocketController();
	virtual	~CPocketController();
#ifdef __WORLDSERVER
	void	Init( CMover* pMover )	{	m_pMover	= pMover;	}
#endif	// __WORLDSERVER
	void	Clear( void );
// Operations
	FLItemElem*	GetAtId( int nPocket, DWORD dwItemObjID, BOOL bExpiration = TRUE );	
	void	SetAtId( int nPocket, OBJID dwObjId, FLItemElem* pItemElem );
	BOOL	Add( int nPocket, FLItemElem*pItemElem );
	BOOL	RemoveAtId( int nPocket, DWORD dwItemObjID, int nNum, BOOL bExpiration = TRUE );		// if nNum < 0, remove all.
	int		RemoveItem( DWORD dwItemId, int nNum );
	void	RemoveAllItem();
	
	int		GetEmptyCount( void );
	BOOL	IsAllClean( void );
	int		GetCount( DWORD dwItemId );
	
	BOOL	IsAvailable( int nPocket, BOOL bExpiration = TRUE );
	void	Avail( int nPocket, time_t t = 0 );

	enum	{	expired, avail,	};
	void	SetAttribute( int nAttribute, int nPocket, int nData );
	time_t	GetAvailable( int nPocket );

	void	Copy( CPocketController & rPocketController );
	void	Serialize( CAr & ar );

	void	SetExpired( int nPocket, BOOL bExpired );
	BOOL	IsExpired( int nPocket );
	void	SetExpirationDate( int nPocket, time_t tExpirationDate );
	time_t	GetExpirationDate( int nPocket );

	BOOL	IsExistDoNotHaveItemServerTransform();

#ifdef __WORLDSERVER
	void	ProcessExpiring( void );
#endif	// __WORLDSERVER
//	CPocket*	GetPocket( int nPocket );

#ifdef __CLIENT
	FLItemElem*	GetAt( int nPocket, int nIndex );
#endif	// __CLIENT

	CItemContainer< FLItemElem >		m_kPocket[MAX_CHARACTER_POCKET];

private:
#ifdef __WORLDSERVER
	CMover*	m_pMover;
#endif	// __WORLDSERVER

private:
	T_POCKET_PROPERTY	m_kPocketProperty[MAX_CHARACTER_POCKET];
	
};
#endif	// __POCKET_H__