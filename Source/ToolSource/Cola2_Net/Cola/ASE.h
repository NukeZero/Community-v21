#ifndef	__ASE_H__
#define	__ASE_H__

#include <stdio.h>
#include <d3dx9.h>
#include "scanner.h"
//#include "Bone.h"

class CGeoMObject;
class CAse;

#define		VER_INFO			2		// *.inf파일의 버전 

#ifdef	__XVS
  #ifdef	__X2LINK
	#define D3DFVF_ASEVERTEX (D3DFVF_XYZB3 | D3DFVF_LASTBETA_D3DCOLOR | D3DFVF_NORMAL | D3DFVF_TEX1)
  #else
	#define D3DFVF_ASEVERTEX (D3DFVF_XYZB1 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
  #endif
#else
#define D3DFVF_ASEVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#endif
//#define D3DFVF_ASEVERTEX		D3DFVF_NORMALVERTEX
#define	D3DFVF_SWDFORCE			(D3DFVF_XYZ | D3DFVF_DIFFUSE)

#ifdef	__XVS
struct FVF_ASEMESH
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
  #ifdef	__X2LINK
	FLOAT		w1, w2;		// vertex weight
  #endif
	DWORD		matIdx;
    D3DXVECTOR3 normal;		// The surface normal for the vertex
//    DWORD		color;		// The vertex color
    FLOAT       tu, tv;		// The texture coordinates
};
#else
struct FVF_ASEMESH
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    DWORD		color;		// The vertex color
    FLOAT       tu, tv;		// The texture coordinates
};
#endif

struct SKIN_VERTEX
{
    D3DXVECTOR3 m_vLocalPos;	// bone기준의 로컬 좌표
    D3DXVECTOR3 m_vWorldPos;	// World로 변환된 좌표 - 실시간 계산 된다.
	D3DXVECTOR3	m_vNormal;		// 버텍스 노말
	D3DXVECTOR3	m_vWorldNormal;	// 본에 따라 트랜스폼 된 노말 - 실시간 계산
};

struct PHYSIQUE_BLEND
{
	float		m_nWeight;		// 블렌딩 가중치
	char		m_strBoneName[32];	// 소속된 본의 strName
	CGeoMObject	*m_pBone;			// 소속된 본의 포인터
};

struct PHYSIQUE
{
	int				m_nNumBone;		// 소속된 본 갯수.		기본은 하나.
	PHYSIQUE_BLEND	*m_pBlendVertex;
	PHYSIQUE()
	{
		m_pBlendVertex = NULL;
	}
	~PHYSIQUE()
	{
		SAFE_DELETE( m_pBlendVertex );
	}
};

struct TEXTURE_COORDI
{
	float	u;
	float	v;
	float	w;
};

struct	MESH_FACE
{
	int		a, b, c;		// 버텍스 버퍼의 index a, b, c
//	int		ta, tb, tc;		// TVertex[]의 인덱스
	int		v1, v2, v3;		// 버텍스 리스트의 인덱스
	int		m_nMaterialID;	// 각 페이스의 서브 매터리얼 ID
	D3DXVECTOR3	n;
};

//struct	INDEX_BUFFER
//{
//	int		nIdx;				// 버텍스 리스트의 인덱스
//	D3DXVECTOR3		normal;
//};
/*
struct	VERTEXBUFFER_BLOCK
{
	int		m_nStartVertex;			// 버텍스 버퍼에서의 시작 인덱스
	int		m_nPrimitiveCount;		// 프리미티브(삼각형)갯수
	int		m_nTextureID;			// 텍스쳐 번호
	int		m_n2Side;				// 2side 속성
	int		m_nMaxUseBone;
	int		m_UseBone[MAX_VS_BONE];
#ifndef	__XVS
	LPDIRECT3DTEXTURE9	*m_pMtrlBlkTexture;		// 이 지오메트리가 사용할 텍스쳐
#endif
};
*/
struct SUB_MATERIAL
{
	LPDIRECT3DTEXTURE9      m_pTexture;			// 텍스쳐 비트맵읽어다 텍스쳐로 만들어 둔것.
	D3DMATERIAL9	m_Material;
	char	m_strBitmapFileName[256];			// 텍스쳐로 쓸 비트맵 파일명
//	int		m_bOpacity;		// 오파시티맵이 있는가?
	int		m_nAmount;
//	int		m_n2Side;
//	int		m_bReflect;
//	int		m_nSelfIlluminate;
	DWORD	m_dwEffect;
	FLOAT	m_fTilingU, m_fTilingV;
};

