// DlgContinent.cpp : implementation file
//


#include "stdafx.h"

#include "DlgContinent.h"
#include "beast.h"
#include ".\dlgcontinent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgContinent dialog

CDlgContinent::CDlgContinent(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgContinent::IDD, pParent)
	, m_bCheckTown(FALSE)
	, m_iBGM(0)
{
	//{{AFX_DATA_INIT(CDlgContinent)
	m_strContinentName = _T("");
	m_efAR = 1.0f;
	m_efAG = 1.0f;
	m_efAB = 1.0f;
	m_efDB = 1.0f;
	m_efDG = 1.0f;
	m_efDR = 1.0f;
	m_fFogEnd = 300.0f;
	m_fFogStart = 20.0f;
	m_iWeather = 0;
	m_bUseSky = FALSE;
	m_bUseCloud = FALSE;
	m_bUseMoon = FALSE;
	m_bUseSun = FALSE;
	m_strSkyTexName = _T("");
	m_strCloudTexName = _T("");
	m_strMoonTexName = _T("");
	m_strSunTexName = _T("");
	m_bUseEnvir = FALSE;
	m_dwID = 0;
	m_bRealData = FALSE;
	//}}AFX_DATA_INIT

	_pCurContinent = NULL;
	_sIndex = 0;
}

CDlgContinent::~CDlgContinent( )
{
	Reset( );
}


void CDlgContinent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgContinent)
	DDX_Text(pDX, IDC_EDIT_CONTINENT, m_strContinentName);
	DDX_Text(pDX, IDC_EDIT_C_AR, m_efAR);
	DDV_MinMaxFloat(pDX, m_efAR, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_C_AG, m_efAG);
	DDV_MinMaxFloat(pDX, m_efAG, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_C_AB, m_efAB);
	DDV_MinMaxFloat(pDX, m_efAB, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_C_DB, m_efDB);
	DDV_MinMaxFloat(pDX, m_efDB, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_C_DG, m_efDG);
	DDV_MinMaxFloat(pDX, m_efDG, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_C_DR, m_efDR);
	DDV_MinMaxFloat(pDX, m_efDR, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_FOG_END, m_fFogEnd);
	DDV_MinMaxFloat(pDX, m_fFogEnd, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_FOG_START, m_fFogStart);
	DDV_MinMaxFloat(pDX, m_fFogStart, 0.f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_WEATHER, m_iWeather);
	DDV_MinMaxInt(pDX, m_iWeather, 0, 5);
	DDX_Check(pDX, IDC_CHECK_USE_SKY, m_bUseSky);
	DDX_Check(pDX, IDC_CHECK_USE_CLOUD, m_bUseCloud);
	DDX_Check(pDX, IDC_CHECK_USE_MOON, m_bUseMoon);
	DDX_Check(pDX, IDC_CHECK_USE_SUN, m_bUseSun);
	DDX_Text(pDX, IDC_EDIT_SKY_TEXTURE, m_strSkyTexName);
	DDX_Text(pDX, IDC_EDIT_CLOUD_TEXTURE, m_strCloudTexName);
	DDX_Text(pDX, IDC_EDIT_MOON_TEXTURE, m_strMoonTexName);
	DDX_Text(pDX, IDC_EDIT_SUN_TEXTURE, m_strSunTexName);
	DDX_Check(pDX, IDC_CHECK_USE_CONTENVIR, m_bUseEnvir);
	DDX_Text(pDX, IDC_EDIT_CONTINENT_NUMBER, m_dwID);
	DDX_Check(pDX, IDC_CHECK_REAL_DATA, m_bRealData);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_TOWN, m_bCheckTown);
	DDX_Text(pDX, IDC_EDIT_BGM, m_iBGM);
}


