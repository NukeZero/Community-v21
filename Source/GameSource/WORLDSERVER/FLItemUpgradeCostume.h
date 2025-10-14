
#ifndef __FLITEMUPGRADECOSTUME_H__
#define __FLITEMUPGRADECOSTUME_H__


#include "FLItemUpgradeBase.h"


class FLItemUpgradeCostume : public FLItemUpgradeBase
{
	public:
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		enum { COSTUME_GEM_SUCCESS, COSTUME_GEM_FAILED, COSTUME_GEM_CANCEL, COSTUME_GEM_ISULTIMATE,
			COSTUME_GEM_INVENTORY, COSTUME_GEM_ISNOTULTIMATE };


		struct __GEMABILITYKIND
		{
			int						nAbility;
			std::vector<int>	vecAbility;
		};
		
		struct __GEMABILITYPROB
		{
			int	nAbility;
			int nProbability;
		};

		struct __PARTGEMABILITYPROB
		{
			int	nPart;
			std::vector<__GEMABILITYPROB> vecAbilityProb;
		};

		struct __MIXPROB
		{
			DWORD dwItemIndex;
			int nProbability[3];
		};
#endif

		FLItemUpgradeCostume();
		virtual ~FLItemUpgradeCostume();

		static FLItemUpgradeCostume*	GetInstance();

		bool			LoadScript( const TCHAR* pszFileName );

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		BOOL			Load_GemProbability();

		virtual DWORD	OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
#endif

#ifdef COSTUME_UPGRADE_MIX
		BOOL			Load_MixProbability();
#endif


		virtual DWORD	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const;


		virtual size_t	GetMaxSizeRandomOption( const DWORD dwItemID ) const;


	private:
		void			Clear();

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		bool			CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
#endif
		bool			CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		virtual DWORD	DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemSetGem( FLWSUser* pUser, const FLPacketItemUpgradeSetGemReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRemoveGem( FLWSUser* pUser, const FLPacketItemUpgradeRemoveGemReq* pMsg ) const;
#endif
#ifdef COSTUME_UPGRADE_MIX
		virtual DWORD	OnDoUpgradeItemMix( FLWSUser* pUser, const FLPacketItemUpgradeMixReq* pMsg, DWORD &dwItemIndex ) const;
#endif
		DWORD			DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM	
		DWORD			GetGemAbilityKindRandom( int nPart ) const;
#endif
	private:
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		EnchantDataVec				m_vecGeneralEnchantData;
#endif
		RandomOptionDataVec			m_vecRandomOptionData;
		DstParameterDataMap			m_mapDstParameterData;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		std::vector<__PARTGEMABILITYPROB>	m_vecPartGemAbilityProb;
#endif

#ifdef COSTUME_UPGRADE_MIX
		DWORD GetMixRandom( int nIndex ) const;

		std::vector<__MIXPROB>	m_vecMixProb;
#endif


};

#endif // __FLITEMUPGRADECOSTUME_H__
