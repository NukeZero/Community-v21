#include "stdafx.h"

#if defined(__DBSERVER)// || defined(__CORESERVER)
#include "..\_Network\Objects\Obj.h"
#else
#include "Mover.h"
#endif


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void CCtrl::Serialize( CAr & ar, int nMethod )	// 33
{
	CObj::Serialize( ar, nMethod );

	if( ar.IsStoring() )
	{
		ar << m_objid;
	}
	else
	{
		ar >> m_objid;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef __CORESERVER

void CItem::Serialize( CAr & ar, int nMethod )	// 45
{
	CCtrl::Serialize( ar, nMethod );		// 33
	
	if( ar.IsStoring() )
	{
		static_cast<FLItemElem*>( m_pItemBase )->Serialize( ar );	// 11
	}
	else
	{
		SAFE_DELETE( m_pItemBase );
		m_pItemBase	= new FLItemElem;
		m_pItemBase->Deserialize( ar );	// 11
	}
}

#endif // __CORESERVER
