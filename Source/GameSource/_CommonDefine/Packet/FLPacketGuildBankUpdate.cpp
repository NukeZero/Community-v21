
#include "StdAfx.h"
#include "FLPacketGuildBankUpdate.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// world -> trans

FLPacketGuildBankInputItemReq::FLPacketGuildBankInputItemReq()
: FLPacket( PACKETTYPE_GUILD_BANK_INPUT_ITEM_REQ )
{
	dwGuildID			= NULL_GUILD_ID;
	dwPlayerID			= NULL_PLAYER_ID;

	GuildBank.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_GUILD_BANK, MAX_GUILDBANK );
}

bool	FLPacketGuildBankInputItemReq::BodySerialize( CAr & kAr ) const
{
	kAr << dwGuildID;
	kAr << dwPlayerID;

	GuildBank.Serialize( kAr );
	kInputItemElem.Serialize( kAr );

	return true;
}

bool	FLPacketGuildBankInputItemReq::BodyDeserialize( CAr & kAr )
{
	kAr >> dwGuildID;
	kAr >> dwPlayerID;

	GuildBank.Deserialize( kAr );
	kInputItemElem.Deserialize( kAr );

	return true;
}

FLPacketGuildBankOutputItemReq::FLPacketGuildBankOutputItemReq()
: FLPacket( PACKETTYPE_GUILD_BANK_OUTPUT_ITEM_REQ )
{
	dwGuildID			= NULL_GUILD_ID;
	dwPlayerID			= NULL_PLAYER_ID;

	GuildBank.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_GUILD_BANK, MAX_GUILDBANK );
}

bool	FLPacketGuildBankOutputItemReq::BodySerialize( CAr & kAr ) const
{
	kAr << dwGuildID;
	kAr << dwPlayerID;

	GuildBank.Serialize( kAr );
	kOutputItemElem.Serialize( kAr );

	return true;
}

bool	FLPacketGuildBankOutputItemReq::BodyDeserialize( CAr & kAr )
{
	kAr >> dwGuildID;
	kAr >> dwPlayerID;

	GuildBank.Deserialize( kAr );
	kOutputItemElem.Deserialize( kAr );

	return true;
}

FLPacketGuildBankInputGoldReq::FLPacketGuildBankInputGoldReq()
: FLPacket( PACKETTYPE_GUILD_BANK_INPUT_GOLD_REQ )
{
	dwGuildID				= NULL_GUILD_ID;
	dwPlayerID				= NULL_PLAYER_ID;
	dwGuildGold				= 0;
	dwInputGold				= 0;
	bUpdateContribution		= false;
}

bool	FLPacketGuildBankInputGoldReq::BodySerialize( CAr & kAr ) const
{
	kAr << dwGuildID;
	kAr << dwPlayerID;
	kAr << dwGuildGold;
	kAr << dwInputGold;
	kAr << bUpdateContribution;

	return true;
}

bool	FLPacketGuildBankInputGoldReq::BodyDeserialize( CAr & kAr )
{
	kAr >> dwGuildID;
	kAr >> dwPlayerID;
	kAr >> dwGuildGold;
	kAr >> dwInputGold;
	kAr >> bUpdateContribution;

	return true;
}

FLPacketGuildBankOutputGoldReq::FLPacketGuildBankOutputGoldReq()
: FLPacket( PACKETTYPE_GUILD_BANK_OUTPUT_GOLD_REQ )
{
	dwGuildID				= NULL_GUILD_ID;
	dwPlayerID				= NULL_PLAYER_ID;
	dwGuildGold				= 0;
	dwOutputGold			= 0;
	bUpdateContribution		= false;
}

bool	FLPacketGuildBankOutputGoldReq::BodySerialize( CAr & kAr ) const
{
	kAr << dwGuildID;
	kAr << dwPlayerID;
	kAr << dwGuildGold;
	kAr << dwOutputGold;
	kAr << bUpdateContribution;

	return true;
}

