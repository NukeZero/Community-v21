// stdafx.cpp : source file that includes just the standard includes
//	Beast.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CWorldMng   g_WorldMng;
CProject    prj;         
CMainFrame* g_pMainFrame;
CWorldView* g_pWorldView;
C2DRender   g_2DRender;
CULLINFO    g_cullinfo;
CDlgContinent g_kDlgContinent;
CDlgLight24 g_kDlg24Light;
CDlgCalcWorldmap g_kDlgCalcWorlmap;

const int OLD_MPU = 4;

int			g_MPU = OLD_MPU;

const char* DEFAULT_FNAME = _T("default");



 