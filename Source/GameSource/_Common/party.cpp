#include "stdafx.h"
#include "party.h"

#ifdef __CORESERVER
#include "../CoreServer/dpcachesrvr.h"
extern	CDPCacheSrvr	g_DPCacheSrvr;

#include "../CoreServer/dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;

#include "../CoreServer/player.h"
extern	CPlayerMng	g_PlayerMng;
#endif // __CORESERVER

#ifdef __WORLDSERVER
#include "worldmng.h"
#include "../WorldServer/User.h"
extern CWorldMng   g_WorldMng;
//sun: 12, 군주
#include "../WorldServer/slord.h"
#include "../worldserver/LinkMap.h"
#include "../worldserver/FLMapGrid.h"
#include "../worldserver/FLEventArenaGlobal.h"
#endif	// __WORLDSERVER

#include "playerdata.h"	//sun: 11, 캐릭터 정보 통합

MEMPOOLER_IMPLEMENT( CParty, 512 );

#include "InstanceDungeonParty.h"

/*--------------------------------------------------------------------------------*/

CParty::CParty()
{
	m_uPartyId	= 0;
	m_nSizeofMember	= 0;
	memset( m_sParty, 0, sizeof(m_sParty) );
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = m_nTroupeShareItem = 0;
	m_dwKind = PARTY_KIND_BASE;							// 단막극단
	m_nReferens = 0;
	m_nGetItemPlayerId = 0;
	for( int i = 0 ; i < MAX_PARTYMODE ; i++ )
	{
		m_dwModeTime[i] = 0;
	}
	m_idDuelParty = 0;
#ifdef __WORLDSERVER
	m_dwWorldId = 0;
#endif // __WORLDSERVER

	for( DWORD Nth = 0; Nth < PARTY_EFFECT_MAX; ++Nth )
	{
		m_arrPartyEffect[ Nth ] = false;
	}
}

CParty::~CParty()
{
}

void CParty::InitParty()
{
	m_uPartyId	= 0;
	m_nSizeofMember	= 0;
	memset( m_sParty, 0, sizeof(m_sParty) );
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = m_nTroupeShareItem = 0;
	m_dwKind = PARTY_KIND_BASE;							// 단막극단
	m_nReferens = 0;	
	for( int i = 0 ; i < MAX_PTMEMBER_SIZE ; i++ )
	{
		m_aMember[i].m_uPlayerId	= NULL_PLAYER_ID;
		m_aMember[i].m_tTime = CTime::GetCurrentTime();
		m_aMember[i].m_bRemove = FALSE;
	}
}

#ifndef __CORESERVER
CMover* CParty::GetLeader( void )  const
{ 
#ifdef __WORLDSERVER
	CMover *pLeader = (CMover *)g_xWSUserManager->GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
	return pLeader;
#else
	CMover *pLeader = (CMover *)prj.GetUserByID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
	return pLeader;
#endif
}
#endif // __CORESERVER

int CParty::FindMember( u_long uPlayerId )
{
	for( int i = 0; i < m_nSizeofMember; i++ )
		if( m_aMember[i].m_uPlayerId == uPlayerId )
			return i;
	return -1;
}

//sun: 11, 캐릭터 정보 통합
BOOL CParty::NewMember( u_long uPlayerId )
{
	if( IsMember( uPlayerId ) == FALSE && m_nSizeofMember < MAX_PTMEMBER_SIZE )
	{
		m_aMember[m_nSizeofMember].m_uPlayerId = uPlayerId;
		m_aMember[m_nSizeofMember].m_bRemove	= FALSE;
		m_nSizeofMember++;
		return TRUE;
	}
	return FALSE;
}

BOOL CParty::DeleteMember( u_long uPlayerId )
{
	int Findid = FindMember( uPlayerId );
	if( Findid >= 0)
	{
		for( int i = Findid ; i < m_nSizeofMember-1 ; i++ )
		{
			memcpy( &m_aMember[i], &m_aMember[i+1], sizeof(PartyMember) );
		}
		m_nSizeofMember--;
#ifdef __WORLDSERVER
		FLWSUser * pUser		= g_xWSUserManager->GetUserByPlayerID( uPlayerId );	
		if( IsValidObj( pUser ) == TRUE && CInstanceDungeonHelper::GetInstance()->GetDungeonKind( pUser ) == IDKIND_COLOSSEUM )
		{
			CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		}
		else
		{
			CInstanceDungeonParty::GetInstance()->SetPartyLeaveTime( uPlayerId );
		}
#endif // __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

void CParty::ChangeLeader( u_long uLeaderId )
{
	int nIndex = FindMember( uLeaderId );

	if( nIndex <= 0 || nIndex >= MAX_PTMEMBER_SIZE )
	{
		return;
	}

	SwapPartyMember( 0, nIndex );
}

void CParty::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_uPartyId << m_dwKind << m_nSizeofMember;
		ar << m_nLevel << m_nExp << m_nPoint;
		ar << m_nTroupsShareExp << m_nTroupeShareItem;
		ar << m_idDuelParty;
		for( int i = 0 ; i < MAX_PARTYMODE  ; i++ )
		{
			ar << m_dwModeTime[i];
		}
		if( m_dwKind == PARTY_KIND_EXTEND )
			ar.WriteString( m_sParty );
		for( int i = 0 ; i < m_nSizeofMember ; i++ )
		{
			ar << m_aMember[i].m_uPlayerId;
			ar << m_aMember[i].m_bRemove;
		}

		for( size_t Nth = 0; Nth < PARTY_EFFECT_MAX; ++Nth )
		{
			ar << m_arrPartyEffect[ Nth ];
		}
	}
	else
	{
		ar >> m_uPartyId >> m_dwKind >> m_nSizeofMember;
		ar >> m_nLevel >> m_nExp >> m_nPoint;
		ar >> m_nTroupsShareExp >> m_nTroupeShareItem;
		ar >> m_idDuelParty;
		for( int i = 0 ; i < MAX_PARTYMODE  ; i++ )
		{
			ar >> m_dwModeTime[i];
		}
		if( m_dwKind == PARTY_KIND_EXTEND )
			ar.ReadString( m_sParty, _countof( m_sParty ) );
		for( int i = 0 ; i < m_nSizeofMember ; i++ )
		{
			ar >> m_aMember[i].m_uPlayerId;
			ar >> m_aMember[i].m_bRemove;
		}

		for( size_t Nth = 0; Nth < PARTY_EFFECT_MAX; ++Nth )
		{
			ar >> m_arrPartyEffect[ Nth ];
		}
	}
}

bool	CParty::SwapPartyMember( int first, int Second )
{
	if( first == Second
		|| first < 0 || Second < 0
		|| first >= MAX_PTMEMBER_SIZE || Second >= MAX_PTMEMBER_SIZE )
	{
		return false;
	}

	PartyMember PartyMemberBuf;

	memcpy_s( &PartyMemberBuf, sizeof( PartyMemberBuf ), &m_aMember[first], sizeof(PartyMember) );
	memcpy_s( &m_aMember[first], sizeof( m_aMember[first] ), &m_aMember[Second], sizeof(PartyMember) );
	memcpy_s( &m_aMember[Second], sizeof( m_aMember[Second] ), &PartyMemberBuf, sizeof(PartyMember) );

	return true;
}

//sun: 12, 극단유료아이템
DWORD CParty::GetPartyModeTime( int nMode )
{
	return m_dwModeTime[nMode];	
}
void CParty::SetPartyMode( int nMode, DWORD dwSkillTime ,int nCachMode )
{
	if( nCachMode == 1)
        m_dwModeTime[nMode] += dwSkillTime;	
	else
		m_dwModeTime[nMode] = dwSkillTime;	
}

#ifdef __WORLDSERVER
void CParty::SetPartyLevel( FLWSUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp )
{
	g_DPCoreClient.SendPartyLevel( pUser, dwLevel, dwPoint, dwExp );
}
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
void CParty::GetPoint( int nTotalLevel, int nMemberSize, int nDeadLeavel )
{
	if( (nTotalLevel / nMemberSize) - nDeadLeavel <= 20 )
	{
		BOOL bExpResult = TRUE;
		BOOL bSuperLeader = FALSE;
		BOOL bLeaderSMExpUp = FALSE;	//sun: 12, 극단유료아이템

		if( m_dwKind == PARTY_KIND_BASE && m_nLevel >= MAX_PARTYLEVEL )
			bExpResult = FALSE;

		CMover* pMover = GetLeader();
		if( pMover && pMover->HasBuff( BUFF_ITEM, ITEM_INDEX( 26200, II_SYS_SYS_SCR_SUPERLEADERPARTY ) ) )
		{
			bSuperLeader = TRUE;
		}
//sun: 12, 극단유료아이템
		if( pMover && ( pMover->HasBuff( BUFF_ITEM, ITEM_INDEX( 26537, II_SYS_SYS_SCR_PARTYEXPUP01 ) ) || pMover->HasBuff( BUFF_ITEM, ITEM_INDEX( 26538, II_SYS_SYS_SCR_PARTYEXPUP02 ) )
			 || pMover->HasBuff( BUFF_ITEM, ITEM_INDEX( 20296, II_SYS_SYS_SCR_PARTYEXPUP01_01 ) ) ) )
		{
			bLeaderSMExpUp = TRUE;
		}

		if( bExpResult )
//sun: 12, 극단유료아이템
			g_DPCoreClient.SendAddPartyExp( m_uPartyId, nDeadLeavel, bSuperLeader , bLeaderSMExpUp );
	}
}
#else
void CParty::GetPoint( int /*nTotalLevel*/, int /*nMemberSize*/, int /*nDeadLeavel*/ )
{
}
#endif //__WORLDSERVER



