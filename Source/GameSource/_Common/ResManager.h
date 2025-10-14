#ifndef __RESMANAGER_H
#define __RESMANAGER_H

#include "data.h"
#include "lang.h"

#define DEF_CTRL_GROUP 		"WndEditTile200.tga"
#define DEF_CTRL_TEXT  		"WndEditTile00.tga"


//---------------------------------------------------------------------------------------------
// 컨트롤 윈도우 정보 구조체 ( 기본 )
//---------------------------------------------------------------------------------------------
typedef struct tagWndCtrl
{
	DWORD			dwWndId;
	DWORD			dwWndType;
	DWORD			dwWndStyle;

	BOOL			bTile;
	BOOL			m_bVisible;
	BOOL			m_bDisabled;
	BOOL			m_bTabStop;
	BOOL			m_bGroup;

	CString			strDefine;
	CString			strTitle;
	CString			strToolTip;	
	CString			strTexture;

	int				m_nFontColorR;
	int				m_nFontColorG;
	int				m_nFontColorB;

	CRect			rect;

	tagWndCtrl()
	:	dwWndId( NULL )
	,	dwWndType( WTYPE_BASE )
	,	dwWndStyle( 0x00000000L )
	,	bTile( FALSE )
	,	m_bVisible( FALSE )
	,	m_bDisabled( FALSE )
	,	m_bTabStop( FALSE )
	,	m_bGroup( FALSE )
	,	strDefine( _T("") )
	,	strTitle( _T("") )
	,	strToolTip( _T("") )
	,	strTexture( _T("") )
	,	m_nFontColorR( NULL )
	,	m_nFontColorG( NULL )
	,	m_nFontColorB( NULL )
	{
	}
} WNDCTRL,* LPWNDCTRL; 


//---------------------------------------------------------------------------------------------
// 애플릿 윈도우 정보 구조체
//---------------------------------------------------------------------------------------------
typedef struct tagWndApplet
{
	CWndBase*		pWndBase;

	DWORD			dwWndId;
	DWORD			dwWndStyle;

	CString			strTitle;
	CString			strDefine;
	CString			strToolTip;	
	CString			strTexture;
	CString			strDefaultImg;

	BOOL			bTile;
	BOOL			m_bNoCloseButton;
	
	CSize			size;
	CPtrArray		ptrCtrlArray;

	D3DFORMAT		d3dFormat;

	LPWNDCTRL		GetAt( DWORD dwWndId );

	tagWndApplet()
	:	pWndBase( NULL )
	,	dwWndId( NULL )
	,	dwWndStyle( 0x00000000L )
	,	strTitle( _T("") )
	,	strDefine( _T("") )
	,	strToolTip( _T("") )
	,	strTexture( _T("") )
	,	strDefaultImg( _T("") )
	,	bTile( FALSE )
	,	m_bNoCloseButton( FALSE )
	,	size( CSize( 0, 0 ) )
	{
	}
} WNDAPPLET,* LPWNDAPPLET;


//---------------------------------------------------------------------------------------------
// 리소스 메니져 클레스
//---------------------------------------------------------------------------------------------
class CResManager
{

public:

					CResManager();
					~CResManager();
		
	void			Free( void );
	BOOL			Load( LPCTSTR lpszName );
	LPWNDAPPLET		GetAt( DWORD dwAppletId );
	LPWNDCTRL		GetAtControl( DWORD dwAppletId, DWORD dwCtrlId );


private:


	CString			GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip );
	CString			GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip );


private:

	CMapPtrToPtr	m_mapWndApplet;

};


#endif