BEGIN_MESSAGE_MAP(CDlgContinent, CDialog)
	//{{AFX_MSG_MAP(CDlgContinent)
	ON_EN_CHANGE(IDC_EDIT_CONTINENT, OnChangeEditContinent)
	ON_BN_CLICKED(IDTEST, OnButtonSave)
	ON_LBN_SELCHANGE(IDC_LIST_CONTINENT, OnSelchangeListContinent)
	ON_BN_CLICKED(ID_BTN_CONTINENT_ADD, OnBtnContinentAdd)
	ON_BN_CLICKED(ID_BTN_CONTINENT_DELETE, OnBtnContinentDelete)
	ON_EN_CHANGE(IDC_EDIT_C_AR, OnChangeEdit_AmbientValue)
	ON_EN_CHANGE(IDC_EDIT_FOG_START, OnChangeEditFogStart)
	ON_EN_CHANGE(IDC_EDIT_FOG_END, OnChangeEditFogEnd)
	ON_LBN_DBLCLK(IDC_LIST_CONTINENT_VERTEX, OnDblclkListContinentVertex)
	ON_EN_CHANGE(IDC_EDIT_WEATHER, OnChangeEditWeather)
	ON_BN_CLICKED(IDC_CHECK_USE_SKY, OnCheckUseSky)
	ON_BN_CLICKED(IDC_CHECK_USE_CLOUD, OnCheckUseCloud)
	ON_BN_CLICKED(IDC_CHECK_USE_MOON, OnCheckUseMoon)
	ON_BN_CLICKED(IDC_CHECK_USE_SUN, OnCheckUseSun)
	ON_EN_CHANGE(IDC_EDIT_SKY_TEXTURE, OnChangeEditSkyTexture)
	ON_EN_CHANGE(IDC_EDIT_CLOUD_TEXTURE, OnChangeEditCloudTexture)
	ON_LBN_SELCANCEL(IDC_LIST_CONTINENT, OnSelcancelListContinent)
	ON_EN_CHANGE(IDC_EDIT_CONTINENT_NUMBER, OnChangeEditContinentNumber)
	ON_BN_CLICKED(IDC_CHECK_USE_CONTENVIR, OnCheckUseContenvir)
	ON_EN_CHANGE(IDC_EDIT_C_AG, OnChangeEdit_AmbientValue)
	ON_EN_CHANGE(IDC_EDIT_C_AB, OnChangeEdit_AmbientValue)
	ON_EN_CHANGE(IDC_EDIT_C_DR, OnChangeEdit_AmbientValue)
	ON_EN_CHANGE(IDC_EDIT_C_DG, OnChangeEdit_AmbientValue)
	ON_EN_CHANGE(IDC_EDIT_C_DB, OnChangeEdit_AmbientValue)
	ON_BN_CLICKED(IDC_CHECK_REAL_DATA, OnCheckRealData)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_TOWN, OnBnClickedCheckTown)
	ON_EN_CHANGE(IDC_EDIT_BGM, OnEnChangeEditBgm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgContinent message handlers

void CDlgContinent::Init( CWorld* pNewWorld )
{
	Reset( );
	_pWorld = pNewWorld;

	SetWindowText( pNewWorld->m_szFileName );

}

void CDlgContinent::Init( CWorld* pNewWorld, const char* filename )
{
	Init( pNewWorld );

	ReadFile( filename );
	_UpdateContinentList( );
}

void CDlgContinent::Reset( )
{	
	SeqDeleter< ContInfo_Container > ( ) ( _cContInfos );

	_pCurContinent = NULL;
}

ContinentInfo* CDlgContinent::AddZone( const string& _name )
{
	ContinentInfo* pInfo = FindContinent( _name );
	if( !pInfo )
	{
		pInfo = MakeNewContinentZone( _name );
		_cContInfos.push_back( pInfo );
		return pInfo;
	}
	
	return pInfo;
}

void CDlgContinent::SubZone( const string& _name )
{
	for( ContInfo_Iter iter = _cContInfos.begin(); iter != _cContInfos.end(); /*none*/ )
	{
		ContinentInfo* pInfo = *iter;
		if( pInfo->_strName == _name )
		{
			delete pInfo;
			iter = _cContInfos.erase( iter );
			return;
		}else
		{
			++iter;
		}
	}
}

ContinentInfo* CDlgContinent::FindContinent( const string&  _name )
{
	for( ContInfo_Iter iter = _cContInfos.begin(); iter != _cContInfos.end(); ++iter )
	{
		ContinentInfo* pInfo = *iter;
		if( pInfo->_strName == _name )
		{
			return pInfo;
		}
	}

	return NULL;
}

ContinentInfo* CDlgContinent::MakeNewContinentZone( const string& _name )
{
	ContinentInfo* pNewInfo = new ContinentInfo;
	pNewInfo->_index = _sIndex++;
	pNewInfo->_strName = _name;

	return pNewInfo;
}

void CDlgContinent::ConnectVariables( ContinentInfo* pInfo )
{
	m_bUseEnvir = pInfo->_bUseEnvir;
	m_bRealData = pInfo->_bUseRealData;
	m_dwID = pInfo->_dwID;

	m_efAR = pInfo->_fMulAmbient[ 0 ];
	m_efAG = pInfo->_fMulAmbient[ 1 ];
	m_efAB = pInfo->_fMulAmbient[ 2 ];

	m_efDR = pInfo->_fMulDiffuse[ 0 ];
	m_efDG = pInfo->_fMulDiffuse[ 1 ];
	m_efDB = pInfo->_fMulDiffuse[ 2 ];

	m_fFogStart = pInfo->_fFogStart;
	m_fFogEnd = pInfo->_fFogEnd;

	m_iWeather = pInfo->_iWeather;
	m_iBGM = pInfo->_iBGM;

	m_bUseSky = pInfo->_kSky._bUse;
	m_strSkyTexName = pInfo->_kSky._strFileName;

	m_bUseCloud = pInfo->_kCloud._bUse;
	m_strCloudTexName = pInfo->_kCloud._strFileName;
	m_bCheckTown = pInfo->_bTown;

	UpdateData( FALSE );
}

void CDlgContinent::ReadFile( const char* filename )
{
	char fullname[ 256 ];
//	sprintf( filename, "%s%s%s%s", filepath, "/../AllContinent/", _pWorld->m_szFileName, ".cnt" );		//상위폴더AllContinent로!
	sprintf( fullname, "%s%s", filename, ".cnt" );

	CScript script;
	if( script.Load( fullname ) == FALSE )
		return;

	do{
		script.GetToken();

		if( script.Token == _T( "WORLD" ) )
		{
			script.GetToken( );
			if( script.Token != _T( filename ) )
			{
			}
		}
		else
		if( script.Token == _T( "Continent" ) )
		{
			script.GetToken();
			if( script.Token == _T( "BEGIN" ) )
			{
				_pCurContinent = new ContinentInfo;
			}
			else if( script.Token == _T( "END" ) )
			{
				_cContInfos.push_back( _pCurContinent );
				_pCurContinent = NULL;
			}
		}
		else
		if( script.Token == _T( "C_useEnvir" ) )
		{
			_pCurContinent->_bUseEnvir = script.GetNumber( );
		}
		else
		if( script.Token == _T( "C_useRealData" ) )
		{
			_pCurContinent->_bUseRealData = script.GetNumber( );
		}
		else
		if( script.Token == _T( "C_id" ) )
		{
			_pCurContinent->_dwID = script.GetNumber( );
		}
		else
		if( script.Token == _T( "C_name" ) )
		{
			//대륙 이름
			script.GetToken();
			_pCurContinent->_strName = script.token;
		}
		else
		if( script.Token == _T( "C_ambient" ) )
		{
			_pCurContinent->_fMulAmbient[ 0 ] = script.GetFloat();
			_pCurContinent->_fMulAmbient[ 1 ] = script.GetFloat();
			_pCurContinent->_fMulAmbient[ 2 ] = script.GetFloat();
		}
		else
		if( script.Token == _T( "C_diffuse" ) )
		{
			_pCurContinent->_fMulDiffuse[ 0 ] = script.GetFloat( );
			_pCurContinent->_fMulDiffuse[ 1 ] = script.GetFloat( );
			_pCurContinent->_fMulDiffuse[ 2 ] = script.GetFloat( );
		}
		else
		if( script.Token == _T( "C_fogstartend" ) )
		{
			_pCurContinent->_fFogStart = script.GetFloat( );
			_pCurContinent->_fFogEnd = script.GetFloat( );
		}	
		else
		if( script.Token == _T( "C_weather" ) )
		{
			_pCurContinent->_iWeather = script.GetNumber();
		}
		else
		if( script.Token == _T( "C_bgm" ) )
		{
			_pCurContinent->_iBGM = script.GetNumber();
		}
		else
		if( script.Token == _T( "C_sky" ) )
		{
			_pCurContinent->_kSky._bUse = script.GetNumber();
			
			script.GetToken();
			_pCurContinent->_kSky._strFileName = script.Token;
		}
		else
		if( script.Token == _T( "C_cloud" ) )
		{
			_pCurContinent->_kCloud._bUse = script.GetNumber();

			script.GetToken();
			_pCurContinent->_kCloud._strFileName = script.Token;
		}
		else
		if( script.Token == _T( "C_sun" ) )
		{
			_pCurContinent->_kSun._bUse = script.GetNumber();
			
			script.GetToken();
			_pCurContinent->_kSun._strFileName = script.Token;
		}
		else
		if( script.Token == _T( "C_moon" ) )
		{
			_pCurContinent->_kMoon._bUse = script.GetNumber();
			
			script.GetToken();
			_pCurContinent->_kMoon._strFileName = script.Token;
		}
		else
		if( script.Token == _T( "VERTEX" ) )
		{
			float x = script.GetFloat();
			float y = script.GetFloat();
			float z = script.GetFloat();
			_pCurContinent->_cVertices.push_back( D3DXVECTOR3( x, y, z ) );
		}	
		else
		if( script.Token == _T( "TOWN" ) )
		{
			_pCurContinent->_bTown = script.GetNumber();
		}

	}while( script.tok != FINISHED );
}

void CDlgContinent::WriteFile( const char* filepath, const char* filename )
{
	if( _cContInfos.empty( ) )		// 대륙정보가 없다면 파일자체를 만들지 않는다.
		return;

	//TODO : 대륙 정보가 없는데 기존 파일이 남아있는경우 파일 지워줘야 하지 않을까?
	
	FILE* fp;
	char fullname[ 256 ];

	//한단계 상위 디렉토리에 몰아서 저장!!!
//	sprintf( filename, "%s%s%s%s", _pWorld->m_szFilePath, "/../AllContinent/", _pWorld->m_szFileName, ".cnt" );
	sprintf( fullname, "%s%s%s", filepath, filename, ".cnt" );
	if( !( fp = fopen( fullname, "wt" ) ) )
		return;

	fprintf( fp, __T( "WORLD %s" ), filename );

	for( ContInfo_Iter iter = _cContInfos.begin(); iter != _cContInfos.end(); ++iter )
	{
		ContinentInfo* pInfo = *iter;

		fprintf( fp, __T( "\n\nContinent BEGIN" ) );

		//마을인가?	클라에서 단독으로 쓰일 데이터 ( 전체맵 등 )
		fprintf( fp, __T( "\nTOWN %d" ), pInfo->_bTown );

		fprintf( fp, __T( "\nC_useEnvir %d" ), pInfo->_bUseEnvir );
		fprintf( fp, __T( "\nC_useRealData %d" ), pInfo->_bUseRealData );

		//대륙이름
		fprintf( fp, __T( "\nC_name %s" ), pInfo->_strName.c_str() );
		fprintf( fp, __T( "\nC_id %d" ), pInfo->_dwID );

		fprintf( fp, __T( "\nC_bgm %d" ), pInfo->_iBGM );

		if( pInfo->_bUseEnvir )	//환경정보를 이용할 경우만 output
		{
			fprintf( fp, __T( "\nC_ambient %6.3f %6.3f %6.3f" ), pInfo->_fMulAmbient[0], pInfo->_fMulAmbient[1], pInfo->_fMulAmbient[2] );
			fprintf( fp, __T( "\nC_diffuse %6.3f %6.3f %6.3f" ), pInfo->_fMulDiffuse[0], pInfo->_fMulDiffuse[1], pInfo->_fMulDiffuse[2] );
			fprintf( fp, __T( "\nC_fogstartend %6.3f %6.3f" ), pInfo->_fFogStart, pInfo->_fFogEnd );
			fprintf( fp, __T( "\nC_weather %d" ), pInfo->_iWeather );

			fprintf( fp, __T( "\nC_sky %d %s" ), pInfo->_kSky._bUse, pInfo->_kSky._strFileName.GetString() );
			fprintf( fp, __T( "\nC_cloud %d %s" ), pInfo->_kCloud._bUse, pInfo->_kCloud._strFileName.GetString() );
			fprintf( fp, __T( "\nC_sun %d %s" ), pInfo->_kSun._bUse, pInfo->_kSun._strFileName.GetString() );
			fprintf( fp, __T( "\nC_moon %d %s" ), pInfo->_kMoon._bUse, pInfo->_kMoon._strFileName.GetString() );
		}
								
		for( Vec3D_Iter iter2 = pInfo->_cVertices.begin(); iter2 != pInfo->_cVertices.end(); ++iter2 )
		{
			//대륙 정점 정보 
			D3DXVECTOR3 kPos = *iter2;
			fprintf( fp, __T( "\nVERTEX %d %d %d" ), (int)kPos.x, (int)kPos.y, (int)kPos.z );
		}

		fprintf( fp, __T( "\nContinent END" ) );
	}

	fclose( fp );

}

void CDlgContinent::AddExternalVertex( const D3DXVECTOR3& kPos )
{
	//외곽 정점 입력
	_pCurContinent->_cVertices.push_back( kPos );

	_UpdateVerticesList( );
}

void CDlgContinent::SubExternalVertex_Back( )
{
	// 외각 정점중 최후미 제거 
	if( _pCurContinent->_cVertices.empty() )
		return;

	_pCurContinent->_cVertices.pop_back( );

	_UpdateVerticesList( );
}

void CDlgContinent::_UpdateContinentList( )
{
	// 대륙 정보 리스트 갱신
	HWND hList = ::GetDlgItem( GetSafeHwnd(), IDC_LIST_CONTINENT );
	::SendMessage( hList, LB_RESETCONTENT, 0, 0 );	

	for( ContInfo_Iter iter =  _cContInfos.begin(); iter != _cContInfos.end(); ++iter )
	{
		ContinentInfo* pInfo = *iter;

		pInfo->_index = ::SendMessage( hList, LB_INSERTSTRING, -1, (LPARAM)pInfo->_strName.c_str() );
	}

	if( _pCurContinent )
		::SendMessage( hList, LB_SETCURSEL, _pCurContinent->_index, 0 );
}

void CDlgContinent::_UpdateContinentName( )
{
	// 대륙 이름 갱신
	HWND hEdit = ::GetDlgItem( GetSafeHwnd(), IDC_EDIT_CONTINENT );

	if( _pCurContinent )
		::SetWindowText( hEdit, _pCurContinent->_strName.c_str() );
	else ::SetWindowText( hEdit, "" );
}

void CDlgContinent::_UpdateSelectedContinent( const string& strCont )
{
	//선택한 대륙 Ready to edit
	ContinentInfo* pInfo = FindContinent( strCont );
	if( pInfo )
	{
		_pCurContinent = pInfo;
		_UpdateVerticesList( );
		_UpdateContinentName( );

		ConnectVariables( pInfo );
	}

}

void CDlgContinent::_UpdateVerticesList( )
{
	//리셋 후 현재 외각 정점 정보로 다시 갱신
	HWND hList = ::GetDlgItem( GetSafeHwnd(), IDC_LIST_CONTINENT_VERTEX );
	::SendMessage( hList, LB_RESETCONTENT, 0, 0 );

	if( !_pCurContinent )
		return;
	
	int idx = 0;
	char str[ 128 ];
	
	for( Vec3D_Iter iter = _pCurContinent->_cVertices.begin(); iter != _pCurContinent->_cVertices.end(); ++iter )
	{
		++idx;

		D3DXVECTOR3& kPos = *iter;
		sprintf( str, "%d : [ %d, %d ]", idx, (int)kPos.x, (int)kPos.z );

		kPos.y = _pWorld->GetLandHeight( kPos.x, kPos.z ) + 3.0f;	// 높이정보 채움 

		::SendMessage( hList, LB_ADDSTRING, 0, (LPARAM)str ); 
	}

}

Vec3D_Container& CDlgContinent::GetCurVertices( )
{
	return _pCurContinent->_cVertices;
}

void CDlgContinent::OnChangeEditContinent() 
{
	
	// TODO: Add your control notification handler code here
//	HWND hEdit = ::GetDlgItem( GetSafeHwnd(), IDC_EDIT_CONTINENT );
	
//	char str[ 64 ];
//	::GetWindowText(hEdit, str, 64);//에디터 박스에서 문자열을 가져옴

	UpdateData( TRUE );

	if( _pCurContinent )
		_pCurContinent->_strName = m_strContinentName.GetString();

	_UpdateContinentList( );
}

void CDlgContinent::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
//	HWND hList = ::GetDlgItem( GetSafeHwnd(), IDC_LIST_CONTINENT );

	UpdateData( TRUE );
	_pWorld->StartTEV( );
}


