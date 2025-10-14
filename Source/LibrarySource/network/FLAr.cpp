// String format:
//      UNICODE strings are always prefixed by 0xff, 0xfffe
//      if < 0xff chars: len:BYTE, TCHAR chars
//      if >= 0xff characters: 0xff, len:WORD, TCHAR chars
//      if >= 0xfffe characters: 0xff, 0xffff, len:DWORD, TCHARs


#include "FLAr.h"
#include "../dump/FLDumpCommon.h"


FLLowBufferPool< CAr::nGrowSize, 128 >	CAr::m_cLowPool;


CAr::CAr( void* lpBuf, u_long nBufSize )
{
	if( lpBuf )
	{
		m_nMode			= load;
		m_lpBufStart	= reinterpret_cast<LPBYTE>(lpBuf);
		m_nBufSize		= nBufSize;
	}
	else
	{
		m_nMode			= store;
		m_lpBufStart	= m_cLowPool.Alloc( nGrowSize );
		m_nBufSize		= nGrowSize;
	}

	m_lpBufMax	= m_lpBufStart + m_nBufSize;
	m_lpBufCur	= m_lpBufStart;
}

CAr::CAr( u_long nBufSize )
{
	if( nBufSize < nGrowSize )
	{
		nBufSize = nGrowSize;
	}

	m_nMode = store;
	m_lpBufStart = m_cLowPool.Alloc( nBufSize );

	FLASSERT( m_lpBufStart );
	m_nBufSize	= nBufSize;
	m_lpBufCur	= m_lpBufStart;
	m_lpBufMax	= m_lpBufStart + m_nBufSize;
}

CAr::~CAr()
{
	if( IsStoring() )
	{
		m_cLowPool.Free( m_lpBufStart, m_nBufSize );
	}
}

BOOL CAr::IsLoading() const
{
	return (m_nMode == CAr::load);
}

BOOL CAr::IsStoring() const
{
	return (m_nMode == CAr::store);
}

void CAr::Read( void* lpBuf, u_long nSize )
{
	FLASSERT( IsLoading() );
	FLASSERT( lpBuf );

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100708
	if( lpBuf == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- lpBuf Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- lpBuf Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}
	if( nSize <= 0 )
	{
		return;
	}

	if( IsLoading() == false )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- IsLoading() Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- IsLoading() Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}
	//	END100708
	//////////////////////////////////////////////////////////////////////////

	if( m_lpBufCur + nSize <= m_lpBufMax )
	{
		memcpy( lpBuf, m_lpBufCur, nSize );
		m_lpBufCur	+= nSize;
	}
	else	// overflow
	{
		memset( lpBuf, 0, nSize );
		m_lpBufCur	= m_lpBufMax;
	}
}

void CAr::Write( const CAr& ar )
{
	u_long nSize = 0;
	LPBYTE lpBuf = ar.GetBuffer( &nSize );
	Write( lpBuf, nSize );
}

void CAr::Write( const void* lpBuf, u_long nSize )
{
	FLASSERT( IsStoring() );
	FLASSERT( lpBuf );

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100708
	if( lpBuf == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- lpBuf Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- lpBuf Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}
	if( nSize <= 0 )
	{
		return;
	}

	if( IsStoring() == false )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- IsStoring() Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- IsStoring() Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}	
	//	END100708
	//////////////////////////////////////////////////////////////////////////

	CheckBuf( nSize );

	memcpy( m_lpBufCur, lpBuf, nSize );
	m_lpBufCur	+= nSize;
}

void CAr::WriteString( LPCTSTR lpsz )
{
	if( lpsz == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- WriteString Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- WriteString Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}

	size_t nLen	= _tcslen( lpsz );
	if( nLen >= INT_MAX )
	{
		nLen = 0;
	}

	u_long nSafeLen = static_cast< u_long >( nLen );
	*this << nSafeLen;
	Write( lpsz, sizeof( TCHAR ) * nSafeLen );
}

