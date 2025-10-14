// FileUploader.cpp: implementation of the CFileUploader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileUploader.h"
#include <Shlwapi.h>	// link Shlwapi.lib

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileUploader::CFileUploader()
{
	m_pDPMng		= NULL;
	m_fp			= NULL;
	m_dwFileSize	= 0;
	m_szFilename[0] = '\0';
	strcpy( m_szDownDirectory, "c:\\down" );
}

CFileUploader::~CFileUploader()
{
}

const char*	CFileUploader::GetFilename() const
{
	return m_szFilename;
}

const char*	CFileUploader::GetDownDirectory() const
{
	return m_szDownDirectory;
}

// 주의: szDirectory는 c:\\down과 같이 \\로 끝나면 안된다.
void CFileUploader::SetDownDirectory( const char* szDirectory )
{
	ASSERT( szDirectory );
	strcpy( m_szDownDirectory, szDirectory );
}

void CFileUploader::Init(CDPMng* pDPMng)
{
	m_pDPMng = pDPMng;
}

bool CFileUploader::SendFile( const char* szFilename, DPID dpidTo )
{
	FILE* fp = fopen( szFilename, "rb" );
	if( fp == NULL )
	{
		TRACE( "file:%s not found\n", szFilename );
		return false;
	}
	DWORD dwSize = _filelength( fileno( fp ) );

	BEFORESEND( ar, PACKETTYPE_CONSOLE_FILEHEADER );
	ar.WriteString( PathFindFileName( szFilename ) );		
	ar << dwSize;
	SEND( ar, m_pDPMng, dpidTo );
	
	char buffer[2048];
	WORD count;
	if( dwSize >= 2048)
		count = 2048; 
	else
		count = dwSize;

	while( dwSize > 0 )
	{
		fread(buffer, count, 1, fp);
		dwSize -= count;

		BEFORESEND( ar, PACKETTYPE_CONSOLE_FILEDATA );
		ar << count;	
		ar.Write( buffer, count );
		SEND( ar, m_pDPMng, dpidTo );

		if( dwSize <= 2048 )
			count = dwSize; 
	}	

	fclose( fp );
	return true;
}

void CFileUploader::SendUploadACK( bool bResult, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_CONSOLE_UPLOADACK );
	ar << (BYTE)bResult;
	SEND( ar, m_pDPMng, dpid );
}

bool CFileUploader::OnUploadACK( CAr & ar )
{
	BYTE cbResult;
	ar >> cbResult;

	return ( cbResult == 1 );
}

//
//
//

void CFileUploader::OnFileHeader( CAr & ar, DPID dpid )
{
	DWORD dwFileSize;

	ar.ReadString( m_szFilename );
	ar >> dwFileSize;

	char szPath[MAX_PATH];
	sprintf( szPath, "%s\\%s", m_szDownDirectory, m_szFilename); 
	FILE * fp = fopen( szPath , "wb" );
	if( fp )
	{
		m_fp = fp;
		m_dwFileSize = dwFileSize;
	}
	else
	{
		printf( "%s fopen error: %s\n", szPath, strerror(NULL) );
		SendUploadACK( false, dpid );
	}
}

bool CFileUploader::OnFileData( CAr & ar, DPID dpid )
{
	WORD nSize;
	BYTE data[2048];

	ar >> nSize;
	ar.Read( data, nSize );

	if( m_fp )
	{
		fwrite( data, nSize, 1, m_fp );
		m_dwFileSize -= nSize;
		TRACE( "OnFileData nSize:%d, m_dwFileSize:%d\n", nSize, m_dwFileSize );
		if( m_dwFileSize == 0 )
		{
			fclose( m_fp );
			m_fp = NULL;

			SendUploadACK( true, dpid );
			return true;
		}
	}
	else
	{
		TRACE("file is not ready.\n");
	}

	return false;
}
