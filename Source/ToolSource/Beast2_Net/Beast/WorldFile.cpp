#include "stdafx.h"  
#include "defineObj.h"
#include "region.h"
#include "WorldMng.h"
#include "lang.h"

#ifdef __CSC_WORLDOBJ_LIST
#include "WorldTreeView.h"
#endif //__CSC_WORLDOBJ_LIST

extern	CWorldMng	g_WorldMng;
#ifdef	__YENV	
extern float s_fFogStart;
extern float s_fFogEnd;
#endif //__YENV

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define CURVER_WORLD "WORLD v0.00001"
//#define OLDVER_WORLD "WORLD v0.00001"
#define ADDPOS_X 0//( 1024 * MPU )
#define ADDPOS_Y 0//( 0 )
#define ADDPOS_Z 0//( 256 * MPU )

void DivPathAndName( LPCTSTR lpszPathName, TCHAR* lpszPath, TCHAR* lpszName )
{
	CString strFileName = lpszPathName;
	TCHAR szCurDirTemp[ 128 ];
	int nLength = strlen( lpszPathName );

	if( nLength > 0 )
	{
		// FULL PATH라면 파일명과 directory를 나눈다.
		for( int i = nLength - 1; i ; i--)
		{
			if( lpszPathName[ i ] == '\\' )
			{
				strcpy( szCurDirTemp, strFileName );
				szCurDirTemp[ i + 1 ] = '\0';
				if( lpszPath )
					strcpy( lpszPath, szCurDirTemp ); 
				strFileName = strFileName.Right( nLength - ( i + 1 ));
				if( lpszName )
					strcpy( lpszName, strFileName );
				return;
			}
		}
	}
}
void MakeFullPathName( LPCTSTR lpszFileName, TCHAR* lpszPathName )
{
	// Full Path이므로 관련없음 
	if( strchr( lpszFileName, ':' ) )
		strcpy( lpszPathName, lpszFileName );
	else
	{
		GetCurrentDirectory( 128, lpszPathName );
		strcat( lpszPathName, "\\" );
		strcat( lpszPathName, lpszFileName );
	}
}
#ifdef __BEAST
BOOL CWorld::OpenBmpWorld( LPCTSTR lpszFileName, BOOL bDir )
{
	CFile file;
	if (! file.Open(lpszFileName, CFile::modeRead | CFile::shareDenyWrite)) return FALSE;

    BOOL bIsPM = FALSE;
	BITMAPINFO *pBmpInfo = NULL;
	BYTE *pBits = NULL;

    DWORD dwFileStart = (DWORD)( file.GetPosition() );

	BITMAPFILEHEADER BmpFileHdr;
	int iBytes;
	iBytes = file.Read(&BmpFileHdr, sizeof(BmpFileHdr));
	if (iBytes != sizeof(BmpFileHdr)) return FALSE;
	if (BmpFileHdr.bfType != 0x4D42) return FALSE;

	BITMAPINFOHEADER BmpInfoHdr;
	iBytes = file.Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
	if (iBytes != sizeof(BmpInfoHdr)) return FALSE;
/*
	if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER)) {
		if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER)) return FALSE;
		bIsPM = TRUE;

		file.Seek(dwFileStart + sizeof(BITMAPFILEHEADER), CFile::begin);
		BITMAPCOREHEADER BmpCoreHdr;
		iBytes = file.Read(&BmpCoreHdr, sizeof(BmpCoreHdr)); 
		if (iBytes != sizeof(BmpCoreHdr)) return FALSE;

		BmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
		BmpInfoHdr.biWidth = (int) BmpCoreHdr.bcWidth;
		BmpInfoHdr.biHeight = (int) BmpCoreHdr.bcHeight;
		BmpInfoHdr.biPlanes = BmpCoreHdr.bcPlanes;
		BmpInfoHdr.biBitCount = BmpCoreHdr.bcBitCount;
		BmpInfoHdr.biCompression = BI_RGB;
		BmpInfoHdr.biSizeImage = 0;
		BmpInfoHdr.biXPelsPerMeter = 0;
		BmpInfoHdr.biYPelsPerMeter = 0;
		BmpInfoHdr.biClrUsed = 0;
		BmpInfoHdr.biClrImportant = 0;
	}
*/

	if(BmpInfoHdr.biBitCount!=8) return FALSE;
	if(BmpInfoHdr.biWidth%MAP_SIZE!=0) return FALSE;
	if(BmpInfoHdr.biHeight%MAP_SIZE!=0) return FALSE;

	pBits = (BYTE *) malloc(BmpFileHdr.bfSize - BmpFileHdr.bfOffBits);
	if (!pBits) return FALSE;
	file.Seek(dwFileStart + BmpFileHdr.bfOffBits, CFile::begin);
	iBytes = file.Read(pBits, BmpFileHdr.bfSize - BmpFileHdr.bfOffBits);
	if (iBytes != BmpFileHdr.bfSize - BmpFileHdr.bfOffBits) { free(pBits); return FALSE; }

	m_nLandWidth = BmpInfoHdr.biWidth/MAP_SIZE;
	m_nLandHeight = BmpInfoHdr.biHeight/MAP_SIZE;
	m_apLand = new CLandscape * [m_nLandWidth * m_nLandHeight];

	BYTE* pHeightTemp=new BYTE[(MAP_SIZE+1)*(MAP_SIZE+1)];

	for(int i = 0; i < m_nLandHeight; i++)
	{
		for(int j = 0; j < m_nLandWidth; j++) 
		{
			int nHeightOffset=(j*MAP_SIZE) + (i*m_nLandWidth*MAP_SIZE*MAP_SIZE);
			int nOffset = i * m_nLandWidth + j;
			m_apLand[ nOffset ] = new CLandscape;
			m_apLand[ nOffset ]->m_nWorldX=j*MAP_SIZE;
			m_apLand[ nOffset ]->m_nWorldY=i*MAP_SIZE;
			m_apLand[ nOffset ]->InitDeviceObjects( m_pd3dDevice, this );
			for(int k=0;k<MAP_SIZE+1;k++)
				for(int l=0;l<MAP_SIZE+1;l++) {
					if( (j==m_nLandWidth-1 && l==MAP_SIZE) || (i==m_nLandHeight-1 && k==MAP_SIZE) ) 
						pHeightTemp[l+k*(MAP_SIZE+1)]=0;
					else pHeightTemp[l+k*(MAP_SIZE+1)]=pBits[nHeightOffset+l+k*(MAP_SIZE*m_nLandWidth)];
				}
			m_apLand[ nOffset ]->ResetTerrain( 10 ,pHeightTemp);
			m_apLand[ nOffset ]->NewLandscape( INIT_TEX );
			m_apLand[ nOffset ]->RestoreDeviceObjects( m_pd3dDevice );
		}
	}

	delete pHeightTemp;
	free(pBits);
	return TRUE;

}
#endif

BOOL CWorld::LoadLandAttr()
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.csv", m_szFilePath, strLandName );
	
	CScript script( TRUE );
	if( script.Load( strLandTemp, FALSE ) == FALSE )
		return FALSE;
	/*
	typedef struct tagLANDATTR
	{
	DWORD dwColor;
	DWORD dwGenRegionAttr;
	DWORD dwPayRegionAttr;
	DWORD dwMusic;
	DWORD dwRepeat;
	DWORD dwCityName;
	DWORD dwLandName;
	DWORD dwPayID;
	CHAR  dwKey[ 64 ];
	} LANDATTR,* LPLANDATTR;	
	 */

	LANDATTR aLandAttr[ 256 ];
	ZeroMemory( aLandAttr, sizeof( aLandAttr ) );
	LPLANDATTR pLandAttr = NULL;
	int j, i = script.GetNumber( TRUE ); // id
	while ( script.tok != FINISHED )
	{
		pLandAttr = &aLandAttr[ i ];
		pLandAttr->dwColor = script.GetNumber();
		script.GetToken();

		CScript scriptTemp;
		scriptTemp.SetProg( script.token );
		j = scriptTemp.GetNumber();
		while ( scriptTemp.tok != FINISHED )
		{
			pLandAttr->dwGenRegionAttr |= j;
			j = scriptTemp.GetNumber();
		}
		script.GetToken( TRUE );
		scriptTemp.SetProg( script.token );
		j = scriptTemp.GetNumber();
		while ( scriptTemp.tok != FINISHED )
		{
			pLandAttr->dwPayRegionAttr |= j;
			j = scriptTemp.GetNumber();
		}
		pLandAttr->dwMusic = script.GetNumber();
		pLandAttr->dwRepeat = script.GetNumber();
		pLandAttr->dwCityName = script.GetNumber();
		pLandAttr->dwLandName = script.GetNumber();
		pLandAttr->dwPayID = script.GetNumber();
		script.GetToken();
		strcpy( pLandAttr->dwKey, script.token );
		
		i = script.GetNumber(); 
	}
	if( pLandAttr )
	{
		i++;
		m_apLandAttr = new LANDATTR[ i ];
		for( j = 0; j < i; j++ )
		{
			memcpy( &m_apLandAttr[ j ], &aLandAttr[ j ], sizeof( LANDATTR ) );
		}
	}
	return TRUE;	
}
BOOL CWorld::OpenWorld( OBJID idWorld, BOOL bDir )
{
	LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( idWorld );
	ASSERT( lpWorld->IsValid() );
	return( OpenWorld( lpWorld->m_szFileName, bDir ) );
}

BOOL CWorld::OpenWorld( LPCTSTR lpszFileName, BOOL bDir )
{
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	TCHAR szPathName[ _MAX_PATH ];

	//TCHAR szName[ 64];
//	TCHAR szPath[128];

	if( bDir == FALSE )
	{
		strcpy( szPathName, lpszFileName );
		/*
		// 드라이브 명을 포함한 전체 전체 path를 만든다.
		MakeFullPathName( lpszFileName, szPathName );
		// PathName를 Path와 FileName으로 나눈다.
		DivPathAndName( szPathName, szPath, szName );

		CString strFileName = szName;
		strFileName.MakeLower();
		if( strFileName.Find( ".wld" ) == -1 )
			strcat( szName, ".wld" );
			*/
	}
	else
	{
		_splitpath( lpszFileName, drive, dir, name, ext );
		strcpy( szPathName, lpszFileName );
		strcat( szPathName, "\\" );
		strcat( szPathName, name );
		strcat( szPathName, ".wld" );
		
		/*
		// 드라이브 명을 포함한 전체 전체 path를 만든다.
		MakeFullPathName( lpszFileName, szPathName );
		// PathName를 Path와 FileName으로 나눈다.
		DivPathAndName( szPathName, szPath, szName );
		strcat( szPath, szName );
		strcat( szPath, "\\" );
		strcat( szName, ".wld" );
		strcat( szPathName, "\\" );
		strcat( szPathName, szName );
		*/
	}

	CScript script;
	//script.Load( MakePath( DIR_WORLD, "WdMadrigal\\WdMadrigal.rgn" ) );
	if( script.Load( szPathName ) == FALSE )
		return FALSE;

	g_kDlgContinent.Reset( );

	do {
		script.GetToken();
		if( script.Token == _T( "size" ) )
		{
			m_nLandWidth = script.GetNumber();
			script.GetToken();
			m_nLandHeight = script.GetNumber();
		}
		else
		if( script.Token == _T( "ambient" ) )
		{
			m_dwAmbient = script.GetHex();
		}
		else
		if( script.Token == _T( "bgColor" ) )
		{
			//CWorld::m_dwBgColor = script.GetHex();
		}
#  ifdef __BEAST
		else
		if( script.Token == _T( "camera" ) )
		{
			CToolCamera* pCamera = (CToolCamera*)m_pCamera;
			pCamera->m_vPos.x = script.GetFloat();
			pCamera->m_vPos.y = script.GetFloat();
			pCamera->m_vPos.z = script.GetFloat();
			pCamera->m_fPitch = script.GetFloat();
			pCamera->m_fYaw = script.GetFloat();
		}
#  endif
		else
		if( script.Token == _T( "revival" ) )
		{
			m_dwIdWorldRevival = script.GetNumber();
			script.GetToken();
			strcpy( m_szKeyRevival, script.token );
		}
		else
		if( script.Token == _T( "indoor" ) )
		{
			m_bIsIndoor = script.GetNumber();
#if defined( __CLIENT ) || defined( __BEAST )
			m_bViewSkybox = m_bViewWeather = !m_bIsIndoor;
#endif //__CLIENT
//			script.GetToken();    << 이부분 왜 있는거지? 이것땜에 앰비언트값이 안읽혀서 지웠음 -xuzhu 2006.7.26-
		}
		else
		if( script.Token == _T( "fly" ) )
		{
			m_bFly = script.GetNumber();
		}
		else
		if( script.Token == _T( "bgm" ) )
		{
			m_dwIdMusic = script.GetNumber();
		}

#ifdef __N_TRIGGER_AREASET_050203
		else
		if( script.Token == _T( "pkmode" ) )
		{
			m_nPKMode = script.GetNumber();
		}
#endif // __N_TRIGGER_AREASET_050203
		else
		if( script.Token == _T( "diffuse" ) )
		{
			m_dwDiffuse = script.GetHex();
		}
		else
		if( script.Token == _T( "lightDir" ) )
		{
			m_v3LightDir.x = script.GetFloat();
			m_v3LightDir.y = script.GetFloat();
			m_v3LightDir.z = script.GetFloat();
		}
#if defined( __CLIENT ) || defined( __BEAST )
		else
		if( script.Token == _T( "fogSetting" ) )
		{
			m_fFogStartValue = script.GetFloat();
			m_fFogEndValue   = script.GetFloat();
			m_fFogDensity    = script.GetFloat();
			
		}
		
		//gmpbigsun
		else
		if( script.Token == _T( "MPU" ) )
		{
			g_MPU = script.GetNumber( );

			if( g_MPU < 1 || g_MPU > 4 )
			{
				AfxMessageBox( "MPU : out of range" );
				assert( 0 );
			}
		}
		else
		if( script.Token == _T( "sky" ) )
		{
			script.GetToken( );
			m_strSkyTex[0] = script.Token;
			script.GetToken( );
			m_strSkyTex[1] = script.Token;
			script.GetToken( );
			m_strSkyTex[2] = script.Token;
		}
		else
		if( script.Token == _T( "cloud" ) )
		{
			script.GetToken( );
			m_strCloudTex[0] = script.Token;
			script.GetToken( );
			m_strCloudTex[1] = script.Token;
			script.GetToken( );
			m_strCloudTex[2] = script.Token;
		}
		else
		if( script.Token == _T( "sun" ) )
		{
			script.GetToken( );
			m_strSunTex = script.Token;
		}
		else
		if( script.Token == _T( "moon" ) )
		{
			script.GetToken( );
			m_strMoonTex = script.Token;
		}
		else 
		if( script.Token == _T( "use24light" ) )
		{
			m_bIsUsing24Light = script.GetNumber( );
		}
		else
		if( script.Token == _T( "seacloud" ) )
		{
			script.GetToken( );
			m_strSeaCloud = script.Token;

			if( m_strSeaCloud != "default" )
				LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_WEATHER, m_strSeaCloud ), &m_pTextureSeaCloud );
		}


