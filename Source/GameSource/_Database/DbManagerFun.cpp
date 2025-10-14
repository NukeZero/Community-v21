#include "StdAfx.h"
#include "dbmanager.h"
#include "../DatabaseServer/dploginsrvr.h"
#include "../DatabaseServer/dpcoresrvr.h"
#include "../DatabaseServer/dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include <logging/FLMyTrace.h>

#include "serialnumber.h"

extern	BOOL CheckValidItem( DWORD dwItemId, int nNum );

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "honor.h"

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];


#include "eveschool.h"

#include "..\_CommonDefine\FLStringParsing.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;

static const TCHAR	LOG_REMOVE_QUEST[]	= _T( "query_removequest" );


void CDbManager::GetStrTime( CTime *time, const char *strbuf )
{
	//////////////////////////////////////////////////////////////////////////
	// mirchang_20100706
	if( _tcslen( strbuf ) == 0 )
	{
		CTime tCurrentTime	= CTime::GetCurrentTime();
		*time	= tCurrentTime;

		return;
	}
	//////////////////////////////////////////////////////////////////////////



	char cYear[5] = {0,};
	char cMonth[3] = {0,};
	char cDay[3] = {0,};
	char cHour[3] = {0,};
	char cMin[3] = {0,};
	
	FLStrncpy(cYear, _countof( cYear ), strbuf, 4);
	FLStrncpy(cMonth, _countof( cMonth ), strbuf + 4, 2 );
	FLStrncpy(cDay, _countof( cDay ), strbuf + 6, 2 );
	FLStrncpy(cHour, _countof( cHour ), strbuf + 8, 2 );
	FLStrncpy(cMin, _countof( cMin ), strbuf + 10, 2 );


	//////////////////////////////////////////////////////////////////////////
	// mirchang_20100706
	int nYear, nMonth, nDay, nHour, nMin;
	nYear = nMonth = nDay = nHour = nMin = 0;

	nYear = atoi( cYear );
	nMonth = atoi( cMonth );
	nDay = atoi( cDay );
	nHour = atoi( cHour );
	nMin = atoi( cMin );

	struct tm atm;
	atm.tm_sec = 0;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	atm.tm_mday = nDay;
	atm.tm_mon = nMonth - 1;
	atm.tm_year = nYear - 1900;
	atm.tm_isdst = -1;
	__time64_t tTimeTmp = _mktime64(&atm);

	if( tTimeTmp == -1 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CTime Is Invalid. szTime:[%s], Y:[%04d], M:[%02d], D:[%02d], H:[%02d], M:[%02d]" ), strbuf, nYear, nMonth, nDay, nHour, nMin );

		CTime tCurrentTime	= CTime::GetCurrentTime();
		*time	= tCurrentTime;
	}
	else
	{

		CTime Timebuf( nYear, nMonth, nDay, nHour, nMin, 0 );
		*time = Timebuf;
	}
	//////////////////////////////////////////////////////////////////////////

	return;
}

void CDbManager::GetDBFormatStr( char* szDst, size_t cchDst, const char* szSrc )
{
	char szDigit[3] = {0, };
	int ch2;
	char* pCur	= const_cast<char*>( szSrc );
	size_t nLen	= strlen( pCur );
	if( nLen % 2 != 0 )
	{
		pCur[nLen-1]	= '\0';
		nLen--;
	}
	if( nLen >= cchDst * 2 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nLen = %d, nMaxLen = %d" ), nLen, cchDst );
		*szDst	= '\0';
		return;
	}
	while( *pCur )
	{
		szDigit[0]	= pCur[0];
		szDigit[1]	= pCur[1];
		pCur	+= 2;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		sscanf_s( szDigit, "%2X", &ch2 );
#else
		sscanf( szDigit, "%2X", &ch2 );
#endif
		*szDst++	= static_cast< char >( ch2 );
	}
	*szDst	= '\0';
}

void CDbManager::SetDBFormatStr( char* szDst, size_t cchDst, const char* szSrc )
{
	size_t nLen	= strlen( szSrc );
	if( cchDst < nLen * 2 + 1 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nLen = %d, nMaxLen = %d" ), nLen, cchDst );
		*szDst	= '\0';
		return;
	}

	char* pCur	= szDst;
	while( *szSrc && cchDst >= 3 )
	{
		FLSPrintf( pCur, cchDst, "%2X",  (BYTE)*szSrc );
		pCur	+= 2;
		cchDst -= 2;
		szSrc++;
	}
	*pCur	= '\0';
}


// szSrc - 16진수로 변환된 문자열, szDst - ascii문자열, n - szSrc에서 현재작업 index 
void CDbManager::GetStrFromDBFormat( char* szDst, const char* szSrc, int& n )
{
	char szDigit[3] = {0, };
	char ch;
	int ch2;
	
	const char* pCur = szSrc + n;
	while( *pCur != '/' && *pCur )		// 문자열은 '/' 로 끝난다. 안전하게 NULL도 검사 
	{
		szDigit[0] = pCur[0];
		szDigit[1] = pCur[1];
		pCur += 2;

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		sscanf_s( szDigit, "%2X", &ch2 );
#else
		sscanf( szDigit, "%2X", &ch2 );
#endif
		ch = static_cast< char >( ch2 );
		*szDst++ = ch;
	}
	*szDst = '\0';
	n = (pCur - szSrc) + 1;				// +1은 '/'를 무시하기 위해서 
}

BOOL CDbManager::GetBank( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nSlot )
{
	int CountStr				= 0;
	int itemCount				= 0;
	int	IndexItem				= 0;
	int IndexObjIndex			= 0;
	int nExtBankCount			= 0;
	int nPirecingBankCount		= 0;
	char Bank[7500]				= { 0, };
	char m_apIndexBank[512]		= { 0, };
	char ObjIndexBank[512]		= { 0, };
	char ExtBank[2000]			= {0,};	
	char PirecingBank[4000]		= {0,};
	char szPet[4200]	= { 0,};

	u_long uPlayerid	= (u_long)qry->GetInt( "m_idPlayer" );
	DWORD dwGold = qry->GetInt( "m_dwGoldBank" );

	qry->GetStr( "m_Bank", Bank, _countof( Bank ) );
	qry->GetStr( "m_apIndex_Bank", m_apIndexBank, _countof( m_apIndexBank ) );
	qry->GetStr( "m_dwObjIndex_Bank", ObjIndexBank, _countof( ObjIndexBank ) );
	qry->GetStr( "m_extBank", ExtBank, _countof( ExtBank ) );
	qry->GetStr( "m_BankPiercing", PirecingBank, _countof( PirecingBank ) );
	qry->GetStr( "szBankPet", szPet, _countof( szPet ) );

	VERIFYSTRING_RETURN( Bank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( m_apIndexBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( ObjIndexBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( ExtBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( PirecingBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( szPet, lpDbOverlappedPlus->AccountInfo.szPlayer );
	
	pMover->m_idPlayerBank[nSlot] = uPlayerid;
	pMover->m_dwGoldBank[nSlot]	= dwGold;
	
	CountStr	= 0;	IndexItem	= 0;			
	while( '$' != Bank[CountStr] )
	{
		FLItemElem BufItemElem;
		IndexItem = GetOneItem( &BufItemElem, Bank, &CountStr );
		if( IndexItem == -1 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Not Prop. %d, %d \nBank = %s" ), uPlayerid, BufItemElem.m_dwItemId, Bank );
		}
		else
		{
			if( IndexItem >= MAX_BANK )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "MAX_BANK %d\nBank = %s" ), uPlayerid, Bank );
				return FALSE;
			}
			pMover->m_Bank[nSlot].m_apItem[IndexItem] = BufItemElem;
		}
	}

	pMover->m_Bank[nSlot].SetAllocedSerialNumber( GetSerialNumberStatus( Bank, &CountStr ) );
	
	CountStr	= 0;	itemCount	= 0;
	while( '$' != m_apIndexBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apIndex[itemCount]						= (DWORD)GetIntFromStr( m_apIndexBank, &CountStr );
		itemCount++;
	}
	
	CountStr	= 0;	IndexObjIndex	= 0;
	while( '$' != ObjIndexBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apItem[IndexObjIndex].m_dwObjIndex		= (DWORD)GetIntFromStr( ObjIndexBank, &CountStr );
		IndexObjIndex++;
	}

	CountStr	= 0;	nExtBankCount	= 0;
	while( '$' != ExtBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged			= (BOOL)GetIntPaFromStr( ExtBank, &CountStr );
		if( pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged != 1 )
			pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged	= 0;
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_dwKeepTime		= (DWORD)GetIntPaFromStr( ExtBank, &CountStr );
		__int64 iRandomOptItemId	= GetInt64PaFromStr( ExtBank, &CountStr );
		if( iRandomOptItemId == -102 )
			iRandomOptItemId	= 0;

//		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].SetRandomOptItemId( iRandomOptItemId );
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].RandomOptionDataConverting( iRandomOptItemId );
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtBank, &CountStr ) );

		++CountStr;

		++nExtBankCount;
	}

	CountStr	= 0;	nPirecingBankCount = 0;
	while( '$' != PirecingBank[CountStr] )
	{
		LoadPiercingInfo( pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount], PirecingBank, &CountStr );
		++nPirecingBankCount;
	}

	CountStr	= 0;
	int	nId	= 0;
	while( '$' != szPet[CountStr] )
	{
		BOOL bPet	= (BOOL)GetIntFromStr( szPet, &CountStr );
		if( bPet )
		{
			SAFE_DELETE( pMover->m_Bank[nSlot].m_apItem[nId].m_pPet );
			CPet* pPet	= pMover->m_Bank[nSlot].m_apItem[nId].m_pPet		= new CPet;
			BYTE nKind	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetKind( nKind );
			BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetLevel( nLevel );
			DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetExp( dwExp );
			WORD wEnergy	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetEnergy( wEnergy );

			WORD wLife	= (WORD)GetIntPaFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( BYTE i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			GetStrFromStr( szPet, szFmt, &CountStr );
			char szName[MAX_PET_NAME]	= { 0,};
			GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
			pPet->SetName( szName );
		}
		nId++;
	}

	pMover->m_Bank[nSlot].RefreshIndex();
	return TRUE;
}

