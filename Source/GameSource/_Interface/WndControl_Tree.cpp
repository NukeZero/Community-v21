
#include "StdAfx.h"
#include "WndControl_Tree.h"

#define TREEELEM_DEFAULT_COLOR		D3DCOLOR_ARGB( 255, 64, 64, 64 )
#define TREEELEM_SELECT_COLOR		D3DCOLOR_ARGB( 255,  0,  0, 255 )


//---------------------------------------------------------------------------------------------
// 트리 요소
//---------------------------------------------------------------------------------------------
tagScriptElem::tagScriptElem( void )
:	m_lpParent( NULL ) 
,	m_pWndCheckBox( NULL )
,	m_dwColor( TREEELEM_DEFAULT_COLOR )
,	m_dwSelectColor( TREEELEM_SELECT_COLOR )
,	m_dwData( 0 )
,	m_bOpen( FALSE )
,	m_strKeyword( _T( "" ) )
{
}

tagScriptElem::tagScriptElem( CRect* pRect )
:	m_lpParent( NULL )
,	m_pWndCheckBox( NULL )
,	m_dwColor( TREEELEM_DEFAULT_COLOR ) 
,	m_dwSelectColor( TREEELEM_SELECT_COLOR ) 
,	m_dwData( 0 ) 
,	m_strKeyword( _T( "" ) ) 
,	m_bOpen( FALSE ) 
{
	m_strKeyword.Init( CWndBase::m_Theme.m_pFontText, pRect );
}


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndTreeCtrl::CWndTreeCtrl()
:	m_nLineSpace( 3 )
,	m_bForceDrawFrame( FALSE )
,	m_pTexButtOpen( NULL )
,	m_pTexButtClose( NULL )
,	m_pFocusElem( NULL )
,	m_bMemberCheckingMode( FALSE )
,	m_nFontHeight( NULL )
,	m_nTreeTabWidth( 16 )
,	m_nCategoryTextSpace( 16 )
,	m_nTreeItemsMaxWidth( 0 )
{
	m_byWndType		= WTYPE_TREECTRL;
	m_strTexture	= DEF_CTRL_TEXT;
	m_bTile			= TRUE;

	PFONTCOLOR_WNDTREECTRL pFontColorWndTreeCtrl = g_WndFontColorManager->GetWndTreeCtrl();

	m_nFontColor   = pFontColorWndTreeCtrl->m_stDefault.GetFontColor();
	m_nSelectColor = pFontColorWndTreeCtrl->m_stSelect.GetFontColor();
} 


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndTreeCtrl::~CWndTreeCtrl()
{
	DeleteAllItems();
}


//---------------------------------------------------------------------------------------------
// 업데이트 전 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTreeCtrl::OnInitialUpdate()
{
	CRect rect( GetWindowRect() );

	m_pTexButtOpen  = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButtTreeOpen.tga" ) ), 0, TRUE );
	m_pTexButtClose = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButtTreeClose.tga" ) ), 0, TRUE );

	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndTreeCtrl::Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID )
{
	return CWndBase::Create( WBS_CHILD | dwTextStyle, rect, pParentWnd, nID );
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndTreeCtrl::Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	SetFontColor( nFontColorR, nFontColorG, nFontColorB );
	return CWndBase::Create( WBS_CHILD | dwTextStyle, rect, pParentWnd, nID );
}


//---------------------------------------------------------------------------------------------
// 트리 스크립트 불러오기 
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndTreeCtrl::LoadTreeScript( LPCTSTR lpFileName ) 
{
	CScript script;

	if( FALSE == script.Load( lpFileName ) )
		FLERROR_ASSERT_LOG_RETURN_VOID( FALSE, _T("[윈도우 트리 컨트롤] 스크립트 불러오기 실패 - [ %s ]"), lpFileName );

	do
	{

		InterpriteScript( script, m_treeElem.m_ptrArray );
		script.GetToken(); // keyword

	}while( script.tok != FINISHED );
}


