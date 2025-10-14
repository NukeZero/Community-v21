// merge2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "merge2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL	  g_bSync;
extern void	  DoMerge(); 
extern BOOL   SplitFile( LPCTSTR szFileName ); 
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		if( argc >= 2 )
		{
			if( _tcscmp( argv[1], "-sync" ) == 0 )
			{
				g_bSync = TRUE;
			}
			else if( argc >=3 && _tcscmp( argv[1], "-split" ) == 0 )
			{
				SplitFile( argv[2] );
				return 0;
			}
		}

		// TODO: code your application's behavior here.
		DoMerge();
	}

	return nRetCode;
}



