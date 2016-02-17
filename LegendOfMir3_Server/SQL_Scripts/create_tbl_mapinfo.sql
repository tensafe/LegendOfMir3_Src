/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_MAPINFO
GO

/* 
	Map Information
*/

CREATE TABLE TBL_MAPINFO
(
	FLD_MAPFILENAME	CHAR(14)		NOT NULL,
	FLD_MAPNAME		VARCHAR(40)		NOT NULL,
	FLD_SERVERINDEX	SMALLINT		NOT NULL,
	FLD_ATTRIBUTE		INT			NULL,
	FLD_RECALLMAPFNAME	CHAR(14)		NULL,
	FLD_DESCRIPTION	VARCHAR(100)		NULL,
)
GO

ALTER TABLE TBL_MAPINFO ADD CONSTRAINT PK_MAPINFO PRIMARY KEY (FLD_MAPFILENAME)
GO
