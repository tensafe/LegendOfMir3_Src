#ifndef _CAvi_H_
#define _CAvi_H_

#define RELEASE( x )		if( x != NULL ) { x->Release(); x = NULL; }
#define FREE(p)				if( p )			{ free(p); p=NULL; }


class CAvi
{
public:
	CAvi(void);
	~CAvi(void);

	BOOL	Create(const char* Filename, BOOL bLoop = FALSE, BOOL bPlayBack = TRUE);
	void	Start(void);
	void	Stop(void);
	void	NextFrame(void);
	void	ReadFrame(LONG Frame);
	BOOL	GetSound(VOID);
	HRESULT	DrawBuffer(void);
	void	InitAvi(LPDIRECTSOUND lpDS);
	void	ReleaseAvi( void );
	__inline void SetDestRect(RECT rcDst)
	{
		m_DstRect = rcDst;
	}
	__inline void SetStopFrame(INT nFrame)
	{
		m_nStopFrame = nFrame;
	}
	virtual BOOL ReadDatas(INT nLoopTime);
	virtual BOOL Draw(INT nLoopTime);
	virtual HRESULT DrawFast(INT nLoopTime, int X, int Y);
public:
	BOOL	m_bPlaying;
	BOOL	m_bLoop;
	BOOL	m_bPlayDirection;
	LONG	m_Index;
	LONG	m_nFrames;
	DWORD	m_Fps;
	int		m_TimerID;
	int		m_TimeTick;

	DDSURFACEDESC2			m_DDSD;
	LPDIRECTDRAWSURFACE7	m_lpDDS;
	RECT					m_SrcRect;
	RECT					m_DstRect;

	PAVIFILE				m_AviFile;
	CRITICAL_SECTION		m_AccessBuffer;
	PAVISTREAM				m_AviStream;
	AVISTREAMINFO			m_StreamInfo;
	LPBYTE					m_Input;
	LPBYTE					m_Output;
	HIC						m_Decompressor;
	LONG					m_Length;
	LONG					m_LinePitch;
	LPBITMAPINFOHEADER		m_InputFormat;
	LPBITMAPV4HEADER		m_TargetFormat;

	PAVISTREAM				m_AviSound;
	AVISTREAMINFO			m_SoundInfo;
	WAVEFORMATEX*			m_SoundFormat;
	LPDIRECTSOUNDBUFFER		m_SoundBuffer;
	DWORD					m_LoadPos;
	DWORD					m_LoadSize;
	int						m_SoundFramesAhead;

private:
	LPDIRECTSOUND			m_lpDS;
	BOOL					m_bIsPlaying;
	INT						m_nStopFrame;
};

#endif _CAvi_H_