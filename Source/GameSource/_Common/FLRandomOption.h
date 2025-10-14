
#ifndef __FLRANDOMOPTION_H__
#define __FLRANDOMOPTION_H__


#define _AWAKE_OLD_VALUE	1
#define _AWAKE_NEW_VALUE	2


typedef struct _T_RANDOMOPTION_EXT
{
	mutable BYTE	byIndex;
	WORD			wDstID;
	short			shAdjValue;

	_T_RANDOMOPTION_EXT() : byIndex( 0 ), wDstID( 0 ), shAdjValue( 0 )	{}
	_T_RANDOMOPTION_EXT( const WORD DST, const short ADJ ) : byIndex( 0 ), wDstID( DST ), shAdjValue( ADJ )	{}

} T_RANDOMOPTION_EXT, *PT_RANDOMOPTION_EXT;

enum RANDOMOPTION_FLAG
{
	RO_FLAG_SAFEGUARD		= 0x01
};

union DB_RANDOMOPTION_DST
{
	struct	{	int lower_DST : 16;	int	upper_DST : 16;	};
	int	random_option_DST;

	DB_RANDOMOPTION_DST() : lower_DST( 0 ), upper_DST( 0 ), random_option_DST( 0 )	{}
	DB_RANDOMOPTION_DST( const int _DST ) : random_option_DST( _DST )	{}
	DB_RANDOMOPTION_DST( const int _lower_DST, const int _upper_DST ) : lower_DST( _lower_DST ), upper_DST( _upper_DST )	{}
};

union DB_RANDOMOPTION_ADJ
{
	struct	{	int	lower_ADJ : 16;	int	upper_ADJ : 16;	};
	int random_option_ADJ;

	DB_RANDOMOPTION_ADJ() : lower_ADJ( 0 ), upper_ADJ( 0 ), random_option_ADJ( 0 )	{}
	DB_RANDOMOPTION_ADJ( const int _ADJ ) : random_option_ADJ( _ADJ )	{}
	DB_RANDOMOPTION_ADJ( const int _lower_ADJ, const int _upper_ADJ ) : lower_ADJ( _lower_ADJ ), upper_ADJ( _upper_ADJ )	{}
};

typedef		std::vector< T_RANDOMOPTION_EXT >			RandomOptionExtensionVec;
typedef		RandomOptionExtensionVec::iterator			RandomOptionExtensionItr;
typedef		RandomOptionExtensionVec::const_iterator	RandomOptionExtensionCItr;


const int	MAX_RANDOMOPTION_DB_COLUMN				= 4;
const int	MAX_RANDOMOPTION_MERGE_SIZE				= 4;
const int	MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE	= MAX_RANDOMOPTION_DB_COLUMN * 2;
const int	MAX_RANDOMOPTION_RETRY_SIZE				= 6;

class FLRandomOption
{
	public:
		FLRandomOption();
		FLRandomOption( const FLRandomOption & rhs );
		~FLRandomOption();

		FLRandomOption &	operator = ( const FLRandomOption & kRandomOptionExtension );

		void		Clear();

		void		InitializeRandomOptionExtension();
		void		InitializeNewRandomOptionExtension();
		void		InitializeCombinedRandomOptionExtension();

		WORD		GetRandomOptionExtensionFlag() const;
		void		SetRandomOptionExtensionFlag( const WORD wRandomOptionExtensionFlag );

		bool		IsSetSafeGuardRandomOptionExtensionFlag() const;
		void		SetSafeGuardRandomOptionExtensionFlag();
		void		ResetSafeGuardRandomOptionExtensionFlag();

		bool		IsSetRandomOptionExtension() const;
		bool		IsSetNewRandomOptionExtension() const;
		bool		IsSetCombinedRandomOptionExtension() const;

		size_t		GetRandomOptionExtensionSize() const;
		size_t		GetNewRandomOptionExtensionSize() const;
		size_t		GetCombinedRandomOptionExtensionSize() const;

		WORD		GetRandomOptionExtensionDstID( const size_t Nth ) const;
		WORD		GetNewRandomOptionExtensionDstID( const size_t Nth ) const;
		WORD		GetCombinedRandomOptionExtensionDstID( const size_t Nth ) const;

		short		GetRandomOptionExtensionAdjValue( const size_t Nth ) const;
		short		GetNewRandomOptionExtensionAdjValue( const size_t Nth ) const;
		short		GetCombinedRandomOptionExtensionAdjValue( const size_t Nth ) const;

		bool		SetRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension );
		bool		SetNewRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension );
		bool		SetCombinedRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension );

		bool		OverwriteNewRandomOptionExtension();

		void		SerializeRandomOptionExtension( CAr & ar ) const;
		void		SerializeNewRandomOptionExtension( CAr & ar ) const;
		void		SerializeCombinedRandomOptionExtension( CAr & ar ) const;

		void		DeserializeRandomOptionExtension( CAr & ar );
		void		DeserializeNewRandomOptionExtension( CAr & ar );
		void		DeserializeCombinedRandomOptionExtension( CAr & ar );

		void		SerializeAllRandomOptionExtension( CAr & ar ) const;
		void		DeserializeAllRandomOptionExtension( CAr & ar );


	private:
		RandomOptionExtensionVec		m_vecRandomOptionExtension;
		RandomOptionExtensionVec		m_vecNewRandomOptionExtension;			// 각성 보호 사용 후의 각성
		RandomOptionExtensionVec		m_vecCombinedRandomOptionExtension;		// 합성 아이템의 각성

		WORD	m_wRandomOptionExtensionFlag;
};


#endif // __FLRANDOMOPTION_H__

