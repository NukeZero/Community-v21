
#include "FLHeapMng.h"


FLHeapMng::FLHeapMng()
{
	m_hHeap	= ::HeapCreate( 0, 0, 0 );
}

FLHeapMng::~FLHeapMng()
{
	if( m_hHeap != NULL )
	{
		::HeapDestroy( m_hHeap );
		m_hHeap = NULL;
	}
}

void*	FLHeapMng::Malloc( size_t nSize )
{
	return ::HeapAlloc( m_hHeap, 0, nSize );
}

void*	FLHeapMng::Realloc( void* pMem, size_t nSize  )
{
	return ::HeapReAlloc( m_hHeap, 0, pMem, nSize );
}

void	FLHeapMng::Free( void* pMem )
{
	::HeapFree( m_hHeap, 0, pMem );
}
