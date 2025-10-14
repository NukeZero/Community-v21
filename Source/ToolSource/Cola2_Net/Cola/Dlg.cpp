// Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cola.h"
#include "ColaDoc.h"
#include "ColaView.h"
#include "Dlg.h"

#include "ASE.h"
//#include "Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CScript g_DefineSound;

/////////////////////////////////////////////////////////////////////////////
// CMyDialog

IMPLEMENT_DYNAMIC(CMyDialog, CFileDialog)

CMyDialog::CMyDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CMyDialog, CFileDialog)
	//{{AFX_MSG_MAP(CMyDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMyDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CDlgOption dialog


CDlgOption::CDlgOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOption)
	m_fScale = 0.0f;
	m_nR = 0;
	m_nG = 0;
	m_nB = 0;
	m_strResource = _T("");
	m_strTexture = _T("");
	//}}AFX_DATA_INIT
}


void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOption)
	DDX_Text(pDX, IDC_EDIT1, m_fScale);
	DDX_Text(pDX, IDC_AMB_R, m_nR);
	DDV_MinMaxByte(pDX, m_nR, 0, 255);
	DDX_Text(pDX, IDC_AMB_G, m_nG);
	DDV_MinMaxByte(pDX, m_nG, 0, 255);
	DDX_Text(pDX, IDC_AMB_B, m_nB);
	DDV_MinMaxByte(pDX, m_nB, 0, 255);
	DDX_Text(pDX, IDC_EDIT_RESOURCE, m_strResource);
	DDX_Text(pDX, IDC_EDIT_TEXTURE, m_strTexture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	//{{AFX_MSG_MAP(CDlgOption)
	ON_BN_CLICKED(IDC_COLORDLG, OnColordlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnColordlg() 
{
	// TODO: Add your control notification handler code here
	CColorDialog	dlgColor;
	if( dlgColor.DoModal() == IDOK )
	{
		COLORREF	color = dlgColor.GetColor();
		int r = color & 0xff;
		int g = (color >> 8) & 0xff;
		int b = (color >> 16) & 0xff;
		g_Option.m_bgColor = D3DCOLOR_ARGB( 255, r, g, b );
		Invalidate( TRUE );
	}

}

void CDlgOption::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	RECT rect = { 100, 130, 150, 170 };
	int r = (g_Option.m_bgColor >> 16) & 0xff;
	int g = (g_Option.m_bgColor >> 8) & 0xff;
	int b = g_Option.m_bgColor & 0xff;
	dc.FillSolidRect( &rect, RGB(r, g, b) );
	
	// Do not call CDialog::OnPaint() for painting messages
}


/////////////////////////////////////////////////////////////////////////////
// CDlgConvert dialog
static _TCHAR *_gszColumnLabel[ 2 ] =
{
	_T("번호"), _T("ASE 파일")
};
static int _gnColumnWidth[ 2 ] =
{
	50, 600
};


CDlgConvert::CDlgConvert(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConvert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConvert)
	m_strState = _T("");
	//}}AFX_DATA_INIT
}


void CDlgConvert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConvert)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT1, m_strState);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConvert, CDialog)
	//{{AFX_MSG_MAP(CDlgConvert)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CONVERT, OnConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConvert message handlers

BOOL CDlgConvert::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	int		i;
	for( i = 0; i < 2; i ++ )
	{
		lvc.iSubItem = i;
		lvc.pszText = _gszColumnLabel[i];
		lvc.cx = _gnColumnWidth[i];
		lvc.fmt = LVCFMT_LEFT;
		m_List.InsertColumn( i, &lvc );
	}
	DragAcceptFiles( TRUE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// 외부에서 주는 스트링을 대화화면에 넘겨줌.(컴퓨터측)
// char *버전
void CDlgConvert :: SayMessage( LPCTSTR str )
{
	CString s(str);

	UpdateData(TRUE);
//	m_strState += s + "\r\n";
	m_strState += s;
	UpdateData( FALSE );

	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT1);
	edit->SetSel( m_strState.GetLength(), m_strState.GetLength() );
}

void CDlgConvert :: AddList( LPCTSTR szFileName )
{
	int		nIdx;
	CString	strName = szFileName;

	// ASE가 맞는지 검사
	strName.MakeLower();
	if( strName.Right(3) != "ase" )
		return;

	m_straryList.Add( szFileName );

	nIdx = m_straryList.GetSize() - 1;

	LV_ITEM lvi;
	CString		strItem;

	lvi.mask = LVIF_TEXT;
	lvi.iItem = nIdx;
	
	lvi.iSubItem = 0;
	strItem.Format( "%04d", nIdx + 1 );
	lvi.pszText = (LPTSTR)((LPCTSTR)strItem);
	m_List.InsertItem(&lvi);

	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)((LPCTSTR)m_straryList.GetAt(nIdx));
	m_List.SetItem(&lvi);

}

