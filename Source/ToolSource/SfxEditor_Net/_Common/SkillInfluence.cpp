#include "stdafx.h"
#include "skillinfluence.h"
#include "defineSkill.h"
#include "..\_UnhandledException\ExceptionHandler.h"

#ifdef __WORLDSERVER
#include "User.h"
extern	CUserMng	g_UserMng;
#endif // worldserver

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __XITEMBUFF
CSkillInfluence::CSkillInfluence()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CSkillInfluence::m_csAddRemove" )
#endif	// __INFOCRITSECT_DEBUG__
{
	Init();
}

//
CSkillInfluence::~CSkillInfluence()
{
	Destroy();
}

//
void	CSkillInfluence::Init( void )
{
	m_pMover		= NULL;
	memset( m_aSkillInfluence, 0, sizeof(m_aSkillInfluence) );
	m_pEmptyNode = NULL;
}

//
// 새로운 스킬 상태를 추가한다.
//
BOOL CSkillInfluence::Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime )
{
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bAdd = FALSE;
	
	if( (int)dwTime <= 0 )
	{
		LPCTSTR szErr = Message( "CSkillInfluence::Set : dwTime=%d, %d %d %d", (int)dwTime, wType, wID, dwLevel );
		return FALSE;
	}
	
	m_AddRemoveLock.Enter( theLineFile );
	
	pNode = Find( wType, wID );		// 이미 같은 스킬상태가 있었는지 찾음.
	
	if( pNode )		// 있었으면 시간만 재 세팅함.
	{
		Set( pNode, pNode->wType, pNode->wID, pNode->dwLevel, dwTime );		// 시작 시간을 재 세팅.
		m_AddRemoveLock.Leave( theLineFile );
		return FALSE;
	}
	else	// 없었으면 추가함.
	{
		int		i = MAX_SKILLINFLUENCE;
		
		if( m_pEmptyNode )
		{
			if( Set( m_pEmptyNode, wType, wID, dwLevel, dwTime ) )
				bAdd = TRUE;
			m_pEmptyNode = NULL;	// 빈곳에 넣었으니 지워줘야 한다.
		} else
		{
			while( i-- )
			{
				pNode = pList++;
				if( (int)pNode->wID > 0 )	continue;	// 비어있는 공간을 찾음.
				
				Set( pNode, wType, wID, dwLevel, dwTime );
				bAdd = TRUE;
				break;
			}
		}
		if( bAdd == FALSE )
		{
		}
	}
	m_AddRemoveLock.Leave( theLineFile );
	return( bAdd );
}


//
BOOL CSkillInfluence::Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime )
{
//	locked
	if( pNode == NULL )
	{
		Error( "CSkillInfluence::Set : pNode가 NULL" );
		return FALSE;
	}

	pNode->wType	= wType;
	pNode->wID		= wID;
	pNode->dwLevel	= dwLevel;
	pNode->tmTime	= timeGetTime();
	pNode->tmCount = dwTime;
#ifdef __WORLDSERVER
	BOOL bCharged = FALSE;
	if( pNode->wType == BUFF_ITEM )
	{
		ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
		if( pItemProp && pItemProp->bCharged ) // 상용화 아이템은 종료가 안되게 수정
			bCharged = TRUE;
	}
	if( bCharged == FALSE && dwTime >= MIN(61) )
		if( m_pMover )
			Message( "SkillInflu::Set : %s %d", m_pMover->GetName(), dwTime );

	g_UserMng.AddSetSkillState( m_pMover, m_pMover, wType, wID, dwLevel, dwTime );

#endif //__WORLDSERVER
	return	TRUE;
}

