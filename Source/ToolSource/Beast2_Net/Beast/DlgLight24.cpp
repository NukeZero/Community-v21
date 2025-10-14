// DlgLight24.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgLight24.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLight24 dialog


CDlgLight24::CDlgLight24(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLight24::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLight24)
	// 너무 무대뽀 인듯 -_-그러게 왜 24시간 단위를 빼달래!!
	m_fAR0 = 0.3f;
	m_fAR1 = 0.3f;
	m_fAR2 = 0.3f;
	m_fAR3 = 0.3f;
	m_fAR4 = 0.3f;
	m_fAR5 = 0.3f;
	m_fAR6 = 0.4f;
	m_fAR7 = 0.5f;
	m_fAR8 = 0.5f;
	m_fAR9 = 0.5f;
	m_fAR10 = 0.5f;
	m_fAR11 = 0.6f;
	m_fAR12 = 0.6f;
	m_fAR13 = 0.6f;
	m_fAR14 = 0.6f;
	m_fAR15 = 0.5f;
	m_fAR16 = 0.5f;
	m_fAR17 = 0.5f;
	m_fAR18 = 0.4f;
	m_fAR19 = 0.4f;
	m_fAR20 = 0.35f;
	m_fAR21 = 0.33f;
	m_fAR22 = 0.31f;
	m_fAR23 = 0.3f;

	m_fAG0 = 0.3f;
	m_fAG1 = 0.3f;
	m_fAG2 = 0.3f;
	m_fAG3 = 0.3f;
	m_fAG4 = 0.3f;
	m_fAG5 = 0.3f;
	m_fAG6 = 0.4f;
	m_fAG7 = 0.5f;
	m_fAG8 = 0.5f;
	m_fAG9 = 0.5f;
	m_fAG10 = 0.5f;
	m_fAG11 = 0.6f;
	m_fAG12 = 0.6f;
	m_fAG13 = 0.6f;
	m_fAG14 = 0.6f;
	m_fAG15 = 0.5f;
	m_fAG16 = 0.5f;
	m_fAG17 = 0.5f;
	m_fAG18 = 0.4f;
	m_fAG19 = 0.4f;
	m_fAG20 = 0.35f;
	m_fAG21 = 0.33f;
	m_fAG22 = 0.31f;
	m_fAG23 = 0.3f;

	m_fAB0 = 0.4f;
	m_fAB1 = 0.4f;
	m_fAB2 = 0.4f;
	m_fAB3 = 0.4f;
	m_fAB4 = 0.4f;
	m_fAB5 = 0.4f;
	m_fAB6 = 0.4f;
	m_fAB7 = 0.5f;
	m_fAB8 = 0.5f;
	m_fAB9 = 0.5f;
	m_fAB10 = 0.5f;
	m_fAB11 = 0.6f;
	m_fAB12 = 0.6f;
	m_fAB13 = 0.6f;
	m_fAB14 = 0.6f;
	m_fAB15 = 0.5f;
	m_fAB16 = 0.5f;
	m_fAB17 = 0.4f;
	m_fAB18 = 0.4f;
	m_fAB19 = 0.4f;
	m_fAB20 = 0.35f;
	m_fAB21 = 0.3f;
	m_fAB22 = 0.3f;
	m_fAB23 = 0.4f;

	m_fDR0 = 0.4f;
	m_fDR1 = 0.4f;
	m_fDR2 = 0.4f;
	m_fDR3 = 0.4f;
	m_fDR4 = 0.4f;
	m_fDR5 = 0.4f;
	m_fDR6 = 0.5f;
	m_fDR7 = 0.7f;
	m_fDR8 = 0.8f;
	m_fDR9 = 0.9f;
	m_fDR10 = 1.0f;
	m_fDR11 = 1.0f;
	m_fDR12 = 1.0f;
	m_fDR13 = 1.0f;
	m_fDR14 = 1.0f;
	m_fDR15 = 1.0f;
	m_fDR16 = 0.9f;
	m_fDR17 = 0.9f;
	m_fDR18 = 0.6f;
	m_fDR19 = 0.5f;
	m_fDR20 = 0.45f;
	m_fDR21 = 0.43f;
	m_fDR22 = 0.41f;
	m_fDR23 = 0.4f;

	m_fDG0 = 0.4f;
	m_fDG1 = 0.4f;
	m_fDG2 = 0.4f;
	m_fDG3 = 0.4f;
	m_fDG4 = 0.4f;
	m_fDG5 = 0.4f;
	m_fDG6 = 0.5f;
	m_fDG7 = 0.7f;
	m_fDG8 = 0.8f;
	m_fDG9 = 0.9f;
	m_fDG10 = 1.0f;
	m_fDG11 = 1.0f;
	m_fDG12 = 1.0f;
	m_fDG13 = 1.0f;
	m_fDG14 = 1.0f;
	m_fDG15 = 1.0f;
	m_fDG16 = 0.9f;
	m_fDG17 = 0.6f;
	m_fDG18 = 0.6f;
	m_fDG19 = 0.5f;
	m_fDG20 = 0.45f;
	m_fDG21 = 0.43f;
	m_fDG22 = 0.41f;
	m_fDG23 = 0.4f;

	m_fDB0 = 0.5f;
	m_fDB1 = 0.5f;
	m_fDB2 = 0.5f;
	m_fDB3 = 0.5f;
	m_fDB4 = 0.5f;
	m_fDB5 = 0.5f;
	m_fDB6 = 0.6f;
	m_fDB7 = 0.7f;
	m_fDB8 = 0.8f;
	m_fDB9 = 0.9f;
	m_fDB10 = 1.0f;
	m_fDB11 = 1.0f;
	m_fDB12 = 1.0f;
	m_fDB13 = 1.0f;
	m_fDB14 = 1.0f;
	m_fDB15 = 1.0f;
	m_fDB16 = 0.9f;
	m_fDB17 = 0.2f;
	m_fDB18 = 0.4f;
	m_fDB19 = 0.4f;
	m_fDB20 = 0.4f;
	m_fDB21 = 0.5f;
	m_fDB22 = 0.5f;
	m_fDB23 = 0.5f;

	
	//}}AFX_DATA_INIT

}


