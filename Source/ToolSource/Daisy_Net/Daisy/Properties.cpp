// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "daisy.h"
#include "Properties.h"
#include "MainFrm.h"
#include "ResourceView.h"
#include "WndNeuz.h"
#include "DaisyDoc.h"
#include "DaisyView.h"
#include "properties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TCHAR PROGRAM_NAME[128];
/////////////////////////////////////////////////////////////////////////////
// CProperties

IMPLEMENT_DYNAMIC(CProperties, CPropertySheet)

CProperties::CProperties(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CProperties::CProperties(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CProperties::~CProperties()
{
}


BEGIN_MESSAGE_MAP(CProperties, CPropertySheet)
	//{{AFX_MSG_MAP(CProperties)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProperties message handlers


/////////////////////////////////////////////////////////////////////////////
// CPropAppletGeneral property page


IMPLEMENT_DYNCREATE(CPropAppletGeneral, CPropertyPage)

CPropAppletGeneral::CPropAppletGeneral() : CPropertyPage(CPropAppletGeneral::IDD)
{
	//{{AFX_DATA_INIT(CPropAppletGeneral)
	m_nWidth = 0;
	m_nHeight = 0;
	m_bTile = FALSE;
	m_strToolTip = _T("");
	m_strSurfaceFormat = _T("");
	//}}AFX_DATA_INIT
	m_pWndApplet_ = NULL;
	m_pWndDialog = NULL;
	m_pTexture = NULL;
}

CPropAppletGeneral::~CPropAppletGeneral()
{
	SAFE_DELETE( m_pTexture );
}

void CPropAppletGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropAppletGeneral)
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	DDX_Check(pDX, IDC_TILE, m_bTile);
	DDX_Text(pDX, IDC_TOOLTIP, m_strToolTip);
	DDV_MaxChars(pDX, m_strToolTip, 128);
	DDX_CBString(pDX, IDC_SURFACE_FORMAT, m_strSurfaceFormat);
	DDV_MaxChars(pDX, m_strSurfaceFormat, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropAppletGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPropAppletGeneral)
	ON_BN_CLICKED(IDC_TEXTURE_BROWSER, OnTextureBrowser)
	ON_BN_CLICKED(IDC_FIT_TEXUTRE_SIZE, OnFitTexutreSize)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_IMG, OnBnClickedButtonDefaultImg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropAppletGeneral message handlers


BOOL CPropAppletGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	//m_bError = FALSE;
	CComboBox* pWIDC    = (CComboBox*)GetDlgItem( IDC_WIDC    );
	CEdit*     pCaption = (CEdit    *)GetDlgItem( IDC_CAPTION );
	CEdit*     pEdit    = (CEdit    *)GetDlgItem( IDC_TEXTURE );
	CEdit*     pEdit_Default_Img    = (CEdit    *)GetDlgItem( IDC_EDIT_DEFAULT_IMG );
	CComboBox* pFormat  = (CComboBox*)GetDlgItem( IDC_SURFACE_FORMAT );

	m_strOldDefine = CScript::GetFindIdStr( _T( "APP_" ), m_pWndDialog->GetWndId() );
	pWIDC->SetWindowText( m_strOldDefine );
	m_strOldTexture = m_pWndApplet_->strTexture;
	m_bTile = m_pWndApplet_->m_bTile;

	m_pTexture = new CTexture;
	m_pTexture->LoadTexture( m_pWndDialog->m_pApp->m_pd3dDevice, m_pWndApplet_->strTexture, 0xffff00ff, TRUE ); 

	CStringArray strArray;
	CScript::GetFindIdToArray( _T( "APP_" ), &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		pWIDC->AddString( strArray.GetAt( i ) );
	}
	pCaption->SetWindowText( m_pWndDialog->GetTitle() );
	pEdit->SetWindowText( m_pWndApplet_->strTexture );
	pEdit_Default_Img->SetWindowText( m_pWndApplet_->strDefaultImg );

	m_nWidth = m_pWndApplet_->size.cx + m_pWndApplet_->nDefSizeX;
	m_nHeight = m_pWndApplet_->size.cy + m_pWndApplet_->nDefSizeY;
	m_strToolTip = m_pWndApplet_->strToolTip;

	pFormat->AddString( "D3DFMT_A4R4G4B4" );
	pFormat->AddString( "D3DFMT_A8R8G8B8" );
	switch( m_pWndApplet_->d3dFormat )
	{
		case D3DFMT_A4R4G4B4:
			m_strSurfaceFormat = "D3DFMT_A4R4G4B4";
			break;
		case D3DFMT_A8R8G8B8:
			m_strSurfaceFormat = "D3DFMT_A8R8G8B8";
			break;
	}
	//m_strHelpKey = m_pWndApplet_->szHelpKey;
	
	UpdateData( FALSE );

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPropAppletGeneral::OnOK( )
{
	UpdateData( TRUE );

	// TODO: Add your message handler code here
	CComboBox* pWIDC    = (CComboBox*)GetDlgItem( IDC_WIDC    );
	CEdit*     pCaption = (CEdit    *)GetDlgItem( IDC_CAPTION );
	CEdit*     pTexture = (CEdit*)GetDlgItem( IDC_TEXTURE );

	CEdit*     pEdit_Default_Img = (CEdit*)GetDlgItem( IDC_EDIT_DEFAULT_IMG );

	CString str;
	pWIDC->GetWindowText( str );
	int nBegin = str.Find( _T( "APP_" ) );
	if( nBegin != 0 ) 
	{
		AfxMessageBox( _T( "Window의 ID가 APP_로 시작하지 않습니다." ) );
		return;
	}

	/*
	CString string = CScript::GetFindIdStr( _T( "APP_" ), m_pWndDialog->GetWndId() );
	pWIDC->SetWindowText( string );

	CStringArray strArray;
	CScript::GetFindIdToArray( _T( "APP_" ), &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		pWIDC->AddString( strArray.GetAt( i ) );
	}
	pCaption->SetWindowText( m_pWndDialog->GetTitle() );
	*/
	CString strTitle;
	pCaption->GetWindowText( strTitle );
	DWORD dwNewAppletId;
	if( CWndBase::m_resMng.ChangeAppletDefine( m_strOldDefine, str, &dwNewAppletId ) )
	{
		CResourceView* pResourceView = (CResourceView*)g_pMainFrame->m_wndSTCBar.GetView( 0 );
		pResourceView->ChangeApplet( m_strOldDefine, str );
		//pCaption->GetWindowText( strTitle );
		m_pWndDialog->SetWndId( dwNewAppletId );
		m_pWndDialog->SetTitle( strTitle );

		// WNDAPPLET 업데이트
		// 수정되었으면 외국어 데이타를 날린다.
#ifndef __USE_STR_TBL0407
		if( m_pWndApplet_->strTitle != strTitle )
		{
			for( int i = 0; i < LANG_MAX; i++ )
				m_pWndApplet_->awszTitle[ i ][ 0 ] = 0;
		}
		if( m_pWndApplet_->strToolTip != m_strToolTip )
		{
			for( int i = 0; i < LANG_MAX; i++ )
				m_pWndApplet_->awszToolTip[ i ][ 0 ] = 0;
		}
#endif	// __USE_STR_TBL0407

		// flyingjin : Log
		if(!strncmp(strTitle,"IDS_",3))
		{
			FILEOUT( "Daisy_Log.txt","==================================================");
			FILEOUT( "Daisy_Log.txt","CPropAppletGeneral::OnOK_Title [%s]",strTitle );
			strTitle.Format("");
		}
		if(!strncmp(m_strToolTip,"IDS_",3))
		{
			FILEOUT( "Daisy_Log.txt","==================================================");
			FILEOUT( "Daisy_Log.txt","CPropAppletGeneral::OnOK_ToolTip[%s]",m_strToolTip );
			m_strToolTip.Format("");
		}
		m_pWndApplet_->strTitle = strTitle;
		m_pWndApplet_->strToolTip = m_strToolTip;


		pTexture->GetWindowText( m_pWndApplet_->strTexture );
		pEdit_Default_Img->GetWindowText(m_pWndApplet_->strDefaultImg);
		if( m_bTile )
			m_pWndDialog->m_strTexture = m_pWndApplet_->strTexture;
		else
		{
#ifdef __TOON
			if( m_pWndDialog->m_strTexture.GetLength() <= 0 )
				m_pWndDialog->m_strTexture.Empty();

			m_pWndDialog->m_strTexture = m_pWndApplet_->strTexture;
#else
			m_pWndDialog->m_strTexture = m_pWndApplet_->strTexture;
#endif 
		}

		if( m_strOldTexture != m_pWndApplet_->strTexture )
		{
//			m_pWndDialog->m_bWndTile = m_bTile;
			if( m_bTile == FALSE )
				m_pWndDialog->SetTexture( m_pWndDialog->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),m_pWndApplet_->strTexture ), TRUE );
			else
			{
				m_pWndDialog->SetWndRect( m_pWndDialog->GetWindowRect( TRUE ) );
				m_nWidth = m_pWndDialog->GetWndRect().Width();
				m_nHeight = m_pWndDialog->GetWndRect().Height();
				// SetWndRect에 TRUE를 불러도 동일 사이즈로 부르기 때문에 OnSize를 호출하지 못한다.
				// 따라서 직접 불러서 AdjustWndBase를 호출하게 해야한다.
				m_pWndDialog->OnSize( 0, m_nWidth, m_nHeight );
			}
		}
		if( m_strSurfaceFormat == "D3DFMT_A4R4G4B4" )
			m_pWndApplet_->d3dFormat = D3DFMT_A4R4G4B4;
		else
		if( m_strSurfaceFormat == "D3DFMT_A8R8G8B8" )
			m_pWndApplet_->d3dFormat = D3DFMT_A8R8G8B8;

		m_pWndApplet_->m_bTile = m_bTile;
		m_pWndApplet_->size.cx = m_nWidth;
		m_pWndApplet_->size.cy = m_nHeight;
		m_pWndApplet_->m_bTile = m_bTile; 
		m_pWndDialog->m_bTile = m_bTile ? true : false;
		m_pWndDialog->m_d3dFormat = m_pWndApplet_->d3dFormat;
		m_pWndDialog->SetDefaultImg(m_pWndApplet_->strDefaultImg);

		CRect rect = m_pWndDialog->GetWindowRect( TRUE );
		rect.right = rect.left + m_nWidth;
		rect.bottom = rect.top + m_nHeight;
		m_pWndDialog->SetWndRect( rect );
		m_pWndDialog->AdjustWndBase();//>OnSize( 0, m_nWidth, m_nHeight );
		m_pDaisyView->InvalidateRect( FALSE );
		//m_pWndApplet_->strToolTip = m_strToolTip;
		//strcpy( m_pWndApplet_->szHelpKey, m_strHelpKey );
		
	}
	else
		AfxMessageBox( _T( "지정하신 ID는 다른 Applet에서 사용하고 있습니다." ) );

	CWndBase::m_resMng.SetModifiedScript();
}
void CPropAppletGeneral::OnFitTexutreSize() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	if( m_pTexture )
	{
		m_nWidth = m_pTexture->m_size.cx;
		m_nHeight = m_pTexture->m_size.cy;
	}
	UpdateData( FALSE );
	
}

