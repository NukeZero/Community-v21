#include "stdafx.h"

#include <stdio.h>
#include "ase.h"
#include "scanner.h"

extern float	g_fScale;
//extern char		g_strError[];


CGeoMObject :: CGeoMObject()
{
	Init();
}

CGeoMObject :: CGeoMObject( CAse *pASE )
{
	m_pASE = pASE;
	Init();
}

CGeoMObject :: ~CGeoMObject()
{
	Destroy();
}

void	CGeoMObject :: Init( void )
{
	m_Type = GMT_ERROR;
	m_pParent = NULL;
	m_bHelper = 0;

	m_nID = -1;
	m_nIdx = -1;
	memset( m_strName, 0, sizeof(m_strName) );
	m_pMesh = NULL;
	m_pFrame = NULL;	
	m_pLastFrame = NULL;
	m_nMaxChild = 0;
	m_pChildList = NULL;
	m_pd3dDevice = NULL;

	D3DXMatrixIdentity( &m_matWorld );

	m_vScaleFactor = D3DXVECTOR3( 0, 0, 0 );
	m_vScaleAxis   = D3DXVECTOR3( 0, 0, 0 );
	m_fScaleAngle  = 0;
	m_fFlip = 1.0f;
}

void CGeoMObject :: Destroy( void )
{
	int		i;

	DeleteDeviceObjects();

	SAFE_DELETE( m_pMesh );

	// ASETM_ANIMATION이 있을때 그것을 파괴.
	if( m_pFrame )
	{
		for( i = 0; i < m_pASE->m_nFrameMax; i ++ )
		{
			SAFE_DELETE( m_pFrame[i] );
		}
		SAFE_DELETE_ARRAY( m_pFrame );
	}
	// 하위 노드들도 차례로 파괴
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		SAFE_DELETE( m_pChildList[i] );
	}
	SAFE_DELETE_ARRAY( m_pChildList );

	Init();
}

HRESULT CGeoMObject :: InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	if( m_pMesh )
		m_pMesh->InitDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT CGeoMObject :: RestoreDeviceObjects()
{
	if( m_pMesh )
		m_pMesh->RestoreDeviceObjects();

	return S_OK;
}

HRESULT CGeoMObject :: InvalidateDeviceObjects()
{
	return DeleteDeviceObjects();
}

HRESULT CGeoMObject :: DeleteDeviceObjects()
{
	if( m_pMesh )
		m_pMesh->DeleteDeviceObjects();
	return S_OK;
}


// *NODE_TM 부분의 Transform Matrix를 읽음
int		CGeoMObject :: Read_NODE_TM( CScanner *scan )
{
	int		nNumBlock = 0;
	D3DXMATRIX	*m = &m_matTM;
//	D3DXMATRIX	*pm;
	D3DXMATRIX	matScale;
	CGeoMObject *pParent = m_pParent;

//	m = matScale;
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "TM_ROW0" ) == 0 )		// MAX의 좌표계를 D3D로 바꾸기 위해 y,z를 바꿔준다.
		{
			m->_11 = scan->GetFloat();	m->_13 = scan->GetFloat();	m->_12 = scan->GetFloat();	m->_14 = 0;		scan->GetToken();	scan->GetToken();	// * TM_ROW1이 나올게 뻔하므로 그냥 스킵한다
			m->_31 = scan->GetFloat();	m->_33 = scan->GetFloat();	m->_32 = scan->GetFloat();	m->_34 = 0;		scan->GetToken();	scan->GetToken();	// * TM_ROW2이 나올게 뻔하므로 그냥 스킵한다
			m->_21 = scan->GetFloat();	m->_23 = scan->GetFloat();	m->_22 = scan->GetFloat();	m->_24 = 0;		scan->GetToken();	scan->GetToken();	// * TM_ROW3이 나올게 뻔하므로 그냥 스킵한다
			m->_41 = scan->GetFloat();	m->_43 = scan->GetFloat();	m->_42 = scan->GetFloat();	m->_44 = 1;
			
//			D3DXMatrixScaling( &matScale, 0.1, 0.1, 0.1 );
//			D3DXMatrixMultiply( &mat2, &matScale, m );
//			*m = mat2;
			m->_41 *= g_fScale;
			m->_42 *= g_fScale;
			m->_43 *= g_fScale;

//			m_matWorld = *m;
		}

		if( strcmp( scan->token, "TM_ROTAXIS" ) == 0 )
		{
			m_vRotAxis.x = scan->GetFloat();
			m_vRotAxis.z = scan->GetFloat();
			m_vRotAxis.y = scan->GetFloat();
		}
		if( strcmp( scan->token, "TM_ROTANGLE" ) == 0 )
		{
			m_fRotAngle = scan->GetFloat();
		}
#ifdef __XSCALE
		if( strcmp( scan->token, "TM_SCALE" ) == 0 )
		{
			m_vScaleFactor.x = scan->GetFloat();
			m_vScaleFactor.z = scan->GetFloat();
			m_vScaleFactor.y = scan->GetFloat();
		}
		if( strcmp( scan->token, "TM_SCALEAXIS" ) == 0 )
		{
			m_vScaleAxis.x = scan->GetFloat();
			m_vScaleAxis.z = scan->GetFloat();
			m_vScaleAxis.y = scan->GetFloat();
		}
		if( strcmp( scan->token, "TM_SCALEAXISANG" ) == 0 )
		{
			m_fScaleAngle = scan->GetFloat();
		}
#endif
		if( strcmp( scan->token, "DET" ) == 0 )
		{
			m_fFlip = scan->GetFloat();
			if( m_fFlip < 0 )
				Message( "경고 : 오브젝트%s의 스케일이 마이너스값이다. 트랜스폼 리셋을 할것.", m_strName );
		}
		
	}
