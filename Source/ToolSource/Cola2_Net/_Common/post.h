#ifndef __POST_H__
#define	__POST_H__

#include <map>
#include <list>
using	namespace	std;

#define	MAX_MAILTITLE	128
#define	MAX_MAILTEXT	256

class CItemElem;
class CMail
{
public:
	CMail();
	CMail( u_long idSender, CItemElem* pItemElem, int nGold, char* szTitle, char* szText );
	virtual	~CMail();

	enum	{	mail,	item,	gold	};

	void	Clear( void );
	void	Serialize( CAr & ar );
public:
	u_long	m_nMail;
	u_long	m_idSender;
	CItemElem*	m_pItemElem;
	DWORD	m_nGold;
	char	m_szTitle[MAX_MAILTITLE];
	char	m_szText[MAX_MAILTEXT];

	static	u_long	s_nMail;
};

class CMailBox : public vector<CMail*>
{
public:
	CMailBox();
	CMailBox( u_long idReceiver );
	virtual	~CMailBox();
	u_long	AddMail( CMail* pMail );
	void	Serialize( CAr & ar );
	CMail*	GetMail( u_long nMail );
	BOOL	RemoveMail( u_long nMail );
	BOOL	RemoveMailItem( u_long nMail );
	BOOL	RemoveMailGold( u_long nMail );
	void	Clear( void );
	static	CMailBox*	GetInstance( void );
private:
	vector<CMail*>::iterator Find( u_long nMail );

public:
	u_long	m_idReceiver;
};

class CPost
{
public:
//	Constructions
	CPost();
	virtual	~CPost();
	void	Clear( void );
//	Operations
	u_long	AddMail( u_long idReceiver, u_long idSender, CItemElem* pItemElem, int nGold, char* szTitle, char* szText );
	u_long	AddMail( u_long idReceiver, CMail* pMail );
	CMailBox*	GetMailBox( u_long idReceiver );
	BOOL	AddMailBox( CMailBox* pMailBox );
	void	Serialize( CAr & ar );
	static	CPost*	GetInstance( void );
private:
	map< u_long, CMailBox* >	m_mapMailBox;
};
#endif	// __POST_H__