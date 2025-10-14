#include "stdafx.h"
#include "definetext.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "WndDialog.h"
#include "WndQuest.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/****************************************************
  WndId : APP_DIALOG - Dialog
****************************************************/

CWndDialog::CWndDialog() 
{ 
	m_nWordButtonNum = 0;
	m_nKeyButtonNum = 0;
	m_nContextButtonNum = 0;
	m_bWordButtonEnable = FALSE;
	m_dwQuest = 0;
#ifdef __V050322_CACHEQUEST
	ZeroMemory( m_apWndAnswer, sizeof( m_apWndAnswer ) );
#endif
} 
CWndDialog::~CWndDialog() 
{ 
	for( int i = 0; i < m_strArray.GetSize(); i++ )
	{
		CEditString* pEditString = (CEditString*) m_strArray.GetAt( i );
		delete pEditString;
	}
#ifdef __V050322_CACHEQUEST
	for( i = 0; i < 6; i++ )
		SAFE_DELETE( m_apWndAnswer[ i ] )
#endif
#ifdef __V050721_BUGFIX_QUESTTIMER
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST );
	if( pWndQuest ) pWndQuest->Update();
#endif
} 
BOOL CWndDialog::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	return TRUE;
}
#ifdef __V050322_CACHEQUEST
BOOL CWndDialog::Process() 
{
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) Destroy();
	return TRUE;
}
#endif // __V050322_CACHEQUEST
void CWndDialog::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndButton* pEnter = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	POINT pt = lpWndCtrl->rect.TopLeft();

	int i;
	p2DRender->TextOut_EditString( pt.x, pt.y, m_string, 0, 0, 6 );//f000000 );
	if( m_bWordButtonEnable )
	{
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			DWORD dwColor = 0xff505050;
			WORDBUTTON* pKeyButton = &m_aWordButton[ i ];
			if( strcmp( pKeyButton->szWord, "__YES__" ) == 0 )
				continue; 
			if( strcmp( pKeyButton->szWord, "__NO__" ) == 0 )
				continue; 
			if( strcmp( pKeyButton->szWord, "__CANCEL__" ) == 0 )
				continue; 
			if( strcmp( pKeyButton->szWord, "__OK__" ) == 0 )
				continue; 
			if( pKeyButton->bStatus )
			{
				dwColor = 0xff1010ff;
				CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
				p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
					CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
				
			}
			p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
		}
	}
	for( i = 0; i < m_nKeyButtonNum; i++ )
	{
		DWORD dwColor = 0xff505050;
		WORDBUTTON* pKeyButton = &m_aKeyButton[ i ];
		if( pKeyButton->bStatus )
		{
			dwColor = 0xff1010ff;
			CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
			p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
				CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
	
		}
		p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
		p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
	}
	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		DWORD dwColor = 0xff101010;
		WORDBUTTON* pKeyButton = &m_aContextButton[ i ];

		if( pKeyButton->bStatus )
		{
			dwColor = 0xff1010ff;
			CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
			p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
				CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
			p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			if( pKeyButton->nLinkIndex >= 0 )
			{
				WORDBUTTON* pKeyButton2 = &m_aContextButton[ pKeyButton->nLinkIndex ];
				CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton2->szWord );
				p2DRender->RenderLine( CPoint( pKeyButton2->rect.left, pKeyButton2->rect.top + size.cy ),
					CPoint( pKeyButton2->rect.left + size.cx, pKeyButton2->rect.top + size.cy ), dwColor );
				p2DRender->TextOut( pKeyButton2->rect.left, pKeyButton2->rect.top,  pKeyButton2->szWord, dwColor );
				p2DRender->TextOut( pKeyButton2->rect.left + 1, pKeyButton2->rect.top,  pKeyButton2->szWord, dwColor );
			}
		}
	}
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	m_texChar.Render( p2DRender, lpWndCtrl->rect.TopLeft() );
} 
void CWndDialog::OnMouseWndSurface( CPoint point )
{
	for( int i = 0; i < m_nWordButtonNum; i++ )
		m_aWordButton[ i ].bStatus = FALSE;
	for( i = 0; i < m_nKeyButtonNum; i++ )
		m_aKeyButton[ i ].bStatus = FALSE;
	for( i = 0; i < m_nContextButtonNum; i++ )
		m_aContextButton[ i ].bStatus = FALSE;
	if( m_bWordButtonEnable )
	{
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			if( m_aWordButton[ i ].rect.PtInRect( point ) )
			{
				SetMouseCursor( CUR_SELECT );
			 	m_aWordButton[ i ].bStatus = TRUE;
				return;
			}
		}
	}
	for( i = 0; i < m_nKeyButtonNum; i++ )
	{
		WORDBUTTON* pKeyButton = &m_aKeyButton[ i ];
		if( pKeyButton->rect.PtInRect( point ) )
		{
			SetMouseCursor( CUR_SELECT );
			pKeyButton->bStatus = TRUE;
			CString string;// = prj.GetWordToolTip( pKeyButton->text );
			string.Format( prj.GetText(TID_GAME_ABOUTQUESTION), pKeyButton->szWord );
			if( string.IsEmpty() == FALSE )
			{
				CRect rect = pKeyButton->rect;
				ClientToScreen( &rect );

				CPoint point2 = point;
				ClientToScreen( &point2 );
				rect.InflateRect( 2, 2);
				g_toolTip.PutToolTip( (DWORD)pKeyButton, string, &rect, point2 );
			}
			return;
		}
	}
	CString strWord, strOriginal;
	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		DWORD dwColor = 0xff101010;
		WORDBUTTON* pKeyButton = &m_aContextButton[ i ];
		if( pKeyButton->rect.PtInRect( point ) )
		{
			SetMouseCursor( CUR_HELP );
			pKeyButton->bStatus = TRUE;
			CRect rect = pKeyButton->rect;
			ClientToScreen( &rect );

			CPoint point2 = point;
			ClientToScreen( &point2 );
			rect.InflateRect( 2, 2);

			return;
		}
	}
	SetMouseCursor( CUR_BASE );
}

