/***************************************
 * 내장 함수들.						   *
 * 원하는 기능을 이곳에 함수로 만든다. *
 ***************************************/

#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#ifdef __CLIENT
#include "AppDefine.h"
#endif

#include "party.h"
#ifdef __WORLDSERVER
#include "User.h"
extern	CUserMng	g_UserMng;
#include "company.h"
#include "worldmng.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpSrvr.h"
extern	CPartyMng	g_PartyMng;
extern	CCompanyMng		g_CompanyMng;
extern	CWorldMng	g_WorldMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPSrvr	g_DPSrvr;
#endif	// __WORLDSERVER
#ifdef __CLIENT
#include "WndQuest.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;
#endif	// __CLIENT

#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "guildquest.h"
#include "defineQuest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetToken      pScript->GetToken
#define PutBack       pScript->PutBack
#define Compute       pScript->Compute
#define FindEob       pScript->FindEob
#define InterpBlock   pScript->InterpBlock
#define SntxErr       pScript->SntxErr
#define prog          pScript->prog
#define token         pScript->token
#define tok           pScript->tok
#define answerInfo    pScript->answerInfo
#define answerCnt     pScript->answerCnt
#define tokenType     pScript->tokenType
#define callStack     pScript->callStack
#define functos       pScript->functos

#ifdef __V050322_CACHEQUEST
#define internalArray pScript->m_anInternalVal
#else
#define globalArray pScript->m_globalArray
#endif
//
// Func(1);를 가정했을 때 (와 1과 )을 읽는다.
//
int EvalExp( CScript *pScript )
{
	int v;
	GetToken(); // (
	Compute(&v);
	GetToken(); // )
	return v;
}
int EvalDefault(CScript *pScript,int nDefault)
{
	GetToken(); // , or ) or (
	int v2;
	if(*token == ',' || *token == '(')
		Compute(&v2);
	else
	{
		v2 = nDefault;
		PutBack();
	}
 return v2;
}
//
// Func(1);를 가정했을 때 (와 1을 읽는다.
//
void Eval1(CScript *pScript,int &v1)
{
	GetToken(); Compute(&v1);
}
//
// Func(1,2);를 가정했을 때 (와 1,2을 읽는다.
//
void Eval2(CScript *pScript,int &v1,int &v2)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
}
void Eval3(CScript *pScript,int &v1,int &v2,int &v3)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
}
void Eval4(CScript *pScript,int &v1,int &v2,int &v3,int &v4)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
}
void Eval5(CScript *pScript,int &v1,int &v2,int &v3,int &v4,int &v5)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
	GetToken(); Compute(&v5);
}

void Eval6(CScript *pScript,int &v1,int &v2,int &v3,int &v4,int &v5,int &v6)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
	GetToken(); Compute(&v5);
	GetToken(); Compute(&v6);
}
void Eval7(CScript *pScript,int &v1,int &v2,int &v3,int &v4,int &v5,int &v6,int &v7)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
	GetToken(); Compute(&v5);
	GetToken(); Compute(&v6);
	GetToken(); Compute(&v7);
}
void Get2Tkn( CScript *pScript )
{
	GetToken(); GetToken();    
}
/**********************
 * 사용자 정의 함수들 *
 **********************/

/////////////
//
// SayName(int nId,int nNameId,LPCTSTR lpszStr,...);
//
int SayName( CScript *pScript )
{
#ifndef __DBSERVER
	int array[32], v1,v2, i = 0;
	CString strToken;
	CString string;
	Eval2(pScript,v1,v2);
	GetToken(); // ,
	GetToken(); // lpszStr
	strToken = token;
	GetToken(); 
	while(*token != ')')
	{
		Compute(&array[i++]);
		GetToken();
	}
	string.FormatV(strToken,(va_list)array);
	CMover* pMover = prj.GetMover(v1);
	CMover* pMoverName = prj.GetMover(v2);
	if(pMover && pMover->GetType() == OT_MOVER && pMoverName && pMoverName->GetType() == OT_MOVER)
	{
		CString stringName;
		stringName = pMoverName->GetName( TRUE );
		stringName += "! ";
		stringName += string;
#ifdef __CLIENT
		pMover->DialogOut(stringName);
#endif
	}
#endif	// __DBSERVER
	return 1;
}
// 서버에서 클라이언트로 대화 메시지를 보낼 때 
// Speak(int nId,LPCTSTR lpszStr,...);
//
int Speak( CScript *pScript )
{
#ifndef __DBSERVER
	int array[32], v1, i = 0;
	CString strToken;
	CString string;
	Eval1(pScript,v1);
	GetToken(); // ,
	GetToken(); // lpszStr
	strToken = token;
	GetToken(); 
	while(*token != ')')
	{
		Compute(&array[i++]);
		GetToken();
	}
	string.FormatV(strToken,(va_list)array);

	CMover* pMover = prj.GetMover(v1);
	if(pMover && pMover->GetType() == OT_MOVER)
	{
#ifdef __WORLDSERVER
		g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)string );
#else
		pMover->DialogOut(string);
#endif
	}
#endif	// __DBSERVER
	return 1;
}
// 서버에서 클라이언트로 대화 메시지를 보낼 때 
// SpeakName(int nId,int nNameId,LPCTSTR lpszStr,...);
//
int SpeakName( CScript *pScript )
{
#ifndef __DBSERVER
	int array[32], v1,v2, i = 0;
	CString strToken;
	CString string;
	Eval2(pScript,v1,v2);
	GetToken(); // ,
	GetToken(); // lpszStr
	strToken = token;
	GetToken(); 
	while(*token != ')')
	{
		Compute(&array[i++]);
		GetToken();
	}
	string.FormatV(strToken,(va_list)array);
	CMover* pMover = prj.GetMover(v1);
	CMover* pMoverName = prj.GetMover(v2);
	if(pMover && pMover->GetType() == OT_MOVER && pMoverName && pMoverName->GetType() == OT_MOVER)
	{
		CString stringName;
		stringName = pMoverName->GetName( TRUE );
		stringName += "! ";
		stringName += string;
#ifdef __WORLDSERVER
		g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)stringName );
#else
		pMover->DialogOut(stringName);
#endif
	}
#endif	// __DBSERVER
	return 1;
}
////////////////////////////////////////////////////////////////////////////
// Set Script Key 

// void SetAllKey();
int SetAllKey( CScript* pScript )
{
	/*
#ifdef __CLIENT
	BOOL bStatus = EvalExp(pScript);
	CScriptDialog* pScriptDlg = (CScriptDialog*)globalArray[SMC_FUNC];
	pScriptDlg->SetAllScript(bStatus);
	CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->MakeKeyButton();
#endif
		*/
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __V050322_CACHEQUEST

void __CreateItem( DWORD dwIdMover, int v1, int v2 );	
void __AddGold( DWORD dwIdMover, int nGold )     ;
void __SetQuest( DWORD dwIdMover, int nQuest );
void __RemoveGold( DWORD dwIdMover, int nGold );
int __SetQuestState( DWORD dwIdMover, int nQuest, int nState );
int __SetQuestState( DWORD dwIdMover, int nQuest, int nState );
/*
#define SMC_VAL    0
#define SMC_DST_ID 1 // npc
#define SMC_SRC_ID 2 // pc
#define SMC_CALLER 3
#define SMC_FUNC   4
*/
int GetVal( CScript *pScript )
{
	Get2Tkn(pScript); return internalArray[SMC_VAL];
}
int GetSrcId( CScript *pScript )
{
	Get2Tkn(pScript); return internalArray[SMC_SRC_ID];
}
int NpcId( CScript *pScript )
{
	Get2Tkn(pScript); return internalArray[SMC_DST_ID];
}
int PcId( CScript *pScript )
{
	Get2Tkn(pScript); return internalArray[SMC_SRC_ID];
}
int GetDstId( CScript *pScript )
{
	Get2Tkn(pScript); return internalArray[SMC_DST_ID];
}
int GetCaller( CScript *pScript )
{
	Get2Tkn(pScript); return internalArray[SMC_CALLER];
}
int IsTalk( CScript *pScript )
{
	Get2Tkn(pScript);
	return internalArray[0] == -1;
}
int GetGValue_( CScript *pScript )
{
	int v = EvalExp(pScript);
	if( v < 0 || v > 15 )
	{
		Error( "GetGValue_에서 %d(0~15) 값 이상", v );
		v = 0;
	}
	return pScript->m_panGlobalVal[ v ];
}
int SetGValue( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	if( v1 < 0 || v1 > 15 )
	{
		Error( "GetGValue_에서 %d(0~15) 값 이상", v1 );
		v1 = 0;
	}
	pScript->m_panGlobalVal[ v1 ] = v2;
	return TRUE;
}
int AddGValue( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	if( v1 < 0 || v1 > 15 )
	{
		Error( "GetGValue_에서 %d(0~15) 값 이상", v1 );
		v1 = 0;
	}
	pScript->m_panGlobalVal[ v1 ] += v2;
	return TRUE;
}
int GetIValue( CScript *pScript )
{
	return internalArray[EvalExp(pScript)];
}
int SetIValue( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	internalArray[v1] = v2;
	return TRUE;
}
int AddIValue( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	internalArray[v1] += v2;
	return TRUE;
}
#ifdef __WORLDSERVER
CWorld* __GetWorld( CScript* pScript )
{
	CWorld* pWorld = NULL;
	if( pScript->GetCaller() == WES_DIALOG )
	{
		CUser* pUser = prj.GetUser( pScript->GetPcId() );
		pWorld = pUser->GetWorld();
	}
	else
	if( pScript->GetCaller() == WES_WORLD )
		pWorld = g_WorldMng.GetWorld( pScript->GetSrcId() );

	return pWorld;
}
#endif
// void AddKey( LPCTSTR lpWord, LPCTSTR lpKey, int nVal );
int __AddKey( CScript* pScript, LPCTSTR lpszWord, LPCTSTR lpszKey = "", DWORD dwParam = 0)
{
	CHAR szKey[ 128 ], szWord[ 128 ];

	strcpy( szWord, lpszWord );

	if( strlen( lpszKey ) == 0 ) 
		strcpy( szKey, szWord );
	else
		strcpy( szKey, lpszKey );

#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ADDKEY;
		lstrcpy( rsf.lpszVal1, szWord );
		lstrcpy( rsf.lpszVal2, szKey );
		rsf.dwVal1	= dwParam;
		//CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		rsf.dwVal2 = 0;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT

//	if(lpSCript)	lpSCript->m_bStatus		= TRUE;
//	GetToken();
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
	{ 
		CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		pWndDialog->AddKeyButton( szWord, szKey, dwParam, 0 );
//		pWndDialog->MakeKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int AddKey( CScript* pScript )
{
	DWORD dwParam = 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		GetToken(); // data
		if( i == 0 )
			strWord		= token;
		else
		if( i == 1 )
			strKey	= token;
		else
		if( i == 2 )
			dwParam		= atoi( token );
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	
		strKey	= strWord;
	CScriptDialog* pScriptDlg = (CScriptDialog*)internalArray[SMC_FUNC];
	LPSCRIPT lpScript = (LPSCRIPT)pScriptDlg->Lookup( strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {, }를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	__AddKey( pScript, strWord, strKey, dwParam );

	return 1;
}
int AddCondKey( CScript* pScript )
{
#ifdef __WORLDSERVER
	DWORD dwParam = 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		GetToken(); // data
		if( i == 0 )
			strWord		= token;
		else
		if( i == 1 )
			strKey	= token;
		else
		if( i == 2 )
			dwParam		= atoi( token );
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	
		strKey	= strWord;
	
	CScriptDialog* pScriptDlg = (CScriptDialog*)internalArray[SMC_FUNC];
	LPSCRIPT lpScript = (LPSCRIPT)pScriptDlg->Lookup( strKey );

	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {, }를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );

	for( i = 0; i < pUser->m_nQuestSize; i++ )
	{
		LPQUEST lpQuest = &pUser->m_aQuest[ i ];
		QuestProp* pQuestProp = pUser->m_aQuest[ i ].GetProp();
		if( pQuestProp )
		{
			if( strcmp( pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey ) == 0 )
			{
				if( strcmp( pQuestProp->m_szEndCondDlgAddKey, strKey ) == 0 )
				{
					__AddKey( pScript, strWord, strKey, dwParam );
					return 1;
				}
			}
		}
	}
#endif
	return 0;
}
// void AddAnswer( LPCTSTR lpWord, LPCTSTR lpKey,DWORD dwParam );
int __AddAnswer( CScript* pScript, LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam1, int nQuest )
{
	CHAR szKey[ 128 ], szWord[ 128 ];

	strcpy( szWord, lpszWord );

	if( strlen( lpszKey ) == 0 ) 
		strcpy( szKey, szWord );
	else
		strcpy( szKey, lpszKey );


#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ADDANSWER;
		lstrcpy( rsf.lpszVal1, szWord );
		lstrcpy( rsf.lpszVal2, szKey );
		rsf.dwVal1 = (DWORD)dwParam1;
		rsf.dwVal2 = nQuest;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
		#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
	{
		CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		pWndDialog->AddAnswerButton( szWord, szKey, dwParam1, nQuest );//  pMover->m_pNpcProperty->GetQuest() );
//		pWndDialog->MakeKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int AddAnswer( CScript* pScript )
{
	int nParam1	= 0;
	int nParam2	= 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		if( i == 0 )
		{
			GetToken();
			strWord	= token;
		}
		else
		if( i == 1 )
		{
			GetToken();
			if( tokenType == STRING )
				strKey = token;
			else
			{
				PutBack();
				i++;
			}
		}
		else
		if( i == 2 )
		{
			Compute( &nParam1 );
		}
		else
		if( i == 3 )
		{
			Compute( &nParam2 );
		}

		GetToken();	// ,
	}
	if( strKey.IsEmpty() )	
		strKey	= strWord;
	CScriptDialog* pScriptDlg = (CScriptDialog*)internalArray[SMC_FUNC];
	LPSCRIPT lpScript = (LPSCRIPT)pScriptDlg->Lookup( strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {, }를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	return __AddAnswer( pScript, strWord, strKey, nParam1, nParam2 );
}
int GetParam( CScript* pScript )
{
	int v1, v2;
	Eval2( pScript, v1, v2 );

	if( v1 < 0 || v1 > 3 )
	{
		v1 = 0;
		Error( "GetParam에서 Idx 값 범위 초과(0~3)" ); 
	}
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v2 );
	if( pQuestProp )
		return pQuestProp->m_nParam[ v1 ];
	return 0;
}
int __RemoveQuest( CScript* pScript, int nQuest )      
{
#ifdef __WORLDSERVER
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
	if( pQuestProp )
	{
		CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//		if( pUser )
//		{
			LPQUEST pQuest	= pUser->GetQuest( nQuest );

			if( pQuest && pQuest->m_nState < 14 ) 
				pUser->AddDefinedText( TID_EVE_CANCELQUEST, "\"%s\"", pQuestProp->m_szTitle );
			pUser->RemoveQuest( nQuest );
			pUser->AddRemoveQuest( nQuest );
			// 시작시 변신을 했으면 퀘스트 삭제시 변신 해제시킨다.
			if( pQuest && pQuestProp->m_nBeginSetDisguiseMoverIndex )
			{
				CUser* pUser = prj.GetUser( pScript->GetPcId() );
				pUser->Lock( theLineFile );
				pUser->NoDisguise( NULL );
				pUser->Unlock( theLineFile );
				g_UserMng.AddNoDisguise( pUser );
			}
//		}
	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
	if( pQuestProp )
	{
		LPQUEST pQuest	= g_pPlayer->GetQuest( nQuest );
		if( pQuest )
		{
			if( pQuest->m_nState < 14 ) 
			{
				CString string;
				string.Format( GETTEXT( TID_EVE_CANCELQUEST ), pQuestProp->m_szTitle );
				g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_CANCELQUEST ) );
			}
			g_pPlayer->RemoveQuest( nQuest );
			CWndQuest* pWndQuest	= (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
			if( pWndQuest )
				pWndQuest->Update();
		}
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int RemoveQuest( CScript* pScript )      
{
	int v1	= EvalExp( pScript );
	return __RemoveQuest( pScript, v1 );
}
int __RemoveAllKey( CScript* pScript )
{
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		{
			RunScriptFunc rsf;
			rsf.wFuncType		= FUNCTYPE_REMOVEALLKEY;
			pUser->AddRunScriptFunc( rsf );
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	{
		CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
		if( pWndDialog )
			pWndDialog->RemoveAllKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER

	return 1;
}
int RemoveAllKey(CScript* pScript)
{
	Get2Tkn( pScript );
	__RemoveAllKey( pScript );
	return 1;
}
// SayQuest( int nQuest, int nDialog )
int __SayQuest( CScript* pScript,int nQuestId, int nIdx )
{
	CString strToken;

	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
#if defined( __SINGLE ) || defined( __WORLDSERVER ) 
	if( pQuestProp && pQuestProp->m_apQuestDialog[ nIdx ] )
		strToken = pQuestProp->m_apQuestDialog[ nIdx ];
#endif
	if( strToken.IsEmpty() == TRUE )
		return FALSE;

#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );

//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_SAY;//QUEST;
		lstrcpy( rsf.lpszVal1, strToken );
		rsf.dwVal2 = nQuestId;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->Say( strToken, nQuestId );
//	GetToken();	// )
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int SayQuest( CScript* pScript )
{
	int v1, v2, i = 0;
	CString strToken = "No Dialog";
	Eval2( pScript, v1, v2 );
	GetToken(); // )

#ifdef __WORLDSERVER
	__SayQuest( pScript, v1, v2 );
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->Say( strToken, v2 );
//	GetToken();	// )
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int __RunQuest( CScript* pScript, int nQuestId )
{
#ifdef __WORLDSERVER
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		for( int  i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nBeginSetAddItemIdx[ i ] )
			{
				__CreateItem( pScript->GetPcId(), pQuestProp->m_nBeginSetAddItemIdx[ i ], pQuestProp->m_nBeginSetAddItemNum[ i ] );
			}
		}
		if( pQuestProp->m_nBeginSetAddGold )
		{
			__AddGold(  pScript->GetPcId(), pQuestProp->m_nBeginSetAddGold );
		}

		// 퀘스트 시작시 변신함 
		if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
		{
			CUser* pUser = prj.GetUser( pScript->GetPcId() );
			pUser->Lock( theLineFile );
			pUser->Disguise( NULL, pQuestProp->m_nBeginSetDisguiseMoverIndex );
			pUser->Unlock( theLineFile );
			g_UserMng.AddDisguise( pUser, pQuestProp->m_nBeginSetDisguiseMoverIndex );
		}
		__SetQuest( pScript->GetPcId(), nQuestId );
	}
#endif __WORLSERVER
	return 1;
}
int BeginQuest( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	return __RunQuest( pScript, v1 );
}
int __EndQuest( CScript* pScript, int nQuestId )
{
#ifdef __WORLDSERVER
	int nNum;
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		CUser* pUser = prj.GetUser( pScript->GetPcId() );

		// 보상 아이템 갯수 만큼, 인벤토리 여유가 있는가?
		int nItemNum = 0;
		for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
		{
			QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ];
#ifdef __V050524_QUEST
			if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == pUser->GetSex() )
#endif
			{
				if( pEndRewardItem->m_nType == 0 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == pUser->GetJob() )
					{
#ifdef __V050524_QUEST
						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp(  pEndRewardItem->m_nItemIdx );
							int nDiv = pEndRewardItem->m_nItemNum / pItemProp->dwPackMax;
							int nSur = pEndRewardItem->m_nItemNum % pItemProp->dwPackMax;
							if( nSur ) nDiv++;
							nItemNum += nDiv;
						}
#else
//{{AFX
						for( int j = 0; j < 4; j++ )
						{
							if( pEndRewardItem->m_anItemIdx[ j ] )
							{
								ItemProp* pItemProp = prj.GetItemProp(  pEndRewardItem->m_anItemIdx[ j ] );
								int nDiv = pEndRewardItem->m_anItemNum[ j ] / pItemProp->dwPackMax;
								int nSur = pEndRewardItem->m_anItemNum[ j ] % pItemProp->dwPackMax;
								if( nSur ) nDiv++;
								nItemNum += nDiv;
							}
						}
//}}AFX
#endif
					}
				}
				else
				if( pEndRewardItem->m_nType == 1 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pUser->GetItemNum( pEndRewardItem->m_nJobOrItem ) )
					{
#ifdef __V050524_QUEST
						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp(  pEndRewardItem->m_nItemIdx );
							int nDiv = pEndRewardItem->m_nItemNum / pItemProp->dwPackMax;
							int nSur = pEndRewardItem->m_nItemNum % pItemProp->dwPackMax;
							if( nSur ) nDiv++;
							nItemNum += nDiv;
						}
#else
//{{AFX
						for( int j = 0; j < 4; j++ )
						{
							if( pEndRewardItem->m_anItemIdx[ j ] )
							{
								ItemProp* pItemProp = prj.GetItemProp(  pEndRewardItem->m_anItemIdx[ j ] );
								int nDiv = pEndRewardItem->m_anItemNum[ j ] / pItemProp->dwPackMax;
								int nSur = pEndRewardItem->m_anItemNum[ j ] % pItemProp->dwPackMax;
								if( nSur ) nDiv++;
								nItemNum += nDiv;
							}
						}
//}}AFX
#endif
					}
				}
			}
		}
		if( nItemNum > pUser->m_Inventory.GetEmptyCount() ) 
		{
			pUser->AddDefinedText( TID_QUEST_NOINVENTORYSPACE ); // 인벤토리 공간이 없어서 퀘스트를 완료할 수 없습니다.
			RunScriptFunc rsf;
			rsf.wFuncType		= FUNCTYPE_EXIT;
			pUser->AddRunScriptFunc( rsf );
			return FALSE;
		}
		////////////////////////////
		// 보상 지급 
		////////////////////////////
		for( i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
		{
			QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ];
#ifdef __V050524_QUEST
			if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == pUser->GetSex() )
#endif
			{
				if( pEndRewardItem->m_nType == 0 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == pUser->GetJob() )
					{
#ifdef __V050524_QUEST
						if( pEndRewardItem->m_nItemIdx )
						{
							int nItemNum = pEndRewardItem->m_nItemNum;
							__CreateItem( pScript->GetPcId(),  pEndRewardItem->m_nItemIdx, nItemNum );
						}
#else
//{{AFX
						for( int j = 0; j < 4; j++ )
						{
							if( pEndRewardItem->m_anItemIdx[ j ] )
							{
								int nItemNum = pEndRewardItem->m_anItemNum[ j ];
								__CreateItem( pScript->GetPcId(),  pEndRewardItem->m_anItemIdx[ j ], nItemNum );
							}
						}
//}}AFX
#endif
					}
				}
				else
				if( pEndRewardItem->m_nType == 1 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pUser->GetItemNum( pEndRewardItem->m_nJobOrItem ) )
					{
#ifdef __V050524_QUEST
						if( pEndRewardItem->m_nItemIdx )
						{
							int nItemNum = pEndRewardItem->m_nItemNum;
							__CreateItem( pScript->GetPcId(),  pEndRewardItem->m_nItemIdx, nItemNum );
						}
#else
//{{AFX
						for( int j = 0; j < 4; j++ )
						{
							if( pEndRewardItem->m_anItemIdx[ j ] )
							{
								int nItemNum = pEndRewardItem->m_anItemNum[ j ];
								__CreateItem( pScript->GetPcId(),  pEndRewardItem->m_anItemIdx[ j ], nItemNum );
							}
						}
//}}AFX
#endif
					}
				}
			}
		}
		if(	pQuestProp->m_nEndRewardGoldMin )
		{
			nNum = pQuestProp->m_nEndRewardGoldMax - pQuestProp->m_nEndRewardGoldMin + 1; 
			int nGold = pQuestProp->m_nEndRewardGoldMin + xRandom( nNum ); 
			pUser->AddGold( nGold );
			pUser->AddGoldText( nGold );
		}
		if(	pQuestProp->m_nEndRewardExpMin )
		{
			nNum = pQuestProp->m_nEndRewardExpMax - pQuestProp->m_nEndRewardExpMin + 1; 
			int nExp = pQuestProp->m_nEndRewardExpMin + xRandom( nNum ); 
			if( pUser->AddExperience( nExp ) )
			{
				g_UserMng.AddSetLevel( pUser, pUser->GetLevel() );
				pUser->AddSetGrowthLearningPoint( pUser->m_nRemainGP, pUser->m_nRemainLP );
			}
			pUser->AddSetExperience( pUser->GetExp1(), pUser->GetExp2(), (WORD)pUser->m_nLevel );
			pUser->AddDefinedText( TID_GAME_REAPEXP );
		}
		if( pQuestProp->m_nEndRewardKarmaStyle )
		{
			if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
				pUser->ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, pQuestProp->m_nEndRewardKarmaPoint );
			else
			if( pQuestProp->m_nEndRewardKarmaStyle == 2 )
			{
				int nKarma = pUser->GetKarma();
				pUser->ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, nKarma + pQuestProp->m_nEndRewardKarmaPoint ); 
			}
		}
		// 시작시 변신을 했으면 종료시 변신 해제시킨다.
		if( pQuestProp->m_nBeginSetDisguiseMoverIndex )
		{
			CUser* pUser = prj.GetUser( pScript->GetPcId() );
			pUser->Lock( theLineFile );
			pUser->NoDisguise( NULL );
			pUser->Unlock( theLineFile );
			g_UserMng.AddNoDisguise( pUser );
		}
		////////////////////////////
		// 아이템 제거 
		////////////////////////////
		for( i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nEndRemoveItemIdx[ i ] )
				pUser->RemoveItemA( pQuestProp->m_nEndRemoveItemIdx[ i ], pQuestProp->m_nEndRemoveItemNum[ i ] );
		}
		if( pQuestProp->m_nEndRemoveGold )
			__RemoveGold( pScript->GetPcId(), pQuestProp->m_nEndRemoveGold );
		////////////////////////////
		// 퀘스트 제거 (완료,진행 모두 제거함)
		////////////////////////////
		for( i = 0; i < MAX_QUESTREMOVE; i++ )
		{
			if( pQuestProp->m_anEndRemoveQuest[ i ] )
			{
				__RemoveQuest( pScript, pQuestProp->m_anEndRemoveQuest[ i ] );
			}
		}
		// 퀘스트 종료 
		__SetQuestState( pScript->GetPcId(), nQuestId, QS_END );
	}
