#include "stdafx.h"

#include <stdio.h>
#include "modelSfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if 0
using namespace std ;

CSfxObj::CSfxObj() 
{ 
	m_bDelete = FALSE; 
	m_pModel = NULL; 
	m_fAngle = 0.0f; 
	m_vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ); 
}
CSfxObj::~CSfxObj() 
{ 
}
CModel* CSfxObj::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex ) 
{
	return ( m_pModel = prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex ) );
}
void CSfxObj::Process() 
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////
///////////
///////////
///////////
CSfxParticleMng::CSfxParticleMng()
{
}
CSfxParticleMng::~CSfxParticleMng()
{
}
void CSfxParticleMng::Process()
{
	list<CSfxObj*>::iterator i;
	for( i = m_listParticle.begin(); i != m_listParticle.end(); ++i )
	{
		(*i)->Process();
		if( (*i)->m_bDelete )
		{
			delete *i;
			m_listParticle.erase( i-- );
		}

	}
}


CSfxJetParticleMng::CSfxJetParticleMng()
{
}
CSfxJetParticleMng::~CSfxJetParticleMng()
{
}
void CSfxJetParticleMng::GenerateParticle( D3DXVECTOR3* pvPos )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////
///////////
///////////
///////////

CSfxParticle::CSfxParticle(WORD nAppear,WORD nKeep,WORD nDisappear,D3DXVECTOR3 vPos,D3DXVECTOR3 vDirection,PARTICLEACCELTYPE nAccelType,D3DXVECTOR3 vAccel,FLOAT fAccel)
{
	m_nFrame=0;
	m_nAppear=nAppear;
	m_nKeep=nKeep;
	m_nDisappear=nDisappear;

	m_vPos = vPos;
	m_vDirection=vDirection;

	m_nAccelType=nAccelType;
	m_vAccel=vAccel;
	m_fAccel=fAccel;
	m_nEffect=0;
}
CSfxParticle::~CSfxParticle()
{
}
void CSfxParticle::Process()
{
	DWORD nBlendFactor=255;
	if(m_nFrame<m_nAppear)
		nBlendFactor=255*m_nFrame/m_nAppear;
	else if(m_nFrame<m_nKeep)
		nBlendFactor=255;
	else if(m_nFrame<m_nDisappear)
		nBlendFactor=255-255*(m_nFrame-m_nKeep)/(m_nDisappear-m_nKeep);
	else
		m_bDelete = TRUE;
	//(	m_nFrame > 60 ) m_bDelete = TRUE;
	if(nBlendFactor<255)
	{
		nBlendFactor=nBlendFactor;
	}
	m_pModel->SetBlendFactor(nBlendFactor);

	m_vPos +=	m_vDirection;
	switch(m_nAccelType) {
		case PARTICLEACCELTYPE_VECTOR:
			{
				m_vDirection+=m_vAccel;
			}
			break;
		case PARTICLEACCELTYPE_FACTOR:
			{
				m_vDirection*=m_fAccel;
			}
			break;
	}
	m_nFrame++;
}

////////////////////////////////////////////////////////////////////////////////
// ¸Å½¬ 

CSfxMesh::CSfxMesh()
{
}
CSfxMesh::~CSfxMesh()
{
}
void CSfxMesh::Process()
{
	m_pModel->FrameMove();
}

////////////////////////////////////////////////////////////////////////////////
// ModelSfx
//

int CModelSfx::LoadModel( LPCTSTR szFileName )
{
	return 1;
}
CModelSfx::CModelSfx()
{
}
CModelSfx::~CModelSfx()
{
}
/*
void CModelSfx::GenerateParticle( D3DXVECTOR3* pvPos ) 
{
	D3DXVECTOR3 vPos = *pvPos;
	DWORD nAngle=rand(); FLOAT fSpeed=((float)(rand()%100)+100.0f)/2000.0f; FLOAT fSpeedY=((float)(rand()%100)+200.0f)/2000.0f;
	FLOAT dx=sin(nAngle)*fSpeed;
	FLOAT dz=cos(nAngle)*fSpeed;
	CSfxParticle* pObj = new CSfxParticle(0,30,60,vPos,D3DXVECTOR3(dx,fSpeedY,dz),PARTICLEACCELTYPE_VECTOR,D3DXVECTOR3(0,-0.005f,0),0);
	pObj->m_vScale = D3DXVECTOR3(3.5f,3.5f,3.5f);
	pObj->m_pModel = prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_SFX, XI_DMG_EFFECT03 );
	m_listParticle.insert( m_listParticle.end(), pObj );
}*/
void CModelSfx::AddSfxObj( CSfxObj* pSfxObj )
{
	m_listSfx.insert( m_listSfx.end(), pSfxObj );
}
void CModelSfx::FrameMove( D3DXVECTOR3 *pvSndPos )
{

	list<CSfxObj*>::iterator i;
	for( i = m_listSfx.begin(); i != m_listSfx.end(); ++i )
	{
		(*i)->Process();
		if( (*i)->m_bDelete )
		{
			delete *i;
			m_listSfx.erase( i-- );
		}
	}
}

#endif