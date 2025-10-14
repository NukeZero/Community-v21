
//InfoCritSect.h Header File

#ifndef __INFOCRITSECT_H__
#define __INFOCRITSECT_H__

//#define __INFOCRITSECT_DEBUG__

//#pragma warning(disable:4786)

#include "Afxmt.h"
//#include "WThread.h"
#include <string>
#include <strstream>
#include <fstream>
#include <map>

using namespace std;
/*
struct SData
{
	//CONSTRUCTOR
	SData(string const& rostrBeforeLock="No Init", string const& rostrAfterLock="No Init",
		string const& rostrBeforeUnlock="No Init", string const& rostrAfterUnlock="No Init",
		string const& rostrDesc="No Desc") : m_ostrBeforeLock(rostrBeforeLock), m_ostrAfterLock(rostrAfterLock),
		m_ostrBeforeUnlock(rostrBeforeUnlock), m_ostrAfterUnlock(rostrAfterUnlock), m_ostrDesc(rostrDesc),
		m_poCriticalSection(NULL) {}
	string m_ostrBeforeLock;
	string m_ostrAfterLock;
	string m_ostrBeforeUnlock;
	string m_ostrAfterUnlock;
	string m_ostrDesc;
	CMclCritSec* m_poCriticalSection;
};
*/

typedef struct _lockdesc
{
	char sFileName[MAX_PATH];
	u_int	uiLine;
	DWORD	dwThreadId;
	DWORD	dwTickCount;
}	lockdesc;

struct SData
{
	SData()
	{
//		*m_ldBeforeLock.sFileName	= '\0';
//		*m_ldAfterLock.sFileName	= '\0';
//		*m_ldBeforeUnlock.sFileName	= '\0';
//		*m_ldAfterUnlock.sFileName	= '\0';
		ZeroMemory( &m_ldBeforeLock, sizeof(m_ldBeforeLock) );
		ZeroMemory( &m_ldAfterLock, sizeof(m_ldAfterLock) );
		ZeroMemory( &m_ldBeforeUnlock, sizeof(m_ldBeforeUnlock) );
		ZeroMemory( &m_ldAfterUnlock, sizeof(m_ldAfterUnlock) );
		m_nLocked	= 0;
		m_nTrying	= 0;
	}

	lockdesc	m_ldBeforeLock;
	lockdesc	m_ldAfterLock;
	lockdesc	m_ldBeforeUnlock;
	lockdesc	m_ldAfterUnlock;
	char	m_sDesc[128];
	CMclCritSec*	m_poCriticalSection;

	int	m_nLocked;
	int	m_nTrying;
};

//CInfoCritSect class
//class CInfoCritSect : public CCriticalSection
//inline DWORD IDHash( DWORD id, size_t HashSize )
//{
//	return (((id) + (id>>8) + (id>>16) + (id>>24)) & (HashSize-1));
//}


class CInfoCritSect : public CMclCritSec
{
public:
	//CONSTRUCTOR
	CInfoCritSect( const char* sDesc = "" )
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		sm_oCritSect[idHash].Enter();
		//Add a new data element in the map
		m_iIndex	= sm_iCount[idHash];
		sm_iCount[idHash]++;
		sm_oMap[idHash][m_iIndex]	= SData();
//		sm_oMap[m_iIndex].m_ostrDesc	= rostrDesc;
		lstrcpy( sm_oMap[idHash][m_iIndex].m_sDesc, sDesc );
		sm_oMap[idHash][m_iIndex].m_poCriticalSection	= this;
		sm_oCritSect[idHash].Leave();
	}

	void SetDescription( const char* sDesc )
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		//Remove the data element from the map
		map<unsigned int, SData>::iterator it;
		sm_oCritSect[idHash].Enter();
			it = sm_oMap[idHash].find(m_iIndex);
			if(it != sm_oMap[idHash].end())
//				it->second.m_ostrDesc	= rostrDesc;
				lstrcpy( it->second.m_sDesc, sDesc );
		sm_oCritSect[idHash].Leave();
	}

	//DESTRUCTOR
	~CInfoCritSect()
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		//Remove the data element from the map
		map<unsigned int, SData>::iterator it;
		sm_oCritSect[idHash].Enter();
			it = sm_oMap[idHash].find(m_iIndex);
			if(it != sm_oMap[idHash].end())
				sm_oMap[idHash].erase(it);
		sm_oCritSect[idHash].Leave();
	}

	DWORD IDHash( DWORD id, size_t HashSize )
	{	
		return (((id) + (id>>8) + (id>>16) + (id>>24)) & (HashSize-1));
	}
	
