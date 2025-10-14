#ifndef __APPDEFINE_H__
#define __APPDEFINE_H__

#include "ResData.h"           // 윈도 고유 ID 모음 

#ifdef __Y0402						
const UINT APP_SHOP_ =  APP_SHOPEX;
#else // __Y0402
const UINT APP_SHOP_ =  APP_SHOP;
#endif // __Y0402

#ifdef __BUYITEM_METHOD_CHANGED0603
const  UINT APP_CONFIRM_BUY_ = APP_CONFIRM_BUYEX;
#else // __Y0402
const  UINT APP_CONFIRM_BUY_ = APP_CONFIRM_BUY;
#endif // __Y0402


#ifdef __RENDER_SERVERINFO0608
const  UINT APP_MESSENGER_ = APP_MESSENGER;
//	const  UINT APP_MESSENGER_ = APP_MESSENGER_EX;
#else // __Y0402
const  UINT APP_MESSENGER_ = APP_MESSENGER;
#endif // __Y0402


#endif