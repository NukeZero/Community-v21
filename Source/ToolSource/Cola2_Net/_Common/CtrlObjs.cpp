#include "stdafx.h"

#include "CtrlObjs.h"
#include "defineSkill.h"


#ifdef __WORLDSERVER
 #include "user.h"
 #include "dpsrvr.h"

 extern	CUserMng	g_UserMng;
 extern	CDPSrvr		g_DPSrvr;
 #include "Party.h"
 extern	CPartyMng	g_PartyMng;
 #include "dpDatabaseClient.h"
 extern	CDPDatabaseClient	g_dpDBClient;
#endif // Worldserver

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 

void CContDamageCtrl::Init( void )
{
	m_tmStart = m_tmUpdate = timeGetTime();
	m_pSkillProp = NULL;
	m_pAddSkillProp = NULL;
	m_idTarget = m_idSrc = NULL_ID;
}
 
void CContDamageCtrl::Destroy( void )
{
	Init();
}
 
void CContDamageCtrl::ApplyDamage( void )
{
	ItemProp *pSkillProp = m_pSkillProp;
	AddSkillProp *pAddSkillProp = m_pAddSkillProp;
	
	CCtrl *pSrc = prj.GetCtrl( m_idSrc );			// 시전자.  중간에 없어질수도 있슴.
	CCtrl *pCenter = prj.GetCtrl( m_idTarget );		// 중심이 되는 오브젝트.  중간에 없어질수도 있슴.
	D3DXVECTOR3 vPos = GetPos();					// 중심좌표

	if( IsInvalidObj(pSrc) )
	{
		// 이때 처리를 해야한다.
		Delete();
		return;
	}

	int nApplyType = 0;
	nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;
	pSrc->ApplySkillRegion( vPos, nApplyType, pSkillProp, pAddSkillProp, false, TRUE, pCenter );		// 일단은 온리 데미지만 줄수 있게 하자.
}

 // 지속데미지를 줌.
void	CContDamageCtrl::Process( FLOAT fElapsedTime )
{
	ItemProp *pSkillProp = m_pSkillProp;
	AddSkillProp *pAddSkillProp = m_pAddSkillProp;
	
	CCtrl *pSrc = prj.GetCtrl( m_idSrc );			// 시전자.  중간에 없어질수도 있슴.
	CCtrl *pCenter = prj.GetCtrl( m_idTarget );		// 중심이 되는 오브젝트.  중간에 없어질수도 있슴.

	if( pSkillProp->tmContinuousPain == NULL_ID )
		Message( "%s의 SkillProp tmContinuousPain의 값이 -1", pSkillProp->dwID );

	if( (int)(g_tmCurrent - m_tmUpdate) > (int)pSkillProp->tmContinuousPain )		// 단위시간이 지나면
	{
		m_tmUpdate = g_tmCurrent;		// 시간 갱신하고
		// 지속데미지 처리를 이곳에 넣음.
		ApplyDamage();
	}

	if( (int)(g_tmCurrent - m_tmStart) > (int)m_pAddSkillProp->dwSkillTime )		// 총시간이 지나면 끝.
		 Delete();
}
 
 // 가상 오브젝트이기때문에 렌더하지 않음.
void	CContDamageCtrl::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
}
	

