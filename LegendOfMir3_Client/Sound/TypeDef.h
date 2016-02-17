#if !defined _TYPEDEF_H
#define _TYPEDEF_H


#define MAX_DUPLE_COUNT		5
#define ID_TIMER_1			101
#define ID_AMBIANCE_TIMER	102

#define WM_USER_ENDTIMER	WM_USER+100
#define WM_DSHOW_NOTIFY		WM_APP + 500

#define DELETEARRAY(x) if(x != NULL) { delete [] x; x = NULL; }

typedef struct
{
	int x;
	int y;
} Position;


#pragma pack(1)

typedef struct
{
	signed short int ID;			//	2 bytes
	char  Des[14];				// 14 bytes
} WAVELIST;						// 16 bytes


typedef struct
{
	char Title[40];				//	40 bytes
	char TargetDir[10];			//	10 bytes
	unsigned int FieldCount;	//	 4 bytes
	unsigned int ListCount;		//	 4 bytes  
}	WAVELISTHEADER;				//	58 bytes

#pragma pack(8)




#endif //_TYPEDEF_H
