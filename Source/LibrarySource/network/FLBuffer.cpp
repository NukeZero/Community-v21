
#include "FLBuffer.h"


FLLowBufferPool< MAX_BUFFER, 512 >	CBuffer::m_cLowPool;
MEMPOOLER_IMPLEMENT( CBuffer, 512 );


CBuffer::CBuffer( u_long uBufSize )
{
	if( uBufSize < MAX_BUFFER )
	{
		uBufSize = MAX_BUFFER;
	}

	m_lpBufStart = m_cLowPool.Alloc( uBufSize );
	if( m_lpBufStart == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "버퍼를 할당하지 못했습니다. BufSize: %d" ), uBufSize );
		uBufSize = 0;
	}

	m_pHead		= m_pTail	= m_lpBufStart;
	m_lpBufMax	= m_lpBufStart + uBufSize;

	m_nBlockCount = 0;
	m_dpid = DPID_UNKNOWN;
	pNext = NULL;
}

CBuffer::~CBuffer()
{
	if( m_lpBufStart )
	{
		m_cLowPool.Free( m_lpBufStart, GetSize() );
	}
}

u_long	CBuffer::GetSize()	const
{
	FLASSERT( m_lpBufMax >= m_lpBufStart );
	if( m_lpBufStart > m_lpBufMax )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 m_lpBufStart: %p, m_lpBufMax: %p" ), m_lpBufStart, m_lpBufMax );
		return 0;
	}

	return static_cast< u_long >( m_lpBufMax - m_lpBufStart );
}

LPBYTE	CBuffer::GetWritableBuffer( u_long* pnBufSize )	const
{
	*pnBufSize	= GetWritableBufferSize();
	return m_pTail;
}

u_long	CBuffer::GetWritableBufferSize()	const
{
	FLASSERT( m_lpBufMax >= m_pTail );
	if( m_pTail > m_lpBufMax )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 m_pTail: %p, m_lpBufMax: %p" ), m_pTail, m_lpBufMax );
		return 0;
	}

	return static_cast< u_long >( m_lpBufMax - m_pTail );
}

LPBYTE	CBuffer::GetReadableBuffer( u_long* pnBufSize )	const
{
	*pnBufSize	= GetReadableBufferSize();
	return m_pHead;
}

u_long	CBuffer::GetReadableBufferSize()	const
{
	FLASSERT( m_pTail >= m_pHead );
	if( m_pHead > m_pTail )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 m_pHead: %p, m_pTail: %p" ), m_pHead, m_pTail );
		return 0;
	}

	return static_cast< u_long >( m_pTail - m_pHead );
}

u_long	CBuffer::GetBlockCount()	const
{
	return m_nBlockCount;
}

void	CBuffer::IncBlockCount()
{
	++m_nBlockCount;
}

void	CBuffer::SetDPID( DPID dpid )
{
	m_dpid = dpid;
}

DPID	CBuffer::GetDPID()	const
{
	return m_dpid;
}

bool	CBuffer::Write( const void* pData, u_long nSize )
{
	FLASSERT( GetWritableBufferSize() >= nSize );
	if( GetWritableBufferSize() >= nSize )
	{
		::memcpy( m_pTail, pData, nSize );
		m_pTail += nSize;
		return true;
	}

	FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 복사 실패 WriteableSize: %d, Size: %d" ), GetWritableBufferSize(), nSize );
	return false;
}

void	CBuffer::CutTail( u_long nSize )
{
	FLASSERT( GetReadableBufferSize() >= nSize );
	if( GetReadableBufferSize() >= nSize )
	{
		m_pTail -= nSize;
	}
	else
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 ReadableSize: %d, Size: %d" ), GetReadableBufferSize(), nSize );
	}
}

void	CBuffer::UseTail( u_long nSize )
{
	FLASSERT( GetWritableBufferSize() >= nSize );
	if( GetWritableBufferSize() >= nSize )
	{
		m_pTail += nSize;
	}
	else
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 GetWritableBufferSize: %d, Size: %d" ), GetWritableBufferSize(), nSize );
	}
}

void	CBuffer::CutHead( u_long nSize )
{
	FLASSERT( GetReadableBufferSize() >= nSize );
	if( GetReadableBufferSize() >= nSize )
	{
		m_pHead += nSize;
	}
	else
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 GetReadableBufferSize: %d, Size: %d" ), GetReadableBufferSize(), nSize );
	}
}

void	CBuffer::DropHead( u_long nSize )
{
	u_long nReadableSize = GetReadableBufferSize();
	FLASSERT( nReadableSize >= nSize );
	if( nReadableSize >= nSize )
	{
		u_long nMoveSize = nReadableSize - nSize;
		::memmove( m_pHead, &m_pHead[nSize], nMoveSize );
		m_pTail = m_pHead + nMoveSize;
	}
	else
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "유효하지 않은 버퍼 nReadableSize: %d, Size: %d" ), nReadableSize, nSize );
	}
}

/*--------------------------------------------------------------------------------*/
CBufferQueue::CBufferQueue()
{
	m_uCount	= 0;
	m_pHead		= m_pTail	= NULL;
}

CBufferQueue::~CBufferQueue()
{
	Clear( TRUE );
}

