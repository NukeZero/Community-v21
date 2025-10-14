

#include "StdAfx.h"
#include "WndControl_Text.h"

#include "ClipBoard.h"

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndText::CWndText()
:	m_string( _T("") )
,	m_nLineSpace( 5 )
,	m_bEnableClipboard( FALSE )
,	m_ptDeflateRect( 6, 6 )
,	m_bScr( FALSE )
,	m_bCaretVisible( FALSE )
,	m_bLButtonDown( FALSE )
,	m_nLineRefresh( NULL )
,	m_dwBlockBegin( NULL )
,	m_dwBlockEnd( NULL )
,	m_dwOffset( NULL )
,	m_ptCaret( 0, 0 )
{
	m_byWndType		= WTYPE_TEXT;
	m_strTexture	= DEF_CTRL_TEXT;
	m_bTile			= TRUE;

	if( g_xFlyffConfig->GetMainLanguage() == LANG_FRE )
		m_nLineSpace = 0;

	ZeroMemory( &m_TextComp,	sizeof( TCHAR ) * 3 );
	ZeroMemory( &m_szCaret,		sizeof( TCHAR ) * 3 );

	m_timerCaret.Set(1000);
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndText::~CWndText()
{
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndText::Create( DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID )
{
	FLERROR_ASSERT_LOG_RETURN( pParentWnd,	NULL, _T("[윈도우 텍스트] 부모 윈도우 포인터 유효하지 않음.") );	

	PFONTCOLOR_WNDTEXT pFontColorWndText = g_WndFontColorManager->GetWndText();

	BYTE byR = pFontColorWndText->m_stDefault.GetFontColorR();
	BYTE byG = pFontColorWndText->m_stDefault.GetFontColorG();
	BYTE byB = pFontColorWndText->m_stDefault.GetFontColorB();
	
	SetFontColor( byR, byG, byB );
	return CWndBase::Create( dwStyle | WBS_CHILD, rect, pParentWnd, nID );
}

//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndText::Create( DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID,  int nFontColorR, int nFontColorG, int nFontColorB )
{
	FLERROR_ASSERT_LOG_RETURN( pParentWnd,	NULL, _T("[윈도우 텍스트] 부모 윈도우 포인터 유효하지 않음.") );	

	SetFontColor( nFontColorR, nFontColorG, nFontColorB );
	return CWndBase::Create( dwStyle | WBS_CHILD, rect, pParentWnd, nID );
}


//---------------------------------------------------------------------------------------------
// On Initialize Update ( 초기화 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::OnInitialUpdate()
{
	m_wndMenu.CreateMenu( this );	

	if( g_xFlyffConfig->GetMainLanguage() == LANG_FRE )
	{
		CRect Rect = GetWndRect();
		Rect.bottom	+= 2;
		SetWndRect( Rect, TRUE );
	}

	m_string.Init( m_pFont, &GetClientRect() );

	CRect Rect = GetClientRect();
	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, Rect, this, 1000 );
	UpdateScrollBar();
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	
	m_bScr = TRUE;
	m_nLineRefresh = 0;
}


//---------------------------------------------------------------------------------------------
// Reset String ( 문자열 리셋 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::ResetString()
{
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
	UpdateScrollBar();
}


//---------------------------------------------------------------------------------------------
// Update Scroll Bar ( 스크롤 바 갱신 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::UpdateScrollBar()
{
	if(	IsWndStyle( WBS_VSCROLL ) )
	{
		int nPage( GETRANGE );
		int nRange( m_string.GetLineCount() );

		if(	IsWndStyle( WBS_VSCROLL ) )  
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );

		if( m_wndScrollBar.GetScrollPos() > ( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() ) )
			m_wndScrollBar.SetMaxScrollPos();
	}
}


//---------------------------------------------------------------------------------------------
// Insert ( 삽입 )
// param	: nIndex, pstr
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::Insert( int nIndex, LPCTSTR pstr )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pstr, _T("[윈도우 텍스트] 문자열 포인트가 유효하지 않음") );

	int nLine = m_string.GetLineCount() - 1;

	m_string.Insert( nIndex, pstr );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();

	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );	
}