void CPropAppletGeneral::OnTextureBrowser() 
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Windows or os/2 file(*.png)|*.png|Windows or os/2 file(*.bmp)|*.bmp|PaintBrush file(*.tga)|*.tga||");
	if( dlg.DoModal() != IDOK ) 
		return;
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_TEXTURE );
	pEdit->SetWindowText( dlg.GetFileName() );

	// 사이즈를 얻기 위해선 텍스춰를 로드해야한다.
	SAFE_DELETE( m_pTexture );
	m_pTexture = new CTexture;
	m_pTexture->LoadTexture( m_pWndDialog->m_pApp->m_pd3dDevice, dlg.GetFileName(), 0xffff00ff, TRUE ); 
}

/////////////////////////////////////////////////////////////////////////////
// CPropControlGeneral property page

IMPLEMENT_DYNCREATE(CPropControlGeneral, CPropertyPage)

CPropControlGeneral::CPropControlGeneral() : CPropertyPage(CPropControlGeneral::IDD)
, m_Edit_X(0)
, m_Edit_Y(0)
{
	//{{AFX_DATA_INIT(CPropControlGeneral)
	m_nWidth = 0;
	m_nHeight = 0;
	m_bVisible = FALSE;
	m_bTabStop = FALSE;
	m_bGroup = FALSE;
	m_bDisabled = FALSE;
	m_strToolTip = _T("");
	m_bTile = FALSE;
	//}}AFX_DATA_INIT
	m_pWndCtrl_ = NULL;
	m_pWndBase = NULL;
	m_pWndDialog = NULL;
	m_pTexture = NULL;
}