/*
	BOOL	bFlip = (m_fFlip < 0) ? TRUE : FALSE;
	BOOL	bPFlip = FALSE;

	if( pParent && pParent->m_fFlip < 0 )
		bPFlip = TRUE;

	if( bFlip && pParent == NULL )	// 루트가 미러 되었다.
	{
		m_matTM._41 = -m_matTM._41;		// 부호 바꿈
		m_matTM._42 = -m_matTM._42;
		m_matTM._43 = -m_matTM._43;
	} else
	if( bFlip != bPFlip )
	{
		m_matTM._41 = -m_matTM._41;		// 부호 바꿈
		m_matTM._42 = -m_matTM._42;
		m_matTM._43 = -m_matTM._43;
	}
*/
	m_matWorld = m_matTM;
	

	D3DXMatrixInverse( &m_matInverseTM, NULL, &m_matWorld );		// NODE_TM을 인버스 시킴
	if( pParent )
	{
		//				// 자기 pivot월드좌표에서 부모 pivot월드 좌표를 빼준다.  그러면 부모pivot기준의 로컬좌표가 나온다.
		//				pm = &pParent->m_matTM;
		// NodeTM을 부모중심의 좌표로 바꾸기 위해
		// LocalTM = World * Inv(Parent)를 적용 한다;
		D3DXMatrixMultiply( &m_matLocalTM, &m_matWorld, &pParent->m_matInverseTM );
		// LocalTM의 회전과 위치를 쿼터니언과 좌표로 변환시켜둠
		D3DXQuaternionRotationMatrix( &m_qLocal, &m_matLocalTM );		
		m_vLocal.x = m_matLocalTM._41;
		m_vLocal.y = m_matLocalTM._42;
		m_vLocal.z = m_matLocalTM._43;
	}
	else
	{
		m_matLocalTM = m_matWorld;
	}

	{
		D3DXQUATERNION	qScaleRot, qAdd;
		D3DXMATRIX mTest, _mRot, _mTrans, mScale;
		D3DXMATRIX mITrans, mIRot, mIScale, m1, m2;

		// 스케일 매트릭스
		D3DXQuaternionRotationAxis( &qScaleRot, &m_vScaleAxis, m_fScaleAngle );
		D3DXMatrixRotationAxis( &mScale, &m_vScaleAxis, m_fScaleAngle );	// U
		D3DXMatrixInverse( &m1, NULL, &mScale );		// Ut
		D3DXMatrixScaling( &m2, m_vScaleFactor.x, m_vScaleFactor.y, m_vScaleFactor.z );
		m1 *= m2;	// Ut * K
		mScale = m1 * mScale;		// S = Ut K U
		
		D3DXMatrixTranslation( &_mTrans, m_matTM._41, m_matTM._42, m_matTM._43 );	
//		m_vRotAxis *= m_fFlip;
		D3DXQuaternionRotationAxis( &qAdd, &m_vRotAxis, m_fRotAngle );
		D3DXMatrixRotationQuaternion( &_mRot, &qAdd );	// 누적된 쿼터니언을 회전 매트릭스로 변환

		mTest = mScale;
		mTest *= _mRot;
		mTest *= _mTrans;
		
		
//		D3DXMatrixInverse( &mITrans, NULL, &_mTrans );
//		D3DXMatrixInverse( &mIRot, NULL, &_mRot );
//		D3DXMatrixInverse( &mIScale, NULL, &mScale );
		
//		m1 = mIRot;
//		m1 *= mIScale;
//		m1 *= m_matTM;
//		m1 *= mITrans;
		
		int a = 0;
		
	}
	


	return 1;
}



int		CGeoMObject :: Read_CONTROL_POS_TRACK( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nTick = 0;
	ASETM_ANIMATION	*pTmAni = NULL;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "CONTROL_POS_SAMPLE" )	== 0 )
		{
			int		nFrame;
			nTick = scan->GetNumber();				// 첫번째 Frame Tick을 읽음
			nFrame = nTick / (m_pASE->m_nTicksPerFrame * m_pASE->m_nFramePerSample);
			pTmAni = m_pFrame[ nFrame ];
			pTmAni->m_nTick = nTick;
			pTmAni->m_vPos.x = scan->GetFloat() * g_fScale;	// x
			pTmAni->m_vPos.z = scan->GetFloat() * g_fScale;	// z
			pTmAni->m_vPos.y = scan->GetFloat() * g_fScale;	// y 
			// 아직 Animation 행렬은 만들지 않음.
			pTmAni->m_bFlag |= 0x01;		// POS정보가 있었다는걸 표시
		}
	}

	return 1;
}


int		CGeoMObject :: Read_CONTROL_ROT_TRACK( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nTick = 0;
	ASETM_ANIMATION	*pTmAni = NULL;

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "CONTROL_ROT_SAMPLE" ) == 0 )
		{
			nTick = scan->GetNumber();				// 첫번째 Frame Tick을 읽음
			int nFrame = nTick / (m_pASE->m_nTicksPerFrame * m_pASE->m_nFramePerSample);
			pTmAni = m_pFrame[ nFrame ];
			pTmAni->m_nTick = nTick;
			pTmAni->m_vAxis.x = scan->GetFloat();	// x
			pTmAni->m_vAxis.z = scan->GetFloat();	// z
			pTmAni->m_vAxis.y = scan->GetFloat();	// y
			pTmAni->m_fAngle = scan->GetFloat();	// angle
			// 아직 Animation 행렬은 만들지 않음.
			pTmAni->m_bFlag |= 0x02;		// ROT정보가 있었다는걸 의미
		}
	}

	return 1;
}

int		CGeoMObject :: Read_CONTROL_SCALE_TRACK( CScanner *scan )
{
	int		nNumBlock = 0;
	int		nTick = 0;
	ASETM_ANIMATION	*pTmAni = NULL;
	D3DXVECTOR3	vScaleAxis;
	FLOAT		fScaleAngle;
	
	m_pASE->m_bScale = TRUE;		// 스케일 애니메이션이 포함되었다.

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "CONTROL_SCALE_SAMPLE" ) == 0 )
		{
			nTick = scan->GetNumber();				// 첫번째 Frame Tick을 읽음
			int nFrame = nTick / (m_pASE->m_nTicksPerFrame * m_pASE->m_nFramePerSample);
			pTmAni = m_pFrame[ nFrame ];
			pTmAni->m_nTick = nTick;
			pTmAni->m_vScaleFactor.x = scan->GetFloat();	// scale x
			pTmAni->m_vScaleFactor.z = scan->GetFloat();	// scale z
			pTmAni->m_vScaleFactor.y = scan->GetFloat();	// scale y
			vScaleAxis.x = scan->GetFloat();	// scale axis x;
			vScaleAxis.z = scan->GetFloat();	// scale axis z;
			vScaleAxis.y = scan->GetFloat();	// scale axis y;
			fScaleAngle  = scan->GetFloat();	// scale angle
			D3DXQuaternionRotationAxis( &pTmAni->m_qAniScale, &vScaleAxis, fScaleAngle );	// 축/각도 => 쿼터니언
			{
				D3DXMATRIX	m, m2;
				D3DXMatrixRotationQuaternion( &m, &pTmAni->m_qAniScale );	// 쿼터니온 -> 매트릭스.
				D3DXMatrixRotationAxis( &m2, &vScaleAxis, fScaleAngle );	// 요건 테스트
			}
			// 아직 Animation 행렬은 만들지 않음.
			//	D3DXQuaternionRotationAxis( &qRot, &m_vRotAxis, m_fRotAngle );		// 회전은 쿼터니온으로 변환.
			//	D3DXMatrixRotationQuaternion( &mRot, &qRot );						// 회전 매트릭스
			//	D3DXMatrixRotationAxis( &m1, &m_vRotAxis, &m_fRotAngle );	// mRot하고 m1하고 결과가 같은지 검사할것.
			pTmAni->m_bFlag |= 0x04;		// ROT정보가 있었다는걸 의미
		}
	}
	
	return 1;
}



