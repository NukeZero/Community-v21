#include "StdAfx.h"

#include "tools.h"
extern char	g_szVersion[];

#include "DPCertified.h"
#include "dpclient.h"
#include "DPLoginClient.h"
#include "Network.h"
#include "Wnd2ndPassword.h"


#ifdef _USE_XTRAP

#include <xtrap/20110503_522_Xtrap_C_Interface.h>
#include <xtrap/Server/20101129_525_Xtrap_S_Interface.h>

#endif //_USE_XTRAP


#include "NPGameLib.h"
extern CNPGameLib* GetNProtect();

extern	CDPClient		g_DPlay;
extern	CDPLoginClient	g_dpLoginClient;
 
CDPLoginClient::CDPLoginClient()
{
	m_timer.Set( SEC( 10 ) );
	m_fConn	= FALSE;
	m_nSlot	= -1;
	m_bShowDisconnectMsg = TRUE;
	m_lError = 0;
	m_idNumberPad = 0;

	ON_MSG( PACKETTYPE_ERROR, OnError );
	ON_MSG( PACKETTYPE_PLAYER_LIST, OnPlayerList );
	ON_MSG( PACKETTYPE_CACHE_ADDR, OnCacheAddr );
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, OnQueryTickCount );
	ON_MSG( PACKETTYPE_PRE_JOIN, OnPreJoin );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, OnOneHourNotify );
	
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_AUTHQUERY, OnAuthQuery );
#ifdef __gameguard
	ON_MSG( PACKETTYPE_AUTHQUERY_VER30, OnAuthQueryVer30 );
#endif
	ON_MSG( PACKETTYPE_LOGIN_PROTECT_NUMPAD, OnLoginNumPad );
	ON_MSG( PACKETTYPE_LOGIN_PROTECT_CERT, OnLoginProtect );

#ifdef _USE_XTRAP
	ON_MSG( PACKETTYPE_XTRAP_COMMAND_REQUEST, OnXTrapCommandRequest );
#endif //_USE_XTRAP
}

CDPLoginClient::~CDPLoginClient()
{

}

LONG CDPLoginClient::GetNetError()
{
	return m_lError;
}


void CDPLoginClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			m_fConn		= TRUE;
			break;

		case DPSYS_DESTROYPLAYERORGROUP:
			{
				{
					LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
					m_lError = lpDestroyPlayer->dwFlags;
				}

				CNetwork::GetInstance().OnEvent( LOGIN_DISCONNECT );

				m_fConn		= FALSE;
				g_DPlay.DeleteDPObject();

				if( m_bShowDisconnectMsg )
					g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_0023), MB_OK, TRUE );	//서버로부터 접속이 해제되었습니다.		  
				m_bShowDisconnectMsg = TRUE;
				break;
			}
	}
}

void CDPLoginClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) 
		( this->*( pfn ) )( ar );
}

void CDPLoginClient::QueryTickCount( void )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERYTICKCOUNT, DPID_UNKNOWN );
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::OnQueryTickCount( CAr & ar )
{
	DWORD dwTime;
	__int64 nTickCount;
	ar >> dwTime >> nTickCount;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );

	FLTRACE_LOG( PROGRAM_NAME, _T( "OnQueryTickCount, %d" ), dwTime );
}

