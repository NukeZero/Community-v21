#include "stdafx.h"
#include "Respawn.h"
#ifdef __J0315
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#endif	// __J0315

#include "CommonCtrl.h"
#include "defineObj.h"

///////////////////////////////////////////////////////////////////////////////////////////
// CRespawnInfo
///////////////////////////////////////////////////////////////////////////////////////////

CRespawnInfo::CRespawnInfo()
#ifdef __INFOCRITSECT_DEBUG__
//: m_AccessLock( "CRespawnInfo::m_AccessLock" )
#endif	// __INFOCRITSECT_DEBUG__
{
	m_dwIndex	= 0; 
	m_dwType	= 0;
	m_cb		= 0; // 10
	m_uTime	= m_cbTime = 0; // 60
	memset( &m_rect, 0, sizeof(m_rect) );
	m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_nActiveAttackNum = 0;
	m_nDayMin = 1;
	m_nDayMax = 30;
	m_nHourMin = 1;
	m_nHourMax = 24;
	m_nItemMin = 1;
	m_nItemMax = 1;
	m_fY = 0;
	ZeroMemory( &m_CtrlElem, sizeof( m_CtrlElem ) );
	m_CtrlElem.m_dwMinItemNum = 1;
	m_CtrlElem.m_dwMaxiItemNum = 1;

#ifdef __S1108_BACK_END_SYSTEM
	m_nMaxcb = 0;
	m_nMaxAttackNum = 0;
	m_nGMIndex = 0;
#endif // __S1108_BACK_END_SYSTEM

#ifdef __V050426_REMOVERESPAWN 
	m_bRemove = FALSE;
#endif
}

CRespawnInfo::~CRespawnInfo()
{
}

void CRespawnInfo::GetPos( D3DXVECTOR3 & v )
{
	int nWidth = m_rect.right - m_rect.left;
	if( nWidth <= 0 ) nWidth = 1;
	int nHeight = m_rect.bottom - m_rect.top;
	if( nHeight <= 0 ) nHeight = 1;
	v.x		= m_rect.left + xRandom( nWidth );
	v.z		= m_rect.top + xRandom( nHeight );
	v.y		= m_vPos.y;
}

void CRespawnInfo::Increment( BOOL bActiveAttack )	
{ 
#ifdef __S1108_BACK_END_SYSTEM
	InterlockedDecrement( &m_cb ); 
	if( bActiveAttack ) 
		InterlockedDecrement( &m_nActiveAttackNum );	
#else // __S1108_BACK_END_SYSTEM
	InterlockedIncrement( &m_cb ); 
	if( bActiveAttack ) 
		InterlockedIncrement( &m_nActiveAttackNum );	
#endif // __S1108_BACK_END_SYSTEM
}

