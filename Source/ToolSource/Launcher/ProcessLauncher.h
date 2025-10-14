// ProcessLauncher.h: interface for the CProcessLauncher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSLAUNCHER_H__9625E009_4A1F_4DFA_9B59_FE6D9DA886D3__INCLUDED_)
#define AFX_PROCESSLAUNCHER_H__9625E009_4A1F_4DFA_9B59_FE6D9DA886D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPeer;

class CProcessLauncher  
{
public:
	CProcessLauncher();
	virtual ~CProcessLauncher();

protected:
	CPeer*	m_pPeer;

	char	m_lpAddr[16];
	char	m_lpApplicationName[MAX_PATH];
	char	m_lpCurrentDirectory[MAX_PATH];
	char	m_lpDownDirectory[MAX_PATH];
	DWORD	m_dwId;
	DWORD	m_dwMilliseconds;	
	bool	m_bStartWait;	
	HANDLE	m_hEvent;
	LONG	m_lProcessCount;

	HANDLE	m_hMonitor;					// 모니터 쓰레드 
	PROCESS_INFORMATION		m_pi;

public:
	void		Run();
	BOOL		Init();

	BYTE		IsProcessRunning(); 
	LPCTSTR		GetAddr()         { return m_lpAddr; }
	LPCTSTR		GetAppName()      { return m_lpApplicationName; }
	LPCTSTR		GetAppDirectory() { return m_lpCurrentDirectory; }
	DWORD		GetAppID()		  { return m_dwId; }	
	HANDLE		GetEvent()		  { return m_hEvent; }
	bool		GetStartWait()	  { return m_bStartWait; }

	void		BeginMonitor();
	void		MonitorThread();
	bool		CreateMyProcess();
	void		TerminateMyProcess();
	void		OnTerminated();
};

#endif // !defined(AFX_PROCESSLAUNCHER_H__9625E009_4A1F_4DFA_9B59_FE6D9DA886D3__INCLUDED_)
