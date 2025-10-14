// ProfileViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProfileView.h"
#include "ProfileViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileViewDlg dialog

CProfileViewDlg::CProfileViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProfileViewDlg::IDD, pParent), m_hMapFile(NULL), m_hUpdateThread(NULL), m_bSuspend(FALSE)
{
	//{{AFX_DATA_INIT(CProfileViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CProfileViewDlg::~CProfileViewDlg()
{
	if(m_hMapFile != NULL)
		CloseHandle(m_hMapFile);

	if(m_hUpdateThread != NULL)
		CloseHandle(m_hUpdateThread);

	m_vecSpPfNodeTreeItem.clear();
}

void CProfileViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ColumnTreeCtrl);
	//{{AFX_DATA_MAP(CProfileViewDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProfileViewDlg, CDialog)
	//{{AFX_MSG_MAP(CProfileViewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnPauseResume)
	ON_BN_CLICKED(IDC_BUTTON2, OnClear)
	ON_BN_CLICKED(IDC_BUTTON3, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileViewDlg message handlers

BOOL CProfileViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	DWORD dwThreadId;
	m_hUpdateThread = chBEGINTHREADEX( NULL, 0, CProfileViewDlg::UpdateThread, this, 0, &dwThreadId );

	UINT uTreeStyle = TVS_HASLINES|TVS_LINESATROOT;
	m_ColumnTreeCtrl.GetTreeCtrl().ModifyStyle(0,uTreeStyle);

	m_ColumnTreeCtrl.InsertColumn(0, _T("Profile Name"), LVCFMT_CENTER, 310);
	m_ColumnTreeCtrl.InsertColumn(1, _T("Usages"), LVCFMT_CENTER, 130);
	m_ColumnTreeCtrl.InsertColumn(2, _T("%"), LVCFMT_RIGHT, 60);
	m_ColumnTreeCtrl.InsertColumn(3, _T("TotalTimes"), LVCFMT_CENTER, 110);
	m_ColumnTreeCtrl.InsertColumn(4, _T("Calls"), LVCFMT_CENTER, 60);
	m_ColumnTreeCtrl.InsertColumn(5, _T("TimesByCall"), LVCFMT_CENTER, 110);
	m_ColumnTreeCtrl.InsertColumn(6, _T("Recur."), LVCFMT_CENTER, 60);

	// Set Progress Dummy
	HTREEITEM hTreeItem;

	m_ColumnTreeCtrl.ShowWindow(SW_HIDE);
	for(int i=0; i<40; i++)
	{
		hTreeItem = m_ColumnTreeCtrl.GetTreeCtrl().InsertItem(_T("0"), 0, 0);
		m_ColumnTreeCtrl.SetItemProgress(hTreeItem, 1, 0);
	}

	m_ColumnTreeCtrl.CleanAll();
	m_ColumnTreeCtrl.ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProfileViewDlg::UpdateThread(LPVOID pVoid)
{
	while(TRUE)
	{
		Sleep(1500);
		((CProfileViewDlg*)pVoid)->UpdateInfo();
	}
}

void CProfileViewDlg::UpdateInfo()
{
	if(m_hMapFile != NULL && !m_bSuspend)
	{
		m_ColumnTreeCtrl.ShowTreeWindow(SW_HIDE);

		BYTE* pBuffer = NULL;
		pBuffer = (BYTE*)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_FILEMAP_BYTE);
		
		if(pBuffer != NULL)
		{
			int nProfileCount = 0;
			BYTE* pTempBuffer = pBuffer;
			CopyMemory(&nProfileCount, pTempBuffer, sizeof(int));

			if(nProfileCount > 0)
			{
				m_vecSpPfNodeTreeItem.clear();
				m_ColumnTreeCtrl.CleanAll();

				pTempBuffer += sizeof(int);
				for(int i=0; i<nProfileCount; i++)
				{
					PfNodeTreeItem* pPfNodeTree = new PfNodeTreeItem;
					CopyMemory(&pPfNodeTree->m_pfNode, pTempBuffer, sizeof(ProfileNode));

					if(pPfNodeTree->m_pfNode.m_nParentId == 0) // Root
					{
						pPfNodeTree->m_htItem = m_ColumnTreeCtrl.GetTreeCtrl().InsertItem(pPfNodeTree->m_pfNode.m_chrName, 0, 0);
					}
					else // Node
					{
						HTREEITEM hParentItem = GetParentHtreeItem(pPfNodeTree->m_pfNode.m_nParentId);
						if(hParentItem != NULL)
						{
							pPfNodeTree->m_htItem = m_ColumnTreeCtrl.GetTreeCtrl().InsertItem(pPfNodeTree->m_pfNode.m_chrName, 0, 0, hParentItem);
							m_ColumnTreeCtrl.GetTreeCtrl().Expand(hParentItem, TVE_EXPAND);
						}
					}

					if(pPfNodeTree->m_htItem != NULL)
					{
						CString strTemp;
						strTemp.Format("%.2f%%", pPfNodeTree->m_pfNode.m_fPercent);
						m_ColumnTreeCtrl.SetItemText(pPfNodeTree->m_htItem, 2, strTemp);
						strTemp.Format("%.3f ms", pPfNodeTree->m_pfNode.m_fTotalTime * 1000);
						m_ColumnTreeCtrl.SetItemText(pPfNodeTree->m_htItem, 3, strTemp);
						strTemp.Format("%d", pPfNodeTree->m_pfNode.m_nTotalCalls);
						m_ColumnTreeCtrl.SetItemText(pPfNodeTree->m_htItem, 4, strTemp);
						strTemp.Format("%.3f ms/call", (pPfNodeTree->m_pfNode.m_fTotalTime * 1000) / static_cast<float>(pPfNodeTree->m_pfNode.m_nTotalCalls));
						m_ColumnTreeCtrl.SetItemText(pPfNodeTree->m_htItem, 5, strTemp);
						strTemp.Format("%d", pPfNodeTree->m_pfNode.m_nRecursionCount);
						m_ColumnTreeCtrl.SetItemText(pPfNodeTree->m_htItem, 6, strTemp);
					}

					// Progress Bar표시는 Root, Node 상관없이 순서대로 오는 것으로 가정한다. (그래서 트리에 확장/축소는 없다.)
					m_ColumnTreeCtrl.SetProgressPercent(i, static_cast<int>(pPfNodeTree->m_pfNode.m_fPercent));

					m_vecSpPfNodeTreeItem.push_back(SpPfNodeTreeItem(pPfNodeTree));
					pTempBuffer += sizeof(ProfileNode);
				}
			}
		}

		m_ColumnTreeCtrl.ShowTreeWindow(SW_SHOW);
	}
}

