/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

USE LEGENDOFMIR
GO

DROP TABLE TBL_MONSTER
GO

/* 
	Monster 
*/

CREATE TABLE TBL_MONSTER
(
	FLD_NAME		CHAR(14)		NOT NULL,
	FLD_RACE		SMALLINT		NULL,
	FLD_RACEIMG		SMALLINT		NULL,
	FLD_APPR		SMALLINT		NULL,
	FLD_LEVEL		SMALLINT		NULL,
	FLD_UNDEAD		SMALLINT		NULL,
	FLD_EXP			SMALLINT		NULL,
	FLD_HP			SMALLINT		NULL,
	FLD_MP			SMALLINT		NULL,
	FLD_AC			SMALLINT		NULL,
	FLD_MAXAC		SMALLINT		NULL,
	FLD_MAC		SMALLINT		NULL,
	FLD_MAXMAC		SMALLINT		NULL,
	FLD_DC			SMALLINT		NULL,
	FLD_MAXDC		SMALLINT		NULL,
	FLD_SPEED		SMALLINT		NULL,
	FLD_HIT			SMALLINT		NULL,
	FLD_WALKSPEED		SMALLINT		NULL,
	FLD_ATTACKSPEED	SMALLINT		NULL,
)
GO
