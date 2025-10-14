#include "stdafx.h"
#include "FLStringParsing.h"

FLSkillStringParsing::FLSkillStringParsing()
{

}

FLSkillStringParsing::~FLSkillStringParsing()
{

}

FLSkillStringParsing* FLSkillStringParsing::GetInstance()
{
	static FLSkillStringParsing xSkillStringParsing;

	return	&xSkillStringParsing;
}

bool	FLSkillStringParsing::GetDataFromString( TCHAR* pszSkillString, std::vector<T_SKILL_DATA> & vecSkillData )
{
	int nLen = _tcslen( pszSkillString );

	if( nLen <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Length is Invalid" ) );
		return	false;
	}

	if( pszSkillString[nLen - 1] != '$' )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "String end is not $" ) );

		return false;
	}

	
	int nCount = 0;
	while( pszSkillString[nCount] != '$' )
	{
		//mem_set( &tSkillData, 0, sizeof( tSkillData ) );
		T_SKILL_DATA tSkillData	= { 0 };
		
		tSkillData.wType		= (WORD)GetIntFromStr( pszSkillString, &nCount );

		tSkillData.wId			= (WORD)GetIntFromStr( pszSkillString, &nCount );
		//////////////////////////////////////////////////////////////////////////
		//	BEGIN101109	경험치 증가 중복 사용때문에 만들어졌던 ID 변경
		
		if( tSkillData.wType == BUFF_ITEM )
		{
			if( tSkillData.wId == ITEM_INDEX( 26206, II_SYS_SYS_SCR_AMPESA1 ) || tSkillData.wId == ITEM_INDEX( 26207, II_SYS_SYS_SCR_AMPESA2 ) || tSkillData.wId == ITEM_INDEX( 20659, II_SYS_SYS_SCR_AMPESA3 ) || tSkillData.wId == ITEM_INDEX( 20660, II_SYS_SYS_SCR_AMPESA4 ) )
			{
				tSkillData.wId = ITEM_INDEX( 26205, II_SYS_SYS_SCR_AMPESA );
			}
			else if( tSkillData.wId == ITEM_INDEX( 26209, II_SYS_SYS_SCR_AMPESB1 ) || tSkillData.wId == ITEM_INDEX( 26210, II_SYS_SYS_SCR_AMPESB2 ) || tSkillData.wId == ITEM_INDEX( 20661, II_SYS_SYS_SCR_AMPESB3 ) || tSkillData.wId == ITEM_INDEX( 20662, II_SYS_SYS_SCR_AMPESB4 ) )
			{
				tSkillData.wId = ITEM_INDEX( 26208, II_SYS_SYS_SCR_AMPESB );
			}
			else if( tSkillData.wId == ITEM_INDEX( 26211, II_SYS_SYS_SCR_AMPESC ) || tSkillData.wId == ITEM_INDEX( 26212, II_SYS_SYS_SCR_AMPESC1 ) || tSkillData.wId == ITEM_INDEX( 26213, II_SYS_SYS_SCR_AMPESC2 ) )
			{
				tSkillData.wId = ITEM_INDEX( 26211, II_SYS_SYS_SCR_AMPESC );
			}
			else if( tSkillData.wId == ITEM_INDEX( 26215, II_SYS_SYS_SCR_AMPESD1 ) || tSkillData.wId == ITEM_INDEX( 26216, II_SYS_SYS_SCR_AMPESD2 ) )
			{
				tSkillData.wId = ITEM_INDEX( 26214, II_SYS_SYS_SCR_AMPESD );
			}
			else if( tSkillData.wId == ITEM_INDEX( 30149, II_SYS_SYS_SCR_AMPESS1 ) || tSkillData.wId == ITEM_INDEX( 30150, II_SYS_SYS_SCR_AMPESS2 ) || tSkillData.wId == ITEM_INDEX( 20663, II_SYS_SYS_SCR_AMPESS3 ) || tSkillData.wId == ITEM_INDEX( 20664, II_SYS_SYS_SCR_AMPESS4 ) )
			{
				tSkillData.wId = ITEM_INDEX( 30148, II_SYS_SYS_SCR_AMPESS );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// BEGIN110217 절대시간버프타입 삭제. item spec 의 bAbsoluteTime 사용.
		else if( tSkillData.wType == BUFF_ITEM2 )
		{
			tSkillData.wType = BUFF_ITEM;
		}
		// END110217 절대시간버프타입 삭제. item spec 의 bAbsoluteTime 사용.
		//////////////////////////////////////////////////////////////////////////

		//	END101109	경험치 증가 중복 사용때문에 만들어졌던 ID 변경
		//////////////////////////////////////////////////////////////////////////
		tSkillData.dwLevel		= (DWORD)GetIntFromStr( pszSkillString, &nCount );
		tSkillData.dwTotal		= (DWORD)GetInt64FromStr( pszSkillString, &nCount );
		tSkillData.nAdjValue	= GetIntFromStr( pszSkillString, &nCount );

		++nCount;

		if( tSkillData.wType == 0 && tSkillData.wId == 0 && tSkillData.dwLevel == 0 && tSkillData.dwTotal == 0 )
		{
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		// 기존 티켓 버프 삭제
		if( tSkillData.wType == BUFF_ITEM )
		{
			PT_ITEM_SPEC pSpec	= g_xSpecManager->GetSpecItem( tSkillData.wId );
			if( pSpec == NULL )
			{
				continue;
			}
			else if( pSpec->dwItemKind3 == IK3_TICKET )
			{
				continue;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		vecSkillData.push_back( tSkillData );
	}
	return true;
}

int		FLSkillStringParsing::GetIntFromStr( TCHAR* pBuf, int* pLocation )
{
	TCHAR szBuf[50] = { 0 };
	//mem_set( szBuf, 0, sizeof( szBuf ) );
	int nCount=0;

	if( pBuf[*pLocation] == '/' )
	{
		return 0;
	}

	bool bEnd = false;
	while( pBuf[*pLocation] != ',' && pBuf[*pLocation] != 0 )
	{
		szBuf[nCount] = pBuf[*pLocation];
		nCount++;
		(*pLocation)++;

		if( pBuf[*pLocation] == '/' )
		{
			bEnd = true;
			break;
		}
	}

	szBuf[nCount] = 0;
	if( bEnd == false )
	{
		(*pLocation)++;
	}
	
	return _tstoi( szBuf );
}

__int64		FLSkillStringParsing::GetInt64FromStr( TCHAR* pBuf, int* pLocation )
{
	TCHAR szBuf[50] = { 0 };
	//mem_set( szBuf, 0, sizeof( szBuf ) );
	int nCount=0;

	if( pBuf[*pLocation] == '/' )
	{
		return 0;
	}

	bool bEnd = false;
	while( pBuf[*pLocation] != ',' && pBuf[*pLocation] != 0 )
	{
		szBuf[nCount] = pBuf[*pLocation];
		nCount++;
		(*pLocation)++;

		if( pBuf[*pLocation] == '/' )
		{
			bEnd = true;
			break;
		}
	}

	szBuf[nCount] = 0;
	if( bEnd == false )
	{
		(*pLocation)++;
	}

	return _tstoi64( szBuf );
}



