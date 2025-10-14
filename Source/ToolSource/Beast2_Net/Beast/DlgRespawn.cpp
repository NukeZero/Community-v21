// DlgRespawn.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgRespawn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgRespawn dialog


CDlgRespawn::CDlgRespawn(CWorld* pWorld,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRespawn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRespawn)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pWorldView = (CWorldView*)pParent;
	m_pWorld = pWorld;
	m_selectItem.iItem = -1;
}


void CDlgRespawn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRespawn)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRespawn, CDialog)
	//{{AFX_MSG_MAP(CDlgRespawn)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESPAWN, OnDblclkListRespawn)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RESPAWN, OnClickListRespawn)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_RESPAWN, OnItemchangingListRespawn)
	ON_BN_CLICKED(IDC_MOVE, OnMove)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_RESPAWN, OnColumnclickListRespawn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRespawn message handlers

BOOL CDlgRespawn::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	TCHAR	szTitle[7][10] = {_T("NO."),_T("이름"),_T("타입"), _T("갯수"), 
		_T("시간M"),_T("지연S"),_T("범위")};
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_RESPAWN);
	LV_COLUMN  pColumn;
	CRect      rect;
	GetClientRect(&rect);
	for(int i = 0;  i < 7; i++)
	{
		pColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		pColumn.fmt = LVCFMT_LEFT;
		pColumn.iSubItem = i;
		//if(i == 0)
			pColumn.cx = rect.Width() / 7;  // SubItem is twice as large
		//else
		//if(i == 1)
//			pColumn.cx = 70;  // SubItem is twice as large
	//	else
		//	pColumn.cx = 70;  // SubItem is twice as large
		pColumn.pszText = szTitle[i];	
		pList->InsertColumn(i, &pColumn);  // assumes return value is OK.
	}
	Update(m_pWorldView,m_pWorld);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgRespawn::Update(CWorldView* pWorldView,CWorld* pWorld) 
{/*
	//CPropertyPage::OnInitDialog();
	m_pWorldView = pWorldView;
	m_pWorld = pWorld;
	CRect      rect; 
	int        i  = 0;
	int        i2 = 0;
	CItem*     pItem;
	ItemProp*  pItemProp;
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_RESPAWN);

	LV_COLUMN  pColumn;
	LV_ITEM	   lvitem;
	CString    string;

	pList->DeleteAllItems();
	pList->GetWindowRect(&rect);

	if(m_pWorldView == NULL)
		return;

	int nMoverNum = 0;
	int nItemNum = 0;
	//
	// 소스 아이템을 입력
	//
	for(i = pWorld->m_nRespawnSize-1; i >= 0; i--) 
	{
		CRespawnObj* pRespawnObj = &pWorld->m_pRespawnAry[i];
		lvitem.mask = LVIF_TEXT | LVIF_PARAM; // | (iSubItem == 0? LVIF_IMAGE : 0);
		lvitem.iItem = pList->GetItemCount(); //(iSubItem == 0)? iItem : iActualItem;
		// 번호  
		lvitem.mask = LVIF_TEXT| LVIF_PARAM;
		lvitem.iSubItem = 0; 
		string.Format("%d",i);
		lvitem.pszText = string.LockBuffer(); string.UnlockBuffer();
		lvitem.lParam = (LPARAM) pRespawnObj;
		pList->InsertItem(&lvitem);
		// 이름 
		lvitem.iSubItem = 1; 
		lvitem.mask = LVIF_TEXT;
		if(pRespawnObj->m_nType == OT_MOVER)
		{
			string = prj.GetMoverProp(pRespawnObj->m_nIdx)->m_strName;
			nMoverNum += pRespawnObj->m_nTotalCount;
		}
		else
		{
			string = prj.GetItemProp(pRespawnObj->m_nIdx)->m_strName;
			nItemNum += pRespawnObj->m_nTotalCount;
		}
		lvitem.pszText = string.LockBuffer(); string.UnlockBuffer();
		pList->SetItem(&lvitem);
		// 타입 
		lvitem.iSubItem = 2;
		lvitem.lParam = (LPARAM) pRespawnObj;
		if(pRespawnObj->m_nType == OT_MOVER)
			lvitem.pszText = "MOVER";
		else
			lvitem.pszText = "ITEM";
		pList->SetItem(&lvitem);
		// 갯수
		lvitem.iSubItem = 3; 
		string.Format("%d",pRespawnObj->m_nTotalCount);
		lvitem.pszText = string.LockBuffer(); string.UnlockBuffer();
		pList->SetItem(&lvitem);
		// 시간 
		lvitem.iSubItem = 4; 
		string.Format("%d",pRespawnObj->m_nTimeRespawn);
		lvitem.pszText = string.LockBuffer(); string.UnlockBuffer();
		pList->SetItem(&lvitem);
		// 지연 
		lvitem.iSubItem = 5; 
		string.Format("%d",pRespawnObj->m_nTimeDelay);
		lvitem.pszText = string.LockBuffer(); string.UnlockBuffer();
		pList->SetItem(&lvitem);
		// 사이즈
		lvitem.iSubItem = 6; 
		string.Format("%d,%d",pRespawnObj->m_rectRange.Width()/CWorld::LG_GRID_X,pRespawnObj->m_rectRange.Height()/CWorld::LG_GRID_Y);
		lvitem.pszText = string.LockBuffer(); string.UnlockBuffer();
		pList->SetItem(&lvitem);
	}
	CStatic* pTotalStc = (CStatic*)GetDlgItem(IDC_STATIC_TOTAL);
	CStatic* pMoverStc = (CStatic*)GetDlgItem(IDC_STATIC_MOVER);
	CStatic* pItemStc = (CStatic*)GetDlgItem(IDC_STATIC_ITEM);
	string.Format("%d",nMoverNum+nItemNum); pTotalStc->SetWindowText(string);
	string.Format("%d",nMoverNum); pMoverStc->SetWindowText(string);
	string.Format("%d",nItemNum); pItemStc->SetWindowText(string);
	*/
}
void CDlgRespawn::OnDblclkListRespawn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	if(m_pWorldView)
	{
		// TODO: Add your control notification handler code here
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		if(pNMListView->iItem != -1)
		{
			//CRespawnObj* pRespawnObj = &m_pWorld->m_pRespawnAry[m_pWorld->m_nRespawnSize-pNMListView->iItem-1];
			int nIdx = m_pWorld->FindRespawnObjIdx((CRespawnObj*)m_selectItem.lParam);
			if(nIdx >= 0)
			{
				CRespawnObj* pRespawnObj = &m_pWorld->m_pRespawnAry[nIdx];
				CSetRespawnDlg setRespawnDlg(pRespawnObj);

				if(setRespawnDlg.DoModal())
				{
					m_pWorldView->UpdateField();
					Update(m_pWorldView,m_pWorld);

					//CButton* pButton = (CButton*)GetDlgItem(IDC_SAVE);
					//pButton->EnableWindow(TRUE);
				}
			}
			else
				AfxMessageBox("리스폰 객체를 찾을 수 없음.");
		}
	}
	*/
	*pResult = 0;
}

