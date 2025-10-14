
#if !defined(AFX_WNDBASE_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_)
#define AFX_WNDBASE_H__0B45596D_70D7_48A4_BCB2_3D0F32F58E57__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif 

#include "ResManager.h"

#define	WSIZE_WINDOW			0
#define	WSIZE_MIN				1
#define	WSIZE_MAX				2

#define	EDIT_HEIGHT				8

#define D3DCOLOR_TEMP(a,b,g,r)	((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))) 

// 클라이언트의 좌표 얻기 
#define GET_CLIENT_POINT( hwnd, pt ) CPoint pt; ::GetCursorPos( &pt ); ::ScreenToClient( hwnd, &pt );

class CWndBase;
class C2DRender;


//---------------------------------------------------------------------------------------------
// 윈도우 메시지 구조체
//---------------------------------------------------------------------------------------------
typedef struct tagWNDMESSAGE
{
	CWndBase* m_pWndBase ;
	UINT      m_message  ;
	WPARAM    m_wParam   ;
	LPARAM    m_lParam   ;

	tagWNDMESSAGE()
	:	m_pWndBase( NULL )
	,	m_message( NULL )
	,	m_wParam( NULL )
	,	m_lParam( NULL )
	{
	}
} WNDMESSAGE,* LPWNDMESSAGE;


//---------------------------------------------------------------------------------------------
// 윈도우 기본 클레스
//---------------------------------------------------------------------------------------------
class CWndBase  
{

friend class CWndButton;

public:


								CWndBase();
	virtual						~CWndBase();


	virtual HRESULT				InitDeviceObjects( void );
	virtual HRESULT				DeleteDeviceObjects( void );
	virtual HRESULT				RestoreDeviceObjects( void );
	virtual HRESULT				InvalidateDeviceObjects( void );


	BOOL						Create( DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID );
	void						Destroy( BOOL bAutoFree = FALSE ); 
	void						DestroyAllWnd( CWndBase* pWndRoot ); 


