#include "stdafx.h"

#include "FLCStyleScript.h"

//#include "FLParser.h"

#include "FLDefinesToString.h"

#include "NpcScript.h"





VOID DebugOutput( TCHAR* strMsg, ... )
{
#if defined(DEBUG) | defined(_DEBUG)
	TCHAR strBuffer[2048];

	va_list args;
	va_start(args, strMsg);
	vsprintf_s( strBuffer, _countof( strBuffer ), strMsg, args );
	va_end(args);

	::strcat_s( strBuffer, _countof( strBuffer ), "\n" );

#else
	UNREFERENCED_PARAMETER(strMsg);
#endif
}


bool FLCStyleScript::CompileScript( FLCStyleReader & reader, map< string, FLExecuter_Sequence * > & o_map )
{
	vector< FLCStyleReader::FLToken > v;

	// read
	if( reader.Load( v ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[ ComplileScript Error : (%s) ]"), reader.GetPath() );
		RELEASE_BREAK;
		return false;
	}

	// complie
	vector< FLCStyleReader::FLToken >::const_iterator it = v.begin();
	vector< FLCStyleReader::FLToken >::const_iterator it_end = v.end();
	while( it != it_end )
	{
		// void 함수 시작	 
		const FLCStyleReader::FLToken ret = FLCStyleReader::PopToken( it, it_end );
		if( ret.e == FLCStyleReader::EOF_VALUE )
			break;

		RELEASE_ASSERT( ret.e == FLCStyleReader::NAME_VALUE && ::strcmp( ret.s.c_str(), "void" ) == 0 );

		// namespace
		const FLCStyleReader::FLToken nameSpace = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( nameSpace.e == FLCStyleReader::NAME_VALUE && ::strcmp( nameSpace.s.c_str(), "CNpcScript" ) == 0 );

		// ::
		const FLCStyleReader::FLToken n1 = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( n1.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( n1.s.c_str(), ":" ) == 0 );
		const FLCStyleReader::FLToken n2 = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( n2.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( n2.s.c_str(), ":" ) == 0 );


		// 함수명
		const FLCStyleReader::FLToken func = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( func.e == FLCStyleReader::NAME_VALUE );

		// (
		const FLCStyleReader::FLToken o = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( o.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( o.s.c_str(), "(" ) == 0  );
		//	)
		const FLCStyleReader::FLToken c = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( c.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( c.s.c_str(), ")" ) == 0 );


		// {
		const FLCStyleReader::FLToken open = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( open.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( open.s.c_str(), "{" ) == 0 );

		// 함수 내용
		FLExecuter_Sequence * p = compileProcess( it, it_end );
		RELEASE_ASSERT( p != NULL );

		// 함수 끝		// }
		const FLCStyleReader::FLToken close = FLCStyleReader::PopToken( it, it_end );
		RELEASE_ASSERT( close.e == FLCStyleReader::SPECIAL_VALUE && ::strcmp( close.s.c_str(), "}" ) == 0 );

		// insert
		RELEASE_ASSERT( o_map.insert( make_pair( func.s, p ) ).second == TRUE );
	}
	return true;
}


FLExecuter_Sequence * FLCStyleScript::compileProcess( vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	static size_t		depth = 0;

	++depth;


	FLExecuter_Sequence * pExec = new FLExecuter_Sequence( 0 );

	FLExecuter_If * pIf			= NULL;
	BOOL bIfPasing				= FALSE;

	for( ; it != it_end;  )
	{
		const FLCStyleReader::FLToken & r	= *it;
		switch( r.e )
		{
		case FLCStyleReader::NAME_VALUE:
			{
				if( ::strcmp( r.s.c_str(), "if" ) == 0 )
				{
					if( bIfPasing == TRUE )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : DUPLICATE IF ]" ), r.l, r.s.c_str() );	
						DEBUG_BREAK;
						return NULL;
					}

					// ()
					{
						bIfPasing	= TRUE;

						pIf			= new FLExecuter_If( r.l );
						pIf->AddCondition( * new_IfSentence( it, it_end ) );							//조건 추가.
					}

					// {}
					{
						const FLCStyleReader::FLToken o = FLCStyleReader::PopToken( it, it_end );
						if( ::strcmp( o.s.c_str(), "{" ) != 0 )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : IF문 뒤에 중괄호'{'가 없습니다. ]" ), o.l, o.s.c_str() );	
							DEBUG_BREAK;	
							return NULL;
						}

						FLExecuter_Sequence * pSeq =  compileProcess( it, it_end );

						pIf->AddSequence( * pSeq );
						const FLCStyleReader::FLToken c = FLCStyleReader::PopToken( it, it_end );
						if( ::strcmp( c.s.c_str(), "}" ) != 0 )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : IF문 뒤에 중괄호'}'가 없습니다. ]" ), c.l, c.s.c_str() );
							DEBUG_BREAK;
							return NULL;
						}

						//if의 끝인가?
						if( it == it_end || ::strcmp( (*it).s.c_str(), "else" ) != 0 && ::strcmp( (*it).s.c_str(), "it" ) != 0 )
						{
							pExec->AddExecuter( *pIf );
							pIf			= NULL;
							bIfPasing	= FALSE;
						}
					}
				}
				else if( ::strcmp( r.s.c_str(), "else" ) == 0 )
				{
					if( bIfPasing == FALSE )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s): if문 없는 else문 입니다. ]" ), r.l, r.s.c_str() );	
						DEBUG_BREAK;
						return NULL;
					}

					++it;
					// ()
					if( ::strcmp( (*it).s.c_str(), "if" ) == 0 )
					{
						pIf->AddCondition( * new_IfSentence( it, it_end ) );							//조건 추가.
					}

					// {}
					{
						const FLCStyleReader::FLToken o = FLCStyleReader::PopToken( it, it_end );
						if( ::strcmp( o.s.c_str(), "{" ) != 0 )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : IF문 뒤에 중괄호'{'가 없습니다. ]" ), o.l, o.s.c_str() );	
							DEBUG_BREAK;
							return NULL;
						}

						FLExecuter_Sequence * pSeq =  compileProcess( it, it_end );

						pIf->AddSequence( * pSeq );
						const FLCStyleReader::FLToken c = FLCStyleReader::PopToken( it, it_end );
						if( ::strcmp( c.s.c_str(), "}" ) != 0 )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : IF문 뒤에 중괄호'}'가 없습니다. ]" ), c.l, c.s.c_str() );	
							DEBUG_BREAK;
							return NULL;
						}

						//if의 끝인가?
						if( it == it_end || ( ::strcmp( (*it).s.c_str(), "else" ) != 0 && ::strcmp( (*it).s.c_str(), "it" ) != 0 ) )
						{
							pExec->AddExecuter( *pIf );
							pIf			= NULL;
							bIfPasing	= FALSE;
						}
					}
				}
				else		//function
				{
					for( ; it != it_end; )
					{
						pExec->AddExecuter( * new_Arg( it, it_end ) );
						const FLCStyleReader::FLToken c = FLCStyleReader::PopToken( it, it_end );
						if( ::strcmp( c.s.c_str(), ";" ) != 0 )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : 문장뒤에 ';'가 없습니다. ]" ), c.l, c.s.c_str() );	
							DEBUG_BREAK;
							return NULL;
						}

						break;
					}
				}
			}
			break;
		case FLCStyleReader::EOF_VALUE:
			break;

		case FLCStyleReader::SPECIAL_VALUE:
			{
				//상위 대괄호를 만났다면
				const FLCStyleReader::FLToken c = FLCStyleReader::GetToken( it, it_end );
				if( ::strcmp( c.s.c_str(), "}" ) != 0 )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) :중괄호'}'가 없습니다. ]" ), c.l, c.s.c_str() );	
					DEBUG_BREAK;
					return NULL;
				}
				--depth;

				return pExec;					//한 depth가 끝났다.
			}
			break;
		case FLCStyleReader::NUMBERIC_VALUE:
		case FLCStyleReader::STRING_VALUE:
		default:
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ line(%u) s(%s) : 지원하지 않는 문자 입니다. ]" ), r.l, r.s.c_str() );	
				DEBUG_BREAK;
				return NULL; 
			}
		}
	}

	--depth;
	return pExec;
}



