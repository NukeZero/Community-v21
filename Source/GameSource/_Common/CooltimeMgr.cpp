#include "stdafx.h"
#include "misc.h"
#include "CooltimeMgr.h"
#include "ProjectCmn.h"

CCooltimeMgr::CCooltimeMgr()
{
	ZeroMemory( m_times, sizeof(m_times) );
	ZeroMemory( m_bases, sizeof(m_bases) );
}
	
CCooltimeMgr::~CCooltimeMgr()
{
}

// 쿨타임 아이템의 쿨타임 그룹번호를 얻는다. 
// 같은 그룹의 아이템은 한번 사용하면 모두 같이 쿨타임을 적용받는다.
// 주의: 수정하려면 헤더파일에 MAX_COOLTIME_TYPE를 잘 조절할 것 
DWORD CCooltimeMgr::GetGroup( PT_ITEM_SPEC pItemProp )
{
	DWORD dwCooltimeItemGroup = 0;

	if( pItemProp->dwSkillReady > 0 )
	{
		switch ( pItemProp->dwItemKind2 )
		{
		case IK2_FOOD:
			dwCooltimeItemGroup = 1;
			if( pItemProp->dwItemKind3 == IK3_PILL )
			{
				dwCooltimeItemGroup = 2;
			}
			break;

		case IK2_SKILL:
			dwCooltimeItemGroup = 3;
			break;

//		case IK2_POTION:
//			dwCooltimeItemGroup = 3;
//			break;

		default:
			{
				if( pItemProp->dwID == ITEM_INDEX( 23957, II_GEN_POT_ARTIFACT_NOSFERATU ) )
				{
					dwCooltimeItemGroup	= 4;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23958, II_GEN_POT_ARTIFACT_SUFFERENCE ) )
				{
					dwCooltimeItemGroup	= 5;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23959, II_GEN_POT_ARTIFACT_VANGUARD ) )
				{
					dwCooltimeItemGroup	= 6;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23960, II_GEN_POT_ARTIFACT_VINDICATION ) )
				{
					dwCooltimeItemGroup	= 7;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23961, II_GEN_POT_ARTIFACT_LUMINESCENSE ) )
				{
					dwCooltimeItemGroup	= 8;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23962, II_GEN_POT_ARTIFACT_MISTRAL ) )
				{
					dwCooltimeItemGroup	= 9;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23963, II_GEN_POT_ARTIFACT_DECIMATION ) )
				{
					dwCooltimeItemGroup	= 10;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23964, II_GEN_POT_ARTIFACT_MALEVOLENCE ) )
				{
					dwCooltimeItemGroup	= 11;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23965, II_GEN_POT_ARTIFACT_VIGILENCE ) )
				{
					dwCooltimeItemGroup	= 12;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23966, II_GEN_POT_ARTIFACT_BELIGERENCE ) )
				{
					dwCooltimeItemGroup	= 13;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23967, II_GEN_POT_ARTIFACT_CLARITY ) )
				{
					dwCooltimeItemGroup	= 14;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23968, II_GEN_POT_ARTIFACT_BRAWN ) )
				{
					dwCooltimeItemGroup	= 15;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23969, II_GEN_POT_ARTIFACT_EXPERTISE ) )
				{
					dwCooltimeItemGroup	= 16;
				}
				else if( pItemProp->dwID == ITEM_INDEX( 23970, II_GEN_POT_ARTIFACT_PERCEPTION ) )
				{
					dwCooltimeItemGroup	= 17;
				}
			}
			break;
		}

	}

	return dwCooltimeItemGroup <= MAX_COOLTIME_TYPE ? dwCooltimeItemGroup : 0;
}


// dwGroup을 사용할 수 있는가?
BOOL CCooltimeMgr::CanUse( DWORD dwGroup )
{
	FLASSERT( dwGroup > 0 );
	return g_tmCurrent > GetTime( dwGroup );		// 재사용 시각을 넘으면 사용가능 
}

// 사용한 시각을 기록해서, CanUse에서 판단 근거로 사용한다.
void CCooltimeMgr::SetTime( DWORD dwGroup, DWORD dwCoolTime )
{
	FLASSERT( dwGroup > 0 && dwCoolTime > 0 );
	DWORD dwTick = g_tmCurrent;

	m_times[dwGroup - 1] = dwTick + dwCoolTime;
	m_bases[dwGroup - 1] = dwTick;
}	


