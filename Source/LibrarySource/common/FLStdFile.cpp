
#include "FLStdFile.h"



FILE*	FLStdFileOpen( const TCHAR* szFileName, const TCHAR* szMode )
{
	FILE* pFile = NULL;
	if( szFileName != NULL && szMode != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		if( ::_tfopen_s( &pFile, szFileName, szMode ) != 0 )
		{
			return NULL;
		}
#else
		pFile = ::_tfopen( szFileName, szMode );
#endif
	}

	return pFile;
}
