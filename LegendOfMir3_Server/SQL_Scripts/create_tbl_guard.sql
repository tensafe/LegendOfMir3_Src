/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_GUARD
GO

/* 
	Guard
*/

CREATE TABLE TBL_GUARD
(
	FLD_INDEX		INT			NOT NULL,
	FLD_ID			CHAR(14)		NOT NULL,
	FLD_MAPNAME		CHAR(14)		NOT NULL,
	FLD_POSX		INT			NOT NULL,
	FLD_POSY		INT			NOT NULL,
	FLD_DIRECTION		SMALLINT		NOT NULL,
	FLD_DESCRIPTION	VARCHAR(100)		NULL,
)
GO

/*ALTER TABLE TBL_MERCHANT ADD CONSTRAINT PK_MOVEMAPEVENT PRIMARY KEY (FLD_INDEX)
GO */

