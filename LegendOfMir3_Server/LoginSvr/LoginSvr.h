#pragma once

#define PACKET_KEEPALIVE				"%++$"

#define LOGPARAM_STR			1
#define LOGPARAM_INT			2

class CUserInfo
{
public:
	SOCKET		sock;
	TCHAR		szSockHandle[4];

	TCHAR		szUserID[11];				// User ID
	TCHAR		szAddress[20];				// User's local address 
	
	BYTE		btPayMode;
	
	int			nClientVersion;
	int			nCertification;
	BOOL		fVersionAccept;
	BOOL		fSelServerOk;

	int			nServerID;
};

class CGateInfo
{
public:
	SOCKET					sock;

	CWHList<CUserInfo*>		xUserInfoList;
	
	// For Overlapped I/O
	OVERLAPPED				Overlapped;
	WSABUF					DataBuf;
	CHAR					Buffer[DATA_BUFSIZE];
	int						bufLen;

	CWHQueue				g_SendToGateQ;

public:
	void	SendToGate(SOCKET cSock, char *pszPacket);

	void	ReceiveSendUser(char *pszPacket);
	void	ReceiveCloseUser(char *pszPacket);
	void	ReceiveOpenUser(char *pszPacket);
	void	ReceiveServerMsg(char *pszPacket);
	void	MakeNewUser(char *pszPacket);

	bool	ParseUserEntry( char *buf, _AUSERENTRYINFO *userInfo );

	void	ProcAddUser(SOCKET s, char *pszData);
	void	ProcLogin(SOCKET s, char *pszData);
	void	ProcSelectServer(SOCKET s, WORD wServerIndex);

	void	Close();

	__inline void SendKeepAlivePacket() { send(sock, PACKET_KEEPALIVE, sizeof(PACKET_KEEPALIVE), 0); }

	CGateInfo()
	{
		bufLen	= 0;
	}

	int  Recv()
	{
		DWORD nRecvBytes = 0, nFlags = 0;

		DataBuf.len = DATA_BUFSIZE - bufLen;
		DataBuf.buf = Buffer + bufLen;

		memset( &Overlapped, 0, sizeof( Overlapped ) );

		return WSARecv( sock, &DataBuf, 1, &nRecvBytes, &nFlags, &Overlapped, 0 );
	}

	bool HasCompletionPacket()
	{
		return memchr( Buffer, '$', bufLen ) ? true : false;
	}

	// recv 버퍼에서 완성된 하나의 패킷을 뽑아낸다.
	char * ExtractPacket( char *pPacket )
	{
		int packetLen = (char *) memchr( Buffer, '$', bufLen ) - Buffer + 1;

		memcpy( pPacket, Buffer, packetLen );

		memmove( Buffer, Buffer + packetLen, DATA_BUFSIZE - packetLen );
		bufLen -= packetLen;

		return pPacket + packetLen;
	}
};


struct GAMESERVERINFO
{
	int  index;
	char name[50];
	char ip[50];
	int  connCnt;	// 접속 수 관리
};

// ORZ: 허용 게이트서버 목록 노드 데이터
struct GATESERVERINFO
{
	char ip[50];
};


typedef struct tag_TSENDBUFF
{
	SOCKET			sock;
	char			szData[1024];
}_TSENDBUFF, *_LPTSENDBUFF;

void InsertLogMsg(UINT nID);
void InsertLogMsg(LPTSTR lpszMsg);
void InsertLogMsgParam(UINT nID, void *pParam, BYTE btFlags = LOGPARAM_STR);
