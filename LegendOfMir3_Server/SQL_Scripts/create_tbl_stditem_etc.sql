/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_STDITEM_ETC
GO

/* 
	Standard General Item
*/

CREATE TABLE TBL_STDITEM_ETC
(
	FLD_INDEX		INT			NOT NULL,
	FLD_NAME		VARCHAR(20)		NOT NULL,
	FLD_STDMODE		SMALLINT		NULL,			
	FLD_SHAPE		SMALLINT		NULL,
	FLD_LOOKS		INT			NULL,
	FLD_WEIGHT		SMALLINT		NULL,
	FLD_PRICE		INT			NULL,
	FLD_VAL1		INT			NULL,
	FLD_VAL2		INT			NULL,
)
GO

ALTER TABLE TBL_STDITEM_ETC ADD CONSTRAINT PK_STDITEM_ETC PRIMARY KEY (FLD_INDEX)
GO