int		CGeoMObject :: Read_TM_ANIMATION( CScanner *scan )
{
	int		nNumBlock = 0;
	int		i;
	ASETM_ANIMATION	*pTmAni = NULL;
	D3DXMATRIX		matTemp, matTemp2, matLast;
	D3DXQUATERNION		qAdd;//, qLastKey;
	D3DXMATRIX		mTrans, mRot, mScale;
	D3DXMATRIX		m1, m2, m3;
	D3DXQUATERNION	qRot, qScaleRot;
	D3DXVECTOR3		vScaleFactor;

//	return 1;
	m_pFrame = new ASETM_ANIMATION*[ m_pASE->m_nFrameMax ];
	for( i = 0; i < m_pASE->m_nFrameMax; i ++ )				// 프레임은 모두 할당받아놓는다
	{
		m_pFrame[i] = new ASETM_ANIMATION;
		memset( m_pFrame[i], 0, sizeof(ASETM_ANIMATION) );
	}

	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( strcmp( scan->token, "CONTROL_POS_TRACK" ) == 0 )	Read_CONTROL_POS_TRACK( scan );
		if( strcmp( scan->token, "CONTROL_ROT_TRACK" ) == 0 )	Read_CONTROL_ROT_TRACK( scan );
#ifdef __XSCALE
		if( strcmp( scan->token, "CONTROL_SCALE_TRACK" ) == 0 )	Read_CONTROL_SCALE_TRACK( scan );
#endif
	}

	// NODE_TM에서 분해된 요소들.....이동,회전,확축 
	// M = T F R U K Ut .. Ut 부터 뒤에서 곱한다.
	// 이동매트릭스.
	{
	D3DXVECTOR3 _vPos;
	_vPos.x = m_matLocalTM._41;		
	_vPos.y = m_matLocalTM._42;		
	_vPos.z = m_matLocalTM._43;
//	_vPos *= m_fFlip;

	D3DXMatrixTranslation( &mTrans, _vPos.x, _vPos.y, _vPos.z );	
//	D3DXMatrixTranslation( &mTrans, m_matLocalTM._41, m_matLocalTM._42, m_matLocalTM._43 );	
	}
	// 회전 매트릭스.
//	D3DXQuaternionRotationAxis( &qRot, &m_vRotAxis, m_fRotAngle );		// 회전은 쿼터니온으로 변환.
//	D3DXMatrixRotationQuaternion( &mRot, &qRot );						// 회전 매트릭스
//	D3DXMatrixRotationAxis( &m1, &m_vRotAxis, &m_fRotAngle );	// mRot하고 m1하고 결과가 같은지 검사할것.
	// 스케일 매트릭스
	D3DXQuaternionRotationAxis( &qScaleRot, &m_vScaleAxis, m_fScaleAngle );
	D3DXMatrixRotationAxis( &mScale, &m_vScaleAxis, m_fScaleAngle );	// U
	D3DXMatrixInverse( &m1, NULL, &mScale );		// Ut
	D3DXMatrixScaling( &m2, m_vScaleFactor.x, m_vScaleFactor.y, m_vScaleFactor.z );
	m1 *= m2;	// Ut * K
	mScale = m1 * mScale;		// S = Ut K U
	vScaleFactor = m_vScaleFactor;
	// ROT키가 없는 경우에는 NODE_TM의 축/각도로 쿼터니언을 만들어 사용
	qAdd = D3DXQUATERNION( 0, 0, 0, 1.0f );
	// POS과 ROT정보를 다 읽었으면 이정보로 Animation TM과 각각의 쿼터니온회전, 이동행렬을 만든다
	for( i = 0; i < m_pASE->m_nFrameMax; i ++ )
	{
		pTmAni = m_pFrame[i];
		// 추후에 애니메이션할때 m_pFrame[i]가 NULL이면 GeoM TM과 같으므로 그것을 사용한다.
		if( i == 0 )
			if( (pTmAni->m_bFlag & 0x02) == 0 )		// 0번키의 회전이 없을땐 LocalTM에서 가져온다.
				D3DXQuaternionRotationMatrix( &qAdd, &m_matLocalTM );


		// POS이 있을경우.
		if( pTmAni->m_bFlag & 0x01 )
		{
			D3DXMatrixTranslation( &mTrans, pTmAni->m_vPos.x,  pTmAni->m_vPos.y,  pTmAni->m_vPos.z );
		}

		// ROT이 있을경우
		if( pTmAni->m_bFlag & 0x02 )
		{
			D3DXQUATERNION		q;
			// ASE의 축/각도값으로 쿼터니언으로 변환
			D3DXQuaternionRotationAxis( &q, &pTmAni->m_vAxis, pTmAni->m_fAngle );	// 축/각도 회전값을 쿼터니온으로 변환.
			D3DXQuaternionMultiply( &qAdd, &qAdd, &q );								// 쿼터니온 누적
			D3DXMatrixRotationQuaternion( &mRot, &qAdd );	// 누적된 쿼터니언을 회전 매트릭스로 변환
		}

		// SCALE이 있을경우 S = Ut K U
		if( pTmAni->m_bFlag & 0x04 )
		{
			D3DXMatrixRotationQuaternion( &m1, &pTmAni->m_qAniScale );	// U
			D3DXMatrixScaling( &m2, pTmAni->m_vScaleFactor.x, pTmAni->m_vScaleFactor.y, pTmAni->m_vScaleFactor.z ); // K
			D3DXMatrixInverse( &mScale, NULL, &m1 );	// Ut
			mScale *= m2;
			mScale *= m1;
			vScaleFactor = pTmAni->m_vScaleFactor;		// 마지막으로 읽은 scale factor
			qScaleRot    = pTmAni->m_qAniScale;			// 마지막으로 읽은 scale 쿼터니온
		}	
		// mAniTM = Scale * Rotation * Translation 으로 최종 합침.
//		D3DXMatrixIdentity( &pTmAni->m_matAniTM );
#ifdef __XSCALE
//		D3DXMatrixMultiply( &pTmAni->m_matAniTM, &pTmAni->m_matAniTM, &mScale );
#endif
//		D3DXMatrixMultiply( &pTmAni->m_matAniTM, &pTmAni->m_matAniTM, &mRot );
//		D3DXMatrixMultiply( &pTmAni->m_matAniTM, &pTmAni->m_matAniTM, &mTrans );
		// Animate()안에서 보간을 해야하므로 POS와 ROT(쿼터니온)를 따로 가지고 있는다.
		pTmAni->m_qAni = qAdd;
		pTmAni->m_vPos.x = mTrans._41;
		pTmAni->m_vPos.y = mTrans._42;
		pTmAni->m_vPos.z = mTrans._43;
		pTmAni->m_vScaleFactor = vScaleFactor;
		pTmAni->m_qAniScale	   = qScaleRot;
//		matLast = pTmAni->m_matAniTM;		// 마지막으로 사용된 매트릭스를 알게 하기 위해...
	}
	

	return 1;
}

