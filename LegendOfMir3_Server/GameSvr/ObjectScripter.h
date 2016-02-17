
class CScripterObject : public CPlayerObject
{
protected:
	FILE*	m_fScriptFile;

public:
	DWORD		m_dwRunTime;
	DWORD		m_dwRunNextTick;

	void	Create(int nX, int nY, CMirMap* pMap);
	
	virtual void	Operate();
};
