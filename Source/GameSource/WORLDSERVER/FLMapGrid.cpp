#include "StdAfx.h"

#include "FLMapGrid.h"

#undef max
#undef min


FLMapGrid::FLMapGrid() : m_pTiles(nullptr), m_dwWidth(0), m_dwHeight(0), m_dwIndexMax(0), m_nTileSize(0), m_nViewRangeCount(0)
{
}

FLMapGrid::~FLMapGrid()
{
	SAFE_DELETE_ARRAY(m_pTiles);
}

bool FLMapGrid::Create(const int nWidth, const int nHeight, const size_t uTileSize, const size_t uViewRangeCount)
{
	if (nWidth <= 0 || nHeight <= 0 || uTileSize <= 0 || uViewRangeCount <= 0)
	{
		DEBUG_BREAK;
		return false;
	}

	if (m_pTiles != nullptr)
	{
		DEBUG_BREAK;
		return false;
	}

	m_dwWidth = static_cast<DWORD>(nWidth);
	m_dwHeight = static_cast<DWORD>(nHeight);
	m_dwIndexMax = m_dwWidth * m_dwHeight;

	m_pTiles = new FLTile[m_dwWidth * m_dwHeight];
	m_nTileSize = uTileSize;
	m_nViewRangeCount = uViewRangeCount;

	return true;
}


bool FLMapGrid::Enter(CObj* pObj)
{
	if (pObj->IsCCtrl() == true)
	{
		CCtrl* pCtrl = dynamic_cast<CCtrl*>(pObj);
		enterSight(pCtrl, pCtrl->GetPos());
	}

	if (pObj->IsCollitionObj() == true)
	{
		enterCollision(pObj, pObj->GetPos());
	}

	return true;
}

bool FLMapGrid::Leave(CObj* pObj)
{
	if (pObj->IsCCtrl() == true)
	{
		CCtrl* pCtrl = dynamic_cast<CCtrl*>(pObj);
		leaveSight(pCtrl, pCtrl->GetPos());
	}

	if (pObj->IsCollitionObj() == true)
	{
		leaveCollision(pObj, pObj->GetPos());
	}

	return true;
}

bool FLMapGrid::TryMove(CObj* pObj, const D3DXVECTOR3& vOldPixelPos, const D3DXVECTOR3& vPixelPos)
{
	if (pObj->IsCCtrl() == true)
	{
		if (tryMoveSight(dynamic_cast<CMover*>(pObj), vOldPixelPos, vPixelPos) == false)
			return false;
	}


	if (pObj->IsCollitionObj() == true)
	{
		if (tryMoveCollision(pObj, vOldPixelPos, vPixelPos) == false)
			return false;
	}

	return true;
}


bool FLMapGrid::enterCollision(CObj* pObj, const D3DXVECTOR3& vPixelPos)
{
	if (IsValidObj(pObj) == FALSE)
		return false;

	const FLTilePos kPos = getPos(vPixelPos);

	// 엔터되는 타일 리브되는 타일
	if (validPos(kPos) == false)
	{
		DEBUG_BREAK;
		return false;
	}

	const int nModelMaxSize = static_cast<int>(pObj->GetModelMaxSize());

	const int nObjTileCountMax = nModelMaxSize / GetTileSize() + ((nModelMaxSize % GetTileSize()) ? 1 : 0);
	const int nTileRangeCountMax = nObjTileCountMax * 2 + 1;

	FLSyncIterator syncIt(*this, nTileRangeCountMax, vPixelPos);
	FLTile* pTile = syncIt.GetNextTile();
	while (pTile)
	{
		pTile->vecObjCollision.push_back(pObj);

		pTile = syncIt.GetNextTile();
	}

	return true;
}

bool FLMapGrid::leaveCollision(CObj* pObj, const D3DXVECTOR3& vPixelPos)
{
	if (pObj == NULL)
		return false;

	const FLTilePos kPos = getPos(vPixelPos);

	// 엔터되는 타일 리브되는 타일
	if (validPos(kPos) == false)
	{
		DEBUG_BREAK;
		return false;
	}

	const int nModelMaxSize = static_cast<int>(pObj->GetModelMaxSize());

	const int nObjTileCountMax = nModelMaxSize / GetTileSize() + ((nModelMaxSize % GetTileSize()) ? 1 : 0);
	const int nTileRangeCountMax = nObjTileCountMax * 2 + 1;

	FLSyncIterator syncIt(*this, nTileRangeCountMax, vPixelPos);
	FLTile* pTile = syncIt.GetNextTile();
	while (pTile)
	{
		std::vector< CObj* >::iterator it = find(pTile->vecObjCollision.begin(), pTile->vecObjCollision.end(), pObj);
		DEBUG_ASSERT(it != pTile->vecObjCollision.end());
		if (it != pTile->vecObjCollision.end())
			pTile->vecObjCollision.erase(it);

		pTile = syncIt.GetNextTile();
	}

	return true;
}

