#ifndef __FLCHARGEZONETICKET_H__
#define __FLCHARGEZONETICKET_H__

class FLChargeZoneTicket
{
	public:
		FLChargeZoneTicket();
		~FLChargeZoneTicket();

		static FLChargeZoneTicket*	GetInstance();

		void		OnDoUseChargeZoneTicket( FLWSUser* pUser, DWORD dwTicketObjID );

		void		EnterChargeZoneWorld( FLWSUser* pUser, FLPacketChargeZoneTicketEnteranceReq* pMsg );
		void		LeaveChargeZoneWorld( FLWSUser* pUser, FLPacketChargeZoneTicketLeaveReq* pMsg );

		bool		CanStayChargeZoneWorld( FLWSUser* pUser );

		FLItemElem*	GetUsingChargeZoneTicket( FLWSUser* pUser );

	private:
		bool		CanUseChargeZoneTicket( FLWSUser* pUser, DWORD dwItemObjID );
		bool		IsValidStateUser( FLWSUser* pUser );
		bool		IsValidStateItem( FLWSUser* pUser, FLItemElem* pItemElem );
		bool		HasSameActiveChargeZoneTicket( FLWSUser* pUser, FLItemElem* pItemElem );
		bool		IsMatching( DWORD dwItemID, DWORD dwWorldID, BYTE byChannel );
		bool		GetStartingPosition( DWORD dwItemID, DWORD dwWorldID, D3DXVECTOR3 & kPosition );
		bool		IsUsingChargeZoneTicket( FLWSUser* pUser, FLItemElem* pItemElem );

};

#define		g_pChargeZoneTicket		FLChargeZoneTicket::GetInstance()

#endif // __FLCHARGEZONETICKET_H__
