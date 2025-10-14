// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "BetaPatchClient.h"
#include "DlgOption.h"
#include "BetaPatchClientDlg.h"
#include "Picture.h"
#include "DialogOptionFrame.h"

#include <D3D9.h>
#pragma comment	(lib, "d3d9.lib")

/////////////////////////////////////////////////////////////////////////////
// CDlgOption dialog

CDlgOption::CDlgOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOption)
	m_nTexQual = 0;
	m_nViewArea = 0;
	m_nObjectDetail = 0;
	m_nShadow = -1;
	//}}AFX_DATA_INIT
	m_bStartFullScreen = TRUE;
	m_nNormalRCount = 0;
	m_nWideRCount = 0;
}

void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOption)
	DDX_Control(pDX, IDCANCEL, m_BtnCANCEL);
	DDX_Control(pDX, IDOK, m_BtnOK);
	DDX_Control(pDX, IDC_RES_COMBO, m_cbRes);
	DDX_Radio(pDX, IDC_TQ_RADIO1, m_nTexQual);
	DDX_Radio(pDX, IDC_VIEW_RADIO1, m_nViewArea);
	DDX_Radio(pDX, IDC_OD_RADIO1, m_nObjectDetail);
	DDX_Radio(pDX, IDC_SHD_RADIO1, m_nShadow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	//{{AFX_MSG_MAP(CDlgOption)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOption message handlers

//CFont m_font;
extern CBetaPatchClientDlg *g_pDlg;
BOOL CDlgOption::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//transparent
	m_clrTransparent = ::GetSysColor( COLOR_3DFACE ) ; //RGB( 0, 255, 0 );
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_BORDER) == 0);
	VERIFY((dwStyle & WS_SIZEBOX) == 0);
	VERIFY((dwStyle & WS_DLGFRAME) == 0);

	m_brush.CreateSolidBrush(m_clrTransparent);
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	VERIFY((dwExStyle & WS_EX_APPWINDOW) == 0);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	HMODULE hInst = LoadLibrary(_T("User32.DLL")); 
	typedef BOOL (WINAPI *MYFUNC)(HWND, COLORREF, BYTE, DWORD); 
	MYFUNC SetLayeredWindowAttributes = NULL; 
	SetLayeredWindowAttributes = (MYFUNC)::GetProcAddress(hInst, "SetLayeredWindowAttributes"); 
	SetLayeredWindowAttributes(GetSafeHwnd(), m_clrTransparent, 0, 1); 
	::FreeLibrary(hInst);

	LoadSkin();
	LoadButton();

	IDirect3D9* pD3D9;
	int nSelect = 0;
	pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	
	BOOL checkRes[11] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

	D3DFORMAT allowedAdapterFormat = D3DFMT_X8R8G8B8;
	UINT numAdapterModes = pD3D9->GetAdapterModeCount( 0, allowedAdapterFormat );
	for (UINT mode = 0; mode < numAdapterModes; mode++)
	{
		D3DDISPLAYMODE displayMode;
		pD3D9->EnumAdapterModes( 0, allowedAdapterFormat, mode, &displayMode );
		if( !checkRes[0] && displayMode.Width == 800 && displayMode.Height == 600 )
		{
			if(m_cbRes.FindString(0, "800x600") == -1)
			{
				m_cbRes.InsertString( -1, "800x600" );
				checkRes[0] = TRUE;
				m_nNormalRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[1] && displayMode.Width == 1024 && displayMode.Height == 768 )
		{
			if(m_cbRes.FindString(0, "1024x768") == -1)
			{
				m_cbRes.InsertString( -1, "1024x768" );
				checkRes[1] = TRUE;
				m_nNormalRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[2] && displayMode.Width == 1280 && displayMode.Height == 1024 )
		{
			if(m_cbRes.FindString(0, "1280x1024") == -1)
			{
				m_cbRes.InsertString( -1, "1280x1024" );
				checkRes[2] = TRUE;
				m_nNormalRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[3] && displayMode.Width == 1400 && displayMode.Height == 1050 )
		{
			if(m_cbRes.FindString(0, "1400x1050") == -1)
			{
				m_cbRes.InsertString( -1, "1400x1050" );
				checkRes[3] = TRUE;
				m_nNormalRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[4] && displayMode.Width == 1600 && displayMode.Height == 1200 )
		{
			if(m_cbRes.FindString(0, "1600x1200") == -1)
			{
				m_cbRes.InsertString( -1, "1600x1200" );
				checkRes[4] = TRUE;
				m_nNormalRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[5] && displayMode.Width == 1280 && displayMode.Height == 720 )
		{
#if __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1280x720 (Breitbild)") == -1)
			{
				m_cbRes.InsertString( -1, "1280x720 (Breitbild)" );
#else // __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1280x720 (Wide)") == -1)
			{
				m_cbRes.InsertString( -1, "1280x720 (Wide)" );
#endif // __CURRENT_LANG == LANG_GER
				checkRes[5] = TRUE;
				m_nWideRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[6] && displayMode.Width == 1280 && displayMode.Height == 768 )
		{
#if __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1280x768 (Breitbild)") == -1)
			{
				m_cbRes.InsertString( -1, "1280x768 (Breitbild)" );
#else // __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1280x768 (Wide)") == -1)
			{
				m_cbRes.InsertString( -1, "1280x768 (Wide)" );
#endif // __CURRENT_LANG == LANG_GER
				checkRes[6] = TRUE;
				m_nWideRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[7] && displayMode.Width == 1280 && displayMode.Height == 800 )
		{
#if __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1280x800 (Breitbild)") == -1)
			{
				m_cbRes.InsertString( -1, "1280x800 (Breitbild)" );
#else // __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1280x800 (Wide)") == -1)
			{
				m_cbRes.InsertString( -1, "1280x800 (Wide)" );
#endif // __CURRENT_LANG == LANG_GER
				checkRes[7] = TRUE;
				m_nWideRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[8] && displayMode.Width == 1360 && displayMode.Height == 768 )
		{
#if __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1360x768 (Breitbild)") == -1)
			{
				m_cbRes.InsertString( -1, "1360x768 (Breitbild)" );
#else // __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1360x768 (Wide)") == -1)
			{
				m_cbRes.InsertString( -1, "1360x768 (Wide)" );
#endif // __CURRENT_LANG == LANG_GER
				checkRes[8] = TRUE;
				m_nWideRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[9] && displayMode.Width == 1440 && displayMode.Height == 900 )
		{
#if __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1440x900 (Breitbild)") == -1)
			{
				m_cbRes.InsertString( -1, "1440x900 (Breitbild)" );
#else // __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1440x900 (Wide)") == -1)
			{
				m_cbRes.InsertString( -1, "1440x900 (Wide)" );
#endif // __CURRENT_LANG == LANG_GER
				checkRes[9] = TRUE;
				m_nWideRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
		else if( !checkRes[10] && displayMode.Width == 1680 && displayMode.Height == 1050 )
		{
#if __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1680x1050 (Breitbild)") == -1)
			{
				m_cbRes.InsertString( -1, "1680x1050 (Breitbild)" );
#else // __CURRENT_LANG == LANG_GER
			if(m_cbRes.FindString(0, "1680x1050 (Wide)") == -1)
			{
				m_cbRes.InsertString( -1, "1680x1050 (Wide)" );
#endif // __CURRENT_LANG == LANG_GER
				checkRes[10] = TRUE;
				m_nWideRCount++;
				if(m_bStartFullScreen && m_nResWidth == displayMode.Width && m_nResHeight == displayMode.Height)
					nSelect = m_nNormalRCount + m_nWideRCount - 1;
			}
		}
	}

#if __CURRENT_LANG == LANG_KOR
	m_cbRes.InsertString( -1, "800x600(창모드)" );
	m_cbRes.InsertString( -1, "1024x768(창모드)" );
	m_cbRes.InsertString( -1, "1280x1024(창모드)" );
	m_cbRes.InsertString( -1, "1400x1050(창모드)" );
	m_cbRes.InsertString( -1, "1280x720(W창모드)" );
	m_cbRes.InsertString( -1, "1280x768(W창모드)" );
	m_cbRes.InsertString( -1, "1280x800(W창모드)" );
	m_cbRes.InsertString( -1, "1360x768(W창모드)" );
	m_cbRes.InsertString( -1, "1440x900(W창모드)" );
#else // __CURRENT_LANG == LANG_KOR
	m_cbRes.InsertString( -1, "800x600(Window)" );
	m_cbRes.InsertString( -1, "1024x768(Window)" );
	m_cbRes.InsertString( -1, "1280x1024(Window)" );
	m_cbRes.InsertString( -1, "1400x1050(Window)" );
	m_cbRes.InsertString( -1, "1280x720(W Window)" );
	m_cbRes.InsertString( -1, "1280x768(W Window)" );
	m_cbRes.InsertString( -1, "1280x800(W Window)" );
	m_cbRes.InsertString( -1, "1360x768(W Window)" );
	m_cbRes.InsertString( -1, "1440x900(W Window)" );
#endif // __CURRENT_LANG == LANG_KOR

	if(!m_bStartFullScreen)
	{
		int n = 0;
		switch(m_nResWidth) 
		{
			case 800:
				n = 0;
				break;
			case 1024:
				n = 1;
				break;
			case 1280:
				if(m_nResHeight == 1024)
					n = 2;
				else if(m_nResHeight == 720)
					n = 4;
				else if(m_nResHeight == 768)
					n = 5;
				else if(m_nResHeight == 800)
					n = 6;
				break;
			case 1400:
				n = 3;
				break;
			case 1360:
				n = 7;
				break;
			case 1440:
				n = 8;
				break;
		}
		nSelect = m_nNormalRCount + m_nWideRCount + n;
	}
	pD3D9->Release();

	m_cbRes.SetCurSel( nSelect );

	bool bFont = false;
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	//if (hFont == NULL)
	//	hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	if( hFont )
	{
		LOGFONT lf;
		if( ::GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
		{
			if( m_font.CreateFontIndirect( &lf ) )
				bFont = true;
		}
	}

	struct POSINFO
	{
		UINT nID;
		int x, y;
		int nMsgID;
	};
	

	CSize size( 610, 303 );
			
	CRect rect;
	m_pParentWnd->GetWindowRect( &rect );
	
	SetWindowPos( NULL, 0, 0, size.cx * 2, size.cy * 2, SWP_NOZORDER | SWP_NOMOVE );

	HRGN hRgn = CreateRoundRectRgn( 0, 0, size.cx * 2, size.cy * 2, 20, 20 );
	if( hRgn )
		::SetWindowRgn( m_hWnd, hRgn, FALSE );

	int nX_Gap = 200;
	int nY_Gap = 150;
	int nX_Radio = 75;
	int nX_Radio2 = nX_Radio * 2;
	POSINFO posInfos[] =  
	{
		{IDC_STATIC,       2 + nX_Gap,  				54 + nY_Gap,  IDS_RESOLUTION },
		{IDC_STATIC2,      2 + nX_Gap,  				130 + nY_Gap, IDS_VIEW },
		{IDC_STATIC3,      255 + nX_Gap, 				130 + nY_Gap, IDS_TEXTURE },
		{IDC_STATIC4,	   2 + nX_Gap,  				200 + nY_Gap, IDS_SHADOW },
		{IDC_STATIC5,      255 + nX_Gap, 				200 + nY_Gap, IDS_OBJECTDETAIL },
		{IDC_VIEW_RADIO1,  2 + nX_Gap,  				156 + nY_Gap, IDS_FAR },
		{IDC_VIEW_RADIO2,  2 + nX_Radio + nX_Gap,  	156 + nY_Gap, IDS_MEDIUM },
		{IDC_VIEW_RADIO3,  2 + nX_Radio2 + nX_Gap, 	156 + nY_Gap, IDS_NEAR },
		{IDC_TQ_RADIO1,    255 + nX_Gap,				156 + nY_Gap, IDS_HIGH },
		{IDC_TQ_RADIO2,    255 + nX_Radio + nX_Gap, 	156 + nY_Gap, IDS_MEDIUM },
		{IDC_TQ_RADIO3,    255 + nX_Radio2 + nX_Gap,	156 + nY_Gap, IDS_LOW },
		{IDC_SHD_RADIO1,   2 + nX_Gap,					222 + nY_Gap, IDS_HIGH },
		{IDC_SHD_RADIO2,   2 + nX_Radio + nX_Gap,		222 + nY_Gap, IDS_MEDIUM },
		{IDC_SHD_RADIO3,   2 + nX_Radio2 + nX_Gap,		222 + nY_Gap, IDS_LOW },
		{IDC_OD_RADIO1,    255 + nX_Gap,				222 + nY_Gap, IDS_HIGH },
		{IDC_OD_RADIO2,    255 + nX_Radio + nX_Gap,		222 + nY_Gap, IDS_MEDIUM },
		{IDC_OD_RADIO3,    255 + nX_Radio2 + nX_Gap,	222 + nY_Gap, IDS_LOW },
		{IDC_RES_COMBO,	   70 + nX_Gap,					50 + nY_Gap,  -1 },
		{IDOK,             315 + nX_Gap,				22 + nY_Gap, -1 },
		{IDCANCEL,         315+ nX_Gap,					64 + nY_Gap, -1 }
	};

	CString str;
	int n = sizeof(posInfos) / sizeof(posInfos[0]);
	for( int i=0; i<n; i++)
	{
		if( IDCANCEL == posInfos[i].nID )
			int b = 0;

		CWnd *pWnd = GetDlgItem( posInfos[i].nID );
		if( pWnd == NULL )
			continue;
		pWnd->SetWindowPos( NULL, posInfos[i].x, posInfos[i].y, 0, 0, SWP_NOREDRAW | SWP_NOZORDER | SWP_NOSIZE );
		if( posInfos[i].nMsgID != -1 )
		{
			str.LoadString(posInfos[i].nMsgID);
			SetDlgItemText( posInfos[i].nID, str );
		}

		if( bFont )
		{
			pWnd->SetFont( &m_font, FALSE );
		}
	}
	
	UpdateData(FALSE);		// 변수에서 컨트롤로.
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnOK() 
{
	int nCurSel = m_cbRes.GetCurSel();

	CString text1, text2;
	BOOL bWide;
	m_cbRes.GetWindowText(text1);
	m_cbRes.GetWindowText(text2);
	int nNum = text1.Find("x", 0);
	int nSize = text1.GetLength();
	text1.Delete(nNum, nSize-nNum);

	if(nCurSel < m_nNormalRCount + m_nWideRCount)
	{
		int nWidth = atoi(text1);
		switch(nWidth) 
		{
			case 800:
				m_nResWidth = 800;	
				m_nResHeight = 600;
				break;
			case 1024:
				m_nResWidth = 1024;	
				m_nResHeight = 768;
				break;
			case 1280:
				{
					CString temp;
					m_cbRes.GetWindowText(temp);
					int num = temp.Find("Wide", 0);
					if(num > -1)
					{
						num = temp.Find("720", 0);
						if(num > -1)
						{
							m_nResWidth = 1280;	
							m_nResHeight = 720;
						}
						else
						{
							num = temp.Find("768", 0);
							if(num > -1)
							{
								m_nResWidth = 1280;	
								m_nResHeight = 768;
							}
							else
							{
								m_nResWidth = 1280;	
								m_nResHeight = 800;
							}
						}
					}
					else
					{
						m_nResWidth = 1280;	
						m_nResHeight = 1024;
					}
				}
				break;
			case 1400:
				m_nResWidth = 1400;	
				m_nResHeight = 1050;
				break;
			case 1600:
				m_nResWidth = 1600;	
				m_nResHeight = 1200;
				break;
			case 1360:
				m_nResWidth = 1360;	
				m_nResHeight = 768;
				break;
			case 1440:
				m_nResWidth = 1440;	
				m_nResHeight = 900;
				break;
			case 1680:
				m_nResWidth = 1680;	
				m_nResHeight = 1050;
				break;
		}
		m_bStartFullScreen = TRUE;
	}
	else if(nCurSel >= m_nNormalRCount + m_nWideRCount) //Windowed Mode
	{
		int nWidth = atoi(text1);
		if(nWidth == 1280)
		{
			text2.Delete(0, nNum+1);
			int nNum2 = text2.Find("(", 0);
			nSize = text2.GetLength();
			text2.Delete(nNum2, nSize-nNum2);
			if(atoi(text2) == 1024)
				bWide = FALSE;
			else
				bWide = TRUE;
		}

		switch(nWidth) 
		{
			case 800:
				m_nResWidth = 800;	
				m_nResHeight = 600;
				break;
			case 1024:
				m_nResWidth = 1024;	
				m_nResHeight = 768;
				break;
			case 1280:
				if(!bWide)
				{
					m_nResWidth = 1280;	
					m_nResHeight = 1024;
				}
				else
				{
					int nNum = text2.Find("720", 0);
					if(nNum > -1)
					{
						m_nResWidth = 1280;	
						m_nResHeight = 720;
					}
					else
					{
						nNum = text2.Find("768", 0);
						if(nNum > -1)
						{
							m_nResWidth = 1280;	
							m_nResHeight = 768;
						}
						else
						{
							m_nResWidth = 1280;	
							m_nResHeight = 800;
						}
					}
				}
				break;
			case 1360:
				m_nResWidth = 1360;	
				m_nResHeight = 768;
				break;
			case 1400:
				m_nResWidth = 1400;	
				m_nResHeight = 1050;
				break;
			case 1440:
				m_nResWidth = 1440;	
				m_nResHeight = 900;
				break;
		}
		m_bStartFullScreen = FALSE;
	}

	UpdateData(TRUE);	// 컨트롤에서 변수.

	CDialog::OnOK();

	if( m_pParentWnd )
	{
		((CDialogOptionFrame*)m_pParentWnd)->OK_byChild( );
		::SendMessage( m_pParentWnd->GetSafeHwnd(), WM_CLOSE, 0, 0 );
	}
}

void CDlgOption::LoadSkin()
{
	m_pic.Load( IDC_BITMAP_OPTION );
	CSize size = m_pic.GetImageSize();
	SetWindowPos( NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOMOVE );

	HRGN hRgn = CreateRoundRectRgn( 0, 0, size.cx, size.cy, 18, 18 );
	::SetWindowRgn( m_hWnd, hRgn, FALSE );
}

//BOOL CDlgOption::OnEraseBkgnd(CDC* pDC) 
//{
	//m_pic.Render( pDC );
//	return TRUE;
//}

HBRUSH CDlgOption::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	switch( pWnd->GetDlgCtrlID() )
	{
	 	case IDC_VIEW_RADIO1:
	 	case IDC_VIEW_RADIO2:
	 	case IDC_VIEW_RADIO3:
	 	case IDC_TQ_RADIO1:
	 	case IDC_TQ_RADIO2:
	 	case IDC_TQ_RADIO3:
	 	case IDC_OD_RADIO1:
	 	case IDC_OD_RADIO2:
	 	case IDC_OD_RADIO3:
	 	case IDC_SHD_RADIO1:
	 	case IDC_SHD_RADIO2:
	 	case IDC_SHD_RADIO3:
		{
			return (HBRUSH)GetStockObject(BLACK_BRUSH);
		//	return (HBRUSH)GetStockObject(WHITE_BRUSH);
		}
		break;
	}

	if( nCtlColor == CTLCOLOR_DLG )
		return m_brush;

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
//		pDC->SetBkColor( RGB( 0, 0, 0 ));
		pDC->SetTextColor( RGB( 255, 255, 255 ));
		return m_brush;
		//HBITMAP hPic = m_pic.GetBitmap();
		//if( hPic )
		//{
		//	CRect rtStatic;
		//	CRect rtBitmap;
		//	
		//	pWnd->GetClientRect( rtStatic );		
		//	pWnd->GetWindowRect( rtBitmap );
		//	ScreenToClient( rtBitmap );

		//	HDC hMemdc=CreateCompatibleDC( pDC->m_hDC );
		//	HBITMAP hOldBmp=(HBITMAP)SelectObject(hMemdc, hPic );
		//	BitBlt( pDC->m_hDC, 0, 0, rtStatic.Width(), rtStatic.Height(), hMemdc, rtBitmap.left, rtBitmap.top, SRCCOPY );
		//	SelectObject(hMemdc,hOldBmp);
		//	DeleteDC(hMemdc);

		//	return (HBRUSH)GetStockObject(NULL_BRUSH);
		//}
	}
	else 
		return m_brush;

	return hbr;
}

void CDlgOption::LoadButton()
{
	m_BtnOK.SetBitmaps(IDB_BITMAP_OK00, RGB(255, 0, 255), IDB_BITMAP_OK01, RGB(255, 0, 255) );
	m_BtnOK.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_BtnOK.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_BtnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_BtnOK.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_BtnOK.SizeToContent();
	m_BtnOK.DrawTransparent(TRUE);
	m_BtnOK.DrawBorder(FALSE, FALSE);
	m_BtnOK.SetBtnCursor(IDC_CURSOR1);
	
	m_BtnCANCEL.SetBitmaps(IDB_BITMAP_CANCEL00, RGB(255, 0, 255), IDB_BITMAP_CANCEL01, RGB(255, 0, 255) );
	m_BtnCANCEL.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_BtnCANCEL.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_BtnCANCEL.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_BtnCANCEL.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_BtnCANCEL.SizeToContent();
	m_BtnCANCEL.DrawTransparent(TRUE);
	m_BtnCANCEL.DrawBorder(FALSE, FALSE);
	m_BtnCANCEL.SetBtnCursor(IDC_CURSOR1);
}

void CDlgOption::OnCancel() 
{
	CDialog::OnCancel();

	if( m_pParentWnd )
		::SendMessage( m_pParentWnd->GetSafeHwnd(), WM_CLOSE, 0, 0 );
}

void CDlgOption::ShowDialog()
{
	Create(m_lpszTemplateName, m_pParentWnd);
	ShowWindow(SW_SHOW);
}
void CDlgOption::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgOption::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnClose();
}