//
//	ON_WM_DROPFILES()
//	afx_msg void OnDropFiles( HDROP hDropInfo ); 
//	위 두개를 반드시 포함시킬것.
//
void CDlgConvert :: OnDropFiles( HDROP hDropInfo )
{
	char	szFileName[1024];
	int		nSize;
	int		i;

	nSize = DragQueryFile( hDropInfo ,-1 ,  szFileName ,  1024 );
	for( i = 0; i < nSize; i ++ )
	{
		DragQueryFile( hDropInfo ,i ,  szFileName ,  1024 );
		AddList( szFileName );
	}

	CDialog::OnDropFiles( hDropInfo );
}


// 리스트에서 ase파일명을 하나 꺼낸다
// mvr_이면 다시 언더라인(_)이 몇개 있는지 검색해서, 
// 한개면 bone과 mesh로 변환
// 두개면 ani로 변환
// mvr_이외의 ase는 mes로 변환
void CDlgConvert::OnConvert() 
{
	// TODO: Add your control notification handler code here
	int	nSize = m_straryList.GetSize();
	int	i;
	CString	strASE, strHead, strName, strTitle, strSimple;
	CString	strMsg;
	static CAse	ase;
	int		nType = 0;

	for( i = 0; i < nSize; i ++ )		// ase 리스트 크기
	{
		strASE = m_straryList.GetAt(i);		// ASE이름 하나 꺼냄

		strSimple = GetFileName( strASE );
		strMsg.Format( "%s 변환중...", strSimple );
		SayMessage( strMsg );

		ase.LoadModel( strASE );					// ase 읽음
		ase.SaveAuto( strASE );

		/*

		strTitle = GetFileTitle( strASE );
		strSimple = GetFileName( strASE );
		strHead = strTitle.Left(strTitle.Find( '_' )+1);		// 헤더부분만 떼냄.  mvr_  part_
		strHead.MakeLower();

		strMsg.Format( "%s 변환중...", strSimple );
		SayMessage( strMsg );

		strTitle = strASE.Left( strASE.GetLength() - 4 );	// 패스 + 타이틀만 남겨두고 확장자는 떼냄.

		ase.LoadModel( strASE );					// ase 읽음
		CGeoMObject	*pBiped = ase.GetBiped();	// 바이페드포인터

		// 본+스킨형과 일반형의 세이브를 따로 불러준다
		// 스킨형은 o3d, ani로 뽑아냄
//		if( strHead == "mvr_" )
		if( pBiped )		// 
		{
			int		n = 0, nNum = 0;
			while(1)		// 이름내 _갯수를 셈
			{
				n = strSimple.Find( '_', n );
				if( n == -1 )	break;
				n++;
				nNum ++;
			}
			// 바이페드ase 중에서....
			if( strHead == "mvr_" )	// mvr은 스킨과 애니메이션 두가지 형태
			{
				switch( nNum )
				{
				case 1:		nType = 0;	break;		// mvr_heroine.ase 밑줄하나형태. 스킨만 뽑아냄.
				case 2:		
				default:	nType = 2;	break;		// mvr_heroine_run.ase 밑줄두개나 그이상. 애니메이션만 뽑음.
				}
			} else
			if( strHead == "parts_" ) // parts는 모두 스킨.
			{
				nType = 1;		// 스킨만 뽑아냄.
			} else
			{
				nType = 0;		// ase파일명으로 o3d생성.
			}

			switch( nType )
			{
			case 0:		// test.ase 파일명에 밑줄이 없는경우.
				strName = strTitle + ".o3d";			// test -> test.o3d
				ase.SaveSkinOne( strName );				// 파일 하나짜리로 저장.
				// mes
				break;
			case 1:		// mvr_heroine.ase	// 파일명에 밑줄이 하나 있는 경우 스킨을 뽑아냄
				// 
				{
				strName = strTitle + ".o3d";			// test -> test.o3d
				ase.SaveSkinEach( strName );				// save skin
				}
				break;
			case 2:		// mvr_heroine_attack.ase		// 파일명에 밑줄이 2개이상 있는 경우
			default:	// mvr_heroine_attack_2.ase
				// ani
				strName = strTitle + ".ani";			// test -> test.ani
				ase.SaveANI( strName );				// save ani
				break;
			}
			// save 완료
		} else
		// 바이페드가 아닌것들은 일반오브젝트로 뽑아냄
		{
			// mes로 변환
			strName = strTitle + ".o3d";			// test -> test.o3d
			ase.SaveNormMesh( strName );			// save normal mesh
		}
*/
		ase.Destroy();

		strMsg = "완료";
		strMsg += "\r\n";
		SayMessage( strMsg );
	}
	
	SayMessage( "모든 변환 완료\r\n" );

	
}
/////////////////////////////////////////////////////////////////////////////
// CDlgTexture dialog