void CDlgContinent::OnSelchangeListContinent() 
{
	// TODO: Add your control notification handler code here

	HWND hList = ::GetDlgItem( GetSafeHwnd(), IDC_LIST_CONTINENT );

	char str[ 128 ];
	int sel = ::SendMessage( hList, LB_GETCURSEL, 0, 0 );
	::SendMessage( hList, LB_GETTEXT, sel, (LPARAM)str );

	_UpdateSelectedContinent( str );

	g_pWorldView->UpdateLineVertex( );

	//TEST 대륙에 들어왔다고 치고
	InContinent( );
}

void CDlgContinent::OnBtnContinentAdd() 
{
	// TODO: Add your control notification handler code here

	static int idx = 0;
	const char* pName = "NEW_";

	char chLastName[ 128 ];
	sprintf( chLastName, "%s%d", pName, idx );
	++idx;

	_pCurContinent = AddZone( chLastName );
	ConnectVariables( _pCurContinent );
	_UpdateContinentName( );

	//TEST 대륙에 들어왔다고 치고
	InContinent( );
}

void CDlgContinent::OnBtnContinentDelete() 
{
	// TODO: Add your control notification handler code here

	if( _pCurContinent )
	{
		SubZone( _pCurContinent->_strName );

		_pCurContinent = NULL;

		_UpdateContinentName( );
		_UpdateContinentList( );
		_UpdateVerticesList( );

		OutContinent( );
	}
	
}

