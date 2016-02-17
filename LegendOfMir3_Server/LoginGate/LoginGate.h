
#ifndef _LOGINGATE_DEFINE
#define _LOGINGATE_DEFINE

class CSessionInfo
{
public:
	SOCKET						sock;

	OVERLAPPED					Overlapped;
	WSABUF						DataBuf;
	CHAR						Buffer[DATA_BUFSIZE];
	int							bufLen;

public:
	// ORZ:
	CSessionInfo()
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


void InsertLogMsg(UINT nID);
void InsertLogMsg(LPTSTR lpszMsg);
void InsertLogPacket(char *pszPacket);

#endif //_LOGINGATE_DEFINE