CDlgTexture::CDlgTexture(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTexture::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTexture)
	m_fScrlU = 0.0f;
	m_fScrlV = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgTexture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTexture)
	DDX_Text(pDX, IDC_USCRL, m_fScrlU);
	DDX_Text(pDX, IDC_VSCRL, m_fScrlV);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTexture, CDialog)
	//{{AFX_MSG_MAP(CDlgTexture)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTexture message handlers
/////////////////////////////////////////////////////////////////////////////
// CDlgInfo dialog


CDlgInfo::CDlgInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInfo)
	DDX_Control(pDX, IDC_LIST2, m_ctrlList2);
	DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgInfo)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInfo message handlers

BOOL CDlgInfo::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreCreateWindow(cs);
}
//////////////////////////////////////////////////

BOOL CDlgInfo::OnInitDialog() 
{
	int		i;

	CDialog::OnInitDialog();

	// 탭 컨트롤...
	TC_ITEM tcItem;
	tcItem.mask    = TCIF_TEXT | TCIF_IMAGE;
	LPTSTR lpszTabName[] = { "General", "Bone", "DDR" };
	
	for ( i = 0; i < 3; i ++ ) 
	{
		tcItem.pszText    = lpszTabName[i];
		tcItem.cchTextMax = sizeof(lpszTabName[i]) + 1;
		m_ctrlTab.InsertItem(i, &tcItem);
	}

	static _TCHAR *_gszColumnLabel[ 2 ] =
	{
		_T("알라바스타"), _T("루피")
	};
	static int _gnColumnWidth[ 2 ] =
	{
		120, 75
	};

	// General 리스트 컨트롤
	// TODO: Add extra initialization here
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	for( i = 0; i < 2; i ++ )
	{
		lvc.iSubItem = i;
		lvc.pszText = _gszColumnLabel[i];
		lvc.cx = _gnColumnWidth[i];
		lvc.fmt = LVCFMT_LEFT;
		m_ctrlList.InsertColumn( i, &lvc );
	}

	m_nMaxList = 0;
	m_ctrlList.SetExtendedStyle( LVS_EX_GRIDLINES );

	RECT	rect;
	MoveWindow(0, 0, 200, 500 );	
	
	GetClientRect( &rect );
	m_ctrlList.MoveWindow( &rect );

	{
		static _TCHAR *_gszColumnLabel[ 3 ] =
		{
			_T("  "), _T("Bone"), _T("DDR")
		};
		static int _gnColumnWidth[ 3 ] =
		{
			32, 120, 75
		};
		
		// Bone 리스트 컨트롤
		// TODO: Add extra initialization here
		LV_COLUMN lvc;
		
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		
		for( i = 0; i < 3; i ++ )
		{
			lvc.iSubItem = i;
			lvc.pszText = _gszColumnLabel[i];
			lvc.cx = _gnColumnWidth[i];
			lvc.fmt = LVCFMT_LEFT;
			m_ctrlList2.InsertColumn( i, &lvc );
		}
		
		m_nMaxList2 = 0;
		m_ctrlList2.SetExtendedStyle( LVS_EX_GRIDLINES );
		GetClientRect( &rect );
		m_ctrlList2.MoveWindow( &rect );
	}
	
	m_ctrlList.ShowWindow(SW_SHOW);
	m_ctrlList2.ShowWindow(SW_HIDE);
	
//	AddList( "MaxFace", 235 );
//	AddList( "MaxVertex", 534 );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void	CDlgInfo :: AddList( LPCTSTR szName, LPCTSTR szValue )
{
	LV_ITEM lvi;
	CString		strItem;

	lvi.mask = LVIF_TEXT;
	lvi.iItem = m_nMaxList;
	
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)szName;
	m_ctrlList.InsertItem(&lvi);		// 처음에만 인서트하고

	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)szValue;
	m_ctrlList.SetItem(&lvi);

	m_nMaxList ++;
}

