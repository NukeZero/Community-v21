#ifndef __DEBUG_H
#define __DEBUG_H

extern void DEBUGOUT(LPCTSTR lpszFormat, ...);
extern void DEBUGOUT2( LPCTSTR lpszStr );
extern void FILEOUT (LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...);
extern void WriteLog( LPCTSTR lpszFormat, ... );
extern void WriteError( LPCTSTR lpszFormat, ... );

#ifdef _CLIENT
extern void mprintf(LPCTSTR lpszFormat, ...);
#endif

extern CString		GetTimeString( void );

#define SELECT_TRACE DEBUGOUT

#endif