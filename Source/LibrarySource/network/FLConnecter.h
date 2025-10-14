
#ifndef	__FLCONNECTER_H__
#define	__FLCONNECTER_H__


#include "FLNetworkCommon.h"
#include "FLSock.h"


class	FLConnecter
{
public:

	FLConnecter();
	~FLConnecter();

	bool	Connect( CSock* pSock, const char* lpAddr, WORD wPort );

};





#endif