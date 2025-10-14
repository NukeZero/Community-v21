
#include "FLListView.h"


FLListView::FLListView()
{
	m_hWnd = NULL;
}

FLListView::~FLListView()
{
	Destroy();
}

bool	FLListView::Create( HWND hDlg, int nDlgItemID )
{
	m_hWnd = ::GetDlgItem( hDlg, nDlgItemID );
	if( m_hWnd == NULL )
	{
		return false;
	}

	return true;
}

void	FLListView::Destroy()
{
	m_hWnd = NULL;
}

void	FLListView::GetRect( RECT* pRect )
{
	if( m_hWnd != NULL && pRect != NULL )
	{
		::GetClientRect( m_hWnd, pRect );
	}
}

int		FLListView::GetCount()
{
	int nCount = 0;
	if( m_hWnd != NULL )
	{
		nCount = ListView_GetItemCount( m_hWnd );
	}

	return nCount;
}

void	FLListView::SetColumn( int nCount, const TCHAR** szNames, const int* pWidths )
{
	if( m_hWnd != NULL && nCount > 0 && szNames != NULL && pWidths != NULL )
	{
		for( int i = 0; i < nCount; ++i )
		{
			LVCOLUMN tLVColumn;
			::memset( &tLVColumn, 0, sizeof( tLVColumn ) );

			tLVColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			tLVColumn.fmt = LVCFMT_LEFT;

			tLVColumn.iSubItem = i;
			tLVColumn.pszText = const_cast< TCHAR* >( szNames[i] );
			tLVColumn.cx = pWidths[i];

			ListView_InsertColumn( m_hWnd, i, &tLVColumn );
		}
	}
}

void	FLListView::SetStyle( DWORD dwExStyle )
{
	if( m_hWnd != NULL )
	{
		ListView_SetExtendedListViewStyle( m_hWnd, dwExStyle );
	}
}

void	FLListView::SetScroll( int nPos )
{
	if( m_hWnd != NULL && nPos >= 0 )
	{
		ListView_Scroll( m_hWnd, 0, nPos );
	}
}

void	FLListView::EnsureVisible( int nPos )
{
	if( m_hWnd != NULL )
	{
		ListView_EnsureVisible( m_hWnd, nPos, TRUE );
	}
}

void	FLListView::InsertItem( int nCount, const TCHAR** szItems )
{
	if( m_hWnd != NULL && nCount > 0 && szItems != NULL )
	{
		LVITEM tLVItem;

		tLVItem.mask = LVIF_TEXT;
		tLVItem.state = 0;
		tLVItem.stateMask = 0;

		tLVItem.iItem = GetCount();
		tLVItem.iSubItem = 0;
		tLVItem.pszText = const_cast< TCHAR* >( szItems[0] );
		ListView_InsertItem( m_hWnd, &tLVItem );

		for( int i = 1; i < nCount; ++i )
		{
			tLVItem.iSubItem = i;
			tLVItem.pszText = const_cast< TCHAR* >( szItems[i] );
			ListView_SetItem( m_hWnd, &tLVItem );
		}
	}
}
