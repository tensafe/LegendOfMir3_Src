// NPCTxtAnalysis.cpp: implementation of the CNPCTxtAnalysis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define LINE_DIVIDER				0x01			// Line Divider

#define DEFAULT_TEXT_HEIGHT			16				// Default Text Height(as Pixel)
#define DEFAULT_TEXT_WIDTH			16				// Default Text Width(as Pixel)

#define	DEFAULT_TXT_COLOR			RGB(255,255,255)	// WHITE COLOR
#define DEFAULT_BTN_COLOR			RGB(255,255,0)		// YELLOW COLOR
#define DEFAULT_BTN_CLICK_COLOR		RGB(255,0,0)		// RED COLOR

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNPCTxtAnalysis::CNPCTxtAnalysis()
{
	m_pxBtn = NULL;
	m_pxpTxt = NULL;
	m_pSourceText = NULL;
	m_bReady = FALSE;
	m_bInited = FALSE;
	m_pxGWND = NULL;
}

CNPCTxtAnalysis::~CNPCTxtAnalysis()
{
	FreeAllMsg();
}

BOOL CNPCTxtAnalysis::InitAnalysis(CTextButton *xpBtn,CPDLList<CMText>*	pxpTxt,CWHDXGraphicWindow*	pxGWND)
{
	m_pxBtn = xpBtn;									// Link to Text Button Class
	m_pxpTxt = pxpTxt;
	ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);
	m_pxGWND=pxGWND;
	m_bInited = TRUE;
	return TRUE;
}

BOOL CNPCTxtAnalysis::FreeAllMsg(VOID)
{
	if(m_bInited)
	{
		m_pxpTxt->ClearAllNodes();							// 분석된 글 모두 날리기
		m_pxBtn->ReleaseButtons();							// 생성된 단투 모두 날리기
	}

	if(m_pSourceText!=NULL)
	{
		delete[] m_pSourceText;								// 받은 소스도 모두 날리기
		m_pSourceText = NULL;
	}

	m_bReady = FALSE;
	return TRUE;
}

// Divider 는 각 CHAR별로 모두 검출한다.
// 결과는 받는 즉시 Memcpy를 할것.. 아니면 쓰레기 값으 됨...
CHAR* CNPCTxtAnalysis::GetValidStr(CHAR* szSource,CHAR* szDest,CHAR* Divider)
{
	CHAR* szBuffer;
	int nSrcLen;
	int	nBuf_Count,nCount;
	int nArrCount;

	szBuffer = new CHAR[MAX_BUFFER_SIZE];
	nSrcLen = strlen(szSource);

	nBuf_Count	=	0;
	nCount		=	0;

	if (nSrcLen >= MAX_BUFFER_SIZE - 1 )				// 최대 길이보다 길면 실행을 취소 한다.
	{
		ZeroMemory(szDest,sizeof(szDest));
		delete[] szBuffer;
		return NULL;
	}

	if (strlen(szSource)==0)						// 소스 Text가 없으면 실행을 취소 한다.
	{
		ZeroMemory(szDest,sizeof(szDest));
		delete[] szBuffer;
		return szDest;
	}

	nArrCount = strlen(Divider)/sizeof(char);		// Divider의 길이를 구한다.

	while(true)
	{
		char Ch;

		if( nCount <= nSrcLen )
		{
			Ch=szSource[nCount];
			for(int i = 0; i<= nArrCount-1;i++)		
			{
				if(Ch == Divider[i])				// Divider 검출
				{
					goto CATCH_DIV;
				}
			}
		}
		if(nCount > nSrcLen)
		{
			CATCH_DIV:
			if(nBuf_Count >0)
			{
				if(nBuf_Count < MAX_BUFFER_SIZE -1)
				{
					szBuffer[nBuf_Count] = '\0';	// End String (sz)
					strcpy(szDest,szBuffer);
					strncpy(m_InnerResult,szSource + nCount+1,nSrcLen-nCount);
				}
				break;
			}
			else
			{
				if(nCount>nSrcLen)
				{
					ZeroMemory(szDest,sizeof(szDest));
					strncpy(m_InnerResult,szSource +nCount+2,nSrcLen-1);
					break;
				}
			}
		}
		else
		{
			if(nBuf_Count< MAX_BUFFER_SIZE -1)
			{
				szBuffer[nBuf_Count] = Ch;
				nBuf_Count++;
			}
		}
		nCount++;
	}

	delete[] szBuffer;
	return m_InnerResult;
}


