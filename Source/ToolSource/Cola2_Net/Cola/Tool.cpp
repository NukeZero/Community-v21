#include "stdafx.h"

#include "Tool.h"

//CString		g_strASE;		// 읽어들인 ASE명
CString		g_strModel;		// 읽어들인 Model명
CString		g_strModelPath;	// 읽어들인 Model의 Path명

CString		g_strPathTexture;	// 텍스쳐 읽기 폴더

EDITMODE	g_EditMode = EM_ROTATE;
int			g_Cursor = CURSOR_NORMAL;


CSelectObject	g_SelObj;		// 현재 선택된 오브젝트
COption			g_Option;

int		m_nShowFNormal = 0;
int		m_nShowVNormal = 0;
int		g_nLighting = 0;
int		g_nEnvMap = 0;
int		g_nSpecular = 0;
D3DFILLMODE  g_nFillMode = D3DFILL_SOLID;
int		g_nEdgedFace = 0;
int		g_nBoundBox = VIEWBB_ALL;
int		g_nAutoRotate = 0;
int		g_nAutoRotateLight = 0;
int		g_nLOD = 0;		// 현재 lod단계 0, 1, 2
int		g_nGrid = 1;
int		g_nLightning = 0;
int		g_nCollObject = 0;

//gmpbigsun( 100419 )
BOOL	g_bTextureEffect = FALSE;
BOOL	g_bTextureEffectBase = FALSE;
BOOL	g_bTopView = FALSE;

D3DTEXTUREOP g_TextureOp[MAX_TEXTURE_OP] = 
{
	D3DTOP_MODULATE,
	D3DTOP_MODULATE2X,
	D3DTOP_MODULATE4X,
	D3DTOP_ADD,
	D3DTOP_ADDSIGNED,
	D3DTOP_ADDSIGNED2X,
	D3DTOP_SUBTRACT,
	D3DTOP_ADDSMOOTH,
	D3DTOP_MULTIPLYADD,
	D3DTOP_LERP
};
int		g_nSelectTextureOp = 0;

int		g_nPlay = 1;
CString	g_strVersion = "2006.9.8b";

D3DXVECTOR3	g_vForce1, g_vForce2, g_vForce3, g_vForce4;

//
//
//
void	SetCursor( int nType )
{
	if( nType == -1 )
		nType = g_Cursor;
	else
		g_Cursor = nType;

	switch( nType )
	{
	case CURSOR_NORMAL:		::SetCursor( ::LoadCursor( NULL, IDC_ARROW) );	break;
	case CURSOR_SIZEWE:		::SetCursor( ::LoadCursor( NULL, IDC_SIZEWE) );	break;
	case CURSOR_SIZENS:		::SetCursor( ::LoadCursor( NULL, IDC_SIZENS) );	break;
	case CURSOR_CROSS:		::SetCursor( ::LoadCursor( NULL, IDC_CROSS) );	break;
	}
}

void	InitTool( void )
{
	m_nShowFNormal = FALSE;
	m_nShowVNormal = FALSE;
}