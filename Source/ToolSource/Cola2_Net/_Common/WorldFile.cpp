#include "stdafx.h"  
#include "defineObj.h"
#include "region.h"
#include "WorldMng.h"
#include "lang.h"

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

    DWORD dwFileStart = file.GetPosition();

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
#ifdef __WORLDSERVER	
	TCHAR szScriptName[ _MAX_PATH ];
#endif 
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
#ifdef  __V050322_CACHEQUEST
#ifdef __WORLDSERVER
	strcpy( szScriptName, lpszFileName );
	strcat( szScriptName, "\\" );
	strcat( szScriptName, name );
	strcat( szScriptName, ".txt" );
	m_WorldScript.LoadScript( szScriptName );
	m_WorldScript.Run( "#init", NULL, 0, m_dwWorldID, m_dwWorldID, 0, WES_WORLD );
#endif // __BEAST
#endif // __V050322_CACHEQUEST

	CScript script;
	//script.Load( MakePath( DIR_WORLD, "WdMadrigal\\WdMadrigal.rgn" ) );
	if( script.Load( szPathName ) == FALSE )
		return FALSE;
	do {
		script.GetToken();
		if( script.Token == _T( "size" ) )
		{
			m_nLandWidth = script.GetNumber();
			script.GetToken();
			m_nLandHeight = script.GetNumber();
		}
#ifndef __WORLDSERVER
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
#endif	// __WORLDSERVER
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
			script.GetToken();
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
			
#ifdef __YENV
			// 셰이더 넘기기용
			s_fFogStart      = m_fFogStartValue;
			s_fFogEnd        = m_fFogEndValue;	
#endif //__YENV
		}			
#endif //__CLIENT

	} while( script.tok != FINISHED );
	// 파일명 보관 

	_splitpath( szPathName, drive, dir, name, ext );
	strcpy( m_szFilePath, dir );
	strcpy( m_szFileName, name );
	strcat( m_szFileName, ext );

#ifdef __WORLDSERVER
	_tcscpy( m_lpszWorld, m_szFileName );
	
//	link map
	m_linkMap.Init( m_nLandWidth, m_nLandHeight );

	m_apHeightMap = new float[ WORLD_SIZE ];
	memset( m_apHeightMap, 0, sizeof( float ) * WORLD_SIZE );
	m_apWaterHeight = new WATERHEIGHT[ NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight ];
	memset( m_apWaterHeight, 0, sizeof( WATERHEIGHT) * NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight );
	m_apLocation = new BYTE[ NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight ];
	memset( m_apLocation, 0, sizeof( BYTE ) * NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight );

#else	// __WORLDSERVER
	m_apLand = new CLandscape * [ m_nLandWidth * m_nLandHeight ];
	memset( m_apLand, 0, sizeof( CLandscape*) * m_nLandWidth * m_nLandHeight );
#ifndef __ONLINE
	LoadObject();
#endif	// __ONLINE
	LoadRegion();
#endif	// __WORLDSERVER

#ifdef __VLANDATTR
	LoadLandAttr();
#endif
	return TRUE;
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
			
		#ifdef __WORLDSERVER
			BOOL bAddObj = TRUE;

			if( GetPosAttr( pObj->GetPos() ) < 0 )
			{
				bAddObj = FALSE;
			}
			else
			{
				pObj->SetIndex( NULL, pObj->m_dwIndex );
				pObj->InitMotion( pObj->m_dwMotion );
				pObj->UpdateLocalMatrix();

				if( pObj->m_dwType == OT_MOVER )
				{
					LPCHARACTER pCharacter = prj.GetCharacter( ((CMover*)pObj)->m_szCharacterKey );					
					if( pCharacter )
					{
						// raiders_051101 - 마을 경비대장도 PK서버에서만 나오게 수정 
						if( stricmp( pCharacter->m_szKey, "MaFl_GuildWar" ) == 0  ||
							stricmp( pCharacter->m_szKey, "Npc_Reward" ) == 0 )
						{
							if( g_eLocal.GetState( EVE_PK ) == FALSE )
							{
								bAddObj = FALSE;
							}
						}
					}
				}
			
			}

			if( bAddObj )
				AddObj( pObj, TRUE );
			else
			{
				delete pObj;
				continue;
			}
	
		#else	// __WORLDSERVER

			#ifdef __CLIENT
				#ifndef __ONLINE
					if( pObj->IsDynamicObj() )
					( (CCtrl*)pObj )->AddItToGlobalId();
				#endif	// __ONLINE
			#endif	// __CLIENT

			// 비스트에서는 랜드를 모두 읽지 않았기 때문에 동적 오브젝트를 모든 랜드에 추가할 수 없다.
			// 따라서 m_aryObjBuffer에 임시로 보관해 둔다.
			m_aryObjBuffer.push_back( pObj );
		#endif	// __WORLDSERVER

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
		pRe->m_cDescSize = (LONG)s.GetNumber();
		if( pRe->m_cDescSize )
		{
			pRe->m_cDescSize = 1;
			CString string;
			s.GetToken(); // {
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
		}
	}
	else
	{
		s.GetToken(); // get "title"
		BOOL bDesc = s.GetNumber();
		if( bDesc )
		{
			CString string;
			s.GetToken(); // {
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
		}
////////////////////////////////////////////////////////////////////////////////////////////////
		s.GetToken(); // get "desc"
		bDesc = s.GetNumber();
		if( bDesc )
		{
			pRe->m_cDescSize = 1;
			CString string;
			s.GetToken(); // {
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
		}
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__BEAST) //|| defined(__ONLINE)
	CPoint point( vPos.x, vPos.z );
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

