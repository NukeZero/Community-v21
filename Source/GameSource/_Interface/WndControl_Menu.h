
#pragma once

namespace WndMenu
{
	static const UINT		MAX_BACKGROUND_TEXTRUE_COUNT( 12 );
}

//---------------------------------------------------------------------------------------------
// 윈도우 메뉴 클레스
//---------------------------------------------------------------------------------------------
class CWndMenu : public CWndBase
{

public:


							CWndMenu();
							~CWndMenu();

	BOOL 					CreateMenu( CWndBase* pWndParent );
	BOOL 					DeleteMenu(UINT nPosition, UINT nFlags );
	void 					DeleteAllMenu( void );
	
	void 					SetVisibleSub( BOOL bVisible );
	void 					SetVisibleAllMenu( BOOL bVisible );

	virtual CWndButton*		AppendMenu(UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);
	BOOL					InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,	LPCTSTR lpszNewItem = NULL);

	UINT 					CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
	UINT 					GetMenuState(UINT nID, UINT nFlags) const;

	CWndButton*				GetMenuItem( int nPos );

	void					SetLargeWidth( int nLargeWidth )		{	m_nLargeWidth = nLargeWidth;	}

	virtual	void 			OnInitialUpdate( void ); 
	virtual void 			OnKillFocus( CWndBase* pNewWnd );
	virtual BOOL 			OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL 			OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL 			OnEraseBkgnd(C2DRender* p2DRender);
	virtual void 			PaintFrame( C2DRender* p2DRender );
	virtual void 			SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );


protected:


	BOOL					IsOnMenu(CPoint pt);
	void 					LoadBackgroundTexture( void );
	void 					DrawBackground( C2DRender* p2DRender );


public:


	CPtrArray				m_awndMenuItem;


protected:


	int						m_nLargeWidth;
	WORD					m_wButtonWidth;
	CTexture*				m_pTextureBackground[WndMenu::MAX_BACKGROUND_TEXTRUE_COUNT];
};