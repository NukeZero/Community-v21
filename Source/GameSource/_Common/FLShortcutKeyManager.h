

#pragma once


//---------------------------------------------------------------------------------------------
// 단축키 구조체 (UI)
//---------------------------------------------------------------------------------------------
typedef struct FLShortcutKey_UI
{
	BYTE	KeyAllUIOnOff;
	BYTE	KeyNavigation;
	BYTE	KeyQuest;
	BYTE	KeyParty;
	BYTE	KeyGuild;
	BYTE	KeyItemContinuedDate;
	BYTE	KeyMessanger;
	BYTE	KeyPrivateBag;
	BYTE	KeyHousing;
	BYTE	KeyFurnitureStorage;
	BYTE	KeyCharInfo;
	BYTE	KeyInventory;
	BYTE	KeyMotion;
	BYTE	KeyElldinsJar;
	BYTE	KeyWorldMap;
	BYTE	KeySkill;
	BYTE	KeyLordSkill;
	BYTE	KeyCouple;

	FLShortcutKey_UI()
	:	KeyAllUIOnOff( VK_MULTIPLY )
	,	KeyNavigation( _T('N') )
	,	KeyQuest( _T('Q') )
	,	KeyParty( _T('P') )
	,	KeyGuild( _T('G') )
	,	KeyItemContinuedDate( _T('J') )
	,	KeyMessanger( _T('E') )
	,	KeyPrivateBag( _T('B') )
	,	KeyHousing( _T('Y') )
	,	KeyFurnitureStorage( _T('R') )
	,	KeyCharInfo( _T('H') )
	,	KeyInventory( _T('I') )
	,	KeyMotion( _T('O') )
	,	KeyElldinsJar( _T('T') )
	,	KeyWorldMap( _T('M') )
	,	KeySkill( _T('K') )
	,	KeyLordSkill( _T('L') )
	,	KeyCouple( _T('F') )
	{
	}

}FLSHORTCUTKEY_UI, *PFLSHORTCUTKEY_UI;



//---------------------------------------------------------------------------------------------
// 단축키 구조체 ( 컨트롤 )
//---------------------------------------------------------------------------------------------
typedef struct FLShortcutKey_Control
{
	BYTE	KeyUp;
	BYTE	KeyDown;
	BYTE	KeyLeft;
	BYTE	KeyRight;
	BYTE	KeyWalk;
	BYTE	KeyJump;
	BYTE	KeyTrace;
	BYTE	KeyCameraUp;
	BYTE	KeyCameraDown;
	BYTE	KeyCameraLeft;
	BYTE	KeyCameraRight;
	BYTE	KeyCameraZoomIn;
	BYTE	KeyCameraZoomOut;
	BYTE	KeySelectNearByTarget;

	FLShortcutKey_Control()
	:	KeyUp( _T('W') )
	,	KeyDown( _T('S') )
	,	KeyLeft( _T('A') )
	,	KeyRight( _T('D') )
	,	KeyWalk( _T('X') )
	,	KeyJump( VK_SPACE )
	,	KeyTrace( _T('Z') )	
	,	KeyCameraUp( VK_UP )
	,	KeyCameraDown( VK_DOWN )
	,	KeyCameraLeft( VK_LEFT )
	,	KeyCameraRight( VK_RIGHT )
	,	KeyCameraZoomIn( VK_PRIOR )
	,	KeyCameraZoomOut( VK_NEXT )
	,	KeySelectNearByTarget( VK_TAB )
	{
	}


}FLSHORTCUTKEY_CONTROL, *PFLSHORTCUTKEY_CONTROL;


//---------------------------------------------------------------------------------------------
// 단축키 구조체 ( 시스템 )
//---------------------------------------------------------------------------------------------
typedef struct FLShortcutKey_System
{
	BYTE	Key1;
	BYTE	Key2;
	BYTE	Key3;
	BYTE	Key4;
	BYTE	Key5;
	BYTE	Key6;
	BYTE	Key7;
	BYTE	Key8;
	BYTE	Key9;
	BYTE	Key0;

	BYTE	KeyF1;
	BYTE	KeyF2;
	BYTE	KeyF3;
	BYTE	KeyF4;
	BYTE	KeyF5;
	BYTE	KeyF6;
	BYTE	KeyF7;
	BYTE	KeyF8;
	BYTE	KeyF9;
	BYTE	KeyF10;
	BYTE	KeyF11;
	BYTE	KeyF12;

	BYTE	KeyEnter;
	BYTE	KeyEscape;
	BYTE	KeyScreenCapture;
	BYTE	KeyBackTick;

	FLShortcutKey_System()
	:	Key1( _T('1') )
	,	Key2( _T('2') )
	,	Key3( _T('3') )
	,	Key4( _T('4') )
	,	Key5( _T('5') )
	,	Key6( _T('6') )
	,	Key7( _T('7') )
	,	Key8( _T('8') )
	,	Key9( _T('9') )
	,	Key0( _T('0') )
	,	KeyF1( VK_F1 )
	,	KeyF2( VK_F2 )
	,	KeyF3( VK_F3 )
	,	KeyF4( VK_F4 )
	,	KeyF5( VK_F5 )
	,	KeyF6( VK_F6 )
	,	KeyF7( VK_F7 )
	,	KeyF8( VK_F8 )
	,	KeyF9( VK_F9 )
	,	KeyF10( VK_F10 )
	,	KeyF11( VK_F11 )
	,	KeyF12( VK_F12 )
	,	KeyEnter( VK_RETURN )
	,	KeyEscape( VK_ESCAPE )
	,	KeyScreenCapture( VK_HOME )
	,	KeyBackTick( VK_OEM_3 )
	{
	}


}FLSHORTCUTKEY_SYSTEM, *PFLSHORTCUTKEY_SYSTEM;

//---------------------------------------------------------------------------------------------
// 단축키 매니져
//---------------------------------------------------------------------------------------------
class	FLShortcutKeyManager
{

public:

	
											FLShortcutKeyManager( void );
											~FLShortcutKeyManager( void );


	static	FLShortcutKeyManager*			GetInstance( void );

	const	PFLSHORTCUTKEY_UI				GetShortcutKey_UI( void )			{	return &m_stShortcutKey_UI;			}
	const	PFLSHORTCUTKEY_CONTROL			GetShortcutKey_Control( void )		{	return &m_stShortcutKey_Control;	}
	const	PFLSHORTCUTKEY_SYSTEM			GetShortcutKey_System( void )		{	return &m_stShortcutKey_System;		}


private:

	
	void									SetShortcutKey_French( void );


private:


	FLSHORTCUTKEY_UI						m_stShortcutKey_UI;
	FLSHORTCUTKEY_CONTROL					m_stShortcutKey_Control;
	FLSHORTCUTKEY_SYSTEM					m_stShortcutKey_System;

};


#define g_xKeyManager						FLShortcutKeyManager::GetInstance()

