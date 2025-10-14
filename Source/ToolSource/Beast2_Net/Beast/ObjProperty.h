#if !defined(AFX_OBJPROPERTY_H__E589848E_4143_4353_9E7A_7E9906446760__INCLUDED_)
#define AFX_OBJPROPERTY_H__E589848E_4143_4353_9E7A_7E9906446760__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjProperty.h : header file
//

class CObj;
class CCtrl;
class CItem;
class CMover; 

/////////////////////////////////////////////////////////////////////////////
// CObjProperty

class CModelView;
class CWorldView;

class CObjProperty : public CPropertySheet
{
	DECLARE_DYNAMIC(CObjProperty)

// Construction
public:
	CObjProperty(CWorldView* pWorldView,UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CObjProperty(CWorldView* pWorldView,LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CButton m_buttonOk;
	CButton m_buttonCancel;
	CWorldView*	m_pWorldView;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjProperty)
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjProperty();

	// Generated message map functions
protected:
	//{{AFX_MSG(CObjProperty)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnOk();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPagePropObj dialog

class CPagePropObj : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropObj)

// Construction
public:
	CObj* m_pObj;
	CPagePropObj();
	~CPagePropObj();

	void Init( CObj* pObj );


// Dialog Data
	//{{AFX_DATA(CPagePropObj)
	enum { IDD = IDD_PROP_OBJ };
	BOOL	m_bLandIndependence;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropObj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropObj)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CString m_strOBJID;
	CString m_strModifyOBJID;
	CString m_strModifyOBJName;

	afx_msg void OnBnClickedFindObjid();
	afx_msg void OnBnClickedChangeObjid();
	afx_msg void OnBnClickedChangeallObjid();
};
/////////////////////////////////////////////////////////////////////////////
// CPagePropLight dialog

class CPagePropLight : public CPropertyPage
{
// Construction
public:
	CObj* m_pObj;
	CPagePropLight();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPagePropLight)
	enum { IDD = IDD_PROP_LIGHT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void Init( CObj* pObj );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPagePropLight)
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPagePropLight)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnInitMenu(CMenu* pMenu);
//	virtual BOOL OnInitDialog();
};
/////////////////////////////////////////////////////////////////////////////
// CPagePropRegion dialog

class CPagePropRegion : public CPropertyPage
{
// Construction
public:
	CPagePropRegion();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPagePropRegion)
	enum { IDD = IDD_PROP_REGION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPagePropRegion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPagePropRegion)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CPagePropTrigger dialog

class CPagePropTrigger : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropTrigger)

// Construction
public:
	CRegion* m_pRegion;
	CPagePropTrigger();
	~CPagePropTrigger();
	void Init( CRegion* pRegion );

// Dialog Data
	//{{AFX_DATA(CPagePropTrigger)
	enum { IDD = IDD_PROP_TRIGGER };
	CString	m_strFunction;
	CString	m_strMusic;
	CString	m_strSound;
	float	m_fTeleX;
	float	m_fTeleY;
	float	m_fTeleZ;
	BOOL	m_bBgm;
	CString	m_strTeleport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropTrigger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropTrigger)
	virtual BOOL OnInitDialog();
	afx_msg void OnPlaySound();
	afx_msg void OnPlayMusic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPagePropAttribute dialog

class CPagePropAttribute : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropAttribute)

	CCheckList m_checkList;
// Construction
public:
	CRegion* m_pRegion;
	CPagePropAttribute();
	~CPagePropAttribute();
	void Init( CRegion* pRegion );

// Dialog Data
	//{{AFX_DATA(CPagePropAttribute)
	enum { IDD = IDD_PROP_ATTRIBUTE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropAttribute)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropAttribute)
	virtual BOOL OnInitDialog();
	afx_msg void OnClear();
	afx_msg void OnSetAll();
	afx_msg void OnInvert();
	afx_msg void OnSelcancelList1();
	afx_msg void OnSetfocusList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPagePropRespawn dialog
class CCommonCtrl;
class CPagePropRespawn : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropRespawn)

// Construction
public:
	CItem* m_pItem;
	CMover* m_pMover;
	CCommonCtrl* m_pCtrl;

	CPagePropRespawn();
	~CPagePropRespawn();

	void Init( CObj* pObj );


// Dialog Data
	//{{AFX_DATA(CPagePropRespawn)
	enum { IDD = IDD_PROP_RESPAWN };
	int		m_nDelay;
	BOOL	m_bRespawnEnable;
	int		m_nSum;
	int		m_nActiveAttackNum;
	int		m_nDayMax;
	int		m_nDayMin;
	int		m_nHourMax;
	int		m_nHourMin;
	int		m_nItemMax;
	int		m_nItemMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropRespawn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropRespawn)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPagePropMover dialog

class CPagePropMover : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropMover)

// Construction
public:
	CWorldView* m_pWorldView;
	CMover* m_pMover;

	CPagePropMover();
	~CPagePropMover();

	void Init( CObj* pObj );

// Dialog Data
	//{{AFX_DATA(CPagePropMover)
	enum { IDD = IDD_PROP_MOVER };
	CButton	m_Cycle;
	CString	m_strCharacter;
	CString	m_strName;
	BOOL	m_bImmortality;
	BOOL	m_bSuperPower;
	CString	m_strDialog;
	CString	m_strBelli;
	CString	m_strAI;
	CString	m_strState;
	CString	m_PathIndex;
	CString m_strPropMoverID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropMover)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropMover)
	virtual BOOL OnInitDialog();
	afx_msg void OnDialogBrowser();
	afx_msg void OnSelchangeCharacter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};
