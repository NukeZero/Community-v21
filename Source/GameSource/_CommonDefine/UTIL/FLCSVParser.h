#pragma once
//
//#include <boost/spirit.hpp>
////#include <boost/spirit/include/classic.hpp>
//
//class FLCSVParser
//{
//public:
//	//using namespace boost::spirit;
//	typedef					std::vector< std::string >	VecStr;
//
//private:
//	struct csv_grammar : public boost::spirit::grammar< csv_grammar >
//	{
//	public:
//		csv_grammar( VecStr & vec ) : v( vec ) {}
//
//		template < typename ScannerT >
//		struct definition
//		{
//			//boost::spirit::rule< ScannerT >			line, col, quata_col, simple_col;
//			boost::spirit::rule< ScannerT >			line, col, quata_col, simple_col, newline;
//			boost::spirit::rule< ScannerT > const &	start() const 
//			{ return line; }
//
//
//			//definition(csv_grammar const &self)
//			//{
//			//	quata_col = 
//			//		chlit<>('\"') >>
//			//		(*( strlit<>("\"\"") | anychar_p - chlit<>('\"'))) [push_back_a(self.v)] >>
//			//		chlit<>('\"')
//			//		;
//
//			//	simple_col = (*~chlit<>(','))[push_back_a(self.v)];
//
//			//	col = quata_col
//			//		| simple_col
//			//		;
//
//			//	line = col >> *(',' >> !col);
//			//}
//
//			definition(csv_grammar const &self)
//			{
//				quata_col	= 
//					chlit<>('\"') >>
//					(*( strlit<>("\"\"") | anychar_p - chlit<>('\"'))) [push_back_a(self.v)] >>
//					chlit<>('\"')
//					;
//
//				simple_col	= (*~chlit<>(','))[push_back_a(self.v)];
//
//				newline		= (*( strlit<>("\r\n") ))[push_back_a(self.v)];
//
//				col			= 
//					quata_col
//					| simple_col
//					| newline
//					;
//
//				
//				//col = quata_col
//				//	| simple_col
//				//;
//				//line = col >> *(',' >> !col);
//				line		= col >> *(',' >> !col);// >> eps_p;
//			}
//		};
//
//	public:
//		VecStr &			v;
//	};
//
//public:
//	FLCSVParser( const TCHAR * szFileName );
//	FLCSVParser( const char * pMemory, const size_t nMemorySize );
//	virtual ~FLCSVParser();
//
//	const std::string &		GetString() const;
//	void					NextColumn() const;
//	BOOL					GetLine();
//
//	template < typename _T >
//	const _T				GetNum() const
//	{
//		const _T t			= std::lexical_cast( m_vecData.at( m_nIndex ).c_str() );
//		NextColumn();
//		return t;
//	}
//
//private:
//	csv_grammar				m_g;
//
//	char *					m_pBuff;
//	size_t					m_nBuffSize;
//	size_t					m_nReadBuff;
//
//public:
//	VecStr					m_vecData;
//	mutable int				m_nIndex;
//};
//
//