#ifdef __WORLDSERVER
void CParty::DoUsePartySkill( u_long uPartyId, u_long uLeaderid, int nSkill )
{

	if( IsLeader( uLeaderid ) && m_dwKind == PARTY_KIND_EXTEND )
	{
		SkillProp* pSkillProp =  prj.GetPartySkill( nSkill );
		if( pSkillProp )
		{
			if( int( GetLevel() - pSkillProp->dwReqDisLV ) >= 0 )
			{
				// 극단유료아이템
				FLWSUser *pLeader = g_xWSUserManager->GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
				if( IsValidObj( pLeader ) == FALSE )
					return;

				//////////////////////////////////////////////////////////////////////////
				// 파스킬풀 아이템 사용 했다면????? 맞아? -> 파스킬풀이 아니라 극단 스킬 증폭의 두루마리 
				int nHasCashSkill = 0;
				if( pLeader->HasBuff( BUFF_ITEM, ITEM_INDEX( 26539, II_SYS_SYS_SCR_PARTYSKILLUP01 ) ) 
					|| pLeader->HasBuff( BUFF_ITEM, ITEM_INDEX( 26540, II_SYS_SYS_SCR_PARTYSKILLUP02 ) )
					|| pLeader->HasBuff( BUFF_ITEM, ITEM_INDEX( 20297, II_SYS_SYS_SCR_PARTYSKILLUP01_01 ) ) )
				{
					if( nSkill == SKILL_INDEX( 5, ST_LINKATTACK )
						|| nSkill == SKILL_INDEX( 6, ST_FORTUNECIRCLE )
						|| nSkill == SKILL_INDEX( 7, ST_STRETCHING )
						|| nSkill == SKILL_INDEX( 8, ST_GIFTBOX ) )
					{
						nHasCashSkill = 1;
					}
				}
				//////////////////////////////////////////////////////////////////////////

// 				int nRemovePoint	= pSkillProp->dwReqPartyPoint;
// 				int nFPoint	= int( GetPoint() - nRemovePoint );

				const int nRemovePoint	= GetReqPointByUseSkill( nSkill );
				const int nFPoint		= GetPoint() - nRemovePoint;

				// 군주의 극단
				// 군주가 극단장 으로써 극단스킬 사용 시,
				// 지속시간 4배 증가(소모 포인트는 동일)
				DWORD dwSkillTime	= pSkillProp->dwSkillTime;
				if( CSLord::Instance()->IsLord( uLeaderid ) )
				{
					dwSkillTime	*= 4;
				}

				if( nFPoint >= 0 )
				{
					switch( nSkill )
					{
					case SKILL_INDEX( 1, ST_CALL ):
						{
							// 단장 중심으로 모여달라는 표시
							for( int i = 0; i < m_nSizeofMember; i ++ )		// 단장(0)에게는 보낼필요 없다.
							{
								FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
								// 단장 어라운드 지역에 있는 사람에게만 보낸다
								if( IsValidObj( pMember ) )
								{
									// 여기도 if( pLeader->IsNearPC( pMember ) ) 검사해야 할 듯.
									// 아니면 맵ID도 같이 보내야 할지 않을까?
									// m_aMember관리방식도 살펴봐야 할듯.
									pMember->AddPartySkillCall( pLeader->GetPos() );		// 각 멤버들에게 단장이 좌표를 전송함.
								}
							}

							// 응답을 받기 전까지 포인트가 차감되지 않기 때문에
							// 필요포인트 검증에 오류가 있다.
							// (예, 현 포인트 1일때 스피어서클->링크어택 연달아 보냈을 경우)
							// 미리 차감하고 보내는 것이 더 좋지 않을까?
							g_DPCoreClient.SendRemovePartyPoint( uPartyId, nRemovePoint );
						}
						break;
					case SKILL_INDEX( 2, ST_BLITZ ):
						{
							if( pLeader->m_idSetTarget != NULL_ID )
							{
								CMover *pT = prj.GetMover( pLeader->m_idSetTarget );
								if( pT && !pT->IsPlayer() )
								{
									// 단장이 타겟으로 집중공격 표시
									for( int i = 0; i < m_nSizeofMember; i ++ )
									{
										FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
										if( IsValidObj( pMember ) )
										{
											// 뭘까?
											if( m_dwModeTime[PARTY_GIFTBOX_MODE] || m_dwModeTime[PARTY_FORTUNECIRCLE_MODE] )
											{
												pMember->AddPartySkillBlitz( pLeader->m_idSetTarget );		// 각 멤버들에게 단장타겟으로 잡은 무버의 아이디를 보냄.
											}
											else
											{
												if( pLeader->IsNearPC( pMember ) )
													pMember->AddPartySkillBlitz( pLeader->m_idSetTarget );		// 각 멤버들에게 단장타겟으로 잡은 무버의 아이디를 보냄.
											}
										}
									}

									g_DPCoreClient.SendRemovePartyPoint( uPartyId, nRemovePoint );
								}
								else
								{
									pLeader->AddSendErrorParty( ERROR_NOTTARGET, SKILL_INDEX( 2, ST_BLITZ ) );
								}
							}
							else
							{
								pLeader->AddSendErrorParty( ERROR_NOTTARGET, SKILL_INDEX( 2, ST_BLITZ ) );
								// 타겟을 안잡았다.
							}
						}
						break;
					case SKILL_INDEX( 3, ST_RETREAT ):
						{
							// 후퇴 표시
							for( int i = 0; i < m_nSizeofMember; i ++ )
							{
								FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
								if( IsValidObj( pMember ) )
								{
									if( m_dwModeTime[PARTY_GIFTBOX_MODE] || m_dwModeTime[PARTY_FORTUNECIRCLE_MODE] )
									{
										pMember->AddHdr( pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT );
									}
									else
									{
										if( pLeader->IsNearPC( pMember ) )
											pMember->AddHdr( pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT );
									}
								}
							}

							g_DPCoreClient.SendRemovePartyPoint( uPartyId, nRemovePoint );
						}
						break;
					case SKILL_INDEX( 4, ST_SPHERECIRCLE ):
						{
							// 크리티컬 확률
							if( pLeader->m_idSetTarget != NULL_ID )
							{
								CMover * pT = prj.GetMover( pLeader->m_idSetTarget );
								if( pT && !pT->IsPlayer() )
								{
									for( int i = 0; i < m_nSizeofMember; i ++ )
									{
										FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
										if( IsValidObj( pMember ) )
										{
											if( m_dwModeTime[PARTY_GIFTBOX_MODE] || m_dwModeTime[PARTY_FORTUNECIRCLE_MODE] )
											{
												pMember->AddHdr( pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE );
												pMember->m_dwFlag |= MVRF_CRITICAL;
											}
											else
											{
												if( pLeader->IsNearPC( pMember ) )
												{
													pMember->AddHdr( pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE );
													pMember->m_dwFlag |= MVRF_CRITICAL;
												}
											}
										}
									}

									g_DPCoreClient.SendRemovePartyPoint( uPartyId, nRemovePoint );
								}
								else
								{
									pLeader->AddSendErrorParty( ERROR_NOTTARGET, SKILL_INDEX( 4, ST_SPHERECIRCLE) );
									// 타겟이 몬스터가 아니다
								}
							}
							else
							{
								pLeader->AddSendErrorParty( ERROR_NOTTARGET, SKILL_INDEX( 4, ST_SPHERECIRCLE ) );
								// 타겟을 안잡았다.
							}
						}
						break;

//sun: 12, 극단유료아이템
					case SKILL_INDEX( 5, ST_LINKATTACK ):
						{
							// 데미지 증가
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_LINKATTACK_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
					case SKILL_INDEX( 6, ST_FORTUNECIRCLE ):
						{
							// 유니크 아이템 발생확률 증가
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_FORTUNECIRCLE_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
					case SKILL_INDEX( 7, ST_STRETCHING ):
						{
							// 쉬는경우 회복속도 높여줌
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_STRETCHING_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
					case SKILL_INDEX( 8, ST_GIFTBOX ):
						{
							// 아이템 양이 두배
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_GIFTBOX_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;

					default:
						break;
					}
				}
				else
				{
					FLWSUser *pLeader = g_xWSUserManager->GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
					if( IsValidObj( pLeader ) )
					{
						pLeader->AddSendErrorParty( ERROR_NOTPARTYPOINT );
					}
					//포인트가 모자라 스킬을사용할수 없습니다.
				}
			}
			else
			{
				FLWSUser *pLeader = g_xWSUserManager->GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
				if( IsValidObj( pLeader ) )
				{
					pLeader->AddSendErrorParty( ERROR_NOTPARTYSKILL );
				}
				//이 스킬은 배우지를 못했습니다.
			}
		}
	}
	else
	{
		// 리더가 아니거나 순회극단이 아닌경우
	}

}
#else
void CParty::DoUsePartySkill( u_long /*uPartyId*/, u_long /*uLeaderid*/, int /*nSkill*/ )	
{

}
#endif	// __WORLDSERVER

//
//
// idDstParty가 this파티가 결투를 시작했다는걸 모든 멤버들에게 알림.
#ifdef __WORLDSERVER
void CParty::DoDuelPartyStart( CParty *pDst )
{
	int		i, j;
	OBJID	pDstMember[ MAX_PTMEMBER_SIZE ] = { NULL_ID };
	//mem_set( pDstMember, 0xff, sizeof(pDstMember) );
	
	m_idDuelParty = pDst->m_uPartyId;		// this파티의 상대
	
	CMover *pMember, *pMember2;

	LPCSTR pszLeader;
	if( pDst->m_dwKind == PARTY_KIND_EXTEND )	// 순회극단 극단 이름 보냄
	{
		pszLeader = pDst->m_sParty;
	}
	else	// 단막극단 일때는 리더이름을 보냄
	{
//sun: 11, 캐릭터 정보 통합
		pszLeader	= CPlayerDataCenter::GetInstance()->GetPlayerString( pDst->GetPlayerId( 0 ) );
	}
	
	if( !pszLeader )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Party PVP 1" ) );
		return;
	}

	for( i = 0; i < m_nSizeofMember; i ++ )		// 극단원들 루프.
	{
		pMember	= (CMover *)g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( pMember ) )
		{
			pMember->m_nDuel = 2;		// 2는 파티듀얼중.
			pMember->m_idDuelParty = m_idDuelParty;
			pMember->m_nDuelState = 104;
			// 상대방 파티의 멤버아이디를 다 꺼냄.
			for( j = 0; j < pDst->m_nSizeofMember; j ++ )
			{
				pMember2 = (CMover *)g_xWSUserManager->GetUserByPlayerID( pDst->m_aMember[j].m_uPlayerId );
				if( IsValidObj(pMember2) )
					pDstMember[j] = pMember2->GetId();
			}
			// 상대 파티에 대한 정보를 우리멤버들에게 보냄
			((FLWSUser *)pMember)->AddDuelPartyStart( pszLeader , pDst->m_nSizeofMember, pDstMember, pDst->m_uPartyId );		// 상대방 멤버의 ID를 다보낸다.
		}
	}
}	

//
// 파티원들에게 bWin에 따라 승/패 사실을 알리고 듀얼을 종료시킴.
//
void CParty::DoDuelResult( CParty *pDuelOther, BOOL bWin, int nAddFame, float fSubFameRatio )
{
	FLWSUser *pMember;
	int		i;
	
	for( i = 0; i < m_nSizeofMember; i ++ )
	{
		pMember = g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( pMember ) )
		{
			pMember->AddDuelPartyResult( pDuelOther, bWin );		// 각 멤버들에게 승/패 사실을 알림. / 상대파티원 리스트도 보냄.
			pMember->ClearDuelParty();

			if( bWin )
			{
				pMember->m_nFame += nAddFame;	// 이긴측이면 명성 증가
				pMember->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nAddFame );	// xxx 명성 포인트가 증가
#ifdef __WORLDSERVER
				g_dpDBClient.SendLogPkPvp( pMember, NULL, nAddFame, 'E' );
#endif // __WORLDSERVER
			} 
			else
			{
				int nDecVal;
				fSubFameRatio = fSubFameRatio / 100.0f;	// 비율로 환산.
				nDecVal = (int)(pMember->m_nFame * fSubFameRatio);
				if( nDecVal == 0 )	nDecVal = 1;
				pMember->m_nFame -= nDecVal;
				if( pMember->m_nFame < 0 )	pMember->m_nFame = 0;
				pMember->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx 명성 포인트가 감소.
#ifdef __WORLDSERVER
				g_dpDBClient.SendLogPkPvp( NULL, pMember, nAddFame, 'E' );
#endif // __WORLDSERVER
			}
			
			// 바뀐 명성치를 pMember의 주위에 날려줌.
			g_xWSUserManager->AddSetFame( pMember, pMember->m_nFame );
		}
	}
	m_idDuelParty = 0;
}


