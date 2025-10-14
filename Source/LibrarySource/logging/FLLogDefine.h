

#ifndef	__LOG_DEFINE_H__
#define	__LOG_DEFINE_H__



#ifndef	__FUNCTION__
#define	__FUNCTION__	"Unknown"
#endif


#ifdef	_UNICODE

#ifndef	WIDEN2
#define WIDEN2( x )		L ##x
#endif

#ifndef	WIDEN
#define WIDEN( x )		WIDEN2( x )
#endif

#else

#ifndef	WIDEN
#define	WIDEN( x )		x
#endif

#endif

#ifndef	__TFILE__
#define __TFILE__		WIDEN( __FILE__ )
#endif

#ifndef	__TFUNCTION__
#define __TFUNCTION__	WIDEN( __FUNCTION__ )
#endif



#define	MAX_LOG_STRING			1024

enum
{
	LOG_DEBUG,
	LOG_TRACE,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FAULT,
	LOG_UNKNOWN,
};



#endif
