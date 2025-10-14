

#pragma once


#include "WndBase.h"
#include "WndEditCtrl.h"
#include "WndControl_Button.h"
#include "WndControl_ListBox.h"


namespace WndComboBox
{
	static const UINT		nBackgroundTextureCount( 9 );
}


//---------------------------------------------------------------------------------------------
// 윈도우 콤보 박스 컨트롤 클레스
//---------------------------------------------------------------------------------------------
class CWndComboBox : public CWndEdit
{

public: 

							CWndComboBox();
							~CWndComboBox();


	BOOL					Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR = 0, int nFontColorG = 0, int nFontColorB = 0 );
	void					ResetContent( void );

	void					DrawBackground( C2DRender* p2DRender );

	int 					AddString( LPCTSTR lpszString );
	int 					DeleteString( UINT nIndex );

	int						GetCurSel( void ) const;
	LCID					GetLocale( void ) const;
	DWORD					GetEditSel( void ) const;
	int						GetTopIndex( void ) const;
	DWORD					GetItemData( int nIndex ) const;
	int						GetComboListBoxMaxLine( void ) const;
	CWndComboListBox*		GetListBox( void )		{	return &m_wndListBox;			}

	int						SetCurSel( int nSelect );
	int 					SetItemData(int nIndex, DWORD dwItemData);
	void					SetComboListBoxMaxLine( int nComboListBoxMaxLine );
	void					SetDrawCallFromOutSide( BOOL bKeepDraw );

	virtual	void			OnInitialUpdate( void );
	virtual void			OnLButtonDown( UINT nFlags, CPoint point );
	virtual void			PaintFrame( C2DRender* p2DRender );
	virtual void			OnDraw( C2DRender* p2DRender );
	virtual void			SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual BOOL			OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );

#ifdef __IMPROVE_MAP_SYSTEM

	DWORD					GetSelectedItemData( void ) const;
	void					GetListBoxText( int nIndex, CString& strString ) const;
	int						GetListBoxTextLength( int nIndex ) const;
	int						GetListBoxSize( void ) const;
	void					SelectItem( const CString& strItem );
	void					SelectItem( DWORD dwItem, BOOL bMessage = TRUE );

#endif 


private:

	void					CreateButton( void );
	void					CreateListBox( void );

	void					OpenListBox( void );

	void					LoadBackgroundTexture( void );

#ifdef __IMPROVE_MAP_SYSTEM
	void					SetSelectedItemInformation( BOOL bMessage = TRUE );
#endif

public:


	CWndComboListBox		m_wndListBox;
	CWndButton				m_wndButton;


protected:


	enum					{ COMBO_LIST_BOX_MARGIN = 10 };
	int						m_nComboListBoxMaxLine;
	BOOL					m_bOpen;
	BOOL					m_bDrawCallFromOutSide;

	CTexture*				m_pTextureBackground[WndComboBox::nBackgroundTextureCount];
};









