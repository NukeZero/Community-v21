#ifndef __USERMACRO_H__
#define __USERMACRO_H__

#define ALLOCBLOCK( ar )	\
							CAr ar;

#define	GETBLOCK( ar, lpBuf, nBufSize )	\
	u_long nBufSize;	\
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );

//#define	USERPTR	\
//	( _it->second )

//#define FOR_VISIBILITYRANGE( pCtrl )	\
//	std::map< DWORD, FLWSUser* >::iterator _it  = pCtrl->m_2pc.begin(); \
//	std::map< DWORD, FLWSUser* >::iterator _end = pCtrl->m_2pc.end(); \
//	while( _it != _end )	\
//	{
//
//#define	NEXT_VISIBILITYRANGE( pCtrl )	\
//		++_it;	\
//	}


//#define FOR_VISIBILITYRANGE( pCtrl )																				\
//	if( pCtrl->GetWorld() != NULL && pCtrl->GetWorld()->m_linkMap.GetLinkMap( pCtrl->GetLayer() ) != NULL )			\
//	{																												\
//		CLinkMap * pLinkMap 	= pCtrl->GetWorld()->m_linkMap.GetLinkMap( pCtrl->GetLayer() );						\
//		FLMapGrid::FLSyncIterator _it( pLinkMap->m_kMapGrid, pCtrl->GetPos() );										\
//		FLMapGrid::FLTile * pTile = _it.GetNextTile();																\
//		while( pTile != NULL )																						\
//		{																											\
//			for( std::map<OBJID, OBJID>::const_iterator __it = pTile->arrCtrlSightMap[ CObj::linkPlayer ].begin(); __it != pTile->arrCtrlSightMap[ CObj::linkPlayer ].end(); ++__it )	\
//			{																										\
//				FLWSUser * pUser		= static_cast< FLWSUser * >( prj.GetMover( __it->first ) );	

#define FOR_VISIBILITYRANGE( pCtrl )																				\
	if( pCtrl->GetWorld() != NULL && pCtrl->GetWorld()->m_linkMap.GetLinkMap( pCtrl->GetLayer() ) != NULL )			\
	{																												\
		CLinkMap * pLinkMap 	= pCtrl->GetWorld()->m_linkMap.GetLinkMap( pCtrl->GetLayer() );						\
		FLMapGrid::FLSyncIterator _it( pLinkMap->m_kMapGrid, pCtrl->GetPos() );										\
		FLMapGrid::FLTile * pTile = _it.GetNextTile();																\
		while( pTile != NULL )																						\
		{																											\
		for( FLRing<CObj>::iterator /*std::map< CCtrl*, CCtrl* >::const_iterator*/ __it = pTile->listCtrlSight[ CObj::linkPlayer ].Begin(); __it != pTile->listCtrlSight[ CObj::linkPlayer ].End(); ++__it )	\
			{																										\
				FLWSUser * __pUser		= static_cast< FLWSUser * >( __it._MyType() );//FLWSUser * __pUser		= static_cast< FLWSUser * >( __it->second );

#define	NEXT_VISIBILITYRANGE( pCtrl )		\
			}								\
			pTile = _it.GetNextTile();		\
		}									\
	}

#define	USERPTR	\
				__pUser

#endif // USERMACRO_H