// 스킵시켜야 할 오브젝트를 판별
BOOL	IsSkipObject( LPCTSTR szName )
{
	char	szBuff[256];
	
	memset( szBuff, 0, sizeof(szBuff) );	// 이걸 안해주면 szBuff+8할때 쓰레기값을 억세스 할 수 있다.
	strcpy( szBuff, szName );
	_strlwr( szBuff );		// 소문자로 변환해서 검사.
	if( strncmp( szBuff, "particle", 8 ) == 0 )		return TRUE;
	if( strncmp( szBuff, "dummy", 5 ) == 0 )		return TRUE;
	if( strncmp( szBuff+8, "finger", 6 ) == 0 )		return TRUE;
	if( strncmp( szBuff+8, "toe", 3 ) == 0 )		return TRUE;
	if( strncmp( szBuff+6, "footstep", 8 ) == 0 )	return TRUE;
	
	return FALSE;
}


// GEOMOBJECT의 블럭이 끝날때까지 처리
int		CGeoMObject :: Read_GEOMOBJECT( CScanner *scan )
{
	int		nNumBlock = 0;
	BOOL	bBiped = FALSE;		// Bip, Bone등의 노드는 TRUE가 된다.
	BOOL	bSkip = FALSE;		// 현재 블럭을 빠져나옴.
	char	szName[256];
	
	while(1)
	{
		scan->GetToken();
		if( scan->token[0] == '{' )		nNumBlock ++;
		if( scan->token[0] == '}' )	
		{
			if( --nNumBlock == 0 )	break;
		}
		if( bSkip )		continue;		// 현재 블럭을 빠져 나가게 함

		if( strcmp( scan->token, "HELPER_CLASS" ) == 0 )	// IK오브젝트가 나오면 스킵한다.
		{
			scan->GetToken();
			if( strncmp( scan->token, "IK Chain", 8 ) == 0 )
				bSkip = TRUE;
		}

		if( strcmp( scan->token, "NODE_NAME" ) == 0 )
		{
			scan->GetToken();
			strcpy( m_strName, scan->token );			// 노드 이름 읽음
			strcpy( szName, m_strName );
			_strlwr( szName );
			if( strncmp( szName, "bone", 4 ) == 0 || strncmp( szName, "bip", 3 ) == 0 )
				bBiped = TRUE;
			if( IsSkipObject( m_strName ) )		// 손가락 발가락 더미 등은 스킵시킨다.
				bSkip = TRUE;
			if( strncmp( szName, "#lod", 4 ) == 0 )		// #lod는 생성시키지 않음.
				bSkip = TRUE;
			
//			if( strcmp( scan->token, "Bip01" ) == 0 )	g_pBone = pTree;		// 바이페드 루트를 찾으면 그걸 루트 포인트로 설정 - 이거 나중에 바꿔야 한다.
//			TRACE( "이거 바꿀것!" );
		}
		// 링크메쉬는 바이페드에 링크 되어 있다.
		// 그러므로 피지크가 없는 "메쉬"가 어떤 오브젝트를 부모로 가지고 있다면.
		// 부모만 등록 시키고 부모측에서는 자식으로 등록 하지 않는다(ASE에서조차 자식으로 되어 있지 않다).
		// 나중에 링크 오브젝트들을 처리할땐 부모 오브젝트의 WorldTM을 그대로 사용한다
		if( strcmp( scan->token, "NODE_PARENT" ) == 0 )		// 이 오브젝트가 어떤 부모에 연결되어 있음.
		{
			scan->GetToken();		// 따옴표내의 Parent오브젝트 이름 읽음
			strcpy( szName, scan->token );
			_strlwr( szName );
			// 부모 포인터 찾음.
			if( strncmp( scan->token, "Dummy", 5 ) == 0 )
			{
				Message( "Read_GEOMOBJECT : 부모오브젝트가 %s. Dummy는 사용할 수 없다.", scan->token );
				continue;
			}
			if( IsSkipObject( scan->token ) )		// 부모오브젝트가 스킵대상이면 이 오브젝트도 등록하지 앟음.
			{
				bSkip = TRUE;
				continue;
			}
			if( strncmp( szName, "#path", 5 ) == 0 )	// Bip01부모가 path면 부모로 등록하지 않고 따로 처리한다.
				continue;
			if( strncmp( szName, "#lod", 4 ) == 0 )		// #lod가 부모면 부모로쓰지 않음.
				continue;
			m_pParent = m_pASE->FindGeoM( m_pASE->GetCurrentGroup(), scan->token );
			if( m_pParent == NULL )
			{
				Error( "Parent Node %s 못찾음", scan->token );
			}
//				AfxMessageBox( "Parent노드 못찾음?" );
			// m_pParent의 자식노드에 현재 노드인pTree를 링크
			// this노드가 바이페드일 경우만 부모에 자식으로 등록한다.  메쉬일경우는 등록하지 않는다
			// Read_GEOMOBJECT를 다 처리한 후에 링크에 넣을지 말지 결정하면 어떨까 한다.
			if( bBiped )
				m_pParent->TreeLinkChild( this );		// 찾은 부모포인터에 현재 노드를 링크.
			else
			{
				int	a=0;
			}
		}
		// 스킨메쉬만 MESH정보를 읽는다.  바이페드는 읽을 필요 없다!
		if( strcmp( scan->token, "MESH" ) == 0 && bBiped == FALSE )
		{
			m_pMesh = new CASEMesh;
			m_pMesh->InitDeviceObjects( m_pd3dDevice );
			m_pMesh->SetGeoMObject( this );
			if( FAIL == m_pMesh->Read_MESH( scan ) )		return FAIL;
		}
		if( strcmp( scan->token, "PHYSIQUE" ) == 0 && bBiped == FALSE )	
		{
			m_pMesh->Read_PHYSIQUE( scan );		// 3link이상의 블렌드 스킨
#ifdef	__X2LINK
			if( m_pMesh->m_bRigid )		// 3link이상도 rigid로 처리하기땜에 그렇다.
			{
				Message( "Read_PHYSIQUE: %s 오브젝트에 링크가 2개를 초과하는 피지크가 있다.", m_strName );
				return 0;
			}
#endif

		}
//		TRACE( "여기도 바꿀것!" );
		if( strcmp( scan->token, "NODE_TM" ) == 0 )
		{
			// NODE_TM까지왔으면 PARENT_NODE가 나왔어야 했는데 NULL이라면 this는 탑노드라는
			// 얘기므로 이 노드를 임시로 탑으로 설정함
			// 단, 하위노드 읽는동안 다른 Top이 나오면 안된다.  그러면 방식 수정해야함.
//			if( m_pParent == NULL )		m_pASE->m_pTempTop = this;
			Read_NODE_TM( scan );
		}
		if( strcmp( scan->token, "TM_ANIMATION" ) == 0 )			
			Read_TM_ANIMATION( scan );
		if( strcmp( scan->token, "MATERIAL_REF" ) == 0 && bBiped == FALSE )
		{
			m_pMesh->m_nMaterialRef = scan->GetNumber();		// 이 GEOMOBJECT가 사용하는 매터리얼 번호
		}
	}

	
	if( bSkip )
	{
//		Message( "%d %s", m_nIdx, m_strName );
		return FAIL;
	}

	// 읽은 GMOBJECT가 스킨이었다면 자체TM이 필요없으므로 초기화 시킴.
	if( m_pMesh && m_pMesh->m_pPhysiqueVertex )
	{
		D3DXMatrixIdentity( &m_matTM );
		D3DXMatrixInverse( &m_matInverseTM, NULL, &m_matTM );
		m_matLocalTM = m_matTM;
	}


	return 1;
}

