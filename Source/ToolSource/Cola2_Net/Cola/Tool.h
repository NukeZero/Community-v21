#ifndef		__TOOL_H__
#define		__TOOL_H__

#include "..\_Common\xUtil.h"

//extern CString		g_strASE;		// 읽어들인 ASE명
extern CString		g_strModel;		// 읽어들인 Model명
extern CString		g_strModelPath;	// 읽어들인 Model의 Path명

extern CString		g_strPathTexture;	// 텍스쳐 읽기 폴더

extern int		m_nShowFNormal;
extern int		m_nShowVNormal;

extern int		g_nLighting;
extern int		g_nEnvMap;
extern int		g_nSpecular;
extern D3DFILLMODE  g_nFillMode;
extern int		g_nEdgedFace;
extern int		g_nBoundBox;
extern int		g_nAutoRotate;
extern int		g_nAutoRotateLight;
extern int		g_nLOD;		// 현재 lod단계 0, 1, 2
extern int		g_nGrid;
extern int		g_nLightning;
extern int		g_nCollObject;

extern BOOL		g_bTextureEffect;
extern BOOL		g_bTextureEffectBase;
extern BOOL		g_bTopView;

#define MAX_TEXTURE_OP	10
extern int		g_nSelectTextureOp;
extern D3DTEXTUREOP g_TextureOp[];



extern D3DXVECTOR3	g_vForce1, g_vForce2, g_vForce3, g_vForce4;

extern CString	g_strVersion;


enum EDITMODE
{
	EM_SELECT,
	EM_ROTATE,
	EM_PAN
};

// 마우스 커서 종류
#define		CURSOR_NORMAL	0	// 일반커서
#define		CURSOR_SIZEWE	1	// 좌우화살표
#define		CURSOR_SIZENS	2	// 상하화살표
#define		CURSOR_CROSS	3	// 십자

// 바운딩박스 표시
#define		VIEWBB_NONE		0	// 보이지 않음
#define		VIEWBB_MAX		1	// 대표바운딩박스만 보이기
#define		VIEWBB_ALL		2	// 모든 바운딩 박스 보이기

extern EDITMODE	g_EditMode;

// Select Type
enum SELTYPE
{
	ST_NONE,
	ST_VERTEX,
	ST_TRI,
	ST_ELEMENT,
};

// Mesh edit
class	CSelectObject
{
	SELTYPE  m_Type;			// 선택한 오브젝트 타입
	void	*m_pSelect;		// 선택한 어떤것.(버텍스나 페이스)

public:
	D3DXVECTOR3	m_vList[3];		// 버텍스나 삼각형 셀렉트때 이것을 사용.

	CSelectObject() { Init(); }
	~CSelectObject() { Destroy(); }

	void	Init( void )
	{
		m_Type = ST_NONE;
		m_pSelect = NULL;
	}

	void	Destroy( void )
	{
		Init();
	}

	void	Clear( void )
	{
		m_Type = ST_NONE;
		m_pSelect = NULL;
		memset( m_vList, 0, sizeof(D3DXVECTOR3) * 3 );
	}

	SELTYPE	GetType( void ) { return m_Type; }
	void	SetType( SELTYPE type ) { m_Type = type; }
	void	*GetSelect( void ) 
	{
		switch( m_Type )
		{
		case ST_VERTEX:
		case ST_TRI:
			return &m_vList[0];
		}
		return NULL;
	}
	void	SetSelect( void *pSelect )
	{
		m_pSelect = pSelect;
	}
	void	SetSelect( D3DXVECTOR3 *pV )
	{
		m_vList[0] = *pV;
	}
};

extern CSelectObject	g_SelObj;		// 현재 선택된 오브젝트

//
//
//
class COption
{
public:
	float	m_fScale;
	int		m_nR, m_nG, m_nB;
	COLORREF	m_bgColor;		// 배경색
	CString	m_strResource;		// 리소스 폴더(워킹폴더)

	COption() { Init(); }
	~COption() { Destroy(); }

	void	Init( void )
	{
		m_fScale = 10;
		m_nR = m_nG = m_nB = 255;
		m_strResource = "";
		m_bgColor = D3DCOLOR_ARGB(255, 128, 128, 128);
	}
	void	Destroy( void )
	{
		Init();
	}
};

extern COption	g_Option;


//////////////////////////////////////////////////////////////////////////
void	SetCursor( int nType = -1 );

#endif