BOOL CDbManager::GetPocket( CMover* pMover, CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	char	szItem[7500]	= { 0,};
	char	szIndex[512]	= { 0,};
	char	szObjIndex[512]	= { 0,};
	char	szExt[2000]		= { 0,};
	char	szPiercing[4000]	= { 0,};
	char	szPet[4200]	= { 0,};

	pQuery->GetStr( "szItem", szItem, _countof( szItem ) );
	pQuery->GetStr( "szIndex", szIndex, _countof( szIndex ) );
	pQuery->GetStr( "szObjIndex", szObjIndex, _countof( szObjIndex ) );
	pQuery->GetStr( "szExt", szExt, _countof( szExt ) );
	pQuery->GetStr( "szPiercing", szPiercing, _countof( szPiercing ) );
	pQuery->GetStr( "szPet", szPet, _countof( szPet ) );

	char szPlayerName[64] = {0, };
	if( lpDbOverlappedPlus != NULL )
	{
		FLStrcpy( szPlayerName, _countof( szPlayerName ), lpDbOverlappedPlus->AccountInfo.szPlayer );
	}
	else
	{
		FLStrcpy( szPlayerName, _countof( szPlayerName ), pMover->m_szName );
	}
	VERIFYSTRING_RETURN( szItem, szPlayerName );
	VERIFYSTRING_RETURN( szIndex, szPlayerName );
	VERIFYSTRING_RETURN( szObjIndex, szPlayerName );
	VERIFYSTRING_RETURN( szExt, szPlayerName );
	VERIFYSTRING_RETURN( szPiercing, szPlayerName );
	VERIFYSTRING_RETURN( szPet, szPlayerName );

	int nPocket	= pQuery->GetInt( "nPocket" );
	BOOL bExpired	= (BOOL)pQuery->GetInt( "bExpired" );

	if( szItem[0] != '$' || !bExpired )
	{
		pMover->m_Pocket.Avail( nPocket );

// 		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
// 		pPocket->SetExpired( bExpired );
// 		pPocket->SetExpirationDate( (time_t)pQuery->GetInt( "tExpirationDate" ) );

		pMover->m_Pocket.SetExpired( nPocket, bExpired );
		time_t t = static_cast<time_t>( pQuery->GetInt( "tExpirationDate" ) );
		pMover->m_Pocket.SetExpirationDate( nPocket, t );

		int nOffset	= 0, i	= 0;
		while( '$' != szItem[nOffset] )
		{
			FLItemElem itemElem;
			i	= GetOneItem( &itemElem, szItem, &nOffset );
			if( i == -1 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Property not found" ) );
			}
			else
			{
				if( i >= MAX_EXTENSION_POCKET )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "POCKET_SLOT_SIZE_1_2" ) );
					return FALSE;
				}
				pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i]	= itemElem;
			}
		}

		pMover->m_Pocket.m_kPocket[nPocket].SetAllocedSerialNumber( GetSerialNumberStatus( szItem, &nOffset ) );

		nOffset		= i	= 0;
		while( '$' != szIndex[nOffset] )
		{
			pMover->m_Pocket.m_kPocket[nPocket].m_apIndex[i++]		= (DWORD)GetIntFromStr( szIndex, &nOffset );
		}

		nOffset		= i	= 0;
		while( '$' != szObjIndex[nOffset] )
		{
			pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i++].m_dwObjIndex	= (DWORD)GetIntFromStr( szObjIndex, &nOffset );
		}

		nOffset		= i	= 0;
		while( '$' != szExt[nOffset] )
		{
			pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_bCharged		= (BOOL)GetIntPaFromStr( szExt, &nOffset );
			if( pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_bCharged != 1 )
			{
				pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_bCharged		= 0;
			}
			pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_dwKeepTime	= (DWORD)GetIntPaFromStr( szExt, &nOffset );
			__int64 iRandomOptItemId	= GetInt64PaFromStr( szExt, &nOffset );

			pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( szExt, &nOffset ) );

			if( iRandomOptItemId == -102 )
			{
				iRandomOptItemId	= 0;
			}
//			pPocket->m_apItem[i++].SetRandomOptItemId( iRandomOptItemId );
			pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i++].RandomOptionDataConverting( iRandomOptItemId );
			nOffset++;
		}

		nOffset		= i	= 0;
		while( '$' != szPiercing[nOffset] )
		{
			LoadPiercingInfo( pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i], szPiercing, &nOffset );
			i++;
		}

		nOffset		= i	= 0;
		while( '$' != szPet[nOffset] )
		{
			BOOL bPet	= (BOOL)GetIntFromStr( szPet, &nOffset );
			if( bPet )
			{
				SAFE_DELETE( pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_pPet );
				CPet* pPet	= pMover->m_Pocket.m_kPocket[nPocket].m_apItem[i].m_pPet	= new CPet;
				BYTE nKind	= (BYTE)GetIntFromStr( szPet, &nOffset );
				pPet->SetKind( nKind );
				BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &nOffset );
				pPet->SetLevel( nLevel );
				DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &nOffset );
				pPet->SetExp( dwExp );
				WORD wEnergy	= (WORD)GetIntFromStr( szPet, &nOffset );
				pPet->SetEnergy( wEnergy );

				WORD wLife	= (WORD)GetIntPaFromStr( szPet, &nOffset );
				pPet->SetLife( wLife );
				for( BYTE j = PL_D; j <= pPet->GetLevel(); j++ )
				{
					BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &nOffset );
					pPet->SetAvailLevel( j, nAvailLevel );
				}
				char szFmt[MAX_PET_NAME_FMT]	= { 0,};
				GetStrFromStr( szPet, szFmt, &nOffset );
				char szName[MAX_PET_NAME]	= { 0,};
				GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
				pPet->SetName( szName );
			}
			i++;
		}

		pMover->m_Pocket.m_kPocket[nPocket].RefreshIndex();
	}
	return TRUE;
}

