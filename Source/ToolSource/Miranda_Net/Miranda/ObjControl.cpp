#include "StdAfx.h"


CObjControl::CObjControl()
{
	m_pObjMt = NULL;
	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_fAngle = 0.0f;
	D3DXMatrixIdentity(&m_RotateMt);
}

CObjControl::~CObjControl()
{

}

void CObjControl::getPosition(D3DXVECTOR3* pos)
{
	*pos = _pos;
}

void CObjControl::setPosition(D3DXVECTOR3* pos)
{
	_pos = *pos;
}

void CObjControl::getRight(D3DXVECTOR3* right)
{
	*right = _right;
}

void CObjControl::getUp(D3DXVECTOR3* up)
{
	*up = _up;
}

void CObjControl::getLook(D3DXVECTOR3* look)
{
	*look = _look;
}

void CObjControl::walk(float units)
{

	//_pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;	
	_pos += D3DXVECTOR3(m_RotateMt._13, m_RotateMt._23, -m_RotateMt._33) * -units;	

}

void CObjControl::strafe(float units)
{

	//_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;
	_pos += D3DXVECTOR3(m_RotateMt._11, m_RotateMt._21, -m_RotateMt._31) * units;

}

void CObjControl::RotateY(float angle)
{
	//D3DXMATRIX T;
	//D3DXMatrixRotationY(&T, angle);
	
	m_fAngle -=  angle;
	D3DXMatrixRotationY(&m_RotateMt, m_fAngle);
//	D3DXMatrixRotationYawPitchRoll();
	// rotate _right and _look around _up or y-axis
	//D3DXVec3TransformCoord(&_right,&_right, &m_RotateMt);
	//D3DXVec3TransformCoord(&_look,&_look, &m_RotateMt);
}


void CObjControl::getObjMt(D3DXMATRIX* V)
{

	m_pObjMt   = V;
	m_RotateMt = *V;
	m_RotateMt._41 = 0.0f;
	m_RotateMt._42 = 0.0f;
	m_RotateMt._43 = 0.0f;
	m_fAngle = 0.0f;
	//_right.x = (*m_pObjMt)(0,0); _up.x = (*m_pObjMt)(0, 1); _look.x = (*m_pObjMt)(0, 2); 
	//_right.y = (*m_pObjMt)(1,0); _up.y = (*m_pObjMt)(1, 1); _look.y = (*m_pObjMt)(1, 2); 
	//_right.z = (*m_pObjMt)(2,0); _up.z = (*m_pObjMt)(2, 1); _look.z = (*m_pObjMt)(2, 2); 
	_pos.x = (*m_pObjMt)(3,0);
	_pos.y = (*m_pObjMt)(3,1);    
	_pos.z = (*m_pObjMt)(3,2);

}

void CObjControl::UpdateMatrix()
{
	_pos.x = (*m_pObjMt)(3,0);
	_pos.y = (*m_pObjMt)(3,1);    
	_pos.z = (*m_pObjMt)(3,2);
	
	 D3DXMatrixIdentity( m_pObjMt );
	(*m_pObjMt)(3,0) = 0.0f;        (*m_pObjMt)(3, 1) = 0.0f;     (*m_pObjMt)(3, 2) = 0.0f;
	 D3DXMatrixMultiply( m_pObjMt, m_pObjMt, &m_RotateMt );
	 (*m_pObjMt)(3,0) = _pos.x;        (*m_pObjMt)(3, 1) = _pos.y;     (*m_pObjMt)(3, 2) = _pos.z;

/*
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*m_pObjMt)(0,0) = _right.x; (*m_pObjMt)(0, 1) = _up.x; (*m_pObjMt)(0, 2) = _look.x; (*m_pObjMt)(0, 3) = 0.0f;
	(*m_pObjMt)(1,0) = _right.y; (*m_pObjMt)(1, 1) = _up.y; (*m_pObjMt)(1, 2) = _look.y; (*m_pObjMt)(1, 3) = 0.0f;
	(*m_pObjMt)(2,0) = _right.z; (*m_pObjMt)(2, 1) = _up.z; (*m_pObjMt)(2, 2) = _look.z; (*m_pObjMt)(2, 3) = 0.0f;
	(*m_pObjMt)(3,0) = x;        (*m_pObjMt)(3, 1) = y;     (*m_pObjMt)(3, 2) = z;       (*m_pObjMt)(3, 3) = 1.0f;
*/
	

}