// FramePanel.cpp : implementation file
//

#include "stdafx.h"
#include "SFXEditor.h"
#include "FramePanel.h"
#include "sfxbase.h"
#include "texpanel.h"
#include "childfrm.h"
#include "DialogAddPart.h"
#include "ValueChange.h"
#include "LayerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFramePanel dialog
CFramePanel*   g_pFramePanel = NULL;

int xgap[3]={12,6,3};

CFramePanel::CFramePanel(CWnd* pParent /*=NULL*/)
	: CDialogBar()
{
	//{{AFX_DATA_INIT(CFramePanel)
	//}}AFX_DATA_INIT
	m_pDoc=NULL;
	m_pView=NULL;

	m_bLBut=FALSE;
	m_bRBut=FALSE;
	m_bMBut=FALSE;

	m_nLeftFrame=m_nTopObj=0;
	m_nCurFrame=m_nCurPart=0;
//	m_pKey=0;
	m_bKeyLocked=FALSE;
	m_pSfxPart=NULL;
	m_nGapLevel=0;
	m_bSelecting=FALSE;
	m_pClickedKey=NULL;

#ifdef __AALLSELECT
	m_bLayerDragDrop = FALSE;
	m_nSfxPartClipBoard = -1;
	m_nSfxPartLayer = -1;
#endif // __AALLSELECT

	g_pFramePanel = this;
}


void CFramePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFramePanel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFramePanel, CDialogBar)
	//{{AFX_MSG_MAP(CFramePanel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_POPUPMENU, OnPopupmenu)
	ON_UPDATE_COMMAND_UI(ID_POPUPMENU, OnUpdatePopupmenu)
	ON_COMMAND(ID_VALUTCHANGE, OnValutchange)
	ON_COMMAND(ID_NAMECHANGE, OnNamechange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFramePanel message handlers

void CFramePanel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT	rect;	
	GetClientRect( &rect );
	int nWidth  = rect.right-rect.left;
	int nHeight = rect.bottom-rect.top;
	int i,j;

	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	CBitmap *pOldBitmap;
	pOldBitmap = memDC.SelectObject( &m_hBitmap );

	CDC	*pDC = &memDC;
	pDC->FillSolidRect(0, 0, nWidth, nHeight, 0x00c8d0d4 );

	CFont font, *pOldFont;
	font.CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE,
					 FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                     VARIABLE_PITCH, "굴림" );
	pOldFont = pDC->SelectObject( &font );

	CPen pen,pen2, *pOldPen;
	pen.CreatePen( PS_SOLID, 1, RGB(170, 170, 170) );
	pen2.CreatePen( PS_SOLID, 1, RGB(255, 100, 100) );
	pOldPen = pDC->SelectObject( &pen );

	CBrush brush, brush2, brush3, brush4, brush5, *pOldBrush;
	brush.CreateSolidBrush(RGB(170, 150, 150));
	brush2.CreateSolidBrush(RGB(255, 100, 100));
	brush3.CreateSolidBrush(RGB(100, 100, 100));
	brush4.CreateSolidBrush(RGB(255, 255, 100));
	brush5.CreateSolidBrush(RGB(50, 50, 50));

	int tempx,tempy;
	CString str;
	CSfxBase* pSfxBase=g_SfxMng.GetSfxBase(0);
	pDC->SetBkMode(TRANSPARENT);

	for(i=100,tempx=m_nLeftFrame;i<nWidth;i+=xgap[m_nGapLevel],tempx++)
	{
		if(tempx%5==0)
		{
			RECT rt;
			rt.top=16; rt.bottom=nHeight; rt.left=i; rt.right=i+xgap[m_nGapLevel];
			pDC->FillRect( &rt ,&brush);
		}
	}
	
	// 선택된 열 하이라이트 시킴 
	RECT rt;
	rt.top=(m_nCurPart-m_nTopObj)*16+16; rt.bottom=rt.top+16; rt.left=100; rt.right= nWidth;
	pDC->FillRect( &rt ,&brush3);

	for(i=100,tempx=m_nLeftFrame;i<nWidth;i+=xgap[m_nGapLevel],tempx++) 
	{
		pDC->MoveTo( i, 16 );
		pDC->LineTo( i, nHeight );
	}

	// 선택된 프레임
	if(m_nLeftFrame<=m_nCurFrame && m_nTopObj<=m_nCurPart) {
//		m_nLeftFrame m_nTopObj
//		m_nCurFrame  m_nCurPart
		RECT rt;
		rt.top=(m_nCurPart-m_nTopObj)*16+16; rt.bottom=rt.top+16; rt.left=(m_nCurFrame-m_nLeftFrame)*xgap[m_nGapLevel]+100; rt.right=rt.left+xgap[m_nGapLevel];
		pDC->FillRect( &rt ,&brush5);
	}
	if(m_bSelecting) {
		CRect rect(m_ptSelectStart,m_ptSelectEnd);
		rect.NormalizeRect();
		rect.top=(rect.top-m_nTopObj)*16+16; rect.bottom=(rect.bottom-m_nTopObj)*16+16+16; rect.left=(rect.left-m_nLeftFrame)*xgap[m_nGapLevel]+100; rect.right=(rect.right-m_nLeftFrame)*xgap[m_nGapLevel]+100+xgap[m_nGapLevel];
		pDC->FillRect( &rect ,&brush3);
	}
	for(i=16,tempy=m_nTopObj;i<nHeight;i+=16,tempy++) {
		pDC->MoveTo( 0,i );
		pDC->LineTo( nWidth,i );
		if(pSfxBase->m_apParts.GetSize()>tempy) {
//			if(pSfxBase->Part(tempy)->m_strTex.IsEmpty()) {
				if(pSfxBase->Part(tempy)->m_nType==SFXPARTTYPE_BILL) {
					str.Format("%d B %s",tempy,pSfxBase->Part(tempy)->m_strName);
				}
				else if(pSfxBase->Part(tempy)->m_nType==SFXPARTTYPE_PARTICLE) {
					str.Format("%d P %s",tempy,pSfxBase->Part(tempy)->m_strName);
				}
				else if(pSfxBase->Part(tempy)->m_nType==SFXPARTTYPE_CUSTOMMESH) {
					str.Format("%d C %s",tempy,pSfxBase->Part(tempy)->m_strName);
				}
				else if(pSfxBase->Part(tempy)->m_nType==SFXPARTTYPE_MESH) {
					str.Format("%d M %s",tempy,pSfxBase->Part(tempy)->m_strName);
				}
/*
			}
			else {
				str.Format("%d %s",tempy,pSfxBase->Part(tempy)->m_strTex);
			}
*/
		}
		else {
			str.Format("%d",tempy);
		}
		pDC->TextOut( 0, i,str);
	}
	for(i=100,tempx=m_nLeftFrame;i<nWidth;i+=xgap[m_nGapLevel],tempx++) {
		if((tempx%5==0 && m_nGapLevel!=2) || (tempx%10==0 && m_nGapLevel==2)) {
			str.Format("%d",tempx);
			pDC->TextOut( i, 0,str);
		}
	}