CPropControlGeneral::~CPropControlGeneral()
{
	SAFE_DELETE( m_pTexture );
}

void CPropControlGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropControlGeneral)
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	DDX_Text(pDX, IDC_X, m_Edit_X);
	DDX_Text(pDX, IDC_Y, m_Edit_Y);
	DDX_Check(pDX, IDC_CHECK_VISIBLE, m_bVisible);
	DDX_Check(pDX, IDC_CHECK_TAB_STOP, m_bTabStop);
	DDX_Check(pDX, IDC_CHECK_GROUP, m_bGroup);
	DDX_Check(pDX, IDC_CHECK_DISABLED, m_bDisabled);
	DDX_Text(pDX, IDC_TOOLTIP, m_strToolTip);
	DDV_MaxChars(pDX, m_strToolTip, 128);
	DDX_Check(pDX, IDC_TILE, m_bTile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropControlGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPropControlGeneral)
	ON_BN_CLICKED(IDC_TEXTURE_BROWSER, OnTextureBrowser)
	ON_BN_CLICKED(IDC_FIT_TEXUTRE_SIZE, OnFitTexutreSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropControlGeneral message handlers

BOOL CPropControlGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CComboBox* pWIDC    = (CComboBox*)GetDlgItem( IDC_WIDC    );
	CEdit*     pCaption = (CEdit    *)GetDlgItem( IDC_CAPTION );

	//m_strOldDefine = m_pWndCtrl_->szDefine;
	m_strOldDefine = m_pWndCtrl_->strDefine;
	pWIDC->SetWindowText( m_strOldDefine );
	CString strX,strY;


	m_bDisabled = m_pWndCtrl_->m_bDisabled ;
	m_bTabStop  = m_pWndCtrl_->m_bTabStop  ;
	m_bVisible  = m_pWndCtrl_->m_bVisible  ;
	m_bGroup    = m_pWndCtrl_->m_bGroup    ;
	m_strToolTip = m_pWndCtrl_->strToolTip;
	m_bTile = m_pWndCtrl_->m_bTile;

	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_TEXTURE );
	pWIDC->SetWindowText( m_strOldDefine );
	m_strOldTexture = m_pWndCtrl_->strTexture;
	pEdit->SetWindowText( m_pWndCtrl_->strTexture );

	m_nWidth = m_pWndCtrl_->rect.Width();
	m_nHeight = m_pWndCtrl_->rect.Height();
	
	m_pTexture = new CTexture;
	m_pTexture->LoadTexture( m_pWndDialog->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, CWndBase::m_resMng.GetSelectLang(),m_pWndCtrl_->strTexture ), 0xffff00ff, TRUE ); 

	CStringArray strArray;
	CScript::GetFindIdToArray( _T( "WIDC_" ), &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		pWIDC->AddString( strArray.GetAt( i ) );
	}
	if( m_pWndBase->GetWndType() == WTYPE_TEXT )
		pCaption->SetWindowText( ((CWndText*)m_pWndBase)->m_string );
	else
		pCaption->SetWindowText( m_pWndBase->GetTitle() );

	m_Edit_X  = m_pWndCtrl_->rect.left;
	m_Edit_Y  = m_pWndCtrl_->rect.top;

	CString string;
	pCaption->GetWindowText(string);
	if(!strncmp(string,"IDS_",3))
	{	
		//testcode
		//IDS_이Text가 추가되면 안되니까 강제로 텍스트를 지워버린다 IDS_<- 이것만 
        FILEOUT( "Daisy_Log.txt","==================================================");
		FILEOUT( "Daisy_Log.txt","CPropControlGeneral::OnInitDialog():[%s]_Title",string );
		pCaption->SetWindowText("");
	}
	if(!strncmp(m_strToolTip,"IDS_",3))
	{
		FILEOUT( "Daisy_Log.txt","==================================================");
		FILEOUT( "Daisy_Log.txt","CPropControlGeneral::OnInitDialog():[%s]_ToolTip",m_strToolTip );
		m_strToolTip.Format("");
	}
	UpdateData( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPropControlGeneral::OnOK( )
{
	UpdateData( TRUE );

	// TODO: Add your message handler code here
	CComboBox* pWIDC    = (CComboBox*)GetDlgItem( IDC_WIDC    );
	CEdit*     pCaption = (CEdit    *)GetDlgItem( IDC_CAPTION );
	CEdit*     pTexture = (CEdit*)GetDlgItem( IDC_TEXTURE );

	CString string;
	pWIDC->GetWindowText( string );
	int nBegin = string.Find( _T( "WIDC_" )  );
	if( nBegin != 0 ) 
	{
		AfxMessageBox( _T( "Control의 ID가 WIDC_로 시작하지 않습니다." ) );
		return;
	}
	//string = CScript::GetFindIdStr( _T( "WIDC_" ), m_pWndBase->GetWndId() );
	//pWIDC->SetWindowText( string );
/*
	CStringArray strArray;
	CScript::GetFindIdToArray( _T( "APP_" ), &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		pWIDC->AddString( strArray.GetAt( i ) );
	}
	pCaption->SetWindowText( m_pWndDialog->GetTitle() );
	*/
	LPWNDCTRL lpWndCtrl = m_pWndCtrl_;//m_pWndBase->m_resMng.GetAtControl( m_pWndApplet_->dwWndId, m_pWndBase->GetWndId() );
	CString strTitle;

	pCaption->GetWindowText( strTitle );

	if( CWndBase::m_resMng.ChangeControlDefine( m_pWndApplet_->dwWndId, m_strOldDefine, string ) )
	{
#ifdef __DAISY_UPDATE_3
		if(CWndBase::m_resMng.m_nChangeItem == -1 && lpWndCtrl)
		{
			//lpWndCtrl->dwWndHistoryType = CWndBase::m_resMng.CTRL_CHANGE;
			memcpy(&CWndBase::m_resMng.m_OldWndCtrl, lpWndCtrl, sizeof(WNDCTRL));
			CWndBase::m_resMng.m_nChangeItem = CWndBase::m_resMng.CTRL_CHANGE;
		}
#endif //__DAISY_UPDATE_3

		if( m_pWndBase->GetWndType() == WTYPE_TEXT )
			((CWndText*)m_pWndBase)->m_string = strTitle; 
		else
			m_pWndBase->SetTitle( strTitle );
		//_tcscpy( lpWndCtrl->szTitle, strTitle );

		// 수정되었으면 외국어 데이타를 날린다.
#ifndef __USE_STR_TBL0407
		if( lpWndCtrl->strTitle != strTitle )
		{
			for( int i = 0; i < LANG_MAX; i++ )
				lpWndCtrl->awszTitle[ i ][ 0 ] = 0;
		}
		if( lpWndCtrl->strToolTip != m_strToolTip )
		{
			for( int i = 0; i < LANG_MAX; i++ )
				lpWndCtrl->awszToolTip[ i ][ 0 ] = 0;
		}
#endif	// __USE_STR_TBL0407
		// flyingjin : Log
		if(!strncmp(strTitle,"IDS_",3))
		{
			//testcode
			//IDS_이Text가 추가되면 안되니까 강제로 텍스트를 지워버린다 IDS_<- 이것만 
			FILEOUT( "Daisy_Log.txt","==================================================");
			FILEOUT( "Daisy_Log.txt","CPropControlGeneral::OnOK:[%s]_Title",strTitle );
			pCaption->SetWindowText("");
		}
		if(!strncmp(m_strToolTip,"IDS_",3))
		{
			FILEOUT( "Daisy_Log.txt","==================================================");
			FILEOUT( "Daisy_Log.txt","CPropControlGeneral::OnOK:[%s]_ToolTip",m_strToolTip );
			m_strToolTip.Format("");
		}
		lpWndCtrl->strTitle = strTitle;
		lpWndCtrl->strToolTip = m_strToolTip;

		pTexture->GetWindowText( lpWndCtrl->strTexture );
		if( m_strOldTexture != lpWndCtrl->strTexture )
		{
// 			if(!m_bTile)
// 			{
// 				AfxMessageBox( _T( "Tile is FALSE!" ) );
// 				m_pWndBase->SetTexture( m_pWndBase->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, lpWndCtrl->strTexture ), TRUE );
// 			}

			if( lpWndCtrl->dwWndType == WTYPE_BUTTON ) // || m_bTile == FALSE )
			{
// 				if(!m_bTile)
// 				{
// 					FILEOUT( "Daisy_Log.txt","Tile = FLASE , SetButtom" );
// 				}
				m_pWndBase->SetTexture( m_pWndBase->m_pApp->m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), lpWndCtrl->strTexture ), TRUE );
			}
			else
			{
				if( lpWndCtrl->dwWndType == WTYPE_STATIC )
				{
					((CWndStatic*)m_pWndBase)->m_strTexture = lpWndCtrl->strTexture;
				}
				//m_pWndDialog->SetWndRect( m_pWndDialog->GetWindowRect( TRUE ) );
				//m_nWidth = m_pWndDialog->GetWndRect().Width();
				//m_nHeight = m_pWndDialog->GetWndRect().Height();
				// SetWndRect에 TRUE를 불러도 동일 사이즈로 부르기 때문에 OnSize를 호출하지 못한다.
				// 따라서 직접 불러서 AdjustWndBase를 호출하게 해야한다.
				
			}
		}

		lpWndCtrl->rect.left = m_Edit_X;
		lpWndCtrl->rect.top = m_Edit_Y; 
		lpWndCtrl->rect.right = lpWndCtrl->rect.left + m_nWidth;
		lpWndCtrl->rect.bottom = lpWndCtrl->rect.top + m_nHeight;
		lpWndCtrl->m_bDisabled = m_bDisabled;
		lpWndCtrl->m_bTabStop = m_bTabStop;
		lpWndCtrl->m_bVisible = m_bVisible;
		lpWndCtrl->m_bGroup = m_bGroup;
		lpWndCtrl->m_bTile = m_bTile; 
		m_pWndBase->m_bTile = m_bTile ? true : false;
		//m_pWndBase->SetFontColor(lpWndCtrl->m_nFontColorR,
		//						 lpWndCtrl->m_nFontColorG,
		//						 lpWndCtrl->m_nFontColorB);
	
 		CRect rect = m_pWndBase->GetWindowRect( TRUE );
		rect = lpWndCtrl->rect; 
		//rect.bottom = rect.top + m_nHeight;
		m_pWndBase->SetWndRect( rect );
		m_pWndBase->SetWndId( lpWndCtrl->dwWndId );
		//lpWndCtrl->strToolTip = m_strToolTip;
		m_pWndBase->AdjustWndBase();//>OnSize( 0, m_nWidth, m_nHeight );
		m_pWndDialog->AdjustWndBase();
		m_pDaisyView->InvalidateRect( FALSE );	

#ifdef __DAISY_UPDATE_3
		if(CWndBase::m_resMng.m_nChangeItem == CWndBase::m_resMng.CTRL_CHANGE && lpWndCtrl)
		{
			memcpy(&CWndBase::m_resMng.m_OldWndCtrl,lpWndCtrl,sizeof(WNDCTRL));
			LPWNDCTRL lpWndCtrl_New = new WNDCTRL;
			LPWNDCTRL lpWndCtrl_Old = lpWndCtrl;
			*lpWndCtrl_New = *lpWndCtrl_Old;

			lpWndCtrl_New->dwWndHistoryType = CWndBase::m_resMng.CTRL_CHANGE;

			CWndBase::m_resMng.SaveHistory(lpWndCtrl_New);
			CWndBase::m_resMng.m_nChangeItem = -1;
		}
#endif //__DAISY_UPDATE_3
	}
	else
		AfxMessageBox( _T( "동일한 Id가 정의되어 있습니다." ) );

	CWndBase::m_resMng.SetModifiedScript();

