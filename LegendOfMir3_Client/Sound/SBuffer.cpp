// SBuffer.cpp: implementation of the CSBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSBuffer::CSBuffer()
{
	m_pDS = NULL;
	m_lpDSB = NULL;
	m_Filename = NULL;
	m_Current = 0;
	m_dwLastUseTime = 0;
	m_nBuffers = 1;
	m_3d = FALSE;
	m_lp3dBuffer = NULL;
	m_Streamed = FALSE;
	m_sFile=NULL;
//	m_pbUsingBuffer = NULL;
	m_ID=0;
}

CSBuffer::~CSBuffer()
{
	Stop();
	Release();
}

VOID CSBuffer::Release(VOID)
{
	DWORD Status=0;
	if(m_ID!=0)
	{
		if( m_lpDSB )
		{
			for(int i = m_nBuffers-1; i >-1 ; i--)
			{
				if(SUCCEEDED(m_lpDSB[i]->GetStatus(&Status)))
				{
					if(Status&DSBSTATUS_PLAYING)
						m_lpDSB[i]->Stop();
					HRESULT hResult = m_lpDSB[i]->Release();
//					delete *m_lpDSB[i];
					m_lpDSB[i] = NULL;
				}
			}
			DELETEARRAY(m_lpDSB);
			m_lpDSB = NULL;
		}
		
		if( m_lp3dBuffer )
		{
			for(int i = 0; i < m_nBuffers; i++) SAFE_RELEASE(m_lp3dBuffer[i]);
			DELETEARRAY(m_lp3dBuffer);
		}
		
		if( m_Streamed && m_sStop )
		{
			if( m_sFile )
			{
				fclose(m_sFile);
				m_sFile=NULL;
			}
		}
		m_pDS = NULL;
		m_ID=0;
	}
}



HRESULT CSBuffer::Load(CSound *pDS, const char *Filename, int Num)
{
	CHAR	szFname[MAX_PATH];
	// Check the Parameters
	if( Filename == NULL ) return DSERR_INVALIDPARAM;
	
	HRESULT rval=DSERR_GENERIC;
	FILE *fptr;

	strcpy(szFname,"Sound\\");
	strcat(szFname,Filename);

	fptr = fopen( szFname, "rb" );
	if( fptr )
	{
		if(m_Streamed)
		{
			m_sStop=FALSE;
			m_sFile=fptr;
		}
		rval = Load( pDS, 0, fptr, Num );
		if(!m_Streamed) fclose( fptr );
		m_Filename = Filename;
	}
	return rval;
}

HRESULT CSBuffer::Load(CSound* pDS, LONG lSize, CHAR* lpCache, int Num)
{
    HRESULT rval;
    
    rval = Extract(pDS, lSize, lpCache, Num);

    return rval;
}

HRESULT CSBuffer::Load(CSound *pDS, LONG lSize, FILE *fptr, int Num)
{
    HRESULT rval;
    CHAR* lpCache;
    int length, save;

	// Release all allocated Memory, befor continue
//	m_nBuffers = Num;
	Release( );

    length = lSize;

    // If lSize equals zero get the size of the file.
    if(length == 0)
    {
        // Save the pointer location
        save = ftell(fptr);
        if(ferror(fptr))
            return NULL;

        // Seek to end of file
        fseek(fptr, 0, SEEK_END);
        if(ferror(fptr))
            return NULL;

        // Get the size of the file
        length = ftell(fptr);
        if(ferror(fptr))
            return NULL;

        // Seek back to save position
        fseek(fptr, save, SEEK_SET);
        if(ferror(fptr))
            return NULL;

    }

    // Cache the whole file in memory
    // Allocate memory to hold the data
    lpCache = new CHAR[length];
    if(lpCache == NULL)
        return NULL;

    // Read in the data
    fread(lpCache, 1, length, fptr);
    if(ferror(fptr))
    {
        delete [] lpCache;
        return NULL;
    }

    // Load it from memory.
    rval = Extract(pDS, length, lpCache, Num);

    // Delete the cache
    delete [] lpCache;

	return rval;
}