#ifdef __AALLSELECT
	if( m_nSfxPartClipBoard >= 0 )
	{
		if( (m_nTopObj - m_nSfxPartClipBoard < 1) )
		{
			m_ptSelectStart.x = m_nLeftFrame;
			
			m_ptSelectEnd.y = m_ptSelectStart.y = m_nSfxPartClipBoard;
			m_ptSelectEnd.x = nWidth;
			CRect rect(m_ptSelectStart,m_ptSelectEnd);
			rect.NormalizeRect();
			rect.top=(rect.top-m_nTopObj)*16+16; rect.bottom=(rect.bottom-m_nTopObj)*16+16+16; rect.left=(rect.left-m_nLeftFrame)*xgap[m_nGapLevel]+100; rect.right=(rect.right-m_nLeftFrame)*xgap[m_nGapLevel]+100+xgap[m_nGapLevel];
			pDC->FillRect( &rect ,&brush3);
		}
	}

	for( int k=0; k<g_SfxMng.GetSfxBase(0)->m_apParts.GetSize(); k++ )
	{
		CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( k );
		if( pSfxPart ) 
		{
			if( pSfxPart->m_bUseing == FALSE )
			{
				if( (m_nTopObj - k < 1) )
				{
					CRect rect( CPoint( 0, k ),CPoint(nWidth, k));
					rect.NormalizeRect();
					rect.top=(rect.top-m_nTopObj)*16+16; rect.bottom=(rect.bottom-m_nTopObj)*16+16+16; rect.left=(rect.left-m_nLeftFrame)*xgap[m_nGapLevel]+100; rect.right=(rect.right-m_nLeftFrame)*xgap[m_nGapLevel]+100+xgap[m_nGapLevel];
					pDC->FillRect( &rect ,&brush);
				}
			}
		}
	}
		
#endif
	
	rect=CRect(0,0,95,16);
	pDC->DrawText(_T("Layer Add"),&rect,DT_END_ELLIPSIS);
	pDC->SelectObject( &pen2 );
	pOldBrush=pDC->SelectObject( &brush2 );
	for(i=16,tempy=m_nTopObj;i<nHeight;i+=16,tempy++) {
		CSfxPart* pSfxPart=g_SfxMng.GetSfxBase(0)->Part(tempy);
		if(pSfxPart)
			for(j=100,tempx=m_nLeftFrame;j<nWidth;j+=xgap[m_nGapLevel],tempx++) {
				SfxKeyFrame* pKey=pSfxPart->KeyByFrame(tempx);
				if(pKey) {
					BOOL bCheck=FALSE;
					for(int k=0;k<m_SelectedKeys.GetSize();k++) {
						if(((SfxKeyFrame*)(m_SelectedKeys[k]))==pKey) {
							bCheck=TRUE;
							break;
						}
					}
					if(bCheck) pDC->SelectObject( &brush4 );
					else pDC->SelectObject( &brush2 );
					pDC->Ellipse(j+1,i+3,j+xgap[m_nGapLevel]-1,i+13);
				}
			}
	}


	pDC->SelectObject(pOldBrush);
	pDC->SelectObject( pOldPen );


	pDC->SelectObject( pOldFont );
	dc.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY );
	memDC.SelectObject( pOldBitmap );

	// Do not call CDialogBar::OnPaint() for painting messages
}