//---------------------------------------------------------------------------------------------
// Delete ( 삭제 )
// param	: nIndex, nLen
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::Delete( int nIndex, int nLen )
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.Delete( nIndex, nLen );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );		
}


//---------------------------------------------------------------------------------------------
// Set String ( 문자열 설정 )
// param	: pszString
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::SetString( LPCTSTR pszString )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pszString, _T("[윈도우 텍스트] 문자열 포인트가 유효하지 않음") );

	m_string.Init( m_pFont, &GetClientRect() );
	m_string.SetParsingString( pszString, m_dwColor );

	if( IsWndStyle( WBS_VSCROLL ) )
		UpdateScrollBar();

	m_dwOffset = m_string.GetLength();

	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
	
	m_dwBlockBegin = m_dwBlockEnd = 0;
}


//---------------------------------------------------------------------------------------------
// Set String ( 문자열 설정 )
// param	: pszString, dwColor
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::SetString( LPCTSTR pszString, DWORD dwColor )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pszString, _T("[윈도우 텍스트] 문자열 포인트가 유효하지 않음") );

	m_string.Init( m_pFont, &GetClientRect() );
	m_string.SetParsingString( pszString, dwColor );

	if( IsWndStyle( WBS_VSCROLL ) )
		UpdateScrollBar();

	m_dwOffset = m_string.GetLength();

	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
	
	m_dwBlockBegin = m_dwBlockEnd = 0;
}


//---------------------------------------------------------------------------------------------
// Add String ( 문자열 추가 )
// param	: pszString
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::AddString( LPCTSTR pszString )
{
	int nLine = m_string.GetLineCount() - 1;

	m_string.AddParsingString( pszString, m_dwColor );

	UpdateScrollBar();

	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
}


//---------------------------------------------------------------------------------------------
// Add String ( 문자열 추가 )
// param	: pszString, dwColor
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::AddString(LPCTSTR pszString, DWORD dwColor )
{
	int nLine = m_string.GetLineCount() - 1;

	m_string.AddParsingString( pszString, dwColor );

	UpdateScrollBar();

	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
}


//---------------------------------------------------------------------------------------------
// Add String ( 문자열 추가 )
// param	: pszString, dwColor, dwPStyle
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndText::AddString(LPCTSTR pszString, DWORD dwColor, DWORD dwPStyle )
{
	int nLine = m_string.GetLineCount() - 1;

	m_string.AddParsingString( pszString, dwColor, 0x00000000, 0, dwPStyle );

	UpdateScrollBar();

	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
}



void CWndText::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	rect = GetWindowRect();
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );
	UpdateScrollBar();
	CWndBase::OnSize( nType, cx, cy);
}


void CWndText::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	if( !IsWndStyle( WBS_NOFRAME ) )
		m_rectClient.DeflateRect( m_ptDeflateRect.x, m_ptDeflateRect.y );

	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
	
}