CRespawnInfo& CRespawnInfo::operator=( CRespawnInfo & ri )
{
	m_dwIndex	= ri.m_dwIndex;	
	m_dwType	= ri.m_dwType;	
	m_uTime	    = ri.m_uTime;		
	m_cb		= ri.m_cb; 	
	m_cbTime	= ri.m_cbTime;	
	m_rect	    = ri.m_rect;
	m_vPos	    = ri.m_vPos;
	m_fY  	   = ri.m_fY;
	m_nDayMin  = ri.m_nDayMin ;
	m_nDayMax  = ri.m_nDayMax ;
	m_nHourMin = ri.m_nHourMin;
	m_nHourMax = ri.m_nHourMax;
	m_nItemMin = ri.m_nItemMin;
	m_nItemMax = ri.m_nItemMax;
	m_nActiveAttackNum = ri.m_nActiveAttackNum;
	memcpy( &m_CtrlElem, &ri.m_CtrlElem, sizeof( m_CtrlElem ) );

	m_cbRespawn	= m_cb;	
	m_uTime		/= 2;	
	m_bHalf		= FALSE;

#ifdef __S1108_BACK_END_SYSTEM
	m_nMaxcb = ri.m_nMaxcb;
	m_nMaxAttackNum = ri.m_nMaxAttackNum;
	m_nGMIndex = ri.m_nGMIndex;
#endif // __S1108_BACK_END_SYSTEM
#ifdef __V050426_REMOVERESPAWN 
	m_bRemove = ri.m_bRemove;
#endif

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////
// CRespawner
///////////////////////////////////////////////////////////////////////////////////////////
#ifdef  __V050426_REMOVERESPAWN
CRespawner::CRespawner()
{
	m_nRespawnInfo[ 0 ] = 0;
	m_nRespawnInfo[ 1 ] = 0;
	m_nRespawnInfo[ 2 ] = 0;
}
CRespawner::~CRespawner()
{
}
int CRespawner::Add( CRespawnInfo & ri, int nType )
{	
	//chASSERT( m_nRespawnInfo < MAX_RESPAWN );		
	if( m_nRespawnInfo[ nType ] >= MAX_RESPAWN )
	{
		Error( "CRespawner::Add : %d, %d, %f, %f, %d\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z, nType );
		return -1;
	}
	else
	if( nType == RESPAWNTYPE_REGION )
	{
		ri.m_nGMIndex = m_nRespawnInfo[ nType ];
		m_aRespawnInfo[ nType ][ m_nRespawnInfo[ nType ] ] = ri;
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		CRespawnInfo* pRespawnInfo = m_aRespawnInfo[ nType ];
		int nSize = m_nRespawnInfo[ nType ];
		for( int i = 0; i < nSize; i++ )
		{
			// 같은게 발견되면 애러 
			if( pRespawnInfo[ i ].m_nGMIndex == ri.m_nGMIndex )
			{
				Error( "CRespawner::Add 같은 ID 발견 : %d, %d, %f, %f, %d\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z, nType );
				return -1;
			}
		}
		m_aRespawnInfo[ nType ][ m_nRespawnInfo[ nType ] ] = ri;
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
	}
	m_nRespawnInfo[ nType ]++;

	return m_nRespawnInfo[ nType ] - 1;

}
// 실제 삭제 
BOOL CRespawner::DoRemove( int nRespawnNo, int nType )
{
	if( nRespawnNo < 0 )	
		return NULL;
	BOOL bResult = FALSE;
	if( nType == RESPAWNTYPE_REGION )
	{
		Error( "CRespawner::Remove에서 RESPAWN_REGION 타입은 삭제할 수 없음 : %d\n", nRespawnNo );
		return FALSE;
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		CRespawnInfo* pRespawnInfo = m_aRespawnInfo[ nType ];
		int nSize = m_nRespawnInfo[ nType ];
		for( int i = 0; i < nSize; i++ )
		{
			if( pRespawnInfo[ i ].m_nGMIndex == nRespawnNo )
			{
				bResult = TRUE;
				break;
			}
		}
		for( ; i < nSize - 1 ; i++ )
			pRespawnInfo[ i ] = pRespawnInfo[ i + 1 ];
		if( bResult )
			m_nRespawnInfo[ nType ]--;
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
		return TRUE;
	}
	return bResult;
}
// 여기서는 Remove 상태만 세팅 
BOOL CRespawner::Remove( int nRespawnNo, int nType )
{
	if( nRespawnNo < 0 )	
		return NULL;
	if( nType == RESPAWNTYPE_REGION )
	{
		Error( "CRespawner::Remove에서 RESPAWN_REGION 타입은 삭제할 수 없음 : %d\n", nRespawnNo );
		return FALSE;
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		CRespawnInfo* pRespawnInfo = m_aRespawnInfo[ nType ];
		int nSize = m_nRespawnInfo[ nType ];
		for( int i = 0; i < nSize; i++ )
		{
			if( pRespawnInfo[ i ].m_nGMIndex == nRespawnNo )
			{
				pRespawnInfo[ i ].m_bRemove = TRUE;
				return TRUE;
			}
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
		return TRUE;
	}
	return FALSE;
}
CRespawnInfo* CRespawner::GetRespawnInfo( int nRespawnNo, int nType )
{
	if( nRespawnNo < 0 )	
		return NULL;
	if( nType == RESPAWNTYPE_REGION )
	{
		if( nRespawnNo < m_nRespawnInfo[ nType ] )
			return &m_aRespawnInfo[ nType ][ nRespawnNo ];	
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		CRespawnInfo* pRespawnInfo = m_aRespawnInfo[ nType ];
		int nSize = m_nRespawnInfo[ nType ];
		for( int i = 0; i < nSize; i++ )
		{
			if( pRespawnInfo[ i ].m_nGMIndex == nRespawnNo )
				return &pRespawnInfo[ i ];
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
	}
	return NULL;
}
void CRespawner::Increment( int nRespawnNo, int nType, BOOL bActiveAttack )	
{
	if( nRespawnNo < 0 )	
		return;
	if( nType == RESPAWNTYPE_REGION )
	{
		if( nRespawnNo < m_nRespawnInfo[ nType ] )
			m_aRespawnInfo[ nType ][ nRespawnNo ].Increment( bActiveAttack );	
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		CRespawnInfo* pRespawnInfo = m_aRespawnInfo[ nType ];
		int nSize = m_nRespawnInfo[ nType ];
		for( int i = 0; i < nSize; i++ )
		{
			if( pRespawnInfo[ i ].m_nGMIndex == nRespawnNo )
			{
				pRespawnInfo[ i ].Increment( bActiveAttack );	
				break;
			}
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
	}
}
u_long CRespawner::Spawn( CWorld* pWorld )
{
	if( g_eLocal.GetState( EVE_RESPAWN ) == 0 )
		return 0;

	ASSERT( pWorld );

	CRespawnInfo*	pi;
	short			cb;
	CObj*			pObj;
	D3DXVECTOR3		v;
	u_long			uRespawned	= 0;
	int				nAttr, nCheckAttr;
	int				nRetry;
	MoverProp*		pMoverProp;
	DWORD			dwFlying;

	for( int nType = 0; nType < 3; nType++ )
	{
		int nSize = m_nRespawnInfo[ nType ];
		for( int i = 0; i < nSize; i++ )			// 04.10.11 - 480개 정도 이다.
		{
			pi = &m_aRespawnInfo[ nType ][ i ];
			
			if( pi->m_dwIndex == 0 )
				continue;

			// 삭제 명령이 내려졌고, 리스폰된 오브젝트가 없다면?
			
			if( pi->m_bRemove )
			{
				if( pi->m_cb == 0 ) //== pi->m_nMaxcb )
				{
					if( DoRemove( pi->m_nGMIndex, nType ) )
					{
						i--; 
						nSize--;
						continue;
					}
				}
			}

			int nDay  = g_GameTimer.m_nDay ;
			int nHour = g_GameTimer.m_nHour;

			if( pi->m_nDayMin < pi->m_nDayMax )
			{
				if( nDay < pi->m_nDayMin || nDay > pi->m_nDayMax )
					continue;
			}
			else
			{
				if( nDay < pi->m_nDayMin && nDay > pi->m_nDayMax )
					continue;
			}
			if( pi->m_nHourMin < pi->m_nHourMax )
			{
				if( nHour < pi->m_nHourMin || nHour > pi->m_nHourMax )
					continue;
			}
			else
			{
				if( nHour < pi->m_nHourMin && nHour > pi->m_nHourMax )
					continue;
			}

			pi->m_cbTime--;									// 리스폰 타이머 시간 감소 
			if( pi->m_cbTime < 0 )							// 0 이면 리스폰 시작
			{
				short nTime = ( pi->m_uTime * xRandom( 50, 150 ) ) / 100;
	#ifdef __S1108_BACK_END_SYSTEM
				cb = 0;
				if( pi->m_nGMIndex != 0 )
				{
					pi->m_cbTime	= nTime;	// 타이머 reset	
					if( pi->m_nMaxcb - pi->m_cb > 0 )
					{
						cb	= pi->m_nMaxcb - pi->m_cb;				// cb = 죽은 갯수
					}
				}
				else
				{
					if( pWorld->GetID() == WI_WORLD_MADRIGAL )
					{
						pi->m_cbTime	= (short)(nTime * prj.m_fMonsterRebirthRate);	// 타이머 reset
						if( (pi->m_nMaxcb * prj.m_fMonsterRespawnRate) - pi->m_cb > 0 )
						{
							cb	= (pi->m_nMaxcb * prj.m_fMonsterRespawnRate) - pi->m_cb;				// cb = 죽은 갯수
						}
					}
					else
					{
						pi->m_cbTime	= nTime;	// 타이머 reset
						if( pi->m_nMaxcb - pi->m_cb > 0 )
						{
							cb	= pi->m_nMaxcb - pi->m_cb;				// cb = 죽은 갯수
						}
					}
				}
	#else // __S1108_BACK_END_SYSTEM
				pi->m_cbTime	= nTime;	// 타이머 reset
				cb	= pi->m_cb;								// cb = 죽은 갯수
	#endif // __S1108_BACK_END_SYSTEM
				
				
				if( pi->m_bHalf )
				{
	#ifdef __S1108_BACK_END_SYSTEM
					cb	= ( pi->m_nMaxcb / 2 ) - ( pi->m_nMaxcb - cb );
	#else	// __S1108_BACK_END_SYSTEM
					cb	= ( pi->m_cbRespawn / 2 ) - ( pi->m_cbRespawn - cb );
	#endif	// __S1108_BACK_END_SYSTEM
				}
				pi->m_bHalf		= !pi->m_bHalf;

	//#ifdef __J0315
	//			g_dpDBClient.AddRespawnFrequencyLog( pi->m_dwIndex, cb );
	//#endif	// __J0315

				if( pi->m_dwType == OT_MOVER )
				{
					pMoverProp = prj.GetMoverProp( pi->m_dwIndex );
					if( pMoverProp )
						dwFlying	= pMoverProp->dwFlying;
					else
						dwFlying	= 0;
				}
				else
					dwFlying	= 0;
				if( dwFlying )
					nCheckAttr = HATTR_NOFLY;
				else
					nCheckAttr = HATTR_NOWALK;

				while( cb-- > 0 )
				{
					if( pi->m_dwIndex == 0 )
					{
						Error( "CRespawner::Spawn()\t// 0 index\n" );
						continue;
					}
					pObj	= CreateObj( D3DDEVICE, pi->m_dwType, pi->m_dwIndex );
					
					if( pi->m_dwType == OT_ITEM )
					{
						CItemElem* pItemElem	= new CItemElem;
						pItemElem->m_dwItemId	= pi->m_dwIndex;
						pItemElem->m_nItemNum	= pi->m_nItemMin + xRandom( pi->m_nItemMax - pi->m_nItemMin + 1 );
						pItemElem->m_nHitPoint	= pItemElem->GetProp()->dwEndurance;
						pItemElem->m_dwSerialNumber		= xRand();
						( (CItem*)pObj )->m_pItemBase	= pItemElem;
						if( pItemElem->m_nItemNum > pItemElem->GetProp()->dwPackMax )
							pItemElem->m_nItemNum	= pItemElem->GetProp()->dwPackMax;
					}
					else
					if( pi->m_dwType == OT_CTRL )
					{
						memcpy( &(((CCommonCtrl*)pObj)->m_CtrlElem), &(pi->m_CtrlElem), sizeof( CCtrlElem ) );
						((CCommonCtrl*)pObj)->SetAngle( xRandom(360) );
					}
	#ifdef __ACROBAT_0504
					else
					if( pi->m_dwType == OT_MOVER )
					{
						((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정
					}
	#endif //__ACROBAT_0504

					if( !pObj )
						WriteLog( "SPAWN: %d, %d", pi->m_dwType, pi->m_dwIndex );

					nRetry	= 0;
	lbl_retry:
					pi->GetPos( v ); // v에 랜덤 좌표를 얻는다.
					nAttr	= pWorld->GetHeightAttribute( v.x, v.z );

					if( nAttr == -1 || nAttr == HATTR_NOMOVE || nAttr == nCheckAttr ) 
					{
						v	= pi->m_vPos;
						if( v == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
						{
							if( ++nRetry < 100 )
								goto lbl_retry;

	#ifdef __S1108_BACK_END_SYSTEM
							if( pi->m_nGMIndex != 0 )
								Remove( pi->m_nGMIndex, TRUE );
							WriteLog( "GM RESPAWN(%d): %d, %d, %d, %d, %d, %d",
								pi->m_nGMIndex, pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );
	#else // __S1108_BACK_END_SYSTEM
							WriteLog( "SPAWN: %d, %d, %d, %d, %d, %d",
								pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );
	#endif // __S1108_BACK_END_SYSTEM

							SAFE_DELETE( pObj );
							pi->m_dwIndex		= 0;	// REMOVE
							break;
						}
					}
					else
					{
						pi->m_vPos	= v;
					}

					if( dwFlying )
					{
						v.y	+= xRandom( 21 );	
						float y	= pWorld->GetLandHeight( v.x, v.z );
						if( v.y < y )	
							v.y	= y	+ 10.0f;
					}
					else
					{
						v.y = pi->m_fY + 1.0f;	// 약간 더해주자. 안그러면 뚫고 들어갈 수 있음 
						v.y	= pWorld->GetUnderHeight( v );
					}

					//( (CCtrl*)pObj )->SetRespawn( i, nType );
					( (CCtrl*)pObj )->SetRespawn( pi->m_nGMIndex, nType );
	#ifdef __S1108_BACK_END_SYSTEM
					int nMaxAttckNum = 0;
					if( nType == RESPAWNTYPE_BACKEND )	// 운영자가 설정을 해놓은것은 적용을 안시킴(선공갯수를 적용시킬수 없음)
					{
						nMaxAttckNum = int( float( pi->m_nMaxcb * prj.m_fMonsterRespawnRate ) * float((float)pi->m_nMaxAttackNum / 100.0f) );
					}
					else
					{
						if( pi->m_nMaxAttackNum == 1 && pi->m_nMaxcb == 1 ) // 거대 몬스터? 몬스터 마리수가 1명이고 선공 몬스터시 무조건 선공 몬스터 임
						{
							nMaxAttckNum = pi->m_nActiveAttackNum + 1;
						}
						else
						{
							if( pWorld->GetID() == WI_WORLD_MADRIGAL )
							{
								nMaxAttckNum = int( ( pi->m_nMaxcb * prj.m_fMonsterRespawnRate ) * prj.m_fMonsterAggressiveRate );
							}
							else
							{
								nMaxAttckNum = pi->m_nMaxAttackNum;
							}
						}
					}				
					if( pObj->GetType() == OT_MOVER && ( pi->m_nActiveAttackNum < nMaxAttckNum ) )
					{
						InterlockedIncrement( &pi->m_nActiveAttackNum );
						((CMover*)pObj)->m_bActiveAttack	= TRUE;
					}
	#else // __S1108_BACK_END_SYSTEM
					if( pObj->GetType() == OT_MOVER && pi->m_nActiveAttackNum > 0 )
					{
						InterlockedDecrement( &pi->m_nActiveAttackNum );
						((CMover*)pObj)->m_bActiveAttack	= TRUE;
					}
	#endif // __S1108_BACK_END_SYSTEM
					pObj->SetPos( v );
					pWorld->AddObj( pObj, TRUE );
					uRespawned++;
	#ifdef __S1108_BACK_END_SYSTEM
					InterlockedIncrement( &pi->m_cb );
	#else // __S1108_BACK_END_SYSTEM
					InterlockedDecrement( &pi->m_cb );
	#endif // __S1108_BACK_END_SYSTEM
				}						
			}

		}
	}
	return uRespawned;
}
#else // __V050426_REMOVERESPAWN
//{{AFX
CRespawner::CRespawner()
{
	m_nRespawnInfo	= 0;
#ifdef __S1108_BACK_END_SYSTEM
	m_nGMRespawnSize = 0;
	m_dwGMIndex = 1;
#endif // __S1108_BACK_END_SYSTEM
}
CRespawner::~CRespawner()
{
}
#ifdef __S1108_BACK_END_SYSTEM
CRespawnInfo* CRespawner::GetRespawnInfo( int nRespawnNo )
{
	for( int i = 0 ; i < m_nRespawnInfo ; ++i )
	{
		if( m_aRespawnInfo[i].m_nGMIndex == nRespawnNo )
		{
			return &m_aRespawnInfo[i];
		}
	}
	return NULL;
}
int CRespawner::Add( CRespawnInfo & ri, BOOL bGM )
{	
	//chASSERT( m_nRespawnInfo < MAX_RESPAWN );		
	if( m_nRespawnInfo >= MAX_RESPAWN )
	{
		Error( "ASSERT: %d, %d, %f, %f, %d\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z, bGM );
		return -1;
	}
	m_aRespawnInfo[m_nRespawnInfo++] = ri;	

	if( bGM )
	{
		GMAdd( ri.m_nGMIndex );
	}
	else
	{
		m_aRespawnInfo[ m_nRespawnInfo - 1 ].m_nGMIndex = 0;
	}
	return m_nRespawnInfo - 1;
}
BOOL CRespawner::Remove( int nRespawnNo, BOOL bGM )
{
	BOOL bResult = FALSE;
	int i = 0;
	for(  ; i < m_nRespawnInfo ; ++i )
	{
		if( m_aRespawnInfo[i].m_nGMIndex == nRespawnNo )
		{
			bResult = TRUE;
			break;
		}
	}
	for( ; i < m_nRespawnInfo - 1 ; ++i )
	{
		m_aRespawnInfo[ i ] = m_aRespawnInfo[ i + 1 ];
	}
	--m_nRespawnInfo;

	if( bGM && bResult )
		GMRemove( nRespawnNo );

	return bResult;
}
void CRespawner::GMAdd( int nIndex )
{
	m_nGMRespawn[m_nGMRespawnSize++] = nIndex;
}
void CRespawner::GMRemove( int nIndex )
{
	int i = 0;
	for( ; i < m_nGMRespawnSize ; ++i )
	{
		if( m_nGMRespawn[i] == nIndex )
		{
			break;
		}
	}

	for( int j = i ; j < m_nGMRespawnSize - 1 ; ++j )
	{
		m_nGMRespawn[ j ] = m_nGMRespawn[ j + 1 ];
	}
	--m_nGMRespawnSize;
}
#else // __S1108_BACK_END_SYSTEM
int CRespawner::Add( CRespawnInfo & ri )
{	
//	chASSERT( m_nRespawnInfo < MAX_RESPAWN );		
	if( m_nRespawnInfo >= MAX_RESPAWN )
	{
		TRACE( "ASSERT: %d, %d, %f, %f\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z );
		return 0;
	}
	m_aRespawnInfo[m_nRespawnInfo++] = ri;	
	return m_nRespawnInfo - 1;
}
#endif // __S1108_BACK_END_SYSTEM

void CRespawner::Increment( int i, BOOL bActiveAttack )	
{
	if( i >= 0 && i < MAX_RESPAWN )	
		m_aRespawnInfo[i].Increment( bActiveAttack );	
}
#ifndef __BEAST
u_long CRespawner::Spawn( CWorld* pWorld )
{
	if( g_eLocal.GetState( EVE_RESPAWN ) == 0 )
		return 0;

	ASSERT( pWorld );

	CRespawnInfo*	pi;
	short			cb;
	CObj*			pObj;
	D3DXVECTOR3		v;
	u_long			uRespawned	= 0;
	int				nAttr, nCheckAttr;
	int				nRetry;
	MoverProp*		pMoverProp;
	DWORD			dwFlying;

	for( int i = 0; i < m_nRespawnInfo; i++ )			// 04.10.11 - 480개 정도 이다.
	{
		pi = &m_aRespawnInfo[i];
		
		if( pi->m_dwIndex == 0 )
			continue;

		int nDay  = g_GameTimer.m_nDay ;
		int nHour = g_GameTimer.m_nHour;

		if( pi->m_nDayMin < pi->m_nDayMax )
		{
			if( nDay < pi->m_nDayMin || nDay > pi->m_nDayMax )
				continue;
		}
		else
		{
			if( nDay < pi->m_nDayMin && nDay > pi->m_nDayMax )
				continue;
		}
		if( pi->m_nHourMin < pi->m_nHourMax )
		{
			if( nHour < pi->m_nHourMin || nHour > pi->m_nHourMax )
				continue;
		}
		else
		{
			if( nHour < pi->m_nHourMin && nHour > pi->m_nHourMax )
				continue;
		}

		pi->m_cbTime--;									// 리스폰 타이머 시간 감소 
		if( pi->m_cbTime < 0 )							// 0 이면 리스폰 시작
		{
			short nTime	= ( pi->m_uTime * xRandom( 50, 150 ) ) / 100;
#ifdef __S1108_BACK_END_SYSTEM
			cb = 0;
			if( pi->m_nGMIndex != 0 )
			{
				pi->m_cbTime	= nTime;	// 타이머 reset	
				if( pi->m_nMaxcb - pi->m_cb > 0 )
				{
					cb	= pi->m_nMaxcb - pi->m_cb;				// cb = 죽은 갯수
				}
			}
			else
			{
				if( pWorld->GetID() == WI_WORLD_MADRIGAL )
				{
					pi->m_cbTime	= (short)(nTime * prj.m_fMonsterRebirthRate);	// 타이머 reset
					if( (pi->m_nMaxcb * prj.m_fMonsterRespawnRate) - pi->m_cb > 0 )
					{
						cb	= (pi->m_nMaxcb * prj.m_fMonsterRespawnRate) - pi->m_cb;				// cb = 죽은 갯수
					}
				}
				else
				{
					pi->m_cbTime	= nTime;	// 타이머 reset
					if( pi->m_nMaxcb - pi->m_cb > 0 )
					{
						cb	= pi->m_nMaxcb - pi->m_cb;				// cb = 죽은 갯수
					}
				}
			}
#else // __S1108_BACK_END_SYSTEM
			pi->m_cbTime	= nTime;	// 타이머 reset
			cb	= pi->m_cb;								// cb = 죽은 갯수
#endif // __S1108_BACK_END_SYSTEM
			
			
			if( pi->m_bHalf )
			{
#ifdef __S1108_BACK_END_SYSTEM
				cb	= ( pi->m_nMaxcb / 2 ) - ( pi->m_nMaxcb - cb );
#else	// __S1108_BACK_END_SYSTEM
				cb	= ( pi->m_cbRespawn / 2 ) - ( pi->m_cbRespawn - cb );
#endif	// __S1108_BACK_END_SYSTEM
			}
			pi->m_bHalf		= !pi->m_bHalf;

//#ifdef __J0315
//			g_dpDBClient.AddRespawnFrequencyLog( pi->m_dwIndex, cb );
//#endif	// __J0315

			if( pi->m_dwType == OT_MOVER )
			{
				pMoverProp = prj.GetMoverProp( pi->m_dwIndex );
				if( pMoverProp )
					dwFlying	= pMoverProp->dwFlying;
				else
					dwFlying	= 0;
			}
			else
				dwFlying	= 0;
			if( dwFlying )
				nCheckAttr = HATTR_NOFLY;
			else
				nCheckAttr = HATTR_NOWALK;

			while( cb-- > 0 )
			{
				
				pObj	= CreateObj( D3DDEVICE, pi->m_dwType, pi->m_dwIndex );

				if( pi->m_dwType == OT_ITEM )
				{
					CItemElem* pItemElem	= new CItemElem;
					pItemElem->m_dwItemId	= pi->m_dwIndex;
					pItemElem->m_nItemNum	= pi->m_nItemMin + xRandom( pi->m_nItemMax - pi->m_nItemMin + 1 );
					pItemElem->m_nHitPoint	= pItemElem->GetProp()->dwEndurance;
					pItemElem->m_dwSerialNumber		= xRand();
					( (CItem*)pObj )->m_pItemBase	= pItemElem;
					if( pItemElem->m_nItemNum > pItemElem->GetProp()->dwPackMax )
						pItemElem->m_nItemNum	= pItemElem->GetProp()->dwPackMax;
				}
				if( !pObj )
					WriteLog( "SPAWN: %d, %d", pi->m_dwType, pi->m_dwIndex );

				nRetry	= 0;
lbl_retry:
				pi->GetPos( v ); // v에 랜덤 좌표를 얻는다.
				nAttr	= pWorld->GetHeightAttribute( v.x, v.z );

				if( nAttr == -1 || nAttr == HATTR_NOMOVE || nAttr == nCheckAttr ) 
				{
					v	= pi->m_vPos;
					if( v == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
					{
						if( ++nRetry < 100 )
							goto lbl_retry;

#ifdef __S1108_BACK_END_SYSTEM
						if( pi->m_nGMIndex != 0 )
							Remove( pi->m_nGMIndex, TRUE );
						WriteLog( "GM RESPAWN(%d): %d, %d, %d, %d, %d, %d",
							pi->m_nGMIndex, pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );
#else // __S1108_BACK_END_SYSTEM
						WriteLog( "SPAWN: %d, %d, %d, %d, %d, %d",
							pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );
#endif // __S1108_BACK_END_SYSTEM

						SAFE_DELETE( pObj );
						pi->m_dwIndex		= 0;	// REMOVE
						break;
					}
				}
				else
				{
					pi->m_vPos	= v;
				}

				if( dwFlying )
				{
					v.y	+= xRandom( 21 );	
					float y	= pWorld->GetLandHeight( v.x, v.z );
					if( v.y < y )	
						v.y	= y	+ 10.0f;
				}
				else
				{
					v.y = pi->m_fY + 1.0f;	// 약간 더해주자. 안그러면 뚫고 들어갈 수 있음 
					v.y	= pWorld->GetUnderHeight( v );
				}

				( (CCtrl*)pObj )->SetRespawn( i );
#ifdef __S1108_BACK_END_SYSTEM
				int nMaxAttckNum = 0;
				if( pi->m_nGMIndex != 0 )	// 운영자가 설정을 해놓은것은 적용을 안시킴(선공갯수를 적용시킬수 없음)
				{
					nMaxAttckNum = int( float( pi->m_nMaxcb * prj.m_fMonsterRespawnRate ) * float((float)pi->m_nMaxAttackNum / 100.0f) );
				}
				else
				{
					if( pi->m_nMaxAttackNum == 1 && pi->m_nMaxcb == 1 ) // 거대 몬스터? 몬스터 마리수가 1명이고 선공 몬스터시 무조건 선공 몬스터 임
					{
						nMaxAttckNum = pi->m_nActiveAttackNum + 1;
					}
					else
					{
						if( pWorld->GetID() == WI_WORLD_MADRIGAL )
						{
							nMaxAttckNum = int( ( pi->m_nMaxcb * prj.m_fMonsterRespawnRate ) * prj.m_fMonsterAggressiveRate );
						}
						else
						{
							nMaxAttckNum = pi->m_nMaxAttackNum;
						}
					}
				}				
				if( pObj->GetType() == OT_MOVER && ( pi->m_nActiveAttackNum < nMaxAttckNum ) )
				{
					InterlockedIncrement( &pi->m_nActiveAttackNum );
					((CMover*)pObj)->m_bActiveAttack	= TRUE;
				}
#else // __S1108_BACK_END_SYSTEM
				if( pObj->GetType() == OT_MOVER && pi->m_nActiveAttackNum > 0 )
				{
					InterlockedDecrement( &pi->m_nActiveAttackNum );
					((CMover*)pObj)->m_bActiveAttack	= TRUE;
				}
#endif // __S1108_BACK_END_SYSTEM
				pObj->SetPos( v );
				pWorld->AddObj( pObj, TRUE );
				uRespawned++;
#ifdef __S1108_BACK_END_SYSTEM
				InterlockedIncrement( &pi->m_cb );
#else // __S1108_BACK_END_SYSTEM
				InterlockedDecrement( &pi->m_cb );
#endif // __S1108_BACK_END_SYSTEM
			}						
		}
	}
	return uRespawned;
}
#endif // __BEAST
//}}AFX
#endif // __V050426_REMOVERESPAWN



