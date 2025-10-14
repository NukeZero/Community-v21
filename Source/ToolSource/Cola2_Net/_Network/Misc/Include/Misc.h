#ifndef __MISC_H__
#define	__MISC_H__

typedef	struct	tagSERVER_DESC
{
#ifdef __MSVR
	DWORD	dwParent;
	long	lCount;
#endif	// __MSVR
	DWORD	dwID;
	char	lpName[36];
	char	lpAddr[16];
	BOOL	b18;
	long	lEnable;
	long	lMax;
	tagSERVER_DESC()
		{
#ifdef __MSVR
			dwParent	= NULL_ID;
			lCount	= 0;
#endif	// __MSVR
			dwID	= NULL_ID;
			*lpName		= '\0';
			*lpAddr		= '\0';
#ifdef __SCHOOL0701
			b18	= 0x00;
#else	// __SCHOOL0701
			b18	= FALSE;
#endif	// __SCHOOL0701
			lEnable		= 0;
			lMax	= 0;
		}
}
SERVER_DESC, *LPSERVER_DESC;

// 쪽지 기능관련 스트럭쳐 
struct TAG_ENTRY
{
	u_long		idFrom;				// 누가 
	DWORD		dwDate;				// 언제 
	char		szString[256];		// 쪽지 내용 
};

const int MAX_TAGS = 20;			// 최대 쪽지 저장치 

// 길드 스탯 enum
enum GUILD_STAT
{
	GUILD_STAT_LOGO,
	GUILD_STAT_PXPCOUNT,
	GUILD_STAT_PENYA,
	GUILD_STAT_NOTICE,
};

const int WANTED_MSG_MAX = 20;

// 포상금 항목 
struct WANTED_ENTRY
{
	char		szPlayer[64];						// 현상범 이름 
	long		nEnd;								// 마감일 
	__int64		nGold;								// 현상금액 
	char		szMsg[WANTED_MSG_MAX + 1];			// 십자평 
};


#endif	// __MISC_H__