#ifndef __SERIAL_NUMBER_H__
#define	__SERIAL_NUMBER_H__


class	CSerialNumberRequest
{
public:

	virtual ~CSerialNumberRequest()	{}

	virtual void	Request()	= 0;
};

class	CSerialNumber
{
public:

	enum
	{
		SN_TYPE_ITEM			= 1,			// 시리얼 넘버 아이템 타입 아이디
	};

	enum
	{
		SN_END_NUMBER			= 0xFFFFFFFE,	// 마지막 시리얼 넘버 값
		SN_IGNORE_SERIAL_NUMBER	= 0xFFFFFFFF,	// 중복 무시 고정 시리얼넘버값(Shop 아이템에 사용)
	};

	static	CSerialNumber*	GetInstance();

// constructions
	virtual ~CSerialNumber();

// operations
	virtual	bool			Init( CSerialNumberRequest * pRequest )			= 0;
	virtual bool			Add( SERIALNUMBER iStart, SERIALNUMBER iEnd )	= 0;
	virtual SERIALNUMBER	Get()		= 0;

	// TODO 시리얼넘버 초기화 완료 확인을 위한 임시함수
	virtual bool			IsInit()	= 0;

	//////////////////////////////////////////////////////////////////////////
	virtual bool			IsManaged()	= 0;
	// 일단 여기에 몰자
	virtual void			InsertSerialNumber( SERIALNUMBER iSerialNumber )	= 0;
	virtual	void			RemoveSerialNumber( SERIALNUMBER iSerialNumber )	= 0;
	//////////////////////////////////////////////////////////////////////////

};

#endif	// __SERIAL_NUMBER_H__