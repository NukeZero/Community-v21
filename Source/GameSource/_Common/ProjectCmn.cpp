#include "stdafx.h"
#include "lang.h"
#include "langman.h"

#if !( defined(__DBSERVER) ) 
	#include "project.h"
#endif	// __DBSERVER

CString GetLangFileName( int nLang, int nType )
{
	char* file[] =
		{ "Filter", "InvalidName", "Notice", "GuildCombatTEXT_1", "GuildCombatTEXT_2", "GuildCombatTEXT_3", "GuildCombatTEXT_4", "GuildCombatTEXT_5", "GuildCombatTEXT_6" 
			,"Letter"	//sun: 10, __RULE_0615

//sun: 11, 일대일 길드 대전
			,"GuildCombat1to1_TEXT1", "GuildCombat1to1_TEXT2", "GuildCombat1to1_TEXT3", "GuildCombat1to1_TEXT4", "GuildCombat1to1_TEXT5"

#ifdef __VENDOR_1106
			, "Letter1"
#endif	// __VENDOR_1106
		};

	CString fileName = file[nType];

	fileName	= fileName + CLangMan::GetInstance()->GetLangData( nLang )->szFileName;

	return fileName;
}

BOOL CProject::LoadJobItem( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	script.GetToken();
	int nJob, nSex, i, j, nItem;
	memset( m_jobItem, 0xff, sizeof( m_jobItem ) );
	while( script.tok != FINISHED )
	{
		if( script.Token == _T( "job" ) )
		{
			nJob = script.GetNumber();
			script.GetToken(); // {
			script.GetToken(); // sex
			j = 0;
			while( script.Token != _T( "}" ) )
			{
				if( script.Token == _T( "sex" ) )
				{
					nSex = script.GetNumber();
					script.GetToken(); // {
					i = 0; nItem = script.GetNumber();
					while( script.Token != _T( "}" ) )
					{
						m_jobItem[ nJob ].adwMale[ i ][ nSex ] = nItem;
						nItem = script.GetNumber();
						i++;
					} 
				}
				if( script.Token == _T( "item" ) )
				{
					m_jobItem[ nJob ].beginItem[ j ].dwItemType = script.GetNumber();
					m_jobItem[ nJob ].beginItem[ j ].dwItemId   = script.GetNumber();
					m_jobItem[ nJob ].beginItem[ j ].dwNum      = script.GetNumber();
					j++;
				}
				script.GetToken();
			}
		}
		script.GetToken();
	}
	return TRUE;
}

BOOL CProject::LoadPropMover( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Mover", "프리프", MB_OK );
		FLERROR_LOG( PROGRAM_NAME, _T( "프로퍼티 읽기실패 : Mover" ) );
#endif //__CLIENT
		return FALSE;
	}

	std::set<int> checker;

	TCHAR szLastName[256] = { 0 };

	int i = 0;
	int old;
	for( ;; )
	{
		old = i;
		i = scanner.GetNumber();		// id
		if( scanner.tok == FINISHED )
			break;

		if( i == 0 )
			continue;

		if( i<0 || i>=MAX_PROPMOVER )		// 범위체크 
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "too many mover property or invalid id:%d FILE:%s" ), i, lpszFileName );
			return FALSE;
		}

		if( !checker.insert( i ).second )	// id중복체크 
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Last read:%s, ID:%d is duplicated. 컬럼이 밀려도 이런에러가 난다. FILE:%s" ), szLastName, i, lpszFileName );
			return FALSE;
		}

		MoverProp* pProperty = &m_pPropMover[i];
		pProperty->dwID	= i;
		
		scanner.GetToken();
		FLStrcpy( szLastName, _countof( szLastName ), scanner.token );
		FLStrcpy( pProperty->szName, _countof( pProperty->szName ), scanner.token );

#if !defined(__DBSERVER)
		m_mapMVI.insert( std::map<std::string, DWORD>::value_type( pProperty->szName, pProperty->dwID ) );
#endif	// __DBSERVER

		pProperty->dwAI					= scanner.GetNumber();
		pProperty->dwStr				= scanner.GetNumber();
		pProperty->dwSta				= scanner.GetNumber();
		pProperty->dwDex				= scanner.GetNumber();
		pProperty->dwInt				= scanner.GetNumber();
		pProperty->dwHR					= scanner.GetNumber();
		pProperty->dwER					= scanner.GetNumber();
		pProperty->dwRace				= scanner.GetNumber();
		pProperty->dwBelligerence		= scanner.GetNumber();
		pProperty->dwGender				= scanner.GetNumber();
		pProperty->dwLevel				= scanner.GetNumber();
		pProperty->dwFlightLevel		= scanner.GetNumber();
		pProperty->dwSize				= scanner.GetNumber();
		pProperty->dwClass				= scanner.GetNumber();
		pProperty->bIfParts				= scanner.GetNumber();

		if( pProperty->bIfParts == NULL_ID )
			FLERROR_LOG( PROGRAM_NAME, _T( "bIfParts엔 -1을 넣으면 안된다. %s 컬럼이 밀렸거나 워킹폴더를 잘못지정했을수도있음" ), pProperty->szName );

		pProperty->nChaotic				= scanner.GetNumber();
