#include <boost/shared_ptr.hpp>

// 기본 커맨드 클래스
class CCommand
{
private:
	int m_nCommandType;

public:
	CCommand();
	virtual ~CCommand() = 0;	// 상속객체만 사용할 것
	
public:
	inline int GetCommandType() {return m_nCommandType;};
	inline void SetCommandType(int nCommandType) {m_nCommandType = nCommandType;};
	
	virtual void Excute() = 0;
	virtual void Undo() = 0;
};

// 사용자 정의 커맨드 클래스 템플릿
template <typename T, typename U>
class CUserCommand : public CCommand
{
public:
	typedef U (*UserGetElemFunc) (T);		// 이전 데이터로 부터 변경된 데이터를 찾기 위해 사용자쪽에서 정의해야 할 함수
	typedef void (*UserExcuteFunc) (T, U);	// Redo시 T,U를 가지고 동작하는 부분 사용자 정의 함수
	typedef void (*UserUndoFunc) (T, U);	// Undo시 T,U를 가지고 동작하는 부분 사용자 정의 함수	(Redo/Undo의 동작이 같다면 한 함수로 같이 쓰면된다.)

private:
	map<T, U> m_mapOldData;
	map<T, U> m_mapChgData;

	UserGetElemFunc m_GetElemFunc;
	UserExcuteFunc m_ExcuteFunc;
	UserUndoFunc m_UndoFunc;

public:
	CUserCommand(UserGetElemFunc GetElemFunc = NULL, UserExcuteFunc ExcuteFunc = NULL, UserUndoFunc UndoFunc = NULL);
	~CUserCommand();

public:
	void Initialize(int nCommandType, map<T, U>* pmapOldData);
	void Excute();
	void Undo();	
};

// 사용자 정의 커맨드 클래스 템플릿 정의
template <typename T, typename U>
CUserCommand<T, U>::CUserCommand(UserGetElemFunc GetElemFunc, UserExcuteFunc ExcuteFunc, UserUndoFunc UndoFunc) : m_GetElemFunc(GetElemFunc), m_ExcuteFunc(ExcuteFunc), m_UndoFunc(UndoFunc)
{
}

template <typename T, typename U>
CUserCommand<T, U>::~CUserCommand()
{
}

template <typename T, typename U>
void CUserCommand<T, U>::Initialize(int nCommandType, map<T, U>* pmapOldData)
{
	SetCommandType(nCommandType);

	if(pmapOldData != NULL)
	{
		for(map<T, U>::const_iterator i=pmapOldData->begin(); i!=pmapOldData->end(); ++i)
		{
			T Key = i->first;
			U OldElem = i->second;
			U ChgElem;
			
			if(m_GetElemFunc)
				ChgElem = m_GetElemFunc(Key);

			m_mapOldData.insert(map<T, U>::value_type(Key, OldElem));
			m_mapChgData.insert(map<T, U>::value_type(Key, ChgElem));
		}
	}
}

template <typename T, typename U>
void CUserCommand<T, U>::Excute()
{
	for(map<T, U>::const_iterator i=m_mapChgData.begin(); i!=m_mapChgData.end(); ++i)
	{
		T Key = i->first;
		U ChgElem = i->second;
		
		if(m_ExcuteFunc)
			m_ExcuteFunc(Key, ChgElem);
	}
}

template <typename T, typename U>
void CUserCommand<T, U>::Undo()
{
	for(map<T, U>::const_iterator i=m_mapOldData.begin(); i!=m_mapOldData.end(); ++i)
	{
		T Key = i->first;
		U OldElem = i->second;
		
		if(m_UndoFunc)
			m_UndoFunc(Key, OldElem);
	}
}

typedef	boost::shared_ptr<CCommand>	SpCommand;	// smart pointer for CCommand
typedef	vector<SpCommand> VecSpCommand;

// 커맨드 매니저
class CCommandMgr
{
private:
	enum {MAX_COMMAND = 30};

	VecSpCommand m_vecCommand;
	VecSpCommand m_vecUndoCommand;

	int m_nCurCommandType;	// 현재 동작중인 CommandType

public:
	CCommandMgr();
	virtual ~CCommandMgr() = 0;	// 상속객체만 사용할 것

protected:
	void InsertCommand(CCommand* pCommand);

public:
	virtual void EndCommand() = 0;	// Command의 종료를 받는다. 상속객체에서 Insert할 Command를 선택 생성 하도록 재정의 될 것.

	inline int GetCurCommandType() {return m_nCurCommandType;};
	inline void SetCurCommandType(int nCurCommandType) {m_nCurCommandType = nCurCommandType;};

	void UndoCommand();
	void RedoCommand();
	void ClearAllCommand();
};

