
#include "stdafx.h"

#include "dpcertified.h"
#include "wndbase.h"
#include "wndcloseexistingconnection.h"
#include "Network.h"

#include "WndHelperWebBox.h"

#include "../_Network/tools.h"
#include "../_Common/FLServerDescription.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL		g_bRecvSvrList	= FALSE;
CDPCertified	g_dpCertified;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDPCertified::CDPCertified()
{
	m_timer.Set( MIN( 1 ) );
	m_lError = 0;

	ON_MSG( PACKETTYPE_CERTIFY, OnCertifyAck );
	ON_MSG( PACKETTYPE_SERVER_LIST, OnServerList );
	ON_MSG( PACKETTYPE_ERROR, OnError );
	ON_MSG( PACKETTYPE_ERROR_STRING, OnErrorString );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, OnKeepAlive );
	ON_MSG( PACKETTYPE_ERROR_CODE_STRING, OnErrorCodeString );
}

CDPCertified::~CDPCertified()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////



LONG CDPCertified::GetNetError()
{
	return m_lError;
}

// 접속 끊김을 표시해야 하는가?
BOOL CDPCertified::CheckNofityDisconnected()
{
	//서버 리스트를 받았으면 서버 끊김을 표시하지 않는다.
	if( g_bRecvSvrList )
		return FALSE;

	// 에러로 끊기면 표시하지 않는다.
	if( m_lError )
		return FALSE;

	return TRUE;
}

void CDPCertified::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*dpId*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				m_fConn		= TRUE;
				g_bRecvSvrList	= FALSE;
			}
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				m_lError = lpDestroyPlayer->dwFlags;
			}
			CNetwork::GetInstance().OnEvent( CERT_DISCONNECT );

			m_fConn		= FALSE;

			if( CheckNofityDisconnected() )		// 접속 끊김을 표시해야 하는가?
			{
				g_WndMng.CloseMessageBox();
				g_WndMng.OpenMessageBox( prj.GetText(TID_DIAG_0023) );

				CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
				if( pWndLogin )
					pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
			}

			m_lError = 0;		// 에러코드 clear
			break;
	}
}

void CDPCertified::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );

	GETTYPE( ar );
	
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );

	if( pfn ) {
		( this->*( pfn ) )( ar, dpId );
	}
	else {
		/*
		switch( dw ) {
			default:
				FLTRACE_LOG( PROGRAM_NAME, _T( "Handler not found(%08x)" ), dw );
				break;
		}
		*/
	}
}

void CDPCertified::SendNewAccount( LPCSTR lpszAccount, LPCSTR lpszpw )
{
	BEFORESEND( ar, PACKETTYPE_NEW_ACCOUNT );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszpw );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCertified::SendCertify()
{
	g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 120 );

	BEFORESEND( ar, PACKETTYPE_CERTIFY );

	int nSiteValue	= 0;
	CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetApplet( APP_LOGIN );
	if( pWndLogin ) {
		nSiteValue	= pWndLogin->GetCurrentDomainChannel();
	}
	else {
		nSiteValue	= g_Neuz.m_certifySiteValue;
	}

	ar << nSiteValue;
	ar.WriteString( g_Neuz.m_szAccount );

	char	szEnc[ MAX_ENCRYPT_PWD ] = {0, };

	g_xRijndael->ResetChain();
	g_xRijndael->Encrypt( g_Neuz.m_szPassword, szEnc, MAX_ENCRYPT_PWD, CRijndael::CBC );

	ar.Write( szEnc, MAX_ENCRYPT_PWD );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN ) {
		ar.WriteString( g_Neuz.m_szSessionPwd );
	}

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCertified::SendCloseExistingConnection()
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	
	int nSiteValue	= 0;
	CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetApplet( APP_LOGIN );
	if( pWndLogin ) {
		nSiteValue = pWndLogin->GetCurrentDomainChannel();
	}

	ar << nSiteValue;
	ar.WriteString( g_Neuz.m_szAccount );
	ar.WriteString( g_Neuz.m_szPassword );

	SEND( ar, this, DPID_SERVERPLAYER );
}

