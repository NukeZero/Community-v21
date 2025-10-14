#include "stdafx.h"
#include "CommandBeast.h"
#include "..\beast\worldDoc.h"
#include "..\beast\worldview.h"

#ifdef __CSC_UNDO_REDO

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Terrain)
//////////////////////////////////////////////////////////////////////////

FLOAT TerrainGetElemFunc(TERRAINCOORD terraincoord)
{
	FLOAT rtnval = 0.0f;
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
		rtnval = pBeastView->m_world.GetLandHeight( (float)( terraincoord.m_Point.x ), (float)( terraincoord.m_Point.y ) );

	return rtnval;
}

void TerrainExcuteFunc(TERRAINCOORD terraincoord, FLOAT fHeight)
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
		pBeastView->m_world.SetHeight(terraincoord.m_Point, fHeight);
}

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Texture Change)
//////////////////////////////////////////////////////////////////////////

int TexChgGetElemFunc(TEXLANDLAYER texlandlayer)
{
	int rtnval = 0;
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
	{
		if(texlandlayer.m_nLandScape > -1)
		{
			for(int i=0; i<pBeastView->m_world.m_apLand[texlandlayer.m_nLandScape]->m_aLayer.GetSize(); i++)
			{
				CLandLayer* pLayer = (CLandLayer*)pBeastView->m_world.m_apLand[texlandlayer.m_nLandScape]->m_aLayer.GetAt(i);
				if(texlandlayer.m_nTex == pLayer->m_nTex)
				{
					rtnval = pLayer->m_pTempLightMap[texlandlayer.m_nLayerTemp] >> 24;
					i = pBeastView->m_world.m_apLand[texlandlayer.m_nLandScape]->m_aLayer.GetSize();
				}
			}
		}
	}

	return rtnval;
}

void TexChgExcuteFunc(TEXLANDLAYER texlandlayer, int nAlpha)
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
	{
		CLandLayer* pLandLayer = NULL;
		CLandscape* pLandScape = pBeastView->m_world.m_apLand[texlandlayer.m_nLandScape];

		if(pLandScape && texlandlayer.m_nLandScape > -1)
		{
			pLandLayer = pLandScape->AddLayerBeast( texlandlayer.m_nTex );

			if(pLandLayer)
				pLandLayer->m_pTempLightMap[texlandlayer.m_nLayerTemp] = (pLandLayer->m_pTempLightMap[texlandlayer.m_nLayerTemp] & 0x00ffffff) | ( nAlpha << 24 );

			int px = texlandlayer.m_nX / ( PATCH_SIZE - 1 );
			int pz = texlandlayer.m_nZ / ( PATCH_SIZE - 1 );

			pLandScape->SetLandMarkBeast( px, pz, texlandlayer.m_nTex, FALSE );
			pLandScape->SetDirtyPatch( px * PATCH_SIZE, pz * PATCH_SIZE );
			if( px * ( PATCH_SIZE - 1 ) == texlandlayer.m_nX ) 
			{
				pLandScape->SetDirtyPatch( ( px - 1 ) * PATCH_SIZE, pz * PATCH_SIZE );
				if( pz * ( PATCH_SIZE - 1 ) == texlandlayer.m_nZ )
					pLandScape->SetDirtyPatch( ( px - 1 ) * PATCH_SIZE, ( pz - 1 ) * PATCH_SIZE );
			}
			if( pz * ( PATCH_SIZE - 1 ) == texlandlayer.m_nZ ) 
			{
				pLandScape->SetDirtyPatch( px * PATCH_SIZE, ( pz - 1 ) * PATCH_SIZE );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Water Change)
//////////////////////////////////////////////////////////////////////////

WATERLAYER WaterChgGetElemFunc(TEXLANDLAYER texwaterlayer)
{
	WATERLAYER rtnval;
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
	{
		CLandscape* pLandScape = pBeastView->m_world.m_apLand[texwaterlayer.m_nLandScape];
		if(pLandScape && texwaterlayer.m_nLandScape > -1)
		{
			rtnval.m_nTex = pLandScape->GetWaterTexBeast(texwaterlayer.m_nX, texwaterlayer.m_nZ);
			rtnval.m_dwHeight = pLandScape->GetWaterHeightBeast(texwaterlayer.m_nX, texwaterlayer.m_nZ);
		}
	}

	return rtnval;
}

void WaterChgExcuteFunc(TEXLANDLAYER texwaterlayer, WATERLAYER fWater)
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
	{
		CLandscape* pLandScape = pBeastView->m_world.m_apLand[texwaterlayer.m_nLandScape];

		if(pLandScape && texwaterlayer.m_nLandScape > -1)
			pLandScape->SetWaterHeightBeast( texwaterlayer.m_nX, texwaterlayer.m_nZ, fWater.m_dwHeight, fWater.m_nTex );
	}
}

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Color Change)
//////////////////////////////////////////////////////////////////////////

