#pragma once

class CFLWndCancellation : public CWndNeuz
{
public:
	CFLWndCancellation();
	~CFLWndCancellation();
	
	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent, DWORD dwWndId);
	virtual BOOL OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnDraw( C2DRender* p2DRender );
	void ReceivedPacket( PT_PACKET_RESET_BIND  ptBind );
	
protected:
	CRect	m_rcSlot;
	OBJID	m_objID;

private:
	FLItemElem* m_pSlotItem; //귀속 해제 할 아이템
	FLItemElem* m_pScrItem;  //귀속 해제 스크롤 
	void ItemClear();

};