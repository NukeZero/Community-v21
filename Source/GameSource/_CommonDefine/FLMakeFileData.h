#ifndef __FLMAKEFILEDATA_H__
#define __FLMAKEFILEDATA_H__

#pragma once

#include "FLFileData.h"
#include <crypto/FLDes.h>

class	FLMakeFileData
{

public:
	FLMakeFileData();
	~FLMakeFileData();

	static	FLMakeFileData*	GetInstance();

	bool	MakeFileDataFromList( const TCHAR* szFileName );


private:
	void	Init();

	void	Decrypt( const char* szIn, char* szOut );


private:
	TCHAR	m_szCurrentDirectory[_MAX_PATH];

	FL_des3_context m_tCtx3;
};

#define g_xMakeFileData	FLMakeFileData::GetInstance()


#endif // __FLMAKEFILEDATA_H__