#endif //__CLIENT

	} while( script.tok != FINISHED );
	// 파일명 보관 

	g_pMainFrame->m_pageTerrainHeight.m_iMPU = MPU;
	g_pMainFrame->m_pageTerrainHeight.UpdateData( FALSE );

	_splitpath( szPathName, drive, dir, name, ext );
	strcpy( m_szFilePath, dir );
	strcpy( m_szFileName, name );
	strcat( m_szFileName, ext );

	//! Continent info
	g_kDlgContinent.Init( this, szPathName );

	//! world 24hour light
	if( m_bIsUsing24Light )
		g_kDlg24Light.Init( this, szPathName );

	m_apLand = new CLandscape * [ m_nLandWidth * m_nLandHeight ];
	memset( m_apLand, 0, sizeof( CLandscape*) * m_nLandWidth * m_nLandHeight );
#ifndef __ONLINE
	LoadObject();
#endif	// __ONLINE
	LoadRegion();

#ifdef __Y_PATROL
	LoadPatrol();
#endif //__Y_PATROL

#ifdef __VLANDATTR
	LoadLandAttr();
#endif

	return TRUE;
}

BOOL CWorld::LoadPatrol()
{
#ifdef __Y_PATROL
	return TRUE;	
#endif //__Y_PATROL
}


// 동적 오브젝트 스크립트 로드
// ReadObject는 사실 클라이언트에서는 필요없다. 그러나 싱글 테스트를 위해서 컴파일을 허용한다.
BOOL CWorld::LoadObject()
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.dyo", m_szFilePath, strLandName );
#ifdef __BEAST
	CFileIO file;
#else
	CResFile file;
#endif
	if( file.Open( strLandTemp,"rb" ) == FALSE )
		return FALSE;

	CObj* pObj;
	do
	{
	#ifdef __XNOMOB
		pObj = NULL;
	#else
		pObj = ::ReadObj( &file );
	#endif

		if( pObj )
		{
			D3DXVECTOR3 vPos = pObj->GetPos();
			vPos.x += ADDPOS_X;
			vPos.z += ADDPOS_Z;
			pObj->SetPos( vPos );
			
			// 비스트에서는 랜드를 모두 읽지 않았기 때문에 동적 오브젝트를 모든 랜드에 추가할 수 없다.
			// 따라서 m_aryObjBuffer에 임시로 보관해 둔다.
			m_aryObjBuffer.push_back( pObj );

		}	// if( pObj )
	} while( pObj );
	
#ifdef __XSHIPBUS
	if( GetID() == WI_WORLD_MADRIGAL )	// 마드리갈 읽을때
	{
		CObj *pObj	= CreateObj( D3DDEVICE, OT_SHIP, 4 );
		D3DXVECTOR3 vPos = D3DXVECTOR3( 6997.2f, 124.2f, 3243.7f );
		pObj->SetPos( vPos );
		pObj->SetAngle( 180.0f );
		AddObj( pObj, TRUE );
	}
#endif

	return TRUE;
}

// 저장 포맷을 실제 스트링으로 변환 
void RegionDesc_SaveFormatToString( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\\' && *(pwszSrc + 1 )== 'n')
		{
			*(pwszSrc + 0) = '\r';
			*(pwszSrc + 1) = '\n';
		}
		pwszSrc++;
	}
}
// 실제 스트링을 저장 포맷으로 변경 
void RegionDesc_StringToSaveFormat( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\r' && *(pwszSrc + 1 )== '\n')
		{
			*(pwszSrc + 0) = '\\';
			*(pwszSrc + 1) = 'n';
		}
		pwszSrc++;
	}	
}
// LoadRegion는 사실 클라이언트에서는 필요없다. 
// 그러나 싱글 테스트를 위해서 컴파일을 허용한다.
BOOL CWorld::ReadRegion( CScript& s )
{
	BOOL bNewFormat = FALSE;
	BOOL bNewFormat3 = FALSE;
	if( s.Token == _T( "region2" ) )
		bNewFormat = TRUE;
	if( s.Token == _T( "region3" ) )
	{
		bNewFormat3 = TRUE;
		bNewFormat = TRUE;
	}
	DWORD dwType = s.GetNumber();
	DWORD dwIndex = s.GetNumber();
	D3DXVECTOR3 vPos;
	vPos.x = s.GetFloat() + ADDPOS_X;
	vPos.y = s.GetFloat() + ADDPOS_Y;
	vPos.z = s.GetFloat() + ADDPOS_Z;

	REGIONELEM re,* pRe = &re;
	ZeroMemory( &re, sizeof( re ) );

#ifdef __N_TRIGGER_AREASET_050203
	pRe->m_uItemId		= 0xffffffff;
	pRe->m_uiItemCount	= 0xffffffff;
	pRe->m_uiMinLevel	= 0xffffffff;
	pRe->m_uiMaxLevel	= 0xffffffff;
	pRe->m_iQuest		= 0xffffffff;
	pRe->m_iQuestFlag	= 0xffffffff;
	pRe->m_iJob			= 0xffffffff;
	pRe->m_iGender		= 0xffffffff;
	pRe->m_bCheckParty	= FALSE;
	pRe->m_bCheckGuild	= FALSE;
	pRe->m_bChaoKey		= FALSE;
#endif // __N_TRIGGER_AREASET_050203
#ifdef __BEAST
	CRegion* pRegion = (CRegion*) NewObj( dwType, dwIndex );
	pRegion->InitProp();
	pRegion->SetPos( vPos );
	m_aryObjBuffer.push_back( pRegion );
	pRe = &pRegion->m_region;
#endif		
	pRe->m_dwAttribute	= s.GetNumber();
	pRe->m_dwIdMusic = s.GetNumber();
	pRe->m_bDirectMusic = (bool)(s.GetNumber() == 1);

	s.GetToken(); _tcscpy( pRe->m_szScript, s.token );
	s.GetToken(); _tcscpy( pRe->m_szSound, s.token );
	pRe->m_dwIdTeleWorld = s.GetNumber();

	//s.GetToken(); _tcscpy( pRe->m_szTeleport, s.token );

	pRe->m_vPos        = vPos;
	pRe->m_vTeleWorld.x = s.GetFloat() + ADDPOS_X;
	pRe->m_vTeleWorld.y = s.GetFloat() + ADDPOS_Y;
	pRe->m_vTeleWorld.z = s.GetFloat() + ADDPOS_Z;
	pRe->m_rect.left   = (LONG)s.GetNumber()+ ADDPOS_X;
	pRe->m_rect.top	   = (LONG)s.GetNumber()+ ADDPOS_Z;
	pRe->m_rect.right  = (LONG)s.GetNumber()+ ADDPOS_X;
	pRe->m_rect.bottom = (LONG)s.GetNumber()+ ADDPOS_Z;
	s.GetToken();
	strcpy( pRe->m_szKey, s.Token );
	pRe->m_bTargetKey  =  (bool)(s.GetNumber() == 1);
#ifdef __N_TRIGGER_AREASET_050203
	if( bNewFormat3 )
	{
		pRe->m_uItemId		= (u_long)s.GetNumber();
		pRe->m_uiItemCount	= (UINT)s.GetNumber();
		pRe->m_uiMinLevel	= (UINT)s.GetNumber();
		pRe->m_uiMaxLevel	= (UINT)s.GetNumber();
		pRe->m_iQuest		= s.GetNumber();
		pRe->m_iQuestFlag	= s.GetNumber();
		pRe->m_iJob			= s.GetNumber();
		pRe->m_iGender		= s.GetNumber();
		pRe->m_bCheckParty	= (BOOL)s.GetNumber();
		pRe->m_bCheckGuild	= (BOOL)s.GetNumber();
		pRe->m_bChaoKey		= (BOOL)s.GetNumber();
	}
#endif // __N_TRIGGER_AREASET_050203
#ifdef __DEV
	if( pRe->m_dwIdTeleWorld != WI_WORLD_NONE )
	{
		TRACE( "%d, %d , %d, %d-> <%d>%0.1f, %0.1f\n", pRe->m_rect.left, pRe->m_rect.top, pRe->m_rect.right, pRe->m_rect.bottom, pRe->m_dwIdTeleWorld, pRe->m_vTeleWorld.x, pRe->m_vTeleWorld.z );
	}
#endif	// __DEV

	if( bNewFormat == FALSE )
	{
		pRe->m_cDescSize = (char)( (LONG)s.GetNumber() );
		if( pRe->m_cDescSize )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
#ifdef __USE_STR_TBL0407
			s.GetToken();
			int nLength	= wcslen( s.m_wszToken );
			wchar_t wszDesc[ 512 ];
			wcscpy( wszDesc, s.m_wszToken );
			RegionDesc_SaveFormatToString( wszDesc );
			_wcstombsz( pRe->m_szDesc, wszDesc, ( nLength * 2 ) + 1 );
			lstrcpy( pRe->m_szDescKey, s.m_strKey );
			s.GetToken();	// }
#else	// __USE_STR_TBL0407
			CString string;
			int nLang;
			nLang = s.GetNumber();
			do
			{
				s.GetLastFull();
#	ifdef __BEAST
				wcscpy( pRe->m_apwszDesc[ nLang ], s.m_wszToken );
				RegionDesc_SaveFormatToString( pRe->m_apwszDesc[ nLang ] );
				if( nLang == prj.m_nLanguage )
				{		
					CHAR szDesc[ 1024 ];
					int nLength = wcslen( pRe->m_apwszDesc[ nLang ] ); 
					_wcstombsz( szDesc, pRe->m_apwszDesc[ nLang ], ( nLength * 2 ) + 1 );
					strcpy( pRe->m_szDesc, szDesc );
				}
#	else
				if( nLang == prj.m_nLanguage )
				{
					int nLength = wcslen( s.m_wszToken ); 
					wchar_t wszDesc[ 512 ];
					wcscpy( wszDesc, s.m_wszToken );
					RegionDesc_SaveFormatToString( wszDesc );
					_wcstombsz( pRe->m_szDesc, wszDesc, ( nLength * 2 ) + 1 );
				}
#	endif
				nLang = s.GetNumber();
			} while( *s.token != '}' );
#endif	// __USE_STR_TBL0407
		}
	}
	else
	{
		s.GetToken(); // get "title"
		BOOL bDesc = s.GetNumber();
		if( bDesc )
		{
			s.GetToken(); // {
#ifdef __USE_STR_TBL0407
			s.GetToken();
			int nLength		= wcslen( s.m_wszToken ); 
			wchar_t wszDesc[ 512 ];
			wcscpy( wszDesc, s.m_wszToken );
			RegionDesc_SaveFormatToString( wszDesc );
			_wcstombsz( pRe->m_szTitle, wszDesc, ( nLength * 2 ) + 1 );
			lstrcpy( pRe->m_szTitleKey, s.m_strKey );
			s.GetToken();	// }
#else	// __USE_STR_TBL0407
			CString string;
			int nLang;
			nLang = s.GetNumber();
			do
			{
				s.GetLastFull();
#	ifdef __BEAST
				wcscpy( pRe->m_apwszTitle[ nLang ], s.m_wszToken );
				RegionDesc_SaveFormatToString( pRe->m_apwszTitle[ nLang ] );
				if( nLang == prj.m_nLanguage )
				{		
					CHAR szDesc[ 1024 ];
					int nLength = wcslen( pRe->m_apwszTitle[ nLang ] ); 
					_wcstombsz( szDesc, pRe->m_apwszTitle[ nLang ], ( nLength * 2 ) + 1 );
					strcpy( pRe->m_szTitle, szDesc );
				}
#	else
				if( nLang == prj.m_nLanguage )
				{
					int nLength = wcslen( s.m_wszToken ); 
					wchar_t wszDesc[ 512 ];
					wcscpy( wszDesc, s.m_wszToken );
					RegionDesc_SaveFormatToString( wszDesc );
					_wcstombsz( pRe->m_szTitle, wszDesc, ( nLength * 2 ) + 1 );
				}
#	endif
				nLang = s.GetNumber();
			} while( *s.token != '}' );
#endif	// __USE_STR_TBL0407
		}
////////////////////////////////////////////////////////////////////////////////////////////////
		s.GetToken(); // get "desc"
		bDesc = s.GetNumber();
		if( bDesc )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
#ifdef __USE_STR_TBL0407
			s.GetToken();
			int nLength		= wcslen( s.m_wszToken ); 
			wchar_t wszDesc[ 512 ];
			wcscpy( wszDesc, s.m_wszToken );
			RegionDesc_SaveFormatToString( wszDesc );
			_wcstombsz( pRe->m_szDesc, wszDesc, ( nLength * 2 ) + 1 );
			lstrcpy( pRe->m_szDescKey, s.m_strKey );
			s.GetToken();	// }
#else	// __USE_STR_TBL0407
			CString string;
			int nLang;
			nLang = s.GetNumber();
			do
			{
				s.GetLastFull();
#	ifdef __BEAST
				wcscpy( pRe->m_apwszDesc[ nLang ], s.m_wszToken );
				RegionDesc_SaveFormatToString( pRe->m_apwszDesc[ nLang ] );
				if( nLang == prj.m_nLanguage )
				{		
					CHAR szDesc[ 1024 ];
					int nLength = wcslen( pRe->m_apwszDesc[ nLang ] ); 
					_wcstombsz( szDesc, pRe->m_apwszDesc[ nLang ], ( nLength * 2 ) + 1 );
					strcpy( pRe->m_szDesc, szDesc );
				}
#	else
				if( nLang == prj.m_nLanguage )
				{
					int nLength = wcslen( s.m_wszToken ); 
					wchar_t wszDesc[ 512 ];
					wcscpy( wszDesc, s.m_wszToken );
					RegionDesc_SaveFormatToString( wszDesc );
					_wcstombsz( pRe->m_szDesc, wszDesc, ( nLength * 2 ) + 1 );
				}
#	endif
				nLang = s.GetNumber();
			} while( *s.token != '}' );
#endif	// __USE_STR_TBL0407
		}
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__BEAST) //|| defined(__ONLINE)
	CPoint point( (int)( vPos.x ), (int)( vPos.z ) );
	point.x = point.x / MPU * MPU;
	point.y = point.y / MPU * MPU;
	pRe->m_rect -= point;
