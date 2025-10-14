#include "stdafx.h"

#ifdef __WORLDSERVER
#include "User.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define __SMC_COMPILER

#ifdef __WORLDSERVER
char	CScriptDialog::s_pszName[64];
char	CScriptDialog::s_pszKey[260];
char	CScriptDialog::s_pszBeforeRun[260];
char	CScriptDialog::s_pszAfterRun[260];
#endif	// __WORLDSERVER

CScriptDialog::CScriptDialog()
{
#ifdef __V050322_CACHEQUEST
	ZeroMemory( m_anGlobalVal, sizeof( m_anGlobalVal ) );
#endif
}
CScriptDialog::CScriptDialog(LPCTSTR lpszFileName)
{
#ifdef __V050322_CACHEQUEST
	ZeroMemory( m_anGlobalVal, sizeof( m_anGlobalVal ) );
#endif
	LoadScript(lpszFileName);
}
CScriptDialog::~CScriptDialog()
{
	Free();
}
void CScriptDialog::Free()
{
	LPSCRIPT lpScript;
	for(int i = 0; i < m_scriptAry.GetSize(); i++)
	{
		lpScript = (LPSCRIPT)m_scriptAry.GetAt(i);
		SAFE_DELETE( lpScript->m_pScript );
		SAFE_DELETE( lpScript );
#ifdef __CLIENT
		lpScript = (LPSCRIPT)m_scriptMarkAry.GetAt(i);
		SAFE_DELETE( lpScript );
#endif
	}
	m_scriptAry.RemoveAll();
#ifdef __CLIENT
	m_scriptMarkAry.RemoveAll();
#endif
}
BOOL CScriptDialog::IsEmpty()
{
	return m_scriptAry.GetSize() ? FALSE : TRUE;
}
BOOL CScriptDialog::InterpretScript( CScript& smc )
{
//	int nType;
	LPSTR pProg;
	TCHAR szSubKey[ 64 ];
 	//CString strSubKey;
	LPSCRIPT lpScript;
	LPSTR pScript;
	int nBlockCount;
	{
		int nTokenType = smc.GetToken(); // first word

		while(*smc.token != '}' && smc.tok != FINISHED )
		{	
			if( nTokenType != STRING )
			{
				CString string;
				string.Format( "%s(%d) : CScriptDialog::InterpretScript에서 스트링 형태가 아닌 Key(%s)가 발견되었습니다.", smc.m_szFileName, smc.GetLineNum(), smc.token );
				Error( string );
			}
			//strSubKey = smc.Token;
			_tcscpy( szSubKey, smc.token );
			pProg = smc.m_pProg;
			if(smc.IsCompilerMode())
				nBlockCount = 8; // "# " + "main" + "()"
			else
				nBlockCount = 6; // "main" + "()"
			int nBlock = -1;
			while( nBlock ) 
			{ 
				if( IsDBCSLeadByte( *pProg ) == FALSE )
				{
					if( *pProg == '"' ) 
					{
						do 
						{
							if( IsDBCSLeadByte( *pProg ) == FALSE )
							{
								nBlockCount++; pProg++; 
							}
							else
							{
								nBlockCount += 2; pProg += 2; 
							}

						} while( *pProg != '"' );
						nBlockCount++; pProg++; 
					}
					if( *pProg == '{' )	
					{ 
						if( nBlock == -1 ) 
							nBlock = 1; 
						else 
							nBlock++;	
					}
					if( *pProg == '}' )	
						nBlock--;
					nBlockCount++; pProg++; 
				}
				else
				{
					nBlockCount += 2; pProg += 2; 
				}
			}
			lpScript = new SCRIPT;
			ZeroMemory( lpScript, sizeof( SCRIPT ) );
			pScript = new CHAR[ nBlockCount + 10 ]; // 10은 그냥 여분 
			lpScript->m_pScript = pScript;
			//lpScript->m_strKey = strSubKey;
			_tcscpy( lpScript->m_szKey, szSubKey );
			
			TRACE( "%d\n", lstrlen( szSubKey ) );


			//lpScript->m_bStatus = FALSE;
			lpScript->m_bHidden = FALSE;
			m_scriptAry.Add( lpScript );
#ifdef __CLIENT
			LPSCRIPT lpScriptMark = new SCRIPT;
			memcpy( lpScriptMark, lpScript, sizeof( SCRIPT ) );
			m_scriptMarkAry.Add( lpScriptMark );
#endif			
			if( smc.IsCompilerMode() )
			{
				strcpy(pScript,"# main()");	
				memcpy(pScript+8,smc.m_pProg,nBlockCount-8);
			}
			else
			{
				strcpy(pScript,"main()");	
				memcpy(pScript+6,smc.m_pProg,nBlockCount-6);
			}
			*(pScript + nBlockCount) = 0;
			smc.m_pProg = pProg;
			nTokenType = smc.GetToken(); // next
		}
		smc.GetToken(); // next word
	}
	return TRUE;
}
#ifdef __CLIENT
void CScriptDialog::SetMark()
{
	for( int i = 0; i < m_scriptAry.GetSize(); i++ )
	{
		LPSCRIPT lpScriptSrc = (LPSCRIPT)m_scriptAry.GetAt( i );
		LPSCRIPT lpScriptDst = (LPSCRIPT)m_scriptMarkAry.GetAt( i );
		memcpy( lpScriptDst, lpScriptSrc, sizeof( SCRIPT ) );
	}
}
void CScriptDialog::GoMark()
{
	if( m_scriptMarkAry.GetSize() )
	{
		for( int i = 0; i < m_scriptMarkAry.GetSize(); i++ )
		{
			LPSCRIPT lpScriptSrc = (LPSCRIPT)m_scriptMarkAry.GetAt( i );
			LPSCRIPT lpScriptDst = (LPSCRIPT)m_scriptAry.GetAt( i );
			memcpy( lpScriptDst, lpScriptSrc, sizeof( SCRIPT ) );
		}
	}
}
#endif

