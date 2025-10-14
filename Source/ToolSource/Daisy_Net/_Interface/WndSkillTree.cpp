#include "stdafx.h"
#include "definetext.h"
#include "AppDefine.h"
#include "defineskill.h"
#include "WndSkillTree.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __11_MONTH_JOB_PRO

CWndSkillTree::CWndSkillTree()
{
}
CWndSkillTree::~CWndSkillTree()
{
}
	
BOOL CWndSkillTree::Initialize( CWndBase* pWndParent, DWORD nType )
{
	if( g_pPlayer == NULL )
		return FALSE;

	m_dwJobSkill = 0;

	switch( g_pPlayer->GetJob() )
	{
		case JOB_VAGRANT:
			m_dwJobSkill = APP_SKILLBILL_BACK;
			break;			

		case JOB_MERCENARY:
		case JOB_KNIGHT:
			m_dwJobSkill = APP_SKILLKNIGHT_BACK;
			break;			
		case JOB_BLADE:
			m_dwJobSkill = APP_SKILLBLADE_BACK;
			break;			
			
		case JOB_ASSIST:
		case JOB_RINGMASTER:
			m_dwJobSkill = APP_SKILLRING_BACK;
			break;			
		case JOB_BILLPOSTER:
			m_dwJobSkill = APP_SKILLBILL_BACK;
			break;			
			
		case JOB_MAGICIAN:
		case JOB_PSYCHIKEEPER:
			m_dwJobSkill = APP_SKILLPHY_BACK;
			break;			
		case JOB_ELEMENTOR:
			m_dwJobSkill = APP_SKILLELE_BACK;
			break;			
	}

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), m_dwJobSkill, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndSkillTree::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndSkillTree::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndStatic* pWndStatc1 = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndStatic* pWndStatc2 = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	
	switch( g_pPlayer->GetJob() )
	{
	case JOB_VAGRANT:
		pWndStatc1->SetTitle("");
		pWndStatc2->SetTitle("");
		break;			
		
	case JOB_MERCENARY:
		pWndStatc2->SetTitle("");
		break;		

	case JOB_ASSIST:
		pWndStatc2->SetTitle("");
		break;		
		
	case JOB_MAGICIAN:
		pWndStatc2->SetTitle("");
		break;		
	}

	MoveParentCenter();
}
void CWndSkillTree::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSkillTree::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSkillTree::OnLButtonDown( UINT nFlags, CPoint point )
{
}
void CWndSkillTree::OnDraw( C2DRender* p2DRender )
{
	/*
	if( m_nJob < MAX_JOB )
	{
		RenderJob( p2DRender, 0 );
	}
	*/


	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
			
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		if( dwSkill != NULL_ID )
		{
			// 스킬 아이콘 출력 
			if( m_atexSkill[ i ] && CheckSkill( i ) )
			{
				CPoint pt;
				if( GetSkillPoint( pSkillProp->dwID, pt ) )
					p2DRender->RenderTexture( pt, m_atexSkill[ i ], 255, 0.9f, 0.9f );
			}	
		}
	}
}

