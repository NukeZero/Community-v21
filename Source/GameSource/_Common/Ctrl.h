#if !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
#define AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Obj.h"
#include "ProjectCmn.h"

#ifdef __WORLDSERVER
	class		FLWSUser;
	class		CCtrl;
#endif	// __WORLDSERVER

struct		CtrlProp;
class		CWorld;
class       CSfx;
class		CShip;

/// 배경 오브젝트를 제외한 모든 오브젝트의 base class
class CCtrl : public CObj
{
public:
	CCtrl();
	virtual ~CCtrl();

protected:
	OBJID			m_objid;			/// 객체 아이디
	CShip*			m_pIAObjLink;		/// 비공정 관련 - this가 어느IA오브젝트에 연결이 되었는가. 나중에 CShip*은 CIAObj* 형으로 대체되어야 한다.
	D3DXMATRIX		m_mInvTM;			/// 비공정 관련 - m_matWorld의 역행렬. pIAObjLink와 곱해서 로컬행렬을 얻음.

public:

#ifdef __WORLDSERVER
	LONG			m_lRespawn;			/// 리스폰 번호
	int	            m_nRespawnType;		/// 리스폰 타입 
	//std::map< DWORD, FLWSUser* > m_2pc;			/// 주변 플레이어 맵 
	//int				m_nOldCenter[MAX_LINKLEVEL];	/// 링크맵 계산용 
#endif	// __WORLDSERVER

public:
	virtual BOOL	GetSkillProp( SkillProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );

	virtual int		SendDamage( DWORD /*dwAtkFlag*/, OBJID /*idAttacker*/, int nParam = 0, BOOL bTarget = TRUE ) { UNREFERENCED_PARAMETER( nParam );	UNREFERENCED_PARAMETER( bTarget );	return 0; }
	virtual int		SendDamageForce( DWORD /*dwAtkFlags*/, OBJID /*idSender*/, int nParam = 0, BOOL bTarget = TRUE ) { UNREFERENCED_PARAMETER( nParam );	UNREFERENCED_PARAMETER( bTarget );	return 0; }	// 강공격

	virtual void	AddItToGlobalId();
	virtual	void	RemoveItFromGlobalId();

	virtual void	Process();
	virtual void	Render(LPDIRECT3DDEVICE9 pd3dDevice);

	virtual BOOL	Read( CFileIO* pFile );
	virtual	void	Serialize( CAr & ar, int nMethod );

	void			SetId( OBJID objid )	{ m_objid = objid;  }
	OBJID			GetId()					{ return m_objid;   }

	int				TargetSelecter( CCtrl *pTarget );

	CShip*			GetIAObjLink()			{ return m_pIAObjLink; }
	void			SetIAObjLink( CShip *pIA );
	D3DXMATRIX*		GetInvTM()				{ return &m_mInvTM; }

#ifdef __WORLDSERVER
	//void			PCSetAt( OBJID objid, CCtrl* pCtrl );	
	//BOOL			PCRemoveKey( OBJID objid );	
	//void			RemoveItFromView( BOOL bRemoveall = FALSE );
	//BOOL			IsNearPC( OBJID objid );
	BOOL			IsNearPC( FLWSUser* pUser );
	LONG			GetRespawn()			{ return m_lRespawn; }
	void			SetRespawn( LONG n, BYTE nType )	{ m_lRespawn = n; m_nRespawnType = nType; }	
	BOOL			ProcessDeleteRespawn();
	int				GetRespawnType( )		{ return m_nRespawnType; }	
#endif	//__WORLDSERVER

};

#ifdef __WORLDSERVER

//inline void CCtrl::PCSetAt( OBJID objid, CCtrl* pCtrl )	
//{	
//	m_2pc[ objid ] = (FLWSUser *)pCtrl;
//}
//
//inline BOOL CCtrl::PCRemoveKey( OBJID objid )	
//{	
//	return m_2pc.erase( objid ) > 0;
//}

#endif	// __WORLDSERVER


#endif // !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)