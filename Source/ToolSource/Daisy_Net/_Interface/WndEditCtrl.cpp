// ImeView.cpp : implementation file
//

#include "stdafx.h"
#include "AppDefine.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "clipboard.h"
#include <malloc.h>

#if !defined(_MULTI_LANGUAGE_)
#define EDIT_COLOR 0xff000000
//{{AFX
///////////////////////////////////////////////////////////////////
// class CWndCandList
// Remark:
//   일본어나 중국어는 입력 이후 유사어 리스트를 통해 단어를 고른다.
//   CWndCandList는 그 유사어를 선택할 수 있게하는 팝업 윈도다.
//
///////////////////////////////////////////////////////////////////
CWndCandList::CWndCandList() 
{ 
	m_pWndEdit = NULL;
	m_lpCandList = NULL;
	m_nScrollPos = 0;
	m_nStartIndex = 0;
	m_bReadyCandList = FALSE;
#ifdef __V050823_JAPAN_ATOK
	m_ptWindowPos = 0;
#endif
} 
CWndCandList::~CWndCandList() 
{ 
} 
void CWndCandList::SetCandList( LPCANDIDATELIST lpCandList )
{
	m_lpCandList = lpCandList;

//HDC         hdc;            // Storage for device context handle.
//DWORD       dwTextColor;
//RECT        rect;           // Storage for client rect.
//DWORD       dwBackColor;    // Storage for background color value

    LPSTR       lpStr;          // Storage for LP to a string.
    DWORD       dwNumPerPage;   // Storage for num per page
    DWORD       dwStartIndex;   // Storage for candidate index
    DWORD       dwEndIndex;     // Storage for candidate index
    int         i;				// y, i
	char		buf[255];

    DWORD dwSize = lpCandList->dwSize;
    DWORD dwStyle = lpCandList->dwStyle;
    DWORD dwCount = lpCandList->dwCount;
    DWORD dwSelection = lpCandList->dwSelection;
    DWORD dwPageStart = lpCandList->dwPageStart;
    DWORD dwPageSize = lpCandList->dwPageSize;
	TRACE( "Candlist ----------------------\n" );
	TRACE( "Count %d \n", dwCount );
	TRACE( "Selection %d \n", dwSelection );
	TRACE( "PageStart %d \n", dwPageStart );
	TRACE( "PageSize %d \n", dwPageSize );

    dwNumPerPage = (!m_lpCandList->dwPageSize ) ? DEFAULT_CAND_NUM_PER_PAGE : m_lpCandList->dwPageSize;
    dwStartIndex = m_lpCandList->dwPageStart;
	
    dwEndIndex = dwStartIndex + dwNumPerPage;
    dwEndIndex = ( dwEndIndex >= m_lpCandList->dwCount ) ? m_lpCandList->dwCount : dwEndIndex;
	dwEndIndex = m_lpCandList->dwCount;
    
	int m_charHeight = 16;
    //rect.bottom = rect.top + m_charHeight;

	int nDiv = m_lpCandList->dwSelection / 9;

	if( m_nStartIndex != m_lpCandList->dwSelection / 9 )
	{
		m_nStartIndex = m_lpCandList->dwSelection / 9; 
		ImmNotifyIME( ((CWndEdit*)m_pWndEdit)->m_hIMC,NI_SETCANDIDATE_PAGESTART, 0, nDiv * 9 );//m_lpCandList->dwSelection );
		// ImmNotifeIME를 하게 되면 다시 이 함수를 부른다. 그렇게 되면 위에 ResetContent를 실행하게 되므로
		// 스크롤 포지션이 다시 리셋된다. 이 부분이 맨 마지막에 실행되므로 여기서 스크롤 포지션을 세팅해야한다.
		m_wndScrollBar.SetScrollPos( nDiv * 9 );//m_lpCandList->dwSelection );
		m_nScrollPos = nDiv * 9;
		//m_wndScrollBar.SetScrollRange( 0, dwCount + 20 );		
		return;
	}
	// 리셋을 하면 스크롤 포지션이 초기화 된다. 
	// 스크롤 바를 눌러서 스크롤 위치가 조절된 상태라면 초기화가 되면
	// 안되기 때문에 현재 스크롤 포지션을 저장해 둔다.
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	ResetContent();

	for( i = 0; i < dwCount; i++ ) 
	{
        lpStr = (LPSTR) m_lpCandList + m_lpCandList->dwOffset[ i ];
		wsprintf(buf, "  %s",  lpStr);
		AddString( buf );
    }
	// 9개의 배수로 갯수를 채운다.
	int nDiv2 = 9 - ( dwCount % 9 );
	if( nDiv2 < 9 )
	{
		for( i = 0; i < nDiv2; i++ )
			AddString( "" );
	}
	// 스크롤 포지션을 복원한다.
	m_wndScrollBar.SetScrollPos( nScrollPos );//m_lpCandList->dwSelection );
	SetCurSel( m_lpCandList->dwSelection );
}
void CWndCandList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE )
		Destroy();
}
void CWndCandList::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndListBox::OnDraw( p2DRender );
    LPSTR       lpStr = "";          // Storage for LP to a string.
	char		buf[255];
	int m_charHeight = 15;

    for ( int y = 0 ; y < 9; y++ ) 
	{
		wsprintf(buf, "%d:", y + 1, lpStr);
		p2DRender->TextOut( 2, 4 + y * m_charHeight, buf, 0xff000000 );//, lstrlen( buf ) );
    }
	if( m_nScrollPos != m_wndScrollBar.GetScrollPos() )
	{
		m_nScrollPos = m_wndScrollBar.GetScrollPos();
		ImmNotifyIME( ((CWndEdit*)m_pWndEdit)->m_hIMC,NI_SETCANDIDATE_PAGESTART, 0, m_nScrollPos );
	}
//	GetParentWnd()->SetFocus();
} 
void CWndCandList::OnInitialUpdate() 
{ 
	CWndListBox::OnInitialUpdate(); 

	//CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );

//	int nCount = 0, nIndex, nNum = 0;
	int nCount = 0, nNum = 0;

	m_wndScrollBar.SetScrollPage( 9 );

	if( nNum > 20 ) nNum = 20;
	if( nNum < 5 ) nNum = 5;
	SetWndRect( CRect( 0, 0, 150, nNum * ( GetFontHeight() + 3 ) + 8 ) );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	//Move( point );
	//MoveParentCenter();

	//m_pWndEdit->SetFocus();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCandList::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 0, 0, 150, 200 );
	SetTitle( _T( "World" ) );
	//CWndListBox::Create( 0, rect, pWndParent, APP_COMMAND );
	CWndBase::Create( WBS_TOPMOST, rect, pWndParent, APP_COMMAND );
//( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return TRUE;//CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMMAND, WBS_TOPMOST, CPoint( 0, 0 ), pWndParent );
} 
void CWndCandList::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CWndListBox::OnLButtonUp( nFlags, point );
	CWndEdit* pWndEdit = (CWndEdit*)m_pParentWnd;
	// 클릭했을 경우 리스트박스의 현재 셀렉트된 것을 Ime에게 선택했다고 알린다.
	ImmNotifyIME( pWndEdit->m_hIMC,NI_SELECTCANDIDATESTR,0,GetCurSel());
	// 선택 이후 유사어 리스트를 닫으라고 Ime알린다.
	ImmNotifyIME( pWndEdit->m_hIMC,NI_CLOSECANDIDATE,0,0);
} 
void CWndCandList::OnKillFocus(CWndBase* pNewWnd)
{
	// 마우스 커서로 다른 창을 클릭하거나 포커스를 변경했을 경우 단어 조합을 완료한다.
	// 완료 조건은 pNewWnd가 NULL이 아니어야한다. (NULL일 때는 창이 파괴될 때 외에는 없다.)
	// 즉, 포커스가 다른 창으로 변경될 때만 조합을 완결하는 것이다.
	// 그리고 현재창의 프레임윈도와 새윈도의 프레임윈도가 다르면 포커스가 이전된 것으로 간주한다.
	if( pNewWnd && pNewWnd != m_pParentWnd->GetParentWnd() )
	{
		// 단어 조합을 완료하라는 매시지를 Ime에게 보낸다.
		// 보통 완료는 유사어 창이 열려 있으면 닫으면서 완료하고 열리 있지 않으면 그냥 완료한다.
		ImmNotifyIME( ((CWndEdit*)m_pWndEdit)->m_hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );
	}
}

#define EDIT_COLOR 0xff000000

HCURSOR CWndEdit::m_hEditCursor = NULL;

///////////////////////////////////////////////////////////////////////////////
// CWndEdit

CWndEdit* CWndEdit::m_pActWndEditCtrl = NULL;

HIMC CWndEdit::m_hIMCBackup = NULL;
HIMC CWndEdit::m_hIMC = NULL;
HWND CWndEdit::m_hWnd = NULL;
HKL  CWndEdit::m_hKeyLayout = 0;

CWndEdit::CWndEdit()
{
	m_nState = 0;   // Current Ime state.
	m_nCompLen = 0;   // To save previous composition string length.
	m_property = 0;
	m_hIMC = NULL;
	memset(m_hwndCand, 0, sizeof(HWND) * MAX_LISTCAND);
	memset(m_CandList, 0, sizeof(LPCANDIDATELIST) * MAX_LISTCAND);

	m_pFont = NULL;
	m_pOldWndEditCtrl = NULL;

	m_bNativeMode = TRUE;
	m_bEnableModeChange = FALSE; // 한영 전환 가능한가?

	// override
	m_bCaretVisible = TRUE;
	m_byWndType = WTYPE_EDITCTRL;
	
	m_bEnableClipboard = TRUE;//FALSE;
	
	memset( m_TextComp, 0, sizeof(char) * 3);
	m_pWndCandList = NULL;
	m_bNativeMode = FALSE;
}

CWndEdit::~CWndEdit()
{
	if(m_pActWndEditCtrl == this)
		m_pActWndEditCtrl = NULL;
	SAFE_DELETE( m_pWndCandList );

	//if(m_hIMCBackup)
		//::ImmAssociateContext(m_hWnd, m_hIMCBackup);
}
// 캐럿을 그린다. 국가마다 캐럿모양이 다를 수 있다. 한글은 박스 형태의 캐럿모양이다.
void CWndEdit::DrawCaret(C2DRender* p2DRender)
{
	if( m_bCaretVisible )
	{
		CPoint pt1, pt2;
		int nFontBottom = m_pFont->GetMaxHeight();// - 1;
		pt1 = CPoint( m_ptCaret.x, m_ptCaret.y - 1 );
		pt2 = CPoint( m_ptCaret.x, m_ptCaret.y + nFontBottom - 2 );
		int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() * ( p2DRender->m_pFont->GetMaxHeight() + m_nLineSpace ) : 0;
		pt1.y -= nPos;
		pt2.y -= nPos;
		if( m_timerCaret.GetLeftTime() < 500 )
		{
			//int nLength = strlen( m_TextComp );
			if( PRIMARYLANGID( m_hKeyLayout ) == LANG_KOREAN && m_TextComp[ 0 ] ) 
			{
				CSize size = m_pFont->GetTextExtent( m_TextComp );
				p2DRender->RenderFillRect( CRect( pt1.x, pt1.y, pt1.x + size.cx, pt1.y + size.cy ), D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
				p2DRender->TextOut( m_ptCaret.x, m_ptCaret.y, m_TextComp, m_dwColor );
			}
			else
			{
				p2DRender->RenderFillRect( CRect( pt1.x, pt1.y, pt1.x + 2, pt1.y + nFontBottom ), m_dwColor );
			}
		}
		if( m_timerCaret.IsTimeOut() )
			m_timerCaret.Reset();
	}
}
// 마우스 왼쪽 버튼을 에디트 창에 누르면 조합이 완료된다.
// 이는 조합시 다른 오동작을 방지하기 위해 클릭하면 무조건 조합을 완료하게 한 것이다.
void CWndEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
	// 조합 완료 매지시를 ime에게 보낸다.
	ImmNotifyIME(m_hIMC,NI_COMPOSITIONSTR,CPS_COMPLETE,0);
	CWndText::OnLButtonDown( nFlags, point );
}
// 알파벳 상태로 변경한다. 
void CWndEdit::SetAlphaNumeric()
{
	if(!(m_hIMC = ::ImmGetContext(m_hWnd)))	return;
	//::ImmSetConversionStatus(m_hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);

	//DWORD dwConv; // , dwSent;
    //DWORD dwTemp;
	//::ImmGetConversionStatus(m_hIMC, &dwConv, &dwSent );
	//dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	//dwTemp |= IME_CMODE_ALPHANUMERIC;
	//::ImmSetConversionStatus(m_hIMC, dwConv, IME_SMODE_NONE );//dwSent );
	//::ImmSetConversionStatus(m_hIMC, IME_CMODE_ALPHANUMERIC, dwSent );//IME_SMODE_NONE );//IME_SMODE_NONE);
	ImmSetOpenStatus(m_hIMC,FALSE );//TRUE);	
	
	::ImmReleaseContext(m_hWnd, m_hIMC);

}

