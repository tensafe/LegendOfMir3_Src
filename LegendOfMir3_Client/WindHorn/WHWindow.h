#ifndef _WINDHORN_WINDOW
#define	_WINDHORN_WINDOW



/******************************************************************************************************************

	CWHApp Class Declaration

*******************************************************************************************************************/

class CWHApp
{
public:
//1: Constuctor/Destructor
	CWHApp();
	~CWHApp();

//2: Variables
protected:
	HINSTANCE			m_hInstance;
	__inline VOID		SethInstance(HINSTANCE hInst)	{ m_hInstance = hInst; }

//3: Methods
public:
	__inline HINSTANCE	GetSafehInstance()				{ return  m_hInstance; }

//4: Message Map
	virtual LRESULT	MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};



/******************************************************************************************************************

	CWHWindow Class Declaration

*******************************************************************************************************************/
class CWHWindow : public CWHApp
{
//1: Constuctor/Destructor
public:
	CWHWindow();
	~CWHWindow();

//2: Variables
protected:
	HWND	m_hWnd;

//3: Methods
	BOOL	RegisterClass(CHAR* pszMenuName, CHAR* pszIconName);
public:
	BOOL	Create(HINSTANCE hInst, LPTSTR lpCaption = NULL, CHAR* pszMenuName = NULL, CHAR* pszIconName = NULL);
	__inline	HWND GetSafehWnd()					{ return m_hWnd; }

//4: Message Map
	virtual LRESULT MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDestroy();
};

#endif //_WINDHORN_WINDOW