void CDPLoginClient::SendGetPlayerList( DWORD dwID, LPCSTR lpszAccount, LPCSTR lpszpw, BYTE bySelectPage )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETPLAYERLIST, DPID_UNKNOWN );
	ar << g_Neuz.m_dwAuthKey;
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszpw );
	ar << dwID;
	ar << bySelectPage;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::SendCreatePlayer(BYTE nSlot, LPCSTR lpszPlayer/*, LPDWORD adwEquipment*/, BYTE nFace, BYTE nCostume, BYTE nSkinSet, BYTE nHairMesh, DWORD dwHairColor, BYTE nSex, BYTE nJob, BYTE nHeadMesh, int nBankPW, BYTE bySelectPage )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CREATE_PLAYER, DPID_UNKNOWN );
	ar.WriteString( g_Neuz.m_bGPotatoAuth?g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount );
	ar.WriteString( g_Neuz.m_szPassword );
	ar << nSlot;

	if( strlen( lpszPlayer ) > 16 )
		FLERROR_LOG( PROGRAM_NAME, _T( "이상 캐릭터 명 E: %s" ), lpszPlayer );
	ar.WriteString( lpszPlayer );
	if( strlen( lpszPlayer ) > 16 )
		FLERROR_LOG( PROGRAM_NAME, _T( "이상 캐릭터 명 F: %s" ), lpszPlayer );
	
	//	ar.Write( adwEquipment, sizeof(DWORD) * MAX_HUMAN_PARTS );

	ar << nFace << nCostume << nSkinSet << nHairMesh;
	ar << dwHairColor;
	ar << nSex << nJob << nHeadMesh;
	ar << nBankPW;
	ar << g_Neuz.m_dwAuthKey;

	// BEGIN110520
	ar << bySelectPage;
	// END110520

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::SendDeletePlayer( BYTE nSlot, LPCTSTR szNo, BYTE bySelectPage )
{
	if(nSlot >= MAX_CHARACTER_SLOT ) {
		return;
	}

	BEFORESENDSOLE( ar, PACKETTYPE_DEL_PLAYER, DPID_UNKNOWN );

	ar.WriteString( g_Neuz.m_bGPotatoAuth? g_Neuz.m_szGPotatoNo: g_Neuz.m_szAccount );
	ar.WriteString( g_Neuz.m_szPassword );
	ar.WriteString( szNo );
	ar << g_Neuz.m_apPlayer[nSlot]->m_idPlayer;
	ar << g_Neuz.m_dwAuthKey;
	ar << g_Neuz.m_aRTMessenger[nSlot].size();
	for( CRTMessenger::iterator it=g_Neuz.m_aRTMessenger[nSlot].begin(); it!=g_Neuz.m_aRTMessenger[nSlot].end(); it++ )
	{
		ar << it->first;
	}
	ar << bySelectPage;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::SendPreJoin( const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer, int nSlot, int nSecretNum )
{
	m_nSlot		= nSlot;
	BEFORESENDSOLE( ar, PACKETTYPE_PRE_JOIN, DPID_UNKNOWN );
//	ar << g_Neuz.m_dwAuthKey;
	ar.WriteString( lpszAccount );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << nSecretNum;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPLoginClient::OnPreJoin( CAr & ar )
{
	CNetwork::GetInstance().OnEvent( LOGIN_ACK_PREJOIN );

	// ata2k - (1)시간 해제
	g_Neuz.m_dwTimeOutDis = 0xffffffff;
	
	// Open world here.	
	g_DPlay.SendJoin( (BYTE)m_nSlot, g_Neuz.m_adwWorldID[m_nSlot], g_Neuz.m_apPlayer[m_nSlot], &g_Neuz.m_aRTMessenger[m_nSlot], g_Neuz.m_uIdofMulti );	

	// ata2k - (2)시간 저장
	// todo: lower timeout?
	g_Neuz.m_dwTimeOutDis = GetTickCount() + MIN( 5 );
	
}

void	CDPLoginClient::RemoteShowDisconnectMsg( BOOL bShow )
{
	m_bShowDisconnectMsg	 = bShow;
}

void CDPLoginClient::OnOneHourNotify( CAr & ar )
{
	g_WndMng.OpenMessageBox( prj.GetText(TID_DIAG_EXPIRYTIME) );
}

void CDPLoginClient::OnPing( CAr & ar )
{
	DWORD dwPingTime;
	ar >> dwPingTime;
	g_Neuz.m_dwPingTime = GetCurrentTime() - dwPingTime;
	g_Neuz.m_nPingCount = 0;
}
#ifdef PASSWORD_RESET_2ND
void CDPLoginClient::SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CHANGEBANKPASS, DPID_UNKNOWN );
	ar.WriteString( szName );
	ar.WriteString( szNewPass );
	ar << uidPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // PASSWORD_RESET_2ND

void CDPLoginClient::OnError( CAr & ar )
{
	ar >> m_lError;

	CNetwork::GetInstance().OnEvent( LOGIN_ERROR );

	switch( m_lError )
	{
	case ERROR_DUPLICATE_ACCOUNT:
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0042) ) );
//				g_WndMng.OpenMessageBox( _T( "접속중인 계정입니다." ) );
			break;
		}
	case ERROR_USER_EXISTS:
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0034) ) );
//				g_WndMng.OpenMessageBox( _T( "이미 사용중인 캐릭터 이름입니다" ) );
			CWndCreateChar* pWnd = (CWndCreateChar*)g_WndMng.GetWndBase( APP_CREATE_CHAR );
			if( pWnd )
			{
				pWnd->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
				pWnd->GetDlgItem( WIDC_CANCEL )->EnableWindow( TRUE );
			}
			break;
		}
	case ERROR_DUPLICATE_SLOT:
		{
			CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CREATE_CHAR );
			if( pWndBase )
				( (CWndCreateChar*)pWndBase )->Destroy();
			g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0018) ) );
