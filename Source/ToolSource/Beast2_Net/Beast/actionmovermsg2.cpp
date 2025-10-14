#include "stdafx.h"
#include "ActionMover.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
//	Action Message Process
//	액션 메시지를 받아 처리한다.
//	어떤 행위가 발생하는 시점에 대한 처리를 담당.
//	최적화를 위해서 이미 설정되어 있는 상태면 중복 처리 하지 않음
//	빗자루 비행용

//
int		CActionMover::ProcessActMsg2(  CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 )
{

	return 1;
}

