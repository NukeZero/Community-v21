#include "StdAfx.h"


CObjMgr::CObjMgr()
{

	m_nFocusedObj = 0;
	m_nObjCount   = 0;
	m_vObject.clear();
	m_vObject.reserve(20);

}


CObjMgr::~CObjMgr()
{

	Destroy();

}


int CObjMgr::Init(LPDIRECT3DDEVICE9 currDevice)
{

	m_pDevice = currDevice;
	
	return 0;

}


bool CObjMgr::Save(CFileIO &File)
{

	// 오브젝트 갯수 저장.
	int nCount = GetObjCount();
	File.Write(&nCount, sizeof(int));
	// 오브젝트를 모두 저장한다.
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		File.Write(iter->pId,			sizeof(int));
		//File.Write(iter->m_Model,		sizeof(CModelObject));
		File.Write(iter->pPartCount,	sizeof(int));
		File.Write(iter->pWorldTM,		sizeof(D3DXMATRIX));
		File.Write(iter->pIsDeployed,	sizeof(bool));
		// 파일명에 관해서 기록
		int nBoneLen = strlen(iter->pBoneName);
		File.Write(&nBoneLen, sizeof(int));
		if(nBoneLen > 0) File.Write(iter->pBoneName, nBoneLen);

		int nAniLen = strlen(iter->pAniName);
		File.Write(&nAniLen, sizeof(int));
		if(nAniLen > 0) 
		{
			File.Write(iter->pAniName, nAniLen);
			File.Write(&iter->m_Model->m_fFrameCurrent, sizeof(float));
		}
		
		
		vector<char *>::iterator iter2;
		for(iter2 = iter->vecPartName.begin(); iter2 != iter->vecPartName.end(); iter2++)
		{
			int nNameLen = strlen(*iter2);
			File.Write(&nNameLen, sizeof(int));
			File.Write(*iter2, nNameLen);
		}
		
	}

	return true;

}

int CObjMgr::Load(CFileIO &File)
{
	Destroy();
	
	// 오브젝트 갯수
	int nCount = 0;
	int nId	   = 0;

	File.Read(&nCount, sizeof(int));

	for(int nIndex = 1; nIndex < nCount + 1; ++nIndex)
	{
		AddNewObj(nIndex);
	}

	for(int nIndex = 1; nIndex < nCount + 1; ++nIndex)
	{
		int nId = 0, nMisMatch = 0;

		File.Read(&nId,	sizeof(int));

		vector<stObjectUnit>::iterator iter = m_vObject.begin();

		for(; iter != m_vObject.end(); ++iter)
		{
			if(*iter->pId == nId)
			{
				//File.Read(iter->m_Model,		sizeof(CModelObject));
				File.Read(iter->pPartCount,	sizeof(int));
				File.Read(iter->pWorldTM,		sizeof(D3DXMATRIX));
				File.Read(iter->pIsDeployed,	sizeof(bool));
				
				// 파일로딩 시작~
				int nNameLen = 0;
				int nPartNum = *iter->pPartCount - 2;
				char szTemp[256];
				
				// 본 로드
				File.Read(&nNameLen, sizeof(int));
				if(nNameLen > 0) 
				{
					memset(szTemp, 0, 256);
					File.Read(szTemp, nNameLen);
					LoadBone(nId, szTemp);
				}
				// 애니 로딩
				File.Read(&nNameLen, sizeof(int));
				if(nNameLen > 0) 
				{
					memset(szTemp, 0, 256);
					File.Read(szTemp, nNameLen);
					LoadAnim(nId, szTemp);
					File.Read(&iter->m_Model->m_fFrameCurrent, sizeof(float));
				}
				// 파츠 로딩
				if(nPartNum > 0) *iter->pPartCount = 2;
				for(int nIdx = 0; nIdx < nPartNum; nIdx++)
				{
					memset(szTemp, 0, 256);
					File.Read(&nNameLen, sizeof(int));
					File.Read(szTemp, nNameLen);
					AddPart(nId, szTemp);
				}

			}
			else
			{
				nMisMatch++;
			}

		}
		if(nMisMatch == nCount) return -1;
	}
	return nCount;

}


