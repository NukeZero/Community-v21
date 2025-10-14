#include "stdafx.h"
#include "lang.h"



//#ifndef __DBSERVER
#if !( defined(__DBSERVER) || defined(__VPW) ) 
 #include "project.h"
#else	// __DBSERVER
  #include "..\DatabaseServer\project.h"
#endif	// __DBSERVER





/*
job JOB_MERCENARY
{
	sex SEX_FEMALE
	{
		II_AMR_F_MER_HELMET1  
		II_AMR_F_MER_SUIT1    
		II_AMR_F_MER_GAUNTLET1
		II_AMR_F_MER_BOOTS1  
		II_WPN_SWD_BOKKEN
	}
	sex SEX_MALE
	{
		II_AMR_M_MER_HELMET1  
		II_AMR_M_MER_SUIT1    
		II_AMR_M_MER_GAUNTLET1
		II_AMR_M_MER_BOOTS1   
		II_WPN_SWD_BOKKEN
	}
	// item    type item                   num // 10개가 맥스 
	item ITYPE_CARD II_CARD_BLANK          1
	item ITYPE_CARD II_CARD_TEMPLATE       1
	item ITYPE_CUBE II_CUBE_NORMAL         1
	item ITYPE_CUBE II_CUBE_NORMAL         1
	item ITYPE_ITEM II_WPN_AXE_PRACTICE    1
	item ITYPE_ITEM II_WPN_SWD_BOKKEN      1
	item ITYPE_ITEM II_RIDE_BESOM          1
	item ITYPE_ITEM II_FOOD_LOLLIPOP       1
	item ITYPE_ITEM II_POT_REFRESHER_FRIST 1
}
typedef struct tagJOBITEM
{
	DWORD adwMale[ MAX_BEGINEQUIP ][ 2 ];
	BEGINITEM beginItem[ MAX_BEGINITEM ];

} JOBITEM,* LPJOBITEM;
*/

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

#if !defined(__DBSERVER)
JobProp* CProject::GetJobProp( int nIndex )
{
	if( nIndex < 0 || nIndex >= MAX_JOB )
	{
		LPCTSTR szErr = Error( "CProject::GetJobProp 범위침범. %d", nIndex );
		ADDERRORMSG( szErr );
		return NULL;
	}
	return &m_aPropJob[nIndex];
}

// propJob.inc를 읽어들인다.
BOOL CProject::LoadPropJob( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		return FALSE;
	}
	
	while( 1 )
	{
		int nJob = scanner.GetNumber();	
		if( scanner.tok == FINISHED )
			break;

		JobProp* pProperty = &m_aPropJob[nJob];
		pProperty->fAttackSpeed      = scanner.GetFloat();		
		pProperty->fFactorMaxHP      = scanner.GetFloat();		
		pProperty->fFactorMaxMP      = scanner.GetFloat();		
		pProperty->fFactorMaxFP      = scanner.GetFloat();		
		pProperty->fFactorDef        = scanner.GetFloat();		
		pProperty->fFactorHPRecovery = scanner.GetFloat();	
		pProperty->fFactorMPRecovery = scanner.GetFloat();	
		pProperty->fFactorFPRecovery = scanner.GetFloat();	
		pProperty->fMeleeSWD		 = scanner.GetFloat();	
		pProperty->fMeleeAXE		 = scanner.GetFloat();	
		pProperty->fMeleeSTAFF		 = scanner.GetFloat();	
		pProperty->fMeleeSTICK		 = scanner.GetFloat();	
		pProperty->fMeleeKNUCKLE	 = scanner.GetFloat();	
		pProperty->fMagicWAND		 = scanner.GetFloat();	
		pProperty->fBlocking		 = scanner.GetFloat();	
		pProperty->fMeleeYOYO        = scanner.GetFloat();	
		pProperty->fCritical         = scanner.GetFloat();	
	}
	return TRUE;
}
#endif //!__DBSERVER