#else
	if( dwIndex == RI_BEGIN )
		m_aBeginPos.AddTail( pRe );	// memcpy
	else 
	if( dwIndex == RI_REVIVAL ) 
	{
	}
	else 
	if( dwIndex == RI_STRUCTURE ) 
		m_aStructure.AddTail( pRe );
	else
	{
//		TRACE("region:%d %d %d %d\n", pRe->m_rect.left, pRe->m_rect.top, 
//			                          pRe->m_rect.right, pRe->m_rect.bottom );

		m_aRegion.AddTail( pRe );	
	}
#endif
	return TRUE;
}
BOOL CWorld::ReadRespawn( CScript& s )
{
	int nVersion = 1;
	if( s.Token == _T( "respawn2" ) )
		nVersion = 2;
	if( s.Token == _T( "respawn3" ) )
		nVersion = 3;
#ifdef __YAIMONSTER_EX
	if( s.Token == _T( "respawn4" ) )
		nVersion = 4;	
#endif //__YAIMONSTER_EX

#ifdef __Y_PATROL
	if( s.Token == _T( "respawn5" ) )
		nVersion = 5;	
	
	if( s.Token == _T( "respawn6" ) )
		nVersion = 6;	
#ifdef __LEGEND
	if( s.Token == _T( "respawn7" ) )
		nVersion = 7;
#endif //__LEGEND
#endif //__Y_PATROL

	if( s.Token == _T( "respawn8" ) )
		nVersion = 8;

	//BOOL bRespawn2 = s.Token == _T( "respawn2" );
	CRespawnInfo ri,* pInfo = &ri;
	DWORD dwType;
	DWORD dwIndex;
	D3DXVECTOR3 vPos;
	dwType = s.GetNumber();
//				if( dwType == 4 )
//				{
//					int a;
//					a++;
//				}
	dwIndex = s.GetNumber();
	vPos.x = s.GetFloat() + ADDPOS_X;
	vPos.y = s.GetFloat() + ADDPOS_Y;
	vPos.z = s.GetFloat() + ADDPOS_Z;
#ifdef __BEAST
	CObj* pObj;
	pObj = NewObj( dwType, dwIndex );
	pObj->InitProp();
	pObj->SetPos( vPos );
	
	if( dwType == OT_MOVER )
	{
		pInfo = &((CMover*)pObj)->m_respawnInfo;
		strcpy( ((CMover*)pObj)->m_szName, pObj->GetProp()->szName );  
	}
	else
	if( dwType == OT_ITEM )
		pInfo = &((CItem*)pObj)->m_respawnInfo;
	else
	if( dwType == OT_CTRL )
		pInfo = &((CCommonCtrl*)pObj)->m_respawnInfo;

	m_aryObjBuffer.push_back( pObj );
#endif	// __BEAST	
	pInfo->m_dwIndex     = dwIndex;
	pInfo->m_dwType	= dwType;

	pInfo->m_cb          = (u_short)s.GetNumber();

	pInfo->m_uTime	= (short)s.GetNumber();	// 60/1

	pInfo->m_nActiveAttackNum = (long)s.GetNumber();

	pInfo->m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	pInfo->m_fY = vPos.y;
	if( dwType == OT_MOVER )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( dwIndex );
	}
	pInfo->m_rect.left   = (LONG)s.GetNumber()+ ADDPOS_X;;
	pInfo->m_rect.top    = (LONG)s.GetNumber()+ ADDPOS_Z;;
	pInfo->m_rect.right  = (LONG)s.GetNumber()+ ADDPOS_X;;
	pInfo->m_rect.bottom = (LONG)s.GetNumber()+ ADDPOS_Z;;
	if( nVersion >= 2 )
	{
		pInfo->m_nDayMin = s.GetNumber();
		pInfo->m_nDayMax = s.GetNumber();
		pInfo->m_nHourMin = s.GetNumber();
		pInfo->m_nHourMax = s.GetNumber();
		pInfo->m_nItemMin = s.GetNumber();
		pInfo->m_nItemMax = s.GetNumber();
	}

#ifdef __YAIMONSTER_EX
	if( nVersion >= 4 )
	{		
		pInfo->m_dwAiState = s.GetNumber();
		pInfo->m_fAngle    = s.GetFloat();
	}

#ifdef __Y_PATROL
	if( nVersion >= 5 )
	{		
		pInfo->m_dwPatrolIndex = s.GetNumber();
	}

	if( nVersion >= 6 )
	{		
		pInfo->m_bPatrolCycle  = s.GetNumber();
	}
	
#endif //__Y_PATROL

	if( nVersion >= 8 )
		pInfo->m_dwAI = s.GetNumber();
	
	
#endif //__YAIMONSTER_EX
	
	if( nVersion >= 3 )
	{	
		int nContrl = s.GetNumber();
		if( nContrl )
		{
			pInfo->m_CtrlElem.m_dwSet = s.GetNumber(); 
			pInfo->m_CtrlElem.m_dwSetItem = s.GetNumber();  
#ifdef __S_DONGEON_0504
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = s.GetNumber();  
			}
			else
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = 1;
			}
#endif // __S_DONGEON_0504
			pInfo->m_CtrlElem.m_dwSetLevel = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetQuestNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetFlagNum = s.GetNumber();  
#ifdef __S_DONGEON_0504
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetQuestNum1 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetFlagNum1 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetQuestNum2 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetFlagNum2 = s.GetNumber();  
			}
#endif // __S_DONGEON_0504
			pInfo->m_CtrlElem.m_dwSetGender = s.GetNumber();

#ifdef __LEGEND
			int maxjob = 0;
			if(nVersion <= 6)
				maxjob = 16;
			else
				maxjob = 32;

			for( int i = 0; i < maxjob; i++ )
				pInfo->m_CtrlElem.m_bSetJob[ i ] = s.GetNumber();
#else //__LEGEND
			for( int i = 0; i < MAX_JOB; i++ )
				pInfo->m_CtrlElem.m_bSetJob[ i ] = s.GetNumber();
#endif //__LEGEND

			pInfo->m_CtrlElem.m_dwSetEndu = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMinItemNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMaxiItemNum = s.GetNumber();  
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemKind[ i ] = s.GetNumber();  
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemPer[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResKind[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResNum[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonActAttack[ i ] = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapOperType = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapRandomPer = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapDelay = s.GetNumber();  
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapKind[ i ] = s.GetNumber();  
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapLevel[ i ] = s.GetNumber(); 
			
#ifdef __S_DONGEON_0504
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwTeleWorldId = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleX = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleY = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleZ = s.GetNumber();  
			}
#endif // __S_DONGEON_0504
		}
	}	
//				pInfo->m_vPos	   = vPos;

#ifdef __BEAST
	CPoint point( (int)( vPos.x ), (int)( vPos.z ) );
	point.x = point.x / MPU * MPU;
	point.y = point.y / MPU * MPU;
	pInfo->m_rect -= point;
#endif	// __BEAST

#ifdef __IVERSION_NOFLY
	if( prj.GetMoverProp( dwIndex )->dwFlying )
		return;
#endif	// __IVERSION_NOFLY

#if !defined(__BEAST) && !defined(__ONLINE)
	m_respawner.Add( ri );
#endif	// __BEAST	// __ONLINE

	return TRUE;
}
BOOL CWorld::LoadRegion()
{
#if defined( __WORLDSERVER ) ||  defined( __BEAST ) || defined(__CLIENT)
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );

	CScript s;
	if( s.Load( strLandTemp, FALSE ) )
	{
#ifdef __USE_STR_TBL0407
		m_strStrFile	= strLandTemp.Left( strLandTemp.GetLength() - 3 ) + "txt.txt";
		LoadString();
#endif	// __USE_STR_TBL0407

		D3DXVECTOR3 vPos;
		//DWORD dwTemp = s.GetNumber();
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "region" ) || s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
			{
				ReadRegion( s );
			}
			else
			if( s.Token == _T( "respawn" ) || s.Token == _T( "respawn2" ) || s.Token == _T( "respawn3" ) || 
				s.Token == _T( "respawn4" ) || s.Token == _T( "respawn5" ) || s.Token == _T( "respawn6" ) || 
				s.Token == _T( "respawn7" ) || s.Token == _T( "respawn8" ) )
			{
				ReadRespawn( s );
			}
			//dwTemp = s.GetNumber();
			s.GetToken();
		}
		return TRUE;
	}
#endif
	return FALSE;
}

