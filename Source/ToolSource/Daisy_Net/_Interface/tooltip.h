#ifndef _TOOLTIP_H_
#define _TOOLTIP_H_

enum 
{
	FIRST_TT,
//	SECOND_TT,
//	THIRD_TT,
	MAX_TT
};

class CToolTip
{
	BOOL        m_bEnable       ;
	BOOL        m_bPutToolTip   ;
	BOOL        m_bReadyToopTip ;
	DWORD       m_dwToolTipId   ;
	CEditString m_strToolTip    ;
	CTimer      m_timerToopTip  ;
	CPoint      m_ptToolTip     ;
	CPoint      m_ptOldToolTip  ;
	CRect       m_rect          ;
	CRect       m_rectRender    ;
	int         m_nPosition     ;
	BOOL        m_bToolTip      ;
	int         m_nAlpha        ;
#ifdef __TOOLTIPTEXTURE
	CTexture	m_apTextureToolTip[ MAX_TT * 9 ];
#endif // __TOOLTIPTEXTURE
public:
	CToolTip();
	~CToolTip();
	void SetPosition( int nPos ) { m_nPosition = nPos; }
	void SetEnable( BOOL bEnable ) { m_bEnable = bEnable; }
	BOOL GetEnable() { return m_bEnable; }
	void CancelToolTip();
	void PutToolTip( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTip( DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void PutToolTip( DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos = 0 );
	void Process( CPoint pt, C2DRender* p2DRender );
	void Paint( C2DRender* p2DRender );
#ifdef __TOOLTIPTEXTURE
	void Delete();
	void InitTexture();
#endif // __TOOLTIPTEXTURE
};
/*
#define TOOLTIP_COMBATMODE   1  // "전투/평화\n단축키[C]"
#define TOOLTIP_INVENTORY    2  // "소지품\n단축키[I]"
#define TOOLTIP_OPTION       3  // "종료\n단축키[ALT-X]"
#define TOOLTIP_STATUS       4  // "능력치보기\n단축키[S]"

#define TOOLTIP_EQUIPMENT    5  // "장비장착\n단축키[E]"
#define TOOLTIP_KNOWLEDGE    6  // "지식사용\n단축키[K]"
#define TOOLTIP_LOOK         7  // "자세히보기\n단축키[L]"

#define TOOLTIP_SLOT         8  // "아이템 슬롯"  
#define TOOLTIP_SLOT_F1      9  // "1번 슬롯 선택\n단축키[F1]"
#define TOOLTIP_SLOT_F2      10 // "2번 슬롯 선택\n단축키[F2]"
#define TOOLTIP_SLOT_F3      11 // "3번 슬롯 선택\n단축키[F3]"

#define TOOLTIP_CHAT         12 // "채팅 입력창\n단축키[ENTER]"
#define TOOLTIP_CHAT1        14 // "일반 채팅"
#define TOOLTIP_CHAT2        15 // "귓속말"
#define TOOLTIP_CHAT3        16 // 

#define TOOLTIP_MESSAGE      17 // "메시지 출력창"
*/
#endif