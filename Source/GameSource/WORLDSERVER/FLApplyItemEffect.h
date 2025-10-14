#ifndef __FLAPPLYITEMEFFECT_H__
#define __FLAPPLYITEMEFFECT_H__


class FLApplyItemEffect
{
	public:
		static FLApplyItemEffect* GetInstance();

		FLApplyItemEffect();
		~FLApplyItemEffect();


		void	DoApplyHitActiveSkill( CMover* pSrc, CMover* pDest, const DWORD dwProb );
		void	DoApplyDamageActiveSkill( CMover* pSrc, CMover* pDest, const DWORD dwProb );
		void	DoApplyEquipActiveSkill( CMover* pSrc );

		void	RefreshAllEquipActiveSkill( CMover* pSrc );

		void	RemoveAllEquipActiveSkill( CMover* pSrc );
		void	RemoveEquipActiveSkill( CMover* pSrc, FLItemElem* pItemElem );
		void	RemoveEquipActiveSkillPiercing( CMover* pSrc, FLItemElem* pItemElem );


		int		GetAdjValueDuplicationBuff( FLWSUser* pUser, DWORD dwItemKind3, bool bRemove = false );
//		int		GetAdjValueSingularBuff( FLWSUser* pUser, DWORD dwItemKind3 );
		int		GetAdjValueGeneralEnchantRateBuff( FLWSUser* pUser, FLItemElem* pTargetItem, DWORD dwItemKind3, bool bRemove = false );
		int		GetAdjValueAttributeEnchantRateBuff( FLWSUser* pUser, FLItemElem* pTargetItem, DWORD dwItemKind3, bool bRemove = false );


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		void	DoApplyEffect( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp );

		void	ApplyEffect( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp );
		BOOL	ApplyParameter( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp, BOOL bSend = TRUE, int nDamage = 0 );

		void	SetDestParameter( CMover* pMover, int nIdx, PT_ITEM_SPEC pItemProp, BOOL bSend = TRUE );
		void	ResetDestParameter( CMover* pMover, int nIdx, PT_ITEM_SPEC pItemProp, BOOL bSend = TRUE );

		void	SendDamageAroundPos( const D3DXVECTOR3 *pvPos, int nDmgType, CMover* pAttacker, int nApplyType, int nAttackID, float fRange );
		void	SendDamageAroundTarget( int nDmgType, CMover* pAttacker, CMover* pTarget, int nApplyType, int nAttackID, float fRange, float fTargetRatio );
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	private:
		void	DoApplyHitActiveSkillPiercing( CMover* pSrc, CMover* pDest, FLItemElem* pItemElem, const DWORD dwProb );
		void	DoApplyDamageActiveSkillPiercing( CMover* pSrc, CMover* pDest, FLItemElem* pItemElem, const DWORD dwProb );
		void	DoApplyEquipActiveSkillPiercing( CMover* pSrc, FLItemElem* pItemElem );

};


#define		g_xApplyItemEffect		FLApplyItemEffect::GetInstance()


#endif // __FLAPPLYITEMEFFECT_H__

