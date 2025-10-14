#include "stdafx.h"

typedef	__int64	EXPINTEGER;

#include "File.h"
#include "Scanner.h"
#include <time.h>

#ifdef __SECURITY_0628
#include <wincrypt.h>
void md5(char* out, const BYTE* pbContent, DWORD cbContent)
{
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;
	BYTE bHash[0x7f];
	DWORD dwHashLen = 16;	// The MD5 algorithm always returns 16 bytes.
	*out = '\0';

	if (CryptAcquireContext(&hCryptProv,
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
	{
		if (CryptCreateHash(hCryptProv,
			CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
			0, 0, &hHash))
		{
			if (CryptHashData(hHash, pbContent, cbContent, 0))
			{
				if (CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
				{
					// Make a string version of the numeric digest value
					char tmp[4];
					for (int i = 0; i < 16; i++)
					{
						sprintf(tmp, "%02x", bHash[i]);
						strcat(out, tmp);
					}
				}
			}
		}
	}
	CryptDestroyHash(hHash);
	CryptReleaseContext(hCryptProv, 0);
}

void md5(char* out, const char* in)
{
	DWORD cbContent = lstrlen(in);
	BYTE* pbContent = (BYTE*)in;
	md5(out, pbContent, cbContent);
}
#endif	// __SECURITY_0628


enum MAKEPATCHLIST_RESULT
{
	MAKEPATCHLIST_FILE_NOTFOUND,
	MAKEPATCHLIST_VERSION_MISMATCH,
	MAKEPATCHLIST_OK
};

typedef struct tagFILE_INFO
{
	CString FilePath;
	int FileSize;
	CTime m_mtime;
}FILE_INFO;

typedef vector<FILE_INFO> FileContainer;
typedef vector<FILE_INFO>::iterator FileContainerIterator;
#define RESOURCE_VERSION "\"V0.01\""

FileContainer				g_fv;
BOOL						g_bOutsideMerge = TRUE;
char						g_strArgumentFileName[] = ".\\resource.txt";
CString						g_strVersion;
BYTE						g_byEncryptionKey = 0x57;
BOOL						g_bSync = FALSE;
map< string, FILETIME >		g_map;

#ifdef __SECURITY_0628
map<string, string>	s_mapAuth;
void	MakeClientAuthFile(void);
void	MakeServerAuthFile(void);
void	MakeAuthFile(void);
BOOL IsDataFile(CString& szFileName);
void AddAuth(CString& szFileName);
#endif	// __SECURITY_0628

int						MergeFile(CString szFileName, bool bEncryption);
MAKEPATCHLIST_RESULT	MakePatchList(int nVersion);

BOOL _GetStatus(LPCTSTR lpszFileName, CFileStatus& rStatus)
{
	extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

	// attempt to fully qualify path first
	if (!AfxFullPath(rStatus.m_szFullName, lpszFileName))
	{
		rStatus.m_szFullName[0] = '\0';
		return FALSE;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)lpszFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	VERIFY(FindClose(hFind));

	// strip attribute of NORMAL bit, our API doesn't have a "normal" bit.
	rStatus.m_attribute = (BYTE)
		(findFileData.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL);

	// get just the low DWORD of the file size
	ASSERT(findFileData.nFileSizeHigh == 0);
	rStatus.m_size = (LONG)findFileData.nFileSizeLow;

	// 모든 단위는 GMT로 
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&findFileData.ftLastWriteTime, &sysTime);
//	sysTime.wSecond = 0;
	sysTime.wMilliseconds = 0;

	rStatus.m_ctime = 0;
	rStatus.m_atime = 0;
	rStatus.m_mtime = CTime(sysTime, 0); // CTime의 FILETIME용 constructor는 localtime으로 변환을 하니까 사용하지 않는다.

	return TRUE;
}
/*
{
	struct _finddata_t c_file;
	long hFile;

	TCHAR drive[ 64 ], dir[ 64 ], name[ 64 ], ext[ 64 ];
	_splitpath( s.token, drive, dir, name, ext );
	if( (hFile = _findfirst( s.token, &c_file )) != -1L )
	{
		strcpy( name, dir );
		strcat( name, c_file.name );
		if( !(c_file.attrib & _A_SUBDIR ) )
		{
			InsertFile.m_mtime = c_file.time_write;
			InsertFile.FilePath = name;
			InsertFile.FileSize = c_file.size;
			g_fv.push_back( InsertFile );
		}
		while( _findnext( hFile, &c_file ) == 0 )
		{
			strcpy( name, dir );
			strcat( name, c_file.name );
			if( !(c_file.attrib & _A_SUBDIR ) )
			{
				InsertFile.m_mtime = c_file.time_write;
				InsertFile.FilePath = name;
				InsertFile.FileSize = c_file.size;
				g_fv.push_back( InsertFile );
			}
		}
		_findclose( hFile );
	}
}
*/
int InterpriteScript(CScanner& s)
{
	s.GetToken(); // {
	while (*s.token != '}')
	{
		//strFileName += "%05d"
		CFile file;
		CFileStatus fileStatus;

		FILE_INFO InsertFile;
		if (s.Token == "count")
		{
			s.GetToken();
			CScanner scanner;
			TCHAR lpszCount[256];
			TCHAR lpszTemp[256];
			strcpy(lpszTemp, s.Token);
			CString strFileName;
			CString strMaster;
			strcpy(lpszCount, lpszTemp);
			LPTSTR lpCurrent;
			lpCurrent = strstr(lpszCount, "(");
			int nCount = 0;
			int anBegin[6];
			int anEnd[6];
			while (lpCurrent)
			{
				scanner.SetProg(lpCurrent);
				scanner.GetToken(); // (
				int nNum = scanner.GetNumber();
				scanner.GetToken(); // ,
				anBegin[nCount] = scanner.GetNumber();
				scanner.GetToken(); // ,
				anEnd[nCount] = scanner.GetNumber();
				scanner.GetToken(); // )
				*lpCurrent = 0;
				strFileName.Format("%s%s%dd", lpszCount, "%0", nNum);
				strcpy(lpszCount, scanner.m_pProg);
				strMaster += strFileName;
				lpCurrent = strstr(lpszCount, "(");
				nCount++;
			}
			strMaster += lpszCount;
			if (nCount == 1)
			{
				for (int i = anBegin[0]; i <= anEnd[0]; i++)
				{
					strFileName.Format(strMaster, i);
					if (_GetStatus(strFileName, fileStatus) == TRUE)
					{
						InsertFile.m_mtime = fileStatus.m_mtime;
						InsertFile.FilePath = strFileName;
						InsertFile.FileSize = (int)(fileStatus.m_size);
						g_fv.push_back(InsertFile);
					}
				}
			}
			if (nCount == 2)
			{
				for (int i = anBegin[1]; i <= anEnd[1]; i++)
				{
					for (int j = anBegin[0]; j <= anEnd[0]; j++)
					{
						strFileName.Format(strMaster, j, i);
						if (_GetStatus(strFileName, fileStatus) == TRUE)
						{
							InsertFile.m_mtime = fileStatus.m_mtime;
							InsertFile.FilePath = strFileName;
							InsertFile.FileSize = (int)(fileStatus.m_size);
							g_fv.push_back(InsertFile);
						}
					}
				}
			}
		}
		if (s.Token == "file")
		{
			s.GetToken();

			if (_GetStatus(s.token, fileStatus) == TRUE)
			{
				InsertFile.m_mtime = fileStatus.m_mtime;
				InsertFile.FilePath = s.Token;
				InsertFile.FileSize = (int)(fileStatus.m_size);
				g_fv.push_back(InsertFile);
			}
			else
			{
				CString string;
				string.Format("%s not found.", s.Token);
				AfxMessageBox(string);
			}
		}
		if (s.Token == "wildcard")
		{
			s.GetToken();

			TCHAR drive[64], dir[64], name[64], ext[64];
			_splitpath(s.token, drive, dir, name, ext);

			CFileFind finder;
			BOOL bWorking = finder.FindFile(s.token);
			while (bWorking)
			{
				bWorking = finder.FindNextFile();

				if (finder.IsDirectory())
					continue;

				if (finder.GetFileName() == "vssver.scc")
					continue;

				strcpy(name, dir);
				strcat(name, (LPCTSTR)finder.GetFileName());

				FILETIME ft;
				finder.GetLastWriteTime(&ft);
				SYSTEMTIME sysTime;
				FileTimeToSystemTime(&ft, &sysTime);
//				sysTime.wSecond = 0;
				sysTime.wMilliseconds = 0;

				InsertFile.m_mtime = CTime(sysTime, 0);
				InsertFile.FilePath = name;
				InsertFile.FileSize = (int)(finder.GetLength());
				g_fv.push_back(InsertFile);
			}
			finder.Close();
		}
		s.GetToken(); // }
	}
	return 1;
}

BOOL FindFileFromResource(LPCTSTR lpszFileName, CMapStringToPtr& mapStringToPtr)
{
	int nFileHeaderSize = 0;
	int nFileNumber = 0;
	int nFileNameLength = 0;
	char szFileName[_MAX_FNAME] = "";
	int nFileSize = 0;
	int nFilePosition = 0;
	bool m_bEncryption;
	BYTE byEncryptionKey;

	CFile file;
	if (file.Open(lpszFileName, CFile::modeRead) == FALSE)
		return FALSE;

#ifdef __one_byte
	unsigned char useless;
	file.Read(&useless, sizeof(useless));
#endif
	file.Read(&byEncryptionKey, sizeof(byEncryptionKey));
	file.Read(&m_bEncryption, sizeof(m_bEncryption));
	file.Read(&nFileHeaderSize, sizeof(int));

	char* pHeader = new char[nFileHeaderSize];
	// 해더는 암호 기본 
	//if( m_bEncryption ) // 암호화 되었다면
	{
		char* pCryptHeader = new char[nFileHeaderSize];
		file.Read(pCryptHeader, nFileHeaderSize);

		for (int i = 0; i < nFileHeaderSize; i++)
		{
			pHeader[i] = CResFile::Decryption(g_byEncryptionKey, pCryptHeader[i]);
		}

		delete[] pCryptHeader;
	}
	//else // 암호화 안되었다면
	//{
	//	file.Read( pHeader, nFileHeaderSize );
	//}
	int nHeaderPosition = 0;
	char strVersion[8] = "";
	memcpy(strVersion, &pHeader[nHeaderPosition], 7); nHeaderPosition += 7;
	//FILEOUT( "c:\\debug.txt", "1 %s %s %d \n", lpszFileName, strVersion, nHeaderPosition );

	memcpy(&nFileNumber, &pHeader[nHeaderPosition], sizeof(short)); nHeaderPosition += sizeof(short);
//	FILEOUT( "c:\\debug.txt", "1 -1 %s %s %d \n", lpszFileName, strVersion, nFileNumber );

	time_t time_;
	for (int i = 0; i < nFileNumber; i++)
	{
	//	FILEOUT( "c:\\debug.txt", "2 %s %s %d \n", lpszFileName, szFileName, nFileNumber );

		memcpy(&nFileNameLength, &pHeader[nHeaderPosition], sizeof(short)); nHeaderPosition += sizeof(short);
		memcpy(szFileName, &pHeader[nHeaderPosition], nFileNameLength); nHeaderPosition += nFileNameLength;
		memcpy(&nFileSize, &pHeader[nHeaderPosition], sizeof(int)); nHeaderPosition += sizeof(int);
		memcpy(&time_, &pHeader[nHeaderPosition], sizeof(time_t)); nHeaderPosition += sizeof(time_t);
		memcpy(&nFilePosition, &pHeader[nHeaderPosition], sizeof(int)); nHeaderPosition += sizeof(int);
		FILE_INFO* pFileInfo = new FILE_INFO;
		pFileInfo->FilePath = szFileName;
		pFileInfo->m_mtime = time_;
		_strlwr(szFileName);
		mapStringToPtr.SetAt(szFileName, pFileInfo);
		ZeroMemory(szFileName, sizeof(szFileName));
	}
	delete[] pHeader;

	file.Close();
	return TRUE;
}

int MergeFile(CString szFileName, bool bEncryption)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char filename[_MAX_PATH];

	// 헤더 크기 구함 시작
	int nMergeFileNumber = g_fv.size();
	int nMergeFileHeaderSize = 0;
	// version + nMergeFileNumber + ( nMergeFileNumber * ( 
	nMergeFileHeaderSize += 7;
	nMergeFileHeaderSize += sizeof(short) + (nMergeFileNumber * (sizeof(short) + sizeof(int) + sizeof(int) + sizeof(time_t)));

	// 날짜 비교
	BOOL m_bModified = FALSE;
	FILE_INFO* pFileInfo;
	CTime timeCur, timeRes;

	CMapStringToPtr mapStringToPtr;
	FindFileFromResource(szFileName, mapStringToPtr);

	// 파일 갯수가 다르면 변경!
	if (mapStringToPtr.GetCount() != g_fv.size())
	{
//		printf( "diff - file count\n" );
		m_bModified = TRUE;
	}

	FileContainerIterator fvi = g_fv.begin();
	for (; fvi != g_fv.end(); ++fvi)
	{
		_splitpath((*fvi).FilePath, drive, dir, fname, ext);
		wsprintf(filename, "%s%s", fname, ext);
		// ( *fvi ).FilePath를 lower로 만들면 안된다. 저장할 때는 오리지날 스트링 그대로 저장하기 위해서임.
		// 왜 오리지날 스트링을 그대로 저장하나? 그냥. 오리지날로 저장하고 싶어서.
		_strlwr(filename);

		timeCur = (*fvi).m_mtime;
		// 기존 리소스에서 새 머지 파일을 찾을 수 없다. 파일이 새로 추가된 경우다. 변경 되었음.
		// mapStringToPtr의 filename은 lower로 되어 있다.
		if (mapStringToPtr.Lookup(filename, (void*&)pFileInfo) == FALSE)
		{
//			printf( "diff - lookup %s\n" , filename);
			m_bModified = TRUE;
		}
		else
		{
			// 찾았으면 제거하자. 나머지 카운트가 필요하다.
			mapStringToPtr.RemoveKey(filename);
			// 같은 파일을 찾았다. 그런데 두 파일이 시간의 최종 갱신 시간이 다른가? 다르면 변경!
			timeRes = pFileInfo->m_mtime;
			// 최종 갱신 시간이 다르고 소스 세이프 파일이 아니어야 수정된 것이다.
			if (timeCur != timeRes && strcmp(filename, "vssver.scc"))
			{
//				printf( "diff - time %s  res:%s sys:%s\n" , filename, 
//															timeRes.Format("%y %m %d - %H:%M:%S"),
//															timeCur.Format("%y %m %d - %H:%M:%S") );
				m_bModified = TRUE;
			}

			delete pFileInfo;
		}

		nMergeFileHeaderSize += strlen(filename);
	}

	// 갯수가 남아있나? 그렇다면 새 머지 파일중 파일이 삭제되어 갯수가 남아있는거다. 변경!
	if (mapStringToPtr.GetCount())
	{
		POSITION pos = mapStringToPtr.GetStartPosition();
		while (pos)
		{
			CString string;
			mapStringToPtr.GetNextAssoc(pos, string, (void*&)pFileInfo);
			delete pFileInfo;
		}
		m_bModified = TRUE;
	}

	if (g_bSync)
		m_bModified = TRUE;

#ifdef __SECURITY_0628
	AddAuth(szFileName);
#endif	// __SECURITY_0628

	// 수정된게 있나?
	if (m_bModified == FALSE)
		return 1;
	// 헤더 크기 구함 끝

	FILETIME ft;
	if (g_bSync)
	{
		szFileName.SetAt(3, '_');

		CString strKey = (LPCTSTR)szFileName;
		strKey.MakeLower();

		map< string, FILETIME>::iterator it = g_map.find((LPCTSTR)strKey);
		if (it != g_map.end())
		{
			ft = it->second;
		}
		else
		{
			return 100;
		}
	}
//	puts( szFileName );

	// 머지 파일 오픈
	CFile out;
	if (out.Open(szFileName, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		printf("%s file open error\n", szFileName);
		return -1;
	}

	// 파일 넣기 시작
#ifdef __one_byte
	out.Seek((sizeof(g_byEncryptionKey) + sizeof(bool) + sizeof(int)) + sizeof(unsigned char) + nMergeFileHeaderSize, CFile::begin);
#else
	out.Seek((sizeof(g_byEncryptionKey) + sizeof(bool) + sizeof(int)) + nMergeFileHeaderSize, CFile::begin);
#endif
	fvi = g_fv.begin();
	for (; fvi != g_fv.end(); ++fvi)
	{
		CFile file((*fvi).FilePath, CFile::modeRead);
		BYTE* pBuffer = new BYTE[(*fvi).FileSize];
		file.Read(pBuffer, (*fvi).FileSize);
		// 암호화라면 실제 데이타 암호화 하기 
		if (bEncryption)
		{
			for (int i = 0; i < (*fvi).FileSize; i++)
			{
				pBuffer[i] = CResFile::Encryption(g_byEncryptionKey, pBuffer[i]);
			}
		}
		file.Close();

		out.Write(pBuffer, (*fvi).FileSize);

		delete[] pBuffer;
	}
	// 파일 넣기 끝

	// 헤더 구하기 시작
	char* pHeader = new char[nMergeFileHeaderSize];
	int nHederWirtePosition = 0;

	// 머지 파일 버젼 - 7bytes "V0.01"
	memcpy(&pHeader[nHederWirtePosition], RESOURCE_VERSION, 7);
	nHederWirtePosition += 7;
	// 머지할 파일 개수 넣기
	memcpy(&pHeader[nHederWirtePosition], &nMergeFileNumber, sizeof(short));
	nHederWirtePosition += sizeof(short);
	// 머지할 파일 위치 구하기

#ifdef __one_byte
	int nFilePostion = (sizeof(g_byEncryptionKey) + sizeof(bool) + sizeof(int)) + sizeof(unsigned char) + nMergeFileHeaderSize;
#else
	int nFilePostion = (sizeof(g_byEncryptionKey) + sizeof(bool) + sizeof(int)) + nMergeFileHeaderSize;
#endif
	
	time_t time_;

	fvi = g_fv.begin();
	for (; fvi != g_fv.end(); ++fvi)
	{
		_splitpath((*fvi).FilePath, drive, dir, fname, ext);
		wsprintf(filename, "%s%s", fname, ext);
		int nFileNameLength = strlen(filename);
		// 머지할 파일 이름 길이 넣기
		memcpy(&pHeader[nHederWirtePosition], &nFileNameLength, sizeof(nFileNameLength));
		nHederWirtePosition += sizeof(short);
		// 머지할 파일 이름 넣기
		memcpy(&pHeader[nHederWirtePosition], filename, nFileNameLength);
		nHederWirtePosition += nFileNameLength;
		// 머지할 파일 크기 넣기
		memcpy(&pHeader[nHederWirtePosition], &(*fvi).FileSize, sizeof(int));
		nHederWirtePosition += sizeof(int);
		// 머지할 파일의 최근 갱신 날짜 넣기
		time_ = (time_t)((*fvi).m_mtime.GetTime());
		memcpy(&pHeader[nHederWirtePosition], &time_, sizeof(time_t));
		nHederWirtePosition += sizeof(time_t);
		// 머지할 파일 위치 넣기
		memcpy(&pHeader[nHederWirtePosition], &nFilePostion, sizeof(int));
		nHederWirtePosition += sizeof(int);

		// 다음 머지할 파일 위치 구하기 : 기존 위치 + 전 파일 크기
		nFilePostion += (*fvi).FileSize;
	}
	// 해더는 암호 기본 
	// 암호화라면
	//if( bEncryption )
	{
		// xor
		char* pTemp = new char[nMergeFileHeaderSize];
		for (int i = 0; i < nMergeFileHeaderSize; i++)
			pTemp[i] = CResFile::Encryption(g_byEncryptionKey, pHeader[i]);

		// 헤더 넣기 시작
		out.Seek(0, CFile::begin);
		// 암호키 넣기

		//random char
#ifdef __one_byte	
		unsigned char wew = rand() % 255;
		out.Write(&wew, sizeof(unsigned char));
#endif

		out.Write(&g_byEncryptionKey, sizeof(g_byEncryptionKey));
		// 암호화인가?
		out.Write(&bEncryption, sizeof(bool));
		// 헤더 크기 넣기
		out.Write(&nMergeFileHeaderSize, sizeof(int));
		// 헤더 넣기
		out.Write(pTemp, nMergeFileHeaderSize);

		delete[] pTemp;
	}

	delete[] pHeader;

	if (g_bSync)
	{
		SetFileTime((HANDLE)out.m_hFile, &ft, &ft, &ft);
	}

	out.Close();

	return 0;
}


void DoMerge()
{
	if (g_bSync)
	{
		if (MakePatchList(0) == MAKEPATCHLIST_OK)
			printf("list.txt process ok.\n\n\n");
		else
		{
			printf("list.txt not found or process error\n");
			return;
		}
	}

	CString string;

	CPtrArray ptrArray;

	if (TRUE == g_bOutsideMerge)
	{
		g_fv.clear();
		CScanner s;

		if (s.Load_FileIO(g_strArgumentFileName))
		{
			printf("%s file load\n", g_strArgumentFileName);

			s.GetToken();
			while (s.tok != FINISHED)
			{
				OutputDebugString(s.Token);

				if (s.Token == "version")
				{
					s.GetToken();
					g_strVersion = s.Token;
				}
				if (s.Token == "encryptionKey")
				{
					g_byEncryptionKey = s.GetNumber();
				}
				if (s.Token == "resource")
				{
					bool bEncryption = (s.GetNumber() == 1); // 암호 여부 

					s.GetToken(); // 리소스 파일명
					CString strResource = s.token;

					InterpriteScript(s);

//					FileContainerIterator fvi = g_fv.begin();
//					for( ; fvi != g_fv.end(); ++fvi )
//					{
//						printf( "%s\n", ( *fvi ).FilePath );
//					}

					// 머지 
					int nResult;
					try
					{
						printf("MergeFile( %s, %d ) - ", strResource, bEncryption);
						nResult = MergeFile(strResource, bEncryption);
					}
					catch (...)
					{
						AfxMessageBox(strResource);
						return;
					}

					switch (nResult)
					{
						case 0:
							printf("ok\n");
							break;
						case 1:
							printf("skip\n");
							break;
						case 100:
							printf("skip sync\n");
							break;
						default:
							printf("error\n");
							break;
					}

					g_fv.clear();

				}
				s.GetToken();
			}
#ifdef __SECURITY_0628
			MakeAuthFile();
#endif	// __SECURITY_0628
		}
		else
		{
			printf("%s file load error\n", g_strArgumentFileName);
		}
	}
}



void EnqueueFILE(LPCTSTR szFile, bool bDir, int nSize, SYSTEMTIME* pTime)
{
/*
	FILE_INFO info = { 0, };
	if( pTime )
		SystemTimeToFileTime( pTime, &info.ft );
	info.bDir = bDir;
	info.nFileSize = nSize;
	sprintf( info.szPath, "%s%s", m_szCurrentDirectory, szFile );
	char szServerPath[MAX_PATH];
	sprintf( szServerPath, "%s%s.gz", s_cdn, szFile );
	CString str = szServerPath;
	str.Replace( '\\', '/' );
	strcpy( info.szServerPath, str );

	m_files.push_back( info );
*/
	FILETIME ft;
	if (pTime)
		SystemTimeToFileTime(pTime, &ft);

	CString str;
	str = "..\\_esClient";
	str += szFile;
	str.MakeLower();

	puts(str);

	g_map.insert(make_pair((LPCTSTR)str, ft));
}

MAKEPATCHLIST_RESULT MakePatchList(int nVersion)
{
//	m_files.clear();

//	char szPath[MAX_PATH];
//	sprintf( szPath, "%s\\%s", m_szCurrentDirectory, "list.txt" ); 
	char szPath[MAX_PATH] = "list.txt";

	FILE* fp = fopen(szPath, "rt");
	if (!fp)
		return MAKEPATCHLIST_FILE_NOTFOUND;

	SYSTEMTIME	sysTime;
	char	szTokens[4][256];
	char	szBuffer[256], szDir[256], szFile[256];
	char	szRoot[256] = { 0, };
	char	cbFlag;
	int		nYY, nMM, nDD;
	int		nHour, nMin;

	while (1)
	{
		if (fgets(szBuffer, 127, fp) == NULL)
			break;

		if (strlen(szBuffer) < 6)
			continue;

		if (szBuffer[0] == 'v')
		{
			if (memcmp(szBuffer, "ver:", 4) == 0)
			{
			}
		}
		else if (szBuffer[2] == ':')
		{
			sscanf(szBuffer, "%s %s", szTokens[0], szTokens[1]);
			if (szRoot[0] == 0)
			{
				strcpy(szRoot, szTokens[0]);
				szDir[0] = 0;
			}
			else
			{
				memset(szDir, 0x00, sizeof(szDir));
				memcpy(szDir, szTokens[0] + strlen(szRoot), strlen(szTokens[0]) - strlen(szRoot));

//				EnqueueFILE( szDir, true, 0, NULL );
			}
		}
		else if (szBuffer[4] == '-')
		{
			sscanf(szBuffer, "%s %s %s %s", szTokens[0], szTokens[1], szTokens[2], szTokens[3]);
			if (strcmp(szTokens[2], "<DIR>"))
			{
				sscanf(szTokens[0], "%d-%d-%d", &nYY, &nMM, &nDD);
				sscanf(szTokens[1], "%02d:%02d%c", &nHour, &nMin, &cbFlag);
				if (nHour == 12)
					nHour = 0;
				if (cbFlag == 'p')
					nHour += 12;

				sprintf(szFile, "%s\\%s", szDir, szBuffer + 39);
				szFile[strlen(szFile) - 1] = 0;					// line feed 제거 

				sysTime.wYear = (WORD)nYY;
				sysTime.wMonth = (WORD)nMM;
				sysTime.wDay = (WORD)nDD;
				sysTime.wHour = (WORD)nHour;
				sysTime.wMinute = (WORD)nMin;
				sysTime.wSecond = 0;
				sysTime.wMilliseconds = 0;

				EnqueueFILE(szFile, false, atoi(szTokens[2]), &sysTime);
			}
		}
	}
	fclose(fp);

//	CheckFiles();

	return MAKEPATCHLIST_OK;
}

struct FILE_INFO_EX : public FILE_INFO
{
	int nFilePosition;
};

BOOL F(LPCTSTR lpszFileName, CMapStringToPtr& mapStringToPtr)
{
	int nFileHeaderSize = 0;
	int nFileNumber = 0;
	int nFileNameLength = 0;
	char szFileName[_MAX_FNAME] = "";
	int nFileSize = 0;
	int nFilePosition = 0;
	BYTE byEncryptionKey;
	bool bEncryption;

//	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
//	_splitpath( lpszResName, drive, dir, name, ext );

	CFile file;
	if (file.Open(lpszFileName, CFile::modeRead) == FALSE)
		return FALSE;

	//one_byte
	unsigned char useless;
	file.Read(&useless, sizeof(useless));

	file.Read(&byEncryptionKey, sizeof(byEncryptionKey));
	file.Read(&bEncryption, sizeof(bEncryption));
	file.Read(&nFileHeaderSize, sizeof(int));

	char* pHeader = new char[nFileHeaderSize];
	// 해더는 암호 기본 
	//if( m_bEncryption ) // 암호화 되었다면
	{
		char* pCryptHeader = new char[nFileHeaderSize];
		file.Read(pCryptHeader, nFileHeaderSize);

		for (int i = 0; i < nFileHeaderSize; i++)
		{
			pHeader[i] = CResFile::Decryption(byEncryptionKey, pCryptHeader[i]);
		}

		delete[] pCryptHeader;
	}
	//else // 암호화 안되었다면
	//{
	//	m_File.Read( pHeader, nFileHeaderSize );
	//}
	int nHeaderPosition = 0;
	char strVersion[8] = "";
	memcpy(strVersion, &pHeader[nHeaderPosition], 7); nHeaderPosition += 7;

	memcpy(&nFileNumber, &pHeader[nHeaderPosition], sizeof(short)); nHeaderPosition += sizeof(short);

	time_t time_;
	for (int i = 0; i < nFileNumber; i++)
	{
	//	FILEOUT( "c:\\debug.txt", "2 %s %s %d \n", lpszFileName, szFileName, nFileNumber );

		memcpy(&nFileNameLength, &pHeader[nHeaderPosition], sizeof(short)); nHeaderPosition += sizeof(short);
		memcpy(szFileName, &pHeader[nHeaderPosition], nFileNameLength); nHeaderPosition += nFileNameLength;
		memcpy(&nFileSize, &pHeader[nHeaderPosition], sizeof(int)); nHeaderPosition += sizeof(int);
		memcpy(&time_, &pHeader[nHeaderPosition], sizeof(time_t)); nHeaderPosition += sizeof(time_t);
		memcpy(&nFilePosition, &pHeader[nHeaderPosition], sizeof(int)); nHeaderPosition += sizeof(int);
		FILE_INFO_EX* pFileInfo = new FILE_INFO_EX;

		pFileInfo->FileSize = nFileSize;  // 
		pFileInfo->FilePath = szFileName;
		pFileInfo->m_mtime = time_;
		pFileInfo->nFilePosition = nFilePosition;

		_strlwr(szFileName);
		mapStringToPtr.SetAt(szFileName, pFileInfo);
		ZeroMemory(szFileName, sizeof(szFileName));
	}
	delete[] pHeader;

	file.Close();
	return TRUE;
}

// Part_femaleHead05.o3d
BOOL SplitFile(LPCTSTR szFileName)
{
//	FindFileFromResource( szFileName, mapStringToPtr );
	CMapStringToPtr mapStringToPtr;
	F(szFileName, mapStringToPtr);

	if (mapStringToPtr.GetCount())
	{
		POSITION pos = mapStringToPtr.GetStartPosition();
		while (pos)
		{
			CString strFile;
			FILE_INFO_EX* pFileInfo;

			mapStringToPtr.GetNextAssoc(pos, strFile, (void*&)pFileInfo);
			int nSize = pFileInfo->FileSize;
			CFile file(szFileName, CFile::modeRead);
			BYTE* pBuffer = new BYTE[nSize];
			file.Seek(pFileInfo->nFilePosition, CFile::begin);
			file.Read(pBuffer, nSize);
			file.Close();

			CFile out;
			if (out.Open(pFileInfo->FilePath, CFile::modeCreate | CFile::modeWrite))
			{
				for (int i = 0; i < nSize; i++)
				{
					pBuffer[i] = CResFile::Decryption(g_byEncryptionKey, pBuffer[i]);
				}

				out.Write(pBuffer, nSize);
			}
			else
			{
				printf("%s file create error.\n", pFileInfo->FilePath);
			}

			delete pFileInfo;
			delete[] pBuffer;

		}
	}
	return TRUE;
}

#ifdef __SECURITY_0628
void	MakeClientAuthFile(void)
{
	CFile out;
	if (out.Open("..\\ResClient\\Flyff.a", CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		printf("%s file open error\n", "Flyff.a");
		return;
	}
	for (map<string, string>::iterator i = s_mapAuth.begin(); i != s_mapAuth.end(); ++i)
	{
		out.Write(i->first.data(), lstrlen(i->first.data()));
		out.Write(i->second.data(), lstrlen(i->second.data()));
	}
	out.Close();
}

void	MakeServerAuthFile(void)
{
	CFile a("..\\ResClient\\Flyff.a", CFile::modeRead);
	int nLen = (int)(a.GetLength());
	BYTE* ptr = new BYTE[nLen];
	a.Read(ptr, nLen);
	char sAuth[100] = { 0, };
	md5(sAuth, ptr, nLen);
	delete[] ptr;
	a.Close();

	CFile b("..\\Flyff.b", CFile::modeCreate | CFile::modeWrite);
	b.Write(sAuth, lstrlen(sAuth));
	b.Close();
}

void	MakeAuthFile(void)
{
	MakeClientAuthFile();
	MakeServerAuthFile();
}

BOOL IsDataFile(CString& szFileName)
{
	return szFileName.Find("data.res", 0) >= 0
		|| szFileName.Find("dataSub1.res", 0) >= 0
		|| szFileName.Find("dataSub2.res", 0) >= 0;
}

void AddAuth(CString& szFileName)
{
	if (IsDataFile(szFileName))
	{
		char sFile[100] = { 0, };
		char sData[100] = { 0, };

		FileContainerIterator	i = g_fv.begin();
		for (; i != g_fv.end(); ++i)
		{
			CFile file(i->FilePath, CFile::modeRead);
			BYTE* pBuffer = new BYTE[i->FileSize];
			file.Read(pBuffer, i->FileSize);

			CString strFile = i->FilePath;
			strFile.MakeLower();
			md5(sFile, strFile);
			md5(sData, pBuffer, i->FileSize);
			file.Close();
			delete[] pBuffer;

			bool b = s_mapAuth.insert(map<string, string>::value_type(sFile, sData)).second;
		}
	}
}
#endif	// __SECURITY_0628