// 현재 기본으로 세팅된 국가 언어 상태로 변경한다.
void CWndEdit::SetNative()
{
	if(!(m_hIMC = ::ImmGetContext(m_hWnd)))	return;
	//::ImmSetConversionStatus(m_hIMC, IME_CMODE_NATIVE, IME_SMODE_NONE);
	DWORD dwConv, dwSent;
    DWORD dwTemp;
	::ImmGetConversionStatus(m_hIMC, &dwConv, &dwSent );
	
	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	dwTemp |= IME_CMODE_NATIVE | IME_CMODE_KATAKANA;
	//::ImmetConversionStatus(hIMC,&dwConv,&dwSent);
	::ImmSetConversionStatus(m_hIMC, dwConv, dwSent );
    //BOOL fOpen = ImmGetOpenStatus(m_hIMC);
    //ImmSetOpenStatus(m_hIMC,!fOpen);
	ImmSetOpenStatus(m_hIMC,TRUE);
	::ImmReleaseContext(m_hWnd, m_hIMC);
}
// 알파벳 모드로 변경한다. 모드 플렉이 있으므로 상태를 알수 있다.
// 모드를 알고 있어야 ime이 리셋후에 다시 모드를 복원할 수 있다.
void CWndEdit::SetAlphaNumericMode()
{
	m_bNativeMode = FALSE;
	SetAlphaNumeric();
}
// 국가 모드로 변경한다. 모드 플렉이 있으므로 상태를 알수 있다.
// 모드를 알고 있어야 ime이 리셋후에 다시 모드를 복원할 수 있다.
void CWndEdit::SetNativeMode()
{
	m_bNativeMode = TRUE;
	SetNative();
}
// ime 언어 변경 여부를 세팅한다.
// 특정 언어로 유지하게 하는 기능을 한다.
// 영문에서 한글로 변경하지 못하게 한다던가..
void CWndEdit::EnableModeChange( BOOL bEnable )
{
	if( bEnable )
	{
		if( m_hIMCBackup )
		{
			::ImmAssociateContext( m_hWnd, m_hIMCBackup );
			m_hIMCBackup = NULL;
		}
	}
	else
	{
		if( m_hIMCBackup == NULL )
		{
			m_hIMCBackup = ::ImmAssociateContext( m_hWnd, NULL );
		}
	}
	m_bEnableModeChange = bEnable;

}
// 창을 생성한다.
BOOL CWndEdit::Create(HWND hwnd,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	Init( hwnd, 0, 0 );
	if( dwStyle & EBS_PASSWORD )
	{
		SetAlphaNumericMode();
		EnableModeChange( FALSE );
	}
	else
	{
		SetAlphaNumericMode();
		EnableModeChange( TRUE );
	}
#ifdef __DAISY
	//dwStyle = dwStyle & ~WBS_NODRAWFRAME;
#endif
	BOOL b=  CWndBase::Create(dwStyle|WBS_CHILD,rect,pParentWnd,nID);
	return b;
}

// Ime초기화
void CWndEdit::Init(HWND hwnd,int x,int y)
{
	InitIme(hwnd,x+2,y+3);
}
// Ime초기화
void CWndEdit::InitIme(HWND hwnd,int x,int y)
{
	char ga[] = "가";
	SIZE size;
	HFONT hFont;
	LOGFONT lf;
	if(hwnd)
		m_hWnd = hwnd;
	hFont = (HFONT)::SendMessage(m_hWnd,WM_GETFONT, 0, 0);
	if (hFont) 
	{
		if (GetObject(hFont, sizeof(LOGFONT), &lf)) 
		{
			SetCompositionFont(&lf);
		}
	}

	//m_hKeyWnd = hwnd;
	m_hKeyLayout = GetKeyboardLayout(0);	// 0 means Current thread
	m_property = ImmGetProperty(m_hKeyLayout, IGP_PROPERTY);
	for (int i = 0; i < MAX_LISTCAND; i++) 
	{
		m_hwndCand[i] = NULL;
		m_CandList[i] = NULL;
	}

	HDC hDC = ::GetDC(m_hWnd);
	HFONT hScrFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hScrFont);
	GetTextExtentPoint(hDC, (LPSTR)&ga, 2, &size);
	SelectObject(hDC, hOldFont);
    ::ReleaseDC(m_hWnd, hDC);


//	HIMC hIMC = ::ImmAssociateContext(m_hWnd, NULL);
	if(!(m_hIMC = ::ImmGetContext(m_hWnd)))
		return;
	DWORD dwConv, dwSent;
    DWORD dwTemp;
	::ImmGetConversionStatus(m_hIMC, &dwConv, &dwSent );
	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	dwTemp |= IME_CMODE_NATIVE;
	//dwTemp |= IME_CMODE_NATIVE | IME_CMODE_KATAKANA );
	//::ImmetConversionStatus(hIMC,&dwConv,&dwSent);
	::ImmSetConversionStatus(m_hIMC, dwConv, dwSent );
	ImmSetOpenStatus(m_hIMC,TRUE);	
	//::ImmSetConversionStatus(m_hIMC, IME_CMODE_NATIVE, IME_SMODE_NONE);
/*
	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	dwTemp |= ( IME_CMODE_NATIVE | IME_CMODE_KATAKANA );
	//::ImmetConversionStatus(hIMC,&dwConv,&dwSent);
	::ImmSetConversionStatus(m_hIMC, dwConv, dwSent );
    //BOOL fOpen = ImmGetOpenStatus(m_hIMC);
    //ImmSetOpenStatus(m_hIMC,!fOpen);
	ImmSetOpenStatus(m_hIMC,TRUE);
*/
	::ImmReleaseContext(m_hWnd, m_hIMC);
	//::ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
	//::ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
	//::ImmReleaseContext(g_ImeInput.m_hWnd, hIMC);
}

BOOL CWndEdit::IsProcessEvent()
{
	if (m_nState & IME_IN_CHOSECAND ||
			(m_nState & IME_IN_COMPOSITION && GetCompCursorPos()) ) {
		return FALSE;
	} else
		return TRUE;
}

// Set IME open state 
void CWndEdit::SetState(BOOL fOpen)
{
	if (Enter()) {
		ImmSetOpenStatus(m_hIMC, fOpen);
		Leave();
	}
}

CWndEdit::SetCompositionFont(LPLOGFONT lf)
{
	BOOL ret;
	if (Enter()) {
		ret = ImmSetCompositionFont(m_hIMC, lf);
		Leave();
	} else
		ret = FALSE;
	return ret;
}

void CWndEdit::SetCompWndPos(POINT& pt)
{
	if (m_property & IME_PROP_SPECIAL_UI || m_property & IME_PROP_AT_CARET)
		return;
	// at_near

	if (Enter()) {
		COMPOSITIONFORM cf;

		cf.dwStyle = CFS_POINT;
		cf.ptCurrentPos = pt;
		ImmSetCompositionWindow(m_hIMC, &cf);
		Leave();
	}
}

int CWndEdit::GetCompCursorPos()
{
	if (!Enter())
		return 0;
    int ret = LOWORD(ImmGetCompositionString(m_hIMC, GCS_CURSORPOS, NULL, 0));
	Leave();
	return ret;
}

void CWndEdit::ClearData()
{
    for(int i = 0; i < MAX_LISTCAND; i++) 
	{
        if (m_hwndCand[i]) 
		{
            ::DestroyWindow(m_hwndCand[i]);
			m_hwndCand[i] = NULL;
            delete m_CandList[i];
			m_CandList[i] = NULL;
        }
    }
    m_nCompLen = m_nState = 0;
	//m_pPullItem = NULL;
}
BOOL CWndEdit::GetResultString()
{
    DWORD       len;					// Storage for length of result str.
    LPSTR       str;					// Pointer to result string.

	if (Enter()) 
	{
		if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0) 
		{
			str = new char[len + 1];
			ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, str, len);
			str[len] = 0;
			ProcessResultString( str );
			delete str;
		}
		Leave();
	}
	return TRUE;
}

BOOL CWndEdit::GetCompString(LONG flag)
{
    DWORD      len;              // Stogare for len. of composition str
    LPSTR      str;              // Pointer to composition str.
    LPSTR      strAttr;          // Pointer to composition str array.
    DWORD      lenAttr;
   
	if (!Enter())	
		return FALSE;
	if ((len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0)) >= 0)
	{
		str = new char[len+1];
		ImmGetCompositionString(m_hIMC, GCS_COMPSTR, str, len);
		str[len] = 0;
		strAttr = NULL;
		if (flag & GCS_COMPATTR) 
		{
			if ((lenAttr = ImmGetCompositionString(m_hIMC, GCS_COMPATTR, NULL, 0)) > 0) 
			{
				strAttr = new char[lenAttr+1];
				ImmGetCompositionString(m_hIMC, GCS_COMPATTR, strAttr, lenAttr);
				strAttr[lenAttr] = 0;
			}
		}
		// Override function
		ProcessCompString(str, strAttr);
		m_nCompLen = (UINT)len;
		if (strAttr)
			delete strAttr;
		if (str)
			delete str;
	}
	Leave();
	return TRUE;
}

///////////////////////////////////////////////////////////////
// for IME Control Message

LONG CWndEdit::OnInputLangChange(WPARAM dwCommand, LPARAM dwData)
{
    if (ImmIsIME(m_hKeyLayout) && m_property & IME_PROP_AT_CARET)
		ClearData();

    // Set new keyboard layout.
	InitIme(NULL,0,0);

	if(Enter()) 
	{
		for (int i = 0; i < MAX_LISTCAND; i++) 
		{
			CANDIDATEFORM cf;

			if (m_property & IME_PROP_AT_CARET) 
			{
	/*
				// This application do not want to set candidate window to
				// any position. Anyway, if an application need to set the
				// candiadet position, it should remove the if 0 code
				// the position you want to set
				cf.dwIndex = i;
				cf.dwStyle = CFS_CANDIDATEPOS;
				cf.ptCurrentPos.x = ptAppWantPosition[i].x;
				cf.ptCurrentPos.y = ptAppWantPosition[i].y;
				ImmSetCandidateWindow(m_hIMC, &CandForm );
	*/
			} 
			else 
			{
				if (!ImmGetCandidateWindow(m_hIMC, i, &cf))
					continue;
				if (cf.dwStyle == CFS_DEFAULT)
					continue;
				cf.dwStyle = CFS_DEFAULT;
				ImmSetCandidateWindow(m_hIMC, &cf);
			}
		}
		Leave();
	}
	return ::DefWindowProc(m_hWnd,WM_INPUTLANGCHANGE, dwCommand, dwData);
}

LONG CWndEdit::OnIMESetContext(WPARAM dwCommand, LPARAM dwData)
{
	if (m_property & IME_PROP_AT_CARET) {
		// application wants to draw UI by itself.
		dwData &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
	}
	return ::DefWindowProc(m_hWnd,WM_IME_SETCONTEXT, dwCommand, dwData);
}

LONG CWndEdit::OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		m_nCompLen = 0; // length of composition string.
		m_nState |= IME_IN_COMPOSITION;
		IMEStartComposition(dwData);
		return TRUE;
	} else
		return FALSE;
}

LONG CWndEdit::OnIMEComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) 
	{ // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		if (dwData & GCS_RESULTSTR)
			GetResultString(); // Result String
		//else //
		if (dwData & GCS_COMPSTR)
			GetCompString(dwData); // Composition string
		return 0;
	}
	return 0;
}

LONG CWndEdit::OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData)
{
	m_TextComp[0]='\0';

	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		m_nCompLen = 0;
		m_nState &= ~IME_IN_COMPOSITION;
		IMEEndComposition(dwData);
		return TRUE;
	} else
		return FALSE;
}

LONG CWndEdit::OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData)
{
	// Make sure the size for drawing the composition string.
	// Application should draw the composition string correctly.
	return 0;
}

LONG CWndEdit::OnIMENotify(WPARAM dwCommand, LPARAM dwData)
{
	// This class does not handle all notification message.
	// So we pass those notification messages which are not hanlded
	// by this application to the DefWindowProc.

    switch (dwCommand) {
        case IMN_OPENCANDIDATE:
 			return OpenCandidate(dwData);

        case IMN_CLOSECANDIDATE:
			return CloseCandidate(dwData);

        case IMN_CHANGECANDIDATE:
			return ChangeCandidate(dwData);

        case IMN_SETOPENSTATUS:
			return SetOpenStatus();
    }
    return ::DefWindowProc(m_hWnd,WM_IME_NOTIFY, dwCommand, dwData);
}

LONG CWndEdit::OnIMEControl(WPARAM dwCommand, LPARAM dwData)
{
	// This message is not received by the application window.
	// But don't pass it to DefWindowProc().
	return 0;
}
/////////////////////////////////////////////////////////////////
// for Candidate window

// IMN_OPENCANDIDATE:
BOOL CWndEdit::OpenCandidate(LONG lParam)
{
    LPCANDIDATELIST lpCandList = NULL;         // Storage for LP to candidate list.
    DWORD       dwBufLen;               // Storage for candidate strings.
    LPSTR       lpStr;                  // Storage for LP to a string.
    int         max_width = 0;          // Storage for width of listCand
    int         CurNumCandList = 0;     // Storage for number of cand. lists.
    DWORD       dwPreferNumPerPage;     // Storage for PreferNumPerPage

	if( lParam != 1 )
		return FALSE;

    if (!Enter())
        return FALSE;
	
	//CWndCandList* pWndCandList;
	m_pWndCandList = new CWndCandList;
	m_pWndCandList->m_pWndEdit = this;
	m_pWndCandList->Initialize( this );//GetParentWnd() );
	m_pWndCandList->m_bReadyCandList = TRUE;

	dwBufLen = ImmGetCandidateList(m_hIMC, 0, lpCandList, 0 );
	if( ( m_CandList[ 0 ] = (LPCANDIDATELIST) new char[ dwBufLen ] ) )
	{
		lpCandList = m_CandList[0];
		ImmGetCandidateList(m_hIMC, 0, lpCandList, dwBufLen);
		
		dwPreferNumPerPage = ( !lpCandList->dwPageSize ) ?
			DEFAULT_CAND_NUM_PER_PAGE :	lpCandList->dwPageSize;
		
		// get the longest string length
		for (int i = 0; i < (int)lpCandList->dwCount; i++ ) 
		{
			lpStr = (LPSTR)lpCandList + lpCandList->dwOffset[i];
			CSize size = m_pWndCandList->GetFont()->GetTextExtent( lpStr );
	
			max_width = (max_width < size.cx ) ? size.cx : max_width;
		}
		CRect rectScreen = GetScreenRect();
		CRect rect( 0, 0, max_width + 40, dwPreferNumPerPage * 16 );
		rect.OffsetRect( rectScreen.left, rectScreen.top - rect.Height() );
		rect += m_ptConverted;
		m_pWndCandList->SetWndRect( rect );
		m_pWndCandList->SetCandList( lpCandList );
	}
    m_nState |= IME_IN_CHOSECAND;
// exit_opencand:
    Leave();
	return TRUE;
}
// IMN_CLOSECANDIDATE:
BOOL CWndEdit::CloseCandidate(LONG CandList)
{
	if( m_pWndCandList )
		m_pWndCandList->Destroy();
	delete m_CandList[0];
	m_CandList[0] = NULL;
    m_nState &= ~IME_IN_CHOSECAND;
	return TRUE;
}

