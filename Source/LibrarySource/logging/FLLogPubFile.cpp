
#include "FLLogPubFile.h"
#include "../common/FLStringFunction.h"


FLLogPubFile::FLLogPubFile()
{
	m_pLogFileName = NULL;
	m_pFormater = NULL;
	m_nMaxSize = 0;
}

FLLogPubFile::~FLLogPubFile()
{
	Destroy();
}

bool	FLLogPubFile::Create( FLLogFileName* pLogFileName, FLLogFormat* pFormater, size_t nMaxSize )
{
	if( pLogFileName == NULL || pFormater == NULL )
	{
		return false;
	}

	m_pFormater = pFormater;
	m_pLogFileName = pLogFileName;
	m_nMaxSize = nMaxSize;
	return true;
}

void	FLLogPubFile::Destroy()
{
	if( m_cTextFile.Opened() == true )
	{
		m_cTextFile.Flush();
		m_cTextFile.Close();
	}

	m_pLogFileName = NULL;
	m_pFormater = NULL;
	m_nMaxSize = 0;
}

void	FLLogPubFile::DoPublish( const FLLogEvent* pEvent )
{
	if( NeedNewFile() == true )
	{
		if( CreateNewFile() == false )
		{
			return;
		}
	}

	WriteLogEvent( pEvent );
}

void	FLLogPubFile::DoDoneTick()
{
	m_cTextFile.Flush();
}

bool	FLLogPubFile::NeedNewFile()	const
{
	if( m_cTextFile.Opened() == false )
	{
		return true;
	}

	if( m_nMaxSize > 0 && m_cTextFile.Size() >= m_nMaxSize )
	{
		return true;
	}

	return false;
}

bool	FLLogPubFile::CreateNewFile()
{
	if( m_pLogFileName == NULL )
	{
		return false;
	}

	if( m_cTextFile.Opened() == true )
	{
		m_cTextFile.Close();
	}

	TCHAR szFileName[MAX_PATH] = { 0, };
	DWORD dwTryCount = 0;

	while( dwTryCount < USHRT_MAX )
	{
		if( m_pLogFileName->Generate( szFileName, _countof( szFileName ), dwTryCount ) == false )
		{
			return false;
		}

		if( m_cTextFile.Open( szFileName, GENERIC_WRITE, FILE_SHARE_READ ) == true ||
			m_cTextFile.Create( szFileName, GENERIC_WRITE, FILE_SHARE_READ ) == true )
		{
			m_cTextFile.Seek( 0, FILE_END );
			break;
		}

		++dwTryCount;
	}

	return true;
}

void	FLLogPubFile::WriteLogEvent( const FLLogEvent* pEvent )
{
	if( pEvent == NULL || m_pFormater == NULL )
	{
		return;
	}

	TCHAR szBuffer[FLLOG_MAX_LOG_STRING+1024] = { 0, };
	if( m_pFormater->Format( pEvent, szBuffer, _countof( szBuffer ) ) == true )
	{
		m_cTextFile.WriteText( szBuffer, FLStrlen( szBuffer, _countof( szBuffer ) ) );
	}
}
