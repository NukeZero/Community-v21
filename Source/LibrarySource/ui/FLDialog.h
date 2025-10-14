
#ifndef	__FLDIALOG_H__
#define	__FLDIALOG_H__


#include "FLUICommon.h"


class	FLDialog
{
public:

	static INT_PTR CALLBACK	DefaultDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:

	FLDialog();
	virtual ~FLDialog();

	INT_PTR	Modal( const TCHAR* szResourceID, HWND hParent, HINSTANCE hInstance = NULL );

	bool	Create( const TCHAR* szResourceID, HWND hParent, HINSTANCE hInstance = NULL );
	void	Destroy();

	void	SetTitle( const TCHAR* szTitle );

	void	Show();
	void	Hide();

	void	MoveCenterOfParent();

private:

	virtual INT_PTR	MessageHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:

	HWND	m_hParent;
	HWND	m_hWnd;

};








#endif