#include "StdAfx.h"
#include "FLFileKeyGenerator.h"


FLFileKeyGenerator::FLFileKeyGenerator()
{
	byGeneration	= FL_KEY_GENERATION_NONE;
	un64Key			= 0;
}

FLFileKeyGenerator::~FLFileKeyGenerator()
{
}

BYTE	FLFileKeyGenerator::FileKeyGeneration( const TCHAR * pFileName )
{
	bool		bFileOpen =false;

	FLFile	file;
	bFileOpen	= file.Open( pFileName, GENERIC_READ );

	if( bFileOpen == true )
	{
		unsigned __int64	un64FileSize	= file.Size();
		if( un64FileSize <= FL_KEY_GENERATION_MAX_FILE_SIZE )
		{
			un64Key = 0;

			char	szReadBuffer[ FL_KEY_GENERATION_READ_SIZE ] = { 0 };
			while( file.Read( szReadBuffer, sizeof(szReadBuffer) - 1 ) != 0 )
			{
				un64Key	= un64Key + *(unsigned __int64*)( szReadBuffer );

				memset( szReadBuffer, 0, sizeof(szReadBuffer) );
			}

			byGeneration = FL_KEY_GENERATION_SUCCESS;
		}
		else
		{
			byGeneration	= FL_KEY_GENERATION_FILE_TO_BIG;
		}

		file.Close();
	}
	else
	{
		byGeneration	= FL_KEY_GENERATION_FILE_OPEN_ERROR;
	}

	return	byGeneration;
}

unsigned __int64	FLFileKeyGenerator::GetFileKey()
{
	return	un64Key;
}