void CAr::ReadString( LPTSTR lpsz, u_long nBufSize )
{
	// if nMax is negative (such a large number doesn't make sense given today's
	// 2gb address space), then assume it to mean "keep the newline".
	if( lpsz == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- ReadString Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- ReadString Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}

	u_long nLen = 0;
	*this >> nLen;

	if( nLen+1 <= nBufSize )
	{
		Read( lpsz, sizeof( TCHAR ) * nLen );
		lpsz[nLen] = _T( '\0' );
	}
	else
	{
		memset( lpsz, 0, nBufSize );
		m_lpBufCur	= m_lpBufMax;
	}
}

ptrdiff_t CAr::ReserveSpace( u_long nSize )
{
	ptrdiff_t nOffset = -1;

	FLASSERT( nSize > 0 );
	if( nSize > 0 )
	{
		CheckBuf( nSize );

		nOffset = m_lpBufCur - m_lpBufStart;

		::memset( m_lpBufCur, 0, nSize );
		m_lpBufCur += nSize;
	}

	return nOffset;
}

void CAr::WriteReservedSpace( ptrdiff_t nOffset, const void* lpBuf, u_long nSize )
{
	FLASSERT( nOffset >= 0 );
	FLASSERT( lpBuf != NULL );
	FLASSERT( nSize > 0 );

	if( nOffset < 0 || lpBuf == NULL || nSize <= 0 )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- offset param Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- offset param Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}

	FLASSERT( IsStoring() );
	if( IsStoring() == false )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- offset IsStoring() Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- offset IsStoring() Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}

	if( nOffset >= (m_lpBufCur - m_lpBufStart) )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "-- offset out of range Error" ) );
		FLPrintCallStackHelper( _T( "CAr" ), _T( "-- offset out of range Error" ), _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ );
		return;
	}

	::memcpy( m_lpBufStart+nOffset, lpBuf, nSize );
}

void CAr::CheckBuf( u_long nSize )
{
	if( IsOverflow( nSize ) == true )
	{
		Grow();
	}
}

bool CAr::IsOverflow( u_long nSize )	const
{
	return (m_lpBufCur + nSize > m_lpBufMax);
}

void CAr::Grow()
{
	ptrdiff_t uOffset	= m_lpBufCur - m_lpBufStart;
	if( uOffset >= 0 )
	{
		u_long nExtension	= m_nBufSize * 2;

		LPBYTE pTemp = m_cLowPool.Alloc( nExtension );
		memcpy( pTemp, m_lpBufStart, uOffset );
		m_cLowPool.Free( m_lpBufStart, m_nBufSize );

		m_lpBufStart	= pTemp;
		m_nBufSize		= nExtension;
		m_lpBufCur		= m_lpBufStart + uOffset;
		m_lpBufMax		= m_lpBufStart + m_nBufSize;
	}
}

LPBYTE CAr::GetBuffer( u_long* pnBufSize )	const
{
	FLASSERT( IsStoring() );
	FLASSERT( pnBufSize );

	ptrdiff_t nBufSize = m_lpBufCur - m_lpBufStart;
	if( nBufSize >= 0 )
	{
		*pnBufSize	= static_cast< u_long >( nBufSize );
	}

	return m_lpBufStart;
}

void CAr::Flush()
{
	FLASSERT( IsStoring() );
	if( IsStoring() )
	{
		m_lpBufCur	= m_lpBufStart;
	}
}

void CAr::ReelIn( u_long uOffset )
{
	FLASSERT( IsStoring() );
	FLASSERT( m_lpBufStart + uOffset <= m_lpBufCur );
	FLASSERT( uOffset <= nGrowSize );

	if( m_nBufSize > nGrowSize )
	{
		LPBYTE pTemp = m_cLowPool.Alloc( nGrowSize );
		::memcpy( pTemp, m_lpBufStart, uOffset );
		m_cLowPool.Free( m_lpBufStart, m_nBufSize );

		m_lpBufStart	= pTemp;
		m_nBufSize		= nGrowSize;
		m_lpBufMax		= m_lpBufStart + m_nBufSize;
	}

	m_lpBufCur	= m_lpBufStart + uOffset;
}