CHAR* CNPCTxtAnalysis::GetValidStr(CHAR* szSource,CHAR* szDest,CHAR Divider)
{
	CHAR* szBuffer;
	int nSrcLen;
	int	nBuf_Count,nCount;

	szBuffer = new CHAR[MAX_BUFFER_SIZE];
	nSrcLen = strlen(szSource);

	nBuf_Count	=	0;
	nCount		=	0;

	if (nSrcLen >= MAX_BUFFER_SIZE - 1 )				// 최대 길이보다 길면 실행을 취소 한다.
	{
		ZeroMemory(szDest,sizeof(szDest));
		delete[] szBuffer;
		return NULL;
	}

	if (strlen(szSource)==0)							// 소스 Text가 없으면 실행을 취소 한다.
	{
		ZeroMemory(szDest,sizeof(szDest));
		delete[] szBuffer;
		return szDest;
	}

	while(true)
	{
		char Ch;

		if( nCount <= nSrcLen )
		{
			Ch=szSource[nCount];
			if(Ch == Divider)				// Divider 검출
			{
				goto CATCH_DIV;
			}
		}
		if(nCount > nSrcLen)
		{
			CATCH_DIV:
			if(nBuf_Count >0)
			{
				if(nBuf_Count < MAX_BUFFER_SIZE -1)
				{
					szBuffer[nBuf_Count] = '\0';	// End String (sz)
					strcpy(szDest,szBuffer);
					ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);
					if(nSrcLen-nCount>0) 
						strncpy(m_InnerResult,szSource + nCount+1,nSrcLen-nCount);
				}
				break;
			}
			else
			{
				if(nCount>nSrcLen)
				{
					ZeroMemory(szDest,sizeof(szDest));
					ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);
					if(nSrcLen-1>0) strncpy(m_InnerResult,szSource +nCount+2,nSrcLen-1);
					break;
				}
			}
		}
		else
		{
			if(nBuf_Count< MAX_BUFFER_SIZE -1)
			{
				szBuffer[nBuf_Count] = Ch;
				nBuf_Count++;
			}
		}
		nCount++;
	}
	if(szBuffer!=NULL)
	{
		delete[] szBuffer;
		szBuffer = NULL;
	}

	return m_InnerResult;
}

CHAR* CNPCTxtAnalysis::GetValidStr(CHAR* szSource,INT nSrcLen,CHAR* szDest,INT nDestLen,CHAR Divider)
{
	CHAR* szBuffer;
	int	nBuf_Count,nCount;

	szBuffer = new CHAR[MAX_BUFFER_SIZE];

	nBuf_Count	=	0;
	nCount		=	0;

	if (nSrcLen >= MAX_BUFFER_SIZE - 1 )				// 최대 길이보다 길면 실행을 취소 한다.
	{
		ZeroMemory(szDest,nDestLen);
		delete[] szBuffer;
		return NULL;
	}

	if (strlen(szSource)==0)							// 소스 Text가 없으면 실행을 취소 한다.
	{
		ZeroMemory(szDest,nDestLen);
		delete[] szBuffer;
		return szDest;
	}

	while(true)
	{
		char Ch;

		if( nCount <= nSrcLen )
		{
			Ch=szSource[nCount];
			if(Ch == Divider)				// Divider 검출
			{
				goto CATCH_DIV;
			}
		}
		if(nCount > nSrcLen)
		{
			CATCH_DIV:
			if(nBuf_Count >0)
			{
				if(nBuf_Count < MAX_BUFFER_SIZE -1)
				{
					szBuffer[nBuf_Count] = '\0';	// End String (sz)
					memcpy(szDest,szBuffer,nBuf_Count);
					ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);
					if(nSrcLen-nCount>0) 
						strncpy(m_InnerResult,szSource + nCount+1,nSrcLen-nCount);
				}
				break;
			}
			else
			{
				if(nCount>nSrcLen)
				{
					ZeroMemory(szDest,nDestLen);
					ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);
					if(nSrcLen-1>0) strncpy(m_InnerResult,szSource +nCount+2,nSrcLen-1);
					break;
				}
			}
		}
		else
		{
			if(nBuf_Count< MAX_BUFFER_SIZE -1)
			{
				szBuffer[nBuf_Count] = Ch;
				nBuf_Count++;
			}
		}
		nCount++;
	}
	if(szBuffer!=NULL)
	{
		delete[] szBuffer;
		szBuffer = NULL;
	}

	return m_InnerResult;
}