void CDlgLight24::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLight24)
	DDX_Text(pDX, IDC_EDIT_AB_1, m_fAB0);
	DDX_Text(pDX, IDC_EDIT_AB_2, m_fAB1);
	DDX_Text(pDX, IDC_EDIT_AB_3, m_fAB2);
	DDX_Text(pDX, IDC_EDIT_AB_4, m_fAB3);
	DDX_Text(pDX, IDC_EDIT_AB_5, m_fAB4);
	DDX_Text(pDX, IDC_EDIT_AB_6, m_fAB5);
	DDX_Text(pDX, IDC_EDIT_AB_7, m_fAB6);
	DDX_Text(pDX, IDC_EDIT_AB_8, m_fAB7);
	DDX_Text(pDX, IDC_EDIT_AB_9, m_fAB8);
	DDX_Text(pDX, IDC_EDIT_AB_10, m_fAB9);
	DDX_Text(pDX, IDC_EDIT_AB_11, m_fAB10);
	DDX_Text(pDX, IDC_EDIT_AB_12, m_fAB11);
	DDX_Text(pDX, IDC_EDIT_AB_13, m_fAB12);
	DDX_Text(pDX, IDC_EDIT_AB_14, m_fAB13);
	DDX_Text(pDX, IDC_EDIT_AB_15, m_fAB14);
	DDX_Text(pDX, IDC_EDIT_AB_16, m_fAB15);
	DDX_Text(pDX, IDC_EDIT_AB_17, m_fAB16);
	DDX_Text(pDX, IDC_EDIT_AB_18, m_fAB17);
	DDX_Text(pDX, IDC_EDIT_AB_19, m_fAB18);
	DDX_Text(pDX, IDC_EDIT_AB_20, m_fAB19);
	DDX_Text(pDX, IDC_EDIT_AB_21, m_fAB20);
	DDX_Text(pDX, IDC_EDIT_AB_22, m_fAB21);
	DDX_Text(pDX, IDC_EDIT_AB_23, m_fAB22);
	DDX_Text(pDX, IDC_EDIT_AB_24, m_fAB23);

	DDX_Text(pDX, IDC_EDIT_AR_1, m_fAR0);
	DDX_Text(pDX, IDC_EDIT_AR_2, m_fAR1);
	DDX_Text(pDX, IDC_EDIT_AR_3, m_fAR2);
	DDX_Text(pDX, IDC_EDIT_AR_4, m_fAR3);
	DDX_Text(pDX, IDC_EDIT_AR_5, m_fAR4);
	DDX_Text(pDX, IDC_EDIT_AR_6, m_fAR5);
	DDX_Text(pDX, IDC_EDIT_AR_7, m_fAR6);
	DDX_Text(pDX, IDC_EDIT_AR_8, m_fAR7);
	DDX_Text(pDX, IDC_EDIT_AR_9, m_fAR8);
	DDX_Text(pDX, IDC_EDIT_AR_10, m_fAR9);
	DDX_Text(pDX, IDC_EDIT_AR_11, m_fAR10);
	DDX_Text(pDX, IDC_EDIT_AR_12, m_fAR11);
	DDX_Text(pDX, IDC_EDIT_AR_13, m_fAR12);
	DDX_Text(pDX, IDC_EDIT_AR_14, m_fAR13);
	DDX_Text(pDX, IDC_EDIT_AR_15, m_fAR14);
	DDX_Text(pDX, IDC_EDIT_AR_16, m_fAR15);
	DDX_Text(pDX, IDC_EDIT_AR_17, m_fAR16);
	DDX_Text(pDX, IDC_EDIT_AR_18, m_fAR17);
	DDX_Text(pDX, IDC_EDIT_AR_19, m_fAR18);
	DDX_Text(pDX, IDC_EDIT_AR_20, m_fAR19);
	DDX_Text(pDX, IDC_EDIT_AR_21, m_fAR20);
	DDX_Text(pDX, IDC_EDIT_AR_22, m_fAR21);
	DDX_Text(pDX, IDC_EDIT_AR_23, m_fAR22);
	DDX_Text(pDX, IDC_EDIT_AR_24, m_fAR23);

	DDX_Text(pDX, IDC_EDIT_AG_1, m_fAG0);
	DDX_Text(pDX, IDC_EDIT_AG_2, m_fAG1);
	DDX_Text(pDX, IDC_EDIT_AG_3, m_fAG2);
	DDX_Text(pDX, IDC_EDIT_AG_4, m_fAG3);
	DDX_Text(pDX, IDC_EDIT_AG_5, m_fAG4);
	DDX_Text(pDX, IDC_EDIT_AG_6, m_fAG5);
	DDX_Text(pDX, IDC_EDIT_AG_7, m_fAG6);
	DDX_Text(pDX, IDC_EDIT_AG_8, m_fAG7);
	DDX_Text(pDX, IDC_EDIT_AG_9, m_fAG8);
	DDX_Text(pDX, IDC_EDIT_AG_10, m_fAG9);
	DDX_Text(pDX, IDC_EDIT_AG_11, m_fAG10);
	DDX_Text(pDX, IDC_EDIT_AG_12, m_fAG11);
	DDX_Text(pDX, IDC_EDIT_AG_13, m_fAG12);
	DDX_Text(pDX, IDC_EDIT_AG_14, m_fAG13);
	DDX_Text(pDX, IDC_EDIT_AG_15, m_fAG14);
	DDX_Text(pDX, IDC_EDIT_AG_16, m_fAG15);
	DDX_Text(pDX, IDC_EDIT_AG_17, m_fAG16);
	DDX_Text(pDX, IDC_EDIT_AG_18, m_fAG17);
	DDX_Text(pDX, IDC_EDIT_AG_19, m_fAG18);
	DDX_Text(pDX, IDC_EDIT_AG_20, m_fAG19);
	DDX_Text(pDX, IDC_EDIT_AG_21, m_fAG20);
	DDX_Text(pDX, IDC_EDIT_AG_22, m_fAG21);
	DDX_Text(pDX, IDC_EDIT_AG_23, m_fAG22);
	DDX_Text(pDX, IDC_EDIT_AG_24, m_fAG23);

	DDX_Text(pDX, IDC_EDIT_DB_1, m_fDB0);
	DDX_Text(pDX, IDC_EDIT_DB_2, m_fDB1);
	DDX_Text(pDX, IDC_EDIT_DB_3, m_fDB2);
	DDX_Text(pDX, IDC_EDIT_DB_4, m_fDB3);
	DDX_Text(pDX, IDC_EDIT_DB_5, m_fDB4);
	DDX_Text(pDX, IDC_EDIT_DB_6, m_fDB5);
	DDX_Text(pDX, IDC_EDIT_DB_7, m_fDB6);
	DDX_Text(pDX, IDC_EDIT_DB_8, m_fDB7);
	DDX_Text(pDX, IDC_EDIT_DB_9, m_fDB8);
	DDX_Text(pDX, IDC_EDIT_DB_10, m_fDB9);
	DDX_Text(pDX, IDC_EDIT_DB_11, m_fDB10);
	DDX_Text(pDX, IDC_EDIT_DB_12, m_fDB11);
	DDX_Text(pDX, IDC_EDIT_DB_13, m_fDB12);
	DDX_Text(pDX, IDC_EDIT_DB_14, m_fDB13);
	DDX_Text(pDX, IDC_EDIT_DB_15, m_fDB14);
	DDX_Text(pDX, IDC_EDIT_DB_16, m_fDB15);
	DDX_Text(pDX, IDC_EDIT_DB_17, m_fDB16);
	DDX_Text(pDX, IDC_EDIT_DB_18, m_fDB17);
	DDX_Text(pDX, IDC_EDIT_DB_19, m_fDB18);
	DDX_Text(pDX, IDC_EDIT_DB_20, m_fDB19);
	DDX_Text(pDX, IDC_EDIT_DB_21, m_fDB20);
	DDX_Text(pDX, IDC_EDIT_DB_22, m_fDB21);
	DDX_Text(pDX, IDC_EDIT_DB_23, m_fDB22);
	DDX_Text(pDX, IDC_EDIT_DB_24, m_fDB23);

	DDX_Text(pDX, IDC_EDIT_DR_1, m_fDR0);
	DDX_Text(pDX, IDC_EDIT_DR_2, m_fDR1);
	DDX_Text(pDX, IDC_EDIT_DR_3, m_fDR2);
	DDX_Text(pDX, IDC_EDIT_DR_4, m_fDR3);
	DDX_Text(pDX, IDC_EDIT_DR_5, m_fDR4);
	DDX_Text(pDX, IDC_EDIT_DR_6, m_fDR5);
	DDX_Text(pDX, IDC_EDIT_DR_7, m_fDR6);
	DDX_Text(pDX, IDC_EDIT_DR_8, m_fDR7);
	DDX_Text(pDX, IDC_EDIT_DR_9, m_fDR8);
	DDX_Text(pDX, IDC_EDIT_DR_10, m_fDR9);
	DDX_Text(pDX, IDC_EDIT_DR_11, m_fDR10);
	DDX_Text(pDX, IDC_EDIT_DR_12, m_fDR11);
	DDX_Text(pDX, IDC_EDIT_DR_13, m_fDR12);
	DDX_Text(pDX, IDC_EDIT_DR_14, m_fDR13);
	DDX_Text(pDX, IDC_EDIT_DR_15, m_fDR14);
	DDX_Text(pDX, IDC_EDIT_DR_16, m_fDR15);
	DDX_Text(pDX, IDC_EDIT_DR_17, m_fDR16);
	DDX_Text(pDX, IDC_EDIT_DR_18, m_fDR17);
	DDX_Text(pDX, IDC_EDIT_DR_19, m_fDR18);
	DDX_Text(pDX, IDC_EDIT_DR_20, m_fDR19);
	DDX_Text(pDX, IDC_EDIT_DR_21, m_fDR20);
	DDX_Text(pDX, IDC_EDIT_DR_22, m_fDR21);
	DDX_Text(pDX, IDC_EDIT_DR_23, m_fDR22);
	DDX_Text(pDX, IDC_EDIT_DR_24, m_fDR23);

	DDX_Text(pDX, IDC_EDIT_DG_1, m_fDG0);
	DDX_Text(pDX, IDC_EDIT_DG_2, m_fDG1);
	DDX_Text(pDX, IDC_EDIT_DG_3, m_fDG2);
	DDX_Text(pDX, IDC_EDIT_DG_4, m_fDG3);
	DDX_Text(pDX, IDC_EDIT_DG_5, m_fDG4);
	DDX_Text(pDX, IDC_EDIT_DG_6, m_fDG5);
	DDX_Text(pDX, IDC_EDIT_DG_7, m_fDG6);
	DDX_Text(pDX, IDC_EDIT_DG_8, m_fDG7);
	DDX_Text(pDX, IDC_EDIT_DG_9, m_fDG8);
	DDX_Text(pDX, IDC_EDIT_DG_10, m_fDG9);
	DDX_Text(pDX, IDC_EDIT_DG_11, m_fDG10);
	DDX_Text(pDX, IDC_EDIT_DG_12, m_fDG11);
	DDX_Text(pDX, IDC_EDIT_DG_13, m_fDG12);
	DDX_Text(pDX, IDC_EDIT_DG_14, m_fDG13);
	DDX_Text(pDX, IDC_EDIT_DG_15, m_fDG14);
	DDX_Text(pDX, IDC_EDIT_DG_16, m_fDG15);
	DDX_Text(pDX, IDC_EDIT_DG_17, m_fDG16);
	DDX_Text(pDX, IDC_EDIT_DG_18, m_fDG17);
	DDX_Text(pDX, IDC_EDIT_DG_19, m_fDG18);
	DDX_Text(pDX, IDC_EDIT_DG_20, m_fDG19);
	DDX_Text(pDX, IDC_EDIT_DG_21, m_fDG20);
	DDX_Text(pDX, IDC_EDIT_DG_22, m_fDG21);
	DDX_Text(pDX, IDC_EDIT_DG_23, m_fDG22);
	DDX_Text(pDX, IDC_EDIT_DG_24, m_fDG23);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLight24, CDialog)
	//{{AFX_MSG_MAP(CDlgLight24)
	ON_BN_CLICKED(IDAPPLY_24, On24Apply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLight24 message handlers

BOOL CDlgLight24::Init( CWorld* pWorld, const char* filename )
{
	// 24Light를 이용할 경우만 초기화 된다.
	_pWorld = pWorld;
	assert( _pWorld );

	BOOL b24 = Setup24Light( filename );
	BOOL bRead = ReadFile( filename );

	return ( b24 && bRead );
}

BOOL CDlgLight24::Setup24Light( const char* filename )
{
	char fullname[ 256 ];
	sprintf( fullname, "%s%s", filename, ".24l" );

	CScript script;
	if( script.Load( fullname ) == FALSE )
		return FALSE;

	for( int i = 0; i < 24; ++i )
	{
		_pWorld->m_k24Light[ i ].r1 = script.GetFloat( );
		_pWorld->m_k24Light[ i ].g1 = script.GetFloat( );
		_pWorld->m_k24Light[ i ].b1 = script.GetFloat( );
		_pWorld->m_k24Light[ i ].r2 = script.GetFloat( );
		_pWorld->m_k24Light[ i ].g2 = script.GetFloat( );
		_pWorld->m_k24Light[ i ].b2 = script.GetFloat( );
	}

	return TRUE;
}

void CDlgLight24::On24Apply() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );
	
}

