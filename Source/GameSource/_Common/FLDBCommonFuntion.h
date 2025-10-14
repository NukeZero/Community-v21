#pragma once

void MakeItemLogInfo( LogItemInfo & o_info, const char * pszAction, const char * pszSender, const char * pszRecver
					 , const u_long uSenderID, const u_long uRecverID
					 , const DWORD dwWorldID, FLItemElem & kItemElem, const int nItemNum, const int nPayedGold, const int nPay );

										//, req.m_szPlayerName, req.m_dwWorldID, req.m_nGold, req.m_kItemElem, req.m_kItemElem.m_nItemNum );