	virtual BOOL				Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0);


	void						RenderWnd( void );
	void						Paint( C2DRender* p2DRender, BOOL bPaintChild = TRUE );
	void						PaintChild( C2DRender* p2DRender );
	virtual void				PaintRoot( C2DRender* p2DRender );
	virtual	void				PaintFrame( C2DRender* p2DRender );
	void						GradationRect( C2DRender* p2DRender, CRect rect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent = 40 );


	void						SetTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName, BOOL bMyLoader = FALSE );
	void						SetTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpKey, CTexture* m_pTexture );
	static BOOL					SetForbidTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName );
	static void					SetForbid( BOOL bForbid );
	void						MakeVertexBuffer( void );
	static void					FreeTileTexture( void );
	void						FitTextureSize( void );


	virtual void				AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void				AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void				AdjustWndBase( D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	virtual void				AdjustWndBase2( D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	BOOL 						AdjustMinRect( CRect* pRect, int nWidth, int nHeight );
	BOOL 						AdjustMaxRect( CRect* pRect, int nWidth, int nHeight );


	static void					ClipStrArray( C2DRender* p2DRender,CRect rect,int nLineSpace, CStringArray* pStringArray,CStringArray* pNewStringArray );
	virtual void				AlighWindow( CRect rcOld, CRect rcNew );
	void						EnableWindow( BOOL bEnable = TRUE )				{	m_bEnable = (bEnable != FALSE );	}


	void 						AddWnd( CWndBase* pWnd ); 
	void 						RemoveWnd( CWndBase* pWnd );
	void 						RemoveDestroyWnd( void );
	void 						Move( CPoint pt ); 
	void 						Move( int x, int y )							{	Move( CPoint(x, y) );				}  
	void 						SetWndStyle( DWORD dwStyle ) 					{ 	m_dwStyle = dwStyle;				}
	void 						AddWndStyle( DWORD dwStyle ) 					{ 	m_dwStyle |= dwStyle;				}
	void 						DelWndStyle( DWORD dwStyle ) 					{ 	m_dwStyle &= ~dwStyle;				}
	void 						MoveParentCenter( void );						// 윈도를 부모 윈도의 중앙으로 옮긴다.
	void 						MovePointCenter( CPoint pt );					// 윈도를 포인트의 중앙으로 옮긴다.
	void 						MoveRectCenter( CRect rect );					// 윈도를 영역의 중앙으로 옮긴다.
	CRect						MakeCenterRect( int nWidth, int nHeight );


	BOOL 						WindowToScreen( LPPOINT lpPoint	);
	BOOL 						WindowToScreen( LPRECT	lpRect  );
	BOOL 						ClientToScreen( LPPOINT lpPoint );
	BOOL 						ClientToScreen( LPRECT	lpRect  );
	BOOL 						ScreenToWindow( LPPOINT lpPoint );
	BOOL 						ScreenToWindow( LPRECT	lpRect  );
	BOOL 						ScreenToClient( LPPOINT lpPoint );
	BOOL 						ScreenToClient( LPRECT	lpRect  );


	void 						SetCapture( void );
	void 						ReleaseCapture( void );


	BOOL 						IsDestroy( void );
	BOOL 						IsParentWnd(CWndBase* pParentWnd);
	BOOL 						IsOpenWnd( CWndBase* pWnd );
	BOOL 						IsOpenWnd( UINT nId );
	BOOL 						IsOnWndBase( CPoint pt );  
	CWndBase*					IsOnWndBase2( CPoint pt );				//by gmpbigsun( 20120104 ) : pt위에 있는 가장 마지막에 그려지는 Wnd
	BOOL 						IsOpenModalWnd( void );
	BOOL 						IsOpenWnd( void )								{	return m_nIdWnd ? TRUE : FALSE;											}
	BOOL						IsPush( void )									{	return m_bPush;															}
	BOOL 						IsGroup( void )									{	return m_bGroup;														}
	BOOL 						IsDefault( void )								{	return m_bDefault; 														}
	BOOL 						IsFullWnd( void )								{	return m_bFullWnd; 														}
	BOOL 						IsWndRoot( void )								{	return this == m_pWndRoot;												}
	BOOL 						IsVisible( void )								{	return m_bVisible;														} 
	BOOL 						IsAutoFree( void )								{	return m_bAutoFree;														}
	BOOL 						IsFocusWnd( void )								{	return m_pWndFocus == this;												}
	BOOL 						IsFocusChild( void )							{	return m_pParentWnd ? m_pParentWnd->m_pWndFocusChild == this : FALSE;	}
	BOOL 						IsWindowEnabled() const							{	return m_bEnable;														}
	BOOL 						IsWndStyle( DWORD dwStyle )						{	return (m_dwStyle & dwStyle) ? TRUE : FALSE;							}
	static BOOL					IsForbid( void )								{	return m_pTexForbid && m_bForbid;										}
	virtual BOOL				IsPickupSpace( CPoint point ); 


	void 						SetGroup( BOOL bGroup )							{ 	m_bGroup = bGroup;														}
	void 						SetWndId( DWORD dwWndId )						{ 	m_nIdWnd = dwWndId;														}
	void 						SetAutoFree( BOOL bFree )						{ 	m_bAutoFree = (bFree != FALSE );										}
	void						SetWndType( int nWndType )						{	m_byWndType = (BYTE)nWndType;											}
	void 						SetFont( CD3DFont* pFont )						{ 	m_pFont = pFont;														}
	void 						SetVisible( BOOL bVisible )						{ 	m_bVisible = ( bVisible != FALSE );										} 
	void 						SetTabStop( BOOL bTabStop )						{ 	m_bTabStop = (bTabStop != FALSE);										}
	void 						SetDefault( BOOL bDefault )						{ 	m_bDefault = (bDefault != FALSE);										}
	void 						SetTitle( CString strTitle )					{ 	m_strTitle = strTitle;													}
	void						SetTileColor( DWORD dwColor )					{	m_dwColor = dwColor;													} 
	void 						SetToolTip( LPCTSTR lpszToolTip, int nPos = 0 ) { 	m_strToolTip = lpszToolTip; m_nToolTipPos = nPos;						}
	virtual void				SetFocus( void );
	void						SetWndSize( int cx, int cy );
	void						SetFontColor( int nR, int nG, int nB );
	void						SetChildFocus( CWndBase* pWndBase, POINT point );
	virtual	void				SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );


	CD3DFont*					GetFont( void )									{ 	return m_pFont;															}
	int							GetWndId( void )								{ 	return m_nIdWnd;														}
	CString						GetTitle( void )								{ 	return m_strTitle;														} 
	DWORD						GetStyle( void )								{ 	return m_dwStyle;														}
	DWORD						GetWndType( void )								{ 	return (DWORD)m_byWndType;												}
	CRect						GetWndRect( void )								{ 	return m_rectWindow;													}
	CPtrArray*					GetWndArray( void )								{ 	return &m_wndArray;														}
	CWndBase*					GetFocusWnd( void )								{ 	return m_pWndFocus;														}
	int							GetWndStyle( void )								{ 	return m_dwStyle;														}
	CTexture*					GetTexture( void )								{	return m_pTexture;														}
	LPWNDAPPLET 				GetWndApplet( void )							{ 	return m_resMng.GetAt( m_nIdWnd );										}
	DWORD						GetFontColor( void )							{ 	return m_dwColor;														}
	DWORD						GetTileColor( void )							{ 	return m_dwColor;														}
	CWndBase*					GetParentWnd( void )							{ 	return m_pParentWnd;													}
	DWORD						GetFontColorR( void )							{ 	return m_nFontColorR;													}
	DWORD						GetFontColorG( void )							{ 	return m_nFontColorG;													}
	DWORD						GetFontColorB( void )							{ 	return m_nFontColorB;													}
	CWndBase*					GetFocusChild( void )							{ 	return m_pWndFocusChild;												}
	CPoint						GetMousePoint( void )							{ 	return m_ptMouse;														} 
	LPWNDCTRL					GetWndCtrl( DWORD dwWndId ) 					{ 	return m_resMng.GetAtControl( m_nIdWnd, dwWndId );						}
	CWndBase*					GetWndBase( void );
	CWndBase*					GetFrameWnd( void );
	int							GetFontHeight( void );
	CRect						GetScreenRect( void );
	CWndBase*					GetDlgItem( UINT nID ); 
	CPoint						GetClientCenter( void ); 
	CWndBase*					GetChildWnd( UINT nID );
	CWndBase*					GetWndBase( UINT idWnd );
	virtual int					GetResizeDir( CPoint ptClient );
	CRect						GetClientRect( BOOL bParent = FALSE );
	CRect						GetWindowRect( BOOL bParent = FALSE );
	CRect						GetLayoutRect( BOOL bParent = FALSE );
	CPoint						GetStrCenter(C2DRender* p2DRender,LPCTSTR str);					// 화면 중심에서 스트링의 중앙에 출력하기 위한 좌표를 돌려준다.
	CWndBase*					GetChildFocus( CWndBase* pWndBase, POINT point );
	void						GetLogFont(C2DRender* p2DRender,LOGFONT* pLogFont);
	CPoint						GetStrRectCenter(C2DRender* p2DRender,CRect rect,LPCTSTR str);	// 영역의 중심에서 스트링의 중앙에 출력하기 위한 좌표를 돌려준다.


	CWndBase*					FindWnd(UINT idWnd)								{	return GetWndBase( idWnd );												}
	CWndBase*					FindFullWnd( void );

	// 윈도우 메시지 
	LRESULT						SendMessage(UINT message,WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL						PostMessage(UINT message,WPARAM wParam = 0, LPARAM lParam = 0);

	virtual	void				OnInitialUpdate( void );
	virtual	void				OnUpdate( void );
	virtual	BOOL				Process( void );

	virtual void				OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void 				OnDestroy( void );
	virtual void 				OnClose( void );

	// 키보드
	virtual void 				OnChar( UINT nChar );
	virtual void 				OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	virtual void 				OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );

	// 마우스
	virtual void 				OnMouseMove( UINT nFlags, CPoint point );
	virtual BOOL 				OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void				OnLButtonUp( UINT nFlags, CPoint point );
	virtual void				OnLButtonDown( UINT nFlags, CPoint point );
	virtual void 				OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual void				OnRButtonUp( UINT nFlags, CPoint point );
	virtual void				OnRButtonDown( UINT nFlags, CPoint point );
	virtual void 				OnRButtonDblClk( UINT nFlags, CPoint point );
	virtual void 				OnMButtonUp( UINT nFlags, CPoint point );
	virtual void 				OnMButtonDown( UINT nFlags, CPoint point );
	virtual void 				OnMButtonDblClk( UINT nFlags, CPoint point );
	virtual void 				OnNonClientLButtonDblClk( UINT nFlags, CPoint point );
	virtual void				OnMouseWndSurface( CPoint point );

	// 그리기
	virtual void 				OnPaint( void );
	virtual	void				OnDraw( C2DRender* p2DRender );
	virtual BOOL 				OnEraseBkgnd( C2DRender* p2DRender );
	virtual	BOOL				OnDrawIcon( CWndBase* pWndBase,C2DRender* p2DRender );

	// 포커스
	virtual void 				OnSetFocus(CWndBase* pOldWnd);
	virtual void 				OnKillFocus(CWndBase* pNewWnd);

	// 타이머
	virtual void 				OnTimer(UINT nIDEvent);

	virtual void 				OnMove(int x,int y);
	virtual void 				OnSize(UINT nType, int cx, int cy);
	virtual BOOL 				OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual BOOL				OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL				OnSystemNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL				OnParentNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL				OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult );
	virtual BOOL				OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	LRESULT						WindowRootProc( UINT message, WPARAM wParam, LPARAM lParam );
	virtual LRESULT				DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);
	LRESULT						WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
	void						KillFocus( CWndBase* pWndFocus, CWndBase* pWndNew );


