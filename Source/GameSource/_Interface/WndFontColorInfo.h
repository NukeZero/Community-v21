

#pragma once

#define WNDFONTCOLOR_WHITE	0xFFFFFFFF 
#define WNDFONTCOLOR_BLACK	0xFF000000 

//---------------------------------------------------------------------------------------------
// 윈도우 폰트 색상 정보 클레스 
//---------------------------------------------------------------------------------------------
class CWndFontColorInfo
{

public:


					CWndFontColorInfo();
					~CWndFontColorInfo();

	void			SetFontColorInfo( BYTE byR, BYTE byG, BYTE byB, BYTE byA = 255 );
	void			SetFontColorInfo( DWORD dwColor );

	const DWORD		GetFontColor( void )	{	return m_dwColor;	}

	const BYTE		GetFontColorR( void )	{	return m_byR;	}
	const BYTE		GetFontColorG( void )	{	return m_byG;	}
	const BYTE		GetFontColorB( void )	{	return m_byB;	}
	const BYTE		GetFontColorA( void )	{	return m_byA;	}


protected:


	BYTE			m_byA;
	BYTE			m_byR;
	BYTE			m_byG;
	BYTE			m_byB;

	DWORD			m_dwColor;

};


