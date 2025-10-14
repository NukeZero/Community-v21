#ifndef __HEAPMNG_H__
#define __HEAPMNG_H__

#pragma once

#include "CMclCritSec.h"

#ifdef __HEAPCOMPACT1005
#define	PRINTHEAPERROR( i )		PrintError( __FILE__, __LINE__, i );
#endif	// __HEAPCOMPACT1005

class CHeapMng
{
private:
	HANDLE	m_hHeap;
//	CMclCritSec		m_cs;
	CRITICAL_SECTION	m_cs;

public:
//	Constructions
	CHeapMng ();
	virtual	~CHeapMng();
//	Operations
	void*	Malloc( size_t nSize, LPCSTR lpszFileName = NULL, int nLine = 0 );
	void	Free( LPVOID lpMem );
	void*	Realloc( LPVOID lpMem, size_t nSize, LPCSTR lpszFileName = NULL, int nLine = 0 );
#ifdef __HEAPCOMPACT1005
	void	PrintError( const char* LpszFilename, int nLine, int i );
#endif	// __HEAPCOMPACT1005
};

inline CHeapMng::CHeapMng()
{
	CSystemInfo si;
	m_hHeap		= HeapCreate( 0, si.dwPageSize, 0 );
#ifndef __BEAST
	InitializeCriticalSectionAndSpinCount( &m_cs, 2000 );
#else	// __BEAST
	InitializeCriticalSection( &m_cs );
#endif	// __BEAST
}

inline CHeapMng::~CHeapMng()
{
	HeapDestroy( m_hHeap );
	DeleteCriticalSection( &m_cs );
}

inline void* CHeapMng::Malloc( size_t nSize, LPCSTR lpszFileName, int nLine )
{
//	CMclAutoLock Lock( m_cs );
//	HeapLock( m_hHeap );
	EnterCriticalSection( &m_cs );
	LPVOID lpMem	= HeapAlloc( m_hHeap, 0, nSize );
#ifdef __HEAPCOMPACT1005
	if( !lpMem )
	{
		DWORD dwTickCount	= GetTickCount();
		HeapCompact( m_hHeap, 0 );
		PRINTHEAPERROR( GetTickCount() - dwTickCount );
		lpMem	= HeapAlloc( m_hHeap, 0, nSize );
		if( !lpMem )
			PRINTHEAPERROR( -1 );
	}
#endif	// __HEAPCOMPACT1005
	LeaveCriticalSection( &m_cs );
//	HeapUnlock( m_hHeap );
	return lpMem;
}

inline void CHeapMng::Free( LPVOID lpMem )
{
//	CMclAutoLock Lock( m_cs );
//	HeapLock( m_hHeap );
	EnterCriticalSection( &m_cs );
	HeapFree( m_hHeap, 0, lpMem );
	LeaveCriticalSection( &m_cs );
//	HeapUnlock( m_hHeap );
}

inline void* CHeapMng::Realloc( LPVOID lpMem, size_t nSize, LPCSTR lpszFileName, int nLine )
{
//	CMclAutoLock Lock( m_cs );
//	HeapLock( m_hHeap );
	EnterCriticalSection( &m_cs );
	LPVOID lpNewMem		= HeapReAlloc( m_hHeap, 0, lpMem, nSize );
#ifdef __HEAPCOMPACT1005
	if( !lpNewMem )
	{
		DWORD dwTickCount	= GetTickCount();
		HeapCompact( m_hHeap, 0 );
		PRINTHEAPERROR( GetTickCount() - dwTickCount );
		lpNewMem	= HeapReAlloc( m_hHeap, 0, lpMem, nSize );
		if( !lpNewMem )
			PRINTHEAPERROR( -1 );
	}
#endif	// __HEAPCOMPACT1005
	LeaveCriticalSection( &m_cs );
//	HeapUnlock( m_hHeap );
	return lpNewMem;
}

#ifdef __HEAPCOMPACT1005
inline void CHeapMng::PrintError( const char* lpszFilename, int nLine, int i )
{
	FILEOUT( "heap.txt", "%s, %d	// %d", lpszFilename, nLine, i );
}
#endif	// __HEAPCOMPACT1005

#endif //__HEAPMNG_H__
