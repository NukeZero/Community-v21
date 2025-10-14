#ifndef __SKILLINFLUENCE_H__
#define	__SKILLINFLUENCE_H__

#include "ar.h"

#define		BUFF_ITEM	0
#define		BUFF_SKILL	1

#ifdef __XITEMBUFF

typedef	struct	tagSKILLINFLUENCE
{
	WORD	wType;			// 0:아이템 1:스킬 2:기타?
	WORD	wID;			// 아이템이나 스킬의 프로퍼티 ID
	DWORD	dwLevel;		// 스킬 레벨 - 저장
	DWORD	tmCount;		// 남은 시간(카운트).  - 저장
	DWORD	tmTime;			// 시작 타이머.
	BOOL	bEffect;		// 지속효과이펙트를 가지고 있을때 그것이 로딩됐는지.. FALSE면 로딩해야한다.
}

SKILLINFLUENCE, * LPSKILLINFLUENCE;
#define	MAX_SKILLINFLUENCE		64

class CMover;
class CSkillInfluence
{
private:
	SKILLINFLUENCE *m_pEmptyNode;	// 비어있는 공간의 인덱스.
	CMover* m_pMover;
	CRIT_SEC	m_AddRemoveLock;

	void	RemoveSkillInfluence( SKILLINFLUENCE* pSkillInfluence );
	
public:
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 	
	BOOL	RemoveSkillInfluence( WORD wType, WORD wID );
	BOOL	RemoveSkillInfluenceState( DWORD dwChrState );
	BOOL    RemoveAllSkillInfluence();
	BOOL	RemoveAllSkillDebuff( void );
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
//	Constructions
	CSkillInfluence();
	virtual	~CSkillInfluence();

	SKILLINFLUENCE		m_aSkillInfluence[ MAX_SKILLINFLUENCE ];
	
	void	Init( void );
	void	Destroy( void ) {}
	
	void	Serialize( CAr & ar );

//	Operations
	void	SetMover( CMover* pMover );
	BOOL	Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
	BOOL	Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
	LPSKILLINFLUENCE	Get( int nIdx );
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
	void	Process( void );
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
	void	Reset( void );		// 클라로 버프정보를 다시 갱신하도록 타이머를 클리어 시킴.
	BOOL	HasSkill( WORD wType, WORD wID );
#ifdef __GM_COMMAND_ITEM
#ifdef __CLIENT
	DWORD	GetDisguise( void );
#endif // __CLIENT
	BOOL	HasLikeItemBuf( DWORD dwItemKind3 );
	void	RemoveLikeItemBuf( DWORD dwItemKind3 );
#endif // __GM_COMMAND_ITEM
	
private:
	SKILLINFLUENCE *Find( WORD wType, WORD wID );
//	Attributes
	void	InfluenceMover( WORD wType, WORD wID, BOOL fInfluence );
	void	Remove( SKILLINFLUENCE *pNode );
	LPSKILLINFLUENCE	FindPtr( WORD wType, WORD wID );
#ifdef __GM_COMMAND_ITEM
	BOOL	LikeItemBuf( DWORD dwItemKind3 );
#endif // __GM_COMMAND_ITEM
};

//
inline void CSkillInfluence::SetMover( CMover* pMover )
	{	m_pMover	= pMover;	}

//
inline SKILLINFLUENCE* CSkillInfluence::Find( WORD wType, WORD wID )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == wType && pNode->wID == wID )	// 같은걸 찾음.
			return pNode;
	}

	return NULL;
}

//
inline BOOL CSkillInfluence::HasSkill( WORD wType, WORD wID )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	return Find( wType, wID ) != NULL;
}

//
inline LPSKILLINFLUENCE CSkillInfluence::FindPtr( WORD wType, WORD wID )
{
	return( Find( wType, wID ) );
}

//
inline LPSKILLINFLUENCE CSkillInfluence::Get( int nIdx )
{
	if( nIdx < 0 || nIdx >= MAX_SKILLINFLUENCE )
	{
		Error( "SKILLINFLUENCE::Get() : 범위를 넘어섬 %d", nIdx );
		return( NULL );
	}

	return( m_aSkillInfluence + nIdx );
}

