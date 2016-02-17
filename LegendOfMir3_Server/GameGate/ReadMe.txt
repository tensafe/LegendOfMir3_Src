========================================================================
       WIN32 APPLICATION : GameGate
========================================================================

Following functions was defined in ClientSockMsg.cpp


OnClientSockMsg() --FD_READ--> ProcReceiveBuffer() --GM_DATA-->	ProcMakeSocketStr()


LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam)

void ProcReceiveBuffer(char *pszPacket, int nRecv)

void ProcMakeSocketStr(_LPTMSGHEADER lpMsgHeader)
	This function called by ProcReceiveBuffer() when Received GM_DATA pakcet from Game Server.
	ProcMakeSocketStr are will make encode packet for send to client.



Following functions was defined in ServerSockMsg.cpp

Threads - Accept Thread(1)
          IOCP Worker Thread(2 * Number of Processor)