void CWndText::OnDraw( C2DRender* p2DRender )
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine( m_dwOffset );
	CPoint ptCaret = 0;
	DWORD dwLineCount = m_string.GetLineCount();
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( m_pFont );
	
	int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() : 0;
	int nLines = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPage() : GETRANGE;// GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();

	int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
	int nRange = m_string.GetLineCount();

	if(	IsWndStyle( WBS_VSCROLL ) ) 
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		if( GetParentWnd()->m_nIdWnd == APP_MESSAGE_NOTE )
		{
			SetWndRect( GetWindowRect( TRUE ), FALSE);
			if( m_nLineRefresh < (int)( m_string.GetLineCount() ) )
			{
				m_wndScrollBar.SetScrollDown();
				m_nLineRefresh = m_string.GetLineCount();
			}
		}
	}
	else
		m_wndScrollBar.SetVisible( FALSE );

	if( IsWndStyle( EBS_PASSWORD ) )
	{
		for(int i = nPos, y = 0; i < (int)( dwLineCount ); i++, y++)
		{
			dwOffset = m_string.GetLineOffset( i );
			string = m_string.GetLine(i);
			dwBegin = 0;
			int nLength = string.GetLength();
			const char* begin = string;
			const char* end = begin + string.GetLength();
			const char* iter = begin;

			while(*iter) {

				WORD wCodePage = m_string.m_awCodePage[dwCurOffset];

				dwCurOffset = dwOffset + (iter - begin);

				const char* next = CharNextEx( iter, wCodePage );

				strHan[0] = '*';
				strHan[1] = 0;

				CSize size;
				p2DRender->m_pFont->GetTextExtent( "*", &size );
				size.cx *= next - iter;

				DWORD dwColor = m_string.m_adwColor[dwCurOffset];
				DWORD dwStyle = m_string.m_abyStyle[dwCurOffset];

				if( m_bEnableClipboard )
				{
					DWORD dwBlockBegin;
					DWORD dwBlockEnd;
					if(m_dwBlockBegin > m_dwBlockEnd)
						dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
					else
						dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;


					if(dwCurOffset >= dwBlockBegin && dwCurOffset < dwBlockEnd)
					{
						DWORD dwBkgr = dwColor;
						dwColor = ~dwColor | 0xff000000;
						p2DRender->RenderFillRect(CRect( dwBegin, 0 + y * dwMaxHeight, dwBegin + size.cx, 0 + y * dwMaxHeight + size.cy), dwBkgr );
					}
				}
				p2DRender->TextOut( dwBegin, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_BOLD )
					p2DRender->TextOut( dwBegin + 1, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_UNDERLINE )
					p2DRender->RenderLine( CPoint( dwBegin, y * dwMaxHeight + size.cy ), CPoint( dwBegin + size.cx, y * dwMaxHeight + size.cy ), dwColor );
				

				// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
				if(dwCurOffset == m_dwOffset)
				{
					FLStrcpy( m_szCaret, _countof( m_szCaret ), strHan );
					ptCaret = CPoint( dwBegin, 0 + y * dwMaxHeight );
				}
				dwBegin += size.cx;

				iter = next;
			}
		}
		 //m_ptCaret = ptCaret; 
		 //m_timerCaret.Reset(); 
	}
	else
	{
		if( nLines == 6 )
		{
			int a = 0;
		}
		BlockSetStyle(ESSTY_BLOCK);
		p2DRender->TextOut_EditString( 0, 0, m_string, nPos, nLines, m_nLineSpace );
	}

	p2DRender->SetFont( pOldFont );
	DrawCaret( p2DRender );
}


void CWndText::BlockSetStyle(DWORD dwStyle)
{
	if( m_bEnableClipboard )
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if(m_dwBlockBegin > m_dwBlockEnd) 
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if( dwBlockEnd - dwBlockBegin )
		{
			m_string.SetStyle( dwBlockBegin, dwBlockEnd - dwBlockBegin, dwStyle );
		}
	}	
}


void CWndText::BlockSetColor( DWORD dwColor )
{
	if( m_bEnableClipboard )
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if(m_dwBlockBegin > m_dwBlockEnd) 
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if( dwBlockEnd - dwBlockBegin )
		{
			m_string.SetColor( dwBlockBegin, dwBlockEnd - dwBlockBegin, dwColor );
		}
	}	
}


void CWndText::BlockClearStyle(DWORD dwStyle)
{
	if( m_bEnableClipboard )
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if(m_dwBlockBegin > m_dwBlockEnd) 
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if( dwBlockEnd - dwBlockBegin )
		{
			m_string.ClearStyle( dwBlockBegin, dwBlockEnd - dwBlockBegin, dwStyle );
		}
	}	
}




