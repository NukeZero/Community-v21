// DlgNewWorld.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgNewWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewWorld dialog


CDlgNewWorld::CDlgNewWorld(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewWorld::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewWorld)
	m_strTexture = _T("");
	m_bIndoor = FALSE;
	m_nUnitHeight = 5;
	m_nUnitWidth = 5;
	m_strWorldHeight = _T("");
	m_strWorldWidth = _T("");
	m_fLandHeight = 100.0f;
	m_strHeightBitmap = _T("");
	m_fMutipleHeight = 0.0f;
	m_iMPU = g_MPU;
	//}}AFX_DATA_INIT
	m_nSelectTexture = 0;
}


void CDlgNewWorld::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewWorld)
	DDX_CBString(pDX, IDC_COMBO_TEXTURE, m_strTexture);
	DDX_Check(pDX, IDC_INDOOR, m_bIndoor);
	DDX_Text(pDX, IDC_UNIT_HEIGHT, m_nUnitHeight);
	DDX_Text(pDX, IDC_UNIT_WIDTH, m_nUnitWidth);
	DDX_Text(pDX, IDC_WORLD_HEIGHT, m_strWorldHeight);
	DDX_Text(pDX, IDC_WORLD_WIDTH, m_strWorldWidth);
	DDX_Text(pDX, IDC_BASE_HEIGHT, m_fLandHeight);
	DDX_Text(pDX, IDC_HEIGHT_BITMAP, m_strHeightBitmap);
	DDX_Text(pDX, IDC_MULTIPLE_HEIGHT, m_fMutipleHeight);
	DDX_Text(pDX, IDC_EDIT_NEWWORLD_MPU, m_iMPU);
	DDV_MinMaxInt(pDX, m_iMPU, 1, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewWorld, CDialog)
	//{{AFX_MSG_MAP(CDlgNewWorld)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE, OnSelchangeComboTexture)
	ON_BN_CLICKED(IDC_BROWSER, OnBrowser)
	ON_EN_CHANGE(IDC_MULTIPLE_HEIGHT, OnChangeMultipleHeight)
	ON_EN_CHANGE(IDC_HEIGHT_BITMAP, OnChangeHeightBitmap)
	ON_BN_CLICKED(IDC_INDOOR, OnIndoor)
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