#ifdef __S1108_BACK_END_SYSTEM
		pProperty->dwUseable			= scanner.GetNumber();
#else // __S1108_BACK_END_SYSTEM
		pProperty->dwDefExtent			= scanner.GetNumber();
#endif // __S1108_BACK_END_SYSTEM
		pProperty->dwActionRadius		= scanner.GetNumber();
		pProperty->dwAtkMin				= scanner.GetNumber();
		pProperty->dwAtkMax				= scanner.GetNumber();
		pProperty->dwAtk1				= scanner.GetNumber();
		pProperty->dwAtk2				= scanner.GetNumber();
		pProperty->dwAtk3				= scanner.GetNumber();
		pProperty->dwAtk4				= scanner.GetNumber();
//sun: 9,	//__AI_0509
		pProperty->fFrame	= scanner.GetFloat();	// -1이면 가중치 영향 없음. 1.0 기본 값
		if( abs( -1.0F  - pProperty->fFrame ) < 0.000001F )
			pProperty->fFrame	= 1.0F;
		pProperty->dwOrthograde	= scanner.GetNumber();
		pProperty->dwThrustRate			= scanner.GetNumber();

		pProperty->dwChestRate			= scanner.GetNumber();
		pProperty->dwHeadRate			= scanner.GetNumber();
		pProperty->dwArmRate			= scanner.GetNumber();
		pProperty->dwLegRate			= scanner.GetNumber();

		pProperty->dwAttackSpeed		= scanner.GetNumber();
		pProperty->dwReAttackDelay		= scanner.GetNumber();
		pProperty->dwAddHp				= scanner.GetNumber();
		pProperty->dwAddMp				= scanner.GetNumber();
		pProperty->dwNaturalArmor		= scanner.GetNumber();
		pProperty->nAbrasion			= scanner.GetNumber();
		pProperty->nHardness			= scanner.GetNumber();
		pProperty->dwAdjAtkDelay		= scanner.GetNumber();
		
		pProperty->eElementType			= static_cast<SAI79::ePropType>(scanner.GetNumber());	// 정수타입과 enum	타입이 틀려서 타입 변환을 허용함.
		pProperty->wElementAtk			= static_cast<short>(scanner.GetNumber());				// 정수타입과 short	타입이 틀려서 타입 변환을 허용함.

		pProperty->dwHideLevel			= scanner.GetNumber();
		pProperty->fSpeed				= scanner.GetFloat();
		if ( pProperty->dwID == OBJECT_INDEX( 12, MI_FEMALE ) || pProperty->dwID == OBJECT_INDEX( 11, MI_MALE ) )
			pProperty->fSpeed = 0.1f;
		pProperty->dwShelter			= scanner.GetNumber();
		pProperty->dwFlying				= scanner.GetNumber();
		pProperty->dwJumpIng			= scanner.GetNumber();
		pProperty->dwAirJump			= scanner.GetNumber();
		pProperty->bTaming				= scanner.GetNumber();
		pProperty->dwResisMgic			= scanner.GetNumber();

		pProperty->nResistElecricity	= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistFire			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWind			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWater			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistEarth			= (int)(scanner.GetFloat() * 100.0f);

		pProperty->dwCash				= scanner.GetNumber();
		pProperty->dwSourceMaterial		= scanner.GetNumber();
		pProperty->dwMaterialAmount		= scanner.GetNumber();
		pProperty->dwCohesion			= scanner.GetNumber();
		pProperty->dwHoldingTime		= scanner.GetNumber();
		pProperty->dwCorrectionValue	= scanner.GetNumber();
		pProperty->nExpValue			= scanner.GetExpInteger();
		pProperty->nFxpValue			= scanner.GetNumber();
		pProperty->nBodyState			= scanner.GetNumber();
		pProperty->dwAddAbility			= scanner.GetNumber();
		pProperty->bKillable			= scanner.GetNumber();
		
		pProperty->dwVirtItem[0]		= scanner.GetNumber();
		pProperty->dwVirtItem[1]		= scanner.GetNumber();
		pProperty->dwVirtItem[2]		= scanner.GetNumber();
		pProperty->bVirtType[0]			= scanner.GetNumber();
		pProperty->bVirtType[1]			= scanner.GetNumber();
		pProperty->bVirtType[2]			= scanner.GetNumber();

		pProperty->dwSndAtk1			= scanner.GetNumber();
		pProperty->dwSndAtk2 			= scanner.GetNumber();

		pProperty->dwSndDie1 			= scanner.GetNumber();
		pProperty->dwSndDie2 			= scanner.GetNumber();

		pProperty->dwSndDmg1 			= scanner.GetNumber();
		pProperty->dwSndDmg2 			= scanner.GetNumber();
		pProperty->dwSndDmg3 			= scanner.GetNumber();

		pProperty->dwSndIdle1 			= scanner.GetNumber();
		pProperty->dwSndIdle2 			= scanner.GetNumber();
		scanner.GetToken();

		pProperty->dwAreaColor			= scanner.GetNumber();

		scanner.GetToken();

		FLStrcpy( pProperty->szMarkFileName, _countof( pProperty->szMarkFileName ), scanner.token );

		pProperty->nMadrigalGiftPoint	= scanner.GetNumber();