struct ASE_MATERIAL
{
	D3DMATERIAL9	m_Material;
	char	m_strBitmapFileName[256];			// 텍스쳐로 쓸 비트맵 파일명
//	int		m_bOpacity;		// 오파시티맵이 있는가?
	int		m_nAmount;
	DWORD	m_dwEffect;
//	int		m_n2Side;		// 2 side속성.
//	int		m_bReflect;
//	int		m_nSelfIlluminate;
	FLOAT	m_fTilingU, m_fTilingV;
	
	int				m_nMaxSubMaterial;			// 서브 메터리얼 갯수
	SUB_MATERIAL	*m_pSubMaterial;			// 서브 메터리얼 리스트
	LPDIRECT3DTEXTURE9      m_pTexture;			// 텍스쳐 비트맵읽어다 텍스쳐로 만들어 둔것.
};

struct ASETM_ANIMATION
{
	BYTE			m_bFlag;	// ASE에서 POS이 있었는지, ROT가 있었는지를 표시하는 플래그
	int				m_nTick;	// *
	D3DXVECTOR3		m_vPos;		// 각 프레임의 부모로 부터의 상대 좌표
	D3DXVECTOR3		m_vAxis;	// 회전 축
	FLOAT			m_fAngle;	// 회전 각도
//	D3DXMATRIX		m_matAniTM;	// * Animation Matrix
	D3DXQUATERNION	m_qAni;		// m_matAniTm의 회전 쿼터니언 버전
	D3DXQUATERNION	m_qAniScale;// 보간때 쓸 스케일 회전 쿼터니언
	D3DXVECTOR3		m_vScaleFactor;	// 보간때 쓸 스케일 팩터값.
//	D3DXVECTOR3		m_vAni;		// m_matAniTm의 Translation벡터
};


// *GEOMOBJECT는 Biped의 노드가 될수도 있고 스킨의 메쉬가 될수도 있다.
// GEOMOBJECT의 이름이 Bip로 시작하는건 바이페드의 본이므로 읽지 않는다.


class CASEMesh
{
protected:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
public:
	CGeoMObject	*m_pGeoMObject;					// 상위 GeoMObject의 포인터

	int			m_nMaxVertexBuffer;				// maxface * 3 된 버텍스 버퍼 크기
	int			m_nMaxVertexList;				// 버텍스 리스트 갯수
	PHYSIQUE	*m_pPhysiqueVertex;				// 피지크 정보가 있는 버텍스 리스트.

	int				m_nMaxFaceList;			// 페이스 갯수
	FVF_ASEMESH		*m_pVertexBuffer;		// 버텍스 버퍼.  이 버퍼가 AGP버퍼로 일괄 전송된다.
	INDEX_BUFFER	*m_pIndexBuffer;		// 페이스 인덱스 버퍼
	SKIN_VERTEX		*m_pVertexList;			// 스킨의 버텍스리스트 원본 - 소속 bone에 따라 로컬로 변환된이다.
//	D3DXVECTOR3		*m_pNoalList;			// ??
	MESH_FACE		*m_pFaceList;			// Face List
	int				m_nMaterialRef;			// 스킨메쉬가 사용하는 메터리얼 ID
	FVF_ASEMESH		*m_ptempIVB;
	FVF_ASEMESH		*m_pIVB;		// Indexed용 버텍스 버퍼 - max vertex list + a(tu,tv가 다른 버텍스)
	WORD			*m_pIB;			// 인덱스 버퍼 - MaxFace * 3
	WORD			*m_pIIB;		// Indexed용 버텍스 버퍼가 참조하는 버텍스리스트의 인덱스 - m_pIVB와 크기가 같다. VertexList의 인덱스가 대응된다.
	WORD			*m_ptempIIB;
	BYTE			*m_ptempID;		// MaterialID 배열. 버텍스가 같은지 비교할때 ID까지 비교해야한다.
	int				m_nMaxUseBone;	// this오브젝트에서 사용하는 뼈대의 최소 개수.
	int				m_UseBone[ MAX_VS_BONE ];
	
