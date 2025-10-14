#ifndef __FLTEXTFILE_H__
#define __FLTEXTFILE_H__


#include "FLCommon.h"
#include "FLFile.h"

class	FLTextFile
{
public:

	FLTextFile();
	~FLTextFile();

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

	size_t				WriteText( const TCHAR* szText, size_t cchCount );

	size_t				ReadText( TCHAR* szText, size_t cchCount );
	size_t				ReadLine( TCHAR* szText, size_t cchCount );	// TODO 공백 라인과 EOF 구분

	__int64				Seek( __int64 n64Offset, int nOrigin );
	bool				Flush();

	unsigned __int64	Size()	const;

private:

	FLFile				m_cFile;

};

#endif	//__FFL_TEXT_FILE_H__
