#pragma once

#include <vector>
#include "..\_Common\ModelObject.h"
#include "ObjControl.h"
#include "..\_Common\File.h"

typedef struct stObjectUnit
{

	CModelObject	*m_Model;
	int				*pPartCount;
	int				*pId;
	D3DXMATRIX		*pWorldTM;
	bool			*pIsDeployed;
	//int				*pPartNum;
	vector<char *>  vecPartName;
	char			*pBoneName;
	char			*pAniName;

} stObjectUnit;   

class CObjMgr
{

	private:
		int						m_nObjCount;
		int						m_nFocusedObj;
		vector<stObjectUnit>	m_vObject;
		LPDIRECT3DDEVICE9		m_pDevice;

	public:

		CObjControl				m_ObjControl;

		CObjMgr();
		~CObjMgr();

		// 기능 함수 
		int		AddNewObj(int nID);
		int		AddPart(int nObjID, LPCTSTR szFileName);
		int		LoadBone(int nObjID, LPCTSTR szFileName);
		int		LoadAnim(int nObjID, LPCTSTR szFileName);
		int		Move(int nObjID, D3DXMATRIX *tmWorld);
		D3DXMATRIX	*GetWorldTM(int nObjID);
		D3DXVECTOR3 GetPos(int nObjID);
		bool	Save(CFileIO &File);
		int		Load(CFileIO &File);
	
		// 위치값 추가해야 함 
		int		DeployObj(int nObjID, D3DXVECTOR3 vPos);
		int		RemoveObj(int nObjID);
		int		SetLod(int nObjID, int nLodLv);
		int		SetAllLod(int nLodLv);
		int		SetFocus(int nObjID);
		int		AddFrame(int nObjID, int nCount);
		int		GetCurrentFrame(int nObjID);
		int		GetMaxFrame(int nObjID);
		int		SetFrame(int nObjID, int nCount);
		
		// 기본 함수 
		int		Init(LPDIRECT3DDEVICE9 currDevice);
		void	Update();
		int		Render();
		int		Destroy();		
		int		GetObjCount();

};