void CDlgContinent::OnChangeEdit_AmbientValue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	if( !_pCurContinent )
		return;

	UpdateData( TRUE );

	if( _pCurContinent )
	{
		_pCurContinent->_fMulAmbient[ 0 ] = m_efAR;
		_pCurContinent->_fMulAmbient[ 1 ] = m_efAG;
		_pCurContinent->_fMulAmbient[ 2 ] = m_efAB;

		_pCurContinent->_fMulDiffuse[ 0 ] = m_efDR;
		_pCurContinent->_fMulDiffuse[ 1 ] = m_efDG;
		_pCurContinent->_fMulDiffuse[ 2 ] = m_efDB;
	}

	g_pWorldView->Invalidate( TRUE );
	
}

void CDlgContinent::OnChangeEditFogStart() 
{
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	if( !_pCurContinent )
		return;

	UpdateData( TRUE );

	_pCurContinent->_fFogStart = m_fFogStart;

	g_pWorldView->Invalidate( TRUE );

}

void CDlgContinent::OnChangeEditFogEnd() 
{
	// TODO: Add your control notification handler code here

	if( !_pCurContinent )
		return;

	UpdateData( TRUE );

	_pCurContinent->_fFogEnd = m_fFogEnd;

	g_pWorldView->Invalidate( TRUE );
	
}

