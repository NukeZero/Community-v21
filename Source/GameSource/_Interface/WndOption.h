
#pragma once

#include "WndOptionGame.h"	 // 게임 옵션

class CWndOption : public CWndNeuz 
{ 
public: 
	CWndOption(); 
	~CWndOption(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

//////////////////////////////////////////////////////////////////////////////////////
// Sound
//

class CWndOptSound : public CWndNeuz 
{ 
public: 
	CTexture         m_Texture;
	CTexture         m_TexturePt;
	BOOL			 m_bLButtonClick;
	BOOL			 m_bLButtonClick2;
	int				 m_nStep[2];
	CWndOptSound(); 
	~CWndOptSound(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point );
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point );
	CPoint GetStepPos( int nStep, int nWidth, int nDivision );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	
}; 
class CWndOptVideo : public CWndNeuz 
{ 
public: 
	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point );
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point );
	void GetRangeSlider( DWORD dwWndId, int& nStep, CPoint point, int nDivision);
	int GetSliderStep( DWORD dwWndId, int &nStep, CPoint point, int nDivision);
	CPoint GetStepPos( int nStep, int nWidth, int nDivision );
	CWndOptVideo(); 
	~CWndOptVideo(); 

	//sun: 밝기, 감마, 명암 조절 Neuz
	int				 m_nStep[5];
	int				 m_nBrightTable[11];

	BOOL			 m_bLButtonClick3;
	BOOL			 m_bLButtonClick4;
	BOOL			 m_bLButtonClick5;

	CTexture         m_Texture;
	CTexture         m_TexturePt;
	BOOL			 m_bLButtonClick;
	BOOL			 m_bLButtonClick2;

	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	
	//gmpbigsun(20101208) : 디폴트 기능 추가
	void UpdateData( );
}; 



class CWndOptMyInfo : public CWndNeuz 
{ 
public: 
	CWndOptMyInfo(); 
	~CWndOptMyInfo(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndOptWindow : public CWndNeuz 
{ 
public: 
	CWndOptWindow(); 
	~CWndOptWindow(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 




//---------------------------------------------------------------------------------------------
// 메뉴 ( 설정 윈도우 )
//---------------------------------------------------------------------------------------------
class CWndTotalOption : public CWndNeuz
{

public:


						CWndTotalOption( void ); 
						~CWndTotalOption( void ); 

	CWndOptionGame		m_OptTabGame;
	CWndOptVideo		m_OptTabVideoSnd;

	bool				m_bRollBack;				
	COption				m_kOrgOption;			

	void				UpdateDataAll( void );
	void				ResetInfoPang( void );

	virtual BOOL 		Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL 		OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void 		OnInitialUpdate(); 


private:


	void				SaveOriginalData( void );
	void				LoadOriginalData( void );


};
//---------------------------------------------------------------------------------------------
