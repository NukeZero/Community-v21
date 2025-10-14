#ifndef __BUFF_H
#define	__BUFF_H

#include <network/FLAr.h>


#define	RBF_COMMON			(DWORD)0x00000001
#define	RBF_IK3				(DWORD)0x00000002
#define	RBF_GOODSKILL		(DWORD)0x00000004
#define	RBF_CHRSTATE		(DWORD)0x00000008
#define	RBF_DSTPARAM		(DWORD)0x00000010
#define	RBF_DEBUF			(DWORD)0x00000020
#define	RBF_ATTACKER		(DWORD)0x00000040
#define	RBF_UNCONDITIONAL	(DWORD)0x01000000
#define	RBF_ONCE			(DWORD)0x10000000



class	CBuffMgr;
class	IBuff
{
public:
	IBuff();
	virtual	~IBuff();

	////////////////////////////////////////
	WORD	GetType()	{	return m_wType;		}
	WORD	GetId()		{	return m_wId;	}
	DWORD	GetLevel()	{	return m_dwLevel;	}
	DWORD	GetTotal()	{	return m_tmTotal;	}
	DWORD	GetInst()	{	return m_tmInst;	}
	bool	IsPainTime( DWORD dwNowTick );
	DWORD	GetRemain();
	BOOL	GetRemove()		{	return m_bRemove;	}
	BOOL	IsExpired()		{	return m_bExpire;	}
	int		GetAdjValue();

	void	SetType( WORD wType )	{	m_wType	= wType;	}
	void	SetId( WORD wId )		{	m_wId	= wId;	}
	void	SetTotal( DWORD tmTotal );
	void	SetLevel( DWORD dwLevel )	{	m_dwLevel	= dwLevel;	}
	void	SetAttacker( OBJID oiAttacker )		{	m_oiAttacker	= oiAttacker;	}
	void	SetInst( DWORD tmInst )	{	m_tmInst	= tmInst;	}
	void	SetPain( DWORD dwPain, BYTE byBuffTickType, DWORD dwNowTick );
	void	SetRemove()		{	m_bRemove	= TRUE;		}
	void	SetExpired()	{	m_bExpire	= TRUE;		}
	void	SetAdjValue( int nAdjValue );
	bool	IsAbsoluteTimeBuff();

	PT_ITEM_SPEC	GetSpecItem();
	SkillProp*		GetPropSkill();

	virtual	void	Serialize( CAr & ar, CMover* pMover );
	virtual	void	SerializeLevel( CAr & ar );

	virtual	IBuff& operator=( const IBuff & buff );

	////////////////////////////////////////
#ifndef __DBSERVER
public:

	virtual BOOL	Process( CMover* pMover, DWORD tmCurrent );

	virtual	BOOL	IsIk1( DWORD /*dwIk1*/ )	{	return FALSE;	}
	virtual	BOOL	IsIk3( DWORD /*dwIk3 */)	{	return FALSE;	}

	virtual	BOOL	IsAddable( CMover* pMover );

	virtual	void	AddTotal( DWORD tmTotal );
	virtual	void	Release( CBuffMgr* /*pBuffMgr*/ )	{}

	virtual	BOOL	IsRemovable( CMover* /*pMover*/ )	{	return TRUE;	}
	virtual	DWORD	GetDisguise()	{	return NULL_ID;		}

	OBJID	GetAttacker()	{	return m_oiAttacker;	}

	virtual	BOOL	IsCommon()	{	return TRUE;	}
	virtual	BOOL	IsGood( CMover* /*pMover*/ )	{	return FALSE;	}
	virtual	BOOL	HasDstParam( CMover*, DWORD /*dwDstParam*/ )	{	return FALSE;	}
	virtual	BOOL	HasChrState( CMover*, DWORD /*dwChrState*/ )	{	return FALSE;	}
	virtual	BOOL	IsDebuf( CMover* /*pMover*/ )	{	return FALSE;	}
	virtual	BOOL	IsAttacker( CMover*, OBJID /*oiAttacker*/ )	{	return FALSE;	}
	virtual DWORD	GetRemainSec();
	virtual BOOL	CanOverwrite()	{	return TRUE;	}

protected:
	virtual	void	Apply( CMover* /*pMover*/ )		{}
	virtual void	ApplyPain( CMover* /*pMover*/ )	{}
	virtual	BOOL	Timeover( CMover* pMover, DWORD tmCurrent );

private:

