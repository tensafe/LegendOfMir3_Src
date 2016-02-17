
class CPlayerCloneObject : public CPlayerObject
{
public:
	DWORD		m_dwRunTime;
	DWORD		m_dwRunNextTick;

	void	Create(CPlayerObject* pPlayerObject);
	
	virtual void	Operate();
};