#ifdef __BEAST
BOOL CWorld::NewWorld( int nWidth, int nHeight, int nTexture, FLOAT fHeight )
{
	m_nLandWidth = nWidth;
	m_nLandHeight = nHeight;
	m_apLand = new CLandscape * [m_nLandWidth * m_nLandHeight];

	for(int i = 0; i < m_nLandHeight; i++)
	{
		for(int j = 0; j < m_nLandWidth; j++) 
		{
			int nOffset = i * m_nLandWidth + j;
			m_apLand[ nOffset ] = new CLandscape;
#ifndef __NewWater
			m_apLand[nOffset]->usingNewWater = true;
#endif
			m_apLand[ nOffset ]->m_nWorldX=j*MAP_SIZE;
			m_apLand[ nOffset ]->m_nWorldY=i*MAP_SIZE;
			m_apLand[ nOffset ]->InitDeviceObjects( m_pd3dDevice, this );
			m_apLand[ nOffset ]->ResetTerrain( (DWORD)( fHeight ) );
			m_apLand[ nOffset ]->NewLandscape( nTexture );
			m_apLand[ nOffset ]->RestoreDeviceObjects( m_pd3dDevice );
			//m_apLand[ nOffset ]->CalculateBound();
		}
	}
	return TRUE;
}
BOOL CWorld::NewWorld( LPCTSTR lpszFileName,int nTexture, FLOAT fMultipleHeight )
{
	/*
	m_nLandWidth = nWidth;
	m_nLandHeight = nHeight;
	m_apLand = new CLandscape * [m_nLandWidth * m_nLandHeight];

	for(int i = 0; i < m_nLandHeight; i++)
	{
		for(int j = 0; j < m_nLandWidth; j++) 
		{
			int nOffset = i * m_nLandWidth + j;
			m_apLand[ nOffset ] = new CLandscape;
			m_apLand[ nOffset ]->m_nWorldX=j*MAP_SIZE;
			m_apLand[ nOffset ]->m_nWorldY=i*MAP_SIZE;
			m_apLand[ nOffset ]->InitDeviceObjects( m_pd3dDevice, this );
			m_apLand[ nOffset ]->ResetTerrain( fHeight );
			m_apLand[ nOffset ]->NewLandscape( nTexture );
			m_apLand[ nOffset ]->RestoreDeviceObjects( m_pd3dDevice );
			//m_apLand[ nOffset ]->CalculateBound();
		}
	}
*/

	CFile file;
	if (! file.Open( lpszFileName, CFile::modeRead | CFile::shareDenyWrite)) return FALSE;

    BOOL bIsPM = FALSE;
	BITMAPINFO *pBmpInfo = NULL;
	BYTE *pBits = NULL;

    DWORD dwFileStart = (DWORD)( file.GetPosition() );

	BITMAPFILEHEADER BmpFileHdr;
	int iBytes;
	iBytes = file.Read(&BmpFileHdr, sizeof(BmpFileHdr));
	if (iBytes != sizeof(BmpFileHdr)) return FALSE;
	if (BmpFileHdr.bfType != 0x4D42) return FALSE;

	BITMAPINFOHEADER BmpInfoHdr;
	iBytes = file.Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
	if (iBytes != sizeof(BmpInfoHdr)) return FALSE;
/*
	if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER)) {
		if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER)) return FALSE;
		bIsPM = TRUE;

		file.Seek(dwFileStart + sizeof(BITMAPFILEHEADER), CFile::begin);
		BITMAPCOREHEADER BmpCoreHdr;
		iBytes = file.Read(&BmpCoreHdr, sizeof(BmpCoreHdr)); 
		if (iBytes != sizeof(BmpCoreHdr)) return FALSE;

		BmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
		BmpInfoHdr.biWidth = (int) BmpCoreHdr.bcWidth;
		BmpInfoHdr.biHeight = (int) BmpCoreHdr.bcHeight;
		BmpInfoHdr.biPlanes = BmpCoreHdr.bcPlanes;
		BmpInfoHdr.biBitCount = BmpCoreHdr.bcBitCount;
		BmpInfoHdr.biCompression = BI_RGB;
		BmpInfoHdr.biSizeImage = 0;
		BmpInfoHdr.biXPelsPerMeter = 0;
		BmpInfoHdr.biYPelsPerMeter = 0;
		BmpInfoHdr.biClrUsed = 0;
		BmpInfoHdr.biClrImportant = 0;
	}
*/

	if(BmpInfoHdr.biBitCount!=8) return FALSE;
	if(BmpInfoHdr.biWidth%MAP_SIZE!=0) return FALSE;
	if(BmpInfoHdr.biHeight%MAP_SIZE!=0) return FALSE;

	pBits = (BYTE *) malloc(BmpFileHdr.bfSize - BmpFileHdr.bfOffBits);
	if (!pBits) return FALSE;
	file.Seek(dwFileStart + BmpFileHdr.bfOffBits, CFile::begin);
	iBytes = file.Read(pBits, BmpFileHdr.bfSize - BmpFileHdr.bfOffBits);
	if (iBytes != BmpFileHdr.bfSize - BmpFileHdr.bfOffBits) { free(pBits); return FALSE; }

	m_nLandWidth = BmpInfoHdr.biWidth/MAP_SIZE;
	m_nLandHeight = BmpInfoHdr.biHeight/MAP_SIZE;
	m_apLand = new CLandscape * [m_nLandWidth * m_nLandHeight];

	BYTE* pHeightTemp=new BYTE[(MAP_SIZE+1)*(MAP_SIZE+1)];

	for(int i = 0; i < m_nLandHeight; i++)
	{
		for(int j = 0; j < m_nLandWidth; j++) 
		{
			int nHeightOffset=(j*MAP_SIZE) + (i*m_nLandWidth*MAP_SIZE*MAP_SIZE);
			int nOffset = i * m_nLandWidth + j;
			m_apLand[ nOffset ] = new CLandscape;
			m_apLand[ nOffset ]->m_nWorldX=j*MAP_SIZE;
			m_apLand[ nOffset ]->m_nWorldY=i*MAP_SIZE;
#ifndef __NewWater
			m_apLand[nOffset]->usingNewWater = true;
#endif
			m_apLand[ nOffset ]->InitDeviceObjects( m_pd3dDevice, this );
			for(int k=0;k<MAP_SIZE+1;k++)
				for(int l=0;l<MAP_SIZE+1;l++) {
					if( (j==m_nLandWidth-1 && l==MAP_SIZE) || (i==m_nLandHeight-1 && k==MAP_SIZE) ) 
						pHeightTemp[l+k*(MAP_SIZE+1)]=0;
					else pHeightTemp[l+k*(MAP_SIZE+1)]=pBits[nHeightOffset+l+k*(MAP_SIZE*m_nLandWidth)];
				}
			m_apLand[ nOffset ]->ResetTerrain( 0 ,pHeightTemp);
			m_apLand[ nOffset ]->NewLandscape( nTexture );
			m_apLand[ nOffset ]->RestoreDeviceObjects( m_pd3dDevice );
		}
	}
	delete pHeightTemp;
	free(pBits);
	return TRUE;
}
BOOL CWorld::WriteRegionSub( CFileIO* pFile, CObj* pObj )
{
#ifdef __VMULTILANG
	CString str;
	wchar_t wszBuff[ 1024 ];
	
	D3DXVECTOR3 vPos = pObj->GetPos();
	if( pObj->IsType( OT_REGION ) )
	{
		CRegion* pRegion = (CRegion*) pObj;
		LPREGIONELEM lpRegionElem;
		lpRegionElem = &pRegion->m_region;
		CPoint point( (int)( vPos.x ), (int)( vPos.z ) );
		CRect rect = lpRegionElem->m_rect;
		point.x = point.x / MPU * MPU;
		point.y = point.y / MPU * MPU;
		rect += point;
#ifdef __N_TRIGGER_AREASET_050203
		str.Format( "region3 %d %d %f %f %f 0x%x %d %d \"%s\" \"%s\" %d %f %f %f %d %d %d %d \"%s\" %d %d %d %d %d %d %d %d %d %d %d %d \r\n", 
			pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, 
			lpRegionElem->m_dwAttribute,
			lpRegionElem->m_dwIdMusic,
			lpRegionElem->m_bDirectMusic,
			lpRegionElem->m_szScript,
			lpRegionElem->m_szSound,
			lpRegionElem->m_dwIdTeleWorld, lpRegionElem->m_vTeleWorld.x,lpRegionElem->m_vTeleWorld.y,lpRegionElem->m_vTeleWorld.z,
			rect.left, rect.top, rect.right, rect.bottom, lpRegionElem->m_szKey, lpRegionElem->m_bTargetKey 
			, lpRegionElem->m_uItemId, lpRegionElem->m_uiItemCount, lpRegionElem->m_uiMinLevel, lpRegionElem->m_uiMaxLevel
			, lpRegionElem->m_iQuest, lpRegionElem->m_iQuestFlag, lpRegionElem->m_iJob, lpRegionElem->m_iGender
			, lpRegionElem->m_bCheckParty, lpRegionElem->m_bCheckGuild, lpRegionElem->m_bChaoKey
			);
#else // __N_TRIGGER_AREASET_050203
		str.Format( "region2 %d %d %f %f %f 0x%x %d %d \"%s\" \"%s\" %d %f %f %f %d %d %d %d \"%s\" %d  \r\n", 
			pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, 
			lpRegionElem->m_dwAttribute,
			lpRegionElem->m_dwIdMusic,
			lpRegionElem->m_bDirectMusic,
			lpRegionElem->m_szScript,
			lpRegionElem->m_szSound,
			lpRegionElem->m_dwIdTeleWorld, lpRegionElem->m_vTeleWorld.x,lpRegionElem->m_vTeleWorld.y,lpRegionElem->m_vTeleWorld.z,
			rect.left, rect.top, rect.right, rect.bottom, lpRegionElem->m_szKey, lpRegionElem->m_bTargetKey 
			);
#endif // __N_TRIGGER_AREASET_050203
		_mbstowcsz( wszBuff, str, str.GetLength() + 1 );
		pFile->Write( wszBuff, wcslen( wszBuff) * 2 );

#ifdef __USE_STR_TBL0407
		pFile->PutWideString( "title " );
		if( *lpRegionElem->m_szTitle )
		{
			str = "1 \r\n{\r\n";
			pFile->PutWideString( str );
			if( *lpRegionElem->m_szTitleKey == '\0' )	// new
			{
				CString strKey, strtemp;
				if( m_nString > 0 )
				{
					map<string, string>::iterator i	= m_mapString.begin();
					strtemp		= i->first.data();
					strKey.Format( _T( "%s%06d" ), strtemp.Left( strtemp.GetLength() - 6 ), m_nString++ );
				}
				else
				{
					strKey.Format( _T( "IDS_%s_%06d" ), m_szFileName, m_nString++ );
					strKey.MakeUpper();
					strKey.Replace( _T( "." ), _T( "_" ) );
					strKey.Replace( _T( "-" ), _T( "_" ) );
				}
				lstrcpy( lpRegionElem->m_szTitleKey, (LPCTSTR)strKey );
				BOOL bResult	= m_mapString.insert( map<string,string>::value_type( (LPCTSTR)strKey, lpRegionElem->m_szTitle ) ).second;
			}
			else
			{
				CString strKey	= lpRegionElem->m_szTitleKey;
				map<string, string>::iterator i	= m_mapString.find( (LPCTSTR)strKey );
				if( i != m_mapString.end() )
				{
					CString strTitleOld		= i->second.data();
					CString strTitle	= lpRegionElem->m_szTitle;
					strTitleOld.Replace( "\\n", "\r\n" );
					if( strTitle != strTitleOld )
					{
						m_mapString.erase( i );
						BOOL bResult	= m_mapString.insert( map<string, string>::value_type( (LPCTSTR)strKey, (LPCTSTR)strTitle ) ).second;
					}
				}
			}

			wchar_t wszTitle[ 1024 ];
			int nResult		= MultiByteToWideChar( CP_ACP, 0, lpRegionElem->m_szTitleKey, -1, wszTitle, 1024 );
//			RegionDesc_StringToSaveFormat( wszTitle );
			pFile->Write( wszTitle, wcslen( wszTitle ) * 2 );

			str = "\r\n}\r\n";
			pFile->PutWideString( str );
		}
		else
			pFile->PutWideString( "0\r\n" );

		pFile->PutWideString( "desc " );
		if( *lpRegionElem->m_szDesc )
		{
			str = "1 \r\n{\r\n";
			pFile->PutWideString( str );
			if( *lpRegionElem->m_szDescKey == '\0' )
			{
				CString strKey, strtemp;
				if( m_nString > 0 )
				{
					map<string, string>::iterator i	= m_mapString.begin();
					strtemp		= i->first.data();
					strKey.Format( _T( "%s%06d" ), strtemp.Left( strtemp.GetLength() - 6 ), m_nString++ );
				}
				else
				{
					strKey.Format( _T( "IDS_%s_%06d" ), m_szFileName, m_nString++ );
					strKey.MakeUpper();
					strKey.Replace( _T( "." ), _T( "_" ) );
					strKey.Replace( _T( "-" ), _T( "_" ) );
				}
				lstrcpy( lpRegionElem->m_szDescKey, (LPCTSTR)strKey );
				BOOL bResult	= m_mapString.insert( map<string,string>::value_type( (LPCTSTR)strKey, lpRegionElem->m_szDesc ) ).second;
			}
			else
			{
				CString strKey	= lpRegionElem->m_szDescKey;
				map<string, string>::iterator i	= m_mapString.find( (LPCTSTR)strKey );
				if( i != m_mapString.end() )
				{
					CString strDescOld		= i->second.data();
					CString strDesc	= lpRegionElem->m_szDesc;
					strDescOld.Replace( "\\n", "\r\n" );
					if( strDesc != strDescOld )
					{
						m_mapString.erase( i );
						BOOL bResult	= m_mapString.insert( map<string, string>::value_type( (LPCTSTR)strKey, (LPCTSTR)strDesc ) ).second;
					}
				}
			}
			wchar_t wszDesc[1024];
			int nResult = MultiByteToWideChar( CP_ACP, 0, lpRegionElem->m_szDescKey, -1, wszDesc, 1024 );
//			wcscpy(lpRegionElem->m_apwszDesc[prj.m_nLanguage], wszBuff );
//			wcscpy( wszDesc, lpRegionElem->m_apwszDesc[prj.m_nLanguage] );
//			RegionDesc_StringToSaveFormat( wszDesc );
			pFile->Write( wszDesc, wcslen( wszDesc ) * 2 );

			str = "\r\n}\r\n";
			pFile->PutWideString( str );
		}
		else
			pFile->PutWideString( "0\r\n" );
#else	// __USE_STR_TBL0407
		pFile->PutWideString( "title " );
		if( lpRegionElem->m_szTitle[ 0 ] )
		{
			str = "1 \r\n{\r\n";
			pFile->PutWideString( str );

			int nResult = MultiByteToWideChar( CP_ACP, 0, lpRegionElem->m_szTitle, -1, wszBuff, 1024 );
			wcscpy(lpRegionElem->m_apwszTitle[ prj.m_nLanguage ], wszBuff );
				
			for( int i = 0; i < LANG_MAX; i++ )
			{
				str.Format( "%s ", g_szLanguage[ i ] );
				pFile->PutWideString( str );

				wchar_t wszTitle[ 512 ];
				wcscpy( wszTitle, lpRegionElem->m_apwszTitle[ i ] );
				RegionDesc_StringToSaveFormat( wszTitle );
				pFile->Write( wszTitle, wcslen( wszTitle ) * 2 );

				str = "\r\n";
				pFile->PutWideString( str );
			}
			str = "}\r\n";
			pFile->PutWideString( str );

		}
		else
			pFile->PutWideString( "0\r\n" );


		pFile->PutWideString( "desc " );
		if( lpRegionElem->m_szDesc[ 0 ] )
		{
			str = "1 \r\n{\r\n";
			pFile->PutWideString( str );

			int nResult = MultiByteToWideChar( CP_ACP, 0, lpRegionElem->m_szDesc, -1, wszBuff, 1024 );
			wcscpy(lpRegionElem->m_apwszDesc[ prj.m_nLanguage ], wszBuff );
				
			for( int i = 0; i < LANG_MAX; i++ )
			{
				str.Format( "%s ", g_szLanguage[ i ] );
				pFile->PutWideString( str );

				wchar_t wszDesc[ 512 ];
				wcscpy( wszDesc, lpRegionElem->m_apwszDesc[ i ] );
				RegionDesc_StringToSaveFormat( wszDesc );
				pFile->Write( wszDesc, wcslen( wszDesc ) * 2 );

				str = "\r\n";
				pFile->PutWideString( str );
			}
			str = "}\r\n";
			pFile->PutWideString( str );

		}
		else
			pFile->PutWideString( "0\r\n" );
#endif	// __USE_STR_TBL0407
	}
	else
	{
		CRespawnInfo* pRespawnInfo = NULL;
		if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_respawnInfo.m_dwIndex ) 
		{
			CMover* pMover = (CMover*) pObj; pRespawnInfo = &pMover->m_respawnInfo;
		}
		if( pObj->GetType() == OT_ITEM && ((CItem*)pObj)->m_respawnInfo.m_dwIndex ) 
		{
			CItem* pItem = (CItem*) pObj; pRespawnInfo = &pItem->m_respawnInfo;
		}
		if( pObj->GetType() == OT_CTRL && ((CCommonCtrl*)pObj)->m_respawnInfo.m_dwIndex ) 
		{
			CCommonCtrl* pCtrl = (CCommonCtrl*) pObj; pRespawnInfo = &pCtrl->m_respawnInfo;
		}

		if( pRespawnInfo )
		{
			CPoint point( (int)( vPos.x ), (int)( vPos.z ) );
			CRect rect = pRespawnInfo->m_rect;
			point.x = point.x / MPU * MPU;
			point.y = point.y / MPU * MPU;
			rect += point;

			/*
			DWORD		m_dwSet;				    //-열기조건 
			DWORD 		m_dwSetItem;				//-박스 오픈 
			DWORD		m_dwSetLevel;				//-박스 오픈 
			DWORD		m_dwSetQuestNum;			//-박스 오픈 
			DWORD		m_dwSetFlagNum;
			DWORD		m_dwSetGender;				//-박스 오픈 
			BOOL		m_bSetJob[MAX_JOB];			//-박스 오픈 
			
			DWORD		m_dwSetEndu;				//-박스 내구
			 
			DWORD		m_dwMinItemNum;				//-최소 발생 
			DWORD		m_dwMaxiItemNum;			//-최대 발생 
			DWORD		m_dwInsideItemKind[MAX_CTRLDROPITEM];	
			DWORD		m_dwInsideItemPer [MAX_CTRLDROPITEM];	
			
			DWORD       m_dwMonResKind  [MAX_CTRLDROPMOB];       
			DWORD		m_dwMonResNum   [MAX_CTRLDROPMOB];		
			DWORD		m_dwMonActAttack[MAX_CTRLDROPMOB];		

			// 트랩관련
			DWORD		m_dwTrapOperType;              // 트랩 발

			DWORD		m_dwTrapKind	[MAX_TRAP];    // 트랩 종
 			DWORD		m_dwTrapParam1	[MAX_TRAP];
			DWORD		m_dwTrapParam2	[MAX_TRAP];
			*/
#ifdef __BS_AI_RESPAWN_ATTR
				str.Format( "respawn8 %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %d %d %d ", 
				pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, pRespawnInfo->m_cb, pRespawnInfo->m_uTime, pRespawnInfo->m_nActiveAttackNum,
				rect.left, rect.top, rect.right, rect.bottom, pRespawnInfo->m_nDayMin, pRespawnInfo->m_nDayMax, pRespawnInfo->m_nHourMin, pRespawnInfo->m_nHourMax, 
				pRespawnInfo->m_nItemMin, pRespawnInfo->m_nItemMax, pRespawnInfo->m_dwAiState, pObj->GetAngle(), pRespawnInfo->m_dwPatrolIndex, 
				pRespawnInfo->m_bPatrolCycle, pRespawnInfo->m_dwAI );
#else


#ifdef __YAIMONSTER_EX			
	#ifdef __Y_PATROL
#ifdef __LEGEND

			str.Format( "respawn7 %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %d %d ", 
				pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, pRespawnInfo->m_cb, pRespawnInfo->m_uTime, pRespawnInfo->m_nActiveAttackNum,
				rect.left, rect.top, rect.right, rect.bottom, pRespawnInfo->m_nDayMin, pRespawnInfo->m_nDayMax, pRespawnInfo->m_nHourMin, pRespawnInfo->m_nHourMax, 
				pRespawnInfo->m_nItemMin, pRespawnInfo->m_nItemMax, pRespawnInfo->m_dwAiState, pObj->GetAngle(), pRespawnInfo->m_dwPatrolIndex, 
				pRespawnInfo->m_bPatrolCycle );
#else //__LEGEND
			str.Format( "respawn6 %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %d %d ", 
				pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, pRespawnInfo->m_cb, pRespawnInfo->m_uTime, pRespawnInfo->m_nActiveAttackNum,
				rect.left, rect.top, rect.right, rect.bottom, pRespawnInfo->m_nDayMin, pRespawnInfo->m_nDayMax, pRespawnInfo->m_nHourMin, pRespawnInfo->m_nHourMax, pRespawnInfo->m_nItemMin, pRespawnInfo->m_nItemMax, pRespawnInfo->m_dwAiState , pObj->GetAngle(), pRespawnInfo->m_dwPatrolIndex, pRespawnInfo->m_bPatrolCycle );
#endif //__LEGEND
	#else //__Y_PATROL			
			str.Format( "respawn4 %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f ", 
				pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, pRespawnInfo->m_cb, pRespawnInfo->m_uTime, pRespawnInfo->m_nActiveAttackNum,
				rect.left, rect.top, rect.right, rect.bottom, pRespawnInfo->m_nDayMin, pRespawnInfo->m_nDayMax, pRespawnInfo->m_nHourMin, pRespawnInfo->m_nHourMax, pRespawnInfo->m_nItemMin, pRespawnInfo->m_nItemMax, pRespawnInfo->m_dwAiState , pObj->GetAngle() );
	#endif //__Y_PATROL
#else //__YAIMONSTER_EX
			str.Format( "respawn3 %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d ", 
				pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, pRespawnInfo->m_cb, pRespawnInfo->m_uTime, pRespawnInfo->m_nActiveAttackNum,
				rect.left, rect.top, rect.right, rect.bottom, pRespawnInfo->m_nDayMin, pRespawnInfo->m_nDayMax, pRespawnInfo->m_nHourMin, pRespawnInfo->m_nHourMax, pRespawnInfo->m_nItemMin, pRespawnInfo->m_nItemMax );
#endif //__YAIMONSTER_EX
#endif //__BS_AI_RESPAWN_ATTR

			if( pObj->GetType() == OT_CTRL )
			{
				CCtrlElem* pCtrl = &((CCommonCtrl*)pObj)->m_respawnInfo.m_CtrlElem;
				CString string2;
#ifdef __S_DONGEON_0504
#ifdef __LEGEND

				string2.Format( "2  %d %d %d %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d  %d %d   %d %d %d %d  %d %d %d %d  %d %d %d  %d %d %d  %d %d %d   %d %d %d   %d %d %d   %d %d %d   %d %d %d %d", 
					pCtrl->m_dwSet, pCtrl->m_dwSetItem, pCtrl->m_dwSetItemCount, pCtrl->m_dwSetLevel, pCtrl->m_dwSetQuestNum, pCtrl->m_dwSetFlagNum, 
					pCtrl->m_dwSetQuestNum1, pCtrl->m_dwSetFlagNum1, pCtrl->m_dwSetQuestNum2, pCtrl->m_dwSetFlagNum2, pCtrl->m_dwSetGender,
#else //__LEGEND
				string2.Format( "2  %d %d %d %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d  %d %d   %d %d %d %d  %d %d %d %d  %d %d %d  %d %d %d  %d %d %d   %d %d %d   %d %d %d   %d %d %d   %d %d %d %d", 
					pCtrl->m_dwSet, pCtrl->m_dwSetItem, pCtrl->m_dwSetItemCount, pCtrl->m_dwSetLevel, pCtrl->m_dwSetQuestNum, pCtrl->m_dwSetFlagNum, pCtrl->m_dwSetQuestNum1, pCtrl->m_dwSetFlagNum1, pCtrl->m_dwSetQuestNum2, pCtrl->m_dwSetFlagNum2, pCtrl->m_dwSetGender, 
#endif //__LEGEND
#else // __S_DONGEON_0504
				string2.Format( "1  %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d  %d %d   %d %d %d %d  %d %d %d %d  %d %d %d  %d %d %d  %d %d %d   %d %d %d   %d %d %d   %d %d %d ", 
					pCtrl->m_dwSet, pCtrl->m_dwSetItem, pCtrl->m_dwSetLevel, pCtrl->m_dwSetQuestNum, pCtrl->m_dwSetFlagNum, pCtrl->m_dwSetGender, 
#endif // __S_DONGEON_0504
#ifdef __LEGEND
					pCtrl->m_bSetJob[ 0 ], pCtrl->m_bSetJob[ 1 ], pCtrl->m_bSetJob[  2 ], pCtrl->m_bSetJob[  3 ], pCtrl->m_bSetJob[  4 ], pCtrl->m_bSetJob[  5 ], pCtrl->m_bSetJob[  6 ], pCtrl->m_bSetJob[  7 ],
					pCtrl->m_bSetJob[ 8 ], pCtrl->m_bSetJob[ 9 ], pCtrl->m_bSetJob[ 10 ], pCtrl->m_bSetJob[ 11 ], pCtrl->m_bSetJob[ 12 ], pCtrl->m_bSetJob[ 13 ], pCtrl->m_bSetJob[ 14 ], pCtrl->m_bSetJob[ 15 ],
					pCtrl->m_bSetJob[ 16 ], pCtrl->m_bSetJob[ 17 ], pCtrl->m_bSetJob[ 18 ], pCtrl->m_bSetJob[ 19 ], pCtrl->m_bSetJob[ 20 ], pCtrl->m_bSetJob[ 21 ], pCtrl->m_bSetJob[ 22 ], pCtrl->m_bSetJob[ 23 ],
					pCtrl->m_bSetJob[ 24 ], pCtrl->m_bSetJob[ 25 ], pCtrl->m_bSetJob[ 26 ], pCtrl->m_bSetJob[ 27 ], pCtrl->m_bSetJob[ 28 ], pCtrl->m_bSetJob[ 29 ], pCtrl->m_bSetJob[ 30 ], pCtrl->m_bSetJob[ 31 ],
#else //__LEGEND
					pCtrl->m_bSetJob[ 0 ], pCtrl->m_bSetJob[ 1 ], pCtrl->m_bSetJob[  2 ], pCtrl->m_bSetJob[  3 ], pCtrl->m_bSetJob[  4 ], pCtrl->m_bSetJob[  5 ], pCtrl->m_bSetJob[  6 ], pCtrl->m_bSetJob[  7 ],
					pCtrl->m_bSetJob[ 8 ], pCtrl->m_bSetJob[ 9 ], pCtrl->m_bSetJob[ 10 ], pCtrl->m_bSetJob[ 11 ], pCtrl->m_bSetJob[ 12 ], pCtrl->m_bSetJob[ 13 ], pCtrl->m_bSetJob[ 14 ], pCtrl->m_bSetJob[ 15 ],
#endif //__LEGEND
					pCtrl->m_dwSetEndu, 
					pCtrl->m_dwMinItemNum, pCtrl->m_dwMaxiItemNum, 
					pCtrl->m_dwInsideItemKind[ 0 ], pCtrl->m_dwInsideItemKind[ 1 ], pCtrl->m_dwInsideItemKind[ 2 ], pCtrl->m_dwInsideItemKind[ 3 ],
					pCtrl->m_dwInsideItemPer [ 0 ], pCtrl->m_dwInsideItemPer [ 1 ], pCtrl->m_dwInsideItemPer [ 2 ], pCtrl->m_dwInsideItemPer [ 3 ], 
					pCtrl->m_dwMonResKind    [ 0 ], pCtrl->m_dwMonResKind    [ 1 ], pCtrl->m_dwMonResKind    [ 2 ], 
					pCtrl->m_dwMonResNum     [ 0 ], pCtrl->m_dwMonResNum     [ 1 ], pCtrl->m_dwMonResNum     [ 2 ], 
					pCtrl->m_dwMonActAttack  [ 0 ], pCtrl->m_dwMonActAttack  [ 1 ], pCtrl->m_dwMonActAttack  [ 2 ],
					pCtrl->m_dwTrapOperType,  pCtrl->m_dwTrapRandomPer, pCtrl->m_dwTrapDelay,
					pCtrl->m_dwTrapKind      [ 0 ], pCtrl->m_dwTrapKind      [ 1 ], pCtrl->m_dwTrapKind      [ 2 ], 
					pCtrl->m_dwTrapLevel     [ 0 ], pCtrl->m_dwTrapLevel     [ 1 ], pCtrl->m_dwTrapLevel     [ 2 ] 
#ifdef __S_DONGEON_0504
					, pCtrl->m_dwTeleWorldId, pCtrl->m_dwTeleX, pCtrl->m_dwTeleY, pCtrl->m_dwTeleZ
#endif // __S_DONGEON_0504
				);
				str += string2;
			}
			else
			{
				str += "0 ";
			}
			str += "\r\n";

			_mbstowcsz( wszBuff, str, str.GetLength() + 1 );
			pFile->Write( wszBuff, wcslen( wszBuff) * 2 );
		}
	}
#endif //
	return TRUE;
}
BOOL CWorld::SaveRegion( LPCTSTR lpszFileName )
{
#ifdef __VMULTILANG
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wb" ) ) == FALSE )
		return FALSE;
	CString string;
	wchar_t wszBuff[ 1024 ];

	file.PutC( (char)0xff );
	file.PutC( (char)0xfe );
	
	string = "// Region Script File\r\n\r\n";
	_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
	file.Write( wszBuff, wcslen( wszBuff ) * 2 );
	
	int i, j, k, l;
	CLandscape* pLand;
	for( i = 0; i < m_nLandHeight; i++)
	{
		for( j = 0; j < m_nLandWidth; j++)
		{
			if( m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
				for( k = OT_OBJ + 1; k < MAX_OBJARRAY; k++ )
				{
					CObj** apObject = pLand->m_apObject[ k ];
					for( l = 0; l < (int)( pLand->m_adwObjNum[ k ] ); l++ )
					{
						CObj* pObj = apObject[ l ];
						if( pObj )
						{
							WriteRegionSub( &file, pObj );
						}
					}
				}
			}
		}
	}
	// 추가로 아직 랜드에 할당되지 않은 오브젝트를 저장한다.
	for( ObjItor m = m_aryObjBuffer.begin(); m != m_aryObjBuffer.end(); ++m )
	{
		CObj* pObj = *m;
		if( pObj )
		{
			WriteRegionSub( &file, pObj );
		}
	}
#else
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wt" ) ) == FALSE )
		return FALSE;
	file.PutString( _T( "// Region Script File\n\n" ) );
	CString string;

	int i, j, k, l, x, y;
	CLandscape* pLand;
	for( i = 0; i < m_nLandHeight; i++)
	{
		for( j = 0; j < m_nLandWidth; j++)
		{
			if( m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
				for( k = OT_OBJ + 1; k < MAX_OBJARRAY; k++ )
				{
					CObj** apObject = pLand->m_apObject[ k ];
					for( l = 0; l < pLand->m_adwObjNum[ k ]; l++ )
					{
						CObj* pObj = apObject[ l ];
						if( pObj )
						{
							WriteRegionSub( &file, pObj );
						}
					}
				}
			}
		}
	}
	// 추가로 아직 랜드에 할당되지 않은 오브젝트를 저장한다.
	for( ObjItor m = m_aryObjBuffer.begin(); m != m_aryObjBuffer.end(); ++m )
	{
		CObj* pObj = *m;
		if( pObj )
		{
			WriteRegionSub( &file, pObj );
		}
	}
#endif

	if(m_strStrFile.GetLength() == 0)
	{
		CString strTemp;
		strTemp = lpszFileName;
		m_strStrFile = strTemp.Left( strTemp.GetLength() - 3 ) + "txt.txt";
	}

	WriteString();

	return TRUE;
}
BOOL CWorld::SaveObject( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName,"wb" ) == FALSE )
		return FALSE;
	int i, j, k, l;
	CLandscape* pLand;
	for( i = 0; i < m_nLandHeight; i++)
	{
		for( j = 0; j < m_nLandWidth; j++)
		{
			if( m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
				for( k = OT_OBJ + 1; k < MAX_OBJARRAY; k++ )
				{
					if( k != OT_SFX )
					{
						CObj** apObject = pLand->m_apObject[ k ];
						for( l = 0; l < (int)( pLand->m_adwObjNum[ k ] ); l++ )
						{
							CObj* pObj = apObject[ l ];
							if( pObj )
							{
								if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_respawnInfo.m_dwIndex )
									continue;
								if( pObj->GetType() == OT_ITEM && ((CItem*)pObj)->m_respawnInfo.m_dwIndex )
									continue;
								if( pObj->GetType() == OT_CTRL && ((CCommonCtrl*)pObj)->m_respawnInfo.m_dwIndex )
									continue;
								if( pObj->GetType() == OT_REGION )
									continue;
						#ifdef __Y_PATROL
								if( pObj->GetType() == OT_PATH )
									continue;
						#endif //__Y_PATROL								
								::WriteObj( &file, pObj );
							}
						}
					}
				}
			}
		}
	}
	// 추가로 아직 랜드에 할당되지 않은 오브젝트를 저장한다.
	for( ObjItor m = m_aryObjBuffer.begin(); m != m_aryObjBuffer.end(); ++m )
	{
		CObj* pObj = *m;
		if( pObj )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_respawnInfo.m_dwIndex )
				continue;
			if( pObj->GetType() == OT_ITEM && ((CItem*)pObj)->m_respawnInfo.m_dwIndex )
				continue;
			if( pObj->GetType() == OT_CTRL && ((CCommonCtrl*)pObj)->m_respawnInfo.m_dwIndex )
				continue;
			if( pObj->GetType() == OT_REGION )
				continue;
	#ifdef __Y_PATROL
			if( pObj->GetType() == OT_PATH )
				continue;
	#endif //__Y_PATROL								
			
		}
		::WriteObj( &file, pObj );
	}
	DWORD dwEndObject = 0xffffffff;
	file.Write( &dwEndObject, sizeof( DWORD ) );
	return TRUE;
}
#include "..\beast\worldDoc.h"
#include "..\beast\worldview.h"
/*
 biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
*/
// dwType = 0, general
// dwType = 1, autosave
// dwType = 2, backup
#define __SAVEMAP
// bFullSave가 TRUE일 경우에는 Modified 상관없이 모두 저장한다. 
// 그리고 Modified를 FALSE로 바꿔 놓지도 않는다.
// 맵 수정중 play를 할 때 사용.(play때는 맵을 모두 저장해야하며, Modified를 FALSE로 해버리면 실제 맵에 저장할 수 없게 됨)
BOOL CWorld::SaveWorld( LPCTSTR lpszFileName ,BOOL bSaveLND ,BOOL bSaveDYO ,BOOL bSaveRGN , BOOL bSaveDDS, BOOL bFullSave )
{
	// gmpbigsun : 원래 버젼으로 다시 복구 함 ( Saveas 는 메모리 부족으로 클라가 죽게된다.. 고치자 -_ㅜ )
	BOOL bBackup = TRUE;

	TCHAR szPathName[ 128 ];
	TCHAR szPath[ 128 ];
	TCHAR szName[ 32 ];

	// Full Path 얻기

	MakeFullPathName( lpszFileName, szPathName );

	// 확장자가 붙어 있는지 확인하고 붙어있지 않으면 붙인다.
	CString strTempName = szPathName;
	strTempName.MakeLower(); 
	if( strTempName.Find( ".wld" ) == -1 )
		strcat( szPathName, ".wld" );

	// PathName를 Path와 Name으로 나누기 
	DivPathAndName( szPathName, szPath, szName );

	CString strFileName = szName;

	// 폴더를 만든다. 만약 존재한다면 이 함수는 무시된다.
	CreateDirectory( szPath, NULL );

	FILE* fp;
	// 월드 스크립트 저장 
	if( !( fp = fopen( szPathName, "wt" ) ) )
		return FALSE;

	fprintf( fp, _T( "// World script\n\n" ) );
	fprintf( fp, _T( "size %d, %d\n" ), m_nLandWidth, m_nLandHeight );
	fprintf( fp, _T( "indoor %d\n" ), m_bIsIndoor );

	fprintf( fp, _T( "ambient 0x%x\n" ), m_dwAmbient );
	fprintf( fp, _T( "bgColor 0x%x\n" ), m_dwBgColor );
	fprintf( fp, _T( "fly %d\n" ), m_bFly );
	
	CToolCamera* pCamera = (CToolCamera*)m_pCamera;
	fprintf( fp, _T( "camera %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), 
		pCamera->m_vPos.x,
		pCamera->m_vPos.y,
		pCamera->m_vPos.z,
		pCamera->m_fPitch,
		pCamera->m_fYaw
		//m_pCamera->m_vLookAt.x,
		//m_pCamera->m_vLookAt.y,
		//m_pCamera->m_vLookAt.z 
		);
	fprintf( fp, _T( "revival %d \"%s\" \n" ), m_dwIdWorldRevival, m_szKeyRevival );
		
	fprintf( fp, _T( "diffuse 0x%x\n" ), m_dwDiffuse );
	fprintf( fp, _T( "lightDir %6.3f %6.3f %6.3f \n" ), m_v3LightDir.x, m_v3LightDir.y, m_v3LightDir.z );
	fprintf( fp, _T( "fogSetting %6.3f %6.3f %6.3f \n" ), m_fFogStartValue, m_fFogEndValue, m_fFogDensity );
	fprintf( fp, _T( "bgm %d \n" ), m_dwIdMusic );
	
#ifdef __N_TRIGGER_AREASET_050203
	fprintf( fp, _T( "pkmode %d \n" ), m_nPKMode );
#endif // __N_TRIGGER_AREASET_050203

	//added by gmpbigsun
	fprintf( fp, __T( "MPU %d \n" ), MPU );
	fprintf( fp, __T( "sky %s %s %s\n" ), m_strSkyTex[0].GetString(), m_strSkyTex[ 1 ].GetString(), m_strSkyTex[ 2 ].GetString() );
	fprintf( fp, __T( "cloud %s %s %s\n" ), m_strCloudTex[ 0 ].GetString(), m_strCloudTex[ 1 ].GetString(), m_strCloudTex[ 2 ].GetString() );
	fprintf( fp, __T( "sun %s\n" ), m_strSunTex.GetString() );
	fprintf( fp, __T( "moon %s\n" ), m_strMoonTex.GetString() );
	if( m_bIsUsing24Light )
		fprintf( fp, __T( "use24light 1" ) );		//ah .. -_-

	fprintf( fp, __T( "seacloud %s\n" ), m_strSeaCloud.GetString() );		//바다구름

	fclose( fp );
		
	if( m_bIsUsing24Light )
		g_kDlg24Light.SaveFile( szPathName );


	CString strLandTemp;
	CString strLandName = szName;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );

	for( int i = 0; i < m_nLandHeight; i++)
	{
		for( int j = 0; j < m_nLandWidth; j++)
		{
			if( m_apLand[ i * m_nLandWidth + j] )
			{
				BOOL bModified = bFullSave ? TRUE : m_apLand[ i * m_nLandWidth + j]->IsModified();
				if( bModified )
				{
					strLandTemp.Format( "%s%s%02d-%02d.lnd", szPath, strLandName, j , i );
					if( bSaveLND )
					{
						if( m_apLand[ i * m_nLandWidth + j]->SaveLandscape( strLandTemp, j, i ) == FALSE )
						{
							CString string;
							string.Format( "%s를 저장할 수 없습니다. Read only로 되어 있는지 확인하십시오." );
							AfxMessageBox( string );
							return FALSE;
						}
						else
						if( bFullSave == FALSE )
							m_apLand[ i * m_nLandWidth + j]->SetModified( FALSE );
					}
					if( bSaveDDS )
					{
						strLandTemp.Format( "%s%s%02d-%02d.dds", szPath, strLandName, j , i );
						SaveLandBitmap( strLandTemp, j, i );
					}
				}
			}
			else
			{
				// 이곳으로 올 경우는 원본을 오픈 후 다른 폴더에 저장할 때 로딩하지 않은 랜드가 있다면

				// 바이너리로 복사한다. 바이너리로 원본을 오픈하지 못하면 파일이 임의로 삭제되거나 문제가 생긴 것이다.

				// gmpbigsun: 09_23 현재 Mardrigal( 30 * 30 ) SaveAS 저장시 메모리 부족 오류가 발생한다.
				// 로드되지 않은 모든 landscape를 로드하기는 역시 역부족, 일단 마드리갈의 경우 오직 save기능만을 이용하고
				// 추후 SaveAs는 일정한 양의 랜드스케이프를 저장하고 메모리 해제하는 식으로 변경해야 한다.

			}
		}
	}

	fclose( fp );

	strcpy( m_szFilePath, szPath );
	strcpy( m_szFileName, szName );

	//Save continent info
	g_kDlgContinent.WriteFile( szPath, szName );

	// 동적 오브젝트 저장 
	strLandName = m_szFileName;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	if( bSaveDYO )
	{
		strLandTemp.Format( "%s%s.dyo", m_szFilePath, strLandName );
		if( SaveObject( strLandTemp ) == FALSE )
		{
			CString string;
			string.Format( "%s를 저장할 수 없습니다. Read only가 아닌지 확인 바랍니다.", strLandTemp );
			AfxMessageBox( string );
		}
	}
	// Respawn 스크립트 저장 
	if( bSaveRGN )
	{
		strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );
		SaveRegion( strLandTemp );
	}
	// Region 스크립트 저장 
