

#pragma once


#define GLOBALLIST_MAXLIST		20000
#define GLOBALLIST_MAXBUCKET	1000


#include "..\Def\_OrzEx\syncobj.h"
#include "..\Def\_OrzEx\indexmap.h"
#include "..\Def\_OrzEx\fsa.h"


class CGlobalUserList : public CIntLock
{
public:
	class CUserInfo
	{
	public:
		enum { MAXID = 32, MAXIP = 32 };

		char szID[MAXID];
		char szIP[MAXIP];

	public:
		CUserInfo() { szID[0] = '\0'; szIP[0] = '\0'; }
	};

public:
	CIndexMap< CUserInfo >				m_listUser;
	CFixedSizeAllocator< CUserInfo >	m_fixedMemory;	

public:
	CGlobalUserList();
	virtual ~CGlobalUserList();

	bool		Insert( char *pID, char *pIP );
	CUserInfo *	Search( char *pID );
	void		Remove( char *pID );

protected:
	static char * __cbGetKey( CUserInfo *pUser );
};


typedef CGlobalUserList::CUserInfo CGlobalUserInfo;


CGlobalUserList * GetGlobalUserList();
