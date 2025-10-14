#pragma once


const size_t DEFAULT_TILE_SIZE	= 36;
//const size_t DEFAULT_TILE_SIZE	= 16;

const size_t DEFAULT_VIEW_RANGE_COUNT = 3;
//const size_t DEFAULT_VIEW_RANGE_COUNT = 7;


class FLMapGrid
{
public:
	struct FLTile
	{
		std::vector< CObj * >			vecObjCollision;
		//std::map< CCtrl *, CCtrl * >		arrCtrlSightMap[ MAX_LINKTYPE ];

		//FLRing< CObj >				listObjCollision;
		FLRing< CObj >				listCtrlSight[ MAX_LINKTYPE ];
	};

	struct FLTilePos
	{
		FLTilePos() : x( 0 ), y( 0 ) {}
		FLTilePos( const int X, const int Y ) : x( X ), y( Y ) {}

		int					x;
		int					y;
	};

	struct FLSyncIterator			//@@@@@@@ 테스트 아직 안함.
	{
		FLSyncIterator( const FLMapGrid & mapGrid, const D3DXVECTOR3 & vPixelCenterPos );
		FLSyncIterator( const FLMapGrid & mapGrid, const int nTileRangeCount, const D3DXVECTOR3 & vPixelCenterPos );
		FLSyncIterator( const FLMapGrid & mapGrid, const D3DXVECTOR3 & vPixelCenterPos, const size_t uRange );
		FLSyncIterator( const FLMapGrid & mapGrid, const D3DXVECTOR3 & vPixelCenterPos, const D3DXVECTOR3 & vPixelPos );
		FLSyncIterator( const FLMapGrid & mapGrid, const D3DXVECTOR3 & vPixelCenterPos, const D3DXVECTOR3 & vPixelPos1, const D3DXVECTOR3 & vPixelPos2 );

		/*const */FLTile *		GetNextTile();

	private:
		void *				operator new( size_t )			{}
		void				operator delete( void *, size_t ) {}

	private:
		const FLMapGrid &	kMapGrid;

		int					nReadCount;

		int					x_start;
		int					y_start;
		int					x_end;
		int					y_end;

		int					nWidth;
		int					nHeight;
	};



public:
	FLMapGrid();
	~FLMapGrid();

	bool					Create( const int nWidth, const int nHeight, const size_t uTileSize, const size_t uViewRangeCount );

	bool					Enter( CObj * pObj );
	bool					Leave( CObj * pObj );
	bool					TryMove( CObj * pObj, const D3DXVECTOR3 & vOldPixelPos, const D3DXVECTOR3 & vPixelPos );


	int						GetTileSize() const;
	int						GetViewRangeCount() const; 

private:
	bool					enterCollision( CObj * pObj, const D3DXVECTOR3 & vPixelPos );
	bool					leaveCollision( CObj * pObj, const D3DXVECTOR3 & vPixelPos );
	bool					tryMoveCollision( CObj * pObj, const D3DXVECTOR3 & vOldPixelPos, const D3DXVECTOR3 & vPixelPos );

	bool					enterSight( CCtrl * pCtrl, const D3DXVECTOR3 & vPixelPos );
	bool					leaveSight( CCtrl * pCtrl, const D3DXVECTOR3 & vPixelPos );
	bool					tryMoveSight( CCtrl * pCtrl, const D3DXVECTOR3 & vOldPixelPos, const D3DXVECTOR3 & vPixelPos );

private:
	void					onEnter( CCtrl * pCtrl, FLTile & kTile );
	void					onLeave( CCtrl * pCtrl, FLTile & kTile );

private:
	void					addTile( const size_t index, CCtrl * pCtrl );
	void					tryPopTile( const size_t index, CCtrl * pCtrl );
	FLTilePos				getPos( const D3DXVECTOR3 & vPixelPos ) const;
	int						getIndex( const D3DXVECTOR3 & vPixelPos ) const;
	int						getIndex( const FLTilePos & vTilePos ) const;

	bool					validY( const int y ) const;
	bool					validX( const int x ) const;
	bool					validPos( const FLTilePos & pos ) const;
	bool					validPixel( const D3DXVECTOR3 & vPixelPos ) const;

private:
	FLTile *				m_pTiles;

	DWORD					m_dwWidth;
	DWORD					m_dwHeight;
	DWORD					m_dwIndexMax;

	int						m_nTileSize;
	int						m_nViewRangeCount; //VIEW_RANGE_COUNT;
};



