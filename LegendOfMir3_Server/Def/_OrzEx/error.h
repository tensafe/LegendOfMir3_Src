

/*
	C++ Exception Processing Class

	Date:
		2001/10/09 (Last Updated: 2002/03/06)
*/
#ifndef __ORZ_ERROR__
#define __ORZ_ERROR__


#define ERROR_MAXBUF	1024


class CError
{
protected:
	char	m_szMsg[ERROR_MAXBUF];

public:
	CError( char *pMsg );
	virtual ~CError();

	const char * GetMsg();
};


#endif