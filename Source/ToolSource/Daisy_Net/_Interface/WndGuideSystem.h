// GuideSystem.h: interface for the CGuideSystem class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __WNDGUIDESYSTEM__H
#define __WNDGUIDESYSTEM__H

#ifdef __YGUIDESYSTEM0923

typedef struct GUIDE_STRUCT
{
	CString m_str;
	int     m_nkey;
	int     m_nEventMsg;
	BOOL    m_bBeginner;
	BOOL    m_bFlag;
	BOOL    m_bIsClear;
	int		m_nShowLevel;
} GUIDE_STRUCT;

class CWndGuideTextMgr : public CWndNeuz 
{ 
	void _SetGuideText( GUIDE_STRUCT guide );

public: 
	CString		m_strHelpKey;
	CTexture*	m_pTextureBG;
	CRect       m_Rect[4];
	int						  m_nCurrentVector;

	vector<GUIDE_STRUCT>	  m_VecGuideText;

	GUIDE_STRUCT GetGuideText()
	{
		return m_VecGuideText[m_nCurrentVector];
	}

	void AddGuideText( GUIDE_STRUCT guide );
	CWndGuideTextMgr(); 
	~CWndGuideTextMgr(); 
	
	void UpDate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	BOOL OnEraseBkgnd( C2DRender* p2DRender );
}; 

class CWndGuideSelection : public CWndNeuz 
{
public:
	CWndGuideSelection();
	virtual ~CWndGuideSelection();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
};


class CWndGuideSystem : public CWndNeuz 
{
public:
	BOOL				 m_bIsLoad;
	enum { ANI_INTRO = 0, ANI_IDLE, ANI_BYTE };
	enum { KEY = 0, STR };
	DWORD				 m_dwGuideLevel;
	DWORD				 m_dwTime;
	BYTE				 m_bAniState;
	
	list<int>			 m_listGuideMsg;
	list<GUIDE_STRUCT>   m_listGuide;
	list<GUIDE_STRUCT*>  m_listGuideChart;
	
	CModelObject*		 m_pModel;
	CWndGuideTextMgr*    m_wndGuideText;
	CWndGuideSelection*  m_wndGuideSelection;
	
	CWndMenu			m_wndMenuPlace;
	
	
public:
	void SetAni(int nJob, int nAniKind);
	void ChangeModel( int nJob );
	void CreateGuideSelection();
	void GuideStart(BOOL ischart);
	void PushBack( GUIDE_STRUCT guide );
	BOOL	m_bIsGuideChart[2];

	CWndGuideSystem();
	virtual ~CWndGuideSystem();

	void		 SendGuideMessage( int nMsg );
	BOOL		 LoadGuide( LPCTSTR lpszFileName );
		
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL Process();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
};











#endif //__YGUIDESYSTEM0923

#endif // __WNDGUIDESYSTEM__H
