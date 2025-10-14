#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndQuest.h"

#include "dpclient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




BOOL CWndRemoveQuest::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	Create( "", MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );

	m_wndText.SetString( prj.GetText(TID_GAME_QUEST_DELCONFIRM) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	return CWndMessageBox::Initialize( pWndParent, dwWndId );
}
BOOL CWndRemoveQuest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:       
			{
				LPQUEST lpQuest = g_pPlayer->FindQuest( m_nRemoveQuestId );
#ifdef __V050322_CACHEQUEST
				if( lpQuest )
					g_DPlay.SendRemoveQuest( m_nRemoveQuestId );
#else // __V050322_CACHEQUEST
//{{AFX
				if( lpQuest )
					g_DPlay.SendQuestCancel( m_nRemoveQuestId );
//}}AFX
#endif // __V050322_CACHEQUEST
				Destroy(); 
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


CWndQuest::CWndQuest() 
{ 
	m_idSelQuest = -1;
}     
CWndQuest::~CWndQuest() 
{ 
} 
void CWndQuest::OnDraw( C2DRender* p2DRender ) 
{ 
	//CWndText* pText4 = (CWndText*)GetDlgItem( WIDC_TEXT4 );
	//CRect rect = pText4->GetWindowRect( TRUE );
	//p2DRender->RenderTexture( rect.TopLeft(), &m_texChar, 255, 0.3, 0.3 );
} 
#ifdef __V050322_CACHEQUEST
void CWndQuest::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		{
			int nNum;
			WORD nQuest;
			ar >> m_idSelQuest;
			ar >> nNum;
			for( int i = 0; i < nNum; i++ )
			{
				ar >> nQuest;
				m_aOpenTree.Add( nQuest );
			}
			TreeOpen();
			UpdateText();
		}
	}
	else
	{
		dwVersion = 1;
		ar << m_idSelQuest;
		ar << m_aOpenTree.GetSize();
		for( int i = 0; i < m_aOpenTree.GetSize(); i++ )
			ar << m_aOpenTree.GetAt( i );
	}
}
void CWndQuest::TreeOpen() 
{
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	LPTREEELEM lpRoot = pTreeCtrl->GetRootElem();
	int nPos = 0;
	LPTREEELEM lpElem = NULL;
	do {
		lpElem = pTreeCtrl->GetNextElem( lpRoot, nPos );
		if( lpElem )
		{
			for( int i = 0; i < m_aOpenTree.GetSize(); i++ )
			{
				if( m_aOpenTree.GetAt( i ) == lpElem->m_dwData )
				{
					lpElem->m_bOpen = TRUE;
					break;
				}
			}
		}
	} while( lpElem );
	pTreeCtrl->SetCurSel( m_idSelQuest );
}
void CWndQuest::Update( int nNewQuestId ) 
{
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );

	LPQUEST lpQuest = NULL;

	CString strQuest;

	//pTreeCtrl->GetText( pTreeCtrl->GetCurSel(), strQuest );

	//pTreeCtrl->m_dwColor = 0xff000000;
	pTreeCtrl->DeleteAllItems();

	QuestProp * pQuestProp;
	//CPtrArray* paQuestArray;
	
	CUIntArray* paHeadQuest;
	CPtrArray aOrderQuest;
	// 해드 퀘스트를 만든다. 
	int i;
	for( i = 0; i < g_pPlayer->m_nCompleteQuestSize; i++ )
	{
		WORD wQuest = g_pPlayer->m_aCompleteQuest[ i ];
		if( wQuest != 0xffff )
		{
			pQuestProp = prj.m_aPropQuest.GetAt( wQuest );
			if( pQuestProp )
			{
				for( int j = 0; j < aOrderQuest.GetSize(); j++ )
				{
					paHeadQuest = (CUIntArray*) aOrderQuest.GetAt( j );
					// 같은게 발견되면 뒤를 이어 추가 : 추가된건 일반 퀘스트 
					if( paHeadQuest->GetAt( 0 ) == pQuestProp->m_nHeadQuest ) 
					{
						paHeadQuest->Add( wQuest );
						break;
					}
				}
				// 발견 못했으면 해드 퀘스트를 처음 추가 
				if( j == aOrderQuest.GetSize() )
				{
					paHeadQuest = new CUIntArray;
					paHeadQuest->Add( pQuestProp->m_nHeadQuest );
					paHeadQuest->Add( wQuest );
					aOrderQuest.Add( paHeadQuest );
				}
			}
			else
			{
				CString string;
				string.Format( "CWndQuest::Update 1 : Quest %d의 pQuestProp NULL이다.", wQuest );
				ADDERRORMSG( string );
			}
		}
	}
	for( i = 0; i < g_pPlayer->m_nQuestSize; i++ )
	{
		lpQuest = &g_pPlayer->m_aQuest[ i ];
		if( lpQuest->m_wId != 0xffff )
		{
			pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
			if( pQuestProp )
			{
				for( int j = 0; j < aOrderQuest.GetSize(); j++ )
				{
					paHeadQuest = (CUIntArray*) aOrderQuest.GetAt( j );
					// 같은게 발견되면 뒤를 이어 추가 : 추가된건 일반 퀘스트 
					if( paHeadQuest->GetAt( 0 ) == pQuestProp->m_nHeadQuest ) 
					{
						paHeadQuest->Add( lpQuest->m_wId );
						break;
					}
				}
				// 발견 못했으면 해드 퀘스트를 처음 추가 
				if( j == aOrderQuest.GetSize() )
				{
					paHeadQuest = new CUIntArray;
					paHeadQuest->Add( pQuestProp->m_nHeadQuest );
					paHeadQuest->Add( lpQuest->m_wId );
					aOrderQuest.Add( paHeadQuest );
				}
			}
			else
			{
				CString string;
				string.Format( "CWndQuest::Update 2 : Quest %d의 pQuestProp NULL이다.", lpQuest->m_wId );
				ADDERRORMSG( string );
			}
		}
	}

	// 퀘스트를 list에 넣는다.
	QUEST questTemp;
	BOOL bCompleteQuest;
	for( i = 0; i < aOrderQuest.GetSize(); i++ )
	{
		paHeadQuest = (CUIntArray*)aOrderQuest.GetAt( i );
		LPTREEELEM lpTreeElem = NULL;
		for( int j = 0; j < paHeadQuest->GetSize(); j++ )
		{
			int nQuestId = paHeadQuest->GetAt( j );
			QuestProp * pQuestProp = prj.m_aPropQuest.GetAt( nQuestId );
			if( pQuestProp )
			{
				lpQuest = g_pPlayer->GetQuest( nQuestId );
				bCompleteQuest = FALSE;
				if( lpQuest == NULL )
				{
					if( g_pPlayer->IsCompleteQuest( nQuestId ) )
					{
						g_pPlayer->MakeCompleteQuest( nQuestId, &questTemp );
						lpQuest = &questTemp;
						bCompleteQuest = TRUE;
					}
				}
				// 실제 퀘스트 
				CString string;
				if( lpQuest )
				{
					CString strState;
					if( g_Option.m_bOperator || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
						strState.Format( "(%d,%d)", lpQuest->m_nState, lpQuest->m_wId );
					string.Format( "%s%s", pQuestProp->m_szTitle, strState );
					if( bCompleteQuest )
						pTreeCtrl->InsertItem( lpTreeElem, string, nQuestId )->m_dwColor = 0xffc0c0c0;
					else
						pTreeCtrl->InsertItem( lpTreeElem, string, nQuestId );

				}
				// 해드 퀘스트 
				else
				{
					string.Format( "%s", pQuestProp->m_szTitle );
					lpTreeElem = pTreeCtrl->InsertItem( NULL, string, nQuestId );
					// 새 퀘스트의 헤드랑 나랑 같으면 트리 자동 오픈 
					if( nNewQuestId != -1 && prj.m_aPropQuest.GetAt( nNewQuestId )->m_nHeadQuest == nQuestId )
					{
						lpTreeElem->m_bOpen = TRUE;
						m_aOpenTree.Add( nQuestId );
					}
				}
			}
		}
		delete paHeadQuest;
	}
	if( nNewQuestId != -1 )
		m_idSelQuest = nNewQuestId;
	TreeOpen();
	UpdateText();
}
#else
//{{AFX
void CWndQuest::Update() 
{
	CWndListBox* pListBox = &m_listGeneral;//(CWndListBox*)GetDlgItem( WIDC_LIST1 );

	LPQUEST lpQuest = NULL;

	CString strGeneral;
	CString strRequest;
	CString strScenario;

	m_listGeneral.GetText( m_listGeneral.GetCurSel(), strGeneral );
	m_listRequest.GetText( m_listRequest.GetCurSel(), strGeneral );
	m_listScenario.GetText( m_listScenario.GetCurSel(), strGeneral );

	//for( int i = 0; i < 3; i ++ )
	{
		/*
		switch( i )
		{
		case 0: pListBox = &m_listGeneral; break;
		case 1: pListBox = &m_listRequest; break;
		case 2: pListBox = &m_listScenario; break;
		}
		*/
		m_listGeneral.m_dwColor = 0xff000000;
		m_listGeneral.ResetContent();
		m_listRequest.m_dwColor = 0xff000000;
		m_listRequest.ResetContent();
		m_listScenario.m_dwColor = 0xff000000;
		m_listScenario.ResetContent();

		QuestProp * pQuestProp;
		int nHeadQuest;
		CPtrArray* paQuestArray;
		
		CMapPtrToPtr map;
		for( int i = 0; i < g_pPlayer->m_nQuestSize; i++ )
		{
			lpQuest = &g_pPlayer->m_aQuest[ i ];
			if( lpQuest->m_wId != 0xffff && lpQuest->m_wId >= 1000 && lpQuest->m_wId < 2000 )
			{
				pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( map.Lookup( (void*)pQuestProp->m_nHeadQuest, (void*&)paQuestArray ) == FALSE )
				{
					paQuestArray = new CPtrArray;
					map.SetAt( (void*)pQuestProp->m_nHeadQuest, paQuestArray ); 
				}
				paQuestArray->Add( lpQuest );
			}
		}
		POSITION pos = map.GetStartPosition();
		CPtrArray aOrderQuest;
		while( pos )
		{
			map.GetNextAssoc( pos, (void*&)nHeadQuest, (void*&)paQuestArray );
			aOrderQuest.Add( paQuestArray );
		}
		for( i = aOrderQuest.GetSize() - 1; i >= 0; i-- )
		{
			paQuestArray = (CPtrArray*)aOrderQuest.GetAt( i );
			for( int j = 0; j < paQuestArray->GetSize(); j++ )
			{
				lpQuest = (LPQUEST)paQuestArray->GetAt( j );
				QuestProp * pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( pQuestProp )
				{
					CString string, strState;
					if( g_Option.m_bOperator )
						strState.Format( "(%d)", lpQuest->m_nState );
					if( j == 0 )
						string.Format( "%s%s", pQuestProp->m_szTitle, strState);
					else
						string.Format( "   - %s%s", pQuestProp->m_szTitle, strState );

					int nIndex = m_listScenario.AddString( string );
					m_listScenario.SetItemData( nIndex, lpQuest->m_wId );
				}
			}
			delete paQuestArray;
		}

		//pText1->SetString( "" );
		for( i = 0; i < g_pPlayer->m_nQuestSize; i++ )
		{
			lpQuest = &g_pPlayer->m_aQuest[ i ];
			if( lpQuest->m_wId != 0xffff && lpQuest->m_wId != 0xff )
			{
				//int nQuestState = g_pPlayer->m_aQuest[ i ].m_nState;
				//QuestState* pQuestStep = &g_pPlayer->m_aQuest[ i ].m_wId )->m_questStep[ nQuestState ];
				QuestProp * pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
				if( pQuestProp )
				{
					CString string;
					if( g_Option.m_bOperator )
						string.Format( "%s(%d)", pQuestProp->m_szTitle, lpQuest->m_nState );
					else
						string = pQuestProp->m_szTitle;

					if( lpQuest->m_wId < 1000 )
					{
						int nIndex = m_listGeneral.AddString( string );
						m_listGeneral.SetItemData( nIndex, lpQuest->m_wId );
					}
					else
					if( lpQuest->m_wId < 2000 )
					{
					}
					else
					{
						int nIndex = m_listRequest.AddString( string );
						m_listRequest.SetItemData( nIndex, lpQuest->m_wId );
					}
				}
			}
		}
	}
	int nSelGeneral  = m_listGeneral.FindString ( 0, strGeneral );
	int nSelRequest  = m_listRequest.FindString ( 0, strRequest );
	int nSelScenario = m_listScenario.FindString( 0, strScenario );

	if( nSelGeneral  != -1 ) m_listGeneral.SetCurSel( nSelGeneral );
	if( nSelRequest  != -1 ) m_listGeneral.SetCurSel( nSelRequest );
	if( nSelScenario != -1 ) m_listGeneral.SetCurSel( nSelScenario );

	UpdateText();
}
//}}AFX
#endif // __V050322_CACHEQUEST
CString CWndQuest::MakeQuestString( CString& string, BOOL bCond ) 
{
	CString strResult;
	if( bCond )
		strResult = "#s#cffa0a0a0" + string + "#ns#nc\n";
	else
		strResult = string + "\n";
	return strResult;
}
void CWndQuest::UpdateText() 
{
#ifdef __V050322_CACHEQUEST
	QuestProp* pQuestProp; 
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	CWndButton* pWndRemove = (CWndButton*)GetDlgItem( WIDC_REMOVE );
	CWndText* pTextDesc   = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndText* pTextCond   = (CWndText*)GetDlgItem( WIDC_TEXT2 );
	CWndText* pTextReward  = (CWndText*)GetDlgItem( WIDC_TEXT3 );
	LPQUEST lpQuest = NULL;
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	int nQuest = -1;
	if( lpTreeElem && lpTreeElem->m_lpParent )
	{
		nQuest = lpTreeElem->m_dwData ;
		lpQuest = g_pPlayer->FindQuest( nQuest );
	}
	BOOL bComplete = nQuest != -1 && g_pPlayer->IsCompleteQuest( nQuest ); 

	if( lpQuest || bComplete ) 
	{
#ifdef __V050721_BUGFIX_QUESTTIMER
		CWndDialog* pWndDialog = (CWndDialog*)g_WndMng.GetWndBase( APP_DIALOG );
		pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		// 영자는 언제나 퀘스트를 지울 수 있다.
		if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) && g_Option.m_bOperator )
			pWndRemove->EnableWindow( TRUE );
		else
		if( bComplete || lpQuest->m_nState == QS_END || pWndDialog )
		{
			pWndRemove->EnableWindow( FALSE );
		}
		else
		{
			if( pQuestProp->m_bNoRemove )
				pWndRemove->EnableWindow( FALSE );
			else
				pWndRemove->EnableWindow( TRUE );
		}