BOOL CScriptDialog::LoadScript(LPCTSTR lpszFileName)
{
#ifdef __WORLDSERVER
	int nProcess = CScript::COMPILER;
#else
	int nProcess = CScript::INTERPRITER;
#endif
	CScript smc;
//	if( strstr( lpszFileName, "Tina.txt" ) )
//	{
//		int a;
//		a++;
//	}
	if( smc.Load( lpszFileName, TRUE, nProcess ) == TRUE )
	{
		strcpy( m_szFileName, lpszFileName );
		// 첫 키가 발견될 때까지 스킵하기.
		smc.SetErrorCheck( FALSE );
		while( smc.tokenType != STRING ) 
			smc.GetToken();
		smc.PutBack();
		smc.SetErrorCheck( TRUE );
		InterpretScript( smc );
		smc.Free();
	}
	struct _finddata_t c_file;
	long hFile;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpszFileName, drive, dir, name, ext );
	CString string;
	string.Format( "%s%s-*%s", dir, name, ext );
	/* Find first .c file in current directory */
	if( (hFile = _findfirst( string, &c_file )) != -1L )
	{
		//strcpy( name, dir );
		//strcat( name, c_file.name );
		if( !(c_file.attrib & _A_SUBDIR ) )//CFile::GetStatus( name, fileStatus) == TRUE )
		{
			string.Format( "%s%s", dir, c_file.name );
			if( smc.Load( string, TRUE, nProcess ) == FALSE )
				return FALSE;
			smc.SetErrorCheck( FALSE );
			while( smc.tokenType != STRING ) 
				smc.GetToken();
			smc.PutBack();
			smc.SetErrorCheck( TRUE );
			InterpretScript( smc );
			smc.Free();
		}
		/* Find the rest of the .c files */
		while( _findnext( hFile, &c_file ) == 0 )
		{
			//strcpy( name, dir );
			//strcat( name, c_file.name );
			if( !(c_file.attrib & _A_SUBDIR ) )//if( CFile::GetStatus( name, fileStatus) == TRUE )
			{
				string.Format( "%s%s", dir, c_file.name );
				if( smc.Load( string, TRUE, nProcess ) == FALSE )
					return FALSE;
				smc.SetErrorCheck( FALSE );
				while( smc.tokenType != STRING ) 
					smc.GetToken();
				smc.PutBack();
				smc.SetErrorCheck( TRUE );
				InterpretScript( smc );
				smc.Free();
			}
		}
		_findclose( hFile );
	}	