void	CDPCertified::SendPacket( const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDPCertified::OnKeepAlive( CAr & ar, DPID )
{
//	BEFORESENDSOLE( ar2, PACKETTYPE_KEEP_ALIVE, DPID_UNKNOWN );
	BEFORESEND( ar2, PACKETTYPE_KEEP_ALIVE );
	SEND( ar2, this, DPID_SERVERPLAYER );
}


//	Handlers
void CDPCertified::OnCertifyAck( CAr & ar, DPID )
{
	ar >> g_Neuz.m_dwAuthKey;
	ar >> g_Neuz.m_cbAccountFlag;

	//ar.ReadString( g_Neuz.m_szGPotatoNo, _countof( g_Neuz.m_szGPotatoNo ) );
	char temp[256];
	ar.ReadString( temp, _countof( g_Neuz.m_szGPotatoNo ) );
	ar.ReadString( g_Neuz.m_szCheck, _countof( g_Neuz.m_szCheck ) );

	char account[256]="";
	char no[256] = "";
	sscanf(temp,"%s %s", account,no);

	if (no)
	{	strcpy(g_Neuz.m_szGPotatoNo ,account);
		strcpy(g_Neuz.m_szGPotatoNo2,no);
	}	else
	{	strcpy(g_Neuz.m_szGPotatoNo ,account);
	}


	char szBak[MAX_ACCOUNT]	= { 0,};
	ar.ReadString( szBak, _countof( szBak ) );
	if( lstrcmp( g_Neuz.m_szAccount, szBak ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CDPCertified.OnCertifyAck" ) );
		exit( 0 );
	}

	if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
	{
		long lTimeLeft;	
		ar >> lTimeLeft;		// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 받는다.
		g_Neuz.SetLeftTime( lTimeLeft );
	}
	//  [10/22/2014 ChangSeop.Kim]
	char url[128] = { 0 };
	ar.ReadString( url, _countof( url ) );
	g_Neuz.m_InGameShopURL = url;
	//  [10/22/2014 ChangSeop.Kim]
}

void CDPCertified::OnServerList( CAr & ar, DPID )
{
	g_bRecvSvrList	= TRUE;

	FLPacketServerDescriptionNoti kPacket;
	kPacket.Deserialize( ar );

	g_pServerDescription->Clear();

	for( std::vector< T_SERVER_DESC >::const_iterator kCItr = kPacket.vecServerDesc.begin(); kCItr != kPacket.vecServerDesc.end(); ++kCItr )
	{
		const T_SERVER_DESC kServerDesc = *kCItr;
		g_pServerDescription->Add( &kServerDesc );
	}

	CNetwork::GetInstance().OnEvent( CERT_SRVR_LIST );

	CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_LOGIN );
	if( pWndBase )
	{
		( (CWndLogin*)pWndBase )->Connected();
	}
}

void CDPCertified::OnErrorString( CAr & ar, DPID dpid )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;			// 타임 아웃 메세지 박스 표시를 막는다.

	char szError[256]	= { 0,};
	ar.ReadString( szError, _countof( szError ) );

	g_WndMng.CloseMessageBox();
	g_WndMng.OpenMessageBox( szError );
		
	CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
	if( pWndLogin )
		pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );

	// 에러 표시 후 소켓이 끊기게 패킷을 보낸다. shutdown이 되지 않는 이 아픔 
	// BEFORESENDSOLE( arWrite, PACKETTYPE_ERROR, DPID_UNKNOWN );
	BEFORESEND( arWrite, PACKETTYPE_ERROR);		// chipi_090219
	SEND( arWrite, this, DPID_SERVERPLAYER );
}

