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

//wx: add for bit opteration, below code Maybe Optimize As Tab lookup method will be more faster.
#define UTIL_BIT_TST(val, pos)     (((val)>>(pos))& 0x1)
#define UTIL_BIT_CLR(val, pos)      ((val)&(~(1<<(pos))))
#define UTIL_BIT_SET(val, pos)      ((val)|(1<<((pos))))
#define UTIL_BIT_CPL(val, pos)      ((UTIL_BIT_TST((val),(pos)))?UTIL_BIT_CLR((val), (pos)):UTIL_BIT_SET((val), (pos)))


#define UTIL_LBIT_VAL(array, pos)     (((u_int8*)array)[(pos)/8 ])
#define UTIL_LBIT_TST(array,  pos)     (UTIL_LBIT_VAL(array, pos)  &  (0x1<<((pos)%8)))
#define UTIL_LBIT_CLR(array,  pos)     (UTIL_LBIT_VAL(array, pos) &  (~(0x1<<((pos)%8))))
#define UTIL_LBIT_SET(array, pos)       (UTIL_LBIT_VAL(array, pos) |  (0x1<<((pos)%8)))
#define UTIL_LBIT_CPL(array, pos)      ((UTIL_LBIT_TST((array),(pos)))?UTIL_LBIT_CLR((array), (pos)):UTIL_LBIT_SET((array), (pos)))



#define UTIL_BIT_TST_RANGE(val, idxStart, idxEnd)    (util_bit32_1num((val)& util_bit32_set_range((idxStart), (idxEnd))))
#define UTIL_BIT_CLR_RANGE(val, idxStart, idxEnd)    ((val)&(~ util_bit32_set_range((idxStart), (idxEnd))))
#define UTIL_BIT_SET_RANGE(val, idxStart, idxEnd)    ((val)|util_bit32_set_range((idxStart), (idxEnd)))



//wx: add for val opteration
#define UTIL_VAL_SWAP(a,b)          (a)=(a)^(b);(b)=(b)^(a);(a)=(a)^(b)     
#define UTIL_VAL_MAX(a,b)           (a)>(b)?((a)):(b)   
#define UTIL_VAL_MIN(a,b)           (a)<(b)?((a)):(b)   
#define UTIL_VAL_CMP_SWAP(min,max)  (UTIL_VAL_MIN((min),(max)) != (min) )?(UTIL_VAL_SWAP((min),(max))):(;) 
#define UTIL_VAL_SUB(_val_, _v_, _type_)    {\
    _type_  _tmp##_v_ = (_v_);\
    while((_val_) && (_tmp##_v_)){(_val_)--; _tmp##_v_--;}\
}


//wx: add for char encode for compiler system environment configure.
#define UTIL_STR_MBCS_LEN            (2)
#define UTIL_STR_NOT_NULL(strbuf)   (((u_int8*)(strbuf))[0] != '\0')

//wx: add for assert .
#define UTIL_ASSERT_VALUE(_bool_, _msg_, _retval_)  if (!(_bool_)) { return (_retval_); }
#define UTIL_ASSERT_VOID(_bool_, _msg_)  if (!(_bool_)) { return;}


//追加模式
#define UTIL_STR_ADD_APPEND   (0x0)
// 插入模式
#define UTIL_STR_ADD_INSERT   (0x1)
//替换模式
#define UTIL_STR_ADD_REPLACE  (0x2)
//----------------------------------------------------------------------
u_int16 util_strlen(p_cstr str);
u_int16 util_strsize(p_cstr str);
p_str  util_strcat(p_str str, p_cstr  srcstr);
p_str  util_strcpy(p_str str, p_cstr  srcstr);
p_str  util_strncpy(p_str dest, p_cstr src, s_int32 length);
u_int16 util_str_charlen(p_cstr str, u_int16 pos);
u_int16 util_str_offset(p_cstr str, u_int16 pos);
u_int16 util_strlen(p_cstr str);
u_int16 util_str_charcount(p_cstr str);
u_int16 util_str_remove_char(p_str str, u_int16 pos);
u_int16 util_str_addchar(u_int8 *str, u_int16 maxlen, u_int16 *pos, const u_int8 *chbuf, bool_t addmode);
u_int16 util_str_delchar(u_int8 *str, u_int16 maxlen, u_int16 *pos, bool_t isbefore);
u_int8 util_charclear(p_str chbuf);
u_int8 util_charlen(p_cstr chbuf);
u_int8 util_charcopy(p_str destbuf, p_cstr chbuf);

//----------------------------------------------------------------------
u_int32 util_value_inc(u_int32 min, u_int32 max, u_int32 val, bool_t isInc);
//----------------------------------------------------------------------
u_int32 util_byte_shift(p_str data, u_int32 size, u_int32 shiftNum, bool_t isLeft);
void util_hex_dump(p_str tips, p_str data, u_int32 len);
//----------------------------------------------------------------------
u_int16 util_bit16_reverse_order(u_int16 val);
u_int32 util_bit32_reverse_order(u_int32 val);
u_int32 util_bit32_set_range(u_int8 start, u_int8 end);
u_int8 util_bit32_1num(u_int32 val);
u_int8 util_bit8_1num(u_int8 val);

void  util_bit32_dump(p_str tips, u_int32 val);
u_int8 util_array_check(u_int8 *array, u_int32 bitTotal);

//----------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif


