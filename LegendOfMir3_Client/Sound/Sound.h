// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__DC5CA4B7_9E44_462D_BEF8_B1E4FDD01802__INCLUDED_)
#define AFX_SOUND_H__DC5CA4B7_9E44_462D_BEF8_B1E4FDD01802__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MAKEVOL(right,left) MAKELONG(((right*65535L)/100), ((left*65535L)/100))

class CSound  
{
public:

	HRESULT					Enable3d(VOID);
	HRESULT					SetFormat( long Frequency, BOOL Bit16, BOOL stereo );
	HRESULT					Create(void *hWnd, BOOL Exclusive);

	HRESULT					GetCaps(void);
	LPDIRECTSOUND3DLISTENER Get3dListener( void ) { return m_pDSListener;}
	LPDIRECTSOUND			GetDS( void ) { return m_pDS; }
	BOOL					GetSoundOn(void) { return m_SoundOff; }

	DWORD					GetCDVolume( void );
	DWORD					GetDIGIVolume( void );
	DWORD					GetMIDIVolume( void );

	HRESULT					SetMIDIVolume( BYTE rvol, BYTE lvol );
	HRESULT					SetDIGIVolume( BYTE rvol, BYTE lvol );
	HRESULT					SetCDVolume( BYTE rvol, BYTE lvol );
	
	HRESULT					ContinuousPlay( BOOL Enable );

	HRESULT					Set3dFactors( float DopplerFactor, float RolloffFactor, float DistanceFactor );
	HRESULT					Place3dListener( float posX, float posY, float posZ, float frontX=0.0, float frontY=0.0, float frontZ=1.0, float topX=0.0, float topY=1.0, float topZ=0.0, BOOL setOrientation=FALSE );
		
	VOID					FreeDirectSound(VOID);

	CSound();
	virtual ~CSound();


private:
	LPDIRECTSOUND3DBUFFER   m_pDS3DBuffer;				// 3D sound buffer


	DS3DBUFFER              m_dsBufferParams;           // 3D buffer properties
	DS3DLISTENER			m_dsListenerParams;         // Listener properties
	DWORD                   m_dwBufferBytes;
	DWORD					m_dwDSBufferSize;
	BOOL					m_bDeferSettings;


	LPDIRECTSOUND			m_pDS;						// DirectSound object	
	BOOL					m_SoundOff;
	BOOL					m_3dSound;					// Stores if 3d Sound is enabled
	LPDIRECTSOUND3DLISTENER m_pDSListener;				// 3D listener object
	DSCAPS					m_DSCaps;					// Direct sound capabilities
};

#endif // !defined(AFX_SOUND_H__DC5CA4B7_9E44_462D_BEF8_B1E4FDD01802__INCLUDED_)