BOOL CDbManager::GetInventory( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexItem	= 0;
	char Inven[6000]	= { 0, };
	char szPlayerName[64] = {0, };
	if( lpDbOverlappedPlus != NULL )
	{
		FLStrcpy( szPlayerName, _countof( szPlayerName ), lpDbOverlappedPlus->AccountInfo.szPlayer );
	}
	else
	{
		FLStrcpy( szPlayerName, _countof( szPlayerName ), pMover->m_szName );
	}
	qry->GetStr("m_Inventory", Inven, _countof( Inven ));
	VERIFYSTRING_RETURN( Inven, szPlayerName );	// if false, return
	while('$' != Inven[CountStr])
	{
		FLItemElem BufItemElem;
		IndexItem = GetOneItem( &BufItemElem, Inven, &CountStr );
		if( IndexItem == -1 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Inventory : << 프로퍼티 없음. %s, %d" ), pMover->m_szName, BufItemElem.m_dwItemId );
		}
		else
		{
			if( IndexItem >= MAX_INVENTORY + MAX_HUMAN_PARTS)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "IndexItem %s, %d" ), pMover->m_szName, IndexItem );
				FLERROR_LOG( PROGRAM_NAME, _T( "Inventory = %s" ), Inven );
				return FALSE;
			}
			pMover->m_Inventory.m_apItem[IndexItem] = BufItemElem;
		}
	}

	pMover->m_Inventory.SetAllocedSerialNumber( GetSerialNumberStatus( Inven, &CountStr ) );
	
	CountStr	= 0;
	int itemCount	= 0;
	char m_apIndex[512]		= { 0, };
	qry->GetStr( "m_apIndex", m_apIndex, _countof( m_apIndex ) );
	VERIFYSTRING_RETURN( m_apIndex, szPlayerName );
	while( '$' != m_apIndex[CountStr] )
	{
		pMover->m_Inventory.m_apIndex[itemCount]	= (DWORD)GetIntFromStr( m_apIndex, &CountStr );
		itemCount++;
	}
	
	CountStr	= 0;
	int IndexObjIndex	= 0;
	char ObjIndex[512]	= {0,};
	qry->GetStr( "m_dwObjIndex", ObjIndex, _countof( ObjIndex ) );
	VERIFYSTRING_RETURN( ObjIndex, szPlayerName );
	while( '$' != ObjIndex[CountStr] )
	{
		pMover->m_Inventory.m_apItem[IndexObjIndex].m_dwObjIndex	= (DWORD)GetIntFromStr( ObjIndex, &CountStr );
		IndexObjIndex++;
	}

	CountStr	= 0;
	int nExtInven = 0;
	char ExtInven[2000] = {0,};
	qry->GetStr( "m_extInventory", ExtInven, _countof( ExtInven ) );
	VERIFYSTRING_RETURN( ExtInven, szPlayerName );
	while( '$' != ExtInven[CountStr] )
	{
		pMover->m_Inventory.m_apItem[nExtInven].m_bCharged					= (BOOL)GetIntPaFromStr( ExtInven, &CountStr );
		if( pMover->m_Inventory.m_apItem[nExtInven].m_bCharged != 1 )
			pMover->m_Inventory.m_apItem[nExtInven].m_bCharged	= 0;
		pMover->m_Inventory.m_apItem[nExtInven].m_dwKeepTime				= (DWORD)GetIntPaFromStr( ExtInven, &CountStr );

		//////////////////////////////////////////////////////////////////////////
		// mirchang_101215 기간제 아이템인데 기간셋팅이 안되어 있는 경우..
		if( pMover->m_Inventory.m_apItem[nExtInven].m_dwKeepTime == 0 )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pMover->m_Inventory.m_apItem[nExtInven].m_dwItemId );
			if( pItemProp != NULL )
			{
				if( ( pItemProp->dwFlag & IP_FLAG_PERIOD ) || ( pMover->m_Inventory.m_apItem[nExtInven].m_byFlag & FLItemElem::period ) )
				{
					CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pItemProp->dwSkillTime, 0 );
					pMover->m_Inventory.m_apItem[nExtInven].m_dwKeepTime	= (DWORD)( tm.GetTime() );
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		__int64 iRandomOptItemId	= GetInt64PaFromStr( ExtInven, &CountStr );
		if( iRandomOptItemId == -102 )
			iRandomOptItemId	= 0;
//		pMover->m_Inventory.m_apItem[nExtInven].SetRandomOptItemId( iRandomOptItemId );
		pMover->m_Inventory.m_apItem[nExtInven].RandomOptionDataConverting( iRandomOptItemId );
		pMover->m_Inventory.m_apItem[nExtInven].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtInven, &CountStr ) );

		++CountStr; 	++nExtInven;
	}

	CountStr	= 0;
	int nPirecingInven = 0;
	char PirecingInven[4000] = {0,};
	qry->GetStr( "m_InventoryPiercing", PirecingInven, _countof( PirecingInven ) );
	VERIFYSTRING_RETURN( PirecingInven, szPlayerName );
	while( '$' != PirecingInven[CountStr] )
	{
		LoadPiercingInfo( pMover->m_Inventory.m_apItem[nPirecingInven], PirecingInven, &CountStr );
		++nPirecingInven;
	}

	CountStr	= 0;
	int	nId	= 0;
	char szPet[4200]	= { 0, };
	qry->GetStr( "szInventoryPet", szPet, _countof( szPet ) );
	VERIFYSTRING_RETURN( szPet, szPlayerName );
	while( '$' != szPet[CountStr] )
	{
		BOOL bPet	= (BOOL)GetIntFromStr( szPet, &CountStr );
		if( bPet )
		{
			SAFE_DELETE( pMover->m_Inventory.m_apItem[nId].m_pPet );
			CPet* pPet	= pMover->m_Inventory.m_apItem[nId].m_pPet		= new CPet;
			BYTE nKind	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetKind( nKind );
			BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetLevel( nLevel );
			DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetExp( dwExp );
			WORD wEnergy	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetEnergy( wEnergy );

			WORD wLife	= (WORD)GetIntPaFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( BYTE i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			GetStrFromStr( szPet, szFmt, &CountStr );
			char szName[MAX_PET_NAME]	= { 0,};
			GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
			pPet->SetName( szName );
		}
		nId++;
	}

	pMover->m_Inventory.ConvertInventoryIndex( MAX_INVENTORY_OLD, MAX_HUMAN_PARTS_OLD );
	pMover->m_Inventory.RefreshIndex();

	return TRUE;
}

void CDbManager::LoadPiercingInfo( FLItemElem & itemElem, char* szPirecingInven, int* pLocation )
{
	PT_ITEM_SPEC itemProp = itemElem.GetProp();
	if( itemProp && ( itemProp->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				|| itemProp->IsCostumeEnhanceParts() 
#endif			
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
				|| itemProp->IsBarunaWeapon()
#endif
		))
	{
		itemElem.SetUltimatePiercingSize( GetIntPaFromStr( szPirecingInven, pLocation ) );
		for( size_t i = 0 ; i < itemElem.GetUltimatePiercingSize(); ++i )
			itemElem.SetUltimatePiercingItemID( i, static_cast<DWORD>( GetIntPaFromStr( szPirecingInven, pLocation ) ) );
	}
	itemElem.SetGeneralPiercingSize( GetIntFromStr( szPirecingInven, pLocation ) );
	for( size_t i = 0 ; i < itemElem.GetGeneralPiercingSize(); ++i )
		itemElem.SetGeneralPiercingItemID( i, static_cast<DWORD>( GetIntFromStr( szPirecingInven, pLocation ) ) );

	if( itemElem.IsVisPet() )
	{
		itemElem.SetVisPetPiercingSize( itemElem.GetGeneralPiercingSize() );
		for( size_t i=0; i<itemElem.GetGeneralPiercingSize(); i++ )
			itemElem.SetVisPetPiercingItemKeepTime( i, static_cast<time_t>( GetIntFromStr( szPirecingInven, pLocation ) ) );
	}
}

