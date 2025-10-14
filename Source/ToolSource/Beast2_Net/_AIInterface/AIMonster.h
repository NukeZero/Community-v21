#ifndef __AI_MONSTER_H
#define __AI_MONSTER_H

#include "mempooler.h"
#include "Mover.h"

class CAIMonster : public CAIInterface
{
#ifdef __NPP_050308
protected:
#endif 
	DWORD m_dwIdTarget;
	D3DXVECTOR3 m_vPosBegin;
	D3DXVECTOR3 m_vDestPos;
	D3DXVECTOR3	m_vOldPos;			// 이전 프레임 좌표
	CTimer m_timerMove;
	CTimer m_timerAttackDelay;
	CTimer m_timerAttack;		// 추적 카운터.
	CTimer m_timerTrace;
	BOOL m_bRangeOut;			// 활동반경을 벗어났다.
	//DWORD m_dwMoveState;
	BOOL m_bReturnToBegin;
	BOOL m_bGoTarget;
	BOOL m_bTargetNoMovePos;
	DWORD m_dwAtkMethod;
	int  m_nCollisionCount;
	DWORD	m_tmReturnToBegin;			// 리젠지점으로 돌아가는 시간측정.
	BOOL m_bFirstRunaway;
	BOOL m_bCallHelper;
	//CTimer m_timerValidAttack;
	BOOL	m_bRangeAttack;				// 레인지 어택이냐 아니냐.
#ifdef __XAI1013	
	OBJID	m_idSummon[ MAX_SUMMON ];	// 소환된 몬스터는 아이디가 채워진다.
#endif
	DWORD	m_tmSummon;					// 소환 타이머.
	DWORD	m_tmHelp;					// 도움 요청 타이머.
	BOOL	m_bLootMove;				// 루팅하러 가는중.
	DWORD	m_idLootItem;				// 루팅할 아이템.

	DWORD	GetAtkMethod_Near();
	DWORD	GetAtkMethod_Far();
	DWORD	GetAtkRange( DWORD dwAtkMethod );
	ItemProp* GetAtkProp( DWORD dwAtkMethod );
	DWORD	GetAtkItemId( DWORD dwAtkMethod );
	void	DoAttack( DWORD dwAtkMethod, CMover* pTarget );
	void	DoReturnToBegin( void );
		

	void MoveToDst(	OBJID dwIdTarget );
	void MoveToDst(	D3DXVECTOR3 vDst );
	void MoveToRandom( UINT nState );

	void CallHelper( MoverProp* pMoverProp );

	void SetStop( DWORD dwTime ) { m_timerMove.Set( (float)( dwTime ) ); GetMover()->SetStop(); } 
	BOOL IsEndStop() { return GetMover()->m_pActMover->IsMove() == FALSE && m_timerMove.IsTimeOut() == TRUE; }
	BOOL IsMove() { return GetMover()->m_pActMover->IsMove(); }
	BOOL IsInRange( D3DXVECTOR3 vDistant, FLOAT fRange );
	int		SelectAttackType( CMover *pTarget );	// AI에 따른 공격방식을 선택.
		
#ifdef __POSTAIMSG1008
	BOOL  MoveProcessIdle( const AIMSG & msg );
	BOOL  MoveProcessRage( const AIMSG & msg );
	BOOL  SubAttackChance( const AIMSG & msg, CMover *pTarget );
#else	// __POSTAIMSG1008
	BOOL  MoveProcessIdle();
	BOOL  MoveProcessRage();
	BOOL  SubAttackChance( CMover *pTarget );
#endif	// __POSTAIMSG1008
	BOOL  MoveProcessRunaway();
	
	BOOL StopProcessIdle();
		
public:

	CAIMonster();
	CAIMonster( CObj* pObj );
	virtual ~CAIMonster();

	virtual void InitAI();

#ifdef __POSTAIMSG1008
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateWander( const AIMSG & msg );
	BOOL	StateRunaway( const AIMSG & msg );
	BOOL	StateEvade( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );
#else	// __POSTAIMSG1008
	BOOL StateInit();
	BOOL StateIdle();
	BOOL StateWander();
	BOOL StateRunaway();
	BOOL StateEvade();
	BOOL StateRage();
#endif	// __POSTAIMSG1008

	void SetAttackTarget( OBJID idTarget ) ;
	void SubSummonProcess( CMover *pTarget );
	BOOL SubItemLoot( void );
		
public:
	static	MemPooler<CAIMonster>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIMonster::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIMonster::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIMonster::m_pPool->Free( (CAIMonster*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIMonster::m_pPool->Free( (CAIMonster*)lpMem );	}
	
	DECLARE_AISTATE()
};

#endif

