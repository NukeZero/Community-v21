#ifndef __WNDSELECTCH__H
#define __WNDSELECTCH__H

class CWndSelectCh : public CWndNeuz 
{ 
public: 
	CWndSelectCh(); 
	~CWndSelectCh(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void ReceivedPacket( FLSnapshotChargeZoneTicketEnteranceNoti* pkOpenTicketEnterance);


private:
	void InitializeTicketInformation();

	SpecChargeZoneWorldMap		m_mapTicket;
	SpecChargeZoneWorldItr      m_itr_TicketInfo;
	CWndListBox					*m_pWndListBox_Channel;
	CWndListBox					*m_pWndListBox_World;
	DWORD						m_dwItemId;
	std::vector<DWORD>				m_vecWorldID;
}; 

#endif