void CWndDialog::RunScript( const char* szKey, DWORD dwParam, DWORD dwQuest )
{
#ifdef __ONLINE
#ifdef __V050322_CACHEQUEST
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover )
		g_DPlay.SendScriptDialogReq( pMover->GetId(), szKey, dwParam, dwQuest, pMover->GetId(), g_pPlayer->GetId() );
#else // __V050322_CACHEQUEST
	g_DPlay.SendScriptDialogReq( m_pMover->GetId(), szKey, dwParam, dwQuest, m_pMover->GetId(), g_pPlayer->GetId() );
#endif // __V050322_CACHEQUEST
	m_timer.Set( SEC( 1 ) );
#else	// __ONLINE
#ifdef __V050322_CACHEQUEST
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover )
		pMover->m_pNpcProperty->RunDialog( szKey, NULL, dwParam, pMover->GetId(), g_pPlayer->GetId(), 0 );	// 싱글컴팔 에러나서 뒤에 0 파라메터 하나 더 넣었음 -xuzhu-
#else // __V050322_CACHEQUEST
	m_pMover->m_pNpcProperty->RunDialog( szKey, NULL, dwParam, m_pMover->GetId(), g_pPlayer->GetId() );
#endif // __V050322_CACHEQUEST
#endif	// __ONLINE
}

void CWndDialog::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

#ifdef __V050322_CACHEQUEST
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) return;
	LPCHARACTER lpCharacter = pMover->GetCharacter();
#else // __V050322_CACHEQUEST
	LPCHARACTER lpCharacter = m_pMover->GetCharacter();
#endif // __V050322_CACHEQUEST

	if( lpCharacter )
	{
		m_texChar.DeleteDeviceObjects();
		m_texChar.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( "char\\",lpCharacter->m_szChar ), 0xffff00ff, TRUE );
#ifdef __V050322_CACHEQUEST
		if( lpCharacter->m_dwMusicId )
			PlayMusic( lpCharacter->m_dwMusicId, 1 );
#endif
	}
	m_nCurArray = 0;
	for( int i = 0; i < m_strArray.GetSize(); i++ )
	{
		CEditString* pEditString = (CEditString*) m_strArray.GetAt( i );
		delete pEditString;
	}
	m_strArray.RemoveAll();
	MakeKeyButton();
	
	m_bSay = FALSE;

#if !defined(__ONLINE)
	RunScript( _T( "#init" ), 0, 0 );		// 임의로 세번째 파라메터에 0을 넣었슴다. -xuzhu-
#endif

	UpdateButtonEnable();
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.top  + (rectRoot.Height() / 2) - (nHeight / 2);	
	CPoint point( x, 10 );
	Move( point );