void CDlgContinent::InContinent( )
{
	_pWorld->m_bIsContinent = TRUE;

	g_pWorldView->Invalidate( TRUE );
}

void CDlgContinent::OutContinent( )
{
	_pWorld->m_bIsContinent = FALSE;
	_pCurContinent = NULL;

	_UpdateContinentList( );
	g_pWorldView->Invalidate( TRUE );
}


void CDlgContinent::OnDblclkListContinentVertex() 
{
	// TODO: Add your control notification handler code here

	HWND hList = ::GetDlgItem( GetSafeHwnd(), IDC_LIST_CONTINENT_VERTEX );

	int index = ::SendMessage( hList, LB_GETCURSEL, 0, 0 );
	
	char str[ 128 ];
	::SendMessage( hList, LB_GETTEXT, index, (LPARAM)str );

	string temp( str );
	size_t f1 = temp.find( '[' );
	size_t c = temp.find(',');
	size_t f2 = temp.find( ']' );

	char strBuffer[ 64 ];

	std::copy( temp.begin() + f1 + 1, temp.begin() + c, strBuffer );
	float x = (float)( atof( strBuffer ) );

	std::copy( temp.begin() + c + 1, temp.begin() + f2, strBuffer );
	float z = (float)( atof( strBuffer ) );

	float y = _pWorld->GetLandHeight( x, z ) ;

	CToolCamera* pCam = ( CToolCamera* )_pWorld->m_pCamera;
	assert( pCam );

	D3DXVECTOR3 kPointPos( x, y, z );

	D3DXVECTOR3 kCamPos = pCam->GetPos( );
	D3DXVECTOR3 kDir( 0.0f, -1.0f, 0.0f );

	float camY = ( pCam->GetPos() ).y - kPointPos.y ;
	kCamPos = kPointPos - kDir * camY;
	
	pCam->SetPos( kCamPos );

	pCam->m_fYaw = 0.0f;
	pCam->m_fPitch = D3DX_PI / 2.0f;

	g_pWorldView->Invalidate( TRUE );
}