//
void CSkillInfluence::Serialize( CAr & ar )
{
	int j, nMax = 0;
	DWORD	dwOdd;

	if( ar.IsStoring() )	// Saving
	{
		for( j = 0; j < MAX_SKILLINFLUENCE; j++ )		// 데이타 있는것만 개수 셈.
			if( m_aSkillInfluence[j].wID > 0 )
				nMax ++;

		ar << nMax;
		for( j = 0; j < MAX_SKILLINFLUENCE ; ++j )
		{
			if( m_aSkillInfluence[j].wID > 0 )
			{
				ar << m_aSkillInfluence[j].wType;
				ar << m_aSkillInfluence[j].wID;
				ar << m_aSkillInfluence[j].dwLevel;
#ifdef __WORLDSERVER			
				if( m_aSkillInfluence[j].tmCount )
				{
					if( m_aSkillInfluence[j].tmTime )
						dwOdd = m_aSkillInfluence[j].tmCount - (timeGetTime() - m_aSkillInfluence[j].tmTime);	// 남은시간 계산.
					else
						dwOdd = m_aSkillInfluence[j].tmCount;
				}
				else
					dwOdd = 0;
				if( (int)dwOdd < 0 )		dwOdd = 0;
				ar << dwOdd;		// 남은 시간을 저장.
#else // World
				ar << m_aSkillInfluence[j].tmCount;
#endif // not World
			}
		}
	} else
	// Loading
	{

		memset( m_aSkillInfluence, 0, sizeof(m_aSkillInfluence) );
		m_pEmptyNode = NULL;

		ar >> nMax;
		for( j = 0 ; j < nMax; ++j )		// 있는만큼만 부른다.
		{
			ar >> m_aSkillInfluence[j].wType;
			ar >> m_aSkillInfluence[j].wID;
			ar >> m_aSkillInfluence[j].dwLevel;
			ar >> dwOdd;

#ifdef __WORLDSERVER				
			BOOL bCharged = FALSE;
			if( m_aSkillInfluence[j].wType == BUFF_ITEM )
			{
				ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[j].wID );
				if( pItemProp )
				{
					if( pItemProp->bCharged ) // 상용화 아이템은 종료가 안되게 수정
					bCharged = TRUE;

				}
			}
			if( bCharged == FALSE && m_pMover && dwOdd >= MIN(61) )
			{
				Message( "SkillInflu::>Load : %s %d", m_pMover->GetName(), dwOdd );
				dwOdd = MIN(60);
			}
#endif //__WORLDSERVER
			m_aSkillInfluence[j].tmCount = dwOdd;			// 로딩했을때 남은시간을 로딩해서 세기 시작.
		} // for

	}
}

//
//
void CSkillInfluence::Reset( void )
{
	int i;
	for( i = 0; i < MAX_SKILLINFLUENCE; i ++ )		// 시작타이머를 클리어시켜 버프스킬을 다시 세팅함.
		m_aSkillInfluence[i].tmTime = 0;
}

#ifdef __GM_COMMAND_ITEM
#ifdef __CLIENT
DWORD CSkillInfluence::GetDisguise( void )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	int i;
	for( i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType == BUFF_ITEM )
		{
			ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
			if( pItemProp && pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )
			{
				char szGMCommand[64] = {0,};
				int nGMCountStart = strcspn( pItemProp->szTextFileName, "(" ) + 1;
				int nGMCountEnd = strcspn( pItemProp->szTextFileName, ")" );
				strncpy( szGMCommand, &pItemProp->szTextFileName[nGMCountStart], nGMCountEnd - nGMCountStart );
				MoverProp* pMoverProp	= prj.GetMoverProp( szGMCommand );
				if( pMoverProp )
					return pMoverProp->dwID;
			}
		}
	}
	return NULL_ID;
}
#endif // __CLIENT

BOOL CSkillInfluence::HasLikeItemBuf( DWORD dwItemKind3 )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	return LikeItemBuf( dwItemKind3 );
}

BOOL CSkillInfluence::LikeItemBuf( DWORD dwItemKind3 )
{
	for( int i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType == BUFF_ITEM )
		{
			ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
			if( pItemProp && dwItemKind3 == pItemProp->dwItemKind3 )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CSkillInfluence::RemoveLikeItemBuf( DWORD dwItemKind3 )
{
	for( int i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType == BUFF_ITEM )
		{
			ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
			if( pItemProp && dwItemKind3 == pItemProp->dwItemKind3 )
			{
				RemoveSkillInfluence( m_aSkillInfluence[i].wType, pItemProp->dwID );
			}
		}
	}
}
#endif // __GM_COMMAND_ITEM

#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 

BOOL CSkillInfluence::RemoveAllSkillInfluence()
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	m_AddRemoveLock.Enter( theLineFile );
	
	while( i-- )
	{
		pNode = pList++;

		if( (int)pNode->wID > 0 )
		{
			if( pNode->wType == BUFF_ITEM )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp && pItemProp->bCharged ) // 상용화 아이템은 종료가 안되게 수정
					continue;
			}
			RemoveSkillInfluence( pNode );
		}
	}

	m_AddRemoveLock.Leave( theLineFile );

	return TRUE;
}		

