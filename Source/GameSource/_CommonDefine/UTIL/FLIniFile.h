#ifndef __FLINIFILE_H__
#define __FLINIFILE_H__


#include <io.h>


class	FLIniFile
{
	public:
		struct E_BASE_PATH {
			enum E {
				ABSOLUTE_PATH		= 0,
				CURRENT_PATH,
				WINDOWS_PATH,
			};
		};

		enum	E_RESULT
		{
			E_OK			= 0
			, E_ERROR
			, E_NOT_EXIST
			, E_BASEPATH_TYPE_ERROR
		};

	public:
		FLIniFile();
		virtual ~FLIniFile();

		E_RESULT	InitializeINIFile(  const TCHAR * pPathString, const E_BASE_PATH::E eBasePath = E_BASE_PATH::CURRENT_PATH );

		bool	IniReadString	( const TCHAR * pApp, const TCHAR * pKey, TCHAR * o_rOut, const int CCh );
		bool	IniWriteString	( const TCHAR * pApp, const TCHAR * pKey, const TCHAR * pIn );

		bool	IniReadInt		( const TCHAR * pApp, const TCHAR *pKey, int & o_rOut );
		bool	IniWriteInt		( const TCHAR * pApp, const TCHAR * pKey, const int & rIn );

	private:
		TCHAR	m_szFilePath[ MAX_PATH ];
};

#endif	//__FLINIFILE_H__