//	strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );
//	SaveRegion( strLandTemp );

	AfxMessageBox( "Ok... Saved the world!!" );

	return TRUE;

	//return _SaveTheWorld( lpszFileName, bSaveLND, bSaveDYO, bSaveRGN, bSaveDDS, bFullSave );
}

BOOL CWorld::SaveasWorld( LPCTSTR lpszFileName ,BOOL bSaveLND ,BOOL bSaveDYO ,BOOL bSaveRGN , BOOL bSaveDDS, BOOL bFullSave )
{
	return _SaveTheWorld( lpszFileName, bSaveLND, bSaveDYO, bSaveRGN, bSaveDDS, bFullSave );
}

BOOL CWorld::_SaveTheWorld( LPCTSTR lpszFileName, BOOL bSaveLND, BOOL bSaveDYO, BOOL bSaveRGN, BOOL bSaveDDS, BOOL bFullSave )
{
	BOOL bBackup = TRUE;

	TCHAR szPathName[ 128 ];
	TCHAR szPath[ 128 ];
	TCHAR szName[ 32 ];

	// Full Path 얻기
	MakeFullPathName( lpszFileName, szPathName );

	// 확장자가 붙어 있는지 확인하고 붙어있지 않으면 붙인다.
	CString strTempName = szPathName;
	strTempName.MakeLower(); 
	if( strTempName.Find( ".wld" ) == -1 )
		strcat( szPathName, ".wld" );

	// PathName를 Path와 Name으로 나누기 
	DivPathAndName( szPathName, szPath, szName );

	CString strFileName = szName;

	// 폴더를 만든다. 만약 존재한다면 이 함수는 무시된다.
	CreateDirectory( szPath, NULL );

	FILE* fp;
	// 월드 스크립트 저장 
	if( !( fp = fopen( szPathName, "wt" ) ) )
		return FALSE;

	fprintf( fp, _T( "// World script\n\n" ) );
	fprintf( fp, _T( "size %d, %d\n" ), m_nLandWidth, m_nLandHeight );
	fprintf( fp, _T( "indoor %d\n" ), m_bIsIndoor );

	fprintf( fp, _T( "ambient 0x%x\n" ), m_dwAmbient );
	fprintf( fp, _T( "diffuse 0x%x\n" ), m_dwDiffuse );
	fprintf( fp, _T( "bgColor 0x%x\n" ), m_dwBgColor );
	fprintf( fp, _T( "fly %d\n" ), m_bFly );
	
	CToolCamera* pCamera = (CToolCamera*)m_pCamera;
	fprintf( fp, _T( "camera %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), 
		pCamera->m_vPos.x,
		pCamera->m_vPos.y,
		pCamera->m_vPos.z,
		pCamera->m_fPitch,
		pCamera->m_fYaw
		//m_pCamera->m_vLookAt.x,
		//m_pCamera->m_vLookAt.y,
		//m_pCamera->m_vLookAt.z 
		);
	fprintf( fp, _T( "revival %d \"%s\" \n" ), m_dwIdWorldRevival, m_szKeyRevival );
	fprintf( fp, _T( "lightDir %6.3f %6.3f %6.3f \n" ), m_v3LightDir.x, m_v3LightDir.y, m_v3LightDir.z );
	fprintf( fp, _T( "fogSetting %6.3f %6.3f %6.3f \n" ), m_fFogStartValue, m_fFogEndValue, m_fFogDensity );
	fprintf( fp, _T( "bgm %d \n" ), m_dwIdMusic );
	
#ifdef __N_TRIGGER_AREASET_050203
	fprintf( fp, _T( "pkmode %d \n" ), m_nPKMode );
#endif // __N_TRIGGER_AREASET_050203

	//added by gmpbigsun
	fprintf( fp, __T( "MPU %d \n" ), MPU );
	fprintf( fp, __T( "sky %s %s %s\n" ), m_strSkyTex[0].GetString(), m_strSkyTex[ 1 ].GetString(), m_strSkyTex[ 2 ].GetString() );
	fprintf( fp, __T( "cloud %s %s %s\n" ), m_strCloudTex[ 0 ].GetString(), m_strCloudTex[ 1 ].GetString(), m_strCloudTex[ 2 ].GetString() );
	fprintf( fp, __T( "sun %s\n" ), m_strSunTex.GetString() );
	fprintf( fp, __T( "moon %s\n" ), m_strMoonTex.GetString() );
	if( m_bIsUsing24Light )
		fprintf( fp, __T( "use24light 1" ) );		//ah .. -_-

	fprintf( fp, __T( "seacloud %s\n" ), m_strSeaCloud.GetString() );		//바다구름
			
	fclose( fp );

	if( m_bIsUsing24Light )
		g_kDlg24Light.SaveFile( szPathName );

	CString strLandTemp;
	CString strLandName = szName;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );


	//	BEGINTEST 전체???????
	for( int i = 0; i < m_nLandHeight; i++)
	{
		for( int j = 0; j < m_nLandWidth; j++)
		{
			if( m_apLand[ i * m_nLandWidth + j] )
			{
//				BOOL bModified = bFullSave ? TRUE : m_apLand[ i * m_nLandWidth + j]->IsModified();
//				if( bModified )
				{
					strLandTemp.Format( "%s%s%02d-%02d.lnd", szPath, strLandName, j , i );
					if( bSaveLND )
					{
						if( m_apLand[ i * m_nLandWidth + j]->SaveLandscape( strLandTemp, j, i ) == FALSE )
						{
							CString string;
							string.Format( "%s를 저장할 수 없습니다. Read only로 되어 있는지 확인하십시오." );
							AfxMessageBox( string );
							return FALSE;
						}
						else
						if( bFullSave == FALSE )
							m_apLand[ i * m_nLandWidth + j]->SetModified( FALSE );
					}
					if( bSaveDDS )
					{
						strLandTemp.Format( "%s%s%02d-%02d.dds", szPath, strLandName, j , i );
						SaveLandBitmap( strLandTemp, j, i );
					}
				}
			}
			else
			{
				//gmpbigsun: 전체저장의 경우 컬링 오브젝트 m_aobjCull에 쓰레기값이 들어가있어 죽는다.. 나중에 해결하고 일단 이렇게 
#ifdef __BS_SAVE_2DMAP
				for( int icull = 0; icull < m_nObjCullSize; icull++ )
					m_aobjCull[ icull ] = NULL;
				for( int icull = 0; m_nSfxCullSize < m_nSfxCullSize; icull++ )
					m_asfxCull[ icull ] = NULL;

				m_nObjCullSize = 0;
				m_nSfxCullSize = 0;
#endif

				// 이곳으로 올 경우는 원본을 오픈 후 다른 폴더에 저장할 때 로딩하지 않은 랜드가 있다면

				// 바이너리로 복사한다. 바이너리로 원본을 오픈하지 못하면 파일이 임의로 삭제되거나 문제가 생긴 것이다.
				if( LandInWorld( j, i ) )
				{
					CString strLand = m_szFileName;
					strLand.Delete( strLand.GetLength() - 4, 4 );

					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLand, j , i );

					CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
					if( pLand == NULL )
					{
						pLand = new CLandscape;
						pLand->m_nWorldX = j * MAP_SIZE;
						pLand->m_nWorldY = i * MAP_SIZE;
						pLand->InitDeviceObjects( m_pd3dDevice, this );
						if( pLand->LoadLandscape( strLandTemp, j, i ) == FALSE )
						{
							delete pLand;
							continue;
						}
						pLand->RestoreDeviceObjects(m_pd3dDevice);
						m_apLand[ i * m_nLandWidth + j] = pLand;

// 						for( ObjItor k = m_aryObjBuffer.begin(); k != m_aryObjBuffer.end(); ++k )
// 						{
// 							CObj* pObj = *k;
// 							D3DXVECTOR3 vPos = pObj->GetPos();
// 							if( vPos.x > pLand->m_nWorldX * MPU && vPos.z > pLand->m_nWorldY * MPU &&
// 								vPos.x < ( pLand->m_nWorldX + MAP_SIZE ) * MPU && vPos.z < ( pLand->m_nWorldY + MAP_SIZE ) * MPU )
// 							{
// 								pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex );//, TRUE );
// #ifdef __YAIMONSTER_EX
// 								if( pObj->GetType() == OT_MOVER )
// 								{
// 									CRespawnInfo *pInfo = NULL;								
// 									pInfo = &((CMover*)pObj)->m_respawnInfo;
// 
// 									if( pInfo->m_cb == 1 )
// 										((CMover*)pObj)->AddAngle(pInfo->m_fAngle);
// 								}
// #endif //__YAIMONSTER_EX
// 								pObj->SetWorld( this );
// 								InsertObjLink( pObj );
// 								AddObjArray( pObj );
// 								pObj->UpdateLocalMatrix();
// 								m_aryObjBuffer.erase( k-- );
// 							}
// 						}

						pLand->SetModified( FALSE );

						strLandTemp.Format( "%s%s%02d-%02d.lnd", szPath, strLandName, j , i );
						if( bSaveLND )
						{
							if( m_apLand[ i * m_nLandWidth + j]->SaveLandscape( strLandTemp, j, i ) == FALSE )
							{
								CString string;
								string.Format( "%s를 저장할 수 없습니다. Read only로 되어 있는지 확인하십시오." );
								AfxMessageBox( string );
								return FALSE;
							}
							else
								if( bFullSave == FALSE )
									m_apLand[ i * m_nLandWidth + j]->SetModified( FALSE );
						}
						if( bSaveDDS )
						{
							strLandTemp.Format( "%s%s%02d-%02d.dds", szPath, strLandName, j , i );
							SaveLandBitmap( strLandTemp, j, i );
						}

#ifdef __BS_SAVE_2DMAP
					if( pLand )						//gmpbigsun: 메모리 부족으로 죽는데 쓰고 지워버리자 
						SAFE_DELETE( pLand );

					m_apLand[ i * m_nLandWidth + j] = NULL;
#endif
					}

				}
			}
		}
	}

	//fclose( fp );

	//Save continent info
	g_kDlgContinent.WriteFile( szPath, szName );

	strcpy( m_szFilePath, szPath );
	strcpy( m_szFileName, szName );

	// 동적 오브젝트 저장 
	strLandName = m_szFileName;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	if( bSaveDYO )
	{
		strLandTemp.Format( "%s%s.dyo", m_szFilePath, strLandName );
		if( SaveObject( strLandTemp ) == FALSE )
		{
			CString string;
			string.Format( "%s를 저장할 수 없습니다. Read only가 아닌지 확인 바랍니다.", strLandTemp );
			AfxMessageBox( string );
		}
	}
	// Respawn 스크립트 저장 
	if( bSaveRGN )
	{
		strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );
		SaveRegion( strLandTemp );
	}
	// Region 스크립트 저장 
