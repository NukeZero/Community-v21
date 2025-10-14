#include "stdafx.h"
#include "NpcScript.h"

/////////////////////////////////////////////////////////////////////////////
// extern functions
/////////////////////////////////////////////////////////////////////////////
extern		void InitFunctions();
extern		PFN GetFunction( const char* szFunction );

/////////////////////////////////////////////////////////////////////////////
// global variable
/////////////////////////////////////////////////////////////////////////////
static map< string, int >	g_mapKey;
static vector<string>		g_strings;
static int g_nLanguage;

//static FLCStyleScript		g_kNPCScript;

static map< string, FLExecuter_Sequence * >		g_mapExternFunc;
static CNpcScript			g_kNPCScript;

/////////////////////////////////////////////////////////////////////////////
// global functions
/////////////////////////////////////////////////////////////////////////////
LPCTSTR						GetDialogText( DWORD dwText );
BOOL						LoadDialogText( LPCTSTR szFile );
BOOL						InitDLL( LPCTSTR szFile );
int							GetKeyIndex( const char * szKey );

bool						CompileScript( FLCStyleReader & script );
bool						FindNPCFunction( const string & func );
bool						CallNPCFunction( /*CNpcScript & kNPCScript,*/ const string & func );

CNpcScript &				GetNPCScriptInstance()
{
	return g_kNPCScript;
}

//#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p)=NULL; } }

// 베트남어 처리
// 유니코드에서 1258 : [완성형 유니코드] -> [조합형 유니코드] -> [CodePage 1258 ANSI 호환코드] 
// 1258에서 유니코드 : [CodePage 1258 ANSI 호환코드] -> [조합형 유니코드] -> [완성형 유니코드] 

namespace VTN
{
	wchar_t wVTNToneChar[24][6] = {

		/*--------Precomposed characters------------Various*/
		{0x00c1, 0x00c0, 0x1ea2, 0x00c3, 0x1ea0,    0x0041},
		{0x00e1, 0x00e0, 0x1ea3, 0x00e3, 0x1ea1,    0x0061},
		{0x1eae, 0x1eb0, 0x1eb2, 0x1eb4, 0x1eb6,    0x0102},
		{0x1eaf, 0x1eb1, 0x1eb3, 0x1eb5, 0x1eb7,    0x0103},
		{0x1ea4, 0x1ea6, 0x1ea8, 0x1eaa, 0x1eac,    0x00c2},
		{0x1ea5, 0x1ea7, 0x1ea9, 0x1eab, 0x1ead,    0x00e2},
		{0x00c9, 0x00c8, 0x1eba, 0x1ebc, 0x1eb8,    0x0045},
		{0x00e9, 0x00e8, 0x1ebb, 0x1ebd, 0x1eb9,    0x0065},
		{0x1ebe, 0x1ec0, 0x1ec2, 0x1ec4, 0x1ec6,    0x00ca},
		{0x1ebf, 0x1ec1, 0x1ec3, 0x1ec5, 0x1ec7,    0x00ea},
		{0x00cd, 0x00cc, 0x1ec8, 0x0128, 0x1eca,    0x0049},
		{0x00ed, 0x00ec, 0x1ec9, 0x0129, 0x1ecb,    0x0069},
		{0x00d3, 0x00d2, 0x1ece, 0x00d5, 0x1ecc,    0x004f},
		{0x00f3, 0x00f2, 0x1ecf, 0x00f5, 0x1ecd,    0x006f},
		{0x1ed0, 0x1ed2, 0x1ed4, 0x1ed6, 0x1ed8,    0x00d4},
		{0x1ed1, 0x1ed3, 0x1ed5, 0x1ed7, 0x1ed9,    0x00f4},
		{0x1eda, 0x1edc, 0x1ede, 0x1ee0, 0x1ee2,    0x01a0},
		{0x1edb, 0x1edd, 0x1edf, 0x1ee1, 0x1ee3,    0x01a1},
		{0x00da, 0x00d9, 0x1ee6, 0x0168, 0x1ee4,    0x0055},
		{0x00fa, 0x00f9, 0x1ee7, 0x0169, 0x1ee5,    0x0075},
		{0x1ee8, 0x1eea, 0x1eec, 0x1eee, 0x1ef0,    0x01af},
		{0x1ee9, 0x1eeb, 0x1eed, 0x1eef, 0x1ef1,    0x01b0},
		{0x1ef2, 0x00dd, 0x1ef6, 0x1ef8, 0x1ef4,    0x0059},
		{0x1ef3, 0x00fd, 0x1ef7, 0x1ef9, 0x1ef5,    0x0079}
	};

