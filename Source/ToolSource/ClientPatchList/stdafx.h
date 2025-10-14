// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define	_WIN32_WINNT	0x0500
#define	WINVER	0x0500

// Windows 헤더 파일입니다.
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <stdio.h>



#define MAKEINT64(a, b)		((__int64)(((DWORD)((DWORD_PTR)(a) & 0xffffffff)) | ((__int64)((DWORD)((DWORD_PTR)(b) & 0xffffffff))) << 32))

