#include "stdafx.h"
#include "serialnumber.h"


CSerialNumber::~CSerialNumber()
{
}

//////////////////////////////////////////////////////////////////////////
class	CRandomSerialNumber : public CSerialNumber
{
public:

	// constructions
	CRandomSerialNumber()	{}
	~CRandomSerialNumber()	{}

	// operations
	bool	Init( CSerialNumberRequest * ) { return true; }
	bool	Add( SERIALNUMBER /*iStart*/, SERIALNUMBER /*iEnd*/ )	{ return true; }

	bool	IsInit() { return true; }

	SERIALNUMBER	Get()
	{
		//return xRand();
		//time_t time2 = GetTimeT(2016,1,1,1,1);
		time_t t	= ::time( NULL ) - 1451779200;
		int cur_sec = t;
		static int last_record_sec = t;
		static int record_count = 0;
		static int static_count = t - 315360000;
		int unique_number = 0;
		
		
		
		if   (cur_sec != last_record_sec)
		{
			last_record_sec = t;
			 record_count = 0;
			unique_number = (cur_sec <<3);
		}   else
		{
			  record_count++;
			  if (record_count>=8)    
			 {
					  //unique_number = (cur_sec <<3) + (record_count%8);
					 unique_number = static_count++;
			 }   else
 			{
				   unique_number = (cur_sec <<3) + record_count;
			}
		}
		
		return unique_number;
	}

	bool	IsManaged()
	{
		return false;
	}
	void	InsertSerialNumber( SERIALNUMBER /*iSerialNumber*/ )
	{
	}
	void	RemoveSerialNumber( SERIALNUMBER /*iSerialNumber*/ )
	{
	}

};
//////////////////////////////////////////////////////////////////////////
typedef	struct	_T_SERIALNUMBER_RANGE
{
	SERIALNUMBER	iMinNumber;
	SERIALNUMBER	iMaxNumber;

} T_SERIALNUMBER_RANGE, * PT_SERIALNUMBER_RANGE;


class	CManagedSerialNumber : public CSerialNumber
{
public:

	// constructions
	CManagedSerialNumber()
	{
		Clear();
	}
	~CManagedSerialNumber()
	{
		Clear();
	}

	bool	Init( CSerialNumberRequest * pRequest )
	{
		FLASSERT( pRequest != NULL );
		if( pRequest != NULL )
		{
			m_pRequest = pRequest;

			//SendAllocSerialNumber();
			//SendAllocSerialNumber();
			return true;
		}

		return false;
	}

	bool	Add( SERIALNUMBER iStart, SERIALNUMBER iEnd )
	{
		T_SERIALNUMBER_RANGE tNumbers;
		tNumbers.iMinNumber = iStart;
		tNumbers.iMaxNumber = iEnd;

		if( AddSerialNumberPool( &tNumbers ) == false )
		{
			return false;
		}

		return true;
	}

	bool	IsInit()
	{
		CMclAutoLock Lock( m_cLock );

		if( m_pCurrentNumbers == NULL )
		{
			return false;
		}

		if( m_tNumberPool[0].iMinNumber == 0 || m_tNumberPool[0].iMaxNumber == 0 )
		{
			return false;
		}

		if( m_tNumberPool[1].iMinNumber == 0 || m_tNumberPool[1].iMaxNumber == 0 )
		{
			return false;
		}

		return true;
	}

