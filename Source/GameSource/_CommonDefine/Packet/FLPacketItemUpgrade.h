
#ifndef	__FLPACKETITEMUPGRADE_H__
#define	__FLPACKETITEMUPGRADE_H__

#include "FLPacket.h"
#include "../_Common/FLRandomOption.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

// 일반 제련 요청
class	FLPacketItemUpgradeGeneralEnchantReq : public FLPacket
{
public:

	FLPacketItemUpgradeGeneralEnchantReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	DWORD	dwProtectionItemObjID;			// 보호 아이템
	DWORD	dwProbIncreaseItemObjID;		// 확률 증가 아이템
	bool	bSafetyUpgrade;					// 안전 제련 여부


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 속성 제련 요청
class	FLPacketItemUpgradeAttributeEnchantReq : public FLPacket
{
public:

	FLPacketItemUpgradeAttributeEnchantReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	DWORD	dwProtectionItemObjID;			// 보호 아이템
	DWORD	dwProbIncreaseItemObjID;		// 확률 증가 아이템
	bool	bSafetyUpgrade;					// 안전 제련 여부


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 속성 변경 요청
class	FLPacketItemUpgradeAttributeChangeReq : public FLPacket
{
public:

	FLPacketItemUpgradeAttributeChangeReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	BYTE	byAttribute;					// 변경 적용할 속성


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 속성 제거 요청
class	FLPacketItemUpgradeAttributeRemoveReq : public FLPacket
{
public:

	FLPacketItemUpgradeAttributeRemoveReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 피어싱 소켓 증가 요청
class	FLPacketItemUpgradePiercingSizeIncreaseReq : public FLPacket
{
public:

	FLPacketItemUpgradePiercingSizeIncreaseReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	DWORD	dwProtectionItemObjID;			// 보호 아이템
	DWORD	dwProbIncreaseItemObjID;		// 확률 증가 아이템
	bool	bSafetyUpgrade;					// 안전 제련 여부


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 피어싱 소켓 제련 요청
class	FLPacketItemUpgradePiercingItemInsertReq : public FLPacket
{
public:

	FLPacketItemUpgradePiercingItemInsertReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	DWORD	dwProtectionItemObjID;			// 보호 아이템
	DWORD	dwProbIncreaseItemObjID;		// 확률 증가 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 피어싱 소켓 옵션 제거 요청
class	FLPacketItemUpgradePiercingItemRemoveReq : public FLPacket
{
public:

	FLPacketItemUpgradePiercingItemRemoveReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	DWORD	dwProtectionItemObjID;			// 보호 아이템
	DWORD	dwProbIncreaseItemObjID;		// 확률 증가 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 각성 옵션 생성 요청
class	FLPacketItemUpgradeRandomOptionGenerateReq : public FLPacket
{
public:

	FLPacketItemUpgradeRandomOptionGenerateReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 각성 옵션 재굴림 요청
class	FLPacketItemUpgradeRandomOptionGenerateRetryReq : public FLPacket
{
public:

	FLPacketItemUpgradeRandomOptionGenerateRetryReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	BYTE	byIndex;						// 재시도 할 옵션 인덱스


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 각성 옵션 제거 요청
class	FLPacketItemUpgradeRandomOptionInitializeReq : public FLPacket
{
public:

	FLPacketItemUpgradeRandomOptionInitializeReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 착용 레벨 하락 요청
class	FLPacketItemUpgradeEquipLevelDecreaseReq : public FLPacket
{
public:

	FLPacketItemUpgradeEquipLevelDecreaseReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 착용 레벨 하락 제거 요청
class	FLPacketItemUpgradeEquipLevelInitializeReq : public FLPacket
{
public:

	FLPacketItemUpgradeEquipLevelInitializeReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 아이템 옵션 합성 요청
class	FLPacketItemUpgradeCombineReq : public FLPacket
{
public:

	FLPacketItemUpgradeCombineReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템
	DWORD	dwProtectionItemObjID;			// 보호 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 아이템 옵션 합성 제거 관련 데이터 요청
class	FLPacketItemUpgradeCombineInitializeDataReq : public FLPacket
{
public:

	FLPacketItemUpgradeCombineInitializeDataReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 아이템 옵션 합성 제거 요청
class	FLPacketItemUpgradeCombineInitializeReq : public FLPacket
{
public:

	FLPacketItemUpgradeCombineInitializeReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 외형 변경 요청
class	FLPacketItemUpgradeLooksChangeReq : public FLPacket
{
public:

	FLPacketItemUpgradeLooksChangeReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 외형 변경 제거 요청
class	FLPacketItemUpgradeLooksInitializeReq : public FLPacket
{
public:

	FLPacketItemUpgradeLooksInitializeReq();

	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;			// 재료 아이템


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

#ifdef BARUNA_ULTIMATE_UPDATE
// 바루나 -> 바루나 얼터밋 변환
class	FLPacketItemUpgradeUltimateTransReq : public FLPacket
{
public:

	FLPacketItemUpgradeUltimateTransReq();

