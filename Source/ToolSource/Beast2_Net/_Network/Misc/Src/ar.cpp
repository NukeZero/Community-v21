// String format:
//      UNICODE strings are always prefixed by 0xff, 0xfffe
//      if < 0xff chars: len:BYTE, TCHAR chars
//      if >= 0xff characters: 0xff, len:WORD, TCHAR chars
//      if >= 0xfffe characters: 0xff, 0xffff, len:DWORD, TCHARs

#include "StdAfx.h"
#include "Ar.h"

#ifdef _DEBUG
#define heapAlloc( nSize )	m_pHeapMng->Malloc( nSize, __FILE__, __LINE__ );
#define heapRealloc( lpMem, nSize )	m_pHeapMng->Realloc( lpMem, nSize, __FILE__, __LINE__ );
#else
#define heapAlloc( nSize )	m_pHeapMng->Malloc( nSize );
#define heapRealloc( lpMem, nSize )	m_pHeapMng->Realloc( lpMem, nSize );
#endif

CHeapMng*	CAr::m_pHeapMng		= new CHeapMng;

CAr::CAr( void* lpBuf, u_int nBufSize )
{
	if( lpBuf )
	{
		m_nMode		= load;
		m_lpBufStart	= (LPBYTE)lpBuf;
		m_nBufSize	= nBufSize;
	}
	else
	{
		m_nMode		= store;
		m_lpBufStart	= m_lpBuf;
		m_nBufSize	= nGrowSize;
	}
	m_lpBufMax	= m_lpBufStart + m_nBufSize;
	m_lpBufCur	= m_lpBufStart;
}

CAr::~CAr()
{
	if( IsStoring() && ( m_nBufSize > nGrowSize ) )
	{
		CAr::m_pHeapMng->Free( m_lpBufStart );
	}
}

void CAr::Read( void* lpBuf, u_int nSize )
{
	if( nSize == 0 )	return;
//	ASSERT( IsLoading() );
//	ASSERT( lpBuf );
//	ASSERT( m_lpBufCur + nSize <= m_lpBufMax );
	__va( IsLoading() );
	__va( lpBuf );
	__va( m_lpBufCur + nSize <= m_lpBufMax );

	memcpy( lpBuf, m_lpBufCur, nSize );
	m_lpBufCur	+= nSize;
}

void CAr::Write( const void* lpBuf, u_int nSize )
{
	if( nSize == 0 )	return;
//	ASSERT( IsStoring() );
//	ASSERT( lpBuf );
	__va( IsStoring() );
	__va( lpBuf );
	
	CheckBuf( nSize );
	memcpy( m_lpBufCur, lpBuf, nSize );
	m_lpBufCur	+= nSize;
}

// special functions for text file input and output
void CAr::WriteString( LPCTSTR lpsz )
{
	int nLen	= _tcslen( lpsz );
	*this << nLen;
	Write( lpsz, sizeof(TCHAR) * nLen );
}

LPTSTR CAr::ReadString( LPTSTR lpsz )
{
	// if nMax is negative (such a large number doesn't make sense given today's
	// 2gb address space), then assume it to mean "keep the newline".
	int nLen;
	*this >> nLen;
	Read( lpsz, sizeof(TCHAR) * nLen );
	lpsz[nLen] = '\0';
	return lpsz;
}

void CAr::CheckBuf( u_int nSize )
{
	if( m_lpBufCur + nSize > m_lpBufMax )
	{
		u_int uOffset	= m_lpBufCur - m_lpBufStart;

		u_int nExtension	= nGrowSize * ( nSize / nGrowSize + 1 );
		
		if( m_nBufSize > nGrowSize )
		{
			m_lpBufStart	= (LPBYTE) heapRealloc( m_lpBufStart, m_nBufSize + nExtension );
		}
		else
		{
			LPBYTE lpBuf	= (LPBYTE) heapAlloc( m_nBufSize + nExtension );
			memcpy( lpBuf, m_lpBufStart, m_nBufSize );
			m_lpBufStart	= lpBuf;
		}

//		ASSERT( m_lpBufStart );
		__va( m_lpBufStart );
//
//		if( !m_lpBufStart )
//			Error( "HEAP_GENERATE_EXCEPTIONS // %s, %d", __FILE__, __LINE__ );
//
		m_nBufSize	+= nExtension;
		m_lpBufCur	= m_lpBufStart + uOffset;
		m_lpBufMax	= m_lpBufStart + m_nBufSize;
	}
}

LPBYTE CAr::GetBuffer( int* pnBufSize )
{
//	ASSERT( IsStoring() );
//	ASSERT( pnBufSize );
	__va( IsStoring() );
	__va( pnBufSize );
	*pnBufSize	= m_lpBufCur - m_lpBufStart;
	return m_lpBufStart;
}

void CAr::Flush( void )
{
//	ASSERT( IsStoring() );
	__va( IsStoring() );
	m_lpBufCur	= m_lpBufStart;
}

void CAr::ReelIn( u_int uOffset )
{
//	ASSERT( IsStoring() );
//	ASSERT( m_lpBufStart + uOffset <= m_lpBufCur );
	__va( IsStoring() );
	__va( m_lpBufStart + uOffset <= m_lpBufCur );
	m_lpBufCur	= m_lpBufStart + uOffset;
}