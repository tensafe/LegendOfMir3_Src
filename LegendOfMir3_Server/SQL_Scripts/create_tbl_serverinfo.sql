/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_SERVERINFO
GO

/* 
	Server Information Table 
*/

CREATE TABLE TBL_SERVERINFO
(
	FLD_SERVERIDX		INT		IDENTITY(0, 1),
	FLD_SERVERNAME	NCHAR(40)	NOT NULL,
	FLD_SERVERIP		NCHAR(20)	NOT NULL
)
GO

ALTER TABLE TBL_SERVERINFO ADD CONSTRAINT PK_SERVERINFO PRIMARY KEY (FLD_SERVERIDX)
GO