bool FLMapGrid::tryMoveCollision(CObj* pObj, const D3DXVECTOR3& vOldPixelPos, const D3DXVECTOR3& vPixelPos)
{

	if (IsValidObj(pObj) == FALSE)
		return false;

	const FLTilePos kOldPos = getPos(vOldPixelPos);
	const FLTilePos kNewPos = getPos(vPixelPos);

	if (kOldPos.x == kNewPos.x && kOldPos.y == kNewPos.y)
	{
		return true;
	}

	if (validPos(kOldPos) == false || validPos(kNewPos) == false)
	{
		DEBUG_BREAK;
		return false;
	}

	const int nModelMaxSize = static_cast<int>(pObj->GetModelMaxSize());

	const int nObjTileCountMax = nModelMaxSize / GetTileSize() + ((nModelMaxSize % GetTileSize()) ? 1 : 0);
	const int nTileRangeCountMax = nObjTileCountMax * 2 + 1;

	enum E_DIRECTION
	{
		OLDPOS_TO_NEWPOS = 0,
		NEWPOS_TO_OLDPOS,
		__E_DIRECTION_MAX__,
	};

	FLTilePos kMoved[__E_DIRECTION_MAX__];
	kMoved[OLDPOS_TO_NEWPOS].x = kNewPos.x - kOldPos.x;
	kMoved[OLDPOS_TO_NEWPOS].y = kNewPos.y - kOldPos.y;
	kMoved[NEWPOS_TO_OLDPOS].x = -kMoved[OLDPOS_TO_NEWPOS].x;
	kMoved[NEWPOS_TO_OLDPOS].y = -kMoved[OLDPOS_TO_NEWPOS].y;


	enum E_POS
	{
		OLDPOS = 0,
		NEWPOS,
		__E_POS_MAX__,
	};

	FLTilePos kPos[__E_POS_MAX__];
	kPos[OLDPOS] = kOldPos;
	kPos[NEWPOS] = kNewPos;

	for (int d = 0; d < _countof(kMoved); ++d)
	{
		const FLTilePos kStartTile(kPos[d].x - (nTileRangeCountMax >> 1), kPos[d].y - (nTileRangeCountMax >> 1));
		const FLTilePos kEndTile(kPos[d].x + (nTileRangeCountMax >> 1) + 1, kPos[d].y + (nTileRangeCountMax >> 1) + 1);

		//Leave |
		if (kMoved[d].x != 0)
		{
			const int x_start = (kMoved[d].x > 0)
				? std::max(kStartTile.x, 0)
				: std::min(std::max((kEndTile.x + kMoved[d].x), kStartTile.x), static_cast<int>(m_dwWidth));
			const int x_end = (kMoved[d].x > 0)
				? std::max(std::min((kStartTile.x + kMoved[d].x), kEndTile.x), 0)
				: std::min(kEndTile.x, static_cast<int>(m_dwWidth));

			const int y_start = std::max(kStartTile.y, 0);
			const int y_end = std::min(kEndTile.y, static_cast<int>(m_dwHeight));

			for (int x = x_start; x < x_end; ++x)
			{
#ifdef _DEBUG
				if (validX(x) == false)
					continue;
#endif

				for (int y = y_start; y < y_end; ++y)
				{
#ifdef _DEBUG
					if (validY(y) == false)
						continue;
#endif
					const size_t index = x + (y * m_dwWidth);
					if (d == OLDPOS_TO_NEWPOS)
					{
						std::vector< CObj* >::iterator it = find(m_pTiles[index].vecObjCollision.begin(), m_pTiles[index].vecObjCollision.end(), pObj);
						DEBUG_ASSERT(it != m_pTiles[index].vecObjCollision.end());
						if (it != m_pTiles[index].vecObjCollision.end())
							m_pTiles[index].vecObjCollision.erase(it);
					}
					else if (d == NEWPOS_TO_OLDPOS)
						m_pTiles[index].vecObjCollision.push_back(pObj);
				}
			}
		}

		//Leave -
		if (kMoved[d].y != 0)
		{
			const int x_start = std::max(std::min((kStartTile.x + ((kMoved[d].x > 0) ? kMoved[d].x : 0)), kEndTile.x), 0);
			const int x_end = std::min(std::max((kEndTile.x + ((kMoved[d].x < 0) ? kMoved[d].x : 0)), kStartTile.x), static_cast<int>(m_dwWidth));

			const int y_start = (kMoved[d].y > 0)
				? std::max(kStartTile.y, 0)
				: std::min(std::max((kEndTile.y + kMoved[d].y), kStartTile.y), static_cast<int>(m_dwHeight));
			const int y_end = (kMoved[d].y > 0)
				? std::max(std::min((kStartTile.y + kMoved[d].y), kEndTile.y), 0)
				: std::min(kEndTile.y, static_cast<int>(m_dwHeight));

			for (int x = x_start; x < x_end; ++x)
			{
#ifdef _DEBUG
				if (validX(x) == false)
					continue;
#endif
				for (int y = y_start; y < y_end; ++y)
				{
#ifdef _DEBUG
					if (validY(y) == false)
						continue;
#endif
					const size_t index = x + (y * m_dwWidth);
					if (d == OLDPOS_TO_NEWPOS)
					{
						std::vector< CObj* >::iterator it = find(m_pTiles[index].vecObjCollision.begin(), m_pTiles[index].vecObjCollision.end(), pObj);
						DEBUG_ASSERT(it != m_pTiles[index].vecObjCollision.end());
						if (it != m_pTiles[index].vecObjCollision.end())
							m_pTiles[index].vecObjCollision.erase(it);
					}
					else if (d == NEWPOS_TO_OLDPOS)
						m_pTiles[index].vecObjCollision.push_back(pObj);
				}
			}
		}
	}



	//@@@@@@@@@@@@@@@ 나중에 최적화 하도록 하자
	//LeaveCollision( pObj, vOldPixelPos );
	//EnterCollision( pObj, vPixelPos );
	return true;
}