	BOOL	Expire( CMover* pMover, DWORD tmCurrent );
#endif	// __DBSERVER

#ifdef __WORLDSERVER
public:

	virtual	void	OnExpire( CMover* /*pMover*/ )	{}
	virtual bool	IsValidBuff( CMover* /*pMover*/ )	{	return true;	}

protected:
	virtual	BOOL	IsValid( CMover* )	{	return TRUE;	}
#endif	// __WORLDSERVER

#ifdef __CLIENT
public:

	BOOL	HasSFX()	{	return m_bSFX;	}
	void	SetSFX()	{	m_bSFX	= TRUE;	}
	virtual	BOOL	IsCommercial()	{	return FALSE;	}

	void    RenderSfx( );

private:

	void	CreateSFX( CMover* pMover );

//__BS_SKILLSFX_TO_LUA
protected:

	virtual void TerminateWorking( );

protected:

	CSfx* m_pSfx;
#endif	// __CLIENT

private:

	WORD	m_wType;
	WORD	m_wId;
	DWORD	m_dwLevel;
	DWORD	m_tmTotal;
	DWORD	m_tmInst;
	DWORD	m_tmPain;
	DWORD	m_tmPainTick;

#ifdef __CLIENT
	BOOL	m_bSFX;
#endif	// __CLIENT

	OBJID	m_oiAttacker;
	BOOL	m_bRemove;

	BOOL	m_bExpire;

	int		m_nAdjValue;
};

#ifndef __DBSERVER
class	IBuffItemBase : public IBuff
{
public:
	IBuffItemBase()		{}
	virtual	~IBuffItemBase()	{}

	virtual void	Release( CBuffMgr * pBuffMgr );
	virtual	BOOL	IsCommon();
	virtual	BOOL	HasChrState( CMover*, DWORD dwChrState );
	virtual	BOOL	IsDebuf( CMover* pMover );
	////////////////////////////////////////
#ifdef __CLIENT
	BOOL	IsCommercial();
#endif	// __CLIENT
};

class CBuffItem : public IBuffItemBase
{
public:
	CBuffItem()		{}
	virtual	~CBuffItem()	{}
	virtual	void	AddTotal( DWORD tmTotal );
	virtual	BOOL	IsRemovable( CMover* pMover );
	virtual	DWORD	GetDisguise();
	virtual	BOOL	IsIk1( DWORD dwIk1 );
	virtual	BOOL	IsIk3( DWORD dwIk3 );

	virtual BOOL	CanOverwrite();

protected:
	virtual	void	Apply( CMover* pMover );
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	virtual bool	IsValidBuff( CMover* pMover );
#endif	// __WORLDSERVER
};

class CBuffSkill : public IBuff
{
public:
	CBuffSkill()	{}
	virtual	~CBuffSkill()	{}
	virtual	BOOL	IsAddable( CMover* pMover );
	virtual void	Release( CBuffMgr * pBuffMgr );
	virtual	BOOL	IsRemovable( CMover* pMover );
	virtual	BOOL	IsGood( CMover* pMover );
	virtual	BOOL	HasDstParam( CMover* pMover, DWORD dwDstParam );
	virtual	BOOL	HasChrState( CMover* pMover, DWORD dwChrState );
	virtual	BOOL	IsDebuf( CMover* pMover );
	virtual	BOOL	IsAttacker( CMover* pMover, OBJID oiAttacker );
protected:
	virtual	void	Apply( CMover* pMover );
	virtual	void	ApplyPain( CMover* pMover );
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	virtual	void	OnExpire( CMover* pMover );
	virtual bool	IsValidBuff( CMover* pMover );

protected:
	virtual	BOOL	IsValid( CMover* pMover );
#endif	// __WORLDSERVER
};

class CBuffPet : public IBuff
{
public:
	CBuffPet()	{}
	virtual	~CBuffPet()	{}
	virtual	BOOL	IsRemovable( CMover* /*pMover*/ )	{	return FALSE;	}
	virtual	BOOL	IsCommon()	{	return FALSE;	}
protected:
	virtual	void	Apply( CMover* pMover );
	virtual void	Release( CBuffMgr * pBuffMgr );
};

// class CBuffItem2 : public IBuffItemBase
// {
// public:
// 	CBuffItem2()	{}
// 	virtual	~CBuffItem2()	{}
// 	virtual	void	SerializeLevel( CAr & ar );
// 	virtual	BOOL	Timeover( CMover* pMover, DWORD tmCurrent );	
// };

