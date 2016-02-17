===============================================================================
       
       LoginSvr
       
       All written by Euy-heon, Jeong.
       Copyright(C) 2001 Wemade Entertainment Co., LTD. All rights Reserved.
       
===============================================================================

Last Update		: 2001/09/20 
Last Update Author	: Euy-heon, Jeong.

1. Using common module file name.

	- RegistryHandler.cpp
	- ServerSockHandler.cpp

2. Global variables.

StdAfx.cpp
AddSvrListFunc.cpp
ConfigDlgFunc.cpp
GateCommSockMsg.cpp
LoginSvr.cpp
LoginSvr.rc
MainWndProc.cpp
ServerListProc.cpp
ThreadFuncForMsg.cpp

3. File and function description.


4. History


GateCommSockMsg -> ThreadFuncForMsg -------------> DecodeUserData
  (FD_READ)      (ThreadFuncForMsg.cpp)
                  Decode Gate Protocol