BOOL CFramePanel::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return 0;
	return CDialogBar::OnEraseBkgnd(pDC);
}
void CFramePanel::UpdatePanel(void)
{
	CTexPanel* pTexPanel=&(((CChildFrame*)(m_pView->GetParent()))->m_WorkPanel.m_TexPanel);

#ifdef __AVALUEAPPLEY
	if(m_SelectedKeys.GetSize()==1) 
	{
		SfxKeyFrame* pKey=(SfxKeyFrame*)(m_SelectedKeys[0]);
		if( pKey == NULL ) return;
		pTexPanel->EnableControl(TRUE);
		CString strTemp;
		strTemp.Format("%g",pKey->vPos.x);
		pTexPanel->m_X.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPos.y);
		pTexPanel->m_Y.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPos.z);
		pTexPanel->m_Z.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPosRotate.x);
		pTexPanel->m_PosRotX.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPosRotate.y);
		pTexPanel->m_PosRotY.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPosRotate.z);
		pTexPanel->m_PosRotZ.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vScale.x);
		pTexPanel->m_ScaleX.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vScale.y);
		pTexPanel->m_ScaleY.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vScale.z);
		pTexPanel->m_ScaleZ.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vRotate.x);
		pTexPanel->m_AngleX.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vRotate.y);
		pTexPanel->m_AngleY.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vRotate.z);
		pTexPanel->m_AngleZ.SetWindowText(strTemp);
		strTemp.Format("%d",pKey->nAlpha);
		pTexPanel->m_Alpha.SetWindowText(strTemp);
		CSfxBase* pSfxBase = g_SfxMng.GetSfxBase( 0 );
		CSfxPart* pSfxPart = pSfxBase->Part( m_nCurPart );
		pTexPanel->EnableRadio( pSfxPart->m_nBillType );
		pTexPanel->EnableAlphaRadio( pSfxPart->m_nAlphaType );

		if(m_pSfxPart) {
			CString strTemp;
			strTemp.Format("%d",m_pSfxPart->m_nTexFrame);
			pTexPanel->m_TexFrame.SetWindowText(strTemp);
			strTemp.Format("%d",m_pSfxPart->m_nTexLoop);
			pTexPanel->m_TexLoop.SetWindowText(strTemp);
			if(m_pSfxPart->m_nType==SFXPARTTYPE_PARTICLE) {
				pTexPanel->EnableParticleEx(TRUE);
				pTexPanel->EnableParticle(TRUE);
				CSfxPartParticle* pPartParticle=(CSfxPartParticle*)m_pSfxPart;
				CString strTemp;
				strTemp.Format("%g",pPartParticle->m_vParticleAccel.x);
				pTexPanel->m_PAccelX.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vParticleAccel.y);
				pTexPanel->m_PAccelY.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vParticleAccel.z);
				pTexPanel->m_PAccelZ.SetWindowText(strTemp);
				
				strTemp.Format("%d",pPartParticle->m_nParticleFrameAppear);
				pTexPanel->m_PAppear.SetWindowText(strTemp);
				strTemp.Format("%d",pPartParticle->m_nParticleCreate);
				pTexPanel->m_PCreate.SetWindowText(strTemp);
				strTemp.Format("%d",pPartParticle->m_nParticleCreateNum);
				pTexPanel->m_PCreateNum.SetWindowText(strTemp);
				strTemp.Format("%d",pPartParticle->m_nParticleFrameDisappear);
				pTexPanel->m_PDisappear.SetWindowText(strTemp);
				strTemp.Format("%d",pPartParticle->m_nParticleFrameKeep);
				pTexPanel->m_PKeep.SetWindowText(strTemp);
				
				strTemp.Format("%g",pPartParticle->m_fParticleStartPosVar);
				pTexPanel->m_PPosVar.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fParticleStartPosVarY);
				pTexPanel->m_PPosVarY.SetWindowText(strTemp);
				
				strTemp.Format("%g",pPartParticle->m_vScaleSpeed.x);
				pTexPanel->m_PScaleSpeedX.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vScaleSpeed.y);
				pTexPanel->m_PScaleSpeedY.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vScaleSpeed.z);
				pTexPanel->m_PScaleSpeedZ.SetWindowText(strTemp);
				
				strTemp.Format("%g",pPartParticle->m_vScale.x);
				pTexPanel->m_PScaleX.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vScale.y);
				pTexPanel->m_PScaleY.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vScale.z);
				pTexPanel->m_PScaleZ.SetWindowText(strTemp);
				
				strTemp.Format("%g",pPartParticle->m_fParticleXZHigh);
				pTexPanel->m_PXZHigh.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fParticleXZLow);
				pTexPanel->m_PXZLow.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fParticleYHigh);
				pTexPanel->m_PYHigh.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fParticleYLow);
				pTexPanel->m_PYLow.SetWindowText(strTemp);

				strTemp.Format("%g",pPartParticle->m_vRotationLow.x );
				pTexPanel->m_PRotXLow.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vRotationHigh.x );
				pTexPanel->m_PRotXHigh.SetWindowText(strTemp);

				strTemp.Format("%g",pPartParticle->m_vRotationLow.y );
				pTexPanel->m_PRotYLow.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vRotationHigh.y );
				pTexPanel->m_PRotYHigh.SetWindowText(strTemp);

				strTemp.Format("%g",pPartParticle->m_vRotationLow.z );
				pTexPanel->m_PRotZLow.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vRotationHigh.z );
				pTexPanel->m_PRotZHigh.SetWindowText(strTemp);

				pTexPanel->m_bScalRepeat = pPartParticle->m_bRepeatScal;

				strTemp.Format("%g",pPartParticle->m_vScaleEnd.x );
				pTexPanel->m_ScalEndX.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vScaleEnd.y );
				pTexPanel->m_ScalEndY.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_vScaleEnd.z );
				pTexPanel->m_ScalEndZ.SetWindowText(strTemp);
				
				strTemp.Format("%g",pPartParticle->m_fScalSpeedXHigh );
				pTexPanel->m_ScalRepeatXHigh.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fScalSpeedYHigh );
				pTexPanel->m_ScalRepeatYHigh.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fScalSpeedZHigh );
				pTexPanel->m_ScalRepeatZHigh.SetWindowText(strTemp);

				strTemp.Format("%g",pPartParticle->m_fScalSpeedXLow );
				pTexPanel->m_ScalRepeatXLow.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fScalSpeedYLow );
				pTexPanel->m_ScalRepeatYLow.SetWindowText(strTemp);
				strTemp.Format("%g",pPartParticle->m_fScalSpeedZLow );
				pTexPanel->m_ScalRepeatZLow.SetWindowText(strTemp);
				
				if( pTexPanel->m_bScalRepeat )
				{
					pTexPanel->m_ScalEndX.EnableWindow( TRUE );
					pTexPanel->m_ScalEndY.EnableWindow( TRUE );
					pTexPanel->m_ScalEndZ.EnableWindow( TRUE );
					pTexPanel->m_ScalRepeatXHigh.EnableWindow( TRUE );
					pTexPanel->m_ScalRepeatYHigh.EnableWindow( TRUE );
					pTexPanel->m_ScalRepeatZHigh.EnableWindow( TRUE );
					pTexPanel->m_ScalRepeatXLow.EnableWindow( TRUE );
					pTexPanel->m_ScalRepeatYLow.EnableWindow( TRUE );
					pTexPanel->m_ScalRepeatZLow.EnableWindow( TRUE );

					pTexPanel->m_PScaleSpeedX.EnableWindow(FALSE);
					pTexPanel->m_PScaleSpeedY.EnableWindow(FALSE);
					pTexPanel->m_PScaleSpeedZ.EnableWindow(FALSE);
				}
				else
				{
					pTexPanel->m_ScalEndX.EnableWindow( FALSE );
					pTexPanel->m_ScalEndY.EnableWindow( FALSE );
					pTexPanel->m_ScalEndZ.EnableWindow( FALSE );
					pTexPanel->m_ScalRepeatXHigh.EnableWindow( FALSE );
					pTexPanel->m_ScalRepeatYHigh.EnableWindow( FALSE );
					pTexPanel->m_ScalRepeatZHigh.EnableWindow( FALSE );
					pTexPanel->m_ScalRepeatXLow.EnableWindow( FALSE );
					pTexPanel->m_ScalRepeatYLow.EnableWindow( FALSE );
					pTexPanel->m_ScalRepeatZLow.EnableWindow( FALSE );
					
					pTexPanel->m_PScaleSpeedX.EnableWindow(TRUE);
					pTexPanel->m_PScaleSpeedY.EnableWindow(TRUE);
					pTexPanel->m_PScaleSpeedZ.EnableWindow(TRUE);
				}
			}
			else
			if(m_pSfxPart->m_nType==SFXPARTTYPE_CUSTOMMESH) {
				pTexPanel->EnableParticle(TRUE);
				CSfxPartCustomMesh* pPartCustomMesh=(CSfxPartCustomMesh*)m_pSfxPart;
				CString strTemp;
				strTemp.Format("%d",pPartCustomMesh->m_nPoints);
				pTexPanel->m_PCreate.SetWindowText(strTemp);
			}
			else
			{
				pTexPanel->EnableParticle(FALSE);
				pTexPanel->EnableParticleEx(FALSE);
			}
		}
	}
	else 
	if(m_SelectedKeys.GetSize() > 1 ) 
	{
		CString strTemp = "0";
		pTexPanel->EnableControl(TRUE);
		pTexPanel->m_X.SetWindowText(strTemp);
		pTexPanel->m_Y.SetWindowText(strTemp);
		pTexPanel->m_Z.SetWindowText(strTemp);
		pTexPanel->m_PosRotX.SetWindowText(strTemp);
		pTexPanel->m_PosRotY.SetWindowText(strTemp);
		pTexPanel->m_PosRotZ.SetWindowText(strTemp);
		pTexPanel->m_ScaleX.SetWindowText(strTemp);
		pTexPanel->m_ScaleY.SetWindowText(strTemp);
		pTexPanel->m_ScaleZ.SetWindowText(strTemp);
		pTexPanel->m_AngleX.SetWindowText(strTemp);
		pTexPanel->m_AngleY.SetWindowText(strTemp);
		pTexPanel->m_AngleZ.SetWindowText(strTemp);
		pTexPanel->m_Alpha.SetWindowText(strTemp);

		pTexPanel->EnableRadio_All( FALSE );
	}	
	else
	{
		pTexPanel->EnableControl(FALSE);
		pTexPanel->EnableParticle(FALSE);
		pTexPanel->EnableParticleEx(FALSE);
	}
	
	
