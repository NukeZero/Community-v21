

#pragma once

#define WNDCOLOR_WHITE			0xFFFFFFFF 
#define WNDCOLOR_BLACK			0xFF000000 
#define WNDCOLOR_DEFAULT_KEY	0xFFFF00FF 

//---------------------------------------------------------------------------------------------
// 프리프 색상 정보 클레스 
//---------------------------------------------------------------------------------------------
class CFLColorInfo
{

public:


					CFLColorInfo();
					~CFLColorInfo();

	void			SetColorInfo( BYTE byR, BYTE byG, BYTE byB, BYTE byA = 255 );
	void			SetColorInfo( DWORD dwColor );

	const DWORD		GetColor( void )	{	return m_dwColor;	}

	const BYTE		GetColorR( void )	{	return m_byR;	}
	const BYTE		GetColorG( void )	{	return m_byG;	}
	const BYTE		GetColorB( void )	{	return m_byB;	}
	const BYTE		GetColorA( void )	{	return m_byA;	}


protected:


	BYTE			m_byA;
	BYTE			m_byR;
	BYTE			m_byG;
	BYTE			m_byB;

	DWORD			m_dwColor;

};


