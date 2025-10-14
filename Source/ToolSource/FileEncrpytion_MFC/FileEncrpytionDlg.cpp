// FileEncrpytionDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FileEncrpytion.h"
#include "FileEncrpytionDlg.h"
#include ".\fileencrpytiondlg.h"

#include "CommonTemp/File.h"
// //////////////////////////////////////////////////////////////////////////
// //	BEGINTEST100111	FFL_DUMP
#ifdef	_DEBUG
#pragma comment( lib, "FLCommond.lib" )
#pragma comment( lib, "FLCryptod.lib" )
#else
#pragma comment( lib, "FLCommon.lib" )
#pragma comment( lib, "FLCrypto.lib" )
#endif
// //	ENDTEST100111	FFL_DUMP
// //////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND	g_hWndStatus;
int		g_aWidths[7] = {0,};
#define ID_STATUSBAR		0xf0

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFileEncrpytionDlg 대화 상자



CFileEncrpytionDlg::CFileEncrpytionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileEncrpytionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileEncrpytionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CONFIGFILE_NAME, m_staticConfigFileName);
	DDX_Control(pDX, IDC_STATIC_ENCRYPTION, m_staticEncryption);
	DDX_Control(pDX, IDC_STATIC_DECRYPTION, m_staticDecryption);
	DDX_Control(pDX, IDC_EDIT_STRING, m_editString);
	DDX_Control(pDX, IDC_EDIT_ENCRYPTION_DATA, m_editEncryptionData);
	DDX_Control(pDX, IDC_EDIT_DECRYPTION_DATA, m_editDecryptionData);
	DDX_Control(pDX, IDC_STATIC_ENCRYPTION_ORGDATA, m_staticEncryptionOrgData);
	DDX_Control(pDX, IDC_STATIC_DECRYPTION_ORGDATA, m_staticDecryptionOrgData);
	DDX_Control(pDX, IDC_STATIC_DECRYPTION_LISTFILE, m_staticDecryptionListFile);
	DDX_Control(pDX, IDC_STATIC_LISTFILE_NAME, m_staticListFileName);
	DDX_Control(pDX, IDC_STATIC_RESFILE_NAME, m_staticResFileName);
	DDX_Control(pDX, IDC_EDIT_MD5DATA, m_editMD5Data);
	DDX_Control(pDX, IDC_EDIT_ENCRYPTION_MD5, m_editEncryptionMD5Data);
}

