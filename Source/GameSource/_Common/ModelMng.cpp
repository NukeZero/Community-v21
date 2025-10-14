#include "stdafx.h"
#include "ModelMng.h"
#include "SeasonEffect.h"


TCHAR*	MODELELEM::GetMotion( int i ) 
{ 
	if( i < 0 || i >= m_nMax )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "out of range %d" ), i );
		i = 0;
	}

	if( m_apszMotion ) 
		return &m_apszMotion[ i * 32 ]; 
	return NULL; 
}

TCHAR*	MODELELEM::getName()
{
	if( m_variableSeasonFile == true ) {
		const int season	= g_pSeasonEffect->getSeason();
		std::string fileName( m_szName );
		if( nsSeasonFile::changeVariableSeasonFile( season, m_dwIndex, fileName ) == true ) {
			FLStrcpy( m_szName, _countof( m_szName ), fileName.c_str() );
			m_fScale	= 1.0f;
		}

		m_variableSeasonFile	= false;
	}

	return m_szName;
}

void	MODELELEM::setName( const TCHAR* fileName )
{
	FLStrcpy( m_szName, _countof( m_szName ), fileName );

	m_variableSeasonFile	= m_dwType == OT_OBJ ? nsSeasonFile::isVariableSeasonFile( m_dwIndex ) : false;
}


static TCHAR g_szRoot[ MAX_OBJTYPE ][32] = { 
	"obj", 
	"ani", 
	"ctrl", 
	"sfx", 
	"item", 
	"mvr", 
	"region",
	"obj",		// ship
	"path"
};

