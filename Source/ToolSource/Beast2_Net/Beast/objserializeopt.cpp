#include "stdafx.h"

#	if defined(__WORLDSERVER) || defined(__CLIENT) || defined(__BEAST)
#include "mover.h"
#include "defineItem.h"
#	else	// __WORLDSERVER, __CLIENT
#include "..\_network\objects\obj.h"
#	endif	// __WORLDSERVER, __CLIENT

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK

void CObj::Serialize( CAr & ar )	// 21
{
	if( ar.IsStoring() )
	{
		ar << (u_char)m_dwType;
		ar << (u_short)m_dwIndex;
		ar << (u_short)( m_vScale.x * 100.0f );
		ar << (u_short)m_dwMotion;
		ar << m_vPos;
		ar << (short)( m_fAngle * 10.0f );
	}
	else
	{
		u_short u2;

		short i2;
		
		m_dwType	=
		m_dwIndex		=
		m_dwMotion	= 0;
		
		ar >> (u_char&)m_dwType;	// m_dwType
		ar >> u2;								// m_dwIndex
		m_dwIndex	= (DWORD)u2;
		ar >> u2;								// m_vScale.x * 100.0f
		m_vScale.x	=	m_vScale.y	=	m_vScale.z	= (float)u2 / 100.0f;
		ar >> u2;								// m_dwMotion
		ar >> m_vPos;							// m_vPos
		ar >> i2;									// m_fAngle * 10.0f
		m_fAngle	= (float)i2 / 10.0f;
#	if defined(__WORLDSERVER) || defined(__CLIENT)
		SetMotion( u2 );
		SetAngle( m_fAngle );
#	else	// __WORLDSERVER	// __CLIENT
		m_dwMotion	= (DWORD)u2;
#	endif	// __WORLDSERVER	// __CLIENT
#	ifdef __WORLDSERVER
		m_vLink		= m_vPos;
#	endif	// __WORLDSERVER
	}
}

// 장어구이, 신속의 두루마리 등의 예외처리
#	if defined(__WORLDSERVER) || defined(__CLIENT)
void CMover::OnApplySM()
{
	ItemProp* aItemprop;
	int nAdjParamVal;

	if( m_dwSMTime[SM_MAX_HP] > 0 )
	{
		aItemprop = prj.GetItemProp( II_CHR_FOO_COO_BULLHAMS );
		nAdjParamVal = aItemprop->nAdjParamVal2;
		SetDestParam( aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1 );	// fixed by jeff
	}

	if( m_dwSMTime[SM_MAX_HP50] > 0 )
	{
		aItemprop = prj.GetItemProp( II_CHR_FOO_COO_GRILLEDEEL );
		nAdjParamVal = m_nPlusMaxHitPoint;
		SetDestParam( aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1 );	// fixed by jeff
	}

	if( m_dwSMTime[SM_VELOCIJUMP] > 0 )
	{
		aItemprop = prj.GetItemProp( g_AddSMMode.dwSMItemID[SM_VELOCIJUMP] );
		if( aItemprop )
		{
			if( aItemprop->dwDestParam1 != -1 )
				SetDestParam( aItemprop->dwDestParam1, aItemprop->nAdjParamVal1, NULL_CHGPARAM );
			if( aItemprop->dwDestParam2 != -1 )
				SetDestParam( aItemprop->dwDestParam2, aItemprop->nAdjParamVal2, NULL_CHGPARAM );
		}
	}
}
#endif	//

void CMover::Serialize( CAr & ar )
{

}
