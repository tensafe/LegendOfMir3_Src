#ifndef _CITEM_H
#define	_CITEM_H


#pragma pack(1)
typedef struct tagSTANDARDITEM
{
	CHAR			szName[15];

	BYTE			bStdMode;          
	BYTE			bShape;            
	BYTE			bWeight;           
	BYTE			bAniCount;
	
	BYTE			sSpecialPower;           

	BYTE			bNeedIdentify;

	WORD			wLooks;            
	WORD			wDuraMax;

	WORD			wAC;			
	WORD			wMAC;			

	WORD			wDC;			
	WORD			wMC;			

	WORD			wSC;			
	BYTE			bNeed;             
	BYTE			bNeedLevel;        

	int				nPrice;
}STANDARDITEM, *LPSTANDARDITEM;

typedef struct tagCLIENTITEMRCD
{
	STANDARDITEM	stStdItem;
	int				nMakeIndex;
	WORD			nDura;
	WORD			nDuraMax;
}CLIENTITEMRCD, *LPCLIENTITEMRCD;

typedef struct tagGROUNDITEM
{
	INT				nRecog;
	SHORT			shTileX;
	SHORT			shTileY;
	WORD			wLooks;
	CHAR			szItemName[40];
}GROUNDITEM, *LPGROUNDITEM;
#pragma pack(8)


class CItem
{ 
public:
	CItem();
	~CItem();

private:
public:
	BYTE			m_bAttr;
	SHORT			m_shCellNum;
	CLIENTITEMRCD	m_stItemInfo;

protected:
public:
	BOOL		SetItem(CLIENTITEMRCD stItemInfo, SHORT shInventoryNum);
	BOOL		DrawItem(INT nX, INT nY, BYTE bItemType = _ITEM_TYPE_INVENTORY);
	VOID		ShowItemStatus(INT nX, INT nY);
public:
};



#endif //_CITEM_H