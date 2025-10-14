#ifndef __RESMANAGER_H
#define __RESMANAGER_H

#include "data.h"
#include "lang.h"

typedef struct tagWndCtrl
{
	DWORD dwWndId;
	DWORD dwWndType;
	DWORD dwWndStyle;
	//TCHAR szTitle [ 64 ];
	//TCHAR szDefine[ 64 ];

	CString strDefine;//[ 64 ];
	CString strTitle;
	CString strToolTip;	
#ifdef __DAISY
	wchar_t awszTitle[ LANG_MAX ][ 64 ];
	wchar_t awszToolTip[ LANG_MAX ][ 256 ];
#endif	
	//CString strToolTip;	
	CString strTexture;
	CRect rect;
	BOOL m_bVisible;
	BOOL m_bDisabled;
	BOOL m_bTabStop;
	BOOL m_bGroup;
} WNDCTRL,* LPWNDCTRL; 

typedef struct tagWndApplet
{
	void* pWndBase;
	DWORD dwWndId;
	DWORD dwWndStyle;
	BOOL bTile;
	
	CString strTitle;
	CString strToolTip;	
#ifdef __DAISY
	wchar_t awszTitle[ LANG_MAX ][ 64 ];
	wchar_t awszToolTip[ LANG_MAX ][ 256 ];
#endif	
	
	CString strDefine;
	CString strTexture;
	CPtrArray ptrCtrlArray;
	CSize size;

	LPWNDCTRL GetAt( DWORD dwWndId );

} WNDAPPLET,* LPWNDAPPLET;

class CResManager
{
	void ResetNewControlId();
	BOOL IsSameDefine( LPWNDAPPLET lpExceptionWndApplet, LPCTSTR lpszDefine );
	DWORD m_dwNewCtrlId;
	BOOL m_bModifiedHeader;
	BOOL m_bModifiedScript;

	LPCTSTR AddDefine( LPCTSTR lpDefine, LPCTSTR lpDefData );
	int     AddDefine( LPCTSTR lpDefine, int nDefData );
	void    RemoveDefine( LPCTSTR lpDefine );
	CString GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip );
	CString GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip );
		
public:
	CMapPtrToPtr m_mapWndApplet;
	CDWordStack m_wndAppletIdStack;
	CDWordStack m_wndCtrlIdStack;
	int m_nLanguage;
//	DWORD m_dwAppletId;

	BOOL IsModifiedHeader() { return m_bModifiedHeader; }
	BOOL IsModifiedScript() { return m_bModifiedScript; }

	void SetModifiedHeader( BOOL bModified = TRUE ) { m_bModifiedHeader = bModified; }
	void SetModifiedScript( BOOL bModified = TRUE ) { m_bModifiedScript = bModified; }
	
	CResManager();
	~CResManager();
	void Free();
	BOOL Load( LPCTSTR lpszName );
#ifdef __DAISY
	BOOL Save( LPCTSTR lpszFileName );
	BOOL SaveHeader( LPCTSTR lpszFileName );
#endif
	CString GetNewAppletDefine();
	CString GetNewControlDefine( DWORD dwAppletId, LPCTSTR lpszBaseDefine );
	DWORD GetNewAppletId();
	DWORD GetNewControlId( DWORD dwAppletId );
	//LPWNDAPPLET GetAt( LPCTSTR lpszDefine );
	LPWNDAPPLET GetAt( DWORD dwAppletId );
	LPWNDCTRL GetAtControl( DWORD dwAppletId, DWORD dwCtrlId );
	BOOL RemoveApplet( DWORD dwAppletId );
	BOOL RemoveControl( DWORD dwAppletId, LPCTSTR lpszDefine );
	//BOOL RemoveApplet( LPCTSTR lpszDefine );
	BOOL AddApplet( LPWNDAPPLET pWndApplet );
	BOOL AddControl( DWORD dwAppletId, LPWNDCTRL lpWndCtrl );

	BOOL ChangeControlDefine( DWORD dwAppletId, LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC );
	BOOL ChangeAppletDefine( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC, DWORD* lpdwNewWndId = NULL );
};
//#ifdef __NEWINTERFACE
//#define RESDATA_HEADER _T( "ResData.h" )
//#define RESDATA_INC    _T( "ResDataNew.inc" )
//#else
#define RESDATA_HEADER _T( "ResData.h" )
#define RESDATA_INC    _T( "ResData.inc" )
//#endif
#endif