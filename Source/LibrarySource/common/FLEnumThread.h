
#ifndef	__FLENUMTHREAD_H__
#define	__FLENUMTHREAD_H__


#include "FLCommon.h"

#include <TlHelp32.h>


class	FLEnumThread
{
public:

	FLEnumThread();
	~FLEnumThread();

	bool	Open();
	void	Close();

	DWORD	GetFirst( DWORD dwProcessID );
	DWORD	GetNext( DWORD dwProcessID );

private:

	HANDLE			m_hSnap;
	THREADENTRY32	m_tEntry;

};








#endif