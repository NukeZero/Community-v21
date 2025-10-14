#ifndef __COMMONCTRL_H__
#define __COMMONCTRL_H__

#include "Ctrl.h"
#include "Respawn.h"
#include "ProjectCmn.h"


class CCommonCtrl : public CCtrl
{
public:
	OBJID		m_nMoverID;
	CCtrlElem	m_CtrlElem;

	BOOL		 m_bTrap;
	BOOL		 m_bAniPlay;
	BOOL		 m_bAlpha;
	int			 m_nBlendFactorLocal;
	DWORD		 m_dwTrapProcessTime;
	
	float		 m_fFrameCount;
	BOOL		 m_bAniDelay;
	DWORD		 m_dwAniDelay;
	
public:
	void Init();
	ItemProp *m_pSkillProp;
	AddSkillProp *m_pAddSkillProp;
	CSfxModel *m_pSfxModel;
	CSfxModel *m_pSfxModel2;
	OBJID	m_idAttacker;			// 사이킥 월 시전자.
	int		m_nCount;
	int		m_nEvent;
	DWORD	m_tmStart;
	int		m_nLife;				// 몇번 부딪히면 없어지냐.

	void DestroyWall( void );
	void _ProcessWall( void );
	void _CreateTrapSkill();
	void _ProcessTrap();
	void _ProcessAction();
	void DropNPC();
	CCommonCtrl();
	virtual ~CCommonCtrl();
	
	void			DropItem();
	void			SetActionPlay();
	CtrlProp*		GetProp() { return prj.GetCtrlProp( GetIndex() ); }
//	DWORD			GetCtrlType() const { return m_dwCtrlId; }
//	void			SetCtrlBase( CItemBase* pItemBase ) { m_pItemBase = pItemBase; }
	
//	virtual void Serialize( CAr & ar );
	virtual void Process( FLOAT fElapsedTime );
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );

	virtual BOOL Read( CFileIO* pFile );
	virtual BOOL Write( CFileIO* pFile );

#ifdef __BEAST
	CRespawnInfo m_respawnInfo;
	BOOL IsRespawn() { return m_respawnInfo.m_dwIndex ? TRUE : FALSE; }
#endif

};

#endif //__COMMONCTRL_H__