// IMN_CHANGECANDIDATE:
BOOL CWndEdit::ChangeCandidate(LONG CandList)
{
    LPCANDIDATELIST lpCandList = NULL;         
    DWORD           dwIndex;
    DWORD 			dwBufLen;
    LPSTR			lpStr;
    DWORD			i = 1;
//    RECT			rect;
    int				max_width = 0;
    DWORD			dwPreferNumPerPage;

	if (!Enter())
		return FALSE;

	dwIndex = 0;
    if (!(dwBufLen = ImmGetCandidateList(m_hIMC, dwIndex, lpCandList, 0))) 
	{
		goto exit_changecand;
	}
	delete m_CandList[dwIndex];
	m_CandList[dwIndex] = lpCandList = (LPCANDIDATELIST)new char[dwBufLen];
	
    ImmGetCandidateList(m_hIMC, dwIndex, lpCandList, dwBufLen);
    dwPreferNumPerPage = (!lpCandList->dwPageSize ) ?
		DEFAULT_CAND_NUM_PER_PAGE : lpCandList->dwPageSize;
	
    for( i = 0; i < lpCandList->dwCount; i++ ) 
	{
        lpStr = (LPSTR)lpCandList + lpCandList->dwOffset[i];
        max_width = (max_width < lstrlen(lpStr)) ? lstrlen(lpStr) : max_width;
    }
	m_pWndCandList->SetCandList( lpCandList );

exit_changecand:
	Leave();
	return TRUE;
}

// IMN_SETOPENSTATUS:
BOOL CWndEdit::SetOpenStatus()
{
    if (!Enter())
        return FALSE;

//	if( IsEnableModeChange() )
	{
		if ( ImmGetOpenStatus(m_hIMC)) 
		{
		  // the IME conversion engine is open
				m_bNativeMode = TRUE;
		}
		else 
		{
			// Here we close and destroy all of candidate windows
			// if IME conversion engine is closed.
			m_bNativeMode = FALSE;
			for (int i = 0; i < MAX_LISTCAND; i++ ) 
			{
				if (m_CandList[i]) 
				{
					::DestroyWindow(m_hwndCand[i]);
					m_hwndCand[i] = NULL;
					delete m_CandList[i];
					m_CandList[i] = NULL;
				}
			}
		m_nCompLen = 0;
		m_nState = 0;
	  }
	}
	Leave();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// 조합 최종 처리 : ime에서 완성된 Composition 최종 입력 
void CWndEdit::ProcessResultString(LPSTR str)
{
	BOOL bDeleteBlock = FALSE;
	if( m_dwBlockBegin != m_dwBlockEnd )
	{
		DeleteBlock();
		bDeleteBlock = TRUE;
	}
	int nLen = strlen(str);

	m_string = m_stringBack;
	
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );
	
	for( int i = 0 ; i < nLen; i++)
	{
		//if( i + m_dwOffset >= m_string.GetLength() )
		//	m_string.Insert(i + m_dwOffset, str [ i ], EDIT_COLOR );
		//else
			m_string.Insert( i + m_dwOffset, str[ i ], EDIT_COLOR );
	}
	m_dwOffset+=nLen;
	m_TextComp[0] = 0;
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 

	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	SetCaretPos( ptCaret );

	m_stringBack = m_string;
}
// 조합 입력중 
void CWndEdit::ProcessCompString( LPSTR str, LPSTR strAttr )
{
	if( m_dwBlockBegin != m_dwBlockEnd )
		DeleteBlock();

	int len = lstrlen( str );
	//if( len > 2 ) return;

	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );

	m_string = m_stringBack;

	// 입력 내용을 스트링에 입력 
	
	if( len  )
	{
		//if( !IsWndStyle( EBS_AUTOHSCROLL ) && !IsWndStyle( EBS_AUTOVSCROLL ) )
	//	{
		// 처음 입력이면 해당 포지션의 스트링을 늘려진다.
		BOOL bFirstConverted = FALSE;
		for(int i = 0; i < len; i++)
		{
			DWORD dwColor = 0xff000000;
			DWORD dwStyle = 0;
			switch ( strAttr[ i ] )
			{
				case ATTR_INPUT:
					dwStyle = 0;
					if( PRIMARYLANGID( m_hKeyLayout ) != LANG_KOREAN ) 
						dwColor = 0xff0000ff;
					//dwColor SetTextColor(hDC,RGB(0,0,0));
					//SetBkMode(hDC,TRANSPARENT);
					break;
				case ATTR_TARGET_CONVERTED:
					if( bFirstConverted == FALSE )
					{
						bFirstConverted = TRUE;
						m_ptConverted = OffsetToPoint( m_dwOffset + i, NULL );
					}
					dwStyle = ESSTY_UNDERLINE | ESSTY_BOLD;
					dwColor = 0xff0000ff;
					//SetTextColor(hDC,RGB(255,255,255));
					//SetBkMode(hDC,OPAQUE);
					//SetBkColor(hDC,RGB(0,0,255));
					break;
				case ATTR_CONVERTED:
					dwColor = 0xff0000ff;
					//SetTextColor(hDC,RGB(0,0,255));
					//SetBkMode(hDC,TRANSPARENT);
					break;
				case ATTR_TARGET_NOTCONVERTED:
					dwStyle = 0;
					//SetTextColor(hDC,RGB(255,255,255));
					//SetBkMode(hDC,OPAQUE);
					//SetBkColor(hDC,RGB(0,255,0));
					break;
				case ATTR_INPUT_ERROR:
					dwStyle = 0;
					//SetTextColor(hDC,RGB(255,255,0));
					//SetBkMode(hDC,TRANSPARENT);
					break;
				case ATTR_FIXEDCONVERTED:
					dwStyle = 0;
					//SetTextColor(hDC,RGB(255,0,0));
					//SetBkMode(hDC,TRANSPARENT);
					break;
				default:
					dwStyle = 0;
					//SetTextColor(hDC,RGB(0,0,0));
					//SetBkMode(hDC,TRANSPARENT);
					break;
			}
			m_string.Insert( i + m_dwOffset, str[ i ], dwColor, dwStyle );
		}
/*
		if( m_pFont->GetTextExtent( m_string ).cx >= GetClientRect().Width() - 8 )
		{
			// IME 입력 버퍼에 있는 내용을 클리어 
			m_TextComp[ 0 ] = 0;
			ImmSetCompositionString(m_hIMC,SCS_SETSTR, "", 1, "", 1);
		}
		else
		*/
		{
			if( PRIMARYLANGID( m_hKeyLayout ) == LANG_KOREAN ) 
			{
				if( len <= 2 )
					strcpy( m_TextComp, str );
			}
			else
				m_TextComp[ 0 ] = 0;
			//m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
		}		
		if( PRIMARYLANGID( m_hKeyLayout ) == LANG_KOREAN ) 
			len = 0;
		if( bFirstConverted == FALSE )
			len = GetCompCursorPos();
		//CPoint ptCaret = OffsetToPoint( m_dwOffset + len, m_szCaret);
		//SetCaretPos( ptCaret );
		//return;
		//}
		//for( int i = 0; i < len; i++ )
		//	m_string.Insert( i + m_dwOffset, str[ i ], 0xff0000ff );
	}
	else
		m_TextComp[ 0 ] = 0;

	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	CPoint ptCaret = OffsetToPoint( m_dwOffset + len, m_szCaret);
	SetCaretPos( ptCaret );
}
// 조합 시작 
void CWndEdit::IMEStartComposition(LONG dwData)
{
	m_stringBack = m_string;
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	SetCaretPos( ptCaret );
}
// 조합 끝 
void CWndEdit::IMEEndComposition(LONG dwData)
{
	if (Check()) 
	{ // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		if ( Enter() )	
		{
			int len
				= ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0);
			if( len == 0 )
				m_string = m_stringBack;
			Leave();
		}
	}
	else
		m_stringBack = m_string;
}
void CWndEdit::OnChar_(UINT nChar)
{
	if( !IsWndStyle( EBS_AUTOHSCROLL ) && !IsWndStyle( EBS_AUTOVSCROLL ) )
	{
		CString strTemp = m_string + (CHAR)nChar; // 길이를 확인하기 위해서 일단 뒤에 str을 추가한다.
		if( m_pFont->GetTextExtent( strTemp ).cx >= GetClientRect().Width() )
			return;
	}
	TRACE( "CWndEdit::OnChar_(UINT nChar = %x)\n",nChar );

	if( ( nChar == 8 ) || ( nChar == 27 )) // backspace
		return;
	
	if( nChar == 3 ) // CTRL-C : copy
	{
		if( m_dwBlockBegin != m_dwBlockEnd )
		{
			DWORD dwBlockBegin;
			DWORD dwBlockEnd;
			if(m_dwBlockBegin > m_dwBlockEnd)
				dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
			else
				dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
			CString strClipboard;
			strClipboard = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
			if( m_bEnableClipboard ) 
				CClipboard::SetText( strClipboard );
		}
		return;
	}
	if( nChar == 22 && m_bEnableClipboard ) // Ctrl-V : Paste
	{
		if( !IsEmptyBlock() )
			DeleteBlock();
		CHAR* buf;
		DWORD dwSize;
		CClipboard::GetTextLength( &dwSize );
		if( dwSize )
		{
			buf = new CHAR[ dwSize + 1 ];
			CClipboard::GetText( buf, dwSize + 1 );
			buf[ dwSize ] = 0; 
			CRect rect = GetClientRect();
			m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );
			m_string.Insert( m_dwOffset, buf, EDIT_COLOR );
			m_dwOffset += dwSize - 1;
			m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
			delete buf;
		//	for( int i = 0 ; i < dwSize; i++)
		//		m_string.Insert( i + m_dwOffset, buf[ i ], EDIT_COLOR );
				//m_string.SetAt( i + m_dwOffset, buf[ i ] );
		}
		return;
	}
	if( nChar == 24 ) // Ctrl-X : Paste
	{
	}
	else
	if( nChar != 9 ) // !tab
	{
		if( !IsEmptyBlock() )
			DeleteBlock();
		// 콘트롤 문자면 리턴 
		if( iscntrl( nChar ) )
			return;
		if( IsDBCSLeadByte( nChar ) && IsHangul( nChar ) == FALSE )
			return;
		char str[2];
		str[0] = nChar;// & 0xff;
		str[1] = 0;
		CRect rect = GetClientRect();
		m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );
		m_string.Insert(m_dwOffset,str,EDIT_COLOR);
		m_dwOffset++;
		m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	}
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	SetCaretPos( ptCaret );
}
void CWndEdit::DeleteBlock( )
{
	//if( m_dwOffset <= m_string.GetLength() )
	{
		CRect rect = GetClientRect();
		m_string.Init( m_pFont, &rect );
		
		if( m_dwBlockBegin == m_dwBlockEnd )
		{
			if( IsDBCSLeadByte( m_string.GetAt( m_dwOffset ) ) )
				m_string.Delete( m_dwOffset, 2);
			else
				m_string.Delete( m_dwOffset, 1);
		}
		else
		{
			DWORD dwBlockBegin;
			DWORD dwBlockEnd;
			if(m_dwBlockBegin > m_dwBlockEnd)
				dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
			else
				dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;

			m_string.Delete( dwBlockBegin, dwBlockEnd - dwBlockBegin );
			m_dwOffset = dwBlockBegin;
			m_dwBlockBegin = m_dwBlockEnd = 0;
		}
		m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	}
}
BOOL CWndEdit::IsYouMessage(UINT msg,WPARAM wparam, LPARAM lparam)
{
	if( !m_hWnd )//|| !m_bAct ) 
		return FALSE;
	if( m_pWndCandList == NULL )
	{		
		if(GetFocusWnd() != GetParentWnd())
			return FALSE;
		if(GetParentWnd()->GetFocusChild() != this)
			return FALSE;
	}
	switch( msg )
	{ 
		case WM_IME_STARTCOMPOSITION:     // 글자 조합의 시작. WM_IME_COMPOSITION 메시지를           // 받을 준비를 한다. 
			OnIMEStartComposition(wparam,lparam);
      return TRUE;    
		case WM_IME_ENDCOMPOSITION:          // 글자 조합의 끝. 조합된 문자열 처리를 끝낸다          break; 
			OnIMEEndComposition(wparam,lparam);
			TRACE( "WM_IME_ENDCOMPOSITION\n" );
      return TRUE;    
    case WM_IME_COMPOSITION:          // 현재 조합중인 문자열이나 완성된 문자열을 얻어서 화면 출력 
			//if(m_bPassword == FALSE)
			OnIMEComposition(wparam,lparam);
			//InvalidateRect(m_hWnd,NULL,TRUE);
      return TRUE;    
	  case WM_IME_SETCONTEXT:						// 글자 조합 윈도우와 한자 변환 윈도우를 표시하지 않게 바꿈
			OnIMESetContext(wparam,lparam);
      return TRUE;     
    case WM_IME_NOTIFY:        
		  OnIMENotify(wparam,lparam);
      return TRUE;             
    case WM_INPUTLANGCHANGE:       // 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
		  OnInputLangChange(wparam,lparam);
      return TRUE;    
	case WM_IME_CONTROL:
		  OnIMEControl(wparam,lparam);
      return TRUE;    
	  case WM_IME_COMPOSITIONFULL:
		  OnIMECompositionFull(wparam,lparam);
      return TRUE;    
	  case WM_CHAR:
			if( wparam != VK_RETURN )
			{
				OnChar_(wparam);
				CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
				SetCaretPos( ptCaret );
			}				
		  break;
	  case WM_KEYDOWN:
		{
//			int	i;
			BYTE chr, chr2;
			TRACE( "WM_KEYDOWN %x\n", wparam );
			switch( wparam )
			{
				case 3: // Ctrl-C : Copy
				{
					/*
					if( m_dwBlockBegin != m_dwBlockEnd )
					{
						DWORD dwBlockBegin;
						DWORD dwBlockEnd;
						if(m_dwBlockBegin > m_dwBlockEnd)
							dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
						else
							dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
						CString strClipboard;
						strClipboard = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
						CClipboard::SetText(strClipboard);
					
					}
					*/
					/*
					int nSize = m_strEdit.GetLength();
					char* buf = new char[nSize + 1];
					strcpy (buf, m_strEdit);

					CClipboard::SetText(buf);

					delete[] buf;
					*/
					break;
				}
				case 22: // Ctrl-V : Paste
				{
					/*
					CHAR* buf;
					DWORD dwSize;
					CClipboard::GetTextLength( &dwSize );
					buf = new CHAR[ dwSize + 1 ];
					CClipboard::GetText( buf, dwSize + 1 );
					buf[ dwSize ] = 0;
					m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
					*/
					break;
				}
				case 24: // Ctrl-X : Paste
				{
				}
				case VK_BACK:
				{
					if( m_dwOffset == 0)
						break;
					BOOL bHangul;
					// 내 위치(m_dwOffset) 바로 이전에 있는 문자가 한글인지 영문인지 알아내기.
					// 한글과 영문 구조상 처음부터 스켄해야 알수 있다. --;
					for( int i = 0; i < m_dwOffset; )
					{
						if( IsDBCSLeadByte( m_string.GetAt( i ) ) )
						{
							i += 2;
							bHangul = TRUE;
						}
						else
						{
							i++;
							bHangul = FALSE;
						}
					}
					if( bHangul )
						m_dwOffset -= 2;
					else
						m_dwOffset--;

					if( m_dwOffset <= m_string.GetLength() )
						DeleteBlock();

					//m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 

					/*
					if( m_dwOffset <= m_string.GetLength() )
					{
						if( m_dwBlockBegin == m_dwBlockEnd )
						{
							if( IsDBCSLeadByte( m_string.GetAt( m_dwOffset ) ) )
								m_string.Delete( m_dwOffset, 2);
							else
								m_string.Delete( m_dwOffset, 1);
						}
						else
						{
							DWORD dwBlockBegin;
							DWORD dwBlockEnd;
							if(m_dwBlockBegin > m_dwBlockEnd)
								dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
							else
								dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;

							m_string.Delete( dwBlockBegin, dwBlockEnd - dwBlockBegin );
							m_dwOffset = dwBlockBegin;
							m_dwBlockBegin = m_dwBlockEnd = 0;
						}
						CRect rect = GetClientRect();
						m_string.Reset(m_pFont,&rect);
					}
					*/
				}
				break;

				case VK_LEFT:
					if( m_dwOffset )
					{
						BOOL bWideChar;
						for( int i = 0; i < (int)m_dwOffset; )
						{
							if( ( bWideChar = IsDBCSLeadByte( m_string[ i ] ) ) )
								i+=2;
							else
								i++;
						}
						if( bWideChar )
							i-=2;
						else
							i--;
						m_dwOffset = i;
					}
					break;

				case VK_RIGHT:
					if( m_dwOffset < m_string.GetLength() )
					{
					if( IsDBCSLeadByte( m_string.GetAt( m_dwOffset ) ) )
						m_dwOffset += 2;
					else
						m_dwOffset += 1;
					}
					break;

				case VK_DELETE:
					if( m_dwOffset < m_string.GetLength() )
						DeleteBlock();
					//m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 

					/*
					if( m_dwOffset < m_string.GetLength() )
					{
						if( m_dwBlockBegin == m_dwBlockEnd )
						{
							if( IsDBCSLeadByte( m_string.GetAt( m_dwOffset ) ) )
								m_string.Delete( m_dwOffset, 2);
							else
								m_string.Delete( m_dwOffset, 1);
						}
						else
						{
							DWORD dwBlockBegin;
							DWORD dwBlockEnd;
							if(m_dwBlockBegin > m_dwBlockEnd)
								dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
							else
								dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;

							m_string.Delete( dwBlockBegin, dwBlockEnd - dwBlockBegin );
							m_dwOffset = dwBlockBegin;
							m_dwBlockBegin = m_dwBlockEnd = 0;
						}
						CRect rect = GetClientRect();
						m_string.Reset(m_pFont,&rect);
					}
					*/
					break;

				case VK_DOWN:
					break;
				case VK_UP:
					break;

				case VK_HOME:
					{
						DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
						m_dwOffset = m_string.GetLineOffset( dwLine );
					}
					break;

				case VK_END:
					{
						if( m_dwOffset < m_string.GetLength())
						{
							DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
							m_dwOffset = m_string.GetLineOffset( dwLine + 1 );

							if( m_dwOffset >= 1 )
								chr  = m_string.GetAt( m_dwOffset - 1 );
							if( m_dwOffset >= 2)
								chr2 = m_string.GetAt( m_dwOffset - 2 );

							if( m_dwOffset < m_string.GetLength() || chr == '\n' )
							{
								if( IsDBCSLeadByte( chr ) && IsDBCSLeadByte( chr2 ) ) // KS 한글?
								{
									if( m_dwOffset > 1 ) m_dwOffset -= 2;
								}
								else
								{
									if( m_dwOffset > 0 ) m_dwOffset--;
								}
							}
						}
					}
					break;

				case VK_RETURN:
				{
					if( IsWndStyle( EBS_WANTRETURN ) )
					{
						m_string.Insert(m_dwOffset,'\n');
						m_dwOffset++;
						CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
						SetCaretPos( ptCaret );
					}
					//m_pParentWnd->OnCommand(m_nIdWnd,WM_KEYDOWN);
					m_pParentWnd->OnChildNotify( EN_RETURN, m_nIdWnd, (LRESULT*)this ); 
					return FALSE;//TRUE;
				}
				case VK_ESCAPE:
					break;
			}
			CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
			SetCaretPos( ptCaret );
			OnKeyDown(wparam, LOWORD(lparam), HIWORD(lparam));
		}
		case WM_KEYUP:
			TRACE( "WM_KEYUP %x\n", wparam );
			if( wparam == 0x0d )
			{
				int a = 0;
				a++;
			}
			OnKeyUp(wparam, LOWORD(lparam), HIWORD(lparam));
			return TRUE;
			break;
  }
	return FALSE;
}

