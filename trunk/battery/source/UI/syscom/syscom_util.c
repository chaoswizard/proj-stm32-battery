#include "syscom_util.h"
#include "common_define.h"

u_int8 util_charsize(u_int8 *chbuf)
{
    if ((chbuf[0]>0xA0) && (chbuf[1]>0xA0))
    {
        return UTIL_MUTICHAR_LEN;
    }
    else
    {
        return 1;
    }
}

u_int8 util_charclear(u_int8 *chbuf)
{
    u_int8 size = 0;
    
    size = util_charsize(chbuf);
    if (2 == size)
    {
        ((u_int16 *)chbuf)[0] = 0;
    }
    else
    {
        chbuf[0] = 0;
    }

    return size;
}

u_int8 util_charcopy(u_int8 *destbuf, u_int8 *chbuf)
{
    u_int8 size = 0;
    
    size = util_charsize(chbuf);
    if (2 == size)
    {
        ((u_int16 *)destbuf)[0] = ((u_int16 *)chbuf)[0];
    }
    else
    {
        destbuf[0] = chbuf[0];
    }

    return size;
}

u_int16 util_strlen(u_int8 *str)
{
    return strlen(str);
}

u_int16 util_str_charsize(u_int8 *str, u_int16 pos)
{
    u_int16 idx = 0;
    
    while(UTIL_STR_NOT_NULL(str+idx) && idx < pos)
    {
        idx += util_charsize(str);
    }

    return util_charsize(str+idx);
}


u_int16 util_strsize(u_int8 *str)
{
    u_int16 len = 0;
    
    while(UTIL_STR_NOT_NULL(str+len))
    {
        len += util_charsize(str);
    }

    return len;
}

u_int8 * util_strcat(u_int8 *str, const u_int8 * srcstr)
{
    return (u_int8 *)strcat(str, srcstr);
}

u_int8 * util_strcpy(u_int8 *str, const u_int8 * srcstr)
{
     return (u_int8 *)strcpy(str, srcstr);

}


u_int16 util_str_insert_char(u_int8 *str, u_int16 pos, u_int8 *chbuf)
{
    u_int16 len, i, charsize;
    
    if (NULL == str)
        return 0;

    len = util_strlen(str);
    MY_DEBUG("char = %c ,%d\n", chbuf[0], len);
    if (0 == len)
    {
        len = util_charcopy(str, chbuf);
        str[len] = '\0';//
    }
    else if (pos < len)
    {
        charsize = util_str_charsize(str, len - 1);
        len = util_strsize(str) + util_charsize(chbuf);
        str[len] = '\0';//
        i = len - charsize;
        for(;i<pos;)
        {
            charsize = util_charcopy(str + i + charsize, str + i);
            if (i >= charsize)
                i-= charsize;
            else
                break;
        }
        util_charcopy(str+pos, chbuf);
        return pos;
    }
    MY_DEBUG("str = %s\n", str);
    return 0;
}



u_int32 util_byte_shift(u_int8 *data, u_int32 size, u_int32 startPos, u_int32 shiftNum, bool_t isLeft)
{

    if (0 == size)
        return 0;

    if (startPos > (size-1))
    {
        startPos = (size-1);
    }

    if (isLeft)
    for(;startPos<(size-1) && (shiftNum > 0);startPos++,shiftNum--)
    {
        data[startPos] = data[startPos + 1];
    }
    else
    for(;startPos<(size-1) && (shiftNum > 0);startPos++,shiftNum--)
    {
        data[startPos] = data[startPos + 1];
    }
    
    return startPos;

}

u_int32 util_bit_set_range(u_int8 start, u_int8 end)
{
    u_int8 i;
    u_int32 flg = 0;

    if (start > end)
    {
        i = end;
        end = start;
        start = end;
    }
    
    i = start;

    do{
        flg |= (1<<i);
        i++;
    } while(i<=end);

    //MY_DEBUG("BitRange: %x\n", flg);
    return flg;
}


u_int32 util_value_inc(u_int32 min, u_int32 max, u_int32 val, bool_t isInc)
{
    if (isInc)
    {
        val++;
    }
    else
    {
        if (val > min)
        {
            val --;
        }
    }
    
    if (val > max)
    {
        val= max;
    }

    return val;
}
void  util_hex_dump(u_int8 *tips, u_int8*data, u_int32 Len)
{
#define DUMP_COL 16
	u_int32 i= 0;
    MY_DEBUG("%s ",tips);
	if (NULL != data)
	{
		for (i=0;i<Len;i++)
		{
		    MY_DEBUG(" %02X",data[i]);
			if ((i+1)%DUMP_COL == 0)
			{
		       MY_DEBUG("\n");
			}
		}
	}
    MY_DEBUG("\n");
}