#else
	if(m_SelectedKeys.GetSize()==1) 
	{
		SfxKeyFrame* pKey=(SfxKeyFrame*)(m_SelectedKeys[0]);
		if( pKey == NULL ) return;
		pTexPanel->EnableControl(TRUE);
		CString strTemp;
		strTemp.Format("%g",pKey->vPos.x);
		pTexPanel->m_X.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPos.y);
		pTexPanel->m_Y.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPos.z);
		pTexPanel->m_Z.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPosRotate.x);
		pTexPanel->m_PosRotX.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPosRotate.y);
		pTexPanel->m_PosRotY.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vPosRotate.z);
		pTexPanel->m_PosRotZ.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vScale.x);
		pTexPanel->m_ScaleX.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vScale.y);
		pTexPanel->m_ScaleY.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vScale.z);
		pTexPanel->m_ScaleZ.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vRotate.x);
		pTexPanel->m_AngleX.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vRotate.y);
		pTexPanel->m_AngleY.SetWindowText(strTemp);
		strTemp.Format("%g",pKey->vRotate.z);
		pTexPanel->m_AngleZ.SetWindowText(strTemp);
		strTemp.Format("%d",pKey->nAlpha);
		pTexPanel->m_Alpha.SetWindowText(strTemp);
		CSfxBase* pSfxBase = g_SfxMng.GetSfxBase( 0 );
		CSfxPart* pSfxPart = pSfxBase->Part( m_nCurPart );
		pTexPanel->EnableRadio( pSfxPart->m_nBillType );
		pTexPanel->EnableAlphaRadio( pSfxPart->m_nAlphaType );
	}
	else {
		pTexPanel->EnableControl(FALSE);
	}
	if(m_pSfxPart) {
		CString strTemp;
		strTemp.Format("%d",m_pSfxPart->m_nTexFrame);
		pTexPanel->m_TexFrame.SetWindowText(strTemp);
		strTemp.Format("%d",m_pSfxPart->m_nTexLoop);
		pTexPanel->m_TexLoop.SetWindowText(strTemp);
		if(m_pSfxPart->m_nType==SFXPARTTYPE_PARTICLE) {
			CSfxPartParticle* pPartParticle=(CSfxPartParticle*)m_pSfxPart;
			CString strTemp;
			strTemp.Format("%g",pPartParticle->m_vParticleAccel.x);
			pTexPanel->m_PAccelX.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_vParticleAccel.y);
			pTexPanel->m_PAccelY.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_vParticleAccel.z);
			pTexPanel->m_PAccelZ.SetWindowText(strTemp);

			strTemp.Format("%d",pPartParticle->m_nParticleFrameAppear);
			pTexPanel->m_PAppear.SetWindowText(strTemp);
			strTemp.Format("%d",pPartParticle->m_nParticleCreate);
			pTexPanel->m_PCreate.SetWindowText(strTemp);
			strTemp.Format("%d",pPartParticle->m_nParticleCreateNum);
			pTexPanel->m_PCreateNum.SetWindowText(strTemp);
			strTemp.Format("%d",pPartParticle->m_nParticleFrameDisappear);
			pTexPanel->m_PDisappear.SetWindowText(strTemp);
			strTemp.Format("%d",pPartParticle->m_nParticleFrameKeep);
			pTexPanel->m_PKeep.SetWindowText(strTemp);

			strTemp.Format("%g",pPartParticle->m_fParticleStartPosVar);
			pTexPanel->m_PPosVar.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_fParticleStartPosVarY);
			pTexPanel->m_PPosVarY.SetWindowText(strTemp);

			strTemp.Format("%g",pPartParticle->m_vScaleSpeed.x);
			pTexPanel->m_PScaleSpeedX.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_vScaleSpeed.y);
			pTexPanel->m_PScaleSpeedY.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_vScaleSpeed.z);
			pTexPanel->m_PScaleSpeedZ.SetWindowText(strTemp);

			strTemp.Format("%g",pPartParticle->m_vScale.x);
			pTexPanel->m_PScaleX.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_vScale.y);
			pTexPanel->m_PScaleY.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_vScale.z);
			pTexPanel->m_PScaleZ.SetWindowText(strTemp);

			strTemp.Format("%g",pPartParticle->m_fParticleXZHigh);
			pTexPanel->m_PXZHigh.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_fParticleXZLow);
			pTexPanel->m_PXZLow.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_fParticleYHigh);
			pTexPanel->m_PYHigh.SetWindowText(strTemp);
			strTemp.Format("%g",pPartParticle->m_fParticleYLow);
			pTexPanel->m_PYLow.SetWindowText(strTemp);
		}
		if(m_pSfxPart->m_nType==SFXPARTTYPE_CUSTOMMESH) {
			CSfxPartCustomMesh* pPartCustomMesh=(CSfxPartCustomMesh*)m_pSfxPart;
			CString strTemp;
			strTemp.Format("%d",pPartCustomMesh->m_nPoints);
			pTexPanel->m_PCreate.SetWindowText(strTemp);
		}
	}
