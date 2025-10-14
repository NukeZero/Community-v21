#ifndef __AI_HUMAN_H
#define __AI_HUMAN_H

#include "mempooler.h"

class CAIHuman : public CAIInterface
{
	D3DXVECTOR3 m_vDestPos;
	DWORD m_dwIdTarget;
	CTimer m_timer;
	DWORD m_dwMoveType;

public:

	CAIHuman();
	CAIHuman( CObj* pObj );
	virtual	~CAIHuman();

#ifdef __POSTAIMSG1008
	BOOL StateInit( const AIMSG & msg );
	BOOL StateIdle( const AIMSG & msg );
	BOOL StateWander( const AIMSG & msg );
	BOOL StatePursue( const AIMSG & msg );
	BOOL StateEvade( const AIMSG & msg );
	BOOL StateRage( const AIMSG & msg );
#else	// __POSTAIMSG1008
	BOOL StateInit();
	BOOL StateIdle();
	BOOL StateWander();
	BOOL StatePursue();
	BOOL StateEvade();
	BOOL StateRage();
#endif	// __POSTAIMSG1008

public:
	static	MemPooler<CAIHuman>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIHuman::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIHuman::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIHuman::m_pPool->Free( (CAIHuman*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIHuman::m_pPool->Free( (CAIHuman*)lpMem );	}

	DECLARE_AISTATE()
};
#endif

