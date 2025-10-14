#ifndef __ACTION_H
#define __ACTION_H

#include "MoverMsg.h"

/////////////////////////////////////////////////////////////////////////////
//
//  Mover action msg / state

// 무버의 동작상태를 기술
enum ACTTYPE
{
	ACT_NONE,

	ACT_STOP,				// 대기
	ACT_STOPWALK,			// 제자리걷기
	ACT_FORWARD,			// 전진
	ACT_BACKWARD,			// 후진
	ACT_LEFT,				// 왼쪽으로이동
	ACT_RIGHT,				// 오른쪽으로 이동

	ACT_WALKMODE,			// 걷기/뛰기모드

	ACT_LEFTTURN,			// 왼족으로 도는중
	ACT_RIGHTTURN,			// 오른쪽으로 도는중

	ACT_FJUMPREADY,
	ACT_FJUMP,				// 앞으로 점프중
	ACT_FLAND,
	ACT_SJUMPREADY,
	ACT_SJUMP,				// 제자리 점프중
	ACT_SLAND,
	ACT_BJUMPREADY,
	ACT_BJUMP,				// 백 점프중
	ACT_BLAND,
	ACT_LJUMPREADY,
	ACT_LJUMP,				// 왼족 점프중
	ACT_LLAND,
	ACT_RJUMPREADY,
	ACT_RJUMP,				// 오른쪽 점프중
	ACT_RLAND,

	ACT_ATTACKMODE,			// 전투모드
	ACT_ATTACK1,			// 공격동작 1
	ACT_ATTACK2,
	ACT_ATTACK3,
	ACT_ATTACK4,

	ACT_RANGE1,				// 공격동작 1
	ACT_RANGE2,
	ACT_RANGE3,
	ACT_RANGE4,

	ACT_DAMAGE,				// 피격중
	ACT_DIE					// 죽어있는 중(?)
};

#define	MAX_ACTMSG	0xff

class CAction;

typedef struct tagACTMSG
{
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	int		nParam3;
}	ACTMSG, *LPACTMSG;

typedef	struct	tagACTMSG2
{
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	int		nParam3;
	u_short	puRHit[3];
	BYTE	fSuccess;
}	ACTMSG2, *LPACTMSG2;

class CActMsgq
{
private:
	ACTMSG	m_aActMsg[MAX_ACTMSG];
	u_long	m_uHead;
	u_long	m_uTail;
public:
	//	Constructions
	CActMsgq()	{	m_uHead	= m_uTail	= 0;	}
	virtual ~CActMsgq()	{}

	//	Operations
	VOID		AddTail( const ACTMSG & rActMsg );
	VOID		AddTail( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 );
	LPACTMSG	RemoveHead();
	LPACTMSG	GetHead();
	BOOL		IsEmpty()	{ return( m_uHead == m_uTail );	}
	void		Empty()		{ m_uHead	= m_uTail	= 0; }
};

inline VOID CActMsgq::AddTail( const ACTMSG & rActMsg )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	chASSERT( uTail != m_uHead );
	
	memcpy( ( m_aActMsg + m_uTail ), &rActMsg, sizeof(ACTMSG) );
	m_uTail	= uTail;
}

inline VOID CActMsgq::AddTail( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	chASSERT( uTail != m_uHead );
	
	LPACTMSG lpActMsg	= m_aActMsg + m_uTail;
	lpActMsg->dwMsg	= dwMsg;
	lpActMsg->nParam1	= nParam1;
	lpActMsg->nParam2	= nParam2;
	lpActMsg->nParam3	= nParam3;
	m_uTail	= uTail;
}

inline LPACTMSG CActMsgq::RemoveHead()
{
	if( IsEmpty() )
		return NULL;
	LPACTMSG lpActMsg	= m_aActMsg + m_uHead;
	m_uHead		= ( m_uHead + 1 ) % MAX_ACTMSG;
	return lpActMsg;
}

inline LPACTMSG CActMsgq::GetHead()
{
	if( IsEmpty() )
		return NULL;
	return( m_aActMsg + m_uHead );
}


class CActMsgq2
{
private:
	ACTMSG2	m_aActMsg2[MAX_ACTMSG];
	u_long	m_uHead;
	u_long	m_uTail;
public:
	//	Constructions
	CActMsgq2()	  {	m_uHead	= m_uTail = 0; }
	virtual  ~CActMsgq2()  {}
	//	Operations
	VOID		AddTail( const ACTMSG2 & rActMsg2 );
	VOID		AddTail( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, u_short* puRHit, BYTE fSuccess );
	LPACTMSG2	RemoveHead();
	LPACTMSG2	GetHead();
	BOOL		IsEmpty()	{	return( m_uHead == m_uTail ); }
	void		Empty()		{	m_uHead	= m_uTail	= 0;	  }
};

