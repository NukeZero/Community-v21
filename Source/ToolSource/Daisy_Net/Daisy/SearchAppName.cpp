// SearchAppName.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceView.h"
#include "daisy.h"
#include "DaisyDoc.h"
#include "DaisyView.h"
#include "Daisy.h"
#include "SearchAppName.h"
#include ".\searchappname.h"

// CSearchAppName 대화 상자입니다.


IMPLEMENT_DYNAMIC(CSearchAppName, CDialog)
CSearchAppName::CSearchAppName(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchAppName::IDD, pParent)
{
}

CSearchAppName::~CSearchAppName()
{
}

void CSearchAppName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_Edit_Search);
	DDX_Control(pDX, IDC_LIST_SEARCH_RESULT, m_List_SearchResult);
}


BEGIN_MESSAGE_MAP(CSearchAppName, CDialog)
	ON_BN_CLICKED(IDC_BT_SEARCH, OnBnClickedBtSearch)
	ON_LBN_DBLCLK(IDC_LIST_SEARCH_RESULT, OnLbnDblclkListSearchResult)
	ON_LBN_SELCHANGE(IDC_LIST_SEARCH_RESULT, OnLbnSelchangeListSearchResult)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, OnEnChangeEditSearch)
END_MESSAGE_MAP()


// CSearchAppName 메시지 처리기입니다.

void CSearchAppName::OnBnClickedBtSearch()
{
	//검색버튼을 눌렀~다.
	SearchResultList();
}

void CSearchAppName::SearchResultList()
{
	CString strSearchTemp;
	m_Edit_Search.GetWindowText(strSearchTemp);//검색 단어 가져오기
	if(m_List_SearchResult.GetCount() != 0)//검색 결과창에 검색단어가 있다면 
	{
		m_List_SearchResult.ResetContent();	//리스트 박스 리셋
	}
	for( WndAppletIter iter = CWndBase::m_resMng.m_mapWndApplet.begin(); iter != CWndBase::m_resMng.m_mapWndApplet.end(); ++iter )
	{
		CString strDef = CScript::GetFindIdStr( _T( "APP_" ), iter->first );
		if(!strncmp(strDef,strSearchTemp,7))
		{
			m_List_SearchResult.AddString(strDef);//검색단어 추출 APP_포함 총7글자 검색
		}
	}
	int nCnt = m_List_SearchResult.GetCount();
	//검색결과가 없을때
	if(nCnt == 0)
	{
		m_List_SearchResult.AddString("검색결과 없음");
	}
}

BOOL CSearchAppName::OnInitDialog()
{
	CDialog::OnInitDialog();
	//APP_를 기본으로 하는 검색창
	m_Edit_Search.SetWindowText("APP_");
	return TRUE;  
}
void CSearchAppName::OpenApplet(LPWNDAPPLET lpWndApplet)
{	
	// 작업할 창을 선택했으니 열어준다.
	if( lpWndApplet->pWndBase == NULL )
	{
		CWndDialog* pWndDialog = new CWndDialog;
		lpWndApplet->pWndBase = pWndDialog;
		pWndDialog->m_lpWndApplet = lpWndApplet;
		pWndDialog->InitDialog(	g_pMainFrame->m_pDaisyView->CView::GetSafeHwnd(), lpWndApplet->dwWndId );
	}
	else
	{
		lpWndApplet->pWndBase->SetFocus();
	}
	
	//검색창은 사라진다.
	CDialog::OnOK();
}

void CSearchAppName::OnLbnDblclkListSearchResult()
{
	CString strSelectApp;

	//리스트 박스 더블클릭으로,,,
	int nCnt = m_List_SearchResult.GetCurSel();
	m_List_SearchResult.GetText(nCnt,strSelectApp);
	for( WndAppletIter iter = CWndBase::m_resMng.m_mapWndApplet.begin(); iter != CWndBase::m_resMng.m_mapWndApplet.end(); ++iter )
	{
		CString strDef = CScript::GetFindIdStr( _T( "APP_" ), iter->first );
		//선택한 APP를 기존 APP리스트랑 비교해서 같은걸 찾는다.
		if(!strcmp(strDef,strSelectApp))
		{
			//작업할 창 선택하고 
			OpenApplet(iter->second);
		}
	}
}

void CSearchAppName::OnLbnSelchangeListSearchResult()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CSearchAppName::OnEnChangeEditSearch()
{
	//여기엔 언젠간 코드를 넣을것이다.
}

void CSearchAppName::OnOK()
{
	// Enter로 검색 버튼을 누르는 함수를 호출 했다.
	OnBnClickedBtSearch();
}
