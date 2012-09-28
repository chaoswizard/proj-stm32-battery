#include "gui_menu_item.h"


//==========================================================
void gmenu_content_list_cell_draw(struct GMENU_CONTENT_LIST *list, u_int8 pos, enum T_UICOM_STATUS type)
{
    if (list && list->inititem)
    {
        T_UICOM_COUNT idx, itemcount;
        T_UICOM_PAINT_FLAG status;
        struct OSD_ZONE groupzone, cellzone;
        struct UICOM_DATA content = {0};
        u_int8 str[64];
        
        itemcount = (list->initzone)(&groupzone, pos);
        for (idx=0; idx<itemcount; idx++)
        {
            content.data = str;
            memset(&cellzone, 0, sizeof(cellzone));
            status = (list->inititem)(&cellzone, &content, pos, idx, type);
            cellzone.zone.x += groupzone.zone.x;
            cellzone.zone.y += groupzone.zone.y;
            if (PAINT_FLAG_BORDER & status)
            {
                Screen_PrintRect(&cellzone, PIXEL_MODE_SET);
            }

            if (PAINT_FLAG_DATA & status)
            {
                cellzone.zone.x += cellzone.border.l;
                cellzone.zone.y += cellzone.border.t;
                gui_osd_data_draw(&content, &cellzone.zone);
            }
        }
        if (UICOM_STATUS_SELECTED == type)
        {
            Screen_PrintFillRect(&groupzone.zone, PIXEL_MODE_TURN);
        }
        else
        {
            Screen_PrintRect(&groupzone, PIXEL_MODE_SET);
        }
    }
}

void gmenu_content_list_cell_clear(struct GMENU_CONTENT_LIST *list, u_int8 pos, u_int8 isClearBorder)
{
    if (list->initzone)
    {
        struct OSD_ZONE zone;
        
        (list->initzone)(&zone, pos);
        if (!isClearBorder)//±£Áô±ß¿ò
        {
            zone.zone.x += 1;
            zone.zone.y += 1;
            zone.zone.w -= 1;
            zone.zone.h -= 1;
        }
        Screen_PrintClear(&zone.zone);
    }
}

void gmenu_content_list_clear(struct GMENU_CONTENT_LIST *list, u_int8 pos1, u_int8 pos2, u_int8 isClearBorder)
{
    if (list)
    {
        while(pos1<=pos2)
        {
            gmenu_content_list_cell_clear(list, pos1, isClearBorder);
            pos1++;
        }
    }
}

void gmenu_content_list_clear_all(struct GMENU_CONTENT_LIST *list, u_int8 isClearBorder)
{
    if (list)
        if (list->len)
            gmenu_content_list_clear(list, 0, list->len - 1, isClearBorder);
}


static void list_cell_content_proc(T_UICOM_COUNT pos, struct GMENU_CONTENT_LIST *list)
{
    enum T_UICOM_STATUS type;

    if (pos < list->len)
    {
        if (list->curFocus == pos)
        {
            type = UICOM_STATUS_SELECTED;
        }
        else
        {
            type = UICOM_STATUS_NORMAL;
        }
        gmenu_content_list_cell_draw(list, pos, type);
    }
}

void gmenu_content_list_draw(struct GMENU_CONTENT_LIST *list, u_int8 len, u_int8 focus)
{
    if (list && len)
    {
        list->curFocus = focus;
        list->len = len;
        uicom_obj_list(list->len, list, list_cell_content_proc, 0);
    }
}

void gmenu_content_list_changefocus(struct GMENU_CONTENT_LIST *list, u_int8 focus)
{
    if (list)
    {
        if (focus < list->len)
        {
            gmenu_content_list_cell_clear(list, list->curFocus, 1);
            gmenu_content_list_cell_draw(list, list->curFocus, UICOM_STATUS_NORMAL);
            list->curFocus = focus;
            gmenu_content_list_cell_clear(list, list->curFocus, 1);
            gmenu_content_list_cell_draw(list, list->curFocus, UICOM_STATUS_SELECTED);
        }
    }
}