// 얼터멋 웨폰의 경우 nPiercedSize가 얼터멋 피어싱 사이즈이고 dwItemId1 ~ 5 까지는 얼터멋 피어싱 아이템(보석)이다.
// nPiercedSize2가 일반 피어싱 사이즈이고 dwItemId6 ~ 15 까지가 일반 피어싱 아이템(카드)이다.
// 얼터멋 웨폰이 아닌 경우에는 nPiercedSize가 피어싱 사이즈이고 dwItemId1~10 까지가 피어싱 아이템(카드)이다.
// 이는 MakeQueryAddMail() 함수에도 동일하게 적용된다.
void CDbManager::GetPiercingInfoFromMail( CQuery* pQuery, FLItemElem* pItemElem )
{
	int nAddCount = 1;
	if( pItemElem->GetProp() && (pItemElem->GetProp()->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				|| pItemElem->GetProp()->IsCostumeEnhanceParts() 
#endif			
				|| pItemElem->GetProp()->IsBaruna() 
		))
	{
		if( pQuery->IsValidColumn( "nPiercedSize" ) == true )
		{
			pItemElem->SetUltimatePiercingSize( pQuery->GetInt( "nPiercedSize" ) );
		}

		for( size_t i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
		{
			char szItem[32] = { 0, };
			FLSPrintf( szItem, _countof( szItem ), "dwItemId%d", i + nAddCount );
			if( pQuery->IsValidColumn( szItem ) == true )
			pItemElem->SetUltimatePiercingItemID( i, static_cast<DWORD>( pQuery->GetInt( szItem ) ) );
		}

		if( pQuery->IsValidColumn( "nPiercedSize2" ) == true )
		{
			pItemElem->SetGeneralPiercingSize( pQuery->GetInt( "nPiercedSize2" ) );
		}

		nAddCount = 6;
	}
	else
	{
		if( pQuery->IsValidColumn( "nPiercedSize" ) == true )
		{
			pItemElem->SetGeneralPiercingSize( pQuery->GetInt( "nPiercedSize" ) );
		}
	}

	for( size_t i=0; i<pItemElem->GetGeneralPiercingSize(); i++ )
	{
		char szItem[32] = { 0, };
		FLSPrintf( szItem, _countof( szItem ), "dwItemId%d", i + nAddCount );
		if( pQuery->IsValidColumn( szItem ) == true )
		{
			pItemElem->SetGeneralPiercingItemID( i, static_cast<DWORD>( pQuery->GetInt( szItem ) ) );
		}
	}
}

BOOL CDbManager::GetEquipment( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexEquipment	= 0;
	char Equipment[500]		= {0,};
	qry->GetStr( "m_adwEquipment", Equipment, _countof( Equipment ) );
	VERIFYSTRING_RETURN( Equipment, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != Equipment[CountStr] )
	{
		if( IndexEquipment < MAX_HUMAN_PARTS )
			pMover->m_aEquipInfo[IndexEquipment].dwItemID	= (DWORD)GetIntFromStr( Equipment, &CountStr );
		IndexEquipment++;
	}
	return TRUE;
}

BOOL CDbManager::GetTaskBar( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;

	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
	{
		char AppletTaskBar[MAX_APPLETTASKBAR]	= { 0, };
		qry->GetStr( "m_aSlotApplet", AppletTaskBar, _countof( AppletTaskBar ) );
		VERIFYSTRING_RETURN( AppletTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer );
		while( '$' != AppletTaskBar[CountStr] )
		{
			const int nIndex	= GetIntFromStr( AppletTaskBar, &CountStr );

			if( nIndex >= MAX_SLOT_APPLET )
			{
				break;
			}

			pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwShortcut	= static_cast<DWORD>( GetIntFromStr( AppletTaskBar, &CountStr ) );
			pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwId			= static_cast<DWORD>( GetIntFromStr( AppletTaskBar, &CountStr ) );
			pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwType		= static_cast<DWORD>( GetIntFromStr( AppletTaskBar, &CountStr ) );
			pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwIndex		= static_cast<DWORD>( GetIntFromStr( AppletTaskBar, &CountStr ) );
			pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwUserId		= static_cast<DWORD>( GetIntFromStr( AppletTaskBar, &CountStr ) );
			pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwData		= static_cast<DWORD>( GetIntFromStr( AppletTaskBar, &CountStr ) );
			
			if( pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT )
			{
				GetStrFromDBFormat( pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_szString, AppletTaskBar, CountStr );
			}
		}
	}
	
	CountStr	= 0;
	char ItemTaskBar[MAX_ITEMTASKBAR]	= { 0, };
	qry->GetStr( "m_aSlotItem", ItemTaskBar, _countof( ItemTaskBar ) );
	VERIFYSTRING_RETURN( ItemTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != ItemTaskBar[CountStr] )
	{
		const int nSlotIndex	= GetIntFromStr( ItemTaskBar, &CountStr );
		const int nIndex		= GetIntFromStr( ItemTaskBar, &CountStr );

		if( nSlotIndex >= MAX_SLOT_ITEM_COUNT || nIndex >= MAX_SLOT_ITEM )
		{
			break;
		}

		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut	= static_cast<DWORD>( GetIntFromStr( ItemTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId		= static_cast<DWORD>( GetIntFromStr( ItemTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType		= static_cast<DWORD>( GetIntFromStr( ItemTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwIndex		= static_cast<DWORD>( GetIntFromStr( ItemTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwUserId	= static_cast<DWORD>( GetIntFromStr( ItemTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwData		= static_cast<DWORD>( GetIntFromStr( ItemTaskBar, &CountStr ) );
		
		if( pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT )
		{
			GetStrFromDBFormat( pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, ItemTaskBar, CountStr );
		}
	}
	
	CountStr	= 0;
	char SkillTaskBar[1024]		= { 0, };
	qry->GetStr( "m_aSlotQueue", SkillTaskBar, _countof( SkillTaskBar ) );
	VERIFYSTRING_RETURN( SkillTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != SkillTaskBar[CountStr] )
	{
		const int nIndex	= GetIntFromStr( SkillTaskBar, &CountStr );

		if( nIndex >= MAX_SLOT_QUEUE )
		{
			break;
		}

		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwShortcut		= static_cast<DWORD>( GetIntFromStr( SkillTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwId			= static_cast<DWORD>( GetIntFromStr( SkillTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwType			= static_cast<DWORD>( GetIntFromStr( SkillTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwIndex		= static_cast<DWORD>( GetIntFromStr( SkillTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwUserId		= static_cast<DWORD>( GetIntFromStr( SkillTaskBar, &CountStr ) );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwData			= static_cast<DWORD>( GetIntFromStr( SkillTaskBar, &CountStr ) );
	}
	
	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
	{
		pMover->m_UserTaskBar.m_nActionPoint = qry->GetInt( "m_SkillBar" );
	}
	
	return TRUE;
}

void CDbManager::GetBaseCharacter( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/ )
{
	qry->GetStr( "m_szName", pMover->m_szName, _countof( pMover->m_szName ) );
	pMover->m_vScale.x		= qry->GetFloat( "m_vScale_x" );
	pMover->m_dwMotion		= (DWORD)qry->GetInt( "m_dwMotion" );
	pMover->m_dwWorldID		= qry->GetInt( "dwWorldID");
	pMover->m_vPos.x		= qry->GetFloat( "m_vPos_x" );
	pMover->m_vPos.y		= qry->GetFloat( "m_vPos_y" );
	pMover->m_vPos.z		= qry->GetFloat( "m_vPos_z" );
	pMover->m_fAngle		= qry->GetFloat( "m_fAngle" );
	qry->GetStr( "m_szCharacterKey", pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ) );
	pMover->m_nHitPoint		= (LONG)qry->GetInt( "m_nHitPoint" );
	pMover->m_nManaPoint	= (LONG)qry->GetInt( "m_nManaPoint" );
	pMover->m_nFatiguePoint		= (LONG)qry->GetInt( "m_nFatiguePoint" );
	pMover->m_dwSkinSet		= (DWORD)qry->GetInt( "m_dwSkinSet" );
	pMover->m_dwHairMesh	= (DWORD)qry->GetInt( "m_dwHairMesh" );
	pMover->m_dwHairColor	= (DWORD)qry->GetInt( "m_dwHairColor" );
	pMover->m_dwHeadMesh	= (DWORD)qry->GetInt( "m_dwHeadMesh" );
	pMover->SetSex( (BYTE)qry->GetInt( "m_dwSex" ) );
	pMover->m_dwRideItemIdx		= (DWORD)qry->GetInt( "m_dwRideItemIdx" );
	pMover->SetGold( qry->GetInt("m_dwGold") );
	pMover->m_nJob			= (LONG)qry->GetInt( "m_nJob" );
	pMover->m_idparty		= (u_long)qry->GetInt( "m_idparty" );
	pMover->m_nPKValue			= qry->GetInt( "m_nPKValue" );
	pMover->m_dwPKPropensity	= qry->GetInt( "m_dwPKPropensity" );
	pMover->m_dwPKExp			= qry->GetInt( "m_dwPKExp" );
	pMover->m_nAngelExp		= qry->GetExpInteger( "m_nAngelExp" );
	pMover->m_nAngelLevel	= qry->GetInt( "m_nAngelLevel" );
	
	pMover->m_idMurderer	= (u_long)qry->GetInt( "m_idMuerderer" );
	pMover->m_nFame			= qry->GetInt( "m_nFame" );
	
	int CountStr	= 0;
	char ActMover[50]	= {0,};
	qry->GetStr( "m_pActMover", ActMover, _countof( ActMover ) );
	pMover->m_pActMover->m_dwState	= (DWORD)GetIntFromStr( ActMover, &CountStr );
	pMover->m_pActMover->m_dwStateFlag	= (DWORD)GetIntFromStr( ActMover, &CountStr );
	
	CountStr	= 0;
	pMover->m_Stat.SetOriginStr( qry->GetInt( "m_nStr" ) );
	pMover->m_Stat.SetOriginSta( qry->GetInt( "m_nSta" ) );
	pMover->m_Stat.SetOriginDex( qry->GetInt( "m_nDex" ) );
	pMover->m_Stat.SetOriginInt( qry->GetInt( "m_nInt" ) );
	pMover->m_nLevel	= (LONG)qry->GetInt( "m_nLevel" );
	pMover->m_nExp1		= qry->GetExpInteger( "m_nExp1" );
	pMover->m_nFuel		= qry->GetInt( "m_nFuel" );
	pMover->m_tmAccFuel	= qry->GetInt( "m_tmAccFuel" );
	
//	pMover->m_dwExpertLv	= (DWORD)qry->GetInt( "m_dwExpertLv");
	pMover->m_idMarkingWorld	= (DWORD)qry->GetInt( "m_idMarkingWorld");
	pMover->m_vMarkingPos.x		= qry->GetFloat( "m_vMarkingPos_x" );
	pMover->m_vMarkingPos.y		= qry->GetFloat( "m_vMarkingPos_y" );
	pMover->m_vMarkingPos.z		= qry->GetFloat( "m_vMarkingPos_z" );
	pMover->m_Stat.SetRemainStatPoint( qry->GetInt( "m_nRemainGP" ) );
//	pMover->m_nRemainLP = (long)qry->GetInt( "m_nRemainLP" );
	pMover->m_kMadrigalGiftPoint.nPoint = (long)qry->GetInt( "m_nRemainLP" );

	int nTutorialState	= qry->GetInt( "m_nFlightLv" );
	pMover->SetTutorialState( nTutorialState );

	pMover->m_nFxp	= (long)qry->GetInt( "m_nFxp" );
//	pMover->m_nTxp	= (long)qry->GetInt( "m_nTxp" );
	pMover->m_dwMode	= (DWORD)qry->GetInt( "m_dwMode" );
	pMover->m_bPlayer	= 1;
	pMover->m_nDeathExp	= qry->GetExpInteger( "m_nDeathExp" );
	pMover->m_nDeathLevel = qry->GetInt( "m_nDeathLevel" );
	pMover->m_nSkillLevel	= qry->GetInt( "m_SkillLv" );
	pMover->m_nSkillPoint	= qry->GetInt( "m_SkillPoint" );
	pMover->m_nHonor	= qry->GetInt( "m_nHonor" );

	// Dice
	pMover->m_Stat.SetDiceStr(qry->GetInt("dcStr"));
	pMover->m_Stat.SetDiceDex(qry->GetInt("dcDex"));
	pMover->m_Stat.SetDiceInt(qry->GetInt("dcInt"));
	pMover->m_Stat.SetDiceSta(qry->GetInt("dcSta"));
}

void	CDbManager::GetHonor( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/ )
{
	// 내가 등록한 아이디 가지고 오기
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"usp_Master_Select '%02d','%07d'",
		g_appInfo.dwSys, pMover->m_idPlayer );
	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}
	
	int aTempHonor[3][50]={0,};
//	int nSec = 0;
	while( qry->Fetch() )
	{
		int nSec = qry->GetInt( "sec" );
		nSec--;
		if(nSec > 2 || nSec < 0 )
			return;

		aTempHonor[nSec][0] = qry->GetInt( "c01" );
		aTempHonor[nSec][1] = qry->GetInt( "c02" );
		aTempHonor[nSec][2] = qry->GetInt( "c03" );
		aTempHonor[nSec][3] = qry->GetInt( "c04" );
		aTempHonor[nSec][4] = qry->GetInt( "c05" );
		aTempHonor[nSec][5] = qry->GetInt( "c06" );
		aTempHonor[nSec][6] = qry->GetInt( "c07" );
		aTempHonor[nSec][7] = qry->GetInt( "c08" );
		aTempHonor[nSec][8] = qry->GetInt( "c09" );
		aTempHonor[nSec][9] = qry->GetInt( "c10" );

		aTempHonor[nSec][10] = qry->GetInt( "c11" );
		aTempHonor[nSec][11] = qry->GetInt( "c12" );
		aTempHonor[nSec][12] = qry->GetInt( "c13" );
		aTempHonor[nSec][13] = qry->GetInt( "c14" );
		aTempHonor[nSec][14] = qry->GetInt( "c15" );
		aTempHonor[nSec][15] = qry->GetInt( "c16" );
		aTempHonor[nSec][16] = qry->GetInt( "c17" );
		aTempHonor[nSec][17] = qry->GetInt( "c18" );
		aTempHonor[nSec][18] = qry->GetInt( "c19" );
		aTempHonor[nSec][19] = qry->GetInt( "c20" );

		aTempHonor[nSec][20] = qry->GetInt( "c21" );
		aTempHonor[nSec][21] = qry->GetInt( "c22" );
		aTempHonor[nSec][22] = qry->GetInt( "c23" );
		aTempHonor[nSec][23] = qry->GetInt( "c24" );
		aTempHonor[nSec][24] = qry->GetInt( "c25" );
		aTempHonor[nSec][25] = qry->GetInt( "c26" );
		aTempHonor[nSec][26] = qry->GetInt( "c27" );
		aTempHonor[nSec][27] = qry->GetInt( "c28" );
		aTempHonor[nSec][28] = qry->GetInt( "c29" );
		aTempHonor[nSec][29] = qry->GetInt( "c30" );

		aTempHonor[nSec][30] = qry->GetInt( "c31" );
		aTempHonor[nSec][31] = qry->GetInt( "c32" );
		aTempHonor[nSec][32] = qry->GetInt( "c33" );
		aTempHonor[nSec][33] = qry->GetInt( "c34" );
		aTempHonor[nSec][34] = qry->GetInt( "c35" );
		aTempHonor[nSec][35] = qry->GetInt( "c36" );
		aTempHonor[nSec][36] = qry->GetInt( "c37" );
		aTempHonor[nSec][37] = qry->GetInt( "c38" );
		aTempHonor[nSec][38] = qry->GetInt( "c39" );
		aTempHonor[nSec][39] = qry->GetInt( "c40" );

		aTempHonor[nSec][40] = qry->GetInt( "c41" );
		aTempHonor[nSec][41] = qry->GetInt( "c42" );
		aTempHonor[nSec][42] = qry->GetInt( "c43" );
		aTempHonor[nSec][43] = qry->GetInt( "c44" );
		aTempHonor[nSec][44] = qry->GetInt( "c45" );
		aTempHonor[nSec][45] = qry->GetInt( "c46" );
		aTempHonor[nSec][46] = qry->GetInt( "c47" );
		aTempHonor[nSec][47] = qry->GetInt( "c48" );
		aTempHonor[nSec][48] = qry->GetInt( "c49" );
		aTempHonor[nSec][49] = qry->GetInt( "c50" );
	}

	int nMonster = 0,nItem = 0,nEtc = 0;
	int nType = 0;
	int nCurrentTitleCount =  CTitleManager::Instance()->m_nCurrentTitleCount;
	FLASSERT( nCurrentTitleCount <= MAX_HONOR_TITLE );
	for(int i=0;i<nCurrentTitleCount;i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if( nType == HI_HUNT_MONSTER)
		{
			pMover->SetHonorCount(i,aTempHonor[2][nMonster]);
			nMonster++;
		}
		else if( nType == HI_USE_ITEM)
		{
			pMover->SetHonorCount(i,aTempHonor[1][nItem]);
			nItem++;
		}
		else
		{
			pMover->SetHonorCount(i,aTempHonor[0][nEtc]);
			nEtc++;
		}
	}
}

BOOL CDbManager::GetSkill( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/ )
{
	/************************************************************************/
	/*
	배우지 않은 스킬정보도 DB에 있다.  m_aJobSkill 에 대한 유효성 체크가 없기 때문에 DB 데이터가 잘못됐을 시 문제가 발생한다.
	그래서 DB에서 데이터를 가져오기 전에 먼저 초기화를 시켜주고 DB데이터 중 실제로 스킬포인트를 사용 하여 습득한 스킬에 대한 정보만
	업데이트 시켜준다. 그리고 선행 스킬에 대한 데이터 누락이 있는지 체크하여 누락된 데이터가 있을 시엔 플레이어의 스킬 정보를
	초기화 시켜버린다. 스킬 포인트는 월드서버에 진입했을때 재분배 해준다.
	*/
	/************************************************************************/
	
	if( pMover == NULL )
	{
		return FALSE;
	}

	pMover->InitializePlayerSkill();

	memset( pMover->m_abUpdateSkill, 0, sizeof( pMover->m_abUpdateSkill ) );

	// 내가 등록한 아이디 가지고 오기
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"uspLoadCharacterSkill '%02d','%07d'",
		g_appInfo.dwSys, pMover->m_idPlayer );
	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return FALSE;
	}

	while( qry->Fetch() )
	{
// 		SKILL BufSkill;
// 		DWORD dwSkillPosition;
// 		dwSkillPosition = (DWORD)qry->GetInt( "SkillPosition" );
// 		BufSkill.dwSkill = (DWORD)qry->GetInt( "SkillID" );
// 		BufSkill.dwLevel = (DWORD)qry->GetInt( "SkillLv" );
// 		pMover->m_aJobSkill[dwSkillPosition] = BufSkill;


		const DWORD dwSkillPosition		= static_cast<DWORD>( qry->GetInt( "SkillPosition" ) );
		const DWORD dwSkillID			= static_cast<DWORD>( qry->GetInt( "SkillID" ) );
		const DWORD dwSkillLevel		= static_cast<DWORD>( qry->GetInt( "SkillLv" ) );

		if( dwSkillPosition < 0 || dwSkillPosition >= MAX_SKILL_JOB )
		{
			FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ INVALID SkillPosition. PLAYER_ID(%07d), dwSkillPosition:(%u), MAX(%d) ]" )
				, pMover->m_idPlayer, dwSkillPosition, MAX_SKILL_JOB );
			continue;
		}

		const SkillProp* pSkillProp		= prj.GetSkillProp( dwSkillID );
		if( pSkillProp == NULL )
		{
			FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ INVALID SkillID. PLAYER_ID(%07d), dwSkillID:(%u) ]" )
				, pMover->m_idPlayer, dwSkillID );
			continue;
		}

		if( dwSkillLevel > pSkillProp->dwExpertMax )
		{
			FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ INVALID SkillLv. PLAYER_ID(%07d), dwSkillID:(%u), dwSkillLevel(%u) ]" )
				, pMover->m_idPlayer, dwSkillID, dwSkillLevel );
			continue;
		}

		// 이건 에러는 아니고 배우지 않은 스킬이므로 업데이트 하지 않을뿐임. 마스터 스킬은 특정 레벨 도달시 주어지므로 초기화에서 셋팅된 레벨로 그냥 사용.
		if( dwSkillLevel <= 0 || pSkillProp->dwSkillKind1 == JTYPE_MASTER )
		{
			continue;
		}

		SKILL* const pSkill		= &pMover->m_aJobSkill[ dwSkillPosition ];
		if( pSkill == NULL )
		{
			FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ pSkill is NULL. PLAYER_ID(%07d), dwSkillPosition:(%u) ]" )
				, pMover->m_idPlayer, dwSkillPosition );
			continue;
		}

		if( pSkill->dwSkill != dwSkillID )
		{
			FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ DIFFERENT SKILL ID. PLAYER_ID(%07d), dwSkillPosition:(%u), DB_SKILL_ID(%u), PROP_JOB_SKILL_ID(%u) ]" )
				, pMover->m_idPlayer, dwSkillPosition, dwSkillID, pSkill->dwSkill );
			continue;
		}

		pSkill->dwLevel			= dwSkillLevel;
	}

	if( pMover->CheckPlayerSkill() == false )
	{
		FLERROR_LOG( _T( "MODIFY_SKILL" ), _T( "[ INVALID DB SKILL DATA. INITIALIZE PLAYER SKILL LEVEL. PLAYER_ID(%07d) ]" ), pMover->m_idPlayer );
		pMover->InitializeSkillLevel();
	}

	pMover->CorrectErrorsSkillPoint();

	return TRUE;
}

BOOL CDbManager::GetQuest( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexQuest	= 0;
	char QuestCnt[3072]		= {0,};
	qry->GetStr( "m_lpQuestCntArray", QuestCnt, _countof( QuestCnt ) );
	VERIFYSTRING_RETURN( QuestCnt, pMover->m_szName );
	while( '$' != QuestCnt[CountStr] )
	{
		QUEST BufQuest;
		GetOneQuest( &BufQuest, QuestCnt, &CountStr );
		pMover->m_aQuest[IndexQuest] = BufQuest;
		IndexQuest++;
	}
	pMover->m_nQuestSize = IndexQuest;

	size_t nQuestSize = IndexQuest;
	CountStr	= 0;
	IndexQuest	= 0;
	char CompleteQuest[3072]		= {0,};		// 1024 -> 3072
	qry->GetStr( "m_aCompleteQuest", CompleteQuest, _countof( CompleteQuest ) );
	VERIFYSTRING_RETURN( CompleteQuest, pMover->m_szName );
	while( '$' != CompleteQuest[CountStr] )
	{
		pMover->m_aCompleteQuest[IndexQuest] = (WORD)GetIntFromStr( CompleteQuest, &CountStr ); 
		IndexQuest++;
	}
	pMover->m_nCompleteQuestSize = IndexQuest;

	CountStr = 0;
	IndexQuest = 0;
	char CheckedQuest[100] = {0, };
	qry->GetStr( "m_aCheckedQuest", CheckedQuest, _countof( CheckedQuest ) );
	VERIFYSTRING_RETURN( CheckedQuest, pMover->m_szName );
	while( '$' != CheckedQuest[CountStr] )
	{
		WORD wCheckedQuestId = (WORD)GetIntFromStr( CheckedQuest, &CountStr );
		for( size_t i = 0; i < pMover->m_nQuestSize; ++i )
		{
			if( pMover->m_aQuest[i].m_wId == wCheckedQuestId )
			{
				pMover->m_aCheckedQuest[IndexQuest] = wCheckedQuestId;
				IndexQuest++;
			}
		}
	}
	pMover->m_nCheckedQuestSize = IndexQuest;

	// 기존 것중에 완료된것을 완료 배열에 넣는다.
	for( size_t i = 0; i < nQuestSize; i++ )
	{
		if( pMover->m_aQuest[ i ].m_nState == QS_END )
		{
			size_t j = 0;
			for( ; j < pMover->m_nCompleteQuestSize; j++ )
			{
				if( pMover->m_aCompleteQuest[ j ] == pMover->m_aQuest[ i ].m_wId )
					break;
			}
			if( j == pMover->m_nCompleteQuestSize )
			{
				pMover->m_aCompleteQuest[ pMover->m_nCompleteQuestSize ] = pMover->m_aQuest[ i ].m_wId;
				pMover->m_nCompleteQuestSize++;
			}
			for( j = i; j < nQuestSize - 1; j++ )
			{
				pMover->m_aQuest[ j ] = pMover->m_aQuest[ j + 1 ];
			}

			for( size_t k = 0; k < pMover->m_nCheckedQuestSize; ++k )
			{
				if( pMover->m_aCheckedQuest[ k ] == pMover->m_aQuest[ i ].m_wId )
				{
					for( size_t l = k; l < pMover->m_nCheckedQuestSize - 1; ++l )
						pMover->m_aCheckedQuest[ k ] = pMover->m_aCheckedQuest[ k + 1 ];
					pMover->m_aCheckedQuest[ --pMover->m_nCheckedQuestSize ] = 0;
					break;
				}
			}

			nQuestSize--;
			i--;
		}
		else
		{
			size_t j = 0;
			do
			{
				for( j = i + 1; j < nQuestSize; j++ )
				{
					if( pMover->m_aQuest[ j ].m_wId == pMover->m_aQuest[ i ].m_wId )
						break;
				}
				if( j != nQuestSize )
				{
					for( size_t k = j; k < nQuestSize - 1; k++ )
					{
						pMover->m_aQuest[ k ] = pMover->m_aQuest[ k + 1 ];
					}
					nQuestSize--;
				}
			} while( j != nQuestSize );
		}
	}
	pMover->m_nQuestSize = nQuestSize;
	return TRUE;
}

BOOL CDbManager::GetSMMode( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexSM	= 0;
	char SMCnt[3072]		= {0,};
	qry->GetStr( "m_dwSMTime", SMCnt, _countof( SMCnt ) );
	if( 0 != strcmp( SMCnt, "NULL" ) ) 
	{
		VERIFYSTRING_RETURN( SMCnt, pMover->m_szName );
		while( '$' != SMCnt[CountStr] )
		{
			pMover->m_dwSMTime[IndexSM] = (DWORD)GetIntFromStr( SMCnt, &CountStr );
			switch( IndexSM )
			{
			case SM_MAX_HP50:
				{
					pMover->m_nPlusMaxHitPoint = (LONG)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			case SM_RESIST_ATTACK_LEFT:
				{
					pMover->m_nAttackResistLeft = (BYTE)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			case SM_RESIST_ATTACK_RIGHT:
				{
					pMover->m_nAttackResistRight = (BYTE)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			case SM_RESIST_DEFENSE:
				{
					pMover->m_nDefenseResist = (BYTE)GetIntFromStr( SMCnt, &CountStr );
				}
				break;

			case SM_ATTACK_UP1:
				{
					PT_ITEM_SPEC pItemPropUpCut = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10270, II_CHR_SYS_SCR_UPCUTSTONE ) );
					if( pItemPropUpCut != NULL )
					{
						if( pMover->m_dwSMTime[IndexSM] > pItemPropUpCut->dwCircleTime )
						{
							pMover->m_dwSMTime[IndexSM] = pItemPropUpCut->dwCircleTime;
						}
					}
				}
				break;
			}
			++IndexSM;
		}			
	}
	return TRUE;
}

BOOL CDbManager::GetSKillInfluence( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
//	int CountStr	= 0;
//	int iSkillIndex = 0;
	char szSKillInfluence[7500]		= {0,};
	qry->GetStr( "SkillInfluence", szSKillInfluence, _countof( szSKillInfluence ) );
	VERIFYSTRING_RETURN( szSKillInfluence, pMover->m_szName );

	std::vector<FLSkillStringParsing::T_SKILL_DATA> vecSkillData;
	vecSkillData.clear();

 	if( g_xSkillStringParsing->GetDataFromString( szSKillInfluence, vecSkillData ) == true )
 	{
		for( std::vector<FLSkillStringParsing::T_SKILL_DATA>::iterator it = vecSkillData.begin(); it != vecSkillData.end(); ++it )
		{
			FLSkillStringParsing::T_SKILL_DATA tSkillData = { 0 };
			tSkillData = *it;
			
			IBuff* pBuff	= pMover->m_buffs.CreateBuff( tSkillData.wType );
			pBuff->SetId( tSkillData.wId );
			pBuff->SetLevel( tSkillData.dwLevel );
			pBuff->SetTotal( tSkillData.dwTotal );
			pBuff->SetAdjValue( tSkillData.nAdjValue );
			bool bResult	= pMover->m_buffs.Add( pBuff );
			FLASSERT( bResult );
		}
 	}
	else
	{
		return FALSE;
	}

// 	while( '$' != szSKillInfluence[CountStr] )
// 	{
// 		WORD wType	= (WORD)GetIntFromStr( szSKillInfluence, &CountStr );
// 		WORD wId	= (WORD)GetIntFromStr( szSKillInfluence, &CountStr );
// 		DWORD dwLevel	= (DWORD)GetIntFromStr( szSKillInfluence, &CountStr );
// 		DWORD tmTotal	= GetIntFromStr( szSKillInfluence, &CountStr );
// 
// 		if( wType == 0 && wId == 0 && dwLevel == 0 && tmTotal == 0 )
// 			break;
// 
// 		IBuff* pBuff	= pMover->m_buffs.CreateBuff( wType );
// 		pBuff->SetId( wId );
// 		pBuff->SetLevel( dwLevel );
// 		pBuff->SetTotal( tmTotal );
// 		bool bResult	= pMover->m_buffs.Add( pBuff );
// 		FLASSERT( bResult );
// 		
// 		++iSkillIndex;
// 	}
	return TRUE;
}

int CDbManager::GetOneItem( FLItemElem* pItemElem, char* pstrItem, int *pLocation )
{
	int IndexItem		= 0;
	IndexItem	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_dwItemId	= (DWORD)GetIntPaFromStr( pstrItem, pLocation );
	BOOL bEquip	= GetIntPaFromStr( pstrItem, pLocation );	// == 0
	UNREFERENCED_PARAMETER( bEquip );
	int nTrade	= GetIntPaFromStr( pstrItem, pLocation );	// == 0
	UNREFERENCED_PARAMETER( nTrade );
	GetStrPaFromStr( pstrItem, pItemElem->m_szItemText, pLocation );	// == ""
	pItemElem->m_nItemNum	= GetIntPaFromStr( pstrItem, pLocation );

	if( CheckValidItem( pItemElem->m_dwItemId, pItemElem->m_nItemNum ) == FALSE )
		pItemElem->m_nItemNum	= 1;

	pItemElem->m_nRepairNumber	= (BYTE)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nHitPoint	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nRepair	= GetIntPaFromStr( pstrItem, pLocation );
	int nMaterial	= GetIntPaFromStr( pstrItem, pLocation );
	UNREFERENCED_PARAMETER( nMaterial );
	pItemElem->m_byFlag		= (BYTE)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->SetSerialNumber( GetSerialNumberPaFromStr( pstrItem, pLocation ) );
	pItemElem->SetAbilityOption( GetIntPaFromStr( pstrItem, pLocation ) );
	pItemElem->m_byItemResist	= (BYTE)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nResistAbilityOption	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_idGuild = (u_long)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nResistSMItemId = GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_dwObjId		= IndexItem;

	if( pItemElem->m_dwItemId == ITEM_INDEX( 26476, II_SYS_SYS_SCR_SEALCHARACTER ) )
	{
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pItemElem->m_nHitPoint );

		if( lpszPlayer != NULL)
			memcpy(pItemElem->m_szItemText,lpszPlayer,sizeof(pItemElem->m_szItemText));

		CPlayerDataCenter::GetInstance()->m_Access.Leave();

	}

	++*pLocation;
	return IndexItem;
}

void CDbManager::GetOneSkill( LPSKILL pSkill, char* pstrSkill, int *pLocation )
{
	(DWORD)GetIntPaFromStr( pstrSkill, pLocation );
	pSkill->dwLevel	= (DWORD)GetIntPaFromStr( pstrSkill, pLocation );
	pSkill->dwSkill	= (DWORD)GetIntPaFromStr( pstrSkill, pLocation );
	(BOOL)GetIntPaFromStr( pstrSkill, pLocation );
	
	++*pLocation;
}

void CDbManager::GetOneQuest( LPQUEST pQuest, char* pstrQuest, int *pLocation )
{
	pQuest->m_wId				= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// 5
	pQuest->m_nState			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 3
	pQuest->m_wTime				= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// 6?

	pQuest->m_nKillNPCNum[0]	= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// 5
	pQuest->m_nKillNPCNum[1]	= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// 5
	pQuest->m_bPatrol			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve2			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve3			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve4			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve5			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve6			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve7			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_bReserve8			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// 2
	pQuest->m_nKillNPCNum[2]	= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// 5
	pQuest->m_nKillNPCNum[3]	= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// 5	total size 50

	++*pLocation;
}

BOOL	CDbManager::GetSerialNumberStatus( char* pstrInven, int* pLocation )
{
	//////////////////////////////////////////////////////////////////////////
	// 시리얼넘버 변환 테그 확인
	BOOL bNewSerialNumber = FALSE;

	if( CSerialNumber::GetInstance()->IsManaged() == false )
	{
		bNewSerialNumber = TRUE;
	}
	else
	{
		if( pstrInven != NULL && pLocation != NULL )
		{
			if( ++(*pLocation) < 0 )
			{
				return bNewSerialNumber;
			}

			if( ::isalnum( static_cast<unsigned char>( pstrInven[*pLocation] ) ) )
			{
				if( '$' != pstrInven[*pLocation] )
				{
					int nSerialNumberStatus = GetIntPaFromStr( pstrInven, pLocation );
					if( nSerialNumberStatus == g_appInfo.nSerialNumberStatus )
					{
						bNewSerialNumber = TRUE;
					}
				}
			}
		}
	}

	return bNewSerialNumber;
	//////////////////////////////////////////////////////////////////////////
}

void	CDbManager::SaveSerialNumberStatus( char* pstrInven, size_t cchInven, BOOL bNewSerialNumber )
{
	if( CSerialNumber::GetInstance()->IsManaged() == true )
	{
		if( pstrInven != NULL )
		{
			int nSerialNumberStatus = 0;
			if( bNewSerialNumber == TRUE )
			{
				nSerialNumberStatus = g_appInfo.nSerialNumberStatus;
			}

			//////////////////////////////////////////////////////////////////////////
			char szSerialNumberStatus[32] = { 0, };
			FLSPrintf( szSerialNumberStatus, _countof( szSerialNumberStatus ), "%d%s", nSerialNumberStatus, NullStr );
			FLStrncat( pstrInven, cchInven, szSerialNumberStatus, _countof( szSerialNumberStatus ) );
			//////////////////////////////////////////////////////////////////////////
		}
	}
}

BOOL CDbManager::GetRemoveItem( CQuery *pQry, int &nNo, char* pOneItem, size_t nOneItemCount, int &nItem_Count, int &nAbilityOption, int &nItemResist, int &nResistAbilityOption, char &chState )
{
	nNo = pQry->GetInt( "m_nNo" );									// 유니크 넘버
	pQry->GetStr( "Item_Name", pOneItem, nOneItemCount );							// 이름
	
	nItem_Count	= pQry->GetInt( "Item_count" );						// 갯수
	nAbilityOption = pQry->GetInt( "m_nAbilityOption" );			// AbilityOption ( + ) Option
	chState = pQry->GetChar( "State" );
	nItemResist = pQry->GetInt( "m_bItemResist" );					// 속성값	
	nResistAbilityOption = pQry->GetInt( "m_nResistAbilityOption" );// 속성에 대한 Option값
	
	if( 0 == strcmp( pOneItem, "NULL") )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Remove NULL Item_Name : %s" ), pOneItem );
		return FALSE;
	}

	if( chState != 'I' && chState != 'B'
		&& chState != 'P'
		)
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Remove Not Data State : %c" ), chState );
		return FALSE;
	}

	
	if( nItem_Count == -100 || nItem_Count == 0 )
		nItem_Count = 1;
	
	if( nAbilityOption == -100 )
		nAbilityOption = 0;
	
	if( nItemResist == -100 )
		nItemResist = 0;
	
	if( nResistAbilityOption == -100 )
		nResistAbilityOption = 0;

	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

BOOL CDbManager::RemovePenya( CQuery *pQry, CMover* pMover, int nItem_Count, int nNo, char chState )
{
	if( RemoveItemDeleteQuery( pQry, nNo ) == FALSE )
		return FALSE;

	if( chState == 'I' )	// Inventory
	{
		pMover->AddGold( -nItem_Count, FALSE );
	}
	else	// Bank
	{
		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pMover->m_nDBDataSlot );
		DWORD dwGoldBank	= pMover->m_dwGoldBank[byProgramDataSlot];
		pMover->m_dwGoldBank[byProgramDataSlot]	-= nItem_Count;
		if( dwGoldBank < pMover->m_dwGoldBank[byProgramDataSlot] )
		{
			pMover->m_dwGoldBank[byProgramDataSlot]	= 0;
		}
	}

	return TRUE;
}

SERIALNUMBER CDbManager::RemoveItem( CQuery *pQry, int nNo, CMover* pMover, const char* szItemName, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState )
{
	SERIALNUMBER iSerialNumber = 0;

	int nIndex = atoi( szItemName );
	PT_ITEM_SPEC pItemProp = NULL;
	if( nIndex > 0 )
		pItemProp = g_xSpecManager->GetSpecItem( nIndex );	
	else
		pItemProp = g_xSpecManager->GetSpecItem( szItemName );	

	if( pItemProp )
	{
		if( RemoveItemDeleteQuery( pQry, nNo ) == FALSE )
			return 0;

		switch( chState )
		{
			case 'I' :
				iSerialNumber	= RemoveItemInventory( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
				break;
			
			case 'B' :
				iSerialNumber	= RemoveItemBank( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
				break;

			case 'P' :
				iSerialNumber	= RemoveItemPocket( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
				break;
		}
		
		//if( iSerialNumber == 0 )
		//	return 0;
		//else
		return iSerialNumber;
	}
//	else
//	{	// no propItem		
//		return 0;
//	}
	return 0;
}

// END110119
//////////////////////////////////////////////////////////////////////////

SERIALNUMBER CDbManager::RemoveItemInventory( PT_ITEM_SPEC pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount )
{
	SERIALNUMBER iSerialNumber = 0;
	FLItemElem *pItemElem = pMover->m_Inventory.GetItem( pItemProp, nAbilityOption, nItemResist, nResistAbilityOption );
	
	if( pItemElem != NULL ) // 인벤토리에 아이템이 있음을 검사.
	{
		iSerialNumber = pItemElem->GetSerialNumber();
		if( pMover->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )	// 장비를 착용하고 있는지 검사
		{
			if( !pMover->m_Inventory.UnEquip( pItemProp->dwParts ) ) // 장비 해제
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Not UnEquip : %s, %s" ), pMover->m_szName, pItemProp->szName );
				return (SERIALNUMBER)0;
			}
			else
				pMover->m_aEquipInfo[pItemProp->dwParts].dwItemID	= 0;
		}

		if( pItemElem->m_nItemNum <= nItemCount )
			pMover->m_Inventory.RemoveAt( pItemElem->m_dwObjIndex );
		else
			pItemElem->m_nItemNum -= nItemCount;
		
		return iSerialNumber;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "Not Inventory : %s, %s" ), pMover->m_szName, pItemProp->szName );
	return 0;
}

SERIALNUMBER CDbManager::RemoveItemBank( PT_ITEM_SPEC pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount )
{
	SERIALNUMBER iSerialNumber = 0;

	const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( pMover->m_nDBDataSlot );
	FLItemElem *pItemElem	= pMover->m_Bank[byProgramDataSlot].GetItem( pItemProp, nAbilityOption, nItemResist, nResistAbilityOption );

	if( pItemElem != NULL )
	{
		iSerialNumber	= pItemElem->GetSerialNumber();

		if( pItemElem->m_nItemNum <= nItemCount )
		{
			pMover->m_Bank[byProgramDataSlot].RemoveAt( pItemElem->m_dwObjIndex );
		}
		else
		{
			pItemElem->m_nItemNum -= nItemCount;
		}

		return iSerialNumber;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "item not found - bank: %s, %s" ), pMover->m_szName, pItemProp->szName );
	return 0;
}

SERIALNUMBER CDbManager::RemoveItemPocket( PT_ITEM_SPEC pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount )
{
	SERIALNUMBER iSerialNumber = 0;
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( pMover->m_Pocket.m_kPocket[nPocket].IsAlloced() == TRUE )
		{
			FLItemElem* pItemElem = pMover->m_Pocket.m_kPocket[nPocket].GetItem( pItemProp, nAbilityOption, nItemResist, nResistAbilityOption );
			if( pItemElem != NULL )
			{
				iSerialNumber	= pItemElem->GetSerialNumber();

				if( pItemElem->m_nItemNum <= nItemCount )
				{
					pMover->m_Pocket.m_kPocket[nPocket].RemoveAt( pItemElem->m_dwObjIndex );
				}
				else
				{
					pItemElem->m_nItemNum -= nItemCount;
				}

				return iSerialNumber;
			}
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "item not found - Pocket: %s, %s" ), pMover->m_szName, pItemProp->szName );
	return 0;
}
