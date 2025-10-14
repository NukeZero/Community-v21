

#include "StdAfx.h"
#include "FLShortcutKeyManager.h"

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
FLShortcutKeyManager::FLShortcutKeyManager()
{
	SetShortcutKey_French();
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
FLShortcutKeyManager::~FLShortcutKeyManager()
{
}


//---------------------------------------------------------------------------------------------
// 인스턴스 얻기
//---------------------------------------------------------------------------------------------
FLShortcutKeyManager* FLShortcutKeyManager::GetInstance()
{
	static	FLShortcutKeyManager xShortcutKeyManager;
	return	&xShortcutKeyManager;
}


//---------------------------------------------------------------------------------------------
// 단축키 설정 (프랑스)
//---------------------------------------------------------------------------------------------
void FLShortcutKeyManager::SetShortcutKey_French()
{
	if( g_xFlyffConfig->GetMainLanguage() == LANG_FRE )
	{
		m_stShortcutKey_Control.KeyUp			= 'Z';
		m_stShortcutKey_Control.KeyLeft			= 'Q';
		m_stShortcutKey_Control.KeyWalk			= 'W';
		m_stShortcutKey_Control.KeyTrace		= 'F';	

		m_stShortcutKey_UI.KeyQuest				= 'L';
		m_stShortcutKey_UI.KeyCouple			= 'X';
		m_stShortcutKey_UI.KeyLordSkill			= 'A';
	}
}

