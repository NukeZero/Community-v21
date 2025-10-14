
#include <stdAfx.h>
#include "camera.h"


Camera::Camera()
{
	_cameraType	 = AIRCRAFT;

	_pos		 = D3DXVECTOR3(0.0f, 1.0f, -10.0f);
	_right		 = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up			 = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look		 = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vTargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_fZoom = 0;
	m_fZoom = 6.0f;
	m_bLock = FALSE;
	m_fRoty = -12.0f;
    
	m_fLength1 = 0.0f;
	m_fLength2 = 0.0f;
	m_bOld     = FALSE;
	m_bStart   = FALSE;
	m_fCameraPitchAngle = m_fCameraYawAngle = 0.0f;
	m_fDistance = 0.0f;
	m_nMode				= MODE_WORLD_ROTATE;

}

Camera::Camera(CameraType cameraType)
{
	_cameraType = cameraType;
	m_nMode		= MODE_WORLD_ROTATE;
	m_fDistance = 0.0f;
	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

Camera::~Camera()
{

}

void Camera::getPosition(D3DXVECTOR3* pos)
{
	*pos = _pos;
}

void Camera::setPosition(D3DXVECTOR3* pos)
{
	_pos = *pos;
}

void Camera::getRight(D3DXVECTOR3* right)
{
	*right = _right;
}

void Camera::getUp(D3DXVECTOR3* up)
{
	*up = _up;
}

void Camera::getLook(D3DXVECTOR3* look)
{
	*look = _look;
}

void Camera::walk(float units)
{
	if(m_nMode == MODE_WORLD_ROTATE)
	{
		// move only on xz plane for land object
		if( _cameraType == LANDCRAFT )
			_pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;

		if( _cameraType == AIRCRAFT )
			_pos += _look * units;

		if(_pos.y < 1) _pos.y = 1;
	}
}

void Camera::strafe(float units)
{
	if(m_nMode == MODE_WORLD_ROTATE)
	{
		// move only on xz plane for land object
		if( _cameraType == LANDCRAFT )
			_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;

		if( _cameraType == AIRCRAFT )
			_pos += _right * units;
	}
}

void Camera::fly(float units)
{
	// move only on y-axis for land object
	if( _cameraType == LANDCRAFT )
		_pos.y += units;

	if( _cameraType == AIRCRAFT )
		_pos += _up * units;
}

void Camera::pitch(float angle)
{
	
	if(m_nMode == MODE_WORLD_ROTATE)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_right,	angle);

		// rotate _up and _look around _right vector
		D3DXVec3TransformCoord(&_up,&_up, &T);
		D3DXVec3TransformCoord(&_look,&_look, &T);
	}	
}

void Camera::yaw(float angle)
{
	if(m_nMode == MODE_WORLD_ROTATE)
	{
		D3DXMATRIX T;
		// rotate around world y (0, 1, 0) always for land object
		D3DXMatrixRotationY(&T, angle);
		D3DXVec3TransformCoord(&_right,&_right, &T);
		D3DXVec3TransformCoord(&_look,&_look, &T);
	}
}

void Camera::roll(float angle)
{
	if(m_nMode == MODE_WORLD_ROTATE)
	{
		// only roll for aircraft type
		if( _cameraType == AIRCRAFT )
		{
			D3DXMATRIX T;
			D3DXMatrixRotationAxis(&T, &_look,	angle);

			// rotate _up and _right around _look vector
			D3DXVec3TransformCoord(&_right,&_right, &T);
			D3DXVec3TransformCoord(&_up,&_up, &T);
		}
	}
}

void Camera::SetTarget(D3DXVECTOR3 vPos)
{

	D3DXVECTOR3		vDistance = vPos - _pos;
	float			fDistance = D3DXVec3Length(&vDistance);


	m_vTargetPos = vPos;
	// 카메라와 대상의 거리 측정
	m_fDistance  = fDistance;
}


float Camera::AddDistance(float fAmount)
{

	m_fDistance += fAmount;

	return m_fDistance;

}