	int				m_nMaxIVB;
	int				m_nMaxIB;

	int			m_bOpacity;		// 이 메쉬가 알파를 사용하고 잇는가?
	int			m_bRigid;		// 1링크방식(non-blend)으로 계산 한다.
	int			m_bBump;		// 이 메쉬가 범프를 사용중인가.

	int					m_nMaxVertexBufferBlock;
	MATERIAL_BLOCK		*m_pVertexBufferBlock;
#ifdef	__XVS
	LPDIRECT3DTEXTURE9	*m_pMtrlBlkTexture;		// 매터리얼 블럭네 텍스쳐 포인터를 밖으로 뺐다.
#endif
	LPDIRECT3DVERTEXBUFFER9		m_pVB;			
	LPDIRECT3DVERTEXBUFFER9		m_pIVertexBuffer;		// Indexed용 VB
	LPDIRECT3DINDEXBUFFER9		m_pIIndexBuffer;		// Indexed용 IB

	// 이부분은 읽을때만 사용하고 저장하지 않는다.
//	int				m_nMaxFace;
//	MESH_FACE		*m_pFaceList;	// 페이스 리스트
	int				m_nMaxTVertices;
	TEXTURE_COORDI	*m_pTVertices;	// 텍스쳐 좌표 버텍스
	int				m_nMaxCVertices;
	DWORD			*m_pCVertices;	// 컬러 버텍스
	
	float	m_nWidth, m_nHeight, m_nDepth;
	D3DXVECTOR3		m_vMin, m_vMax;		// bound box

	// 
	CASEMesh();
	~CASEMesh();

	void	Init( void );
	void	Destroy( void );

	void	SetGeoMObject( CGeoMObject *pObj ) { m_pGeoMObject = pObj; };

	// Read ASE Key
	int		Read_MESH_MAPPING_CHANNEL( CScanner *scan );
	int		Read_MESH_TVERTLIST( CScanner *scan );
	int		Read_MESH_TFACELIST( CScanner *scan );
	int		Read_MESH_CVERTLIST( CScanner *scan );
	int		Read_MESH_CFACELIST( CScanner *scan );
	int		Read_MESH_VERTEX_LIST( CScanner *scan );
	int		Read_MESH_FACE_LIST( CScanner *scan );
//	int		Read_MESH_NORMALS( CScanner *scan );
	int		Read_MESH( CScanner *scan );
	int		Read_PHYSIQUE( CScanner *scan );
	int		Read_SKIN( CScanner *scan );
	int		Read_SKIN( CScanner *scan , int nVertNum);
//	int		SaveSKN( LPCTSTR strFileName );
	int		SaveMesh( FILE *fp );

	void	SortMaterialBlock( void );
	void	CreateVertexBufferBlock( void );
	int		CalcUseBone( int nStart, int nCnt, int *pBuff );
	void	ReindexingBoneIdx( int nGroup );
	void	Reindex( int nStart, int nCnt, int nMaxUseBone, int *pUseBone );
	void	DivideMesh(int nStart, int nCount);
	void	SetBoneIdx( void );

	int		AddIVB( const FVF_ASEMESH &v, int nIdx, int nID );
	void	MakeIndexed( void );