#else
//{{AFX
		pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		// 영자는 언제나 퀘스트를 지울 수 있다.
		if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) && g_Option.m_bOperator )
			pWndRemove->EnableWindow( TRUE );
		else
		if( bComplete || lpQuest->m_nState == QS_END )
		{
			pWndRemove->EnableWindow( FALSE );
		}
		else
		{
			if( pQuestProp->m_bNoRemove )
				pWndRemove->EnableWindow( FALSE );
			else
			{
#ifdef __V050322_CACHEQUEST
				pWndRemove->EnableWindow( TRUE );
#else
				if( lpQuest->m_wId >= 1000 && pQuestProp->m_wId < 2000 ) // 시나리오는 못지운다.
					pWndRemove->EnableWindow( FALSE );
				else

					pWndRemove->EnableWindow( TRUE );
#endif
			}
		}
//}}AFX
#endif
		// 디스크립션만 출력
		if( bComplete )
		{
			for( int i = QS_END; i >= 0; i-- )
			{
				QuestState* pQuestState = pQuestProp->m_questState[ i ];
				if( pQuestState )
				{
					pTextDesc->SetString( pQuestState->m_szDesc, 0xff000000 );
					break;
				}
			}
		}
		else
		{
			QuestState* pQuestState = pQuestProp->m_questState[ lpQuest->m_nState ];
			pTextDesc->SetString( pQuestState->m_szDesc, 0xff000000 );
		}
		//////////////////////////////////////////////////////////////////////////////////
		// 퀘스트 종료 조건 
		//////////////////////////////////////////////////////////////////////////////////
		CString strTemp;
		CString strCond;
		CString strComplete;

		if( bComplete )
			pTextCond->SetString( GETTEXT( TID_QUEST_COMPLETED  ), 0xff404040 );
		else
		{
			for( int i = 0; i < 14; i++ )
			{
				if( pQuestProp->m_questState[ i ] == NULL )
					break;
			}
			if( lpQuest->m_nState == ( i - 1 ) || lpQuest->m_nState == 14 )
			{
				int nNum;
				if( pQuestProp->m_nEndCondLimitTime )
				{
					if( lpQuest->m_wTime )
					{
#ifdef __V050721_BUGFIX_QUESTTIMER
						strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), lpQuest->m_wTime & 0x7fff );
#else
//{{AFX
						strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), lpQuest->m_wTime );