void CDPCertified::OnError( CAr & ar, DPID dpid )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;			// 타임 아웃 메세지 박스 표시를 막는다.
	int nText = 0;

	ar >> m_lError;

	CNetwork::GetInstance().OnEvent( CERT_ERROR );

	switch( m_lError )
	{
		case ERROR_DUPLICATE_ACCOUNT:			// 103L
			{
				g_WndMng.CloseMessageBox();
				g_WndMng.m_pWndCloseExistingConnection	= new CWndCloseExistingConnection;
				g_WndMng.m_pWndCloseExistingConnection->Initialize();
				return;
			}

		case ERROR_ILLEGAL_VER:
			nText = TID_DIAG_0035;
			break;
		case ERROR_ACCOUNT_EXISTS:				// 100L 이미 같은 이름의 계정이 있습니다.
			nText = TID_DIAG_0032;
			break;
		case ERROR_FLYFF_PASSWORD:				// 120L (비밀 번호가 틀립니다.)
			nText = TID_DIAG_0016;
			break;
		case ERROR_FLYFF_ACCOUNT:				// 121L (잘못된 계정입니다.)
			nText = TID_DIAG_0038;
			break;
		case ERROR_OVERFLOW:					// 108L 접속자가 너무 많습니다.
			nText = TID_DIAG_0041;
			break;
		case ERROR_EXTERNAL_ADDR:				// 109L 현재 서비스중이 아닙니다.
			nText = TID_DIAG_0053;
			break;
		case ERROR_BLOCKGOLD_ACCOUNT:			// 119L 블럭된 계정입니다.
			nText = TID_DIAG_0015;
			break;
		case ERROR_FLYFF_AUTH:					// 122L 실명인증후 게임접속이 가능합니다 www.flyff.com으로 접속해주십시오
			nText = TID_DIAG_0026;
			break;
		case ERROR_FLYFF_PERMIT:				// 123L 프리프는 12세 이상 이용가 이므로 게임접속을 할수 없습니다.
			nText = TID_DIAG_0050;
			break;		
		case ERROR_FLYFF_NEED_AGREEMENT:		// 124L 14세 미만 가입자 분들은 부모님 동의서를 보내주셔야 게임 접속이 가능합니다. www.flyff.com 으로 접속하셔서 확인해 주세요.
			nText = TID_DIAG_0001;
			break;
		case ERROR_FLYFF_NO_MEMBERSHIP:			// 125L	 웹에서 탈퇴한 계정입니다. www.flyff.com 으로 접속하셔서 확인해 주세요." ) );
			nText = TID_GAME_ACCOUNTWEBRETIRE;
			break;		
		case ERROR_BILLING_INFO_FAILED:			// 빌링 정보 없음
			nText = TID_DIAG_NOCHARGING;
			break;
		case ERROR_BILLING_DATABASE_ERROR:		// 빌링 DB 에러 
			nText = TID_DIAG_DBERROR1;
			break;
		case ERROR_BILLING_TIME_OVER:			// 빌링 사용시간 만료 
			nText = TID_DIAG_EXPIRY;
			break;		
		case ERROR_BILLING_OTHER_ERROR:			// 빌링 기타 다른 에러 
			nText = TID_DIAG_DBERROR2;
			break;
		case ERROR_BILLING_DISCONNECTED:
			nText = TID_DIAG_DBERROR2;
			break;
		case ERROR_TOO_LATE_PLAY:				// 131L
			nText = TID_GAME_LIMITCONNECTION;
			break;
		case ERROR_IP_CUT:						// 132L
			nText = TID_GAME_OTHERCOUNTRY; 
			break;
		case ERROR_FLYFF_DB_JOB_ING:			// 133L
			nText = TID_DB_INSPECTION;
			break;
		case ERROR_15SEC_PREVENT:				// 134L
			nText = TID_15SEC_PREVENT;
			break;
		case ERROR_15MIN_PREVENT:				// 135L 
			nText = TID_15MIN_PREVENT;
			break;
		case ERROR_CERT_GENERAL:
			nText = TID_ERROR_CERT_GENERAL;
			break;
		case ERROR_FLYFF_EXPIRED_SESSION_PASSWORD:
			nText	= TID_ERROR_EXPIRED_SESSION_PASSWORD;
			break;

		//gmpbigsun(2010_0617) : added error msg
		case ERROR_JAPAN_AUTH_CONVERTED_USER:
			nText = TID_ERROR_JAPAN_AUTH_CONVERTED_USER;
			break;
		case ERROR_JAPAN_AUTH_NEED_AGREEMENT:
			//웹윈도우 띄움
			{
				CWndHelperWebBox* pWndWebBox = (CWndHelperWebBox*)g_WndMng.CreateApplet( APP_WEBBOX2 );
				FLASSERT( pWndWebBox );
				if( pWndWebBox )
					pWndWebBox->SetMode( CWndHelperWebBox::WMODE_CERTIFICATION );

				g_WndMng.CloseMessageBox();

				CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
				if( pWndLogin )
					pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );

				// 에러 표시 후 소켓이 끊기게 패킷을 보낸다. shutdown이 되지 않는 이 아픔 
				//BEFORESENDSOLE( ar, PACKETTYPE_ERROR, DPID_UNKNOWN );
				BEFORESEND( ar, PACKETTYPE_ERROR );		// chipi_090219 
				SEND( ar, this, DPID_SERVERPLAYER );

				//메세지 없음.
				nText = 0;

			}
		  		  
			break;
		case ERROR_JAPAN_AUTH_ID_ERROR:
			nText = TID_ERROR_JAPAN_AUTH_ID_ERROR;
			break;
		case ERROR_JAPAN_AUTH_PASSWORD_ERROR:
			nText = TID_ERROR_JAPAN_AUTH_PASSWORD_ERROR;
			break;
		case ERROR_JAPAN_AUTH_BLOCK:
			nText = TID_ERROR_JAPAN_AUTH_BLOCK;
			break;
		case ERROR_JAPAN_AUTH_AGREE:
			nText = TID_ERROR_JAPAN_AUTH_AGREE;
			break;
		case ERROR_JAPAN_AUTH_PARAMETER_ERROR:
			nText = TID_ERROR_JAPAN_AUTH_PARAMETER_ERROR;
			break;
		case ERROR_JAPAN_AUTH_REJECT:
			nText = TID_ERROR_JAPAN_AUTH_REJECT;
			break;
		case ERROR_JAPAN_AUTH_HANGAME_ERROR:
			nText = TID_ERROR_JAPAN_AUTH_HANGAME_ERROR;
			break;
		case ERROR_JAPAN_AUTH_FAIL:
			nText = TID_ERROR_JAPAN_AUTH_FAIL;
			break;

			// 인도네시아 인증 에러
		case ERROR_INDONESIA_AUTH_DUPLICATED_LOGIN:
			nText = TID_INDONESIA_LOGINERROR01;
			break;
		case ERROR_INDONESIA_AUTH_ID_PASSWORD_WRONG:
			nText = TID_INDONESIA_IDPASSERROR01;
			break;
		case ERROR_SHUTDOWN_RULE:
			nText = TID_MMI_SHUTDOWNNOTICE03;
			break;
		case ERROR_OK:
			{
				BEFORESEND( ar, PACKETTYPE_ERROR );		// chipi_090219 
				SEND( ar, this, DPID_SERVERPLAYER );
			}
			break;
	}

	if( nText > 0 )
	{
		g_WndMng.CloseMessageBox();
		g_WndMng.OpenMessageBox( _T( prj.GetText(nText) ) );
		
		CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( FALSE );
			
			//패스워드 지움
			CWndEdit* pPassword = (CWndEdit*) pWndLogin->GetDlgItem( WIDC_PASSWORD );
			if( pPassword )
			{
				pPassword->SetString("");
			}
		}

		BEFORESEND( ar, PACKETTYPE_ERROR );		// chipi_090219 
		SEND( ar, this, DPID_SERVERPLAYER );
	}
}

void	CDPCertified::OnErrorCodeString( CAr & ar, DPID dpid )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;			// 타임 아웃 메세지 박스 표시를 막는다.

	LONG errorCode = 0;
	ar >> errorCode;
	char szError[256] = { 0, };
	ar.ReadString( szError, _countof( szError ) );

	g_WndMng.CloseMessageBox();
	if( g_WndMng.OpenMessageBox( szError ) ) {
		g_WndMng.m_pWndMessageBox->setCode( errorCode );
	}

	CWndLogin* pWndLogin = (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
	if( pWndLogin != NULL ) {
		pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
	}

	BEFORESEND( arWrite, PACKETTYPE_ERROR );
	SEND( arWrite, this, DPID_SERVERPLAYER );
}