void CDlgContinent::OnChangeEditWeather() 
{

	// TODO: Add your control notification handler code here

	if( !_pCurContinent )
		return;

	UpdateData( TRUE );

	if( 4 == m_iWeather )
		AfxMessageBox( "Not supported anymore." );
	
	_pCurContinent->_iWeather = m_iWeather;
}


void CDlgContinent::OnCheckUseSky() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	_pCurContinent->_kSky._bUse = m_bUseSky;

}

void CDlgContinent::OnCheckUseCloud() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	_pCurContinent->_kCloud._bUse = m_bUseCloud;

}

void CDlgContinent::OnCheckUseMoon() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData( TRUE );

	_pCurContinent->_kMoon._bUse = m_bUseMoon;
	
}

void CDlgContinent::OnCheckUseSun() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );
	
	_pCurContinent->_kSun._bUse = m_bUseSun;

}

void CDlgContinent::OnChangeEditSkyTexture() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	_pCurContinent->_kSky._strFileName = m_strSkyTexName.GetString();
}

void CDlgContinent::OnChangeEditCloudTexture() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData( TRUE );

	_pCurContinent->_kCloud._strFileName = m_strCloudTexName.GetString();
	
}

void CDlgContinent::OnSelcancelListContinent() 
{
	// TODO: Add your control notification handler code here
}

