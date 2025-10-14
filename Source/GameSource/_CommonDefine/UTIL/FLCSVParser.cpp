#include "StdAfx.h"

#include "FLCSVParser.h"
//
//using namespace boost::spirit;
//
//#include <boost/tokenizer.hpp>
//
//FLCSVParser::FLCSVParser( const TCHAR * szFileName ) : m_g( m_vecData ), m_nIndex( 0 ), m_pBuff( NULL ), m_nBuffSize( 0 ), m_nReadBuff( 0 )
//{
//	CResFile file; 
//	if( !file.Open( szFileName, "rb" ) ) 
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST FILENAME(%s) ]" ), szFileName );
//		return;
//	}
//
//	m_nBuffSize				= file.GetLength();
//	m_pBuff					= new char[ m_nBuffSize ];
//
//	file.Read( m_pBuff, m_nBuffSize );
//
//
////	std::vector< std::string > vec; 
////	using namespace boost;
////	typedef tokenizer< escaped_list_separator< char > > CSVToken;
////	CSVToken tk( m_pBuff, escaped_list_separator< char >( '\\', ',', '\"' ) ); 
//////	for( CSVToken::iterator i( tk.begin() ); i != tk.end(); ++i )  
//////	{    
//////		vec.push_back(*i); 
//////	} 
//
//
//}
//
//FLCSVParser::FLCSVParser( const char * pMemory, const size_t nMemorySize ) : m_g( m_vecData ), m_nIndex( 0 ), m_pBuff( NULL ), m_nBuffSize( 0 ), m_nReadBuff( 0 )
//{
//	//메모리 로드
//	m_pBuff			= new char[ nMemorySize ];
//	::memcpy( m_pBuff, pMemory, nMemorySize );
//}
//
//FLCSVParser::~FLCSVParser()
//{
//	SAFE_DELETE( m_pBuff );
//}
//
//const std::string & FLCSVParser::GetString() const
//{
//	const std::string & str = m_vecData.at( m_nIndex );
//	NextColumn();
//	return str;
//}
//
//void FLCSVParser::NextColumn() const
//{
//	++m_nIndex;
//}
//
//BOOL FLCSVParser::GetLine()
//{
//	using namespace boost::spirit;
//	//template <typename CharT, typename ParserT, typename SkipT>
//	//	parse_info<CharT const*>
//	//	parse(
//	//	CharT const*            str,
//	//	parser<ParserT> const&  p,
//	//	parser<SkipT> const&    skip);
//
//	//parse_info< char > pi = parse( m_pBuff + m_nReadBuff, m_g, space_p );		
//
//	m_vecData.clear();
//	m_nIndex				= 0;
//
//	//parse_info<> pi = parse( m_pBuff + m_nReadBuff, m_g, space_p );		
//	parse_info<> pi			= parse( m_pBuff + m_nReadBuff, m_g/*, end_p*/ );		
//	if( pi.full == true )
//	{
////#undef min
//		m_nReadBuff			= /*std::*/min( ( m_nReadBuff + pi.length ) , m_nBuffSize );
//		m_nIndex			= 0;
//		return TRUE;
//	}
//
//	return FALSE;
//}