//				g_WndMng.OpenMessageBox( _T( "사용중인 슬롯 입니다." ) );
			break;
		}
	case ERROR_ILLEGAL_VER:
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0035) ) );
//				g_WndMng.OpenMessageBox( _T( "이전 버전의 클라이언트입니다" ) );
			break;
		}
	case ERROR_INVALID_NAME_CHARACTER:
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
//				g_WndMng.OpenMessageBox( _T( "사용할수 없는 이름입니다" ) );
			CWndCreateChar* pWnd = (CWndCreateChar*)g_WndMng.GetWndBase( APP_CREATE_CHAR );
			if( pWnd )
				pWnd->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
			break;
		}
	case ERROR_NO_SUCH_GROUP:
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0039) ) );
//				g_WndMng.OpenMessageBox( _T( "잘못입력하였습니다. 다시 입력하여 주십시요" ) );
			CWndDeleteChar* pWnd = (CWndDeleteChar*)g_WndMng.GetWndBase( APP_DELETE_CHAR );
			if( pWnd )
			{
				CWndEdit* pWndEdit = (CWndEdit*)pWnd->GetDlgItem( WIDC_EDIT1 );
				pWndEdit->SetString( "" );
				CWndButton* pWndButton = (CWndButton*)pWnd->GetDlgItem( WIDC_OK );
				pWndButton->EnableWindow( TRUE );
				pWndButton	= (CWndButton*)pWnd->GetDlgItem( WIDC_CANCEL );
				pWndButton->EnableWindow( TRUE );	
			}
			break;
		}

	case ERROR_WARTIME:
		{
			CWndDeleteChar* pWnd = (CWndDeleteChar*)g_WndMng.GetWndBase( APP_DELETE_CHAR );
			if( pWnd )
			{
				CWndEdit* pWndEdit = (CWndEdit*)pWnd->GetDlgItem( WIDC_EDIT1 );
				pWndEdit->SetString( "" );
				CWndButton* pWndButton = (CWndButton*)pWnd->GetDlgItem( WIDC_OK );
				pWndButton->EnableWindow( TRUE );
				pWndButton	= (CWndButton*)pWnd->GetDlgItem( WIDC_CANCEL );
				pWndButton->EnableWindow( TRUE );	
			}
			break;
		}
	case ERROR_BILLING_TIME_OVER:			// 플레이 도중 사용기간 만료
		m_bShowDisconnectMsg = FALSE;       // 서버로 부터 접속이 종료되었습니다를 표시하지 않음 
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_PLAYNOCHARGING), MB_OK, TRUE );  // TRUE - 메세지 표시후 로그인화면으로 
		break;
	case ERROR_15MIN_PREVENT:	// 2차 비밀번호 3회이상 틀렸을 경우
		{
			g_WndMng.CloseMessageBox();
			g_WndMng.OpenMessageBox( _T( prj.GetText( TID_15MIN_PREVENT ) ) );
			g_Neuz.m_dwTimeOutDis = 0xffffffff;
			break;
		}
	case ERROR_USER_OVERFLOW:	// 캐릭터 생성 수 초과
		{
			CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CREATE_CHAR );
			if( pWndBase )
			{
				( (CWndCreateChar*)pWndBase )->Destroy();
			}
			g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
			g_WndMng.OpenMessageBox( _T( prj.GetText( TID_MMI_CREATECHAR_OVERFLOW ) ) );
			break;
		}
	case ERROR_SLOT_DONOT_CREATE:
		{
			CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CREATE_CHAR );
			if( pWndBase )
			{
				( (CWndCreateChar*)pWndBase )->Destroy();
			}

			g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
			CWndSelectChar* pWndSelectChar = (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
			if( pWndSelectChar )
				pWndSelectChar->UpdateCharacter();

			g_WndMng.OpenMessageBox( _T( prj.GetText( TID_MMI_CREATECHAR_OVERFLOW ) ) );
		}
		break;
	case ERROR_SLOT_DONOT_DELETE:
		{
			CWndDeleteChar* pWnd = (CWndDeleteChar*)g_WndMng.GetWndBase( APP_DELETE_CHAR );
			if( pWnd )
			{
				( (CWndDeleteChar*)pWnd )->Destroy();
			}

			g_WndMng.OpenMessageBox( _T( prj.GetText( TID_MMI_DELETECHAR_ERROR ) ) );
		}
		break;
	}
}