/////////////////////////////////////////////////////////////////////////////
// CPagePropCaption dialog

class CPagePropCaption : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropCaption)

// Construction
public:
	CPagePropCaption();
	~CPagePropCaption();

	void Init( CRegion* pRegion );

	CRegion* m_pRegion;
// Dialog Data
	//{{AFX_DATA(CPagePropCaption)
	enum { IDD = IDD_PROP_CAPTION };
	CString	m_strDescription;
	CString	m_strSubTitle;
	CString	m_strTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropCaption)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropCaption)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPagePropPos dialog

class CPagePropPos : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropPos)

// Construction
public:
	CPagePropPos();
	~CPagePropPos();
	CRegion* m_pRegion;
	
	void Init( CRegion* pRegion );
	
// Dialog Data
	//{{AFX_DATA(CPagePropPos)
	enum { IDD = IDD_PROP_POS };
	CString	m_strKey;
	BOOL	m_bTargetKey;
	BOOL	m_bChaoKey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropPos)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropPos)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPagePropStructure dialog

class CPagePropStructure : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropStructure)

// Construction
public:
	CPagePropStructure();
	~CPagePropStructure();

	CRegion* m_pRegion;
	void Init( CRegion* pRegion );
	
// Dialog Data
	//{{AFX_DATA(CPagePropStructure)
	enum { IDD = IDD_PROP_STRUCTURE };
	BOOL	m_bUnit;
	CString	m_strStructure;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropStructure)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropStructure)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CPagePropControl dialog

class CPagePropControl : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropControl)

// Construction
public:
	CPagePropControl();
	~CPagePropControl();

	CCommonCtrl* m_pCommonCtrl;
	void Init( CCommonCtrl* pCommonCtrl );

// Dialog Data
	//{{AFX_DATA(CPagePropControl)
	enum { IDD = IDD_PROP_CTRL };
	CString	m_strCondGender;
	CString	m_strCondItem;
	CString	m_strCondItemCount;
	CString	m_strCondJob;
	CString m_strContQuest1;
	CString m_strContQuest2;
	CString m_strContQuest3;
	CString	m_strItemKind1;
	CString	m_strItemKind2;
	CString	m_strItemKind3;
	CString	m_strMonsterKind1;
	CString	m_strMonsterKind2;
	CString	m_strMonsterKind3;
	CString	m_strTrapKind1;
	CString	m_strTrapKind2;
	CString	m_strTrapKind3;
	BOOL	m_bGender;
	BOOL	m_bItem;
	BOOL	m_bLevel;
	BOOL	m_bQuest;
	BOOL	m_bQuestEnd;
	BOOL	m_bTeleport;
	int		m_nItemMax;
	int		m_nItemRare1;
	int		m_nItemRare2;
	int		m_nItemRare3;
	int		m_nMonActAttack1;
	int		m_nMonActAttack2;
	int		m_nMonActAttack3;
	int		m_nMonNum1;
	int		m_nMonNum2;
	int		m_nMonNum3;
	int		m_nCondLevel;
	int		m_nCondQuestState1;
	int		m_nCondQuestState2;
	int		m_nCondQuestState3;
	int		m_nItemMin;
	int		m_nTrapType;
	int		m_nTrapRandomPercent;
	int		m_nEndurance;
	int		m_nTrapLevel1;
	int		m_nTrapLevel2;
	int		m_nTrapLevel3;
	int		m_nTrapDelay;
	int		m_nCondTeleWorldId;
	int		m_nCondTeleX;
	int		m_nCondTeleY;
	int		m_nCondTeleZ;
	CString	m_strCtrlKey;
	CString	m_strLinkKey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropControl)
	public:
	virtual void OnOK();
	afx_msg void OnCheckItem();
	afx_msg void OnCheckQuest();
	afx_msg void OnCheckQuestEnd();
	afx_msg void OnCheckLevel();
	afx_msg void OnCheckGender();
	afx_msg void OnCheckTeleport();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropControl)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPagePropAreaSet dialog
#ifdef __N_TRIGGER_AREASET_050203
class CPagePropAreaSet : public CPropertyPage
{
	DECLARE_DYNCREATE(CPagePropAreaSet)

// Construction
public:
	CPagePropAreaSet();
	~CPagePropAreaSet();

	CRegion * m_pRegion;
	void Init(CRegion* pRegion);

// Dialog Data
	//{{AFX_DATA(CPagePropAreaSet)
	enum { IDD = IDD_PROP_AREASETTING };
	BOOL	m_bCheckItem;
	BOOL	m_bCheckLevel;
	BOOL	m_bCheckQuest;
	BOOL	m_bCheckJob;
	BOOL	m_bCheckGender;

	BOOL	m_bCheckParty;
	BOOL	m_bCheckGuild;

	u_long	m_uItemId;
	UINT	m_uiItemCount;
	UINT	m_uiMinLevel;
	UINT	m_uiMaxLevel;
	int		m_iQuestFlag;
	int		m_iJob;
	int		m_iGender;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPagePropAreaSet)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPagePropAreaSet)
	afx_msg void OnChecklevel();
	afx_msg void OnCheckjob();
	afx_msg void OnCheckquest();
	afx_msg void OnCheckgender();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditmin();
	afx_msg void OnChangeEditmax();
	afx_msg void OnCheckitem();
	afx_msg void OnChangeItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif//__N_TRIGGER_AREASET_050203

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJPROPERTY_H__E589848E_4143_4353_9E7A_7E9906446760__INCLUDED_)
