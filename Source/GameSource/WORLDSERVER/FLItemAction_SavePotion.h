#pragma once

#include "IItemAction.h"

class FLItemAction_SavePotionStorage : public IItem_UseFunc
{
public:
	static FLItemAction_SavePotionStorage & GetInstance();

	virtual bool			Use( FLWSUser & kUser, FLItemElem & io_kUseItem, CAr & ar );
};

class FLItemAction_SavePotionCharge : public IItem_ApplyFunc
{
public:
	static FLItemAction_SavePotionCharge & GetInstance();

	virtual bool			Apply( FLWSUser & kUser, FLItemElem & io_kUseItem, FLItemElem & io_kDestItem, CAr & ar );
};


class FLItemAction_SavePotionKey : public IItem_ApplyFunc
{
public:
	static FLItemAction_SavePotionKey & GetInstance();

	virtual bool			Apply( FLWSUser & kUser, FLItemElem & io_kUseItem, FLItemElem & io_kDestItem, CAr & ar );
};
