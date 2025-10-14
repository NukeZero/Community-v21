// BeastView.h : interface of the CBeastView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELMNG_H__25FE7788_56AD_4D50_8D9E_E69C969DE0F9__INCLUDED_)
#define AFX_MODELMNG_H__25FE7788_56AD_4D50_8D9E_E69C969DE0F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "data.h"
#include "2DRender.h"
 
#include <D3DX9.h>

#define BEGIN_MODEL_ID 10

typedef struct tagMODELELEM
{
	DWORD m_dwType;
	DWORD m_dwIndex;
	TCHAR m_szName [48];

	TCHAR* GetMotion( int i ) 
	{ 
		if( i < 0 || i >= m_nMax )
		{
			Error( "tagMODELELEM : out of range %d", i );
			i = 0;
		}

		if( m_apszMotion ) 
			return &m_apszMotion[ i * 32 ]; 
		return NULL; 
	}
	int		m_nMax;
	TCHAR* m_apszMotion;
	DWORD m_dwModelType;
	TCHAR m_szPart[48]; 
	FLOAT m_fScale;
	LPDIRECT3DTEXTURE9 m_pTexture;	// 4바이트 정렬을 위해서 옮김.-xuzhu-
	BYTE  m_bMark;
	BYTE  m_bFly      : 1;
	BYTE  m_dwDistant : 2;
	BYTE  m_bPick     : 1; // 캐릭터 이동시 마우스 피킹에 걸리는 오브젝트인가?
	BYTE  m_bUsed     : 1;
	BYTE  m_bTrans    : 1; // 캐릭터를 가릴 시 반투명이 되는 오브젝트인가?
	BYTE  m_bShadow   : 1;
#ifdef __XTEXTURE_EX2
	BYTE  m_bReserved : 1;
	int m_nTextureEx;			// 텍스쳐 셋 번호 0 ~ 7
#else
	BYTE  m_bTextureEx : 1;		// 확장 텍스쳐를 사용하는 모델이냐.
#endif

#ifdef __YRENDER_FLAG
	BYTE m_bRenderFlag : 1;
#endif //__YRENDER_FLAG

}* LPMODELELEM, MODELELEM;


class CBillboard;
class CModel; 

class CModelMng 
{
	BOOL LoadXFile(LPDIRECT3DDEVICE9 pd3dDevice,TCHAR* lpszFileName,int nModelType);
public:
	CMapStrToPtr m_mapFileToMesh;
	CMapStrToPtr m_mapFileToBillboard;
//	CTextureMng m_billboardTextureMng;

//	int m_anModelElemNum[ 7 ]; 
//	LPMODELELEM m_paModelElem[ 7 ];		// ??? -xuzhu-
//	CMapDWordToPtr m_mapModelElem[ 7 ];

#ifdef __BEAST

#ifdef __Y_PATROL
	CPtrArray m_aaModelElem[ 9 ]; // 편집 가능한 배열 
#else //__Y_PATROL
	CPtrArray m_aaModelElem[ 8 ]; // 편집 가능한 배열 	
#endif //__Y_PATROL

#else	// __BEAST
	CFixedArray< tagMODELELEM > m_aaModelElem[ 8 ]; // 편집 불가능. 모든 것이 최적화되어 있다. 편집이 필요없는 클라이언트, 서버에서만 사용한다.
#endif	// __BEAST


	LPMODELELEM GetModelElem( DWORD dwType, DWORD dwIndex );
	int			GetModelElemCount( DWORD dwType );

	CModelMng();
	~CModelMng();

	void Free();
	
#ifdef __BEAST
	DWORD m_dwModelId[ OT_MOVER + 1 ];
	CString GetNewModelDefine( DWORD dwType );
	DWORD GetNewModelId( DWORD dwType );
	BOOL RemoveModel( DWORD dwType, DWORD dwId );
	BOOL AddModel( DWORD dwType, LPMODELELEM pModelElem );
#endif
	//BOOL UpdateObjFile( LPCTSTR lpszFileName );

	void MakeBoneName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex );
	void MakeModelName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex );
	void MakeMotionName( TCHAR* pszMotionName, DWORD dwType, DWORD dwIndex, DWORD dwMotion );
	void MakePartsName( TCHAR* pszPartsName, LPCTSTR lpszRootName, DWORD dwIndex, int nSex = SEX_SEXLESS );
	void GetSubString( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex );

	BOOL    LoadMotion( CModel* pModel, DWORD dwType, DWORD dwIndex, DWORD dwMotion );
	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts = FALSE );
	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, LPMODELELEM lpModelElem, int nType, BOOL bParts = FALSE ); //int nModelType, LPCTSTR lpszTexture = NULL );

	CModel* CreateBillboard( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName, BOOL bAnimation );

	BOOL LoadScript( LPCTSTR lpszFileName );
	BOOL LoadBillboard( LPCTSTR lpszFileName );

	// dx 초기화 관련 
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELMNG_H__25FE7788_56AD_4D50_8D9E_E69C969DE0F9__INCLUDED_)