bool	FLPacketGuildBankOutputGoldReq::BodyDeserialize( CAr & kAr )
{
	kAr >> dwGuildID;
	kAr >> dwPlayerID;
	kAr >> dwGuildGold;
	kAr >> dwOutputGold;
	kAr >> bUpdateContribution;

	return true;
}

// world -> trans
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// trans -> world

FLPacketGuildBankInputItemAck::FLPacketGuildBankInputItemAck()
: FLPacket( PACKETTYPE_GUILD_BANK_INPUT_ITEM_ACK )
{
	bSuccess		= false;
	dwGuildID		= NULL_GUILD_ID;
	dwPlayerID		= NULL_PLAYER_ID;
}

bool	FLPacketGuildBankInputItemAck::BodySerialize( CAr & kAr )	const
{
	kAr << bSuccess;
	kAr << dwGuildID;
	kAr << dwPlayerID;
	kInputItemElem.Serialize( kAr );

	return true;
}

bool	FLPacketGuildBankInputItemAck::BodyDeserialize( CAr & kAr )
{
	kAr >> bSuccess;
	kAr >> dwGuildID;
	kAr >> dwPlayerID;
	kInputItemElem.Deserialize( kAr );

	return true;
}

FLPacketGuildBankOutputItemAck::FLPacketGuildBankOutputItemAck()
: FLPacket( PACKETTYPE_GUILD_BANK_OUTPUT_ITEM_ACK )
{
	bSuccess		= false;
	dwGuildID		= NULL_GUILD_ID;
	dwPlayerID		= NULL_PLAYER_ID;
}

bool	FLPacketGuildBankOutputItemAck::BodySerialize( CAr & kAr )	const
{
	kAr << bSuccess;
	kAr << dwGuildID;
	kAr << dwPlayerID;
	kOutputItemElem.Serialize( kAr );

	return true;
}

bool	FLPacketGuildBankOutputItemAck::BodyDeserialize( CAr & kAr )
{
	kAr >> bSuccess;
	kAr >> dwGuildID;
	kAr >> dwPlayerID;
	kOutputItemElem.Deserialize( kAr );

	return true;
}

FLPacketGuildBankInputGoldAck::FLPacketGuildBankInputGoldAck()
: FLPacket( PACKETTYPE_GUILD_BANK_INPUT_GOLD_ACK )
{
	bSuccess		= false;
	dwGuildID		= NULL_GUILD_ID;
	dwPlayerID		= NULL_PLAYER_ID;
	dwInputGold		= 0;
}

bool	FLPacketGuildBankInputGoldAck::BodySerialize( CAr & kAr )	const
{
	kAr << bSuccess;
	kAr << dwGuildID;
	kAr << dwPlayerID;
	kAr << dwInputGold;

	return true;
}

bool	FLPacketGuildBankInputGoldAck::BodyDeserialize( CAr & kAr )
{
	kAr >> bSuccess;
	kAr >> dwGuildID;
	kAr >> dwPlayerID;
	kAr >> dwInputGold;

	return true;
}

FLPacketGuildBankOutputGoldAck::FLPacketGuildBankOutputGoldAck()
: FLPacket( PACKETTYPE_GUILD_BANK_OUTPUT_GOLD_ACK )
{
	bSuccess		= false;
	dwGuildID		= NULL_GUILD_ID;
	dwPlayerID		= NULL_PLAYER_ID;
	dwOutputGold	= 0;
}

bool	FLPacketGuildBankOutputGoldAck::BodySerialize( CAr & kAr )	const
{
	kAr << bSuccess;
	kAr << dwGuildID;
	kAr << dwPlayerID;
	kAr << dwOutputGold;

	return true;
}

bool	FLPacketGuildBankOutputGoldAck::BodyDeserialize( CAr & kAr )
{
	kAr >> bSuccess;
	kAr >> dwGuildID;
	kAr >> dwPlayerID;
	kAr >> dwOutputGold;

	return true;
}

// trans -> world
//////////////////////////////////////////////////////////////////////////
