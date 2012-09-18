#include "brush_print.h"
#include "common_define.h"
#include "stm32f10x.h"
#include "lcd12864.h"

#define USE_HW_LCD

#define SCREEN_WIDTH_PIXEL_NUM                (128)
#define SCREEN_HEIGHT_PIXEL_NUM               (64)
#define SCREEN_ROW_NUM                         (8)
#define SCREEN_ROW_HEIGHT_PIXEL_NUM            (8)


#define Y_PIXLE_TO_ROW(pos_y)       ((pos_y) >> 3) 
#define X_PIXLE_TO_COL(pos_x)       ((pos_x) & 0x3F) 

#define SCREEN_POS_IS_INVALID(x, y) (((x) > (SCREEN_WIDTH_PIXEL_NUM - 1)) || ((y) > (SCREEN_HEIGHT_PIXEL_NUM - 1)))

#define XLCD_DAT	1

#define DEV_LCD_CS_A() {\
    GPIOA->BRR |= 0x00000800;\
    GPIOB->BSRR|= 0x00000100;\
}

#define DEV_LCD_CS_B() {\
    GPIOA->BSRR |= 0x00000800;\
    GPIOB->BRR |= 0x00000100;\
}

#define DEV_LCD_START_FILL() {\
    GPIOA->BRR |= 0x00000800; \
    GPIOB->BRR |= 0x00000100;\
}

#define DEV_LCD_END_FILL() {\
    GPIOA->BSRR |= 0x00000800; \
    GPIOB->BSRR |= 0x00000100;\
}

#define DEV_LCD_MOVE_POS(pos_x, pos_y)  XLCD_MOV_POS(pos_x, pos_y)
#define DEV_LCD_WRITE(data)  XLCD_SEND_DATA((data))
#define DEV_LCD_READ(data)   (data) = XLCD_RECV_DATA()


#define DEV_LCD_CS(pos_x) {\
    if((pos_x)<(SCREEN_WIDTH_PIXEL_NUM/2))\
    {\
        DEV_LCD_CS_A();\
    } else { \
       DEV_LCD_CS_B();\
    }\
}


#define DEV_LCD_SET_DATA(pos_x, pos_y, data)  {\
    DEV_LCD_MOVE_POS(pos_x, pos_y); \
    DEV_LCD_WRITE(data);\
}
#define DEV_LCD_GET_DATA(pos_x, pos_y, data)  {\
    DEV_LCD_MOVE_POS(pos_x, pos_y);\
    DEV_LCD_READ(data); \
}


void Screen_PrintInit(void)
{
    INIT_XLCD();
}

void Screen_PrintFillRect(struct SCREEN_ZONE *rect,  enum PIXEL_MODE pixel_mode)
{
    T_SCREEN_PIXEL i,j;

    for (i=0;i<rect->h;i++)
    {
        for (j=0;j<rect->w;j++)
        {
            Screen_PrintPixel(rect->x + j, rect->y + i, pixel_mode);
        }
    }
}

void Screen_PrintClear(struct SCREEN_ZONE *rect)
{
    T_SCREEN_PIXEL i,j;
    
    if (rect)
    {
        Screen_PrintFillRect(rect, PIXEL_MODE_CLEAR);
    }
    else
    {
        DEV_LCD_START_FILL(); 
        for(j=0;j<SCREEN_ROW_NUM;j++)
        {
            DEV_LCD_MOVE_POS(0, j);
            for(i=0;i<(SCREEN_WIDTH_PIXEL_NUM/2);i++)
            {
                DEV_LCD_WRITE(0);
            }
        }
        DEV_LCD_END_FILL();
    }
}