//}}AFX
#endif
						strCond += strTemp + "\n";
					}
					else
					{
						strTemp.Format( GETTEXT( TID_QUEST_LIMIT_TIMEOUT ) );
						strCond += "#cffff0000" + strTemp + "#ns#nc\n";
					}
				}
				for( int i = 0 ; i < 2; i++ )
				{
					if( pQuestProp->m_nEndCondKillNPCIdx[ i ] )
					{
						MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondKillNPCIdx[ i ] );
						nNum = lpQuest->m_nKillNPCNum[ i ];
						if( nNum > pQuestProp->m_nEndCondKillNPCNum[ i ] )
							nNum = pQuestProp->m_nEndCondKillNPCNum[ i ];
						strTemp.Format( GETTEXT( TID_QUEST_KILL_NPC ), pMoverProp->szName, lpQuest->m_nKillNPCNum[ i ], pQuestProp->m_nEndCondKillNPCNum[ i ] );

						if( nNum == pQuestProp->m_nEndCondKillNPCNum[ i ] )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else 
							strCond += strTemp + "\n";
					}
				}
				for( i = 0; i < pQuestProp->m_nEndCondItemNum; i++ )
				{
					QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[ i ];
#ifdef __V050524_QUEST
					if( pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == g_pPlayer->GetSex() )
#endif
					{
						if( pEndCondItem->m_nType == 0 )
						{
							if( pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == g_pPlayer->GetJob() )
							{
#ifdef __V050524_QUEST
								if( pEndCondItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
									strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

									if( nNum >= pEndCondItem->m_nItemNum )
										strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
									else 
										strCond += strTemp + "\n";
								}
#else
//{{AFX
								for( int j = 0; j < 4; j++ )
								{
									if( pEndCondItem->m_anItemIdx[ j ] )
									{
										ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_anItemIdx[ j ] );
										nNum = g_pPlayer->GetItemNum( pEndCondItem->m_anItemIdx[ j ] );
										strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_anItemNum[ j ] );

										if( nNum >= pEndCondItem->m_anItemNum[ j ] )
											strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
										else 
											strCond += strTemp + "\n";
									}
								}
//}}AFX
#endif
							}
						}
						else
						if( pEndCondItem->m_nType == 1 )
						{
							if( pEndCondItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndCondItem->m_nJobOrItem ) )
							{
#ifdef __V050524_QUEST
								if( pEndCondItem->m_nItemIdx )
								{
									ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_nItemIdx );
									nNum = g_pPlayer->GetItemNum( pEndCondItem->m_nItemIdx );
									strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_nItemNum );

									if( nNum >= pEndCondItem->m_nItemNum )
										strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
									else 
										strCond += strTemp + "\n";
								}