// 특정 오브젝트의 LOD레벨을 조정한다 
int	CObjMgr::SetLod(int nObjID, int nLodLv)
{

	if(nLodLv > 2 || nLodLv < 0) return 0;

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			iter->m_Model->SetGroup(nLodLv);
			return 1;
		}
	}

	return 0;

}


// 모든 오브젝트의 LOD레벨을 조정한다 
int	CObjMgr::SetAllLod(int nLodLv)
{

	if(nLodLv > 2 || nLodLv < 0) return 0;

	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
			iter->m_Model->SetGroup(nLodLv);
	}

	return 0;

}


// 해당 위치에 오브젝트를 배치상태로 함
int	CObjMgr::DeployObj(int nObjID, D3DXVECTOR3 vPos)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			*iter->pIsDeployed = true;
			iter->pWorldTM->_41 = vPos.x;
			iter->pWorldTM->_42 = vPos.y;
			iter->pWorldTM->_43 = vPos.z;
			return 1;
		}
	}

	// 아이디에 해당하는 오브젝트가 없음 
	return 0;

}


// 파괴하는게 아니고 화면에만 배치안된 상태로 함
int	CObjMgr::RemoveObj(int nObjID)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			*iter->pIsDeployed = false;
			return 1;
		}
	}

	// 아이디에 해당하는 오브젝트가 없음 
	return 0;

}


int	CObjMgr::SetFocus(int nObjID)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		// 아이디가 맞고 배치된 오브젝트라면 포커스를 준다
		if(*iter->pId == nObjID && *iter->pIsDeployed) 
		{
			m_nFocusedObj = nObjID;
			m_ObjControl.getObjMt(iter->pWorldTM);
			return 1;
		}
	}

	// 아이디에 해당하는 오브젝트가 없음 
	return 0;
	

}


// 새로운 오브젝트를 생성하고 아이디를 리턴 
int CObjMgr::AddNewObj(int nID)
{
	if(m_vObject.size() > 0)
	{
		// 아이디로 해당 오브젝트를 찾는다 
		vector<stObjectUnit>::iterator iter = m_vObject.begin();

		for(; iter != m_vObject.end(); ++iter)
		{
			if(*iter->pId == nID) 
			{
				return 0;
			}
		}
	}
	
	CModelObject*	pModel			= new CModelObject;
	int*			pPartCount		= new int;
	int*			pId				= new int;
	D3DXMATRIX*		pWorld			= new D3DXMATRIX;
	bool*			pIsDeployed		= new bool;
	//char			*tmpBoneName	= new char[256];
	//char			*tmpAniName		= new char[256];
	stObjectUnit	TempUnit;
	

	pModel->InitDeviceObjects(m_pDevice);

	pModel->m_bEndFrame = FALSE;
	pModel->m_fFrameCurrent = 0;
	// 0으로 초기화했더니 라이팅을 못받는 파츠가 생김 --
	*pPartCount = 0;
	*pId = nID;
	//memset(tmpBoneName, 0, 256);
	//memset(tmpAniName, 0, 256);
	D3DXMatrixIdentity(pWorld);

	*pIsDeployed = false;

	TempUnit.m_Model		= pModel;
	TempUnit.pPartCount		= pPartCount;
	TempUnit.pId			= pId;
	TempUnit.pIsDeployed	= pIsDeployed;
	TempUnit.pWorldTM		= pWorld;
	TempUnit.pBoneName		= NULL;
	TempUnit.pAniName		= NULL;
	TempUnit.vecPartName.clear();
	m_vObject.push_back(TempUnit);
	 
	m_nObjCount++;
	
	return *TempUnit.pId;
}


int CObjMgr::AddPart(int nObjID, LPCTSTR szFileName)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			if(iter->m_Model->LoadElement(szFileName, (*iter->pPartCount)++)) 
			{
				iter->m_Model->RestoreDeviceObjects();
				char *szTmp = new char[256];
				strcpy(szTmp, szFileName); 
				iter->vecPartName.push_back(szTmp);
				return 1;
			}
		}
	}

	// fail
	return 0;

}


