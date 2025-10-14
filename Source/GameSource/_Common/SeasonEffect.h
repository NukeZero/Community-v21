
#ifndef __SEASONEFFECT_H__
#define __SEASONEFFECT_H__

#ifndef __CORESERVER
namespace nsSeasonFile
{
	typedef	std::map< DWORD, std::string >	SeasonFileTable;

	static SeasonFileTable variableSeasonFileTable[SEASON_MAX];

	void	setVariableSeasonFile();

	bool	isVariableSeasonFile( const SeasonFileTable::value_type::first_type key );

	bool	changeVariableSeasonFile( const int season, const SeasonFileTable::value_type::first_type key, std::string & fileName );
}
#endif // __CORESERVER


class CSeasonEffect
{
public:
	CSeasonEffect();
	~CSeasonEffect();

	static CSeasonEffect*	GetInstance();

	bool	load();

	int		getSeason() const;

	bool	getEffect() const;

	void	serializeSeason( CAr & ar ) const;
	void	deserializeSeason( CAr & ar );

	void	serializeEffect( CAr & ar ) const;
	void	deserializeEffect( CAr & ar );


private:
	int		m_season;
	bool	m_effect;

#ifdef __CORESERVER
public:
	void	setEffectByGM( const bool effect );
	bool	CreateWorkers( void );

private:
	void	CloseWorkers( void );
	static	UINT	_Worker( LPVOID pParam );
	void	Worker( void );

private:
	bool	m_gmEffect;
	HANDLE	m_hWorker;
	HANDLE	m_hCloseWorker;

	CTimer	m_effectKeepTime;
#endif // __CORESERVER
};

#define		g_pSeasonEffect		CSeasonEffect::GetInstance()

#endif // __SEASONEFFECT_H__
