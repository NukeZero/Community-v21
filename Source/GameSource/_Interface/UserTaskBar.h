// UserTaskBar.h: interface for the CUserTaskBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERTASKBAR_H__FBB0414C_20FE_4032_9180_1F9559FAA4A8__INCLUDED_)
#define AFX_USERTASKBAR_H__FBB0414C_20FE_4032_9180_1F9559FAA4A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <network/FLAr.h>
//#include "User.h"

class CUserTaskBar  
{
private:
	DWORD	m_dwActionSlotUseTick;
	bool	m_bActionSlotNoti;

public:
	CUserTaskBar();
	virtual ~CUserTaskBar();

public:
	SHORTCUT	m_aSlotApplet[ MAX_SLOT_APPLET ]; // 1 ~ 20		// TODO : 19차에서는 사용 안함.
	SHORTCUT	m_aSlotItem  [ MAX_SLOT_ITEM_COUNT ][ MAX_SLOT_ITEM   ]; // 1 ~ 0(10)
	SHORTCUT	m_aSlotQueue [ MAX_SLOT_QUEUE  ];
	int			m_nActionPoint;		// TODO : 19차에서는 사용 안함.
	int			m_nUsedSkillQueue;	// -1:실행중이지 않음 0~4:현재 실행중인 스킬큐 인덱스.

public:
	void	Serialize(CAr &ar);
	void	SetShortcut( int nIndex, DWORD dwShortcut, DWORD dwType, DWORD dwIndex, DWORD dwId, DWORD dwData = 0, int nWhere = 0 );
	void	InitTaskBar();
	void	InitTaskBarShorcutKind( DWORD dwShortcutKind );

#ifdef __WORLDSERVER
	bool	SetNextSkill( FLWSUser *pUser );
	void	OnEndSkillQueue( FLWSUser *pUser );
	bool	ExistCanUseSkillByActionSlot( FLWSUser* pUser ) const;
	DWORD	GetNextActionSlotSkillIndex_HardCording( FLWSUser* pUser );
	
	bool	IsExecutionSkillQueue() const		{	return m_nUsedSkillQueue != SKILL_QUEUE_NONE;	}

	void	SetUseSkillQueue();
	bool	CanUseSkillQueue( FLWSUser* pUser ) const;
	void	Process( FLWSUser* pUser );

private:
	bool	SkipActionSlotSkill_HardCording( FLWSUser* pUser );
#endif // __WORLDSERVER
};

#endif // !defined(AFX_USERTASKBAR_H__FBB0414C_20FE_4032_9180_1F9559FAA4A8__INCLUDED_)