//------------------------------------------------------------------------------------------------------//


E_OPERATION_TYPE 
FLCStyleScript::GetOperationType( const string & op )
{
	if( ::strcmp( op.c_str(), "+" ) == 0 
		|| ::strcmp( op.c_str(), "-" ) == 0 )
		return CALC_ADD_SUB_OPERATION;

	if( ::strcmp( op.c_str(), "/" ) == 0 
		|| ::strcmp( op.c_str(), "*" ) == 0 
		|| ::strcmp( op.c_str(), "%" ) == 0 )
		return CALC_MUL_DIV_OPERATION;

	if( ::strcmp( op.c_str(), "<=" ) == 0		//ARITHMETIC 
		|| ::strcmp( op.c_str(), ">=" ) == 0 
		|| ::strcmp( op.c_str(), "<" ) == 0 
		|| ::strcmp( op.c_str(), ">" ) == 0 
		|| ::strcmp( op.c_str(), "==" ) == 0 )
		return ARITHMETIC_COMPARE_OPERATION;

	if( ::strcmp( op.c_str(), "||" ) == 0 
		|| ::strcmp( op.c_str(), "&&" ) == 0 )		//LOGICAL
		return LOGIC_CALC_OPERATION;

	return NONE_OPERATION;
}

FLExecuter_Sentence * 
//IExecuter *
FLCStyleScript::new_IfSentence( vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	const FLCStyleReader::FLToken op		= FLCStyleReader::PopToken( it, it_end );
	if( ::strcmp( op.s.c_str(), "if" ) != 0 )	//if문이 없습니다.
	{
		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) : if문이 없습니다.", op.l, op.s.c_str() );
		DEBUG_BREAK;
		return NULL;
	}

	const FLCStyleReader::FLToken start	= FLCStyleReader::PopToken( it, it_end );
	if( ::strcmp( start.s.c_str(), "(" ) != 0 )		//if뒤에 (  가 없습니다. line
	{
		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) : if문의 '('가 없습니다.", start.l, start.s.c_str() );
		DEBUG_BREAK;
		return NULL;
	}

	FLExecuter_Sentence * pSentence = new_Sentence( it, it_end );
	//IExecuter * pSentence = new_Sentence( it, it_end );
	if( pSentence == NULL )							//조건이 없습니다.
	{
		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) : if문의 조건이 없거나 유효하지 않습니다.", op.l, op.s.c_str() );
		DEBUG_BREAK;
		return NULL;
	}

	const FLCStyleReader::FLToken end	= FLCStyleReader::PopToken( it, it_end );
	if( ::strcmp( end.s.c_str(), ")" ) != 0 )		//if뒤에 (  가 없습니다. line
	{
		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) : if문의 ')'가 없습니다.", end.l, end.s.c_str() );
		DEBUG_BREAK;
		return NULL;
	}

	return pSentence;
}

//FLExecuter_Sentence * 
//FLCStyleScript::new_Sentence( vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
//{
//	FLExecuter_Sentence * pSentence	= new FLExecuter_Sentence( 0 );
//
//	//IExecuter *		pVec = NULL;
//	E_OPERATION_TYPE old			= NONE_OPERATION;
//	FLExecuter_CalcVec * pCalcVec	= NULL;
//
//	// 인자가 연산이 아닌 경우
//	IExecuter * pLeft				= new_Arg( it, it_end );
//	const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
//	if( ::strcmp( op.s.c_str(), ")" ) == 0 || ::strcmp( op.s.c_str(), ";" ) == 0 || ::strcmp( op.s.c_str(), "," ) == 0 )	// left 인자만 있네요
//	{
//		pSentence->Add( *pLeft );
//		return pSentence;
//	}
//
//	//인자가 연산인 경우
//	for( ;; )
//	{
//		// 끝났다면
//		const FLCStyleReader::FLToken op	= FLCStyleReader::GetToken( it, it_end );
//		if( ::strcmp( op.s.c_str(), ")" ) == 0 || ::strcmp( op.s.c_str(), ";" ) == 0 || ::strcmp( op.s.c_str(), "," ) == 0 )// left 인자만 있네요
//		{
//			if( pCalcVec != NULL )
//				pSentence->Add( *pCalcVec );
//			break;
//		}
//
//		//skip op
//		++it;
//
//		// operation type 체크
//		const E_OPERATION_TYPE curr	= GetOperationType( op.s );
//		if( curr == NONE_OPERATION )
//		{
//			FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 유효하지 않은 기호 입니다.", op.l, op.s.c_str() );
//			DEBUG_BREAK;
//			return NULL;
//		}
//
//
//		// right
//		IExecuter * pRight	= new_Arg( it, it_end );
//
//
//		// 사칙 연산일 경우.
//		if( old != curr && curr == CALC_OPERATION )			//연산 처음이면		NEW VEC 
//		{
//			pCalcVec		= new FLExecuter_CalcVec( op.l );
//		}
//
//		if( curr == CALC_OPERATION )						//연산이면				VEC->add
//		{
//			FLExecuter_Calc * pCalc = new FLExecuter_Calc( pLeft->GetLine(), *pLeft, op.s, *pRight );
//			pCalcVec->Add( *pCalc );
//
//			old				= curr;
//			pLeft			= pCalc;
//			continue;
//		}
//
//		if( old != curr && old == CALC_OPERATION )			//연산 마지막이면		GROUP->add
//		{
//			pSentence->Add( *pCalcVec );
//
//			pCalcVec		= NULL;
//			old				= curr;
//
//			continue;
//		}
//
//		// 사칙 연산 이외의 경우 
//		pLeft					= new_Operation( pLeft->GetLine(), *pLeft, op.s, *pRight );
//
//		pSentence->Add( * pLeft );
//	}
//
//	return pSentence;
//}

