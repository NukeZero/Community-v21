#include "stdafx.h"
#include "defineObj.h"
#include "CreateObj.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL IsUsableItem( CItemBase* pItem )
{
	if( pItem == NULL )
		return FALSE;
	
	if( pItem->GetExtra() == 0 )
		return TRUE;
	else
		return FALSE;	// 거래중이거나 개인 상점에 올린 아이템은 사용할 수 없다.
}

BOOL IsUsingItem( CItemBase* pItem )
{
	if( pItem == NULL )
		return FALSE;
	
	if( pItem->GetExtra() != 0 )
		return TRUE;
	else
		return FALSE;	// 거래중이거나 개인 상점에 올린 아이템은 사용할 수 없다.
}


//////////////////////////////////////////////////////////////////////
// CItemBase
//////////////////////////////////////////////////////////////////////
CItemBase::CItemBase() 
{ 
	m_szItemText[0]  = '\0';
	m_dwItemId	     = 0;
	m_dwObjId	     = NULL_ID;
	m_dwObjIndex	 = NULL_ID;
	m_nExtra	     = 0;
	m_nCost		     = 0;
	m_dwSerialNumber = 0;

	SetTexture( NULL );	
}

void CItemBase::Empty()
{
	m_szItemText[0] = '\0';
	m_dwItemId = 0;
	m_nExtra   = 0;
	SetTexture( NULL );
}

ItemProp* CItemBase::GetProp() 
{ 
	return	prj.GetItemProp( m_dwItemId );	
}

CItemBase& CItemBase::operator = ( CItemBase& ib )
{
	_tcscpy( m_szItemText, ib.m_szItemText );
	m_dwItemId	     = ib.m_dwItemId  ;
	m_dwSerialNumber = ib.m_dwSerialNumber;

#ifdef __CLIENT
	m_pTexture       = ib.m_pTexture;
#endif

	return *this;
}

void CItemBase::SetTexture()
{
#ifdef __CLIENT
	ItemProp* pProp =GetProp();
	if (!pProp)
	{
		LPCTSTR szErr = Error("CItemBase::SetTexture GetProp() NULL Return %d", m_dwItemId);
		ADDERRORMSG( szErr );
	}

	m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, GetProp()->szIcon), 0xffff00ff );
#endif
}

//
// 아이템 가격을 계산해서 리턴한다.
// -1을 리턴하면 처리해선 안된다.
//
int	CItemBase::GetCost( void )
{
	ItemProp *pProp = GetProp();
	if( pProp == NULL )		
		return -1;
	if( pProp->dwCost == 0xFFFFFFFF )	
		return -1;
		
	CItemElem *pItemElem = (CItemElem *)this;

	int nCost;
	if( pItemElem->GetAbilityOption() )
	{		
		//INT((아이템가격+아이템가격*(0.1+(아이템강화레벨*아이템강화레벨)/18))
		nCost = (int)( pProp->dwCost + pProp->dwCost * ( 0.1f + ( pItemElem->GetAbilityOption() * pItemElem->GetAbilityOption() ) / 18.0f ) );
	}
	else
	{
		nCost = (int)pProp->dwCost;
	}
	
	return nCost;
}