void Camera::Update()
{
	

	if(m_nMode == MODE_MODEL_ROTATE)
	{
		D3DXMATRIX		matView, matTemp1, matTemp2;
		D3DXVECTOR3		vDistance;
		D3DXVECTOR3		vLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		D3DXVECTOR3		vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		
		
		D3DXMatrixRotationY(&matTemp1,DEGREETORADIAN(m_fCameraYawAngle));
		D3DXMatrixRotationX(&matTemp2,DEGREETORADIAN(m_fCameraPitchAngle));
		matTemp1*=matTemp2;
		
		D3DXVec3TransformCoord( &_look, &vLook, &matTemp1 ); 
		D3DXVec3TransformCoord( &_up, &vUp, &matTemp1 ); 
		//D3DXVec3TransformCoord( &_right, &_right, &matTemp1 ); 
		//D3DXVec3Cross(&_right, &_up, &_look);
		//D3DXVec3Normalize(&_right, &_right);

		vLook = -_look;
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Scale(&vDistance, &vLook, m_fDistance);
		_pos = m_vTargetPos + vDistance;

		if(_pos.y < 1.0f) _pos.y = 1.0f;

		D3DXMatrixLookAtLH(&m_matView, &_pos, &m_vTargetPos, &vUp);

		_right.x = (m_matView)(0,0); _up.x = (m_matView)(0, 1); _look.x = (m_matView)(0, 2);
		_right.y = (m_matView)(1,0); _up.y = (m_matView)(1, 1); _look.y = (m_matView)(1, 2);
		_right.z = (m_matView)(2,0); _up.z = (m_matView)(2, 1); _look.z = (m_matView)(2, 2);
		//_pos.x   = (m_matView)(3,0); _pos.y = (m_matView)(3, 1); _pos.z   = (m_matView)(3, 2);
		
	}
	else
	{
		// Keep camera's axes orthogonal to eachother
		D3DXVec3Normalize(&_look, &_look);

		D3DXVec3Cross(&_up, &_look, &_right);
		D3DXVec3Normalize(&_up, &_up);

		D3DXVec3Cross(&_right, &_up, &_look);
		D3DXVec3Normalize(&_right, &_right);

		// Build the view matrix:
		float x = -D3DXVec3Dot(&_right, &_pos);
		float y = -D3DXVec3Dot(&_up, &_pos);
		float z = -D3DXVec3Dot(&_look, &_pos);

		(m_matView)(0,0) = _right.x; (m_matView)(0, 1) = _up.x; (m_matView)(0, 2) = _look.x; (m_matView)(0, 3) = 0.0f;
		(m_matView)(1,0) = _right.y; (m_matView)(1, 1) = _up.y; (m_matView)(1, 2) = _look.y; (m_matView)(1, 3) = 0.0f;
		(m_matView)(2,0) = _right.z; (m_matView)(2, 1) = _up.z; (m_matView)(2, 2) = _look.z; (m_matView)(2, 3) = 0.0f;
		(m_matView)(3,0) = x;        (m_matView)(3, 1) = y;     (m_matView)(3, 2) = z;       (m_matView)(3, 3) = 1.0f;
	}
	
/*
	D3DXMATRIX mCameraRot;
    D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0 );

    // Transform vectors based on camera's rotation matrix
    D3DXVECTOR3 vLocalUp    = D3DXVECTOR3(0,1,0);
    D3DXVECTOR3 vLocalAhead = D3DXVECTOR3(0,0,1);
    D3DXVec3TransformCoord( &_up, &vLocalUp, &mCameraRot );
    D3DXVec3TransformCoord( &_look, &vLocalAhead, &mCameraRot );

	m_vLookAt = _pos + _look;
	// Update the view matrix
    D3DXMatrixLookAtLH( &m_matView, &_pos, &m_vLookAt, &_up );*/
}

void Camera::getViewMatrix(D3DXMATRIX* V)
{
	/*
	// Keep camera's axes orthogonal to eachother
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
	*/
	 *V = m_matView;

}


void Camera::setCameraType(CameraType cameraType)
{
	_cameraType = cameraType;
}


void Camera::switchMode(int	nMode)
{

	// 카메라의 모드를 바꾼다
	m_nMode = nMode;

}


void Camera::Lock()
{
#ifdef __CLIENT

	m_bLock = TRUE;
	if( g_WorldMng.Get() )
	{
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
		if( !(pMoverTarget && CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover->IsFly()) ) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle(); 
			m_fRotx=-fAngle;
		}
	}
#endif
}
void Camera::Unlock()
{
	m_bLock = FALSE;
	m_fRotx = 0;
	m_fRoty = 0;
}

D3DXVECTOR3 m_vPosVal; 


