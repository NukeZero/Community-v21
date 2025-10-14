#pragma once

class FLWUser;
class FLItemElem;

//ret : 사용/적용 갯수

class IItem_UseFunc
{
public:
	virtual bool			Use( FLWSUser & kUser, FLItemElem & io_kUseItem, CAr & ar )= 0;		
};

class IItem_ApplyFunc
{
public:
	virtual bool			Apply( FLWSUser & kUser, FLItemElem & io_kUseItem, FLItemElem & io_kDestItem, CAr & ar )	= 0;
};

//기간 만료
//삭제 // 사용하면 삭제되는 타입 : 아닌타입	//	bPermanence
//장착해야 쓸수 있는 아이템