#ifdef __S1108_BACK_END_SYSTEM
		pProperty->m_fHitPoint_Rate = 1.0f;		
		pProperty->m_fAttackPower_Rate = 1.0f;
		pProperty->m_fDefence_Rate = 1.0f;		
		pProperty->m_fExp_Rate = 1.0f;			
		pProperty->m_fItemDrop_Rate = 1.0f;
		pProperty->m_fPenya_Rate = 1.0f;			
		pProperty->m_bRate = FALSE;
#endif // __S1108_BACK_END_SYSTEM
	}

	if( checker.size() > 0 )
		m_nMoverPropSize = *(checker.rbegin()) + 1;
	
	/*
	#ifdef __INTERNALSERVER
	{
		int nIndex = 20; // MI_AIBATT1 
		MoverProp* pProperty = &m_pPropMover[ nIndex ];
		pProperty->dwAI = AII_VER2_TYPE0; 
	}	
	#endif
	*/

#ifdef __CLIENT
	m_bLoadedMoverProp = TRUE;
#endif // __CLIENT

	return TRUE;
}

BOOL CProject::LoadPropSkill( LPCTSTR lpszFileName, CIndexArray< SkillProp >* apObjProp )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
	#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "LoadPropSkill", "flyff", MB_OK );
		FLERROR_LOG( PROGRAM_NAME, _T( "file open error" ) );
	#endif //__CLIENT
		return FALSE;
	}

	SkillProp prop;
	ZeroMemory( &prop, sizeof(prop) );

	int nVer = scanner.GetNumber();	// version
	while( scanner.tok != FINISHED )
	{
		DWORD dwID = scanner.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s ID:%d last_read(ID:%d name:%s)" ), lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

//#ifdef __CLIENT
		prop.nVer	= nVer;
//#endif	// __CLIENT
		prop.dwID = dwID;
		scanner.GetToken();
//		_tcscpy( prop.szName, scanner.token );
		scanner.Token.TrimRight();
		FLStrcpy( prop.szName, _countof( prop.szName ), (LPCSTR)scanner.Token );

 		prop.dwNum_				= scanner.GetNumber();            
		prop.dwPackMax_			= scanner.GetNumber();      
		prop.dwSkillKind1		= scanner.GetNumber();      
		prop.dwSkillKind2		= scanner.GetNumber();      
		prop.dwSkillKind3_		= scanner.GetNumber();      
		prop.dwSkillJob_			= scanner.GetNumber();      
		prop.bPermanence_		= scanner.GetNumber();      
		prop.dwUseable_			= scanner.GetNumber();      
		prop.dwItemSex_			= scanner.GetNumber();      
		prop.dwCost_				= scanner.GetNumber();      
		prop.dwEndurance_		= scanner.GetNumber(); 
		prop.nAbrasion_			= scanner.GetNumber();
		prop.nMaxRepair_			= scanner.GetNumber();
		prop.dwNeedItemHanded			= scanner.GetNumber();      
		prop.dwFlag_				= scanner.GetNumber();      
		prop.dwParts_			= scanner.GetNumber();      
		prop.dwPartsub_			= scanner.GetNumber();      
		prop.bPartsFile_			= scanner.GetNumber();      
		prop.dwExclusive_		= scanner.GetNumber();      
		prop.dwBasePartsIgnore_	= scanner.GetNumber();      
		prop.dwItemLV_			= scanner.GetNumber();      
		prop.dwSkillRare_			= scanner.GetNumber();   
		prop.dwShopAble_			= scanner.GetNumber();
		prop.nSkillLog_				= scanner.GetNumber();
		prop.bCharged_			= scanner.GetNumber();
		prop.bCharged_	= ( prop.bCharged_ != 1? 0: 1 );
		prop.dwNeedItemBulletKind2	= scanner.GetNumber();      
		prop.dwNeedItemKind3			= scanner.GetNumber();      
		prop.dwAbilityMin		= scanner.GetNumber();      
		prop.dwAbilityMax		= scanner.GetNumber();      
		prop.eSkillType			= static_cast<SAI79::ePropType>(scanner.GetNumber());
		prop.wItemEatk_			= static_cast<short>(scanner.GetNumber());
		prop.dwParry_			= scanner.GetNumber();
		prop.dwblockRating_		= scanner.GetNumber();
		prop.nAddSkillMin_		= scanner.GetNumber();
		prop.nAddSkillMax_		= scanner.GetNumber();
		prop.dwAtkStyle			= scanner.GetNumber();      
		prop.dwWeaponType_		= scanner.GetNumber();
		prop.dwItemAtkOrder1_	= scanner.GetNumber();      	
		prop.dwItemAtkOrder2_	= scanner.GetNumber();      	
		prop.dwItemAtkOrder3_	= scanner.GetNumber();      	
		prop.dwItemAtkOrder4_	= scanner.GetNumber();
		prop.tmContinuousPain	= scanner.GetNumber();
		prop.nShellQuantity_		= scanner.GetNumber();
		prop.dwRecoil_			= scanner.GetNumber();
		prop.dwLoadingTime_		= scanner.GetNumber();
		prop.nAdjHitRate_		= scanner.GetNumber();
		prop.fAttackSpeed_		= scanner.GetFloat();
		prop.dwDmgShift			= scanner.GetNumber();
		prop.dwAttackRange		= scanner.GetNumber();
		prop.nProbability		= scanner.GetNumber();
		prop.dwDestParam[0]		= scanner.GetNumber();
		prop.dwDestParam[1]		= scanner.GetNumber();
		prop.dwDestParam[2]		= scanner.GetNumber();

		prop.nAdjParamVal[0]	= scanner.GetNumber();
		prop.nAdjParamVal[1]	= scanner.GetNumber();
		prop.nAdjParamVal[2]	= scanner.GetNumber();

		prop.dwChgParamVal[0]	= scanner.GetNumber();
		prop.dwChgParamVal[1]	= scanner.GetNumber();
		prop.dwChgParamVal[2]	= scanner.GetNumber();

		prop.nDestData1[0]		= scanner.GetNumber();
		prop.nDestData1[1]		= scanner.GetNumber();
		prop.nDestData1[2]		= scanner.GetNumber();
		prop.dwActiveSkill		= scanner.GetNumber();
		prop.dwActiveSkillLv	= scanner.GetNumber();
		prop.dwActiveSkillRate	= scanner.GetNumber();
		prop.dwReqMp_			= scanner.GetNumber();
		prop.dwReqFp_			= scanner.GetNumber();
        prop.dwReqDisLV			= scanner.GetNumber();
		prop.dwReqSkill1			= scanner.GetNumber();
		prop.dwReqSkillLevel1	= scanner.GetNumber();
		prop.dwReqSkill2			= scanner.GetNumber();
		prop.dwReqSkillLevel2	= scanner.GetNumber();
        prop.dwSkillReadyType_	= scanner.GetNumber();
		prop.dwSkillReady		= scanner.GetNumber();
		prop.dwSkillRange		= scanner.GetNumber();
		prop.dwSfxElemental_		= scanner.GetNumber();
		prop.dwSfxObj			= scanner.GetNumber();
		prop.dwSfxObj2			= scanner.GetNumber();
		prop.dwSfxObj3			= scanner.GetNumber();
		prop.dwSfxObj4			= scanner.GetNumber();
		prop.dwSfxObj5			= scanner.GetNumber();
		prop.dwUseMotion		= scanner.GetNumber();
		prop.dwCircleTime_		= scanner.GetNumber();
		prop.dwSkillTime		= scanner.GetNumber();
		prop.dwExeTarget		= scanner.GetNumber();
		prop.dwUseChance		= scanner.GetNumber();
		prop.dwSpellRegion		= scanner.GetNumber();
		prop.dwSkillSpellType		= scanner.GetNumber();
		prop.dwReferStat1		= scanner.GetNumber();
		prop.dwReferStat2		= scanner.GetNumber();
		prop.dwReferTarget1		= scanner.GetNumber();
		prop.dwReferTarget2		= scanner.GetNumber();
		prop.dwReferValue1		= scanner.GetNumber();
		prop.dwReferValue2		= scanner.GetNumber();
		prop.dwSkillType		= scanner.GetNumber();
		prop.nItemResistElecricity_	= (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistFire_	    = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistWind_	    = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistWater_	    = (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistEarth_	    = (int)(scanner.GetFloat() * 100.0f);
		prop.nEvildoing			= scanner.GetNumber();
		prop.dwExpertLV_		= scanner.GetNumber();
		prop.dwExpertMax		= scanner.GetNumber();
		prop.dwSubDefine		= scanner.GetNumber();
		prop.dwReqPartyPoint	= scanner.GetNumber();
		prop.dwComboStyle		= scanner.GetNumber();
		prop.fFlightSpeed_		= scanner.GetFloat();
		prop.fFlightLRAngle_		= scanner.GetFloat();
		prop.fFlightTBAngle_		= scanner.GetFloat();
		prop.dwFlightLimit_		= scanner.GetNumber();
		prop.dwFFuelReMax_		= scanner.GetNumber();
		prop.dwAFuelReMax_		= scanner.GetNumber();
		prop.dwFuelRe_			= scanner.GetNumber();
		prop.dwLimitLevel1_		= scanner.GetNumber();
		prop.nReflect_			= scanner.GetNumber();
		prop.dwSndAttack1		= scanner.GetNumber();
		prop.dwSndAttack2_		= scanner.GetNumber();
		
		// szIcon
		scanner.GetToken();
		scanner.GetToken();
	#ifdef __CLIENT
		FLStrncpy( prop.szIcon, _countof( prop.szIcon ), scanner.token, _countof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
	#endif

		scanner.GetToken();

		// dwQuestId
		prop.dwQuestId_	= scanner.GetNumber();

		// szTextFileName
		scanner.GetToken();
		scanner.GetToken();
		FLStrcpy( prop.szTextFileName_, _countof( prop.szTextFileName_ ), scanner.token );
		scanner.GetToken();

		// szCommand
		scanner.GetToken();
	#ifdef __CLIENT
		FLStrncpy( prop.szCommand, _countof( prop.szCommand ), scanner.token, _countof( prop.szCommand )-1 );
		prop.szCommand[ _countof(prop.szCommand)-1 ] = 0;
	#endif // __CLIENT

		prop.byBuffTickType			= static_cast< BYTE >( scanner.GetNumber() );	// 버프 타입

		prop.dwAdjMonsterGrade		= static_cast<DWORD>( scanner.GetNumber() );	// 적용 가능 몬스터 등급

		prop.dwEquipItemKeepSkill	= static_cast<DWORD>( scanner.GetNumber() );

		prop.bCanUseActionSlot		= static_cast<bool>( scanner.GetNumber() != 0 );


		if( nVer <= __VER )
		{
			m_mapII.insert( std::map<std::string, DWORD>::value_type( prop.szName, prop.dwID ) );
			apObjProp->SetAt( prop.dwID, &prop);
		}

		nVer = scanner.GetNumber();	// version; 
	}
	apObjProp->Optimize();
	return TRUE;
}

BOOL CProject::LoadText( LPCTSTR lpszFileName )
{
	tagColorText colorText, *pColorText;

	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD. FileName:(%s) ]" ), lpszFileName );
		return FALSE;
	}

	CStringArray strArray;
	CDWordArray  colorArray;
	DWORD dwId = scanner.GetNumber();

	do {
		DWORD dwColor = scanner.GetNumber(); // color
		scanner.GetToken();
		if( *scanner.token  == '{' )
		{

			scanner.GetToken();
			CString str	= scanner.token;
			str.Replace( "\"", "" );
//			if( str.IsEmpty() )
//				str	= "Empty";
#ifdef _DEBUG
			if( strArray.GetSize() > (int)( dwId ) )
				if( strArray.GetAt( dwId ).IsEmpty() == FALSE )
					FLERROR_LOG( PROGRAM_NAME, _T( "같은 아이디 존재 %d - %s" ), dwId, str );
#endif	// _DEBUG	
			strArray.SetAtGrow( dwId, str );
			colorArray.SetAtGrow( dwId, dwColor );
			scanner.GetToken();	// }
		}
		dwId = scanner.GetNumber();	// next
	} while( scanner.tok != FINISHED );

	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		if( strArray.GetAt( i ).IsEmpty() == FALSE )
		{
			m_colorText.SetAt( i, &colorText );

			pColorText = m_colorText.GetAt( i );
			pColorText->dwColor = colorArray.GetAt( i );
			pColorText->lpszData = _strdup( strArray.GetAt( i ) ) ;
		}
	}
	m_colorText.Optimize();
	return TRUE;
}