BOOL CDlgLight24::SaveFile( const char* szFileName )
{
	char szFullName[ 256 ];
	sprintf( szFullName, "%s%s", szFileName, ".24l" );

	FILE* fp;
	// 월드 스크립트 저장 
	if( !( fp = fopen( szFullName, "wt" ) ) )
		return FALSE;

	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR0, m_fAG0, m_fAB0, m_fDR0, m_fDG0, m_fDB0 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR1, m_fAG1, m_fAB1, m_fDR1, m_fDG1, m_fDB1 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR2, m_fAG2, m_fAB2, m_fDR2, m_fDG2, m_fDB2 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR3, m_fAG3, m_fAB3, m_fDR3, m_fDG3, m_fDB3 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR4, m_fAG4, m_fAB4, m_fDR4, m_fDG4, m_fDB4 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR5, m_fAG5, m_fAB5, m_fDR5, m_fDG5, m_fDB5 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR6, m_fAG6, m_fAB6, m_fDR6, m_fDG6, m_fDB6 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR7, m_fAG7, m_fAB7, m_fDR7, m_fDG7, m_fDB7 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR8, m_fAG8, m_fAB8, m_fDR8, m_fDG8, m_fDB8 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR9, m_fAG9, m_fAB9, m_fDR9, m_fDG9, m_fDB9 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR10, m_fAG10, m_fAB10, m_fDR10, m_fDG10, m_fDB10 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR11, m_fAG11, m_fAB11, m_fDR11, m_fDG11, m_fDB11 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR12, m_fAG12, m_fAB12, m_fDR12, m_fDG12, m_fDB12 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR13, m_fAG13, m_fAB13, m_fDR13, m_fDG13, m_fDB13 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR14, m_fAG14, m_fAB14, m_fDR14, m_fDG14, m_fDB14 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR15, m_fAG15, m_fAB15, m_fDR15, m_fDG15, m_fDB15 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR16, m_fAG16, m_fAB16, m_fDR16, m_fDG16, m_fDB16 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR17, m_fAG17, m_fAB17, m_fDR17, m_fDG17, m_fDB17 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR18, m_fAG18, m_fAB18, m_fDR18, m_fDG18, m_fDB18 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR19, m_fAG19, m_fAB19, m_fDR19, m_fDG19, m_fDB19 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR20, m_fAG20, m_fAB20, m_fDR20, m_fDG20, m_fDB20 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR21, m_fAG21, m_fAB21, m_fDR21, m_fDG21, m_fDB21 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR22, m_fAG22, m_fAB22, m_fDR22, m_fDG22, m_fDB22 );
	fprintf( fp, _T( "%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f \n" ), m_fAR23, m_fAG23, m_fAB23, m_fDR23, m_fDG23, m_fDB23 );

	fclose( fp );

	return TRUE;

}