#endif // #ifdef __WORLDSERVER
	return 1;
}
int __IsEndQuestCondition( CMover* pMover, int nQuestId )
{
#ifdef __NPP_050308	
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->GetQuest( nQuestId );
		if( pCurQuest == NULL || pCurQuest->m_nState == QS_END ) 
			return 0;
		// 시간 제한 체크 1
		if( pQuestProp->m_nEndCondLimitTime == 0 || pCurQuest->m_wTime )
			nResult++;
		// 정찰 임무  2
		if( pQuestProp->m_dwEndCondPatrolWorld == 0 || pCurQuest->m_bPatrol )
			nResult++;
		// NPC 퇴치 갯수  3,4
		for( int i = 0; i < 2; i++ )
		{
			if( pQuestProp->m_nEndCondKillNPCIdx[ i ] == 0 || pCurQuest->m_nKillNPCNum[ i ] >= pQuestProp->m_nEndCondKillNPCNum[ i ] ) 
				nResult++;
		}
		// 스킬 5
		if( pQuestProp->m_nEndCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill( pQuestProp->m_nEndCondSkillIdx );
			if( lpSkill )
			{
				if( pMover->CheckSkill( pQuestProp->m_nEndCondSkillIdx ) && lpSkill->dwLevel >= pQuestProp->m_nEndCondSkillLvl )
					nResult++;
			}
		}
		// 카르마 6
		if( pQuestProp->m_nEndCondKarmaPoint == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nEndCondKarmaComp == 0 )
			{
				if( pMover->GetKarma() == pQuestProp->m_nEndCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nEndCondKarmaComp == -1 )
			{
				if( pMover->GetKarma() <= pQuestProp->m_nEndCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nEndCondKarmaComp == 1 )
			{
				if( pMover->GetKarma() >= pQuestProp->m_nEndCondKarmaPoint )
					nResult++;
			}
		}
		// 카오틱 7
		if( pQuestProp->m_nEndCondChaotic == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nEndCondChaotic == 1 && pMover->IsChaotic() )
				nResult++;
			else
			if( pQuestProp->m_nEndCondChaotic == 2 && pMover->IsChaotic() == FALSE )
				nResult++;
		}
		// 변신 8
		if( pQuestProp->m_nEndCondDisguiseMoverIndex == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nEndCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nEndCondDisguiseMoverIndex == pMover->GetIndex() )
				nResult++;
		}
		// 스테이트 체크 9 
		if( pQuestProp->m_nEndCondState == 0 )
		{
			for( i = QS_BEGIN + 1; i < QS_END; i++ ) // (1~13)
			{
				if( pQuestProp->m_questState[ i ] == NULL )
					break;
			}
			if( ( i - 1 ) == pCurQuest->m_nState )
				nResult++;
		}
		else
		if( pQuestProp->m_nEndCondState == pCurQuest->m_nState )
			nResult++;
		// 완료 퀘스트 체크 9,10,11,12,13,14 
		if( pQuestProp->m_nEndCondCompleteQuestOper == 0 ) // 
			nResult += 6;
		else
		if( pQuestProp->m_nEndCondCompleteQuestOper == 1 ) // or
		{
			for( i = 0; i < 6; i++ )
			{
				if( pMover->IsCompleteQuest( pQuestProp->m_nEndCondCompleteQuest[ i ] ) )
					break;
			}
			if( i != 6 )
				nResult += 6;
		}
		else
		if( pQuestProp->m_nEndCondCompleteQuestOper == 2 ) // and
		{
			for( i = 0; i < 6; i++ )
			{
				if( pQuestProp->m_nEndCondCompleteQuest[ i ] == 0 || pMover->IsCompleteQuest( pQuestProp->m_nEndCondCompleteQuest[ i ] ) )
					nResult++;
			}
		}
		else
		// 파티 여부 15 
		if( pQuestProp->m_nEndCondParty == 0 )
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
				
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			if( pQuestProp->m_nEndCondParty == 1 && bParty == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nEndCondParty == 2 && bParty == TRUE )
			{
				if( pQuestProp->m_nEndCondPartyLeader == 0 || ( pQuestProp->m_nEndCondPartyLeader - 1 ) == bLeader )
				{
					if( pQuestProp->m_nEndCondPartyNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nEndCondPartyNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nEndCondPartyNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nEndCondPartyNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nEndCondPartyNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nEndCondPartyNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nEndCondPartyNum )
							nResult++;
					}						
				}
			}
		}
		// 길드 여부 16 
		if( pQuestProp->m_nEndCondGuild == 0 )
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			/*
#ifdef __WORLDSERVER
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			*/
			if( pQuestProp->m_nEndCondGuild == 1 && bGuild == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nEndCondGuild == 2 && bGuild == TRUE )
			{
				if( pQuestProp->m_nEndCondGuildLeader == 0 || ( pQuestProp->m_nEndCondGuildLeader - 1 ) == bLeader )
				{
					if( pQuestProp->m_nEndCondGuildNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nEndCondGuildNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nEndCondGuildNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nEndCondGuildNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nEndCondGuildNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nEndCondGuildNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nEndCondGuildNum )
							nResult++;
					}
				}
			}
		}
#ifdef	__V050524_QUEST
		// 수집 아이템 갯수  16 + MAX_QUESTCONDITEM
		if( pQuestProp->m_nEndCondItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nEndCondItemNum )
			{
				QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];
				if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == pMover->GetSex() )
				{
					if( pEndCondItem->m_nType == 0 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pEndCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pEndCondItem->m_nItemIdx ) >= pEndCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pEndCondItem->m_nType == 1 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pEndCondItem->m_nJobOrItem ) )
						{
							if( pEndCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pEndCondItem->m_nItemIdx ) >= pEndCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		// 총합이 16 + MAX_QUESTCONDITEM면 퀘스트 조건 성립 
		if( nResult == 16 + MAX_QUESTCONDITEM ) 
		{
#ifdef __V050721_BUGFIX_QUESTTIMER
			// 모든 조건 달성 순간에 타이머 카운트 중지 시키기 
			// 이후에 인벤토리가 부족해서 퀘스트를 완료 못시키더라도 시간은 정지된다.
			if( pCurQuest->m_wTime )
				pCurQuest->m_wTime |= 0x8000;
#endif
			return 1;
		}
#else
//{{AFX
		// 수집 아이템 갯수  16 + ( MAX_JOB * 4 )
		if( pQuestProp->m_nEndCondItemNum == 0 )
			nResult += 4 * MAX_JOB;
		else
		for( i = 0; i < MAX_JOB; i++ )
		{
			if( i < pQuestProp->m_nEndCondItemNum )
			{
				QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];
				{
					if( pEndCondItem->m_nType == 0 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							for( int j = 0; j < 4; j++ )
							{
								if( pEndCondItem->m_anItemIdx[ j ] == 0 || pMover->GetItemNum( pEndCondItem->m_anItemIdx[ j ] ) >= pEndCondItem->m_anItemNum[ j ] ) 
									nResult++;
							}

						}
						else
							nResult += 4;
					}
					else
					if( pEndCondItem->m_nType == 1 )
					{
						if( pEndCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pEndCondItem->m_nJobOrItem ) )
						{
							for( int j = 0; j < 4; j++ )
							{
								if( pEndCondItem->m_anItemIdx[ j ] == 0 || pMover->GetItemNum( pEndCondItem->m_anItemIdx[ j ] ) >= pEndCondItem->m_anItemNum[ j ] ) 
									nResult++;
							}
						}
						else
							nResult += 4;
					}
				}
			}
			else
				nResult += 4;
		}
		// 총합이 16 + ( MAX_JOB * 4 )면 퀘스트 조건 성립 
		if( nResult == 16 + ( MAX_JOB * 4 ) ) 
			return 1;
//}}AFX
#endif
	}
#endif // NPP
	return 0;
}
int IsEndQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
#else
	CMover* pUser = prj.GetMover( pScript->GetPcId() );
#endif
	return __IsEndQuestCondition( pUser, v1 );
}
int __IsBeginQuestCondition( CMover* pMover, int nQuestId )
{
#ifdef __NPP_050308
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->GetQuest( nQuestId );
		BOOL bComplete = pMover->IsCompleteQuest( nQuestId );
		// 퀘스트가 존재하거나, 이미 완료된 적이 있는 퀘스트는 조건 성립 안됨. 반복하기 위해서는 완전히 제거하여 완료 배열에 없어여 한다.
		if( pCurQuest || bComplete ) 
			return 0;

		// 이전 퀘스트 1,2,3,4,5,6
		for( int i = 0; i < 6; i++ )
		{
			if( pQuestProp->m_anBeginCondPreviousQuest[ i ] == 0 ) 
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->GetQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				BOOL bPreComplete = pMover->IsCompleteQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				if( pQuestProp->m_nBeginCondPreviousQuestType == 0 )
				{
					if( pPreQuest || bPreComplete )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondPreviousQuestType == 1 )
				{
					if( pPreQuest == NULL && bPreComplete )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondPreviousQuestType == 2 )
				{
					if( pPreQuest && bPreComplete == FALSE )
						nResult++;
				}
			}
		}
		// 이전 퀘스트 7,8,9,10,11,12
		for( i = 0; i < 6; i++ )
		{
			if( pQuestProp->m_anBeginCondExclusiveQuest[ i ] == 0 ) 
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->GetQuest( pQuestProp->m_anBeginCondExclusiveQuest[ i ] );
				BOOL bPreComplete = pMover->IsCompleteQuest( pQuestProp->m_anBeginCondPreviousQuest[ i ] );
				if( pPreQuest == NULL && bPreComplete == FALSE )
					nResult++;
			}
		}
		// 직업 13
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pMover->GetJob() )
			{
				nResult++;
				break;
			}
		}
		// 레벨 14
		if( pQuestProp->m_nBeginCondLevelMin == 0 || ( pMover->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pMover->GetLevel() <= pQuestProp->m_nBeginCondLevelMax ) )
			nResult++;
		// 파티 여부 15
		if( pQuestProp->m_nBeginCondParty == 0 )
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
				
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			if( pQuestProp->m_nBeginCondParty == 1 && bParty == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondParty == 2 && bParty == TRUE )
			{
				if( pQuestProp->m_nBeginCondPartyLeader == -1 || pQuestProp->m_nBeginCondPartyLeader == bLeader )
				{
					if( pQuestProp->m_nBeginCondPartyNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nBeginCondPartyNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nBeginCondPartyNum )
							nResult++;
					}			
				}
			}
		}
		// 길드 여부 16
		if( pQuestProp->m_nBeginCondGuild == 0 )
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			/*
#ifdef __WORLDSERVER
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pMover->m_idparty );
			if( pParty )
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
			}
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
#else
			bParty = g_Party.IsMember( g_pPlayer->m_idPlayer );
			nSize = g_Party.GetSizeofMember();
#endif
			*/
			if( pQuestProp->m_nBeginCondGuild == 1 && bGuild == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondGuild == 2 && bGuild == TRUE )
			{
				if( pQuestProp->m_nBeginCondGuildLeader == -1 || pQuestProp->m_nBeginCondGuildLeader == bLeader )
				{
					if( pQuestProp->m_nBeginCondGuildNum == 0 )
						nResult++;
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == 0 )
					{
						if( nSize == pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == 1 )
					{
						if( nSize >= pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}	
					else
					if( pQuestProp->m_nBeginCondGuildNumComp == -1 )
					{
						if( nSize <= pQuestProp->m_nBeginCondGuildNum )
							nResult++;
					}			
				}
			}
		}
		// 성별 17
		if( pQuestProp->m_nBeginCondSex == -1 || pQuestProp->m_nBeginCondSex == pMover->GetSex() )
			nResult++;
		// 스킬 18
		if( pQuestProp->m_nBeginCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill( pQuestProp->m_nBeginCondSkillIdx );
			if( lpSkill )
			{
				if( pMover->CheckSkill( pQuestProp->m_nBeginCondSkillLvl ) && lpSkill->dwLevel >= pQuestProp->m_nBeginCondSkillLvl )
					nResult++;
			}
		}
		// 카르마 19
		if( pQuestProp->m_nBeginCondKarmaPoint == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondKarmaComp == 0 )
			{
				if( pMover->GetKarma() == pQuestProp->m_nBeginCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nBeginCondKarmaComp == -1 )
			{
				if( pMover->GetKarma() < pQuestProp->m_nBeginCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nBeginCondKarmaComp == 1 )
			{
				if( pMover->GetKarma() > pQuestProp->m_nBeginCondKarmaPoint )
					nResult++;
			}
		}
		// 카오틱 20
		if( pQuestProp->m_nBeginCondChaotic == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondChaotic == 1 && pMover->IsChaotic() )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondChaotic == 2 && pMover->IsChaotic() == FALSE )
				nResult++;
		}
		// 변신 21 
		if( pQuestProp->m_nBeginCondDisguiseMoverIndex == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondDisguiseMoverIndex == pMover->GetIndex() )
				nResult++;
		}
#ifdef	__V050524_QUEST
		// 수집 아이템 갯수  21 + MAX_QUESTCONDITEM
		if( pQuestProp->m_nBeginCondItemNum == 0 )
			nResult += MAX_QUESTCONDITEM;
		else
		for( int i = 0; i < MAX_QUESTCONDITEM; i++ )
		{
			if( i < pQuestProp->m_nBeginCondItemNum )
			{
				QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondItem[ i ];
				if( pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex() )
				{
					if( pBeginCondItem->m_nType == 0 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) >= pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
					else
					if( pBeginCondItem->m_nType == 1 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pBeginCondItem->m_nJobOrItem ) )
						{
							if( pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum( pBeginCondItem->m_nItemIdx ) >= pBeginCondItem->m_nItemNum ) 
								nResult++;
						}
						else
							nResult++;
					}
				}
			}
			else
				nResult++;
		}
		if( nResult == 21 + MAX_QUESTCONDITEM )
			return 1;
#else
//{{AFX
		// 수집 아이템 갯수  21 + ( MAX_JOB * 4 )
		if( pQuestProp->m_nBeginCondItemNum == 0 )
			nResult += 4 * MAX_JOB;
		else
		for( int i = 0; i < MAX_JOB; i++ )
		{
			if( i < pQuestProp->m_nBeginCondItemNum )
			{
				QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondItem[ i ];
				{
					if( pBeginCondItem->m_nType == 0 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob() )
						{
							for( int j = 0; j < 4; j++ )
							{
								if( pBeginCondItem->m_anItemIdx[ j ] == 0 || pMover->GetItemNum( pBeginCondItem->m_anItemIdx[ j ] ) >= pBeginCondItem->m_anItemNum[ j ] ) 
									nResult++;
							}
						}
						else
							nResult += 4;
					}
					else
					if( pBeginCondItem->m_nType == 1 )
					{
						if( pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum( pBeginCondItem->m_nJobOrItem ) )
						{
							for( int j = 0; j < 4; j++ )
							{
								if( pBeginCondItem->m_anItemIdx[ j ] == 0 || pMover->GetItemNum( pBeginCondItem->m_anItemIdx[ j ] ) >= pBeginCondItem->m_anItemNum[ j ] ) 
									nResult++;
							}
						}
						else
							nResult += 4;
					}
				}
			}
			else
				nResult += 4;
		}
		if( nResult == 21 + ( MAX_JOB * 4 ) )
			return 1;
//}}AFX
#endif
	}
#endif
	return 0;
}
int IsBeginQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
#else
	CMover* pUser = prj.GetMover( pScript->GetPcId() );
#endif
	return __IsBeginQuestCondition( pUser, v1 );
}
int EndQuest( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	return __EndQuest( pScript, v1 );
}

// void AddQuestKey( int nQuest_szTitle, LPCTSTR lpKey, int nParam );
int __AddQuestKey( CScript* pScript, int nQuestId, LPCTSTR lpKey, int nParam = 0 )
{
	CHAR szWord[ 128 ], szKey[ 128 ];
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
		strcpy( szWord, pQuestProp->m_szTitle );
	if( strlen( lpKey ) == 0 ) 
		strcpy( szKey, szWord );
	else
		strcpy( szKey, lpKey );
#ifdef __WORLDSERVER

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ADDKEY;
		lstrcpy( rsf.lpszVal1, szWord );
		lstrcpy( rsf.lpszVal2, szKey );
		rsf.dwVal1	= nParam;
		rsf.dwVal2 = nQuestId;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT

	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );

//	if(lpSCript)	lpSCript->m_bStatus		= TRUE;
//	GetToken();
	if( pWndDialog )
	{
		CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		pWndDialog->AddKeyButton( szWord, szKey, nParam,  nQuestId );
//		pWndDialog->MakeKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
// void AddQuestKey( int nQuest_szTitle, LPCTSTR lpKey, int nVal );
int AddQuestKey( CScript* pScript )
{
	int nParam = 0;
	int nQuest = -1;
	CString strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		if( i == 0 )
		{
			Compute( &nQuest );

		}
		if( i == 1 )
		{
			GetToken(); // data
			if( tokenType == STRING )
				strKey = token;
			else
			{
				PutBack();
				i++;
			}
		}
		if( i == 2 )
		{
			Compute( &nParam );
		}
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		if( pQuestProp )
			strKey = pQuestProp->m_szTitle;
	}
	// 테스트
	CScriptDialog* pScriptDlg	= (CScriptDialog*)internalArray[SMC_FUNC];
	LPSCRIPT lpScript	= (LPSCRIPT)pScriptDlg->Lookup( strKey );
	if( lpScript == NULL ) 
	{
		CString string;
		string.Format( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {, }를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		AfxMessageBox( string );
		return 1;
	}

	return __AddQuestKey( pScript, nQuest, strKey, nParam );
}
// SysMessage( int nLeft, int nTop, int nTight, int nBottom, LPCTSTR lpszMessage );
int SysMessage( CScript* pScript )
{
#ifdef __WORLDSERVER
	CRect rect;
	Eval4( pScript, (int&)rect.left, (int&)rect.top, (int&)rect.right, (int&)rect.bottom );
	GetToken(); // ,
	GetToken(); // string
	rect.NormalizeRect();
	g_UserMng.AddWorldMsg( &rect, token );
	GetToken(); // )
#endif
	return 1;
}
// BOOL CreateRespawn( int nRespawnId, int nMoverId, int x,int y,int z, int nLeft, int nTop, int nTight, int nBottom, int nNum, int nActAttackNum, int nTime );
int CreateRespawn( CScript* pScript )
{
#ifdef __WORLDSERVER
	int nRespawnId, nMoverIdx;
	int x, y, z, nNum, nActAttackNum, nTime;
	CRect rect;

	Eval2( pScript, nRespawnId, nMoverIdx );
	Eval3( pScript, x, y, z );
	Eval4( pScript, (int&)rect.left, (int&)rect.top, (int&)rect.right, (int&)rect.bottom );
	Eval3( pScript, nNum, nActAttackNum, nTime );
	GetToken(); // )

	CWorld* pWorld = __GetWorld( pScript );
	if( pWorld == NULL )
		return 0;

	DWORD dwData;
	if( pWorld->m_respawner.GetRespawnInfo( nRespawnId, RESPAWNTYPE_SCRIPT )  )
	{
		// 동일한 키가 들어있다. 제거 되기 전에는 안됨.
		return FALSE;
	}
	MoverProp* pMoverProp = prj.GetMoverProp( nMoverIdx );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		if( nNum > 30 ) nNum = 30;
		if( nNum < 1 ) nNum = 1;

		if( nActAttackNum > nNum ) nActAttackNum = nNum;
		if( nActAttackNum < 1 ) nActAttackNum = 0;

		if( nTime > 10800 ) nTime = 10800;
		if( nTime < 10 ) nTime = 10;
/*
		int nAllServer = scanner.GetNumber();
		if( nAllServer != 0 )
		{
			BOOL bFlying = FALSE;
			if( pMoverProp->dwFlying )
				bFlying = TRUE;
			g_DPCoreClient.SendSetMonsterRespawn( pUser->m_idPlayer, pMoverProp->dwID, dwNum, dwAttackNum, dwRect, dwTime, bFlying );
			return TRUE;
		}
*/
		g_WorldMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = pMoverProp->dwID;
		ri.m_nMaxcb = nNum;
		ri.m_cb = 0;
		ri.m_nMaxAttackNum = nActAttackNum;
		ri.m_nActiveAttackNum = 0;
		ri.m_fY = (FLOAT)y;
		if( pMoverProp->dwFlying != 0 )
			ri.m_vPos = D3DXVECTOR3( (FLOAT)x,(FLOAT)y,(FLOAT)z );
		else
			ri.m_vPos = D3DXVECTOR3( 0, 0, 0 );
		rect.NormalizeRect();
		ri.m_rect = rect;
		ri.m_uTime = nTime;
		ri.m_cbTime = 0;
		ri.m_nGMIndex = nRespawnId;

		//char chMessage[512] = {0,};
		pWorld->m_respawner.Add( ri, RESPAWNTYPE_SCRIPT );
		g_WorldMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1
//		sprintf( chMessage, "Add Respwan Monster : %s(%d/%d) Rect(%d, %d, %d, %d) Time : %d", 
//			pMoverProp->szName, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
//		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	return TRUE;
}  
// CreateMover( int nMoverId, int nMoverIdx, int x, int y, int z, BOOL bActAttack )      
int CreateMover( CScript* pScript )
{
#ifdef __WORLDSERVER
	int nMoverId, nMoverIdx;
	int x, y, z, nNum, nActAttack;

	Eval2( pScript, nMoverId, nMoverIdx );
	Eval3( pScript, x, y, z );
	Eval1( pScript, nActAttack );
	GetToken(); // )

	CWorld* pWorld = __GetWorld( pScript );
	if( pWorld == NULL )
		return 0;

	DWORD dwData;
	if( pWorld->m_mapCreateChar.Lookup( nMoverId, (void*&)dwData ) )
	{
		// 동일한 키가 들어있다. 제거되기 전에는 안됨.
		return FALSE;
	}
	MoverProp* pMoverProp = prj.GetMoverProp( nMoverIdx );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
		if( NULL == pMover )	return FALSE;	// ASSERT( pObj );
//			FILEOUT( "error2.txt", "a %s b %s c %d \n", strName, pMoverProp->szName, pMoverProp->dwID );
		D3DXVECTOR3 vPos( (FLOAT)x, (FLOAT)y, (FLOAT)z );
		pMover->SetPos( vPos );
		vPos.y -= 1.0f;
		pWorld->GetUnderHeight( vPos );
		pMover->InitMotion( pMover->m_dwMotion );
		pMover->UpdateLocalMatrix();
		if( nActAttack )
			pMover->m_bActiveAttack = nActAttack;
		pMover->AddItToGlobalId();
		pWorld->AddObj( pMover, FALSE );
		pWorld->m_mapCreateChar.SetAt( nMoverId, (void*)pMover->GetId() );
	}
#endif
	return 1;
}     
// CreateChar( int nMoverId, int nMoverIdx, LPCTSTR lpszCharKey, int x,int y,int z )
int CreateChar( CScript* pScript )
{
#ifdef __WORLDSERVER
	int nMoverId, nMoverIdx;
	int x, y, z, nNum, nActAttack;
	CHAR szKey[ 64 ];

	Eval2( pScript, nMoverId, nMoverIdx );
	GetToken(); // ,
	GetToken();
	strcpy( szKey, token );
	Eval3( pScript, x, y, z );
	GetToken(); // )

	CWorld* pWorld = __GetWorld( pScript );
	if( pWorld == NULL )
		return 0;

	DWORD dwData;
	if( pWorld->m_mapCreateChar.Lookup( nMoverId, (void*&)dwData ) )
	{
		// 동일한 키가 들어있다. 제거되기 전에는 안됨.
		return FALSE;
	}
	//CWorld* pWorld = g_WorldMng.GetWorld( globalArray[1] );
	MoverProp* pMoverProp = prj.GetMoverProp( nMoverIdx );
	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
		if( NULL == pMover ) return FALSE; // ASSERT( pObj );
		strcpy( pMover->m_szCharacterKey, szKey );
		pMover->InitNPCProperty();
		pMover->InitCharacter( pMover->GetCharacter() );
		D3DXVECTOR3 vPos( (FLOAT)x, (FLOAT)y, (FLOAT)z );
		pMover->SetPos( vPos );
		vPos.y -= 1.0f;
		pWorld->GetUnderHeight( vPos );
		pMover->InitMotion( pMover->m_dwMotion );
		pMover->UpdateLocalMatrix();
		pMover->AddItToGlobalId();
		pWorld->AddObj( pMover, FALSE );
		pWorld->m_mapCreateChar.SetAt( nMoverId, (void*)pMover->GetId() );
	}
#endif	// __WORLDSERVER
	return TRUE;
}    
// RemoveMover( int nMoverId );
int RemoveMover( CScript* pScript )
{
	BOOL bResult = FALSE;
#ifdef __WORLDSERVER
	int nMoverId = EvalExp( pScript );

	CWorld* pWorld = __GetWorld( pScript );
	if( pWorld == NULL )
		return 0;

	OBJID idMover;
	if( pWorld->m_mapCreateChar.Lookup( nMoverId, (void*&)idMover ) )
	{
	//	CMclAutoLock	Lock( prj.m_AddRemoveLock );	// lock
		prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
		CMover* pMover	= prj.GetMover( idMover );
		if( IsValidObj( (CObj*)pMover ) )
		{
			if( pMover->IsNPC() )
			{
				pMover->Delete();
				bResult = TRUE;
			}
		}
		prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
		pWorld->m_mapCreateChar.RemoveKey( nMoverId );
	}
#endif
	return bResult;
}   
int RemoveRespawn( CScript* pScript )
{
	BOOL bResult = FALSE;
#ifdef __WORLDSERVER
	int nRespawnId = EvalExp( pScript );

	CWorld* pWorld = __GetWorld( pScript );
	if( pWorld == NULL )
		return 0;

	if( pWorld->m_respawner.GetRespawnInfo( nRespawnId, RESPAWNTYPE_SCRIPT )  )
	{
		g_WorldMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
		if( pWorld->m_respawner.Remove( nRespawnId, RESPAWNTYPE_SCRIPT ) )
			bResult = TRUE;
		g_WorldMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1	
	}
#endif	// __WORLDSERVER
	return bResult;
} 
// Disguise();
BOOL NoDisguise( CScript* pScript )     
{ 
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	pUser->Lock( theLineFile );
	pUser->NoDisguise( NULL );
	pUser->Unlock( theLineFile );
	g_UserMng.AddNoDisguise( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
// Disguise( int nMoverIndex );
int Disguise( CScript* pScript )
{ 
#ifdef __WORLDSERVER
	int nMoverIndex = EvalExp( pScript );
	MoverProp* pMoverProp = prj.GetMoverProp( nMoverIndex );
	if( pMoverProp )
	{
		CUser* pUser = prj.GetUser( pScript->GetPcId() );
		pUser->Lock( theLineFile );
		pUser->Disguise( NULL, nMoverIndex );
		pUser->Unlock( theLineFile );
		g_UserMng.AddDisguise( pUser, nMoverIndex );
	}
#endif
	return TRUE;
}
int GetDay( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_GameTimer.m_nDay;
}        
int GetMin( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_GameTimer.m_nMin;
}      
int GetHour( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_GameTimer.m_nHour;
}       
int GetRealSec( CScript* pScript )
{
	Get2Tkn( pScript );
	CTime time;
	return time.GetSecond();
}      
int GetRealMin( CScript* pScript )
{
	Get2Tkn( pScript );
	CTime time;
	return time.GetMinute();
}      
int GetRealHour( CScript* pScript )
{
	Get2Tkn( pScript );
	CTime time;
	return time.GetHour();
}       
int GetRealDay( CScript* pScript )
{
	Get2Tkn( pScript );
	CTime time;
	return time.GetDay();
}        
int GetRealMonth( CScript* pScript )
{
	Get2Tkn( pScript );
	CTime time;
	return time.GetMonth();
}        
int GetRealYear( CScript* pScript )
{
	Get2Tkn( pScript );
	CTime time;
	return time.GetYear();
}       
int SetTimer( CScript* pScript )  
{
	int nTimerId, nSec;
	Eval2( pScript, nTimerId, nSec );
	GetToken();
	if( nTimerId >= MAX_SCRIPT_TIMER )
	{
		Error( "Script : SetTimer에서 타이머 ID Max 초과 ( %d / %d )", nTimerId, MAX_SCRIPT_TIMER  );
		nTimerId = 0 ;
	}
	pScript->m_paTimer[ nTimerId ].Set( SEC( nSec ) );
	return 1;
}
int IsTimeOut( CScript* pScript )
{
	int nTimerId = EvalExp( pScript );
	if( nTimerId >= MAX_SCRIPT_TIMER )
	{
		Error( "Script : IsTimeOut 타이머 ID Max 초과 ( %d / %d )", nTimerId, MAX_SCRIPT_TIMER  );
		nTimerId = 0 ;
	}
	if( pScript->m_paTimer[ nTimerId ].IsTimeOut() )
	{
		pScript->m_paTimer[ nTimerId ].Reset();
		return TRUE;
	}
	return FALSE;
}
void __QuestBegin( CScript* pScript, int nQuestId )
{
	__SayQuest( pScript, nQuestId, QSAY_BEGIN1 );
	__SayQuest( pScript, nQuestId, QSAY_BEGIN2 );
	__SayQuest( pScript, nQuestId, QSAY_BEGIN3 );
	__SayQuest( pScript, nQuestId, QSAY_BEGIN4 );
	__SayQuest( pScript, nQuestId, QSAY_BEGIN5 );
	__AddAnswer( pScript, "__YES__", "QUEST_BEGIN_YES", 0, nQuestId );
	__AddAnswer( pScript, "__NO__", "QUEST_BEGIN_NO", 0, nQuestId  );
	__RemoveAllKey( pScript );
	// 기타 대화를 위한 키를 추가한다.
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	pMover->m_pNpcProperty->RunDialog( "#addKey", NULL, 0, pScript->GetNpcId(), pScript->GetPcId(), 0 );
	pMover->m_pNpcProperty->RunDialog( "#questBegin", NULL, 0, pScript->GetNpcId(), pScript->GetPcId(), nQuestId );
}
void __QuestEnd( CScript* pScript )
{
#ifdef __WORLDSERVER
	// 현재 진행 중인 퀘스트 중에서 현재 NPC가 처리해야될 퀘스트를 실행(자신일 수도 있음)
	int nResult = 0;//_RunNextMyQuest( pUser );

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );

	LPQUEST lpQuest;

	__RemoveAllKey( pScript );

	if( pScript->m_panGlobalVal[ 15 ] < pUser->m_nQuestSize )
	{
		LPCHARACTER lpChar = pMover->GetCharacter();

		for( ; pScript->m_panGlobalVal[ 15 ] < pUser->m_nQuestSize;  )
		{
			lpQuest = &pUser->m_aQuest[ pScript->m_panGlobalVal[ 15 ] ];
			pScript->m_panGlobalVal[ 15 ]++;
			if( lpQuest )
			{
				QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( pQuestProp )
				{
					// 대화하고 있는 캐릭터와 현재 실행 중인 퀘스트가 같으면 그놈이 종결 조건을 판정하는 놈이다.
					if( strcmpi( pQuestProp->m_szEndCondCharacter, pMover->m_szCharacterKey ) == 0 )
					{
						if( lpQuest && lpQuest->m_nState != QS_END ) 
						{
							if( __IsEndQuestCondition( pUser, lpQuest->m_wId ) )
							{
								__SayQuest( pScript, lpQuest->m_wId, QSAY_END_COMPLETE1 );
								__SayQuest( pScript, lpQuest->m_wId, QSAY_END_COMPLETE2 );
								__SayQuest( pScript, lpQuest->m_wId, QSAY_END_COMPLETE3 );
								__AddAnswer( pScript,"__OK__", "QUEST_END_COMPLETE", 0, lpQuest->m_wId );
								
							}
							else
							{
								__SayQuest( pScript, lpQuest->m_wId, QSAY_END_FAILURE1 );
								__SayQuest( pScript, lpQuest->m_wId, QSAY_END_FAILURE2 );
								__SayQuest( pScript, lpQuest->m_wId, QSAY_END_FAILURE3 );
								__AddAnswer( pScript,"__OK__", "QUEST_END", 0, lpQuest->m_wId );
							}
							nResult = TRUE;
							break;
						}
					}
				}
				else WriteError( "__QuestEnd : pQuestProp(%d) NULL이다.", lpQuest->m_wId );
			}
			else WriteError( "__QuestEnd : lpQuest(%d)가 NULL이다.", lpQuest->m_wId  );
		}
	}
	if( nResult == 0 )
	{
		LPCHARACTER lpChar = prj.GetCharacter( pMover->m_szCharacterKey );
		int anQuest[ 32 ];

		for( int i = 0; i < lpChar->m_awSrcQuest.GetSize(); i++ )
		{
			int nQuest = lpChar->m_awSrcQuest.GetAt( i );
			lpQuest = pUser->GetQuest( nQuest );

			// 퀘스트 시작 
			if( __IsBeginQuestCondition( pUser, nQuest ) == TRUE && lpQuest == NULL && pUser->IsCompleteQuest( nQuest ) == FALSE )
			{
				anQuest[ nResult ] = nQuest;
				nResult++;
			}
		}
		// 가능한 퀘스트가 하나일 때는 바로 퀘스트 시작 
		if( nResult == 1 )
			__QuestBegin( pScript, anQuest[ 0 ] );
		else
		{
			// 여러개일 경우는 키를 추가 
			for( i = 0; i < nResult; i++ )
				__AddQuestKey( pScript, anQuest[ i ], "QUEST_BEGIN" );
			if ( nResult == 0 )  
				// 준비된 퀘스트가 없을 때의 인사말 
				pMover->m_pNpcProperty->RunDialog( "#noQuest", &nResult, 0, pScript->GetNpcId(), pScript->GetPcId(), 0 );
			else
				// 준비된 퀘스트가 있을 때의 인사말 
				pMover->m_pNpcProperty->RunDialog( "#yesQuest", &nResult, 0, pScript->GetNpcId(), pScript->GetPcId(), 0 );
		}
	}
	// 기타 대화를 위한 키를 추가한다.
	pMover->m_pNpcProperty->RunDialog( "#addKey", NULL, 0, pScript->GetNpcId(), pScript->GetPcId(), 0 );
#endif
}
void __QuestBeginYes( CScript* pScript, int nQuestId )
{
	__SayQuest( pScript, nQuestId, QSAY_BEGIN_YES );
	__RunQuest( pScript, nQuestId ); // 퀘스트 시작 
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	pMover->m_pNpcProperty->RunDialog( "#questBeginYes", NULL, 0, pScript->GetNpcId(), pScript->GetPcId(), nQuestId );

}
void __QuestBeginNo( CScript* pScript, int nQuestId )
{
	__SayQuest( pScript, nQuestId, QSAY_BEGIN_NO );
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	pMover->m_pNpcProperty->RunDialog( "#questBeginNo", NULL, 0, pScript->GetNpcId(), pScript->GetPcId(), nQuestId );
}
void __QuestEndComplete( CScript* pScript, int nVal, int nQuestId )
{
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	if( __EndQuest( pScript, nQuestId ) == TRUE )
		pMover->m_pNpcProperty->RunDialog( "#questEndComplete", NULL, 0, pScript->GetNpcId(), pScript->GetPcId(), nQuestId );
	__QuestEnd( pScript );
}
int LaunchQuest( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	pScript->m_panGlobalVal[ 15 ] = 0;
	__QuestEnd( pScript );
#endif
	return 1;
}
int GetInventoryNum( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return pUser->m_Inventory.GetCount();
#endif
	return 1;
}     
int GetEmptyInventoryNum( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return pUser->m_Inventory.GetEmptyCount();
#endif
	return 1;
}
int IsQuestPatrol( CScript* pScript )
{
#ifdef __WORLDSERVER
	int nQuest	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	LPQUEST lpQuest = pUser->GetQuest( nQuest );
	if( lpQuest == NULL )
	{
		Error( "IsQuestPatrol에서 NULL QUEST" );
		return 0;
	}
	return lpQuest->m_bPatrol;
#endif
	return 1;
}
int IsQuestDialog( CScript* pScript )
{
#ifdef __WORLDSERVER
	int nQuest	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	LPQUEST lpQuest = pUser->GetQuest( nQuest );
	if( lpQuest == NULL )
	{
		Error( "IsQuestDialog에서 NULL QUEST" );
		return 0;
	}
	return lpQuest->m_bDialog;
#endif
	return 1;
}        
int IsQuestKillNPC( CScript* pScript )
{
#ifdef __WORLDSERVER // VHOME
	int nQuest, nIdx;
	Eval2( pScript, nQuest, nIdx );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	LPQUEST lpQuest = pUser->GetQuest( nQuest );
	if( lpQuest == NULL )
	{
		Error( "IsQuestKillNPC에서 NULL QUEST" );
		return 0;
	}
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
	return lpQuest->m_nKillNPCNum[ nIdx ] >= pQuestProp->m_nEndCondKillNPCNum[ nIdx ];
#endif
	return 1;
}       
int GetQuestTime( CScript* pScript )
{
#ifdef __WORLDSERVER
	int nQuest	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	LPQUEST lpQuest = pUser->GetQuest( nQuest );
	if( lpQuest == NULL )
	{
		Error( "GetQuestTime에서  NULL QUEST" );
		return 0;
	}
	return lpQuest->m_wTime & 0x7fff;
#endif
	return 1;
}         
int IsDisguise( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return pUser->IsDisguise();
#endif
	return 1;
}
int GetMoverIndex( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return pUser->GetIndex();
#endif
	return 1;
}
int GetMoverMotion( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return pUser->GetMotion();
#endif
	return 1;
}
int GetQuestState( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1 = EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		LPQUEST pQuest = pUser->GetQuest( v1 );
		if( pQuest ) 
			return pQuest->m_nState;
		QUEST quest;
		if( pUser->MakeCompleteQuest( v1, &quest ) )
			return quest.m_nState;
//	}
	return -1;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	LPQUEST pQuest	= g_pPlayer->GetQuest( v1 );
	if( pQuest )	
		return pQuest->m_nState;
	QUEST quest;
	if( g_pPlayer->MakeCompleteQuest( v1, &quest ) )
		return quest.m_nState;
	return -1;
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int IsSetQuest( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1 = EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		LPQUEST pQuest = pUser->GetQuest( v1 );
		if( pQuest ) return TRUE;
		if( pUser->IsCompleteQuest( v1 ) )
			return TRUE;
//	}
	return FALSE;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	LPQUEST pQuest	= g_pPlayer->GetQuest( v1 );
	if( pQuest )	return TRUE;
	if( g_pPlayer->IsCompleteQuest( v1 ) )
		return TRUE;
	return FALSE;
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int	IsExecuteQuest( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( pUser->m_nQuestSize )
		return TRUE;
#endif	// __WORLDSERVER
	return 0;
}
int __SetQuestState( DWORD dwIdMover, int nQuest, int nState )
{
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( dwIdMover );
//	if( pUser )
//	{
		if( nState == QS_END )
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
			if( pQuestProp )
				pUser->AddDefinedText( TID_EVE_ENDQUEST, "\"%s\"", pQuestProp->m_szTitle );

			g_dpDBClient.SendLogQuest( pUser, 2, nQuest );
		}
		else
		if( nState == QS_BEGIN )
		{
			g_dpDBClient.SendLogQuest( pUser, 1, nQuest );
		}
		QUEST quest;
		if( pUser->SetQuest( nQuest, nState, &quest ) )
			pUser->AddSetQuest( &quest );
		// #dlvr
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	if( nState == QS_END )
	{
		QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
		if( pQuestProp )
		{
			CString string;
			string.Format( GETTEXT( TID_EVE_ENDQUEST ), pQuestProp->m_szTitle );
			g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_ENDQUEST ) );
		}
	}
	QUEST quest;
	g_pPlayer->SetQuest( nQuest, nState, &quest );
	CWndQuest* pWndQuest	= (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
	if( pWndQuest )
		pWndQuest->Update(); 
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

void __SetQuest( DWORD dwIdMover, int nQuest )
{
#ifdef __WORLDSERVER
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
	CUser* pUser	= prj.GetUser( dwIdMover );
//	if( pUser )
//	{
		if( pQuestProp )
			pUser->AddDefinedText( TID_EVE_STARTQUEST, "\"%s\"", pQuestProp->m_szTitle );
		QUEST quest;
		if( pUser->SetQuest( nQuest, 0, &quest ) )
			pUser->AddSetQuest( &quest );
		g_dpDBClient.SendLogQuest( pUser, 1, nQuest );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	QUEST quest;
	g_pPlayer->SetQuest( nQuest, 0, &quest );
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
	if( pQuestProp )
	{
		CString string;
		string.Format( GETTEXT( TID_EVE_STARTQUEST ), pQuestProp->m_szTitle );
		g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_STARTQUEST ) );
	}
	CWndQuest* pWndQuest	= (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
	if( pWndQuest )
		pWndQuest->Update(); 
#	endif // __CLIENT
#endif // __WORLDSERVER
}
void __CreateItem( DWORD dwIdMover, int v1, int v2 )	
{
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( dwIdMover );
//	if( pUser )
//	{
		ItemProp* pItemProp		= prj.GetItemProp( v1 );
		if( pItemProp )
			pUser->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
		CItemElem itemElem;
		itemElem.m_dwItemId		= v1;
		itemElem.m_nItemNum		= v2;
		if( pUser->CreateItem( &itemElem ) )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "Q";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "QUEST";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
		}
//	}
#else // __WORLDSERVER
#	ifdef __CLIENT
	ItemProp* pItemProp		= prj.GetItemProp( v1 );
	if( pItemProp )
	{
		CString string;
		string.Format( GETTEXT( TID_EVE_REAPITEM ), pItemProp->szName );
		g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_REAPITEM ) );
	}
	CItemElem itemElem;
	itemElem.m_dwItemId		= v1;
	itemElem.m_nItemNum		= v2;
	g_pPlayer->AddItem( &itemElem );
