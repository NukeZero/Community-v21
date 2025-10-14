#include "stdafx.h"

#include "FLCStyleParser.h"

#include <io.h>
#include <ctype.h>


FLCStyleReader::FLCStyleReader( const char * pBuffer, const size_t uBufferSize ) 
: m_pBuffer( NULL ), m_uBufferSize( 0 ), m_uCurser( 0 ), m_uLine( 1 )
{
	m_szPath[ 0 ]		= '\0';

	if( pBuffer == NULL || uBufferSize <= 0 )
		return;

	m_pBuffer			= new char[ uBufferSize ];
	::memcpy( m_pBuffer, pBuffer, uBufferSize );
	m_uBufferSize		= uBufferSize;
}

FLCStyleReader::FLCStyleReader( const char * szFileName ) 
: m_pBuffer( NULL ), m_uBufferSize( 0 ), m_uCurser( 0 ), m_uLine( 1 )
{
	m_szPath[ 0 ]		= '\0';

	if( szFileName == NULL )
		return;

	strcpy_s( m_szPath, _countof( m_szPath ), szFileName );

	FILE * fp			= NULL;
	errno_t err			= ::fopen_s( &fp, szFileName, "rb" );
	if( err != 0 )
		return;

	const long r		= ::_filelength( ::_fileno( fp ) );
	if( r == -1 )
		return; 

	const size_t uBufferSize = static_cast< size_t >( r );
	m_pBuffer			= new char[ uBufferSize ];
	const size_t ret		= ::fread( m_pBuffer, 1, uBufferSize, fp );
	if( ret != uBufferSize )
	{
		delete [] m_pBuffer;
		return;
	}

	m_uBufferSize		= uBufferSize;
}

