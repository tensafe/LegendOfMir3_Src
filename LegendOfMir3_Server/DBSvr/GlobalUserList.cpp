

#include "stdafx.h"
#include "globalUserList.h"
#include "..\Def\_OrzEx\error.h"


static CGlobalUserList s_globalUserList;


CGlobalUserList::CGlobalUserList()
: m_fixedMemory( GLOBALLIST_MAXLIST )
{
	if ( !m_listUser.InitHashTable( GLOBALLIST_MAXBUCKET, IHT_ROUNDUP ) )
		throw CError( "CGlobalUserList::CGlobalUserList 해쉬테이블 생성 실패" );

	m_listUser.SetGetKeyFunction( __cbGetKey );
}


CGlobalUserList::~CGlobalUserList()
{
	m_listUser.UninitHashTable( false );
}


bool CGlobalUserList::Insert( char *pID, char *pIP )
{
	if ( strlen( pID ) >= CUserInfo::MAXID || strlen( pIP ) >= CUserInfo::MAXIP )
		return false;

	CUserInfo *pUser = m_fixedMemory.Alloc();
	if ( !pUser )
		return false;

	strcpy( pUser->szID, pID );
	strcpy( pUser->szIP, pIP );

	Lock();
	if ( !m_listUser.Insert( pUser ) )
	{
		Unlock();
	
		m_fixedMemory.Free( pUser );
		return false;
	}
	Unlock();

	return true;
}


CGlobalUserList::CUserInfo * CGlobalUserList::Search( char *pID )
{
	Lock();
	CUserInfo *pUser = m_listUser.SearchKey( pID );
	Unlock();

	return pUser;
}


void CGlobalUserList::Remove( char *pID )
{
	Lock();
	m_fixedMemory.Free( m_listUser.Remove( (CUserInfo *) pID ) );
	Unlock();
}


char * CGlobalUserList::__cbGetKey( CUserInfo *pUser )
{
	return pUser->szID;
}




CGlobalUserList * GetGlobalUserList()
{
	return &s_globalUserList;
}