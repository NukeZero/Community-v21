
#pragma once 

//---------------------------------------------------------------------------------------------
// µ·
//---------------------------------------------------------------------------------------------
class CWndGold : public CWndButton
{

public:

					CWndGold( void );
	virtual			~CWndGold( void );

	virtual	void	OnInitialUpdate( void );
	virtual void	OnLButtonDown(UINT nFlags, CPoint point);


private:


	CTexture*		m_pTexture;

};