#ifdef __V050721_BUGFIX_QUESTTIMER
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST );
	if( pWndQuest ) pWndQuest->Update();
#endif
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDialog::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DIALOG, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndDialog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDialog::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDialog::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	if( m_timer.IsTimeOut() == FALSE )
		return;

	PLAYSND( SND_INF_CLICK );
	if( m_bWordButtonEnable )
	{
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			if( m_aWordButton[ i ].rect.PtInRect( point ) )
			{			
				BeginText();
				RunScript( m_aWordButton[i].szKey, m_aWordButton[i].dwParam, m_aWordButton[i].dwParam2 );
				MakeKeyButton();
				UpdateButtonEnable();
			}
		}
	}
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		if( m_aKeyButton[ i ].rect.PtInRect( point ) )
		{
			BeginText();
			RunScript( m_aKeyButton[i].szKey, m_aKeyButton[i].dwParam, m_aKeyButton[i].dwParam2 );
			MakeKeyButton();
			UpdateButtonEnable();
		}
	}
	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		WORDBUTTON* pContextButton = &m_aContextButton[ i ];
		if( pContextButton->rect.PtInRect( point ) )
		{
			CString strKey = pContextButton->szKey;
			BeginText();
			RunScript( strKey, 0, 0 );
		}
	}
} 
void CWndDialog::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDialog::Say( LPCTSTR lpszString, DWORD dwQuest )
{
 	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuest );
 	CString string = "  ";
#ifdef __V050322_QUESTTITLE
	if( dwQuest )
	{
		string = "#b#cff5050f0";
		string += pQuestProp->m_szTitle;
		string += "#nb#nc\n";
		string += "  ";
		string += lpszString;
	}
	else
#endif
		string += lpszString;

	int nLength = string.GetLength();//_tcslen( szString );
	for( int i = 0, j = 0; i < nLength; i++ )
	{
		if( string[ i ] == '\\' && string[ i + 1 ] == 'n' )
		{
			// 중간의 두개의 코드를 \n와 ' '로 대체.
			string.SetAt( i, '\n' );
			string.SetAt( i + 1, ' ' );
			// 그리고 스페이스 한개 삽입.
			string.Insert( i + 1, " " );
		}
	}
	m_dwQuest = dwQuest;
	ParsingString( string );
	EndSay();
}

void CWndDialog::ParsingString( LPCTSTR lpszString )
{
#ifdef __V050322_CACHEQUEST
	CString strWord, strOriginal;
	int nMarkBraceIdx;
	BOOL bKey = FALSE;
	BOOL bBrace = FALSE;
	int nTemp  =0;
	BOOL bWirdChar = FALSE;

	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) return;

	int nPos =  m_strArray.GetSize();
	m_aContextMark[ nPos ].RemoveAll();

	CEditString editString;
	editString.AddParsingString( lpszString );
	CEditString* pEditString = new CEditString;

	int nLength = editString.GetLength();
	
	// 왼쪽이 strWord, 오른쪽이 strOrigital
	for( int i = 0; i < nLength; i++ )
	{
		if( editString[ i ] == '['  )
		{
			nMarkBraceIdx = i;
			bBrace = TRUE;
			if( bKey == FALSE )
			{
				strOriginal.Empty();
				strWord.Empty();
				nTemp = 1;
			}
		}
		else
		if( editString[ i ] == ']' )
		{
			if( bBrace == TRUE )
			{
				bBrace = FALSE;
				if( bKey == FALSE )
				{
					bKey = TRUE;
					int nLen = strWord.GetLength();
					int nMark = 3;
					if( nLen == 1 ) 
						nMark = 4;
					if( nLen == 2 && bWirdChar )
						nMark = 4;
					m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 1, nMark );
					if( bWirdChar )
						m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 2, nMark );
				}
				else
				{
					bKey = FALSE;
					if( strOriginal.IsEmpty() )
						strOriginal = strWord;
					m_mapWordToOriginal.SetAt( strWord, strOriginal );
				}
			}
		}
		else
		{
			if( bKey == TRUE )
			{
				if( bBrace == FALSE )
				{
					LPCHARACTER lpCharacter = pMover->GetCharacter();
					CString string;
					string.Format( "%s에서 [%s]와 [] 사이에 잡문자가 들어있거나 연결이 안된다.", lpCharacter->m_szDialog, strWord );
					AfxMessageBox( string );
				}
				strOriginal += editString[ i ];
				if( IsDBCSLeadByte( editString[ i ] ) )
					strOriginal += editString[ ++i ];
			}
			else
			{
				int nMarkTemp = 0;
				DWORD dwStyle = editString.m_abyStyle.GetAt( i );
				DWORD dwColor = editString.m_adwColor.GetAt( i );
				if( bBrace )
				{
					nMarkTemp = nTemp;
					if( nTemp == 1 ) nTemp++;
					dwStyle = ESSTY_BOLD;
				}
				m_aContextMark[ nPos ].Add( nMarkTemp );
				pEditString->AddString( editString[ i ], dwColor, dwStyle );

				strWord += editString[ i ];
				bWirdChar = FALSE;
				if( IsDBCSLeadByte( editString[ i ] ) )
				{
					m_aContextMark[ nPos ].Add( nMarkTemp );
					pEditString->AddString( editString[ ++i ], 0xff000000, dwStyle );

					strWord += editString[ i ];
					bWirdChar = TRUE;
				}
			}
		}
	}
	pEditString->Align( pEditString->m_pFont, 0 );
	m_strArray.Add( pEditString );