	wchar_t wVTNCombiningDia[5] = {0x0301, 0x0300, 0x0309, 0x0303, 0x0323};
}



int g_nKeys[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	865,
	866,
};

BOOL MakeKeyIndex()
{
	int n = sizeof(g_nKeys) / sizeof(g_nKeys[0]);
	for( int i=0; i<n; ++i )
	{
		int nIndex = g_nKeys[i];
		const char* szText = GetDialogText( nIndex );

		if( szText[0] != '\0' )
			g_mapKey.insert( make_pair( szText, nIndex ) );
		else
			return FALSE;
	}
	return TRUE;
}

int GetKeyIndex( const char * szKey )
//int GetKeyIndex( const string & szKey )
{
	map< string, int >::iterator it = g_mapKey.find( szKey );
	if( it != g_mapKey.end() )
		return it->second;
	else
		return -1;
}

bool GetTone(wchar_t input, int* pnLetterLine, int* pnTone)
{
	using namespace VTN;

	for(int i=0; i<24; i++)
	{
		for(int j=0; j<5; j++)
		{
			if(wVTNToneChar[i][j] == input)
			{
				if(pnLetterLine != NULL && pnTone != NULL)
				{
					*pnLetterLine = i;
					*pnTone = j;
					return true;
				}
			}
		}
	}

	return false;
}

// 완성형 유니코드 -> 조합형 유니코드
void DecomposeString(wchar_t* pwSrc, wchar_t* pwDest, int nSize)
{
	using namespace VTN;

	if(pwSrc == NULL || pwDest == NULL || nSize <= 0)
		return;

	int nLetterLine;
	int nTone;

	for(int i=0; i<nSize; i++)
	{
		if(GetTone(*pwSrc, &nLetterLine, &nTone))
		{
			*pwDest = wVTNToneChar[nLetterLine][5];
			++pwDest;
			*pwDest = wVTNCombiningDia[nTone];
		}
		else
		{
			*pwDest = *pwSrc;
		}
		++pwSrc;
		++pwDest;
	}
}

int WideCharToMultiByteEx(
						  UINT     CodePage,
						  DWORD    dwFlags,
						  LPCWSTR  lpWideCharStr,
						  int      cchWideChar,
						  LPSTR    lpMultiByteStr,
						  int      cchMultiByte,
						  LPCSTR   lpDefaultChar,
						  LPBOOL   lpUsedDefaultChar)
{
	int nLength;
	if(g_nLanguage == LANG_VTN && lpMultiByteStr != NULL && cchMultiByte > 0)
	{
		int nSize = wcslen((LPCWSTR)lpWideCharStr);
		wchar_t* pwSrc = new wchar_t[nSize+1];
		wchar_t* pwDest = new wchar_t[(nSize*2)+1];
		memcpy(pwSrc, lpWideCharStr, sizeof(wchar_t)*(nSize+1));
		DecomposeString(pwSrc, pwDest, nSize+1);
		nLength = WideCharToMultiByte(CodePage, dwFlags, pwDest, cchWideChar, lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar);
		SAFE_DELETE_ARRAY(pwSrc);
		SAFE_DELETE_ARRAY(pwDest);
	}
	else
		nLength = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar);

	return nLength;
}

