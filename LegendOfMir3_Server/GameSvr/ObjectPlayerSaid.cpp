#include "stdafx.h"

#define _MAX_USERCOMMAND_LIST	(IDS_USERCMD_SABUKDOOR - IDS_USERCMD_NOWHISPER1 + 1)

void CryCry(UINT wMsgType, CMirMap* pMap, int nX, int nY, int nWide, char *pszSaying);

void InitUserCommandList()
{
	TCHAR	szCommand[64];
	int		nLen;

	for (int i = 0; i < _MAX_USERCOMMAND_LIST; i++)
	{
		LoadString(g_hInst, IDS_USERCMD_NOWHISPER1 + i, szCommand, sizeof(szCommand)/sizeof(TCHAR));

		nLen = lstrlen(szCommand) * sizeof(TCHAR) + 1;

		char *pszUserCommand = new char[nLen];

		if (pszUserCommand)
		{
			WideCharToMultiByte(CP_ACP, 0, szCommand, -1, pszUserCommand, nLen, NULL, NULL);
			g_xUserCommandList.AddNewNode(pszUserCommand);
		}
	}

	InsertLogMsgParam(IDS_LOAD_USERCOMMAND, &i, LOGPARAM_INT);
}

void UnInitUserCommandList()
{
	PLISTNODE pListNode = g_xUserCommandList.GetHead();

	while (pListNode)
	{
		delete [] g_xUserCommandList.GetData(pListNode);

		pListNode = g_xUserCommandList.RemoveNode(pListNode);
	}
}

BOOL CPlayerObject::ProcessForUserSaid(char *pszData)
{
	char	szDecodeMsg[256];
	char	szChatMsg[512];

	int nPos = fnDecode6BitBufA(pszData, szDecodeMsg, sizeof(szDecodeMsg));
	
	szDecodeMsg[nPos] = '\0';

	if (szDecodeMsg[0] == '@')
	{
		ProcessForAdminSaid(szDecodeMsg);

		return FALSE;
	}
	else
	{
		if (szDecodeMsg[0] == '!')
		{
			if (GetTickCount() - m_dwLastCryTime > 10 * 1000)
			{
				if (m_Ability.Level <= 7)
				{
					TCHAR	wszMsg[128];
					char	szMsg[128];

					LoadString(g_hInst, IDS_NOTENOUGHLEVEL_CRY, wszMsg, sizeof(wszMsg)/sizeof(TCHAR));
					WideCharToMultiByte(CP_ACP, 0, wszMsg, -1, szMsg, sizeof(szMsg), NULL, NULL);

					SysMsg(szMsg, 1);

					return TRUE;
				}
				else
				{
					m_dwLastCryTime = GetTickCount();

					int nLen = memlen(m_pUserInfo->m_szCharName);
				
					memmove(szChatMsg, "(!)", 3);
					memmove(&szChatMsg[3], m_pUserInfo->m_szCharName, (nLen - 1));
					memmove(&szChatMsg[nLen - 1 + 3], " : ", 3);
					memmove(&szChatMsg[nLen + 2 + 3], &szDecodeMsg[1], memlen(&szDecodeMsg[1]));

					CryCry(RM_CRY, m_pMap, m_nCurrX, m_nCurrY, 50, szChatMsg);

					return TRUE;
				}
			}
		}
		else if (szDecodeMsg[0] == '/')
		{
			char *pszCharName = &szDecodeMsg[1];

			char *pszMsg = (char *)memchr(pszCharName, 0x20, memlen(pszCharName) - 1);
			
			if (pszMsg)
			{
				*pszMsg++ = '\0'; 
				Whisper(pszCharName, pszMsg);

				return TRUE;
			}
		}
		else
		{
			int nLen = memlen(m_szName);
		
			memmove(szChatMsg, m_szName, (nLen - 1));
			memmove(&szChatMsg[nLen - 1], " : ", 3);
			memmove(&szChatMsg[nLen + 2], szDecodeMsg, memlen(szDecodeMsg));

			AddRefMsg(RM_HEAR, 0, 0x0000, 0xFFFF, 0, szChatMsg);

			return TRUE;
		}
	}

	return FALSE;
}