void CParty::DoUsePartyReCall( u_long /*uPartyId*/, u_long uLeaderid, int /*nSkill*/ )
{
	FLWSUser* pLeaderUser    = g_xWSUserManager->GetUserByPlayerID( uLeaderid );
	if( !IsValidObj( (CObj*)pLeaderUser ) )
		return;
	
	FLWSUser* pMember	= NULL;
	for( int i = 0; i < m_nSizeofMember; ++i )		// 단장(0)에게는 보낼필요 없다.
	{
		pMember		= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( (CObj*)pMember ) )
		{
			if( pMember->m_idPlayer != pLeaderUser->m_idPlayer )
			{
				pLeaderUser->SummonPlayer( pMember );

//				g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), pMember->m_idPlayer, pUser->GetLayer() );
			}
		}
	}
}

#endif // worldserver

//
// 극단 듀얼 해제
// 
void CParty::DoDuelPartyCancel( CParty* 
#ifdef __WORLDSERVER
							   pDuelParty 
#endif 
							   )
{
#ifndef __CORESERVER
	int		i;
	
	CMover *pMember;
	for( i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
#ifdef __WORLDSERVER
		pMember	= (CMover *)g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
#else
#ifdef __CLIENT
		pMember	= prj.GetUserByID( m_aMember[i].m_uPlayerId );
#endif
#endif // worldserver
		if( IsValidObj( pMember ) )
		{
#ifdef __WORLDSERVER
			if( pDuelParty )
				((FLWSUser *)pMember)->AddDuelPartyCancel( pDuelParty );		// 각 멤버들에게 듀얼이 취소되었다고 알림.
			else
				((FLWSUser *)pMember)->AddDuelPartyCancel( NULL );		// 각 멤버들에게 듀얼이 취소되었다고 알림.
			if( pMember->m_idDuelParty != m_idDuelParty )
				FLERROR_LOG( PROGRAM_NAME, _T( "파티멤버 %s의 정보이상. %d %d" ), pMember->GetName(), pMember->m_idDuelParty, m_idDuelParty );
			pMember->ClearDuelParty();
#endif // worldserver
		}
	}
	m_idDuelParty = 0;		// 파티 해제
#endif	// __CORESERVER
}	

#ifdef __WORLDSERVER
void CParty::ReplaceLodestar( const CRect &rect )
{
	// locked
	FLWSUser* pUser;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pUser	= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );

		if( IsValidObj( pUser ) )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				POINT point	= { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
				if( rect.PtInRect( point ) )
				{
					PRegionElem pRgnElem	= NULL;
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
					if( pRgnElem )
						pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
				}
			}
		}
	}
}

void CParty::Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL /*bMasterAround*/ )
{
	FLWSUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );

		if( IsValidObj( pMember ) )
		{
			/*
			if( GetQuest( QUEST_BOSS_LV1 ) != NULL )
				pMember->UnequipRide();
			
			if( GetQuest( QUEST_BOSS_LV2 ) != NULL )
				pMember->UnequipRide();
			
			if( GetQuest( QUEST_BOSS_LV3 ) != NULL )
				pMember->UnequipRide();
			*/			
			pMember->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nTempLayer );
			pMember->m_buffs.RemoveBuffs( RBF_COMMON, 0 );
		}
	}
}

void CParty::Replace( DWORD dwWorldId, LPCTSTR sKey )
{
	FLWSUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		
		if( IsValidObj( pMember ) )
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( dwWorldId, sKey );
			if( NULL != pRgnElem )
				pMember->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
		}
	}
}

