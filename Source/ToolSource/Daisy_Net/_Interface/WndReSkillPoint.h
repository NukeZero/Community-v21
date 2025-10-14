#ifndef __RESKILLPOINT_AS_H
#define __RESKILLPOINT_AS_H


typedef struct RECT_INFO
{
	DWORD		m_dwID;
	CPoint		m_pt;
} RECT_INFO;

class CWndReSkillPoint : public CWndNeuz 
{ 
public: 
	LPSKILL  CheckBonusPoint();
	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	DWORD		  m_dwTotalSkillPoint;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;
	
	CWndStatic*   m_pWndFocusStatic;
	LPSKILL		  m_pFocusSkill;
	int			  m_nJob;
	DWORD		  m_dwJobSkill;
	LPSKILL       m_apSkill;
	CTexture*     m_atexSkill[ MAX_SKILL_JOB ];
	SKILL		  m_aJobSkill[ MAX_SKILL_JOB ];
	
	CWndReSkillPoint(); 
	~CWndReSkillPoint(); 
	
	int GetSkillPxp( DWORD dwID, DWORD dwLv );
	BOOL IsDownPoint( DWORD skill1, DWORD skill2 );
	void ReSetPoint();

	virtual void PutToolTip_Skill( DWORD dwSkill, CPoint point, CRect* pRect );
	virtual	HRESULT RestoreDeviceObjects();
	virtual	HRESULT InvalidateDeviceObjects();
	virtual	HRESULT DeleteDeviceObjects();
	virtual void OnDraw( C2DRender* p2DRender ); 
	LPSKILL		 GetSkillArry( DWORD dwSkill );
	BOOL		 CheckSkillList( LPSKILL pSkill );
	LPSKILL		 CheckSkillDefine( DWORD dwIndex );
	int			 SkillLevelUp( DWORD dwIndex );
	int			 SkillLevelDown( DWORD dwIndex );
	BOOL		 CheckSkill( int i );
	LPSKILL		 GetSkill( int i );
	BOOL	     GetSkillPoint(DWORD dwSkillID, CPoint& pt );
	void		 InitItem( DWORD dwTotalPoint, int nJob, LPSKILL apSkill );
}; 



class CWndReSkillPoint_As : public CWndReSkillPoint 
{ 
public: 
	RECT_INFO m_rect_info[17];
	
	CWndReSkillPoint_As(); 
	~CWndReSkillPoint_As(); 
	
	virtual void OnMouseWndSurface( CPoint point );
	virtual	BOOL Process();
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 
class CWndReSkillPoint_Me : public CWndReSkillPoint 
{ 
public: 
	RECT_INFO m_rect_info[15];

	CWndReSkillPoint_Me(); 
	~CWndReSkillPoint_Me(); 

	virtual void OnMouseWndSurface( CPoint point );
	virtual	BOOL Process();
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 
class CWndReSkillPoint_Ma : public CWndReSkillPoint 
{ 
public: 
	RECT_INFO m_rect_info[17];

	CWndReSkillPoint_Ma(); 
	~CWndReSkillPoint_Ma(); 
	
	virtual void OnMouseWndSurface( CPoint point );
	virtual	BOOL Process();
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 




class CWndReSkillPoint_Text : public CWndNeuz 
{ 
public: 
	CWndReSkillPoint_Text(); 
	~CWndReSkillPoint_Text(); 
	
	virtual	void OnInitialUpdate(); 
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 




#endif
