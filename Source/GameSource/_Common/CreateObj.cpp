#include "stdafx.h"
#include "CommonCtrl.h"
#include "Ship.h"
#ifdef __CLIENT
	#include "Sfx.h"
	#include "defineSound.h"
#endif




#ifdef __CLIENT

/*
	XI_로 시작하는건 하나의 빌보드 텍스춰일 수도 있고, 특수효과 툴에 의해 생성된 데이타일 수도 있다.
	
	CreateGenSfx - 일반 효과 인덱스에 해당하는 효과 인덱스로 생성 
			dwSfxIdx( EI_EXPLOSION ) -> Create( XI_EXPLOSION )

	CreateItemSfx - 아이템 인덱스에 해당하는 효과 인덱스로 생성 
			pItemElem( II_FOOD ) -> Create( XI_FOOD )
			
	CreateSkillSfx - 스킬 인덱스에 해당하는 효과 인덱스로 생성 
			pItemElem( SI_FIREBALL ) -> Create( XI_FIREBALL )
		
*/



// 일반 효과 생성 
// SFX 새로 만들고도 여기다가 추가 안해주면 생성 안된다.
CSfx* CreateSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 const D3DXVECTOR3& vPosSrc, OBJID idSrc, const D3DXVECTOR3& vPosDest, OBJID idDest, int nSec, BOOL bManageWorld )
{
	CSfx* pObj = NULL;
#ifdef _DEBUG
	if( dwSfxObj == XI_INDEX( 1234, XI_SKILL_LORDK_HOLYARMOR02 ) )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif

	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
#ifndef __WORLDSERVER

	switch( dwSfxObj )
	{
		case XI_INDEX( 1735, XI_SETIEM_EFFECTHAND ):
		case XI_INDEX( 1719, XI_GEN_ITEM_SETITEM03 ):
		case XI_INDEX( 1720, XI_GEN_ITEM_SETITEM04 ):
		case XI_INDEX( 1721, XI_GEN_ITEM_SETITEM05 ):
		case XI_INDEX( 1722, XI_GEN_ITEM_SETITEM06 ):
		case XI_INDEX( 1723, XI_GEN_ITEM_SETITEM07 ):
		case XI_INDEX( 1724, XI_GEN_ITEM_SETITEM08 ):
		case XI_INDEX( 1725, XI_GEN_ITEM_SETITEM09 ):
		case XI_INDEX( 1726, XI_GEN_ITEM_SETITEM10 ):

		case XI_INDEX( 2576, XI_SETIEM_EFFECTHAND_GOLD ):
		case XI_INDEX( 2528, XI_GEN_ITEM_SETITEM03_GOLD ):
		case XI_INDEX( 2529, XI_GEN_ITEM_SETITEM04_GOLD ):
		case XI_INDEX( 2530, XI_GEN_ITEM_SETITEM05_GOLD ):
		case XI_INDEX( 2531, XI_GEN_ITEM_SETITEM06_GOLD ):
		case XI_INDEX( 2532, XI_GEN_ITEM_SETITEM07_GOLD ):
		case XI_INDEX( 2533, XI_GEN_ITEM_SETITEM08_GOLD ):
		case XI_INDEX( 2534, XI_GEN_ITEM_SETITEM09_GOLD ):
		case XI_INDEX( 2535, XI_GEN_ITEM_SETITEM10_GOLD ):

		case XI_INDEX( 2577, XI_SETIEM_EFFECTHAND_RED ):
		case XI_INDEX( 2536, XI_GEN_ITEM_SETITEM03_RED ):
		case XI_INDEX( 2537, XI_GEN_ITEM_SETITEM04_RED ):
		case XI_INDEX( 2538, XI_GEN_ITEM_SETITEM05_RED ):
		case XI_INDEX( 2539, XI_GEN_ITEM_SETITEM06_RED ):
		case XI_INDEX( 2540, XI_GEN_ITEM_SETITEM07_RED ):
		case XI_INDEX( 2541, XI_GEN_ITEM_SETITEM08_RED ):
		case XI_INDEX( 2542, XI_GEN_ITEM_SETITEM09_RED ):
		case XI_INDEX( 2543, XI_GEN_ITEM_SETITEM10_RED ):

		case XI_INDEX( 2578, XI_SETIEM_EFFECTHAND_BLACK ):
		case XI_INDEX( 2544, XI_GEN_ITEM_SETITEM03_BLACK ):
		case XI_INDEX( 2545, XI_GEN_ITEM_SETITEM04_BLACK ):
		case XI_INDEX( 2546, XI_GEN_ITEM_SETITEM05_BLACK ):
		case XI_INDEX( 2547, XI_GEN_ITEM_SETITEM06_BLACK ):
		case XI_INDEX( 2548, XI_GEN_ITEM_SETITEM07_BLACK ):
		case XI_INDEX( 2549, XI_GEN_ITEM_SETITEM08_BLACK ):
		case XI_INDEX( 2550, XI_GEN_ITEM_SETITEM09_BLACK ):
		case XI_INDEX( 2551, XI_GEN_ITEM_SETITEM10_BLACK ):

		case XI_INDEX( 2579, XI_SETIEM_EFFECTHAND_PUPPLE ):
		case XI_INDEX( 2552, XI_GEN_ITEM_SETITEM03_PUPPLE ):
		case XI_INDEX( 2553, XI_GEN_ITEM_SETITEM04_PUPPLE ):
		case XI_INDEX( 2554, XI_GEN_ITEM_SETITEM05_PUPPLE ):
		case XI_INDEX( 2555, XI_GEN_ITEM_SETITEM06_PUPPLE ):
		case XI_INDEX( 2556, XI_GEN_ITEM_SETITEM07_PUPPLE ):
		case XI_INDEX( 2557, XI_GEN_ITEM_SETITEM08_PUPPLE ):
		case XI_INDEX( 2558, XI_GEN_ITEM_SETITEM09_PUPPLE ):
		case XI_INDEX( 2559, XI_GEN_ITEM_SETITEM10_PUPPLE ):

		case XI_INDEX( 2580, XI_SETIEM_EFFECTHAND_GREEN ):
		case XI_INDEX( 2560, XI_GEN_ITEM_SETITEM03_GREEN ):
		case XI_INDEX( 2561, XI_GEN_ITEM_SETITEM04_GREEN ):
		case XI_INDEX( 2562, XI_GEN_ITEM_SETITEM05_GREEN ):
		case XI_INDEX( 2563, XI_GEN_ITEM_SETITEM06_GREEN ):
		case XI_INDEX( 2564, XI_GEN_ITEM_SETITEM07_GREEN ):
		case XI_INDEX( 2565, XI_GEN_ITEM_SETITEM08_GREEN ):
		case XI_INDEX( 2566, XI_GEN_ITEM_SETITEM09_GREEN ):
		case XI_INDEX( 2567, XI_GEN_ITEM_SETITEM10_GREEN ):

		case XI_INDEX( 2581, XI_SETIEM_EFFECTHAND_WHITE ):
		case XI_INDEX( 2568, XI_GEN_ITEM_SETITEM03_WHITE ):
		case XI_INDEX( 2569, XI_GEN_ITEM_SETITEM04_WHITE ):
		case XI_INDEX( 2570, XI_GEN_ITEM_SETITEM05_WHITE ):
		case XI_INDEX( 2571, XI_GEN_ITEM_SETITEM06_WHITE ):
		case XI_INDEX( 2572, XI_GEN_ITEM_SETITEM07_WHITE ):
		case XI_INDEX( 2573, XI_GEN_ITEM_SETITEM08_WHITE ):
		case XI_INDEX( 2574, XI_GEN_ITEM_SETITEM09_WHITE ):
		case XI_INDEX( 2575, XI_GEN_ITEM_SETITEM10_WHITE ):
			pObj = new CSfxPartsLinkShoulder();
			break;

	//3rd skills
	case XI_INDEX( 1223, XI_SKILL_LORDK_TEMPLARPULLING01 ):
		pObj = new CSfxAttackParts( CSfxAttackParts::HIT_RHAND ); 
		break;    
	
	case XI_INDEX( 1229, XI_SKILL_LORDK_SHILDSTRIKE01 ):
	case XI_INDEX( 1292, XI_SKILL_ELE_FINALSPEAR01 ):
	case XI_INDEX( 1283, XI_SKILL_MENT_DARKNESSSCREAM01 ):
		pObj = new CSfx();
		pObj->SetAngle( -pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
		break;

	case XI_INDEX( 1287, XI_SKILL_MENT_DARKNESSRAKE02 ):
		pObj = new CSfx();
		pObj->SetAngle( -pSrc->GetAngle() + 90.0f ); // 시전자의 방향과 일치시킨다.
		break;

	case XI_INDEX( 1274, XI_SKILL_FLO_FETTERS02 ):
	case XI_INDEX( 1284, XI_SKILL_MENT_DARKNESSSCREAM02 ):
	case XI_INDEX( 1290, XI_SKILL_MENT_SPEEDDECREASE01 ):
	case XI_INDEX( 1289, XI_SKILL_MENT_DEFENDERDECREASE01 ):
	case XI_INDEX( 1288, XI_SKILL_MENT_ATTACKDECREASE01 ):
		pObj = new CSfxHitParts( CSfxHitParts::HIT_OVERHEAD );	
		break;
		
	case XI_INDEX( 1276, XI_SKILL_FORCEM_AURORAOFTHERAGE02 ):
	case XI_INDEX( 1278, XI_SKILL_FORCEM_AURORAOFTHETENACITY02 ):
	case XI_INDEX( 1280, XI_SKILL_FORCEM_AURORAOFTHESPEED02 ):
	case XI_INDEX( 1282, XI_SKILL_FORCEM_AURORAOFTHEMAD02 ):
	case XI_INDEX( 1257, XI_SKILL_CRACK_HWAKEYE02 ):
		pObj = new CSfxRotate_New();	
		break;

	case XI_INDEX( 1252, XI_SKILL_WINDL_CONTROLINCREASE01 ):
	case XI_INDEX( 1258, XI_SKILL_CRACK_POWERINCREASE01 ):
	case XI_INDEX( 1255, XI_SKILL_CRACK_CONTROL01 ):
	case XI_INDEX( 1273, XI_SKILL_FLO_FETTERS01 ):
		pObj = new CSfxAttackParts( CSfxAttackParts::HIT_OVERHEAD ); 
		break;
	
	case XI_INDEX( 20, XI_HIT_SWORD01 ): pObj = new CSfxGenNormalDmg01(); break;
	case XI_INDEX( 21, XI_HIT_SWORD02 ): pObj = new CSfxGenNormalDmg01(); break;
	case XI_INDEX( 24, XI_HIT_STICK01 ): pObj = new CSfxGenNormalDmg01(); break;
	case XI_INDEX( 25, XI_HIT_STICK02 ): pObj = new CSfxGenNormalDmg01(); break;

	case XI_INDEX( 27, XI_FIR_WAND01  ): pObj = new CSfxItemWandAtk1(); break;

	case XI_INDEX( 28, XI_FIR_WAND02  ): pObj = new CSfxItemWandAtkAir(); break;

	case XI_INDEX( 29, XI_FIR_RANGE01 ): pObj = new CSfx(); break;
	case XI_INDEX( 30, XI_FIR_RANGE02 ): pObj = new CSfx(); break;
		
	case XI_INDEX( 34, XI_GEN_RESTORATION01 ): pObj = new CSfxGenRestoration01();break;
	case XI_INDEX( 46, XI_GEN_LOGIN01       ): pObj = new CSfxGenLogin()        ;break;
	case XI_INDEX( 51, XI_GEN_WARP01        ): pObj = new CSfxGenWarp()         ;break;
	case XI_INDEX( 55, XI_GEN_PC_DIE01      ): pObj = new CSfxGenPcDie()        ;break;
	case XI_INDEX( 63, XI_GEN_MOVEMARK01	): pObj = new CSfxGenMoveMark()     ;break;
		
	case XI_INDEX( 72, XI_GEN_RAINCIRCLE01	): pObj = new CSfxGenRainCircle()  ;break;
		
	case XI_INDEX( 70, XI_GEN_WATERCIRCLE01	): pObj = new CSfxGenWaterCircle()  ;break;
	case XI_INDEX( 71, XI_GEN_WATERCROWN01	): pObj = new CSfxGenWaterCrown()   ; PLAYSND( SND_GEN_WATER, &vPosSrc ); break;
		
	case XI_INDEX( 100, XI_ITEM_WAND_ATK1		): pObj = new CSfxItemWandAtk1()   ;break;
	case XI_INDEX( 101, XI_ITEM_WAND_ATK2		): pObj = new CSfxItemWandAtk2()   ;break;
	case XI_INDEX( 102, XI_ITEM_WAND_ATK3		): pObj = new CSfxItemWandAtk3()   ;break;
	case XI_INDEX( 103, XI_ITEM_WAND_ATK4		): pObj = new CSfxItemWandAtk4()   ;break;
		
	case XI_INDEX( 112, XI_ITEM_RANGE_ATK1		): pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_INDEX( 113, XI_ITEM_RANGE_ATK2		): pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_INDEX( 114, XI_ITEM_RANGE_ATK3		): pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_INDEX( 115, XI_ITEM_RANGE_ATK4		): pObj = new CSfxItemRangeAtk1()   ;break;
		
	//gmpsun skill here
	case XI_INDEX( 203, XI_SKILL_VAG_ONE_OVERCUTTER01 ): 
	case XI_INDEX( 201, XI_SKILL_VAG_ONE_CLEANHIT01   ): 
	case XI_INDEX( 202, XI_SKILL_VAG_ONE_BRANDISH01   ): 
	case XI_INDEX( 229, XI_SKILL_MER_ONE_SPECIALHIT02 ):		//스페셜 히트 터지는 sfx
	case XI_INDEX( 232, XI_SKILL_MER_ONE_REFLEXHIT02 ):
	case XI_INDEX( 375, XI_SKILL_BIL_KNU_BELIALSMESHING02 ):
	case XI_INDEX( 343, XI_SKILL_BLD_DOUBLE_CROSSSTRIKE02 ):
	case XI_INDEX( 345, XI_SKILL_BLD_DOUBLE_ARMORPENETRATE02 ):
//	case XI_SKILL_KNT_TWOAX_POWERSTUMP03:
	case XI_INDEX( 377, XI_SKILL_BIL_KNU_BLOODFIST02 ):
	case XI_INDEX( 349, XI_SKILL_BLD_DOUBLE_SONICBLADE03 ):
	case XI_INDEX( 376, XI_SKILL_BIL_KNU_PIERCINGSERPENT02 ):
	case XI_INDEX( 503, XI_SKILL_RIN_SUP_MERKABAHANZELRUSHA03 ):
	case XI_INDEX( 421, XI_SKILL_ELE_FIRE_BURINGFIELD02 ):
	case XI_INDEX( 326, XI_SKILL_KNT_TWOSW_EARTHDIVIDER02 ):
		pObj = new CSfxFixed();		// fixed는 이펙트 
		if( pObj && IsValidObj(pSrc) )
			pObj->SetAngle( -pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
//			pObj->SetAngle( 180.0f - pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.

		break;
	case XI_INDEX( 378, XI_SKILL_BIL_KNU_SONICHAND02 ):		// sfx 방향바껴서 각도 조절 따로 하는거
	case XI_INDEX( 382, XI_SKILL_BIL_KNU_ASALRAALAIKUM02 ):
		pObj = new CSfxFixed();		// fixed는 이펙트 
		if( pObj && IsValidObj(pSrc) )
		{
			float fAngle = -pSrc->GetAngle() + 180;
			pObj->SetAngle( fAngle ); // 시전자의 방향과 일치시킨다.
		}
		
		break;
		// 폭발씬 류는 지상에서 좀 띄워서 출력
//	case XI_SKILL_RAG_SUP_FASTATTACK02:
//	case XI_SKILL_ACR_YOYO_SLOWSTEP02:
	case XI_INDEX( 17, XI_HIT_YOY01 ):
	case XI_INDEX( 33, XI_HIT_BOW01 ):
	case XI_INDEX( 569, XI_SKILL_JST_YOYO_HITOFPENYA02 ):
	case XI_INDEX( 571, XI_SKILL_JST_YOYO_VATALSTAB02 ):
	case XI_INDEX( 554, XI_SKILL_ACR_YOYO_SLOWSTEP02 ):
	case XI_INDEX( 574, XI_SKILL_RAG_BOW_FLAMEARROW03 ):
	case XI_INDEX( 575, XI_SKILL_RAG_BOW_PIRCINGARROW03 ):
	case XI_INDEX( 576, XI_SKILL_RAG_BOW_POISONARROW03 ):
	case XI_INDEX( 577, XI_SKILL_RAG_BOW_SILENTARROW03 ):
	case XI_INDEX( 578, XI_SKILL_RAG_SUP_NATURE02 ):
	case XI_INDEX( 579, XI_SKILL_RAG_BOW_TRIPLESHOT03 ):
		
	case XI_INDEX( 560, XI_SKILL_ACR_YOYO_SNITCH02 ):
	case XI_INDEX( 553, XI_SKILL_ACR_BOW_AIMEDSHOT03 ):
	case XI_INDEX( 551, XI_SKILL_ACR_BOW_JUNKBOW03 ):
	case XI_INDEX( 555, XI_SKILL_ACR_BOW_SILENTSHOT03 ):
	case XI_INDEX( 559, XI_SKILL_ACR_BOW_AUTOSHOT03 ):
	case XI_INDEX( 348, XI_SKILL_BLD_DOUBLEAX_HAWKATTACK03 ):
	case XI_INDEX( 340, XI_SKILL_BLD_DOUBLESW_BLADEDANCE02 ):
	case XI_INDEX( 347, XI_SKILL_BLD_DOUBLESW_BLADEDANCE03 ):
	case XI_INDEX( 318, XI_SKILL_KNT_TWO_POWERSWING02 ):

//		vPosDest.y += 1.0f;
		pObj = new CSfxFixed();		// fixed는 이펙트 
		if( pObj && IsValidObj(pSrc) )
			pObj->SetAngle( 180.0f - pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
		break;
		
	case XI_INDEX( 205, XI_SKILL_MER_ONE_KEENWHEEL01    ): pObj = new CSfxSkillMerKeenWheel()    ;break;
	case XI_INDEX( 204, XI_SKILL_MER_ONE_SPLMASH01      ): pObj = new CSfxSkillMerSplmash()      ;break;
	case XI_INDEX( 206, XI_SKILL_MER_ONE_BLINDSIDE01    ): pObj = new CSfxSkillMerBlindSide()    ;break;
	case XI_INDEX( 269, XI_SKILL_MAG_WIND_SWORDWIND01  ): pObj = new CSfxSkillMagSwordWind()     ; break;
	case XI_INDEX( 270, XI_SKILL_MAG_WIND_STRONGWIND01 ): pObj = new CSfxSkillMagStrongWind()    ; break;
	case XI_INDEX( 264, XI_SKILL_MAG_FIRE_BOOMERANG01  ): pObj = new CSfxSkillMagFireBoomerang() ; break;
	case XI_INDEX( 265, XI_SKILL_MAG_FIRE_HOTAIR01     ): pObj = new CSfxSkillMagHotAir()        ; break;

	case XI_INDEX( 278, XI_SKILL_MAG_EARTH_CASTING01 ):
	case XI_INDEX( 277, XI_SKILL_MAG_ELECTRICITY_CASTING01 ):
	case XI_INDEX( 276, XI_SKILL_MAG_WATER_CASTING01 ):		// 일단 캐스팅 동작은 모두 FireCasting으로 쓴다.
	case XI_INDEX( 279, XI_SKILL_MAG_MAG_CASTING01 ):
	case XI_INDEX( 401, XI_SKILL_ELE_FIRE_CASTING01 ):
	case XI_INDEX( 438, XI_SKILL_ASS_CHEER_HASCASTING01 ):
	case XI_INDEX( 406, XI_SKILL_ELE_MULTI_CASTING01 ):
	case XI_INDEX( 384, XI_SKILL_PSY_PSY_CASTING01 ):
	case XI_INDEX( 274, XI_SKILL_MAG_FIRE_CASTING01 ): 
	case XI_INDEX( 295, XI_SKILL_ASS_HEAL_CASTING01 ):
	case XI_INDEX( 297, XI_SKILL_ASS_RES_CASTING01 ):
		pObj = new CSfxSkillMagFireCasting();
		break;
	case XI_INDEX( 275, XI_SKILL_MAG_WIND_CASTING01 )  : pObj = new CSfxSkillMagWindCasting();break;

	case XI_INDEX( 296, XI_SKILL_ASS_CHEER_CASTING01 ) : pObj = new CSfxSkillMagWindCasting();break;
		
	case XI_INDEX( 62, XI_GEN_CURE01 ): PLAYSND( SND_GEN_LEVELUP, &vPosSrc ); pObj = new CSfx(); break;
	case XI_INDEX( 65, XI_GEN_REF01 ): PLAYSND( SND_GEN_LEVELUP, &vPosSrc ); pObj = new CSfx(); break;

	case XI_INDEX( 11, XI_HIT_CRITICAL01 ): pObj = new CSfx(); break;	
	case XI_INDEX( 12, XI_HIT_MISS01 ): pObj = new CSfx(); break;	
		
	case XI_INDEX( 1600, XI_NPC_RAN_SPITTL    ): pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_INDEX( 1601, XI_NPC_RAN_GAS       ): pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_INDEX( 1602, XI_NPC_RAN_MAGICBALL ): pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_INDEX( 1604, XI_NPC_RAN_MAGICBLUE ): pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_INDEX( 1605, XI_NPC_RAN_CARD      ): pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_INDEX( 1607, XI_NPC_RAN_CANNON    ): pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_INDEX( 1746, XI_SKILL_MUSHMOOT_02 ): pObj = new CSfxMagicMiAtk1()   ;break;    
	case XI_INDEX( 1507, XI_SKILL_TRO_STRETCHING01 ): pObj = new CSfxTroStretching01()   ;break;    
	case XI_INDEX( 1508, XI_SKILL_TRO_STRETCHING02 ): pObj = new CSfxTroStretching02()   ;break;    
	case XI_INDEX( 1502, XI_SKILL_TRO_BLITZ ): pObj = new CSfxTroBlitz()   ;break;    
	case XI_INDEX( 298, XI_SKILL_ASS_KNU_BURSTCRACK01 ): pObj = new CSfxSkillAssBurstcrack()   ;break;    
	case XI_INDEX( 299, XI_SKILL_ASS_KNU_TAMPINGHOLE01 ): pObj = new CSfxSkillAssTampinghole()   ;break;    
	case XI_INDEX( 1608, XI_NPC_DIR_STEAM ): pObj = new CSfxNpcDirSteam()   ;break;    
	case XI_INDEX( 1700, XI_GEN_PVP_FLAG01 ): pObj = new CSfxDuelParty();		break;	// 이거 CSfxDuelParty()로 안쓸거면 모든소스검색해서 이거쓴거 바꿔줘야함
	case XI_INDEX( 104, XI_ITEM_COLLECT ): pObj = new CSfxCollect();		break;
	case XI_INDEX( 1611, XI_NPCSP1DIRAMP ):		pObj = new CSfxClockWorksCharge();	break;
	case XI_INDEX( 1613, XI_NPCSP1DIRCANNON ):	pObj = new CSfxClockWorksCannon();	break;
	case XI_INDEX( 1616, XI_NPCSP1RANBALL ):		pObj = new CSfxAtkStraight();		break;
	case XI_INDEX( 1747, XI_SKILL_MUSHMOOT_CHARGE ):	pObj = new CSfxMushmootCharge();	break;
	case XI_INDEX( 302, XI_SKILL_MAG_WATER_ICEMISSILE01 ): pObj = new CSfxSkillMagIceMissile()    ;break;
	case XI_INDEX( 226, XI_SKILL_MER_ONE_BLOODYSTRIKE02 ):	
		pObj = new CSfx();
		if( pObj )
		{
			CMover *pSrc = prj.GetMover( idSrc );
			CMover *pDst = prj.GetMover( idDest );
			if( IsValidObj(pSrc) && IsValidObj(pDst) )
			{
				FLOAT fAngle = GetDegree( pSrc->GetPos(), pDst->GetPos() );
				pObj->SetAngle( -fAngle );
			}
		}
		break;
	// 일반적인 쏘고 날아가서 맞는 이펙트는 이걸로 통일하자.
	case XI_INDEX( 303, XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01 ): 	pObj = new CSfxSkillMagLightningBall();		break;
	case XI_INDEX( 236, XI_SKILL_MAG_FIRE_FIRESTRIKE02 ):		pObj = new CSfxShoot();		break;
	case XI_INDEX( 304, XI_SKILL_MAG_EARTH_SPIKESTONE01 ):	pObj = new CSfxSkillMagSpikeStone();		break;
	case XI_INDEX( 1095, XI_NAT_WINGANGEL01 ):	// 천사의 날개
		pObj = new CSfxRotate();
		break;
	case XI_INDEX( 1096, XI_NAT_WASTART01 ):	// 천사의 날개 시전 효과.
		pObj = new CSfx();
		CreateSfx( pd3dDevice, XI_INDEX( 1095, XI_NAT_WINGANGEL01 ), vPosSrc, idSrc, vPosDest, idDest, nSec );		// 날개 2분동안 출력.
		break;
	case XI_INDEX( 1098, XI_NAT_CUPITSTART01 ):	// 큐피트날개 시전 효과.
		pObj = new CSfx();
		CreateSfx( pd3dDevice, XI_INDEX( 1095, XI_NAT_WINGANGEL01 ), vPosSrc, idSrc, vPosDest, idDest, nSec );		// 날개 5분동안 출력.
		break;
	case XI_INDEX( 313, XI_SKILL_KNT_TWO_POWERSWING01 ):		// 나이트-가드스킬
		idDest = NULL_ID;
		pObj = new CSfx();
		break;
	case XI_INDEX( 230, XI_SKILL_MER_ONE_GUILOTIN02 ): 	// 필참 - 파워웨이브.
		pObj = new CSfxShootWave();
		break;
	case XI_INDEX( 1745, XI_SKILL_DROP_DUST_RAIN ):
		pObj = new CSfxItemRangeAtk1_StoneRain();
		break;
	case XI_INDEX( 1744, XI_SKILL_DROP_DUST ): 
		pObj = new CSfxItemRangeAtk1_Stone();
		break;
	case XI_INDEX( 557, XI_SKILL_ACR_BOW_ARROWRAIN03 ):
		pObj = new CSfxItemRangeAtk1_AllowRain();		
		break;
	case XI_INDEX( 603, XI_SKILL_RAG_BOW_ARROWRAIN ):
		pObj = new CSfxItemRangeAtk1_Allow();
		break;
		
	// 활용 시전이펙트는 팔에 링크되게 한다.
	case XI_INDEX( 529, XI_SKILL_RAG_BOW_ICEARROW01 ):
	case XI_INDEX( 528, XI_SKILL_RAG_SUP_FASTATTACK01 ):
	case XI_INDEX( 530, XI_SKILL_RAG_BOW_FLAMEARROW01 ):
	case XI_INDEX( 531, XI_SKILL_RAG_BOW_PIRCINGARROW01 ):
	case XI_INDEX( 532, XI_SKILL_RAG_BOW_POISONARROW01 ):
	case XI_INDEX( 533, XI_SKILL_RAG_BOW_SILENTARROW01 ):
	case XI_INDEX( 535, XI_SKILL_RAG_BOW_TRIPLESHOT01 ):
					
	case XI_INDEX( 507, XI_SKILL_ACR_BOW_JUNKBOW01 ):
	case XI_INDEX( 509, XI_SKILL_ACR_BOW_AIMEDSHOT01 ):
	case XI_INDEX( 511, XI_SKILL_ACR_BOW_SILENTSHOT01 ):
	case XI_INDEX( 513, XI_SKILL_ACR_BOW_ARROWRAIN01 ):
	case XI_INDEX( 515, XI_SKILL_ACR_BOW_AUTOSHOT01 ):
	case XI_INDEX( 1728, XI_SKILL_MER_ONE_SUPPORT01 ):
	case XI_INDEX( 1729, XI_SKILL_MER_ONE_SUPPORT02 ):
	case XI_INDEX( 1730, XI_SKILL_MER_ONE_SUPPORT03 ):
	case XI_INDEX( 1731, XI_SKILL_MER_ONE_SUPPORT04 ):
	case XI_INDEX( 1732, XI_SKILL_ASS_KNU_SUPPORT01 ):
	case XI_INDEX( 1733, XI_SKILL_ASS_KNU_SUPPORT02 ):
	case XI_INDEX( 1734, XI_SKILL_ASS_KNU_SUPPORT03 ):
		pObj = new CSfxAllowPartsLink();
		break;

	case XI_INDEX( 527, XI_SKILL_JST_YOYO_VATALSTAB01 ):
	case XI_INDEX( 525, XI_SKILL_JST_YOYO_HITOFPENYA01 ):
	case XI_INDEX( 510, XI_SKILL_ACR_YOYO_SLOWSTEP01 ):
	case XI_INDEX( 521, XI_SKILL_JST_SUP_POISON01 ):  // 손에 링크되는 이펙
	case XI_INDEX( 522, XI_SKILL_JST_SUP_BLEEDING01 ):
	case XI_INDEX( 524, XI_SKILL_JST_YOYO_BACKSTAB01 ):		
	case XI_INDEX( 523, XI_SKILL_JST_SUP_ABSORB01 ):
	case XI_INDEX( 520, XI_SKILL_JST_SUP_CRITICALSWING01 ):
		pObj = new CSfxPartsLinkJst();
		break;

	case XI_INDEX( 333, XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01 ):		// 손에 링크되는 이펙
	case XI_INDEX( 336, XI_SKILL_BLD_DOUBLE_SONICBLADE01 ):
	case XI_INDEX( 315, XI_SKILL_KNT_TWOSW_CHARGE01 ):
	case XI_INDEX( 334, XI_SKILL_BLD_DOUBLESW_BLADEDANCE01 ):
		pObj = new CSfxPartsLinkBlade();		
		break;
	case XI_INDEX( 810, XI_EVE_EVENT_FAIRYLY ):
		pObj = new CSfxFixed();
		break;
		
//sun: 12, 몬스터 생성 Neuz, World
	case XI_INDEX( 119, XI_CHR_CURSOR1 ):
		pObj = new CSfxCursor();
		break;


	case XI_INDEX( 1914, XI_BUFFPET_GRADE1 ):
	case XI_INDEX( 1915, XI_BUFFPET_GRADE2 ):
	case XI_INDEX( 1916, XI_BUFFPET_GRADE3 ):
		pObj = new CSfxLinkMover();
		break;

		
	case 0: break;
	default: pObj = new CSfx(); break;
	}

	if( pObj )
	{
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel() == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModel이 없음" ), dwSfxObj );
				SAFE_DELETE( pObj );
				return NULL;
			}
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModelElem이 없음" ), dwSfxObj );
				SAFE_DELETE( pObj );
				return NULL;
			}

			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );

			// 오브젝트 생성후 후처리.
			switch( dwSfxObj )
			{
/*			case XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01:
				{
					CSfxShoot *pSfxShoot = (CSfxShoot *)pObj;
					pSfxShoot->SetHitSfx( "sfx_sklmagelelgtbal01", idSrc, idDest, 2.0f );
					pSfxShoot->SetDir( TRUE );
					pSfxShoot->SetSndHit( SND_PC_SKILLM_FIREBOOMERANG2 );
				}
				break;*/
			case XI_INDEX( 230, XI_SKILL_MER_ONE_GUILOTIN02 ): 	// 필참 - 파워웨이브.
				{
					CSfxShootWave *pSfxShootWave = (CSfxShootWave *)pObj;
					pSfxShootWave->SetHitSfx( XI_INDEX( 230, XI_SKILL_MER_ONE_GUILOTIN02 ), idSrc, idDest, 2.0f );
					pSfxShootWave->SetDir( TRUE );
//					pSfxShoot->SetSndHit( SND_PC_SKILLM_FIREBOOMERANG2 );
				}
				break;
			case XI_INDEX( 236, XI_SKILL_MAG_FIRE_FIRESTRIKE02 ):
				{
					CSfxShoot *pSfxShoot = (CSfxShoot *)pObj;
					pSfxShoot->SetDir( TRUE );
				}
				break;
			case XI_INDEX( 231, XI_SKILL_MER_ONE_SNEAKER02 ):	// 스네어
				{
					CMover *pDst = prj.GetMover( idDest );
					if( IsValidObj( pDst ) )
					{
						FLOAT fRadius = pDst->GetRadius();
						pObj->SetScale( D3DXVECTOR3( fRadius, fRadius, fRadius ) );
					}
				}
				break;
			}

			// 월드에 오브젝트 추가.
			if( bManageWorld )
			{
				if( g_WorldMng.Get()->AddObj( pObj ) == FALSE )
				{
					SAFE_DELETE( pObj );
				}
			}
		} else
		{
			SAFE_DELETE( pObj );
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

// 요요무기는 무조건 여기만 호출
CSfx* CreateSfxYoYo( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;

#ifdef _DEBUG
	if( dwSfxObj == 116 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
#ifndef __WORLDSERVER
	/*
	switch( dwSfxObj )
	{
	}
	*/
	// 요요
	pObj = new CSfxItemYoyoAtk();

	if( pObj )
	{
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel() == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModel이 없음" ), dwSfxObj );
			}
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModelElem이 없음" ), dwSfxObj );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );

			// 월드에 오브젝트 추가.
			if( g_WorldMng.Get()->AddObj(pObj) == FALSE )
			{
				SAFE_DELETE( pObj );
			}
		} else
		{
			SAFE_DELETE( pObj );
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

CSfxShoot* CreateShootSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
						D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfxShoot* pObj = NULL;
#ifdef _DEBUG
	if( dwSfxObj == XI_INDEX( 235, XI_SKILL_MAG_FIRE_FIRESTRIKE01 ) )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
#ifndef __WORLDSERVER
	switch( dwSfxObj )
	{
	case XI_INDEX( 27, XI_FIR_WAND01  ): pObj = new CSfxItemWandAtk1(); break;
	case XI_INDEX( 28, XI_FIR_WAND02  ): pObj = new CSfxItemWandAtkAir(); break;

	case XI_INDEX( 100, XI_ITEM_WAND_ATK1		): pObj = new CSfxItemWandAtk1()   ;break;
	case XI_INDEX( 101, XI_ITEM_WAND_ATK2		): pObj = new CSfxItemWandAtk2()   ;break;
	case XI_INDEX( 102, XI_ITEM_WAND_ATK3		): pObj = new CSfxItemWandAtk3()   ;break;
	case XI_INDEX( 103, XI_ITEM_WAND_ATK4		): pObj = new CSfxItemWandAtk4()   ;break;

	case XI_INDEX( 547, XI_SKILL_RAG_BOW_TRIPLESHOT02 ):
	case XI_INDEX( 536, XI_SKILL_ACR_BOW_JUNKBOW02 ):
		pObj = new CSfxItemRangeAtk_JunkBow()   ;break;
		
	case XI_INDEX( 542, XI_SKILL_RAG_BOW_ICEARROW02 ):
	case XI_INDEX( 543, XI_SKILL_RAG_BOW_FLAMEARROW02 ):
	case XI_INDEX( 544, XI_SKILL_RAG_BOW_PIRCINGARROW02 ):
	case XI_INDEX( 545, XI_SKILL_RAG_BOW_POISONARROW02 ):
	case XI_INDEX( 546, XI_SKILL_RAG_BOW_SILENTARROW02 ):
			
	case XI_INDEX( 537, XI_SKILL_ACR_BOW_AIMEDSHOT02 ):
	case XI_INDEX( 538, XI_SKILL_ACR_BOW_SILENTSHOT02 ):
	case XI_INDEX( 539, XI_SKILL_ACR_BOW_ARROWRAIN02 ):
	case XI_INDEX( 540, XI_SKILL_ACR_BOW_AUTOSHOT02 ):

	case XI_INDEX( 112, XI_ITEM_RANGE_ATK1		): pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_INDEX( 113, XI_ITEM_RANGE_ATK2		): pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_INDEX( 114, XI_ITEM_RANGE_ATK3		): pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_INDEX( 115, XI_ITEM_RANGE_ATK4		): pObj = new CSfxItemRangeAtk1()   ;break;
		
	case XI_INDEX( 269, XI_SKILL_MAG_WIND_SWORDWIND01  ): pObj = new CSfxSkillMagSwordWind()     ; break;
	case XI_INDEX( 270, XI_SKILL_MAG_WIND_STRONGWIND01 ): pObj = new CSfxSkillMagStrongWind()    ; break;
	case XI_INDEX( 264, XI_SKILL_MAG_FIRE_BOOMERANG01  ): pObj = new CSfxSkillMagFireBoomerang() ; break;
	case XI_INDEX( 265, XI_SKILL_MAG_FIRE_HOTAIR01     ): pObj = new CSfxSkillMagHotAir()        ; break;

	case XI_INDEX( 1600, XI_NPC_RAN_SPITTL    ): 
	case XI_INDEX( 1601, XI_NPC_RAN_GAS       ): 
	case XI_INDEX( 1602, XI_NPC_RAN_MAGICBALL ): 
	case XI_INDEX( 1604, XI_NPC_RAN_MAGICBLUE ): 
	case XI_INDEX( 1605, XI_NPC_RAN_CARD      ): 
	case XI_INDEX( 1607, XI_NPC_RAN_CANNON    ): 
		pObj = new CSfxItemWandAtk1()   ;break;    
		
	case XI_INDEX( 1616, XI_NPCSP1RANBALL ):		pObj = new CSfxAtkStraight();		break;

	case XI_INDEX( 302, XI_SKILL_MAG_WATER_ICEMISSILE01   ): pObj = new CSfxSkillMagIceMissile()    ;break;

	
	case XI_INDEX( 303, XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01 ): 	pObj = new CSfxSkillMagLightningBall();		break;
	case XI_INDEX( 304, XI_SKILL_MAG_EARTH_SPIKESTONE01 ):	pObj = new CSfxSkillMagSpikeStone();		break;
	case XI_INDEX( 230, XI_SKILL_MER_ONE_GUILOTIN02 ): 	// 필참 - 파워웨이브.
		pObj = new CSfxShootWave();
		break;
		
	case 0:		break;
	default:	// 그외 모든 디폴트.
		pObj = new CSfxShoot();		
		break;		// 일반적인 쏘고 날아가서 맞는 이펙트는 이걸로 통일하자.
	}

	if( pObj )
	{
		D3DXVECTOR3 vPosShoot = vPosSrc;	vPosShoot.y += 1.0f;
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosShoot, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModelElem이 없음" ), dwSfxObj );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );

			pObj->SetDir( TRUE );		// 방향성을 갖게 하자.

			// 오브젝트 생성후 후처리.
			switch( dwSfxObj )
			{
			case XI_INDEX( 230, XI_SKILL_MER_ONE_GUILOTIN02 ): 	// 필참 - 파워웨이브.
				{
					CSfxShootWave *pSfxShootWave = (CSfxShootWave *)pObj;
					pSfxShootWave->SetHitSfx( XI_INDEX( 230, XI_SKILL_MER_ONE_GUILOTIN02 ), idSrc, idDest, 2.0f );
					pSfxShootWave->SetDir( TRUE );
				}
				break;
			}

			// 월드에 오브젝트 추가.
			if( g_WorldMng.Get()->AddObj( pObj ) == FALSE )
			{
				SAFE_DELETE( pObj );
			}
		}
		else
		{
			SAFE_DELETE( pObj );
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

CSfx* CreateItemReadySfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 const D3DXVECTOR3& vPosSrc, OBJID idSrc, const D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
#ifndef __WORLDSERVER
	pObj = new CSfxReady();
	if( pObj )
	{
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel() == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModel이 없음" ), dwSfxObj );
			}
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%d sfx의 m_pModelElem이 없음" ), dwSfxObj );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );	

			if( g_WorldMng.Get()->AddObj(pObj) == FALSE )
			{
				SAFE_DELETE( pObj );
			}

		} else
			SAFE_DELETE( pObj );
	}
#endif // not World

	return pObj;
}

#endif //__CLIENT

#ifdef __LAYER_1015
CMover *CreateMover( CWorld *pWorld, DWORD dwID, const D3DXVECTOR3 &vPos, int nLayer )
#else	// __LAYER_1015
CMover *CreateMover( CWorld *pWorld, DWORD dwID, D3DXVECTOR3 vPos )
#endif	// __LAYER_1015
{
	CMover *pMover;

	pMover = (CMover *)CreateObj( D3DDEVICE, OT_MOVER, dwID );
	if( pMover == NULL )	
		return NULL;
	pMover->SetPos( vPos );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();
	
	pMover->AddItToGlobalId();
	if( pWorld->ADDOBJ( pMover, FALSE, nLayer ) == FALSE )
	{
		SAFE_DELETE( pMover );
	}

	return pMover;
}


#ifdef __ENCHANT_BARUNA16
CMover *	CreateNPC( const OBJID OwnerOBJID, CWorld & World, const TCHAR * pCharacterKey, 
					const D3DXVECTOR3 &vPos, const DWORD dwMoverIndex, const DWORD dwBelligerence, const int nLayer 
												  )
{
	CMover * pMover = (CMover *)CreateObj( D3DDEVICE, OT_MOVER, dwMoverIndex );
	if( pMover == NULL )	
		return NULL;

	pMover->SetPos( vPos );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();

	// NPC 세팅
	::ZeroMemory( pMover->m_szCharacterKey, sizeof( pMover->m_szCharacterKey ) );
	FLStrncpy( pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ), pCharacterKey, _countof( pMover->m_szCharacterKey ) );
	pMover->InitNPCProperty();
#ifdef __WORLDSERVER
	pMover->InitCharacter( pMover->GetCharacter() );
	pMover->SetNPCOwner( OwnerOBJID );
#endif 
	pMover->m_dwBelligerence		= dwBelligerence;

	// 기본 세팅
	pMover->AddItToGlobalId();

	if( World.ADDOBJ( pMover, FALSE, nLayer ) == FALSE )
	{
		SAFE_DELETE( pMover );
	}

	return pMover;
}
#endif //__ENCHANT_BARUNA16

CObj* CreateObj( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwObjType, DWORD dwObjIndex, BOOL bInitProp )
{
	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj;			break;
		case OT_CTRL : pObj = new CCommonCtrl;	break;
		case OT_SFX  : 
			#ifdef __CLIENT
				pObj = new CSfx; 
			#else
				return NULL;
			#endif
			break;  
		case OT_ITEM : pObj = new CItem;		break;
		case OT_MOVER: pObj = new CMover;		break;
		case OT_SHIP:	
			switch( dwObjIndex )
			{
			case 4:		pObj = new CShipLoop;	break;
			default:	pObj = new CShip;		break;
			}
			break;
	}
	pObj->m_dwType = dwObjType;

	if( pObj && pObj->GetType() == OT_SFX )
	{
		#ifdef __CLIENT
			((CSfx*)pObj)->SetSfx( pd3dDevice, dwObjIndex, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID ); 
		#endif
		return pObj;
	}
	else if( pObj && pObj->SetIndex( pd3dDevice, dwObjIndex, bInitProp ) == TRUE )
	{
		pObj->SetMotion( MTI_STAND );
		// 스피드를 설정하려면 현재로선 이수밖에....좆치안타.
		if( dwObjType == OT_MOVER )
		{
			CMover *pMover = (CMover *)pObj;
			pMover->m_pActMover->m_fSpeed = pMover->GetProp()->fSpeed;
//sun: 9,	//__AI_0509
			MoverProp* pProp	= pMover->GetProp();
			if( pProp->dwAI == AII_MONSTER 
				|| pProp->dwAI == AII_PARTY_AGGRO_LEADER )
			{
				pMover->m_pActMover->m_fSpeed	/= 2.0F;
//				if( pProp->dwClass == RANK_SUPER )
//					pMover->m_pActMover->m_fSpeed	*= 1.5F;
			}

		}
		return pObj;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "type=%d, index=%d, bInitProp=%d" ), dwObjType, dwObjIndex, bInitProp );
	SAFE_DELETE( pObj );
	return NULL;
}


