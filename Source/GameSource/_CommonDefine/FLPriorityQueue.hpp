#pragma once

#include "FLPriorityQueue.h"


template < class _Key, class _Ptr >
FLPriorityQueue< _Key, _Ptr >::FLPriorityQueue()
{

}

template < class _Key, class _Ptr >
FLPriorityQueue< _Key, _Ptr >::~FLPriorityQueue() 
{
	List::iterator it	= m_list.Begin();
	while( it != m_list.End() )
	{
		it	= m_list.Erase( it );
	}
}

template < class _Key, class _Ptr >
void					
FLPriorityQueue< _Key, _Ptr >::Push( const _Key key, const _Ptr & t )
{
	List::iterator it	= m_list.Begin();
	for( ; it != m_list.End(); ++it )
	{
		if( it->key >= key )
		{
			t->key				= key;
			m_list.InsertForward( it, static_cast< FLNode * >( t ) );
			return;
		}
	}

	t->key				= key;
	m_list.InsertForward( it, static_cast< FLNode * >( t ) );
}

template < class _Key, class _Ptr >
BOOL					
FLPriorityQueue< _Key, _Ptr >::PopFront( _Key & o_key, _Ptr & o_t )
{
	List::iterator it	= m_list.Begin();
	if( it == m_list.End() )
		return FALSE;

	o_key				= it->key;
	o_t					= static_cast< _Ptr >( it._MyType() );

	it	= m_list.Erase( it );
	return TRUE;
}

template < class _Key, class _Ptr >
BOOL					
FLPriorityQueue< _Key, _Ptr >::Top( _Key & o_key, _Ptr & o_t )
{
	List::iterator it	= m_list.Begin();
	if( it == m_list.End() )
		return FALSE;

	o_key				= it->key;
	o_t					= static_cast< _Ptr >( it._MyType() );
	return TRUE;
}