void	CDlgInfo :: UpdateInfo( void )
{
	if( m_pModel == NULL )	return;
	CString		str;
	m_nMaxList = 0;
	m_ctrlList.DeleteAllItems();
	
	GMOBJECT* pGMObject = m_pModel->GetGMObject( );
	str.Format("%d", m_pModel->GetMaxObject());		AddList( "Object", str );
	str.Format( "%d", m_pModel->GetMaxVertex() );	AddList( "VertexList", str );
	if( pGMObject )
		str.Format( "%d", pGMObject->m_nMaxVB );	AddList( "VertexBuffer", str );
	str.Format( "%d", m_pModel->GetMaxFace() );		AddList( "Face", str );
	str.Format( "%d", m_pModel->GetMaxMtrlBlk() );	AddList( "Material Block", str );
	str.Format( "%d", m_pModel->GetMaxBone() );		AddList( "Bones", str );
	AddList( "충돌메시", (m_pModel->IsHaveCollObj())? "있다" : "-" );
}

// Bone tab에 추가되는 리스트
void	CDlgInfo :: AddList2( LPCTSTR szName, LPCTSTR szValue )
{
	LV_ITEM lvi;
	CString		strItem;
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = m_nMaxList2;

	lvi.iSubItem = 0;
	strItem.Format( "%2d", m_nMaxList2 + 1 );
	lvi.pszText = (LPTSTR)((LPCTSTR)strItem);
	m_ctrlList2.InsertItem(&lvi);		// 처음에만 인서트하고
	
	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)szName;
	m_ctrlList2.SetItem(&lvi);
//	m_ctrlList2.InsertItem(&lvi);		// 처음에만 인서트하고
	
//	lvi.iSubItem = 1;
//	lvi.pszText = (LPTSTR)szValue;
//	m_ctrlList2.SetItem(&lvi);
	
	m_nMaxList2 ++;
}

void	CDlgInfo :: UpdateInfo2( char *pBuff )
{
	char	buff[128];
	if( m_pModel == NULL )	return;
	CString		str;
	m_nMaxList2 = 0;
	m_ctrlList2.DeleteAllItems();

	memset( buff, 0, sizeof(buff) );

	while(1)
	{
		if( *pBuff == 0 )	break;
		strcpy( buff, pBuff );		// 첫번째 본이름 카피.
		pBuff += (strlen(pBuff) + 1);

		AddList2( buff, NULL );
	}
}


void CDlgInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgInfo::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_ctrlList.GetSafeHwnd() )
	{
		m_ctrlTab.MoveWindow(0, 0, cx, cy-2);
		m_ctrlList.MoveWindow( 0, 2, cx, cy-24 );
		m_ctrlList2.MoveWindow( 0, 2, cx, cy-24 );
	}
	
}

void CDlgInfo::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_ctrlList.ShowWindow(SW_HIDE);
	m_ctrlList2.ShowWindow(SW_HIDE);
	
	switch (m_ctrlTab.GetCurSel())
	{
	case 0: 
		m_ctrlList.ShowWindow(SW_SHOW);
		break;
	case 1: 
		m_ctrlList2.ShowWindow(SW_SHOW);
		break;
	case 2: 
		break;
	}
	
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgTrack dialog


CDlgTrack::CDlgTrack(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrack::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrack)
	m_strID = _T("");
	//}}AFX_DATA_INIT
	m_nModeDivision = 0;
}


void CDlgTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrack)
	DDX_Text(pDX, IDC_FILENAME_WAV, m_strID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrack, CDialog)
	//{{AFX_MSG_MAP(CDlgTrack)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_EN_KILLFOCUS(IDC_FILENAME_WAV, OnKillfocusFilenameWav)
	ON_COMMAND(IDM_VIEW_FRAME, OnViewFrame)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_FRAME, OnUpdateViewFrame)
	ON_COMMAND(IDM_VIEW_SEC, OnViewSec)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_SEC, OnUpdateViewSec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrack message handlers

BOOL CDlgTrack::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	MoveWindow(0, 0, 1280, 124 );	

	CPaintDC dc(this);
	m_hBitmap.CreateCompatibleBitmap( &dc, 1600, 100 );
	m_hAttrHit.LoadBitmap(IDB_ATTRHIT);
	m_hAttrSnd.LoadBitmap(IDB_ATTRSND);

//	m_clrBk = dc.GetBkColor();
	m_clrBk = 0x00c8d0d4;

	m_pModel = NULL;
