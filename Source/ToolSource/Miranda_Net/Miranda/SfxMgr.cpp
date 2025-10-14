#include "StdAfx.h"
#include "SfxMgr.h"


CSfxMgr::CSfxMgr()
{

	m_pObjMgr	 = NULL;

	m_vUnit.clear();
	m_vUnit.reserve(20);

}


CSfxMgr:: ~CSfxMgr()
{
	
	Destroy();

}


void CSfxMgr::GetObjMgr(CObjMgr* pMgr)
{
	m_pObjMgr = pMgr;
}


// 이펙트 매니져 클래스 초기화 루틴 
int CSfxMgr::Init(LPDIRECT3DDEVICE9 currDevice)
{

	m_pDevice = currDevice;

	return 1;

}


bool CSfxMgr::Save(CFileIO &File)
{
	
	// 오브젝트 갯수 저장.
	int nCount = GetObjCount();
	File.Write(&nCount, sizeof(int));
	// 오브젝트를 모두 저장한다.
	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		int		nLen	 = iter->m_Base->m_strName.GetLength();
		char *szFileName = (LPSTR)(LPCSTR)iter->m_Base->m_strName;
		
		File.Write(iter->pId,		sizeof(int));
		File.Write(&nLen,			sizeof(int));
		File.Write(szFileName,		nLen);
		File.Write(&iter->m_Model->m_vPos,		sizeof(D3DXVECTOR3));
		File.Write(&iter->m_Model->m_nCurFrame,	sizeof(int));
		File.Write(iter->pIsLoop,	sizeof(bool));
		File.Write(iter->pHotKey,	sizeof(UINT));
		File.Write(iter->pMaxFrame, sizeof(int));
		File.Write(iter->pInterval, sizeof(int));
		File.Write(iter->pFollowObj, sizeof(int));
		File.Write(iter->pIsDeployed, sizeof(bool));
	}
	return true;

}


int CSfxMgr::Load(CFileIO &File)
{

	Destroy();
	
	// 오브젝트 갯수
	int nCount = 0;
	int nId	   = 0;

	File.Read(&nCount, sizeof(int));

	for(int nIndex = 1; nIndex < nCount + 1; ++nIndex)
	{
		NewSfx(nIndex);
	}

	for(int nIndex = 1; nIndex < nCount + 1; ++nIndex)
	{
		int nId = 0, nMisMatch = 0;

		File.Read(&nId,	sizeof(int));

		vector<stSfxUnit>::iterator iter = m_vUnit.begin();

		for(; iter != m_vUnit.end(); ++iter)
		{
			if(*iter->pId == nId)
			{
				char	szFileName[128];
				int		nLen;
				
				memset(szFileName, 0, 128);
				File.Read(&nLen, sizeof(int));
				File.Read(&szFileName, nLen);

				iter->m_Base->m_strName = (LPCSTR)(LPSTR)szFileName;

				if(iter->m_Base->Load())
				{
					iter->m_Model->SetSfx(iter->m_Base);
					File.Read(&iter->m_Model->m_vPos,		sizeof(D3DXVECTOR3));
					File.Read(&iter->m_Model->m_nCurFrame,	sizeof(int));
					File.Read(iter->pIsLoop,	sizeof(bool));
					File.Read(iter->pHotKey,	sizeof(UINT));
					File.Read(iter->pMaxFrame, sizeof(int));
					File.Read(iter->pInterval, sizeof(int));
					File.Read(iter->pFollowObj, sizeof(int));
					File.Read(iter->pIsDeployed, sizeof(bool));
				}
				else	return -1;
				
			}
			else	nMisMatch++;

		}
		if(nMisMatch == nCount) return -1;
	}
	
	return nCount;

}


int CSfxMgr::SetLoop(int nSfxId, bool bIsLoop)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)	
		{
			*iter->pIsLoop = bIsLoop;
		}
	}

	return 0;

}


int CSfxMgr::SetInterval(int nSfxId, int nInterval)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)	
		{
			*iter->pInterval = nInterval;
			return 1;
		}
	}

	return 0;

}


int CSfxMgr::SetHotKey(int nSfxId, UINT nKey)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)	
		{
			*iter->pHotKey = nKey;
			return 1;
		}
	}

	return 0;

}