HTREEITEM CProfileViewDlg::GetParentHtreeItem(int nParentId)
{
	HTREEITEM rtnHTreeItem = NULL;
	BOOL bFind = FALSE;

	vector<SpPfNodeTreeItem>::iterator iter;
	for(iter=m_vecSpPfNodeTreeItem.begin(); iter!=m_vecSpPfNodeTreeItem.end() && !bFind; iter++)
	{
		if((*iter)->m_pfNode.m_nNodeId == nParentId)
		{
			rtnHTreeItem = (*iter)->m_htItem;
			bFind = TRUE;
		}
	}

	return rtnHTreeItem;
}

void CProfileViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProfileViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProfileViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProfileViewDlg::OnPauseResume() 
{
	if(m_hUpdateThread != NULL && m_hMapFile != NULL)
	{
		if(m_bSuspend)
		{
			ResumeThread(m_hUpdateThread);
			m_bSuspend = FALSE;
			GetDlgItem(IDC_BUTTON1)->SetWindowText("Pause");
		}
		else
		{
			SuspendThread(m_hUpdateThread);
			m_bSuspend = TRUE;
			GetDlgItem(IDC_BUTTON1)->SetWindowText("Resume");
		}
	}
}

void CProfileViewDlg::OnClear() 
{
	m_vecSpPfNodeTreeItem.clear();
	m_ColumnTreeCtrl.CleanAll();

	if(m_hMapFile != NULL)
		CloseHandle(m_hMapFile);
}

void CProfileViewDlg::OnStart() 
{
	CString strMapFileName;
	CComboBox* pWndCombo = (CComboBox*)GetDlgItem(IDC_COMBO2);

	if(pWndCombo)
	{
		pWndCombo->GetWindowText(strMapFileName);
		m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, strMapFileName);

		if(m_hMapFile == NULL)
		{
			m_vecSpPfNodeTreeItem.clear();
			m_ColumnTreeCtrl.CleanAll();
		}
		else
		{
			if(pWndCombo->FindString(0, strMapFileName) == -1)
				pWndCombo->AddString(strMapFileName);
		}
	}
}
