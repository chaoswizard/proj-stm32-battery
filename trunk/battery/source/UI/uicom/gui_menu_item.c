#include "gui_menu_item.h"


//==========================================================
void gmenu_content_list_cell_draw(struct GMENU_CONTENT_LIST *list, u_int8 pos, enum OSD_OBJ_DRAW_TYPE type)
{
    if (list && list->inititem)
    {
        u_int8 idx, itemcount,status;
        struct OSD_ZONE groupzone, cellzone;
        struct UICOM_DATA content = {0};
        u_int8 str[64];
        
        itemcount = (list->initzone)(&groupzone, pos);
        Screen_PrintRect(&groupzone, PIXEL_MODE_SET);
        for (idx=0; idx<itemcount; idx++)
        {
            content.data = str;
            memset(&cellzone, 0, sizeof(cellzone));
            status = (list->inititem)(&cellzone, &content, pos, idx, type);
            cellzone.zone.x += groupzone.zone.x;
            cellzone.zone.y += groupzone.zone.y;
            if (PAINT_STATUS_BORDER & status)
            {
                Screen_PrintRect(&cellzone, PIXEL_MODE_SET);
            }

            if (PAINT_STATUS_DATA & status)
            {
                cellzone.zone.x += cellzone.border.l;
                cellzone.zone.y += cellzone.border.t;
                gui_osd_data_draw(&content, &cellzone.zone);
            }
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



static void list_cell_content_proc(T_UICOM_OBJ_COUNT pos, struct GMENU_CONTENT_LIST *list)
{
    enum OSD_OBJ_DRAW_TYPE type;

    if (pos < list->len)
    {
        if (list->curFocus == pos)
        {
            type = OSD_OBJ_DRAW_SELECTED;
        }
        else
        {
            type = OSD_OBJ_DRAW_NORMAL;
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


//==========================================================

void gmenu_content_tab_cell_draw(struct GMENU_CONTENT_TAB *table, u_int8 row, u_int8 col, enum OSD_OBJ_DRAW_TYPE type)
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
            if (PAINT_STATUS_BORDER & status)
            {
                Screen_PrintRect(&zone, PIXEL_MODE_SET);
            }

            if (PAINT_STATUS_DATA & status)
            {
                zone.zone.x += dataZone.x;
                zone.zone.y += dataZone.y;
                gui_osd_data_draw(&content, &zone.zone);
            }
        }
    }
}


void gmenu_content_tab_cell_clear(struct GMENU_CONTENT_TAB *table, u_int8 row, u_int8 col, u_int8 isClearBorder)
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


static void table_cell_content_proc(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, struct GMENU_CONTENT_TAB *table)
{
   enum OSD_OBJ_DRAW_TYPE type;

   if (col < table->colCount && row < table->rowCount)
   {
       if ((col==table->colFocus && row==table->rowFocus))// || (col==0) || (row==0)
       {
           type = OSD_OBJ_DRAW_SELECTED;
       }
       else
       {
           type = OSD_OBJ_DRAW_NORMAL;
       }
       gmenu_content_tab_cell_draw(table, row, col, type);
   }
}

void gmenu_content_tab_draw(struct GMENU_CONTENT_TAB *table, u_int8 row, u_int8 col, u_int8 rowfocus, u_int8 colfocus)
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

void gmenu_content_tab_clear_row(struct GMENU_CONTENT_TAB *table, u_int8 row1, u_int8 row2, u_int8 isClearBorder)
{
    if (table)
    {
        u_int8 i;
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

void gmenu_content_tab_clear_col(struct GMENU_CONTENT_TAB *table, u_int8 col1, u_int8 col2, u_int8 isClearBorder)
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


