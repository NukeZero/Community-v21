
#ifndef __FLPIERCINGOPTION_H__
#define __FLPIERCINGOPTION_H__


class FLPiercingOption
{
	public:
		FLPiercingOption();
		FLPiercingOption( const FLPiercingOption & rhs);
		virtual	~FLPiercingOption();

		FLPiercingOption &	operator = ( const FLPiercingOption & kPiercing );

		void			Clear();

		void			InitializeCombinedPiercingItem();

		bool			IsSetGeneralPiercingOption();
		bool			IsSetUltimatePiercingOption();
		bool			IsExistGeneralPiercedItemID( const DWORD dwItemID );
		bool			IsSetCombinedGeneralPiercingOption();

		const size_t	GetGeneralPiercingSize() const;
		const size_t	GetUltimatePiercingSize() const;
		const size_t	GetVisPetPiercingSize() const;
		const size_t	GetCombinedGeneralPiercingSize() const;

		const DWORD		GetGeneralPiercingItemID( const size_t Nth ) const;
		const DWORD		GetUltimatePiercingItemID( const size_t Nth ) const;
		const time_t	GetVisPetPiercingItemKeepTime( const size_t Nth ) const;
		const DWORD		GetCombinedGeneralPiercingItemID( const size_t Nth ) const;

		void			SetGeneralPiercingSize( const size_t nSize );
		void			SetUltimatePiercingSize( const size_t nSize );
		void			SetVisPetPiercingSize( const size_t nSize );
		void			SetCombinedGeneralPiercingSize( const size_t nSize );

		void			SetGeneralPiercingItemID( const size_t Nth, const DWORD dwItemID );
		void			SetUltimatePiercingItemID( const size_t Nth, const DWORD dwItemID );
		void			SetVisPetPiercingItemKeepTime( const size_t Nth, const time_t tKeepTime );
		void			SetCombinedGeneralPiercingItemID( const size_t Nth, const DWORD dwItemID );
		
		void			SwapVisItem( const size_t nSrc, const size_t nDest );

		void			SerializePiercingOption( CAr & ar ) const;
		void			SerializeCombinedPiercingOption( CAr & ar ) const;
		void			DeserializePiercingOption( CAr & ar );
		void			DeserializeCombinedPiercingOption( CAr & ar );

		void			SerializeAllPiercingOption( CAr & ar ) const;
		void			DeserializeAllPiercingOption( CAr & ar );


	private:
		std::vector< DWORD /*ItemID*/ >		m_vecGeneralPiercing;
		std::vector< DWORD /*ItemID*/>		m_vecUltimatePiercing;
		std::vector< time_t /*KeepTime*/>	m_vecVisPetPiercing;
		std::vector< DWORD /*ItemID*/>		m_vecCombinedGeneralPiercing;
		//////////////////////////////////////////////////////////////////////////
};
#endif // __FLPIERCINGOPTION_H__
