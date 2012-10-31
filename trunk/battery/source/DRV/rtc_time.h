#ifndef RTC_TIME_H
#define RTC_TIME_H
#include "stm32f10x.h"
struct rtc_time {
	uint8_t tm_sec;
	uint8_t tm_min;
	uint8_t tm_hour;
	uint8_t tm_mday;
	uint8_t tm_mon;
	uint8_t tm_wday;
	uint16_t tm_year;	
};

extern __IO uint32_t TimeUpdate;

extern void Rtc_time_init(void); 
extern void systime_update(uint32_t rtc_cnt);
#endif//RTC_TIME_H
