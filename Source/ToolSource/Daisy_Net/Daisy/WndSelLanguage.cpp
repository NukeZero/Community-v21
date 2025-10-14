// WndSelLanguage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Daisy.h"
#include "DaisyDoc.h"
#include "DaisyView.h"
#include "WndSelLanguage.h"
#include ".\wndsellanguage.h"


// CWndSelLanguage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWndSelLanguage, CDialog)
CWndSelLanguage::CWndSelLanguage(CWnd* pParent /*=NULL*/)
	: CDialog(CWndSelLanguage::IDD, pParent)
{
}

CWndSelLanguage::~CWndSelLanguage()
{
}

void CWndSelLanguage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LANGUAGE, m_combo_Laguage_list);
}


BEGIN_MESSAGE_MAP(CWndSelLanguage, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CWndSelLanguage 메시지 처리기입니다.

BOOL CWndSelLanguage::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString		strLanguage[LANG_MAX] = { "한국", "미국", "일본",
										  "태국", "대만", "독일",
										  "칠레","프랑스","홍콩",
										  "브라질","베트남","러시아",
									   	  "필리핀","인도네시아"};

	for(int nLang = 0; nLang < LANG_MAX; nLang++)
	{
		m_combo_Laguage_list.AddString(strLanguage[nLang]);
	}

	m_combo_Laguage_list.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWndSelLanguage::OnBnClickedOk()
{
	CWndBase::m_resMng.SetSelectLang(m_combo_Laguage_list.GetCurSel());
	OnOK();
}
