// DialogProgress.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Beast.h"
#include "DialogProgress.h"


// CDialogProgress 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogProgress, CDialog)
CDialogProgress::CDialogProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProgress::IDD, pParent)
{
}

CDialogProgress::~CDialogProgress()
{
}

void CDialogProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_kProgress);
}

BEGIN_MESSAGE_MAP(CDialogProgress, CDialog)
END_MESSAGE_MAP()


// CDialogProgress 메시지 처리기입니다.