//
// 트리를 훑어 나가며 차례로 번호를 매겨줌 
int		CGeoMObject :: SetIdx( int nIdx )
{
	int		i;

	m_nIdx = nIdx++;

	for( i = 0; i < m_nMaxChild; i ++ )
	{
		nIdx = m_pChildList[i]->SetIdx( nIdx );
	}

	return nIdx;
}

// 트리 노드 갯수를 셈
int		CGeoMObject :: GetLength( void )
{
	int		nNum = 0;
	int		i;

//	Message( "%d %s", m_nIdx, m_strName );

	nNum ++;
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		nNum += m_pChildList[i]->GetLength();
	}

	return nNum;
}

// pNode부터 모든 하위 노드들을 검색하며 노드이름이 strName인것을 찾는다.
CGeoMObject	*CGeoMObject :: TreeFindNode( LPCTSTR strName )
{
	int		i, nMax;
	CGeoMObject *pTemp;

	if( strcmp( m_strName, strName ) == 0 )		// 현재 노드와 같으면 리턴
		return this;

	nMax = m_nMaxChild;
	for( i = 0; i < nMax; i ++ )
	{
		pTemp = m_pChildList[ i ]->TreeFindNode( strName );
		if( pTemp )			return pTemp;
	}

	return NULL;
}

// this에 pChild를 자식으로 등록
void	CGeoMObject :: TreeLinkChild( CGeoMObject *pChild )
{
	CGeoMObject **pTemp = new CGeoMObject*[ m_nMaxChild + 1 ];	// 현재 할당되어 있는 버퍼의 +1크기만한 버퍼 생성.

	if( m_nMaxChild > 0 )	// 이미 리스트가 있는 상태면 백업받고 원본 지움.
	{
		memcpy( pTemp, m_pChildList, sizeof( CGeoMObject* ) * m_nMaxChild );	// 새 버퍼로 옮김
		SAFE_DELETE_ARRAY( m_pChildList );		// 원본 지움 - 디스트럭쳐가 호출되면 안된다.
	}
	pTemp[ m_nMaxChild ] = pChild;		// 새버퍼 마지막에다 child넣음.
	
	m_pChildList = pTemp;	// 새버퍼를 다시 정식 등록
	m_nMaxChild ++;			// 갯수 하나 증가.
}

// 계층구조를 따라가며 의미없는 헬퍼오브젝트를 찾아서 삭제시킨다.
// 헬퍼를 찾으면 TRUE를 리턴하고 그놈을 삭제.
BOOL	CGeoMObject :: TreeDeleteHelper( void )
{
	int		i, j;
	BOOL	b;
	int		nMaxChild = m_nMaxChild;

	if( m_bHelper && m_pChildList == NULL )		return TRUE;	// 자식이 없는 헬퍼. 최하위 헬퍼다. 삭제대상.

	for( i = 0; i < nMaxChild; i ++ )
	{
		b = m_pChildList[i]->TreeDeleteHelper();
		if( b )		// [i]가 삭제대상이다.
		{
			SAFE_DELETE( m_pChildList[i] );		// index i의 차일드를 삭제;
/*
			if( m_nMaxChild > 1 )	// 차일드가 최소 1개는 있는가.
			{
				CGeoMObject **pTemp = new CGeoMObject*[ m_nMaxChild - 1 ];	// 현재 할당되어 있는 버퍼의 -1크기만한 버퍼 생성.
				nMax = 0;
				for( j = 0; j < m_nMaxChild; j ++ )		// 리스트 다시 카피.
				{
					if( m_pChildList[j] )
						pTemp[nMax++] = m_pChildList[j];
				}
				m_nMaxChild = nMax;		// 하나 삭제했으니 리스트 크기 재조정.
				SAFE_DELETE_ARRAY( m_pChildList );
				m_pChildList = pTemp;
			} else
			{
				m_nMaxChild = 0;
				SAFE_DELETE_ARRAY( m_pChildList );
			}
*/
		}
	}

	// 리스트 재정렬.
	nMaxChild = 0;
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		if( m_pChildList[i] )	
			nMaxChild ++;
	}
	if( nMaxChild > 0 )	// 차일드가 1개라도 존재해야 할당하겠지..
	{
		CGeoMObject **pTemp = new CGeoMObject*[ nMaxChild ];	// 현재 할당되어 있는 버퍼의 -1크기만한 버퍼 생성.
		CGeoMObject **p;
		p = pTemp;
		for( j = 0; j < m_nMaxChild; j ++ )		// 리스트 다시 카피.
		{
			if( m_pChildList[j] )
				*p++ = m_pChildList[j];
		}
		m_nMaxChild = nMaxChild;		// 하나 삭제했으니 리스트 크기 재조정.
		SAFE_DELETE_ARRAY( m_pChildList );
		m_pChildList = pTemp;
	} else
	{
		m_nMaxChild = 0;
		SAFE_DELETE_ARRAY( m_pChildList );
	}
	


	// 위에서 자식이 삭제되서 m_pChildList가 널이 될수 있으므로 한번더.
	if( m_bHelper && m_pChildList == NULL )		
		return TRUE;	// 자식이 없는 헬퍼. 최하위 헬퍼다. 삭제대상.
	

	return FALSE;
}

