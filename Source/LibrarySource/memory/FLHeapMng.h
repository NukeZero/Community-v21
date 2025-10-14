#ifndef __FLHEAPMNG_H__
#define __FLHEAPMNG_H__


#include "FLMemoryCommon.h"


class	FLHeapMng
{
public:

	FLHeapMng();
	~FLHeapMng();

	void*	Malloc( size_t nSize );
	void*	Realloc( void* pMem, size_t nSize );

	void	Free( void* pMem );

private:

	HANDLE	m_hHeap;

};


#endif