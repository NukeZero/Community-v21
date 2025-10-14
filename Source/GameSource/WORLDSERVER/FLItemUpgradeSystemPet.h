
#ifndef __FLITEMUPGRADESYSTEMPET_H__
#define __FLITEMUPGRADESYSTEMPET_H__


#include "FLItemUpgradeBase.h"


class FLItemUpgradeSystemPet : public FLItemUpgradeBase
{
	public:
		FLItemUpgradeSystemPet();
		virtual ~FLItemUpgradeSystemPet();

		static FLItemUpgradeSystemPet*	GetInstance();

		bool			LoadScript( const TCHAR* pszFileName );

		virtual DWORD	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		virtual DWORD	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;


		virtual bool	GetRandomOptionDummy( const FLItemElem & kItemElem, const BYTE byIndex, RandomOptionExtensionVec & vecDummyRandomOption, BYTE & byDummyIndex ) const;

		virtual size_t	GetMaxSizeRandomOption( const DWORD dwItemID ) const;


	private:
		void			Clear();


		bool			CanUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		bool			CanUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;


		DWORD			DoUpgradeItemRandomOptionGenerate( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionGenerateRetry( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionGenerateRetryReq* pMsg ) const;
		DWORD			DoUpgradeItemRandomOptionInitialize( FLWSUser* pUser, const FLPacketItemUpgradeRandomOptionInitializeReq* pMsg ) const;


	private:
		RandomOptionDataVec			m_vecRandomOptionData;
		DstParameterDataMap			m_mapDstParameterData;

};

#endif // __FLITEMUPGRADESYSTEMPET_H__