//
// 등록된 스킬상태들을 매 프로세스 검사하여 시간이 지난것들은 삭제한다.
//
void CSkillInfluence::Process( void )
{
	DWORD dwTimeCurrent	= timeGetTime();		// 현재 시간 읽음.
	CMover *pMover = m_pMover;
	ItemProp *pProp = NULL;

	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;

	m_AddRemoveLock.Enter( theLineFile );

	while( i-- )
	{
		pNode = pList++;
		if( (int)pNode->wID <= 0 )	continue;	// 비어있는 공간은 스킵.

		// 현재시각에서 시작시간을 빼면 흐른시간이 나온다. 흐른시간이 스킬지속시간을 넘어서면 삭제.
 #if defined(__WORLDSERVER)
		// 시간이 다되기전에 어떤특정 조건에 의해서 중간해제가 되는경우의 처리.
		if( pMover )
		{
			if( pNode->wType == BUFF_SKILL )
			{
				switch( pNode->wID )
				{
		#ifdef __ACROBAT_0504
				case SI_JST_SUP_POISON:
				case SI_JST_SUP_BLEEDING:
				case SI_JST_SUP_ABSORB:
				case SI_ACR_SUP_YOYOMASTER:
				case SI_JST_YOYO_CRITICALSWING:
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_YOYO )		// 손에들고 있는 무기가 요요가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_RAG_SUP_FASTATTACK:
				case SI_ACR_SUP_BOWMASTER:
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_RANGE_BOW )		// 손에들고 있는 무기가 너클이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
		#endif // __ACROBAT_0504				
				case SI_MER_SUP_IMPOWERWEAPON:		// 
					{
						BOOL bRelease = FALSE;

						CItemElem *pItemElemR = pMover->GetWeaponItem();
						CItemElem *pItemElemL = pMover->GetLWeaponItem();

						if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
						{
							if( (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&		// 양손다 속성이 없을땐
								(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP) )
								bRelease = TRUE;		// 해제
						} else
						if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
						{
							if( pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
								bRelease = TRUE;		// 해제
						} else
						if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
						{
							bRelease = TRUE;		// 이때는 무조건 해제
						}
	//					else
	//						Message( "CSkillInfluence::Process : 뭐냐 이상황은? %s", pMover->GetName() );

						if( bRelease )
						{
							RemoveSkillInfluence( pNode );		// 해제.
							continue;
						}
					}
					break;
				case SI_MER_SUP_BLAZINGSWORD:		// 
				case SI_MER_SUP_SWORDMASTER:		// 
					{
						CItemElem *pItemElem = pMover->GetLWeaponItem();
						if( pItemElem && pItemElem->GetProp()->dwWeaponType != WT_MELEE_SWD )		// 왼손에 무기를 들고 있고 들고 있는 무기가 칼이 아닐때.
						{
							RemoveSkillInfluence( pNode );		// 해제.
							continue;
						}
					}
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_SWD )		// 오른손에들고 있는 무기가 칼이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_MER_SUP_SMITEAXE:		// 
				case SI_MER_SUP_AXEMASTER:		// 
					{
						CItemElem *pItemElem = pMover->GetLWeaponItem();
						if( pItemElem && pItemElem->GetProp()->dwWeaponType != WT_MELEE_AXE )		// 왼손에 무기를 들고 있고 들고 있는 무기가 도끼가 아닐때.
						{
							RemoveSkillInfluence( pNode );		// 해제.
							continue;
						}
					}
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_AXE )		// 손에들고 있는 무기가 도끼가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_BIL_PST_ASMODEUS:
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_KNUCKLE )		// 손에들고 있는 무기가 너클이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				} // switch
			} // BuffSkill
		} else	// pMover
		{
			Message( "CSkillInfluence::Process : pMover가 널 %d", pNode->wID );
		}
 #elif defined(__CLIENT)
 #endif // Client

#ifdef __CLIENT
		if( pMover )	// 이게 NULL일때도 있다 CWndWorld::m_SkillState 에서는 무버가 없다.
		{
			if( pNode->wType == BUFF_ITEM )
				pProp = prj.GetItemProp( pNode->wID );
			else
				pProp = prj.GetSkillProp( pNode->wID );

			if( pProp )
			{
				if( pNode->bEffect == FALSE )	// 이펙트 생성안된거면.
				{
					// 계속 유지되는 이펙트는 dwSfxObj4를 사용한다.
 #ifdef __ACROBAT_0504
					float fSkillTime = (float)pNode->tmCount / 1000.0f;		// nCount를 초단위로 변환.
					pNode->bEffect = TRUE;
					
					if( pProp->dwSfxObj4 != NULL_ID )
						CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), fSkillTime );					
 #else //__ACROBAT_0504
 					switch( pNode->dwSkill )
					{
					case SI_MER_SHIELD_PROTECTION:		// 프로텍션.
					case SI_MER_SHIELD_PANBARRIER:		// 팬바리어.
						{

						float fSkillTime = (float)pNode->tmCount / 1000.0f;		// nCount를 초단위로 변환.
						pNode->bEffect = TRUE;
						// 이펙트 생성.
						CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), fSkillTime );
						break;
						}
					}
 #endif //__ACROBAT_0504		
				}
			}
		}
#endif // CLIENT
		
		if( m_pMover && pNode->tmTime == 0 )		// Loading되고 난후 SetDestParam이 아직 세팅 안됐다.
		{
 #if defined(__CLIENT) || defined(__WORLDSERVER)
			if( pNode->wType == BUFF_SKILL )
			{
				ItemProp *pSkillProp = NULL;
				AddSkillProp *pAddSkillProp = NULL;
				BOOL bRet = m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, 
					pNode->wID, pNode->dwLevel, "CSkillInfluence::Process" );
				if( bRet )
				{
					m_pMover->ApplyParam( m_pMover, pSkillProp, pAddSkillProp, FALSE, 0 );	// SetDestParam적용. 이때는 클라로 따로 보내지 않음.
				}
			} else
			if( pNode->wType == BUFF_ITEM )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp )
				{
					m_pMover->ApplyParam( m_pMover, pItemProp, NULL, FALSE, 0 );	// SetDestParam적용. 이때는 클라로 따로 보내지 않음.
				}
			}
			
 #endif // Client || World
			pNode->tmTime = timeGetTime();
		}

