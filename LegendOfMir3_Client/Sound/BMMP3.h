// CBMMp3.h: interface for the CBMMp3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBMMp3_H__E7B9DE69_5CF3_4A3A_AE0D_D13BD100E109__INCLUDED_)
#define AFX_CBMMp3_H__E7B9DE69_5CF3_4A3A_AE0D_D13BD100E109__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define	WM_DSHOW_NOTIFY	WM_APP+500  // Pivate message

class CBMMp3  
{
public:
///////////////////////////////////////////////////////////////
	CBMMp3();
	virtual ~CBMMp3();
///////////////////////////////////////////////////////////////
	BOOL	LoadMp3(CHAR*	szFileName,HWND hWnd);
	BOOL	OnPlay(BOOL	Looping);
	BOOL	OnPlay(LONG lVolume,BOOL Looping);
	BOOL	OnStop(VOID);
	BOOL	OnPause(VOID);
	BOOL	InitBMMp3(HWND hWnd);
	BOOL	SetVolume(LONG lVolume);
	LONG	GetVolume(VOID);
	BOOL	SetNotifyWindow(LONG lMsg,LONG lOnOff);
	BOOL	SetNotifyFlags(LONG lFlag);
	HRESULT	MessageProcess(VOID);
private:
///////////////////////////////////////////////////////////////
	IBasicAudio		*m_pBasicAudio;
	IMediaControl	*m_pMediaControl;
	IMediaSeeking	*m_pMediaSeeking;
	IMediaPosition	*m_pMediaPosition;
	IMediaEventEx	*m_pMediaEventEx;
	IGraphBuilder	*m_pGraphBuilder;
///////////////////////////////////////////////////////////////
	BOOL			m_bIsPlaying;		// Is this Mp3 Media playing?
	BOOL			m_bInited;			// Is this class initialized?
	BOOL			m_bLooping;			// Is this Mp3 Media Looping Play?
	HWND			m_hWnd;				// Handle of this class's owner
	BOOL			m_bIsLoadMp3;
	CHAR			m_szFileName[MAX_PATH];

protected:


public:

	VOID Release(VOID);
	__inline BOOL	IsPlaying(VOID)
	{
		return m_bInited;
	}
};

#endif // !defined(AFX_CBMMp3_H__E7B9DE69_5CF3_4A3A_AE0D_D13BD100E109__INCLUDED_)
