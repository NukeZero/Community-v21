// ProfileViewDlg.h : header file
//
#include "ColumnTreeCtrl.h"
#include "Process.h"
#include <map>

using namespace std;

#if !defined(AFX_PROFILEVIEWDLG_H__F7750B8C_D206_4439_8C94_47D934E03F7A__INCLUDED_)
#define AFX_PROFILEVIEWDLG_H__F7750B8C_D206_4439_8C94_47D934E03F7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
   pvParam, dwCreateFlags, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                          \
         (void *)        (psa),                         \
         (unsigned)      (cbStackSize),                 \
         (PTHREAD_START) (pfnStartAddr),                \
         (void *)        (pvParam),                     \
         (unsigned)      (dwCreateFlags),               \
         (unsigned *)    (pdwThreadId)))

#define MAX_FILEMAP_BYTE 13000
#define MAX_PROFILE_NAME 40

typedef struct ProfileNode
{
	int m_nTotalCalls;
	int m_nRecursionCount;
	int m_nNodeId;
	int m_nParentId;
	float m_fPercent;

	float m_fTotalTime;
	__int64 m_i64StartTime;
	
	char m_chrName[MAX_PROFILE_NAME];

	ProfileNode() : m_nTotalCalls(0), m_nRecursionCount(0), m_nNodeId(-1), m_nParentId(-1), m_fPercent(0.0f), m_fTotalTime(0.0f), m_i64StartTime(0) {}
} PROFILENODE, *LPPROFILENODE;

typedef struct PfNodeTreeItem
{
	ProfileNode m_pfNode;
	HTREEITEM m_htItem;

	PfNodeTreeItem() : m_htItem(NULL) {}
} PFNODETREEITEM, *LPPFNODETREEITEM;

typedef	boost::shared_ptr<PFNODETREEITEM> SpPfNodeTreeItem;
typedef	vector<SpPfNodeTreeItem> VecSpPfNodeTreeItem;

/////////////////////////////////////////////////////////////////////////////
// CProfileViewDlg dialog

class CProfileViewDlg : public CDialog
{
// Construction
public:
	CProfileViewDlg(CWnd* pParent = NULL);	// standard constructor
	~CProfileViewDlg();

// Dialog Data
	//{{AFX_DATA(CProfileViewDlg)
	enum { IDD = IDD_PROFILEVIEW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
private:
	CColumnTreeCtrl m_ColumnTreeCtrl;
	VecSpPfNodeTreeItem m_vecSpPfNodeTreeItem;
	HANDLE m_hMapFile;
	HANDLE m_hUpdateThread;
	BOOL m_bSuspend;

public:
	static void UpdateThread(LPVOID pVoid);
	void UpdateInfo();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProfileViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPauseResume();
	afx_msg void OnClear();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM GetParentHtreeItem(int nParentId);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFILEVIEWDLG_H__F7750B8C_D206_4439_8C94_47D934E03F7A__INCLUDED_)
