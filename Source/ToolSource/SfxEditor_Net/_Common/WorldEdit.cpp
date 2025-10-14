#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CWorld::Undo()
{

	return TRUE;
}
BOOL CWorld::Redo() 
{

	return TRUE;
}
void CWorld::RenderBoundBox()
{
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	int nSize = m_arySelect.size();

	for( int i = 0; i < nSize; i++)
	{
		CObj* pObj = (CObj*)m_arySelect.at( i ); 
		SetBoundBoxVertex( pObj );
		RenderBoundBoxVertex( pObj );
	}
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, m_bViewLight );
}
/*
void CWorld::RenderGrid()
{
	//if( m_bViewGrid || m_bViewGridPatch || m_bViewGridLandscape )
	{ 
		if( m_bViewGrid )
	 		RenderGrids( CRect( 0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU) , 1, 0xff808080 );
		if( m_bViewGridPatch )
			RenderGrids( CRect( 0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU) , MAP_SIZE / NUM_PATCHES_PER_SIDE, 0xffffffff );
		if( m_bViewGridLandscape )
			RenderGrids( CRect( 0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU) , MAP_SIZE, 0xffffff00 );

		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pd3dDevice->SetTexture( 0, NULL);
		m_pd3dDevice->SetVertexShader( D3DFVF_GRIDVERTEX );
		m_pd3dDevice->SetStreamSource( 0, m_pGridVertexBuffer, sizeof(GRIDVERTEX) );
		m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_nGridVertexNum/2);
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	}
}
*/
void CWorld::RenderAxis()
{
	return;
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
/*
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,  0xffff0000 );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
*/


	D3DXMATRIX  matView;
	//D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 1.0f );
	//D3DXVECTOR3 vecPos(  0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vecLookAt( 0.0f, -.5f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, -2.0f, -3.5f );

	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,0.0f,1.0f) );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXVECTOR3 vPos( 0, 0, 0 ); // = pObj->GetPos();
