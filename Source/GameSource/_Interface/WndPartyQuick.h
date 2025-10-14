
#pragma once

class CWndParty;

#define	MAX_PARTY_MEMBER	8

//---------------------------------------------------------------------------------------------
// 윈도우 파티 퀵 클레스
//---------------------------------------------------------------------------------------------
class CWndPartyQuick : public CWndNeuz 
{ 

public: 

								CWndPartyQuick(); 
								~CWndPartyQuick(); 
	
	virtual BOOL				Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	void						SetActiveMember( int MemberNum );

	virtual HRESULT				RestoreDeviceObjects( void );
	virtual HRESULT				InvalidateDeviceObjects( void );
	virtual HRESULT				DeleteDeviceObjects( void );	


	virtual	void 				OnInitialUpdate( void ); 
	virtual BOOL 				OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void 				OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void 				OnDraw( C2DRender* p2DRender ); 


public:


	LPDIRECT3DVERTEXBUFFER9		m_pVBGauge;
	int							m_MemberCount;

	CWndParty*					m_pWndParty;
	CMover*						m_pFocusMember;	

	CWndButton*					m_pBtnParty;
	CWndStatic*					m_pWndMemberStatic[MAX_PARTY_MEMBER];

	CTexture					m_texGauEmptyNormal;
	CTexture					m_texGauFillNormal;

	CTexture*					m_pGauEmptyNormal;
	CTexture*					m_pGauFillNormal;
}; 

