#ifndef __FLDABADATACONVERT_H__
#define __FLDABADATACONVERT_H__

#include "DbManager.h"

class FLDBDataConvert
{
	public:
		FLDBDataConvert();
		~FLDBDataConvert();

		static FLDBDataConvert* GetInstance();

		bool	ConvertGuildBank();

	private:
		void	Init();
		void	Clear();
		bool	IsValidString( const TCHAR* pszData );
		void	OutputConvertLog( int nConvertDataKind, DWORD dwId, const TCHAR* pszColumn, const TCHAR* pszData );


	private:
		CQuery*	m_pOldCharacterQuery;
		CQuery* m_pNewCharacterQuery;

		enum CD_KIND
		{
			CD_GUILD_BANK = 0
		};
};

#define		g_kFLDBDataConvert		FLDBDataConvert::GetInstance()

#endif // __FLDABADATACONVERT_H__