//
//FLExecuter_Sentence * 
//FLCStyleScript::new_Sentence( vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
//{
//	FLExecuter_Sentence * pSentence	= new FLExecuter_Sentence( 0 );
//
//
//	// 인자가 연산이 아닌 경우
//	IExecuter * pLeft				= new_Arg( it, it_end );
//	const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
//	if( ::strcmp( op.s.c_str(), ")" ) == 0 || ::strcmp( op.s.c_str(), ";" ) == 0 || ::strcmp( op.s.c_str(), "," ) == 0 )	// left 인자만 있네요
//	{
//		pSentence->Add( *pLeft );
//		return pSentence;
//	}
//
//	//IExecuter *		pVec = NULL;
//	E_OPERATION_TYPE old			= NONE_OPERATION;
//	FLExecuter_CalcVec * pCalcVec	= NULL;
//
//	//인자가 연산인 경우
//	for( ;; )
//	{
//		// 끝났다면
//		const FLCStyleReader::FLToken op	= FLCStyleReader::GetToken( it, it_end );
//		if( ::strcmp( op.s.c_str(), ")" ) == 0 || ::strcmp( op.s.c_str(), ";" ) == 0 || ::strcmp( op.s.c_str(), "," ) == 0 )// left 인자만 있네요
//		{
//			if( pCalcVec != NULL )
//				pSentence->Add( *pCalcVec );
//			break;
//		}
//
//		//skip op
//		++it;
//
//		// operation type 체크
//		const E_OPERATION_TYPE curr	= GetOperationType( op.s );
//		if( curr == NONE_OPERATION )
//		{
//			FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 유효하지 않은 기호 입니다.", op.l, op.s.c_str() );
//			DEBUG_BREAK;
//			return NULL;
//		}
//
//
//		// right
//		IExecuter * pRight	= new_Arg( it, it_end );
//
//
//		// 사칙 연산일 경우.
//		if( old != curr && curr == CALC_OPERATION )			//연산 처음이면		NEW VEC 
//		{
//			pCalcVec		= new FLExecuter_CalcVec( op.l );
//		}
//
//		if( curr == CALC_OPERATION )						//연산이면				VEC->add
//		{
//			FLExecuter_Calc * pCalc = new FLExecuter_Calc( pLeft->GetLine(), *pLeft, op.s, *pRight );
//			pCalcVec->Add( *pCalc );
//
//			old				= curr;
//			pLeft			= pCalc;
//			continue;
//		}
//
//		if( old != curr && old == CALC_OPERATION )			//연산 마지막이면		GROUP->add
//		{
//			pSentence->Add( *pCalcVec );
//
//			pCalcVec		= NULL;
//			old				= curr;
//
//			continue;
//		}
//
//		// 사칙 연산 이외의 경우 
//		pLeft					= new_Operation( pLeft->GetLine(), *pLeft, op.s, *pRight );
//
//		pSentence->Add( * pLeft );
//	}
//
//	return pSentence;
//}


IExecuter * 
FLCStyleScript::new_Arg( vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	const FLCStyleReader::FLToken arg	= FLCStyleReader::PopToken( it, it_end );	
	switch( arg.e )
	{
	case FLCStyleReader::NAME_VALUE:		
		{
			int num = 0;
			if( FLDefinedsToString::GetInstance().GetInt( num, arg.s.c_str() ) == true )		//number
			{
				return new FLExecuter_Number( arg.l, num );
			}

			return new_Funtion( arg.s, it, it_end );		//funtion
		}
		break;
	case FLCStyleReader::NUMBERIC_VALUE:	//Number
		return new FLExecuter_Number( arg.l, ::atoi( arg.s.c_str() ) );
		break;
	case FLCStyleReader::SPECIAL_VALUE:
		{
			//숫자
			if( ::strcmp( arg.s.c_str(), "+" ) == 0 || ::strcmp( arg.s.c_str(), "-" ) == 0 )
			{
				const FLCStyleReader::FLToken num = FLCStyleReader::PopToken( it, it_end );
				if( num.e != FLCStyleReader::NUMBERIC_VALUE )
				{
					FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 숫자가 아닌곳에 부호를 붙였습니다.", num.l, num.s.c_str() );
					DEBUG_BREAK;
					return NULL;
				}
				if( ::strcmp( arg.s.c_str(), "+" ) == 0 )
					return new FLExecuter_Number( num.l, ::atoi( num.s.c_str() ) );
				else //if( ::strcmp( arg.s.c_str(), "-" ) == 0 )
					return new FLExecuter_Number( num.l, ::atoi( num.s.c_str() ) * -1 );
			}

			// 괄호 시작
			if( ::strcmp( arg.s.c_str(), "(" ) != 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 괄호 시작'('이 없습니다.", arg.l, arg.s.c_str() );
				DEBUG_BREAK;
				return NULL;
			}

			IExecuter * pNew	= new_Sentence( it, it_end );
			const FLCStyleReader::FLToken close = FLCStyleReader::PopToken( it, it_end );
			if( ::strcmp( close.s.c_str(), ")" ) != 0 )					//닫히는 부분이 없네요.
			{
				FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 괄호 끝')'이 없습니다.", close.l, close.s.c_str() );
				DEBUG_BREAK;
				return NULL;
			}
			return pNew;
		}
		break;

	default:
		{
			FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 유효한 문자가 아닙니다 ]", arg.l, arg.s.c_str() );
			DEBUG_BREAK;
			return NULL;
		}
		break;
	}

//	return NULL;
}

//IExecuter_BinaryOperation * 
//FLCStyleScript::new_Operation( const size_t line, IExecuter & l, const string & op, IExecuter & r )
//{
//	const E_OPERATION_TYPE e = GetOperationType( op );
//
//	if( e == LOGIC_CALC_OPERATION )
//		return new FLExecuter_LogicCmp( line, l, op, r );
//	else if( e == ARITHMETIC_COMPARE_OPERATION )
//		return new FLExecuter_ArithmeticCmp( line, l, op, r );
//	else if( e == CALC_OPERATION )
//		return new FLExecuter_Calc( line, l, op, r );
//	
//
//	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) value(%s) 사칙연산도 비교연산도 아닙니다. ]", line, op.c_str() );
//	DEBUG_BREAK;
//	return NULL;
//}

