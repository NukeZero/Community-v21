#ifndef __CCLASS_H
#define __CCLASS_H

#include <vector>

class WCSTR
{
public:	
	wchar_t m_wcstr[2048];
};

class NATION_STR
{
public:
	CString m_strTitle;
	wchar_t awszMessage [ LANG_MAX ][ 2048 ];
};

class NATION_STR_EX : public NATION_STR
{
public:
	CString m_strIndex;
};

class NATION_STATE
{
public:
	CString		    m_strIndex;
	NATION_STR		m_strDesc;
};

typedef vector<WCSTR>				VEC_STR;
typedef vector<NATION_STATE>		VEC_STR_STATE;
typedef vector<NATION_STR>			VEC_NATION_STR;

class NATION_STATE_EX : public NATION_STATE
{
public:
	VEC_NATION_STR  m_vecStateSet;
	VEC_STR			m_vecQuestItem;
};


typedef vector<NATION_STR_EX>		VEC_STR_EX;
typedef vector<NATION_STATE_EX>		VEC_STR_STATE_EX;

//////////////////////////////////////////////////////////////////////////////////
class PROPQUEST
{
public:
	CString				m_strNumber;
	NATION_STR			m_Title;
	NATION_STR			m_NpcName;
	VEC_STR				m_vecSetting;
	NATION_STR			m_PatrolZoneName;
	VEC_STR_EX			m_vecSetDialog;
	VEC_STR_STATE_EX	m_vecState;
};

typedef vector<PROPQUEST>		VEC_PROPQUEST;


//////////////////////////////////////////////////////////////////////////////////
class CHARACTER_INC
{
public:
	CString	m_strNPCName;
	
	VEC_STR m_strString;
	
	NATION_STR     m_strSetName;
	VEC_STR_EX	   m_vecAddVendorSlot;
};

typedef vector< CHARACTER_INC >		VEC_CHARACTER_INC;




///////////////////////////////////////////////////////////////////////
class LANG_STR
{
public:
	wchar_t awszMessage [ LANG_MAX ][ 2048 ];
#ifdef _DEBUG
	CString m_str[LANG_MAX];
#endif //_DEBUG
};
typedef vector< LANG_STR >		VEC_LANG_STR;
///////////////////////////////////////////////////////////////////////
class HELP_INC
{
public:
	LANG_STR		   m_str;
	VEC_LANG_STR	   m_vecstr;
};
typedef vector< HELP_INC >		VEC_HELP_STR;
///////////////////////////////////////////////////////////////////////
class LANG_STR_EX : public LANG_STR
{
public:
	BOOL m_bSkip;
	BOOL m_bIsHeader;
	int  m_nBraces; // 0:없음 1:열기 2:닫기
};
typedef vector< LANG_STR_EX >		VEC_LANG_STR_EX;
///////////////////////////////////////////////////////////////////////
class BRACES
{
public:
	int  m_nLine;
	int  m_nBraces; // 0:없음 1:열기 2:닫기
};
typedef vector< BRACES >		VEC_BRACES;




#endif