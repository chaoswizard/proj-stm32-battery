#include "stm32f10x.h"
#include "lcd12864.h"
#include "input_output.h"
#include "at45db041.h"
#include "keyboard.h"
#include "global.h"
#include <string.h>
#include <stdio.h>

typedef	void(*ACTION)(void);
typedef	struct _menu	_MENU;
struct _menu{
	ACTION	action;
	const unsigned char name[13];
	unsigned char myID;
};

unsigned char curMenuIndex,curMenuRow;
unsigned char curCurveIndex,curSetIndex,subMenuIndex;


void selectCurve(unsigned char clearing);
void StatusSeting(unsigned char clearing);
void ModeSeting(unsigned char clearing);
void CurveQuery(unsigned char clearing);
void DrawIOTest(unsigned char clearing);
void Version(void);
void drawTimeValtage(unsigned char clearing);
void inpupNum(unsigned char iNum);


void KeyServer(uint32_t qkey)
{
	int i;
	switch(qkey)
	{
	case KEY_INC:	//设置
		if(curFace==MAIN_FACE)	{	curIndex=0;	curMenuRow=0;	drawSetMenu(1);	}
		else if(curFace==FACE_MODESET)
		{
				editIndex=0;	workMode=subMenuIndex;	
				if(workMode==1)		editIndex=4;
				drawTimeValtage(1);
		}
		if(curFace==FACE_PICTURE)
		{
			subMenuIndex=0;	curSetIndex=0;
			selectCurve(1);
		}		
		break;
	case KEY_DEC:	//+/-
		if(curFace==FACE_SETSTATUS)
		{
			if(EnableCh[curSetIndex]==1) EnableCh[curSetIndex]=0;
			else EnableCh[curSetIndex]=1;
			StatusSeting(0); 
		}else if(curFace==FACE_IOTEST)
		{
			if(tmpSet[curSetIndex]==1) tmpSet[curSetIndex]=0;
			else tmpSet[curSetIndex]=1;
			testIOTran();	Flag_OutputChange=1;
			DrawIOTest(0); 
		}else if(curFace==FACE_SELECTCURVE)
		{
			if(tmpSet[curSetIndex]==1) tmpSet[curSetIndex]=0;
			else tmpSet[curSetIndex]=1;
			selectCurve(0);
		}
		break;
	case KEY_UP:
		if(curFace==MAIN_FACE)
		{
			if(curIndex>0) {	curIndex--;	drawMainFace(0);	}
		}else if(curFace==FACE_SETMENU)
		{			
			if(curMenuIndex>1) 	{	curMenuIndex--;	drawSetMenu(0);		}
			else{ if(curMenuIndex==1) { curMenuIndex--; if(curMenuRow>0) curMenuRow--;	} drawSetMenu(0); }
		}else if(curFace==FACE_CURVEQUERY)
		{
			if(curCurveIndex>0)	{	curCurveIndex--; CurveQuery(0);	}
		}else if(curFace==FACE_SETSTATUS)
		{
			if(curSetIndex>subMenuIndex) {	curSetIndex--;	StatusSeting(0);	}
			else	{
				if(subMenuIndex>0)	{	subMenuIndex--; curSetIndex--;	StatusSeting(0);	}
			}
		}else if(curFace==FACE_SELECTCURVE)
		{
			if(curSetIndex>subMenuIndex) {	curSetIndex--;	selectCurve(0);	}
			else	{
				if(subMenuIndex>0)	{	subMenuIndex--; curSetIndex--;	selectCurve(0);	}
			}
		}else if(curFace==FACE_IOTEST)
		{
			if(curSetIndex>subMenuIndex) {	curSetIndex--;	DrawIOTest(0);	}
			else	{
				if(subMenuIndex>0)	{	subMenuIndex--; curSetIndex--;	DrawIOTest(0);	}
			}
		}else if(curFace==FACE_MODESET)
		{
			if(subMenuIndex>0)	{	subMenuIndex--; ModeSeting(0);	}
		}else if(curFace==FACE_TIMESET)
		{
			if(workMode==2)
			{		if(editIndex>=4)	{	editIndex-=4;	drawTimeValtage(0);	}		}
		}else if(curFace==FACE_PICTURE)
		{
			if(curPages>0)	
			{
				curPages--;	drawPicture();
				Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
			}
		}	
		break;
	case KEY_DOWN:
		if(curFace==MAIN_FACE)
		{
			if(curIndex<97) {	curIndex++;	drawMainFace(0);	}
		}else if(curFace==FACE_SETMENU)
		{			
			if(curMenuIndex<3) {	curMenuIndex++;	drawSetMenu(0);	}
			else {	if(curMenuRow==0) {	curMenuRow++; curMenuIndex++;	drawSetMenu(0);	}	}
		}else if(curFace==FACE_CURVEQUERY)
		{
			if(curCurveIndex<2)	{	curCurveIndex++; CurveQuery(0);	}
		}else if(curFace==FACE_SETSTATUS)
		{
			if(curSetIndex<subMenuIndex+2) {	curSetIndex++;	StatusSeting(0);	}
			else	{
				if(subMenuIndex<97)	{	subMenuIndex++; curSetIndex++;	StatusSeting(0);	}
			}
		}else if(curFace==FACE_SELECTCURVE)
		{
			if(curSetIndex<subMenuIndex+2) {	curSetIndex++;	selectCurve(0);	}
			else	{
				if(subMenuIndex<97)	{	subMenuIndex++; curSetIndex++;	selectCurve(0);	}
			}
		}else if(curFace==FACE_IOTEST)
		{
			if(curSetIndex<subMenuIndex+2) {	curSetIndex++;	DrawIOTest(0);	}
			else	{
				if(subMenuIndex<97)	{	subMenuIndex++; curSetIndex++;		DrawIOTest(0);	}
			}
		}else if(curFace==FACE_MODESET)
		{
			if(subMenuIndex<2)	{	subMenuIndex++; ModeSeting(0);	}
		}else if(curFace==FACE_TIMESET)
		{
			if(workMode==2)
			{		if(editIndex<4)	{	editIndex+=4;	drawTimeValtage(0);	}	}
		}else if(curFace==FACE_PICTURE)
		{
			if(curPages<4)	
			{
				curPages++;	drawPicture();	
				Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
			}
		}		
		break;
	case KEY_LEFT:
		if(curFace==FACE_TIMESET)
		{
			if(workMode!=1)	{	if(editIndex>0)	{	editIndex--;	drawTimeValtage(0);	}	}
			else {	if(editIndex>4)	{	editIndex--;	drawTimeValtage(0);	}	}
		}else if(curFace==FACE_PICTURE)
		{
			if(curline_x>0)
			{
				Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
				curline_x--;
				if(curline_x>0xF0)curline_x=0;
				Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
			}
		}	
		break;
	case KEY_RIGHT:
		if(curFace==FACE_TIMESET)
		{
			if(workMode!=0)	{	if(editIndex<7)	{	editIndex++;	drawTimeValtage(0);	}	}
			else {	if(editIndex<3)	{	editIndex++;	drawTimeValtage(0);	}	}
		}else if(curFace==FACE_PICTURE)
		{
			if(curline_x<110)
			{
				Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
				curline_x++;
				Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
			}
		}	
		break;
	case KEY_OK:
		if(curFace==FACE_SETMENU)
		{
			if(curMenuIndex==0) 
			{				
				ReadMemory(EnableCh,100,2047,0);
				for(i=0;i<100;i++)
				{	
					if(EnableCh[i]!=1) EnableCh[i]=0;
				}				
				subMenuIndex=0;	curSetIndex=0;
				StatusSeting(1);
			}
			else if(curMenuIndex==1) 
			{
				ReadMemory(tmpSet,5,2047,100);
				workMode=tmpSet[0];	if(workMode>2) workMode=0;
				setingTime=tmpSet[1]*256+tmpSet[2];
				setingVal=tmpSet[3]*256+tmpSet[4];
				subMenuIndex=workMode;	ModeSeting(1);	
			}
			else if(curMenuIndex==2) {	curCurveIndex=0;	CurveQuery(1);	}
			else if(curMenuIndex==3) 
			{
				for(i=0;i<100;i++) tmpSet[i]=0;
				for(i=0;i<13;i++) OutputBuf[i]=0;
				Flag_OutputChange=1;
				subMenuIndex=0;	curSetIndex=0;
				DrawIOTest(1);				
			}
			else if(curMenuIndex==4) Version();			
			return;
		}else if(curFace==FACE_SETSTATUS)
		{
			write_Page(EnableCh,100,2047,0);
			saveSucceed();	StatusSeting(1);
		}else if(curFace==FACE_MODESET)
		{
			tmpSet[0]=subMenuIndex; workMode=subMenuIndex;
			write_Page(tmpSet,1,2047,100);
			saveSucceed(); ModeSeting(1);	
			return;
		}else if(curFace==FACE_TIMESET)
		{
			if(workMode==0)
			{
				tmpSet[0]=(uint8_t)(setingTime/256);	tmpSet[1]=(uint8_t)(setingTime);
				write_Page(tmpSet,2,2047,101);
			}else if(workMode==1)
			{
				tmpSet[0]=(uint8_t)(setingVal/256);	tmpSet[1]=(uint8_t)(setingVal);
				write_Page(tmpSet,2,2047,103);
			}else
			{
				tmpSet[0]=(uint8_t)(setingTime/256);	tmpSet[1]=(uint8_t)(setingTime);
				tmpSet[2]=(uint8_t)(setingVal/256);		tmpSet[3]=(uint8_t)(setingVal);
				write_Page(tmpSet,4,2047,101);
			}	
			saveSucceed();	drawTimeValtage(1);			
		}else if(curFace==FACE_CURVEQUERY)
		{
			for(i=0;i<100;i++) tmpSet[i]=EnableCh[i];			
			curPages=0;	curFace=FACE_PICTURE;	drawPicture();
			Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);
		}
		break;
	case KEY_ESC:
		if(curFace==FACE_SETMENU)	{	drawMainFace(1);	}
		else if((curFace==FACE_VERSION)||(curFace==FACE_CURVEQUERY)||(curFace==FACE_SETSTATUS)||(curFace==FACE_MODESET)||(curFace==FACE_IOTEST))	
		{
			if(curFace==FACE_SETSTATUS)
			{
				for(i=0;i<100;i++)
				{	
					if(EnableCh[i]!=1) Voltage[i]=0;
				}
			}else if(curFace==FACE_IOTEST)
			{
				for(i=0;i<13;i++) OutputBuf[i]=0;
				Flag_OutputChange=1;
			}
			drawSetMenu(1);
		}
		else if(curFace==FACE_TIMESET) {	ModeSeting(1);	}
		else if(curFace==FACE_PICTURE)
		{
				CurveQuery(1);
		}else if(curFace==FACE_SELECTCURVE)
		{
			curPages=0;	curFace=FACE_PICTURE;	drawPicture();
			Screen_PrintLine(curline_x+10,3,curline_x+10,53,2);			
		}
		break;
	case KEY_RUN:
		break;
	case KEY_STOP:
		break;
	case KEY_0:
		if(curFace==FACE_TIMESET) inpupNum(0);
		break;
	case KEY_1:
		if(curFace==FACE_TIMESET) inpupNum(1);
		break;
	case KEY_2:
		if(curFace==FACE_TIMESET) inpupNum(2);
		break;
	case KEY_3:
		if(curFace==FACE_TIMESET) inpupNum(3);
		break;
	case KEY_4:
		if(curFace==FACE_TIMESET) inpupNum(4);
		break;
	case KEY_5:
		if(curFace==FACE_TIMESET) inpupNum(5);
		break;
	case KEY_6:
		if(curFace==FACE_TIMESET) inpupNum(6);
		break;
	case KEY_7:
		if(curFace==FACE_TIMESET) inpupNum(7);
		break;
	case KEY_8:
		if(curFace==FACE_TIMESET) inpupNum(8);
		break;
	case KEY_9:
		if(curFace==FACE_TIMESET) inpupNum(9);
		break;	
	}
}

