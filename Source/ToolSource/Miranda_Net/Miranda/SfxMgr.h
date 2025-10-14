#pragma once

#include "..\_Common\sfxbase.h"
#include "ObjMgr.h"
#include "..\_Common\File.h"

#include <vector>


// 이펙트 단위 클래스 
typedef struct stSfxUnit
{

	CSfxBase	*m_Base;
	CSfxModel	*m_Model;
	bool		*pIsLoop;
	int			*pId;
	UINT		*pHotKey;
	int			*pMaxFrame;
	int			*pInterval;
	int			*pFollowObj;
	bool		*pIsDeployed;

} stSfxUnit;                


// sfx관리자 
class CSfxMgr
{

	private:
		//int				  m_SfxIdValue;
		vector<stSfxUnit> m_vUnit;
		LPDIRECT3DDEVICE9 m_pDevice;
		CObjMgr*		  m_pObjMgr;
		
	public:

		CSfxMgr();
		~CSfxMgr();

		// 기능 함수 
		int		NewSfx(int nID);
		int		AddSfx(int nId, CString FileName);
		int		SetLoop(int nSfxId, bool bIsLoop);
		int		SetInterval(int nSfxId, int nInterval);
		int		SetHotKey(int nSfxId, UINT nKey);
		int		FollowObj(int nSfxId, int nObjId);
		int		SetWorld(int nSfxId, D3DXMATRIX* pWorldTM);
		
		int		SetFrame(int nSfxId, int nFrameIdx);
		int		AddFrame(int nSfxId, int nAmount);
		int		GetCurrentFrame(int nSfxId);
		int		GetMaxFrame(int nSfxId);
		
		int		SetPos(int nSfxId, D3DXVECTOR3 vPos);
		int		SetRotate(int nSfxId, D3DXVECTOR3 vRotate);
		int		DeploySfx(int nSfxId, D3DXVECTOR3 vPos);
		int		DeleteSfx(int nSfxId);
		bool	Save(CFileIO &File);
		int		Load(CFileIO &File);

		// 기본 함수 
		int		Init(LPDIRECT3DDEVICE9 currDevice);
		void	Update();
		int		Render();
		int		Destroy();
		int		GetObjCount();
		void	GetObjMgr(CObjMgr* pMgr);
		
};


//gmpbigsun ( 100121 )
//본에 링크되는 이펙트 및 루아로 빼기위해 새로운 이펙트 단위를 설정한다.

#define MAX_STRING_NUM 64
struct BsSfxUnit
{
	BsSfxUnit( ) 
	{
		_pModel = NULL;
	}

	~BsSfxUnit( )
	{
		SAFE_DELETE( _pModel );
	}

	void Init( const char* szFileName );

	void Render( LPDIRECT3DDEVICE9 pd3device )
	{
		_pModel->Render( pd3device );
	}

	void Update( CModelObject* pModel );
	void SetScale( const D3DXVECTOR3& kScale );
	void SetFollowBoneName( const char* szBoneName )		{ strcpy( _szFollowBoneName, szBoneName ); }
	
	CSfxModel* _pModel;
	TCHAR	_szName[ MAX_STRING_NUM ];
	TCHAR	_szFollowBoneName[ MAX_STRING_NUM ];
	D3DXVECTOR3 _kScale;
	int		_nMaxFrame;
};

typedef vector< BsSfxUnit* >		BsSfxContainer;
typedef BsSfxContainer::iterator	BsSfxIter;