CHAR* CNPCTxtAnalysis::ArrestString(CHAR* szSource,CHAR SearchAfter,CHAR AssertBefore,CHAR* AssertStr)
{
	int Src_Len;
	bool Right_Data,Fin;

	ZeroMemory(AssertStr,strlen(AssertStr));
	ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);

	if(strlen(szSource)==0)
	{
		ZeroMemory(AssertStr,sizeof(AssertStr));
		return m_InnerResult;
	}
	int n;
	Src_Len = strlen(szSource);
	Right_Data = false;
	if(Src_Len >=2)
	{
		if(szSource[0] == SearchAfter)
		{
			strncpy(szSource,szSource+1,Src_Len-1);
			szSource[Src_Len-1]=NULL;
			Src_Len = strlen(szSource);
			Right_Data = true;
		}
		else
		{
			n= FindChr(szSource,SearchAfter);
			if(n>=0)
			{
				strncpy(szSource,szSource+n,Src_Len - (n));
				Src_Len = strlen(szSource);
				Right_Data = true;
			}
		}

		Fin = false;

		if(Right_Data)
		{
			n = FindChr(szSource,AssertBefore);
			if(n>=0)
			{
				strncpy(AssertStr,szSource,n);
				strncpy(m_InnerResult,szSource+n,Src_Len - n);
			}
			else
			{
				strncat(m_InnerResult,&SearchAfter,1);
				strcat(m_InnerResult,szSource);
			}
		}
		else
		{
			for(int i = 0; i< Src_Len;i++)
			{
				if(szSource[i] == SearchAfter)
				{

					strncpy(m_InnerResult,szSource+i,Src_Len-i+2);
					break;
				}
			}
		}
	}
	return m_InnerResult;
}


CHAR* CNPCTxtAnalysis::UpperCase(CHAR* szStr)
{
	int Len;
	ZeroMemory(m_InnerResult,MAX_BUFFER_SIZE);
	Len = strlen(szStr);
	for(int i = 0;i<Len;i++)
	{
		if(szStr[i]>'a' && szStr[i]<'z')
		{
			m_InnerResult[i] = szStr[i]-32;
		}
		else
		{
			m_InnerResult[i] = szStr[i];
		}
	}
	return m_InnerResult;
}

CHAR CNPCTxtAnalysis::UpperCase(CHAR chr)
{
	char szResult;
	szResult=0x00;

	szResult = (chr>'a' && chr<'z') ? chr-32:chr;

	return szResult;
}

int CNPCTxtAnalysis::FindChr(CHAR* szstr,CHAR ch)
{
	int Count = 0;
	bool Conditions=true;
	while(Conditions && Count<((int)strlen(szstr)))
	{
		if(szstr[Count]==ch)
			Conditions = false;
		Count++;
	}

	if(!Conditions) return Count;
	else return -1;
}

int CNPCTxtAnalysis::FindChr(CHAR* szStr,CHAR* ch)
{
	int Count = 0;
	int nLenObject;
	bool Conditions=true;

	nLenObject = strlen(ch);

	while(Conditions && Count <((int)strlen(szStr)))
	{
		if(szStr[Count]==ch[0])
		{
			int nChk=0;
			for(int i = 0 ; (i <nLenObject)&&(Count+i<(int)strlen(szStr)); i++)
			{
				if(szStr[Count+i]==ch[i])
					nChk++;
			}
			if(nChk == nLenObject)
				Conditions = false;
			// Text가 모두 일치 할 경우만 검출
		}
		Count++;
	}

	if(!Conditions) return Count;
	else return -1;
}

int CNPCTxtAnalysis::StrLength(CHAR* str)
{
	int Counter = 0;
	while(str[Counter++]!='\0');
	return Counter;
}


