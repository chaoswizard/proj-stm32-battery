#include "gui_menu_item.h"


//==========================================================
void gmenu_content_list_cell_draw(struct GMENU_CONTENT_LIST *list, u_int8 pos, u_int16 type)
{
    if (list && list->inititem)
    {
        T_UICOM_COUNT idx, itemcount;
        T_UICOM_DRAW_MODE status;
        struct OSD_ZONE groupzone, cellzone;
        struct UICOM_DATA content = {0};
        u_int8 str[64];
        
        itemcount = (list->initzone)(&groupzone, pos);
        for (idx=0; idx<itemcount; idx++)
        {
            content.data = str;
            memset(&cellzone, 0, sizeof(cellzone));
            status = (list->inititem)(&cellzone, &content, pos, idx, &type);
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
                if (PAINT_FLAG_CLEAR& status)
                {
                    Screen_PrintClear(&cellzone.zone);
                }
                gui_osd_data_draw(&content, &cellzone.zone);
            }
        }
        if (UICOM_TST_SLECTED(type))
        {
            Screen_PrintCursor(&groupzone, PIXEL_MODE_TURN);
        }
        
        Screen_PrintRect(&groupzone, PIXEL_MODE_SET);
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
            if (zone.border.l)
            zone.zone.x += 1;
            if (zone.border.t)
            zone.zone.y += 1;
            if (zone.border.r)
            zone.zone.w -= 1;
            if (zone.border.b)
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
    T_UICOM_EVENT type;

    if (pos < list->len)
    {
        if (list->curFocus == pos)
        {
            UICOM_SET_SLECTED(type);
        }
        else
        {
            UICOM_CLR_SLECTED(type);
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
            if (focus != list->curFocus)
            {
                gmenu_content_list_cell_clear(list, list->curFocus, 1);
                gmenu_content_list_cell_draw(list, list->curFocus, UICOM_STATUS_NORMAL);
            }
            MY_DEBUG("MoveFocus %d->%d\n", list->curFocus, focus);
            list->curFocus = focus;
            gmenu_content_list_cell_clear(list, list->curFocus, 1);
            gmenu_content_list_cell_draw(list, list->curFocus, UICOM_STATUS_SELECTED);
        }
    }
}

