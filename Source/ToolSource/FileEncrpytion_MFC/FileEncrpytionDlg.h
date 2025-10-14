// FileEncrpytionDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


#define MAKEINT64(a, b)		((__int64)(((DWORD)((DWORD_PTR)(a) & 0xffffffff)) | ((__int64)((DWORD)((DWORD_PTR)(b) & 0xffffffff))) << 32))

// CFileEncrpytionDlg 대화 상자
class CFileEncrpytionDlg : public CDialog
{
// 생성
public:
	CFileEncrpytionDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_FILEENCRPYTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnDropFiles( HDROP hDropInfo );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	DECLARE_MESSAGE_MAP()
public:
	FLConfigFileEncryption	m_xConfigFileEncryption;
	CStatic m_staticConfigFileName;
	CStatic m_staticEncryption;
	CStatic m_staticDecryption;
	afx_msg void OnStnClickedStaticFileName();
	afx_msg void OnBnClickedBtnConfigFileOpen();
	afx_msg void OnBnClickedBtnEncryption();
	afx_msg void OnBnClickedBtnDecryption();
	afx_msg void OnBnClickedBtnOpenFolder();

	CEdit m_editString;
	CEdit m_editEncryptionData;
	CEdit m_editDecryptionData;
	CStatic m_staticEncryptionOrgData;
	CStatic m_staticDecryptionOrgData;
	afx_msg void OnBnClickedBtnEncryptionString();
	afx_msg void OnBnClickedBtnDecryptionString();


	FLListFileEncryption	m_xListFileEncryption;
	CStatic m_staticListFileName;
	CStatic m_staticDecryptionListFile;
	afx_msg void OnBnClickedBtnListFileOpen();
	afx_msg void OnBnClickedBtnViewDecryptionListfile();
	afx_msg void OnBnClickedBtnDecryptionListfile();
	afx_msg void OnBnClickedBtnViewOpenedConfigfile();

	TCHAR	m_szResFileName[MAX_PATH];
	CStatic m_staticResFileName;
	afx_msg void OnBnClickedBtnResFileOpen();
	afx_msg void OnBnClickedBtnSplitResfile();
	afx_msg void OnBnClickedBtnEncryptionMd5();
	CEdit m_editMD5Data;
	CEdit m_editEncryptionMD5Data;
};
