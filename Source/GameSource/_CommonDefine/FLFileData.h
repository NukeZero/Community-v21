#ifndef __FLFILEDATA_H__
#define __FLFILEDATA_H__

#pragma once

typedef	struct	_T_FILE_INFO
{	
//	bool		bDir;						// is Directory?
	DWORD		dwFileSize;					// Size
	__int64		n64FileTime;				// Modify Time
	TCHAR		szFullFileName[MAX_PATH];	// Path	
	BYTE		byCompare;

} T_FILE_INFO, *PT_FILE_INFO;


enum	FILECOMPARE_STATE
{
	FILECOMPARE_NONE	= 0
	, FILECOMPARE_NOT_COMPARE
	, FILECOMPARE_DIFFERENT
	, FILECOMPARE_SAME
};


typedef std::list<T_FILE_INFO>		FileContainer;
typedef FileContainer::iterator	FileContainerIterator;


#endif // __FLFILEDATA_H__