FLExecuter_Function * 
FLCStyleScript::new_Funtion( const string & name, vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	const FLCStyleReader::FLToken s		= FLCStyleReader::PopToken( it, it_end );	
	if( ::strcmp( s.s.c_str(), "(" ) != 0 )		// 함수 시작이 없습니다.
	{
		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) function(%s) 함수 시작'('이 없습니다.", s.l, name.c_str() );
		DEBUG_BREAK;
		return NULL;
	}

	FLExecuter_Function * func	= new FLExecuter_Function( s.l, name );
	for( ;; )
	{
		//함수 끝
		if( ::strcmp( (*it).s.c_str(), ")" ) == 0 )
		{
			++it;
			break;
		}

		// arg 시작
		IExecuter * arg			= new_Sentence( it, it_end );
		if( arg == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, "[ line(%u) function(%s) 유효하지 않은 인자 입니다.", s.l, name.c_str() );
			DEBUG_BREAK;
			return NULL;
		}

		func->AddArgs( *arg );

		// 인자가 더 있으면
		if( ::strcmp( (*it).s.c_str(), "," ) == 0 )
		{
			++it;
			continue;
		}

		if( ::strcmp( (*it).s.c_str(), ")" ) != 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, "[ line(%u) function(%s) 함수의 끝')'이 없습니다.", s.l, name.c_str() );
			DEBUG_BREAK;
			return NULL;
		}
	}

	return func;
}


bool FLCStyleScript::FindFunction( const string & func )
{
	return FindNPCFunction( func );
}

bool FLCStyleScript::CallFunction( const string & func )
{
	return CallNPCFunction( func );
}



//------------------------------------------------------------------------------------------------//

//int FLExecuter_Noop::Execute()
//{
//	DebugOutput( "[%06d] _noop : 0 ", GetLine() );
//	return 0;
//}

//------------------------------------------------------------------------------------------------//

FLExecuter_Number::FLExecuter_Number( const size_t line, const int num ) 
: m_line( line ), m_num( num )
{

}

int FLExecuter_Number::Execute()
{
	DebugOutput( "[%06d] num : %d ", m_line, m_num );
	return m_num;
}

//------------------------------------------------------------------------------------------------//

FLExecuter_Function::FLExecuter_Function( const size_t line, const string & str ) 
: m_line( line ), m_strFuntion( str )
{

}

FLExecuter_Function::~FLExecuter_Function()
{
	for( vector< IExecuter * >::iterator it = m_args.begin(); it != m_args.end(); ++it )
		delete *it;
}

void FLExecuter_Function::AddArgs( IExecuter & op )
{
	m_args.push_back( &op );
}

int FLExecuter_Function::Execute()
{
	DebugOutput( "[%06d] f%s : %d ", m_line, m_strFuntion.c_str(), 0 );

	// args
	vector< int > args;
	for( vector< IExecuter * >::iterator it = m_args.begin(); it != m_args.end(); ++it )
	{
		IExecuter * p		= *it;
		const int n			= p->Execute();
		args.push_back( n );
	}

	// execute
	if( GetNPCScriptInstance().m_pInfo == NULL )			
	{
		//FLERROR_LOG( PROGRAM_NAME, _T("[ line(%u) func(%s) : ]"), m_line, m_strFuntion.c_str() );	//  CWorldDialog::Run 재귀 호출로 인해 NULL이 될수도 있다.
		//DEBUG_BREAK;
		return 0;
	}
	
	return GetNPCScriptInstance().CallIncomeFunc( m_strFuntion.c_str(), args );
}

//------------------------------------------------------------------------------------------------//

//FLExecuter_Calc::FLExecuter_Calc( const size_t line, IExecuter & l, const std::string & op, IExecuter & r )
//: IExecuter_BinaryOperation( line, l, op, r )
//{
//	if( FLCStyleScript::GetOperationType( op ) != CALC_OPERATION )
//	{
//		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 연산 인자 입니다.]", l.GetLine(), op.c_str() );
//		DEBUG_BREAK;
//	}
//}
//
//FLExecuter_Calc::~FLExecuter_Calc()
//{
//	delete m_l;
//	delete m_r;
//}
//
//int FLExecuter_Calc::Execute()
//{
//	return execute( *m_l, m_op, *m_r );
//}
//
//int FLExecuter_Calc::Execute_ModifyLeftValue( const int l )
//{
//	FLExecuter_Number tmp( 0, l );
//	return execute( tmp, m_op, *m_r );
//}
//
//int FLExecuter_Calc::Execute_ModifyRightValue( const int r )
//{
//	FLExecuter_Number tmp( 0, r );
//	return execute( *m_l, m_op, tmp );
//}
//
//int FLExecuter_Calc::Execute_ModifyValue( const int l, const int r )
//{
//	FLExecuter_Number tmpL( 0, l );
//	FLExecuter_Number tmpR( 0, r );
//	return execute( tmpL, m_op, tmpR );
//}
//
//int FLExecuter_Calc::execute( IExecuter & l, const std::string & op, IExecuter & r )
//{
//	const int nl			= l.Execute();
//	const int nr			= r.Execute();
//
//	DebugOutput( "[%06d] %d (%s) %d ", m_line, nl, op.c_str(), nr );
//
//	// 산술 연산
//	if( ::strcmp( op.c_str(), "+" ) == 0 )
//	{
//		return nl + nr;
//	}
//	else if( ::strcmp( op.c_str(), "-" ) == 0 )
//	{
//		return nl - nr;
//	}
//	else if( ::strcmp( op.c_str(), "/" ) == 0 )
//	{
//		return nl / nr;
//	}
//	else if( ::strcmp( op.c_str(), "*" ) == 0 )
//	{
//		return nl * nr;
//	}
//
//	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 연산 인자 입니다.]", l.GetLine(), op.c_str() );
//	DEBUG_BREAK;
//	return 0;
//}
//------------------------------------------------------------------------------------------------//

