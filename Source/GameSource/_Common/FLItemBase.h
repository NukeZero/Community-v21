
#ifndef __FLITEMBASE_H__
#define __FLITEMBASE_H__


class FLItemBase
{
	public:
		FLItemBase();
		FLItemBase( const FLItemBase & rhs );
		virtual ~FLItemBase()	{}
		
		virtual	FLItemBase &	operator = ( const FLItemBase & rhs );
		
		
		virtual void	Serialize( CAr & ar ) const;
		virtual void	Deserialize( CAr & ar );
		
		BOOL			IsEmpty() const { return m_dwItemId ? FALSE : TRUE; }
		BOOL			IsQuest() const;

		PT_ITEM_SPEC	GetProp();

		virtual	void	Empty();

		int				GetCost() const;
		void			SetExtra( int nExtra );
		int				GetExtra() const;

		DWORD			GetChipCost();

		SERIALNUMBER	GetSerialNumber( void ) const		{	return m_iSerialNumber;	}
		void			SetSerialNumber( void );
		void			SetSerialNumber( const SERIALNUMBER iSerialNumber );

		void			SetTexture();

#ifdef __CLIENT
		void			SetTexture( CTexture* pTexture );
		CTexture*		GetTexture();
#endif // __CLIENT


	private:
		void			Init();


	public:
		OBJID			m_dwObjId;		// 아이템 콘테이너 안에서의 ID (아이템의 ID로 사용)
		DWORD			m_dwObjIndex;	// 아이템 콘테이너 안에서의 Index
		DWORD			m_dwItemId;		// 아이템 식별 인덱스( Property에서 사용 )
		int 			m_nExtra;		// 거래시 물품개수 or 개인상점에 등록한 갯수 
		TCHAR			m_szItemText[ 32 ];
		int				m_nCost;

#ifdef __CLIENT
		CTexture*		m_pTexture;
		BOOL			m_bControlForbidden;
#endif // __CLIENT

	private:
		SERIALNUMBER	m_iSerialNumber;
};

extern BOOL IsUsableItem( FLItemBase* pItem );

extern BOOL IsUsingItem( FLItemBase* pItem );

#endif // __FLITEMBASE_H__