#else
//{{AFX
								for( int j = 0; j < 4; j++ )
								{
									if( pEndCondItem->m_anItemIdx[ j ] )
									{
										ItemProp* pItemProp = prj.GetItemProp( pEndCondItem->m_anItemIdx[ j ] );
										nNum = g_pPlayer->GetItemNum( pEndCondItem->m_anItemIdx[ j ] );
										strTemp.Format( GETTEXT( TID_QUEST_ITEM ), pItemProp->szName, nNum, pEndCondItem->m_anItemNum[ j ] );

										if( nNum >= pEndCondItem->m_anItemNum[ j ] )
											strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
										else 
											strCond += strTemp + "\n";
									}
								}
//}}AFX
#endif
							}
						}
					}
				}
				if( pQuestProp->m_dwEndCondPatrolWorld )
				{
					strTemp.Format( GETTEXT( TID_QUEST_PATROL ), pQuestProp->m_szPatrolZoneName );
					if( lpQuest->m_bPatrol )
						strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
					else
						strCond += strTemp + "\n";
				}
				if( pQuestProp->m_nEndCondDisguiseMoverIndex )
				{
					if( pQuestProp->m_nEndCondDisguiseMoverIndex == -1 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_NODISQUISE ) );
						if( g_pPlayer->IsDisguise() == FALSE )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					{
						MoverProp* pMoverProp = prj.GetMoverProp( pQuestProp->m_nEndCondDisguiseMoverIndex );
						strTemp.Format( GETTEXT( TID_QUEST_DISQUISE ), pMoverProp->szName );
						if( g_pPlayer->IsDisguise() )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
				}
				if( pQuestProp->m_nEndCondSkillIdx ) 
				{
					ItemProp* pSkillProp = prj.GetSkillProp( pQuestProp->m_nEndCondSkillIdx );
					LPSKILL lpSkill = g_pPlayer->GetSkill( pQuestProp->m_nEndCondSkillIdx );
					if( g_pPlayer->CheckSkill( pQuestProp->m_nEndCondSkillIdx ) )
					{
						strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, lpSkill->dwLevel, pQuestProp->m_nEndCondSkillLvl );
						if( lpSkill->dwLevel >= pQuestProp->m_nEndCondSkillLvl )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					{
						strTemp.Format( GETTEXT( TID_QUEST_SKILL_LVL ), pSkillProp->szName, 0, pQuestProp->m_nEndCondSkillLvl );
						strCond += strTemp + "\n";
					}
				}
				if( pQuestProp->m_nEndCondKarmaPoint ) 
				{
					if( pQuestProp->m_nEndCondKarmaComp == 0 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),"=",pQuestProp->m_nEndCondKarmaPoint );
						if( g_pPlayer->GetKarma() == pQuestProp->m_nEndCondKarmaPoint )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					if( pQuestProp->m_nEndCondKarmaComp == -1 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),"<=",pQuestProp->m_nEndCondKarmaPoint );
						if( g_pPlayer->GetKarma() <= pQuestProp->m_nEndCondKarmaPoint )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					if( pQuestProp->m_nEndCondKarmaComp == 1)
					{
						strTemp.Format( GETTEXT( TID_QUEST_KARMA_POINT ), g_pPlayer->GetKarma(),">=",pQuestProp->m_nEndCondKarmaPoint );
						if( g_pPlayer->GetKarma() >= pQuestProp->m_nEndCondKarmaPoint )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
				}
				// 파티 체크 
				if( pQuestProp->m_nEndCondParty != 0 ) 
				{
					BOOL bLeader = FALSE;
					BOOL bParty  = g_Party.IsMember( g_pPlayer->m_idPlayer );
					int nSize = g_Party.GetSizeofMember();

					if( pQuestProp->m_nEndCondParty == 1 ) // 싱글이어야함 
					{
						strTemp.Format( GETTEXT( TID_QUEST_PARTY_SOLO ) );
						strCond += MakeQuestString( strTemp, !bParty );
					}
					else
					if( pQuestProp->m_nEndCondParty == 2 ) // 파티여야함
					{
						int nTextId = 0;
						BOOL bResult = FALSE;

						if( pQuestProp->m_nEndCondPartyNum == 0 ) 
						{
							if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
								nTextId = TID_QUEST_PARTY;
							else // 파티, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 1 )
								nTextId = TID_QUEST_PARTY_MEMBER;
							else // 파티, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 2 )
								nTextId = TID_QUEST_PARTY_LEADER;

							strTemp.Format( GETTEXT( nTextId ) );

							if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 
								bResult = TRUE;
							else // 파티, 맴버 여부 
							if( ( pQuestProp->m_nEndCondPartyLeader - 1 ) == bLeader )
								bResult = TRUE;
						}
						else
						{
							if( pQuestProp->m_nEndCondPartyLeader == 0 ) // 파티 여부 (0 == 0 )
								nTextId = TID_QUEST_PARTY_NUM;
							else // 파티, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 1 )
								nTextId = TID_QUEST_PARTY_MEMBER_NUM;
							else // 파티, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondPartyLeader == 2 )
								nTextId = TID_QUEST_PARTY_LEADER_NUM;

							if( pQuestProp->m_nEndCondPartyNumComp == 0 ) 
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondPartyNum );
								if( nSize == pQuestProp->m_nEndCondPartyNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondPartyNumComp == -1 )
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondPartyNum );
								if( nSize <= pQuestProp->m_nEndCondPartyNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondPartyNumComp == 1)
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondPartyNum );
								if( nSize >= pQuestProp->m_nEndCondPartyNum )
									bResult = TRUE;
							}
						}
						strCond += MakeQuestString( strTemp, bResult && bParty );
					}
				}
				// 길드 체크 
				if( pQuestProp->m_nEndCondGuild != 0 ) 
				{
					BOOL bLeader = FALSE;
					BOOL bGuild  = g_Party.IsMember( g_pPlayer->m_idPlayer );
					int nSize = g_Party.GetSizeofMember();



					if( pQuestProp->m_nEndCondGuild == 1 ) // 길드가 아니어야됨 
					{
						strTemp.Format( GETTEXT( TID_QUEST_GUILD_SOLO ) );
						strCond += MakeQuestString( strTemp, !bGuild );
					}
					else
					if( pQuestProp->m_nEndCondGuild == 2 ) // 길드여야됨 
					{
						int nTextId = 0;
						BOOL bResult = FALSE;

						if( pQuestProp->m_nEndCondGuildNum == 0 ) 
						{
							if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 파티 여부 (0 == 0 )
								nTextId = TID_QUEST_GUILD;
							else // 파티, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 1 )
								nTextId = TID_QUEST_GUILD_MEMBER;
							else // 파티, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 2 )
								nTextId = TID_QUEST_GUILD_LEADER;

							strTemp.Format( GETTEXT( nTextId ) );

							if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부 
								bResult = TRUE;
							else // 길드, 리더 여부 
							if( ( pQuestProp->m_nEndCondGuildLeader - 1 ) == bLeader )
								bResult = TRUE;
						}
						else
						{
							if( pQuestProp->m_nEndCondGuildLeader == 0 ) // 길드 여부 (0 == 0 )
								nTextId = TID_QUEST_GUILD_NUM;
							else // 길드, 맴버 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 1 )
								nTextId = TID_QUEST_GUILD_MEMBER_NUM;
							else // 길드, 리더 여부 (0 == 0 ) 
							if( pQuestProp->m_nEndCondGuildLeader == 2 )
								nTextId = TID_QUEST_GUILD_LEADER_NUM;

							if( pQuestProp->m_nEndCondGuildNumComp == 0 ) 
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"=",pQuestProp->m_nEndCondGuildNum );
								if( nSize == pQuestProp->m_nEndCondGuildNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondGuildNumComp == -1 )
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,"<=",pQuestProp->m_nEndCondGuildNum );
								if( nSize <= pQuestProp->m_nEndCondGuildNum )
									bResult = TRUE;
							}
							else
							if( pQuestProp->m_nEndCondGuildNumComp == 1)
							{
								strTemp.Format( GETTEXT( nTextId ), nSize,">=",pQuestProp->m_nEndCondGuildNum );
								if( nSize >= pQuestProp->m_nEndCondGuildNum )
									bResult = TRUE;
							}
						}
						strCond += MakeQuestString( strTemp, bResult && bGuild );
					}
				}
				//////////////////////////////////////////////////////
				if( pQuestProp->m_nEndCondChaotic ) 
				{
					if( pQuestProp->m_nEndCondChaotic == 1 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_CAOTIC1 ) );
						if( g_pPlayer->IsChaotic() )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
					else
					if( pQuestProp->m_nEndCondChaotic == 2 )
					{
						strTemp.Format( GETTEXT( TID_QUEST_CAOTIC2 ) );
						if( g_pPlayer->IsChaotic() == FALSE )
							strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
						else
							strCond += strTemp + "\n";
					}
				}
				if( pQuestProp->m_szEndCondDlgCharKey[ 0 ] )
				{
					LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondDlgCharKey );
					strTemp.Format( GETTEXT( TID_QUEST_DIALOG ), lpCharacter->m_strName );
					if( lpQuest->m_bDialog )
						strCond += "#s#cffa0a0a0" + strTemp + "#ns#nc\n";
					else
						strCond += strTemp + "\n";
				}
				if( pQuestProp->m_szEndCondCharacter[ 0 ] ) 
				{
					LPCHARACTER lpCharacter = prj.GetCharacter( pQuestProp->m_szEndCondCharacter );
					strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
					strCond += strTemp + "\n";
				}
				else
				if( pQuestProp->m_lpszEndCondMultiCharacter ) 
				{
					for( int i = 0; i < 10; i++ )
					{
						CHAR* lpszChar = &pQuestProp->m_lpszEndCondMultiCharacter[ i * 64 ];
						if( lpszChar[ 0 ] )
						{
							LPCHARACTER lpCharacter = prj.GetCharacter( lpszChar );
							if( lpCharacter )
							{
								for( int j = 0; j < lpCharacter->m_anDstQuestItem.GetSize(); j++ )
								{
									if( g_pPlayer->GetItemNum( lpCharacter->m_anDstQuestItem.GetAt( j ) ) )
									{
										strTemp.Format( GETTEXT( TID_QUEST_DESTINATION ), lpCharacter->m_strName );
										strCond += strTemp + "\n";
										i = 10;
										break;
									}
								}
							}
						}
					}
				}
			}
			pTextCond->SetString( strCond, 0xff404040 );
		}

		//////////////////////////////////////////////////////////////////////////////////
		// 보상 아이템 목록
		//////////////////////////////////////////////////////////////////////////////////
		CString strReward;

		for( int i = 0; i < pQuestProp->m_nEndRewardItemNum; i++ )
		{
			QuestPropItem* pEndRewardItem = &pQuestProp->m_paEndRewardItem[ i ]; 
#ifdef __V050524_QUEST
			if( pEndRewardItem->m_nSex == -1 || pEndRewardItem->m_nSex == g_pPlayer->GetSex() )
#endif
			{
				if( pEndRewardItem->m_nType == 0 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || pEndRewardItem->m_nJobOrItem == g_pPlayer->GetJob() )
					{
#ifdef __V050524_QUEST
						if( pEndRewardItem->m_nItemIdx )
						{
							ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
							if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
								strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
							else
								strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
							strReward += strTemp + "\n";
						}
#else
//{{AFX
						for( int j = 0; j < 4; j++ )
						{
							if( pEndRewardItem->m_anItemIdx[ j ] )
							{
								ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_anItemIdx[ j ] );
								if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
									strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_anItemNum[ j ] );
								else
									strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
								strReward += strTemp + "\n";
							}
						}
//}}AFX
#endif
					}
				}
				else
				if( pEndRewardItem->m_nType == 1 )
				{
					if( pEndRewardItem->m_nJobOrItem == -1 || g_pPlayer->GetItemNum( pEndRewardItem->m_nJobOrItem ) ) 
					{
#ifdef __V050524_QUEST
							if( pEndRewardItem->m_nItemIdx )
							{
								ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_nItemIdx );
								if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
									strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_nItemNum );
								else
									strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
								strReward += strTemp + "\n";
							}
