// Ctrl.cpp: implementation of the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "defineText.h"
#include "ResData.h"
#include "Ship.h"


extern CMapStrToObjId g_MapStrToObjId;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl::CCtrl()
{
	m_dwType = OT_CTRL;
	m_objid		= NULL_ID;

#if !defined(__BEAST) && !defined(__ONLINE)
	m_lRespawn	= -1;
#ifdef __V050322_CACHEQUEST
	m_nRespawnType = 0;
#endif
#endif

	m_pIAObjLink = NULL;
	D3DXMatrixIdentity( &m_mInvTM );
}

CCtrl::~CCtrl()
{

	RemoveItFromGlobalId();
}

void CCtrl::Process( FLOAT fElapsedTime )
{
	CObj::Process( fElapsedTime );
}

void CCtrl::Execute()
{

}

void CCtrl::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CObj::Render( pd3dDevice );
}

BOOL CCtrl::Read( CFileIO* pFile )
{
	return CObj::Read( pFile );
}

BOOL CCtrl::Write( CFileIO* pFile )
{
	return CObj::Write( pFile );
}

// this가 pIA에 링크되었다.
// pIA가 NULL이면 링크해제.
void CCtrl::SetIAObjLink( CShip *pIA )
{
}

void CCtrl::AddItToGlobalId( void )
{
	prj.m_objmap.Add( this );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() )
	{
		bool bResult = prj.m_idPlayerToUserPtr.insert( map<u_long, CMover*>::value_type( ( (CMover*)this )->m_idPlayer, (CMover*)this ) ).second;
		if( bResult == false )
		{
			Error("AddItToGlobalId id:%d duplicated.", ((CMover*)this)->m_idPlayer );
		}
	}

	if( GetType() == OT_CTRL )
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		
		if( _tcslen( pCommonCtrl->m_CtrlElem.m_strCtrlKey ) != NULL )
			g_MapStrToObjId.insert( MapStrToObjIdType(pCommonCtrl->m_CtrlElem.m_strCtrlKey, pCommonCtrl->GetId() ) );
	}
}

void CCtrl::RemoveItFromGlobalId( void )
{
	if( m_objid == NULL_ID )	return;
	prj.m_objmap.RemoveKey( m_objid );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() && ( (CMover*)this )->m_idPlayer != (u_long)0xffffffff )
		prj.m_idPlayerToUserPtr.erase( ( (CMover*)this )->m_idPlayer );
}


BOOL	CCtrl::GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	ItemProp* pSkillProp = prj.GetSkillProp( nSkill );	// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
	if( pSkillProp == NULL )
	{
		Error( "%s : %d. 스킬(%d)의 프로퍼티가 없다.", szErr, GetIndex(), nSkill );
		return FALSE;
	}
	
	
	AddSkillProp *pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "%s : %d. Add스킬(%d) Lv%d의 프로퍼티가 없다.", szErr, GetIndex(), nSkill, dwLevel );
		return FALSE;
	}
	
	*ppSkillProp = pSkillProp;
	*ppAddSkillProp = pAddSkillProp;
	
	return TRUE;
}


// vPos를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
// pCenter : 광역 지속스킬의 타겟. NULL일수도 있다
void	CCtrl::ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, CCtrl* pCenter )
{
	UNUSED_ALWAYS( pCenter );
	
} // ApplySkillRegion()


// this를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
void	CCtrl::ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg )
{
	
} // ApplySkillAround()

// 
// 폭, 높이, 길이를 설정하여 육면체 형태의 데미지 영역을 생성.
// nDmgType : AF_???
// nApplyType : OBJTYPE_PLAYER , MONSTER
// nAttackID : 스킬이나 아이템 ID
// fWidth, fHeight, fDepth : 폭, 높이, 길이.
// this == Attacker
void	CCtrl::ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProb )
{
	
} // ApplySkillLine()

//
// idParty 극단대상 범위 효과.
//
int		CCtrl::ApplySkillAroundTroupe( u_long idParty, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb )
{
	BOOL bRet = FALSE;

	return bRet;
} // ApplySkillAroundTroupe