void Screen_PrintPixel(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, enum PIXEL_MODE pixel_mode)
{
	T_SCREEN_PIXEL tmp;

    if (SCREEN_POS_IS_INVALID(x, y))
    {
        return;
    }

    DEV_LCD_CS(x);
    DEV_LCD_GET_DATA(X_PIXLE_TO_COL(x), Y_PIXLE_TO_ROW(y), tmp);
    if (pixel_mode == PIXEL_MODE_SET)
    {
        tmp |= (1<<((y & 0x07)));
    }
    else if (pixel_mode == PIXEL_MODE_CLEAR)    
    {
        tmp &= (~(1<<((y & 0x7))));
    }
    else if (pixel_mode == PIXEL_MODE_TURN) 
    {
        tmp ^= (1<<((y & 0x7)));
    }
    else if (pixel_mode == PIXEL_MODE_CURSOR)
    {
        tmp ^= 0xFF;
    }
    else
    {// unknown mode
        return;
    }
    DEV_LCD_SET_DATA(X_PIXLE_TO_COL(x), Y_PIXLE_TO_ROW(y), tmp);
}

void Screen_PrintLine(T_SCREEN_PIXEL x1, T_SCREEN_PIXEL y1, T_SCREEN_PIXEL x2, T_SCREEN_PIXEL y2, enum PIXEL_MODE pixel_mode)
{
    int xdelta,ydelta,xstep,ystep,change;

    xdelta=x2-x1;
    ydelta=y2-y1;
    
    if(xdelta<0)
    {
        xdelta=-xdelta;
        xstep=-1;
    }
    else
    {
        xstep=1;
    }

    if(ydelta<0)
    {
        ydelta=-ydelta;
        ystep=-1;
    }
    else 
    {
        ystep=1;
    }

    
    if (xdelta>ydelta)
    {
        change=xdelta>>1;
        while(x1!=x2)
        {
            Screen_PrintPixel(x1, y1, pixel_mode);
            x1+=xstep;
            change+=ydelta;
            if(change>xdelta)
            {
                y1+=ystep;
                change-=xdelta;
            }
        }
    }
    else
    {
        change=ydelta>>1;
        while(y1!=y2)
        {
            Screen_PrintPixel(x1, y1, pixel_mode);
            y1+=ystep;
            change+=xdelta;
            if(change>ydelta)
            {
                x1+=xstep;
                change-=ydelta;
            }
        }
    }
    Screen_PrintPixel(x2, y2, pixel_mode);
}



void Screen_PrintRect(struct OSD_ZONE *zone, enum PIXEL_MODE pixel_mode)
{
    //MY_DEBUG("\n|RECT|(%d,%d)[%d x %d]<%d,%d,%d,%d>\n", 
     //      zone->zone.x, zone->zone.y, zone->zone.w, zone->zone.h,
     //      zone->border.l, zone->border.t, zone->border.r, zone->border.b);

    if (zone->border.t > 0)
        Screen_PrintLine(zone->zone.x,                zone->zone.y, 
                         zone->zone.x + zone->zone.w, zone->zone.y,                pixel_mode);

    if (zone->border.l > 0)
        Screen_PrintLine(zone->zone.x,                zone->zone.y, 
                     zone->zone.x,                    zone->zone.y + zone->zone.h, pixel_mode);

    if (zone->border.r > 0)
        Screen_PrintLine(zone->zone.x + zone->zone.w, zone->zone.y, 
                         zone->zone.x + zone->zone.w, zone->zone.y + zone->zone.h, pixel_mode);

    if (zone->border.b > 0)
        Screen_PrintLine(zone->zone.x,                zone->zone.y + zone->zone.h, 
                         zone->zone.x + zone->zone.w, zone->zone.y + zone->zone.h, pixel_mode);

}
void Screen_PrintEllipse(struct SCREEN_ZONE *rect, T_SCREEN_PIXEL_ATTR attr)
{

}

void Screen_PrintCursor(struct OSD_ZONE *zone, T_SCREEN_PIXEL_ATTR attr)
{
    Screen_PrintFillRect(&zone->zone, PIXEL_MODE_CURSOR);
}