// 하위 노드들을 리커시브 하며 TM_ANIMATION에 따라 World트랜스폼 한다.
void	CGeoMObject :: SetupMatrices( int nFrame )
{/*
	D3DXMATRIX	matWorld, matWorld2;
	int		nCurrFrame = (int)m_pASE->m_fFrameCurrent;

	if( m_pFrame == NULL )		// 부모가 있을땐 나의AniTM과 부모의 최종변환 월드매트릭스를 곱한다.
	{	// 아예 프레임이 없는경우는 항상 LocalTM을 사용
		if( m_pParent )
			D3DXMatrixMultiply( &matWorld, &m_matLocalTM,	
										   &m_pParent->m_matWorld );
		else
			matWorld = m_matLocalTM;	
	} else
	{
		if( m_pParent )
			D3DXMatrixMultiply( &matWorld, &m_pFrame[ nCurrFrame ]->m_matAniTM, 
										   &m_pParent->m_matWorld );
		else
			matWorld = m_pFrame[ nCurrFrame ]->m_matAniTM;
		
//		m_pLastFrame = m_pFrame[ nCurrFrame ];
	}
	
	// 최종 변환된 월드매트릭스를 멤버에 넣어서 Render()에서 SetTransfrom함.
	m_matWorld = matWorld;

	// WorldTM = LocalTM * ParentTM;

	int		i;
	// 리커시브
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->SetupMatrices( 0 );
	}
*/
}


// 모든 노드를 리커시브하며 nCurrFrame에서 fSlerp의 보간을 한다
// 보간한 결과는 월드 매트릭스로 변환한다.
void	CGeoMObject :: SetupQuat( float fSlerp, D3DXMATRIX *pmRoot )
{
	D3DXMATRIX	matWorld;
	int		nCurrFrame = (int)m_pASE->m_fFrameCurrent;
	D3DXMATRIX	matPos, matRot, matScale, matAniTM, m1, m2;
	ASETM_ANIMATION	*pFrame, *pNextFrame;
	int		nNextFrame = 0;
	D3DXQUATERNION		qSlerp, qScaleSlerp;
	D3DXVECTOR3			vSlerp, vScaleSlerp;
	FLOAT det = m_fFlip;
	
	if( m_pFrame == NULL )		// 부모가 있을땐 나의AniTM과 부모의 최종변환 월드매트릭스를 곱한다.
	{	// 아예 프레임이 없는경우는 항상 LocalTM을 사용
		if( m_pParent )
			D3DXMatrixMultiply( &matWorld, &m_matLocalTM, &m_pParent->m_matWorld );
		else
			if( pmRoot )
				matWorld = m_matLocalTM * *pmRoot;
			else
				matWorld = m_matLocalTM;
	} else
	{
		pFrame = m_pFrame[ nCurrFrame ];

		// 보간
		nNextFrame = (nCurrFrame + 1);
		if( nNextFrame >= m_pASE->m_nFrameMax )		nNextFrame = 0;
		pNextFrame = m_pFrame[ nNextFrame ];			// 다음 키의 포인터 받는다

		D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qAni, &pNextFrame->m_qAni, fSlerp );	// 회전 보간
		D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNextFrame->m_vPos, fSlerp );			// 이동 보간
#ifdef __XSCALE
		// 스케일 보간
		D3DXQuaternionSlerp( &qScaleSlerp, &pFrame->m_qAniScale, &pNextFrame->m_qAniScale, fSlerp );	// scale 쿼터니온 보간
		D3DXVec3Lerp( &vScaleSlerp, &pFrame->m_vScaleFactor, &pNextFrame->m_vScaleFactor, fSlerp );		// scale factor 보간
#endif
		
		// matAniTM계산
		// 이동행렬 만듦
		D3DXMatrixTranslation( &matPos,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
		// 쿼터니온 회전키값을 회전행렬로 만듦
		D3DXMatrixRotationQuaternion( &matRot, &qSlerp );
#ifdef __XSCALE
		// 스케일 행렬 만듦 = S = scale rotation inverse * scale factor * scale rotation
		D3DXMatrixRotationQuaternion( &matScale, &qScaleSlerp );	// U(scale rotation)
		D3DXMatrixInverse( &m1, &det, &matScale );					// Ut(U's inverse)
		D3DXMatrixScaling( &m2, vScaleSlerp.x, vScaleSlerp.y, vScaleSlerp.z );	// K(scale factor)
		m1 *= m2;

		matAniTM = m1 * matScale;	// SCALE 매트릭스
//		matRot *= m_fFlip;
		matAniTM *= matRot;			// SCALE * ROTATION
//		matAniTM *= m_fFlip;
		matAniTM *= matPos;			// SCALE * ROT * POS
//		matAniTM *= m_fFlip;
#else
		D3DXMatrixMultiply( &matAniTM, &matRot, &matPos );		// 이동행렬 X 회전행렬 = 애니행렬
#endif
		

		if( m_pParent )
		{
			D3DXMatrixMultiply( &matWorld, &matAniTM, &m_pParent->m_matWorld );
//			matWorld._41 *= m_fFlip;
//			matWorld._42 *= m_fFlip;
//			matWorld._43 *= m_fFlip;
//			matWorld._44 *= m_fFlip;
		}
		else
			if( pmRoot )
				matWorld = matAniTM * *pmRoot;
			else
				matWorld = matAniTM;
		
	}
	
	// WorldTM = LocalTM * ParentTM;
	// 최종 변환된 월드매트릭스를 멤버에 넣어서 Render()에서 SetTransfrom함.
	m_matWorld = matWorld;

	int		i;
	// 리커시브
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->SetupQuat( fSlerp );
	}
}

//////////////////////////////////////////////////////////////

