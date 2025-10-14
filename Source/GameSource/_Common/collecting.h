#ifndef __COLLECTING_H__
#define	__COLLECTING_H__

typedef	struct	_COLLECTING_ITEM
{
	DWORD	dwItemId;
	DWORD	dwProb;
	_COLLECTING_ITEM()
	{
		dwItemId	= 0;
		dwProb	= 0;
	}
}	COLLECTING_ITEM;

class CCollectingProperty
{
public:
	CCollectingProperty();
	virtual	~CCollectingProperty();
	static	CCollectingProperty*	GetInstance();
	BOOL	LoadScript( LPCTSTR szFile );

	int		GetCool( int nAbilityOption );
	int		GetMaxBattery( void )	{	return m_nMaxBattery;	}
	DWORD	GetItem( void );
#ifdef BATTERY_PREMIUM
	DWORD GetPremiumItem( void );
#endif
	int		GetEnchantProbability( int nAbilityOption );
	int		GetMaxCollectorLevel( void )	{	return m_anEnchant.size();	}

#ifdef __WORLDSERVER
	std::vector<int>*	GetEnchantProbabilityPtr( void )	{	return &m_anEnchant;		}
	std::vector<COLLECTING_ITEM>*	GetItemPtr( void )	{	return &m_aItem;	}

#ifdef BATTERY_PREMIUM
	std::vector<COLLECTING_ITEM>*	GetPremiumItemPtr( void )	{	return &m_aPremiumItem;	}
#endif
	
#endif	// __WORLDSERVER

private:
	std::vector<int>	m_anCool;
	std::vector<int>	m_anEnchant;
	std::vector<COLLECTING_ITEM>	m_aItem;
#ifdef BATTERY_PREMIUM
	std::vector<COLLECTING_ITEM>	m_aPremiumItem;
#endif
	int		m_nMaxBattery;
};
#endif	// __COLLECTING_H__