/*
	CResourceView* pResourceView = (CResourceView*)g_pMainFrame->m_wndSTCBar.GetView( 0 );
	
	pWIDC->GetWindowText( strTitle );
	LPSTR lpDefData = CScript::GetDefineStr( strTitle );
	if( lpDefData )
		m_pWndBase->m_nIdWnd = _ttoi( lpDefData );
	else
	{
		TCHAR* szDefData = new TCHAR[ 32 ];
		_itot( g_pMainFrame->m_nIdApplet, szDefData, 10 );
		CScript::AddDefine( strTitle, szDefData );
		m_pWndDialog->m_nIdWnd = _ttoi( szDefData );
		g_pMainFrame->m_nIdApplet++;
	}
	if( m_strOldDefine != strTitle )
		pResourceView->ChangeApplet( m_strOldDefine, strTitle );
*/
}



void CPropControlGeneral::OnTextureBrowser() 
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Windows or os/2 file(*.png)|*.png|Windows or os/2 file(*.bmp)|*.bmp|PaintBrush file(*.tga)|*.tga||");
	if( dlg.DoModal() != IDOK ) 
		return;
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_TEXTURE );
	pEdit->SetWindowText( dlg.GetFileName() );
	//m_pWndCtrl_->strTexture = dlg.GetFileName();

	// 사이즈를 얻기 위해선 텍스춰를 로드해야한다.
	SAFE_DELETE( m_pTexture );
	m_pTexture = new CTexture;
	m_pTexture->LoadTexture( m_pWndDialog->m_pApp->m_pd3dDevice, dlg.GetFileName(), 0xffff00ff, TRUE ); 
}

