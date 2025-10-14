
#include "FLFileUtil.h"
#include "FLStringFunction.h"
#include "FLFindFile.h"

#include <cstdlib>

namespace
{
	bool	FLCreateDirectory( const TCHAR* szDirectory )
	{
		if( szDirectory != NULL )
		{
			if( ::CreateDirectory( szDirectory, NULL ) == FALSE )
			{
				if( ::GetLastError() != ERROR_ALREADY_EXISTS )
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}
}


void	FLSplitPath( const TCHAR* szFolderAndFile, TCHAR* szFolder, size_t cchFolder )
{
	if( szFolderAndFile != NULL && szFolder != NULL && cchFolder > 0 )
	{
		TCHAR szDrive[_MAX_DRIVE] = { 0, };
		TCHAR szDir[_MAX_DIR] = { 0, };

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		::_tsplitpath_s( szFolderAndFile, szDrive, _countof( szDrive ), szDir, _countof( szDir ), NULL, 0, NULL, 0 );
#else
		::_tsplitpath( szFolderAndFile, szDrive, szDir, NULL, NULL );
#endif

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		::_tmakepath_s( szFolder, cchFolder, szDrive, szDir, NULL, NULL );
#else
		::_tmakepath( szFolder, szDrive, szDir, NULL, NULL );
#endif
	}
}

bool	FLIsFolder( const TCHAR* szFolder )
{
	if( szFolder != NULL )
	{
		DWORD dwAttr = ::GetFileAttributes( szFolder );
		if( dwAttr != INVALID_FILE_ATTRIBUTES )
		{
			if( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
			{
				return true;
			}
		}
	}

	return false;
}

bool	FLCreateFolder( const TCHAR* szFolder )
{
	if( szFolder == NULL )
	{
		return false;
	}

	size_t nSize = ::_tcslen( szFolder );
	if( nSize > 0 )
	{
		if( FLIsFolder( szFolder ) == true )
		{
			return true;
		}

		// 부모 폴더를 생성한다.
		TCHAR szPathBuffer[MAX_PATH] = { 0, };
		for( size_t i = 0 ; i < nSize ; ++i )
		{
			szPathBuffer[i] = szFolder[i];
			if( szPathBuffer[i] == _T( '\\' ) || szPathBuffer[i] == _T( '/' ) )
			{
				szPathBuffer[i + 1] = _T( '\0' );
				if( FLIsFolder( szPathBuffer ) == false )
				{
					if( FLCreateDirectory( szPathBuffer ) == false )
					{
						return false;
					}
				}
			}
		}

		// 최종 폴더를 생성한다.
		// 마지막이 '\\' '/' 로 끝났다면 위에서 생성되었다.
		if( FLIsFolder( szFolder ) == false )
		{
			if( FLCreateDirectory( szFolder ) == false )
			{
				return false;
			}
		}
	}

	return true;
}

bool	FLDeleteFolder( const TCHAR* szFolder )
{
	if( szFolder == NULL )
	{
		return false;
	}

	FLFindFile cFindFile;
	bool bSearch = cFindFile.FindFirst( szFolder, _T( "*.*" ) );
	while( bSearch == true )
	{
		FLFindFileInfo cFindInfo = cFindFile.GetFindFileInfo();
		bSearch = cFindFile.FindNext();

		//////////////////////////////////////////////////////////////////////////
		if( cFindInfo.IsDots() == false )
		{
			if( cFindInfo.IsFolder() == true )
			{
				if( FLDeleteFolder( cFindInfo.GetName() ) == false )
				{
					return false;
				}
			}
			else
			{
				if( cFindInfo.IsReadOnly() == true )
				{
					::SetFileAttributes( cFindInfo.GetName(), FILE_ATTRIBUTE_NORMAL );
				}

				if( ::DeleteFile( cFindInfo.GetName() ) == FALSE )
				{
					return false;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	cFindFile.Close();

	if( ::RemoveDirectory( szFolder ) == FALSE )
	{
		return false;
	}

	return true;
}

bool	FLDeleteFile( const TCHAR* szFile )
{
	if( szFile == NULL )
	{
		return false;
	}

	FLFindFile cFindFile;
	bool bSearch = cFindFile.FindFirst( szFile );
	while( bSearch == true )
	{
		FLFindFileInfo cFindInfo = cFindFile.GetFindFileInfo();
		bSearch = cFindFile.FindNext();

		//////////////////////////////////////////////////////////////////////////
		if( cFindInfo.IsDots() == false )
		{
			if( cFindInfo.IsFolder() == false )
			{
				if( cFindInfo.IsReadOnly() == true )
				{
					::SetFileAttributes( cFindInfo.GetName(), FILE_ATTRIBUTE_NORMAL );
				}

				if( ::DeleteFile( cFindInfo.GetName() ) == FALSE )
				{
					return false;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	cFindFile.Close();
	return true;
}

bool	FLDeleteFile( const TCHAR* szFolder, const TCHAR* szFile )
{
	if( szFolder == NULL || szFile == NULL )
	{
		return false;
	}

	// 현재 폴더의 szFile 삭제
	TCHAR szFullFilter[MAX_PATH] = _T( "" );
	DWORD dwPos = ::GetFullPathName( szFolder, _countof( szFullFilter ), szFullFilter, NULL );
	if( 0 < dwPos && dwPos < _countof( szFullFilter ) )
	{
		if( szFullFilter[dwPos-1] != _T( '\\' ) && szFullFilter[dwPos-1] != _T( '/' ) )
		{
			FLStrcat( szFullFilter, _countof( szFullFilter ), _T( "\\" ) );
		}
	}

	FLStrcat( szFullFilter, _countof( szFullFilter ), szFile );
	FLDeleteFile( szFullFilter );

	// 하위 폴더 검색
	FLFindFile cFindFile;
	bool bSearch = cFindFile.FindFirst( szFolder, _T( "*.*" ) );
	while( bSearch == true )
	{
		FLFindFileInfo cFindInfo = cFindFile.GetFindFileInfo();
		bSearch = cFindFile.FindNext();

		//////////////////////////////////////////////////////////////////////////
		if( cFindInfo.IsDots() == false )
		{
			if( cFindInfo.IsFolder() == true )
			{
				FLDeleteFile( cFindInfo.GetName(), szFile );
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	cFindFile.Close();
	return true;
}
