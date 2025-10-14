// FunnyCoin.cpp: implementation of the CFunnyCoin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FunnyCoin.h"

const int FC_DBSUCCESS = 1;
const int FC_DBFAILED = -1;
const int FC_SAMEUNIQUENO = -2;

#ifdef __WORLDSERVER
//////////////////////////////////////////////////////////////////////
// CFunnyCoin ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include "../WorldServer/User.h"

CFunnyCoin::CFunnyCoin()
{
	m_vecAckWaitPlayer.clear();
}

CFunnyCoin::~CFunnyCoin()
{
	for( size_t i=0; i<m_vecAckWaitPlayer.size(); i++ )
		FLERROR_LOG( PROGRAM_NAME, _T( "FunnyCoin AckWait Player - %07d" ), m_vecAckWaitPlayer.at( i ) );
	
	m_vecAckWaitPlayer.clear();
}

CFunnyCoin* CFunnyCoin::GetInstance()
{
	static CFunnyCoin sFunnyCoin;
	return & sFunnyCoin;
}

BOOL CFunnyCoin::DoUseFunnyCoin( FLWSUser* pUser, FLItemElem* pItemElem )	// 퍼니코인을 사용한다.
{
	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
	{
		return FALSE;
	}

	if ( !IsUsable( pUser ) )
		return FALSE;
	
	m_vecAckWaitPlayer.push_back( pUser->m_idPlayer );
	g_dpDBClient.SendFunnyCoinReqUse( pUser->m_idPlayer, pItemElem );	// Trans Server 로 전송
	g_DPSrvr.PutItemLog( pUser, "M", "FUNNYCOIN_USE", pItemElem );
	
	return TRUE;
}

void CFunnyCoin::OnFunnyCoinAckUse( CAr & ar )	// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.
{
	DWORD dwPlayerId = 0, dwItemId = 0;
	SERIALNUMBER iSerialNumber = 0;
	int nResult = 0;

	ar >> dwPlayerId >> dwItemId >> iSerialNumber >> nResult;

	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
	{
		return;
	}

	if( IsAckWaitPlayer( dwPlayerId, TRUE ) )
	{
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( dwPlayerId ) );	
		switch( nResult )
		{
		case FC_DBSUCCESS:
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
				if( IsValidObj( pUser ) && pItemProp )
					pUser->AddDefinedText( TID_GAME_FUNNYCOIN_USECOMPLETE, "\"%s\"", pItemProp->szName );
			}
			break;

		case FC_DBFAILED:
			{
				if( IsValidObj( pUser ) )
				{
					FLItemElem itemElem;
					itemElem.m_dwItemId = dwItemId;
					itemElem.m_nItemNum = 1;
					itemElem.SetSerialNumber( iSerialNumber );		// 기존 시리얼 넘버로 복구한다.
					if( pUser->CreateItem( &itemElem ) )	
						g_DPSrvr.PutItemLog( pUser, "M", "FUNNYCOIN_REPAIR_SUCCESS", &itemElem );
					else	// 인벤토리가 가득차는 등의 이유로 아이템 복구 실패!!
					{
						g_DPSrvr.PutItemLog( pUser, "M", "FUNNYCOIN_REPAIR_FAILED", &itemElem );
						FLERROR_LOG( PROGRAM_NAME, _T( "FunnyCoin Repair Failed! [PlayerId:%07d], [Item:%d], [SerialNumber:%d]" ),
								dwPlayerId, dwItemId, iSerialNumber );
					}
				}
				else	// 해당 플레이어가 접속 중이지 않다.
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "FunnyCoin Repair Failed!(Invalid User) [PlayerId:%07d], [Item:%d], [SerialNumber:%d]" ),
							dwPlayerId, dwItemId, iSerialNumber );
				}
			}
			break;

		case FC_SAMEUNIQUENO:
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Same SerialNumber! [PlayerId:%07d], [Item:%d], [SerialNumber:%d]" ),
						dwPlayerId, dwItemId, iSerialNumber );
			}
			break;
		}
	}
	else	// 퍼니코인 사용 응답 대기 플레이어가 아니다.
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Is Not AckWait Player [PlayerId:%07d]" ), dwPlayerId );
	}
}

