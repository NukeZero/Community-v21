
#include "FLLogPubLoggingDialog.h"

#include "../common/FLStringFunction.h"
#include "../resource/FLCommonResourceIDs.h"
#include "../resource/FLCommonResourceDefine.h"
#include "../thread/FLAutoLock.h"


namespace
{
	const int LOG_COLUMN_COUNT = 5;
	const TCHAR	LOG_LEVEL_STRING[][10] =
	{
		_T( "ALL" ),
		_T( "DEBUG" ),
		_T( "TRACE" ),
		_T( "INFO" ),
		_T( "WARNING" ),
		_T( "ERROR" ),
		_T( "FAULT" ),
		_T( "NONE" ),
	};

	#define	FLLOG_DIALOG_LOGGING_MESSAGE	WM_USER+1
}


FLLogPubLoggingDialog::FLLogPubLoggingDialog()
{
	m_hResDll = NULL;
}

FLLogPubLoggingDialog::~FLLogPubLoggingDialog()
{
	Destroy();
}

bool	FLLogPubLoggingDialog::Create( const TCHAR* szName, HWND hParent, const TCHAR* szResDllPath )
{
	if( szResDllPath == NULL )
	{
		return false;
	}

	m_hResDll = ::LoadLibrary( szResDllPath );
	if( m_hResDll == NULL )
	{
		return false;
	}

	if( FLDialog::Create( FLRESOURCE_LOGGING_DIALOG_ID, hParent, m_hResDll ) == false )
	{
		return false;
	}

	TCHAR szTitle[128] = { 0, };
	if( szName != NULL )
	{
		FLSPrintf( szTitle, _countof( szTitle ), _T( "%s Logging Dialog" ), szName );
	}
	else
	{
		FLStrcpy( szTitle, _countof( szTitle ), _T( "Logging Dialog" ) );
	}

	SetTitle( szTitle );

	m_cLevelFilter.SetLevel( FLLOG_INFO );
	SetShowLevel( FLLOG_INFO );
	return true;
}

void	FLLogPubLoggingDialog::Destroy()
{
	FLDialog::Destroy();

	if( m_hResDll != NULL )
	{
		::FreeLibrary( m_hResDll );
		m_hResDll = NULL;
	}

	{
		CMclAutoLock kLock( m_kListLock );

		LOGGING_LIST_ITER pos = m_listLoggingMessage.begin();
		LOGGING_LIST_ITER end = m_listLoggingMessage.end();

		for( ; pos != end; ++pos )
		{
			T_LOGGING_MESSAGE_PARAM* pMessage = *pos;
			if( pMessage != NULL )
			{
				delete pMessage;
				pMessage = NULL;
			}
		}

		m_listLoggingMessage.clear();
	}
}

int		FLLogPubLoggingDialog::SetShowLevel( int nLevel )
{
	m_cComboBox.SetCurSel( nLevel );
	return m_cLevelFilter.SetLevel( nLevel );
}

void	FLLogPubLoggingDialog::DoPublish( const FLLogEvent* pEvent )
{
	if( pEvent != NULL && m_hWnd != NULL )
	{
		if( m_cLevelFilter.IsPublish( pEvent ) == true )
		{
			T_LOGGING_MESSAGE_PARAM* pParam = new T_LOGGING_MESSAGE_PARAM;
			if( pParam != NULL )
			{
				pParam->nLevel = pEvent->nLevel;
				pParam->szFunction = pEvent->szFunction;
				pParam->szFile = pEvent->szFile;
				pParam->nLine = pEvent->nLine;
				pParam->tTime = pEvent->tTime;
				pParam->strText = pEvent->szText;

				{
					CMclAutoLock kLock( m_kListLock );
					m_listLoggingMessage.push_back( pParam );
				}

				::PostMessage( m_hWnd, FLLOG_DIALOG_LOGGING_MESSAGE, 0, 0 );
			}
		}
	}
}

INT_PTR	FLLogPubLoggingDialog::MessageHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:					OnInitDialog( hWnd, wParam, lParam );		break;
	case WM_COMMAND:					OnCommand( hWnd, wParam, lParam );			break;
	case FLLOG_DIALOG_LOGGING_MESSAGE:	OnLoggingMessage( hWnd, wParam, lParam );	break;
	}

	return FALSE;
}

INT_PTR	FLLogPubLoggingDialog::OnInitDialog( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	(hWnd);
	(wParam);
	(lParam);

	MoveUnderOfParent();
	if( InitListView() == false )
	{
		return FALSE;
	}

	if( InitComboBox() == false )
	{
		return FALSE;
	}

	return TRUE;
}

