#include "stdafx.h"
#include "Sfx.h"

#include "defineObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif	// __DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSfx::CSfx()
{
	m_dwType = OT_SFX;
	m_idSrc = NULL_ID;
	m_idDest = NULL_ID;
	m_vPosDest = D3DXVECTOR3( 0, 0, 0 );
	m_nFrame = 0;
	m_nSec = -1;
	m_pSfxObj = NULL;
}
CSfx::CSfx( int nIndex, const OBJID idSrc, const D3DXVECTOR3& vPosSrc, const OBJID idDest, const D3DXVECTOR3& vPosDest )
{
//	SetSfx( nIndex, idSrc, vPosSrc, idDest, vPosDest );
	m_pSfxObj = NULL;
}
CSfx::~CSfx()
{
}

BOOL CSfx::SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp )
{
	BOOL bResult = FALSE;
	m_idSrc = NULL_ID;
	m_idDest = NULL_ID;
	m_vPosDest = D3DXVECTOR3( 0, 0, 0);

	if( dwIndex >= 0 ) 
	{
#ifndef __WORLDSERVER
		bResult = SetTypeIndex( pd3dDevice, OT_SFX, dwIndex, bInitProp );
		m_pSfxObj = (CSfxModel*)m_pModel;
#endif	// __WORLDSERVER
	}
	UpdateLocalMatrix();
	return bResult;
}

int	CSfx::SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, 
				   D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	m_idSrc = idSrc;
	m_idDest = idDest;
	m_vPosDest = vPosDest;
	m_nSec = nSec;

	vPosSrc.y += 0.01f; // 지면에서 약간 띄워주자. 안그러면 밑에 뭍혀서 지글거림.
	SetPos( vPosSrc );
	
	if( nIndex >= 0 ) 
	{
#ifndef __WORLDSERVER
		SetTypeIndex( pd3dDevice, OT_SFX, nIndex,TRUE);
		m_pSfxObj = (CSfxModel*)m_pModel;
#endif	// __WORLDSERVER
		return 1;
	}
	UpdateLocalMatrix();

	if( m_pSfxObj )
		return 1;

	return 0;
}

//
// nDmgCnt : 일반적으론 0 : 지속데미지를 사용할경우에 0이 아닌값이 들어온다.
//
void CSfx::DamageToTarget( int nDmgCnt, float fDmgAngle, float fDmgPower, int nMaxDmgCnt )
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	CCtrl* pObjDest = prj.GetCtrl( m_idDest );
	
	if( IsInvalidObj(pObjSrc) )		return;		// 지금은 걍 리턴하지만 이렇게 실패한경우는 m_idSfxHit을 Clear해주는작업이 필요하다.
	if(	IsInvalidObj(pObjDest) )	return;

	if( pObjDest->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*) pObjDest;

	}
}
void CSfx::Process( FLOAT fElapsedTime )
{
#ifdef _DEBUG
	if( m_dwIndex == XI_SKILL_RIN_SUP_MERKABAHANZELRUSHA03 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	m_nFrame++;
#ifdef __BEAST
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
#else
	if( m_nSec == 0 )	// 0은 1회 플레이후 종료.
	{
		if( m_pSfxObj->Process() )		// return true는 애니메이션 끝.
			Delete();
	}
	else
	{
		// 반복 애니메이션
		if( m_pSfxObj->Process() )		
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )		// 무한반복(-1)이 아니면
		{
			if( m_nFrame > ( m_nSec * 60 ) )	// 시간 체크를 한다.
				Delete();
		}
	}
#endif
	if( m_idDest != NULL_ID )		// Dest가 지정되어 있을때.
	{
		CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
		if( IsValidObj( pObjDest ) )		// 유효한넘인가?
			m_vPosDest = pObjDest->GetPos();	// 당시 좌표를 계속 받아둠.  Invalid상태가 되면 마지막 좌표로 세팅된다.
		
		SetPos( m_vPosDest );	// 타겟에 오브젝트 발동.
	} else
	{
		// Dest가 지정되어 있지 않을때. Src로...
		if( m_idSrc != NULL_ID )
		{
			CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
			if( IsValidObj( pObjSrc ) )			// 소스아이디가 지정되어 있으면
				SetPos( pObjSrc->GetPos() );	// 소스측에 이펙 발동.
			else
				Delete();
		}
	}
}
#ifndef __WORLDSERVER
// y축으로만 회전도는 버전.
void CSfx::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vPos.y += 0.2f;		// 0.2씩이나 올릴필요가 있남?
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
	
}
#endif // not worldserver

