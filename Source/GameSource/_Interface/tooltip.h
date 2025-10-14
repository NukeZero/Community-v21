

#pragma once


enum 
{
	FIRST_TT,
	SECOND_TT,
	MAX_TT
};


//---------------------------------------------------------------------------------------------
// 툴팁 클레스
//---------------------------------------------------------------------------------------------
class CToolTip
{

public:


						CToolTip();
						~CToolTip();

	void 				InitTexture( void );
	void 				Paint( C2DRender* p2DRender );
	void 				Process( CPoint pt, C2DRender* p2DRender );
	void 				Delete( void );

	void 				CancelToolTip( void );

	void 				PutToolTip(		DWORD dwToolTipId, CString&		string,		CRect rect,	CPoint pt, int nToolTipPos = 0 );
	void 				PutToolTip(		DWORD dwToolTipId, LPCTSTR		lpszString,	CRect rect,	CPoint pt, int nToolTipPos = 0 );	
	void 				PutToolTip(		DWORD dwToolTipId, CEditString& string,		CRect rect,	CPoint pt, int nToolTipPos = 0 );
	void 				PutToolTipEx(	DWORD dwToolTipId, CEditString& string,		CRect rect,	CPoint pt, int nToolTipPos = 0, int nSubToolTipFlag = 0 );

	void 				SetEnable( BOOL bEnable )						{	m_bEnable = bEnable;		}
	void 				SetPosition( int nPos )							{	m_nPosition = nPos;			}
	void 				SetVal_m_nAdded_( int val )						{	m_nAdded = val;				}
	void				SetRenderRect( const CRect& rectRender );
	void 				SetUltimateToolTip(FLItemBase* pItemBase);		//sun: 9차 전승관련 Clienet
	void				SetSubToolTipNumber( int nSubToolTipNumber );


	BOOL 				GetEnable( void )								{	return m_bEnable;			}
	BOOL				GetReadyToolTipSwitch( void ) const;
	int					GetSubToolTipNumber( void ) const;
	const CPoint&		GetPointToolTip( void ) const;
	const CRect&		GetRevisedRect( void ) const;
	const CRect&		GetRenderRect( void ) const;
	const CRect&		GetRect( void ) const;

#ifndef __IMPROVE_MAP_SYSTEM

	void				SetWorldMapMonsterInfo(int nMonCnt, DWORD* pDwMonId);	//sun: 13, WorldMap 몬스터 표시

#else

	void				InsertMonsterID( DWORD dwMonsterID );
	void				ResizeMapMonsterToolTip( void );

#endif 


private:


	void				ResetMapMonsterInfo( void );
	CPoint				GetStartPoint( void );
	CRect				AdjustRenderRect( C2DRender* p2DRender, CPoint& ptTooltip );


private:

	DWORD       		m_dwToolTipId;

	int         		m_nAlpha;
	int         		m_nPosition;
	int					m_nSubToolTipFlag;
	int					m_nSubToolTipNumber;

	BOOL        		m_bReadyToopTip;
	BOOL        		m_bPutToolTip;
	BOOL        		m_bToolTip;
	BOOL        		m_bEnable;

	CEditString 		m_strToolTip;
	CTimer      		m_timerToopTip;

	CPoint      		m_ptToolTip;
	CPoint      		m_ptOldToolTip;

	CRect       		m_rect;
	CRect       		m_rectRender;
	CRect				m_nRevisedRect;

	CTexture			m_apTextureToolTip[ MAX_TT * 9 ];

	//sun: 9차 전승관련 Clienet
	int					m_nAdded;
	int					m_nAddedJewel[5];
	int					m_nSlot;
	FLItemBase*			m_pUltimateItemBase;

	//sun: 12, 무기 피어싱
	CTexture*			m_pUltimateTexture;
	CTexture*			m_pJewelBgTexture;

#ifdef __ENCHANT_BARUNA16

	CTexture*			m_pBarunaTexture;

#endif 

#ifndef __IMPROVE_MAP_SYSTEM

	//sun: 13, WorldMap 몬스터 표시
	int					m_nMonInfoCnt;
	DWORD				m_pDwMonId[5];

#else

	std::vector< DWORD >		m_vecMapMonsterID;

#endif 

};