bool FLMapGrid::enterSight(CCtrl* pCtrl, const D3DXVECTOR3& vPixelPos)
{
#ifdef _DEBUG
//	const bool bPlayer		= ( pCtrl->GetType() == OT_MOVER && static_cast< CMover * >( pCtrl )->IsPlayer() == TRUE  );
#endif 

	if (IsValidObj(pCtrl) == FALSE)
		return false;

	const FLTilePos kPos = getPos(vPixelPos);

	// 엔터되는 타일 리브되는 타일
	if (validPos(kPos) == false)
	{
		DEBUG_BREAK;
		return false;
	}

	const int x_start = std::max((kPos.x - (m_nViewRangeCount >> 1)), 0);
	const int y_start = std::max((kPos.y - (m_nViewRangeCount >> 1)), 0);

	const int x_end = std::min((kPos.x + (m_nViewRangeCount >> 1) + 1), static_cast<int>(m_dwWidth));
	const int y_end = std::min((kPos.y + (m_nViewRangeCount >> 1) + 1), static_cast<int>(m_dwHeight));

	for (int x = x_start; x < x_end; ++x)
	{
		for (int y = y_start; y < y_end; ++y)
		{
			const int index = x + (y * m_dwWidth);
			onEnter(pCtrl, m_pTiles[index]);
		}
	}

	addTile(kPos.x + (kPos.y * m_dwWidth), pCtrl);

	return true;
}


bool FLMapGrid::leaveSight(CCtrl* pCtrl, const D3DXVECTOR3& vPixelPos)
{
#ifdef _DEBUG
//	const bool bPlayer		= ( pCtrl->GetType() == OT_MOVER && static_cast< CMover * >( pCtrl )->IsPlayer() == TRUE  );
#endif 

	if (pCtrl == NULL)
		return false;

	const FLTilePos kPos = getPos(vPixelPos);

	// 엔터되는 타일 리브되는 타일

	if (validPos(kPos) == false)
	{
		DEBUG_BREAK;
		return false;
	}

	const int x_start = std::max((kPos.x - (m_nViewRangeCount >> 1)), 0);
	const int y_start = std::max((kPos.y - (m_nViewRangeCount >> 1)), 0);

	const int x_end = std::min((kPos.x + (m_nViewRangeCount >> 1) + 1), static_cast<int>(m_dwWidth));
	const int y_end = std::min((kPos.y + (m_nViewRangeCount >> 1) + 1), static_cast<int>(m_dwHeight));

	tryPopTile(kPos.x + (kPos.y * m_dwWidth), pCtrl);

	for (int x = x_start; x < x_end; ++x)
	{
		for (int y = y_start; y < y_end; ++y)
		{
			const int index = x + (y * m_dwWidth);
			onLeave(pCtrl, m_pTiles[index]);
		}
	}

	return true;
}