//	void Enter(unsigned int uiLine/*=0*/, string const& rostrFileName/*=""*/);
	void Enter( unsigned int uiLine/*=0*/, const char* rostrFileName/*=""*/ );
//	void Leave(unsigned int uiLine/*=0*/, string const& rostrFileName/*=""*/);
	void Leave( unsigned int uiLine/*=0*/, const char* rostrFileName/*=""*/ );

//	void BeforeLock(unsigned int uiLine, string const& rostrFileName)
	void BeforeLock( unsigned int uiLine, const char* rostrFileName )
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		//Find the position in map
		map<unsigned int, SData>::iterator it;
		sm_oCritSect[idHash].Enter();
			it = sm_oMap[idHash].find(m_iIndex);
			if(it != sm_oMap[idHash].end())
			{
				/*
				DWORD dwId = ::GetCurrentThreadId();
				ostrstream ostr;
				ostr << "Trying to Lock: ID=" << ::GetCurrentThreadId()
					<< ", File=" << rostrFileName << ", Line=" << uiLine << ", Time=" << ::GetTickCount();
				ostr << ends;
				it->second.m_ostrBeforeLock = (char *)ostr.str();
				ostr.freeze(0);
				*/
				if( it->second.m_nTrying == 0 )
				{
					it->second.m_ldBeforeLock.dwThreadId	= GetCurrentThreadId();
					it->second.m_ldBeforeLock.uiLine	= uiLine;
					lstrcpy( it->second.m_ldBeforeLock.sFileName, rostrFileName );
					it->second.m_ldBeforeLock.dwTickCount	= GetTickCount();
				}
				it->second.m_nTrying++;
			}
		sm_oCritSect[idHash].Leave();
	}

//	void AfterLock(unsigned int uiLine, string const& rostrFileName)
	void AfterLock( unsigned int uiLine, const char* rostrFileName )
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		//Find the position in map
		map<unsigned int, SData>::iterator it;
		sm_oCritSect[idHash].Enter();
			it = sm_oMap[idHash].find(m_iIndex);
			if(it != sm_oMap[idHash].end())
			{
				/*
				DWORD dwId = ::GetCurrentThreadId();
				ostrstream ostr;
				ostr << "Locked: ID=" << ::GetCurrentThreadId()
					<< ", File=" << rostrFileName << ", Line=" << uiLine << ", Time=" << ::GetTickCount();
				ostr << ends;
				it->second.m_ostrAfterLock = (char *)ostr.str();
				ostr.freeze(0);
				*/
				it->second.m_nTrying--;
				if( it->second.m_nLocked == 0 )
				{
					it->second.m_ldAfterLock.dwThreadId		= GetCurrentThreadId();
					it->second.m_ldAfterLock.uiLine	= uiLine;
					lstrcpy( it->second.m_ldAfterLock.sFileName, rostrFileName );
					it->second.m_ldAfterLock.dwTickCount	= GetTickCount();
				}
				it->second.m_nLocked++;
			}
		sm_oCritSect[idHash].Leave();
	}

//	void BeforeUnlock(unsigned int uiLine, string const& rostrFileName)
	void BeforeUnlock( unsigned int uiLine, const char* rostrFileName )
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		//Find the position in map
		map<unsigned int, SData>::iterator it;
		sm_oCritSect[idHash].Enter();
			it = sm_oMap[idHash].find(m_iIndex);
			if(it != sm_oMap[idHash].end())
			{
				/*
				DWORD dwId = ::GetCurrentThreadId();
				ostrstream ostr;
				ostr << "Trying to Unlock: ID=" << ::GetCurrentThreadId()
					<< ", File=" << rostrFileName << ", Line=" << uiLine << ", Time=" << ::GetTickCount();
				ostr << ends;
				it->second.m_ostrBeforeUnlock = (char *)ostr.str();
				ostr.freeze(0);
				*/
				if( it->second.m_nLocked == 1 )
				{
					it->second.m_ldBeforeUnlock.dwThreadId	= GetCurrentThreadId();
					it->second.m_ldBeforeUnlock.uiLine	= uiLine;
					lstrcpy( it->second.m_ldBeforeUnlock.sFileName, rostrFileName );
					it->second.m_ldBeforeUnlock.dwTickCount	= GetTickCount();
				}
			}
		sm_oCritSect[idHash].Leave();
	}

