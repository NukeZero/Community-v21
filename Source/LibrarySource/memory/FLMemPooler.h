
#ifndef	__FLMEMPOOLER_H__
#define	__FLMEMPOOLER_H__

#include "FLMemoryCommon.h"

#include <list>


//#define DISABLE_MEMPOOLER 
//
//#ifdef DISABLE_MEMPOOLER
//
#define	MEMPOOLER_DECLARE( type )
#define	MEMPOOLER_IMPLEMENT( type, size )
#define	MEMPOOLER_DESTROY( type )	
//
//#else
//#define	MEMPOOLER_DECLARE( type )	\
//	static	FLMemPooler< type >*	s_pPool;	\
//	void*	operator new( size_t nSize )	{	return type::s_pPool->Alloc( nSize );	}	\
//	void	operator delete( void* pMem )	{	type::s_pPool->Free( reinterpret_cast< type* >( pMem ) );	}
//
//#define	MEMPOOLER_IMPLEMENT( type, size )	\
//	FLMemPooler< type >*	type::s_pPool = new FLMemPooler< type >( size )
//
//#define	MEMPOOLER_DESTROY( type )	\
//	SAFE_DELETE( type::s_pPool )

//#endif //DISABLE_MEMPOOLER


template< class T >
class	FLMemPooler
{
private:

//	struct	BlockNode
//	{
//		BlockNode*	pNext;
//
//#ifdef	_DEBUG
//		size_t		nSize;
//#endif
//
//		BlockNode()
//		{
//			pNext	= NULL;
//
//#ifdef	_DEBUG
//			nSize	= 0;
//#endif
//		}
//	};
//
//	const size_t		m_nObjectBlockSize;	// size per block
//	const int			m_nNumOfBlock;
//
//	std::list< void* >	m_listOfMemBlockPtr;
//
//	BlockNode*			m_pFreeList;
//	int					m_nAllocCount;
//
//	CRITICAL_SECTION	m_cs;

public:

	explicit FLMemPooler( int /*nNumOfBlock*/ ) 
		//:
		//m_nObjectBlockSize( sizeof( BlockNode ) + sizeof( T ) ),
		//m_nNumOfBlock( nNumOfBlock )
	{
		//::InitializeCriticalSection( &m_cs );

		//FLASSERT( m_nNumOfBlock > 0 );
		//m_pFreeList = NULL;
		//m_nAllocCount = 0;
	}

	~FLMemPooler()
	{
		//Destroy();

		//::DeleteCriticalSection( &m_cs );
	}

//	Operations
	T*		Alloc()
	{
		return new T;
//		BlockNode* pNode	= NULL;
//		T* lpMem			= NULL;
//
//		::EnterCriticalSection( &m_cs );
//
//		if( m_pFreeList == NULL )
//		{
//			if( Grow() == false )
//			{
//				::LeaveCriticalSection( &m_cs );
//				return NULL;
//			}
//		}
//
//		pNode		= m_pFreeList;
//		m_pFreeList	= m_pFreeList->pNext;
//
//		pNode->pNext = NULL;
//
//#ifdef	_DEBUG
//		pNode->nSize = sizeof( T );
//#endif
//
//		++m_nAllocCount;
//
//		lpMem	= reinterpret_cast<T*>( (pNode + 1) );
//
//		::LeaveCriticalSection( &m_cs );
//
//		return lpMem;
	}
	T*		Alloc( size_t /*nSize*/ )
	{
		return Alloc();
	}

	void	Free( T* lpMem )
	{
		delete lpMem;
		//BlockNode* pNode	= NULL;

		//::EnterCriticalSection( &m_cs );

		//pNode	= (reinterpret_cast<BlockNode*>( lpMem )) - 1;

		//FLASSERT( pNode->pNext == NULL );
		//FLASSERT( pNode->nSize == sizeof( T ) );

		//FLASSERT( m_nAllocCount > 0 );
		//if( m_nAllocCount > 0 )
		//{
		//	pNode->pNext	= m_pFreeList;
		//	m_pFreeList		= pNode;
		//	--m_nAllocCount;

		//	//	mulcom	BEGIN100422	메모리풀 초기화
		//	::memset( lpMem, 0, sizeof( T ) );
		//	//	mulcom	END100422	메모리풀 초기화
		//}

		//::LeaveCriticalSection( &m_cs );
	}

	int		GetAllocCount()	const
	{
		return 0;

		//::EnterCriticalSection( &m_cs );
		//int nAllocCount = m_nAllocCount;
		//::LeaveCriticalSection( &m_cs );

		//return nAllocCount;
	}

	int		GetFreeCount()	const
	{
		return 0;

		//BlockNode* pNode	= NULL;
		//int nCount = 0;

		//::EnterCriticalSection( &m_cs );

		//pNode = m_pFreeList;
		//while( pNode != NULL )
		//{
		//	pNode = pNode->pNext;
		//	++nCount;
		//}

		//::LeaveCriticalSection( &m_cs );

		//return nCount;
	}

private:

	// no copy
	FLMemPooler( const FLMemPooler< T >& );
	const FLMemPooler< T >&	operator=( const FLMemPooler< T >& );

	bool	Grow()
	{
		return true;
		//static const size_t AllocationSize	= m_nObjectBlockSize * m_nNumOfBlock;

		//void* pMemBlock		= malloc( AllocationSize );
		//FLASSERT( pMemBlock );
		//if( !pMemBlock )
		//{
		//	return false;
		//}

		//m_listOfMemBlockPtr.push_back( pMemBlock );

		//BlockNode* pNode	= reinterpret_cast<BlockNode*>( (reinterpret_cast<char*>( pMemBlock )) + ((m_nNumOfBlock - 1) * m_nObjectBlockSize) );	// set pNode to last
		//for( int i = m_nNumOfBlock - 1; i >= 0; --i )
		//{
		//	pNode->pNext	= m_pFreeList;
		//	m_pFreeList		= pNode;
		//	pNode			= reinterpret_cast<BlockNode*>( (reinterpret_cast<char*>( pNode )) - m_nObjectBlockSize );	// set pNode to previous
		//}

		//return true;
	}

	void	Destroy()
	{
		//std::list<void*>::iterator pos = m_listOfMemBlockPtr.begin();
		//std::list<void*>::iterator end = m_listOfMemBlockPtr.end();

		//for( ; pos != end; ++pos )
		//{
		//	free( (*pos) );
		//}

		//m_listOfMemBlockPtr.clear();
	}
};

#endif