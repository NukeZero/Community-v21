#ifndef __HTTPDOWNLOAD_H__
#define	__HTTPDOWNLOAD_H__


#define	WM_HTTPDOWNLOAD_THREAD_FINISHED	( WM_USER + 2003 )
#define	WM_HTTPDOWNLOAD_FAIL_TO			( WM_USER + 2004 )

//struct	FILE_INFO;
//class	CPatchManager;

#include "PatchManager.h"

class CHttpDownload
{
public:
//	Constructions
	CHttpDownload( CWnd* pParent );
	~CHttpDownload();

protected:
	HINTERNET		m_hInternetSession;
	HINTERNET		m_hHttpConnection;
	HINTERNET		m_hHttpFile;
	CString			m_sServer;
	CWinThread*		m_pThread;
	CWnd*			m_pParent;
	int				m_nRecvdFile;
	CPatchManager*	m_pPatchManager;

public:
//	Operations
	void	BeginDownload( CPatchManager* pPatchManager );
	void	WaitForSafeToClose();
	void	SetServer( const TCHAR* szServer ) { m_sServer = szServer; }
	void	ThreadFunc();

protected:
	void	SetStatusText( const TCHAR* szText );
	void	SetTextFileNum( int nRecv, int nTotal );
	BOOL	HttpConnect();
	void	HttpClose();
	UINT	DownloadFile( T_FILE_INFO& tFileInfo );
	BOOL	DownloadThread( FileContainerIterator& it );
	void	OnError( UINT nResult );
	static	UINT	_ThreadFunc( LPVOID pParam );	
};

#endif	// __HTTPDOWNLOAD_H__