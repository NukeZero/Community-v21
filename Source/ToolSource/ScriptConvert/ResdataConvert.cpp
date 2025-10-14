// ResdataConvert.cpp : implementation file
//

#include "stdafx.h"
#include "resmanager.h"
#include "ScriptConvert.h"
#include "ResdataConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResdataConvert dialog


CResdataConvert::CResdataConvert(CWnd* pParent /*=NULL*/)
	: CDialog(CResdataConvert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResdataConvert)
	m_DestPath = _T("");
	m_SrcPath = _T("");
	m_bSkip = FALSE;
	//}}AFX_DATA_INIT

	m_nLang = -1;
	m_nLangKor = -1;
	
	g_codePage = 949;
}


void CResdataConvert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResdataConvert)
	DDX_Control(pDX, IDC_COMBO2, m_ChangeLangKor);
	DDX_Control(pDX, IDC_COMBO1, m_ChangeLang);
	DDX_Text(pDX, IDC_DESTEDIT, m_DestPath);
	DDX_Text(pDX, IDC_SRCEDIT, m_SrcPath);
	DDX_Check(pDX, IDC_CHECK1, m_bSkip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResdataConvert, CDialog)
//{{AFX_MSG_MAP(CResdataConvert)
	ON_BN_CLICKED(IDC_RES_BUTTON1, OnResButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_RES_BUTTON3, OnResButton3)
	ON_BN_CLICKED(IDC_RES_BUTTON2, OnResButton2)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





void CResdataConvert::OnResButton1() 
{
	if( m_nLangKor == -1 )
	{
		AfxMessageBox( "Please Select Nation" );
		return;
	}

	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Resdata Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());

	if ( dlg.DoModal() == IDOK )
    {
		CScript script;
		if( script.Load( "lang.h" ) == TRUE )
			script.PreScan( FALSE );
		
		if( m_ressrc.Load( dlg.GetFileName() ) )
		{
			char Buffer[1024] = { 0 };

			GetCurrentDirectory( 1024, Buffer );

			m_SrcPath = Buffer+("\\"+dlg.GetFileName());
		}
		else
			m_SrcPath = "";

		UpdateData(FALSE);
	}
}

void CResdataConvert::OnResButton2() 
{
	if( m_nLang == -1 )
	{
		AfxMessageBox( "Please Select Nation" );
		return;
	}
	
	CFileDialog dlg(   
        TRUE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Resdata Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());
	
	if ( dlg.DoModal() == IDOK )
    {
		CScript script;
		if( script.Load( "lang.h" ) == TRUE )
			script.PreScan( FALSE );
		
		if( m_resdest.Load( dlg.GetFileName() ))
		{
			char Buffer[1024] = { 0 };
			
			GetCurrentDirectory( 1024, Buffer );
			
			m_DestPath = Buffer+("\\"+dlg.GetFileName());
		}
		else
			m_DestPath = "";
		
		UpdateData(FALSE);
	}
}