FLCStyleReader::~FLCStyleReader()
{
	if( m_pBuffer != NULL )
		delete [] m_pBuffer;
}
bool			FLCStyleReader::GetToken( FLToken & out )
{
	if( m_pBuffer == NULL )
	{
		out			= FLToken();
		FLERROR_LOG( PROGRAM_NAME, "[ NULL BUFFER ]" );	
		DEBUG_BREAK;
		return false;
	}

	size_t uStart		= m_uCurser;
	//size_t count = 0;
	while( m_uCurser < m_uBufferSize )
	{
		const char c			= m_pBuffer[ m_uCurser ];

		// skip : newline
		if( c == '\n' )
		{
			++m_uCurser;
			++m_uLine;
		//	count = 0;

			uStart		= m_uCurser;
			continue;
		}

		// skip : space
		if( ::isspace( c ) != 0 )
		{
			++m_uCurser;
			uStart		= m_uCurser;
			continue;
		}

		// skip : control
		if( ::iscntrl( m_pBuffer[ m_uCurser ] ) != 0 )
		{
			++m_uCurser;
			uStart		= m_uCurser;
			continue;
		}

		// skip : comment
		if( m_uCurser + 1 < m_uBufferSize )
		{
			// "//"
			if( m_pBuffer[ m_uCurser ] == '/' && m_pBuffer[ m_uCurser + 1 ] == '/' )
			{
				m_uCurser		+= 2;

				while( m_uCurser < m_uBufferSize )
				{
					if( m_pBuffer[ m_uCurser ] == '\n' )
						break;

					++m_uCurser;
				}

				continue;
			}

			// "/* */"
			else if(  m_pBuffer[ m_uCurser ] == '/' && m_pBuffer[ m_uCurser + 1 ] == '*' )
			{
				m_uCurser		+= 2;
				bool bFindEnd	= FALSE;
				const size_t uBeginLine = m_uLine;
				while( m_uCurser < m_uBufferSize )
				{
					if( m_pBuffer[ m_uCurser ] == '\n' )
					{
						++m_uCurser;
						++m_uLine;
						//count = 0;
						continue;
					}

					if( m_uCurser + 1 < m_uBufferSize )
					{
						if( m_pBuffer[ m_uCurser ] == '*' && m_pBuffer[ m_uCurser + 1 ] == '/' )
						{
							m_uCurser	+= 2;
							uStart		= m_uCurser;
							bFindEnd		= TRUE;
							break;
						}
					}

					++m_uCurser;
				}

				if( bFindEnd == FALSE )
				{
					FLERROR_LOG( PROGRAM_NAME, "[ file(%s) line(%u) : /* 주석의 끝이 없습니다 ]", m_szPath, uBeginLine );	
					DEBUG_BREAK;
					return false;
				}

				continue;
			}
		}

		//string
		if( c == '"' )
		{
			++m_uCurser;

			while( m_uCurser < m_uBufferSize )
			{
				if( m_pBuffer[ m_uCurser ] == '\n ')
				{
					FLERROR_LOG( PROGRAM_NAME, "[ file(%s) line(%u) : 문자열 사이에는 개행 문자가 올 수 없습니다. ]", m_szPath, m_uLine );	
					DEBUG_BREAK;
					return false;
				}


				if( m_pBuffer[ m_uCurser ] == '"' )
				{
					++m_uCurser;
					( m_uCurser <= ( uStart + 1 ) ) ? out.s.clear() : out.s.assign( &m_pBuffer[ uStart + 1 ], ( m_uCurser - uStart - 2 ) );
					out.l		= m_uLine;
					out.e		= STRING_VALUE;
					return true;
				}

				++m_uCurser;
			}

			FLERROR_LOG( PROGRAM_NAME, "[ file(%s) line(%u) : 문자열 종료 마크가 없습니다. ]", m_szPath, m_uLine );	
			DEBUG_BREAK;
			return false;
		}

		// fun
		if( __iscsymf( m_pBuffer[ m_uCurser ] ) != 0 )
		{
			while( m_uCurser < m_uBufferSize )
			{
				if( __iscsym( m_pBuffer[ m_uCurser ] ) != 0 )
				{
					++m_uCurser;
					continue;
				}

				break;
			}

			if( m_uCurser == uStart )
				break;

			out.s.assign( &m_pBuffer[ uStart ], ( m_uCurser - uStart ) );
			out.l		= m_uLine;
			out.e		= NAME_VALUE;
			return true;
		}


		// num
		if( ::isdigit( m_pBuffer[ m_uCurser ] ) != 0 )
		{
			bool bHex			= false;

			if( m_uCurser + 1 < m_uBufferSize )
			{
				if( m_pBuffer[ m_uCurser ] == '0' && m_pBuffer[ m_uCurser + 1 ] == 'x' ) 
				{
					m_uCurser += 2;
					bHex		= true;
				}
			}

			if( ::isdigit( m_pBuffer[ m_uCurser ] ) == 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, "[ file(%s) line(%u) : HEX가 아닙니다]", m_szPath, m_uLine );	
				DEBUG_BREAK;
				return false;
			}

			while( m_uCurser < m_uBufferSize )
			{
				if( ::isdigit( m_pBuffer[ m_uCurser ] ) != 0 )
				{
					++m_uCurser;
					continue;
				}
	
				if( m_uCurser == uStart )
					break;

				if( bHex == false )
				{
					out.s.assign( &m_pBuffer[ uStart ], ( m_uCurser - uStart ) );
					out.l		= m_uLine;
					out.e		= NUMBERIC_VALUE;
					return true;
				}

				const size_t HEX_TO_NUMBER_STRING_BUFFER_MAX = 21;
				char szHex[ HEX_TO_NUMBER_STRING_BUFFER_MAX ];
				szHex[ 0 ]		= '\0';
				UINT64 n = 0;
				::strncpy_s( szHex, _countof( szHex ), &m_pBuffer[ uStart ], ( m_uCurser - uStart ) );
				::sscanf_s( szHex, "%x", &n );
				out.s.assign( szHex );
				return true;
			}
		}
		//while( m_uCurser < m_uBufferSize )
		//{
		//	if( ::isdigit( m_pBuffer[ m_uCurser ] ) != 0 )
		//	{
		//		++m_uCurser;
		//		continue;
		//	}

		//	if( m_uCurser == uStart )
		//		break;

		//	out.s.assign( &m_pBuffer[ uStart ], ( m_uCurser - uStart ) );
		//	out.l		= m_uLine;
		//	out.e		= NUMBERIC_VALUE;
		//	return true;
		//}

		//special
		if( ::ispunct( m_pBuffer[ m_uCurser ] ) != 0 )
		{
			if( m_uCurser + 1 < m_uBufferSize )
			{
				if( ( m_pBuffer[ m_uCurser ] == '|' && m_pBuffer[ m_uCurser + 1 ] == '|' ) 
					|| ( m_pBuffer[ m_uCurser ] == '&' && m_pBuffer[ m_uCurser + 1 ] == '&' )
					|| ( m_pBuffer[ m_uCurser ] == '=' && m_pBuffer[ m_uCurser + 1 ] == '=' )
					|| ( m_pBuffer[ m_uCurser ] == '<' && m_pBuffer[ m_uCurser + 1 ] == '=' )
					|| ( m_pBuffer[ m_uCurser ] == '>' && m_pBuffer[ m_uCurser + 1 ] == '=' ) )
				{
					++m_uCurser;
				}
			}

			++m_uCurser;

			out.s.assign( &m_pBuffer[ uStart ], ( m_uCurser - uStart ) );
			out.l		= m_uLine;
			out.e		= SPECIAL_VALUE;
			return true;
		}

		FLERROR_LOG( PROGRAM_NAME, "[ file(%s) line(%u) : 쓰일수 없는 문자 입니다(%c). ]", m_szPath, m_uLine, c );	
		DEBUG_BREAK;
		return false;
		//++m_uCurser;
	}

	out.l		= m_uLine;
	out.e		= EOF_VALUE;
	return true;
}

bool FLCStyleReader::Load( vector< FLToken > & out )
{
	// read
	for( ;; )
	{
		FLCStyleReader::FLToken r;
		const bool ret 		= GetToken( r );
		if( ret == false )
			return false;

		if( r.e == FLCStyleReader::EOF_VALUE )
		{
			out.push_back( r );
			break;
		}

		out.push_back( r );
	}

	return true;
}

FLCStyleReader::FLToken 
FLCStyleReader::PopToken( vector< FLToken >::const_iterator & it, vector< FLToken >::const_iterator & it_end )
{
	return ( it != it_end ) ? *it++ : FLToken();
}

FLCStyleReader::FLToken 
FLCStyleReader::GetToken( vector< FLToken >::const_iterator & it, vector< FLToken >::const_iterator & it_end )
{
	return ( it != it_end ) ? *it : FLToken();
}

const char * FLCStyleReader::GetPath() const
{
	return m_szPath;
}

