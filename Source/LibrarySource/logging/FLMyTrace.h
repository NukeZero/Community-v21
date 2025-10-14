#ifndef __FLMYTRACE_H__
#define __FLMYTRACE_H__


#include "FLLogCommon.h"

#include "../thread/FLCriticalSection.h"


class	FLMyTrace
{
public:

	static	DWORD	Key( const char* pszKey );

public:

	FLMyTrace();
	virtual	~FLMyTrace();

	void	Initialize( HWND hWnd, const TCHAR* pszFace, COLORREF cr, COLORREF crBk );

	void	Add( DWORD dwKey, BOOL bInverse, const TCHAR* pszFormat, ... );
	void	AddLine( TCHAR ch );
	void	Paint( HDC hDC );
	void	GetText( DWORD dwKey, TCHAR* pszText, size_t cchText );

private:

	void	TextOut( HDC hDC, int x, int y, const TCHAR* pszString, BOOL bInverse );
	int		FindKey( DWORD dwKey );

private:

	enum
	{
		MAX_LOG	= 26,
	};

	struct	T_MY_TRACE
	{
		DWORD	dwKey;
		BOOL	bInverse;
		TCHAR	szText[160];

	};


	T_MY_TRACE		m_tMyTrace[MAX_LOG];

	int				m_nHead;
	int				m_nTail;

	HFONT			m_hFont;
	HWND			m_hWnd;

	COLORREF		m_cr;
	COLORREF		m_crBk;

	TCHAR			m_szStatusBar[160];

	CMclCritSec		m_kAddRemoveLock;

};

#endif