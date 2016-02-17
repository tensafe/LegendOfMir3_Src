class CWHAbusive
{
protected:
	char	m_szAbusiveList[100][10];
	int		m_nCount;

public:
	CWHAbusive();
	~CWHAbusive();

	BOOL LoadAbusiveList();
	void ChangeAbusiveText(char *pszText);

};
