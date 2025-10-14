#if !defined(AFX_DLGCONTINENT_H__C7D2348F_1EC3_43DA_AE09_39CA22436483__INCLUDED_)
#define AFX_DLGCONTINENT_H__C7D2348F_1EC3_43DA_AE09_39CA22436483__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgContinent.h : header file
//

//author : gmpbigsun
//date : 2009_08_20

/////////////////////////////////////////////////////////////////////////////
// CDlgContinent dialog

typedef vector< D3DXVECTOR3 >		Vec3D_Container;
typedef Vec3D_Container::iterator	Vec3D_Iter;

struct TextureSet
{
	TextureSet( ) { _bUse = 0, _strFileName = "NO", _pTexture = NULL; }
	~TextureSet( ) { SAFE_RELEASE( _pTexture ); }
	BOOL _bUse;
	CString _strFileName;
	IDirect3DTexture9* _pTexture;
};

struct ContinentInfo
{
	ContinentInfo( )
	{
		_bUseEnvir = FALSE;
		_bUseRealData = FALSE;
		_bTown = FALSE;
		_index = 0;
		_dwID = 0;

		for( int i = 0; i < 3; ++i )
		{
			_fMulAmbient[ i ] = 0.5f;
			_fMulDiffuse[ i ] = 0.5f;
		}

		_fFogStart = 70.0f;
		_fFogEnd = 400.0f;

		_iWeather = 0;
		_iBGM = 0;
	}
	int _index;			// 내부 리스트 인덱스 
	DWORD _dwID;		// 대륙 아이디 ( 중요 )!
	string _strName;
	BOOL	_bUseEnvir;
	BOOL	_bTown;
	BOOL	_bUseRealData;
	float _fMulAmbient[3];
	float _fMulDiffuse[3];
	float _fFogStart;
	float _fFogEnd;
	int	  _iWeather;
	int   _iBGM;

	TextureSet _kSky;
	TextureSet _kCloud;
	TextureSet _kSun;
	TextureSet _kMoon;
	
	Vec3D_Container _cVertices;
};

typedef vector< ContinentInfo* >		ContInfo_Container;			//이름이 변경될수있으므로 pair로 안씀.
typedef ContInfo_Container::iterator	ContInfo_Iter;


struct WORLD_ENVIR_INFO
{
	TextureSet _kSky[ 3 ];		// 0 : night, 1 : day, 2 : evening
	TextureSet _kCloud[ 3 ];
	TextureSet _kSun;
	TextureSet _kMoon;
};


class CDlgContinent : public CDialog
{
// Construction
public:
	CDlgContinent(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgContinent( );

// Dialog Data
	//{{AFX_DATA(CDlgContinent)
	enum { IDD = IDD_DIALOG_CONTINENT_INFO };
	CString	m_strContinentName;
	float	m_efAR;
	float	m_efAG;
	float	m_efAB;
	float	m_efDB;
	float	m_efDG;
	float	m_efDR;
	float	m_fFogEnd;
	float	m_fFogStart;
	int		m_iWeather;
	BOOL	m_bUseSky;
	BOOL	m_bUseCloud;
	BOOL	m_bUseMoon;
	BOOL	m_bUseSun;
	CString	m_strSkyTexName;
	CString	m_strCloudTexName;
	CString	m_strMoonTexName;
	CString	m_strSunTexName;
	BOOL	m_bUseEnvir;
	DWORD	m_dwID;
	BOOL	m_bRealData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgContinent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	ContInfo_Container _cContInfos;		//대륙 정보들 ( 한개의 월드에 복수개의 대륙이 존재 )

	int _sIndex;

public:
	ContinentInfo*	_pCurContinent;

public:
	void Init( CWorld* pNewWorld );		// 새로운 월드 생성시 
	void Init( CWorld* pNewWorld, const char* filename );	// ".cnt"파일을 읽을 경우 ( 기존의 월드를 불러올때 )
	
	void InContinent( );
	void OutContinent( );
	void Reset( );
	void WriteFile( const char* filepath, const char* filename );
	void AddExternalVertex( const D3DXVECTOR3& kPos );	//외각 정점 입력
	void SubExternalVertex_Back( );		//외각 정점중 최후미 제거 
	Vec3D_Container& GetCurVertices( );

	ContinentInfo*	AddZone( const string& _name );
	void			SubZone( const string& _name );
	ContinentInfo*	FindContinent( const string& _name );

protected:
	void ReadFile( const char* filename );
	ContinentInfo* MakeNewContinentZone( const string& _name );
	void ConnectVariables( ContinentInfo* );

	void _UpdateContinentList( );							//전체 대륙정보 업데이트 
	void _UpdateContinentName( );
	void _UpdateSelectedContinent( const string& strCont );	//선택한 대륙 정보로 업데이트 
	void _UpdateVerticesList( );	//리셋 후 현재 외각 정점 정보로 다시 갱신

protected:
	CWorld* _pWorld;


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgContinent)
	afx_msg void OnChangeEditContinent();
	afx_msg void OnButtonSave();
	afx_msg void OnSelchangeListContinent();
	afx_msg void OnBtnContinentAdd();
	afx_msg void OnBtnContinentDelete();
	afx_msg void OnChangeEdit_AmbientValue();
	afx_msg void OnChangeEditFogStart();
	afx_msg void OnChangeEditFogEnd();
	afx_msg void OnDblclkListContinentVertex();
	afx_msg void OnChangeEditWeather();
	afx_msg void OnCheckUseSky();
	afx_msg void OnCheckUseCloud();
	afx_msg void OnCheckUseMoon();
	afx_msg void OnCheckUseSun();
	afx_msg void OnChangeEditSkyTexture();
	afx_msg void OnChangeEditCloudTexture();
	afx_msg void OnSelcancelListContinent();
	afx_msg void OnChangeEditContinentNumber();
	afx_msg void OnCheckUseContenvir();
	afx_msg void OnCheckRealData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCheckTown;
	afx_msg void OnBnClickedCheckTown();
	int m_iBGM;
	afx_msg void OnEnChangeEditBgm();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONTINENT_H__C7D2348F_1EC3_43DA_AE09_39CA22436483__INCLUDED_)
