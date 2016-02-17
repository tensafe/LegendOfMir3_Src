# Microsoft Developer Studio Project File - Name="GameSvr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GameSvr - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GameSvr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GameSvr.mak" CFG="GameSvr - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GameSvr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GameSvr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GameSvr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GameSvr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../_Bin/Debug"
# PROP Intermediate_Dir "../_Obj_Debug/GameSvr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GameSvr - Win32 Release"
# Name "GameSvr - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DataHandler.h
# End Source File
# Begin Source File

SOURCE=..\Def\EnDecode.h
# End Source File
# Begin Source File

SOURCE=.\FrontEngine.h
# End Source File
# Begin Source File

SOURCE=.\GameSvr.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\ItemHandler.h
# End Source File
# Begin Source File

SOURCE=..\Def\_OrzEx\list.h
# End Source File
# Begin Source File

SOURCE=..\Def\List.h
# End Source File
# Begin Source File

SOURCE=.\MagicHandler.h
# End Source File
# Begin Source File

SOURCE=..\Def\Misc.h
# End Source File
# Begin Source File

SOURCE=.\ObjectEngine.h
# End Source File
# Begin Source File

SOURCE=.\ObjectEvent.h
# End Source File
# Begin Source File

SOURCE=.\ObjectMonster.h
# End Source File
# Begin Source File

SOURCE=.\ObjectNPC.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayer.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayerClone.h
# End Source File
# Begin Source File

SOURCE=.\ObjectScripter.h
# End Source File
# Begin Source File

SOURCE=..\Def\Protocol.h
# End Source File
# Begin Source File

SOURCE=..\Def\_OrzEx\queue.h
# End Source File
# Begin Source File

SOURCE=..\Def\Queue.h
# End Source File
# Begin Source File

SOURCE=..\Def\ServerSockHandler.h
# End Source File
# Begin Source File

SOURCE=..\Def\StaticArray.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Tables.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\MIR2.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar.bmp
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConfigDlgFunc.cpp
# End Source File
# Begin Source File

SOURCE=..\Def\_OrzEx\database.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSvr.rc
# End Source File
# Begin Source File

SOURCE=.\GateInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Handler_CommonDB.cpp
# End Source File
# Begin Source File

SOURCE=.\Handler_Item.cpp
# End Source File
# Begin Source File

SOURCE=.\Handler_Magic.cpp
# End Source File
# Begin Source File

SOURCE=.\Handler_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWndProc.cpp
# End Source File
# Begin Source File

SOURCE=..\Def\Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectMerchant.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayerAdmin.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayerAdminSaid.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayerClone.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPlayerSaid.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectScripter.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessMerchants.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessUserHuman.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessUserMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\SockMsg_DBSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\SockMsg_GateComm.cpp
# End Source File
# Begin Source File

SOURCE=.\SockMsg_LogSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ThreadFuncForComm.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInfo.cpp
# End Source File
# End Group
# Begin Group "Def"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\Def\EnDecode.cpp
# End Source File
# Begin Source File

SOURCE=..\Def\RegstryHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\Def\ServerSockHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\Def\_OrzEx\syncobj.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