//---------------------------------------------------------------------------------------------
// 스크립트 파싱하기
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
void CWndTreeCtrl::InterpriteScript( CScript& script, CPtrArray& ptrArray ) 
{
	LPTREEELEM lpTreeElem( NULL );
	script.GetToken();	// keyword

	while( *script.token != '}' && script.tok != FINISHED )
	{
		lpTreeElem	= new TREEELEM;
		lpTreeElem->m_strKeyword	= script.Token;
		lpTreeElem->m_bOpen			= FALSE;
		lpTreeElem->m_dwColor		= m_dwColor;
		ptrArray.Add(lpTreeElem);
		lpTreeElem->m_dwData	= 0;
		script.GetToken();	// keyword
		if( *script.token == '{' )
		{
			if( lpTreeElem )
				InterpriteScript( script, lpTreeElem->m_ptrArray );
		}
	}
	if( script.tok == FINISHED )
		return;

	script.GetToken();
}


//---------------------------------------------------------------------------------------------
// 모든 아이템 삭제
// param	: void
// return	: 성공 / 실패 
//---------------------------------------------------------------------------------------------
BOOL CWndTreeCtrl::DeleteAllItems()
{
	FreeTree(m_treeElem.m_ptrArray);
	DeleteItemArray();
	m_pFocusElem = NULL;

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 아이템 배열 삭제
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTreeCtrl::DeleteItemArray()
{
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		SAFE_DELETE_CONST( (LPTREEITEM)m_treeItemArray.GetAt(i) );

	m_treeItemArray.RemoveAll();
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTreeCtrl::PaintFrame(C2DRender* p2DRender)
{
	if( m_bForceDrawFrame )
	{
		CRect rect = GetWindowRect();
		m_pTheme->RenderWndTextFrame( p2DRender, &rect );	//gmpbigsun: 아왜 안그려?
	}
}

//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndTreeCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3 );

	m_nFontHeight = GetFontHeight();
	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();

	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		SAFE_DELETE_CONST( (LPTREEITEM)m_treeItemArray.GetAt(i) );

	m_treeItemArray.RemoveAll();

	m_nTreeItemsMaxWidth = 0;

	PaintTree(p2DRender,pt,m_treeElem.m_ptrArray) ;

	int nPage = GetClientRect().Height() / m_nFontHeight;
	int nRange = m_treeItemArray.GetSize();

	if(	IsWndStyle( WBS_VSCROLL ) )  
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		if(nPage >= m_wndScrollBar.GetMaxScrollPos()) // 스크롤바가 활성화되지 않을 영역에서의 포지션은 0로 설정
			m_wndScrollBar.SetScrollPos(0);
	}
	else
		m_wndScrollBar.SetVisible( FALSE );
}