void Screen_PrintString(struct SCREEN_ZONE *rect, u_int8 *str, T_SCREEN_PIXEL_ATTR attr)
{
    u_int32 idx, codelen, len, fontwidth;
    struct UICOM_1PP_BMP_INFO  bmpInfo;
    //MY_DEBUG("\t[TXT] %s\n", str);

    if (NULL == str)
    {
        return;
    }
    
    len = strlen(str);
    if (len)
    {
        idx = 0;
        fontwidth = 0;
        while((str[idx] != '\0') && (idx < len))
        {
            codelen = uicom_font_getdata(str+idx, &bmpInfo, attr);
            if (codelen > 0)
            {
                idx += codelen;
                fontwidth += Screen_PrintFont(rect->x + fontwidth, rect->y, &bmpInfo);
            }
            else
            {
                idx += 1;
            }
        }
    }
}

void Screen_PrintBmp(struct SCREEN_ZONE *rect, u_int8 *data, T_SCREEN_PIXEL_ATTR attr)
{
    MY_DEBUG("\t[BMP] %x\n", data);
}

T_SCREEN_PIXEL Screen_PrintFont(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info)
{
    // byte mode
    if (0 == (info->height % SCREEN_ROW_HEIGHT_PIXEL_NUM))
    {
       return  Screen_PrintFont_By_Byte(x, y, info);
    }
    else 
    {
        return  Screen_PrintFont_By_Bit(x, y, info);
    }
}


T_SCREEN_PIXEL Screen_PrintFont_By_Byte(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info)
{
    T_SCREEN_PIXEL i, j,rowCnt, col, row;

    rowCnt = info->height/SCREEN_ROW_HEIGHT_PIXEL_NUM;
    row = Y_PIXLE_TO_ROW(y);
    for(j=0; j<rowCnt; j++)
    {
        col = x;
        for(i=0; i<info->width; i++)
        {
            if (SCREEN_POS_IS_INVALID(col, j+y))
            {
                break;
            }
            DEV_LCD_CS(col);
            DEV_LCD_SET_DATA(X_PIXLE_TO_COL(col), row + j, info->data[info->width*j + i]);
            col++;
        }
    }

    return info->width;
}



T_SCREEN_PIXEL Screen_PrintFont_By_Bit(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info)
{
    T_SCREEN_PIXEL i, j;
    u_int32 prePixelNum, curPixelPos;
    
    for (i=0; i<info->height; i++)// scan height
    {
        prePixelNum = info->width * i;
        for (j=0; j < info->width; j++) //scan  width
        {
            if (SCREEN_POS_IS_INVALID((x + j), (y + i)))
            {
                break;
            }
            curPixelPos = prePixelNum + j;
            //if ((info->data[curPixelPos/8] << (curPixelPos % 8)) & 0x80)
            if ((info->data[curPixelPos/8]) & (1<<(curPixelPos % 8)))
            {
                Screen_PrintPixel((x + j), (y + i), PIXEL_MODE_SET);
            }
            else
            {
                Screen_PrintPixel((x + j), (y + i), PIXEL_MODE_CLEAR);
            }
        }
    }

    return info->width;
}


void  XLCD_TEST(u_int8 w, u_int8 h)
{
    struct OSD_ZONE rect = {{0,0,128,64}, {1,1,1,1}};

  //  rect.zone.w = w;
 //   rect.zone.h = h;
    if (h!=40){
        rect.zone.y = 10;
        rect.zone.h = 44;
    }
    Screen_PrintFillRect(&rect.zone, (h==40)?PIXEL_MODE_CLEAR:PIXEL_MODE_SET);

    if (h!=40){
        rect.zone.x = 20;
        rect.zone.y = 20;
        rect.zone.w = 20;
        rect.zone.h = 20;
        Screen_PrintRect(&rect, PIXEL_MODE_TURN);
    }
}

