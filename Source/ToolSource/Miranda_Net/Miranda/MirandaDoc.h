// MirandaDoc.h : interface of the CMirandaDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIRANDADOC_H__86539A93_8D04_4275_A517_B9C02F2A06AD__INCLUDED_)
#define AFX_MIRANDADOC_H__86539A93_8D04_4275_A517_B9C02F2A06AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CSelected
{

public:
	int		nID;
	int		nType;

	CSelected() { nID = nType = 0; };
	~CSelected(){};
	void SetSelected(int ID, int Type) { nID = ID; nType = Type;};

};


class CMirandaDoc : public CDocument
{
protected: // create from serialization only
	CMirandaDoc();
	DECLARE_DYNCREATE(CMirandaDoc)

// Attributes
public:

	CSfxMgr		m_SfxMgr;
	CObjMgr		m_ObjMgr;
	Terrain*	m_Terrain;
	CSkyBox		m_SkyBox;
	
	// 현재 선택된 오브젝트나 아이템 
	CSelected	m_Selected;
// Operations
public:
	int	Setup(LPDIRECT3DDEVICE9 Device);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirandaDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMirandaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;


#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMirandaDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIRANDADOC_H__86539A93_8D04_4275_A517_B9C02F2A06AD__INCLUDED_)
