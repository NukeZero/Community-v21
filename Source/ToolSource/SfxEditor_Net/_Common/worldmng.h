#ifndef __WORLDMNG_H__
#define __WORLDMNG_H__

#pragma once

typedef	REGIONELEM	RegionElem, *PRegionElem;

typedef struct tagWORLD
{
	TCHAR	m_szFileName[128];
	TCHAR	m_szWorldName[128];
	DWORD	m_dwId;
	BOOL	IsValid()	{	return( m_szFileName[0] != '\0' );	}
}
WORLD,* LPWORLD;

#define MAX_WORLD	256

class CWorld;

#ifdef __WORLDSERVER
class CWorldMng
#else	// __WORLDSERVER
class CWorldMng : public CMyMap2<CWorld*>
#endif	// __WORLDSERVER
{

#ifdef __WORLDSERVER	//	server	--------------------------------------------
private:
	CWorld*		m_pFirstActive;
public:
	CRIT_SEC	m_AddRemoveLock;
public:
	LONG	m_lCount;
//	Constructions
	void	Free( void );
//	Operations
	CWorld*		GetWorld( DWORD dwWorldID );
	CWorld*		GetFirstActive( void )	{	return m_pFirstActive;	}
	void	Add( CJurisdiction* pJurisdiction );
	void	ReadObject( void );
	void	Duplicate( WORD wWorldSrvr );
	void	Unify( WORD wWorldSrvr );
//	void	Process( void );
	BOOL	AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId = FALSE );
	BOOL	PreremoveObj( OBJID objid );
	CObj*	PregetObj( OBJID objid );
	u_long	Respawn( void );
#ifdef __INSTANCE_DUNGEON
	CWorld*	Add( DWORD dwWorldId, DWORD dwAddId );
#endif	// __INSTANCE_DUNGEON

	CRegionElemArray	m_aRevivalPos;
	CRegionElemArray	m_aRevivalRgn;	// target
#else	// __WORLDSERVER	//	not server	------------------------------------
private:
	CWorld*		m_pWorld;
public:
//	Constructions
//	Operations
	void	Free( void );
#ifdef __BEAST
	DWORD	AddWorldStruct( LPCTSTR lpszFileName ); 
	BOOL	RemoveWorldStruct( OBJID idWorld ); 
#else	// __BEAST
	CWorld*		operator () () {	return	m_pWorld; }	
	CWorld*		Get( void )	{	return	m_pWorld;	}
	CWorld*		Open( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR lpszWorld );
	CWorld*		Open( LPDIRECT3DDEVICE9 pd3dDevice, OBJID idWorld );
	void DestroyCurrentWorld();
#endif	// __BEAST
#endif	// __WOLDSERVER	--------------------------------------------------------
	
//	common	--------------------------------------------------------------------
private:
	int		m_nSize;
	void	LoadRevivalPos( DWORD dwWorldId, LPCTSTR lpszWorld );		// as loading script, it is called

#ifdef __WORLDSERVER
public:
	PRegionElem		GetRevivalPosChao( DWORD dwWorldId, LPCTSTR sKey );
	PRegionElem		GetNearRevivalPosChao( DWORD dwWorldId, D3DXVECTOR3 & vPos );
	PRegionElem		GetRevivalPos( DWORD dwWorldId, LPCTSTR sKey );
	PRegionElem		GetNearRevivalPos( DWORD dwWorldId, D3DXVECTOR3 & vPos );
#endif	// __WORLDSERVER
public:
	//WORLD	m_aWorld[MAX_WORLD];
#ifdef __BEAST
	CPtrArray m_aWorld;
#else
	CFixedArray< WORLD > m_aWorld;
#endif
//	Constructions
	CWorldMng();
	virtual	~CWorldMng();
//	Operations
	LPWORLD	GetWorldStruct( OBJID idWorld )	{ return (LPWORLD)m_aWorld.GetAt( idWorld ); }
	BOOL	LoadScript( LPCTSTR lpszFileName );
	void LoadAllMoverDialog();
/*--------------------------------------------------------------------------------*/
};

#endif	// __WORLDMNG_H__