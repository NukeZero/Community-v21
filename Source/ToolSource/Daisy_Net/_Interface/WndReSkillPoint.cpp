#include "stdafx.h"
#include "definetext.h"
#include "AppDefine.h"
#include "defineskill.h"
#include "WndReSkillPoint.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define REVIEW_TIME			60

CWndReSkillPoint::CWndReSkillPoint()
{
	SetPutRegInfo( TRUE );
	
	m_pWndFocusStatic = NULL;
	m_pFocusSkill     = NULL;
	m_pVBGauge = NULL;
}
CWndReSkillPoint::~CWndReSkillPoint()
{
}
HRESULT CWndReSkillPoint::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndReSkillPoint::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndReSkillPoint::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}


LPSKILL CWndReSkillPoint::CheckBonusPoint()
{
#ifdef	__YPERSENTTOPOINT
	int nIndex     = 0;
	int nSkillpxp  = 0;
/*
	float fPersent = 0;
	float fOldPersent = 1000;
	float fskill = ( (float)m_dwTotalSkillPoint / (float)g_pPlayer->m_dwSkillPoint ) * 100.0f;
*/	
	DWORD dwOldPoint = -1;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp->dwItemJob == JOB_VAGRANT )
			continue;
		
		if( dwSkill != NULL_ID )
		{
			if( CheckSkillList( pSkill ) )
			{
				nSkillpxp = GetSkillPxp(pSkillProp->dwSubDefine, (pSkill->dwLevel != pSkillProp->dwExpertMax) ? pSkill->dwLevel+1 : pSkill->dwLevel );
				
				if( nSkillpxp <= dwOldPoint )
				{
					dwOldPoint = nSkillpxp;
					nIndex = i;
				}
				
				if( nSkillpxp <= m_dwTotalSkillPoint )
					return NULL;
			}	
		}
	}	
	
	return GetSkill( nIndex );
#else //__YPERSENTTOPOINT
	int nIndex     = 0;
	int nSkillpxp  = 0;
	float fPersent = 0;
	float fOldPersent = 1000;
	float fskill = ( (float)m_dwTotalSkillPoint / (float)g_pPlayer->m_dwSkillPoint ) * 100.0f;
	
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp->dwItemJob == JOB_VAGRANT )
			continue;
		
		if( dwSkill != NULL_ID )
		{
			if( CheckSkillList( pSkill ) )
			{
				nSkillpxp = GetSkillPxp(pSkillProp->dwSubDefine, (pSkill->dwLevel != pSkillProp->dwExpertMax) ? pSkill->dwLevel+1 : pSkill->dwLevel );
				fPersent = ( (float)nSkillpxp / (float)g_pPlayer->m_dwSkillPoint ) * 100.0f;

				if( fPersent < fOldPersent )
				{
					fOldPersent = fPersent;
					nIndex = i;
				}
				
				if( fPersent <= fskill )
					return NULL;
			}	
		}
	}	

	return GetSkill( nIndex );
#endif //__YPERSENTTOPOINT	
}

void CWndReSkillPoint::OnDraw( C2DRender* p2DRender )
{
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
			if( m_atexSkill[ i ] && CheckSkillList( pSkill ) )
			{
				CPoint pt;
				if( GetSkillPoint( pSkillProp->dwID, pt ) )
				{
					pt.x -= 1;
					pt.y -= 1;
					p2DRender->RenderTexture( pt, m_atexSkill[ i ], 255, 0.9f, 0.9f );
				}
			}	
		}
	}	
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
				
	rect = lpWndCtrl->rect;
	float fWidth = ( (float)m_dwTotalSkillPoint / (float)g_pPlayer->m_dwSkillPoint ) * 100.0f;
	CRect rectTemp = rect; 
	rectTemp.right = rectTemp.left + fWidth;
	if( rect.right < rectTemp.right )
		rectTemp.right = rect.right;
	
	m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
	m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
	
#ifdef __YPERSENTTOPOINT
	TCHAR szNum[ 32 ]; 
	sprintf( szNum, "%d", m_dwTotalSkillPoint );
	p2DRender->m_pFont->GetTextExtent( szNum ); // 레벨 폰트의 사이즈 읽기 
	p2DRender->TextOut( rect.left+4, rect.top, szNum, D3DCOLOR_COLORVALUE( 0.0f, 0.0f, 0.0f, 1.0f ) );	
#else //__YPERSENTTOPOINT
	TCHAR szNum[ 32 ]; 
	sprintf( szNum, "%.3f%%", fWidth );
	p2DRender->m_pFont->GetTextExtent( szNum ); // 레벨 폰트의 사이즈 읽기 
	p2DRender->TextOut( rect.left+4, rect.top, szNum, D3DCOLOR_COLORVALUE( 0.0f, 0.0f, 0.0f, 1.0f ) );
#endif //__YPERSENTTOPOINT
}

BOOL CWndReSkillPoint::GetSkillPoint(DWORD dwSkillID, CPoint& pt )
{
	LPWNDCTRL lpWndCtrl;

	switch(dwSkillID)
	{
//머서너리
		case SI_MER_ONE_SPLMASH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_SHIELD_PROTECTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_MER_ONE_KEENWHEEL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_BLOODYSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MER_SHIELD_PANBARRIER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_MER_SUP_IMPOWERWEAPON:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_MER_ONE_BLINDSIDE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_REFLEXHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MER_ONE_SNEAKER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_MER_SUP_SMITEAXE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_MER_SUP_BLAZINGSWORD:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+4*2;
			}
			break;
		case SI_MER_ONE_SPECIALHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MER_ONE_GUILOTINE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MER_SUP_AXEMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_MER_SUP_SWORDMASTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+4*2;
			}
			break;
// 어시스트	
		case SI_ASS_HEAL_HEALING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_HEAL_PATIENCE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_QUICKSTEP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_MENTALSIGN:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_ASS_KNU_TAMPINGHOLE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_ASS_HEAL_RESURRECTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_HASTE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_HEAPUP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_ASS_CHEER_STONEHAND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;			
		case SI_ASS_CHEER_CIRCLEHEALING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_CATSREFLEX:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_BEEFUP:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_ASS_KNU_BURSTCRACK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;			
		case SI_ASS_HEAL_PREVENTION:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_ASS_CHEER_CANNONBALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_ASS_CHEER_ACCURACY:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_ASS_KNU_POWERFIST:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;			
// 매지션
		case SI_MAG_MAG_MENTALSTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_FIRE_BOOMERANG:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_WIND_SWORDWIND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MAG_WATER_ICEMISSILE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=(24*2)+(2*2);
			}
			break;
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_MAG_EARTH_SPIKESTONE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+4*2;
			}
			break;
		case SI_MAG_FIRE_HOTAIR:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_WIND_STRONGWIND:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MAG_WATER_WATERBALL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_MAG_EARTH_ROCKCRASH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+4*2;
			}
			break;
		case SI_MAG_FIRE_FIRESTRIKE:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
			}
			break;
		case SI_MAG_WIND_WINDCUTTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*1+2;
			}
			break;
		case SI_MAG_WATER_SPRINGWATER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*3+3*2;
			}
			break;
		case SI_MAG_EARTH_LOOTING:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*4+4*2;
			}
			break;
		case SI_MAG_MAG_BLINKPOOL:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				pt = lpWndCtrl->rect.TopLeft();
				pt.y+=24*2+2*2;
			}
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

LPSKILL CWndReSkillPoint::GetSkill( int i )
{
	LPSKILL pSkill = &m_aJobSkill[ i ];//m_apSkill[ i ];
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

LPSKILL CWndReSkillPoint::GetSkillArry( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return &m_aJobSkill[i];
	}
	return NULL;
}