	int		SendVertexBuffer( BOOL bSendIB );
	void	TransformToLocal( void );
	int		SetVertexBuffer( void );
	void	SetBlendVertex( void );

	void	TransformSkinVertex( void );
	void	CalcBoundBox( void );
	int		CalcNormal( void );
	void	ClearNormal( void );

	// 충돌/교차 판정.
	D3DXVECTOR3 *IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir );


	HRESULT	Render( void );
	HRESULT	CreateVB( LPDIRECT3DDEVICE9 pd3dDevice );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; return S_OK; }
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

};


//#define		GEOTYPE_SKINMESH	0		// 피지크 정보가 있는 메시
//#define		GEOTYPE_BIPED		1		// 바이페드
//#define		GEOTYPE_LINKMESH	2		// 링크된 메시
//#define		GEOTYPE_STATICMESH	3		// 걍 정적 메시

// Mesh Type
//#define		MT_NORMAL		0
//#define		MT_SKIN			1
//#define		MT_BIPEDLINK	2
//#define		MT_BIPED		3

// 지오메트리 오브젝트.. 
// TM만 가지면 Bone으로 쓰이고 메쉬까지 가지고 있다면 그 자체로 독립적인 메쉬로 쓰일수 있다.
#define		CBone		CGeoMObject

class CGeoMObject
{
protected:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	GMTYPE	m_Type;		// 바이페드인가 스킨메쉬인가
public:
	CGeoMObject	*m_pParent;		// 상위 부모 노드
	CAse		*m_pASE;			// this를 포함하는 ASE의 포인터
	int		m_bHelper;

	int			m_nID;			// 계층순서 대로 부여된 번호
	int			m_nIdx;			// 바이페드 GeoMObject 트리는 루트부터 차례대로 인덱스를 붙여준다.
	char		m_strName[64];		// * 스트링형태의 노드 이름
	D3DXVECTOR3		m_vRotAxis;		// 회전축(TM의)
	float			m_fRotAngle;	// 회전각(TM의)
	D3DXVECTOR3		m_vScaleAxis;	// 스케일 회전축(TM의)
	float			m_fScaleAngle;	// 스케일 회전각(TM)
	D3DXVECTOR3		m_vScaleFactor;	// 스케일 값.
	float			m_fFlip;		// TM의 flip
	D3DXMATRIX		m_matTM;		// ase원본 TM
	D3DXMATRIX		m_matInverseTM;	// * 미리 변환된 인버스TM 
	D3DXMATRIX		m_matLocalTM;	// * 미리변환된 로컬 TM
	D3DXQUATERNION	m_qLocal;		// LocalTM회전의 쿼터니언 판
	D3DXVECTOR3		m_vLocal;		// LocalTM의 이동좌표
	D3DXMATRIX		m_matWorld;		// 최종 변환된 월드 매트릭스.
	D3DXQUATERNION	m_qWorld;		// 각 노드별로 최종변환된 쿼터니언
	D3DXVECTOR3		m_vWorld;		// World매트릭스의 이동벡터

	CASEMesh			*m_pMesh;		// 지오메트리가 메쉬정보를 가지면 이곳에.
//	int				m_nTempMaxVertices;	
//	FVF_ASEMESH	*m_pTempVertices;	// ASE에 표기되어 있는 버텍스 리스트를 그대로 읽음. 사용후 삭제
//	int				m_nMaxVertices;	// 버텍스 갯수
//	FVF_ASEMESH	*m_pVertices;	// 중복된 버텍스를 따로 떼내어 NumFace * 3의 크기가 된 버텍스리스트. (원본)
//	LPDIRECT3DVERTEXBUFFER9 m_pVB;	// 하드웨어에 세팅될 버텍스 버퍼

//	int				m_nMaxFrame;	// * 프레임 갯수(이건 상위 CAse에 있다.)
	ASETM_ANIMATION	**m_pFrame;		// * 애니메이션 프레임정보의 배열
	ASETM_ANIMATION	*m_pLastFrame;	// 중간에프레임이 비어있을때 마지막으로 사용한 프레임정보를 가지고 채워넣어야 한다.

//	LPDIRECT3DTEXTURE9      m_pTexture;		// 이 지오메트리가 사용할 텍스쳐


