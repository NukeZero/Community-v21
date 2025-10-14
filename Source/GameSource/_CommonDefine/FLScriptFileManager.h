
#ifndef __FLSCRIPTFILEMANAGER_H__
#define __FLSCRIPTFILEMANAGER_H__


class FLScriptFileManager
{
	public:
		FLScriptFileManager();
		~FLScriptFileManager();

		
		static FLScriptFileManager* GetInstance();


		void	SetScriptPath( /*const TCHAR* pszResourcePath*/ );

		bool	LoadMoveFileList( const TCHAR* pszFileName ) const;

//		DWORD	GetOrder( const TCHAR* pszKey ) const;

		void	GetScriptFileFullPath( TCHAR* pszFullPath, size_t size, const TCHAR* pszFileName ) const;


	private:
		bool	CreateDir( const TCHAR* pszPath, const TCHAR* pszDirName ) const;

		bool	Move( const TCHAR* pszFileName/*, const TCHAR* pszDestPath*/ ) const;
		bool	Delete( const TCHAR* pszFileName ) const;



	private:
		TCHAR	m_szResourcePath[ _MAX_PATH ];
		TCHAR	m_szScriptPath[ _MAX_PATH ];

//		enum E_ORDER { E_NONE = 0, E_MOVE, E_DELETE };
};

#define		g_pScriptFileManager		FLScriptFileManager::GetInstance()

#endif // __FLSCRIPTFILEMANAGER_H__