void CPropControlGeneral::OnFitTexutreSize() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	if( m_pTexture )
	{
		if( m_pWndCtrl_->dwWndType == WTYPE_BUTTON )
		{
			if( m_pWndCtrl_->dwWndStyle & WBS_RADIO || m_pWndCtrl_->dwWndStyle & WBS_CHECK )
				m_nWidth = m_pTexture->m_size.cx / 6;
			else
				m_nWidth = m_pTexture->m_size.cx / 4;
		}
		else
			m_nWidth = m_pTexture->m_size.cx;
		m_nHeight = m_pTexture->m_size.cy;
	}
	UpdateData( FALSE );
	
}


/////////////////////////////////////////////////////////////////////////////
// CPropControlBaseStyle property page
//

#define	MIN_FONTCOLOR_VALUE		0
#define	MAX_FONTCOLOR_VALUE		255

IMPLEMENT_DYNCREATE(CPropControlBaseStyle, CPropertyPage)

BEGIN_MESSAGE_MAP(CPropControlBaseStyle, CPropertyPage)
	ON_BN_CLICKED(IDC_FONT_COLOR, OnBnClickedFontColor)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CPropControlBaseStyle::CPropControlBaseStyle() 
: CPropertyPage(CPropControlBaseStyle::IDD)
,	m_pDaisyView( NULL )
,	m_pWndApplet_( NULL )
,	m_pWndCtrl_( NULL )
,	m_pWndBase( NULL )
,	m_pWndDialog( NULL )
,	m_bNoDrawFrame( FALSE )
,	m_bVScrollBar( FALSE )

#ifdef __DAISY_UPDATE_2
,	m_bAlightRight( FALSE )
#endif 

#ifdef __DAISY_UPDATE_4
,	m_nStaticHAlign( ALIGN_H_LEFT )
,	m_nStaticVAlign( ALIGN_V_TOP )
#endif 

, m_strFontColorR(_T("0"))
, m_strFontColorG(_T("0"))
, m_strFontColorB(_T("0"))
{
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CPropControlBaseStyle::~CPropControlBaseStyle()
{
}


//---------------------------------------------------------------------------------------------
// Do Data Exchange ( 데이터를 교환 할 때 )
// param	: pDX
// return	: void 
//---------------------------------------------------------------------------------------------
void CPropControlBaseStyle::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX,		IDC_NO_DRAW_FRAME,		m_bNoDrawFrame);
	DDX_Check(pDX,		IDC_VSCROLLBAR,			m_bVScrollBar);
	DDX_Text(pDX,		IDC_FONT_R, 			m_strFontColorR);
	DDX_Text(pDX,		IDC_FONT_G, 			m_strFontColorG);
	DDX_Text(pDX,		IDC_FONT_B, 			m_strFontColorB);

#ifdef __DAISY_UPDATE_2
	DDX_Check(pDX,		IDC_ALIGNRIGHT,			m_bAlightRight);
	DDX_Control(pDX,	IDC_ALIGNRIGHT,			m_CheckAlign);
#endif

#ifdef __DAISY_UPDATE_4
	DDX_Radio(pDX,		IDC_RADIO_H_LEFT,		m_nStaticHAlign);
	DDX_Control(pDX,	IDC_RADIO_H_LEFT,		m_CheckHAlign);
	DDX_Control(pDX,	IDC_RADIO_H_RIGHT,		m_CheckHAlign);
	DDX_Control(pDX,	IDC_RADIO_H_CENTER,		m_CheckHAlign);
	DDX_Radio(pDX,		IDC_RADIO_V_TOP,		m_nStaticVAlign);
	DDX_Control(pDX,	IDC_RADIO_V_TOP,		m_CheckVAlign);
	DDX_Control(pDX,	IDC_RADIO_V_BOTTOM,		m_CheckVAlign);
	DDX_Control(pDX,	IDC_RADIO_V_CENTER,		m_CheckVAlign);
#endif 
	DDX_Control(pDX, IDC_FONT_R, m_EditFontColorR);
	DDX_Control(pDX, IDC_FONT_G, m_EditFontColorG);
	DDX_Control(pDX, IDC_FONT_B, m_EditFontColorB);
	DDX_Control(pDX, IDC_FONT_COLOR, m_ButtonChangeFontColor);
}