#ifdef __SM_ITEM_2ND
		BOOL bRemove = TRUE;
		if( pNode->wType == BUFF_ITEM )
		{
			ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
			if( pItemProp && pItemProp->dwSkillTime == 999999999 )
				bRemove = FALSE;
		}

		if( bRemove && timeGetTime() - pNode->tmTime >= pNode->tmCount )	// tmCount시간만큼 경과했으면 종료.
#else // __SM_ITEM_2ND
		if( timeGetTime() - pNode->tmTime >= pNode->tmCount )	// tmCount시간만큼 경과했으면 종료.
#endif // __SM_ITEM_2ND
		{
			RemoveSkillInfluence( pNode );
		}
	}
	m_AddRemoveLock.Leave( theLineFile );
}

#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 

void CSkillInfluence::InfluenceMover( WORD wType, WORD wID, BOOL fInfluece )
{
}

#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
void CSkillInfluence::RemoveSkillInfluence( SKILLINFLUENCE *pSkillInfluence )
{
#ifdef __WORLDSERVER
	if( m_pMover->IsPlayer() )
		g_UserMng.AddRemoveSkillInfluence( m_pMover, pSkillInfluence->wType, pSkillInfluence->wID );
//	if( m_pMover->IsPlayer() )
//		((CUser*)m_pMover)->AddRemoveSkillInfluence( pSkillInfluence->wType, pSkillInfluence->wID );
#endif // __WORLDSERVER

	
	if( m_pMover )
		if( pSkillInfluence->wType == BUFF_ITEM )	// 아이템의 지속효과가 다되어 끝냄.
		{
#ifdef __WORLDSERVER
			ItemProp *pItemProp = prj.GetItemProp( pSkillInfluence->wID );
			if( pItemProp )
			{
				m_pMover->ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, TRUE );
				m_pMover->ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, TRUE );
#ifdef __GM_COMMAND_ITEM
				if( 0 < strlen( pItemProp->szTextFileName ) )
				{
					char szGMCommand[64] = {0,};
					CString szGMText = pItemProp->szTextFileName;
					szGMText.Replace( '(', '\"' );
					szGMText.Replace( ')', '\"' ); 
					int nGMCount = szGMText.Find( "/", 1 );
					if( nGMCount != -1 )
						strncpy( szGMCommand, &pItemProp->szTextFileName[nGMCount], szGMText.GetLength() );
					else
						Error( "GMText 해제하는 것이 없음 : %d, %s", pItemProp->dwID, pItemProp->szTextFileName );
					ParsingCommand( szGMCommand, (CMover*)m_pMover, TRUE );
				}
#endif // __GM_COMMAND_ITEM
			}
#endif
		} else
		if( pSkillInfluence->wType == BUFF_SKILL )
			m_pMover->OnEndSkillState( pSkillInfluence->wID, pSkillInfluence->dwLevel );		// 스킬상태가 하나 끝나면 핸들러 호출.
	Remove( pSkillInfluence );
}

// dwSkill을 찾아서 없앰.
BOOL	CSkillInfluence::RemoveSkillInfluence( WORD wType, WORD wID )
{
	SKILLINFLUENCE* pSkillInfluence;
	
	m_AddRemoveLock.Enter( theLineFile );
	
	pSkillInfluence = Find( wType, wID );
	if( pSkillInfluence )
	{
		RemoveSkillInfluence( pSkillInfluence );
		m_AddRemoveLock.Leave( theLineFile );
		return TRUE;
	} else
	{
		m_AddRemoveLock.Leave( theLineFile );
		return FALSE;
	}
	m_AddRemoveLock.Leave( theLineFile );
}

