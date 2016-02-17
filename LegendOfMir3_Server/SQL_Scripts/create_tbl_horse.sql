/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_HORSE
GO

/* 
	User Character Table
*/

CREATE TABLE TBL_HORSE
(
	FLD_CHARNAME		CHAR(20)		NOT NULL,
	FLD_HORSEINDEX	CHAR(11)		NOT NULL,

	FLD_HORSETYPE		TINYINT			NOT NULL,

	FLD_LEVEL		TINYINT			NULL,
	FLD_PRICE		INT			NULL,
	FLD_HP			TINYINT			NULL
)
GO

ALTER TABLE TBL_HORSE ADD CONSTRAINT PK_HORSE  PRIMARY KEY (FLD_CHARNAME, FLD_HORSEINDEX)
GO
