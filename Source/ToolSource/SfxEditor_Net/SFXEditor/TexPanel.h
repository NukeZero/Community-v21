#if !defined(AFX_TEXPANEL_H__E0868B00_CA3B_4CC5_8DDB_0A911459BEEC__INCLUDED_)
#define AFX_TEXPANEL_H__E0868B00_CA3B_4CC5_8DDB_0A911459BEEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTexPanel dialog

class CTexPanel : public CDialog
{
// Construction
public:
	void EnableRadio_All(BOOL bEnable);
	CTexPanel(CWnd* pParent = NULL);   // standard constructor

	CSFXEditorDoc* m_pDoc;
	CSFXEditorView* m_pView;

	void EnableRadio(BYTE nIndex);
	void EnableAlphaRadio(BYTE nIndex);
	void LoadTexture(LPCTSTR strTex);
	void EnableParticle(BOOL bEnable);
	void EnableParticleEx(BOOL bEnable);
		
// Dialog Data
	//{{AFX_DATA(CTexPanel)
	enum { IDD = IDD_1TEX };
	CEdit	m_ScalRepeatZLow;
	CEdit	m_ScalRepeatZHigh;
	CEdit	m_ScalRepeatYLow;
	CEdit	m_ScalRepeatYHigh;
	CEdit	m_ScalRepeatXHigh;
	CEdit	m_ScalRepeatXLow;
	CEdit	m_ScalEndZ;
	CEdit	m_ScalEndY;
	CEdit	m_ScalEndX;
	CEdit	m_PRotZLow;
	CEdit	m_PRotZHigh;
	CEdit	m_PRotYLow;
	CEdit	m_PRotYHigh;
	CEdit	m_PRotXLow;
	CEdit	m_PRotXHigh;
	CEdit	m_TexLoop;
	CEdit	m_TexFrame;
	CEdit	m_PYLow;
	CEdit	m_PYHigh;
	CEdit	m_PXZLow;
	CEdit	m_PXZHigh;
	CEdit	m_PScaleZ;
	CEdit	m_PScaleY;
	CEdit	m_PScaleX;
	CEdit	m_PScaleSpeedZ;
	CEdit	m_PScaleSpeedY;
	CEdit	m_PScaleSpeedX;
	CEdit	m_PPosVarY;
	CEdit	m_PPosVar;
	CEdit	m_PKeep;
	CEdit	m_PDisappear;
	CEdit	m_PCreateNum;
	CEdit	m_PCreate;
	CEdit	m_PAppear;
	CEdit	m_PAccelZ;
	CEdit	m_PAccelY;
	CEdit	m_PAccelX;
	CEdit	m_PosRotZ;
	CEdit	m_PosRotY;
	CEdit	m_PosRotX;
	CEdit	m_AngleZ;
	CEdit	m_AngleY;
	CEdit	m_AngleX;
	CEdit	m_ScaleZ;
	CEdit	m_ScaleY;
	CEdit	m_ScaleX;
	CEdit	m_Alpha;
	CEdit	m_Z;
	CEdit	m_Y;
	CEdit	m_X;
	BOOL	m_bScalRepeat;
	BOOL	m_bRepeat;
	//}}AFX_DATA

	void EnableControl(BOOL bEnable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTexPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeX();
	afx_msg void OnChangeY();
	afx_msg void OnChangeZ();
	afx_msg void OnChangeA();
	afx_msg void OnChangeSx();
	afx_msg void OnChangeSy();
	afx_msg void OnChangeAngX();
	afx_msg void OnRadiobill();
	afx_msg void OnRadiobottom();
	afx_msg void OnRadioblend();
	afx_msg void OnRadioglow();
	afx_msg void OnButtontex();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRadionormal();
	afx_msg void OnRadiopole();
	afx_msg void OnChangeSz();
	afx_msg void OnChangeAngY();
	afx_msg void OnChangeAngZ();
	afx_msg void OnChangeProtX();
	afx_msg void OnChangeProtY();
	afx_msg void OnChangeProtZ();
	afx_msg void OnButtontexnull();
	afx_msg void OnButtonloadsfx();
	afx_msg void OnButtonsavesfx();
	afx_msg void OnChangePaccelx();
	afx_msg void OnChangePaccely();
	afx_msg void OnChangePaccelz();
	afx_msg void OnChangePappear();
	afx_msg void OnChangePcreate();
	afx_msg void OnChangePcreatenum();
	afx_msg void OnChangePdisappear();
	afx_msg void OnChangePkeep();
	afx_msg void OnChangePposvar();
	afx_msg void OnChangePposvary();
	afx_msg void OnChangePscalespeedx();
	afx_msg void OnChangePscalespeedy();
	afx_msg void OnChangePscalespeedz();
	afx_msg void OnChangePscalex();
	afx_msg void OnChangePscaley();
	afx_msg void OnChangePscalez();
	afx_msg void OnChangePxzhigh();
	afx_msg void OnChangePxzlow();
	afx_msg void OnChangePyhigh();
	afx_msg void OnChangePylow();
	afx_msg void OnChangeTexframe();
	afx_msg void OnChangeTexloop();
	afx_msg void OnButtonqsavesfx();
	afx_msg void OnButtonqloadsfx();
	afx_msg void OnChangeParticleRotXLow();
	afx_msg void OnChangeParticleRotXHigh();
	afx_msg void OnChangeParticleRotYHigh();
	afx_msg void OnChangeParticleRotYLow();
	afx_msg void OnChangeParticleRotZHigh();
	afx_msg void OnChangeParticleRotZLow();
	afx_msg void OnScalRepeatCheck();
	afx_msg void OnRepeatCheck();
	afx_msg void OnButtonloadsfx2();
	afx_msg void OnChangeXscalrepeathigh();
	afx_msg void OnChangeXscalrepeatlow();
	afx_msg void OnChangeYscalrepeathigh();
	afx_msg void OnChangeYscalrepeatlow();
	afx_msg void OnChangeZscalrepeathigh();
	afx_msg void OnChangeZscalrepeatlow();
	afx_msg void OnChangePscaleendx();
	afx_msg void OnChangePscaleendy();
	afx_msg void OnChangePscaleendz();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXPANEL_H__E0868B00_CA3B_4CC5_8DDB_0A911459BEEC__INCLUDED_)