BEGIN_MESSAGE_MAP(CFileEncrpytionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_WM_DROPFILES()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_STN_DBLCLK(IDC_STATIC_CONFIGFILE_NAME, OnStnClickedStaticFileName)
	ON_BN_CLICKED(IDC_BTN_ENCRYPTION, OnBnClickedBtnEncryption)
	ON_BN_CLICKED(IDC_BTN_DECRYPTION, OnBnClickedBtnDecryption)
	ON_BN_CLICKED(IDC_BTN_OPENFOLDER, OnBnClickedBtnOpenFolder)
	ON_BN_CLICKED(IDC_BTN_ENCRYPTION_STRING, OnBnClickedBtnEncryptionString)
	ON_BN_CLICKED(IDC_BTN_DECRYPTION_STRING, OnBnClickedBtnDecryptionString)
	ON_BN_CLICKED(IDC_BTN_CONFIG_FILEOPEN, OnBnClickedBtnConfigFileOpen)
	ON_BN_CLICKED(IDC_BTN_LIST_FILE_OPEN, OnBnClickedBtnListFileOpen)
	ON_BN_CLICKED(IDC_BTN_VIEW_DECRYPTION_LISTFILE, OnBnClickedBtnViewDecryptionListfile)
	ON_BN_CLICKED(IDC_BTN_DECRYPTION_LISTFILE, OnBnClickedBtnDecryptionListfile)
	ON_BN_CLICKED(IDC_BTN_VIEW_OPENED_CONFIGFILE, OnBnClickedBtnViewOpenedConfigfile)
	ON_BN_CLICKED(IDC_BTN_RES_FILE_OPEN, OnBnClickedBtnResFileOpen)
	ON_BN_CLICKED(IDC_BTN_SPLIT_RESFILE, OnBnClickedBtnSplitResfile)
	ON_BN_CLICKED(IDC_BTN_ENCRYPTION_MD5, OnBnClickedBtnEncryptionMd5)
END_MESSAGE_MAP()


// CFileEncrpytionDlg 메시지 처리기

BOOL CFileEncrpytionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(TRUE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//	Create Server Status Bar
	g_hWndStatus = CreateWindowEx(0L, STATUSCLASSNAME, "",
		WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,
		0, 0, 0, 0, GetSafeHwnd(), (HMENU)ID_STATUSBAR, theApp.m_hInstance, NULL);

	if( g_hWndStatus == NULL )
	{
		DestroyWindow();
	}

	::SendMessage(g_hWndStatus, SB_SETPARTS, 6, (LPARAM)g_aWidths);
	::SendMessage(g_hWndStatus, SB_SETTEXT, 0, (LPARAM)( "FileEncrpytion" ) );
	::SendMessage(g_hWndStatus, SB_SETTEXT, 1, (LPARAM)"");
	::SendMessage(g_hWndStatus, SB_SETTEXT, 2, (LPARAM)"");
	::SendMessage(g_hWndStatus, SB_SETTEXT, 3, (LPARAM)"");
	::SendMessage(g_hWndStatus, SB_SETTEXT, 4, (LPARAM)"");


//	m_staticConfigFileName.SetWindowText( "No Data" );
//	m_staticEnctyptionFileName.SetWindowText( "Double Click Make Encryption Data." );
//	m_staticDectyptionFileName.SetWindowText( "Double Click Make Decryption Data." );

	::memset( m_szResFileName, 0, sizeof(m_szResFileName) );

	OnStnClickedStaticFileName();
	SendMessage( WM_SIZE, 0, 0 );

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

//void CFileEncrpytionDlg::OnDropFiles( HDROP hDropInfo )
//{
//	TCHAR	szFileName[MAX_PATH] = {0, };
//
//	UINT	nFileCount = 0;
//	nFileCount	= DragQueryFile( hDropInfo, -1, NULL, 0 );
//
//
//	if( nFileCount != 1 )
//	{
//		m_staticConfigFileName.SetWindowText( "파일 열기로 암호화 / 복호화 할 파일을 선택하세요." );
//
//		return;
//	}
//	else
//	{
//		::memset( szFileName, 0, sizeof(szFileName) );
//		DragQueryFile( hDropInfo, 0, (LPTSTR)szFileName, sizeof(szFileName) );
//
//		m_staticConfigFileName.SetWindowText( szFileName );
//		m_xConfigFileEncryption.SetSourceFile( szFileName );
//
//		m_staticEncryption.SetWindowText( "" );
//		m_staticDecryption.SetWindowText( "" );
//	}
//
//	return;
//}

void CFileEncrpytionDlg::OnSize( UINT nType, int cx, int cy )
{
	RECT	rc;
	int		nStatusGap;

	GetClientRect( &rc);

	if(g_hWndStatus != NULL)
	{
		RECT	rcStatus;

		::MoveWindow(g_hWndStatus, 0, rc.bottom-10, rc.right, rc.bottom, TRUE);
		g_aWidths[0] = (rc.right *2) / 6;
		g_aWidths[1] = (rc.right *3) / 6;
		g_aWidths[2] = (rc.right *4) / 6;
		g_aWidths[3] = (rc.right *5) / 6;
		g_aWidths[4] = (rc.right *6) / 6;
		::SendMessage(g_hWndStatus, SB_SETPARTS, 6, (LPARAM)g_aWidths);

		::GetClientRect(g_hWndStatus, &rcStatus);
		nStatusGap = rcStatus.bottom - rcStatus.top +1;
	}

// 	GetClientRect(hWnd, &rcClient);
// 	cxSize = rcClient.right - rcClient.left +1;
// 	cySize = rcClient.bottom - rcClient.top +1;
// 
// 	if(g_hWndLog != NULL)
// 	{
// 		MoveWindow(g_hWndLog, 0, 0, cxSize, cySize-14, TRUE);
// 	}

	return;
}

void CFileEncrpytionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CFileEncrpytionDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CFileEncrpytionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





//////////////////////////////////////////////////////////////////////////
//	CONFIG FILE 관련
//////////////////////////////////////////////////////////////////////////
void CFileEncrpytionDlg::OnStnClickedStaticFileName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_staticConfigFileName.SetWindowText( "파일 열기로 암호화 / 복호화 할 Config 파일을 선택하세요." );
	m_staticEncryption.SetWindowText( "" );
	m_staticDecryption.SetWindowText( "" );

	m_xConfigFileEncryption.SetSourceFile( NULL );

	return;
}


void CFileEncrpytionDlg::OnBnClickedBtnConfigFileOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CFileDialog dlg( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , "프리프 Config 파일 (*.*)|*.*|" );

	CString strFileName;
	TCHAR	szFileName[MAX_PATH] = {0, };
	::memset( szFileName, 0, sizeof(szFileName) );

	if( dlg.DoModal() == IDOK ) 
	{
		strFileName = dlg.GetPathName();

		m_staticConfigFileName.SetWindowText( strFileName.GetBuffer() );
		m_xConfigFileEncryption.SetSourceFile( strFileName.GetBuffer() );

		m_staticEncryption.SetWindowText( "" );
		m_staticDecryption.SetWindowText( "" );

	}

	return;
}

