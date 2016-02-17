# Microsoft Developer Studio Project File - Name="WindHorn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WindHorn - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WindHorn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WindHorn.mak" CFG="WindHorn - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WindHorn - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WindHorn - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WindHorn - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WindHorn - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "WindHorn - Win32 Release"
# Name "WindHorn - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\RegHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WHDefProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\WHDXGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\WHImage.cpp
# End Source File
# Begin Source File

SOURCE=.\WHSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\WHWilImage.cpp
# End Source File
# Begin Source File

SOURCE=.\WHWilTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\WHWindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\RegHandler.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WHDefProcess.h
# End Source File
# Begin Source File

SOURCE=.\WHDXGraphic.h
# End Source File
# Begin Source File

SOURCE=.\WHEngine.h
# End Source File
# Begin Source File

SOURCE=.\WHImage.h
# End Source File
# Begin Source File

SOURCE=.\WHSurface.h
# End Source File
# Begin Source File

SOURCE=.\WHWilImage.h
# End Source File
# Begin Source File

SOURCE=.\WHWilTexture.h
# End Source File
# Begin Source File

SOURCE=.\WHWindow.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# Begin Group "D3DCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\D3DCommon\d3dmath.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\d3dmath.h
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\d3dtextr.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\d3dtextr.h
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DCommon\ddutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
