//+	szPath	        "C:\Program Files\Aeonsoft\Flyff Test/Client/notice.inc.gz"
//+	szServerPath	"flyff/NeuroSpace/TEST_RESCLIENT_KOR/Client/notice.inc.gz"


	m_pThread = AfxBeginThread( _DownloadThread, this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED );

CDPPatchClient 를 제거 
g_PatchManager.fv 를 내부 멤버로 변경시킨다.
ui 변경을 알아서 하게 


			int nIndex;
			char* p = &szBuffer[3];
			for( int i=3; *p != ' '; ++i, ++p )
			{
				if( *p == '\\' )
					nIndex = i;
			}
			i = i - nIndex - 1;
			if( i > 0 )
			{
				char szDir[256] = {0, };
				memcpy( szDir , szBuffer + nIndex + 1, i);
				TRACE( "HEAD: %s\n", szDir );
			}



void CBetaPatchClientDlg::MakePatchList()
{
	char szPath[MAX_PATH];
	sprintf( szPath, "%s\\%s", g_PatchManager.m_szCurrentDirectory, "list.txt" ); 

	FILE* fp = fopen( szPath, "rt" );
	if( !fp )
		return;

	char szTokens[4][256];
	char szBuffer[256];
	char szRoot[256] = {0, };
	char szDir[256], szFile[256];
	CTime tmFile;
	char flag;
	int nYY, nMM, nDD;
	int nHour, nMin;

	while( 1 )
	{
		if( fgets( szBuffer, 127, fp ) == NULL)
			break;

		if( strlen( szBuffer ) < 6 )
			continue;
		
		if( szBuffer[2] == ':' )
		{
//			TRACE( "%s", szBuffer );
			sscanf( szBuffer, "%s %s", szTokens[0], szTokens[1] );
			if( szRoot[0] == 0 )
			{
				strcpy( szRoot, szTokens[0] );
				szDir[0] = 0;
			}
			else
			{
				memset( szDir, 0x00, sizeof( szDir ) );
				memcpy( szDir, szTokens[0] + strlen( szRoot ),  strlen( szTokens[0] ) - strlen( szRoot ) );

				EnqueueFILE( szDir, true, 0, NULL );
			}
//			TRACE( "HEAD:%s", szDir );
		}
		else if( szBuffer[4] == '-' )
		{
//			TRACE( "%s", szBuffer );
			char* p  =  strstr( szBuffer, "<DIR>" );

			sscanf( szBuffer, "%s %s %s %s", szTokens[0], szTokens[1], szTokens[2], szTokens[3] );
			if( strcmp( szTokens[2], "<DIR>") == 0  )
			{
				if( szTokens[3][0] != '.' )
					TRACE( "<DIR>:%s\n", szTokens[3] );
			}
			else
			{
				 // TRACE("ENTRY:%s %s %s %s\n", szTokens[0], szTokens[1], szTokens[2], szTokens[3] );
				 //  TRACE("ENTRY:%s\\%s\n", szDir, szTokens[3] );
				sscanf( szTokens[0], "%d-%d-%d", &nYY, &nMM, &nDD );
				sscanf( szTokens[1], "%02d:%02d%c", &nHour, &nMin, &flag );
				if( nHour == 12 )
					nHour = 0;
				if( flag == 'p' )
					nHour += 12;

				sprintf( szFile, "%s\\%s", szDir, szTokens[3] );
				tmFile = CTime( nYY, nMM, nDD, nHour, nMin, 0 );
				EnqueueFILE( szFile, false, atoi(szTokens[2]), &tmFile  );
			}
		}
	}
	fclose( fp );
}



			//Copy the resulting FILETIME structure to a LARGE_INTEGER structure. 
			//Use normal 64-bit arithmetic on the LARGE_INTEGER value. 
			__int64 remote, local;
			//memcpy( &remote, &info.ft, sizeof( remote ));
			//memcpy( &local, &attribute.ftLastWriteTime, sizeof( local ));

			remote = ((__int64)info.ft.dwHighDateTime << 32) + info.ft.dwLowDateTime;
			local  = ((__int64)attribute.ftLastWriteTime.dwHighDateTime << 32) + attribute.ftLastWriteTime.dwLowDateTime;
			__int64 a = remote - local;
			if( (remote - local) < 180 )
				return true;


AfxMessageBox(IDS_REINSTALL);
AfxMessageBox(IDS_RUNNING_NEUZ);
AfxMessageBox(IDS_RUNNING_PATCH);
AfxMessageBox(IDS_OPTION);
AfxMessageBox(IDS_CLOSE);
AfxMessageBox(IDS_RESOLUTION);
AfxMessageBox(IDS_VIEW);
AfxMessageBox(IDS_TEXTURE);
AfxMessageBox(IDS_SHADOW);
AfxMessageBox(IDS_OBJECTDETAIL);
AfxMessageBox(IDS_FAR);
AfxMessageBox(IDS_MEDIUM);
AfxMessageBox(IDS_NEAR);
AfxMessageBox(IDS_HIGH);
AfxMessageBox(IDS_LOW);
AfxMessageBox(IDS_DIRECTX_INSTALL);
AfxMessageBox(IDS_UPDATE_PATCH);
AfxMessageBox(IDS_RETRY);

LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while( *p1 != NULL )
	{
		LPCTSTR p = p2;
		while( *p != NULL )
		{
			if( *p1 == *p++ )
				return p1+1;
		}
		p1++;
	}
	return NULL;
}


	TCHAR szTokens[] = "-/";
	LPCTSTR lpszToken = FindOneOf(m_lpCmdLine, szTokens);
	while( lpszToken != NULL )
	{
		if( lstrcmpi(lpszToken, _T("domain"))==0 )
		{
			break;
		}
		if( lstrcmpi(lpszToken, _T("account"))==0 )
		{
			break;
		}
		if( lstrcmpi(lpszToken, _T("password"))==0 )
		{
			break;
		}
		lpszToken = FindOneOf( lpszToken, szTokens );
	}



enum OPTION_STRING
{
	OPTION_DOMAIN,
	OPTION_ACCOUNT,
	OPTION_PASSWORD,
};


LPCTSTR CBetaPatchClientApp::GetOptionString( OPTION_STRING type )
{
	switch( type )
	{
	case OPTION_DOMAIN:		return m_strDomain;		break;
	case OPTION_ACCOUNT:	return m_strAccount;	break;
	case OPTION_PASSWORD:	return m_strPassword;	break;
	}

	return "";
}

#ifdef	__HANGAME0307     // 한게임 대응 버젼 
	TCHAR* szText   = HOME_LINK;
	TCHAR* szURL    = HOME_LINK;
	TCHAR* szWebURL = WEB_FIRST_PAGE;

	LPCTSTR lpszDomain = ((CBetaPatchClientApp *)AfxGetApp())->GetOptionString( OPTION_DOMAIN );
	if( lstrcmpi(lpszDomain, "hangame.co.jp") ==0 )
	{
		szText   = "http://www.hangame.co.jp";
		szURL    = "http://www.hangame.co.jp/game.asp?fo";
		szWebURL = "http://www.flyff.jp/info/news/info_list.asp?domain=hangame.co.jp";
	}
#else					// 일반버젼 
	TCHAR* szText   = HOME_LINK;
	TCHAR* szURL    = HOME_LINK;
	TCHAR* szWebURL = WEB_FIRST_PAGE;
#endif	// __HANGAME0307
