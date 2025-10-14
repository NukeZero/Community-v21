#if !defined(AFX_IMECTRL_H__DECA6B06_644E_11D2_A0AB_008651C10000__INCLUDED_)
#define AFX_IMECTRL_H__DECA6B06_644E_11D2_A0AB_008651C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ImeView.h : header file
//
#include <imm.h>

#define WM_CLOSECANDIDATE ( WM_USER + 1 )

#if !defined(_MULTI_LANGUAGE_)
#define IME_IN_COMPOSITION      1
#define IME_IN_CHOSECAND        2

#define MAX_LISTCAND 32
#define MAX_CHARS_PER_LINE		10
#define MAX_COMP_STRING_LEN		10
#define MAX_CHAR_100			100
#define DEFAULT_CAND_NUM_PER_PAGE 5

#define CANDWND_CLASS	"HanjaCandidate"

/* 기존 Edit Control의 Notify
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501
#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602
*/
#define EN_RETURN           0x0603 // WndEdit에서만 사용하는 추가된 Notify

class CWndCandList : public CWndListBox //CWndNeuz 
{ 
public: 
	BOOL m_bReadyCandList;
	int m_nStartIndex;
	int m_nScrollPos;
	LPCANDIDATELIST m_lpCandList;
	CWndBase* m_pWndEdit;

	CWndCandList(); 
	~CWndCandList(); 
	void SetCandList( LPCANDIDATELIST lpCandList );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
}; 
class CWndEdit : public CWndText
{
	CPoint m_ptConverted;
	BOOL m_bNativeMode;
	CEditString m_stringBack;

	CWndCandList* m_pWndCandList;// m_wndCandList;
	
	void OnChar_(UINT nChar);
	CWndEdit*  m_pOldWndEditCtrl;
static CWndEdit* m_pActWndEditCtrl;

//	CHAR m_TextComp[ 3 ];
public:
static HCURSOR m_hEditCursor;
static HKL m_hKeyLayout;

	CWndEdit();           // protected constructor used by dynamic creation

	BOOL Create(HWND hwnd,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);
	
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);

static CWndEdit* GetWndEditCtrl() { return m_pActWndEditCtrl; }

//	void SetMaxCharLen(int nLen) { m_nMaxCharLen = nLen; }

virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
virtual	void PaintFrame(C2DRender* p2DRender);
virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );

	void OnLButtonDown( UINT nFlags, CPoint point );
	void Empty( void );
	LPCTSTR GetString() { return m_string; }
	void SetString( LPCTSTR str, DWORD dwColor = 0xff000000 );
	void PutString( LPSTR str );
	void AddString( LPCTSTR pszString, DWORD dwColor = 0xff000000 );
	void ResetString();
	DWORD GetOffset() { return m_dwOffset; }

	BOOL IsYouMessage(UINT msg,WPARAM wparam,LPARAM lparam);
	void SetAlphaNumericMode();
	void SetNativeMode();
	void EnableModeChange( BOOL bEnable );
	void DisableModeChange();
	void DeleteBlock();
	BOOL IsAlphaNumericMode() { return m_bNativeMode == FALSE; }
	BOOL IsNativeMode() { return m_bNativeMode; }
	BOOL IsEnableModeChange() { return m_bEnableModeChange; }

static void SetAlphaNumeric();
static void SetNative();