#	endif // __CLIENT
#endif // __WORLDSERVER
}
#else // __V050322_CACHEQUEST
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//{{AFX
/*
#define SMC_VAL    0
#define SMC_SRC_ID 1 // npc
#define SMC_DST_ID 2 // pc
#define SMC_CALLER 3
#define SMC_FUNC   4
*/
int GetVal( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[SMC_VAL];
}
int GetSrcId( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[SMC_SRC_ID];
}
int NpcId( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[SMC_SRC_ID];
}
int PcId( CScript *pScript )
{
	//Get2Tkn(pScript); return globalArray[SMC_SRC_ID];
	Get2Tkn(pScript); return globalArray[SMC_DST_ID];
}
int GetDstId( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[SMC_DST_ID];
}
int GetCaller( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[SMC_CALLER];
}
/*
 * int GetGlobal0123();
 */
int GetGlobal0( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[0];
}
int GetGlobal1( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[1];
}
int GetGlobal2( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[2];
}
int GetGlobal3( CScript *pScript )
{
	Get2Tkn(pScript); return globalArray[3];
}
//
//int SetGlobal1(int v);
 //
int SetGlobal0( CScript *pScript )
{
	globalArray[0] = EvalExp(pScript); return 0;
}
int SetGlobal1( CScript *pScript )
{
	globalArray[1] = EvalExp(pScript); return 0;
}
int SetGlobal2( CScript *pScript )
{
	globalArray[2] = EvalExp(pScript); return 0;
}
int SetGlobal3( CScript *pScript )
{
	globalArray[3] = EvalExp(pScript); return 0;
}
int GetGlobal( CScript *pScript )
{
	return globalArray[EvalExp(pScript)];
}
int SetGlobal( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	globalArray[v1] = v2;
	return TRUE;
}
int AddGlobal( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	globalArray[v1] += v2;
	return TRUE;
}
int IsTalk( CScript *pScript )
{
	Get2Tkn(pScript);
	return globalArray[0] == -1;
}
// void AddKey( LPCTSTR lpWord, LPCTSTR lpKey, int nVal );
int AddKey( CScript* pScript )
{
#ifdef __WORLDSERVER
	DWORD dwParam = 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		GetToken(); // data
		if( i == 0 )
			strWord		= token;
		else
		if( i == 1 )
			strKey	= token;
		else
		if( i == 2 )
			dwParam		= atoi( token );
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	strKey	= strWord;

	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
	LPSCRIPT lpScript	= (LPSCRIPT)pScriptDlg->Lookup( strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {, }를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		return 1;
	}

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ADDKEY;
		lstrcpy( rsf.lpszVal1, strWord );
		lstrcpy( rsf.lpszVal2, strKey );
		rsf.dwVal1	= dwParam;
		//CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		rsf.dwVal2 = pUser->m_dwCurQuest;//pMover->m_pNpcProperty->GetQuest();
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	DWORD dwParam	= 0;
	CString strWord, strKey;
	GetToken();	// (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		GetToken();	// data
		if( i == 0 )
			strWord	= token;
		else
		if( i == 1 )
			strKey	= token;
		else
		if( i == 2 )
			dwParam	= atoi( token );
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	strKey	= strWord;
	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
	LPSCRIPT lpScript	= (LPSCRIPT)pScriptDlg->Lookup( strKey );
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( lpScript == NULL ) 
	{
		CString string;
		string.Format( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {, }를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		AfxMessageBox( string );
		return 1;
	}
//	if(lpSCript)	lpSCript->m_bStatus		= TRUE;
//	GetToken();
	if( pWndDialog )
	{ 
		CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		pWndDialog->AddKeyButton( strWord, strKey, dwParam, g_pPlayer->m_dwCurQuest );//pMover->m_pNpcProperty->GetQuest() );
//		pWndDialog->MakeKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
// void AddAnswer( LPCTSTR lpWord, LPCTSTR lpKey,DWORD dwParam );
int AddAnswer( CScript* pScript )
{
	int nParam	= 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		if( i == 0 )
		{
			GetToken();
			strWord	= token;
		}
		else
		if( i == 1 )
		{
			GetToken();
			if( tokenType == STRING )
				strKey = token;
			else
			{
				PutBack();
				i++;
			}
		}
		else
		if( i == 2 )
		{
			Compute( &nParam );
		}
		GetToken();	// ,
	}
	if( strKey.IsEmpty() )	strKey	= strWord;
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ADDANSWER;
		lstrcpy( rsf.lpszVal1, strWord );
		lstrcpy( rsf.lpszVal2, strKey );
		rsf.dwVal1 = (DWORD)nParam;
		//CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		rsf.dwVal2 = pUser->m_dwCurQuest;//pMover->m_pNpcProperty->GetQuest();
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
	{
		CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		pWndDialog->AddAnswerButton( strWord, strKey, nParam, g_pPlayer->m_dwCurQuest );//  pMover->m_pNpcProperty->GetQuest() );
//		pWndDialog->MakeKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int __EndQuest( CScript* pScript, int nQuestId )
{
	int nNum;
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
#ifdef __WORLDSERVER
		CUser* pUser = prj.GetUser( pScript->GetPcId() );
#endif
		if( pQuestProp->m_nRewardItemIdx )
		{
			nNum = pQuestProp->m_nRewardItemMax - pQuestProp->m_nRewardItemMin + 1; 
			int nItemNum = pQuestProp->m_nRewardItemMin + xRandom( nNum ); 
			__CreateItem( pScript->GetPcId(),  pQuestProp->m_nRewardItemIdx, nItemNum );
		}
		if(	pQuestProp->m_nRewardGoldMin )
		{
			nNum = pQuestProp->m_nRewardGoldMax - pQuestProp->m_nRewardGoldMin + 1; 
			int nGold = pQuestProp->m_nRewardGoldMin + xRandom( nNum ); 
#ifdef __WORLDSERVER
			pUser->AddGold( nGold );
			pUser->AddGoldText( nGold );
#else	// __WORLDSERVER
#	ifdef __CLIENT
			g_pPlayer->AddGold( nGold, FALSE );
			CString strGold1; strGold1.Format( "%d", nGold );
			CString strGold2; strGold2.Format( "%d", g_pPlayer->GetGold() );
			CString string;
			string.Format( GETTEXT( TID_GAME_REAPMONEY ), strGold1, strGold2 );
			g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_GAME_REAPMONEY ) );
#	endif	// __CLIENT
#endif	// __WORLDSERVER

		}
		if(	pQuestProp->m_nRewardExpMin )
		{
			nNum = pQuestProp->m_nRewardExpMax - pQuestProp->m_nRewardExpMin + 1; 
			int nExp = pQuestProp->m_nRewardExpMin + xRandom( nNum ); 
#ifdef __WORLDSERVER
			if( pUser->AddExperience( nExp ) )
			{
				g_UserMng.AddSetLevel( pUser, pUser->GetLevel() );
	//			g_DPCoreClient.SendSetGrowthLearningPoint( (CMover*)pUser, pUser->m_nRemainGP, pUser->m_nRemainLP );
				pUser->AddSetGrowthLearningPoint( pUser->m_nRemainGP, pUser->m_nRemainLP );
			}
	//		g_DPCoreClient.SendSetExperience( (CMover*)pUser, (DWORD)pUser->m_nExp1, (DWORD)pUser->m_nExp2, (WORD)pUser->m_nLevel );
			pUser->AddSetExperience( pUser->GetExp1(), pUser->GetExp2(), (WORD)pUser->m_nLevel );
			pUser->AddDefinedText( TID_GAME_REAPEXP );
#else	// __WORLDSERVER
#	ifdef __CLIENT
			g_WndMng.PutString( prj.GetText( TID_GAME_REAPEXP ), NULL, prj.GetTextColor( TID_GAME_REAPEXP ) );
			//g_Caption.AddCaption( string, CWndBase::m_Theme.m_pFontCaption );
			g_pPlayer->AddExperience( nExp );
#	endif	// __CLIENT
#endif	// __WORLDSERVER

		}
		// 퀘스트 종료 
		__SetQuestState( pScript->GetPcId(), nQuestId, QS_END );
#ifdef __WORLDSERVER
		// 아이템 제거 
		if( pQuestProp->m_nEndRemoveItem1 )
			pUser->RemoveItemA( pQuestProp->m_nEndRemoveItem1, pQuestProp->m_nEndRemoveItemNum1 );
		if( pQuestProp->m_nEndRemoveItem2 )
			pUser->RemoveItemA( pQuestProp->m_nEndRemoveItem2, pQuestProp->m_nEndRemoveItemNum2 );
		if( pQuestProp->m_nEndRemoveItem3 )
			pUser->RemoveItemA( pQuestProp->m_nEndRemoveItem3, pQuestProp->m_nEndRemoveItemNum3 );
		if( pQuestProp->m_nEndRemoveGold )
			__RemoveGold( pScript->GetPcId(), pQuestProp->m_nEndRemoveGold );

#else
#	ifdef __CLIENT
		// 아이템 제거 
		/*
		if( pQuestProp->m_nEndRemoveItem1 )
			g_pPlayer->RemoveItemA( pQuestProp->m_nEndRemoveItem1, pQuestProp->m_nEndRemoveItemNum1 );
		if( pQuestProp->m_nEndRemoveItem2 )
			g_pPlayer->RemoveItemA( pQuestProp->m_nEndRemoveItem2, pQuestProp->m_nEndRemoveItemNum2 );
		if( pQuestProp->m_nEndRemoveItem3 )
			g_pPlayer->RemoveItemA( pQuestProp->m_nEndRemoveItem3, pQuestProp->m_nEndRemoveItemNum3 );
		if( pQuestProp->m_nEndRemoveGold )
			__RemoveGold( pScript->GetPcId(), pQuestProp->m_nEndRemoveGold );
			*/
#	endif
#endif
	}
	return 0;
}
int __IsEndQuestCondition( CScript* pScript, int nQuestId )
{
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );

	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( nQuestId );
		if( pCurQuest == NULL || pCurQuest->m_nState == QS_END ) 
			return 0;
		// 시간 제한 체크 1
		if( pQuestProp->m_nEndCondLimitTime == 0 || pCurQuest->m_wTime == 0 )
			nResult++;
		// 정찰 임무  2
		if( pQuestProp->m_dwEndCondPatrolWorld == 0 || pCurQuest->m_bPatrol )
			nResult++;
		// NPC 퇴치 갯수  3,4
		for( int i = 0; i < 2; i++ )
		{
			if( pQuestProp->m_nEndCondKillNPCIdx[ i ] == 0 || pCurQuest->m_nKillNPCNum[ i ] >= pQuestProp->m_nEndCondKillNPCNum[ i ] ) 
				nResult++;
		}
		// 수집 아이템 갯수  5,6,7,8
		for( i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nEndCondItemIdx[ i ] == 0 || pUser->GetItemNum( pQuestProp->m_nEndCondItemIdx[ i ] ) >= pQuestProp->m_nEndCondItemNum[ i ] ) 
				nResult++;
		}
		// 스킬 9
		if( pQuestProp->m_nEndCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pUser->GetSkill( pQuestProp->m_nEndCondSkillIdx );
			if( lpSkill )
			{
				if( pQuestProp->m_nEndCondSkillComp == 0 )
				{
					if( lpSkill->dwLevel == pQuestProp->m_nEndCondSkillLvl )
						nResult++;
				}
				else
				if( pQuestProp->m_nEndCondSkillComp == -1 )
				{
					if( lpSkill->dwLevel < pQuestProp->m_nEndCondSkillLvl )
						nResult++;
				}
				else
				if( pQuestProp->m_nEndCondSkillComp == 1 )
				{
					if( lpSkill->dwLevel < pQuestProp->m_nEndCondSkillLvl )
						nResult++;
				}
			}
		}
		// 카르마 10
		if( pQuestProp->m_nEndCondKarmaPoint == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nEndCondKarmaComp == 0 )
			{
				if( pUser->GetKarma() == pQuestProp->m_nEndCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nEndCondKarmaComp == -1 )
			{
				if( pUser->GetKarma() < pQuestProp->m_nEndCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nEndCondKarmaComp == 1 )
			{
				if( pUser->GetKarma() > pQuestProp->m_nEndCondKarmaPoint )
					nResult++;
			}
		}
		// 카오틱  11
		if( pQuestProp->m_nEndCondChaotic == 0 || pQuestProp->m_nEndCondChaotic == pUser->IsChaotic() )
			nResult++;
		if( nResult == 11 )
			return 1;
	}
#else
	// 클라이언트는 다 똑같은데 파티만 제외한 버젼
	CMover* pUser = prj.GetMover( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( nQuestId );
		for( int i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nEndCondItemIdx[ i ] == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItemIdx[ i ] ) >= pQuestProp->m_nEndCondItemNum[ i ] && pQuestProp->m_nEndCondItemNum[ i ] != -1 ) || ( pQuestProp->m_nEndCondItemNum[ i ] == -1 && pUser->GetItemNum( pQuestProp->m_nEndCondItemIdx[ i ] ) ) ) 
				nResult++;
		}
		if( nResult == 4 )
			return 1;
	}
#endif
	return 0;
}
int IsEndQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	return __IsEndQuestCondition( pScript, v1 );
}
int __IsBeginQuestCondition( CScript* pScript, int nQuestId )
{
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( nQuestId );
		if( pCurQuest != NULL ) 
			return 0;

		// 이전 퀘스트 1
		if( pQuestProp->m_nBeginCondPreviousQuest == 0 ) 
			nResult++;
		else
		{
			LPQUEST pPreQuest = pUser->GetQuest( pQuestProp->m_nBeginCondPreviousQuest );
			if( pQuestProp->m_nBeginCondPreviousQuestType == 0 )
			{
				if( pPreQuest )
					nResult++;
			}
			else
			if( pQuestProp->m_nBeginCondPreviousQuestType == 1 )
			{
				if( pPreQuest && pPreQuest->m_nState == QS_END )
					nResult++;
			}
			else
			if( pQuestProp->m_nBeginCondPreviousQuestType == 2 )
			{
				if( pPreQuest && pPreQuest->m_nState != QS_END )
					nResult++;
			}
		}
		// 이전 퀘스트 2
		if( pQuestProp->m_nBeginCondExclusiveQuest == 0 ) 
			nResult++;
		else
		{
			LPQUEST pPreQuest = pUser->GetQuest( pQuestProp->m_nBeginCondExclusiveQuest );
			if( pPreQuest == NULL )
				nResult++;
		}
		// 직업 3
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pUser->GetJob() )
			{
				nResult++;
				break;
			}
		}
		// 레벨 4
		if( pQuestProp->m_nBeginCondLevelMin == 0 || ( pUser->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pUser->GetLevel() <= pQuestProp->m_nBeginCondLevelMax ) )
			nResult++;
		// 그룹 5
		if( pQuestProp->m_nBeginCondGroup == GRP_ALL )
			nResult++;
		else
		{
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
			if( pQuestProp->m_nBeginCondGroup == GRP_SOLO && pParty == NULL )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondGroup == GRP_PARTY && pParty )
				nResult++;
		}
		// 아이템 6,7,8,9
		for( int i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nBeginCondItemIdx[ i ] == 0 || pUser->GetItemNum( pQuestProp->m_nBeginCondItemIdx[ i ] ) >= pQuestProp->m_nEndCondItemNum[ i ] )
				nResult++;
		}
		// 성별 10
		if( pQuestProp->m_nBeginCondSex == -1 || pQuestProp->m_nBeginCondSex == pUser->GetSex() )
			nResult++;
		// 스킬 11
		if( pQuestProp->m_nBeginCondSkillIdx == 0 )
			nResult++;
		else
		{
			LPSKILL lpSkill = pUser->GetSkill( pQuestProp->m_nBeginCondSkillIdx );
			if( lpSkill )
			{
				if( pQuestProp->m_nBeginCondSkillComp == 0 )
				{
					if( lpSkill->dwLevel == pQuestProp->m_nBeginCondSkillLvl )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondSkillComp == -1 )
				{
					if( lpSkill->dwLevel < pQuestProp->m_nBeginCondSkillLvl )
						nResult++;
				}
				else
				if( pQuestProp->m_nBeginCondSkillComp == 1 )
				{
					if( lpSkill->dwLevel < pQuestProp->m_nBeginCondSkillLvl )
						nResult++;
				}
			}
		}
		// 카르마 12
		if( pQuestProp->m_nBeginCondKarmaPoint == 0 )
			nResult++;
		else
		{
			if( pQuestProp->m_nBeginCondKarmaComp == 0 )
			{
				if( pUser->GetKarma() == pQuestProp->m_nBeginCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nBeginCondKarmaComp == -1 )
			{
				if( pUser->GetKarma() < pQuestProp->m_nBeginCondKarmaPoint )
					nResult++;
			}
			else
			if( pQuestProp->m_nBeginCondKarmaComp == 1 )
			{
				if( pUser->GetKarma() > pQuestProp->m_nBeginCondKarmaPoint )
					nResult++;
			}
		}
		// 카오틱 13
		if( pQuestProp->m_nBeginCondChaotic == 0 || pQuestProp->m_nBeginCondChaotic == pUser->IsChaotic() )
			nResult++;
		if( nResult == 13 )
			return 1;
	}