bool FLMapGrid::tryMoveSight(CCtrl* pCtrl, const D3DXVECTOR3& vOldPixelPos, const D3DXVECTOR3& vPixelPos)
{
#ifdef _DEBUG
	//	const bool bPlayer		= ( pCtrl->GetType() == OT_MOVER && static_cast< CMover * >( pCtrl )->IsPlayer() == TRUE  );
#endif 

	if (IsValidObj(pCtrl) == FALSE)
		return false;

	const FLTilePos kOldPos = getPos(vOldPixelPos);
	const FLTilePos kNewPos = getPos(vPixelPos);

	if (kOldPos.x == kNewPos.x && kOldPos.y == kNewPos.y)
	{
		return true;
	}

	if (validPos(kOldPos) == false || validPos(kNewPos) == false)
	{
		DEBUG_BREAK;
		return false;
	}


	tryPopTile(getIndex(kOldPos), pCtrl);
	addTile(getIndex(kNewPos), pCtrl);

	enum E_DIRECTION
	{
		OLDPOS_TO_NEWPOS = 0,
		NEWPOS_TO_OLDPOS,
		__E_DIRECTION_MAX__,
	};

	FLTilePos kMoved[__E_DIRECTION_MAX__];
	kMoved[OLDPOS_TO_NEWPOS].x = kNewPos.x - kOldPos.x;
	kMoved[OLDPOS_TO_NEWPOS].y = kNewPos.y - kOldPos.y;
	kMoved[NEWPOS_TO_OLDPOS].x = -kMoved[OLDPOS_TO_NEWPOS].x;
	kMoved[NEWPOS_TO_OLDPOS].y = -kMoved[OLDPOS_TO_NEWPOS].y;


	enum E_POS
	{
		OLDPOS = 0,
		NEWPOS,
		__E_POS_MAX__,
	};

	FLTilePos kPos[__E_POS_MAX__];
	kPos[OLDPOS] = kOldPos;
	kPos[NEWPOS] = kNewPos;



	for (int d = 0; d < _countof(kMoved); ++d)
	{
		const FLTilePos kStartTile(kPos[d].x - (m_nViewRangeCount >> 1), kPos[d].y - (m_nViewRangeCount >> 1));
		const FLTilePos kEndTile(kPos[d].x + (m_nViewRangeCount >> 1) + 1, kPos[d].y + (m_nViewRangeCount >> 1) + 1);

		//Leave |
		if (kMoved[d].x != 0)
		{
			const int x_start = (kMoved[d].x > 0)
				? std::max(kStartTile.x, 0)
				: std::min(std::max((kEndTile.x + kMoved[d].x), kStartTile.x), static_cast<int>(m_dwWidth));
			const int x_end = (kMoved[d].x > 0)
				? std::max(std::min((kStartTile.x + kMoved[d].x), kEndTile.x), 0)
				: std::min(kEndTile.x, static_cast<int>(m_dwWidth));

			const int y_start = std::max(kStartTile.y, 0);
			const int y_end = std::min(kEndTile.y, static_cast<int>(m_dwHeight));

			for (int x = x_start; x < x_end; ++x)
			{
#ifdef _DEBUG
				if (validX(x) == false)
					continue;
#endif

				for (int y = y_start; y < y_end; ++y)
				{
#ifdef _DEBUG
					if (validY(y) == false)
						continue;
#endif
					const size_t index = x + (y * m_dwWidth);
					if (d == OLDPOS_TO_NEWPOS)
						onLeave(pCtrl, m_pTiles[index]);
					else if (d == NEWPOS_TO_OLDPOS)
						onEnter(pCtrl, m_pTiles[index]);
				}
			}
		}

		//Leave -
		if (kMoved[d].y != 0)
		{
			const int x_start = std::max(std::min((kStartTile.x + ((kMoved[d].x > 0) ? kMoved[d].x : 0)), kEndTile.x), 0);
			const int x_end = std::min(std::max((kEndTile.x + ((kMoved[d].x < 0) ? kMoved[d].x : 0)), kStartTile.x), static_cast<int>(m_dwWidth));

			const int y_start = (kMoved[d].y > 0)
				? std::max(kStartTile.y, 0)
				: std::min(std::max((kEndTile.y + kMoved[d].y), kStartTile.y), static_cast<int>(m_dwHeight));
			const int y_end = (kMoved[d].y > 0)
				? std::max(std::min((kStartTile.y + kMoved[d].y), kEndTile.y), 0)
				: std::min(kEndTile.y, static_cast<int>(m_dwHeight));

			for (int x = x_start; x < x_end; ++x)
			{
#ifdef _DEBUG
				if (validX(x) == false)
					continue;
#endif
				for (int y = y_start; y < y_end; ++y)
				{
#ifdef _DEBUG
					if (validY(y) == false)
						continue;
#endif
					const size_t index = x + (y * m_dwWidth);
					if (d == OLDPOS_TO_NEWPOS)
						onLeave(pCtrl, m_pTiles[index]);
					else if (d == NEWPOS_TO_OLDPOS)
						onEnter(pCtrl, m_pTiles[index]);
				}
			}
		}
	}

	return true;
}