//	strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );
//	SaveRegion( strLandTemp );

	return TRUE;
}

BOOL CWorld::SaveLandBitmap( LPCTSTR lpszFileName, int xx, int yy )
{
	if( m_apLand[ yy * m_nLandWidth + xx ] == NULL ) 
		return FALSE;

	int nImageBlock = 256;//128 * 2;
	//int nImageSize  = nImageBlock * m_nLandWidth; //1280 * 2;//( g_pWorldView->m_d3dsdBackBuffer.Height / 8 ); 

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = nImageBlock;//nImageSize;
	viewport.Height = nImageBlock;//nImageSize;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);

	CTexture texture;
	//BOOL CreateTexture( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight, 
	//					   UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool = D3DPOOL_DEFAULT );
	texture.CreateTexture( m_pd3dDevice, nImageBlock, nImageBlock, 0, 0, D3DFMT_A8R8G8B8,	D3DPOOL_MANAGED );


	//LPBYTE  lpData = new BYTE[ nImageBlock * nImageBlock * 4 ];
	//LPDWORD lpdwData = (LPDWORD) lpData;
	//memset( lpData, 255, nImageBlock * nImageBlock * 4 );
   
	BOOL _bViewLODTerrain = m_bViewLODTerrain;
	BOOL _bViewFog = m_bViewFog;
	BOOL _bViewRegion = m_bViewRegion;
	BOOL _bViewSpawn = m_bViewSpawn;
	BOOL _bViewName = m_bViewName;
	BOOL _bViewLODObj = m_bViewLODObj;
	BOOL _bViewHgtAttr = m_bViewHeightAttribute;

	m_bViewFog = FALSE;
	m_bViewRegion = FALSE;//bViewRegion;
	m_bViewSpawn = FALSE;//bViewSpawn;
	m_bViewLODTerrain = FALSE;
	m_bViewName = FALSE;//bViewName;
	m_bViewLODObj = FALSE;
	m_bViewHeightAttribute = FALSE;
	
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

//	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 0,0,0,0) );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

///////////////////////////////////////

	// 변수 세이브 
	D3DXVECTOR3 vOldPos = m_pCamera->m_vPos;
	D3DXVECTOR3 vOldLookAt = m_pCamera->m_vLookAt;
				
	FLOAT fAspect = (FLOAT) 128 / (FLOAT) 128;
	//D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane, CWorld::m_fFarPlane );
	//m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	// 퍼스팩티브 없이 프로젝션 
	//FLOAT fov = D3DX_PI/796.0f;
	FLOAT fov = D3DX_PI/( 199.0f * MPU );		//gmpbigsun : MPU에 따라 변동
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &m_matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 );
	
	D3DLOCKED_RECT rectLock;
	texture.m_pTexture->LockRect( 0, &rectLock, 0, 0 );

	//memset( rectLock.pBits, 0xffffffff, nImageBlock * nImageBlock * 4 );

	if( m_pd3dDevice->BeginScene() == S_OK )
	{
		((CToolCamera*)m_pCamera)->m_fYaw = 0;
		((CToolCamera*)m_pCamera)->m_fPitch = D3DX_PI/2;
			//= D3DXVECTOR3( (FLOAT) ( xx * MAP_SIZE * MPU ) + ( MAP_SIZE * MPU / 2),   0 , (FLOAT) ( yy * MAP_SIZE * MPU )  + ( MAP_SIZE * MPU / 2) + 0.1f );
		m_pCamera->m_vPos// = m_pCamera->m_vPos2 
			= D3DXVECTOR3( (FLOAT) ( xx * MAP_SIZE * MPU ) + ( MAP_SIZE * MPU / 2), 500 , (FLOAT) ( yy * MAP_SIZE * MPU )  + ( MAP_SIZE * MPU / 2));
		//ReadWorld( m_pCamera->m_vPos );
		Render( m_pd3dDevice, NULL );

		IDirect3DSurface9* pDestSurface;
		m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

		//hr = m_ptexCur->GetSurfaceLevel(0, &psurfDest);

		D3DLOCKED_RECT lockedRect;
		CRect rectIn ( 0, 0, g_pWorldView->m_d3dsdBackBuffer.Width, g_pWorldView->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
		HRESULT hr = pDestSurface->LockRect( &lockedRect, NULL, 0 ) ;
		if( hr == D3D_OK )// Lock
		{
			LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
			LPWORD pWord = (LPWORD)lockedRect.pBits; // 16bit
			LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

			for(int y = 0; y < nImageBlock; y++ )
			{
				for(int x = 0; x < nImageBlock; x++)
				{
					/*
					// 16 bit color code
					WORD wData = pWord
						[ 
						//( ( nImageSize / 2 - ( nImageBlock / 2 ) + y ) * ( lockedRect.Pitch / 2) ) + ( nImageSize / 2 - ( nImageBlock / 2 ) + x ) 
							y * ( lockedRect.Pitch / 2) + x
						];
					BYTE byData1 = ( wData & 0xf800 ) >> 8;
					BYTE byData2 = ( wData & 0x07e0 ) >> 3;
					BYTE byData3 = ( wData & 0x001f ) << 3 ;
					DWORD dwOffset = 
						//( ( ( 7 - yy ) * nImageBlock + y ) * nImageSize + ( xx * nImageBlock + x ) ) * 3;
						//( y * nImageSize + x) * 3;
						( ( ( m_nLandHeight - yy - 1 ) * nImageBlock + y ) * nImageSize + ( xx * nImageBlock + x ) ) * 3;
					lpData[ dwOffset + 0 ] = byData3;
					lpData[ dwOffset + 1 ] = byData2;
					lpData[ dwOffset + 2 ] = byData1;
					*/
					// 32 bit color code

					
					BYTE byData3 = pSrcByte[ y * lockedRect.Pitch + ( x * 4 ) + 0 ];
					BYTE byData2 = pSrcByte[ y * lockedRect.Pitch + ( x * 4 ) + 1 ];
					BYTE byData1 = pSrcByte[ y * lockedRect.Pitch + ( x * 4 ) + 2 ];

					DWORD dwData = *pSrcDWord;
						//pDWord[ y * lockedRect.Pitch / 4 + x	];
					DWORD dwOffset = ( nImageBlock * y + x );
					//lpData[ dwOffset + 0 ] = byData3;
					//lpData[ dwOffset + 1 ] = byData2;
					//lpData[ dwOffset + 2 ] = byData1;

					((DWORD *)rectLock.pBits)[ dwOffset ] = D3DCOLOR_ARGB( 255, byData1, byData2, byData3 );

				}
			}
			pDestSurface->UnlockRect();
		}
		pDestSurface->Release();
	}
	m_pd3dDevice->EndScene();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	texture.m_pTexture->UnlockRect( 0 );
	TRACE( "SaveBMP Render x = %d, y = %d\n", xx, yy );

	D3DXSaveTextureToFile( lpszFileName, D3DXIFF_DDS, texture.m_pTexture, NULL );
	/*
    LPCTSTR pDestFile,
    D3DXIMAGE_FILEFORMAT DestFormat,
    LPDIRECT3DBASETEXTURE9 pSrcTexture,
    const PALETTEENTRY *pSrcPalette
	*/

	//SaveBMP( lpszFileName, lpData,CSize( nImageBlock, nImageBlock ) );
	//SAFE_DELETE( lpData );

	// 변수 복원 
	Projection( m_pd3dDevice, g_pWorldView->m_d3dsdBackBuffer.Width, g_pWorldView->m_d3dsdBackBuffer.Height );
	m_pCamera->m_vPos = vOldPos;
	m_pCamera->m_vLookAt = vOldLookAt;
	m_bViewFog    = _bViewFog    ;
	m_bViewRegion = _bViewRegion	;
	m_bViewSpawn  = _bViewSpawn	;
	m_bViewName   = _bViewName   ;
	m_bViewLODTerrain    = _bViewLODTerrain    ;
	m_bViewLODObj = _bViewLODObj;
	m_bViewHeightAttribute = _bViewHgtAttr;
	
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = g_pWorldView->m_d3dsdBackBuffer.Width;
	viewport.Height = g_pWorldView->m_d3dsdBackBuffer.Height;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);

	return TRUE;
}