LPTREEELEM CWndTreeCtrl::GetCurSel()
{
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::GetRootElem() 
{ 
	return &m_treeElem; 
}
LPTREEELEM CWndTreeCtrl::GetNextElem( LPTREEELEM pElem, int& nPos )
{
	LPTREEELEM pGetElem = NULL;
	if( nPos < pElem->m_ptrArray.GetSize() ) 
	{
		pGetElem = (LPTREEELEM)pElem->m_ptrArray.GetAt( nPos );
		nPos++;
	}
	return pGetElem;
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( DWORD dwData )
{
	return FindTreeElem( m_treeElem.m_ptrArray, dwData );
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( CPtrArray& ptrArray, DWORD dwData )
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if( lpTreeElem->m_dwData == dwData )
			return lpTreeElem;
		if( lpTreeElem->m_ptrArray.GetSize() )
		{
			lpTreeElem = FindTreeElem( lpTreeElem->m_ptrArray, dwData );
			if( lpTreeElem ) return lpTreeElem;
		}
	}
	return NULL;
}
LPTREEELEM CWndTreeCtrl::SetCurSel( DWORD dwData )
{
	m_pFocusElem = FindTreeElem( m_treeElem.m_ptrArray, dwData );
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::SetCurSel( LPCTSTR lpszKeyword )
{
	m_pFocusElem = FindTreeElem( m_treeElem.m_ptrArray, lpszKeyword );
	return m_pFocusElem;
}

void CWndTreeCtrl::FreeTree( CPtrArray& ptrArray ) 
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);

		if( m_bMemberCheckingMode == TRUE )
			SAFE_DELETE( lpTreeElem->m_pWndCheckBox );

		if(lpTreeElem->m_ptrArray.GetSize())
			FreeTree(lpTreeElem->m_ptrArray);
		SAFE_DELETE( lpTreeElem );
	}
	ptrArray.RemoveAll();
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( LPCTSTR lpszKeyword )
{
	return FindTreeElem( m_treeElem.m_ptrArray, lpszKeyword );
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword )
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if( lpTreeElem->m_strKeyword == lpszKeyword )
			return lpTreeElem;
		if( lpTreeElem->m_ptrArray.GetSize() )
		{
			lpTreeElem = FindTreeElem( lpTreeElem->m_ptrArray, lpszKeyword );
			if( lpTreeElem ) return lpTreeElem;
		}
	}
	return NULL;
}


LPTREEELEM CWndTreeCtrl::InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData, BOOL bForbidChecking, BOOL bCheck )
{
	LPTREEELEM lpTreeElem = new TREEELEM( &GetClientRect() );
	lpTreeElem->m_lpParent = lpParent;
	lpTreeElem->m_dwColor = m_nFontColor;
	lpTreeElem->m_dwSelectColor = m_nSelectColor;
	lpTreeElem->m_strKeyword.Init( m_pFont, &GetClientRect() );
	lpTreeElem->m_strKeyword.SetParsingString( lpString, m_nFontColor, 0x00000000, 0, 0x00000001, TRUE );
	lpTreeElem->m_dwData = dwData;
	lpTreeElem->m_bOpen = FALSE;

	if( m_bMemberCheckingMode == TRUE && bForbidChecking == FALSE )
	{
		lpTreeElem->m_pWndCheckBox = new CWndButton;
		CWndButton* pWndCheckBox = lpTreeElem->m_pWndCheckBox;
		CRect rectCheckBox( 0, 0, CHECK_BOX_SIZE_XY, CHECK_BOX_SIZE_XY );
		pWndCheckBox->Create( "", WBS_CHECK, rectCheckBox, this, WIDC_CHECK );

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			pWndCheckBox->SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCheck.bmp" ) ), 1 );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndCheckBox->SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCheck.tga" ) ), 1 );

		pWndCheckBox->FitTextureSize();
		pWndCheckBox->SetCheck( bCheck );
		pWndCheckBox->EnableWindow( FALSE );
		pWndCheckBox->SetVisible( FALSE );
	}
	if( lpParent )
		lpParent->m_ptrArray.Add( lpTreeElem );
	else
		m_treeElem.m_ptrArray.Add( lpTreeElem );

	return lpTreeElem;
}


