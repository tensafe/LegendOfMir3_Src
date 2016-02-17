/* **************************************************************************
	
	Create Database for Legend of Mir 2

	All written by Euy-heon, Jeong
	Copyright (C) 2001 Wemade Entertainment. All rights reserved.

   *************************************************************************** */

IF EXISTS (SELECT * FROM SYSDATABASES WHERE NAME = 'LEGENDOFMIR')
	DROP DATABASE LEGENDOFMIR

GO

CREATE DATABASE LEGENDOFMIR ON 
( 
	NAME = LEGENDOFMIR_DATA,
	FILENAME = 'D:\DATA\LEGENDOFMIR_DATA.MDF',
	SIZE = 10,
	MAXSIZE = 50,
	FILEGROWTH = 10
)
LOG ON
(
	NAME = LEGENDOFMIR_LOG,
	FILENAME = 'D:\DATA\LEGENDOFMIR_LOG.LDF',
	SIZE = 5MB,
	MAXSIZE = 25MB,
	FILEGROWTH = 5MB
)

GO