#else
//{{AFX
	CString strWord, strOriginal;
	int nLength = strlen( lpszString );
	int nMarkBraceIdx;
	BOOL bKey = FALSE;
	BOOL bBrace = FALSE;
	int nTemp  =0;
	BOOL bWirdChar = FALSE;

	int nPos =  m_strArray.GetSize();
	m_aContextMark[ nPos ].RemoveAll();

	CEditString* pEditString = new CEditString;
	
	// 왼쪽이 strWord, 오른쪽이 strOrigital
	for( int i = 0; i < nLength; i++ )
	{
		if( lpszString[ i ] == '['  )
		{
			nMarkBraceIdx = i;
			bBrace = TRUE;
			if( bKey == FALSE )
			{
				strOriginal.Empty();
				strWord.Empty();
				nTemp = 1;
			}
		}
		else
		if( lpszString[ i ] == ']' )
		{
			if( bBrace == TRUE )
			{
				bBrace = FALSE;
				if( bKey == FALSE )
				{
					bKey = TRUE;
					int nLen = strWord.GetLength();
					int nMark = 3;
					if( nLen == 1 ) 
						nMark = 4;
					if( nLen == 2 && bWirdChar )
						nMark = 4;
					m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 1, nMark );
					if( bWirdChar )
						m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 2, nMark );
				}
				else
				{
					bKey = FALSE;
					if( strOriginal.IsEmpty() )
						strOriginal = strWord;
					m_mapWordToOriginal.SetAt( strWord, strOriginal );
				}
			}
		}
		else
		{
			if( bKey == TRUE )
			{
				if( bBrace == FALSE )
				{
					LPCHARACTER lpCharacter = m_pMover->GetCharacter();
					CString string;
					string.Format( "%s에서 [%s]와 [] 사이에 잡문자가 들어있거나 연결이 안된다.", lpCharacter->m_szDialog, strWord );
					AfxMessageBox( string );
				}
				strOriginal += lpszString[ i ];
				if( IsDBCSLeadByte( lpszString[ i ] ) )
					strOriginal += lpszString[ ++i ];
			}
			else
			{
				int nMarkTemp = 0;
				DWORD dwStyle = 0;
				if( bBrace )
				{
					nMarkTemp = nTemp;
					if( nTemp == 1 ) nTemp++;
					dwStyle = ESSTY_BOLD;
				}
				m_aContextMark[ nPos ].Add( nMarkTemp );
				pEditString->AddString( lpszString[ i ], 0xff000000, dwStyle );

				strWord += lpszString[ i ];
				bWirdChar = FALSE;
				if( IsDBCSLeadByte( lpszString[ i ] ) )
				{
					m_aContextMark[ nPos ].Add( nMarkTemp );
					pEditString->AddString( lpszString[ ++i ], 0xff000000, dwStyle );

					strWord += lpszString[ i ];
					bWirdChar = TRUE;
				}
			}
		}
	}
	pEditString->Align( pEditString->m_pFont, 0 );
	m_strArray.Add( pEditString );