private:


	LPDIRECT3DVERTEXBUFFER9		m_pVB;


protected:

	
	static CPoint				m_pointOld;
	static CRect				m_rectOld;


public:


	static HWND           		m_hWnd;

	static CWndBase*      		m_pWndRoot;
	static CWndBase*      		m_pWndFocus;		// 다이얼로그 윈도 포커스 
	static CWndBase*      		m_pCurFocus;		// 다이얼로그, 차일드 중 최종 현재 포커스 
	static CWndBase*			m_pWndCapture;
	static CWndBase*      		m_pWndOnMouseMove;
	static CWndBase*      		m_pWndOnSetCursor;

	static CTexture*      		m_pTexForbid;

	static BOOL           		m_bEdit;
	static BOOL           		m_bCling;
	static BOOL           		m_bForbid;
	static BOOL           		m_bFullWnd;
	static int            		m_nAlpha;

	static CTheme         		m_Theme;
	static CResManager			m_resMng;
	static CPoint         		m_ptForbid;
	static CPtrArray      		m_wndRemove;
	static CTextureMng			m_textureMng;
	static CPtrArray      		m_postMessage;
	static CTimer         		m_timerForbid;
	static CMapStringToPtr		m_strWndTileMap;
	static SHORTCUT			    m_GlobalShortcut;
	static HCURSOR        		m_hDefaultCursor;

	CD3DApplication*			m_pApp;
	CD3DFont* 					m_pFont;
	CTheme*						m_pTheme;
	CTexture*					m_pTexture;
	C2DRender*					m_p2DRender;
	CWndBase*					m_pParentWnd;
	CWndBase* 					m_pWndFocusChild;

	UINT						m_nIdWnd;

	BYTE						m_byWndType;

	DWORD						m_dwStyle;
	DWORD						m_dwColor;

	bool					    m_bTile;
	bool						m_bPush; 
	bool      					m_bEnable;
	bool      					m_bActive;
	bool      					m_bVisible;
	bool      					m_bTabStop;
	bool      					m_bDefault;
	bool      					m_bAutoFree;
	bool						m_bLButtonUp;
	bool						m_bLButtonDown;
	bool						m_bRButtonDown;
	bool						m_bMButtonDown;
	bool						m_bLButtonDowned;

	BOOL						m_bGroup;
	BOOL						m_bPickup;				// 포커스 윈도를 들었나?
	BOOL						m_bKeyButton;
	BOOL						m_bNoCloseButton;
	BOOL						m_bNoHelpButton;
	BOOL						m_bLocalizeNoTileIMG;		

	int							m_nWinSize;				// 0 = nomal, 1 - minimaize, 2 = maximize
	int							m_nResizeDir;
	int       					m_nToolTipPos;
	int       					m_nAlphaCount;
	int							m_nFontColorR;
	int							m_nFontColorG;
	int							m_nFontColorB;

	CRect    					m_rectWindow;			// Window ; 패어런트부터의 좌표 
	CRect    					m_rectClient;			// Client ; 패어런트부터의 좌표 ( 보통 에찌 때문에 m_rectWindow 보다는 조금 안쪽 좌표를 갖는다. )
	CRect    					m_rectLayout;			// Client ; 패어런트부터의 좌표 ( 보통 에찌 때문에 m_rectWindow 보다는 조금 안쪽 좌표를 갖는다. )
	CRect    					m_rectCurrentWindow;
	CRect    					m_rectCurrentClient;

	CString						m_strTitle;
	CString						m_strTexture;
	CString						m_strToolTip;
	CString						m_strDefaultImg;


	CPoint    					m_ptMouse;
	CSize     					m_sizeTile;
	CPtrArray 					m_wndArray;

};

#endif 
