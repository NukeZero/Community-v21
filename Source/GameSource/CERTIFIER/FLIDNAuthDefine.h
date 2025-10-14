
#ifndef	__FLIDNAUTHDEFINE_H__
#define __FLIDNAUTHDEFINE_H__


#include <network/FLAr.h>


enum
{
	IDN_AUTH_PACKET_INITIALIZE_REQ			= 10,	// 처음 인증서버 접속 요청
	IDN_AUTH_PACKET_INITIALIZE_ACK			= 10,	// 처음 인증서버 접속 응답

	IDN_AUTH_PACKET_LOGIN_REQ				= 20,	// Account 로그인
	IDN_AUTH_PACKET_LOGIN_ACK				= 20,	// Account 로그인

	IDN_AUTH_PACKET_LOGOUT_NTY				= 30,	// Account 로그아웃

	IDN_AUTH_PACKET_ALIVE_NTY				= 40,	// Alive (1분당 한번)

	IDN_AUTH_PACKET_ACCOUNT_TERMINATE_NTY	= 50,	// Account 접속 종료 요청

};


enum
{
	IDN_AUTH_ERROR_OK						= 0,	// 성공
	IDN_AUTH_ERROR_WRONG_PACKET_SIZE		= 1,	// 패킷 사이즈 오류
	IDN_AUTH_ERROR_DUPLICATED_INITIALIZE	= 2,	// INITIALIZE 패킷(서버 중복 )

	IDN_AUTH_ERROR_DUPLICATED_LOGIN			= 2,	// ILOGIN( Account 중복 )

	IDN_AUTH_ERROR_ID_PASSWORD_WRONG		= 3,	// ID 혹은 Password 오류
};

enum
{
	IDN_GENDER_MALE		= 1,
	IDN_GENDER_FEMALE	= 2,
};


#define	IDN_AUTH_FIXED_MARK	0xBC
#define	IDN_AUTH_GAME_SN	1


#pragma pack( 1 )

struct	T_IDN_AUTH_HEADER
{
	BYTE	byMark;		// 0xBC 고정
	WORD	wLen;		// 패킷 길이(big-endian) header와 데이터 포함

	T_IDN_AUTH_HEADER()
	{
		byMark = IDN_AUTH_FIXED_MARK;
		wLen = 0;
	}

	WORD	GetSize()	const
	{
		return ::ntohs( wLen );
	}

	void	SetSize( WORD wSize )
	{
		wLen = ::htons( wSize );
	}

};

#pragma pack()



class	FLSerializer
{
public:

	virtual bool	Serialize( CAr& ar )	= 0;

};

class	FLDeserializer
{
public:

	virtual bool	Deserialize( CAr& ar )	= 0;

};

//////////////////////////////////////////////////////////////////////////
// 인도네시아 공용 인증 헤더
class	T_IDN_AUTH_MESSAGE_HEADER
{
public:

	BYTE	byType;		// 패킷 타입

	T_IDN_AUTH_MESSAGE_HEADER() : byType( 0 )
	{
	}
	explicit T_IDN_AUTH_MESSAGE_HEADER( BYTE byPacketType )
	{
		byType = byPacketType;
	}

	static	BYTE	GetType( void* pBuff, size_t nSize )
	{
		BYTE byType = 0;
		CAr ar( pBuff, nSize );
		T_IDN_AUTH_MESSAGE_HEADER cHeader;
		if( cHeader.IDNPacketDeserialize( ar ) == true )
		{
			byType = cHeader.byType;
		}

		return byType;
	}

protected:

	bool	IDNPacketSerialize( CAr& ar )
	{
		ar << byType;

		if( BodySerialize( ar ) == false )
		{
			return false;
		}

		return true;
	}

	bool	IDNPacketDeserialize( CAr& ar )
	{
		ar >> byType;

		if( BodyDeserialize( ar ) == false )
		{
			return false;
		}

		return true;
	}

	static bool	ReadString( CAr& ar, char* szString, size_t cchLen )
	{
		BYTE byStrLen = 0;
		ar >> byStrLen;
		if( cchLen > byStrLen )
		{
			ar.Read( szString, sizeof( char ) * byStrLen );
			szString[byStrLen] = '\0';
			return true;
		}
		else
		{
			szString[0] = '\0';
			return false;
		}
	}

	static bool	WriteString( CAr& ar, const char* szString )
	{
		size_t nLen = ::strlen( szString );
		if( nLen <= UCHAR_MAX )
		{
			BYTE byLen = static_cast< BYTE >( nLen );
			ar << byLen;
			ar.Write( szString, sizeof( char ) * byLen );
			return true;
		}

		return false;
	}

private:

	virtual bool	BodySerialize( CAr& /*ar*/ )	{ return true; }
	virtual bool	BodyDeserialize( CAr& /*ar*/ )	{ return true; }

};

class	FLIDNSerializeAdapter : public T_IDN_AUTH_MESSAGE_HEADER, public FLSerializer
{
public:

	explicit FLIDNSerializeAdapter( BYTE byPacketType )
		: T_IDN_AUTH_MESSAGE_HEADER( byPacketType )
	{
	}

	bool	Serialize( CAr& ar )
	{
		return IDNPacketSerialize( ar );
	}
};

class	FLIDNDeserializeAdapter : public T_IDN_AUTH_MESSAGE_HEADER, public FLDeserializer
{
public:

	explicit FLIDNDeserializeAdapter( BYTE byPacketType )
		: T_IDN_AUTH_MESSAGE_HEADER( byPacketType )
	{
	}

