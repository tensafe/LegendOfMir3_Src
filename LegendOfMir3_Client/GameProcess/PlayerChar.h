/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/


#ifndef _CPLAYERCHAR_H
#define _CPLAYERCHAR_H


#include <ddraw.h>
#include <mmsystem.h>


class CPlayerChar : public CCharacter
{
private:	
public:
	CDLList<CItem>		m_xBeltItemList;

protected:
public:
	CPlayerChar();
	~CPlayerChar();
};

#endif // _CPLAYERCHAR_H