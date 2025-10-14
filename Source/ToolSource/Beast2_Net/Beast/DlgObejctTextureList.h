#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ObjectTextureView.h"


// DlgObejctTextureList 대화 상자입니다.

class CObj;

class DlgObejctTextureList : public CDialog
{
	DECLARE_DYNAMIC(DlgObejctTextureList)

public:
	DlgObejctTextureList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgObejctTextureList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OBJECT_TEXTURE };

private:
	typedef struct E_TYPE{ enum ENUM {
		NUM,
		TEXTURE_NAME,
		_MAX_,
	};  };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()


public:
	void BindObjectTexture( CObj* pObj );
	void UpdateList();
	void UpdateTexture();

	ObjectTextureView& GetTextureDataContainer() { return m_ObjectTextureContainer; }

private:
	void ListItemsClipboardText();


private:
	ObjectTextureView m_ObjectTextureContainer;
	CString m_Object_Name;
	CString m_Select_Full_Texture_Name;
	CString m_Select_Texture_FileName;
	CListCtrl m_ListCtrl_Texture_List;
	CButton m_Button_Texture_Update;
	CStatic m_Static_Object_Name;
	CButton m_Button_Folder_Open;

	ObjectTextureView::GMOBJECT_INFO*  m_pSelectGmObject_info;

	int m_nSelectIndex;

public:
	afx_msg void OnLvnItemchangedListTextureList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTextureUpdate();
	afx_msg void OnBnClickedButtonFolderOpen();
	
};
