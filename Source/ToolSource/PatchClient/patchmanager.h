#ifndef _PATCH_MANAGER_H_
#define _PATCH_MANAGER_H_

#include <crypto/FLDes.h>


//패치 파일 정보 구조체
typedef struct _T_FILE_INFO
{
	bool		bDir;						// 디렉토리 체크
	DWORD		dwFileSize;					// 파일 크기	
	__int64		n64FileTime;				// 파일 최종 수정시간	
	TCHAR		szPath[MAX_PATH];			// Local Path
	TCHAR		szServerPath[MAX_PATH];		// CDN Path
} T_FILE_INFO, *PT_FILE_INFO;


enum MAKEPATCHLIST_RESULT
{
	MAKEPATCHLIST_FILE_NOTFOUND,
	MAKEPATCHLIST_VERSION_MISMATCH,
	MAKEPATCHLIST_OK
};


typedef list<T_FILE_INFO> FileContainer;
typedef FileContainer::iterator FileContainerIterator;

class CPatchManager
{
public:
	CPatchManager();
	~CPatchManager();

private:
	TCHAR					m_szCurrentDirectory[MAX_PATH];	//사용자 현재 디렉토리										
	
public:
	FileContainer			m_files;								// 파일 벡터
	
public:
	LPCTSTR					GetCurrentLocalDirectory() { return m_szCurrentDirectory; }
	void					SetCurrentLocalDirectory();
	MAKEPATCHLIST_RESULT	MakePatchList( int nVersion );
	void					EnqueueFILE( const TCHAR* szFile, bool bDir, DWORD dwSize, __int64 n64FileTime );
	size_t					GetListCount() { return m_files.size(); }

private:
	bool					IsSameFile( T_FILE_INFO& tFileInfo );
	void					CheckFiles();

	void					InitKey();
	void					Decrypt( const TCHAR* szIn, TCHAR* szOut );


	FL_des3_context			m_tCtx3;
};

#endif