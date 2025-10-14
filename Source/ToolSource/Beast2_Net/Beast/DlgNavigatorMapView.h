#pragma once


// CDlgNavigatorMapView 대화 상자입니다.

#define TexSIZEX	512
#define TexSIZEY	512

typedef struct tagNAVIMAPINFO {

		BYTE*				lpvBits;
		D3DXIMAGE_INFO		ImageInfo;
		BITMAPINFO			bmi;
		BOOL				bRender;
} NAVIMAPINFO, FAR *LPNAVIMAPINFO, *PNAVIMAPINFO;

class CDlgNavigatorMapView : public CDialog
{
	DECLARE_DYNAMIC(CDlgNavigatorMapView)

public:
	D3DXVECTOR3			m_vPos;
	CWorld*				m_pWorld;
	CWorldView*			m_pWorldView;
	NAVIMAPINFO*		m_TexInfo;
	CDC					m_memDC;
	CBitmap				m_bmpNavi;	

	void UpdatePos();
	void Update( CWorld* pWorld, CWorldView* pWorldView );
	void LoadNaviMapTexture();
	void CreateNaviTex();

	CDlgNavigatorMapView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNavigatorMapView();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NAVIGATOR_MAP };	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
};