#else
//{{AFX
						for( int j = 0; j < 4; j++ )
						{
							if( pEndRewardItem->m_anItemIdx[ j ] )
							{
								ItemProp* pItemProp = prj.GetItemProp( pEndRewardItem->m_anItemIdx[ j ] );
								if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
									strTemp.Format( GETTEXT( TID_QUEST_ITEM1 ), pItemProp->szName, pEndRewardItem->m_anItemNum[ j ] );
								else
									strTemp.Format( GETTEXT( TID_QUEST_ITEM2 ) );
								strReward += strTemp + "\n";
							}
						}
//}}AFX
#endif
					}
				}
			}
		}
		if( pQuestProp->m_nEndRewardGoldMin )
		{
			if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
			{
				if( ( pQuestProp->m_nEndRewardGoldMax - pQuestProp->m_nEndRewardGoldMin ) == 0 )
					strTemp.Format( GETTEXT( TID_QUEST_GOLD1 ), pQuestProp->m_nEndRewardGoldMin );
				else
					strTemp.Format( GETTEXT( TID_QUEST_GOLD2 ), pQuestProp->m_nEndRewardGoldMin, pQuestProp->m_nEndRewardGoldMax );
			}
			else
				strTemp.Format( GETTEXT( TID_QUEST_GOLD3 ) );
			strReward += strTemp + "\n";
		}
		if( pQuestProp->m_nEndRewardExpMin )
		{
			strTemp.Format( GETTEXT( TID_QUEST_EXP1 ) );
			/*
			if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
			{
				if( ( pQuestProp->m_nEndRewardExpMax - pQuestProp->m_nEndRewardExpMin ) == 0 )
					strTemp.Format( GETTEXT( TID_QUEST_EXP1 ), pQuestProp->m_nEndRewardExpMin );
				else
					strTemp.Format( GETTEXT( TID_QUEST_EXP2 ), pQuestProp->m_nEndRewardExpMin, pQuestProp->m_nEndRewardExpMax );
			}
			else
				strTemp.Format( GETTEXT( TID_QUEST_EXP3 ) );
				*/
			strReward += strTemp + "\n";
		}
		if( pQuestProp->m_nEndRewardKarmaStyle )
		{
			if( pQuestProp->m_bEndRewardHide == FALSE || bComplete == TRUE )
			{
				if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
					strTemp.Format( GETTEXT( TID_QUEST_MODIFY_KARMA1 ), pQuestProp->m_nEndRewardKarmaPoint );
				else
					strTemp.Format( GETTEXT( TID_QUEST_ADD_KARMA1 ), pQuestProp->m_nEndRewardKarmaPoint );
			}
			else
			{
				if( pQuestProp->m_nEndRewardKarmaStyle == 1 )
					strTemp.Format( GETTEXT( TID_QUEST_MODIFY_KARMA2 ) );
				else
					strTemp.Format( GETTEXT( TID_QUEST_ADD_KARMA2 ) );
			}
			strReward += strTemp + "\n";
		}
		pTextReward->SetString( strReward );
	}
	else
	{
		pWndRemove->EnableWindow( FALSE );
		pTextDesc->SetString( "" );
		pTextCond->SetString( "" );
		pTextReward->SetString( "" );
	}
	pTextDesc->ResetString();
	pTextCond->ResetString();
	pTextReward->ResetString();