void CDlgRespawn::OnClickListRespawn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CDlgRespawn::OnItemchangingListRespawn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_selectItem = *pNMListView;
	*pResult = 0;
}
/*
void CDlgRespawn::EnableSave(BOOL bEnable)
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_SAVE);
	pButton->EnableWindow(bEnable);
}
*/
void CDlgRespawn::OnRemove() 
{
	/*
	if(m_pWorldView && m_selectItem.iItem != -1)
	{
		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_RESPAWN);
		int nIdx = m_pWorld->FindRespawnObjIdx((CRespawnObj*)m_selectItem.lParam);
		if(nIdx >= 0)
		{
			m_pWorld->RemoveRespawnObj(nIdx);
			Update(m_pWorldView,m_pWorld);	
			m_pWorldView->UpdateField();
		}
		else
			AfxMessageBox("리스폰 객체를 찾을 수 없음.");
		//EnableSave(TRUE);
	}
	*/
}

void CDlgRespawn::OnRemoveAll() 
{
	/*
	if(m_pWorldView)
	{
		if(AfxMessageBox("모두 삭제할 것인가?",MB_OKCANCEL) == IDOK)
		{
			m_pWorld->m_nRespawnSize = 0;
			Update(m_pWorldView,m_pWorld);
			m_pWorldView->UpdateField();
			//EnableSave(TRUE);
		}
	}
	*/
}
void CDlgRespawn::OnMove() 
{
	/*
	if(m_pWorldView && m_selectItem.iItem != -1)
	{
		//CRespawnObj* pRespawnObj = &m_pWorld->m_pRespawnAry[m_selectItem.lParamm_pWorld->m_nRespawnSize-m_selectItem.iItem-1];
		int nIdx = m_pWorld->FindRespawnObjIdx((CRespawnObj*)m_selectItem.lParam);
		if(nIdx >= 0)
		{
			CRespawnObj* pRespawnObj = &m_pWorld->m_pRespawnAry[nIdx];
			// 출력 화면의 사이즈를 계산하고, 중심에 맞추기 위해 m_pos를 변경한다.
			CPoint pt(m_pWorldView->m_pBitmap->GetWidth()/2,m_pWorldView->m_pBitmap->GetHeight()/2);
			pt = CWorld::PgToLg(pt);
			CPoint point = pRespawnObj->m_ptz;
			point -= pt;
			m_pWorld->SetFieldPos(point);
			// 필드뷰의 스크롤바를 세팅한다.
			pt = CPoint(m_pWorld->GetFieldPos());
			pt = CWorld::LgToPg(pt);
			pt.y += 4 * (CWorld::PG_GRID_Y / 2);	
			pt.x += (m_pWorld->m_sizeLgGrid.cy + 4) * (CWorld::PG_GRID_X / 2);	
			m_pWorldView->SetScrollPos(SB_HORZ,pt.x);
			m_pWorldView->SetScrollPos(SB_VERT,pt.y);
			m_pWorldView->UpdateField();
		}
		else
			AfxMessageBox("리스폰 객체를 찾을 수 없음.");
	} 
	*/
}
void CDlgRespawn::OnSave() 
{
	/*
	if(m_pWorldView && m_pWorld)
	{
		m_pWorld->SaveRespawn(m_pWorld->GetRespawnFileName(m_pWorld->m_strFileName));
		CButton* pButton = (CButton*)GetDlgItem(IDC_SAVE);
		//EnableSave(FALSE);
	}
	*/
}
void CDlgRespawn::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	/*
	CDialog::OnClose();
	if(m_pWorldView->m_pRespawnDlg)
		m_pWorldView->m_pRespawnDlg = NULL;
	delete this;
*/
}