//-----------------------------------------------------------------------------------------------------------------------------------//

//CNpcScript & CNpcScript::GetInstance()
//{
//	static CNpcScript inst;
//	return inst;
//}

//@@@@@ 맘에 안듬
int CNpcScript::CallIncomeFunc( /*CNpcScript & kNPCScript,*/ const char * szFunc, const vector< int > & args )
{
	if( ::strcmp( "GetSrcId", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetSrcId();
	}
	if( ::strcmp( "GetDstId", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetDstId();
	}
	if( ::strcmp( "NpcId", szFunc ) == 0 )
	{
		return /*kNPCScript.*/NpcId();
	}
	if( ::strcmp( "Say", szFunc ) == 0 )
	{
		/*kNPCScript.*/Say( args.at( 0 ) );
		return 0;
	}
	if( ::strcmp( "Speak", szFunc ) == 0 )
	{
		/*kNPCScript.*/Speak( args.at( 0 ), args.at( 1 ) );
		return 0;
	}
	if( ::strcmp( "Random", szFunc ) == 0 )
	{
		return /*kNPCScript.*/Random( args.at( 0 ) );
	}
	if( ::strcmp( "Replace", szFunc ) == 0 )
	{
		/*kNPCScript.*/Replace( args.at( 0 ), args.at( 1 ), args.at( 2 ), args.at( 3 ) );
		return 0;
	}
	if( ::strcmp( "GetPlayerLvl", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetPlayerLvl();
	}
	if( ::strcmp( "GetPlayerJob", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetPlayerJob();
	}
	if( ::strcmp( "GetQuestState", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetQuestState( args.at( 0 ) );   
	}
	if( ::strcmp( "IsSetQuest", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsSetQuest( args.at( 0 ) );
	}
	if( ::strcmp( "CreateItem", szFunc ) == 0 )
	{
		/*kNPCScript.*/CreateItem( args.at( 0 ), args.at( 1 ) );  
		return 0;
	}
	if( ::strcmp( "RemoveGold", szFunc ) == 0 )
	{
		/*kNPCScript.*/RemoveGold( args.at( 0 ) );
		return 0;
	}
	if( ::strcmp( "GetPlayerGold", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetPlayerGold(); 
	}
	if( ::strcmp( "IsGuildQuest", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsGuildQuest( args.at( 0 ) );	
	}
	if( ::strcmp( "GetGuildQuestState", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetGuildQuestState( args.at( 0 ) );
	}
	if( ::strcmp( "IsWormonServer", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsWormonServer();  
	}
	if( ::strcmp( "MonHuntStart", szFunc ) == 0 )
	{
		return /*kNPCScript.*/MonHuntStart( args.at( 0 ), args.at( 1 ), args.at( 2 ), args.at( 3 ) );      
	}
	if( ::strcmp( "MonHuntStartParty", szFunc ) == 0 )
	{
		return /*kNPCScript.*/MonHuntStartParty( args.at( 0 ), args.at( 1 ), args.at( 2 ), args.at( 3 ) );
	}
	if( ::strcmp( "RemoveItem", szFunc ) == 0 )
	{
		/*kNPCScript.*/RemoveItem( args.at( 0 ), args.at( 1 ) );
		return 0;
	}
	if( ::strcmp( "InitStat", szFunc ) == 0 )
	{
		/*kNPCScript.*/InitStat();
		return 0;
	}
	if( ::strcmp( "GetItemNum", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetItemNum( args.at( 0 ) );    
	}
	if( ::strcmp( "RemoveAllItem", szFunc ) == 0 )
	{
		/*kNPCScript.*/RemoveAllItem( args.at( 0 ) );
		return 0;
	}
	if( ::strcmp( "ChangeJob", szFunc ) == 0 )
	{
		/*kNPCScript.*/ChangeJob( args.at( 0 ) );
		return 0;
	}
	if( ::strcmp( "AddKey", szFunc ) == 0 )
	{
		/*kNPCScript.*/AddKey( args.at( 0 ) );	
		return 0;
	}
	if( ::strcmp( "RemoveKey", szFunc ) == 0 )
	{
		/*kNPCScript.*/RemoveKey( args.at( 0 ) );
		return 0;
	}
	if( ::strcmp( "IsParty", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsParty(); 
	}
	if( ::strcmp( "IsPartyMaster", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsPartyMaster(); 
	}
	if( ::strcmp( "GetPartyNum", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetPartyNum();	 
	}
	if( ::strcmp( "IsPartyGuild", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsPartyGuild(); 
	}
	if( ::strcmp( "IsGuild", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsGuild(); 
	}
	if( ::strcmp( "IsGuildMaster", szFunc ) == 0 )
	{
		return /*kNPCScript.*/IsGuildMaster(); 
	}
	if( ::strcmp( "CreateGuild", szFunc ) == 0 )
	{
		/*kNPCScript.*/CreateGuild();
		return 0;
	}
	if( ::strcmp( "PlaySound", szFunc ) == 0 )
	{
		/*kNPCScript.*/PlaySound( args.at( 0 ) ); 
		return 0;
	}
	if( ::strcmp( "SayQuest", szFunc ) == 0 )
	{
		/*kNPCScript.*/SayQuest( args.at( 0 ), args.at( 1 ) );
		return 0;
	}

	if( ::strcmp( "PrintSystemMessage", szFunc ) == 0 )
	{
		/*kNPCScript.*/PrintSystemMessage( args.at( 0 ) ); 
		return 0;
	}
	if( ::strcmp( "AddCondKey", szFunc ) == 0 )
	{
		/*kNPCScript.*/AddCondKey( args.at( 0 ), args.at( 1 ) );
		return 0;
	}
	if( ::strcmp( "GetEmptyInventoryNum", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetEmptyInventoryNum( args.at( 0 ) );
	}
	if( ::strcmp( "LaunchQuest", szFunc ) == 0 )
	{
		/*kNPCScript.*/LaunchQuest();	
		return 0;
	}
	if( ::strcmp( "Exit", szFunc ) == 0 )
	{
		/*kNPCScript.*/Exit();	
		return 0;
	}
	if( ::strcmp( "GetPlayerExpPercent", szFunc ) == 0 )
	{
		return /*kNPCScript.*/GetPlayerExpPercent();
	}
	if( ::strcmp( "SetLevel", szFunc ) == 0 )
	{
		/*kNPCScript.*/SetLevel( args.at( 0 ) );
		return 0;
	}
	if( ::strcmp( "AddGPPoint", szFunc ) == 0 )
	{
		/*kNPCScript.*/AddGPPoint( args.at( 0 ) );
		return 0;
	}


	FLERROR_LOG( PROGRAM_NAME, _T("[ func(%s) : INVALID FUNCTION ]"), szFunc );
	DEBUG_BREAK;
	return 0;
}


void CNpcScript::SetLang(int nLanguage)
{
	g_nLanguage = nLanguage;
}

BOOL LoadDialogText( LPCTSTR szFile )
{
	FILE* fp = NULL;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	if( fopen_s( &fp, szFile, "rt" ) != 0 )
	{
		return FALSE;
	}
#else
	fp = fopen( szFile, "rt" );
#endif
	if( fp == NULL )
		return FALSE;

	BOOL bOK = FALSE;
	char szBuffer[1024] = { 0, };

	if(g_nLanguage == LANG_VTN)
	{
		int nFileLength = _filelength(_fileno(fp));
		int nSize = nFileLength + 2;
		char* pProg = new char[ nSize ];
		if( !pProg )
			return FALSE;

		fseek( fp, 0, SEEK_SET );
		if( fread( pProg, nFileLength, 1, fp ) ) 
		{
			*(pProg + nFileLength ) = '\0';
			*(pProg + nFileLength + 1 ) = '\0';

			if( (BYTE)*(pProg + 0 ) == 0xff && (BYTE)*(pProg + 1 ) == 0xfe ) // is unicode ?
			{
				char* lpMultiByte = new char[ nFileLength + 2 ];
				int nResult = WideCharToMultiByteEx( 1258, 0, (LPWSTR)(pProg+2), -1, lpMultiByte, nFileLength, NULL, NULL );
				if( nResult > 0 )
				{
					lpMultiByte[nResult-1] = 0;
					//char szBuffer[1024] = { 0, };
					//int nIndex = 0;

					std::string strBuffer;

					CHAR* pCur = lpMultiByte;
					while(*pCur != '\0')
					{
						//szBuffer[nIndex] = *pCur;
						if(*pCur == '\r')
						{
							//szBuffer[nIndex] = 0;
							//g_strings.push_back( szBuffer );
							g_strings.push_back( strBuffer );
							strBuffer.clear();
							//nIndex = 0;
							pCur += 2;
						}
						else
						{
							strBuffer.push_back( *pCur );
							//nIndex++;
							pCur++;
						}
					}
				}

				delete [] lpMultiByte;
				lpMultiByte = NULL;
			}

			if( ferror( fp ) )
				bOK = FALSE;
			else
				bOK = TRUE;
		}

		delete [] pProg;
		pProg = NULL;
	}
	else
	{
		for( ;; )
		{
			char* result = fgets( szBuffer, sizeof(szBuffer), fp );
			if( result )
			{
				if( szBuffer[0] != '\0' )
					szBuffer[ strlen( szBuffer ) - 1 ] = 0;
					//szBuffer[ _countof( szBuffer ) - 1 ] = 0;
					
				g_strings.push_back( std::string( szBuffer ) );
			}
			else
			{
				if( ferror( fp ) )
					bOK = FALSE;
				else
					bOK = TRUE;
				break;
			}
		}
	}

	fclose( fp );
	return bOK;
}

LPCTSTR GetDialogText( DWORD dwText )
{
	LPCTSTR lpText = "";

	if( dwText < g_strings.size() )
		lpText = g_strings.at( dwText ).c_str();
	return lpText;
}

BOOL InitDLL( LPCTSTR szFile )
{
	//g_strings.reserve( 100 );

	if( g_strings.size() > 0 )		// for single initilize
		return TRUE;

	if( LoadDialogText( szFile ) == FALSE )
		return FALSE;

	if( MakeKeyIndex() == FALSE )
		return FALSE;

	InitFunctions();
	return TRUE;
}

bool CompileScript( FLCStyleReader & script )
{
	return FLCStyleScript::CompileScript( script, g_mapExternFunc );
}

bool FindNPCFunction( const string & func )
{
	map< string, FLExecuter_Sequence * >::iterator it = g_mapExternFunc.find( func );
	return ( it != g_mapExternFunc.end() ) ? true : false;
}

bool CallNPCFunction( /*CNpcScript & kNPCScript,*/ const string & func )
{
	map< string, FLExecuter_Sequence * >::iterator it = g_mapExternFunc.find( func );
	if( it == g_mapExternFunc.end() )
	{
#ifdef _DEBUG
	//	FLERROR_LOG( PROGRAM_NAME, _T("[ func(%s) : NOT FOUND ]"), func.c_str() );
	//@@@@@@@	DEBUG_BREAK;
#endif //_DEBUG
		return false;
	}

	FLExecuter_Sequence * p = it->second;
	p->Execute( /*kNPCScript*/ );
	return true;
}

//
//bool CompileScript( FLCStyleReader & script )
//{	
//	vector< FLCStyleReader::FLToken > v;
//
//	// read
//	RELEASE_ASSERT( reader.Load( v ) == true );
//
//	// complie
//
//	vector< FLCStyleReader::FLToken >::const_iterator it = v.begin();
//	vector< FLCStyleReader::FLToken >::const_iterator it_end = v.end();
//	while( it != it_end )
//	{
//		// void 함수 시작	 
//		const FLCStyleReader::FLToken ret = FLCStyleReader::PopToken( it, it_end );
//		if( ret.e == FLCStyleReader::EOF_VALUE )
//			break;
//
//		RELEASE_ASSERT( ret.e == FLCStyleReader::NAME_VALUE && ::strcmp( ret.s.c_str(), "void" ) == 0 );
//
//		// namespace
//		const FLCStyleReader::FLToken nameSpace = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( nameSpace.e == FLCStyleReader::NAME_VALUE && ::strcmp( nameSpace.s.c_str(), "CNpcScript" ) == 0 );
//
//		// ::
//		const FLCStyleReader::FLToken n1 = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( n1.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( n1.s.c_str(), ":" ) == 0 );
//		const FLCStyleReader::FLToken n2 = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( n2.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( n2.s.c_str(), ":" ) == 0 );
//
//
//		// 함수명
//		const FLCStyleReader::FLToken func = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( func.e == FLCStyleReader::NAME_VALUE );
//
//		// (
//		const FLCStyleReader::FLToken o = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( o.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( o.s.c_str(), "(" ) == 0  );
//		//	)
//		const FLCStyleReader::FLToken c = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( c.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( c.s.c_str(), ")" ) == 0 );
//
//
//		// {
//		const FLCStyleReader::FLToken open = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( open.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( open.s.c_str(), "{" ) == 0 );
//
//		// 함수 내용
//		FLExecuter_Sequence * p = compileProcess( it, it_end );
//		RELEASE_ASSERT( p != NULL );
//
//		// 함수 끝		// }
//		const FLCStyleReader::FLToken close = FLCStyleReader::PopToken( it, it_end );
//		RELEASE_ASSERT( close.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( close.s.c_str(), "}" ) == 0 );
//
//		// insert
//		RELEASE_ASSERT( g_mapExternFunc.insert( make_pair( func.s, p ) ).second == TRUE );
//	}
//	return true;
//
//}

/////////////////////////////////////////////////////////////////////////////
// CNpcScript
/////////////////////////////////////////////////////////////////////////////

void CNpcScript::DoExec( const char* szName, const char* szKey )
{
	if( szName == NULL || szKey == NULL )
	{
		return;
	}

	//const string strKey( szKey );
	int nIndex = GetKeyIndex( szKey );
	if( nIndex < 0 )
		return;

	char szFunction[64] = { 0, };
	StringCchPrintfEx( szFunction, sizeof( szFunction ) /sizeof( szFunction[0] ), NULL, NULL, 0, "%s_%d", szName, nIndex );
//	PFN pmf = GetFunction( szFunction );
//	if( pmf )
//	{
//		(this->*pmf)();
//	}
//#ifdef _DEBUG
//	else
//	{
//		char szBuffer[1024] = { 0, };
//		StringCchPrintfEx( szBuffer, sizeof( szBuffer ) /sizeof( szBuffer[0] ), NULL, NULL, 0, "%s key:%s\n", szFunction, szKey );
//		OutputDebugString( szBuffer );
//	}
//#endif
	
	CallNPCFunction( /**this,*/ szFunction );
}

BOOL CNpcScript::Find( const char* szName, const char* szKey )
{
	if( szName == NULL || szKey == NULL )
	{
		return FALSE;
	}

	int nIndex = GetKeyIndex( szKey );
	if( nIndex < 0 )
		return FALSE;

	char szFunction[64] = { 0, };
	StringCchPrintfEx( szFunction, sizeof( szFunction ) /sizeof( szFunction[0] ), NULL, NULL, 0, "%s_%d", szName, nIndex );

	return FindNPCFunction( szFunction );
	//FLCStyleScript
	//PFN pmf = GetFunction( szFunction );
	//if( pmf )
	//	return TRUE;
	//else
	//	return FALSE;


}

