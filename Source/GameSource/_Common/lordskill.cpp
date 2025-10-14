#include "stdafx.h"

//sun: 12, 군주

#include "lordskill.h"

#ifdef __WORLDSERVER
#include "../WorldServer/slord.h"
#include "../WorldServer/user.h"
#endif	// __WORLDSERVER

CLordSkillComponent::CLordSkillComponent()
:
m_nId( 0 ),
m_nCooltime( 0 ),
m_nTick( 0 ),
m_nItem( 0 ),
m_bPassive( 0 ),
m_nTargetType( SKILL_INDEX( 0, LTT_NA ) )
#ifdef __CLIENT
, m_pTexture( NULL )
, m_dwSrcSfx( NULL_ID )
, m_dwDstSfx( NULL_ID )
#endif	// __CLIENT
{
	m_szName[0]	= '\0';
	m_szDesc[0]	= '\0';
	m_szIcon[0]	= '\0';
}

CLordSkillComponent::~CLordSkillComponent()
{
}

void CLordSkillComponent::Initialize( int nId, int nCooltime, int nItem, const char* szName,
									 const char* szDesc, const char* szIcon, BOOL bPassive, int nTargetType,
									 DWORD dwSrcSfx, DWORD dwDstSfx, FLOAT fRange )
{
	m_nId	= nId;
	m_nCooltime		= nCooltime;
	m_nItem		= nItem;
	FLStrcpy( m_szName, _countof( m_szName ), szName );
	FLStrcpy( m_szDesc, _countof( m_szDesc ), szDesc );
	FLStrcpy( m_szIcon, _countof( m_szIcon ), szIcon );
	m_bPassive	= bPassive;
	m_nTargetType	= nTargetType;
#ifdef __CLIENT
	m_dwSrcSfx	= dwSrcSfx;
	m_dwDstSfx	= dwDstSfx;
#else
	UNREFERENCED_PARAMETER( dwSrcSfx );
	UNREFERENCED_PARAMETER( dwDstSfx );
#endif	// __CLIENT

#ifndef __DBSERVER
	m_fRange	= fRange;
#else
	UNREFERENCED_PARAMETER( fRange );
#endif	// __DBSERVER
}

#ifdef __CLIENT
CTexture* CLordSkillComponent::GetTexture( void )
{
	if( !m_pTexture )
		m_pTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, m_szIcon ), 0xffff00ff );
	return m_pTexture;
}
#endif	// __CLIENT

int CLordSkillComponent::Tick( void )
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
CLordSkill::CLordSkill( CLord*	pLord )
:
m_pLord( pLord )
{
}

CLordSkill::~CLordSkill()
{
	Clear();
}

