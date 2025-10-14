
#ifndef __FLITEMUPGRADEEATPET_H__
#define __FLITEMUPGRADEEATPET_H__


#include "FLItemUpgradeBase.h"


class FLItemUpgradeEatPet : public FLItemUpgradeBase
{
	public:
		FLItemUpgradeEatPet();
		virtual ~FLItemUpgradeEatPet();

		static FLItemUpgradeEatPet*	GetInstance();

		bool			LoadScript( const TCHAR* pszFileName );

		virtual DWORD	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;


		virtual size_t	GetMaxSizeRandomOption( const DWORD dwItemID ) const;


	private:
		void			Clear();


		bool			CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;


		DWORD			DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;


	private:
		RandomOptionDataVec			m_vecRandomOptionData;
		DstParameterDataMap			m_mapDstParameterData;

};

#endif // __FLITEMUPGRADEEATPET_H__
