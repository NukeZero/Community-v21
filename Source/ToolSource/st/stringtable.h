#ifndef __STRING_TABLE_H__
#define	__STRING_TABLE_H__
DECLARE_HANDLE(HSTRBLOCK);
#define	NO_OF_STRINGS_PER_BLOCK	16

class CStringTable
{
	LPWSTR m_strArray[NO_OF_STRINGS_PER_BLOCK]{};
	BOOL CreateBlock(HINSTANCE hInstLib, WORD wLangID);
	BOOL ParseRes(LPVOID pRes);
	void BuildRes(LPVOID pRes);
	BOOL UpdateBlock(HANDLE hUpdate, WORD wLangID);
	DWORD GetResSize(void);

public:
	CStringTable();
	~CStringTable();
	
	void Clear();
	BOOL GetStringBlock( LPCSTR strAppName, WORD wLangID );
	BOOL GetString( int nIndex, LPSTR pszText );
	BOOL UpdateStringBlock( LPCSTR sApp, WORD wLangID );
	BOOL SetString( int nIndex, LPCSTR pszText );
};

#endif	// __STRING_TABLE_H__