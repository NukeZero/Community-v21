#include "stdafx.h"
#include "defineObj.h"
#include "defineskill.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"
#include "Sfx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define		P_ACC		0.01f
//#define		P_MAXACC	0.004f

//
//	State process
//	가급적 이곳에선 pMover->SetMotion을 하지 말것
//
void	CActionMover::ProcessState1( CMover* pMover,  DWORD dwState, int nParam )
{

}

//
//	전사들의 근접 전투스킬류....
//

void	CActionMover::ProcessStateMeleeSkill( DWORD dwState, int nParam )
{

}

//
//	마법사들의 마법공격동작 중....
//


// 신버전. - 마법 발사동작중.
void	CActionMover::ProcessStateMagicSkill( DWORD dwState, int nParam )
{

}

//
//		공격 액션 처리
//
void	CActionMover::_ProcessStateAttack( DWORD dwState, int nParam )
{

}

//
//		특수공격동작중.
//
void	CActionMover::ProcessStateAttackSP( DWORD dwState, int nParam )
{

}


//
//		완드공격동작
//
void	CActionMover::ProcessStateAttackMagic( DWORD dwState, int nParam )
{

}


//
//		원거리공격동작
//
void	CActionMover::ProcessStateAttackRange( DWORD dwState, int nParam )
{

}

/*
ContinueAttack( )
{
	ResetState( OBJSTA_ATK_ALL );

	if( IsNPC() )
		return;


}
*/