u_int8  gmenu_content_list_getfocus(struct GMENU_CONTENT_LIST *list)
{
    if (list)
        return list->curFocus;

    return 0;
}
//----------------------------------------------------------------------

void gmenu_content_list_movefocus(struct GMENU_CONTENT_LIST *list, s_int8 step, bool_t isloop)
{
    s_int8 cur;
    if (NULL == list)
    {
        return;
    }

    cur = (s_int8)list->curFocus;
    cur += step;
    // if invalid
    if (cur < 0 || cur >= list->len)
    {
        if (isloop)
        {
            if (cur < 0) cur = list->len - 1;
            else cur = 0;
        }
        else
        {
            if (cur < 0) cur = 0;
            else cur = list->len - 1;
        }
    }

    gmenu_content_list_changefocus(list, cur);
    
}

//==========================================================
void gmenu_content_tab_cell_draw(struct GMENU_CONTENT_TAB *table, 
        T_UICOM_COUNT row, T_UICOM_COUNT col, enum T_UICOM_STATUS type)
{
    if (table && table->inititem)
    {
        u_int8 status;
        struct UICOM_DATA content = {0};
        u_int8 str[64];
        struct SCREEN_ZONE dataZone = {0};
        
        content.data = str;
        status = (table->inititem)(&dataZone, (PUICOM_DATA)&content, row, col, type);
        if (PAINT_STATUS_NULL != status)
        {
            struct OSD_ZONE zone = {0};
            
            (table->initzone)(&zone, row, col);
            if (PAINT_FLAG_DATA & status)
            {
                dataZone.x += zone.zone.x;
                dataZone.y += zone.zone.y;
                gui_osd_data_draw(&content, &dataZone);
            }
            
            if (PAINT_FLAG_BORDER & status)
            {
                if (UICOM_STATUS_SELECTED == type)
                {
                    Screen_PrintFillRect(&zone.zone, PIXEL_MODE_TURN);
                }
                else
                {
                    Screen_PrintRect(&zone, PIXEL_MODE_SET);
                }
            }
        }
    }
}


void gmenu_content_tab_cell_clear(struct GMENU_CONTENT_TAB *table, 
                                T_UICOM_COUNT row, T_UICOM_COUNT col, 
                                bool_t isClearBorder)
{
    if (table->initzone)
    {
        struct OSD_ZONE zone;
        
        (table->initzone)(&zone, row, col);
        if (!isClearBorder)//±£Áô±ß¿ò
        {
            zone.zone.x += 1;
            zone.zone.y += 1;
            zone.zone.w -= 1;
            zone.zone.h -= 1;
        }
        Screen_PrintClear(&zone.zone);
    }
}


static void table_cell_content_proc(T_UICOM_COUNT row, T_UICOM_COUNT col, 
                                                struct GMENU_CONTENT_TAB *table)
{
   enum T_UICOM_STATUS type;

   if (col < table->colCount && row < table->rowCount)
   {
       if ((col==table->colFocus && row==table->rowFocus))// || (col==0) || (row==0)
       {
           type = UICOM_STATUS_SELECTED;
       }
       else
       {
           type = UICOM_STATUS_NORMAL;
       }
       gmenu_content_tab_cell_draw(table, row, col, type);
   }
}

void gmenu_content_tab_draw(struct GMENU_CONTENT_TAB *table, 
                        T_UICOM_COUNT row, T_UICOM_COUNT col, 
                        T_UICOM_COUNT rowfocus, T_UICOM_COUNT colfocus)
{
    if (table)
    {
        table->colFocus = colfocus;
        table->rowFocus = rowfocus;
        table->rowCount = row;
        table->colCount = col;
        uicom_obj_tab(row, col, table, table_cell_content_proc, 0);
    }
}

