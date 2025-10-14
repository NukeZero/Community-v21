// Region.h: interface for the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGION_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
#define AFX_REGION_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000  

#define MAX_REGIONDESC 256
#define MAX_REGIONTITLE 256

#ifdef __BEAST 
#	include "lang.h"
#endif

struct REGIONELEM
{
	FLOAT         m_fTeleAngle;
	D3DXVECTOR3   m_vTeleWorld;  
	D3DXVECTOR3   m_vPos;  
//union
//{
	TCHAR         m_szName[ 32];
	TCHAR         m_szKey[ 32];	
//};//
	//TCHAR         m_szTeleport[ 32];
	DWORD         m_dwIdTeleWorld; // 
	TCHAR         m_szSound[ 32];
	//TCHAR         m_szMusic[ 32];
	TCHAR         m_szScript[ 32];
	DWORD         m_dwAttribute;
	CRect         m_rect;
	bool          m_bInside;
	char          m_cDescSize;

	TCHAR         m_szDesc [ MAX_REGIONDESC  ];
	TCHAR         m_szTitle[ MAX_REGIONTITLE ];
#	ifdef __BEAST
	wchar_t       m_apwszDesc [ LANG_MAX ][ MAX_REGIONDESC  ];
	wchar_t       m_apwszTitle[ LANG_MAX ][ MAX_REGIONTITLE ];
#	endif

	//TCHAR*        m_lpszDesc;
	union
	{
	bool          m_bDirectMusic;
	bool          m_bUnit; // structure type에서 unit인가?
	};
	bool          m_bPlayDirectMusic;
	bool          m_bTargetKey;
	union
	{
	DWORD		  m_dwStructure; // structure type 
	DWORD         m_dwIdMusic;
	};
	DWORD	m_dwWorldId;
	DWORD	m_dwIndex;
	
#ifdef __N_TRIGGER_AREASET_050203
	u_long	m_uItemId;
	UINT	m_uiItemCount;
	UINT	m_uiMinLevel;
	UINT	m_uiMaxLevel;
	int		m_iQuest;
	int		m_iQuestFlag;
	int		m_iJob;
	int		m_iGender;
	BOOL	m_bCheckParty;
	BOOL	m_bCheckGuild;
	BOOL	m_bChaoKey;
#endif // __N_TRIGGER_AREASET_050203
};
typedef REGIONELEM*  LPREGIONELEM;

/*----------------------------------------*/
#define	MAX_REGIONELEM	256
class CRegionElemArray
{
private:
	DWORD	m_cbRegionElem;
	REGIONELEM	m_aRegionElem[MAX_REGIONELEM];
public:
//	Constructions
	CRegionElemArray()	{	m_cbRegionElem	= 0;	}
	~CRegionElemArray()		{}
//	Operations
	int	GetSize( void )	{	return m_cbRegionElem;	}
	void	AddTail( const LPREGIONELEM lpRegionElem );
	LPREGIONELEM	GetAt( int nIndex );
};

inline void CRegionElemArray::AddTail( const LPREGIONELEM lpRegionElem )
{
	chASSERT( m_cbRegionElem < MAX_REGIONELEM );
	memcpy( &m_aRegionElem[m_cbRegionElem++], lpRegionElem, sizeof(REGIONELEM) );
}
inline LPREGIONELEM CRegionElemArray::GetAt( int nIndex )
{
	if( nIndex >= 0 && nIndex < MAX_REGIONELEM )
		return &m_aRegionElem[nIndex];
	return NULL;
}
/*----------------------------------------*/
class CRegion : public CObj
{
public:
//	DWORD m_dwType; // event, attribute
//	D3DXVECTOR3              m_vPosEnd;
	BOOL m_bEditable ;
	REGIONELEM m_region;
	
	/*
	FLOAT                   m_fLeft                 ;
	FLOAT                   m_fRight                ; 
	FLOAT                   m_fTop                  ;
	FLOAT                   m_fBottom               ;
	FLOAT                   m_fBase                 ;
	FLOAT                   m_fHeight               ;
	*/


	CRegion();
	virtual ~CRegion();
	virtual void Process(FLOAT fElapsedTime);
	void Render( LPDIRECT3DDEVICE9 pd3dDevice );

	/*

	BOOL PickBoundBoxVertex( LPDIRECT3DDEVICE9 pd3dDevice, CPoint point );

	// Direct3D 관련 오브젝트 초기화및 제거, 재설정 관련 
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT DeleteDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
*/
	virtual BOOL Read( CFileIO* pFile );
	virtual BOOL Write( CFileIO* pFile );
};


#endif // !defined(AFX_REGION_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
