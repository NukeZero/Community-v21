#ifndef __AI_PET_H
#define __AI_PET_H

#include "mempooler.h"
#include "Mover.h"

class CAIPet : public CAIInterface
{
private:
	OBJID m_idOwner;		// 주인님
	BOOL	m_bLootMove;				// 루팅하러 가는중.
	DWORD	m_idLootItem;				// 루팅할 아이템.
	int		m_nState;					// 현재 상태.
	
	void MoveToDst(	OBJID idTarget );
	void MoveToDst(	D3DXVECTOR3 vDst );
	

#ifdef __POSTAIMSG1008
	BOOL  MoveProcessIdle( const AIMSG & msg );
	BOOL  MoveProcessRage( const AIMSG & msg );
#else	// __POSTAIMSG1008
	BOOL  MoveProcessIdle();
	BOOL  MoveProcessRage();
#endif	// __POSTAIMSG1008
	

	void Init( void );
	void Destroy( void );
	
public:
	CAIPet();
	CAIPet( CObj* pObj );
	virtual ~CAIPet();

	virtual void InitAI();

	void	SetOwner( OBJID idOwner ) { m_idOwner = idOwner; }
#ifdef __POSTAIMSG1008
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );
#else	// __POSTAIMSG1008
	BOOL StateInit();
	BOOL StateIdle();
	BOOL StateRage();
#endif	// __POSTAIMSG1008

	BOOL SubItemLoot( void );
		
public:
	static	MemPooler<CAIPet>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIPet::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIPet::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIPet::m_pPool->Free( (CAIPet*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIPet::m_pPool->Free( (CAIPet*)lpMem );	}
	
	DECLARE_AISTATE()
};

#endif