	DWORD	dwUpgradeNpcObjID;				// 제련 정령 NPC
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID1;			// 재료 아이템1
	DWORD	dwMaterialItemObjID2;			// 재료 아이템2


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif

#ifdef CARD_UPGRADE_SYSTEM
class	FLPacketItemUpgradeCardReq : public FLPacket
{
public:
	FLPacketItemUpgradeCardReq();
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwProtectionItemObjID;				// 보호 아이템
private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif	// CARD_UPGRADE_SYSTEM

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
class	FLPacketItemUpgradeSetGemReq : public FLPacket
{
public:
	FLPacketItemUpgradeSetGemReq();
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwGemItemObjID;				// 보석 아이템
private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLPacketItemUpgradeRemoveGemReq : public FLPacket
{
public:
	FLPacketItemUpgradeRemoveGemReq();
	DWORD	dwMainItemObjID;				// 적용할 아이템
	DWORD	dwMaterialItemObjID;				// 재료 아이템
private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif

#ifdef COSTUME_UPGRADE_MIX
class	FLPacketItemUpgradeMixReq : public FLPacket
{
public:
	FLPacketItemUpgradeMixReq();
	DWORD	dwMaterialItemObjID[5];				// 재료 아이템
private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

// 일반 제련 결과 응답
class	FLSnapshotItemUpgradeGeneralEnchantResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeGeneralEnchantResultAck();

	DWORD	dwResult;			// 결과값
	bool	bSafetyUpgrade;		// 안전 제련 여부


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 속성 제련 결과 응답
class	FLSnapshotItemUpgradeAttributeEnchantResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeAttributeEnchantResultAck();

	DWORD	dwResult;			// 결과값
	bool	bSafetyUpgrade;		// 안전 제련 여부


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 속성 변경 결과 응답
class	FLSnapshotItemUpgradeAttributeChangeResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeAttributeChangeResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 속성 제거 결과 응답
class	FLSnapshotItemUpgradeAttributeRemoveResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeAttributeRemoveResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 피어싱 소켓 증가 결과 응답
class	FLSnapshotItemUpgradePiercingSizeIncreaseResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradePiercingSizeIncreaseResultAck();

	DWORD	dwResult;			// 결과값
	bool	bSafetyUpgrade;		// 안전 제련 여부


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 피어싱 소켓 제련 결과 응답
class	FLSnapshotItemUpgradePiercingItemInsertResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradePiercingItemInsertResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 피어싱 소켓 제련 제거 응답
class	FLSnapshotItemUpgradePiercingItemRemoveResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradePiercingItemRemoveResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 각성 옵션 생성 결과 응답
class	FLSnapshotItemUpgradeRandomOptionGenerateResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeRandomOptionGenerateResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 각성 옵션 재굴림 결과 응답
class	FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck();

	DWORD	dwMainItemObjID;	// 적용할 아이템
	BYTE	byIndex;			// 재시도 한 옵션 인덱스
	DWORD	dwResult;			// 결과값
	
	BYTE	byDummyIndex;								// 더미 데이터에서 재시도 한 옵션의 인덱스
	RandomOptionExtensionVec	vecDummyRandomOption;	// 재시도 한 옵션이 포함 된 더미 데이터


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 각성 옵션 제거 결과 응답
class	FLSnapshotItemUpgradeRandomOptionInitializeResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeRandomOptionInitializeResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 착용 레벨 하락 결과 응답
class	FLSnapshotItemUpgradeEquipLevelDecreaseResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeEquipLevelDecreaseResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 착용 레벨 하락 제거 결과 응답
class	FLSnapshotItemUpgradeEquipLevelInitializeResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeEquipLevelInitializeResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 아이템 옵션 합성 결과 응답
class	FLSnapshotItemUpgradeCombineResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeCombineResultAck();

public:
	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 아이템 옵션 합성 제거 데이터 응답
class	FLSnapshotItemUpgradeCombineInitializeDataAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeCombineInitializeDataAck();

	bool	bCanInitialize;			// 초기화 가능 여부
	int		nInitializePenya;		// 초기화 시 소모되는 페냐


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 아이템 옵션 합성 제거 결과 응답
class	FLSnapshotItemUpgradeCombineInitializeResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeCombineInitializeResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 외형 변경 결과 응답
class	FLSnapshotItemUpgradeLooksChangeResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeLooksChangeResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 외형 변경 제거 결과 응답
class	FLSnapshotItemUpgradeLooksInitializeResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeLooksInitializeResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

#ifdef BARUNA_ULTIMATE_UPDATE
// 바루나 -> 바루나 얼터밋 변환
class	FLSnapshotItemUpgradeUltimateTransResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeUltimateTransResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif

#ifdef CARD_UPGRADE_SYSTEM
class	FLSnapshotItemUpgradCardResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradCardResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif	// CARD_UPGRADE_SYSTEM

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
// 보석 합성 응답
class	FLSnapshotItemUpgradeSetGemResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeSetGemResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 보석 삭제 응답
class	FLSnapshotItemUpgradeRemoveGemResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeRemoveGemResultAck();

	DWORD	dwResult;			// 결과값


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif

#ifdef COSTUME_UPGRADE_MIX
// 아이템 합성 응답
class	FLSnapshotItemUpgradeMixResultAck : public FLPacket
{
public:

	FLSnapshotItemUpgradeMixResultAck();

	DWORD	dwResult;			// 결과값

	DWORD	dwResultItemObjID;	// 적용할 아이템
private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};
#endif



//////////////////////////////////////////////////////////////////////////

#endif // __FLPACKETITEMUPGRADE_H__
