// DlgObejctTextureList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Beast.h"
#include "DlgObejctTextureList.h"
#include "ObjectTextureView.h"

// DlgObejctTextureList 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgObejctTextureList, CDialog)

DlgObejctTextureList::DlgObejctTextureList(CWnd* pParent /*=NULL*/)
	: CDialog(DlgObejctTextureList::IDD, pParent)
	, m_Object_Name(_T(""))
	, m_Select_Full_Texture_Name( _T( "" ) )
	, m_Select_Texture_FileName( _T("") )
	,m_pSelectGmObject_info( NULL )
	, m_nSelectIndex( -1 )
{

}

DlgObejctTextureList::~DlgObejctTextureList()
{
}

BOOL DlgObejctTextureList::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListCtrl_Texture_List.InsertColumn( E_TYPE::NUM, "번호", LVCFMT_LEFT, 50  );
	m_ListCtrl_Texture_List.InsertColumn( E_TYPE::TEXTURE_NAME, "텍스쳐 이름", LVCFMT_LEFT, 179  );
	m_ListCtrl_Texture_List.SetExtendedStyle(  LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_TWOCLICKACTIVATE );

	return TRUE;
	
}

void DlgObejctTextureList::BindObjectTexture( CObj* pObj )
{
	m_ObjectTextureContainer.Bind_Object( pObj );
	this->UpdateList();
}

BOOL DlgObejctTextureList::PreTranslateMessage( MSG* pMsg )
{
	if( pMsg->hwnd == m_ListCtrl_Texture_List.GetSafeHwnd() )
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			if( ::GetKeyState( VK_CONTROL ) < 0 && pMsg->wParam ==  67 )
			{
				ListItemsClipboardText();
			}
		}
	}

	return CDialog::PreTranslateMessage( pMsg);
}

void DlgObejctTextureList::UpdateList()
{
	m_pSelectGmObject_info = NULL;
	m_nSelectIndex = -1;
	
	m_Select_Full_Texture_Name = "";
	m_Select_Texture_FileName = "";

	m_Button_Folder_Open.EnableWindow( FALSE );
	m_Button_Texture_Update.EnableWindow( FALSE );

	m_ListCtrl_Texture_List.DeleteAllItems();
	
	const int nSize = m_ObjectTextureContainer.TextureNameCount();

	for( int iTexName = 0; iTexName < nSize ; ++iTexName )
	{
		const char* pszName = m_ObjectTextureContainer.GetTextureName( iTexName );

		int nItemCount = m_ListCtrl_Texture_List.GetItemCount();


		CString strID = "";
		strID.Format( _T("%d"), iTexName );
		m_ListCtrl_Texture_List.InsertItem( nItemCount, strID, E_TYPE::NUM );
		m_ListCtrl_Texture_List.SetItem( nItemCount, E_TYPE::TEXTURE_NAME, LVIF_TEXT, pszName, 0, NULL, NULL, NULL);
	}

	m_Object_Name = m_ObjectTextureContainer.GetObjectName();
	m_Static_Object_Name.SetWindowText(  m_Object_Name );

	if( nSize > 0 )
	{
		m_Button_Folder_Open.EnableWindow( TRUE );
		m_Button_Texture_Update.EnableWindow( TRUE );
	}
}

void DlgObejctTextureList::UpdateTexture()
{

}

void DlgObejctTextureList::ListItemsClipboardText()
{
	int nItemCount = m_ListCtrl_Texture_List.GetSelectedCount();

	if( nItemCount < 0 )
		return ;

	POSITION pos = m_ListCtrl_Texture_List.GetFirstSelectedItemPosition();

	CString strClipBorad;

	for( int i = 0; i < nItemCount; ++i )
	{
		int nIndex = m_ListCtrl_Texture_List.GetNextSelectedItem( pos );

		strClipBorad += m_ListCtrl_Texture_List.GetItemText( nIndex,  E_TYPE::TEXTURE_NAME );
		strClipBorad = strClipBorad + "\r\n";
	}

	HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (strClipBorad.GetLength() + 1) *sizeof(TCHAR));
	PSTR pGlobal = (PSTR)GlobalLock(hGlobal);
	lstrcpy(pGlobal, TEXT(strClipBorad));
	GlobalUnlock(hGlobal);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

