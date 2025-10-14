
#include "FLComboBox.h"


FLComboBox::FLComboBox()
{
	m_hWnd = NULL;
}

FLComboBox::~FLComboBox()
{
	Destroy();
}

bool	FLComboBox::Create( HWND hDlg, int nDlgItemID )
{
	m_hWnd = ::GetDlgItem( hDlg, nDlgItemID );
	if( m_hWnd == NULL )
	{
		return false;
	}

	return true;
}

void	FLComboBox::Destroy()
{
	m_hWnd = NULL;
}

void	FLComboBox::AddItem( const TCHAR* szString )
{
	::SendMessage( m_hWnd, CB_ADDSTRING, 0, reinterpret_cast< LPARAM >( szString ) );
}

void	FLComboBox::SetCurSel( int nIndex )
{
	::SendMessage( m_hWnd, CB_SETCURSEL, nIndex, 0 );
}

LRESULT	FLComboBox::GetCurSel()	const
{
	return ::SendMessage( m_hWnd, CB_GETCURSEL, 0, 0 );
}