// dwChrState의 상태를 가진 스킬을 찾아 모두 해제함
BOOL	CSkillInfluence::RemoveSkillInfluenceState( DWORD dwChrState )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	m_AddRemoveLock.Enter( theLineFile );

	int	j;
				
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveSkillInfluence" );
				for( j = 0; j < 2; j ++ )
				{
					if( pAddSkillProp->dwDestParam[j] == DST_CHRSTATE )		// DestParam을 검사.
					{
						if( pAddSkillProp->nAdjParamVal[j] & dwChrState )	// dwChrState 상태를 가지고 있는 스킬이면
						{
							RemoveSkillInfluence( pNode );		// 이상태 해제
							bRet = TRUE;
							continue;
						}
					} 
	#ifdef __ACROBAT_0504
					if( dwChrState == CHS_LOOT_SLOW )		// 이동방해 상태해제.
					{
						if( pAddSkillProp->dwDestParam[j] == DST_SPEED &&
							( pAddSkillProp->nAdjParamVal[j] < 0 || pAddSkillProp->dwChgParamVal[j] == 0 ) )
						{
							RemoveSkillInfluence( pNode );		// 이상태 해제
							bRet = TRUE;
							continue;
						}
					}
	#endif // ACROBAT0504
				}
			}
		} else
		if( pNode->wType == BUFF_ITEM )
		{
			if( pNode->wID > 0 )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp )
				{
					for( j = 0; j < 2; j ++ )
					{
						if( pItemProp->dwDestParam[j] == DST_CHRSTATE )		// DestParam을 검사.
						{
							if( pItemProp->nAdjParamVal[j] & dwChrState )	// dwChrState 상태를 가지고 있는 스킬이면
							{
								RemoveSkillInfluence( pNode );		// 이상태 해제
								bRet = TRUE;
								continue;
							}
						} 
	#ifdef __ACROBAT_0504
						if( dwChrState == CHS_LOOT_SLOW )		// 이동방해 상태해제.
						{
							if( pItemProp->dwDestParam[j] == DST_SPEED &&
								( pItemProp->nAdjParamVal[j] < 0 || pItemProp->dwChgParamVal[j] == 0 ) )
							{
								RemoveSkillInfluence( pNode );		// 이상태 해제
								bRet = TRUE;
								continue;
							}
						}
	#endif // ACROBAT0504
					}
				}
			}
		}
	} // loop
	
	m_AddRemoveLock.Leave( theLineFile );
	return bRet;
}

// dwChrState의 상태를 가진 스킬을 찾아 모두 해제함
BOOL	CSkillInfluence::RemoveAllSkillDebuff( void )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	m_AddRemoveLock.Enter( theLineFile );
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveAllSkillDebuff" );
				if( pSkillProp && pSkillProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		} else
		if( pNode->wType == BUFF_ITEM )
		{
			if( pNode->wID > 0 )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp && pItemProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		}
	} // loop
	
	m_AddRemoveLock.Leave( theLineFile );
	return bRet;
}

#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) )

#else // xItemBuff
//{{AFX
CSkillInfluence::CSkillInfluence()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CSkillInfluence::m_csAddRemove" )
#endif	// __INFOCRITSECT_DEBUG__
{
	Init();
}

CSkillInfluence::~CSkillInfluence()
{
	Destroy();
}

void	CSkillInfluence::Init( void )
{
	m_pMover		= NULL;
	memset( m_aSkillInfluence, 0, sizeof(m_aSkillInfluence) );
	m_pEmptyNode = NULL;
}

BOOL CSkillInfluence::Set( SKILLINFLUENCE *pNode, DWORD dwSkill, DWORD dwLevel, DWORD dwTime )
{
//	locked
	if( pNode == NULL )
	{
		Error( "CSkillInfluence::Set : pNode가 NULL" );
		return FALSE;
	}

	pNode->dwSkill	= dwSkill;
	pNode->dwLevel	= dwLevel;
	pNode->tmTime	= timeGetTime();
	pNode->tmCount = dwTime;
#ifdef __WORLDSERVER
	if( dwTime >= MIN(61) )
	{
		if( m_pMover )
		{
			Message( "SkillInflu::Set : %s %d", m_pMover->GetName(), dwTime );
		}
	}
#endif //__WORLDSERVER
	

#ifdef __WORLDSERVER
	g_UserMng.AddSetSkillState( m_pMover, m_pMover, dwSkill, dwLevel, dwTime );
#endif // WorldServer
	
	return	TRUE;
}

void CSkillInfluence::Serialize( CAr & ar )
{
	int j, nMax = 0;
	DWORD	dwOdd;

	if( ar.IsStoring() )	// Saving
	{
		for( j = 0; j < MAX_SKILLINFLUENCE; j++ )		// 데이타 있는것만 개수 셈.
			if( (int)m_aSkillInfluence[j].dwSkill > 0 )
				nMax ++;

		ar << nMax;

		for( j = 0; j < MAX_SKILLINFLUENCE ; ++j )
		{
			if( (int)m_aSkillInfluence[j].dwSkill > 0 )
			{
				ar << m_aSkillInfluence[j].dwSkill;
				ar << m_aSkillInfluence[j].dwLevel;

	#ifdef __WORLDSERVER			
				if( m_aSkillInfluence[j].tmCount )
				{
					if( m_aSkillInfluence[j].tmTime )
						dwOdd = m_aSkillInfluence[j].tmCount - (timeGetTime() - m_aSkillInfluence[j].tmTime);	// 남은시간 계산.
					else
						dwOdd = m_aSkillInfluence[j].tmCount;
				}
				else
					dwOdd = 0;
				if( (int)dwOdd < 0 )		dwOdd = 0;
				ar << dwOdd;		// 남은 시간을 저장.
	#else // World
				ar << m_aSkillInfluence[j].tmCount;
	#endif // not World
			}
		}
	} else
	// Loading
	{

		memset( m_aSkillInfluence, 0, sizeof(m_aSkillInfluence) );
		m_pEmptyNode = NULL;

		ar >> nMax;
		for( j = 0 ; j < nMax; ++j )		// 있는만큼만 부른다.
		{
			ar >> m_aSkillInfluence[j].dwSkill;
			ar >> m_aSkillInfluence[j].dwLevel;
			ar >> dwOdd;
#ifdef __WORLDSERVER
			if( m_pMover && dwOdd >= MIN(61) )
			{
				Message( "SkillInflu::>Load : %s %d", m_pMover->GetName(), dwOdd );
				dwOdd = MIN(60);
			}
#endif //__WORLDSERVER
			m_aSkillInfluence[j].tmCount = dwOdd;			// 로딩했을때 남은시간을 로딩해서 세기 시작.
		} // for

	}
}

