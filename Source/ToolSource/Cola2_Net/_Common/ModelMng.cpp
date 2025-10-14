// BeastView.cpp : implementation of the CBeastView class
//

#include "stdafx.h"
#include "sfxbase.h"
#include "ModelMng.h"
#include "Billboard.h"
#include "..\Resource\DefineNeuz.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CModelMng::CModelMng()
#ifdef __INFOCRITSECT_DEBUG__
: m_AccessLock( "CModelMng::m_AccessLock" )
#endif	// __INFOCRITSECT_DEBUG__
{

}
CModelMng::~CModelMng()
{
#ifdef __BEAST
	for( int i = 0; i < MAX_OBJTYPE ; i++ )
	{
		CPtrArray* pPtrArray = &m_aaModelElem[ i ];
		for( int j = 0; j < pPtrArray->GetSize(); j++ )
		{
			LPMODELELEM pModelElem = (LPMODELELEM) pPtrArray->GetAt( j );
			if( pModelElem )
				SAFE_DELETE( pModelElem->m_apszMotion );
			SAFE_DELETE( pModelElem ); 
		}
	}
#else
	for( int i = 0; i < MAX_OBJTYPE ; i++ )
	{
		CFixedArray< tagMODELELEM >* apModelElem = &m_aaModelElem[ i ];
		for( int j = 0; j < apModelElem->GetSize(); j++ )
		{
			LPMODELELEM pModelElem = (LPMODELELEM) apModelElem->GetAt( j );
			if( pModelElem )
				SAFE_DELETE( pModelElem->m_apszMotion );
		}
	}
#endif
	Free();
}
void CModelMng::Free()
{
/*
	MapStrToPtrItor pos;
	for( pos = m_mapFileToMesh.begin(); pos != m_mapFileToMesh.end(); pos++ )
	{
		CModel* pModel = ( CModel* )(*pos).second;
		SAFE_DELETE( pModel );
	}
	m_mapFileToMesh.clear();
	*/
}
TCHAR szRoot[ MAX_OBJARRAY ][32] = { "obj", "ani", "ctrl", "sfx", "item", "mvr", "region","obj" };
//TCHAR szRoot[ MAX_OBJARRAY ][32] = { "obj", "ctrl", "sfx", "item", "mvr", "region" ,"" };

