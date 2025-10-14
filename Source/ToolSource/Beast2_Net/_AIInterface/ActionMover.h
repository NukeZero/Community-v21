#ifndef __ACTIONMOVER_H
#define __ACTIONMOVER_H


#include "Mover.h"
#include "Action.h"
#include "MoverMsg.h"
#ifdef __CLIENT
#include "TailEffectMng.h"
#endif
#include "mempooler.h"

#ifdef __CLIENT
extern CDamageNumMng g_DamageNumMng;
#endif	// __CLIENT

#define MAX_MOVE_HEIGHT		270.0f
#define MIN_MOVE_HEIGHT		85.0f

#define	CTRLMSG_FORWARD		0x00000001
#define	CTRLMSG_BACKWARD	0x00000002
#define	CTRLMSG_LEFT		0x00000004
#define	CTRLMSG_RIGHT		0x00000008
#define	CTRLMSG_LDOWN		0x00000010
#define	CTRLMSG_RDOWN		0x00000020

#define	AF_GENERIC			(DWORD)0x00000001
#define	AF_MISS				(DWORD)0x00000002
#define	AF_RESERVED			(DWORD)0x00000004
#define	AF_MAGIC			(DWORD)0x00000008
#define AF_MELEESKILL		(DWORD)0x00000010
#define AF_MAGICSKILL		(DWORD)0x00000020
#define AF_CRITICAL1		(DWORD)0x00000040	// 2.3배 크리티컬
#define AF_CRITICAL2		(DWORD)0x00000080	// 2.6배 크리티컬
#define AF_CRITICAL			(DWORD)0x000000c0	// 위에 두개 합한거.
#define AF_PUSH				(DWORD)0x00000100	// 맞으면 밀림.
#define AF_PARRY			(DWORD)0x00000200	// 회피...
#define AF_RESIST			(DWORD)0x00000400	// 회피...
#define AF_STUN				(DWORD)0x00000800	// 스턴 - 기절
#define AF_BLOCKING			(DWORD)0x00001000	// 블록킹 
#define AF_FORCE			(DWORD)0x00002000	// 계산된 데미지를 줄 경우 사용 (예: 반사데미지, 사이킥 월) 
#define AF_RANGE			(DWORD)0x00004000	// 장거리 공격 데미지 
#define AF_FLYING			(DWORD)0x10000000	// 강공격으로 날아감.

// 스킬류인가?
inline BOOL IsSkillAttack( DWORD dwAtkFlags )
{
	return (dwAtkFlags & (AF_MELEESKILL|AF_MAGICSKILL) );
}


#	ifdef __WORLDSERVER
#		define	MAX_CORR_SIZE_45	5
#		define	MAX_CORR_SIZE_150	9
#	else	// __WORLDSERVER
#		define	MAX_CORR_SIZE_45	22
#		define	MAX_CORR_SIZE_150	37
#	endif	// __WORLDSERVER


/////////////////////////////////////////////////////////////////////
// CActionMover 
/////////////////////////////////////////////////////////////////////
class CActionMover;
struct DAMAGE_MSG;

typedef	MemPooler<CActionMover>	CActionMoverPool;

