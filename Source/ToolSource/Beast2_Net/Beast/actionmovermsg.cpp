#include "stdafx.h"
#include "defineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"


#include "party.h"
extern	CPartyMng	g_PartyMng;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 현재 상태를 무시하고 새로운 동작으로 강제 세팅.
// 서버와 동기화 할때외엔 사용하지 말것.
void	CActionMover::ForcedSet( D3DXVECTOR3 &vDelta, DWORD dwState, DWORD dwStateFlag, int nMotionEx )
{

	if( ( GetStateFlag() & OBJSTAF_FLY ) == 0 && ( dwStateFlag & OBJSTAF_FLY ) )
	{
//		WriteLog( "ForcedSet(): 0 & 1" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_FLY ) && ( dwStateFlag & OBJSTAF_FLY ) == 0 )
	{
//		WriteLog( "ForcedSet(): 1 & 0" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_SIT ) == 0 && ( dwStateFlag & OBJSTAF_SIT ) )
	{
//		WriteLog( "ForcedSet()OBJSTAF_SIT: 0 & 1" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_SIT ) && ( dwStateFlag & OBJSTAF_SIT ) == 0 )
	{
//		WriteLog( "ForcedSet()OBJSTAF_SIT: 1 & 0" ); 
		return;
	}

	m_vDelta = vDelta;
	__ForceSetState( dwState );	// 여기 이외엔 이함수 사용하지 말것.
			
	ClearStateFlag();
	AddStateFlag( dwStateFlag );

	if( (GetStateFlag() & OBJSTAF_SIT) && ((dwState & OBJSTA_MOVE_ALL) != OBJSTA_SIT) )	// IsSit()인데
	{
		Message( "ForcedSet : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	} 
	else if( (GetStateFlag() & OBJSTAF_SIT) == 0 && ((dwState & OBJSTA_MOVE_ALL) == OBJSTA_SIT) )	// IsSit()아닌데 SIT이면
	{
		Message( "ForcedSet : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	}

	m_nMotionEx = nMotionEx;
	DefaultSet();
}

void	CActionMover::ForcedSet2( D3DXVECTOR3 &vDelta, float fAccPower, float fTurnAngle, DWORD dwState, DWORD dwStateFlag, int nMotionEx )
{
	if( ( GetStateFlag() & OBJSTAF_FLY ) == 0 && ( dwStateFlag & OBJSTAF_FLY ) )
	{
//		WriteLog( "ForcedSet2(): 0 & 1" ); 
		return;
	}
	if( ( GetStateFlag() & OBJSTAF_FLY ) && ( dwStateFlag & OBJSTAF_FLY ) == 0 )
	{
//		WriteLog( "ForcedSet2(): 1 & 0" ); 
		return;
	}

	m_vDelta = vDelta;
	m_fAccPower	= fAccPower;
	m_fTurnAngle	= fTurnAngle;
	__ForceSetState( dwState );
	ClearStateFlag();
	AddStateFlag( dwStateFlag );
	m_nMotionEx = nMotionEx;
	DefaultSet();
	if( (GetStateFlag() & OBJSTAF_SIT) && (dwState != OBJSTA_SIT))	// IsSit()인데
	{
		Message( "ForcedSet2 : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	} 
	else if( (GetStateFlag() & OBJSTAF_SIT) == 0 && (dwState == OBJSTA_SIT) )	// IsSit()아닌데 SIT이면
	{
		Message( "ForcedSet2 : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	}
}

void	CActionMover::DefaultSet( void )
{
//	m_pMover->ResetDestParam( DST_SPEED, 0, FALSE );
	m_pMover->m_fCrrSpd		= 0.0f;
}

//
//	Action Message Process
//	액션 메시지를 받아 처리한다.
//	어떤 행위가 발생하는 시점에 대한 처리를 담당.
//	최적화를 위해서 이미 설정되어 있는 상태면 중복 처리 하지 않음
//
int		CActionMover::ProcessActMsg1( CMover* pMover,  OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 )
{
	return 1;
}


/*

  p0 + tD0 = -e0

  t = -( p0 + e0 ) / d0

void	Test( RECT r, Point p, Point.d )
{
	Point t;	// 교차점

	tx = (p0 + 
	
}

*/


