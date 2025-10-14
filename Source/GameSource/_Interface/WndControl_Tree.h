

#pragma once


#include "WndBase.h"


//---------------------------------------------------------------------------------------------
// 트리 요소
//---------------------------------------------------------------------------------------------
typedef struct tagScriptElem
{
	tagScriptElem*	m_lpParent;
	CWndButton*		m_pWndCheckBox;
	DWORD			m_dwColor;
	DWORD			m_dwSelectColor;
	DWORD			m_dwData;
	BOOL			m_bOpen;
	CEditString		m_strKeyword;
	CPtrArray		m_ptrArray;

	tagScriptElem( void );
	tagScriptElem( CRect* pRect );

}TREEELEM,* LPTREEELEM;


//---------------------------------------------------------------------------------------------
// 윈도우 트리 컨트롤 클레스
//---------------------------------------------------------------------------------------------
class CWndTreeCtrl				: public CWndBase
{

public:


								CWndTreeCtrl( void );
								~CWndTreeCtrl( void );

	BOOL 						Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd,UINT nID );
	BOOL 						Create( DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd,UINT nID,  int nFontColorR, int nFontColorG, int nFontColorB );
								
	void						LoadTreeScript( LPCTSTR lpFileName ); 

	BOOL						DeleteAllItems( void );
	void						DeleteItemArray( void );

	LPTREEELEM 					InsertItem( LPTREEELEM	lpParent, 
											LPCTSTR		lpString, 
											DWORD		dwData, 
											BOOL		bForbidChecking = TRUE, 
											BOOL		bCheck = FALSE ); 

	LPTREEELEM 					InsertItem( LPTREEELEM	lpParent, 
											LPCTSTR		lpString, 
											DWORD		dwData, 
											BOOL		bForbidChecking, 
											BOOL		bCheck, 
											DWORD		dwColor, 
											DWORD		dwSelectColor );


	LPTREEELEM 					FindTreeElem( DWORD dwData );
	LPTREEELEM 					FindTreeElem( LPCTSTR lpszKeyword );
	LPTREEELEM 					FindTreeElem( CPtrArray& ptrArray, DWORD dwData );
	LPTREEELEM 					FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword );

	LPTREEELEM 					GetCurSel( void );
	LPTREEELEM 					GetRootElem( void );
	LPTREEELEM 					GetNextElem( LPTREEELEM pElem, int& nPos );

	LPTREEELEM 					SetCurSel( DWORD dwData );
	LPTREEELEM 					SetCurSel( LPCTSTR lpszKeyword );

	BOOL						CheckParentTreeBeOpened( LPTREEELEM lpTreeElem );


	CPtrArray*					GetTreeItemArray( void );
	const LPTREEELEM			GetFocusElem( void ) const;
	int							GetLineSpace( void ) const;
	int							GetTreeTabWidth( void ) const;
	int							GetMaxCheckNumber( void ) const;
	int							GetTreeItemsNumber( void ) const;
	int 						GetTreeItemsMaxWidth( void ) const;
	int							GetCategoryTextSpace( void ) const;
	int 						GetTreeItemsMaxHeight( void );
	BOOL 						GetMemberCheckingMode( void ) const;
	int							GetFontHeight( void )					{ return m_pFont->GetMaxHeight() + m_nLineSpace;	}


	void						SetLineSpace( int nLineSpace );
	void						SetTreeTabWidth( int nTreeTabWidth );
	void 						SetMaxCheckNumber( int nMaxCheckNumber );
	void						SetFocusElem( const LPTREEELEM pFocusElem );
	void						SetCategoryTextSpace( int nCategoryTextSpace );
	void						SetTreeItemsMaxWidth( int nTreeItemsMaxWidth );
	void 						SetMemberCheckingMode( BOOL bMemberCheckingMode );
	void						SetTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor );


	virtual void 				OnInitialUpdate( void );
	virtual void 				OnDraw( C2DRender* p2DRender );
	virtual	void 				PaintFrame( C2DRender* p2DRender );
	virtual BOOL 				OnEraseBkgnd( C2DRender* p2DRender );
	virtual void 				OnSize( UINT nType, int cx, int cy );
	virtual void 				OnLButtonUp( UINT nFlags, CPoint point );
	virtual void 				OnLButtonDown( UINT nFlags, CPoint point );
	virtual void 				OnRButtonUp( UINT nFlags, CPoint point );
	virtual void 				OnRButtonDown( UINT nFlags, CPoint point );
	virtual void 				OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void 				OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual	void 				SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual BOOL 				OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );


private:							


	void						InterpriteScript( CScript& script, CPtrArray& ptrArray ); 

	virtual void				PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray );
	void						FreeTree( CPtrArray& ptrArray );

	void 						CalculateTreeItemsNumber( int& nSumTreeItemsNumber, const CPtrArray& rPtrArray ) const;
	void 						CalculateTreeItemsMaxHeight( int& nSumHeight, const CPtrArray& rPtrArray );
	void 						CalculateTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor, const CPtrArray& rPtrArray );


public:							


	enum						{	CHECK_BOX_SIZE_XY = 13	};

	typedef struct tagITEM
	{
		CRect			m_rect;
		BOOL			m_bButton;
		LPTREEELEM		m_lpTreeElem;

		tagITEM()
		:	m_rect( 0, 0, 0, 0 )
		,	m_bButton( FALSE )
		,	m_lpTreeElem( NULL )
		{
		}

	} TREEITEM,* LPTREEITEM;

	DWORD						m_nFontColor; 
	DWORD						m_nSelectColor;

	int							m_nLineSpace;
	BOOL						m_bForceDrawFrame;		//강제로 프레임그리기

	CTexture*					m_pTexButtOpen;
	CTexture*					m_pTexButtClose;


private:


	LPTREEELEM					m_pFocusElem;

	BOOL						m_bMemberCheckingMode;

	int							m_nFontHeight;
	int 						m_nTreeTabWidth;
	int 						m_nMaxCheckNumber;
	int 						m_nCategoryTextSpace;
	int 						m_nTreeItemsMaxWidth;

	TREEELEM					m_treeElem;
	CWndScrollBar				m_wndScrollBar;
	CPtrArray					m_treeItemArray;


};