BOOL CWorld::SaveBitmap( LPCTSTR lpszFileName, CD3DFont* pFont, BOOL bViewRegion, BOOL bViewName, BOOL bViewSpawn, BOOL bViewWater, BOOL bViewHeightAttrb )	
{
	int nImageBlock  = 256;//128 * 2;
	int nImageSize   = nImageBlock * m_nLandWidth; //1280 * 2;//( g_pWorldView->m_d3dsdBackBuffer.Height / 8 ); 

	CString strLandName = m_szFileName;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = nImageBlock;//nImageSize;
	viewport.Height = nImageBlock;//nImageSize;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);

	LPBYTE  lpData = new BYTE[ nImageSize * nImageSize * 4 ];
	LPDWORD lpdwData = (LPDWORD) lpData;
	memset( lpData, 255, nImageSize * nImageSize * 4 );
   
	BOOL _bViewLODTerrain = m_bViewLODTerrain;
	BOOL _bViewFog = m_bViewFog;
	BOOL _bViewRegion = m_bViewRegion;
	BOOL _bViewSpawn = m_bViewSpawn;
	BOOL _bViewName = m_bViewName;
	BOOL _bViewLODObj = m_bViewLODObj;
	BOOL _bViewWater = m_bViewWater;
	BOOL _bViewHeightAttrb = m_bViewHeightAttribute;

	m_bViewFog = FALSE;
	m_bViewRegion = bViewRegion;
	m_bViewSpawn = bViewSpawn;
	m_bViewLODTerrain = FALSE;
	m_bViewName = bViewName;
	m_bViewLODObj = FALSE;
	m_bViewWater = bViewWater;
	m_bViewHeightAttribute = bViewHeightAttrb;
	
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

//	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 0,0,0,0) );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

///////////////////////////////////////

	// 변수 세이브 
	D3DXVECTOR3 vOldPos = m_pCamera->m_vPos;
	D3DXVECTOR3 vOldLookAt = m_pCamera->m_vLookAt;
				
	FLOAT fAspect = (FLOAT) 128 / (FLOAT) 128;
	//D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane, CWorld::m_fFarPlane );
	//m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	// 퍼스팩티브 없이 프로젝션 
	//FLOAT fov = D3DX_PI/796.0f * 2.0f;
	FLOAT fov = D3DX_PI/( 199.0f * MPU );		//gmpbigsun : MPU에 따라 변동
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &m_matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	for( int yy = 0; yy < m_nLandHeight; yy++)
	{
		for( int xx = 0; xx < m_nLandWidth; xx++)
		{
			m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 );
			if( m_pd3dDevice->BeginScene() == S_OK )
			{
				((CToolCamera*)m_pCamera)->m_fYaw = 0;
				((CToolCamera*)m_pCamera)->m_fPitch = D3DX_PI/2;
					//= D3DXVECTOR3( (FLOAT) ( xx * MAP_SIZE * MPU ) + ( MAP_SIZE * MPU / 2),   0 , (FLOAT) ( yy * MAP_SIZE * MPU )  + ( MAP_SIZE * MPU / 2) + 0.1f );
				m_pCamera->m_vPos //= m_pCamera->m_vPos2 
					= D3DXVECTOR3( (FLOAT) ( xx * MAP_SIZE * MPU ) + ( MAP_SIZE * MPU / 2), 500 , (FLOAT) ( yy * MAP_SIZE * MPU )  + ( MAP_SIZE * MPU / 2));
				//ReadWorld( m_pCamera->m_vPos );
				BOOL bTempLoad = FALSE;
				if( m_apLand[ yy * m_nLandWidth + xx ] == NULL ) 
				{
					bTempLoad = TRUE;
					CString strLandTemp;
					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, xx , yy );
					CLandscape* pLand = new CLandscape;
					pLand->m_nWorldX = xx * MAP_SIZE;
					pLand->m_nWorldY = yy * MAP_SIZE;
					pLand->InitDeviceObjects( m_pd3dDevice, this );
					if( pLand->LoadLandscape( strLandTemp, xx, yy ) == FALSE )
					{
						delete pLand;
						continue;
					}
					pLand->RestoreDeviceObjects( m_pd3dDevice );
					m_apLand[ yy * m_nLandWidth + xx ] = pLand;
				}
				Render( m_pd3dDevice, pFont );

				IDirect3DSurface9* pDestSurface;
				m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );

				    //hr = m_ptexCur->GetSurfaceLevel(0, &psurfDest);

				D3DLOCKED_RECT lockedRect;
				CRect rectIn ( 0, 0, g_pWorldView->m_d3dsdBackBuffer.Width, g_pWorldView->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
				HRESULT hr = pDestSurface->LockRect( &lockedRect, NULL, 0 ) ;
				if( hr == D3D_OK )// Lock
				{
					LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
					LPWORD pWord = (LPWORD)lockedRect.pBits; // 16bit
					LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

					for(int y = 0; y < nImageBlock; y++ )
					{
						for(int x = 0; x < nImageBlock; x++)
						{
							/*
							// 16 bit color code
							WORD wData = pWord
								[ 
								//( ( nImageSize / 2 - ( nImageBlock / 2 ) + y ) * ( lockedRect.Pitch / 2) ) + ( nImageSize / 2 - ( nImageBlock / 2 ) + x ) 
									y * ( lockedRect.Pitch / 2) + x
								];
							BYTE byData1 = ( wData & 0xf800 ) >> 8;
							BYTE byData2 = ( wData & 0x07e0 ) >> 3;
							BYTE byData3 = ( wData & 0x001f ) << 3 ;
							DWORD dwOffset = 
								//( ( ( 7 - yy ) * nImageBlock + y ) * nImageSize + ( xx * nImageBlock + x ) ) * 3;
								//( y * nImageSize + x) * 3;
								( ( ( m_nLandHeight - yy - 1 ) * nImageBlock + y ) * nImageSize + ( xx * nImageBlock + x ) ) * 3;
							lpData[ dwOffset + 0 ] = byData3;
							lpData[ dwOffset + 1 ] = byData2;
							lpData[ dwOffset + 2 ] = byData1;
							*/
							// 32 bit color code

							
							BYTE byData3 = pSrcByte[ y * lockedRect.Pitch + ( x * 4 ) + 0 ];
							BYTE byData2 = pSrcByte[ y * lockedRect.Pitch + ( x * 4 ) + 1 ];
							BYTE byData1 = pSrcByte[ y * lockedRect.Pitch + ( x * 4 ) + 2 ];

							DWORD dwData = *pSrcDWord;
								//pDWord[ y * lockedRect.Pitch / 4 + x	];
							DWORD dwOffset = 
								( ( ( m_nLandHeight - yy - 1 ) * nImageBlock + y ) * nImageSize + ( xx * nImageBlock + x ) ) * 3;
							lpData[ dwOffset + 0 ] = byData3;
							lpData[ dwOffset + 1 ] = byData2;
							lpData[ dwOffset + 2 ] = byData1;
						}
					}
					pDestSurface->UnlockRect();
				}
				pDestSurface->Release();
				if( bTempLoad )
				{
					for( int i = 0; i < m_nObjCullSize; i++ )
						if( m_aobjCull[ i ] )
							m_aobjCull[ i ]->m_ppViewPtr = NULL;
					for( int i = 0; i < m_nSfxCullSize; i++ )
						if( m_asfxCull[ i ] )
							m_asfxCull[ i ]->m_ppViewPtr = NULL;	
					m_nObjCullSize = 0;
					m_nSfxCullSize = 0;
					m_apLand[ yy * m_nLandWidth + xx ]->InvalidateDeviceObjects();
					m_apLand[ yy * m_nLandWidth + xx ]->DeleteDeviceObjects();
					SAFE_DELETE( m_apLand[ yy * m_nLandWidth + xx ] );
				}
			}
			m_pd3dDevice->EndScene();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			TRACE( "SaveBMP Render x = %d, y = %d\n", xx, yy );
		}
	}
	SaveBMP( lpszFileName, lpData,CSize( nImageSize, nImageSize ) );
	SAFE_DELETE( lpData );

	// 변수 복원 
	Projection( m_pd3dDevice, g_pWorldView->m_d3dsdBackBuffer.Width, g_pWorldView->m_d3dsdBackBuffer.Height );
	m_pCamera->m_vPos = vOldPos;
	m_pCamera->m_vLookAt = vOldLookAt;
	m_bViewFog    = _bViewFog    ;
	m_bViewRegion = _bViewRegion	;
	m_bViewSpawn  = _bViewSpawn	;
	m_bViewName   = _bViewName   ;
	m_bViewLODTerrain    = _bViewLODTerrain    ;
	m_bViewLODObj = _bViewLODObj;
	m_bViewHeightAttribute = _bViewHeightAttrb;
	m_bViewWater = _bViewWater;
	
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = g_pWorldView->m_d3dsdBackBuffer.Width;
	viewport.Height = g_pWorldView->m_d3dsdBackBuffer.Height;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);

	return TRUE;
}
#else	// __BEAST
// __WORLDSERVER