BOOL CParty::ReplaceChkLv( int Lv )
{
	FLWSUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		
		if( IsValidObj( pMember ) )
		{
			if( pMember->GetLevel() > Lv )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
BOOL CParty::ReplaceChkLowLv( int Lv )
{
	FLWSUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		
		if( IsValidObj( pMember ) )
		{
			if( pMember->GetLevel() < Lv )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CParty::IsGuildParty()
{
	CMover * pLeader		= static_cast< FLWSUser * >( GetLeader() );		
	if( IsValidObj( pLeader ) == FALSE )
		return FALSE;

	if( pLeader->GetGuild() == NULL )
		return FALSE;

	if( pLeader->GetGuild()->m_idGuild == 0 )
		return FALSE;

	const u_long idGuild	= pLeader->GetGuild()->m_idGuild;
	for( int i = 0; i < m_nSizeofMember; ++i )
	{
		FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( pMember ) == FALSE )
			continue;

		if( pMember->GetGuild() == NULL )
			return FALSE;

		if( pMember->GetGuild()->m_idGuild != idGuild )
			return FALSE;
	}

	return TRUE;
}
#endif //__WORLDSERVER

#if defined( __WORLDSERVER ) || defined( __CLIENT )

void CParty::SetPos( int Nth, D3DXVECTOR3 vPos )
{
	if( Nth < 0 || Nth >= MAX_PTMEMBER_SIZE )
	{
		return;
	}

	m_aMember[ Nth ].m_vPos = vPos;
}

#endif



//////////////////////////////////////////////////////////////////////////
DWORD	CParty::GetKind() const
{
	return m_dwKind;
}

void	CParty::SetKind( const DWORD dwPartyKind )
{
	m_dwKind	= dwPartyKind;
}
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
int		CParty::GetMinLevel( const DWORD dwPartyKind ) const
{
	int nMinLevel = INT_MAX;

	switch( dwPartyKind )
	{
	case PARTY_KIND_BASE:
		nMinLevel = 0;
		break;

	case PARTY_KIND_EXTEND:
		nMinLevel = MAX_PARTY_BASE_LEVEL;
		break;

	default:
		break;
	}

	return nMinLevel;
}

bool	CParty::IsActivatedEffect( const DWORD dwEffectMode ) const
{
	if( dwEffectMode >= PARTY_EFFECT_MAX )
	{
		return false;
	}

	return m_arrPartyEffect[ dwEffectMode ];
}

bool	CParty::SetEffectActivation( const DWORD dwEffectMode, const bool bActivate )
{
	if( dwEffectMode <= PARTY_EFFECT_NONE || dwEffectMode >= PARTY_EFFECT_MAX )
	{
		return false;
	}

	m_arrPartyEffect[ dwEffectMode ] = bActivate;

	return true;
}

bool	CParty::SetDisconnectedMember( const u_long uPlayerID )
{
	const int nIndex = FindMember( uPlayerID );
	if( nIndex < 0  || nIndex >= MAX_PTMEMBER_SIZE )
	{
		return false;
	}

	m_aMember[ nIndex ].m_tTime		= CTime::GetCurrentTime();
	m_aMember[ nIndex ].m_bRemove	= TRUE;
	m_nReferens++;

	return true;
}

bool	CParty::ForceTurnOverLeader()
{
	for( int Nth = 1; Nth < m_nSizeofMember; ++Nth )
	{
		if( m_aMember[ Nth ].m_bRemove == FALSE )
		{
			SwapPartyMember( 0, Nth );
			return true;
		}
	}

	return false;
}

void	CParty::InitializeSkillMode()
{
	for( size_t Nth = 0; Nth < MAX_PARTYMODE; ++Nth )
	{
		m_dwModeTime[ Nth ] = 0;
	}
}

// void	CParty::InitializeEffectMode()
// {
// 	for( size_t Nth = 0; Nth < PARTY_EFFECT_MAX; ++Nth )
// 	{
// 		m_arrPartyEffect[ Nth ]	= false;
// 	}
// }

bool	CParty::CanChangePartyKind( const DWORD dwPartyKind ) const
{
	if( m_dwKind >= dwPartyKind )
	{
		return false;
	}

	if( GetLevel() < GetMinLevel( dwPartyKind ) )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
int		CParty::GetReqPointByUseSkill( const int nSkillID )
{
	int nReqPoint = INT_MAX;

	SkillProp* pSkillProp =  prj.GetPartySkill( nSkillID );
	if( pSkillProp == NULL )
	{
		return nReqPoint;
	}

	nReqPoint = ( IsActivatedEffect( PARTY_EFFECT_POINT_LIMITLESS ) == false ) ? static_cast<int>( pSkillProp->dwReqPartyPoint ) : 0;

	return nReqPoint;
}

bool	CParty::CanApplySkillMode( const u_long uPlayerID, const int nSkillMode ) const
{
	if( m_dwModeTime[ nSkillMode ] == 0 )
	{
		return false;
	}

	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( uPlayerID );
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( IsActivatedEffect( PARTY_EFFECT_PARSKILLFULL ) == false && IsLeader( uPlayerID ) == FALSE )
	{
		FLWSUser* pLeader = static_cast<FLWSUser*>( GetLeader() );
		if( IsValidObj( pLeader ) == FALSE )
		{
			return false;
		}

		if( pUser->IsValidArea( pLeader, 255.0f ) == FALSE )
		{
			return false;
		}
	}

	return true;
}
#endif // __WORLDSERVER
//////////////////////////////////////////////////////////////////////////

// endparty

CParty g_Party;
/*--------------------------------------------------------------------------------*/
#ifndef __CLIENT

CPartyMng::CPartyMng()
{
	m_id	= 0;
#ifdef __WORLDSERVER
	//m_nSecCount = 0;
	m_dwPartyMapInfoLastTick		= 0;
#endif // __WORLDSERVER
//	m_2Party.SetSize( 1024, 4096, 1024 );
#ifdef __CORESERVER
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __CORESERVER
}

CPartyMng::~CPartyMng()
{
//	CPartyBucket* pBucket	= m_2Party.GetFirstActive();
//	while( pBucket )
//	{
//		delete pBucket->m_value;
//		pBucket		= pBucket->pNext;
//	}
//	m_2Party.RemoveAll();
	Clear();
}

void CPartyMng::Clear( void )
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter();	// lock1
#endif	// __WORLDSERVER

	for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
		SAFE_DELETE( i->second );

	m_2PartyPtr.clear();
	
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER

#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave();	// unlock1
#endif	// __WORLDSERVER
}

// 극단 생성
// uLeaderPlayerId : 단장, uPartyId : 1번째 단원
// 극단를 생설할때는 2명으로 생성함( 혼자서는 극단를 생성할수 없음 )
// u_long	CPartyMng::NewParty( u_long uLeaderId, LONG /*nLeaderLevel*/, LONG /*nLeaderJob*/, BYTE /*nLeaderSex*/, LPSTR /*szLeaderName*/, u_long uMemberId, LONG /*nMemberLevel*/, LONG /*nMemberJob*/, BYTE /*nMemberSex*/, LPSTR /*szMembername*/, u_long uPartyId )
// {
// //	locked
// 	if( 0 == uPartyId )
// 	{
// 		m_id++;
// 	}
// 	else
// 	{
// 		m_id = uPartyId;
// 	}
// 
// 	
// 	if( NULL == GetParty( m_id ) ) // NULL 이면 극단이 없으므로 만들어야 한다
// 	{
// 		CParty* pParty	= new CParty;
// 		
// //		pParty->Lock();
// 		pParty->SetPartyId( m_id );
// //sun: 11, 캐릭터 정보 통합
// 		if( TRUE == pParty->NewMember( uLeaderId ) && TRUE == pParty->NewMember( uMemberId ) )
// 		{
// //			m_2Party.SetAt( m_id, pParty );
// 			m_2PartyPtr.insert( C2PartyPtr::value_type( m_id, pParty ) );
// 			pParty->m_nGetItemPlayerId = pParty->m_aMember[0].m_uPlayerId;
// //			pParty->Unlock();
// 			return m_id;
// 		}
// 		else // 극단에 소속되어 있다
// 		{
// //			pParty->Unlock();
// 			SAFE_DELETE( pParty );
// 		}
// 	}
// 	return 0;
// }

u_long	CPartyMng::NewParty( const u_long uLeaderId, const u_long uMemberId, const u_long uPartyId )
{
	CParty* pParty	= new CParty;
	
	if( TRUE == pParty->NewMember( uLeaderId ) && TRUE == pParty->NewMember( uMemberId ) )
	{
		m_id	= ( uPartyId == 0 ) ? m_id + 1 : uPartyId;

		pParty->SetPartyId( m_id );
		m_2PartyPtr.insert( C2PartyPtr::value_type( m_id, pParty ) );
		pParty->m_nGetItemPlayerId = pParty->m_aMember[0].m_uPlayerId;

		return m_id;
	}
	else // 극단에 소속되어 있다
	{
		SAFE_DELETE( pParty );

		return 0;
	}
}

// 극단 해체
// 해체는 사람이 부족하여 자동으로 해체
// 단독적으로 해체할수 없음
BOOL CPartyMng::DeleteParty( u_long uPartyId )
{
	CParty* pParty = GetParty( uPartyId );
	if( pParty )
	{
		//////////////////////////////////////////////////////////////////////////
		for( int Nth = 0; Nth < pParty->m_nSizeofMember; ++Nth )
		{
#ifdef __CORESERVER
			CPlayer* pMember	= g_PlayerMng.GetPlayer( pParty->GetPlayerId( Nth ) );
			if( pMember != NULL )
			{
				pMember->m_uPartyId	= 0;
			}
#endif // __CORESERVER

#ifdef __WORLDSERVER
			FLWSUser* pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( Nth ) );
			if( IsValidObj( pMember ) == TRUE )
			{
				pMember->m_idparty = 0;
			}
#endif // __WORLDSERVER
		}
		//////////////////////////////////////////////////////////////////////////

		if( pParty->m_idDuelParty )		// 극단듀얼중일때 극단이 해체되었으면
		{
			CParty *pDstParty = GetParty( pParty->m_idDuelParty );		// 상대 파티원들에게도 this파티가 해체되어 듀얼이해제됐다는걸 알림.
			if( pDstParty )
			{
				pDstParty->DoDuelPartyCancel( pParty );
			} 
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "상대파티를 찾을 수 없음 %d" ), pParty->m_idDuelParty );
			}

	#ifdef __WORLDSERVER
			g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pParty->m_idDuelParty, FALSE );
	#endif // __WORLDSERVER
			pParty->DoDuelPartyCancel( pDstParty );	// this파티원들에게도 듀얼해제됐다는걸 알림.
		}
