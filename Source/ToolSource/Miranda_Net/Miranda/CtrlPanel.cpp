// CtrlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "MainFrm.h"
#include "CtrlPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlPanel dialog


CCtrlPanel::CCtrlPanel()
{
	//{{AFX_DATA_INIT(CCtrlPanel)
	m_cbvSize = _T("");
	//}}AFX_DATA_INIT
	m_pDoc		  = NULL;
	m_nObjValue   = 1;
	m_nSfxValue   = 1;
	m_nSelectedID = 0;

}


CMirandaDoc* CCtrlPanel::GetDoc()
{

	return (CMirandaDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();

}


CMirandaView* CCtrlPanel::GetView()
{

	return (CMirandaView*)((CMainFrame *)AfxGetMainWnd())->GetActiveView();

}


void CCtrlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCtrlPanel)
	DDX_Control(pDX, IDC_SLIDER_LIGHTING, m_sLighting);
	DDX_Control(pDX, IDC_MAP_SIZE, m_cbSize);
	DDX_Control(pDX, IDC_OBJ_TREE, m_ObjTree);
	DDX_CBString(pDX, IDC_MAP_SIZE, m_cbvSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCtrlPanel, CDialogBar)
	//{{AFX_MSG_MAP(CCtrlPanel)
	ON_NOTIFY(NM_RCLICK, IDC_OBJ_TREE, OnRclickObjTree)
	ON_COMMAND(ID_MI_ADDOBJ, OnMiAddobj)
	ON_COMMAND(ID_MI_ADDSFX, OnMiAddsfx)
	ON_COMMAND(ID_MI_DELOBJ, OnMiDelobj)
	ON_COMMAND(ID_MI_LOADANIM, OnMiLoadanim)
	ON_COMMAND(ID_MI_LOADBONE, OnMiLoadbone)
	ON_COMMAND(ID_MI_LOADMODEL, OnMiLoadmodel)
	ON_COMMAND(ID_MI_SETLOD, OnMiSetlod)
	ON_COMMAND(ID_MI_DELSFX, OnMiDelsfx)
	ON_COMMAND(ID_MI_LOADSFX, OnMiLoadsfx)
	ON_COMMAND(ID_MI_SET, OnMiSet)
	ON_BN_CLICKED(IDC_CAM_MODE, OnCamMode)
	ON_BN_CLICKED(IDC_TEXTURE, OnTexture)
	ON_BN_CLICKED(IDC_CHANGE_PLAY, OnChangePlay)
	ON_CBN_SELCHANGE(IDC_MAP_SIZE, OnSelchangeMapSize)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CLICK, IDC_OBJ_TREE, OnClickObjTree)
	ON_WM_KEYUP()
	ON_UPDATE_COMMAND_UI(IDC_TEXTURE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_CHANGE_PLAY, OnUpdateCmdUI)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_OBJ_TREE, OnBegindragObjTree)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrlPanel message handlers