//
inline void CSkillInfluence::Remove( SKILLINFLUENCE *pNode )
{
	InfluenceMover( pNode->wType, pNode->wID, FALSE );
	
	pNode->wType = 0;
	pNode->wID = 0;
	pNode->bEffect = 0;
	pNode->tmTime = 0;
	pNode->tmCount = 0;
	m_pEmptyNode = pNode;		// 지운 노드는 비어있으므로 그것을 받아둠.
}
#else // XITEMBUFF
//{{AFX
typedef	struct	tagSKILLINFLUENCE
{
	DWORD	dwSkill;		// 스킬 ID	- 저장
	DWORD	dwLevel;		// 스킬 레벨 - 저장
	DWORD	tmCount;		// 남은 시간(카운트).  - 저장
	DWORD	tmTime;			// 시작 타이머.
	BOOL	bEffect;		// 지속효과이펙트를 가지고 있을때 그것이 로딩됐는지.. FALSE면 로딩해야한다.
}
SKILLINFLUENCE, * LPSKILLINFLUENCE;
#define	MAX_SKILLINFLUENCE		16

class CMover;
class CSkillInfluence
{
private:
	//	u_long	m_uHead;
//	u_long	m_uTail;
	SKILLINFLUENCE *m_pEmptyNode;	// 비어있는 공간의 인덱스.
	CMover* m_pMover;
	CRIT_SEC	m_AddRemoveLock;

	void	RemoveSkillInfluence( SKILLINFLUENCE* pSkillInfluence );
	
public:
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 	
	BOOL	RemoveSkillInfluence( DWORD dwSkill );
	BOOL	RemoveSkillInfluenceState( DWORD dwChrState );
	BOOL    RemoveAllSkillInfluence();
	BOOL	RemoveAllSkillDebuff( void );
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
//	Constructions
	CSkillInfluence();
	virtual	~CSkillInfluence();

	SKILLINFLUENCE		m_aSkillInfluence[MAX_SKILLINFLUENCE];
	
	void	Init( void );
	void	Destroy( void ) {}
	
	void	Serialize( CAr & ar );

//	Operations
	void	SetMover( CMover* pMover );
	BOOL	Set( DWORD dwSkill, DWORD dwLevel, DWORD dwTime );
	BOOL	Set( SKILLINFLUENCE *pNode, DWORD dwSkill, DWORD dwLevel, DWORD dwTime );
	LPSKILLINFLUENCE	Get( int nIdx );
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
	void	Process( void );
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
	void	Reset( void );		// 클라로 버프정보를 다시 갱신하도록 타이머를 클리어 시킴.
	BOOL	HasSkill( DWORD dwType, DWORD dwSkill );
private:
	SKILLINFLUENCE *Find( DWORD dwSkill );
//	Attributes
	void	InfluenceMover( DWORD dwSkill, BOOL fInfluence );
	void	Remove( SKILLINFLUENCE *pNode );
	LPSKILLINFLUENCE	FindPtr( DWORD dwSkill );
};

inline void CSkillInfluence::SetMover( CMover* pMover )
	{	m_pMover	= pMover;	}

inline SKILLINFLUENCE* CSkillInfluence::Find( DWORD dwSkill )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->dwSkill == dwSkill )	// 같은걸 찾음.
			return pNode;
	}

	return NULL;
}

inline BOOL CSkillInfluence::HasSkill( DWORD dwType, DWORD dwSkill )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	return Find( dwSkill ) != NULL;
}

inline LPSKILLINFLUENCE CSkillInfluence::FindPtr( DWORD dwSkill )
{
	return( Find( dwSkill ) );
}

inline LPSKILLINFLUENCE CSkillInfluence::Get( int nIdx )
{
	if( nIdx < 0 || nIdx >= MAX_SKILLINFLUENCE )
	{
		Error( "SKILLINFLUENCE::Get() : 범위를 넘어섬 %d", nIdx );
		return( NULL );
	}
	return( m_aSkillInfluence + nIdx );
}

inline void CSkillInfluence::Remove( SKILLINFLUENCE *pNode )
{
	InfluenceMover( pNode->dwSkill, FALSE );
	
	pNode->dwSkill = 0;
	pNode->bEffect = 0;
	pNode->tmTime = 0;
	pNode->tmCount = 0;
	m_pEmptyNode = pNode;		// 지운 노드는 비어있으므로 그것을 받아둠.
}
//}}AFX
#endif // not xItemBuff
#endif	// __SKILLINFLUENCE_H__