void CFileEncrpytionDlg::OnBnClickedBtnViewOpenedConfigfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_xConfigFileEncryption.IsSetSourceFile() == true )
	{
		TCHAR	szExec[MAX_PATH];
		::memset( szExec, 0, sizeof(szExec) );

		FLSPrintf( szExec, _countof(szExec), "notepad.exe %s", m_xConfigFileEncryption.GetSourceFileName() );
		::WinExec( szExec, 1 );
	}
	else
	{
		MessageBox( "Config 파일이 설정 되지 않았습니다.", MB_OK );
	}

	return;

}


void CFileEncrpytionDlg::OnBnClickedBtnEncryption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_xConfigFileEncryption.IsSetSourceFile() == true )
	{
		m_xConfigFileEncryption.EncryptionFile();

		m_staticEncryption.SetWindowText( m_xConfigFileEncryption.GetEncryptionFileName() );
	}
	else
	{
		MessageBox( "암호화할 Config 파일이 설정 되지 않았습니다.", MB_OK );
	}

	return;
}

void CFileEncrpytionDlg::OnBnClickedBtnDecryption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_xConfigFileEncryption.IsSetSourceFile() == true )
	{
		m_xConfigFileEncryption.DecryptionFile();

		m_staticDecryption.SetWindowText( m_xConfigFileEncryption.GetDecryptionFileName() );

	}
	else
	{
		MessageBox( "복호화할 Config 파일이 설정 되지 않았습니다.", MB_OK );
	}

	return;
}

void CFileEncrpytionDlg::OnBnClickedBtnOpenFolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

// 	if( m_xConfigFileEncryption.IsSetSourceFile() == false )
// 	{
		TCHAR	szExec[MAX_PATH];
		::memset( szExec, 0, sizeof(szExec) );

		TCHAR	szDrive[MAX_PATH];
		::memset( szDrive, 0, sizeof(szDrive) );
		TCHAR	szDirectory[MAX_PATH];
		::memset( szDirectory, 0, sizeof(szDirectory) );

		::_tsplitpath( m_xConfigFileEncryption.GetSourceFileName(), szDrive, szDirectory, NULL, NULL );

		::FLSPrintf( szExec, _countof(szExec), "explorer.exe %s%s", szDrive, szDirectory );
		::WinExec( szExec, 1 );
// 	}

	return;
}