LONG CCtrlPanel::OnInitDialog(UINT wParam, LONG lParam) 
{
	#ifdef __VS6
	if(!HandleInitDialog(wParam, lParam) || !UpdateData(FALSE))
	{
		return FALSE;
	}
#else
	//gmpbigsun: for VS2003
	HandleInitDialog( wParam, lParam );

	if( !UpdateData( FALSE ) )
	{
		TRACE0("Warning:UpdateData failed during dialog init. \n" );
		return FALSE;
	}
#endif
	// TODO: Add extra initialization here
	// 오브젝트 트리 설정
	m_ImgTree.Create(16, 16, ILC_COLORDDB, 2, 2);
	m_ImgTree.Add(LoadIcon(AfxGetInstanceHandle(), (LPCSTR)IDI_ICON4));
	m_ImgTree.Add(LoadIcon(AfxGetInstanceHandle(), (LPCSTR)IDI_ICON1));
	m_ImgTree.Add(LoadIcon(AfxGetInstanceHandle(), (LPCSTR)IDI_ICON2));

	m_ObjTree.SetImageList(&m_ImgTree, TVSIL_NORMAL);
	
	m_Instruct.hParent			= NULL;
	m_Instruct.hInsertAfter		= TVI_LAST;
	m_Instruct.item.mask		= TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM;
	m_Instruct.item.state		= 0;
	m_Instruct.item.stateMask	= 0;
	m_Instruct.item.cchTextMax	= 60;
	m_Instruct.item.cChildren	= 0;

	m_Instruct.item.pszText = "Object List";
	m_Instruct.item.iImage = 1;
	m_Instruct.item.lParam = -1;
	m_ObjRoot = m_ObjTree.InsertItem(&m_Instruct);

	m_Instruct.item.pszText = "Sfx List";
	m_Instruct.item.iImage = 2;
	m_Instruct.item.lParam = -2;
	m_SfxRoot = m_ObjTree.InsertItem(&m_Instruct);

	// 지형크기 콤보박스 설정
	m_cbSize.AddString("10");
	m_cbSize.AddString("20");
	m_cbSize.AddString("30");
	m_cbSize.AddString("40");
	m_cbSize.AddString("50");
	m_cbSize.SetCurSel(1);

	m_PopupMenu.LoadMenu(IDR_MENU1);

	// 라이팅 슬라이더 설정 
	m_sLighting.SetRangeMin(0);
	m_sLighting.SetRangeMax(264); // 5분 단위로 틱을 설정
	m_sLighting.SetPos(0);
	m_sLighting.SetTicFreq(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
	
}

void CCtrlPanel::OnUpdateCmdUI(CCmdUI* pCmdUI)
{

	pCmdUI->Enable(TRUE);

}


void CCtrlPanel::OnClickObjTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	m_pDoc  = GetDoc();
	m_pView = GetView();

	// TODO: Add your control notification handler code here
	CPoint		pt;

	
	GetCursorPos(&pt);

	// 트리 아이템 선택하기 
	m_ObjTree.ScreenToClient(&pt);
	HTREEITEM hit = m_ObjTree.HitTest(pt);
	m_ObjTree.SelectItem(hit);

	if(hit)
	{
		TV_ITEM		item;
		char		text[20];

		memset(text, 0, 20);
		item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
		item.hItem = hit;
		item.pszText = text;
		item.cchTextMax = 60;
		m_ObjTree.GetItem(&item);

		//해당 팝업 띄우기 
		ClientToScreen(&pt);
		if(!strcmp(item.pszText,"Object List"))
		{
			m_nSelectedID = 0;
			m_pDoc->m_Selected.SetSelected(0, 0);
		}
		else if(!strcmp(item.pszText,"Sfx List"))
		{	
			m_nSelectedID = 0;
			m_pDoc->m_Selected.SetSelected(0, 0);
		} 
		// 오브젝트 
		else if(item.iImage == 1 && item.lParam > 0)
		{	
			m_nSelectedID = item.lParam;
			m_pDoc->m_Selected.SetSelected(m_nSelectedID, TYPE_OBJECT);
			m_pDoc->m_ObjMgr.SetFocus(m_nSelectedID);		
		}
		// SFX
		else if(item.iImage == 2 && item.lParam > 0)
		{
			
			m_nSelectedID = item.lParam;
			m_pDoc->m_Selected.SetSelected(m_nSelectedID, TYPE_SFX);
		}
		else
		{
			m_pDoc->m_Selected.SetSelected(0, 0);
		}
	}

}


void CCtrlPanel::OnRclickObjTree(NMHDR* pNMHDR, LRESULT* pResult) 
{

	m_pDoc  = GetDoc();
	m_pView = GetView();

	// TODO: Add your control notification handler code here
	CPoint		pt;
	CMenu*		pMenu;

	
	GetCursorPos(&pt);

	// 트리 아이템 선택하기 
	m_ObjTree.ScreenToClient(&pt);
	HTREEITEM hit = m_ObjTree.HitTest(pt);
	m_ObjTree.SelectItem(hit);

	if(hit)
	{
		TV_ITEM		item;
		char		text[20];

		memset(text, 0, 20);
		item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
		item.hItem = hit;
		item.pszText = text;
		item.cchTextMax = 60;
		m_ObjTree.GetItem(&item);

		//해당 팝업 띄우기 
		ClientToScreen(&pt);
		if(!strcmp(item.pszText,"Object List"))
		{
			pMenu = m_PopupMenu.GetSubMenu(0);
			m_nSelectedID = 0;
			m_pDoc->m_Selected.SetSelected(0, 0);
			pMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, pt.x + 60, pt.y+ 50, this);
		}
		else if(!strcmp(item.pszText,"Sfx List"))
		{
			pMenu = m_PopupMenu.GetSubMenu(2);
			m_nSelectedID = 0;
			m_pDoc->m_Selected.SetSelected(0, 0);
			
			pMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, pt.x + 60, pt.y+ 50, this);
		} 
		// 오브젝트 
		else if(item.iImage == 1 && item.lParam > 0)
		{
			pMenu = m_PopupMenu.GetSubMenu(1);
			m_nSelectedID = item.lParam;
			m_pDoc->m_Selected.SetSelected(m_nSelectedID, TYPE_OBJECT);
			m_pDoc->m_ObjMgr.SetFocus(m_nSelectedID);
			pMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, pt.x + 60, pt.y+ 50, this);
			
		}
		// SFX
		else if(item.iImage == 2 && item.lParam > 0)
		{
			pMenu = m_PopupMenu.GetSubMenu(3);
			m_nSelectedID = item.lParam;
			m_pDoc->m_Selected.SetSelected(m_nSelectedID, TYPE_SFX);
			pMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, pt.x + 60, pt.y+ 50, this);
		}
		else
		{
			m_pDoc->m_Selected.SetSelected(0, 0);
		}
	}
	
	*pResult = 0;
}

