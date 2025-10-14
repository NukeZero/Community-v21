
#ifndef __FLFILE_H__
#define __FLFILE_H__

#include "FLCommon.h"


class	FLFile
{
public:
	FLFile();
	~FLFile();

	bool				Create(	const TCHAR* szName,
								DWORD dwAccessMode = GENERIC_WRITE,
								DWORD dwShareMode = FILE_SHARE_READ,
								DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL );
	bool				Open(	const TCHAR* szName,
								DWORD dwAccessMode = GENERIC_READ,
								DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE,
								DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL );

	void				Close();

	bool				Opened()	const;

	size_t				Read( void* pBuffer, size_t nSize );
	size_t				Write( const void* pBuffer, size_t nSize );

	__int64				Seek( __int64 n64Offset, int nOrigin );
	bool				Flush();

	unsigned __int64	Size()	const;


private:

	HANDLE				m_hFile;

};



#endif