void CWndEdit::Empty( )
{
	m_string.Empty();
	m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = 0;
	m_TextComp[0] = 0;
	m_nCompLen = 0;
	SetCaretPos( CPoint(0,0) );
	m_TextComp[0] = 0;
	m_wndScrollBar.SetMinScrollPos();
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
}

void CWndEdit::SetString( LPCTSTR str, DWORD dwColor )
{
	char strTemp[2];
	int	 i=strlen(str);

	//m_string = str; //rResult ) strcpy( m_string , m_string );
	Empty();
	for( int j = 0; j < i; j++ )
	{
		strTemp[0] = str[j];// & 0xff;
		strTemp[1] = 0;
		PutString(strTemp);
	}
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	SetCaretPos( ptCaret );
	//m_string[i]='\0';
}
void CWndEdit::AddString( LPCTSTR pszString, DWORD dwColor )
{
	CWndText::AddString( pszString, dwColor );
	m_dwOffset += strlen( pszString );
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	SetCaretPos( ptCaret );
	
}
void CWndEdit::ResetString()
{
	CWndText::ResetString();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	SetCaretPos( ptCaret );
	
}
void CWndEdit::PutString( LPSTR str )
{
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	m_string.AddString( str );// += str;
//	m_string = "나는\n 자랑스러운\n 태극기 앞에 조국과 민족의 무궁한 영광을 위하여 몸과 마음을 바쳐 충성을 다할 것을 굳게 다짐합니다.\n 할랠루야 ";
	//m_string.Reset(m_pFont,&rect);
	if(IsDBCSLeadByte(str[0]))
		m_dwOffset+=2;
	else
		m_dwOffset++;
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	//CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret);
	//SetCaretPos( ptCaret );
	
}
void CWndEdit::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );
	if( m_wndScrollBar.IsVisible() )
		m_rectClient.right -= 16; 
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndEdit::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	m_pTheme->RenderWndEditFrame( p2DRender, &rect );

}
void CWndEdit::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pWndCandList == pWndChild )
	{
		SAFE_DELETE( m_pWndCandList );
		GetParentWnd()->SetFocus();
	}
}
void CWndEdit::OnSetFocus(CWndBase* pOldWnd)
{
	//PlayEffect(m_strSndEffect);
	m_pOldWndEditCtrl = m_pActWndEditCtrl;
	m_pActWndEditCtrl = this;

	if( IsEnableModeChange() )
		EnableModeChange( TRUE );
	else
		EnableModeChange( FALSE ); 

	if( IsAlphaNumericMode() )
		SetAlphaNumericMode();
	else
		SetNativeMode();
	m_bCaretVisible = TRUE;

	// 입력 완료할 때 m_stringBack을 제대로 지우면 여기서는 안해도 된다.
	// 여기서하게 되면 입력중인 스트링이 사라질 수도 있다.
	//m_stringBack.Empty();

	CWndText::OnSetFocus( pOldWnd );
}
void CWndEdit::OnKillFocus(CWndBase* pNewWnd)
{
	if( !( m_pWndCandList && m_pWndCandList->m_bReadyCandList == FALSE ) )
		ImmNotifyIME( m_hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );
	
	if( m_pWndCandList == NULL || ( m_pWndCandList && m_pWndCandList->IsDestroy() ) )

	{
		// 이걸 실행하면 CloseCandilist가 실행되면서 m_pWndCandList->Destroy가 실행되고 
		// 따라서 다시 CWndEdit::OnKillFocus(CWndBase* pNewWnd)가 실행되고 무한 반복된다.
		// 따라서 m_pWndCandList == NULL 일 때 실행시키자.
		//ImmNotifyIME(m_hIMC,NI_COMPOSITIONSTR,CPS_COMPLETE,0);
		//GetParentWnd()->SetFocus();
		m_bCaretVisible = FALSE;
		m_pActWndEditCtrl = NULL;
	}
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	CWndText::OnKillFocus( pNewWnd );
}
BOOL CWndEdit::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	m_pApp->SetDeviceCursor( m_hEditCursor );
	return TRUE;
}
//}}AFX
#else //_MULTI_LANGUAGE_
///////////////////////////////////////////////////////////////////
// class CIMEMgr
// Remark:
//	IME 의 상태를 관리한다.
//   
///////////////////////////////////////////////////////////////////
CIMEMgr	g_imeMgr;

#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )	// New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )	// New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )	// New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )	// New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )	// New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )	// New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )	// New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41	( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )	// MSPY1.5	// SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42	( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )	// MSPY2	// Win2k/WinME
#define IMEID_CHS_VER53	( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )	// MSPY3	// WinXP

enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
enum { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

wchar_t s_aszIndicator[5][3] =  
{
	L"En",
	L"\x7B80",
	L"\x7E41",
	L"\xAC00",
	L"\x3042",
};

int	GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
	case LANG_JAPANESE:
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		return HANGEUL_CHARSET;
	case LANG_CHINESE:
		switch( SUBLANGID(langid) )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;
		case SUBLANG_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;
		default:
			return ANSI_CHARSET;
		}
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

int	GetCodePageFromCharset( int charset )
{
	switch( charset )
	{
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	default:
		return 1252;
	}
}

int	GetCodePageFromLang( LANGID langid )
{
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}


///////////////////////////////////////////////////////////////////
// class CIMEMgr
CIMEMgr::CIMEMgr()
{
	/* Class Member Variables */ 
	m_dwId[0] = m_dwId[1] = 0;
	m_ImeState = IMEUI_STATE_OFF;

	/* IME.DLL */ 
	m_hDllIme = NULL;

	/* IMM32.DLL */ 
	CHAR szPath[MAX_PATH+1] = { NULL, };
    
	if( GetSystemDirectoryA( szPath, MAX_PATH+1 ) ) {

		_tcscat_s( szPath, _countof(szPath), "\\imm32.dll" );
		m_hDllImm32 = LoadLibraryA( szPath );
		if( m_hDllImm32 )
		{
			_ImmLockIMC		= (INPUTCONTEXT*(WINAPI *)( HIMC ))
				GetProcAddress( m_hDllImm32, "ImmLockIMC" );
			_ImmUnlockIMC	= (BOOL(WINAPI *)( HIMC ))
				GetProcAddress( m_hDllImm32, "ImmUnlockIMC" );
			_ImmLockIMCC	= (LPVOID(WINAPI *)( HIMCC ))
				GetProcAddress( m_hDllImm32, "ImmLockIMCC" );
			_ImmUnlockIMCC	= (BOOL(WINAPI *)( HIMCC ))
				GetProcAddress( m_hDllImm32, "ImmUnlockIMCC" );
		}
	}
}
/*----------------------------------------------------------------------------*/ 
CIMEMgr::~CIMEMgr()
{
	if( m_hDllIme ) FreeLibrary( m_hDllIme );

	if(m_hDllImm32) FreeLibrary(m_hDllImm32);
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::InputLangChange(HWND hWnd, HKL hkl)
{
	if(m_hkl != hkl) {

		m_hkl = (HKL)hkl;
		m_langId = LOWORD(m_hkl);
		m_codePage = GetCodePageFromLang(m_langId);

		/* Check Property */ 
		m_property = ImmGetProperty(m_hkl, IGP_PROPERTY);

		m_bUnicodeIME = (m_property & IME_PROP_UNICODE) ? true : false;

		/* Update Indicator */ 
		CheckToggleState(hWnd);

		/* Update m_dwId[] */ 
		GetImeId();

		/* Bind Proc */ 
		SetupImeApi(hWnd);

		/* Setup IME */ 
		HIMC hIMC = ImmGetContext(hWnd);
		if(hIMC) {
			ImmSetOpenStatus(hIMC,TRUE);	
			ImmReleaseContext(hWnd, hIMC);
		}

	}
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::Composition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		if(lParam&GCS_COMPSTR) {

			int compSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
			
			m_composition.resize( compSize/sizeof(wchar_t) );

#ifdef __VS6
			ImmGetCompositionStringW(hIMC, GCS_COMPSTR, m_composition.begin(), compSize);
#else
			ImmGetCompositionStringW(hIMC, GCS_COMPSTR, &m_composition.begin(), compSize);
#endif
		}
		if(lParam&GCS_RESULTSTR) {

			int resultSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);

			m_result.resize( resultSize/sizeof(wchar_t) );
#ifdef __VS6
			ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, m_result.begin(), resultSize);
#else
			ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, &m_result.begin(), resultSize);