int FLMapGrid::GetTileSize() const
{
	return m_nTileSize;
}

int FLMapGrid::GetViewRangeCount() const
{
	return m_nViewRangeCount;
}

void FLMapGrid::addTile(const size_t index, CCtrl* pCtrl)
{
#ifdef _DEBUG
//	const bool bPlayer		= ( pCtrl->GetType() == OT_MOVER && static_cast< CMover * >( pCtrl )->IsPlayer() == TRUE  );
#endif 

	if (m_dwIndexMax <= index || index < 0)
	{
		DEBUG_BREAK;
		return;
	}

	const int eLinkType = pCtrl->GetLinkType();
	if (0 > eLinkType || eLinkType >= MAX_LINKTYPE)
	{
		DEBUG_BREAK;
		return;
	}

	const bool ret = m_pTiles[index].listCtrlSight[eLinkType].InsertBack(m_pTiles[index].listCtrlSight[eLinkType].Begin(), pCtrl);

	//m_pTiles[ index ].arrCtrlSightMap[ eLinkType ].insert( make_pair( pCtrl->GetId(), pCtrl->GetId() ) );
	//const bool ret = m_pTiles[ index ].arrCtrlSightMap[ eLinkType ].insert( make_pair( pCtrl, pCtrl ) ).second;



	DEBUG_ASSERT(ret == true);
}

void FLMapGrid::tryPopTile(const size_t index, CCtrl* pCtrl)
{
#ifdef _DEBUG
//	const bool bPlayer		= ( pCtrl->GetType() == OT_MOVER && static_cast< CMover * >( pCtrl )->IsPlayer() == TRUE  );
#endif 

	if (m_dwIndexMax <= index || index < 0)
	{
		DEBUG_BREAK;
		return;
	}

	const int eLinkType = pCtrl->GetLinkType();
	if (0 > eLinkType || eLinkType >= MAX_LINKTYPE)
	{
		DEBUG_BREAK;
		return;
	}

	//const size_t ret =  m_pTiles[ index ].arrCtrlSightMap[ eLinkType ].erase( pCtrl );
	//DEBUG_EXCUTE( RELEASE_ASSERT( ret == 1 ) );

	const bool ret = m_pTiles[index].listCtrlSight[eLinkType].UnlinkNode(pCtrl);
	DEBUG_ASSERT(ret == true);

}

FLMapGrid::FLTilePos FLMapGrid::getPos(const D3DXVECTOR3& vPixelPos) const
{
	const int maxX = m_dwWidth * m_nTileSize;
	const int maxY = m_dwHeight * m_nTileSize;

	FLTilePos pos;
	pos.x = static_cast<int>(std::max(std::min(static_cast<int>(vPixelPos.x), maxX), 0) / m_nTileSize);
	pos.y = static_cast<int>(std::max(std::min(static_cast<int>(vPixelPos.z), maxY), 0) / m_nTileSize);

	return pos;
}

int FLMapGrid::getIndex(const D3DXVECTOR3& vPixelPos) const
{
	const FLTilePos	pos = getPos(vPixelPos);
	const size_t index = pos.x + (pos.y * m_dwWidth);
	return index;
}

int FLMapGrid::getIndex(const FLTilePos& vTilePos) const
{
	const size_t index = vTilePos.x + (vTilePos.y * m_dwWidth);
	return index;
}

bool FLMapGrid::validY(const int y) const
{
	return (0 <= y && y < static_cast<int>(m_dwHeight));
}

bool FLMapGrid::validX(const int x) const
{
	return (0 <= x && x < static_cast<int>(m_dwWidth));
}

bool FLMapGrid::validPos(const FLTilePos& pos) const
{
	return (validX(pos.x) == true && validY(pos.y) == true);
}

bool FLMapGrid::validPixel(const D3DXVECTOR3& vPixelPos) const
{
	if (0 > vPixelPos.x || vPixelPos.x > (m_dwWidth * m_nTileSize))
		return false;

	if (0 > vPixelPos.z || vPixelPos.z > (m_dwHeight * m_nTileSize))
		return false;

	return true;
}