CString CProject::GetLangScript( CScript& script )
{
	CString string;
	script.GetToken();
	string	= script.Token;
	script.GetToken();	// )
	script.GetToken(); // ;
	if( string.IsEmpty() )
		string = " ";
	return string;
}

bool CProject::LoadStrings()
{
	CScript kScanner;

	const TCHAR* pszPreLoadFileName	= _T( "PreLoadText.inc" );
	if( kScanner.Load( pszPreLoadFileName, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD TEXT LIST. FileName:(%s) ]" ), pszPreLoadFileName );
		return false;
	}

	DWORD dwLine	= 0;

	kScanner.GetToken();	// COMMON, SERVER, CLIENT

	while( kScanner.tok != FINISHED )
	{
		++dwLine;

		TCHAR szCategory[ 32 ] = { 0, };
		FLStrcpy( szCategory, _countof( szCategory ), kScanner.Token );

		kScanner.GetToken();	// FILE NAME
		TCHAR szTextFileName[ _MAX_PATH + _MAX_FNAME ] = { 0, };
		FLStrcpy( szTextFileName, _countof( szTextFileName ), kScanner.Token );

#ifdef __SERVER
		if( _tcscmp( szCategory, _T( "CLIENT" ) ) != 0 )
#else	// !__SERVER
		if( _tcscmp( szCategory, _T( "SERVER" ) ) != 0 )
#endif // __SERVER
		{
			CScript kScript;
			if( kScript.Load( szTextFileName ) == TRUE )
			{
				kScript.LoadString();
			}
			else
			{
				FLWARNING_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD TEXT FILE. FileName:(%s), Line:(%d), TextFileName:(%s) ]" )
					, pszPreLoadFileName, dwLine, szTextFileName );
			}
		}

		kScanner.GetToken();	// COMMON, SERVER, CLIENT
	}

	const BOOL bRet = LoadText( "textClient.inc" );

	return ( dwLine > 0 && bRet == TRUE ) ? true : false;