void CNPCTxtAnalysis::AnalySisText(CDLList<CHAR*>* pxpTxt,INT nLineMax)
{
	BOOL	DrawCenter = FALSE;
	CHAR*	szTemp;
	CHAR*	CmdStr;
	CHAR*	CmdParamStr;
	CHAR*	szStr;

	INT	nPosX = 20;;
	INT nLineCount = 0;
	COLORREF	cColor = RGB(1,1,1);

	SIZE	tSize;

	if(m_bInited)
	{

		// Init Buffers
		CmdParamStr	=	new CHAR[MAX_BUFFER_SIZE];
		CmdStr		=	new CHAR[MAX_BUFFER_SIZE];
		szStr		=	new CHAR[MAX_BUFFER_SIZE];

		ZeroMemory(CmdStr,MAX_BUFFER_SIZE);
		ZeroMemory(szStr,MAX_BUFFER_SIZE);

		pxpTxt->MoveCurrentToTop();
		while(true)
		{
			if(nLineMax<=nLineCount) 
				break;
			strcpy(szStr,*pxpTxt->GetCurrentData());	//	Line Divide

			if(strlen(szStr)!=0)
			{

				while((FindChr(szStr,'<')>0)&&(FindChr(szStr,'>')>0) && (strlen(szStr)!=0))
				{
					CHAR*	Fdata;
					Fdata	=	new CHAR[MAX_BUFFER_SIZE];
					ZeroMemory(Fdata,MAX_BUFFER_SIZE);
					// <>Tag 분리 Text Button 생성
					if((szStr[0])!='<')
					{
						szTemp =  GetValidStr(szStr,Fdata,'<');
						strcpy(szStr,"<");
						strcat(szStr,szTemp);				
					}	// endif

					// <>안에 있는 내용 CmdStr로
					szTemp = ArrestString(szStr,'<','>',CmdStr);
					strcpy(szStr,szTemp);
					szTemp = NULL;
					
					if(strlen(CmdStr)!=0)
					{
						switch(UpperCase(CmdStr[0]))
						{
							case '0':		// <0> 일경우 글자색 BLACK
							{
								cColor = RGB(0,0,0);
							}
							break;
							case '1':		// <1> 일경우 글자색 RED
							{
								cColor = RGB(255,0,0);
							}
							break;
							case '2':		// <2> 일경우 글자색 GREEN
							{
								cColor = RGB(0,128,0);
							}
							break;
							case '3':		// <3> 일경우 글자색 Olive
							{
								cColor = RGB(128,128,0);
							}
							break;
							case '4':		// <4> 일경우 글자색 GRAY
							{
								cColor = RGB(128,128,128);
							}
							break;
							case '5':		// <5> 일경우 글자색 Maroon
							{
								cColor = RGB(128,0,0);
							}
							break;
							case '6':		// <6> 일경우 글자색 Teal
							{
								cColor = RGB(0,128,128);
							}
							break;
							case '7':		// <7> 일경우 글자색 Navy
							{
								cColor = RGB(0,0,128);
							}
							break;
							case '8':		// <8> 일경우 글자색 Silver
							{
								cColor = RGB(192,192,192);
							}
							break;
							case '9':		// <9> 일경우 글자색 Purple
							{
								cColor = RGB(128,0,128);
							}
							break;
							case 'A':		// <A> 일경우 글자색 Lime
							{
								cColor = RGB(0,255,0);
							}
							break;
							case 'B':		// <B> 일경우 글자색 Blue
							{
								cColor = RGB(0,0,255);
							}
							break;
							case 'C':		// <C> 일경우 글자색 White
							{
								cColor = RGB(255,255,255);
							}
							break;
							case 'D':		// <D> 일경우 글자색 Fuchsia
							{
								cColor = RGB(255,0,255);
							}
							break;
							case 'E':		// <E> 일경우 글자색 Aqua
							{
								cColor = RGB(0,255,255);
							}
							break;
							case 'F':		// <F> 일경우 글자색 Yellow
							{
								cColor = RGB(255,255,0);
							}
							break;
							case 'X':		// <Z> 일경우 글자색 Default
							{
								cColor = RGB(1,1,1);
							}
							break;
						} // endswitch
						szTemp = NULL;
						szTemp = GetValidStr(CmdStr,CmdStr,'/');
						ZeroMemory(CmdParamStr,MAX_BUFFER_SIZE);
						if(szTemp[0]!=NULL)
							strncpy(CmdParamStr,szTemp,strlen(szTemp)-1);
						szTemp = NULL;

					}  // endif
					else
					{
						// 종결 <> 가 있으면 종료
					}  // endifelse

					if(strlen(Fdata)!=0)		// '<' 전에 있는 Text를 일반 Text에 추가
					{	// 일반 Text 추가
						CMText* pxTmpTxt;
						pxTmpTxt = new CMText;
						pxTmpTxt->pText = new CHAR[strlen(Fdata)+1];
						// Text 설정
						pxTmpTxt->bDrawCenter = DrawCenter;
						pxTmpTxt->fColor =	cColor!=RGB(1,1,1) ? cColor : DEFAULT_TXT_COLOR;		// 색은 Tag에 의해 결정해 주어야함.
						pxTmpTxt->nLine = nLineCount;
						pxTmpTxt->nPosx = nPosX;
						strcpy(pxTmpTxt->pText,Fdata);
						m_pxpTxt->AddNode(pxTmpTxt);
						tSize = m_pxGWND->GetStrLength(NULL,NULL,"%s",Fdata);
						nPosX=nPosX+tSize.cx;
						delete[] Fdata;
						Fdata = NULL;
						cColor = RGB(1,1,1);
					} // endif

					if(strlen(CmdParamStr)!=0)//CmdStr)!=0)
					{
						RECT tRect;
						tRect.top =0;
						tRect.left = nPosX;

						tSize = m_pxGWND->GetStrLength(NULL,NULL,"%s",CmdStr);
						tRect.right = tSize.cx;					
						tRect.bottom = tSize.cy;

						// Text 버튼 생성
						m_pxBtn->AddButton(CmdStr,tRect,nLineCount,CmdParamStr,(cColor!=RGB(1,1,1) ? cColor : DEFAULT_BTN_COLOR),DEFAULT_BTN_CLICK_COLOR);

						nPosX=nPosX+tSize.cx;

					}	// Text Button  // endif
					if(Fdata!=NULL)
					{
						delete[] Fdata;
						Fdata = NULL;
					}
				} // end while
				// '>'뒷부분 혹은 '<','>' 없는 기냥 Text일 경우
				if(strlen(szStr)!=0)
				{
					do
					{
						// Line 을 나누고 그 결과를 Text List 에 넣는다.
						CHAR  *szTemp1=NULL,*szTemp2=NULL;
						szTemp1 = new CHAR[MAX_BUFFER_SIZE];
						ZeroMemory(szTemp1,MAX_BUFFER_SIZE);

						// Line Divider
						szTemp2 =  GetValidStr(szStr,szTemp1,LINE_DIVIDER);	
						strcpy(szStr,szTemp2);
						
						// Text 생성
						CMText* pxTmpTxt;
						pxTmpTxt = new CMText;
						pxTmpTxt->pText = new CHAR[strlen(szTemp1)+1];

						// Text 설정
						pxTmpTxt->bDrawCenter = DrawCenter;
						pxTmpTxt->fColor =	cColor!=RGB(1,1,1) ? cColor : DEFAULT_TXT_COLOR;			// 임시 Tag에 의하여 색변환
						pxTmpTxt->nLine = nLineCount;
						pxTmpTxt->nPosx = nPosX;
						strcpy(pxTmpTxt->pText,szTemp1);
						m_pxpTxt->AddNode(pxTmpTxt);

						tSize = m_pxGWND->GetStrLength(NULL,NULL,"%s",szTemp1);
						nPosX=nPosX+tSize.cx;

						delete[] szTemp1;
					}while(strlen(szStr)!=0);
					// Draw Normal Text
				} // endif
			}	// endif
			// Line Feed
			nLineCount ++;
			nPosX = 20;		//시작은 20 pixel 부터
			pxpTxt->MoveNextNode();
		} // end while
		delete[] CmdParamStr;
		delete[] CmdStr;
		delete[] szStr;
		m_bReady = TRUE;
	} // endif
	else
	{
		m_bReady = FALSE;
	} // end ifelse
}


