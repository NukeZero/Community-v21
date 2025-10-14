
#include "FLDOS.h"

CDos2::CDos2()
{
	m_pPool		= new FLMemPooler< CONNECTIONREQUEST >( 128 );
	m_nAddr		= 0;
	m_dwStartTick = 0;
}

CDos2::~CDos2()
{
	for( std::map< u_long, PCONNECTIONREQUEST >::iterator i = begin(); i != end(); ++i )
	{
		m_pPool->Free( i->second );
	}

	clear();
	SAFE_DELETE( m_pPool );
}

PCONNECTIONREQUEST	CDos2::Get( u_long addr )
{
	std::map< u_long, PCONNECTIONREQUEST >::iterator i	= find( addr );
	if( i != end() )
	{
		return i->second;
	}

	return NULL;
}

void	CDos2::Add( u_long addr )
{
	PCONNECTIONREQUEST ptrtmp	= (PCONNECTIONREQUEST)m_pPool->Alloc();	// add
	memset( ptrtmp, 0, sizeof(*ptrtmp ) );
	ptrtmp->ip		= addr;
	ptrtmp->dwHit	= GetTickCount();
	insert( std::map< u_long, PCONNECTIONREQUEST >::value_type( addr, ptrtmp ) );
}

int		CDos2::IsOver( u_long addr )
{
	Release();

	PCONNECTIONREQUEST ptr	= Get( addr );
	if( ptr )
	{
		DWORD dwHit	= GetTickCount();
		if( dwHit - ptr->dwHit < 200 )
		{
			ptr->nHit++;
		}
		else
		{
			ptr->dwHit	= dwHit;
		}

		if( ptr->nHit < MAX_HIT )
		{
			return 0;
		}
		else
		{
			if( !ptr->b )
			{
				if( m_nAddr < MAX_ADDR )
				{
					m_adwAddr[m_nAddr++]	= ptr->ip;
					ptr->b	= TRUE;
					return -1;
				}

				return 1;
			}
			else	// > 0
			{
				return 1;
			}
		}
	}
	else
	{
		Add( addr );
		return 0;
	}
}

void CDos2::Release()
{
	DWORD dwNowTick = ::GetTickCount();
	if( (dwNowTick-m_dwStartTick) >= DOS_TIMEOUT )
	{
		m_dwStartTick = dwNowTick;
		for( int i = 0; i < m_nAddr; i++ )
		{
			PCONNECTIONREQUEST ptrtmp	= Get( m_adwAddr[i] );
			if( ptrtmp )
			{
				ptrtmp->b		= FALSE;
				ptrtmp->nHit	= 0;
			}
		}

		m_nAddr	= 0;
	}
}
