#ifndef __FLFILEKEYGENERATOR_H__
#define __FLFILEKEYGENERATOR_H__



#include <common/FLFile.h>

#define	FL_KEY_GENERATION_READ_SIZE			9
#define FL_KEY_GENERATION_MAX_FILE_SIZE		6291456			// 6MByte

class	FLFileKeyGenerator
{
	public:
		enum
		{
			FL_KEY_GENERATION_NONE					= 0
			, FL_KEY_GENERATION_FILE_OPEN_ERROR
			, FL_KEY_GENERATION_FILE_TO_BIG
			, FL_KEY_GENERATION_SUCCESS
		};

		FLFileKeyGenerator();
		~FLFileKeyGenerator();

		BYTE				FileKeyGeneration( const TCHAR* pFileName );
		unsigned __int64	GetFileKey();

	private:
		BYTE				byGeneration;
		unsigned __int64	un64Key;
};

#endif	//__FLFILEKEYGENERATOR_H__