//////////////////////////////////////////////////////////////////////////
//	STRING ENCRYPTION 관련
//////////////////////////////////////////////////////////////////////////
void CFileEncrpytionDlg::OnBnClickedBtnEncryptionString()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	BYTE szInputData[64] = {0, };
	m_editString.GetWindowText( (LPTSTR)( szInputData ), sizeof(szInputData) );

	size_t	nInputDataCount = 0;
	nInputDataCount	= strlen( (char *)( szInputData ) );

	if( nInputDataCount == 0 )
	{
		MessageBox( "암호화할 데이타를 입력하지 않으셨습니다.", MB_OK );

		return;
	}

	if( nInputDataCount > 24 )
	{
		MessageBox( "암호화할 데이타의 길이가 24자를 초과하였습니다.", MB_OK );

		return;
	}

	static BYTE keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	FL_des3_context ctx3;
	des3_set_3keys( &ctx3, keys[0], keys[1], keys[2] );


	//////////////////////////////////////////////////////////////////////////
	BYTE output[64];
	::memset( output, 0, sizeof(output) );

	for( int i=0; i<3; i++ )
	{
		BYTE byBuffer[8] = { 0, };
		::memcpy( byBuffer, szInputData+i*8, 8 );
		::des3_encrypt( &ctx3, byBuffer, output + i*8 );
	}

	char szEncryptionData[256] = {0, };
	for( int i=0; i<24; i++ )
	{
		FLSPrintf( szEncryptionData + ( i * 2 ), _countof(szEncryptionData), "%02X", output[i] );
	}

	::memset( output, 0, sizeof(output) );


	m_staticEncryptionOrgData.SetWindowText( (LPCTSTR)( szInputData ) );
	m_editEncryptionData.SetWindowText( (LPCTSTR)( szEncryptionData ) );

	return;
}

void CFileEncrpytionDlg::OnBnClickedBtnDecryptionString()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	BYTE szInputData[64] = {0, };
	m_editString.GetWindowText( (LPTSTR)( szInputData ), sizeof(szInputData) );

	size_t	nInputDataCount = 0;
	nInputDataCount	= strlen( (char *)( szInputData ) );

	if( nInputDataCount != 48 )
	{
		MessageBox( "암호화할 데이타의 길이가 48자가 아닙니다.", MB_OK );

		return;
	}

	static BYTE keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	FL_des3_context ctx3;
	des3_set_3keys( &ctx3, keys[0], keys[1], keys[2] );


	//////////////////////////////////////////////////////////////////////////

	BYTE szDecryptionData[256] = {0, };
	BYTE byPWD[256]	= { 0, };

	int nCount = 0;
	nCount	= (int)( strlen( (char *)( szInputData ) ) / 2 );

	if( nCount > 0 )
	{
		char szBuf[3] = {0, };
		for( int i=0; i<nCount; i++ )
		{
			memcpy( szBuf, szInputData + i*2, 2 );						
			sscanf( szBuf, "%02x", (byPWD + i) );
		}
	}

	BYTE* pResult = szDecryptionData;
	for( int i=0; i<3; i++ )
	{
		des3_decrypt( &ctx3, byPWD + i*8, szDecryptionData + i*8 );
	}

	m_staticDecryptionOrgData.SetWindowText( (LPCTSTR)( szInputData ) );
	m_editDecryptionData.SetWindowText( (LPCTSTR)( szDecryptionData ) );

	return;
}






//////////////////////////////////////////////////////////////////////////
//	LIST FILE 관련
//////////////////////////////////////////////////////////////////////////

void CFileEncrpytionDlg::OnBnClickedBtnListFileOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CFileDialog dlg( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , "프리프 List 파일 (*.*)|*.*|" );

	CString strFileName;
	TCHAR	szFileName[MAX_PATH] = {0, };
	::memset( szFileName, 0, sizeof(szFileName) );

	if( dlg.DoModal() == IDOK ) 
	{
		strFileName = dlg.GetPathName();

		m_staticListFileName.SetWindowText( strFileName.GetBuffer() );
		m_xListFileEncryption.SetSourceFile( strFileName.GetBuffer() );

		m_staticDecryptionListFile.SetWindowText( "" );

	}

	return;
}

