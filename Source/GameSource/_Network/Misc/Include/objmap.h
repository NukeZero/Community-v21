#ifndef __OBJMAP_H__
#define __OBJMAP_H__

#pragma once

#ifdef __CORESERVER
#include "..\..\Objects\Obj.h"
#include "Data.h"
#else	// __CORESERVER
#include "Ctrl.h"
#include "Data.h"
#endif	// __CORESERVER


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
				FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
			FLASSERT( !IsEmpty() );
			return Pop();
		}

	int GetCount( void )
		{
//			CMclAutoLock Lock( m_AccessLock );
			return m_nTop;
		}
};
#endif

class CObjMap 
{
public:
#ifndef __CLIENT
	CIdStack					m_idStack;
#endif
	std::map<DWORD, CCtrl*>		m_map;

public:
	BOOL		Add( CCtrl* pCtrl );
	BOOL		RemoveKey( DWORD dwKey );
	BOOL		Lookup( DWORD dwKey, CCtrl* & value );
};

inline BOOL CObjMap::Lookup( DWORD dwKey, CCtrl* & value )
{
	std::map<DWORD, CCtrl*>::iterator it = m_map.find( dwKey );
	if( it == m_map.end() )
		return FALSE;

	value = it->second;
	return TRUE;
}

inline BOOL CObjMap::RemoveKey( DWORD dwKey )
{
	return m_map.erase( dwKey ) > 0;
}

inline BOOL CObjMap::Add( CCtrl* pCtrl )
{
	OBJID id = pCtrl->GetId();
	if( id == NULL_ID )
	{
	#ifdef __CLIENT
		FLASSERT( 0 );
		return FALSE;
	#else
		id = m_idStack.GetId();
		pCtrl->SetId( id );
	#endif
	}
	//else
	//{
	//	int i = 0;
	//}

	if( m_map.insert(std::make_pair( id, pCtrl ) ).second )
		return TRUE;

	FLASSERT( 0 );
	return FALSE;
}


#endif // __OBJMAP_H__