void CNPCTxtAnalysis::AnalySisText(CHAR* Msg)
{
	BOOL	DrawCenter = FALSE;
	CHAR*	szTemp;
	CHAR*	CmdStr;
	CHAR*	CmdParamStr;
	CHAR*	szStr;

	INT	nPosX =0;
	INT nLineCount = 0;
	COLORREF	cColor = 0;

	SIZE	tSize;

	if(m_bInited)
	{

		// Init Buffers
		CmdParamStr	=	new CHAR[MAX_BUFFER_SIZE];
		CmdStr		=	new CHAR[MAX_BUFFER_SIZE];
		szStr		=	new CHAR[MAX_BUFFER_SIZE];


		ZeroMemory(CmdParamStr,MAX_BUFFER_SIZE);
		ZeroMemory(CmdStr,MAX_BUFFER_SIZE);
		ZeroMemory(szStr,MAX_BUFFER_SIZE);


		while(true)
		{
			if(strlen(Msg)==0) break;

			szTemp =  GetValidStr(Msg,szStr,LINE_DIVIDER);				// Line Divider
			strcpy(Msg,szTemp);

			if(strlen(szStr)!=0)
			{

				while((FindChr(szStr,'<')>0)&&(FindChr(szStr,'>')>0) && (strlen(szStr)!=0))
				{
					CHAR*	Fdata;
					Fdata	=	new CHAR[MAX_BUFFER_SIZE];
					ZeroMemory(Fdata,MAX_BUFFER_SIZE);
					// <>Tag 분리 Text Button 생성
					if((szStr[0])!='<')
					{
						szTemp =  GetValidStr(szStr,Fdata,'<');
						strcpy(szStr,"<");
						strcat(szStr,szTemp);				
					}

					// <>안에 있는 내용 CmdStr로
					szTemp = ArrestString(szStr,'<','>',CmdStr);
					strcpy(szStr,szTemp);
					szTemp = NULL;
					
					if(strlen(CmdStr)!=0)
					{
						switch(UpperCase(CmdStr[0]))
						{
							case '0':		// <0> 일경우 글자색 BLACK
							{
								cColor = RGB(0,0,0);
								continue;
							}
							break;
							case '1':		// <1> 일경우 글자색 RED
							{
								cColor = RGB(255,0,0);
								continue;
							}
							break;
							case '2':		// <2> 일경우 글자색 GREEN
							{
								cColor = RGB(0,128,0);
								continue;
							}
							break;
							case '3':		// <3> 일경우 글자색 Olive
							{
								cColor = RGB(128,128,0);
								continue;
							}
							break;
							case '4':		// <4> 일경우 글자색 GRAY
							{
								cColor = RGB(128,128,128);
								continue;
							}
							break;
							case '5':		// <5> 일경우 글자색 Maroon
							{
								cColor = RGB(128,0,0);
								continue;
							}
							break;
							case '6':		// <6> 일경우 글자색 Teal
							{
								cColor = RGB(0,128,128);
								continue;
							}
							break;
							case '7':		// <7> 일경우 글자색 Navy
							{
								cColor = RGB(0,0,128);
								continue;
							}
							break;
							case '8':		// <8> 일경우 글자색 Silver
							{
								cColor = RGB(192,192,192);
								continue;
							}
							break;
							case '9':		// <9> 일경우 글자색 Purple
							{
								cColor = RGB(128,0,128);
								continue;
							}
							break;
							case 'A':		// <A> 일경우 글자색 Lime
							{
								cColor = RGB(0,255,0);
								continue;
							}
							break;
							case 'B':		// <B> 일경우 글자색 Blue
							{
								cColor = RGB(0,0,255);
								continue;
							}
							break;
							case 'C':		// <C> 일경우 글자색 White
							{
								cColor = RGB(255,255,255);
								continue;
							}
							break;
							case 'D':		// <D> 일경우 글자색 Fuchsia
							{
								cColor = RGB(255,0,255);
								continue;
							}
							break;
							case 'E':		// <E> 일경우 글자색 Aqua
							{
								cColor = RGB(0,255,255);
								continue;
							}
							break;
							case 'F':		// <F> 일경우 글자색 Yellow
							{
								cColor = RGB(255,255,0);
								continue;
							}
							break;
							case 'X':		// <Z> 일경우 글자색 Default
							{

								cColor = RGB(1,1,1);
								continue;
							}
							break;
						}

/*						szTemp = new CHAR[2];
						strncpy(szTemp,CmdStr,2);
						szTemp = UpperCase(szTemp);
						if(strncmp(szTemp,"/C",2)==0)		// </C> 일 경우 가운데 찍기 끝
						{
							DrawCenter = FALSE;
							delete[] szTemp;
							szTemp = NULL;
							continue;
						}
						delete[] szTemp;
*/						szTemp = NULL;
						szTemp = GetValidStr(CmdStr,CmdStr,'/');
						strcpy(CmdParamStr,szTemp);
						szTemp = NULL;

					}
					else
					{
						// 종결 <> 가 있으면 종료
					}

					if(strlen(Fdata)!=0)		// '<' 전에 있는 Text를 일반 Text에 추가
					{	// 일반 Text 추가
						CMText* pxTmpTxt;
						pxTmpTxt = new CMText;
						pxTmpTxt->pText = new CHAR[strlen(Fdata)+1];
						// Text 설정
						pxTmpTxt->bDrawCenter = DrawCenter;
						pxTmpTxt->fColor =	cColor!=RGB(1,1,1) ? cColor : DEFAULT_TXT_COLOR;		// 색은 Tag에 의해 결정해 주어야함.
						pxTmpTxt->nLine = nLineCount;
						pxTmpTxt->nPosx = nPosX;
						strcpy(pxTmpTxt->pText,Fdata);

						m_pxpTxt->AddNode(pxTmpTxt);
						tSize = m_pxGWND->GetStrLength(NULL,NULL,"%s",Fdata);
						nPosX=nPosX+tSize.cx;
					}

					if(strlen(CmdStr)!=0)
					{
						RECT tRect;
						tRect.top =0;
						tRect.left = nPosX;

						tSize = m_pxGWND->GetStrLength(NULL,NULL,"%s",CmdStr);
						tRect.right = tSize.cx;					
						tRect.bottom = tSize.cy;

						// Text 버튼 생성
						m_pxBtn->AddButton(CmdStr,tRect,nLineCount,CmdParamStr,(cColor!=RGB(1,1,1) ? cColor : DEFAULT_BTN_COLOR),DEFAULT_BTN_CLICK_COLOR);

						nPosX=nPosX+tSize.cx;

					}	// Text Button
				}
				// '>'뒷부분 혹은 '<','>' 없는 기냥 Text일 경우
				if(strlen(szStr)!=0)
				{
					do
					{
						// Line 을 나누고 그 결과를 Text List 에 넣는다.
						CHAR  *szTemp1=NULL,*szTemp2=NULL;
						szTemp1 = new CHAR[MAX_BUFFER_SIZE];
						ZeroMemory(szTemp1,MAX_BUFFER_SIZE);

						// Line Divider
						szTemp2 =  GetValidStr(szStr,szTemp1,LINE_DIVIDER);	
						strcpy(szStr,szTemp2);
						
						// Text 생성
						CMText* pxTmpTxt;
						pxTmpTxt = new CMText;
						pxTmpTxt->pText = new CHAR[strlen(szTemp1)+1];

						// Text 설정
						pxTmpTxt->bDrawCenter = DrawCenter;
						pxTmpTxt->fColor =	cColor!=RGB(1,1,1) ? cColor : DEFAULT_TXT_COLOR;			// 임시 Tag에 의하여 색변환
						pxTmpTxt->nLine = nLineCount;
						pxTmpTxt->nPosx = nPosX;
						strcpy(pxTmpTxt->pText,szTemp1);
						m_pxpTxt->AddNode(pxTmpTxt);

						tSize = m_pxGWND->GetStrLength(NULL,NULL,"%s",szTemp1);
						nPosX=nPosX+tSize.cx;

						delete[] szTemp1;

						nPosX = 20;		// 시작은 20 pixel 부터
						nLineCount ++;	// Line Feed
					}while(strlen(szStr)!=0);
					// Draw Normal Text
				}
			}
			// Line Feed
			nLineCount ++;
			nPosX = 20;		//시작은 20 pixel 부터
		}
		delete CmdParamStr;
		delete CmdStr;
		delete szStr;
		m_bReady = TRUE;
	}
	else
	{
		m_bReady = FALSE;
	}
}