class CActionMover : public CAction
{
public:
	CActionMover();
	CActionMover( CMover* pMover );
	virtual	~CActionMover();

private:
	OBJID			m_objidHit;
	int				m_nCastingTime;
	int				m_bMove;		// 이번턴에 움직인적 있는가?

#ifdef __CLIENT
  #ifdef __YTAIL_EFFECT_EX
	CTailEffectBelt*	m_pTail;		// 보드탔을때 꼬랑지 이펙트
  #else //__YTAIL_EFFECT_EX
	CTailEffect*		m_pTail;		// 보드탔을때 꼬랑지 이펙트
  #endif //__YTAIL_EFFECT_EX
#endif

public:
	int				m_nCount;
	DWORD			m_idTarget;		// 타겟의 아이디(다용도 타겟)
	float			m_fAccPower;	// 오브젝트에 가해지는 힘
	FLOAT			m_fTurnAngle;
	int				m_nMotionEx;		
	float			m_fAccAng;		
	float			m_fSpeed;		// 뛰기 스피드
	D3DXVECTOR3		m_vDelta;
	D3DXVECTOR3		m_vDeltaE;		// 외부에서 가해지는 힘 m_vDelta와 같이 쓰는게 정석이지만 우리시스템은 걸을때 마찰력이라는 개념이 없어서 따로 준비했다.
	D3DXVECTOR3		m_vDeltaAccu;	// delta값 누적
	D3DXVECTOR3		m_vPosLast;		// 이번 좌표값
	int				m_bGround;		// 바닥에 닿아있다.
	float			m_fCurrentHeight;	// 현재 서있는곳의 Height좌표
	float			m_fDistance;
	DWORD			m_dwAtkFlags;
	int				m_nShootCnt;	// 발사체 개수.  ??프레임에 한번씩 발사된다.
	
#ifdef __YMOTIONHITCOUNT
	int				m_nMotionHitCount;
#endif //__YHITCOUNT

#ifdef __CLIENT
	DWORD			m_dwCtrlMsg;	// 컨트롤 메시지
#endif
	
#ifdef __WORLDSERVER
		int			m_nDeadCnt;		
#endif

public:
	static	BOOL	IsObjMsgType( DWORD dwMsg, OBJMSGTYPE nType );

	virtual int		SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0 );
	virtual	void	ProcessState( DWORD dwState, int nParam = 0 );	
	virtual	BOOL	Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorld = NULL );	// 특별한 출력을 원하신다면 저를 애용해주세요.

	void			ForcedSet( D3DXVECTOR3 &vDelta, DWORD dwState, DWORD dwStateFlag, int nMotionEx );
	void			ForcedSet2( D3DXVECTOR3 &vDelta, float fAccPower, float fTurnAngle, DWORD dwState, DWORD dwStateFlag, int nMotionEx );
	void			DefaultSet();
	void			HalfForcedSet( D3DXVECTOR3 & vd, float fAccPower, float fTurnAngle );

	void			DoDamageFly( float fAngleXZ, float fAngleY, float fPower );		
	void			SetExternalForce( const D3DXVECTOR3 &vForce ) { m_vDeltaE = vForce; }		// 외부에서 가해지는 힘.
	int				MakeParamDamage( int nMagicPower, int nSkill, int nDmgCnt = 0 );
	int				SendDamage( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE  )	{	return SendActMsg( OBJMSG_DAMAGE, dwAtkFlags, idSender, nParam, bTarget );	}
	int				SendDamageForce( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE ) { return SendActMsg( OBJMSG_DAMAGE_FORCE, dwAtkFlags, idSender, nParam, bTarget ); }	// 강공격

	void			ControlGround();

	// 언더라인(_) 붙은함수는 XuZhu이외엔 직접 Call하지 말것
	DWORD			_ProcessMsgDmg( DWORD dwMsg, CMover* pSender, DWORD dwAtkFlags, int nParam = 0, BOOL bTarget = TRUE );
	void			_ProcessStateAttack( DWORD dwState, int nParam = 0 );	// 공격부분은 코드가 길어서 따로 빼냄
	void			_ProcessStateAttack2( DWORD dwState, int nParam = 0 );

	void			ProcessStateAttackSP( DWORD dwState, int nParam = 0 );
	void			ProcessStateAttackMagic( DWORD dwState, int nParam = 0 );		// 완드공격
	void			ProcessStateAttackRange( DWORD dwState, int nParam = 0 );		// 레인지공격
	void			ProcessStateMeleeSkill( DWORD dwState, int nParam );	// 스킬동작 처리
	void			ProcessStateMagicSkill( DWORD dwState, int nParam );	// 마법스킬동작 처리
	void			ProcessStateMagicCasting( DWORD dwState, int nParam );
	void			ProcessPreAction();
	int				ProcessAction( D3DXVECTOR3 *vPos, int nMsg );
	int				ProcessActMsg1( CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 );
	int				ProcessActMsg2( CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nparam3, int nParam4 );
	void			ProcessState1( CMover* pMover, DWORD dwState, int nParam );
	void			ProcessState2( CMover* pMover, DWORD dwState, int nParam );
	void			ProcessFlyMove();
	BOOL			ProcessCollisionFly( D3DXVECTOR3 *vPos );
	BOOL			ProcessCollisionGround( D3DXVECTOR3 *vPos );
	BOOL			ProcessCollision( D3DXVECTOR3 *vPos );


	void			SetObjHit( OBJID idHit ) {	m_objidHit = idHit;	}
	OBJID			GetObjHit() {	return m_objidHit; }	
	
