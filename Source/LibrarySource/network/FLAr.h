#ifndef __FLAR_H__
#define __FLAR_H__

#pragma once

#include "FLNetworkCommon.h"
#include "../memory/FLLowBufferPool.h"


class CAr
{
public:

// Flag values
	enum	{ store = 0, load = 1 };
	enum	{ nGrowSize = (16*1024) };

private:

	BYTE	m_nMode;	// read or write

	u_long	m_nBufSize;
	LPBYTE	m_lpBufCur;
	LPBYTE	m_lpBufMax;
	LPBYTE	m_lpBufStart;

public:

	CAr( u_long nBufSize = nGrowSize );
	CAr( void* lpBuf, u_long nBufSize );
	~CAr();

// Attributes
	BOOL	IsLoading()	const;
	BOOL	IsStoring()	const;

// Operations
	void	Read( void* lpBuf, u_long nSize );
	void	Write( const CAr& ar );
	void	Write( const void* lpBuf, u_long nSize );

	void	WriteString( LPCTSTR lpsz );
	void	ReadString( LPTSTR lpsz, u_long nBufSize );

	ptrdiff_t	ReserveSpace( u_long nSize );
	void		WriteReservedSpace( ptrdiff_t nOffset, const void* lpBuf, u_long nSize );

	void	Flush();
	void	ReelIn( u_long uOffset );

	LPBYTE	GetBuffer( u_long* pnBufSize )	const;

	template< typename T >
	CAr& operator<<(const T& value)
	{
		Write( &value, sizeof( value ) );
		return *this;
	}
	template< typename T >
	CAr& operator>>(T& value)
	{
		Read( &value, sizeof( value ) );
		return *this;
	}

private:

	static FLLowBufferPool< nGrowSize, 128 >	m_cLowPool;

	// 포인터는 Write*(), Read*() 류 함수 이용.
	template< typename T >
	CAr& operator<<(T*);
	template< typename T >
	CAr& operator>>(T*);

	void	CheckBuf( u_long nSize );
	bool	IsOverflow( u_long nSize )	const;
	void	Grow();

};


#endif //__AR_H__