BOOL CFunnyCoin::IsAckWaitPlayer( DWORD dwPlayerId, BOOL bRemove )	// 퍼니코인을 사용한 후 DB응답을 기다리고 있는 플레이어 인가?
{																	// bRemove가 TRUE이면 해당 사용자를 삭제한다.
	for( std::vector<DWORD>::iterator it=m_vecAckWaitPlayer.begin(); it!=m_vecAckWaitPlayer.end(); it++ )
	{
		if( (*it) == dwPlayerId )
		{
			if( bRemove )	
				m_vecAckWaitPlayer.erase( it );

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFunnyCoin::IsUsable( FLWSUser* pUser )
{
	switch( g_xFlyffConfig->GetMainLanguage() )
	{
		case LANG_KOR :	
			{
				CString strTemp;
				strTemp.Format( "%s", pUser->m_playAccount.lpszAccount );
				if( strTemp.Right( 4 ) == "__bu" )
				{
					pUser->AddDefinedText( TID_GAME_FUNNYCOIN_NOUSEBUDDYPLAYER );
					return FALSE;
				}

				if( IsAckWaitPlayer( pUser->m_idPlayer ) )
				{
					//pUser->AddText( "잠시후에 다시 사용하게나..." );
					return FALSE;
				}

				return TRUE;
			}
	}
	
	return FALSE;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
//////////////////////////////////////////////////////////////////////
// CFunnyCoinDbCtrl //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include "../DatabaseServer/dptrans.h"
extern AppInfo g_appInfo;

CFunnyCoinDbCtrl::CFunnyCoinDbCtrl()
{
}

CFunnyCoinDbCtrl::~CFunnyCoinDbCtrl()
{
}

void CFunnyCoinDbCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, ULONG_PTR dwCompletionKey )
{
	switch( pov->nQueryMode )
	{
	case FC_USE:
		{
			DWORD dwPlayerId = 0, dwItemId = 0;
			SERIALNUMBER iSerialNumber = 0;

			CAr ar( pov->lpBuf, pov->uBufSize );
			ar >> dwPlayerId >> dwItemId >> iSerialNumber;

			int nResult = InsertFunnyCoin( dwPlayerId, dwItemId, iSerialNumber );
			CDPTrans::GetInstance()->SendFunnyCoinAckUse( dwPlayerId, dwItemId, iSerialNumber, nResult, dwCompletionKey );
		}
		break;

	default:
		break;
	}
}

int CFunnyCoinDbCtrl::InsertFunnyCoin( DWORD dwPlayerId, DWORD dwItemId, SERIALNUMBER iSerialNumber )	// 사용한 퍼니코인을 DB에 저장한다.
{
	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
	{
		return FC_DBFAILED;
	}

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
	if( !pItemProp )
		return FC_DBFAILED;
	
	CQuery* pQuery = GetQueryObject();
	if( !pQuery->Execute( "usp_FunnyCoin_input '%02d', '%07d', %d, %d, %d", g_appInfo.dwSys, dwPlayerId, dwItemId, pItemProp->dwCost, iSerialNumber ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Call \"usp_FunnyCoin_input\" FAILED! - %07d, %d" ), dwPlayerId, dwItemId );
		return FC_DBFAILED;
	}

	if( pQuery->Fetch() )
	{
		int nResult = pQuery->GetInt( "result" );
		if( nResult != FC_DBSUCCESS )	// 실패chipi	
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "usp_FunnyCoin_input Result is Fail - ErrorCode : %d, %07d, %d, %d" ), nResult, dwPlayerId, dwItemId, iSerialNumber );
			return nResult;
		}
	}
	else
	{
		return FC_DBFAILED;
	}

	// 성공 했다!!
	return FC_DBSUCCESS;
}

//////////////////////////////////////////////////////////////////////
// CFunnyCoin ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CFunnyCoinDbMng::CFunnyCoinDbMng()
{
	if( !m_FCDbCtrl.CreateDbHandler() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_FCDbCtrl.CreateDbHandler()" ) );
	}
}

CFunnyCoinDbMng::~CFunnyCoinDbMng()
{
	m_FCDbCtrl.CloseDbHandler();
}

CFunnyCoinDbMng* CFunnyCoinDbMng::GetInstance()
{
	static CFunnyCoinDbMng sFunnyCoinDbMng;
	return &sFunnyCoinDbMng;
}
#endif // __DBSERVER