#endif

	Invalidate( FALSE );
	((CSFXEditorView*)m_pView)->RenderScene();//->Invalidate(FALSE);
}
void CFramePanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	RECT rtClient;
	GetClientRect(&rtClient);
	m_bLBut = TRUE;
	m_ptLClicked=point;
	SetCapture();

	CRect rectTemp;
	rectTemp.SetRect(100,16,rtClient.right,rtClient.bottom);
	if(nFlags & MK_CONTROL) 
	{
		if(m_SelectedKeys.GetSize()==1) 
		{
			SfxKeyFrame* pKeyTemp=static_cast<SfxKeyFrame*>(m_SelectedKeys[0]);
			if(rectTemp.PtInRect(point)) 
			{
				int nLCFrame=m_nLeftFrame+(point.~x-100)/xgap[m_nGapLevel]; m_nCurPart=m_nLCPart=m_nTopObj+(point.y-16)/16;
				CSfxPart* pSfxPart=g_SfxMng.GetSfxBase(0)->Part(m_nLCPart);
				SfxKeyFrame* pKey;
				if(pSfxPart) 
				{
					pKey = pSfxPart->KeyByFrame(nLCFrame);
					if(!pKey) 
					{
						pSfxPart->AddKeyFrame(nLCFrame);
						pKey=pSfxPart->KeyByFrame(nLCFrame);
						*(pKey)=*(pKeyTemp);
						pKey->nFrame=nLCFrame;
						m_pSfxPart=pSfxPart;
						m_nCurFrame=m_nLCFrame=nLCFrame;
						m_SelectedKeys[0]=pKey;
					}
				}
				UpdatePanel();
			}
		}
	}
	else 
	{
		if(rectTemp.PtInRect(point)) 
		{

			m_nCurFrame=m_nLCFrame=m_nLeftFrame+(point.x-100)/xgap[m_nGapLevel]; m_nCurPart=m_nLCPart=m_nTopObj+(point.y-16)/16;

			if( m_pView->m_pModel )
				m_pView->m_pModel->m_fFrameCurrent = (float)( ( m_nCurFrame < m_pView->m_pModel->GetMaxFrame() ) ? m_nCurFrame:m_pView->m_pModel->GetMaxFrame()-1 );
			
			for(int i=0;i<g_SfxMng.GetSfxBase(0)->m_apParts.GetSize();i++) 
			{
				if( g_SfxMng.GetSfxBase(0)->Part(i)->m_nType != SFXPARTTYPE_MESH )
						continue;
					
				CModelObject* pMesh = NULL;
					
				pMesh = g_SfxMeshMng.Mesh( g_SfxMng.GetSfxBase(0)->Part(i)->m_strTex );
					
				if( pMesh )
					pMesh->m_fFrameCurrent = (float)( ( m_nCurFrame < pMesh->GetMaxFrame() ) ? m_nCurFrame:pMesh->GetMaxFrame()-1 );
			}

			CSfxPart* pSfxPart=g_SfxMng.GetSfxBase(0)->Part(m_nLCPart);
			m_pSfxPart=pSfxPart;
			if(m_SelectedKeys.GetSize()>0 && pSfxPart) 
			{
				for(int i=0;i<m_SelectedKeys.GetSize();i++) 
				{
					if(pSfxPart->KeyByFrame(m_nLCFrame)==m_SelectedKeys[i]) 
					{
						m_pClickedKey=pSfxPart->KeyByFrame(m_nLCFrame);
						m_bKeyLocked=TRUE;
						break;
					}
				}
				if(!m_bKeyLocked) 
				{
					m_SelectedKeys.RemoveAll();
					m_ptSelectStart.x=m_nLCFrame; m_ptSelectStart.y=m_nLCPart;
					m_ptSelectEnd.x=m_nLCFrame; m_ptSelectEnd.y=m_nLCPart;
					m_bSelecting=TRUE;
				}
			}
			else 
			{
				m_SelectedKeys.RemoveAll();
				m_ptSelectStart.x=m_nLCFrame; m_ptSelectStart.y=m_nLCPart;
				m_ptSelectEnd.x=m_nLCFrame; m_ptSelectEnd.y=m_nLCPart;
				m_bSelecting=TRUE;
			}
			UpdatePanel();
		}
		else 
		{
			m_nLCFrame=m_nLCPart=-1;
			rectTemp.SetRect(0,16,100,rtClient.bottom);
			if(rectTemp.PtInRect(point)) 
			{
				m_nCurPart=m_nLCPart=m_nTopObj+(point.y-16)/16;
				m_ptSelectStart.x=m_nLCFrame; m_ptSelectStart.y=m_nLCPart;
			}
		}
	}

