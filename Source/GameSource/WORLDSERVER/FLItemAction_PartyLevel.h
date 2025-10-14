
#ifndef __FLITEMACTION_PARTYLEVEL_H__
#define __FLITEMACTION_PARTYLEVEL_H__


#include "IItemAction.h"

class FLItemAction_PartyLevelUp : public IItem_UseFunc
{
public:
	static FLItemAction_PartyLevelUp & GetInstance();

	virtual bool			Use( FLWSUser & kUser, FLItemElem & kItemElem, CAr & ar );
};

#endif // __FLITEMACTION_PARTYLEVEL_H__