BOOL CDlgLight24::ReadFile( const char* filename )
{
	char fullname[ 256 ];
	sprintf( fullname, "%s%s", filename, ".24l" );

	CScript script;
	if( script.Load( fullname ) == FALSE )
		return FALSE;

	m_fAR0=script.GetFloat(); m_fAG0=script.GetFloat(); m_fAB0=script.GetFloat(); m_fDR0=script.GetFloat(); m_fDG0=script.GetFloat(); m_fDB0=script.GetFloat();
	m_fAR1=script.GetFloat(); m_fAG1=script.GetFloat(); m_fAB1=script.GetFloat(); m_fDR1=script.GetFloat(); m_fDG1=script.GetFloat(); m_fDB1=script.GetFloat();
	m_fAR2=script.GetFloat(); m_fAG2=script.GetFloat(); m_fAB2=script.GetFloat(); m_fDR2=script.GetFloat(); m_fDG2=script.GetFloat(); m_fDB2=script.GetFloat();
	m_fAR3=script.GetFloat(); m_fAG3=script.GetFloat(); m_fAB3=script.GetFloat(); m_fDR3=script.GetFloat(); m_fDG3=script.GetFloat(); m_fDB3=script.GetFloat();
	m_fAR4=script.GetFloat(); m_fAG4=script.GetFloat(); m_fAB4=script.GetFloat(); m_fDR4=script.GetFloat(); m_fDG4=script.GetFloat(); m_fDB4=script.GetFloat();
	m_fAR5=script.GetFloat(); m_fAG5=script.GetFloat(); m_fAB5=script.GetFloat(); m_fDR5=script.GetFloat(); m_fDG5=script.GetFloat(); m_fDB5=script.GetFloat();
	m_fAR6=script.GetFloat(); m_fAG6=script.GetFloat(); m_fAB6=script.GetFloat(); m_fDR6=script.GetFloat(); m_fDG6=script.GetFloat(); m_fDB6=script.GetFloat();
	m_fAR7=script.GetFloat(); m_fAG7=script.GetFloat(); m_fAB7=script.GetFloat(); m_fDR7=script.GetFloat(); m_fDG7=script.GetFloat(); m_fDB7=script.GetFloat();
	m_fAR8=script.GetFloat(); m_fAG8=script.GetFloat(); m_fAB8=script.GetFloat(); m_fDR8=script.GetFloat(); m_fDG8=script.GetFloat(); m_fDB8=script.GetFloat();
	m_fAR9=script.GetFloat(); m_fAG9=script.GetFloat(); m_fAB9=script.GetFloat(); m_fDR9=script.GetFloat(); m_fDG9=script.GetFloat(); m_fDB9=script.GetFloat();
	m_fAR10=script.GetFloat(); m_fAG10=script.GetFloat(); m_fAB10=script.GetFloat(); m_fDR10=script.GetFloat(); m_fDG10=script.GetFloat(); m_fDB10=script.GetFloat();
	m_fAR11=script.GetFloat(); m_fAG11=script.GetFloat(); m_fAB11=script.GetFloat(); m_fDR11=script.GetFloat(); m_fDG11=script.GetFloat(); m_fDB11=script.GetFloat();
	m_fAR12=script.GetFloat(); m_fAG12=script.GetFloat(); m_fAB12=script.GetFloat(); m_fDR12=script.GetFloat(); m_fDG12=script.GetFloat(); m_fDB12=script.GetFloat();
	m_fAR13=script.GetFloat(); m_fAG13=script.GetFloat(); m_fAB13=script.GetFloat(); m_fDR13=script.GetFloat(); m_fDG13=script.GetFloat(); m_fDB13=script.GetFloat();
	m_fAR14=script.GetFloat(); m_fAG14=script.GetFloat(); m_fAB14=script.GetFloat(); m_fDR14=script.GetFloat(); m_fDG14=script.GetFloat(); m_fDB14=script.GetFloat();
	m_fAR15=script.GetFloat(); m_fAG15=script.GetFloat(); m_fAB15=script.GetFloat(); m_fDR15=script.GetFloat(); m_fDG15=script.GetFloat(); m_fDB15=script.GetFloat();
	m_fAR16=script.GetFloat(); m_fAG16=script.GetFloat(); m_fAB16=script.GetFloat(); m_fDR16=script.GetFloat(); m_fDG16=script.GetFloat(); m_fDB16=script.GetFloat();
	m_fAR17=script.GetFloat(); m_fAG17=script.GetFloat(); m_fAB17=script.GetFloat(); m_fDR17=script.GetFloat(); m_fDG17=script.GetFloat(); m_fDB17=script.GetFloat();
	m_fAR18=script.GetFloat(); m_fAG18=script.GetFloat(); m_fAB18=script.GetFloat(); m_fDR18=script.GetFloat(); m_fDG18=script.GetFloat(); m_fDB18=script.GetFloat();
	m_fAR19=script.GetFloat(); m_fAG19=script.GetFloat(); m_fAB19=script.GetFloat(); m_fDR19=script.GetFloat(); m_fDG19=script.GetFloat(); m_fDB19=script.GetFloat();
	m_fAR20=script.GetFloat(); m_fAG20=script.GetFloat(); m_fAB20=script.GetFloat(); m_fDR20=script.GetFloat(); m_fDG20=script.GetFloat(); m_fDB20=script.GetFloat();
	m_fAR21=script.GetFloat(); m_fAG21=script.GetFloat(); m_fAB21=script.GetFloat(); m_fDR21=script.GetFloat(); m_fDG21=script.GetFloat(); m_fDB21=script.GetFloat();
	m_fAR22=script.GetFloat(); m_fAG22=script.GetFloat(); m_fAB22=script.GetFloat(); m_fDR22=script.GetFloat(); m_fDG22=script.GetFloat(); m_fDB22=script.GetFloat();
	m_fAR23=script.GetFloat(); m_fAG23=script.GetFloat(); m_fAB23=script.GetFloat(); m_fDR23=script.GetFloat(); m_fDG23=script.GetFloat(); m_fDB23=script.GetFloat();

	UpdateData( FALSE );

	return TRUE;
}