#else
	// 클라이언트는 다 똑같은데 파티만 제외한 버젼
	CMover* pUser = prj.GetMover( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( nQuestId );
		if( pQuestProp->m_nBeginCondPreviousQuest == -1 ) 
			nResult++;
		else
		{
			LPQUEST pPreQuest = pUser->GetQuest( pQuestProp->m_nBeginCondPreviousQuest );
			if( pPreQuest && pPreQuest->m_nState == QS_END )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pUser->GetJob() )
			{
				nResult++;
				break;
			}
		}
		if( pQuestProp->m_nBeginCondLevelMin == -1 || ( pUser->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pUser->GetLevel() <= pQuestProp->m_nBeginCondLevelMax ) )
			nResult++;
		for( int i = 0; i < 4; i++ )
		{
			if( pQuestProp->m_nBeginCondItemIdx[ i ] == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItemIdx[ i ] ) >= pQuestProp->m_nEndCondItemNum[ i ] && pQuestProp->m_nBeginCondItemNum[ i ] != -1 ) || ( pQuestProp->m_nBeginCondItemNum[ i ] == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItemIdx[ i ] ) ) ) 
				nResult++;
		}
		if( nResult == 8 )
			return 1;
	}
#endif
	return 0;
}
int IsBeginQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	return __IsBeginQuestCondition( pScript, v1 );
}
int GetParam1( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
		return pQuestProp->m_nParam1;
	return 0;
}
int GetParam2( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
		return pQuestProp->m_nParam2;
	return 0;
}
int GetParam3( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
		return pQuestProp->m_nParam3;
	return 0;
}
int GetParam4( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
		return pQuestProp->m_nParam4;
	return 0;
}

int RemoveQuest( CScript* pScript )      
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
	{
		CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//		if( pUser )
//		{
			LPQUEST pQuest	= pUser->GetQuest( v1 );
			if( pQuest )
			{
				if( pQuest->m_nState < 14 ) 
					pUser->AddDefinedText( TID_EVE_CANCELQUEST, "\"%s\"", pQuestProp->m_szTitle );
				pUser->RemoveQuest( v1 );
				pUser->AddScriptRemoveQuest( v1 );
			}
//		}
	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
	{
		LPQUEST pQuest	= g_pPlayer->GetQuest( v1 );
		if( pQuest )
		{
			if( pQuest->m_nState < 14 ) 
			{
				CString string;
				string.Format( GETTEXT( TID_EVE_CANCELQUEST ), pQuestProp->m_szTitle );
				g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_CANCELQUEST ) );
			}
			g_pPlayer->RemoveQuest( v1 );
			CWndQuest* pWndQuest	= (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
			if( pWndQuest )
				pWndQuest->Update();
		}
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int RemoveAllKey(CScript* pScript)
{
	Get2Tkn( pScript );
	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		{
			RunScriptFunc rsf;
			rsf.wFuncType		= FUNCTYPE_REMOVEALLKEY;
			pUser->AddRunScriptFunc( rsf );
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	{
		CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
		if( pWndDialog )
			pWndDialog->RemoveAllKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int SayQuest( CScript* pScript )
{
	int array[32], v1, v2, i = 0;
	CString strToken = "No Dialog";
	CString string;
	//GetToken(); // (
	//GetToken(); // lpszStr
	Eval2( pScript, v1, v2 );

	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp && pQuestProp->m_apQuestDialog[ v2 ] )
		strToken = pQuestProp->m_apQuestDialog[ v2 ];
	GetToken(); 
	while( *token != ')' )
	{
		Compute( &array[i++] );
		GetToken();
	}
	string.FormatV( strToken, (va_list)array );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );

//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_SAY;
		lstrcpy( rsf.lpszVal1, string );
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->Say( string, g_pPlayer->m_dwCurQuest );
//	GetToken();	// )
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int BeginQuest( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	if( pQuestProp )
	{
		if( pQuestProp->m_nBeginAddItem1 )
		{
			__CreateItem( pScript->GetPcId(), pQuestProp->m_nBeginAddItem1, pQuestProp->m_nBeginAddItemNum1 );
		}
		if( pQuestProp->m_nBeginAddItem2 )
		{
			__CreateItem( pScript->GetPcId(), pQuestProp->m_nBeginAddItem2, pQuestProp->m_nBeginAddItemNum2 );
		}
		if( pQuestProp->m_nBeginAddItem3 )
		{
			__CreateItem( pScript->GetPcId(), pQuestProp->m_nBeginAddItem3, pQuestProp->m_nBeginAddItemNum3 );
		}
		if( pQuestProp->m_nBeginAddGold )
		{
			__AddGold(  pScript->GetPcId(), pQuestProp->m_nBeginAddGold );
		}
		__SetQuest( pScript->GetPcId(), v1 );
	}
	return 1;
}
int EndQuest( CScript* pScript )
{
	int nNum, v1	= EvalExp( pScript );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );

	if( pQuestProp )
	{
#ifdef __WORLDSERVER
		CUser* pUser = prj.GetUser( pScript->GetPcId() );
#endif
		if( pQuestProp->m_nRewardItemIdx )
		{
			nNum = pQuestProp->m_nRewardItemMax - pQuestProp->m_nRewardItemMin + 1; 
			int nItemNum = pQuestProp->m_nRewardItemMin + xRandom( nNum ); 
			__CreateItem( pScript->GetPcId(),  pQuestProp->m_nRewardItemIdx, nItemNum );
		}
		if(	pQuestProp->m_nRewardGoldMin )
		{
			nNum = pQuestProp->m_nRewardGoldMax - pQuestProp->m_nRewardGoldMin + 1; 
			int nGold = pQuestProp->m_nRewardGoldMin + xRandom( nNum ); 
#ifdef __WORLDSERVER
			pUser->AddGold( nGold );
			pUser->AddGoldText( nGold );
#else	// __WORLDSERVER
#	ifdef __CLIENT
			g_pPlayer->AddGold( nGold, FALSE );
			CString strGold1; strGold1.Format( "%d", nGold );
			CString strGold2; strGold2.Format( "%d", g_pPlayer->GetGold() );
			CString string;
			string.Format( GETTEXT( TID_GAME_REAPMONEY ), strGold1, strGold2 );
			g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_GAME_REAPMONEY ) );
#	endif	// __CLIENT
#endif	// __WORLDSERVER

		}
		if(	pQuestProp->m_nRewardExpMin )
		{
			nNum = pQuestProp->m_nRewardExpMax - pQuestProp->m_nRewardExpMin + 1; 
			int nExp = pQuestProp->m_nRewardExpMin + xRandom( nNum ); 
#ifdef __WORLDSERVER
			if( pUser->AddExperience( nExp ) )
			{
				g_UserMng.AddSetLevel( pUser, pUser->GetLevel() );
	//			g_DPCoreClient.SendSetGrowthLearningPoint( (CMover*)pUser, pUser->m_nRemainGP, pUser->m_nRemainLP );
				pUser->AddSetGrowthLearningPoint( pUser->m_nRemainGP, pUser->m_nRemainLP );
			}
	//		g_DPCoreClient.SendSetExperience( (CMover*)pUser, (DWORD)pUser->m_nExp1, (DWORD)pUser->m_nExp2, (WORD)pUser->m_nLevel );
			pUser->AddSetExperience( pUser->GetExp1(), pUser->GetExp2(), (WORD)pUser->m_nLevel );
			pUser->AddDefinedText( TID_GAME_REAPEXP );
#else	// __WORLDSERVER
#	ifdef __CLIENT
			g_WndMng.PutString( prj.GetText( TID_GAME_REAPEXP ), NULL, prj.GetTextColor( TID_GAME_REAPEXP ) );
			//g_Caption.AddCaption( string, CWndBase::m_Theme.m_pFontCaption );
			g_pPlayer->AddExperience( nExp );
#	endif	// __CLIENT
#endif	// __WORLDSERVER

		}
		// 퀘스트 종료 
		__SetQuestState( pScript->GetPcId(), v1, QS_END );
#ifdef __WORLDSERVER
		// 아이템 제거 
		if( pQuestProp->m_nEndRemoveItem1 )
			pUser->RemoveItemA( pQuestProp->m_nEndRemoveItem1, pQuestProp->m_nEndRemoveItemNum1 );
		if( pQuestProp->m_nEndRemoveItem2 )
			pUser->RemoveItemA( pQuestProp->m_nEndRemoveItem2, pQuestProp->m_nEndRemoveItemNum2 );
		if( pQuestProp->m_nEndRemoveItem3 )
			pUser->RemoveItemA( pQuestProp->m_nEndRemoveItem3, pQuestProp->m_nEndRemoveItemNum3 );
		if( pQuestProp->m_nEndRemoveGold )
			__RemoveGold( pScript->GetPcId(), pQuestProp->m_nEndRemoveGold );

#else
#	ifdef __CLIENT
		// 아이템 제거 
		/*
		if( pQuestProp->m_nEndRemoveItem1 )
			g_pPlayer->RemoveItemA( pQuestProp->m_nEndRemoveItem1, pQuestProp->m_nEndRemoveItemNum1 );
		if( pQuestProp->m_nEndRemoveItem2 )
			g_pPlayer->RemoveItemA( pQuestProp->m_nEndRemoveItem2, pQuestProp->m_nEndRemoveItemNum2 );
		if( pQuestProp->m_nEndRemoveItem3 )
			g_pPlayer->RemoveItemA( pQuestProp->m_nEndRemoveItem3, pQuestProp->m_nEndRemoveItemNum3 );
		if( pQuestProp->m_nEndRemoveGold )
			__RemoveGold( pScript->GetPcId(), pQuestProp->m_nEndRemoveGold );
			*/
#	endif
#endif
	}
	return 0;
}
int IsEndQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( v1 );
		if( pQuestProp->m_nEndCondItem1 == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItem1 ) >= pQuestProp->m_nEndCondItemNum1 && pQuestProp->m_nEndCondItemNum1 != -1 ) || ( pQuestProp->m_nEndCondItemNum1 == -1 && pUser->GetItemNum( pQuestProp->m_nEndCondItem1 ) ) ) 
			nResult++;
		if( pQuestProp->m_nEndCondItem2 == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItem2 ) >= pQuestProp->m_nEndCondItemNum2 && pQuestProp->m_nEndCondItemNum2 != -1 ) || ( pQuestProp->m_nEndCondItemNum2 == -1 && pUser->GetItemNum( pQuestProp->m_nEndCondItem2 ) ) )   
			nResult++;
		if( pQuestProp->m_nEndCondItem3 == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItem3 ) >= pQuestProp->m_nEndCondItemNum3 && pQuestProp->m_nEndCondItemNum3 != -1 ) || ( pQuestProp->m_nEndCondItemNum3 == -1 && pUser->GetItemNum( pQuestProp->m_nEndCondItem3 ) ) )  
			nResult++;
		if( nResult == 3 )
			return 1;
	}
#else
	// 클라이언트는 다 똑같은데 파티만 제외한 버젼
	CMover* pUser = prj.GetMover( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( v1 );
		if( pQuestProp->m_nEndCondItem1 == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItem1 ) >= pQuestProp->m_nEndCondItemNum1 && pQuestProp->m_nEndCondItemNum1 != -1 ) || ( pQuestProp->m_nEndCondItemNum1 == -1 && pUser->GetItemNum( v1 ) ) ) 
			nResult++;
		if( pQuestProp->m_nEndCondItem2 == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItem2 ) >= pQuestProp->m_nEndCondItemNum2 && pQuestProp->m_nEndCondItemNum2 != -1 ) || ( pQuestProp->m_nEndCondItemNum2 == -1 && pUser->GetItemNum( v1 ) ) )   
			nResult++;
		if( pQuestProp->m_nEndCondItem3 == 0 || ( pUser->GetItemNum( pQuestProp->m_nEndCondItem3 ) >= pQuestProp->m_nEndCondItemNum3 && pQuestProp->m_nEndCondItemNum3 != -1 ) || ( pQuestProp->m_nEndCondItemNum3 == -1 && pUser->GetItemNum( v1 ) ) )  
			nResult++;
		if( nResult == 3 )
			return 1;
	}
#endif
	return 0;
}

int IsBeginQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( v1 );
		if( pQuestProp->m_nBeginCondPreviousQuest == -1 ) 
			nResult++;
		else
		{
			LPQUEST pPreQuest = pUser->GetQuest( pQuestProp->m_nBeginCondPreviousQuest );
			if( pPreQuest && pPreQuest->m_nState == QS_END )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pUser->GetJob() )
			{
				nResult++;
				break;
			}
		}
		if( pQuestProp->m_nBeginCondLevelMin == -1 || ( pUser->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pUser->GetLevel() <= pQuestProp->m_nBeginCondLevelMax ) )
			nResult++;
		if( pQuestProp->m_nBeginCondGroup == -1 )
			nResult++;
		else
		{
			g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
			g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
			if( pQuestProp->m_nBeginCondGroup == 0 && pParty == NULL )
				nResult++;
			else
			if( pQuestProp->m_nBeginCondGroup == 1 && pParty )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondItem1 == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItem1 ) >= pQuestProp->m_nEndCondItemNum1 && pQuestProp->m_nBeginCondItemNum1 != -1 ) || ( pQuestProp->m_nBeginCondItemNum1 == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItem1 ) ) ) 
			nResult++;
		if( pQuestProp->m_nBeginCondItem2 == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItem2 ) >= pQuestProp->m_nEndCondItemNum2 && pQuestProp->m_nBeginCondItemNum2 != -1 ) || ( pQuestProp->m_nBeginCondItemNum2 == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItem2 ) ) )   
			nResult++;
		if( pQuestProp->m_nBeginCondItem3 == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItem3 ) >= pQuestProp->m_nEndCondItemNum3 && pQuestProp->m_nBeginCondItemNum3 != -1 ) || ( pQuestProp->m_nBeginCondItemNum3 == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItem3 ) ) )  
			nResult++;
		if( nResult == 7 )
			return 1;
	}
#else
	// 클라이언트는 다 똑같은데 파티만 제외한 버젼
	CMover* pUser = prj.GetMover( pScript->GetPcId() );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v1 );
	if( pQuestProp )
	{
		int nResult = 0;
		LPQUEST pCurQuest = pUser->GetQuest( v1 );
		if( pQuestProp->m_nBeginCondPreviousQuest == -1 ) 
			nResult++;
		else
		{
			LPQUEST pPreQuest = pUser->GetQuest( pQuestProp->m_nBeginCondPreviousQuest );
			if( pPreQuest && pPreQuest->m_nState == QS_END )
				nResult++;
		}
		if( pQuestProp->m_nBeginCondJobNum == 0 )
			nResult++;
		else
		for( int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++ )
		{
			if( pQuestProp->m_nBeginCondJob[ i ] == pUser->GetJob() )
			{
				nResult++;
				break;
			}
		}
		if( pQuestProp->m_nBeginCondLevelMin == -1 || ( pUser->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pUser->GetLevel() <= pQuestProp->m_nBeginCondLevelMax ) )
			nResult++;
		if( pQuestProp->m_nBeginCondItem1 == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItem1 ) >= pQuestProp->m_nEndCondItemNum1 && pQuestProp->m_nBeginCondItemNum1 != -1 ) || ( pQuestProp->m_nBeginCondItemNum1 == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItem1 ) ) ) 
			nResult++;
		if( pQuestProp->m_nBeginCondItem2 == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItem2 ) >= pQuestProp->m_nEndCondItemNum2 && pQuestProp->m_nBeginCondItemNum2 != -1 ) || ( pQuestProp->m_nBeginCondItemNum2 == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItem2 ) ) )   
			nResult++;
		if( pQuestProp->m_nBeginCondItem3 == 0 || ( pUser->GetItemNum( pQuestProp->m_nBeginCondItem3 ) >= pQuestProp->m_nEndCondItemNum3 && pQuestProp->m_nBeginCondItemNum3 != -1 ) || ( pQuestProp->m_nBeginCondItemNum3 == -1 && pUser->GetItemNum( pQuestProp->m_nBeginCondItem3 ) ) )  
			nResult++;
		if( nResult == 7 )
			return 1;
	}