LPMODELELEM CModelMng::GetModelElem( DWORD dwType, DWORD dwIndex )
{
	int		nSize = m_aaModelElem[ dwType ].GetSize();
	if( (int)dwIndex < 0 || (int)dwIndex >= nSize )
	{
		LPCTSTR szErr = Error( "CModelMng::GetModelElem : 범위를 넘어섬 type=%d, size=%d, index=%d", (int)dwType, nSize, (int)dwIndex );
		ADDERRORMSG( szErr );
		dwIndex = 0;
	}
	return (LPMODELELEM)m_aaModelElem[ dwType ].GetAt( dwIndex );
}
void CModelMng::MakeBoneName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	//_tcscpy(pszModelName,"Model\\");
	_tcscpy( pszModelName, szRoot[dwType]);
	//_tcscpy(pszModelName,szRoot[dwType]);
	_tcscat( pszModelName, "_" );
	_tcscat( pszModelName, lpModelElem->m_szName );
	_tcscat( pszModelName, _T(".chr") );
}
void CModelMng::MakeModelName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	if( lpModelElem == NULL )
	{
		Error( "MakeModelName : dwType=%d dwIndex=%d", dwType, dwIndex );
	}
	if( lpModelElem->m_dwModelType == MODELTYPE_BILLBOARD )//|| lpModelElem->m_dwModelType == MODELTYPE_ANIMATED_BILLBOARD )
	{
		_tcscpy( pszModelName, lpModelElem->m_szName );
		return;
	}
	if( dwType == OT_SFX )
	{
		if( _tcschr( lpModelElem->m_szName, _T( '_' ) ) == NULL )
		{
			_tcscpy( pszModelName, szRoot[ dwType ] );
			_tcscat( pszModelName, "_" );
			_tcscat( pszModelName, lpModelElem->m_szName );
		}
		else
			_tcscpy( pszModelName, lpModelElem->m_szName );
	}
	else
	{
		_tcscpy( pszModelName, szRoot[ dwType ] );
		_tcscat( pszModelName, "_" );
		_tcscat( pszModelName, lpModelElem->m_szName );
	}
	if( lpModelElem->m_dwModelType != MODELTYPE_SFX )
		_tcscat( pszModelName, _T( ".o3d" ) );
	//_tcscat(pszModelName,".skin");
}
void CModelMng::GetSubString( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	_tcscpy( pszModelName, lpModelElem->m_szName );
}
void CModelMng::MakePartsName( TCHAR* pszPartsName, LPCTSTR lpszRootName, DWORD dwIndex, int nSex )
{
	LPMODELELEM lpModelElem = GetModelElem( OT_ITEM, dwIndex );
	_tcscpy( pszPartsName, lpszRootName );
	_tcscat( pszPartsName, _T( "_" ) );
	//_tcscat( pszPartsName, lpModelElem->m_szName );
	if( nSex == SEX_SEXLESS || nSex == -1 )
		_tcscat( pszPartsName, lpModelElem->m_szPart );
	else
	{
		TCHAR* pszPart2 = strchr( lpModelElem->m_szPart, '/' );
		if( pszPart2 == NULL )
			_tcscat( pszPartsName, lpModelElem->m_szPart );
		else
		{
			TCHAR szPartName[ 128 ];
			if( nSex == SEX_MALE )
			{
				_tcscpy( szPartName, lpModelElem->m_szPart );
				szPartName[ (int)pszPart2 - (int)lpModelElem->m_szPart ] = 0;
				_tcscat( pszPartsName, szPartName );
			}
			else
			{
				_tcscat( pszPartsName, pszPart2 + 1 );
			}
		}
	}
	_tcscat( pszPartsName, _T( ".o3d" ) );
}
void CModelMng::MakeMotionName( TCHAR* pszMotionName, DWORD dwType, DWORD dwIndex, DWORD dwMotion )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	if( lpModelElem == NULL )
		Error( "MakeMotionName실패 %s. Type = %d, Index = %d, Motion = %d", pszMotionName, dwType, dwIndex, dwMotion );

	// 초과됐을 경우 MTI_STAND(정지상태)로 강제 세팅 
	if( dwMotion >= (unsigned int)lpModelElem->m_nMax )
		dwMotion = MTI_STAND;
	_tcscpy( pszMotionName, szRoot[ dwType ] );
	_tcscat( pszMotionName, "_");
	//if( _tcschr( lpModelElem->m_motionAry[ dwMotion ], _T( '_' ) ) == NULL )
	//if( _tcschr( lpModelElem->m_aMotion.GetAt( dwMotion ), _T( '_' ) ) == NULL )

	if( _tcschr( lpModelElem->GetMotion( dwMotion ), _T( '_' ) ) == NULL )
	{
		_tcscat( pszMotionName, lpModelElem->m_szName );
		_tcscat( pszMotionName, "_");
	}
	//if( lpModelElem->m_motionAry[ dwMotion ][0] == 0 )
	//if( lpModelElem->m_aMotion.GetAt( dwMotion )[0] == 0 )
	if( lpModelElem->GetMotion( dwMotion )[0] == 0 )
	{
		// 공란일 경우 MTI_STAND(정지상태)로 강제 세팅 
		dwMotion = MTI_STAND;
		//_tcscat( pszMotionName, "GenStand" );
		//Error( "MakeMotionName에 %s가 강제로 GenStand를 만들었음. mdlDyna.inc에 Motion이 없다. Type = %d, Index = %d, Motion = %d", pszMotionName, dwType, dwIndex, dwMotion );
	}
	//_tcscat( pszMotionName, lpModelElem->m_motionAry[ dwMotion ] );
	//_tcscat( pszMotionName, lpModelElem->m_aMotion.GetAt( dwMotion ) );
	_tcscat( pszMotionName, lpModelElem->GetMotion( dwMotion ) );
	_tcscat( pszMotionName, ".ani" );
}
BOOL CModelMng::LoadMotion( CModel* pModel, DWORD dwType, DWORD dwIndex, DWORD dwMotion )
{
#ifdef _DEBUG
	if( dwIndex == 214 && dwMotion == 15 )
	{
		int a = 0;
	}
#endif
	if( dwType != OT_MOVER )
		return FALSE;
//	SFrame* pFrame;
//	TCHAR szModelName[128];
	TCHAR szMotionName[128];
//	MakeModelName( szModelName, dwType, dwIndex );
	MakeMotionName( szMotionName, dwType, dwIndex, dwMotion );

	//CModel* pModel = _pMotion = g_MotionMng.LoadMotion( strFileName );;
//	if(m_mapFileToMesh.Lookup(szModelName,(void*&)pd3dModel) == FALSE)
//		return FALSE;

	//switch( pModel->GetModelType() )
	//{
	//	case MODELTYPE_BIPED_MESH: 
			((CModelObject*)pModel)->LoadMotion( szMotionName );		// bone animation 읽음
	//		break;
	//}
	return TRUE;
}
CModel* CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts )
{
	LPMODELELEM lpModelElem = GetModelElem( nType, nIndex );
	if( lpModelElem == NULL ) 
	{
		Error( "CModelMng::loadModel mdlObj/mdlDyna 에 objtype=%d index=%d bpart=%d 의 정보가 없군여.", nType, nIndex, bParts );
		return NULL;
	}
	CModel* pModel = NULL;
	TCHAR szFileName[ 128 ];
	MakeModelName( szFileName, nType, nIndex );
	/*
	if( IsEmpty( lpModelElem->m_szTexture ) == FALSE )
	{
		D3DXCreateTextureFromFileEx( pd3dDevice, MakePath( DIR_MODELTEX, lpModelElem->m_szTexture ) , 
					  D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A1R5G5B5, 
					  D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					  D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0x00000000, NULL, NULL, &lpModelElem->m_pTexture );

	}
	*/
	return LoadModel( pd3dDevice, szFileName, lpModelElem, nType, bParts ); //lpModelElem[ nIndex ].m_dwModelType, lpModelElem[ nIndex ].m_szTexture );
}
CModel* CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, LPMODELELEM lpModelElem, int nType, BOOL bParts ) //int nModelType, LPCTSTR lpszTexture )
{
	HRESULT hr;
	CModel* pModel = NULL;
	CModel* pModelBill = NULL;
	int nModelType = lpModelElem->m_dwModelType;
	//LPMODELELEM lpModelElem = m_pModelElem[ nModelType ];
	TCHAR szFileName[128];
	MapStrToPtrItor mapItor;
	switch( nModelType )
	{
		case MODELTYPE_SFX: 
 #ifndef __WORLDSERVER		// 새버전에선 월드에서 sfx를 new하지 않는다.
	
//			pModel = new CSfxModel;		
//			pModel->SetModelType( nModelType );
//			((CSfxModel*)pModel)->SetSfx( lpszFileName );
//			pModel->m_pModelElem = lpModelElem;
//			pModel->m_pModelElem->m_bUsed = TRUE;
 #endif // not World
			break;
			/*
		case MODELTYPE_BILLBOARD: 
			mapItor = m_mapFileToMesh.find( lpszFileName );
			if( mapItor != m_mapFileToMesh.end() )
				return (CModel*) (*mapItor).second;
			pModel = CreateBillboard( pd3dDevice, lpszFileName, FALSE );
			pModel->SetModelType( nModelType );
			m_mapFileToMesh.insert( MapStrToPtrType( lpszFileName, pModel ) );
			pModel->m_pModelElem = lpModelElem;
			break;
			*/
			/*
		case MODELTYPE_ANIMATED_BILLBOARD: 
			pModel = CreateBillboard( pd3dDevice, lpszFileName, TRUE );
			pModel->SetModelType( nModelType );
			pModel->m_pModelElem = lpModelElem;
			break;
*/
		case MODELTYPE_MESH: 
#ifdef _DEBUG
			if( strcmp( lpszFileName, "obj_MaSaprBench01.o3d" ) == 0 )
			{
				int a = 0;
			}
#endif
			mapItor = m_mapFileToMesh.find( lpszFileName );
			if( mapItor != m_mapFileToMesh.end() )
			{
				((CModel*)(*mapItor).second)->m_pModelElem->m_bUsed = TRUE;
				return (CModel*) (*mapItor).second;
			}
#ifdef _DEBUG
			if( strcmp( lpszFileName, "obj_MaSaprBench01.o3d" ) == 0 )
			{
				int a = 0;
			}
#endif
			pModel = new CModelObject;
#ifdef _DEBUG
			if( pModel == (CModel*)0x11F54510 )
			{
				int a = 0;
			}
#endif
			pModel->SetModelType( nModelType );
			pModel->m_pModelElem = lpModelElem;
			//_tcscpy( szFileName, lpszFileName );
			//_tcscat( szFileName, ".o3d" );
			hr = pModel->InitDeviceObjects( pd3dDevice );
			hr = pModel->LoadModel( lpszFileName );
			if( hr == SUCCESS )
			{
				hr = pModel->RestoreDeviceObjects();
#ifdef _DEBUG
				if( ((CModelObject*)pModel)->GetObject3D()->m_nHavePhysique )
					Message( "CModelMng::LoadModel : %s가 동적오브젝트인데 정적오브젝트로 설정되어 있다.", lpszFileName );
#endif			
				m_mapFileToMesh.insert( MapStrToPtrType( lpszFileName, pModel ) );
				pModel->m_pModelElem->m_bUsed = TRUE;
				/*
				if( lpModelElem->m_szBillboard[ 0 ] )
				{
					// 스태틱 매쉬와 한쌍인 빌보드 읽기. 멀어졌을 경우 빌보드로 대체한다.
					mapItor = m_mapFileToMesh.find( lpModelElem->m_szBillboard );
					if( mapItor == m_mapFileToMesh.end() )
					{
						pModelBill = CreateBillboard( pd3dDevice, lpModelElem->m_szBillboard, FALSE );
						pModelBill->SetModelType( MODELTYPE_BILLBOARD );
						m_mapFileToMesh.insert( MapStrToPtrType( lpszFileName, pModelBill ) );
						pModel->m_pBillboard = pModelBill;
					}
				}*/
			}
			else
				SAFE_DELETE( pModel )
			break;
		case MODELTYPE_ANIMATED_MESH:
			pModel = new CModelObject;
			pModel->SetModelType( nModelType );
			pModel->InitDeviceObjects( pd3dDevice );
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
			// Object Type이 Mover면 검광을 생성??
//			_tcscpy( szFileName, lpszFileName );
//			_tcscat( szFileName, ".bon" );
//			((CModelObject*)pModel)->OpenBone( szFileName );
			if( nType == OT_MOVER )	// 무버는 외장본(.chr)을 먼저로딩해야한다.  여기다 넣어도 되는건지 몰겠군 -_-;
			{
				memset( szFileName, 0, sizeof(szFileName) );
				_tcsncpy( szFileName, lpszFileName, _tcslen( lpszFileName ) - 4 );	// .o3d를 떼고 파일명부분만 카피
				_tcscat( szFileName, _T(".chr") );
				((CModelObject*)pModel)->LoadBone( szFileName );
			}
			if( bParts == FALSE )
			{
				//_tcscpy( szFileName, lpszFileName );
				//_tcscat( szFileName, ".o3d" );
				if( ((CModelObject*)pModel)->LoadModel( lpszFileName ) == SUCCESS ) //Element(  szFileName, PARTS_UPPER_BODY ); // skin 읽음
				{
					((CModelObject*)pModel)->RestoreDeviceObjects();
				}
			}
			break;
	}
	return pModel;
}
/*
BOOL CModelMng::LoadXFile(LPDIRECT3DDEVICE9 pd3dDevice,TCHAR* lpszFileName,int nModelType)
{
		CModel* pd3dModel;
	if(m_mapFileToMesh.Lookup(lpszFileName,(void*&)pd3dModel) == TRUE)
		return TRUE;
	if(nModelType == MODELTYPE_STATIC_MESH)
	{
		pd3dModel = new CD3DMesh;
		((CD3DMesh*)pd3dModel)->Create(pd3dDevice,lpszFileName);
	}
	else
	if(nModelType == MODELTYPE_BIPED_MESH)
	{
		pd3dModel = new CSkinMesh;
		((CSkinMesh*)pd3dModel)->Load(lpszFileName);
	}
	m_mapFileToMesh.SetAt(lpszFileName,pd3dModel);
	return TRUE;
}
*/
HRESULT CModelMng::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT CModelMng::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
	MapStrToPtrItor itor;
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
		((CModel*)(*itor).second)->RestoreDeviceObjects();
	return hr;
}
HRESULT CModelMng::InvalidateDeviceObjects()
{
	HRESULT hr = S_OK;
	MapStrToPtrItor itor;
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
		((CModel*)(*itor).second)->InvalidateDeviceObjects();
	return hr;
}
HRESULT CModelMng::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	MapStrToPtrItor itor;
#ifdef __WORLDSERVER
//	CMclAutoLock	Lock( m_AccessLock );
	m_AccessLock.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
	{
		((CModel*)(*itor).second)->DeleteDeviceObjects();
		CModel* pModel = ( CModel* )(*itor).second;
		SAFE_DELETE( pModel );
	}
	m_mapFileToMesh.clear();
