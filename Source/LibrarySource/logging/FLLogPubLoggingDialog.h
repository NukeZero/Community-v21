
#ifndef	__FLLOGPUBLOGGINGDIALOG_H__
#define	__FLLOGPUBLOGGINGDIALOG_H__


#include "FLLogCommon.h"
#include "FLLogPublisher.h"
#include "FLLogLevelFilter.h"

#include "../ui/FLDialog.h"
#include "../ui/FLListView.h"
#include "../ui/FLComboBox.h"

#include "../thread/FLCriticalSection.h"

#include <list>


class	FLLogPubLoggingDialog : public FLLogPublisher, public FLDialog
{
public:

	FLLogPubLoggingDialog();
	~FLLogPubLoggingDialog();

	bool	Create( const TCHAR* szName, HWND hParent, const TCHAR* szResDllPath );
	void	Destroy();

	int		SetShowLevel( int nLevel );

	void	MoveUnderOfParent();

private:

	void	DoPublish( const FLLogEvent* pEvent );

	void	InsertLog( const TCHAR* szDate, const TCHAR* szLevel, const TCHAR* szFunction, const TCHAR* szLine, const TCHAR* szMessage );

	bool	InitListView();
	bool	InitComboBox();

	INT_PTR	MessageHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	INT_PTR	OnInitDialog( HWND hWnd, WPARAM wParam, LPARAM lParam );
	INT_PTR	OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam );
	INT_PTR	OnLoggingMessage( HWND hWnd, WPARAM wParam, LPARAM lParam );

	void	OnComboBoxSelChange();

private:

	struct	T_LOGGING_MESSAGE_PARAM
	{
		int				nLevel;
		const TCHAR *	szFunction;
		const TCHAR *	szFile;
		int				nLine;
		SYSTEMTIME		tTime;
		tstring			strText;
	};

	typedef	std::list< T_LOGGING_MESSAGE_PARAM* >	LOGGING_LIST;
	typedef	LOGGING_LIST::iterator					LOGGING_LIST_ITER;

	FLLogLevelFilter	m_cLevelFilter;
	HMODULE				m_hResDll;

	FLListView			m_cListView;
	FLComboBox			m_cComboBox;

	CMclCritSec			m_kListLock;
	LOGGING_LIST		m_listLoggingMessage;

};







#endif