inline VOID CActMsgq2::AddTail( const ACTMSG2 & rActMsg2 )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	chASSERT( uTail != m_uHead );
	memcpy( ( m_aActMsg2 + m_uTail ), &rActMsg2, sizeof(ACTMSG2) );
	m_uTail	= uTail;
}
inline VOID CActMsgq2::AddTail( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, u_short* puRHit, BYTE fSuccess )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	chASSERT( uTail != m_uHead );
	LPACTMSG2 lpActMsg2	= m_aActMsg2 + m_uTail;
	lpActMsg2->dwMsg	= dwMsg;
	lpActMsg2->nParam1	= nParam1;
	lpActMsg2->nParam2	= nParam2;
	lpActMsg2->nParam3	= nParam3;
	lpActMsg2->fSuccess		= fSuccess;
	memcpy( lpActMsg2->puRHit, puRHit, sizeof(u_short) * 3 );
	m_uTail	= uTail;
}
inline LPACTMSG2 CActMsgq2::RemoveHead()
{
	if( IsEmpty() )
		return NULL;
	LPACTMSG2 lpActMsg2	= m_aActMsg2 + m_uHead;
	m_uHead		= ( m_uHead + 1 ) % MAX_ACTMSG;
	return lpActMsg2;
}
inline LPACTMSG2 CActMsgq2::GetHead()
{
	if( IsEmpty() )
		return NULL;
	return( m_aActMsg2 + m_uHead );
}

class CAction;

class CAtkMsgq2 : public CActMsgq2
{
public:
	CAtkMsgq2()	{}
	virtual ~CAtkMsgq2()	{}
	void	Process( CAction* pActMover, u_short* puRHit );
};

enum OBJMSGTYPE
{	OMT_PRE_PROCESS,	OMT_PRE_TRANSFER,	OMT_DEPENDENT,	OMT_INPUT,	};



class CMeleeAtkMsgq : public CActMsgq
{
public:
	CMeleeAtkMsgq()	{}
	virtual ~CMeleeAtkMsgq() {}
	BOOL	Process( CAction* pActMover );
};

typedef	struct	tagMAGICATKMSG : public ACTMSG
{
	int		nMagicPower;
	int		idSfxHit;
}	MAGICATKMSG, *LPMAGICATKMSG;

class CMover;
class CMagicAtkMsgq
{
private:
	MAGICATKMSG	m_aMagicAtkMsg[MAX_ACTMSG];
	u_long	m_uHead;
	u_long	m_uTail;

public:
	//	Constructions
	CMagicAtkMsgq() { m_uHead = m_uTail	= 0; }
	virtual  ~CMagicAtkMsgq()	{}
	//	Operations
	void	AddTail( OBJMSG dwAtkMsg, int nParam1, int nParam2, int nParam3, int nMagicPower, int idSfxHit )
	{
		u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
		chASSERT( uTail != m_uHead );
		
		LPMAGICATKMSG pMagicAtkMsg	= m_aMagicAtkMsg + m_uTail;
		pMagicAtkMsg->dwMsg	= dwAtkMsg;
		pMagicAtkMsg->nParam1	= nParam1;
		pMagicAtkMsg->nParam2	= nParam2;
		pMagicAtkMsg->nParam3	= nParam3;
		pMagicAtkMsg->nMagicPower	= nMagicPower;
		pMagicAtkMsg->idSfxHit	= idSfxHit;
		m_uTail	= uTail;
	}
	LPMAGICATKMSG	RemoveHead()
	{
		if( IsEmpty() )
			return NULL;
		LPMAGICATKMSG pMagicAtkMsg	= m_aMagicAtkMsg + m_uHead;
		m_uHead		= ( m_uHead + 1 ) % MAX_ACTMSG;
		return pMagicAtkMsg;
	}
	LPMAGICATKMSG	GetHead()
	{
		if( IsEmpty() )
			return NULL;
		return( m_aMagicAtkMsg + m_uHead );
	}
	BOOL	IsEmpty()
	{
		return( m_uHead == m_uTail );
	}
	void	Empty()
	{
		m_uHead	= m_uTail	= 0;
	}
	BOOL	Process( CMover* pMover );
};


