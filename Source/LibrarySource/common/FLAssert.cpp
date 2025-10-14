
#include "FLAssert.h"
#include "FLStringFunction.h"

#include <sstream>


namespace
{
	wchar_t*	InnerMultiByteToWideCharWithAlloc( UINT dwCodePage, const char* szMultiByteString, int nMultiBytes )
	{
		wchar_t* pWideChar = NULL;
		if( szMultiByteString != NULL )
		{
			int nLen = ::MultiByteToWideChar( dwCodePage, 0, szMultiByteString, nMultiBytes, NULL, 0 );
			if( nLen > 0 )
			{
				pWideChar = new wchar_t[nLen];
				if( pWideChar != NULL )
				{
					if( ::MultiByteToWideChar( dwCodePage, 0, szMultiByteString, nMultiBytes, pWideChar, nLen ) <= 0 )
					{
						delete [] pWideChar;
						pWideChar = NULL;
					}
				}
			}
		}

		return pWideChar;
	}

	static FLAssertHooker	g_pAssertHooker = NULL;
}


bool	FLAssertFunctionA(	bool		bExp,
							const char*	szExpression,
							const char*	szDescription,
							const char*	szFunction,
							const char*	szFile,
							int			nLine,
							bool*		pIgnoreAlways )
{
	if( bExp == true )
	{
		return true;
	}

	wchar_t* szWExpression = InnerMultiByteToWideCharWithAlloc( CP_ACP, szExpression, -1 );
	wchar_t* szWDescription = InnerMultiByteToWideCharWithAlloc( CP_ACP, szDescription, -1 );
	wchar_t* szWFunction = InnerMultiByteToWideCharWithAlloc( CP_ACP, szFunction, -1 );
	wchar_t* szWFile = InnerMultiByteToWideCharWithAlloc( CP_ACP, szFile, -1 );

	bool bResult = FLAssertFunctionW( bExp, szWExpression, szWDescription, szWFunction, szWFile, nLine, pIgnoreAlways );

	if( szWExpression != NULL )
	{
		delete [] szWExpression;
		szWExpression = NULL;
	}

	if( szWDescription != NULL )
	{
		delete [] szWDescription;
		szWDescription = NULL;
	}

	if( szWFunction != NULL )
	{
		delete [] szWFunction;
		szWFunction = NULL;
	}

	if( szWFile != NULL )
	{
		delete [] szWFile;
		szWFile = NULL;
	}

	return bResult;
}

bool	FLAssertFunctionW(	bool			bExp,
							const wchar_t*	szExpression,
							const wchar_t*	szDescription,
							const wchar_t*	szFunction,
							const wchar_t*	szFile,
							int				nLine,
							bool*			pIgnoreAlways )
{
	if( bExp == true )
	{
		return true;
	}

	if( szExpression == NULL )	szExpression = L"no expression";
	if( szDescription == NULL )	szDescription = L"no description";
	if( szFunction == NULL )	szFunction = L"unknown";
	if( szFile == NULL )		szFile = L"unknown";

	//////////////////////////////////////////////////////////////////////////
	if( g_pAssertHooker != NULL )
	{
		if( g_pAssertHooker( szExpression, szDescription, szFunction, szFile, nLine, pIgnoreAlways ) == true )
		{
			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	std::wostringstream strMessage;
	strMessage	<< L"Assertion failed.\r\n\r\nExpression: "
				<< szExpression
				<< L"\r\nDescription: "
				<< szDescription
				<< L"\r\n\r\nFunction: "
				<< szFunction
				<< L"\r\nFile: "
				<< szFile
				<< L"\r\nLine: "
				<< nLine
				<< L"\r\n";

	HWND hHwnd = ::GetActiveWindow();
	int nChoice = ::MessageBoxW( hHwnd, strMessage.str().c_str(), L"Assertion failed.", MB_ABORTRETRYIGNORE | MB_ICONERROR );

	if( nChoice == IDIGNORE )
	{
		if( pIgnoreAlways != NULL )
		{
			*pIgnoreAlways = true;
		}

		return true;
	}
	else if( nChoice == IDABORT )
	{
		::exit( EXIT_FAILURE );
	}

	return false;
}

FLAssertHooker	FLSetAssertHooker( FLAssertHooker pHooker )
{
	FLAssertHooker pOldHooker = g_pAssertHooker;
	g_pAssertHooker = pHooker;
	return pOldHooker;
}