	int					m_nMaxChild;	// 하위 자식 노드 갯수
	CGeoMObject			**m_pChildList;	// 하위 자식 노드들의 리스트;

	CGeoMObject();
	CGeoMObject( CAse *pASE );
	~CGeoMObject();

	void	Destroy( void );
	void	Init( void );
	GMTYPE GetType( void )	{ 
		if( m_Type == GMT_ERROR )	
			Error( "%s의 Type이 정해지지 않았다.", m_strName );
		return m_Type;
	}
	void	SetType( GMTYPE type ) { m_Type = type; }

	int		Read_GEOMOBJECT( CScanner *scan );
	int		Read_TM_ANIMATION( CScanner *scan );
	int		Read_CONTROL_ROT_TRACK( CScanner *scan );
	int		Read_CONTROL_POS_TRACK( CScanner *scan );
	int		Read_CONTROL_SCALE_TRACK( CScanner *scan );
	int		Read_NODE_TM( CScanner *scan );

//	int		SaveCHR( LPCTSTR strFileName );
	int		SaveANI( FILE *fp );
	int		SaveTMAni( FILE *fp );
	void	SavePathRecv( FILE *fp );
	void	SaveBON_Ani( FILE *fp );
//	int		SaveMesh( LPCTSTR szFileName ) { return m_pMesh->SaveMesh( szFileName ); }
	int		SaveMesh( FILE *fp ) { return m_pMesh->SaveMesh(fp); }
	int		WriteBON( FILE *fp );		// .bon 파일 저장
	void	WriteBON_Recursive( FILE *fp );	// 뼈대저장 리커시브함수
	void	WriteBone( FILE *fp );			// o3d파일내의 기본 뼈대셋부분 저장.
	void	WriteBone_Recursive( FILE *fp );	// 리커시브 함수
	void	WriteBoneInv_Recursive( FILE *fp );	// 
	void	WriteBoneInfo( FILE *fp );		// 모션파일에 들어가는 뼈대구성정보
	char	*GetBonesName_Recursive( char *pBuff );
		
	// 리커시브 함수
	int		SetIdx( int nIdx );
	void	SetType_Recu( GMTYPE type );
	int		GetLength( void );
	void	GetFrameSize( void );
	void	SetupMatrices( int nFrame );
	void	SetupQuat( float fSlerp, D3DXMATRIX *pmRoot = NULL );
//	void	SetupQuat( float fSlerp );

	void	SetWorldTransform( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX *matWorld );
	void	ClearNormal( void );

	void	TreeLinkChild( CGeoMObject *pChild );
	CGeoMObject	*TreeFindNode( LPCTSTR strName );
	BOOL	TreeDeleteHelper( void );		// 트리내에 의미없는 헬퍼 오브젝트들을 삭제.
		
	// 충돌/교차 판정
	D3DXVECTOR3 *IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir );

	HRESULT	Render( const D3DXMATRIX *mWorld );
	void	RenderBB( const D3DXMATRIX* pmWorld );
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};

#define		MAX_GROUP			3
#define		MAX_GEOMOBJECT		128

struct GROUP
{
	int			m_nMaxGeoMObject;
	CGeoMObject	*m_pGeoMObject[ MAX_GEOMOBJECT ];		// ASE내의 오든 오브젝트. 각각의 오브젝트는 바이페드 트리로도 쓰인다
};

class CAse : public CModel
{
	int				m_nMaxMaterial;				// 총 매터리얼 갯수
	ASE_MATERIAL	*m_pMaterialList;			// 매터리얼 리스트
	MOTION_ATTR	*m_pAttr;
	char			m_szFullPath[1024];			// ASE파일의 풀패스.

