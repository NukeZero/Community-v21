// FileUploader.h: interface for the CFileUploader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEUPLOADER_H__BAAD6C38_AC55_4452_88C0_0F5582B8AE4E__INCLUDED_)
#define AFX_FILEUPLOADER_H__BAAD6C38_AC55_4452_88C0_0F5582B8AE4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dpmng.h"
#include "msghdr.h"

class CFileUploader  
{
public:
	CFileUploader();
	virtual ~CFileUploader();

public:
	void	Init(CDPMng* pDPMng);
	bool	SendFile( const char* szFilename, DPID dpidTo = DPID_SERVERPLAYER );
	bool	OnUploadACK( CAr & ar );
	void	OnFileHeader( CAr & ar, DPID dpid );
	bool	OnFileData( CAr & ar, DPID dpid );
	void	SendUploadACK( bool bResult, DPID dpid );
	const char*		GetDownDirectory() const;
	void			SetDownDirectory( const char* szDirectory ); 
	const char*		GetFilename() const;

private:
	CDPMng*	m_pDPMng;
	FILE*	m_fp;
	DWORD	m_dwFileSize;
	char    m_szDownDirectory[MAX_PATH];
	char    m_szFilename[MAX_PATH];
};

#endif // !defined(AFX_FILEUPLOADER_H__BAAD6C38_AC55_4452_88C0_0F5582B8AE4E__INCLUDED_)
