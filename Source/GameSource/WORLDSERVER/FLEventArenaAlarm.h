
#ifndef	__FLEVENTARENAALARM_H__
#define	__FLEVENTARENAALARM_H__


struct	T_ALARM_DATA
{
	DWORD	dwInterval;
	int		nUserTag;

	T_ALARM_DATA()
	{
		dwInterval = 0;
		nUserTag = 0;
	};

	T_ALARM_DATA( DWORD dwIntervalParam, int nUserTagParam )
	{
		dwInterval = dwIntervalParam;
		nUserTag = nUserTagParam;
	};
};

class	FLEventArenaAlarm
{
public:

	FLEventArenaAlarm();
	~FLEventArenaAlarm();

	void	Start( DWORD dwInterval, int nUserTag = 0 );
	void	Start( const std::vector< T_ALARM_DATA >& vecIntervals );
	void	Stop();

	int		TryPop();

	DWORD	GetReamin()	const;
	bool	IsOver()	const;

private:

	void	Start();

private:

	std::list< T_ALARM_DATA >	m_listAlarms;
	mutable CTimer				m_kTimer;

};









#endif