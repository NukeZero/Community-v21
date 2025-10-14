#pragma once

const int		DEFAULT_ORIGIN_STAT_VALUE	= 15;

class Stat
{
public:
	enum	{ StatDice_None = 0, StatDice_Set, StatDice_Reset	};
public:
	Stat();
	virtual ~Stat();
	Stat&	operator=( const Stat& rhs );
	void	Serialize( CAr & ar );

	void	InitializeOriginStat();
	void	InitializeDiceStat();
	
	int		GetStr()					{	return GetOriginStr() + GetDiceStr();	}
	int		GetSta()					{	return GetOriginSta() + GetDiceSta();	}
	int		GetDex()					{	return GetOriginDex() + GetDiceDex();	}
	int		GetInt()					{	return GetOriginInt() + GetDiceInt();	}

	void	SetOriginStr( int _str )	{	SetOriginStat( Str, _str );	}
	void	SetOriginSta( int _sta )	{	SetOriginStat( Sta, _sta );	}
	void	SetOriginDex( int _dex )	{	SetOriginStat( Dex, _dex );	}
	void	SetOriginInt( int _int )	{	SetOriginStat( Int, _int );	}
	int		GetOriginStr()				{	return GetOriginStat( Str );	}
	int		GetOriginSta()				{	return GetOriginStat( Sta );	}
	int		GetOriginDex()				{	return GetOriginStat( Dex );	}
	int		GetOriginInt()				{	return GetOriginStat( Int );	}

#ifdef __DBSERVER	// DB 로딩용
	void	SetDiceStr( int _str )		{	SetDiceStat( Str, _str );	}
	void 	SetDiceSta( int _sta )		{	SetDiceStat( Sta, _sta );	}
	void	SetDiceDex( int _dex )		{	SetDiceStat( Dex, _dex );	}
	void	SetDiceInt( int _int )		{	SetDiceStat( Int, _int );	}
#endif // __DBSERVER
	
	int		GetDiceStr()				{	return GetDiceStat( Str );	}
	int		GetDiceSta()				{	return GetDiceStat( Sta );	}
	int		GetDiceDex()				{	return GetDiceStat( Dex );	}
	int		GetDiceInt()				{	return GetDiceStat( Int );	}

	bool	SetRemainStatPoint( const int _RemainStatPoint );

	long	GetRemainStatPoint() const		{	return m_RemainStatPoint;	}

private:
	enum	StatType	{	LoopBaseStatType = 0, Str = 0, Sta, Dex, Int, MaxStatType };
	
	int*	GetOriginStatPtr( StatType st );
	int		GetOriginStat( StatType st );
	void	SetOriginStat( StatType st, int value );
	
	int*	GetDiceStatPtr( StatType st );
	int		GetDiceStat( StatType st );
	void	SetDiceStat( StatType st, int value );

#ifdef __WORLDSERVER
// Stat Dice 아이템 사용 관련..
public:
	void	ApplyRandomDiceStats( int job );
	bool	ResetDiceStat();
private:
	int		GetVirtualJob( int job );
	BYTE	gettable( int job );
	bool	IsStatUp( int job, StatType st, const BYTE table );
#endif // __WORLDSERVER

private:
	// Origin Stat
	int		m_OriginStr;
	int		m_OriginSta;
	int		m_OriginDex;
	int		m_OriginInt;
	
	// Dice Stat
	int 	m_DiceStr = 0;
	int		m_DiceSta = 0;
	int		m_DiceDex = 0;
	int		m_DiceInt = 0;

	long	m_RemainStatPoint;
};