void	CGeoMObject :: WriteBON_Recursive( FILE *fp )
{
	int		i;
	int		nLen;

	nLen = strlen( m_strName ) + 1;		// 널문자까지 포함해서 저장
	fwrite( &nLen, 4, 1, fp );
	fwrite( m_strName, nLen, 1, fp );		// bone node 이름 저장
	fwrite( &m_matWorld,	sizeof(D3DXMATRIX), 1, fp );		// ObjectTM
	fwrite( &m_matInverseTM,sizeof(D3DXMATRIX), 1, fp );		// Inv NODE TM
	fwrite( &m_matLocalTM,	sizeof(D3DXMATRIX), 1, fp );		// LocalTM

	// 부모인덱스 저장
	int	nParentIdx = -1;
	if( m_pParent == NULL )
		fwrite( &nParentIdx, 4, 1, fp );			// 부모가 없는놈은 -1
	else
		fwrite( &m_pParent->m_nIdx, 4, 1, fp );		// 부모 인덱스를 저장
	
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->WriteBON_Recursive( fp );
	}
}

//
//	*.chr중 본의 계층구조를 Write하는 부분.
//
int		CGeoMObject :: WriteBON( FILE *fp )
{
	int		nNumNode = GetLength();
	DWORD	dwTemp = 0;
	// m_vEvent가 버전5   
	// m_vEvent개수 늘어난거 버전6    
	// 쌍칼땜에 LocalLH추가된거 버전 7
	int		nVer = 7;		// VER_BONE - 
	int		nSerial = xRand();			// 32비트형 시리얼 번호

	// 헤더 - 헤더는 별로 함수로 빠져야 한다.
	fwrite( &nVer, 4, 1, fp );			// version		
	fwrite( &nSerial, 4, 1, fp );		// 고유 번호
	// bound box size

	fwrite( &nNumNode, sizeof(int), 1, fp );
	WriteBON_Recursive( fp );

	return SUCCESS;
}

void	CGeoMObject :: WriteBone_Recursive( FILE *fp )
{
	int		i;

	fwrite( &m_matWorld, sizeof(D3DXMATRIX), 1, fp );		// 뼈대기본셋을 저장할땐 변환이 끝난 매트릭스를 넣으면 된다.

	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->WriteBone_Recursive( fp );
	}
}

void	CGeoMObject :: WriteBoneInv_Recursive( FILE *fp )
{
	int		i;

	fwrite( &m_matInverseTM, sizeof(D3DXMATRIX), 1, fp );		// 뼈대기본셋을 저장할땐 변환이 끝난 매트릭스를 넣으면 된다.

	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->WriteBoneInv_Recursive( fp );
	}
}

//
// 뼈대 개수와 기본 매트릭스 셋트를 기록한다.
//
void	CGeoMObject :: WriteBone( FILE *fp )
{
//	int		nNumNode = GetLength();

	WriteBone_Recursive( fp );			// 뼈대트리를 돌면서 matWorld만 저장.
	WriteBoneInv_Recursive( fp );		// 뼈대트리를 돌면서 matInverseTM만 저장.

}

// Bone정보만 저장
/*
int		CGeoMObject :: SaveBON( LPCTSTR strFileName )
{
	FILE	*fp;
	int		nNumNode = GetLength();
	DWORD	dwTemp = 0;
	int		nVer = 2;
	int		nSerial = xRand();			// 32비트형 시리얼 번호

	fp = fopen( strFileName, "wb" );
	if( fp == NULL )	
	{
		Error( "%s : 저장실패", strFileName );
		return FAIL;
	}


	fwrite( &nVer, 4, 1, fp );			// version
	fwrite( &nSerial, 4, 1, fp );		// 고유 번호
	// bound box size
//	fwrite( &g_nWidth, 4, 1, fp );
//	fwrite( &g_nHeight, 4, 1, fp );
//	fwrite( &g_nDepth, 4, 1, fp );
	fwrite( &dwTemp, 4, 1, fp );
	fwrite( &dwTemp, 4, 1, fp );
	fwrite( &dwTemp, 4, 1, fp );

	fwrite( &nNumNode, sizeof(int), 1, fp );
	SaveBON_Bone( fp );

	fclose(fp);
	return 1;
}
*/

// path의 m_vPos만 저장.
void	CGeoMObject :: SavePathRecv( FILE *fp )
{
	int		i;

	// m_pPath의 m_pFrame은 무조건 있다고 가정한다. m_pFrame이 없다면 m_pPath가 될 수 없다.
	for( i = 0; i < m_pASE->m_nFrameMax; i ++ )
	{
		fwrite( &(m_pFrame[i]->m_vPos), sizeof(D3DXVECTOR3), 1, fp );			// pos key
	}
}

void	CGeoMObject :: SaveBON_Ani( FILE *fp )
{
	int		i;
	int		nTemp;

	if( m_pFrame )
	{
		nTemp = 1;
		fwrite( &nTemp, 4, 1, fp );			// 프레임 있음
		for( i = 0; i < m_pASE->m_nFrameMax; i ++ )
		{
//			fwrite( &(m_pFrame[i]->m_matAniTM), sizeof(D3DXMATRIX), 1, fp );		// Ani TM
			fwrite( &(m_pFrame[i]->m_qAni), sizeof(D3DXQUATERNION), 1, fp );		// rot quaternion key
			fwrite( &(m_pFrame[i]->m_vPos), sizeof(D3DXVECTOR3), 1, fp );			// pos key
		}
	} else
	{
		nTemp = 0;
		fwrite( &nTemp, 4, 1, fp );			// 프레임 없음
		fwrite( &m_matLocalTM, sizeof(D3DXMATRIX), 1, fp );		// 애니메이션이 없는경우는 오브젝트의 LocalTM을 사용
	}

	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->SaveBON_Ani( fp );
	}
}

int		_nSize;

void	CGeoMObject :: GetFrameSize( void )
{
	int		i;
	
	if( m_pFrame )
		_nSize += m_pASE->m_nFrameMax;
#ifndef	__X0617
	else
		_nSize += 1;	// TM_ANIMATION에 m_mLocalTM이 따로 있으므로 프레임이 없는곳에선 메모리풀을 쓸필요가 없다.
#endif
	
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->GetFrameSize();
	}

}