//
// 새로운 스킬 상태를 추가한다.
//
BOOL CSkillInfluence::Set( DWORD dwSkill, DWORD dwLevel, DWORD dwTime )
{
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bAdd = FALSE;
	
	if( (int)dwTime <= 0 )
	{
		LPCTSTR szErr = Message( "CSkillInfluence::Set : dwTime=%d, %d %d", (int)dwTime, dwSkill, dwLevel );
		return FALSE;
	}

	m_AddRemoveLock.Enter( theLineFile );

	pNode = Find( dwSkill );		// 이미 같은 스킬상태가 있었는지 찾음.

	if( pNode )		// 있었으면 시간만 재 세팅함.
	{
//		pNode->dwTime	= dwTime;
//		pNode->dwStartTime = timeGetTime();		
		Set( pNode, pNode->dwSkill, pNode->dwLevel, dwTime );		// 시작 시간을 재 세팅.
		m_AddRemoveLock.Leave( theLineFile );
		return FALSE;
	}
	else	// 없었으면 추가함.
	{
		int		i = MAX_SKILLINFLUENCE;

		if( m_pEmptyNode )
		{
			if( Set( m_pEmptyNode, dwSkill, dwLevel, dwTime ) )
				bAdd = TRUE;
			m_pEmptyNode = NULL;	// 빈곳에 넣었으니 지워줘야 한다.
		} else
		{
			while( i-- )
			{
				pNode = pList++;
				if( (int)pNode->dwSkill > 0 )	continue;	// 비어있는 공간을 찾음.

//				InfluenceMover( dwSkill, TRUE );		// ??
	
				Set( pNode, dwSkill, dwLevel, dwTime );
				bAdd = TRUE;
				break;
			}
		}
		if( bAdd == FALSE )
		{
//			Message( "CSkillInfluence::Set : SkillSetting Failed" );
/*			char szBuf[1024], szStr[128];
			memset( szBuf, 0, sizeof(szBuf) );
			SKILLINFLUENCE *p;
			for( i = 0; i < MAX_SKILLINFLUENCE; i ++ )
			{
				p = &m_aSkillInfluence[i];
				sprintf( szStr, "%d:%d %d %d %d %d\r\n", i, p->dwSkill, p->dwLevel, p->tmCount, p->tmTime, p->bEffect );
				strcat( szBuf, szStr );
			}
			Error( szBuf );
			ADDERRORMSG( szBuf ); */
		}
	}
		m_AddRemoveLock.Leave( theLineFile );
	return( bAdd );
}

//
//
void CSkillInfluence::Reset( void )
{
	int i;
	for( i = 0; i < MAX_SKILLINFLUENCE; i ++ )		// 시작타이머를 클리어시켜 버프스킬을 다시 세팅함.
		m_aSkillInfluence[i].tmTime = 0;
}


#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 

BOOL CSkillInfluence::RemoveAllSkillInfluence()
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	m_AddRemoveLock.Enter( theLineFile );
	
	while( i-- )
	{
		pNode = pList++;

		if( (int)pNode->dwSkill > 0 )
			RemoveSkillInfluence( pNode );
	}
	
	m_AddRemoveLock.Leave( theLineFile );
	
	return TRUE;
}		

//
// 등록된 스킬상태들을 매 프로세스 검사하여 시간이 지난것들은 삭제한다.
//

