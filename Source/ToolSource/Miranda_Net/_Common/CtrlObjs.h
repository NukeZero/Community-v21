#ifndef __CTRLOBJS_H__
#define __CTRLOBJS_H__

#ifdef __WORLDSERVER
#include "Ctrl.h"

// 일정지역에 지속적 데미지를 주는 가상오브젝트
class CContDamageCtrl : public CCtrl
{
private:
	DWORD	m_tmStart, m_tmUpdate;
	
	void Init( void );
	void Destroy( void );
	
public:
	ItemProp *m_pSkillProp;
	AddSkillProp *m_pAddSkillProp;
	OBJID m_idSrc;
	OBJID m_idTarget;

	void	ApplyDamage( void );
	void	Process( FLOAT fElapsedTime );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
	
	CContDamageCtrl() 
	{ 
	#ifdef __WORLDSERVER
		SetVirtual( TRUE ); 
	#endif
		Init(); 
	}
	~CContDamageCtrl() { Destroy(); }
	
};

#endif // Worldserver

#endif // CtrlObjs_h