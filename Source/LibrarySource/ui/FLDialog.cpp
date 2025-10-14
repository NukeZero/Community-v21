
#include "FLDialog.h"

//////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK	FLDialog::DefaultDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_INITDIALOG && ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) == 0 )
	{
		::SetWindowLongPtr( hWnd, GWLP_USERDATA, lParam );
		FLDialog* pDlg = reinterpret_cast< FLDialog* >( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
		if( pDlg != NULL )
		{
			pDlg->m_hWnd = hWnd;
		}
	}

	FLDialog* pDlg = reinterpret_cast< FLDialog* >( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	if( pDlg != NULL )
	{
		return pDlg->MessageHandler( hWnd, uMsg, wParam, lParam );
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////////


FLDialog::FLDialog()
{
	m_hParent = NULL;
	m_hWnd = NULL;
}

FLDialog::~FLDialog()
{
	m_hParent = NULL;
	m_hWnd = NULL;
}

INT_PTR	FLDialog::Modal( const TCHAR* szResourceID, HWND hParent, HINSTANCE hInstance )
{
	m_hParent = hParent;

	INT_PTR nResult = DialogBoxParam( hInstance, szResourceID, m_hParent, DefaultDialogProc, reinterpret_cast< LPARAM >( this ) );

	m_hWnd = NULL;
	m_hParent = NULL;

	return nResult;
}

bool	FLDialog::Create( const TCHAR* szResourceID, HWND hParent, HINSTANCE hInstance )
{
	if( m_hWnd == NULL )
	{
		m_hParent = hParent;
		if( CreateDialogParam( hInstance, szResourceID, m_hParent, DefaultDialogProc, reinterpret_cast< LPARAM >( this ) ) != NULL )
		{
			return true;
		}
	}

	return false;
}

void	FLDialog::Destroy()
{
	if( m_hWnd != NULL )
	{
		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
		m_hParent = NULL;
	}
}

void	FLDialog::SetTitle( const TCHAR* szTitle )
{
	if( m_hWnd != NULL )
	{
		::SetWindowText( m_hWnd, szTitle );
	}
}

void	FLDialog::Show()
{
	if( m_hWnd != NULL )
	{
		::ShowWindow( m_hWnd, SW_SHOW );
	}
}

void	FLDialog::Hide()
{
	if( m_hWnd != NULL )
	{
		::ShowWindow( m_hWnd, SW_HIDE );
	}
}

void	FLDialog::MoveCenterOfParent()
{
	if( m_hWnd != NULL && m_hParent != NULL )
	{
		RECT tMyRect, tParentRect, tParentRealRect;
		::GetClientRect( m_hWnd, &tMyRect );
		::GetClientRect( m_hParent, &tParentRect );
		::GetWindowRect( m_hParent, &tParentRealRect );

		int nCaption = ::GetSystemMetrics( SM_CYCAPTION );
		int nWidth = tMyRect.right - tMyRect.left;
		int nHeight = tMyRect.bottom - tMyRect.top;

		int nX = tParentRealRect.left + ((tParentRect.right - tParentRect.left)/2) - (nWidth / 2);
		if( nX < 0 )
		{
			nX = 0;
		}
		int nY = tParentRealRect.top + ((tParentRect.bottom - tParentRect.top)/2) - (nHeight / 2);
		if( nY < 0 )
		{
			nY = 0;
		}

		::MoveWindow( m_hWnd, nX, nY, nWidth, nHeight + nCaption, TRUE );
	}
}

INT_PTR	FLDialog::MessageHandler( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	return FALSE;
}