void	FLLogPubLoggingDialog::MoveUnderOfParent()
{
	if( m_hWnd != NULL && m_hParent != NULL )
	{
		RECT tMyRect, tParentRealRect;
		::GetClientRect( m_hWnd, &tMyRect );
		::GetWindowRect( m_hParent, &tParentRealRect );

		int nCaption = ::GetSystemMetrics( SM_CYCAPTION );
		int nWidth = tMyRect.right - tMyRect.left;
		int nHeight = tMyRect.bottom - tMyRect.top;

		int nX = tParentRealRect.left;
		if( nX < 0 )
		{
			nX = 0;
		}
		int nY = tParentRealRect.bottom;
		if( nY < 0 )
		{
			nY = 0;
		}

		::MoveWindow( m_hWnd, nX, nY, nWidth, nHeight + nCaption, TRUE );
	}
}

bool	FLLogPubLoggingDialog::InitListView()
{
	// 리스트 초기화
	if( m_cListView.Create( m_hWnd, IDC_FLRESOURCE_LOGGING_LIST ) == false )
	{
		return false;
	}

	m_cListView.SetStyle( LVS_EX_FULLROWSELECT );

	RECT tViewSize;
	m_cListView.GetRect( &tViewSize );

	static const TCHAR* cColNames[LOG_COLUMN_COUNT] =
	{
		_T( "Date" ),
		_T( "Level" ),
		_T( "Function" ),
		_T( "Line" ),
		_T( "Text" )
	};
	static const int nColWitdh[LOG_COLUMN_COUNT] =
	{
		125,
		75,
		200,
		50,
		480
		//static_cast< int >( tViewSize.right*0.15 ),
		//static_cast< int >( tViewSize.right*0.10 ),
		//static_cast< int >( tViewSize.right*0.20 ),
		//static_cast< int >( tViewSize.right*0.05 ),
		//static_cast< int >( tViewSize.right*0.50 )
	};

	m_cListView.SetColumn( LOG_COLUMN_COUNT, cColNames, nColWitdh );
	return true;
}

bool	FLLogPubLoggingDialog::InitComboBox()
{
	if( m_cComboBox.Create( m_hWnd, IDC_FLRESOURCE_LOGGING_COMBO ) == false )
	{
		return false;
	}

	for( int i = 0; i < _countof( LOG_LEVEL_STRING ); ++i )
	{
		m_cComboBox.AddItem( LOG_LEVEL_STRING[i] );
	}

	return true;
}

INT_PTR	FLLogPubLoggingDialog::OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	(hWnd);
	(lParam);

	switch( LOWORD( wParam ) )
	{
	case IDOK:		::EndDialog( m_hWnd, IDOK );		return TRUE;
	case IDCANCEL:	::EndDialog( m_hWnd, IDCANCEL );	return TRUE;

	default:
		{
			if( LOWORD( wParam ) == IDC_FLRESOURCE_LOGGING_COMBO )
			{
				if( HIWORD( wParam ) == CBN_SELCHANGE )
				{
					OnComboBoxSelChange();
				}
			}
		}
		break;
	}

	return TRUE;
}

void	FLLogPubLoggingDialog::OnComboBoxSelChange()
{
	int nSelect = static_cast< int >( m_cComboBox.GetCurSel() );
	m_cLevelFilter.SetLevel( nSelect );
}

INT_PTR	FLLogPubLoggingDialog::OnLoggingMessage( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	(hWnd);
	(wParam);
	(lParam);

	LOGGING_LIST listSwap;
	{
		CMclAutoLock kLock( m_kListLock );
		listSwap.swap( m_listLoggingMessage );
	}

	LOGGING_LIST_ITER pos = listSwap.begin();
	LOGGING_LIST_ITER end = listSwap.end();

	for( ; pos != end; ++pos )
	{
		T_LOGGING_MESSAGE_PARAM* pMessage = *pos;
		if( pMessage != NULL )
		{
			TCHAR szDate[128] = { 0, };
			TCHAR szLine[32] = { 0, };

			FLSPrintf( szDate, _countof( szDate ),
				_T( "%04d-%02d-%02d %02d:%02d:%02d" ),
				pMessage->tTime.wYear, pMessage->tTime.wMonth, pMessage->tTime.wDay,
				pMessage->tTime.wHour, pMessage->tTime.wMinute, pMessage->tTime.wSecond );

			FLSPrintf( szLine, _countof( szLine ), _T( "%d" ), pMessage->nLine );

			InsertLog( szDate, GetLogLevelString( pMessage->nLevel ), pMessage->szFunction, szLine, pMessage->strText.c_str() );

			delete pMessage;
			pMessage = NULL;
		}
	}

	return TRUE;
}

void	FLLogPubLoggingDialog::InsertLog( const TCHAR* szDate, const TCHAR* szLevel, const TCHAR* szFunction, const TCHAR* szLine, const TCHAR* szMessage )
{
	const TCHAR* szItems[LOG_COLUMN_COUNT] = { NULL, };
	szItems[0] = szDate;
	szItems[1] = szLevel;
	szItems[2] = szFunction;
	szItems[3] = szLine;
	szItems[4] = szMessage;

	m_cListView.InsertItem( LOG_COLUMN_COUNT, szItems );
	m_cListView.EnsureVisible( m_cListView.GetCount()-1 );
}
