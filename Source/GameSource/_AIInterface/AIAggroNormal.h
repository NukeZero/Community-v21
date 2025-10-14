#pragma once 

#if defined __WORLDSERVER && defined __AGGRO16

//20100802 : 인던 외에는 절때 적용 할일이 없다고 함.
// 만일 인던에 넣을경우. 몹 소유권 관련한 처리를 해줘야함.

class CAIAggroNormal : public CAIAggroInterface
{
public:
	CAIAggroNormal( CObj* pObj, const DWORD dwAggroID );
	virtual ~CAIAggroNormal();

	virtual void			InitAI();
	virtual	BOOL			IsReturnToBegin( void )		{	return m_bReturnToBegin;	}
	virtual BOOL			IsIdleMode();

protected:
	DWORD					GetAtkMethod_Near();
	DWORD					GetAtkMethod_Far();
	DWORD					GetAtkRange( DWORD dwAtkMethod );
	PT_ITEM_SPEC			GetAtkProp( DWORD dwAtkMethod );
	DWORD					GetAtkItemId( DWORD dwAtkMethod );
	void					DoAttack( DWORD dwAtkMethod, CMover* pTarget );
	void					DoReturnToBegin( BOOL bReturn = TRUE );
	void					MoveToDst( OBJID dwIdTarget );
	void					MoveToDst( const D3DXVECTOR3& vDst );
	void					MoveToRandom( UINT nState );
	void					CallHelper( const MoverProp* pMoverProp );
	void					SetStop( DWORD dwTime );
	BOOL					IsEndStop();
	BOOL					IsMove() { return GetMover()->m_pActMover->IsMove(); }
	BOOL					IsInRange( const D3DXVECTOR3& vDistant, FLOAT fRange );
	int						SelectAttackType( CMover *pTarget );	// AI에 따른 공격방식을 선택.
	BOOL					MoveProcessIdle( const AIMSG & msg );
	BOOL					MoveProcessRage( const AIMSG & msg );
	BOOL					SubAttackChance( const AIMSG & msg, CMover *pTarget );
	BOOL					MoveProcessRunaway();
	BOOL					StopProcessIdle();
	void					SubSummonProcess( CMover *pTarget );
	BOOL					SubItemLoot();
	void					Init();
	BOOL					StateInit( const AIMSG & msg );
	BOOL					StateIdle( const AIMSG & msg );
	BOOL					StateWander( const AIMSG & msg );
	BOOL					StateRunaway( const AIMSG & msg );
	BOOL					StateEvade( const AIMSG & msg );
	BOOL					StateRage( const AIMSG & msg );

	BOOL					StateRagePatrol( const AIMSG & msg );
	BOOL					MoveProcessRagePatrol( const AIMSG & msg );	
	BOOL					StateStand( const AIMSG & msg );
	BOOL					StatePatrol( const AIMSG & msg );
	BOOL					MoveProcessStand( const AIMSG & msg );
	BOOL					MoveProcessPatrol( const AIMSG & msg );

	void					SetTarget( OBJID dwIdTarget, u_long uParty );


	CMover *				ScanTargetAndSetTarget( int nRangeMeter = 5, int nJobCond = 0, DWORD dwQuest = 0, DWORD dwItem = 0, int nChao = 0 );

public:
	MEMPOOLER_DECLARE( CAIAggroNormal );

	DECLARE_AISTATE( CAIAggroNormal )


protected:
	DWORD 					m_tmMove;
	DWORD					m_tmAttackDelay;
	DWORD					m_tmAttack;		
	DWORD					m_tmSummon;					// 소환 타이머.
	DWORD					m_tmHelp;					// 도움 요청 타이머.
	DWORD					m_tmReturnToBegin;			// 리젠지점으로 돌아가는 시간측정.

	D3DXVECTOR3				m_vPosBegin;
	D3DXVECTOR3				m_vDestPos;
	D3DXVECTOR3				m_vOldPos;					// 이전 프레임 좌표

	D3DXVECTOR3				m_vPosDamage;

	_VECINFO				m_vecPatrolInfo;
	FLOAT					m_fAngleBegine;

	BOOL					m_bReturnToBegin;
	BOOL					m_bGoTarget;
	BOOL					m_bTargetNoMovePos;
	BOOL					m_bFirstRunaway;
	BOOL					m_bCallHelper;
	BOOL					m_bRangeAttack;				// 레인지 어택이냐 아니냐.
	BOOL					m_bLootMove;				// 루팅하러 가는중.

	DWORD					m_dwIdTarget;


	DWORD					m_dwAtkMethod;
	std::vector<OBJID>			m_vecIdSummon;					// 소환된 몬스터는 아이디가 채워진다.
	DWORD					m_idLootItem;				// 루팅할 아이템.
};

#endif 