void gmenu_content_tab_clear_row(struct GMENU_CONTENT_TAB *table, 
                                T_UICOM_COUNT row1, T_UICOM_COUNT row2,
                                bool_t isClearBorder)
{
    if (table)
    {
        T_UICOM_COUNT i;
        while(row1<=row2)
        {
            for (i=0;i<table->colCount;i++)
            {
                gmenu_content_tab_cell_clear(table, row1, i, isClearBorder);
            }
            row1++;
        }
    }
}

void gmenu_content_tab_clear_col(struct GMENU_CONTENT_TAB *table, 
                                T_UICOM_COUNT col1, T_UICOM_COUNT col2, 
                                bool_t isClearBorder)
{
    if (table)
    {
        u_int8 i;
        while(col1<=col2)
        {
            for (i=0;i<table->rowCount;i++)
            {
                gmenu_content_tab_cell_clear(table, i,  col1,isClearBorder);
            }
            col1++;
        }
    }
}



void gmenu_content_tab_changefocus(struct GMENU_CONTENT_TAB *table, 
                                T_UICOM_COUNT row, T_UICOM_COUNT col)
{
    if (table)
    {
        if ((col==table->colFocus && row==table->rowFocus))// || (col==0) || (row==0)
        {
            gmenu_content_tab_cell_clear(table, table->rowFocus, table->colFocus, 1);
            gmenu_content_tab_cell_draw(table, table->rowFocus, table->colFocus, UICOM_STATUS_NORMAL);
            table->rowFocus = row;
            table->colFocus = col;
            gmenu_content_tab_cell_clear(table, table->rowFocus, table->colFocus, 1);
            gmenu_content_tab_cell_draw(table, table->rowFocus, table->colFocus, UICOM_STATUS_SELECTED);
        }
    }
}

bool_t  gmenu_content_tab_getfocus(struct GMENU_CONTENT_TAB *table, 
                                T_UICOM_COUNT *row, T_UICOM_COUNT *col)
{
    if (table)
    {
        if (row) (*row) = table->rowFocus;
        if (col) (*col) = table->colFocus;
    }
    if (row) (*row) = 0;
    if (col) (*col) = 0;
    return 0;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
static T_UICOM_COUNT map_cell_value_next(T_UICOM_COUNT *key,  struct GMENU_VALUE_MAP *map)
{
    T_UICOM_COUNT next = (*key);

    next ++;

    if (next < map->count)
    {
        (*key) = next;
        return 1;
    }
    
    return 0;  
}

static void map_cell_value_proc(T_UICOM_COUNT key,  struct GMENU_VALUE_MAP *map)
{
    T_UICOM_COUNT  x, y;

    x = key;
    if (PAINT_MAP_NULL == (map->init)(map->curCh, &x, &y))
    {
        map->paintflg = PAINT_STATUS_NULL;
        return;
    }
    
    if ((0 == key) || (PAINT_STATUS_NULL == map->paintflg))
    {
        Screen_PrintPixel(x, y, PIXEL_MODE_SET);
        map->paintflg = PAINT_FLAG_DATA;
    }
    else
    {
        Screen_PrintLine(map->pre_x, map->pre_y, x , y, PIXEL_MODE_SET);
    }
    map->pre_x = x;
    map->pre_y = y;
}

void gmenu_value_map_draw(struct GMENU_VALUE_MAP *map, struct OSD_ZONE *zone,
                        T_UICOM_COUNT count, T_UICOM_COUNT unit,
                        T_UICOM_COUNT chCount) 
{
    if (map)
    {
        T_UICOM_COUNT grp;

        Screen_PrintRect(zone, PIXEL_MODE_SET);
        Screen_PrintMapRule(zone, PIXEL_MODE_SET);
        map->steps = unit;
        for (grp=0; grp<chCount; grp++)
        {
            map->count  = count;
            map->curCh  = grp;
            map->paintflg = PAINT_STATUS_NULL;
            uicom_obj_map(0, map, map_cell_value_next, map_cell_value_proc, 0);
        }
    }
}

//==========================================================