CBuffer*	CBufferQueue::GetHead()	const
{
	return m_pHead;
}

CBuffer*	CBufferQueue::GetTail()	const
{
	return m_pTail;
}

BOOL	CBufferQueue::IsEmpty()	const
{
	return( m_uCount == 0 );
}

u_long	CBufferQueue::GetCount()	const
{
	return m_uCount;
}

void	CBufferQueue::AddTail( CBuffer* pBuffer )
{
	if( m_pHead == NULL )
	{
		m_pHead		= pBuffer;
	}
	if( m_pTail )
	{
		m_pTail->pNext	= pBuffer;
	}

	m_pTail = pBuffer;
	++m_uCount;
}

void	CBufferQueue::AddTail( CBufferQueue* pQueue )
{
	if( pQueue->IsEmpty() == FALSE )
	{
		if( m_pHead == NULL )
		{
			m_pHead = pQueue->GetHead();
		}
		else
		{
			FLASSERT( m_pTail != NULL );
			m_pTail->pNext = pQueue->GetHead();
		}

		m_pTail = pQueue->GetTail();
		m_uCount += pQueue->GetCount();
		pQueue->Clear( FALSE );
	}
}

bool	CBufferQueue::Write( const void* pData, u_long nSize )
{
	const char* pSrc = reinterpret_cast< const char* >( pData );
	while( nSize > 0 )
	{
		CBuffer* pBuffer = GetTail();
		if( pBuffer == NULL || pBuffer->GetWritableBufferSize() <= 0 )
		{
			pBuffer	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
			if( pBuffer == NULL )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 DefaultSize" ) );
				return false;
			}

			AddTail( pBuffer );
		}

		u_long nSpace	= pBuffer->GetWritableBufferSize();
		u_long nCopySize = 0;

		if( nSpace >= nSize )
		{
			nCopySize = nSize;
		}
		else
		{
			nCopySize = nSpace;
		}

		pBuffer->Write( pSrc, nCopySize );

		nSize	-= nCopySize;
		pSrc	+= nCopySize;
	}

	return true;
}

CBuffer*	CBufferQueue::PopHead()
{
	CBuffer* pBuffer = GetHead();
	if( pBuffer != NULL )
	{
		RemoveHead();
	}

	return pBuffer;
}

void	CBufferQueue::RemoveData( u_long uDataSize )
{
	while( uDataSize > 0 )
	{
		CBuffer* pBuffer	= GetHead();
		if( NULL == pBuffer )
			return;

		u_long uRemnant	= pBuffer->GetReadableBufferSize();
		if( uRemnant > uDataSize )
		{
			pBuffer->CutHead( uDataSize );
			uDataSize	= 0;
		}
		else
		{
			RemoveHead();
			SAFE_DELETE( pBuffer );
			uDataSize	-= uRemnant;
		}
	}
}

void	CBufferQueue::RemoveHead()
{
	if( !IsEmpty() )
	{
		CBuffer* pTemp = m_pHead;
		if( m_pTail == pTemp )
		{
			m_pTail	= NULL;
		}

		m_pHead = pTemp->pNext;
		pTemp->pNext = NULL;
		--m_uCount;
	}
}

void	CBufferQueue::Clear( BOOL bDelete )
{
	if( bDelete )
	{
		CBuffer* pBuffer = GetHead();
		CBuffer* pTemp = NULL;
		while( pBuffer != NULL )
		{
			pTemp	= pBuffer->pNext;
			SAFE_DELETE( pBuffer );
			pBuffer	= pTemp;
		}
	}

	m_uCount	= 0;
	m_pHead	= m_pTail	= NULL;
}

void	CBufferQueue::GetData( LPWSABUF lpBuffers, LPDWORD lpdwBufferCount )	const
{
	*lpdwBufferCount	= 0;

	int	i	= 0;
	CBuffer* pBuffer	= GetHead();
	while( (pBuffer != NULL) && (i < MAX_WSABUF) )
	{
		lpBuffers[i].buf	= reinterpret_cast< char* >( pBuffer->GetReadableBuffer( &lpBuffers[i].len ) );
		++i;
		pBuffer	= pBuffer->pNext;
	}

	*lpdwBufferCount	= i;
}

void	CBufferQueue::SetDPID( DPID dpid )
{
	CBuffer* pBuffer = GetHead();
	while( pBuffer != NULL )
	{
		pBuffer->SetDPID( dpid );
		pBuffer = pBuffer->pNext;
	}
}

/*--------------------------------------------------------------------------------*/
CBufferFactory::CBufferFactory()
{
}

CBufferFactory& CBufferFactory::GetInstance()
{
	static CBufferFactory g_factory;
	return g_factory;
}

CBuffer* CBufferFactory::CreateBuffer( u_long uBufSize )
{
	CBuffer* pBuffer = new CBuffer( uBufSize );
	if( pBuffer != NULL )
	{
		if( pBuffer->GetSize() >= uBufSize )
		{
			return pBuffer;
		}

		SAFE_DELETE( pBuffer );
	}

	FLERROR_LOG( GetNetworkLogName(), _T( "버퍼를 할당하지 못했습니다. BufSize: %d" ), uBufSize );
	return NULL;
}


