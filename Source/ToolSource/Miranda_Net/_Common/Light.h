#ifndef __LIGHT_H
#define __LIGHT_H

//class CWorld;

typedef struct tagLIGHTCOLOR 
{ 
	FLOAT r1,g1,b1,r2,g2,b2; 
} LIGHTCOLOR,* LPLIGHTCOLOR;


class CLight : public D3DLIGHT9
{
public:
	CString m_strKey;
	DWORD m_dwIndex;
//public:
	CLight();
	CLight( LPCTSTR lpszKey, D3DLIGHTTYPE ltType, FLOAT x, FLOAT y, FLOAT z );
	~CLight() { }
	void SetLight( LPCTSTR lpszKey, D3DLIGHTTYPE ltType, FLOAT x, FLOAT y, FLOAT z );
	void Appear( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bEnable = TRUE );
	void SetPos( D3DXVECTOR3 vPos ) { Position = vPos; }
	void SetPos( FLOAT x, FLOAT y, FLOAT z ) { Position.x = x; Position.y = y; Position.z = z; }
	void SetDir( D3DXVECTOR3 vDir ) { Direction = vDir; }
	void SetDir( FLOAT x, FLOAT y, FLOAT z ) { Direction.x = x; Direction.y = y; Direction.z = z; }
};
#endif