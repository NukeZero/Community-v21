// Billboard.cpp : implementation of the CBeastView class
//

#include "stdafx.h"
#include "lang.h"
#include "defineText.h"
#include "defineSound.h"
#ifndef __WORLDSERVER
#include "DialogMsg.h"
extern CDialogMsg g_DialogMsg;
#else
//#include "DPCoreClient.h"
//#include "User.h"
//extern	CDPCoreClient	g_DPCoreClient;
//extern	CUserMng	g_UserMng;
#endif

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#else	// __CLIENT
#ifdef __WORLDSERVER
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER
#endif	// __CLIENT

#include "MsgHdr.h"
#include "CommonCtrl.h"
#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_WANDER,
	STATE_PURSUE,
	STATE_EVADE,
	STATE_RAGE
};
BEGIN_AISTATE_MAP( CAIHuman, CAIInterface )

	ON_STATE( STATE_INIT   , StateInit   )
	ON_STATE( STATE_IDLE   , StateIdle   )
	ON_STATE( STATE_WANDER , StateWander )
	ON_STATE( STATE_PURSUE , StatePursue )
	ON_STATE( STATE_EVADE  , StateEvade  )
	ON_STATE( STATE_RAGE   , StateRage   )

END_AISTATE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __WORLDSERVER
MemPooler<CAIHuman>*	CAIHuman::m_pPool	= new MemPooler<CAIHuman>( 512 );
#else	// __WORLDSERVER
MemPooler<CAIHuman>*	CAIHuman::m_pPool	= new MemPooler<CAIHuman>( 128 );
#endif	// __WORLDSERVER

CAIHuman::CAIHuman()
{
	m_dwIdTarget = NULL_ID;
	m_dwMoveType = 0;
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
#endif	// __POSTAIMSG1008
}
CAIHuman::CAIHuman( CObj* pObj ) : CAIInterface( pObj, 0 )
{
	m_dwIdTarget = NULL_ID;
	m_dwMoveType = 0;
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
#endif	// __POSTAIMSG1008
}
CAIHuman::~CAIHuman()
{
}

