#ifndef __DEBUG_H
#define __DEBUG_H

LPCTSTR Message( LPCTSTR str, ... );

extern void DEBUGOUT(LPCTSTR lpszFormat, ...);
#ifdef __IDC
 extern void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName = "..\\error.txt" );
#else
 extern void DEBUGOUT2( LPCTSTR lpszStr, LPCTSTR szFileName = "error.txt" );
#endif
extern void FILEOUT (LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...);
extern void WriteLog( LPCTSTR lpszFormat, ... );
extern void WriteError( LPCTSTR lpszFormat, ... );

void ConnectLog( LPCTSTR szLog );

#ifdef _CLIENT
extern void mprintf(LPCTSTR lpszFormat, ...);
#endif

extern CString		GetTimeString( void );

#define SELECT_TRACE DEBUGOUT


#endif