void  gmenu_content_list_update(struct GMENU_CONTENT_LIST *list, u_int8 focus,  u_int16 type)
{
    if (list)
    {
        gmenu_content_list_cell_draw(list, focus, type);
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
        T_UICOM_COUNT row, T_UICOM_COUNT col, u_int16 type)
{
    if (table && table->inititem)
    {
        u_int8 status;
        struct UICOM_DATA content = {0};
        u_int8 str[64];
        struct SCREEN_ZONE dataZone = {0};
        
        content.data = str;
        status = (table->inititem)(&dataZone, (PUICOM_DATA)&content, row, col, &type);
        if (PAINT_FLAG_EMPTY != status)
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
                if (UICOM_TST_SLECTED(type))
                {
                    Screen_PrintCursor(&zone, PIXEL_MODE_TURN);
                }

                Screen_PrintRect(&zone, PIXEL_MODE_SET);
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
   T_UICOM_EVENT type;

   if (col < table->colCount && row < table->rowCount)
   {
       if ((col==table->colFocus && row==table->rowFocus))// || (col==0) || (row==0)
       {
           UICOM_SET_SLECTED(type);
       }
       else
       {
           UICOM_CLR_SLECTED(type);
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
            if ((table->rowFocus != row) && (table->colFocus != col))
            {
                gmenu_content_tab_cell_clear(table, table->rowFocus, table->colFocus, 1);
                gmenu_content_tab_cell_draw(table, table->rowFocus, table->colFocus, UICOM_STATUS_NORMAL);
            }
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

    if (next <=  map->mapcount)
    {
        (*key) = next;
        map->curMap = next;
        return 1;
    }
    
    return 0;  
}

static void map_cell_value_proc(T_UICOM_COUNT key,  struct GMENU_VALUE_MAP *map)
{
    T_UICOM_COUNT  x, y;
    T_UICOM_COUNT grp;
    
    for (grp=0; grp<map->penCount; grp++)
    {
        if (0 == UTIL_BIT_TST(map->penSwitchBitmap, grp))
        {
            continue;
        }
        
        x = key;
        
        if (PAINT_FLAG_EMPTY == (map->mapinit)(grp, &x, &y))
        {
            map->penTbl[grp].paintflg = PAINT_FLAG_EMPTY;
            continue;
        }
        
        if ((0 == key) || (PAINT_FLAG_EMPTY == map->penTbl[grp].paintflg))
        {
            Screen_PrintPixel(x, y, PIXEL_MODE_SET);
            map->penTbl[grp].paintflg = PAINT_FLAG_DATA;
        }
        else
        {
            Screen_PrintLine(map->penTbl[grp].pre_x, map->penTbl[grp].pre_y, x , y, PIXEL_MODE_SET);
        }
        map->penTbl[grp].pre_x = x;
        map->penTbl[grp].pre_y = y;
    }
}


static void gmenu_ruler_cell_draw(T_UICOM_ORDER orderflg, u_int16 x, u_int16 y, u_int16 idx, T_UICOM_DRAW_MODE (*initname_cbf)(T_UICOM_ORDER order, u_int16 num, struct OSD_ZONE *cellzone, PUICOM_DATA item))
{
    if (initname_cbf)
    {   
        struct UICOM_DATA content = {0};
        struct OSD_ZONE dataZone = {0};
        u_int8 str[64];
        T_UICOM_DRAW_MODE status;
        
        content.data = str;
        SCREEN_ZONE_INIT(&dataZone.zone, x, y, 0, 0);
        
        status = (*initname_cbf)(orderflg, idx, &dataZone, &content);
        if (PAINT_FLAG_BORDER & status)
        {
            Screen_PrintRect(&dataZone, PIXEL_MODE_SET);
        }
        
        if (PAINT_FLAG_DATA & status)
        {
            dataZone.zone.x += dataZone.border.l;
            dataZone.zone.y += dataZone.border.t;
            gui_osd_data_draw(&content, &dataZone.zone);
        }
    }
}

void gmenu_ruler_draw(struct OSD_ZONE *zone, enum PIXEL_COLOR pixel_mode, T_UICOM_DRAW_MODE (*initname_cbf)(T_UICOM_ORDER, u_int16, struct OSD_ZONE *, PUICOM_DATA))
{
    u_int16 align, next, limt, idx, nextnext;
    
    Screen_PrintRect(zone, pixel_mode);
    
    if (zone->border.t > 0)
    {
        align= zone->zone.y + 1;
        nextnext = next = zone->zone.x; 
        limt = zone->zone.x + zone->zone.w;
        idx = 0;
        do
        {
            nextnext += zone->border.t;
            Screen_PrintPixel(next, align, pixel_mode);
            if (initname_cbf) 
                gmenu_ruler_cell_draw((nextnext < limt)?LINE_DIR_X_LR:(LINE_DIR_X_LR|LINE_DIR_BIT_ENDIAN), 
                                        next, align, idx, initname_cbf);
            idx++;
            next = nextnext;
        }while (next < limt);
    }

    if (zone->border.l > 0)
    {
        align= zone->zone.x + 1;
        next = zone->zone.y + zone->zone.h; 
        limt = zone->zone.y;
        idx = 0;
        do
        {
            Screen_PrintPixel(align, next, pixel_mode);
            if (next < zone->border.l)
            {
                if (initname_cbf) gmenu_ruler_cell_draw(LINE_DIR_Y_BT|LINE_DIR_BIT_ENDIAN, align, next, idx, initname_cbf);
                break;
            }
            else
            {
                if (initname_cbf) gmenu_ruler_cell_draw(LINE_DIR_Y_BT, align, next, idx, initname_cbf);
                next -= zone->border.l;
            }
            idx++;
            //MY_DEBUG("\tLL_(%d,%d)+%d = %d\n", align, next, zone->border.b, limt);
        }while (next >= limt);
    }

    if (zone->border.r > 0)
    {
        align= zone->zone.x + zone->zone.w - 1;
        next = zone->zone.y + zone->zone.h; 
        limt = zone->zone.y;
        idx = 0;
        do
        {
            Screen_PrintPixel(align, next,  pixel_mode);
            if (next < zone->border.r)
            {
                if (initname_cbf) gmenu_ruler_cell_draw(LINE_DIR_Y_BT|LINE_DIR_BIT_ENDIAN, align, next, idx,initname_cbf);
                break;
            }
            else
            {
                if (initname_cbf) gmenu_ruler_cell_draw(LINE_DIR_Y_BT, align, next, idx, initname_cbf);
                next -= zone->border.r;
            }
            idx++;
        }while (next >= limt);
    }

    if (zone->border.b > 0)
    {
        align= zone->zone.y + zone->zone.h -1;
        nextnext = next = zone->zone.x; 
        limt = zone->zone.x + zone->zone.w;
        idx = 0;
        do
        {
            nextnext += zone->border.b;
            Screen_PrintPixel(next, align, pixel_mode);
            if (initname_cbf) 
                gmenu_ruler_cell_draw((nextnext <= limt)?LINE_DIR_X_LR:(LINE_DIR_X_LR|LINE_DIR_BIT_ENDIAN), 
                next, align, idx, initname_cbf);
            idx++;
            next = nextnext;
        }while (next <= limt);
    }
}


static struct GMENU_MAP_PEN  gValueMapPenTab[8];


void gmenu_value_map_draw(struct GMENU_VALUE_MAP *map,
                        T_UICOM_COUNT count, T_UICOM_COUNT penCount, u_int8 penswitch) 
{
    if (map && (penCount > 0) && (penCount<=8))
    {
        //--------------------------------------------------------------
        map->penCount  = penCount;
        map->mapcount  = count;
        map->penTbl  = gValueMapPenTab;
        map->penSwitchBitmap = penswitch;
        gmenu_value_map_reset(map);
        uicom_obj_map(0, map, map->mapcount, map_cell_value_next, map_cell_value_proc, 0);
    }
}

T_UICOM_COUNT   gmenu_value_get_curkey(struct GMENU_VALUE_MAP *map)
{
        if (map)
            return   map->curMap;
         else
         return  0;
}

void    gmenu_value_map_reset(struct GMENU_VALUE_MAP *map)
{
            T_UICOM_COUNT grp;

            map->curMap = 0;
            for (grp=0; grp<map->penCount; grp++)
            {
                map->penTbl[grp].paintflg = PAINT_FLAG_EMPTY;
                map->penTbl[grp].pre_x = 0;
                map->penTbl[grp].pre_y = 0;
            }
}

void gmenu_value_map_draw_next(struct GMENU_VALUE_MAP *map,
                                  T_UICOM_COUNT count,  T_UICOM_COUNT penCount, u_int8 penSwitch) 
{
    if  (map )
    {
        map->penCount             = penCount;
        map->penSwitchBitmap = penSwitch;
        map->penTbl  = gValueMapPenTab;
        map->mapcount += count;

        xprintf("DNext:%d+%d,%d\n", map->curMap, count, map->mapcount);
        
        uicom_obj_map(map->curMap, map, count,  map_cell_value_next, map_cell_value_proc, 0);
    }
}

//==========================================================
struct GMENU_INPUT gInputEditorInstance = {0};
static u_int8 gInputCharBuffer[10];
static struct GMENU_INPUT  *input = NULL;
//==========================================================
static u_int8 gmenu_input_reset_pos(bool_t ishead)
{
    T_INPUT_EVENT evt = 0;
    
    UTIL_ASSERT_VALUE(input, 0, 0);
    
    //here just only clear the focus, not clear usr's callback, the callback's valid need check by caller.
    if (ishead)
    {
        evt = INPUT_PROC_SET(INPUT_PROC_MOVE, 2);
    }
    else
    {
        evt = INPUT_PROC_SET(INPUT_PROC_MOVE, 3);
    }

    input->curcharnum = uicom_obj_input(evt, input->charbuffer, &input->curfocus, input->cfg.limitlen, NULL);

    return input->curfocus;
}

//author:wangxi
// isreset: if evt process finished, if false, evt will bypass to other routinue, other event not pass to outside
// retval: if do exchange successful, return true;
static bool_t gmenu_input_data_exchange(u_int8 checktype, bool_t *isbypass)
{
    bool_t isExchange = FALSE;
    bool_t isBypass = TRUE;

    UTIL_ASSERT_VALUE(input, 0, FALSE);

    if (input->cfg.inputdetector && (0 != (input->cfg.checktype & checktype)))
    {
        u_int32 val = 0;
        u_int16 len = util_strlen(input->charbuffer);
        u_int16 echostatus;
        
        switch (input->cfg.type)
        {
            case GMENU_INPUT_TYPE_VALUE:
                val = atoi(input->charbuffer);
                //util_bit32_dump("mask=\n",input->cfg.checktype);
                //util_bit32_dump("type=\n",checktype);
                //MY_DEBUG("DOEXCHANGE[%s]_BEGIN\n", input->charbuffer);
                //MY_DEBUG(">>>>>>(%d)\n", val);
                echostatus = (*input->cfg.inputdetector)(checktype, (p_void)val, len, input->cfg.param);
                //util_bit32_dump(">>>>ECHO:\n",echostatus);
                if (INPUT_EXCHANGE_STATUS(echostatus))
                {
                    gmenu_input_makestr(GMENU_INPUT_TYPE_VALUE, input->charbuffer, (p_void)val);
                    isExchange = TRUE;
                    //MY_DEBUG("<<<<<<[%s]\n", input->charbuffer);
                }
                if (INPUT_BYPASS_STATUS(echostatus) & checktype)
                {
                    //MY_DEBUG("-------- BYPASS-----\n");
                    isBypass = FALSE;
                }
                //MY_DEBUG("DOEXCHANGE[%s]_END  ret=%d\n", input->charbuffer, isExchange);
                break;
            case GMENU_INPUT_TYPE_CHAR:
                echostatus = (*input->cfg.inputdetector)(checktype, (p_void)input->charbuffer, len, input->cfg.param);
                isExchange = TRUE;
                isBypass = TRUE;
                break;
            default:
                break;
        }
    }
    
    if (isbypass)(*isbypass) = isBypass;
    
    return isExchange;
}

void gmenu_input_close(void)
{
    UTIL_ASSERT_VOID(input, 0);
    
    memset(input, 0, sizeof(struct GMENU_INPUT));
    input = NULL;
}

u_int8 gmenu_input_makestr(enum GMENU_INPUT_TYPE type, u_int8 *buffer,p_void data)
{
    switch (type)
    {
        case GMENU_INPUT_TYPE_VALUE:
            sprintf(buffer, "%d", (u_int32)data);
            break;
        case GMENU_INPUT_TYPE_CHAR:
            if (data)
            {
                util_strcpy(buffer, (p_cstr)data);
            }
        default:
            break;
    }
    return 0;
}


u_int8 gmenu_input_getstr(u_int8 *buffer)
{
    if (input)
    {
        if (input->charbuffer && (buffer != input->charbuffer))
        {
            util_strncpy(buffer, input->charbuffer, input->maxlen);
        }
        return input->curfocus;
    }
    
    buffer[0] = '\0';
    return 0;
}

bool_t gmenu_input_proc(T_INPUT_EVENT evt_val, p_void param)
{
    u_int16 count, evt;
    bool_t ret = FALSE;
    
    UTIL_ASSERT_VALUE(input, 0, FALSE);

    evt = INPUT_PROC_TYPE(evt_val);
    if (INPUT_PROC_NULL != evt)
    {
        if (INPUT_PROC_DEL == evt)
        {
            if (!UTIL_STR_NOT_NULL(input->charbuffer))
            {
                // if exchange successful, switch to replace mode
                gmenu_input_data_exchange(INPUT_STATUS_DEL_EMPTY, &ret);
                gmenu_input_reset_pos(TRUE);
                input->appendflg = FALSE;// is empty, auto switch append mode
                return ret;
            }
        }
        
        if (INPUT_PROC_NEW == evt)
        {
            if (input->cfg.isinsert) 
            {
                evt = UTIL_STR_ADD_INSERT;
            }
            else 
            {
                if (input->appendflg)
                {
                    ret = TRUE;
                    evt = UTIL_STR_ADD_APPEND;
                }
                else
                {
                    evt = UTIL_STR_ADD_REPLACE;
                }
            }
            
            if (input->curfocus == input->curcharnum - 1)
            {
                input->appendflg = TRUE;
            }
            
            evt_val = INPUT_PROC_SET(INPUT_PROC_NEW, evt);
            evt = INPUT_PROC_NEW;
        }
        else //if (INPUT_PROC_MOVE != evt)// if just 1 char or nothing, not change flg
        {
            input->appendflg = FALSE;
        }
        
        count = uicom_obj_input(evt_val, input->charbuffer, &input->curfocus,
            input->cfg.limitlen, param);

        //MY_DEBUG("\n\n>>>> BEGIN[%s]: %d/%d->%d @ %d\n", input->charbuffer, input->curfocus, 
         //   input->curcharnum, count, input->appendflg);
        if (count)
        {
            if ((count>1)&&(input->curfocus == input->curcharnum - 1) && (count == input->curcharnum) && ret)
            {
                input->appendflg = FALSE;
            }
            
            if ((count == input->cfg.limitlen) && (INPUT_PROC_NEW == evt))
            {
                input->appendflg = !gmenu_input_data_exchange(INPUT_STATUS_LEN_LIMIT, &ret);
            }
            else
            {
                ret = TRUE;
            }
        }
        else
        {
            input->appendflg = !gmenu_input_data_exchange(INPUT_STATUS_DEL_ALL, &ret);
        }

        input->curcharnum = uicom_obj_input(INPUT_PROC_SET(INPUT_PROC_UPDATE, 0), 
                     input->charbuffer, &input->curfocus, input->cfg.limitlen, NULL);
        //MY_DEBUG("\n\n>>>> ENDED[%s]: %d/%d->%d @ %d\n", input->charbuffer, input->curfocus, 
        //    input->curcharnum, count, input->appendflg);
    }
    else if (param)
    {
        // if  exchange successful, retun true, reset means this event need other routinue process
        input->appendflg = !gmenu_input_data_exchange(INPUT_PROC_SUBTYPE(evt_val), &ret);
        gmenu_input_reset_pos(TRUE);
        input->isInit = ret;
    }

    return ret;
}

static void gmenu_input_init(enum GMENU_INPUT_TYPE type, p_void defData, bool_t isFocusTail)
{
    UTIL_ASSERT_VOID(input, 0);
    
    memset(input->charbuffer, 0, input->maxlen);
    input->cfg.type = type;
    gmenu_input_makestr(type, input->charbuffer, defData);
    gmenu_input_reset_pos(!isFocusTail);
    input->appendflg = isFocusTail;
}

bool_t gmenu_input_getstatus(void_t)
{
    UTIL_ASSERT_VALUE(input, 0, FALSE);
    
    return input->isInit;
}

bool_t gmenu_input_open(struct gmenu_input_config *cfg, struct gmenu_input_param *param, bool_t popMode)
{
    input = &gInputEditorInstance;
    input->charbuffer   = (u_int8*)&gInputCharBuffer;
    input->maxlen = sizeof(gInputCharBuffer);
    input->isInit = TRUE;
    if (cfg)
    {
        memcpy(&input->cfg, cfg, sizeof(struct gmenu_input_config ));
        
        if ((0 == input->cfg.limitlen) || (input->cfg.limitlen >= input->maxlen))
        {
            input->cfg.limitlen = input->maxlen - 1;
        }

        //input->cfg.title = cfg->title;
        if (param)
        {
            gmenu_input_init(cfg->type, param->initdata, param->isfocustail);
            return TRUE;
        }
    }
    else
    {
        input->cfg.limitlen = input->maxlen - 1;
    }
    
    gmenu_input_init(GMENU_INPUT_TYPE_CHAR, 0, 0);
    if (popMode)
    {
        //ui_mmi_enter(UI_NODE_POP_INPUTBOX, 0);
    }

    return 0;
}





