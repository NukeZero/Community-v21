#ifndef __AI_INTERFACE_H
#define __AI_INTERFACE_H

#define CAI CAIInterface

#include "MoverMsg.h"

class CAIInterface;

struct AIMSG
{
	DWORD dwMessage;
	DWORD dwParam1;
	DWORD dwParam2;
#ifdef __DEADLOCK__
	long	lIdx;
#endif	// __DEADLOCK__
};

#ifdef __POSTAIMSG1008
typedef BOOL (CAIInterface::*FUNCAISTATE)( const AIMSG & );
#else	// __POSTAIMSG1008
typedef BOOL (CAIInterface::*FUNCAISTATE)();
#endif	// __POSTAIMSG1008

struct STATEMAP_ENTRIES
{
	DWORD dwAIStateId;
	FUNCAISTATE pStateFunc;
};// STATEMAP_ENTRIES,* LPSTATEMAP_ENTRIES;

struct STATEMAP
{
	STATEMAP* lpBaseStateMap;
	STATEMAP_ENTRIES* lpStateMapEntries;
};// STATEMAP,* LPSTATEMAP;

enum
{
	AIMSG_INIT = 1,
	AIMSG_PROCESS, 
	AIMSG_TIMEOUT,
	AIMSG_SETSTATE,
	AIMSG_SETPROCESS,
	AIMSG_DSTDAMAGE,
	AIMSG_DSTDIE,
	AIMSG_DAMAGE,
	AIMSG_DIE,
	AIMSG_COLLISION,		// 충돌 Collision
	AIMSG_ARRIVAL,			// 이동 목표에 도착 
	AIMSG_NOMOVE,			// 갈 수 없음 
	AIMSG_BEGINMOVE,		// 이동 시작시 
	AIMSG_ATTACK_MELEE,		// 일반공격시 타점에서 호출.
	AIMSG_END_MELEEATTACK,	// 일반공격 끝났을때 호출.
	AIMSG_END_APPEAR,		// 등장씬 끝남.
	AIMSG_INIT_TARGETCLEAR,
	AIMSG_EXIT
};
//////////////////////////////////////////////////////////////////////////////
// AI 메시지 핸들러 매크로 정의 
#define BeginAIHandler() if( 0 ) { 
#ifdef __POSTAIMSG1008
#define OnMessage( x ) return TRUE; } else if( msg.dwMessage == x )	{
#else	// __POSTAIMSG1008
#define OnMessage( x ) return TRUE; } else if( m_AIMsg.dwMessage == x )	{ 
#endif	// __POSTAIMSG1008
#define EndAIHandler() return TRUE; }

// State Map 핸들러 매크로 정의 
#define BEGIN_AISTATE_MAP( theClass, baseClass ) \
	const STATEMAP* theClass::GetStateMap() const  \
		{ return (STATEMAP*)&theClass::stateMap; } \
	const STATEMAP theClass::stateMap = \
		{ (STATEMAP*)&baseClass::stateMap, (STATEMAP_ENTRIES*)&theClass::stateMapEntries[0] }; \
	const STATEMAP_ENTRIES theClass::stateMapEntries[] = { \

#define END_AISTATE_MAP() 0, NULL };
#define ON_STATE( id, pStateFunc ) id, (FUNCAISTATE)pStateFunc, 
#define DECLARE_AISTATE() \
	static const STATEMAP_ENTRIES stateMapEntries[]; \
	static const STATEMAP stateMap; \
	virtual const STATEMAP* GetStateMap() const; 

//////////////////////////////////////////////////////////////////////////////

class CAIInterface
{
	FUNCAISTATE GetAIStateFunc( DWORD dwState );

protected:
	queue< AIMSG > m_MsgQueue;
//#ifdef __POSTAIMSG0927
	CRIT_SEC	m_AddRemoveLock;
//#endif	// __POSTAIMSG0927
	//DWORD m_dwState;
	FUNCAISTATE m_pStateFunc;
#ifndef __POSTAIMSG1008
	AIMSG m_AIMsg;
#endif	// __POSTAIMSG1008
	//BOOL m_bProcess;
	CObj* m_pObj;

public:
	DECLARE_AISTATE()
	DWORD m_dwCurrentState;

	// 매시지 관련 처리 매소드 
#ifdef __DEADLOCK__
	void RouteMessage( long lIdx );
#else	// __DEADLOCK__
	void RouteMessage();
#endif	// __DEADLOCK__

	void SendAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	void PostAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );

	virtual void InitAI() { }

	CObj* GetObj() { return m_pObj; }
	CCtrl* GetCtrl() { return (CCtrl*)m_pObj; }
	CItem* GetItem() { return (CItem*)m_pObj; }
	CMover* GetMover() { return (CMover*)m_pObj; }
	CWorld* GetWorld() { return m_pObj->GetWorld(); }

	CAIInterface();
	CAIInterface( CObj* pObj, BYTE byAIInterface );
	virtual	~CAIInterface();

	//기타 잡다한 매소드 
#ifdef __XAI1013
#ifdef __V050324_AISCAN
	CMover* ScanTarget( CObj* pObjCenter, int nRangeMeter = 5, int nJobCond = 0, DWORD dwQuest = 0, DWORD dwItem = 0, int nChao = 0 );
#else
//{{AFX
	CMover* ScanTarget( CObj* pObjCenter, int nRangeMeter = 5, int nJobCond = 0 );
#endif
#else
	CMover* ScanTarget( CObj* pObjCenter, int nRangeMeter = 5 );
//}}AFX
#endif
	CMover* ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter );
	CMover* ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  );
	CMover* ScanNpc( CObj* pObjCenter, DWORD dwObjIdx, int nRangeMeter = 5 );
	BOOL IsValidRange( D3DXVECTOR3* pvPosBegin, D3DXVECTOR3* pvPosCur, FLOAT fRange );
};
//////////////////////////////////////////////////////////////////////////////


#endif