CObj* ReadObj( CFileIO* pFile )
{
	DWORD dwObjType;
	pFile->Read( &dwObjType, sizeof( DWORD ), 1 );

	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj; break;
		case OT_CTRL : pObj = new CCommonCtrl; break;
		case OT_SFX  : 
			#ifdef __CLIENT
				pObj = new CSfx; 
			#endif
			break;  
		case OT_ITEM : pObj = new CItem; break;
		case OT_MOVER:				// 파일에서 읽어들이는 것은 모두 NPC
			pObj = new CMover; 
			((CMover *)pObj)->InitNPCProperty();
			break;
		case OT_SHIP:	pObj = new CShip;	break;
	}

	if( pObj ) 
	{
		pObj->Read( pFile );
		return pObj;
	}
	return NULL;
}
BOOL CheckVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion )
{
	CHAR szName[32];
	CHAR szVersion[32];

	fread( szName, sizeof( szName ), 1, fp );
	fread( szVersion, sizeof( szVersion ), 1, fp );

	if( !strcmp( szName, lpszName ) && !strcmp( szVersion, lpszVersion) )
		return TRUE;

	return FALSE;
}
BOOL WriteVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion )
{
	if( strlen( lpszName ) >= 32 ||  strlen( lpszVersion ) >= 32 )
		return FALSE;
	fwrite( lpszName, sizeof( 32 ), 1, fp );
	fwrite( lpszVersion, sizeof( 32 ), 1, fp );
	return TRUE;
}
BOOL CheckVersion( FILE* fp, DWORD dwVersion )
{
	DWORD dwVer;
	fread( &dwVer, sizeof( DWORD ), 1, fp );

	if( dwVer == dwVersion )
		return TRUE;

	return FALSE;
}
BOOL WriteVersion( FILE* fp, DWORD dwVersion  )
{
	fwrite( &dwVersion, sizeof( DWORD ), 1, fp );
	return TRUE;
}




















