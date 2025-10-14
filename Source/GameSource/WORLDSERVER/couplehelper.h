#ifndef __COUPLEHELPER_H
#define	__COUPLEHELPER_H

#include <network/FLAr.h>
#include <couple.h>
#include <playerdata.h>

class CCoupleMgr;
class CDPDatabaseClient;
class CDPSrvr;
class CCoupleHelper
{
private:
	CCoupleHelper();
public:
	virtual	~CCoupleHelper();
	static	CCoupleHelper*	Instance();
	BOOL	Initialize( CDPDatabaseClient* pdpClient, CDPSrvr* pdpServer );
	void	Clear();
	void	OnPropose( FLWSUser* pUser, const char* szPlayer );
	void	OnProposeResult( CAr & ar );
	void	OnRefuse( FLWSUser* pUser );
	void	OnCouple( FLWSUser* pUser );
	void	OnCoupleResult( CAr & ar );
	void	OnDecouple( FLWSUser* pUser );
	void	OnDecoupleResult( CAr & ar );
	void	Serialize( CAr & ar )	{	FLASSERT( m_pMgr );	m_pMgr->Serialize( ar );	}
	CCouple* GetCouple( u_long idPlayer )	{	FLASSERT( m_pMgr );	return m_pMgr->GetCouple( idPlayer );	}
	void	OnAddCoupleExperience( CAr & ar );
	void	OnUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData );
private:
	void	PlayProposeAnimation( FLWSUser* pProposer, FLWSUser* pTarget );
private:
	CCoupleMgr*	m_pMgr;
	CDPDatabaseClient*	m_pdpClient;
	CDPSrvr*	m_pdpServer;
};

#endif	// __COUPLEHELPER_H