//	m_fFrame = 0;
//	m_nMaxFrame = 100;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#define		TRACK_KEY_WIDTH		15

void CDlgTrack::OnPaint() 
{
	char *szText[4] = { "Frame", "Hit/Shoot", "Sound", "Quake" };
	CPaintDC dc(this); // device context for painting
	RECT	cRect;	
	int		w, h, i;
	int		nBorderX = 100;		// 세로구분선의 x좌표
	GetClientRect( &cRect );
	w  = cRect.right - cRect.left; // + 1;
	h = cRect.bottom - cRect.top; // + 1;

	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	CBitmap *pOldBitmap;
	pOldBitmap = memDC.SelectObject( &m_hBitmap );

	CDC	*pDC = &memDC;

	pDC->FillSolidRect(0, 0, w, 100, m_clrBk );		// clear

	CFont font, *pOldFont;
	font.CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE,
					 FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                     VARIABLE_PITCH, "굴림" );
	pOldFont = pDC->SelectObject( &font );

	// 그리드 라인 그리기
	{
	CPen pen, *pOldPen;
	pen.CreatePen( PS_SOLID, 1, RGB(170, 170, 170) );
	pOldPen = pDC->SelectObject( &pen );
	pDC->FillSolidRect( nBorderX, 0, TRACK_KEY_WIDTH, h, RGB(190, 190, 190) );		// 0프레임 왼쪽 빈곳
	if( m_pModel )
	{
		int nMaxFrame = m_pModel->m_nFrameMax;
		pDC->FillSolidRect( nBorderX + TRACK_KEY_WIDTH + nMaxFrame * TRACK_KEY_WIDTH, 0,			// maxframe 오른쪽 빈곳
						  w - nBorderX + TRACK_KEY_WIDTH + nMaxFrame * TRACK_KEY_WIDTH, h, RGB(190, 190, 190) );
	}
	for( i = 16; i < h; i += 16 )		// 가로 라인
	{
		pDC->MoveTo( 0, i );
		pDC->LineTo( w, i );
	}
	if( m_nModeDivision == 0 )
	{
		for( i = nBorderX + TRACK_KEY_WIDTH; i < w; i += (TRACK_KEY_WIDTH * 5) )		// 5프레임단위 세로라인
		{
			pDC->MoveTo( i, 0 );
			pDC->LineTo( i, h );
		}
	} else
	{
		for( i = nBorderX + TRACK_KEY_WIDTH; i < w; i += 60 )		// 0.1초(60도트)단위 세로라인
		{
			pDC->MoveTo( i, 16 );
			pDC->LineTo( i, h );
		}
	}
	pDC->SelectObject( pOldPen );
	}

	CString		str;
	pDC->SetBkMode( TRANSPARENT );
	
	// 왼쪽 텍스트 부분 쓰기
	for( i = 0; i < 4; i ++ )
		pDC->TextOut( 8, 3 + i * 16, szText[i] );
	pDC->SelectObject( pOldFont );

	CFont font2;
	font2.CreateFont( 10, 0, 0, 0, FW_NORMAL, FALSE,
		FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		VARIABLE_PITCH, "굴림" );
	pOldFont = pDC->SelectObject( &font2 );
	
	
	pDC->Rectangle(100, 0, 105, h );		// 세로 구분선
	// 눈금자
	pDC->DrawEdge( CRect(0, 0, w, 16), BDR_RAISEDINNER, BF_RECT );	// 눈금자 막대기
	if( m_nModeDivision == 0 )
	{
		for( i = 0; i < w / TRACK_KEY_WIDTH; i ++ )	// 20도트간격
		{
			if( ((i-1) % 5) == 0 )	pDC->MoveTo( nBorderX + i * TRACK_KEY_WIDTH, 0 );		// 5프레임 간격으로는 긴 눈금.
			else					pDC->MoveTo( nBorderX + i * TRACK_KEY_WIDTH, 16-4 );
			pDC->LineTo( nBorderX + i * TRACK_KEY_WIDTH, 16 );
			if( ((i-1) % 5) == 0 )
			{
				str.Format( "%d", i - 1 );
				pDC->TextOut( nBorderX + i * TRACK_KEY_WIDTH + 2, 3, str );
			}
		}
	} else
	{	// millisec 단위
		for( i = 0; i < w / 30; i ++ )	// 30도트간격
		{
			if( (i % 2) == 0 )	pDC->MoveTo( nBorderX + TRACK_KEY_WIDTH + i * 30, 10 );		// 5프레임 간격으로는 긴 눈금.
			else				pDC->MoveTo( nBorderX + TRACK_KEY_WIDTH + i * 30, 16-2 );
			pDC->LineTo( nBorderX + TRACK_KEY_WIDTH + i * 30, 16 );
			if( (i % 2) == 0 )	// 0.1초 단위
			{
				str.Format( "%1.2f", i * 0.05f );
				pDC->TextOut( nBorderX + 10 + i * 30 + 2, 0, str );
			}
		}
	}
	pDC->SelectObject( pOldFont );
	

	//--------------- 동적으로 변하는 부분
	if( m_pModel )
	{
		int nFrame = (int)(m_pModel->m_nFrameMax * 0.65f);
		int x  = nBorderX + TRACK_KEY_WIDTH + (int)(nFrame * TRACK_KEY_WIDTH);
		pDC->FillSolidRect( x-1, 0, 2, h, RGB(255,0,0) );
		
		if( m_pModel->IsLoadAttr() )		// 속성 메모리가 할당되어 있을때만.
		{
			CDC			dcImg;

			dcImg.CreateCompatibleDC(pDC);
			CBitmap *pOld = dcImg.SelectObject(&m_hAttrHit);

			int i = 0;
			for( i = 0; i < m_pModel->m_nFrameMax; i ++ )
			{
				if( m_pModel->IsAttrHit((float)i) )
					::TransparentBlt( pDC->m_hDC, nBorderX + TRACK_KEY_WIDTH + i * TRACK_KEY_WIDTH - 8,  1 + 1 * 16,  16, 16, 
									  dcImg.m_hDC, 0, 0, 16, 16, RGB(255,255,255) );
			}
			dcImg.SelectObject( pOld );

			pOld = dcImg.SelectObject(&m_hAttrSnd);

			for( i = 0; i < m_pModel->m_nFrameMax; i ++ )
			{
				if( m_pModel->IsAttrSound((float)i) )
					::TransparentBlt( pDC->m_hDC, nBorderX + TRACK_KEY_WIDTH + i * TRACK_KEY_WIDTH - 8,  1 + 2 * 16,  16, 16, 
									  dcImg.m_hDC, 0, 0, 16, 16, RGB(255,255,255) );
			}
			dcImg.SelectObject( pOld );

			// 지진키
			pOld = dcImg.SelectObject(&m_hAttrHit);
			
			for( i = 0; i < m_pModel->m_nFrameMax; i ++ )
			{
				if( m_pModel->IsAttrQuake((float)i) )
					::TransparentBlt( pDC->m_hDC, nBorderX + TRACK_KEY_WIDTH + i * TRACK_KEY_WIDTH - 8,  1 + 3 * 16,  16, 16, 
					dcImg.m_hDC, 0, 0, 16, 16, RGB(255,255,255) );
			}
			dcImg.SelectObject( pOld );
			


			// 현재 프레임번호에 파란선 그리기
			
			float fFrame = m_pModel->m_fFrameCurrent;
			int x  = nBorderX + TRACK_KEY_WIDTH + (int)(fFrame * (float)TRACK_KEY_WIDTH);
			pDC->FillSolidRect( x-1, 0, 3, h, RGB(128,128,255) );
		}
	}


	pDC->SetBkMode( OPAQUE );
