
#ifndef _GAMEGATE_DEFINE
#define _GAMEGATE_DEFINE

class CSessionInfo
{
public:
	SOCKET			sock;
//	SOCKADDR_IN		addr;

	int				nServerUserIndex;
	int				nGateIndex;
	
	BOOL			fLoginCode;

	int				nPreviousCheckCode;
	int				nCrackWarningLevel;

	CHAR			SendBuffer[DATA_BUFSIZE];
	int				nSendBufferLen;
    //SocData: string;   //받은 버퍼
    //SendData: string;  //보낼것이 남음

//    SendLock: Boolean;
//    SendLatestTime: longword;
//    CheckSendLength: integer;
//    SendAvailable: Boolean;
//    SendCheck: Boolean;
//    TimeOutTime: longword;
//    ReceiveLength: integer;
//    ReceiveTime: longword;
//    shandle:  integer;
//    RemoteAddr: string;

	// For Overlapped I/O
	OVERLAPPED		Overlapped;
	WSABUF			DataBuf;
	CHAR			Buffer[DATA_BUFSIZE];
//	CHAR			RemainBuff[DATA_BUFSIZE];
	INT				bufLen;
	INT				nOvFlag;

	CSessionInfo()
	{
		bufLen	= 0;
	}

	int  Recv()
	{
		nOvFlag = OVERLAPPED_RECV;
		DWORD nRecvBytes = 0, nFlags = 0;

		DataBuf.len = DATA_BUFSIZE - bufLen;
		DataBuf.buf = Buffer + bufLen;

		memset( &Overlapped, 0, sizeof( Overlapped ) );

		return WSARecv( sock, &DataBuf, 1, &nRecvBytes, &nFlags, &Overlapped, 0 );
	}

	bool HasCompletionPacket()
	{
		return memchr( Buffer, '!', bufLen ) ? true : false;
	}

	// recv 버퍼에서 완성된 하나의 패킷을 뽑아낸다.
	char * ExtractPacket( char *pPacket )
	{
		int packetLen = (char *) memchr( Buffer, '!', bufLen ) - Buffer + 1;

		memcpy( pPacket, Buffer, packetLen );

		memmove( Buffer, Buffer + packetLen, DATA_BUFSIZE - packetLen );
		bufLen -= packetLen;

		return pPacket + packetLen;
	}
};

typedef struct tag_TSENDBUFF
{
	SOCKET			sock;
	int				nGateIndex;
	char			szData[1024];
}_TSENDBUFF, *_LPTSENDBUFF;

#define LOGPARAM_STR						1
#define LOGPARAM_INT						2

void InsertLogMsg(UINT nID);
void InsertLogMsg(LPTSTR lpszMsg);
void InsertLogPacket(char *pszPacket);
void InsertLogMsgParam(UINT nID, void *pParam, BYTE btFlags);

void SendSocketMsgS (_LPTMSGHEADER lpMsg, int nLen1, char *pszData1, int nLen2, char *pszData2);
void SendSocketMsgS (int nIdent, WORD wIndex, int nSocket, WORD wSrvIndex, int nLen, char *pszData);

extern BOOL	g_fTerminated;

#endif //_GAMEGATE_DEFINE