LPTREEELEM CWndTreeCtrl::InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData, BOOL bForbidChecking, BOOL bCheck, DWORD dwFontColor, DWORD dwSelectColor )
{
	LPTREEELEM lpTreeElem = new TREEELEM( &GetClientRect() );
	lpTreeElem->m_lpParent = lpParent;
	lpTreeElem->m_dwColor = dwFontColor;
	lpTreeElem->m_dwSelectColor = dwSelectColor;
	lpTreeElem->m_strKeyword.Init( m_pFont, &GetClientRect() );
	lpTreeElem->m_strKeyword.SetParsingString( lpString, dwFontColor, 0x00000000, 0, 0x00000001, TRUE );
	lpTreeElem->m_dwData = dwData;
	lpTreeElem->m_bOpen = FALSE;

	if( m_bMemberCheckingMode == TRUE && bForbidChecking == FALSE )
	{
		lpTreeElem->m_pWndCheckBox = new CWndButton;
		CWndButton* pWndCheckBox = lpTreeElem->m_pWndCheckBox;
		CRect rectCheckBox( 0, 0, CHECK_BOX_SIZE_XY, CHECK_BOX_SIZE_XY );
		pWndCheckBox->Create( "", WBS_CHECK, rectCheckBox, this, WIDC_CHECK );

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			pWndCheckBox->SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCheck.bmp" ) ), 1 );
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndCheckBox->SetTexture( D3DDEVICE, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCheck.tga" ) ), 1 );

		pWndCheckBox->FitTextureSize();
		pWndCheckBox->SetCheck( bCheck );
		pWndCheckBox->EnableWindow( FALSE );
		pWndCheckBox->SetVisible( FALSE );
	}
	if( lpParent )
		lpParent->m_ptrArray.Add( lpTreeElem );
	else
		m_treeElem.m_ptrArray.Add( lpTreeElem );
	return lpTreeElem;
}


BOOL CWndTreeCtrl::CheckParentTreeBeOpened( LPTREEELEM lpTreeElem )
{
	if( lpTreeElem->m_bOpen == FALSE )
		return lpTreeElem->m_bOpen;
	else
	{
		if( lpTreeElem->m_lpParent )
			return CheckParentTreeBeOpened( lpTreeElem->m_lpParent );
		else
			return lpTreeElem->m_bOpen;
	}
}
CPtrArray* CWndTreeCtrl::GetTreeItemArray( void )
{
	return &m_treeItemArray;
}
void CWndTreeCtrl::SetFocusElem( const LPTREEELEM pFocusElem )
{
	m_pFocusElem = pFocusElem;
}
const LPTREEELEM CWndTreeCtrl::GetFocusElem( void ) const
{
	return m_pFocusElem;
}
void CWndTreeCtrl::SetLineSpace( int nLineSpace )
{
	m_nLineSpace = nLineSpace;
}
int CWndTreeCtrl::GetLineSpace( void ) const
{
	return m_nLineSpace;
}
void CWndTreeCtrl::SetMemberCheckingMode( BOOL bMemberCheckingMode )
{
	m_bMemberCheckingMode = bMemberCheckingMode;
}
BOOL CWndTreeCtrl::GetMemberCheckingMode( void ) const
{
	return m_bMemberCheckingMode;
}

void CWndTreeCtrl::SetTreeTabWidth( int nTreeTabWidth )
{
	m_nTreeTabWidth = nTreeTabWidth;
}
int CWndTreeCtrl::GetTreeTabWidth( void ) const
{
	return m_nTreeTabWidth;
}
void CWndTreeCtrl::SetCategoryTextSpace( int nCategoryTextSpace )
{
	m_nCategoryTextSpace = nCategoryTextSpace;
}
int CWndTreeCtrl::GetCategoryTextSpace( void ) const
{
	return m_nCategoryTextSpace;
}
int CWndTreeCtrl::GetTreeItemsNumber( void ) const
{
	int nTreeItemsNumber = 0;
	CalculateTreeItemsNumber( nTreeItemsNumber, m_treeElem.m_ptrArray );
	return nTreeItemsNumber;
}
void CWndTreeCtrl::SetTreeItemsMaxWidth( int nTreeItemsMaxWidth )
{
	m_nTreeItemsMaxWidth = nTreeItemsMaxWidth;
}
int CWndTreeCtrl::GetTreeItemsMaxWidth( void ) const
{
	return m_nTreeItemsMaxWidth;
}
int CWndTreeCtrl::GetTreeItemsMaxHeight( void )
{
	int nTreeItemsMaxHeight = 6;
	CalculateTreeItemsMaxHeight( nTreeItemsMaxHeight, m_treeElem.m_ptrArray );
	return nTreeItemsMaxHeight;
}

void CWndTreeCtrl::SetMaxCheckNumber( int nMaxCheckNumber )
{
	m_nMaxCheckNumber = nMaxCheckNumber;
}
int CWndTreeCtrl::GetMaxCheckNumber( void ) const
{
	return m_nMaxCheckNumber;
}

void CWndTreeCtrl::SetTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor )
{
	CalculateTextColor( dwCategoryTextColor, dwNormalTextColor, dwSelectedCategoryTextColor, dwSelectedNormalTextColor, m_treeElem.m_ptrArray );
}
void CWndTreeCtrl::CalculateTreeItemsNumber( int& nSumTreeItemsNumber, const CPtrArray& rPtrArray ) const
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
 		if( lpTreeElem )
		{
			++nSumTreeItemsNumber;
			if( lpTreeElem->m_ptrArray.GetSize() > 0 )
				CalculateTreeItemsNumber( nSumTreeItemsNumber, lpTreeElem->m_ptrArray );
		}
	}
}
void CWndTreeCtrl::CalculateTreeItemsMaxHeight( int& nSumHeight, const CPtrArray& rPtrArray )
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
		if( lpTreeElem )
		{
			nSumHeight += GetFontHeight();
			if( lpTreeElem->m_ptrArray.GetSize() > 0 && lpTreeElem->m_bOpen == TRUE )
				CalculateTreeItemsMaxHeight( nSumHeight, lpTreeElem->m_ptrArray );
		}
	}
}
void CWndTreeCtrl::CalculateTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor, const CPtrArray& rPtrArray )
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
 		if( lpTreeElem )
		{
			if( lpTreeElem->m_ptrArray.GetSize() > 0 || lpTreeElem->m_lpParent == NULL )
			{
				lpTreeElem->m_dwColor = dwCategoryTextColor;
				lpTreeElem->m_dwSelectColor = dwSelectedCategoryTextColor;
				CalculateTextColor( dwCategoryTextColor, dwNormalTextColor, dwSelectedCategoryTextColor, dwSelectedNormalTextColor, lpTreeElem->m_ptrArray );
			}
			else
			{
				lpTreeElem->m_dwColor = dwNormalTextColor;
				lpTreeElem->m_dwSelectColor = dwSelectedNormalTextColor;
			}
		}
	}
}


void CWndTreeCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );
	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}



void CWndTreeCtrl::PaintTree(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray) 
{
	LPTREEELEM pTreeElem;
	LPTREEITEM pTreeItem;
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		pTreeElem = (LPTREEELEM)ptrArray.GetAt( i );
		pTreeItem = new TREEITEM;
		CSize sizeStr;
		p2DRender->m_pFont->GetTextExtent( pTreeElem->m_strKeyword, &sizeStr);

		int nRectLeft = pt.x + m_nCategoryTextSpace;
		int nRectTop = pt.y;
		int nRectRight = pt.x + m_nCategoryTextSpace + sizeStr.cx;
		int nRectBottom = pt.y + sizeStr.cy;
		pTreeItem->m_rect.SetRect( nRectLeft, nRectTop, nRectRight, nRectBottom );
		m_nTreeItemsMaxWidth = ( nRectRight > m_nTreeItemsMaxWidth ) ? nRectRight : m_nTreeItemsMaxWidth;
		if( m_bMemberCheckingMode == TRUE && pTreeElem->m_pWndCheckBox )
		{
			CWndButton* pWndCheckBox = pTreeElem->m_pWndCheckBox;
			if( pTreeElem->m_ptrArray.GetSize() == 0 )
			{
				pWndCheckBox->EnableWindow( TRUE );
				pWndCheckBox->SetVisible( TRUE );
				pWndCheckBox->SetWndRect( CRect( pt.x, pt.y - 1, pt.x + CHECK_BOX_SIZE_XY, pt.y + CHECK_BOX_SIZE_XY - 1 ) );
			}
		}

		pTreeItem->m_lpTreeElem = pTreeElem;
		m_treeItemArray.Add( pTreeItem );
		if( pTreeElem->m_ptrArray.GetSize() )
		{

			if( pTreeElem->m_bOpen )
				p2DRender->RenderTexture( pt, m_pTexButtOpen );
			else
				p2DRender->RenderTexture( pt, m_pTexButtClose );

		}

		if( m_pFocusElem == pTreeElem )
		{
			pTreeElem->m_strKeyword.SetColor( pTreeElem->m_dwSelectColor );
			pTreeElem->m_strKeyword.SetStyle( ESSTY_BOLD );
			p2DRender->TextOut_EditString( nRectLeft, nRectTop, pTreeElem->m_strKeyword );
		}
		else
		{
			pTreeElem->m_strKeyword.SetColor( pTreeElem->m_dwColor );
			pTreeElem->m_strKeyword.ClearStyle( ESSTY_BOLD );
			p2DRender->TextOut_EditString( nRectLeft, nRectTop, pTreeElem->m_strKeyword );
		}

		pt.y += GetFontHeight();
		if( pTreeElem->m_ptrArray.GetSize() && pTreeElem->m_bOpen )
		{
			pt.x += m_nTreeTabWidth;
			PaintTree( p2DRender, pt, pTreeElem->m_ptrArray );
			pt.x -= m_nTreeTabWidth;
		}
	}
}

void CWndTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if(pTreeItem->m_rect.PtInRect(point))
		{
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			
			CWndBase* pWnd = m_pParentWnd;
			if( NULL == pWnd )
				return ;

			// gmpbigsun( 20110726 ) : 일단 직부모한테 알려준다
			pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)m_pFocusElem );

			// 직 부모에게 부모가 없다면 리턴
			if( NULL == pWnd->GetParentWnd() )
				return ;
			else
			{
				// 부모가 있다면 최상의 부모를 찾아서 알려줌
				// 기존 로직인데.. 왜 이렇게 만들었을까 어쩔수 없이 살려둠...
				while(pWnd->GetStyle() & WBS_CHILD)
					pWnd = pWnd->GetParentWnd();
				
				if( NULL != pWnd )
					pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)m_pFocusElem );
			}
			return;
		}
	}
}

void CWndTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if( m_bMemberCheckingMode == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened( pParentTreeElem ) == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}
		CRect rect = pTreeItem->m_rect;
		SIZE size = m_pTexButtOpen->m_size;
		rect.left -= m_nCategoryTextSpace;
		rect.SetRect( rect.left, rect.top, rect.left + size.cx, rect.top + size.cy );
		if( rect.PtInRect( point ) )
		{
			pTreeItem->m_lpTreeElem->m_bOpen = !pTreeItem->m_lpTreeElem->m_bOpen;
			m_pFocusElem = pTreeItem->m_lpTreeElem;
		}
	}
}

void CWndTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);

		if( m_bMemberCheckingMode == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened( pParentTreeElem ) == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}

		if(pTreeItem->m_rect.PtInRect(point))
		{
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			m_pFocusElem->m_bOpen = !m_pFocusElem->m_bOpen;
			CWndBase* pWnd = m_pParentWnd;
			while( pWnd->GetStyle() & WBS_CHILD )
				pWnd = pWnd->GetParentWnd();
			pWnd->OnChildNotify( WNM_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusElem );
		}
	}
}
void CWndTreeCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );
	CWndBase::OnSize( nType, cx, cy);
}
BOOL CWndTreeCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if(m_wndScrollBar.GetScrollPage() < m_wndScrollBar.GetMaxScrollPos())	//스크롤이 활성화 되었을 때에만 동작하자.
	{
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
	}
	
	return TRUE;
}
BOOL CWndTreeCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