#endif
	return 0;
}
// void AddQuestKey( int nQuest_szTitle, LPCTSTR lpKey, int nVal );
int AddQuestKey( CScript* pScript )
{
	int nParam = 0;
	int nQuest = -1;
	CString strWord = "No Quest", strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		if( i == 0 )
		{
			Compute( &nQuest );
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
			if( pQuestProp )
				strWord = pQuestProp->m_szTitle;
		}
		if( i == 1 )
		{
			GetToken(); // data
			if( tokenType == STRING )
				strKey = token;
			else
			{
				PutBack();
				i++;
			}
		}
		if( i == 2 )
		{
			Compute( &nParam );
		}
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	strKey	= strWord;
	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
	LPSCRIPT lpScript	= (LPSCRIPT)pScriptDlg->Lookup( strKey );
#ifdef __WORLDSERVER
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {}, 를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ADDKEY;
		lstrcpy( rsf.lpszVal1, strWord );
		lstrcpy( rsf.lpszVal2, strKey );
		rsf.dwVal1	= nParam;
		rsf.dwVal2 = pUser->m_dwCurQuest;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( lpScript == NULL ) 
	{
		CString string;
		string.Format( "%s(%d) : 키 리스트 중에서 \"%s\"를 찾을 수 없소! {}, 를 잘못했거나, /**/를 잘못했거나 분명 문법 오류가 있소이다!", pScript->m_szFileName, pScript->GetLineNum(), strKey );
		AfxMessageBox( string );
		return 1;
	}
	if( pWndDialog )
	{
		CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		pWndDialog->AddKeyButton( strWord, strKey, nParam,  g_pPlayer->m_dwCurQuest );//pMover->m_pNpcProperty->GetQuest() );
//		pWndDialog->MakeKeyButton();
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
// RunQuestId( );
/*
int RunQuestId( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1, v2, v3;
	CString strKey;
	GetToken(); // (
	GetToken(); // key
	strKey = token;
	Eval2( pScript, v1, v2 );
	GetToken(); // )
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );

	int nResult = 0;
	pUser->m_dwCurQuest = (DWORD)v1;
	if( pMover->m_pNpcProperty->RunDialog( strKey, &nResult, v2, pScript->GetNpcId(), pScript->GetPcId() ) == FALSE ) 
		return FALSE;
	return nResult ? TRUE : 0;
#endif
	return 1;
}
// RunMyQuest( LPCTSTR szQuestKey, int nId, int nVal );
int RunNextMyQuest( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1, v2, v3;
	CString strKey;
	GetToken(); // (
	GetToken(); // key
	strKey = token;
	Eval1( pScript, v1 );
	GetToken(); // )

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );

	int nResult1 = 0;
	int nResult2 = 0;

	if( pUser->m_apCacheQuest.GetSize() )
	{
		LPCHARACTER lpChar = pMover->GetCharacter();
		int i = pScript->m_globalArray[ 15 ];
		LPQUEST lpQuest = (LPQUEST) pUser->m_apCacheQuest.GetAt( i );
		if( lpQuest )
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
			if( pQuestProp )
			{
				// 대화하고 있는 캐릭터와 현재 실행 중인 퀘스트가 같으면 그놈이 종결 조건을 판정하는 놈이다.
				if( strcmpi( pQuestProp->m_szEndCondCharacter, pMover->m_szCharacterKey ) == 0 )
				{
					pScript->m_globalArray[ 15 ] = i + 1;
					pUser->m_dwCurQuest = (DWORD)lpQuest->m_wId;
					if( pMover->m_pNpcProperty->RunDialog( strKey, &nResult1, v2, pScript->GetNpcId(), pScript->GetPcId() ) == FALSE ) 
						return FALSE;
					//return TRUE;
					nResult2 += nResult1;
				}
			}
			else
				WriteError( "RunMyQuest : pQuestProp NULL이다." );
		}
		else
			WriteError( "RunMyQuest : lpQuest가 NULL이다." );
	}
	return nResult2 ? TRUE : 0;
#endif
	return 1;
}
*/
int GetQuestState( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1 = EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		LPQUEST pQuest = pUser->GetQuest( v1 );
		if( pQuest ) 
			return pQuest->m_nState;
//	}
	return -1;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	LPQUEST pQuest	= g_pPlayer->GetQuest( v1 );
	if( pQuest )	
		return pQuest->m_nState;
	return -1;
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int IsSetQuest( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1 = EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		LPQUEST pQuest = pUser->GetQuest( v1 );
		if( pQuest ) return TRUE;
//	}
	return FALSE;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	LPQUEST pQuest	= g_pPlayer->GetQuest( v1 );
	if( pQuest )	return TRUE;
	return FALSE;
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int	IsExecuteQuest( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( pUser->m_nQuestSize )
		return TRUE;
#endif	// __WORLDSERVER
	return 0;
}
int __SetQuestState( DWORD dwIdMover, int nQuest, int nState )
{
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( dwIdMover );
//	if( pUser )
//	{
		if( nState == QS_END )
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
			if( pQuestProp )
				pUser->AddDefinedText( TID_EVE_ENDQUEST, "\"%s\"", pQuestProp->m_szTitle );

			g_dpDBClient.SendLogQuest( pUser, 2, nQuest );
		}
		else
		if( nState == QS_BEGIN )
		{
			g_dpDBClient.SendLogQuest( pUser, 1, nQuest );
		}
		pUser->SetQuest( nQuest, nState );
		pUser->AddSetQuest( nQuest, nState );
		// #dlvr
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	if( nState == QS_END )
	{
		QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
		if( pQuestProp )
		{
			CString string;
			string.Format( GETTEXT( TID_EVE_ENDQUEST ), pQuestProp->m_szTitle );
			g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_ENDQUEST ) );
		}
	}
	g_pPlayer->SetQuest( nQuest, nState );
	CWndQuest* pWndQuest	= (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
	if( pWndQuest )
		pWndQuest->Update(); 
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

void __SetQuest( DWORD dwIdMover, int nQuest )
{
#ifdef __WORLDSERVER
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
	CUser* pUser	= prj.GetUser( dwIdMover );
//	if( pUser )
//	{
		if( pQuestProp )
			pUser->AddDefinedText( TID_EVE_STARTQUEST, "\"%s\"", pQuestProp->m_szTitle );
		pUser->SetQuest( nQuest, 0 );
		pUser->AddSetQuest( nQuest, 0 );
		g_dpDBClient.SendLogQuest( pUser, 1, nQuest );
		// #dlvr
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	g_pPlayer->SetQuest( nQuest, 0 );
	QuestProp* pQuestProp	= prj.m_aPropQuest.GetAt( nQuest );
	if( pQuestProp )
	{
		CString string;
		string.Format( GETTEXT( TID_EVE_STARTQUEST ), pQuestProp->m_szTitle );
		g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_STARTQUEST ) );
	}
	CWndQuest* pWndQuest	= (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
	if( pWndQuest )
		pWndQuest->Update(); 
#	endif	// __CLIENT
#endif	// __WORLDSERVER
}
void __CreateItem( DWORD dwIdMover, int v1, int v2 )	
{
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( dwIdMover );
//	if( pUser )
//	{
		ItemProp* pItemProp		= prj.GetItemProp( v1 );
		if( pItemProp )
			pUser->AddDefinedText( TID_EVE_REAPITEM, "\"%s\"", pItemProp->szName );
		if( v2 > pItemProp->dwPackMax )
			v2 = pItemProp->dwPackMax;
		CItemElem itemElem;
		itemElem.m_dwItemId		= v1;
		itemElem.m_nItemNum		= v2;
		if( pUser->CreateItem( &itemElem ) )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "Q";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "QUEST";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetGold();
			aLogItem.Gold2 = pUser->GetGold();
			aLogItem.ItemNo = itemElem.m_dwSerialNumber;
			aLogItem.ItemName = itemElem.GetProp()->szName;
			aLogItem.itemNumber = itemElem.m_nItemNum;
			aLogItem.nAbilityOption = itemElem.GetAbilityOption();
			aLogItem.nItemResist = itemElem.m_bItemResist;
			aLogItem.nResistAbilityOption = itemElem.m_nResistAbilityOption;
			aLogItem.Negudo = int((float)itemElem.m_nHitPoint * 100 / (float)itemElem.GetProp()->dwEndurance );
			g_DPSrvr.OnLogItem( aLogItem );
		}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	ItemProp* pItemProp		= prj.GetItemProp( v1 );
	if( pItemProp )
	{
		CString string;
		string.Format( GETTEXT( TID_EVE_REAPITEM ), pItemProp->szName );
		g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_REAPITEM ) );
	}
	CItemElem itemElem;
	itemElem.m_dwItemId		= v1;
	itemElem.m_nItemNum		= v2;
	g_pPlayer->AddItem( &itemElem );
#	endif	// __CLIENT
#endif	//__WORLDSERVER
}
//}}AFX
#endif //  __V050322_CACHEQUEST
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// SetMark();
int SetMark( CScript* pScript )
{
	/*
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
//	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
//	pScriptDlg->SetMark();
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_SETMARK;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
	pScriptDlg->SetMark();
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->MakeKeyButton();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
		*/
	return 1;
}
// GoMark();
int GoMark( CScript* pScript )
{
	/*
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
//	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
//	pScriptDlg->GoMark();
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_GOMARK;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
	pScriptDlg->GoMark();
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->MakeKeyButton();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
		*/
	return 1;
}

