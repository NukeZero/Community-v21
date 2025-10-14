#ifndef __FLCOMMONCHECK_H__
#define __FLCOMMONCHECK_H__

class FLCommonCheck
{
	public:
		FLCommonCheck();
		~FLCommonCheck();

		static FLCommonCheck*	GetInstance();

		bool	CheckTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1 );
		bool	CheckTime( const SYSTEMTIME & kTime, int nDST = -1 );
		bool	CheckTime( const FILETIME & kTime, int nDST = -1 );
		
};

#define		g_pCommonCheck		FLCommonCheck::GetInstance()

#endif // __FLCOMMONCHECK_H__
