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

class CWorldMng : public CMyMap2<CWorld*>
{

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
	
//	common	--------------------------------------------------------------------
private:
	int		m_nSize;
	void	LoadRevivalPos( DWORD dwWorldId, LPCTSTR lpszWorld );		// as loading script, it is called

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