

#pragma once


#include "WndFontColor.h"


//---------------------------------------------------------------------------------------------
// 윈도우 폰트 색상 메니져 클레스
//---------------------------------------------------------------------------------------------
class CWndFontColorManager 
{

public:

												CWndFontColorManager();
												~CWndFontColorManager();

	static	CWndFontColorManager*				GetInstance( void );

	// 윈도우 컨트롤	
	PFONTCOLOR_WNDBASE							GetWndBase( void )						{	return &m_stWndBase;					}
	PFONTCOLOR_WNDTEXT							GetWndText( void )						{	return &m_stWndText;					}
	PFONTCOLOR_WNDBUTTON						GetWndButton( void )					{	return &m_stWndButton;					}
	PFONTCOLOR_WNDDIALOG						GetWndDialog( void )					{	return &m_stWndDialog;					}
	PFONTCOLOR_WNDTABCTRL						GetWndTabCtrl( void )					{	return &m_stWndTabCtrl;					}	
	PFONTCOLOR_WNDLISTBOX						GetWndListBox( void )					{	return &m_stWndListBox;					}
	PFONTCOLOR_WNDTREECTRL						GetWndTreeCtrl( void )					{	return &m_stWndTreeCtrl;				}
	PFONTCOLOR_EDITSTRING						GetEditString( void	)					{	return &m_stEditString;					}

	// 컨텐츠
	PFONTCOLOR_WNDSELECTSERVER					GetWndSelectServer( void )				{	return &m_stWndSelectServer;			}
	PFONTCOLOR_WNDQUESTDETAIL					GetWndQuestDetail( void )				{	return &m_stWndQuestDetail;				}
	PFONTCOLOR_WNDTASKBAR						GetWndTaskBar( void )					{	return &m_stWndTaskBar;					}	
	PFONTCOLOR_WNDTOOLTIP						GetWndToolTip( void )					{	return &m_stWndToolTip;					}
	PFONTCOLOR_WNDCHARINFO						GetWndCharInfo( void )					{	return &m_stWndCharInfo;				}
	PFONTCOLOR_WNDPETSTATUS						GetWndPetStatus( void )					{	return &m_stWndPetStatus;				}
	PFONTCOLOR_WNDPARTYINFO						GetWndPartyInfo( void )					{	return &m_stWndPartyInfo;				}
	PFONTCOLOR_WNDPARTYSKILL					GetWndPartySkill( void )				{	return &m_stWndPartySkill;				}
	PFONTCOLOR_WNDMESSENGEREX					GetWndMessengerEx( void )				{	return &m_stWndMessengerEx;				}
	PFONTCOLOR_WNDFRIENDCTRLEX					GetWndFriendCtrlEx( void )				{	return &m_stWndFriendCtrlEx;			}
	PFONTCOLOR_WNDGUILDCTRLEX					GetWndGuildCtrlEx( void )				{	return &m_stWndGuildCtrlEx;				}
	PFONTCOLOR_WNDCAMPUS						GetWndCampus( void )					{	return &m_stWndCampus;					}
	PFONTCOLOR_WNDHOUSING						GetWndHousing( void )					{	return &m_stWndHousing;					}
	PFONTCOLOR_WNDGUILDHOUSING					GetWndGuildHousing( void )				{	return &m_stWndGuildHousing;			}
	PFONTCOLOR_WNDGUILDMETRICCTRL				GetWndGuildMetricCtrl( void )			{	return &m_stWndGuildMetricCtrl;			}
	PFONTCOLOR_WNDGUILDCOMBATRANK_CLASS			GetWndGuildCombatRank_Class( void )		{	return &m_stWndGuildCombatRank_Class;	}
	PFONTCOLOR_WNDSHOP_CART						GetWndShop_Cart( void )					{	return &m_stWndShop_Cart;				}
	PFONTCOLOR_WNDELLDINSJAR					GetWndElldinsJar( void )				{	return &m_stWndElldinsJar;				}
	PFONTCOLOR_WNDINVENTORY						GetWndInventory( void )					{	return &m_stWndInventory;				}
	PFONTCOLOR_WNDITEMCTRL						GetWndItemCtrl( void )					{	return &m_stWndItemCtrl;				}
	PFONTCOLOR_WNDPOSTSEND						GetWndPostSend( void )					{	return &m_stWndPostSend;				}
	PFONTCOLOR_WNDPOSTRECEIVE					GetWndPostReceive( void )				{	return &m_stWndPostReceive;				}
	PFONTCOLOR_WNDPOSTREAD						GetWndPostRead( void )					{	return &m_stWndPostRead;				}
	PFONTCOLOR_WNDGUILDTABMEMBER				GetWndGuildTabMember( void )			{	return &m_stWndGuildTabMember;			}
	PFONTCOLOR_WNDBUFFSTATUS					GetWndBuffStatus( void )				{	return &m_stWndBuffStatus;				}
	PFONTCOLOR_WNDMGR							GetWndMgr( void )						{	return &m_stWndMgr;						}
	PFONTCOLOR_WNDMADRIGALGIFT					GetWndMadrigalGift( void )				{	return &m_stWndMadrigalGift;			}
	PFONTCOLOR_WNDFLYFFPIECESYSTEM				GetWndFlyffPieceSystem( void )			{	return &m_stWndFlyffPieceSystem;		}	
	PFONTCOLOR_WNDWORLD							GetWndWorld( void )						{	return &m_stWndWorld;					}	
	PFONTCOLOR_WNDCOMMITEMCTRL					GetWndCommItemCtrl( void )				{	return &m_stWndCommItemCtrl;			}	
	PFONTCOLOR_WNDINSTANTMSG					GetWndInstantMsg( void )				{	return &m_stWndInstantMsg;				}	
	PFONTCOLOR_WNDCOMMONITEMEXCHANGE			GetWndCommonItemExchange( void )		{	return &m_stWndCommonItemExchange;		}


protected:


