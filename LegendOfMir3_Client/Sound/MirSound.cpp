// MirSound.cpp: implementation of the CMirSound class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#define MAX_KINDS_OF_WAVE	50
//#define MAX_DUPLE_COUNT		5
#define MAX_LIM_TIME		60000


#define MAX_MP3_WAR			-1000
#define MAX_MP3_PEACE		-100

#define _PI					3.1415927

#define MAKEPOS(a1,a2,a3)	{a1.x = a2;a1.y = a3;}

void CALLBACK AmbianceTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime );
void CALLBACK FadeInTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime );
void CALLBACK FadeOutTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime );
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//  Name		:	CMirSound
//  Destination :	Constructor
//
//  Parameter	:	none
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
CMirSound::CMirSound()
{
	m_ppWavList = NULL;
	m_pWavListHeader = NULL;
	m_ppBuffer = NULL;
	m_3DEnable = FALSE;
	m_bPlay = TRUE;
	m_bBGMPlay = TRUE;	// 임시
	ReadWavFileList("SoundList.wwl");
	m_ppBuffer = new CSBuffer*[MAX_KINDS_OF_WAVE];
	for(int i = 0 ; i < MAX_KINDS_OF_WAVE ; i++)
	{
		m_ppBuffer[i] = new CSBuffer;
	}

	m_pMp3 = new CBMMp3;

	m_lWavMaxVol = -100;
	m_lMp3MaxVol = -1000;
	m_nUseCount = 0;
	m_bRunningTimer = FALSE;
	m_bIsWarMode = FALSE;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	~CMirSound
//  Destination :	Destructor
//
//  Parameter	:	none
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
CMirSound::~CMirSound()
{
	unsigned int i;

	if(m_ppWavList != NULL)
	{
		for(i=0;i<m_pWavListHeader->FieldCount;i++)
		{
			delete m_ppWavList[i];
		}
	delete[] m_ppWavList;
	}

	if(m_bAmbiance)
	{
		KillTimer(g_xMainWnd.GetSafehWnd(),ID_AMBIANCE_TIMER);
		for(i = 0 ; i < MAX_AMBIANCE_COUNT ; i++)
		{
			m_pAmbianceBuffer[i]->Release();
			delete m_pAmbianceBuffer[i];
		}
	}
	m_CommonWavBuffer.Release();

	if(m_ppBuffer!=NULL)
	{
		for(int j =0;j<m_nUseCount;j++)			// Buffer Class
		{
			m_ppBuffer[j]->Release();
			delete m_ppBuffer[j];
		}
		delete[] m_ppBuffer;
	}

	if(m_pWavListHeader != NULL)	delete m_pWavListHeader;
	if(m_pMp3!=NULL) delete m_pMp3;
	if(m_pSound!=NULL)		delete m_pSound;	// Sound Class 
}


///////////////////////////////////////////////////////////////////////
//  Name		:	CalsDistance
//  Destination :	Calculate Distance from User to Sound Source
//
//  Parameter	:	Position  Src
//					Position Chr
//
//	Return Value:	int Distance
//  Update		:
///////////////////////////////////////////////////////////////////////
float CMirSound::CalsDistanceX(POINT Src,POINT Chr)		// 소리 발생 위치와 현재 케릭터간의 거리 조사
{
	float l_Result;
	l_Result = (float)((Chr.x - Src.x)%13);						// 거리는 12 이상일수 없다.
	l_Result = (float)(1.0-(float)sqrt(1.0 - l_Result/(float)12.0))*((float)-1.0);
	return l_Result;
}

float CMirSound::CalsDistanceY(POINT Src,POINT Chr)		// 소리 발생 위치와 현재 케릭터간의 거리 조사
{
	float l_Result;
	l_Result = (float)((Chr.y- Src.y)%13);						// 거리는 12 이상일수 없다.
	l_Result =float(1.0-(float)sqrt(1.0 - (double)(l_Result)/12.0))*((float)-1.0);
	return l_Result;
}

INT CMirSound::CalsDistance(POINT Src,POINT Chr)
{
	INT Cx,Cy;
	INT nResult;
	Cx = Src.x - Chr.x;
	Cy = Src.y - Chr.y;
	Cx = Cx<0 ? Cx*(-1) : Cx;
	Cy = Cy<0 ? Cy*(-1) : Cy;
	
	nResult = Cx >Cy ? Cx : Cy;
	return nResult;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	CalsPan
//  Destination :	Calculate Pans value
//
//  Parameter	:	int Direction
//					int Distance
//
//	Return Value:	int Pans
//  Update		:
///////////////////////////////////////////////////////////////////////
int CMirSound::CalsPan(int Dir,int Dis)	
{
	int pans;
	pans = Dir*(int)(5000.0*(sin(_PI*(Dis/26.0))));

	return pans;				
}


///////////////////////////////////////////////////////////////////////
//  Name		:
//  Destination :
//
//  Parameter	:
//
//
//	Return Value:
//  Update		:
///////////////////////////////////////////////////////////////////////
int CMirSound::CalsVolume(int Dis) // 수정 요망
{
	int l_Result;
	if(Dis!=0)
	{
		l_Result = (int)((cos(_PI*(Dis/26.0)))*100);
		l_Result = (101 - l_Result);		// 최대 볼룸 0 최소 볼륨 (-1)*x : 현제의 100-(x)%
		l_Result = m_lWavMaxVol+l_Result*(-20);			// 최소음량 -2000 ~ 최대음량 0
		return l_Result;
	}
	else
	{
		return m_lWavMaxVol;							// 겹쳐 져 있거나 본인의 소리	최대 음량으로
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:
//  Destination :
//
//  Parameter	:
//
//
//	Return Value:
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::ReadWavFileList(char* fName)
{
	unsigned int i;
	int Count;
	DWORD dwReadSize;
	DWORD	dwError;
	WAVELIST* l_WavList;

	HANDLE hFile = ::CreateFile(fName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);

	dwError = GetLastError();
	if(dwError==ERROR_SUCCESS)
	{
		//	Header Reading
		m_pWavListHeader = new WAVELISTHEADER;

		ReadFile(hFile,m_pWavListHeader,sizeof(WAVELISTHEADER),&dwReadSize,NULL);
		//	fclose(hFile);	
		m_ppWavList = new WAVELIST*[m_pWavListHeader->FieldCount];

		// List Reading
		Count = 0;
		for(i=0;i<m_pWavListHeader->ListCount ; i++)
		{
			l_WavList = new WAVELIST;
			ZeroMemory(l_WavList,sizeof(WAVELIST));
			ReadFile(hFile,l_WavList,sizeof(WAVELIST),&dwReadSize,NULL);

			if(l_WavList->ID != 0) 
			{
				for(unsigned int j=0; j<strlen(l_WavList->Des);j++)
				{
					char	temp[4];
					int		cmpResult;
					strncpy(temp,l_WavList->Des+j,3);
					temp[3] = NULL;
					cmpResult = strcmp(temp,"wav");
					if(cmpResult==0)
					{
						*(l_WavList->Des+j+3)=NULL;
					}
				}
				l_WavList->Des;
				m_ppWavList[Count] = l_WavList;
				Count++;
			}
			else
			{
				delete l_WavList;
			}
		}
		CloseHandle(hFile);
	}
	else
	{
		m_bPlay = FALSE;
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:
//  Destination :
//
//  Parameter	:
//
//
//	Return Value:
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::PlayMagicEffect(POINT Target, POINT Chr, int Wavnum)
{
	if(m_bPlay)
	{
		FreeNotUseBuffer();
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::PlayAmbianceWav(int WavNum)
{
	if(m_bAmbiance)
	{
		FreeNotUseBuffer();
		m_pAmbianceBuffer[WavNum]->SetVolume(m_lWavMaxVol);
		m_pAmbianceBuffer[WavNum]->Play(0);
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:	PlayBkMusic
//  Destination :	Play Wave File with stream
//
//  Parameter	:	bool	looped
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::PlayBkMusicMp3(BOOL Looped,INT	nIndex)
{
	if(m_bBGMPlay)
	{
		CHAR		szMp3FileName[MAX_PATH];
		ZeroMemory(szMp3FileName,MAX_PATH);
		if(m_pMp3->IsPlaying())
		{
			// Play중
			m_pMp3->OnStop();
		}
//		FindBGMFileName(nIndex,szMp3FileName);
		strcpy(szMp3FileName,"1.mp3");	// 임시
		if(szMp3FileName[0]!=NULL)
		{
			m_pMp3->LoadMp3(szMp3FileName,g_xMainWnd.GetSafehWnd());
			m_pMp3->OnPlay(-5000,Looped);			
			BGMFadeIn();
		}
		else
		{
			
		}
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:	StopBkMusic
//  Destination :	Stop play BackGround Sound
//
//  Parameter	:	none
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::StopBkMusicMp3(void)
{
	BGMFadeOut();
}
///////////////////////////////////////////////////////////////////////
//  Name		:	PlayActorWav
//  Destination :	Play Wave File with any effect
//
//  Parameter	:	Position Mon
//					Position Chr
//					int WavNum
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
INT CMirSound::PlayActorWav(INT Sx,INT Sy,INT Dx,INT Dy,int Wavnum,INT lVolume,BOOL bLooping)
{
	CSBuffer*	pBuffer;
	char* fname;
	POINT	Mon,Chr;
	INT Dis;
	INT Dir;
	MAKEPOS(Mon,Sx,Sy);
	MAKEPOS(Chr,Dx,Sy);
 	if(m_bPlay)
	{
		FreeNotUseBuffer();
		fname = SeekFileName(Wavnum);
		if(fname != NULL)
		{
			pBuffer= FindBuffer(Wavnum,TRUE);
			if(pBuffer!=NULL)
			{
				if(m_3DEnable)
				{
						pBuffer->PlayExtended((float)CalsDistanceX(Mon,Chr),(float)CalsDistanceY(Mon,Chr),(float)0.1,0);	// 3D pan
				}
				else
				{
					Dis = CalsDistance(Mon,Chr);
					Dir = CalsDirection(Mon,Chr);
					if(bLooping)
					{
						pBuffer->PlayExtended(DSBPLAY_LOOPING,CalsPan(Dir,Dis),CalsVolume(Dis),0);										    // 2D pan
					}
					else
					{
						lVolume = CalsVolume(Dis)*((100-lVolume==0)?1:(100-lVolume)/5);
						pBuffer->PlayExtended(0,CalsPan(Dir,Dis),lVolume,0);										    // 2D pan
					}
					return pBuffer->nBufferIdx;
				}
			}
		}
	}
	return MAX_DUPLE_COUNT;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	StopAllSound
//  Destination :	Stop All playing Sound
//
//  Parameter	:	none
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::StopAllSound(void)
{
	for(int i = 0;i<m_nUseCount;i++)
	{
		m_ppBuffer[i]->Stop();
	}
}

///////////////////////////////////////////////////////////////////////
//  Name		:	StopSound
//  Destination :	Stop play Sound
//
//  Parameter	:	int nNum
//
//	Return Value:	void
//  Update		:
///////////////////////////////////////////////////////////////////////
void CMirSound::StopSound(int nNum)
{
	CSBuffer*	pBuffer;
	pBuffer = NULL;
	pBuffer = FindBuffer(nNum,FALSE);
	if(pBuffer != NULL)
		pBuffer->Stop();
}


void CMirSound::StopSound(int nNum,int BufIdx)
{
	CSBuffer*	pBuffer;
	pBuffer = NULL;
	pBuffer = FindBuffer(nNum,FALSE);
	if(pBuffer != NULL)
		pBuffer->Stop(BufIdx);
}


///////////////////////////////////////////////////////////////////////
//  Name		:	SeekFileName
//  Destination :	Find File name with Wave number
//
//  Parameter	:	int wavnum
//
//	Return Value:	char*
//  Update		:
///////////////////////////////////////////////////////////////////////
char* CMirSound::SeekFileName(int wavnum)
{
	unsigned int i;
	if(m_bPlay)
	{
		for(i=0;i<m_pWavListHeader->FieldCount;i++)
		{
			if(wavnum == m_ppWavList[i]->ID) 
				return m_ppWavList[i]->Des;
		}
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	FindBuffer
//  Destination :	
//
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
CSBuffer* CMirSound::FindBuffer(int nNum,BOOL ForLoad)	// 없을때 Load하는 것이면 TRUE 아니면 FASLE
{
	if(m_nUseCount>0)
	{
		for(int i=0 ; i<MAX_KINDS_OF_WAVE;i++)
		{
			if(m_ppBuffer[i]->m_ID==nNum)	// 읽어 놓은 것들중에 있냐?
			{
				return m_ppBuffer[i];
			}
			
		}
	}
	// 읽어 놓은것중에 없단말이냐?
	if(ForLoad)
	{
		if(m_nUseCount<MAX_KINDS_OF_WAVE)
		{
			char* fname;
			// 그럼 읽자!
			fname = SeekFileName(nNum);
			if(m_3DEnable) m_ppBuffer[m_nUseCount]->Enable3d();
			if(m_ppBuffer[m_nUseCount]->Load(m_pSound,fname,5)!=E_FAIL)
			{
				m_ppBuffer[m_nUseCount]->m_ID = nNum;
				m_nUseCount++;
				return m_ppBuffer[m_nUseCount-1];
			}
		}
		else
		{	// 헉! 읽을 공간 마저 없단 말이냐?
			// 그럼 말자!
			return NULL;
		}
	}
	return NULL;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
BOOL CMirSound::InitMirSound(HWND hWnd)
{
	if(hWnd != NULL)
	{
		m_hWnd = hWnd;
		m_pSound = new CSound;
		m_pSound->Create(m_hWnd,TRUE);
		return TRUE;
	}
	return FALSE;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::Enable3D(VOID)
{
	m_3DEnable = TRUE;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
INT CMirSound::CalsDirection(POINT Src,POINT Chr)
{
	return Src.x>Chr.x ? 1 :Src.x<Chr.x ?-1:0;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
BOOL CMirSound::FreeNotUseBuffer(VOID)
{
	DWORD	dwCurrentTime;
	DWORD	dwTempTime;
	CSBuffer*	pxTempBuffer;
	int i=0;

	dwCurrentTime = timeGetTime();		// 현재 시간을 구한다.

	do
	{
		dwTempTime = m_ppBuffer[i]->GetLastUsedTime();
		if(dwTempTime!=0)
		{
			dwTempTime = dwCurrentTime - dwTempTime;
			if(dwTempTime >  MAX_LIM_TIME)	// 1분 이상 안쓴것을 날려라
			{	
				m_ppBuffer[i]->Release();
				pxTempBuffer = m_ppBuffer[i];
				m_nUseCount--;
				for(int j = i; j<m_nUseCount;j++)
				{
					m_ppBuffer[j] = m_ppBuffer[j+1];
				}
				m_ppBuffer[m_nUseCount] = pxTempBuffer;
				i--;
			}
			i++;
		}
	}
	while(i<m_nUseCount);
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetEnableBGM(VOID)
{
	m_bBGMPlay = TRUE;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetDisableBGM(VOID)
{
	m_bBGMPlay = FALSE;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetEnablePlay(VOID)
{
	m_bPlay = TRUE;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetDisablePlay(VOID)
{
	m_bPlay = FALSE;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetDisableAmbiance(VOID)
{
	if(m_bAmbiance)
	{
		m_bAmbiance = FALSE;
		for(INT i = 0 ; i < MAX_AMBIANCE_COUNT ; i++)
		{
			m_pAmbianceBuffer[i]->Release();
			delete m_pAmbianceBuffer[i];
		}

//		m_CommonWavBuffer.Release();
		if(m_CommonWavBuffer.Load(m_pSound,"10.wav",1)!=E_FAIL)
		{
			m_CommonWavBuffer.m_ID = 1;
			m_CommonWavBuffer.SetVolume(m_lWavMaxVol);
			m_CommonWavBuffer.Play(0);
		}
		KillTimer(g_xMainWnd.GetSafehWnd(),ID_AMBIANCE_TIMER);
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetEnableAmbiance(INT nMapNum,INT nCount)
{
	CHAR	szPname[MAX_PATH];
	CHAR	szFname[MAX_PATH];
	CHAR	szTname[MAX_PATH];
	if(!m_bAmbiance)
	{
		m_nAmbienceNum = nMapNum;
		m_nAmbianceCount = nCount;
//		_itoa(m_nAmbienceNum,szFname,10);
		ZeroMemory(szPname,MAX_PATH);
//		strcat(szPname,szFname);
		if(m_CommonWavBuffer.Load(m_pSound,"00.wav",1)!=E_FAIL)		// FadeIn
		{
			m_CommonWavBuffer.SetVolume(m_lWavMaxVol);
			m_CommonWavBuffer.m_ID = 1;			// 엠비언스일경우에만 1로 해준다.
			m_CommonWavBuffer.Play(0);
		}

		for(INT i = 0 ; i < MAX_AMBIANCE_COUNT ; i++)
		{
			_itoa(i,szTname,10);
			strcat(szTname,".wav");
			strcpy(szFname,szPname);
			strcat(szFname,szTname);
			m_pAmbianceBuffer[i] = new CSBuffer;
			if(m_3DEnable)	m_pAmbianceBuffer[i]->Enable3d();
			m_pAmbianceBuffer[i]->Load(m_pSound,szFname,2);
			m_pAmbianceBuffer[i]->m_ID = 1;		// 엠비언스일경우에만 1로 해준다.
		}
		m_bAmbiance = TRUE;
		SetTimer(m_hWnd,ID_AMBIANCE_TIMER,1000,(TIMERPROC)(AmbianceTimerProc));
	}
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
BOOL CMirSound::BGMFadeIn(LONG	lMaxVol)
{
	if(!m_bRunningTimer)
	{
		if(m_pMp3->IsPlaying())
		{
			m_lMp3MaxVol = lMaxVol;
			SetTimer(m_hWnd,ID_TIMER_1,100,(TIMERPROC)(FadeInTimerProc));
			m_bRunningTimer = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
BOOL CMirSound::BGMFadeOut(LONG lTime,LONG	lMinVol)
{
	if(!m_bRunningTimer)
	{
		if(m_pMp3->IsPlaying())
		{
			lTime = (lTime*2)/100;
			m_lMp3MaxVol = lMinVol;
			SetTimer(m_hWnd,ID_TIMER_1,lTime,(TIMERPROC)(FadeOutTimerProc));
			m_bRunningTimer = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
HRESULT CMirSound::MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_DSHOW_NOTIFY:
			{
				m_pMp3->MessageProcess();
				break;
			}
	}
	return 0l;
}




///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::EndFading(INT nState)
{
	if(nState==0)
	{
		// End of Fade in
		KillTimer(g_xMainWnd.GetSafehWnd(),ID_TIMER_1);
		//
		
	}
	else
	{
		// End of Fade out
		if(m_lMp3MaxVol<=(-5000)) m_pMp3->OnStop();
		KillTimer(g_xMainWnd.GetSafehWnd(),ID_TIMER_1);
		//
	}
	m_bRunningTimer = FALSE;
}




///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetInWarMode(VOID)
{
	if(!m_bIsWarMode)
	{
		m_lMp3MaxVol =(-2000);
		BGMFadeOut(m_lMp3MaxVol);
		m_bIsWarMode = TRUE;
	}
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
VOID CMirSound::SetInPeaceMode(VOID)
{
	if(m_bIsWarMode)
	{
		m_lMp3MaxVol =(0);
		BGMFadeIn(m_lMp3MaxVol);
		m_bIsWarMode = FALSE;
	}
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
// MAX = 100 Min = 1
BOOL CMirSound::SetMasterVol(INT nVolume)
{
	if(nVolume>=1 && nVolume<=100)
	{
		m_lWavMaxVol = (nVolume-101)*(50);		// 100 -> -50    0 -> -5000
		m_lMp3MaxVol = (m_lWavMaxVol-1)*(10);		// Wav volume의 -10db
		return TRUE;
	}
	return FALSE;
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
// MAX = 100 Min = 1
BOOL CMirSound::SetBGMVol(INT nVolume)
{
	if(nVolume>=0 && nVolume<=100)
	{
		m_lMp3MaxVol = (m_lWavMaxVol-1)*(10)*(nVolume+1);		// Wav volume의 -10dB의 몇%?
		return TRUE;
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
BOOL CMirSound::SetWavVol(INT nVolume)
{
	return SetMasterVol(nVolume);
}



///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
void CALLBACK FadeInTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	LONG	lVolume;
	lVolume = g_xSound.m_pMp3->GetVolume();
	// if End of Fade-out SendMessage to Window WM_USER_ENDTIMER
	if(lVolume>=(g_xSound.m_lMp3MaxVol))
		SendMessage(g_xMainWnd.GetSafehWnd(),WM_USER_ENDTIMER,(WPARAM)0,NULL);
	lVolume = lVolume + 100;
	g_xSound.m_pMp3->SetVolume(lVolume);
}

///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
void CALLBACK FadeOutTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime )
{
	LONG	lVolume;
	lVolume = g_xSound.m_pMp3->GetVolume();
	// if End of Fade-out SendMessage to Window WM_USER_ENDTIMER
	if(lVolume<(g_xSound.m_lMp3MaxVol))
		SendMessage(g_xMainWnd.GetSafehWnd(),WM_USER_ENDTIMER,(WPARAM)1,NULL);
	lVolume = lVolume - 100;
	g_xSound.m_pMp3->SetVolume(lVolume);
}




///////////////////////////////////////////////////////////////////////
//  Name		:	
//  Destination :	
//
//  Parameter	:	
//
//	Return Value:	
//  Update		:
///////////////////////////////////////////////////////////////////////
void CALLBACK AmbianceTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime )
{
	static	INT nCount = 11;
	static	INT	nReply = 0;
	INT nNum;
	INT	Fact_1,Fact_2;
	Fact_1 = (rand())%10+1;
	Fact_2 = (rand())%10+1;

	nCount++;
	
	if((Fact_1+Fact_2)<=6)  // 15%이상의 확률로 Play   1~15% = 15%
		nNum = 1;
	else
		if((Fact_1+Fact_2)<=8)  // 28%이상의 확률로 Play  16~28% = 12%
			nNum = 2;
		else
			if((Fact_1+Fact_2)<=10)  // 45%이상의 확률로 Play   27~45%  = 18%
				nNum = 3;
			else
				if((Fact_1+Fact_2)<=11)  // 55%이상의 확률로 Play   46~55% = 10%
					nNum = 4;
				else
					if((Fact_1+Fact_2)<=13)  // 72%이상의 확률로 Play  56~72% = 16%
						nNum = 5;
					else
						if((Fact_1+Fact_2)<=16)  // 90%이상의 확률로 Play	72~90 = 18%
							nNum = 7;
						else
							nNum = 8;			 //	10%

	if(nCount>=13)
	{
		nNum = 0;
		nCount = 0;
		nReply++;
	}
	if(nNum >= 0 && nNum <= 8)
	{
		g_xSound.PlayAmbianceWav(nNum);		// 원래
	}
	if(nReply>=g_xSound.m_nAmbianceCount)
	{
		nReply = 0;
		g_xSound.SetDisableAmbiance();
		return;
	}
}

BOOL CMirSound::ChgPlayingSet(INT nWavIdx,INT nBufferIdx,INT Sx,INT Sy,INT Dx,INT Dy)
{
	POINT	Mon,Chr;
	INT Dis;
	INT Dir;
	CSBuffer*	pBuffer;
	MAKEPOS(Mon,Sx,Sy);
	MAKEPOS(Chr,Dx,Sy);

	pBuffer= FindBuffer(nWavIdx,FALSE);
	if(pBuffer!=NULL)
	{
		Dis = CalsDistance(Mon,Chr);
		Dir = CalsDirection(Mon,Chr);
		pBuffer->Pause(nBufferIdx);
		pBuffer->SetPan(nBufferIdx,CalsPan(Dir,Dis));
		pBuffer->SetVolume(nBufferIdx,CalsVolume(Dis));
		pBuffer->Continue(nBufferIdx);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}