#ifdef __AALLSELECT
	CRect rect(0, 16, 100, rtClient.bottom );

	CSfxBase* pSfxBase = g_SfxMng.GetSfxBase( 0 );
	m_nCurFrame = m_nLCFrame = m_nLeftFrame + (point.x-100) / xgap[m_nGapLevel]; 
	m_nCurPart = m_nLCPart = m_nTopObj + (point.y-16) / 16;
	if( m_nCurPart < 0 )
		m_nCurPart = 0;
	if( m_nCurPart >= pSfxBase->m_apParts.GetSize() )
		m_nCurPart = pSfxBase->m_apParts.GetSize() - 1;
	if( m_nCurFrame < 0 )
		m_nCurFrame = m_nLCFrame = 0;
	
	if(rect.PtInRect(point) &&  m_nSfxPartClipBoard != m_nCurPart ) 
	{
		m_SelectedKeys.RemoveAll();
		m_nSfxPartClipBoard  = -1;
		UpdatePanel();
	}		

	CRect rect2(100, 0, rtClient.right, rtClient.bottom );
	if(rect2.PtInRect(point)) 
	{
		//m_SelectedKeys.RemoveAll();
		m_nSfxPartClipBoard  = -1;
		//UpdatePanel();
	}		
#endif
	
	CDialogBar::OnLButtonDown(nFlags, point);
}

#ifdef __AALLSELECT
void CFramePanel::ReplaceSfx(int nSrcIndex, int nDestIndex)
{
	CSfxPart* pNewPart;
	
	pNewPart  = (g_SfxMng.GetSfxBase(0)->Part(nSrcIndex));

	g_SfxMng.GetSfxBase(0)->m_apParts[nSrcIndex] = g_SfxMng.GetSfxBase(0)->m_apParts[nDestIndex];
	g_SfxMng.GetSfxBase(0)->m_apParts[nDestIndex] = pNewPart;
	
}
#endif

void CFramePanel::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLBut = FALSE;
	ReleaseCapture();

	RECT rtClient;
	GetClientRect(&rtClient);

	CRect rectTemp(0,0,100,16);
	if(rectTemp.PtInRect(m_ptLClicked) && rectTemp.PtInRect(point)) 
	{
/*
		CFileDialog dlgFile(TRUE);
		dlgFile.m_ofn.lpstrInitialDir=DIR_SFXTEX;
		dlgFile.m_ofn.lpstrTitle=_T("새 텍스쳐 추가");
		dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
		if(dlgFile.DoModal()==IDOK) {
			CString filename=dlgFile.GetFileName();
			filename.MakeLower();
			CSfxBase* pSfxBase=g_SfxMng.GetSfxBase(0);
			pSfxBase->AddPart(SFXPARTTYPE_BILL);
		}
*/
		CDialogAddPart dlgAddPart;
		if( dlgAddPart.DoModal() == IDOK ) 
		{
			CSfxBase* pSfxBase = g_SfxMng.GetSfxBase(0);
			CSfxPart* pSfxPart;
			pSfxPart = pSfxBase->AddPart( dlgAddPart.m_nType );

//#ifdef __ATEST
			pSfxPart->m_strName = dlgAddPart.m_StrName;

			Invalidate();
//#endif
		}
	}
	rectTemp.SetRect(100,16,rtClient.right,rtClient.bottom);
	if(rectTemp.PtInRect(point)) 
	{
		if(m_nLCFrame==m_nLeftFrame+(point.x-100)/xgap[m_nGapLevel] && m_nLCPart==m_nTopObj+(point.y-16)/16) 
		{
			m_nCurFrame=m_nLCFrame; m_nCurPart=m_nLCPart;
		}
	}

	CSfxBase* pSfxBase = g_SfxMng.GetSfxBase( 0 );
	if( m_nCurPart >= pSfxBase->m_apParts.GetSize() )
		m_nCurPart = pSfxBase->m_apParts.GetSize() - 1;
	if( m_nCurPart < 0 ) 
		m_nCurPart = 0;
	
#ifdef __AALLSELECT
	if( m_bLayerDragDrop )
	{
		int Pos = m_nTopObj + (point.y-16) / 16;
		if( Pos < 0 )
			Pos = 0;
		if( Pos >= pSfxBase->m_apParts.GetSize() )
			Pos = pSfxBase->m_apParts.GetSize() - 1;

		if( m_nSfxPartClipBoard != Pos )
		{
			ReplaceSfx(m_nSfxPartClipBoard, Pos );
				
			m_nSfxPartClipBoard = Pos;
			m_SelectedKeys.RemoveAll();
		}

		m_bLayerDragDrop = FALSE;

		UpdatePanel();
	}
#endif // __AALLSELECT
	if( m_bSelecting ) 
	{
		m_SelectedKeys.RemoveAll();
		if(m_ptSelectStart.x>m_ptSelectEnd.x) { int temp=m_ptSelectStart.x; m_ptSelectStart.x=m_ptSelectEnd.x; m_ptSelectEnd.x=temp; }
		if(m_ptSelectStart.y>m_ptSelectEnd.y) { int temp=m_ptSelectStart.y; m_ptSelectStart.y=m_ptSelectEnd.y; m_ptSelectEnd.y=temp; }

		CSfxPart* pSfxPart1=g_SfxMng.GetSfxBase(0)->Part(0);
		CSfxPart* pSfxPart2=g_SfxMng.GetSfxBase(0)->Part(1);

		if( pSfxPart1 && pSfxPart2 )
		{
			SfxKeyFrame* pKey1=pSfxPart1->KeyByFrame(0);
			SfxKeyFrame* pKey2=pSfxPart2->KeyByFrame(0);
		}
		
		for(int i=m_ptSelectStart.y;i<m_ptSelectEnd.y+1;i++) 
		{
			CSfxPart* pSfxPart=g_SfxMng.GetSfxBase(0)->Part(i);
			if(pSfxPart) 
			{
				for(int j=m_ptSelectStart.x;j<m_ptSelectEnd.x+1;j++) 
				{
					SfxKeyFrame* pKey=pSfxPart->KeyByFrame(j);
					if(pKey) 
					{
						m_SelectedKeys.Add(pKey);
					}
				}
			}
		}
		UpdatePanel();
	}
	m_bSelecting=m_bKeyLocked=FALSE;

	CDialogBar::OnLButtonUp(nFlags, point);
}

