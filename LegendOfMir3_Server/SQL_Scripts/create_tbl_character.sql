/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_CHARACTER
GO

/* 
	User Character Table
*/

CREATE TABLE TBL_CHARACTER
(
	FLD_LOGINID		CHAR(10)		NOT NULL,
	FLD_CHARNAME		CHAR(20)		NOT NULL,

	FLD_JOB			CHAR(1)			NOT NULL,
	FLD_GENDER		CHAR(1)			NOT NULL,

	FLD_LEVEL		TINYINT			NOT NULL,
	
	FLD_DIRECTION		INT			NOT NULL,
	FLD_CX			INT			NOT NULL,
	FLD_CY			INT			NOT NULL,
	FLD_MAPNAME		CHAR(16)		NOT NULL,
	
	FLD_GOLD		INT			NOT NULL,
	
	FLD_DRESS_ID		CHAR(11)		NOT NULL,
	FLD_WEAPON_ID		CHAR(11)		NOT NULL,
	FLD_LEFTHAND_ID	CHAR(11)		NOT NULL,
	FLD_RIGHTHAND_ID	CHAR(11)		NOT NULL,
	FLD_HELMET_ID		CHAR(11)		NOT NULL,
	FLD_NECKLACE_ID	CHAR(11)		NOT NULL,
	FLD_ARMRINGL_ID	CHAR(11)		NOT NULL,
	FLD_ARMRINGR_ID	CHAR (11)		NOT NULL,
	FLD_RINGL_ID		CHAR(11)		NOT NULL,
	FLD_RINGR_ID		CHAR(11)		NOT NULL,
)
GO

ALTER TABLE TBL_CHARACTER ADD CONSTRAINT PK_CHARACTER  PRIMARY KEY (FLD_LOGINID, FLD_CHARNAME)
GO
