#if !defined(AFX_TERRAINEDIT_H__9DD71E37_3711_45D7_8759_A1517B23FCA5__INCLUDED_)
#define AFX_TERRAINEDIT_H__9DD71E37_3711_45D7_8759_A1517B23FCA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainEdit.h : header file
//

#define LANDTEX_NORMAL   0
#define LANDTEX_DETAIL   1
#define LANDTEX_BLANK    2
#define LANDTEX_LANDMARK 3 

#define DEFAULT_
/////////////////////////////////////////////////////////////////////////////
// CPageTerrainHeight dialog

class CPageTerrainHeight : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTerrainHeight)

// Construction
public:
static BOOL m_bCheckFixedHeight;
static int m_nRadiusHeight;
static int m_nHardnessHeight;
static int m_nHeightStyle;
static float m_fFixedHeight;
static int m_nHeightAttribute;

	CPageTerrainHeight();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageTerrainHeight)
	enum { IDD = IDD_TERRAIN_HEIGHT };
	int		m_iMPU;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageTerrainHeight)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageTerrainHeight)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureHeightRadius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHeightMountain();
	afx_msg void OnHeightFlat();
	afx_msg void OnHeightSoft();
	afx_msg void OnChangeFixedHeight();
	afx_msg void OnCheckFixedHeight();
	afx_msg void OnReleasedcaptureHeightHardness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeightRandom();
	afx_msg void OnHeightAttribute();
	afx_msg void OnRadioHeightAttribute();
	afx_msg void OnAttrNone();
	afx_msg void OnAttrNomove();
	afx_msg void OnAttrDie();
	afx_msg void OnAttrNofly();
	afx_msg void OnAttrNowalk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainColor dialog

class CPageTerrainColor : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTerrainColor)

// Construction
public:
static int m_nRadiusVColor;
static int m_nHardnessVColor;
static D3DCOLOR m_dwVertexColor;

	CPageTerrainColor();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageTerrainColor)
	enum { IDD = IDD_TERRAIN_COLOR };
	int		m_nColorRadius;
	int		m_nColorSliderBlue;
	int		m_nColorSliderGreen;
	int		m_nColorSliderRed;
	int		m_nColorSliderRadius;
	int		m_nColorSliderShade;
	//}}AFX_DATA
	int		m_nColorBlue;
	int		m_nColorGreen;
	int		m_nColorRed;
	int		m_nColorShade;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageTerrainColor)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageTerrainColor)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureColorSliderRadius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureColorSliderRed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureColorSliderGreen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureColorSliderBlue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureColorSliderShade(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeColorEditBlue();
	afx_msg void OnChangeColorEditGreen();
	afx_msg void OnChangeColorEditRed();
	afx_msg void OnVcolor();
	afx_msg void OnChangeColorEditShade();
	afx_msg void OnReleasedcaptureColorSliderHardness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CPageTerrainTexture dialog

class CPageTerrainTexture : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTerrainTexture)

// Construction
public:
static int m_nHardnessTexture;
static int m_nRadiusTexture;
#ifdef __CSC_TERRAIN_OPACITY
static int m_nOpacityTexture;
#endif //__CSC_TERRAIN_OPACITY
static BOOL m_nLandTexture;

	CPageTerrainTexture();
	~CPageTerrainTexture();

#ifdef __CSC_TERRAIN_OPACITY
	void ResetOpacityValue();
#endif //__CSC_TERRAIN_OPACITY
// Dialog Data
	//{{AFX_DATA(CPageTerrainTexture)
#ifdef __CSC_TERRAIN_OPACITY
	enum { IDD = IDD_TERRAIN_TEXTURE_EX };
#else
	enum { IDD = IDD_TERRAIN_TEXTURE };
#endif //__CSC_TERRAIN_OPACITY
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageTerrainTexture)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageTerrainTexture)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureTextureRadius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureTextureHardness(NMHDR* pNMHDR, LRESULT* pResult);
#ifdef __CSC_TERRAIN_OPACITY
	afx_msg void OnReleasedcaptureTextureOpacity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
#endif //__CSC_TERRAIN_OPACITY
	afx_msg void OnLandtexDetail();
	afx_msg void OnLandtexNormal();
	afx_msg void OnLandtexBlank();
	afx_msg void OnLandtexLandmark();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CTerrainSheet

class CTerrainSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTerrainSheet)

// Construction
public:
	CTerrainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTerrainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerrainSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTerrainSheet)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPageTerrainWater dialog

class CPageTerrainWater : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTerrainWater)

// Construction
public:
static int m_nWaterTexture;
static int m_nWaterHeight;
	
	CPageTerrainWater();
	~CPageTerrainWater();

#ifdef __N_WATERFRAME_050202
	void ComboBoxAddItem( const int& BoxIndex );
	void ComboBoxDeleteItem( const int& BoxIndex );
static int	m_nWaterFrame;		//	텍스쳐의 종류를 선택할 수 있게 0 : default , n (1 ~ ) : n번째 애니메이션 선택
	int	m_nWaterTexNum;

private:
	CTreeCtrl		m_Tree;
	HTREEITEM		m_hTreeMain;
public:
#endif//__N_WATERFRAME_050202

// Dialog Data
	//{{AFX_DATA(CPageTerrainWater)
	enum { IDD = IDD_TERRAIN_WATER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageTerrainWater)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageTerrainWater)
	afx_msg void OnWaterNone();
	afx_msg void OnWaterWater();
	afx_msg void OnWaterCloud();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeEdit1();
	afx_msg void OnSelchangeCombo1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPageTerrainLayer dialog

class CPageTerrainLayer : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTerrainLayer)
// Construction
public:
	CLandscape* m_pLandscape;
	CCheckList m_checkList;
	
	CPageTerrainLayer();   // standard constructor
	~CPageTerrainLayer();

	void UpdateLayer( D3DXVECTOR3& vPos, CWorld* pWorld );

// Dialog Data
	//{{AFX_DATA(CPageTerrainLayer)
	enum { IDD = IDD_TERRAIN_LAYER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageTerrainLayer)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageTerrainLayer)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	afx_msg void OnUpdate();
	afx_msg void OnAllVisible();
	afx_msg void OnOpenTexture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINEDIT_H__9DD71E37_3711_45D7_8759_A1517B23FCA5__INCLUDED_)
