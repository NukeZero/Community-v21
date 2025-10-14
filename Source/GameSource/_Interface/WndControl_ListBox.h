

#pragma once


#include "WndBase.h"

namespace WndListBox
{
	static const BYTE		MAX_BACKGROUND_TEXTRUE_COUNT( 9 );
	static const BYTE		TEXTOUT_OFFSET( 4 ); 
}

//---------------------------------------------------------------------------------------------
// 윈도우 리스트 박스 클레스
//---------------------------------------------------------------------------------------------

class CWndListBox : public CWndBase
{
	void InterpriteScript(CScanner& scanner,CPtrArray& ptrArray); 
	void PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray);

public:
	typedef struct tagITEM
	{
		CRect      m_rect;
		CEditString m_strWord;
		BOOL		m_bIsValid;
		BOOL		m_bIsVisible;
		DWORD      m_dwData;
		CString    m_strKey;
		DWORD      m_dwData2;
		tagITEM( void ) : m_rect( 0, 0, 0, 0 ), m_strWord( _T("") ), m_bIsValid( TRUE ), m_bIsVisible( TRUE ), m_dwData( 0 ), m_strKey( _T("") ), m_dwData2( 0 ) {}
	} LISTITEM,* LPLISTITEM;

protected:
	CPtrArray m_listItemArray;
	LPLISTITEM m_pFocusItem;
	int           m_nCurSelect  ;
	LISTITEM      m_listItem    ;
	CWndScrollBar m_wndScrollBar;
	BOOL		  m_bStyleState;

public:
	DWORD         m_nSelectColor;
private:
	DWORD         m_dwOnMouseColor;
	DWORD         m_dwInvalidColor;
	DWORD         m_nLeftMargin;
	DWORD		  m_dwFontStyle;



public:
	int           m_nLineSpace  ;
	int           m_nFontHeight ;

	//gmpbigsun( 20111130 ) : WNM_ONITEM 메세지 처리여부
	BOOL          m_bOnItem; // message마다 처리를 해줘야 하는데 막코딩 되있어서 일단 이렇게...

	CWndListBox();
	~CWndListBox();

	BOOL Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID );
	BOOL Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID, int nR, int nG, int nB );
	int GetFontHeight() { return m_pFont->GetMaxHeight() + m_nLineSpace; }
	int   GetCount() const;
	DWORD GetItemData(int nIndex) const;
	void* GetItemDataPtr(int nIndex) const;
	int   SetItemData(int nIndex,DWORD dwItemData);
	int   SetItemDataPtr(int nIndex,void* pData);

	DWORD GetItemData2( int nIndex ) const;
	void* GetItemData2Ptr( int nIndex ) const;
	BOOL GetItemValidity( int nIndex );
	BOOL GetItemVisibility( int nIndex );
	int SetItemData2( int nIndex,DWORD dwItemData );
	int SetItemData2Ptr( int nIndex,void* pData );
	int SetItemValidity( int nIndex, BOOL bValidity );
	int SetItemVisibility( int nIndex, BOOL bIsVisible );

	const CRect& GetItemRect( int nIndex ) const;

	int   GetSel(int nIndex) const;
	int   GetText(int nIndex,LPSTR lpszBuffer) const;
	void  GetText(int nIndex,CString& rString) const;
	int   GetTextLen(int nIndex) const;
	
	int   GetCurSel() const;
	int   SetCurSel(int nSelect);
	int   SetSel(int nIndex,BOOL bSelect = TRUE);
	int   GetSelCount() const;
	int   GetSelItems(int nMaxItems,LPINT rgIndex) const;
	int		GetScrollWidth( void );

	int   GetScrollPos() {return m_wndScrollBar.GetScrollPos();};
	void  SetScrollPos( int nPos, BOOL bRedraw = TRUE ) { m_wndScrollBar.SetScrollPos( nPos, bRedraw ); }	//gmpbigsun: added
	int   AddString(LPCTSTR lpszItem);
	int   AddString(LPCTSTR lpszItem, DWORD color, DWORD style );
	int   DeleteString(UINT nIndex);
	int   InsertString(int nIndex,LPCTSTR lpszItem);
	void	SetString( int nIndex, LPCTSTR lpszItem );

	void  SetStyleState(BOOL bState,DWORD dwStyle = ESSTY_BOLD); //폰트 스타일 변경하고싶을때 

	const CString& GetString( int nIndex ) const;
	void SetListStringAlpha( int nIndex, BYTE byAlpha );
	void SetKeyString( int nIndex, LPCTSTR lpszItem );
	const CString& GetKeyString( int nIndex ) const;
	void SetOnMouseColor( DWORD dwOnMouseColor );
	void SetOnSelectColor( DWORD dwOnSelectColor );
	DWORD GetOnMouseColor( void ) const;
	void SetInvalidColor( DWORD dwInvalidColor );
	DWORD GetInvalidColor( void ) const;
	void SetLeftMargin( int nLeftMargin );
	int GetLeftMargin( void ) const;

#ifdef __IMPROVE_MAP_SYSTEM
	int GetItemIndex( const CString& strItem ) const;
	int GetItemIndex( DWORD dwItem ) const;
#endif // __IMPROVE_MAP_SYSTEM
	void  ResetContent();
	int   FindString(int nStartAfter,LPCTSTR lpszItem) const;
	int   FindStringExact(int nIndexStart,LPCTSTR lpszItem) const;
	int   SelectString(int nStartAfter,LPCTSTR lpszItem);
	void  SortListBox();
	BOOL IsInValidArea( CPoint point );		//마우스포인터가 위치한곳에 아이템이 있는가?

	void LoadListBoxScript(LPCTSTR lpFileName); 

	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSetFocus( CWndBase* pOldWnd );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	
	friend int QSortListBox( const VOID* arg1, const VOID* arg2 );
};


//---------------------------------------------------------------------------------------------
// 윈도우 콤보 박스 리스트 컨트롤 클레스
//---------------------------------------------------------------------------------------------
class CWndComboListBox : public CWndListBox
{

public:

							CWndComboListBox();
							~CWndComboListBox();

	void					LoadBackgroundTexture( void );
	void					DrawBackground( C2DRender* p2DRender );

#ifdef __IMPROVE_MAP_SYSTEM
	virtual	void			PaintFrame( C2DRender* p2DRender );
#endif 
	virtual void			OnKillFocus( CWndBase* pNewWnd );
	virtual void			AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );


private:


	CTexture*				m_pTextureBackground[WndListBox::MAX_BACKGROUND_TEXTRUE_COUNT];

};