void CFileEncrpytionDlg::OnBnClickedBtnViewDecryptionListfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	TCHAR szDecryptionListFileName[MAX_PATH] = {0, };
	m_staticDecryptionListFile.GetWindowText( (LPTSTR)( szDecryptionListFileName ), sizeof(szDecryptionListFileName) );

	size_t	nFileNameCount = 0;
	nFileNameCount = _tcslen(szDecryptionListFileName);

	if( nFileNameCount > 0 )
	{
		TCHAR	szExec[MAX_PATH];
		::memset( szExec, 0, sizeof(szExec) );

		FLSPrintf( szExec, _countof(szExec), "notepad.exe %s", szDecryptionListFileName );
		::WinExec( szExec, 1 );
	}
	else
	{
		MessageBox( "복호화한 List 파일이 설정 되지 않았습니다.", MB_OK );
	}

	return;

}

void CFileEncrpytionDlg::OnBnClickedBtnDecryptionListfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_xListFileEncryption.IsSetSourceFile() == true )
	{
		m_xListFileEncryption.DecryptionFile();

		m_staticDecryptionListFile.SetWindowText( m_xListFileEncryption.GetDecryptionFileName() );

	}
	else
	{
		MessageBox( "복호화할 List 파일이 설정 되지 않았습니다.", MB_OK );
	}

	return;
}






//////////////////////////////////////////////////////////////////////////
//	RES 파일 관련
//////////////////////////////////////////////////////////////////////////
void CFileEncrpytionDlg::OnBnClickedBtnResFileOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CFileDialog dlg( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , "프리프 Res 파일 (*.*)|*.*|" );

	CString strFileName;
	TCHAR	szFileName[MAX_PATH] = {0, };
	::memset( szFileName, 0, sizeof(szFileName) );

	if( dlg.DoModal() == IDOK ) 
	{
		strFileName = dlg.GetPathName();

		m_staticResFileName.SetWindowText( strFileName.GetBuffer() );

		::memcpy( m_szResFileName, strFileName.GetBuffer(), strFileName.GetLength());
//		m_xListFileEncryption.SetSourceFile( strFileName.GetBuffer() );
	}

	return;

}



//////////////////////////////////////////////////////////////////////////
BYTE	g_byEncryptionKey = 0x57;


typedef struct tagFILE_INFO
{
	CString FilePath;
	int FileSize;
	CTime m_mtime; 
}FILE_INFO;

struct FILE_INFO_EX : public FILE_INFO
{
	int nFilePosition;
};


