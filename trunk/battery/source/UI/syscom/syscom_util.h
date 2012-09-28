/***********************************************************************
 *
 * MODULE NAME:  
 * DESCRIPTION:  sys com常用的系统功能实现
 * AUTHOR:     
 * DATE: 2012/09/27
 *
 ***********************************************************************/
#ifndef __SYS_COM_UTIL_H__
#define __SYS_COM_UTIL_H__
#include "bits_types.h"
#include "syscom_util.h"


#ifdef __cplusplus
extern "C" {
#endif

#define UTIL_MUTICHAR_LEN          (2)
#define UTIL_STR_NOT_NULL(strbuf)   (((u_int8*)(strbuf))[0] != '\0')

u_int8 * util_strcat(u_int8 *str, const u_int8 * srcstr);
u_int8 * util_strcpy(u_int8 *str, const u_int8 * srcstr);
u_int16 util_strsize(u_int8 *str);
u_int8 util_charclear(u_int8 *chbuf);
u_int8 util_charsize(u_int8 *chbuf);
u_int8 util_charcopy(u_int8 *destbuf, u_int8 *chbuf);
u_int16 util_strlen(u_int8 *str);
u_int16 util_str_insert_char(u_int8 *str, u_int16 pos, u_int8 *chbuf);
u_int16 util_str_remove_char(u_int8 *str, u_int16 pos);
u_int32 util_value_inc(u_int32 min, u_int32 max, u_int32 val, bool_t isInc);
u_int32 util_bit_set_range(u_int8 start, u_int8 end);
u_int32 util_byte_shift(u_int8 *data, u_int32 size, u_int32 startPos, u_int32 shiftNum, bool_t isLeft);
void util_hex_dump(u_int8 *tips, u_int8*data, u_int32 Len);

#ifdef __cplusplus
}
#endif

#endif


