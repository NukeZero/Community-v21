
#ifndef	__FLCOMMONTYPES_H__
#define	__FLCOMMONTYPES_H__


#include <string>
#include <ctime>


#if	defined(UNICODE) || defined(_UNICODE)

#ifndef	tstring
typedef	std::wstring	tstring;
#endif

#else
#ifndef	tstring
typedef	std::string		tstring;
#endif

#endif


#ifndef	__time32_t
	typedef	long	__time32_t;
#endif



#endif