	bool	AddSerialNumberPool( PT_SERIALNUMBER_RANGE pNumbers )
	{
#pragma warning ( push )
#pragma warning ( disable : 6011 )

		FLASSERT( pNumbers != NULL );
		FLASSERT( pNumbers->iMinNumber > 0 );
		FLASSERT( pNumbers->iMaxNumber > 0 );
		FLASSERT( pNumbers->iMaxNumber > pNumbers->iMinNumber );

#pragma warning ( pop )

		if( pNumbers == NULL ||
			pNumbers->iMinNumber <= 0 ||
			pNumbers->iMaxNumber <= 0 ||
			pNumbers->iMaxNumber <= pNumbers->iMinNumber )
		{
			return false;
		}

		PT_SERIALNUMBER_RANGE pSelected = NULL;

		CMclAutoLock Lock( m_cLock );

		if( m_tNumberPool[0].iMinNumber == 0 && m_tNumberPool[0].iMaxNumber == 0 )
		{
			pSelected = &m_tNumberPool[0];
		}
		else if( m_tNumberPool[1].iMinNumber == 0 && m_tNumberPool[1].iMaxNumber == 0 )
		{
			pSelected = &m_tNumberPool[1];
		}

		if( pSelected != NULL )
		{
			*pSelected = *pNumbers;
			if( m_pCurrentNumbers == NULL )
			{
				if( SetCurrentNumbers( pSelected ) == false )
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}

	// operations
	SERIALNUMBER	Get()
	{
		CMclAutoLock Lock( m_cLock );

		FLASSERT( m_pCurrentNumbers != NULL );
		if( m_pCurrentNumbers == NULL )
		{
			return 0;
		}

		//[m_pCurrentNumbers->dwMinNumber, m_pCurrentNumbers->dwMaxNumber)
		if( m_iCurrentNumber >= m_pCurrentNumbers->iMaxNumber )
		{
			if( SelectExtraNumber() == false )
			{
				return 0;
			}
		}

		SERIALNUMBER iSerialNumber = m_iCurrentNumber;
		++m_iCurrentNumber;
		return iSerialNumber;
	}

	bool	IsManaged()
	{
		return true;
	}
	void	InsertSerialNumber( SERIALNUMBER iSerialNumber )
	{
		// Shop 아이템은 무시
		if( iSerialNumber == CSerialNumber::SN_IGNORE_SERIAL_NUMBER )
		{
			return;
		}

		CMclAutoLock	Lock( m_csManagedSerialNumber );

		std::pair< std::map< SERIALNUMBER, u_long >::iterator, bool > ret = m_mapManagedSerialNumber.insert( std::map< SERIALNUMBER, u_long >::value_type( iSerialNumber, 0 ) );
		if( ret.second == false )
		{
			std::map< SERIALNUMBER, u_long >::iterator pos = m_mapManagedSerialNumber.find( iSerialNumber );
			if( pos != m_mapManagedSerialNumber.end() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Item 중복: SerialNumber: %d, SrcID: %d, DestID: %d" ), iSerialNumber, pos->second, 0 );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Item Insert failed: SerialNumber: %d, ID: %d" ), iSerialNumber, 0 );
			}
		}
	}
	void	RemoveSerialNumber( SERIALNUMBER iSerialNumber )
	{
		// Shop 아이템은 무시
		if( iSerialNumber == CSerialNumber::SN_IGNORE_SERIAL_NUMBER )
		{
			return;
		}

		CMclAutoLock	Lock( m_csManagedSerialNumber );

		std::map< SERIALNUMBER, u_long >::iterator pos = m_mapManagedSerialNumber.find( iSerialNumber );
		if( pos != m_mapManagedSerialNumber.end() )
		{
			m_mapManagedSerialNumber.erase( pos );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "존재하지 않는 Item 시리얼 넘버 삭제 요청: SerialNumber: %d" ), iSerialNumber );
		}
	}

private:

	bool	SelectExtraNumber()
	{
		FLASSERT( m_pCurrentNumbers != NULL );
		if( m_pCurrentNumbers != NULL )
		{
			m_pCurrentNumbers->iMinNumber = 0;
			m_pCurrentNumbers->iMaxNumber = 0;

			if( SetCurrentNumbers( (m_pCurrentNumbers == &m_tNumberPool[0]) ? &m_tNumberPool[1] : &m_tNumberPool[0] ) == false )
			{
				return false;
			}

			SendAllocSerialNumber();
			return true;
		}

		return false;
	}

	bool	SetCurrentNumbers( PT_SERIALNUMBER_RANGE pNumbers )
	{
#pragma warning ( push )
#pragma warning ( disable : 6011 )

		FLASSERT( pNumbers != NULL );
		FLASSERT( pNumbers->iMinNumber > 0 );
		FLASSERT( pNumbers->iMaxNumber > 0 );
		FLASSERT( pNumbers->iMaxNumber > pNumbers->iMinNumber );

#pragma warning ( pop )

		if( pNumbers == NULL ||
			pNumbers->iMinNumber <= 0 ||
			pNumbers->iMaxNumber <= 0 ||
			pNumbers->iMaxNumber <= pNumbers->iMinNumber )
		{
			return false;
		}

		m_pCurrentNumbers = pNumbers;
		m_iCurrentNumber = m_pCurrentNumbers->iMinNumber;
		return true;
	}

	void	SendAllocSerialNumber()
	{
		FLASSERT( m_pRequest != NULL );

		if( m_pRequest != NULL )
		{
			m_pRequest->Request();
		}
	}

	void	Clear()
	{
		m_pRequest = NULL;

		m_iCurrentNumber = 0;
		m_pCurrentNumbers = NULL;

		memset( m_tNumberPool, 0, sizeof( m_tNumberPool ) );
	}

private:

	CMclCritSec				m_cLock;

	CSerialNumberRequest *	m_pRequest;

	SERIALNUMBER			m_iCurrentNumber;
	PT_SERIALNUMBER_RANGE	m_pCurrentNumbers;

	T_SERIALNUMBER_RANGE	m_tNumberPool[2];

	//////////////////////////////////////////////////////////////////////////
	CMclCritSec							m_csManagedSerialNumber;
	std::map< SERIALNUMBER, u_long >	m_mapManagedSerialNumber;
	//////////////////////////////////////////////////////////////////////////

};
//////////////////////////////////////////////////////////////////////////

//#define	__MANAGED_SERIALNUMBER



CSerialNumber*	CSerialNumber::GetInstance()
{
#ifdef	__MANAGED_SERIALNUMBER
	static	CManagedSerialNumber	sSerialNumber;
#else
	static	CRandomSerialNumber		sSerialNumber;
#endif

	return &sSerialNumber;
}
