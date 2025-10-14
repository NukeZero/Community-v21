

#include "StdAfx.h"
#include "WndFontColorManager.h"


//---------------------------------------------------------------------------------------------
// »ý¼ºÀÚ 
//---------------------------------------------------------------------------------------------
CWndFontColorManager::CWndFontColorManager()
{
}


//---------------------------------------------------------------------------------------------
// ÆÄ±«ÀÚ 
//---------------------------------------------------------------------------------------------
CWndFontColorManager::~CWndFontColorManager()
{
}


//---------------------------------------------------------------------------------------------
// Get Instance  ( °´Ã¼ ¾ò±â )
// param	: void
// return	: CWndFontColorManager* 
//---------------------------------------------------------------------------------------------
CWndFontColorManager* CWndFontColorManager::GetInstance()
{
	static CWndFontColorManager WndFontColorManager;
	return &WndFontColorManager;
}

