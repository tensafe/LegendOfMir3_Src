# Microsoft Developer Studio Project File - Name="Mir2Ex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Mir2Ex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mir2Ex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mir2Ex.mak" CFG="Mir2Ex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mir2Ex - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Mir2Ex - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Mir2Ex", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mir2Ex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"C:\Mir2EI\Mir2EIRlz.exe"

!ELSEIF  "$(CFG)" == "Mir2Ex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"C:\Mir2EI\Mir2EI.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Mir2Ex - Win32 Release"
# Name "Mir2Ex - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GFun.cpp
# End Source File
# Begin Source File

SOURCE=.\Mir2Ex.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Define.h
# End Source File
# Begin Source File

SOURCE=.\Extern.h
# End Source File
# Begin Source File

SOURCE=.\GFun.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Common\Typedeftxt.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Mir2Ex.ico
# End Source File
# End Group
# Begin Group "Login Process"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LoginProcess\Common\BMWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Common\BMWnd.h
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Common\define.h
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Login\Login.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Login\Login.h
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Intro\LoginAvi.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Intro\LoginAvi.h
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\LoginProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\LoginProcess.h
# End Source File
# Begin Source File

SOURCE=".\LoginProcess\New Account\NewAccount.cpp"
# End Source File
# Begin Source File

SOURCE=".\LoginProcess\New Account\NewAccount.h"
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Patch.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Patch.h
# End Source File
# Begin Source File

SOURCE=".\LoginProcess\Select Srv\SelectSrv.cpp"
# End Source File
# Begin Source File

SOURCE=".\LoginProcess\Select Srv\SelectSrv.h"
# End Source File
# Begin Source File

SOURCE=.\LoginProcess\Common\typedef.h
# End Source File
# End Group
# Begin Group "Game Precess"

# PROP Default_Filter ""
# Begin Group "Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameProcess\Actor.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\ChatPopWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\ClientSysMsg.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\ExchangeWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GameBtn.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GameProc.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GameWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GroupPopWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GroupWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GuildMasterWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GuildWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\HorseWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Interface.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\InventoryWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Item.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\LightFog.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Magic.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\MapHandler.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\NPCWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\OptionWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Particle.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\QuestWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\SprDfn.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\StatusWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\StoreWnd.h
# End Source File
# Begin Source File

SOURCE=.\GameProcess\WeatherEffect.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameProcess\Actor.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\ChatPopWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\ClientSysMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\ExchangeWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GameBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GameProc.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GroupPopWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GroupWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GuildMasterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\GuildWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\HorseWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\InventoryWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\LightFog.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Magic.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\MapHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\NPCWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\OptionWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\QuestWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\SprDfn.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\StatusWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\StoreWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProcess\WeatherEffect.cpp
# End Source File
# End Group
# End Group
# Begin Group "Common "

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\Button.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Button.H
# End Source File
# Begin Source File

SOURCE=.\Common\ChatEditBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\ChatEditBox.h
# End Source File
# Begin Source File

SOURCE=.\Common\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=.\Common\DblList.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DblList.h
# End Source File
# Begin Source File

SOURCE=.\Common\DLinkedList.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\DLinkedList.h
# End Source File
# Begin Source File

SOURCE=.\Common\EnDecode.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\EnDecode.h
# End Source File
# Begin Source File

SOURCE=.\Common\ImageHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\ImageHandler.h
# End Source File
# Begin Source File

SOURCE=.\Common\MirButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\MirButton.h
# End Source File
# Begin Source File

SOURCE=.\Common\MirMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\MirMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\Common\MirMsgBox3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\MirMsgBox3D.h
# End Source File
# Begin Source File

SOURCE=.\Common\MsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\MsgBox.h
# End Source File
# Begin Source File

SOURCE=.\Common\NoticeBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\NoticeBox.h
# End Source File
# Begin Source File

SOURCE=.\Common\NPCTxtAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\NPCTxtAnalysis.h
# End Source File
# Begin Source File

SOURCE=.\Common\Protocol.h
# End Source File
# Begin Source File

SOURCE=.\Common\Queue.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Queue.h
# End Source File
# End Group
# Begin Group "Chr Select Process"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CharSelectProcess\BackGround\BackGround.cpp
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\BackGround\BackGround.h
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\CharacterProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\CharacterProcess.h
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\CreateChr\CreateChr.cpp
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\CreateChr\CreateChr.h
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\Common\define.h
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\SelectChr\SelectChr.cpp
# End Source File
# Begin Source File

SOURCE=.\CharSelectProcess\SelectChr\SelectChr.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sound\BMMP3.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\BMMP3.h
# End Source File
# Begin Source File

SOURCE=.\Sound\MirSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\MirSound.h
# End Source File
# Begin Source File

SOURCE=.\Sound\SBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\SBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Sound\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\Sound.h
# End Source File
# Begin Source File

SOURCE=.\Sound\TypeDef.h
# End Source File
# End Group
# Begin Group "Avi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AVI\Avi.cpp
# End Source File
# Begin Source File

SOURCE=.\AVI\Avi.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
