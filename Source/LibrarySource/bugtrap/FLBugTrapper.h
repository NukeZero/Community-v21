
#ifndef __FLDUMPBUGTRAPPER_H__
#define __FLDUMPBUGTRAPPER_H__

#include "../common/FLCommon.h"
#include "../../../ThirdParty/BugTrap/BugTrap.h"


class FLBugTrapper
{
	public:
		static void		Install( const TCHAR* pszAppName, const bool bInternal = false );

		static void		AddFile( const TCHAR* pszFullPathFileName );
		static void		AddFile( const TCHAR* pszFullPathFolderName, DWORD dwDays );

		static void		SendToBugTrapServer( const DWORD dwDumpType = MiniDumpNoDump );


	private:
		FLBugTrapper();
		~FLBugTrapper();

		static bool		s_bInstall;
};

#endif // __FLDUMPBUGTRAPPER_H__
