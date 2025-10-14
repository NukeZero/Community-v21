#ifndef __FLBARUNAUPGRADENPC_H__
#define __FLBARUNAUPGRADENPC_H__


class FLBarunaUpgradeNpc
{
public:
	static FLBarunaUpgradeNpc*	GetInstance();

	FLBarunaUpgradeNpc();
	~FLBarunaUpgradeNpc();

	bool			LoadScript();

	DWORD			GetExp( FLWSUser* pUser );
	DWORD			ExpToLevel( const DWORD dwExp );
	DWORD			GetEachExp( const DWORD dwLevel ) const;
	DWORD			GetExpRequired(  const DWORD dwLevel  ) const;
	DWORD			GetAdditionalSuccessProb( FLWSUser* pUser );
	DWORD			GetLimitExp()	{	return m_dwExpLimit;	}

	void			AddExpToBarunaNPC( FLWSUser* pUser, DWORD dwUpgradeType, DWORD dwUpgradeLevel, DWORD dwUpgradeResult );

	bool			IsValidStateUpgradeNPC( FLWSUser* pUser, const DWORD dwUpgradeNpcObjID );

private:
	void			Clear();

	DWORD			GetAcquisitionExpEnchantGeneral( DWORD dwUpgradeLevel, DWORD dwUpgradeResult );
	DWORD			GetAcquisitionExpEnchantAttribute( DWORD dwUpgradeLevel, DWORD dwUpgradeResult );
	DWORD			GetAcquisitionExpRandomOption( DWORD dwUpgradeLevel, DWORD dwUpgradeResult );

private:
	typedef struct _T_BARUNA_NPC_LEVEL
	{
		DWORD				dwExpRequired;
		DWORD				dwEachExp;
		DWORD				dwAdditionalSuccessProp;

	} T_BARUNA_NPC_LEVEL, *PT_BARUNA_NPC_LEVEL;
	typedef					std::map< DWORD, T_BARUNA_NPC_LEVEL >			MapBarunaNPCLevelTbl;
	MapBarunaNPCLevelTbl	m_tblBarunaNPCLevel;


	typedef struct _T_BARUNA_NPC_EXP
	{
		DWORD		dwSuccessExp;
		DWORD		dwFailExp;
		DWORD		dwMinusExp;

	} T_BARUNA_NPC_EXP, *PT_BARUNA_NPC_EXP;

	typedef		std::map<DWORD /*UpgradeLevel*/, T_BARUNA_NPC_EXP>		MapBarunaNPCExp;
	typedef		MapBarunaNPCExp::iterator							BarunaNPCExpItr;
	MapBarunaNPCExp			m_mapBarunaNPCExpEnchantGeneral;
	MapBarunaNPCExp			m_mapBarunaNPCExpEnchantAttribute;
	MapBarunaNPCExp			m_mapBarunaNPCExpRandomOption;


	DWORD			m_dwExpLimit;

};

#define		g_xBarunaUpgradeNpc		FLBarunaUpgradeNpc::GetInstance()

#endif // __FLBARUNAUPGRADENPC_H__

