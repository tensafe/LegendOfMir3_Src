/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_MAGIC
GO

/* 
	Magic Item
*/

CREATE TABLE TBL_MAGIC
(
	FLD_INDEX		INT			NOT NULL,
	FLD_NAME		CHAR(12)		NOT NULL,
	FLD_EFFECTTYPE		SMALLINT		NULL,
	FLD_EFFECT		SMALLINT		NULL,
	FLD_SPELL		SMALLINT		NULL,
	FLD_POWER		SMALLINT		NULL,
	FLD_MAXPOWER		SMALLINT		NULL,
	FLD_DEFSPELL		SMALLINT		NULL,
	FLD_DEFPOWER		SMALLINT		NULL,
	FLD_DEFMAXPOWER	SMALLINT		NULL,
	FLD_JOB			SMALLINT		NULL,
	FLD_NEEDL1		SMALLINT		NULL,
	FLD_L1TRAIN		INT			NULL,
	FLD_NEEDL2		SMALLINT		NULL,
	FLD_L2TRAIN		INT			NULL,
	FLD_NEEDL3		SMALLINT		NULL,
	FLD_L3TRAIN		INT			NULL,
	FLD_DELAY		SMALLINT		NULL,
	FLD_DESC		CHAR(8)			NULL,
	FLD_DESCRIPTION	VARCHAR(100)		NULL,
)
GO

ALTER TABLE TBL_MAGIC ADD CONSTRAINT PK_MAGIC PRIMARY KEY (FLD_INDEX)
GO
