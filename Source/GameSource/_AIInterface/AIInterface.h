#ifndef __AI_INTERFACE_H
#define __AI_INTERFACE_H

#include <MoverMsg.h>

#include <FLAggro.h>

class CAIInterface;

struct AIMSG
{
	DWORD dwMessage;
	DWORD dwParam1;
	DWORD dwParam2;
};

typedef BOOL (CAIInterface::*FUNCAISTATE)( const AIMSG & );



#pragma	warning( push )
#pragma warning( disable : 4121 )	
struct STATEMAP_ENTRIES
{
	DWORD dwAIStateId;
	FUNCAISTATE pStateFunc;
};
#pragma warning( pop )

struct STATEMAP
{
	STATEMAP* lpBaseStateMap;
	STATEMAP_ENTRIES* lpStateMapEntries;
};

enum
{
	AIMSG_NONE = 0,
	AIMSG_INIT = 1,
	AIMSG_PROCESS, 
	AIMSG_TIMEOUT,				//쫒아가기 타임아웃?
	AIMSG_SETSTATE,	
	AIMSG_SETPROCESS,
	AIMSG_DSTDAMAGE,
	AIMSG_DSTDIE,				//상대방 죽임?
	AIMSG_DAMAGE,
	AIMSG_DIE,					// 죽음		nParam1 ( Attacker pointer ) 
	AIMSG_COLLISION,			// 충돌 Collision
	AIMSG_ARRIVAL,				// 이동 목표에 도착 
	AIMSG_NOMOVE,				// 갈 수 없음 
	AIMSG_BEGINMOVE,			// 이동 시작시 
	AIMSG_ATTACK_MELEE,			// 일반공격시 타점에서 호출.
	AIMSG_END_MELEEATTACK,		// 일반공격 끝났을때 호출.
	AIMSG_END_APPEAR,			// 등장씬 끝남.
	AIMSG_INIT_TARGETCLEAR,		// 타겟을 클리어하고 대기모드로 돌아감.
	AIMSG_EXIT,					// DELETE?
	AIMSG_TARGET_FIND,			// 타겟을 찾았다.
	AIMSG_REATTACK_TIMEOUT,  
	AIMSG_INVALID_TARGET,		// 타겟이 날거나, 없어지거나, 죽거나 .. 등등으로 공격 불가하다. 

	AIMSG_REQUEST_HELP,			//dwParam1 : 타겟
	
	

//	AIMSG_SET_TARGET,			//dwParam1 : 타겟
//	AIMSG_PARTY_MONSTER_DAMAGE,	//dwparam1 : 데미지를 입은놈
//	AIMSG_PARTY_MONSTER_ATTACK,	//dwparam1 : 공격을 한 몬스터
		

//	AIMSG_TARGET_FIND,			// PCObjID
	//AIMSG_FIRST_DAMAGE_NOTI,		//dwParam1 : AttackerObjID
	//AIMSG_FIRST_TARGET_NOTI,		//dwParam1 : TargetObjID
	//AIMSG_DEBUFF_NOTI,			//dwParam1 : AttackerObjID		//FLAggro::SumDebuffAggro( *pSrcMover, *pThisMover );
	//AIMSG_DST_BUFF_NOTI,			//dwParam1 : BehaviorObjID		//FLAggro::SumBuffAggro( *pSrcMover, *pThisMover );
	//AIMSG_DST_HEAL_NOTI,			//dwParam1 : BehaviorObjID
};

//struct AI_STATE	{	enum E {
//	IDLE			= 0,
//	RAGE,
//	RUNAWAY,
//
//};					};

enum AI2_STATE
{
	AI2_IDLE,
	AI2_MOVE,
	AI2_RAGE,
	AI2_SEARCH,			// 탐색 상태 
	AI2_TRACKING,		// 추적 상태 
	AI2_ATTACK,			// 공격 상태 
};