void FLMapGrid::onEnter(CCtrl* pSrcCtrl, FLTile& kTile)
{
//#ifdef _DEBUG
	const bool bSrcPlayer = (pSrcCtrl->GetType() == OT_MOVER && static_cast<CMover*>(pSrcCtrl)->IsPlayer() == TRUE);
//#endif 

	for (size_t i = 0; i < MAX_LINKTYPE; ++i)
	{
		//for( std::map< CCtrl *, CCtrl * >::iterator it = kTile.arrCtrlSightMap[ i ].begin(); it != kTile.arrCtrlSightMap[ i ].end();  )
		for ( /*std::map< CCtrl *, CCtrl * >::iterator*/FLRing<CObj>::iterator it = kTile.listCtrlSight[i].Begin(); it != kTile.listCtrlSight[i].End(); )
		{
			//CCtrl * pDestCtrl		= it->second;
			CCtrl* pDestCtrl = static_cast<CCtrl*>(it._MyNode());
			if (IsValidObj(pDestCtrl) == FALSE)
			{
				++it;
				continue;
			}

			if (pDestCtrl->GetId() == pSrcCtrl->GetId())
			{
				++it;
				continue;
			}


			//내가 Player라면		// 주변 정보를 보냄
			if (bSrcPlayer == true)
			{
				static_cast<FLWSUser*>(pSrcCtrl)->AddAddObj(pDestCtrl);
				if (pDestCtrl->m_dwType == OT_MOVER)
					++static_cast<CMover*>(pDestCtrl)->m_nNearPCCount;
			}

			//상대방이 Player라면
			const bool bDestPlayer = (pDestCtrl->GetType() == OT_MOVER && static_cast<CMover*>(pDestCtrl)->IsPlayer() == TRUE);
			if (bDestPlayer == true)
			{
				static_cast<FLWSUser*>(pDestCtrl)->AddAddObj(pSrcCtrl);
				if (pSrcCtrl->m_dwType == OT_MOVER)
					++static_cast<CMover*>(pSrcCtrl)->m_nNearPCCount;
			}

			++it;
		}
	}
}

void FLMapGrid::onLeave(CCtrl* pSrcCtrl, FLTile& kTile)
{
//#ifdef _DEBUG
	const bool bSrcPlayer = (pSrcCtrl->GetType() == OT_MOVER && static_cast<CMover*>(pSrcCtrl)->IsPlayer() == TRUE);
//#endif 

	for (size_t i = 0; i < MAX_LINKTYPE; ++i)
	{
		//for( std::map< OBJID, OBJID >::iterator it = kTile.arrCtrlSightMap[ i ].begin(); it != kTile.arrCtrlSightMap[ i ].end();  )
		for (FLRing< CObj >::iterator it = kTile.listCtrlSight[i].Begin(); it != kTile.listCtrlSight[i].End(); )
		{
			//CCtrl* pDestCtrl	= prj.GetCtrl( it->first );
			//CCtrl * pDestCtrl		= static_cast< CCtrl * >( it->second );
			//CCtrl * pDestCtrl		= static_cast< CCtrl * >( it->second );
			CCtrl* pDestCtrl = static_cast<CCtrl*>(it._MyNode());
			if (IsValidObj(pDestCtrl) == FALSE)
			{
				++it;
				continue;
			}

			if (pDestCtrl->GetId() == pSrcCtrl->GetId())
			{
				++it;
				continue;
			}

			//내가 Player라면		// 주변 정보를 보냄
			if (bSrcPlayer == true)
			{
				static_cast<FLWSUser*>(pSrcCtrl)->AddRemoveObj(pDestCtrl->GetId());
				if (pDestCtrl->m_dwType == OT_MOVER)
					--static_cast<CMover*>(pDestCtrl)->m_nNearPCCount;
			}

			//상대방이 Player라면
			const bool bDestPlayer = (pDestCtrl->GetType() == OT_MOVER && static_cast<CMover*>(pDestCtrl)->IsPlayer() == TRUE);
			if (bDestPlayer == true)
			{
				static_cast<FLWSUser*>(pDestCtrl)->AddRemoveObj(pSrcCtrl->GetId());
				if (pSrcCtrl->m_dwType == OT_MOVER)
					--static_cast<CMover*>(pSrcCtrl)->m_nNearPCCount;
			}

			//if( pSrcCtrl->GetType() == OT_MOVER && pDestCtrl->GetId() != pSrcCtrl->GetId() )
			//{
			//	CMover * pSrcMover		= static_cast< CMover * >( pSrcCtrl );
			//	if( pSrcMover->IsPlayer() == TRUE )
			//		static_cast< FLWSUser * >( pSrcMover )->AddRemoveObj( pDestCtrl->GetId() );							//남의 정보를 얻음

			//	if( pDestCtrl->GetType() == OT_MOVER ) 
			//	{
			//		CMover * pDestMover	= static_cast< CMover * >( pDestCtrl );
			//		if( pDestMover->IsPlayer() == TRUE )		// 내정보를 보내줌
			//		{
			//			static_cast< FLWSUser * >( pDestMover )->AddRemoveObj( pSrcCtrl->GetId() );
			//			--pSrcMover->m_nNearPCCount;
			//		}

			//		if( pSrcMover->IsPlayer() == TRUE )
			//			--pDestMover->m_nNearPCCount;
			//	}
			//}

			++it;
		}
	}
}





