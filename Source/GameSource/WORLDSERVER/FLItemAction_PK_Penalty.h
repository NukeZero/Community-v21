
#ifndef __FLITEMACTION_PKPENALTY_H__
#define __FLITEMACTION_PKPENALTY_H__


#include "IItemAction.h"

class FLItemAction_PK_Penalty : public IItem_UseFunc
{
public:
	static FLItemAction_PK_Penalty & GetInstance();

	virtual bool			Use( FLWSUser & kUser, FLItemElem & kItemElem, CAr & ar );
};

#endif // __FLITEMACTION_PKPENALTY_H__
