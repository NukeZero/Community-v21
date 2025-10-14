// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sfxeditor.h"
#include "InfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog


CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Control(pDX, IDC_LIST2, m_ListBoxTexture);
	DDX_Control(pDX, IDC_LIST1, m_cListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers

BOOL CInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText( g_SfxMng.GetSfxBase(0)->m_strName );
	
	CString strInfo;

	for(int i=0;i<g_SfxMng.GetSfxBase(0)->m_apParts.GetSize();i++) 
	{
		CString strType;
		switch( g_SfxMng.GetSfxBase(0)->Part(i)->m_nType )
		{
			case SFXPARTTYPE_BILL:
				strType = "B";
				break;
			case SFXPARTTYPE_PARTICLE:
				strType = "P";
				break;
			case SFXPARTTYPE_MESH:
				strType = "M";
				break;
			case SFXPARTTYPE_CUSTOMMESH:
				strType = "C";
				break;
			default:
				ASSERT(0);
				
		}
			
		strInfo.Empty();
		strInfo.Format( "%3d        %s       %s", i, strType, g_SfxMng.GetSfxBase(0)->Part(i)->m_strTex );
		m_cListBox.AddString(strInfo);

		if( m_ListBoxTexture.GetCount() == 0 )
			m_ListBoxTexture.AddString(g_SfxMng.GetSfxBase(0)->Part(i)->m_strTex);
		
		int nIndex = m_ListBoxTexture.FindString( 0, g_SfxMng.GetSfxBase(0)->Part(i)->m_strTex );

		if( nIndex == -1 )
			m_ListBoxTexture.AddString(g_SfxMng.GetSfxBase(0)->Part(i)->m_strTex);
	}


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CInfoDlg::OnSelchangeList2() 
{
	// TODO: Add your control notification handler code here
	CString source; 
	m_ListBoxTexture.GetText( m_ListBoxTexture.GetCurSel() , source );
	
    //put your text in source
	
    if(OpenClipboard())
		
    {
		
        HGLOBAL clipbuffer;
		
        char * buffer;
		
        EmptyClipboard();
		
        clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength()+1);
		
        buffer = (char*)GlobalLock(clipbuffer);
		
        strcpy(buffer, LPCSTR(source));
		
        GlobalUnlock(clipbuffer);
		
        SetClipboardData(CF_TEXT,clipbuffer);
		
        CloseClipboard();
		
    }
	
}