void testIOTran(void)
{
	uint8_t i,j,k;
	for(i=0;i<13;i++)
	{
		for(j=0;j<8;j++)
		{
			k=i*8+j;	if(k>99) return;
			if(tmpSet[k]==1) OutputBuf[i]|=(1<<j);
			else OutputBuf[i]&=~(1<<j);			
		}
	}
}

void drawMainFace(unsigned char clearing)
{
	unsigned char j;
	if(clearing==1)	XFILLRAM(0);
	strcpy(mystr,"序号 电压mV  状态");
	printstr(0,0);
	for(j=0;j<3;j++)
	{
		sprintf(mystr,"%03d",curIndex+1+j);	printstr(0,2+j*2);		
		sprintf(mystr,"%04d",Voltage[curIndex+j]);	printstr(42,2+j*2);		
		if(StatusCh[curIndex+j]==1) strcpy(mystr,"放电中");
		else strcpy(mystr,"放电完");
		printstr(84,2+j*2);
	}
	curFace=MAIN_FACE;
}

void refreshMainFace(void)
{
	unsigned char j;
//	uint32_t tmp; 
//	tmp=RTC_GetCounter();
//	if(tmp==0x0001517F)	//Reset RTC Counter when Time is 23:59:59
//	{
//     RTC_SetCounter(0x0);     
//     RTC_WaitForLastTask();		// Wait until last write operation on RTC registers has finished 
//	}
//	sprintf(mystr,"%02d:%02d:%02d",tmp/3600,(tmp/60)%60,tmp%60);	printstr(0,0);		
	for(j=0;j<3;j++)
	{
		sprintf(mystr,"%04d",Voltage[curIndex+j]);	printstr(42,2+j*2);		
		if(StatusCh[curIndex+j]==1) strcpy(mystr,"放电中");
		else strcpy(mystr,"放电完");
		printstr(84,2+j*2);		
	}
}

