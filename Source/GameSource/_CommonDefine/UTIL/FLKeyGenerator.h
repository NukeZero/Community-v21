#ifndef __FLKEYGENERATOR_H__
#define __FLKEYGENERATOR_H__

#pragma once


#include "FLFileKeyGenerator.h"


#define _MAX_HEAD_KEY_SIZE	6
#define _MAX_TAIL_KEY_SIZE	6
#define _MAX_KEY_SIZE		33


class	FLKeyGenerator
{
	private:
		TCHAR	m_szHeadKey[ _MAX_HEAD_KEY_SIZE ];
		TCHAR	m_szTailKey[ _MAX_TAIL_KEY_SIZE ];

		TCHAR	m_szKey[ _MAX_KEY_SIZE ];

		TCHAR	GetNumeral( WORD wData );
		TCHAR	GetSmallLetter( WORD wData );
		TCHAR	GetCapitalLetter( WORD wData );
		TCHAR	GetSpecificFirst( WORD wData );
		TCHAR	GetSpecificSecond( WORD wData );

	public:
		FLKeyGenerator();
		~FLKeyGenerator();


		static	FLKeyGenerator	*	GetInstance();

		bool	SetHeadKey( TCHAR* pHeadKey );
		bool	SetTailKey( TCHAR* pTailKey );

		bool	SetKey( TCHAR* pHeadKey, TCHAR* pTailKey );

		bool	MakeFileKey( const TCHAR* pFileName );

		TCHAR*	GetKey();
};

#define g_xKeyGenerator	FLKeyGenerator::GetInstance()


#endif	//__FLKEYGENERATOR_H__