	bool	Deserialize( CAr& ar )
	{
		return IDNPacketDeserialize( ar );
	}

};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Initialize 패킷
// Game->Auth
class	T_IDN_AUTH_PACKET_INITIALIZE_REQ : public FLIDNSerializeAdapter
{
public:

	BYTE	byGameSN;		// ???
	BYTE	byWorldSN;		// 월드 인덱스?
	BYTE	byChannelSN;	// ???

	T_IDN_AUTH_PACKET_INITIALIZE_REQ()
		: FLIDNSerializeAdapter( IDN_AUTH_PACKET_INITIALIZE_REQ )
	{
		byGameSN = 0;
		byWorldSN = 0;
		byChannelSN = 0;
	}

private:

	bool	BodySerialize( CAr& ar )
	{
		ar << byGameSN;
		ar << byWorldSN;
		ar << byChannelSN;
		return true;
	}

};

// Auth->Game
class	T_IDN_AUTH_PACKET_INITIALIZE_ACK  : public FLIDNDeserializeAdapter
{
public:

	BYTE	byResult;	// 결과

	T_IDN_AUTH_PACKET_INITIALIZE_ACK()
		: FLIDNDeserializeAdapter( IDN_AUTH_PACKET_INITIALIZE_ACK )
	{
		byResult = 0;
	}

private:

	bool	BodyDeserialize( CAr& ar )
	{
		ar >> byResult;
		return true;
	}

};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Login 패킷
// Game->Auth
class	T_IDN_AUTH_PACKET_LOGIN_REQ : public FLIDNSerializeAdapter
{
public:

	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_ENCRYPT_PWD];
	DWORD	dwIPAddress;				// little-endian

	T_IDN_AUTH_PACKET_LOGIN_REQ()
		: FLIDNSerializeAdapter( IDN_AUTH_PACKET_LOGIN_REQ )
	{
		memset( szAccount, 0, sizeof( szAccount ) );
		memset( szPassword, 0, sizeof( szPassword ) );
		dwIPAddress = 0;
	}

private:

	bool	BodySerialize( CAr& ar )
	{
		if( WriteString( ar, szAccount ) == false )		return false;
		if( WriteString( ar, szPassword ) == false )	return false;
		ar << dwIPAddress;
		return true;
	}
};

// Auth->Game
class	T_IDN_AUTH_PACKET_LOGIN_ACK : public FLIDNDeserializeAdapter
{
public:

	char	szAccount[MAX_ACCOUNT];
	BYTE	byResult;
	BYTE	byGender;

	BYTE	byYear;
	BYTE	byMonth;
	BYTE	byDay;
	BYTE	byHour;
	BYTE	byMinute;
	BYTE	bySecond;

	char	szEmail[UCHAR_MAX];

	T_IDN_AUTH_PACKET_LOGIN_ACK()
		: FLIDNDeserializeAdapter( IDN_AUTH_PACKET_LOGIN_ACK )
	{
		memset( szAccount, 0, sizeof( szAccount ) );
		byResult = 0;
		byGender = 0;
		byYear = 0;
		byMonth = 0;
		byDay = 0;
		byHour = 0;
		byMinute = 0;
		bySecond = 0;
		memset( szEmail, 0, sizeof( szEmail ) );
	}

private:

	bool	BodyDeserialize( CAr& ar )
	{
		if( ReadString( ar, szAccount, _countof( szAccount ) ) == false )	return false;
		ar >> byResult;
		ar >> byGender;
		ar >> byYear;
		ar >> byMonth;
		ar >> byDay;
		ar >> byHour;
		ar >> byMinute;
		ar >> bySecond;
		if( ReadString( ar, szEmail, _countof( szEmail ) ) == false )	return false;
		return true;
	}

};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Logout 패킷
// Game->Auth
class	T_IDN_AUTH_PACKET_LOGOUT_NTY : public FLIDNSerializeAdapter
{
public:

	char	szAccount[MAX_ACCOUNT];

	T_IDN_AUTH_PACKET_LOGOUT_NTY()
		: FLIDNSerializeAdapter( IDN_AUTH_PACKET_LOGOUT_NTY )
	{
		memset( szAccount, 0, sizeof( szAccount ) );
	}

private:

	bool	BodySerialize( CAr& ar )
	{
		if( WriteString( ar, szAccount ) == false )	return false;
		return true;
	}
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Alive 패킷(1분당 한번 보내야 한다.)
// Game->Auth
class	T_IDN_AUTH_PACKET_ALIVE_NTY : public FLIDNSerializeAdapter
{
public:

	enum
	{
		SEND_INTERVAL_SEC	= 60,	// 60초(1분) 간격으로 보낸다.
	};

	T_IDN_AUTH_PACKET_ALIVE_NTY()
		: FLIDNSerializeAdapter( IDN_AUTH_PACKET_ALIVE_NTY )
	{
	}

private:

	bool	BodySerialize( CAr& /*ar*/ )
	{
		return true;
	}
};
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Account 접속 종료 요청
// Auth->Game
class	T_IDN_AUTH_PACKET_TERMINATE_NTY : public FLIDNDeserializeAdapter
{
public:

	char	szAccount[MAX_ACCOUNT];

	T_IDN_AUTH_PACKET_TERMINATE_NTY()
		: FLIDNDeserializeAdapter( IDN_AUTH_PACKET_ACCOUNT_TERMINATE_NTY )
	{
		memset( szAccount, 0, sizeof( szAccount ) );
	}

private:

	bool	BodyDeserialize( CAr& ar )
	{
		if( ReadString( ar, szAccount, _countof( szAccount ) ) == false )	return false;
		return true;
	}
};
//////////////////////////////////////////////////////////////////////////






#endif