void CDlgContinent::OnChangeEditContinentNumber() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );
	
	_pCurContinent->_dwID = m_dwID;
	
}

void CDlgContinent::OnCheckUseContenvir() 
{
	// TODO: Add your control notification handler code here

	// 이 체크박스가 on된다면 대륙 환경을 이용한다는 의미임
	if( !_pCurContinent )
	{
		AfxMessageBox( "대륙을 선택해야 합니다 " );

		UpdateData( FALSE );
		return;
	}

	UpdateData( TRUE );

	_pCurContinent->_bUseEnvir = m_bUseEnvir;

	if( !m_bUseEnvir )
		AfxMessageBox( "This continent matches the continental environment with the world, the continental environment values are ignored.." );
	else
		AfxMessageBox( "Ignore the world environment and use the continental environment. Please set the environment value" );

	g_pWorldView->Invalidate( TRUE );
	
}

void CDlgContinent::OnCheckRealData() 
{
	// TODO: Add your control notification handler code here

	if( !_pCurContinent )
	{
		UpdateData( FALSE );
		return;
	}

	UpdateData( TRUE );

	_pCurContinent->_bUseRealData = m_bRealData;

	if( !m_bRealData )
		AfxMessageBox( "Use only visual changes in the client environment" );
	else 
		AfxMessageBox( "t is used as real continent data in the game!! Is it correct?" );
}

