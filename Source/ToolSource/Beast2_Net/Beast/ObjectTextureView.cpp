#include "stdafx.h"
#include "ObjectTextureView.h"


ObjectTextureView::ObjectTextureView()
:m_pObject( NULL )
{
	m_VecTextureName.reserve( 30 );
}

ObjectTextureView::~ObjectTextureView()
{
	m_pObject = NULL;

	//실제 메모리 반환하는 방법은 빈 컨테이너를 Swap시켜주는 방법밖에없다.
	//한번 해봄 "STL Swap Trick 참조"
	Vec_Data( ).swap( m_VecTextureName );

	//m_VecTextureName.clear();
	m_MapTextureData_Info.clear();
}

int ObjectTextureView::Bind_Object( CObj* pObject )
{
	m_pObject = pObject;

	if( true != Extract_Texture_Name() )
		return -1;

	return ( m_VecTextureName.size() < 1 ) ? -1 : m_VecTextureName.size() - 1;
}

void ObjectTextureView::Add_TextureData_Info( const char* szTextureName,  GMOBJECT_INFO object )
{
	if( FindTexture( szTextureName  ) )
		return ;

	m_MapTextureData_Info.insert( Map_Data::value_type( szTextureName, object )  );
	m_VecTextureName.push_back( szTextureName );
}

ObjectTextureView::GMOBJECT_INFO* ObjectTextureView::FindTexture( const char* szFileName )
{
	if( m_MapTextureData_Info.empty() )
		return NULL;

	Map_Data::iterator it = m_MapTextureData_Info.find( szFileName  );

	if( it != m_MapTextureData_Info.end() )
		return &it->second;

	return NULL;
}

const char* ObjectTextureView::GetTextureName( const int& nIndex )
{
	if( m_VecTextureName.empty() )
		return "";

	return m_VecTextureName.at( nIndex ).c_str();
}

bool ObjectTextureView::Extract_Texture_Name()
{
	m_VecTextureName.clear();
	m_MapTextureData_Info.clear();
	m_strObejctName = "";

	if( !m_pObject )
		return false;
	
	CModelObject* pModelObject = static_cast< CModelObject* >( m_pObject->m_pModel );

	DWORD dwType = m_pObject->GetType();

	m_strObejctName = pModelObject->m_pModelElem->m_szName;

	for( int oi = 0 ; oi < MAX_ELEMENT ; ++oi )
	{
		CObject3D* pObject3d = pModelObject->GetObject3D( oi );

		if( !pObject3d )
			continue;

		for( int gi = 0 ;gi < pObject3d->GetMaxObject(); ++gi )
		{
			GMOBJECT* pGmObj = pObject3d->GetGMOBJECT( gi ) ;

			if( !pGmObj )
				continue;

			std::string strCurrent = "";
			for( int i = 0; i < pGmObj->m_nMaxMtrlBlk; i ++ )
			{
				int nID = pGmObj->m_pMtrlBlk[ i ].m_nTextureID;

				strCurrent = pGmObj->m_MaterialAry[ nID ].strBitMapFileName;

				GMOBJECT_INFO info;
				info.nTextureIndex = nID;
				info.pObject = pGmObj;
				Add_TextureData_Info( strCurrent.c_str(), info );
			}
		}
	}


	return true;
}

int ObjectTextureView::DeleteToTextureMgr( LPDIRECT3DTEXTURE9 lpDeleteTexture  )
{
	return  g_TextureMng.DeleteTexture( lpDeleteTexture );
}

void ObjectTextureView::SetFromTextureMgr( const int nID, LPDIRECT3DTEXTURE9 lpNewTexture )
{
	g_TextureMng.SetTexture( nID, lpNewTexture );
}

bool ObjectTextureView::Link_TexturePointer( const int nTextureIndex, const char* szSelectTextureName, const LPDIRECT3DTEXTURE9 lpNewTexture )
{
	if( nTextureIndex < 0 )
		return false;

	SetFromTextureMgr( nTextureIndex, lpNewTexture );
	return Find_Shared_Texture_Link( szSelectTextureName, lpNewTexture );
}

int ObjectTextureView::UnLink_TexturePointer( const char* szFileName , const LPDIRECT3DTEXTURE9 lpDeleteTexture )
{
	this->Find_Shared_Texture_Unlink( szFileName,  lpDeleteTexture );
	return this->DeleteToTextureMgr( lpDeleteTexture );
}


//@@ HACK [ YT 2012 / 10/ 21 ] 
// 같은 이름의 텍스쳐 객체 포인터는 같은건줄 알았지만,
// 다른 포인터로 할당한경우가 있다, 그래서 선택된 텍스쳐 포인터와 같은 
// 오브젝트 ID 와 텍스쳐 ID를 Link_Index에 넣어서 처리하는 식으로 바뀌었다....-_-;

bool ObjectTextureView::Find_Shared_Texture_Unlink( const char* szFileName , const LPDIRECT3DTEXTURE9 lpNewTexture )
{
	typedef CTextureManager::LINK_GMOBJECT::Link_Index Link_Index;

	CTextureManager::LINK_GMOBJECT* pLink_GmOBejctList = g_TextureMng.Find_Link_GmObject( szFileName );

	if( !pLink_GmOBejctList )
		return false;

	int nGmObjectCount = pLink_GmOBejctList->vecData.size();

	for( int nGMI = 0 ; nGMI < nGmObjectCount; ++nGMI )
	{
		GMOBJECT* pObject = pLink_GmOBejctList->vecData.at( nGMI );

		for( int nMi = 0; nMi < pObject->m_nMaxMtrlBlk; nMi ++ )
		{
			int nTID = pObject->m_pMtrlBlk[ nMi ].m_nTextureID;

			if( lpNewTexture != pObject->m_pMtrlBlkTexture[ nTID ] )
				continue;

			pObject->m_pMtrlBlkTexture[ nTID ] = NULL;

			Link_Index link_Index ( nGMI, nTID );
			pLink_GmOBejctList->vecIndex.push_back( link_Index );
		}
	}

	return true;
}

bool ObjectTextureView::Find_Shared_Texture_Link( const char* szFileName , const LPDIRECT3DTEXTURE9 lpNewTexture )
{
	if( !lpNewTexture )
		return false;

	typedef CTextureManager::LINK_GMOBJECT::Link_Index Link_Index;

	CTextureManager::LINK_GMOBJECT* pLink_GmObjectList = g_TextureMng.Find_Link_GmObject( szFileName );

	if( !pLink_GmObjectList )
		return false;

	int nSharedTextureCount = (int)pLink_GmObjectList->vecIndex.size();

	for( int nSTI = 0 ; nSTI < nSharedTextureCount; ++nSTI )
	{
		Link_Index* pLink_Index  = &pLink_GmObjectList->vecIndex.at( nSTI );

		int nGmObejctIndex = pLink_Index->nGmObjectIndex;
		int nTextureIndex =  pLink_Index->nTextureIndex;

		GMOBJECT* pGmObject = pLink_GmObjectList->vecData.at( nGmObejctIndex );
		pGmObject->m_pMtrlBlkTexture[ nTextureIndex ] = lpNewTexture;
	}

	return true;
}
