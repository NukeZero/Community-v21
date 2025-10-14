#include "stdafx.h"

#include "FLArenaReaper.h"



FLArenaReaper::FLArenaReaper( CMover & Owner ) : FLStalkerFSM( Owner )
{
	m_OptinalFlags.bIgnoreTransparent = TRUE;
	m_OptinalFlags.bIgnoreMatchless = TRUE;
	m_OptinalFlags.bIgnoreCollision = TRUE;

	//m_OptinalFlags.bOneKillMode = TRUE;
	m_OptinalFlags.bUnconditionalHit = TRUE;

	Owner.m_dwMode &= (~MATCHLESS2_MODE);
	Owner.m_dwMode |= MATCHLESS_MODE;	
	//Owner.m_dwMode |= MATCHLESS2_MODE;
}

FLArenaReaper::~FLArenaReaper()
{

}