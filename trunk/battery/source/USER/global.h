#ifdef GLOBAL
	#define EXT_GLOBAL 
#else
	#define EXT_GLOBAL extern
#endif

EXT_GLOBAL char mystr[72];
EXT_GLOBAL uint16_t Voltage[100];
EXT_GLOBAL uint16_t ADBuf[100][7];
EXT_GLOBAL uint8_t 	EditBuf[200];
EXT_GLOBAL uint8_t 	EnableCh[100];
EXT_GLOBAL uint8_t 	StatusCh[100];
EXT_GLOBAL uint8_t	scanCh,ADBufIndex;
EXT_GLOBAL uint8_t	OutputBuf[13];	//13*8=104¸ö¼ÌµçÆ÷Êä³ö
EXT_GLOBAL uint8_t	tmpSet[100];
EXT_GLOBAL uint16_t testCount;
//EXT_GLOBAL float Temperature;

//EXT_GLOBAL uint16_t t1count,t3count,t4count,rtcount;

EXT_GLOBAL uint8_t 	CD4094_Buf,led_Buf,curPages,curline_x,Sec3Count,saveCurveIndex;
EXT_GLOBAL uint16_t saveCount;	//<600
EXT_GLOBAL uint32_t	curKey,preKey,inputDelay,cal32;
EXT_GLOBAL uint8_t	keyDelay,curIndex;
EXT_GLOBAL volatile struct {
	unsigned int vv0				:1;
	unsigned int vv1				:1;
	unsigned int vv2				:1;
	unsigned int vv3				:1;
	unsigned int vv4				:1;
	unsigned int vv5				:1;
	unsigned int vv6				:1;
	unsigned int vv7				:1;	
	unsigned int vv8				:1;	
}mybitValable;

#define	Flag_keyPressed		mybitValable.vv0
#define	Flag_exInput			mybitValable.vv1
#define	Flag_AD_done			mybitValable.vv2
#define	Flag_1_second			mybitValable.vv3
#define	Flag_10ms_second	mybitValable.vv4
#define	Flag_OutputChange	mybitValable.vv5
#define	Flag_IsRunning		mybitValable.vv6
#define	Flag_LEDChange		mybitValable.vv7
#define	Flag_CheckDealy		mybitValable.vv8

EXT_GLOBAL uint8_t curFace,curIndex;
EXT_GLOBAL uint8_t workMode,editIndex;
EXT_GLOBAL uint16_t setingTime,setingVal,runingTime,runingVal;

#define	MAIN_FACE					0x01
#define	FACE_SETMENU			0x02
#define	FACE_VERSION			0x03
#define	FACE_CURVEQUERY		0x04
#define	FACE_SETSTATUS		0x05
#define	FACE_MODESET			0x06
#define	FACE_TIMESET			0x07
#define	FACE_IOTEST				0x08
#define	FACE_PICTURE			0x09
#define	FACE_SELECTCURVE	0x0A


