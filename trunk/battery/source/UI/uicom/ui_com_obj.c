#include "ui_com_obj.h"
#include "common_define.h"



/*
* AN OBJ  SET WHICH ALL IS SAME STRUCT
*/
void uicom_obj_list(T_UICOM_COUNT len, void *param, 
                          void (*paint)(T_UICOM_COUNT pos, void *param), 
                          T_UICOM_ORDER paintOrder)
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
                          T_UICOM_ORDER paintOrder)
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
                            T_UICOM_ORDER paintOrder)
{
    T_UICOM_COUNT  key;

    key=start;

    do
    {
      (paint)(key,  param);
    } while((getNext)(&key,  param));
}

//==========================================================

u_int16 uicom_obj_input(T_INPUT_EVENT evt, u_int8 *data, u_int8 *pfocus,u_int8 maxnum, void *param)
{
    u_int16 charcnt;

    //MY_DEBUG("EVT %d: Str=%s,%d\n", evt, data, pfocus);
    if ((0 == maxnum) || (NULL == pfocus))
        return 0;

    switch (INPUT_PROC_TYPE(evt))
    {
        case INPUT_PROC_NEW:
            charcnt = util_str_addchar(data, maxnum, (u_int16*)pfocus, param, INPUT_PROC_SUBTYPE(evt));
            break;
        case INPUT_PROC_UPDATE:
            charcnt = util_strsize(data);
            if (0 == charcnt)
            {
                (*pfocus) = 0;
            }
            else if ((*pfocus) >= charcnt)
            {
                (*pfocus) = (charcnt-1);
            }
            break;
        case INPUT_PROC_MOVE:
            charcnt = util_strsize(data);
            if (charcnt)
            {
            switch (INPUT_PROC_SUBTYPE(evt))
            {
                case 0://back
                    (*pfocus) = (u_int8)util_value_inc(0, charcnt-1, (*pfocus), 1);
                    break;
                case 1://front:
                    (*pfocus) =  util_value_inc(0, charcnt-1, (*pfocus), 0);
                    break;
                case 2://head:
                    (*pfocus) = 0;
                    break;
                case 3://tail
                    (*pfocus) = charcnt - 1;
                    break;
                default:
                    break;
            }
            }
            else
            {
                (*pfocus) = 0;
            }
            break;
        case INPUT_PROC_DEL:
            switch (INPUT_PROC_SUBTYPE(evt))
            {
                case 0://back
                    charcnt = util_str_delchar(data, maxnum, (u_int16*)pfocus, 0);
                    break;
                case 1://front:
                    charcnt = util_str_delchar(data, maxnum, (u_int16*)pfocus, 1);
                    break;
                default:
                    charcnt = util_strsize(data);
                    break;
            }
            break;
        default:
        break;
    }

    return charcnt;
}

