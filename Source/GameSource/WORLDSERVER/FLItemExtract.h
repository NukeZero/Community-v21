
#ifndef __FLITEMEXTRACT_H__
#define __FLITEMEXTRACT_H__


class FLItemExtract
{
	public:
		static	FLItemExtract*	GetInstance();

		FLItemExtract();
		~FLItemExtract();

		bool		LoadScript();

		bool		OnExtractOperByWeapon( FLWSUser* pUser, const PT_PACKET_OPER_EXTRACT ptOperExtract );
		bool		OnExtractOperByArmor( FLWSUser* pUser, const PT_PACKET_OPER_EXTRACT ptOperExtract );
		bool		OnCreateOper( FLWSUser* pUser, const PT_PACKET_OPER_CREATE ptOperCreate );
		bool		OnCreateCid( FLWSUser* pUser, const PT_PACKET_CID_CREATE ptCidCreate );
		bool		OnUpgradeCidPiece( FLWSUser* pUser, const PT_PACKET_CIDPIECE_UPGRADE ptCidPieceUpgrade );
		bool		OnCreateOperCid( FLWSUser* pUser, const PT_PACKET_OPERCID_CREATE ptOperCidCreate );


	private:
		void		Clear();

		bool		IsExtractAble( FLWSUser* pUser, int nExtractType, FLItemElem* pMaterial );
		bool		IsMixAble( FLWSUser* pUser, int nMixType, FLItemElem* pMaterial, FLItemElem* pCombine, FLItemElem* pBaryummeal = NULL );
		bool		GetOperOrOperPiece( FLItemElem* pMaterial, int nMaterialType, int nProb, FLItemElem & itemElem );


		enum EXTRACT_TYPE{ EXTRACT_WEAPON = 0, EXTRACT_ARMOR };
		enum MIX_TYPE{ MIX_CREATE_OPER = 0, MIX_CREATE_CID, MIX_UPGRADE_CIDPIECE, MIX_CREATE_OPERCID };


	private:
		typedef struct _T_OPER_EXTRACT_PROB
		{
			int		nItemMinLevel;
			int		nItemMaxLevel;
			int		nOperExtractProb;
			int		nOperMinNum;
			int		nOperMaxNum;
			int		nOperPieceExtractProb;
			int		nOperPieceMinNum;
			int		nOperPieceMaxNum;

		} T_OPER_EXTRACT_PROB, *PT_OPER_EXTRACT_PROB;
		typedef					std::map< DWORD, std::vector<T_OPER_EXTRACT_PROB> >	MAP_OPER_EXTRACT_PROB;
		MAP_OPER_EXTRACT_PROB	m_mapOperExtractProb;
};

#define		g_xItemExtract		FLItemExtract::GetInstance()

#endif // __FLITEMEXTRACT_H__
