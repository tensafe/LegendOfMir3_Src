/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_MERCHANT
GO

/* 
	Merchant
*/

CREATE TABLE TBL_MERCHANT
(
	FLD_ID			INT			IDENTITY(1, 1),
	FLD_MAPNAME		CHAR(14)		NOT NULL,
	FLD_POSX		INT			NOT NULL,
	FLD_POSY		INT			NOT NULL,
	FLD_NPCNAME		VARCHAR(40)		NOT NULL,
	FLD_FACE		INT			NOT NULL,
	FLD_BODY		INT			NOT NULL,
	FLD_GENDER		SMALLINT		NOT NULL,
	FLD_DESCRIPTION	VARCHAR(100)		NULL,
)
GO

ALTER TABLE TBL_MERCHANT ADD CONSTRAINT PK_MERCHANT PRIMARY KEY (FLD_ID)
GO
