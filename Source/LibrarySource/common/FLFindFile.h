
#ifndef	__FLFINDFILE_H__
#define	__FLFINDFILE_H__


#include "FLCommon.h"


//////////////////////////////////////////////////////////////////////////
class	FLFindFileInfo
{
public:

	FLFindFileInfo( const WIN32_FIND_DATA* pFindFileData, const TCHAR* szFolder );
	~FLFindFileInfo();

	bool	IsFolder()	const;
	bool	IsDots()	const;
	bool	IsReadOnly()	const;

	const TCHAR*	GetName()	const;

private:

	WIN32_FIND_DATA	m_tFindFileData;
	TCHAR			m_szFullFileName[MAX_PATH];

};
//////////////////////////////////////////////////////////////////////////


class	FLFindFile
{
public:

	FLFindFile();
	~FLFindFile();

	bool	FindFirst( const TCHAR* szFolder, const TCHAR* szFindFilter );
	bool	FindFirst( const TCHAR* szFindFilter );
	bool	FindNext();

	FLFindFileInfo	GetFindFileInfo()	const;

	void	Close();

private:

	HANDLE			m_hFind;
	WIN32_FIND_DATA	m_tFindFileData;
	TCHAR			m_szDestFolder[MAX_PATH];

};






#endif