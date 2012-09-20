#include "ui_com_obj.h"



void uicom_obj_list(T_UICOM_OBJ_COUNT len, void *param, 
                          void (*paint)(T_UICOM_OBJ_COUNT pos, void *param), 
                          u_int8 paintOrder)
{
    T_UICOM_OBJ_COUNT idx;
    
    if (paint)
    for (idx=0; idx<len; idx++)
    {
        (paint)(idx, param);
    }
}

void uicom_obj_tab(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, void *param, 
                          void (*paint)(T_UICOM_OBJ_COUNT row, T_UICOM_OBJ_COUNT col, void *param), 
                          u_int8 paintOrder)
{
    T_UICOM_OBJ_COUNT rowIdx, colIdx;
    
    if (paint)
    for (rowIdx=0; rowIdx<row; rowIdx++)
    {
        for (colIdx=0; colIdx<col; colIdx++)
        {
            (paint)(rowIdx, colIdx, param);
        }
    }
}