BOOL CWndReSkillPoint::CheckSkill( int i )
{
	LPSKILL pSkill = &m_aJobSkill[ i ];//m_apSkill[ i ];
	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	if( pSkillProp == NULL )
		return FALSE;
	
//	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
//		return FALSE;
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetSkillArry( pSkillProp->dwReSkill1 );
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
		LPSKILL pSkillBuf = GetSkillArry( pSkillProp->dwReSkill2 );
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
BOOL CWndReSkillPoint::CheckSkillList( LPSKILL pSkill )
{
	int  i = 0;
	DWORD dwBaseSkill1 = -1;
	DWORD dwBaseSkill2 = -1;

	int* npArry;
	int nAssArry[17] = { SI_ASS_HEAL_HEALING, 
		SI_ASS_HEAL_PATIENCE, 
		SI_ASS_CHEER_QUICKSTEP,
		SI_ASS_CHEER_MENTALSIGN,
		SI_ASS_KNU_TAMPINGHOLE,
		SI_ASS_HEAL_RESURRECTION,
		SI_ASS_CHEER_HASTE,
		SI_ASS_CHEER_HEAPUP,
		SI_ASS_CHEER_STONEHAND,
		SI_ASS_CHEER_CIRCLEHEALING,
		SI_ASS_CHEER_CATSREFLEX,
		SI_ASS_CHEER_BEEFUP,
		SI_ASS_KNU_BURSTCRACK,
		SI_ASS_HEAL_PREVENTION,
		SI_ASS_CHEER_CANNONBALL,
		SI_ASS_CHEER_ACCURACY,
		SI_ASS_KNU_POWERFIST };
	
	int nMerArry[15] = { SI_MER_ONE_SPLMASH,
		SI_MER_SHIELD_PROTECTION,
		SI_MER_ONE_KEENWHEEL,
		SI_MER_ONE_BLOODYSTRIKE,
		SI_MER_SHIELD_PANBARRIER,
		SI_MER_SUP_IMPOWERWEAPON,
		SI_MER_ONE_BLINDSIDE,
		SI_MER_ONE_REFLEXHIT,
		SI_MER_ONE_SNEAKER,
		SI_MER_SUP_SMITEAXE,
		SI_MER_SUP_BLAZINGSWORD,
		SI_MER_ONE_SPECIALHIT,
		SI_MER_ONE_GUILOTINE,
		SI_MER_SUP_AXEMASTER,
		SI_MER_SUP_SWORDMASTER };
	
	int nMagArry[17] = { SI_MAG_MAG_MENTALSTRIKE,
		SI_MAG_FIRE_BOOMERANG,
		SI_MAG_WIND_SWORDWIND,
		SI_MAG_WATER_ICEMISSILE,
		SI_MAG_ELECTRICITY_LIGHTINGBALL,
		SI_MAG_EARTH_SPIKESTONE,
		SI_MAG_FIRE_HOTAIR,
		SI_MAG_WIND_STRONGWIND,
		SI_MAG_WATER_WATERBALL,
		SI_MAG_ELECTRICITY_LIGHTINGPARM,
		SI_MAG_EARTH_ROCKCRASH,
		SI_MAG_FIRE_FIRESTRIKE,
		SI_MAG_WIND_WINDCUTTER,
		SI_MAG_WATER_SPRINGWATER,
		SI_MAG_ELECTRICITY_LIGHTINGSHOCK,
		SI_MAG_EARTH_LOOTING,
		SI_MAG_MAG_BLINKPOOL };

	if( m_nJob == JOB_ASSIST )
	{
		npArry = &nAssArry[0];
		
		for( i=0; i<17; i++ )
		{
			if( nAssArry[i] == pSkill->dwSkill )
			{
				dwBaseSkill1 = SI_ASS_HEAL_HEALING;
				break;
			}
		}
	}
	else
	if( m_nJob == JOB_MAGICIAN )
	{
		npArry = &nMagArry[0];
		
		for( i=0; i<17; i++ )
		{
			if( nMagArry[i] == pSkill->dwSkill )
			{
				dwBaseSkill1 = SI_MAG_MAG_MENTALSTRIKE;
				break;
			}
		}
	}
	else
	if( m_nJob == JOB_MERCENARY )
	{
		npArry = &nMerArry[0];
		
		for( i=0; i<15; i++ )
		{
			if( nMerArry[i] == pSkill->dwSkill )
			{
				dwBaseSkill1 = SI_MER_ONE_SPLMASH;
				dwBaseSkill2 = SI_MER_SHIELD_PROTECTION;
				break;
			}
		}
	}

	ItemProp* pSkillProp;

	if( dwBaseSkill1 != 0xffffffff )
	{
		pSkillProp = prj.GetSkillProp( npArry[i] );

		if( pSkillProp )
		{
			while( pSkillProp && pSkillProp->dwID != dwBaseSkill1 )
			{
				if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
					return FALSE;
				
				if( pSkillProp->dwReSkill1 != 0xffffffff )
				{
					LPSKILL pSkillBuf = GetSkillArry( pSkillProp->dwReSkill1 );

					if( pSkillBuf )
					{
						if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
						{	
							return FALSE;
						}	
						else
						{
						}
					}
				}

				if( pSkillProp->dwReSkill1 != 0xffffffff )
					pSkillProp = prj.GetSkillProp( pSkillProp->dwReSkill1 );
				else
					break;
			}
		}
	}

	if( dwBaseSkill2 != 0xffffffff ) 
	{
		pSkillProp = prj.GetSkillProp( npArry[i] );

		if( pSkillProp )
		{
			if(pSkillProp->dwReSkill2 != 0xffffffff)
			{
				while( pSkillProp && pSkillProp->dwID != dwBaseSkill2 )
				{
					if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
						return FALSE;
										
					if( pSkillProp->dwReSkill2 != 0xffffffff )
					{
						LPSKILL pSkillBuf = GetSkillArry( pSkillProp->dwReSkill2 );

						if( pSkillBuf )
						{
							if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
							{
								return FALSE;
							}	
							else
							{
							}
						}
					}	
					if( pSkillProp->dwReSkill2 != 0xffffffff )
						pSkillProp = prj.GetSkillProp( pSkillProp->dwReSkill2 );
					else
						break;
				}
			}
		}
	}

	return TRUE;
}
void CWndReSkillPoint::InitItem( DWORD dwTotalPoint, int nJob, LPSKILL apSkill )
{
	m_nJob = nJob;
	
	CPoint pt;

	memcpy( m_aJobSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );
	
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &m_aJobSkill[ i ];
		m_atexSkill[ i ] = NULL;
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}

	m_dwTotalSkillPoint = dwTotalPoint;

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );
	
/*
	CWndStatic*	pWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC19);
	char buffer[10];
	itoa( m_dwTotalSkillPoint, buffer, 10 );
	pWndStatic->SetTitle(buffer);
*/
}

LPSKILL CWndReSkillPoint::CheckSkillDefine( DWORD dwIndex )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( dwSkill != NULL_ID )
		{
			if( dwSkill == dwIndex )
			{
				if( CheckSkillList( pSkill ) )
					return pSkill;
			}
		}
	}

	return NULL;
}

int CWndReSkillPoint::GetSkillPxp(DWORD dwID, DWORD dwLv)
{
	AddSkillProp*     pAddSkillProp  = prj.GetAddSkillProp( dwID, dwLv - 1 );

	if( pAddSkillProp )
		return ( prj.m_aExpSkill[dwLv] * pAddSkillProp->dwSkillExp );

	return 0;
}

int CWndReSkillPoint::SkillLevelUp( DWORD dwIndex )
{
	int		nSkillPxp = 0;
	LPSKILL pSkill = CheckSkillDefine( dwIndex );
	ItemProp*     pSkillProp;

	if( pSkill )
	{
		pSkillProp    = prj.GetSkillProp( pSkill->dwSkill );

		if( pSkillProp )
		{
			
			if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_TIP_RESKILLPOINT_ALERT) );
				return 0;
			}
			
			

			nSkillPxp = GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel+1 );

			if( (int)m_dwTotalSkillPoint-(int)nSkillPxp < 0 )
				return 0;
				
			if( pSkill->dwLevel < pSkillProp->dwExpertMax )
			{
				pSkill->dwLevel++;
				return (int)-nSkillPxp;
			}
			return 0;
		}
		return 0;
	}
	return 0;
}

int CWndReSkillPoint::SkillLevelDown( DWORD dwIndex )
{
	int		nSkillPxp = 0;
	LPSKILL pSkill = CheckSkillDefine( dwIndex );
	ItemProp*     pSkillProp;

	if( pSkill )
	{
		pSkillProp    = prj.GetSkillProp( pSkill->dwSkill );
		
		if( pSkillProp )
		{
			nSkillPxp = GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel );

			if( g_pPlayer->m_dwSkillPoint < m_dwTotalSkillPoint+nSkillPxp )
				return 0;
			
			if( pSkill->dwLevel > pSkillProp->dwExpertLV )
			{
				pSkill->dwLevel--;
				return GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel+1 );
			}				

			return 0;
		}

		return 0;
	}

	return 0;
}

