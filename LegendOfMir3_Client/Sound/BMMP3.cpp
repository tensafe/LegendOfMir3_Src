// BMMP3.cpp: implementation of the BMMP3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBMMp3::CBMMp3()
{
	CoInitialize(NULL);
	m_pBasicAudio = NULL;
	m_pMediaControl= NULL;
	m_pMediaSeeking= NULL;
	m_pMediaPosition= NULL;
	m_pMediaEventEx= NULL;
	m_pGraphBuilder= NULL;
	m_bIsPlaying = FALSE;		// Is this Mp3 Media playing?
	m_bInited = FALSE;			// Is this class initialized?
	m_bLooping = FALSE;			// Is this Mp3 Media Looping Play?
	m_hWnd = NULL;				// Handle of this class's owner
	m_bIsLoadMp3 = FALSE;
}

CBMMp3::~CBMMp3()
{
	Release();
	CoUninitialize();
}

BOOL CBMMp3::InitBMMp3(HWND hWnd)
{
//	HRESULT	hResult;
	m_bInited = TRUE;
/*	m_hWnd = hWnd;
	if(SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&m_pGraphBuilder)))
	{
		hResult = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
		if(FAILED(hResult)) 
			m_bInited = FALSE;

		hResult = m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&m_pBasicAudio);
		if(FAILED(hResult))
			m_bInited = FALSE;

		hResult = m_pGraphBuilder->QueryInterface(IID_IMediaSeeking,(void **)&m_pMediaSeeking);
		if(FAILED(hResult))
			m_bInited = FALSE;

		hResult = m_pGraphBuilder->QueryInterface(IID_IMediaEventEx,(void **)&m_pMediaEventEx);
		if(FAILED(hResult))
			m_bInited = FALSE;

		hResult = m_pGraphBuilder->QueryInterface(IID_IMediaPosition,(void **)&m_pMediaPosition);
		if(FAILED(hResult))
			m_bInited = FALSE;
	}
	else
	{*/
		m_bInited = FALSE;
	//}
	return m_bInited;
}

BOOL CBMMp3::LoadMp3(CHAR*	szFileName,HWND hWnd)
{
	WCHAR	wcFileName[MAX_PATH];
	if(!m_bInited)
	{
		if(InitBMMp3(hWnd))
		{
			MultiByteToWideChar(CP_ACP,0,szFileName,	-1,wcFileName,MAX_PATH);
			strcpy(m_szFileName,szFileName);
			if(SUCCEEDED(m_pGraphBuilder->RenderFile(wcFileName, NULL)))
			{
				m_bIsLoadMp3 = TRUE;
			}
			else
			{
				m_bIsLoadMp3 = FALSE;
			}
		}
	}
	else
	{
		OnStop();
		Release();
		LoadMp3(szFileName,hWnd);
	}
	return TRUE;
}

BOOL CBMMp3::OnPlay(BOOL Looping)
{
	if(m_bInited&&m_bIsLoadMp3)
	{
		if(m_bIsPlaying)
		{
			OnStop();
			m_bLooping = Looping;
			return OnPlay(Looping);
		}
		m_bLooping = Looping;
		if(FAILED(m_pMediaControl->Run()))
			return FALSE;
		SetNotifyWindow(WM_DSHOW_NOTIFY,0x00);
		m_bIsPlaying = TRUE;
	}
	else
		return FALSE;
	return TRUE;
}

BOOL CBMMp3::OnPlay(LONG lVolume,BOOL Looping)
{
	if(m_bInited&&m_bIsLoadMp3)
	{
		if(m_bIsPlaying)
		{
			OnStop();
			m_bLooping = Looping;
			return OnPlay(Looping);
		}
		m_bLooping = Looping;
		SetVolume(lVolume);
		if(FAILED(m_pMediaControl->Run()))
			return FALSE;

		SetNotifyWindow(WM_DSHOW_NOTIFY,0x00);
		m_bIsPlaying = TRUE;
	}
	else
		return FALSE;
	return TRUE;
}
BOOL CBMMp3::OnStop(VOID)
{
	if(m_bInited&&m_bIsLoadMp3)
	{
		if(m_bIsPlaying)
		{
			SetNotifyFlags(AM_MEDIAEVENT_NONOTIFY);	// WM_DSHOW_NOTIFY 를 발생시키지 않음
			if(FAILED(m_pMediaControl->Stop()))
				return FALSE;
			m_pMediaPosition->put_CurrentPosition(0);	// 처음으로 돌리낟.
			m_bIsPlaying = FALSE;
//			m_bLooping = FALSE;
			Release();
		}
		else
			return FALSE;
	}
	return TRUE;
}