void CSkillInfluence::Process( void )
{
	DWORD dwTimeCurrent	= timeGetTime();		// 현재 시간 읽음.
	CMover *pMover = m_pMover;
	ItemProp *pProp = NULL;

	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;

	m_AddRemoveLock.Enter( theLineFile );

	while( i-- )
	{
		pNode = pList++;
		if( (int)pNode->dwSkill <= 0 )	continue;	// 비어있는 공간은 스킵.

		// 현재시각에서 시작시간을 빼면 흐른시간이 나온다. 흐른시간이 스킬지속시간을 넘어서면 삭제.
 #if defined(__WORLDSERVER)
		if( pMover )
		{
			switch( pNode->dwSkill )
			{
	#ifdef __ACROBAT_0504
			case SI_JST_SUP_POISON:
			case SI_JST_SUP_BLEEDING:
			case SI_JST_SUP_ABSORB:
			case SI_ACR_SUP_YOYOMASTER:
			case SI_JST_YOYO_CRITICALSWING:
				if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_YOYO )		// 손에들고 있는 무기가 요요가 아닐때.
				{
					RemoveSkillInfluence( pNode );		// 해제.
					continue;
				}
				break;
			case SI_RAG_SUP_FASTATTACK:
			case SI_ACR_SUP_BOWMASTER:
				if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_RANGE_BOW )		// 손에들고 있는 무기가 너클이 아닐때.
				{
					RemoveSkillInfluence( pNode );		// 해제.
					continue;
				}
				break;
	#endif // __ACROBAT_0504				
			case SI_MER_SUP_IMPOWERWEAPON:		// 
				{
					BOOL bRelease = FALSE;

					CItemElem *pItemElemR = pMover->GetWeaponItem();
					CItemElem *pItemElemL = pMover->GetLWeaponItem();

					if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
					{
						if( (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&		// 양손다 속성이 없을땐
							(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP) )
							bRelease = TRUE;		// 해제
					} else
					if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
					{
						if( pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
							bRelease = TRUE;		// 해제
					} else
					if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
					{
						bRelease = TRUE;		// 이때는 무조건 해제
					}
//					else
//						Message( "CSkillInfluence::Process : 뭐냐 이상황은? %s", pMover->GetName() );

					if( bRelease )
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
				}
				break;
			case SI_MER_SUP_BLAZINGSWORD:		// 
			case SI_MER_SUP_SWORDMASTER:		// 
				{
					CItemElem *pItemElem = pMover->GetLWeaponItem();
					if( pItemElem && pItemElem->GetProp()->dwWeaponType != WT_MELEE_SWD )		// 왼손에 무기를 들고 있고 들고 있는 무기가 칼이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
				}
				if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_SWD )		// 오른손에들고 있는 무기가 칼이 아닐때.
				{
					RemoveSkillInfluence( pNode );		// 해제.
					continue;
				}
				break;
			case SI_MER_SUP_SMITEAXE:		// 
			case SI_MER_SUP_AXEMASTER:		// 
				{
					CItemElem *pItemElem = pMover->GetLWeaponItem();
					if( pItemElem && pItemElem->GetProp()->dwWeaponType != WT_MELEE_AXE )		// 왼손에 무기를 들고 있고 들고 있는 무기가 도끼가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
				}
				if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_AXE )		// 손에들고 있는 무기가 도끼가 아닐때.
				{
					RemoveSkillInfluence( pNode );		// 해제.
					continue;
				}
				break;
			case SI_BIL_PST_ASMODEUS:
				if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_KNUCKLE )		// 손에들고 있는 무기가 너클이 아닐때.
				{
					RemoveSkillInfluence( pNode );		// 해제.
					continue;
				}
				break;
			}
		} else
		{
			Message( "CSkillInfluence::Process : pMover가 널 %d", pNode->dwSkill );
		}
 #elif defined(__CLIENT)
 #endif // Client

#ifdef __CLIENT
#ifdef __XBUG0813
		if( pMover )	// 이게 NULL일때도 있다 CWndWorld::m_SkillState 에서는 무버가 없다.
		{
			pProp = prj.GetSkillProp( pNode->dwSkill );
			if( pProp )
			{
				if( pNode->bEffect == FALSE )	// 이펙트 생성안된거면.
				{
					// 계속 유지되는 이펙트는 dwSfxObj4를 사용한다.
 #ifdef __ACROBAT_0504
					float fSkillTime = (float)pNode->tmCount / 1000.0f;		// nCount를 초단위로 변환.
					pNode->bEffect = TRUE;
					
					if( pProp->dwSfxObj4 != NULL_ID )
						CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), fSkillTime );					
 #else //__ACROBAT_0504
 					switch( pNode->dwSkill )
					{
					case SI_MER_SHIELD_PROTECTION:		// 프로텍션.
					case SI_MER_SHIELD_PANBARRIER:		// 팬바리어.
						{

						float fSkillTime = (float)pNode->tmCount / 1000.0f;		// nCount를 초단위로 변환.
						pNode->bEffect = TRUE;
						// 이펙트 생성.
						CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), fSkillTime );
						break;
						}
					}
 #endif //__ACROBAT_0504		
				}
			}
		}
