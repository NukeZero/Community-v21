
#ifndef	__FLTELEPORTMAPUSERPOINTMANAGER_H__
#define	__FLTELEPORTMAPUSERPOINTMANAGER_H__


#include "FLTeleportMapUserPoints.h"


class	FLTeleportMapUserPointManager
{
public:

	static FLTeleportMapUserPointManager*	GetInstance();

private:

	enum
	{
		NEAR_DISTANCE	= 100,
	};

	FLTeleportMapUserPointManager();
	~FLTeleportMapUserPointManager();

public:

	bool	Initialize();
	void	Shutdown();

	bool	Add( OBJID dwUserID, const std::vector< T_TELEPORTMAP_POINT >* pUserPoints );
	void	Del( OBJID dwUserID );

	FLTeleportMapUserPoints*	Find( OBJID dwUserID );


	//////////////////////////////////////////////////////////////////////////
	bool	OnDoUseTeleportMapItem( FLWSUser* pUser, DWORD dwItemObjID, PT_ITEM_SPEC pItemProp );
	void	HandleTeleportMapToPoint( FLWSUser* pUser, const FLPacketTeleportMapToPointReq* pMsg );
	void	HandleTeleportMapAddMyPoint( FLWSUser* pUser, const FLPacketTeleportMapAddMyPointReq* pMsg );
	void	HandleTeleportMapChgPointName( FLWSUser* pUser, const FLPacketTeleportMapChgPointNameReq* pMsg );
	void	HandleTeleportMapDelPoint( FLWSUser* pUser, const FLPacketTeleportMapDelPointReq* pMsg );
	void	HandleTeleportMapDelAllPoint( FLWSUser* pUser, const FLPacketTeleportMapDelAllPointReq* pMsg );
	//////////////////////////////////////////////////////////////////////////


private:

	void	Clear();

	bool	CanUseTeleportMapItem( FLWSUser* pUser, DWORD dwItemObjID );
	bool	CanTeleport( FLWSUser* pUser, DWORD dwWorldID, const D3DXVECTOR3& tPos );

private:

	typedef	std::map< OBJID, FLTeleportMapUserPoints* >	USER_POINTS_LIST;
	typedef	USER_POINTS_LIST::iterator					USER_POINTS_ITER;

	USER_POINTS_LIST	m_mapUserPoints;

};


#define	g_pTeleportMapUserPointMng	FLTeleportMapUserPointManager::GetInstance()





#endif