#pragma once





// 지원안함 : 0x, 문자열

class FLCStyleReader
{
public:
	enum E_VALUE
	{
		EOF_VALUE			= 0,
		NAME_VALUE,						//Sum12			__iscsymf// -> __iscsym
		NUMBERIC_VALUE,					//123			isdigit
		SPECIAL_VALUE,					// = + /		ispunct
		STRING_VALUE,					//""			"""" """"
	};

public:
	struct FLToken
	{
		FLToken()
			: e( EOF_VALUE ), l( 0 ), c( 0 ) {}
		FLToken( const E_VALUE ee, const std::string & str, const size_t line, const size_t count ) 
			: e( ee ), s( str ), l( line ), c( count ) {}

		FLCStyleReader::E_VALUE			e;
		std::string						s;
		size_t							l;
		size_t							c;
	};

public:
	FLCStyleReader( const char * pBuffer, const size_t uBufferSize );
	FLCStyleReader( const char * szFileName );
	virtual ~FLCStyleReader();

	bool					Load( vector< FLToken > & out );
	bool					GetToken( FLToken & out );

	static FLToken			PopToken( vector< FLToken >::const_iterator & it, vector< FLToken >::const_iterator & it_end );
	static FLToken			GetToken( vector< FLToken >::const_iterator & it, vector< FLToken >::const_iterator & it_end );

	const char *			GetPath() const;


private:
	char					m_szPath[ MAX_PATH ];

	char *					m_pBuffer;
	size_t					m_uBufferSize;

	size_t					m_uCurser;
	size_t					m_uLine;
	//size_t					m_uUnitCount;			//한줄당
};