// 	char* lpszFiles[] = {
// 		"character.txt.txt",
// 		"character-etc.txt.txt",
// 		"character-school.txt.txt",
// 		"etc.txt.txt",
// 		"propCtrl.txt.txt",
// 		"propItem.txt.txt",
// 		"propItemEtc.txt.txt",
// 		"propKarma.txt.txt",
// 		"propMotion.txt.txt",
// 		"propMover.txt.txt",
// 		"propQuest.txt.txt",
// 		"propQuest-DungeonandPK.txt.txt",
// 		"propQuest-RequestBox2.txt.txt",
// 		"propQuest-RequestBox.txt.txt",
// 		"propQuest-Scenario.txt.txt",
// 		"propSkill.txt.txt",
// 		"propTroupeSkill.txt.txt",
// 		"resData.txt.txt",
// 		"textClient.txt.txt",
// 		"textEmotion.txt.txt",
// 		"world.txt.txt",
// 		"Client\\treeHelp.txt.txt",
// 		"Client\\tip.txt.txt",
// 		"Client\\PatchClient.txt.txt",
// 		"Client\\InstantHelp.txt.txt",
// 		"Client\\help.txt.txt",
// 		"Client\\Guide.txt.txt",
// 		"Client\\tutorial.txt.txt",		//sun: 12, 튜토리얼 개선
// 
// //		"Client\\GameGuard.txt.txt",
// 		"Client\\faq.txt.txt",
// 		"World\\WdVolcane\\WdVolcane.txt.txt",
// 		"World\\WdMadrigal\\wdMadrigal.txt.txt",
// 		"World\\WdKebaras\\WdKebaras.txt.txt",
// 		"World\\WdGuildWar\\WdGuildWar.txt.txt",
// 		"World\\WdEvent01\\WdEvent01.txt.txt",
// 		"World\\DuMuscle\\DuMuscle.txt.txt",
// 		"World\\DuKrr\\DuKrr.txt.txt",
// 		"World\\DuFlMas\\DuFlMas.txt.txt",
// 		"World\\DuDaDk\\DuDaDk.txt.txt",
// 		"World\\DuBear\\DuBear.txt.txt",
// 		"World\\DuSaTemple\\DuSaTemple.txt.txt",
// 		"World\\DuSaTempleBoss\\DuSaTempleBoss.txt.txt"
// //sun: 9, // __JEFF_9
// 		,"World\\WdVolcane\\WdVolcane.txt.txt"
// 		,"World\\WdVolcaneRed\\WdVolcaneRed.txt.txt"
// 		,"World\\WdVolcaneYellow\\WdVolcaneYellow.txt.txt"
// 
// #if !defined(__DBSERVER)
// 		,"World\\WdArena\\WdArena.txt.txt"
// #endif	// __DBSERVER
// 
// 		,"lordskill.txt.txt"	//sun: 12, 군주
// 
// //sun: 12, 비밀의 방
// 		,"World\\WdHeaven01\\wdheaven01.txt.txt"
// 		,"World\\WdHeaven02\\wdheaven02.txt.txt"
// 		,"World\\WdHeaven03\\wdheaven03.txt.txt"
// 		,"World\\WdHeaven04\\wdheaven04.txt.txt"
// 		,"World\\WdHeaven05\\wdheaven05.txt.txt"
// 		,"World\\WdHeaven06\\wdheaven06.txt.txt"
// 		,"World\\WdHeaven06_1\\wdheaven06_1.txt.txt"
// 		, "honorList.txt.txt"	//sun: 13, 달인
// 
// #if !defined(__DBSERVER)
// 		, "World\\WdCisland\\WdCisland.txt.txt"
// #endif // __DBSERVER
// 
// 		, "World\\DuOminous\\duominous.txt.txt"
// 		, "World\\DuOminous_1\\duominous_1.txt.txt"
// 
// 		, "World\\WdGuildhousesmall\\WdGuildhousesmall.txt.txt"		// 소형 길드하우스
// 		, "World\\WdGuildhousemiddle\\WdGuildhousemiddle.txt.txt"	// 중형 길드하우스
// 		, "World\\WdGuildhouselarge\\WdGuildhouselarge.txt.txt"		// 대형 길드하우스
// 		, "World\\DuDreadfulCave\\DuDreadfulCave.txt.txt"			// 추가 인던(드래드풀 케이브)
// 		, "World\\DuRustia\\DuRustia.txt.txt"						// 추가 인던(러스티아 일반)
// 		, "World\\DuRustia_1\\DuRustia_1.txt.txt"					// 추가 인던(러스티아 마스터)
// 
// #ifdef __IMPROVE_MAP_SYSTEM
// 		, "propMapComboBoxData.txt.txt"
// #endif // __IMPROVE_MAP_SYSTEM
// 
// #ifdef __SKILL_UI16
// 		, "World\\WdRartesia\\WdRartesia.txt.txt"			// 16차 라르테시아
// 		, "World\\DuBehamah\\DuBehamah.txt.txt"				// 16차 베히모스 신전
// #endif // __SKILL_UI16
// 		, "World\\DuKalgas\\DuKalgas.txt.txt"				// 17차 칼가스
// 		, "World\\WdColosseum\\WdColosseum.txt.txt"			// 17차 콜로세움
// 		, "World\\DuUpresia\\DuUpresia.txt.txt"
// 		, "World\\DuUpresia_1\\DuUpresia_1.txt.txt"
// 		, "World\\DuSanpres\\DuSanpres.txt.txt"
// 		, "World\\DuSanpres_1\\DuSanpres_1.txt.txt"
// 		, "World\\DuHerneos\\DuHerneos.txt.txt"
// 		, "World\\DuHerneos_1\\DuHerneos_1.txt.txt"
// 		, "World\\WdFwc\\WdFwc.txt.txt"
// 		, "World\\WdMarket\\WdMarket.txt.txt"
// 		, "World\\WdDarkRartesia\\WdDarkRartesia.txt.txt"
// 
// 	};
// 
// 	int n = sizeof(lpszFiles) / sizeof(lpszFiles[0]);
// 	for( int i=0; i<n; ++i )
// 	{
// 		CScript s;
// 		if( s.Load( lpszFiles[i] ) == TRUE )
// 			s.LoadString();
// 	}		
// 
// 	LoadText( "textClient.inc" );
}

