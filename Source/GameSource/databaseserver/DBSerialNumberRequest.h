
#ifndef	__DB_SERIALNUMBER_REQUEST_H
#define __DB_SERIALNUMBER_REQUEST_H


#include <serialnumber.h>


class	CDBSerialNumberRequest : public CSerialNumberRequest
{
private:

	enum
	{
		DEFAULT_REQUEST_COUNT	= 1000,
	};

public:

	static CDBSerialNumberRequest *	GetInstance();

public:

	CDBSerialNumberRequest();
	~CDBSerialNumberRequest();

	void	SetRequestCount( int nCount );
	LPDB_OVERLAPPED_PLUS	MakeRequest();
	void	Request();

private:

	int		m_nRequestCount;

};








#endif