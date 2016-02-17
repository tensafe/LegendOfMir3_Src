#ifndef _CCLIENTMSG_H
#define _CCLIENTMSG_H


#define _MAX_SYSMSG 10
typedef struct tagCONSTSTRING
{
	CHAR szMsg[MAX_PATH];
}CONSTSTRING, *LPCONSTSTRING;

class ClientSysMsg
{ 
private:
public:
	WORD					m_wCurrDelay;
	CPDLList<CONSTSTRING>	m_stSysMsg;

	ClientSysMsg();
	~ClientSysMsg();
	VOID		Init();
	VOID		Destroy();
	VOID		AddSysMsg(CHAR* szStr);
	VOID		ShowSysMsg(INT nLoopTime, INT nX, INT nY);
};



#endif _CCLIENTMSG_H_