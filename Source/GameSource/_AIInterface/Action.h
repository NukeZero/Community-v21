#ifndef __ACTION_H
#define __ACTION_H

#include <MoverMsg.h>

/////////////////////////////////////////////////////////////////////////////
//
//  Mover action msg / state

#define	MAX_ACTMSG	0xff

typedef struct tagACTMSG
{
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	int		nParam3;

	tagACTMSG() : dwMsg( 0 ), nParam1( 0 ), nParam2( 0 ), nParam3( 0 )	{}
	tagACTMSG( DWORD dw, int n1, int n2, int n3 ) : dwMsg( dw ), nParam1( n1 ), nParam2( n2 ), nParam3( n3 )	{}

}	ACTMSG, *LPACTMSG;

template <class T>
class CActMsgq
{
private:

	std::deque<T*>	m_qpam;

public:

	CActMsgq();
	virtual ~CActMsgq();

public:

	T*		GetHead();
	void	RemoveHead();

	void	AddTail( T * ptr );

	BOOL	IsEmpty();
	void	Empty();
};

//--------------------------------------------------------------------------------
template <class T>
CActMsgq<T>::CActMsgq()
{
}

template <class T>
CActMsgq<T>::~CActMsgq()
{
	for(std::deque<T*>::iterator i = m_qpam.begin(); i != m_qpam.end(); ++i )
	{
		SAFE_DELETE( *i );
	}

	m_qpam.clear();
}

template <class T>
inline BOOL CActMsgq<T>::IsEmpty()
{
	return m_qpam.empty();
}

template <class T>
inline void CActMsgq<T>::Empty()
{
	m_qpam.clear();
}

template <class T>
void CActMsgq<T>::RemoveHead()
{
	if( !IsEmpty() )
	{
		SAFE_DELETE( m_qpam.front() );
		m_qpam.pop_front();
	}
}

template <class T>
void CActMsgq<T>::AddTail( T* ptr )
{
	if( m_qpam.size() >= MAX_ACTMSG )
	{
		SAFE_DELETE( ptr );
		return;
	}

	m_qpam.push_back( ptr );
}

template <class T>
T* CActMsgq<T>::GetHead()
{
	return( IsEmpty() ? NULL: m_qpam.front() );
}
//--------------------------------------------------------------------------------

class CAction;
class CMeleeAtkMsgq : public CActMsgq<ACTMSG>
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

	tagMAGICATKMSG() : ACTMSG( 0, 0, 0, 0 ), nMagicPower( 0 ), idSfxHit( 0 )	{}
	tagMAGICATKMSG( DWORD dw, int n1, int n2, int n3, int p, int s ) : ACTMSG( dw, n1, n2, n3 ), nMagicPower( p ), idSfxHit( s )	{}

}	MAGICATKMSG, *LPMAGICATKMSG;


class CMover;
class CMagicAtkMsgq : public CActMsgq<MAGICATKMSG>
{
public:
	CMagicAtkMsgq()	{}
	virtual	~CMagicAtkMsgq()	{}
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

public:

	void			SetMover( CMover* pMover ) { m_pMover = pMover; }
	CMover*			GetMover() { return m_pMover; }
	CWorld*			GetWorld();

	void			Init();

	void			__ForceSetState( DWORD dwState );		// dwState값으로 강제로 세팅. 서버와 동기화때 외엔 사용하지 말것.
	void			ClearStateFlag() { m_dwStateFlag = 0; }

	void			AddStateFlag( DWORD dwFlag );
	void			RemoveStateFlag( DWORD dwFlag );

	DWORD			GetStateFlag() { return m_dwStateFlag; }

	// 동작 설정/참조
	void			SetState( DWORD dwType, DWORD dwState );
	void			SetMoveState( DWORD dwState );
	void			SetTurnState( DWORD dwState ) { ResetState(OBJSTA_TURN_ALL);	m_dwState |= dwState; }
	void			SetLookState( DWORD dwState ) { ResetState(OBJSTA_LOOK_ALL);	m_dwState |= dwState; }
	void			SetJumpState( DWORD dwState ) { ResetState(OBJSTA_JUMP_ALL);	m_dwState |= dwState; }
	void			SetActionState( DWORD dwState ) { ResetState(OBJSTA_ACTION_ALL);	m_dwState |= dwState; }

	void			ClearState();
	void			ResetState( DWORD dwState );