	GROUP		*m_pGroup;					// 현재 스위칭된 그룹.
	GROUP		m_Group[ MAX_GROUP ];		// 그룹 리스트. 
	int			m_nGroupCount;
protected:

public:
//	int			m_nMaxGeoMObject;
//	CGeoMObject	*m_pGeoMObject[ MAX_GEOMOBJECT ];		// ASE내의 오든 오브젝트. 각각의 오브젝트는 바이페드 트리로도 쓰인다
//	CGeoMObject	*m_pObjectLOD2[ MAX_GEOMOBJECT ];		// #LOD2 그룹
//	CGeoMObject	*m_pObjectLOD3[ MAX_GEOMOBJECT ];		// #LOD3 그룹
	CGeoMObject *m_pCollObject;				// 충돌용 메시
	CGeoMObject *m_pPath;					// 이동경로데이타가 있는 헬퍼 오브젝트.
	int			m_bBiped;					// 읽어들인 ASE에 Biped가 포함되어 있는가.
	int			m_nMaxBoneNode;			// 본의 노드 개수
	int			m_bSendVS;				// for VS. CAse렌더전에 뼈대를 VS로 다 보낼것인지 어떨지에 대한 플래그
	int			m_bScale;				// 스케일 애니메이션이 포함되어 있음.
	int			m_bLOD;					// LOD모델이 포함되어 있음.
	
	int			m_nFramePerSample;			// Frame per Sample		몇 프레임단위로 샘플링 했는가.
	int			m_nFrameSpeed;				// 초당 프레임수는 얼마 기준인가?(기본 30)
	int			m_nTicksPerFrame;			// 프레임당 틱은 몇인가?(ASE의 *SCENE에서 추출)
	int			m_nCount;
	LARGE_INTEGER	m_lnFreq;
	LARGE_INTEGER	m_lnPrev, m_lnCurr;
	double		m_dUps;		// update per sec - 화면 갱신 시간
	D3DXMATRIX	m_mLocalRH, m_mLocalShield, m_mLocalKnuckle, m_mLocalLH;
//	D3DXMATRIX	m_mLocalBow, m_mLocalYoyo
	int			m_nMaxEvent;
	D3DXVECTOR3	m_vEvent[MAX_MDL_EVENT];		// 이벤트 좌표.
	CGeoMObject *m_pEvent[MAX_MDL_EVENT];		// 이벤트 4좌표의 부모노드;

	CAse();
	~CAse();
	void	Init( void );
	void	Destroy( void );

	void	ClearNormal( void );

	BOOL	IsEmpty( void ) { return (m_pGroup->m_nMaxGeoMObject == 0); }
	BOOL	IsBiped( void ) { return m_bBiped; }
	ASE_MATERIAL *GetMaterial( void ) { return m_pMaterialList; }
	int		GetMaxMaterial( void ) { return m_nMaxMaterial; }
//	CGeoMObject *GetSelectObj( void ) { return m_pSelectGeo; }
	CGeoMObject *GetBiped( void );
	CGeoMObject	*FindGeoM( GROUP *pGroup, LPCTSTR szName );		// GeoMObj를 찾는다.  하위 트리까지 검색한다.
	CGeoMObject	*GetGeoM( int nIdx ) { return m_pGroup->m_pGeoMObject[ nIdx ]; }
//	GROUP *SetGroup( int nNum ) { return m_pGroup = &m_Group[nNum]; }
	void	SetGroup( int nNum ) { m_pGroup = &m_Group[nNum]; }
	GROUP *GetGroup( int nIndex ) { return &m_Group[nIndex]; }
	GROUP *GetCurrentGroup( void ) { return m_pGroup; }

//	float	GetRadius( void );