#endif
		}
		if(lParam&GCS_COMPATTR) {

			int attrSize = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, NULL, 0);

			m_attr.resize( attrSize );
#ifdef __VS6
			ImmGetCompositionStringW(hIMC, GCS_COMPATTR, m_attr.begin(), attrSize);
#else
			ImmGetCompositionStringW(hIMC, GCS_COMPATTR, &m_attr.begin(), attrSize);
#endif
		}
		if(lParam&GCS_CURSORPOS) {

			m_cursorPos = LOWORD(ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0));

		}
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::Candidate(HWND hWnd)
{
	HIMC hIMC = ImmGetContext(hWnd);

	if(hIMC) {

		int candidateSize = ImmGetCandidateListW(hIMC, 0, NULL, 0);

		if(candidateSize > 0) { 

			m_candidate.resize(candidateSize);

			ImmGetCandidateListW(hIMC, 0, (CANDIDATELIST*)&m_candidate[0], candidateSize);
		}

		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::Reading(HWND hWnd)
{
    m_reading.clear();

    if( !m_dwId[0] ) return;

	HIMC hIMC = ImmGetContext(hWnd);
	if( hIMC ) {

		DWORD dwErr = 0;

		if( _GetReadingString ) {

			UINT uMaxUiLen;
			BOOL bVertical;
			// Obtain the reading string size

			m_reading.resize( _GetReadingString( hIMC, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen ) );

			if( m_reading.size() ) {

#ifdef __VS6
				_GetReadingString( hIMC, m_reading.size(), m_reading.begin(), (PINT)&dwErr, &bVertical, &uMaxUiLen );
#else
				_GetReadingString( hIMC, m_reading.size(), &(*m_reading.begin()), (PINT)&dwErr, &bVertical, &uMaxUiLen );
#endif
			}

			m_bVerticalReading = bVertical ? true : false;

		} else {

			// IMEs that doesn't implement Reading String API
			wchar_t* temp;
			DWORD tempLen;
			bool bUnicodeIme = false;
			INPUTCONTEXT *lpIC = _ImmLockIMC(hIMC);		

			if(lpIC == NULL) {
				temp = NULL;
				tempLen = 0;
			} else {
				LPBYTE p = 0;
				switch( m_dwId[0] )
				{
					case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
					case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
					case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
						p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 24 );
						if( !p ) break;
						tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
						dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
						temp = (wchar_t *)( p + 56 );
						bUnicodeIme = true;
						break;

					case IMEID_CHT_VER50: // 5.0.x.x // WinME
						p = *(LPBYTE *)( (LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
						if( !p ) break;
						p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
						temp = (wchar_t *)(p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = false;
						break;

					case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
					case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
					case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
						p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 4);
						if( !p ) break;
						p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = true;
						break;

					// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
					case IMEID_CHS_VER41:
						{
							int nOffset;
							nOffset = ( m_dwId[1] >= 0x00000002 ) ? 8 : 7;

							p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
							if( !p ) break;
							tempLen = *(DWORD *)(p + 7*4 + 16*2*4);
							dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
							dwErr = min( dwErr, tempLen );
							temp = (wchar_t *)(p + 6*4 + 16*2*1);
							bUnicodeIme = true;
						}
						break;

					case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
						{
							OSVERSIONINFOA osi;
							osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
							GetVersionExA( &osi );

							int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(wchar_t) : sizeof(char);
							p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
							if( !p ) break;
							tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
							dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
							temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
							bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
						}
						break;

					default:
						temp = NULL;
						tempLen = 0;
						break;
				}
			}

			

			if(tempLen == 0) {
				m_reading.clear();
			} else {
				if( bUnicodeIme ) {
					m_reading.assign(temp, temp + tempLen);
				} else {
					m_reading.resize( MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, NULL, 0) );

#ifdef __VS6
					MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, m_reading.begin(), m_reading.size()); 
#else
					MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, &(*m_reading.begin()), m_reading.size()); 
#endif
				}
			}

			_ImmUnlockIMCC(lpIC->hPrivate);
			_ImmUnlockIMC(hIMC);

			m_bVerticalReading = !GetReadingWindowOrientation();
		}
		ImmReleaseContext(hWnd, hIMC);
	}

	return;
}
/*----------------------------------------------------------------------------*/ 
bool CIMEMgr::Private(HWND hWnd, LPARAM dwData)
{
	// Trap some messages to hide reading window
	switch( m_dwId[0] )
	{
	case IMEID_CHT_VER42:
	case IMEID_CHT_VER43:
	case IMEID_CHT_VER44:
	case IMEID_CHS_VER41:
	case IMEID_CHS_VER42:
		if((dwData==1)||(dwData==2)) 
			return true;
		else
			return false;

	case IMEID_CHT_VER50:
	case IMEID_CHT_VER51:
	case IMEID_CHT_VER52:
	case IMEID_CHT_VER60:
	case IMEID_CHS_VER53:
		if((dwData==16)||(dwData==17)||(dwData==26)||(dwData==27)||(dwData==28))
			return true;
		else
			return false;

	default:
		return false;
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::CompleteComposition(HWND hWnd)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );
		ImmReleaseContext(hWnd, hIMC);
	}
}
#ifdef __VBUGFIX_20050201
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::CancelComposition(HWND hWnd)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
		ImmReleaseContext(hWnd, hIMC);
	}
}
#endif
/*----------------------------------------------------------------------------*/ 
HIMC	CIMEMgr::AssociateContext(HWND hWnd, HIMC hIMC)
{
	return ImmAssociateContext( hWnd, hIMC );
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::SetOpenStatus(HWND hWnd, BOOL openState)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmSetOpenStatus(hIMC,openState);
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
BOOL	CIMEMgr::GetOpenStatus(HWND hWnd)
{
	BOOL result = FALSE;

	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		result = ImmGetOpenStatus(hIMC);
		ImmReleaseContext(hWnd, hIMC);
	}

	return result;
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::NotifyIME(HWND hWnd, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmNotifyIME(hIMC, dwAction, dwIndex, dwValue);
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetResult(char* text, int len)
{
#ifdef __VS6
	return ConvertString(m_codePage, m_result.begin(), m_result.size(), text, len);
#else
	return ConvertString(m_codePage, &(*m_result.begin()), m_result.size(), text, len);
#endif
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetCompositon(char* text, int len)
{
	if( m_composition.empty() )
		return 0;
#ifdef __VS6
	return ConvertString(m_codePage, m_composition.begin(), m_composition.size(), text, len);
#else
	return ConvertString(m_codePage, &(*m_composition.begin()), m_composition.size(), text, len);
#endif
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetAttribute(BYTE* attr, int len)
{
#ifdef __VS6
	int attrSize = WideCharToMultiByte(m_codePage, 0, m_composition.begin(), m_composition.size(), NULL, 0, NULL, NULL);
#else
	int attrSize = WideCharToMultiByte(m_codePage, 0, &(*m_composition.begin()), m_composition.size(), NULL, 0, NULL, NULL);
#endif

	if(attr == NULL) {
		return attrSize;
	} else {
		if(len < attrSize) {
			return 0;
		} else {

			for(int i=0; i<(int)( m_composition.size() ); ++i)
			{
				int count = WideCharToMultiByte(m_codePage, 0, &m_composition[i], 1, NULL, 0, NULL, NULL);

				for(int j=0; j<count; ++j) *attr++ = m_attr[i];
			}

			return attrSize;
		}			
	}
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetCursorPos()
{
#ifdef __VS6
	return ConvertString(m_codePage, m_composition.begin(), min(m_cursorPos, (int)( m_composition.size() )), NULL, 0);
#else
	return ConvertString(m_codePage, &(*m_composition.begin()), min(m_cursorPos, (int)( m_composition.size() )), NULL, 0);
#endif
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetCandidate(DWORD index, char* text, int len)
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount) {
			return 0;
		} else {

			if(m_bUnicodeIME) {
				
				wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);

				return ConvertString(m_codePage, wText, wcslen(wText), text, len);

			} else {

				char* temp = (char*)(&m_candidate[0] + candidateList->dwOffset[index]);

				if(text == 0) {
					return strlen(temp);
				} else {
					int tempLen = strlen(temp);

					if(len < tempLen) {
						return 0;
					} else {
						memcpy(text, temp, tempLen);
						return tempLen;
					}
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*/ 
bool	CIMEMgr::GetCandidateList(CANDIDATELIST* candidateList)
{
	if(m_candidate.empty()) {
		return false;
	} else {
		memcpy(candidateList, &m_candidate[0], sizeof(CANDIDATELIST));
		return true;
	}
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetReading(char* text, int len)
{
#ifdef __VS6
	return ConvertString(m_codePage, m_reading.begin(), m_reading.size(), text, len);
#else
	return ConvertString(m_codePage, &(*m_reading.begin()), m_reading.size(), text, len);
#endif
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len)
{
	if(text == 0) {
		return WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);
	} else {
		int tLen = WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);

		if(tLen > len) {
			return 0;
		} else {
			return WideCharToMultiByte(codePage, 0, wText, wLen, text, tLen, NULL, NULL);
		}
	}
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::CheckToggleState(HWND hWnd)
{
	/* Update Indicator */ 
    switch (PRIMARYLANGID(m_langId)) 
	{
    case LANG_KOREAN:
        m_bVerticalCandidate = false;
        m_wszCurrIndicator = s_aszIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_bVerticalCandidate = true;
        m_wszCurrIndicator = s_aszIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

	    m_bVerticalCandidate = true;
		switch(SUBLANGID(m_langId)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            m_bVerticalCandidate = m_dwId[0] == 0;
            m_wszCurrIndicator = s_aszIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszCurrIndicator = s_aszIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

    default:
        m_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszCurrIndicator == s_aszIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = towlower( szLang[1] );
    }
	
	/* Check Toggle State */ 
	HIMC hIMC = ImmGetContext(hWnd);

    if( hIMC ) {

		bool bIme = ImmIsIME( m_hkl ) != 0;

        if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) {

            DWORD dwConvMode, dwSentMode;
			ImmGetConversionStatus(hIMC, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

        } else {

            m_ImeState = ( bIme && ImmGetOpenStatus(hIMC) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }

		ImmReleaseContext(hWnd, hIMC);
        
	} else {

        m_ImeState = IMEUI_STATE_OFF;
	}
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::GetImeId()
{
    char    szTmp[1024];

	m_dwId[0] = m_dwId[1] = 0;

    if(!((m_hkl==_CHT_HKL) || (m_hkl==_CHT_HKL2) || (m_hkl==_CHS_HKL)))
		return;
        
	if ( !ImmGetIMEFileNameA( m_hkl, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
        return;

    if ( !_GetReadingString ) {

        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) ) {

	        return;
        }
    }

    DWORD   dwVerHandle;
    DWORD   dwVerSize = GetFileVersionInfoSize( szTmp, &dwVerHandle );

    if( dwVerSize ) {

        LPVOID  lpVerBuffer = alloca( dwVerSize );

        if( GetFileVersionInfo( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) {

			LPVOID  lpVerData;
			UINT    cbVerData;

            if( VerQueryValue( lpVerBuffer, "\\", &lpVerData, &cbVerData ) ) {

                DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                if( _GetReadingString
                    ||
                    ( m_langId == LANG_CHT &&
                        ( dwVer == MAKEIMEVERSION(4, 2) || 
                        dwVer == MAKEIMEVERSION(4, 3) || 
                        dwVer == MAKEIMEVERSION(4, 4) || 
                        dwVer == MAKEIMEVERSION(5, 0) ||
                        dwVer == MAKEIMEVERSION(5, 1) ||
                        dwVer == MAKEIMEVERSION(5, 2) ||
                        dwVer == MAKEIMEVERSION(6, 0) ) )
                    ||
                    ( m_langId == LANG_CHS &&
                        ( dwVer == MAKEIMEVERSION(4, 1) ||
                        dwVer == MAKEIMEVERSION(4, 2) ||
                        dwVer == MAKEIMEVERSION(5, 3) ) ) ) {

                    m_dwId[0] = dwVer | m_langId;
                    m_dwId[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

    _GetReadingString = NULL;
    _ShowReadingWindow = NULL;

	if( ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) != 0 ) {

		if( m_hDllIme ) FreeLibrary( m_hDllIme );
	    
		m_hDllIme = LoadLibraryA( szImeFile );

		if ( m_hDllIme ) {
			_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
				( GetProcAddress( m_hDllIme, "GetReadingString" ) );
			_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
				( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

			if( _ShowReadingWindow ) {
				HIMC hIMC = ImmGetContext(hWnd);
				if(hIMC) {
					_ShowReadingWindow( hIMC, false );
					ImmReleaseContext(hWnd, hIMC);
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*/ 
bool CIMEMgr::GetReadingWindowOrientation()
{
    bool bHorizontalReading = ( m_hkl == _CHS_HKL ) || ( m_hkl == _CHT_HKL2 ) || ( m_dwId[0] == 0 );
    if( !bHorizontalReading && ( m_dwId[0] & 0x0000FFFF ) == LANG_CHT )
    {
		char szRegPath[MAX_PATH] =  { NULL, };
        HKEY hKey;
        DWORD dwVer = m_dwId[0] & 0xFFFF0000;
        _tcscpy_s( szRegPath, _countof(szRegPath), "software\\microsoft\\windows\\currentversion\\" );
        _tcscat_s( szRegPath, _countof(szRegPath), ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }

	return bHorizontalReading;
}

///////////////////////////////////////////////////////////////////
// class CWndCandList
// Remark:
//   일본어나 중국어는 입력 이후 유사어 리스트를 통해 단어를 고른다.
//   CWndCandList는 그 유사어를 선택할 수 있게하는 팝업 윈도다.
//
///////////////////////////////////////////////////////////////////
CWndCandList::CWndCandList() 
{ 
	m_nPageStart = 0;
	m_nScrollPos = 0;
	m_nLineSpace = 0;
#ifdef __V050823_JAPAN_ATOK
	m_ptWindowPos = 0;
#endif
} 
CWndCandList::~CWndCandList() 
{ 
} 
void CWndCandList::UpdateCandList( CPoint windowPos )
{ 
	//if( m_ptWindowPos != CPoint( 0, 0 ) && m_ptWindowPos != windowPos )
	//{
		//g_imeMgr.NotifyIME( m_hWnd, NI_CLOSECANDIDATE,0,0);
	//	return;
	//}
	CANDIDATELIST candidateList;

	g_imeMgr.GetCandidateList(&candidateList);

	TRACE( "Candlist ----------------------\n" );
	TRACE( "Count %d \n", candidateList.dwCount );
	TRACE( "Selection %d \n", candidateList.dwSelection );
	TRACE( "PageStart %d \n", candidateList.dwPageStart );
	TRACE( "PageSize %d \n", candidateList.dwPageSize );

	int nPageStart = candidateList.dwSelection - (candidateList.dwSelection%9);

	if( m_nPageStart != nPageStart )
	{
		m_nPageStart = nPageStart; 
		g_imeMgr.NotifyIME( m_hWnd, NI_SETCANDIDATE_PAGESTART, 0, nPageStart );
		// ImmNotifeIME를 하게 되면 다시 이 함수를 부른다. 그렇게 되면 위에 ResetContent를 실행하게 되므로
		// 스크롤 포지션이 다시 리셋된다. 이 부분이 맨 마지막에 실행되므로 여기서 스크롤 포지션을 세팅해야한다.
		m_wndScrollBar.SetScrollPos( nPageStart );
		m_nScrollPos = nPageStart;
		return;
	}
	// 리셋을 하면 스크롤 포지션이 초기화 된다. 
	// 스크롤 바를 눌러서 스크롤 위치가 조절된 상태라면 초기화가 되면
	// 안되기 때문에 현재 스크롤 포지션을 저장해 둔다.
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	ResetContent();

	// get the longest string length
	DWORD dwPreferNumPerPage = max(candidateList.dwPageSize, DEFAULT_CAND_NUM_PER_PAGE);
	
	int max_width = 0;

	for( int i = 0; i < (int)( candidateList.dwCount ); i++ ) 
	{
		char buf[255];
		int len = g_imeMgr.GetCandidate(i, buf+2, sizeof(buf)-3);
		buf[0] = ' ';
		buf[1] = ' ';
		buf[len+2] = 0;

		AddString( buf );

		SIZE size;
		m_pFont->GetTextExtent( buf, &size, g_imeMgr.m_codePage );

		max_width = max(max_width, size.cx);
    }
#ifdef __V050823_JAPAN_ATOK
	m_ptWindowPos = windowPos;
#endif
	CRect rect( 0, 0, max_width + 24, dwPreferNumPerPage * ( GetFontHeight() + m_nLineSpace ) + 10  );
	rect.OffsetRect( windowPos.x, windowPos.y - rect.Height() );
	SetWndRect( rect );
	AdjustWndBase();
	RestoreDeviceObjects();
	
	// 9개의 배수로 갯수를 채운다.
	int nDiv2 = 9 - ( candidateList.dwCount % 9 );
	if( nDiv2 < 9 )
	{
		for( int i = 0; i < nDiv2; i++ )
			AddString( "" );
	}
	// 스크롤 포지션을 복원한다.
	m_wndScrollBar.SetScrollPos( nScrollPos );
	SetCurSel( candidateList.dwSelection );
}
void CWndCandList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE )
		Destroy();
}
void CWndCandList::OnDraw( C2DRender* p2DRender ) 
{ 
	p2DRender->SetFont(m_pFont);

	WORD wOldCodePage = m_pFont->m_wCodePage;
	m_pFont->m_wCodePage = g_imeMgr.m_codePage;
	
	CWndListBox::OnDraw( p2DRender );
	int nMaxHeight = m_pFont->GetMaxHeight() + m_nLineSpace;

    for ( int y = 0 ; y < 9; y++ ) 
	{
		char buf[255];
		wsprintf(buf, "%d:", y + 1);
		p2DRender->TextOut( 2, 2 + y * nMaxHeight, buf, 0xff000000 );//, lstrlen( buf ) );
    }
	if( m_nScrollPos != m_wndScrollBar.GetScrollPos() )
	{
		m_nScrollPos = m_wndScrollBar.GetScrollPos();
		g_imeMgr.NotifyIME( m_hWnd, NI_SETCANDIDATE_PAGESTART, 0, m_nScrollPos );
	}
	m_pFont->m_wCodePage = wOldCodePage;
} 
void CWndCandList::OnInitialUpdate() 
{ 
	CWndListBox::OnInitialUpdate(); 

	int nCount = 0, nNum = 0;

	m_wndScrollBar.SetScrollPage( 9 );

	if( nNum > 20 ) nNum = 20;
	if( nNum < 5 ) nNum = 5;
	SetWndRect( CRect( 0, 0, 150, nNum * ( GetFontHeight() + 3 ) + 8 ) );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCandList::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	SetTitle( _T( "World" ) );
	CRect rect( 0, 0, 150, 200 );
	CWndBase::Create( WBS_TOPMOST, rect, pWndParent, APP_COMMAND );
#ifdef __V050322_CACHEQUEST
///	CWndBase::Create( WBS_TOPMOST | WBS_VSCROLL, rect, pWndParent, APP_COMMAND );
	AdjustWndBase();
	RestoreDeviceObjects();
#endif
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return TRUE;
} 
void CWndCandList::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CWndListBox::OnLButtonUp( nFlags, point );
	// 클릭했을 경우 리스트박스의 현재 셀렉트된 것을 Ime에게 선택했다고 알린다.
	g_imeMgr.NotifyIME( m_hWnd, NI_SELECTCANDIDATESTR,0,GetCurSel());
	// 선택 이후 유사어 리스트를 닫으라고 Ime알린다.
#ifdef __V050823_JAPAN_ATOK
	m_pParentWnd->PostMessage( WM_CLOSECANDIDATE );
#else
	g_imeMgr.NotifyIME( m_hWnd, NI_CLOSECANDIDATE,0,0);
#endif
} 
void CWndCandList::OnKillFocus(CWndBase* pNewWnd)
{
	// 마우스 커서로 다른 창을 클릭하거나 포커스를 변경했을 경우 단어 조합을 완료한다.
	// 완료 조건은 pNewWnd가 NULL이 아니어야한다. (NULL일 때는 창이 파괴될 때 외에는 없다.)
	// 즉, 포커스가 다른 창으로 변경될 때만 조합을 완결하는 것이다.
	// 그리고 현재창의 프레임윈도와 새윈도의 프레임윈도가 다르면 포커스가 이전된 것으로 간주한다.
	if( pNewWnd && pNewWnd != m_pParentWnd->GetParentWnd() )
	{
		// 단어 조합을 완료하라는 매시지를 Ime에게 보낸다.
		// 보통 완료는 유사어 창이 열려 있으면 닫으면서 완료하고 열리 있지 않으면 그냥 완료한다.
		g_imeMgr.CompleteComposition(m_hWnd);
	}
}

///////////////////////////////////////////////////////////////////////////////
// class CWndHCandList
// 
///////////////////////////////////////////////////////////////////////////////
void CWndHCandList::UpdateCandList(CPoint windowPos)
{
	CANDIDATELIST candidateList;

	g_imeMgr.GetCandidateList(&candidateList);

	int nPageStart = candidateList.dwSelection - (candidateList.dwSelection%9);

	DWORD dwPreferNumPerPage = max(candidateList.dwPageSize, DEFAULT_CAND_NUM_PER_PAGE);
	
	m_maxWidth = 0;

	DWORD dwPageEnd = min(candidateList.dwPageStart+candidateList.dwPageSize, candidateList.dwCount);

	m_candidate.clear();

	for( int i = candidateList.dwPageStart; i < (int)( dwPageEnd ); i++ ) 
	{
		int len = 0;
		char buf[255];
		len += wsprintf(buf, "%d:", i-candidateList.dwPageStart+1);
		len += g_imeMgr.GetCandidate(i, buf+len, sizeof(buf)-len);
		buf[len] = 0;

		m_candidate.push_back( string(buf, buf+len) );

		SIZE size;
		
		m_pFont->GetTextExtent( buf, &size, g_imeMgr.m_codePage );

		m_maxWidth = max(m_maxWidth, size.cx + 2);
    }

	CRect rect( 0, 0, m_maxWidth * (dwPageEnd-candidateList.dwPageStart) + 4, 16 );
	rect.OffsetRect( windowPos.x, windowPos.y - rect.Height() );
	SetWndRect( rect );
}
BOOL CWndHCandList::Initialize( CWndBase* pWndParent, DWORD nType )
{
	CRect rect( 0, 0, 100, 100 );
	CWndBase::Create( WBS_TOPMOST | WBS_NOFRAME, rect, pWndParent, APP_COMMAND );
	return TRUE;
}
void CWndHCandList::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	
	m_pTheme->RenderWndTextFrame( p2DRender, &rect );
}
void CWndHCandList::OnDraw( C2DRender* p2DRender )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 255,255,255 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 226,198,181 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	CRect rtWindow = this->GetWindowRect();
	p2DRender->RenderFillRect(rtWindow, dwColor2 );
	p2DRender->RenderFillRect(CRect(rtWindow.left+1, rtWindow.top+1, rtWindow.right-1, rtWindow.bottom-1), 0xFFFFFFFF);

	p2DRender->SetFont(m_pFont);

	WORD wOldCodePage = m_pFont->m_wCodePage;
	m_pFont->m_wCodePage = g_imeMgr.m_codePage;

	for(int i=0; i<(int)( m_candidate.size() ); ++i)
	{
		const char* text = m_candidate[i].c_str();

		p2DRender->TextOut(2+m_maxWidth*i, 2, text, 0xFF000000 );
	}

	m_pFont->m_wCodePage = wOldCodePage;
}
///////////////////////////////////////////////////////////////////////////////
// class CReadingList
// 
// CandidateList 와 흡사한 ReadingList 이다.
///////////////////////////////////////////////////////////////////////////////
void CReadingList::UpdateReading(CPoint windowPos)
{
	int len = g_imeMgr.GetReading(NULL, 0);
	
	m_reading.resize(len + 1);

#ifdef __VS6
	g_imeMgr.GetReading(m_reading.begin(), m_reading.size());
#else
	g_imeMgr.GetReading( &(*m_reading.begin()), m_reading.size());
#endif

	m_reading[len] = 0;

	CRect rect;
	if(g_imeMgr.IsVerticalReading())
		rect = CRect(0,0,18,66);
	else
	{
		SIZE size;

#ifdef __VS6
		m_pFont->GetTextExtent(m_reading.begin(), &size, g_imeMgr.m_codePage);
#else
		m_pFont->GetTextExtent( &(*m_reading.begin()), &size, g_imeMgr.m_codePage );
#endif

		rect = CRect(0,0,size.cx+4,18);
	}
	rect.OffsetRect( windowPos.x, windowPos.y - rect.Height() );
	SetWndRect( rect );
}
BOOL CReadingList::Initialize( CWndBase* pWndParent, DWORD nType)
{
	CRect rect( 0, 0, 100, 100 );
	CWndBase::Create( WBS_TOPMOST | WBS_NOFRAME, rect, pWndParent, APP_COMMAND );
	return TRUE;
}
void CReadingList::PaintFrame(C2DRender* p2DRender)
{
}
void CReadingList::OnDraw( C2DRender* p2DRender )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 255,255,255 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 226,198,181 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	CRect rtWindow = this->GetWindowRect();
	p2DRender->RenderFillRect(rtWindow, dwColor2 );
	p2DRender->RenderFillRect(CRect(rtWindow.left+1, rtWindow.top+1, rtWindow.right-1, rtWindow.bottom-1), 0xFFFFFFFF);


	LPCTSTR begin = &(*m_reading.begin());
	LPCTSTR end = begin + m_reading.size();

	p2DRender->SetFont(m_pFont);

	WORD wOldCodePage = m_pFont->m_wCodePage;
	m_pFont->m_wCodePage = g_imeMgr.m_codePage;
	
	if(g_imeMgr.IsVerticalReading())
	{
		int x = 1;
		int y = 1;

		while( *begin && begin < end )
		{
			char text[8];
			const char* next = CharNextEx( begin, g_imeMgr.m_codePage );
			memcpy(text, begin, next-begin);
			text[next-begin] = 0;

			SIZE size;
			
			m_pFont->GetTextExtent(text, &size, g_imeMgr.m_codePage);

			p2DRender->TextOut(x + (16-size.cx)/2, y + (16-size.cy)/2, text, 0xFF000000 );

			y += 16;
			begin = next;
		}
	} 
	else 
	{
#ifdef __VS6
		p2DRender->TextOut( 2, 1, m_reading.begin(), 0xFF000000 );
#else
		p2DRender->TextOut( 2, 1, &(*m_reading.begin()), 0xFF000000 );
#endif

	}

	m_pFont->m_wCodePage = wOldCodePage;
}


#define EDIT_COLOR 0xff000000


///////////////////////////////////////////////////////////////////////////////
// CWndEdit

HWND		CWndEdit::m_hWnd = NULL;
HIMC		CWndEdit::m_hIMCBackup = NULL;
HCURSOR		CWndEdit::m_hEditCursor = NULL;
CWndEdit*	CWndEdit::m_pActWndEditCtrl = NULL;

CWndEdit::CWndEdit()
{
	m_pFont = NULL;

	m_bEnableModeChange = FALSE; // 한영 전환 가능한가?

	// override
	m_bCaretVisible = TRUE;
	m_byWndType = WTYPE_EDITCTRL;
	
	m_bEnableClipboard = TRUE;//FALSE;
	
	m_pWndCandList = NULL;
	m_pWndHCandList = NULL;
	m_pWndReadingList = NULL;
	m_bImeNativeMode = FALSE;

	m_bKeepComposition = FALSE;


#ifdef __V051004_ENGLISHALIGN
	m_string.m_bWordAlign = FALSE;
	m_stringBack.m_bWordAlign = FALSE;
#endif
}

CWndEdit::~CWndEdit()
{
	if(m_pActWndEditCtrl == this)
		m_pActWndEditCtrl = NULL;

	SAFE_DELETE( m_pWndCandList );
	SAFE_DELETE( m_pWndHCandList );
	SAFE_DELETE( m_pWndReadingList );
}
// 창을 생성한다.
BOOL CWndEdit::Create(HWND hwnd,DWORD dwStyle,const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	if(hwnd) m_hWnd = hwnd;

	g_imeMgr.InputLangChange(m_hWnd, GetKeyboardLayout(0));
	ClearAll();

	SetAlphaNumericMode();

	if( dwStyle & EBS_PASSWORD )
		EnableModeChange( FALSE );
	else
		EnableModeChange( TRUE );

#ifdef __V050322_CACHEQUEST
	m_strTexture = DEF_CTRL_TEXT;
#endif
	
	SetFontColor( nFontColorR, nFontColorG, nFontColorB );

	return CWndBase::Create(dwStyle|WBS_CHILD,rect,pParentWnd,nID);
}
// CWndBase overide
void CWndEdit::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pWndCandList == pWndChild )
	{
		SAFE_DELETE( m_pWndCandList );
		SetFocus();
	}
	if( m_pWndHCandList == pWndChild )
	{
		SAFE_DELETE( m_pWndHCandList );
		SetFocus();
	}
	if( m_pWndReadingList == pWndChild )
	{
		SAFE_DELETE( m_pWndReadingList );
		SetFocus();
	}
}
void CWndEdit::OnSetFocus(CWndBase* pOldWnd)
{
	m_pActWndEditCtrl = this;

	if( m_bEnableModeChange )
		EnableModeChange( TRUE );
	else
		EnableModeChange( FALSE ); 

	if( m_bImeNativeMode )
		SetNativeMode();
	else
		SetAlphaNumericMode();

	m_bCaretVisible = TRUE;

	HKL hkl = GetKeyboardLayout(0);

	if(g_imeMgr.m_hkl != hkl) {
		g_imeMgr.InputLangChange(m_hWnd, hkl);
		Empty();
	}
	// 입력 완료할 때 m_stringBack을 제대로 지우면 여기서는 안해도 된다.
	// 여기서하게 되면 입력중인 스트링이 사라질 수도 있다.
	//m_stringBack.Empty();

	CWndText::OnSetFocus( pOldWnd );
}
void CWndEdit::OnKillFocus(CWndBase* pNewWnd)
{
	if( !m_bKeepComposition )
		g_imeMgr.CompleteComposition(m_hWnd);

	if( (m_pWndCandList == NULL || (m_pWndCandList && m_pWndCandList->IsDestroy())) &&
		(m_pWndHCandList == NULL || (m_pWndHCandList && m_pWndHCandList->IsDestroy())) &&
		(m_pWndReadingList == NULL || (m_pWndReadingList && m_pWndReadingList->IsDestroy())) )
	{
		m_bCaretVisible = FALSE;
		m_pActWndEditCtrl = NULL;
	}
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	CWndText::OnKillFocus( pNewWnd );
}
void CWndEdit::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
 	m_rectClient.DeflateRect( 3, 3, 3, 0 );
	if( m_wndScrollBar.IsVisible() )
		m_rectClient.right -= 16; 
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndEdit::PaintFrame(C2DRender* p2DRender)
{
#ifdef __TOON
	if( ( m_pParentWnd && !m_pParentWnd->m_bTile ) &&  !m_bTile )
	{
		if( !m_pTexture && !m_strTexture.IsEmpty() )
			SetTexture(  p2DRender->m_pd3dDevice, MakePath( DIR_THEME,  CWndBase::m_resMng.GetSelectLang(), m_strTexture)  );

		CRect rcWindow = GetWindowRect();
		m_pTexture->Render( p2DRender, CPoint(0, 0), CPoint(rcWindow.right, rcWindow.bottom ) );
	}
#else
	CRect rect = GetWindowRect();
	m_pTheme->RenderWndEditFrame( p2DRender, &rect );
#endif
}
BOOL CWndEdit::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	m_pApp->SetDeviceCursor( m_hEditCursor );
	return TRUE;
}
// 마우스 왼쪽 버튼을 에디트 창에 누르면 조합이 완료된다.
// 이는 조합시 다른 오동작을 방지하기 위해 클릭하면 무조건 조합을 완료하게 한 것이다.
void CWndEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
	// 조합 완료 매지시를 ime에게 보낸다.
	g_imeMgr.CompleteComposition(m_hWnd);
	CWndText::OnLButtonDown( nFlags, point );
}
// CWndText overide
// 캐럿을 그린다. 국가마다 캐럿모양이 다를 수 있다. 한글은 박스 형태의 캐럿모양이다.
void CWndEdit::DrawCaret(C2DRender* p2DRender)
{
	if( m_bCaretVisible )
	{
		CPoint pt1;
		int nFontBottom = m_pFont->GetMaxHeight();// - 1;
		pt1 = CPoint( m_ptCaret.x, m_ptCaret.y - 1 );
		int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() * ( p2DRender->m_pFont->GetMaxHeight() + m_nLineSpace ) : 0;
		pt1.y -= nPos;
		if( m_timerCaret.GetLeftTime() < 500 )
		{
			if( PRIMARYLANGID( g_imeMgr.m_hkl ) == LANG_KOREAN && g_imeMgr.GetCompositon(NULL, 0) ) 
			{

				char comp[64];
				int len = g_imeMgr.GetCompositon(comp, sizeof(comp));
				comp[len] = 0;

				SIZE size;
				m_pFont->GetTextExtent( comp, &size, g_imeMgr.m_codePage );

				p2DRender->RenderFillRect( CRect( pt1.x, pt1.y, pt1.x + size.cx, pt1.y + size.cy ), D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
				p2DRender->TextOut( m_ptCaret.x, m_ptCaret.y, comp, m_dwColor );
			}
			else
			{
				p2DRender->RenderFillRect( CRect( pt1.x, pt1.y, pt1.x + 2, pt1.y + nFontBottom ), m_dwColor );
			}
		}
		if( m_timerCaret.IsTimeOut() )
			m_timerCaret.Reset();
	}
}
// public CWndEdit functions
void CWndEdit::Empty()
{
	ClearAll();
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
}

void CWndEdit::OnKeyEnd()
{
	if( m_dwOffset < (DWORD)( m_string.GetLength() ))
	{
		BYTE chr, chr2;

		DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
		m_dwOffset = m_string.GetLineOffset( dwLine + 1 );

		if( m_dwOffset >= 1 )
			chr  = m_string.GetAt( m_dwOffset - 1 );
		if( m_dwOffset >= 2)
			chr2 = m_string.GetAt( m_dwOffset - 2 );

		if( m_dwOffset < (DWORD)( m_string.GetLength() ) || chr == '\n' )
		{
			if( IsDBCSLeadByte( chr ) && IsDBCSLeadByte( chr2 ) ) // KS 한글?
			{
				if( m_dwOffset > 1 ) m_dwOffset -= 2;
			}
			else
			{
				if( m_dwOffset > 0 ) m_dwOffset--;
			}
		}
	}
}
#ifdef __V050823_JAPAN_ATOK
LRESULT CWndEdit::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	CWndBase::WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_CLOSECANDIDATE: 
		g_imeMgr.NotifyIME( m_hWnd, NI_CLOSECANDIDATE,0,0);
		break;
	}
	return 1;
}
#endif
BOOL CWndEdit::IsYouMessage(UINT msg,WPARAM wparam, LPARAM lparam)
{
	if( !m_hWnd )//|| !m_bAct ) 
		return FALSE;
	if( m_pWndCandList == NULL && m_pWndHCandList == NULL && m_pWndReadingList == NULL )
	{		
#ifdef __V050913_TABWND
		if(GetFocusWnd() != GetFrameWnd())
#else
		if(GetFocusWnd() != GetParentWnd())
#endif
			return FALSE;
		if(GetParentWnd()->GetFocusChild() != this)
			return FALSE;
	}
	switch( msg )
	{ 
/*
	case WM_INPUTLANGCHANGE:       // 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
		OnInputLangChange(wparam, lparam);
		return TRUE;
*/
    case WM_INPUTLANGCHANGE:       // 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
		{
			if( PRIMARYLANGID((WORD)lparam) != LANG_ENGLISH) 
				OnInputLangChange(wparam,lparam);
			else
				::DefWindowProc(m_hWnd, WM_INPUTLANGCHANGE, wparam, lparam);
			
			return TRUE;
		}
	case WM_IME_STARTCOMPOSITION:     // 글자 조합의 시작. WM_IME_COMPOSITION 메시지를           // 받을 준비를 한다. 
		OnIMEStartComposition(wparam,lparam);
		return TRUE;    
	case WM_IME_COMPOSITION:          // 현재 조합중인 문자열이나 완성된 문자열을 얻어서 화면 출력 
		OnIMEComposition(wparam,lparam);
		return TRUE;    
	case WM_IME_ENDCOMPOSITION:          // 글자 조합의 끝. 조합된 문자열 처리를 끝낸다          break; 
		HideReading();
		OnIMEEndComposition(wparam,lparam);
		TRACE( "WM_IME_ENDCOMPOSITION\n" );
		return TRUE;    
	case WM_IME_SETCONTEXT:						// 글자 조합 윈도우와 한자 변환 윈도우를 표시하지 않게 바꿈
		OnIMESetContext(wparam,lparam);
		return TRUE;     
	case WM_IME_NOTIFY:        
		OnIMENotify(wparam,lparam);
		return TRUE;             
	case WM_IME_CONTROL:
		OnIMEControl(wparam,lparam);
		return TRUE;    
	case WM_IME_COMPOSITIONFULL:
		OnIMECompositionFull(wparam,lparam);
		return TRUE;    
	case WM_CHAR:
		if( wparam != VK_RETURN )
			OnChar_(wparam);
		break;
	  case WM_KEYDOWN:
		{
//			int	i;
			TRACE( "WM_KEYDOWN %x\n", wparam );
			switch( wparam )
			{
				case 3: // Ctrl-C : Copy
					break;

				case 22: // Ctrl-V : Paste
					break;

				case 24: // Ctrl-X : Paste
					break;

				case VK_BACK:
					if(!IsEmptyBlock())
					{
						DeleteBlock();
					} 
					else
					{
						m_dwBlockBegin = m_dwOffset;
						m_dwBlockEnd = m_dwOffset;
						if(KeyLeft()) 
						{
							m_dwBlockBegin = m_dwOffset;
							DeleteBlock();
						}
					}
					break;

				case VK_LEFT:
					KeyLeft();
					break;

				case VK_RIGHT:
					KeyRight();
					break;

				case VK_DELETE:
					if(!IsEmptyBlock())
					{
						DeleteBlock();
					} 
					else
					{
						m_dwBlockBegin = m_dwOffset;
						m_dwBlockEnd = m_dwOffset;
						if(KeyRight()) 
						{
							m_dwBlockEnd = m_dwOffset;
							DeleteBlock();
						}						
					}
					break;

				case VK_DOWN:
					break;
				case VK_UP:
#ifdef _DEBUG
					{
						if( m_pParentWnd->OnChildNotify( EN_UP, m_nIdWnd, (LRESULT*)this ))
						{
							OnKeyEnd();
							CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
							SetCaretPos( ptCaret );
							return FALSE;
						}
					}
#endif // _DEBUG
					break;

				case VK_HOME:
					{
						DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
						m_dwOffset = m_string.GetLineOffset( dwLine );
					}
					break;

				case VK_END:
					OnKeyEnd();
					break;

				case VK_RETURN:
					if( IsWndStyle( EBS_WANTRETURN ) )
					{
						m_string.Insert( m_dwOffset, '\n', EDIT_COLOR, 0, g_imeMgr.m_codePage );
						m_dwOffset++;
						CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
						SetCaretPos( ptCaret );
					}
#ifdef __VBUGFIX_20050201
					CloseCandidate();
					g_imeMgr.CancelComposition( m_hWnd );
#endif
					m_pParentWnd->OnChildNotify( EN_RETURN, m_nIdWnd, (LRESULT*)this ); 
					return FALSE;
				case VK_ESCAPE:
					break;
			}
			CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
			SetCaretPos( ptCaret );
			OnKeyDown(wparam, LOWORD(lparam), HIWORD(lparam));
		}
		case WM_KEYUP:
			TRACE( "WM_KEYUP %x\n", wparam );
			if( wparam == 0x0d )
			{
				int a = 0;
				a++;
			}
			OnKeyUp(wparam, LOWORD(lparam), HIWORD(lparam));
			return TRUE;
			break;
  }
	return FALSE;
}
// 알파벳 모드로 변경한다. 모드 플렉이 있으므로 상태를 알수 있다.
// 모드를 알고 있어야 ime이 리셋후에 다시 모드를 복원할 수 있다.
void CWndEdit::SetAlphaNumericMode()
{
	m_bImeNativeMode = FALSE;
	SetAlphaNumeric();
}
// 국가 모드로 변경한다. 모드 플렉이 있으므로 상태를 알수 있다.
// 모드를 알고 있어야 ime이 리셋후에 다시 모드를 복원할 수 있다.
void CWndEdit::SetNativeMode()
{
	m_bImeNativeMode = TRUE;
	SetNative();
}
// ime 언어 변경 여부를 세팅한다.
// 특정 언어로 유지하게 하는 기능을 한다.
// 영문에서 한글로 변경하지 못하게 한다던가..
void CWndEdit::EnableModeChange( BOOL bEnable )
{
	if( bEnable )
	{
		if( m_hIMCBackup )
		{
			g_imeMgr.AssociateContext(m_hWnd, m_hIMCBackup);
			m_hIMCBackup = NULL;
		}
	}
	else
	{
		if( m_hIMCBackup == NULL )
		{
			m_hIMCBackup = g_imeMgr.AssociateContext(m_hWnd, NULL);
		}
	}
	m_bEnableModeChange = bEnable;
}
///////////////////////////////////////////////////////////////
// public static
// 알파벳 상태로 변경한다. 
void CWndEdit::SetAlphaNumeric()
{
	g_imeMgr.SetOpenStatus(m_hWnd, FALSE);
}

// 현재 기본으로 세팅된 국가 언어 상태로 변경한다.
void CWndEdit::SetNative()
{
	g_imeMgr.SetOpenStatus(m_hWnd, TRUE);
}
///////////////////////////////////////////////////////////////
// private 
void CWndEdit::DeleteBlock( )
{
	if( !IsEmptyBlock() )
	{
		CRect rect = GetClientRect();
		m_string.Init( m_pFont, &rect );
		
		DWORD dwBlockBegin = min(m_dwBlockBegin, m_dwBlockEnd);
		DWORD dwBlockEnd = max(m_dwBlockBegin, m_dwBlockEnd);

		m_string.Delete( dwBlockBegin, dwBlockEnd - dwBlockBegin );

		m_dwOffset = dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd = 0;

		m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	}
}
///////////////////////////////////////////////////////////////
// for IME Control Message
LONG CWndEdit::OnInputLangChange(WPARAM dwCommand, LPARAM dwData)
{
	g_imeMgr.InputLangChange(m_hWnd, (HKL)dwData);
//	Empty();

	g_imeMgr.ClearComposition();
	
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 

	return ::DefWindowProc(m_hWnd, WM_INPUTLANGCHANGE, dwCommand, dwData);
}
// WM_IME_STARTCOMPOSITION
LONG CWndEdit::OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData)
{
	m_stringBack = m_string;
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );

	return TRUE;
}
// WM_IME_COMPOSITION
LONG CWndEdit::OnIMEComposition(WPARAM dwCommand, LPARAM dwData)
{
	g_imeMgr.Composition(m_hWnd, dwCommand, dwData);

	if (dwData & GCS_RESULTSTR)
		GetResultString();
	
	if (dwData & GCS_COMPSTR)
		GetCompString(dwData);

	return TRUE;
}
// WM_IME_ENDCOMPOSITION
LONG CWndEdit::OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData)
{
	g_imeMgr.Composition(m_hWnd, dwCommand, GCS_RESULTSTR);

	if( g_imeMgr.GetResultSize() == 0 )
		m_string = m_stringBack;
	g_imeMgr.ClearComposition();

	return TRUE;
}
// WM_IME_SETCONTEXT
LONG CWndEdit::OnIMESetContext(WPARAM dwCommand, LPARAM dwData)
{
	if (g_imeMgr.m_property & IME_PROP_AT_CARET) {
		dwData &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
	}
	return ::DefWindowProc(m_hWnd, WM_IME_SETCONTEXT, dwCommand, dwData);
}
// WM_IME_COMPOSITIONFULL
LONG CWndEdit::OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData)
{
	return 0;
}
// WM_IME_NOTIFY
LONG CWndEdit::OnIMENotify(WPARAM dwCommand, LPARAM dwData)
{
    switch (dwCommand) {
        case IMN_OPENCANDIDATE:
 			return OpenCandidate();

        case IMN_CLOSECANDIDATE:
			return CloseCandidate();

        case IMN_CHANGECANDIDATE:
			return ChangeCandidate();

        case IMN_SETOPENSTATUS:
			return SetOpenStatus();

		case IMN_PRIVATE:
			g_imeMgr.Reading(m_hWnd);
			if(g_imeMgr.GetReading(NULL, 0) > 0)
				ShowReading();
			else
				HideReading();
			if(g_imeMgr.Private(m_hWnd, dwData)) {
				return TRUE;
			}
			break;

	}
    return ::DefWindowProc(m_hWnd,WM_IME_NOTIFY, dwCommand, dwData);
}
// WM_IME_CONTROL
LONG CWndEdit::OnIMEControl(WPARAM dwCommand, LPARAM dwData)
{
	return 0;
}
// WM_CHAR
void CWndEdit::OnChar_(UINT nChar)
{
	if( !IsWndStyle( EBS_AUTOHSCROLL ) && !IsWndStyle( EBS_AUTOVSCROLL ) )
	{
		CString strTemp = m_string + (CHAR)nChar; // 길이를 확인하기 위해서 일단 뒤에 str을 추가한다.
		if( m_pFont->GetTextExtent( strTemp ).cx >= GetClientRect().Width() )
			return;
	}
	TRACE( "CWndEdit::OnChar_(UINT nChar = %x)\n",nChar );

	if( ( nChar == 8 ) || ( nChar == 27 )) // backspace
		return;
	
	if( nChar == 3 ) // CTRL-C : copy
	{
		if( m_dwBlockBegin != m_dwBlockEnd  && m_bEnableClipboard)
		{
			DWORD dwBlockBegin = min(m_dwBlockBegin, m_dwBlockEnd);
			DWORD dwBlockEnd = max(m_dwBlockBegin, m_dwBlockEnd);

			CString strClipboard = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );

			CClipboard::SetText( strClipboard );
		}
		return;
	}
	if( nChar == 22 && m_bEnableClipboard ) // Ctrl-V : Paste
	{
		DeleteBlock();

		CHAR* buf;
		DWORD dwSize;
		CClipboard::GetTextLength( &dwSize );
		if( dwSize )
		{
			buf = new CHAR[ dwSize + 1 ];
			CClipboard::GetText( buf, dwSize + 1 );
			buf[ dwSize ] = 0; 
			CRect rect = GetClientRect();
			m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );
			m_string.Insert( m_dwOffset, buf, EDIT_COLOR, 0, g_imeMgr.m_codePage );
			m_dwOffset += dwSize - 1;
			m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
			delete buf;
		}
		return;
	}
	if( nChar == 24 ) // Ctrl-X : Paste
	{
	}
	else
	if( nChar != 9 ) // !tab
	{
		DeleteBlock();

		// 콘트롤 문자면 리턴 
		if( iscntrl( nChar ) )
			return;

		if( g_imeMgr.m_codePage == 874 )
		{
			UINT prevChar;

			if(m_dwOffset)
				prevChar = (BYTE)m_string.GetAt(m_dwOffset-1);
			else 
				prevChar = 0;

			if(!IsComposibleTh(prevChar, nChar, 1) )
				return;
		}


		CRect rect = GetClientRect();
		m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );

		m_string.Insert( m_dwOffset, nChar, EDIT_COLOR, 0, g_imeMgr.m_codePage );
		m_dwOffset++;
		m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	}
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
}
// GCS_RESULTSTR
BOOL CWndEdit::GetResultString()
{
    DWORD	len = g_imeMgr.GetResult(NULL, 0);
    LPSTR	str = (LPSTR)_alloca(len + 1);

	g_imeMgr.GetResult(str, len);
	str[len] = 0;

	/* Update m_string */ 
	m_string = m_stringBack;
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	
	for( int i = 0 ; i < (int)( len ); i++)
	{
		m_string.Insert( i + m_dwOffset, str[ i ], EDIT_COLOR, 0, g_imeMgr.m_codePage );
	}
	m_dwOffset+=len;

	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	m_stringBack = m_string;

	return TRUE;
}
// GCS_COMPSTR
BOOL CWndEdit::GetCompString(LONG flag)
{
    int len = g_imeMgr.GetCompositon(NULL, 0);
    LPSTR str = (LPSTR)_alloca(len);
    BYTE* attr = (BYTE*)_alloca(len);

    g_imeMgr.GetCompositon(str, len);

	if (flag & GCS_COMPATTR) {
		g_imeMgr.GetAttribute(attr, len);
	} else {
		memset(attr, 0, len);
	}

	int curPos = (flag & GCS_CURSORPOS) ? g_imeMgr.GetCursorPos() : 0;

	/* Update m_string */ 
	m_string = m_stringBack;

	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );

	int inputPos = len;

	DWORD dwInputColor = (PRIMARYLANGID(g_imeMgr.m_hkl)==LANG_KOREAN) ? 0xff000000 : 0xff0000ff;

	for(int i=0; i<len; i++)
	{
		DWORD dwColor = 0xff000000;
		DWORD dwStyle = 0;
		switch ( attr[ i ] )
		{
			case ATTR_INPUT:
				dwColor = dwInputColor;
				break;
			case ATTR_TARGET_CONVERTED:
				inputPos = min(inputPos, i);
				dwColor = 0xff0000ff;
				dwStyle = ESSTY_BOLD;
				break;
#ifdef __V050823_JAPAN_ATOK
			case ATTR_TARGET_NOTCONVERTED:
				inputPos = min(inputPos, i);
				dwColor = 0xff0000ff;
				dwStyle = ESSTY_UNDERLINE | ESSTY_BOLD;
				break;
#endif
			case ATTR_CONVERTED:
				dwColor = 0xff0000ff;
				break;
		}
		m_string.Insert( m_dwOffset + i, str[ i ], dwColor, dwStyle, g_imeMgr.m_codePage );
	}

	m_ptInputPos = OffsetToPoint( m_dwOffset + inputPos, m_szCaret );
	
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	CPoint ptCaret = OffsetToPoint( m_dwOffset + curPos, m_szCaret );

	SetCaretPos( ptCaret );

	return TRUE;
}
// IMN_OPENCANDIDATE:
BOOL CWndEdit::OpenCandidate()
{
  	if(g_imeMgr.IsVerticalCandidate())
	{
		if(m_pWndCandList == NULL) 
		{
 			m_bKeepComposition = TRUE;
			m_pWndCandList = new CWndCandList;
			m_pWndCandList->Initialize( this );
			m_bKeepComposition = FALSE;
		}
	}
	else
	{
		if(m_pWndHCandList == NULL) 
		{
			m_bKeepComposition = TRUE;
			m_pWndHCandList = new CWndHCandList;
			m_pWndHCandList->Initialize( this );
			m_bKeepComposition = FALSE;
		}
	}

	ChangeCandidate();

	return TRUE;
}
// IMN_CHANGECANDIDATE:
BOOL CWndEdit::ChangeCandidate()
{
	HideReading();

	g_imeMgr.Candidate(m_hWnd);

	CANDIDATELIST candidateList;

   	if(g_imeMgr.GetCandidateList(&candidateList)) {

		CRect rectScreen = GetScreenRect();
		CPoint candidatePos( rectScreen.left + m_ptInputPos.x, rectScreen.top + m_ptInputPos.y );

		if(g_imeMgr.IsVerticalCandidate())
		{
			if( m_pWndCandList )
				m_pWndCandList->UpdateCandList(candidatePos);
		}
		else
		{
			if( m_pWndCandList )
				m_pWndHCandList->UpdateCandList(candidatePos);
		}
	}
	return TRUE;
}

// IMN_CLOSECANDIDATE:
BOOL CWndEdit::CloseCandidate()
{
#ifdef __V050823_JAPAN_ATOK
	SAFE_DELETE( m_pWndCandList );
	SAFE_DELETE( m_pWndHCandList );
#else
	if( m_pWndCandList )
		m_pWndCandList->Destroy();
	if( m_pWndHCandList )
		m_pWndHCandList->Destroy();
#endif
	g_imeMgr.ClearCandidate();
	
	return TRUE;
}

// IMN_SETOPENSTATUS:
BOOL CWndEdit::SetOpenStatus()
{
	m_bImeNativeMode = g_imeMgr.GetOpenStatus(m_hWnd);

	return TRUE;
}
void CWndEdit::ShowReading()
{
	char reading[16];
	int readingLen = g_imeMgr.GetReading(reading, sizeof(reading));
	reading[readingLen] = 0;

	if( m_pWndReadingList == NULL )
	{
		m_bKeepComposition = TRUE;
		m_pWndReadingList = new CReadingList;
		m_pWndReadingList ->Initialize( this );//GetParentWnd() );
		m_bKeepComposition = FALSE;
	}

	CRect rectScreen = GetScreenRect();
	CPoint candidatePos( rectScreen.left + m_ptInputPos.x, rectScreen.top + m_ptInputPos.y );

	m_pWndReadingList->UpdateReading(candidatePos);
}
void CWndEdit::HideReading()
{
	if( m_pWndReadingList )
		m_pWndReadingList->Destroy();
	g_imeMgr.ClearReading();
}
bool CWndEdit::KeyLeft()
{
	WORD codePage = g_codePage;

	const char* begin = m_string;
	const char* end = begin + m_string.GetLength();
	const char* iter = begin;

	while(*iter && iter < end)
	{
		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		const char* next = CharNextEx(iter, wCodePage);
		if( next - begin >= (int)( m_dwOffset ) )
		{
			m_dwOffset = iter - begin;
			return true;
		}
		iter = next;
	}

	m_dwOffset = iter - begin;
	return false;		
}
bool CWndEdit::KeyRight()
{
	const char* begin = m_string;
	const char* end = begin + m_string.GetLength();
	const char* iter = begin;

	while(*iter && iter < end)
	{
		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		iter = CharNextEx(iter, wCodePage);
		if( iter - begin > (int)( m_dwOffset ) )
		{
			m_dwOffset = iter - begin;
			return true;
		}
	}

	m_dwOffset = iter - begin;
	return false;		
}
void CWndEdit::ClearAll()
{
	g_imeMgr.ClearComposition();

	m_string.Empty();
#ifdef __VBUGFIX_20050201
	m_stringBack.Empty();
#endif
	m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = 0;

	SetCaretPos( CPoint(0,0) );

	m_wndScrollBar.SetMinScrollPos();
}
/*----------------------------------------------------------------------------*/ 
#endif //_MULTI_LANGUAGE_