#ifdef __WORLDSERVER
		CInstanceDungeonParty::GetInstance()->DestroyAllDungeonByDungeonID( uPartyId );
#endif // __WORLDSERVER

		SAFE_DELETE( pParty );
		m_2PartyPtr.erase( uPartyId );
		return TRUE;
	}
	return FALSE;
}

CParty* CPartyMng::GetParty( u_long uPartyId ) const
{
//	CParty* pParty;
//	if( m_2Party.Lookup( uPartyId, pParty ) )
//	{
//		return pParty;
//	}
	C2PartyPtr::const_iterator i		= m_2PartyPtr.find( uPartyId );
	if( i != m_2PartyPtr.end() )
		return i->second;
	return NULL;
}

void CPartyMng::Serialize( CAr & ar )
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter();	// lock1
#endif	// __WORLDSERVER

	if( ar.IsStoring() )
	{
		ar << m_id;
		int nCount	= 0;
		ptrdiff_t nCountTag = ar.ReserveSpace( sizeof( nCount ) );
		for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
		{
			CParty* pParty = (CParty*)i->second;
			pParty->Serialize( ar );
			nCount++;
		}
		ar.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );
	}
	else
	{
		int nCount;
		ar >> m_id;
		ar >> nCount;		
		for( int i = 0 ; i < nCount ; i++ )
		{
			CParty* pParty	= new CParty;
//			pParty->Lock();
			pParty->Serialize( ar );
			bool bResult = m_2PartyPtr.insert( C2PartyPtr::value_type( pParty->m_uPartyId, pParty ) ).second;
			if( bResult == false )
			{
				SAFE_DELETE( pParty );
			}
//			pParty->Unlock();
		}
	}
#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave();	// unlock1
#endif	// __WORLDSERVER
}

#ifdef __CORESERVER
BOOL CPartyMng::IsPartyNameId( u_long uidPlayer )
{
	if( m_2PartyNameLongPtr.end() != m_2PartyNameLongPtr.find( uidPlayer ) )
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CPartyMng::IsPartyName( const char* szPartyName )
{
	if( m_2PartyNameStringPtr.end() != m_2PartyNameStringPtr.find( szPartyName ) )
	{
		return TRUE;
	}
	return FALSE;
}

LPCSTR CPartyMng::GetPartyString( u_long uidPlayer )
{
	ULONG2STRING::iterator i	= m_2PartyNameLongPtr.find( uidPlayer );
	if( i != m_2PartyNameLongPtr.end() )
		return( i->second.c_str() );
	return NULL;
}

u_long CPartyMng::GetPartyID( const char* szPartyName )
{
	STRING2ULONG::iterator i	= m_2PartyNameStringPtr.find( szPartyName );
	if( i != m_2PartyNameStringPtr.end() )
		return( i->second );
	return 0;
}



void CPartyMng::AddPartyName( u_long uidPlayer, const char* szPartyName )
{
	m_2PartyNameLongPtr.insert(  std::map<u_long, std::string>::value_type( uidPlayer, szPartyName ) );
	m_2PartyNameStringPtr.insert(  std::map<std::string, u_long>::value_type(szPartyName, uidPlayer ) );
}

void CPartyMng::RemovePartyName( u_long uidPlayer, const char* szPartyName )
{
	ULONG2STRING::iterator iter = m_2PartyNameLongPtr.find( uidPlayer );
	STRING2ULONG::iterator iter1 = m_2PartyNameStringPtr.find( szPartyName );

	if( iter != m_2PartyNameLongPtr.end() )
		m_2PartyNameLongPtr.erase( iter );

	if( iter1 != m_2PartyNameStringPtr.end() )
		m_2PartyNameStringPtr.erase( iter1 );
//	m_2PartyNameLongPtr.insert(  std::map<u_long, string>::value_type( uidPlayer, szPartyName ) );
//	m_2PartyNameStringPtr.insert(  std::map<string, u_long>::value_type(szPartyName, uidPlayer ) );
}

BOOL CPartyMng::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return FALSE;
	return TRUE;
}

void CPartyMng::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT CPartyMng::_Worker( LPVOID pParam )
{
	CPartyMng* pPartyMng	= (CPartyMng*)pParam;
	pPartyMng->Worker();
	return 0;
}

void CPartyMng::Worker( void )
{
	CPlayer* pMember;

	HANDLE hHandle	= m_hCloseWorker;
	while( WaitForSingleObject( hHandle, 1000 ) == WAIT_TIMEOUT )
	{
		CTime timeCurr	= CTime::GetCurrentTime();

		CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
		CMclAutoLock	Lock2( m_AddRemoveLock );

		for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); )
		{
			CParty* pParty = (CParty*)i->second;
			++i;
			if( pParty->m_nReferens > 0 )
			{
				for( int j = 1; j < pParty->m_nSizeofMember; j++ )
				{
					if( pParty->m_aMember[j].m_uPlayerId != NULL_PLAYER_ID && pParty->m_aMember[j].m_bRemove )
					{
						if( timeCurr.GetTime() - pParty->m_aMember[j].m_tTime.GetTime() > 60 * 10 )	// 10분 : 60 * 10 // 지금은 1분
						{
							u_long idMember		= pParty->GetPlayerId( j );
							if( pParty->DeleteMember( idMember ) )
							{
								pParty->m_nReferens--;
								
								g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, pParty->m_aMember[0].m_uPlayerId, idMember );
								
								if( pParty->GetSizeofMember() < MIN_PTMEMBER_SIZE )
								{
									pMember		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( 0 ) );
									if( pMember )
										pMember->m_uPartyId	= 0;
									DeleteParty( pParty->m_uPartyId );
									pParty = NULL;
									break;
								}

								if( pParty->m_nReferens == 0 )
									break;
							}
						}
					}
				}	// for
			}	// if

			if( pParty == NULL )
			{
				continue;
			}

			// 파티모드를 체크
			for( int j = 0 ; j < MAX_PARTYMODE ; j++ )
			{
				if( pParty->m_dwModeTime[j] )
				{
//sun: 12, 파스킬 아이템 수정
// 					if( j == PARTY_PARSKILL_MODE)
// 						continue;

					const DWORD dwPrevModeTime	= pParty->m_dwModeTime[ j ];
					const DWORD dwCurrModeTime	= pParty->m_dwModeTime[ j ] - 1000;

					pParty->m_dwModeTime[ j ]	= dwCurrModeTime < dwPrevModeTime ? dwCurrModeTime : 0;
					if( pParty->m_dwModeTime[ j ] == 0 )
					{
						g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, j, FALSE );
					}


// 					pParty->m_nModeTime[j] -= 1000;
// 					if( pParty->m_nModeTime[j] <= 0 )
// 					{
// 						pParty->m_nModeTime[j] = 0;
// 						g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, j, FALSE );
// 					}
				}
			}
		}	// for