//
// ApplySkill 내부에서 스킬의 하드코딩 처리부.
// this : Target
// pSrc : 시전자.
//
BOOL CCtrl::ApplySkillHardCoding( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{

	return TRUE;
}

void CCtrl::CreateYoyoSkill( CSfx* pSfx, CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	return;
}

//
// 스킬을 썼을때 클라에서 생성해야할 sfx 처리
//
void CCtrl::CreateSkillSfx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	return;
} // CreateSkillSfx


// ApplySkill 내부에서 데미지 주는부분만 빠짐.
int		CCtrl::ApplyDamage( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) 
{
	int nDamage = 0;
	// 데미지류 스킬의 데미지 전달.
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_MELEEATK:		// 근접스킬데미지공격
	case EXT_MAGICATK:			// 즉시마법데미지공격
	case EXT_MAGICATKSHOT:
		//		if( pSkillProp->dwID != SI_MAG_MAG_MENTALSTRIKE )	// 멘탈 스트라이크는 예외.
		{
			if( pSkillProp->dwID == SI_MAG_FIRE_HOTAIR )	// 핫에어 땜빵처리.
			{
				nDamage = SendDamage( AF_MAGICSKILL, pSrc->GetId(), nParam, bTarget );
			} else
			{
				int nAtkFlag = 0;
				if( pSkillProp->dwSkillType == KT_MAGIC )
					nAtkFlag = AF_MAGICSKILL;
				else
					nAtkFlag = AF_MELEESKILL;
				
				if( pSkillProp->dwComboStyle == CT_FINISH )		nDamage = SendDamageForce( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16), bTarget );
				else											nDamage = SendDamage( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16), bTarget );
			}
		}
		break;
	}

	return nDamage;
}

// 적용가능한 스킬이냐?
BOOL CCtrl::IsPossibleApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __XITEM0722_2
#if 0 
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_OBJCHGPARAMET:
	case EXT_MAGIC:
	case EXT_ANOTHER:
	case EXT_ANOTHERWITH:
		{
			DWORD	dwDestParam, dwSkillTime, dwPainTime, dwAbilityMin;
			int		nAdjParam, nChgParam;
			int		nDestData[3];
			for( int i = 0; i < 2; i ++ )
			{
				if( pAddSkillProp )
				{
					dwDestParam = pAddSkillProp->dwDestParam[i];
					dwSkillTime = pAddSkillProp->dwSkillTime;
					dwPainTime = pAddSkillProp->dwPainTime;
					dwAbilityMin = pAddSkillProp->dwAbilityMin;
					nAdjParam	= pAddSkillProp->nAdjParamVal[i];
					nChgParam	= (int)pAddSkillProp->dwChgParamVal[i];
					nDestData[0] = pAddSkillProp->nDestData1[0];
					nDestData[1] = pAddSkillProp->nDestData1[1];
					nDestData[2] = pAddSkillProp->nDestData1[2];
				} else
				{	
					dwDestParam = pSkillProp->dwDestParam[i];
					dwSkillTime = pSkillProp->dwSkillTime;
					dwPainTime  = pSkillProp->tmContinuousPain;
					dwAbilityMin = pSkillProp->dwAbilityMin;
					nAdjParam	= pSkillProp->nAdjParamVal[i];
					nChgParam	= (int)pSkillProp->dwChgParamVal[i];
					nDestData[0] = pSkillProp->nDestData1[0];
					nDestData[1] = pSkillProp->nDestData1[1];
					nDestData[2] = pSkillProp->nDestData1[2];
				}
				// destParam 1
				switch( dwDestParam )
				{
				case DST_CHRSTATE:		// 상태 변경
					if( nAdjParam & CHS_POISON )	// 독감염
						if( GetAdjParam( DST_IMMUNITY ) & CHS_POISON ) 
							return;		// 독에 면역상태다.
						break;
				}
			} // for
		}
		break;
	} // switch
#endif // 0
#endif // xItem0722
	return TRUE;
}


void	CCtrl::ApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bTarget )
{

} // ApplySkill

#ifdef __XSYNC0727
// 발사형태의 스킬의 발사체를 생성하는 부분
// 이 발사체가 타겟에게 맞았을때 DoApplySkill()이 발동된다.
int		CCtrl::ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	return 1;
}
#else // xSync0727
//{{AFX
// 발사형태의 스킬의 발사체를 생성하는 부분
// 이 발사체가 타겟에게 맞았을때 DoApplySkill()이 발동된다.
int		CCtrl::ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	return 1;
}
//}}AFX
#endif // not xSync0727

