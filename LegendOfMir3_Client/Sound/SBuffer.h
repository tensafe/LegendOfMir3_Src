// SBuffer.h: interface for the CSBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SBUFFER_H__9B72C71D_B3EA_4B35_B9FD_1EF3906BDDF1__INCLUDED_)
#define AFX_SBUFFER_H__9B72C71D_B3EA_4B35_B9FD_1EF3906BDDF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<stdio.h>
#include<windows.h>
#include "Sound.h"

class CSBuffer  
{
public:
	DWORD GetLastUsedTime(VOID);
	INT		m_ID;					// Buffer ID;
	INT	   nBufferIdx;

	CSBuffer();
	virtual ~CSBuffer();

	HRESULT Load(CSound* pDS, const char *Filename, int Num=1);
	HRESULT Load(CSound* pDS, LONG lSize, FILE *fptr, int Num=1);
	HRESULT Load(CSound* pDS, LONG lSize, CHAR* lpCache, int Num=1);

	HRESULT Extract(CSound* pDS, LONG lSize, char *ptr, int Num=1);
	HRESULT Play( DWORD dwFlags );
	HRESULT PlayExtended( DWORD dwFlags, int Pan, int Volume, DWORD freq );
	HRESULT PlayExtended( float x, float y, float z, DWORD dwFlags );
	HRESULT Stop(VOID);
	HRESULT Stop(INT nBufferIdx);

	VOID	SetVolume(INT nBufferIdx,LONG Volume);
	VOID	SetVolume(LONG Volume);
	VOID	SetPan(INT nBufferIdx,LONG Pan);
	VOID	SetPan(LONG Pan);

	VOID	Pause(INT nBufferIdx);
	VOID	Continue(INT nBufferIdx);

	DWORD	Playing(VOID);

	HRESULT	SetCone( float x, float y, float z, DWORD InsideAngle, DWORD OutsideAngle, long OutsideVolume );
	HRESULT Set3dProperties( DWORD Mode, float MinDistance, float MaxDistance, float VelX, float VelY, float VelZ );

	HRESULT	FillStreamBuffer(int nr);
	HRESULT	UpdateStream( BOOL FillAll );


	HRESULT ReadDataFromMemory(char *ptr, DWORD dwSize);
	HRESULT LoadFromRes( CSound* pDS, const char* Filename, int Num = 1 );
	HRESULT CreateSoundBuffer(DWORD dwBufSize, PCMWAVEFORMAT *pcmwf, int Num);
	LPDIRECTSOUNDBUFFER GetFreeBuffer(VOID);
	HRESULT EnableStreaming( int size, int blocks );
	HRESULT Enable3d( void );

	VOID Release(VOID);

private:
	int m_nBuffers;					// The number of sound buffers created
	int m_Current;					// The currently selected buffer
	const char* m_Filename;			// The name of the WAVE file
	DWORD	m_dwLastUseTime;		// Last Used Time;
	CSound* m_pDS;				// Contains a pointer to the CDXSound object
	LPDIRECTSOUNDBUFFER *m_lpDSB;	// The IDirectSoundBuffer object
	LPDIRECTSOUND3DBUFFER *m_lp3dBuffer;
	BOOL m_Streamed;				// The playback will be streamed	
	BOOL m_sLoop;
	BOOL m_sStop;
	BYTE m_sDone;
	WORD m_sSize;					// size of each stream block
	BYTE m_sBlocks, m_sCurrent;		// Stream Block Count and Current position
	DWORD m_sWaveLength;
	FILE *m_sFile;					// Filepointer for Streaming
	BOOL m_3d;						// Enables 3d functions for this Buffer
};

#endif // !defined(AFX_SBUFFER_H__9B72C71D_B3EA_4B35_B9FD_1EF3906BDDF1__INCLUDED_)