BOOL F( LPCTSTR lpszFileName, CMapStringToPtr &mapStringToPtr )
{
	int nFileHeaderSize = 0;
	int nFileNumber = 0;
	int nFileNameLength = 0;
	char szFileName[_MAX_FNAME] = "";
	int nFileSize = 0;
	int nFilePosition = 0;
	BYTE byEncryptionKey;
	bool bEncryption;

	//	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	//	_splitpath( lpszResName, drive, dir, name, ext );

	CFile file;
	if( file.Open( lpszFileName, CFile::modeRead ) == FALSE )
		return FALSE;
	file.Read( &byEncryptionKey, sizeof( byEncryptionKey ) );
	file.Read( &bEncryption, sizeof( bEncryption ) );
	file.Read( &nFileHeaderSize, sizeof( int ) );

	char *pHeader = new char[ nFileHeaderSize ];
	// 해더는 암호 기본 
	//if( m_bEncryption ) // 암호화 되었다면
	{
		char *pCryptHeader = new char[ nFileHeaderSize ];
		file.Read( pCryptHeader, nFileHeaderSize );

		for( int i = 0; i < nFileHeaderSize; i++ )
		{
			pHeader[i] = CResFile::Decryption( byEncryptionKey, pCryptHeader[ i ] );
		}

		delete [] pCryptHeader;
	}
	//else // 암호화 안되었다면
	//{
	//	m_File.Read( pHeader, nFileHeaderSize );
	//}
	int nHeaderPosition = 0;
	char strVersion[8] ="";
	memcpy( strVersion, &pHeader[ nHeaderPosition ], 7 ); nHeaderPosition += 7;

	memcpy( &nFileNumber, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );

	time_t time_;
	for( int i = 0; i < nFileNumber; i++ )
	{
		//	FILEOUT( "c:\\debug.txt", "2 %s %s %d \n", lpszFileName, szFileName, nFileNumber );

		memcpy( &nFileNameLength, &pHeader[ nHeaderPosition ], sizeof( short ) ); nHeaderPosition += sizeof( short );
		memcpy( szFileName, &pHeader[ nHeaderPosition ], nFileNameLength ); nHeaderPosition += nFileNameLength;
		memcpy( &nFileSize, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		memcpy( &time_, &pHeader[ nHeaderPosition ], sizeof( time_t ) ); nHeaderPosition += sizeof( time_t );
		memcpy( &nFilePosition, &pHeader[ nHeaderPosition ], sizeof( int ) ); nHeaderPosition += sizeof( int );
		FILE_INFO_EX* pFileInfo = new FILE_INFO_EX;

		pFileInfo->FileSize = nFileSize;  // 
		pFileInfo->FilePath = szFileName;
		pFileInfo->m_mtime = time_;
		pFileInfo->nFilePosition = nFilePosition;

		_strlwr( szFileName );
		mapStringToPtr.SetAt( szFileName, pFileInfo );
		ZeroMemory( szFileName, sizeof( szFileName ) );
	}
	delete [] pHeader;

	file.Close();
	return TRUE;
}

void CFileEncrpytionDlg::OnBnClickedBtnSplitResfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( _tcslen(m_szResFileName) == 0 )
	{
		MessageBox( "Res 파일이 설정 되지 않았습니다.", MB_OK );

		return;
	}

	CMapStringToPtr mapStringToPtr ;
	F( m_szResFileName, mapStringToPtr );

	if( mapStringToPtr.GetCount() )
	{
		TCHAR	szFolderName[MAX_PATH];
		::memset( szFolderName, 0, sizeof(szFolderName) );

		::_tsplitpath( m_szResFileName, NULL, NULL, szFolderName, NULL );

		BOOL	bCreateDir = CreateDirectory( szFolderName, NULL );

		if( bCreateDir == false )
		{
			int	nRetValue = MessageBox( "해당 파일명의 디렉토리 생성에 실패하였습니다.\n해당 파일명의 디렉토리를 지우고 다시 시도 하시기 바랍니다.", "디렉토리 생성 실패", MB_OK );

			return;
		}

		POSITION pos = mapStringToPtr.GetStartPosition();
		while( pos )
		{
			CString strFile;
			FILE_INFO_EX* pFileInfo;

			mapStringToPtr.GetNextAssoc( pos, strFile,(void*&) pFileInfo );
			int nSize = pFileInfo->FileSize;
			CFile file( m_szResFileName, CFile::modeRead );
			BYTE *pBuffer = new BYTE[ nSize ];
			file.Seek( pFileInfo->nFilePosition, CFile::begin );
			file.Read( pBuffer, nSize );
			file.Close();

			CFile out;
			
			TCHAR	szFilePath[MAX_PATH];
			::memset( szFilePath, 0, sizeof(szFilePath) );

			::FLSPrintf( szFilePath, _countof(szFilePath), ".\\%s\\%s", szFolderName, pFileInfo->FilePath );
			
			if( out.Open( szFilePath, CFile::modeCreate | CFile::modeWrite ) )
			{
				for( int i = 0; i < nSize; i++ )
				{
					pBuffer[ i ] = CResFile::Decryption( g_byEncryptionKey, pBuffer[ i ] );
				}

				out.Write( pBuffer, nSize );
			}
			else
			{
				printf( "%s file create error.\n", pFileInfo->FilePath );
			}

			delete pFileInfo;
			delete [] pBuffer;

		}
	}

	return;
}

void CFileEncrpytionDlg::OnBnClickedBtnEncryptionMd5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	char szInputData[64] = {0, };
	m_editMD5Data.GetWindowText( (LPTSTR)( szInputData ), sizeof(szInputData) );

	size_t	nInputDataCount = 0;
	nInputDataCount	= strlen( (char *)( szInputData ) );

	if( nInputDataCount == 0 )
	{
		MessageBox( "MD5 암호화할 데이타를 입력하지 않으셨습니다.", MB_OK );

		return;
	}

	//////////////////////////////////////////////////////////////////////////
	char output[64];
	::memset( output, 0, sizeof(output) );

	::md5( output, _countof( output ), szInputData );

	m_editEncryptionMD5Data.SetWindowText( (LPCTSTR)( output ) );

	return;
}
