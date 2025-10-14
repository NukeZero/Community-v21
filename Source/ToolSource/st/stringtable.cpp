#include "stdafx.h"
#include "stringtable.h"

#define SAFE_FREE(p) { if(p) { free (p); (p) = nullptr; } }

CStringTable::CStringTable()
{
	memset(m_strArray, 0, sizeof(LPWSTR) * NO_OF_STRINGS_PER_BLOCK);
}

CStringTable::~CStringTable()
{
	Clear();
}

void CStringTable::Clear(void)
{
	for (int i = 0; i < NO_OF_STRINGS_PER_BLOCK; i++)
	{
		SAFE_FREE(m_strArray[i]);
	}
}

BOOL CStringTable::GetStringBlock(const char* sApp, const unsigned short wLangID)
{
	HINSTANCE__* hInstLib = LoadLibraryExA(sApp, nullptr, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
	if (nullptr == hInstLib)
		return FALSE;

	const BOOL bResult = CreateBlock(hInstLib, wLangID);
	FreeLibrary(hInstLib);
	return bResult;
}

BOOL CStringTable::CreateBlock(HINSTANCE__* hInstLib, const unsigned short wLangID)
{
	HRSRC__* hFindRes = FindResourceEx(hInstLib, RT_STRING, MAKEINTRESOURCE(1), wLangID);
	if (nullptr == hFindRes)
		return FALSE;

	void* hLoadRes = LoadResource(hInstLib, hFindRes);
	if (nullptr == hLoadRes)
		return FALSE;

	void* pRes = LockResource(hLoadRes);
	if (nullptr == pRes)
		return FALSE;

	if (!ParseRes(pRes))
		return 0;
	return TRUE;
}

BOOL CStringTable::ParseRes(void* pRes)
{
	int j;
	wchar_t* pParse = static_cast<wchar_t*>(pRes);
	for (int i = 0; i < NO_OF_STRINGS_PER_BLOCK; i++)
	{
		const int nLen = *pParse++;

		m_strArray[i] = static_cast<wchar_t*>(malloc((nLen + 1) * sizeof(wchar_t)));

		for (j = 0; j < nLen; j++)
			m_strArray[i][j] = *pParse++;
		m_strArray[i][j] = 0;
	}
	return TRUE;
}

BOOL CStringTable::GetString(const int nIndex, char* pszText)
{
	if (nIndex < 0 || nIndex >= NO_OF_STRINGS_PER_BLOCK)
		return FALSE;
	if (!WideCharToMultiByte(CP_ACP, 0, m_strArray[nIndex], -1, pszText, wcslen(m_strArray[nIndex]) + 1, nullptr, nullptr))
		return FALSE;
	return TRUE;
}

VOID	CStringTable::BuildRes(LPVOID pRes)
{
	int	i, j;
	int	nLen;
	WCHAR* pParse = (WCHAR*)pRes;
	//	There are NO_OF_STRINGS_PER_BLOCK (= 16) strings per block.
	for (i = 0; i < NO_OF_STRINGS_PER_BLOCK; i++)
	{
		*pParse++ = nLen = wcslen(m_strArray[i]);
		for (j = 0; j < nLen; j++)
			*pParse++ = m_strArray[i][j];
	}
}

BOOL	CStringTable::UpdateBlock(HANDLE hUpdate, WORD wLangID)
{
	DWORD	dwResSize;
	LPVOID	pRes;
	DWORD	dwRet = 0;

	//	Get the resource length as required by a raw string resource block.
	dwResSize = GetResSize();
	pRes = malloc(dwResSize);

	BuildRes(pRes);

	if (!UpdateResource(
		hUpdate,
		RT_STRING,
		MAKEINTRESOURCE(1),
		wLangID,
		pRes,
		dwResSize
	))
	{
		//		DWORD dwError = GetLastError();
		free(pRes);
		return FALSE;
	}
	free(pRes);
	return TRUE;
}

DWORD	CStringTable::GetResSize(void)
{
	DWORD dwResSize = 0;
	int i = 0;

	for (i = 0; i < NO_OF_STRINGS_PER_BLOCK; i++)
		dwResSize += (wcslen(m_strArray[i]) + 1);

	return dwResSize * sizeof(WCHAR);
}

BOOL	CStringTable::UpdateStringBlock(LPCSTR sApp, WORD wLangID)
{
	HANDLE	hUpdate;
	hUpdate = BeginUpdateResourceA(sApp, FALSE);
	if (NULL == hUpdate)
		return FALSE;
	//	Update the resource.
	if (!UpdateBlock(hUpdate, wLangID))
		return FALSE;
	if (!EndUpdateResource(hUpdate, FALSE))
		return FALSE;
	return TRUE;
}

BOOL	CStringTable::SetString(const int nIndex, LPCSTR pszText)
{
	if (nIndex < 0 || nIndex >= NO_OF_STRINGS_PER_BLOCK)
		return FALSE;

	SAFE_FREE(m_strArray[nIndex]);

	const int nLen = strlen(pszText) + 1;
	m_strArray[nIndex] = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * nLen));
	if (!MultiByteToWideChar(CP_ACP, 0, pszText, -1, m_strArray[nIndex], nLen))
		return FALSE;
	return TRUE;
}