// = pObj->GetPos();
	//D3DXVECTOR3 vScale;// = pObj->GetScale();



	D3DXVECTOR3	vDir = m_pCamera->m_vLookAt - m_pCamera->m_vPos;
	D3DXVec3Normalize( &vDir, &vDir );


	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixTranslation( &mat, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	D3DXMatrixRotationAxis( &mat, &vDir, 0.5f);
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
/*
	// 화살표의 위치, 회전값을 결정한다.
	//D3DXMatrixLookAtLH( &matWorld, &vDest, &m_pCamera->m_vPos, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	D3DXMatrixLookAtLH( &matWorld, &m_pCamera->m_vLookAt, &m_pCamera->m_vPos, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	D3DXMatrixInverse ( &matWorld, NULL, &matWorld );
	//matWorld._41 = g_pPlayer->GetPos().x; matWorld._42 = g_pPlayer->GetPos().y + 2.0f; matWorld._43 = g_pPlayer->GetPos().z;
	//matWorld._41 = m_pCamera->m_vPos.x; matWorld._42 = m_pCamera->m_vPos.y; matWorld._43 = m_pCamera->m_vPos.z - 10;;
	matWorld._41 = 0; matWorld._42 = 0; matWorld._43 = 0;
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
*/

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_pCamera->m_matInvView );
	
	//FLOAT fTheta ;//= D3DXToRadian( -pObj->GetAngle() );
	//D3DXMatrixRotationY( &mat, fTheta );
	//D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
	//D3DXMatrixTranslation(&mat,vPos.x,vPos.y,vPos.z);
	//D3DXMatrixMultiply(&matWorld,&matWorld,&mat);

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetTexture( 0, NULL);
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_AXISVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pAxisVertexBuffer, 0, sizeof(AXISVERTEX) );
	m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_nAxisVertexNum / 2 );

	m_pCamera->Transform( m_pd3dDevice, this );
}
void CWorld::RenderGizmo()
{
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	m_vCenter = GetSelectCenter();

	// 기즈모 출력 
	D3DXVECTOR3 vPos = m_vCenter;
	D3DXVECTOR3 vInverval;
	D3DXMATRIX matRot;
	D3DXMATRIX matScale;
	D3DXMATRIX matTrans; 
	D3DXMATRIX matWorld;
	vInverval.x = m_pCamera->m_vPos.x - vPos.x;
	vInverval.y = m_pCamera->m_vPos.y - vPos.y;
	vInverval.z = m_pCamera->m_vPos.z - vPos.z;
 
	double dRange = sqrt( vInverval.x * vInverval.x + vInverval.y * vInverval.y + vInverval.z * vInverval.z ) / 15.0f;

	D3DXMatrixScaling( &matScale, dRange, dRange, dRange );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	m_meshGizmo.Render( m_pd3dDevice, &matWorld );

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationX( &matRot, D3DXToRadian( -90 ) );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matRot );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	m_meshGizmo.Render( m_pd3dDevice, &matWorld );

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationZ( &matRot, D3DXToRadian( 90 ) );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matRot );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	m_meshGizmo.Render( m_pd3dDevice, &matWorld );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}
void CWorld::SetSelectGridPos()
{
	for( ObjItor i = m_arySelect.begin(); i != m_arySelect.end(); i++ )
	{
		CObj* pObj = *i;
		pObj->m_vPosGrid = pObj->GetPos();
	}
}		
void CWorld::SetSelectObj( CObj* pObj, BOOL bSelect )
{
	if( IsValidSelectObj( pObj ) == FALSE )
		return;
	for( ObjItor i = m_arySelect.begin(); i != m_arySelect.end(); i++ )
	{
		if( *i == pObj )
			break;
	}
	if( bSelect == TRUE )
	{
		if( i != m_arySelect.end() )
			return;
		if( pObj->GetType() == OT_REGION )
			((CRegion*)pObj)->m_bEditable = TRUE;
		m_arySelect.push_back( pObj );
		pObj->m_vPosGrid = pObj->GetPos();
	}
	else
	{
		if( i == m_arySelect.end() )
			return;
		m_arySelect.erase( i );
		if( pObj->GetType() == OT_REGION )
			((CRegion*)pObj)->m_bEditable = FALSE;
	}
	int a = m_arySelect.max_size();
	int b = m_arySelect.capacity();
	int c = m_arySelect.size();

} 
BOOL CWorld::IsValidSelectObj( CObj* pObj )
{
	if( m_bViewAllObjects == FALSE )
		return FALSE;
	if( !pObj->IsVisible() )
		return FALSE;
	switch( pObj->GetType() )
	{
	case OT_OBJ:
		if( m_bViewObj == FALSE )
			return FALSE;
		break;
	case OT_ITEM:
		if( m_bViewItem == FALSE )
			return FALSE;
	case OT_SFX:
		break;
	case OT_MOVER:
		if( m_bViewMover == FALSE )
			return FALSE;
		if( ((CMover*)pObj)->IsRespawn() && m_bViewSpawn == FALSE )
			return FALSE;
		break;
	case OT_REGION:
		break;
	}
	return TRUE;
}
void CWorld::SetSelectObj( CRect rect )
{
	int i, j, k, l, x, y;
	WorldPosToLand( m_pCamera->m_vPos, x, y );
	CLandscape* pLand;
	for( i = y - m_nVisibilityLand; i <= y + m_nVisibilityLand; i++ )
	{
		for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++ )
		{
			if( LandInWorld( j, i ) && m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
				if( pLand && pLand->isVisibile() )
				{
					for( k = 0; k < MAX_OBJARRAY; k++ )
					{
						
						CObj** apObject = pLand->m_apObject[ k ];
						for( l = 0; l < pLand->m_adwObjNum[ k ]; l++ )
						{
							CObj* pObj = apObject[ l ];
							if( pObj && pObj->IsDelete() == FALSE && pObj->GetWorld() != NULL && IsValidSelectObj( pObj ) )
							{
								CPoint point = pObj->GetClientPoint( m_pd3dDevice );
								if( rect.PtInRect( point ) )
									SetSelectObj( pObj, TRUE );
							}
						}
					}
				}
			}
		}
	}
}
void CWorld::MoveSelectObj( D3DXVECTOR3 vAdd )
{
}
void CWorld::UnselectObj()
{
	for( int i = 0; i < m_arySelect.size(); i++ )
	{
		CObj* pObj = (CObj*)m_arySelect.at( i );	
		if( pObj->GetType() == OT_REGION )
			((CRegion*)pObj)->m_bEditable = FALSE;
	}
	m_arySelect.clear();
}
BOOL CWorld::IsSelectObj( CObj* pObj )
{
	for( int i = 0; i < m_arySelect.size(); i++)
	{
		if( m_arySelect.at( i ) == pObj )
			return TRUE;
	}
	return FALSE;
}
void CWorld::DeleteSelectObj()
{
	for( int i = 0; i < m_arySelect.size(); i++)
	{
		((CObj*)m_arySelect.at( i ))->Delete();
	}
	m_arySelect.clear();

	// 오브젝트 Delete ( DeleteObj가 호출된 오브젝트들)
	CObj* pObj;
	for( i = 0; i < m_nDeleteObjs; i++ )
	{
		pObj = m_apDeleteObjs[ i ];
		if( m_pObjFocus == pObj )
			m_pObjFocus = 0;
//#ifdef __NEWPROCESS
		// 화면에 출력되고 있는 오브젝트인가.
		if( pObj->m_ppViewPtr )
		{
			// 그렇다면 화면 출력 배열에서 자신을 삭제 
			*pObj->m_ppViewPtr = NULL;
			pObj->m_ppViewPtr = NULL;
		}
//#endif
		RemoveObjLink( pObj );
		RemoveObjArray( pObj );
		SAFE_DELETE( pObj );
	}
	m_nDeleteObjs = 0;
}
void CWorld::InvisibleSelectObj()
{
	for( int i = 0; i < m_arySelect.size(); i++)
	{
		((CObj*)m_arySelect.at( i ))->SetVisible( FALSE );
	}
}
void CWorld::InvisibleUpstairsObj( FLOAT fY )
{
	CObj* pObj;
	for( int i = 0; i < m_nObjCullSize; i++ )
	{
		pObj = m_aobjCull[ i ];
		if( pObj )
		{
			if( pObj->GetPos().y >= fY )
				pObj->SetVisible( FALSE );
		}
	}
}
void CWorld::ShowAllObjects()
{
	int i, j, k, l, y, x;
	WorldPosToLand( m_pCamera->m_vPos, x, y );
	for( i = y - m_nVisibilityLand; i <= y + m_nVisibilityLand; i++ )
	{
		for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++ )
		{
			if( LandInWorld( j, i ) && m_apLand[ i * m_nLandWidth + j ] )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j ];
				for( k = 0; k < MAX_OBJARRAY; k++ )
				{
					CObj** apObject = pLand->m_apObject[ k ];
					int nNumObj = pLand->m_adwObjNum[ k ];
					for( l = 0; l < nNumObj; l++ )
					{
						if( (*apObject) ) 
							(*apObject)->SetVisible( TRUE );
						apObject ++;
					}
				}
			}
		}
	}
}
void CWorld::CopySelectObj()
{
	// 선택된 오브젝트를 모두 복사하고, 현재 셀렉트를 지운다.
	CPtrArray arySelect;
	for( int i = 0; i < m_arySelect.size(); i++)
	{
		CObj* pObj = (CObj*)m_arySelect.at( i );
		CObj* pNewObj = ::CreateObj( m_pd3dDevice, pObj->m_dwType, pObj->m_dwIndex );
		pNewObj->SetPos( pObj->GetPos() );
		pNewObj->SetAngle( pObj->GetAngle() );
		pNewObj->SetScale( pObj->GetScale() );
		AddObj( pNewObj );
		arySelect.Add( pNewObj );
	}
	UnselectObj();
	for( i = 0; i < arySelect.GetSize(); i++)
	{
		CObj* pObj = (CObj*)arySelect.GetAt( i );
		SetSelectObj( pObj, TRUE );
	}
}
D3DXVECTOR3 CWorld::GetSelectCenter() 
{
	// 중심축 구하기 
	D3DXVECTOR3 v1 = D3DXVECTOR3( 9999999.0f, 9999999.0f, 9999999.0f);
	D3DXVECTOR3 v2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);
	for( int i = 0; i < m_arySelect.size(); i++ )
	{
		CObj* pObj = (CObj*)m_arySelect.at( i );
		//if( pObj && pObj->m_bSelect )
		{
			if( pObj->GetPos().x < v1.x )
				v1.x = pObj->GetPos().x;
			if( pObj->GetPos().y < v1.y )
				v1.y = pObj->GetPos().y;
			if( pObj->GetPos().z < v1.z )
				v1.z = pObj->GetPos().z;
			if( pObj->GetPos().x > v2.x )
				v2.x = pObj->GetPos().x;
			if( pObj->GetPos().y > v2.y )
				v2.y = pObj->GetPos().y;
			if( pObj->GetPos().z > v2.z )
				v2.z = pObj->GetPos().z;
		}
	}
	FLOAT fx = v2.x - v1.x;
	FLOAT fy = v2.y - v1.y;
	FLOAT fz = v2.z - v1.z;
	D3DXVECTOR3 vCenter;
	vCenter.x = v1.x + ( fx / 2.0f );
	vCenter.y = v1.y + ( fy / 2.0f );
	vCenter.z = v1.z + ( fz / 2.0f );
	return vCenter;
}
/*
HRESULT CWorld::BuildGridVertex(LPDIRECT3DDEVICE9 pd3dDevice)
{
 
	HRESULT hr;
	GRIDVERTEX* pVertices;
	m_nGridVertexNum = (m_nLandWidth + 1) * 2 + (m_nLandHeight + 1) * 2;
	// 버텍스 버퍼 만들기 
	hr = pd3dDevice->CreateVertexBuffer( m_nGridVertexNum * sizeof(GRIDVERTEX),
										 D3DUSAGE_WRITEONLY, D3DFVF_GRIDVERTEX,
										 D3DPOOL_MANAGED, &m_pGridVertexBuffer 
										 );
	if( FAILED(hr) ) return hr;
	hr = m_pGridVertexBuffer->Lock( 0, m_nGridVertexNum*sizeof(GRIDVERTEX),(BYTE**)&pVertices, 0 );
	if( FAILED(hr) ) return hr;

	// 버텍스 생성 
	for(int i = 0; i < m_nLandHeight + 1; i++ ) // 수평 라인 버틱스 생성 
	{
		pVertices->p.x = 0;
		pVertices->p.y = 0;
		pVertices->p.z = (FLOAT)i * MAP_SIZE * MPU;
		//pVertices->n.x = 0;
		//pVertices->n.y = 1;
		//pVertices->n.z = 0;
		pVertices->color = D3DCOLOR_ARGB( 255, 255, 255, 0 );
		pVertices++;

		pVertices->p.x = m_nLandWidth * MAP_SIZE * MPU;
		pVertices->p.y = 0;
		pVertices->p.z = (FLOAT)i * MAP_SIZE * MPU;
		//pVertices->n.x = 0;
		//pVertices->n.y = 1;
		//pVertices->n.z = 0;
		pVertices->color = D3DCOLOR_ARGB( 255, 255, 255, 0 );
		pVertices++;

	}
	for(i = 0; i < m_nLandWidth + 1; i++ ) // 수직 라인 버틱스 생성 
	{
		pVertices->p.x = (FLOAT)i * MAP_SIZE * MPU;
		pVertices->p.y = 0;
		pVertices->p.z = 0;
		//pVertices->n.x = 0;
		//pVertices->n.y = 1;
		//pVertices->n.z = 0;
		pVertices->color = D3DCOLOR_ARGB( 255, 255, 255, 0 );
		pVertices++;

		pVertices->p.x = i * MAP_SIZE * MPU;
		pVertices->p.y = 0;
		pVertices->p.z = (FLOAT)m_nLandHeight * MAP_SIZE * MPU;
		//pVertices->n.x = 0;
		//pVertices->n.y = 1;
		//pVertices->n.z = 0;
		pVertices->color = D3DCOLOR_ARGB( 255, 255, 255, 0 );
		pVertices++;
	}
	m_pGridVertexBuffer->Unlock();

	return hr;
}
*/
void CWorld::RenderCircleVertex( CObj* pObj )
{
	/*
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vPos = pObj->GetPos();
	D3DXVECTOR3 vScale = pObj->GetScale();

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&mat,vScale.x,vScale.y,vScale.z);
	D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
	FLOAT fTheta = D3DXToRadian( pObj->GetAngle() );//m_fAngle
	D3DXMatrixRotationY( &mat, fTheta );
	D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
	D3DXMatrixTranslation(&mat,vPos.x,vPos.y,vPos.z);
	D3DXMatrixMultiply(&matWorld,&matWorld,&mat);

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetVertexShader( D3DFVF_BOUNDBOXVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pCircleVertexBuffer, sizeof(BOUNDBOXVERTEX) );
	m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_nCircleVertexNum / 2 );
	*/
}
void CWorld::SetCircleVertex( CObj* pObj )
{/*
	const BOUND_BOX* pBB;
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vCircle;
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1
	//
	//   7 | 6
	// - --+-- + x
	//   4 | 5
	BOUNDBOXVERTEX* pVertices;
	m_pCircleVertexBuffer->Lock( 0, m_nCircleVertexNum * sizeof(BOUNDBOXVERTEX), (BYTE**)&pVertices, 0 );

	// 0
	vCircle = pBB->m_vPos[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 1 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 1
	vCircle = pBB->m_vPos[ 1 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 2 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 2
	vCircle = pBB->m_vPos[ 2 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 3 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 3
	vCircle = pBB->m_vPos[ 3 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 0 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;

	// 4
	vCircle = pBB->m_vPos[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 4 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 5
	vCircle = pBB->m_vPos[ 1 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 5 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 6
	vCircle = pBB->m_vPos[ 3 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 7 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 7
	vCircle = pBB->m_vPos[ 2 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 6 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;

	// 8
	vCircle = pBB->m_vPos[ 4 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 5 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 9
	vCircle = pBB->m_vPos[ 5 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 6 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 10
	vCircle = pBB->m_vPos[ 6 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 7 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	// 11
	vCircle = pBB->m_vPos[ 7 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;
	vCircle = pBB->m_vPos[ 4 ];//avCircle[ 0 ];
	pVertices->p = vCircle                         ; pVertices->color = 0xffffffff;	pVertices++;

	m_pCircleVertexBuffer->Unlock();
	*/
}
void CWorld::SetHeight(int x,int z,FLOAT factor)
{
	if( VecInWorld( x, z ) == FALSE )
		return;
	x/=MPU;z/=MPU;
	int rX=x;//-m_nWorldX;
	int rZ=z;//-m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);
	if( m_apLand[mX + mZ * m_nLandWidth] == NULL ) 
		return;

	m_apLand[mX + mZ * m_nLandWidth]->SetHeight(x-(mX*MAP_SIZE),z-(mZ*MAP_SIZE),factor);
	if(x%MAP_SIZE==0 && x>0) {
		m_apLand[mX-1 + mZ * m_nLandWidth]->SetHeight(MAP_SIZE,z-(mZ*MAP_SIZE),factor);
		if(z%MAP_SIZE==0 && z>0) {
			m_apLand[mX-1 + (mZ-1) * m_nLandWidth]->SetHeight(MAP_SIZE,MAP_SIZE,factor);
		}
	}
	if(z%MAP_SIZE==0 && z>0) {
		m_apLand[mX + (mZ-1) * m_nLandWidth]->SetHeight(x-(mX*MAP_SIZE),MAP_SIZE,factor);
	}
}
void CWorld::SetHeight(POINT pos,FLOAT factor)
{
	SetHeight(pos.x,pos.y,factor);
}
/*
DWORD CWorld::ObjTypeToObjFilter( DWORD dwType )
{
	static DWORD filter[] = 
	{ 
		OF_OBJ  , //OT_OBJ  
		OF_ANIM , //OT_ANIM 
		OF_CTRL , //OT_CTRL 
		OF_SFX  , //OT_SFX  
		OF_ITEM , //OT_ITEM 
		OF_MOVER  //OT_MOVER
	};
	return filter[dwType];
}

static DWORD m_dwFilter[] = { OF_OBJ, OF_ANIM, OF_CTRL, OF_SFX, OF_ITEM, OF_MOVER }

inline ObjTypeToObjFilter( DWORD dwType ) 
{  
	static DWORD m_dwFilter[] = { OF_OBJ, OF_ANIM, OF_CTRL, OF_SFX, OF_ITEM, OF_MOVER };
	return m_dwFilter[ dwType ] 
}
*/

