
#ifndef	__NET_SERIALNUMBER_REQUEST_H
#define	__NET_SERIALNUMBER_REQUEST_H


#include <serialnumber.h>


class	CNetSerialNumberRequest : public CSerialNumberRequest
{
private:

	enum
	{
		DEFAULT_REQUEST_COUNT	= 3000,
	};

public:

	static CNetSerialNumberRequest *	GetInstance();

public:

	CNetSerialNumberRequest();
	~CNetSerialNumberRequest();

	void	SetRequestCount( int nCount );
	void	Request();

private:

	int		m_nRequestCount;

};



#endif