CModelMng::CModelMng()
{
}
CModelMng::~CModelMng()
{
	for( int i = 0; i < MAX_OBJTYPE ; i++ )
	{
		CIndexArray< tagMODELELEM >* apModelElem = &m_aaModelElem[ i ];
		for( int j = 0; j < apModelElem->GetSize(); j++ )
		{
			LPMODELELEM pModelElem = (LPMODELELEM) apModelElem->GetAt( j );
			if( pModelElem )
				SAFE_DELETE_ARRAY( pModelElem->m_apszMotion );
		}
	}

	Free();
}
void CModelMng::Free()
{
}
LPMODELELEM CModelMng::GetModelElem( DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM pElem = m_aaModelElem[ dwType ].GetAt( dwIndex );
	if( pElem == NULL )
	{
		if( dwType == OT_ITEM )	// 아이템인 경우, 돈 모델을 사용   
//			return m_aaModelElem[ OT_ITEM ].GetAt( ITEM_INDEX( 12, II_GOLD_SEED1 ) );
			return m_aaModelElem[ OT_ITEM ].GetAt( ITEM_INDEX( 500, II_ARM_M_VAG_HELMET02 ) );	// 일반 방어구 모양으로 교체
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "out of range: type=%d, size=%d, index=%d" ), (int)dwType, m_aaModelElem[ dwType ].GetSize(), (int)dwIndex );
		}
	}
	
	return pElem;
}
void CModelMng::MakeBoneName( TCHAR* pszModelName, size_t cchModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	FLStrcpy( pszModelName, cchModelName, g_szRoot[dwType]);
	FLStrcat( pszModelName, cchModelName, "_" );
	FLStrcat( pszModelName, cchModelName, lpModelElem->getName() );
	FLStrcat( pszModelName, cchModelName, _T(".chr") );
}
void CModelMng::MakeModelName( TCHAR* pszModelName, size_t cchModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	if( lpModelElem == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "dwType=%d dwIndex=%d" ), dwType, dwIndex );
	}
	if( lpModelElem->m_dwModelType == MODELTYPE_BILLBOARD )
	{
		FLStrcpy( pszModelName, cchModelName, lpModelElem->getName() );
		return;
	}
	if( dwType == OT_SFX )
	{
		if( _tcschr( lpModelElem->getName(), _T( '_' ) ) == NULL )
		{
			FLStrcpy( pszModelName, cchModelName, g_szRoot[ dwType ] );
			FLStrcat( pszModelName, cchModelName, "_" );
			FLStrcat( pszModelName, cchModelName, lpModelElem->getName() );
		}
		else
			FLStrcpy( pszModelName, cchModelName, lpModelElem->getName() );
	}
	else
	{
		FLStrcpy( pszModelName, cchModelName, g_szRoot[ dwType ] );
		FLStrcat( pszModelName, cchModelName, "_" );
		FLStrcat( pszModelName, cchModelName, lpModelElem->getName() );
	}
	if( lpModelElem->m_dwModelType != MODELTYPE_SFX )
		FLStrcat( pszModelName, cchModelName, _T( ".o3d" ) );
}
void CModelMng::MakePartsName( TCHAR* pszPartsName, size_t cchPartsName, LPCTSTR lpszRootName, DWORD dwIndex, int nSex )
{
	LPMODELELEM lpModelElem = GetModelElem( OT_ITEM, dwIndex );
	FLStrcpy( pszPartsName, cchPartsName, lpszRootName );
	FLStrcat( pszPartsName, cchPartsName, _T( "_" ) );
	if( nSex == SEX_SEXLESS || nSex == -1 )
		FLStrcat( pszPartsName, cchPartsName, lpModelElem->m_szPart );
	else
	{
		TCHAR* pszPart2 = strchr( lpModelElem->m_szPart, '/' );
		if( pszPart2 == NULL )
			FLStrcat( pszPartsName, cchPartsName, lpModelElem->m_szPart );
		else
		{
			TCHAR szPartName[ 128 ];
			if( nSex == SEX_MALE )
			{
				FLStrcpy( szPartName, _countof( szPartName ), lpModelElem->m_szPart );
				szPartName[ (int)pszPart2 - (int)lpModelElem->m_szPart ] = 0;
				FLStrcat( pszPartsName, cchPartsName, szPartName );
			}
			else
			{
				FLStrcat( pszPartsName, cchPartsName, pszPart2 + 1 );
			}
		}
	}
	FLStrcat( pszPartsName, cchPartsName, _T( ".o3d" ) );
}
void CModelMng::MakeMotionName( TCHAR* pszMotionName, size_t cchMotionName, DWORD dwType, DWORD dwIndex, DWORD dwMotion )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	if( lpModelElem == NULL )
		FLERROR_LOG( PROGRAM_NAME, _T( "GetModelElem dwType:%d dwIndex:%d, dwMotion:%d" ), dwType, dwIndex, dwMotion );

	// 초과됐을 경우 MTI_STAND(정지상태)로 강제 세팅 
	if( (int)( dwMotion ) >= lpModelElem->m_nMax || dwMotion == NULL_ID )
	{
		dwMotion = MTI_STAND;
	}

	FLStrcpy( pszMotionName, cchMotionName, g_szRoot[ dwType ] );
	FLStrcat( pszMotionName, cchMotionName, "_" );

	LPCTSTR lpszMotion = lpModelElem->GetMotion( dwMotion ); 
	if( _tcschr( lpszMotion, '_' ) == NULL )
	{
		FLStrcat( pszMotionName, cchMotionName, lpModelElem->getName() );
		FLStrcat( pszMotionName, cchMotionName, "_" );
	}

	// 공란일 경우 MTI_STAND(정지상태)로 강제 세팅 
	if( lpszMotion[0] == 0 )
	{
		dwMotion = MTI_STAND;
		lpszMotion = lpModelElem->GetMotion( dwMotion ); 
	}

	FLStrcat( pszMotionName, cchMotionName, lpszMotion );
	FLStrcat( pszMotionName, cchMotionName, ".ani" );
}
BOOL CModelMng::LoadMotion( CModel* pModel, DWORD dwType, DWORD dwIndex, DWORD dwMotion )
{
	if( dwType != OT_MOVER )
		return FALSE;
	TCHAR szMotionName[ MAX_PATH ];
	MakeMotionName( szMotionName, _countof( szMotionName ), dwType, dwIndex, dwMotion );

	((CModelObject*)pModel)->LoadMotion( szMotionName );		// bone animation 읽음
	return TRUE;
}
CModel* CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts )
{
	LPMODELELEM lpModelElem = GetModelElem( nType, nIndex );
	if( lpModelElem == NULL ) 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "mdlObj/mdlDyna 에 objtype=%d index=%d bpart=%d 의 정보가 없군여." ), nType, nIndex, bParts );
		return NULL;
	}

	TCHAR szFileName[ MAX_PATH ];
	MakeModelName( szFileName, _countof( szFileName ), nType, nIndex );
	return LoadModel( pd3dDevice, szFileName, lpModelElem, nType, bParts ); 
}
CModel* CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, LPMODELELEM lpModelElem, int nType, BOOL bParts ) 
{
	//safety
	if( NULL == lpModelElem )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "lpModelElem is NULL" ) );
		return NULL;
	}

	HRESULT hr;
	CModel* pModel = NULL;