void CCtrlPanel::OnMiAddobj() 
{
	// TODO: Add your command handler code here


	// 오브젝트를 추가한다 
	if(m_pDoc->m_ObjMgr.AddNewObj(m_nObjValue))
	{
		AddObj();	
	}
	
}

bool CCtrlPanel::AddObj(int nCount)
{

	char		szName[30];

	for(int nIndex = 1; nIndex < nCount + 1;nIndex++)
	{
		// 트리컨트롤에 추가
		sprintf(szName, "Object %d",  m_nObjValue);
		m_Instruct.hParent		= m_ObjRoot;
		m_Instruct.item.pszText = szName;
		m_Instruct.item.iImage	= 1;
		m_Instruct.item.lParam	= m_nObjValue;
		m_ObjTree.InsertItem(&m_Instruct);
		m_ObjTree.Expand(m_ObjRoot, TVE_EXPAND);

		m_nObjValue++;
	}

	return true;

}

void CCtrlPanel::OnMiAddsfx() 
{
	// TODO: Add your command handler code here

	// sfx를 추가한다 
	if(m_pDoc->m_SfxMgr.NewSfx(m_nSfxValue))
	{
		AddSfx();
	}
	
}


bool CCtrlPanel::AddSfx(int nCount)
{

	char		szName[30];

	for(int nIndex = 1; nIndex < nCount + 1;nIndex++)
	{
		// 트리컨트롤에도 추가해준다
		sprintf(szName, "Sfx %d",  m_nSfxValue);
		m_Instruct.hParent		= m_SfxRoot;
		m_Instruct.item.pszText = szName;
		m_Instruct.item.iImage	= 2;
		m_Instruct.item.lParam	= m_nSfxValue;
		m_ObjTree.InsertItem(&m_Instruct);
		m_ObjTree.Expand(m_SfxRoot, TVE_EXPAND);

		m_nSfxValue++;
	}
	return true;

}


void  CCtrlPanel::DelAll()
{

	m_ObjTree.DeleteAllItems( );

	m_Instruct.hParent			= NULL;
	m_Instruct.hInsertAfter		= TVI_LAST;
	m_Instruct.item.mask		= TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM;
	m_Instruct.item.state		= 0;
	m_Instruct.item.stateMask	= 0;
	m_Instruct.item.cchTextMax	= 60;
	m_Instruct.item.cChildren	= 0;

	m_Instruct.item.pszText = "Object List";
	m_Instruct.item.iImage = 1;
	m_Instruct.item.lParam = -1;
	m_ObjRoot = m_ObjTree.InsertItem(&m_Instruct);

	m_Instruct.item.pszText = "Sfx List";
	m_Instruct.item.iImage = 2;
	m_Instruct.item.lParam = -2;
	m_SfxRoot = m_ObjTree.InsertItem(&m_Instruct);


}


// 오브젝트 삭제 
void CCtrlPanel::OnMiDelobj() 
{
	// TODO: Add your command handler code here
	m_pDoc->m_ObjMgr.RemoveObj(m_pDoc->m_Selected.nID); 
}


// 로드 애니메이션 
void CCtrlPanel::OnMiLoadanim() 
{
	// TODO: Add your command handler code here
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"ani file (*.ani)|*.ani||");
	
	dlgFile.m_ofn.lpstrInitialDir="Model\\";
	dlgFile.m_ofn.lpstrTitle=_T("load ani file");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		
		CString filename=dlgFile.GetFileName();
		filename.MakeLower();
		m_pDoc->m_ObjMgr.LoadAnim(m_nSelectedID, filename);
	}
}


// 로드 본  
void CCtrlPanel::OnMiLoadbone() 
{
	// TODO: Add your command handler code here
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"bone file (*.chr)|*.chr||");
	
	dlgFile.m_ofn.lpstrInitialDir="Model\\";
	dlgFile.m_ofn.lpstrTitle=_T("load chr file");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		
		CString filename=dlgFile.GetFileName();
		filename.MakeLower();
		m_pDoc->m_ObjMgr.LoadBone(m_nSelectedID, filename);
	}
}


// 로드 모델 - 오브젝트 
void CCtrlPanel::OnMiLoadmodel() 
{
	// TODO: Add your command handler code here
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"o3d file (*.o3d)|*.o3d||");
	
	dlgFile.m_ofn.lpstrInitialDir="Model\\";
	dlgFile.m_ofn.lpstrTitle=_T("load o3d file");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		
		CString filename=dlgFile.GetFileName();
		filename.MakeLower();
		m_pDoc->m_ObjMgr.AddPart(m_nSelectedID, filename);
	}
}


