
#ifndef	__FLENUMMODULE_H__
#define	__FLENUMMODULE_H__


#include "FLCommon.h"

#include <TlHelp32.h>


class	FLEnumModule
{
public:

	FLEnumModule();
	~FLEnumModule();

	bool	Open( DWORD dwProcessID );
	void	Close();

	const TCHAR*	GetFirst();
	const TCHAR*	GetNext();

private:

	HANDLE			m_hSnap;
	MODULEENTRY32	m_tEntry;

};





#endif