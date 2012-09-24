#include "gui_osd_obj.h"


void gui_osd_obj_clear(POSD_OBJ_HEAD obj)
{
    switch (obj->type)
    {
        case OSD_OBJ_RECT:
            Screen_PrintClear(&((struct OSD_OBJ_RECT_INFO *)obj)->rect.zone);
            break;
        case OSD_OBJ_CONTENT:
            Screen_PrintClear(&((struct OSD_OBJ_CONTENT_INFO *)obj)->rect.zone);
            break;
        case OSD_OBJ_BUTTON:
            Screen_PrintClear(&((struct OSD_OBJ_BUTTON_INFO *)obj)->rect.zone);
            break;
        default:
            break;
    }
}


void gui_osd_obj_draw(POSD_OBJ_HEAD obj, enum T_UICOM_STATUS drawType)
{
    switch (obj->type)
    {
        case OSD_OBJ_RECT:
            gui_osd_rect_draw((struct OSD_OBJ_RECT_INFO *)obj, drawType);
            break;
        case OSD_OBJ_CONTENT:
            gui_osd_content_draw((struct OSD_OBJ_CONTENT_INFO *)obj, drawType);
            break;
        case OSD_OBJ_BUTTON:
            gui_osd_button_draw((struct OSD_OBJ_BUTTON_INFO *)obj, drawType);
            break;
        default:
            break;
    }
}


void gui_osd_rect_draw(struct OSD_OBJ_RECT_INFO *rect, enum T_UICOM_STATUS drawType)
{
    if (OSD_OBJ_RECT != rect->type)
    {
        return;
    }
    Screen_PrintRect(&rect->rect, PIXEL_MODE_SET);
}


void gui_osd_data_draw(struct UICOM_DATA *data, struct SCREEN_ZONE *zone)
{
    if (UICOM_DATA_TYPE_TEXT == data->type)
    {
        Screen_PrintString(zone, (data->data), data->attr);
    }
    else if (UICOM_DATA_TYPE_PICTURE == data->type)
    {
        Screen_PrintBmp(zone, (u_int8 *)data->data, data->attr);
    }
}


void gui_osd_content_draw(struct OSD_OBJ_CONTENT_INFO *content, enum T_UICOM_STATUS drawType)
{
    if (OSD_OBJ_CONTENT != content->type)
    {
        return;
    }
    
    Screen_PrintRect(&content->rect, PIXEL_MODE_SET);
    gui_osd_data_draw(&content->content, &content->rect.zone);
}

void gui_osd_button_draw(struct OSD_OBJ_BUTTON_INFO *button, enum T_UICOM_STATUS drawType)
{
    if (OSD_OBJ_BUTTON != button->type)
    {
        return;
    }
    Screen_PrintFillRect(&button->rect.zone, PIXEL_MODE_SET);
    gui_osd_data_draw(&button->content, &button->rect.zone);
}




