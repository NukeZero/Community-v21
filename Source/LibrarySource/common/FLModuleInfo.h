#ifndef __FLMODULEINFO_H__
#define __FLMODULEINFO_H__


#include "FLCommon.h"

#pragma comment( lib, "Version.lib" )


class	FLModuleInfo
{
public:

	FLModuleInfo();

	bool	Load( const TCHAR* szPath );

	const TCHAR*	CompanyName()		const;
	const TCHAR*	ProductName()		const;
	const TCHAR*	FileDescription()	const;

	const WORD*		FileVersion()		const;
	const WORD*		ProductVersion()	const;

private:

	bool	Query( void* pData );
	bool	QueryFixedInfo( void* pData, VS_FIXEDFILEINFO* pFixedInfo );
	bool	QueryStringInfo( void* pData, const TCHAR* szKey, TCHAR* szValue, size_t cchCount );

private:

	WORD	m_wFileVersion[4];
	WORD	m_wProductVersion[4];

	TCHAR	m_szCompanyName[128];
	TCHAR	m_szProductName[128];
	TCHAR	m_szFileDescription[512];

};



#endif
