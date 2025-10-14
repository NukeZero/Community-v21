#ifndef __MAP_INFORMATION_MANAGER_H__
#define __MAP_INFORMATION_MANAGER_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapComboBoxData.h"
#include "MapMonsterInformationPack.h"
#include "RainbowNPCInformationPack.h"
#include "UserMarkPositionInfo.h"

class CMapInformationManager
{
#ifdef _DEBUG
	friend class CWndMapEx;
#endif;

public:
	CMapInformationManager( void );
	~CMapInformationManager( void );

public:
	BOOL LoadMapInformationData( void );
	void DeleteAllMapInformation( void );

public:
	CTexture* const GetPCArrowTexture( void );
	CTexture* const GetPartyPCArrowTexture( void );
	CTexture* const GetTeleportationPositionTexture( void );
	CTexture* const GetTeleportTexture_General( void );
	CTexture* const GetTeleportTexture_User( void );
	CTexture* const GetDestinationPositionTexture( void );
	CTexture* const GetNPCPositionTexture( void );
	CTexture* const GetUserMarkPositionTexture( void );
	CTexture* const GetHyperlinkMarkPositionTexture( void );
	MapComboBoxDataVector* GetMapCategoryVector( void );
	MapComboBoxDataVector* GetMapNameVector( void );
	MapComboBoxDataVector* GetNPCNameVector( void );
	CMapMonsterInformationPack* FindMapMonsterInformationPack( DWORD dwFirstKey );
	CRect& FindRealPosition( BYTE byFirstKey );
	CRainbowNPCInformationPack* GetRainbowNPCInformationPack( void );
	DWORD InsertUserMarkPositionInfo( const CString& strName, FLOAT fPositionX, FLOAT fPositionY );		//return: ID
	void DeleteUserMarkPositionInfo( DWORD dwID );
	CUserMarkPositionInfo* const FindUserMarkPositionInfo( DWORD dwID );
	void DeleteAllUserMarkPositionInfo( void );
	UserMarkPositionInfoList& GetUserMarkPositionInfoList( void );

	CMapComboBoxData* FindMapComboBoxData( const DWORD worldID );				//by gmpbigsun(20110214)
	
	//버프등을 고려해서 오픈가능한 맵?, arg1: wnd용 world id, 만일WI_시작되는 리얼 아이디라면 arg2에 컨버팅요청을 해야한다.
	BOOL CanOpenMap( const DWORD wndWorldID, const DWORD wndID, BOOL bNeedConverting = FALSE );	

private:
	BOOL LoadMapIconTexture( void );
	BOOL LoadPropMapComboBoxData( void );
	BOOL LoadMapMonsterInformationPack( void );
	BOOL LoadRainbowNPCInformationPack( void );

private:
	enum { USER_MARK_MAX_SIZE = 20 };

private:
	CTexture* m_pPCArrowTexture;
	CTexture* m_pPartyPCArrowTexture;
	CTexture* m_pTeleportationPositionTexture;
	CTexture* m_pDestinationPositionTexture;
	CTexture* m_pNPCPositionTexture;
	CTexture* m_pUserMarkPositionTexture;
	CTexture* m_pHyperlinkMarkPositionTexture;
	CTexture* m_pTexTeleport_General;
	CTexture* m_pTexTeleport_User;
	MapComboBoxDataVector m_MapCategoryVector;
	MapComboBoxDataVector m_MapNameVector;
	MapComboBoxDataVector m_NPCNameVector;
	MapMonsterInformationPackMap m_MapMonsterInformationPackMap;
	std::map< BYTE, CRect* > m_RealPositionRectMap;
	CRainbowNPCInformationPack* m_pRainbowNPCInformationPack;
	UserMarkPositionInfoList m_UserMarkPositionInfoList;
};
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __MAP_INFORMATION_MANAGER_H__
