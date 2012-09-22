#include "gui_menu_item.h"

//==========================================================
void gmenu_item_draw(PGUI_MENU_ITEM item,  enum OSD_OBJ_DRAW_TYPE type)
{
    T_UICOM_OBJ_COUNT idx;

    for (idx=0; idx<item->objsCount; idx++)
    {
        if (item->initobj)
        {
            (item->initobj)(idx, item->objs[idx]);
        }
        
        if (item->objs[idx])
        {
            gui_osd_obj_draw(item->objs[idx], type);
        }
    }
}

void gmenu_item_clear(PGUI_MENU_ITEM item)
{
    T_UICOM_OBJ_COUNT idx;

    for (idx=0; idx<item->objsCount; idx++)
    {
        if (item->objs[idx])
        {
            gui_osd_obj_clear(item->objs[idx]);
        }
    }
}

void gmenu_win_clear(struct SCREEN_ZONE *win)
{
    Screen_PrintClear(win);
}
//==========================================================
void gmenu_list_item_draw(struct GMENU_ITEM_LIST *list, u_int8 pos, enum OSD_OBJ_DRAW_TYPE type)
{
    if (list)
    {
        if (list->inititem)
        {
            (list->inititem)(pos, &list->list[pos]);
        }
        gmenu_item_draw(&list->list[pos], type);
    }
}


static void simple_list_item_proc(T_UICOM_OBJ_COUNT pos, struct GMENU_ITEM_LIST *list)
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
        gmenu_list_item_draw(list, pos, type);
    }
}

void gmenu_list_draw(struct GMENU_ITEM_LIST *list, u_int8 focus, u_int8 drawAttr)
{
    if (list)
    {
        list->curFocus = focus;
        uicom_obj_list(list->len, list, gmenu_item_draw, 0);
    }
}


//==========================================================

void gmenu_content_tab_cell_draw(struct GMENU_CONTENT_TAB *table, u_int8 row, u_int8 col, enum OSD_OBJ_DRAW_TYPE type)
{

    if (table)
    {
        if (table->inititem)
        {
            u_int8 status;
            struct UICOM_DATA content = {0};
            u_int8 str[64];
             
            status = (table->inititem)((PUICOM_DATA)&content, str, row, col);
            if(PAINT_STATUS_SKIP_ALL != status)
            {
                struct OSD_ZONE zone = {0};
                
                (table->initzone)(&zone, row, col);
                if(PAINT_STATUS_SKIP_BORDER != status)
                {
                    Screen_PrintRect(&zone, PIXEL_MODE_SET);
                }

                if (UICOM_DATA_TYPE_TEXT == content.type)
                {
                    if (content.data == NULL)
                    {
                        content.data = str;
                    }
                }

                if(PAINT_STATUS_SKIP_DATA != status)
                {
                    zone.zone.x += 2;
                    zone.zone.y += 2;
                    gui_osd_data_draw(&content, &zone.zone);
                }
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



