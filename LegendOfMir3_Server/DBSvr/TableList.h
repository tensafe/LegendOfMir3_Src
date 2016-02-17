

#pragma once


#include "../def/dbmgr.h"


class CTblStartPoint
{
public:
	struct TABLE
	{
		char	mapName[15];	// search index
		int		index;			// sequence id
		int		posX;
		int		posY;
		char	desc[101];
	};

	CList< TABLE >	m_list;

public:
	CTblStartPoint();
	virtual ~CTblStartPoint();

	bool Init( CConnection *conn );
	TABLE * Get( char *mapName );

protected:
	static int __cbCompare( void *arg, TABLE *n1, TABLE *n2 );
};


CTblStartPoint * GetTblStartPoint();