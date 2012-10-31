#include "stm32f10x.h"
#include "rtc_time.h"
#include "xprintf.h"
#include "global.h"

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])

static int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


__IO uint32_t TimeUpdate = 0;

u8 const *WEEK_STR[] = {"日", "一", "二", "三", "四", "五", "六"};

void GregorianDay(struct rtc_time * tm);


/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
static uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    tmp[index++] = xUSART1_getchar() ;
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
        if((index == 2) && (tmp[index - 1] == '\r'))
        {
            tmp[1] = tmp[0];
            tmp[0] = 0x30;
        }
        else
        {
          xprintf("\n\rPlease enter valid number between 0 and 9 -->:  ");
          index--;
        }
    }
    else
    {
        xprintf("%c", tmp[index - 1]);
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    xprintf("\n\rPlease enter valid number between 0 and %d -->:  ", value);
    return 0xFF;
  }
  return index;
}
static void Time_Regulate(struct rtc_time *tm)
{
  uint32_t DataIn = 0xFF;

  xprintf("\r\n===========www.armjishu.com==============Time Settings==================");

  xprintf("\r\n  请输入年份(Please Set Years):  20");
  while (DataIn == 0xFF)
  {
    DataIn = USART_Scanf(99);
  }
  xprintf("\n\r  年份被设置为:  20%0.2d\n\r", DataIn);
  tm->tm_year = DataIn+2000;

  DataIn = 0xFF;
  xprintf("\r\n  请输入月份(Please Set Months):  ");
  while (DataIn == 0xFF)
  {
    DataIn = USART_Scanf(12);
  }
  xprintf("\n\r  月份被设置为:  %d\n\r", DataIn);
  tm->tm_mon= DataIn;

  DataIn = 0xFF;
  xprintf("\r\n  请输入日期(Please Set Dates):  ");
  while (DataIn == 0xFF)
  {
    DataIn = USART_Scanf(31);
  }
  xprintf("\n\r  日期被设置为:  %d\n\r", DataIn);
  tm->tm_mday= DataIn;

  DataIn = 0xFF;
  xprintf("\r\n  请输入时钟(Please Set Hours):  ");
  while (DataIn == 0xFF)
  {
    DataIn = USART_Scanf(23);
  }
  xprintf("\n\r  时钟被设置为:  %d\n\r", DataIn);
  tm->tm_hour= DataIn;
    
  DataIn = 0xFF;
  xprintf("\r\n  请输入分钟(Please Set Minutes):  ");
  while (DataIn == 0xFF)
  {
    DataIn = USART_Scanf(59);
  }
  xprintf("\n\r  分钟被设置为:  %d\n\r", DataIn);
  tm->tm_min= DataIn;
  
  DataIn = 0xFF;
  xprintf("\r\n  请输入秒钟(Please Set Seconds):  ");
  while (DataIn == 0xFF)
  {
    DataIn = USART_Scanf(59);
  }
  xprintf("\n\r  秒钟被设置为:  %d\n\r", DataIn);
  tm->tm_sec= DataIn;
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
static  u32 mktimev(struct rtc_time *tm)
{
	if (0 >= (int) (tm->tm_mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		tm->tm_mon += 12;		/* Puts Feb last since it has leap day */
		tm->tm_year -= 1;
	}

	return (((
		(u32) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
			tm->tm_year*365 - 719499
	    )*24 + tm->tm_hour /* now have hours */
	  )*60 + tm->tm_min /* now have minutes */
	)*60 + tm->tm_sec; /* finally seconds */
}
void Time_Adjust(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Get time entred by the user on the hyperterminal */
  Time_Regulate(&systmtime);
  /* Get wday */
  GregorianDay(&systmtime);
  /* Change the current time */
  RTC_SetCounter(mktimev(&systmtime));
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
/*
 * This only works for the Gregorian calendar - i.e. after 1752 (in the UK)
 */
void GregorianDay(struct rtc_time * tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	lastYear=tm->tm_year-1;

	/*
	 * Number of leap corrections to apply up to end of last year
	 */
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;

	/*
	 * This year is a leap year if it is divisible by 4 except when it is
	 * divisible by 100 unless it is divisible by 400
	 *
	 * e.g. 1904 was a leap year, 1900 was not, 1996 is, and 2000 will be
	 */
	if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) {
		/*
		 * We are past Feb. 29 in a leap year
		 */
		day=1;
	} else {
		day=0;
	}

	day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday;

	tm->tm_wday=day%7;
}
void to_tm(u32 tim, struct rtc_time * tm)
{
	register u32    i;
	register long   hms, day;

	day = tim / SECDAY;
	hms = tim % SECDAY;

	/* Hours, minutes, seconds are easy */
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;

	/* Number of years in days */
	for (i = STARTOFTIME; day >= days_in_year(i); i++) {
		day -= days_in_year(i);
	}
	tm->tm_year = i;

	/* Number of months in days left */
	if (leapyear(tm->tm_year)) {
		days_in_month(FEBRUARY) = 29;
	}
	for (i = 1; day >= days_in_month(i); i++) {
		day -= days_in_month(i);
	}
	days_in_month(FEBRUARY) = 28;
	tm->tm_mon = i;

	/* Days are what is left over (+1) from all that. */
	tm->tm_mday = day + 1;

	/*
	 * Determine the day of week
	 */
	//GregorianDay(tm);
}

#if SHOW_TIME_USE_UART
/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */
static void Time_Display(uint32_t TimeVar)
{
   u8 str[15]; // 字符串暂存
   
   to_tm(TimeVar, &systmtime);

  /* 输出公历时间 */
  xprintf("\r cur time : %d年 %d月 %d日 (星期%s)  %d:%d:%d", 
                    systmtime.tm_year, systmtime.tm_mon, systmtime.tm_mday, 
                    WEEK_STR[systmtime.tm_wday], systmtime.tm_hour, 
                    systmtime.tm_min, systmtime.tm_sec);

}

/**
  * @brief  Shows the current time (HH:MM:SS) on the Hyperterminal.
  * @param  None
  * @retval None
  */   
static void Time_Show(void)
{
  xprintf("\n\r");
  /* Infinite loop */
  while (1)
  {
    /* If 1s has paased */
    if (TimeUpdate == 1)
    {
      /* Display current time */
      Time_Display(RTC_GetCounter());
      TimeUpdate = 0;
    }
  }
}

#else
void   Rtc_to_tm(u32 tim, struct rtc_time  tm)
{
    if (TimeUpdate == 1)
    {
        to_tm(tim, &tm);
        TimeUpdate = 0;
    }
}
#endif

void Rtc_time_init(void)
{
 if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

    xprintf("\r\n\n RTC not yet configured....");

    /* RTC Configuration */
    RTC_Configuration();

    xprintf("\r\n RTC configured....");

    /* Adjust time by values entred by the user on the hyperterminal */
    Time_Adjust();

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      xprintf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      xprintf("\r\n\n External Reset occurred....");
    }

    xprintf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }

  /* Clear reset flags */
  RCC_ClearFlag();
  
#if SHOW_TIME_USE_UART
  /* Display time in infinite loop */
  xprintf("Display time in infinite loop....\r\n");
  Time_Show();
 #else
 Rtc_to_tm(RTC_GetCounter(), systmtime);
  #endif
}

void systime_update(uint32_t rtc_cnt)
{     
        to_tm(rtc_cnt, &systmtime);
}