void CFramePanel::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
#ifdef __AALLSELECT
	RECT rtClient;
	GetClientRect(&rtClient);
	CRect rect(0, 16, 100, rtClient.bottom );
	
	if(rect.PtInRect(m_ptLClicked) && rect.PtInRect(point)) 
	{
		CSfxBase* pSfxBase = g_SfxMng.GetSfxBase( 0 );
		CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( m_nCurPart );
		
		if( pSfxPart ) 
		{
//			m_bLayerSelect     = TRUE;
			m_nSfxPartClipBoard = m_nCurPart;
			
			m_SelectedKeys.RemoveAll();

			m_pSfxPart = pSfxPart;
			
			for(int i=0;i<pSfxPart->m_apKeyFrames.GetSize();i++) 
			{
				m_SelectedKeys.Add( pSfxPart->Key(i) );
			}
			
			UpdatePanel();
		}

		return;
	}
	else
		m_nSfxPartClipBoard = -1;
//		m_bLayerSelect     = FALSE;
	
#endif
	// TODO: Add your message handler code here and/or call default
	CSfxBase* pSfxBase = g_SfxMng.GetSfxBase( 0 );
	CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( m_nCurPart );
	if( pSfxPart ) 
	{
		m_nCurFrame = m_nLCFrame = m_nLeftFrame + (point.x-100) / xgap[m_nGapLevel]; 
		m_nCurPart = m_nLCPart = m_nTopObj + (point.y-16) / 16;
		if( m_nCurPart < 0 )
			m_nCurPart = 0;
		if( m_nCurPart >= pSfxBase->m_apParts.GetSize() )
			m_nCurPart = pSfxBase->m_apParts.GetSize() - 1;
		if( m_nCurFrame < 0 )
			m_nCurFrame = m_nLCFrame = 0;
		pSfxPart->AddKeyFrame( m_nCurFrame );
		m_SelectedKeys.RemoveAll();
		m_SelectedKeys.Add( pSfxPart->KeyByFrame( m_nLCFrame ) );
		UpdatePanel();
	}
	CDialogBar::OnLButtonDblClk(nFlags, point);
}

void CFramePanel::OnMButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bMBut = TRUE;
	m_ptClicked.x=(point.x-100)/xgap[m_nGapLevel]; m_ptClicked.y=(point.y-16)/16;
	m_ptClickedScrollState.x=m_nLeftFrame; m_ptClickedScrollState.y=m_nTopObj;
	SetCapture();
	Invalidate( FALSE );
	CDialogBar::OnMButtonDown(nFlags, point);
}

void CFramePanel::OnMButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bMBut = FALSE;
	ReleaseCapture();

	CDialogBar::OnMButtonUp(nFlags, point);
}

void CFramePanel::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

#ifdef __AALLSELECT
	if( nFlags == MK_LBUTTON && m_nSfxPartClipBoard >= 0 )
		m_bLayerDragDrop = TRUE;
	else
		m_bLayerDragDrop = FALSE;
#endif // __AALLSELECT
	
	int tempCurX=m_ptCurrent.x;
	int tempCurY=m_ptCurrent.y;
	m_ptCurrent.x=(point.x-100)/xgap[m_nGapLevel]; m_ptCurrent.y=(point.y-16)/16; 

	CRect	tooltiprect;	
	GetClientRect( &tooltiprect );
	tooltiprect.top   = 16;
	tooltiprect.right = 100;

	if(tooltiprect.PtInRect(point)) 
	{
		CSfxPart* pSfxPart;
		pSfxPart  = g_SfxMng.GetSfxBase(0)->Part((BYTE)( m_nTopObj+m_ptCurrent.y ));

		CString str = "Texture : ";

		if(pSfxPart)
		{
			if( pSfxPart->m_strTex.IsEmpty() )
				str += "없음";
			else
				str += pSfxPart->m_strTex;
			
		}
		else
		{
			str += "없음";
		}
		m_cToolTip.UpdateTipText( str, this, 1 );
	}

	if( m_bMBut ) 
	{
		int tempx,tempy;
		tempx=m_ptClicked.x-m_ptCurrent.x;
		tempy=m_ptClicked.y-m_ptCurrent.y;
		tempx=m_ptClickedScrollState.x+tempx;
		tempy=m_ptClickedScrollState.y+tempy;
		if(tempx<0) tempx=0;
		if(tempy<0) tempy=0;
		m_nLeftFrame=tempx;
		m_nTopObj=tempy;
		Invalidate( FALSE );
	}
	if( m_bKeyLocked ) 
	{
		RECT rtClient;
		GetClientRect(&rtClient);
		m_ptLClicked=point;

		CRect rectTemp;
		rectTemp.SetRect(100,16,rtClient.right,rtClient.bottom);
		if(rectTemp.PtInRect(point)) 
		{
			m_nCurFrame=m_nLCFrame=m_nLeftFrame+(point.x-100)/xgap[m_nGapLevel];
			//m_nLCPart=m_nTopObj+(point.y-16)/16;
			if(m_SelectedKeys.GetSize()>0) 
			{
				int nDelta=m_nLCFrame-m_pClickedKey->nFrame;
				int i,j,k;
				BOOL bCheck=TRUE;
				for(i=0;i<m_SelectedKeys.GetSize();i++) {
					if(nDelta+((SfxKeyFrame*)(m_SelectedKeys[i]))->nFrame<0) bCheck=FALSE;
				}
				if(bCheck) {
					for(i=0;i<m_SelectedKeys.GetSize();i++) {
						((SfxKeyFrame*)(m_SelectedKeys[i]))->nFrame+=nDelta;
					}

					// 키프레임 소트
					for(k=0;k<g_SfxMng.GetSfxBase(0)->m_apParts.GetSize();k++) {
						CSfxPart* pSfxPart=g_SfxMng.GetSfxBase(0)->Part(k);
						for(i=0;i<pSfxPart->m_apKeyFrames.GetSize()-1;i++) {
							for(j=i+1;j<pSfxPart->m_apKeyFrames.GetSize();j++) {
								SfxKeyFrame* pKeyTemp;
								if(pSfxPart->Key(i)->nFrame > pSfxPart->Key(j)->nFrame) {
									pKeyTemp=pSfxPart->Key(i);
									pSfxPart->m_apKeyFrames[i]=pSfxPart->m_apKeyFrames[j];
									pSfxPart->m_apKeyFrames[j]=pKeyTemp;
								}
							}
						}
					}
				}
			}
			Invalidate( FALSE );
		}
	}
	else if(m_bLBut) {
		RECT rtClient;
		GetClientRect(&rtClient);
		m_ptLClicked=point;

		m_ptSelectStart.x; m_ptSelectStart.y;
		m_ptSelectEnd.x; m_ptSelectEnd.y;

		CRect rectTemp;
		rectTemp.SetRect(100,16,rtClient.right,rtClient.bottom);
		if(rectTemp.PtInRect(point)) {
			if(m_nLCFrame!=m_nLeftFrame+(point.x-100)/xgap[m_nGapLevel]) {
				m_nCurFrame=m_nLCFrame=m_nLeftFrame+(point.x-100)/xgap[m_nGapLevel]; m_nLCPart=m_nTopObj+(point.y-16)/16;
				m_ptSelectEnd.x=m_nLCFrame; m_ptSelectEnd.y=m_nLCPart;
				
				if( m_pView->m_pModel )
					m_pView->m_pModel->m_fFrameCurrent = (float)( ( m_nCurFrame < m_pView->m_pModel->GetMaxFrame() ) ? m_nCurFrame:m_pView->m_pModel->GetMaxFrame()-1 );

				for(int i=0;i<g_SfxMng.GetSfxBase(0)->m_apParts.GetSize();i++) 
				{
					if( g_SfxMng.GetSfxBase(0)->Part(i)->m_nType != SFXPARTTYPE_MESH )
						continue;
					
					CModelObject* pMesh = NULL;
					
					pMesh = g_SfxMeshMng.Mesh( g_SfxMng.GetSfxBase(0)->Part(i)->m_strTex );
					
					if( pMesh )
						pMesh->m_fFrameCurrent = (float)( ( m_nCurFrame < pMesh->GetMaxFrame() ) ? m_nCurFrame:pMesh->GetMaxFrame()-1 );
				}
				
				
				UpdatePanel();
				Invalidate(FALSE);
				m_pView->Invalidate(FALSE);
			}
		}
	}

	CDialogBar::OnMouseMove(nFlags, point);
}

