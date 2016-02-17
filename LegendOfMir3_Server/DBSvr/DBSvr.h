
#pragma once

#define PACKET_KEEPALIVE		"%++$"

#define LOGPARAM_STR			1
#define LOGPARAM_INT			2

class CServerInfo
{
public:
	SOCKET					m_sock;

	// For Overlapped I/O
	OVERLAPPED				Overlapped;
	WSABUF					DataBuf;
	INT						nOvFlag;
	CHAR					Buffer[DATA_BUFSIZE];
	CHAR					RemainBuff[DATA_BUFSIZE];
	INT						nRemainBuffLen;
};

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
};

class CGateInfo
{
public:
	SOCKET					sock;

	CWHList<CUserInfo*>		xUserInfoList;
													  
	WSABUF					DataBuf;
	CHAR					Buffer[DATA_BUFSIZE];
	int						bufLen;

	CWHQueue				m_GateQ;

public:
	void	ReceiveOpenUser(char *pszPacket);
	void	ReceiveCloseUser(char *pszPacket);
	void	ReceiveSendUser(char *pszPacket);

	void	QueryCharacter(SOCKET s, char *pszPacket);
	void	DeleteExistCharacter(SOCKET s, _LPTCREATECHR lpTCreateChr);
	void	MakeNewCharacter(SOCKET s, _LPTCREATECHR lpTCreateChr);
	void	GetSelectCharacter(SOCKET s, char *pszPacket);

	void	SendToGate(SOCKET cSock, char *pszPacket);

	__inline void SendKeepAlivePacket() { send(sock, PACKET_KEEPALIVE, sizeof(PACKET_KEEPALIVE), 0); }

	// ORZ:
	CGateInfo()
	{
		bufLen	= 0;
	}

	int  Recv()
	{
		return recv( sock, Buffer + bufLen, DATA_BUFSIZE - bufLen, 0 );
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


// ORZ:
struct GAMESERVERINFO
{
	int  index;
	char name[50];
	char ip[50];
	int  connCnt;	// 접속 수 관리

	CServerInfo*	pServerInfo;
};


typedef struct tag_TSENDBUFF
{
	CServerInfo*		pServerInfo;

	int					nCertification;
	_TDEFAULTMESSAGE	DefMsg;
	_TLOADHUMAN			HumanLoad;
	BYTE				*lpbtAddData;
	BYTE				*lpbtAddData2;
} _TSENDBUFF, *_LPTSENDBUFF;

typedef struct tag_TGATESENDBUFF
{
	SOCKET				sock;
	char				szData[1024];
}_TGATESENDBUFF, *_LPTGATESENDBUFF;

void InsertLogMsg(UINT nID);
void InsertLogMsg(LPTSTR lpszMsg);
void InsertLogMsgParam(UINT nID, void *pParam, BYTE btFlags = LOGPARAM_STR);
