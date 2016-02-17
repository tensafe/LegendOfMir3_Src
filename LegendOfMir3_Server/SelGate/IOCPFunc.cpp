#include "stdafx.h"

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL CheckAvailableIOCP()
{
	OSVERSIONINFO VersionInfo;

	GetVersionEx(&VersionInfo);

	if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && VersionInfo.dwMajorVersion >= 5)
		return TRUE;

	return FALSE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************
/*
INT CreateIOCPWorkerThread(HANDLE hCP, LPDWORD lpdwThreadID, LPTHREAD_START_ROUTINE WorkerThreadFunc)
{
	if (CompletionPort != INVALID_HANDLE_VALUE)
	{
		SYSTEM_INFO SystemInfo;

		if ((hCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
			return -1;

		GetSystemInfo(&SystemInfo);

		for (UINT i = 0; i < SystemInfo.dwNumberOfProcessors * 2; i++)
		{
			HANDLE ThreadHandle;

			if ((ThreadHandle = CreateThread(NULL, 0, WorkerThreadFunc, hCP, 0, lpdwThreadID)) == NULL)
				return -1;

			CloseHandle(ThreadHandle);
		}

		return SystemInfo.dwNumberOfProcessors * 2;
	}

	return -1;
}
*/