//		FLTRACE_LOG( PROGRAM_NAME, _T( "Worker Party" ) );
	}
}

void CPartyMng::AddConnection( CPlayer* pPlayer )
{
	CParty* pParty;
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	pParty	= GetParty( pPlayer->m_uPartyId );
	if( pParty )
	{
		int i	= pParty->FindMember( pPlayer->uKey );
		if( i < 0 )
		{
			pPlayer->m_uPartyId		= 0;
			return;
		}

		pParty->m_aMember[i].m_bRemove	= FALSE;
		pParty->m_nReferens--;

		BEFORESENDDUAL( ar, PACKETTYPE_ADDPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );
	}
	else
		pPlayer->m_uPartyId		= 0;
}

void CPartyMng::RemoveConnection( CPlayer* pPlayer )
{
	if( pPlayer->m_uPartyId == 0 )
		return;

	CParty* pParty;
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	pParty	= GetParty( pPlayer->m_uPartyId );
	if( pParty )
	{
		int i	= pParty->FindMember( pPlayer->uKey );
		if( i < 0 )
			return;
		pParty->m_aMember[i].m_tTime	= CTime::GetCurrentTime();
		pParty->m_aMember[i].m_bRemove	= TRUE;
		pParty->m_nReferens++;

		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );

//sun: 12, 파스킬 아이템 수정
// 		if( pParty->m_nModeTime[PARTY_PARSKILL_MODE] )
// 			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );

		if( i == 0 )
		{
			bool fRemoveParty	= true;
			for( int j = 1; j < pParty->m_nSizeofMember; j++ )
			{
				if( pParty->m_aMember[j].m_bRemove == FALSE )
				{
					fRemoveParty	= false;
					pParty->SwapPartyMember( 0, j );
					break;
				}
			}
//sun: 12, 파스킬 아이템 수정
			for( int k = 0 ; k < MAX_PARTYMODE ; k++ )
			{
				if( pParty->m_dwModeTime[k] )
				{
// 					if( k == PARTY_PARSKILL_MODE)
// 						continue;
					pParty->m_dwModeTime[k] = 0;
				}
			}

			if( fRemoveParty )
			{
				CPlayer* pMember = NULL;
				for( int j = 0; j < pParty->m_nSizeofMember; j++ )
				{
					pMember		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( j ) );
					if( pMember )
						pMember->m_uPartyId		= 0;
				}
				DeleteParty( pParty->m_uPartyId );
			}
		}
	}
}
#endif // __CORESERVER

#ifdef __WORLDSERVER

void CPartyMng::PartyMapInfo( )
{
	//const float PARTY_MAP_AROUND = 32.0f * 32.0f;	// m_arrVisibilityRangeNum에 영향을 받는다.
	const float MOVE_DIST_MIN	 = 0.0f;

	//2. interval check
	if( m_dwPartyMapInfoLastTick + PARTY_MAP_SEC * 1000 > ::GetTickCount() )
		return;

	m_dwPartyMapInfoLastTick = ::GetTickCount();


	//3. 
	for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
	{
		D3DXVECTOR3 vPosBuf;
		float fDist;

		CParty* pParty = (CParty*)i->second;
		for( int j = 0 ; j < pParty->GetSizeofMember() ; ++j )
		{
			CMover* pMover = prj.GetUserByID( pParty->GetPlayerId( j ) );
			if( !IsValidObj( pMover ) )
				continue;

			if( pMover->GetWorld() == NULL || pMover->GetWorld()->m_linkMap.GetLinkMap( pMover->GetLayer() ) == NULL )
				continue;


			vPosBuf = pMover->GetPos() - pParty->GetPos( j );
			fDist = D3DXVec3LengthSq( &vPosBuf );
			if( MOVE_DIST_MIN > fDist )
				continue;

			
			pParty->SetPos( j, pMover->GetPos() );

			
			for( int k = 0 ; k < pParty->GetSizeofMember() ; ++k )
			{
				if( k == j )
					continue;

				CMover* pSendMover = prj.GetUserByID( pParty->GetPlayerId( k ) );
				if( !IsValidObj( pSendMover ) )
					continue;

				if( pSendMover->GetWorld() == NULL )
					continue;

				if( pSendMover->GetWorld() != pMover->GetWorld() )
					continue;

				vPosBuf = pSendMover->GetPos() - pMover->GetPos();
				fDist = D3DXVec3LengthSq( &vPosBuf );

				const FLMapGrid & kMapGrid	= pMover->GetWorld()->m_linkMap.GetLinkMap( pMover->GetLayer() )->m_kMapGrid;
				const int uViewRange		= kMapGrid.GetTileSize() * kMapGrid.GetViewRangeCount();
				if( pMover->GetWorld() == pSendMover->GetWorld() && fDist > uViewRange )
					((FLWSUser*)pSendMover)->AddPartyMapInfo( j, pMover->GetPos() );		
			}
		}
	}
}

//void CPartyMng::PartyMapInfo( )
//{
//	const float PARTY_MAP_AROUND = 32.0f * 32.0f;	// m_arrVisibilityRangeNum에 영향을 받는다.
//
//// 	if( ++m_nSecCount < PARTY_MAP_SEC )
//// 		return;
//// 
//// 	m_nSecCount = 0;
//
//	if( m_dwPartyMapInfoLastTick + PARTY_MAP_SEC * 1000 > ::GetTickCount() )
//		return;
//
//	m_dwPartyMapInfoLastTick = ::GetTickCount();
//
//
//	D3DXVECTOR3 vPosBuf;
//	float fDist;
//	for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
//	{
//		CParty* pParty = (CParty*)i->second;
//		for( int j = 0 ; j < pParty->GetSizeofMember() ; ++j )
//		{
//			CMover* pMover = prj.GetUserByID( pParty->GetPlayerId( j ) );
//			if( !IsValidObj( pMover ) )
//				continue;
//			
//			vPosBuf = pMover->GetPos() - pParty->GetPos( j );
//			fDist = D3DXVec3LengthSq( &vPosBuf );
//			if( 0.0f < fDist )
//			{
//				pParty->SetPos( j, pMover->GetPos() );
//				
//				CMover* pSendMover;
//				for( int k = 0 ; k < pParty->GetSizeofMember() ; ++k )
//				{
//					if( k == j )
//						continue;
//					pSendMover = prj.GetUserByID( pParty->GetPlayerId( k ) );
//					if( !IsValidObj( pSendMover ) )
//						continue;
//
//					vPosBuf = pSendMover->GetPos() - pMover->GetPos();
//					fDist = D3DXVec3LengthSq( &vPosBuf );
//					if( fDist > PARTY_MAP_AROUND )
//						((FLWSUser*)pSendMover)->AddPartyMapInfo( j, pMover->GetPos() );		
//				}
//			}
//		}
//	}
//}

#endif // __WORLDSERVER




//////////////////////////////////////////////////////////////////////////
bool	CPartyMng::SetPartyEffectMode( const u_long uPlayerID, const u_long uPartyID, const DWORD dwEffectMode, const bool bActivate )
{
#ifdef __CORESERVER
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif // __CORESERVER

	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL || pParty->IsMember( uPlayerID ) == FALSE )
	{
		return false;
	}

	if( dwEffectMode != PARTY_EFFECT_PARSKILLFULL && pParty->IsLeader( uPlayerID ) == FALSE )
	{
		return false;
	}

	const bool bRet = pParty->SetEffectActivation( dwEffectMode, bActivate );

#ifdef __WORLDSERVER
	if( dwEffectMode == PARTY_EFFECT_POINT_LIMITLESS && bActivate == false )
	{
		FLWSUser* pLeader = g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( 0 ) );
		if( IsValidObj( pLeader ) == FALSE )
		{
			return bRet;
		}
		
		for( int nMode = PARTY_LINKATTACK_MODE; nMode < MAX_PARTYMODE; ++nMode )
		{
			const DWORD dwSkillTime = GetDefaultSkillTime( pLeader->m_idPlayer, nMode );

			if( pParty->m_dwModeTime[ nMode ] != 0 && dwSkillTime != 0 )
			{
				g_DPCoreClient.SendForceSetPartySkillTime( pLeader->m_idPlayer, nMode, dwSkillTime );
			}
		}
	}
#endif // __WORLDSERVER
	
	return bRet;
}