/*
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
		((CModel*)(*itor).second)->DeleteDeviceObjects();
		*/
//	m_mapFileToMesh.clear(); 이건 머지 파괴자에서 해야할 일을 왜 여기서..
#ifdef __WORLDSERVER
	m_AccessLock.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
	return hr;
/*
	HRESULT hr;
	POSITION pos = m_mapFileToMesh.GetStartPosition();
	CString strFileName;
	CModel* pModel;
	while(pos)
	{
		m_mapFileToMesh.GetNextAssoc( pos, strFileName, (void*&)pModel );
		pModel->DeleteDeviceObjects();
	}
#ifdef __WORLDSERVER
	m_AccessLock.Leave();	// unlock1
#endif	// __WORLDSERVER
	return hr;
*/
}
BOOL CModelMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
#ifdef __BEAST
	CPtrArray* apModelElem;// = (LPMODELELEM) //CFixedArray< tagMODELELEM >* apModelElem; 
#else
	CFixedArray< tagMODELELEM >* apModelElem; 
#endif
	TCHAR szObject[48];
	TCHAR szMotion[48];
	UINT iType, iObject;
	int iMotion = 0;
	MODELELEM modelElem;
	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		iType = script.GetNumber();
		apModelElem = &m_aaModelElem[ iType ];
		script.GetToken(); // {
		script.GetToken(); // object name or }
		int nBrace = 1;
		// 여기부터 오브젝트 단위 obj, ctrl, item, sfx, mover
		while( nBrace )
		{
			ZeroMemory( &modelElem, sizeof( modelElem ) );
			if( *script.token == '}' ) 
			{
				nBrace--;
				if( nBrace > 0 )
				{
					script.GetToken(); // object name or }
					continue;
				}
				if( nBrace == 0 )
					continue;
			}
#ifdef _DEBUG
			if( sizeof(szObject) <= strlen(script.token) + 1 )
				Message( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
#endif
				
			_tcscpy( szObject, script.token ); // folder 또는 object name
			script.SetMark();
			// 스크립트 실수 검출 루틴 필요. - xuzhu -
			script.GetToken(); // {
			if( *script.token == '{' )
			{
				nBrace++;
				script.GetToken(); // object name or }
#ifdef _DEBUG
				if( sizeof(szObject) <= strlen(script.token) + 1 )
					Message( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
#endif
				
				_tcscpy( szObject, script.token ); // folder 또는 object name
				continue;
			}
			else
				script.GoMark();
			iObject = script.GetNumber();
			if( iObject == 0 )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) 0으로 지정된 모션 아이디 : %s, %s", script.GetLineNum(), szObject, script.token );
//				str.Format( "%d", script.GetLineNum() );
				AfxMessageBox( str );
			}
			modelElem.m_dwType = iType;
			modelElem.m_dwIndex = iObject;
#ifdef _DEBUG
			if( sizeof(modelElem.m_szName) <= strlen(szObject) + 1 )
				Message( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(szObject) );
#endif
			_tcscpy( modelElem.m_szName, szObject );
			modelElem.m_dwModelType = script.GetNumber();
			script.GetToken();
#ifdef _DEBUG
			if( sizeof(modelElem.m_szPart) <= strlen(script.token) + 1 )
				Message( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
#endif
			_tcscpy( modelElem.m_szPart, script.Token );
			modelElem.m_bFly = script.GetNumber();
			modelElem.m_dwDistant = script.GetNumber();
			modelElem.m_bPick = script.GetNumber();
			modelElem.m_fScale = script.GetFloat();
			modelElem.m_bTrans = script.GetNumber();
			modelElem.m_bShadow = script.GetNumber();			
#ifdef __XTEXTURE_EX2
			modelElem.m_bReserved = 0;
			modelElem.m_nTextureEx = script.GetNumber();
#else
			modelElem.m_bTextureEx = script.GetNumber();
#endif
	
	#ifdef __YRENDER_FLAG			
			//if( iType == 3 )
				modelElem.m_bRenderFlag = script.GetNumber();
			//else
			//	modelElem.m_bRenderFlag = 1;
	#endif //__YRENDER_FLAG

			// 최대 크기를 저장 
			//if( iObject >= m_modelElemNum[ iType ] )
			//	m_modelElemNum[ iType ] = iObject + 1;
			script.GetToken(); // object name or { or }
			// 여기부터 애니메이션 
			if( *script.token == '{' )
			{
				script.SetMark();
				script.GetToken(); // motion name or }
				int nMax = 0;
				// 모션 리스트 카운트 
				while( *script.token != '}' )
				{
#ifdef _DEBUG
					if( sizeof(szMotion) <= strlen(script.token) + 1 )
						Message( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
#endif
					_tcscpy( szMotion, script.token );
					iMotion = script.GetNumber();
					if( iMotion > nMax )
						nMax = (int)iMotion;
					script.GetToken(); // motion name or }
				}
				nMax++;
				script.GoMark();
				// 실제 모션 리스트 세팅 
				script.GetToken(); // motion name or }
				modelElem.m_apszMotion = new TCHAR[ nMax * 32 ];
				modelElem.m_nMax = nMax;
				ZeroMemory( modelElem.m_apszMotion, sizeof( TCHAR ) * nMax * 32 );
				TRACE( " %s %p\n", modelElem.m_szName, modelElem.m_apszMotion);
				while( *script.token != '}' )
				{
#ifdef _DEBUG
					if( sizeof(szMotion) <= strlen(script.token) + 1 )
						Message( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
#endif
					_tcscpy( szMotion, script.token );
					iMotion = script.GetNumber();
					TCHAR* lpszMotion = modelElem.GetMotion( iMotion );
					if( lpszMotion[0] )
					{
						CString str;
						str.Format( "CModelMng::LoadScript(%d) %s모션 중복 아이디 : %s", script.GetLineNum(), lpszFileName, lpszMotion );
						AfxMessageBox( str );
					}
					_tcscpy( lpszMotion, szMotion );
					script.GetToken(); // motion name or }
				}
				script.GetToken(); // object name or }
			}
#ifdef __BEAST
			LPMODELELEM pModelElem = new MODELELEM;
			memcpy( pModelElem, &modelElem, sizeof( modelElem ) );
			apModelElem->SetAtGrow( iObject, pModelElem );	
#else
			if( apModelElem->GetAt( iObject ) )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) %s중복 아이디 : type = %d, idx = %d, name = %s", script.GetLineNum(), lpszFileName, iType, iObject, modelElem.m_szName );
				AfxMessageBox( str );
			}
//			TRACE( "%d\n", iObject );
			apModelElem->SetAtGrow( iObject, &modelElem );
			ZeroMemory( &modelElem, sizeof( modelElem ) );		// 이거 클리어를 안시켜서 이전값이 계속 남아있었음.  xuzhu추가.
#endif
		}
		script.GetToken(); // type name or }
#ifndef __BEAST
		apModelElem->Optimize();
#endif
	}
	return TRUE;
}
BOOL CModelMng::LoadBillboard( LPCTSTR lpszFileName )
{
	return FALSE;
	CScript script;
	if(script.Load(lpszFileName)==FALSE)
		return FALSE;

	LPBILLBOARD lpBillboard;
	// 스킵 label
	script.SkipToken( 16, TRUE );

	script.GetNumber( TRUE );  // 파일 
	while(script.tok!=FINISHED)
	{
		lpBillboard = new BILLBOARD;
		_tcscpy( lpBillboard->m_lpszFileName, script.token );
		script.GetToken( TRUE );  // 이름 
		_tcscpy( lpBillboard->m_lpszName, script.token );
		lpBillboard->rect.left   = script.GetNumber( TRUE );  // left
		lpBillboard->rect.top    = script.GetNumber( TRUE );  // top
		lpBillboard->rect.right  = script.GetNumber( TRUE );  // right
		lpBillboard->rect.bottom = script.GetNumber( TRUE );  // bottom
		lpBillboard->ptCenter.x  = script.GetNumber( TRUE );  // xCenter
		lpBillboard->ptCenter.y  = script.GetNumber( TRUE );  // yCenter
		lpBillboard->dwColor     = script.GetNumber( TRUE );  // red
		lpBillboard->dwColor     |= script.GetNumber( TRUE ) << 8;  // green
		lpBillboard->dwColor     |= script.GetNumber( TRUE ) << 16;  // bluie
		lpBillboard->bAnimation  = script.GetNumber( TRUE );  
		lpBillboard->dwAniMethod = script.GetNumber( TRUE );  
		lpBillboard->dwFrame     = script.GetNumber( TRUE );  
		lpBillboard->bEffect     = script.GetNumber( TRUE );  
		lpBillboard->fScale      = script.GetFloat ( TRUE );  

		lpBillboard->m_pBillboard = NULL;
		//m_mapFileToBillboard.SetAt( lpBillboard->m_lpszName, lpBillboard );
		m_mapFileToBillboard.insert( MapStrToPtrType( lpBillboard->m_lpszName, lpBillboard ) );
		script.GetNumber( TRUE );  // 파일 
	}
	return TRUE;
}
CModel* CModelMng::CreateBillboard( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName, BOOL bAnimation )
{
	return NULL;
}





	//CString GetNewModelDefine( DWORD dwType );
	//DWORD GetNewModelId( DWORD dwType );
	//BOOL RemoveModel( DWORD dwType, DWORD dwId );
	//BOOL AddModel( LPMODELELEM pModelElem );