CPoint CWndText::OffsetToPoint( DWORD dwSetOffset, TCHAR* pszStr, size_t cchStr )
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine( m_dwOffset );
	CPoint ptCaret( 0, 0);
	for(int i = m_wndScrollBar.GetScrollPos(); i < (int)( m_string.GetLineCount() ); i++)
	{
		dwOffset = m_string.GetLineOffset( i );
		string = m_string.GetLine(i);
		dwBegin = 0;
		strHan[0] = 0;
		const char* begin = string;
		const char* end = begin + string.GetLength();
		const char* iter = begin;

		while( *iter && iter < end ) {

			WORD wCodePage = m_string.m_awCodePage[dwOffset + (iter-begin)];

			const char* next = CharNextEx(iter, wCodePage );

			SIZE size;

			if( IsWndStyle( EBS_PASSWORD ) ) {
				m_pFont->GetTextExtent("*", &size);
				size.cx *= next-iter;
			} else {
				char temp[16];
				memcpy(temp, iter, next-iter);
				temp[next-iter] = 0;
				m_pFont->GetTextExtent(temp, &size, wCodePage);
			}

			dwCurOffset = dwOffset + (iter-begin);

			// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
			if( dwCurOffset == dwSetOffset )
			{
				if( pszStr )
					FLStrcpy( pszStr, cchStr, strHan );
				return CPoint( dwBegin, 0 + i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			dwBegin += size.cx;
			iter = next;
		}

		// 캐럿이 문장 내에 있지 않고 문장의 끝에 새로운 입력을 기다려야하는 위치이다.
		if( dwSetOffset == m_string.GetLength() )
		{
			if( !m_dwOffset || m_string.GetAt( m_dwOffset - 1 ) != '\n' )
			{
				if( pszStr )
					pszStr[0] = 0;
				ptCaret = CPoint( dwBegin, i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			else
			{
				if( pszStr )
					pszStr[0] = 0;
				ptCaret = CPoint( 0, ( i + 1 ) * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
		}
	}
	return ptCaret;
}


LONG CWndText::GetOffset(CPoint point)
{
	CString string;
	int dwMaxHeight = GetFontHeight();
	CPoint pt = point;
	pt.y /= dwMaxHeight;
	pt.y += m_wndScrollBar.GetScrollPos();
	DWORD dwOffset1 = m_string.GetLineOffset( pt.y );
	DWORD dwOffset2 = m_string.GetLineOffset( pt.y + 1);
	DWORD dwBegin = 0;
	const char* begin = m_string;
	const char* end = begin + dwOffset2;
	const char* iter = begin + dwOffset1;

	while(*iter && iter < end) {

		if( *iter == '\r' || *iter == '\n')
		{
			return iter-begin;
		}

		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		const char* next = CharNextEx( iter, wCodePage );

		SIZE size;

		if( IsWndStyle( EBS_PASSWORD ) )
		{
			m_pFont->GetTextExtent("*", &size);
			size.cx *= next-iter;
		}
		else
		{
			char temp[16];
			memcpy(temp, iter, next-iter);
			temp[next-iter] = 0;
			m_pFont->GetTextExtent(temp, &size, wCodePage);
		}

		if( (int)( dwBegin+size.cx ) > pt.x)
		{
			//gmpbigsun( 20100818 ) : pt.x가 음수가 되다면 마우스가 보다 앞에있음에도 불구하고 next-begin으로 계산되어진다.
			// 절대값으로 바꺼서 계산해야만 올바른 결과가 도출된다.
			 int absX = abs( pt.x );
			//if(pt.x-dwBegin < (dwBegin+size.cx)-pt.x)
			if(absX-dwBegin < (dwBegin+size.cx)-absX)		//#9849
				return iter-begin;
			else 
				return next-begin;
		}

		dwBegin += size.cx;
		iter = next;
	}
	return iter-begin;
}
void CWndText::OnChar( UINT nChar  )
{
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
	}
}
void CWndText::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_string.ClearStyle( ESSTY_BLOCK );
	LONG lOffset = GetOffset(point);
	if(lOffset != -1)
		m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = lOffset; 
	m_bLButtonDown = TRUE;
	SetCapture();
	m_TextComp[ 0 ] = 0;
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
	SetCaretPos( ptCaret );
}
void CWndText::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_string.ClearStyle( ESSTY_BLOCK );
	m_bLButtonDown = FALSE;
	ReleaseCapture();
	CWndBase* pWnd = m_pParentWnd;
	pWnd->OnChildNotify( STN_CLICKED, m_nIdWnd, (LRESULT*)this); 
}
void CWndText::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( g_pPlayer && g_pPlayer->HasBuff( BUFF_ITEM, ITEM_INDEX( 30011, II_SYS_SYS_SCR_FONTEDIT ) ) )
	{
		m_wndMenu.DeleteAllMenu();
		m_wndMenu.AppendMenu( 0, MGI_APPELL_UP,		prj.GetText( TID_GAME_FONT_EDIT ) );
		m_wndMenu.Move( CPoint( m_rectCurrentWindow.left, m_rectCurrentWindow.top ) + point );
		m_wndMenu.SetVisible( TRUE );//!m_wndMenuMover.IsVisible() );
		m_wndMenu.SetFocus();
	}
}

BOOL CWndText::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
//	m_wndMenu.SetVisible( FALSE );
	return CWndBase::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndText::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	if( pWndBase == &m_wndMenu )
	{
		switch( nID )
		{
		case MGI_APPELL_UP:
			SAFE_DELETE(g_WndMng.m_pWndFontEdit);
			g_WndMng.m_pWndFontEdit = new CWndFontEdit;
			g_WndMng.m_pWndFontEdit->Initialize();
			g_WndMng.m_pWndFontEdit->m_pWndText = this;
			break;
		} 
	}
	
	m_wndMenu.SetVisible(FALSE);

	return CWndBase::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndText::OnMouseMove(UINT nFlags, CPoint point)
{
	//return;
	if(m_bLButtonDown == TRUE)
	{
		m_string.ClearStyle( ESSTY_BLOCK );
		LONG lOffset = GetOffset(point);

		if(lOffset != -1)
			m_dwBlockEnd = m_dwOffset = lOffset; 
	}
	else
	{
		int dwOffset;
		int dwBlockBegin;
		int dwBlockEnd;
		LONG lOffset = GetOffset( point );
		BYTE chr1, chr2;

		if(lOffset >= 0 && lOffset < m_string.GetLength() ) 
		{
			if( !IsWhite( m_string.GetAt( lOffset ) ) ) 
			{
				dwBlockBegin = lOffset;
				dwBlockEnd = lOffset;

				while( dwBlockBegin > 1 )
				{
					chr1 = m_string.GetAt ( dwBlockBegin - 1 );
					chr2 = m_string.GetAt ( dwBlockBegin - 2 ); 

					if( IsDBCSLeadByte( chr1 ) && IsDBCSLeadByte( chr2 ) )
						dwBlockBegin -= 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockBegin -= 1;
					else
						break;
				}
				/*
				while(dwBlockEnd < m_string.GetLength() )
				{
					chr1 = m_string.GetAt ( dwBlockEnd );

					if( IsDBCSLeadByte( chr1 ) )
						dwBlockEnd += 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockEnd += 1;
					else
						break;
				}
				*/
				dwOffset = dwBlockEnd; 

				CString strFind = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
				/*
				CStringArray* pStrArray = prj.GetHelp( strFind );
				if(pStrArray)
				{
					GET_CLIENT_POINT( m_pApp->GetSafeHwnd(), point);
					CPoint pt1 = OffsetToPoint(dwBlockBegin);
					CPoint pt2 = OffsetToPoint(dwBlockEnd);
					g_toolTip.PutToolTip( 100, pStrArray->GetAt(0), CRect
						(	pt1.x, pt1.y, pt2.x, pt2.y + m_pFont->GetMaxHeight() )
						, point);
				}
				*/
			}
		}
	}
}
void CWndText::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LONG lOffset = GetOffset(point);
	BYTE chr1;

	if(lOffset >= 0 && lOffset < m_string.GetLength() ) 
	{
		if( !IsWhite( m_string.GetAt( lOffset ) ) ) 
		{
			chr1 = 0;
			const char* begin = m_string;
			const char* end = begin + m_string.GetLength();
			const char* iter = begin;

			m_dwBlockBegin = 0;

			// Get m_dwBlockBegin
			while(*iter && iter < end) {

				if( IsWhite( *iter ) )
					m_dwBlockBegin = iter - begin;

				WORD wCodePage = m_string.m_awCodePage[iter-begin];

				iter = CharNextEx( iter, wCodePage );
				if(iter - begin > lOffset) break;
			}

			m_dwBlockEnd = m_dwBlockBegin;

			while(*iter && iter < end) {

				if( IsWhite( *iter ) ) {
					m_dwBlockEnd = iter - begin;
					break;
				}

				WORD wCodePage = m_string.m_awCodePage[iter-begin];

				iter = CharNextEx( iter, wCodePage );
				if(iter - begin > lOffset) break;
			}

			m_dwOffset = m_dwBlockEnd;
		}
	}
}
BOOL CWndText::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos() )
		return TRUE;

	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

