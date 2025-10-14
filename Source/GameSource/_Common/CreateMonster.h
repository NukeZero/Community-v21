#ifndef __CREATEMONSTER_H__
#define	__CREATEMONSTER_H__


struct CREATE_MONSTER_PROP
{
	DWORD dwKeepTime;
	std::map<DWORD, int> mapMonster;
	
	CREATE_MONSTER_PROP() :	dwKeepTime(0) {}
		
	DWORD GetRandomMonsterId()
	{
		int nRandom = xRandom( 100 );
		std::map<DWORD, int>::iterator it=mapMonster.begin();
		for( ; it!=mapMonster.end(); it++ )
		{
			if( nRandom < it->second )
				break;
			nRandom -= it->second;
		}

		return it!=mapMonster.end() ? it->first : NULL_ID;
	}
};

struct CREATE_MONSTER_INFO
{
	DWORD dwOwnerId;
	DWORD dwEndTick;
	char  chState;
	
	CREATE_MONSTER_INFO(): dwOwnerId(NULL_ID), dwEndTick(0), chState( 'N' ) {}
};

typedef std::map<DWORD, CREATE_MONSTER_PROP*> MAPPROP;
typedef std::map<DWORD, CREATE_MONSTER_INFO> MAPINFO;

class CCreateMonster
{
public:
	//	Constructions
	CCreateMonster();
	~CCreateMonster();

	static CCreateMonster* GetInstance( void );
	void LoadScript();
	void SetState( OBJID objId, const char chState );
	void ProcessRemoveMonster();
	BOOL IsAttackAble( FLWSUser* pUser, CMover* pTarget, BOOL bTextOut = FALSE );
	void CreateMonster( FLWSUser* pUser, DWORD dwObjId, D3DXVECTOR3 vPos );
	void RemoveInfo( CMover* pMover );
	

private:
	CREATE_MONSTER_PROP* GetCreateMonsterProp( FLItemElem* pItemElem );

	int m_nMaxCreateNum;
	MAPPROP m_mapCreateMonsterProp;
	MAPINFO m_mapCreateMonsterInfo;

};



#endif	//	__CREATEMONSTER_H__
