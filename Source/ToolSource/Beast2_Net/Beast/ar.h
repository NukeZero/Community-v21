#ifndef __AR_H__
#define __AR_H__

#pragma once

#include "HeapMng.h"

#ifdef __VERIFY_ARCHIVE1129
#include "chassert.h"
#define	__va( x )	\
		if( !( x ) )	\
			chASSERTFAIL( __FILE__, __LINE__, #x, "" );
#else	// __VERIFY_ARCHIVE1129
#define	__va( x )	/##/
#endif	// __VERIFY_ARCHIVE1129

class CAr
{
public:
// Flag values
	enum	{ store = 0, load = 1 };
	enum	{ nGrowSize = 16384 };

	static CHeapMng*	m_pHeapMng;

protected:
	BYTE	m_nMode;	// read or write
	u_int	m_nBufSize;
	LPBYTE	m_lpBufCur;
	LPBYTE	m_lpBufMax;
	LPBYTE	m_lpBufStart;
	BYTE	m_lpBuf[nGrowSize];

public:
	CAr( void* lpBuf = NULL, u_int nBufSize = 0 );
	~CAr();

// Attributes
	BOOL	IsLoading()	const;
	BOOL	IsStoring()	const;

// Operations
	void	Read( void* lpBuf, u_int nSize );
	void	Write( const void* lpBuf, u_int nSize );
	void	CheckBuf( u_int nSize );
	void	Flush( void );
	void	ReelIn( u_int uOffset );

	// reading and writing strings
	void WriteString(LPCTSTR lpsz);
	LPTSTR ReadString(LPTSTR lpsz );

public:
	LPBYTE	GetBuffer( int* pnBufSize );
	u_long	GetOffset( void );
/*
	void	Copy( CAr & ar );
	CAr& operator = ( CAr & ar );
*/
	// insertion operations
	CAr& operator<<(BYTE by);
	CAr& operator<<(WORD w);
	CAr& operator<<(LONG l);
	CAr& operator<<(DWORD dw);
	CAr& operator<<(float f);
	CAr& operator<<(double d);

	CAr& operator<<(int i);
	CAr& operator<<(short w);
	CAr& operator<<(char ch);
	CAr& operator<<(unsigned u);

	// extraction operations
	CAr& operator>>(BYTE& by);
	CAr& operator>>(WORD& w);
	CAr& operator>>(DWORD& dw);
	CAr& operator>>(LONG& l);
	CAr& operator>>(float& f);
	CAr& operator>>(double& d);

	CAr& operator>>(int& i);
	CAr& operator>>(short& w);
	CAr& operator>>(char& ch);
	CAr& operator>>(unsigned& u);

};
/*
inline void CAr::Copy( CAr & ar )
	{	ASSERT( IsLoading() );	ASSERT( ar.IsStoring() );	ar.Write( (void*)m_lpBufStart, (u_int)( m_lpBufMax - m_lpBufStart ) );	}
inline CAr& CAr::operator = ( CAr & ar )
	{	ar.Copy( *this );	return *this;	}
*/
inline BOOL CAr::IsLoading() const
	{ return (m_nMode & CAr::load) != 0; }
inline BOOL CAr::IsStoring() const
	{ return (m_nMode & CAr::load) == 0; }

inline CAr& CAr::operator<<(int i)
	{ return CAr::operator<<((LONG)i); }
inline CAr& CAr::operator<<(unsigned u)
	{ return CAr::operator<<((LONG)u); }
inline CAr& CAr::operator<<(short w)
	{ return CAr::operator<<((WORD)w); }
inline CAr& CAr::operator<<(char ch)
	{ return CAr::operator<<((BYTE)ch); }
inline CAr& CAr::operator<<(BYTE by)
	{ CheckBuf( sizeof(BYTE) );
		*(UNALIGNED BYTE*)m_lpBufCur = by; m_lpBufCur += sizeof(BYTE); return *this; }
inline CAr& CAr::operator<<(WORD w)
	{ CheckBuf( sizeof( WORD ) );
		*(UNALIGNED WORD*)m_lpBufCur = w; m_lpBufCur += sizeof(WORD); return *this; }
inline CAr& CAr::operator<<(LONG l)
	{ CheckBuf( sizeof(LONG) );
		*(UNALIGNED LONG*)m_lpBufCur = l; m_lpBufCur += sizeof(LONG); return *this; }
inline CAr& CAr::operator<<(DWORD dw)
	{ CheckBuf( sizeof(DWORD) );
		*(UNALIGNED DWORD*)m_lpBufCur = dw; m_lpBufCur += sizeof(DWORD); return *this; }
inline CAr& CAr::operator<<(float f)
	{ CheckBuf( sizeof(float) );
		*(UNALIGNED FLOAT*)m_lpBufCur = *(FLOAT*)&f; m_lpBufCur += sizeof(float); return *this; }
inline CAr& CAr::operator<<(double d)
	{ CheckBuf( sizeof(double) );
		*(UNALIGNED double*)m_lpBufCur = *(double*)&d; m_lpBufCur += sizeof(double); return *this; }

inline CAr& CAr::operator>>(int& i)
	{ return CAr::operator>>((LONG&)i); }
inline CAr& CAr::operator>>(unsigned& u)
	{ return CAr::operator>>((LONG&)u); }
inline CAr& CAr::operator>>(short& w)
	{ return CAr::operator>>((WORD&)w); }
inline CAr& CAr::operator>>(char& ch)
	{ return CAr::operator>>((BYTE&)ch); }
inline CAr& CAr::operator>>(BYTE& by)
	{ //ASSERT( m_lpBufCur + sizeof(BYTE) <= m_lpBufMax );
		__va( m_lpBufCur + sizeof(BYTE) <= m_lpBufMax );
		by = *(UNALIGNED BYTE*)m_lpBufCur; m_lpBufCur += sizeof(BYTE); return *this; }
inline CAr& CAr::operator>>(WORD& w)
	{ //ASSERT( m_lpBufCur + sizeof(WORD) <= m_lpBufMax );
		__va( m_lpBufCur + sizeof(WORD) <= m_lpBufMax );
		w = *(UNALIGNED WORD*)m_lpBufCur; m_lpBufCur += sizeof(WORD); return *this; }
inline CAr& CAr::operator>>(DWORD& dw)
	{ //ASSERT( m_lpBufCur + sizeof(DWORD) <= m_lpBufMax );
		__va( m_lpBufCur + sizeof(DWORD) <= m_lpBufMax );
		dw = *(UNALIGNED DWORD*)m_lpBufCur; m_lpBufCur += sizeof(DWORD); return *this; }
inline CAr& CAr::operator>>(float& f)
	{ //ASSERT( m_lpBufCur + sizeof(float) <= m_lpBufMax );
		__va( m_lpBufCur + sizeof(float) <= m_lpBufMax );
		*(FLOAT*)&f = *(UNALIGNED FLOAT*)m_lpBufCur; m_lpBufCur += sizeof(float); return *this; }
inline CAr& CAr::operator>>(double& d)
	{ //ASSERT( m_lpBufCur + sizeof(double) <= m_lpBufMax );
		__va( m_lpBufCur + sizeof(double) <= m_lpBufMax );
		*(double*)&d = *(UNALIGNED double*)m_lpBufCur; m_lpBufCur += sizeof(double); return *this; }
inline CAr& CAr::operator>>(LONG& l)
	{ //ASSERT( m_lpBufCur + sizeof(LONG) <= m_lpBufMax );
		__va( m_lpBufCur + sizeof(LONG) <= m_lpBufMax );
		l = *(UNALIGNED LONG*)m_lpBufCur; m_lpBufCur += sizeof(LONG); return *this; }


#include <D3DX9Math.h>

inline CAr& operator<<(CAr & ar, D3DXVECTOR3 v)
	{	ar.Write( &v, sizeof(D3DXVECTOR3) );	return ar;	}

inline CAr& operator>>(CAr & ar, D3DXVECTOR3& v)
	{	ar.Read( &v, sizeof(D3DXVECTOR3) );		return ar;	}

inline CAr& operator<<(CAr & ar, __int64 i)
	{	ar.Write( &i, sizeof(__int64) );	return ar;	}

inline CAr& operator>>(CAr & ar, __int64& i)
	{	ar.Read( &i, sizeof(__int64) );	return ar;	}

inline CAr& operator<<(CAr & ar, CRect rect)
	{	ar.Write( &rect, sizeof(CRect) );	return ar;	}

inline CAr& operator>>(CAr & ar, CRect & rect)
	{	ar.Read( &rect, sizeof(CRect) );	return ar;	}

inline CAr& operator<<(CAr & ar, PLAY_ACCOUNT pa)
	{	ar.Write( &pa, sizeof(PLAY_ACCOUNT) );	return ar;	}

inline CAr& operator>>(CAr & ar, PLAY_ACCOUNT& pa)
	{	ar.Read( &pa, sizeof(PLAY_ACCOUNT) );	return ar;	}
			
inline u_long CAr::GetOffset( void )
{
//	ASSERT( IsStoring() );
	__va( IsStoring() );
	return( m_lpBufCur - m_lpBufStart );
}

#endif //__AR_H__