afx_msg void CDlgNewWorld::OnActivate(UINT state, CWnd* other, BOOL bMinimized)
{
	if (state == WA_ACTIVE || state == WA_CLICKACTIVE)
	{
		ShowWindow(SW_SHOWNORMAL);
		UpdateWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgNewWorld message handlers

void CDlgNewWorld::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();


}

BOOL CDlgNewWorld::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CComboBox* pComboTexture = (CComboBox*)GetDlgItem(IDC_COMBO_TEXTURE);
	for (std::map<unsigned long, TERRAIN>::iterator iter = prj.m_terrainMng.begin(); iter != prj.m_terrainMng.end(); ++iter)
	{
		if (iter->second.IsValid())
		{
			const int nIndex = pComboTexture->AddString(iter->second.m_szTextureFileName);
			pComboTexture->SetItemDataPtr(nIndex, &iter->second);
		}
	}
	pComboTexture->SetCurSel(0);
		//lpTerrain = prj.m_terrainMng.GetTerrain( 0 ); // default texture

	//pComboTexture->SetDlgItemText()
	//pComboTexture->SetCurSel( 0 );

/*
	TCHAR	szTitle[3][10] = {_T("File Name"), _T("Id"), _T("최대")};
	CListCtrl* pListTexture = (CListCtrl*)GetDlgItem(IDC_LIST_TEXTURE);
	
	LV_COLUMN  pColumn;
	LV_ITEM	   lvitem;
	TCHAR      strNum[10];
	//
	// Report Column을 입력 
	//
	for(int i = 0;  i < 3; i++)
	{
		pColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		pColumn.fmt = LVCFMT_LEFT;
		pColumn.iSubItem = i;
		if(i == 0)
			pColumn.cx = 150;  // SubItem is twice as large
		else
			pColumn.cx = 40;  // SubItem is twice as large
		pColumn.pszText = szTitle[i];	
		pListTexture->InsertColumn(i, &pColumn);  // assumes return value is OK.
	}
	//
	// 소스 아이템을 입력
	//
	for(i = 0; i <  256; i++) 
	{
		LPTERRAIN lpTerrain = prj.m_terrainMng.GetTerrain( i );
		if( lpTerrain->IsValid() )
		{
			lvitem.mask = LVIF_TEXT | LVIF_PARAM; // | (iSubItem == 0? LVIF_IMAGE : 0);
			lvitem.iItem = pListTexture->GetItemCount(); //(iSubItem == 0)? iItem : iActualItem;
			lvitem.iSubItem = 0;
			lvitem.lParam = (LPARAM) lpTerrain;
			lvitem.pszText = lpTerrain->m_szTextureFileName; 
			pListTexture->InsertItem(&lvitem);

			lvitem.mask = LVIF_TEXT;
			lvitem.iSubItem = 1; 
			_itoa(lpTerrain->m_dwId,strNum,10);
			lvitem.pszText = strNum;
			pListTexture->SetItem(&lvitem);

			lvitem.iSubItem = 2; 
			//_itoa(pItemProp->m_nMax,strNum,10);
			lvitem.pszText = "b";
			pListTexture->SetItem(&lvitem);
		}
	}
	*/
    m_hwndRenderWindow = GetDlgItem(IDC_TEXTUREVIEW)->GetSafeHwnd();
    CD3DApplication::m_hWnd = m_hwndRenderWindow;
    CD3DApplication::m_hWndFocus = NULL;
    CD3DApplication::Create( AfxGetInstanceHandle() );

	LPTERRAIN lpTerrain = (LPTERRAIN) pComboTexture->GetItemDataPtr( pComboTexture->GetCurSel() );
	m_nSelectTexture = lpTerrain->m_dwId;

	m_texture.DeleteDeviceObjects();//Free();
	m_texture.LoadTexture( m_pd3dDevice, _T( MakePath( DIR_WORLDTEX, lpTerrain->m_szTextureFileName ) ), 0);
	Render3DEnvironment();

	EnableButton();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
HRESULT CDlgNewWorld::InitDeviceObjects()
{
	m_2DRender.InitDeviceObjects( m_pd3dDevice );
	return S_OK;
}
HRESULT CDlgNewWorld::RestoreDeviceObjects()
{
	m_2DRender.RestoreDeviceObjects( &m_d3dsdBackBuffer );
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );
   // m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x44444444 );

    // Set up a texture
    //m_pd3dDevice->SetTexture( 0, m_pFloorTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Set the transform matrices
    D3DXMATRIX matWorld, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, 1.0f, CWorld::m_fNearPlane, CWorld::m_fFarPlane );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up the light
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 50.0f, 0.0f );
    light.Attenuation0 =  0.1f;
    light.Range        = 200.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,    TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,            TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,       TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,     FALSE );
	return S_OK;
}
HRESULT CDlgNewWorld::InvalidateDeviceObjects()
{
	m_2DRender.InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CDlgNewWorld::DeleteDeviceObjects()
{
	m_2DRender.DeleteDeviceObjects();
	return S_OK;
}
HRESULT CDlgNewWorld::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET,
                      0, 1.0f, 0L );