bool	CProject::LoadDefines()
{
	CScript kScanner;

	const TCHAR* pszPreLoadFileName	= _T( "PreLoadDefine.inc" );
	if( kScanner.Load( pszPreLoadFileName, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD DEFINE LIST. FileName:(%s) ]" ), pszPreLoadFileName );
		return false;
	}

	DWORD dwLine	= 0;

	kScanner.GetToken();	// COMMON, SERVER, CLIENT

	while( kScanner.tok != FINISHED )
	{
		++dwLine;

		TCHAR szCategory[ 32 ] = { 0, };
		FLStrcpy( szCategory, _countof( szCategory ), kScanner.Token );

		kScanner.GetToken();	// FILE NAME
		TCHAR szDefineFileName[ _MAX_PATH + _MAX_FNAME ] = { 0, };
		FLStrcpy( szDefineFileName, _countof( szDefineFileName ), kScanner.Token );

#ifdef __SERVER
		if( _tcscmp( szCategory, _T( "CLIENT" ) ) != 0 )
#else	// !__SERVER
		if( _tcscmp( szCategory, _T( "SERVER" ) ) != 0 )
#endif // __SERVER
		{
			CScript kScript;
			if( kScript.Load( szDefineFileName ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD DEFINE FILE. FileName:(%s), Line:(%d), DefineFile:(%s) ]" )
					, pszPreLoadFileName, dwLine, szDefineFileName );
				return false;
			}

			kScript.PreScan();
		}

		kScanner.GetToken();	// COMMON, SERVER, CLIENT
	}

	return ( dwLine > 0 ) ? true : false;

