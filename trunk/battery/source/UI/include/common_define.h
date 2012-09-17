#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__



#ifndef NULL
#define NULL	((void *)0)
#endif


#define ARRAY_SIZE(table)  (sizeof(table)/sizeof((table)[0]))
#define OFFSETOF(TYPE, MEMBER) ((unsigned int)&((TYPE *)0)->MEMBER)
#define CONTAINER_OF(ptr, type, member) ((type *)(OFFSETOF(type, member) - (unsigned int)(ptr)))

//printf
#define MY_DEBUG   

#endif
