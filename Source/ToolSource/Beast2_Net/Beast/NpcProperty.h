#ifndef __NPC_PROPERTY__
#define __NPC_PROPERTY__

#include "Project.h"
#include "ScriptDialog.h"

class CNpcProperty
{
public:
	CNpcProperty();
	virtual ~CNpcProperty();

public:
	LPCTSTR			GetDlgName();
	void			SetDlgName( LPCTSTR szName );
	BOOL			LoadDialog( LPCHARACTER lpCharacter );
	BOOL			IsDialogLoaded();
		
#ifdef __V050322_CACHEQUEST
	BOOL			IsTimeOut();
	void			ResetTimer();
	BOOL			RunDialog( LPCTSTR pKey, int* nResult, int nValue, int nDstId, int nSrcId, int nQuestId );
#else
	BOOL			RunDialog( LPCTSTR pKey, int* nResult, int nValue, int nGlobal2, int nGlobal3, int nQuestId = 0 );
#endif
public:
	CTimer			m_timerScript;

private:
	TCHAR			m_szDialogFile[32];
//	CScriptDialog	m_DlgQuest;				// 퀘스트 스크립트 대사 
};


#endif