#ifdef __CLIENT
	void			PresupposePos( D3DXVECTOR3* pv, D3DXVECTOR3* pvd, float* pf, u_long uTickCount );
	void			PresupposePos2( D3DXVECTOR3* pv, D3DXVECTOR3* pvd, float* pf, float* pfAngleX, float* pfAccPower, u_long uTickCount );
	DWORD			OnDamageMsgC( DWORD dwMsg, CMover* pAttacker, DWORD dwAtkFlags, int nParam );
#endif	// __CLIENT

	static			CActionMoverPool*	m_pPool;
	void*			operator new( size_t nSize )	{	return CActionMover::m_pPool->Alloc();	}
	void*			operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CActionMover::m_pPool->Alloc();	}
	void			operator delete( void* lpMem )	{	CActionMover::m_pPool->Free( (CActionMover*)lpMem );	}
	void			operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CActionMover::m_pPool->Free( (CActionMover*)lpMem );	}

protected:
	void			Init();
	void			Destroy();
};

inline int CActionMover::MakeParamDamage( int nMagicPower, int nSkill, int nDmgCnt)
{
	int nParam = nMagicPower;	// D0~D7
	nParam |= (nDmgCnt << 8);	// D7~D15
	nParam |= (nSkill << 16);	// D16~D31
	return nParam;
}

inline BOOL CActionMover::IsObjMsgType( DWORD dwMsg, OBJMSGTYPE nType )
{
	switch( nType )
	{
		case OMT_PRE_PROCESS:
			{
				switch( dwMsg )
				{
					case OBJMSG_FORWARD:
					case OBJMSG_BACKWARD:
					case OBJMSG_LTURN:
					case OBJMSG_RTURN:
					case OBJMSG_JUMP:
					case OBJMSG_LOOKUP:
					case OBJMSG_LOOKDOWN:
					case OBJMSG_STAND:
					case OBJMSG_STOP:
					case OBJMSG_ASTOP:
					case OBJMSG_STOP_TURN:
					case OBJMSG_STOP_LOOK:
					case OBJMSG_MODE_WALK:
					case OBJMSG_MODE_RUN:
					case OBJMSG_TURNMOVE:
					case OBJMSG_MODE_COMBAT:
					case OBJMSG_MODE_PEACE:
					case OBJMSG_MODE_FLY:
					case OBJMSG_MODE_GROUND:
					case OBJMSG_ACC_START:
					case OBJMSG_ACC_STOP:
						return TRUE;
				}
				break;
			}
		case OMT_PRE_TRANSFER:
			{
				switch( dwMsg )
				{
					case OBJMSG_ATK1:
					case OBJMSG_ATK2:
					case OBJMSG_ATK3:
					case OBJMSG_ATK4:
						return TRUE;
				}
				break;
			}
		case OMT_DEPENDENT:
			{
				switch( dwMsg )
				{
					case OBJMSG_STAND:
					case OBJMSG_STOP:
					case OBJMSG_ASTOP:
					case OBJMSG_STOP_TURN:
					case OBJMSG_MOTION:
						return TRUE;
				}
				break;
			}
		default:
			break;
	}
	return FALSE;
}

#endif	// __ACTIONMOVER_H