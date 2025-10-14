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


class CObjMap : public CMyMap<CCtrl*>
{
public:
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

	}

	OBJID id	= pCtrl->GetId();

	if( !Lookup( id, pTemp ) )
	{
		SetAt( id, pCtrl );
		return TRUE;
	}
	else
	{
		assert( 0 );
		return FALSE;
	}
}

#endif // __OBJMAP_H__