#ifdef __S1108_BACK_END_SYSTEM
	pInfo->m_nMaxcb		 = s.GetNumber();
	pInfo->m_cb          = 0;
#else // __S1108_BACK_END_SYSTEM
	pInfo->m_cb          = (u_short)s.GetNumber();
#endif // __S1108_BACK_END_SYSTEM

#ifdef __WORLDSERVER
	pInfo->m_uTime	   = (short)s.GetNumber();
#else	// __WORLDSERVER
	pInfo->m_uTime	= (short)s.GetNumber();	// 60/1
#endif	// __WORLDSERVER

#ifdef __S1108_BACK_END_SYSTEM
	pInfo->m_nMaxAttackNum = (long)s.GetNumber();
	pInfo->m_nActiveAttackNum = 0;
#else // __S1108_BACK_END_SYSTEM
	pInfo->m_nActiveAttackNum = (long)s.GetNumber();
#endif // __S1108_BACK_END_SYSTEM

	pInfo->m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	pInfo->m_fY = vPos.y;
	if( dwType == OT_MOVER )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( dwIndex );
#ifdef __WORLDSERVER
		// raiders 2005.09.23 
		if( pMoverProp && pMoverProp->dwClass == RANK_GUARD && 	g_eLocal.GetState( EVE_PK ) == FALSE ) // PK서버가 아니면 가드를 skip한다.
			return TRUE;

#ifdef	__RESPAWN70
		if( pMoverProp )
		{
			if( pMoverProp->dwClass != RANK_BOSS )
			{
//						pInfo->cb	*= 0.7;
				switch( pMoverProp->dwID )
				{
					case MI_AIBATT1:
					case MI_AIBATT2:
					case MI_AIBATT3:
					case MI_MUSHPANG1:
					case MI_MUSHPANG2:
					case MI_MUSHPANG3:
					case MI_BURUDENG1:
					case MI_BURUDENG2:
					case MI_BURUDENG3:
						pInfo->m_uTime	*= 0.5;
						break;
					default:
						pInfo->m_uTime	*= 0.7;
						break;
				}						
			}
		if( pMoverProp->dwFlying )
			pInfo->m_vPos	= vPos;
		}
