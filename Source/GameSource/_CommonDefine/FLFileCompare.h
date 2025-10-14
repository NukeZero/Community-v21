#ifndef	__FLFILECOMPARE_H__
#define __FLFILECOMPARE_H__

#pragma once

#include "FLFileData.h"


class	FLFileCompare
{

public:

				FLFileCompare( void );
				~FLFileCompare( void );

	
public:
	static	FLFileCompare*	GetInstance();

	bool	CheckFile();

	bool	CompareFile( T_FILE_INFO& info );

	void	InsertFileData( T_FILE_INFO& tFileInfo );

	void	Clear();

private:

	void	Init();

	bool	DeleteWrongFile( TCHAR* pszDeleteFileFullPath );

	FileContainer	m_listFileContainer;

	TCHAR		m_szCurrentDirectory[MAX_PATH];	//사용자 현재 디렉토리
	int			m_nCompareCount;

};

#define g_xFileCompare	FLFileCompare::GetInstance()

#endif	//__FLFILECOMPARE_H__