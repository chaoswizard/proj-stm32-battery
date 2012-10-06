#include "syscom_util.h"
#include "common_define.h"

#define UTIL_STR_SIZE_MAX    (64)

#define UTIL_ALIGNBASE	(sizeof(u_int32))
#define UTIL_ALIGNED(x)	((u_int32)(x) & (sizeof(u_int32)-1))
#define UTIL_ISNULL(x)	(((x)-0x01010101) & ~(x) & 0x80808080)

u_int8 util_charlen(p_cstr chbuf)
{
    if ((chbuf[0]>0xA0) && (chbuf[1]>0xA0))
    {
        return UTIL_STR_MBCS_LEN;
    }
    else
    {
        return 1;
    }
}

u_int8 util_charclear(p_str chbuf)
{
    u_int8 size = 0;
    
    size = util_charlen(chbuf);
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

u_int8 util_charcopy(p_str destbuf, p_cstr chbuf)
{
    u_int8 size = 0;
    
    size = util_charlen(chbuf);
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


u_int16 util_strsize(p_cstr str)
{
    u_int16 size = 0, len = 0;
    
    while(UTIL_STR_NOT_NULL(str+size) && (size < UTIL_STR_SIZE_MAX))
    {
        size += util_charlen(str);
        len++;
    }

    return len;
}


u_int16 util_str_charlen(p_cstr str, u_int16 pos)
{
    u_int16 idx = 0, chrnum = 0;
    
    while(UTIL_STR_NOT_NULL(str+idx) && (chrnum < pos) && (idx < UTIL_STR_SIZE_MAX))
    {
        idx += util_charlen(str);
        chrnum++;
    }

    return util_charlen(str+idx);
}

u_int16 util_str_offset(p_cstr str, u_int16 pos)
{
    u_int16 idx = 0, chrnum = 0;
    
    while(UTIL_STR_NOT_NULL(str+idx) && (chrnum < pos) && (idx < UTIL_STR_SIZE_MAX))
    {
        idx += util_charlen(str);
        chrnum++;
    }

    return idx;
}

u_int16 util_strlen(p_cstr str)
#if 0
{
    u_int16 len = 0;
    
    if (str)
    while(UTIL_STR_NOT_NULL(str+len) && (len < UTIL_STR_SIZE_MAX))
    {
        len += util_charlen(str);
    }

    return len;
}
#else
{
	p_str ptr0 = (p_str)str;

	if(!UTIL_ALIGNED(str))
	{
		u_int32	*align = (u_int32*)str;
		while(!UTIL_ISNULL(*align))
			align ++;
		str = (p_str )align;
	}

	while(*str)
		str ++;

	return str-ptr0;
}

#endif
p_str  util_strcat(p_str dest, p_cstr  src)
{
    p_str ptr0 = dest;
    
    UTIL_ASSERT_VALUE(dest&&src, 0, NULL);

    dest += util_strlen(dest);
    util_strcpy(dest, src);

    return ptr0;
}


p_str util_strcpy(p_str dest, p_cstr  src)
{
	p_str  ptr0 = dest;
    
    UTIL_ASSERT_VALUE(dest&&src, 0, NULL);

	if(!UTIL_ALIGNED(dest) && !UTIL_ALIGNED(src))
	{
		u_int32	*align1 = (u_int32*)dest;
		u_int32	*align2 = (u_int32*)src;

		while(!UTIL_ISNULL(*align2))
			*align1++ = *align2++;

		dest = (p_str )align1;
		src  = (p_str )align2;
	}

	while((bool_t)(*dest++ = *src++));

	return ptr0;
}


p_str  util_strncpy(p_str dest, p_cstr src, s_int32 length)
{
	p_str ptr0 = dest;

    UTIL_ASSERT_VALUE(dest&&src, 0, NULL);

    if((length>=UTIL_ALIGNBASE) && !UTIL_ALIGNED(dest) && !UTIL_ALIGNED(src))
	{
		u_int32	*align1 = (u_int32*)dest;
		u_int32	*align2 = (u_int32*)src;

		while(!UTIL_ISNULL(*align2) && length>=UTIL_ALIGNBASE)
		{
			*align1++ = *align2++;	
			length -= UTIL_ALIGNBASE;
		}

		dest = (p_str)align1;
		src  = (p_str)align2;		
	}

	while(length-- > 0)
	{
		if(!(*dest++ = *src++))
			break;
	}

	*dest++ = 0;

	return ptr0;
}

/*
* Author: WangXi
 * Default Mode: 如果为空，则直接填入第一位，光标置为起始
 *-----------------------------------------------------------------------
 * Insert Mode:在当前位置插入一个字符，如果超过最大长度，
 * 则尾部自动丢弃，使之顺利插入，光标不移动
 *-----------------------------------------------------------------------
 * Append Mode:如果有可用空间(剩余大于等于1)，且位置为
 * 字符串尾部，则插入新字符，光标后移；否则(Replace Mode)
 * 替换当前 位置，光标如果可移动(替换位置不在尾部)，
 * 则后移,否则 不移动
*/
u_int16 util_str_addchar(u_int8 *str, u_int16 maxlen, u_int16 *ppos, const u_int8 *chbuf, u_int8 addmode)
{
    u_int16 charnum, newlen, oldlen, oldcharlen, newcharlen, pos;

    UTIL_ASSERT_VALUE(str&&chbuf&&ppos, 0, 0);
    
    if (0 == maxlen)
    {
        (*ppos) = 0;
        return 0;
    }

    // default proc.
    pos = (*ppos);
    charnum = util_strsize(str);
    if (0 == charnum)// if empty, insert and rewind
    {
        oldcharlen = util_charcopy(str, chbuf);
        str[oldcharlen] = '\0';
        (*ppos) = 0;
        return 1;//must ensure the chbuf is valid char
    }
    //-----------------------------------------------------------------
    newcharlen = util_charlen(chbuf);
    oldcharlen = util_str_charlen(str, pos);
    oldlen = util_strlen(str);
    //-----------------------------------------------------------------
    if (UTIL_STR_ADD_INSERT == addmode) // insert to current position
    {
       newlen = util_str_offset(str, pos);//offset of insert char pos in string buffer
       //move front char to tail position.
       util_byte_shift(str + newlen, oldlen + 1 - newlen, newcharlen, 1);
       util_charcopy(str + newlen, chbuf);
       //if old string has full, drop last char for new insert char
       if (charnum == maxlen)
       {
           util_charclear(str + oldlen + newcharlen - oldcharlen);//Drop Tail directly
           //MY_DEBUG("INS_D: [%s], %d %d %d\n", str, newlen, oldlen, newcharlen);
           //util_hex_dump("INS_D:", str, 6);
       }
       else
       {
            charnum ++;
       }
       (*ppos) = pos;
       
       return charnum;
    }
    else
    {
        if ((charnum < maxlen) && (pos == (charnum - 1)) && (UTIL_STR_ADD_APPEND == addmode))
        {
            util_charcopy(str + oldlen, chbuf);
            util_charclear(str + oldlen + newcharlen);//tail move more back
            (*ppos) = (pos + 1);
            return (charnum + 1);
        }
        else// UTIL_STR_ADD_REPLACE
        {
            newlen = util_str_offset(str, pos + 1);

            // if replace pos's char len is not equal new char len, move front char to back some byte OR reverse
            if (oldcharlen < newcharlen)// low --> high
            {
                util_byte_shift(str + newlen, oldlen + 1 - newlen, newcharlen - oldcharlen, 1);
            }
            else if(oldcharlen > newcharlen)// hight --> low
            {
                util_byte_shift(str + newlen, oldlen  + 1 - newlen, oldcharlen - newcharlen, 0);
            }
            else //(oldcharlen == newcharlen)
            {
                // replace directly
            }
            util_charcopy(str + newlen - oldcharlen, chbuf);
            if (pos < (charnum - 1))
            {
                pos += 1;
            }
            
            (*ppos) = pos;
            
            return charnum;
        }
    }

    return charnum;
}

/*
* Author: WangXi
* 删除当前位置，如果前向移动，则光标在允许下向前移动
* 如果向后移动，光标在允许范围(删除位置不在尾部)不动，
* 否则向前移动
*/
u_int16 util_str_delchar(u_int8 *str, u_int16 maxlen, u_int16 *ppos, bool_t isbefore)
{
    u_int16 charnum, newlen, pos, charlen;

    UTIL_ASSERT_VALUE(str&&ppos, 0, 0);

    pos = (*ppos);
    charnum = util_strsize(str);
    if (charnum < 2)// if 0-1 char, clear and rewind
    {
        (*ppos) = 0;
        util_charclear(str);//Drop 
        return 0;
    }
    
    else if (pos < charnum)
    {
        if (pos == (charnum - 1))// if tail
        {
            util_charclear(str + util_str_offset(str ,pos));//Drop Tail directly
        }
        else
        {
            newlen = util_str_offset(str, pos);//Replace pos With after next char
            charlen = util_charlen(str+pos);
            // tail --> front :Tail Char Replace Front Char, until  postion 'pos'
            util_byte_shift(str + newlen, util_strlen(str) + 1 - charlen - newlen,/*data count of before pos(include '\0') */
                            charlen, 0);
        }
        if (pos > 0)
        {
            if (isbefore || (pos == (charnum - 1)))
            {
                pos -= 1;
            }
        }
    }

   (*ppos) = pos;
   
   return (charnum-1); 
}



u_int32 util_byte_shift(p_str data, u_int32 size, u_int32 shiftNum, bool_t isLeft)
{
    s_int32 startPos;

    if (0 == size)
        return 0;

    if (isLeft) //[3][2][1][0]<<<<<[2][1][0][X]
    {
        while(size && shiftNum--)
        {
            if (size > 1)
            {
                for(startPos = (size-1);startPos>0;startPos--)
                {
                     data[startPos] = data[startPos-1];
                }
            }
            //data[0] = 0;
            size--;
        }
    }
    else // [3][2][1][0]>>>>[X][3][2][1]
    {
        while(size && shiftNum--)
        {
            startPos = 0;
            //MY_DEBUG("SHIFT:%d, %d, %d\n", size, startPos, shiftNum);
            //util_hex_dump("SHIFT A:", data, 6);
            if (size > 1)
            {
                for(;startPos < size;startPos++)
                {
                     data[startPos] =  data[startPos + 1];
                }
            }
            size--;
            //data[startPos] = 0;
        }
    }
    
    return startPos;

}



u_int32 util_bit32_set_range(u_int8 start, u_int8 end)
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

    //MY_DEBUG("BitRange: [%d-%d]%x\n",start, end, flg);
    return flg;
}


/**
交换位的顺序
算法实现：
第一行代码为奇偶位相互交换, 
第二行为以两位为一单元，奇偶单元进行交换；
第三行为以四位为一单元，奇偶单元进行交换；
第四行为以八位为一单元，奇偶单元进行交换；
最后一行为以十六位为一单元，奇偶单元进行交换。
至此，32位移位完成，算法结束。
*/
u_int32 util_bit32_reverse_order(u_int32 val)
{
    val = ((val >> 1)  & 0x55555555) | ((val << 1)  & 0xaaaaaaaa);
    val = ((val >> 2)  & 0x33333333) | ((val << 2)  & 0xcccccccc);
    val = ((val >> 4)  & 0x0f0f0f0f) | ((val << 4)  & 0xf0f0f0f0);
    val = ((val >> 8)  & 0x00ff00ff) | ((val << 8)  & 0xff00ff00);
    val = ((val >> 16) & 0x0000ffff) | ((val << 16) & 0xffff0000);

    return val;
}


u_int16 util_bit16_reverse_order(u_int16 val)
{
	val = ((val >> 1) & 0x5555) | ((val << 1) & 0xaaaa);
	val = ((val >> 2) & 0x3333) | ((val << 2) & 0xcccc);
	val = ((val >> 4) & 0x0f0f) | ((val << 4) & 0xf0f0);
	val = ((val >> 8) & 0x00ff) | ((val << 8) & 0xff00);
    val = ((val >> 16) & 0x0000ffff) | ((val << 16) & 0xffff0000);
    
	return val;
}


u_int8 util_bit32_1num(u_int32 val)
{
    int i;
    int len = sizeof(u_int32) << 3;
    
    for(i = len-1; i >= 0; i--)
    {
        if(val>>i)//add wx:Maybe can use  n&(n-1) more faster?
        {
            //MY_DEBUG("Bit1Num: %x, Count=%d\n",val,i);
            return (i+1);
        }
    }
    
    return 0;
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


#define DUMP_BIT32_COL 8

void  util_bit32_dump(p_str tips, u_int32 val)
{
    s_int32  i;
    
    MY_DEBUG("0x%X, %s",val, tips);
    for(i = 31; i >= 0; i--)
    {
        if ((val&((1<<(i+1))-1)) >> i)
        {
            MY_DEBUG(" 1");
        }
        else
        {
            MY_DEBUG(" 0");
        }
        
        if(i%DUMP_BIT32_COL == 0)
        {
            MY_DEBUG("\n");
        }
    }
}


#define DUMP_HEX_COL  16
void  util_hex_dump(p_str tips, p_str data, u_int32 len)
{
	s_int32 i= 0;
    MY_DEBUG("0x%X[%d] %s ", data, len, tips);
	if (NULL != data)
	{
		for (i=0;i<len;i++)
		{
		    MY_DEBUG(" %02X",data[i]);
			if ((i+1)%DUMP_HEX_COL == 0)
			{
		       MY_DEBUG("\n");
			}
		}
	}
    MY_DEBUG("\n");
}



