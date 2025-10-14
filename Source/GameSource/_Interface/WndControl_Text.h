
#pragma once

#include "WndBase.h"
#include "WndControl_Menu.h"
#include "WndControl_ScrollBar.h"

//---------------------------------------------------------------------------------------------
// 윈도우 텍스트 클레스 
//---------------------------------------------------------------------------------------------
class CWndText			: public CWndBase
{


public:

						CWndText();
						~CWndText();

	BOOL				Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
	BOOL				Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR, int nFontColorG, int nFontColorB );

	virtual	void		DrawCaret( C2DRender* p2DRender ) {}

	void				ResetString( void );
	void				UpdateScrollBar( void );

	void				Insert( int nIndex, LPCTSTR pstr );
	void				Delete( int nIndex, int nLen );

	void 				BlockSetColor( DWORD dwColor );
	void 				BlockSetStyle( DWORD dwStyle );
	void 				BlockClearStyle( DWORD dwStyle );
	void 				BlockDelete( LPCTSTR pstr, LPCTSTR pstr2 );
	void 				BlockInsert( LPCTSTR pstr, LPCTSTR pstr2 );

	CPoint				OffsetToPoint( DWORD dwOffset, TCHAR* pszStr, size_t cchStr );
	virtual void 		SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );

	void				SetString( LPCTSTR pszString );
	void				SetString( LPCTSTR pszString, DWORD dwColor );

	void				AddString( LPCTSTR pszString );
	void				AddString( LPCTSTR pszString, DWORD dwColor );
	void				AddString( LPCTSTR pszString, DWORD dwColor, DWORD dwPStyle );

	virtual	void 		OnInitialUpdate();
	virtual void 		OnDraw( C2DRender* p2DRender );
	virtual BOOL 		OnCommand ( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL 		OnChildNotify ( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual void 		OnSetFocus( CWndBase* pOldWnd );
	virtual void 		OnKillFocus( CWndBase* pNewWnd );
	virtual void 		OnMouseMove( UINT nFlags, CPoint point );
	virtual BOOL 		OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void 		OnLButtonUp( UINT nFlags, CPoint point );
	virtual void 		OnRButtonUp( UINT nFlags, CPoint point );
	virtual void 		OnLButtonDown( UINT nFlags, CPoint point );
	virtual void 		OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void 		OnSize( UINT nType, int cx, int cy );
	virtual void 		OnChar( UINT nChar );

	void				HideCaret( void )				{		m_bCaretVisible = FALSE;							}
	void				ShowCaret( void )				{		m_bCaretVisible = TRUE;								}			

	LONG				GetOffset( CPoint point );
	CPoint				GetCaretPos( void )				{		return m_ptCaret;									}
	int					GetFontHeight( void )			{		return m_pFont->GetMaxHeight() + m_nLineSpace;		}
	DWORD				GetBlockEnd( void )				{		return m_dwBlockEnd;								}
	DWORD				GetBlockBegine( void )			{		return m_dwBlockBegin;								}

	BOOL				IsEmptyBlock( void )			{		return m_dwBlockBegin == m_dwBlockEnd;				}

	void				SetCaretPos( CPoint ptCaret )	{	m_ptCaret = ptCaret; m_timerCaret.Reset();				}


public:


	CEditString			m_string;

	int					m_nLineSpace; 
	BOOL				m_bEnableClipboard;

	CPoint				m_ptDeflateRect;

	CWndMenu			m_wndMenu;
	CWndScrollBar		m_wndScrollBar;
	
	TCHAR				m_TextComp[3];
	TCHAR				m_szCaret[ 3 ];


protected:


	BOOL				m_bScr;
	BOOL   				m_bCaretVisible;
	BOOL   				m_bLButtonDown;

	int					m_nLineRefresh;

	DWORD 				m_dwBlockBegin;
	DWORD 				m_dwBlockEnd;
	DWORD 				m_dwOffset;

	CPoint 				m_ptCaret;
	CTimer 				m_timerCaret;

};