void CWndReSkillPoint::ReSetPoint()
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( dwSkill != NULL_ID )
			pSkill->dwLevel = 1;
	}
	
	m_dwTotalSkillPoint = g_pPlayer->m_dwSkillPoint;
}



BOOL CWndReSkillPoint::IsDownPoint(DWORD skill1, DWORD skill2)
{
	{
		LPSKILL pSkill1 = GetSkillArry(skill1);
		LPSKILL pSkill2 = GetSkillArry(skill2);

		if( pSkill1->dwLevel == 1 )
			return FALSE;
		
		if( pSkill2 == NULL || pSkill2->GetProp() == NULL )
			return FALSE;
		
		if( pSkill1->dwLevel-1 < pSkill2->GetProp()->dwReSkillLevel1 )
		{
			if( pSkill2->dwLevel > 1 )
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_RESKILLPOINT_TEXT5) );
				return FALSE;
			}
		}
	}
	
	return TRUE;
}
void CWndReSkillPoint::PutToolTip_Skill( DWORD dwSkill, CPoint point, CRect* pRect )
{
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
	{
		Error( "CWndReSkillPoint::PutToolTip_Skill : 스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	LPSKILL pSkill = GetSkillArry(dwSkill);

	if( pSkill == NULL )
	{
		Error( "CWndReSkillPoint::PutToolTip_Skill : pSkill스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}
	
	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "CWndReSkillPoint::PutToolTip_Skill : add스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}
	
	CString string;
	string.Format( "#cff0000ff%s#nc", pSkillProp->szName );

	CString str;
	CString strstr;

	if( CheckSkillDefine(dwSkill) )
	{
		str.Format( prj.GetText(TID_GAME_SKILLLEVEL), pSkill->dwLevel );
		string += '\n';
		string += str;
	}

	
	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
		str.Format( "\n#cffff0000%s#nc", strstr );
	else
		str.Format( "\n%s", strstr );
	string += str;
	
	if( pSkillProp->dwReSkill1 != -1 )
	{
		LPSKILL pskill = GetSkillArry(pSkillProp->dwReSkill1);
		
		if( pskill )
		{
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1 );
				str.Format( "\n#cffff0000%s#nc", strstr );
				string += str;
			}
		}
	}
	if( pSkillProp->dwReSkill2 != -1 )
	{
		LPSKILL pskill = GetSkillArry(pSkillProp->dwReSkill2);
		
		if( pskill )
		{
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2 );
				str.Format( "\n#cffff0000%s#nc", strstr );
				string += str;
			}
		}
	}
	
	if( pSkill->dwLevel+1 <= pSkillProp->dwExpertMax )
	{
		int nPxp = GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel+1 );

#ifdef __YPERSENTTOPOINT
		string += '\n';
		str.Format( prj.GetText(TID_RESKILLPOINT_PXP), nPxp);
		string += str;	
#else //__YPERSENTTOPOINT
		float fPersent = ((float)nPxp / (float)g_pPlayer->m_dwSkillPoint ) * 100.0f;
		
		string += '\n';
		str.Format( prj.GetText(TID_RESKILLPOINT_PXP), fPersent);
		string += str;	
#endif //__YPERSENTTOPOINT		
	}
	
	if( _stricmp( pSkillProp->szCommand, "=" ) )
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}
	
	
	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}