#endif	// __RESPAWN70
#endif	// __WORLDSERVER
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
			for( int i = 0; i < MAX_JOB; i++ )
				pInfo->m_CtrlElem.m_bSetJob[ i ] = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetEndu = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMinItemNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMaxiItemNum = s.GetNumber();  
			for( i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemKind[ i ] = s.GetNumber();  
			for( i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemPer[ i ] = s.GetNumber(); 
			for( i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResKind[ i ] = s.GetNumber(); 
			for( i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResNum[ i ] = s.GetNumber(); 
			for( i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonActAttack[ i ] = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapOperType = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapRandomPer = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapDelay = s.GetNumber();  
			for( i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapKind[ i ] = s.GetNumber();  
			for( i = 0; i < MAX_TRAP; i++ )
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
	CPoint point( vPos.x, vPos.z );
	point.x = point.x / MPU * MPU;
	point.y = point.y / MPU * MPU;
	pInfo->m_rect -= point;
#endif	// __BEAST

#ifdef __IVERSION_NOFLY
	if( prj.GetMoverProp( dwIndex )->dwFlying )
		return;
#endif	// __IVERSION_NOFLY

#ifdef __WORLDSERVER
	short nAttr1, nAttr2;
	nAttr1	= GetPosAttr( D3DXVECTOR3( ri.m_rect.left, 0, ri.m_rect.top ) );
	nAttr2	= GetPosAttr( D3DXVECTOR3( ri.m_rect.right, 0, ri.m_rect.bottom ) );

	if( nAttr1 >= 0 && nAttr2 >= 0 ) 
	{
		m_respawner.Add( ri );
	}
	else 
	if( nAttr1 < 0 && nAttr2 < 0 ) 
	{
		// ignore
	}
	else
	{
		CPoint point;
		if( nAttr1 >= 0 )
			point	= CPoint( ri.m_rect.left, ri.m_rect.top );
		else
			point	= CPoint( ri.m_rect.right, ri.m_rect.bottom );

		CRect rcJurisdiction;
		if( GetRect( point, rcJurisdiction ) )
		{
			CRect rcIntersection;
			if( rcIntersection.IntersectRect( (LPCRECT)rcJurisdiction, &ri.m_rect ) )
			{
				ri.m_cb	=	ri.m_cb * rcIntersection.Width() / ( ri.m_rect.right - ri.m_rect.left );
				if( ri.m_cb > 0 )
				{
					memcpy( &ri.m_rect, (LPCRECT)rcIntersection, sizeof(RECT) );
					m_respawner.Add( ri );
				}
			}
		}
	}
#else	// __WORLDSERVER
#if !defined(__BEAST) && !defined(__ONLINE)
	m_respawner.Add( ri );
#endif	// __BEAST	// __ONLINE
#endif	// __WORLDSERVER
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
			if( s.Token == _T( "respawn" ) || s.Token == _T( "respawn2" ) || s.Token == _T( "respawn3" ) )
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
			m_apLand[ nOffset ]->m_nWorldX=j*MAP_SIZE;
			m_apLand[ nOffset ]->m_nWorldY=i*MAP_SIZE;
			m_apLand[ nOffset ]->InitDeviceObjects( m_pd3dDevice, this );
			m_apLand[ nOffset ]->ResetTerrain( fHeight );
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

    DWORD dwFileStart = file.GetPosition();

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
	CString string;
	wchar_t wszBuff[ 1024 ];
	
	D3DXVECTOR3 vPos = pObj->GetPos();
	if( pObj->IsType( OT_REGION ) )
	{
		CRegion* pRegion = (CRegion*) pObj;
		LPREGIONELEM lpRegionElem;
		lpRegionElem = &pRegion->m_region;
		CPoint point( vPos.x, vPos.z );
		CRect rect = lpRegionElem->m_rect;
		point.x = point.x / MPU * MPU;
		point.y = point.y / MPU * MPU;
		rect += point;
#ifdef __N_TRIGGER_AREASET_050203
		string.Format( "region3 %d %d %f %f %f 0x%x %d %d \"%s\" \"%s\" %d %f %f %f %d %d %d %d \"%s\" %d %d %d %d %d %d %d %d %d %d %d %d \r\n", 
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
		string.Format( "region2 %d %d %f %f %f 0x%x %d %d \"%s\" \"%s\" %d %f %f %f %d %d %d %d \"%s\" %d  \r\n", 
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
		_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
		pFile->Write( wszBuff, wcslen( wszBuff) * 2 );

		pFile->PutWideString( "title " );
		if( lpRegionElem->m_szTitle[ 0 ] )
		{
			string = "1 \r\n{\r\n";
			pFile->PutWideString( string );

			int nResult = MultiByteToWideChar( CP_ACP, 0, lpRegionElem->m_szTitle, -1, wszBuff, 1024 );
			wcscpy(lpRegionElem->m_apwszTitle[ prj.m_nLanguage ], wszBuff );
				
			for( int i = 0; i < LANG_MAX; i++ )
			{
				string.Format( "%s ", g_szLanguage[ i ] );
				pFile->PutWideString( string );

				wchar_t wszTitle[ 512 ];
				wcscpy( wszTitle, lpRegionElem->m_apwszTitle[ i ] );
				RegionDesc_StringToSaveFormat( wszTitle );
				pFile->Write( wszTitle, wcslen( wszTitle ) * 2 );

				string = "\r\n";
				pFile->PutWideString( string );
			}
			string = "}\r\n";
			pFile->PutWideString( string );

		}
		else
			pFile->PutWideString( "0\r\n" );
		pFile->PutWideString( "desc " );
		if( lpRegionElem->m_szDesc[ 0 ] )
		{
			string = "1 \r\n{\r\n";
			pFile->PutWideString( string );

			int nResult = MultiByteToWideChar( CP_ACP, 0, lpRegionElem->m_szDesc, -1, wszBuff, 1024 );
			wcscpy(lpRegionElem->m_apwszDesc[ prj.m_nLanguage ], wszBuff );
				
			for( int i = 0; i < LANG_MAX; i++ )
			{
				string.Format( "%s ", g_szLanguage[ i ] );
				pFile->PutWideString( string );

				wchar_t wszDesc[ 512 ];
				wcscpy( wszDesc, lpRegionElem->m_apwszDesc[ i ] );
				RegionDesc_StringToSaveFormat( wszDesc );
				pFile->Write( wszDesc, wcslen( wszDesc ) * 2 );

				string = "\r\n";
				pFile->PutWideString( string );
			}
			string = "}\r\n";
			pFile->PutWideString( string );

		}
		else
			pFile->PutWideString( "0\r\n" );
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
			CPoint point( vPos.x, vPos.z );
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
			string.Format( "respawn3 %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d ", 
				pObj->GetType(), pObj->GetIndex(), vPos.x, vPos.y, vPos.z, pRespawnInfo->m_cb, pRespawnInfo->m_uTime, pRespawnInfo->m_nActiveAttackNum,
				rect.left, rect.top, rect.right, rect.bottom, pRespawnInfo->m_nDayMin, pRespawnInfo->m_nDayMax, pRespawnInfo->m_nHourMin, pRespawnInfo->m_nHourMax, pRespawnInfo->m_nItemMin, pRespawnInfo->m_nItemMax );

			if( pObj->GetType() == OT_CTRL )
			{
				CCtrlElem* pCtrl = &((CCommonCtrl*)pObj)->m_respawnInfo.m_CtrlElem;
				CString string2;
#ifdef __S_DONGEON_0504
				string2.Format( "2  %d %d %d %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d  %d %d   %d %d %d %d  %d %d %d %d  %d %d %d  %d %d %d  %d %d %d   %d %d %d   %d %d %d   %d %d %d   %d %d %d %d", 
					pCtrl->m_dwSet, pCtrl->m_dwSetItem, pCtrl->m_dwSetItemCount, pCtrl->m_dwSetLevel, pCtrl->m_dwSetQuestNum, pCtrl->m_dwSetFlagNum, pCtrl->m_dwSetQuestNum1, pCtrl->m_dwSetFlagNum1, pCtrl->m_dwSetQuestNum2, pCtrl->m_dwSetFlagNum2, pCtrl->m_dwSetGender, 
#else // __S_DONGEON_0504
				string2.Format( "1  %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d  %d  %d %d   %d %d %d %d  %d %d %d %d  %d %d %d  %d %d %d  %d %d %d   %d %d %d   %d %d %d   %d %d %d ", 
					pCtrl->m_dwSet, pCtrl->m_dwSetItem, pCtrl->m_dwSetLevel, pCtrl->m_dwSetQuestNum, pCtrl->m_dwSetFlagNum, pCtrl->m_dwSetGender, 
#endif // __S_DONGEON_0504
					pCtrl->m_bSetJob[ 0 ], pCtrl->m_bSetJob[ 1 ], pCtrl->m_bSetJob[  2 ], pCtrl->m_bSetJob[  3 ], pCtrl->m_bSetJob[  4 ], pCtrl->m_bSetJob[  5 ], pCtrl->m_bSetJob[  6 ], pCtrl->m_bSetJob[  7 ],
					pCtrl->m_bSetJob[ 8 ], pCtrl->m_bSetJob[ 9 ], pCtrl->m_bSetJob[ 10 ], pCtrl->m_bSetJob[ 11 ], pCtrl->m_bSetJob[ 12 ], pCtrl->m_bSetJob[ 13 ], pCtrl->m_bSetJob[ 14 ], pCtrl->m_bSetJob[ 15 ],
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
				string += string2;
			}
			else
			{
				string += "0 ";
			}
			string += "\r\n";

			_mbstowcsz( wszBuff, string, string.GetLength() + 1 );
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
						for( l = 0; l < pLand->m_adwObjNum[ k ]; l++ )
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

	fclose( fp );

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

			}
		}
	}
	fclose( fp );

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
	FLOAT fov = D3DX_PI/796.0f;
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
	Projection( m_pd3dDevice, g_pWorldView->m_d3dsdBackBuffer.Width, (FLOAT)g_pWorldView->m_d3dsdBackBuffer.Height );
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
	FLOAT fov = D3DX_PI/796.0f;
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
					for( i = 0; i < m_nSfxCullSize; i++ )
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
	Projection( m_pd3dDevice, g_pWorldView->m_d3dsdBackBuffer.Width, (FLOAT)g_pWorldView->m_d3dsdBackBuffer.Height );
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
#ifdef __WORLDSERVER
BOOL CWorld::ReadWorld( const CRect & rcLandscape )
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	FLOAT aHeightMap[ ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) ];
	WATERHEIGHT aWaterHeight[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BYTE aLocation[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	int xLand, yLand;
	D3DXVECTOR3	vPos;

	CObj* pBkgnd;
	DWORD dwObjNum;
	
	for( int y = 0; y < m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_nLandWidth; x++ )
		{
//			if( rcLandscape.PtInRect( CPoint( x, y ) ) )
			{
				strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, x , y );

				CFileIO f;
				if( f.Open( strLandTemp, "rb" ) )
				{
					DWORD dwVersion;
					f.Read( &dwVersion, sizeof( DWORD) );
					if( dwVersion >= 1 )
					{
						f.Read( &xLand, sizeof( xLand ) );
						f.Read( &yLand, sizeof( yLand ) );
					}
					f.Read( (void*)aHeightMap, sizeof(FLOAT), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );
#ifdef __IVERSION_NODARKON
					// x가 11 이하면 완전히 갈 수 없도록 하자.
					if( x <= 11 || ( x == 12 && y == 9 ) || ( x == 12 && y == 8 ) )
					{
						for( int i = 0; i < ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ); i++ )
						{
							if( aHeightMap[ i ] >= HGT_DIE )
								aHeightMap[ i ] -= HGT_DIE;
							else
							if( aHeightMap[ i ] >= HGT_NOMOVE )
								aHeightMap[ i ] -= HGT_NOMOVE;
							else
							if( aHeightMap[ i ] >= HGT_NOFLY )
								aHeightMap[ i ] -= HGT_NOFLY;
							else
							if( aHeightMap[ i ] >= HGT_NOWALK )
								aHeightMap[ i ] -= HGT_NOWALK;

							aHeightMap[ i ] += HGT_NOMOVE;
						}
					}
					if( x == 12 && y == 7 ) // 대각선 
					{
						int x2max = 0;
						for( int y2  = 0; y2 < ( MAP_SIZE + 1 ); y2++ )
						{
							for( int x2  = 0; x2 < x2max; x2++ )
							{
								int i = y2 * ( MAP_SIZE + 1 ) + x2;
								if( aHeightMap[ i ] >= HGT_DIE )
									aHeightMap[ i ] -= HGT_DIE;
								else
								if( aHeightMap[ i ] >= HGT_NOMOVE )
									aHeightMap[ i ] -= HGT_NOMOVE;
								else
								if( aHeightMap[ i ] >= HGT_NOFLY )
									aHeightMap[ i ] -= HGT_NOFLY;
								else
								if( aHeightMap[ i ] >= HGT_NOWALK )
									aHeightMap[ i ] -= HGT_NOWALK;

								aHeightMap[ i ] += HGT_NOMOVE;
							}
							x2max++;
							if( x2max >= ( MAP_SIZE + 1 ) ) 
								x2max = ( MAP_SIZE + 1 );
						}
					}
#endif
					for( int y2 = 0; y2 < MAP_SIZE; y2++ )
					{
						for( int x2 = 0; x2 < MAP_SIZE; x2++ )
						{
							m_apHeightMap[ ( ( ( y * MAP_SIZE ) + y2 ) * WORLD_WIDTH ) + ( ( x * MAP_SIZE ) + x2 ) ]
								= aHeightMap[ ( y2 * ( MAP_SIZE + 1 ) ) + x2 ];
						}
					}
					
					// waste
					//f.Seek( ( sizeof(BOOL) * NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), SEEK_CUR );
					//f.Seek( ( sizeof(WATERHEIGHT) * NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), SEEK_CUR );
					f.Read( (void*)aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
					for( y2 = 0; y2 < NUM_PATCHES_PER_SIDE; y2++ )
					{
						for( int x2 = 0; x2 < NUM_PATCHES_PER_SIDE; x2++ )
						{
							m_apWaterHeight[ ( ( ( y * NUM_PATCHES_PER_SIDE ) + y2 ) * ( NUM_PATCHES_PER_SIDE * m_nLandWidth ) ) + ( ( x * NUM_PATCHES_PER_SIDE ) + x2 ) ]
								= aWaterHeight[ ( y2 * ( NUM_PATCHES_PER_SIDE ) ) + x2 ];
						}
					}
					if( dwVersion >= 2 )
					{
						f.Read( aLocation, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
						for( y2 = 0; y2 < NUM_PATCHES_PER_SIDE; y2++ )
						{
							for( int x2 = 0; x2 < NUM_PATCHES_PER_SIDE; x2++ )
							{
								m_apLocation[ ( ( ( y * NUM_PATCHES_PER_SIDE ) + y2 ) * ( NUM_PATCHES_PER_SIDE * m_nLandWidth ) ) + ( ( x * NUM_PATCHES_PER_SIDE ) + x2 ) ]
									= aLocation[ ( y2 * ( NUM_PATCHES_PER_SIDE ) ) + x2 ];
							}
						}
					}
					BYTE nLayer;
					f.Read( &nLayer, sizeof(BYTE), 1 );
					for( BYTE u = 0; u < nLayer; u++ ) {
						f.Seek( sizeof(WORD), SEEK_CUR );
						f.Seek( ( sizeof(BOOL) * NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), SEEK_CUR );
						f.Seek( ( sizeof(DWORD) * MAP_SIZE * MAP_SIZE ), SEEK_CUR );
					}

					// read objects
					f.Read( (void*)&dwObjNum, sizeof(DWORD), 1 );
					for( DWORD dw =  0; dw < dwObjNum; dw++ )
					{
						pBkgnd	= ReadObj( &f );
						vPos	= pBkgnd->GetPos();

						vPos.x	+= x * LANDREALSCALE;
						vPos.z	+= y * LANDREALSCALE;

						pBkgnd->SetPos( vPos );

//						ASSERT( m_dwBkgndNum < MAX_BKGND );
//						ASSERT( pBkgnd->GetType() == OT_OBJ );
						if( pBkgnd->m_dwIndex <= 10 )
							pBkgnd->SetAngle( rand() % 360 );

						pBkgnd->SetIndex( D3DDEVICE, pBkgnd->m_dwIndex );

						if( m_dwBkgndNum >= MAX_BKGND )
						{
							WriteError( "MAXBKGND//0" );
							SAFE_DELETE( pBkgnd );
							continue;
						}
						if( pBkgnd->GetType() != OT_OBJ )
						{
							SAFE_DELETE( pBkgnd );
							continue;
						}
						
						if( prj.m_nLanguage == LANG_JAP && pBkgnd->m_dwIndex == 360 )
						{
							SAFE_DELETE( pBkgnd );
						}
						else
						{
							if( pBkgnd->m_pModel && TRUE == pBkgnd->m_pModel->IsHaveCollObj() )
							{
								pBkgnd->InitMotion( pBkgnd->m_dwMotion );
								pBkgnd->SetWorld( this );
								InsertObjLink( pBkgnd );
								m_apBkgnd[m_dwBkgndNum++]	= pBkgnd;
								pBkgnd->UpdateLocalMatrix();
							}
							else
							{
								SAFE_DELETE( pBkgnd );
							}
						}
					}
/*
					size_t size	= f.Read( &dwObjNum, sizeof(DWORD), 1 );
					if( size )
					{
						for( i = 0; i < dwObjNum; i++ )
						{
						}
					}
*/
				}
				f.Close();
			}
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER
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
								pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex );//, TRUE );
#ifndef __BEAST
								pObj->InitMotion( pObj->m_dwMotion );
#endif
							}
							pObj->SetWorld( this );
							InsertObjLink( pObj );
							AddObjArray( pObj );
							pObj->UpdateLocalMatrix();
							m_aryObjBuffer.erase( k-- );
						}
 					}
					// 로드할 때는 내부에서 추가되거나 생성되는 것에 의해 m_bModified가 TRUE가 
					// 될 수 있으므로 FALSE로 바꿔준다. Load할 때 TRUE가 되어선 안된다.
					pLand->SetModified( FALSE );
				}
			}
		}
	}
	if( bEraseOldLand )
	{
#ifdef __CLIENT
		// 현재 보이지 않는 랜드는 파괴 읽을 때는 3*3을 읽고, 파괴는 현재 위치에서 4*4를 벗어난 것들.
		// 만약 파괴할 때 3*3를 벗어난 것을 파괴하면 캐릭터는 조금만 움직여도 재로딩을 하게 될지 모르기 때문이다.
		// 즉, 3*3을 벗어난 것을 파괴하게 되면 늘 3*3 정도가 로딩을 유지하게 되는데, 
		// 그렇게 되면 로딩 빈도가 잦아져 속도가 느려지게 되는 것이다. 로딩 데이타를 유지하는 것은
		// 4*4로 하여 로딩 빈도를 낮춘다.
		CRect rect( x - m_nVisibilityLand - 1, z - m_nVisibilityLand - 1, x + m_nVisibilityLand + 2, z + m_nVisibilityLand + 2 );
		for( i = 0; i < m_nLandHeight; i++ )
		{
			for( int j = 0; j < m_nLandWidth; j++ )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
				if( pLand )
				{
					if( rect.PtInRect( CPoint( j, i ) ) == FALSE )
					{
						m_apLand[ i * m_nLandWidth + j ]->InvalidateDeviceObjects();
						m_apLand[ i * m_nLandWidth + j ]->DeleteDeviceObjects();
						SAFE_DELETE( m_apLand[ i * m_nLandWidth + j] );
						m_nObjCullSize = 0;
						m_nSfxCullSize = 0;
					}
					else
						pLand->SetUsedAllObjects();
				}
				
			}			
		}
		// 사용하지 않는 모델을 파괴한다.
		// 정적인것만 이리로 들어와야 한다. 동적인것(스키닝을 쓰는것)이 여기서 파괴되면 안된다.
		for( MapStrToPtrItor itor = prj.m_modelMng.m_mapFileToMesh.begin(); itor != prj.m_modelMng.m_mapFileToMesh.end(); /*itor++*/ )
		{
			CModel* pModel = ( CModel* )(*itor).second;
			pModel->DeleteDeviceObjects();
			if( pModel->m_pModelElem->m_bUsed == FALSE &&  pModel->m_pModelElem->m_dwType != OT_ITEM )
			{
				SAFE_DELETE( pModel );
				MapStrToPtrItor itor2 = itor;
				++itor;
				//itor--;
				prj.m_modelMng.m_mapFileToMesh.erase( itor2 );
			}
			else
			{
				pModel->m_pModelElem->m_bUsed = FALSE;
				++itor;
			}
		}
		
#endif	// __CLIENT
	}
	return TRUE;
}
BOOL CWorld::MakeMiniMapTexture( int xx, int yy, CTexture* pTexture, LPBYTE pBitmap )
{
#ifdef __CLIENT
	//int nImageWidth  = pTexture->m_size.cx * m_nLandWidth;
	//int nImageHeight = pTexture->m_size.cy * m_nLandHeight;
	int nImageBlock  = pTexture->m_size.cx;

	////////////////////////////////////////////////////////////////////////////////

	//CRect rect( 0, 0, 256, 256 );
	//CPoint point( 0, 0 );
	IDirect3DSurface9* pOldSurface;
	IDirect3DSurface9* pOldSurfaceZ;
	IDirect3DSurface9* pDstSurface; 
	m_pd3dDevice->GetRenderTarget( 0, &pOldSurface );
	m_pd3dDevice->GetDepthStencilSurface( &pOldSurfaceZ );
	pTexture->m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
	m_pd3dDevice->SetRenderTarget( 0, pDstSurface );//, pOldSurfaceZ );
    
	/*
	D3DLOCKED_RECT lockedRect;
	CRect rectIn( 0, 0, 256, 256 );//g_pD3dApp->m_d3dsdBackBuffer.Width, g_pD3dApp->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	if( pTexture->LockRect( 0, &lockedRect, rectIn, D3DLOCK_DISCARD ) == D3D_OK )// Lock
	{
		LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 

		for(int i = 0; i < nImageBlock; i++ )
		{
			for(int j = 0; j < nImageBlock; j++)
			{
				pSrcByte[ i * lockedRect.Pitch + ( j * 4 ) + 0 ] = 0x00;
				pSrcByte[ i * lockedRect.Pitch + ( j * 4 ) + 1 ] = 0x00;
				pSrcByte[ i * lockedRect.Pitch + ( j * 4 ) + 2 ] = 0x00;;

			}
		}
		pTexture->UnlockRect(0);
	}
	*/
	//pSrcSurface->Release();
	//pDstSurface->Release();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BOOL m_bOldViewSkybox     =  m_bViewSkybox    ; m_bViewSkybox     = FALSE;
	BOOL m_bOldViewFog        =  m_bViewFog       ; m_bViewFog        = FALSE;
	BOOL m_bOldViewName       =  m_bViewName      ; m_bViewName       = FALSE;
	BOOL m_bOldViewLight      =  m_bViewLight     ; m_bViewLight      = FALSE;
	BOOL m_bOldViewWeather    =  m_bViewWeather   ; m_bViewWeather    = FALSE;
	BOOL m_bOldViewLODTerrain =  m_bViewLODTerrain; m_bViewLODTerrain = FALSE;

	m_bMiniMapRender = TRUE;

//	m_bViewWater = FALSE;

	SetFogEnable( m_pd3dDevice, FALSE );
	SetLight( FALSE );

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = nImageBlock;//nImageSize;
	viewport.Height = nImageBlock;//nImageSize;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);

	// 변수 세이브 
	D3DXVECTOR3 vOldPos = m_pCamera->m_vPos;
	D3DXVECTOR3 vOldLookAt = m_pCamera->m_vLookAt;
				
	// 퍼스팩티브 없이 프로젝션 
	FLOAT fAspect = (FLOAT) 128 / (FLOAT) 128;
	FLOAT fov = D3DX_PI/796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &m_matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffff0000/*CWorld::m_dwBgColor*/, 1.0f, 0);
	if( m_pd3dDevice->BeginScene() == S_OK )
	{
		m_pCamera->m_vLookAt
			= D3DXVECTOR3( (FLOAT) ( xx * MAP_SIZE * MPU ) + ( MAP_SIZE * MPU / 2),   0 , (FLOAT) ( yy * MAP_SIZE * MPU )  + ( MAP_SIZE * MPU / 2) + 0.1f );
		m_pCamera->m_vPos //m_pCamera->m_vPos2 
			= D3DXVECTOR3( (FLOAT) ( xx * MAP_SIZE * MPU ) + ( MAP_SIZE * MPU / 2), 500 , (FLOAT) ( yy * MAP_SIZE * MPU )  + ( MAP_SIZE * MPU / 2));
		Render( m_pd3dDevice );
/*
		//CRect rect( 0, 0, 256, 256 );
		//CPoint point( 0, 0 );
		IDirect3DSurface9* pSrcSurface;
		IDirect3DSurface9* pDstSurface; 
		m_pd3dDevice->GetBackBuffer ( 0, D3DBACKBUFFER_TYPE_MONO, &pSrcSurface );
		pTexture->GetSurfaceLevel( 0, &pDstSurface );
		HRESULT hr = m_pd3dDevice->CopyRects( pSrcSurface, NULL, 0, pDstSurface, NULL );
		if( hr == D3DERR_DEVICELOST )
		{
			int a; a++;
		}
		pSrcSurface->Release();
		pDstSurface->Release();
		
		D3DLOCKED_RECT lockedRect;
		CRect rectIn( 0, 0, g_pD3dApp->m_d3dsdBackBuffer.Width, g_pD3dApp->m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
		if( pSrcSurface->LockRect( &lockedRect, rectIn, 0 ) == D3D_OK )// Lock
		{
			LPBYTE pSrcByte = (LPBYTE)lockedRect.pBits; 
			LPWORD pWord = (LPWORD)lockedRect.pBits; // 16bit
			LPDWORD pSrcDWord = (LPDWORD)lockedRect.pBits; // 32bit

			for(int i = 0; i < nImageBlock; i++ )
			{
				for(int j = 0; j < nImageBlock; j++)
				{
					// 32 bit color code
					BYTE byData3 = pSrcByte[ i * lockedRect.Pitch + ( j * 4 ) + 0 ];
					BYTE byData2 = pSrcByte[ i * lockedRect.Pitch + ( j * 4 ) + 1 ];
					BYTE byData1 = pSrcByte[ i * lockedRect.Pitch + ( j * 4 ) + 2 ];
					DWORD dwOffset = ( ( ( m_nLandHeight - yy - 1 ) * nImageBlock + i ) * nImageSize + ( xx * nImageBlock + j ) ) * 3;
					lpData[ dwOffset + 0 ] = byData3;
					lpData[ dwOffset + 1 ] = byData2;
					lpData[ dwOffset + 2 ] = byData1;
				}
			}
			pDestSurface->UnlockRect();
		}
		pSrcSurface->Release();
		pDstSurface->Release();
		*/
	}
	m_pd3dDevice->EndScene();
//	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	//SAFE_DELETE( lpData );
	m_pd3dDevice->SetRenderTarget( 0, pOldSurface );//, pOldSurfaceZ );
	pOldSurface->Release();
	pOldSurfaceZ->Release();
	pDstSurface->Release();

	// 변수 복원 
	Projection( m_pd3dDevice, g_pD3dApp->m_d3dsdBackBuffer.Width, (FLOAT)g_pD3dApp->m_d3dsdBackBuffer.Height );
	m_pCamera->m_vPos = vOldPos;
	m_pCamera->m_vLookAt = vOldLookAt;
	
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = g_pD3dApp->m_d3dsdBackBuffer.Width;
	viewport.Height = g_pD3dApp->m_d3dsdBackBuffer.Height;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);

	m_bViewSkybox     =  m_bOldViewSkybox ;
	m_bViewFog        =  m_bOldViewFog    ;
	m_bViewName       =  m_bOldViewName   ;
	m_bViewLight      =  m_bOldViewLight  ;
	m_bViewWeather    =  m_bOldViewWeather;
	m_bViewLODTerrain =  m_bOldViewLODTerrain;
	m_bMiniMapRender = FALSE;
#endif
	return TRUE;
}
#endif