int  Camera::GetAnglePie( FLOAT fAngle )
{
	FLOAT fAngTemp = fabs(fAngle);

	if( fAngTemp > 0.0f && fAngTemp <= 90.0f )
	{
		return 4;
	}
	
	if( fAngTemp > 90.0f && fAngTemp <= 180.0f )
	{
		return 3;
	}
	
	if( fAngTemp > 180.0f && fAngTemp <= 270.0f )
	{
		return 2;
	}	
	
	if( fAngTemp > 270.0f && fAngTemp <= 360.0f )
	{
		return 1;
	}

	return 0;
}

void Camera::Process( LPDIRECT3DDEVICE9 pd3dDevice ,float fFactor )
{

	/*
	D3DXMATRIX matView, mat;

	FLOAT fAngle = 0, fAngleY = 0;

	// 타겟 위치 
	D3DXVECTOR3 vPos = pMover->GetPos();

	vPos.y += 0.9f;

	CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
	D3DXVECTOR3 vTarget,vTemp;
	
	fAngle = pMover->GetAngle();
	if( m_bLock )
		fAngle = 0;
	fAngleY = 90.0f;
	m_fCurRoty = m_fRoty + m_fZoom * 4;
	
	m_vLookAt = vPos;

	m_fCurRotx = m_fRotx;

	
	float fAdjAng = (1.0f - fAngleY / 90.0f) * 45.0f;
	m_fCurRoty += fAdjAng;
	m_fCurRoty += pMover->GetAngleX();
	
	if( m_fCurRoty > 80.0f ) 
		m_fCurRoty = 80.0f;

	fAngle = m_fCurRotx - fAngle + 180.0f;

	D3DXMATRIX matTemp;
	// zoom 상태에 따라 카메라 위치를 조정
	extern float fDiv;
	
	if( fDiv == 2.0f )
		//vTemp = D3DXVECTOR3( 0.0f, 0.0f, -0.0f - (m_fZoom / 2.0f) * 2.0f );
		vTemp = D3DXVECTOR3( 0.0f, 0.0f, -0.0f - 2.0f );
	else
	{
		if( g_pShip )
			vTemp = D3DXVECTOR3( 0.0f, 0.0f, -4.0f - m_fZoom * 16.0f );
		else
			//vTemp = D3DXVECTOR3( 0.0f, 0.0f, -50.0f );
			vTemp = D3DXVECTOR3( 0.0f, 0.0f, -4.0f - m_fZoom * 2.0f );
	}
	
	D3DXVECTOR3 vecOut;
	D3DXMatrixRotationX( &matTemp, D3DXToRadian( m_fCurRoty / 1.0f ) );
	D3DXVec3TransformCoord( &vTemp, &vTemp, &matTemp );
	D3DXMatrixRotationY( &matTemp, D3DXToRadian( fAngle ) );
	D3DXVec3TransformCoord( &m_vOffsetDest, &vTemp, &matTemp );

	D3DXVECTOR3 vecOffsetDelta = ( ( m_vOffsetDest - m_vOffset ) + m_vPosVal ) / fFactor;
	m_vOffset += vecOffsetDelta;

	m_vPosVal /= 2;

	m_vPos = vPos + m_vOffset;

	BOOL  bCrash;
	FLOAT fLength;
	static D3DXVECTOR3 m_vLimitPos;
	D3DXVECTOR3 m_vOutPos = m_vPos;

	m_vLookAt.y += 0.4f;

	if( m_bStart )
	{
		D3DXVECTOR3 vCPos = vPos + m_vOffset;
		D3DXVECTOR3 vDir  = vCPos - m_vLookAt;
		D3DXVec3Normalize(&vDir, &vDir);
		
		m_fLength2 += 0.07f;

		if( m_fLength2 > fLength )
			m_bStart = FALSE;

		m_vOutPos = m_vLookAt + (vDir * m_fLength2);
	}

	m_bOld = bCrash;

	g_ModelGlobal.SetCamera( m_vOutPos, m_vLookAt );

	m_vPos = m_vOutPos;
*/
}


void Camera::Transform( LPDIRECT3DDEVICE9 pd3dDevice)
{
	D3DXVECTOR3	vTempPos = m_vPos;
	
	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1.0f, 0 );


	D3DXMatrixLookAtLH( &m_matView, &vTempPos, &m_vLookAt, &vUp );

	pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	//pWorld->UpdateCullInfo( &m_matView, &pWorld->m_matProj );
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