//FLExecuter_Cmp::FLExecuter_Cmp( const size_t line, IExecuter & l, const std::string & op, IExecuter & r )
//: IExecuter_BinaryOperation( line, l, op, r )
//{
//	DEBUG_ASSERT( FLCStyleScript::GetOperationType( op ) == COMPARE_OPERATION );
//}
//
//FLExecuter_Cmp::~FLExecuter_Cmp()
//{
//	delete m_l;
//	delete m_r;
//}
//
//int FLExecuter_Cmp::Execute()
//{
//	return execute( *m_l, m_op, *m_r );
//}
//
//int FLExecuter_Cmp::Execute_ModifyLeftValue( const int l )
//{
//	FLExecuter_Number ll( 0, l );
//	return execute( ll, m_op, *m_r );
//}
//
//int FLExecuter_Cmp::Execute_ModifyRightValue( const int r )
//{
//	FLExecuter_Number rr( 0, r );
//	return execute( *m_l, m_op, rr );
//}
//
//int FLExecuter_Cmp::Execute_ModifyValue( const int l, const int r )
//{
//	FLExecuter_Number ll( 0, l );
//	FLExecuter_Number rr( 0, r );
//	return execute( ll, m_op, rr );
//}
//
//int FLExecuter_Cmp::execute( IExecuter & l, const std::string & op, IExecuter & r )
//{
//	DebugOutput( "[%06d] ? (%s) ? ", m_line, op.c_str() );
//
//	//// 비교연산
//	if( ::strcmp( op.c_str(), "==" ) == 0 )
//	{
//		return l.Execute() == r.Execute();
//	}
//	else if( ::strcmp( op.c_str(), "||" ) == 0 )
//	{
//		return l.Execute() || r.Execute();	
//	}
//	else if( ::strcmp( op.c_str(), "&&" ) == 0 )
//	{
//		return l.Execute() && r.Execute();
//	}
//	else if( ::strcmp( op.c_str(), "<=" ) == 0 )
//	{
//		return l.Execute() <= r.Execute();
//	}
//	else if( ::strcmp( op.c_str(), ">=" ) == 0 )
//	{
//		return l.Execute() >= r.Execute();
//	}
//	else if( ::strcmp( op.c_str(), "<" ) == 0 )
//	{
//		return l.Execute() < r.Execute();
//	}
//	else if( ::strcmp( op.c_str(), ">" ) == 0 )
//	{
//		return l.Execute() > r.Execute();
//	}
//
//	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 비교 인자 입니다.]", l.GetLine(), op.c_str() );
//	DEBUG_BREAK;
//	return 0;
//}


//------------------------------------------------------------------------------------------------//

FLExecuter_Sequence::~FLExecuter_Sequence()
{
	for( vector< IExecuter * >::iterator it = m_vecOp.begin(); it != m_vecOp.end(); ++it )
	{
		IExecuter * p	= *it;
		delete p;
	}
}

void FLExecuter_Sequence::AddExecuter( IExecuter & p )
{
	m_vecOp.push_back( &p );
}

int FLExecuter_Sequence::Execute()
{
	for( vector< IExecuter * >::iterator it = m_vecOp.begin(); it != m_vecOp.end(); ++it )
	{
		IExecuter * p	= *it;
		p->Execute();
	}

	return 0;
}


//----------------------------------------------------------------------------------------------------//
FLExecuter_If::~FLExecuter_If()
{
	for( vector< FLExecuter_Sentence * >::iterator it = m_vecConditions.begin(); it != m_vecConditions.end(); ++it )		
	{
		IExecuter * p		= *it;
		delete p;
	}

	for( vector< FLExecuter_Sequence * >::iterator it = m_vecProcess.begin(); it != m_vecProcess.end(); ++it )		
	{
		IExecuter * p		= *it;
		delete p;
	}

	if( m_pLastElseProcess != NULL )
		delete m_pLastElseProcess;
}

void FLExecuter_If::AddCondition( FLExecuter_Sentence & p )
{
	m_vecConditions.push_back( &p );
}

void FLExecuter_If::AddSequence( FLExecuter_Sequence & p )
{
	if( m_vecConditions.size() == m_vecProcess.size() )
	{
		m_pLastElseProcess		= &p;
		return;
	}

	m_vecProcess.push_back( &p );
}

int FLExecuter_If::Get_GotoIndex()
{
	int i = 0;
	for( vector< FLExecuter_Sentence * >::iterator it = m_vecConditions.begin(); it != m_vecConditions.end(); ++it, ++i )		
	{
		IExecuter * p		= *it;
		if( p->Execute() != 0 )
			return i;
	}
	return -1;			//조건이 업는경우 마지막 else 조건이다
}

int FLExecuter_If::Execute()
{
	const int i			= Get_GotoIndex();
	if( i == -1 )
	{
		if( m_pLastElseProcess != NULL )			//마지막 ELSE가 있다면
			return m_pLastElseProcess->Execute();

		return 0;
	}

	return m_vecProcess.at( i )->Execute();
}



//--------------------------------------------------------------------------------------------------------//

//
//FLExecuter_CalcVec::~FLExecuter_CalcVec()
//{
//	for( vector< FLExecuter_Calc * >::iterator it = m_vecMulDiv.begin(); it != m_vecMulDiv.end(); ++it )
//	{
//		IExecuter_BinaryOperation * p	= *it;
//		delete p;
//	}
//
//	for( vector< FLExecuter_Calc * >::iterator it = m_vecSumSub.begin(); it != m_vecSumSub.end(); ++it )
//	{
//		IExecuter_BinaryOperation * p	= *it;
//		delete p;
//	}
//
//
//}
//
//void FLExecuter_CalcVec::Add( FLExecuter_Calc & p )
//{
//	if( ::strcmp( p.m_op.c_str(), "/" ) == 0 || ::strcmp( p.m_op.c_str(), "*" ) == 0 )
//	{
//		m_vecMulDiv.push_back( &p );
//		return;
//	}
//
//	if( ::strcmp( p.m_op.c_str(), "+" ) == 0 || ::strcmp( p.m_op.c_str(), "-" ) == 0 )
//	{
//		m_vecSumSub.push_back( &p );
//		return;
//	}
//
//	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 연산 객체 입니다.]", p.GetLine(), p.m_op.c_str() );
//	DEBUG_BREAK;
//}
//
//int FLExecuter_CalcVec::Execute()
//{
//	if( m_vecMulDiv.empty() == true && m_vecSumSub.empty() == true )
//	{
//		FLERROR_LOG( PROGRAM_NAME, "[ line(%u) 연산할 것이 없습니다 ]", GetLine() );
//		DEBUG_BREAK;
//		return 0;
//	}
//
//
//	int left			= 0;
//
//	if( m_vecMulDiv.empty() == false )
//	{
//		vector< FLExecuter_Calc * >::iterator it = m_vecMulDiv.begin();
//		IExecuter * p	= *it;
//		left			= p->Execute();
//		++it;
//		for( ; it != m_vecMulDiv.end(); ++it )
//		{
//			IExecuter_BinaryOperation * p	= *it;
//			left			= p->Execute_ModifyLeftValue( left );
//		}
//
//		for( vector< FLExecuter_Calc * >::iterator it = m_vecSumSub.begin(); it != m_vecSumSub.end(); ++it )
//		{
//			IExecuter_BinaryOperation * p	= *it;
//			left			= p->Execute_ModifyLeftValue( left );
//		}
//		return left;
//	}
//	else if( m_vecSumSub.empty() == false )
//	{
//		vector< FLExecuter_Calc * >::iterator it = m_vecSumSub.begin();
//		IExecuter * p	= *it;
//		left			= p->Execute();
//		++it;
//
//		for( ; it != m_vecSumSub.end(); ++it )
//		{
//			IExecuter_BinaryOperation * p	= *it;
//			left			= p->Execute_ModifyLeftValue( left );
//		}
//		return left;
//	}
//
//	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) 연산할 것이 없습니다 ]", GetLine() );
//	DEBUG_BREAK;
//	return 0;
//}