// LOD조절 - 오브젝트 
void CCtrlPanel::OnMiSetlod() 
{
	// TODO: Add your command handler code here
	
}


// SFX 삭제 
void CCtrlPanel::OnMiDelsfx() 
{
	// TODO: Add your command handler code here
	m_pDoc->m_SfxMgr.DeleteSfx(m_pDoc->m_Selected.nID); 
}


// SFX 로드 
void CCtrlPanel::OnMiLoadsfx() 
{
	// TODO: Add your command handler code here
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"sfx file (*.sfx)|*.sfx||");
	
	dlgFile.m_ofn.lpstrInitialDir="sfx\\";
	dlgFile.m_ofn.lpstrTitle=_T("load sfx file");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		CString filename=dlgFile.GetPathName();
		filename.MakeLower();
		m_pDoc->m_SfxMgr.AddSfx(m_nSelectedID, filename);
	}
}


// SFX 세팅 
void CCtrlPanel::OnMiSet() 
{
	// TODO: Add your command handler code here
	if(m_SetDlg.DoModal() == IDOK)
	{
		m_pDoc->m_SfxMgr.SetLoop(m_nSelectedID, m_SetDlg.GetIsLoop());
		m_pDoc->m_SfxMgr.SetHotKey(m_nSelectedID, m_SetDlg.GetHotKey());
		m_pDoc->m_SfxMgr.FollowObj(m_nSelectedID, m_SetDlg.GetObjNum());
	}

}

void CCtrlPanel::OnCamMode() 
{
	// TODO: Add your control notification handler code here
	// 아직 미지원 
	m_pDoc  = GetDoc();
	m_pView = GetView();

}

void CCtrlPanel::OnTexture() 
{
	// TODO: Add your control notification handler code here
	m_pDoc  = GetDoc();
	m_pView = GetView();

	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"dds file (*.dds)|*.dds||");
	
	dlgFile.m_ofn.lpstrInitialDir="..\\";
	dlgFile.m_ofn.lpstrTitle=_T("load texture");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		CString filename = dlgFile.GetPathName();
		char	*szTemp;

		filename.MakeLower();
		szTemp = (LPSTR)(LPCSTR)filename;
		m_pDoc->m_Terrain->loadTexture(szTemp);	
	}
}

void CCtrlPanel::OnChangePlay() 
{
	// TODO: Add your control notification handler code here
	m_pDoc  = GetDoc();
	m_pView = GetView();
	m_pView->m_bPlayMode = !m_pView->m_bPlayMode;
}

void CCtrlPanel::OnSelchangeMapSize() 
{
	// TODO: Add your control notification handler code here
	m_pDoc  = GetDoc();

	m_cbSize.SetCurSel(m_cbSize.GetCurSel());
	UpdateData((TRUE));
	int nSize = atoi(m_cbvSize);
	m_pDoc->m_Terrain->Resize(nSize, nSize, 4);
}

void CCtrlPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	// 시간 계산 - 최소 1시 ~ 최대 23시 
	int nTemp	= m_sLighting.GetPos();
	int nHour	= 1 + (nTemp / 12);
	int nMin	= nTemp % 12;

	
	g_GameTimer.SetTime(1, nHour, nMin, 0);
	//char text[30];
	//OutputDebugString();
	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CCtrlPanel::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	m_pDoc  = GetDoc();
	m_pView = GetView();

	switch(nChar)
	{
		case VK_SPACE :
			// 스위칭 디플로이모드 / 시뮬모드 
			m_pView->m_bPlayMode = !m_pView->m_bPlayMode;
			break;

		case VK_SUBTRACT :
			if(m_pDoc->m_Selected.nType == TYPE_OBJECT) 
				 m_pDoc->m_ObjMgr.AddFrame(m_pDoc->m_Selected.nID, -1);
			else if(m_pDoc->m_Selected.nType == TYPE_SFX)
				 m_pDoc->m_SfxMgr.AddFrame(m_pDoc->m_Selected.nID, -1);
			break;

		case VK_ADD :
			if(m_pDoc->m_Selected.nType == TYPE_OBJECT) 
				 m_pDoc->m_ObjMgr.AddFrame(m_pDoc->m_Selected.nID, 1);
			else if(m_pDoc->m_Selected.nType == TYPE_SFX)
				 m_pDoc->m_SfxMgr.AddFrame(m_pDoc->m_Selected.nID, 1);
			break;
	};

	CDialogBar::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCtrlPanel::OnBegindragObjTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