// 	const char* pszCDefines[] =
// 	{
// 		"define.h",
// 		"defineNeuz.h",
// 		"defineQuest.h",
// 		"defineJob.h",
// 		"defineItem.h",
// 		"defineWorld.h",
// 		"defineItemkind.h",
// 		"lang.h",
// 		"defineObj.h",
// 		"defineAttribute.h",
// 		"defineSkill.h",
// 		"defineText.h",
// 		"defineSound.h",
// 		"resdata.h",
// 		"WndStyle.h", 
// 		"definelordskill.h"	//sun: 12, 군주
// 		, "defineHonor.h"	//sun: 13, 달인
// 
// #ifdef __IMPROVE_MAP_SYSTEM
// 		, "ContinentDef.h"
// 		, "defineMapComboBoxData.h"
// #endif // __IMPROVE_MAP_SYSTEM
// 
// 		, "defineEventArena.h"
// 		, "defineItemGrade.h"
// 		, "defineItemType.h"
// 	};
// 
// 	for( int i = 0; i < _countof( pszCDefines ); ++i )
// 	{
// 		CScript kScript;
// 		if( kScript.Load( pszCDefines[i] ) == TRUE )
// 		{
// 			kScript.PreScan();
// 		}
// 	}

	// TODO txt defines
	/*
	const char* pszTxtDefines[] =
	{
	};

	for( int i = 0; i < _countof( pszTxtDefines ); ++i )
	{
		CScript kScript;
		if( kScript.LoadTxtDefines( pszTxtDefines[i] ) == FALSE )
		{
		}
	}
	*/
}