int CObjMgr::LoadBone(int nObjID, LPCTSTR szFileName)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID)
		{
			if(iter->m_Model->LoadBone( szFileName ))
			{
				if(!iter->pBoneName) iter->pBoneName = new char[256];
				strcpy(iter->pBoneName, szFileName); 
				return 1;
			}
		}
	}

	// fail
	return 0;

}


int CObjMgr::LoadAnim(int nObjID, LPCTSTR szFileName)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			if(iter->m_Model->LoadMotion( szFileName ))
			{
				if(!iter->pAniName) iter->pAniName = new char[256];
				strcpy(iter->pAniName, szFileName); 
				return 1;
			}
		}
	}

	// fail
	return 0;

}


int CObjMgr::Move(int nObjID, D3DXMATRIX *tmWorld)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) *iter->pWorldTM = *tmWorld;
	}

	// fail
	return 0;

}


D3DXMATRIX* CObjMgr::GetWorldTM(int nObjID)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) return iter->pWorldTM;
	}

	// fail
	return NULL;

}


int CObjMgr::AddFrame(int nObjID, int nCount)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			iter->m_Model->m_fFrameCurrent += nCount;
			return 1;
		}
			
	}

	// fail
	return 0;

}


int CObjMgr::GetCurrentFrame(int nObjID)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			return	( (int)( iter->m_Model->m_fFrameCurrent ) );
		}
	}

	// fail
	return -1;
}


int CObjMgr::GetMaxFrame(int nObjID)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			return iter->m_Model->m_nFrameMax;
		}	
	}

	// fail
	return -1;

}


int CObjMgr::SetFrame(int nObjID, int nCount)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID) 
		{
			iter->m_Model->m_fFrameCurrent = (float)( nCount );
			return 1;
		}			
	}

	// fail
	return 0;

}


D3DXVECTOR3 CObjMgr::GetPos(int nObjID)
{

	// 아이디로 해당 오브젝트를 찾는다 
	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(*iter->pId == nObjID)
		{
			D3DXVECTOR3 vValue;

			vValue.x = iter->pWorldTM->_41;
			vValue.y = iter->pWorldTM->_42;
			vValue.z = iter->pWorldTM->_43;

			return vValue;
		}
	}

	// fail
	return D3DXVECTOR3(0.0f, 0.0f, 0.0f);

}


void CObjMgr::Update()
{
	// 포커스 된 객체의 업데이트를 한다.
	if(m_nFocusedObj)
	{

		m_ObjControl.UpdateMatrix();
		if( ::GetAsyncKeyState('W') & 0x8000f )
			m_ObjControl.walk(-0.05f);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			m_ObjControl.walk(0.02f);

		if( ::GetAsyncKeyState('A') & 0x8000f )
			m_ObjControl.RotateY(0.06f);

		if( ::GetAsyncKeyState('D') & 0x8000f )
			m_ObjControl.RotateY(-0.06f);
		
		m_ObjControl.UpdateMatrix();
	}

	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(iter->pIsDeployed) iter->m_Model->FrameMove();
	}
	
}


int CObjMgr::Render()
{

	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(iter->pIsDeployed) iter->m_Model->Render(m_pDevice, iter->pWorldTM);
	}

	return 1;

}


int CObjMgr::Destroy()
{

	vector<stObjectUnit>::iterator iter = m_vObject.begin();

	for(; iter != m_vObject.end(); ++iter)
	{
		if(iter->m_Model)		delete iter->m_Model;
		if(iter->pId)			delete iter->pId;
		if(iter->pPartCount)	delete iter->pPartCount;
		if(iter->pWorldTM)		delete iter->pWorldTM;
		if(iter->pIsDeployed)	delete iter->pIsDeployed;
		if(iter->pAniName)		delete [] iter->pAniName;
		if(iter->pBoneName)		delete [] iter->pBoneName;
		vector<char *>::iterator iter2;
		for(iter2 = iter->vecPartName.begin(); iter2 != iter->vecPartName.end(); iter2++)
		{
			delete [] *iter2;
		}
		iter->vecPartName.clear();
		iter->m_Model = NULL;
	}

	m_vObject.clear();

	return 1;

}


int	CObjMgr::GetObjCount()
{

	return m_vObject.size();

}