void CDlgContinent::OnBnClickedCheckTown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 마을 지정  체크버튼 클릭
	if( !_pCurContinent )
	{
		UpdateData( FALSE );
		return;
	}

	UpdateData( TRUE );

	if( m_bCheckTown )
	{
		if( AfxMessageBox( "It is used as data only for town boundaries. It cannot be used for environment variables and other data. Do you really want to set it as a town?", MB_OKCANCEL ) == IDCANCEL )
		{
			m_bCheckTown = FALSE;
			UpdateData( FALSE );
			return;
		}
	}
	

	_pCurContinent->_bTown = m_bCheckTown;

	if( m_bCheckTown )
	{
		// disable real data and envir
		CButton* pEnvirBtn = (CButton*) GetDlgItem( IDC_CHECK_USE_CONTENVIR );
		if( pEnvirBtn )
			pEnvirBtn->EnableWindow( FALSE );

		m_bUseEnvir = FALSE;

		CButton* pRealDataBtn = ( CButton* )GetDlgItem( IDC_CHECK_REAL_DATA );
		if( pRealDataBtn )
			pRealDataBtn->EnableWindow( FALSE );

		m_bRealData = FALSE;

		UpdateData( FALSE );
	}
	else
	{
		// enable real data and envir
		CButton* pEnvirBtn = (CButton*) GetDlgItem( IDC_CHECK_USE_CONTENVIR );
		if( pEnvirBtn )
			pEnvirBtn->EnableWindow( TRUE );

		CButton* pRealDataBtn = ( CButton* )GetDlgItem( IDC_CHECK_REAL_DATA );
		if( pRealDataBtn )
			pRealDataBtn->EnableWindow( TRUE );
	}
}

void CDlgContinent::OnEnChangeEditBgm()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );
	
	_pCurContinent->_iBGM = m_iBGM;
}
