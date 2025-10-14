#ifndef __SKILLTREE_H
#define __SKILLTREE_H

#ifdef __11_MONTH_JOB_PRO

class CWndSkillTree : public CWndNeuz 
{ 
public: 
	int			  m_nJob;
	DWORD		  m_dwJobSkill;
	LPSKILL       m_apSkill;
	CTexture*     m_atexSkill[ MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL];
	
	CWndSkillTree(); 
	~CWndSkillTree(); 

	LPSKILL GetSkill( int i );
	void SetExpert( int nExpert );
	void SetPro( int nJob );
	void InitItem( int nJob, LPSKILL apExpertSkill );
	BOOL CheckSkill( int i );
	BOOL	     GetSkillPoint(DWORD dwSkillID, CPoint& pt );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#endif //__11_MONTH_JOB_PRO


#endif
