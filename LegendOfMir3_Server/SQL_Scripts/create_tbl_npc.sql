/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_NPC
GO

/* 
	NPC
*/

CREATE TABLE TBL_NPC
(
	FLD_INDEX		INT			NOT NULL,
	FLD_NAME		CHAR(14)		NOT NULL,
	FLD_RACE		SMALLINT		NOT NULL,
	FLD_MAPNAME		CHAR(14)		NOT NULL,
	FLD_POSX		INT			NOT NULL,
	FLD_POSY		INT			NOT NULL,
	FLD_FACE		SMALLINT		NOT NULL,
	FLD_BODY		SMALLINT		NOT NULL,
	FLD_DESCRIPTION	VARCHAR(100)		NULL,
)
GO

/*ALTER TABLE TBL_MERCHANT ADD CONSTRAINT PK_MOVEMAPEVENT PRIMARY KEY (FLD_INDEX)
GO */