//}}AFX
#endif // __V050322_CACHEQUEST
}
void CWndDialog::EndSay()
{
	if( m_strArray.GetSize() )
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_string.Init( m_pFont, &lpWndCtrl->rect );
		m_string.SetEditString( *(CEditString*)m_strArray.GetAt( 0 ) );//, 0xff000000 );
		m_nCurArray = 0;
	}
	MakeKeyButton();
	MakeContextButton();
	MakeAnswerButton();
	UpdateButtonEnable();

}
void CWndDialog::BeginText()
{
	for( int i = 0; i < m_strArray.GetSize(); i++ )
	{
		CEditString* pEditString = (CEditString*) m_strArray.GetAt( i );
		delete pEditString;
	}
	m_strArray.RemoveAll();
	m_mapWordToOriginal.RemoveAll();
	m_nWordButtonNum = 0;
#ifdef __V050322_CACHEQUEST
	for( i = 0; i < 6; i++ )
	{
		if( m_apWndAnswer[ i ] )
			m_apWndAnswer[ i ]->Destroy( TRUE );
		m_apWndAnswer[ i ] = NULL;
	}
#endif
}
void CWndDialog::MakeContextButton()
{
	DWORD dwLineCount = m_string.GetLineCount();
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	m_nContextButtonNum = 0;
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	int x, y = lpWndCtrl->rect.top;
	DWORD dwCurOffset;
	TCHAR strHan[ 3 ];
	CRect rectWord(0, 0, 0, 0);
	DWORD dwMark = 0;
	CString strKey, strWord;
	BOOL bLinkWord = FALSE;
	for( int i = 0; i < dwLineCount; i++)
	{
		CString string = m_string.GetLine( i );
		DWORD dwOffset = m_string.GetLineOffset( i );
		LPCTSTR lpszStr = string;
		int nLength = string.GetLength();
		x = lpWndCtrl->rect.left;
		strWord.Empty();
		
		for( int j = 0; j < nLength; j++ )
		{
			if( IsDBCSLeadByte( string[ j ] ) )
			{
				strHan[ 0 ] = string[ j++ ];
				strHan[ 1 ] = string[ j ];
				strHan[ 2 ] = '\0';
				dwCurOffset = dwOffset + ( j - 1 );
			}
			else
			{
				strHan[ 0 ] = string[ j ];
				strHan[ 1 ] = '\0';
				dwCurOffset = dwOffset + j;
			}
			CSize size = m_pFont->GetTextExtent( strHan );
			dwMark = m_aContextMark[ m_nCurArray ].GetAt( dwCurOffset );

			if( dwMark )
			{
				strWord += strHan;
				rectWord.right += size.cx;
			}
			if( dwMark == 1 || dwMark == 4 ) 
			{
				rectWord.SetRect( x, y, x + size.cx, y + dwMaxHeight );
				if( dwMark == 4 ) dwMark = 3;
			}
			if( dwMark == 3 ) 
			{
				m_aContextButton[ m_nContextButtonNum ].rect = rectWord;
				m_aContextButton[ m_nContextButtonNum ].bStatus = FALSE;
				m_aContextButton[ m_nContextButtonNum ].nLinkIndex = -1;
				m_aContextButton[ m_nContextButtonNum ].dwParam2 = m_dwQuest;
				CString strTemp = strWord;
				if( bLinkWord )
				{
					m_aContextButton[ m_nContextButtonNum ].nLinkIndex = m_nContextButtonNum - 1;
					strWord = m_aContextButton[ m_nContextButtonNum - 1 ].szWord;
					strWord += strTemp;
				}
				CString strKey;
				m_mapWordToOriginal.Lookup( strWord, strKey );
				strcpy( m_aContextButton[ m_nContextButtonNum ].szWord, strTemp );
				strcpy( m_aContextButton[ m_nContextButtonNum ].szKey, strKey );
				if( bLinkWord )
					strcpy( m_aContextButton[ m_nContextButtonNum - 1 ].szKey, strKey );
				m_nContextButtonNum++;
				bLinkWord = FALSE;
				strWord.Empty();
			}
			x += size.cx;
		}	
		y += dwMaxHeight;
		
		// 중간에 끊어진 경우 
		if( dwMark == 1 || dwMark == 2 ) //bKeyButton == TRUE )
		{
			//bKeyButton = FALSE;
			bLinkWord = TRUE;
			m_aContextButton[ m_nContextButtonNum ].rect = rectWord;
			m_aContextButton[ m_nContextButtonNum ].bStatus = FALSE;
			m_aContextButton[ m_nContextButtonNum ].nLinkIndex = m_nContextButtonNum + 1;
			m_aContextButton[ m_nContextButtonNum ].dwParam2 = m_dwQuest;
			m_mapWordToOriginal.Lookup( strWord, strKey );
			strcpy( m_aContextButton[ m_nContextButtonNum ].szWord, strWord );
			strcpy( m_aContextButton[ m_nContextButtonNum ].szKey, strKey );
			rectWord.SetRect( lpWndCtrl->rect.left, y, lpWndCtrl->rect.left, y + dwMaxHeight );
			m_nContextButtonNum++;
		}
	}
}
void CWndDialog::AddKeyButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest )
{
	WORDBUTTON* lpKeyButton;// = &m_aKeyButton[ m_nKeyButtonNum ];
	// 같은 워드가 발견되면 무시한다. 키는 같아도 되지만 워드는 같으면 하나는 무시함.
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		if( strcmp( lpKeyButton->szWord, lpszWord ) == 0 )
			return; 
		
	}
	lpKeyButton = &m_aKeyButton[ m_nKeyButtonNum ];
	strcpy( lpKeyButton->szWord, lpszWord );
	strcpy( lpKeyButton->szKey, lpszKey );
	lpKeyButton->bStatus = FALSE;
	//lpWordButton->rect
	lpKeyButton->dwParam = dwParam;
	lpKeyButton->dwParam2 = dwQuest;
	m_nKeyButtonNum++;
	EndSay();
}
void CWndDialog::RemoveAllKeyButton()
{
	m_nKeyButtonNum = 0;
	EndSay();
}
void CWndDialog::RemoveKeyButton( LPCTSTR lpszKey )
{
	WORDBUTTON* lpKeyButton;// = &m_aKeyButton[ m_nKeyButtonNum ];
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		//if( strcmp( lpKeyButton->szWord, lpszWord ) == 0 )
		if( strcmp( lpKeyButton->szKey, lpszKey ) == 0 )
		{
			for( int j = i; j < m_nKeyButtonNum - 1; j++ )
			{
				memcpy( &m_aKeyButton[ j ], &m_aKeyButton[ j + 1 ], sizeof( WORDBUTTON ) ) ;
			}
			m_nKeyButtonNum--;
			EndSay();
			return;
		}
	}
	EndSay();
}
// 하얀색 대화영역에 나오는 버튼 
void CWndDialog::AddAnswerButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest )
{
	WORDBUTTON* lpWordButton = &m_aWordButton[ m_nWordButtonNum ];
	strcpy( lpWordButton->szWord, lpszWord );
	strcpy( lpWordButton->szKey, lpszKey );
	lpWordButton->bStatus = FALSE;
	//lpWordButton->rect
	lpWordButton->dwParam  = dwParam;
	lpWordButton->dwParam2 = dwQuest;
	m_nWordButtonNum++;
	EndSay();
}

