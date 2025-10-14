// DlgCalcWorldmap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Beast.h"
#include "DlgCalcWorldmap.h"
#include ".\dlgcalcworldmap.h"


// CDlgCalcWorldmap 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCalcWorldmap, CDialog)
CDlgCalcWorldmap::CDlgCalcWorldmap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCalcWorldmap::IDD, pParent)
	, m_editBitmapX1(0)
	, m_editBitmapX2(0)
	, m_editBitmapY1(0)
	, m_editBitmapY2(0)
	, m_editWorldmapX1(0)
	, m_editWorldmapX2(0)
	, m_editWorldmapY1(0)
	, m_editWorldmapY2(0)
{
}

CDlgCalcWorldmap::~CDlgCalcWorldmap()
{
}

void CDlgCalcWorldmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_editBitmapX1);
	DDV_MinMaxInt(pDX, m_editBitmapX1, 0, 1280);
	DDX_Text(pDX, IDC_EDIT2, m_editBitmapX2);
	DDV_MinMaxInt(pDX, m_editBitmapX2, 0, 1280);
	DDX_Text(pDX, IDC_EDIT3, m_editBitmapY1);
	DDV_MinMaxInt(pDX, m_editBitmapY1, 0, 960);
	DDX_Text(pDX, IDC_EDIT4, m_editBitmapY2);
	DDV_MinMaxInt(pDX, m_editBitmapY2, 0, 960);
	DDX_Text(pDX, IDC_EDIT5, m_editWorldmapX1);
	DDX_Text(pDX, IDC_EDIT6, m_editWorldmapX2);
	DDX_Text(pDX, IDC_EDIT7, m_editWorldmapY1);
	DDX_Text(pDX, IDC_EDIT8, m_editWorldmapY2);
	DDX_Control(pDX, IDC_LIST1, m_listResult);
	DDX_Control(pDX, IDC_LIST2, m_listHistory);
}


BEGIN_MESSAGE_MAP(CDlgCalcWorldmap, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCalcWorldmap 메시지 처리기입니다.


void CDlgCalcWorldmap::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );

	if( m_editBitmapX1 >= m_editBitmapX2 || m_editBitmapY1 >= m_editBitmapY2 )
	{
		AfxMessageBox( "CDlgCalcWorldMap:: incorrect Value. Check again. (bitmap x or y greater)" );
		return;
	}
	
	int sizeBitmapX = m_editBitmapX2 - m_editBitmapX1;
	int sizeBitmapY = m_editBitmapY2 - m_editBitmapY1;
	int sizeWorldX = m_editWorldmapX2 - m_editWorldmapX1;
	int sizeWorldY = m_editWorldmapY2 - m_editWorldmapY1;

	//비트맵 여백의 월드크기
	float extraX1 = (float)(m_editBitmapX1 * sizeWorldX / sizeBitmapX);
	float extraX2 = (float)(( 1280 - m_editBitmapX2 ) * sizeWorldX / sizeBitmapX);

	float extraY1 = (float)(m_editBitmapY1 * sizeWorldY / sizeBitmapY);
	float extraY2 = (float)(( 960 - m_editBitmapY2 ) * sizeWorldY / sizeBitmapY);
	
	int extraWorldX1 = m_editWorldmapX1 - (int)extraX1;
	int extraWorldX2 = m_editWorldmapX2 + (int)extraX2;

	int extraWorldY1 = m_editWorldmapY1 - (int)extraY1;
	int extraWorldY2 = m_editWorldmapY2 + (int)extraY2;

	m_listResult.ResetContent( );

	CString str, strTemp;
	m_listResult.AddString( "RESULT:" );

	str.Format( "SetRealPositionRect( %d, %d, %d, %d )", extraWorldX1, extraWorldY1, extraWorldX2, extraWorldY2 );
	m_listResult.AddString( str.GetString() );

	static int index = 0;
	++index;

	strTemp.Format( "-----------------------(%d)-----------------------------", index );
	m_listHistory.AddString( strTemp.GetString() );
	strTemp.Format( "Bitmap( %d, %d, %d, %d )", m_editBitmapX1, m_editBitmapX2, m_editBitmapY1, m_editBitmapY2 );
	m_listHistory.AddString( strTemp.GetString() );
	strTemp.Format( "Worldmap( %d, %d, %d, %d )", m_editWorldmapX1, m_editWorldmapX2, m_editWorldmapY1, m_editWorldmapY2 );
	m_listHistory.AddString( strTemp.GetString() );
	m_listHistory.AddString( str.GetString() );
    
	//OnOK();
}
