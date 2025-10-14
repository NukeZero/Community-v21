#pragma once

#include "FLFSM.h"
//#include "FLMoverAction.h"
#include "FLBehaviorFunction.h"

#include "FLAggro.h"



#if defined __WORLDSERVER && defined __AGGRO16
class FLAggroNormalFSM : public FLFSM< FLAggroNormalFSM >
{
private:
	enum						{			RAGE_TIME_OVER_MAX	= SEC( 15 ),		};


private:
	class FLIdleState : public FLAggroNormalFSM::FLState			//재활용 할라면 template화 해서 밖으로 빼시던가하셈.
	{
	public:
		virtual	void			OnEnter( FLAggroNormalFSM & FSM );
		virtual void			UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLAggroNormalFSM & FSM );
	};

	class FLRageState : public FLAggroNormalFSM::FLState
	{
	public:
		FLRageState();
		virtual ~FLRageState();

		void					Clear();

		virtual	void			OnEnter( FLAggroNormalFSM & FSM );
		virtual void			UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLAggroNormalFSM & FSM );

	private:
		DWORD					m_tmAttackPoint;
	};

	class FLTokenToBeginState : public FLAggroNormalFSM::FLState
	{
		virtual	void			OnEnter( FLAggroNormalFSM & FSM );
		virtual void			UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLAggroNormalFSM & FSM );
	};

	class FLRunawayState : public FLAggroNormalFSM::FLState
	{
		virtual	void			OnEnter( FLAggroNormalFSM & FSM );
		virtual void			UpdateAI( FLAggroNormalFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLAggroNormalFSM & FSM );
	};

public:
	FLAggroNormalFSM( CMover & Owner, const DWORD dwAggroID );//, AggroPtr pAggro );
	virtual ~FLAggroNormalFSM();

	void					SetPartyLeader( const DWORD dwLeaderObjid );

	virtual void			InitProp();

	void					ClearAll();
	void					ClearWithoutProp();
	virtual FLAggro *		GetAggroMng();

	virtual void			SetManualTarget( const OBJID /*objid*/ ) { __noop; }

protected:
	virtual void			sendAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 );

public:
	FLIdleState				m_IdleState;
	FLRageState				m_RageState;
	FLTokenToBeginState	m_ReturnToBeginState;
	FLRunawayState			m_RunawayState;

public:
	//FLAIOptionalFlags			m_OptinalFlags;


	OBJID					m_dwPartyOwnerOBJID;
	// 
	OBJID					m_OwnerObjid;
	FLAggro *				m_pAggro;

	// 파티 관련.
	FLPartyAggroLeaderBehavior	m_LeaderBehavior;
	

	//neodeath : 한눈에 필요한 것들을 보게 상태가 아니라 FSM에 때려 박음 (초기화 코드도 한곳에 몰음, 변수 설정 함수 만들기 귀찮음. -_-)
	//IDLE
	FLPatrolBehavior			m_Patrol;
	D3DXVECTOR3					m_vPosBegin;			
	float						m_fPatrolRange;
	
	//RAGE 
	FLAttackBehavior			m_Attack;
	D3DXVECTOR3					m_vOldPos;				//케릭터가 갖혔나 체크하는 위치


	FLSummonMonsterBehavior		m_TrySummonMonster;
	DWORD						m_tmRageTimeOutInterval;

	FLCallHelperBehavior		m_CallHelper;			//주변 몬스터 부름.
	
	//RUNAWAY
	FLRunawayNSelfHealBehavior	m_RunawayNSelfHeal;
};


#endif //defined __WORLDSERVER && defined __AGGRO16