BOOL CBMMp3::OnPause(VOID)
{
	if(m_bInited&&m_bIsLoadMp3)
	{
		if(m_bIsPlaying)
		{
			if(FAILED(m_pMediaControl->Pause()))
				return FALSE;
			m_bIsPlaying = TRUE;
		}
		else
			return FALSE;
	}
	return TRUE;
}


HRESULT CBMMp3::MessageProcess(VOID)
{
	LONG evCode, evParam1, evParam2;
	while(SUCCEEDED(m_pMediaEventEx->GetEvent(&evCode, &evParam1, &evParam2,0)))
	{
		switch(evCode)
		{
		case EC_WINDOW_DESTROYED:
			OnStop();
			break;
		case EC_COMPLETE:
			if(m_bLooping)
			{
				LoadMp3(m_szFileName,m_hWnd);
				OnPlay(m_bLooping);
				return 0l;
			}
			else 
			{
				OnStop();
				return 0l;
			}
			break;
		case EC_ACTIVATE:
			break;
		}
		m_pMediaEventEx->FreeEventParams(evCode, evParam1, evParam2);
	}
	return 0l;
}

VOID CBMMp3::Release(VOID)
{
	if(m_bIsPlaying)
		OnStop();
	if(m_pMediaPosition!=NULL)
	{
		m_pMediaPosition->Release();
		m_pMediaPosition = NULL;
	}
	if(m_pMediaEventEx!=NULL)
	{
		m_pMediaEventEx->Release();
		m_pMediaEventEx = NULL;
	}
	if(m_pMediaSeeking!=NULL)
	{
		m_pMediaSeeking->Release();
		m_pMediaSeeking = NULL;
	}
	if(m_pBasicAudio!=NULL)
	{
		m_pBasicAudio->Release();
		m_pBasicAudio = NULL;
	}
	if(m_pMediaControl!=NULL)
	{
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}
	if(m_pGraphBuilder!=NULL)
	{
		m_pGraphBuilder->Release();
		m_pGraphBuilder= NULL;
	}
	m_bInited = FALSE;
}

BOOL CBMMp3::SetVolume(LONG lVolume)
{
	if(m_bInited&&m_bIsLoadMp3)
	{
		if(FAILED(m_pBasicAudio->put_Volume(lVolume)))
			return FALSE;
	}
	return TRUE;
}

LONG CBMMp3::GetVolume(VOID)
{
	LONG	lResult = 0;;
	if(m_bInited&&m_bIsLoadMp3)
	{
		if(m_bIsPlaying)
		{
			m_pBasicAudio->get_Volume(&lResult);
		}
	}
	return lResult;
}

////////////////////////////////////////////////////////////////
// Set Message that is Event occur
// lMsg : Long
// cOnOff : BYTE  (ex) On = 0x00     Off = 0x01
/////////////////////////////////////////////////////////////////
BOOL CBMMp3::SetNotifyWindow(LONG lMsg,LONG lOnOff)
{	
	if(FAILED(m_pMediaEventEx->SetNotifyWindow((OAHWND)m_hWnd, lMsg ,0))) return FALSE;
	if(SetNotifyFlags(lOnOff)) return FALSE;
	return TRUE;
}

BOOL CBMMp3::SetNotifyFlags(LONG lFlag)
{
	if(FAILED(m_pMediaEventEx->SetNotifyFlags(lFlag))) return FALSE;
	return TRUE;
}