void CWndDialog::MakeKeyButton()
{
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CRect rectClient = GetClientRect();
	int x = 0, y = 0;
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		WORDBUTTON* pWndButton = &m_aKeyButton[ i ];

		CSize size = m_pFont->GetTextExtent( pWndButton->szWord );
		size.cx += 20;//10 * 6;
		//size.cy += 4;
		pWndButton->bStatus = FALSE;
		
		pWndButton->rect = CRect( lpWndCtrl->rect.left + x, lpWndCtrl->rect.top + y * dwMaxHeight, 
			lpWndCtrl->rect.left + x + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );
		if( pWndButton->rect.right > lpWndCtrl->rect.right )
		{
			x = 0, y++;
			pWndButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				lpWndCtrl->rect.left + x * size.cx + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );
		}				
		x += size.cx;
	}
}
// 하얀색 대화영역에 나오는 버튼 
// AddAnswer로 추가 
void CWndDialog::MakeAnswerButton()
{
	m_bWordButtonEnable = FALSE;
	if( m_nCurArray == m_strArray.GetSize() - 1 )
	{
		DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		
		int yBegin = lpWndCtrl->rect.top + m_string.GetLineCount() * dwMaxHeight + dwMaxHeight;
		int x = 0, y = 0;//
		int nButtonNum = 0;
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			WORDBUTTON* pWordButton = &m_aWordButton[ i ];
#ifdef __V050322_CACHEQUEST
			if( strcmp( pWordButton->szWord, "__YES__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__NO__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__CANCEL__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__OK__" ) == 0 )
			{ nButtonNum++; continue; }
#endif
			CSize size = m_pFont->GetTextExtent( pWordButton->szWord );
			size.cx += 10;

			pWordButton->rect = CRect( lpWndCtrl->rect.left + x, yBegin + y * dwMaxHeight, 
				lpWndCtrl->rect.left + x + size.cx, yBegin + y * size.cy + dwMaxHeight );
			if( pWordButton->rect.right > lpWndCtrl->rect.right )
			{
				x = 0, y++;
				//pWordButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				//	lpWndCtrl->rect.left + x * size.cx + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );

				pWordButton->rect = CRect( lpWndCtrl->rect.left + x, yBegin + y * dwMaxHeight, 
					lpWndCtrl->rect.left + x + size.cx, yBegin + y * size.cy + dwMaxHeight );

			}				
			x += size.cx;
		}
#ifdef __V050322_CACHEQUEST
		x = lpWndCtrl->rect.left + ( lpWndCtrl->rect.Width() / 2 ) - ( ( 90 * nButtonNum ) / 2 );
		CRect rect;
		CString strTexture;
		int nWndId = 0;
		int j = 0;
		for( i = 0; i < 6; i++ )
			SAFE_DELETE( m_apWndAnswer[ i ] )
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			WORDBUTTON* pWordButton = &m_aWordButton[ i ];
			rect = CRect( x, lpWndCtrl->rect.bottom - 10, x + 70, lpWndCtrl->rect.bottom + 10 );
			if( strcmp( pWordButton->szWord, "__YES__" ) == 0 )
			{
				nWndId =  WIDC_YES;
				strTexture = "buttYes.tga";
			}
			else
			if( strcmp( pWordButton->szWord, "__NO__" ) == 0 )
			{
				nWndId =  WIDC_NO;
				strTexture = "buttNo.tga";
			}
			else
			if( strcmp( pWordButton->szWord, "__CANCEL__" ) == 0 )
			{
				nWndId =  WIDC_CANCEL;
				strTexture = "buttCancel.tga";
			}
			else
			if( strcmp( pWordButton->szWord, "__OK__" ) == 0 )
			{
				nWndId =  WIDC_OK;
				strTexture = "buttOk.tga";
			}
			if(	nWndId )
			{
				m_apWndAnswer[ j ] = new CWndAnswer;
				m_apWndAnswer[ j ]->Create( "", WBS_CHILD, rect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( D3DDEVICE, MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = (WORDBUTTON*)	pWordButton;
				j++;
			}
			x += 90;
		}
#endif
		m_bWordButtonEnable = TRUE;
	}
}