//	str.Format( "%d", x );
//	pDC->TextOut( 8, 3 + 4 * 16, str );
	// 복구

	dc.BitBlt(0, 0, w, h, pDC, 0, 0, SRCCOPY );
	memDC.SelectObject( pOldBitmap );

	// 사운드 파일명 EditBox의 활성처리.
	if( m_pModel && m_pModel->IsLoadAttr() )
	{
		CEdit *pEdit = (CEdit *)GetDlgItem( IDC_FILENAME_WAV );
		MOTION_ATTR *pAttr;
		if( pAttr = m_pModel->IsAttrSound() )
		{
//			m_strID.Format( "%d", pAttr->m_nSndID );
			m_strID = g_DefineSound.GetFindIdStr( "SND_", pAttr->m_nSndID );
			UpdateData(FALSE);		// 변수 -> 컨트롤
			pEdit->SetReadOnly(FALSE);		// 입력가능
			pEdit->SetFocus();

		}
		else
		{
			m_strID = "";
			UpdateData(FALSE);	// 변수에서 컨트롤로
			pEdit->SetReadOnly(TRUE);		// 입력불가능
			g_pView->SetFocus();
		}
	}

	// Do not call CDialog::OnPaint() for painting messages
}


void CDlgTrack::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CModel	*pModel = m_pModel;
	if( m_pModel == NULL )	return;
	int		nBorderX = 100;		// 세로구분선의 x좌표

	int nFrame = (point.x - (nBorderX + TRACK_KEY_WIDTH - 10)) / TRACK_KEY_WIDTH;

	if( nFrame >= 0 && nFrame < pModel->m_nFrameMax )
	{
		UpdateWavName();
		pModel->m_fFrameCurrent = (float)nFrame;
		Invalidate(0);
	}


	CDialog::OnLButtonDown(nFlags, point);
}