int CSfxMgr::FollowObj(int nSfxId, int nObjId)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			*iter->pFollowObj = nObjId;
			return 1;
		}
	}

	return 0;

}


int	CSfxMgr::SetWorld(int nSfxId, D3DXMATRIX* pWorldTM)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			iter->m_Model->m_vPos.x = pWorldTM->_41;
			iter->m_Model->m_vPos.y = pWorldTM->_42;
			iter->m_Model->m_vPos.z = pWorldTM->_43;
			return 1;
		}
	}
	
	return 0;

}


int	CSfxMgr::SetPos(int nSfxId, D3DXVECTOR3 vPos)
{
	
	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			iter->m_Model->m_vPos = vPos;
			return 1;
		}
	}
	return 0;

}


int CSfxMgr::DeploySfx(int nSfxId, D3DXVECTOR3 vPos)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			*iter->pIsDeployed = true;
			iter->m_Model->m_vPos = vPos;
			return 1;
		}
	}
	return 0;

}


int	CSfxMgr::DeleteSfx(int nSfxId)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			*iter->pIsDeployed = false;
			return 1;
		}
	}

	return 0;
	
}


int	CSfxMgr::SetRotate(int nSfxId, D3DXVECTOR3 vRotate)
{

	return 0;

}

int	CSfxMgr::NewSfx(int nID)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nID)
			return 0;
	}

	CSfxBase	*tempBase	= new CSfxBase;
	CSfxModel	*tempModel	= new CSfxModel;
	int			*pMaxFrame  = new int;
	bool		*pIsLoop	= new bool;
	int			*pId		= new int;
	UINT		*pHotKey	= new UINT;
	int			*pInterval	= new int;
	int			*pFollowObj = new int;
	bool		*pIsDeployed = new bool;

	stSfxUnit	tempUnit;


	*pMaxFrame		    = 0;
	*pIsLoop			= false;
	*pId				= nID;
	*pHotKey			= HOTKEY_NONE;
	*pInterval			= 0;
	*pFollowObj			= 0;
	*pIsDeployed		= false;

	tempUnit.m_Base     = tempBase;
	tempUnit.m_Model    = tempModel;
	tempUnit.pMaxFrame  = pMaxFrame;
	tempUnit.pIsLoop    = pIsLoop;
	tempUnit.pId	    = pId;
	tempUnit.pHotKey    = pHotKey;
	tempUnit.pInterval  = pInterval;
	tempUnit.pFollowObj = pFollowObj;
	tempUnit.pIsDeployed = pIsDeployed;

	m_vUnit.push_back(tempUnit);

	return 1;

}


int	CSfxMgr::AddSfx(int nId, CString FileName)
{

	FileName.MakeLower();

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nId)
		{
			iter->m_Base->m_strName = FileName;

			if(iter->m_Base->Load())
			{
				int nMaxFrame = 0;


				iter->m_Model->SetSfx(iter->m_Base);

				// 최대 프레임을 찾는다 
				for(int i = 0; i < iter->m_Model->m_pSfxBase->m_apParts.GetSize(); i++)
				{	
					for(int j = 0; j < iter->m_Model->m_pSfxBase->Part( i )->m_apKeyFrames.GetSize(); j++)
					{
						// 각 파트의 프레임 수와 비교해서 최대 프레임을 찾음
						if(nMaxFrame < iter->m_Model->m_pSfxBase->Part( i )->Key(j)->nFrame)
							nMaxFrame = iter->m_Model->m_pSfxBase->Part( i )->Key(j)->nFrame;
					}
					
				}

				*iter->pMaxFrame = nMaxFrame;

				return *iter->pId;
			}
			
		}
			
	}



	return 0;

}

