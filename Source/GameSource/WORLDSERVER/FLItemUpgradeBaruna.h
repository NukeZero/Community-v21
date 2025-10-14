
#ifndef __FLITEMUPGRADEBARUNA_H__
#define __FLITEMUPGRADEBARUNA_H__


#include "FLItemUpgradeBase.h"


class FLItemUpgradeBaruna : public FLItemUpgradeBase
{
	public:
		FLItemUpgradeBaruna();
		virtual ~FLItemUpgradeBaruna();

		static FLItemUpgradeBaruna*	GetInstance();

		bool			LoadScript( const TCHAR* pFileName );

		virtual DWORD	OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;

		virtual DWORD	OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const;


		virtual DWORD	GetAttributeDamageFactor( const FLItemElem & kItemElem ) const;
		virtual DWORD	GetAttributeDefenseFactor( const FLItemElem & kItemElem ) const;
		virtual DWORD	GetAttributeAddAtkDmgFactor( const FLItemElem & kItemElem ) const;

		virtual bool	GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const;

		virtual size_t	GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const;
		virtual size_t	GetMaxSizeAttributeEnchant( const DWORD dwItemID ) const;
		virtual size_t	GetMaxSizePiercing( const DWORD dwItemID ) const;
		virtual size_t	GetMaxSizeRandomOption( const DWORD dwItemID ) const;
#ifdef BARUNA_ULTIMATE_UPDATE
	public:	
		DWORD			OnDoUpgradeItemUltimateTrans( FLWSUser* pUser, const FLPacketItemUpgradeUltimateTransReq* pMsg ) const;
#endif


	private:
		void			Clear();


		bool			CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		bool			CanUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;
		bool			CanUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const;
		bool			CanUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const;

		bool			CanUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;

		bool			CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		bool			CanUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		bool			CanUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;


		DWORD			DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		DWORD			DoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;
		DWORD			DoUpgradeItemAttributeChange( FLWSUser* pUser, const FLPacketItemUpgradeAttributeChangeReq* pMsg ) const;
		DWORD			DoUpgradeItemAttributeRemove( FLWSUser* pUser, const FLPacketItemUpgradeAttributeRemoveReq* pMsg ) const;

		DWORD			DoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;

		DWORD			DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		DWORD			DoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		DWORD			DoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;

#ifdef BARUNA_ULTIMATE_UPDATE
		bool			CanUpgradeItemUltimateTrans( FLWSUser* pUser, const FLPacketItemUpgradeUltimateTransReq* pMsg ) const;
		DWORD			DoUpgradeItemUltimateTrans( FLWSUser* pUser, const FLPacketItemUpgradeUltimateTransReq* pMsg ) const;
		bool			LoadUltimateTransData( CLuaBase & kLua, UltimateTransDataVec & vecUltimateTransData ) const;
		const			T_ULTIMATETRANS_DATA*		GetUltimateData( const T_ITEM_SPEC* pItemSpec, const UltimateTransDataVec & vecUltimateTransData ) const;
#endif	
	private:
		EnchantDataVec				m_vecGeneralEnchantData;
		EnchantDataVec				m_vecAttributeEnchantData;

		PiercingDataVec				m_vecPiercingData;

		RandomOptionDataVec			m_vecRandomOptionData;
		DstParameterDataMap			m_mapDstParameterData;
#ifdef BARUNA_ULTIMATE_UPDATE
		UltimateTransDataVec		m_vecUltimateTransData;
#endif
};

#endif // __FLITEMUPGRADEBARUNA_H__