void CDPLoginClient::OnPlayerList( CAr & ar )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;

	CWndBase* pWndBase = g_WndMng.GetWndBase( APP_SELECT_SERVER );
	if( pWndBase )
		((CWndSelectServer*)pWndBase)->Connected();
	
	if( ( pWndBase = g_WndMng.GetWndBase( APP_CREATE_CHAR ) ) )
	{
		( (CWndCreateChar*)pWndBase )->Destroy();
		g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SELECT_CHAR );
	}
	if( ( pWndBase = g_WndMng.GetWndBase( APP_DELETE_CHAR ) ) )
	{
		( (CWndDeleteChar*)pWndBase )->Destroy();
	}
	if( ( pWndBase = g_WndMng.GetWndBase( APP_2ND_PASSWORD_NUMBERPAD ) ) )
	{
		( (CWnd2ndPassword*)pWndBase )->Destroy();
	}

	DWORD dwAuthKey;
	ar >> dwAuthKey;
	if( dwAuthKey != g_Neuz.m_dwAuthKey )
	{
		CNetwork::GetInstance().OnEvent( LOGIN_AUTHKEY_ERROR );
		return;
	}

	DWORD dwIndex;
	for( int i = 0 ; i < MAX_CHARACTER_SLOT ; i++ )
	{
		g_Neuz.m_nCharacterBlock[i] = 2; // 모두 빈슬롯으로 함
		SAFE_DELETE( g_Neuz.m_apPlayer[i] );
	}
	
	int countPlayer = 0;
	ar >> countPlayer;

	for( int i = 0 ; i < countPlayer ; i++ )
	{
//		CMover* pMover;
		BYTE bySex;
		BYTE bySlot;	
		DWORD dwJobLv;

		ar >> bySlot;

		ar >> g_Neuz.m_nCharacterBlock[bySlot];
		
		ar >> g_Neuz.m_adwWorldID[bySlot];
		ar >> dwIndex;


		CMover* pMover = new CMover;
		pMover->m_dwType = OT_MOVER;
		pMover->m_dwIndex = dwIndex;
		g_Neuz.m_apPlayer[bySlot]	= pMover;

		//g_Neuz.m_apPlayer[slot]	= (CMover*)CreateObj( g_Neuz.m_pd3dDevice, OT_MOVER, dwIndex, FALSE );
		g_Neuz.m_apPlayer[bySlot]->m_bPlayer	= TRUE;
		g_Neuz.m_apPlayer[bySlot]->InitProp();

		ar.ReadString(  (char*)g_Neuz.m_apPlayer[bySlot]->GetName(), MAX_NAME );
		D3DXVECTOR3 Pos;
		ar >> Pos;
		g_Neuz.m_apPlayer[bySlot]->SetPos(Pos);
		ar >> g_Neuz.m_apPlayer[bySlot]->m_idPlayer;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_idparty;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_idGuild;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_idWar;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_dwSkinSet;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_dwHairMesh;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_dwHairColor;

		ar >> g_Neuz.m_apPlayer[bySlot]->m_dwHeadMesh;
		ar >> bySex;
		g_Neuz.m_apPlayer[bySlot]->SetSex( bySex );
		ar >> g_Neuz.m_apPlayer[bySlot]->m_nJob;
		ar >> g_Neuz.m_apPlayer[bySlot]->m_nLevel;
		ar >> dwJobLv;
		g_Neuz.m_apPlayer[bySlot]->m_Stat.Serialize( ar );
		g_Neuz.m_apPlayer[bySlot]->SetHairColor( g_Neuz.m_apPlayer[bySlot]->m_dwHairColor );
		ar >> g_Neuz.m_apPlayer[bySlot]->m_dwMode;

		int CountEquip = 0;
		ar >> CountEquip;
		
		PT_ITEM_SPEC pItemProp;
		DWORD dwItemId;
		int nParts = 0;
		for( int i = 0; i < CountEquip ; i++ )
		{
			ar >> dwItemId;
			pItemProp	= g_xSpecManager->GetSpecItem( dwItemId );
			if( pItemProp == NULL)
				FLERROR_LOG( PROGRAM_NAME, _T( "%d아이템 프로퍼티 없음" ), dwItemId );

			nParts = (int)pItemProp->dwParts;
			// 오른손 무기장착할 차례인데 이미 오른손에 뭔가 장착되어 있으면 
			if( nParts == PARTS_RWEAPON && g_Neuz.m_apPlayer[bySlot]->m_aEquipInfo[nParts].dwItemID != 0 )
			{
				// 오른손에걸 왼손으로 옮김. - 순서가 거꾸로 들어가있어서 이렇게 했다.
				g_Neuz.m_apPlayer[bySlot]->m_aEquipInfo[PARTS_LWEAPON].dwItemID = g_Neuz.m_apPlayer[bySlot]->m_aEquipInfo[PARTS_RWEAPON].dwItemID;
				// 새로 읽은 무기는 오른손에 쥠.
			}
			g_Neuz.m_apPlayer[bySlot]->m_aEquipInfo[nParts].dwItemID	= dwItemId;
		}
	}
	int CountMessenger = 0;
	ar >> CountMessenger;
	BYTE nSlot = 0;
	for( int i = 0 ; i < CountMessenger ; i++ )
	{		
		ar >> nSlot;
		g_Neuz.m_aRTMessenger[nSlot].Serialize( ar );

		//////////////////////////////////////////////////////////////////////////
		size_t nSize = 0;
		ar >> nSize;
		for( size_t j = 0; j < nSize; ++j )
		{
			EQUIP_INFO kEquipInfo;
			ar >> kEquipInfo;

			PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( kEquipInfo.dwItemID );
			if( pItemProp == NULL)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d아이템 프로퍼티 없음" ), kEquipInfo.dwItemID );
				continue;
			}

			int nParts = static_cast<int>( pItemProp->dwParts );
			if( nParts == PARTS_RWEAPON && g_Neuz.m_apPlayer[nSlot]->m_aEquipInfo[nParts].dwItemID != 0 )
			{
				g_Neuz.m_apPlayer[nSlot]->m_aEquipInfo[PARTS_LWEAPON].dwItemID = g_Neuz.m_apPlayer[nSlot]->m_aEquipInfo[PARTS_RWEAPON].dwItemID;
			}
			
			g_Neuz.m_apPlayer[nSlot]->m_aEquipInfo[nParts].dwItemID	= kEquipInfo.dwItemID;

			if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
			{
				g_Neuz.m_apPlayer[nSlot]->m_aEquipInfo[nParts].dwLooksChangeItemID	= kEquipInfo.dwLooksChangeItemID;
				g_Neuz.m_apPlayer[nSlot]->m_aEquipInfo[nParts].bIsCombined			= kEquipInfo.bIsCombined;
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

	// BEGIN110520
	// 총 캐릭터 수
	ar >> g_Neuz.m_byTotalCharNum;

	// 캐릭터 선택 페이지 수
	ar >> g_Neuz.m_byTotalCharSelectPage;

	// 현재 선택한 페이지
	ar >> g_Neuz.m_byCurrentCharSelectPage;
	// END110520


#ifdef PASSWORD_RESET_2ND
	for ( int i = 0; i < MAX_CHARACTER_SLOT; i++ )
		g_Neuz.m_nResetPassword[i] = 1;
	for ( int i = 0; i < g_Neuz.m_byTotalCharNum; i++ )
	{
		if ( i >= MAX_CHARACTER_SLOT ) break;
		ar >> g_Neuz.m_nResetPassword[i];
	}
#endif // PASSWORD_RESET_2ND

	CNetwork::GetInstance().OnEvent( LOGIN_PLAYER_LIST );

	CWndSelectChar* pWndSelectChar = (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
	if( pWndSelectChar )
	{
		pWndSelectChar->DeleteCharacter();
		pWndSelectChar->UpdateCharacter();
		if( g_Neuz.m_nCharacterBlock[ pWndSelectChar->m_nSelectCharacter ] == 0 )
		{
			for( int i = 0 ; i < MAX_CHARACTER_SLOT ; i++ )
			{
				if( g_Neuz.m_nCharacterBlock[i] == 1 )
				{
					pWndSelectChar->m_nSelectCharacter = i;
					break;
				}
			}
		}
		pWndSelectChar->SelectCharacter( pWndSelectChar->m_nSelectCharacter );
//		g_WndMng.OpenCustomBox( _T( "접속할수 없는 계정입니다"), new CWndAllCharBlockBox );
		// 모든 캐릭터가 블럭되어 있습니다.		
	}
}

void CDPLoginClient::OnAuthQuery( CAr & /*ar*/ )
{
	return;

// #ifdef _USE_XTRAP
// 	FLERROR_LOG( PROGRAM_NAME, _T( "====Received OnAuthQuery But Our system is using the XTRAP!!====" ) );
// 	::AfxMessageBox( "Conflict between NProtect and XTrap!!!" );
// 	::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
// 	return;
// #endif 
// 
// 	static	GG_AUTH_DATA	s_AuthData;
// 	ar.Read( (void*)&s_AuthData, sizeof(GG_AUTH_DATA) );
// 	GetNProtect()->Auth2( &s_AuthData );
// 
// #ifdef NO_GAMEGUARD
// 	// 클라는 NO_GAMEGUARD인데 서버는 게임가드로 돌아가는 중이다 
// 	// 게임가드가 설치되지 않았다는 메시지와 함께 OK를 누르면 클라는 종료된다.
// 
// 	SAFE_DELETE( g_WndMng.m_pWndConfirmVis );
// 	g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
// 	CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;
// 	if( pWnd )
// 	{
// 		pWnd->m_eSection = CWndConfirmVis::ETC_GAMEGUARD_ERR;		
// 		pWnd->Initialize( &g_WndMng );
// 	}
// 
// 	FLERROR_LOG( PROGRAM_NAME, _T( "====Received OnAuthQuery But not installed GAMEGUARD!!====" ) );
// #endif //NO_GAMEGUARD
}
#ifdef __gameguard
void CDPLoginClient::OnAuthQueryVer30( CAr & ar )
{
#ifdef _USE_XTRAP
	FLERROR_LOG( PROGRAM_NAME, _T( "====Received OnAuthQuery But Our system is using the XTRAP!!====" ) );
	::AfxMessageBox( "Conflict between NProtect and XTrap!!!" );
	::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
	return;
#endif 

	static CSAuth3Data s_CSAuth3Data;
	memset( s_CSAuth3Data.bPacket, 0, sizeof( s_CSAuth3Data.bPacket ) * sizeof( BYTE ) );
	ar >> s_CSAuth3Data.dwPacketSize;
	if( s_CSAuth3Data.dwPacketSize <= 4096 ) {
		ar.Read( (void*)s_CSAuth3Data.bPacket, s_CSAuth3Data.dwPacketSize );
		s_CSAuth3Data.dwServerNumber = 0;
		GetNProtect()->Auth3( s_CSAuth3Data.bPacket, s_CSAuth3Data.dwPacketSize, s_CSAuth3Data.dwServerNumber );
	}
	else {
		FLERROR_LOG( PROGRAM_NAME, _T( "[NPROTECT] invalid data : s_CSAuth3Data.dwPacketSize (%d)" ), s_CSAuth3Data.dwPacketSize );
	}

#ifdef NO_GAMEGUARD
	// 클라는 NO_GAMEGUARD인데 서버는 게임가드로 돌아가는 중이다 
	// 게임가드가 설치되지 않았다는 메시지와 함께 OK를 누르면 클라는 종료된다.

	SAFE_DELETE( g_WndMng.m_pWndConfirmVis );
	g_WndMng.m_pWndConfirmVis = new CWndConfirmVis;
	CWndConfirmVis* pWnd = g_WndMng.m_pWndConfirmVis;
	if( pWnd )
	{
		pWnd->m_eSection = CWndConfirmVis::ETC_GAMEGUARD_ERR;		
		pWnd->Initialize( &g_WndMng );
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "====Received OnAuthQuery But not installed GAMEGUARD!!====" ) );
#endif //NO_GAMEGUARD
}
#endif

void CDPLoginClient::OnCacheAddr( CAr & ar )
{	
	ar.ReadString( g_Neuz.m_lpCacheAddr, _countof( g_Neuz.m_lpCacheAddr ) );
	
	// 독일 테스트 서버의 현지 접속이면, 사설 주소로 대체
	CString strAddr	= g_xFlyffConfig->GetCertifierAddr();
	if( g_xFlyffConfig->GetMainLanguage() == LANG_GER && strAddr.Find( "192.168", 0 ) == 0 )
		FLStrcpy( g_Neuz.m_lpCacheAddr, _countof( g_Neuz.m_lpCacheAddr ), g_xFlyffConfig->GetCertifierAddr() );

	CNetwork::GetInstance().OnEvent( LOGIN_CACHE_ADDR );
}

void CDPLoginClient::OnLoginNumPad( CAr & ar )
{
	u_long idNumPad;
	ar >> idNumPad;
	m_idNumberPad = idNumPad;
}

void CDPLoginClient::OnLoginProtect( CAr & ar )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;
	BOOL bLogin;
	u_long idNumPad;
	ar >> bLogin >> idNumPad;

	if( g_xFlyffConfig->Is2ndPWDMode() == true )
	{
		CWndSelectChar* pWndSelectChar = (CWndSelectChar*)g_WndMng.GetWndBase( APP_SELECT_CHAR );
		if( pWndSelectChar == NULL )
			return;

		if( bLogin == FALSE )
		{
			g_WndMng.CloseMessageBox();
			g_WndMng.OpenMessageBox( prj.GetText( TID_2ND_PASSWORD_CONNECTING_FAIL_ERROR ) );	// 2차 비밀번호가 틀렸습니다.
			m_idNumberPad = idNumPad;
			pWndSelectChar->m_pWnd2ndPassword->ResetNumberpad( idNumPad );
			pWndSelectChar->m_pWnd2ndPassword->DeletePassword();
		}
	}
}

