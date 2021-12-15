#ifndef _OI_MACRO_H
#define _OI_MACRO_H

#include <linux/stddef.h>
#include "ENVIRONMENT"

#define DIM(x) (sizeof(x) / sizeof((x)[0]))
#define MemberToStruct(MemberPtr, StructType, MemberName) \
	((StructType *) ((char *) MemberPtr - offsetof(StructType, MemberName)))


#define CAT_TOKEN_1(t1,t2) t1##t2
#define CAT_TOKEN(t1,t2) CAT_TOKEN_1(t1,t2)


#ifndef CHECK_SIZE
#define CHECK_SIZE(type, size) extern int sizeof_##type##_is_##size [!!(sizeof(type)==(size_t)size) - 1]
#endif//CHECK_SIZE

#ifndef CHECK_OFFSET
#define CHECK_OFFSET(type, member, value) \
	    extern int offset_of_##member##_in_##type##_is_##value \
	[!!(__builtin_offsetof(type,member)==((size_t)(value))) - 1]
#endif//CHECK_OFFSET

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#elif __GNUC__ >= 4
#define __compiler_offsetof(a,b) __builtin_offsetof(a,b)
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#warning "should be __GUNC__ >= 4, this will define offset here!!!"
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif	//offsetof

#endif
