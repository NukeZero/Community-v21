#pragma once

#include "FLFSM.h"
//#include "FLMoverAction.h"
#include "FLBehaviorFunction.h"

//죽어도 한놈만 끝까지 쫒아가는 AI

class FLStalkerFSM : public FLFSM< FLStalkerFSM >
{
private:
	enum						{			RAGE_TIME_OVER_MAX	= SEC( 15 ), 		};

private:
	class FLIdleState : public FLStalkerFSM::FLState
	{
	public:
		virtual	void			OnEnter( FLStalkerFSM & FSM );
		virtual void			UpdateAI( FLStalkerFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLStalkerFSM & FSM );
	};

	class FLRageState : public FLStalkerFSM::FLState
	{
	public:
		FLRageState();
		virtual ~FLRageState();

		void					Clear();

		virtual	void			OnEnter( FLStalkerFSM & FSM );
		virtual void			UpdateAI( FLStalkerFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLStalkerFSM & FSM );

	private:
		DWORD					m_tmAttackPoint;
	};

	class FLTraceCarcassState : public FLStalkerFSM::FLState
	{
	public:
		virtual	void			OnEnter( FLStalkerFSM & FSM );
		virtual void			UpdateAI( FLStalkerFSM & FSM, const AIMSG & msg );
		virtual	void			OnLeave( FLStalkerFSM & FSM );
	};


public:
	FLStalkerFSM( CMover & Owner );
	virtual ~FLStalkerFSM();

	virtual void			InitProp();

	void					ClearAll();
	void					ClearWithoutProp();
	virtual FLAggro *		GetAggroMng()	{	return NULL;	}

	virtual void			SetManualTarget( const OBJID objid );

protected:
	virtual void			sendAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 );

public:
	FLRageState				m_RageState;
	FLIdleState				m_IdleState;
	FLTraceCarcassState			m_TraceCarcassState;

public:
	//FLAIOptionalFlags			m_OptinalFlags;
	D3DXVECTOR3					m_vPosBegin;			
	D3DXVECTOR3					m_vOldPos;				//케릭터가 갖혔나 체크하는 위치
	
	//RAGE 
	FLAttackBehavior			m_Attack;
	OBJID						m_TargetOBJID;	

	DWORD						m_tmRageTimeOutInterval;
};