class CBuffEquip : public IBuff
{
public:
	CBuffEquip()	{}
	virtual	~CBuffEquip()	{}
	virtual	BOOL	IsRemovable( CMover* /*pMover*/ )	{	return FALSE;	}
	virtual	BOOL	IsCommon()	{	return FALSE;	}
};
#endif	// __DBSERVER

typedef	std::list<IBuff*>			LIST_BUFF;
typedef LIST_BUFF::iterator		BuffItr;

#ifdef __CLIENT
class	CWndWorld;
#endif	// __CLIENT
class CMover;
class CBuffMgr
{
public:
	CBuffMgr();
	virtual	~CBuffMgr();
	void	Clear();
	void	SetMover( CMover* pMover ) { m_pMover = pMover; }
	CMover*	GetMover()	{	return const_cast<CMover*>( m_pMover );	}
	void	Serialize( CAr & ar );
	CBuffMgr&	operator=( CBuffMgr & bm );
	virtual	IBuff*	CreateBuff( WORD wType );
protected:
	virtual	DWORD	GetCurrentTime()	{	return timeGetTime();	}
#ifdef __DBSERVER
public:
	bool	Add( IBuff* pBuff )	{	m_listBuffs.push_back( pBuff );	return true;	}
#else	// __DBSERVER
private:
	bool	Add( IBuff* pBuff )	{	m_listBuffs.push_back( pBuff );	return true;	}
#endif	// __DBSERVER
	////////////////////////////////////////
#ifndef __DBSERVER
public:
	void	Process();
	BOOL	HasBuff( WORD wType, WORD wId );
	void	RemoveBuff( IBuff* pBuff, BOOL bFake );
	void	RemoveBuff( WORD wType, WORD wId, BOOL bFake = TRUE );
	void	RemoveExpiredBuff( WORD wType, WORD wId, BOOL bFake );
	BOOL	HasBuffByIk3( DWORD dwIk3 );
	IBuff*	GetBuffByDestParam( DWORD dwDestParam );

	//////////////////////////////////////////////////////////////////////////
	// mirchang_101101
	bool	HasUseLimitGroupItemBuff( int nUseLimitGroup );
	int		GetCountBuffByItemGroup( int nItemGroup );
	int		GetCountBuffByItemId( DWORD dwId );
	int		GetAdjValueDuplicationBuff( DWORD dwItemKind3 );
	int		GetRemainCountBuff( const DWORD dwItemKind3 );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void	DoApplyHitActiveSkill( CMover* pDest, const DWORD dwProb );
	void	DoApplyDamageActiveSkill( CMover* pDest, const DWORD dwProb );
	//////////////////////////////////////////////////////////////////////////

	BOOL	HasPet()	{	return GetBuffPet() != NULL;	}
	void	RemoveBuffPet();
	IBuff*	GetBuffPet();
	
	IBuff*	GetBuff( WORD wType, WORD wId );
	IBuff*	GetExpiredBuff( WORD wType, WORD wId );
	IBuff*	GetBuffByIk3( DWORD dwIk3 );
	void	RemoveBuffs( DWORD dwFlags, DWORD dwParam = 0 );
	void	ClearInst();
	DWORD	GetDisguise();
	BOOL	AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTotal, DWORD dwPain, BYTE byBuffTickType, OBJID oiAttacker );
protected:
	BOOL	AddBuff( IBuff* pBuff );
private:
	BOOL	Overwrite( IBuff* pBuff );
	void	PrepareBS( IBuff* pBuff );
	BOOL	IsBSFull();
	IBuff*	GetFirstBS();
#endif	// __DBSERVER
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	void	RemoveBuffPet( IBuff* pBuff );
#endif	// __WORLDSERVER
	////////////////////////////////////////
#ifdef __CLIENT
public:
	int		GetCommercialCount();
	void	Render( );
#endif	// __CLIENT
	////////////////////////////////////////
#ifdef __DBSERVER
public:
	void	ToString( char* szString, size_t cchString );
#endif	// __DBSERVER
	////////////////////////////////////////
private:
	LIST_BUFF	m_listBuffs;
	const CMover* m_pMover;
#ifdef __CLIENT
	friend	class	CWndCommItemCtrl;
	friend	class	CWndBuffStatus;
	friend	class	CWndMgr;
	friend	class	CWndWorld;
	friend	class	CMover;
#endif	// __CLIENT
// chipi_090217
	size_t	GetRemoveBuffSize();
};


#endif	// __BUFF_H
