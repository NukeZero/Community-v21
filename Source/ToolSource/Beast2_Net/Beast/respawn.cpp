#include "stdafx.h"
#include "Respawn.h"

#include "CommonCtrl.h"
#include "defineObj.h"

///////////////////////////////////////////////////////////////////////////////////////////
// CRespawnInfo
///////////////////////////////////////////////////////////////////////////////////////////

CRespawnInfo::CRespawnInfo()
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


#ifdef __YAIMONSTER_EX
	m_dwAiState = 2;
	m_fAngle    = 0.0f;

#ifdef __Y_PATROL
	m_dwPatrolIndex = NULL_ID;
	m_bPatrolCycle  = 0;
#endif //__Y_PATROL
	
#endif //__YAIMONSTER_EX

	m_dwAI = AII_NONE;

}

CRespawnInfo::~CRespawnInfo()
{
}

void CRespawnInfo::GetPos( D3DXVECTOR3 & v )
{
#ifdef __YAIMONSTER_EX
	if( m_cb == 1 )
	{
		v = m_vPos;
		return;
	}
#endif //__YAIMONSTER_EX

	int nWidth = m_rect.right - m_rect.left;
	if( nWidth <= 0 ) nWidth = 1;
	int nHeight = m_rect.bottom - m_rect.top;
	if( nHeight <= 0 ) nHeight = 1;
	v.x		= (float)( m_rect.left + xRandom( nWidth ) );
	v.z		= (float)( m_rect.top + xRandom( nHeight ) );
	v.y		= m_vPos.y;
}

void CRespawnInfo::Increment( BOOL bActiveAttack )	
{ 
	InterlockedIncrement( &m_cb ); 
	if( bActiveAttack ) 
		InterlockedIncrement( &m_nActiveAttackNum );	
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

	m_cbRespawn	= (u_short)( m_cb );
	m_uTime		/= 2;	
	m_bHalf		= FALSE;


#ifdef __YAIMONSTER_EX
	m_dwAiState = ri.m_dwAiState;
	m_fAngle    = ri.m_fAngle;

#ifdef __Y_PATROL
	m_dwPatrolIndex = ri.m_dwPatrolIndex;
	m_bPatrolCycle  = ri.m_bPatrolCycle;
#endif //__Y_PATROL

	m_dwAI = ri.m_dwAI;
	
#endif //__YAIMONSTER_EX
	

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

				pi->m_cbTime	= nTime;	// 타이머 reset
				cb	= pi->m_cb;								// cb = 죽은 갯수
				
				
				if( pi->m_bHalf )
				{
					cb	= ( pi->m_cbRespawn / 2 ) - ( pi->m_cbRespawn - cb );
				}
				pi->m_bHalf		= !pi->m_bHalf;

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
					else
					if( pi->m_dwType == OT_MOVER )
					{
						((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정
					}

					if( !pObj )
						WriteLog( "SPAWN: %d, %d", pi->m_dwType, pi->m_dwIndex );

	#ifdef __YAIMONSTER_EX
					pObj->m_dwAIInterfaceState = pi->m_dwAiState;

					if( pi->m_nMaxcb == 1 )
						pObj->SetAngle( pi->m_fAngle );
	#endif //__YAIMONSTER_EX

#ifdef __BS_AI_RESPAWN_ATTR
					pObj->m_dwAIInterface = pi->m_dwAI;
#endif
			
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

							WriteLog( "SPAWN: %d, %d, %d, %d, %d, %d",
								pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );

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

					if( pObj->GetType() == OT_MOVER && pi->m_nActiveAttackNum > 0 )
					{
						InterlockedDecrement( &pi->m_nActiveAttackNum );
						((CMover*)pObj)->m_bActiveAttack	= TRUE;
					}
					pObj->SetPos( v );
					pWorld->AddObj( pObj, TRUE );
					uRespawned++;
					InterlockedDecrement( &pi->m_cb );
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
}
CRespawner::~CRespawner()
{
}

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

void CRespawner::Increment( int i, BOOL bActiveAttack )	
{
	if( i >= 0 && i < MAX_RESPAWN )	
		m_aRespawnInfo[i].Increment( bActiveAttack );	
}

//}}AFX
#endif // __V050426_REMOVERESPAWN



