

#pragma once


#include "FLColor.h"


//---------------------------------------------------------------------------------------------
// 프리프 색상 메니져 클레스
//---------------------------------------------------------------------------------------------
class CFLColorManager 
{

public:

										CFLColorManager();
										~CFLColorManager();

	static	CFLColorManager*			GetInstance( void );

	// Control	
	PFLCOLOR_WNDBUTTON					GetWndButton( void )				{	return &m_stWndButton;					}

	// Content
	PFLCOLOR_WNDPARTYINFO				GetWndPartyInfo( void )				{	return &m_stWndPartyInfo;				}
	PFLCOLOR_WNDGUILDHOUSING			GetWndGuildHousing( void )			{	return &m_stWndGuildHousing;			}
	PFLCOLOR_WNDGHOUSINGSHOWONEUNIT		GetWndGHousingShowOneUnit( void )	{	return &m_stWndGHousingShowOneUnit;		}
	PFLCOLOR_WNDGUILDCOMBATRANK_CLASS	GetWndGuildCombatRank_Class( void )	{	return &m_stWndGuildCombatRank_Class;	}
	PFLCOLOR_WNDMADRIGALGIFT			GetWndMadrigalGift( void )			{	return &m_stWndMadrigalGift;			}
	PFLCOLOR_WNDFLYFFPIECESYSTEM		GetWndFlyffPieceSystem( void )		{	return &m_stWndFlyffPieceSystem;		}
	PFLCOLOR_WNDCOMMONITEMEXCHANGE		GetWndCommonItemExchange( void )	{	return &m_stWndCommonItemExchange;		}


protected:


	FLCOLOR_WNDBUTTON					m_stWndButton;

	FLCOLOR_WNDPARTYINFO				m_stWndPartyInfo;
	FLCOLOR_WNDGUILDHOUSING				m_stWndGuildHousing;
	FLCOLOR_WNDGHOUSINGSHOWONEUNIT		m_stWndGHousingShowOneUnit;
	FLCOLOR_WNDGUILDCOMBATRANK_CLASS	m_stWndGuildCombatRank_Class;
	FLCOLOR_WNDMADRIGALGIFT				m_stWndMadrigalGift;
	FLCOLOR_WNDFLYFFPIECESYSTEM			m_stWndFlyffPieceSystem;
	FLCOLOR_WNDCOMMONITEMEXCHANGE		m_stWndCommonItemExchange;

};

#define g_FLColorManager				CFLColorManager::GetInstance()