BOOL CProject::LoadPropMover( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		return FALSE;
	}

	std::set<int> checker;

	TCHAR szLastName[256];
	int i = 0;
	memset( szLastName, 0, sizeof(szLastName) );
	int old;
	while( 1 )
	{
		old = i;
		i = scanner.GetNumber();		// id
		if( scanner.tok == FINISHED )
			break;

		if( i == 0 )
			continue;

		if( i<0 || i>=MAX_PROPMOVER )		// 범위체크 
		{
			Error( "too many mover property or invalid id:%d FILE:%s\n", i, lpszFileName );
			return FALSE;
		}

		if( !checker.insert( i ).second )	// id중복체크 
		{
			Error( "Last read:%s, ID:%d is duplicated. 컬럼이 밀려도 이런에러가 난다. FILE:%s\n", szLastName, i, lpszFileName );
			return FALSE;
		}

		MoverProp* pProperty = &m_pPropMover[i];
		pProperty->dwID	= i;
		memset( pProperty->m_szDefineString, 0, sizeof(pProperty->m_szDefineString) );
		_tcscpy( pProperty->m_szDefineString, scanner.m_strDefine );
		
#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		_tcscpy( szLastName, scanner.token );
		_tcscpy( pProperty->szName, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// korean Name 
		TRACE( "PropMover = %d, %s\n", i, scanner.token );
		_tcscpy( szLastName, scanner.token );
		if( m_nLanguage == LANG_KOR ) _tcscpy( pProperty->szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// english Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( pProperty->szName, scanner.token );
		_tcscpy( pProperty->szNameEn, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( pProperty->szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( pProperty->szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( pProperty->szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( pProperty->szName, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407

//#ifndef __DBSERVER
#if !( defined(__DBSERVER) || defined(__VPW) )
//		m_mapMVI.SetAt( pProperty->szName, pProperty->dwID );
//		m_mapMVI.SetAt( pProperty->szNameEn, pProperty->dwID );
		m_mapMVI.insert( map<string, DWORD>::value_type( pProperty->szName, pProperty->dwID ) );
#ifndef __USE_STR_TBL0407
		m_mapMVI.insert( map<string, DWORD>::value_type( pProperty->szNameEn, pProperty->dwID ) );
#endif	// __USE_STR_TBL0407
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


// 비스트에서는 불필요한 부분을 읽지 않게 한다. 따라서 프로퍼티가 바뀌더라도 자주 컴팔할 필요가 없음 
#ifndef __V050515_PROPLASTFULL

		if( pProperty->bIfParts == NULL_ID )
			Message( "LoadPropMover : bIfParts엔 -1을 넣으면 안된다. %s \r\n컬럼이 밀렸거나 워킹폴더를 잘못지정했을수도있음 ", pProperty->szName );

		pProperty->nChaotic				= scanner.GetNumber();
		pProperty->dwDefExtent			= scanner.GetNumber();
		pProperty->dwActionRadius		= scanner.GetNumber();
		pProperty->dwAtkMin				= scanner.GetNumber();
		pProperty->dwAtkMax				= scanner.GetNumber();
		pProperty->dwAtk1				= scanner.GetNumber();
		pProperty->dwAtk2				= scanner.GetNumber();
		pProperty->dwAtk3				= scanner.GetNumber();
		pProperty->dwAtk4				= scanner.GetNumber();
		pProperty->dwVerticalRate		= scanner.GetNumber();
		pProperty->dwDiagonalRate		= scanner.GetNumber();
		pProperty->dwThrustRate			= scanner.GetNumber();

		pProperty->dwChestRate			= scanner.GetNumber();
		pProperty->dwHeadRate			= scanner.GetNumber();
		pProperty->dwArmRate			= scanner.GetNumber();
		pProperty->dwLegRate			= scanner.GetNumber();

		pProperty->dwAttackSpeed		= scanner.GetNumber();
		pProperty->dwReAttackDelay		= scanner.GetNumber();
		pProperty->dwAddHp				= scanner.GetNumber();
#ifdef __X1014_PROP
		pProperty->dwAddMp				= scanner.GetNumber();
#endif
		pProperty->dwNaturalArmor		= scanner.GetNumber();
		pProperty->nAbrasion			= scanner.GetNumber();
		pProperty->nHardness			= scanner.GetNumber();
		pProperty->dwAdjAtkDelay		= scanner.GetNumber();
		
		pProperty->eElementType			= static_cast<SAI79::ePropType>(scanner.GetNumber());	// 정수타입과 enum	타입이 틀려서 타입 변환을 허용함.
		pProperty->wElementAtk			= static_cast<short>(scanner.GetNumber());				// 정수타입과 short	타입이 틀려서 타입 변환을 허용함.

		pProperty->dwHideLevel			= scanner.GetNumber();
		pProperty->fSpeed				= scanner.GetFloat();
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

		pProperty->nExpValue	= scanner.GetExpInteger();

#ifdef __XFXP0521
		pProperty->nFxpValue			= scanner.GetNumber();
#else		
		pProperty->nFxpValue			= 0;
#endif
		pProperty->nBodyState			= scanner.GetNumber();
		pProperty->dwAddAbility			= scanner.GetNumber();
		pProperty->bKillable			= scanner.GetNumber();
		
		pProperty->dwVirtItem[0]		= scanner.GetNumber();
		pProperty->dwVirtItem[1]		= scanner.GetNumber();
		pProperty->dwVirtItem[2]		= scanner.GetNumber();
		pProperty->bVirtType[0]			= scanner.GetNumber();
		pProperty->bVirtType[1]			= scanner.GetNumber();
		pProperty->bVirtType[2]			= scanner.GetNumber();

		pProperty->dwSndAtk1				= scanner.GetNumber();
		pProperty->dwSndAtk2 			= scanner.GetNumber();

		pProperty->dwSndDie1 			= scanner.GetNumber();
		pProperty->dwSndDie2 			= scanner.GetNumber();

		pProperty->dwSndDmg1 			= scanner.GetNumber();
		pProperty->dwSndDmg2 			= scanner.GetNumber();
		pProperty->dwSndDmg3 			= scanner.GetNumber();

		pProperty->dwSndIdle1 			= scanner.GetNumber();
		pProperty->dwSndIdle2 			= scanner.GetNumber();

#ifdef __USE_STR_TBL0407
		scanner.GetToken();
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken(); // 설명 
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // English 
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan 
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		scanner.GetToken();
#endif	// __USE_STR_TBL0407

#else // __V050515_PROPLASTFULL
		if( scanner.GetDefine().IsEmpty() == FALSE )
			scanner.GetLastFull();
		scanner.GetLastFull();
#endif // __V050515_PROPLASTFULL

		

	}
	if( checker.size() > 0 )
		m_nMoverPropSize = *(checker.rbegin()) + 1;
	
	return TRUE;
}

//gmpbigsun( 20101021 ) : propCtrl 읽음			
//#if defined(__WORLDSERVER) || defined(__CLIENT)
BOOL CProject::LoadPropCtrl( LPCTSTR lpszFileName, CFixedArray<CtrlProp>*	apObjProp)
{
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		return FALSE;
	}
	
	CtrlProp ctrlProp;

	char szTemp[ 128 ];

	int i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		ctrlProp.dwID = i;    
		if( i <= 0 )
			Message( "%s 읽다가 ID가 %d이나왔다. 마지막으로 읽은것 %s", lpszFileName, i, szTemp );

#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		TRACE( "PropCtrl = %d, %s\n", i, scanner.token );
		_tcscpy( ctrlProp.szName, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// Name 

		TRACE( "PropCtrl = %d, %s\n", i, scanner.token );

		if( m_nLanguage == LANG_KOR ) _tcscpy( ctrlProp.szName, scanner.token );
		strcpy( szTemp, ctrlProp.szName );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( ctrlProp.szName, scanner.token );
		_tcscpy( ctrlProp.szNameEn, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( ctrlProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( ctrlProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( ctrlProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( ctrlProp.szName, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407
		m_mapCtrl.insert( map<string, DWORD>::value_type( ctrlProp.szName, ctrlProp.dwID ) );
		ctrlProp.dwCtrlKind1 = scanner.GetNumber();
		ctrlProp.dwCtrlKind2 = scanner.GetNumber();
		ctrlProp.dwCtrlKind3 = scanner.GetNumber();
		ctrlProp.dwSfxCtrl = scanner.GetNumber();
		ctrlProp.dwSndDamage = scanner.GetNumber();

#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		_tcscpy( ctrlProp.szCommand, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// korean Name 
		if( m_nLanguage == LANG_KOR ) _tcscpy( ctrlProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// english Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( ctrlProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( ctrlProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( ctrlProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( ctrlProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( ctrlProp.szCommand, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407
			
		apObjProp->SetAtGrow( i, &ctrlProp);
		i = scanner.GetNumber(); 
	}
	apObjProp->Optimize();
	return TRUE;
}
//#endif // __WORLDSERVER

#if defined(__NPP_050308 ) && !defined(__DBSERVER)
BOOL CProject::LoadPropKarma( LPCTSTR lpszFileName )
{
	ZeroMemory( m_aKarmaProp, sizeof( m_aKarmaProp ) );
				
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		return FALSE;
	}
	
	KarmaProp karmaProp;

	char szTemp[ 128 ];

	DWORD dwCount = 0;
	DWORD i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		if( dwCount >= MAX_KARMAPROP )
			Message( "최대읽을수 있는 배열 범위 벗어남(PropKarma) : Max %d", MAX_KARMAPROP );

		karmaProp.nGrade = i;

#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		TRACE( "PropKarma = %d, %s\n", i, scanner.token );
		_tcscpy( karmaProp.szName, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// Name 

		TRACE( "PropKarma = %d, %s\n", i, scanner.token );

		if( m_nLanguage == LANG_KOR ) _tcscpy( karmaProp.szName, scanner.token );
		strcpy( szTemp, karmaProp.szName );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( karmaProp.szName, scanner.token );
		_tcscpy( karmaProp.szNameEn, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( karmaProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( karmaProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( karmaProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( karmaProp.szName, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407

		karmaProp.nKarmaPoint		= scanner.GetNumber();
		karmaProp.dwKarmaGrade      = scanner.GetNumber();
		karmaProp.dwColor			= scanner.GetNumber();
		karmaProp.dwKarmaRecoverPoint = scanner.GetNumber();
		karmaProp.fDiscountRate		= scanner.GetFloat();
		karmaProp.fSellPenaltyRate	= scanner.GetFloat();
		karmaProp.bGuardReaction	= scanner.GetNumber();
		karmaProp.nSubtractExpRate	= scanner.GetNumber();
		karmaProp.nDropGoldPercent	= scanner.GetNumber();
		karmaProp.nDropItem			= scanner.GetNumber();
		karmaProp.nDropPercent		= scanner.GetNumber();
		karmaProp.nKarmaRecoverNum	= scanner.GetNumber();
		karmaProp.dwStatLimitTime	= scanner.GetNumber();
		karmaProp.nStatLimitNum		= scanner.GetNumber();
		karmaProp.nStatLimitRate	= scanner.GetNumber();

#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		TRACE( "szCommand = %s\n", scanner.token );
		_tcscpy( karmaProp.szCommand, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// korean Name 
		if( m_nLanguage == LANG_KOR ) _tcscpy( karmaProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// english Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( karmaProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( karmaProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( karmaProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( karmaProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( karmaProp.szCommand, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407
		i = scanner.GetNumber(); 

		m_aKarmaProp[dwCount++] = karmaProp;
	}
	return TRUE;
}
#endif	// __NPP_050308

/*
BOOL CProject::LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp )
{
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		return FALSE;
	}

	ItemProp itemProp;
	char szTemp[ 128 ];

	int oldi	= 0;
	int i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		itemProp.dwID = i;
		if( i <= 0 )
			Message( "%s 읽다가 ID가 %d이나왔다. 마지막으로 읽은것 %s", lpszFileName, i, szTemp );

#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		TRACE( "PropItem = %d, %s\n", i, scanner.token );
		_tcscpy( itemProp.szName, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// Name 
		TRACE( "PropItem = %d, %s\n", i, scanner.token );
		if( m_nLanguage == LANG_KOR ) _tcscpy( itemProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( itemProp.szName, scanner.token );
		strcpy( szTemp, scanner.token );
		_tcscpy( itemProp.szNameEn, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( itemProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( itemProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( itemProp.szName, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( itemProp.szName, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407

//		m_mapII.SetAt( itemProp.szName, itemProp.dwID );
//		m_mapII.SetAt( itemProp.szNameEn , itemProp.dwID );
		m_mapII.insert( map<string, DWORD>::value_type( itemProp.szName, itemProp.dwID ) );
#ifndef __USE_STR_TBL0407
		m_mapII.insert( map<string, DWORD>::value_type( itemProp.szNameEn, itemProp.dwID ) );
#endif	// __USE_STR_TBL0407

 		itemProp.dwNum	= scanner.GetNumber(  );            
		itemProp.dwPackMax	= scanner.GetNumber(  );      
		itemProp.dwItemKind1	= scanner.GetNumber(  );      
		itemProp.dwItemKind2	= scanner.GetNumber(  );      
		itemProp.dwItemKind3	= scanner.GetNumber(  );      
		itemProp.dwItemJob	= scanner.GetNumber(  );      

// 비스트에서는 불필요한 부분을 읽지 않게 한다. 따라서 프로퍼티가 바뀌더라도 자주 컴팔할 필요가 없음 
#ifndef __V050515_PROPLASTFULL

		itemProp.bPermanence	= scanner.GetNumber(  );      
		itemProp.dwUseable	= scanner.GetNumber(  );      
		itemProp.dwItemSex	= scanner.GetNumber(  );      
		itemProp.dwWeight	= scanner.GetNumber(  );      
		itemProp.dwCost		= scanner.GetNumber(  );      
		itemProp.dwCash		= scanner.GetNumber(  );      
		itemProp.dwEndurance	= scanner.GetNumber(  ); 
		itemProp.dwRepairNumber	= scanner.GetNumber(  );			
		itemProp.bRepairUnable	= scanner.GetNumber(  );      
		itemProp.bJammed	= scanner.GetNumber(  ); 
		itemProp.nAbrasion	= scanner.GetNumber(  );
		itemProp.nHardness	= scanner.GetNumber(  );
		itemProp.dwHanded	= scanner.GetNumber(  );      
//		itemProp.dwHeelH	= scanner.GetNumber(  );      
		itemProp.dwFlag		= scanner.GetNumber(  );      //raiders_060112
		itemProp.dwParts	= scanner.GetNumber(  );      
		itemProp.dwPartsub	= scanner.GetNumber(  );      
		itemProp.bPartsFile		= scanner.GetNumber(  );      
		itemProp.dwExclusive	= scanner.GetNumber(  );      
		itemProp.dwBasePartsIgnore	= scanner.GetNumber(  );      
		itemProp.dwItemLV	= scanner.GetNumber(  );      
		itemProp.dwAddLV	= scanner.GetNumber(  );      
		itemProp.dwItemRare		=  scanner.GetNumber(  );   
		itemProp.dwShopAble = scanner.GetNumber(  );
		itemProp.nLog	= scanner.GetNumber(  );
		itemProp.bCharged	= scanner.GetNumber(  );
#ifdef __DEV
		itemProp.nLog = -1;		// 개발섭에선 이거 무시.
#endif
		itemProp.dwSpeedPenalty		= scanner.GetNumber(  );      
		itemProp.dwLinkKindBullet		= scanner.GetNumber(  );      
		itemProp.dwLinkKind		= scanner.GetNumber(  );      
		itemProp.dwAbilityMin	= scanner.GetNumber(  );      
		itemProp.dwAbilityMax	= scanner.GetNumber(  );      
		itemProp.dwSetItem	= scanner.GetNumber(  );      

		itemProp.eItemType		= static_cast<SAI79::ePropType>(scanner.GetNumber(  ));
		itemProp.wItemEatk		= static_cast<short>(scanner.GetNumber(  ));
		
		itemProp.dwParry		= scanner.GetNumber(  );
		itemProp.dwAtkRating	= scanner.GetNumber(  );
		itemProp.dwblockRating	= scanner.GetNumber(  );
		
		itemProp.nAddSkillMin	= scanner.GetNumber(  );
		itemProp.nAddSkillMax	= scanner.GetNumber(  );
		itemProp.dwAtkStyle		= scanner.GetNumber(  );      
		itemProp.dwAtkType	= scanner.GetNumber(  );      
		itemProp.dwWeaponType	= scanner.GetNumber(  );      

		itemProp.dwItemAtkOrder1	= scanner.GetNumber(  );      	
		itemProp.dwItemAtkOrder2	= scanner.GetNumber(  );      	
		itemProp.dwItemAtkOrder3	= scanner.GetNumber(  );      	
		itemProp.dwItemAtkOrder4	= scanner.GetNumber(  );      	
		itemProp.dwItemMotion1	= scanner.GetNumber(  );      	
		itemProp.dwItemMotion2	= scanner.GetNumber(  );      	
		itemProp.dwItemMotion3	= scanner.GetNumber(  );      	
		itemProp.dwItemMotion4	= scanner.GetNumber(  );      

		itemProp.tmContinuousPain	= scanner.GetNumber(  );      
		itemProp.nShellQuantity	= scanner.GetNumber(  );      
		itemProp.dwRecoil	= scanner.GetNumber(  );      
		itemProp.dwCaliber	= scanner.GetNumber(  );      
		itemProp.dwBarrageRate	= scanner.GetNumber(  );      
		itemProp.dwLoadingTime	= scanner.GetNumber(  );      
		itemProp.nAdjHitRate	= scanner.GetNumber(  );      
		itemProp.nAdjSpellRate	= scanner.GetNumber(  );      
		itemProp.fAttackSpeed	= scanner.GetFloat(  );      
		itemProp.dwDmgShift		= scanner.GetNumber(  );      
		itemProp.dwAttackRange	= scanner.GetNumber(  );      
		itemProp.nProbability	= scanner.GetNumber(  );      
		itemProp.dwDestParam[0]	= scanner.GetNumber(  );      // 50
		itemProp.dwDestParam[1]	= scanner.GetNumber(  );      
		itemProp.nAdjParamVal[0] = scanner.GetNumber(  );      
		itemProp.nAdjParamVal[1] = scanner.GetNumber(  );      
		itemProp.dwChgParamVal[0]	= scanner.GetNumber(  );      
		itemProp.dwChgParamVal[1]	= scanner.GetNumber(  );      
		itemProp.nDestData1[0]		= scanner.GetNumber( );      
		itemProp.nDestData1[1]		= scanner.GetNumber( );   
		itemProp.nDestData1[2]		= scanner.GetNumber( );      
		itemProp.dwActiveSkill		= scanner.GetNumber( );
 #ifdef __X0721_ACTIVESKILL
		itemProp.dwActiveSkillLv	= scanner.GetNumber( );
 #endif
		itemProp.dwActiveSkillRate	= scanner.GetNumber( );      
		itemProp.dwReqJob	= scanner.GetNumber(  );      
		itemProp.dwReqStr	= scanner.GetNumber(  );      
		itemProp.dwReqSta	= scanner.GetNumber(  );      
		itemProp.dwReqDex	= scanner.GetNumber(  );      
		itemProp.dwReqInt	= scanner.GetNumber(  );      
		itemProp.dwReqMp	= scanner.GetNumber(  );      
		itemProp.dwReqFp	= scanner.GetNumber(  );      
        itemProp.dwReqJobLV		= scanner.GetNumber(  );      
        itemProp.dwReqDisLV		= scanner.GetNumber(  );      

		itemProp.dwReSkill1			= scanner.GetNumber(  );
		itemProp.dwReSkillLevel1	= scanner.GetNumber(  );
		itemProp.dwReSkill2			= scanner.GetNumber(  );
		itemProp.dwReSkillLevel2	= scanner.GetNumber(  );
		itemProp.dwBaseSkill		= scanner.GetNumber(  );
		
        itemProp.dwSkillReadyType	= scanner.GetNumber(  );      

		itemProp.dwSkillReady	= scanner.GetNumber();      
		itemProp._dwSkillRange	= scanner.GetNumber();      
		itemProp.dwSfxElemental = scanner.GetNumber();      
		itemProp.dwSfxObj		= scanner.GetNumber();      
		itemProp.dwSfxObj2		= scanner.GetNumber();      
		itemProp.dwSfxObj3		= scanner.GetNumber();      

		itemProp.dwSfxObj4		= scanner.GetNumber();      
		itemProp.dwSfxObj5		= scanner.GetNumber();      
		itemProp.dwUseMotion	= scanner.GetNumber();      
		itemProp.dwCircleTime	= scanner.GetNumber();      
		itemProp.dwSkillTime	= scanner.GetNumber();      
		itemProp.dwExeChance	= scanner.GetNumber();      
		itemProp.dwExeTarget	= scanner.GetNumber();      

		itemProp.dwUseChance	= scanner.GetNumber();      
		itemProp.dwSpellRegion	= scanner.GetNumber();   

		itemProp.dwSpellType	= scanner.GetNumber();

		itemProp.dwReferStat1	= scanner.GetNumber();      
		itemProp.dwReferStat2	= scanner.GetNumber();      
		itemProp.dwReferTarget1	= scanner.GetNumber();      
		itemProp.dwReferTarget2	= scanner.GetNumber();      
		itemProp.dwReferValue1	= scanner.GetNumber();      
		itemProp.dwReferValue2	= scanner.GetNumber();      

		itemProp.dwSkillType	= scanner.GetNumber(  );      
		itemProp.dwItemResistMagic	= scanner.GetNumber(  );      

		
		itemProp.nItemResistElecricity	= (int)(scanner.GetFloat(  ) * 100.0f);
		itemProp.nItemResistFire	= (int)(scanner.GetFloat(  ) * 100.0f);      
		itemProp.nItemResistWind	= (int)(scanner.GetFloat(  ) * 100.0f);      
		itemProp.nItemResistWater	= (int)(scanner.GetFloat(  ) * 100.0f);      
		itemProp.nItemResistEarth	= (int)(scanner.GetFloat(  ) * 100.0f);      
		

		itemProp.bAmplification	= scanner.GetNumber(  );      
		itemProp.bFusion	= scanner.GetNumber(  );      
		itemProp.nEvildoing		= scanner.GetNumber(  );      
		itemProp.bGrow	= scanner.GetNumber(  );      
		itemProp.bCraft		= scanner.GetNumber(  );      
		itemProp.dwExpertLV		= scanner.GetNumber(  );      
		itemProp.dwExpertMax	= scanner.GetNumber(  );   
		
		itemProp.dwSubDefine	= scanner.GetNumber(  );
		itemProp.dwExp	= scanner.GetNumber(  );      
		itemProp.LVRatio	= scanner.GetNumber(  );   
		itemProp.dwComboStyle	= scanner.GetNumber(  );      
		itemProp.dwComboSkillTime = scanner.GetNumber(  );      
		itemProp.dwCardType		= scanner.GetNumber(  );      
		itemProp.fFlightSpeed	= scanner.GetFloat(  );      
		itemProp.fFlightLRAngle	= scanner.GetFloat(  );      
		itemProp.fFlightTBAngle	= scanner.GetFloat(  );      
		itemProp.dwFlightLimit	= scanner.GetNumber(  );      
		itemProp.dwFFuelReMax	= scanner.GetNumber(  );      
		itemProp.dwFFuelSp		= scanner.GetNumber(  );      
		itemProp.dwAFuelReMax	= scanner.GetNumber(  );      
		itemProp.dwFuelRe		= scanner.GetNumber(  );      
		itemProp.dwLimitJob1	= scanner.GetNumber(  );      
		itemProp.dwLimitLevel1	= scanner.GetNumber();
		if( m_nLanguage == LANG_TWN || m_nLanguage == LANG_THA )
		{
			if( itemProp.dwLimitLevel1 > 0 && itemProp.dwLimitLevel1 <= 5 )
				itemProp.dwLimitLevel1	= 1;
			else if( itemProp.dwLimitLevel1 > 5 && itemProp.dwLimitLevel1 <= 15 )
				itemProp.dwLimitLevel1	= 5;
			else if( itemProp.dwLimitLevel1 > 15 && itemProp.dwLimitLevel1 <= 30 )
				itemProp.dwLimitLevel1	= 15;
			else if( itemProp.dwLimitLevel1 > 30 && itemProp.dwLimitLevel1 <= 45 )
				itemProp.dwLimitLevel1	= 30;
			else if( itemProp.dwLimitLevel1 > 45 && itemProp.dwLimitLevel1 < 60 )
				itemProp.dwLimitLevel1	= 45;
		}
		itemProp.dwLimitJob2	= scanner.GetNumber(  );    
		itemProp.dwLimitLevel2	= scanner.GetNumber(  );      
		itemProp.nReflect		= scanner.GetNumber(  );      

		itemProp.dwSndAttack1	= scanner.GetNumber();     
		itemProp.dwSndAttack2	= scanner.GetNumber();     
		itemProp.dwSndAttack3	= scanner.GetNumber();     
		itemProp.dwSndAttack4	= scanner.GetNumber();     
		itemProp.dwSndDefense	= scanner.GetNumber();     
		itemProp.dwSndHit   	= scanner.GetNumber();     
		itemProp.dwSndDamage	= scanner.GetNumber();     
 #ifdef _DEBUG
		if( i == 113)
		{
			int a = 0;
		}
 #endif

		itemProp.dwSndClang	    = scanner.GetNumber();     


		scanner.GetToken();
		scanner.GetToken();	// SndIconName
		_tcscpy( itemProp.szIcon, scanner.token );
		scanner.GetToken();

#ifdef __V050322_CACHEQUEST
		itemProp.dwQuestId	= scanner.GetNumber(  );
		scanner.GetToken();
		scanner.GetToken(); // text filename
		_tcscpy( itemProp.szTextFileName, scanner.token );
		scanner.GetToken();
#endif


#ifdef __USE_STR_TBL0407
		scanner.GetToken();
		_tcscpy( itemProp.szCommand, scanner.token );
#else	// __USE_STR_TBL0407
		scanner.GetToken();
		scanner.GetToken();	// korean Name 
		if( m_nLanguage == LANG_KOR ) _tcscpy( itemProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken();	// english Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( itemProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( itemProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( itemProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( itemProp.szCommand, scanner.token );
		scanner.GetToken();
		scanner.GetToken();
		scanner.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( itemProp.szCommand, scanner.token );
		scanner.GetToken();
#endif	// __USE_STR_TBL0407

#else // __V050515_PROPLASTFULL
		if( scanner.GetDefine().IsEmpty() == FALSE )
			scanner.GetLastFull();
		scanner.GetLastFull();
#endif // __V050515_PROPLASTFULL


		apObjProp->SetAtGrow( i, &itemProp);
		oldi = i;
		i = scanner.GetNumber(); 
	}
	apObjProp->Optimize();
	return TRUE;
}
*/


BOOL CProject::LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		AfxMessageBox( "LoadPropItem" );
		return FALSE;
	}

	ItemProp prop;
	ZeroMemory( &prop, sizeof(prop) );

	int nVer = scanner.GetNumber();	// version
	while( scanner.tok != FINISHED )
	{
		DWORD dwID = scanner.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			Error( "%s ID:%d last_read(ID:%d name:%s)", lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

		prop.dwID = dwID;
		scanner.GetToken();
		_tcscpy( prop.szName, scanner.token );
 		prop.dwNum				= scanner.GetNumber();            
		prop.dwPackMax			= scanner.GetNumber();      
		prop.dwItemKind1		= scanner.GetNumber();      
		prop.dwItemKind2		= scanner.GetNumber();      
		prop.dwItemKind3		= scanner.GetNumber();      

		if( scanner.GetDefine().IsEmpty() == FALSE )
			scanner.GetLastFull();
		scanner.GetLastFull();

//		if( nVer <= __VER )
//		{
//			TRACE( "PropItem = %d, %s\n", prop.dwID, prop.szName );
			m_mapII.insert( map<string, DWORD>::value_type( prop.szName, prop.dwID ) );
			apObjProp->SetAtGrow( prop.dwID, &prop);
//		}

		nVer = scanner.GetNumber();	// version; 
	}
	apObjProp->Optimize();
	return TRUE;
}