// 업데이트 루틴 
void CSfxMgr::Update()
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		// loop 기능 지원 		
		if(iter->m_Model->m_nCurFrame > *iter->pMaxFrame + *iter->pInterval)
		{
			if(*iter->pIsLoop)
			{
				iter->m_Model->m_nCurFrame = 0;
			}
			else
			{
				if(iter->m_Model->m_nCurFrame > *iter->pMaxFrame + 1)
				iter->m_Model->m_nCurFrame = *iter->pMaxFrame + 1;
			}
			
		}
	
		// HOT Key 지원 
		if(*iter->pHotKey) 
		{
			if( ::GetAsyncKeyState(*iter->pHotKey) & 0x8000f)
			{
				iter->m_Model->m_nCurFrame = 0;
			}
		}

		// 따라다니기 지원 
		if(*iter->pFollowObj)
		{
			if(m_pObjMgr)
			{
				iter->m_Model->m_vPos = m_pObjMgr->GetPos(*iter->pFollowObj);
			}
		}		

		if(*iter->pIsDeployed) iter->m_Model->Process();
	}

}

int CSfxMgr::SetFrame(int nSfxId, int nFrameIdx)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			iter->m_Model->m_nCurFrame = nFrameIdx;
			return 1;
		}
	}

	return 0;

}


int CSfxMgr::AddFrame(int nSfxId, int nAmount)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			iter->m_Model->m_nCurFrame += nAmount;
			return 1;
		}
	}

	return 0;

}


int	CSfxMgr::GetCurrentFrame(int nSfxId)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			return iter->m_Model->m_nCurFrame;
		}
	}

	return -1;

}


int	CSfxMgr::GetMaxFrame(int nSfxId)
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pId == nSfxId)
		{
			return *iter->pMaxFrame;
		}
	}

	return -1;

}


// 렌더링 루틴 
int CSfxMgr::Render()
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(*iter->pIsDeployed) iter->m_Model->Render(m_pDevice, NULL);
	}

	return 1;

}


// 해제 루틴 
int CSfxMgr::Destroy()
{

	vector<stSfxUnit>::iterator iter = m_vUnit.begin();

	for(; iter != m_vUnit.end(); ++iter)
	{
		if(iter->m_Model)		delete iter->m_Model;
		if(iter->m_Base)		delete iter->m_Base;
		if(iter->pMaxFrame)		delete iter->pMaxFrame;
		if(iter->pIsLoop)		delete iter->pIsLoop;
		if(iter->pId)			delete iter->pId;
		if(iter->pHotKey)		delete iter->pHotKey;
		if(iter->pInterval)		delete iter->pInterval;
		if(iter->pFollowObj)	delete iter->pFollowObj;
		if(iter->pIsDeployed)	delete iter->pIsDeployed;
	}

	m_vUnit.clear();

	return 1;
}

int	CSfxMgr::GetObjCount()
{

	return m_vUnit.size();

}

////////////////////////////////////////////////////////////////////////////////////

void BsSfxUnit::Init( const char* szFileName )
{
	SAFE_DELETE( _pModel );
	_pModel = new CSfxModel;
	_pModel->SetSfx(szFileName);

	strcpy( _szName, szFileName );
	strcpy( _szFollowBoneName, "" );

	_kScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	_nMaxFrame = 0;

	// 최대 프레임을 찾는다 
	for(int i = 0; i < _pModel->m_pSfxBase->m_apParts.GetSize(); i++)
	{	
		for(int j = 0; j < _pModel->m_pSfxBase->Part( i )->m_apKeyFrames.GetSize(); j++)
		{
			// 각 파트의 프레임 수와 비교해서 최대 프레임을 찾음
			if(_nMaxFrame < _pModel->m_pSfxBase->Part( i )->Key(j)->nFrame)
				_nMaxFrame = _pModel->m_pSfxBase->Part( i )->Key(j)->nFrame;
		}
		
	}
}

void BsSfxUnit::Update( CModelObject* pParent )
{
	D3DXVECTOR3 kPos( 0.0f, 0.0f, 0.0f );
	if( pParent )
	{
		BOOL bOK = pParent->GetPosBone( &kPos, _szFollowBoneName );

		if( !bOK )
			pParent->GetPosBone( &kPos, 0 );

		_pModel->m_vPos = kPos;
	}
	
	//looping
	if( _nMaxFrame < _pModel->m_nCurFrame )
		_pModel->m_nCurFrame = 0;

	_pModel->Process( );
}

void BsSfxUnit::SetScale( const D3DXVECTOR3& kScale )
{
	_kScale = kScale;
	_pModel->m_vScale = kScale;
}