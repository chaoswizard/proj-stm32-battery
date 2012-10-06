#include "brush_print.h"
#include "common_define.h"

//#define USE_HW_LCD

#define SCREEN_WIDTH_PIXEL_NUM                (128)
#define SCREEN_HEIGHT_PIXEL_NUM               (64)
#define SCREEN_POS_IS_INVALID(x, y)           (((x) > (SCREEN_WIDTH_PIXEL_NUM - 1)) || ((y) > (SCREEN_HEIGHT_PIXEL_NUM - 1)))


#define SCREEN_PUT_PIXEL(x, y, mode)                     putpixel(x, y, mode)
#define SCREEN_INIT()                                    INIT_XLCD()
#define SCREEN_CLEAR()                                   XFILLRAM(0)

static T_SCREEN_PIXEL Screen_PrintFont_ColRowMode(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_FONT_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor);
static T_SCREEN_PIXEL Screen_PrintFont_RowMode(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_FONT_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor);

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
    //MY_DEBUG("\n|RECT|(%d,%d)[%dx%d]<%d,%d,%d,%d>\n", 
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

void Screen_PrintEllipse(struct SCREEN_ZONE *rect, T_UICOM_PIXEL_ATTR attr)
{

}

void Screen_PrintCursor(struct OSD_ZONE *zone, T_UICOM_PIXEL_ATTR attr)
{
    struct SCREEN_ZONE rect;

    rect.x = zone->zone.x + zone->border.l;
    rect.y = zone->zone.y + zone->border.t;
    
    rect.w = zone->border.l;// + zone->border.r;
    if (zone->zone.w > rect.w)
    {
        rect.w = zone->zone.w - rect.w;
    }
    else
    {
        rect.w = zone->zone.w;
    }
    
    rect.h = zone->border.t;// + zone->border.b;
    if (zone->zone.h > rect.h)
    {
        rect.h = zone->zone.h - rect.h;
    }
    else
    {
        rect.h = zone->zone.h;
    }

    Screen_PrintFillRect(&rect, attr);
}

void Screen_PrintString(struct SCREEN_ZONE *rect, u_int8 *str, T_UICOM_DATA_ATTR attr)
{
    u_int32 idx, codelen, len, fontwidth, focusbitmap;
    u_int8 pos;
    struct UICOM_1PP_FONT_INFO  bmpInfo;
    enum PIXEL_COLOR  fgColor, bgColor;
    T_UICOM_FONT_SIZE txtsize;
    struct SCREEN_ZONE zone;
    //MY_DEBUG("\t[TXT] %s\n", str);
    if (NULL == str)
    {
        return;
    }

    len = util_strlen(str);
    if (len)
    {
        
        //MY_DEBUG("len=%d / %d\n", len, strlen(str));
        txtsize     = TEXT_ATTR_GET_SIZE(attr);
        fgColor     = TEXT_ATTR_GET_COLOR_FG(attr);
        bgColor     = TEXT_ATTR_GET_COLOR_BG(attr);
        focusbitmap = TEXT_ATTR_GET_FOCUS(attr);
        idx = 0;
        fontwidth = 0;
        pos = 0;
        zone.y  =  rect->y;
        while(UTIL_STR_NOT_NULL(&str[idx]) && (idx < len))
        {
            codelen = util_charlen(str+idx);
            
            //MY_DEBUG("cdlen=%d, idx=%d\n", codelen, idx);
            //default code len
            if (0 == codelen) codelen = 1;
            //
            if (uicom_font_getdata(str+idx, codelen, &bmpInfo, txtsize) > 0)
            {       
                zone.x  = rect->x + fontwidth;
                fontwidth += Screen_PrintFont_RowMode(zone.x , rect->y, &bmpInfo, fgColor, bgColor);
                if (focusbitmap&(1<<pos))
                {
                    zone.w =  bmpInfo.width;
                    zone.h =  bmpInfo.height - 1;
                    Screen_PrintFillRect(&zone, PIXEL_MODE_TURN);
                }
                pos++;
            }
            idx += codelen;
        }
    }
}

void Screen_PrintBmp(struct SCREEN_ZONE *rect, u_int8 *data, T_UICOM_DATA_ATTR attr)
{
    MY_DEBUG("\t[BMP] %x\n", data);
}


#if 0
void Screen_Print_8Bit_V(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, u_int8 data, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor)
{
    T_SCREEN_PIXEL i;
    
    for (i=0;i<8;i++)
    {
        if (data &(1<< i))
        {
            Screen_PrintPixel(x, (y + i),fgcolor);
        }
        else
        {
            Screen_PrintPixel(x, (y + i), bgcolor);
        }
    }
}


  
static T_SCREEN_PIXEL Screen_PrintFont_ColRowMode(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_FONT_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor)
{
    T_SCREEN_PIXEL i, j,k,rowCnt, col, row;


    rowCnt = info->height/8;

    //MY_DEBUG("\n|FONT|(%d,%d)[%dx%d]_%d_[%d]\n", 
   //       x,y,info->width,info->height,info->size, rowCnt);
    
    for(j=0; j<rowCnt; j++)
    {
        col = x;
        row = y + (j*8);
        k = info->width*j;
        for(i=0; i<info->width; i++)
        {
            if (SCREEN_POS_IS_INVALID(col, row))
            {
                break;
            }
            SCREEN_SET_BYTE(col, row, info->data[k + i], fgcolor, bgcolor);
            col++;
        }
    }

    rowCnt = info->height%8;

    

    return info->width;
}
#endif

static T_SCREEN_PIXEL Screen_PrintFont_RowMode(T_SCREEN_PIXEL x, T_SCREEN_PIXEL y, struct UICOM_1PP_FONT_INFO *info, enum PIXEL_COLOR fgcolor, enum PIXEL_COLOR bgcolor)
{
    T_SCREEN_PIXEL i, j, k, m,n,rightMax;
    u_int32 linesize;
    u_int8 *line;

    n = x;
    x += info->left;
    if (y > info->bottom)
    {
        y -= info->bottom;
    }
    rightMax = x;
    linesize = (info->width + 7)&(~0x7);
    for (i=0; i<info->height; i++)// scan height
    {
        line = info->data + linesize * i/8;
        k = y + i;
        for (j=0; j < info->width; j++) //scan  width
        {
            if (SCREEN_POS_IS_INVALID((x + j), k))
            {
                break;
            }
            m = (x + j);
            if ((line[j/8]) & (1<<(j % 8)))
            {
                Screen_PrintPixel(m, k, fgcolor);
                if (m > rightMax)
                {
                    rightMax = m;
                }

            }
            else
            {
                Screen_PrintPixel((x + j), k, bgcolor);
            }
        }
    }

   return (rightMax + 1 - n + info->right);
}
