
#ifndef	__FLCOMBOBOX_H__
#define	__FLCOMBOBOX_H__


#include "FLUICommon.h"


class	FLComboBox
{
public:

	FLComboBox();
	~FLComboBox();

	bool	Create( HWND hDlg, int nDlgItemID );
	void	Destroy();

	void	AddItem( const TCHAR* szString );
	LRESULT	GetCurSel()	const;
	void	SetCurSel( int nIndex );

private:

	HWND	m_hWnd;

};




#endif