
#include "StdAfx.h"
#include "FLPacketItemUpgrade.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketItemUpgradeGeneralEnchantReq::FLPacketItemUpgradeGeneralEnchantReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_GENERAL_ENCHANT_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID			= NULL_ID;
	dwProtectionItemObjID		= NULL_ID;
	dwProbIncreaseItemObjID		= NULL_ID;
	bSafetyUpgrade				= false;
}

bool	FLPacketItemUpgradeGeneralEnchantReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << dwProtectionItemObjID;
	kAr << dwProbIncreaseItemObjID;
	kAr << bSafetyUpgrade;
	
	return true;
}

bool	FLPacketItemUpgradeGeneralEnchantReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> dwProtectionItemObjID;
	kAr >> dwProbIncreaseItemObjID;
	kAr >> bSafetyUpgrade;
	
	return true;
}

FLPacketItemUpgradeAttributeEnchantReq::FLPacketItemUpgradeAttributeEnchantReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_ATTRIBUTE_ENCHANT_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID			= NULL_ID;
	dwProtectionItemObjID		= NULL_ID;
	dwProbIncreaseItemObjID		= NULL_ID;
	bSafetyUpgrade				= false;
}

bool	FLPacketItemUpgradeAttributeEnchantReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << dwProtectionItemObjID;
	kAr << dwProbIncreaseItemObjID;
	kAr << bSafetyUpgrade;

	return true;
}

bool	FLPacketItemUpgradeAttributeEnchantReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> dwProtectionItemObjID;
	kAr >> dwProbIncreaseItemObjID;
	kAr >> bSafetyUpgrade;

	return true;
}

FLPacketItemUpgradeAttributeChangeReq::FLPacketItemUpgradeAttributeChangeReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_ATTRIBUTE_CHANGE_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID			= NULL_ID;
	byAttribute					= SAI79::NO_PROP;
}

bool	FLPacketItemUpgradeAttributeChangeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << byAttribute;

	return true;
}

bool	FLPacketItemUpgradeAttributeChangeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> byAttribute;

	return true;
}

FLPacketItemUpgradeAttributeRemoveReq::FLPacketItemUpgradeAttributeRemoveReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_ATTRIBUTE_REMOVE_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
}

bool	FLPacketItemUpgradeAttributeRemoveReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;

	return true;
}

bool	FLPacketItemUpgradeAttributeRemoveReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;

	return true;
}

FLPacketItemUpgradePiercingSizeIncreaseReq::FLPacketItemUpgradePiercingSizeIncreaseReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_PIERCING_INCREASE_SIZE_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID			= NULL_ID;
	dwProtectionItemObjID		= NULL_ID;
	dwProbIncreaseItemObjID		= NULL_ID;
	bSafetyUpgrade				= false;
}

bool	FLPacketItemUpgradePiercingSizeIncreaseReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << dwProtectionItemObjID;
	kAr << dwProbIncreaseItemObjID;
	kAr << bSafetyUpgrade;

	return true;
}

bool	FLPacketItemUpgradePiercingSizeIncreaseReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> dwProtectionItemObjID;
	kAr >> dwProbIncreaseItemObjID;
	kAr >> bSafetyUpgrade;

	return true;
}

FLPacketItemUpgradePiercingItemInsertReq::FLPacketItemUpgradePiercingItemInsertReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_PIERCING_INSERT_ITEM_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID			= NULL_ID;
	dwProtectionItemObjID		= NULL_ID;
	dwProbIncreaseItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradePiercingItemInsertReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << dwProtectionItemObjID;
	kAr << dwProbIncreaseItemObjID;

	return true;
}

bool	FLPacketItemUpgradePiercingItemInsertReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> dwProtectionItemObjID;
	kAr >> dwProbIncreaseItemObjID;

	return true;
}

FLPacketItemUpgradePiercingItemRemoveReq::FLPacketItemUpgradePiercingItemRemoveReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_PIERCING_REMOVE_ITEM_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID			= NULL_ID;
	dwProtectionItemObjID		= NULL_ID;
	dwProbIncreaseItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradePiercingItemRemoveReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << dwProtectionItemObjID;
	kAr << dwProbIncreaseItemObjID;

	return true;
}

bool	FLPacketItemUpgradePiercingItemRemoveReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> dwProtectionItemObjID;
	kAr >> dwProbIncreaseItemObjID;

	return true;
}

FLPacketItemUpgradeRandomOptionGenerateReq::FLPacketItemUpgradeRandomOptionGenerateReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_GENERATE_REQ )
{
	dwUpgradeNpcObjID		= NULL_ID;
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradeRandomOptionGenerateReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;

	return true;
}