bool	CPartyMng::RemovePartyMember( const u_long uPartyID, const u_long uLeaderID, const u_long uMemberID )
{
#ifdef __CORESERVER
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif // __CORESERVER

	CParty* pParty	= GetParty( uPartyID );
	if( pParty == NULL )
	{
		return false;
	}

	if( pParty->m_idDuelParty > 0 )
	{
		return false;
	}

	if( pParty->IsLeader( uLeaderID ) == FALSE )
	{
		return false;
	}

	if( pParty->IsMember( uMemberID ) == FALSE )
	{
		return false;
	}

	if( pParty->DeleteMember( uMemberID ) == FALSE )
	{
		return false;
	}

#ifdef __CORESERVER
	g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, uLeaderID, uMemberID );
#endif // __CORESERVER

	if( pParty->IsLeader( uMemberID ) == TRUE )
	{
		if( pParty->m_idDuelParty > 0 )
		{
			CParty* pDuelParty	= GetParty( pParty->m_idDuelParty );
			if( pDuelParty )
				pParty->DoDuelPartyCancel( pDuelParty );
		}

		pParty->InitializeSkillMode();

		if( pParty->ForceTurnOverLeader() == false )
		{
			DeleteParty( pParty->m_uPartyId );
			pParty = NULL;
		}
		else
		{
#ifdef __CORESERVER
			u_long idChangeLeader =  pParty->GetPlayerId( 0 );
			g_dpCoreSrvr.SendChangeLeader( pParty->m_uPartyId, idChangeLeader );
#endif // __CORESERVER
		}
	}

	return true;
}

bool	CPartyMng::DisconnectPartyMember( const u_long uPartyID, const u_long uPlayerID )
{
#ifdef __CORESERVER
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif // __CORESERVER

	CParty* pParty	= GetParty( uPartyID );
	if( pParty == NULL )
	{
		return false;
	}

	if( pParty->IsMember( uPlayerID ) == FALSE )
	{
		return false;
	}

	if( pParty->SetDisconnectedMember( uPlayerID ) == false )
	{
		return false;
	}

#ifdef __WORLDSERVER
	for( int Nth = 0; Nth < pParty->m_nSizeofMember; ++Nth )
	{
		FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( Nth ) );
		if( IsValidObj( pMember ) == TRUE )
		{
			pMember->AddSetPartyMemberParam( uPlayerID, PP_REMOVE, 1 );
		}
	}
#endif // __WORLDSERVER

	if( pParty->IsLeader( uPlayerID ) == TRUE )
	{
		if( pParty->m_idDuelParty > 0 )
		{
			CParty* pDuelParty	= GetParty( pParty->m_idDuelParty );
			if( pDuelParty )
				pParty->DoDuelPartyCancel( pDuelParty );
		}

		pParty->InitializeSkillMode();

		if( pParty->ForceTurnOverLeader() == false )
		{
			DeleteParty( pParty->m_uPartyId );
		}
	}

	return true;
}


#ifdef __CORESERVER
DWORD	CPartyMng::CanInviteParty( const CPlayer & kSrcPlayer, const CPlayer & kDestPlayer )
{
//	CMclAutoLock	Lock( m_AddRemoveLock );

	CParty* pSrcPlayerParty		= GetParty( kSrcPlayer.m_uPartyId );
	CParty* pDestPlayerParty	= GetParty( kDestPlayer.m_uPartyId );

	if( pSrcPlayerParty != NULL )
	{
		if( pSrcPlayerParty->IsLeader( kSrcPlayer.uKey ) == FALSE )		// 극단에 소속되어 있는데 단장이 아닐 경우
		{
			return ERROR_NOLEADER;
		}

		if( pSrcPlayerParty->GetSizeofMember() >= MAX_PTMEMBER_SIZE )	// 극단이 꽉찼을 경우
		{
			return ERROR_FULLPARTY;
		}
	}

	if( pDestPlayerParty != NULL )		// 상대방이 극단 소속일 경우
	{
		return ERROR_DIFFRENTPARTY;
	}

	return ERROR_INVITE_SUCCESS;
}

void	CPartyMng::OnInviteParty( const u_long uSrcPlayerID, const u_long uDestPlayerID )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pSrcPlayer		= g_PlayerMng.GetPlayer( uSrcPlayerID );
	CPlayer* pDestPlayer	= g_PlayerMng.GetPlayer( uDestPlayerID );

	if( pSrcPlayer == NULL || pDestPlayer == NULL )
	{
		return;
	}

	CMclAutoLock	Lock1( m_AddRemoveLock );

	const DWORD dwErrorCode	= CanInviteParty( *pSrcPlayer, *pDestPlayer );
	if( dwErrorCode != ERROR_INVITE_SUCCESS )
	{
		g_DPCacheSrvr.SendErrorParty( dwErrorCode, pSrcPlayer );
		return;
	}

	CParty* pParty		= GetParty( pSrcPlayer->m_uPartyId );
	if( pParty != NULL )	// ADD
	{
		if( pParty->NewMember( uDestPlayerID ) == TRUE )
		{
			pDestPlayer->m_uPartyId	= pParty->m_uPartyId;

			g_dpCoreSrvr.SendAddPartyMember( pParty->m_uPartyId, uSrcPlayerID, uDestPlayerID );
		}
	}
	else	// NEW
	{
		const u_long uPartyId = NewParty( uSrcPlayerID, uDestPlayerID );
		if( uPartyId != 0 )
		{
			pSrcPlayer->m_uPartyId	= uPartyId;
			pDestPlayer->m_uPartyId	= uPartyId;

			g_dpCoreSrvr.SendAddPartyMember( uPartyId, uSrcPlayerID, uDestPlayerID );
		}
	}
}

void	CPartyMng::RefreshPartyEffectMode( const u_long /*uPlayerID*/, const u_long uPartyID, const DWORD dwEffectMode )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL )
	{
		return;
	}

// 	if( pParty->IsMember( uPlayerID ) == FALSE )
// 	{
// 		return;
// 	}

	if( pParty->IsActivatedEffect( dwEffectMode ) == false )
	{
		return;
	}

	if( pParty->SetEffectActivation( dwEffectMode, false ) == false )
	{
		return;
	}

	NotiRefreshPartyEffectModeToWS( uPartyID, dwEffectMode );
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
DWORD	CPartyMng::CanInviteParty( const u_long uLeaderPlayerID, const u_long uMemberPlayerID )
{
	FLWSUser* pLeader	= g_xWSUserManager->GetUserByPlayerID( uLeaderPlayerID );
	if( IsValidObj( pLeader ) == FALSE )
	{
		return FSC_PARTY_INVITE_LEADER_NOT_EXIST;
	}

	FLWSUser* pMember	= g_xWSUserManager->GetUserByPlayerID( uMemberPlayerID );
	if( IsValidObj( pMember ) == FALSE )
	{
		return FSC_PARTY_INVITE_MEMBER_NOT_EXIST;
	}

	CParty* pMemberParty	= GetParty( pMember->m_idparty );
	if( pMemberParty != NULL )
	{
		return FSC_PARTY_INVITE_MEMBER_HAVE_PARTY;
	}

	CParty* pLeaderParty	= GetParty( pLeader->m_idparty );
	if( pLeaderParty != NULL )	// 기존 극단에 멤버 추가인 경우..
	{
		if( pLeaderParty->IsLeader( uLeaderPlayerID ) == FALSE )
		{
			return FSC_PARTY_INVITE_NOT_LEADER;
		}
		if( pLeaderParty->GetSizeofMember() >= MAX_PTMEMBER_SIZE )
		{
			return FSC_PARTY_INVITE_MEMBER_OVERFLOW;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	if( pLeader->m_nDuel > 0 || pMember->m_nDuel > 0 )
	{
		return FSC_PARTY_INVITE_PVP;
	}

	if( pMember->IsAttackMode() == TRUE )
	{
		return FSC_PARTY_INVITE_ATTACK_MODE;
	}

	if( g_pEventArenaGlobal->IsArenaChannel() == true )
	{
		return FSC_PARTY_INVITE_CANNOT_WORLD;
	}

	CWorld* pLeaderWorld	= pLeader->GetWorld();
	if( pLeaderWorld == NULL || pLeaderWorld->CanCreateParty() == false )
	{
		return FSC_PARTY_INVITE_CANNOT_WORLD;
	}

	CWorld* pMemberWorld	= pMember->GetWorld();
	if( pMemberWorld == NULL || pMemberWorld->CanCreateParty() == false )
	{
		return FSC_PARTY_INVITE_CANNOT_WORLD;
	}

	if( g_GuildCombat1to1Mng.IsPossibleUser( pLeader ) == TRUE || g_GuildCombat1to1Mng.IsPossibleUser( pMember ) == TRUE )
	{
		return FSC_PARTY_INVITE_GUILD_COMBAT;
	}
	//////////////////////////////////////////////////////////////////////////

	return FSC_PARTY_INVITE_SUCCESS;
}

void	CPartyMng::RefreshPartyEffectMode( const u_long uPartyID, const DWORD dwEffectMode ) const
{
	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL )
	{
		return;
	}

	if( pParty->SetEffectActivation( dwEffectMode, false ) == false )
	{
		return;
	}

	NotiSetPartyEffectModeToClient( uPartyID, dwEffectMode, false );

	if( dwEffectMode == PARTY_EFFECT_PARSKILLFULL )
	{
		for( int Nth = 0; Nth < pParty->m_nSizeofMember; ++Nth )
		{
			FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( Nth ) );
			if( IsValidObj( pMember ) == FALSE )
			{
				continue;
			}

			if( pMember->IsSMMode( SM_PARTYSKILL1 ) || pMember->IsSMMode( SM_PARTYSKILL15 ) || pMember->IsSMMode( SM_PARTYSKILL30 ) )
			{
				NotiSetPartyEffectModeToCS( pMember->m_idPlayer, uPartyID, dwEffectMode, true );

				break;
			}
		}
	}
	else if( dwEffectMode == PARTY_EFFECT_POINT_LIMITLESS )
	{
		FLWSUser* pMember		= static_cast<FLWSUser*>( pParty->GetLeader() );
		if( IsValidObj( pMember ) == FALSE )
		{
			return;
		}

		if( pMember->HasBuffByIk3( IK3_PARTYPOINTFREE ) == TRUE )
		{
			NotiSetPartyEffectModeToCS( pMember->m_idPlayer, uPartyID, dwEffectMode, true );
		}
	}
}

