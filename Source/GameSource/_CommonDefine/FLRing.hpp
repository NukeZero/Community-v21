#pragma once

#include "FLRing.h"

template < class _Type >
FLRing< _Type >::
FLNode::FLNode()
{
	m_pRNext			= NULL; 
	m_pRPrev			= NULL; 
}

template < class _Type >
FLRing< _Type >::
FLNode::~FLNode()
{
	Unlink();
}

template < class _Type >
bool				
FLRing< _Type >::
FLNode::Unlink()
{
	if( m_pRNext == NULL || m_pRPrev == NULL )
		return false;

	m_pRNext->m_pRPrev		= m_pRPrev;
	m_pRPrev->m_pRNext		= m_pRNext;

	m_pRNext				= NULL;
	m_pRPrev				= NULL;
	return true;
}

//-------------------------------------------------------------------------------------------------------

template < class _Type >
FLRing< _Type >::FLRing()
{	
	m_stHeadNode.m_pRPrev	= m_stHeadNode.m_pRNext = &m_stHeadNode; 			
}

template < class _Type >
FLRing< _Type >::~FLRing() 
{
	this->Clear();
}


template < class _Type >
bool
FLRing< _Type >::UnlinkNode( _Type * pObj )
{
	if( pObj == NULL )
		return false;

	if( pObj == &m_stHeadNode )
		return false;

	if( pObj->m_pRNext == NULL || pObj->m_pRPrev == NULL )
		return false;

	pObj->m_pRNext->m_pRPrev	= pObj->m_pRPrev; 
	pObj->m_pRPrev->m_pRNext	= pObj->m_pRNext; 

	pObj->m_pRNext			= NULL; 
	pObj->m_pRPrev			= NULL; 

	return true;
}

template < class _Type >
bool 
FLRing< _Type >::insertForward( FLNode * pSrc, FLNode * pDest )  	
{
	if( pSrc == NULL || pDest == NULL )
		return false;

	if( pSrc->m_pRNext == NULL || pSrc->m_pRPrev == NULL )
		return false;

	if( pDest->m_pRNext != NULL || pDest->m_pRPrev != NULL )
		return false;

	pDest->m_pRNext		= pSrc;
	pDest->m_pRPrev		= pSrc->m_pRPrev;

	pSrc->m_pRPrev			= pDest;
	pDest->m_pRPrev->m_pRNext = pDest;

	return true;
}

template < class _Type >
bool 
FLRing< _Type >::insertBack( FLNode * pSrc, FLNode * pDest )
{
	if( pSrc == NULL || pDest == NULL )
		return false;

	if( pSrc->m_pRNext == NULL || pSrc->m_pRPrev == NULL )
		return false;	

	if( pDest->m_pRNext != NULL || pDest->m_pRPrev != NULL )
		return false;

	pDest->m_pRPrev		= pSrc;
	pDest->m_pRNext		= pSrc->m_pRNext;

	pSrc->m_pRNext			= pDest;
	pDest->m_pRNext->m_pRPrev = pDest;

	return true;
}


template < class _Type >
bool					
FLRing< _Type >::InsertBack( iterator it, _Type * pDest )
{
	if( it._MyNode() == NULL )
		return false;

	return insertBack( it._MyNode(), pDest );
}

template < class _Type >
bool					
FLRing< _Type >::InsertForward( iterator it, _Type * pDest )
{
	if( it._MyNode() == NULL )
		return false;

	return insertForward( it._MyNode(), pDest );
}

template < class _Type >
typename FLRing< _Type >::iterator
FLRing< _Type >::PopHead()
{
	FLNode * pNode		= m_stHeadNode.m_pRNext;

	return ( UnlinkNode( static_cast< _Type * >( pNode ) ) == TRUE )
		? iterator( pNode )
		: iterator( End() );
}

template < class _Type >
typename FLRing< _Type >::iterator
FLRing< _Type >::PopTail()
{
	FLNode * pNode		= m_stHeadNode.m_pRPrev;

	return ( UnlinkNode( static_cast< _Type * >( pNode ) ) == TRUE )
		? iterator( pNode )
		: iterator( End() );
}

template < class _Type >
typename FLRing< _Type >::iterator
FLRing< _Type >::Begin()
{
	FLNode * pNode		= m_stHeadNode.m_pRNext;
	if( pNode == & m_stHeadNode )
		return End();

	return iterator( pNode );
}

template < class _Type >
typename FLRing< _Type >::iterator
FLRing< _Type >::Tail()
{
	FLNode * pNode		= m_stHeadNode.m_pRPrev;
	if( pNode == & m_stHeadNode )
		return End();

	return iterator( pNode );
}

template < class _Type >
typename FLRing< _Type >::iterator
FLRing< _Type >::End()	
{
	return iterator( &m_stHeadNode );
}


template < class _Type >
typename FLRing< _Type >::iterator				
FLRing< _Type >::Erase( iterator it )
{
	FLNode * pNode		= it._MyNode();
	if( UnlinkNode( static_cast< _Type * >( pNode ) ) == TRUE )
	{
		++it;
		return it;
	}
	else
		return End();
}

template < class _Type >
void
FLRing< _Type >::Clear()
{
	for( ;; )
	{
		FLNode * pNode		= m_stHeadNode.m_pRPrev;
		if( pNode == & m_stHeadNode )
			break;

		UnlinkNode( static_cast< _Type * >( pNode ) );
	}
}

