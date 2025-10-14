
#ifndef	__FLEVENTARENAHELPER_H__
#define	__FLEVENTARENAHELPER_H__


#include "FLEventArenaSpecStruct.h"


class	FLEventArenaHelper
{
public:

	static CObj*	CreateCtrl( DWORD dwCtrlID, DWORD dwWorldID, const D3DXVECTOR3& tPos, float fAngle, int nLayer );
	static CObj*	CreateNPC( DWORD dwNPCID, OBJID dwTargetObjID, DWORD dwWorldID, const D3DXVECTOR3& tPos, int nLayer );

	static void		AllRecovery( FLWSUser* pUser );
	static void		ClearInven( FLWSUser* pUser );
	static void		ClearBuff( FLWSUser* pUser );
	static void		GiveItems( FLWSUser* pUser, const std::vector< T_EVENT_ARENA_GIVE_ITEM_SPEC >& vecGiveItem );
	static void		Move( FLWSUser* pUser, const T_WORLD_POSITION* pPosition, int nLayer );

	static void		Disguise( FLWSUser* pUser, DWORD dwTransformMoverID );
	static void		NoDisguise( FLWSUser* pUser );

	static void		BroadCast( const std::vector< DWORD >& vecWorldIDs, const FLPacket* pPacket );
	static void		BroadCastChat( const std::vector< DWORD >& vecWorldIDs, int nTextID, const TCHAR* pszChat, ... );
	static void		BroadCastNotice( const std::vector< DWORD >& vecWorldIDs, int nTextID, const TCHAR* pszNotice, ... );

	static void		BroadCastChat( CWorld* pWorld, int nTextID, const TCHAR* pszBuffer );
	static void		BroadCastNotice( CWorld* pWorld, int nTextID, const TCHAR* pszBuffer );
	static void		BroadCast( CWorld* pWorld, const FLPacket* pPacket );

};






#endif