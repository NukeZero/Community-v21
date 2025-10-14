#ifndef __HELPER_WEB_BOX_H__
#define __HELPER_WEB_BOX_H__

class CHelperWebBox
{
public:
	enum { HELPER_WEB_SIZE_NORMAL_X = 790, HELPER_WEB_SIZE_NORMAL_Y = 570 };
	enum { HELPER_WEB_SIZE_LARGE_X = 1014, HELPER_WEB_SIZE_LARGE_Y = 738 };
    enum 
	{
		TYPE_A_X = 690,
		TYPE_A_Y = 560,
	};

public:
	CHelperWebBox( void );
	virtual ~CHelperWebBox( void );

public:
	static CHelperWebBox* GetInstance( void );

public:
	void Initialize( void );
	void Process( void );
	void Release( void );

public:
	void InitializeWebInformation( void );
	void Close( void );

public:
	void GoFirstPage( void );

public:
	const CString& GetHelperWebAddress( void ) const;
	const HWND& GetParentHandle( void ) const;
	const HWND& GetDialogHandle( void ) const;
	const HWND& GetDialogBoxHandle( void ) const;

	void AutoPostion( );	//gmpbigsun : 웹 시작좌표 자동조정
	void SetSize( int cx, int cy );
	void OnSize( int cx, int cy );

public:
	CString m_strHelperWebAddress;
	HINSTANCE m_hInstance;
	HWND m_hParent;
	HWND m_hDialog;
	HWND m_hDialogBox;
	CRect m_rectHelperWeb;

	int m_iSizeX;
	int m_iSizeY;
};


#endif // __HELPER_WEB_BOX_H__