int RandomIndex( CScript* pScript) //100, 20, 12, 1, 20, 12, 1, 20, 13, 1 )
{
#ifndef _BEAST
	int nRandom, nMax = 1;
	Eval1( pScript, nRandom ); // ( max
	GetToken(); // , or )
	int anValue[64], v1;
	anValue[ 0 ] = 0;
	while( *token != ')' )
	{
		Compute( &v1 );
		anValue[ nMax++ ] = v1;
		GetToken(); // , or )
	}
	int nResult = 0;
	for( int i = 0; i < nMax; i++ )
	{
		nResult += anValue[ i ];
		anValue[ i ] = nResult;
	}
	int nRand = random( nRandom );
	for( i = 0; i < nMax - 1;  i++ )
	{
		if( nRand >= anValue[ i ] && nRand < anValue[ i + 1 ]  )
			return i;
	}
	return 0;
#endif
}
// void RemoveKey(LPCTSTR lpStr);
int RemoveKey(CScript* pScript)
{
	GetToken();
	GetToken();
#ifdef __V050322_CACHEQUEST
	CScriptDialog* pScriptDlg	= (CScriptDialog*)internalArray[SMC_FUNC];
#else
	CScriptDialog* pScriptDlg	= (CScriptDialog*)globalArray[SMC_FUNC];
#endif

	CString strWord		= token;
	LPSCRIPT lpScript	= pScriptDlg->Lookup( strWord );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		if( lpScript )
		{
			RunScriptFunc rsf;
			rsf.wFuncType		= FUNCTYPE_REMOVEKEY;
			lstrcpy( rsf.lpszVal1, strWord );
			pUser->AddRunScriptFunc( rsf );
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	if( lpScript ) 
	{
		CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
		if( pWndDialog )
			pWndDialog->RemoveKeyButton( strWord );
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}


////////////////////////////////////////////////////////////////////////////
// Dialog

int Say(CScript* pScript)
{
	int array[32], i	= 0;
	CString strToken;
	CString string;
	GetToken(); // (
	GetToken(); // lpszStr
	strToken	= token;
	GetToken(); 
	while( *token != ')' )
	{
		Compute( &array[i++] );
		GetToken();
	}
	string.FormatV( strToken, (va_list)array );
#ifdef __WORLDSERVER

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_SAY;
		lstrcpy( rsf.lpszVal1, string );
		//CMover* pMover = prj.GetMover( pScript->GetNpcId() );
		rsf.dwVal2 = 0;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->Say( string, 0 );
//	GetToken();	// )
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int EndSay( CScript* pScript )
{
	Get2Tkn( pScript );
	// 사용 안함 
	/*
#ifdef __WORLDSERVER
	GetToken();	// (
	GetToken();	// )
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_ENDSAY;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	GetToken();	// (
//	if( pWndDialog )
//		pWndDialog->EndSay();
	GetToken();	// )
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	*/
	return 1;
}

int Random( CScript* pScript )
{
	int val = EvalExp( pScript );
	return rand() % val;
}

int Exit( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_EXIT;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	CWndDialog* pWndDialog	= (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndDialog )
		pWndDialog->Destroy();
//	g_pPlayer->EndScript();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}

#ifndef __DBSERVER
int SetScriptTimer( CScript* pScript )
{
	UINT nTimer = EvalExp( pScript );
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	//if( pMover && pMover->m_pNpcProperty )
	//	pMover->m_pNpcProperty->m_timerScript.Set( SEC( nTimer ) );
	return 1;
}
#endif	// __DBSERVER

//////////////////////////////////
// Replace( int nWorld, int x, int y, int z )
//////////////////////////////////
int Replace( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1, v2, v3, v4;
	GetToken(); // (
	Compute( &v1 );
	GetToken(); // ,
	Compute( &v2 );
	GetToken(); // ,
	Compute( &v3 );
	GetToken(); // ,
	Compute( &v4 );
	GetToken(); // )

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		if( v2 > 0 && v4 > 0 )
		{
			RunScriptFunc rsf;	// Exit()
			rsf.wFuncType		= FUNCTYPE_EXIT;
			pUser->AddRunScriptFunc( rsf );
#ifdef __SCHOOL0701
			if( g_eLocal.GetState( EVE_SCHOOL ) )
			{
				// seghope: 여기에 로그 작업, pUser->m_idPlayer
				g_dpDBClient.SendLogSchool( pUser->m_idPlayer, pUser->GetName() );
			}
#endif	// __SCHOOL0701

#ifdef __MSVR
			pUser->Replace( g_uIdofMulti, v1, D3DXVECTOR3( v2, v3, v4 ) );
#else	// __MSVR
			pUser->Replace( v1, D3DXVECTOR3( v2, v3, v4 ) );
#endif	// __MSVR
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1, v2, v3, v4;
	GetToken();	// (
	Compute( &v1 );
	GetToken();	// ,
	Compute( &v2 );
	GetToken();	// ,
	Compute( &v3 );
	GetToken();	// ,
	Compute( &v4 );
	GetToken();	// )
	CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndBase )
		pWndBase->Destroy();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

#ifdef __SCHOOL0701
int Replace2( CScript* pScript )
{
	int v1, v2, v3, v4, v5, v6;
	v1	= v2	= v3	= v4	= v5	= v6	= 0;
	GetToken(); // (
	Compute( &v1 );	// world
	GetToken(); // ,
	Compute( &v2 );	// left
	GetToken(); // ,
	Compute( &v3 );	// top
	GetToken(); // ,
	Compute( &v4 );	// right
	GetToken(); // )
	Compute( &v5 );	// bottom
	GetToken(); // )
	Compute( &v6 );	// height
	GetToken(); // )
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	{
		RunScriptFunc rsf;	// Exit()
		rsf.wFuncType		= FUNCTYPE_EXIT;
		pUser->AddRunScriptFunc( rsf );
		D3DXVECTOR3 v;
		v.x	= v2 + xRandom( v4 - v2 );
		v.z	= v3 + xRandom( v5 - v3 );
		v.y	= v6;
#ifdef __MSVR
		pUser->Replace( g_uIdofMulti, v1, v );
#else	// __MSVR
		pUser->Replace( v1, v );
#endif	// __MSVR
	}
#endif	// __WORLDSERVER
	return 1;
}
#endif	// __SCHOOL0701

//////////////////////////////////
// Replace( int nWorld, LPCTSTR szKey )
//////////////////////////////////
int ReplaceKey( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1;
	CString strKey;
	GetToken(); // (
	Compute( &v1 );
	GetToken(); // ,
	GetToken(); // key
	strKey = token;
	GetToken(); // )

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		RunScriptFunc rsf;	// Exit()
		rsf.wFuncType		= FUNCTYPE_EXIT;
		pUser->AddRunScriptFunc( rsf );

		PRegionElem pRgnElem	= g_WorldMng.GetRevivalPos( v1, strKey );
		if( NULL != pRgnElem )
		{
#ifdef __MSVR
			pUser->Replace( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#else	// __MSVR
			pUser->Replace( pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#endif	// __MSVR
		}
		else
		{
//			TRACE( "
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1;
	CString strKey;
	GetToken();	// (
	Compute( &v1 );
	GetToken();	// ,
	GetToken();	// key
	strKey = token;
	GetToken();	// )
	CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_DIALOG );
	if( pWndBase )
		pWndBase->Destroy();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
/*
GetDstId() 캐릭터의 아이디를 얻어온다.
GetPlayerLvl( Id ) 캐릭터의 레벨을 얻어온다.
GetFlightLv( Id )	캐릭터의 비행 레벨을 얻어온다.
GetPlayerJob( Id ) 캐릭터의 직업을 얻어온다. defineJob의 넘버

SetEvent( Id ) 해당 이벤트를 실행시켜준다.
GetEvent( Id ) 해당 이벤트가 실행중인지 확인한다.
EndEvent( Id ) 해당 이벤트를 종료시켜준다.
GetItemNum( Id, Item_ID ) 해당 유저의 해당 아이템을 인벤토리에 몇개 가지고 있는지 확인한다. 장비창 제외
RemoveAllItem( Id, Item_ID) 해당 유저의 해당 아이템을 모두 제거 한다.
ChangeJob( Id ) 해당 넘버로 직업을 변경해준다. defineJob의 넘버

GetPlayerSex( ID ) 해당 유저의 성이 무엇인지 확인 defineAttribute.h에 설정
EquipItem( ID, Item_ID) 해당 유저에게 해당 아이템을 장착해줌 능력치가 안되면 인벤토리에 넣어주며 인벤토리가 꽉차면 아무거나 땅에 떨어뜨림 장착하고 떨어지는 아이템도 동일하게 처리
*/
//#ifdef __CLIENT
int GetPlayerLvl( CScript* pScript )  
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		return pUser->GetLevel();
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	return g_pPlayer->GetLevel();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

// 스킬 아이디, 스킬 레벨
int GetPlayerSkillLvl( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	GetToken();
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	LPSKILL pSkill = pUser->GetSkill( v1 );
	if( pSkill )
	{
		return pSkill->dwLevel;
	}
#endif	// __WORLDSERVER
	return -1;
}

int	GetFlightLvl( CScript* pScript )
{
#	ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		return pUser->GetFlightLevel();
//	}
#	else	// __WORLDSERVER
#		ifdef __CLIENT
	Get2Tkn( pScript );
	return g_pPlayer->GetFlightLevel();
#		endif	// __CLIENT
#	endif	// __WORLDSERVER
	return 1;
}

int	SetFlightLvl( CScript* pScript )
{
#	ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		pUser->m_nFlightLv	= (LONG)v1;
//	}
#	else	// __WORLDSERVER
#		ifdef __CLIENT
	int v1	= EvalExp( pScript );
	g_pPlayer->m_nFlightLv	= (LONG)v1;
#		endif	// __CLIENT
#	endif	// __WORLDSERVER
	return 1;
}

int GetPlayerJob( CScript* pScript )  
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		return pUser->GetJob();
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	return g_pPlayer->GetJob();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int	SetGuildQuest( CScript* pScript )
{
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( v1 );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( pProp )
	{
		//
	}
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		pGuild->SetQuest( v1, QS_BEGIN );
		g_dpDBClient.SendInsertGuildQuest( pGuild->m_idGuild, v1 );
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
#endif	// __WORLDSERVER
	return 1;
}

int IsGuildQuest( CScript* pScript )
{
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		PGUILDQUEST pQuest	= pGuild->GetQuest( v1 );
		if( pQuest )
		{
			g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
			return TRUE;
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
#endif	// __WORLDSERVER
	return FALSE;
}

int SetGuildQuestState( CScript* pScript )
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		pGuild->SetQuest( v1, v2 );
		g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, v1, v2 );
	}
	else
	{
		// error
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
#endif	// __WORLDSERVER
	return 1;
}

int GetGuildQuestState( CScript* pScript )
{
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		PGUILDQUEST pQuest	= pGuild->GetQuest( v1 );
		if( pQuest )
		{
			g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
			return pQuest->nState;
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
#endif	// __WORLDSERVER
	return -1;
}

int IsWormonServer( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	return g_eLocal.GetState( EVE_WORMON );
#endif	// __WORLDSERVER
	return 1;
}

/*
int IsHuntinWormon( CScipt* pScript )
{
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
	return pProcessor->IsQuesting( v1 );
#endif	// __WORLDSERVER
	return 1;
}
*/
int PrintSystemMessage( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	pUser->AddDefinedText( (DWORD)v1 );
#endif	// __WORLDSERVER
	return 1;
}

int MonHuntStart( CScript* pScript )
{
	int v1, v2, v3, v4;
	Eval4( pScript, v1, v2, v3, v4 );
	GetToken();
#ifdef __WORLDSERVER
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
	if( pProcessor->IsQuesting( v1 ) )
	{
		return 0;
	}
	else
	{
		CUser* pUser	= prj.GetUser( pScript->GetPcId() );
		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
		CGuild* pGuild	= pUser->GetGuild();
		if( pGuild )
		{
			GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( v1 );
			if( pProp )
			{
				CMover* pWormon	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pProp->dwWormon );
				if( pWormon )
				{
					pWormon->SetPos( pProp->vPos );
					pWormon->InitMotion( pWormon->m_dwMotion );
					pWormon->UpdateLocalMatrix();
					pWormon->m_nQuestKeeping	= v1;

					CWorld* pWorld;
					if( ( pWorld = pUser->GetWorld() ) )
					{
						RunScriptFunc rsf;	// Exit()
						rsf.wFuncType		= FUNCTYPE_EXIT;
						pUser->AddRunScriptFunc( rsf );
						pWormon->AddItToGlobalId();
						pWorld->AddObj( pWormon, FALSE );
						pGuild->SetQuest( v1, v2 );
						if( v2 == QS_BEGIN )
							g_dpDBClient.SendInsertGuildQuest( pGuild->m_idGuild, v1 );
						else
							g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, v1, v2 );

						pProcessor->SetGuildQuest( v1, v2, v3, v4, pGuild->m_idGuild, pWormon->GetId() );
						D3DXVECTOR3 vPos( pProp->vPos.x, pProp->vPos.y, ( ( pProp->vPos.z * 2 ) + pProp->y2 ) / 3 );
						pGuild->Replace( pProp->dwWorldId, vPos, TRUE );
						g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
						return 1;
					}
					delete pWormon;
				}
			}
		}
		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	}
#endif	// __WORLDSERVER
	return 0;
}

int	InitStat( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		pUser->SetStr( 15 );
		pUser->SetInt( 15 );
		pUser->SetDex( 15 );
		pUser->SetSta( 15 );
		pUser->m_nRemainGP	= ( pUser->GetLevel() - 1 ) * 2;
		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_INITSTAT;
		rsf.dwVal1	= pUser->m_nRemainGP;
		pUser->AddRunScriptFunc( rsf );
//	}
#else	// __WORLDSERVER
#ifdef __CLIENT
	Get2Tkn( pScript );
	g_pPlayer->SetStr( 15 );
	g_pPlayer->SetInt( 15 );
	g_pPlayer->SetDex( 15 );
	g_pPlayer->SetSta( 15 );
	g_pPlayer->m_nRemainGP	= ( g_pPlayer->GetLevel() - 1 ) * 2;
#endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int	InitStr( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nStr = pUser->m_nStr;
	if( nStr - 15 > 0 )
	{
		pUser->SetStr( 15 );
		pUser->m_nRemainGP	+= nStr - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTR;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
#endif	// __WORLDSERVER
	return 1;
}
int	InitSta( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nSta = pUser->m_nSta;
	if( nSta - 15 > 0 )
	{
		pUser->SetSta( 15 );
		pUser->m_nRemainGP	+= nSta - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTA;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
#endif	// __WORLDSERVER
	return 1;
}
int	InitDex( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nDex = pUser->m_nDex;
	if( nDex - 15 > 0 )
	{
		pUser->SetDex( 15 );
		pUser->m_nRemainGP	+= nDex - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITDEX;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
#endif	// __WORLDSERVER
	return 1;
}
int	InitInt( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nInt = pUser->m_nInt;
	if( nInt - 15 > 0 )
	{
		pUser->SetInt( 15 );
		pUser->m_nRemainGP	+= nInt - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITINT;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
#endif	// __WORLDSERVER
	return 1;
}

int SetQuestState( CScript* pScript )      
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	return __SetQuestState( pScript->GetPcId(), v1, v2 );
}
int SetQuest( CScript* pScript )      
{
	int v1	= EvalExp( pScript );
	__SetQuest(  pScript->GetPcId(), v1 );
	return 1;
}

int CreateItem( CScript* pScript )	
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	__CreateItem( pScript->GetPcId(), v1, v2 );
	return 1;
}
void __AddGold( DWORD dwIdMover, int nGold )     
{
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( dwIdMover );
	pUser->AddGold( nGold );
	pUser->AddGoldText( nGold );
#else	
	#ifdef __CLIENT
		g_pPlayer->AddGold( nGold, FALSE );
		CString strGold1; strGold1.Format( "%d", nGold );
		CString strGold2; strGold2.Format( "%d", g_pPlayer->GetGold() );
		CString string;
		string.Format( GETTEXT( TID_GAME_REAPMONEY ), strGold1, strGold2 );
		g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_GAME_REAPMONEY ) );
	#endif	// __CLIENT
#endif	// !__WORLDSERVER
}

int AddGold( CScript* pScript )     
{
	int v1	= EvalExp( pScript );
	__AddGold(  pScript->GetPcId(), v1 );
	return 1;
}
void __RemoveGold( DWORD dwIdMover, int nGold )
{
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( dwIdMover );
	if( pUser->AddGold( -nGold ) == FALSE )
		pUser->SetGold( 0 );
#else	// __WORLDSERVER
	#ifdef __CLIENT
		if( g_pPlayer->AddGold( -nGold, FALSE ) == FALSE )
			g_pPlayer->SetGold( 0 );
	#endif	// __CLIENT
#endif	// __WORLDSERVER
}

int	RemoveGold( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	__RemoveGold( pScript->GetPcId(), v1 );
	return 1;
}
int GetPlayerGold( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetGold();
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	return g_pPlayer->GetGold();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
#ifdef __J0103
int	TransPxp( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		CMclAutoLock	Lock( g_CompanyMng.m_AddRemoveLock );
		CCompany* pCompany	= g_CompanyMng.GetCompany( pUser->m_idCompany );
		if( pCompany && pCompany->IsMember( pUser->m_idPlayer ) )
		{
			int nPxp	= pUser->m_nExp2 * v1 / 100;
			pUser->SetPointParam( DST_PXP, pUser->m_nExp2 - nPxp );
//			g_DPCoreClient.SendSetPointParam( pUser, DST_PXP, pUser->m_nExp2 );
			g_DPCoreClient.SendScriptTransPxp( pCompany->m_idCompany, pUser->m_idPlayer, nPxp );
		}
//	}
#else	// __WORLDSERVER
	int v1	= EvalExp( pScript );
#endif	// __WORLDSERVER
	return 1;
}

int	TransPenya( CScript* pScript )
{
	return 1;
}

int	IsCompanyMember( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		CMclAutoLock	Lock( g_CompanyMng.m_AddRemoveLock );
		CCompany* pCompany	= g_CompanyMng.GetCompany( pUser->m_idCompany );
		return( pCompany && pCompany->IsMember( pUser->m_idPlayer ) );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	CCompany* pCompany	= g_CompanyMng.GetCompany( g_pPlayer->m_idCompany );
	return( pCompany && pCompany->IsMember( g_pPlayer->m_idPlayer ) );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
#endif	// __J0103

int GetLocalEventState( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	return g_eLocal.GetState( v1 );
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	return g_eLocal.GetState( v1 );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 0;
}

// AddExp( int nExp )
int AddExp( CScript* pScript )      
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		if( pUser->AddExperience( v1 ) )
		{
			g_UserMng.AddSetLevel( pUser, pUser->GetLevel() );
//			g_DPCoreClient.SendSetGrowthLearningPoint( (CMover*)pUser, pUser->m_nRemainGP, pUser->m_nRemainLP );
			pUser->AddSetGrowthLearningPoint( pUser->m_nRemainGP, pUser->m_nRemainLP );
		}
//		g_DPCoreClient.SendSetExperience( (CMover*)pUser, (DWORD)pUser->m_nExp1, (DWORD)pUser->m_nExp2, (WORD)pUser->m_nLevel );
		pUser->AddSetExperience( pUser->GetExp1(), pUser->GetExp2(), (WORD)pUser->m_nLevel );
		pUser->AddDefinedText( TID_GAME_REAPEXP );
		// #dlvr
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	g_WndMng.PutString( prj.GetText( TID_GAME_REAPEXP ), NULL, prj.GetTextColor( TID_GAME_REAPEXP ) );
	//g_Caption.AddCaption( string, CWndBase::m_Theme.m_pFontCaption );
	g_pPlayer->AddExperience( v1 );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int IsParty( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
//		CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );
		g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
		CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
//		return( pParty != NULL && pParty->IsMember( pUser->m_idPlayer ) );
		int f	= ( pParty != NULL && pParty->IsMember( pUser->m_idPlayer ) );
		g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
		return f;
//	}
	return 0;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	return g_Party.IsMember( g_pPlayer->m_idPlayer );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int IsPartyMaster( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
//		CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );
		g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
		CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
		int f	= ( pParty != NULL && pParty->IsLeader( pUser->m_idPlayer ) );
		g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
		return f;
//	}
	return 0;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	if( g_Party.m_nSizeofMember )
	{
		CMover* pObjMember	= prj.GetUserByID( g_Party.m_aMember[0].m_uPlayerId );
		if( pObjMember == g_pPlayer )
			return TRUE;
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return FALSE;
}

int GetPartyNum( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nSize	= 0;
	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
		nSize	= pParty->GetSizeofMember();
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
	return nSize;
#endif	// __WORLDSERVER
	return 0;
}

int	GetPartyLevel( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nLevel	= 0;
	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
		nLevel	= pParty->GetLevel();
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
	return nLevel;
#endif	// __WORLDSERVER
	return 0;
}

int IsPartyGuild( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER

	int f	= 0;
	u_long	aidPlayer[MAX_PTMEMBER_SIZE];
	int	nSize	= 0;
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );

	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		nSize	= pParty->m_nSizeofMember;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
			aidPlayer[i]	= pParty->m_aMember[i].m_uPlayerId;
	}
	else
	{
		f	= 1;
	}
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1

	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock2
	for( int i = 0; i < nSize; i++ )
	{
		CUser* pUsertmp	= g_UserMng.GetUserByPlayerID( aidPlayer[i] );
		if( IsValidObj( pUsertmp ) )
		{
			if( pUsertmp->GetGuild() )
			{
				f	= 1;
				break;
			}
			CTime tCurrent = CTime::GetCurrentTime();
			if( tCurrent < pUsertmp->m_tGuildMember )
			{
				f	= 2;
				break;
			}
		}
		else
		{
			f	= 1;
			break;
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock2
	return f;

#endif	// __WORLDSERVER
	return 0;
}

int IsGuild( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	int f	= ( pGuild && pGuild->IsMember( pUser->m_idPlayer ) );
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return f;
#else	// __WORLDSERVER
	#ifdef __CLIENT
		return TRUE;
	#endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int IsGuildMaster( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	int f	= ( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) );
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return f;
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	CGuild* pGuild	= g_GuildMng.GetGuild( g_pPlayer->m_idGuild );
	return ( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int CreateGuild( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nSize	= 0;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];

	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
	{
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			CUser* pUsertmp	= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pUsertmp ) )
			{
				info[nSize].idPlayer	= pUsertmp->m_idPlayer;
				info[nSize].nLevel	= pUsertmp->GetLevel();
				info[nSize].nJob	= pUsertmp->GetJob();
				info[nSize].dwSex	= pUsertmp->GetSex();
				nSize++;
			}
			else
			{
				// error	- offline
				g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
				return 0;
			}
		}
	}
	else
	{
		// error	-
		g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
		return 0;
	}

	g_DPCoreClient.SendCreateGuild( info, nSize, "" );
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );

#else	// __WORLDSERVER
	#ifdef __CLIENT
	Get2Tkn( pScript );
	#endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int DestroyGuild( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock1

	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMember( pUser->m_idPlayer ) )
	{
		if( pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			RunScriptFunc rsf;
			rsf.wFuncType	= FUNCTYPE_DESTROYGUILD;
			pUser->AddRunScriptFunc( rsf );
		}
		else
			pUser->AddDefinedText( TID_GAME_COMDELNOTKINGPIN );
	}
	else
		pUser->AddDefinedText( TID_GAME_COMNOHAVECOM );

	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1

#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	if( !CMover::GetActiveMover() )	return 0;

	CGuild* pGuild	= g_GuildMng.GetGuild( g_pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMember( g_pPlayer->m_idPlayer ) )
	{
		if( pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
		{
			// window
		}
		else
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_COMDELNOTKINGPIN ), NULL, prj.GetTextColor( TID_GAME_COMDELNOTKINGPIN ) );
		}
	}
	else
	{
		g_WndMng.PutString( prj.GetText( TID_GAME_COMNOHAVECOM ), NULL, prj.GetTextColor( TID_GAME_COMNOHAVECOM ) );
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

/*
int EmptyQuest( CScript* pScript )      
{
#ifndef __BEAST
	int v1 = EvalExp( pScript );
#ifdef __ONLINE
	g_DPlay.SendSetQuest( v1, 0 );
#else	// __ONLINE
	g_pPlayer->SetQuest( v1, 0 );
#endif	// __ONLINE
#endif	// __BEAST
	return 1;
}

int BeginQuest( CScript* pScript )      
{
#ifndef __BEAST
	int v1 = EvalExp( pScript );
#ifdef __ONLINE
	g_DPlay.SendSetQuest( v1, 0 );
#else	// __ONLINE
	g_pPlayer->SetQuest( v1, 0 );
#endif	// __ONLINE
#endif	// __BEAST
	return 1;
}
int EndQuest( CScript* pScript )      
{
#ifndef __BEAST
	int v1 = EvalExp( pScript );
#ifdef __ONLINE
	g_DPlay.SendSetQuest( v1, 15 );
#else	// __ONLINE
	g_pPlayer->SetQuest( v1, 15 );
#endif	// __ONLINE
#endif	// __BEAST
	return 1;
}
int IsEmptyQuest( CScript* pScript )      
{
#ifndef __BEAST
	int v1 = EvalExp( pScript );
	LPQUEST pQuest = g_pPlayer->GetQuest( v1 );
	if( pQuest ) return FALSE;
	return TRUE;
#endif
	return 1;
}
int IsBeginQuest( CScript* pScript )      
{
#ifndef __BEAST
	int v1 = EvalExp( pScript );
	LPQUEST pQuest = g_pPlayer->GetQuest( v1 );
	if( pQuest ) return pQuest->m_nState == 0;
	return FALSE;
#endif
	return 1;
}
int IsEndQuest( CScript* pScript )      
{
#ifndef __BEAST
	int v1 = EvalExp( pScript );
	LPQUEST pQuest = g_pPlayer->GetQuest( v1 );
	if( pQuest ) return pQuest->m_nState == 15;
	return FALSE;
#endif
	return 1;
}
*/

int IsPlayerSkillPoint( CScript* pScript )     
{
#ifdef __WORLDSERVER
	Get2Tkn(pScript);
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( ((CMover*)pUser)->m_dwSkillPoint != 0 )	return 1;
#endif	// __WORLDSERVER
	return 0;
}

int ChangeJob( CScript* pScript )     
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		if( pUser->IsBaseJob() && pUser->GetLevel() != MAX_JOB_LEVEL )
		{
			Error( "Error ChangeJob : 1" );
			return 0;
		}
		if( pUser->IsExpert() && pUser->GetLevel() < MAX_JOB_LEVEL + MAX_EXP_LEVEL )
		{
			Error( "Error ChangeJob : 2" );
			return 0;
		}
		if( pUser->IsPro() )
		{
			Error( "Error ChangeJob : 3" );
			return 0;
		}
		if( pUser->AddChangeJob( v1 ) )
		{
			pUser->AddSetChangeJob( v1 );
			g_UserMng.AddNearSetChangeJob( (CMover*)pUser, v1, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
			// #dlvr
			g_dpDBClient.SendLogLevelUp( (CUser*)pUser, 4 );
			g_DPCoreClient.SendPartyMemberJob( (CUser*)pUser );
			g_DPCoreClient.SendFriendChangeJob( (CUser*)pUser );
			if( pUser->m_idGuild != 0 )
				g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)pUser );
		}
		else
		{
			Error( "Error ChangeJob : CMover::AddChangeJob" );
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	g_pPlayer->SetExpert( v1 );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int GetPlayerSex( CScript* pScript )  
{
#ifdef __WORLDSERVER
	Get2Tkn(pScript);
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		return pUser->GetSex();
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	Get2Tkn( pScript );
	return g_pPlayer->GetSex();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int GetItemNum( CScript* pScript )    
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		return pUser->GetItemNum( v1 );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	return g_pPlayer->GetItemNum( v1 );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int RemoveAllItem( CScript* pScript ) 
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		pUser->RemoveAllItem( v1 );
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	g_pPlayer->RemoveAllItem( v1 );
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
int EquipItem( CScript* pScript )     
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
//	if( pUser )
//	{
		BYTE nId, nCount	= 1;
		short nNum;
		
		CItemElem itemElem;
		itemElem.m_dwItemId		= v1;
		itemElem.m_nItemNum		= 1;
		itemElem.SetAbilityOption( 0 );
		if( pUser->CreateItem( &itemElem, &nId ) == TRUE )
		{
			CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
			if( pItemElem && TRUE == pUser->DoEquip( pItemElem, TRUE ) )
			{
//				g_DPCoreClient.SendDoEquip( (CMover*)pUser, nId );
				g_UserMng.AddDoEquip( (CMover*)pUser, -1, pItemElem, TRUE );
			}

			LogItemInfo aLogItem;
			aLogItem.Action = "Q";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "QUEST";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
		}
		else
		{
			CItemElem* pItemElem	= new CItemElem;
			pItemElem->m_dwItemId	= v1;
			ItemProp* pItemProp		= pItemElem->GetProp();
			if( pItemProp )
			{
				pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
				pItemElem->m_nItemNum	= 1;
				pItemElem->SetAbilityOption( 0 );
				CItem* pItem	= new CItem;
				pItem->m_pItemBase	= pItemElem;

				if( pItemElem->m_dwItemId == 0 ) Error("EquipItem SetIndex\n" );
				pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
				pItem->SetPos( pUser->GetPos() );
				pItem->SetAngle( xRandom( 360 ) );
				pItem->m_idHolder	= pUser->m_idPlayer;
				pUser->GetWorld()->AddObj( pItem, TRUE );
				pUser->AddDefinedText( TID_GAME_DROPITEM, "\"%s\"", prj.GetItemProp( v1 )->szName );
			}
		}
//	}
#else	// __WORLDSERVER
#	ifdef __CLIENT
	int v1	= EvalExp( pScript );
	BYTE nId;
	CItemElem itemElem;
	itemElem.m_dwItemId		= v1;
	itemElem.m_nItemNum		= 1;
	if( g_pPlayer->m_Inventory.Add( &itemElem, &nId ) == TRUE )
	{
		CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAtId( nId );
		ASSERT( pItemElem );
		g_pPlayer->DoEquip( pItemElem, TRUE );
	}
	else
	{
		CItemElem* pItemElem	= new CItemElem;
		pItemElem->m_dwItemId	= v1;
		pItemElem->m_nItemNum	= 1;
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp )
			pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
		CItem* pItem	= new CItem;
		pItem->m_pItemBase	= pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( g_pPlayer->GetPos() );
		pItem->SetAngle( xRandom( 360 ) );
		pItem->m_idHolder	= g_pPlayer->m_idPlayer;
		g_pPlayer->GetWorld()->AddObj( pItem, TRUE );
	}
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}
//
// int PlaySound( LPCTSTR lpszSoundFileName )
//
int PlaySound(CScript* pScript)
{
#ifdef __WORLDSERVER
	GetToken();
	GetToken();
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	if( pUser )
	{
		pUser->AddPlaySound2( token );
	}
	GetToken();
#else	// __WORLDSERVER
#	ifdef __CLIENT
	GetToken();
	GetToken();
	PLAYSND( token );
	GetToken();
#	endif	// __CLIENT
#endif	// __WORLDSERVER
	return 1;
}

int DropQuestItem( CScript* pScript )
{
// ( dwItemId, dwProbability )
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );

	if( xRandom( 1000 ) <  v2 )
	{
		ItemProp* pItemProp	= prj.GetItemProp( v1 );
		if( pItemProp )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				CItemElem* pItemElem	= new CItemElem;
				pItemElem->m_dwItemId	= v1;
				CItem* pItem	= new CItem;
				pItem->m_pItemBase	= pItemElem;
				if( pItemElem->m_dwItemId == 0 ) Error( "DropQuestItem SetIndex\n" );
				pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
				pItem->m_idHolder	= pUser->m_idPlayer;
				pItem->m_idOwn	= pUser->GetId();
				pItem->m_dwDropTime	= timeGetTime();
				CMover* pMover	= prj.GetMover( pScript->GetNpcId() );
				D3DXVECTOR3 vPos	= pMover->GetPos();
				vPos.x	+= ( xRandomF( 2.0f ) - 1.0f );
				vPos.z	+= ( xRandomF( 2.0f ) - 1.0f );
				pItem->SetPos( vPos );
				pItem->SetAngle( xRandom( 360 ) );
				pWorld->AddObj( pItem, TRUE );
				return 1;
			}
		}
		else
		{
			Error( "DropQuestItem, %d null prop", v1 );
		}
	}
#endif	// __WORLDSERVER
	return 0;
}

int IsUSLecture(CScript* pScript)
{
// ( dwProbability )
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	return( xRandom( 1000 ) < v1 );
#endif	// __WORLDSERVER
	return 1;
}

int PlayQuestBGM(CScript* pScript)
{
// ( dwBGM )
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	pUser->AddPlaySound( v1 );
#endif	// __WORLDSERVER
	return 1; 
}

int ExpQuestEffect(CScript* pScript)
{
// ( dwSfx, bNPC )
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
#ifdef __WORLDSERVER
	CCtrl* pCtrl	= ( v2? prj.GetCtrl( pScript->GetNpcId() ): prj.GetCtrl( pScript->GetPcId() ) );
	g_UserMng.AddCreateSfxObj( pCtrl, v1 );
#endif	// __WORLDSERVER
	return 1;
}

int TeleportParty(CScript* pScript)
{
// ( dwWorldId, x, y, z )
	int v1, v2, v3, v4;
	Eval4( pScript, v1, v2, v3, v4 );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_PartyMng.Lock( theLineFile );
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		D3DXVECTOR3 vPos( v2, v3, v4 );
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			CUser* pUsertmp		= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( (CObj*)pUsertmp ) && pUsertmp->GetWorld() && pUser->IsNearPC( pUsertmp ) )
			{
				D3DXVECTOR3 vDist	= pUsertmp->GetPos() - pUser->GetPos();
				if( D3DXVec3LengthSq( &vDist ) < 4096.0f )	// 64m
					pUsertmp->Replace( g_uIdofMulti, v1, vPos );
			}
		}
	}
	g_PartyMng.Unlock( theLineFile );
#endif	// __WORLDSERVER
	return 1;
}
	
int RecoveryParty(CScript* pScript)
{
// ()
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	g_PartyMng.Lock( theLineFile );
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			CUser* pUsertmp		= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( (CObj*)pUsertmp ) && pUsertmp->GetWorld() && pUser->IsNearPC( pUsertmp ) )
			{
				pUsertmp->SetPointParam( DST_HP, pUsertmp->GetMaxHitPoint() );
				pUsertmp->SetPointParam( DST_MP, pUsertmp->GetMaxManaPoint() );
				pUsertmp->SetPointParam( DST_FP, pUsertmp->GetMaxFatiguePoint() );
			}
		}
	}
	g_PartyMng.Unlock( theLineFile );
#endif	// __WORLDSERVER
	return 1;
}

int SetNevigator( CScript* pScript )
{
// ( dwWorldId, x, y, z )
	int v1, v2, v3, v4;
	Eval4( pScript, v1, v2, v3, v4 );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	RunScriptFunc rsf;
	rsf.wFuncType	= FUNCTYPE_SETNAVIGATOR;
	rsf.dwVal1	= v1;
	rsf.vPos	= D3DXVECTOR3( v2, v3, v4 );
	pUser->AddRunScriptFunc( rsf );
#endif	// __WORLDSERVER
	return 1;
}

int RemoveNevigator( CScript* pScript )
{
	Get2Tkn( pScript );
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	RunScriptFunc rsf;
	rsf.wFuncType	= FUNCTYPE_REMOVENAVIGATOR;
	pUser->AddRunScriptFunc( rsf );
#endif	// __WORLDSERVER
	return 1;
}

int MonsterCreate( CScript* pScript )
{
	int v1, v2, v3, v4, v5, v6, v7;
	Eval7( pScript, v1, v2, v3, v4, v5, v6, v7 );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	MoverProp* pMoverProp = prj.GetMoverPropEx( v1 );
	CWorld* pWorld	= pUser->GetWorld();
	if( pMoverProp )
	{
		if( v2 > 100 ) v2 = 100;
		if( v2 < 1 ) v2 = 1;
		if( v3 > v2 ) v3 = v2;
		if( v3 < 1 ) v3 = 0;

		int nAttackNum = 0;
		D3DXVECTOR3 vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		for( DWORD dw = 0; dw < v2; dw++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	return 1;	// ASSERT( pObj );

			vPos.x = (float)xRandom( v4, v6 );
			vPos.z = (float)xRandom( v5, v7 );
			pObj->SetPos( vPos );
			pObj->InitMotion( pObj->m_dwMotion );
			pObj->UpdateLocalMatrix();

			if( nAttackNum < v3 )
			{
				((CMover*)pObj)->m_bActiveAttack = 1;
				++nAttackNum;
			}
			
			pWorld->AddObj( pObj, TRUE );
		}
	}
	else
	{
		Error( "Script MonsterCreate = %d Not MoverProp", v1 );
	}
#endif	// __WORLDSERVER
	return 1;
}

int RemoveItem( CScript* pScript )
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
#ifdef __WORLDSERVER
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->RemoveItemA( v1, v2 );
#endif	// __WORLDSERVER
	return 1;
}

