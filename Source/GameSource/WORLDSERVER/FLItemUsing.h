
#ifndef	__FLITEMUSING_H__
#define	__FLITEMUSING_H__


class	FLItemUsing
{
public:

	static FLItemUsing*	GetInstance();

	void	HandleDoUseItem( FLWSUser* pUser, CAr & ar );

	void	OnDoUseItem( FLWSUser* pUser, DWORD dwData, OBJID objid, int nPart );

	BOOL	DoUseItem( FLWSUser* pUser, DWORD dwItemId, DWORD dwFocusId, int nPart = -1 );

	BOOL	DoUseEquipmentItem( FLWSUser* pUser, FLItemElem* pItemElem, DWORD dwId, int nPart );
	BOOL	DoUseItemBlinkWing( FLWSUser* pUser, PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem, BOOL bUes = TRUE );

private:

	FLItemUsing();
	~FLItemUsing();

	BOOL	DoUseGenericItem( FLWSUser* pUser, FLItemElem* pItemElem, DWORD dwID, int nPart );

	void	DoUsePackItem( FLWSUser* pUser, FLItemElem* pItemElem, FLPackItemProp::_T_ELEM * pPackItemElem );
	BOOL	DoUseGiftbox( FLWSUser* pUser, FLItemElem* pItemElem, DWORD dwItemId );
	void	DoUseEveFBOX( FLWSUser* pUser, FLItemElem* pItemElem );
	BOOL	DoUseItemWarp( FLWSUser* pUser, PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem );
	int		DoUseItemSystem( FLWSUser* pUser, PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem, int nPart = -1 );
	int		DoUseItemInput( FLWSUser* pUser, PT_ITEM_SPEC pProp, FLItemElem* pItem );
	BOOL	DoUseItemSexChange( FLWSUser* pUser, int nFace );

	BOOL	DoUseItemFood( FLWSUser* pUser, PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem );
	BOOL	DoUseItemFood_SM( FLWSUser* pUser, PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem );
	void	DoUseSystemPet( FLWSUser* pUser, FLItemElem* pItemElem );
	void	DoUseEatPet( FLWSUser* pUser, FLItemElem* PitemElem );
	void	DoUseSummonNPC( FLWSUser* pUser, FLItemElem & ItemElem );

	void	OnAfterUseItem( FLWSUser* pUser, const PT_ITEM_SPEC pItemProp );

	int		DoUseItemPetTonic( FLWSUser* pUser, FLItemElem* pItemElem );	// 펫 영양제 사용
	int		DoUseItemFeedPocket( FLWSUser* pUser, FLItemElem* pItemElem );

	int		DoUseItemPetNaming( FLWSUser* pUser );

public:

	BOOL	CheckUseItem( FLWSUser* pUser, DWORD dwItemObjId );
	void	CompleteUseItem( FLWSUser* pUser, DWORD dwId );

	bool	canUseTeleportation( FLWSUser* pUser ) const;

	static bool				UnbindPeriodLock( FLWSUser & kUser, FLItemElem & kItemElem );
};


#define	g_pItemUsing	FLItemUsing::GetInstance()




#endif