// 범위적용시 타겟의 종류에 따라 주변 타겟의 종류를 선택함.
int		CCtrl::TargetSelecter( CCtrl *pTarget )
{
	int nApplyType = 0;
	CCtrl *pSrc = this;

	if( pTarget->GetType() == OT_MOVER )	// 타겟이 무버일때
	{
		if( ((CMover *)pTarget)->IsPlayer() )			// 타겟이 플레이어면.
			nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;	// 주위 플레이어들/몬스터에게 적용
		else
			nApplyType = OBJTYPE_MONSTER;	// 타겟이 몬스터면 주위 몬스터에게만 적용.
	} else
	{
		// 타겟이 컨트롤일땐
		if( pSrc->GetType() == OT_MOVER )	// 공격자가 무버냐
		{
			if( ((CMover *)pSrc)->IsPlayer() )		// 공격자가 플레이어면
				nApplyType = OBJTYPE_CTRL | OBJTYPE_MONSTER;	// 주위 몬스터와 컨트롤에게 적용.
		}
		else
		{	// 공격자가 컨트롤이냐
			nApplyType = OBJTYPE_PLAYER;	// 플레이어에게 적용
		}
	}

	return nApplyType;
}

int		CCtrl::DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg )
{
	int nResult = DoApplySkillEx( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, nParam, bOnlyDmg );
	if( nResult == 1 )
	{
	}

	return nResult;
}

// 
// 지정된 스킬의 효과를 pTarget에게 적용한다.
// 지정범위에 따라서 pTarget을 중심으로 다른 target에도 적용된다.
// 이것을 사용하는 주체는 사람이 아닐수도 있다
// 여기서는 SpellRegion에 따라 적용대상을 분류만 하고 실제 효과적용은
// ApplySkill에서 처리한다.
// bOnlyDmg : 오직 데미지적용시키는것만 한다.  광역 지속데미지개체에서 사용함.
int		CCtrl::DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg )
{
	int		nRet = 1;
	CCtrl	*pSrc = this;

	switch( pSkillProp->dwSpellRegion )
	{
	case SRO_AROUND:	// 대상 0% 주위 100%
	case SRO_REGION:	// 대상 100% 주위 100%
		{
			if( pSkillProp->dwExeTarget != EXT_TROUPE )	// 극단대상이 아닐때.
			{
				int nApplyType = 0;
				nApplyType = pSrc->TargetSelecter( pTarget );
				pTarget->ApplySkillAround( this, nApplyType, pSkillProp, pAddSkillProp, bIgnoreProb, bOnlyDmg );
			} else
			{
				// 극단 대상 범위 효과.
				if( GetType() == OT_MOVER )
				{
					if( ((CMover *)this)->IsPlayer() )
					{
						if( ApplySkillAroundTroupe( ((CMover *)this)->m_idparty, pSkillProp, pAddSkillProp, bIgnoreProb ) == FALSE )
							return 0;  // 사용실패.
					}
				}
			}
		}
		break;
	case SRO_LINE:		// this를 중심으로 m_fAngle라인.
		{
			int nApplyType = 0;
			nApplyType = pSrc->TargetSelecter( pTarget );
			ApplySkillLine( nApplyType, pSkillProp, pAddSkillProp, 2.5f, 2.5f, (float)pAddSkillProp->dwSkillRange, bIgnoreProb );
		}
		break;
	case SRO_DIRECT:	// 대상 하나에만 적용
	default:
		if( pSkillProp->dwID != SI_PSY_PSY_PSYCHICWALL )		// 사이킥월은 처리 안함.
			pTarget->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb, nParam );
		break;
	} 

	
	// 스킬에 필요한 sfx를 생성.
	CreateSkillSfx( pTarget, pSkillProp, pAddSkillProp );
	
	// 2차 스킬 발동. ActiveSkill

	return nRet;
}

// nLevel의 dwSkill이 pTarget을 향하여 발동됨.  어태커는 this
// bIgnoreProb이 TRUE이면 내부에서 확률계산을 무시하고 100% 실행되게 한다.
void CCtrl::DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb )
{
}


