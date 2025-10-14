#include "stdafx.h"
#include "CreateObj.h"


//////////////////////////////////////////////////////////////////////
// CItem - 이것은 게임 월드 상에 실제로 등장하는 오브젝트 아이템이다. 
//////////////////////////////////////////////////////////////////////
CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
	m_idHolder	= 0;
	m_idOwn = NULL_ID;
	m_dwDropTime = 0;
	m_bDropMob = 0;
	m_dwDropTime = timeGetTime();
#ifdef __CLIENT
	m_fGroundY = 0;
	m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;
#endif //__CLIENT
#ifdef __EVENT_MONSTER
	m_IdEventMonster = NULL_ID;
#endif // __EVENT_MONSTER
}

void CItem::SetOwner( OBJID id )
{
	m_idOwn = id;	// 이 아이템의 소유가 pAttacker(어태커)꺼란걸 표시.
	m_dwDropTime = ::timeGetTime();	// 드랍 했을당시의 시간을 기록함.
	m_bDropMob = TRUE;		// 몹이 죽어서 떨군 돈은 표시를 해둠.
}


CItem::~CItem()
{
	SAFE_DELETE( m_pItemBase );
	if( GetWorld() )
	{
#if !defined(__CLIENT)
#ifdef __LAYER_1021
		GetWorld()->m_respawner.Increment( GetRespawn(), m_nRespawnType, FALSE, GetLayer() );
#else	// __LAYER_1021
		GetWorld()->m_respawner.Increment( GetRespawn(), m_nRespawnType, FALSE );
#endif	// __LAYER_1021
#endif
	}
}

BOOL CItem::Read( CFileIO* pFile )
{
	CObj::Read( pFile );
	return TRUE;
}

#ifdef __CLIENT
// 아이템을 생성시킬땐 이것을 불러줘야 중력에 의해 떨어진다.
void CItem::SetDelta( float fGroundY, D3DXVECTOR3 &vDelta )
{
	m_fGroundY = fGroundY;
	m_vDelta = vDelta;
}
#endif // __CLIENT

void CItem::Process()
{
	CCtrl::Process();
#ifdef __CLIENT
	AddAngle( 0.5f );
	D3DXVECTOR3 vPos = GetPos();

	//--- 비행몹에게 떨어진 좌표는 이처리를 해선 안된다. 
	if( m_fGroundY )	// 이게 0이면 중력처리를 해선 안된다.
	{
		if( vPos.y > m_fGroundY )		// 공중에 떠있느냐?
		{
			m_vDelta.y -= 0.0075f;		// 이동벡터에 중력 벡터 더함.
		} 
		else
		{
			vPos.y = m_fGroundY;		// 바닥에 닿은 상태면 지면좌표와 동일하게 맞춤.
			m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;	// 이동벡터는 없어짐. 튀기게 하려면 이렇게 하면 안된다.
			SetPos( vPos );				// 최종 좌표 세팅.
		}

		// 이동벡터가 없으면 더해줄필요 없다.
		if( m_vDelta.x == 0 && m_vDelta.y == 0 && m_vDelta.z == 0 )
		{
			// 현재 좌표가 변경될 필요 없다.
		} 
		else
		{
			m_vDelta.x = m_vDelta.z = 0;		// 여기에 값 넣지말것.
			vPos += m_vDelta;			// 이동 벡터를 더함.
			SetPos( vPos );
		}
	}
#endif // __CLIENT

#ifdef __WORLDSERVER
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return;
	if( (int)g_tmCurrent - (int)m_dwDropTime > MIN( 3 ) )
		Delete();
#endif	// __WORLDSERVER
}

#ifndef __WORLDSERVER
void CItem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{

	PT_ITEM_SPEC pItemProp = GetProp();
	if( pItemProp && pItemProp->nReflect > 0 )
	{
		// 주의!!! : m_pModel이 CModelObject라는것을 가정하고 한것이다. 아니라면 이렇게 쓰면 안된다.
		((CModelObject*)m_pModel)->SetEffect( 0, XE_REFLECT );	
	}

	CObj::Render( pd3dDevice );
	if( xRandom( 50 ) == 1 )
		CreateSfx( pd3dDevice, XI_INDEX( 64, XI_GEN_ITEM_SHINE01 ), GetPos() );
}
#else
void CItem::Render( LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
{
}
#endif

#ifndef __WORLDSERVER
void CItem::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{

	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	pd3dDevice->GetViewport( &vp );
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	vOut.x -= pFont->GetTextExtent( m_pItemBase->GetProp()->szName ).cx / 2;
	pFont->DrawText( vOut.x + 1, vOut.y + 1, 0xff000000, m_pItemBase->GetProp()->szName	);
	pFont->DrawText( vOut.x, vOut.y, dwColor, m_pItemBase->GetProp()->szName );
	return;
}
#else
void CItem::RenderName( LPDIRECT3DDEVICE9 /*pd3dDevice*/, CD3DFont* /*pFont*/, DWORD /*dwColor*/ )
{

}
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
MEMPOOLER_IMPLEMENT( CItem, 512 );
#else	// __WORLDSERVER
MEMPOOLER_IMPLEMENT( CItem, 128 );
#endif	// __WORLDSERVER
