
#if _MSC_VER > 1000
#pragma once
#endif 

#include "StdAfx.h"
#include "data.h"
#include "lang.h"

#include "../_Interface/WndControlInfo.h"

class CWndDialog;

#define RESDATA_HEADER		_T( "ResData.h" )
#define RESDATA_INC			_T( "ResData.inc" )

//---------------------------------------------------------------------------------------------
// Applet Information Struct
//---------------------------------------------------------------------------------------------
typedef struct tagWndApplet
{
	CWndBase*	pWndBase;

	DWORD		dwWndId;
	DWORD		dwWndStyle;

	CString		strTitle;
	CString		strDefine;
	CString		strToolTip;	
	CString		strTexture;
	CString		strDefaultImg;

	BOOL		m_bTile;
	
	int			nDefSizeX;
	int			nDefSizeY;

	CSize		size;
	CPtrArray	ptrCtrlArray;

#ifdef __USE_STR_TBL0407
	char		m_szTitleKey[100];
	char		m_szToolTipKey[100];
#else	
	#ifdef __DAISY
		wchar_t awszTitle[ LANG_MAX ][ 64 ];
		wchar_t awszToolTip[ LANG_MAX ][ 256 ];
	#endif	
#endif	

#ifdef __V050322_CACHEQUEST
	D3DFORMAT	d3dFormat;
	BOOL		m_bNoCloseButton;
#endif			


	LPWNDCTRL GetAt( DWORD dwWndId );

	tagWndApplet()
	:	pWndBase( NULL )
	,	dwWndId( NULL )
	,	dwWndStyle( 0x00000000L )
	,	strTitle( _T("") )
	,	strDefine( _T("") )
	,	strToolTip( _T("") )
	,	strTexture( _T("") )
	,	strDefaultImg( _T("") )
	,	m_bTile( FALSE )
	,	nDefSizeX( NULL )
	,	nDefSizeY( NULL )
	,	size( CSize( 0, 0) )
	,	m_bNoCloseButton( FALSE )
	{
#ifdef __USE_STR_TBL0407
		memset( m_szTitleKey,	NULL, sizeof(char) * 100 );
		memset( m_szToolTipKey, NULL, sizeof(char) * 100 );
#else	
	#ifdef __DAISY
		memset( awszTitle,		NULL, sizeof(wchar_t) * (LANG_MAX * 64) );
		memset( awszToolTip,	NULL, sizeof(wchar_t) * (LANG_MAX * 256) );
	#endif	
#endif	
	}

} WNDAPPLET,* LPWNDAPPLET;


//---------------------------------------------------------------------------------------------
// Undo Information Struct
//---------------------------------------------------------------------------------------------
typedef struct tUndoData
{
	int nType;
	WNDCTRL WndCTRL;

} UNDODATA,* PUNDODATA;


//---------------------------------------------------------------------------------------------
// Typedef
//---------------------------------------------------------------------------------------------
typedef std::map< DWORD, LPWNDAPPLET >	WndAppletContainer;
typedef WndAppletContainer::iterator	WndAppletIter;

typedef std::vector<LPWNDCTRL>				vecSaveHistory;




//---------------------------------------------------------------------------------------------
// 리소스 매니져
//---------------------------------------------------------------------------------------------
class CResManager
{


public:

	
						CResManager();
						~CResManager();

	BOOL				Load( LPCTSTR lpszName );
	void				Free( void );

	BOOL				AddApplet( LPWNDAPPLET pWndApplet );
	BOOL				AddControl( DWORD dwAppletId, LPWNDCTRL lpWndCtrl, BOOL bIsMode = FALSE );
	BOOL	 			RemoveApplet( DWORD dwAppletId );
	BOOL	 			RemoveControl( DWORD dwAppletId, LPCTSTR lpszDefine,BOOL bIsMode = FALSE);
	BOOL	 			ChangeAppletDefine( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC, DWORD* lpdwNewWndId = NULL );
	BOOL	 			ChangeControlDefine( DWORD dwAppletId, LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC );

	LPCTSTR				AddDefine( LPCTSTR lpDefine, LPCTSTR lpDefData );
	int					AddDefine( LPCTSTR lpDefine, int nDefData );
	void				RemoveDefine( LPCTSTR lpDefine );


	void	 			UndoHistory( LPWNDAPPLET lpWndApplet,CWndBase* pWndBase, CWndDialog* pWndDialog, HWND hWnd );
	void	 			RedoHistory( LPWNDAPPLET lpWndApplet,CWndBase* pWndBase, CWndDialog* pWndDialog, HWND hWnd );
	void	 			SaveHistory( LPWNDCTRL pWndBase );
	void	 			DeleteHistory( void );

	void				SetModifiedHeader( BOOL bModified = TRUE )		{ m_bModifiedHeader = bModified;	}
	void				SetModifiedScript( BOOL bModified = TRUE )		{ m_bModifiedScript = bModified;	}
	void				SetSelectLang( int nLang )						{ m_nSelectLanguage = nLang;		}

	CString				GetNewAppletDefine( void );
	CString				GetNewControlDefine( DWORD dwAppletId, LPCTSTR lpszBaseDefine );
	DWORD				GetNewAppletId( void );
	DWORD				GetNewControlId( DWORD dwAppletId );
	LPWNDAPPLET			GetAt( DWORD dwAppletId );
	LPWNDCTRL			GetAtControl( DWORD dwAppletId, DWORD dwCtrlId );
	int					GetSelectLang()									{ return m_nSelectLanguage;			}

	void				ResetNewControlId( void );

	BOOL				IsModifiedHeader( void )						{ return m_bModifiedHeader;			}
	BOOL				IsModifiedScript( void )						{ return m_bModifiedScript;			}

	BOOL				IsSameDefine( LPWNDAPPLET lpExceptionWndApplet, LPCTSTR lpszDefine );

#ifdef __USE_STR_TBL0407

	void	 			GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip );
	void	 			GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip );
	BOOL				LoadString( void );
	BOOL				WriteString( void );

#else	

	CString  			GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip );
	CString  			GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip );

#endif	

#ifdef __DAISY
	BOOL				Save( LPCTSTR lpszFileName );
	BOOL				SaveHeader( LPCTSTR lpszFileName );
#endif


public:


	DWORD				m_dwNewCtrlId;
	int					m_nLanguage;
	int					m_nChangeItem;	
	BOOL				m_bModifiedHeader;
	BOOL				m_bModifiedScript;

	WNDCTRL				m_OldWndCtrl;
	LPWNDCTRL			m_phistorylog;
	int					m_nHistoryCount;

	vecSaveHistory		m_vecUndoData;
	vecSaveHistory		m_vecRedoData;

	CDWordStack			m_wndAppletIdStack;
	CDWordStack			m_wndCtrlIdStack;

#ifdef __USE_STR_TBL0407
	CString				m_strStrFile;
	int					m_nString;
#endif

	WndAppletContainer	m_mapWndApplet;

	enum CommandType 
	{	
		CTRL_CREATE = 1, 
		CTRL_REMOVE, 
		CTRL_CHANGE, 
		APP_CREATE, 
		APP_REMOVE, 
		APP_CHANGE 
	};


private:


	int					m_nSelectLanguage;

};

