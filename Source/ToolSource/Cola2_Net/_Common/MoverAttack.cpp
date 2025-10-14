#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "resdata.h"
#include "defineObj.h"
#include "Party.h"
#ifdef __CLIENT			
	#include "Dialogmsg.h"
#endif

#include "eveschool.h"
#include "Mover.h"
#include "defineskill.h"

#ifdef __ENABLE_AMX041116
	#include "..\_AMX\srun.h"
#endif

#ifdef __WORLDSERVER
	#include "user.h"
	#include "dpcoreclient.h"
	#include "dpdatabaseclient.h"
	#include "DPSrvr.h"
	#include "AttackArbiter.h"

	extern	CDPSrvr		g_DPSrvr;
	extern	CUserMng			g_UserMng;
	extern	CDPCoreClient		g_DPCoreClient;
	extern	CDPDatabaseClient	g_dpDBClient;
	extern	CDPCoreClient		g_DPCoreClient;

#ifdef __GUILDCOMBAT
#include "eveschool.h"
	extern	CGuildCombat	g_GuildCombatMng;
#endif // __GUILDCOMBAT
#endif	// __WORLDSERVER

extern	CPartyMng				g_PartyMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __BALANCE_050614
	const int MIN_HR = 20;
#else
	const int MIN_HR = 30;
#endif


//////////////////////////////////////////////////////////////////////////
// ENUM
//////////////////////////////////////////////////////////////////////////

/*
		cell input[2];
		cell result;

		amx_ctof( input[0] ) = (float)GetStr();
		amx_ctof( input[1] ) = (float)GetLevel();

		Call_AMX( SCRIPT_GETDAMAGEBASE, &result, 2, input );		
		nDamage = (int)amx_ctof( result );
*/
enum 
{
	SCRIPT_GETATTACKRESULT,
	SCRIPT_GETCRITICALPROBABILITY,
	SCRIPT_GETDAMAGEBASE,
	SCRIPT_ISBLOCKING,
	SCRIPT_CALCDEFENSE,
};

//////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTION
//////////////////////////////////////////////////////////////////////////

// 공속 가중치를 구한다. n - 가중치를 구하기 위한 중간 계산값 
float GetAttackSpeedPlusValue( int n )
{
	const MAX_ATTACK_SPEED_PLUSVALUE = 18;

	int nIndex = n / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );

	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};

	return fPlusValue[nIndex];
}

// 충전레벨에 의한 공격력 증폭값을 얻는다.
float GetChargeMultiplier( int nLevel )
{
	//TRACE("GetChargeMultiplier(%d)\n", nLevel );
	ASSERT( nLevel <= 4 );

	static float factors[5] = { 1.0f, 1.2f, 1.5f, 1.8f, 2.2f };
	if( 0 <= nLevel && nLevel <= 4 )
		return factors[ nLevel ];

	return 1.0f;
}

// 충전레벨( nLevel )에 의한 완드공격력 증폭값을 얻는다.
float GetWandATKMultiplier( int nLevel )
{
//	#ifdef __ACROBAT_0504
//		return GetChargeMultiplier( nLevel );
//	#endif

	switch( nLevel )  
	{
	case 0:	return 0.6f;	
	case 1:	return 0.8f;
	case 2: return 1.05f;
	case 3: return 1.1f;
	case 4: return 1.3f;
	}
	return 1.0f;
}

//////////////////////////////////////////////////////////////////////////
// CMover ATTACK RELATED MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////
int GetWeaponPlusDamage( int nDamage, BOOL bRandom, ItemProp* pItemProp , int nOption )
{
	ASSERT( pItemProp );

	int nPlus = 0;

#ifdef __BALANCE_050614
	return nPlus;		// 방어력 무시가 아니라 공격력을 높이는 방향으로 수정되었음 
#else
	static int nAdds[] = {0, 10, 14, 18, 22, 26, 30, 34, 36, 40, 45};		
	if( nOption > 5 || nDamage > 0 )
		nPlus = nAdds[ nOption ];
#endif // __BALANCE_050614
	
	if( nOption > 5 ) // +6 이상 아이템에서 추가 랜덤 데미지
	{
		int nMin, nMax;
		
		if( pItemProp->dwItemRare <= 20 )
		{
			nMin = 1; nMax = 6;
		}
		else if( pItemProp->dwItemRare <= 40 )
		{
			nMin = 5; nMax = 10;
		}
		else if( pItemProp->dwItemRare <= 60 )
		{
			nMin = 9; nMax = 15;
		}
		else if( pItemProp->dwItemRare <= 80 )
		{
			nMin = 14; nMax = 25;
		}
		else 
		{
			nMin = 24; nMax = 35;
		}
		
		if( bRandom )
			nPlus += xRandom( nMin, nMax );
		else
			nPlus += ( (nMin + nMax)/2.0f );
	}
	return nPlus;	
}


// 들고있는 무기의 옵션에 의한 추가 데미지를 구한다. ( bRandom = TRUE )
int CMover::GetWeaponPlusDamage( int nDamage, BOOL bRandom )
{
	int nPlus = 0;
	CItemElem* pWeapon = GetWeaponItem();
	if( pWeapon )
	{
		int nOption = pWeapon->GetAbilityOption();	
		if( nOption > 10 )
			return nPlus;
		
		ItemProp* pItemProp = GetActiveHandItemProp();
		return ::GetWeaponPlusDamage( nDamage, bRandom, pItemProp, nOption ); 
	}
	return nPlus;
}

// 공격에 의해서 날아갈 수 있는가?
BOOL CMover::CanFlyByAttack()
{
	if( IsFlyingNPC() || m_pActMover->IsFly() )
		return FALSE;

	BOOL bFly = FALSE;
	if( (m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) == 0 )	// 데미지플라잉 동안엔 추가로 날아가지 않는다.
	{
		MoverProp* pProp = GetProp();
		if( pProp->dwClass != RANK_SUPER && pProp->dwClass != RANK_MATERIAL )	// 슈퍼/자원 몬스터는 날아가지 않음.
			bFly = TRUE;
	}
	return bFly;
}
// 공속을 구한다.
float CMover::GetAttackSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;

	ItemProp* pWeaponProp = GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;

	JobProp* pProperty = prj.GetJobProp( GetJob() ); 
	ASSERT( pProperty );

	// A = int( 캐릭터의 공속 + ( 무기의 공속 * ( 4 * 덱스 + ( 레벨 / 8 ) ) ) - 3 )
	// 공격속도 = ( ( 50 / 200 - A ) / 2 ) + 가중치 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * GetDex() + GetLevel() / 8.0f ) ) - 3.0f );
//	TRACE( "A =%d\n", A );
	if( 187.5f <= A )
		A	= 187.5f;
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + GetAttackSpeedPlusValue( A );

	float fDstParam = GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
//	TRACE( "%f\n", fSpeed );

	return fSpeed;
}