	FONTCOLOR_WNDBASE							m_stWndBase;
	FONTCOLOR_WNDTEXT							m_stWndText;
	FONTCOLOR_WNDBUTTON							m_stWndButton;
	FONTCOLOR_WNDDIALOG							m_stWndDialog;
	FONTCOLOR_WNDTABCTRL						m_stWndTabCtrl;
	FONTCOLOR_WNDLISTBOX						m_stWndListBox;
	FONTCOLOR_WNDTREECTRL						m_stWndTreeCtrl;
	FONTCOLOR_EDITSTRING						m_stEditString;		


	FONTCOLOR_WNDSELECTSERVER					m_stWndSelectServer;			// 서버 선택
	FONTCOLOR_WNDQUESTDETAIL					m_stWndQuestDetail;				// 퀘스트
	FONTCOLOR_WNDTOOLTIP						m_stWndToolTip;					// 툴팁
	FONTCOLOR_WNDTASKBAR						m_stWndTaskBar;					// 작업 바
	FONTCOLOR_WNDCHARINFO						m_stWndCharInfo;				// 캐릭터 정보
	FONTCOLOR_WNDPETSTATUS						m_stWndPetStatus;				// 펫 정보
	FONTCOLOR_WNDPARTYINFO						m_stWndPartyInfo;				// 극단
	FONTCOLOR_WNDPARTYSKILL						m_stWndPartySkill;				// 극단 스킬
	FONTCOLOR_WNDMESSENGEREX					m_stWndMessengerEx;				// 메신져
	FONTCOLOR_WNDFRIENDCTRLEX					m_stWndFriendCtrlEx;			// 메신져 (친구)
	FONTCOLOR_WNDGUILDCTRLEX					m_stWndGuildCtrlEx;				// 메신져 (길드)
	FONTCOLOR_WNDCAMPUS							m_stWndCampus;					// 메신져 (사제)
	FONTCOLOR_WNDHOUSING						m_stWndHousing;					// 하우징 (미니룸)
	FONTCOLOR_WNDGUILDHOUSING					m_stWndGuildHousing;			// 하우징 (길드)
	FONTCOLOR_WNDGUILDMETRICCTRL				m_stWndGuildMetricCtrl;			// 길드 공헌
	FONTCOLOR_WNDGUILDCOMBATRANK_CLASS			m_stWndGuildCombatRank_Class;	// 길드 대전 개인 순위
	FONTCOLOR_WNDSHOP_CART						m_stWndShop_Cart;				// 상점 (장바구니 포함)
	FONTCOLOR_WNDELLDINSJAR						m_stWndElldinsJar;				// 엘딘의 항아리
	FONTCOLOR_WNDINVENTORY						m_stWndInventory;				// 인벤토리 (장비)
	FONTCOLOR_WNDITEMCTRL						m_stWndItemCtrl;				// 인벤토리 (아이템)
	FONTCOLOR_WNDPOSTSEND						m_stWndPostSend;				// 우편 (보내기)
	FONTCOLOR_WNDPOSTRECEIVE					m_stWndPostReceive;				// 우편 (받기)
	FONTCOLOR_WNDPOSTREAD						m_stWndPostRead;				// 우편 (읽기)
	FONTCOLOR_WNDGUILDTABMEMBER					m_stWndGuildTabMember;			// 길드 (멤버)
	FONTCOLOR_WNDBUFFSTATUS						m_stWndBuffStatus;				// 버프 
	FONTCOLOR_WNDMGR							m_stWndMgr;						// 윈도우 메니져
	FONTCOLOR_WNDMADRIGALGIFT					m_stWndMadrigalGift;			// 마드리갈의 선물
	FONTCOLOR_WNDFLYFFPIECESYSTEM				m_stWndFlyffPieceSystem;		// 프리프 조각 시스템
	FONTCOLOR_WNDWORLD							m_stWndWorld;					// 월드
	FONTCOLOR_WNDCOMMITEMCTRL					m_stWndCommItemCtrl;			// 아이템 지속일자 컨트롤
	FONTCOLOR_WNDINSTANTMSG						m_stWndInstantMsg;				// 인스턴트 메시지 ( 알림 창 )
	FONTCOLOR_WNDCOMMONITEMEXCHANGE				m_stWndCommonItemExchange;		// 범용 아이템 교환 창

};

#define g_WndFontColorManager					CWndFontColorManager::GetInstance()
