#pragma once

class CObjControl
{
public:
	
	CObjControl();
	~CObjControl();

	void strafe(float units); // left/right
	void walk(float units);   // forward/backward
	void RotateY(float angle);   // rotate on up vector

	void getObjMt(D3DXMATRIX* V); 
	void UpdateMatrix();
	void getPosition(D3DXVECTOR3* pos); 
	void setPosition(D3DXVECTOR3* pos);

	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);
private:

	D3DXMATRIX*	m_pObjMt;
	D3DXMATRIX  m_RotateMt;
	float		m_fAngle;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
};