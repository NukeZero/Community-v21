
#ifndef	__FLENUMPROCESS_H__
#define	__FLENUMPROCESS_H__


#include "FLCommon.h"

#include <TlHelp32.h>


class	FLEnumProcess
{
public:

	FLEnumProcess();
	~FLEnumProcess();

	bool	Open();
	void	Close();

	const TCHAR*	GetFirst();
	const TCHAR*	GetNext();

	DWORD	GetProcessID()	const;

private:

	HANDLE			m_hSnap;
	PROCESSENTRY32	m_tEntry;

};





#endif