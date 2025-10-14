#ifndef		__GRAPHIC2D_H__
#define		__GRAPHIC2D_H__

#define D3DFVF_2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct FVF_2DVERTEX
{
    D3DXVECTOR4 vPos;	// The 3D position for the vertex
    DWORD		dwColor;		// The vertex color
};

#define		MAX_2DVERTEX		1024

class CGraphic2D
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	FVF_2DVERTEX	m_aVertex[ MAX_2DVERTEX ];

public:
	CGraphic2D() { Init(); }
	~CGraphic2D() { Destroy(); }

	void	SetDevice( LPDIRECT3DDEVICE9 d3dDevice ) { m_pd3dDevice = d3dDevice; }

	void	Init( void );
	void	Destroy( void );

	void	DrawPoint( int x, int y, int nSize, DWORD dwColor );
	void	DrawRect( int x, int y, int w, int h, DWORD dwColor );
};

extern CGraphic2D	g_Grp2D;


#endif