#else
//{{AFX
	QuestProp* pQuestProp; 
	CWndListBox* pListBox;
	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );	
	switch( lpTapCtrl->GetCurSel() )
	{
	case 0: pListBox = &m_listGeneral; break;
	case 1: pListBox = &m_listRequest; break;
	case 2: pListBox = &m_listScenario; break;
	}
	CWndButton* pWndRemove = (CWndButton*)GetDlgItem( WIDC_REMOVE );
	CWndText* pTextDesc   = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndText* pTextCond   = (CWndText*)GetDlgItem( WIDC_TEXT2 );
	CWndText* pTextName   = (CWndText*)GetDlgItem( WIDC_TEXT3 );
	CWndText* pTextStatus = (CWndText*)GetDlgItem( WIDC_TEXT3 );
	LPQUEST lpQuest = NULL;
	if( pListBox->GetCurSel() >= 0 )
	{
		int nId = pListBox->GetItemData( pListBox->GetCurSel() );
		lpQuest = g_pPlayer->FindQuest( nId );
	}
	if( lpQuest ) 
	{
		pQuestProp = prj.m_aPropQuest.GetAt( lpQuest->m_wId );
		// 영자는 언제나 퀘스트를 지울 수 있다.
		if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) && g_Option.m_bOperator )
			pWndRemove->EnableWindow( TRUE );
		else
		if( lpQuest->m_nState == QS_END )
		{
			pWndRemove->EnableWindow( FALSE );
		}
		else
		{
			if( pQuestProp->m_bNoRemove )
				pWndRemove->EnableWindow( FALSE );
			else
			{
				if( lpQuest->m_wId >= 1000 && pQuestProp->m_wId < 2000 ) // 시나리오는 못지운다.
					pWndRemove->EnableWindow( FALSE );
				else
					pWndRemove->EnableWindow( TRUE );
			}
		}
		QuestState* pQuestState = pQuestProp->m_questState[ lpQuest->m_nState ];

		//QuestState* pQuestState = &pQuestProp->m_questState[ lpQuest->m_nState ];
		pTextDesc->SetString( pQuestState->m_szDesc, 0xff000000 );

	//	pTextDesc->AddString( "\n", 0xff000000 );

		pTextCond->SetString( pQuestState->m_szCond, prj.GetTextColor(TID_GAME_QUEST_DONE) );//"상태 : 완료", 0xff000000 );
		if( pQuestState->m_szStatus[ 0 ] ) 
		{
			if( lpQuest->m_nState == 14 )
				pTextStatus->SetString( prj.GetText(TID_GAME_QUEST_DONE), prj.GetTextColor(TID_GAME_QUEST_DONE) );//"상태 : 완료", 0xff000000 );
			else
				pTextStatus->SetString( prj.GetText(TID_GAME_QUEST_ING), prj.GetTextColor(TID_GAME_QUEST_ING) );//"상태 : 진행중", 0xff000000 );
		}
		else
		{
//			pTextStatus->SetString( pQuestState->m_szStatus, prj.GetTextColor(TID_GAME_QUEST_DONE) );//"상태 : 완료", 0xff000000 );
			pTextStatus->SetString( prj.GetText(TID_GAME_QUEST_DONE), prj.GetTextColor(TID_GAME_QUEST_DONE) );//"상태 : 완료", 0xff000000 );
		}
		LPCHARACTER lpChar = prj.GetCharacter( pQuestProp->m_szLinkChar );

		if( lpChar )
		{
			pTextName->SetString( lpChar->m_strName );
		}
		else
		{
			pTextName->SetString( pQuestProp->m_szNpcName );
		}
	}
	else
	{
		pWndRemove->EnableWindow( FALSE );
		pTextDesc->SetString( "" );
		pTextCond->SetString( "" );
		pTextName->SetString( "" );
		pTextStatus->SetString( "" );
	}
	pTextDesc->ResetString();
	pTextCond->ResetString();
	pTextName->ResetString();
	pTextStatus->ResetString();
