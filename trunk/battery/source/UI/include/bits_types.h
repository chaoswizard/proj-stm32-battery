#ifndef __BITS_TYPES_H__
#define __BITS_TYPES_H__

typedef	unsigned char           u_int8;		/* unsigned 8 bit integer */
typedef	signed   char			s_int8;		/* signed 8 bit integer */
typedef	unsigned short			u_int16;		/* unsigned 16 bit integer */
typedef	unsigned int			u_int32;		/* unsigned 32 bit integer */
typedef	unsigned int			u_int;		/* unsigned 32 bit integer */

#ifdef OS_LINUX
typedef	unsigned long long		u_int64;		/* unsigned 64 bit integer */
#else
typedef unsigned __int64 		u_int64;		/* unsigned 64 bit integer */
#endif
typedef	signed short			s_int16;		/* signed 16 bit integer */
typedef	signed int 			    s_int32;		/* signed 32 bit integer */
typedef u_int8                  bool_t;

#ifndef TRUE
#define TRUE	                (1)
#endif

#ifndef FALSE
#define FALSE	                (0)
#endif

typedef void                    void_t;
typedef void*                   handle_t;
typedef void*                   p_void;

#ifndef NULL
#define NULL	((void *)0)
#endif



typedef	unsigned   char*		p_str;		
typedef	const unsigned   char*  p_cstr;		

#endif


