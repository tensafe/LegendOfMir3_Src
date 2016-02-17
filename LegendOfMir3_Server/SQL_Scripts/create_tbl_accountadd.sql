/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_ACCOUNTADD
GO

/* 
	User Account AddInfo Table 
*/

CREATE TABLE TBL_ACCOUNTADD
(
	FLD_LOGINID		CHAR(10)		NOT NULL,		
	FLD_SSNO		CHAR(14)		NOT NULL,		
	FLD_BIRTHDAY		CHAR(10)		NOT NULL,
	FLD_PHONE		CHAR(14)		NOT NULL,
	FLD_MOBILEPHONE	CHAR(13)		NOT NULL,
	FLD_ADDRESS1		VARCHAR(20)		NULL,
	FLD_ADDRESS2		VARCHAR(20)		NULL,
	FLD_EMAIL		VARCHAR(40)		NULL,
	FLD_QUIZ1		VARCHAR(20)		NULL,
	FLD_ANSWER1		VARCHAR(20)		NULL,
	FLD_QUIZ2		VARCHAR(20)		NULL,
	FLD_ANSWER2		VARCHAR(20)		NULL
)
GO

ALTER TABLE TBL_ACCOUNTADD ADD CONSTRAINT PK_ACCOUNTADD PRIMARY KEY (FLD_LOGINID)
GO

CREATE INDEX IDX_ACCOUNTADD_SSNO ON TBL_ACCOUNTADD (FLD_SSNO ASC)
GO

