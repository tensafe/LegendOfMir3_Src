; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConsoleSocket
LastTemplate=CAsyncSocket
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ManConsole.h"

ClassCount=4
Class1=CManConsoleApp
Class2=CManConsoleDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CConsoleSocket
Resource3=IDD_MANCONSOLE_DIALOG

[CLS:CManConsoleApp]
Type=0
HeaderFile=ManConsole.h
ImplementationFile=ManConsole.cpp
Filter=N

[CLS:CManConsoleDlg]
Type=0
HeaderFile=ManConsoleDlg.h
ImplementationFile=ManConsoleDlg.cpp
Filter=D
LastObject=CManConsoleDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=ManConsoleDlg.h
ImplementationFile=ManConsoleDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MANCONSOLE_DIALOG]
Type=1
Class=CManConsoleDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,SysListView32,1350631425
Control4=IDC_EDIT1,edit,1350631552

[CLS:CConsoleSocket]
Type=0
HeaderFile=ConsoleSocket.h
ImplementationFile=ConsoleSocket.cpp
BaseClass=CAsyncSocket
Filter=N
LastObject=CConsoleSocket

