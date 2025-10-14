#include "stdafx.h"
#include "Command.h"

//////////////////////////////////////////////////////////////////////////
// 기본 커맨드 클래스
//////////////////////////////////////////////////////////////////////////

CCommand::CCommand() : m_nCommandType(-1)
{
}

CCommand::~CCommand()
{
}

//////////////////////////////////////////////////////////////////////////
// 커맨드 매니저
//////////////////////////////////////////////////////////////////////////

CCommandMgr::CCommandMgr()
{
	m_nCurCommandType = -1;
}

CCommandMgr::~CCommandMgr()
{
}

void CCommandMgr::EndCommand()
{
}

void CCommandMgr::InsertCommand(CCommand* pCommand)
{
	if(pCommand != NULL)
	{
		// Full일 경우 Push
		if(m_vecCommand.size() >= MAX_COMMAND)
			m_vecCommand.erase(m_vecCommand.begin());

		// 커맨드 셋팅
		m_vecCommand.push_back(SpCommand(pCommand));

		// 새 커맨드 셋팅되면 Redo정보는 삭제
		m_vecUndoCommand.clear();

		// CurCommandType 초기화
		m_nCurCommandType = -1;
	}
}

void CCommandMgr::UndoCommand()
{
	if(m_vecCommand.size() > 0)
	{
		SpCommand pSpCommand = m_vecCommand[m_vecCommand.size() - 1];
		pSpCommand->Undo();
		m_vecUndoCommand.push_back(pSpCommand);
		m_vecCommand.pop_back();
	}
}

void CCommandMgr::RedoCommand()
{
	if(m_vecUndoCommand.size() > 0)
	{
		SpCommand pSpCommand = m_vecUndoCommand[m_vecUndoCommand.size() - 1];
		pSpCommand->Excute();
		m_vecCommand.push_back(pSpCommand);
		m_vecUndoCommand.pop_back();
	}
}

void CCommandMgr::ClearAllCommand()
{
	m_vecCommand.clear();
	m_vecUndoCommand.clear();
}
