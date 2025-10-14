#pragma	once
#include <assert.h>
#include <list>

template <class T>	class MemPooler
{
	struct	BlockNode
	{
		BlockNode* pNext;
		BlockNode()
			{	pNext	= NULL;	}
	};

protected:
	BlockNode*	m_pFreeList;
	list<void*>		m_ListOfMemBlockPtr;
	int		m_nNumOfBlock;
	int		m_nListBlockSize;	// size per block
	int		m_nAllocCount;
	CRITICAL_SECTION	m_cs;

public:
//	Constructions
	MemPooler( int nNumOfBlock ): m_nNumOfBlock( nNumOfBlock ),
													m_pFreeList( NULL ),
													m_nAllocCount( 0 )
		{
#ifdef __VERIFY_MEMPOOL
			return;
#endif

			assert( nNumOfBlock > 0 );
			m_nListBlockSize	= sizeof(BlockNode) + sizeof(T);
			Create();
			
#ifndef __BEAST
			InitializeCriticalSectionAndSpinCount( &m_cs, 2000 );
#else	// __BEAST
			InitializeCriticalSection( &m_cs );
#endif	// __BEAST
		}

	~MemPooler()
		{	Destroy();	}
//	Operations
	T*	Alloc( void )
		{
#ifdef __VERIFY_MEMPOOL
			return (T*) ::operator new( sizeof(T) );
#endif

			BlockNode* pNode	= NULL;
			T* lpMem	= NULL;

			EnterCriticalSection( &m_cs );

			if( !m_pFreeList )
				Create();

			pNode	= m_pFreeList;
			m_pFreeList		= m_pFreeList->pNext;
			m_nAllocCount++;
			lpMem	= reinterpret_cast<T*>( pNode + 1 );

			LeaveCriticalSection( &m_cs );
			return lpMem;
		}

	BOOL	Free( T* lpMem )
		{
#ifdef __VERIFY_MEMPOOL
			::operator delete(lpMem);
			return TRUE;
#endif
			BlockNode* pNode	= NULL;

			EnterCriticalSection( &m_cs );

			pNode	= ( reinterpret_cast<BlockNode*>( lpMem ) ) - 1;
			if( m_nAllocCount > 0 )
			{
				pNode->pNext	= m_pFreeList;
				m_pFreeList		= pNode;
				m_nAllocCount--;
			}

			LeaveCriticalSection( &m_cs );

			return TRUE;
		}

	int		GetCount( void )
		{	return m_nAllocCount;	}

protected:
	void	Create( void )
		{
#ifdef __VERIFY_MEMPOOL
			return;
#endif
			const int AllocationSize	= m_nListBlockSize * m_nNumOfBlock;
			void* pMemBlock		= VirtualAlloc( NULL, AllocationSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
			if( !pMemBlock )
				WriteLog( "%s, %d", __FILE__, __LINE__ );
			assert( pMemBlock );
			m_ListOfMemBlockPtr.push_back( pMemBlock );

			BlockNode* pNode	= reinterpret_cast<BlockNode*>(pMemBlock);
			pNode	= reinterpret_cast<BlockNode*>( ( reinterpret_cast<DWORD>( pNode ) ) + ( m_nNumOfBlock - 1 ) * ( m_nListBlockSize ) );
			for( int i = m_nNumOfBlock - 1; i >= 0; i-- )
			{
				pNode->pNext	= m_pFreeList;
				m_pFreeList	= pNode;
				pNode	= reinterpret_cast<BlockNode*>( ( reinterpret_cast<DWORD>( pNode ) ) - m_nListBlockSize );
			}
		}

	void	Destroy( void )
	{
#ifdef __VERIFY_MEMPOOL
		return;
#endif
		for( list<void*>::iterator i = m_ListOfMemBlockPtr.begin(); i != m_ListOfMemBlockPtr.end(); ++i )
			VirtualFree( *i, 0, MEM_RELEASE );
		m_ListOfMemBlockPtr.clear();

		DeleteCriticalSection( &m_cs );
	}
};