//	CModel* pModelBill = NULL;
	int nModelType = lpModelElem->m_dwModelType;
	
	MapStrToPtrItor mapItor;

	switch( nModelType )
	{
		case MODELTYPE_SFX: 
		#ifndef __WORLDSERVER		// 새버전에선 월드에서 sfx를 new하지 않는다.
			pModel = new CSfxModel;		
			pModel->SetModelType( nModelType );
			((CSfxModel*)pModel)->SetSfx( lpszFileName );
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
		#endif // not World
			break;

		case MODELTYPE_MESH: 
			mapItor = m_mapFileToMesh.find( lpszFileName );
			if( mapItor != m_mapFileToMesh.end() )
			{
				pModel = (CModel*)mapItor->second;
				if( pModel )
				{
					if( pModel->m_pModelElem )
						pModel->m_pModelElem->m_bUsed = TRUE;
					else 
					{
						FLASSERT( 0 && "m_pModelElem is NULL" );
						FLERROR_LOG( PROGRAM_NAME, _T( "pModel->m_pModelElem is NULL" ) );
					}
				}

				return pModel; 
			}
			pModel = new CModelObject;
			pModel->SetModelType( nModelType );
			pModel->m_pModelElem = lpModelElem;
			hr = pModel->InitDeviceObjects( pd3dDevice );
			hr = pModel->LoadModel( lpszFileName );
			if( hr == SUCCESS )
			{
				hr = pModel->RestoreDeviceObjects();
			#ifdef _DEBUG
				if( ((CModelObject*)pModel)->GetObject3D()->m_nHavePhysique )
					FLERROR_LOG( PROGRAM_NAME, _T( "%s가 동적오브젝트인데 정적오브젝트로 설정되어 있다." ), lpszFileName );
			#endif			
				bool bResult = m_mapFileToMesh.insert( MapStrToPtrType( lpszFileName, pModel ) ).second;
				if( bResult == true )
				{
					pModel->m_pModelElem->m_bUsed = TRUE;
				}
				else
				{
					SAFE_DELETE( pModel );
				}
			}
			else
			{
				SAFE_DELETE( pModel )
			}
			break;
		case MODELTYPE_ANIMATED_MESH:
			pModel = new CModelObject;
			pModel->SetModelType( nModelType );
			pModel->InitDeviceObjects( pd3dDevice );
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
			//mem_set( szFileName, 0, sizeof(szFileName) );
			TCHAR szFileName[MAX_PATH]= { 0 };
			FLStrncpy( szFileName, _countof( szFileName ), lpszFileName, _tcslen( lpszFileName ) - 4 );	// .o3d를 떼고 파일명부분만 카피
			FLStrcat( szFileName, _countof( szFileName ), _T(".chr") );
			switch( nType )
			{
			case OT_ITEM:	// 아이템일 경우, 외장본(.chr)이 있다면 로딩한다 (예: 날개)
				{
					CResFile resFp;
					BOOL bResult = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
					if( bResult == TRUE )
						((CModelObject*)pModel)->LoadBone( szFileName );
					break;
				}
			case OT_MOVER:	// 무버일 경우 외장본(.chr)을 로딩한다
				{
					((CModelObject*)pModel)->LoadBone( szFileName );
					break;
				}
			}

			if( bParts == FALSE )
			{
				if( ((CModelObject*)pModel)->LoadModel( lpszFileName ) == SUCCESS )  // skin 읽음
				{
					((CModelObject*)pModel)->RestoreDeviceObjects();
				}
			}
			break;
	}
	return pModel;
}
HRESULT CModelMng::InitDeviceObjects( LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT CModelMng::RestoreDeviceObjects( LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
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
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
	{
		((CModel*)(*itor).second)->DeleteDeviceObjects();
		CModel* pModel = ( CModel* )(*itor).second;
		SAFE_DELETE( pModel );
	}
	m_mapFileToMesh.clear();
	return hr;
}
BOOL CModelMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;

	CIndexArray< tagMODELELEM >* apModelElem; 
	TCHAR szObject[48];
	TCHAR szMotion[48];
	UINT iType, iObject, iMotion;
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
			if( _countof(szObject) <= strlen(script.token) + 1 )
				FLERROR_LOG( PROGRAM_NAME, _T( "%s 스트링의 길이가 너무길다. %d" ), lpszFileName, strlen(script.token) );
		#endif

			FLStrcpy( szObject, _countof( szObject ), script.token ); // folder 또는 object name

			script.SetMark();
			// 스크립트 실수 검출 루틴 필요. - xuzhu -
			script.GetToken(); // {
			if( *script.token == '{' )
			{
				nBrace++;
				script.GetToken(); // object name or }
			#ifdef _DEBUG
				if( _countof(szObject) <= strlen(script.token) + 1 )
					FLERROR_LOG( PROGRAM_NAME, _T( "%s 스트링의 길이가 너무길다. %d" ), lpszFileName, strlen(script.token) );
			#endif

				FLStrcpy( szObject, _countof( szObject ), script.token ); // folder 또는 object name
				continue;
			}
			else
				script.GoMark();
			iObject = script.GetNumber();
			if( iObject == 0 )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) 0으로 지정된 모션 아이디 : %s, %s", script.GetLineNum(), szObject, script.token );
				AfxMessageBox( str );
			}
			modelElem.m_dwType = iType;
			modelElem.m_dwIndex = iObject;
		#ifdef _DEBUG
			if( maxFileNameLength <= strlen(szObject) + 1 )
				FLERROR_LOG( PROGRAM_NAME, _T( "%s 스트링의 길이가 너무길다. %d" ), lpszFileName, strlen(szObject) );
		#endif
			modelElem.setName( szObject );
			modelElem.m_dwModelType = script.GetNumber();
			script.GetToken();
		#ifdef _DEBUG
			if( maxFileNameLength <= strlen(script.token) + 1 )
				FLERROR_LOG( PROGRAM_NAME, _T( "%s 스트링의 길이가 너무길다. %d" ), lpszFileName, strlen(script.token) );
		#endif
			FLStrcpy( modelElem.m_szPart, _countof( modelElem.m_szPart ), script.Token );
			modelElem.m_bFly = script.GetNumber();
			modelElem.m_dwDistant = script.GetNumber();
			modelElem.m_bPick = script.GetNumber();
			modelElem.m_fScale = script.GetFloat();
			modelElem.m_bTrans = script.GetNumber();
			modelElem.m_bShadow = script.GetNumber();			
			modelElem.m_bReserved = 0;
			modelElem.m_nTextureEx = script.GetNumber();
			modelElem.m_bRenderFlag = script.GetNumber();

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
					if( _countof(szMotion) <= strlen(script.token) + 1 )
						FLERROR_LOG( PROGRAM_NAME, _T( "%s 스트링의 길이가 너무길다. %d" ), lpszFileName, strlen(script.token) );
					FLStrcpy( szMotion, _countof( szMotion ), script.token );
					iMotion = script.GetNumber();
					if( (int)( iMotion ) > nMax )
						nMax = iMotion;
					script.GetToken(); // motion name or }
				}
				nMax++;
				script.GoMark();
				// 실제 모션 리스트 세팅 
				script.GetToken(); // motion name or }
				modelElem.m_apszMotion = new TCHAR[ nMax * 32 ];
				modelElem.m_nMax = nMax;
				ZeroMemory( modelElem.m_apszMotion, sizeof( TCHAR ) * nMax * 32 );
				//FLTRACE_LOG( PROGRAM_NAME, _T( " %s %p" ), modelElem.m_szName, modelElem.m_apszMotion);
				while( *script.token != '}' )
				{
					if( _countof(szMotion) <= strlen(script.token) + 1 )
						FLERROR_LOG( PROGRAM_NAME, _T( "%s 스트링의 길이가 너무길다. %d" ), lpszFileName, strlen(script.token) );
					FLStrcpy( szMotion, _countof( szMotion ), script.token );
					iMotion = script.GetNumber();
					TCHAR* lpszMotion = modelElem.GetMotion( iMotion );
					if( lpszMotion[0] )
					{
						CString str;
						str.Format( "CModelMng::LoadScript(%d) %s모션 중복 아이디 : %s", script.GetLineNum(), lpszFileName, lpszMotion );
						AfxMessageBox( str );
					}
					FLStrcpy( lpszMotion, 32, szMotion );
					script.GetToken(); // motion name or }
				}
				script.GetToken(); // object name or }
			}
			if( apModelElem->GetAt( iObject ) )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) %s중복 아이디 : type = %d, idx = %d, name = %s", script.GetLineNum(), lpszFileName, iType, iObject, modelElem.getName() );
				AfxMessageBox( str );
			}

//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
			
#ifdef __WORLDSERVER
			if( iType != OT_SFX )	// sfx는 서버에서 skip
				apModelElem->SetAt( iObject, &modelElem );
#else
			apModelElem->SetAt( iObject, &modelElem );
#endif

		} // while( nBrace )

		script.GetToken(); // type name or }
		apModelElem->Optimize();
	}	// while( script.tok != FINISHED )

	return TRUE;
}

