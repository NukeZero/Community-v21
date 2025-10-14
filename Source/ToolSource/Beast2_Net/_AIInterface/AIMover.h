#ifndef __AI_MOVER_H
#define __AI_MOVER_H

#include "..\_Common\MoverMsg.h"
//#include "..\_AIInterface\FuSMclass.h"
//#include "..\_AIInterface\FSMclass.h"
/*
enum
{
	INPUT_ID_PLAYER_SEEN = 1,
	INPUT_ID_PLAYER_ATTACKS,
	INPUT_ID_PLAYER_CRITICAL_ATTACKS,
	INPUT_ID_PLAYER_GONE,
	INPUT_ID_MONSTER_HURT,
	INPUT_ID_MONSTER_HEALED
};
#define	STATE_ID_BERSERK   0x00000001
#define	STATE_ID_RAGE      0x00000002
#define	STATE_ID_MAD       0x00000003
#define	STATE_ID_ANNOYED   0x00000004
#define	STATE_ID_UNCARING  0x00000005
#define	STATE_ID_FEAR      0x00000006
#define	STATE_ID_RUNAWAY   0x00000007
*/
class CAIMover : public CAIInterface
{
public:
	CMover*   m_pMover;
	//FuSMclass m_FuSMclass;
	//FSMclass  m_FSMclass;
	short	  m_cbDead;

	int    m_nActionDelay;
	CTimer m_timerAction ;
	CTimer m_timerBlock  ;
	D3DXVECTOR3 m_vPosBegin;
	CAIMover();
	CAIMover( CObj* pObj );

	DWORD   ScanTarget( CMover* pMover  );

virtual	int Execute( CObj* pObj, int nFlag, int nParam1, int nParam2, int nParam3 );
virtual	int Process( CObj* pObj);
};
#endif