//////////////////////////////////////////////////////////////////////////////
// AI 메시지 핸들러 매크로 정의 
#define BeginAIHandler() if( 0 ) { 
#define OnMessage( x ) return TRUE; } else if( msg.dwMessage == x )	{
#define EndAIHandler() return TRUE; }

// State Map 핸들러 매크로 정의 
#define BEGIN_AISTATE_MAP( theClass, baseClass ) \
	const STATEMAP* theClass::GetStateMap() const  \
		{ return (STATEMAP*)&theClass::stateMap; } \
	const STATEMAP theClass::stateMap = \
		{ (STATEMAP*)&baseClass::stateMap, (STATEMAP_ENTRIES*)&theClass::stateMapEntries[0] }; \
	const STATEMAP_ENTRIES theClass::stateMapEntries[] = { \

#define END_AISTATE_MAP() 0, NULL };
#define ON_STATE( id, pStateFunc ) id, (FUNCAISTATE)&thisClass::pStateFunc, 
#define DECLARE_AISTATE( theClass ) \
	typedef	theClass	thisClass;	\
	static const STATEMAP_ENTRIES stateMapEntries[]; \
	static const STATEMAP stateMap; \
	virtual const STATEMAP* GetStateMap() const; 

//////////////////////////////////////////////////////////////////////////////

#pragma	warning( push )
#pragma warning( disable : 4121 )	

class CAIInterface
{
protected:
	FUNCAISTATE GetAIStateFunc( DWORD dwState );

protected:
	std::queue< AIMSG >	m_MsgQueue;
//	CMclCritSec		m_AddRemoveLock;
	FUNCAISTATE		m_pStateFunc;
	CObj*			m_pObj;

#if defined __WORLDSERVER && defined __AGGRO16
	BOOL			m_HasAggro;
#endif //__AGGRO16

public:
	DECLARE_AISTATE( CAIInterface )

	CAIInterface();
	CAIInterface( CObj* pObj );
	virtual	~CAIInterface();

#if defined __WORLDSERVER && defined __AGGRO16
	BOOL				HasAggro() const 				{ return m_HasAggro; }
	virtual FLAggro *	GetAggroMng() 					{ return NULL; }
#endif //__AGGRO16

	// 매시지 관련 처리 매소드 
	virtual void	RouteMessage();
	virtual void	SendAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	virtual void	PostAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	virtual void	InitAI() { }

	virtual	BOOL	IsReturnToBegin( void )		{	return FALSE;	}
	virtual BOOL	IsIdleMode()			= 0;

protected:
	CMover* GetMover() { return (CMover*)m_pObj; }
	CWorld* GetWorld() { return m_pObj->GetWorld(); }

	//기타 잡다한 매소드 
	//CMover *				CAIInterface::ScanMaxAggro();

	CMover* ScanTarget( CObj* pObjCenter, int nRangeMeter = 5, int nJobCond = 0, DWORD dwQuest = 0, DWORD dwItem = 0, int nChao = 0 );
	CMover* ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter );
	CMover* ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  );

	CMover*	ScanTargetNext( CObj* pObjCenter, int nRange,  OBJID dwIdTarget, u_long uParty );
	u_long	m_uParty;
};
//////////////////////////////////////////////////////////////////////////////

#pragma warning( pop )


#if defined __WORLDSERVER && defined __AGGRO16
class CAIAggroInterface : public CAIInterface 
{
public:
	CAIAggroInterface( CObj * pMover, const DWORD dwAggroID ) ;		//@  AIInterface와의 결합을 Obj가 아니라 Mover에서 해야함
	virtual ~CAIAggroInterface();

	void					Init();
	virtual FLAggro *		GetAggroMng();

	CMover *				ScanTargetByContidion( const int nRangeMeter = 5, const int nJobCond = 0, const DWORD dwQuest = 0, const DWORD dwItem = 0, const int nChao = 0 );
	virtual void			SendAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );

public:
	FLAggro *				m_pAggro;
	//AggroPtr				m_pAggro;
	//FLAggro					m_AggroMng;
};
#endif //__AGGRO16


#endif