void CDlgTrack::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CModel	*pModel = m_pModel;
	if( m_pModel == NULL )	return;
	int		nBorderX = 100;		// 세로구분선의 x좌표

	if ( nFlags == MK_LBUTTON )		// 드래깅 중이다.
	{
		int nFrame = (point.x - (nBorderX + TRACK_KEY_WIDTH - 10)) / TRACK_KEY_WIDTH;

		if( nFrame >= 0 && nFrame < pModel->m_nFrameMax )
		{
			UpdateWavName();
			pModel->m_fFrameCurrent = (float)nFrame;
			Invalidate(0);
		}

	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgTrack::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CModel	*pModel = m_pModel;
	int nArea = point.y / 16;
	if( nArea == 0 )
	{
		CMenu		menu, *popup;
		POINT	p;
		
		menu.LoadMenu( IDR_TRACK_POPUP );
		popup = menu.GetSubMenu(0);
		
		GetCursorPos( &p );
		popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, 
			p.x, p.y, this );
		
		menu.DestroyMenu();
	}
	if( m_pModel == NULL )	return;
	int		nBorderX = 100;		// 세로구분선의 x좌표

	int nFrame = (point.x - (nBorderX + TRACK_KEY_WIDTH - 10)) / TRACK_KEY_WIDTH;
		
	if( nFrame >= 0 && nFrame < pModel->m_nFrameMax )
	{
		UpdateWavName();
		pModel->m_fFrameCurrent = (float)nFrame;
		if( nArea == 1 )
		{
			if( m_pModel->IsAttrHit((float)nFrame) )
				m_pModel->ResetAttr( nFrame, MA_HIT );
			else	
				m_pModel->SetAttr( (float)nFrame, MA_HIT );
		} else
		if( nArea == 2 )
		{
			if( m_pModel->IsAttrSound((float)nFrame) )
				m_pModel->ResetAttr( nFrame, MA_SOUND );
			else	
			{
				m_pModel->SetAttr( (float)nFrame, MA_SOUND );
				CEdit *pEdit = (CEdit *)GetDlgItem( IDC_FILENAME_WAV );
				pEdit->SetFocus();
			}
		} else
		if( nArea == 3 )
		{
			if( m_pModel->IsAttrQuake((float)nFrame) )
				m_pModel->ResetAttr( nFrame, MA_QUAKE );
			else	
				m_pModel->SetAttr( (float)nFrame, MA_QUAKE );
		}
				
		Invalidate(0);
	}
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CDlgTrack::OnOK()
{
	UpdateWavName();
//	CWnd *p = GetFocus();
	g_pView->SetFocus();
}

// Editbox에 포커스가 있다가 다른곳으로 바뀌기전에 EditBox의 내용을 옮김.
void CDlgTrack::UpdateWavName( void )
{
	CEdit *pEdit = (CEdit *)GetDlgItem( IDC_FILENAME_WAV );
	if( GetFocus() == pEdit )
	{
		if( m_pModel )
		{
			MOTION_ATTR *pAttr = m_pModel->GetMotionAttr( (int)m_pModel->m_fFrameCurrent );
			if( pAttr )
			{
				UpdateData( TRUE );
				int nID = g_DefineSound.GetDefineNum( m_strID );
				if( nID != -1 )
					pAttr->m_nSndID = nID;
//				pAttr->m_nSndID = atoi( (LPCTSTR)m_strID );		// Wav EditBox에 입력했던 내용을 속성배열에 카피해넣음.
//				strcpy( pAttr->m_szFileName, m_strWav );
			}
		}
	}
}

void CDlgTrack::OnKillfocusFilenameWav() 
{
	// TODO: Add your control notification handler code here
}


void CDlgTrack::OnViewFrame() 
{
	// TODO: Add your command handler code here
	m_nModeDivision = 0;	
	Invalidate();
}