/*
	if(smc.Load(lpszFileName,TRUE,nProcess) == FALSE)
		return FALSE;
	do {
		smc.GetToken();
		if(smc.Token == "script")
			InterpretScript(smc);
	} while(smc.tok!=FINISHED);

*/
	return TRUE;
}
LPSCRIPT CScriptDialog::Lookup(LPCTSTR pKey)
{
	for(int i = 0; i < m_scriptAry.GetSize(); i++)
	{
		LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry.GetAt(i);
		if( _tcscmp( lpScript->m_szKey, pKey ) == 0 )
			return lpScript;
	}
	return NULL;
}
LPSCRIPT CScriptDialog::Lookup( int nId )
{
	return (LPSCRIPT)m_scriptAry.GetAt( nId );
}
#ifdef __V050322_CACHEQUEST
void __QuestBegin( CScript* pScript, int nQuestId );
void __QuestBeginYes( CScript* pScript, int nQuestId );
void __QuestBeginNo( CScript* pScript, int nQuestId );
void __QuestEnd( CScript* pScript );
//void __QuestEndYes( CScript* pScript, int nQuestId );
//void __QuestEndNo( CScript* pScript, int nQuestId );
//void __QuestEndCancel( CScript* pScript, int nQuestId );
//void __QuestBeginYes_Ok( CScript* pScript, int nQuestId );
//void __QuestBeginNo_Ok( CScript* pScript, int nQuestId );
void __QuestEndComplete( CScript* pScript, int nValue, int nQuestId );
#endif

