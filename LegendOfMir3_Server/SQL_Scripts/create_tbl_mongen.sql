/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_MONGEN
GO

/* 
	Monster Generation Info
*/

CREATE TABLE TBL_MONGEN
(
	FLD_INDEX		INT			NOT NULL,
	FLD_MAPNAME		CHAR(14)		NOT NULL,
	FLD_X			INT			NOT NULL,
	FLD_Y			INT			NULL,
	FLD_MONNAME		CHAR(14)		NULL,
	FLD_AREA		SMALLINT		NULL,
	FLD_COUNT		SMALLINT		NULL,
	FLD_GENTIME		INT			NULL,
	FLD_SMALLGENRATE	INT			NULL,
	FLD_DESCRIPTION	VARCHAR(100)		NULL,
)
GO

ALTER TABLE TBL_MONGEN ADD CONSTRAINT PK_MONGEN PRIMARY KEY (FLD_INDEX)
GO
