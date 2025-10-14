
#pragma once 

#include "WndPartyQuick.h"	
#include "WndPartyChangeTroup.h"


//---------------------------------------------------------------------------------------------
// 윈도우 파티 정보 클레스
//---------------------------------------------------------------------------------------------
class CWndPartyInfo : public CWndNeuz 
{ 

public:


								CWndPartyInfo(); 
								~CWndPartyInfo(); 

	virtual BOOL 				Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	
	virtual HRESULT 			RestoreDeviceObjects( void );
	virtual HRESULT 			InvalidateDeviceObjects( void );
	virtual HRESULT 			DeleteDeviceObjects( void );
	
	virtual	void 				OnInitialUpdate( void ); 
	virtual void 				OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void 				OnDraw( C2DRender* p2DRender ); 


private:

	DWORD						GetFillColorofStatus( CMover* pMember, UINT nMemberIndex );

	BOOL						CheckSummary( void );
	void						DrawStatusBox( C2DRender* p2DRender, CMover* pMember, UINT nMemberIndex, UINT nPosY );
	void						DrawMemberInfo( C2DRender* p2DRender, CMover* pMember, UINT nMemberIndex, UINT nPosY );
	void						DrawGauge( C2DRender* p2DRender, CMover* pMember, UINT nPosY );


public:


	int							m_nSelected;	// 현재 파티창에서 선택 된 인덱스 (위에서부터 zero base)

	LPDIRECT3DVERTEXBUFFER9		m_pVBGauge;

	CTexture					m_texGauEmptyNormal;
	CTexture					m_texGauFillNormal;

	CTexture*					m_pGauEmptyNormal;
	CTexture*					m_pGauFillNormal;
}; 


//---------------------------------------------------------------------------------------------
// 윈도우 파티 스킬 클레스
//---------------------------------------------------------------------------------------------
class CWndPartySkill : public CWndNeuz 
{ 
public: 
	CTexture* m_atexSkill[ 10 ];
	int m_nSkillSelect;
	
	CWndPartySkill(); 
	~CWndPartySkill(); 

	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
}; 


//---------------------------------------------------------------------------------------------
// 윈도우 파티 클레스
//---------------------------------------------------------------------------------------------
class CWndParty : public CWndNeuz 
{ 

public:


	CWndButton* 			m_pWndLeave;
	CWndButton* 			m_pWndChange;
	CWndButton* 			m_pWndTransfer;
	
	CWndButton*				m_pBtnPartyQuick;
	CWndPartyQuick*			m_pWndPartyQuick;

	
	CWndPartyInfo			m_wndPartyInfo;
	CWndPartySkill			m_wndPartySkill;
	CWndPartyChangeTroup*	m_WndPartyChangeTroup;


public: 


							CWndParty(); 
							~CWndParty(); 

	virtual BOOL 			Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void 			SerializeRegInfo( CAr& ar, DWORD& dwVersion );


	virtual	void 			OnInitialUpdate( void ); 
	virtual BOOL 			OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL 			OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void 			OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void 			OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void 			OnSize( UINT nType, int cx, int cy ); 
	virtual void 			OnDraw( C2DRender* p2DRender ); 
}; 

