#pragma once







#define RELEASE_BREAK			do{	*( ( int * )NULL ) = 0;  }while(0)
#define RELEASE_ASSERT( x )		do{ if( x == false ) RELEASE_BREAK; }while(0)

#ifdef _DEBUG			//디버그 모드에서만 작동하는 코드 
#define DEBUG_EXCUTE( exc )			do{		exc;		}while(0)
#define DEBUG_ASSERT( x )		do{ if( x == false ) RELEASE_BREAK; }while(0)
#define DEBUG_BREAK				RELEASE_BREAK
#else
#define DEBUG_EXCUTE( exc )			__noop
#define DEBUG_ASSERT( x )			__noop
#define DEBUG_BREAK					__noop
#endif