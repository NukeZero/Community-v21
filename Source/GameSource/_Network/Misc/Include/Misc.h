#ifndef __MISC_H__
#define	__MISC_H__

#ifdef __TRAFIC_1218
#include <network/FLAr.h>
#endif	// __TRAFIC_1218

#include "../../../_CommonDefine/FLTypes.h"


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


// 길드 창고 로그 기능 world,database,neuz
const int GUILDBANKLOGVIEW_MAX = 100;

struct __GUILDBANKLOG_ENTRY
{
	char		szPlayer[42];						// 케릭터 이름 
	long		nDate;								// 날짜 
	long		nItemID;								// 아이템아이디,돈
	long		nItemAbilityOption;								// 아이템아이디,돈
	long		nItemCount;								// 아이템숫자
	__GUILDBANKLOG_ENTRY()
	{
		szPlayer[0]	= '\0';
		nDate = 0;	
		nItemID = 0;	
		nItemAbilityOption = 0;
		nItemCount = 0;
	};
};

//sun, 11 케릭터 봉인 거래 기능 world,database,neuz
const int SEALCHAR_MAX = 3;

struct __SEALCHAR_ENTRY
{
	char		szPlayer[42];						// 케릭터 이름 
	u_long		idPlayer;							//  
	long		nPlayerSlot;								//
	__SEALCHAR_ENTRY()
	{
		szPlayer[0]	= '\0';
		idPlayer = 0;	
		nPlayerSlot = 0;	
	};
};


#ifdef __TRAFIC_1218
class CTraficLog	: private std::map<int, int>
{
private:
	DWORD	m_dwTotalPacket;
	DWORD	m_dwTotalBytes;

public:

//	Constructions
	CTraficLog()	
	{
		m_dwTotalPacket	= m_dwTotalBytes	= 0;
	}
	virtual	~CTraficLog()
	{
		Clear();
	}

//	Operations
	void	AddTotal( DWORD dwBytes )
	{
		m_dwTotalPacket++;
		m_dwTotalBytes	+= dwBytes;
	}
	void	Add( int nHdr )
	{
		iterator	i	= find( nHdr );
		if( i == end() )
			insert( value_type( nHdr, 1 ) );
		else
			i->second++;
	}

	void	Serialize( CAr & ar )
	{
		if( ar.IsStoring() )
		{
			ar << m_dwTotalPacket << m_dwTotalBytes;
			ar << (int)size();
			for( iterator i = begin(); i != end(); ++i )
				ar << i->first << i->second;
		}
		else	// load
		{
			Clear();

			ar >> m_dwTotalPacket >> m_dwTotalBytes;
			int nSize;
			ar >> nSize;
			int nHdr;
			int nCount;
			for( int i = 0; i < nSize; i++ )
			{
				ar >> nHdr >> nCount;
				insert( value_type( nHdr, nCount ) );
			}
		}
	}

	void	Clear( void )
	{
		clear();
		m_dwTotalPacket	= m_dwTotalBytes	= 0;
	}

};
#endif	// __TRAFIC_1218

#endif	// __MISC_H__