bool CProject::LoadPreFiles()
{
	const bool bRet = ( LoadDefines()
						&& LoadStrings()
						&& CLangMan::GetInstance()->Load( "propLang.txt" ) );

	return bRet;
}


//sun: 10, __RULE_0615
BOOL CProject::LoadInvalidName( void )
{
	CScanner s;
	TCHAR szName[64] = { 0, };
	
	CString strFilter;
	strFilter	= GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_INVALID );

	if( s.Load( strFilter ) ) 
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			FLStrcpy( szName, _countof( szName ), s.Token );
			FLStrLowercase( szName, _countof( szName ) );
			
			m_sInvalidNames.insert( (LPCSTR)szName );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CProject::IsInvalidName( LPCSTR szName )
{
	TCHAR pszName[ 64 ] = { 0, };
	FLStrcpy( pszName, _countof( pszName ), szName );
	FLStrLowercase( pszName, _countof( pszName ) );
//	string str	= pszName;
	CString str = pszName;
	for(std::set<std::string>::iterator i = m_sInvalidNames.begin(); i != m_sInvalidNames.end(); ++i )
	{
		std::string strstr = *i;
		
//		if( str.find( *i, 0 ) != -1 )
		if( str.Find( (*i).c_str(), 0 ) != -1 )
			return TRUE;
	}
	return FALSE;
}

#ifdef __VENDOR_1106
BOOL	CProject::LoadAllowedLetter( BOOL bVendor )
#else	// __VENDOR_1106
BOOL	CProject::LoadAllowedLetter( void )
#endif	// __VENDOR_1106
{
	CScanner s;
	CString strFile;

#ifdef __VENDOR_1106
	std::set<char>*	ptr	= NULL;
	if( bVendor )
	{
		ptr		= &m_sAllowedLetter2;
		strFile		= GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_ALLOWED_LETTER2 );
	}
	else
	{
		ptr		= &m_sAllowedLetter;
		strFile		= GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_ALLOWED_LETTER );
	}
#else	// __VENDOR_1106
	strFile		= GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_ALLOWED_LETTER );
	set<char>*	ptr		= &m_sAllowedLetter;
#endif	// __VENDOR_1106
	if( s.Load( strFile ) ) 
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token.GetLength() )
				ptr->insert( s.Token.GetAt( 0 ) );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL	CProject::IsAllowedLetter( LPCSTR szName, BOOL bVendor )	
{
#ifdef __VENDOR_1106
	std::set<char>* ptr		= NULL;
	if( bVendor )
		ptr		= &m_sAllowedLetter2;
	else
		ptr		= &m_sAllowedLetter;
#else	// __VENDOR_1106
	std::set<char>* ptr		= &m_sAllowedLetter;
#endif	// __VENDOR_1106

	if( !ptr->size() )	// 규칙이 없으면 무시
		return TRUE;

	int nLen	= lstrlen( szName );
	for( int i = 0; i < nLen; i++ )
	{
		char chLetter	= szName[i];
		std::set<char>::iterator it	= ptr->find( chLetter );
		if( it == ptr->end() )
		{
#ifndef __VENDOR_1106
			if( bVendor && ( chLetter == ' ' || chLetter == '+' ) )
				continue;
#endif	// __VENDOR_1106
			return FALSE;
		}
	}
	return TRUE;
}

void	CProject::Formalize( LPSTR szName, size_t cchName )
{
	int nLen	= lstrlen( szName );
	if( nLen == 0 )
		return;

	DWORD dwLanguage	= g_xFlyffConfig->GetMainLanguage();
	switch( dwLanguage )
	{
		case LANG_GER:
		case LANG_FRE:
			{
				FLStrLowercase( szName, cchName );	// 소문자로 변경
				char szBuffer[2];
				szBuffer[0]	= szName[0];
				szBuffer[1]	= '\0';
				FLStrUppercase( szBuffer, _countof( szBuffer ) );
				szName[0]	= szBuffer[0];
				break;
			}
		default:
			{
				break;
			}
	}
}