void CDlgRespawn::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
/*
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_RESPAWN);
	CButton* pButton1 = (CButton*)GetDlgItem(IDC_REMOVE);
	CButton* pButton2 = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
	CButton* pButton3 = (CButton*)GetDlgItem(IDC_MOVE);
	CButton* pButton4 = (CButton*)GetDlgItem(IDC_SAVE);
	if(pList)
	{
		CRect rect;
		GetClientRect(&rect);
		rect.left += 10;
		rect.top += 30;
		rect.right -= 10;
		rect.bottom -= 50;

		pList->MoveWindow(rect);
		pList->SetColumnWidth(0, rect.Width()/7);
		pList->SetColumnWidth(1, rect.Width()/7);
		pList->SetColumnWidth(2, rect.Width()/7);
		pList->SetColumnWidth(3, rect.Width()/7);
		pList->SetColumnWidth(4, rect.Width()/7);
		pList->SetColumnWidth(5, rect.Width()/7);
		pList->SetColumnWidth(6, rect.Width()/7);

		CRect rect2;
		pButton1->GetClientRect(&rect2);
		rect2.OffsetRect(rect.left,rect.bottom+5);
		pButton1->MoveWindow(rect2);

		CRect rect3;
		pButton2->GetClientRect(&rect3);
		rect3.OffsetRect(rect2.right+5,rect.bottom+5);
		pButton2->MoveWindow(rect3);

		CRect rect4;
		pButton3->GetClientRect(&rect4);
		rect4.OffsetRect(rect3.right+5,rect.bottom+5);
		pButton3->MoveWindow(rect4);

		CRect rect5;
		pButton4->GetClientRect(&rect5);
		rect5.OffsetRect(rect.right-rect5.Width(),rect.bottom+5);
		pButton4->MoveWindow(rect5);
	}
	*/
}
void CDlgRespawn::OnColumnclickListRespawn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	//TCHAR	szTitle[7][10] = {_T("NO."),_T("이름"),_T("타입"), _T("갯수"), _T("시간M"),_T("지연S"),_T("범위")};
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_RESPAWN);
	switch(pNMListView->iSubItem)
	{
		case 0: // 번호 
			pList->SortItems(CompareFunc,0);
			break;
		case 1: // 이름 
			pList->SortItems(CompareFunc,1);
			break;
	}
	*pResult = 0;
	*/
}