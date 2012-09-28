#include "ui_com_obj.h"
#include "common_define.h"



/*
* AN OBJ  SET WHICH ALL IS SAME STRUCT
*/
void uicom_obj_list(T_UICOM_COUNT len, void *param, 
                          void (*paint)(T_UICOM_COUNT pos, void *param), 
                          T_UICOM_ORDER_FLAG paintOrder)
{
    T_UICOM_COUNT idx;
    
    if (paint)
    for (idx=0; idx<len; idx++)
    {
        (paint)(idx, param);
    }
}


/*
* AN OBJ  SET WHICH ALL IS A LIST STRUCT
*/
void uicom_obj_tab(T_UICOM_COUNT row, T_UICOM_COUNT col, T_UICOM_HANDL param, 
                          void (*paint)(T_UICOM_COUNT, T_UICOM_COUNT, T_UICOM_HANDL), 
                          T_UICOM_ORDER_FLAG paintOrder)
{
    T_UICOM_COUNT rowIdx, colIdx;
    
    if (paint)
    for (rowIdx=0; rowIdx<row; rowIdx++)
    {
        for (colIdx=0; colIdx<col; colIdx++)
        {
            (paint)(rowIdx, colIdx, param);
        }
    }
}

/*
* AN OBJ  SET WHICH ALL IS A  TREE 1Lvl MAP STRUCT
*/
void uicom_obj_map(T_UICOM_COUNT start, T_UICOM_HANDL param, 
                            T_UICOM_COUNT (*getNext)(T_UICOM_COUNT *idx, T_UICOM_HANDL param), 
                            void (*paint)(T_UICOM_COUNT idx, T_UICOM_HANDL param),
                            T_UICOM_ORDER_FLAG paintOrder)
{
    T_UICOM_COUNT  key;

    key=start;

    do
    {
      (paint)(key,  param);
    } while((getNext)(&key,  param));
}

//==========================================================

u_int8 uicom_inputbox_proc(enum INPUTBOX_PROC_TYPE evt, u_int8 *data, u_int8 focus, void *param)
{
    u_int8 tmp;

    //MY_DEBUG("char = %d, %s, %d, %x\n", evt, data, focus, param);
    switch (evt)
    {
        case INPUTBOX_PROC_NEWCHAR_RMODE:
        case INPUTBOX_PROC_NEWCHAR_IMODE:
            if (param)
            return (u_int8)util_str_insert_char(data, focus, param);
            break;
        case INPUTBOX_PROC_MOVE_BACK:
            tmp = util_strlen(data);
            if (tmp)
            return (u_int8)util_value_inc(0, tmp-1, focus, 1);
            break;
        case INPUTBOX_PROC_MOVE_FRONT:
            tmp = util_strlen(data);
            if (tmp)
            return (u_int8)util_value_inc(0, tmp-1, focus, 0);
        case INPUTBOX_PROC_DEL_BACK:
        case INPUTBOX_PROC_DEL_FRONT:
        break;
    }

    return 0;
}

