// Billboard.cpp : implementation of the CBeastView class
//

#include "stdafx.h"
#include "Action.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAction::CAction()
{
	Init();
}
CAction::CAction( CMover* pMover ) 
{ 
	Init();
	m_pMover = pMover;
}

CAction::~CAction()
{
}

void CAction::Init( void )
{
	m_pMover		= NULL;
	m_dwState		= 0;
	m_dwStateFlag	= 0;
}

#ifdef _DEBUG
void	CAction::AddStateFlag( DWORD dwFlag ) 
{ 
	m_dwStateFlag |= dwFlag; 
	if( m_dwStateFlag & OBJSTAF_ACC )
	{
		int a = 0;
	}
}
void	CAction::RemoveStateFlag( DWORD dwFlag )
{
	m_dwStateFlag &= (~dwFlag);
}
#endif

void CAction::ProcessAtkMsg( void )
{
}


CWorld* CAction::GetWorld( ) 
{ 
	return m_pMover->GetWorld(); 
}

// OBJSTA_MOVE_ALL 스테이트가 클리어되는 순간 발생.
void	CAction::OnEndMoveState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
	switch( m_dwState & OBJSTA_MOVE_ALL )
	{
	case OBJSTA_SIT:		// 착지 동작 중.
		m_dwStateFlag &= (~OBJSTAF_SIT);
		break;
	}	
}

void	CAction::OnEndTurnState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::OnEndLookState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::OnEndJumpState( DWORD dwState )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
	switch( dwState & OBJSTA_JUMP_ALL )
	{
	case OBJSTA_SJUMP4:		// 착지 동작 중.
		break;
	}
}

void	CAction::OnEndAttackState( DWORD dwState )
{

}

void	CAction::OnEndDamageState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::OnEndActionState( void )
{
	// 이곳에 그외 필요한 코드를 넣으셈.
}

void	CAction::ClearState( void )
{
	DWORD dwState = m_dwState;
	// 0xFFFFFFFF 로 한꺼번에 리셋 시키지 말것.
//	ResetState( OBJSTA_MOVE_ALL | OBJSTA_TURN_ALL | OBJSTA_LOOK_ALL | OBJSTA_JUMP_ALL | OBJSTA_ATK_ALL | OBJSTA_DMG_ALL );
	ResetState( OBJSTA_ALL );
	
	if( m_dwState )
	{
		Message( "CAction::ClearState : 아직도 뭔가 클리어되지 않은 값이 있다. %08x, %08x", dwState, m_dwState );
		m_dwState = 0; 
	}
}

// dwState에는 OBJSTA들이 섞여서 올 수 있다.
// 리셋하고자 하는 스테이트만 클리어시키면서 해당 핸들러 불러줌.
void	CAction::ResetState( DWORD dwState )
{ 
	DWORD dw	= m_dwState;
	if( dwState & OBJSTA_MOVE_ALL )
	{
		OnEndMoveState();				// 핸들러 호출
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_TURN_ALL )
	{
		OnEndTurnState();
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_LOOK_ALL )
	{
		OnEndLookState();
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_JUMP_ALL )
	{
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
		OnEndJumpState( dw );
	}
	
	if( dwState &	OBJSTA_ATK_ALL )
	{
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
		OnEndAttackState(dw);
	}
	
	if( dwState &	OBJSTA_DMG_ALL )
	{
		OnEndDamageState();
		m_dwState &= (~dwState);		// 해당 비트값 클리어.
	}
	
	if( dwState & OBJSTA_ACTION_ALL )
	{
		OnEndActionState();
		m_dwState &= (~dwState);
	}
}