	void FrameMove( D3DXVECTOR3 *pvSndPos = NULL, float fSpeed = 1.0f );
//	void	FrameMove( D3DXVECTOR3 *pvSndPos = NULL );
	void		SetupMatrices( void );
	void		SetupQuat( void );
	/*
	int			SetID( void ) { SetID( m_pBone, 0 ); }
	void		Destroy( void ) { Destroy( m_pBone ); }
*/
	// ASE Key읽기
	int		LoadModel( LPCTSTR szFileName );
	void	SaveAuto( LPCTSTR szASEName );
	int		Read_SCENE( CScanner *scan );
	int		Read_MATERIAL_LIST( CScanner *scan );
	void	Read_SKIN_DATA( CScanner *scan );
	void	LoadTexture( void );
	int		SaveCHR( LPCTSTR szFileName );
	int		SaveANI( LPCTSTR szFileName );
//	int		SaveMesh( void );
//	int		SaveMeshHeader( LPCTSTR strHead );
//	int		SaveMesh( LPCTSTR strFileName );
	int		SaveMeshHeader( FILE *fp, LPCTSTR szFileName );		// 메쉬의 공통헤더부분 저장.
	int		SaveNormMesh( LPCTSTR szFileName );		// 일반키프레임 메쉬 저장.
	int		SaveBipedMesh( LPCTSTR szHeader );		// 바이페드 메시저장(말머리 + 오브젝트명 버전)
	int		SaveBipedMeshSeq( LPCTSTR szFileName );	// 파일명 + 번호 버전.
	int		SaveSkinEach( LPCTSTR szFileName );
	int		SaveSkinOne( LPCTSTR szFileName, BOOL bSaveBone );
	int		SaveModelEx( LPCTSTR szFileName );
	int		LoadModelEx( LPCTSTR szFileName );

	void	ProcessSetType( GROUP *pGroup );
	void	ProcessLocalTransform( GROUP *pGroup );	// 피지크오브젝트는 뼈대기준으로 좌표변환시키고 일반오브젝트는 피봇기준으로 변환시킨다.
	
	
	void	CreateMaterialBlock( GROUP *pGroup );
	void	ReindexingBoneIdx( void );		

	// 충돌/교차 판정
	D3DXVECTOR3 *IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir );

	// Animation
	BOOL	IsLoadAttr( void ) { if(m_pAttr) return TRUE; else return FALSE; }
	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }	
	DWORD	GetAttr( int nNumFrm ) { return m_pAttr[ nNumFrm ].m_dwAttr; }
	DWORD	IsAttrHit( float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr->m_dwAttr;
		return 0;
	}
	MOTION_ATTR *IsAttrSound( float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( m_pAttr == NULL )	return NULL;
		if( pAttr->m_dwAttr & MA_SOUND )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr;
		return NULL;
	}
	DWORD	IsAttrQuake( float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( void ) { return IsAttrSound(m_fFrameCurrent); }
	DWORD	IsAttrHit( void ) { return IsAttrHit(m_fFrameCurrent); }
	DWORD	IsAttrQuake( void ) { return IsAttrQuake(m_fFrameCurrent); }
	void	SetAttr( float fNumFrm, DWORD dwAttr ) 
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	int		GetMaxObject( void );			// GMOBJECT 수
	int		GetMaxVertex( void );			// 총 버텍스 개수
	int		GetMaxFace( void );				// 총 면수
	int		GetMaxMtrlBlk( void );			// 총 매터리얼 블럭 수
	int		GetMaxBone( void );				// 본 개수.
	char	*GetBonesName( char *pBuff )
	{
		CGeoMObject *pBiped = GetBiped();
		if( pBiped )
		{
			pBiped->GetBonesName_Recursive( pBuff );
			return pBuff;
		}
		return NULL;
	}
	BOOL	IsHaveCollObj( void ) { return (m_pCollObject)? TRUE : FALSE; }


	// virtual
	int		SaveModel( LPCTSTR szFileName );
	BOOL	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld );
	BOOL	RenderBB( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld );
	
	// 디바이스 초기화 및 삭제 
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};



#endif


















