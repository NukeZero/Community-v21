#pragma once

class CObj;
struct GMOBJECT;
//@@ Class 설명 [YT ( 2012/ 10 / 29 ) ]
// 오브젝트의 텍스쳐 리스트를 보여주고 , 해당 텍스쳐를 업데이트 시켜준다.
// 이 클래스 첫 행동은 픽킹 시 Bind_Object 부터 시작한다.
class ObjectTextureView
{
public:
	struct GMOBJECT_INFO
	{
		GMOBJECT* pObject;
		int nTextureIndex;
	};

public:
	typedef std::map< std::string, GMOBJECT_INFO > Map_Data;   
	typedef std::vector< std::string  > Vec_Data;

public:
	ObjectTextureView();
	~ObjectTextureView();

	//				처음 픽킹시 해당 오브젝트를 셋팅한다. ( NULL 도 포함 )
	int				Bind_Object( CObj* pObject );

	//				이름으로 GMOBJECT_INFO를 찾는다.
	GMOBJECT_INFO*  FindTexture( const char* szFileName );

	//				해당 텍스쳐 객체를 연결과 해제를 시킨다.
	bool			Link_TexturePointer( const int nTextureIndex, const char* szSelectTextureName, const LPDIRECT3DTEXTURE9 lpNewTexture );
	int				UnLink_TexturePointer( const char* szFileName, const LPDIRECT3DTEXTURE9 lpDeleteTexture );

	const char*		GetTextureName( const int& nIndex );
	const char*		GetObjectName() { return m_strObejctName.c_str(); }
	const int		TextureNameCount() { return (int)m_VecTextureName.size(); }

private:
	//				COBJ 에서 텍스쳐 이름을 추출한다.
	bool			Extract_Texture_Name();

	//				[ Key : string ] : 텍스쳐 이름,   [ Data : GMOBJECT_INFO ] : GMOBJECT*, 해당 텍스쳐 ID
	void			Add_TextureData_Info( const char* szTextureName,  GMOBJECT_INFO object );

	//				텍스쳐 메니져에 새로운 텍스쳐 객체 포인터를 연결과 해제를 담당하는 함수.
	bool			Find_Shared_Texture_Link( const char* szFileName , const LPDIRECT3DTEXTURE9 lpNewTexture );
	bool			Find_Shared_Texture_Unlink( const char* szFileName , const LPDIRECT3DTEXTURE9 lpNewTexture  );

	//				텍스쳐 메니져에 삭제와, 삽입하는 함수.
	int				DeleteToTextureMgr( LPDIRECT3DTEXTURE9 lpDeleteTexture  );
	void			SetFromTextureMgr( const int nID, LPDIRECT3DTEXTURE9 lpNewTexture );

private:

	CObj* m_pObject;
	Vec_Data m_VecTextureName;
	Map_Data m_MapTextureData_Info;
	std::string m_strObejctName;
};