/*
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  m_dwFogColor );

    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(m_fFogStartValue) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(m_fFogEndValue) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGDENSITY, FtoDW(m_fFogDensity) );

    if( m_bUsingTableFog )
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   m_dwFogMode );
    }
    else
    {
        m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  m_dwFogMode );
        m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, m_bRangeBasedFog );
    }
*/
    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		if(	m_texture.m_pTexture )
			m_2DRender.RenderTexture( CPoint( 0, 0), &m_texture );
		
		/*
        // Reset the world matrix
        D3DXMATRIX matWorld;
        D3DXMatrixIdentity( &matWorld );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // Draw the terrain
        m_pd3dDevice->SetVertexShader( D3DFVF_FOGVERTEX );
        m_pd3dDevice->SetStreamSource( 0, m_pTerrainVB, sizeof(FOGVERTEX) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
                                   0, m_dwNumTerrainVertices-2 );

        // Draw the columns
        for( DWORD i=0; i<20; i++ )
        {
            FLOAT tx = (i%10)*20.0f - 100.0f;
            FLOAT ty =  0.0f;
            FLOAT tz = (i<=10) ? 40.0f : -40.0f;

            D3DXMatrixTranslation( &matWorld, tx, ty, tz );
            m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

            m_pd3dDevice->SetVertexShader( D3DFVF_FOGVERTEX );
            m_pd3dDevice->SetStreamSource( 0, m_pColumnVB, sizeof(FOGVERTEX) );
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
                                         0, m_dwNumColumnVertices-2 );
        }
*/
        // End the scene.
        m_pd3dDevice->EndScene();
    }
	return S_OK;
}

void CDlgNewWorld::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	Render3DEnvironment();
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgNewWorld::OnSelchangeComboTexture() 
{
	CComboBox* pComboTexture = (CComboBox*)GetDlgItem(IDC_COMBO_TEXTURE);
	int nCurSel =  pComboTexture->GetCurSel();
	LPTERRAIN lpTerrain = (LPTERRAIN) pComboTexture->GetItemDataPtr( nCurSel );
	m_nSelectTexture = lpTerrain->m_dwId;
	m_texture.DeleteDeviceObjects();//Free();
	m_texture.LoadTexture( m_pd3dDevice, _T( MakePath( DIR_WORLDTEX, lpTerrain->m_szTextureFileName ) ), 0);
	Render3DEnvironment();
}

void CDlgNewWorld::OnBrowser() 
{
	CFileDlgWorld dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Height Bitmap files (*.bmp)|*.bmp||");
	if( dlg.DoModal() != IDOK ) 
		return;

	m_strHeightBitmap = dlg.GetPathName();
	UpdateData( FALSE );

	EnableButton();
}

void CDlgNewWorld::OnChangeMultipleHeight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	
}

void CDlgNewWorld::OnChangeHeightBitmap() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	EnableButton();
}
void CDlgNewWorld::EnableButton() 
{
	CEdit* pBaseHeight     = (CEdit*) GetDlgItem( IDC_BASE_HEIGHT     );
	//CEdit* pHeightBitmap   = (CEdit*) GetDlgItem( IDC_HEIGHT_BITMAP   );
	CEdit* pMultipleHeight = (CEdit*) GetDlgItem( IDC_MULTIPLE_HEIGHT );
	CEdit* pUnitWidth      = (CEdit*) GetDlgItem( IDC_UNIT_WIDTH      );
	CEdit* pUnitHeight     = (CEdit*) GetDlgItem( IDC_UNIT_HEIGHT     );

	if( m_strHeightBitmap.IsEmpty() )
	{
		pBaseHeight->EnableWindow     ( TRUE );
	//	pHeightBitmap->EnableWindow   ( TRUE );
		pMultipleHeight->EnableWindow ( FALSE );
		pUnitWidth->EnableWindow      ( TRUE );
		pUnitHeight->EnableWindow     ( TRUE );
	}
	else
	{
		pBaseHeight->EnableWindow     ( FALSE  );
	//	pHeightBitmap->EnableWindow   ( TRUE  );
		pMultipleHeight->EnableWindow ( TRUE  );
		pUnitWidth->EnableWindow      ( FALSE );
		pUnitHeight->EnableWindow     ( FALSE );
	}
}

void CDlgNewWorld::OnIndoor() 
{
	// TODO: Add your control notification handler code here

	CButton* pBox = ( CButton* )GetDlgItem( IDC_INDOOR );
	CEdit* pMPU = ( CEdit* ) GetDlgItem( IDC_EDIT_NEWWORLD_MPU );

	if( pBox && pMPU )
	{
		int checked = pBox->GetCheck( );
		
		if( 0 == checked )
			pMPU->EnableWindow( FALSE );
		else pMPU->EnableWindow( TRUE );
	}

}