void CWndDialog::UpdateButtonEnable()
{
	CWndButton* pEnter = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
#ifdef __V050322_CACHEQUEST
	if( m_strArray.GetSize() == 0 || m_strArray.GetSize() - 1 == m_nCurArray )
#else
	if( m_strArray.GetSize() - 1 == m_nCurArray )
#endif
	{
		pEnter->SetVisible( FALSE );
	}
	else
	{
		pEnter->SetVisible( TRUE );
	}		
}


BOOL CWndDialog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON1:
		if( m_nCurArray < m_strArray.GetSize() - 1 )
		{
			m_nCurArray++;
			LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
			m_string.Init( m_pFont, &lpWndCtrl->rect );
			m_string.SetEditString( *(CEditString*)m_strArray.GetAt( m_nCurArray ) );
			MakeContextButton();
			MakeAnswerButton();
		}
		UpdateButtonEnable();
		break;
#ifdef __V050322_CACHEQUEST
	case WIDC_OK:
	case WIDC_YES:
	case WIDC_NO:
	case WIDC_CANCEL:
		{
			CWndAnswer* pWndAnswer = (CWndAnswer*)pLResult;
			WORDBUTTON* pWordButton = (WORDBUTTON*)pWndAnswer->m_pWordButton;
			BeginText();
			RunScript( pWordButton->szKey,  pWordButton->dwParam,  pWordButton->dwParam2 );
			MakeKeyButton();
			UpdateButtonEnable();
		}
		break;
#endif
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}










































