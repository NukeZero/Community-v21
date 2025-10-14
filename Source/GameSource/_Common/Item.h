#if !defined(AFX_ITEM_H__80E88B36_BD6B_449B_BE76_34F2B5B77552__INCLUDED_)
#define AFX_ITEM_H__80E88B36_BD6B_449B_BE76_34F2B5B77552__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__DBSERVER)
#include "..\_Network\Objects\Obj.h"
#else
#include "Ctrl.h"
#endif
#include "ProjectCmn.h"
#include "lang.h"

#include "pet.h"		//sun: 9, 9-10차 펫
#include "accessory.h"	//sun: 11, 채집 시스템
#include "FLPiercingOption.h"

#include "serialnumber.h"

#include "FLRandomOption.h"

#include "FLItemElem.h"


typedef struct tagSkill
{
	DWORD dwSkill;
	DWORD dwLevel;
	SkillProp* GetProp()	{ return prj.GetSkillProp( dwSkill ); }
	
} SKILL,* LPSKILL;


//////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#include <memory/FLMemPooler.h>

/// 지형위에 존재하는 아이템 
class CItem  : public CCtrl
{
private:
#ifdef __CLIENT
	float	m_fGroundY;		// 아이템이 생성될당시 그 위치의 Y좌표를 미리 구해놓자.
	D3DXVECTOR3	 m_vDelta;	// 
#endif // __CLIENT
	
public:
	FLItemBase* m_pItemBase;
	u_long	m_idHolder;
	OBJID	m_idOwn;		// 누가 이아이템을 가질 권리가 있는가.
	DWORD	m_dwDropTime;	// 드랍했을때의 시간.
	BOOL	m_bDropMob;		// 몬스터가 죽어서 드랍한것이냐?.
#ifdef __EVENT_MONSTER
	BOOL	m_IdEventMonster;	// 이벤트 몬스터가 드랍한 아이템인가?
#endif // __EVENT_MONSTER


public:
	CItem();
	virtual ~CItem();

	void SetOwner( OBJID id );
	void SetItemBase( FLItemBase* pItemBase ) { m_pItemBase = pItemBase; }

	FLItemBase* GetItemBase() const { return m_pItemBase; }

	PT_ITEM_SPEC GetProp() { return g_xSpecManager->GetSpecItem( GetIndex() ); }
	
	virtual void Serialize( CAr & ar, int nMethod );
	virtual void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );
	virtual BOOL Read( CFileIO* pFile );
		
#ifdef __CLIENT
	void SetDelta( float fGroundY, D3DXVECTOR3 &vDelta );
#endif // __CLIENT

public:
	MEMPOOLER_DECLARE( CItem );
};

#endif // !defined(AFX_ITEM_H__80E88B36_BD6B_449B_BE76_34F2B5B77552__INCLUDED_)