//----------------------------------------------------------------------------------------------------------------------------------//


FLMapGrid::FLSyncIterator::FLSyncIterator(const FLMapGrid& mapGrid, const D3DXVECTOR3& vPixelCenterPos)
	: nReadCount(0), kMapGrid(mapGrid)
{
	//const FLTilePos pos		= kMapGrid.getPos( vPixelCenterPos );

//	const size_t maxX	= mapGrid.m_dwWidth * mapGrid.GetTileSize();
//	const size_t maxY	= mapGrid.m_dwHeight * mapGrid.GetTileSize();

	FLTilePos pos;
	pos.x = static_cast<int>(vPixelCenterPos.x) / mapGrid.GetTileSize();
	pos.y = static_cast<int>(vPixelCenterPos.z) / mapGrid.GetTileSize();


	const int x_range_min = pos.x - (mapGrid.GetViewRangeCount() >> 1);
	const int x_range_max = pos.x + (mapGrid.GetViewRangeCount() >> 1);
	const int y_range_min = pos.y - (mapGrid.GetViewRangeCount() >> 1);
	const int y_range_max = pos.y + (mapGrid.GetViewRangeCount() >> 1);

// 	if( kMapGrid.validX( x_range_min ) == false && kMapGrid.validX( x_range_max ) == false )
// 		return;
// 
// 	if( kMapGrid.validY( y_range_min ) == false && kMapGrid.validY( y_range_max ) == false )
// 		return;

	x_start = std::max(x_range_min, 0);
	y_start = std::max(y_range_min, 0);

	x_end = std::min((x_range_max + 1), static_cast<int>(kMapGrid.m_dwWidth));
	y_end = std::min((y_range_max + 1), static_cast<int>(kMapGrid.m_dwHeight));

	nWidth = x_end - x_start;
	nHeight = y_end - y_start;
}

FLMapGrid::FLSyncIterator::FLSyncIterator(const FLMapGrid& mapGrid, const int nTileRangeCount, const D3DXVECTOR3& vPixelCenterPos)
	: nReadCount(0), kMapGrid(mapGrid)
{
	DEBUG_ASSERT(nTileRangeCount >= 0);

	FLTilePos pos;
	pos.x = static_cast<int>(vPixelCenterPos.x) / mapGrid.GetTileSize();
	pos.y = static_cast<int>(vPixelCenterPos.z) / mapGrid.GetTileSize();


	const int x_range_min = pos.x - (nTileRangeCount >> 1);
	const int x_range_max = pos.x + (nTileRangeCount >> 1);
	const int y_range_min = pos.y - (nTileRangeCount >> 1);
	const int y_range_max = pos.y + (nTileRangeCount >> 1);

// 	if( kMapGrid.validX( x_range_min ) == false && kMapGrid.validX( x_range_max ) == false )
// 		return;
// 
// 	if( kMapGrid.validY( y_range_min ) == false && kMapGrid.validY( y_range_max ) == false )
// 		return;

	x_start = std::max(x_range_min, 0);
	y_start = std::max(y_range_min, 0);

	x_end = std::min((x_range_max + 1), static_cast<int>(kMapGrid.m_dwWidth));
	y_end = std::min((y_range_max + 1), static_cast<int>(kMapGrid.m_dwHeight));

	nWidth = x_end - x_start;
	nHeight = y_end - y_start;
}