void CFramePanel::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_bRBut = TRUE;
	SetCapture();

	RECT rtClient;
	GetClientRect(&rtClient);
	CRect rect(0, 16, 100, rtClient.bottom );
	
	if(rect.PtInRect(point) ) 
	{
		int pos;
		m_nSfxPartLayer = pos = m_nTopObj+(point.y-16)/16;
		CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( pos );

		if( pSfxPart )
		{
			CMenu popUp;
			CMenu* pMenu;
			
			RECT pp;
			GetWindowRect(&pp);

			popUp.LoadMenu( IDR_POPUPMENU );
			pMenu = popUp.GetSubMenu(0);
			pMenu->CheckMenuItem( ID_POPUPMENU, (pSfxPart->m_bUseing) ? MF_CHECKED: MF_UNCHECKED );

			pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				pp.left+point.x, pp.top+point.y, this );

		}
	}

	CRect rect2(100, 16, rtClient.right, rtClient.bottom );

	if(rect2.PtInRect(point) ) 
	{
		int pos;
		m_nSfxPartLayer = pos = m_nTopObj+(point.y-16)/16;
		
		if( m_SelectedKeys.GetSize() >= 1 )
		{
			CMenu popUp;
			CMenu* pMenu;
			
			RECT pp;
			GetWindowRect(&pp);
			
			popUp.LoadMenu( IDR_POPUPMENU2 );
			pMenu = popUp.GetSubMenu(0);
			
			pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				pp.left+point.x, pp.top+point.y, this );
		}
	}
	
	CDialogBar::OnRButtonDown(nFlags, point);
}

void CFramePanel::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bRBut = FALSE;
	ReleaseCapture();
	
	CDialogBar::OnRButtonUp(nFlags, point);
}

void CFramePanel::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialogBar::OnRButtonDblClk(nFlags, point);
}

BOOL CFramePanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialogBar::OnMouseWheel(nFlags, zDelta, pt);
}

int CFramePanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_cToolTip.Create(this);
	m_cToolTip.Activate(TRUE);
	RECT	rect;	
	GetClientRect( &rect );
	rect.top   = 16;
	rect.right = 100;
	m_cToolTip.AddTool( this, "", &rect, 1 );
	// TODO: Add your specialized creation code here

	return 0;
}

void CFramePanel::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	RECT	rect;	
	GetClientRect( &rect );
	int nWidth  = rect.right-rect.left;
	int nHeight = rect.bottom-rect.top;

	CPaintDC dc(this);
	m_hBitmap.DeleteObject();
	m_hBitmap.CreateCompatibleBitmap( &dc, nWidth, nHeight );

	Invalidate(FALSE);
}

void CFramePanel::OnPopupmenu() 
{
	// TODO: Add your command handler code here
	CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( m_nSfxPartLayer );
	
	if( pSfxPart )
		pSfxPart->m_bUseing = !pSfxPart->m_bUseing;

	Invalidate( FALSE );
}

void CFramePanel::OnUpdatePopupmenu(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( m_nSfxPartLayer );

	if( pSfxPart )
		(pSfxPart->m_bUseing) ? pCmdUI->SetCheck(1): pCmdUI->SetCheck(0);
}

void CFramePanel::OnValutchange() 
{
	CValueChange Dialog;

	Dialog.DoModal();
}


BOOL CFramePanel::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	{
		// Let the ToolTip process this message.
		m_cToolTip.RelayEvent(pMsg);
	}
	
	return CDialogBar::PreTranslateMessage(pMsg);
}

void CFramePanel::OnNamechange() 
{
	// TODO: Add your command handler code here
	CSfxPart* pSfxPart = g_SfxMng.GetSfxBase( 0 )->Part( m_nSfxPartLayer );
	
	if( pSfxPart )
	{
		CLayerDialog Dlg;

		if( Dlg.DoModal() == IDOK )
			pSfxPart->m_strName = Dlg.m_Name;
	}

	Invalidate();
}