#ifdef __V050322_CACHEQUEST
BOOL CScriptDialog::Run( LPCTSTR pKey, int* nResult, int nValue,int nDstId, int nSrcId, int nQuestId, int nCaller )
#else // __V050322_CACHEQUEST
//{{AFX
BOOL CScriptDialog::Run( LPCTSTR pKey, int* nResult, int nValue,int nSrcId, int nDstId, int nQuestId )
//}}AFX
#endif // __V050322_CACHEQUEST
{
#ifdef __V050322_CACHEQUEST
	CScript script;
	script.m_paTimer = m_aTimer;
	script.m_panGlobalVal = m_anGlobalVal;
	strcpy( script.m_szFileName, m_szFileName );
	script.SetIValue( nValue, nDstId, nSrcId, nCaller, int( this ), nQuestId, -1 );

	if( strcmp( pKey, "QUEST_END" ) == 0 )
	{
		__QuestEnd( &script );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_BEGIN" ) == 0 )
	{
		__QuestBegin(  &script, nQuestId );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_BEGIN_YES" ) == 0 )
	{
		__QuestBeginYes(  &script, nQuestId );
		return TRUE;
	}
	else
		/*
	if( strcmp( pKey, "QUEST_BEGIN_YES_OK" ) == 0 )
	{
		__QuestBeginYes_Ok(  &script, nQuestId );
		return TRUE;
	}
	else
*/
	if( strcmp( pKey, "QUEST_BEGIN_NO" ) == 0 )
	{
		__QuestBeginNo(  &script, nQuestId );
		return TRUE;
	}
	else
		/*
	if( strcmp( pKey, "QUEST_BEGIN_NO_OK" ) == 0 )
	{
		__QuestBeginNo_Ok(  &script, nQuestId );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_END_YES" ) == 0 )
	{
		__QuestEndYes(  &script, nQuestId );
		return TRUE;
	}
	else
	*/
	if( strcmp( pKey, "QUEST_END_COMPLETE" ) == 0 )
	{
		__QuestEndComplete(  &script, nValue, nQuestId );
		return TRUE;
	}
		/*

	else
	if( strcmp( pKey, "QUEST_END_NO" ) == 0 )
	{
		__QuestEndNo(  &script, nQuestId );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_END_CANCEL" ) == 0 )
	{
		__QuestEndCancel(  &script, nQuestId );
		return TRUE;
	}
	*/
#endif

	LPSCRIPT lpScript = Lookup( pKey );
	if( lpScript && lpScript->m_pScript )
	{
#ifdef __V050322_CACHEQUEST
		script.SetProg( lpScript->m_pScript );

		try
		{
			script.Run();
		}
		catch(...)
		{
			Error( "CScriptDialog::Run causes error in %s file\t// %s", script.m_szFileName, script.m_pProg );
		}

		if( nResult != NULL )
			*nResult = script.GetReturn();
#else // __V050322_CACHEQUEST
//{{AFX
		CScript script;
		strcpy( script.m_szFileName, m_szFileName );
		script.SetGlobal( nValue, nSrcId, nDstId, WES_DIALOG, int( this ), -1 );
		script.SetProg( lpScript->m_pScript );
		script.Run();

		if( nResult != NULL )
			*nResult = script.GetGlobal();

//}}AFX
#endif // __V050322_CACHEQUEST
		return TRUE;
	}
#ifdef _CLIENT
	else
	{
		if( CSMC::m_bWaitServerResponse == FALSE )
		{
			// ScriptDialog를 받기전에 다시 눌러지지 않게 한다.
			CSMC::m_bWaitServerResponse = TRUE;
		}
	}
#endif
	return FALSE;
}
/*
#ifdef __V050322_CACHEQUEST
BOOL CScriptDialog::Call( LPCTSTR pKey, int* nResult, int nGlobal1, int nGlobal2, int nGlobal3 )
{
	LPSCRIPT lpScript = Lookup( pKey );
	if( lpScript && lpScript->m_pScript )
	{
		strcpy( m_script.m_szFileName, m_szFileName );
		m_script.SetGlobal( nGlobal1, nGlobal2, nGlobal3, WES_DIALOG, int( this ), -1 );
		m_script.SetProg( lpScript->m_pScript );
		m_script.Run();
#ifdef _CLIENT
		// 서버에서 신호를 주기 전까지는 스크립트를 실행 못시키게 Lock을 건다.
		;//g_Trans.SendLockScript();
#endif
		if( nResult != NULL )
			*nResult = m_script.GetGlobal();
		return TRUE;
	}
#ifdef _CLIENT
	else
	{
		if( CSMC::m_bWaitServerResponse == FALSE )
		{
			// 클라이언트 경우에만 스크립트 포인터가 없다. 따라서 여기는 클라이언트만 실행됨.
;//			g_Trans.SendRequestScriptDialog(pKey);
			// ScriptDialog를 받기전에 다시 눌러지지 않게 한다.
			CSMC::m_bWaitServerResponse = TRUE;
		}
	}
#endif
	return FALSE;
}
BOOL CScriptDialog::Call( int nId, int* nResult, int nGlobal1, int nGlobal2, int nGlobal3 )
{
	LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry.GetAt(nId);
	return Call( lpScript->m_szKey, nResult, nGlobal1, nGlobal2, nGlobal3 );
}

#endif // __V050322_CACHEQUEST
*/
/*
BOOL CScriptDialog::RunKey( LPCTSTR pKey, int* nResult, int nGlobal1 )
{
	//CSMC smc;
	LPSCRIPT lpScript = Lookup( pKey );
	if( lpScript && lpScript->m_pScript )
	{
		//m_smc.SetGlobal( nGlobal1, nGlobal2, nGlobal3, WES_DIALOG, int( this ), -1 );
		m_smc.SetGlobal( nGlobal1 );
		m_smc.SetProg( lpScript->m_pScript );
		m_smc.Run();
		if( nResult != NULL )
			*nResult = m_smc.GetGlobal();
		return TRUE;
	}
	return FALSE;
}
*/
/*
BOOL CScriptDialog::Run(LPCTSTR pKey,int* nResult,CUIntArray& globalArray)
{
	CSMC smc;
	LPSCRIPT lpScript = Lookup(pKey);
	if(lpScript->m_pScript)
	{
		smc.SetGlobal(globalArray);
		smc.SetProg(lpScript->m_pScript);
		smc.Run();
		if(nResult != NULL)
			*nResult = smc.GetGlobal();
		return TRUE;
	}
#ifdef _CLIENT
	else
		// 클라이언트 경우에만 스크립트 포인터가 없다. 따라서 여기는 클라이언트만 실행됨.
		g_Trans.SendRequestScriptDialog(pKey);
#endif
	return FALSE;
}
*/
#ifdef __V050322_CACHEQUEST
BOOL CScriptDialog::Run( int nId, int* nResult, int nValue,int nDstId, int nSrcId, int nQuestId, int nCaller )
{
	LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry.GetAt(nId);
	return Run( lpScript->m_szKey, nResult, nValue, nDstId, nSrcId, nQuestId, nCaller );
}
#else // __V050322_CACHEQUEST
//{{AFX
BOOL CScriptDialog::Run( int nId, int* nResult, int nValue,int nSrcId, int nDstId, int nQuestId )
{
	LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry.GetAt(nId);
	return Run( lpScript->m_szKey, nResult, nValue, nSrcId, nDstId, nQuestId );
}
//}}AFX
#endif // __V050322_CACHEQUEST
/*
BOOL CScriptDialog::Run(int nId,int* nResult,CUIntArray& globalArray)
{
	LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry.GetAt(nId);
	return Run(lpScript->m_strKey,nResult,globalArray);
}
*/

void CScriptDialog::AddScriptButton( CWndDialog* pWndDialog,CObj* pObj )
{
	/*
#ifdef _CLIENT
	LPSCRIPT lpScript;
	int x, y;
	pFieldMessage->RemoveButtonMessage(pObj);
	CString string;
	CString strKey;
	for(int i = 0; i < m_scriptAry.GetSize(); i++)
	{
		lpScript = Lookup(i);
		if(lpScript->m_bStatus)
		{
			string += lpScript->m_strKey;
			string += " ";
		}
	}
	string = RemoveTag(string);
	CMemDC memDC;
	CSize sizeString = memDC.GetTextExtent(string);
	CSize size = sizeString;

	// 좌표 계산 
	if(sizeString.cx > 150)
		size.cx = 150;
	x = -(size.cx / 2);
	for(i = 0; i < m_scriptAry.GetSize(); i++)
	{
		lpScript = Lookup(i);
		if(lpScript->m_bStatus)
		{
			if(x > size.cx / 2)
			{
				x = -(size.cx / 2);
				size.cy += sizeString.cy + 1; 
			}
			strKey = RemoveTag(lpScript->m_strKey);
			CSize sizeWord = memDC.GetTextExtent(strKey);
			x += sizeWord.cx + 5; // 단어 길이 + 공백
		}
	}
	x = -(size.cx / 2);
	y = -size.cy;
	// 출력 
	for(i = 0; i < m_scriptAry.GetSize(); i++)
	{
		lpScript = Lookup(i);
		if(lpScript->m_bStatus)
		{
			if(x > size.cx / 2)
			{
				x = -(size.cx / 2);
				y += sizeString.cy + 1;
			}
			CString strTag, strResult;
			GetTag(lpScript->m_strKey,strTag,strResult);
			CSize sizeWord = memDC.GetTextExtent(strResult);
			CFieldMsg* pFieldMsg = pFieldMessage->AddTextButton(pObj,strResult,x,y,i);
			TagParser(pObj,pFieldMsg,strTag);
			x += sizeWord.cx + 5; // 단어 길이 + 공백
		}
	}
#endif
	*/
}
/*
void CScriptDialog::SetAllScript( BOOL bStatus )
{
	for(int i = 0; i < m_scriptAry.GetSize(); i++)
	{
		LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry[i];
		if( _tcschr( lpScript->m_szKey, '#' ) == NULL ) //lpScript->m_strKey.Find('#') == -1)
			lpScript->m_bStatus = bStatus;
	}
}
*/
// 1. 클라이언트가 스크립트를 실행를 서버로 실행 명령를 보낸다
// 2. 서버는 각 키 리스트와 상태 값을 클라이언트로 보낸다.
// 3. 클라이언트는 키 리스트가 도착하는 즉시 #init를 키로 해서 서버로 #init에 대한 스크립트를 요청한다.
// 4. 서버는 #init의 스크립트를 클라이언트로 보낸다.
// 5. 클라이언트에서는 #init 또는 특정 키의 스트립트를 받아서 인터프리트 한다.