//}}AFX
#endif // __V050322_CACHEQUEST
}
void CWndQuest::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

#ifdef __V050322_CACHEQUEST	
#else // __V050322_CACHEQUEST
	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

	//m_listGeneral;
	//m_listScenario;
	//m_listRequest;
	CRect rect = GetClientRect();
//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
	m_listGeneral.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect, lpTapCtrl, 100000 );
	m_listRequest.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, lpTapCtrl, 1000002 );
	m_listScenario.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, lpTapCtrl, 1000001 );
	
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_NORQUEST);//"일반"
	tabTabItem.pWndBase = &m_listGeneral;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	//lpTapCtrl->InsertTexture( 0 ,MakePath( DIR_THEME, "TabCharacter1.bmp" ) );

	tabTabItem.pszText = prj.GetText(TID_GAME_REQQUEST);//"의뢰소";
	tabTabItem.pWndBase = &m_listRequest;
	lpTapCtrl->InsertItem( 1, &tabTabItem );
	//lpTapCtrl->InsertTexture( 1 ,MakePath( DIR_THEME, "TabCharacter2.bmp" ) );

	
	tabTabItem.pszText = prj.GetText(TID_GAME_SCEQUEST);//"시나리오";
	tabTabItem.pWndBase = &m_listScenario;
	lpTapCtrl->InsertItem( 2, &tabTabItem );
	//lpTapCtrl->InsertTexture( 2 ,MakePath( DIR_THEME, "TabCharacter3.bmp" ) );

	lpTapCtrl->SetCurSel( 0 );