// Implementation

	virtual ~CWndEdit();
	void	Init(HWND hwnd,int x, int y);//, HDC dc);
	
	inline BOOL Enter() {
		if(m_hWnd != NULL)
			return (BOOL)(m_hIMC = ImmGetContext(m_hWnd));
		else return FALSE;
	};
	inline void Leave() {
		ImmReleaseContext(m_hWnd, m_hIMC);
	};
	inline BOOL Check() {
		return (!(m_property & IME_PROP_SPECIAL_UI) && (m_property & IME_PROP_AT_CARET));
	}
	BOOL IsProcessEvent();
	void SetState(BOOL fOpen);
	void ClearData();


	// for Message 
	LONG OnInputLangChange(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMESetContext(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEComposition(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMENotify(WPARAM dwCommand, LPARAM dwData);
	LONG OnIMEControl(WPARAM dwCommand, LPARAM dwData);

	// for IMEComposition
	BOOL SetCompositionFont(LPLOGFONT lf);
	void SetCompWndPos(POINT& pt);
	int	 GetCompCursorPos();

	virtual void DrawCaret( C2DRender* p2DRender );
	virtual BOOL GetResultString();
	virtual BOOL GetCompString(LONG flag);
	virtual void ProcessResultString(LPSTR str);
	virtual void ProcessCompString(LPSTR str, LPSTR strAttr);
	virtual void IMEStartComposition(LONG dwData);
	virtual void IMEEndComposition(LONG dwData);
	
	void OnDestroyChildWnd( CWndBase* pWndChild );

	// for candidate window
	BOOL OpenCandidate(LONG lParam);	// IMN_OPENCANDIDATE
	BOOL CloseCandidate(LONG lParam);	// IMN_CLOSECANDIDATE
	BOOL ChangeCandidate(LONG lParam);	// IMN_CHANGECANDIDATE
	BOOL SetOpenStatus();				// IMN_SETOPENSTATUS

	// friend function
	// variables
	//HWND	m_hKeyWnd;
	int		m_nState;					// Current Ime state.
	int		m_nCompLen;					// To save previous composition string length.
	DWORD	m_property;
	HWND	m_hwndCand[MAX_LISTCAND];
	LPCANDIDATELIST m_CandList[MAX_LISTCAND];
	CANDIDATELIST m_CandateList;

	// transient
	BOOL  m_bEnableModeChange;

static HIMC	m_hIMCBackup;
static HIMC	m_hIMC;
static HWND	m_hWnd;

private:
	// Implementation
	void  InitIme(HWND hwnd,int x, int y);
};

class CWndPopupListBox : CWndListBox
{
public: 
	CWndPopupListBox();
	~CWndPopupListBox();

	void Create( const RECT& rect, CWndBase* pParentWnd, UINT nID );
	virtual void OnKillFocus( CWndBase* pNewWnd );
};

/////////////////////////////////////////////////////////////////////////////
#else //_MULTI_LANGUAGE_
#include <dimm.h>

#define IME_IN_COMPOSITION      1
#define IME_IN_CHOSECAND        2

#define MAX_CHARS_PER_LINE		10
#define MAX_COMP_STRING_LEN		10
#define MAX_CHAR_100			100
#define DEFAULT_CAND_NUM_PER_PAGE 5

#define CANDWND_CLASS	"HanjaCandidate"

/* 기존 Edit Control의 Notify
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501
#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602
*/
#define EN_RETURN           0x0603 // WndEdit에서만 사용하는 추가된 Notify
#define EN_UP				0x0604 // WndEdit에서만 사용하는 추가된 Notify

/////////////////////////////////////////////////////////////////////////////
class CIMEMgr
{
public:
	CIMEMgr();
	~CIMEMgr();

	void	InputLangChange(HWND hWnd, HKL hkl);
	void	Composition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void	Candidate(HWND hWnd);
	void	Reading(HWND hWnd);
	bool	Private(HWND hWnd, LPARAM dwData);	// IMN_PRIVATE

	void	CompleteComposition(HWND hWnd);
#ifdef __VBUGFIX_20050201
	void    CancelComposition(HWND hWnd);
#endif
	HIMC	AssociateContext(HWND hWnd, HIMC hIMC);
	void	SetOpenStatus(HWND hWnd, BOOL openState);
	BOOL	GetOpenStatus(HWND hWnd);
	void	NotifyIME(HWND hWnd, DWORD dwAction, DWORD dwIndex, DWORD dwValue);

	int		GetResult(char* text, int len);
	int		GetCompositon(char* text, int len);
	int		GetAttribute(BYTE* attr, int len);
	int		GetCursorPos();
	
	int		GetCandidate(DWORD index, char* text, int len);
	bool	GetCandidateList(CANDIDATELIST* candidateList);

	int		GetReading(char* text, int len);

	int     GetResultSize() { return m_result.size(); }
	void	ClearResult() { m_result.clear(); }
	void	ClearComposition() { m_composition.clear(); }
	void	ClearCandidate() { m_candidate.clear(); }
	void	ClearReading() { m_reading.clear(); }



	WORD	GetCodePage(){ return m_codePage; }
	bool	IsVerticalReading(){ return m_bVerticalReading; }
	bool	IsVerticalCandidate(){ return m_bVerticalCandidate; }

	HKL			m_hkl;
	LANGID		m_langId;
	WORD		m_codePage;
	DWORD		m_property;

protected:
	int		ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len);

	void	CheckToggleState(HWND hWnd);
	void	GetImeId();
	void	SetupImeApi(HWND);
	bool	GetReadingWindowOrientation();

	//
	DWORD		m_dwId[2];

	vector<wchar_t>		m_result;
	vector<wchar_t>		m_composition;
	vector<BYTE>		m_attr;
	int					m_cursorPos;
	vector<BYTE>		m_candidate;
	vector<wchar_t>		m_reading;

	bool		m_bUnicodeIME;
	bool		m_bVerticalReading;
	bool		m_bVerticalCandidate;
	int			m_ImeState;
	wchar_t*	m_wszCurrIndicator;

	// Imm32.DLL Functions
	HMODULE			m_hDllImm32;

	INPUTCONTEXT*	(WINAPI * _ImmLockIMC)( HIMC );
	BOOL			(WINAPI * _ImmUnlockIMC)( HIMC );
	LPVOID			(WINAPI * _ImmLockIMCC)( HIMCC );
	BOOL			(WINAPI * _ImmUnlockIMCC)( HIMCC );

	// IME DLL Functions
	HMODULE			m_hDllIme;

	UINT			(WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
	BOOL			(WINAPI * _ShowReadingWindow)( HIMC, BOOL );
};
/////////////////////////////////////////////////////////////////////////////
class CWndCandList : public CWndListBox
{ 
public: 
	int m_nPageStart;
	int m_nScrollPos;
#ifdef __V050823_JAPAN_ATOK
	CPoint m_ptWindowPos;
#endif
	CWndCandList(); 
	~CWndCandList(); 
	void UpdateCandList(CPoint windowPos);

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
}; 
/////////////////////////////////////////////////////////////////////////////
class CWndHCandList : public CWndBase
{
public:
	void UpdateCandList(CPoint windowPos);

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw( C2DRender* p2DRender ); 

	int					m_maxWidth;
	vector<string>		m_candidate;
};
/////////////////////////////////////////////////////////////////////////////
class CReadingList : public CWndBase
{
public:
	void UpdateReading(CPoint windowPos);

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw( C2DRender* p2DRender ); 

	CRect	GetReadingSize();

	vector<char>	m_reading;
};
/////////////////////////////////////////////////////////////////////////////
class CWndEdit : public CWndText
{
public:
	CWndEdit();
	~CWndEdit();

	BOOL	Create(HWND hwnd,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID, int nFontColorR = 0, int nFontColorG = 0, int nFontColorB = 0 );

	// CWndBase overide
	virtual void	OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void	OnSetFocus(CWndBase* pOldWnd);
	virtual void	OnKillFocus(CWndBase* pNewWnd);

	virtual void	SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void	PaintFrame(C2DRender* p2DRender);
	virtual BOOL	OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );

	// CWndText overide
	virtual	void	DrawCaret( C2DRender* p2DRender );

	void		Empty( void );
	LPCTSTR		GetString() { return m_string; }
	DWORD		GetOffset() { return m_dwOffset; }
#ifdef __V050823_JAPAN_ATOK
	LRESULT		WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
#endif
	BOOL		IsYouMessage(UINT msg,WPARAM wparam,LPARAM lparam);
	void		SetAlphaNumericMode();
	void		SetNativeMode();
	void		EnableModeChange( BOOL bEnable );
	

	static void			SetNative();
	static void			SetAlphaNumeric();
	static CWndEdit*	GetWndEditCtrl() { return m_pActWndEditCtrl; }


	static HWND			m_hWnd;
	static HIMC			m_hIMCBackup;
	static HCURSOR		m_hEditCursor;
	static CWndEdit*	m_pActWndEditCtrl;

private:
	void	DeleteBlock();

	// for IME Control Message
	LONG	OnInputLangChange(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMESetContext(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMEComposition(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMENotify(WPARAM dwCommand, LPARAM dwData);
	LONG	OnIMEControl(WPARAM dwCommand, LPARAM dwData);
	void	OnChar_(UINT nChar);

	// for composition
	BOOL	GetResultString();
	BOOL	GetCompString(LONG flag);

	// for candidate
	BOOL	OpenCandidate();
	BOOL	ChangeCandidate();
	BOOL	CloseCandidate();
	BOOL	SetOpenStatus();

	// for reading
	void	ShowReading();
	void	HideReading();

	bool	KeyLeft();
	bool	KeyRight();
	void	ClearAll();
	void	OnKeyEnd();	

	CPoint				m_ptInputPos;
	BOOL				m_bImeNativeMode;
	BOOL				m_bEnableModeChange;
	BOOL				m_bKeepComposition;

	CEditString			m_stringBack;

	vector<BYTE>		m_CandidateBuffer;

	CWndCandList*		m_pWndCandList;
	CWndHCandList*		m_pWndHCandList;
	CReadingList*		m_pWndReadingList;

};
/////////////////////////////////////////////////////////////////////////////

extern CIMEMgr	g_imeMgr;

/////////////////////////////////////////////////////////////////////////////
#endif //_MULTI_LANGUAGE_

#endif // !defined(AFX_IMEVIEW_H__DECA6B06_644E_11D2_A0AB_008651C10000__INCLUDED_)