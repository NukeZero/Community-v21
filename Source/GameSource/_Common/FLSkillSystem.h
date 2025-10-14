
#ifndef	__FLSKILLSYSTEM_H__
#define	__FLSKILLSYSTEM_H__


class CMover;

const DWORD		MIN_SKILL_USE_LEVEL		= 1;

class	FLSkillSystem
{
public:

	static	FLSkillSystem*	GetInstance();

private:

	FLSkillSystem();
	~FLSkillSystem();

public:
	//////////////////////////////////////////////////////////////////////////
	bool	CanUseSkill( CMover* pUseMover, const DWORD dwSkillID, const DWORD dwSkillLevel, const OBJID idFocusObj, const SKILLUSETYPE sutType );
	bool	IsRemainSkillDelayTime( CMover* pUseMover, const DWORD dwSkillID ) const;
	//////////////////////////////////////////////////////////////////////////

	DWORD	DoUseSkill( CMover* pUseMover, const DWORD dwSkill, const int nLevel, const OBJID idFocusObj, const SKILLUSETYPE sutType = SUT_NORMAL, const BOOL bControl = FALSE, const int nCastingTime = 0 );		// dwSkill/nLevel만 가지고도 사용할 수 있는 버전.
	void	DoActiveSkill( CCtrl* pUseCtrl, DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProp = false, BOOL bControl = FALSE );		// ActiveSkill발동

	BOOL	OnMeleeSkill( CMover* pUseMover, int nType, int nCount = 1 );		// OBJMSG_ATK_MELEESKILL실행후 타점이 되었을때 발생하는 이벤트.
	BOOL	OnMagicSkill( CMover* pUseMover, int nType, int nCount = 1 );		// OBJMSG_ATK_MAGICSKILL실행후 타점이 되었을때 발생하는 이벤트.
	void	OnEndSkillState( CMover* pTarget, DWORD dwSkill, DWORD dwLevel );

	int		DoApplySkill( CCtrl* pUseCtrl, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bControl = FALSE );		// 스킬을 타겟중심으로 적용

	void	ApplySkillAround( CCtrl* pUseCtrl, CCtrl* pTarget, int nApplyType, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, FLOAT fRangeCustom = 0.0f, BOOL bControl = FALSE );		// 스킬을 this를 중심으로 주변에 적용.
	void	ApplySkillRegion( CCtrl* pUseCtrl, const D3DXVECTOR3 &vPos, int nApplyType, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, CCtrl* pCenter = NULL, BOOL bControl = FALSE );	// vPos를 중심으로 스킬을 적용

	void	ApplySkill( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bTarget = TRUE );		// 타겟하나당 실제 적용되어야 하는 효과

	BOOL	ApplyParam( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend = TRUE, int nDamage = 0 );	// this에게 파라메터를 적용. pSrc는 시전자.
	BOOL	ApplyActiveParam( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend = TRUE, int nDamage = 0 );	// this에게 파라메터를 적용. pSrc는 시전자.

private:

	int		DoApplySkillEx( CCtrl* pUseCtrl, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg, BOOL bControl );

	void	ShootSkill( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp* pSkillProp, AddSkillProp* pAddSkillProp );			// 발사형태의 스킬의 발사체 생성

	void	ApplySkillAroundTroupe( CCtrl* pUseCtrl, u_long idParty, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false );		// idParty 파티 대상으로 스킬효과를 적용.
	void	ApplySkillLine( CCtrl* pUseCtrl, int nApplyType, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProp = false, BOOL bControl = FALSE );

	int		ApplyDamage( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget );
	BOOL	ApplySkillHardCoding( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp );


	BOOL	DoActiveParam( CCtrl* pUseCtrl, CCtrl* pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp, int nIndex, BOOL bSend = TRUE, int nDamage = 0 );	// this에게 파라메터를 적용. pSrc는 시전자.

	bool	CanApplyBuff( CMover* pSrcMover, CMover* pTargetMover, SkillProp* pSkillProp );

#ifdef __WORLDSERVER
	void	ApplyHeal( CMover* pHealer, CMover* pTarget, int nHealValue, DWORD dwChgParamVal, DWORD dwExeTarget );
#endif

#ifdef	__CLIENT
	void	CreateSkillSfx( CCtrl* pUseCtrl, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp );		// 스킬시전시 클라에서 생성되어야 할 sfx
	void	CreateYoyoSkill( CCtrl* pUseCtrl, CSfx* pSfx, CCtrl *pTarget, SkillProp *pSkillProp, AddSkillProp *pAddSkillProp );
#endif


};


#define	g_cSkillSystem	FLSkillSystem::GetInstance()







#endif