//--------------------------------------------------------------------------------------------------------//

FLExecuter_Sentence::~FLExecuter_Sentence()
{
	for( vector< IExecuter * >::iterator it = m_vec.begin(); it != m_vec.end(); ++it )
		delete *it;

}

int FLExecuter_Sentence::Execute()
{
	map< DWORD /* IExecuter* key */, int >		mapResult;

	int ret				= 0;

	for( vector< IExecuter * >::iterator it = m_vec.begin(); it != m_vec.end(); ++it )
	{
		IExecuter_BinaryOperation * p		= dynamic_cast< IExecuter_BinaryOperation *>( *it );
		if( p == NULL )
		{
			IExecuter * op	= *it;
			ret				= op->Execute();
			mapResult.insert( make_pair( reinterpret_cast< DWORD >( p ), ret ) );
			continue;
		}

		map< DWORD, int >::const_iterator it_l		= mapResult.find( reinterpret_cast< DWORD >( p->m_l ) );
		map< DWORD, int >::const_iterator it_r		= mapResult.find( reinterpret_cast< DWORD >( p->m_r ) );
		if( it_l != mapResult.end() && it_r != mapResult.end() )
			ret			= p->Execute_ModifyValue( it_l->second, it_r->second );
		else if( it_l != mapResult.end() )
			ret			= p->Execute_ModifyLeftValue( it_l->second );
		else if( it_r != mapResult.end() )
			ret			= p->Execute_ModifyLeftValue( it_r->second );
		else
			ret			= p->Execute();	

		mapResult.insert( make_pair( reinterpret_cast< DWORD >( p ), ret ) );
	}
	return ret;
}

void FLExecuter_Sentence::Add( IExecuter & p ) 
{
	m_vec.push_back( &p );
}





//---------------------------------------------------------------------------------------------------------------------------------//





FLExecuter_LogicCmp::FLExecuter_LogicCmp( const size_t line, IExecuter & l, const std::string & op, IExecuter & r )
: IExecuter_BinaryOperation( line, l, op, r )
{
	RELEASE_ASSERT( FLCStyleScript::GetOperationType( op ) == LOGIC_CALC_OPERATION );
}

FLExecuter_LogicCmp::~FLExecuter_LogicCmp()
{
	delete m_l;
	delete m_r;
}

int FLExecuter_LogicCmp::Execute()
{
	return execute( *m_l, m_op, *m_r );
}

int FLExecuter_LogicCmp::Execute_ModifyLeftValue( const int l )
{
	FLExecuter_Number ll( 0, l );
	return execute( ll, m_op, *m_r );
}

int FLExecuter_LogicCmp::Execute_ModifyRightValue( const int r )
{
	FLExecuter_Number rr( 0, r );
	return execute( *m_l, m_op, rr );
}

int FLExecuter_LogicCmp::Execute_ModifyValue( const int l, const int r )
{
	FLExecuter_Number ll( 0, l );
	FLExecuter_Number rr( 0, r );
	return execute( ll, m_op, rr );
}

int FLExecuter_LogicCmp::execute( IExecuter & l, const std::string & op, IExecuter & r )
{
	DebugOutput( "[%06d] ? (%s) ? ", m_line, op.c_str() );

	//// 비교연산
	if( ::strcmp( op.c_str(), "==" ) == 0 )
	{
		return l.Execute() == r.Execute();
	}
	else if( ::strcmp( op.c_str(), "||" ) == 0 )
	{
		return l.Execute() || r.Execute();	
	}
	else if( ::strcmp( op.c_str(), "&&" ) == 0 )
	{
		return l.Execute() && r.Execute();
	}

	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 비교 인자 입니다.]", l.GetLine(), op.c_str() );
	DEBUG_BREAK;
	return 0;
}

//--

FLExecuter_ArithmeticCmp::FLExecuter_ArithmeticCmp( const size_t line, IExecuter & l, const std::string & op, IExecuter & r )
: IExecuter_BinaryOperation( line, l, op, r )
{
	RELEASE_ASSERT( FLCStyleScript::GetOperationType( op ) == ARITHMETIC_COMPARE_OPERATION );
}

FLExecuter_ArithmeticCmp::~FLExecuter_ArithmeticCmp()
{
	delete m_l;
	delete m_r;
}

int FLExecuter_ArithmeticCmp::Execute()
{
	return execute( *m_l, m_op, *m_r );
}

int FLExecuter_ArithmeticCmp::Execute_ModifyLeftValue( const int l )
{
	FLExecuter_Number ll( 0, l );
	return execute( ll, m_op, *m_r );
}

int FLExecuter_ArithmeticCmp::Execute_ModifyRightValue( const int r )
{
	FLExecuter_Number rr( 0, r );
	return execute( *m_l, m_op, rr );
}

int FLExecuter_ArithmeticCmp::Execute_ModifyValue( const int l, const int r )
{
	FLExecuter_Number ll( 0, l );
	FLExecuter_Number rr( 0, r );
	return execute( ll, m_op, rr );
}

int FLExecuter_ArithmeticCmp::execute( IExecuter & l, const std::string & op, IExecuter & r )
{
	DebugOutput( "[%06d] ? (%s) ? ", m_line, op.c_str() );

	//// 비교연산
	if( ::strcmp( op.c_str(), "<=" ) == 0 )
	{
		return l.Execute() <= r.Execute();
	}
	else if( ::strcmp( op.c_str(), ">=" ) == 0 )
	{
		return l.Execute() >= r.Execute();
	}
	else if( ::strcmp( op.c_str(), "<" ) == 0 )
	{
		return l.Execute() < r.Execute();
	}
	else if( ::strcmp( op.c_str(), ">" ) == 0 )
	{
		return l.Execute() > r.Execute();
	}
	else if( ::strcmp( op.c_str(), "==" ) == 0 )
	{
		return l.Execute() == r.Execute();
	}

	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 비교 인자 입니다.]", l.GetLine(), op.c_str() );
	DEBUG_BREAK;
	return 0;
}

