
#ifndef __FLTREASURECHESTKEY_H__
#define __FLTREASURECHESTKEY_H__


typedef	std::vector< T_TREASURE_ITEM >		TreasureItemVec;


class FLTreasureChest
{
	public:
		FLTreasureChest();
		~FLTreasureChest();

		static FLTreasureChest*	GetInstance();


		void	OnDoUseTreasureChest( FLWSUser* pUser, const DWORD dwChestItemObjID ) const;

		bool	OnDoOpenTreasureChest( FLWSUser* pUser, const FLPacketTreasureChestOpenByKeyReq* pMsg ) const;


	private:
		bool	DoOpenTreasureChest( FLWSUser* pUser, const FLPacketTreasureChestOpenByKeyReq* pMsg ) const;

		bool	GetTreasureItem( const DWORD dwKeyItemID, TreasureItemVec & vecTreasureItem ) const;
		bool	CreateTreasureItemToInventory( FLWSUser* pUser, const TreasureItemVec & vecTreasureItem ) const;
		void	RemoveTreasureChestAndKey( FLWSUser* pUser, const DWORD dwChestItemObjID, const DWORD dwKeyItemObjID ) const;

		bool	CanUseTreasureChestItem( FLWSUser* pUser, const DWORD dwChestItemObjID ) const;
		bool	CanOpenTreasureChestByKey( FLWSUser* pUser, const FLPacketTreasureChestOpenByKeyReq* pMsg ) const;
		bool	CanCreateItemToInventory( FLWSUser* pUser, const DWORD dwKeyItemID ) const;

		bool	IsValidStateUser( FLWSUser* pUser ) const;
		bool	IsValidStateChestItem( FLWSUser* pUser, FLItemElem* pItemElem ) const;
		bool	IsValidStateKeyItem( FLWSUser* pUser, FLItemElem* pItemElem ) const;
		bool	IsMatching( const DWORD dwChestItemID, const DWORD dwKeyItemID ) const;
};

#define		g_pTreasureChest		FLTreasureChest::GetInstance()

#endif // __FLTREASURECHESTKEY_H__
