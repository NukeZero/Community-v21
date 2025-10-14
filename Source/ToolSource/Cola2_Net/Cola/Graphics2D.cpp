#include "stdafx.h"
#include "Graphic2D.h"

CGraphic2D	g_Grp2D;

//
//
//
void	CGraphic2D :: Init( void )
{
	int		i;

	m_pd3dDevice = NULL;
	memset( m_aVertex, 0, sizeof(FVF_2DVERTEX) * MAX_2DVERTEX );

	for( i = 0; i < MAX_2DVERTEX; i ++ )
	{
		m_aVertex[i].vPos    = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
		m_aVertex[i].dwColor = 0xffffffff;
	}
}

void	CGraphic2D :: Destroy( void )
{
	Init();
}


//
//
//
void	CGraphic2D :: DrawRect( int x, int y, int w, int h, DWORD dwColor )
{
	FVF_2DVERTEX	*aVertex = m_aVertex;
	aVertex[0].vPos.x = (float)x;
	aVertex[0].vPos.y = (float)y;
	aVertex[0].dwColor = dwColor;

	aVertex[1].vPos.x = (float)(x + (w - 1));
	aVertex[1].vPos.y = (float)y;
	aVertex[1].dwColor = dwColor;

	aVertex[2].vPos.x = (float)(x + (w - 1));
	aVertex[2].vPos.y = (float)(y + (h - 1));
	aVertex[2].dwColor = dwColor;

	aVertex[3].vPos.x = (float)x;
	aVertex[3].vPos.y = (float)(y + (h - 1));
	aVertex[3].dwColor = dwColor;

	aVertex[4].vPos.x = (float)x;
	aVertex[4].vPos.y = (float)y;
	aVertex[4].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, aVertex, sizeof(FVF_2DVERTEX) );


}

//
//
//
void	CGraphic2D :: DrawPoint( int x, int y, int nSize, DWORD dwColor )
{
	FVF_2DVERTEX	*aVertex = m_aVertex;

	aVertex[0].vPos.x = (float)x;
	aVertex[0].vPos.y = (float)y;
	aVertex[0].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_2DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, aVertex, sizeof(FVF_2DVERTEX) );
}
