
#ifndef __FLITEMUPGRADEGENERAL_H__
#define __FLITEMUPGRADEGENERAL_H__


#include "FLItemUpgradeBase.h"


class FLItemUpgradeGeneral : public FLItemUpgradeBase
{
	friend class FLItemUpgradeManager;

	public:
		FLItemUpgradeGeneral();
		virtual ~FLItemUpgradeGeneral();

//		static FLItemUpgradeGeneral*	GetInstance();

		bool			LoadScript( const TCHAR* pszFileName );

		virtual DWORD	OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const;

		virtual DWORD	GetAttributeDamageFactor( const FLItemElem & kItemElem ) const;
		virtual DWORD	GetAttributeDefenseFactor( const FLItemElem & kItemElem ) const;
		virtual DWORD	GetAttributeAddAtkDmgFactor( const FLItemElem & kItemElem ) const;

		virtual bool	GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const;
		virtual bool	GetCombineInitializeData( const FLItemElem & kItemElem, T_COMBINE_DATA & kCombineData ) const;

		virtual size_t	GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const;
		virtual size_t	GetMaxSizeAttributeEnchant( const DWORD dwItemID ) const;
		virtual size_t	GetMaxSizePiercing( const DWORD dwItemID ) const;
		virtual size_t	GetMaxSizeRandomOption( const DWORD dwItemID ) const;


	private:
		static FLItemUpgradeGeneral*	GetInstance();
		void			Clear();


		bool			CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		bool			CanUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;
		bool			CanUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const;
		bool			CanUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const;

		bool			CanUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const;
		bool			CanUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;
		bool			CanUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const;

		bool			CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		bool			CanUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		bool			CanUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;

		bool			CanUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const;
		bool			CanUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const;

#ifdef CARD_UPGRADE_SYSTEM
		bool			CanUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const;
#endif	// CARD_UPGRADE_SYSTEM

		DWORD			DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		DWORD			DoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;
		DWORD			DoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const;
		DWORD			DoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const;

		DWORD			DoUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const;
		DWORD			DoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;
		DWORD			DoUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const;

		DWORD			DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		DWORD			DoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		DWORD			DoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;

		DWORD			DoUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const;
		DWORD			DoUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const;
#ifdef CARD_UPGRADE_SYSTEM
		DWORD			DoUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const;
#endif	// CARD_UPGRADE_SYSTEM

	private:
		EnchantDataVec				m_vecGeneralEnchantData;
		EnchantDataVec				m_vecAttributeEnchantData;

		PiercingDataVec				m_vecPiercingData;

		RandomOptionDataVec			m_vecRandomOptionData;
		DstParameterDataMap			m_mapDstParameterData;

		CombineDataVec				m_vecCombineData;
#ifdef CARD_UPGRADE_SYSTEM
		CardCombineDataVec			m_vecCardCombineData;
#endif	// CARD_UPGRADE_SYSTEM
};

#endif // __FLITEMUPGRADEGENERAL_H__
