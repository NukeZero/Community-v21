
#pragma once

class FLFlyffConfig
{
public:
	FLFlyffConfig();
	~FLFlyffConfig();

	static	FLFlyffConfig*		GetInstance();

	bool LoadFlyffConfig( const TCHAR* pFileName );
	
	int GetMainLanguage() const {	return m_language;	}
	WORD GetCodePage() const {	return m_code_page;	}
	bool Is2ndPWDMode() const {	return m_use_bank_password_at_login;	}
	u_short GetCachePort() const {	return m_port_cache_server;	}
	
	// FWC 서버인가.. FWC 서버는 비밀번호 입력 오류에 대한 페널티가 없다.
	bool			IsFWCCertifierIP() const;

#ifdef __SERVER
public:
	bool			IsConnectAccountDBMode() const						{	return m_connect_account_db;	}
	bool			IsConnectLogDBMode() const							{	return m_connect_log_db;	}
#endif // __SERVER

#ifdef __CLIENT
public:
	bool			IsEncryptPWDMode() const							{	return m_encrypt_password;	}
	DWORD			GetShoutLimitSecond() const							{	return m_limit_shout_tick;	}
	int				GetShoutLimitCount() const							{	return m_limit_shout_count;	}
	bool			IsAbleMultiExecution() const						{	return m_allow_multi_execution;	}
 	const TCHAR*	GetCertifierAddr() const							{	return m_certifier_ip.c_str();	}
	const TCHAR*	GetNProtectConfigFileName() const					{	return m_nprotect_config_file_name.c_str();	}
	bool			IsConnectedMainServer() const						{	return m_main_server_certifier_ip == m_certifier_ip;	}
	
	// 특정 국가 웹싸이트 로그인 시 패스워드 암호화 안할수 도 있음.
	void			DeactivatePasswordEncryption( const TCHAR* argument );
	// neuz.ini 에 ip 정보가 있으면 변경됨.
	bool			ModifyCertifierIP( const std::string & certifier_ip );

private:
	bool			IsTestServerCertifierIP( const std::string & certifier_ip ) const;
#endif // __CLIENT


private:
	/************************************************************************/
	/* common */
	int				m_language;
	WORD			m_code_page;
	bool			m_use_bank_password_at_login;
	u_short			m_port_cache_server;
	/************************************************************************/

	/************************************************************************/
	/* server */
	bool			m_connect_account_db;
	bool			m_connect_log_db;
	/************************************************************************/

	/************************************************************************/
	/* client */
	bool			m_encrypt_password;
	DWORD			m_limit_shout_tick;
	int				m_limit_shout_count;
	bool			m_allow_multi_execution;
	std::string		m_main_server_certifier_ip;
	typedef std::vector< std::string >	CertifierIP;
	CertifierIP		m_test_server_certifier_ip;
	std::string		m_main_server_nprotect_config_file_name;
	std::string		m_test_server_nprotect_config_file_name;
	std::string		m_certifier_ip;
	std::string		m_nprotect_config_file_name;
	/************************************************************************/
};

#define		g_xFlyffConfig		FLFlyffConfig::GetInstance()	