int Run( CScript* pScript )
{
	int v1;
	CString string;
	GetToken(); // (
	GetToken(); // key
	string = token;
	Eval1( pScript, v1 );
	GetToken(); // )

#ifdef __WORLDSERVER
	CScriptDialog::SetBeforeRun( (const char*)string );
#endif	// __WORLDSERVER

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	int nResult = 0;
	pMover->m_pNpcProperty->RunDialog( string, &nResult, v1, pScript->GetNpcId(), pScript->GetPcId(), 0 );

#ifdef __WORLDSERVER
	CScriptDialog::SetAfterRun( (const char*)string );
#endif	// __WORLDSERVER

	return nResult;

}
/*
int RunQuest( CScript* pScript )
{
	int v1;
	CString string;
	GetToken(); // (
	GetToken(); // key
	string = token;
	Eval1( pScript, v1 );
	GetToken(); // )

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	int nResult1 = 0;
	int nResult2 = 0;
	for( int i = 0; i < 99; i++ )
	{
		CString strKey;
		strKey.Format( "%s%02d", string, i );
		if( pMover->m_pNpcProperty->RunDialog( strKey, &nResult1, v1, pScript->GetNpcId(), pScript->GetPcId() ) == FALSE ) 
			return FALSE;
		nResult2 += nResult1;
	}	
	return nResult2 ? TRUE : 0;
}
int RunQuestCnt( CScript* pScript )
{
	int v1, v2, v3;
	CString string;
	GetToken(); // (
	GetToken(); // key
	string = token;
	Eval3( pScript, v1, v2, v3 );
	GetToken(); // )

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	int nResult1 = 0;
	int nResult2 = 0;
	for( int i = v1; i <= v2; i++ )
	{
		CString strKey;
		strKey.Format( "%s%02d", string, i );
		if( pMover->m_pNpcProperty->RunDialog( strKey, &nResult1, v3, pScript->GetNpcId(), pScript->GetPcId() ) == FALSE ) 
			return FALSE;
		nResult2 += nResult1;
	}	
	return nResult2 ? TRUE : 0;
}
int RunQuestIdCnt( CScript* pScript )
{
	int v1, v2, v3;
	CString strKey;
	GetToken(); // (
	GetToken(); // key
	strKey = token;
	Eval3( pScript, v1, v2, v3 );
	GetToken(); // )
#ifdef __WORLDSERVER
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );

	int nResult1 = 0;
	int nResult2 = 0;
	
	//CUIntArray arguArray; //argument 
	//arguArray.SetAtGrow( 0, v3 );
	//arguArray.SetAtGrow( 1, pScript->GetNpcId() );
	//arguArray.SetAtGrow( 2, pScript->GetPcId() );
	//arguArray.SetAtGrow( 3, v3 );
	for( int i = v1; i <= v2; i++ )
	{
		pUser->m_dwCurQuest = (DWORD)i;
		if( pMover->m_pNpcProperty->RunDialog( strKey, &nResult1, v3, pScript->GetNpcId(), pScript->GetPcId() ) == FALSE ) 
			return FALSE;
		nResult2 += nResult1;
	}	
	return nResult2 ? TRUE : 0;
#endif
	return 1;
}
int SetQuestId( CScript* pScript )
{
	int v1	= EvalExp( pScript );
#ifdef __WORLDSERVER
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	pUser->m_dwCurQuest = v1;
#endif
	return 1;
}
*/
int GetQuestId( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
#ifdef  __V050322_CACHEQUEST
	return pScript->GetQuest();
#else
//{{AFX
	return (int)pUser->m_dwCurQuest;
//}}AFX
#endif
#endif
	return 1;
}
int GetLang( CScript* pScript )
{
	Get2Tkn( pScript );
	return prj.m_nLanguage;
}

#ifdef __NPP_050308
// 카르마 포인트 얻어오기

int GetKarmaPoint( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetKarma();
#endif	// __WORLDSERVER
	return -1;
}

// 카르마 포인트 셑팅
int SetKarmaPoint( CScript* pScript )
{
#ifdef __WORLDSERVER
	int v1	= EvalExp( pScript );
	GetToken();
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	pUser->ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, v1 );
#endif	// __WORLDSERVER
	return 0;
}

// 카오인지 확인
int IsChaotic( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->IsChaotic();
#endif	// __WORLDSERVER
	return 0;
}

#endif // __NPP_050308
//#endif	// __CLIENT

#ifdef __J0519
// 캐릭터 명 변경

int	QuerySetPlayerName( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId()  );
	if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) || pUser->IsMode( QUERYSETPLAYERNAME_MODE ) )
	{
		RunScriptFunc rsf;
		rsf.wFuncType	= FUNCTYPE_QUERYSETPLAYERNAME;
		pUser->AddRunScriptFunc( rsf );
		return 1;
	}
#endif	// __WORLDSERVER
	return 0;
}
#endif	// __J0519

#ifdef __J0524
int	QuerySetGuildName( CScript* pScript )
{
#ifdef __WORLDSERVER
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) || pUser->IsMode( QUERYSETPLAYERNAME_MODE ) )
	{
		RunScriptFunc rsf;
		rsf.wFuncType	= FUNCTYPE_QUERYSETGUILDNAME;
		pUser->AddRunScriptFunc( rsf );
		return 1;
	}
#endif	// __WORLDSERVER
	return 0;
}
#endif	// __J0524

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//#endif	// !( __BEAST|| __WORLDSERVER || __DBSERVER )
/***************
 * 함수 테이블 *
 ***************/

InterFuncType interFunc[] = 
{
	"GetVal"            , GetVal            , "int  GetVal()",
	"GetSrcId"          , GetSrcId          , "int  GetSrcId()",
	"GetDstId"          , GetDstId          , "int  GetDstId()",
	"NpcId"             , NpcId             , "int  NpcId()",
	"PcId"              , PcId              , "int  PcId()",

	"Src"               , GetSrcId          , "int  Src()",
	"Dst"               , GetDstId          , "int  Dst()",
	"PC"                , GetSrcId          , "int  Src()",
	"NPC"               , GetDstId          , "int  Dst()",
	"GetCaller"         , GetCaller         , "int  GetCaller()",

	// Talk and say
	"IsTalk"            , IsTalk            , "void IsTalk()",
	"Say"               , Say               , "Say"     , 
	"SayName"           , SayName           , "SayName"  ,
	"AddSay"            , Say               , "AddSay"     , 
	"EndSay"            , EndSay            , "EndSay"   , 
	"Speak"             , Speak             , "Speak"    ,
	"SpeakName"         , SpeakName         , "SpeakName",
//  "Talk"              , Talk              , "Talk"     ,

	"Random"            , Random            , "Random",

	"Replace"			, Replace           , "Replace",
	"ReplaceKey"		, ReplaceKey        , "ReplaceKey",

//#ifdef __CLIENT
	"GetDstId"            , GetDstId          , "GetDstId"          ,//() 캐릭터의 아이디를 얻어온다.
	"GetPlayerLvl"        , GetPlayerLvl      , "GetPlayerLvl"      ,//( Id ) 캐릭터의 레벨을 얻어온다.
	"GetPlayerSkillLvl"	  , GetPlayerSkillLvl , "GetPlayerSkillLvl" ,//( Id ) 스킬 레벨을 얻어온다.
	"GetPlayerJob"        , GetPlayerJob      , "GetPlayerJob"      ,//( Id ) 캐릭터의 직업을 얻어온다. defineJob의 넘버

	"GetFlightLvl",	GetFlightLvl,	"GetFlightLvl",//( Id ) 캐릭터의 비행 레벨을 얻어온다.
	"SetFlightLvl",	SetFlightLvl,	"SetFlightLvl",	// ( Id ) 캐릭터의 비행 레벨을 설정한다.
	"SetQuestState"       , SetQuestState     , "SetQuestState"          ,//( Id ) 해당 이벤트를 실행시켜준다.
	"GetQuestState"       , GetQuestState     , "GetQuestState"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"IsSetQuest"          , IsSetQuest        , "IsSetQuest"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"SetQuest"            , SetQuest          , "SetQuest"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"RemoveQuest"         , RemoveQuest       , "RemoveQuest"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"IsExecuteQuest"            , IsExecuteQuest	, "IsExecuteQuest"          ,//( Id ) 실행중인 퀘스트가 있는가?
	"AddExp"              , AddExp            , "AddExp", // 경험치 업 

	"CreateItem"          , CreateItem       , "CreateItem"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"AddGold"         , AddGold      , "AddGold"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.

	"RemoveGold"	, RemoveGold	, "RemoveGold"	,
	"GetPlayerGold"		, GetPlayerGold		, "GetPlayerGold"	,

#ifdef __NPP_050308
	"GetKarmaPoint"				  , GetKarmaPoint		  , "GetKarmaPoint"				, // 카르마 포인트 얻어오기

	"SetKarmaPoint"				  , SetKarmaPoint		  , "SetKarmaPoint"				, // 카르마 포인트 셑팅
	"IsChaotic"					  , IsChaotic			  , "IsChaotic"					, // 카오인지 확인
#endif // __NPP_050308

#ifdef __J0519
	"QuerySetPlayerName"				,	QuerySetPlayerName			, "QuerySetPlayerName"			, // 플레이어 명 변경

#endif	// __J0519
#ifdef __J0524
	"QuerySetGuildName"				,	QuerySetGuildName			, "QuerySetGuildName"			, // 길드 명 변경

#endif	// __J0524

	"SetGuildQuest"	,	SetGuildQuest	,	"SetGuildQuest"	,	// ( nQuestId )
	"IsGuildQuest"	,	IsGuildQuest	,	"IsGuildQuest"	,	//( nQuestId )
	"SetGuildQuestState"	,	SetGuildQuestState	,	"SetGuildQuestState"	,	// ( nQuestId, nQuestState )
	"GetGuildQuestState"	,	GetGuildQuestState	,	"GetGuildQuestState"	,	// ( nQuestId )
	"IsWormonServer"	, IsWormonServer	,	"IsWormonServer"	,	// ()
	"MonHuntStart"	,	MonHuntStart,	"MonHuntStart"	, // ( nQuestId, nState, ns, nf )

	"DropQuestItem"	,	DropQuestItem	,	"DropQuestItem"	,	// ( dwItemId, dwProbability )
	"IsUSLecture"	,	IsUSLecture	,	"IsUSLecture"	,	// ( dwProbability )
	"PlayQuestBGM"	,	PlayQuestBGM	,	"PlayQuestBGM"	,	// ( dwBGM )
	"ExpQuestEffect"	,	ExpQuestEffect	,	"ExpQuestEffect"	,	// ( dwSfx, bNPC )
	"TeleportParty"	,	TeleportParty	,	"TeleportParty"	,	// ( dwWorldId, x, y, z )
	"RecoveryParty"	,	RecoveryParty	,	"RecoveryParty"	,	// ()
	"SetNevigator"	,	SetNevigator	,	"SetNevigator"	,	// ( dwWorldId, x, y, z )
	"RemoveNevigator"	,	RemoveNevigator	,	"RemoveNevigator"	,	// ()
	"RemoveItem"	,	RemoveItem	,	"RemoveItem"	,	//( dwItemId, num )

	"MonsterCreate"	,	MonsterCreate,	"MonsterCreate"	,	//( dwMonsterId, MonsterNum, AttackNum, x, z, x1, x2 )

#ifdef __J0103
	"TransPxp"		, TransPxp,		"TransPxp"	,
	"TransPenya"	, TransPenya	, "TransPenya"		,
	"IsCompanyMember"	,	IsCompanyMember		, "IsCompanyMember"		,
#endif	// __J0103
	"GetLocalEventState"	,	GetLocalEventState		, "GetLocalEventState"		,
#ifdef __SCHOOL0701
	"Replace2"	,	Replace2	, "Replace2"		,
#endif	// __SCHOOL0701

	"InitStat",		InitStat,	"InitStat",		// 스탯 초기화
	"InitStr",		InitStr,	"InitStr",		// 스탯 Str 초기화
	"InitSta",		InitSta,	"InitSta",		// 스탯 Sta 초기화
	"InitDex",		InitDex,	"InitDex",		// 스탯 Dex 초기화
	"InitInt",		InitInt,	"InitInt",		// 스탯 Int 초기화
	"RandomIndex"         , RandomIndex      , "RandomIndex"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.

	//"EmptyQuest"        , EmptyQuest        , "EmptyQuest"          ,//( Id ) 해당 이벤트를 실행시켜준다.
	//"BeginQuest"        , BeginQuest        , "BeginQuest"          ,//( Id ) 해당 이벤트를 실행시켜준다.
	//"EndQuest"          , EndQuest          , "EndQuest"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	//"IsEmptyQuest"      , IsEmptyQuest      , "EmptyQuest"          ,//( Id ) 해당 이벤트를 실행시켜준다.
	//"IsBeginQuest"      , IsBeginQuest      , "BeginQuest"          ,//( Id ) 해당 이벤트를 실행시켜준다.
	//"IsEndQuest"        , IsEndQuest        , "EndQuest"          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.

	//"EndQuest"          , EndQuest          , "EndQuest"          ,//( Id ) 해당 이벤트를 종료시켜준다.
	"GetItemNum"        , GetItemNum        , "GetItemNum"        ,//( Id, Item_ID ) 해당 유저의 해당 아이템을 인벤토리에 몇개 가지고 있는지 확인한다. 장비창 제외
	"RemoveAllItem"     , RemoveAllItem     , "RemoveAllItem"     ,//( Id, Item_ID) 해당 유저의 해당 아이템을 모두 제거 한다.
	"ChangeJob"         , ChangeJob         , "ChangeJob"         ,//( Id ) 해당 넘버로 직업을 변경해준다. defineJob의 넘버

	"GetPlayerSex"      , GetPlayerSex      , "GetPlayerSex"      ,// ( ID ) 해당 유저의 성이 무엇인지 확인 defineAttribute.h에 설정
	"EquipItem"         , EquipItem         , "EquipItem"         ,//( ID, Item_ID) 해당 유저에게 해당 아이템을 장착해줌 능력치가 안되면 인벤토리에 넣어주며 인벤토리가 꽉차면 아무거나 땅에 떨어뜨림 장착하고 떨어지는 아이템도 동일하게 처리
	"IsPlayerSkillPoint", IsPlayerSkillPoint, "IsPlayerSkillPoint",// 해당 유저가 m_dwSkillPoint값이 있으면 TRUE, 0이면 FALSE
//#endif	// __CLIENT

	// Server only
	"SetScriptTimer"    , SetScriptTimer    , "SetScriptTimer",

//#if !( defined( __WORLDSERVER ) || defined(__DBSERVER) )
	///////////////////////////////////////////////////////////////////////
	// Client only

	"SetAllKey"        , SetAllKey          , "void SetAllKey()",
	"AddKey"           , AddKey             , "void AddKey()",
	"AddAnswer"          , AddAnswer            , "void AddAnswer()",
	"RemoveKey"        , RemoveKey          , "void RemoveKey()",
	"RemoveAllKey"     , RemoveAllKey       , "void RemoveAllKey()",
	"SetMark"          , SetMark            , "",
	"GoMark"           , GoMark             , "",

	///////////////////////////////////////////////////////////////////////
	"Say"              , Say                , "void Say()",
 
	////////////////////////////////////////////

	"IsParty"        ,	IsParty        , "",
	"IsPartyMaster"  ,	IsPartyMaster  , "",
	"GetPartyNum"	,	GetPartyNum	,	"",
	"GetPartyLevel"	,	GetPartyLevel,	"",
	"IsPartyGuild"	,	IsPartyGuild	,	"",
	"IsGuild"        ,	IsGuild        , "",
	"IsGuildMaster"  ,	IsGuildMaster  , "",
	"CreateGuild"    ,	CreateGuild    , "",
	"DestroyGuild"   ,	DestroyGuild   , "",

	"PlaySound"        ,  PlaySound, "",
	"GetLang"          ,  GetLang, "",

	"Run"	                , Run                   , "Run"	,	//( dwItemId, num )
	"GetQuestId"            , GetQuestId            , "GetQuestId",
	"BeginQuest"            , BeginQuest            , "BeginQuest",
	"EndQuest"              , EndQuest              , "EndQuest",
	"AddQuestKey"           , AddQuestKey           , "AddQuestKey",
	"SayQuest"              , SayQuest              , "SayQuest",
	"PrintSystemMessage",	PrintSystemMessage,		"PrintSystemMessage",
#ifdef  __V050322_CACHEQUEST

	//"RunQuest"              , RunQuest              , "RunQuest",
	//"RunQuestIdCnt"         , RunQuestIdCnt         , "RunQuestIdCnt",
	//"RunQuestCnt"           , RunQuestCnt           , "RunQuestCnt",
	//"IsBeginQuestCondition" , IsBeginQuestCondition , "IsBeginQuestCondition",
	//"IsEndQuestCondition"   , IsEndQuestCondition   , "IsEndQuestCondition",
	//"SetQuestId"            , SetQuestId            , "SetQuestId",
	//"RunQuestId"            , RunQuestId            , "RunQuestId"  ,
	//"RunNextMyQuest"        , RunNextMyQuest        , "RunNextMyQuest"  ,

	"GetIValue"         , GetIValue         , "void GetIValue(int nIdx)",
	"SetIValue"         , SetIValue         , "void SetIValue(int nIdx,int nVal)",
	"AddIValue"         , AddIValue         , "void AddIValue(int nIdx,int nVal)",

	"GetGValue"				,GetGValue_				,"GetGValue",
	"SetGValue"				,SetGValue				,"SetGValue",
	"AddGValue"				,AddGValue				,"AddGValue",

	"SysMessage"            , SysMessage            , "SysMessage",
	"CreateRespawn"         , CreateRespawn            , "CreateRespawn",
	"CreateMover"             , CreateMover             , "CreateMover",      
	"CreateChar"            , CreateChar            , "CreateChar",   
	"RemoveMover"           , RemoveMover           , "RemoveMover",  
	"RemoveRespawn"         , RemoveRespawn         , "RemoveRespawn",
	"GetDay"                , GetDay                , "GetDay",       
	"GetMin"                , GetMin                , "GetMin",     
	"GetHour"               , GetHour               , "GetHour",      
	"Disguise"              , Disguise              , "Disguise",
	"NoDisguise"            , NoDisguise            , "NoDisguise",
	"GetRealSec"            , GetRealSec            , "GetRealSec",            
	"GetRealMin"            , GetRealMin            , "GetRealMin",             
	"GetRealHour"           , GetRealHour           , "GetRealHour",           
	"GetRealDay"            , GetRealDay            , "GetRealDay",            
	"GetRealMonth"          , GetRealMonth          , "GetRealMonth",           
	"SetTimer"              , SetTimer              , "SetTimer"              ,
	"IsTimeOut"             , IsTimeOut             , "IsTimeOut"             ,
	"LaunchQuest"           , LaunchQuest           , "LaunchQuest",
	"GetParam"              , GetParam              , "GetParam",
	"AddCondKey"            , AddCondKey            , "int AddCondKey( CScript* pScript )",
	"GetInventoryNum"       , GetInventoryNum       , "int GetInventoryNum( CScript* pScript )",
	"GetEmptyInventoryNum"  , GetEmptyInventoryNum  , "int GetEmptyInventoryNum( CScript* pScript )",
	"IsQuestPatrol"        , IsQuestPatrol         , "IsQuestPatrol"  ,
	"IsQuestDialog"          , IsQuestDialog         , "IsQuestDialog"  ,
	"IsQuestKillNPC"        , IsQuestKillNPC        , "IsQuestKillNPC" ,
	"GetQuestTime"          , GetQuestTime          , "GetQuestTime"   ,
	"IsDisguise"            , IsDisguise           , "IsDisguise", 
	"GetMoverIndex"         , GetMoverIndex        , "GetMoverIndex",
	"GetMoverMotion"         , GetMoverMotion        , "GetMoverMotion",
#else // __V050322_CACHEQUEST
//{{AFX
	"GetGlobal0"        , GetGlobal0        , "int GetGloval0()",
	"GetGlobal1"        , GetGlobal1        , "int GetGloval1()",
	"GetGlobal2"        , GetGlobal2        , "int GetGloval2()",
	"GetGlobal3"        , GetGlobal3        , "int GetGloval3()",
	"SetGlobal0"        , SetGlobal0        , "void SetGloval0(int nVal)",
	"SetGlobal1"        , SetGlobal1        , "void SetGloval1(int nVal)",
/	"SetGlobal2"        , SetGlobal2        , "void SetGloval2(int nVal)",
	"SetGlobal3"        , SetGlobal3        , "void SetGloval3(int nVal)",
	"GetGlobal"         , GetGlobal         , "void GetGloval(int nIdx)",
	"SetGlobal"         , SetGlobal         , "void SetGloval(int nIdx,int nVal)",
	"AddGlobal"         , AddGlobal         , "void AddGloval(int nIdx,int nVal)",

	"GetParam1"             , GetParam1             , "GetParam1",
	"GetParam2"             , GetParam2             , "GetParam2",
	"GetParam3"             , GetParam3             , "GetParam3",
	"GetParam4"             , GetParam4             , "GetParam4",
//}}AFX
#endif // __V050322_CACHEQUEST

	"Exit"             , Exit               , "void Exit()",

//#endif	// !( __WORLDSERVER || __DBSERVER )
	0                     ,0                   ,0   
};
//#endif