// 퀘스트 아이템인가?
BOOL CItemBase::IsQuest()
{
	ItemProp* p = GetProp();
	if( p->dwItemKind3 == IK3_QUEST )
	{
	#ifdef __JAPAN__
		if( p->dwID == II_SYS_SYS_QUE_REDSOCKS )	// 예외 
			return FALSE;
	#endif //__JAPAN__

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CItemElem - 아이템 하나하나의 요소를 말한다.
//////////////////////////////////////////////////////////////////////
/*
	1 ~ 99    
	100 ~ 유저 지정 로고

	아이템에 로고 번호.

*/

CItemElem::CItemElem()
{
	m_idGuild = 0;
	m_nItemNum = 1;
	m_nAbilityOption = 0;
	m_nRepairNumber	= 0;
	m_nHitPoint		= -1;
	m_byFlag	= 0;
#ifdef __CLIENT
	m_bRepair	= FALSE;
#endif	// __CLIENT

    m_bItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;	
	m_nResistSMItemId = 0;

#ifdef __J0519
#ifdef __WORLDSERVER
	m_bOnQuerying	= FALSE;
#endif	// __WORLDSERVER
#endif	// __J0519

#ifdef __PIERCING_SM_TIME
	memset( &m_piercingInfo, 0, sizeof(m_piercingInfo) );
	m_bCharged	= FALSE;
	m_dwKeepTime	= 0;
#endif	// __PIERCING_SM_TIME

#ifdef __RANDOMOPTITEM0628
	m_nRandomOptItemId	= 0;
#endif	// __RANDOMOPTITEM0628
	
#ifdef __XPET2
	m_nMaxLevel = 0;	
	m_nLevel = 0;		
	m_dwHungry = 0;		
	m_dwFeeling = 0;	
#endif // xPet2
}

CItemElem::~CItemElem()
{
}

// 돈의 양을 얻는다. 
int	CItemElem::GetGold()
{
#ifdef __NPP_050308
	ASSERT( GetProp()->dwItemKind1 == IK1_GOLD );
	return m_nHitPoint;		// int형으로 돈을 버리게 하기 위해서 이 변수를 사용 
#else
	return m_nItemNum;
#endif
}

void CItemElem::UseItem()
{
	if( GetProp()->bPermanence != TRUE )
		m_nItemNum--;
}

CItemElem& CItemElem::operator =( CItemElem & ie )
{
	CItemBase::operator =( ie );

	SetTexture( ie.GetTexture() );

	m_nItemNum             = ie.m_nItemNum;
	m_nRepairNumber        = ie.m_nRepairNumber;
	m_nHitPoint            = ie.m_nHitPoint;
	m_byFlag               = ie.m_byFlag;
	m_nAbilityOption       = ie.m_nAbilityOption;
	m_idGuild              = ie.m_idGuild;
	m_bItemResist          = ie.m_bItemResist;
	m_nResistAbilityOption = ie.m_nResistAbilityOption;
	m_nResistSMItemId      = ie.m_nResistSMItemId;
	
#ifdef __PIERCING_SM_TIME		
	m_dwKeepTime           = ie.m_dwKeepTime;
	memcpy( &m_piercingInfo, &ie.m_piercingInfo, sizeof(PIERCINGINFO) );
	m_bCharged	           = ie.m_bCharged;
#endif	// __PIERCING_SM_TIME

#ifdef __RANDOMOPTITEM0628
	m_nRandomOptItemId     = ie.m_nRandomOptItemId;
#endif	// __RANDOMOPTITEM0625
	
	return *this;
}

// 유료아이템인가?
BOOL CItemElem::IsCharged()
{
#ifdef __PIERCING_SM_TIME
	if( m_bCharged == 1 || m_nResistSMItemId != 0 )
#else 
	if( GetProp()->bCharged == 1 || m_nResistSMItemId != 0 )
#endif 
		return TRUE;

	return FALSE;
}

#ifdef __PIERCING_SM_TIME
void CItemElem::GetPiercingAvail( PPIERCINGAVAIL pPiercingAvail )
{
	for( int i = 0; i < m_piercingInfo.nPiercedSize; i++ )
	{
		PPIERCINGAVAIL ptr	= CPiercingAvail::GetInstance()->GetPiercingAvail( m_piercingInfo.adwItemId[i] );
		if( ptr )
		{
			for( int j = 0; j < ptr->nSize; j++ )
			{
				int nFind	= -1;
				for( int k = 0; k < pPiercingAvail->nSize; k++ )
				{
					if(  pPiercingAvail->anDstParam[k] == ptr->anDstParam[j] )
					{
						nFind	= k;
						break;
					}
				}
				if( nFind < 0 )
					nFind	= pPiercingAvail->nSize++;
				pPiercingAvail->anDstParam[nFind]		= ptr->anDstParam[j];
				pPiercingAvail->anAdjParam[nFind]		+= ptr->anAdjParam[j];
			}
		}
	}
}
#endif// __PIERCING_SM_TIME

// 주사위로 제련 가능한 아이템 종류
BOOL CItemElem::IsDiceRefineryAble( ItemProp* pProp )
{
	if( !pProp )
		return FALSE;

	if( pProp->dwItemKind2 == IK2_ARMOR ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT ||
		pProp->dwItemKind2 == IK2_ARMORETC )
		return TRUE;
					
	return FALSE;
}

// 카드로 제련 가능한 아이템 종류
BOOL CItemElem::IsEleRefineryAble( ItemProp* pProp )
{
	if( !pProp )
		return FALSE;

	if( pProp->dwItemKind3 == IK3_SUIT ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT )
		return TRUE;
	
	return FALSE;
}



BOOL CItemElem::IsBinds( void )
{
#ifndef __FINITEITEM0705
	return ( GetProp()->dwItemKind3 == IK3_EVENTMAIN );
#else
	if( m_dwKeepTime )
		return TRUE;

	//raiders_060112
	if( (GetProp()->dwFlag & IP_FLAG_BINDS) == IP_FLAG_BINDS )
		return TRUE;
	return FALSE;

#endif	// __FINITEITEM0705
}

	

#ifdef __WORLDSERVER
CItemPool*	CItem::m_pPool	= new CItemPool( 512 );
#else	// __WORLDSERVER
#ifndef __BEAST
CItemPool*	CItem::m_pPool	= new CItemPool( 128 );
#endif
#endif	// __WORLDSERVER

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
#ifdef __XITEMDROP0908
	m_fGroundY = 0;
	m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;
#endif

#if !defined(__NPP_050308)						
	m_idFrom = NULL_ID;
#endif
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
		#if !defined(__BEAST) && !defined(__ONLINE)
			#ifdef __V050426_REMOVERESPAWN
				GetWorld()->m_respawner.Increment( GetRespawn(), m_nRespawnType, FALSE );
			#else
				GetWorld()->m_respawner.Increment( GetRespawn(), FALSE );
			#endif
		#endif
	}
}

