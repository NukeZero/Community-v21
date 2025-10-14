
#ifndef	__FLFILEUTIL_H__
#define	__FLFILEUTIL_H__


#include "FLCommon.h"


void	FLSplitPath( const TCHAR* szFolderAndFile, TCHAR* szFolder, size_t cchFolder );


bool	FLIsFolder( const TCHAR* szFolder );
bool	FLCreateFolder( const TCHAR* szFolder );
bool	FLDeleteFolder( const TCHAR* szFolder );

bool	FLDeleteFile( const TCHAR* szFile );
bool	FLDeleteFile( const TCHAR* szFolder, const TCHAR* szFile );





#endif