#ifdef __POSTAIMSG1008
BOOL CAIHuman::StateInit( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIHuman::StateIdle( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIHuman::StateWander( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIHuman::StatePursue( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIHuman::StateEvade( const AIMSG & msg )
	{	return TRUE;	}
#else	// __POSTAIMSG1008
BOOL CAIHuman::StateInit()
	{	return TRUE;	}
BOOL CAIHuman::StateIdle()
	{	return TRUE;	}
BOOL CAIHuman::StateWander()
	{	return TRUE;	}
BOOL CAIHuman::StatePursue()
	{	return TRUE;	}
BOOL CAIHuman::StateEvade()
	{	return TRUE;	}
#endif	// __POSTAIMSG1008

#ifdef __POSTAIMSG1008
BOOL CAIHuman::StateRage( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIHuman::StateRage()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i = m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	ADDERRORSEQ( _i, 1000 );
	CMover* pMover = GetMover();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
//		SendAIMsg( AIMSG_SETPROCESS, TRUE );
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	OnMessage( AIMSG_ARRIVAL )
		ADDERRORSEQ( _i, 1001 );
#ifdef __POSTAIMSG1008
		CCtrl* pObj = prj.GetCtrl( msg.dwParam1 );
#else	// __POSTAIMSG1008
		CCtrl* pObj = prj.GetCtrl( m_AIMsg.dwParam1 );
#endif	// __POSTAIMSG1008

		if( NULL != pObj )
		{
			if( pObj->GetType() == OT_ITEM )
			{
#ifdef __SCHOOL0701
				if( pMover->IsMode( ITEM_MODE ) )
				{
					return TRUE;
				}
#endif // __SCHOOL0701
				CItem* pItem = (CItem*)pObj;

//				CMclAutoLock	Lock( pItem->m_AccessLock );
				ADDERRORSEQ( _i, 1002 );
				pItem->Lock( theLineFile );	// lock

//				if( pItem->IsDelete() ||
//					( pItem->m_idHolder != 0 && pItem->m_idHolder != pMover->m_idPlayer ) )
//				{
//					pItem->Unlock( theLineFile );	// unlock
//					return TRUE;
//				}
				
				BOOL bSuccess	= TRUE;
				BOOL bTake	= FALSE;

				//--------- pItem이 집을수 있는 아이템인가 검사.
				bTake = pMover->IsLoot( pItem );

				if( bTake == FALSE )	// 집을 수 없는 경우
				{
#ifdef __WORLDSERVER
					((CUser*)pMover)->AddDefinedText( TID_GAME_PRIORITYITEMPER, "\"%s\"", pItem->GetProp()->szName );	// %는 다른사람 아이템임다.
#endif	// __WORLDSERVER
					pItem->Unlock( theLineFile );	// unlock
					return TRUE;
				}
#ifdef __XPET
				//---------- pItem을 줏음.
				if( pMover->DoLoot( pItem ) == FALSE )
				{
					pItem->Unlock( theLineFile );	// unlock
					return TRUE;
				}
#ifdef __WORLDSERVER
				g_UserMng.AddMotion( pMover, OBJMSG_PICKUP );		// 줍는 모션
#endif
				
#else // xPet
//#ifndef __XPET
//{{AFX
				ADDERRORSEQ( _i, 1005 );
				CItemBase* pItemBase = pItem->m_pItemBase;
 #ifdef __WORLDSERVER
				if( pItem->GetPosAttr() < 0 )
				{
					pItem->Unlock( theLineFile );
					return TRUE;
				}
 #else	// __WORLDSERVER
				PLAYSND( SND_INF_GROUNDPICKUP );
 #endif	// __WORLDSERVER
				
				ADDERRORSEQ( _i, 1006 );
				ItemProp *pItemProp = pItem->GetProp();
				if( pItemProp->dwItemKind1 == IK1_GOLD ) 
				{
					pMover->PickupGold( ((CItemElem*)pItemBase)->GetGold(), pItem->m_bDropMob );
					ADDERRORSEQ( _i, 1007 );
				}
				else 
				{
					// <아이템 로그 - 획득>
 #ifdef __WORLDSERVER
					if( pItem->m_bDropMob == TRUE ) // 몬스터가 떨어뜨린것만
					{
						ADDERRORSEQ( _i, 1013 );
//						CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );
						g_PartyMng.m_AddRemoveLock.Enter( theLineFile );	// lock2
						
						CParty *pParty	= g_PartyMng.GetParty( pMover->m_idparty );
						if( pParty && pParty->IsMember( pMover->m_idPlayer )  )	// 파티가 있으므로 경험치 분배대로 해야함
						{
							CUser *pListMember[ 8 ];
							int	nMaxListMember = 0;
							CUser* pMember	= NULL;
							CUser* pGetUser = NULL;
							float fDist;
							D3DXVECTOR3	vDist;
							
							ADDERRORSEQ( _i, 1014 );
							// 아이템줍는사람 반경안에 드는 멤버들만 추려낸다.
							memset( pListMember, 0, sizeof(pListMember) );
							for( int i = 0; i < pParty->m_nSizeofMember; i++ )
							{
								pMember		= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
								if( IsValidObj( (CObj*)pMember ) )
								{
									vDist = pMember->GetPos() - pMover->GetPos();
									fDist = D3DXVec3LengthSq( &vDist );		// 아이템 줍는사람과 멤버간의 거리.
									if( fDist < 32.0f * 32.0f )		// xx미터보다 작으면

									{
										pListMember[ nMaxListMember++ ] = pMember;
									}
								}
							}
							ADDERRORSEQ( _i, 1015 );
							if( nMaxListMember == 0 )
							{
								pGetUser	= (CUser*)pMover;
							}
							else
							{
								switch( pParty->m_nTroupeShareItem )
								{
								case 1 :	// 순서분배

									{
										ADDERRORSEQ( _i, 1016 );
										for( i = 0 ; i < nMaxListMember ; i++ )
										{
											if( pParty->m_nGetItemPlayerId == pListMember[i]->m_idPlayer )
											{
												if( i + 1 >= nMaxListMember )
												{
													pGetUser = pListMember[0];
													break;												
												}
												else
												{
													pGetUser = pListMember[ i+1 ];
													break;
												}
											}
										}
										
										if( pGetUser == NULL )
										{
											pGetUser = pListMember[0];
										}
									}
									break;
								case 2 :	// 수동분배 : 단장에게 다 들어감
									{
										ADDERRORSEQ( _i, 1017 );
										if( pParty->IsLeader( pListMember[0]->m_idPlayer ) )
										{
											pGetUser = pListMember[0];
										}
										else
										{
											pGetUser = (CUser*)pMover;
										}
									}
									break;
								case 3 :	// 랜덤분배 : 랜덤하게 들어감
									{
										DWORD dwRand = xRandom( nMaxListMember );
										pGetUser = pListMember[dwRand];
									}
									break;
								default:	// 기본 개임분배

									{
										pGetUser = (CUser*)pMover; 
									}
									break;
								}	// switch
							}	
							ADDERRORSEQ( _i, 1018 );
							pParty->m_nGetItemPlayerId = pGetUser->m_idPlayer;

							bSuccess	= pGetUser->CreateItem( pItemBase );

							for( i = 0; i < nMaxListMember; i++ )
							{
								pMember = pListMember[i];
								if( TRUE == pMember->IsPlayer() && pItemBase->GetProp() != NULL )
								{
									if( bSuccess )
									{
										if( pMember->m_idPlayer != pGetUser->m_idPlayer )
										{
											if( prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN || prj.m_nLanguage == LANG_JAP )
												( (CUser*)pMember )->AddDefinedText( TID_GAME_TROUPEREAPITEM, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
											else
											{
  #ifdef __THAILAND__
												( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM_THA, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
  #else //__THAILAND__
												( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM, "\"%s%s %s\"", pGetUser->GetName(), prj.GetText(TID_GAME_FROM2), pItemBase->GetProp()->szName );
  #endif //__THAILAND__
											}
											//( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM, "\"%s님이 %s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
										}
										else
										{
											( (CUser*)pGetUser )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );
										}
									}
									else
									{
										// CreateItem을 실패함 Full
										( (CUser*)pMover )->AddDefinedText( TID_GAME_LACKSPACE );
									}
								}									
							}
							ADDERRORSEQ( _i, 1019 );
							
  #ifdef __WORLDSERVER
							if( pItemBase->GetProp()->nLog != -1 && bSuccess )
							{
								switch( pItemBase->GetProp()->dwItemKind2 ) 
								{
								case IK2_FOOD:
								case IK2_MAGIC:
								case IK2_POTION:
								case IK2_REFRESHER:
									{
										if( ( ( (CItemElem*)pItemBase )->m_nItemNum ) > 9 )
										{
											LogItemInfo aLogItem;
											aLogItem.Action = "R";
											aLogItem.SendName = "GROUND";
											aLogItem.RecvName = pGetUser->GetName();
											aLogItem.WorldId = pGetUser->GetWorld()->GetID();
											aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
											g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
										}
										break;
									}
								default:
									{
										LogItemInfo aLogItem;
										aLogItem.Action = "R";
										aLogItem.SendName = "GROUND";
										aLogItem.RecvName = pGetUser->GetName();
										aLogItem.WorldId = pGetUser->GetWorld()->GetID();
										aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
										g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
										break;
									}
								}
							}
  #endif // __WORLDSERVER
							ADDERRORSEQ( _i, 1020 );
						}
						else // 파티가 없을때~
						{
							ADDERRORSEQ( _i, 1021 );
							bSuccess	= pMover->CreateItem( pItemBase );

							if( TRUE == pMover->IsPlayer() && pItemBase->GetProp() != NULL )
							{
								if( bSuccess )
									( (CUser*)pMover )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );
								else
									( (CUser*)pMover )->AddDefinedText( TID_GAME_LACKSPACE );
							}

  #ifdef __WORLDSERVER
							ADDERRORSEQ( _i, 1022 );
							if( pItemBase->GetProp()->nLog != -1 && bSuccess )
							{
								switch( pItemBase->GetProp()->dwItemKind2 ) 
								{
								case IK2_FOOD:
								case IK2_MAGIC:
								case IK2_POTION:
								case IK2_REFRESHER:
									{
										if( ( (CItemElem*)pItemBase )->m_nItemNum > 9 )
										{
											LogItemInfo aLogItem;
											aLogItem.Action = "R";
											aLogItem.SendName = "GROUND";
											aLogItem.RecvName = pMover->GetName();
											aLogItem.WorldId = pMover->GetWorld()->GetID();
											aLogItem.Gold = aLogItem.Gold2 = pMover->GetGold();
											g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
										}
										break;
									}
								default:
									{
										LogItemInfo aLogItem;
										aLogItem.Action = "R";
										aLogItem.SendName = "GROUND";
										aLogItem.RecvName = pMover->GetName();
										aLogItem.WorldId = pMover->GetWorld()->GetID();
										aLogItem.Gold = aLogItem.Gold2 = pMover->GetGold();
										g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
										break;
									}
								}
							}
							ADDERRORSEQ( _i, 1023 );
  #endif // __WORLDSERVER
						}
  #ifdef __WORLDSERVER
						g_PartyMng.m_AddRemoveLock.Leave( theLineFile );	// unlock2
  #endif	// __WORLDSERVER
					}
					else	// 어떤유저가 떨어뜨린것은 줏은넘이 임자~
					{
						ADDERRORSEQ( _i, 1024 );
						bSuccess	= pMover->CreateItem( pItemBase );
						if( TRUE == pMover->IsPlayer() && pItemBase->GetProp() != NULL )
						{
							if( bSuccess )
								( (CUser*)pMover )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );						
							else
								( (CUser*)pMover )->AddDefinedText( TID_GAME_LACKSPACE );
						}

  #ifdef __WORLDSERVER
						ADDERRORSEQ( _i, 1025 );
						if( pItemBase->GetProp()->nLog != -1 && bSuccess )
						{
							switch( pItemBase->GetProp()->dwItemKind2 ) 
							{
							case IK2_FOOD:
							case IK2_MAGIC:
							case IK2_POTION:
							case IK2_REFRESHER:
								{
									if( ( (CItemElem*)pItemBase )->m_nItemNum > 9 )
									{
										LogItemInfo aLogItem;
										aLogItem.Action = "R";
										aLogItem.SendName = "GROUND";
										aLogItem.RecvName = pMover->GetName();
										aLogItem.WorldId = pMover->GetWorld()->GetID();
										aLogItem.Gold = aLogItem.Gold2 = pMover->GetGold();
										g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
									}
									break;
								}
							default:
								{
									LogItemInfo aLogItem;
									aLogItem.Action = "R";
									aLogItem.SendName = "GROUND";
									aLogItem.RecvName = pMover->GetName();
									aLogItem.WorldId = pMover->GetWorld()->GetID();
									aLogItem.Gold = aLogItem.Gold2 = pMover->GetGold();
									g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
									break;
								}
							}
						}
						ADDERRORSEQ( _i, 1026 );
  #endif // __WORLDSERVER
					}
 #else	// __WORLDSERVER
  #ifndef __ONLINE
					ADDERRORSEQ( _i, 1027 );
					bSuccess	= pMover->AddItem( pItemBase );
					ADDERRORSEQ( _i, 1028 );
  #endif	// __ONLINE
 #endif	// not __WORLDSERVER
				}
				if( TRUE == bSuccess )
				{
					ADDERRORSEQ( _i, 1029 );
					pItem->Delete();
					ADDERRORSEQ( _i, 1030 );
#ifdef __WORLDSERVER
					ADDERRORSEQ( _i, 1031 );
//					g_UserMng.AddActMsg( pMover, OBJMSG_PICKUP, 0, 0, TRUE );		// 클라에 줍는동작하도록 보냄.
					g_UserMng.AddMotion( pMover, OBJMSG_PICKUP );
					ADDERRORSEQ( _i, 1032 );

					ADDERRORSEQ( _i, 1033 );
					pMover->RemoveInvisible();		// 아이템 줏으면 투명 해제.
					ADDERRORSEQ( _i, 1034 );
#endif	// __WORLDSERVER
				}
//}}AFX
#endif // NOT xPet

				ADDERRORSEQ( _i, 1035 );
				pItem->Unlock( theLineFile );	// unlock
				ADDERRORSEQ( _i, 1036 );
			}
			else
			if( pObj->GetType() == OT_MOVER )
			{
			}
			else if( pObj->GetType() == OT_CTRL )
			{
				if( IsValidObj( (CObj*)pObj ) )
				{
					CCommonCtrl*  pCommonCtrl = (CCommonCtrl*)pObj;

					if( !pCommonCtrl )
						return FALSE;

					CItemElem* pKey = NULL;

					// 오브젝트 동작 조건 검사
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM )
					{
					#ifdef __S_DONGEON_0504
						// 아이템과 아이템 개수와 내구력이 둘중 있어야 열수 있음
						BOOL bResult = FALSE;
						pKey = pMover->m_Inventory.GetAtItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
						if( IsUsableItem( pKey ) )
						{
							int nItemCount = pMover->m_Inventory.GetAtItemNum( pCommonCtrl->m_CtrlElem.m_dwSetItem );	// 아이템 갯수 꺼내기						
							if( nItemCount < pCommonCtrl->m_CtrlElem.m_dwSetItemCount && pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )	
								bResult = TRUE;
						}
						else
						{
							if( pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )
								bResult = TRUE;
						}
						
						if( bResult )
						{
						#ifdef __WORLDSERVER		
							((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE1, "\"%s\"", prj.GetItemProp(pCommonCtrl->m_CtrlElem.m_dwSetItem)->szName );
						#endif//__WORLDSERVER
							return FALSE;
						}							
					#else // __S_DONGEON_0504
						pKey = pMover->m_Inventory.GetAtItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
						// 아이템과 내구력이 없으면 열수없다. 둘중 하나라도 있어야 열수있음
						if( IsUsableItem( pKey ) == FALSE || pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )
						{
						#ifdef __WORLDSERVER		
							((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE1, "\"%s\"", prj.GetItemProp(pCommonCtrl->m_CtrlElem.m_dwSetItem)->szName );
						#endif//__WORLDSERVER
							return FALSE;
						}
					#endif // __S_DONGEON_0504
					}
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_LEVEL )
					{
						if( pMover->GetLevel() < pCommonCtrl->m_CtrlElem.m_dwSetLevel )
						{
						#ifdef __WORLDSERVER		
							((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE2 );
						#endif//__WORLDSERVER
							return FALSE;
						}
					}
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST )
					{
						LPQUEST pQuest = pMover->GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum );

						if( pQuest )
						{
							if( pQuest->m_nState != pCommonCtrl->m_CtrlElem.m_dwSetFlagNum )
							{
			#ifdef __WORLDSERVER	
								QuestProp * pQuestPorp = prj.m_aPropQuest.GetAt( pQuest->m_wId );

								if( pQuestPorp )
									((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle );
			#endif//__WORLDSERVER
								return FALSE;
							}
						}
						else
						{
			#ifdef __WORLDSERVER	
								QuestProp * pQuestPorp = prj.m_aPropQuest.GetAt( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum );

								if( pQuestPorp )
									((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle );
			#endif//__WORLDSERVER
								return FALSE;
						}
					}
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_CLASS )
					{
						if( pMover->GetJob() != pCommonCtrl->m_CtrlElem.m_bSetJob[pMover->GetJob()] )
						{
			#ifdef __WORLDSERVER		
							((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE5 );
			#endif//__WORLDSERVER
							return FALSE;
						}
					}
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_GENDER )
					{
						if( pMover->GetSex() != pCommonCtrl->m_CtrlElem.m_dwSetGender )
						{
			#ifdef __WORLDSERVER		
							((CUser*)pMover)->AddDefinedText( TID_ITEMBOX_MESSAGE3 );
			#endif//__WORLDSERVER
							return FALSE;
						}
					}

					if( pMover )
						pCommonCtrl->m_nMoverID = pMover->GetId();

//					if( pCommonCtrl->m_pModel && pCommonCtrl->m_pModel->m_fFrameCurrent != 0.0f )
//						return FALSE;		
					
#ifdef __WORLDSERVER	
#ifdef __S_DONGEON_0504
#ifdef __NPP_050308
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_TELEPORT )
					{
						if( pMover->IsFly() == FALSE )
						{
							D3DXVECTOR3 vPos( pCommonCtrl->m_CtrlElem.m_dwTeleX, pCommonCtrl->m_CtrlElem.m_dwTeleY, pCommonCtrl->m_CtrlElem.m_dwTeleZ );
							pMover->Replace( g_uIdofMulti, pCommonCtrl->m_CtrlElem.m_dwTeleWorldId, vPos );
						}
						else
						{
							((CUser*)pMover)->AddDefinedText( TID_PK_NO_FLIGHTAREA );
						}
					}
					// 퀘스트
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST_END )
					{
						if( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 != 0 )
						{
							LPQUEST pQuest = pMover->GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 );
							if( pQuest )
							{
								QUEST quest;
								if( ((CUser*)pMover)->SetQuest( pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum1, &quest ) )
								{
									((CUser*)pMover)->AddSetQuest( &quest );
								}
							}
						}
						if( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 != 0 )
						{
							LPQUEST pQuest = pMover->GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 );
							if( pQuest )
							{
								QUEST quest;
								if( ((CUser*)pMover)->SetQuest( pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum2, &quest ) )
								{
									((CUser*)pMover)->AddSetQuest( &quest );
								}
							}
						}
					}

					// 아이템 삭제
					if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM )
					{
						int nItemCount = pCommonCtrl->m_CtrlElem.m_dwSetItemCount;
						for( int i = 0; i < 10 && nItemCount > 0; ++i )
						{
							pKey = pMover->m_Inventory.GetAtItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
							if( IsUsableItem( pKey ) )
							{
								if( nItemCount <= pKey->m_nItemNum )
								{
									pMover->RemoveItem( (BYTE)pKey->m_dwObjId, (short)nItemCount );
									nItemCount = 0;
								}
								else
								{
									pMover->RemoveItem( (BYTE)pKey->m_dwObjId, (short)pKey->m_nItemNum );
									nItemCount -= pKey->m_nItemNum;
								}							
							}
						}
					}
#endif // NPP거시기
#else // __S_DONGEON_0504
					if( pKey )
					{
						pMover->RemoveItem( (BYTE)pKey->m_dwObjId, (short)1 );
					}
#endif // __S_DONGEON_0504

					pCommonCtrl->SetActionPlay();
#endif//__WORLDSERVER

				}
			}
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
#ifndef __WORLDSERVER
#endif	// __WORLDSERVER
	//SendAIMsg( AIMSG_EXIT );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	
//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

	EndAIHandler( )
	ADDERRORSEQ( _i, 1099 );
	return TRUE;
}

