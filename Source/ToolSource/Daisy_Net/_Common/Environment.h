#ifndef __ENVIRONMENT_H__ 
#define	__ENVIRONMENT_H__

#ifdef __CORESERVER
#include "Ar.h"
#endif // __cORESERVER

class CEnvironment
{
public:
	BOOL	m_bRain;
	BOOL	m_bSnow;
	CTimer  m_tRainTime;
	CTimer  m_tSnowTime;
public:
//	Constructions
	CEnvironment();
	~CEnvironment();
	void Serialize( CAr & ar );

#ifdef __CORESERVER
public:
	BOOL	m_Authority;
public:
	HANDLE	m_hWorker;
	HANDLE	m_hCloseWorker;
	
public:
	BOOL	CreateWorkers( void );
	void	CloseWorkers( void );
	static	UINT	_Worker( LPVOID pParam );
	void	Worker( void );
#endif // __CORESERVER
};

#endif	//	__ENVIRONMENT_H__