void CDlgTrack::OnUpdateViewFrame(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_nModeDivision == 0 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}

void CDlgTrack::OnViewSec() 
{
	// TODO: Add your command handler code here
	m_nModeDivision = 1;	
	Invalidate();
	
}

void CDlgTrack::OnUpdateViewSec(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_nModeDivision == 1 )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	
}



/////////////////////////////////////////////////////////////////////////////
// CDlgAnimation dialog


CDlgAnimation::CDlgAnimation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAnimation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAnimation)
	//}}AFX_DATA_INIT
}


void CDlgAnimation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAnimation)
	DDX_Control(pDX, IDC_LIST_ANI, m_kListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAnimation, CDialog)
	//{{AFX_MSG_MAP(CDlgAnimation)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ANI, OnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ANI, OnRclickList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAnimation message handlers

BOOL CDlgAnimation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	static _TCHAR *_gszColumnLabel[ 2 ] =
	{
		_T("Anim"), _T("..")
	};

	static int _gWidth[ 2 ] =
	{
		250, 50
	};

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	for( int i = 0; i < 2; ++i )
	{
		lvc.iSubItem = i;
		lvc.pszText = _gszColumnLabel[i];
		lvc.cx = _gWidth[ i ];
		lvc.fmt = LVCFMT_LEFT;
		m_kListCtrl.InsertColumn( i, &lvc );
	}

	m_kListCtrl.SetExtendedStyle( LVS_EX_GRIDLINES );

//  	RECT	rect;
//   	GetClientRect( &rect );
//   	m_kListCtrl.MoveWindow( &rect );
	
	
	m_kListCtrl.ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAnimation::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if( m_kListCtrl.GetSafeHwnd() )
	{
		m_kListCtrl.MoveWindow(0, 0, cx, cy-24);
	}
	
	// TODO: Add your message handler code here
	
}

int CDlgAnimation::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

int CDlgAnimation::ListUpAllAnimation( const char* szFilename )
{
	int nAni = 0;

	// TODO : 관련이 있는 Ani파일을 모두 읽어드린후 list up!

	CString szOrgName;
	
	if( strstrlwr( szFilename, "part_" ) )
	{
		if( strstrlwr( szFilename, "part_m" ) ) //남성 
			szOrgName = "mvr_male";
		else szOrgName = "mvr_female";
	}else
	{
		szOrgName = GetFileTitle( szFilename );
	}
	
	// 1. 초기화 
	m_kListCtrl.DeleteAllItems( );
	
	// 2. "*.ani"를 대상으로 szFilename의 확장자를 제거한 같은 문자열이 있다면 등록한다.
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;
//	TCHAR path[MAX_PATH];

//	GetCurrentDirectory( MAX_PATH, path );

//	CString strPath = GetResourcePath();
//	strPath += "Model/*.ani";
	
//	lstrcat( path, "*.ani" );
	
	//gmpbigsun(100420) : 모델경로를 최초 o3d를 불러온 디렉토리로 설정
	CString strPath = g_strModelPath + "*.ani";
	hSrch = FindFirstFile( strPath, &wfd );
	if( hSrch == INVALID_HANDLE_VALUE )
		return 0;

	
	int index = 0;
	while( bResult )
	{
		BOOL bFind = strstrlwr( wfd.cFileName, szOrgName.GetBuffer( 0 ) );		// abcd.o3d 라면 ani이름 규칙은 abcd_actionname 임 
		if( bFind )
		{
			//OK 이녀석 ani임
			m_kListCtrl.InsertItem( index++, wfd.cFileName );
		}

		bResult = FindNextFile( hSrch, &wfd );
	}

	FindClose( hSrch );

	return nAni;
}

void CDlgAnimation::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	TCHAR szData[ 256 ];
	int iSelectedItem = pNMListView->iItem;
	m_kListCtrl.GetItemText( iSelectedItem, 0, szData, 255 );

	//	CString strPath = GetResourcePath();

	//gmpbigsun(20100420) : 리소스 디렉토리 갱신
	CString strPath = g_strModelPath;

	CModelObject* pModel = (CModelObject*)g_pView->m_pModel;
	if( !pModel )
	{
		AfxMessageBox( "모델이 없습니다!" );
	}
	else
	{
		//strPath += "Model/";
		strPath += szData;
		pModel->LoadMotion( strPath );
	}
	
	*pResult = 0;
}

void CDlgAnimation::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	CModelObject* pModel = (CModelObject*)g_pView->m_pModel;
	if( pModel )
	{
		pModel->InitAnimate( );
	}
}