void CLordSkill::Clear( void )
{
	for( VLSC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		SAFE_DELETE( *i );
	m_vComponents.clear();
}

void CLordSkill::AddSkillComponent( CLordSkillComponentExecutable* pComponent )
{
	m_vComponents.push_back( pComponent );
}

CLordSkillComponentExecutable* CLordSkill::GetSkill( int nId )
{
	for( VLSC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
	{
		if( ( *i )->GetId() == nId )
			return ( *i );
	}
	return NULL;
}

CLordSkillComponentExecutable* CLordSkill::CreateSkillComponent( int /*nType*/ )
{
	return new CLordSkillComponentExecutable;
}

void CLordSkill::Reset( void )
{
	for( VLSC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		( *i )->SetTick( 0 );
}

BOOL CLordSkill::Initialize( const char* szFile )
{
	CScript s;
	if( !s.Load( szFile, FALSE ) )
		return FALSE;

	int nId		= s.GetNumber();
	while( s.tok != FINISHED )
	{
		char szName[CLordSkillComponent::eMaxName]	= { 0,};
		char szDesc[CLordSkillComponent::eMaxDesc]	= { 0,};
		char szIcon[CLordSkillComponent::eMaxIcon]	= { 0,};
		int nType	= s.GetNumber();
		s.GetToken();
		FLStrncpy( szName, _countof( szName ), s.token, CLordSkillComponent::eMaxName );

		s.GetToken();
		FLStrncpy( szDesc, _countof( szDesc ), s.token, CLordSkillComponent::eMaxDesc );

		int nTargetType		= s.GetNumber();
		int	nItem	= s.GetNumber();
		int nCooltime	= s.GetNumber();
		s.GetToken();
		FLStrncpy( szIcon, _countof( szIcon ), s.token, CLordSkillComponent::eMaxIcon );

		BOOL bPassive	= static_cast<BOOL>( s.GetNumber() );
		DWORD dwSrcSfx	= s.GetNumber();
		DWORD dwDstSfx	= s.GetNumber();
		FLOAT fRange	= s.GetFloat();
		CLordSkillComponentExecutable* pComponent		= CreateSkillComponent( nType );
		pComponent->Initialize( nId, nCooltime, nItem, szName, szDesc, szIcon, bPassive, nTargetType, dwSrcSfx, dwDstSfx, fRange );
		AddSkillComponent( pComponent );
		nId		= s.GetNumber();
	}
	return TRUE;
}

void CLordSkill::SerializeTick( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_vComponents.size();
		for( VLSC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
		{
			ar << ( *i )->GetId();
			ar << ( *i )->GetTick();
		}
	}
	else
	{
		size_t nSize;
		ar >> nSize;
		for( size_t i = 0; i < nSize; i++ )
		{
			int nSkill, nTick;
			ar >> nSkill >> nTick;
			CLordSkillComponent* pSkill		= GetSkill( nSkill );
			if( pSkill )
				pSkill->SetTick( nTick );
		}
	}
}

#ifdef __WORLDSERVER
int CLordSkillComponentExecutable::IsExecutable( FLWSUser* pUser, const char* szTarget, u_long & idTarget )
{
	if( !CSLord::Instance()->IsLord( pUser->m_idPlayer ) )
	{
		return TID_GAME_LORD_SKILL_USE_E001;
	}
	if( IsPassive() )
	{
		return TID_GAME_LORD_SKILL_USE_E003;
	}

	idTarget	= pUser->m_idPlayer;
	if( GetTargetType() != SKILL_INDEX( 1, LTT_SELF ) )
	{
		idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( const_cast<char*>( szTarget ) );
		FLWSUser* pTarget	= g_xWSUserManager->GetUserByPlayerID( idTarget );
		if( !IsValidObj( pTarget ) || ( GetTargetType() == SKILL_INDEX( 2, LTT_OTHER ) && pTarget == pUser ) )
		{
			return TID_GAME_LORD_SKILL_USE_E004;
		}

		if( GetRange() != 0.0F )
		{
			D3DXVECTOR3 v	= pUser->GetPos() - pTarget->GetPos();
			FLOAT fLengthSq	= D3DXVec3LengthSq( &v );
			if( fLengthSq > GetRange() * GetRange() )
			{
				return TID_GAME_LORD_SKILL_USE_E008;
			}
		}

		// 12-2 康	- 일반화 대상
		if( GetId() == SKILL_INDEX( 4, LI_RAGE ) && pTarget->IsAuthHigher( AUTH_GAMEMASTER ) )
		{
			return TID_GAME_LORD_SKILL_USE_E004;
		}
	}

	return 0;
}

int CLordSkillComponentExecutable::IsTeleportable( FLWSUser* pUser, FLWSUser* pTarget )
{
	FLASSERT( pTarget );
	if( pTarget->IsAuthHigher( AUTH_GAMEMASTER ) )
		return TID_GAME_LORD_SKILL_USE_E004;
	if( !pUser->IsTeleportable() )
		return TID_GAME_LORD_SKILL_USE_E009;
	if( !pTarget->IsTeleportable() )
		return TID_GAME_LORD_SKILL_USE_E010;
	return 0;
}

void CLordSkillComponentExecutable::PutSkillLog( FLWSUser* pUser )
{
	g_DPSrvr.PutPenyaLog( pUser, "s", GetName(), 0 );
}
#endif	// __WORLDSERVER
