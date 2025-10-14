#include "stdafx.h"

#ifdef __CLIENT
#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif
#endif
#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		STATE_FLAG2(A)	(pMover->m_dwStateFlag & (A))
#define		P_ACC		0.01f
#define		P_MAXACC	0.008f

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CLIENT

CAIMover::CAIMover() //: m_FSMclass( STATE_ID_UNCARING )
{
	m_pMover = NULL;
#ifdef __WORLDSERVER
	m_cbDead	= 0;
#endif
}
CAIMover::CAIMover( CObj* pObj ) //: m_FSMclass( STATE_ID_UNCARING )
{
	m_vPosBegin = pObj->GetPos();
	m_nActionDelay = 0;
	CMover* pMover = (CMover*) pObj;
	SendActMsg( pMover, OBJMSG_MODE_WALK, 0, 0 );
	m_pMover = pMover;
}
int CAIMover::Process( CObj* pObj ) 
{
	return TRUE;
}
int CAIMover::Execute( CObj* pObj, int nFlag, int nParam1, int nParam2, int nParam3 )
{
	CMover* pMover = (CMover*) pObj;
	return 1;
}
DWORD CAIMover::ScanTarget( CMover* pMover  )
{
	CObj* pObj;
	D3DXVECTOR3 vPos = pMover->GetPos();
	FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 10 )
	{
		if( pObj->GetType() == OT_MOVER )
		{
			if( ((CMover*)pObj)->IsPlayer() )
			{
			//	m_FSMclass.StateTransition( INPUT_ID_PLAYER_SEEN );
				return ((CMover*)pObj)->GetId();
			}
		}
	}
	END_LINKMAP
	return NULL_ID;
}
#endif