#endif	// __BEAST

#if ( defined( __CLIENT ) || defined( __BEAST ) )
#ifdef __BEAST
BOOL CWorld::ReadWorld()
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );

	for( int y = 0; y < m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_nLandWidth; x++ )
		{
			if( LandInWorld( x, y ) )
			{
				CLandscape* pLand = m_apLand[ y * m_nLandWidth + x];
				if( pLand == NULL )
				{
					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, x , y );
					pLand = new CLandscape;
					pLand->m_nWorldX = x * MAP_SIZE;
					pLand->m_nWorldY = y * MAP_SIZE;
					pLand->InitDeviceObjects( m_pd3dDevice, this );
					if( pLand->LoadLandscape( strLandTemp, x, y ) == FALSE )
					{
						delete pLand;
						continue;
					}
					pLand->RestoreDeviceObjects( m_pd3dDevice );

					m_apLand[ y * m_nLandWidth + x] = pLand;
	//#ifdef __BEAST
					// 비스트에서, ReadObject에서 m_aryObjBuffer에 임시 보관하고 있던 
					// 동적 오브젝트를 랜드를 읽으면서 랜드 영역에 포함되고

					// 랜드에 추가한다. 추가후 m_aryObjBuffer에서 삭제한다.
					for( ObjItor k = m_aryObjBuffer.begin(); k != m_aryObjBuffer.end(); ++k )
					{
						CObj* pObj = *k;
						D3DXVECTOR3 vPos = pObj->GetPos();
						if( vPos.x > pLand->m_nWorldX * MPU && vPos.z > pLand->m_nWorldY * MPU &&
							vPos.x < ( pLand->m_nWorldX + MAP_SIZE ) * MPU && vPos.z < ( pLand->m_nWorldY + MAP_SIZE ) * MPU )
						{/*
							if( pObj->GetType() == OT_REGION )
							{
								((CRegion*)pObj)->InitDeviceObjects( m_pd3dDevice );
								((CRegion*)pObj)->RestoreDeviceObjects( m_pd3dDevice );
							}
							else
							*/
							{
								// 비스트에서는 dyo에서 읽은 놈(초기화된놈)과 rgn에서 읽은 놈(초기화 안된돔)이 
								// 여기서 실제 맵으로 추가된다. 그래서 일관적으로 SetIndex의 마지막 인자를 TRUE를 넣어서
								// 초기화를 해주는데, 이미 초기화가 된 오브젝트는 그 이후 세팅된 일부의 세팅 내용을 날려먹게 된다.
								// 그래서 데미안이 살아 돌아다니는 문제가 발생했다.
								// 비스트에서만 발생한다. 비스트는 dyo를 m_aryObjBuffer 여따 추가하기 때문이다.
								pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex );//, TRUE );
							}
							pObj->SetWorld( this );
							InsertObjLink( pObj );
							AddObjArray( pObj );
							pObj->UpdateLocalMatrix();
							m_aryObjBuffer.erase( k-- );
						}
 					}
				}
			}
		}
	}
	return TRUE;
}
#ifdef __CSC_WORLDOBJ_LIST
void CWorld::ResetWorldObjList(D3DXVECTOR3 vecPos)
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
	{
		pMainFrame->m_pWorldObjList->ResetObjList();
		int x, z;
		WorldPosToLand(vecPos, x, z);

		for(int i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
		{
			for(int j = x - m_nVisibilityLand; j <= ( x + m_nVisibilityLand ); j++)
			{
				if(LandInWorld(j, i))
				{
					CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];

					if(pLand)
						pMainFrame->m_pWorldObjList->MakeObjList(pLand);
				}
			}
		}
		
		pMainFrame->m_pWorldObjList->ResetTree();
	}
}
#endif //__CSC_WORLDOBJ_LIST
#endif // BEAST
BOOL CWorld::ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand  )
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );


	int x, z;
	WorldPosToLand( vPos, x, z );

	// 랜드 이동이 없다. 리턴. 
	if( x == m_nCharLandPosX && z == m_nCharLandPosZ )
		return FALSE;

	m_nCharLandPosX = x;
	m_nCharLandPosZ = z;

#ifdef __BEAST
#ifdef __CSC_WORLDOBJ_LIST
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
		pMainFrame->m_pWorldObjList->ResetObjList();
#endif //__CSC_WORLDOBJ_LIST
#endif //__BEAST

	for(int i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for(int j = x - m_nVisibilityLand; j <= ( x + m_nVisibilityLand ); j++)
		{
			if( LandInWorld( j, i ) )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];

				if( pLand == NULL )
				{
					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, j , i );
					pLand = new CLandscape;
					pLand->m_nWorldX = j * MAP_SIZE;
					pLand->m_nWorldY = i * MAP_SIZE;
					pLand->InitDeviceObjects( m_pd3dDevice, this );
					if( pLand->LoadLandscape( strLandTemp, j, i ) == FALSE )
					{
						delete pLand;
						continue;
					}
					pLand->RestoreDeviceObjects( m_pd3dDevice );
					m_apLand[ i * m_nLandWidth + j] = pLand;
	//#ifdef __BEAST
					// 비스트에서, ReadObject에서 m_aryObjBuffer에 임시 보관하고 있던 
					// 동적 오브젝트를 랜드를 읽으면서 랜드 영역에 포함되고

					// 랜드에 추가한다. 추가후 m_aryObjBuffer에서 삭제한다.
					for( ObjItor k = m_aryObjBuffer.begin(); k != m_aryObjBuffer.end(); )
					{
						CObj* pObj = *k;
						D3DXVECTOR3 vPos = pObj->GetPos();
						if( vPos.x > pLand->m_nWorldX * MPU && vPos.z > pLand->m_nWorldY * MPU &&
							vPos.x < ( pLand->m_nWorldX + MAP_SIZE ) * MPU && vPos.z < ( pLand->m_nWorldY + MAP_SIZE ) * MPU )
						{/*
							if( pObj->GetType() == OT_REGION )
							{
								((CRegion*)pObj)->InitDeviceObjects( m_pd3dDevice );
								((CRegion*)pObj)->RestoreDeviceObjects( m_pd3dDevice );
							}
							else
							*/
							{
								pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex );//, TRUE );
							}

#ifdef __YAIMONSTER_EX
							if( pObj->GetType() == OT_MOVER )
							{
								CRespawnInfo *pInfo = NULL;								
								pInfo = &((CMover*)pObj)->m_respawnInfo;

								if( pInfo->m_cb == 1 )
									((CMover*)pObj)->AddAngle(pInfo->m_fAngle);
							}
#endif //__YAIMONSTER_EX
							
							pObj->SetWorld( this );
							InsertObjLink( pObj );
							AddObjArray( pObj );
							pObj->UpdateLocalMatrix();

							//m_aryObjBuffer.erase( k-- );
							k = m_aryObjBuffer.erase( k );
						}
						else
							++k;
 					}
					// 로드할 때는 내부에서 추가되거나 생성되는 것에 의해 m_bModified가 TRUE가 
					// 될 수 있으므로 FALSE로 바꿔준다. Load할 때 TRUE가 되어선 안된다.
					pLand->SetModified( FALSE );
				}
#ifdef __BEAST
#ifdef __CSC_WORLDOBJ_LIST
				CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
				if(pMainFrame)
					pMainFrame->m_pWorldObjList->MakeObjList(pLand);
#endif //__CSC_WORLDOBJ_LIST
#endif //__BEAST
			}
		}
	}
#ifdef __BEAST
#ifdef __CSC_WORLDOBJ_LIST
	pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
		pMainFrame->m_pWorldObjList->ResetTree();
#endif //__CSC_WORLDOBJ_LIST
#endif //__BEAST
	if( bEraseOldLand )
	{

	}
	return TRUE;
}
BOOL CWorld::MakeMiniMapTexture( int xx, int yy, CTexture* pTexture, LPBYTE pBitmap )
{

	return TRUE;
}
#endif