#ifdef __BEAST
CString CModelMng::GetNewModelDefine( DWORD dwType )
{
	CString string;
	/*
	int i = 0;
BACK:
	string.Format( _T( "APP_APPLET%d" ), i++ );

	POSITION pos 
		= CScript::m_globalDef.GetStartPosition();
	CString strTemp;
	TCHAR* pData;
	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, strTemp, (void*&)pData );
		if( strTemp == string )
			goto BACK;
	}
	*/
	return string;
}

	
DWORD CModelMng::GetNewModelId( DWORD dwType )
{
	CPtrArray* apModelElem = &m_aaModelElem[ dwType ];
	// 일반 Obj Id는 비어있는 구멍을 찾아서 돌려준다.
	int i;
//	if( dwType == OT_OBJ )
	{
		for( i = 0; i < apModelElem->GetSize(); i++ )
		{
			LPMODELELEM pModelELem = (LPMODELELEM)apModelElem->GetAt( i );
			if( i >= BEGIN_MODEL_ID && pModelELem == NULL ) return i;
		}
	}
	/*
	POSITION pos 
		= CScript::m_globalDef.GetStartPosition();
	CDWordArray idAppletArray;
	CString string;
	TCHAR* pData;
	DWORD dwId;
	while(pos)
	{ 
		CScript::m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( _T( "APP_" ) ) == 0 )
		{
			dwId = _ttoi( pData ); 
			// dwId의 값을 id로 갖고 있는 Applet을 찾는다.
			//if( GetAt( dwId ) )
				idAppletArray.SetAtGrow( dwId - BEGIN_MODEL_ID, 1 );
		}
	}	
	for( int i = 0; i < idAppletArray.GetSize(); i++ )
	{
		if( idAppletArray.GetAt( i ) == 0 )
			return i + BEGIN_MODEL_ID;
	}
	return i + BEGIN_MODEL_ID;
	*/ return i;
}

