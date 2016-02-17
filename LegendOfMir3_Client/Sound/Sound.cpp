// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#define ORBIT_MAX_RADIUS        5.5f
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSound::~CSound()
{
	FreeDirectSound();
}

CSound::CSound()
{
	m_pDS = NULL;
	m_pDS3DBuffer = NULL;
//	m_pWaveSoundRead = NULL;
	m_3dSound = FALSE;				// ENABLE 3D?
	m_SoundOff = TRUE;
	m_pDSListener = NULL;
}

HRESULT CSound::GetCaps(void)
{
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	return m_pDS->GetCaps(&m_DSCaps);
}

HRESULT CSound::ContinuousPlay( BOOL Enable )
{
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;

	LPDIRECTSOUNDBUFFER lpDSB;
    DSBUFFERDESC dsbdesc;
	HRESULT rval;

	// Get primary SoundBuffer
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER; 
	rval = m_pDS->CreateSoundBuffer(&dsbdesc, &lpDSB, NULL);
	if( FAILED(rval) ) return rval;

	if( Enable ) rval=lpDSB->Play( 0, 0, DSBPLAY_LOOPING );
	else rval=lpDSB->Stop( );
	SAFE_RELEASE( lpDSB );

	return rval;
}



HRESULT CSound::Create(void *hWnd, BOOL Exclusive)
{
	HRESULT rval;
	m_bDeferSettings = 0;

	rval = DirectSoundCreate(NULL, &m_pDS, NULL);
	if(rval != DS_OK) return rval;

	rval = m_pDS->SetCooperativeLevel((HWND)hWnd, (Exclusive) ? DSSCL_EXCLUSIVE : DSSCL_NORMAL );
	if(rval != DS_OK) return rval;

	m_SoundOff=FALSE;
	if( Exclusive )
		return SetFormat( 22050, TRUE, TRUE );

	return DS_OK;
}


HRESULT CSound::SetFormat(long Frequency, BOOL Bit16, BOOL stereo)
{
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	LPDIRECTSOUNDBUFFER pDSB;
    DSBUFFERDESC dsbdesc;
	WAVEFORMATEX wfx;
	HRESULT rval;

	// Get primary SoundBuffer
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER; 
	rval = m_pDS->CreateSoundBuffer(&dsbdesc, &pDSB, NULL); 
	if( FAILED(rval) ) return rval;

	// Set the desired Format
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec=Frequency;
	wfx.wBitsPerSample=(Bit16) ? 16 : 8;
	wfx.nChannels=(stereo) ? 2 : 1;
	wfx.nBlockAlign=wfx.wBitsPerSample/8*wfx.nChannels;
	wfx.nAvgBytesPerSec=Frequency*wfx.nBlockAlign;
	rval=pDSB->SetFormat( &wfx );

	SAFE_RELEASE( pDSB );
	return rval;
}

VOID CSound::FreeDirectSound(VOID)
{
    // Release DirectSound interfaces
    SAFE_RELEASE( m_pDSListener );
    SAFE_RELEASE( m_pDS ); 
}

HRESULT CSound::Enable3d(VOID)
{
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	LPDIRECTSOUNDBUFFER lpDSB;
    DSBUFFERDESC dsbdesc;
	HRESULT rval;

	// Get primary SoundBuffer
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_CTRL3D|DSBCAPS_PRIMARYBUFFER; 
	rval = m_pDS->CreateSoundBuffer(&dsbdesc, &lpDSB, NULL);
	if( FAILED(rval) ) return rval;

	rval=lpDSB->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&m_pDSListener);
	if( FAILED(rval) ) return rval;
	SAFE_RELEASE( lpDSB );

	m_3dSound = TRUE;
	return DS_OK;
}

HRESULT CSound::Set3dFactors( float DopplerFactor, float RolloffFactor, float DistanceFactor )
{
	HRESULT rval;
	if(m_pDSListener==NULL) return DSERR_UNINITIALIZED;

	rval=m_pDSListener->SetDopplerFactor( DopplerFactor, DS3D_DEFERRED );
	if(FAILED(rval)) return rval;

	rval=m_pDSListener->SetRolloffFactor( RolloffFactor, DS3D_DEFERRED );
	if(FAILED(rval)) return rval;

	rval=m_pDSListener->SetDistanceFactor( DistanceFactor, DS3D_DEFERRED );
	if(FAILED(rval)) return rval;

	return m_pDSListener->CommitDeferredSettings();
}

HRESULT CSound::Place3dListener( float posX, float posY, float posZ, float frontX, float frontY, float frontZ, float topX, float topY, float topZ, BOOL setOrientation )
{
	HRESULT rval;
	if(m_pDSListener==NULL) return DSERR_UNINITIALIZED;

	rval=m_pDSListener->SetPosition( posX, posY, posZ, DS3D_DEFERRED ); //DS3D_IMMEDIATE
	if(FAILED(rval)) return rval;

	if( setOrientation )
	{
		rval=m_pDSListener->SetOrientation( frontX, frontY, frontZ, topX, topY, topZ, DS3D_DEFERRED );
		if(FAILED(rval)) return rval;
	}

	return m_pDSListener->CommitDeferredSettings();
}

HRESULT CSound::SetMIDIVolume( BYTE rvol, BYTE lvol )
{
	MIDIOUTCAPS midiCaps;
	if( FAILED( midiOutGetDevCaps(0, &midiCaps, sizeof(midiCaps)) ) ) 
		return DSERR_NODRIVER;

	if( midiCaps.dwSupport&MIDICAPS_VOLUME )
		return midiOutSetVolume( 0, MAKEVOL(rvol, lvol) );

	return DSERR_UNSUPPORTED;
}


HRESULT CSound::SetDIGIVolume( BYTE rvol, BYTE lvol )
{
	WAVEOUTCAPS waveCaps;
	if( FAILED( waveOutGetDevCaps(0, &waveCaps, sizeof(waveCaps)) ) )
		return DSERR_NODRIVER;

	if( waveCaps.dwSupport&WAVECAPS_VOLUME )
		return waveOutSetVolume( 0, MAKEVOL(rvol, lvol) );

	return DSERR_UNSUPPORTED;
}

HRESULT CSound::SetCDVolume( BYTE rvol, BYTE lvol )
{
	AUXCAPS auxCaps;
	if( FAILED( auxGetDevCaps(0, &auxCaps, sizeof(auxCaps)) ) )
		return DSERR_NODRIVER;

	if( auxCaps.dwSupport&AUXCAPS_VOLUME )
		return auxSetVolume( 0, MAKEVOL(rvol, lvol) );

	return DSERR_UNSUPPORTED;
}

DWORD CSound::GetMIDIVolume( void )
{
	DWORD vol;
	midiOutGetVolume(0, &vol);
	return vol;
}

DWORD CSound::GetDIGIVolume( void )
{
	DWORD vol;
	waveOutGetVolume(0, &vol);
	return vol;
}

DWORD CSound::GetCDVolume( void )
{
	DWORD vol;
	auxGetVolume(0, &vol);
	return vol;
}