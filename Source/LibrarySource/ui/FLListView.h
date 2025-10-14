
#ifndef	__FLLISTVIEW_H__
#define	__FLLISTVIEW_H__


#include "FLUICommon.h"


class	FLListView
{
public:

	FLListView();
	~FLListView();

	bool	Create( HWND hDlg, int nDlgItemID );
	void	Destroy();

	void	GetRect( RECT* pRect );
	int		GetCount();

	void	SetColumn( int nCount, const TCHAR** szNames, const int* pWidths );
	void	SetStyle( DWORD dwExStyle );
	void	SetScroll( int nPos );
	void	EnsureVisible( int nPos );

	void	InsertItem( int nCount, const TCHAR** szItems );

private:

	HWND	m_hWnd;

};









#endif