#endif // xBug0813
#endif // CLIENT
		
		if( m_pMover && pNode->tmTime == 0 )		// Loading되고 난후 SetDestParam이 아직 세팅 안됐다.
		{

 #if defined(__CLIENT) || defined(__WORLDSERVER)
			ItemProp *pSkillProp = NULL;
			AddSkillProp *pAddSkillProp = NULL;
			BOOL bRet = m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, 
				pNode->dwSkill, pNode->dwLevel, "CSkillInfluence::Process" );
			if( bRet )
			{
				m_pMover->ApplyParam( m_pMover, pSkillProp, pAddSkillProp, FALSE, 0 );	// SetDestParam적용. 이때는 클라로 따로 보내지 않음.
			}
			
 #endif // Client || World
			pNode->tmTime = timeGetTime();
		}
		

		
		if( timeGetTime() - pNode->tmTime >= pNode->tmCount )	// tmCount시간만큼 경과했으면 종료.
		{
			RemoveSkillInfluence( pNode );
		}
	}
	m_AddRemoveLock.Leave( theLineFile );
}

#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 

void CSkillInfluence::InfluenceMover( DWORD dwSkill, BOOL fInfluece )
{
}

#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
void CSkillInfluence::RemoveSkillInfluence(SKILLINFLUENCE *pSkillInfluence)
{
#ifdef __WORLDSERVER
	if( m_pMover->IsPlayer() )
		g_UserMng.AddRemoveSkillInfluence( m_pMover, 1, pSkillInfluence->dwSkill );
#endif // __WORLDSERVER

	
	if( m_pMover )
		m_pMover->OnEndSkillState( pSkillInfluence->dwSkill, pSkillInfluence->dwLevel );		// 스킬상태가 하나 끝나면 핸들러 호출.
	Remove( pSkillInfluence );
}

// dwSkill을 찾아서 없앰.
BOOL	CSkillInfluence::RemoveSkillInfluence( DWORD dwSkill )
{
	SKILLINFLUENCE* pSkillInfluence;
	
	m_AddRemoveLock.Enter( theLineFile );
	
	pSkillInfluence = Find( dwSkill );
	if( pSkillInfluence )
	{
		RemoveSkillInfluence( pSkillInfluence );
		m_AddRemoveLock.Leave( theLineFile );
		return TRUE;
	} else
	{
#ifdef __WORLDSERVER
//		DEBUGOUT2( "CSkillInfluence::RemoveSkillInfluence", "skillErr.txt" );
#endif	// __WORLDSERVER
		m_AddRemoveLock.Leave( theLineFile );
		return FALSE;
	}
	m_AddRemoveLock.Leave( theLineFile );
}

// dwChrState의 상태를 가진 스킬을 찾아 모두 해제함
BOOL	CSkillInfluence::RemoveSkillInfluenceState( DWORD dwChrState )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	m_AddRemoveLock.Enter( theLineFile );
	
	while( i-- )
	{
		pNode = pList++;
		if( (int)pNode->dwSkill > 0 )	// 스킬 있는걸 찾음.
		{
			ItemProp		*pSkillProp;
			AddSkillProp	*pAddSkillProp;
			int				j;
			
			// 스킬 프로퍼티를 꺼내서.
			m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->dwSkill, pNode->dwLevel, "RemoveSkillInfluence" );
			for( j = 0; j < 2; j ++ )
			{
				if( pAddSkillProp->dwDestParam[j] == DST_CHRSTATE )		// DestParam을 검사.
				{
					if( pAddSkillProp->nAdjParamVal[j] & dwChrState )	// dwChrState 상태를 가지고 있는 스킬이면
					{
						RemoveSkillInfluence( pNode );		// 이상태 해제
						bRet = TRUE;
						continue;
					}
				} 
#ifdef __ACROBAT_0504
				if( dwChrState == CHS_LOOT_SLOW )		// 이동방해 상태해제.
				{
					if( pAddSkillProp->dwDestParam[j] == DST_SPEED &&
						( pAddSkillProp->nAdjParamVal[j] < 0 || pAddSkillProp->dwChgParamVal[j] == 0 ) )
					{
						RemoveSkillInfluence( pNode );		// 이상태 해제
						bRet = TRUE;
						continue;
					}
				}
#endif // ACROBAT0504
			}
		}
	}
	
	m_AddRemoveLock.Leave( theLineFile );
	return bRet;
}

#if 1
// 해가되는 모든 디버프 스킬 해제.
BOOL	CSkillInfluence::RemoveAllSkillDebuff( void )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	m_AddRemoveLock.Enter( theLineFile );
	
	while( i-- )
	{
		pNode = pList++;
		if( (int)pNode->dwSkill > 0 )	// 스킬 있는걸 찾음.
		{
			ItemProp		*pSkillProp;
			AddSkillProp	*pAddSkillProp;
			int				j;
			
			// 스킬 프로퍼티를 꺼내서.
			m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->dwSkill, pNode->dwLevel, "RemoveAllSkillDebuff" );
			if( pSkillProp->nEvildoing < 0 )	// 해가되는 스킬
			{
				RemoveSkillInfluence( pNode );	// 노드삭제하고 디버프효과도 해제함
			}
		}
	}
	
	m_AddRemoveLock.Leave( theLineFile );
	return bRet;
}
#endif // 0

#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) )

//}}AFX
#endif // not xItemBuff
