// DlgPatrol.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "world.h"
#include "DlgPatrol.h"
#include "WorldDoc.h"
#include "WorldView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPatrol dialog


CDlgPatrol::CDlgPatrol(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPatrol::IDD, pParent), m_dwIndex(0), m_dwOldIndx(-1)
{
}


void CDlgPatrol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPatrol)
	DDX_Control(pDX, IDC_PATH_EDIT, m_Edit);
	DDX_Control(pDX, IDC_PATH_LIST, m_PathList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPatrol, CDialog)
	//{{AFX_MSG_MAP(CDlgPatrol)
	ON_BN_CLICKED(IDC_PATH_NEW, OnPathNew)
	ON_BN_CLICKED(IDC_PATH_DELETE, OnPathDelete)
	ON_BN_CLICKED(IDC_PATH_EDIT, OnPathEdit)
	ON_LBN_SELCHANGE(IDC_PATH_LIST, OnSelchangePathList)
	ON_BN_CLICKED(IDC_PATROL_SAVE, OnPatrolSave)
	ON_BN_CLICKED(IDC_APPLY_OBJECT, OnApplyObject)
	ON_BN_CLICKED(IDC_PATROL_LOAD, OnPatrolLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPatrol message handlers

void CDlgPatrol::OnPathNew() 
{
	if( IsEdit() )
		return;
	
	CString str;
	str.Format( "Path %.2d", m_dwIndex );
	CPatrolPath::GetInstance()->AddPatrolIndex( m_dwIndex );

	m_PathList.SetItemData( m_PathList.AddString( str ), m_dwIndex );
	
	m_dwIndex++;
}

void CDlgPatrol::OnPathDelete() 
{
	if( IsEdit() )
		return;

	DWORD dwIndex;
	dwIndex = m_PathList.GetCurSel();
	DWORD dwTEST;
	dwTEST = m_PathList.GetItemData(dwIndex);
		
	m_PathList.DeleteString( dwIndex );
	CPatrolPath::GetInstance()->DeletePatrolIndex( dwTEST );

	m_PathList.UpdateData(FALSE);
}

void CDlgPatrol::OnPathEdit() 
{
/*
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if( pBeastView && pBeastView->m_world.m_pObjFocus == NULL )
	{
		AfxMessageBox( "Please Select Object" );
		m_Edit.SetCheck(FALSE);
		return;
	}

	DWORD dwIndex;
	dwIndex = m_PathList.GetCurSel();

	if( dwIndex == -1 || pBeastView == NULL )
	{
		m_Edit.SetCheck(FALSE);
		return;
	}
*/
	DWORD dwIndex;
	dwIndex = m_PathList.GetCurSel();
	
	if( dwIndex == -1  )
	{
		m_Edit.SetCheck(FALSE);
		return;
	}
	
}

void CDlgPatrol::AddPath(CPathObj* pPathobj)
{
	DWORD dwIndex;
	dwIndex = m_PathList.GetCurSel();
	CPatrolPath::GetInstance()->AddPatrolPath( dwIndex, pPathobj );	
}

void CDlgPatrol::OnSelchangePathList() 
{
	if( m_Edit.GetCheck() == TRUE )
	{
		m_PathList.SetCurSel(m_dwOldIndx);
		return;	
	}

	int nData = m_PathList.GetItemData(m_dwOldIndx);
	SetSelectPathList( nData, FALSE );
	nData = m_PathList.GetItemData(m_PathList.GetCurSel());
	SetSelectPathList( nData, TRUE );
	
	m_dwOldIndx = m_PathList.GetCurSel();		
}

BOOL CDlgPatrol::IsEdit()
{
	DWORD dwIndex;
	dwIndex = m_PathList.GetCurSel();

	if( dwIndex == -1 || m_Edit.GetCheck() == FALSE )
		return FALSE;
	
	return TRUE;
}


void CDlgPatrol::OnPatrolSave() 
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if( pBeastView == NULL )
		return;
	
	CString strLandPathName = pBeastView->m_world.m_szFilePath;
	CString strLandName = pBeastView->m_world.m_szFileName;
	CString strLandTemp;

	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s\\%s.pat", strLandPathName, strLandName );

	if( CPatrolPath::GetInstance()->Save( strLandTemp ) )
	{
		AfxMessageBox( "Save OK" );
	}
}

void CDlgPatrol::UpdateList()
{
	map< DWORD, vector<CPathObj*> >* mapPath = CPatrolPath::GetInstance()->GetMapPatrolPath() ;

	m_PathList.ResetContent();
	m_PathList.SetCurSel(-1);
	m_Edit.SetCheck(FALSE);
	
	CString str;

	for( map< DWORD, vector<CPathObj*> >::iterator it1 = mapPath->begin(); it1 != mapPath->end() ; ++it1 )
	{
		str.Format( "Path %.2d", it1->first );
		m_PathList.SetItemData( m_PathList.AddString( str ), it1->first );
		m_dwIndex = it1->first;
	}

	m_dwIndex++;
	m_dwOldIndx = -1;
}