//
//	모션 파일내에 뼈대구성정보 기록
//
void	CGeoMObject :: WriteBoneInfo( FILE *fp )
{
	int		i;
	int		nParentIdx;
	int		nLen;

	nLen = strlen( m_strName ) + 1;		// 널문자 포함해서 저장.
	fwrite( &nLen, 4, 1, fp );
	fwrite( m_strName, nLen, 1, fp );		// bone node 이름 저장
	fwrite( &m_matInverseTM, sizeof(D3DXMATRIX), 1, fp );		// Inv NODE TM
	fwrite( &m_matLocalTM, sizeof(D3DXMATRIX), 1, fp );			// LocalTM

	// 부모인덱스 저장
	nParentIdx = -1;
	if( m_pParent == NULL )
		fwrite( &nParentIdx, 4, 1, fp );			// 부모가 없는놈은 -1
	else
		fwrite( &m_pParent->m_nIdx, 4, 1, fp );		// 부모 인덱스를 저장
	
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->WriteBoneInfo( fp );		// 리커시브
	}
	
}

// Bone Animation정보 저장
int		CGeoMObject :: SaveANI( FILE *fp )
{
//	int		nNumNode = GetLength();
//	int		nVer = 9;					// VER_MOTION
//	int		nSerial = xRand();			// 32비트형 시리얼 번호
//	char	reserved[32];

//	memset( reserved, 0, 32 );
//	fwrite( &nVer, 4, 1, fp );
//	fwrite( &nSerial, 4, 1, fp );
//	fwrite( &m_fPerSlerp, sizeof(float), 1, fp );
//	fwrite( reserved, 32, 1, fp );		// 여유공간
//	fwrite( &nNumNode, sizeof(int), 1, fp );
//	fwrite( &m_nFrameMax, sizeof(int), 1, fp );
	
	// 뼈대구성 정보
//	WriteBoneInfo( fp );

	_nSize = 0;
	GetFrameSize();
	fwrite( &_nSize, 4, 1, fp );

	SaveBON_Ani( fp );

	return SUCCESS;
}

// TM Animation 부분만 저장.
int		CGeoMObject :: SaveTMAni( FILE *fp )
{
	int		i;
	int		bFrame;

	// this GEOMOBJECT만 프레임이 없을수도 있으므로 플래그를 달아둔다.
	if( m_pFrame == NULL )	bFrame = 0;
	else					bFrame = 1;
	fwrite( &bFrame, 4, 1, fp );
	
	if( bFrame == 0 )	// 프레임이 없으면 걍 리턴
		return FAIL;

	for( i = 0; i < m_pASE->m_nFrameMax; i ++ )
	{
//		fwrite( &(m_pFrame[i]->m_matAniTM), sizeof(D3DXMATRIX), 1, fp );		// Ani TM
		fwrite( &(m_pFrame[i]->m_qAni), sizeof(D3DXQUATERNION), 1, fp );		// rot quaternion key
		fwrite( &(m_pFrame[i]->m_vPos), sizeof(D3DXVECTOR3), 1, fp );			// pos key
	}

	return SUCCESS;
}


int		_WTMIdx;

void CGeoMObject :: SetWorldTransform( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX *matWorld )
{
	int		i;
	HRESULT	hr;

	hr = pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(_WTMIdx), &m_matWorld );		// 뼈대의 월드매트릭스를 행렬팔레트에 집어 넣는다.
	pd3dDevice->MultiplyTransform(D3DTS_WORLDMATRIX(_WTMIdx), matWorld);
	_WTMIdx ++;

	for( i = 0; i < m_nMaxChild; i ++ )
	{
		m_pChildList[i]->SetWorldTransform( pd3dDevice, matWorld );
	}
}

HRESULT		CGeoMObject :: Render( const D3DXMATRIX *mWorld )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	HRESULT		retv;
	
	if( m_pMesh == NULL )	return S_OK;

	D3DXMATRIX	m1;
	if( m_pMesh->m_pPhysiqueVertex )
		m1 = *mWorld;	// 피지크가 있는 메쉬는 자체 TM을 쓰지 않는다.
	else
		m1 = m_matWorld * *mWorld;
//	m1 = *mWorld;
	pd3dDevice->SetTransform( D3DTS_WORLD, &m1 );		// Set World Transform 

	retv = m_pMesh->Render();

	return retv;

}

void	CGeoMObject :: RenderBB( const D3DXMATRIX *mWorld )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	
	if( m_pMesh == NULL )	return;
	D3DXMATRIX	m1;
	m1 = m_matWorld * *mWorld;
	pd3dDevice->SetTransform( D3DTS_WORLD, &m1 );		// Set World Transform 

	g_Grp3D.RenderAABB( m_pMesh->m_vMin, m_pMesh->m_vMax, 0xffffffff );
}


//
// 트리를 훑어나가며 타입을 지정
void	CGeoMObject :: SetType_Recu( GMTYPE type )
{
	int		i;

	m_Type = type;

	for( i = 0; i < m_nMaxChild; i ++ )
		m_pChildList[i]->SetType_Recu( type );
}

void	CGeoMObject :: ClearNormal( void )
{
	if( m_pMesh )
		m_pMesh->ClearNormal();
}

D3DXVECTOR3 *CGeoMObject::IntersectRayVertex( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir )
{
	D3DXMATRIX	mInv;
	D3DXVECTOR3 v1, v2, v3;
	D3DXVECTOR3 *vRet;

	if( m_pMesh == NULL )	return NULL;
	// ray를 matWorld기준으로 바꿈.
	D3DXMatrixInverse( &mInv, NULL, &m_matWorld );
	D3DXVec3TransformCoord( &v1, &vRayOrig, &mInv );	// 레이원점은 걍 변환시켜도 됨.
	v3 = vRayOrig + vRayDir;							// RayDir은 벡터기땜에 절대좌표로 변환후 
	D3DXVec3TransformCoord( &v3, &v3, &mInv );			// 역변환 시키고
	v2 = v3 - v1;		// v2 : Inv(vRayDir)			// 다시 벡터로 바꿈.

	if( vRet = m_pMesh->IntersectRayVertex( pOut, v1, v2 ) )
	{
		D3DXVec3TransformCoord( pOut, pOut, &m_matWorld );
		return vRet;
	}
		
	return NULL;
	
}

//
// 본의 계층 구조를 돌면서 본이름을 버퍼에 담는다.
// 
char	*CGeoMObject :: GetBonesName_Recursive( char *pBuff )
{
	int		i;
	char	*pNew;
	
	strcpy( pBuff, m_strName );
	pNew = pBuff + (strlen(m_strName) + 1);  // "bip01" + NULL
	
	for( i = 0; i < m_nMaxChild; i ++ )
	{
		pNew = m_pChildList[i]->GetBonesName_Recursive( pNew );
	}

	return pNew;
}