#endif // __V050322_CACHEQUEST

//	lpTapCtrl->SetButtonLength( 55 );
	
	Update();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuest::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEST, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndQuest::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndQuest::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	// 여기서 pWndBase가 NULL이거나 쓰레기 값이라서 다운된적이 있음. 추적요망.
	/*
	if( pWndBase->GetWndId() == APP_MESSAGEBOX )
	{
		if( nID == WIDC_OK)
		{
			CWndListBox* pListBox = (CWndListBox*)GetDlgItem( WIDC_LIST1 );
			int nId = pListBox->GetItemData( pListBox->GetCurSel() );
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nId );
			if( pQuestProp )
			{
				LPQUEST pQuest = g_pPlayer->GetQuest( nId );
				// 삭제하는데, 아직 퀘스트가 종료된 것이 아니라면 취소.
				// 15일 경우는 퀘스트가 종료된 것인데, 종료된걸 제거한다고 취소 되었다고 나오면 곤란하다.
				// 따라서 15보다 작을때(진행중일때)만 매시지 출력.
				if( pQuest )
				{
					if( pQuest->m_nState < 14 ) 
					{
						CString string;
						string.Format( GETTEXT( TID_EVE_CANCELQUEST ), pQuestProp->m_szTitle );
						g_WndMng.PutString( string, NULL, GETTEXTCOLOR( TID_EVE_CANCELQUEST ) );
					}
#ifdef __ONLINE
					g_DPlay.SendScriptRemoveQuest( nId );
#else	// __ONLINE
					g_pPlayer->RemoveQuest( nId );
					CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetApplet( APP_QUEST );
					if( pWndQuest )
						pWndQuest->Update(); 
#endif	// __ONLINE
				}
				
			}

		}
	}
	*/
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndQuest::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndQuest::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndQuest::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndQuest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#ifdef __V050322_CACHEQUEST
	int nId = 0;
	CWndTreeCtrl* pTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	LPTREEELEM lpTreeElem = pTreeCtrl->GetCurSel();
	if( lpTreeElem && lpTreeElem->m_lpParent )
		nId = lpTreeElem->m_dwData;
	if( lpTreeElem )
		m_idSelQuest = lpTreeElem->m_dwData;

	if( ( message == WNM_CLICKED || message == WNM_SELCANCEL ) ) //&& (nID == 100000 || nID == 1000001 || nID == 1000002 ) )
	{
		UpdateText(); 
		LPTREEELEM lpRoot = pTreeCtrl->GetRootElem();
		int nPos = 0;
		LPTREEELEM lpElem = NULL;
		m_aOpenTree.RemoveAll();
		do {
			lpElem = pTreeCtrl->GetNextElem( lpRoot, nPos );
			if( lpElem && lpElem->m_bOpen )
				m_aOpenTree.Add( lpElem->m_dwData );
		} while( lpElem );
	}
#else // __V050322_CACHEQUEST
	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CWndListBox* pListBox = NULL;//&m_listGeneral;//(CWndListBox*)GetDlgItem( WIDC_LIST1 );
	switch( lpTapCtrl->GetCurSel() )
	{
	case 0: pListBox = &m_listGeneral; break;
	case 1: pListBox = &m_listRequest; break;
	case 2: pListBox = &m_listScenario; break;
	}
	int nId = 0;
	if( pListBox->GetCurSel() >= 0 )
		nId = pListBox->GetItemData( pListBox->GetCurSel() );

	if( ( message == WNM_SELCHANGE || message == WNM_SELCANCEL ) ) //&& (nID == 100000 || nID == 1000001 || nID == 1000002 ) )
	{
		UpdateText(); 
	}
#endif // __V050322_CACHEQUEST


	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
		case WIDC_REMOVE: // cancel
			{
				CWndRemoveQuest* pWndRemoveQuest = new CWndRemoveQuest;
				pWndRemoveQuest->m_nRemoveQuestId = nId;
				g_WndMng.OpenCustomBox( NULL, pWndRemoveQuest );
			}
			break;
		case WIDC_CLOSE:
			nID = WTBID_CLOSE;
			break;
			/*
		case WIDC_DELETE:
			{
				g_WndMng.OpenMessageBox( "퀘스트를 중단하시겠습니까?", MB_OKCANCEL, this );
			}
			break;
			*/
			
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
