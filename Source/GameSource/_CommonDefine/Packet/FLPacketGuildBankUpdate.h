
#ifndef __FLPACKETGUILDBANKUPDATE_H__
#define __FLPACKETGUILDBANKUPDATE_H__


#include "FLPacket.h"


//////////////////////////////////////////////////////////////////////////
// world -> trans

class	FLPacketGuildBankInputItemReq : public FLPacket
{
	public:
		FLPacketGuildBankInputItemReq();

		DWORD							dwGuildID;				// 길드 아이디
		DWORD							dwPlayerID;				// 플레이어 아이디
		CItemContainer< FLItemElem >	GuildBank;				// 길드 뱅크
		FLItemElem						kInputItemElem;			// 아이템 정보


	private:
		bool	BodySerialize( CAr & kAr ) const;
		bool	BodyDeserialize( CAr & kAr );
};

class	FLPacketGuildBankOutputItemReq : public FLPacket
{
	public:
		FLPacketGuildBankOutputItemReq();

		DWORD							dwGuildID;				// 길드 아이디
		DWORD							dwPlayerID;				// 플레이어 아이디
		CItemContainer< FLItemElem >	GuildBank;				// 길드 뱅크
		FLItemElem						kOutputItemElem;		// 아이템 정보


	private:
		bool	BodySerialize( CAr & kAr ) const;
		bool	BodyDeserialize( CAr & kAr );
};

class	FLPacketGuildBankInputGoldReq : public FLPacket
{
	public:
		FLPacketGuildBankInputGoldReq();

		DWORD		dwGuildID;					// 길드 아이디
		DWORD		dwPlayerID;					// 플레이어 아이디
		DWORD		dwGuildGold;				// 길드 골드
		DWORD		dwInputGold;				// 골드
		bool		bUpdateContribution;		// 플레이어 공헌 정보 업데이트 여부


	private:
		bool	BodySerialize( CAr & kAr ) const;
		bool	BodyDeserialize( CAr & kAr );
};

class	FLPacketGuildBankOutputGoldReq : public FLPacket
{
	public:
		FLPacketGuildBankOutputGoldReq();

		DWORD		dwGuildID;					// 길드 아이디
		DWORD		dwPlayerID;					// 플레이어 아이디
		DWORD		dwGuildGold;				// 길드 골드
		DWORD		dwOutputGold;				// 골드
		bool		bUpdateContribution;		// 플레이어 공헌도 정보 업데이트 유무


	private:
		bool	BodySerialize( CAr & kAr ) const;
		bool	BodyDeserialize( CAr & kAr );
};

// client -> world
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// trans -> world

class	FLPacketGuildBankInputItemAck : public FLPacket
{
	public:
		FLPacketGuildBankInputItemAck();

		bool			bSuccess;
		DWORD			dwGuildID;
		DWORD			dwPlayerID;
		FLItemElem		kInputItemElem;


	private:
		bool	BodySerialize( CAr & kAr )	const;
		bool	BodyDeserialize( CAr & kAr );
};

class	FLPacketGuildBankOutputItemAck : public FLPacket
{
	public:
		FLPacketGuildBankOutputItemAck();

		bool			bSuccess;
		DWORD			dwGuildID;
		DWORD			dwPlayerID;
		FLItemElem		kOutputItemElem;


	private:
		bool	BodySerialize( CAr & kAr )	const;
		bool	BodyDeserialize( CAr & kAr );
};

class	FLPacketGuildBankInputGoldAck : public FLPacket
{
	public:
		FLPacketGuildBankInputGoldAck();

		bool			bSuccess;
		DWORD			dwGuildID;
		DWORD			dwPlayerID;
		DWORD			dwInputGold;


	private:
		bool	BodySerialize( CAr & kAr )	const;
		bool	BodyDeserialize( CAr & kAr );
};

class	FLPacketGuildBankOutputGoldAck : public FLPacket
{
	public:
		FLPacketGuildBankOutputGoldAck();

		bool			bSuccess;
		DWORD			dwGuildID;
		DWORD			dwPlayerID;
		DWORD			dwOutputGold;


	private:
		bool	BodySerialize( CAr & kAr )	const;
		bool	BodyDeserialize( CAr & kAr );
};

// world -> client
//////////////////////////////////////////////////////////////////////////

#endif // __FLPACKETGUILDBANKUPDATE_H__