void	CPartyMng::CheckAddMemberPartyEffect( const u_long uPlayerID, const u_long uPartyID ) const
{
	FLWSUser* pAddMember		= g_xWSUserManager->GetUserByPlayerID( uPlayerID );
	if( IsValidObj( pAddMember ) == FALSE )
	{
		return;
	}

	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL )
	{
		return;
	}

	if( pParty->IsMember( uPlayerID ) == FALSE )
	{
		return;
	}

	if( pParty->IsActivatedEffect( PARTY_EFFECT_POINT_LIMITLESS ) == false
		&& pParty->IsLeader( uPlayerID ) == TRUE
		&& pAddMember->HasBuffByIk3( IK3_PARTYPOINTFREE ) == TRUE )
	{
		NotiSetPartyEffectModeToCS( uPlayerID, uPartyID, PARTY_EFFECT_POINT_LIMITLESS, true );
	}

	if( pParty->IsActivatedEffect( PARTY_EFFECT_PARSKILLFULL ) == false
		&& ( pAddMember->IsSMMode( SM_PARTYSKILL1 ) || pAddMember->IsSMMode( SM_PARTYSKILL15 ) || pAddMember->IsSMMode( SM_PARTYSKILL30 ) ) )
	{
		NotiSetPartyEffectModeToCS( uPlayerID, uPartyID, PARTY_EFFECT_PARSKILLFULL, true );
	}
}

void	CPartyMng::CheckRemoveMemberPartyEffect( const u_long uPlayerID, const u_long uPartyID ) const
{
	FLWSUser* pRemoveMember		= g_xWSUserManager->GetUserByPlayerID( uPlayerID );
	if( IsValidObj( pRemoveMember ) == FALSE )
	{
		return;
	}

	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL )
	{
		return;
	}

	if( pRemoveMember->IsSMMode( SM_PARTYSKILL1 ) || pRemoveMember->IsSMMode( SM_PARTYSKILL15 ) || pRemoveMember->IsSMMode( SM_PARTYSKILL30 ) )
	{
		NotiRefreshPartyEffectModeToCS( uPlayerID, uPartyID, PARTY_EFFECT_PARSKILLFULL );
	}
}

DWORD	CPartyMng::GetPartySkillID_BySkillMode( const DWORD dwMode ) const
{
	DWORD dwSkillID	= 0;

	switch( dwMode )
	{
	case PARTY_LINKATTACK_MODE:
		dwSkillID	= SKILL_INDEX( 5, ST_LINKATTACK );
		break;

	case PARTY_STRETCHING_MODE:
		dwSkillID	= SKILL_INDEX( 7, ST_STRETCHING );
		break;

	case PARTY_FORTUNECIRCLE_MODE:
		dwSkillID	= SKILL_INDEX( 6, ST_FORTUNECIRCLE );
		break;

	case PARTY_GIFTBOX_MODE:
		dwSkillID	= SKILL_INDEX( 8, ST_GIFTBOX );
		break;

	default:
		break;
	}

	return dwSkillID;
}

DWORD	CPartyMng::GetDefaultSkillTime( const u_long uLeaderID, const DWORD dwMode ) const
{
	DWORD dwSkillTime	= 0;
	
	const DWORD dwSkillID	= GetPartySkillID_BySkillMode( dwMode );

	const SkillProp* pSkillProp =  prj.GetPartySkill( dwSkillID );
	if( pSkillProp == NULL )
	{
		return dwSkillTime;
	}

	dwSkillTime	= pSkillProp->dwSkillTime;

	if( CSLord::Instance()->IsLord( uLeaderID ) == TRUE )
	{
		dwSkillTime	*= 4;
	}

	return dwSkillTime;
}
#endif // __WORLDSERVER
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef __CORESERVER
void	CPartyMng::NotiRefreshPartyEffectModeToWS( const u_long uPartyID, const DWORD dwEffectMode ) const
{
	BEFORESENDDUAL( ar, PACKETTYPE_REFRESH_PARTY_EFFECT_MODE_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyID << dwEffectMode;
	SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );
}

void	CPartyMng::NotiSetPartyEffectModeToWS( const u_long uPlayerID, const u_long uPartyID, const DWORD dwEffectMode, const bool bActivate ) const
{
	BEFORESENDDUAL( ar, PACKETTYPE_SET_PARTY_EFFECT_MODE_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPlayerID << uPartyID << dwEffectMode << bActivate;
	SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );
}
#endif // __CORESERVER

#ifdef __WORLDSERVER
void	CPartyMng::NotiRefreshPartyEffectModeToCS( const u_long uPlayerID, const u_long uPartyID, const DWORD dwEffectMode ) const
{
	BEFORESENDDUAL( ar, PACKETTYPE_REFRESH_PARTY_EFFECT_MODE_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPlayerID << uPartyID << dwEffectMode;
	SEND( ar, &g_DPCoreClient, DPID_SERVERPLAYER );
}

void	CPartyMng::NotiSetPartyEffectModeToCS( const u_long uPlayerID, const u_long uPartyID, const DWORD dwEffectMode, const bool bActivate ) const
{
	BEFORESENDDUAL( ar, PACKETTYPE_SET_PARTY_EFFECT_MODE_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPlayerID << uPartyID << dwEffectMode << bActivate;
	SEND( ar, &g_DPCoreClient, DPID_SERVERPLAYER );
}

void	CPartyMng::RequestAddPartyMemberToCS( const u_long uLeaderPlayerID, const u_long uMemberPlayerID ) const
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTYMEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uLeaderPlayerID << uMemberPlayerID;
	SEND( ar, &g_DPCoreClient, DPID_SERVERPLAYER );
}

void	CPartyMng::NotiRefreshPartyEffectModeToClient( const u_long uPartyID ) const
{
	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL )
	{
		return;
	}

	for( int Nth = 0; Nth < pParty->m_nSizeofMember; ++Nth )
	{
		FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( Nth ) );
		if( IsValidObj( pMember ) == TRUE )
		{
			pMember->m_Snapshot.cb++;
			pMember->m_Snapshot.ar << pMember->GetId();
			pMember->m_Snapshot.ar << SNAPSHOTTYPE_REFRESH_PARTY_EFFECT_MODE_NOTI;
		}
	}
}

void	CPartyMng::NotiSetPartyEffectModeToClient( const u_long uPartyID, const DWORD dwEffectMode, const bool bActivate ) const
{
	CParty* pParty = GetParty( uPartyID );
	if( pParty == NULL )
	{
		return;
	}

	for( int Nth = 0; Nth < pParty->m_nSizeofMember; ++Nth )
	{
		FLWSUser* pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->GetPlayerId( Nth ) );
		if( IsValidObj( pMember ) == TRUE )
		{
			pMember->m_Snapshot.cb++;
			pMember->m_Snapshot.ar << pMember->GetId();
			pMember->m_Snapshot.ar << SNAPSHOTTYPE_SET_PARTY_EFFECT_MODE_NOTI;
			pMember->m_Snapshot.ar << dwEffectMode;
			pMember->m_Snapshot.ar << bActivate;
		}
	}
}

#endif // __WORLDSERVER
//////////////////////////////////////////////////////////////////////////

CPartyMng	g_PartyMng;
#endif // not client