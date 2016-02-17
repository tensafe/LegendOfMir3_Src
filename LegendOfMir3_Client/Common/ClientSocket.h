#ifndef _CLIENTSOCKET_H
#define _CLIENTSOCKET_H

#define ID_SOCKCLIENT_EVENT_MSG		WM_USER + 1000

#include <assert.h>

class CClientSocket
{
protected:
	int					m_nCurRecv;
	CHAR				m_szBuff[8192 * 4];				// For Receive

	char				m_szEncodeDefMsg[32];			// For Send
	char				m_szEncodeBody[8192];			
	char				m_szPacket[8192 * 2];

	int					m_nSync;

	_TDEFAULTMESSAGE	DefMsg;

public:
	CClientSocket();
	virtual ~CClientSocket();

	SOCKET			m_sockClient;
	CWHDefProcess*	m_pxDefProc;

public:
	unsigned long GetINetAddr(CHAR* pszDotAddress);

	BOOL		InitSocket(HWND hWnd);	
	BOOL		ConnectToServer(HWND hWnd, CHAR* szID, INT nPort);
	BOOL		CheckSocketError(LPARAM lParam);
	BOOL		DisconnectToServer();
//	VOID		SendToServer(CHAR *pszPacket);
	VOID		SendPacket(_TDEFAULTMESSAGE* lpDefMsg, char *pszData);

	// For Login
	VOID		OnLogin(char *pszID, char *pszPassword);
	VOID		OnSelectServer(char *pszServerName);
	VOID		OnChangePassword(char *pszID, char *pszOldPassword, char *pszNewPassword);
	VOID		OnNewAccount(char *pszNewAccount);

	// For Character Select
	VOID		OnQueryChar();
	VOID		OnNewChar( char *pszId, int nHair, int nJob, int nSex );
	VOID		OnDelChar( char *pszCharName );
	VOID		OnSelChar( char *pszCharName );

	// For Game playing
	void		SendNoticeOK();

	void		SendNPCMessage(WORD wIdent,DWORD dwNpcId,INT reg,CHAR* szTxt);			// MOON
	void		SendGetDetailItem(DWORD dwNpcId,INT nIndex,CHAR* szItemName);			// MOON
	void		SendBuyItem(DWORD dwNpcId,CHAR* szItemName,CHAR* szItemIndex);			// MOON

	VOID		SendOpenDoor(INT nX, INT nY, INT nIdx);
	VOID		SendQueryMsg(WORD wIdent);
	VOID		SendActMsg(WORD wIdent, INT nX, INT nY, INT nDir);	
	VOID		SendHitMsg(WORD wIdent, INT nX, INT nY, INT nDir, WORD wHitStyle);	
	VOID		SendRideMsg(WORD wIdent, INT nDir, BOOL bRide);
	VOID		SendSpellMsg(INT nMagicID, INT nTargetX, INT nTargetY, INT nTargetID);
	VOID		SendSay(CHAR *pszSay);
	VOID		SendRunLogin(CHAR* szLoginID, CHAR* szCharName, INT nCertification, INT VersionNum);
	VOID		SendTakeOnOffItem(WORD wIdent, BYTE bEquipNum, CHAR* szItemName, INT nItemIdx);
	VOID		SendQueryName(INT nRecog, INT nX, INT nY);
	VOID		SendMagicKeyChange(INT nKeyNum, INT nMagicID);
	VOID		SendItemIndex(WORD wIdent, INT nItemIndex, CHAR* pszItemName);
	VOID		SendPickUp(INT nX, INT nY);
	VOID		SendButchAnimal(INT nX, INT nY, INT nDir, INT nActorID);
	VOID		SendNPCClick(INT nRecog);

	void		SendCreateGroup(char *pszWho);
	void		SendGroupMode(BOOL fFlag);
	void		SendAddGroupMember(char *pszWho);
	void		SendDelGroupMember(char *pszWho);

	void		SendGuildHome();

	LRESULT		OnSocketMessage(WPARAM wParam, LPARAM lParam);

protected:
};

#endif // _CLIENTSOCKET_H