BOOL CModelMng::RemoveModel( DWORD dwType, DWORD dwId )
{
	CPtrArray* apModelElem = &m_aaModelElem[ dwType ];
	LPMODELELEM pModelElem = (LPMODELELEM)apModelElem->GetAt( dwId );
	SAFE_DELETE( pModelElem );
	apModelElem->SetAt( dwId, NULL );

	//CFixedArray< tagMODELELEM >* apModelElem = &m_aaModelElem[ dwType ];
	//apModelElem->RemoveAt( dwId );

	/*
	LPWNDAPPLET lpWndApplet;
	if( m_mapWndApplet.Lookup( (void*) dwAppletId, (void*&) lpWndApplet ) )
	{
		// Applet 제거 
		SAFE_DELETE( lpWndApplet );
		m_mapWndApplet.RemoveKey( (void*)dwAppletId );
		// 삭제된 Id를 스텍에 넣어서 보관
		//m_wndAppletIdStack.Push( dwAppletId );
		// Define에서 제거 
		//CString string =
		//	CScript::GetFindIdStr( _T( "APP_" ), dwAppletId );
		//CScript::RemoveDefine( string );
		return TRUE;
	}
	*/
	return FALSE;
}
BOOL CModelMng::AddModel( DWORD dwType, LPMODELELEM pModelElem )
{
	CPtrArray* apModelElem	= &m_aaModelElem[ dwType ];
	for( int i = 0; i < apModelElem->GetSize(); i++ )
	{
		if( apModelElem->GetAt( i ) && stricmp( ((LPMODELELEM)apModelElem->GetAt( i ))->m_szName, pModelElem->m_szName ) == 0 )
		{
			return FALSE;
		}		
	}
	//CFixedArray< tagMODELELEM >* apModelElem = &m_aaModelElem[ dwType ];
	LPMODELELEM pNewModelElem = new MODELELEM;
	memcpy( pNewModelElem, pModelElem, sizeof( MODELELEM ) );
	apModelElem->SetAtGrow( pModelElem->m_dwIndex, pNewModelElem );

	// define이 이미 정의되어 있나?
//	LPSTR lpDefData = CScript::GetDefineStr( lpszDefine );
//	if( lpDefData ) return FALSE;
/*
	LPWNDAPPLET pWndApplet = new WNDAPPLET;
	//ZeroMemory( pWndApplet, sizeof( WNDAPPLET ) );
	pWndApplet->pWndBase = pWndApp->pWndBase;
	pWndApplet->dwWndId = pWndApp->dwWndId;
	pWndApplet->size = pWndApp->size;
	pWndApplet->dwWndStyle = pWndApp->dwWndStyle;
	pWndApplet->strTitle = pWndApp->strTitle;
	pWndApplet->strDefine = pWndApp->strDefine;

	// 새 아이디 할당 
	//DWORD dwNewId = GetNewAppletId();
	//pWndApplet->dwWndId = dwNewId;//GetNewAppletId();

	// Define을 추가 
	TCHAR* szDefData = new TCHAR[ 32 ];
	_itot( pWndApplet->dwWndId, szDefData, 10 );
	CScript::AddDefine( pWndApplet->strDefine, szDefData );

	// Map에 추가. 정식 등록 
	m_mapWndApplet.SetAt( (void*)pWndApplet->dwWndId, pWndApplet );
	*/
	return TRUE;
}
#endif