//--

FLExecuter_CalcAddSub::FLExecuter_CalcAddSub( const size_t line, IExecuter & l, const std::string & op, IExecuter & r )
: IExecuter_BinaryOperation( line, l, op, r )
{
	RELEASE_ASSERT( FLCStyleScript::GetOperationType( op ) == CALC_ADD_SUB_OPERATION );
}

FLExecuter_CalcAddSub::~FLExecuter_CalcAddSub()
{
	delete m_l;
	delete m_r;
}

int FLExecuter_CalcAddSub::Execute()
{
	return execute( *m_l, m_op, *m_r );
}

int FLExecuter_CalcAddSub::Execute_ModifyLeftValue( const int l )
{
	FLExecuter_Number ll( 0, l );
	return execute( ll, m_op, *m_r );
}

int FLExecuter_CalcAddSub::Execute_ModifyRightValue( const int r )
{
	FLExecuter_Number rr( 0, r );
	return execute( *m_l, m_op, rr );
}

int FLExecuter_CalcAddSub::Execute_ModifyValue( const int l, const int r )
{
	FLExecuter_Number ll( 0, l );
	FLExecuter_Number rr( 0, r );
	return execute( ll, m_op, rr );
}

int FLExecuter_CalcAddSub::execute( IExecuter & l, const std::string & op, IExecuter & r )
{
	DebugOutput( "[%06d] ? (%s) ? ", m_line, op.c_str() );

	//// 비교연산
	if( ::strcmp( op.c_str(), "+" ) == 0 )
	{
		return l.Execute() + r.Execute();
	}
	else if( ::strcmp( op.c_str(), "-" ) == 0 )
	{
		return l.Execute() >= r.Execute();
	}

	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 비교 인자 입니다.]", l.GetLine(), op.c_str() );
	DEBUG_BREAK;
	return 0;
}


//--

FLExecuter_CalcMulDiv::FLExecuter_CalcMulDiv( const size_t line, IExecuter & l, const std::string & op, IExecuter & r )
: IExecuter_BinaryOperation( line, l, op, r )
{
	RELEASE_ASSERT( FLCStyleScript::GetOperationType( op ) == CALC_MUL_DIV_OPERATION );
}

FLExecuter_CalcMulDiv::~FLExecuter_CalcMulDiv()
{
	delete m_l;
	delete m_r;
}

int FLExecuter_CalcMulDiv::Execute()
{
	return execute( *m_l, m_op, *m_r );
}

int FLExecuter_CalcMulDiv::Execute_ModifyLeftValue( const int l )
{
	FLExecuter_Number ll( 0, l );
	return execute( ll, m_op, *m_r );
}

int FLExecuter_CalcMulDiv::Execute_ModifyRightValue( const int r )
{
	FLExecuter_Number rr( 0, r );
	return execute( *m_l, m_op, rr );
}

int FLExecuter_CalcMulDiv::Execute_ModifyValue( const int l, const int r )
{
	FLExecuter_Number ll( 0, l );
	FLExecuter_Number rr( 0, r );
	return execute( ll, m_op, rr );
}