void CDlgPatrol::OnApplyObject() 
{
	if( IsEdit() )
	{
		AfxMessageBox( "Editing path already." );
		return;
	}
	
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if( pBeastView == NULL )
		return;
	
	if( pBeastView->m_world.m_pObjFocus == NULL )
	{
		AfxMessageBox( "Select an Object to route." );
		return;
	}

	DWORD dwIndex;
	dwIndex = m_PathList.GetCurSel();

	if( dwIndex == -1 )
	{
		AfxMessageBox( "No path is selected." );
		return;
	}

	DWORD dwTEST;
	dwTEST = m_PathList.GetItemData(dwIndex);		

	CObj* pObj = pBeastView->m_world.m_pObjFocus;
	CRespawnInfo* pRespawnInfo = NULL;

	if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->m_respawnInfo.m_dwIndex ) 
	{
		CMover* pMover = (CMover*) pObj; pRespawnInfo = &pMover->m_respawnInfo;
	}
	if( pObj->GetType() == OT_ITEM && ((CItem*)pObj)->m_respawnInfo.m_dwIndex ) 
	{
		CItem* pItem = (CItem*) pObj; pRespawnInfo = &pItem->m_respawnInfo;
	}
	if( pObj->GetType() == OT_CTRL && ((CCommonCtrl*)pObj)->m_respawnInfo.m_dwIndex ) 
	{
		CCommonCtrl* pCtrl = (CCommonCtrl*) pObj; pRespawnInfo = &pCtrl->m_respawnInfo;
	}
		
	if( pRespawnInfo )
	{
		pRespawnInfo->m_dwPatrolIndex = dwTEST;
		CPatrolPath::GetInstance()->UpdatePatrolData( dwTEST );
	}	
	else
	{
		AfxMessageBox( "no respawn info." );
	}
}

void CDlgPatrol::RenderPath( DWORD dwIndex )
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	
	if( pBeastView == NULL )
		return;

	CPatrolPath::GetInstance()->RenderPath( dwIndex, &(pBeastView->m_world) );

}


void CDlgPatrol::RenderPath(CObj *pObj)
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	
	if( pBeastView == NULL )
		return;
	
	CPatrolPath::GetInstance()->RenderPath( pObj, &(pBeastView->m_world) );	
}

void CDlgPatrol::RenderAllPath()
{
	DWORD dwTEST;
	for( int i=0; i<m_PathList.GetCount(); i++ )
	{
		dwTEST = m_PathList.GetItemData(i);
		RenderPath( dwTEST );
	}	
}

void CDlgPatrol::SetSelectPathList(DWORD dwIndex, BOOL bFlag)
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	
	if( pBeastView == NULL )
		return;

	pBeastView->m_world.UnselectObj();
	
	map< DWORD, vector<CPathObj*> >::iterator it = CPatrolPath::GetInstance()->GetMapPatrolPath()->find( dwIndex );
	
	// 기존꺼 선택 지우고
	if( it != CPatrolPath::GetInstance()->GetMapPatrolPath()->end() )
	{
		for( int i=0; i<(int)( it->second.size() ); i++ )
		{
			pBeastView->m_world.SetSelectObj( it->second[i], bFlag );
		}		
	}
	
	pBeastView->Invalidate(FALSE);	
}

void CDlgPatrol::UnSelectPathList()
{
	int nData = m_PathList.GetItemData(m_dwOldIndx);
	SetSelectPathList( nData, FALSE );	
}

CString CDlgPatrol::GetPathStringName(DWORD dwIndex)
{
	DWORD dwTEST;
	CString str;
	str.Empty();
	for( int i=0; i<m_PathList.GetCount(); i++ )
	{
		dwTEST = m_PathList.GetItemData(i);

		if( dwIndex == dwTEST )
		{
			m_PathList.GetText( i, str );
			return str;
		}
	}
	
	return str;
}

void CDlgPatrol::OnPatrolLoad()
{
	LoadPatrol();
}

void CDlgPatrol::LoadPatrol()
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	
	if( pBeastView )
	{	
		CString strLandPathName = pBeastView->m_world.m_szFilePath;
		CString strLandName = pBeastView->m_world.m_szFileName;
		CString strLandTemp;
		
		strLandName.Delete( strLandName.GetLength() - 4, 4 );
		strLandTemp.Format( "%s\\%s.pat", strLandPathName, strLandName );
		
		if( CPatrolPath::GetInstance()->Load( strLandTemp ) )
		{
			AfxMessageBox( "Patrol Load OK" );
		}
		
		UpdateList();	
	}
}