//---------------------------------------------------------------------------------------------
// On Initialize Dialog ( 다이얼로그 초기화 할 때 )
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CPropControlBaseStyle::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	FLERROR_ASSERT_LOG_RETURN( m_pWndCtrl_, FALSE, _T("[컨트롤 베이스 스타일 설정] 컨트롤 정보 포인터 없음.") );			
	FLERROR_ASSERT_LOG_RETURN( m_pWndBase, FALSE, _T("[컨트롤 베이스 스타일 설정] 컨트롤 윈도우 포인터 없음.") );			

	m_bVScrollBar =		( m_pWndCtrl_->dwWndStyle & WBS_VSCROLL ) ? TRUE : FALSE;
	m_bNoDrawFrame =	( m_pWndCtrl_->dwWndStyle & WBS_NODRAWFRAME ) ? TRUE : FALSE;

	m_strFontColorR.Format( _T("%d"), m_pWndBase->GetFontColorR() );
	m_strFontColorG.Format( _T("%d"), m_pWndBase->GetFontColorG() );
	m_strFontColorB.Format( _T("%d"), m_pWndBase->GetFontColorB() );


	if( m_pWndBase->m_byWndType == WTYPE_CUSTOM		||
		m_pWndBase->m_byWndType == WTYPE_TABCTRL	||
		m_pWndBase->m_byWndType == WTYPE_TREECTRL	)
	{
		m_EditFontColorR.EnableWindow( FALSE );
		m_EditFontColorG.EnableWindow( FALSE );
		m_EditFontColorB.EnableWindow( FALSE );
	
		m_ButtonChangeFontColor.EnableWindow( FALSE );
	}

	

#ifdef __DAISY_UPDATE_2
	if(m_pWndBase->m_byWndType == WTYPE_STATIC)
	{
		m_bAlightRight = ( m_pWndCtrl_->dwWndStyle & WSS_ALIGNRIGHT ) ? TRUE : FALSE;
		m_CheckAlign.ShowWindow(SW_SHOW);
	}
	else
		m_CheckAlign.ShowWindow(SW_HIDE);
#endif 


#ifdef __DAISY_UPDATE_4
	if(m_pWndBase->m_byWndType == WTYPE_STATIC)
	{
		m_nStaticHAlign = ALIGN_H_LEFT;
		m_CheckHAlign.ShowWindow(SW_SHOW);
		if(m_pWndCtrl_->dwWndStyle & WSS_ALIGNHRIGHT)
		{
			m_nStaticHAlign = ALIGN_H_RIGHT;
			m_CheckHAlign.ShowWindow(SW_SHOW);
		}
		else if(m_pWndCtrl_->dwWndStyle & WSS_ALIGNHCENTER)
		{
			m_nStaticHAlign = ALIGN_H_CENTER;
			m_CheckHAlign.ShowWindow(SW_SHOW);
		}

		m_nStaticVAlign = ALIGN_V_TOP;
		m_CheckVAlign.ShowWindow(SW_SHOW);
		if(m_pWndCtrl_->dwWndStyle & WSS_ALIGNVBOTTOM)
		{
			m_nStaticVAlign = ALIGN_V_BOTTOM;
			m_CheckVAlign.ShowWindow(SW_SHOW);
		}
		else if(m_pWndCtrl_->dwWndStyle & WSS_ALIGNVCENTER)
		{
			m_nStaticVAlign = ALIGN_V_CENTER;
			m_CheckVAlign.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_CheckHAlign.EnableWindow( FALSE );
		m_CheckVAlign.EnableWindow( FALSE );
	}
#endif 
	
	UpdateData( FALSE );
	
	return TRUE;  
	              
}


//---------------------------------------------------------------------------------------------
// Limit Font Color Value ( 폰트 색상 변경 버튼 누를 때 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CPropControlBaseStyle::LimitFontColorValue(  int& nColorValue, CString& strFontColorDDX  )
{
	if( nColorValue < MIN_FONTCOLOR_VALUE )
	{
		nColorValue = MIN_FONTCOLOR_VALUE;
		AfxMessageBox( _T(" [데이지 경고] 폰트 색상 값은 0보다 작을 수 없습니다. 0으로 변경합니다 :)"), MB_OK );
	}

	if( nColorValue > MAX_FONTCOLOR_VALUE )
	{
		nColorValue = MAX_FONTCOLOR_VALUE;
		AfxMessageBox( _T(" [데이지 경고] 폰트 색상 값은 255보다 클 수 없습니다. 255로 변경합니다 :)"), MB_OK );
	}

	strFontColorDDX.Format( _T("%d"), nColorValue );
	UpdateData( FALSE );
}


//---------------------------------------------------------------------------------------------
// On Bn Clicked Font Color ( 폰트 색상 변경 버튼 누를 때 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CPropControlBaseStyle::OnBnClickedFontColor()
{
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pDaisyView,	_T("[컨트롤 베이스 스타일 설정] 뷰 없음.") );			
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndApplet_,  _T("[컨트롤 베이스 스타일 설정] 활성화 된 에플릿 없음.") );			
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndBase,		_T("[컨트롤 베이스 스타일 설정] 컨트롤 없음.") );			
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndCtrl_,	_T("[컨트롤 베이스 스타일 설정] 컨트롤 정보 없음.") );			

	CEdit* pEditR = (CEdit*)GetDlgItem( IDC_FONT_R );
	FLERROR_ASSERT_LOG_RETURN_VOID( pEditR,  _T("[컨트롤 베이스 스타일 설정] R 에디터 얻기 실패.") );			

	CEdit* pEditG = (CEdit*)GetDlgItem( IDC_FONT_G );
	FLERROR_ASSERT_LOG_RETURN_VOID( pEditG,  _T("[컨트롤 베이스 스타일 설정] G 에디터 얻기 실패.") );			

	CEdit* pEditB = (CEdit*)GetDlgItem( IDC_FONT_B );
	FLERROR_ASSERT_LOG_RETURN_VOID( pEditB,  _T("[컨트롤 베이스 스타일 설정] B 에디터 얻기 실패.") );			

	CString	strR( _T("") );
	CString	strG( _T("") );
	CString	strB( _T("") );

	pEditR->GetWindowText( strR );
	pEditG->GetWindowText( strG );
	pEditB->GetWindowText( strB );
	
	int nR( atoi( strR ) );
	int nG( atoi( strG ) );
	int nB( atoi( strB ) );

	LimitFontColorValue( nR, m_strFontColorR );
	LimitFontColorValue( nG, m_strFontColorG );
	LimitFontColorValue( nB, m_strFontColorB );

	m_pWndCtrl_->m_nFontColorR = nR;
	m_pWndCtrl_->m_nFontColorG = nG;
	m_pWndCtrl_->m_nFontColorB = nB;

	m_pWndBase->SetFontColor( nR, nG, nB );
	FLINFO_LOG( PROGRAM_NAME, _T("[컨트롤 베이스 스타일 설정] 폰트 색상 변경 : R - [%u], G - [%u], B - [%u]"), nR, nG, nB );

	Invalidate( TRUE );
}