DWORD ColorChgGetElemFunc(TEXLANDLAYER texcolorlayer)
{
	DWORD rtnval = 0;
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
	{
		int nTex = 0;
		if(texcolorlayer.m_nLandScape > -1 && texcolorlayer.m_nTex > -1)
		{
			//gmpbigsun(100316) : 랜드스케이프 경계면에서 배열의 overflow가 발생한다.!!  
			// 이경우 다른 래이어를 쓰는 랜드스케이프의 경계면 작업시 배열 최대값을 검사해서 가장 높은 레이어의 값을 가지게 했다.

			CLandscape* pScape = pBeastView->m_world.m_apLand[texcolorlayer.m_nLandScape];
			int iSize = pScape->m_aLayer.GetSize();
			nTex = texcolorlayer.m_nTex;
			if( nTex > ( iSize - 1 ) )
				nTex = iSize - 1;

			CLandLayer* pLayer = (CLandLayer*)pScape->m_aLayer[nTex];
			if(pLayer)
				rtnval = pLayer->m_pTempLightMap[texcolorlayer.m_nLayerTemp];
		}
	}

	return rtnval;
}

void ColorChgExcuteFunc(TEXLANDLAYER texcolorlayer, DWORD dwColor)
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();

	if(pBeastView)
	{
		CLandscape* pLandScape = pBeastView->m_world.m_apLand[texcolorlayer.m_nLandScape];
		if(pLandScape && texcolorlayer.m_nLandScape > -1 && texcolorlayer.m_nTex > -1)
		{
			CLandLayer* pLayer;
			pLayer = (CLandLayer*)pLandScape->m_aLayer[texcolorlayer.m_nTex];

			if(pLayer)
			{
				pLayer->m_pTempLightMap[texcolorlayer.m_nLayerTemp] = dwColor | (pLayer->m_pTempLightMap[texcolorlayer.m_nLayerTemp] & 0xff000000);

				int px = texcolorlayer.m_nX / ( PATCH_SIZE - 1 );
				int pz = texcolorlayer.m_nZ / ( PATCH_SIZE - 1 );

				pLandScape->SetLandMarkBeast( px, pz, texcolorlayer.m_nTex, FALSE );
				pLandScape->SetDirtyPatch( px * PATCH_SIZE, pz * PATCH_SIZE );
				if( px * ( PATCH_SIZE - 1 ) == texcolorlayer.m_nX ) 
				{
					pLandScape->SetDirtyPatch( ( px - 1 ) * PATCH_SIZE, pz * PATCH_SIZE );
					if( pz * ( PATCH_SIZE - 1 ) == texcolorlayer.m_nZ )
						pLandScape->SetDirtyPatch( ( px - 1 ) * PATCH_SIZE, ( pz - 1 ) * PATCH_SIZE );
				}
				if( pz * ( PATCH_SIZE - 1 ) == texcolorlayer.m_nZ ) 
				{
					pLandScape->SetDirtyPatch( px * PATCH_SIZE, ( pz - 1 ) * PATCH_SIZE );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Object Move)
//////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 ObjMoveGetElemFunc(CObj* pObj)
{
	D3DXVECTOR3 rtnval(0.0f, 0.0f, 0.0f);

	if(pObj && IsValidObj(pObj))
		rtnval = pObj->GetPos();

	return rtnval;
}

void ObjMoveExcuteFunc(CObj* pObj, D3DXVECTOR3 vecPos)
{
	if(pObj && IsValidObj(pObj))
		pObj->SetPos(vecPos);
}

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Object Scale)
//////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 ObjScaleGetElemFunc(CObj* pObj)
{
	D3DXVECTOR3 rtnval(0.0f, 0.0f, 0.0f);

	if(pObj && IsValidObj(pObj))
		rtnval = pObj->GetScale();

	return rtnval;
}

void ObjScaleExcuteFunc(CObj* pObj, D3DXVECTOR3 vecScale)
{
	if(pObj && IsValidObj(pObj))
		pObj->SetScale(vecScale);
}

//////////////////////////////////////////////////////////////////////////
// 사용자용 커맨드 클래스 함수 (Object Rotate)
//////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 ObjRotateGetElemFunc(CObj* pObj)
{
	D3DXVECTOR3 rtnval(0.0f, 0.0f, 0.0f);

	if(pObj && IsValidObj(pObj))
	{
		rtnval.x = pObj->GetAngleX();
		rtnval.y = pObj->GetAngle();
		rtnval.z = pObj->GetAngleZ();
	}

	return rtnval;
}

void ObjRotateExcuteFunc(CObj* pObj, D3DXVECTOR3 vecAngle)
{
	if(pObj && IsValidObj(pObj))
	{
		pObj->SetAngleX(vecAngle.x);
		pObj->SetAngle(vecAngle.y);
		pObj->SetAngleZ(vecAngle.z);
	}
}

//////////////////////////////////////////////////////////////////////////
// 비스트 커맨드 매니저
//////////////////////////////////////////////////////////////////////////
CCommandMgrBeast::CCommandMgrBeast() : m_nLandScape(-1)
{
}

CCommandMgrBeast::~CCommandMgrBeast()
{
}

CCommandMgrBeast* CCommandMgrBeast::GetInstance()
{	
	static CCommandMgrBeast sCommandMgr;
	return &sCommandMgr;
};

void CCommandMgrBeast::EndCommand()
{
	int nCurCommandType = GetCurCommandType();
	if(nCurCommandType)
	{
		switch(nCurCommandType)
		{
			case CCommandMgrBeast::TERRAIN_HEIGHT_CHG:
				{
					CUserCommand<TERRAINCOORD, FLOAT>* pChgTerrainHeight = new CUserCommand<TERRAINCOORD, FLOAT>(TerrainGetElemFunc, TerrainExcuteFunc, TerrainExcuteFunc);
					pChgTerrainHeight->Initialize(nCurCommandType, &m_mapOldTerrainHeigth);
					InsertCommand(pChgTerrainHeight);
					m_mapOldTerrainHeigth.clear();
				}
				break;
			case CCommandMgrBeast::TERRAIN_TEX_CHG:
				{
					CUserCommand<TEXLANDLAYER, int>* pChgTexLayer = new CUserCommand<TEXLANDLAYER, int>(TexChgGetElemFunc, TexChgExcuteFunc, TexChgExcuteFunc);
					pChgTexLayer->Initialize(nCurCommandType, &m_mapOldTexLandLayer);
					InsertCommand(pChgTexLayer);
					m_mapOldTexLandLayer.clear();
				}
				break;
			case CCommandMgrBeast::TERRAIN_WATER_CHG:
				{
					CUserCommand<TEXLANDLAYER, WATERLAYER>* pChgWaterTexLayer = new CUserCommand<TEXLANDLAYER, WATERLAYER>(WaterChgGetElemFunc, WaterChgExcuteFunc, WaterChgExcuteFunc);
					pChgWaterTexLayer->Initialize(nCurCommandType, &m_mapOldWaterTexLayer);
					InsertCommand(pChgWaterTexLayer);
					m_mapOldWaterTexLayer.clear();
				}
				break;
			case CCommandMgrBeast::TERRAIN_COLOR_CHG:
				{
					CUserCommand<TEXLANDLAYER, DWORD>* pChgTexColorLayer = new CUserCommand<TEXLANDLAYER, DWORD>(ColorChgGetElemFunc, ColorChgExcuteFunc, ColorChgExcuteFunc);
					pChgTexColorLayer->Initialize(nCurCommandType, &m_mapOldTexColorLayer);
					InsertCommand(pChgTexColorLayer);
					m_mapOldTexColorLayer.clear();
				}
				break;
			case CCommandMgrBeast::OBJ_MOVE:
				{
					CUserCommand<CObj*, D3DXVECTOR3>* pObjMove = new CUserCommand<CObj*, D3DXVECTOR3>(ObjMoveGetElemFunc, ObjMoveExcuteFunc, ObjMoveExcuteFunc);
					pObjMove->Initialize(nCurCommandType, &m_mapOldObjMove);
					InsertCommand(pObjMove);
					m_mapOldObjMove.clear();
				}
				break;
			case CCommandMgrBeast::OBJ_SCALE:
				{
					CUserCommand<CObj*, D3DXVECTOR3>* pObjScale = new CUserCommand<CObj*, D3DXVECTOR3>(ObjScaleGetElemFunc, ObjScaleExcuteFunc, ObjScaleExcuteFunc);
					pObjScale->Initialize(nCurCommandType, &m_mapOldObjScale);
					InsertCommand(pObjScale);
					m_mapOldObjScale.clear();			
				}
				break;
			case CCommandMgrBeast::OBJ_ROTATE:
				{
					CUserCommand<CObj*, D3DXVECTOR3>* pObjRotate = new CUserCommand<CObj*, D3DXVECTOR3>(ObjRotateGetElemFunc, ObjRotateExcuteFunc, ObjRotateExcuteFunc);
					pObjRotate->Initialize(nCurCommandType, &m_mapOldObjRotate);
					InsertCommand(pObjRotate);
					m_mapOldObjRotate.clear();
				}
				break;
		}
	}
}

#endif //__CSC_UNDO_REDO