void CWorld::SetAllObjOnLand()
{
	//CObj* pObj;
	for( int iLand = 0; iLand < m_nLandWidth * m_nLandHeight; iLand++ )
	{
		CLandscape* pLand = m_apLand[ iLand ];
		if( pLand ) //&& pLand->isVisibile() )
		{
			for( int i = 0; i < MAX_OBJARRAY; i++)
			{
				CObj** apObjs = pLand->m_apObject[ i ];
				for( int j = 0; j < int( pLand->m_adwObjNum[ i ] ); j++ )
				{
					if( apObjs[ j ] )
						apObjs[ j ]->SetOnLand();
				}  
			}
		}
	}
}
/*
int CWorld::GetLinearObjSize()
{
	for( int iLand = 0, i2 = 0; iLand < m_nLandWidth * m_nLandHeight; iLand++ )
	{
		CLandscape* pLand = m_apLand[ iLand ];
		if( pLand ) //&& pLand->IsModified() )
		{
			for(int i = 0; i < MAX_OBJARRAY; i++ )
			{
				i2 += pLand->m_dwObjNum;
			}
		}
	}
	return i2;
}
CObj* CWorld::GetLinearObj( int nIndex )
{
	int nSum = 0;
	int nOldSum = 0;
	for( int iLand = 0; iLand < m_nLandWidth * m_nLandHeight; iLand++ )
	{
		CLandscape* pLand = m_apLand[ iLand ];
		if( pLand )
		{
			nSum += pLand->GetObjArraySize();
			if( nIndex < nSum )
			{
				nIndex -= nOldSum;
				for(int i = 0, i2 = 0; i < MAX_OBJARRAY; i++, i2 += pLand->m_objNum[ i - 1 ] )
				{
					if( nIndex < i2 + pLand->m_objNum[ i ] )
					{
						CObj** apObj = pLand->m_apObject[ i ];
						CObj* pObj = apObj[ nIndex - i2 ] ;
						if( pObj && pObj->IsDelete() ) return NULL;
						return pObj;
					}
				}
			}
			nOldSum = nSum;
		}
	}
	return NULL;
}
*/
/*
struct tagLINEAROBJPOS
{
	int iLand;
	int iObjArray;

	tagLINEAROBJPOS()
	{
		iLand = 0;

	}
} LINEAROBJPOS,* LPLINEAROBJPOS;

CObj* CWorld::GetLinearNextObj( LPLINEAROBJPOS lpLinearObjPos )
{
	int i2 = 0;
	for( ; lpLinearObjPos->iLand < m_nLandWidth * m_nLandHeight; lpLinearObjPos->iLand++ )
	{
		CLandscape* pLand = m_apLand[ lpLinearObjPos->iLand ];
		if( pLand ) //&& pLand->IsModified() )
		{
			for(; lpLinearObjPos->iObjArray < MAX_OBJARRAY; i++, i2 += pLand->m_objNum[ i - 1 ] )
			{
				if( nIndex < i2 + pLand->m_objNum[ lpLinearObjPos->iObjArray ] )
				{
					CObj** apObj = pLand->m_apObject[ lpLinearObjPos->iObjArray ];
					CObj* pObj = apObj[ nIndex - i2 ] ;
					if( pObj && pObj->IsDestroy() ) return NULL;
					return pObj;
				}
			}
		}
		lpLinearObjPos->iObjArray = 0;
	}
	return NULL;
}
*/
void CWorld::SetBlankPatch(int x,int z)
{
	if( VecInWorld( x, z ) == FALSE )
		return;
	x/=MPU;z/=MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return;
	float rX = x;// - m_nWorldX;
	float rZ = z;// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);

	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return;
	int tx=(x%MAP_SIZE)/PATCH_SIZE;
	int tz=(z%MAP_SIZE)/PATCH_SIZE;

	m_apLand[ mX + mZ * m_nLandWidth ]->SetBlankPatch(tx,tz);
	m_apLand[ mX + mZ * m_nLandWidth ]->SetDirtyPatch(tx*PATCH_SIZE,tz*PATCH_SIZE);
	m_apLand[ mX + mZ * m_nLandWidth ]->CalculateBound();
}
void CWorld::SetLandMarkPatch(int x,int z,WORD nTex)
{
	if( VecInWorld( x, z ) == FALSE )
		return;
	x/=MPU;z/=MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return;
	float rX = x;// - m_nWorldX;
	float rZ = z;// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);

	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return;
	if( prj.m_terrainMng.GetTerrain( nTex )->m_pTexture == NULL )
		prj.m_terrainMng.LoadTexture( nTex );
	int tx=(x%MAP_SIZE)/PATCH_SIZE;
	int tz=(z%MAP_SIZE)/PATCH_SIZE;

	m_apLand[ mX + mZ * m_nLandWidth ]->SetLandMark(tx,tz,nTex,TRUE);
	m_apLand[ mX + mZ * m_nLandWidth ]->CalculateBound();
}
void CWorld::SetDetailPatch(int x,int z,WORD nTex)
{
	if( VecInWorld( x, z ) == FALSE )
		return;
	x/=MPU;z/=MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return;
	float rX = x;// - m_nWorldX;
	float rZ = z;// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);

	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return;
	int tx=(x%MAP_SIZE)/PATCH_SIZE;
	int tz=(z%MAP_SIZE)/PATCH_SIZE;
	int gx=(x%MAP_SIZE)-tx*PATCH_SIZE;
	int gz=(z%MAP_SIZE)-tz*PATCH_SIZE;
	if( prj.m_terrainMng.GetTerrain( nTex )->m_pTexture == NULL )
		prj.m_terrainMng.LoadTexture( nTex );
//	m_apLand[ mX + mZ * m_nLandWidth ]->m_aPatches[tz][tx].SetExPatch(TRUE);
//	m_apLand[ mX + mZ * m_nLandWidth ]->m_aPatches[tz][tx].SetTexture(gx,gz,nTex);
	m_apLand[ mX + mZ * m_nLandWidth ]->SetDirtyPatch(tx*PATCH_SIZE,tz*PATCH_SIZE);
	m_apLand[ mX + mZ * m_nLandWidth ]->CalculateBound();
}
int CWorld::GetTexture( int x,int z )
{
	if( VecInWorld( x, z ) == FALSE )
		return - 1;
	x/=MPU;z/=MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return -1;
	float rX = x;// - m_nWorldX;
	float rZ = z;// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);

	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return -1;

	return m_apLand[ mX + mZ * m_nLandWidth ]->GetTex( x % MAP_SIZE, z % MAP_SIZE);

}
void CWorld::SetWorldTex( int lx,int lz,int gx,int gz,WORD nTex, BYTE nOpacity,BOOL bLandMark)
{
	if ( lx < 0 || lx >= m_nLandWidth || lz < 0 || lz >= m_nLandHeight ) 
		return;
	m_apLand[lx+lz*m_nLandWidth]->SetTex(gx,gz,nTex,nOpacity,bLandMark);
	//m_apLand[lx+lz*m_nLandWidth]->OptimizeLayer( gx,gz );
}
void CWorld::SetTex( int x,int z,WORD nTex, BYTE nOpacity,BOOL bLandMark)
{
	int lx = x / LIGHTMAP_SIZE;
	int lz = z / LIGHTMAP_SIZE;
	int gx = x - lx * LIGHTMAP_SIZE;
	int gz = z - lz * LIGHTMAP_SIZE;
	SetWorldTex( lx, lz, gx, gz, nTex, nOpacity );
		
	if( gx == 0 ) 
	{
		SetWorldTex( lx - 1, lz, LIGHTMAP_SIZE, gz, nTex, nOpacity );
		if(gz==0) 
		{
			SetWorldTex( lx - 1, lz - 1, LIGHTMAP_SIZE, LIGHTMAP_SIZE, nTex, nOpacity, bLandMark );
		}
	}
	if( gz == 0 ) 
	{
		SetWorldTex( lx, lz - 1, gx, LIGHTMAP_SIZE, nTex, nOpacity, bLandMark );
	}
}
/*
void CWorld::SetTexture( FLOAT x,FLOAT z,WORD nTex, FLOAT fRadius, FLOAT fHardness, BOOL bLandMark )
{
	if( VecInWorld( x, z ) == FALSE )
		return;


	x/=MPU;z/=MPU;
	x+=.5f;
	z+=.5f;
	x=x*(float)LIGHTMAP_SIZE/MAP_SIZE;
	z=z*(float)LIGHTMAP_SIZE/MAP_SIZE;

	//fRadius--;
	//fHardness--;

	FLOAT insideradius  = fHardness;
	FLOAT outsideradius = fRadius - fHardness;

	{
		fRadius--;
		fHardness--;
		for(int i =- fRadius; i <= fRadius ; i++ )  
		{
			for(int j =- fRadius; j <= fRadius; j++ ) 
			{
				FLOAT distance = sqrt(( i * i ) + ( j * j ));
				if( distance <= fRadius ) 
				{
					if( distance <= insideradius ) 
					{
						SetTex( x + j, z + i, nTex,255,bLandMark );
					}
					else 
					{
						FLOAT tempdistance = distance - insideradius;
						int alpha = 255- int( 255.0f * tempdistance / outsideradius );
						SetTex( x + j, z + i, nTex, alpha,bLandMark);
					}
				}
			}
		}
	}

	for( int i = 0; i < m_nLandWidth * m_nLandHeight; i++ ) 
		if(m_apLand[i]) m_apLand[i]->CalculateBound();
}
*/
void CWorld::SetWorldCol( int lx,int lz,int gx,int gz,DWORD color)
{
	if (lx<0 || lx>=m_nLandWidth || lz<0 || lz>=m_nLandHeight) return;
	m_apLand[lx+lz*m_nLandWidth]->SetCol(gx,gz,color);
}
void CWorld::SetCol( int x,int z,DWORD color)
{
	int lx=x/LIGHTMAP_SIZE;
	int lz=z/LIGHTMAP_SIZE;
	int gx=x-lx*LIGHTMAP_SIZE;
	int gz=z-lz*LIGHTMAP_SIZE;
	SetWorldCol(lx,lz,gx,gz,color);
	if(gx==0) 
	{
		SetWorldCol(lx-1,lz,LIGHTMAP_SIZE,gz,color);
		if(gz==0) 
		{
			SetWorldCol(lx-1,lz-1,LIGHTMAP_SIZE,LIGHTMAP_SIZE,color);
		}
	}
	if(gz==0) 
	{
		SetWorldCol(lx,lz-1,gx,LIGHTMAP_SIZE,color);
	}
}
DWORD CWorld::GetCol( int x,int z )
{
	int lx=x/LIGHTMAP_SIZE;
	int lz=z/LIGHTMAP_SIZE;
	int gx=x-lx*LIGHTMAP_SIZE;
	int gz=z-lz*LIGHTMAP_SIZE;
	if (lx<0 || lx>=m_nLandWidth || lz<0 || lz>=m_nLandHeight) return 0;
	return m_apLand[lx+lz*m_nLandWidth]->GetCol(gx,gz);
	//return GetWorldCol(lx,lz,gx,gz,color);
}
void CWorld::SetColor(FLOAT x,FLOAT z,DWORD color, FLOAT fRadius, FLOAT fHardness)
{
	if( VecInWorld( x, z ) == FALSE )
		return;

//	fHardness = 2.0f;

	x/=MPU;z/=MPU;
	x+=.5f;
	z+=.5f;
	x=x*(float)LIGHTMAP_SIZE/MAP_SIZE;
	z=z*(float)LIGHTMAP_SIZE/MAP_SIZE;

	FLOAT insideradius  = fHardness;
	FLOAT outsideradius = fRadius - fHardness;

//	SetCol( x , z, color );

	for(int i =- fRadius; i < fRadius + 1; i++ )  
	{
		for(int j =- fRadius; j < fRadius + 1; j++ ) 
		{
			FLOAT distance = sqrt(( i * i ) + ( j * j ));
			if( distance <= fRadius ) 
			{
				if( distance < insideradius ) 
				{
					SetCol( x + j, z + i, color );
				}
				else
				{
					//distance = 3;
					FLOAT tempdistance = distance - insideradius;
					//DWORD dwAlpha = 0;//255- int( 255.0f * tempdistance / outsideradius );
					//dwAlpha <<= 24;
					int dwRed   = ( ( color & 0x00ff0000 ) >> 16 ) - 128;
					int dwGreen = ( ( color & 0x0000ff00 ) >>  8 ) - 128;
					int dwBlue  = ( ( color & 0x000000ff ) >>  0 ) - 128;

					// 원래 칼라를 얻는다.
					DWORD dwSrcColor = GetCol( x + j, z + i );
					int dwSrcRed   = ( ( dwSrcColor & 0x00ff0000 ) >> 16 ) - 128;
					int dwSrcGreen = ( ( dwSrcColor & 0x0000ff00 ) >>  8 ) - 128;
					int dwSrcBlue  = ( ( dwSrcColor & 0x000000ff ) >>  0 ) - 128;

					// outsideradius : dwColor = ( outsideradius - tempdistance ) : x
					// 입력 칼라와 원래 칼라를 합성 
					dwRed   = int( ( outsideradius - tempdistance ) * ( dwRed   - dwSrcRed   ) / outsideradius );
					dwGreen = int( ( outsideradius - tempdistance ) * ( dwGreen - dwSrcGreen ) / outsideradius );
					dwBlue  = int( ( outsideradius - tempdistance ) * ( dwBlue  - dwSrcBlue  ) / outsideradius );

					dwRed   += dwSrcRed  ;
					dwGreen += dwSrcGreen;
					dwBlue  += dwSrcBlue ;

					SetCol( x + j, z + i, D3DCOLOR_ARGB( 0, dwRed + 128, dwGreen + 128, dwBlue + 128 ) );
				}
			}
		}
	}
	for( i = 0; i < m_nLandWidth * m_nLandHeight; i++ ) 
		if(m_apLand[i]) m_apLand[i]->CalculateBound();
}
void CWorld::SetWaterHeight(int x, int z, DWORD dwWaterHeight, DWORD dwWaterTexture )
{
	if( VecInWorld( x, z ) == FALSE )
		return;
	x /= MPU;
	z /= MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return;
	float rX = x;// - m_nWorldX;
	float rZ = z;// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);
	
	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return;

	int tx = ( x % MAP_SIZE ) / PATCH_SIZE;
	int tz = ( z % MAP_SIZE ) / PATCH_SIZE;
	
	m_apLand[ mX + mZ * m_nLandWidth ]->SetWaterHeight( tx, tz, dwWaterHeight, dwWaterTexture );
}
void CWorld::SetHeightAttribute( int x, int z, DWORD dwAttr, FLOAT fRadius )
{
	if( VecInWorld( x, z ) == FALSE )
		return;
	x = int( x ) / MPU * MPU;
	z = int( z ) / MPU * MPU;
	BOOL* lpLand = new BOOL[ m_nLandWidth * m_nLandHeight ];
	ZeroMemory( lpLand, sizeof( BOOL) * m_nLandWidth * m_nLandHeight );
	for( int yc = -fRadius; yc < fRadius; yc += MPU ) 
	{
		for( int xc = -fRadius; xc < fRadius; xc += MPU )
		{
			FLOAT r = fabs( sqrt( xc * xc + yc * yc ) );
			if( r < fRadius && !( xc % MPU) && !(yc % MPU ))
			{
				int xx = x + xc;
				int yy = z + yc;
				/////////////////////
				xx /= MPU;
				yy /= MPU;
				int rX = xx;
				int rZ = yy;
				int mX=int( rX / MAP_SIZE);
				int mZ=int( rZ / MAP_SIZE);
				if( m_apLand[mX + mZ * m_nLandWidth] == NULL ) 
					return;
				m_apLand[ mX + mZ * m_nLandWidth ]->SetHeightAttribute( xx-(mX*MAP_SIZE), yy-(mZ*MAP_SIZE), dwAttr );
				lpLand[ mX + mZ * m_nLandWidth ] = TRUE;
				//m_world.SetHeightAttribute( xx, yy, nAttr );
			}
		}
	}
	for( int i = 0; i < m_nLandHeight; i++ )
	{
		for( int j = 0; j < m_nLandWidth; j++ )
		{
			if( lpLand[ j + i * m_nLandWidth] )
			{
				m_apLand[ j + i * m_nLandWidth ]->MakeHgtAttrVertexBuffer();
			}
		}
	}
	delete lpLand;
	/*
	x /= MPU;
	z /= MPU;
	int rX = x;
	int rZ = z;
	int mX=int( rX / MAP_SIZE);
	int mZ=int( rZ / MAP_SIZE);
	if( m_apLand[mX + mZ * m_nLandWidth] == NULL ) 
		return;
	m_apLand[ mX + mZ * m_nLandWidth ]->SetHeightAttribute( x-(mX*MAP_SIZE), z-(mZ*MAP_SIZE), dwAttr );
	*/
}
/*
HRESULT CWorld::SetHeightAttribute( int x, int z, DWORD dwAttr )
{
	MakeHgtAttrVertexBuffer()
		
	return S_OK;
}
*/
/*
void CWorld::RenderGrids( CRect rect,WORD dx,DWORD color)
{
	rect.NormalizeRect();
	int startx=rect.left  /MPU;
	int starty=rect.top   /MPU;
	int endx  =rect.right /MPU;
	int endy  =rect.bottom/MPU;

	//if(startx>endx) swap(startx,endx);
	//if(starty>endy) swap(starty,endy);
	if(startx<0) startx=0;
	if(starty<0) starty=0;
	if(endx>m_nLandWidth*MAP_SIZE) endx=m_nLandWidth*MAP_SIZE;
	if(endy>m_nLandHeight*MAP_SIZE) endy=m_nLandHeight*MAP_SIZE;


	int startworldx=startx/MAP_SIZE;
	int startworldy=starty/MAP_SIZE;
	int endworldx  =endx  /MAP_SIZE;
	int endworldy  =endy  /MAP_SIZE;

	if(startworldx<0) startworldx=0;
	if(startworldy<0) startworldy=0;
	if(endworldx>=m_nLandWidth) endworldx=m_nLandWidth-1;
	if(endworldy>=m_nLandHeight) endworldy=m_nLandHeight-1;

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
//	m_pd3dDevice->SetRenderState( D3DRS_ZBIAS ,1);

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	D3DXMATRIX matWorld,matTemp;
//	D3DXMatrixIdentity( &matWorld );
//	matWorld._42=0.05;
//	m_pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp);
//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);

	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,  color );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_D3DLANDSCAPEVERTEX );

	for(int wy=startworldy;wy<endworldy+1;wy++) 
	{
		for(int wx=startworldx;wx<endworldx+1;wx++) 
		{
			RenderWorldGrids(wx,wy,CPoint(startx,starty),CPoint(endx,endy),dx,color);
		}
	}
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	//m_pd3dDevice->SetRenderState( D3DRS_ZBIAS ,0);
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
}

void CWorld::RenderWorldGrids(int wx,int wy,CPoint ptLT,CPoint ptRB,WORD dx,DWORD color)
{
	CLandscape* pLand=GetLandscape(wx,wy);
	if(!pLand) return;
	if(!pLand->isVisibile()) return;
	int startx=ptLT.x-wx*MAP_SIZE;
	int starty=ptLT.y-wy*MAP_SIZE;
	int endx  =ptRB.x-wx*MAP_SIZE;
	int endy  =ptRB.y-wy*MAP_SIZE;

	if(startx<0) startx=0;
	if(starty<0) starty=0;
	if(endx>=MAP_SIZE) endx=MAP_SIZE-1;
	if(endy>=MAP_SIZE) endy=MAP_SIZE-1;

	int startpatchx=startx/PATCH_SIZE;
	int startpatchy=starty/PATCH_SIZE;
	int endpatchx  =endx  /PATCH_SIZE;
	int endpatchy  =endy  /PATCH_SIZE;

	WORD* pIB;
	int nCount=0;
	g_pGridIB->Lock( 0, 0, (void**)&pIB, 0 );
	for(int py=startpatchy;py<=endpatchy;py++) {
		for(int px=startpatchx;px<=endpatchx;px++) {
			if(!pLand->m_aPatches[py][px].isVisibile()) continue;

			int indexoffset= (px+py*NUM_PATCHES_PER_SIDE)*(PATCH_SIZE+1)*(PATCH_SIZE+1);

			int startgridx=startx-px*PATCH_SIZE;
			int startgridy=starty-py*PATCH_SIZE;
			int endgridx  =endx  -px*PATCH_SIZE;
			int endgridy  =endy  -py*PATCH_SIZE;

			if(startgridx<0) startgridx=0;
			if(startgridy<0) startgridy=0;
			if(endgridx>=PATCH_SIZE) endgridx=PATCH_SIZE-1;
			if(endgridy>=PATCH_SIZE) endgridy=PATCH_SIZE-1;

			if(endgridx+px*PATCH_SIZE+wx*MAP_SIZE==ptRB.x) endgridx--;
			if(endgridy+py*PATCH_SIZE+wy*MAP_SIZE==ptRB.y) endgridy--;

			if(dx==0) {
				for(int gy=startgridy;gy<=endgridy+1;gy++) {
					if(gy+py*PATCH_SIZE+wy*MAP_SIZE==ptLT.y || gy+py*PATCH_SIZE+wy*MAP_SIZE==ptRB.y) {
						for(int gx=startgridx;gx<endgridx+1;gx++) {
								pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
								pIB[nCount++]=(gx+1)+(gy*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
				for(int gx=startgridx;gx<=endgridx+1;gx++) {
					if(gx+px*PATCH_SIZE+wx*MAP_SIZE==ptLT.x || gx+px*PATCH_SIZE+wx*MAP_SIZE==ptRB.x) {
						for(int gy=startgridy;gy<endgridy+1;gy++) {
							pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
							pIB[nCount++]=gx+((gy+1)*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
			}
			else {
				for(int gy=startgridy;gy<=endgridy+1;gy++) {
					if((gy+py*PATCH_SIZE+wy*MAP_SIZE-ptLT.y)%dx==0) {
						for(int gx=startgridx;gx<endgridx+1;gx++) {
								pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
								pIB[nCount++]=(gx+1)+(gy*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
				for(int gx=startgridx;gx<=endgridx+1;gx++) {
					if((gx+px*PATCH_SIZE+wx*MAP_SIZE-ptLT.x)%dx==0) {
						for(int gy=startgridy;gy<endgridy+1;gy++) {
							pIB[nCount++]=gx+(gy*(PATCH_SIZE+1))+indexoffset;
							pIB[nCount++]=gx+((gy+1)*(PATCH_SIZE+1))+indexoffset;
						}
					}
				}
			}
		}
	}

	g_pGridIB->Unlock();
	m_pd3dDevice->SetIndices( g_pGridIB );
	m_pd3dDevice->SetStreamSource( 0, pLand->m_pVB, 0, sizeof( D3DLANDSCAPEVERTEX ) );

	if(nCount>0)
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_LINELIST, 0, 0, (PATCH_SIZE+1)*(PATCH_SIZE+1) * ( NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), 0, nCount / 2 );
}
*/