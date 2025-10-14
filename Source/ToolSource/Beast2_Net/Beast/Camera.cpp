#include "stdafx.h"

#include "Ship.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

D3DXMATRIX g_matView;
D3DXMATRIX g_matInvView;

CCamera::CCamera()
{
	m_vPos    = D3DXVECTOR3( 50.0f, 110.0f, 50.0f);
	m_vLookAt = D3DXVECTOR3( m_vPos.x + 1.0f, 100.0f, m_vPos.z + 1.0f);
	m_vOffset = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_nQuakeSec = 0;
	m_fQuakeSize = 0.06f;
	m_dwCamStyle = 0;
}
void CCamera::Process( LPDIRECT3DDEVICE9 pd3dDevice ,float fFactor )
{
}
void CCamera::Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld )
{
	D3DXVECTOR3	vTempPos = m_vPos;
	if( m_nQuakeSec > 0 )
	{
//		float x = (float)(8.0f - xRandom(16)) / 256.0f;
//		float y = (float)(8.0f - xRandom(16)) / 256.0f;
//		float z = (float)(8.0f - xRandom(16)) / 256.0f;

		float x = (m_fQuakeSize / 2.0f) - xRandomF( m_fQuakeSize );
		float y = (m_fQuakeSize / 2.0f) - xRandomF( m_fQuakeSize );
		float z = (m_fQuakeSize / 2.0f) - xRandomF( m_fQuakeSize );
		
		vTempPos.x += x;
		vTempPos.y += y;
		vTempPos.z += z;
		--m_nQuakeSec;
	}
	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1.0f, 0 );
	D3DXMatrixLookAtLH( &m_matView, &vTempPos, &m_vLookAt, &vUp );

	pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	pWorld->UpdateCullInfo( &m_matView, &pWorld->m_matProj );
	// 거대한 오브젝트의 컬링을 위해 카메라를 실제보다 뒤로 조금 뺀다.
	/*
	D3DXVECTOR3 vTempLook = m_vLookAt;
	D3DXVec3Normalize(&(vTempLook),&(vTempLook));
	vTempPos -= ( vTempLook * 3);
	D3DXMATRIX mTempView;
	D3DXMatrixLookAtLH( &mTempView, &vTempPos, &m_vLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pWorld->UpdateCullInfo( &mTempView, &pWorld->m_matProj );
	*/
	// 현재 카메라의 역함수를 구한다. 이것은 여기저기서 다목적으로 사용한다.
	D3DXMatrixInverse( &m_matInvView, NULL, &m_matView );
	m_matInvView._41 = 0.0f; m_matInvView._42 = 0.0f; m_matInvView._43 = 0.0f;
	g_matInvView = m_matInvView;
	g_matView = m_matView;
}
void CCamera::SetQuake( int nSec, float fSize )
{
	if( m_nQuakeSec <= 0 )
	{
		m_nQuakeSec = nSec;
		m_fQuakeSize = fSize;
	}
}
void CCamera::Reset() 
{ 
	//m_vPos2 = m_vPos; 
}
void CCamera::ControlCamera( DWORD dwStyle )
{
	m_dwCamStyle = dwStyle;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void CBackCamera::Lock()
{
}
void CBackCamera::Unlock()
{
	m_bLock = FALSE;
	m_fRotx = 0;
	m_fRoty = 0;
}

CBackCamera::CBackCamera()
{
	m_fZoom = 0;
	m_fZoom = 6.0f;
	m_bLock = FALSE;
	m_fRoty = -12.0f;

#ifdef __YCAMERA_EX	
	m_fLength1 = 0.0f;
	m_fLength2 = 0.0f;
	m_bOld     = FALSE;
	m_bStart   = FALSE;
#endif //__YCAMERA_EX
	
}
D3DXVECTOR3 m_vPosVal; 
void CBackCamera::ControlCamera( DWORD dwStyle )
{
	return;
	CCamera::ControlCamera( dwStyle );
	D3DXVECTOR3 vPos = CMover::GetActiveMover()->GetPos();
	D3DXVECTOR3 vTemp = vPos - m_vPos;

	switch( dwStyle )
	{
	case CAMSTY_HIT:
		m_vPosVal = vTemp;
		break;
		
	case CAMSTY_DAMAGE:
		m_vOffset += vTemp;
		break;
	}
}
void CBackCamera::Process( LPDIRECT3DDEVICE9 pd3dDevice ,float fFactor )
{

}
CToolCamera::CToolCamera()
{
	m_vPos = D3DXVECTOR3( 0.0f, 110.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3( 0,0,0);
	m_fYaw = 0;	
	m_fYawVelocity = 0;
	m_fPitch = 0;
	m_fPitchVelocity = 0;
	//m_matOrientation = 0;
}
void CToolCamera::Process( LPDIRECT3DDEVICE9 pd3dDevice, float fFactor )
{
}
void CToolCamera::Transform( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld )
{
    D3DXQUATERNION qR;
    D3DXQuaternionRotationYawPitchRoll( &qR, m_fYaw, m_fPitch, 0.0f );
    //D3DXMatrixAffineTransformation( &m_matOrientation, 1.25f, NULL, &qR, &m_vPos );
	D3DXMatrixAffineTransformation( &m_matOrientation, 1.0f, NULL, &qR, &m_vPos );
    D3DXMatrixInverse( &m_matView, NULL, &m_matOrientation );
    pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	pWorld->UpdateCullInfo( &m_matView, &pWorld->m_matProj );
	D3DXMatrixInverse( &m_matInvView, NULL, &m_matView );
	m_matInvView._41 = 0.0f; m_matInvView._42 = 0.0f; m_matInvView._43 = 0.0f;
	g_matInvView = m_matInvView;
	g_matView = m_matView;
	
}