bool	FLPacketItemUpgradeRandomOptionGenerateReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;

	return true;
}

FLPacketItemUpgradeRandomOptionGenerateRetryReq::FLPacketItemUpgradeRandomOptionGenerateRetryReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_GENERATE_RETRY_REQ )
{
	dwUpgradeNpcObjID		= NULL_ID;
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
	byIndex					= 0;
}

bool	FLPacketItemUpgradeRandomOptionGenerateRetryReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << byIndex;

	return true;
}

bool	FLPacketItemUpgradeRandomOptionGenerateRetryReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> byIndex;

	return true;
}

FLPacketItemUpgradeRandomOptionInitializeReq::FLPacketItemUpgradeRandomOptionInitializeReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_INITIALIZE_REQ )
{
	dwUpgradeNpcObjID		= NULL_ID;
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradeRandomOptionInitializeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;

	return true;
}

bool	FLPacketItemUpgradeRandomOptionInitializeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;

	return true;
}

FLPacketItemUpgradeEquipLevelDecreaseReq::FLPacketItemUpgradeEquipLevelDecreaseReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_EQUIP_LEVEL_DECREASE_REQ )
{
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradeEquipLevelDecreaseReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;

	return true;
}

bool	FLPacketItemUpgradeEquipLevelDecreaseReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;

	return true;
}

FLPacketItemUpgradeEquipLevelInitializeReq::FLPacketItemUpgradeEquipLevelInitializeReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_EQUIP_LEVEL_INITIALIZE_REQ )
{
	dwMainItemObjID			= NULL_ID;
}

bool	FLPacketItemUpgradeEquipLevelInitializeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;

	return true;
}

bool	FLPacketItemUpgradeEquipLevelInitializeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;

	return true;
}

FLPacketItemUpgradeCombineReq::FLPacketItemUpgradeCombineReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_COMBINE_REQ )
{
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
	dwProtectionItemObjID	= NULL_ID;
}

bool	FLPacketItemUpgradeCombineReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	kAr << dwProtectionItemObjID;

	return true;
}

bool	FLPacketItemUpgradeCombineReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	kAr >> dwProtectionItemObjID;

	return true;
}

FLPacketItemUpgradeCombineInitializeDataReq::FLPacketItemUpgradeCombineInitializeDataReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_COMBINE_INITIALIZE_DATA_REQ )
{
	dwMainItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradeCombineInitializeDataReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;

	return true;
}

bool	FLPacketItemUpgradeCombineInitializeDataReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;

	return true;
}

FLPacketItemUpgradeCombineInitializeReq::FLPacketItemUpgradeCombineInitializeReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_COMBINE_INITIALIZE_REQ )
{
	dwMainItemObjID			= NULL_ID;
}

bool	FLPacketItemUpgradeCombineInitializeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;

	return true;
}

bool	FLPacketItemUpgradeCombineInitializeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;

	return true;
}

FLPacketItemUpgradeLooksChangeReq::FLPacketItemUpgradeLooksChangeReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_LOOKS_CHANGE_REQ )
{
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradeLooksChangeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;

	return true;
}

bool	FLPacketItemUpgradeLooksChangeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;

	return true;
}

FLPacketItemUpgradeLooksInitializeReq::FLPacketItemUpgradeLooksInitializeReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_LOOKS_INITIALIZE_REQ )
{
	dwMainItemObjID			= NULL_ID;
	dwMaterialItemObjID		= NULL_ID;
}

bool	FLPacketItemUpgradeLooksInitializeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;

	return true;
}

bool	FLPacketItemUpgradeLooksInitializeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;

	return true;
}

#ifdef BARUNA_ULTIMATE_UPDATE
FLPacketItemUpgradeUltimateTransReq::FLPacketItemUpgradeUltimateTransReq() :
FLPacket( PACKETTYPE_BARUNAULTIMATE_TRANSWEAPON_REQ )
{
	dwUpgradeNpcObjID			= NULL_ID;
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID1		= NULL_ID;
	dwMaterialItemObjID2		= NULL_ID;

}

bool	FLPacketItemUpgradeUltimateTransReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwUpgradeNpcObjID;
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID1;
	kAr << dwMaterialItemObjID2;
	
	return true;
}

bool	FLPacketItemUpgradeUltimateTransReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwUpgradeNpcObjID;
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID1;
	kAr >> dwMaterialItemObjID2;
	
	return true;
}
#endif	// BARUNA_ULTIMATE_UPDATE

#ifdef CARD_UPGRADE_SYSTEM
FLPacketItemUpgradeCardReq::FLPacketItemUpgradeCardReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_CARD_REQ )
{
	dwMainItemObjID				= NULL_ID;
	dwProtectionItemObjID				= NULL_ID;
}

bool	FLPacketItemUpgradeCardReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwProtectionItemObjID;
	
	return true;
}

bool	FLPacketItemUpgradeCardReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwProtectionItemObjID;
	return true;
}
#endif	// CARD_UPGRADE_SYSTEM

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
FLPacketItemUpgradeSetGemReq::FLPacketItemUpgradeSetGemReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_SETGEM_REQ )
{
	dwMainItemObjID				= NULL_ID;
	dwGemItemObjID				= NULL_ID;
}

bool	FLPacketItemUpgradeSetGemReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwGemItemObjID;
	
	return true;
}

bool	FLPacketItemUpgradeSetGemReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwGemItemObjID;
	return true;
}

FLPacketItemUpgradeRemoveGemReq::FLPacketItemUpgradeRemoveGemReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_REMOVEGEM_REQ )
{
	dwMainItemObjID				= NULL_ID;
	dwMaterialItemObjID				= NULL_ID;
}

bool	FLPacketItemUpgradeRemoveGemReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << dwMaterialItemObjID;
	
	return true;
}

bool	FLPacketItemUpgradeRemoveGemReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> dwMaterialItemObjID;
	return true;
}
#endif

#ifdef COSTUME_UPGRADE_MIX
FLPacketItemUpgradeMixReq::FLPacketItemUpgradeMixReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_MIX_REQ )
{
	for( int i = 0; i < 5; i++ )
		dwMaterialItemObjID[i]				= NULL_ID;
}

bool	FLPacketItemUpgradeMixReq::BodySerialize( CAr& kAr )	const
{
	for( int i = 0; i < 5; i++ )
		kAr << dwMaterialItemObjID[i];
	return true;
}

bool	FLPacketItemUpgradeMixReq::BodyDeserialize( CAr& kAr )
{
	for( int i = 0; i < 5; i++ )
		kAr >> dwMaterialItemObjID[i];
	return true;
}
#endif


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotItemUpgradeGeneralEnchantResultAck::FLSnapshotItemUpgradeGeneralEnchantResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_GENERAL_ENCHANT_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
	bSafetyUpgrade	= false;
}

bool	FLSnapshotItemUpgradeGeneralEnchantResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;
	kAr << bSafetyUpgrade;

	return true;
}

bool	FLSnapshotItemUpgradeGeneralEnchantResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;
	kAr >> bSafetyUpgrade;

	return true;
}

FLSnapshotItemUpgradeAttributeEnchantResultAck::FLSnapshotItemUpgradeAttributeEnchantResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_ATTRIBUTE_ENCHANT_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
	bSafetyUpgrade	= false;
}

bool	FLSnapshotItemUpgradeAttributeEnchantResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;
	kAr << bSafetyUpgrade;

	return true;
}

bool	FLSnapshotItemUpgradeAttributeEnchantResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;
	kAr >> bSafetyUpgrade;

	return true;
}

FLSnapshotItemUpgradeAttributeChangeResultAck::FLSnapshotItemUpgradeAttributeChangeResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_ATTRIBUTE_CHANGE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeAttributeChangeResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeAttributeChangeResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeAttributeRemoveResultAck::FLSnapshotItemUpgradeAttributeRemoveResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_ATTRIBUTE_REMOVE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeAttributeRemoveResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeAttributeRemoveResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradePiercingSizeIncreaseResultAck::FLSnapshotItemUpgradePiercingSizeIncreaseResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_PIERCING_SIZE_INCREASE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
	bSafetyUpgrade	= false;
}

bool	FLSnapshotItemUpgradePiercingSizeIncreaseResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;
	kAr << bSafetyUpgrade;

	return true;
}

bool	FLSnapshotItemUpgradePiercingSizeIncreaseResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;
	kAr >> bSafetyUpgrade;

	return true;
}

FLSnapshotItemUpgradePiercingItemInsertResultAck::FLSnapshotItemUpgradePiercingItemInsertResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_PIERCING_ITEM_INSERT_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradePiercingItemInsertResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradePiercingItemInsertResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradePiercingItemRemoveResultAck::FLSnapshotItemUpgradePiercingItemRemoveResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_PIERCING_ITEM_REMOVE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradePiercingItemRemoveResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradePiercingItemRemoveResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeRandomOptionGenerateResultAck::FLSnapshotItemUpgradeRandomOptionGenerateResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_RANDOM_OPTION_GENERATE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeRandomOptionGenerateResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeRandomOptionGenerateResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck::FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_RANDOM_OPTION_GENERATE_RETRY_RESULT_ACK )
{
	dwMainItemObjID	= NULL_ID;
	byIndex			= 0;
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
	byDummyIndex	= 0;
	vecDummyRandomOption.clear();
}

