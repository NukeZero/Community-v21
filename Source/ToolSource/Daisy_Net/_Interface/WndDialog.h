#ifndef __WNDDIALOG2__H
#define __WNDDIALOG2__H

class CWndAnswer : public CWndButton
{
public:
	LPVOID m_pWordButton;
};
class CWndDialog : public CWndNeuz 
{ 
#ifdef __V050322_CACHEQUEST
	CWndAnswer* m_apWndAnswer[ 6 ];
#endif
public: 
	//BOOL m_bLock;
	CTimer m_timer;
	//CTexture m_texDialog;
	CTexture m_texChar;
	BOOL m_bWordButtonEnable;
	int m_nWordButtonNum;
	int m_nKeyButtonNum;
	int m_nContextButtonNum;
	CUIntArray m_aContextMark[ 32 ];
	struct WORDBUTTON 
	{
		BOOL bStatus;
		CRect rect; 
		TCHAR szWord[ 64 ];
		TCHAR szKey[ 64 ];
		DWORD dwParam;
		DWORD dwParam2;
		int nLinkIndex; // 줄바꿈으로 단어가 끊어진 경우 연결하기 위한 인덱스 
	};
	WORDBUTTON m_aWordButton[ 32 ];
	WORDBUTTON m_aKeyButton[ 32 ];
	WORDBUTTON m_aContextButton[ 32 ];
	CEditString m_string;

	DWORD m_dwQuest;// context 버튼에서 사용함 
	BOOL m_bSay;
	//CTexture m_texFace;
	int m_nCurArray;
	//CStringArray m_strArray;
	CPtrArray m_strArray;
#ifdef __V050322_CACHEQUEST
	OBJID m_idMover;
#else
	CMover* m_pMover;
#endif // __V050322_CACHEQUEST
	CPtrArray m_scriptAry;  
	//CScriptDialog m_scriptDlg;
	//CPtrArray m_aKeyButton;   
	CMapStringToString m_mapWordToOriginal;
	 
	CWndDialog(); 
	~CWndDialog(); 

	void RemoveAllKeyButton();
	void RemoveKeyButton( LPCTSTR lpszKey );
	void AddAnswerButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest );
	void AddKeyButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest );
	void ParsingString( LPCTSTR lpszString );
	void Say( LPCTSTR lpszString, DWORD dwQuest );
	//void AddSay( LPCTSTR lpszString );
	void EndSay();
	void BeginText();
	void MakeKeyButton();
	void MakeContextButton();
	void MakeAnswerButton();
	//void ShowQuestion();
	//void ShowAnswer();
	void UpdateButtonEnable();
	BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	void RunScript( const char* szKey, DWORD dwParam, DWORD dwQuest );

#ifdef __V050322_CACHEQUEST
	virtual BOOL Process();
#endif // __V050322_CACHEQUEST
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	//virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point);
}; 
#endif