class CAction
{
public:
	CAction();
	CAction( CMover* pMover );
	virtual	~CAction();

protected:
	DWORD			m_dwState;			// 동작 상태 - 외부에서 직접 건드리지 말것.
	DWORD			m_dwStateFlag;		// 동작 상태 플래그
	CMover*			m_pMover;
	
public:
	CMeleeAtkMsgq	m_qMeleeAtkMsg;
	CMagicAtkMsgq	m_qMagicAtkMsg;
	CAtkMsgq2		m_qAtkMsg2;
	
public:	
	void			SetMover( CMover* pMover ) { m_pMover = pMover; }
	CMover*			GetMover() { return m_pMover; }
	CWorld*			GetWorld();
	void			Init();
	void			__ForceSetState( DWORD dwState ) { m_dwState = dwState; }		// dwState값으로 강제로 세팅. 서버와 동기화때 외엔 사용하지 말것.
	void			ClearStateFlag() { m_dwStateFlag = 0; }
#ifdef _DEBUG
	void			AddStateFlag( DWORD dwFlag );
	void			RemoveStateFlag( DWORD dwFlag );
#else
	void			AddStateFlag( DWORD dwFlag )     { m_dwStateFlag |= dwFlag; }
	void			RemoveStateFlag( DWORD dwFlag )	 { m_dwStateFlag &= (~dwFlag); }
#endif
	DWORD			GetStateFlag() { return m_dwStateFlag; }

	// 동작 설정/참조
	DWORD			GetState() { return m_dwState; }
	void			SetState( DWORD dwType, DWORD dwState )	{ ResetState(dwType);	m_dwState |= dwState; }
	void			SetMoveState( DWORD dwState ) { ResetState(OBJSTA_MOVE_ALL);	m_dwState |= dwState; }
	void			SetTurnState( DWORD dwState ) { ResetState(OBJSTA_TURN_ALL);	m_dwState |= dwState; }
	void			SetLookState( DWORD dwState ) { ResetState(OBJSTA_LOOK_ALL);	m_dwState |= dwState; }
	void			SetJumpState( DWORD dwState ) { ResetState(OBJSTA_JUMP_ALL);	m_dwState |= dwState; }
	void			SetActionState( DWORD dwState ) { ResetState(OBJSTA_ACTION_ALL);	m_dwState |= dwState; }
	void			ClearState();
	void			ResetState( DWORD dwState );
	DWORD			GetMoveState() { return m_dwState & OBJSTA_MOVE_ALL; }
	DWORD			GetTurnState() { return m_dwState & OBJSTA_TURN_ALL; }
	DWORD			GetLookState() { return m_dwState & OBJSTA_LOOK_ALL; }
	DWORD			GetJumpState() { return m_dwState & OBJSTA_JUMP_ALL; }
	DWORD			GetDmgState() { return m_dwState & OBJSTA_DMG_ALL; }
	DWORD			GetActionState() { return m_dwState & OBJSTA_ACTION_ALL; }
	
	BOOL			IsState( DWORD dwState ) { return ( m_dwState & dwState ) ? TRUE : FALSE; }
	BOOL			IsStateFlag( DWORD dwStateFlag ) { return ( m_dwStateFlag & dwStateFlag ) ? TRUE : FALSE; }
	BOOL			IsFly() { return (m_dwStateFlag & OBJSTAF_FLY) ? TRUE : FALSE; }
	BOOL			IsDie() { return (m_dwState & OBJSTA_DIE_ALL) ? TRUE : FALSE; }
	BOOL			IsSit() { return (m_dwStateFlag & OBJSTAF_SIT) ? TRUE : FALSE; }
	BOOL			IsRun() { return ((m_dwStateFlag & OBJSTAF_WALK) == 0) ? TRUE : FALSE; }
	BOOL			IsWalk() { return (m_dwStateFlag & OBJSTAF_WALK) ? TRUE : FALSE; }
	BOOL			IsAction() { return (m_dwState & OBJSTA_ACTION_ALL) ? TRUE : FALSE; }
	BOOL			IsMove() { return ( GetMoveState() == OBJSTA_FMOVE || GetMoveState() == OBJSTA_BMOVE || GetMoveState() == OBJSTA_LEFT || GetMoveState() == OBJSTA_RIGHT ) ? TRUE : FALSE; }  
	
	BOOL			IsActJump()		{ return m_dwState & OBJSTA_JUMP_ALL; }
	BOOL			IsActAttack()		{ return m_dwState & OBJSTA_ATK_ALL; }
	BOOL			IsActDamage()		{ return m_dwState & OBJSTA_DMG_ALL; }
	BOOL			IsActTurn()		{ return m_dwState & OBJSTA_TURN_ALL; }
	BOOL			IsActLook() 		{ return m_dwState & OBJSTA_LOOK_ALL; }

	// event handler
	void			OnEndMoveState();
	void			OnEndTurnState();
	void			OnEndLookState();
	void			OnEndJumpState( DWORD dwState );
	void			OnEndAttackState( DWORD dwState );
	void			OnEndDamageState();
	void			OnEndActionState();
		
	virtual	int		SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0 )	{	return( 1 );	}
	virtual	void	ProcessState( DWORD dwState, int nParam ) { }
	virtual	BOOL	Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorld = NULL ) { return TRUE; }

private:
	BOOL			IsAtkMsgEmpty()	{	return( m_qMeleeAtkMsg.IsEmpty() && m_qMagicAtkMsg.IsEmpty() );	}
	void			ProcessAtkMsg();
};

#endif

