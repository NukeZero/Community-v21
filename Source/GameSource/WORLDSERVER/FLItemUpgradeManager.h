
#ifndef __FLITEMUPGRADEMANAGER_H__
#define __FLITEMUPGRADEMANAGER_H__


#include "FLItemUpgradeDataStruct.h"


class FLItemUpgradeBase;

//////////////////////////////////////////////////////////////////////////
class FLPacketItemUpgradeGeneralEnchantReq;
class FLPacketItemUpgradeAttributeEnchantReq;
class FLPacketItemUpgradeAttributeChangeReq;
class FLPacketItemUpgradeAttributeRemoveReq;
class FLPacketItemUpgradePiercingSizeIncreaseReq;
class FLPacketItemUpgradePiercingItemInsertReq;
class FLPacketItemUpgradePiercingItemRemoveReq;
class FLPacketItemUpgradeRandomOptionGenerateReq;
class FLPacketItemUpgradeRandomOptionGenerateRetryReq;
class FLPacketItemUpgradeRandomOptionInitializeReq;
class FLPacketItemUpgradeEquipLevelDecreaseReq;
class FLPacketItemUpgradeEquipLevelInitializeReq;
class FLPacketItemUpgradeCombineReq;
class FLPacketItemUpgradeCombineInitializeDataReq;
class FLPacketItemUpgradeCombineInitializeReq;
class FLPacketItemUpgradeLooksChangeReq;
class FLPacketItemUpgradeLooksInitializeReq;
class FLPacketItemUpgradeCardReq;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
class FLPacketItemUpgradeSetGemReq;
class FLPacketItemUpgradeRemoveGemReq;
#endif
#ifdef COSTUME_UPGRADE_MIX
class FLPacketItemUpgradeMixReq;
#endif
//////////////////////////////////////////////////////////////////////////


// struct T_ITEM_UPGRADE_HANDLER_KEY
// {
// 	T_ITEM_UPGRADE_HANDLER_KEY( const DWORD ItemKind1, const DWORD ItemKind2, const DWORD ItemKind3, const DWORD ItemGrade )
// 		: dwItemKind1( ItemKind1 ), dwItemKind2( ItemKind2 ), dwItemKind3( ItemKind3 ), dwItemGrade( ItemGrade )
// 	{
// 	}
// 
// 	bool	operator	== ( const T_ITEM_UPGRADE_HANDLER_KEY & rhs ) const
// 	{
// 		return dwItemKind1 == rhs.dwItemKind1 && dwItemKind2 == rhs.dwItemKind2 && dwItemKind3 == rhs.dwItemKind3 && dwItemGrade == rhs.dwItemGrade;
// 	}
// 
// 	bool	operator	<	( const T_ITEM_UPGRADE_HANDLER_KEY & rhs ) const
// 	{
// 		return ( dwItemKind1 != rhs.dwItemKind1 )
// 			? ( dwItemKind1 < rhs.dwItemKind1 )	: ( dwItemKind2 != rhs.dwItemKind2 )
// 				? ( dwItemKind2 < rhs.dwItemKind2 )	: ( dwItemKind3 != rhs.dwItemKind3 )
// 					? ( dwItemKind3 < rhs.dwItemKind3 )	: ( dwItemGrade < rhs.dwItemGrade );
// 	}
// 
// 	private:
// 		const DWORD		dwItemKind1;
// 		const DWORD		dwItemKind2;
// 		const DWORD		dwItemKind3;
// 		const DWORD		dwItemGrade;
// };
// 
// typedef std::map< T_ITEM_UPGRADE_HANDLER_KEY, FLItemUpgradeBase* >		ItemUpgradeHandlerMap;


class FLItemUpgradeManager
{
	public:
		static FLItemUpgradeManager* GetInstance();

		FLItemUpgradeManager();
		~FLItemUpgradeManager();

		void	Load();

		void	OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		void	OnDoUpgradeItemAttributeEnchant( FLWSUser* pUser, const FLPacketItemUpgradeAttributeEnchantReq* pMsg ) const;

		void	OnDoUpgradeItemPiercingSizeIncrease( FLWSUser* pUser, const FLPacketItemUpgradePiercingSizeIncreaseReq* pMsg ) const;
		void	OnDoUpgradeItemPiercingItemInsert( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemInsertReq* pMsg ) const;
		void	OnDoUpgradeItemPiercingItemRemove( FLWSUser* pUser, const FLPacketItemUpgradePiercingItemRemoveReq* pMsg ) const;

		void	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		void	OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		void	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;

		void	OnDoUpgradeItemEquipLevelDecrease( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelDecreaseReq* pMsg ) const;
		void	OnDoUpgradeItemEquipLevelInitialize( FLWSUser* pUser, const FLPacketItemUpgradeEquipLevelInitializeReq* pMsg ) const;

		void	OnDoUpgradeItemCombine( FLWSUser* pUser, const FLPacketItemUpgradeCombineReq* pMsg ) const;
		void	OnDoUpgradeItemCombineInitialize( FLWSUser* pUser, const FLPacketItemUpgradeCombineInitializeReq* pMsg ) const;

		void	OnDoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const;
		void	OnDoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const;

		void	OnDoUpgradeItemCard( FLWSUser* pUser, const FLPacketItemUpgradeCardReq* pMsg ) const;

 		DWORD	GetAttributeDamageFactor( const FLItemElem & kItemElem ) const;
 		DWORD	GetAttributeDefenseFactor( const FLItemElem & kItemElem ) const;
 		DWORD	GetAttributeAddAtkDmgFactor( const FLItemElem & kItemElem ) const;

		bool	GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const;
		bool	GetCombineInitializeData( const FLItemElem & kItemElem, T_COMBINE_DATA & kCombineData ) const;

		size_t	GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const;
		size_t	GetMaxSizeAttributeEnchant( const DWORD dwItemID ) const;
		size_t	GetMaxSizePiercing( const DWORD dwItemID ) const;
		size_t	GetMaxSizeRandomOption( const DWORD dwItemID ) const;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		void	OnDoUpgradeItemSetGem( FLWSUser* pUser, const FLPacketItemUpgradeSetGemReq* pMsg ) const;
		void	OnDoUpgradeItemRemoveGem( FLWSUser* pUser, const FLPacketItemUpgradeRemoveGemReq* pMsg ) const;
#endif
#ifdef COSTUME_UPGRADE_MIX
		void	OnDoUpgradeItemMix( FLWSUser* pUser, const FLPacketItemUpgradeMixReq* pMsg ) const;
#endif
	private:
		const FLItemUpgradeBase*	GetItemUpgradeType( const T_ITEM_SPEC* pItemProp ) const;


	private:
//		ItemUpgradeHandlerMap		m_mapItemUpgradeHandler;
};

#define		g_pItemUpgradeManager	FLItemUpgradeManager::GetInstance()

#endif // __FLITEMUPGRADEMANAGER_H__