	DWORD			GetState() { return m_dwState; }
	DWORD			GetMoveState() { return m_dwState & OBJSTA_MOVE_ALL; }
	DWORD			GetTurnState() { return m_dwState & OBJSTA_TURN_ALL; }
	DWORD			GetLookState() { return m_dwState & OBJSTA_LOOK_ALL; }
	DWORD			GetJumpState() { return m_dwState & OBJSTA_JUMP_ALL; }
	DWORD			GetDmgState() { return m_dwState & OBJSTA_DMG_ALL; }
	DWORD			GetActionState() { return m_dwState & OBJSTA_ACTION_ALL; }
	
	BOOL			IsState( DWORD dwState ) { return ( m_dwState & dwState ) ? TRUE : FALSE; }
	BOOL			IsStateFlag( DWORD dwStateFlag ) { return ( m_dwStateFlag & dwStateFlag ) ? TRUE : FALSE; }
	BOOL			IsFly() { return (m_dwStateFlag & OBJSTAF_FLY) ? TRUE : FALSE; }
	BOOL			IsDie() const { return (m_dwState & OBJSTA_DIE_ALL) ? TRUE : FALSE; }
	BOOL			IsSit() { return (m_dwStateFlag & OBJSTAF_SIT) ? TRUE : FALSE; }
	BOOL			IsRun() { return ((m_dwStateFlag & OBJSTAF_WALK) == 0) ? TRUE : FALSE; }
	BOOL			IsWalk() { return (m_dwStateFlag & OBJSTAF_WALK) ? TRUE : FALSE; }
	BOOL			IsAction() { return (m_dwState & OBJSTA_ACTION_ALL) ? TRUE : FALSE; }
#ifdef __Y_INTERFACE_VER3
	BOOL			IsMove() { return ( GetMoveState() == OBJSTA_FMOVE || GetMoveState() == OBJSTA_BMOVE || GetMoveState() == OBJSTA_LEFT || GetMoveState() == OBJSTA_RIGHT || GetMoveState() == OBJSTA_LMOVE || GetMoveState() == OBJSTA_RMOVE ) ? TRUE : FALSE; }  
#else //__Y_INTERFACE_VER3
#ifdef KEYBOARD_SET
	BOOL			IsMove() { return ( GetMoveState() == OBJSTA_FMOVE || GetMoveState() == OBJSTA_BMOVE || GetMoveState() == OBJSTA_LEFT || GetMoveState() == OBJSTA_RIGHT || GetMoveState() == OBJSTA_LMOVE || GetMoveState() == OBJSTA_RMOVE ) ? TRUE : FALSE; }  
#else
	BOOL			IsMove() { return ( GetMoveState() == OBJSTA_FMOVE || GetMoveState() == OBJSTA_BMOVE || GetMoveState() == OBJSTA_LEFT || GetMoveState() == OBJSTA_RIGHT ) ? TRUE : FALSE; }  	
#endif	// KEYBOARD_SET
#endif //__Y_INTERFACE_VER3
	
	BOOL			IsActJump()		{ return m_dwState & OBJSTA_JUMP_ALL; }
	BOOL			IsActAttack()		{ return m_dwState & OBJSTA_ATK_ALL; }
	BOOL			IsActDamage()		{ return m_dwState & OBJSTA_DMG_ALL; }
	BOOL			IsActTurn()		{ return m_dwState & OBJSTA_TURN_ALL; }
	BOOL			IsActLook() 		{ return m_dwState & OBJSTA_LOOK_ALL; }

	bool			isCorrectState( const DWORD state ) const		{	return ( m_dwState & state ) == state;	}

	// event handler
	void			OnEndMoveState();
	void			OnEndTurnState();
	void			OnEndLookState();
	void			OnEndJumpState( DWORD dwState );
	void			OnEndAttackState( DWORD dwState );
	void			OnEndDamageState();
	void			OnEndActionState();
		
	virtual	int		SendActMsg( OBJMSG /*dwMsg*/, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0, int nParam5 = 0 )	
	{	
		UNREFERENCED_PARAMETER( nParam1 );  UNREFERENCED_PARAMETER( nParam2 ); UNREFERENCED_PARAMETER( nParam3 ); UNREFERENCED_PARAMETER( nParam4 ); UNREFERENCED_PARAMETER( nParam5 );
		return( 1 );	
	}
	virtual	void	ProcessState( DWORD /*dwState*/, float /*fSpeed*/ ) { }
	virtual	BOOL	Render( LPDIRECT3DDEVICE9 /*pd3dDevice*/, D3DXMATRIX* pmatWorld = NULL ) {	UNREFERENCED_PARAMETER( pmatWorld );			return TRUE; }

private:

	BOOL			IsAtkMsgEmpty()	{	return( m_qMeleeAtkMsg.IsEmpty() && m_qMagicAtkMsg.IsEmpty() );	}
	void			ProcessAtkMsg();

};

#endif

