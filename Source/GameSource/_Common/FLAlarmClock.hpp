#pragma once

#include "FLAlarmClock.h"

template < class _Key >
FLAlarmClock< _Key >::FLAlarmClock()
{
}

template < class _Key >
FLAlarmClock< _Key >::~FLAlarmClock()
{
}

template < class _Key >
BOOL
FLAlarmClock< _Key >::Regist( const _Key & key, IAlarm * pAlarm )
{
	if( pAlarm == NULL )
		return FALSE;

	m_PriorityQ.Push( key, pAlarm ); 
	return TRUE;
}

template < class _Key >
BOOL
FLAlarmClock< _Key >::Unregist( IAlarm * pAlarm )
{
	if( pAlarm != NULL )
		return ( pAlarm->Unlink() == true ) ? TRUE : FALSE;
	return FALSE;
}

template < class _Key >
void					
FLAlarmClock< _Key >::Update( const _Key & currTime )
{
	for( ;; )
	{
		_Key key;
		IAlarm * pAlarm;
		if( m_PriorityQ.Top( key, pAlarm ) == FALSE )
			break;

		if( currTime < key )
			break;

		m_PriorityQ.PopFront( key, pAlarm );
		pAlarm->OnAlarm();
	}
}