BOOL CItem::Read( CFileIO* pFile )
{
	CObj::Read( pFile );
/*
	m_pItemBase = new CItemElem;

	pFile->Read( m_szName, sizeof( m_szName ) );
	pFile->Read( szDialogFile, sizeof( szDialogFile ) );
	pFile->Read( m_szCharacterKey, sizeof( m_szCharacterKey ) );
	pFile->Read( &m_dwBelligerence, sizeof( m_dwBelligerence ) );
*/
	return TRUE;
}
BOOL CItem::Write( CFileIO* pFile )
{
	CObj::Write( pFile );
/*
	m_pItemBase = new CItemElem;

	pFile->Read( m_szName, sizeof( m_szName ) );
	pFile->Read( szDialogFile, sizeof( szDialogFile ) );
	pFile->Read( m_szCharacterKey, sizeof( m_szCharacterKey ) );
	pFile->Read( &m_dwBelligerence, sizeof( m_dwBelligerence ) );
*/
	return TRUE;
}
#ifdef __XITEMDROP0908
// 아이템을 생성시킬땐 이것을 불러줘야 중력에 의해 떨어진다.
void CItem::SetDelta( float fGroundY, D3DXVECTOR3 &vDelta )
{
	m_fGroundY = fGroundY;
	m_vDelta = vDelta;
}
#endif // xItemDrop0908

void CItem::Process( FLOAT fElapsedTime )
{
#ifdef __V050426_REMOVERESPAWN
	CCtrl::Process( fElapsedTime );
#else // __V050426_REMOVERESPAWN
//{{AFX
	CObj::Process( fElapsedTime );
//}}AFX
#endif // __V050426_REMOVERESPAWN
#ifdef __CLIENT
	AddAngle( 0.5f );
#ifdef __XITEMDROP0908
	D3DXVECTOR3 vPos = GetPos();

	//--- 비행몹에게 떨어진 좌표는 이처리를 해선 안된다. 
	if( m_fGroundY )	// 이게 0이면 중력처리를 해선 안된다.
	{
		if( vPos.y > m_fGroundY )		// 공중에 떠있느냐?
		{
	//		m_vDelta.y -= 0.005f;		// 이동벡터에 중력 벡터 더함.
			m_vDelta.y -= 0.0075f;		// 이동벡터에 중력 벡터 더함.
		} else
		{
//			m_fGroundY = GetWorld()->GetFullHeight( vPos );		// 땅에 떨어지고 나서 경사면에선 묻힐수가 있으니 한번더 계산.
//			m_fGroundY += 0.25f;
			vPos.y = m_fGroundY;		// 바닥에 닿은 상태면 지면좌표와 동일하게 맞춤.
			m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;	// 이동벡터는 없어짐. 튀기게 하려면 이렇게 하면 안된다.
			SetPos( vPos );				// 최종 좌표 세팅.
		}

		// 이동벡터가 없으면 더해줄필요 없다.
		if( m_vDelta.x == 0 && m_vDelta.y == 0 && m_vDelta.z == 0 )
		{
			// 현재 좌표가 변경될 필요 없다.
		} else
		{
			m_vDelta.x = m_vDelta.z = 0;		// 여기에 값 넣지말것.
			vPos += m_vDelta;			// 이동 벡터를 더함.
			SetPos( vPos );
		}
	}
#endif // xItemDrop0908
#endif // Client

#ifdef __WORLDSERVER
#ifdef __SCHOOL0701
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return;
#endif	// __SCHOOL0701
	if( (int)g_tmCurrent - (int)m_dwDropTime > MIN( 10 ) )
		Delete();
#endif	// __WORLDSERVER
}

void CItem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	ItemProp *pItemProp = GetProp();
	if( pItemProp && pItemProp->nReflect > 0 )
	{
		// 주의!!! : m_pModel이 CModelObject라는것을 가정하고 한것이다. 아니라면 이렇게 쓰면 안된다.
		((CModelObject*)m_pModel)->SetEffect( 0, XE_REFLECT );	
	}

	CObj::Render( pd3dDevice );
	if( !(rand() % 50) )
		CreateSfx( pd3dDevice, XI_GEN_ITEM_SHINE01, GetPos() );
#ifdef __BEAST
	if( m_respawnInfo.m_dwIndex )
		RenderRect( pd3dDevice, m_respawnInfo.m_rect, RECTCOLOR_ITEM );
#endif
}

void CItem::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{
#ifndef __WORLDSERVER
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
#endif	// __WORLDSERVER
}