BOOL CResdataConvert::ConvertStart(CResManager *pSrcWndApplet, CResManager *pDestWndApplet)
{
	DWORD dwAppletId_dest;
	LPWNDAPPLET LPWNDAPPLET_dest;
	POSITION pos_dest = pDestWndApplet->m_mapWndApplet.GetStartPosition();

	while(pos_dest)
	{ 
		pDestWndApplet->m_mapWndApplet.GetNextAssoc( pos_dest, (void*&)dwAppletId_dest, (void*&)LPWNDAPPLET_dest );

		LPWNDCTRL lpWndCtrl;
		LPWNDAPPLET lpWndApplet;

		if( lpWndApplet = IsSameDefine(pSrcWndApplet, LPWNDAPPLET_dest ) )
		{
			CString pp1 = lpWndApplet->strTitle;
			CString pp2 = LPWNDAPPLET_dest->strTitle;

#ifdef __DAISY
			if( m_bSkip )
			{
				if( _wcsicmp( lpWndApplet->awszTitle[0], LPWNDAPPLET_dest->awszTitle[0] ) == 0 )
					wcscpy( lpWndApplet->awszTitle[m_nLangKor], LPWNDAPPLET_dest->awszTitle[m_nLang] );
				else
				{
					*lpWndApplet->awszTitle[m_nLangKor] = 0;
				}
				
				if( _wcsicmp( lpWndApplet->awszToolTip[0], LPWNDAPPLET_dest->awszToolTip[0] ) == 0 )
					wcscpy( lpWndApplet->awszToolTip[m_nLangKor], LPWNDAPPLET_dest->awszToolTip[m_nLang] );
				else
				{
					*lpWndApplet->awszToolTip[m_nLangKor] = 0;
					//wcscpy( lpWndApplet->awszToolTip[m_nLangKor], 0 );
				}
			}
			else
			{
				wcscpy( lpWndApplet->awszTitle[m_nLangKor], LPWNDAPPLET_dest->awszTitle[m_nLang] );
				wcscpy( lpWndApplet->awszToolTip[m_nLangKor], LPWNDAPPLET_dest->awszToolTip[m_nLang] );
			}
#endif

			for( int i = 0; i < LPWNDAPPLET_dest->ptrCtrlArray.GetSize(); i++ )
			{
				LPWNDCTRL pWndCtrl = NULL;

				pWndCtrl = (LPWNDCTRL) LPWNDAPPLET_dest->ptrCtrlArray.GetAt( i );

				if( lpWndCtrl = IsSameDefine( lpWndApplet, pWndCtrl->strDefine ) )
				{
#ifdef __DAISY
					if( m_bSkip )
					{
						if( _wcsicmp( lpWndCtrl->awszTitle[0], pWndCtrl->awszTitle[0] ) == 0 )
							wcscpy( lpWndCtrl->awszTitle[m_nLangKor], pWndCtrl->awszTitle[m_nLang] );
						else
						{
							*lpWndCtrl->awszTitle[m_nLangKor] = 0;
							//wcscpy( lpWndCtrl->awszTitle[m_nLangKor], 0 );
						}
						
						if( _wcsicmp( lpWndCtrl->awszToolTip[0], pWndCtrl->awszToolTip[0] ) == 0 )
							wcscpy( lpWndCtrl->awszToolTip[m_nLangKor], pWndCtrl->awszToolTip[m_nLang] );
						else
						{
							*lpWndCtrl->awszToolTip[m_nLangKor] = 0;
							//wcscpy( lpWndCtrl->awszToolTip[m_nLangKor], 0 );
						}
					}
					else
					{
						wcscpy( lpWndCtrl->awszTitle[m_nLangKor], pWndCtrl->awszTitle[m_nLang] );
						wcscpy( lpWndCtrl->awszToolTip[m_nLangKor], pWndCtrl->awszToolTip[m_nLang] );
					}
#endif
				}
			}
			
		}
	}

	return TRUE;
}


LPWNDAPPLET CResdataConvert::IsSameDefine(CResManager *pWndApplet, LPWNDAPPLET lpWndApplet)
{
	DWORD dwAppletId_src;
	POSITION pos_src;
	LPWNDAPPLET LPWNDAPPLET_src;

	pos_src = pWndApplet->m_mapWndApplet.GetStartPosition();

	while(pos_src)
	{ 
		pWndApplet->m_mapWndApplet.GetNextAssoc( pos_src, (void*&)dwAppletId_src, (void*&)LPWNDAPPLET_src );
		
		if( lpWndApplet->strDefine == LPWNDAPPLET_src->strDefine )
			return LPWNDAPPLET_src;
	}

	return NULL;
}

LPWNDCTRL CResdataConvert::IsSameDefine(LPWNDAPPLET lpWndApplet, LPCTSTR lpszDefine )
{
	for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
	{
		LPWNDCTRL lpWndCtrl = (LPWNDCTRL)lpWndApplet->ptrCtrlArray.GetAt( i );

		if( lpWndCtrl->strDefine == lpszDefine )
		{
			return lpWndCtrl;
		}
	}

	return NULL;
}

void CResdataConvert::OnSelchangeCombo1() 
{
	m_nLang = m_ChangeLang.GetCurSel();
}

void CResdataConvert::OnSelchangeCombo2() 
{
	m_nLangKor = m_ChangeLangKor.GetCurSel();
	
}

void CResdataConvert::OnResButton3() 
{
	if( m_nLang == -1 || m_nLangKor == -1 )
	{
		AfxMessageBox( "Please Select Nation" );
		return;
	}

	CFileDialog dlg(   
        FALSE, // for open
        NULL, // default extension
        NULL, // initial filename
        OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_LONGNAMES,  // flags
        "Resdata Files (*.inc)|*.inc|All Files (*.*)|*.*||", // filters
        AfxGetMainWnd());

	if ( dlg.DoModal() == IDOK )
    {
		ConvertStart( &m_ressrc, &m_resdest );

		m_ressrc.SetModifiedScript();
		m_resdest.SetModifiedScript();

		CString FileName = dlg.GetFileTitle();
#ifdef __DAISY
		if( m_ressrc.Save(FileName+".inc") )
		{
			AfxMessageBox( "Save OK!!!" );
		}
#endif
	}
}

void CResdataConvert::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	m_bSkip = !m_bSkip;	
}

BOOL CResdataConvert::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_bSkip = TRUE;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
