

#include "StdAfx.h"
#include "TextureManager.h"
#include "targa.h"


//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CTextureMng::CTextureMng()
:	m_pDefaultTexture( NULL )
{

}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CTextureMng::~CTextureMng()
{
	DeleteDeviceObjects();
	m_vecLoadFailedFileName.clear();
}


//---------------------------------------------------------------------------------------------
// 무효화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CTextureMng::Invalidate()
{
	for( MapTexItor i = m_mapTexture.begin(); i != m_mapTexture.end(); ++i )
	{
		CTexture* pTexture = ((*i).second);
		FLERROR_ASSERT_LOG_DO( pTexture, continue, _T("[텍스쳐 메니져] 텍스쳐 포인터가 유효하지 않습니다 : [%s]"), (*i).first );

		pTexture->Invalidate();
	}
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 삭제 ( 디바이스에 종속적인 텍스쳐 삭제 )
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CTextureMng::DeleteDeviceObjects()
{
	if( m_mapTexture.empty() )
		return TRUE;

	for( MapTexItor i = m_mapTexture.begin(); i != m_mapTexture.end(); ++i )
	{
		CTexture* pTexture = ((*i).second);
		FLERROR_ASSERT_LOG_DO( pTexture, continue, _T("[텍스쳐 메니져] 텍스쳐 포인터가 유효하지 않습니다 : [%s]"), (*i).first );

		SAFE_DELETE( pTexture );
		(*i).second = NULL;
	}

	m_mapTexture.clear();

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 기본 텍스쳐 불러오기
// param	: ...
// return	: CTexture* 
//---------------------------------------------------------------------------------------------
BOOL CTextureMng::LoadDefaultTexture()
{
	if( m_pDefaultTexture )
		return TRUE;

	std::string strFullPath( _T("") );

	strFullPath += DIR_THEME;
	strFullPath += DIR_DEFAULT;
	strFullPath += _T("HyperlinkUserMark.bmp");

	m_pDefaultTexture = new CTexture();
	FLERROR_ASSERT_LOG_RETURN( m_pDefaultTexture, FALSE, _T("[텍스쳐 메니져] 기본 텍스쳐 할당 실패.") );

	if( m_pDefaultTexture->LoadTexture( g_Neuz.m_pd3dDevice, strFullPath.c_str(), 0XFF000000, FALSE ) )
	{
		bool bResult( m_mapTexture.insert( MapTexType( strFullPath.c_str(), m_pDefaultTexture ) ).second );

		if( bResult == true )
			return TRUE;
		else
		{
			SAFE_DELETE( m_pDefaultTexture );
			FLERROR_ASSERT_LOG_RETURN( m_pDefaultTexture, FALSE, _T("[텍스쳐 메니져] 텍스쳐 삽입에 실패하였습니다 : [%s]"), strFullPath.c_str() );
		}
	}
	else
	{
		SAFE_DELETE( m_pDefaultTexture );
		m_vecLoadFailedFileName.push_back( strFullPath );
		FLERROR_ASSERT_LOG_RETURN( m_pDefaultTexture, FALSE, _T("[텍스쳐 메니져] 텍스쳐 볼러오기를 실패하였습니다 : [%s]"), strFullPath.c_str() );
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 로딩에 실패한 적이 있는 텍스쳐 인가 ?
// param	: ...
// return	: CTexture* 
//---------------------------------------------------------------------------------------------
BOOL CTextureMng::IsLoadFailedTexture( LPCTSTR pFileName )
{
	if( m_vecLoadFailedFileName.empty() )
		return FALSE;

	for( UINT nIndex = 0; nIndex < m_vecLoadFailedFileName.size(); nIndex++ )
	{
		if( m_vecLoadFailedFileName[nIndex] == pFileName )
			return TRUE;
	}

	return FALSE;
}


//---------------------------------------------------------------------------------------------
// 텍스쳐 추가
// param	: ...
// return	: CTexture* 
//---------------------------------------------------------------------------------------------
CTexture* CTextureMng::AddTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader )
{
	CTexture* pTexture( NULL );

	LoadDefaultTexture();
	
	MapTexItor mapTexItor = m_mapTexture.find( pFileName );
	
	if( mapTexItor != m_mapTexture.end() )
	{
		pTexture = (*mapTexItor).second;
		FLERROR_ASSERT_LOG_RETURN( pTexture, m_pDefaultTexture, _T("[텍스쳐 메니져] 텍스쳐를 찾았으나 포인터가 유효하지 않습니다 : [%s]"), pFileName );
		
		return pTexture;
	}
	 
	if( IsLoadFailedTexture( pFileName ) )
		return NULL;

	pTexture = new CTexture();
	FLERROR_ASSERT_LOG_RETURN( pTexture, m_pDefaultTexture, _T("[텍스쳐 메니져] 텍스쳐 할당 실패.") );

	if( pTexture->LoadTexture( pd3dDevice, pFileName, d3dKeyColor, bMyLoader ) )
	{
		bool bResult( m_mapTexture.insert( MapTexType( pFileName, pTexture ) ).second );

		if( bResult == true )
			return pTexture;
		else
		{
			SAFE_DELETE( pTexture );
			FLERROR_ASSERT_LOG_RETURN( pTexture, m_pDefaultTexture, _T("[텍스쳐 메니져] 텍스쳐 삽입에 실패하였습니다 : [%s]"), pFileName );
		}
	}
	else
	{
		SAFE_DELETE( pTexture );
		m_vecLoadFailedFileName.push_back( pFileName );
		FLERROR_ASSERT_LOG_RETURN( pTexture, m_pDefaultTexture, _T("[텍스쳐 메니져] 텍스쳐 볼러오기를 실패하였습니다 : [%s]"), pFileName );
	}

	return NULL;
}


//---------------------------------------------------------------------------------------------
// 텍스쳐 추가
// param	: ...
// return	: CTexture* 
//---------------------------------------------------------------------------------------------
CTexture* CTextureMng::AddTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pKey, CTexture* pTexture )
{
	MapTexItor mapTexItor = m_mapTexture.find( pKey );
	if( mapTexItor != m_mapTexture.end() )
		return (*mapTexItor).second;

	m_mapTexture.insert( MapTexType( pKey, pTexture ) );

	return pTexture;
}


//---------------------------------------------------------------------------------------------
// 텍스쳐 제거
// param	: 키 문자열
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CTextureMng::RemoveTexture( LPCTSTR pKey )
{
	if( m_mapTexture.empty() )
		return TRUE;

	MapTexItor mapTexItor = m_mapTexture.find( pKey );

	if( mapTexItor != m_mapTexture.end() )
	{
		SAFE_DELETE( (*mapTexItor).second );
		m_mapTexture.erase( pKey );
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 무효화 설정
// param	: 그래픽 장치 
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CTextureMng::SetInvalidate(LPDIRECT3DDEVICE9 pd3dDevice)
{
	for( MapTexItor i = m_mapTexture.begin(); i != m_mapTexture.end(); ++i )
	{
		CTexture* pTexture = ((*i).second);
		FLERROR_ASSERT_LOG_DO( pTexture, continue, _T("[텍스쳐 메니져] 텍스쳐 포인터가 유효하지 않습니다 : [%s]"), (*i).first );

		pTexture->SetInvalidate(pd3dDevice);
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 텍스쳐 얻기
// param	: 파일 이름
// return	: CTexture*
//---------------------------------------------------------------------------------------------
CTexture* CTextureMng::GetAt( LPCTSTR pFileName )
{
	MapTexItor mapTexItor = m_mapTexture.find( pFileName );

	if( mapTexItor != m_mapTexture.end() )
	{
		CTexture* pTexture = (*mapTexItor).second;
		FLERROR_ASSERT_LOG_RETURN( pTexture, m_pDefaultTexture, _T("[텍스쳐 메니져] 텍스쳐 얻기 실패하였습니다 : [%s]"), pFileName );

		return (*mapTexItor).second;
	}

	return NULL;
}