#ifdef __NEWWPN0901
float CMover::GetCastingAniSpeed( void )
{
	float fSpeed	= 1.0F + GetParam( DST_SPELL_RATE, 0 ) / 100.0F;
	if( fSpeed < 0.1f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	return fSpeed;
}

int CMover::GetCastingTime( int nCastingTime )
{
	return nCastingTime - nCastingTime * GetParam( DST_SPELL_RATE, 0 ) / 100;
}
#endif	// __NEWWPN0901

// 아이템에의한 방어력 얻기 
// bRandom: TRUE  - random 값으로 얻기 
//          FALSE - 평균값으로 defense값 얻기 
int CMover::GetDefenseByItem( BOOL bRandom )
{
	int nValue = m_nDefenseMax - m_nDefenseMin;
	if( bRandom )
		return m_nDefenseMin + ( nValue > 0 ? xRandom( nValue ) : 0 );
	else
		return m_nDefenseMin + ( nValue / 2 );
}

int	CMover::GetShowDefense( BOOL bRandom )
{
	ATTACK_INFO info;
	memset( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;

	return CalcDefense( &info, FALSE );   // FALSE - 방어 최대/최소의 평균값을 얻겠다.      
}

// Hit rating
int CMover::GetHR()
{
#ifdef __BALANCE_050614
	if( IsPlayer() )
		return GetDex();
	else
		return GetProp()->dwHR;
#else
	if( IsPlayer() )
		return GetDex() + GetAdjHitRate();
	else
		return GetProp()->dwHR;
#endif
}

BOOL CMover::GetAttackResult( CMover* pDefender, DWORD dwOption )
{
	if( m_dwMode & ONEKILL_MODE )
		return TRUE;

	int nHitRate = 0;
	int nHR = GetHR();

#ifdef __BALANCE_050614
	#ifdef __PVP_V2
		if( IsNPC() && pDefender->IsPlayer() )
		{
			nHitRate = ((nHR*1.5f) / (float)(nHR + pDefender->GetDex() * 0.5f)) * 2.0f *
					   (GetLevel()*0.5f / (float)(GetLevel() + pDefender->GetLevel() * 0.3f )) * 100.0f; 
		}
		else if( IsPlayer() && pDefender->IsNPC() )	// Player VS Monster
		{
			nHitRate = ((nHR*1.6f) / (float)(nHR + pDefender->GetParrying())) * 1.5f * 
					   (GetLevel()*1.2f / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f; 

		}
		else
		{
			nHitRate = ((nHR*1.6f) / (float)(nHR + pDefender->GetParrying())) * 1.2f * 
					   (GetLevel()*1.2f / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f; 

		}
	#else // __PVP_V2
		if( IsNPC() && pDefender->IsPlayer() )		// Monster VS Player
		{
			nHitRate = ((nHR*1.5f) / (float)(nHR + pDefender->GetDex() * 0.5f)) * 2.0f *
					   (GetLevel()*0.5f / (float)(GetLevel() + pDefender->GetLevel() * 0.3f )) * 100.0f; 

		}
		else if( IsPlayer() && pDefender->IsNPC() )	// Player VS Monster
		{
			nHitRate = ((nHR*1.6f) / (float)(nHR + pDefender->GetParrying())) * 1.5f * 
					   (GetLevel()*1.2f / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f; 
		}
		else	// Player VS Player 
		{
			nHitRate = ((nHR / 1.2f) / (float)(nHR + pDefender->GetParrying())) * 1.2f * 
					   (GetLevel() / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f; 
		}
	#endif
	nHitRate += GetAdjHitRate();
#else
	if( IsNPC() && pDefender->IsPlayer() )
	{
		nHitRate = ((nHR*1.5f) / (float)(nHR + pDefender->GetDex() * 0.5f)) * 2.0f *
			       (GetLevel()*0.5f / (float)(GetLevel() + pDefender->GetLevel() * 0.3f )) * 100.0f; 
	}
	else
	{
		// GetParrying() == ER
		nHitRate = ((nHR*1.6f) / (float)(nHR + pDefender->GetParrying())) * 1.5f * 
			       (GetLevel()*1.2f / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f; 
	}
#endif

	if( nHitRate > 96 )
		nHitRate = 96;
	else if( nHitRate < MIN_HR )
		nHitRate = MIN_HR;

	return ( xRandom( 100 ) < nHitRate );
}

// 무기의 추가 공격력를 구한다.
int CMover::GetPlusWeaponATK( DWORD dwWeaponType )
{
	int nATK = 0;
	switch( dwWeaponType )
	{
	case WT_MELEE_SWD:	
		nATK = GetParam( DST_SWD_DMG, 0 );		
		break;
	case WT_MELEE_AXE:	
		nATK = GetParam( DST_AXE_DMG, 0 );		
		break;
	case WT_MELEE_KNUCKLE:
		nATK = GetParam( DST_KNUCKLE_DMG, 0 );		
		break;
	#ifdef __ACROBAT_0504
		case WT_MELEE_YOYO:				// 요요 
			nATK = GetParam( DST_YOY_DMG, 0 );		
			break;
		case WT_RANGE_BOW:				// 보우 
			nATK = GetParam( DST_BOW_DMG, 0 );		
			break;
	#endif	// __ACROBAT_0504
	}
	return nATK;
}



// 직업에 따른 factor를 구한다.
float CMover::GetJobPropFactor( JOB_PROP_TYPE type )
{
	if( IsPlayer() == FALSE )
		return 1.0f;

	JobProp* pProperty = prj.GetJobProp( GetJob() ); 
	ASSERT( pProperty );

	switch( type )
	{
	case JOB_PROP_SWD:
		return pProperty->fMeleeSWD;
	case JOB_PROP_AXE:	
		return pProperty->fMeleeAXE;
	case JOB_PROP_STAFF:
		return pProperty->fMeleeSTAFF;
	case JOB_PROP_STICK:
		return pProperty->fMeleeSTICK;
	case JOB_PROP_KNUCKLE:
		return pProperty->fMeleeKNUCKLE;
	case JOB_PROP_WAND:
		return pProperty->fMagicWAND;
	case JOB_PROP_YOYO:
		return pProperty->fMeleeYOYO;
	case JOB_PROP_BLOCKING:
		return pProperty->fBlocking;
	case JOB_PROP_CRITICAL:
		return pProperty->fCritical;	
	// TODO 나머지 
	default:
		ASSERT( 0 );
		return 1.0f;
	}
}

// 무기의 공격력를 구한다.
int CMover::GetWeaponATK( DWORD dwWeaponType )
{
	int nATK = 0;
	switch( dwWeaponType )
	{
	case WT_MELEE_SWD:
		nATK = float(( GetStr() - 12 ) * GetJobPropFactor(JOB_PROP_SWD)) + (float(GetLevel() * 1.1f));
		break;
	case WT_MELEE_AXE:
		nATK = float(( GetStr() - 12 ) * GetJobPropFactor(JOB_PROP_AXE)) + (float(GetLevel() * 1.2f));
		break;
	case WT_MELEE_STAFF:
		nATK = float(( GetStr() - 10 ) * GetJobPropFactor(JOB_PROP_STAFF)) + (float(GetLevel() * 1.1f));
		break;
	case WT_MELEE_STICK:
		nATK = float(( GetStr() - 10 ) * GetJobPropFactor(JOB_PROP_STICK)) + (float(GetLevel() * 1.3f));
		break;
	case WT_MELEE_KNUCKLE:
		nATK = float( (GetStr() - 10 ) * GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(GetLevel() * 1.2f));
		break;
	case WT_MAGIC_WAND:
		nATK = ( GetInt() - 10 ) * GetJobPropFactor(JOB_PROP_WAND) + GetLevel() * 1.2f;
		break;
	#ifdef __ACROBAT_0504
		case WT_MELEE_YOYO:
			nATK = float(( GetStr() - 12 ) * GetJobPropFactor(JOB_PROP_YOYO)) + (float(GetLevel() * 1.1f));
			break;
		case WT_RANGE_BOW:
			nATK = (((GetDex()-14)*4.0f + (GetLevel()*1.3f) + (GetStr()*0.2f)) * 0.7f);
			break;
	#endif // __ACROBAT_0504
	}

	nATK += GetPlusWeaponATK( dwWeaponType );	// 무기의 추가 공격력를 구한다.
	return nATK;
}


// 공격력Min,Max를 구한다.
void CMover::GetHitMinMax( int* pnMin, int* pnMax, int nParts )
{
	*pnMin = 0;
	*pnMax = 0;

	if( IsInvalidObj(this) )	
		return;

	if( IsPlayer() )
	{
		ItemProp* pItemProp = GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;

		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;

		*pnMin = GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = GetParam( DST_ABILITY_MAX, *pnMax );

		int nPlus = GetWeaponATK( pItemProp->dwWeaponType ) + GetParam( DST_CHR_DMG, 0 );
		*pnMin += nPlus;
		*pnMax += nPlus;

		CItemElem *pWeapon = GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}

		#ifdef __BALANCE_050614	
			if( pWeapon )
			{
				int nOption = pWeapon->GetAbilityOption();
				if( nOption > 0 )
				{
					int nValue = (int)pow( nOption, 1.5 );
					*pnMin += nValue;
					*pnMax += nValue;
				}
			}
		#endif
	} 
	else
	{
		MoverProp* pMoverProp = GetProp();
		*pnMin = pMoverProp->dwAtkMin;
		*pnMax = pMoverProp->dwAtkMax;

		*pnMin = GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = GetParam( DST_ABILITY_MAX, *pnMax );
		
		*pnMin = GetParam( DST_CHR_DMG, *pnMin );
		*pnMax = GetParam( DST_CHR_DMG, *pnMax );		
		
		if( pMoverProp->dwAtk1 != NULL_ID )		// 들고있는 무기가 있을땐 그 무기의 min,max 값까지 더한다.
		{
			ItemProp* pItemProp = prj.GetItemProp( pMoverProp->dwAtk1 );
			if( pItemProp )
			{
				*pnMin += pItemProp->dwAbilityMin;
				*pnMax += pItemProp->dwAbilityMax;
			}
		}
	}
}

// 방어력증폭값을 얻는다.    this는 방어자
float CMover::GetDEFMultiplier( ATTACK_INFO* pInfo )
{
	float fFactor = 1.0f;
	if( IsNPC() )
	{
	#ifdef __S1108_BACK_END_SYSTEM
		 fFactor *= GetProp()->m_fDefence_Rate;
	#endif //	__S1108_BACK_END_SYSTEM
	}

	// 아머 페너트레이트는 맞는순간 방어력을 50%로 계산.
	if( pInfo->GetSkill() == SI_BLD_DOUBLE_ARMORPENETRATE )	
		fFactor *= 0.5f;

	fFactor		*= (  ( 100.0f + (float)GetParam( DST_ADJDEF_RATE, 0 ) ) / 100.0f ) ;		// 최종 반환된 방어력 값에 방어력 상승 비율을 곱한다.

	return fFactor;
}

// 방어력을 계산한다.
int CMover::CalcDefense( ATTACK_INFO* pInfo, BOOL bRandom )
{
	int nDefense = CalcDefenseCore( pInfo->pAttacker, pInfo->dwAtkFlags, bRandom );
	nDefense *= GetDEFMultiplier( pInfo );
	return nDefense;
}

// 몬스터 방어력을 계산한다. ( generic과 magicskill은 계산됨 )
int CMover::CalcDefenseNPC( CMover* pAttacker, DWORD dwAtkFlags )
{
#if defined(__LEVELCLASS_EXP)
	const float fMagicFactor		= 0.085f; 
	const float fMeleeSkillFactor	= 0.1f;
#else
	const float fMagicFactor		= 0.04f; 
	const float fMeleeSkillFactor	= 0.1f;	// 예전에는 0.05f
#endif

	float	fDefense = 0.0f;
	float	fDivisor = 1.0f;
	int		nDelta   = GetLevel() - pAttacker->GetLevel();
	
	MoverProp* pProp = GetProp();
	if( dwAtkFlags & AF_MAGIC ) 
	{
		fDefense = pProp->dwResisMgic / 7.0f + 1;
		fDivisor = fMagicFactor;
	}
	else
	{
		ASSERT( (dwAtkFlags & AF_MELEESKILL) == AF_MELEESKILL );
		fDefense = pProp->dwNaturalArmor / 7.0f + 1;
		fDivisor = fMeleeSkillFactor;
	}
#if !defined( __BALANCE_LEVEL_050706 )
	if( nDelta > 0 ) 
		fDefense *= ( 1.0f + ( nDelta / fDivisor ) );
#endif

	return (int)fDefense;
}

// 플레이어 방어력을 계산한다. ( generic, magicskill은 상위에서 계산됨 )
int CMover::CalcDefensePlayer( CMover* pAttacker, DWORD dwAtkFlags )
{	
	int nDefense = 0;
	if( pAttacker->IsPlayer() )		// 플레이어 공격 
	{			
		if( dwAtkFlags & AF_MAGIC )  
		{
			nDefense = float((GetInt() * 9.04f) + (GetLevel() * 35.98f) );
		}
		else
		{
			int nDEF = GetDefenseByItem() + GetParam( DST_ADJDEF, 0 );
			nDefense = (float(nDEF * 2.3f ) + (float(GetLevel()+(GetSta()/2)+GetDex()) / 2.8f ) - 4 + GetLevel() * 2 );
		}
	}
	else							// 몹공격
	{		
		nDefense = ((GetDefenseByItem() / 4 + GetParam( DST_ADJDEF, 0 )) + 
			(float(GetLevel()+(GetSta()/2)+GetDex()) / 2.8f ) - 4 + GetLevel() * 2 );
	}
	return nDefense;
}

// CalcDefense에서 호출되는 sub함수 
int CMover::CalcDefenseCore( CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom )
{
	if( dwAtkFlags & AF_MAGICSKILL )		// 예외처리:속성 매직스킬의 방어력을 구한다.
		return GetResistMagic();

#if defined(__PVP_V2)
	BOOL bGeneric = (dwAtkFlags & AF_GENERIC);
	if( IsPlayer() && pAttacker && pAttacker->IsPlayer() )
		bGeneric = TRUE;
#else
	BOOL bGeneric = (dwAtkFlags & AF_GENERIC);
#endif

	if( bGeneric )		
	{
		float fFactor = 1.0f;
		if( IsPlayer() )
		{
			JobProp* pProperty = prj.GetJobProp( GetJob() ); 
			assert( pProperty );
			fFactor = pProperty->fFactorDef;
		}
		int nDefense = ((((GetLevel()*2) + (GetSta()/2)) / 2.8f ) - 4) + ((GetSta()-14) * fFactor);
		nDefense += (GetDefenseByItem( bRandom ) / 4);	// 아이템에 의한 디펜스   
		nDefense += GetParam( DST_ADJDEF, 0 );	// 디펜스 수정치가 있다면 그것을 더함.
		return nDefense;
	}

	if( IsNPC() )
		return CalcDefenseNPC( pAttacker, dwAtkFlags );
	else
		return CalcDefensePlayer( pAttacker, dwAtkFlags );
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef __WORLDSERVER

int CMover::CalcLinkAttackDamage( int nDamage )
{
	int nAdd = 0;
	if( IsPlayer() && m_idparty && nDamage > 0 ) // 내가 파티에 참여 여부
	{
		g_PartyMng.Lock( theLineFile );
		CParty* pParty = g_PartyMng.GetParty( m_idparty );
		if( pParty && pParty->IsMember( m_idPlayer ) )
		{
			// 순회극단이며 링크어택중이며 단장만 데미지를 높여준다
			if( pParty->m_nKindTroup == 1 && pParty->m_nModeTime[PARTY_LINKATTACK_MODE] && pParty->IsLeader( m_idPlayer ) )
			{
				int nJoinMember = 1; // 참여한 단원수
				CUser* pLeader = (CUser*)this;
				CUser* pMemberUser;
				
				float fDist;
				D3DXVECTOR3	vDist;
				
				for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
				{
					pMemberUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
					if( IsValidObj( (CObj*)pMemberUser )/* && pLeader->IsNearPC( pMemberUser ) && pMemberUser->IsOrigin()*/ )
					{
						if( pLeader->m_idSetTarget == pMemberUser->m_idSetTarget )
						{
							if( pLeader->IsSMMode( SM_PARTYSKILL15 ) || pLeader->IsSMMode( SM_PARTYSKILL30 ) )	// 여기서 유료 아이템 사용중인지 확인
								nJoinMember++; // 참여한 단원수										
							else
							{
								vDist = pMemberUser->GetPos() - GetPos();
								fDist = D3DXVec3LengthSq( &vDist );		// 아이템 줍는사람과 멤버간의 거리.
								if( fDist < 255.0f * 255.0f )		// xx미터보다 작으면
									nJoinMember++; // 참여한 단원수										
							}
						}
					}
				}
				nAdd = ( nDamage * nJoinMember / 10 ); 
			}
		}
		g_PartyMng.Unlock( theLineFile );	// 더 이상 파티를 참조할 필요가 없다.
	}
	return nAdd;
}

#ifdef __NEWWPN0818
int CMover::GetCriticalProb( void )
{
	int nProb	= ( GetDex() / 10 );
	nProb	*= GetJobPropFactor( JOB_PROP_CRITICAL );
	nProb	= GetParam( DST_CHR_CHANCECRITICAL, nProb );
	return nProb;
}
#endif	// __NEWWPN0818

// 크리티컬 공격인가?
BOOL CMover::IsCriticalAttack( CMover* pDefender, DWORD dwAtkFlags )
{
	if( IsSkillAttack( dwAtkFlags ) )	// 스킬이면 critical 없음 
		return FALSE;

	int nProb;

#ifdef __ENABLE_AMX041116
	cell input[3];
	cell result;

	input[0] = GetDex();
	input[1] = GetLevel();
	input[2] = pDefender->GetLevel();

	Call_AMX( SCRIPT_GETCRITICALPROBABILITY, &result, 3, input );
	nProb = result;
#else	// __ENABLE_AMX041116
	nProb = (GetDex() / 10);
#endif // __ENABLE_AMX041116

#ifdef __ACROBAT_0504
#ifdef __NEWWPN0818
	nProb *= GetJobPropFactor( JOB_PROP_CRITICAL );
	nProb = GetParam( DST_CHR_CHANCECRITICAL, nProb );	// 크리티컬 확률을 높여주는 스킬관련 
#else	// __NEWWPN0818
	nProb = GetParam( DST_CHR_CHANCECRITICAL, nProb );	// 크리티컬 확률을 높여주는 스킬관련 
	nProb *= GetJobPropFactor( JOB_PROP_CRITICAL );
#endif	// __NEWWPN0818
#endif	// __ACROBAT_0504

	if( IsPlayer() )
	{
		if( m_idparty && (m_dwFlag & MVRF_CRITICAL) )	// 파티 && 1회 크리티컬 발동?
		{
			g_PartyMng.Lock( theLineFile );
			CParty* pParty = g_PartyMng.GetParty( m_idparty );
			if( pParty && pParty->IsMember( m_idPlayer ) )
				nProb += (pParty->m_nSizeofMember / 2);	
			g_PartyMng.Unlock( theLineFile );	

			m_dwFlag &= (~MVRF_CRITICAL);
		}

		if( IsAfterDeath() )							// 죽음 이후 상태라면?
			nProb += CRITICAL_AFTER_DEATH;
				
		int nHitPercent = GetHitPointPercent( 100 );
		if( nHitPercent < CRITICAL_BERSERK_HP )			// HP가 MAX대비 30% 미만? 
		{
			// CRITICAL_BERSERK_HP : nHitPercent = CRITICAL_BERSERK_PROB : x
			// 30 : 15 = 20 : x
			nProb += CRITICAL_BERSERK_PROB - ( nHitPercent * CRITICAL_BERSERK_PROB / CRITICAL_BERSERK_HP );
		}
	}
	return ( xRandom(100) < nProb );		
}


float CMover::GetBlockFactor( CMover* pAttacker, ATTACK_INFO* pInfo )
{
	if( IsPlayer() )
	{
		int r = xRandom( 80 );
		if( r <= 5 )
			return 1.0f;	// Blocking 실패
		if( r >= 75 )
			return 0.1f;	// 10으로 나눈다.

		float fBlockA = GetLevel() / ( (GetLevel()+pAttacker->GetLevel())*15.0f );
		float fBlockB = (GetDex()+pAttacker->GetDex()+2) * ( (GetDex()-pAttacker->GetDex()) / 800.0f );
		if( fBlockB > 10.0f )
			fBlockB = 10.0f;
		float fAdd = fBlockA + fBlockB;
		if( fAdd < 0.0f )
			fAdd = 0.0f;

	#ifdef __XSKILL0425
		if( pInfo->IsRangeAttack() )	// 원거리평타일때는 
			fAdd += GetParam( DST_BLOCK_RANGE, 0 );		// 원거리블럭율 보정치를 가져다 씀
		else
			fAdd += GetParam( DST_BLOCK_MELEE, 0 );		// 원거리가 아닐땐 근거리 블럭율보정치를 가져다 씀
	#else
		fAdd += GetParam( DST_BLOCK, 0 );		// 
	#endif
		int nBR = ( GetDex() / 8.0f ) * GetJobPropFactor( JOB_PROP_BLOCKING ) + fAdd;
		if( nBR < 0 )
			nBR = 0; 

		if ( nBR > r )		// R이 6 ~ 74사이의 값이면 ->  BR > R인 경우 Blocking 성공
			return 0.0f;
	}
	else
	{
		int r = xRandom( 100 );
	
		if( r <= 5 )			// R이 0 ~ 5의 값이면 Blocking 실패
			return FALSE;
		if( r >= 95 )			// R이 95 ~ 100의 값이면 Blocking 성공
			return TRUE;

		int	nBR = ( GetParrying() - GetLevel() ) * 0.5f;  // GetParrying == ER
		if( nBR < 0 )
			nBR = 0; 

		if ( nBR > r )		// R이 6 ~ 94사이의 값이면 ->  BR > R인 경우 Blocking 성공
			return 0.2f;	// 5로 나눈다.
	}
	return 1.0f;
}

BOOL CMover::IsBlocking( CMover* pAttacker )
{
#ifdef __ENABLE_AMX041116
	cell input[4];
	cell result;

	input[0] = xRandom( 100 );
	input[1] = GetDex();
	input[2] = GetLevel();
	input[3] = pAttacker->GetLevel();

	Call_AMX( SCRIPT_ISBLOCKING, &result, 4, input );		
	return result;
#else
	int r = xRandom( 100 );
	
	if( r <= 5 )			// R이 0 ~ 5의 값이면 Blocking 실패
		return FALSE;
	if( r >= 95 )			// R이 95 ~ 100의 값이면 Blocking 성공
		return TRUE;

	//몬스터 Br = (몬스터ER - 몬스터LV)*0.5
	//플레이어 BR = ((캐릭터Dex - 30) / (2 * Lv)) + (① + ②)
	int nBR = 0;

	if( IsPlayer() )
	{
		// ① : 방패의 블록 수치
		// ② : 아이템과 스킬 추가치의 합(정수, 아이템에 블록킹 값이 정수로 더해진다.)
		CItemElem* pShield = GetEquipItem( PARTS_SHIELD );	
		if( pShield )
		{
			nBR = ( GetDex() - 30 ) / ( 2 * GetLevel() );
			ItemProp* pProp = pShield->GetProp();
			nBR += pProp->dwblockRating;

			//최소 2에서 최대65 사이의 값을 가진다
			if( nBR < 2 )
				nBR = 2;
			else if( nBR > 65 )
				nBR = 65;
		}
		else
			nBR = 0;
	}
	else
	{
		nBR = ( GetParrying() - GetLevel() ) * 0.5f;  // GetParrying == ER
		if( nBR < 0 )
			nBR = 0; 
	}

	return ( nBR > r );		// R이 6 ~ 94사이의 값이면 ->  BR > R인 경우 Blocking 성공
#endif	// !__ENABLE_AMX041116
}

// this -> attacker
// 스피릿밤(SI_PSY_PSY_SPRITBOMB) 
// 자신의 MP가 90% 이상 일경우 공격력의 1.5배
// 자신의 MP가 50% 이상일경우 공격력의 1.2배
// 자신의 MP가 50% 미만일 경우 기본 데미지
float CMover::GetDamageMultiplier( ATTACK_INFO* pInfo )
{
	float factor = 1.0f;
	int nSkill = pInfo->GetSkill(); 

	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;

	if( nSkill )
	{
		pSkillProp = prj.GetSkillProp( nSkill );		// 스킬의 프로퍼티꺼냄
		if( pSkillProp )
		{
			pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pInfo->GetSkillLevel() );
			if( pAddSkillProp && pAddSkillProp->nSkillCount > 0 )		//  발사체 개수가 있는건
				factor /= (float)pAddSkillProp->nSkillCount;	// 발사체 개수만큼 데미지를 나눠서 줌.
		}
	}

#ifdef __ACROBAT_0504
	// 싸이런트샷이면 데미지 2배, 다크일루젼상태이면 해제한다.
	if( m_SkillState.HasSkill( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION ) )
	{
		switch( nSkill )
		{
		case SI_JST_YOYO_BACKSTAB:   //최대 2배
		case SI_JST_YOYO_VATALSTAB:  //최대 4배
		case SI_ACR_BOW_SILENTSHOT:
			{
				factor *= 1.4f;
				break;
			}
		}
	}

	switch( nSkill )
	{
	case SI_JST_YOYO_VATALSTAB:
		if( xRandom( 100 ) < pAddSkillProp->nProbability )
			factor *= 2.0f;
		break;		
	case SI_ACR_BOW_AIMEDSHOT:
		if( xRandom( 100 ) < pAddSkillProp->nProbability )
			factor = 2.0f;
		break;		
	case SI_ACR_BOW_JUNKBOW:		// 엉터리 화살은 확률로 데미지를 준다.
		if( xRandom( 100 ) > pAddSkillProp->nProbability )
			factor = 0.0f;
		break;		
 	case SI_JST_YOYO_HITOFPENYA:
		factor *= (pAddSkillProp->nDestData1[0] / 100.0f);
		break;		
	}
	

#endif //__ACROBAT_0504
	
	if( nSkill == SI_PSY_PSY_SPRITBOMB )
	{
		int nPercent = GetManaPointPercent();
		if( nPercent >= 90 )
			factor *= 1.5f;
		else if( nPercent >= 50 )
			factor *= 1.2f;
	}

	if( IsNPC() )	// 버서커AI가 있다.
	{
		int nBerserkHP = GetProp()->m_nBerserkHP;
		if( nBerserkHP > 0 && GetHitPointPercent( 100 ) <= nBerserkHP )
			factor *= GetProp()->m_fBerserkDmgMul;
	}
	else
	{
	#ifdef __PVP_V2
		if( pInfo->pDefender->IsPlayer() )		// 플레이어와 플레이어의 공격은 데미지 감소 
			factor *= 0.60f;
	#endif // __PVP_V2

		if( pInfo->nParts == PARTS_LWEAPON )	// 왼손은 공격력이 75프로로 
			factor *= 0.75f;
	}

	if( pInfo->pAttacker->IsPlayer() )
		if( pInfo->pDefender->GetAdjParam( DST_CHRSTATE ) & CHS_DOUBLE )		// 더블데미지 상태 걸렸다
			factor *= 2.0f;

#ifdef	__BALANCE_LEVEL_050706
	int nDelta = pInfo->pDefender->GetLevel() - pInfo->pAttacker->GetLevel(); 	

	// 예외처리: 클락워크와 경비병은 레벨차 페널티 skip
	if( pInfo->pAttacker->IsNPC() )
	{
		DWORD dwClass = pInfo->pAttacker->GetProp()->dwClass;
		switch( dwClass )
		{
		case RANK_GUARD:
		case RANK_SUPER:
			nDelta = 0;
			break;
		}
	}

	if( nDelta > 0 )
	{
		if( pInfo->pAttacker->IsPlayer() && pInfo->pDefender->IsPlayer() && GetWorld()->GetID() == WI_WORLD_GUILDWAR )	// 길드대전맵인가? && PVP인가?
		{
			nDelta = min( nDelta, 40 ); 
			const double pi = 3.1415926535;
			double radian = ( pi * nDelta ) / (float)(60 * 2);
			factor *= (float)cos( radian );
		}
		else
		{
			const int MAX_OVER_ATK = 16;
			nDelta = min( nDelta, (MAX_OVER_ATK-1) ); 
			
			const double pi = 3.1415926535;
			double radian = ( pi * nDelta ) / (float)(MAX_OVER_ATK * 2);
			factor *= (float)cos( radian );
		}
	}
#endif // __BALANCE_LEVEL_050706

	return factor;
}





// 일반(AF_MAGICSKILL를 제외한) 속성데미지를 계산한다.
int CMover::CalcPropDamage( CMover* pDefender, DWORD dwAtkFlags )
{
	int nAdd = 0;
	SAI79::ePropType type = SAI79::NO_PROP;
	int nATK = 0;

	if( IsNPC() )	// 공격자가 NPC일 경우	
	{			
		type = GetProp()->eElementType;
		nATK = GetProp()->wElementAtk; 
	}
	else		    // 공격자가 플레이어일 경우
	{		
		CItemElem* pItemElem = GetWeaponItem();
		if( pItemElem && pItemElem->m_bItemResist != SAI79::NO_PROP )
		{
			type = (SAI79::ePropType)pItemElem->m_bItemResist;
			nATK = pItemElem->m_nResistAbilityOption;
		
		#ifdef __THAILAND__   // todo_raiders 제거하자
			float fResult = ((float)nATK+3.5f)*(0.5f+((float)nATK*0.01f));
			int nResult;
			nResult = fResult*fResult;
			nATK    = nResult;
		#else //__THAILAND__
			nATK *= 4;
		#endif //__THAILAND__

			nATK += GetAdjParam( DST_CHR_WEAEATKCHANGE );
		}
		else
		{
			ItemProp* pProp = GetActiveHandItemProp();
			type = pProp->eItemType;
			nATK = pProp->wItemEatk; 
		}
	}

	if( type != SAI79::NO_PROP )	// 속성공격 ?
	{
#ifdef __BALANCE_LEVEL_050706
		nAdd = nATK  * (1.0f - pDefender->GetResist(type));
#else
		float fDelta = pDefender->GetLevel() - GetLevel();
		if( fDelta > 5.0f )
			fDelta = 5.0f;
		else if( fDelta < 0.0f )
			fDelta = 0.0f;
		// (속성 공격력-(속성 공격력*0.2*레벨차)) * (1 - (해당 속성 저항력/100) ))
		nAdd = (nATK - (nATK * 0.2f * fDelta)) * (1.0f - pDefender->GetResist(type) );
#endif
	}
	return nAdd;
}


// AF_MAGICSKILL 속성데미지를 계산한다.
int CMover::GetMagicSkillPower( ATTACK_INFO* pInfo )
{
	CMover* pDefender = pInfo->pDefender; 
	int nSkill = pInfo->GetSkill();
	int nATK = GetMeleeSkillPower( pInfo );
	nATK = GetParam( DST_ADDMAGIC, nATK );		// 추가마법력 수정치.

	ItemProp* pSkillProp = prj.GetSkillProp( nSkill );		// 스킬의 프로퍼티꺼냄

	float fRatio = 0.0f;
	switch( pSkillProp->dwSpellType )
	{
	case ST_FIRE:	
		fRatio = GetParam( DST_MASTRY_FIRE, 0 ) / 100.0f;
		nATK += (nATK * fRatio);
		break;
	case ST_WATER:	
		fRatio = GetParam( DST_MASTRY_WATER, 0 ) / 100.0f;
		nATK += (nATK * fRatio);
		break;
	case ST_ELECTRICITY:	
		fRatio = GetParam( DST_MASTRY_ELECTRICITY, 0 ) / 100.0f;
		nATK += (nATK * fRatio);
		break;
	case ST_WIND:	
		fRatio = GetParam( DST_MASTRY_WIND, 0 ) / 100.0f;
		nATK += (nATK * fRatio);
		break;
	case ST_EARTH:	
		fRatio = GetParam( DST_MASTRY_EARTH, 0 ) / 100.0f;
		nATK += (nATK * fRatio);
		break;
	}

	return nATK;
}


float CMover::GetMagicSkillFactor( CMover* pDefender, SAI79::ePropType skillType )
{
	SAI79::ePropType itemType;
	CItemElem* pWeapon	= GetWeaponItem();
	if( pWeapon && pWeapon->m_bItemResist != SAI79::NO_PROP )
		itemType = (SAI79::ePropType)pWeapon->m_bItemResist;	// 제련속성 
	else
	{
		ItemProp* pItemProp = GetActiveHandItemProp();
		if( pItemProp ) 
			itemType = pItemProp->eItemType;					// 아이템 속성
		else
			return 1.0f;
	}

	if( skillType == itemType )
	{
		return 1.1f;
	}
	else if( ( skillType == 1 && itemType == 2 ) || 
		     ( skillType == 2 && itemType == 3 ) ||
			 ( skillType == 3 && itemType == 5 ) || 
			 ( skillType == 5 && itemType == 4 ) ||
			 ( skillType == 4 && itemType == 1 ) )		
	{
		return 0.9f;
	}
	else
		return 1.0f;
}

int CMover::PostCalcMagicSkill( int nATK, ATTACK_INFO* pInfo )
{
	CMover*				pDefender = pInfo->pDefender;
	int					nSkill = pInfo->GetSkill();
	SAI79::ePropType	skillType = prj.GetSkillProp(nSkill)->eItemType;
	int					nDelta = pDefender->GetLevel() - GetLevel();

#if defined(__TAIWAN__) && !defined(__BALANCE_LEVEL_050706)
	if( nDelta >= 5 && pDefender->IsNPC() )
		return 0;		// 메지션이 자신보다 5레벨 이상의 몹을 사냥할 경우 100%MISS가 나오게
#endif

	float a, b;
	b = 1.0f + (int)( nDelta / 0.05f );
	if( b <= 0.0f )							
		b = 0.0f;

	int nDEF = pDefender->CalcDefense( pInfo );
#ifdef __BALANCE_LEVEL_050706
	a = ( nATK-nDEF ) * (1.0f - pDefender->GetResist(skillType) );
#else
	a = ((nATK-nDEF) - (b * (GetLevel()/4.0f +1.0f))) * (1.0f - pDefender->GetResist(skillType) );
#endif

	return (int)a*GetMagicSkillFactor( pDefender, skillType );
}

// 순수공격력을 증폭한다.
float CMover::GetATKMultiplier( CMover* pDefender, DWORD dwAtkFlags )
{
	float fMultiplier = 1.0f;

#ifdef __PIERCING_SM_TIME
	int nParam = GetParam( DST_ATKPOWER_RATE, 0 );		// 공격력 증가 수정치
	FLOAT fRate = 1.0f + (nParam / 100.0f);
	fMultiplier *= fRate;
#endif

	if( IsPlayer() )
	{
#if !defined( __BALANCE_LEVEL_050706 )
		if( (dwAtkFlags & AF_GENERIC) && pDefender->IsNPC() )
		{
			int nDelta = pDefender->GetLevel() - GetLevel();
			if( nDelta >= 15 )
				fMultiplier *= 0.10f;	// 캐릭터 공격력의 90%가 하락한다.
			else if( nDelta >= 10 )
				fMultiplier *= 0.20f;	// 캐릭터 공격력의 80%가 하락한다.
			else if( nDelta >= 5 )
				fMultiplier *= 0.50f;	// 캐릭터 공격력의 50%가 하락한다.
		}
#endif
		if( IsSMMode( SM_ATTACK_UP ) )
			fMultiplier *= 1.2f;
	}
	else
	{
	#ifdef __S1108_BACK_END_SYSTEM
		fMultiplier *= ( prj.m_fMonsterHitRate * GetProp()->m_fAttackPower_Rate );
	#else 
		fMultiplier *= prj.m_fMonsterHitRate;
	#endif 
	}

	return fMultiplier;
}

// 속성의 공격 추가퍼센트를구한다.
int CMover::GetPropATKPlus( int nParts )
{
	int nPlusATK = 0;
	
	switch( nParts )
	{
	case PARTS_RWEAPON:
		if( m_nAttackResistRight != 255 )
			nPlusATK = 20;
		break;
	case PARTS_LWEAPON:
		if( m_nAttackResistLeft != 255 )
			nPlusATK = 20;
		break;
	}
	return nPlusATK;
}

// 속성의 방어 추가퍼센트를구한다.
int CMover::GetPropDEFPlus()
{
	int nPlusDEF = 0;
	if( m_nDefenseResist != 255 )
		nPlusDEF = 20;
	return nPlusDEF;
}

// GetHitPower함수에서 사용할 ATK, DEF의 증감 Factor를 구한다.
void CMover::GetDamagePropertyFactor( CMover* pDefender, int* pnATKFactor, int* pnDEFFactor, int nParts )
{
	*pnATKFactor = 100;		// 속성 적용이 없으면 ATK를 100% 그대로 사용 
	*pnDEFFactor = 100;		// 속성 적용이 없으면 DEF를 100% 그대로 사용 

	SAI79::ePropType atkType, defType; 
	int atkLevel, defLevel, nPlusATK, nPlusDEF;
	atkType = defType = SAI79::NO_PROP;
	atkLevel = defLevel = 1;
	nPlusATK = nPlusDEF = 0;

	if( IsPlayer() )
	{
		CItemElem* pItemElem = GetWeaponItem( nParts );
		if( pItemElem && pItemElem->m_bItemResist != SAI79::NO_PROP )
		{
			atkType = (SAI79::ePropType)pItemElem->m_bItemResist;
			atkLevel = pItemElem->m_nResistAbilityOption + GetAdjParam( DST_CHR_WEAEATKCHANGE );
		}
		else
		{
			ItemProp* pProp = GetActiveHandItemProp( nParts );
			atkType = pProp->eItemType;
		}	

		nPlusATK = GetPropATKPlus( nParts );
	}
	else
	{
		atkType = GetProp()->eElementType;
	}

	if( atkType == SAI79::NO_PROP )
		return;

	if( pDefender->IsPlayer() )
	{
		CItemElem* pItemElem = pDefender->GetEquipItem( PARTS_UPPER_BODY );	//슈트의 속성을 구한다.
		if( pItemElem )
		{
			defType = (SAI79::ePropType)pItemElem->m_bItemResist;
			defLevel = pItemElem->m_nResistAbilityOption;
		}

		nPlusDEF = GetPropDEFPlus();
	}
	else
	{
		defType = pDefender->GetProp()->eElementType;
	}

	if( defType == SAI79::NO_PROP )
		return;

	// 속성ENUM을 인덱스로 갖는 테이블 
	static int table[SAI79::END_PROP][SAI79::END_PROP] = {
		{0, 0, 0, 0, 0, 0}, // 속성없음 
		{0, 1, 2, 0, 3, 0},	// 불   속성
		{0, 3, 1, 2, 0, 0},	// 물   속성
		{0, 0, 3, 1, 0, 2},	// 전기 속성
		{0, 2, 0, 0, 1, 3},	// 바람 속성
		{0, 0, 0, 3, 2, 1}	// 땅   속성			
	};

#ifdef __ENCHANT0727
	static int anEnchant[11]		= {
		0, 2, 4, 7, 10, 14, 18, 22, 26, 30, 45
	};
#endif	// __ENCHANT0727

	int result = table[ atkType ][ defType ];
	switch( result )
	{
#ifdef __ENCHANT0727
	case 0:
#endif	// __ENCHANT0727
	case 1:				//30% MISS
#ifdef __ENCHANT0727
		*pnATKFactor	+= anEnchant[atkLevel];	// + nPlusATK;
		*pnDEFFactor	+= anEnchant[defLevel];	// + nPlusDEF;
#else	// __ENCHANT0727
		if( xRandom( 100 ) < 30 )
			*pnATKFactor = 0;
#endif	// __ENCHANT0727
		break;
	case 2:				//공격자 열성 
#ifdef __ENCHANT0727
		*pnDEFFactor	+= anEnchant[defLevel] + nPlusDEF;
#else	// __ENCHANT0727
		*pnATKFactor -= (atkLevel + 10 );
		*pnDEFFactor += (defLevel + 5 + nPlusDEF );
#endif	// __ENCHANT0727
		break;
	case 3:				//공격자 우성 
#ifdef __ENCHANT0727
		*pnATKFactor += anEnchant[atkLevel] + nPlusATK;
#else	// __ENCHANT0727
		*pnATKFactor += (atkLevel + 10 + nPlusATK );
		*pnDEFFactor -= (defLevel + 5 );
#endif	// __ENCHANT0727
		break;
	}
}

// 일반 공격 데미지를 구한다.
int CMover::GetHitPower( ATTACK_INFO* pInfo  )
{
	int nMin, nMax, nATKFactor, nATK;
	GetDamagePropertyFactor( pInfo->pDefender, &nATKFactor, &pInfo->nDEFFactor, pInfo->nParts );

	GetHitMinMax( &nMin, &nMax, pInfo->nParts );

	if( IsCriticalAttack( pInfo->pDefender, pInfo->dwAtkFlags ) )
	{
		pInfo->dwAtkFlags |= AF_CRITICAL;

		float fMin = 1.1f;
		float fMax = 1.4f;

		if( GetLevel() > pInfo->pDefender->GetLevel() )  
		{
			fMin = 1.2f;
			fMax = 2.0f;
		#ifdef __BALANCE_LEVEL_050706
			if( IsNPC() )
			{
				fMin = 1.4f;
				fMax = 1.8f;
			}
		#endif // __BALANCE_LEVEL_050706
		}

#ifdef __NEWWPN0818
		float fCriticalBonus	= 1 + (float)GetParam( DST_CRITICAL_BONUS, 0  ) / 100.0F;
		nMin *= fMin * fCriticalBonus;
		nMax *= fMax * fCriticalBonus;
#else	// __NEWWPN0818
		nMin *= fMin;
		nMax *= fMax;
#endif	// __NEWWPN0818

		BOOL bFlyByAttack = xRandom(100) < 30;
		#ifdef __ACROBAT_0504
			ItemProp* pItemProp = GetActiveHandItemProp();			//요요공격이나 반사류는 날리지 않음 
			if( pItemProp && pItemProp->dwWeaponType == WT_MELEE_YOYO || (pInfo->dwAtkFlags & AF_FORCE) )
				bFlyByAttack = FALSE;
		#endif		
		if( pInfo->pDefender->CanFlyByAttack() && bFlyByAttack )
			pInfo->dwAtkFlags |= AF_FLYING;
	}		

	nATK = xRandom( nMin, nMax );						// 공격력에 랜덤적용  
	nATK = MulDiv( nATK, nATKFactor, 100 );				// 속성 상성에의한 팩터를 곱한다. nATKFactor 0-100사이의 퍼센트값  
	if( pInfo->IsRangeAttack() )						// 플레이어의 원거리 공격의 경우 
		nATK *= GetChargeMultiplier( pInfo->GetChargeLevel() );		// 충전레벨에 의한 증폭처리 

//	TRACE("nMin:%d nMax:%d nATK:%d nATKFactor:%d\n", nMin, nMax, nATK, nATKFactor ); 
	return nATK;
}

int CMover::PostCalcGeneric( int nATK, ATTACK_INFO* pInfo )
{
	int nDEF = pInfo->pDefender->CalcDefense( pInfo );  
	nDEF = MulDiv( nDEF, pInfo->nDEFFactor, 100 );

	int nDamage = nATK - nDEF;
	if( nDamage > 0 )
	{
		float fBlock = pInfo->pDefender->GetBlockFactor( this, pInfo );
		if( fBlock < 1.0f )
		{
			pInfo->dwAtkFlags |= AF_BLOCKING;
			nDamage *= fBlock;
		}
	}
	else
	{
		nDamage = 0;
	}

	// NPC의 경우 최소 데미지가 들어가게 한다.
	if( pInfo->pAttacker->IsNPC() && pInfo->pDefender->IsPlayer() )	
	{
		int nMin = max( 0, nATK * 0.1f );	// 몬스터 공격력의 10% 데미지가 무조건 들어갈 수 있게 해 준다.
		nDamage = max( nDamage, nMin );
	}

	nDamage += GetWeaponPlusDamage( nDamage );	// 인첸트 옵션 데미지( 방어력 무관 ) 
	if( nDamage == 0 )
	{
		pInfo->dwAtkFlags &= ~AF_CRITICAL;
		pInfo->dwAtkFlags &= ~AF_FLYING;
	}
	return nDamage;
}

int	CMover::GetMeleeSkillPower( ATTACK_INFO* pInfo )
{
	int nSkill = pInfo->GetSkill();
	int nSkillLv = pInfo->GetSkillLevel();
	//int(무기의AbilityMinMax * (기술의AbilityMinMax + 10) / 6 + 무기의AbilityMinMax)
	ItemProp* pItemProp = GetActiveHandItemProp();			// 들고있는 무기의 프로퍼티 꺼냄
	ItemProp* pSkillProp = prj.GetSkillProp( nSkill );		// 스킬의 프로퍼티꺼냄
	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, nSkillLv );

	CItemElem *pWeapon = GetWeaponItem();		// 손에든 아이템의 포인터.
	int nMin, nMax;
	GetItemATKPower( &nMin, &nMax, pItemProp, pWeapon ); 

	int   nReferStat = 0, nReferStat1 = 0, nReferStat2 = 0;
	switch(pSkillProp->dwReferStat1)
	{
	case DST_STR:		nReferStat1 = GetStr();		break;
	case DST_DEX:		nReferStat1 = GetDex();		break;
	case DST_INT:		nReferStat1 = GetInt();		break;
	case DST_STA:		nReferStat1 = GetSta();		break;
	}
	switch(pSkillProp->dwReferStat2)
	{
	case DST_STR:		nReferStat2 = GetStr();		break;
	case DST_DEX:		nReferStat2 = GetDex();		break;
	case DST_INT:		nReferStat2 = GetInt();		break;
	case DST_STA:		nReferStat2 = GetSta();		break;
	}
	
	if( pSkillProp->dwReferTarget1 == RT_ATTACK )
		if( pSkillProp->dwReferValue1 != NULL_ID )
			nReferStat1 = ((pSkillProp->dwReferValue1/10.0f)*nReferStat1) + (pAddSkillProp->dwSkillLvl * FLOAT(nReferStat1/50.0f));

	if( pSkillProp->dwReferTarget2 == RT_ATTACK )
		if( pSkillProp->dwReferValue2 != NULL_ID )
			nReferStat2 = ((pSkillProp->dwReferValue2/10.0f)*nReferStat2) + (pAddSkillProp->dwSkillLvl * FLOAT(nReferStat2/50.0f));

	nReferStat = nReferStat1 + nReferStat2;
	float fPowerMin, fPowerMax;
	if( pWeapon )
	{
		fPowerMin = (( nMin + ((int)pAddSkillProp->dwAbilityMin + pWeapon->GetProp()->nAddSkillMin ) * 5 + nReferStat - 20 ) * ( 16 + nSkillLv) / 13);
		fPowerMax = (( nMax + ((int)pAddSkillProp->dwAbilityMax + pWeapon->GetProp()->nAddSkillMax ) * 5 + nReferStat - 20 ) * ( 16 + nSkillLv) / 13);
	}
	else
	{
		fPowerMin = (( nMin + ((int)pAddSkillProp->dwAbilityMin ) * 5 + nReferStat - 20 ) * ( 16 + nSkillLv) / 13);
		fPowerMax = (( nMax + ((int)pAddSkillProp->dwAbilityMax ) * 5 + nReferStat - 20 ) * ( 16 + nSkillLv) / 13);
	}

	int nWeaponDamage = GetPlusWeaponATK( pItemProp->dwWeaponType );
	
	fPowerMin = GetParam( DST_CHR_DMG, fPowerMin ) + nWeaponDamage;
	fPowerMax = GetParam( DST_CHR_DMG, fPowerMax ) + nWeaponDamage;

	float fMinMax = (fPowerMax - fPowerMin) + 1;
	if( fMinMax <= 0 )
		fMinMax = 1;
	float fPower = fPowerMin + xRandom(fMinMax);
#if !defined(__BALANCE_LEVEL_050706)
	fPower += ( ( GetLevel() / 60 ) + 1 );	// __NEWATTACK0922_VER2
#endif

	return (int)fPower;
}

//
// 완드(매직)공격 데미지계산.
//
int CMover::GetMagicHitPower( int nChargeLevel )
{
	ItemProp* pItemProp = GetActiveHandItemProp();			// 들고있는 무기의 프로퍼티 꺼냄
	CItemElem *pWeapon = GetWeaponItem();		// 손에든 아이템의 포인터.

	int nMin, nMax;
	GetItemATKPower( &nMin, &nMax, pItemProp, pWeapon ); 

	int nATK = GetWeaponATK( WT_MAGIC_WAND );
	nMin += nATK;
	nMax += nATK;

	int nDamage = xRandom( nMin, nMax );
	nDamage += GetParam( DST_CHR_DMG, 0 );
	nDamage *= GetWandATKMultiplier( nChargeLevel );
	return nDamage;
}


// Parry처리
// nDamage : 공격자의 실제 공격데미지량
// nResult : nHitPower에서 방어력/저항력등을 뺀 실제 타겟에 들어가는 데미지량.
void CMover::_SubParry( CMover* pAttacker, int nATK, int& nResult, DWORD &dwAtkFlags )
{
	if( dwAtkFlags & AF_FORCE )			// 미리 계산된 데미지류는 skip(예: 반사, 사이킥 월)
		return;

	if( (dwAtkFlags & AF_MELEESKILL) ) 
	{
		if( !IsNPC() && !pAttacker->IsNPC() )
		{
			int A, B;
			
			A = (FLOAT)GetLevel() / ( GetLevel()+pAttacker->GetLevel() ) * 9;
			B = ( 3 + ( GetDex() + pAttacker->GetDex() ) * ( GetDex() - pAttacker->GetDex() ) / 400 );
			
			if( B < 0 )
				B = 0;
			
			if( B > 10 )
				B = 10;
			
			int nResist = A + B;
			
			if( xRandom( 100 ) < nResist )
			{
				dwAtkFlags	|= AF_PARRY;
				nResult = nResult * 0.3f;
			}
			else
			{
				if( nResult < 0 )	nResult	= 0;
			}
		}
	}
	else if( dwAtkFlags & (AF_MAGICSKILL | AF_MAGIC ) )
	{
		int A, B;
		
		A = (FLOAT)GetLevel() / ( GetLevel()+pAttacker->GetLevel() ) * 9;
		B = ( 3 + ( GetInt() + pAttacker->GetInt() ) * ( GetInt() - pAttacker->GetInt() ) / 400 );
		
		if( B < 0 )
			B = 0;
		
		if( B > 10 )
			B = 10;
		
		int nResist = A + B;
		
		if( xRandom( 100 ) < nResist )
		{
			dwAtkFlags	|= AF_RESIST;
			nResult = nResult * 0.3f;
		}
		else
		{
			if( nResult < 0 )	nResult	= 0;
			if( pAttacker->IsNPC() )	// 공격자가 NPC일경우만 적용.
				nResult	+= ( nATK / 10 + 1 );
		}
	}
	else	
	{
		// 일반 공격시 패리 검사.	이제 사용안함 
	}
}	




// 디펜스, 페리, 크리티컬을 적용한다.
// nATK : 공격자의 공격력
// nDamage : 최종 데미지 = 공격력 - 방어력
int CMover::ApplyDPC( int nATK, ATTACK_INFO* pInfo )
{	
	int nDamage;
	if( pInfo->CanIgnoreDEF() )
		nDamage = nATK;
	else
	{
		int nDefense = CalcDefense( pInfo );
		nDamage = nATK - nDefense;
	}

#if defined(__PVP_V2)
	if( nDamage < 0 )	
		nDamage = 0;
#else
	if( this->IsPlayer() && pInfo->pAttacker->IsPlayer() )	// 플레이어 vs 플레이어 데미지를 조정
	{
		if( nDamage < 0 )	
			nDamage = 0;
		nDamage += ( nATK / 4 + 1 );
		if( xRandom( 100 ) < 10 )
			nDamage *= 1.8f;
	}

	// Parry / Resist 처리.
	_SubParry( pInfo->pAttacker, nATK, nDamage, pInfo->dwAtkFlags );
#endif // __PVP_V2

	// 크리티컬 처리.
	if( pInfo->pAttacker->IsCriticalAttack( this, pInfo->dwAtkFlags ) )		
	{
		pInfo->dwAtkFlags |= AF_CRITICAL;

		int nChargeLevel = pInfo->GetChargeLevel();		// 완드충전량	
		if( (pInfo->pAttacker->m_pActMover->GetState() & OBJSTA_ATK4) || 
			nChargeLevel == MAX_CHARGE_LEVEL )	// 4패턴공격에 발생한 크리티컬.
		{
			nDamage = (int)(nDamage * 2.6f);
			if( CanFlyByAttack() && xRandom(100) < 50 )
				pInfo->dwAtkFlags |= AF_FLYING;
		}
		else
		{
			nDamage = (int)(nDamage * 2.3f);
			if( CanFlyByAttack() && xRandom(100) < 30 )
				pInfo->dwAtkFlags |= AF_FLYING;
		}
#ifdef __NEWWPN0818
		float fCriticalBonus	= 1 + (float)GetParam( DST_CRITICAL_BONUS, 0  ) / 100.0F;
		nDamage		*= fCriticalBonus;
#endif	// #ifdef __NEWWPN0818
	}
	return nDamage;
}


void CMover::ReSetDuelTime( CMover* pAttacker, CMover* pDefender )
{
#ifdef __NPP_050308
	if( pDefender->m_nDuel == 1 && pAttacker->m_nDuel == 1 
		&& pDefender->m_idDuelOther == pAttacker->GetId() && pAttacker->m_idDuelOther == pDefender->GetId() )
	{
		pDefender->m_timerEndDuel.Reset();
		pAttacker->m_timerEndDuel.Reset();
	}
	else if( pDefender->m_nDuel == 2 && pAttacker->m_nDuel == 2 
		&& pDefender->m_idDuelParty == pAttacker->m_idparty && pAttacker->m_idDuelParty == pDefender->m_idparty )
	{
		g_PartyMng.Lock( theLineFile );
		CMover* pLeader;
		CParty* pParty	= g_PartyMng.GetParty( pDefender->m_idparty );
		
		if( pParty )
		{
			pLeader = pParty->GetLeader();
			if( IsValidObj( pLeader ) )
				pLeader->m_timerEndDuel.Reset();
		}
		
		CParty* pParty2		= g_PartyMng.GetParty( pAttacker->m_idDuelParty );
		if( pParty2 )
		{
			pLeader = pParty2->GetLeader();
			if( IsValidObj( pLeader ) )
				pLeader->m_timerEndDuel.Reset();
		}
		g_PartyMng.Unlock( theLineFile );
	}
#endif // __NPP_050308
}

// 공격자가 변경되면 경험치 지급률을 변경
void CMover::ChangeExpRatio( CMover* pAttacker, CMover* pDefender )
{
	BOOL bDiffer = FALSE;
	if( pDefender->m_idAttacker != pAttacker->GetId() )	// 공격자가 변경되면?
	{
		if( pAttacker->m_idparty )	// 날 친놈이 파티가 있는가.
		{
			CMover *pLastAttacker = prj.GetMover( pDefender->m_idAttacker );
			if( IsValidObj( (CObj*)pLastAttacker ) && pLastAttacker->m_idparty != pAttacker->m_idparty )	// 다른파티가 쳤으면 지급률 바꿈.
				bDiffer = TRUE;
		} else
			bDiffer = TRUE;			// 날 마지막으로 쳤던놈이 파티가 없으면 지급률 바꿈.
	}
	
	if( bDiffer )	// 다른 상대(파티도 다른)가 쳤으면 지급률을 변경 
	{
		if( pDefender->GetProp()->dwClass != RANK_SUPER )		// 보스몹일경우 이 적용을 받지 않음.
			pDefender->m_fExpRatio = (float)pDefender->GetHitPoint() / pDefender->GetMaxHitPoint();
	}
}

// 스킬이나 물리적 공격을 당할 경우 
void CMover::OnAttacked( CMover* pAttacker, int nDamage, BOOL bTarget )
{
	CMover* pDefender = this;
	if( pDefender == pAttacker )
	{
		ASSERT( FALSE );	// 일단 현 기획에서 자신을 공격하는 경우는 없다. 
		return;
	}

	BOOL bAddEnemy = TRUE;
	DWORD dwLast = 0;

#ifdef	__NPP_050308 
	HITTYPE type = pAttacker->GetHitType( pDefender, bTarget );
	if( type == HITTYPE_PVP || type == HITTYPE_WAR || type == HITTYPE_SCHOOL )
		bAddEnemy = FALSE;
#endif	// __NPP_050308
	
	if( bAddEnemy )
	{
		dwLast = pDefender->AddEnemy( pAttacker->GetId(), nDamage );
		pAttacker->AddEnemy( pDefender->GetId(), 0 );
	}

#ifdef	__NPP_050308 
	switch( type )
	{
	case HITTYPE_PK:
		//TRACE("GetTickCount():%u, dwLast:%u GetTickCount() - dwLast:%u\n", GetTickCount(),dwLast, GetTickCount() - dwLast);

		// PK세션을 시작하기 위해서 플레이어간의 적대관계 시작을 보낸다.
		((CUser*)pAttacker)->UpdatePlayerEnemy( ADD_PLAYER_ENEMY, pDefender->GetId() );
		((CUser*)pDefender)->UpdatePlayerEnemy( ADD_PLAYER_ENEMY, pAttacker->GetId() );
		
		// 첫 PK시도이면 슬로터 포인트를 감소한다. 
		if( dwLast == 0 || (GetTickCount() - dwLast) > MIN(1) )		//첫타 or 세션이 1분을 지났는가?
		{
			pAttacker->ChangeSlaughter( CHANGE_SLAUGHTER_ATTACK, pDefender );
		}
		break;

	case HITTYPE_PVP:
		pAttacker->ReSetDuelTime( pAttacker, pDefender );
		break;
	}
#endif	// __NPP_050308 

	if( pAttacker->IsPlayer() && pDefender->IsNPC() )		
	{
		if( pDefender->m_idAttacker != NULL_ID )			// 누군가가 한번 친건가?
		{
			if( nDamage > 0 )
				ChangeExpRatio( pAttacker, pDefender );		 
		}
		else
		{
			((CUser *)pAttacker)->AddCorrReq( pDefender );	// 공격자유저에게 몹 좌표한번 보정.
		}
		pDefender->m_idAttacker = pAttacker->GetId();		// 피격자는 공격자를 기억 
	}
	pAttacker->m_idLastHitMover = pDefender->GetId();		// PK시 아이템 권리처리를 위해서 기억해둠 ( NPP에서 사용하지 않음 )
}

#ifdef __NPP_050308

// 좋은 버프일때만 여기 들어옴
// return HITTYPE_GENERIC이면 안드러감
HITTYPE	CMover::GetHitType2( CMover* pMover, BOOL bTarget )
{
	if( !IsValidObj( (CObj*)this ) || !IsValidObj( (CObj*)pMover ) )
		return HITTYPE_GENERIC;
	if( IsDie() || pMover->IsDie() )					// 죽은상태
		return HITTYPE_FAIL;
	if( m_nDead || pMover->m_nDead )					// 죽은후 무적상태
		return HITTYPE_GENERIC;

	if( IsPlayer() && pMover->IsPlayer() )
	{
#ifdef __JAPAN_PVP
#else // __JAPAN_PVP
		if( !g_eLocal.GetState( EVE_18 ) )
			return HITTYPE_FAIL;
#endif // __JAPAN_PVP
		if( m_pActMover->IsFly() || pMover->m_pActMover->IsFly() )	// 공중에 있으면 불가 
			return HITTYPE_FAIL;
		if( IsPVPTarget( pMover ) )
			return HITTYPE_PVP;
		if( IsWarTarget( pMover ) )
			return HITTYPE_WAR;
		if( IsSchoolTarget( pMover ) )
			return HITTYPE_SCHOOL;

#ifdef __GUILDCOMBAT
		if( IsGuildCombatTarget( pMover ) )
			return HITTYPE_GUILDCOMBAT;
#endif // __GUILDCOMBAT


		if( g_eLocal.GetState( EVE_PK ) )
		{
			DWORD dwLast;										
			GetEnemyHit( pMover->GetId(), &dwLast );
			if( dwLast == 0 || (GetTickCount() - dwLast) > MIN(1) )		//PK세션 검사 
				return HITTYPE_FAIL;
			else
				return HITTYPE_PK;		
		}
		else
		{
			return HITTYPE_FAIL;
		}
	}
	else
	{
		return HITTYPE_GENERIC;		// NPC에게는 좋은 버프를 줄수 없다.
	}
}

HITTYPE	CMover::GetHitType( CMover* pMover, BOOL bTarget )
{
	if( !IsValidObj( (CObj*)this ) || !IsValidObj( (CObj*)pMover ) )
		return HITTYPE_FAIL;
	if( IsDie() || pMover->IsDie() )					// 죽은상태
		return HITTYPE_FAIL;
	if( m_nDead || pMover->m_nDead )					// 죽은후 무적상태
		return HITTYPE_FAIL;

	if( IsPlayer() && pMover->IsPlayer() )
	{
#ifdef __JAPAN_PVP
#else // __JAPAN_PVP
		if( !g_eLocal.GetState( EVE_18 ) )
			return HITTYPE_FAIL;
#endif // __JAPAN_PVP
		if( m_pActMover->IsFly() || pMover->m_pActMover->IsFly() )	// 공중에 있으면 불가 
			return HITTYPE_FAIL;
		if( IsPVPTarget( pMover ) )
			return HITTYPE_PVP;
		if( IsWarTarget( pMover ) )
			return HITTYPE_WAR;
		if( IsSchoolTarget( pMover ) )
			return HITTYPE_SCHOOL;

#ifdef __GUILDCOMBAT
		if( IsGuildCombatTarget( pMover ) )
			return HITTYPE_GUILDCOMBAT;
#endif // __GUILDCOMBAT

		DWORD dwAttr	= GetPKPVPRegionAttr();		
		if( dwAttr == RA_SAFETY || dwAttr != pMover->GetPKPVPRegionAttr() )
			return HITTYPE_FAIL;
		if( m_idparty > 0 && m_idparty == pMover->m_idparty )	
			return HITTYPE_FAIL;
		if( m_idGuild > 0 && m_idGuild == pMover->m_idGuild )
			return HITTYPE_FAIL;
		if( IsBaseJob() || pMover->IsBaseJob() )
			return HITTYPE_FAIL;
		if( g_eLocal.GetState( EVE_GUILDWAR ) )			// 길드전 상태에서는 PK불가 
		{
			if( m_idWar > 0 || pMover->m_idWar > 0 )	
				return HITTYPE_FAIL;
		}
		if( g_eLocal.GetState( EVE_PK ) )
		{
			if( pMover->IsChaotic() )						// 카오        - PK가능 
				return HITTYPE_PK;
			if( GetTickCount() >= m_dwPKTargetLimit )		// PK선공불가시간동안에는 선공이 안됨  
			{
				if( bTarget )								// 타켓유저    - PK가능 
					return HITTYPE_PK;
			}
			
			DWORD dwLast;										
			GetEnemyHit( pMover->GetId(), &dwLast );
			if( dwLast == 0 || (GetTickCount() - dwLast) > MIN(1) )		//PK세션 검사 
				return HITTYPE_FAIL;
			
			return HITTYPE_PK;
		}
		else
		{
			return HITTYPE_FAIL;
		}
	}
	else
	{
		if( IsNPC() && GetProp()->dwClass == RANK_GUARD )			// 예외처리 
			return HITTYPE_GENERIC;

		//TODO: IsFly()로 통합 
		bool bAttackerFly = ( IsNPC() && IsFlyingNPC() ) || ( IsPlayer() && IsFly() );
		bool bDefenderFly = ( pMover->IsNPC() && pMover->IsFlyingNPC() ) || ( pMover->IsPlayer() && pMover->IsFly() );

		if( bAttackerFly != bDefenderFly )
			return HITTYPE_FAIL;

		return HITTYPE_GENERIC;
	}
}
#endif	// __NPP_050308

#endif // __WORLDSERVER

BOOL CMover::IsWarTarget( CMover* pMover )
{
	if( g_eLocal.GetState( EVE_GUILDWAR ) == 1 )
	{
		if( m_idWar > 0 && m_idWar == pMover->m_idWar && m_idGuild != pMover->m_idGuild )
			return TRUE;
	}
	return FALSE;
}

BOOL CMover::IsSchoolTarget( CMover* pMover )
{
#ifdef __SCHOOL0701
	return( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) == SBS_START2 );		// '/sbstart2'
#else	// __SCHOOL0701
	return FALSE;
#endif	// __SCHOOL0701
}

#ifdef __GUILDCOMBAT
BOOL CMover::IsGuildCombatTarget( CMover* pMover )
{
#ifdef __WORLDSERVER
	if( g_GuildCombatMng.m_nGuildCombatState != CGuildCombat::WAR_WAR_STATE )
		return FALSE;
#endif // __WORLDSERVER

	if( m_nGuildCombatState != 1 || pMover->m_nGuildCombatState != 1 )
		return FALSE;

	if( m_idGuild == pMover->m_idGuild )
		return FALSE;

	return TRUE;
}
#endif // __GUILDCOMBAT

// 맞는 모션을 취한다.
void CMover::SetDamagedMotion( CMover* pAttacker, DWORD dwAtkFlags )
{
	if((m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))			// 데미지플라잉이 아닐때만 아래 처리.
		return;

	m_pActMover->ResetState( OBJSTA_JUMP_ALL );
	if( dwAtkFlags & AF_FLYING )									// 맞고 날아가야 하는가?
	{
		m_pActMover->SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_FLY );
		SetMotion( MTI_DMGFLY, ANILOOP_CONT );
		if( pAttacker )
		{ 
			pAttacker->SendActMsg( OBJMSG_STOP );					// 플라잉다운이 일어났을때 어태커는 일단 세우자.
			m_pActMover->DoDamageFly( pAttacker->GetAngle(), 145.0f, 0.18f );	// 어태커가 보는쪽으로 날려보냄.
		}
	} 
	else
		m_pActMover->SetState( OBJSTA_DMG_ALL, OBJSTA_DMG );
	
	if( m_pActMover->IsActJump() || m_pActMover->IsActAttack() )	// 점프중 or 공격중 엔 데미지 모션 안냄.
		return;
	if( m_pActMover->IsAction() )		// 일반액션 중이면 데미지 모션 안냄.
		return;

	m_dwMotion = -1;
	if( m_pActMover->IsFly() )
	{
		ItemProp* pItemProp = prj.GetItemProp( m_dwRideItemIdx );
		if( pItemProp )
			SetMotion( pItemProp->dwUseMotion + MTA_FDMG1, ANILOOP_1PLAY, MOP_FIXED | MOP_NO_TRANS );
	}
	else
	{
		SetMotion( MTI_DMG1 + random( 2 ), ANILOOP_1PLAY );
	}
}


// 아이템의 효능 곱하기 팩터를 구한다.
float CMover::GetItemMultiplier( CItemElem* pItemElem )
{
#ifdef __NPP_050308
	if( IsChaotic() == FALSE && (pItemElem->GetProp()->nEvildoing < 0) )
		return 0.0f;	// 선한자가 카오 아이템을 사용하면 효능이 없다.
#endif

#ifdef __FINITEITEM0705
	if( pItemElem->IsFlag( CItemElem::expired ) )
		return 0.0f;
#endif	// __FINITEITEM0705

	ItemProp* pItemProp = pItemElem->GetProp();
	ASSERT( pItemProp );

	float fValue = CMover::GetItemEnduranceInfluence( ( pItemElem->m_nHitPoint * 100 ) / pItemProp->dwEndurance );
#ifdef __BALANCE_050614	
	if( pItemElem->GetAbilityOption() > 0 )
	{
		int nValue = prj.GetExpUpItem( pItemProp->dwItemKind3, pItemElem->GetAbilityOption() );
		nValue += 100;
		fValue = fValue * nValue / 100.0f;		// 제련수치가 곱해진다.
	}
#endif
	return fValue;
}

void CMover::GetItemATKPower( int *pnMin, int *pnMax, ItemProp* pItemProp, CItemElem *pWeapon ) 
{
	float f	= 1.0f;
	int nOption, nAdd;
	nAdd = nOption = 0;

	if( pWeapon )
		nOption = pWeapon->GetAbilityOption();	// 제련수치 

#if defined(__BALANCE_050614)	
	if( nOption > 0 )
		nAdd = (int)pow( nOption, 1.5 );
#else
	nAdd = prj.GetExpUpItem( pItemProp->dwItemKind3, nOption );
#endif

	if( pWeapon )
	{
		f = GetItemMultiplier( pWeapon );
		if( pWeapon->m_nRepairNumber >= 100 )
			nAdd += prj.GetExpUpItem( pItemProp->dwItemKind3, 1 );
	}
	*pnMin	= (int)( GetItemAbilityMin( pItemProp->dwID ) * f ) + nAdd;
	*pnMax	= (int) ( GetItemAbilityMax( pItemProp->dwID ) * f ) + nAdd;
}


int CMover::GetReqMp( int nReqMp )
{
#ifdef __NEWWPN0818
	return nReqMp - nReqMp * GetParam( DST_MP_DEC_RATE, 0 ) / 100;
#else	// __NEWWPN0818
	return nReqMp;
#endif	// __NEWWPN0818
}

int CMover::GetReqFp( int nReqFp )
{
#ifdef __NEWWPN0818
	return nReqFp - nReqFp * GetParam( DST_FP_DEC_RATE, 0 ) / 100;
#else	// __NEWWPN0818
	return nReqFp;
#endif	// __NEWWPN0818
}