BOOL CNPCTxtAnalysis::IsReadyToDisplay(VOID)
{
	return m_bReady;
}

BOOL CNPCTxtAnalysis::IsInitialized(VOID)
{
	return m_bInited;
}

// 용기
BOOL CNPCTxtAnalysis::StringDivide(INT nDivideWidth, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr)
{
	CHAR szTmpCheck[MAX_PATH];
	CHAR szResult[MAX_PATH];
	
	nDividedLine = 1;

	INT nStartLen	= 0;
	INT nEndLen		= 0;
	INT nResultLen	= 0;
	INT nWordCheck	= 0;

	if ( szSrc[0] != NULL )
	{
		ZeroMemory(szResult,MAX_PATH);

		for ( INT nCnt = 0; nCnt < (INT)strlen(szSrc); nCnt++)
		{
			ZeroMemory(szTmpCheck, MAX_PATH);
			nEndLen = nCnt+1;

			// 현재부분까지의 문자열을 읽는다.
			memcpy(&szTmpCheck, &szSrc[nStartLen], nEndLen-nStartLen);
			SIZE sizeLen;
			sizeLen = m_pxGWND->GetStrLength(NULL,NULL, szTmpCheck);

			// 현재까지 읽어온 부분이 허용넓이를 넘어섰을때.
			if ( sizeLen.cx > nDivideWidth )
			{
				// 현재 2바이트 문자라면. 
				if ( szSrc[nEndLen-1] < 0 )
				{
					// 현재 앞에문자가 2바이트 문자가 아니라면
					if ( !(nWordCheck%2) )
					{
						nStartLen += strlen(szTmpCheck)-1;
						nCnt--;
						CHAR* pszNewLine;
						pszNewLine = new CHAR[nResultLen+1];
						memcpy(pszNewLine,szResult,nResultLen);
						pszNewLine[nResultLen]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
					}
					else
					{
						nStartLen += strlen(szTmpCheck)-2;
						nCnt -= 2;
						CHAR* pszNewLine;
						pszNewLine = new CHAR[nResultLen];
						memcpy(pszNewLine,szResult,nResultLen-1);
						pszNewLine[nResultLen-1]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
						nWordCheck--;
					}
				}
				// 1바이트 문자. 
				// 현재보다 한바이트앞부분까지만 고려해주면 된다.
				else
				{
					nStartLen += strlen(szTmpCheck)-1;
					nCnt--;
					CHAR* pszNewLine;
					pszNewLine = new CHAR[nResultLen+1];
					memcpy(pszNewLine,szResult,nResultLen);
					pszNewLine[nResultLen]=NULL;
					m_pxpStr->AddNode(pszNewLine);
					nResultLen=0;
					nDividedLine++;
				}
			}
			else if(szSrc[nEndLen-1]=='\n')	// 강제개행     \n으로하면  Error ????
			{
				nStartLen += strlen(szTmpCheck)-1;
				CHAR* pszNewLine;
				pszNewLine = new CHAR[nResultLen+1];
				memcpy(pszNewLine,szResult,nResultLen);
				pszNewLine[nResultLen]=NULL;
				m_pxpStr->AddNode(pszNewLine);
				nResultLen=0;
				nDividedLine++;
			}
			else
			{
				if ( szSrc[nEndLen-1] < 0 )
					nWordCheck++;

				szResult[nResultLen] = szSrc[nEndLen-1];
				nResultLen++;
			}
		}
		if(nResultLen!=0)
		{
			CHAR* pszNewLine;
			pszNewLine = new CHAR[nResultLen+1];
			memcpy(pszNewLine,szResult,nResultLen);
			pszNewLine[nResultLen]=NULL;
			m_pxpStr->AddNode(pszNewLine);
			nDividedLine++;
			nResultLen=0;
		}
		return TRUE;
	}
	return FALSE;
}