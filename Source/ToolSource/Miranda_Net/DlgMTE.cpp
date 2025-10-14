// DlgMTE.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Miranda.h"
#include "DlgMTE.h"


// CDlgMTE 대화 상자입니다.

IMPLEMENT_DYNCREATE(CDlgMTE, CDHtmlDialog)

CDlgMTE::CDlgMTE(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CDlgMTE::IDD, CDlgMTE::IDH, pParent)
	, m_iSelectCase(0)
{
}

CDlgMTE::~CDlgMTE()
{
}

void CDlgMTE::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_kComboCase);
	DDX_CBIndex(pDX, IDC_COMBO1, m_iSelectCase);
}

BOOL CDlgMTE::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// 이펙트,사운드가 플레이되는 시점 리스트 
	m_cmbCase.InsertString( 0, "create" );
	m_cmbCase.InsertString( 1, "battle" );

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BEGIN_MESSAGE_MAP(CDlgMTE, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDlgMTE)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CDlgMTE 메시지 처리기입니다.

HRESULT CDlgMTE::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

HRESULT CDlgMTE::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