void drawSetMenu(unsigned char clearing)
{
	if(clearing)XFILLRAM(0);
	if(curMenuRow==0)
	{
		strcpy(mystr,"1 工作状态设置");	printstr(0,0);
		strcpy(mystr,"2 放电模式设置");	printstr(0,2);
		strcpy(mystr,"3 曲线查询    ");	printstr(0,4);
		strcpy(mystr,"4 输出测试    ");	printstr(0,6);
	}else
	{
		strcpy(mystr,"2 放电模式设置");	printstr(0,0);
		strcpy(mystr,"3 曲线查询    ");	printstr(0,2);
		strcpy(mystr,"4 输出测试    ");	printstr(0,4);
		strcpy(mystr,"5 软件版本    ");	printstr(0,6);
	}
	strcpy(mystr,">");	printstr(7,(curMenuIndex-curMenuRow)*2);
	curFace=FACE_SETMENU;	
}

void selectCurve(unsigned char clearing)
{
	unsigned char i;
	if(clearing)
	{
		XFILLRAM(0);
		strcpy(mystr,"曲线查询设置");	printstr(0,0);
	}
	for(i=0;i<3;i++)
	{
		sprintf(mystr,"No.%03d",1+i+subMenuIndex);	printstr(7,2+2*i);
		if(EnableCh[i+subMenuIndex]==1) strcpy(mystr,"开");
		else strcpy(mystr,"关");
		printstr(113,2+2*i);
		if(i+subMenuIndex==curSetIndex)	
		{
			displaycursor(113,2+i*2,14,2);
			strcpy(mystr,">");	
		}
		else strcpy(mystr," ");
		printstr(0,2+2*i);	
		
	}
	curFace=FACE_SELECTCURVE;	
}