HRESULT CSBuffer::Extract(CSound *pDS, LONG lSize, char *ptr, int Num)
{
	HRESULT rval;

	// Release all memory, befor continue
	Release();
	
	// Check the Parameters
	if( ptr == NULL ) return DSERR_INVALIDPARAM;
	if( pDS == NULL ) return DSERR_INVALIDPARAM;
	if( pDS->GetDS() == NULL ) return DSERR_UNINITIALIZED;
	if( m_Streamed ) return DSERR_INVALIDPARAM;

	if( Num<1 ) Num = 1;
	
	m_pDS      = pDS;
	m_nBuffers = Num;
	m_Filename = NULL;
	
	if( strncmp( ptr, "RIFF", 4 ) || strncmp( ptr+8, "WAVE", 4 ) )
		return DSERR_BADFORMAT;

	DWORD hSize = *(DWORD*)(ptr+16);
	LPWAVEFORMATEX lpWav = (LPWAVEFORMATEX)(ptr+20);
	
	// skip all "fact" and search for the data
	// the fact stores the uncompressed filesize
	DWORD dSize = 0;
	ptr += 20 + hSize;
	do
	{
		if( dSize ) ptr += dSize + 8;
		    dSize = *(DWORD*)(ptr+4);
	} while( !strncmp( ptr, "fact", 4 ) );
	ptr += 8;

	// Is it compressed or a normal PCM Wave File
	if( lpWav->wFormatTag == WAVE_FORMAT_PCM )
	{
		// Create the sound buffer for the wave file
		rval = CreateSoundBuffer( dSize, (PCMWAVEFORMAT*)lpWav, Num );
		if(FAILED(rval)) return rval;
			
		// Read the data for the wave file into the sound buffer
		rval = ReadDataFromMemory( ptr, dSize );
		if(FAILED(rval)) return rval;
	}

		
	// Duplicate Buffers
	for( int i = 1; i < m_nBuffers; i++ )
	{
		rval = m_pDS->GetDS()->DuplicateSoundBuffer(m_lpDSB[0], &m_lpDSB[i]);
		if(FAILED(rval)) return rval;

		if( m_3d )
		{
			rval = m_lpDSB[0]->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&m_lp3dBuffer[i]);
			if(FAILED(rval)) return rval;
		}
	}
	
	return DS_OK;
	//return DSERR_UNSUPPORTED;
}



HRESULT CSBuffer::CreateSoundBuffer(DWORD dwBufSize, PCMWAVEFORMAT *pcmwf, int Num)
{
	HRESULT rval;
	// Create Space for Soundbuffer Pointers
	m_lpDSB = new LPDIRECTSOUNDBUFFER[Num];
	if( m_lpDSB == NULL ) return DSERR_OUTOFMEMORY;
	ZeroMemory( m_lpDSB, sizeof( LPDIRECTSOUNDBUFFER )*Num );
	
	if( m_3d )
	{
		m_lp3dBuffer = new LPDIRECTSOUND3DBUFFER[Num];
		if( m_lp3dBuffer == NULL ) return DSERR_OUTOFMEMORY;
		ZeroMemory( m_lp3dBuffer, sizeof( LPDIRECTSOUNDBUFFER )*Num );
	}
	
	DSBUFFERDESC dsbdesc;
	// Set up DSBUFFERDESC structure.
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
	dsbdesc.dwBufferBytes = dwBufSize; 
	dsbdesc.lpwfxFormat   = (LPWAVEFORMATEX)pcmwf;
	if( m_Streamed ) dsbdesc.dwFlags=DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLPOSITIONNOTIFY;
	else if( m_3d )  dsbdesc.dwFlags=DSBCAPS_CTRL3D|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLVOLUME;
	else dsbdesc.dwFlags=DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_STATIC;
	
	rval=m_pDS->GetDS()->CreateSoundBuffer(&dsbdesc, &m_lpDSB[0], NULL);
	if( FAILED(rval) ) return rval;
	
	if( m_3d )
	{
		rval=m_lpDSB[0]->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&m_lp3dBuffer[0]);
		if(FAILED(rval)) return rval;
	}
	
	return DS_OK;
}

HRESULT CSBuffer::ReadDataFromMemory(char *ptr, DWORD dwSize)
{
	// Lock data in buffer for writing
	LPVOID pData1, pData2;
	DWORD  dwData1Size, dwData2Size;
	HRESULT rval;
	
	rval = m_lpDSB[0]->Lock(0, 0, &pData1, &dwData1Size, &pData2, &dwData2Size, DSBLOCK_ENTIREBUFFER);

	if(FAILED(rval)) return rval;
	
	// Read in data(on dx3 read in first chunk)
	if(dwData1Size>0) 
	{
		memcpy(pData1, ptr, dwData1Size);
		ptr+=dwData1Size;
	}
	
	// Read in second chunk if necessary
	if(dwData2Size>0) memcpy(pData2, ptr, dwData2Size);
	
	// Unlock data in buffer
	rval = m_lpDSB[0]->Unlock(pData1, dwData1Size, pData2, dwData2Size);
	if(FAILED(rval)) return rval;
	
	return DS_OK;
}