bool	FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainItemObjID;
	kAr << byIndex;
	kAr << dwResult;
	kAr << byDummyIndex;

	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	for( RandomOptionExtensionCItr pos = vecDummyRandomOption.begin(); pos != vecDummyRandomOption.end(); ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );

	return true;
}

bool	FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainItemObjID;
	kAr >> byIndex;
	kAr >> dwResult;
	kAr >> byDummyIndex;

	BYTE byCount = 0;
	kAr >> byCount;

	for( int i = 0; i < byCount; ++i )
	{
		T_RANDOMOPTION_EXT kRandomOption;
		kAr >> kRandomOption;
		vecDummyRandomOption.push_back( kRandomOption );
	}

	return true;
}

FLSnapshotItemUpgradeRandomOptionInitializeResultAck::FLSnapshotItemUpgradeRandomOptionInitializeResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_RANDOM_OPTION_INITIALIZE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeRandomOptionInitializeResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeRandomOptionInitializeResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeEquipLevelDecreaseResultAck::FLSnapshotItemUpgradeEquipLevelDecreaseResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_EQUIP_LEVEL_DECREASE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeEquipLevelDecreaseResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeEquipLevelDecreaseResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeEquipLevelInitializeResultAck::FLSnapshotItemUpgradeEquipLevelInitializeResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_EQUIP_LEVEL_INITIALIZE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeEquipLevelInitializeResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeEquipLevelInitializeResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeCombineResultAck::FLSnapshotItemUpgradeCombineResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_COMBINE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeCombineResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeCombineResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeCombineInitializeDataAck::FLSnapshotItemUpgradeCombineInitializeDataAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_COMBINE_INITIALIZE_DATA_ACK )
{
	bCanInitialize		= false;
	nInitializePenya	= 0;
}

bool	FLSnapshotItemUpgradeCombineInitializeDataAck::BodySerialize( CAr& kAr )	const
{
	kAr << bCanInitialize;
	kAr << nInitializePenya;

	return true;
}

bool	FLSnapshotItemUpgradeCombineInitializeDataAck::BodyDeserialize( CAr& kAr )
{
	kAr >> bCanInitialize;
	kAr >> nInitializePenya;

	return true;
}

FLSnapshotItemUpgradeCombineInitializeResultAck::FLSnapshotItemUpgradeCombineInitializeResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_COMBINE_INITIALIZE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeCombineInitializeResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeCombineInitializeResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeLooksChangeResultAck::FLSnapshotItemUpgradeLooksChangeResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_LOOKS_CHANGE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeLooksChangeResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeLooksChangeResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

FLSnapshotItemUpgradeLooksInitializeResultAck::FLSnapshotItemUpgradeLooksInitializeResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_LOOKS_INITIALIZE_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeLooksInitializeResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeLooksInitializeResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}

#ifdef BARUNA_ULTIMATE_UPDATE
FLSnapshotItemUpgradeUltimateTransResultAck::FLSnapshotItemUpgradeUltimateTransResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_ULTIMATE_TRANS_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeUltimateTransResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradeUltimateTransResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}
#endif

#ifdef CARD_UPGRADE_SYSTEM
FLSnapshotItemUpgradCardResultAck::FLSnapshotItemUpgradCardResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_CARD_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradCardResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;

	return true;
}

bool	FLSnapshotItemUpgradCardResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;

	return true;
}
#endif	// CARD_UPGRADE_SYSTEM

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
FLSnapshotItemUpgradeSetGemResultAck::FLSnapshotItemUpgradeSetGemResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_SETGEM_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeSetGemResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;
	return true;
}

bool	FLSnapshotItemUpgradeSetGemResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;
	return true;
}



FLSnapshotItemUpgradeRemoveGemResultAck::FLSnapshotItemUpgradeRemoveGemResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_REMOVEGEM_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
}

bool	FLSnapshotItemUpgradeRemoveGemResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;
	return true;
}

bool	FLSnapshotItemUpgradeRemoveGemResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;
	return true;
}
#endif

#ifdef COSTUME_UPGRADE_MIX
FLSnapshotItemUpgradeMixResultAck::FLSnapshotItemUpgradeMixResultAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_MIX_RESULT_ACK )
{
	dwResult		= FSC_ITEMUPGRADE_SYSTEM_ERROR;
	dwResultItemObjID = NULL_ID;
}

bool	FLSnapshotItemUpgradeMixResultAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwResult;
	kAr << dwResultItemObjID;
	return true;
}

bool	FLSnapshotItemUpgradeMixResultAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwResult;
	kAr >> dwResultItemObjID;
	return true;
}
#endif


//////////////////////////////////////////////////////////////////////////