void StatusSeting(unsigned char clearing)
{
	unsigned char i;
	if(clearing)
	{
		XFILLRAM(0);
		strcpy(mystr,"工作状态设置");	printstr(0,0);
	}
	for(i=0;i<3;i++)
	{
		sprintf(mystr,"No.%03d",1+i+subMenuIndex);	printstr(7,2+2*i);
		if(EnableCh[i+subMenuIndex]==1) strcpy(mystr,"开");
		else strcpy(mystr,"关");
		printstr(113,2+2*i);
		if(i+subMenuIndex==curSetIndex)	
		{
			displaycursor(113,2+i*2,14,2);
			strcpy(mystr,">");	
		}
		else strcpy(mystr," ");
		printstr(0,2+2*i);	
		
	}
	curFace=FACE_SETSTATUS;	
}

void DrawIOTest(unsigned char clearing)
{
	unsigned char i;
	if(clearing)
	{
		XFILLRAM(0);
		strcpy(mystr,"输出测试");	printstr(0,0);
	}
	for(i=0;i<3;i++)
	{
		sprintf(mystr,"No.%03d",1+i+subMenuIndex);	printstr(7,2+2*i);
		if(tmpSet[i+subMenuIndex]==1) strcpy(mystr,"开");
		else strcpy(mystr,"关");
		printstr(113,2+2*i);
		if(i+subMenuIndex==curSetIndex)	
		{
			displaycursor(113,2+i*2,14,2);
			strcpy(mystr,">");	
		}
		else strcpy(mystr," ");
		printstr(0,2+2*i);	
		
	}
	curFace=FACE_IOTEST;	
}