//---------------------------------------------------------------------------------------------
// On OK ( OK 버튼 누를 때 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CPropControlBaseStyle::OnOK( )
{
	UpdateData( TRUE );

	FLERROR_ASSERT_LOG_RETURN_VOID( m_pDaisyView,	_T("[컨트롤 베이스 스타일 설정] 뷰 없음.") );			
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndApplet_,  _T("[컨트롤 베이스 스타일 설정] 활성화 된 에플릿 없음.") );			
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndBase,		_T("[컨트롤 베이스 스타일 설정] 컨트롤 없음.") );			
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndCtrl_,	_T("[컨트롤 베이스 스타일 설정] 컨트롤 정보 없음.") );			

	if( m_bNoDrawFrame ) 
	{
		m_pWndCtrl_->dwWndStyle |= WBS_NODRAWFRAME;
		m_pWndBase->AddWndStyle( WBS_NODRAWFRAME );
	}
	else 
	{
		m_pWndCtrl_->dwWndStyle &= ~WBS_NODRAWFRAME;
		m_pWndBase->DelWndStyle( WBS_NODRAWFRAME );
	}
	if( m_bVScrollBar ) 
	{
		m_pWndCtrl_->dwWndStyle |= WBS_VSCROLL;
		m_pWndBase->AddWndStyle( WBS_VSCROLL );
	}
	else 
	{
		m_pWndCtrl_->dwWndStyle &= ~WBS_VSCROLL;
		m_pWndBase->DelWndStyle( WBS_VSCROLL );
	}


#ifdef __DAISY_UPDATE_2
	if( m_pWndBase->m_byWndType == WTYPE_STATIC )
	{
		if( m_bAlightRight )
		{
			m_pWndCtrl_->dwWndStyle |= WSS_ALIGNRIGHT;
			m_pWndBase->AddWndStyle( WSS_ALIGNRIGHT );
		}
		else
		{
			m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNRIGHT;
			m_pWndBase->DelWndStyle( WSS_ALIGNRIGHT );
		}
	}
#endif 


#ifdef __DAISY_UPDATE_4
	if( m_pWndBase->m_byWndType == WTYPE_STATIC )
	{
		switch(m_nStaticHAlign)
		{
		case ALIGN_H_RIGHT:
			{
				m_pWndCtrl_->dwWndStyle |= WSS_ALIGNHRIGHT;
				m_pWndBase->AddWndStyle( WSS_ALIGNHRIGHT );
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNHCENTER;
				m_pWndBase->DelWndStyle( WSS_ALIGNHCENTER );
				break;
			}
		case ALIGN_H_CENTER:
			{
				m_pWndCtrl_->dwWndStyle |= WSS_ALIGNHCENTER;
				m_pWndBase->AddWndStyle( WSS_ALIGNHCENTER );
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNHRIGHT;
				m_pWndBase->DelWndStyle( WSS_ALIGNHRIGHT );
				break;
			}
		default:
			{
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNHRIGHT;
				m_pWndBase->DelWndStyle( WSS_ALIGNHRIGHT );
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNHCENTER;
				m_pWndBase->DelWndStyle( WSS_ALIGNHCENTER );
			}
		}

		switch(m_nStaticVAlign)
		{
		case ALIGN_V_BOTTOM:
			{
				m_pWndCtrl_->dwWndStyle |= WSS_ALIGNVBOTTOM;
				m_pWndBase->AddWndStyle( WSS_ALIGNVBOTTOM );
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNVCENTER;
				m_pWndBase->DelWndStyle( WSS_ALIGNVCENTER );
				break;
			}
		case ALIGN_V_CENTER:
			{
				m_pWndCtrl_->dwWndStyle |= WSS_ALIGNVCENTER;
				m_pWndBase->AddWndStyle( WSS_ALIGNVCENTER );
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNVBOTTOM;
				m_pWndBase->DelWndStyle( WSS_ALIGNVBOTTOM );
				break;
			}
		default:
			{
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNVBOTTOM;
				m_pWndBase->DelWndStyle( WSS_ALIGNVBOTTOM );
				m_pWndCtrl_->dwWndStyle &= ~WSS_ALIGNVCENTER;
				m_pWndBase->DelWndStyle( WSS_ALIGNVCENTER );
			}
		}
	}
#endif 

	CRect rect = m_pWndCtrl_->rect;
	m_pWndBase->SetWndRect( rect );

	m_pWndDialog->AdjustWndBase();
	m_pWndBase->AdjustWndBase();

	m_pDaisyView->Invalidate( FALSE );

	CWndBase::m_resMng.SetModifiedScript();

}


//---------------------------------------------------------------------------------------------
// On Control Color ( 컨트롤 색상이 바뀔 때, 색상 미리보기를 위해 추가 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HBRUSH CPropControlBaseStyle::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

     if(pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_FONTCOLOR)
    {
		pDC->SetBkMode( OPAQUE );
		COLORREF Color = RGB( m_pWndBase->GetFontColorR(), m_pWndBase->GetFontColorG(), m_pWndBase->GetFontColorB());
		pDC->SetBkColor( Color );
        return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}


