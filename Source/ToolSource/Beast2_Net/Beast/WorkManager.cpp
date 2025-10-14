// WorkManager.cpp: implementation of the CWorkManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "beast.h"
#include "WorkManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#ifdef __LENAJANG_BEAST

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorkManager::CWorkManager() : m_wdSaveCount( 0 ),
							   m_wdCurrentCount( 0 )
{
	m_vWorkList.clear();
}

CWorkManager::~CWorkManager()
{
	m_vWorkList.empty();
}

//----------------------------------------------------------
//	우선 이 작업리스트에 포함될 작업의 종류를 구분해 보자
//	1. 오브젝트등의 이동, 회전
//	2. 지역 지정
//	3. NPC위치 설정
//----------------------------------------------------------
int	CWorkManager::AddWorkList()
{
	//	lenajang : undo와 redo를 반복하다가 생긴 틈 데이터를 삭제해주기위한 블럭이다.
	//				(내 멋대로 주석 블럭을 만들어 봤다.)
	//			   거기다가 이 코드는 vector가 가장 쓰기 간편하다고 생각해서 사용
	//			   하고 있는 것이기 때문에 그다지 다른 것이 고려되었다거나 한것은 없다
	//			   라는 것을 먼저 적어 두고 싶다.
	//{
	if ( m_wdSaveCount != 0 &&
		 m_wdSaveCount >= m_wdCurrentCount)
	{
		WORKLIST_ITERATOR wi = m_vWorkList.begin();
		for ( ; wi != m_vWorkList.end() ; ++wi )
		{
			if ( wi->Prior >= (m_wdCurrentCount - 1) )
			{
				m_vWorkList.erase( wi, m_vWorkList.end() );
				break;
			}
		}
		m_wdSaveCount = m_wdCurrentCount - 1;
	}
	//}

	//	실제 데이터 추가등
	//m_wdSaveCount = m_wdCurrentCount;
	m_vWorkList.push_back(m_sWorkValue);
	return 0;
}

//----------------------------------------------------------
//	Name	 : SetWorkList()
//	Function : 중간의 값을 다시 셋팅하기위해 위치를 
//				서칭하여 값을 복사해준다.
//----------------------------------------------------------
int CWorkManager::SetWorkList()
{
	return 0;
}

//----------------------------------------------------------
//	Name	 : SearchWorkList( int )	
//	Function : 우선 순위 인덱스를 받아서 리스트에서의 
//			   배열 인덱스형태의 인덱스를 리턴한다. 값이 
//			   여러개이기 때문에 처음 인덱스만을 리턴하게
//			   된다. 만약 그런 인덱스를 가진 것이 리스트에
//			   있다면 -1을 리턴한다.
//----------------------------------------------------------
int CWorkManager::SearchWorkList( int nIndex , BOOL IsPrior )
{	
	WORKLIST_ITERATOR	wi = m_vWorkList.begin();
	int Cnt = 0;
	for ( ; wi < m_vWorkList.end() ; )
	{
		if ( wi->ExtraIValue1 == 0 )
			break;
		if ( IsPrior == FALSE )
		{
			if ( wi->Prior != nIndex )
			{
				Cnt += wi->ExtraIValue1;
				wi += wi->ExtraIValue1;
			}
			else 
				return Cnt;
		}
		else
		{
			if ( wi->Prior < nIndex)
			{
				Cnt += wi->ExtraIValue1;
				wi += wi->ExtraIValue1;
			}
			else 
				return Cnt;
		}
	}
	if ( IsPrior == FALSE )
		return -1;
	else
		return Cnt;
}

//----------------------------------------------------------
//	Name	 : SearchWorkList( D3DXVECTOR3 )
//	Function : 지형에서 높이값을 제외한 값을 비교하여
//			   그 지형임을 같은 곳의 지형임을 찾아 내어준다.
//----------------------------------------------------------
int CWorkManager::SearchWorkList( D3DXVECTOR3 Pos, int Prior )
{	
	WORKLIST_ITERATOR	wi = m_vWorkList.begin();
	int Cnt = 0;
	for ( ; wi < m_vWorkList.end() ; )
	{
		if ( wi->ExtraIValue1 == 0 )
			break;
		if ( wi->Pos.x != Pos.x || wi->Pos.y != Pos.y || wi->Prior != Prior)
		{
			Cnt ++;
			wi ++;
		}
		else 
			return Cnt;
	}
	return -1;
}

//----------------------------------------------------------
//	Name	 : ChangeListValue(  )
//	Function : Vector의 이미 존재하는 값을 바꾸기위해 
//			   인덱스를 넣어서 배열 참조로 참조하여 값을
//			   바꿔 주게 된다.
//----------------------------------------------------------
int	CWorkManager::ChangeListValue( int Index )
{
	memcpy( &m_vWorkList[Index], &m_sWorkValue, sizeof(WORK_VALUE) ) ;
	return 0;
}


//----------------------------------------------------------
//	Name	 : GetWorkValue(  )
//	Function : 이 함수는 바로 이전 작업된 리스트중에 한개의 값을 
//			   리턴한 후에 주어지는 리스트 값을 m_sWorkValue
//			   에 넣어 준다. 
//	Return	 : TRUE-적절한 값이 있다.
//			   FALSE-끝값을 넘어 서거나 값이 없다.
//----------------------------------------------------------
BOOL CWorkManager::GetWorkValue( const int nPrior, int &nCnt)
{
	//	검색을 한번만 해주고 나머지 값들은 덧셈 연산만으로 값을 넣도록 
	//	할수 있게 해주고자 이렇게 했으나 이 코드에 종속적인 면이 없진 않구만
	if (nCnt == 0)
		nCnt = SearchWorkList( nPrior );

	WORKLIST_ITERATOR wi = m_vWorkList.begin();
	wi += nCnt;
	for ( ; wi < m_vWorkList.end() ; )
	{
		if ( wi->Prior == nPrior )
		{
			//	우선 멤카피가 정상적으로 이루어 지는지 확인을 위해서 멤카피닷
			if ( nCnt == 1500 )
				nCnt = nCnt;
			memcpy( &m_sWorkValue, wi, sizeof(WORK_VALUE) );
			nCnt++;
			return TRUE;
		}
		wi ++;
	}

	return FALSE;
}

CWorkManager	g_WorkManager;

#endif	//__LENAJANG_BEAST