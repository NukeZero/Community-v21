// WorkManager.h: interface for the CWorkManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKMANAGER_H__AD635C6A_31C2_4AA3_9FA7_E882AFAFEA73__INCLUDED_)
#define AFX_WORKMANAGER_H__AD635C6A_31C2_4AA3_9FA7_E882AFAFEA73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#	ifdef __LENAJANG_BEAST

//============================================================================================
//	작성자		: lenajang
//	작성 시작일	: 2005년 1월 11일
//	기능		: Undo를 실행해보기 위해서 툴로 작업을 한 내용을 다루는 클래스
//	주의 사항	: 우선 만들어 보자라는 생각으로 만들기 시작했지만 완성도도 장담할수 없는대다가
//				  스케줄 또한 확실히 장담할 수 없으므로 스케줄에 대해서는 함구하겠음 ㅡ,.ㅡ;;
//============================================================================================
#define		MAX_UNDO				4096

#define		SET_DEFAULT_UNDOSTATE			0
#define		SET_TERRAINPOS_UNDOSTATE		1

/*
	각 디파인에 따른 변수의 값
	SET_TERRAINPOS_UNDOSTATE
	[
		UndoClass;		//	Undo를 하게 될 클래스
		ObjPValue;		//	변화된 오브젝트의 포인터값
		Pos;			//	포지션값( 지형의 위치 )
		Prior;			//	우선순위값
		ExtraIValue1	//	Size

		Rotate;			//	undefined
		ExtraValue1;	//	undefined
		ExtraValue2;	//	undefined		 
		
		ExtraIValue2;	//	undefined
	]
//*/
//#define		

//	Undo와 Redo에 사용하기 위한 인터페이스입니다.
class IUndo
{
public:
	virtual int Undo() = 0;
	virtual int Redo() = 0;
};

//	이렇게 만드는건 생각없는것도 그거지만 방법이 없구만 그랴
struct WORK_VALUE{
	IUndo*		UndoClass;		//	Undo를 하게 될 클래스
	DWORD		ObjPValue;		//	변화된 오브젝트의 포인터값
	DWORD		State;
	D3DXVECTOR3	Pos;			//	포지션값 ( 1.옮겨질 위치 2.마우스 포인트 위치)
	D3DXVECTOR3	Rotate;			//	회전 값
	DWORD		Prior;			//	우선순위값

	FLOAT		ExtraValue1;	//	필요한 값1 (이 값의 크기는 앞으로 어떤 값이 들어가게 될지는 아무도 모르는 것이므로 4바이트다)
	FLOAT		ExtraValue2;	//	필요한 값2 (이 값의 크기는 앞으로 어떤 값이 들어가게 될지는 아무도 모르는 것이므로 4바이트다)
	DWORD		ExtraIValue1;
	DWORD		ExtraIValue2;

	WORK_VALUE()
	{
		UndoClass = NULL;
		State = 0;
		Prior = 0;
		ExtraValue1 = ExtraValue2 = 0.f;
		ObjPValue = 0;
	}
};

typedef vector<WORK_VALUE>				WORKING_LIST;
typedef vector<WORK_VALUE>::iterator	WORKLIST_ITERATOR;

class CWorkManager  
{
private:
	WORD				m_wdSaveCount;
	WORD				m_wdCurrentCount;
	WORKING_LIST		m_vWorkList;
	DWORD				m_dwListRealPos;
public:
	WORK_VALUE			m_sWorkValue;

	inline BOOL			Undo()		   
	{ 
		if ( m_wdCurrentCount	> 0 ) 
			m_wdCurrentCount--; 
		else
			return FALSE;
		return TRUE;
	}
	inline BOOL			Redo()		   
	{ 
		if ( m_wdSaveCount		> m_wdCurrentCount ) 
			m_wdCurrentCount++; 
		else
			return FALSE;
		return TRUE;
	}

	//	Counter
	inline WORD			GetListCurrent()			{ return m_wdCurrentCount; }
	inline void			SetListCurrentCnt()				{ m_wdCurrentCount++; }
	inline WORD			GetListCount()				{ return m_wdSaveCount; }
	inline void			SetListCount()				{ m_wdSaveCount = m_wdCurrentCount; }
	inline void			SetListRealCnt()			{ m_dwListRealPos = m_vWorkList.size(); }
	inline DWORD		GetListRealCnt()			{ return m_dwListRealPos; }

	inline IUndo*		GetUndoObj( int Index )		{ return m_vWorkList[Index].UndoClass; }
	
	//	List Value
	//	아래의 값은 검색이 필요하다. 현재의 리스트가 그냥 인덱스 형식이 아니라 
	//	같은 인덱스를 가진 것이 여러개 존재하기 때문에 
//	inline DWORD		GetCurrentState( int Index ){ return m_vWorkList[].; }
	//inline BOOL			Check();
	int					AddWorkList();
	int					ChangeListValue( int Index );
	
	inline void			ChangeListExIVal1( int Index , int Value ) { m_vWorkList[Index].ExtraIValue1 = Value; }
	
	int					SetWorkList();
	int					SearchWorkList( int nIndex, BOOL IsPrior = FALSE );
	int					SearchWorkList( D3DXVECTOR3 Pos, int Prior );

	BOOL				GetWorkValue( const int nPrior, int &nCnt );		//	m_sWorkValue에 값을 넘겨준다.

	CWorkManager();
	virtual ~CWorkManager();
};
#	endif//__LENAJANG_BEAST

#endif // !defined(AFX_WORKMANAGER_H__AD635C6A_31C2_4AA3_9FA7_E882AFAFEA73__INCLUDED_)
