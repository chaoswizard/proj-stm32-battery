#ifndef __COMMON_DEFINE_H__
#define __COMMON_DEFINE_H__


#define ARRAY_SIZE(table)  (sizeof(table)/sizeof((table)[0]))
#define OFFSETOF(TYPE, MEMBER) ((unsigned int)&((TYPE *)0)->MEMBER)
#define CONTAINER_OF(ptr, type, member) ((type *)(OFFSETOF(type, member) - (unsigned int)(ptr)))

#define MY_DEBUG    


#endif
