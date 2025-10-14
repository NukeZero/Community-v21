// Ctrl.h: interface for the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
#define AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <D3D9.h>
#include <basetsd.h> 
#include "Obj.h"
#include "ProjectCmn.h"


struct		CtrlProp;
class		CWorld;
class       CSfx;
class		CShip;

class CCtrl : public CObj
{
public:
	CCtrl();
	virtual ~CCtrl();
private:
	CShip*			m_pIAObjLink;		// this가 어느IA오브젝트에 연결이 되었는가. 나중에 CShip*은 CIAObj* 형으로 대체되어야 한다.
	
protected:
	OBJID			m_objid;			// 객체 번호 
	D3DXMATRIX		m_mInvTM;			// m_matWorld의 역행렬. pIAObjLink와 곱해서 로컬행렬을 얻음.
	LONG			m_lRespawn;
#ifdef __V050322_CACHEQUEST
	int	            m_nRespawnType;
#endif
public:

	virtual BOOL	GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );
	int		TargetSelecter( CCtrl *pTarget );
	int		ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );			// 발사형태의 스킬의 발사체 생성
	void	CreateSkillSfx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );		// 스킬시전시 클라에서 생성되어야 할 sfx
	int		ApplyDamage( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) ;
	BOOL	IsPossibleApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void	ApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bTarget = TRUE );		// 타겟하나당 실제 적용되어야 하는 효과
	int		DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, int nParam = 0, BOOL bOnlyDmg = FALSE );		// 스킬을 타겟중심으로 적용
	void	ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, CCtrl* pCenter = NULL );	// vPos를 중심으로 스킬을 적용
	void	ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE);		// 스킬을 this를 중심으로 주변에 적용.
	void	ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProp = false );
	int		ApplySkillAroundTroupe( u_long idParty, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false );		// idParty 파티 대상으로 스킬효과를 적용.
	BOOL	ApplySkillHardCoding( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void	DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb = false );		// ActiveSkill발동
			
	void	CreateYoyoSkill( CSfx* pSfx, CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
		
	virtual int		SendDamage( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return 0; }
	virtual int		SendDamageForce( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE ) { return 0; }	// 강공격
	CShip*			GetIAObjLink()			{ return m_pIAObjLink; }
	void			SetIAObjLink( CShip *pIA );
	void			SetId( OBJID objid )	{ m_objid = objid;  }
	OBJID			GetId()					{ return m_objid;   }
	D3DXMATRIX*		GetInvTM()				{ return &m_mInvTM; }
	LONG			GetRespawn()			{ return m_lRespawn; }

//{{AFX
	void			SetRespawn( LONG n )	{ m_lRespawn = n; }	
//}}AFX
	virtual void	AddItToGlobalId();
	virtual	void	RemoveItFromGlobalId();
	virtual void	Process(FLOAT fElapsedTime);
	virtual void	Execute();
	virtual void	Render(LPDIRECT3DDEVICE9 pd3dDevice);
	virtual BOOL	Read( CFileIO* pFile );
	virtual BOOL	Write( CFileIO* pFile );
	virtual	void	Serialize( CAr & ar );

private:
	int				DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg );

};


#endif // !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)