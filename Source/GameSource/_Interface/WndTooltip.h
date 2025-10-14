
//Date : 20100729
//Author : gmpbigsun
//Note : toolip with NeuzWnd

#pragma  once 

#ifdef __HYPERLINK_ITEM16
class CWndTooltip : public CWndNeuz
{
public:
	CWndTooltip( );
	virtual ~CWndTooltip( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();

	virtual BOOL Process ();

	static CWndTooltip* _GetThis( );
	BOOL ShowTooltip( FLItemBase* pItemBase );

protected:
	BOOL InitTexture( );
	BOOL UpdateData( FLItemBase* pItemBase );			//창 크기 및 위치 재조정
	void CheckExtraInfo( FLItemBase* pItemBase );		//울티메이트 및 피어싱등을 체크한다.
	void ResetData( );

protected:
	CEditString m_strToolTip;
	CRect       m_rectRender;
	CRect		m_rectIcon;
	
	CTexture*	m_pIconTexture;
	CTexture*	m_pUltimateTexture;
	CTexture*	m_pJewelBgTexture;

#ifdef __ENCHANT_BARUNA16
	CTexture* m_pBarunaTexture;
#endif //__ENCHANT_BARUNA16

	int			m_nSlot;
	int			m_nAdded;
	int			m_nAddedJewel[5];
};

_inline BOOL _ShowTooltip_LinkedItem( FLItemBase* pItemBase )
{
	return CWndTooltip::_GetThis()->ShowTooltip( pItemBase );
}

#endif //__HYPERLINK_ITEM16