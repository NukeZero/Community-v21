#pragma once

class FLShutdownRule_Mng
{
	//friend TextCmd_DisplayShutdownList;
public:
	static FLShutdownRule_Mng &	GetInstance();

	void					AddUser( const u_long uPlayerID, const CTime & tmBirthDate );
	void					RemoveUser(  const u_long uPlayerID );

	void					TryShutdown();
	
public:
	std::map< u_long /*playerID*/, CTime >		m_mapDoubtUser;
};