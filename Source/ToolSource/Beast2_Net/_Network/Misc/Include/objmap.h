#ifndef __OBJMAP_H__
#define __OBJMAP_H__

#pragma once

#ifdef __CORESERVER
#include "..\..\Objects\Obj.h"
#include "MyMap.h"
#include "Data.h"
#else	// __CORESERVER
#include "Ctrl.h"
#include "MyMap.h"
#include "Data.h"
#endif	// __CORESERVER

#include <assert.h>

#ifdef __CORESERVER
class CIdStack
{
private:
	OBJID	m_id;
	CMclCritSec		m_AccessLock;

public:
	CIdStack()	{	m_id	= 0;	}
	virtual	~CIdStack()	{}
	OBJID	GetIdBlock( u_long uSize )	{	CMclAutoLock Lock( m_AccessLock );	m_id	+= uSize;	return ( m_id - uSize );	}
	OBJID	GetId( void )	{	CMclAutoLock Lock( m_AccessLock );	return m_id++;	}
};
#endif

#ifdef __CLIENT
#ifndef __ONLINE
class CIdStack
{
private:
	OBJID	m_id;

public:
	CIdStack()	{	m_id	= 0;	}
	virtual	~CIdStack()	{}
	OBJID	GetId( void )	{	return m_id++;	}
};
#endif
#endif

#ifdef __WORLDSERVER
class CIdStack : public CStack<OBJID>
{
private:
	CMclCritSec		m_AccessLock;

public:
	CIdStack()
		{
			SetSize( 125440, 16384 );
		}
	virtual	~CIdStack()	{}
	void	PushIdBlock( OBJID idBase, u_long uSize )
		{
			CMclAutoLock Lock( m_AccessLock );
			for( DWORD i=0; i<uSize; i++ )
				Push( idBase + i );
		}
	OBJID GetId( void )
		{
			CMclAutoLock Lock( m_AccessLock );
			if( IsEmpty() )
				WriteLog( "%s, %d", __FILE__, __LINE__ );
			assert( !IsEmpty() );
			return Pop();
		}

	int GetCount( void )
		{
//			CMclAutoLock Lock( m_AccessLock );
			return m_nTop;
		}
};
#endif

class CObjMap : public CMyMap<CCtrl*>
{
public:
#ifndef __BEAST
#ifndef __ONLINE
	CIdStack	m_idStack;
#endif
#endif

public:
	// Constructions
	CObjMap()
		{
			SetSize( 1024, 8192, 1024 );
		}

	virtual ~CObjMap()
		{

		}

	// Operations
	BOOL	Add( CCtrl* pCtrl );
};

inline BOOL CObjMap::Add( CCtrl* pCtrl )
{
	CCtrl* pTemp;

	if( NULL_ID == pCtrl->GetId() )
	{
#ifdef __CLIENT
#ifndef __ONLINE
		pCtrl->SetId( m_idStack.GetId() );
#else	// __ONLINE
		ASSERT( 0 );
		return FALSE;
#endif	// __ONLINE
#else	// __CLIENT
#ifndef __BEAST
		pCtrl->SetId( m_idStack.GetId() );
#endif	// __BEAST
#endif	// CLIENT
	}

	OBJID id	= pCtrl->GetId();

	if( !Lookup( id, pTemp ) )
	{
		SetAt( id, pCtrl );
		return TRUE;
	}
	else
	{
#ifdef __WORLDSERVER
		WriteLog( "CObjMap::Add(): %d\n", id );
#endif	// __WORLDSERVER
		assert( 0 );
		return FALSE;
	}
}

#endif // __OBJMAP_H__