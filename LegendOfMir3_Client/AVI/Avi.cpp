#include "StdAfx.h"


//////////////////////////////////////////////////////////////////////////////////
// CAvi Constructor
//////////////////////////////////////////////////////////////////////////////////
CAvi::CAvi(void)
{
	m_AviFile = NULL;
	m_lpDDS = NULL;
	m_AviStream = NULL;
	ZeroMemory(&m_StreamInfo, sizeof(m_StreamInfo));
	m_Index = 0;
	m_nFrames = 0;
	m_Decompressor = 0;
	m_Input = m_Output = 0;
	m_AviSound = NULL;
	ZeroMemory(&m_SoundInfo, sizeof(m_SoundInfo));
	m_SoundFormat = 0;
	m_LoadPos = 0;
	m_LoadSize = 0;
	m_SoundFramesAhead = 0;
	m_nStopFrame = -1;
	m_bIsPlaying = FALSE;
	m_SoundBuffer = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi Destructor
//////////////////////////////////////////////////////////////////////////////////
CAvi::~CAvi(void)
{
//	CAvi::ReleaseAvi();
}

void CAvi::InitAvi(LPDIRECTSOUND lpDS)
{
	m_lpDS = lpDS;
	InitializeCriticalSection(&m_AccessBuffer);
}

void CAvi::ReleaseAvi( void )
{
	CAvi::Stop( );
	
	Sleep(1);

	if(m_Decompressor)
	{
		ICDecompressEnd(m_Decompressor);
		ICClose(m_Decompressor);
	}
	
	if( m_InputFormat )		FREE(m_InputFormat);
	if( m_TargetFormat )	FREE(m_TargetFormat);
	if( m_Input )			FREE(m_Input);
	if( m_Output )			FREE(m_Output);

	if( m_lpDDS )
		AVIFileExit();

	RELEASE( m_SoundBuffer );
	RELEASE( m_lpDDS );

	if( m_AviStream )
	{
		AVIStreamRelease(m_AviStream);
		m_AviStream = NULL;
	}
	if( m_AviSound )
	{
		AVIStreamRelease(m_AviSound );
		m_AviSound = NULL;
	}
	if( m_AviFile )
	{
		AVIFileRelease(m_AviFile);
		m_AviFile = NULL;
	}

	if(m_SoundFormat)
	{
		FREE(m_SoundFormat);
		m_SoundFormat = NULL;
	}

	DeleteCriticalSection(&m_AccessBuffer);
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi Create
//////////////////////////////////////////////////////////////////////////////////
BOOL CAvi::Create(const char* Filename, BOOL bLoop, BOOL bPlayBack)
{
	if(Filename == NULL) return FALSE;
	AVIFileInit();
	m_bLoop = bLoop;
	m_bPlayDirection = bPlayBack;

	if( AVIFileOpen( &m_AviFile, Filename, OF_READ, 0 ) )
	{
		AVIFileExit( );	
		return FALSE; 
	}

	if( AVIFileGetStream( m_AviFile, &m_AviStream, streamtypeVIDEO, 0 ) )
	{	
		AVIFileExit( );
        return FALSE;
	}

	LONG FmtLenght;

	AVIStreamFormatSize(m_AviStream, 0, &FmtLenght);
	m_InputFormat = (LPBITMAPINFOHEADER)malloc(FmtLenght);
	m_TargetFormat = (LPBITMAPV4HEADER)malloc(max(FmtLenght, sizeof(BITMAPV4HEADER)));
	ZeroMemory(m_TargetFormat, sizeof(BITMAPV4HEADER));
	AVIStreamReadFormat(m_AviStream, 0, m_InputFormat, &FmtLenght);
	m_nFrames = AVIStreamLength(m_AviStream);
	AVIStreamInfo(m_AviStream, &m_StreamInfo, sizeof(AVISTREAMINFO));

	HRESULT rval;

	ZeroMemory(&m_DDSD, sizeof(m_DDSD));
	m_DDSD.dwSize = sizeof(m_DDSD);
	m_DDSD.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	m_DDSD.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
	m_DDSD.dwWidth = m_InputFormat->biWidth;
	m_DDSD.dwHeight = m_InputFormat->biHeight;

	rval = g_xMainWnd.GetDirectDraw()->CreateSurface(&m_DDSD, &m_lpDDS, NULL);
	if(rval != DD_OK)
	{
		AVIFileExit();
		return FALSE;
	}

	m_lpDDS->GetSurfaceDesc(&m_DDSD);

	memcpy(m_TargetFormat, m_InputFormat, FmtLenght);

	m_TargetFormat->bV4Size = max(FmtLenght, sizeof(BITMAPV4HEADER));
	m_TargetFormat->bV4BitCount = (WORD)m_DDSD.ddpfPixelFormat.dwRGBBitCount;
	m_TargetFormat->bV4V4Compression = BI_BITFIELDS;
	
	if(m_TargetFormat->bV4BitCount==24) m_TargetFormat->bV4V4Compression = BI_RGB;

	m_TargetFormat->bV4ClrUsed = 0;
	m_TargetFormat->bV4RedMask = m_DDSD.ddpfPixelFormat.dwRBitMask;
	m_TargetFormat->bV4GreenMask = m_DDSD.ddpfPixelFormat.dwGBitMask;
	m_TargetFormat->bV4BlueMask  = m_DDSD.ddpfPixelFormat.dwBBitMask;
	m_TargetFormat->bV4AlphaMask = m_DDSD.ddpfPixelFormat.dwRGBAlphaBitMask;
	m_TargetFormat->bV4SizeImage = ((m_TargetFormat->bV4Width +3)&0xFFFFFFFC) *
																 m_TargetFormat->bV4Height * (m_TargetFormat->bV4BitCount>>3);
	m_Length = m_InputFormat->biWidth * m_InputFormat->biHeight *	(m_InputFormat->biBitCount >> 3);

	if(m_StreamInfo.dwSuggestedBufferSize) m_Length = (LONG)m_StreamInfo.dwSuggestedBufferSize;

	m_Decompressor = ICDecompressOpen(ICTYPE_VIDEO,	m_StreamInfo.fccHandler, m_InputFormat,
																	 (LPBITMAPINFOHEADER)m_TargetFormat);

	m_Input = (BYTE *)calloc(m_Length, 1);
	ZeroMemory(m_Input, m_Length);
	m_Output = (BYTE *)calloc(m_TargetFormat->bV4SizeImage, 1);
	ZeroMemory(m_Output, m_TargetFormat->bV4SizeImage);

	if(!m_Decompressor) return FALSE;

	m_LinePitch = m_TargetFormat->bV4Width * (m_TargetFormat->bV4BitCount >> 3);
	ICDecompressBegin(m_Decompressor, m_InputFormat, (LPBITMAPINFOHEADER)m_TargetFormat);
	m_Fps = m_StreamInfo.dwRate / m_StreamInfo.dwScale;
	m_TimeTick = (1000 * m_StreamInfo.dwScale + (m_StreamInfo.dwRate >> 1)) / m_StreamInfo.dwRate;

	m_SrcRect.top = 0;
	m_SrcRect.left = 0;
	m_SrcRect.bottom = m_TargetFormat->bV4Height;
	m_SrcRect.right = m_TargetFormat->bV4Width;

	m_DstRect.top		= 0;
	m_DstRect.left		= 0;
	m_DstRect.right		= 800;
	m_DstRect.bottom	= 600;

	if(m_lpDS)
	{
		UINT hResult = 0;
		hResult  = AVIFileGetStream( m_AviFile, &m_AviSound, streamtypeAUDIO, 0 );

		if(hResult  ==0 )
		{ 
			DSBUFFERDESC dsbd;
			ZeroMemory((VOID*)&dsbd, sizeof(DSBUFFERDESC));

			AVIStreamFormatSize(m_AviSound, 0, &FmtLenght);
			m_SoundFormat = (WAVEFORMATEX *)malloc(FmtLenght);

			AVIStreamReadFormat(m_AviSound, 0, m_SoundFormat, &FmtLenght);
			AVIStreamInfo(m_AviSound, &m_SoundInfo, sizeof(AVISTREAMINFO));

			m_LoadSize = (m_SoundInfo.dwSuggestedBufferSize)/(m_Fps);//(m_SoundFormat->nAvgBytesPerSec) / (m_Fps+1);

			dsbd.dwSize = sizeof(dsbd);
			dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY;
			dsbd.dwBufferBytes = m_SoundInfo.dwSuggestedBufferSize;//m_LoadSize * (m_Fps)-2;
			dsbd.dwReserved = 0;
			dsbd.lpwfxFormat = m_SoundFormat;
			hResult = m_lpDS->CreateSoundBuffer(&dsbd, &m_SoundBuffer, NULL);
			
			if( hResult != DS_OK )
			{
				AVIFileExit();
				return FALSE;
			}
		}
	}
	m_Index = (m_bPlayDirection ? 0 : m_nFrames);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////
// CAvi Start
//////////////////////////////////////////////////////////////////////////////////
void CAvi::Start(void)
{
	if( !GetSound() ) return;

	m_bPlaying = TRUE;
	CAvi::ReadFrame(m_Index);
	m_bIsPlaying = TRUE;

	if( m_SoundBuffer )
	{
		m_SoundFramesAhead = m_SoundInfo.dwInitialFrames / m_SoundInfo.dwScale;
		m_SoundBuffer->SetVolume(-200);
		if(m_SoundBuffer->Play(0, 0, DSBPLAY_LOOPING) != DS_OK) return;
	}

}

//////////////////////////////////////////////////////////////////////////////////
// CAvi Stop
//////////////////////////////////////////////////////////////////////////////////
void CAvi::Stop(void)
{
	m_bPlaying = FALSE;	
	if( m_SoundBuffer)
		m_SoundBuffer->Stop();
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi NextFrame
//////////////////////////////////////////////////////////////////////////////////
void CAvi::NextFrame(void)
{
	m_Index = m_Index + (m_bPlayDirection? 1 : -1);
	if(m_Index == (m_bPlayDirection ? m_nFrames : 0) )
	{
		CAvi::Stop( );

		if(m_bLoop)
		{
			m_Index = (m_bPlayDirection ? 0 : m_nFrames);
			m_LoadPos = (m_bPlayDirection ? 0 : m_nFrames);
			m_bPlaying = TRUE;

			if( m_SoundBuffer )
			{
				m_SoundBuffer->SetCurrentPosition(0);
				GetSound();
				m_SoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
			}
		}
		else
		{
			m_bIsPlaying = FALSE;
		}
	}


	if( (m_nStopFrame > 0 ) && ( m_Index == m_nStopFrame ))
	{
		m_bIsPlaying = FALSE;
		m_nStopFrame = -1;
	}
	else
	{
		GetSound();
		ReadFrame(m_Index);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi ReadFrame
//////////////////////////////////////////////////////////////////////////////////
void CAvi::ReadFrame(LONG Frame)
{
	Frame -= m_SoundFramesAhead;

	if(Frame < 0) return;
	if(Frame < m_nFrames)
	{
		AVIStreamRead(m_AviStream, Frame, 1, m_Input, m_Length, NULL, NULL);
		EnterCriticalSection(&m_AccessBuffer);
		ICDecompress(m_Decompressor, ICDECOMPRESS_HURRYUP, m_InputFormat, m_Input, (LPBITMAPINFOHEADER)m_TargetFormat, m_Output);
		LeaveCriticalSection(&m_AccessBuffer);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// CDXAvi GetSound
//////////////////////////////////////////////////////////////////////////////////
BOOL CAvi::GetSound(VOID)
{	
	HRESULT	rval;
	DWORD	dwSize,dwSize2;
	LPVOID	Data,Data2;

	dwSize	= 0;
	dwSize2 = 0;
	rval	= 0;
	Data	= NULL;
	Data2	= NULL;


	if( m_SoundBuffer )
	{
		rval = m_SoundBuffer->Lock(m_LoadPos * m_LoadSize, m_LoadSize, &Data, &dwSize, 0, 0, 0);// &Data2, &dwSize2, 0);
		if(rval != DS_OK)	return FALSE;

		AVIStreamRead(m_AviSound, m_Index * (m_LoadSize >> 2), m_LoadSize, Data, m_LoadSize, NULL, NULL);

		rval = m_SoundBuffer->Unlock(Data, dwSize, 0, 0);// Data2, dwSize2);

		m_LoadPos++;
		m_LoadPos %= m_Fps;

	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi DrawBuffer
//////////////////////////////////////////////////////////////////////////////////
HRESULT CAvi::DrawBuffer(void)
{
	HRESULT rval;

	ZeroMemory(&m_DDSD, sizeof(m_DDSD));
	m_DDSD.dwSize = sizeof(m_DDSD);
	rval = m_lpDDS->Lock(NULL, &m_DDSD, DDLOCK_WAIT, NULL);
	while(rval == DDERR_SURFACELOST) 
	{
//		Restore();
		ZeroMemory(&m_DDSD, sizeof(m_DDSD));
		m_DDSD.dwSize = sizeof(m_DDSD);
		rval = m_lpDDS->Lock(NULL, &m_DDSD, DDLOCK_WAIT, NULL);
	}

	if(rval == DD_OK)
	{
		BYTE *lpSource = m_Output + m_LinePitch * (m_TargetFormat->bV4Height - 1);
        BYTE *lpDest = (LPBYTE)m_DDSD.lpSurface;

        int xs = m_LinePitch>>2,
            xm = m_LinePitch*2,
            y = m_TargetFormat->bV4Height,
            z = m_DDSD.lPitch-m_LinePitch;

        EnterCriticalSection(&m_AccessBuffer);

		// I was to lazy to do the optimaization for all sizes
		// and all my AVI's are DWORD align i had no problem
		// i just putted back the old part for the odd AVIS ;)
		if( m_TargetFormat->bV4Width%4 )
		{	for(int i = 0; i < (int)m_TargetFormat->bV4Height; i++)
			{	memcpy(lpDest, lpSource, m_LinePitch);
				lpDest += m_DDSD.lPitch;
				lpSource -= m_LinePitch;
			}
		}
		else
		{
			__asm
			{   mov ebx, [y]
				mov esi, lpSource
				mov edi, lpDest
				cld

			lp: mov ecx, [xs]
				rep movsd

				sub esi, [xm]
				add edi, [z]

				dec ebx
				jnz lp
			}
		}
        LeaveCriticalSection(&m_AccessBuffer);
		rval = m_lpDDS->Unlock(NULL);
		while(rval == DDERR_SURFACELOST) 
		{
//			Restore();
			rval = m_lpDDS->Unlock(NULL);
		}
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi DrawBuffer
//////////////////////////////////////////////////////////////////////////////////
BOOL CAvi::Draw(INT	nLoopTime)
{
	BOOL rval = 0;

	ReadDatas(nLoopTime);

	rval = CAvi::DrawBuffer();

	if(rval != DD_OK) return rval;

	g_xMainWnd.GetBackBuffer()->Blt(&m_DstRect, m_lpDDS, &m_SrcRect, DDBLT_WAIT, NULL);

	return m_bIsPlaying;
}

//////////////////////////////////////////////////////////////////////////////////
// CAvi DrawBuffer
//////////////////////////////////////////////////////////////////////////////////
HRESULT CAvi::DrawFast(INT	nLoopTime,int X, int Y)
{
	HRESULT rval;

	ReadDatas(nLoopTime);
	rval = CAvi::DrawBuffer();
	if(rval != DD_OK) return rval;

	rval = g_xMainWnd.GetBackBuffer()->BltFast(X, Y, m_lpDDS, &m_SrcRect, DDBLTFAST_WAIT|DDBLTFAST_NOCOLORKEY);
//	if(rval == DDERR_SURFACELOST) Restore();

	return rval;
}

BOOL CAvi::ReadDatas(INT nLoopTime)
{
	static DWORD	dwDelay = 0;
	INT		nDivider;

	if(m_bPlaying)
	{
		dwDelay += nLoopTime;
		nDivider = 1000/m_Fps;
		if(dwDelay>nDivider)
		{
			NextFrame();
			dwDelay = 0;
			return TRUE;
		}
	}

	return FALSE;
}