FLMapGrid::FLSyncIterator::FLSyncIterator(const FLMapGrid& mapGrid, const D3DXVECTOR3& vPixelCenterPos, const size_t uRange)
	: nReadCount(0), kMapGrid(mapGrid)
{
	const D3DXVECTOR3 vPixelLeftTop(vPixelCenterPos.x - uRange, vPixelCenterPos.y, vPixelCenterPos.z - uRange);
	const D3DXVECTOR3 vPixelRightBottom(vPixelCenterPos.x + uRange, vPixelCenterPos.y, vPixelCenterPos.z + uRange);

	const D3DXVECTOR3 vPixelLeftBottom(vPixelCenterPos.x - uRange, vPixelCenterPos.y, vPixelCenterPos.z + uRange);
	const D3DXVECTOR3 vPixelRightTop(vPixelCenterPos.x + uRange, vPixelCenterPos.y, vPixelCenterPos.z - uRange);


	const FLTilePos kPosMin = kMapGrid.getPos(vPixelLeftTop);
	const FLTilePos kPosMax = kMapGrid.getPos(vPixelRightBottom);

	x_start = std::max(kPosMin.x, 0);
	y_start = std::max(kPosMin.y, 0);

	x_end = std::min((kPosMax.x + 1), static_cast<int>(kMapGrid.m_dwWidth));
	y_end = std::min((kPosMax.y + 1), static_cast<int>(kMapGrid.m_dwHeight));

	nWidth = x_end - x_start;
	nHeight = y_end - y_start;
}

FLMapGrid::FLSyncIterator::FLSyncIterator(const FLMapGrid& mapGrid, const D3DXVECTOR3& vPixelCenterPos, const D3DXVECTOR3& vPixelPos)
	: nReadCount(0), kMapGrid(mapGrid)
{
// 	if( mapGrid.validPixel( vPixelCenterPos ) == false && mapGrid.validPixel( vPixelPos ) == false )
// 		return;

	const D3DXVECTOR3 vPixelMin(std::min(vPixelCenterPos.x, vPixelPos.x), vPixelPos.y, std::min(vPixelCenterPos.z, vPixelPos.z));
	const D3DXVECTOR3 vPixelMax(std::max(vPixelCenterPos.x, vPixelPos.x), vPixelPos.y, std::max(vPixelCenterPos.z, vPixelPos.z));

	const FLTilePos kPosMin = kMapGrid.getPos(vPixelMin);
	const FLTilePos kPosMax = kMapGrid.getPos(vPixelMax);

	x_start = std::max(kPosMin.x, 0);
	y_start = std::max(kPosMin.y, 0);

	x_end = std::min((kPosMax.x + 1), static_cast<int>(kMapGrid.m_dwWidth));
	y_end = std::min((kPosMax.y + 1), static_cast<int>(kMapGrid.m_dwHeight));

	nWidth = x_end - x_start;
	nHeight = y_end - y_start;
}

FLMapGrid::FLSyncIterator::FLSyncIterator(const FLMapGrid& mapGrid, const D3DXVECTOR3& vPixelCenterPos, const D3DXVECTOR3& vPixelPos1, const D3DXVECTOR3& vPixelPos2)
	: nReadCount(0), kMapGrid(mapGrid)
{
// 	if( mapGrid.validPixel( vPixelCenterPos ) == false && mapGrid.validPixel( vPixelPos1 ) == false && mapGrid.validPixel( vPixelPos2 ) == false )
// 		return;

	const D3DXVECTOR3 vPixelMin(std::min(vPixelCenterPos.x, std::min(vPixelPos1.x, vPixelPos2.x)), vPixelCenterPos.y, std::min(vPixelCenterPos.z, std::min(vPixelPos1.z, vPixelPos2.z)));
	const D3DXVECTOR3 vPixelMax(std::max(vPixelCenterPos.x, std::max(vPixelPos1.x, vPixelPos2.x)), vPixelCenterPos.y, std::max(vPixelCenterPos.z, std::max(vPixelPos1.z, vPixelPos2.z)));

	const FLTilePos kPosMin = kMapGrid.getPos(vPixelMin);
	const FLTilePos kPosMax = kMapGrid.getPos(vPixelMax);

	x_start = std::max(kPosMin.x, 0);
	y_start = std::max(kPosMin.y, 0);

	x_end = std::min((kPosMax.x + 1), static_cast<int>(kMapGrid.m_dwWidth));
	y_end = std::min((kPosMax.y + 1), static_cast<int>(kMapGrid.m_dwHeight));

	nWidth = x_end - x_start;
	nHeight = y_end - y_start;
}

/*const */FLMapGrid::FLTile* FLMapGrid::FLSyncIterator::GetNextTile()
{
	if (nWidth <= 0 || nHeight <= 0)
		return NULL;

	const int x = x_start + (nReadCount % nWidth);
	const int y = y_start + (nReadCount / nWidth);
	if (y >= y_end)
		return NULL;

	const int index = kMapGrid.getIndex(FLTilePos(x, y));

	++nReadCount;
	return &kMapGrid.m_pTiles[index];
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------//