//	void AfterUnlock(unsigned int uiLine, string const& rostrFileName)
	void AfterUnlock( unsigned int uiLine, const char* rostrFileName )
	{
		u_long idHash	= IDHash( (DWORD)this, 1024 );

		//Find the position in map
		map<unsigned int, SData>::iterator it;
		sm_oCritSect[idHash].Enter();
			it = sm_oMap[idHash].find(m_iIndex);
			if(it != sm_oMap[idHash].end())
			{
				/*
				DWORD dwId = ::GetCurrentThreadId();
				ostrstream ostr;
				ostr << "Unlocked: ID=" << ::GetCurrentThreadId()
					<< ", File=" << rostrFileName << ", Line=" << uiLine << ", Time=" << ::GetTickCount();
				ostr << ends;
				it->second.m_ostrAfterUnlock = ostr.str(); //str() is freezing
				//delete ostr.str(); //freeing up storage
				//or to be automatically freed
				ostr.freeze(0);
				*/
				it->second.m_nLocked--;
				if( it->second.m_nLocked == 0 )
				{
					it->second.m_ldAfterUnlock.dwThreadId	= GetCurrentThreadId();
					it->second.m_ldAfterUnlock.uiLine	= uiLine;
					lstrcpy( it->second.m_ldAfterUnlock.sFileName, rostrFileName );
					it->second.m_ldAfterUnlock.dwTickCount	= GetTickCount();
				}
			}
		sm_oCritSect[idHash].Leave();
	}

	static void PrintInfo()
	{
/*
		string pstrInfo[1024];
		DWORD pdwInfo[1024];
		int cbInfo	= 0;
*/
		//Open Output File
//		ofstream fout("Deadlocks.out");
		CTime time	= CTime::GetCurrentTime();
		ofstream fout( (LPCSTR)time.Format( "../Deadlocks%Y%m%d%H%M%S.txt" ) );
		if(!fout)
			return;
		for( int i = 0; i < 1024; i++ )
		{
			sm_oCritSect[i].Enter();
				map<unsigned int, SData>::iterator it = sm_oMap[i].begin();
				while(it != sm_oMap[i].end())
				{
/*
					fout << "Critical Section: " << it->second.m_ostrDesc << endl;
					fout << " " << it->second.m_ostrBeforeLock << endl;
					fout << " " << it->second.m_ostrAfterLock << endl;
					fout << " " << it->second.m_ostrBeforeUnlock << endl;
					fout << " " << it->second.m_ostrAfterUnlock << endl;
					fout << endl;
*/
					if( it->second.m_nLocked > 0 )
					{
						fout << "Critical Section: " << it->second.m_sDesc << endl;
						fout << " Trying to Lock: ID = " << it->second.m_ldBeforeLock.dwThreadId << endl << ", File=" << it->second.m_ldBeforeLock.sFileName << ", Line=" << it->second.m_ldBeforeLock.uiLine << ", Time=" << it->second.m_ldBeforeLock.dwTickCount << ends << endl;
						fout << " Locked: ID = " << it->second.m_ldAfterLock.dwThreadId << endl << ", File=" << it->second.m_ldAfterLock.sFileName << ", Line=" << it->second.m_ldAfterLock.uiLine << ", Time=" << it->second.m_ldAfterLock.dwTickCount << ends << endl;
						fout << " Trying to Unlock: ID = " << it->second.m_ldBeforeUnlock.dwThreadId << endl << ", File=" << it->second.m_ldBeforeUnlock.sFileName << ", Line=" << it->second.m_ldBeforeUnlock.uiLine << ", Time=" << it->second.m_ldBeforeUnlock.dwTickCount << ends << endl;
						fout << " Unlocked: ID = " << it->second.m_ldAfterUnlock.dwThreadId << endl << ", File=" << it->second.m_ldAfterUnlock.sFileName << ", Line=" << it->second.m_ldAfterUnlock.uiLine << ", Time=" << it->second.m_ldAfterUnlock.dwTickCount << ends << endl;
						fout << endl;
/*
						ostrstream ostr;
						ostr << "Critical Section: " << it->second.m_sDesc << endl;
						ostr << " Trying to Lock: ID = " << it->second.m_ldBeforeLock.dwThreadId << endl << ", File=" << it->second.m_ldBeforeLock.sFileName << ", Line=" << it->second.m_ldBeforeLock.uiLine << ", Time=" << it->second.m_ldBeforeLock.dwTickCount << endl;
						ostr << " Locked: ID = " << it->second.m_ldAfterLock.dwThreadId << endl << ", File=" << it->second.m_ldAfterLock.sFileName << ", Line=" << it->second.m_ldAfterLock.uiLine << ", Time=" << it->second.m_ldAfterLock.dwTickCount << endl;
						ostr << " Trying to Unlock: ID = " << it->second.m_ldBeforeUnlock.dwThreadId << endl << ", File=" << it->second.m_ldBeforeUnlock.sFileName << ", Line=" << it->second.m_ldBeforeUnlock.uiLine << ", Time=" << it->second.m_ldBeforeUnlock.dwTickCount << endl;
						ostr << " Unlocked: ID = " << it->second.m_ldAfterUnlock.dwThreadId << endl << ", File=" << it->second.m_ldAfterUnlock.sFileName << ", Line=" << it->second.m_ldAfterUnlock.uiLine << ", Time=" << it->second.m_ldAfterUnlock.dwTickCount << endl;
						ostr << endl << ends;
						pstrInfo[cbInfo]	= (char *)ostr.str();
						ostr.freeze(0);
						pdwInfo[cbInfo++]	= it->second.m_ldAfterLock.dwTickCount;
*/
					}
					it++;
				}
			sm_oCritSect[i].Leave();
		}
/*
		// sort
		for( int j = 0; j < cbInfo-1; j++ )
		{
			for( int k = 0; k < cbInfo; k++ )
			{
				if( pdwInfo[j] > pdwInfo[k] )
				{
					string str	= pstrInfo[j];
					DWORD dw	= pdwInfo[j];
					pstrInfo[j]	= pstrInfo[k];
					pdwInfo[j]	= pdwInfo[k];
					pstrInfo[k]	= str;
					pdwInfo[k]	= dw;
				}
			}
		}
		for( i = 0; i < cbInfo; i++ )
			fout << pstrInfo[i];
*/
		fout.close();
	}

	static void UnlockAll()
	{
		for( int i = 0; i < 1024; i++ )
		{
			sm_oCritSect[i].Enter();
			map<unsigned int, SData>::iterator it = sm_oMap[i].begin();
			while(it != sm_oMap[i].end())
			{
				it->second.m_poCriticalSection->Leave();
				it++;
			}
			sm_oCritSect[i].Leave();
		}
	}

private:
	static CMclCritSec	sm_oCritSect[1024];
	static map<unsigned int, SData> sm_oMap[1024];
	static unsigned int sm_iCount[1024];
	unsigned int m_iIndex;
};

//inline void CInfoCritSect::Enter(unsigned int uiLine, string const& rostrFileName)
inline void CInfoCritSect::Enter(unsigned int uiLine, const char* rostrFileName)
{
#ifdef __INFOCRITSECT_DEBUG__
	BeforeLock(uiLine, rostrFileName);
#endif
	CMclCritSec::Enter();
#ifdef __INFOCRITSECT_DEBUG__
	AfterLock(uiLine, rostrFileName);
#endif
}

//inline void CInfoCritSect::Leave(unsigned int uiLine, string const& rostrFileName)
inline void CInfoCritSect::Leave(unsigned int uiLine, const char* rostrFileName)
{
#ifdef __INFOCRITSECT_DEBUG__
	BeforeUnlock(uiLine, rostrFileName);
#endif
	CMclCritSec::Leave();
#ifdef __INFOCRITSECT_DEBUG__
	AfterUnlock(uiLine, rostrFileName);
#endif
}

#endif // __INFOCRITSECT_H__

