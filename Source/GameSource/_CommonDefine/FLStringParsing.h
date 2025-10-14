#ifndef __FLSTRINGPARSING_H__
#define __FLSTRINGPARSING_H__


class FLSkillStringParsing
{
public:
	FLSkillStringParsing();
    ~FLSkillStringParsing();

public:
	typedef struct _T_SKILL_DATA
	{
		WORD	wType;
		WORD	wId;
		DWORD	dwLevel;
		DWORD	dwTotal;
		int		nAdjValue;

	} T_SKILL_DATA, *PT_SKILL_DATA;

	static FLSkillStringParsing* GetInstance();

	bool	GetDataFromString( TCHAR* pszSkillString, std::vector<T_SKILL_DATA> & vecSkillData );

	int		GetIntFromStr( TCHAR* pBuf, int* pLocation );
	__int64	GetInt64FromStr( TCHAR* pBuf, int* pLocation );
};

#define g_xSkillStringParsing	FLSkillStringParsing::GetInstance()

#endif // __FLSTRINGPARSING_H__