// 어시스트
CWndReSkillPoint_As::CWndReSkillPoint_As()
{
}
CWndReSkillPoint_As::~CWndReSkillPoint_As()
{
}
BOOL CWndReSkillPoint_As::Process()
{
	int nAssArry[34] = { SI_ASS_HEAL_HEALING, WIDC_STATIC2, 
		SI_ASS_HEAL_PATIENCE, WIDC_STATIC3,
		SI_ASS_CHEER_QUICKSTEP, WIDC_STATIC4,
		SI_ASS_CHEER_MENTALSIGN, WIDC_STATIC6,
		SI_ASS_KNU_TAMPINGHOLE, WIDC_STATIC7,
		SI_ASS_HEAL_RESURRECTION, WIDC_STATIC5,
		SI_ASS_CHEER_HASTE, WIDC_STATIC12,
		SI_ASS_CHEER_HEAPUP, WIDC_STATIC9,
		SI_ASS_CHEER_STONEHAND, WIDC_STATIC11,
		SI_ASS_CHEER_CIRCLEHEALING, WIDC_STATIC8,
		SI_ASS_CHEER_CATSREFLEX, WIDC_STATIC10,
		SI_ASS_CHEER_BEEFUP, WIDC_STATIC13,
		SI_ASS_KNU_BURSTCRACK, WIDC_STATIC17,
		SI_ASS_HEAL_PREVENTION, WIDC_STATIC16,
		SI_ASS_CHEER_CANNONBALL, WIDC_STATIC15,
		SI_ASS_CHEER_ACCURACY, WIDC_STATIC14,
		SI_ASS_KNU_POWERFIST,  WIDC_STATIC18 };

	LPSKILL pSkill;
	CWndStatic* pStatic;
	char buffer[10] = { 0 };
	
	for( int i=0; i<34; i+=2 )
	{
		pSkill = GetSkillArry( nAssArry[i] );

		if( pSkill )
		{
			pStatic = (CWndStatic *)GetDlgItem(nAssArry[i+1]);
			itoa( pSkill->dwLevel, buffer, 10 );
			pStatic->SetTitle(buffer);
		}
	}
/*
	CWndStatic*	pWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC19);
	itoa( m_dwTotalSkillPoint, buffer, 10 );
	pWndStatic->SetTitle(buffer);
*/	
	return TRUE;
}
BOOL CWndReSkillPoint_As::Initialize( CWndBase* pWndParent, DWORD nType )
{
	if( g_pPlayer == NULL )
		return FALSE;
	
	m_dwJobSkill = 0;
	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILLPOINT_AS, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndReSkillPoint_As::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( g_pPlayer == NULL )
		return FALSE;

	char buffer[10] = { 0 };
	int  nCount = 0;

	switch( nID )
	{
		case WTBID_CLOSE: // 종료
			g_Neuz.m_dwTimeReSkillPoint = GetTickCount() + SEC( REVIEW_TIME );
			break;
		
		case WIDC_BUTTON1:
			nCount = SkillLevelUp(SI_ASS_HEAL_HEALING);
			m_pWndFocusStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC2);
			break;
		case WIDC_BUTTON2:
			if( !IsDownPoint( SI_ASS_HEAL_HEALING, SI_ASS_HEAL_PATIENCE ) )
				break;
			if( !IsDownPoint( SI_ASS_HEAL_HEALING, SI_ASS_CHEER_QUICKSTEP ) )
				break;
			if( !IsDownPoint( SI_ASS_HEAL_HEALING, SI_ASS_CHEER_MENTALSIGN ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_HEAL_HEALING);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);

			break;
		case WIDC_BUTTON10:
			nCount = SkillLevelUp(SI_ASS_HEAL_PATIENCE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
			break;
		case WIDC_BUTTON27:
			if( !IsDownPoint( SI_ASS_HEAL_PATIENCE, SI_ASS_HEAL_RESURRECTION ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_HEAL_PATIENCE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
			break;
		case WIDC_BUTTON8:
			nCount = SkillLevelUp(SI_ASS_CHEER_QUICKSTEP);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			break;
		case WIDC_BUTTON24:
			if( !IsDownPoint( SI_ASS_CHEER_QUICKSTEP, SI_ASS_CHEER_HASTE ) )
				break;

			nCount = SkillLevelDown(SI_ASS_CHEER_QUICKSTEP);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			break;
		case WIDC_BUTTON4:
			nCount = SkillLevelUp(SI_ASS_CHEER_MENTALSIGN);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			break;
		case WIDC_BUTTON21:
			if( !IsDownPoint( SI_ASS_CHEER_MENTALSIGN, SI_ASS_CHEER_HEAPUP ) )
				break;
						
			nCount = SkillLevelDown(SI_ASS_CHEER_MENTALSIGN);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			break;
		case WIDC_BUTTON14:
			nCount = SkillLevelUp(SI_ASS_KNU_TAMPINGHOLE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			break;
		case WIDC_BUTTON22:
			if( !IsDownPoint( SI_ASS_KNU_TAMPINGHOLE, SI_ASS_CHEER_STONEHAND ) )
				break;

			nCount = SkillLevelDown(SI_ASS_KNU_TAMPINGHOLE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			break;
		case WIDC_BUTTON9:
			nCount = SkillLevelUp(SI_ASS_HEAL_RESURRECTION);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			break;
		case WIDC_BUTTON25:
			if( !IsDownPoint( SI_ASS_HEAL_RESURRECTION, SI_ASS_CHEER_CIRCLEHEALING ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_HEAL_RESURRECTION);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			break;
		case WIDC_BUTTON15:
			nCount = SkillLevelUp(SI_ASS_CHEER_HASTE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
			break;
		case WIDC_BUTTON20:
			if( !IsDownPoint( SI_ASS_CHEER_HASTE, SI_ASS_CHEER_CATSREFLEX ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_CHEER_HASTE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
			break;
		case WIDC_BUTTON13:
			nCount = SkillLevelUp(SI_ASS_CHEER_HEAPUP);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
			break;
		case WIDC_BUTTON26:
			if( !IsDownPoint( SI_ASS_CHEER_HEAPUP, SI_ASS_CHEER_BEEFUP ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_CHEER_HEAPUP);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
			break;
		case WIDC_BUTTON5:
			nCount = SkillLevelUp(SI_ASS_CHEER_STONEHAND);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			break;
		case WIDC_BUTTON23:
			if( !IsDownPoint( SI_ASS_CHEER_STONEHAND, SI_ASS_KNU_BURSTCRACK ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_CHEER_STONEHAND);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			break;
			
		case WIDC_BUTTON16:
			nCount = SkillLevelUp(SI_ASS_CHEER_CIRCLEHEALING);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
			break;
		case WIDC_BUTTON34:
			if( !IsDownPoint( SI_ASS_CHEER_CIRCLEHEALING, SI_ASS_HEAL_PREVENTION ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_CHEER_CIRCLEHEALING);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
			break;

		case WIDC_BUTTON6:
			nCount = SkillLevelUp(SI_ASS_CHEER_CATSREFLEX);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			break;
		case WIDC_BUTTON32:
			if( !IsDownPoint( SI_ASS_CHEER_CATSREFLEX, SI_ASS_CHEER_CANNONBALL ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_CHEER_CATSREFLEX);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			break;

		case WIDC_BUTTON3:
			nCount = SkillLevelUp(SI_ASS_CHEER_BEEFUP);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
			break;
		case WIDC_BUTTON33:
			if( !IsDownPoint( SI_ASS_CHEER_BEEFUP, SI_ASS_CHEER_ACCURACY ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_CHEER_BEEFUP);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
			break;

		case WIDC_BUTTON12:
			nCount = SkillLevelUp(SI_ASS_KNU_BURSTCRACK);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC17);
			break;
		case WIDC_BUTTON29:
			if( !IsDownPoint( SI_ASS_KNU_BURSTCRACK, SI_ASS_KNU_POWERFIST ) )
				break;
			
			nCount = SkillLevelDown(SI_ASS_KNU_BURSTCRACK);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC17);
			break;

		case WIDC_BUTTON18:
			nCount = SkillLevelUp(SI_ASS_HEAL_PREVENTION);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC16);
			break;
		case WIDC_BUTTON31:
			nCount = SkillLevelDown(SI_ASS_HEAL_PREVENTION);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC16);
			break;

		case WIDC_BUTTON11:
			nCount = SkillLevelUp(SI_ASS_CHEER_CANNONBALL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC15);
			break;
		case WIDC_BUTTON30:
			nCount = SkillLevelDown(SI_ASS_CHEER_CANNONBALL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC15);
			break;

		case WIDC_BUTTON17:
			nCount = SkillLevelUp(SI_ASS_CHEER_ACCURACY);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC14);
			break;
		case WIDC_BUTTON19:
			nCount = SkillLevelDown(SI_ASS_CHEER_ACCURACY);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC14);
			break;
		case WIDC_BUTTON7:
			nCount = SkillLevelUp(SI_ASS_KNU_POWERFIST);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC18);
			break;
		case WIDC_BUTTON28:
			nCount = SkillLevelDown(SI_ASS_KNU_POWERFIST);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC18);
			break;

		case WIDC_BUTTON36:
			{
				ReSetPoint();
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON35);
				pButton->EnableWindow(FALSE);
			}
			break;

		case WIDC_BUTTON35:
			if( m_pFocusSkill )
			{
				m_pFocusSkill->dwExp = m_dwTotalSkillPoint; // 임시로.. 남은 포인트 저장할곳이 없어서 .. 

				int nActiveCount = 0;
				/*
				int nActiveCount = 0;
				for( int i = MAX_JOB_SKILL; i < MAX_SKILL_JOB; i++ ) 
				{
					LPSKILL pSkill = GetSkill( i );
					if( pSkill == NULL ) 
						continue;
					DWORD dwSkill = pSkill->dwSkill;
					
					ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
					if( dwSkill != NULL_ID )
					{
						if(CheckSkillList( pSkill ) )
							nActiveCount++;
					}
				}
				/**/
				/*
				if( m_dwTotalSkillPoint == 0 )
				{
					if( nActiveCount > 1 )
						m_pFocusSkill->dwLevel--;
				}
				/**/

				if( m_dwTotalSkillPoint == 0 )
					g_DPlay.SendDoUseSkillPoint( m_aJobSkill, NULL, nActiveCount );
				else
					g_DPlay.SendDoUseSkillPoint( m_aJobSkill, m_pFocusSkill, nActiveCount );
				
				Destroy();
			}
			break;
	}

	
	if( nCount )
		m_dwTotalSkillPoint += nCount;

	// 남은 스킬포인트가 더이상 올려줄 스킬이 없냐?
	m_pFocusSkill = CheckBonusPoint();
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON35);
	if( m_pFocusSkill )
		pButton->EnableWindow(TRUE);
	else
		pButton->EnableWindow(FALSE);
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndReSkillPoint_As::OnMouseWndSurface( CPoint point )
{
	CRect rect;

	for( int i=0; i<17; i++ )
	{
		rect.left = m_rect_info[i].m_pt.x;
		rect.top  = m_rect_info[i].m_pt.y;

		rect.right  =rect.left+24;
		rect.bottom =rect.top+24;

		if( rect.PtInRect(point) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect );
			
			PutToolTip_Skill( m_rect_info[i].m_dwID, point2, &rect );
		}
		
	}
}

void CWndReSkillPoint_As::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CPoint	  pt;
	LPWNDCTRL lpWndCtrl;

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
	m_rect_info[0].m_dwID = SI_ASS_HEAL_HEALING;
	m_rect_info[0].m_pt   = lpWndCtrl->rect.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	pt = lpWndCtrl->rect.TopLeft();

	m_rect_info[1].m_dwID = SI_ASS_HEAL_PATIENCE;
	m_rect_info[1].m_pt = pt;

	m_rect_info[2].m_dwID = SI_ASS_CHEER_QUICKSTEP;
	m_rect_info[2].m_pt = pt;
	m_rect_info[2].m_pt.y+=24*1+1*2;
	

	m_rect_info[3].m_dwID = SI_ASS_CHEER_MENTALSIGN;
	m_rect_info[3].m_pt = pt;
	m_rect_info[3].m_pt.y+=24*2+2*2;
	
	m_rect_info[4].m_dwID = SI_ASS_KNU_TAMPINGHOLE;
	m_rect_info[4].m_pt = pt;
	m_rect_info[4].m_pt.y+=24*3+3*2;
	

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	pt = lpWndCtrl->rect.TopLeft();
	m_rect_info[5].m_dwID = SI_ASS_HEAL_RESURRECTION;
	m_rect_info[5].m_pt   = lpWndCtrl->rect.TopLeft();

	m_rect_info[6].m_dwID = SI_ASS_CHEER_HASTE;
	m_rect_info[6].m_pt = pt;
	m_rect_info[6].m_pt.y+=24*1+1*2;
	
	m_rect_info[7].m_dwID = SI_ASS_CHEER_HEAPUP;
	m_rect_info[7].m_pt = pt;
	m_rect_info[7].m_pt.y+=24*2+2*2;
	
	m_rect_info[8].m_dwID = SI_ASS_CHEER_STONEHAND;
	m_rect_info[8].m_pt = pt;
	m_rect_info[8].m_pt.y+=24*3+3*2;
	

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt = lpWndCtrl->rect.TopLeft();
	m_rect_info[9].m_dwID = SI_ASS_CHEER_CIRCLEHEALING;
	m_rect_info[9].m_pt   = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[10].m_dwID = SI_ASS_CHEER_CATSREFLEX;
	m_rect_info[10].m_pt = pt;
	m_rect_info[10].m_pt.y+=24*1+1*2;
	
	m_rect_info[11].m_dwID = SI_ASS_CHEER_BEEFUP;
	m_rect_info[11].m_pt = pt;
	m_rect_info[11].m_pt.y+=24*2+2*2;
	
	m_rect_info[12].m_dwID = SI_ASS_KNU_BURSTCRACK;
	m_rect_info[12].m_pt = pt;
	m_rect_info[12].m_pt.y+=24*3+3*2;
	
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
	pt = lpWndCtrl->rect.TopLeft();
	m_rect_info[13].m_dwID = SI_ASS_HEAL_PREVENTION;
	m_rect_info[13].m_pt   = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[14].m_dwID = SI_ASS_CHEER_CANNONBALL;
	m_rect_info[14].m_pt = pt;
	m_rect_info[14].m_pt.y+=24*1+1*2;
	
	m_rect_info[15].m_dwID = SI_ASS_CHEER_ACCURACY;
	m_rect_info[15].m_pt = pt;
	m_rect_info[15].m_pt.y+=24*2+2*2;
	
	m_rect_info[16].m_dwID = SI_ASS_KNU_POWERFIST;
	m_rect_info[16].m_pt = pt;
	m_rect_info[16].m_pt.y+=24*3+3*2;

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON35);
	pButton->EnableWindow(FALSE);
	
	MoveParentCenter();
}


// 매지션
CWndReSkillPoint_Ma::CWndReSkillPoint_Ma()
{
}
CWndReSkillPoint_Ma::~CWndReSkillPoint_Ma()
{
}
BOOL CWndReSkillPoint_Ma::Initialize( CWndBase* pWndParent, DWORD nType )
{
	if( g_pPlayer == NULL )
		return FALSE;
	
	m_dwJobSkill = 0;
	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILLPOINT_MA, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndReSkillPoint_Ma::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( g_pPlayer == NULL )
		return FALSE;

	char buffer[10] = { 0 };
	int  nCount = 0;

	switch( nID )
	{
		case WTBID_CLOSE: // 종료
			g_Neuz.m_dwTimeReSkillPoint = GetTickCount() + SEC( REVIEW_TIME );
			break;
			
		case WIDC_BUTTON1:
			nCount = SkillLevelUp(SI_MAG_MAG_MENTALSTRIKE);
			m_pWndFocusStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC8);
			break;
		case WIDC_BUTTON2:
			if( !IsDownPoint( SI_MAG_MAG_MENTALSTRIKE, SI_MAG_FIRE_BOOMERANG ) )
				break;
			if( !IsDownPoint( SI_MAG_MAG_MENTALSTRIKE, SI_MAG_MAG_BLINKPOOL ) )
				break;
			if( !IsDownPoint( SI_MAG_MAG_MENTALSTRIKE, SI_MAG_WIND_SWORDWIND ) )
				break;
			if( !IsDownPoint( SI_MAG_MAG_MENTALSTRIKE, SI_MAG_WATER_ICEMISSILE ) )
				break;
			if( !IsDownPoint( SI_MAG_MAG_MENTALSTRIKE, SI_MAG_ELECTRICITY_LIGHTINGBALL ) )
				break;
			if( !IsDownPoint( SI_MAG_MAG_MENTALSTRIKE, SI_MAG_EARTH_SPIKESTONE ) )
				break;
			
			nCount = SkillLevelDown(SI_MAG_MAG_MENTALSTRIKE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
			break;
			//
		case WIDC_BUTTON9:
			nCount = SkillLevelUp(SI_MAG_MAG_BLINKPOOL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
			break;
		case WIDC_BUTTON23:
			nCount = SkillLevelDown(SI_MAG_MAG_BLINKPOOL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
			break;
			//
		case WIDC_BUTTON6:
			nCount = SkillLevelUp(SI_MAG_FIRE_BOOMERANG);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			break;
		case WIDC_BUTTON24:
			if( !IsDownPoint( SI_MAG_FIRE_BOOMERANG, SI_MAG_FIRE_HOTAIR ) )
				break;
			nCount = SkillLevelDown(SI_MAG_FIRE_BOOMERANG);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			break;
			//
		case WIDC_BUTTON8:
			nCount = SkillLevelUp(SI_MAG_WIND_SWORDWIND);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			break;
		case WIDC_BUTTON19:
			if( !IsDownPoint( SI_MAG_WIND_SWORDWIND, SI_MAG_WIND_STRONGWIND ) )
				break;
			nCount = SkillLevelDown(SI_MAG_WIND_SWORDWIND);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			break;
			//
		case WIDC_BUTTON7:
			nCount = SkillLevelUp(SI_MAG_WATER_ICEMISSILE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
			break;
		case WIDC_BUTTON22:
			if( !IsDownPoint( SI_MAG_WATER_ICEMISSILE, SI_MAG_WATER_WATERBALL ) )
				break;
			nCount = SkillLevelDown(SI_MAG_WATER_ICEMISSILE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
			break;
			//
		case WIDC_BUTTON3:
			nCount = SkillLevelUp(SI_MAG_ELECTRICITY_LIGHTINGBALL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			break;
		case WIDC_BUTTON20:
			if( !IsDownPoint( SI_MAG_ELECTRICITY_LIGHTINGBALL, SI_MAG_ELECTRICITY_LIGHTINGPARM ) )
				break;
			nCount = SkillLevelDown(SI_MAG_ELECTRICITY_LIGHTINGBALL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			break;
			//
		case WIDC_BUTTON5:
			nCount = SkillLevelUp(SI_MAG_EARTH_SPIKESTONE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			break;
		case WIDC_BUTTON21:
			if( !IsDownPoint( SI_MAG_EARTH_SPIKESTONE, SI_MAG_EARTH_ROCKCRASH ) )
				break;
			nCount = SkillLevelDown(SI_MAG_EARTH_SPIKESTONE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			break;
			//
		case WIDC_BUTTON4:
			nCount = SkillLevelUp(SI_MAG_FIRE_HOTAIR);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			break;
		case WIDC_BUTTON32:
			if( !IsDownPoint( SI_MAG_FIRE_HOTAIR, SI_MAG_FIRE_FIRESTRIKE ) )
				break;
			nCount = SkillLevelDown(SI_MAG_FIRE_HOTAIR);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			break;
			//
		case WIDC_BUTTON13:
			nCount = SkillLevelUp(SI_MAG_WIND_STRONGWIND);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
			break;
		case WIDC_BUTTON27:
			if( !IsDownPoint( SI_MAG_WIND_STRONGWIND, SI_MAG_WIND_WINDCUTTER ) )
				break;
			nCount = SkillLevelDown(SI_MAG_WIND_STRONGWIND);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
			break;
			//
		case WIDC_BUTTON11:
			nCount = SkillLevelUp(SI_MAG_WATER_WATERBALL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
			break;
		case WIDC_BUTTON25:
			if( !IsDownPoint( SI_MAG_WATER_WATERBALL, SI_MAG_WATER_SPRINGWATER ) )
				break;
			nCount = SkillLevelDown(SI_MAG_WATER_WATERBALL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
			break;
			//
		case WIDC_BUTTON14:
			nCount = SkillLevelUp(SI_MAG_ELECTRICITY_LIGHTINGPARM);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC14);
			break;
		case WIDC_BUTTON31:
			if( !IsDownPoint( SI_MAG_ELECTRICITY_LIGHTINGPARM, SI_MAG_ELECTRICITY_LIGHTINGSHOCK ) )
				break;
			nCount = SkillLevelDown(SI_MAG_ELECTRICITY_LIGHTINGPARM);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC14);
			break;
			//
		case WIDC_BUTTON10:
			nCount = SkillLevelUp(SI_MAG_EARTH_ROCKCRASH);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
			break;
		case WIDC_BUTTON26:
			if( !IsDownPoint( SI_MAG_EARTH_ROCKCRASH, SI_MAG_EARTH_LOOTING ) )
				break;
			nCount = SkillLevelDown(SI_MAG_EARTH_ROCKCRASH);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
			break;
			//
		case WIDC_BUTTON16:
			nCount = SkillLevelUp(SI_MAG_FIRE_FIRESTRIKE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC16);
			break;
		case WIDC_BUTTON29:
			nCount = SkillLevelDown(SI_MAG_FIRE_FIRESTRIKE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC16);
			break;
			//
		case WIDC_BUTTON17:
			nCount = SkillLevelUp(SI_MAG_WIND_WINDCUTTER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC15);
			break;
		case WIDC_BUTTON28:
			nCount = SkillLevelDown(SI_MAG_WIND_WINDCUTTER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC15);
			break;
			//
		case WIDC_BUTTON15:
			nCount = SkillLevelUp(SI_MAG_WATER_SPRINGWATER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			break;
		case WIDC_BUTTON36:
			nCount = SkillLevelDown(SI_MAG_WATER_SPRINGWATER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			break;
			//
		case WIDC_BUTTON12:
			nCount = SkillLevelUp(SI_MAG_ELECTRICITY_LIGHTINGSHOCK);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC18);
			break;
		case WIDC_BUTTON30:
			nCount = SkillLevelDown(SI_MAG_ELECTRICITY_LIGHTINGSHOCK);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC18);
			break;
			//
		case WIDC_BUTTON18:
			nCount = SkillLevelUp(SI_MAG_EARTH_LOOTING);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC17);
			break;
		case WIDC_BUTTON33:
			nCount = SkillLevelDown(SI_MAG_EARTH_LOOTING);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC17);
			break;

		case WIDC_BUTTON34:
			{
				ReSetPoint();
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON35);
				pButton->EnableWindow(FALSE);
			}
			break;

		case WIDC_BUTTON35:
			if( m_pFocusSkill )
			{
				m_pFocusSkill->dwExp = m_dwTotalSkillPoint; // 임시로.. 남은 포인트 저장할곳이 없어서 .. 

				int nActiveCount = 0;
				/*
				int nActiveCount = 0;
				for( int i = MAX_JOB_SKILL; i < MAX_SKILL_JOB; i++ ) 
				{
					LPSKILL pSkill = GetSkill( i );
					if( pSkill == NULL ) 
						continue;
					DWORD dwSkill = pSkill->dwSkill;
					
					ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
					if( dwSkill != NULL_ID )
					{
						if(CheckSkillList( pSkill ) )
							nActiveCount++;
					}
				}
				/**/
				/*
				if( m_dwTotalSkillPoint == 0 )
				{
					if( nActiveCount > 1 )
						m_pFocusSkill->dwLevel--;
				}
				/**/

				if( m_dwTotalSkillPoint == 0 )
					g_DPlay.SendDoUseSkillPoint( m_aJobSkill, NULL, nActiveCount );
				else
					g_DPlay.SendDoUseSkillPoint( m_aJobSkill, m_pFocusSkill, nActiveCount );
				
				Destroy();
			}
			break;
	}

	if( nCount )
		m_dwTotalSkillPoint += nCount;			
	
	// 남은 스킬포인트가 더이상 올려줄 스킬이 없냐?
	m_pFocusSkill = CheckBonusPoint();
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON35);
	if( m_pFocusSkill )
		pButton->EnableWindow(TRUE);
	else
		pButton->EnableWindow(FALSE);
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndReSkillPoint_Ma::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	CPoint	  pt;
	LPWNDCTRL lpWndCtrl;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	pt = lpWndCtrl->rect.TopLeft();
	m_rect_info[0].m_dwID = SI_MAG_MAG_MENTALSTRIKE;
	m_rect_info[0].m_pt   = pt;

	m_rect_info[1].m_dwID = SI_MAG_MAG_BLINKPOOL;
	m_rect_info[1].m_pt = pt;
	m_rect_info[1].m_pt.y+=24*2+2*2;
	
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[2].m_dwID = SI_MAG_FIRE_BOOMERANG;
	m_rect_info[2].m_pt = pt;
	
	m_rect_info[3].m_dwID = SI_MAG_WIND_SWORDWIND;
	m_rect_info[3].m_pt = pt;
	m_rect_info[3].m_pt.y+=24*1+1*2;
		
	m_rect_info[4].m_dwID = SI_MAG_WATER_ICEMISSILE;
	m_rect_info[4].m_pt = pt;
	m_rect_info[4].m_pt.y+=24*2+2*2;
	
	m_rect_info[5].m_dwID = SI_MAG_ELECTRICITY_LIGHTINGBALL;
	m_rect_info[5].m_pt = pt;
	m_rect_info[5].m_pt.y+=24*3+3*2;
	
	m_rect_info[6].m_dwID = SI_MAG_EARTH_SPIKESTONE;
	m_rect_info[6].m_pt = pt;
	m_rect_info[6].m_pt.y+=24*4+4*2;

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
	pt = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[7].m_dwID = SI_MAG_FIRE_HOTAIR;
	m_rect_info[7].m_pt = pt;
	
	m_rect_info[8].m_dwID = SI_MAG_WIND_STRONGWIND;
	m_rect_info[8].m_pt = pt;
	m_rect_info[8].m_pt.y+=24*1+1*2;
	
	m_rect_info[9].m_dwID = SI_MAG_WATER_WATERBALL;
	m_rect_info[9].m_pt = pt;
	m_rect_info[9].m_pt.y+=24*2+2*2;
	
	m_rect_info[10].m_dwID = SI_MAG_ELECTRICITY_LIGHTINGPARM;
	m_rect_info[10].m_pt = pt;
	m_rect_info[10].m_pt.y+=24*3+3*2;
	
	m_rect_info[11].m_dwID = SI_MAG_EARTH_ROCKCRASH;
	m_rect_info[11].m_pt = pt;
	m_rect_info[11].m_pt.y+=24*4+4*2;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
	pt = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[12].m_dwID = SI_MAG_FIRE_FIRESTRIKE;
	m_rect_info[12].m_pt = pt;
	
	m_rect_info[13].m_dwID = SI_MAG_WIND_WINDCUTTER;
	m_rect_info[13].m_pt = pt;
	m_rect_info[13].m_pt.y+=24*1+1*2;
	
	m_rect_info[14].m_dwID = SI_MAG_WATER_SPRINGWATER;
	m_rect_info[14].m_pt = pt;
	m_rect_info[14].m_pt.y+=24*2+2*2;
	
	m_rect_info[15].m_dwID = SI_MAG_ELECTRICITY_LIGHTINGSHOCK;
	m_rect_info[15].m_pt = pt;
	m_rect_info[15].m_pt.y+=24*3+3*2;
	
	m_rect_info[16].m_dwID = SI_MAG_EARTH_LOOTING;
	m_rect_info[16].m_pt = pt;
	m_rect_info[16].m_pt.y+=24*4+4*2;

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON35);
	pButton->EnableWindow(FALSE);	
	
	MoveParentCenter();
}
void CWndReSkillPoint_Ma::OnMouseWndSurface( CPoint point )
{
	CRect rect;
	
	for( int i=0; i<17; i++ )
	{
		rect.left = m_rect_info[i].m_pt.x;
		rect.top  = m_rect_info[i].m_pt.y;
		
		rect.right  =rect.left+24;
		rect.bottom =rect.top+24;
		
		if( rect.PtInRect(point) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect );
			
			PutToolTip_Skill( m_rect_info[i].m_dwID, point2, &rect );
		}
		
	}
}	
BOOL CWndReSkillPoint_Ma::Process()
{
	int nMagArry[34] = { SI_MAG_MAG_MENTALSTRIKE, WIDC_STATIC8, 
		SI_MAG_FIRE_BOOMERANG, WIDC_STATIC7, 
		SI_MAG_WIND_SWORDWIND, WIDC_STATIC6,
		SI_MAG_WATER_ICEMISSILE, WIDC_STATIC3,
		SI_MAG_ELECTRICITY_LIGHTINGBALL, WIDC_STATIC4,
		SI_MAG_EARTH_SPIKESTONE, WIDC_STATIC5,
		SI_MAG_FIRE_HOTAIR, WIDC_STATIC11,
		SI_MAG_WIND_STRONGWIND, WIDC_STATIC9,
		SI_MAG_WATER_WATERBALL, WIDC_STATIC12,
		SI_MAG_ELECTRICITY_LIGHTINGPARM, WIDC_STATIC14,
		SI_MAG_EARTH_ROCKCRASH, WIDC_STATIC13,
		SI_MAG_FIRE_FIRESTRIKE, WIDC_STATIC16,
		SI_MAG_WIND_WINDCUTTER, WIDC_STATIC15,
		SI_MAG_WATER_SPRINGWATER, WIDC_STATIC10,
		SI_MAG_ELECTRICITY_LIGHTINGSHOCK, WIDC_STATIC18,
		SI_MAG_EARTH_LOOTING, WIDC_STATIC17,
		SI_MAG_MAG_BLINKPOOL, WIDC_STATIC2 };
	
	LPSKILL pSkill;
	CWndStatic* pStatic;
	char buffer[10] = { 0 };
	
	for( int i=0; i<34; i+=2 )
	{
		pSkill = GetSkillArry( nMagArry[i] );

		if( pSkill )
		{
			pStatic = (CWndStatic *)GetDlgItem(nMagArry[i+1]);
			itoa( pSkill->dwLevel, buffer, 10 );
			pStatic->SetTitle(buffer);
		}
	}
/*	
	CWndStatic*	pWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC19);
	itoa( m_dwTotalSkillPoint, buffer, 10 );
	pWndStatic->SetTitle(buffer);
*/
	return TRUE;
}	




// 머서너리
CWndReSkillPoint_Me::CWndReSkillPoint_Me()
{
}
CWndReSkillPoint_Me::~CWndReSkillPoint_Me()
{
}
BOOL CWndReSkillPoint_Me::Initialize( CWndBase* pWndParent, DWORD nType )
{
	if( g_pPlayer == NULL )
		return FALSE;
	
	m_dwJobSkill = 0;
	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILLPOINT_ME, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndReSkillPoint_Me::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( g_pPlayer == NULL )
		return FALSE;

	char buffer[10] = { 0 };
	int  nCount = 0;

	switch( nID )
	{
		case WTBID_CLOSE: // 종료
			g_Neuz.m_dwTimeReSkillPoint = GetTickCount() + SEC( REVIEW_TIME );
			break;
			
		case WIDC_BUTTON5:
			nCount = SkillLevelUp(SI_MER_ONE_SPLMASH);
			m_pWndFocusStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC7);
			break;
		case WIDC_BUTTON2:
			if( !IsDownPoint( SI_MER_ONE_SPLMASH, SI_MER_ONE_KEENWHEEL ) )
				break;
			if( !IsDownPoint( SI_MER_ONE_SPLMASH, SI_MER_ONE_BLOODYSTRIKE ) )
				break;
			nCount = SkillLevelDown(SI_MER_ONE_SPLMASH);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
			break;
			//
		case WIDC_BUTTON8:
			nCount = SkillLevelUp(SI_MER_SHIELD_PROTECTION);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			break;
		case WIDC_BUTTON22:
			if( !IsDownPoint( SI_MER_SHIELD_PROTECTION, SI_MER_SHIELD_PANBARRIER ) )
				break;
			if( !IsDownPoint( SI_MER_SHIELD_PROTECTION, SI_MER_SUP_IMPOWERWEAPON ) )
				break;
			nCount = SkillLevelDown(SI_MER_SHIELD_PROTECTION);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
			break;
			//
		case WIDC_BUTTON6:
			nCount = SkillLevelUp(SI_MER_ONE_KEENWHEEL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
			break;
		case WIDC_BUTTON17:
			if( !IsDownPoint( SI_MER_ONE_KEENWHEEL, SI_MER_ONE_BLINDSIDE ) )
				break;
			nCount = SkillLevelDown(SI_MER_ONE_KEENWHEEL);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
			break;
			//
		case WIDC_BUTTON1:
			nCount = SkillLevelUp(SI_MER_ONE_BLOODYSTRIKE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			break;
		case WIDC_BUTTON18:
			if( !IsDownPoint( SI_MER_ONE_BLOODYSTRIKE, SI_MER_ONE_REFLEXHIT ) )
				break;
			nCount = SkillLevelDown(SI_MER_ONE_BLOODYSTRIKE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
			break;
			//
		case WIDC_BUTTON10:
			nCount = SkillLevelUp(SI_MER_SHIELD_PANBARRIER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
			break;
		case WIDC_BUTTON20:
			if( !IsDownPoint( SI_MER_SHIELD_PANBARRIER, SI_MER_ONE_SNEAKER ) )
				break;
			nCount = SkillLevelDown(SI_MER_SHIELD_PANBARRIER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
			break;
			//
		case WIDC_BUTTON11:
			nCount = SkillLevelUp(SI_MER_SUP_IMPOWERWEAPON);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			break;
		case WIDC_BUTTON19:
			if( !IsDownPoint( SI_MER_SUP_IMPOWERWEAPON, SI_MER_SUP_SMITEAXE ) )
				break;
			if( !IsDownPoint( SI_MER_SUP_IMPOWERWEAPON, SI_MER_SUP_BLAZINGSWORD ) )
				break;
			nCount = SkillLevelDown(SI_MER_SUP_IMPOWERWEAPON);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
			break;
			//
		case WIDC_BUTTON15:
			nCount = SkillLevelUp(SI_MER_ONE_BLINDSIDE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
			break;
		case WIDC_BUTTON21:
			if( !IsDownPoint( SI_MER_ONE_BLINDSIDE, SI_MER_ONE_SPECIALHIT ) )
				break;
			nCount = SkillLevelDown(SI_MER_ONE_BLINDSIDE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
			break;
			//
		case WIDC_BUTTON13:
			nCount = SkillLevelUp(SI_MER_ONE_REFLEXHIT);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
			break;
		case WIDC_BUTTON28:
			if( !IsDownPoint( SI_MER_ONE_REFLEXHIT, SI_MER_ONE_GUILOTINE ) )
				break;
			nCount = SkillLevelDown(SI_MER_ONE_REFLEXHIT);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC9);
			break;
			//
		case WIDC_BUTTON12:
			nCount = SkillLevelUp(SI_MER_ONE_SNEAKER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
			break;
		case WIDC_BUTTON23:
			nCount = SkillLevelDown(SI_MER_ONE_SNEAKER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
			break;
			//
		case WIDC_BUTTON14:
			nCount = SkillLevelUp(SI_MER_SUP_SMITEAXE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			break;
		case WIDC_BUTTON24:
			if( !IsDownPoint( SI_MER_SUP_SMITEAXE, SI_MER_SUP_AXEMASTER ) )
				break;
			nCount = SkillLevelDown(SI_MER_SUP_SMITEAXE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
			break;
			//
		case WIDC_BUTTON16:
			nCount = SkillLevelUp(SI_MER_SUP_BLAZINGSWORD);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			break;
		case WIDC_BUTTON27:
			if( !IsDownPoint( SI_MER_SUP_BLAZINGSWORD, SI_MER_SUP_SWORDMASTER ) )
				break;
			nCount = SkillLevelDown(SI_MER_SUP_BLAZINGSWORD);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
			break;
			//
		case WIDC_BUTTON7:
			nCount = SkillLevelUp(SI_MER_ONE_SPECIALHIT);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
			break;
		case WIDC_BUTTON25:
			nCount = SkillLevelDown(SI_MER_ONE_SPECIALHIT);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
			break;
			//
		case WIDC_BUTTON4:
			nCount = SkillLevelUp(SI_MER_ONE_GUILOTINE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC15);
			break;
		case WIDC_BUTTON26:
			nCount = SkillLevelDown(SI_MER_ONE_GUILOTINE);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC15);
			break;
			//
		case WIDC_BUTTON9:
			nCount = SkillLevelUp(SI_MER_SUP_AXEMASTER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC14);
			break;
		case WIDC_BUTTON30:
			nCount = SkillLevelDown(SI_MER_SUP_AXEMASTER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC14);
			break;
			//
		case WIDC_BUTTON3:
			nCount = SkillLevelUp(SI_MER_SUP_SWORDMASTER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC16);
			break;
		case WIDC_BUTTON29:
			nCount = SkillLevelDown(SI_MER_SUP_SWORDMASTER);
			m_pWndFocusStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC16);
			break;

		case WIDC_BUTTON32:
			{
				ReSetPoint();
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON31);
				pButton->EnableWindow(FALSE);
			}
			break;

		// 플레이어 스킬포인트 셋팅
		case WIDC_BUTTON31:
			if( m_pFocusSkill )
			{
				m_pFocusSkill->dwExp = m_dwTotalSkillPoint; // 임시로.. 남은 포인트 저장할곳이 없어서 .. 

				int nActiveCount = 0;
				/*
				int nActiveCount = 0;
				for( int i = MAX_JOB_SKILL; i < MAX_SKILL_JOB; i++ ) 
				{
					LPSKILL pSkill = GetSkill( i );
					if( pSkill == NULL ) 
						continue;
					DWORD dwSkill = pSkill->dwSkill;
					
					ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
					if( dwSkill != NULL_ID )
					{
						if(CheckSkillList( pSkill ) )
							nActiveCount++;
					}
				}
				/**/
				/*
				if( m_dwTotalSkillPoint == 0 )
				{
					if( nActiveCount > 1 )
						m_pFocusSkill->dwLevel--;
				}
				/**/

				if( m_dwTotalSkillPoint == 0 )
					g_DPlay.SendDoUseSkillPoint( m_aJobSkill, NULL, nActiveCount );
				else
					g_DPlay.SendDoUseSkillPoint( m_aJobSkill, m_pFocusSkill, nActiveCount );
				
				Destroy();
			}
			break;
	}

	if( nCount )
		m_dwTotalSkillPoint += nCount;			
	
	// 남은 스킬포인트가 더이상 올려줄 스킬이 없냐?
	m_pFocusSkill = CheckBonusPoint();
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON31);

	if( m_pFocusSkill )
		pButton->EnableWindow(TRUE);
	else
		pButton->EnableWindow(FALSE);

	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndReSkillPoint_Me::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CPoint	  pt;
	LPWNDCTRL lpWndCtrl;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	pt = lpWndCtrl->rect.TopLeft();
	m_rect_info[0].m_dwID = SI_MER_ONE_SPLMASH;
	m_rect_info[0].m_pt   = pt;
	
	m_rect_info[1].m_dwID = SI_MER_SHIELD_PROTECTION;
	m_rect_info[1].m_pt = pt;
	m_rect_info[1].m_pt.y+=24*2+2*2;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[2].m_dwID = SI_MER_ONE_KEENWHEEL;
	m_rect_info[2].m_pt = pt;
	
	m_rect_info[3].m_dwID = SI_MER_ONE_BLOODYSTRIKE;
	m_rect_info[3].m_pt = pt;
	m_rect_info[3].m_pt.y+=24*1+1*2;
	
	m_rect_info[4].m_dwID = SI_MER_SHIELD_PANBARRIER;
	m_rect_info[4].m_pt = pt;
	m_rect_info[4].m_pt.y+=24*2+2*2;
	
	m_rect_info[5].m_dwID = SI_MER_SUP_IMPOWERWEAPON;
	m_rect_info[5].m_pt = pt;
	m_rect_info[5].m_pt.y+=24*3+3*2;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
	pt = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[6].m_dwID = SI_MER_ONE_BLINDSIDE;
	m_rect_info[6].m_pt = pt;
	
	m_rect_info[7].m_dwID = SI_MER_ONE_REFLEXHIT;
	m_rect_info[7].m_pt = pt;
	m_rect_info[7].m_pt.y+=24*1+1*2;
	
	m_rect_info[8].m_dwID = SI_MER_ONE_SNEAKER;
	m_rect_info[8].m_pt = pt;
	m_rect_info[8].m_pt.y+=24*2+2*2;
	
	m_rect_info[9].m_dwID = SI_MER_SUP_SMITEAXE;
	m_rect_info[9].m_pt = pt;
	m_rect_info[9].m_pt.y+=24*3+3*2;
	
	m_rect_info[10].m_dwID = SI_MER_SUP_BLAZINGSWORD;
	m_rect_info[10].m_pt = pt;
	m_rect_info[10].m_pt.y+=24*4+4*2;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
	pt = lpWndCtrl->rect.TopLeft();
	
	m_rect_info[11].m_dwID = SI_MER_ONE_SPECIALHIT;
	m_rect_info[11].m_pt = pt;
	
	m_rect_info[12].m_dwID = SI_MER_ONE_GUILOTINE;
	m_rect_info[12].m_pt = pt;
	m_rect_info[12].m_pt.y+=24*1+1*2;
	
	m_rect_info[13].m_dwID = SI_MER_SUP_AXEMASTER;
	m_rect_info[13].m_pt = pt;
	m_rect_info[13].m_pt.y+=24*3+3*2;
	
	m_rect_info[14].m_dwID = SI_MER_SUP_SWORDMASTER;
	m_rect_info[14].m_pt = pt;
	m_rect_info[14].m_pt.y+=24*4+4*2;

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON31);
	pButton->EnableWindow(FALSE);	
	
	MoveParentCenter();
}
BOOL CWndReSkillPoint_Me::Process()
{
	int nMerArry[30] = { SI_MER_ONE_SPLMASH, WIDC_STATIC7,
		SI_MER_SHIELD_PROTECTION, WIDC_STATIC6,
		SI_MER_ONE_KEENWHEEL, WIDC_STATIC3,
		SI_MER_ONE_BLOODYSTRIKE, WIDC_STATIC4,
		SI_MER_SHIELD_PANBARRIER, WIDC_STATIC2,
		SI_MER_SUP_IMPOWERWEAPON, WIDC_STATIC5,
		SI_MER_ONE_BLINDSIDE, WIDC_STATIC13,
		SI_MER_ONE_REFLEXHIT, WIDC_STATIC9,
		SI_MER_ONE_SNEAKER, WIDC_STATIC12,
		SI_MER_SUP_SMITEAXE, WIDC_STATIC10,
		SI_MER_SUP_BLAZINGSWORD, WIDC_STATIC11,
		SI_MER_ONE_SPECIALHIT, WIDC_STATIC8,
		SI_MER_ONE_GUILOTINE, WIDC_STATIC15,
		SI_MER_SUP_AXEMASTER, WIDC_STATIC14,
		SI_MER_SUP_SWORDMASTER, WIDC_STATIC16 };

	LPSKILL pSkill;
	CWndStatic* pStatic;
	char buffer[10] = { 0 };
	
	for( int i=0; i<30; i+=2 )
	{
		pSkill = GetSkillArry( nMerArry[i] );
		
		if(pSkill)
		{
			pStatic = (CWndStatic *)GetDlgItem(nMerArry[i+1]);
			itoa( pSkill->dwLevel, buffer, 10 );
			pStatic->SetTitle(buffer);
		}
	}
/*	
	CWndStatic*	pWndStatic = (CWndStatic *)GetDlgItem(WIDC_STATIC17);
	itoa( m_dwTotalSkillPoint, buffer, 10 );
	pWndStatic->SetTitle(buffer);
*/		
	return TRUE;
}	
void CWndReSkillPoint_Me::OnMouseWndSurface( CPoint point )
{
	CRect rect;
	
	for( int i=0; i<15; i++ )
	{
		rect.left = m_rect_info[i].m_pt.x;
		rect.top  = m_rect_info[i].m_pt.y;
		
		rect.right  =rect.left+24;
		rect.bottom =rect.top+24;
		
		if( rect.PtInRect(point) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect );
			
			PutToolTip_Skill( m_rect_info[i].m_dwID, point2, &rect );
		}
		
	}	
}	





CWndReSkillPoint_Text::CWndReSkillPoint_Text()
{
}
CWndReSkillPoint_Text::~CWndReSkillPoint_Text()
{
}
BOOL CWndReSkillPoint_Text::Initialize( CWndBase* pWndParent, DWORD nType )
{
	if( g_pPlayer == NULL )
		return FALSE;
	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILL_TEXT, 0, CPoint( 0, 0 ), pWndParent );
}
void CWndReSkillPoint_Text::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CString str;
//	str += '\n';
	str += "  ";
	str += prj.GetText(TID_RESKILLPOINT_TEXT1);

	str += '\n';
	str += '\n';
	str += "  ";
	str += prj.GetText(TID_RESKILLPOINT_TEXT2);

	str += '\n';
	str += "  ";
	str += prj.GetText(TID_RESKILLPOINT_TEXT3);
	
	str += '\n';
	str += "  ";
	str += prj.GetText(TID_RESKILLPOINT_TEXT4);

	pWndText->m_string.SetString( str );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
}
BOOL CWndReSkillPoint_Text::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( nID == WTBID_CLOSE )
		g_Option.m_bIsShowReSkillMessage = FALSE;

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