void CWndText::OnSetFocus(CWndBase* pOldWnd)
{
	CWndBase::OnSetFocus(pOldWnd);

	if( g_WndMng.m_pWndFontEdit )
	{
		g_WndMng.m_pWndFontEdit->m_pWndText = this;
	}
}
void CWndText::OnKillFocus(CWndBase* pNewWnd)
{
	CWndBase::OnKillFocus( pNewWnd );
}


void CWndText::BlockInsert( LPCTSTR pstr, LPCTSTR pstr2 )
{
	DWORD dwBlockTemp;

	if(m_dwBlockBegin > m_dwBlockEnd) 
	{
		dwBlockTemp = m_dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd;
		m_dwBlockEnd   = dwBlockTemp;
	}
		
	if( m_dwBlockBegin >= 0 )
	{
		m_string.Insert( m_dwBlockBegin, pstr );
		
		m_dwBlockBegin += strlen(pstr);

		if( (int)( m_dwBlockEnd + strlen(pstr) ) < m_string.GetLength() )
			m_dwBlockEnd += strlen(pstr);
		else
			m_dwBlockEnd = m_string.GetLength();

		m_string.Insert( m_dwBlockEnd, pstr2 );

		UpdateScrollBar();
		m_dwOffset = m_string.GetLength();
		CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
		SetCaretPos( ptCaret );	
	}
}

