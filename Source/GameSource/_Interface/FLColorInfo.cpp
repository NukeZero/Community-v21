

#include "StdAfx.h"
#include "WndFontColorInfo.h"


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CFLColorInfo::CFLColorInfo()
:	m_byA( 255 )
,	m_byR( 255 )
,	m_byG( 255 )
,	m_byB( 255 )
,	m_dwColor( WNDFONTCOLOR_WHITE )
{
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CFLColorInfo::~CFLColorInfo()
{
}


//---------------------------------------------------------------------------------------------
// Set Color Info ( 색상 정보 설정 )
// param	: Red, Green, Blue, Alpha
// return	: void 
//---------------------------------------------------------------------------------------------
void CFLColorInfo::SetColorInfo( BYTE byR, BYTE byG, BYTE byB, BYTE byA /* = 255*/ )
{
	m_byA = byA;
	m_byR = byR;
	m_byG = byG;
	m_byB = byB;

	m_dwColor = D3DCOLOR_ARGB( m_byA, m_byR, m_byG, m_byB );
}


//---------------------------------------------------------------------------------------------
// Set Color Info ( 색상 정보 설정 )
// param	: dwColor
// return	: void 
//---------------------------------------------------------------------------------------------
void CFLColorInfo::SetColorInfo( DWORD dwColor )
{
	m_dwColor = dwColor;

	m_byA = static_cast<BYTE>( ( dwColor >> 24 ) & 0xFF );
	m_byR = static_cast<BYTE>( ( dwColor >> 16 ) & 0xFF );
	m_byG = static_cast<BYTE>( ( dwColor >> 8  ) & 0xFF );
	m_byB = static_cast<BYTE>( dwColor & 0xFF );
}



