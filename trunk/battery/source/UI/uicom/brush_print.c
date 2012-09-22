#include "brush_print.h"
#include "common_define.h"

#define USE_HW_LCD

#define SCREEN_WIDTH_PIXEL_NUM                (128)
#define SCREEN_HEIGHT_PIXEL_NUM               (64)
#define SCREEN_POS_IS_INVALID(x, y)           (((x) > (SCREEN_WIDTH_PIXEL_NUM - 1)) || ((y) > (SCREEN_HEIGHT_PIXEL_NUM - 1)))

#ifdef USE_HW_LCD
#define SCREEN_PUT_PIXEL(x, y, mode)           putpixel(x, y, mode)
#define SCREEN_INIT()                          INIT_XLCD()
#define SCREEN_CLEAR()                         XFILLRAM(0)
#else
#define SCREEN_PUT_PIXEL(x, y, mode)
#define SCREEN_INIT()
#define SCREEN_CLEAR()                         //system("cls")
#endif


void Screen_PrintInit(void)
{
    SCREEN_INIT();
}

void Screen_PrintFillRect(struct SCREEN_ZONE *rect,  enum PIXEL_COLOR pixel_mode)
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
        SCREEN_CLEAR();
    }
}



void Screen_PrintPixel(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, enum PIXEL_COLOR pixel_mode)
{
    SCREEN_PUT_PIXEL(x, y, pixel_mode);
}

void Screen_PrintLine(T_SCREEN_PIXEL x1, T_SCREEN_PIXEL y1, T_SCREEN_PIXEL x2, T_SCREEN_PIXEL y2, enum PIXEL_COLOR pixel_mode)
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



void Screen_PrintRect(struct OSD_ZONE *zone, enum PIXEL_COLOR pixel_mode)
{
    MY_DEBUG("\n|RECT|(%d,%d)[%dx%d]<%d,%d,%d,%d>\n", 
           zone->zone.x, zone->zone.y, zone->zone.w, zone->zone.h,
           zone->border.l, zone->border.t, zone->border.r, zone->border.b);

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
    enum PIXEL_COLOR txtsize, fgColor, bgColor;
    //MY_DEBUG("\t[TXT] %s\n", str);

    if (NULL == str)
    {
        return;
    }
    
    len = strlen(str);
    if (len)
    {
        txtsize  = TEXT_ATTR_SIZE(attr);
        fgColor  = TEXT_ATTR_COLOR_FG(attr);
        bgColor  = TEXT_ATTR_COLOR_BG(attr);
        idx = 0;
        fontwidth = 0;
        while((str[idx] != '\0') && (idx < len))
        {
            codelen = uicom_font_getdata(str+idx, &bmpInfo, txtsize);
            if (codelen > 0)
            {
                idx += codelen;
                fontwidth += Screen_PrintFont(rect->x + fontwidth, rect->y, &bmpInfo, fgColor, bgColor);
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

T_SCREEN_PIXEL Screen_PrintFont(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor)
{
#ifdef USE_HW_LCD
    // byte mode
    if (0 == (info->height % 8))
    {
       return  XLCD_DRAW_FONT_BY_BYTE(x, y, info, fgcolor, bgcolor);
    }
    else 
#endif
    {
        return  Screen_PrintFont_By_Bit(x, y, info, fgcolor, bgcolor);
    }
}



T_SCREEN_PIXEL Screen_PrintFont_By_Bit(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_BMP_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor)
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
                Screen_PrintPixel((x + i), (y + j), fgcolor);
            }
            else
            {
                Screen_PrintPixel((x + i), (y + j), bgcolor);
            }
        }
    }

    return info->height;
}