BOOL CWndSkillTree::GetSkillPoint(DWORD dwSkillID, CPoint& pt )
{
	LPWNDCTRL lpWndCtrl;

	switch(dwSkillID)
	{
		case SI_VAG_ONE_CLEANHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_VAG_ONE_BRANDISH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_VAG_ONE_OVERCUTTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
//머서너리
		case SI_MER_ONE_SPLMASH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_SHIELD_PROTECTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_KEENWHEEL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_BLOODYSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MER_SHIELD_PANBARRIER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_MER_SUP_IMPOWERWEAPON:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_MER_ONE_BLINDSIDE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_REFLEXHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MER_ONE_SNEAKER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_MER_SUP_SMITEAXE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_MER_SUP_BLAZINGSWORD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
		case SI_MER_ONE_SPECIALHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_GUILOTINE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MER_SUP_AXEMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_SUP_SWORDMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
// 나이트
		case SI_KNT_TWOSW_CHARGE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM19 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_KNT_TWOAX_PAINDEALER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM19 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_KNT_SUP_GUARD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM19 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_KNT_TWOSW_EARTHDIVIDER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_KNT_TWOAX_POWERSTUMP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_KNT_SUP_RAGE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_KNT_TWO_POWERSWING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_KNT_SUP_PAINREFLECTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;		
// 블레이드
		case SI_BLD_DOUBLESW_SILENTSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BLD_DOUBLEAX_SPRINGATTACK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_BLD_DOUBLE_ARMORPENETRATE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_BLD_DOUBLESW_BLADEDANCE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BLD_DOUBLEAX_HAWKATTACK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_BLD_SUP_BERZERKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_BLD_DOUBLE_CROSSSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM12 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BLD_DOUBLE_SONICBLADE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM13 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;	
// 어시스트	
		case SI_ASS_HEAL_HEALING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_HEAL_PATIENCE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_QUICKSTEP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_MENTALSIGN:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ASS_KNU_TAMPINGHOLE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_ASS_HEAL_RESURRECTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_HASTE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_HEAPUP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ASS_CHEER_STONEHAND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;			
		case SI_ASS_CHEER_CIRCLEHEALING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_CATSREFLEX:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_BEEFUP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ASS_KNU_BURSTCRACK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;			
		case SI_ASS_HEAL_PREVENTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_CANNONBALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_ACCURACY:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ASS_KNU_POWERFIST:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;			
//링마스터
		case SI_RIN_SUP_PROTECT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_RIN_SUP_HOLYCROSS:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_RIN_HEAL_GVURTIALLA:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_RIN_SUP_HOLYGUARD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_RIN_SUP_SPIRITUREFORTUNE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_RIN_HEAL_HEALRAIN:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_RIN_SQU_GEBURAHTIPHRETH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM23 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_RIN_SUP_MERKABAHANZELRUSHA:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM28 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
//빌포스터
		case SI_BIL_KNU_BELIALSMESHING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BIL_PST_ASMODEUS:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_BIL_KNU_BLOODFIST:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BIL_PST_BARAQIJALESNA:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_BIL_KNU_PIERCINGSERPENT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BIL_PST_BGVURTIALBOLD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_BIL_KNU_SONICHAND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM12 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_BIL_PST_ASALRAALAIKUM:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM13 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
// 매지션
		case SI_MAG_MAG_MENTALSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_FIRE_BOOMERANG:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_WIND_SWORDWIND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MAG_WATER_ICEMISSILE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_MAG_EARTH_SPIKESTONE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
		case SI_MAG_FIRE_HOTAIR:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_WIND_STRONGWIND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MAG_WATER_WATERBALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_MAG_EARTH_ROCKCRASH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
		case SI_MAG_FIRE_FIRESTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_WIND_WINDCUTTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MAG_WATER_SPRINGWATER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_MAG_EARTH_LOOTING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
		case SI_MAG_MAG_BLINKPOOL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
// 엘리멘터
		case SI_ELE_FIRE_FIREBIRD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ELE_EARTH_STONESPEAR:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ELE_WIND_VOID:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_ELE_WATER_ICESHARK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
		case SI_ELE_FIRE_BURINGFIELD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ELE_EARTH_EARTHQUAKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ELE_WIND_WINDFIELD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_ELE_WATER_POISONCLOUD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
		case SI_ELE_MULTY_METEOSHOWER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM23 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ELE_MULTY_SANDSTORM:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM23 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ELE_MULTY_LIGHTINGSTORM:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM23 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ELE_MULTY_AVALANCHE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM28 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ELE_FIRE_FIREMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ELE_EARTH_EARTHMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ELE_WIND_WINDMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_ELE_ELECTRICITY_LIGHTINGMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+2;
			}
			break;
		case SI_ELE_WATER_WATERMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+2;
			}
			break;
// 사이키퍼
		case SI_PSY_NLG_DEMONOLGY:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_PSY_PSY_PSYCHICBOMB:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_PSY_NLG_CRUCIOSPELL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM21 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_PSY_NLG_SATANOLGY:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_PSY_PSY_SPRITBOMB:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_PSY_PSY_MAXIMUMCRISIS:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM22 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2;
			}
			break;
		case SI_PSY_PSY_PSYCHICWALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM23 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_PSY_PSY_PSYCHICSQUARE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM28 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
			
////////////////////////////////////////////////////////////////////////////////////
			
		default:
			return FALSE;
	}

	return TRUE;
}

LPSKILL CWndSkillTree::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];//m_apSkill[ i ];
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	int nJobLevel = g_pPlayer->m_aJobLv[g_pPlayer->GetJob()];
	if( pSkill->GetProp() == NULL )
	{
		Message( "GetSkill Error Not Prop : %d", i );
		return NULL;
	}
	
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}

BOOL CWndSkillTree::CheckSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];//m_apSkill[ i ];
	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = g_pPlayer->GetSkill( pSkillProp->dwReSkill1 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
			//ssage( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
		LPSKILL pSkillBuf = g_pPlayer->GetSkill( pSkillProp->dwReSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
			//ssage( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
		}
	}	
	
	return TRUE;
}
void CWndSkillTree::SetExpert( int nJob )
{
	m_nJob = nJob;
	for( int i = MAX_JOB_SKILL; i < MAX_JOB_SKILL + MAX_EXPERT_SKILL; i++ ) 
	{
		LPSKILL lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill == NULL_ID ) continue;
		ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
		m_atexSkill[ i ] = NULL;
		if( pSkillProp )
			m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
	}
}

void CWndSkillTree::SetPro( int nJob )
{
	m_nJob = nJob;
	for( int i = MAX_JOB_SKILL + MAX_EXPERT_SKILL; i < MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL; i++ ) 
	{
		LPSKILL lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill == NULL_ID ) continue;
		ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
		m_atexSkill[ i ] = NULL;
		if( pSkillProp )
			m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
	}
}
void CWndSkillTree::InitItem( int nJob, LPSKILL apSkill )
{
	m_apSkill = apSkill;
	m_nJob = nJob;
	
	// 소스 아이템을 입력
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &apSkill[ i ];
		m_atexSkill[ i ] = NULL;
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}
}


#endif //__11_MONTH_JOB_PRO

