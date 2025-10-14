
#pragma once 

typedef std::map< std::string, CTexture* >	CMapTexture;
typedef CMapTexture::value_type		MapTexType;
typedef CMapTexture::iterator		MapTexItor;
typedef std::vector<std::string>				VEC_LOADFAILED_FILENAME;

//---------------------------------------------------------------------------------------------
// 텍스쳐 메니져
//---------------------------------------------------------------------------------------------
class CTextureMng
{

public:

									CTextureMng();
									~CTextureMng();

	void							Invalidate( void );
	BOOL							DeleteDeviceObjects( void );

	CTexture*						AddTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor = 0xFFFF00FF, BOOL bMyLoader = FALSE );
	CTexture*						AddTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pKey, CTexture* pTexture );

	BOOL							RemoveTexture( LPCTSTR pKey );

	BOOL							SetInvalidate(LPDIRECT3DDEVICE9 pd3dDevice);
	CTexture*						GetAt( LPCTSTR pFileName );


private:


	BOOL							LoadDefaultTexture( void );
	BOOL							IsLoadFailedTexture( LPCTSTR pFileName );
	

public:

	CTexture*						m_pDefaultTexture;
	CMapTexture						m_mapTexture;
	VEC_LOADFAILED_FILENAME			m_vecLoadFailedFileName;
};