HRESULT CSBuffer::Play( DWORD dwFlags )
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer = NULL;
	
	if( m_pDS==NULL ) return DSERR_UNINITIALIZED;
	if( m_pDS->GetSoundOn( ) ) return DS_OK;
	
	Buffer = GetFreeBuffer();
	if( Buffer==NULL ) return DSERR_ALLOCATED;
	
	if(m_Streamed)
	{
		m_sLoop=dwFlags;
		dwFlags=DSBPLAY_LOOPING;
	}

	m_dwLastUseTime = timeGetTime();
	rval=Buffer->Play(0, 0, dwFlags);
	if(FAILED(rval)) return rval;

	return DS_OK;
}

LPDIRECTSOUNDBUFFER CSBuffer::GetFreeBuffer(VOID)
{
	DWORD Status;
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer;
	Status = 0;
	nBufferIdx = MAX_DUPLE_COUNT;
	if(m_lpDSB==NULL) return NULL;
	if(m_ID!=0)
	{
		Buffer=m_lpDSB[m_Current];
		rval = Buffer->GetStatus(&Status);
		nBufferIdx = m_Current;
		if(FAILED(rval)) Status = 0;
		
		if((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
		{
			if(m_nBuffers > 1)
			{
				if (++m_Current >= m_nBuffers) 
				{
					m_Current = 0;
				}
				
				Buffer = m_lpDSB[m_Current];
				rval = Buffer->GetStatus(&Status);
				nBufferIdx = m_Current;				
				if(SUCCEEDED(rval) && (Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
				{	
					// Play중이면 소리를 멈추고 처음으로 돌린다.
/*					Buffer->Stop();
					Buffer->SetCurrentPosition(0);
					
*/				
					// Play중이면 소리를 다음 소리는 Play 하지 말것
					m_Current = m_Current==0 ? m_nBuffers-1 : m_Current-1;
					Buffer = NULL;
					nBufferIdx = MAX_DUPLE_COUNT;
				}
			}
			else 
			{
				Buffer=NULL;
				nBufferIdx = MAX_DUPLE_COUNT;
			}
		}
		
		if(Buffer && (Status & DSBSTATUS_BUFFERLOST))
			if(FAILED(Buffer->Restore()))
			{
				Buffer = NULL;
				nBufferIdx = MAX_DUPLE_COUNT;
			}
	}

	return Buffer;
}

HRESULT CSBuffer::EnableStreaming( int size, int blocks )
{
	if( m_3d ) return DSERR_ACCESSDENIED;
	m_Streamed=TRUE;
	m_sSize=size;
	m_sBlocks=blocks;
	return DS_OK;
}

HRESULT CSBuffer::Enable3d( void )
{
	if( m_Streamed ) return DSERR_ACCESSDENIED;
	m_3d=TRUE;
	return DS_OK;
}

HRESULT CSBuffer::LoadFromRes( CSound* pDS, const char *Filename, int Num )
{
	if( Filename == NULL ) return DSERR_INVALIDPARAM;
	
	HRSRC hResInfo;
	HGLOBAL hResData;

	void *pvRes;
	
	if((( hResInfo = FindResource(NULL, Filename, "WAV")) == NULL ) ) return DSERR_GENERIC;
	if(( hResData = LoadResource(NULL, hResInfo)) == NULL) return DSERR_GENERIC;
	if(( pvRes = LockResource(hResData)) == NULL) return DSERR_GENERIC;
	
	return CSBuffer::Extract( pDS, 0, (char*)pvRes, Num);
}

HRESULT CSBuffer::PlayExtended( DWORD dwFlags, int Pan, int Volume, DWORD freq )
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer = NULL;

	if( m_pDS==NULL ) return DSERR_UNINITIALIZED;
	if( m_pDS->GetSoundOn( ) ) return DS_OK;
	
	Buffer=GetFreeBuffer();
	if(Buffer==NULL) return DSERR_ALLOCATED;
	
	rval=Buffer->SetPan(Pan);
	if(FAILED(rval)) return rval;
	
	rval=Buffer->SetVolume(Volume);
	if(FAILED(rval)) return rval;
	
	rval=Buffer->SetFrequency(freq);
	if(FAILED(rval)) return rval;
	
	if(m_Streamed)
	{
		m_sLoop=dwFlags;
		dwFlags=DSBPLAY_LOOPING;
	}
	m_dwLastUseTime = timeGetTime();	
	rval=Buffer->Play(0, 0, dwFlags);
	if(FAILED(rval)) return rval;

	return DS_OK;
}


HRESULT CSBuffer::PlayExtended( float x, float y, float z, DWORD dwFlags )
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer = NULL;
	
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	if(m_pDS->GetSoundOn()) return DS_OK;
	if(m_lp3dBuffer==NULL) return DSERR_UNINITIALIZED;
	
	Buffer=GetFreeBuffer();
	if(Buffer==NULL) return DSERR_ALLOCATED;
	if(m_lp3dBuffer[m_Current]==NULL) return DSERR_UNINITIALIZED;
	
	rval=m_lp3dBuffer[m_Current]->SetPosition( x, y, z, DS3D_IMMEDIATE );
	if(FAILED(rval)) return rval;

	m_dwLastUseTime = timeGetTime();
	rval = Buffer->Play(0, 0, dwFlags);
	if(FAILED(rval)) return rval;

	return DS_OK;
}

HRESULT CSBuffer::Stop(VOID)
{
	HRESULT rval;
	
	if( m_pDS==NULL || m_lpDSB==NULL ) return DSERR_UNINITIALIZED;
	
	for(int i=0; i<m_nBuffers; i++)
	{
		if( m_lpDSB[i] )
		{
			rval=m_lpDSB[i]->Stop();
			if( rval!=DS_OK ) return rval;
	
			rval=m_lpDSB[i]->SetCurrentPosition(0);
			if( rval!=DS_OK ) return rval;
		}
	}
	return DS_OK;
}

HRESULT CSBuffer::Stop(INT nBufferIdx)
{
	HRESULT rval;
	
	if( m_pDS==NULL || m_lpDSB==NULL ) return DSERR_UNINITIALIZED;
	
	if( m_lpDSB[nBufferIdx] )
	{
		rval=m_lpDSB[nBufferIdx]->Stop();
		if( rval!=DS_OK ) return rval;
		
		rval=m_lpDSB[nBufferIdx]->SetCurrentPosition(0);
		if( rval!=DS_OK ) return rval;
	}
	return DS_OK;
}

void CSBuffer::SetVolume(LONG Volume)
{
	if(m_lpDSB!=NULL)
	{
		for(int i=0; i<m_nBuffers; i++)
			if( m_lpDSB[i] )
				m_lpDSB[i]->SetVolume(Volume);
	}
}

void CSBuffer::SetVolume(INT nBufferIdx,LONG Volume)
{
	if(m_lpDSB!=NULL)
	{
		if( m_lpDSB[nBufferIdx] )
			m_lpDSB[nBufferIdx]->SetVolume(Volume);
	}
}


void CSBuffer::SetPan(LONG Pan)
{	
	if(m_lpDSB!=NULL)
	{
		for(int i=0; i<m_nBuffers; i++)
			if(m_lpDSB[i])
				m_lpDSB[i]->SetPan(Pan);
	}
}

void CSBuffer::SetPan(INT nBufferIdx,LONG Pan)
{	
	if(m_lpDSB!=NULL)
	{
		if(m_lpDSB[nBufferIdx])
			m_lpDSB[nBufferIdx]->SetPan(Pan);
	}
}

// Play 되고 있는 겟수를 구한다.
DWORD CSBuffer::Playing(VOID)
{
	DWORD ctr=0, Status;
	if(m_lpDSB)
		for( int i=0; i<m_nBuffers; i++ )
			if(m_lpDSB[i])
				if( SUCCEEDED( m_lpDSB[i]->GetStatus(&Status) ) )
					if( Status & DSBSTATUS_PLAYING )
						ctr++;
					
	return ctr;
}

HRESULT CSBuffer::SetCone( float x, float y, float z, DWORD InsideAngle, DWORD OutsideAngle, long OutsideVolume )
{
	HRESULT rval;
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	if(m_lp3dBuffer==NULL) return DSERR_UNINITIALIZED;
	if(m_pDS->Get3dListener()==NULL) return DSERR_UNINITIALIZED;
	
	for( int i=0; i<m_nBuffers; i++ )
	{
		if(m_lp3dBuffer[i])
		{
			rval=m_lp3dBuffer[i]->SetConeOrientation( x, y, z, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetConeAngles( InsideAngle, OutsideAngle, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetConeOutsideVolume( OutsideVolume, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
		}
	}
	
	rval=m_pDS->Get3dListener()->CommitDeferredSettings();
	return rval;
}


HRESULT CSBuffer::Set3dProperties( DWORD Mode, float MinDistance, float MaxDistance, float VelX, float VelY, float VelZ )
{
	HRESULT rval;
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	if(m_lp3dBuffer==NULL) return DSERR_UNINITIALIZED;
	if(m_pDS->Get3dListener()==NULL) return DSERR_UNINITIALIZED;
	
	for( int i=0; i<m_nBuffers; i++ )
	{
		if(m_lp3dBuffer[i])
		{
			rval=m_lp3dBuffer[i]->SetMode( Mode, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetMinDistance( MinDistance, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetMaxDistance( MaxDistance, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetVelocity( VelX, VelY, VelZ, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
		}
	}	
	rval=m_pDS->Get3dListener()->CommitDeferredSettings();
	return rval;
}


HRESULT CSBuffer::FillStreamBuffer(int nr)
{
	LPVOID pData;
	DWORD  dwDataSize;
	HRESULT rval;
	DWORD numread;

	if(!m_Streamed) return DSERR_UNINITIALIZED;
	if(m_lpDSB==NULL) return DSERR_UNINITIALIZED;
	if(m_sFile==NULL) return DSERR_UNINITIALIZED;

	rval = m_lpDSB[0]->Lock(nr*m_sSize, m_sSize, &pData, &dwDataSize, 0, 0, 0);
	if(FAILED(rval)) return rval;


	numread=fread( pData, 1, dwDataSize, m_sFile );

	if( numread<dwDataSize )
	{
		if(m_sLoop)
		{
			fseek( m_sFile, -(long)m_sWaveLength, SEEK_CUR );
			fread( (char*)pData+numread, 1, dwDataSize-numread, m_sFile );
		}
		else
		{
			BYTE fill=128;
			if( numread>0 )	fill=*((BYTE*)pData+numread-1);
			FillMemory( (BYTE*)pData+numread, dwDataSize-numread, fill );

			if( m_sDone==255 )
			{
				m_sDone=nr;
				m_sStop=FALSE;
			}
		}
	}
	m_lpDSB[0]->Unlock(pData,dwDataSize,0,0);
	return DS_OK;
}

HRESULT CSBuffer::UpdateStream( BOOL FillAll )
{
	HRESULT rval;
	if(!m_Streamed) return DSERR_UNINITIALIZED;
	if(m_lpDSB==NULL) return DSERR_UNINITIALIZED;
	
	DWORD pos;
	rval=m_lpDSB[0]->GetCurrentPosition( NULL, &pos );
	if(FAILED(rval)) return rval;
	pos/=m_sSize;

	if( m_sDone<255 )
	{
		if( m_sDone!=pos )
		{
			if( m_sStop )
			{
				Stop();
				return 1;
			}
		}
		else m_sStop=TRUE;
	}

	do
	{
		if(pos!=m_sCurrent)
		{
			rval=FillStreamBuffer(m_sCurrent);
			if(FAILED(rval)) return rval;

			char str[255];
			sprintf( str, "PlayPos:%i\tFillPos:%i\tEndPos:%i\n", pos, m_sCurrent, m_sDone );
			OutputDebugString( str );

			m_sCurrent++;
			if( m_sCurrent>=m_sBlocks ) m_sCurrent=0;
		}
	}
	while( FillAll && pos!=m_sCurrent );
	
	return DS_OK;
}

DWORD CSBuffer::GetLastUsedTime(VOID)
{
	return 	m_dwLastUseTime;
}

VOID CSBuffer::Pause(INT nBufferIdx)
{
	if(m_lpDSB[nBufferIdx])
		m_lpDSB[nBufferIdx]->Stop();
}

VOID CSBuffer::Continue(INT nBufferIdx)
{
	if(m_lpDSB[nBufferIdx])
		m_lpDSB[nBufferIdx]->Play(0, 0, 0);
}

