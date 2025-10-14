
#ifndef	__FLLOGFILENAME_H__
#define	__FLLOGFILENAME_H__


#include "FLLogCommon.h"


class	FLLogFileName
{
public:

	virtual ~FLLogFileName();

	bool	Generate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount );

private:

	virtual bool	DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount )	= 0;

};

//////////////////////////////////////////////////////////////////////////

class	FLLogFixedFileName : public FLLogFileName
{
public:

	FLLogFixedFileName();
	~FLLogFixedFileName();

	bool	SetInfo( const TCHAR* szFolder, const TCHAR* szFixedName );

private:

	bool	DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount );

private:

	TCHAR	m_szFolder[MAX_PATH];
	TCHAR	m_szFileName[MAX_PATH];

};

//////////////////////////////////////////////////////////////////////////

class	FLLogDateFileName : public FLLogFileName
{
public:

	FLLogDateFileName();
	~FLLogDateFileName();

	bool	SetInfo( const TCHAR* szFolder, const TCHAR* szPrefix );

private:

	bool	DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount );

private:

	TCHAR	m_szFolder[MAX_PATH];
	TCHAR	m_szPrefix[MAX_PATH];

};

//////////////////////////////////////////////////////////////////////////

class	FLLogDateTimeFileName : public FLLogFileName
{
public:

	FLLogDateTimeFileName();
	~FLLogDateTimeFileName();

	bool	SetInfo( const TCHAR* szFolder, const TCHAR* szPrefix );

private:

	bool	DoGenerate( TCHAR* szFileName, size_t cchFileName, DWORD dwTryCount );

private:

	TCHAR	m_szFolder[MAX_PATH];
	TCHAR	m_szPrefix[MAX_PATH];

};






#endif