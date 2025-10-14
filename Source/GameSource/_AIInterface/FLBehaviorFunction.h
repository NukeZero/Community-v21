#pragma once

#include "FLFSM.h"


#define						SUB_MONSTER_DIST		3.5f


class CMover;

class FLPartyAggroLeaderBehavior
{
public:
	FLPartyAggroLeaderBehavior();
	virtual ~FLPartyAggroLeaderBehavior();

	CMover *				TryScanTargetByNear_Party( const FLAIOptionalFlags & kOptionalFlags );
	void					TryDoSummon( CMover & Leader, CWorld * pWorld/*, const MoverProp & Prop*/ );

	void					Update_CleanKilledSubMonster( );
	void					ClearAllSubMonster();
	void					KillSubMonsterAll( CMover * pAttacker );

	void					SetSubMonsterPos( CMover & Leader );

	D3DXVECTOR3				GetSubMonsterPosition( const D3DXVECTOR3 vCenterPos, const float fAngle, const DWORD nSequenceIndex, const DWORD nSequenceMax, const float fDist );

private:
	typedef					MoverProp::SummonMonstersMap	CalledMonsterMap;
	CalledMonsterMap		m_mapCalledMonster;
};

class FLPatrolBehavior
{
public:
	FLPatrolBehavior();
	virtual ~FLPatrolBehavior();

	void					Clear();			
	void					SetRandomPatrolPos( CMover & Src, const D3DXVECTOR3 & vPosBegin, const float fPatrolRange );		

	BOOL					PatrolUpdate( CMover & Src, const D3DXVECTOR3 & vPosBegin, const float fPatrolRange );		
	
	void					SetStopMoverTimeOut( CMover & Src, const DWORD tmStopInterval );		

private:
	DWORD					m_tmStopMoverTimeOut;
};

class FLRunawayNSelfHealBehavior
{
public:
	FLRunawayNSelfHealBehavior();
	virtual ~FLRunawayNSelfHealBehavior();

	void					Clear();		//AIMSG_INIT;
	void					StartInit( CMover & Src, CMover & Target );
	void					RunawayUpdate( CMover & Src, CMover & Target );
	
	void					SetStopMoverTimeOut( CMover & Src, const DWORD tmStopInterval );
	BOOL					IsFirstRunaway() const;

private:
	BOOL					m_bFirstRunaway;
	DWORD					m_tmStopMoverTimeOut;
};


class FLAttackBehavior
{
public:
	FLAttackBehavior();
	virtual ~FLAttackBehavior();

	void					Clear();

	void					SetAttackPointTime( const DWORD tmAttackPoint );
	DWORD					GetAttackPointTime() const; 
	void					SetRageTimeOutTime( const DWORD tmRangTimeOutInterval );
	DWORD					GetRageTimeOutTime() const;

	BOOL					AutoChoiceRangeAttack( CMover & Src, CMover & Target );
	void					SetMoveForRangeAttacking( CMover & Src, CMover & Target );		
	BOOL					TryAttack( CMover & Src, CMover & Target, const BOOL bRangeAttack, const DWORD tmRageTimeoutInterval );
		
private:
	BOOL					DoAttack( CMover & Src, CMover & Target, const DWORD dwAttackMethod, const DWORD tmRageTimeoutInterval );
	DWORD					GetAtkRange( CMover & Src, const DWORD dwAtkMethod );

	DWORD					GetAtkMethod_Near( CMover & Src );
	DWORD					GetAtkMethod_Far( CMover & Src );
	DWORD					GetAtkItemId( CMover & Src, const DWORD dwAtkMethod );

private:
	DWORD					m_tmAttackPoint;
	DWORD					m_tmRageTimeOut;
};

class FLSummonMonsterBehavior
{
public:
	FLSummonMonsterBehavior();
	virtual ~FLSummonMonsterBehavior();
	
	void					Clear();
	void					TrySummonUpdate( CMover & Src );

private:
	typedef					std::vector< OBJID >
							SummonVector;

	DWORD					m_tmSummonPoint;
	SummonVector			m_vecCalledObjID;
};


class FLCallHelperBehavior
{
public:
	FLCallHelperBehavior();
	virtual ~FLCallHelperBehavior();

	void					Clear();
	void					TryCallHelper( CMover & Src, CMover & Target );

private:
	DWORD					m_tmHelpPoint;
	DWORD					m_bStopCallHelper;
};



void FLSetRandomPos( CMover & Src );




//Pet, Player, Monster
enum E_AI_TARGET_TYPE		//@@@@@@@  적대치가 상호 있으면 그걸로 대체
{	
	AI_TARGET_TYPE_PLAYER,
	AI_TARGET_TYPE_MONSTER, 
};

//utility
BOOL					IsInRange( const D3DXVECTOR3 & vDist, const float fRange );

//action
void					SetMoveToDest( CMover & Src, const D3DXVECTOR3 & vDest );	
void					SetMoveToDest( CMover & Src, const OBJID dwOBJID );		
void					SetReturnToBegin( CMover & Src, const D3DXVECTOR3 & vPosBegin );
void					SetMaxHP( CMover & Src );

//scan
#ifdef __AGGRO16
class FLAggro;
#endif // __AGGRO16

class FLScanTarget
{
public:
	static BOOL					IsValidTarget( CMover & AIMover, CMover & Target, const FLAIOptionalFlags & kOptionalFlags );

	static CMover *				Stalking( CMover & AIMover, const OBJID objid, const FLAIOptionalFlags & kOptionalFlags );
	static CMover *				ByNear( CMover & Behavior, CObj & CenterObj, const int nRangeMeter, const FLAIOptionalFlags & kOptionalFlags  );
#ifdef __AGGRO16
	static CMover *				ByAggro( CMover & AIMover,  const FLAIOptionalFlags & kOptionalFlags );
#endif // __AGGRO16
};



//BOOL					IsValidTarget( CMover & AIMover, CMover & Target, const FLAIOptionalFlags & kOptionalFlags );
//CMover *				StalkingTarget( CMover & AIMover, const OBJID objid, const FLAIOptionalFlags & kOptionalFlags );
////CMover *				ScanTargetByNear( CMover & Src, CObj & CenterObj, const int nRangeMeter/*, const E_AI_TARGET_TYPE eTargetType*/ );		
//CMover *				ScanTargetByNear( CMover & Behavior, CObj & CenterObj, const int nRangeMeter, const FLAIOptionalFlags & kOptionalFlags  );



//#ifdef __AGGRO16
//class FLAggro;
////CMover *				TryScanValidTargetByAggro( CMover & Src );
////CMover *				TryScanValidTargetByAggroFSM( FLAggro & AggroMng );
//CMover *				TryScanValidTargetByAggro( CMover & AIMover,  const FLAIOptionalFlags & kOptionalFlags );
//#endif // __AGGRO16


BOOL					RecvConditionUpdate( CMover & Src );

