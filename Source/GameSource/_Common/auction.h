#ifndef __AUCTION_H__
#define	__AUCTION_H__

class FLWSUser;
class CAuction
{
public:
	CAuction();
	virtual	~CAuction();

	static	CAuction*	GetInstance( void );

	void	ActionOff( FLWSUser* FLWSUser, DWORD dwItemObjID, int nItemNum, DWORD dw1, DWORD dw2 );
	void	Bid( int nAuctionId, DWORD dwBid, BOOL bKnockdown );
	void	KnockDown( u_long idPlayer, int nAuctionId );
};

#endif	// __AUCTION_H__