void ModeSeting(unsigned char clearing)
{
	XFILLRAM(0);
	strcpy(mystr,"放电模式设置");		printstr(0,0);
	strcpy(mystr,"1 时间模式");			printstr(0,2);
	strcpy(mystr,"2 电压模式");			printstr(0,4);
	strcpy(mystr,"3 时间+电压模式");	printstr(0,6);
	strcpy(mystr,">");	printstr(7,2+subMenuIndex*2);	
	strcpy(mystr,"☆");	printstr(113,subMenuIndex*2+2);
	curFace=FACE_MODESET;	
}
void CurveQuery(unsigned char clearing)
{
	if(clearing)	XFILLRAM(0);
	strcpy(mystr,"曲线查询");	printstr(0,0);
	strcpy(mystr," 曲线1");		printstr(0,2);
	strcpy(mystr," 曲线2");		printstr(0,4);
	strcpy(mystr," 曲线3");		printstr(0,6);
	strcpy(mystr,">");				printstr(0,2+curCurveIndex*2);	
	curFace=FACE_CURVEQUERY;	
}
void Version()
{
	XFILLRAM(0);
	strcpy(mystr,"软件版本");	printstr(0,2);
	strcpy(mystr,"V120707");	printstr(28,4);	
	curFace=FACE_VERSION;	
}

void drawTimeValtage(unsigned char clearing)
{
	if(clearing){
		XFILLRAM(0);
		strcpy(mystr,"参数设置");	printstr(0,0);
		if(workMode!=1)	{		strcpy(mystr,"标准时间(秒)");	printstr(14,2);	}
		if(workMode!=0)	{		strcpy(mystr,"标准电压(mV)");	printstr(14,4);	}
	}
	if(workMode!=1)	{	sprintf(mystr,"%04d",setingTime);	printstr(98,2);	}
	if(workMode!=0)	{	sprintf(mystr,"%04d",setingVal);	printstr(98,4);	}
	if(editIndex<4) displaycursor(98+editIndex*7,2,7,2);
	else displaycursor(98+(editIndex-4)*7,4,7,2);
	curFace=FACE_TIMESET;		
}
uint16_t clearBit(uint16_t qDat,unsigned char qBit)
{
	uint16_t i,j,k;
	if(qBit==0) k=(qDat%1000);
	if(qBit==1) {	i=qDat/1000;	j=(qDat%100);	k=i*1000+j;	}
	if(qBit==2) {	i=qDat/100;		j=(qDat%10);	k=i*100+j;	}
	if(qBit==3) {	j=(qDat/10);		k=j*10;	}
	return(k);
}

void inpupNum(unsigned char iNum)
{
	if(curFace==FACE_TIMESET)
	{
		if(editIndex==0) 			setingTime=clearBit(setingTime,0)+1000*iNum;
		else if(editIndex==1) setingTime=clearBit(setingTime,1)+100*iNum;
		else if(editIndex==2) setingTime=clearBit(setingTime,2)+10*iNum;
		else if(editIndex==3) setingTime=clearBit(setingTime,3)+iNum;
		else if(editIndex==4) setingVal=clearBit(setingVal,0)+1000*iNum;
		else if(editIndex==5) setingVal=clearBit(setingVal,1)+100*iNum;
		else if(editIndex==6) setingVal=clearBit(setingVal,2)+10*iNum;
		else if(editIndex==7) setingVal=clearBit(setingVal,3)+iNum;
		drawTimeValtage(0);
	}
}
void drawLogo(void)
{
	uint16_t i,j;
	XFILLRAM(0);
	strcpy(mystr,"佛山市惠龙自动化");		printstr(7,1);
	strcpy(mystr,"设备有限公司");				printstr(21,3);
	strcpy(mystr,"电话:0757-23330686");	printstr(0,5);
	for(i=0;i<2000;i++)	for(j=0;j<3000;j++);		
}

