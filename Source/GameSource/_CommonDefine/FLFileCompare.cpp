#include "stdafx.h"
#include "FLFileCompare.h"

#include "../_Network/tools.h"


FLFileCompare::FLFileCompare()
{
	Init();
	Clear();
}

FLFileCompare::~FLFileCompare()
{
	Clear();
}

FLFileCompare* FLFileCompare::GetInstance()
{
	static FLFileCompare xFileCompare;

	return	&xFileCompare;
}

void	FLFileCompare::Init()
{
	m_nCompareCount = 0;
}

void	FLFileCompare::Clear()
{
	m_listFileContainer.clear();
}

bool	FLFileCompare::CheckFile()
{
	T_FILE_INFO				tFileInfo;
	FileContainerIterator	it;
	m_nCompareCount = 0;

	for( it = m_listFileContainer.begin(); it != m_listFileContainer.end(); ++it )
	{
		tFileInfo = ( *it );

		if( CompareFile( tFileInfo ) == false )
		{
			DeleteWrongFile( tFileInfo.szFullFileName );
			return false;
		}
	}
	return true;
}

bool	FLFileCompare::CompareFile( T_FILE_INFO& tFileInfo )
{
	WIN32_FILE_ATTRIBUTE_DATA	attribute;
	BOOL bOK = ::GetFileAttributesEx( tFileInfo.szFullFileName, GetFileExInfoStandard, &attribute );
	if( bOK == TRUE )
	{
		TCHAR* pExt = _tcsrchr( tFileInfo.szFullFileName, '.' );
		if( pExt != NULL )
		{
			if( _strcmpi( pExt, ".des" ) == 0 )
			{
				tFileInfo.byCompare = FILECOMPARE_SAME;
				++m_nCompareCount;
				return true;
			}
		}

		if( tFileInfo.dwFileSize == attribute.nFileSizeLow )
		{
			__int64 nLocalFileTime = MAKEINT64( attribute.ftLastWriteTime.dwLowDateTime, attribute.ftLastWriteTime.dwHighDateTime );
			__int64 nFileTimeGap = tFileInfo.n64FileTime - nLocalFileTime;
			if( _abs64( nFileTimeGap ) <= 10000000 * 60 * 3 )	// 10000000 * 60 == 60ÃÊ
			{
				tFileInfo.byCompare = FILECOMPARE_SAME;
				++m_nCompareCount;
				return true;
			}
			else
			{
				tFileInfo.byCompare = FILECOMPARE_DIFFERENT;
				++m_nCompareCount;
				FLERROR_LOG( PROGRAM_NAME, _T( "Invalid File Time:[%s]" ), tFileInfo.szFullFileName  );
				return false;
			}
		}
		else
		{
			tFileInfo.byCompare = FILECOMPARE_DIFFERENT;
			++m_nCompareCount;
			FLERROR_LOG( PROGRAM_NAME, _T( "Invalid File Size:[%s]" ), tFileInfo.szFullFileName  );
			return false;
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid File Attribute:[%s]" ), tFileInfo.szFullFileName  );
		return false;
	}
	return false;
}

void	FLFileCompare::InsertFileData( T_FILE_INFO& tFileInfo )
{
	m_listFileContainer.push_back( tFileInfo );
}

bool	FLFileCompare::DeleteWrongFile( TCHAR *pszDeleteFileFullPath )
{
	FLERROR_ASSERT_LOG_RETURN( pszDeleteFileFullPath, false, _T("[File Compare] Delete File Pointer is Invalid" ) );

	BOOL bResult( ::DeleteFile( pszDeleteFileFullPath ) );

	if( bResult )
		FLERROR_LOG( PROGRAM_NAME, _T("[File Compare] Success Delete File ( %s ) "), pszDeleteFileFullPath );
	else
		FLERROR_ASSERT_LOG_RETURN( pszDeleteFileFullPath, false, _T("[File Compare] Delete File Pointer is Invalid ( %s )" ), pszDeleteFileFullPath );

	return true;
}