void CWndText::BlockDelete( LPCTSTR pstr, LPCTSTR pstr2 )
{
	DWORD dwBlockTemp;
	
	if(m_dwBlockBegin > m_dwBlockEnd) 
	{
		dwBlockTemp = m_dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd;
		m_dwBlockEnd   = dwBlockTemp;
	}
	
	if( m_dwBlockBegin >= 0 )
	{
		int nColor = _stricmp( pstr, "#c" );

		TCHAR ch;
		int nCount = m_dwBlockEnd;
		int nIndex = 0;

		nIndex = m_string.Find( pstr2, nCount );

		if( nIndex != -1 )
			Delete( nIndex, strlen(pstr2) );
		///////////////////////////////////////////

		nCount = m_dwBlockBegin;

		while( nCount-- )
		{
			ch = m_string.GetAt( nCount );

			if( ch == '#' )
			{
				nIndex = m_string.Find( pstr, nCount );

				if( nIndex != -1 )
				{
					Delete( nIndex, strlen(pstr) );	

					if( nColor != -1 )
					{
						Delete( nIndex, 8 );							
					}					
					
					break;
				}
			}
		}

		m_dwBlockBegin -= strlen(pstr);
		m_dwBlockEnd   -= strlen(pstr);

		if( nColor != -1 )
		{
			m_dwBlockBegin -= 8;
			m_dwBlockEnd   -= 8;
		}					
		

		if( (int)m_dwBlockBegin < 0 )
			m_dwBlockBegin = 0;

		if( (int)m_dwBlockEnd < 0 )
			m_dwBlockEnd = 0;
								
		UpdateScrollBar();
		m_dwOffset = m_string.GetLength();
		CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret, _countof( m_szCaret ) );
		SetCaretPos( ptCaret );	
	}
}