//--------------------------------------------------------//

#ifdef _USE_XTRAP

void	CDPLoginClient::OnXTrapCommandRequest( CAr & ar )
{
	FLERROR_LOG( PROGRAM_NAME, _T( "XTRAP Request ..." ) );

	char arrPacketDataIN[ XTRAP_CS4_COMSIZE_PACKDATA ] = { 0 };
	char arrPacketDataOUT[ XTRAP_CS4_COMSIZE_PACKDATA ] = { 0 };
	ar.Read( arrPacketDataIN, sizeof( arrPacketDataIN ) );

	const unsigned int uRet = XTrap_CS_Step2( arrPacketDataIN, arrPacketDataOUT
		, XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS );
	if( uRet != XTRAP_API_RETURN_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ERROR(%u) ]" ), uRet );
		return;
	}

	BEFORESENDSOLE( sendAr, PACKETTYPE_XTRAP_COMMAND_RESPONSE, DPID_UNKNOWN );
	sendAr.Write( arrPacketDataOUT, sizeof( arrPacketDataOUT ) );
	SEND( sendAr, this, DPID_SERVERPLAYER );

	FLERROR_LOG( PROGRAM_NAME, _T( "XTRAP Request OK" ) );
}

#endif //_USE_XTRAP

//--------------------------------------------------------//

u_long CDPLoginClient::GetNumberPad( void ) const
{
	return m_idNumberPad;
}

CDPLoginClient	g_dpLoginClient;