void DlgObejctTextureList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_OBJECT_NAME, m_Object_Name);
	DDX_Control(pDX, IDC_LIST_TEXTURE_LIST, m_ListCtrl_Texture_List);
	DDX_Control(pDX, IDR_TEXTURE_UPDATE, m_Button_Texture_Update);
	DDX_Control(pDX, IDC_STATIC_OBJECT_NAME, m_Static_Object_Name);
	DDX_Control(pDX, IDC_BUTTON_FOLDER_OPEN, m_Button_Folder_Open);
}


BEGIN_MESSAGE_MAP(DlgObejctTextureList, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TEXTURE_LIST, &DlgObejctTextureList::OnLvnItemchangedListTextureList)
	ON_BN_CLICKED(IDR_TEXTURE_UPDATE, &DlgObejctTextureList::OnBnClickedTextureUpdate)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER_OPEN, &DlgObejctTextureList::OnBnClickedButtonFolderOpen)
END_MESSAGE_MAP()


// DlgObejctTextureList 메시지 처리기입니다.

void DlgObejctTextureList::OnLvnItemchangedListTextureList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(  pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == ( LVIS_SELECTED | LVIS_FOCUSED )  )
	{
		CString strItemString = m_ListCtrl_Texture_List.GetItemText( pNMLV->iItem, E_TYPE::TEXTURE_NAME );

		char* szMakePath = strItemString.GetBuffer();
		m_Select_Texture_FileName = szMakePath;
		m_Select_Full_Texture_Name = MakePath( DIR_MODELTEX, szMakePath );

		m_pSelectGmObject_info = m_ObjectTextureContainer.FindTexture( strItemString.GetBuffer() );
		m_nSelectIndex = m_pSelectGmObject_info->nTextureIndex;
	}

	*pResult = 0;
}

void DlgObejctTextureList::OnBnClickedTextureUpdate()
{
	if( !m_pSelectGmObject_info || m_nSelectIndex < 0 )
		return ;

	// 선택한 텍스쳐 객체를 얻는다.
	LPDIRECT3DTEXTURE9 lpSelectTexture = m_pSelectGmObject_info->pObject->m_pMtrlBlkTexture[ m_nSelectIndex ];

	if( !lpSelectTexture )
		return ;

	//@@ [ YT (2012/ 10/ 29 ) ] : 텍스쳐 메니져에서 선택된 텍스쳐 객체를 찾아 실제 메모리에서 삭제 시키고, 삭제 시킬 텍스쳐 객체를 이용하는
	//							  GMOBJECT 객체를 찾아 연결된 애들을 끊어준다.
	int nChangeTextureID = m_ObjectTextureContainer.UnLink_TexturePointer( m_Select_Texture_FileName, lpSelectTexture );

	if( nChangeTextureID < 0 )
		return ;

	// @@ [ YT (2012/ 10/ 29 ) ] : 선택된 텍스쳐 를 다시 로딩시킨다.
	LPDIRECT3DTEXTURE9 pNewTexture = NULL;

	LoadTextureFromRes( g_pWorldView->m_pd3dDevice, m_Select_Full_Texture_Name.GetBuffer(), 
		D3DX_DEFAULT, D3DX_DEFAULT, 4, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
		D3DPOOL_MANAGED,  D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR              , 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR               , 0x00000000, NULL, NULL,
		&pNewTexture );	

	// @@ [ YT (2012/ 10/ 29 ) ] : 텍스쳐 메니져에 다시 로딩된 텍스쳐 객체를 연결시켜주고 , GMOBJECT 객체들도 찾아 연결시켜준다.
	m_ObjectTextureContainer.Link_TexturePointer( nChangeTextureID, m_Select_Texture_FileName, pNewTexture );
}

void DlgObejctTextureList::OnBnClickedButtonFolderOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CFileDialog dlg( TRUE, "dds", m_Select_Texture_FileName.GetBuffer(), OFN_FILEMUSTEXIST, "DDS files (*.dds)|*.dds||" );

	char* szTemp = "\\";
	CString test = MakePath( DIR_MODELTEX, szTemp );

	dlg.m_ofn.lpstrInitialDir = test.GetBuffer();
	dlg.m_ofn.lpstrFile = m_Select_Full_Texture_Name.GetBuffer( 4096  );                 
	dlg.m_ofn.nMaxFile = 4096;

	if( IDOK == dlg.DoModal() )
		ShellExecute(  NULL, "open", m_Select_Full_Texture_Name, "", NULL , SW_SHOW );
}
