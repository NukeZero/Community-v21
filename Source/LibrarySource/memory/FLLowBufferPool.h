
#ifndef	__FLLOWBUFFERPOOL_H__
#define	__FLLOWBUFFERPOOL_H__


#include "FLMemoryCommon.h"
#include "FLMemPooler.h"
#include "FLHeapMng.h"


template< std::size_t nFixedSize, std::size_t nFixedCount >
class	FLLowBufferPool
{
private:

	//struct	T_FIXED_BUFFER
	//{
	//	BYTE	byBuffer[nFixedSize];
	//};

public:

	FLLowBufferPool()
//		: m_cPool( nFixedCount )
	{
	}
	~FLLowBufferPool()
	{
	}

	LPBYTE	Alloc( size_t nSize )
	{
		return new BYTE[ nSize ];
		//LPBYTE pData = NULL;
		//if( nSize > nFixedSize )
		//{
		//	pData = reinterpret_cast< LPBYTE >( m_cHeap.Malloc( nSize ) );
		//}
		//else
		//{
		//	pData = reinterpret_cast< LPBYTE >( m_cPool.Alloc() );
		//}

		//return pData;
	}

	void	Free( LPBYTE pData, size_t /*nSize*/ )
	{
		delete pData;
		//if( pData != NULL )
		//{
		//	if( nSize > nFixedSize )
		//	{
		//		m_cHeap.Free( pData );
		//	}
		//	else
		//	{
		//		m_cPool.Free( reinterpret_cast< T_FIXED_BUFFER* >( pData ) );
		//	}
		//}
	}

private:

//	typedef	FLMemPooler< T_FIXED_BUFFER >	FIXED_POOL;

//	FLHeapMng	m_cHeap;
//	FIXED_POOL	m_cPool;

};





#endif
