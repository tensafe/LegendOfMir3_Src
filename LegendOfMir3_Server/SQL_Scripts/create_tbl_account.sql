/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_ACCOUNT
GO

/* 
	User Account Table 
*/

CREATE TABLE TBL_ACCOUNT
(
	FLD_LOGINID		CHAR(10)		NOT NULL,		
	FLD_PASSWORD		CHAR(10)		NOT NULL,		
	FLD_USERNAME		CHAR(20)		NOT NULL,		

	FLD_CERTIFICATION	INT			DEFAULT(0),
)
GO

ALTER TABLE TBL_ACCOUNT ADD CONSTRAINT PK_ACCOUNT PRIMARY KEY (FLD_LOGINID)
GO
