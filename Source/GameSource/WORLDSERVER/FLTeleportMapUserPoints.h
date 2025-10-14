
#ifndef	__FLTELEPORTMAPUSERPOINTS_H__
#define	__FLTELEPORTMAPUSERPOINTS_H__


class	FLTeleportMapUserPoints
{
public:

	FLTeleportMapUserPoints();
	~FLTeleportMapUserPoints();

	void	Create( OBJID dwUserID, const std::vector< T_TELEPORTMAP_POINT >* pUserPoints );
	void	Destroy();

	DWORD	GetUserID()	const;
	size_t	GetCount()	const;
	DWORD	GetEmptyID()	const;
	void	GetAllUserPoint( std::vector< T_TELEPORTMAP_POINT >* pUserPoints )	const;

	bool	IsNearPos( const T_WORLD_POSITION* pPos, float fNearDist )	const;

	bool	Add( const T_TELEPORTMAP_POINT* pUserPoint );
	bool	ChangeName( DWORD dwPointID, const TCHAR* pszName );
	bool	Del( DWORD dwPointID );
	bool	DelAll();

	const T_TELEPORTMAP_POINT*	Find( DWORD dwPointID )	const;

	MEMPOOLER_DECLARE( FLTeleportMapUserPoints );

private:

	OBJID				m_dwUserID;
	T_TELEPORTMAP_POINT	m_tUserPoints[MAX_TELEPORT_MAP_USER_POINT];

};




#endif