void saveSucceed(void)
{
	uint16_t i,j;
	XFILLRAM(0);
	strcpy(mystr,"保存成功!");		printstr(28,3);
	for(i=0;i<2000;i++)	for(j=0;j<1500;j++);		
}

void drawPicture(void)
{
	long tadd;
	unsigned int i,j,k,pos,len;
	
	XFILLRAM(0);
	for(i=0;i<6;i++)	{	sprintf(mystr,"%d",(5-i));	printSmall(1,i*10);	}
	for(i=0;i<6;i++)	{	for(j=0;j<12;j++)	putpixel(10+j*10,3+i*10,1);			}

	ReadMemory(EditBuf,1,2047,105);
	pos=EditBuf[0];	if(pos>2) pos=0;
	if(curCurveIndex==1) {	if(pos==0) pos=2; else pos--;	}
	if(curCurveIndex==2) {	if(pos==0) pos=2; else pos--;	if(pos==0) pos=2; else pos--;	}
		
	ReadMemory(EditBuf,2,2047,106+saveCurveIndex*2);
	len=EditBuf[1]*256+EditBuf[0];	if(len>600) len=0;

	for(i=0;i<110;i++)
	{
		if(curPages*110+i>len) break;
		tadd=pos*120000L+curPages*220+i*2;		
		read_flash(EditBuf,200,tadd);
		for(j=0;j<100;j++)
		{
			if(tmpSet[j]!=1) continue;
			k=EditBuf[j*2+1]*256+EditBuf[j*2+0];
			putpixel(i+10,3+(5000-k)/100,1);
		}
	}
	sprintf(mystr,"PAGE<%01d>",curPages+1);
	printSmall(42,57);
}


//==========================================================
#include "uimmi_ctrl.h"

struct KEYBOARD_KEY_VAL_MAP {
    unsigned int hdl;
    EVENT_NODE_HANDLE  sig;
};


static const struct KEYBOARD_KEY_VAL_MAP gUiKeyNodeMapTabl[] = {
    {0x10000,  EVENT_KEY_ADD},
    {0x4,      EVENT_KEY_SUB},
    {0x20000,  EVENT_KEY_UP},
    {0x80,     EVENT_KEY_DOWN},
    {0x100,    EVENT_KEY_LEFT},
    {0x400,    EVENT_KEY_RIGHT},
    {0x2,      EVENT_KEY_OK},
    {0x1,      EVENT_KEY_ESC},
    {0x400000, EVENT_KEY_RUN},
    {0x40000,  EVENT_KEY_NUM_0},
    {0x80000,  EVENT_KEY_NUM_1},
    {0x100000, EVENT_KEY_NUM_2},
    {0x200000, EVENT_KEY_NUM_3},
    {0x800,    EVENT_KEY_NUM_4},
    {0x1000,   EVENT_KEY_NUM_5},
    {0x2000,   EVENT_KEY_NUM_6},
    {0x8,      EVENT_KEY_NUM_7},
    {0x10,     EVENT_KEY_NUM_8},
    {0x20,     EVENT_KEY_NUM_9},
};


u_int8 keyboard_scan(struct EVENT_NODE_ITEM *e)
{
    u_int8 ret = 0;
    //check whether any key press
    if(Flag_keyPressed)
    {
        if(curKey!=0) 
        {
            s_int32 i;
            for (i=0; i< ARRAY_SIZE(gUiKeyNodeMapTabl); i++)
            {
                if (curKey == gUiKeyNodeMapTabl[i].hdl)
                {
                    e->sig = gUiKeyNodeMapTabl[i].sig;
                    ret = 1;
                    break;
                }
            }
            curKey=0;  
        }
        Flag_keyPressed=0; 
        preKey=0;
    }    

    return ret;
}




