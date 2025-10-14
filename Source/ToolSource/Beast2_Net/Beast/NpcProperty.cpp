#include "stdafx.h"
#include "NpcProperty.h"

CNpcProperty::CNpcProperty()
{
	m_szDialogFile[0] = 0;
#ifdef __V050322_CACHEQUEST
	m_timerScript.Set( (float)( MIN( 1 ) + xRandom( SEC( 30 ) ) ) );
#else
	m_timerScript.Set( (float)( MIN( 1 ) + xRandom( SEC( 10 ) ) ) );
#endif
}

CNpcProperty::~CNpcProperty()
{
}
LPCTSTR	CNpcProperty::GetDlgName()
{
	return m_szDialogFile;
}
#ifdef __V050322_CACHEQUEST
BOOL CNpcProperty::IsTimeOut()
{
	return m_timerScript.IsTimeOut();
}
void CNpcProperty::ResetTimer()
{
	m_timerScript.Set( (float)( SEC( 15 ) + SEC( xRandom( 10 ) ) ) );
}
#endif
void CNpcProperty::SetDlgName( LPCTSTR szName )
{
	strncpy( m_szDialogFile, szName, 32 );
	m_szDialogFile[31] = 0;
}

BOOL CNpcProperty::IsDialogLoaded()
{
#ifdef __BEAST
	return TRUE;
#else
	return m_Dialog.IsEmpty() == FALSE;
#endif
}
#ifdef __V050322_CACHEQUEST
BOOL CNpcProperty::RunDialog( LPCTSTR pKey, int* nResult, int nValue, int nDstId, int nSrcId, int nQuestId )
{
#ifdef __BEAST
	return TRUE;
#else
	return m_Dialog.Run( pKey, nResult, nValue, nDstId, nSrcId, nQuestId );
#endif
}
#else
//{{AFX
BOOL CNpcProperty::RunDialog( LPCTSTR pKey, int* nResult, int nValue, int nDstId, int nSrcId, int nQuestId );
{
#ifdef __BEAST
	return TRUE;
#else
	return m_Dialog.Run( pKey, nResult, nValue, nGlobal2, nGlobal3, nQuestId );
#endif
}
//}}AFX
#endif
/*
#ifdef __V050322_CACHEQUEST
BOOL CNpcProperty::CallDialog( LPCTSTR pKey, int* nResult, int nGlobal1, int nGlobal2, int nGlobal3 )
{
	return m_Dialog.Call( pKey, nResult, nGlobal1, nGlobal2, nGlobal3 );
}
#endif
*/
BOOL CNpcProperty::LoadDialog( LPCHARACTER lpCharacter )
{
	char szDialog[64]; 
	char szDlgQuest[64]; 

	strcpy( szDialog  , m_szDialogFile );
	strcpy( szDlgQuest, m_szDialogFile );

	TCHAR* pDest = strchr( szDlgQuest, '.' );
	if( pDest )
	{
		szDlgQuest[ pDest - szDlgQuest ] = 0;
		strcat( szDlgQuest, "_q.txt" );
	}

	if( lpCharacter )
	{
		if( lpCharacter->m_szDialog[ 0 ] )
			strcpy( szDialog, lpCharacter->m_szDialog );
		if( lpCharacter->m_szDlgQuest[ 0 ] )
			strcpy( szDlgQuest, lpCharacter->m_szDlgQuest );
	}

	return TRUE;
}
