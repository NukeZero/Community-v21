#pragma once 

class FLConsignmentSale
{
public:
	struct T_USER_DATA
	{
		T_USER_DATA();

		BOOL				bRegisting;				//연속 등록 방지
		DWORD				dwLastQueryTick;
	};

	static DWORD				GetExtendRegistCountMax( FLWSUser & kUser );
	static int					GetTax( FLWSUser* kUser, const int nTotPrice );

	static BOOL					IsQueryableTime_TryUpdateTime( T_USER_DATA & kUserData );
};



class FLConsignmentSale_Spec
{
public: 
	enum						{		TAX_RATE_MAX = 10000,				};

	struct T_STOCK
	{
		T_STOCK();
		int						nSaleHour;
		int						nDefaultRegistLimit;
		int						nTaxRate;					//만분율
		int						nQueryIntervalTick;
	};

public:
	FLConsignmentSale_Spec();
	virtual ~FLConsignmentSale_Spec();
	static FLConsignmentSale_Spec &		GetInstance();

public:
	void					Load();
	const T_STOCK &			m() const;

	BOOL					IsVendible( const DWORD dwWorldID ) const;

private:
	void					loadConfig();
	void					loadVendibleWorldList();

private:
	typedef					std::map< DWORD/*WorldID*/, DWORD/*WorldID*/ > WorldIDMap;

	T_STOCK					m_kStock;

	WorldIDMap				m_mapVendibleWorldID;
};

inline static	FLConsignmentSale_Spec & CONSIGNMENT_SALE_SPEC()
{
	return					FLConsignmentSale_Spec::GetInstance();
}



//----------------------------------------------------------------------------------------------------------------------------------//



template < class _Type >
class FLAutoAssigner
{
public:
	FLAutoAssigner( _Type & o_variable, const _Type & const_value ) : m_bActive( true ), m_variable( o_variable ), m_value( const_value )	{}
	~FLAutoAssigner()
	{
		if( m_bActive == true )
			m_variable			= m_value;	
	}

	void					Disable() { m_bActive	= false; }

private:
	bool					m_bActive;
	_Type &					m_variable;
	const _Type				m_value;
};


//struct FLAutoFalse
//{
//	FLAutoFalse( BOOL & bFlag ) : m_bActive( true )	{}
//	~FLAutoFalse()	
//	{		
//		if( m_bActive == true )
//			m_bFlag				= FALSE;	
//	}
//
//	void					Disable() { m_bActive	= false; }
//
//private:
//	bool					m_bActive;
//	BOOL &					m_bFlag;
//};
