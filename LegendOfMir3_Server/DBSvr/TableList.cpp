

#include "stdafx.h"
#include "tablelist.h"
#include "../def/_orzex/stringex.h"


static CTblStartPoint s_tblStartPoint;


CTblStartPoint::CTblStartPoint()
{
	m_list.SetCompareFunction( __cbCompare, NULL );
}


CTblStartPoint::~CTblStartPoint()
{
	m_list.ClearAll();
}


bool CTblStartPoint::Init( CConnection *conn )
{
	CRecordset *rec = conn->CreateRecordset();
	if ( !rec )
		return false;
	
	if ( rec->Execute( "SELECT * FROM TBL_STARTPOINT" ) )
	{
		while ( rec->Fetch() )
		{
			TABLE *node = new TABLE;
			if ( !node )
				break;

			node->index	= atoi( rec->Get( "FLD_INDEX" ) );
			strcpy( node->mapName, rec->Get( "FLD_MAPNAME" ) );
			node->posX	= atoi( rec->Get( "FLD_POSX" ) );
			node->posY	= atoi( rec->Get( "FLD_POSY" ) );
			strcpy( node->desc, rec->Get( "FLD_DESCRIPTION" ) );

			_trim( node->mapName );
			_trim( node->desc );

			m_list.Insert( node );
		}
	}

	conn->DestroyRecordset( rec );

	return true;
}


CTblStartPoint::TABLE * CTblStartPoint::Get( char *mapName )
{
	return m_list.Search( (TABLE *) mapName );	
}


int CTblStartPoint::__cbCompare( void *arg, TABLE *n1, TABLE *n2 )
{
	return strcmp( n1->mapName, n2->mapName );
}




CTblStartPoint * GetTblStartPoint()
{
	return &s_tblStartPoint;
}