/////////////////////////////////////////////////////////////////////////////
// CPropAppletBaseStyle property page

IMPLEMENT_DYNCREATE(CPropAppletBaseStyle, CPropertyPage)

CPropAppletBaseStyle::CPropAppletBaseStyle() : CPropertyPage(CPropAppletBaseStyle::IDD)
{
	//{{AFX_DATA_INIT(CPropAppletBaseStyle)
	m_bNoDrawFrame = FALSE;
	m_bThickFrame = FALSE;
	m_bCaption = FALSE;
	m_bNoFrame = FALSE;
	//}}AFX_DATA_INIT
}

CPropAppletBaseStyle::~CPropAppletBaseStyle()
{
}

void CPropAppletBaseStyle::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropAppletBaseStyle)
	DDX_Check(pDX, IDC_THICKFRAME, m_bThickFrame);
	DDX_Check(pDX, IDC_CAPTION, m_bCaption);
	DDX_Check(pDX, IDC_NO_FRAME, m_bNoFrame);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_BTCLOSESTATE, m_bCloseBT);
}


BEGIN_MESSAGE_MAP(CPropAppletBaseStyle, CPropertyPage)
	//{{AFX_MSG_MAP(CPropAppletBaseStyle)
	ON_BN_CLICKED(IDC_CAPTION, OnCaption)
	ON_BN_CLICKED(IDC_NO_FRAME, OnNoFrame)
	ON_BN_CLICKED(IDC_NO_DRAW_FRAME, OnNoDrawFrame)
	ON_BN_CLICKED(IDC_THICKFRAME, OnThickframe)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTCLOSESTATE, OnCloseButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropAppletBaseStyle message handlers
BOOL CPropAppletBaseStyle::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bNoDrawFrame = ( m_pWndApplet_->dwWndStyle & WBS_NODRAWFRAME ) ? TRUE : FALSE;
	m_bThickFrame = ( m_pWndApplet_->dwWndStyle & WBS_THICKFRAME ) ? TRUE : FALSE;
	m_bCaption = ( m_pWndApplet_->dwWndStyle & WBS_CAPTION ) ? TRUE : FALSE;
	m_bNoFrame = ( m_pWndApplet_->dwWndStyle & WBS_NOFRAME ) ? TRUE : FALSE;
	m_bCloseBT = m_pWndApplet_->m_bNoCloseButton ? TRUE :FALSE;
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CPropAppletBaseStyle::OnOK( )
{
	UpdateData( TRUE );
	
	// TODO: Add your message handler code here
/*
	if( m_bNoDrawFrame ) m_pWndApplet_->dwWndStyle |= WBS_NODRAWFRAME;
	else m_pWndApplet_->dwWndStyle &= ~WBS_NODRAWFRAME;
	if( m_bThickFrame ) m_pWndApplet_->dwWndStyle |= WBS_THICKFRAME;
	else m_pWndApplet_->dwWndStyle &= ~WBS_THICKFRAME;
*/
	CWndBase::m_resMng.SetModifiedScript();
	//_pWndBase->m_dwStyle = m_pWndCtrl_->dwWndStyle;
}

void CPropAppletBaseStyle::OnCaption() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );

	if( m_bCaption ) 
	{	
		m_pWndApplet_->dwWndStyle |= WBS_CAPTION;
		m_pWndDialog->AddWndStyle( WBS_CAPTION );
	}
	else 
	{
		m_pWndApplet_->dwWndStyle &= ~WBS_CAPTION;
		m_pWndDialog->DelWndStyle( WBS_CAPTION );
	}
	g_pMainFrame->m_pDaisyView->Invalidate();

}

void CPropAppletBaseStyle::OnNoFrame() 
{
	UpdateData( TRUE );
	
	if( m_bNoFrame ) 
	{
		m_pWndApplet_->dwWndStyle |= WBS_NOFRAME;
		m_pWndDialog->AddWndStyle( WBS_NOFRAME );
	}
	else 
	{
		m_pWndApplet_->dwWndStyle &= ~WBS_NOFRAME;
		m_pWndDialog->DelWndStyle( WBS_NOFRAME );
	}
	g_pMainFrame->m_pDaisyView->Invalidate();
}

void CPropAppletBaseStyle::OnNoDrawFrame() 
{
	UpdateData( TRUE );
	
	if( m_bNoDrawFrame ) 
	{
		m_pWndApplet_->dwWndStyle |= WBS_NODRAWFRAME;
		m_pWndDialog->AddWndStyle( WBS_NODRAWFRAME );
	}
	else 
	{
		m_pWndApplet_->dwWndStyle &= ~WBS_NODRAWFRAME;
		m_pWndDialog->DelWndStyle( WBS_NODRAWFRAME );
	}
	g_pMainFrame->m_pDaisyView->Invalidate();
}

void CPropAppletBaseStyle::OnThickframe() 
{
	UpdateData( TRUE );
	/*
	if( m_bCaption ) 
	{
		m_pWndApplet_->dwWndStyle |= WBS_THINKFRAME;
		m_pWndDialog->AddWndStyle( WBS_THINKFRAME );
	}
	else 
	{
		m_pWndApplet_->dwWndStyle &= ~WBS_THINKFRAME;
		m_pWndDialog->DelWndStyle( WBS_THINKFRAME );
	}*/
	g_pMainFrame->m_pDaisyView->Invalidate();
}

void CPropAppletBaseStyle::OnCloseButton()
{
	UpdateData( TRUE );

	if( m_bCloseBT ) 
	{
		m_pWndApplet_->m_bNoCloseButton = TRUE;
	}
	else
	{
		m_pWndApplet_->m_bNoCloseButton = FALSE;
	}
	g_pMainFrame->m_pDaisyView->Invalidate();
}



void CPropAppletGeneral::OnBnClickedButtonDefaultImg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Windows or os/2 file(*.png)|*.png|Windows or os/2 file(*.bmp)|*.bmp|PaintBrush file(*.tga)|*.tga||");
	if( dlg.DoModal() != IDOK ) 
		return;
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_DEFAULT_IMG );
	pEdit->SetWindowText( dlg.GetFileName() );
}