int FLExecuter_CalcMulDiv::execute( IExecuter & l, const std::string & op, IExecuter & r )
{
	DebugOutput( "[%06d] ? (%s) ? ", m_line, op.c_str() );

	//// 비교연산
	if( ::strcmp( op.c_str(), "*" ) == 0 )
	{
		return l.Execute() <= r.Execute();
	}
	else if( ::strcmp( op.c_str(), "/" ) == 0 )
	{
		return l.Execute() >= r.Execute();
	}
	else if( ::strcmp( op.c_str(), "%" ) == 0 )
	{
		return l.Execute() < r.Execute();
	}

	FLERROR_LOG( PROGRAM_NAME, "[ line(%u) (%s) 잘못된 비교 인자 입니다.]", l.GetLine(), op.c_str() );
	DEBUG_BREAK;
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------


FLExecuter_Sentence * 
FLCStyleScript::new_Sentence( vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	FLExecuter_Sentence * pSentence	= new FLExecuter_Sentence( 0 );

	// 인자가 연산이 아닌 경우
	IExecuter * pLeft				= new_Arg( it, it_end );
	const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
	if( ::strcmp( op.s.c_str(), ")" ) == 0 || ::strcmp( op.s.c_str(), ";" ) == 0 || ::strcmp( op.s.c_str(), "," ) == 0 )	// left 인자만 있네요
	{
		pSentence->Add( *pLeft );
		return pSentence;
	}

	FLExecuter_CalcSequence * pOneSentence	= new_TryLogicCmpSentence( pLeft, it, it_end );		//최우선순위 연산 시도.
	if( pOneSentence == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[ ComplieScript line(%d) : (%s) ]"), pLeft->GetLine(), (*it).s.c_str() );
		RELEASE_BREAK;
		return NULL;
	}
	
	pSentence->Add( *pOneSentence );
	
	return pSentence;
}

FLExecuter_CalcSequence * FLCStyleScript::new_TryLogicCmpSentence( IExecuter * pLeft, vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	RELEASE_ASSERT( pLeft != NULL );

	const FLCStyleReader::FLToken token = FLCStyleReader::GetToken( it, it_end );
	if( FLCStyleScript::IsSentenceEnd( token.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
		return NULL;

	FLExecuter_CalcSequence * pSequence	= NULL;
	IExecuter * left					= pLeft;
	for( ;; )
	{
		const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
		if( FLCStyleScript::GetOperationType( op.s ) != LOGIC_CALC_OPERATION )		//1. 하위 우선순위 연산을 검색
		{
			//하위 트리를 찾는다.
			left						= new_TryArithmeticCmpSentence( left, it, it_end );	
			if( left == NULL )					//2. 종료?
				return pSequence;

			const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
			if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			{
				if( pSequence == NULL)
					pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
				pSequence->PushBack( left );
				return pSequence;
			}

			continue;
		}

		FLCStyleReader::PopToken( it, it_end );

		// 다음 우선순위가 있나 찾아본다.
		IExecuter * nextLeft			= new_Arg( it, it_end );
		RELEASE_ASSERT( nextLeft != NULL );

		IExecuter * right				= new_TryArithmeticCmpSentence( nextLeft, it, it_end );	//@@@@@@@@@재귀호출???????
		if( right == NULL )		
			right						= nextLeft;

		RELEASE_ASSERT( right != NULL );

		// 하나의 이항 연산이 완성됨.
		IExecuter_BinaryOperation * pCalc		= new FLExecuter_LogicCmp( left->GetLine(), *left, op.s, *right );
		RELEASE_ASSERT( pCalc != NULL );
		if( pSequence == NULL)
			pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
		pSequence->PushBack( pCalc );

		left							= pCalc;

		if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			return pSequence;
	}
}

FLExecuter_CalcSequence * FLCStyleScript::new_TryArithmeticCmpSentence( IExecuter * pLeft, vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	RELEASE_ASSERT( pLeft != NULL );

	const FLCStyleReader::FLToken token= FLCStyleReader::GetToken( it, it_end );
	if( FLCStyleScript::IsSentenceEnd( token.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
	{
		return NULL;
	}

	FLExecuter_CalcSequence * pSequence	= NULL;
	IExecuter * left					= pLeft;
	for( ;; )
	{
		const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
		if( FLCStyleScript::GetOperationType( op.s ) != ARITHMETIC_COMPARE_OPERATION )		//1. 하위 우선순위 연산을 검색
		{
			//하위 트리를 찾는다.
			left						= new_TryCalcAddSub( left, it, it_end );	
			if( left == NULL )					//2. 종료?
				return pSequence;

			const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
			if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			{
				if( pSequence == NULL)
					pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
				pSequence->PushBack( left );
				return pSequence;
			}

			continue;
		}

		FLCStyleReader::PopToken( it, it_end );

		// 다음 우선순위가 있나 찾아본다.
		IExecuter * nextLeft			= new_Arg( it, it_end );
		RELEASE_ASSERT( nextLeft != NULL );
				
		IExecuter * right				= new_TryCalcAddSub( nextLeft, it, it_end );	//@@@@@@@@@재귀호출???????
		if( right == NULL )		
			right						= nextLeft;

		RELEASE_ASSERT( right != NULL );
		

		// 하나의 이항 연산이 완성됨.
		IExecuter_BinaryOperation * pCalc		= new FLExecuter_ArithmeticCmp( left->GetLine(), *left, op.s, *right );
		RELEASE_ASSERT( pCalc != NULL );
		if( pSequence == NULL)
			pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
		pSequence->PushBack( pCalc );

		left							= pCalc;

		if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			return pSequence;
	}
}


FLExecuter_CalcSequence * FLCStyleScript::new_TryCalcAddSub( IExecuter * pLeft, vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	RELEASE_ASSERT( pLeft != NULL );

	const FLCStyleReader::FLToken token= FLCStyleReader::GetToken( it, it_end );
	if( FLCStyleScript::IsSentenceEnd( token.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
	{
		return NULL;
	}

	FLExecuter_CalcSequence * pSequence	= NULL;
	IExecuter * left					= pLeft;
	for( ;; )
	{
		const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
		if( FLCStyleScript::GetOperationType( op.s ) != CALC_ADD_SUB_OPERATION )		//1. 하위 우선순위 연산을 검색
		{
			//하위 트리를 찾는다.
			left						= new_TryCalcMulDiv( left, it, it_end );	
			if( left == NULL )					//2. 종료?
				return pSequence;

			const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
			if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			{
				if( pSequence == NULL)
					pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
				pSequence->PushBack( left );
				return pSequence;
			}

			continue;
		}

		FLCStyleReader::PopToken( it, it_end );

		// 다음 우선순위가 있나 찾아본다.
		IExecuter * nextLeft			= new_Arg( it, it_end );
		RELEASE_ASSERT( nextLeft != NULL );

		IExecuter * right				= new_TryCalcMulDiv( nextLeft, it, it_end );	//@@@@@@@@@재귀호출???????
		if( right == NULL )		
			right						= nextLeft;

		// 하나의 이항 연산이 완성됨.
		IExecuter_BinaryOperation * pCalc		= new FLExecuter_CalcAddSub( left->GetLine(), *left, op.s, *right );
		RELEASE_ASSERT( pCalc != NULL );
		if( pSequence == NULL )
			pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
		pSequence->PushBack( pCalc );

		left							= pCalc;

		if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			return pSequence;
	}
}

FLExecuter_CalcSequence * FLCStyleScript::new_TryCalcMulDiv( IExecuter * pLeft, vector< FLCStyleReader::FLToken >::const_iterator & it, vector< FLCStyleReader::FLToken >::const_iterator & it_end )
{
	RELEASE_ASSERT( pLeft != NULL );

	const FLCStyleReader::FLToken token= FLCStyleReader::GetToken( it, it_end );
	if( FLCStyleScript::IsSentenceEnd( token.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
	{
		return NULL;
	}

	FLExecuter_CalcSequence * pSequence	= NULL;
	IExecuter * left					= pLeft;
	for( ;; )
	{
		const FLCStyleReader::FLToken op= FLCStyleReader::GetToken( it, it_end );
		if( FLCStyleScript::GetOperationType( op.s ) != CALC_MUL_DIV_OPERATION )		//1. 하위 우선순위 연산을 검색
		{
			return pSequence;
		}

		FLCStyleReader::PopToken( it, it_end );

		IExecuter * right				= new_Arg( it, it_end );
		RELEASE_ASSERT( right != NULL );

		// 하나의 이항 연산이 완성됨.
		IExecuter_BinaryOperation * pCalc		= new FLExecuter_CalcMulDiv( left->GetLine(), *left, op.s, *right );
		RELEASE_ASSERT( pCalc != NULL );
		if( pSequence == NULL )
			pSequence					= new FLExecuter_CalcSequence( left->GetLine() );
		pSequence->PushBack( pCalc );

		left							= pCalc;

		if( FLCStyleScript::IsSentenceEnd( op.s ) == true )				//단항이 들어온다.  // 없애면 NULL이 나간다.
			return pSequence;
	}
}

//----------------------------------------------------------------------------------------------//

FLExecuter_CalcSequence::FLExecuter_CalcSequence( const size_t line ) : m_line( line )
{

}

FLExecuter_CalcSequence::~FLExecuter_CalcSequence()
{
	for( vector< IExecuter * >::iterator it = m_vecSequence.begin(); it != m_vecSequence.end(); ++it )
	{
		delete *it;
	}
}

int FLExecuter_CalcSequence::Execute()
{
	vector< IExecuter * >::iterator it = m_vecSequence.begin();
	IExecuter * p		= *it;
	int left			= p->Execute();
	++it;
	for( ; it != m_vecSequence.end(); ++it )
	{
		IExecuter_BinaryOperation * p	= dynamic_cast< IExecuter_BinaryOperation * >( *it );
		DEBUG_ASSERT( p != NULL );

		if( p != NULL )
			left						= p->Execute_ModifyLeftValue( left );
	}

	return left;
}

size_t FLExecuter_CalcSequence::GetLine()
{
	return m_line;
}

void FLExecuter_CalcSequence::PushBack( IExecuter * p )
{
	m_vecSequence.push_back( p );
}


