
#ifndef __FLCOMMONFUNCTION_H__
#define __FLCOMMONFUNCTION_H__


class FLCommonFunction
{
	public:
		FLCommonFunction();
		~FLCommonFunction();

		static int		GetGeneralSkillPoint( const DWORD dwJob, const DWORD dwLevel );
		static int		GetTotalSkillPoint( const DWORD dwJob, const DWORD dwLevel );
		static DWORD	GetSubLineJob( const DWORD dwJob );
		static DWORD	GetJobSkillCount( const DWORD dwJob );
		static bool		IsMasterJob( const DWORD dwJob );
//		static int		GetTotalStatPoint( const DWORD _Job, const DWORD _Level );
};

#endif // __FLCOMMONFUNCTION_H__
